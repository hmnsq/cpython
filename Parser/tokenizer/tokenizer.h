#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "types.h"

ಸಂರಚನಾ tok_state;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
ಗಣನಾ {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ಸಂರಚನಾ token {
    ಪೂರ್ಣಾಙ್ಕ type;
    ಪೂರ್ಣಾಙ್ಕ level;
    ಪೂರ್ಣಾಙ್ಕ is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಧ್ರುವ ವರ್ಣ *text;
    Py_ssize_t length;
    _PyTok_Span ಸಾಲು_span;
    _PyTok_Off end_ಸಾಲು_start;
    ಪೂರ್ಣಾಙ್ಕ implicit_newline;
    ಪೂರ್ಣಾಙ್ಕ at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ status;
    unsigned ವರ್ಣ implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span ಒಳಹರಿವು_span;
    _PyTok_Span ಸಾಲು_span;
    ಪೂರ್ಣಾಙ್ಕ level;
    ವರ್ಣ delimiter;
    _PyTok_Loc delimiter_loc;
    ಪೂರ್ಣಾಙ್ಕ in_formatted_string;
    ಪೂರ್ಣಾಙ್ಕ is_interactive;
    ಪೂರ್ಣಾಙ್ಕ is_file;
    PyObject *filename;
    PyObject *module;
    ಧ್ರುವ ವರ್ಣ *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
ಶೂನ್ಯ _PyTokenizer_Get(ಸಂರಚನಾ tok_state *, ಸಂರಚನಾ token *);
ಶೂನ್ಯ _PyTokenizer_Free(ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_Traverse(ಸಂರಚನಾ tok_state *, visitproc, ಶೂನ್ಯ *);
ಶೂನ್ಯ _PyTokenizer_raise_init_error(PyObject *filename);
ಶೂನ್ಯ _PyToken_Init(ಸಂರಚನಾ token *);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
_PyToken_Free(ಸಂರಚನಾ token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_GetStatus(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_GetNewlineRequirements(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
/* An absent token span has a nonnull empty text view. */
ಧ್ರುವ ವರ್ಣ *_PyToken_TextView(
    ಧ್ರುವ ಸಂರಚನಾ tok_state *, ಧ್ರುವ ಸಂರಚನಾ token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
ಶೂನ್ಯ _PyToken_GetView(
    ಧ್ರುವ ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ಸಂರಚನಾ token *token,
    _PyToken_View *view);
ಧ್ರುವ ವರ್ಣ *_PyTokenizer_SpanView(
    ಧ್ರುವ ಸಂರಚನಾ tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ಧ್ರುವ ವರ್ಣ *Py_Tokenizer_ಸಾಲು_View(
    ಧ್ರುವ ಸಂರಚನಾ tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ಧ್ರುವ ವರ್ಣ *_PyTokenizer_RetainedSource(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
ಶೂನ್ಯ _PyTokenizer_SetContext(
    ಸಂರಚನಾ tok_state *tok, PyObject *filename, PyObject *module);
ಶೂನ್ಯ _PyTokenizer_SetOptions(
    ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ extra_tokens, ಪೂರ್ಣಾಙ್ಕ type_comments);
ಶೂನ್ಯ Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(ಸಂರಚನಾ tok_state *, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ _PyTokenizer_ImplyDedents(ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_HasTrailingStatement(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_IsInteractive(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
ಶೂನ್ಯ _PyTokenizer_StopInteractive(ಸಂರಚನಾ tok_state *);

ಸಂರಚನಾ tok_state *_PyTokenizer_FromString(
    ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ preserve_crlf);
ಸಂರಚನಾ tok_state *_PyTokenizer_FromUTF8(
    ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ preserve_crlf);
ಸಂರಚನಾ tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ಧ್ರುವ ವರ್ಣ *encoding);
ಸಂರಚನಾ tok_state *_PyTokenizer_FromFile(
    FILE *fp, ಧ್ರುವ ವರ್ಣ *encoding, ಧ್ರುವ ವರ್ಣ *ps1, ಧ್ರುವ ವರ್ಣ *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
ವರ್ಣ *_PyTokenizer_FindEncodingFilename(ಪೂರ್ಣಾಙ್ಕ, PyObject *);

#endif /* !Py_TOKENIZER_H */
