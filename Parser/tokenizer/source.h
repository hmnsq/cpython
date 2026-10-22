#اگر_تعریف_نشده Py_TOKENIZER_منبع_H
#تعریف_کن Py_TOKENIZER_منبع_H

#ضمن "Python.h"

#ضمن "types.h"

تعريفنوع بنية {
    حرف *بایتها;
    _PyTok_Off پایه_جابه_جایی;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحيح nlines;
} Py_Tok_منبع_متن;

ساكن مضمن ثابت حرف *
Py_Tok_منبع_داده(ثابت Py_Tok_منبع_متن *منبع)
{
    أرجع منبع->بایتها != عدم ? منبع->بایتها : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ساكن مضمن ثابت حرف *
Py_Tok_منبع_اشاره_گر(ثابت Py_Tok_منبع_متن *منبع, _PyTok_Off جابه_جایی)
{
    أكد(جابه_جایی >= منبع->پایه_جابه_جایی);
    أكد(جابه_جایی - منبع->پایه_جابه_جایی <= منبع->len);
    أرجع Py_Tok_منبع_داده(منبع) + (جابه_جایی - منبع->پایه_جابه_جایی);
}

ساكن مضمن بیعلامت حرف
Py_Tok_منبع_Byte(ثابت Py_Tok_منبع_متن *منبع, _PyTok_Off جابه_جایی)
{
    أكد(جابه_جایی >= منبع->پایه_جابه_جایی);
    أكد(جابه_جایی - منبع->پایه_جابه_جایی < منبع->len);
    أرجع (بیعلامت حرف)منبع->بایتها[جابه_جایی - منبع->پایه_جابه_جایی];
}

ساكن مضمن ثابت حرف *
Py_Tok_منبع_بازه_View(ثابت Py_Tok_منبع_متن *منبع, Py_Tok_بازه بازه,
                      Py_ssize_t *درازی)
{
    أكد(درازی != عدم);
    أكد(Py_Tok_بازه_Is_Valid(بازه));
    أكد(بازه.انتها - منبع->پایه_جابه_جایی <= منبع->len);
    *درازی = بازه.انتها - بازه.شروع;
    أرجع Py_Tok_منبع_اشاره_گر(منبع, بازه.شروع);
}

/* Return the first matching offset within span, or -1 if absent. */
ساكن مضمن _PyTok_Off
Py_Tok_منبع_پیدا_کن_Byte(ثابت Py_Tok_منبع_متن *منبع, Py_Tok_بازه بازه,
                      صحيح byte)
{
    Py_ssize_t درازی;
    ثابت حرف *داده = Py_Tok_منبع_بازه_View(منبع, بازه, &درازی);
    ثابت حرف *یافت = memchr(داده, byte, درازی);
    أرجع یافت != عدم ? بازه.شروع + (یافت - داده) : -1;
}

PyAPI_FUNC(فراغ) Py_Tok_منبع_Init(Py_Tok_منبع_متن *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(فراغ) Py_Tok_منبع_Clear(Py_Tok_منبع_متن *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(فراغ) Py_Tok_منبع_Discard(Py_Tok_منبع_متن *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_منبع_Append_سطر(
    Py_Tok_منبع_متن *منبع, ثابت حرف *بایتها, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_منبع_Append_سطر_With_Newline(
    Py_Tok_منبع_متن *منبع, ثابت حرف *بایتها, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت حرف *) Py_Tok_منبع_سطر_View(
    ثابت Py_Tok_منبع_متن *منبع, Py_ssize_t lineno, Py_ssize_t *len);

#پایان_اگر
