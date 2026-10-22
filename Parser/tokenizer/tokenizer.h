#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "types.h"

கட்டமைப்பு tok_state;

வகைவரை எண்ணிக்கை {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
எண்ணிக்கை {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
கட்டமைப்பு token {
    முழு type;
    முழு level;
    முழு is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

வகைவரை கட்டமைப்பு {
    மாறிலி எழுத்து *text;
    Py_ssize_t length;
    _PyTok_Span வரி_span;
    _PyTok_Off end_வரி_start;
    முழு implicit_newline;
    முழு at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
வகைவரை கட்டமைப்பு {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

வகைவரை கட்டமைப்பு {
    முழு status;
    unsigned எழுத்து implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span உள்ளீடு_span;
    _PyTok_Span வரி_span;
    முழு level;
    எழுத்து delimiter;
    _PyTok_Loc delimiter_loc;
    முழு in_formatted_string;
    முழு is_interactive;
    முழு is_file;
    PyObject *filename;
    PyObject *module;
    மாறிலி எழுத்து *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
வெறுமை _PyTokenizer_Get(கட்டமைப்பு tok_state *, கட்டமைப்பு token *);
வெறுமை _PyTokenizer_Free(கட்டமைப்பு tok_state *);
முழு _PyTokenizer_Traverse(கட்டமைப்பு tok_state *, visitproc, வெறுமை *);
வெறுமை _PyTokenizer_raise_init_error(PyObject *filename);
வெறுமை _PyToken_Init(கட்டமைப்பு token *);
நிலையான உள்வரி வெறுமை
_PyToken_Free(கட்டமைப்பு token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(மாறிலி கட்டமைப்பு tok_state *);
முழு _PyTokenizer_GetStatus(மாறிலி கட்டமைப்பு tok_state *);
முழு _PyTokenizer_GetNewlineRequirements(மாறிலி கட்டமைப்பு tok_state *);
/* An absent token span has a nonnull empty text view. */
மாறிலி எழுத்து *_PyToken_TextView(
    மாறிலி கட்டமைப்பு tok_state *, மாறிலி கட்டமைப்பு token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
வெறுமை _PyToken_GetView(
    மாறிலி கட்டமைப்பு tok_state *tok, மாறிலி கட்டமைப்பு token *token,
    _PyToken_View *view);
மாறிலி எழுத்து *_PyTokenizer_SpanView(
    மாறிலி கட்டமைப்பு tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
மாறிலி எழுத்து *Py_Tokenizer_வரி_View(
    மாறிலி கட்டமைப்பு tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
மாறிலி எழுத்து *_PyTokenizer_RetainedSource(மாறிலி கட்டமைப்பு tok_state *);
வெறுமை _PyTokenizer_SetContext(
    கட்டமைப்பு tok_state *tok, PyObject *filename, PyObject *module);
வெறுமை _PyTokenizer_SetOptions(
    கட்டமைப்பு tok_state *tok, முழு extra_tokens, முழு type_comments);
வெறுமை Py_Tokenizer_Allow_Incomplete_உள்ளீடு(கட்டமைப்பு tok_state *, முழு);
வெறுமை _PyTokenizer_ImplyDedents(கட்டமைப்பு tok_state *);
முழு _PyTokenizer_HasTrailingStatement(மாறிலி கட்டமைப்பு tok_state *);
முழு _PyTokenizer_IsInteractive(மாறிலி கட்டமைப்பு tok_state *);
வெறுமை _PyTokenizer_StopInteractive(கட்டமைப்பு tok_state *);

கட்டமைப்பு tok_state *_PyTokenizer_FromString(
    மாறிலி எழுத்து *உள்ளீடு, முழு exec_உள்ளீடு, முழு preserve_crlf);
கட்டமைப்பு tok_state *_PyTokenizer_FromUTF8(
    மாறிலி எழுத்து *உள்ளீடு, முழு exec_உள்ளீடு, முழு preserve_crlf);
கட்டமைப்பு tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, மாறிலி எழுத்து *encoding);
கட்டமைப்பு tok_state *_PyTokenizer_FromFile(
    FILE *fp, மாறிலி எழுத்து *encoding, மாறிலி எழுத்து *ps1, மாறிலி எழுத்து *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
எழுத்து *_PyTokenizer_FindEncodingFilename(முழு, PyObject *);

#endif /* !Py_TOKENIZER_H */
