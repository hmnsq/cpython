
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

पूर्णांक (*Py_OS_आदान_Hook)(शून्य) = रिक्त;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

स्थिर पूर्णांक
my_fgets(PyThreadState* tstate, अक्षर *buf, पूर्णांक len, खाता *fp)
{
#ifdef MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    _Py_END_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    जर (handle == INVALID_HANDLE_मूल्य) {
        परत -1; /* EOF */
    }
#endif

    पर्यंत (1) {
        जर (Py_OS_आदान_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_आदान_Hook)();
        }

        त्रुटि_सं = 0;
        साफ_त्रुटि(fp);
        अक्षर *p = ख_माला_लो(buf, len, fp);
        जर (p != रिक्त) {
            परत 0; /* No error */
        }
        पूर्णांक err = त्रुटि_सं;

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
        जर (GetLastError()==ERROR_OPERATION_ABORTED) {
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            निवड (WaitForSingleObjectEx(hInterruptEvent, 10, FALSE)) {
            स्थिती WAIT_OBJECT_0:
                ResetEvent(hInterruptEvent);
                परत 1; /* Interrupt */
            स्थिती WAIT_FAILED:
                परत -2; /* Error */
            }
        }
#endif /* MS_WINDOWS */

        जर (ख_पूर्ण(fp)) {
            साफ_त्रुटि(fp);
            परत -1; /* EOF */
        }

#ifdef EINTR
        जर (err == EINTR) {
            PyEval_RestoreThread(tstate);
            पूर्णांक s = PyErr_CheckSignals();
            PyEval_SaveThread();

            जर (s < 0) {
                परत 1;
            }
            /* try again */
            सुरूठेवा;
        }
#endif

        जर (_PyOS_InterruptOccurred(tstate)) {
            परत 1; /* Interrupt */
        }
        परत -2; /* Error */
    }
    /* NOTREACHED */
}

#ifdef HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

बाह्य अक्षर _get_console_type(HANDLE handle);

अक्षर *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE hStdIn)
{
    स्थिर ब_अक्षर_प्रकार wbuf_local[1024 * 16];
    स्थिरांक DWORD chunk_size = 1024;

    DWORD n_read, total_read, wbuflen, u8len;
    ब_अक्षर_प्रकार *wbuf;
    अक्षर *buf = रिक्त;
    पूर्णांक err = 0;

    n_read = (DWORD)-1;
    total_read = 0;
    wbuf = wbuf_local;
    wbuflen = आकार(wbuf_local) / आकार(wbuf_local[0]) - 1;
    पर्यंत (1) {
        जर (Py_OS_आदान_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_आदान_Hook)();
        }
        जर (!ReadConsoleW(hStdIn, &wbuf[total_read], wbuflen - total_read, &n_read, रिक्त)) {
            err = GetLastError();
            जा निकास;
        }
        जर (n_read == (DWORD)-1 && (err = GetLastError()) == ERROR_OPERATION_ABORTED) {
            थांबा;
        }
        जर (n_read == 0) {
            पूर्णांक s;
            err = GetLastError();
            जर (err != ERROR_OPERATION_ABORTED)
                जा निकास;
            err = 0;
            HANDLE hInterruptEvent = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(hInterruptEvent, 100, FALSE);
            जर (state == WAIT_OBJECT_0) {
                ResetEvent(hInterruptEvent);
                PyEval_RestoreThread(tstate);
                s = PyErr_CheckSignals();
                PyEval_SaveThread();
                जर (s < 0) {
                    जा निकास;
                }
            }
            अन्यथा जर (state != WAIT_TIMEOUT) {
                err = GetLastError();
                जा निकास;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            सुरूठेवा;
        }

        total_read += n_read;
        जर (total_read == 0 || wbuf[total_read - 1] == L'\n') {
            थांबा;
        }
        wbuflen += chunk_size;
        जर (wbuf == wbuf_local) {
            wbuf[total_read] = '\0';
            wbuf = (ब_अक्षर_प्रकार*)PyMem_RawMalloc(wbuflen * आकार(ब_अक्षर_प्रकार));
            जर (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            अन्यथा {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                जा निकास;
            }
        }
        अन्यथा {
            ब_अक्षर_प्रकार *tmp = PyMem_RawRealloc(wbuf, wbuflen * आकार(ब_अक्षर_प्रकार));
            जर (tmp == रिक्त) {
                PyEval_RestoreThread(tstate);
                PyErr_NoMemory();
                PyEval_SaveThread();
                जा निकास;
            }
            wbuf = tmp;
        }
    }

    जर (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        जर (buf) {
            buf[0] = '\0';
        }
        अन्यथा {
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
        }
        जा निकास;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                रिक्त, 0,
                                रिक्त, रिक्त);
    buf = PyMem_RawMalloc(u8len + 1);
    जर (buf == रिक्त) {
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        जा निकास;
    }

    u8len = WideCharToMultiByte(CP_UTF8, 0,
                                wbuf, total_read,
                                buf, u8len,
                                रिक्त, रिक्त);
    buf[u8len] = '\0';

निकास:
    जर (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    जर (err) {
        PyEval_RestoreThread(tstate);
        PyErr_SetFromWindowsErr(err);
        PyEval_SaveThread();
    }
    परत buf;
}

#endif /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

अक्षर *
PyOS_StdioReadline(खाता *sys_stdin, खाता *sys_stdout, स्थिरांक अक्षर *prompt)
{
    माप_प्रकार n;
    अक्षर *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    प्रतिज्ञा(tstate != रिक्त);

#ifdef HAVE_WINDOWS_CONSOLE_IO
    स्थिरांक PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    जर (!config->legacy_windows_stdio && sys_stdin == मानक_निवेश) {
        HANDLE hStdIn, hStdErr;

        hStdIn = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(मानक_त्रुटि));

        जर (_get_console_type(hStdIn) == 'r') {
            ख_साफ(sys_stdout);
            जर (prompt) {
                जर (_get_console_type(hStdErr) == 'w') {
                    ब_अक्षर_प्रकार *wbuf;
                    पूर्णांक wlen;
                    wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                            रिक्त, 0);
                    जर (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * आकार(ब_अक्षर_प्रकार));
                        जर (wbuf == रिक्त) {
                            PyEval_RestoreThread(tstate);
                            PyErr_NoMemory();
                            PyEval_SaveThread();
                            परत रिक्त;
                        }
                        wlen = MultiByteToWideChar(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        जर (wlen) {
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
            परत _PyOS_WindowsConsoleReadline(tstate, hStdIn);
        }
    }
#endif

    ख_साफ(sys_stdout);
    जर (prompt) {
        ख_लिखो(मानक_त्रुटि, "%s", prompt);
    }
    ख_साफ(मानक_त्रुटि);

    n = 0;
    p = रिक्त;
    करा {
        माप_प्रकार incr = (n > 0) ? n + 2 : 100;
        जर (incr > पूर्णांक_उच्च) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_SetString(PyExc_OverflowError, "input line too long");
            PyEval_SaveThread();
            परत रिक्त;
        }
        pr = (अक्षर *)PyMem_RawRealloc(p, n + incr);
        जर (pr == रिक्त) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            PyErr_NoMemory();
            PyEval_SaveThread();
            परत रिक्त;
        }
        p = pr;
        पूर्णांक err = my_fgets(tstate, p + n, (पूर्णांक)incr, sys_stdin);
        जर (err == 1) {
            // Interrupt
            PyMem_RawFree(p);
            परत रिक्त;
        } अन्यथा जर (err != 0) {
            // EOF or error
            p[n] = '\0';
            थांबा;
        }
        n += म_माप(p + n);
    } पर्यंत (n == 0 || p[n-1] != '\n');

    pr = (अक्षर *)PyMem_RawRealloc(p, n+1);
    जर (pr == रिक्त) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        PyErr_NoMemory();
        PyEval_SaveThread();
        परत रिक्त;
    }
    परत pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

अक्षर *(*PyOS_ReadlineFunctionPointer)(खाता *, खाता *, स्थिरांक अक्षर *) = रिक्त;


अक्षर *
PyOS_Readline(खाता *sys_stdin, खाता *sys_stdout, स्थिरांक अक्षर *prompt)
{
    अक्षर *rv, *res;
    माप_प्रकार len;

    PyThreadState *tstate = _PyThreadState_GET();
    जर (_Py_atomic_load_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        PyErr_SetString(PyExc_RuntimeError,
                        "can't re-enter readline");
        परत रिक्त;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_THREADS
    PyMutex_Lock(&_PyOS_ReadlineLock);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    जर (PyOS_ReadlineFunctionPointer == रिक्त) {
        PyOS_ReadlineFunctionPointer = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    जर (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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

    जर (rv == रिक्त)
        परत रिक्त;

    len = म_माप(rv) + 1;
    res = PyMem_Malloc(len);
    जर (res != रिक्त) {
        स_नकल(res, rv, len);
    }
    अन्यथा {
        PyErr_NoMemory();
    }
    PyMem_RawFree(rv);

    परत res;
}
