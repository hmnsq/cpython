
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pystate.h"       // _PyThreadState_GET()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_signal.h"        // _PyOS_SigintEvent()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MS_WINDOWS
#  ifndef WIN32_LEAN_AND_ਔਸਤ
#    define WIN32_LEAN_AND_ਔਸਤ
#  endif
#  include "windows.h"
#ਜੇ_ਅੰਤ /* MS_WINDOWS */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#ਜੇ_ਅੰਤ


// Export the symbol since it's used by the readline shared extension
Py_API_ਅੰਕੜੇ(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = ਨਿਰਾ;

ਸਥਿਰ PyMutex Py_OS_Readline_ਤਾਲਾ;

ਪੂਰਨ (*Py_OS_ਨਿਵੇਸ਼_Hook)(ਖਾਲੀ) = ਨਿਰਾ;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ਸਥਿਰ ਪੂਰਨ
my_fgets(PyThreadState* tstate, ਅੱਖਰ *buf, ਪੂਰਨ len, FILE *fp)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    Py_ਸਿਰਾ_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    ਜੇ (handle == INVALID_HANDLE_ਮੁੱਲ) {
        ਵਾਪਸ -1; /* EOF */
    }
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (1) {
        ਜੇ (Py_OS_ਨਿਵੇਸ਼_Hook != ਨਿਰਾ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ਖਾਲੀ)(Py_OS_ਨਿਵੇਸ਼_Hook)();
        }

        errno = 0;
        clearerr(fp);
        ਅੱਖਰ *p = fgets(buf, len, fp);
        ਜੇ (p != ਨਿਰਾ) {
            ਵਾਪਸ 0; /* No error */
        }
        ਪੂਰਨ err = errno;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MS_WINDOWS
        /* Ctrl-C anywhere on the line or Ctrl-Z if the only character
           on a line will set ERROR_OPERATION_ABORTED. Under normal
           circumstances Ctrl-C will also have caused the SIGINT handler
           to fire which will have set the event object returned by
           _PyOS_SigintEvent. This signal fires in another thread and
           is not guaranteed to have occurred before this point in the
           code.

           Therefore: check whether the event is set with a small timeout.
           If it is, assume this is a Ctrl-C and reset the event. If it
           isn't set assume that this is a Ctrl-Z on its own and drop
           through to check for EOF.
        */
        ਜੇ (Get_ਆਖਰੀ_Error()==ERROR_OPERATION_ABORTED) {
            HANDLE h_ਰੁਕਾਵਟ_Event = _PyOS_SigintEvent();
            ਚੋਣ (WaitForSingleObjectEx(h_ਰੁਕਾਵਟ_Event, 10, FALSE)) {
            ਹਾਲਤ WAIT_OBJECT_0:
                ResetEvent(h_ਰੁਕਾਵਟ_Event);
                ਵਾਪਸ 1; /*ਰੁਕਾਵਟ*/
            ਹਾਲਤ WAIT_FAILED:
                ਵਾਪਸ -2; /* Error */
            }
        }
#ਜੇ_ਅੰਤ /* MS_WINDOWS */

        ਜੇ (feof(fp)) {
            clearerr(fp);
            ਵਾਪਸ -1; /* EOF */
        }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EINTR
        ਜੇ (err == EINTR) {
            PyEval_RestoreThread(tstate);
            ਪੂਰਨ s = Py_Err_Check_ਸੰਕੇਤ();
            PyEval_SaveThread();

            ਜੇ (s < 0) {
                ਵਾਪਸ 1;
            }
            /* try again */
            ਜਾਰੀ;
        }
#ਜੇ_ਅੰਤ

        ਜੇ (Py_OS_ਰੁਕਾਵਟ_Occurred(tstate)) {
            ਵਾਪਸ 1; /*ਰੁਕਾਵਟ*/
        }
        ਵਾਪਸ -2; /* Error */
    }
    /* NOTREACHED */
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

ਬਾਹਰੀ ਅੱਖਰ _get_console_type(HANDLE handle);

ਅੱਖਰ *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE h_Std_ਵਿੱਚ)
{
    ਸਥਿਰ wchar_t wbuf_local[1024 * 16];
    ਅਚਲ DWORD chunk_ਮਾਪ = 1024;

    DWORD n_read, ਕੁੱਲ_read, wbuflen, u8len;
    wchar_t *wbuf;
    ਅੱਖਰ *buf = ਨਿਰਾ;
    ਪੂਰਨ err = 0;

    n_read = (DWORD)-1;
    ਕੁੱਲ_read = 0;
    wbuf = wbuf_local;
    wbuflen = ਆਕਾਰ(wbuf_local) / ਆਕਾਰ(wbuf_local[0]) - 1;
    ਜਦੋਂ (1) {
        ਜੇ (Py_OS_ਨਿਵੇਸ਼_Hook != ਨਿਰਾ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ਖਾਲੀ)(Py_OS_ਨਿਵੇਸ਼_Hook)();
        }
        ਜੇ (!ReadConsoleW(h_Std_ਵਿੱਚ, &wbuf[ਕੁੱਲ_read], wbuflen - ਕੁੱਲ_read, &n_read, ਨਿਰਾ)) {
            err = Get_ਆਖਰੀ_Error();
            ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
        }
        ਜੇ (n_read == (DWORD)-1 && (err = Get_ਆਖਰੀ_Error()) == ERROR_OPERATION_ABORTED) {
            ਰੋਕੋ;
        }
        ਜੇ (n_read == 0) {
            ਪੂਰਨ s;
            err = Get_ਆਖਰੀ_Error();
            ਜੇ (err != ERROR_OPERATION_ABORTED)
                ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
            err = 0;
            HANDLE h_ਰੁਕਾਵਟ_Event = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(h_ਰੁਕਾਵਟ_Event, 100, FALSE);
            ਜੇ (state == WAIT_OBJECT_0) {
                ResetEvent(h_ਰੁਕਾਵਟ_Event);
                PyEval_RestoreThread(tstate);
                s = Py_Err_Check_ਸੰਕੇਤ();
                PyEval_SaveThread();
                ਜੇ (s < 0) {
                    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
                }
            }
            ਨਹੀਂਤਾਂ ਜੇ (state != WAIT_TIMEOUT) {
                err = Get_ਆਖਰੀ_Error();
                ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            ਜਾਰੀ;
        }

        ਕੁੱਲ_read += n_read;
        ਜੇ (ਕੁੱਲ_read == 0 || wbuf[ਕੁੱਲ_read - 1] == L'\n') {
            ਰੋਕੋ;
        }
        wbuflen += chunk_ਮਾਪ;
        ਜੇ (wbuf == wbuf_local) {
            wbuf[ਕੁੱਲ_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * ਆਕਾਰ(wchar_t));
            ਜੇ (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            ਨਹੀਂਤਾਂ {
                PyEval_RestoreThread(tstate);
                Py_Err_No_ਸਮ੍ਰਿਤੀ();
                PyEval_SaveThread();
                ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
            }
        }
        ਨਹੀਂਤਾਂ {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * ਆਕਾਰ(wchar_t));
            ਜੇ (tmp == ਨਿਰਾ) {
                PyEval_RestoreThread(tstate);
                Py_Err_No_ਸਮ੍ਰਿਤੀ();
                PyEval_SaveThread();
                ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
            }
            wbuf = tmp;
        }
    }

    ਜੇ (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        ਜੇ (buf) {
            buf[0] = '\0';
        }
        ਨਹੀਂਤਾਂ {
            PyEval_RestoreThread(tstate);
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            PyEval_SaveThread();
        }
        ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    u8len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0,
                                wbuf, ਕੁੱਲ_read,
                                ਨਿਰਾ, 0,
                                ਨਿਰਾ, ਨਿਰਾ);
    buf = PyMem_RawMalloc(u8len + 1);
    ਜੇ (buf == ਨਿਰਾ) {
        PyEval_RestoreThread(tstate);
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        PyEval_SaveThread();
        ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    u8len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0,
                                wbuf, ਕੁੱਲ_read,
                                buf, u8len,
                                ਨਿਰਾ, ਨਿਰਾ);
    buf[u8len] = '\0';

ਬਾਹਰ_ਨਿਕਲੋ:
    ਜੇ (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    ਜੇ (err) {
        PyEval_RestoreThread(tstate);
        Py_Err_ਸੈੱਟ_From_Windows_Err(err);
        PyEval_SaveThread();
    }
    ਵਾਪਸ buf;
}

#ਜੇ_ਅੰਤ /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

ਅੱਖਰ *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ਅਚਲ ਅੱਖਰ *prompt)
{
    ਮਾਪ_t n;
    ਅੱਖਰ *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    ਪ੍ਰਤਿਜ੍ਞਾ(tstate != ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_WINDOWS_CONSOLE_IO
    ਅਚਲ PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    ਜੇ (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE h_Std_ਵਿੱਚ, hStdErr;

        h_Std_ਵਿੱਚ = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        ਜੇ (_get_console_type(h_Std_ਵਿੱਚ) == 'r') {
            fflush(sys_stdout);
            ਜੇ (prompt) {
                ਜੇ (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    ਪੂਰਨ wlen;
                    wlen = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, prompt, -1,
                            ਨਿਰਾ, 0);
                    ਜੇ (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * ਆਕਾਰ(wchar_t));
                        ਜੇ (wbuf == ਨਿਰਾ) {
                            PyEval_RestoreThread(tstate);
                            Py_Err_No_ਸਮ੍ਰਿਤੀ();
                            PyEval_SaveThread();
                            ਵਾਪਸ ਨਿਰਾ;
                        }
                        wlen = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        ਜੇ (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, ਨਿਰਾ);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } ਨਹੀਂਤਾਂ {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            ਵਾਪਸ _PyOS_WindowsConsoleReadline(tstate, h_Std_ਵਿੱਚ);
        }
    }
#ਜੇ_ਅੰਤ

    fflush(sys_stdout);
    ਜੇ (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = ਨਿਰਾ;
    ਕਰੋ {
        ਮਾਪ_t incr = (n > 0) ? n + 2 : 100;
        ਜੇ (incr > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_ਸੈੱਟ_String(Py_Exc_ਉਛਾਲ_Error, "input line too long");
            PyEval_SaveThread();
            ਵਾਪਸ ਨਿਰਾ;
        }
        pr = (ਅੱਖਰ *)PyMem_RawRealloc(p, n + incr);
        ਜੇ (pr == ਨਿਰਾ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            PyEval_SaveThread();
            ਵਾਪਸ ਨਿਰਾ;
        }
        p = pr;
        ਪੂਰਨ err = my_fgets(tstate, p + n, (ਪੂਰਨ)incr, sys_stdin);
        ਜੇ (err == 1) {
            //ਰੁਕਾਵਟ
            PyMem_RawFree(p);
            ਵਾਪਸ ਨਿਰਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (err != 0) {
            // EOF or error
            p[n] = '\0';
            ਰੋਕੋ;
        }
        n += ਲੜੀ_ਲੰਬਾਈ(p + n);
    } ਜਦੋਂ (n == 0 || p[n-1] != '\n');

    pr = (ਅੱਖਰ *)PyMem_RawRealloc(p, n+1);
    ਜੇ (pr == ਨਿਰਾ) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        PyEval_SaveThread();
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

ਅੱਖਰ *(*Py_OS_Readline_Function_ਸੂਚਕ)(FILE *, FILE *, ਅਚਲ ਅੱਖਰ *) = ਨਿਰਾ;


ਅੱਖਰ *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ਅਚਲ ਅੱਖਰ *prompt)
{
    ਅੱਖਰ *rv, *res;
    ਮਾਪ_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    ਜੇ (Py_atomic_ਲੱਦੋ_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        Py_Err_ਸੈੱਟ_String(PyExc_RuntimeError,
                        "can't re-enter readline");
        ਵਾਪਸ ਨਿਰਾ;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_ਧਾਗੇ
    Py_Mutex_ਤਾਲਾ(&Py_OS_Readline_ਤਾਲਾ);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    ਜੇ (Py_OS_Readline_Function_ਸੂਚਕ == ਨਿਰਾ) {
        Py_OS_Readline_Function_ਸੂਚਕ = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    ਜੇ (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
        // GH-104668: Don't call global callbacks like PyOS_InputHook or
        // PyOS_ReadlineFunctionPointer from subinterpreters, since it seems
        // like there's no good way for users (like readline and tkinter) to
        // avoid using global state to manage them. Plus, we generally don't
        // want to cause trouble for libraries that don't know/care about
        // subinterpreter support. If libraries really need better APIs that
        // work per-interpreter and have ways to access module state, we can
        // certainly add them later (but for now we'll cross our fingers and
        // hope that nobody actually cares):
        !_Py_IsMainInterpreter(tstate->interp))
    {
        rv = PyOS_StdioReadline(sys_stdin, sys_stdout, prompt);
    }
    ਨਹੀਂਤਾਂ {
        rv = (*Py_OS_Readline_Function_ਸੂਚਕ)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, ਨਿਰਾ);
    PyMutex_Unlock(&Py_OS_Readline_ਤਾਲਾ);
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    ਜੇ (rv == ਨਿਰਾ)
        ਵਾਪਸ ਨਿਰਾ;

    len = ਲੜੀ_ਲੰਬਾਈ(rv) + 1;
    res = PyMem_Malloc(len);
    ਜੇ (res != ਨਿਰਾ) {
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(res, rv, len);
    }
    ਨਹੀਂਤਾਂ {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
    }
    PyMem_RawFree(rv);

    ਵਾਪਸ res;
}
