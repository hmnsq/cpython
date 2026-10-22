#اگر_غیر_تعریف_شدہ Py_TOKENIZER_H
#تعریف_کرو Py_TOKENIZER_H

#شامل "Python.h"
#شامل "types.h"

ساخت tok_state;

قسمتعریف شمار {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
شمار {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ساخت token {
    صحیح type;
    صحیح level;
    صحیح is_raw;
    Py_Tok_وسعت وسعت;
    _PyTok_Loc آغاز_loc;
    _PyTok_Loc انتہا_loc;
    PyObject *metadata;
};

قسمتعریف ساخت {
    ثابت حرف *متن;
    Py_ssize_t لمبائی_2;
    Py_Tok_وسعت سطر_وسعت;
    _PyTok_Off انتہا_سطر_آغاز;
    صحیح implicit_newline;
    صحیح at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
قسمتعریف ساخت {
    _PyTok_Loc location;
    Py_Tok_وسعت متن_وسعت;
} _PyTokenizer_Diagnostic;

قسمتعریف ساخت {
    صحیح حالت_2;
    غیرعلامتی حرف implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    Py_Tok_وسعت داخلہ_وسعت;
    Py_Tok_وسعت سطر_وسعت;
    صحیح level;
    حرف delimiter;
    _PyTok_Loc delimiter_loc;
    صحیح اندر_formatted_string;
    صحیح is_interactive;
    صحیح is_file;
    PyObject *filename;
    PyObject *module;
    ثابت حرف *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
خالی _PyTokenizer_Get(ساخت tok_state *, ساخت token *);
خالی _PyTokenizer_Free(ساخت tok_state *);
صحیح _PyTokenizer_Traverse(ساخت tok_state *, visitproc, خالی *);
خالی _PyTokenizer_raise_init_error(PyObject *filename);
خالی _PyToken_Init(ساخت token *);
ساکن اندرونی خالی
_PyToken_Free(ساخت token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ثابت ساخت tok_state *);
صحیح Py_Tokenizer_Get_حالت(ثابت ساخت tok_state *);
صحیح _PyTokenizer_GetNewlineRequirements(ثابت ساخت tok_state *);
/* An absent token span has a nonnull empty text view. */
ثابت حرف *Py_Token_متن_View(
    ثابت ساخت tok_state *, ثابت ساخت token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
خالی _PyToken_GetView(
    ثابت ساخت tok_state *tok, ثابت ساخت token *token,
    _PyToken_View *view);
ثابت حرف *Py_Tokenizer_وسعت_View(
    ثابت ساخت tok_state *, Py_Tok_وسعت, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ثابت حرف *Py_Tokenizer_سطر_View(
    ثابت ساخت tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ثابت حرف *Py_Tokenizer_Retained_ماخذ(ثابت ساخت tok_state *);
خالی Py_Tokenizer_سیٹ_Context(
    ساخت tok_state *tok, PyObject *filename, PyObject *module);
خالی Py_Tokenizer_سیٹ_Options(
    ساخت tok_state *tok, صحیح extra_علامات, صحیح type_comments);
خالی Py_Tokenizer_Allow_Incomplete_داخلہ(ساخت tok_state *, صحیح);
خالی _PyTokenizer_ImplyDedents(ساخت tok_state *);
صحیح _PyTokenizer_HasTrailingStatement(ثابت ساخت tok_state *);
صحیح _PyTokenizer_IsInteractive(ثابت ساخت tok_state *);
خالی _PyTokenizer_StopInteractive(ساخت tok_state *);

ساخت tok_state *_PyTokenizer_FromString(
    ثابت حرف *داخلہ, صحیح exec_داخلہ, صحیح preserve_crlf);
ساخت tok_state *_PyTokenizer_FromUTF8(
    ثابت حرف *داخلہ, صحیح exec_داخلہ, صحیح preserve_crlf);
ساخت tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ثابت حرف *encoding);
ساخت tok_state *_PyTokenizer_FromFile(
    FILE *fp, ثابت حرف *encoding, ثابت حرف *ps1, ثابت حرف *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
حرف *Py_Tokenizer_ڈھونڈو_Encoding_Filename(صحیح, PyObject *);

#اگر_ختم /* !Py_TOKENIZER_H */
