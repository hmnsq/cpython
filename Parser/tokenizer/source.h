#که_نه_تعریف_شوی Py_TOKENIZER_سرچینه_H
#تعریف_کړه Py_TOKENIZER_سرچینه_H

#ضمن "Python.h"

#ضمن "types.h"

تعريفنوع بنية {
    حرف *بایټونه;
    _PyTok_Off بنسټ_دننه_واټن;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحيح nlines;
} Py_Tok_سرچینه_متن;

ساكن مضمن ثابت حرف *
Py_Tok_سرچینه_معلومات(ثابت Py_Tok_سرچینه_متن *سرچینه)
{
    أرجع سرچینه->بایټونه != عدم ? سرچینه->بایټونه : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ساكن مضمن ثابت حرف *
Py_Tok_سرچینه_اشاره_ګر(ثابت Py_Tok_سرچینه_متن *سرچینه, _PyTok_Off دننه_واټن)
{
    أكد(دننه_واټن >= سرچینه->بنسټ_دننه_واټن);
    أكد(دننه_واټن - سرچینه->بنسټ_دننه_واټن <= سرچینه->len);
    أرجع Py_Tok_سرچینه_معلومات(سرچینه) + (دننه_واټن - سرچینه->بنسټ_دننه_واټن);
}

ساكن مضمن بینښه حرف
Py_Tok_سرچینه_Byte(ثابت Py_Tok_سرچینه_متن *سرچینه, _PyTok_Off دننه_واټن)
{
    أكد(دننه_واټن >= سرچینه->بنسټ_دننه_واټن);
    أكد(دننه_واټن - سرچینه->بنسټ_دننه_واټن < سرچینه->len);
    أرجع (بینښه حرف)سرچینه->بایټونه[دننه_واټن - سرچینه->بنسټ_دننه_واټن];
}

ساكن مضمن ثابت حرف *
Py_Tok_سرچینه_پراختیا_View(ثابت Py_Tok_سرچینه_متن *سرچینه, Py_Tok_پراختیا پراختیا,
                      Py_ssize_t *اوږدتیا)
{
    أكد(اوږدتیا != عدم);
    أكد(Py_Tok_پراختیا_Is_Valid(پراختیا));
    أكد(پراختیا.پای_ټکی - سرچینه->بنسټ_دننه_واټن <= سرچینه->len);
    *اوږدتیا = پراختیا.پای_ټکی - پراختیا.پیل;
    أرجع Py_Tok_سرچینه_اشاره_ګر(سرچینه, پراختیا.پیل);
}

/* Return the first matching offset within span, or -1 if absent. */
ساكن مضمن _PyTok_Off
Py_Tok_سرچینه_پیدا_کړه_Byte(ثابت Py_Tok_سرچینه_متن *سرچینه, Py_Tok_پراختیا پراختیا,
                      صحيح byte)
{
    Py_ssize_t اوږدتیا;
    ثابت حرف *معلومات = Py_Tok_سرچینه_پراختیا_View(سرچینه, پراختیا, &اوږدتیا);
    ثابت حرف *وموندل = memchr(معلومات, byte, اوږدتیا);
    أرجع وموندل != عدم ? پراختیا.پیل + (وموندل - معلومات) : -1;
}

PyAPI_FUNC(فراغ) Py_Tok_سرچینه_Init(Py_Tok_سرچینه_متن *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(فراغ) Py_Tok_سرچینه_Clear(Py_Tok_سرچینه_متن *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(فراغ) Py_Tok_سرچینه_Discard(Py_Tok_سرچینه_متن *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_سرچینه_Append_کرښه(
    Py_Tok_سرچینه_متن *سرچینه, ثابت حرف *بایټونه, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_سرچینه_Append_کرښه_With_Newline(
    Py_Tok_سرچینه_متن *سرچینه, ثابت حرف *بایټونه, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت حرف *) Py_Tok_سرچینه_کرښه_View(
    ثابت Py_Tok_سرچینه_متن *سرچینه, Py_ssize_t lineno, Py_ssize_t *len);

#د_که_پای
