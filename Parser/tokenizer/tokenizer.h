#నిర్వచితం_కాకపోతే Py_TOKENIZER_H
#నిర్వచించు Py_TOKENIZER_H

#అన్తర్భావ "Python.h"
#అన్తర్భావ "types.h"

నిర్మాణం tok_state;

రకనిర్వచనం గణన {
    _PyTokenizer_KEEP_NEWLINE,   // Preserve the original line terminator.
    _PyTokenizer_IMPLY_NEWLINE,  // Add a missing line terminator.
    _PyTokenizer_APPEND_NEWLINE, // Also append a blank line to terminated input.
} _PyTokenizer_Newline;

/* Reasons that the appended newline can change the token stream. */
గణన {
    _PyTokenizer_NEWLINE_STATEMENT = 1,
    _PyTokenizer_NEWLINE_INDENT = 2,
    _PyTokenizer_NEWLINE_CONTINUATION = 4,
};

/* Initialize before use. metadata owns a reference released by _PyToken_Free;
   a consumer taking that reference must set metadata to NULL. */
నిర్మాణం token {
    పూర్ణాంకం type;
    పూర్ణాంకం level;
    పూర్ణాంకం is_raw;
    Py_Tok_పరిధి పరిధి;
    _PyTok_Loc ప్రారంభం_loc;
    _PyTok_Loc చివర_loc;
    PyObject *metadata;
};

రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి అక్షరం *పాఠ్యం;
    Py_ssize_t నిడివి;
    Py_Tok_పరిధి పంక్తి_పరిధి;
    _PyTok_Off చివర_పంక్తి_ప్రారంభం;
    పూర్ణాంకం implicit_newline;
    పూర్ణాంకం at_eof;
} _PyToken_View;

/* Supplemental source context for a terminal error. location is the reporting
   cursor, independent of the scanner cursor; lineno == 0 means absent.
   The text span may cover multiple physical lines. */
రకనిర్వచనం నిర్మాణం {
    _PyTok_Loc location;
    Py_Tok_పరిధి పాఠ్యం_పరిధి;
} _PyTokenizer_Diagnostic;

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం స్థితి_2;
    గుర్తులేని అక్షరం implicit_newline;  // _PyTokenizer_Newline
    _PyTokenizer_Diagnostic diagnostic;
    _PyTok_Loc location;
    _PyTok_Off cursor;
    Py_Tok_పరిధి ఆదానం_పరిధి;
    Py_Tok_పరిధి పంక్తి_పరిధి;
    పూర్ణాంకం level;
    అక్షరం delimiter;
    _PyTok_Loc delimiter_loc;
    పూర్ణాంకం లోపల_formatted_string;
    పూర్ణాంకం is_interactive;
    పూర్ణాంకం is_file;
    PyObject *filename;
    PyObject *module;
    స్థిరరాశి అక్షరం *encoding;
} _PyTokenizer_Info;

/* Get replaces the initialized token, releasing its previous metadata.
   Errors are returned as ERRORTOKEN, with or without a Python exception.
   In incomplete-input mode, an unfinished string retains its token type
   with an absent span and E_INCOMPLETE status. No Python exception is set. */
శూన్య _PyTokenizer_Get(నిర్మాణం tok_state *, నిర్మాణం token *);
శూన్య _PyTokenizer_Free(నిర్మాణం tok_state *);
పూర్ణాంకం _PyTokenizer_Traverse(నిర్మాణం tok_state *, visitproc, శూన్య *);
శూన్య _PyTokenizer_raise_init_error(PyObject *filename);
శూన్య _PyToken_Init(నిర్మాణం token *);
స్థిర అంతఃవరుస శూన్య
_PyToken_Free(నిర్మాణం token *token)
{
    Py_CLEAR(token->metadata);
}

/* Views and borrowed snapshot references remain valid until the tokenizer is
   mutated or freed. Source spans may be discarded when reading more input. */
_PyTokenizer_Info _PyTokenizer_GetInfo(స్థిరరాశి నిర్మాణం tok_state *);
పూర్ణాంకం Py_Tokenizer_Get_స్థితి(స్థిరరాశి నిర్మాణం tok_state *);
పూర్ణాంకం _PyTokenizer_GetNewlineRequirements(స్థిరరాశి నిర్మాణం tok_state *);
/* An absent token span has a nonnull empty text view. */
స్థిరరాశి అక్షరం *Py_Token_పాఠ్యం_View(
    స్థిరరాశి నిర్మాణం tok_state *, స్థిరరాశి నిర్మాణం token *, Py_ssize_t *);
/* Use the token from the most recent Get. An absent span has nonnull empty text.
   line_span covers the token's complete physical line range. end_line_start
   is the logical offset of its final physical line. */
శూన్య _PyToken_GetView(
    స్థిరరాశి నిర్మాణం tok_state *tok, స్థిరరాశి నిర్మాణం token *token,
    _PyToken_View *view);
స్థిరరాశి అక్షరం *Py_Tokenizer_పరిధి_View(
    స్థిరరాశి నిర్మాణం tok_state *, Py_Tok_పరిధి, Py_ssize_t *);
/* For retained input: 1-based lines clamp to the first or final line.
   The view excludes the newline and need not be NUL-terminated. */
స్థిరరాశి అక్షరం *Py_Tokenizer_పంక్తి_View(
    స్థిరరాశి నిర్మాణం tok_state *, Py_ssize_t, Py_ssize_t *);
/* Return NUL-terminated retained input, or NULL without setting an exception
   for streaming input or interactive input before its first line. */
స్థిరరాశి అక్షరం *Py_Tokenizer_Retained_ఆధారం(స్థిరరాశి నిర్మాణం tok_state *);
శూన్య Py_Tokenizer_సమితి_Context(
    నిర్మాణం tok_state *tok, PyObject *filename, PyObject *module);
శూన్య Py_Tokenizer_సమితి_Options(
    నిర్మాణం tok_state *tok, పూర్ణాంకం extra_టోకెన్లు, పూర్ణాంకం type_comments);
శూన్య Py_Tokenizer_Allow_Incomplete_ఆదానం(నిర్మాణం tok_state *, పూర్ణాంకం);
శూన్య _PyTokenizer_ImplyDedents(నిర్మాణం tok_state *);
పూర్ణాంకం _PyTokenizer_HasTrailingStatement(స్థిరరాశి నిర్మాణం tok_state *);
పూర్ణాంకం _PyTokenizer_IsInteractive(స్థిరరాశి నిర్మాణం tok_state *);
శూన్య _PyTokenizer_StopInteractive(నిర్మాణం tok_state *);

నిర్మాణం tok_state *_PyTokenizer_FromString(
    స్థిరరాశి అక్షరం *ఆదానం, పూర్ణాంకం exec_ఆదానం, పూర్ణాంకం preserve_crlf);
నిర్మాణం tok_state *_PyTokenizer_FromUTF8(
    స్థిరరాశి అక్షరం *ఆదానం, పూర్ణాంకం exec_ఆదానం, పూర్ణాంకం preserve_crlf);
నిర్మాణం tok_state *_PyTokenizer_FromReadline(
    PyObject *readline, స్థిరరాశి అక్షరం *encoding);
నిర్మాణం tok_state *_PyTokenizer_FromFile(
    FILE *fp, స్థిరరాశి అక్షరం *encoding, స్థిరరాశి అక్షరం *ps1, స్థిరరాశి అక్షరం *ps2);
/* Return the declared encoding in PyMem-allocated storage, or NULL.
   An exception is set on error. */
అక్షరం *Py_Tokenizer_కనుగొను_Encoding_Filename(పూర్ణాంకం, PyObject *);

#అయితే_ముగింపు /* !Py_TOKENIZER_H */
