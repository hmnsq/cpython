
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#अन्तर्भाव "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#अन्तर्भाव "pycore_pystate.h"       // _PyThreadState_GET()
#अन्तर्भाव "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = रिक्त;

स्थिर PyMutex _PyOS_ReadlineLock;

पूर्णाङ्क (*Py_OS_निवेशः_Hook)(शून्य) = रिक्त;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

स्थिर पूर्णाङ्क
my_fgets(PyThreadState* tstate, वर्ण *buf, पूर्णाङ्क len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    सचे (handle == INVALID_HANDLE_मानम्) {
        पटिनिवत्त -1; /* EOF */
    }
#endif

    याव (1) {
        सचे (Py_OS_निवेशः_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_निवेशः_Hook)();
        }

        errno = 0;
        clearerr(fp);
        वर्ण *p = fgets(buf, len, fp);
        सचे (p != रिक्त) {
            पटिनिवत्त 0; /* No error */
        }
        पूर्णाङ्क err = errno;

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
        सचे (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            विकल्प (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            पक्ष WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                पटिनिवत्त 1; /* Interrupt */
            पक्ष WAIT_FAILED:
                पटिनिवत्त -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        सचे (feof(fp)) {
            clearerr(fp);
            पटिनिवत्त -1; /* EOF */
        }

#ifdef EINTR
        सचे (err == EINTR) {
            PyEval_RestoreThread(tstate);
            पूर्णाङ्क s = PyErr_CheckSignals();
            PyEval_SaveThread();

            सचे (s < 0) {
                पटिनिवत्त 1;
            }
            /* try again */
            अनुवर्त;
        }
#endif

        सचे (_PyOS_InterruptOccurred(tstate)) {
            पटिनिवत्त 1; /* Interrupt */
        }
        पटिनिवत्त -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

बाह्य वर्ण _get_console_type(HANDLE handle);

वर्ण *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    स्थिर wchar_t wbuf_local[1024 * 16];
    ध्रुव DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    वर्ण *buf = रिक्त;
    पूर्णाङ्क err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = परिमाण(wbuf_local) / परिमाण(wbuf_local[0]) - 1;
    याव (1) {
        सचे (Py_OS_निवेशः_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_निवेशः_Hook)();
        }
        सचे (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, रिक्त)) {
            err = GetLastError();
            गच्छ exit;
        }
        सचे (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            भङ्ग;
        }
        सचे (n_read == 0) {
            पूर्णाङ्क s;
            err = GetLastError();
            सचे (err != ERROR_OPERATION_ABORTED)
                गच्छ exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            सचे (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                सचे (s < 0) {
                    गच्छ exit;
                }
            }
            अञ्ञथा सचे (state != WAIT_TIMEOUT) {
                err = GetLastError();
                गच्छ exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            अनुवर्त;
        }

        total_read += n_read;
        सचे (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            भङ्ग;
        }
        wbuflen += chunk_size;
        सचे (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * परिमाण(wchar_t));
            सचे (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            अञ्ञथा {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                गच्छ exit;
            }
        }
        अञ्ञथा {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * परिमाण(wchar_t));
            सचे (tmp == रिक्त) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                गच्छ exit;
            }
            wbuf = tmp;
        }
    }

    सचे (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        सचे (buf) {
            buf[0] = '\0';
        }
        अञ्ञथा {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        गच्छ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                रिक्त, 0,
                                रिक्त, रिक्त);
    buf = PyMem_RawMalloc(u8len + 1);
    सचे (buf == रिक्त) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        गच्छ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                रिक्त, रिक्त);
    buf[u8len] = '\0';

exit:
    सचे (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    सचे (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    पटिनिवत्त buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

वर्ण *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ध्रुव वर्ण *prompt)
{
    size_t n;
    वर्ण *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    प्रतिज्ञा(tstate != रिक्त);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    ध्रुव PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    सचे (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        सचे (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            सचे (prompt) {
                सचे (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    पूर्णाङ्क wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            रिक्त, 0);
                    सचे (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * परिमाण(wchar_t));
                        सचे (wbuf == रिक्त) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            पटिनिवत्त रिक्त;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        सचे (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, रिक्त);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } अञ्ञथा {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            पटिनिवत्त _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    सचे (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = रिक्त;
    करोहि {
        size_t incr = (n > 0) ? n + 2 : 100;
        सचे (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            पटिनिवत्त रिक्त;
        }
        pr = (वर्ण *)PyMem_RawRealloc(p, n + incr);
        सचे (pr == रिक्त) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            पटिनिवत्त रिक्त;
        }
        p = pr;
        पूर्णाङ्क err = my_fgets(tstate, p + n, (पूर्णाङ्क)incr, sys_stdin);
        सचे (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            पटिनिवत्त रिक्त;
        } अञ्ञथा सचे (err != 0) {
            // EOF or error
            p[n] = '\0';
            भङ्ग;
        }
        n += strlen(p + n);
    } याव (n == 0 || p[n-1] != '\n');

    pr = (वर्ण *)PyMem_RawRealloc(p, n+1);
    सचे (pr == रिक्त) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

वर्ण *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, ध्रुव वर्ण *) = रिक्त;


वर्ण *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ध्रुव वर्ण *prompt)
{
    वर्ण *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    सचे (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        पटिनिवत्त रिक्त;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    सचे (PyOS_ReadlineFunctionPointer == रिक्त) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    सचे (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    अञ्ञथा {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, रिक्त);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    सचे (rv == रिक्त)
        पटिनिवत्त रिक्त;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    सचे (res != रिक्त) {
        memcpy(res, rv, len);
    }
    अञ्ञथा {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    पटिनिवत्त res;
}
