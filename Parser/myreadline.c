
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
#यदि_परिभाषितम् MS_WINDOWS
#  ifndef WIN32_LEAN_AND_माध्यम्
#    define WIN32_LEAN_AND_माध्यम्
#  endif
#  include "windows.h"
#यदि_अन्तः /* MS_WINDOWS */

#यदि_परिभाषितम् HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#यदि_अन्तः


// Export the symbol since it's used by the readline shared extension
Py_API_दत्त(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = रिक्त;

स्थिर PyMutex Py_OS_Readline_तालकम्;

पूर्णाङ्क (*Py_OS_निवेशः_Hook)(शून्य) = रिक्त;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

स्थिर पूर्णाङ्क
my_fgets(PyThreadState* tstate, वर्ण *buf, पूर्णाङ्क len, FILE *fp)
{
#यदि_परिभाषितम् MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    Py_अन्तः_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    यदि (handle == INVALID_HANDLE_मानम्) {
        प्रत्यागम -1; /* EOF */
    }
#यदि_अन्तः

    यावत् (1) {
        यदि (Py_OS_निवेशः_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_निवेशः_Hook)();
        }

        errno = 0;
        clearerr(fp);
        वर्ण *p = fgets(buf, len, fp);
        यदि (p != रिक्त) {
            प्रत्यागम 0; /* No error */
        }
        पूर्णाङ्क err = errno;

#यदि_परिभाषितम् MS_WINDOWS
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
        यदि (Get_चरमः_Error()==ERROR_OPERATION_ABORTED) {
            HANDLE h_व्यवधानम्_Event = _PyOS_SigintEvent();
            विकल्प (WaitForSingleObjectEx(h_व्यवधानम्_Event, 10, FALSE)) {
            पक्ष WAIT_OBJECT_0:
                ResetEvent(h_व्यवधानम्_Event);
                प्रत्यागम 1; /*व्यवधानम्*/
            पक्ष WAIT_FAILED:
                प्रत्यागम -2; /* Error */
            }
        }
#यदि_अन्तः /* MS_WINDOWS */

        यदि (feof(fp)) {
            clearerr(fp);
            प्रत्यागम -1; /* EOF */
        }

#यदि_परिभाषितम् EINTR
        यदि (err == EINTR) {
            PyEval_RestoreThread(tstate);
            पूर्णाङ्क s = Py_Err_Check_सङ्केताः();
            PyEval_SaveThread();

            यदि (s < 0) {
                प्रत्यागम 1;
            }
            /* try again */
            अनुवर्त;
        }
#यदि_अन्तः

        यदि (Py_OS_व्यवधानम्_Occurred(tstate)) {
            प्रत्यागम 1; /*व्यवधानम्*/
        }
        प्रत्यागम -2; /* Error */
    }
    /* NOTREACHED */
}

#यदि_परिभाषितम् HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

बाह्य वर्ण _get_console_type(HANDLE handle);

वर्ण *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE h_Std_अन्तर्)
{
    स्थिर wchar_t wbuf_local[1024 * 16];
    ध्रुव DWORD chunk_आकार = 1024;

    DWORD n_read, योग_read, wbuflen, u8len;
    wchar_t *wbuf;
    वर्ण *buf = रिक्त;
    पूर्णाङ्क err = 0;

    n_read = (DWORD)-1;
    योग_read = 0;
    wbuf = wbuf_local;
    wbuflen = परिमाण(wbuf_local) / परिमाण(wbuf_local[0]) - 1;
    यावत् (1) {
        यदि (Py_OS_निवेशः_Hook != रिक्त &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (शून्य)(Py_OS_निवेशः_Hook)();
        }
        यदि (!ReadConsoleW(h_Std_अन्तर्, &wbuf[योग_read], wbuflen - योग_read, &n_read, रिक्त)) {
            err = Get_चरमः_Error();
            गच्छ निर्गच्छ;
        }
        यदि (n_read == (DWORD)-1 && (err = Get_चरमः_Error()) == ERROR_OPERATION_ABORTED) {
            भङ्ग;
        }
        यदि (n_read == 0) {
            पूर्णाङ्क s;
            err = Get_चरमः_Error();
            यदि (err != ERROR_OPERATION_ABORTED)
                गच्छ निर्गच्छ;
            err = 0;
            HANDLE h_व्यवधानम्_Event = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(h_व्यवधानम्_Event, 100, FALSE);
            यदि (state == WAIT_OBJECT_0) {
                ResetEvent(h_व्यवधानम्_Event);
                PyEval_RestoreThread(tstate);
                s = Py_Err_Check_सङ्केताः();
                PyEval_SaveThread();
                यदि (s < 0) {
                    गच्छ निर्गच्छ;
                }
            }
            अन्यथा यदि (state != WAIT_TIMEOUT) {
                err = Get_चरमः_Error();
                गच्छ निर्गच्छ;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            अनुवर्त;
        }

        योग_read += n_read;
        यदि (योग_read == 0 || wbuf[योग_read - 1] == L'\n') {
            भङ्ग;
        }
        wbuflen += chunk_आकार;
        यदि (wbuf == wbuf_local) {
            wbuf[योग_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * परिमाण(wchar_t));
            यदि (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            अन्यथा {
                PyEval_RestoreThread(tstate);
                Py_Err_No_स्मृतिः();
                PyEval_SaveThread();
                गच्छ निर्गच्छ;
            }
        }
        अन्यथा {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * परिमाण(wchar_t));
            यदि (tmp == रिक्त) {
                PyEval_RestoreThread(tstate);
                Py_Err_No_स्मृतिः();
                PyEval_SaveThread();
                गच्छ निर्गच्छ;
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
            Py_Err_No_स्मृतिः();
            PyEval_SaveThread();
        }
        गच्छ निर्गच्छ;
    }

    u8len = Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0,
                                wbuf, योग_read,
                                रिक्त, 0,
                                रिक्त, रिक्त);
    buf = PyMem_RawMalloc(u8len + 1);
    यदि (buf == रिक्त) {
        PyEval_RestoreThread(tstate);
        Py_Err_No_स्मृतिः();
        PyEval_SaveThread();
        गच्छ निर्गच्छ;
    }

    u8len = Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0,
                                wbuf, योग_read,
                                buf, u8len,
                                रिक्त, रिक्त);
    buf[u8len] = '\0';

निर्गच्छ:
    यदि (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    यदि (err) {
        PyEval_RestoreThread(tstate);
        Py_Err_समुच्चयः_From_Windows_Err(err);
        PyEval_SaveThread();
    }
    प्रत्यागम buf;
}

#यदि_अन्तः /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

वर्ण *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ध्रुव वर्ण *prompt)
{
    आकार_t n;
    वर्ण *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    प्रतिज्ञा(tstate != रिक्त);

#यदि_परिभाषितम् HAVE_WINDOWS_CONSOLE_IO
    ध्रुव PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    यदि (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE h_Std_अन्तर्, hStdErr;

        h_Std_अन्तर् = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        यदि (_get_console_type(h_Std_अन्तर्) == 'r') {
            fflush(sys_stdout);
            यदि (prompt) {
                यदि (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    पूर्णाङ्क wlen;
                    wlen = Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, 0, prompt, -1,
                            रिक्त, 0);
                    यदि (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * परिमाण(wchar_t));
                        यदि (wbuf == रिक्त) {
                            PyEval_RestoreThread(tstate);
                            Py_Err_No_स्मृतिः();
                            PyEval_SaveThread();
                            प्रत्यागम रिक्त;
                        }
                        wlen = Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        यदि (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, रिक्त);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } अन्यथा {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            प्रत्यागम _PyOS_WindowsConsoleReadline(tstate, h_Std_अन्तर्);
        }
    }
#यदि_अन्तः

    fflush(sys_stdout);
    यदि (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = रिक्त;
    कुरु {
        आकार_t incr = (n > 0) ? n + 2 : 100;
        यदि (incr > अधिकतम_पूर्णाङ्कः) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_समुच्चयः_String(Py_Exc_अतिप्रवाहः_Error, "input line too long");
            PyEval_SaveThread();
            प्रत्यागम रिक्त;
        }
        pr = (वर्ण *)PyMem_RawRealloc(p, n + incr);
        यदि (pr == रिक्त) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_No_स्मृतिः();
            PyEval_SaveThread();
            प्रत्यागम रिक्त;
        }
        p = pr;
        पूर्णाङ्क err = my_fgets(tstate, p + n, (पूर्णाङ्क)incr, sys_stdin);
        यदि (err == 1) {
            //व्यवधानम्
            PyMem_RawFree(p);
            प्रत्यागम रिक्त;
        } अन्यथा यदि (err != 0) {
            // EOF or error
            p[n] = '\0';
            भङ्ग;
        }
        n += वाक्यदैर्घ्यम्(p + n);
    } यावत् (n == 0 || p[n-1] != '\n');

    pr = (वर्ण *)PyMem_RawRealloc(p, n+1);
    यदि (pr == रिक्त) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        Py_Err_No_स्मृतिः();
        PyEval_SaveThread();
        प्रत्यागम रिक्त;
    }
    प्रत्यागम pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

वर्ण *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, ध्रुव वर्ण *) = रिक्त;


वर्ण *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ध्रुव वर्ण *prompt)
{
    वर्ण *rv, *res;
    आकार_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    यदि (Py_atomic_आरोपय_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        Py_Err_समुच्चयः_String(PyExc_RuntimeError,
                        "can't re-enter readline");
        प्रत्यागम रिक्त;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_तन्तवः
    Py_Mutex_तालकम्(&Py_OS_Readline_तालकम्);
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
    PyMutex_Unlock(&Py_OS_Readline_तालकम्);
    Py_अन्तः_ALLOW_तन्तवः

    यदि (rv == रिक्त)
        प्रत्यागम रिक्त;

    len = वाक्यदैर्घ्यम्(rv) + 1;
    res = PyMem_Malloc(len);
    यदि (res != रिक्त) {
        स्मृतिं_प्रतिलिख(res, rv, len);
    }
    अन्यथा {
        Py_Err_No_स्मृतिः();
    }
    PyMem_RawFree(rv);

    प्रत्यागम res;
}
