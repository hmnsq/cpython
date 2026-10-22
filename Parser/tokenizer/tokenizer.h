#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Py_TOKENIZER_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_TOKENIZER_H

#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"

ਬਣਤਰ tok_state;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
ਗਿਣਤੀ {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ਬਣਤਰ token {
    ਪੂਰਨ type;
    ਪੂਰਨ level;
    ਪੂਰਨ is_raw;
    Py_Tok_ਵਿਸਤਾਰ ਵਿਸਤਾਰ;
    _PyTok_Loc ਸ਼ੁਰੂ_loc;
    _PyTok_Loc ਸਿਰਾ_loc;
    PyObject *metadata;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ ਅੱਖਰ *ਪਾਠ;
    Py_ssize_t ਲੰਮਾਈ;
    Py_Tok_ਵਿਸਤਾਰ ਸਤਰ_ਵਿਸਤਾਰ;
    _PyTok_Off ਸਿਰਾ_ਸਤਰ_ਸ਼ੁਰੂ;
    ਪੂਰਨ implicit_newline;
    ਪੂਰਨ at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    _PyTok_Loc location;
    Py_Tok_ਵਿਸਤਾਰ ਪਾਠ_ਵਿਸਤਾਰ;
} _PyTokenizer_Diagnostic;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ ਸਥਿਤੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    Py_Tok_ਵਿਸਤਾਰ ਨਿਵੇਸ਼_ਵਿਸਤਾਰ;
    Py_Tok_ਵਿਸਤਾਰ ਸਤਰ_ਵਿਸਤਾਰ;
    ਪੂਰਨ level;
    ਅੱਖਰ delimiter;
    _PyTok_Loc delimiter_loc;
    ਪੂਰਨ ਵਿੱਚ_formatted_string;
    ਪੂਰਨ is_interactive;
    ਪੂਰਨ is_file;
    PyObject *filename;
    PyObject *module;
    ਅਚਲ ਅੱਖਰ *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
ਖਾਲੀ _PyTokenizer_Get(ਬਣਤਰ tok_state *, ਬਣਤਰ token *);
ਖਾਲੀ _PyTokenizer_Free(ਬਣਤਰ tok_state *);
ਪੂਰਨ _PyTokenizer_Traverse(ਬਣਤਰ tok_state *, visitproc, ਖਾਲੀ *);
ਖਾਲੀ _PyTokenizer_raise_init_error(PyObject *filename);
ਖਾਲੀ _PyToken_Init(ਬਣਤਰ token *);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
_PyToken_Free(ਬਣਤਰ token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ਅਚਲ ਬਣਤਰ tok_state *);
ਪੂਰਨ Py_Tokenizer_Get_ਸਥਿਤੀ(ਅਚਲ ਬਣਤਰ tok_state *);
ਪੂਰਨ _PyTokenizer_GetNewlineRequirements(ਅਚਲ ਬਣਤਰ tok_state *);
/* An absent token span has a nonnull empty text view. */
ਅਚਲ ਅੱਖਰ *Py_Token_ਪਾਠ_View(
    ਅਚਲ ਬਣਤਰ tok_state *, ਅਚਲ ਬਣਤਰ token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
ਖਾਲੀ _PyToken_GetView(
    ਅਚਲ ਬਣਤਰ tok_state *tok, ਅਚਲ ਬਣਤਰ token *token,
    _PyToken_View *view);
ਅਚਲ ਅੱਖਰ *Py_Tokenizer_ਵਿਸਤਾਰ_View(
    ਅਚਲ ਬਣਤਰ tok_state *, Py_Tok_ਵਿਸਤਾਰ, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ਅਚਲ ਅੱਖਰ *Py_Tokenizer_ਸਤਰ_View(
    ਅਚਲ ਬਣਤਰ tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ਅਚਲ ਅੱਖਰ *Py_Tokenizer_Retained_ਸਰੋਤ(ਅਚਲ ਬਣਤਰ tok_state *);
ਖਾਲੀ Py_Tokenizer_ਸੈੱਟ_Context(
    ਬਣਤਰ tok_state *tok, PyObject *filename, PyObject *module);
ਖਾਲੀ Py_Tokenizer_ਸੈੱਟ_Options(
    ਬਣਤਰ tok_state *tok, ਪੂਰਨ extra_ਟੋਕਨ, ਪੂਰਨ type_comments);
ਖਾਲੀ Py_Tokenizer_Allow_Incomplete_ਨਿਵੇਸ਼(ਬਣਤਰ tok_state *, ਪੂਰਨ);
ਖਾਲੀ _PyTokenizer_ImplyDedents(ਬਣਤਰ tok_state *);
ਪੂਰਨ _PyTokenizer_HasTrailingStatement(ਅਚਲ ਬਣਤਰ tok_state *);
ਪੂਰਨ _PyTokenizer_IsInteractive(ਅਚਲ ਬਣਤਰ tok_state *);
ਖਾਲੀ _PyTokenizer_StopInteractive(ਬਣਤਰ tok_state *);

ਬਣਤਰ tok_state *_PyTokenizer_FromString(
    ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਪੂਰਨ exec_ਨਿਵੇਸ਼, ਪੂਰਨ preserve_crlf);
ਬਣਤਰ tok_state *_PyTokenizer_FromUTF8(
    ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਪੂਰਨ exec_ਨਿਵੇਸ਼, ਪੂਰਨ preserve_crlf);
ਬਣਤਰ tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ਅਚਲ ਅੱਖਰ *encoding);
ਬਣਤਰ tok_state *_PyTokenizer_FromFile(
    FILE *fp, ਅਚਲ ਅੱਖਰ *encoding, ਅਚਲ ਅੱਖਰ *ps1, ਅਚਲ ਅੱਖਰ *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
ਅੱਖਰ *Py_Tokenizer_ਲੱਭੋ_Encoding_Filename(ਪੂਰਨ, PyObject *);

#ਜੇ_ਅੰਤ /* !Py_TOKENIZER_H */
