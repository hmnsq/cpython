
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#অন্তর্ভাৱ "Python.h"
#অন্তর্ভাৱ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#অন্তর্ভাৱ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#অন্তর্ভাৱ "pycore_pystate.h"       // _PyThreadState_GET()
#অন্তর্ভাৱ "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = রিক্ত;

স্থির PyMutex _PyOS_ReadlineLock;

পূর্ণাঙ্ক (*Py_OS_প্ৰবেশ_Hook)(শূন্য) = রিক্ত;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

স্থির পূর্ণাঙ্ক
my_fgets(PyThreadState* tstate, ৱর্ণ *buf, পূর্ণাঙ্ক len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    যদি (handle == INVALID_HANDLE_মান) {
        ঘূৰাই -1; /* EOF */
    }
#endif

    যেতিয়ালৈ (1) {
        যদি (Py_OS_প্ৰবেশ_Hook != রিক্ত &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (শূন্য)(Py_OS_প্ৰবেশ_Hook)();
        }

        errno = 0;
        clearerr(fp);
        ৱর্ণ *p = fgets(buf, len, fp);
        যদি (p != রিক্ত) {
            ঘূৰাই 0; /* No error */
        }
        পূর্ণাঙ্ক err = errno;

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
        যদি (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            ৱিকল্প (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            পক্ষ WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                ঘূৰাই 1; /* Interrupt */
            পক্ষ WAIT_FAILED:
                ঘূৰাই -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        যদি (feof(fp)) {
            clearerr(fp);
            ঘূৰাই -1; /* EOF */
        }

#ifdef EINTR
        যদি (err == EINTR) {
            PyEval_RestoreThread(tstate);
            পূর্ণাঙ্ক s = PyErr_CheckSignals();
            PyEval_SaveThread();

            যদি (s < 0) {
                ঘূৰাই 1;
            }
            /* try again */
            অনুৱর্ত;
        }
#endif

        যদি (_PyOS_InterruptOccurred(tstate)) {
            ঘূৰাই 1; /* Interrupt */
        }
        ঘূৰাই -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

বাহ্য ৱর্ণ _get_console_type(HANDLE handle);

ৱর্ণ *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    স্থির wchar_t wbuf_local[1024 * 16];
    ধ্রুৱ DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    ৱর্ণ *buf = রিক্ত;
    পূর্ণাঙ্ক err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = পরিমাণ(wbuf_local) / পরিমাণ(wbuf_local[0]) - 1;
    যেতিয়ালৈ (1) {
        যদি (Py_OS_প্ৰবেশ_Hook != রিক্ত &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (শূন্য)(Py_OS_প্ৰবেশ_Hook)();
        }
        যদি (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, রিক্ত)) {
            err = GetLastError();
            গচ্ছ exit;
        }
        যদি (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            ভঙ্গ;
        }
        যদি (n_read == 0) {
            পূর্ণাঙ্ক s;
            err = GetLastError();
            যদি (err != ERROR_OPERATION_ABORTED)
                গচ্ছ exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            যদি (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                যদি (s < 0) {
                    গচ্ছ exit;
                }
            }
            অন্যথা যদি (state != WAIT_TIMEOUT) {
                err = GetLastError();
                গচ্ছ exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            অনুৱর্ত;
        }

        total_read += n_read;
        যদি (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            ভঙ্গ;
        }
        wbuflen += chunk_size;
        যদি (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * পরিমাণ(wchar_t));
            যদি (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            অন্যথা {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                গচ্ছ exit;
            }
        }
        অন্যথা {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * পরিমাণ(wchar_t));
            যদি (tmp == রিক্ত) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                গচ্ছ exit;
            }
            wbuf = tmp;
        }
    }

    যদি (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        যদি (buf) {
            buf[0] = '\0';
        }
        অন্যথা {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        গচ্ছ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                রিক্ত, 0,
                                রিক্ত, রিক্ত);
    buf = PyMem_RawMalloc(u8len + 1);
    যদি (buf == রিক্ত) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        গচ্ছ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                রিক্ত, রিক্ত);
    buf[u8len] = '\0';

exit:
    যদি (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    যদি (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    ঘূৰাই buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

ৱর্ণ *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ধ্রুৱ ৱর্ণ *prompt)
{
    size_t n;
    ৱর্ণ *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    প্রতিজ্ঞা(tstate != রিক্ত);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    ধ্রুৱ PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    যদি (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        যদি (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            যদি (prompt) {
                যদি (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    পূর্ণাঙ্ক wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            রিক্ত, 0);
                    যদি (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * পরিমাণ(wchar_t));
                        যদি (wbuf == রিক্ত) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            ঘূৰাই রিক্ত;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        যদি (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, রিক্ত);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } অন্যথা {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            ঘূৰাই _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    যদি (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = রিক্ত;
    কৰা {
        size_t incr = (n > 0) ? n + 2 : 100;
        যদি (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            ঘূৰাই রিক্ত;
        }
        pr = (ৱর্ণ *)PyMem_RawRealloc(p, n + incr);
        যদি (pr == রিক্ত) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            ঘূৰাই রিক্ত;
        }
        p = pr;
        পূর্ণাঙ্ক err = my_fgets(tstate, p + n, (পূর্ণাঙ্ক)incr, sys_stdin);
        যদি (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            ঘূৰাই রিক্ত;
        } অন্যথা যদি (err != 0) {
            // EOF or error
            p[n] = '\0';
            ভঙ্গ;
        }
        n += strlen(p + n);
    } যেতিয়ালৈ (n == 0 || p[n-1] != '\n');

    pr = (ৱর্ণ *)PyMem_RawRealloc(p, n+1);
    যদি (pr == রিক্ত) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

ৱর্ণ *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, ধ্রুৱ ৱর্ণ *) = রিক্ত;


ৱর্ণ *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ধ্রুৱ ৱর্ণ *prompt)
{
    ৱর্ণ *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    যদি (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        ঘূৰাই রিক্ত;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    যদি (PyOS_ReadlineFunctionPointer == রিক্ত) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    যদি (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    অন্যথা {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, রিক্ত);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    যদি (rv == রিক্ত)
        ঘূৰাই রিক্ত;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    যদি (res != রিক্ত) {
        memcpy(res, rv, len);
    }
    অন্যথা {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    ঘূৰাই res;
}
