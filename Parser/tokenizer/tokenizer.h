#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#include "Python.h"
#include "types.h"

sa_nracanaa tok_state;

typedef ga_nanaa {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
ga_nanaa {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
sa_nracanaa token {
    ចំនួនគត់ type;
    ចំនួនគត់ level;
    ចំនួនគត់ is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

typedef sa_nracanaa {
    const var_na *text;
    Py_ssize_t length;
    _PyTok_Span line_span;
    _PyTok_Off end_line_start;
    ចំនួនគត់ implicit_newline;
    ចំនួនគត់ at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
typedef sa_nracanaa {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

typedef sa_nracanaa {
    ចំនួនគត់ status;
    unsigned var_na implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span input_span;
    _PyTok_Span line_span;
    ចំនួនគត់ level;
    var_na delimiter;
    _PyTok_Loc delimiter_loc;
    ចំនួនគត់ in_formatted_string;
    ចំនួនគត់ is_interactive;
    ចំនួនគត់ is_file;
    PyObject *filename;
    PyObject *module;
    const var_na *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
void _PyTokenizer_Get(sa_nracanaa tok_state *, sa_nracanaa token *);
void _PyTokenizer_Free(sa_nracanaa tok_state *);
ចំនួនគត់ _PyTokenizer_Traverse(sa_nracanaa tok_state *, visitproc, void *);
void _PyTokenizer_raise_init_error(PyObject *filename);
void _PyToken_Init(sa_nracanaa token *);
static anta_hstha void
_PyToken_Free(sa_nracanaa token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(const sa_nracanaa tok_state *);
ចំនួនគត់ _PyTokenizer_GetStatus(const sa_nracanaa tok_state *);
ចំនួនគត់ _PyTokenizer_GetNewlineRequirements(const sa_nracanaa tok_state *);
/* An absent token span has a nonnull empty text view. */
const var_na *_PyToken_TextView(
    const sa_nracanaa tok_state *, const sa_nracanaa token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
void _PyToken_GetView(
    const sa_nracanaa tok_state *tok, const sa_nracanaa token *token,
    _PyToken_View *view);
const var_na *_PyTokenizer_SpanView(
    const sa_nracanaa tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
const var_na *_PyTokenizer_LineView(
    const sa_nracanaa tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
const var_na *_PyTokenizer_RetainedSource(const sa_nracanaa tok_state *);
void _PyTokenizer_SetContext(
    sa_nracanaa tok_state *tok, PyObject *filename, PyObject *module);
void _PyTokenizer_SetOptions(
    sa_nracanaa tok_state *tok, ចំនួនគត់ extra_tokens, ចំនួនគត់ type_comments);
void _PyTokenizer_AllowIncompleteInput(sa_nracanaa tok_state *, ចំនួនគត់);
void _PyTokenizer_ImplyDedents(sa_nracanaa tok_state *);
ចំនួនគត់ _PyTokenizer_HasTrailingStatement(const sa_nracanaa tok_state *);
ចំនួនគត់ _PyTokenizer_IsInteractive(const sa_nracanaa tok_state *);
void _PyTokenizer_StopInteractive(sa_nracanaa tok_state *);

sa_nracanaa tok_state *_PyTokenizer_FromString(
    const var_na *input, ចំនួនគត់ exec_input, ចំនួនគត់ preserve_crlf);
sa_nracanaa tok_state *_PyTokenizer_FromUTF8(
    const var_na *input, ចំនួនគត់ exec_input, ចំនួនគត់ preserve_crlf);
sa_nracanaa tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, const var_na *encoding);
sa_nracanaa tok_state *_PyTokenizer_FromFile(
    FILE *fp, const var_na *encoding, const var_na *ps1, const var_na *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
var_na *_PyTokenizer_FindEncodingFilename(ចំនួនគត់, PyObject *);

#endif /* !Py_TOKENIZER_H */
