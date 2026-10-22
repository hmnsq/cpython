#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#अन्तर्भाव "Python.h"
#अन्तर्भाव "types.h"

संरचना tok_state;

प्रकारव्याख्या गणना {
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
    पूर्णांक type;
    पूर्णांक level;
    पूर्णांक is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

प्रकारव्याख्या संरचना {
    स्थिरांक अक्षर *text;
    Py_ssize_t length;
    _PyTok_Span ओळ_span;
    _PyTok_Off end_ओळ_start;
    पूर्णांक implicit_newline;
    पूर्णांक at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
प्रकारव्याख्या संरचना {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

प्रकारव्याख्या संरचना {
    पूर्णांक status;
    अचिन्हित अक्षर implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span आदान_span;
    _PyTok_Span ओळ_span;
    पूर्णांक level;
    अक्षर delimiter;
    _PyTok_Loc delimiter_loc;
    पूर्णांक in_formatted_string;
    पूर्णांक is_interactive;
    पूर्णांक is_file;
    PyObject *filename;
    PyObject *module;
    स्थिरांक अक्षर *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
शून्य _PyTokenizer_Get(संरचना tok_state *, संरचना token *);
शून्य _PyTokenizer_Free(संरचना tok_state *);
पूर्णांक _PyTokenizer_Traverse(संरचना tok_state *, visitproc, शून्य *);
शून्य _PyTokenizer_raise_init_error(PyObject *filename);
शून्य _PyToken_Init(संरचना token *);
स्थिर ओळीत शून्य
_PyToken_Free(संरचना token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(स्थिरांक संरचना tok_state *);
पूर्णांक _PyTokenizer_GetStatus(स्थिरांक संरचना tok_state *);
पूर्णांक _PyTokenizer_GetNewlineRequirements(स्थिरांक संरचना tok_state *);
/* An absent token span has a nonnull empty text view. */
स्थिरांक अक्षर *_PyToken_TextView(
    स्थिरांक संरचना tok_state *, स्थिरांक संरचना token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
शून्य _PyToken_GetView(
    स्थिरांक संरचना tok_state *tok, स्थिरांक संरचना token *token,
    _PyToken_View *view);
स्थिरांक अक्षर *_PyTokenizer_SpanView(
    स्थिरांक संरचना tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
स्थिरांक अक्षर *Py_Tokenizer_ओळ_View(
    स्थिरांक संरचना tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
स्थिरांक अक्षर *_PyTokenizer_RetainedSource(स्थिरांक संरचना tok_state *);
शून्य _PyTokenizer_SetContext(
    संरचना tok_state *tok, PyObject *filename, PyObject *module);
शून्य _PyTokenizer_SetOptions(
    संरचना tok_state *tok, पूर्णांक extra_tokens, पूर्णांक type_comments);
शून्य Py_Tokenizer_Allow_Incomplete_आदान(संरचना tok_state *, पूर्णांक);
शून्य _PyTokenizer_ImplyDedents(संरचना tok_state *);
पूर्णांक _PyTokenizer_HasTrailingStatement(स्थिरांक संरचना tok_state *);
पूर्णांक _PyTokenizer_IsInteractive(स्थिरांक संरचना tok_state *);
शून्य _PyTokenizer_StopInteractive(संरचना tok_state *);

संरचना tok_state *_PyTokenizer_FromString(
    स्थिरांक अक्षर *आदान, पूर्णांक exec_आदान, पूर्णांक preserve_crlf);
संरचना tok_state *_PyTokenizer_FromUTF8(
    स्थिरांक अक्षर *आदान, पूर्णांक exec_आदान, पूर्णांक preserve_crlf);
संरचना tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, स्थिरांक अक्षर *encoding);
संरचना tok_state *_PyTokenizer_FromFile(
    खाता *fp, स्थिरांक अक्षर *encoding, स्थिरांक अक्षर *ps1, स्थिरांक अक्षर *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
अक्षर *_PyTokenizer_FindEncodingFilename(पूर्णांक, PyObject *);

#endif /* !Py_TOKENIZER_H */
