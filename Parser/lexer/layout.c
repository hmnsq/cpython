#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

पूर्णाङ्क
Py_Lexer_Continue_पङ्क्तिः(संरचना tok_state *tok)
{
    पूर्णाङ्क c = tok_nextc(tok);
    सचे (c == '\r') {
        c = tok_nextc(tok);
    }
    सचे (c != '\n') {
        tok->done = E_LINECONT;
        पटिनिवत्त -1;
    }
    c = tok_nextc(tok);
    सचे (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    सचे (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        पटिनिवत्त -1;
    } अञ्ञथा {
        tok_backup(tok, c);
    }
    पटिनिवत्त c;
}


स्थिर पूर्णाङ्क
update_indentation(संरचना tok_state *tok, पूर्णाङ्क col, पूर्णाङ्क altcol)
{
    lexer_layout_state *layout = &tok->layout;
    सचे (col == layout->राशिः[layout->depth].column) {
        सचे (altcol != layout->राशिः[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            पटिनिवत्त -1;
        }
    }
    अञ्ञथा सचे (col > layout->राशिः[layout->depth].column) {
        सचे (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            पटिनिवत्त -1;
        }
        सचे (altcol <= layout->राशिः[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            पटिनिवत्त -1;
        }
        layout->pending++;
        layout->राशिः[++layout->depth] = (indentation_level){col, altcol};
    }
    अञ्ञथा {
        याव (layout->depth > 0 &&
            col < layout->राशिः[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        सचे (col != layout->राशिः[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            पटिनिवत्त -1;
        }
        सचे (altcol != layout->राशिः[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            पटिनिवत्त -1;
        }
    }
    पटिनिवत्त 0;
}

पूर्णाङ्क
Py_Lexer_Begin_पङ्क्तिः(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->layout.at_bol);
    पूर्णाङ्क c;
    पूर्णाङ्क blankline = 0;
    पूर्णाङ्क col = 0;
    पूर्णाङ्क altcol = 0;
    tok->layout.at_bol = 0;
    पूर्णाङ्क cont_पङ्क्तिः_col = 0;
    अत्थाय (;;) {
        c = tok_nextc(tok);
        सचे (c == ' ') {
            col++, altcol++;
        }
        अञ्ञथा सचे (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        अञ्ञथा सचे (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        अञ्ञथा सचे (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_पङ्क्तिः_col = cont_पङ्क्तिः_col ? cont_पङ्क्तिः_col : col;
            सचे ((c = Py_Lexer_Continue_पङ्क्तिः(tok)) == -1) {
                पटिनिवत्त -1;
            }
        }
        अञ्ञथा सचे (c == EOF && PyErr_Occurred()) {
            पटिनिवत्त -1;
        }
        अञ्ञथा {
            भङ्ग;
        }
    }
    tok_backup(tok, c);
    सचे (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_पङ्क्तिः_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    सचे (c == '#' || c == '\n' || c == '\r') {
        पूर्णाङ्क interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        सचे (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        अञ्ञथा सचे (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        अञ्ञथा {
            blankline = 1; /* Ignore completely */
        }
    }
    सचे (!blankline && tok->level == 0) {
        col = cont_पङ्क्तिः_col ? cont_पङ्क्तिः_col : col;
        altcol = cont_पङ्क्तिः_col ? cont_पङ्क्तिः_col : altcol;
        सचे (update_indentation(tok, col, altcol) < 0) {
            पटिनिवत्त -1;
        }
    }
    पटिनिवत्त blankline;
}

पूर्णाङ्क
_PyLexer_IndentationToken(संरचना tok_state *tok, संरचना token *token)
{
    प्रतिज्ञा(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    सचे (tok->layout.pending < 0) {
        सचे (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        पटिनिवत्त _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    अञ्ञथा {
        सचे (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        पटिनिवत्त _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

पूर्णाङ्क
_PyLexer_Newline(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क blankline)
{
    सचे (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    सचे (blankline || tok->level > 0) {
        सचे (!tok->tok_extra_tokens) {
            पटिनिवत्त 0;
        }
    }
    अञ्ञथा सचे (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        पटिनिवत्त _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    पटिनिवत्त _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

शून्य
_PyLexer_ImplyDedents(संरचना tok_state *tok)
{
    सचे (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
