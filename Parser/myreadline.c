
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#include "Python.h"
#include "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#include "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#include "pycore_pystate.h"       // _PyThreadState_GET()
#include "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = NULL;

static PyMutex _PyOS_ReadlineLock;

ចំនួនគត់ (*PyOS_InputHook)(void) = NULL;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

static ចំនួនគត់
my_fgets(PyThreadState* tstate, var_na *buf, ចំនួនគត់ len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    ប្រសិនបើ (handle == INVALID_HANDLE_VALUE) {
        ត្រឡប់ -1; /* EOF */
    }
#endif

    ខណៈពេល (1) {
        ប្រសិនបើ (PyOS_InputHook != NULL &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (void)(PyOS_InputHook)();
        }

        errno = 0;
        clearerr(fp);
        var_na *p = fgets(buf, len, fp);
        ប្រសិនបើ (p != NULL) {
            ត្រឡប់ 0; /* No error */
        }
        ចំនួនគត់ err = errno;

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
        ប្រសិនបើ (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            switch (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            pak_sa WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                ត្រឡប់ 1; /* Interrupt */
            pak_sa WAIT_FAILED:
                ត្រឡប់ -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        ប្រសិនបើ (feof(fp)) {
            clearerr(fp);
            ត្រឡប់ -1; /* EOF */
        }

#ifdef EINTR
        ប្រសិនបើ (err == EINTR) {
            PyEval_RestoreThread(tstate);
            ចំនួនគត់ s = PyErr_CheckSignals();
            PyEval_SaveThread();

            ប្រសិនបើ (s < 0) {
                ត្រឡប់ 1;
            }
            /* try again */
            continue;
        }
#endif

        ប្រសិនបើ (_PyOS_InterruptOccurred(tstate)) {
            ត្រឡប់ 1; /* Interrupt */
        }
        ត្រឡប់ -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

extern var_na _get_console_type(HANDLE handle);

var_na *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    static wchar_t wbuf_local[1024 * 16];
    const DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    var_na *buf = NULL;
    ចំនួនគត់ err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = parimaa_na(wbuf_local) / parimaa_na(wbuf_local[0]) - 1;
    ខណៈពេល (1) {
        ប្រសិនបើ (PyOS_InputHook != NULL &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (void)(PyOS_InputHook)();
        }
        ប្រសិនបើ (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, NULL)) {
            err = GetLastError();
            goto exit;
        }
        ប្រសិនបើ (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            bha_nga;
        }
        ប្រសិនបើ (n_read == 0) {
            ចំនួនគត់ s;
            err = GetLastError();
            ប្រសិនបើ (err != ERROR_OPERATION_ABORTED)
                goto exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            ប្រសិនបើ (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                ប្រសិនបើ (s < 0) {
                    goto exit;
                }
            }
            បើមិនដូច្នេះ ប្រសិនបើ (state != WAIT_TIMEOUT) {
                err = GetLastError();
                goto exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            continue;
        }

        total_read += n_read;
        ប្រសិនបើ (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            bha_nga;
        }
        wbuflen += chunk_size;
        ប្រសិនបើ (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * parimaa_na(wchar_t));
            ប្រសិនបើ (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            បើមិនដូច្នេះ {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                goto exit;
            }
        }
        បើមិនដូច្នេះ {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * parimaa_na(wchar_t));
            ប្រសិនបើ (tmp == NULL) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                goto exit;
            }
            wbuf = tmp;
        }
    }

    ប្រសិនបើ (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        ប្រសិនបើ (buf) {
            buf[0] = '\0';
        }
        បើមិនដូច្នេះ {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        goto exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                NULL, 0,
                                NULL, NULL);
    buf = PyMem_RawMalloc(u8len + 1);
    ប្រសិនបើ (buf == NULL) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        goto exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                NULL, NULL);
    buf[u8len] = '\0';

exit:
    ប្រសិនបើ (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    ប្រសិនបើ (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    ត្រឡប់ buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

var_na *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, const var_na *prompt)
{
    size_t n;
    var_na *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    assert(tstate != NULL);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    const PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    ប្រសិនបើ (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        ប្រសិនបើ (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            ប្រសិនបើ (prompt) {
                ប្រសិនបើ (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    ចំនួនគត់ wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            NULL, 0);
                    ប្រសិនបើ (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * parimaa_na(wchar_t));
                        ប្រសិនបើ (wbuf == NULL) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            ត្រឡប់ NULL;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        ប្រសិនបើ (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, NULL);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } បើមិនដូច្នេះ {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            ត្រឡប់ _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    ប្រសិនបើ (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = NULL;
    do {
        size_t incr = (n > 0) ? n + 2 : 100;
        ប្រសិនបើ (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            ត្រឡប់ NULL;
        }
        pr = (var_na *)PyMem_RawRealloc(p, n + incr);
        ប្រសិនបើ (pr == NULL) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            ត្រឡប់ NULL;
        }
        p = pr;
        ចំនួនគត់ err = my_fgets(tstate, p + n, (ចំនួនគត់)incr, sys_stdin);
        ប្រសិនបើ (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            ត្រឡប់ NULL;
        } បើមិនដូច្នេះ ប្រសិនបើ (err != 0) {
            // EOF or error
            p[n] = '\0';
            bha_nga;
        }
        n += strlen(p + n);
    } ខណៈពេល (n == 0 || p[n-1] != '\n');

    pr = (var_na *)PyMem_RawRealloc(p, n+1);
    ប្រសិនបើ (pr == NULL) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        ត្រឡប់ NULL;
    }
    ត្រឡប់ pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

var_na *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, const var_na *) = NULL;


var_na *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, const var_na *prompt)
{
    var_na *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    ប្រសិនបើ (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        ត្រឡប់ NULL;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    ប្រសិនបើ (PyOS_ReadlineFunctionPointer == NULL) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    ប្រសិនបើ (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    បើមិនដូច្នេះ {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, NULL);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    ប្រសិនបើ (rv == NULL)
        ត្រឡប់ NULL;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    ប្រសិនបើ (res != NULL) {
        memcpy(res, rv, len);
    }
    បើមិនដូច្នេះ {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    ត្រឡប់ res;
}
