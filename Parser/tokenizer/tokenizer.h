#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "types.h"

ସଂରଚନା tok_state;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
ଗଣନା {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ସଂରଚନା token {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅଚଳ ଅକ୍ଷର *text;
    Py_ssize_t length;
    _PyTok_Span ଧାଡ଼ି_span;
    _PyTok_Off end_ଧାଡ଼ି_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା implicit_newline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;
    unsigned ଅକ୍ଷର implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span ନିବେଶ_span;
    _PyTok_Span ଧାଡ଼ି_span;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;
    ଅକ୍ଷର delimiter;
    _PyTok_Loc delimiter_loc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_formatted_string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_interactive;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_file;
    PyObject *filename;
    PyObject *module;
    ଅଚଳ ଅକ୍ଷର *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
ଶୂନ୍ୟ _PyTokenizer_Get(ସଂରଚନା tok_state *, ସଂରଚନା token *);
ଶୂନ୍ୟ _PyTokenizer_Free(ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_Traverse(ସଂରଚନା tok_state *, visitproc, ଶୂନ୍ୟ *);
ଶୂନ୍ୟ _PyTokenizer_raise_init_error(PyObject *filename);
ଶୂନ୍ୟ _PyToken_Init(ସଂରଚନା token *);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
_PyToken_Free(ସଂରଚନା token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ଅଚଳ ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_GetStatus(ଅଚଳ ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_GetNewlineRequirements(ଅଚଳ ସଂରଚନା tok_state *);
/* An absent token span has a nonnull empty text view. */
ଅଚଳ ଅକ୍ଷର *_PyToken_TextView(
    ଅଚଳ ସଂରଚନା tok_state *, ଅଚଳ ସଂରଚନା token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
ଶୂନ୍ୟ _PyToken_GetView(
    ଅଚଳ ସଂରଚନା tok_state *tok, ଅଚଳ ସଂରଚନା token *token,
    _PyToken_View *view);
ଅଚଳ ଅକ୍ଷର *_PyTokenizer_SpanView(
    ଅଚଳ ସଂରଚନା tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ଅଚଳ ଅକ୍ଷର *Py_Tokenizer_ଧାଡ଼ି_View(
    ଅଚଳ ସଂରଚନା tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ଅଚଳ ଅକ୍ଷର *_PyTokenizer_RetainedSource(ଅଚଳ ସଂରଚନା tok_state *);
ଶୂନ୍ୟ _PyTokenizer_SetContext(
    ସଂରଚନା tok_state *tok, PyObject *filename, PyObject *module);
ଶୂନ୍ୟ _PyTokenizer_SetOptions(
    ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_tokens, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_comments);
ଶୂନ୍ୟ Py_Tokenizer_Allow_Incomplete_ନିବେଶ(ସଂରଚନା tok_state *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ _PyTokenizer_ImplyDedents(ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_HasTrailingStatement(ଅଚଳ ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_IsInteractive(ଅଚଳ ସଂରଚନା tok_state *);
ଶୂନ୍ୟ _PyTokenizer_StopInteractive(ସଂରଚନା tok_state *);

ସଂରଚନା tok_state *_PyTokenizer_FromString(
    ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exec_ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf);
ସଂରଚନା tok_state *_PyTokenizer_FromUTF8(
    ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exec_ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf);
ସଂରଚନା tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ଅଚଳ ଅକ୍ଷର *encoding);
ସଂରଚନା tok_state *_PyTokenizer_FromFile(
    FILE *fp, ଅଚଳ ଅକ୍ଷର *encoding, ଅଚଳ ଅକ୍ଷର *ps1, ଅଚଳ ଅକ୍ଷର *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
ଅକ୍ଷର *_PyTokenizer_FindEncodingFilename(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyObject *);

#endif /* !Py_TOKENIZER_H */
