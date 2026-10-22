#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Py_TOKENIZER_H
#ವ್ಯಾಖ್ಯಾನಿಸು Py_TOKENIZER_H

#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "types.h"

ರಚನೆ tok_state;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
ಎಣಿಕೆ {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ರಚನೆ token {
    ಪೂರ್ಣಾಂಕ type;
    ಪೂರ್ಣಾಂಕ level;
    ಪೂರ್ಣಾಂಕ is_raw;
    Py_Tok_ಹರವು ಹರವು;
    _PyTok_Loc ಆರಂಭ_loc;
    _PyTok_Loc ಕೊನೆ_loc;
    PyObject *metadata;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ;
    Py_ssize_t ಉದ್ದಳತೆ;
    Py_Tok_ಹರವು ಸಾಲು_ಹರವು;
    _PyTok_Off ಕೊನೆ_ಸಾಲು_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ implicit_newline;
    ಪೂರ್ಣಾಂಕ at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    _PyTok_Loc location;
    Py_Tok_ಹರವು ಪಠ್ಯ_ಹರವು;
} _PyTokenizer_Diagnostic;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    Py_Tok_ಹರವು ಒಳಹರಿವು_ಹರವು;
    Py_Tok_ಹರವು ಸಾಲು_ಹರವು;
    ಪೂರ್ಣಾಂಕ level;
    ಅಕ್ಷರ delimiter;
    _PyTok_Loc delimiter_loc;
    ಪೂರ್ಣಾಂಕ ಒಳ_formatted_string;
    ಪೂರ್ಣಾಂಕ is_interactive;
    ಪೂರ್ಣಾಂಕ is_file;
    PyObject *filename;
    PyObject *module;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
ಶೂನ್ಯ _PyTokenizer_Get(ರಚನೆ tok_state *, ರಚನೆ token *);
ಶೂನ್ಯ _PyTokenizer_Free(ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ _PyTokenizer_Traverse(ರಚನೆ tok_state *, visitproc, ಶೂನ್ಯ *);
ಶೂನ್ಯ _PyTokenizer_raise_init_error(PyObject *filename);
ಶೂನ್ಯ _PyToken_Init(ರಚನೆ token *);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
_PyToken_Free(ರಚನೆ token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ Py_Tokenizer_Get_ಸ್ಥಿತಿ(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ _PyTokenizer_GetNewlineRequirements(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *);
/* An absent token span has a nonnull empty text view. */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Py_Token_ಪಠ್ಯ_View(
    ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *, ಸ್ಥಿರಾಂಕ ರಚನೆ token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
ಶೂನ್ಯ _PyToken_GetView(
    ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ರಚನೆ token *token,
    _PyToken_View *view);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Py_Tokenizer_ಹರವು_View(
    ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *, Py_Tok_ಹರವು, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Py_Tokenizer_ಸಾಲು_View(
    ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Py_Tokenizer_Retained_ಮೂಲ(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *);
ಶೂನ್ಯ Py_Tokenizer_ಗಣ_Context(
    ರಚನೆ tok_state *tok, PyObject *filename, PyObject *module);
ಶೂನ್ಯ Py_Tokenizer_ಗಣ_Options(
    ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ extra_ಟೋಕನುಗಳು, ಪೂರ್ಣಾಂಕ type_comments);
ಶೂನ್ಯ Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(ರಚನೆ tok_state *, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ _PyTokenizer_ImplyDedents(ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ _PyTokenizer_HasTrailingStatement(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ _PyTokenizer_IsInteractive(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *);
ಶೂನ್ಯ _PyTokenizer_StopInteractive(ರಚನೆ tok_state *);

ರಚನೆ tok_state *_PyTokenizer_FromString(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ preserve_crlf);
ರಚನೆ tok_state *_PyTokenizer_FromUTF8(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ preserve_crlf);
ರಚನೆ tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding);
ರಚನೆ tok_state *_PyTokenizer_FromFile(
    FILE *fp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
ಅಕ್ಷರ *Py_Tokenizer_ಕಂಡುಹಿಡಿ_Encoding_Filename(ಪೂರ್ಣಾಂಕ, PyObject *);

#ಆದರೆ_ಅಂತ್ಯ /* !Py_TOKENIZER_H */
