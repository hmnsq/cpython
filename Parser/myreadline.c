
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#ଅନ୍ତର୍ଭାଵ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#ଅନ୍ତର୍ଭାଵ "pycore_pystate.h"       // _PyThreadState_GET()
#ଅନ୍ତର୍ଭାଵ "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = ଖାଲି;

ସ୍ଥିର PyMutex _PyOS_ReadlineLock;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*Py_OS_ନିବେଶ_Hook)(ଶୂନ୍ୟ) = ଖାଲି;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
my_fgets(PyThreadState* tstate, ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    ଯଦି (handle == INVALID_HANDLE_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ -1; /* EOF */
    }
#endif

    ଯେତେବେଳେ (1) {
        ଯଦି (Py_OS_ନିବେଶ_Hook != ଖାଲି &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ଶୂନ୍ୟ)(Py_OS_ନିବେଶ_Hook)();
        }

        errno = 0;
        clearerr(fp);
        ଅକ୍ଷର *p = fgets(buf, len, fp);
        ଯଦି (p != ଖାଲି) {
            ଫେରନ୍ତୁ 0; /* No error */
        }
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = errno;

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
        ଯଦି (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            ବାଛ (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            ସ୍ଥିତି WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                ଫେରନ୍ତୁ 1; /* Interrupt */
            ସ୍ଥିତି WAIT_FAILED:
                ଫେରନ୍ତୁ -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        ଯଦି (feof(fp)) {
            clearerr(fp);
            ଫେରନ୍ତୁ -1; /* EOF */
        }

#ifdef EINTR
        ଯଦି (err == EINTR) {
            PyEval_RestoreThread(tstate);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = PyErr_CheckSignals();
            PyEval_SaveThread();

            ଯଦି (s < 0) {
                ଫେରନ୍ତୁ 1;
            }
            /* try again */
            ଜାରି;
        }
#endif

        ଯଦି (_PyOS_InterruptOccurred(tstate)) {
            ଫେରନ୍ତୁ 1; /* Interrupt */
        }
        ଫେରନ୍ତୁ -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

ବାହ୍ୟ ଅକ୍ଷର _get_console_type(HANDLE handle);

ଅକ୍ଷର *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    ସ୍ଥିର wchar_t wbuf_local[1024 * 16];
    ଅଚଳ DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    ଅକ୍ଷର *buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = ଆକାର(wbuf_local) / ଆକାର(wbuf_local[0]) - 1;
    ଯେତେବେଳେ (1) {
        ଯଦି (Py_OS_ନିବେଶ_Hook != ଖାଲି &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ଶୂନ୍ୟ)(Py_OS_ନିବେଶ_Hook)();
        }
        ଯଦି (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, ଖାଲି)) {
            err = GetLastError();
            ଯାଅ exit;
        }
        ଯଦି (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (n_read == 0) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା s;
            err = GetLastError();
            ଯଦି (err != ERROR_OPERATION_ABORTED)
                ଯାଅ exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            ଯଦି (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                ଯଦି (s < 0) {
                    ଯାଅ exit;
                }
            }
            ଅନ୍ୟଥା ଯଦି (state != WAIT_TIMEOUT) {
                err = GetLastError();
                ଯାଅ exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            ଜାରି;
        }

        total_read += n_read;
        ଯଦି (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            ଭାଙ୍ଗ;
        }
        wbuflen += chunk_size;
        ଯଦି (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * ଆକାର(wchar_t));
            ଯଦି (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            ଅନ୍ୟଥା {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                ଯାଅ exit;
            }
        }
        ଅନ୍ୟଥା {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * ଆକାର(wchar_t));
            ଯଦି (tmp == ଖାଲି) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                ଯାଅ exit;
            }
            wbuf = tmp;
        }
    }

    ଯଦି (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        ଯଦି (buf) {
            buf[0] = '\0';
        }
        ଅନ୍ୟଥା {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        ଯାଅ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                ଖାଲି, 0,
                                ଖାଲି, ଖାଲି);
    buf = PyMem_RawMalloc(u8len + 1);
    ଯଦି (buf == ଖାଲି) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        ଯାଅ exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                ଖାଲି, ଖାଲି);
    buf[u8len] = '\0';

exit:
    ଯଦି (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    ଯଦି (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    ଫେରନ୍ତୁ buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

ଅକ୍ଷର *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ଅଚଳ ଅକ୍ଷର *prompt)
{
    size_t n;
    ଅକ୍ଷର *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    ପ୍ରତିଜ୍ଞା(tstate != ଖାଲି);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    ଅଚଳ PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    ଯଦି (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        ଯଦି (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            ଯଦି (prompt) {
                ଯଦି (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            ଖାଲି, 0);
                    ଯଦି (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * ଆକାର(wchar_t));
                        ଯଦି (wbuf == ଖାଲି) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            ଫେରନ୍ତୁ ଖାଲି;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        ଯଦି (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, ଖାଲି);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } ଅନ୍ୟଥା {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            ଫେରନ୍ତୁ _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    ଯଦି (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = ଖାଲି;
    କର {
        size_t incr = (n > 0) ? n + 2 : 100;
        ଯଦି (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            ଫେରନ୍ତୁ ଖାଲି;
        }
        pr = (ଅକ୍ଷର *)PyMem_RawRealloc(p, n + incr);
        ଯଦି (pr == ଖାଲି) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            ଫେରନ୍ତୁ ଖାଲି;
        }
        p = pr;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = my_fgets(tstate, p + n, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)incr, sys_stdin);
        ଯଦି (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            ଫେରନ୍ତୁ ଖାଲି;
        } ଅନ୍ୟଥା ଯଦି (err != 0) {
            // EOF or error
            p[n] = '\0';
            ଭାଙ୍ଗ;
        }
        n += strlen(p + n);
    } ଯେତେବେଳେ (n == 0 || p[n-1] != '\n');

    pr = (ଅକ୍ଷର *)PyMem_RawRealloc(p, n+1);
    ଯଦି (pr == ଖାଲି) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

ଅକ୍ଷର *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, ଅଚଳ ଅକ୍ଷର *) = ଖାଲି;


ଅକ୍ଷର *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ଅଚଳ ଅକ୍ଷର *prompt)
{
    ଅକ୍ଷର *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    ଯଦି (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        ଫେରନ୍ତୁ ଖାଲି;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    ଯଦି (PyOS_ReadlineFunctionPointer == ଖାଲି) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    ଯଦି (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    ଅନ୍ୟଥା {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, ଖାଲି);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    ଯଦି (rv == ଖାଲି)
        ଫେରନ୍ତୁ ଖାଲି;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    ଯଦି (res != ଖାଲି) {
        memcpy(res, rv, len);
    }
    ଅନ୍ୟଥା {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    ଫେରନ୍ତୁ res;
}
