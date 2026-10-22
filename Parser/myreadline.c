
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#ಅನ್ತರ್ಭಾವ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#ಅನ್ತರ್ಭಾವ "pycore_pystate.h"       // _PyThreadState_GET()
#ಅನ್ತರ್ಭಾವ "pycore_signal.h"        // _PyOS_SigintEvent()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MS_WINDOWS
#  ifndef WIN32_LEAN_AND_ಸರಾಸರಿ
#    define WIN32_LEAN_AND_ಸರಾಸರಿ
#  endif
#  include "windows.h"
#ಆದರೆ_ಅಂತ್ಯ /* MS_WINDOWS */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#ಆದರೆ_ಅಂತ್ಯ


// Export the symbol since it's used by the readline shared extension
Py_API_ದತ್ತಾಂಶ(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = ಖಾಲಿ;

ಸ್ಥಿರ PyMutex Py_OS_Readline_ಬೀಗ;

ಪೂರ್ಣಾಂಕ (*Py_OS_ಒಳಹರಿವು_Hook)(ಶೂನ್ಯ) = ಖಾಲಿ;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
my_fgets(PyThreadState* tstate, ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ len, FILE *fp)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    Py_ಕೊನೆ_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    ಆದರೆ (handle == INVALID_HANDLE_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ -1; /* EOF */
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (1) {
        ಆದರೆ (Py_OS_ಒಳಹರಿವು_Hook != ಖಾಲಿ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ಶೂನ್ಯ)(Py_OS_ಒಳಹರಿವು_Hook)();
        }

        errno = 0;
        clearerr(fp);
        ಅಕ್ಷರ *p = fgets(buf, len, fp);
        ಆದರೆ (p != ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ 0; /* No error */
        }
        ಪೂರ್ಣಾಂಕ err = errno;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MS_WINDOWS
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
        ಆದರೆ (Get_ಕೊನೆಯ_Error()==ERROR_OPERATION_ABORTED) {
            HANDLE h_ಅಡಚಣೆ_Event = _PyOS_SigintEvent();
            ಆಯ್ಕೆ (WaitForSingleObjectEx(h_ಅಡಚಣೆ_Event, 10, FALSE)) {
            ಸ್ಥಿತಿ WAIT_OBJECT_0:
                ResetEvent(h_ಅಡಚಣೆ_Event);
                ಹಿಂತಿರುಗಿ 1; /*ಅಡಚಣೆ*/
            ಸ್ಥಿತಿ WAIT_FAILED:
                ಹಿಂತಿರುಗಿ -2; /* Error */
            }
        }
#ಆದರೆ_ಅಂತ್ಯ /* MS_WINDOWS */

        ಆದರೆ (feof(fp)) {
            clearerr(fp);
            ಹಿಂತಿರುಗಿ -1; /* EOF */
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EINTR
        ಆದರೆ (err == EINTR) {
            PyEval_RestoreThread(tstate);
            ಪೂರ್ಣಾಂಕ s = Py_Err_Check_ಸಂಕೇತಗಳು();
            PyEval_SaveThread();

            ಆದರೆ (s < 0) {
                ಹಿಂತಿರುಗಿ 1;
            }
            /* try again */
            ಮುಂದುವರಿ;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (Py_OS_ಅಡಚಣೆ_Occurred(tstate)) {
            ಹಿಂತಿರುಗಿ 1; /*ಅಡಚಣೆ*/
        }
        ಹಿಂತಿರುಗಿ -2; /* Error */
    }
    /* NOTREACHED */
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

ಬಾಹ್ಯ ಅಕ್ಷರ _get_console_type(HANDLE handle);

ಅಕ್ಷರ *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE h_Std_ಒಳ)
{
    ಸ್ಥಿರ wchar_t wbuf_local[1024 * 16];
    ಸ್ಥಿರಾಂಕ DWORD chunk_ಗಾತ್ರ = 1024;

    DWORD n_read, ಒಟ್ಟು_read, wbuflen, u8len;
    wchar_t *wbuf;
    ಅಕ್ಷರ *buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ err = 0;

    n_read = (DWORD)-1;
    ಒಟ್ಟು_read = 0;
    wbuf = wbuf_local;
    wbuflen = ಗಾತ್ರ(wbuf_local) / ಗಾತ್ರ(wbuf_local[0]) - 1;
    ಆಗ (1) {
        ಆದರೆ (Py_OS_ಒಳಹರಿವು_Hook != ಖಾಲಿ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ಶೂನ್ಯ)(Py_OS_ಒಳಹರಿವು_Hook)();
        }
        ಆದರೆ (!ReadConsoleW(h_Std_ಒಳ, &wbuf[ಒಟ್ಟು_read], wbuflen - ಒಟ್ಟು_read, &n_read, ಖಾಲಿ)) {
            err = Get_ಕೊನೆಯ_Error();
            ಹೋಗು ನಿರ್ಗಮಿಸು;
        }
        ಆದರೆ (n_read == (DWORD)-1 && (err = Get_ಕೊನೆಯ_Error()) == ERROR_OPERATION_ABORTED) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (n_read == 0) {
            ಪೂರ್ಣಾಂಕ s;
            err = Get_ಕೊನೆಯ_Error();
            ಆದರೆ (err != ERROR_OPERATION_ABORTED)
                ಹೋಗು ನಿರ್ಗಮಿಸು;
            err = 0;
            HANDLE h_ಅಡಚಣೆ_Event = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(h_ಅಡಚಣೆ_Event, 100, FALSE);
            ಆದರೆ (state == WAIT_OBJECT_0) {
                ResetEvent(h_ಅಡಚಣೆ_Event);
                PyEval_RestoreThread(tstate);
                s = Py_Err_Check_ಸಂಕೇತಗಳು();
                PyEval_SaveThread();
                ಆದರೆ (s < 0) {
                    ಹೋಗು ನಿರ್ಗಮಿಸು;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state != WAIT_TIMEOUT) {
                err = Get_ಕೊನೆಯ_Error();
                ಹೋಗು ನಿರ್ಗಮಿಸು;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            ಮುಂದುವರಿ;
        }

        ಒಟ್ಟು_read += n_read;
        ಆದರೆ (ಒಟ್ಟು_read == 0 || wbuf[ಒಟ್ಟು_read - 1] == L'\n') {
            ನಿಲ್ಲಿಸು;
        }
        wbuflen += chunk_ಗಾತ್ರ;
        ಆದರೆ (wbuf == wbuf_local) {
            wbuf[ಒಟ್ಟು_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * ಗಾತ್ರ(wchar_t));
            ಆದರೆ (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                PyEval_RestoreThread(tstate);
                Py_Err_No_ಸ್ಮರಣೆ();
                PyEval_SaveThread();
                ಹೋಗು ನಿರ್ಗಮಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * ಗಾತ್ರ(wchar_t));
            ಆದರೆ (tmp == ಖಾಲಿ) {
                PyEval_RestoreThread(tstate);
                Py_Err_No_ಸ್ಮರಣೆ();
                PyEval_SaveThread();
                ಹೋಗು ನಿರ್ಗಮಿಸು;
            }
            wbuf = tmp;
        }
    }

    ಆದರೆ (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        ಆದರೆ (buf) {
            buf[0] = '\0';
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            PyEval_RestoreThread(tstate);
            Py_Err_No_ಸ್ಮರಣೆ();
            PyEval_SaveThread();
        }
        ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    u8len = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0,
                                wbuf, ಒಟ್ಟು_read,
                                ಖಾಲಿ, 0,
                                ಖಾಲಿ, ಖಾಲಿ);
    buf = PyMem_RawMalloc(u8len + 1);
    ಆದರೆ (buf == ಖಾಲಿ) {
        PyEval_RestoreThread(tstate);
        Py_Err_No_ಸ್ಮರಣೆ();
        PyEval_SaveThread();
        ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    u8len = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0,
                                wbuf, ಒಟ್ಟು_read,
                                buf, u8len,
                                ಖಾಲಿ, ಖಾಲಿ);
    buf[u8len] = '\0';

ನಿರ್ಗಮಿಸು:
    ಆದರೆ (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    ಆದರೆ (err) {
        PyEval_RestoreThread(tstate);
        Py_Err_ಗಣ_From_Windows_Err(err);
        PyEval_SaveThread();
    }
    ಹಿಂತಿರುಗಿ buf;
}

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

ಅಕ್ಷರ *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prompt)
{
    ಗಾತ್ರ_t n;
    ಅಕ್ಷರ *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    ಪ್ರತಿಜ್ಞಾ(tstate != ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_WINDOWS_CONSOLE_IO
    ಸ್ಥಿರಾಂಕ PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    ಆದರೆ (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE h_Std_ಒಳ, hStdErr;

        h_Std_ಒಳ = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        ಆದರೆ (_get_console_type(h_Std_ಒಳ) == 'r') {
            fflush(sys_stdout);
            ಆದರೆ (prompt) {
                ಆದರೆ (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    ಪೂರ್ಣಾಂಕ wlen;
                    wlen = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, prompt, -1,
                            ಖಾಲಿ, 0);
                    ಆದರೆ (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * ಗಾತ್ರ(wchar_t));
                        ಆದರೆ (wbuf == ಖಾಲಿ) {
                            PyEval_RestoreThread(tstate);
                            Py_Err_No_ಸ್ಮರಣೆ();
                            PyEval_SaveThread();
                            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                        }
                        wlen = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        ಆದರೆ (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, ಖಾಲಿ);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            ಹಿಂತಿರುಗಿ _PyOS_WindowsConsoleReadline(tstate, h_Std_ಒಳ);
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    fflush(sys_stdout);
    ಆದರೆ (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = ಖಾಲಿ;
    ಮಾಡು {
        ಗಾತ್ರ_t incr = (n > 0) ? n + 2 : 100;
        ಆದರೆ (incr > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_ಗಣ_String(Py_Exc_ಅತಿಪ್ರವಾಹ_Error, "input line too long");
            PyEval_SaveThread();
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        pr = (ಅಕ್ಷರ *)PyMem_RawRealloc(p, n + incr);
        ಆದರೆ (pr == ಖಾಲಿ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_No_ಸ್ಮರಣೆ();
            PyEval_SaveThread();
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        p = pr;
        ಪೂರ್ಣಾಂಕ err = my_fgets(tstate, p + n, (ಪೂರ್ಣಾಂಕ)incr, sys_stdin);
        ಆದರೆ (err == 1) {
            //ಅಡಚಣೆ
            PyMem_RawFree(p);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (err != 0) {
            // EOF or error
            p[n] = '\0';
            ನಿಲ್ಲಿಸು;
        }
        n += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p + n);
    } ಆಗ (n == 0 || p[n-1] != '\n');

    pr = (ಅಕ್ಷರ *)PyMem_RawRealloc(p, n+1);
    ಆದರೆ (pr == ಖಾಲಿ) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        Py_Err_No_ಸ್ಮರಣೆ();
        PyEval_SaveThread();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

ಅಕ್ಷರ *(*Py_OS_Readline_Function_ಸೂಚಕ)(FILE *, FILE *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) = ಖಾಲಿ;


ಅಕ್ಷರ *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prompt)
{
    ಅಕ್ಷರ *rv, *res;
    ಗಾತ್ರ_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    ಆದರೆ (Py_atomic_ತುಂಬು_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        Py_Err_ಗಣ_String(PyExc_RuntimeError,
                        "can't re-enter readline");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Py_Mutex_ಬೀಗ(&Py_OS_Readline_ಬೀಗ);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    ಆದರೆ (Py_OS_Readline_Function_ಸೂಚಕ == ಖಾಲಿ) {
        Py_OS_Readline_Function_ಸೂಚಕ = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    ಆದರೆ (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    ಇಲ್ಲದಿದ್ದರೆ {
        rv = (*Py_OS_Readline_Function_ಸೂಚಕ)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, ಖಾಲಿ);
    PyMutex_Unlock(&Py_OS_Readline_ಬೀಗ);
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    ಆದರೆ (rv == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(rv) + 1;
    res = PyMem_Malloc(len);
    ಆದರೆ (res != ಖಾಲಿ) {
        ಸ್ಮರಣೆ_ನಕಲು(res, rv, len);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        Py_Err_No_ಸ್ಮರಣೆ();
    }
    PyMem_RawFree(rv);

    ಹಿಂತಿರುಗಿ res;
}
