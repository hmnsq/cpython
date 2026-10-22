#اگر_تعریف_نشده Py_TOKENIZER_H
#تعریف_کن Py_TOKENIZER_H

#ضمن "Python.h"
#ضمن "types.h"

بنية tok_state;

تعريفنوع تعداد {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
تعداد {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
بنية token {
    صحيح type;
    صحيح level;
    صحيح is_raw;
    Py_Tok_بازه بازه;
    _PyTok_Loc شروع_loc;
    _PyTok_Loc انتها_loc;
    PyObject *metadata;
};

تعريفنوع بنية {
    ثابت حرف *متن;
    Py_ssize_t درازی;
    Py_Tok_بازه سطر_بازه;
    _PyTok_Off انتها_سطر_شروع;
    صحيح implicit_newline;
    صحيح at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
تعريفنوع بنية {
    _PyTok_Loc location;
    Py_Tok_بازه متن_بازه;
} _PyTokenizer_Diagnostic;

تعريفنوع بنية {
    صحيح وضعیت;
    بیعلامت حرف implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    Py_Tok_بازه درون_داد_بازه;
    Py_Tok_بازه سطر_بازه;
    صحيح level;
    حرف delimiter;
    _PyTok_Loc delimiter_loc;
    صحيح درون_formatted_string;
    صحيح is_interactive;
    صحيح is_file;
    PyObject *filename;
    PyObject *module;
    ثابت حرف *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
فراغ _PyTokenizer_Get(بنية tok_state *, بنية token *);
فراغ _PyTokenizer_Free(بنية tok_state *);
صحيح _PyTokenizer_Traverse(بنية tok_state *, visitproc, فراغ *);
فراغ _PyTokenizer_raise_init_error(PyObject *filename);
فراغ _PyToken_Init(بنية token *);
ساكن مضمن فراغ
_PyToken_Free(بنية token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ثابت بنية tok_state *);
صحيح Py_Tokenizer_Get_وضعیت(ثابت بنية tok_state *);
صحيح _PyTokenizer_GetNewlineRequirements(ثابت بنية tok_state *);
/* An absent token span has a nonnull empty text view. */
ثابت حرف *Py_Token_متن_View(
    ثابت بنية tok_state *, ثابت بنية token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
فراغ _PyToken_GetView(
    ثابت بنية tok_state *tok, ثابت بنية token *token,
    _PyToken_View *view);
ثابت حرف *Py_Tokenizer_بازه_View(
    ثابت بنية tok_state *, Py_Tok_بازه, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ثابت حرف *Py_Tokenizer_سطر_View(
    ثابت بنية tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ثابت حرف *Py_Tokenizer_Retained_منبع(ثابت بنية tok_state *);
فراغ Py_Tokenizer_مجموعه_Context(
    بنية tok_state *tok, PyObject *filename, PyObject *module);
فراغ Py_Tokenizer_مجموعه_Options(
    بنية tok_state *tok, صحيح extra_نشانهها, صحيح type_comments);
فراغ Py_Tokenizer_Allow_Incomplete_درون_داد(بنية tok_state *, صحيح);
فراغ _PyTokenizer_ImplyDedents(بنية tok_state *);
صحيح _PyTokenizer_HasTrailingStatement(ثابت بنية tok_state *);
صحيح _PyTokenizer_IsInteractive(ثابت بنية tok_state *);
فراغ _PyTokenizer_StopInteractive(بنية tok_state *);

بنية tok_state *_PyTokenizer_FromString(
    ثابت حرف *درون_داد, صحيح exec_درون_داد, صحيح preserve_crlf);
بنية tok_state *_PyTokenizer_FromUTF8(
    ثابت حرف *درون_داد, صحيح exec_درون_داد, صحيح preserve_crlf);
بنية tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ثابت حرف *encoding);
بنية tok_state *_PyTokenizer_FromFile(
    FILE *fp, ثابت حرف *encoding, ثابت حرف *ps1, ثابت حرف *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
حرف *Py_Tokenizer_پیدا_کن_Encoding_Filename(صحيح, PyObject *);

#پایان_اگر /* !Py_TOKENIZER_H */
