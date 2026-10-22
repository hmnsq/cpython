#اگر_غیر_تعریف_شدہ Py_TOKENIZER_ماخذ_H
#تعریف_کرو Py_TOKENIZER_ماخذ_H

#شامل "Python.h"

#شامل "types.h"

قسمتعریف ساخت {
    حرف *بائٹیں;
    _PyTok_Off بنیاد_انحراف;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحیح nlines;
} Py_Tok_ماخذ_متن;

ساکن اندرونی ثابت حرف *
Py_Tok_ماخذ_ڈیٹا(ثابت Py_Tok_ماخذ_متن *ماخذ)
{
    واپس ماخذ->بائٹیں != تہی ? ماخذ->بائٹیں : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ساکن اندرونی ثابت حرف *
Py_Tok_ماخذ_اشارہ_گر(ثابت Py_Tok_ماخذ_متن *ماخذ, _PyTok_Off انحراف)
{
    أكد(انحراف >= ماخذ->بنیاد_انحراف);
    أكد(انحراف - ماخذ->بنیاد_انحراف <= ماخذ->len);
    واپس Py_Tok_ماخذ_ڈیٹا(ماخذ) + (انحراف - ماخذ->بنیاد_انحراف);
}

ساکن اندرونی غیرعلامتی حرف
Py_Tok_ماخذ_Byte(ثابت Py_Tok_ماخذ_متن *ماخذ, _PyTok_Off انحراف)
{
    أكد(انحراف >= ماخذ->بنیاد_انحراف);
    أكد(انحراف - ماخذ->بنیاد_انحراف < ماخذ->len);
    واپس (غیرعلامتی حرف)ماخذ->بائٹیں[انحراف - ماخذ->بنیاد_انحراف];
}

ساکن اندرونی ثابت حرف *
Py_Tok_ماخذ_وسعت_View(ثابت Py_Tok_ماخذ_متن *ماخذ, Py_Tok_وسعت وسعت,
                      Py_ssize_t *لمبائی_2)
{
    أكد(لمبائی_2 != تہی);
    أكد(Py_Tok_وسعت_Is_Valid(وسعت));
    أكد(وسعت.انتہا - ماخذ->بنیاد_انحراف <= ماخذ->len);
    *لمبائی_2 = وسعت.انتہا - وسعت.آغاز;
    واپس Py_Tok_ماخذ_اشارہ_گر(ماخذ, وسعت.آغاز);
}

/* Return the first matching offset within span, or -1 if absent. */
ساکن اندرونی _PyTok_Off
Py_Tok_ماخذ_ڈھونڈو_Byte(ثابت Py_Tok_ماخذ_متن *ماخذ, Py_Tok_وسعت وسعت,
                      صحیح byte)
{
    Py_ssize_t لمبائی_2;
    ثابت حرف *ڈیٹا = Py_Tok_ماخذ_وسعت_View(ماخذ, وسعت, &لمبائی_2);
    ثابت حرف *ملا = memchr(ڈیٹا, byte, لمبائی_2);
    واپس ملا != تہی ? وسعت.آغاز + (ملا - ڈیٹا) : -1;
}

PyAPI_FUNC(خالی) Py_Tok_ماخذ_Init(Py_Tok_ماخذ_متن *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(خالی) Py_Tok_ماخذ_Clear(Py_Tok_ماخذ_متن *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(خالی) Py_Tok_ماخذ_Discard(Py_Tok_ماخذ_متن *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ماخذ_Append_سطر(
    Py_Tok_ماخذ_متن *ماخذ, ثابت حرف *بائٹیں, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ماخذ_Append_سطر_With_Newline(
    Py_Tok_ماخذ_متن *ماخذ, ثابت حرف *بائٹیں, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت حرف *) Py_Tok_ماخذ_سطر_View(
    ثابت Py_Tok_ماخذ_متن *ماخذ, Py_ssize_t lineno, Py_ssize_t *len);

#اگر_ختم
