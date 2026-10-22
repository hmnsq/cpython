#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "types.h"

কাঠামো tok_state;

প্রকারলেখ গণনা {
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
কাঠামো token {
    পূর্ণসংখ্যা type;
    পূর্ণসংখ্যা level;
    পূর্ণসংখ্যা is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

প্রকারলেখ কাঠামো {
    ধ্রুবক অক্ষর *text;
    Py_ssize_t দৈর্ঘ্য;
    _PyTok_Span সারি_span;
    _PyTok_Off end_সারি_start;
    পূর্ণসংখ্যা implicit_newline;
    পূর্ণসংখ্যা at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
প্রকারলেখ কাঠামো {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা status;
    unsigned অক্ষর implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span প্রবেশ_span;
    _PyTok_Span সারি_span;
    পূর্ণসংখ্যা level;
    অক্ষর delimiter;
    _PyTok_Loc delimiter_loc;
    পূর্ণসংখ্যা in_formatted_string;
    পূর্ণসংখ্যা is_interactive;
    পূর্ণসংখ্যা is_file;
    PyObject *filename;
    PyObject *module;
    ধ্রুবক অক্ষর *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
শূন্য _PyTokenizer_Get(কাঠামো tok_state *, কাঠামো token *);
শূন্য _PyTokenizer_Free(কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTokenizer_Traverse(কাঠামো tok_state *, visitproc, শূন্য *);
শূন্য _PyTokenizer_raise_init_error(PyObject *filename);
শূন্য _PyToken_Init(কাঠামো token *);
স্থির অন্তঃরেখ শূন্য
_PyToken_Free(কাঠামো token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ধ্রুবক কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTokenizer_GetStatus(ধ্রুবক কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTokenizer_GetNewlineRequirements(ধ্রুবক কাঠামো tok_state *);
/* An absent token span has a nonnull empty text view. */
ধ্রুবক অক্ষর *_PyToken_TextView(
    ধ্রুবক কাঠামো tok_state *, ধ্রুবক কাঠামো token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
শূন্য _PyToken_GetView(
    ধ্রুবক কাঠামো tok_state *tok, ধ্রুবক কাঠামো token *token,
    _PyToken_View *view);
ধ্রুবক অক্ষর *_PyTokenizer_SpanView(
    ধ্রুবক কাঠামো tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ধ্রুবক অক্ষর *Py_Tokenizer_সারি_View(
    ধ্রুবক কাঠামো tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ধ্রুবক অক্ষর *_PyTokenizer_RetainedSource(ধ্রুবক কাঠামো tok_state *);
শূন্য _PyTokenizer_SetContext(
    কাঠামো tok_state *tok, PyObject *filename, PyObject *module);
শূন্য _PyTokenizer_SetOptions(
    কাঠামো tok_state *tok, পূর্ণসংখ্যা extra_tokens, পূর্ণসংখ্যা type_comments);
শূন্য Py_Tokenizer_Allow_Incomplete_প্রবেশ(কাঠামো tok_state *, পূর্ণসংখ্যা);
শূন্য _PyTokenizer_ImplyDedents(কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTokenizer_HasTrailingStatement(ধ্রুবক কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTokenizer_IsInteractive(ধ্রুবক কাঠামো tok_state *);
শূন্য _PyTokenizer_StopInteractive(কাঠামো tok_state *);

কাঠামো tok_state *_PyTokenizer_FromString(
    ধ্রুবক অক্ষর *প্রবেশ, পূর্ণসংখ্যা exec_প্রবেশ, পূর্ণসংখ্যা preserve_crlf);
কাঠামো tok_state *_PyTokenizer_FromUTF8(
    ধ্রুবক অক্ষর *প্রবেশ, পূর্ণসংখ্যা exec_প্রবেশ, পূর্ণসংখ্যা preserve_crlf);
কাঠামো tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ধ্রুবক অক্ষর *encoding);
কাঠামো tok_state *_PyTokenizer_FromFile(
    FILE *fp, ধ্রুবক অক্ষর *encoding, ধ্রুবক অক্ষর *ps1, ধ্রুবক অক্ষর *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
অক্ষর *_PyTokenizer_FindEncodingFilename(পূর্ণসংখ্যা, PyObject *);

#endif /* !Py_TOKENIZER_H */
