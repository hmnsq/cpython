
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#అన్తర్భావ "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#అన్తర్భావ "pycore_pystate.h"       // _PyThreadState_GET()
#అన్తర్భావ "pycore_signal.h"        // _PyOS_SigintEvent()
#నిర్వచితమైతే MS_WINDOWS
#  ifndef WIN32_LEAN_AND_సగటు
#    define WIN32_LEAN_AND_సగటు
#  endif
#  include "windows.h"
#అయితే_ముగింపు /* MS_WINDOWS */

#నిర్వచితమైతే HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#అయితే_ముగింపు


// Export the symbol since it's used by the readline shared extension
Py_API_దత్తాంశం(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = ఖాళీ;

స్థిర PyMutex Py_OS_Readline_తాళం;

పూర్ణాంకం (*Py_OS_ఆదానం_Hook)(శూన్య) = ఖాళీ;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

స్థిర పూర్ణాంకం
my_fgets(PyThreadState* tstate, అక్షరం *buf, పూర్ణాంకం len, FILE *fp)
{
#నిర్వచితమైతే MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    Py_చివర_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    ఒకవేళ (handle == INVALID_HANDLE_విలువ) {
        తిరిగి -1; /* EOF */
    }
#అయితే_ముగింపు

    అయితే (1) {
        ఒకవేళ (Py_OS_ఆదానం_Hook != ఖాళీ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (శూన్య)(Py_OS_ఆదానం_Hook)();
        }

        errno = 0;
        clearerr(fp);
        అక్షరం *p = fgets(buf, len, fp);
        ఒకవేళ (p != ఖాళీ) {
            తిరిగి 0; /* No error */
        }
        పూర్ణాంకం err = errno;

#నిర్వచితమైతే MS_WINDOWS
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
        ఒకవేళ (Get_చివరి_Error()==ERROR_OPERATION_ABORTED) {
            HANDLE h_అంతరాయం_Event = _PyOS_SigintEvent();
            ఎంపిక (WaitForSingleObjectEx(h_అంతరాయం_Event, 10, FALSE)) {
            స్థితి WAIT_OBJECT_0:
                ResetEvent(h_అంతరాయం_Event);
                తిరిగి 1; /*అంతరాయం*/
            స్థితి WAIT_FAILED:
                తిరిగి -2; /* Error */
            }
        }
#అయితే_ముగింపు /* MS_WINDOWS */

        ఒకవేళ (feof(fp)) {
            clearerr(fp);
            తిరిగి -1; /* EOF */
        }

#నిర్వచితమైతే EINTR
        ఒకవేళ (err == EINTR) {
            PyEval_RestoreThread(tstate);
            పూర్ణాంకం s = Py_Err_Check_సంకేతాలు();
            PyEval_SaveThread();

            ఒకవేళ (s < 0) {
                తిరిగి 1;
            }
            /* try again */
            కొనసాగు;
        }
#అయితే_ముగింపు

        ఒకవేళ (Py_OS_అంతరాయం_Occurred(tstate)) {
            తిరిగి 1; /*అంతరాయం*/
        }
        తిరిగి -2; /* Error */
    }
    /* NOTREACHED */
}

#నిర్వచితమైతే HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

బాహ్య అక్షరం _get_console_type(HANDLE handle);

అక్షరం *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE h_Std_లోపల)
{
    స్థిర wchar_t wbuf_local[1024 * 16];
    స్థిరరాశి DWORD chunk_కొలత = 1024;

    DWORD n_read, మొత్తం_read, wbuflen, u8len;
    wchar_t *wbuf;
    అక్షరం *buf = ఖాళీ;
    పూర్ణాంకం err = 0;

    n_read = (DWORD)-1;
    మొత్తం_read = 0;
    wbuf = wbuf_local;
    wbuflen = పరిమాణం(wbuf_local) / పరిమాణం(wbuf_local[0]) - 1;
    అయితే (1) {
        ఒకవేళ (Py_OS_ఆదానం_Hook != ఖాళీ &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (శూన్య)(Py_OS_ఆదానం_Hook)();
        }
        ఒకవేళ (!ReadConsoleW(h_Std_లోపల, &wbuf[మొత్తం_read], wbuflen - మొత్తం_read, &n_read, ఖాళీ)) {
            err = Get_చివరి_Error();
            వెళ్ళు నిష్క్రమించు;
        }
        ఒకవేళ (n_read == (DWORD)-1 && (err = Get_చివరి_Error()) == ERROR_OPERATION_ABORTED) {
            ఆపు;
        }
        ఒకవేళ (n_read == 0) {
            పూర్ణాంకం s;
            err = Get_చివరి_Error();
            ఒకవేళ (err != ERROR_OPERATION_ABORTED)
                వెళ్ళు నిష్క్రమించు;
            err = 0;
            HANDLE h_అంతరాయం_Event = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(h_అంతరాయం_Event, 100, FALSE);
            ఒకవేళ (state == WAIT_OBJECT_0) {
                ResetEvent(h_అంతరాయం_Event);
                PyEval_RestoreThread(tstate);
                s = Py_Err_Check_సంకేతాలు();
                PyEval_SaveThread();
                ఒకవేళ (s < 0) {
                    వెళ్ళు నిష్క్రమించు;
                }
            }
            లేకపోతే ఒకవేళ (state != WAIT_TIMEOUT) {
                err = Get_చివరి_Error();
                వెళ్ళు నిష్క్రమించు;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            కొనసాగు;
        }

        మొత్తం_read += n_read;
        ఒకవేళ (మొత్తం_read == 0 || wbuf[మొత్తం_read - 1] == L'\n') {
            ఆపు;
        }
        wbuflen += chunk_కొలత;
        ఒకవేళ (wbuf == wbuf_local) {
            wbuf[మొత్తం_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * పరిమాణం(wchar_t));
            ఒకవేళ (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            లేకపోతే {
                PyEval_RestoreThread(tstate);
                Py_Err_No_జ్ఞాపకం();
                PyEval_SaveThread();
                వెళ్ళు నిష్క్రమించు;
            }
        }
        లేకపోతే {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * పరిమాణం(wchar_t));
            ఒకవేళ (tmp == ఖాళీ) {
                PyEval_RestoreThread(tstate);
                Py_Err_No_జ్ఞాపకం();
                PyEval_SaveThread();
                వెళ్ళు నిష్క్రమించు;
            }
            wbuf = tmp;
        }
    }

    ఒకవేళ (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        ఒకవేళ (buf) {
            buf[0] = '\0';
        }
        లేకపోతే {
            PyEval_RestoreThread(tstate);
            Py_Err_No_జ్ఞాపకం();
            PyEval_SaveThread();
        }
        వెళ్ళు నిష్క్రమించు;
    }

    u8len = Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0,
                                wbuf, మొత్తం_read,
                                ఖాళీ, 0,
                                ఖాళీ, ఖాళీ);
    buf = PyMem_RawMalloc(u8len + 1);
    ఒకవేళ (buf == ఖాళీ) {
        PyEval_RestoreThread(tstate);
        Py_Err_No_జ్ఞాపకం();
        PyEval_SaveThread();
        వెళ్ళు నిష్క్రమించు;
    }

    u8len = Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0,
                                wbuf, మొత్తం_read,
                                buf, u8len,
                                ఖాళీ, ఖాళీ);
    buf[u8len] = '\0';

నిష్క్రమించు:
    ఒకవేళ (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    ఒకవేళ (err) {
        PyEval_RestoreThread(tstate);
        Py_Err_సమితి_From_Windows_Err(err);
        PyEval_SaveThread();
    }
    తిరిగి buf;
}

#అయితే_ముగింపు /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

అక్షరం *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, స్థిరరాశి అక్షరం *prompt)
{
    కొలత_t n;
    అక్షరం *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    ప్రతిజ్ఞా(tstate != ఖాళీ);

#నిర్వచితమైతే HAVE_WINDOWS_CONSOLE_IO
    స్థిరరాశి PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    ఒకవేళ (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE h_Std_లోపల, hStdErr;

        h_Std_లోపల = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        ఒకవేళ (_get_console_type(h_Std_లోపల) == 'r') {
            fflush(sys_stdout);
            ఒకవేళ (prompt) {
                ఒకవేళ (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    పూర్ణాంకం wlen;
                    wlen = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, prompt, -1,
                            ఖాళీ, 0);
                    ఒకవేళ (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * పరిమాణం(wchar_t));
                        ఒకవేళ (wbuf == ఖాళీ) {
                            PyEval_RestoreThread(tstate);
                            Py_Err_No_జ్ఞాపకం();
                            PyEval_SaveThread();
                            తిరిగి ఖాళీ;
                        }
                        wlen = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        ఒకవేళ (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, ఖాళీ);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } లేకపోతే {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            తిరిగి _PyOS_WindowsConsoleReadline(tstate, h_Std_లోపల);
        }
    }
#అయితే_ముగింపు

    fflush(sys_stdout);
    ఒకవేళ (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = ఖాళీ;
    చేయి {
        కొలత_t incr = (n > 0) ? n + 2 : 100;
        ఒకవేళ (incr > గరిష్ఠ_పూర్ణాంకం) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_సమితి_String(Py_Exc_అధికప్రవాహం_Error, "input line too long");
            PyEval_SaveThread();
            తిరిగి ఖాళీ;
        }
        pr = (అక్షరం *)PyMem_RawRealloc(p, n + incr);
        ఒకవేళ (pr == ఖాళీ) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_No_జ్ఞాపకం();
            PyEval_SaveThread();
            తిరిగి ఖాళీ;
        }
        p = pr;
        పూర్ణాంకం err = my_fgets(tstate, p + n, (పూర్ణాంకం)incr, sys_stdin);
        ఒకవేళ (err == 1) {
            //అంతరాయం
            PyMem_RawFree(p);
            తిరిగి ఖాళీ;
        } లేకపోతే ఒకవేళ (err != 0) {
            // EOF or error
            p[n] = '\0';
            ఆపు;
        }
        n += అక్షరశ్రేణి_పొడవు(p + n);
    } అయితే (n == 0 || p[n-1] != '\n');

    pr = (అక్షరం *)PyMem_RawRealloc(p, n+1);
    ఒకవేళ (pr == ఖాళీ) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        Py_Err_No_జ్ఞాపకం();
        PyEval_SaveThread();
        తిరిగి ఖాళీ;
    }
    తిరిగి pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

అక్షరం *(*Py_OS_Readline_Function_సూచిక)(FILE *, FILE *, స్థిరరాశి అక్షరం *) = ఖాళీ;


అక్షరం *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, స్థిరరాశి అక్షరం *prompt)
{
    అక్షరం *rv, *res;
    కొలత_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    ఒకవేళ (Py_atomic_ఎక్కించు_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        Py_Err_సమితి_String(PyExc_RuntimeError,
                        "can't re-enter readline");
        తిరిగి ఖాళీ;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_దారాలు
    Py_Mutex_తాళం(&Py_OS_Readline_తాళం);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    ఒకవేళ (Py_OS_Readline_Function_సూచిక == ఖాళీ) {
        Py_OS_Readline_Function_సూచిక = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    ఒకవేళ (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    లేకపోతే {
        rv = (*Py_OS_Readline_Function_సూచిక)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, ఖాళీ);
    PyMutex_Unlock(&Py_OS_Readline_తాళం);
    Py_చివర_ALLOW_దారాలు

    ఒకవేళ (rv == ఖాళీ)
        తిరిగి ఖాళీ;

    len = అక్షరశ్రేణి_పొడవు(rv) + 1;
    res = PyMem_Malloc(len);
    ఒకవేళ (res != ఖాళీ) {
        జ్ఞాపకం_నకలు(res, rv, len);
    }
    లేకపోతే {
        Py_Err_No_జ్ఞాపకం();
    }
    PyMem_RawFree(rv);

    తిరిగి res;
}
