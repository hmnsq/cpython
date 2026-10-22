
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
#اگر_تعریف_شده MS_WINDOWS
#  ifndef WIN32_LEAN_AND_میانگین
#    define WIN32_LEAN_AND_میانگین
#  endif
#  include "windows.h"
#پایان_اگر /* MS_WINDOWS */

#اگر_تعریف_شده HAVE_UNISTD_H
#  include <unistd.h>             // isatty()
#پایان_اگر


// Export the symbol since it's used by the readline shared extension
Py_API_داده(PyThreadState*) _PyOS_ReadlineTState;
PyThreadState *_PyOS_ReadlineTState = عدم;

ساكن PyMutex Py_OS_Readline_قفل;

صحيح (*Py_OS_درون_داد_Hook)(فراغ) = عدم;

/* This function restarts a fgets() after an EINTR error occurred
   except if _PyOS_InterruptOccurred() returns true. */

ساكن صحيح
my_fgets(PyThreadState* tstate, حرف *buf, صحيح len, FILE *fp)
{
#اگر_تعریف_شده MS_WINDOWS
    HANDLE handle;
    _Py_BEGIN_SUPPRESS_IPH
    handle = (HANDLE)_get_osfhandle(fileno(fp));
    Py_انتها_SUPPRESS_IPH

    /* bpo-40826: fgets(fp) does crash if fileno(fp) is closed */
    إذا (handle == INVALID_HANDLE_مقدار) {
        أرجع -1; /* EOF */
    }
#پایان_اگر

    طالما (1) {
        إذا (Py_OS_درون_داد_Hook != عدم &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (فراغ)(Py_OS_درون_داد_Hook)();
        }

        errno = 0;
        clearerr(fp);
        حرف *p = fgets(buf, len, fp);
        إذا (p != عدم) {
            أرجع 0; /* No error */
        }
        صحيح err = errno;

#اگر_تعریف_شده MS_WINDOWS
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
        إذا (Get_آخرین_Error()==ERROR_OPERATION_ABORTED) {
            HANDLE h_وقفه_Event = _PyOS_SigintEvent();
            بدل (WaitForSingleObjectEx(h_وقفه_Event, 10, FALSE)) {
            حالة WAIT_OBJECT_0:
                ResetEvent(h_وقفه_Event);
                أرجع 1; /*وقفه*/
            حالة WAIT_FAILED:
                أرجع -2; /* Error */
            }
        }
#پایان_اگر /* MS_WINDOWS */

        إذا (feof(fp)) {
            clearerr(fp);
            أرجع -1; /* EOF */
        }

#اگر_تعریف_شده EINTR
        إذا (err == EINTR) {
            PyEval_RestoreThread(tstate);
            صحيح s = Py_Err_Check_سیگنالها();
            PyEval_SaveThread();

            إذا (s < 0) {
                أرجع 1;
            }
            /* try again */
            استمر;
        }
#پایان_اگر

        إذا (Py_OS_وقفه_Occurred(tstate)) {
            أرجع 1; /*وقفه*/
        }
        أرجع -2; /* Error */
    }
    /* NOTREACHED */
}

#اگر_تعریف_شده HAVE_WINDOWS_CONSOLE_IO
/* Readline implementation using ReadConsoleW */

خارجي حرف _get_console_type(HANDLE handle);

حرف *
_PyOS_WindowsConsoleReadline(PyThreadState *tstate, HANDLE h_Std_درون)
{
    ساكن wchar_t wbuf_local[1024 * 16];
    ثابت DWORD chunk_اندازه = 1024;

    DWORD n_read, مجموع_read, wbuflen, u8len;
    wchar_t *wbuf;
    حرف *buf = عدم;
    صحيح err = 0;

    n_read = (DWORD)-1;
    مجموع_read = 0;
    wbuf = wbuf_local;
    wbuflen = حجم(wbuf_local) / حجم(wbuf_local[0]) - 1;
    طالما (1) {
        إذا (Py_OS_درون_داد_Hook != عدم &&
            // GH-104668: See PyOS_ReadlineFunctionPointer's comment below...
            _Py_IsMainInterpreter(tstate->interp))
        {
            (فراغ)(Py_OS_درون_داد_Hook)();
        }
        إذا (!ReadConsoleW(h_Std_درون, &wbuf[مجموع_read], wbuflen - مجموع_read, &n_read, عدم)) {
            err = Get_آخرین_Error();
            اذهب خروج;
        }
        إذا (n_read == (DWORD)-1 && (err = Get_آخرین_Error()) == ERROR_OPERATION_ABORTED) {
            اكسر;
        }
        إذا (n_read == 0) {
            صحيح s;
            err = Get_آخرین_Error();
            إذا (err != ERROR_OPERATION_ABORTED)
                اذهب خروج;
            err = 0;
            HANDLE h_وقفه_Event = _PyOS_SigintEvent();
            DWORD state = WaitForSingleObjectEx(h_وقفه_Event, 100, FALSE);
            إذا (state == WAIT_OBJECT_0) {
                ResetEvent(h_وقفه_Event);
                PyEval_RestoreThread(tstate);
                s = Py_Err_Check_سیگنالها();
                PyEval_SaveThread();
                إذا (s < 0) {
                    اذهب خروج;
                }
            }
            وإلا إذا (state != WAIT_TIMEOUT) {
                err = Get_آخرین_Error();
                اذهب خروج;
            }
            /* The console cancelled the read and flushed its input buffer,
               but no exception was raised.  Start the read over. */
            استمر;
        }

        مجموع_read += n_read;
        إذا (مجموع_read == 0 || wbuf[مجموع_read - 1] == L'\n') {
            اكسر;
        }
        wbuflen += chunk_اندازه;
        إذا (wbuf == wbuf_local) {
            wbuf[مجموع_read] = '\0';
            wbuf = (wchar_t*)PyMem_RawMalloc(wbuflen * حجم(wchar_t));
            إذا (wbuf) {
                wcscpy_s(wbuf, wbuflen, wbuf_local);
            }
            وإلا {
                PyEval_RestoreThread(tstate);
                Py_Err_No_حافظه();
                PyEval_SaveThread();
                اذهب خروج;
            }
        }
        وإلا {
            wchar_t *tmp = PyMem_RawRealloc(wbuf, wbuflen * حجم(wchar_t));
            إذا (tmp == عدم) {
                PyEval_RestoreThread(tstate);
                Py_Err_No_حافظه();
                PyEval_SaveThread();
                اذهب خروج;
            }
            wbuf = tmp;
        }
    }

    إذا (wbuf[0] == '\x1a') {
        buf = PyMem_RawMalloc(1);
        إذا (buf) {
            buf[0] = '\0';
        }
        وإلا {
            PyEval_RestoreThread(tstate);
            Py_Err_No_حافظه();
            PyEval_SaveThread();
        }
        اذهب خروج;
    }

    u8len = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0,
                                wbuf, مجموع_read,
                                عدم, 0,
                                عدم, عدم);
    buf = PyMem_RawMalloc(u8len + 1);
    إذا (buf == عدم) {
        PyEval_RestoreThread(tstate);
        Py_Err_No_حافظه();
        PyEval_SaveThread();
        اذهب خروج;
    }

    u8len = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0,
                                wbuf, مجموع_read,
                                buf, u8len,
                                عدم, عدم);
    buf[u8len] = '\0';

خروج:
    إذا (wbuf != wbuf_local) {
        PyMem_RawFree(wbuf);
    }

    إذا (err) {
        PyEval_RestoreThread(tstate);
        Py_Err_مجموعه_From_Windows_Err(err);
        PyEval_SaveThread();
    }
    أرجع buf;
}

#پایان_اگر /* HAVE_WINDOWS_CONSOLE_IO */


/* Readline implementation using fgets() */

حرف *
PyOS_StdioReadline(FILE *sys_stdin, FILE *sys_stdout, ثابت حرف *prompt)
{
    اندازه_t n;
    حرف *p, *pr;
    PyThreadState *tstate = _PyOS_ReadlineTState;
    أكد(tstate != عدم);

#اگر_تعریف_شده HAVE_WINDOWS_CONSOLE_IO
    ثابت PyConfig *config = _PyInterpreterState_GetConfig(tstate->interp);
    إذا (!config->legacy_windows_stdio && sys_stdin == stdin) {
        HANDLE h_Std_درون, hStdErr;

        h_Std_درون = _Py_get_osfhandle_noraise(fileno(sys_stdin));
        hStdErr = _Py_get_osfhandle_noraise(fileno(stderr));

        إذا (_get_console_type(h_Std_درون) == 'r') {
            fflush(sys_stdout);
            إذا (prompt) {
                إذا (_get_console_type(hStdErr) == 'w') {
                    wchar_t *wbuf;
                    صحيح wlen;
                    wlen = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, prompt, -1,
                            عدم, 0);
                    إذا (wlen) {
                        wbuf = PyMem_RawMalloc(wlen * حجم(wchar_t));
                        إذا (wbuf == عدم) {
                            PyEval_RestoreThread(tstate);
                            Py_Err_No_حافظه();
                            PyEval_SaveThread();
                            أرجع عدم;
                        }
                        wlen = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, prompt, -1,
                                wbuf, wlen);
                        إذا (wlen) {
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
            أرجع _PyOS_WindowsConsoleReadline(tstate, h_Std_درون);
        }
    }
#پایان_اگر

    fflush(sys_stdout);
    إذا (prompt) {
        fprintf(stderr, "%s", prompt);
    }
    fflush(stderr);

    n = 0;
    p = عدم;
    افعل {
        اندازه_t incr = (n > 0) ? n + 2 : 100;
        إذا (incr > بیشترین_صحیح) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_مجموعه_String(Py_Exc_سرریز_Error, "input line too long");
            PyEval_SaveThread();
            أرجع عدم;
        }
        pr = (حرف *)PyMem_RawRealloc(p, n + incr);
        إذا (pr == عدم) {
            PyMem_RawFree(p);
            PyEval_RestoreThread(tstate);
            Py_Err_No_حافظه();
            PyEval_SaveThread();
            أرجع عدم;
        }
        p = pr;
        صحيح err = my_fgets(tstate, p + n, (صحيح)incr, sys_stdin);
        إذا (err == 1) {
            //وقفه
            PyMem_RawFree(p);
            أرجع عدم;
        } وإلا إذا (err != 0) {
            // EOF or error
            p[n] = '\0';
            اكسر;
        }
        n += طول_رشته(p + n);
    } طالما (n == 0 || p[n-1] != '\n');

    pr = (حرف *)PyMem_RawRealloc(p, n+1);
    إذا (pr == عدم) {
        PyMem_RawFree(p);
        PyEval_RestoreThread(tstate);
        Py_Err_No_حافظه();
        PyEval_SaveThread();
        أرجع عدم;
    }
    أرجع pr;
}


/* By initializing this function pointer, systems embedding Python can
   override the readline function.

   Note: Python expects in return a buffer allocated with PyMem_Malloc. */

حرف *(*Py_OS_Readline_Function_اشاره_گر)(FILE *, FILE *, ثابت حرف *) = عدم;


حرف *
PyOS_Readline(FILE *sys_stdin, FILE *sys_stdout, ثابت حرف *prompt)
{
    حرف *rv, *res;
    اندازه_t len;

    PyThreadState *tstate = _PyThreadState_GET();
    إذا (Py_atomic_بارگیری_ptr_relaxed(&_PyOS_ReadlineTState) == tstate) {
        Py_Err_مجموعه_String(PyExc_RuntimeError,
                        "can't re-enter readline");
        أرجع عدم;
    }

    // GH-123321: We need to acquire the lock before setting
    // _PyOS_ReadlineTState, otherwise the variable may be nullified by a
    // different thread.
    Py_BEGIN_ALLOW_نخها
    Py_Mutex_قفل(&Py_OS_Readline_قفل);
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, tstate);
    إذا (Py_OS_Readline_Function_اشاره_گر == عدم) {
        Py_OS_Readline_Function_اشاره_گر = PyOS_StdioReadline;
    }

    /* This is needed to handle the unlikely case that the
     * interpreter is in interactive mode *and* stdin/out are not
     * a tty.  This can happen, for example if python is run like
     * this: python -i < test1.py
     */
    إذا (!isatty(fileno(sys_stdin)) || !isatty(fileno(sys_stdout)) ||
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
        rv = (*Py_OS_Readline_Function_اشاره_گر)(sys_stdin, sys_stdout, prompt);
    }

    // gh-123321: Must set the variable and then release the lock before
    // taking the GIL. Otherwise a deadlock or segfault may occur.
    _Py_atomic_store_ptr_relaxed(&_PyOS_ReadlineTState, عدم);
    PyMutex_Unlock(&Py_OS_Readline_قفل);
    Py_انتها_ALLOW_نخها

    إذا (rv == عدم)
        أرجع عدم;

    len = طول_رشته(rv) + 1;
    res = PyMem_Malloc(len);
    إذا (res != عدم) {
        کپی_حافظه(res, rv, len);
    }
    وإلا {
        Py_Err_No_حافظه();
    }
    PyMem_RawFree(rv);

    أرجع res;
}
