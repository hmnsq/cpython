
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#அந்தர்பாவ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#அந்தர்பாவ "pycore_pystate.h"       // _PyThreadState_GET()
#அந்தர்பாவ "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = வெற்று;

நிலையான PyMutex _PyOS_ReadlineLock;

முழு (*Py_OS_உள்ளீடு_Hook)(வெறுமை) = வெற்று;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

நிலையான முழு
my_fgets(PyThreadState* tstate, எழுத்து *buf, முழு len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    எனில் (handle == INVALID_HANDLE_மதிப்பு) {
        திரும்பு -1; /* EOF */
    }
#endif

    போது (1) {
        எனில் (Py_OS_உள்ளீடு_Hook != வெற்று &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (வெறுமை)(Py_OS_உள்ளீடு_Hook)();
        }

        errno = 0;
        clearerr(fp);
        எழுத்து *p = fgets(buf, len, fp);
        எனில் (p != வெற்று) {
            திரும்பு 0; /* No error */
        }
        முழு err = errno;

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
        எனில் (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            தேர்வு (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            நிலை WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                திரும்பு 1; /* Interrupt */
            நிலை WAIT_FAILED:
                திரும்பு -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        எனில் (feof(fp)) {
            clearerr(fp);
            திரும்பு -1; /* EOF */
        }

#ifdef EINTR
        எனில் (err == EINTR) {
            PyEval_RestoreThread(tstate);
            முழு s = PyErr_CheckSignals();
            PyEval_SaveThread();

            எனில் (s < 0) {
                திரும்பு 1;
            }
            /* try again */
            தொடர்;
        }
#endif

        எனில் (_PyOS_InterruptOccurred(tstate)) {
            திரும்பு 1; /* Interrupt */
        }
        திரும்பு -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

வெளி எழுத்து _get_console_type(HANDLE handle);

எழுத்து *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    நிலையான wchar_t wbuf_local[1024 * 16];
    மாறிலி DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    எழுத்து *buf = வெற்று;
    முழு err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = அளவு(wbuf_local) / அளவு(wbuf_local[0]) - 1;
    போது (1) {
        எனில் (Py_OS_உள்ளீடு_Hook != வெற்று &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (வெறுமை)(Py_OS_உள்ளீடு_Hook)();
        }
        எனில் (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, வெற்று)) {
            err = GetLastError();
            செல் exit;
        }
        எனில் (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            நிறுத்து;
        }
        எனில் (n_read == 0) {
            முழு s;
            err = GetLastError();
            எனில் (err != ERROR_OPERATION_ABORTED)
                செல் exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            எனில் (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                எனில் (s < 0) {
                    செல் exit;
                }
            }
            இல்லையெனில் எனில் (state != WAIT_TIMEOUT) {
                err = GetLastError();
                செல் exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            தொடர்;
        }

        total_read += n_read;
        எனில் (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            நிறுத்து;
        }
        wbuflen += chunk_size;
        எனில் (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * அளவு(wchar_t));
            எனில் (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            இல்லையெனில் {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                செல் exit;
            }
        }
        இல்லையெனில் {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * அளவு(wchar_t));
            எனில் (tmp == வெற்று) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                செல் exit;
            }
            wbuf = tmp;
        }
    }

    எனில் (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        எனில் (buf) {
            buf[0] = '\0';
        }
        இல்லையெனில் {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        செல் exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                வெற்று, 0,
                                வெற்று, வெற்று);
    buf = PyMem_RawMalloc(u8len + 1);
    எனில் (buf == வெற்று) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        செல் exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                வெற்று, வெற்று);
    buf[u8len] = '\0';

exit:
    எனில் (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    எனில் (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    திரும்பு buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

எழுத்து *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, மாறிலி எழுத்து *prompt)
{
    size_t n;
    எழுத்து *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    ப்ரதிஜ்ஞா(tstate != வெற்று);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    மாறிலி PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    எனில் (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        எனில் (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            எனில் (prompt) {
                எனில் (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    முழு wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            வெற்று, 0);
                    எனில் (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * அளவு(wchar_t));
                        எனில் (wbuf == வெற்று) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            திரும்பு வெற்று;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        எனில் (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, வெற்று);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } இல்லையெனில் {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            திரும்பு _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    எனில் (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = வெற்று;
    செய் {
        size_t incr = (n > 0) ? n + 2 : 100;
        எனில் (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            திரும்பு வெற்று;
        }
        pr = (எழுத்து *)PyMem_RawRealloc(p, n + incr);
        எனில் (pr == வெற்று) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            திரும்பு வெற்று;
        }
        p = pr;
        முழு err = my_fgets(tstate, p + n, (முழு)incr, sys_stdin);
        எனில் (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            திரும்பு வெற்று;
        } இல்லையெனில் எனில் (err != 0) {
            // EOF or error
            p[n] = '\0';
            நிறுத்து;
        }
        n += strlen(p + n);
    } போது (n == 0 || p[n-1] != '\n');

    pr = (எழுத்து *)PyMem_RawRealloc(p, n+1);
    எனில் (pr == வெற்று) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        திரும்பு வெற்று;
    }
    திரும்பு pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

எழுத்து *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, மாறிலி எழுத்து *) = வெற்று;


எழுத்து *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, மாறிலி எழுத்து *prompt)
{
    எழுத்து *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    எனில் (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        திரும்பு வெற்று;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    எனில் (PyOS_ReadlineFunctionPointer == வெற்று) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    எனில் (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    இல்லையெனில் {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, வெற்று);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    எனில் (rv == வெற்று)
        திரும்பு வெற்று;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    எனில் (res != வெற்று) {
        memcpy(res, rv, len);
    }
    இல்லையெனில் {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    திரும்பு res;
}
