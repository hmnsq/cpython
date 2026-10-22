#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#شامل "Python.h"
#شامل "types.h"

ساختار tok_state;

تعریفنوع شمارش {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
شمارش {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ساختار token {
    صحیح type;
    صحیح level;
    صحیح is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

تعریفنوع ساختار {
    ثابت نویسه *text;
    Py_ssize_t length;
    _PyTok_Span سطر_span;
    _PyTok_Off end_سطر_start;
    صحیح implicit_newline;
    صحیح at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
تعریفنوع ساختار {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

تعریفنوع ساختار {
    صحیح status;
    unsigned نویسه implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span درون_داد_span;
    _PyTok_Span سطر_span;
    صحیح level;
    نویسه delimiter;
    _PyTok_Loc delimiter_loc;
    صحیح in_formatted_string;
    صحیح is_interactive;
    صحیح is_file;
    PyObject *filename;
    PyObject *module;
    ثابت نویسه *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
تهی _PyTokenizer_Get(ساختار tok_state *, ساختار token *);
تهی _PyTokenizer_Free(ساختار tok_state *);
صحیح _PyTokenizer_Traverse(ساختار tok_state *, visitproc, تهی *);
تهی _PyTokenizer_raise_init_error(PyObject *filename);
تهی _PyToken_Init(ساختار token *);
ایستا درونخطی تهی
_PyToken_Free(ساختار token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ثابت ساختار tok_state *);
صحیح _PyTokenizer_GetStatus(ثابت ساختار tok_state *);
صحیح _PyTokenizer_GetNewlineRequirements(ثابت ساختار tok_state *);
/* An absent token span has a nonnull empty text view. */
ثابت نویسه *_PyToken_TextView(
    ثابت ساختار tok_state *, ثابت ساختار token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
تهی _PyToken_GetView(
    ثابت ساختار tok_state *tok, ثابت ساختار token *token,
    _PyToken_View *view);
ثابت نویسه *_PyTokenizer_SpanView(
    ثابت ساختار tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ثابت نویسه *Py_Tokenizer_سطر_View(
    ثابت ساختار tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ثابت نویسه *_PyTokenizer_RetainedSource(ثابت ساختار tok_state *);
تهی _PyTokenizer_SetContext(
    ساختار tok_state *tok, PyObject *filename, PyObject *module);
تهی _PyTokenizer_SetOptions(
    ساختار tok_state *tok, صحیح extra_tokens, صحیح type_comments);
تهی Py_Tokenizer_Allow_Incomplete_درون_داد(ساختار tok_state *, صحیح);
تهی _PyTokenizer_ImplyDedents(ساختار tok_state *);
صحیح _PyTokenizer_HasTrailingStatement(ثابت ساختار tok_state *);
صحیح _PyTokenizer_IsInteractive(ثابت ساختار tok_state *);
تهی _PyTokenizer_StopInteractive(ساختار tok_state *);

ساختار tok_state *_PyTokenizer_FromString(
    ثابت نویسه *درون_داد, صحیح exec_درون_داد, صحیح preserve_crlf);
ساختار tok_state *_PyTokenizer_FromUTF8(
    ثابت نویسه *درون_داد, صحیح exec_درون_داد, صحیح preserve_crlf);
ساختار tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ثابت نویسه *encoding);
ساختار tok_state *_PyTokenizer_FromFile(
    FILE *fp, ثابت نویسه *encoding, ثابت نویسه *ps1, ثابت نویسه *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
نویسه *_PyTokenizer_FindEncodingFilename(صحیح, PyObject *);

#endif /* !Py_TOKENIZER_H */
