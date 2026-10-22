#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#अन्तर्भाव "Python.h"
#अन्तर्भाव "types.h"

संरचना tok_state;

प्रकारनिर्देश गणना {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
गणना {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
संरचना token {
    पूर्णाङ्क type;
    पूर्णाङ्क level;
    पूर्णाङ्क is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

प्रकारनिर्देश संरचना {
    ध्रुव वर्ण *text;
    Py_ssize_t length;
    _PyTok_Span पंक्ति_span;
    _PyTok_Off end_पंक्ति_start;
    पूर्णाङ्क implicit_newline;
    पूर्णाङ्क at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
प्रकारनिर्देश संरचना {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क status;
    अचिन्हित वर्ण implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span निवेश_span;
    _PyTok_Span पंक्ति_span;
    पूर्णाङ्क level;
    वर्ण delimiter;
    _PyTok_Loc delimiter_loc;
    पूर्णाङ्क in_formatted_string;
    पूर्णाङ्क is_interactive;
    पूर्णाङ्क is_file;
    PyObject *filename;
    PyObject *module;
    ध्रुव वर्ण *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
शून्य _PyTokenizer_Get(संरचना tok_state *, संरचना token *);
शून्य _PyTokenizer_Free(संरचना tok_state *);
पूर्णाङ्क _PyTokenizer_Traverse(संरचना tok_state *, visitproc, शून्य *);
शून्य _PyTokenizer_raise_init_error(PyObject *filename);
शून्य _PyToken_Init(संरचना token *);
स्थिर अन्तःस्थ शून्य
_PyToken_Free(संरचना token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(ध्रुव संरचना tok_state *);
पूर्णाङ्क _PyTokenizer_GetStatus(ध्रुव संरचना tok_state *);
पूर्णाङ्क _PyTokenizer_GetNewlineRequirements(ध्रुव संरचना tok_state *);
/* An absent token span has a nonnull empty text view. */
ध्रुव वर्ण *_PyToken_TextView(
    ध्रुव संरचना tok_state *, ध्रुव संरचना token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
शून्य _PyToken_GetView(
    ध्रुव संरचना tok_state *tok, ध्रुव संरचना token *token,
    _PyToken_View *view);
ध्रुव वर्ण *_PyTokenizer_SpanView(
    ध्रुव संरचना tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
ध्रुव वर्ण *Py_Tokenizer_पंक्ति_View(
    ध्रुव संरचना tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
ध्रुव वर्ण *_PyTokenizer_RetainedSource(ध्रुव संरचना tok_state *);
शून्य _PyTokenizer_SetContext(
    संरचना tok_state *tok, PyObject *filename, PyObject *module);
शून्य _PyTokenizer_SetOptions(
    संरचना tok_state *tok, पूर्णाङ्क extra_tokens, पूर्णाङ्क type_comments);
शून्य Py_Tokenizer_Allow_Incomplete_निवेश(संरचना tok_state *, पूर्णाङ्क);
शून्य _PyTokenizer_ImplyDedents(संरचना tok_state *);
पूर्णाङ्क _PyTokenizer_HasTrailingStatement(ध्रुव संरचना tok_state *);
पूर्णाङ्क _PyTokenizer_IsInteractive(ध्रुव संरचना tok_state *);
शून्य _PyTokenizer_StopInteractive(संरचना tok_state *);

संरचना tok_state *_PyTokenizer_FromString(
    ध्रुव वर्ण *निवेश, पूर्णाङ्क exec_निवेश, पूर्णाङ्क preserve_crlf);
संरचना tok_state *_PyTokenizer_FromUTF8(
    ध्रुव वर्ण *निवेश, पूर्णाङ्क exec_निवेश, पूर्णाङ्क preserve_crlf);
संरचना tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, ध्रुव वर्ण *encoding);
संरचना tok_state *_PyTokenizer_FromFile(
    खाता *fp, ध्रुव वर्ण *encoding, ध्रुव वर्ण *ps1, ध्रुव वर्ण *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
वर्ण *_PyTokenizer_FindEncodingFilename(पूर्णाङ्क, PyObject *);

#endif /* !Py_TOKENIZER_H */
