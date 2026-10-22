#جيڪڏهن_وصف_نه_ٿيل Py_TOKENIZER_ذريعو_H
#وصف_ڪريو Py_TOKENIZER_ذريعو_H

#ضمن "Python.h"

#ضمن "types.h"

تعريفنوع بنية {
    حرف *بائيٽون;
    _PyTok_Off بنياد_وٿي;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحيح nlines;
} Py_Tok_ذريعو_متن;

ساكن مضمن ثابت حرف *
Py_Tok_ذريعو_معلومات(ثابت Py_Tok_ذريعو_متن *ذريعو)
{
    أرجع ذريعو->بائيٽون != عدم ? ذريعو->بائيٽون : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ساكن مضمن ثابت حرف *
Py_Tok_ذريعو_اشارو(ثابت Py_Tok_ذريعو_متن *ذريعو, _PyTok_Off وٿي)
{
    أكد(وٿي >= ذريعو->بنياد_وٿي);
    أكد(وٿي - ذريعو->بنياد_وٿي <= ذريعو->len);
    أرجع Py_Tok_ذريعو_معلومات(ذريعو) + (وٿي - ذريعو->بنياد_وٿي);
}

ساكن مضمن بنانشان حرف
Py_Tok_ذريعو_Byte(ثابت Py_Tok_ذريعو_متن *ذريعو, _PyTok_Off وٿي)
{
    أكد(وٿي >= ذريعو->بنياد_وٿي);
    أكد(وٿي - ذريعو->بنياد_وٿي < ذريعو->len);
    أرجع (بنانشان حرف)ذريعو->بائيٽون[وٿي - ذريعو->بنياد_وٿي];
}

ساكن مضمن ثابت حرف *
Py_Tok_ذريعو_وسعت_View(ثابت Py_Tok_ذريعو_متن *ذريعو, Py_Tok_وسعت وسعت,
                      Py_ssize_t *ڊيگهائي)
{
    أكد(ڊيگهائي != عدم);
    أكد(Py_Tok_وسعت_Is_Valid(وسعت));
    أكد(وسعت.پڇاڙي - ذريعو->بنياد_وٿي <= ذريعو->len);
    *ڊيگهائي = وسعت.پڇاڙي - وسعت.شروعات;
    أرجع Py_Tok_ذريعو_اشارو(ذريعو, وسعت.شروعات);
}

/* Return the first matching offset within span, or -1 if absent. */
ساكن مضمن _PyTok_Off
Py_Tok_ذريعو_ڳوليو_Byte(ثابت Py_Tok_ذريعو_متن *ذريعو, Py_Tok_وسعت وسعت,
                      صحيح byte)
{
    Py_ssize_t ڊيگهائي;
    ثابت حرف *معلومات = Py_Tok_ذريعو_وسعت_View(ذريعو, وسعت, &ڊيگهائي);
    ثابت حرف *مليو = memchr(معلومات, byte, ڊيگهائي);
    أرجع مليو != عدم ? وسعت.شروعات + (مليو - معلومات) : -1;
}

PyAPI_FUNC(فراغ) Py_Tok_ذريعو_Init(Py_Tok_ذريعو_متن *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(فراغ) Py_Tok_ذريعو_Clear(Py_Tok_ذريعو_متن *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(فراغ) Py_Tok_ذريعو_Discard(Py_Tok_ذريعو_متن *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ذريعو_Append_سٽ(
    Py_Tok_ذريعو_متن *ذريعو, ثابت حرف *بائيٽون, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ذريعو_Append_سٽ_With_Newline(
    Py_Tok_ذريعو_متن *ذريعو, ثابت حرف *بائيٽون, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت حرف *) Py_Tok_ذريعو_سٽ_View(
    ثابت Py_Tok_ذريعو_متن *ذريعو, Py_ssize_t lineno, Py_ssize_t *len);

#جيڪڏهن_پورو
