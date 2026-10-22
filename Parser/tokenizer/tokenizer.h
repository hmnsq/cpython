#ifndef Py_TOKENIZER_H
#define Py_TOKENIZER_H

#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "types.h"

ഘടന tok_state;

തരനിർവചനം എണ്ണൽ {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
എണ്ണൽ {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
ഘടന token {
    പൂർണ്ണസംഖ്യ type;
    പൂർണ്ണസംഖ്യ level;
    പൂർണ്ണസംഖ്യ is_raw;
    _PyTok_Span span;
    _PyTok_Loc start_loc;
    _PyTok_Loc end_loc;
    PyObject *metadata;
};

തരനിർവചനം ഘടന {
    സ്ഥിരം അക്ഷരം *text;
    Py_ssize_t length;
    _PyTok_Span വരി_span;
    _PyTok_Off end_വരി_start;
    പൂർണ്ണസംഖ്യ implicit_newline;
    പൂർണ്ണസംഖ്യ at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
തരനിർവചനം ഘടന {
    _PyTok_Loc location;
    _PyTok_Span text_span;
} _PyTokenizer_Diagnostic;

തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ status;
    unsigned അക്ഷരം implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    _PyTok_Span നിവേശം_span;
    _PyTok_Span വരി_span;
    പൂർണ്ണസംഖ്യ level;
    അക്ഷരം delimiter;
    _PyTok_Loc delimiter_loc;
    പൂർണ്ണസംഖ്യ in_formatted_string;
    പൂർണ്ണസംഖ്യ is_interactive;
    പൂർണ്ണസംഖ്യ is_file;
    PyObject *filename;
    PyObject *module;
    സ്ഥിരം അക്ഷരം *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
ശൂന്യം _PyTokenizer_Get(ഘടന tok_state *, ഘടന token *);
ശൂന്യം _PyTokenizer_Free(ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTokenizer_Traverse(ഘടന tok_state *, visitproc, ശൂന്യം *);
ശൂന്യം _PyTokenizer_raise_init_error(PyObject *filename);
ശൂന്യം _PyToken_Init(ഘടന token *);
സ്ഥിതം വരിയിൽ ശൂന്യം
_PyToken_Free(ഘടന token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(സ്ഥിരം ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTokenizer_GetStatus(സ്ഥിരം ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTokenizer_GetNewlineRequirements(സ്ഥിരം ഘടന tok_state *);
/* An absent token span has a nonnull empty text view. */
സ്ഥിരം അക്ഷരം *_PyToken_TextView(
    സ്ഥിരം ഘടന tok_state *, സ്ഥിരം ഘടന token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
ശൂന്യം _PyToken_GetView(
    സ്ഥിരം ഘടന tok_state *tok, സ്ഥിരം ഘടന token *token,
    _PyToken_View *view);
സ്ഥിരം അക്ഷരം *_PyTokenizer_SpanView(
    സ്ഥിരം ഘടന tok_state *, _PyTok_Span, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
സ്ഥിരം അക്ഷരം *Py_Tokenizer_വരി_View(
    സ്ഥിരം ഘടന tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
സ്ഥിരം അക്ഷരം *_PyTokenizer_RetainedSource(സ്ഥിരം ഘടന tok_state *);
ശൂന്യം _PyTokenizer_SetContext(
    ഘടന tok_state *tok, PyObject *filename, PyObject *module);
ശൂന്യം _PyTokenizer_SetOptions(
    ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ extra_tokens, പൂർണ്ണസംഖ്യ type_comments);
ശൂന്യം Py_Tokenizer_Allow_Incomplete_നിവേശം(ഘടന tok_state *, പൂർണ്ണസംഖ്യ);
ശൂന്യം _PyTokenizer_ImplyDedents(ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTokenizer_HasTrailingStatement(സ്ഥിരം ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTokenizer_IsInteractive(സ്ഥിരം ഘടന tok_state *);
ശൂന്യം _PyTokenizer_StopInteractive(ഘടന tok_state *);

ഘടന tok_state *_PyTokenizer_FromString(
    സ്ഥിരം അക്ഷരം *നിവേശം, പൂർണ്ണസംഖ്യ exec_നിവേശം, പൂർണ്ണസംഖ്യ preserve_crlf);
ഘടന tok_state *_PyTokenizer_FromUTF8(
    സ്ഥിരം അക്ഷരം *നിവേശം, പൂർണ്ണസംഖ്യ exec_നിവേശം, പൂർണ്ണസംഖ്യ preserve_crlf);
ഘടന tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, സ്ഥിരം അക്ഷരം *encoding);
ഘടന tok_state *_PyTokenizer_FromFile(
    FILE *fp, സ്ഥിരം അക്ഷരം *encoding, സ്ഥിരം അക്ഷരം *ps1, സ്ഥിരം അക്ഷരം *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
അക്ഷരം *_PyTokenizer_FindEncodingFilename(പൂർണ്ണസംഖ്യ, PyObject *);

#endif /* !Py_TOKENIZER_H */
