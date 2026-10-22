#నిర్వచితం_కాకపోతే Py_TOKENIZER_ఆధారం_H
#నిర్వచించు Py_TOKENIZER_ఆధారం_H

#అన్తర్భావ "Python.h"

#అన్తర్భావ "types.h"

రకనిర్వచనం నిర్మాణం {
    అక్షరం *బైట్లు;
    _PyTok_Off పునాది_స్థానభ్రంశం;
    _PyTok_Off len;
    _PyTok_Off cap;
    పూర్ణాంకం nlines;
} Py_Tok_ఆధారం_పాఠ్యం;

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం *
Py_Tok_ఆధారం_దత్తాంశం(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం)
{
    తిరిగి ఆధారం->బైట్లు != ఖాళీ ? ఆధారం->బైట్లు : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
స్థిర అంతఃవరుస స్థిరరాశి అక్షరం *
Py_Tok_ఆధారం_సూచిక(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, _PyTok_Off స్థానభ్రంశం)
{
    ప్రతిజ్ఞా(స్థానభ్రంశం >= ఆధారం->పునాది_స్థానభ్రంశం);
    ప్రతిజ్ఞా(స్థానభ్రంశం - ఆధారం->పునాది_స్థానభ్రంశం <= ఆధారం->len);
    తిరిగి Py_Tok_ఆధారం_దత్తాంశం(ఆధారం) + (స్థానభ్రంశం - ఆధారం->పునాది_స్థానభ్రంశం);
}

స్థిర అంతఃవరుస గుర్తులేని అక్షరం
Py_Tok_ఆధారం_Byte(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, _PyTok_Off స్థానభ్రంశం)
{
    ప్రతిజ్ఞా(స్థానభ్రంశం >= ఆధారం->పునాది_స్థానభ్రంశం);
    ప్రతిజ్ఞా(స్థానభ్రంశం - ఆధారం->పునాది_స్థానభ్రంశం < ఆధారం->len);
    తిరిగి (గుర్తులేని అక్షరం)ఆధారం->బైట్లు[స్థానభ్రంశం - ఆధారం->పునాది_స్థానభ్రంశం];
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం *
Py_Tok_ఆధారం_పరిధి_View(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, Py_Tok_పరిధి పరిధి,
                      Py_ssize_t *నిడివి)
{
    ప్రతిజ్ఞా(నిడివి != ఖాళీ);
    ప్రతిజ్ఞా(Py_Tok_పరిధి_Is_Valid(పరిధి));
    ప్రతిజ్ఞా(పరిధి.చివర - ఆధారం->పునాది_స్థానభ్రంశం <= ఆధారం->len);
    *నిడివి = పరిధి.చివర - పరిధి.ప్రారంభం;
    తిరిగి Py_Tok_ఆధారం_సూచిక(ఆధారం, పరిధి.ప్రారంభం);
}

/* Return the first matching offset within span, or -1 if absent. */
స్థిర అంతఃవరుస _PyTok_Off
Py_Tok_ఆధారం_కనుగొను_Byte(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, Py_Tok_పరిధి పరిధి,
                      పూర్ణాంకం byte)
{
    Py_ssize_t నిడివి;
    స్థిరరాశి అక్షరం *దత్తాంశం = Py_Tok_ఆధారం_పరిధి_View(ఆధారం, పరిధి, &నిడివి);
    స్థిరరాశి అక్షరం *దొరికింది = memchr(దత్తాంశం, byte, నిడివి);
    తిరిగి దొరికింది != ఖాళీ ? పరిధి.ప్రారంభం + (దొరికింది - దత్తాంశం) : -1;
}

PyAPI_FUNC(శూన్య) Py_Tok_ఆధారం_Init(Py_Tok_ఆధారం_పాఠ్యం *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(శూన్య) Py_Tok_ఆధారం_Clear(Py_Tok_ఆధారం_పాఠ్యం *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(శూన్య) Py_Tok_ఆధారం_Discard(Py_Tok_ఆధారం_పాఠ్యం *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ఆధారం_Append_పంక్తి(
    Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, స్థిరరాశి అక్షరం *బైట్లు, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ఆధారం_Append_పంక్తి_With_Newline(
    Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, స్థిరరాశి అక్షరం *బైట్లు, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(స్థిరరాశి అక్షరం *) Py_Tok_ఆధారం_పంక్తి_View(
    స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, Py_ssize_t lineno, Py_ssize_t *len);

#అయితే_ముగింపు
