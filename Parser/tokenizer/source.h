#جے_بنا_تعریف Py_TOKENIZER_سروت_H
#تعریف_کرو Py_TOKENIZER_سروت_H

#ضمن "Python.h"

#ضمن "types.h"

تعريفنوع بنية {
    حرف *بائٹاں;
    _PyTok_Off ادھار_وتھ;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحيح nlines;
} Py_Tok_سروت_متن;

ساكن مضمن ثابت حرف *
Py_Tok_سروت_انکڑے(ثابت Py_Tok_سروت_متن *سروت)
{
    أرجع سروت->بائٹاں != عدم ? سروت->بائٹاں : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ساكن مضمن ثابت حرف *
Py_Tok_سروت_اشارہ_گر(ثابت Py_Tok_سروت_متن *سروت, _PyTok_Off وتھ)
{
    أكد(وتھ >= سروت->ادھار_وتھ);
    أكد(وتھ - سروت->ادھار_وتھ <= سروت->len);
    أرجع Py_Tok_سروت_انکڑے(سروت) + (وتھ - سروت->ادھار_وتھ);
}

ساكن مضمن بنانشانی حرف
Py_Tok_سروت_Byte(ثابت Py_Tok_سروت_متن *سروت, _PyTok_Off وتھ)
{
    أكد(وتھ >= سروت->ادھار_وتھ);
    أكد(وتھ - سروت->ادھار_وتھ < سروت->len);
    أرجع (بنانشانی حرف)سروت->بائٹاں[وتھ - سروت->ادھار_وتھ];
}

ساكن مضمن ثابت حرف *
Py_Tok_سروت_وسعت_View(ثابت Py_Tok_سروت_متن *سروت, Py_Tok_وسعت وسعت,
                      Py_ssize_t *لمائی)
{
    أكد(لمائی != عدم);
    أكد(Py_Tok_وسعت_Is_Valid(وسعت));
    أكد(وسعت.سرا - سروت->ادھار_وتھ <= سروت->len);
    *لمائی = وسعت.سرا - وسعت.شروع;
    أرجع Py_Tok_سروت_اشارہ_گر(سروت, وسعت.شروع);
}

/* Return the first matching offset within span, or -1 if absent. */
ساكن مضمن _PyTok_Off
Py_Tok_سروت_لبھو_Byte(ثابت Py_Tok_سروت_متن *سروت, Py_Tok_وسعت وسعت,
                      صحيح byte)
{
    Py_ssize_t لمائی;
    ثابت حرف *انکڑے = Py_Tok_سروت_وسعت_View(سروت, وسعت, &لمائی);
    ثابت حرف *لبھیا = memchr(انکڑے, byte, لمائی);
    أرجع لبھیا != عدم ? وسعت.شروع + (لبھیا - انکڑے) : -1;
}

PyAPI_FUNC(فراغ) Py_Tok_سروت_Init(Py_Tok_سروت_متن *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(فراغ) Py_Tok_سروت_Clear(Py_Tok_سروت_متن *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(فراغ) Py_Tok_سروت_Discard(Py_Tok_سروت_متن *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_سروت_Append_سطر(
    Py_Tok_سروت_متن *سروت, ثابت حرف *بائٹاں, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_سروت_Append_سطر_With_Newline(
    Py_Tok_سروت_متن *سروت, ثابت حرف *بائٹاں, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت حرف *) Py_Tok_سروت_سطر_View(
    ثابت Py_Tok_سروت_متن *سروت, Py_ssize_t lineno, Py_ssize_t *len);

#جے_انت
