#यदि_अपरिभाषितम् Py_TOKENIZER_SOURCE_H
#परिभाषय Py_TOKENIZER_SOURCE_H

#अन्तर्भाव "Python.h"

#अन्तर्भाव "types.h"

प्रकारनिर्देश संरचना {
    वर्ण *अष्टकानि;
    _PyTok_Off आधारः_विस्थापनम्;
    _PyTok_Off len;
    _PyTok_Off cap;
    पूर्णाङ्क nlines;
} _PyTok_SourceText;

स्थिर अन्तःस्थ ध्रुव वर्ण *
Py_Tok_Source_दत्त(ध्रुव _PyTok_SourceText *source)
{
    प्रत्यागम source->अष्टकानि != रिक्त ? source->अष्टकानि : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
स्थिर अन्तःस्थ ध्रुव वर्ण *
_PyTok_SourcePointer(ध्रुव _PyTok_SourceText *source, _PyTok_Off विस्थापनम्)
{
    प्रतिज्ञा(विस्थापनम् >= source->आधारः_विस्थापनम्);
    प्रतिज्ञा(विस्थापनम् - source->आधारः_विस्थापनम् <= source->len);
    प्रत्यागम Py_Tok_Source_दत्त(source) + (विस्थापनम् - source->आधारः_विस्थापनम्);
}

स्थिर अन्तःस्थ अचिह्नित वर्ण
_PyTok_SourceByte(ध्रुव _PyTok_SourceText *source, _PyTok_Off विस्थापनम्)
{
    प्रतिज्ञा(विस्थापनम् >= source->आधारः_विस्थापनम्);
    प्रतिज्ञा(विस्थापनम् - source->आधारः_विस्थापनम् < source->len);
    प्रत्यागम (अचिह्नित वर्ण)source->अष्टकानि[विस्थापनम् - source->आधारः_विस्थापनम्];
}

स्थिर अन्तःस्थ ध्रुव वर्ण *
Py_Tok_Source_विस्तृतिः_View(ध्रुव _PyTok_SourceText *source, Py_Tok_विस्तृतिः विस्तृतिः,
                      Py_ssize_t *दीर्घता_2)
{
    प्रतिज्ञा(दीर्घता_2 != रिक्त);
    प्रतिज्ञा(Py_Tok_विस्तृतिः_Is_Valid(विस्तृतिः));
    प्रतिज्ञा(विस्तृतिः.अन्तः_2 - source->आधारः_विस्थापनम् <= source->len);
    *दीर्घता_2 = विस्तृतिः.अन्तः_2 - विस्तृतिः.start;
    प्रत्यागम _PyTok_SourcePointer(source, विस्तृतिः.start);
}

/* Return the first matching offset within span, or -1 if absent. */
स्थिर अन्तःस्थ _PyTok_Off
_PyTok_SourceFindByte(ध्रुव _PyTok_SourceText *source, Py_Tok_विस्तृतिः विस्तृतिः,
                      पूर्णाङ्क byte)
{
    Py_ssize_t दीर्घता_2;
    ध्रुव वर्ण *दत्त = Py_Tok_Source_विस्तृतिः_View(source, विस्तृतिः, &दीर्घता_2);
    ध्रुव वर्ण *प्राप्तम् = memchr(दत्त, byte, दीर्घता_2);
    प्रत्यागम प्राप्तम् != रिक्त ? विस्तृतिः.start + (प्राप्तम् - दत्त) : -1;
}

PyAPI_FUNC(शून्य) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(शून्य) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(शून्य) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_पङ्क्तिः(
    _PyTok_SourceText *source, ध्रुव वर्ण *अष्टकानि, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_पङ्क्तिः_With_Newline(
    _PyTok_SourceText *source, ध्रुव वर्ण *अष्टकानि, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ध्रुव वर्ण *) Py_Tok_Source_पङ्क्तिः_View(
    ध्रुव _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#यदि_अन्तः
