
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
#ifdef MS_WINDOWS
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include "windows.h"
#endif /* MS_WINDOWS */

#ifdef HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#endif


// Export the symbol since it's used by the readline shared extension
PyAPI_DATA(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = ರಿಕ್ತ;

ಸ್ಥಿರ PyMutex _PyOS_ReadlineLock;

ಪೂರ್ಣಾಙ್ಕ (*Py_OS_ಒಳಹರಿವು_Hook)(ಶೂನ್ಯ) = ರಿಕ್ತ;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
my_fgets(PyThreadState* tstate, ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    ಯದಿ (handle == INVALID_HANDLE_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ -1; /* EOF */
    }
#endif

    ಯಾವತ್ (1) {
        ಯದಿ (Py_OS_ಒಳಹರಿವು_Hook != ರಿಕ್ತ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ಶೂನ್ಯ)(Py_OS_ಒಳಹರಿವು_Hook)();
        }

        errno = 0;
        clearerr(fp);
        ವರ್ಣ *p = fgets(buf, len, fp);
        ಯದಿ (p != ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ 0; /* No error */
        }
        ಪೂರ್ಣಾಙ್ಕ err = errno;

#ifdef MS_WINDOWS
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
        ಯದಿ (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            ವಿಕಲ್ಪ (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            ಪಕ್ಷ WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                ಪ್ರತ್ಯಾಗಮ 1; /* Interrupt */
            ಪಕ್ಷ WAIT_FAILED:
                ಪ್ರತ್ಯಾಗಮ -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        ಯದಿ (feof(fp)) {
            clearerr(fp);
            ಪ್ರತ್ಯಾಗಮ -1; /* EOF */
        }

#ifdef EINTR
        ಯದಿ (err == EINTR) {
            PyEval_RestoreThread(tstate);
            ಪೂರ್ಣಾಙ್ಕ s = PyErr_CheckSignals();
            PyEval_SaveThread();

            ಯದಿ (s < 0) {
                ಪ್ರತ್ಯಾಗಮ 1;
            }
            /* try again */
            ಅನುವರ್ತ;
        }
#endif

        ಯದಿ (_PyOS_InterruptOccurred(tstate)) {
            ಪ್ರತ್ಯಾಗಮ 1; /* Interrupt */
        }
        ಪ್ರತ್ಯಾಗಮ -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

ಬಾಹ್ಯ ವರ್ಣ _get_console_type(HANDLE handle);

ವರ್ಣ *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    ಸ್ಥಿರ wchar_t wbuf_local[1024 * 16];
    ಧ್ರುವ DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    ವರ್ಣ *buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = ಪರಿಮಾಣ(wbuf_local) / ಪರಿಮಾಣ(wbuf_local[0]) - 1;
    ಯಾವತ್ (1) {
        ಯದಿ (Py_OS_ಒಳಹರಿವು_Hook != ರಿಕ್ತ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ಶೂನ್ಯ)(Py_OS_ಒಳಹರಿವು_Hook)();
        }
        ಯದಿ (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, ರಿಕ್ತ)) {
            err = GetLastError();
            ಗಚ್ಛ exit;
        }
        ಯದಿ (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            ಭಙ್ಗ;
        }
        ಯದಿ (n_read == 0) {
            ಪೂರ್ಣಾಙ್ಕ s;
            err = GetLastError();
            ಯದಿ (err != ERROR_OPERATION_ABORTED)
                ಗಚ್ಛ exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            ಯದಿ (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                ಯದಿ (s < 0) {
                    ಗಚ್ಛ exit;
                }
            }
            ಅನ್ಯಥಾ ಯದಿ (state != WAIT_TIMEOUT) {
                err = GetLastError();
                ಗಚ್ಛ exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            ಅನುವರ್ತ;
        }

        total_read += n_read;
        ಯದಿ (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            ಭಙ್ಗ;
        }
        wbuflen += chunk_size;
        ಯದಿ (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * ಪರಿಮಾಣ(wchar_t));
            ಯದಿ (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            ಅನ್ಯಥಾ {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                ಗಚ್ಛ exit;
            }
        }
        ಅನ್ಯಥಾ {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * ಪರಿಮಾಣ(wchar_t));
            ಯದಿ (tmp == ರಿಕ್ತ) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                ಗಚ್ಛ exit;
            }
            wbuf = tmp;
        }
    }

    ಯದಿ (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        ಯದಿ (buf) {
            buf[0] = '\0';
        }
        ಅನ್ಯಥಾ {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        ಗಚ್ಛ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                ರಿಕ್ತ, 0,
                                ರಿಕ್ತ, ರಿಕ್ತ);
    buf = PyMem_RawMalloc(u8len + 1);
    ಯದಿ (buf == ರಿಕ್ತ) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        ಗಚ್ಛ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                ರಿಕ್ತ, ರಿಕ್ತ);
    buf[u8len] = '\0';

exit:
    ಯದಿ (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    ಯದಿ (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

ವರ್ಣ *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ಧ್ರುವ ವರ್ಣ *prompt)
{
    size_t n;
    ವರ್ಣ *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    ಪ್ರತಿಜ್ಞಾ(tstate != ರಿಕ್ತ);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    ಧ್ರುವ PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    ಯದಿ (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        ಯದಿ (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            ಯದಿ (prompt) {
                ಯದಿ (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    ಪೂರ್ಣಾಙ್ಕ wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            ರಿಕ್ತ, 0);
                    ಯದಿ (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * ಪರಿಮಾಣ(wchar_t));
                        ಯದಿ (wbuf == ರಿಕ್ತ) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        ಯದಿ (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, ರಿಕ್ತ);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } ಅನ್ಯಥಾ {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            ಪ್ರತ್ಯಾಗಮ _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    ಯದಿ (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = ರಿಕ್ತ;
    ಕುರು {
        size_t incr = (n > 0) ? n + 2 : 100;
        ಯದಿ (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        pr = (ವರ್ಣ *)PyMem_RawRealloc(p, n + incr);
        ಯದಿ (pr == ರಿಕ್ತ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        p = pr;
        ಪೂರ್ಣಾಙ್ಕ err = my_fgets(tstate, p + n, (ಪೂರ್ಣಾಙ್ಕ)incr, sys_stdin);
        ಯದಿ (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        } ಅನ್ಯಥಾ ಯದಿ (err != 0) {
            // EOF or error
            p[n] = '\0';
            ಭಙ್ಗ;
        }
        n += strlen(p + n);
    } ಯಾವತ್ (n == 0 || p[n-1] != '\n');

    pr = (ವರ್ಣ *)PyMem_RawRealloc(p, n+1);
    ಯದಿ (pr == ರಿಕ್ತ) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

ವರ್ಣ *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, ಧ್ರುವ ವರ್ಣ *) = ರಿಕ್ತ;


ವರ್ಣ *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ಧ್ರುವ ವರ್ಣ *prompt)
{
    ವರ್ಣ *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    ಯದಿ (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    ಯದಿ (PyOS_ReadlineFunctionPointer == ರಿಕ್ತ) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    ಯದಿ (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    ಅನ್ಯಥಾ {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, ರಿಕ್ತ);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    ಯದಿ (rv == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    ಯದಿ (res != ರಿಕ್ತ) {
        memcpy(res, rv, len);
    }
    ಅನ್ಯಥಾ {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    ಪ್ರತ್ಯಾಗಮ res;
}
