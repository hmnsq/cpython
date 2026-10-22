
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

पूर्णाङ्क (*Py_OS_निवेश_Hook)(शून्य) = रिक्त;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

स्थिर पूर्णाङ्क
my_fgets(PyThreadState* tstate, वर्ण *buf, पूर्णाङ्क len, खाता *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    यदि (handle == INVALID_HANDLE_मूल्य) {
        प्रत्यागम -1; /* EOF */
    }
#endif

    यावत् (1) {
        यदि (Py_OS_निवेश_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_निवेश_Hook)();
        }

        त्रुटि_सं = 0;
        साफ_त्रुटि(fp);
        वर्ण *p = ख_माला_लो(buf, len, fp);
        यदि (p != रिक्त) {
            प्रत्यागम 0; /* No error */
        }
        पूर्णाङ्क err = त्रुटि_सं;

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
        यदि (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            विकल्प (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            पक्ष WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                प्रत्यागम 1; /* Interrupt */
            पक्ष WAIT_FAILED:
                प्रत्यागम -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        यदि (ख_पूर्ण(fp)) {
            साफ_त्रुटि(fp);
            प्रत्यागम -1; /* EOF */
        }

#ifdef EINTR
        यदि (err == EINTR) {
            PyEval_RestoreThread(tstate);
            पूर्णाङ्क s = PyErr_CheckSignals();
            PyEval_SaveThread();

            यदि (s < 0) {
                प्रत्यागम 1;
            }
            /* try again */
            अनुवर्त;
        }
#endif

        यदि (_PyOS_InterruptOccurred(tstate)) {
            प्रत्यागम 1; /* Interrupt */
        }
        प्रत्यागम -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

बाह्य वर्ण _get_console_type(HANDLE handle);

वर्ण *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    स्थिर ब_अक्षर_प्रकार wbuf_local[1024 * 16];
    ध्रुव DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    ब_अक्षर_प्रकार *wbuf;
    वर्ण *buf = रिक्त;
    पूर्णाङ्क err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = परिमाण(wbuf_local) / परिमाण(wbuf_local[0]) - 1;
    यावत् (1) {
        यदि (Py_OS_निवेश_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_निवेश_Hook)();
        }
        यदि (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, रिक्त)) {
            err = GetLastError();
            गच्छ निकास;
        }
        यदि (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            भङ्ग;
        }
        यदि (n_read == 0) {
            पूर्णाङ्क s;
            err = GetLastError();
            यदि (err != ERROR_OPERATION_ABORTED)
                गच्छ निकास;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            यदि (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                यदि (s < 0) {
                    गच्छ निकास;
                }
            }
            अन्यथा यदि (state != WAIT_TIMEOUT) {
                err = GetLastError();
                गच्छ निकास;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            अनुवर्त;
        }

        total_read += n_read;
        यदि (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            भङ्ग;
        }
        wbuflen += chunk_size;
        यदि (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (ब_अक्षर_प्रकार*)PyMem_RawMalloc(wbuflen * परिमाण(ब_अक्षर_प्रकार));
            यदि (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            अन्यथा {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                गच्छ निकास;
            }
        }
        अन्यथा {
            ब_अक्षर_प्रकार *tmp = PyMem_RawRealloc(wbuf, wbuflen * परिमाण(ब_अक्षर_प्रकार));
            यदि (tmp == रिक्त) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                गच्छ निकास;
            }
            wbuf = tmp;
        }
    }

    यदि (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        यदि (buf) {
            buf[0] = '\0';
        }
        अन्यथा {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        गच्छ निकास;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                रिक्त, 0,
                                रिक्त, रिक्त);
    buf = PyMem_RawMalloc(u8len + 1);
    यदि (buf == रिक्त) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        गच्छ निकास;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                रिक्त, रिक्त);
    buf[u8len] = '\0';

निकास:
    यदि (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    यदि (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    प्रत्यागम buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

वर्ण *
PyOS_StdioReadline(खाता *sys_stdin, खाता *sys_stdout, ध्रुव वर्ण *prompt)
{
    माप_प्रकार n;
    वर्ण *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    प्रतिज्ञा(tstate != रिक्त);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    ध्रुव PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    यदि (!config->legacy_windows_stdio && sys_stdin == मानक_निवेश) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(मानक_त्रुटि));

        यदि (_get_console_type(hStdIn) == 'r') {
            ख_साफ(sys_stdout);
            यदि (prompt) {
                यदि (_get_console_type(hStdErr) == 'w') {
                    ब_अक्षर_प्रकार *wbuf;
                    पूर्णाङ्क wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            रिक्त, 0);
                    यदि (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * परिमाण(ब_अक्षर_प्रकार));
                        यदि (wbuf == रिक्त) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            प्रत्यागम रिक्त;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        यदि (wlen) {
                            DWORD n;
                            ख_साफ(मानक_त्रुटि);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, रिक्त);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } अन्यथा {
                    ख_लिखो(मानक_त्रुटि, "%s", prompt);
                    ख_साफ(मानक_त्रुटि);
                }
            }
            साफ_त्रुटि(sys_stdin);
            प्रत्यागम _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    ख_साफ(sys_stdout);
    यदि (prompt) {
        ख_लिखो(मानक_त्रुटि, "%s", prompt);
    }
    ख_साफ(मानक_त्रुटि);

    n = 0;
    p = रिक्त;
    कुरु {
        माप_प्रकार incr = (n > 0) ? n + 2 : 100;
        यदि (incr > पूर्णांक_उच्च) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            प्रत्यागम रिक्त;
        }
        pr = (वर्ण *)PyMem_RawRealloc(p, n + incr);
        यदि (pr == रिक्त) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            प्रत्यागम रिक्त;
        }
        p = pr;
        पूर्णाङ्क err = my_fgets(tstate, p + n, (पूर्णाङ्क)incr, sys_stdin);
        यदि (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            प्रत्यागम रिक्त;
        } अन्यथा यदि (err != 0) {
            // EOF or error
            p[n] = '\0';
            भङ्ग;
        }
        n += म_माप(p + n);
    } यावत् (n == 0 || p[n-1] != '\n');

    pr = (वर्ण *)PyMem_RawRealloc(p, n+1);
    यदि (pr == रिक्त) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        प्रत्यागम रिक्त;
    }
    प्रत्यागम pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

वर्ण *(*PyOS_ReadlineFunctionPointer)(खाता *, खाता *, ध्रुव वर्ण *) = रिक्त;


वर्ण *
PyOS_Readline(खाता *sys_stdin, खाता *sys_stdout, ध्रुव वर्ण *prompt)
{
    वर्ण *rv, *res;
    माप_प्रकार len;

    PyThreadState *tstate = _PyThreadState_GET();
    यदि (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        प्रत्यागम रिक्त;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    यदि (PyOS_ReadlineFunctionPointer == रिक्त) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    यदि (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    अन्यथा {
        rv = (*PyOS_ReadlineFunctionPointer)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, रिक्त);
    PyMutex_Unlock(&_PyOS_ReadlineLock);
    Py_END_ALLOW_THREADS

    यदि (rv == रिक्त)
        प्रत्यागम रिक्त;

    len = म_माप(rv) + 1;
    res = PyMem_Malloc(len);
    यदि (res != रिक्त) {
        स_नकल(res, rv, len);
    }
    अन्यथा {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    प्रत्यागम res;
}
