#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "types.h"

સંરચના tok_state;

પ્રકારવ્યાખ્યા ગણના {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
ગણના {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
સંરચના token {
    પૂર્ણાંક type;
    પૂર્ણાંક level;
    પૂર્ણાંક is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

પ્રકારવ્યાખ્યા સંરચના {
    અચલ અક્ષર *text;
    Py_ssize_t length;
    _PyTok_Span લીટી_span;
    _PyTok_Off end_લીટી_start;
    પૂર્ણાંક implicit_newline;
    પૂર્ણાંક at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
પ્રકારવ્યાખ્યા સંરચના {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક status;
    unsigned અક્ષર implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span આગત_span;
    _PyTok_Span લીટી_span;
    પૂર્ણાંક level;
    અક્ષર delimiter;
    _PyTok_Loc delimiter_loc;
    પૂર્ણાંક in_formatted_string;
    પૂર્ણાંક is_interactive;
    પૂર્ણાંક is_file;
    PyObject *filename;
    PyObject *module;
    અચલ અક્ષર *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
શૂન્ય _PyTokenizer_Get(સંરચના tok_state *, સંરચના token *);
શૂન્ય _PyTokenizer_Free(સંરચના tok_state *);
પૂર્ણાંક _PyTokenizer_Traverse(સંરચના tok_state *, visitproc, શૂન્ય *);
શૂન્ય _PyTokenizer_raise_init_error(PyObject *filename);
શૂન્ય _PyToken_Init(સંરચના token *);
સ્થિર અંતઃરેખ શૂન્ય
_PyToken_Free(સંરચના token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(અચલ સંરચના tok_state *);
પૂર્ણાંક _PyTokenizer_GetStatus(અચલ સંરચના tok_state *);
પૂર્ણાંક _PyTokenizer_GetNewlineRequirements(અચલ સંરચના tok_state *);
/* An absent token span has a nonnull empty text view. */
અચલ અક્ષર *_PyToken_TextView(
    અચલ સંરચના tok_state *, અચલ સંરચના token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
શૂન્ય _PyToken_GetView(
    અચલ સંરચના tok_state *tok, અચલ સંરચના token *token,
    _PyToken_View *view);
અચલ અક્ષર *_PyTokenizer_SpanView(
    અચલ સંરચના tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
અચલ અક્ષર *Py_Tokenizer_લીટી_View(
    અચલ સંરચના tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
અચલ અક્ષર *_PyTokenizer_RetainedSource(અચલ સંરચના tok_state *);
શૂન્ય _PyTokenizer_SetContext(
    સંરચના tok_state *tok, PyObject *filename, PyObject *module);
શૂન્ય _PyTokenizer_SetOptions(
    સંરચના tok_state *tok, પૂર્ણાંક extra_tokens, પૂર્ણાંક type_comments);
શૂન્ય Py_Tokenizer_Allow_Incomplete_આગત(સંરચના tok_state *, પૂર્ણાંક);
શૂન્ય _PyTokenizer_ImplyDedents(સંરચના tok_state *);
પૂર્ણાંક _PyTokenizer_HasTrailingStatement(અચલ સંરચના tok_state *);
પૂર્ણાંક _PyTokenizer_IsInteractive(અચલ સંરચના tok_state *);
શૂન્ય _PyTokenizer_StopInteractive(સંરચના tok_state *);

સંરચના tok_state *_PyTokenizer_FromString(
    અચલ અક્ષર *આગત, પૂર્ણાંક exec_આગત, પૂર્ણાંક preserve_crlf);
સંરચના tok_state *_PyTokenizer_FromUTF8(
    અચલ અક્ષર *આગત, પૂર્ણાંક exec_આગત, પૂર્ણાંક preserve_crlf);
સંરચના tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, અચલ અક્ષર *encoding);
સંરચના tok_state *_PyTokenizer_FromFile(
    FILE *fp, અચલ અક્ષર *encoding, અચલ અક્ષર *ps1, અચલ અક્ષર *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
અક્ષર *_PyTokenizer_FindEncodingFilename(પૂર્ણાંક, PyObject *);

#endif /* !Py_TOKENIZER_H */
