
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#ضمن "Python.h"
#ضمن "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#ضمن "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#ضمن "pycore_pystate.h"       // _PyThreadState_GET()
#ضمن "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = عدم;

ساكن PyMutex _PyOS_ReadlineLock;

صحيح (*PyOS_InputHook)(فراغ) = عدم;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ساكن صحيح
my_fgets(PyThreadState* tstate, حرف *buf, صحيح len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    𐤀𐤌 (handle == INVALID_HANDLE_VALUE) {
        أرجع -1; /* EOF */
    }
#endif

    طالما (1) {
        𐤀𐤌 (PyOS_InputHook != عدم &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (فراغ)(PyOS_InputHook)();
        }

        errno = 0;
        clearerr(fp);
        حرف *p = fgets(buf, len, fp);
        𐤀𐤌 (p != عدم) {
            أرجع 0; /* No error */
        }
        صحيح err = errno;

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
        𐤀𐤌 (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            بدل (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            حالة WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                أرجع 1; /* Interrupt */
            حالة WAIT_FAILED:
                أرجع -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        𐤀𐤌 (feof(fp)) {
            clearerr(fp);
            أرجع -1; /* EOF */
        }

#ifdef EINTR
        𐤀𐤌 (err == EINTR) {
            PyEval_RestoreThread(tstate);
            صحيح s = PyErr_CheckSignals();
            PyEval_SaveThread();

            𐤀𐤌 (s < 0) {
                أرجع 1;
            }
            /* try again */
            استمر;
        }
#endif

        𐤀𐤌 (_PyOS_InterruptOccurred(tstate)) {
            أرجع 1; /* Interrupt */
        }
        أرجع -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

خارجي حرف _get_console_type(HANDLE handle);

حرف *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    ساكن wchar_t wbuf_local[1024 * 16];
    ثابت DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    حرف *buf = عدم;
    صحيح err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = حجم(wbuf_local) / حجم(wbuf_local[0]) - 1;
    طالما (1) {
        𐤀𐤌 (PyOS_InputHook != عدم &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (فراغ)(PyOS_InputHook)();
        }
        𐤀𐤌 (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, عدم)) {
            err = GetLastError();
            اذهب exit;
        }
        𐤀𐤌 (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            اكسر;
        }
        𐤀𐤌 (n_read == 0) {
            صحيح s;
            err = GetLastError();
            𐤀𐤌 (err != ERROR_OPERATION_ABORTED)
                اذهب exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            𐤀𐤌 (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                𐤀𐤌 (s < 0) {
                    اذهب exit;
                }
            }
            وإلا 𐤀𐤌 (state != WAIT_TIMEOUT) {
                err = GetLastError();
                اذهب exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            استمر;
        }

        total_read += n_read;
        𐤀𐤌 (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            اكسر;
        }
        wbuflen += chunk_size;
        𐤀𐤌 (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * حجم(wchar_t));
            𐤀𐤌 (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            وإلا {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                اذهب exit;
            }
        }
        وإلا {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * حجم(wchar_t));
            𐤀𐤌 (tmp == عدم) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                اذهب exit;
            }
            wbuf = tmp;
        }
    }

    𐤀𐤌 (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        𐤀𐤌 (buf) {
            buf[0] = '\0';
        }
        وإلا {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        اذهب exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                عدم, 0,
                                عدم, عدم);
    buf = PyMem_RawMalloc(u8len + 1);
    𐤀𐤌 (buf == عدم) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        اذهب exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                عدم, عدم);
    buf[u8len] = '\0';

exit:
    𐤀𐤌 (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    𐤀𐤌 (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    أرجع buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

حرف *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ثابت حرف *prompt)
{
    size_t n;
    حرف *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    أكد(tstate != عدم);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    ثابت PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    𐤀𐤌 (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        𐤀𐤌 (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            𐤀𐤌 (prompt) {
                𐤀𐤌 (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    صحيح wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            عدم, 0);
                    𐤀𐤌 (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * حجم(wchar_t));
                        𐤀𐤌 (wbuf == عدم) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            أرجع عدم;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        𐤀𐤌 (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, عدم);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } وإلا {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            أرجع _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    𐤀𐤌 (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = عدم;
    𐤐𐤏𐤋 {
        size_t incr = (n > 0) ? n + 2 : 100;
        𐤀𐤌 (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            أرجع عدم;
        }
        pr = (حرف *)PyMem_RawRealloc(p, n + incr);
        𐤀𐤌 (pr == عدم) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            أرجع عدم;
        }
        p = pr;
        صحيح err = my_fgets(tstate, p + n, (صحيح)incr, sys_stdin);
        𐤀𐤌 (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            أرجع عدم;
        } وإلا 𐤀𐤌 (err != 0) {
            // EOF or error
            p[n] = '\0';
            اكسر;
        }
        n += strlen(p + n);
    } طالما (n == 0 || p[n-1] != '\n');

    pr = (حرف *)PyMem_RawRealloc(p, n+1);
    𐤀𐤌 (pr == عدم) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        أرجع عدم;
    }
    أرجع pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

حرف *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, ثابت حرف *) = عدم;


حرف *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ثابت حرف *prompt)
{
    حرف *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    𐤀𐤌 (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        أرجع عدم;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    𐤀𐤌 (PyOS_ReadlineFunctionPointer == عدم) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    𐤀𐤌 (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    وإلا {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, عدم);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    𐤀𐤌 (rv == عدم)
        أرجع عدم;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    𐤀𐤌 (res != عدم) {
        memcpy(res, rv, len);
    }
    وإلا {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    أرجع res;
}
