#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "types.h"

সংরচনা tok_state;

প্রকারনির্দেশ গণনা {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
গণনা {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
সংরচনা token {
    পূর্ণাঙ্ক type;
    পূর্ণাঙ্ক level;
    পূর্ণাঙ্ক is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

প্রকারনির্দেশ সংরচনা {
    ধ্রুব বর্ণ *text;
    Py_ssize_t length;
    _PyTok_Span সারি_span;
    _PyTok_Off end_সারি_start;
    পূর্ণাঙ্ক implicit_newline;
    পূর্ণাঙ্ক at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
প্রকারনির্দেশ সংরচনা {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক status;
    unsigned বর্ণ implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span প্রবেশ_span;
    _PyTok_Span সারি_span;
    পূর্ণাঙ্ক level;
    বর্ণ delimiter;
    _PyTok_Loc delimiter_loc;
    পূর্ণাঙ্ক in_formatted_string;
    পূর্ণাঙ্ক is_interactive;
    পূর্ণাঙ্ক is_file;
    PyObject *filename;
    PyObject *module;
    ধ্রুব বর্ণ *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
শূন্য _PyTokenizer_Get(সংরচনা tok_state *, সংরচনা token *);
শূন্য _PyTokenizer_Free(সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTokenizer_Traverse(সংরচনা tok_state *, visitproc, শূন্য *);
শূন্য _PyTokenizer_raise_init_error(PyObject *filename);
শূন্য _PyToken_Init(সংরচনা token *);
স্থির অন্তঃস্থ শূন্য
_PyToken_Free(সংরচনা token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ধ্রুব সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTokenizer_GetStatus(ধ্রুব সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTokenizer_GetNewlineRequirements(ধ্রুব সংরচনা tok_state *);
/* An absent token span has a nonnull empty text view. */
ধ্রুব বর্ণ *_PyToken_TextView(
    ধ্রুব সংরচনা tok_state *, ধ্রুব সংরচনা token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
শূন্য _PyToken_GetView(
    ধ্রুব সংরচনা tok_state *tok, ধ্রুব সংরচনা token *token,
    _PyToken_View *view);
ধ্রুব বর্ণ *_PyTokenizer_SpanView(
    ধ্রুব সংরচনা tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ধ্রুব বর্ণ *Py_Tokenizer_সারি_View(
    ধ্রুব সংরচনা tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ধ্রুব বর্ণ *_PyTokenizer_RetainedSource(ধ্রুব সংরচনা tok_state *);
শূন্য _PyTokenizer_SetContext(
    সংরচনা tok_state *tok, PyObject *filename, PyObject *module);
শূন্য _PyTokenizer_SetOptions(
    সংরচনা tok_state *tok, পূর্ণাঙ্ক extra_tokens, পূর্ণাঙ্ক type_comments);
শূন্য Py_Tokenizer_Allow_Incomplete_প্রবেশ(সংরচনা tok_state *, পূর্ণাঙ্ক);
শূন্য _PyTokenizer_ImplyDedents(সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTokenizer_HasTrailingStatement(ধ্রুব সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTokenizer_IsInteractive(ধ্রুব সংরচনা tok_state *);
শূন্য _PyTokenizer_StopInteractive(সংরচনা tok_state *);

সংরচনা tok_state *_PyTokenizer_FromString(
    ধ্রুব বর্ণ *প্রবেশ, পূর্ণাঙ্ক exec_প্রবেশ, পূর্ণাঙ্ক preserve_crlf);
সংরচনা tok_state *_PyTokenizer_FromUTF8(
    ধ্রুব বর্ণ *প্রবেশ, পূর্ণাঙ্ক exec_প্রবেশ, পূর্ণাঙ্ক preserve_crlf);
সংরচনা tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ধ্রুব বর্ণ *encoding);
সংরচনা tok_state *_PyTokenizer_FromFile(
    FILE *fp, ধ্রুব বর্ণ *encoding, ধ্রুব বর্ণ *ps1, ধ্রুব বর্ণ *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
বর্ণ *_PyTokenizer_FindEncodingFilename(পূর্ণাঙ্ক, PyObject *);

#endif /* !Py_TOKENIZER_H */
