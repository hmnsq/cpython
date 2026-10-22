
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#അന്തര്ഭാവ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#അന്തര്ഭാവ "pycore_pystate.h"       // _PyThreadState_GET()
#അന്തര്ഭാവ "pycore_signal.h"        // _PyOS_SigintEvent()
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
PyThreadState *_PyOS_ReadlineTState = ശൂന്യ;

സ്ഥിതം PyMutex _PyOS_ReadlineLock;

പൂർണ്ണസംഖ്യ (*Py_OS_നിവേശം_Hook)(ശൂന്യം) = ശൂന്യ;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

സ്ഥിതം പൂർണ്ണസംഖ്യ
my_fgets(PyThreadState* tstate, അക്ഷരം *buf, പൂർണ്ണസംഖ്യ len, FILE *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    എങ്കിൽ (handle == INVALID_HANDLE_മൂല്യം) {
        തിരികെ -1; /* EOF */
    }
#endif

    ആകുമ്പോൾ (1) {
        എങ്കിൽ (Py_OS_നിവേശം_Hook != ശൂന്യ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ശൂന്യം)(Py_OS_നിവേശം_Hook)();
        }

        errno = 0;
        clearerr(fp);
        അക്ഷരം *p = fgets(buf, len, fp);
        എങ്കിൽ (p != ശൂന്യ) {
            തിരികെ 0; /* No error */
        }
        പൂർണ്ണസംഖ്യ err = errno;

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
        എങ്കിൽ (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            തിരഞ്ഞെടുപ്പ് (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            അവസ്ഥ WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                തിരികെ 1; /* Interrupt */
            അവസ്ഥ WAIT_FAILED:
                തിരികെ -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        എങ്കിൽ (feof(fp)) {
            clearerr(fp);
            തിരികെ -1; /* EOF */
        }

#ifdef EINTR
        എങ്കിൽ (err == EINTR) {
            PyEval_RestoreThread(tstate);
            പൂർണ്ണസംഖ്യ s = PyErr_CheckSignals();
            PyEval_SaveThread();

            എങ്കിൽ (s < 0) {
                തിരികെ 1;
            }
            /* try again */
            തുടരുക;
        }
#endif

        എങ്കിൽ (_PyOS_InterruptOccurred(tstate)) {
            തിരികെ 1; /* Interrupt */
        }
        തിരികെ -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

ബാഹ്യ അക്ഷരം _get_console_type(HANDLE handle);

അക്ഷരം *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    സ്ഥിതം wchar_t wbuf_local[1024 * 16];
    സ്ഥിരം DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    wchar_t *wbuf;
    അക്ഷരം *buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = വലുപ്പം(wbuf_local) / വലുപ്പം(wbuf_local[0]) - 1;
    ആകുമ്പോൾ (1) {
        എങ്കിൽ (Py_OS_നിവേശം_Hook != ശൂന്യ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (ശൂന്യം)(Py_OS_നിവേശം_Hook)();
        }
        എങ്കിൽ (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, ശൂന്യ)) {
            err = GetLastError();
            പോകുക exit;
        }
        എങ്കിൽ (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            നിർത്തുക;
        }
        എങ്കിൽ (n_read == 0) {
            പൂർണ്ണസംഖ്യ s;
            err = GetLastError();
            എങ്കിൽ (err != ERROR_OPERATION_ABORTED)
                പോകുക exit;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            എങ്കിൽ (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                എങ്കിൽ (s < 0) {
                    പോകുക exit;
                }
            }
            അല്ലെങ്കിൽ എങ്കിൽ (state != WAIT_TIMEOUT) {
                err = GetLastError();
                പോകുക exit;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            തുടരുക;
        }

        total_read += n_read;
        എങ്കിൽ (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            നിർത്തുക;
        }
        wbuflen += chunk_size;
        എങ്കിൽ (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * വലുപ്പം(wchar_t));
            എങ്കിൽ (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            അല്ലെങ്കിൽ {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                പോകുക exit;
            }
        }
        അല്ലെങ്കിൽ {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * വലുപ്പം(wchar_t));
            എങ്കിൽ (tmp == ശൂന്യ) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                പോകുക exit;
            }
            wbuf = tmp;
        }
    }

    എങ്കിൽ (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        എങ്കിൽ (buf) {
            buf[0] = '\0';
        }
        അല്ലെങ്കിൽ {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        പോകുക exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                ശൂന്യ, 0,
                                ശൂന്യ, ശൂന്യ);
    buf = PyMem_RawMalloc(u8len + 1);
    എങ്കിൽ (buf == ശൂന്യ) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        പോകുക exit;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                ശൂന്യ, ശൂന്യ);
    buf[u8len] = '\0';

exit:
    എങ്കിൽ (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    എങ്കിൽ (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    തിരികെ buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

അക്ഷരം *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, സ്ഥിരം അക്ഷരം *prompt)
{
    size_t n;
    അക്ഷരം *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    പ്രതിജ്ഞാ(tstate != ശൂന്യ);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    സ്ഥിരം PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    എങ്കിൽ (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        എങ്കിൽ (_get_console_type(hStdIn) == 'r') {
            fflush(sys_stdout);
            എങ്കിൽ (prompt) {
                എങ്കിൽ (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    പൂർണ്ണസംഖ്യ wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            ശൂന്യ, 0);
                    എങ്കിൽ (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * വലുപ്പം(wchar_t));
                        എങ്കിൽ (wbuf == ശൂന്യ) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            തിരികെ ശൂന്യ;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        എങ്കിൽ (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, ശൂന്യ);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } അല്ലെങ്കിൽ {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            തിരികെ _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    fflush(sys_stdout);
    എങ്കിൽ (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = ശൂന്യ;
    ചെയ്യുക {
        size_t incr = (n > 0) ? n + 2 : 100;
        എങ്കിൽ (incr > INT_MAX) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            തിരികെ ശൂന്യ;
        }
        pr = (അക്ഷരം *)PyMem_RawRealloc(p, n + incr);
        എങ്കിൽ (pr == ശൂന്യ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            തിരികെ ശൂന്യ;
        }
        p = pr;
        പൂർണ്ണസംഖ്യ err = my_fgets(tstate, p + n, (പൂർണ്ണസംഖ്യ)incr, sys_stdin);
        എങ്കിൽ (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            തിരികെ ശൂന്യ;
        } അല്ലെങ്കിൽ എങ്കിൽ (err != 0) {
            // EOF or error
            p[n] = '\0';
            നിർത്തുക;
        }
        n += strlen(p + n);
    } ആകുമ്പോൾ (n == 0 || p[n-1] != '\n');

    pr = (അക്ഷരം *)PyMem_RawRealloc(p, n+1);
    എങ്കിൽ (pr == ശൂന്യ) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        തിരികെ ശൂന്യ;
    }
    തിരികെ pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

അക്ഷരം *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, സ്ഥിരം അക്ഷരം *) = ശൂന്യ;


അക്ഷരം *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, സ്ഥിരം അക്ഷരം *prompt)
{
    അക്ഷരം *rv, *res;
    size_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    എങ്കിൽ (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        തിരികെ ശൂന്യ;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    എങ്കിൽ (PyOS_ReadlineFunctionPointer == ശൂന്യ) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    എങ്കിൽ (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    അല്ലെങ്കിൽ {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, ശൂന്യ);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    എങ്കിൽ (rv == ശൂന്യ)
        തിരികെ ശൂന്യ;

    len = strlen(rv) + 1;
    res = PyMem_Malloc(len);
    എങ്കിൽ (res != ശൂന്യ) {
        memcpy(res, rv, len);
    }
    അല്ലെങ്കിൽ {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    തിരികെ res;
}
