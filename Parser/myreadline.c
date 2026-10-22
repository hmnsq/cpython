
/* Readline interface for the tokenizer and [raw_]input() in bltinmodule.c.
   By default, or when stdin is not a tty device, we have a super
   simple my_readline function using fgets.
   Optionally, we can use the GNU readline library.
   my_readline() has a different return value from GNU readline():
   - NULL if an interrupt occurred or if an error occurred
   - a malloc'ed empty string if EOF was read
   - a malloc'ed string ending in \n normally
*/

#شامل "Python.h"
#شامل "pycore_fileutils.h"     // _Py_BEGIN_SUPPRESS_IPH
#شامل "pycore_interp.h"        // _PyInterpreterState_GetConfig()
#شامل "pycore_pystate.h"       // _PyThreadState_GET()
#شامل "pycore_signal.h"        // _PyOS_SigintEvent()
#اگر_تعریف_شدہ MS_WINDOWS
#  ifndef WIN32_LEAN_AND_اوسط
#    define WIN32_LEAN_AND_اوسط
#  endif
#  include "windows.h"
#اگر_ختم /* MS_WINDOWS */

#اگر_تعریف_شدہ HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#اگر_ختم


// Export the symbol since it's used by the readline shared extension
Py_API_ڈیٹا(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = تہی;

ساکن PyMutex Py_OS_Readline_تالا;

صحیح (*Py_OS_داخلہ_Hook)(خالی) = تہی;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ساکن صحیح
my_fgets(PyThreadState* tstate, حرف *buf, صحیح len, FILE *fp)
{
#اگر_تعریف_شدہ MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    Py_انتہا_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    اگر (handle == INVALID_HANDLE_قدر) {
        واپس -1; /* EOF */
    }
#اگر_ختم

    جب (1) {
        اگر (Py_OS_داخلہ_Hook != تہی &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (خالی)(Py_OS_داخلہ_Hook)();
        }

        errno = 0;
        clearerr(fp);
        حرف *p = fgets(buf, len, fp);
        اگر (p != تہی) {
            واپس 0; /* No error */
        }
        صحیح err = errno;

#اگر_تعریف_شدہ MS_WINDOWS
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
        اگر (Get_آخری_Error()==ERROR_OPERATION_ABORTED) {
            HANDLE h_مداخلت_Event = _PyOS_SigintEvent();
            انتخاب (WaitForSingleObjectEx(h_مداخلت_Event, 10, FALSE)) {
            حالت WAIT_OBJECT_0:
                ResetEvent(h_مداخلت_Event);
                واپس 1; /*مداخلت*/
            حالت WAIT_FAILED:
                واپس -2; /* Error */
            }
        }
#اگر_ختم /* MS_WINDOWS */

        اگر (feof(fp)) {
            clearerr(fp);
            واپس -1; /* EOF */
        }

#اگر_تعریف_شدہ EINTR
        اگر (err == EINTR) {
            PyEval_RestoreThread(tstate);
            صحیح s = Py_Err_Check_اشارات();
            PyEval_SaveThread();

            اگر (s < 0) {
                واپس 1;
            }
            /* try again */
            جاری;
        }
#اگر_ختم

        اگر (Py_OS_مداخلت_Occurred(tstate)) {
            واپس 1; /*مداخلت*/
        }
        واپس -2; /* Error */
    }
    /* NOTREACHED */
}

#اگر_تعریف_شدہ HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

خارجی حرف _get_console_type(HANDLE handle);

حرف *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE h_Std_اندر)
{
    ساکن wchar_t wbuf_local[1024 * 16];
    ثابت DWORD chunk_سائز = 1024;

    DWORD n_read, کل_read, wbuflen, u8len;
    wchar_t *wbuf;
    حرف *buf = تہی;
    صحیح err = 0;

    n_read = (DWORD)-1;
    کل_read = 0;
    wbuf = wbuf_local;
    wbuflen = حجم(wbuf_local) / حجم(wbuf_local[0]) - 1;
    جب (1) {
        اگر (Py_OS_داخلہ_Hook != تہی &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (خالی)(Py_OS_داخلہ_Hook)();
        }
        اگر (!ReadConsoleW(h_Std_اندر, &wbuf[کل_read], wbuflen - کل_read, &n_read, تہی)) {
            err = Get_آخری_Error();
            جاؤ باہر_نکلو;
        }
        اگر (n_read == (DWORD)-1 && (err = Get_آخری_Error()) == ERROR_OPERATION_ABORTED) {
            توڑو;
        }
        اگر (n_read == 0) {
            صحیح s;
            err = Get_آخری_Error();
            اگر (err != ERROR_OPERATION_ABORTED)
                جاؤ باہر_نکلو;
            err = 0;
            HANDLE h_مداخلت_Event = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(h_مداخلت_Event, 100, FALSE);
            اگر (state == WAIT_OBJECT_0) {
                ResetEvent(h_مداخلت_Event);
                PyEval_RestoreThread(tstate);
                s = Py_Err_Check_اشارات();
                PyEval_SaveThread();
                اگر (s < 0) {
                    جاؤ باہر_نکلو;
                }
            }
            ورنہ اگر (state != WAIT_TIMEOUT) {
                err = Get_آخری_Error();
                جاؤ باہر_نکلو;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            جاری;
        }

        کل_read += n_read;
        اگر (کل_read == 0 || wbuf[کل_read - 1] == L'\n') {
            توڑو;
        }
        wbuflen += chunk_سائز;
        اگر (wbuf == wbuf_local) {
            wbuf[کل_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * حجم(wchar_t));
            اگر (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            ورنہ {
                PyEval_RestoreThread(tstate);
                Py_Err_No_حافظہ();
                PyEval_SaveThread();
                جاؤ باہر_نکلو;
            }
        }
        ورنہ {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * حجم(wchar_t));
            اگر (tmp == تہی) {
                PyEval_RestoreThread(tstate);
                Py_Err_No_حافظہ();
                PyEval_SaveThread();
                جاؤ باہر_نکلو;
            }
            wbuf = tmp;
        }
    }

    اگر (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        اگر (buf) {
            buf[0] = '\0';
        }
        ورنہ {
            PyEval_RestoreThread(tstate);
            Py_Err_No_حافظہ();
            PyEval_SaveThread();
        }
        جاؤ باہر_نکلو;
    }

    u8len = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0,
                                wbuf, کل_read,
                                تہی, 0,
                                تہی, تہی);
    buf = PyMem_RawMalloc(u8len + 1);
    اگر (buf == تہی) {
        PyEval_RestoreThread(tstate);
        Py_Err_No_حافظہ();
        PyEval_SaveThread();
        جاؤ باہر_نکلو;
    }

    u8len = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0,
                                wbuf, کل_read,
                                buf, u8len,
                                تہی, تہی);
    buf[u8len] = '\0';

باہر_نکلو:
    اگر (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    اگر (err) {
        PyEval_RestoreThread(tstate);
        Py_Err_سیٹ_From_Windows_Err(err);
        PyEval_SaveThread();
    }
    واپس buf;
}

#اگر_ختم /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

حرف *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ثابت حرف *prompt)
{
    سائز_t n;
    حرف *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    أكد(tstate != تہی);

#اگر_تعریف_شدہ HAVE_WINDOWS_CONSOLE_IO
    ثابت PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    اگر (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE h_Std_اندر, hStdErr;

        h_Std_اندر = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        اگر (_get_console_type(h_Std_اندر) == 'r') {
            fflush(sys_stdout);
            اگر (prompt) {
                اگر (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    صحیح wlen;
                    wlen = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, prompt, -1,
                            تہی, 0);
                    اگر (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * حجم(wchar_t));
                        اگر (wbuf == تہی) {
                            PyEval_RestoreThread(tstate);
                            Py_Err_No_حافظہ();
                            PyEval_SaveThread();
                            واپس تہی;
                        }
                        wlen = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        اگر (wlen) {
                            DWORD n;
                            fflush(stderr);
                            /* wlen includes null terminator, so subtract 1 */
                            WriteConsoleW(hStdErr, wbuf, wlen - 1, &n, تہی);
                        }
                        PyMem_RawFree(wbuf);
                    }
                } ورنہ {
                    fprintf(stderr, "%s", prompt);
                    fflush(stderr);
                }
            }
            clearerr(sys_stdin);
            واپس _PyOS_WindowsConsoleReadline(tstate, h_Std_اندر);
        }
    }
#اگر_ختم

    fflush(sys_stdout);
    اگر (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = تہی;
    کرو {
        سائز_t incr = (n > 0) ? n + 2 : 100;
        اگر (incr > زیادہ_سے_زیادہ_عدد) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_سیٹ_String(Py_Exc_حد_سے_تجاوز_Error, "input line too long");
            PyEval_SaveThread();
            واپس تہی;
        }
        pr = (حرف *)PyMem_RawRealloc(p, n + incr);
        اگر (pr == تہی) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_No_حافظہ();
            PyEval_SaveThread();
            واپس تہی;
        }
        p = pr;
        صحیح err = my_fgets(tstate, p + n, (صحیح)incr, sys_stdin);
        اگر (err == 1) {
            //مداخلت
            PyMem_RawFree(p);
            واپس تہی;
        } ورنہ اگر (err != 0) {
            // EOF or error
            p[n] = '\0';
            توڑو;
        }
        n += لڑی_لمبائی(p + n);
    } جب (n == 0 || p[n-1] != '\n');

    pr = (حرف *)PyMem_RawRealloc(p, n+1);
    اگر (pr == تہی) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        Py_Err_No_حافظہ();
        PyEval_SaveThread();
        واپس تہی;
    }
    واپس pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

حرف *(*Py_OS_Readline_Function_اشارہ_گر)(FILE *, FILE *, ثابت حرف *) = تہی;


حرف *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ثابت حرف *prompt)
{
    حرف *rv, *res;
    سائز_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    اگر (Py_atomic_بھرو_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        Py_Err_سیٹ_String(PyExc_RuntimeError,
                        "can't re-enter readline");
        واپس تہی;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_دھاگے
    Py_Mutex_تالا(&Py_OS_Readline_تالا);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    اگر (Py_OS_Readline_Function_اشارہ_گر == تہی) {
        Py_OS_Readline_Function_اشارہ_گر = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    اگر (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
    ورنہ {
        rv = (*Py_OS_Readline_Function_اشارہ_گر)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, تہی);
    PyMutex_Unlock(&Py_OS_Readline_تالا);
    Py_انتہا_ALLOW_دھاگے

    اگر (rv == تہی)
        واپس تہی;

    len = لڑی_لمبائی(rv) + 1;
    res = PyMem_Malloc(len);
    اگر (res != تہی) {
        حافظہ_نقل(res, rv, len);
    }
    ورنہ {
        Py_Err_No_حافظہ();
    }
    PyMem_RawFree(rv);

    واپس res;
}
