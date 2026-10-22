#شامل "Python.h"
#شامل "errcode.h"
#شامل "pycore_token.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"
#شامل "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

صحیح
Py_Lexer_Continue_سطر(ساختار tok_state *tok)
{
    صحیح c = tok_nextc(tok);
    اگر (c == '\r') {
        c = tok_nextc(tok);
    }
    اگر (c != '\n') {
        tok->done = E_LINECONT;
        بازگشت -1;
    }
    c = tok_nextc(tok);
    اگر (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    اگر (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        بازگشت -1;
    } وگرنه {
        tok_backup(tok, c);
    }
    بازگشت c;
}


ایستا صحیح
update_indentation(ساختار tok_state *tok, صحیح col, صحیح altcol)
{
    lexer_layout_state *layout = &tok->layout;
    اگر (col == layout->پشته[layout->depth].column) {
        اگر (altcol != layout->پشته[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            بازگشت -1;
        }
    }
    وگرنه اگر (col > layout->پشته[layout->depth].column) {
        اگر (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            بازگشت -1;
        }
        اگر (altcol <= layout->پشته[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            بازگشت -1;
        }
        layout->pending++;
        layout->پشته[++layout->depth] = (indentation_level){col, altcol};
    }
    وگرنه {
        وقتی (layout->depth > 0 &&
            col < layout->پشته[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        اگر (col != layout->پشته[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            بازگشت -1;
        }
        اگر (altcol != layout->پشته[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            بازگشت -1;
        }
    }
    بازگشت 0;
}

صحیح
Py_Lexer_Begin_سطر(ساختار tok_state *tok)
{
    أكد(tok->layout.at_bol);
    صحیح c;
    صحیح blankline = 0;
    صحیح col = 0;
    صحیح altcol = 0;
    tok->layout.at_bol = 0;
    صحیح cont_سطر_col = 0;
    برای (;;) {
        c = tok_nextc(tok);
        اگر (c == ' ') {
            col++, altcol++;
        }
        وگرنه اگر (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        وگرنه اگر (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        وگرنه اگر (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_سطر_col = cont_سطر_col ? cont_سطر_col : col;
            اگر ((c = Py_Lexer_Continue_سطر(tok)) == -1) {
                بازگشت -1;
            }
        }
        وگرنه اگر (c == EOF && PyErr_Occurred()) {
            بازگشت -1;
        }
        وگرنه {
            بشکن;
        }
    }
    tok_backup(tok, c);
    اگر (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_سطر_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    اگر (c == '#' || c == '\n' || c == '\r') {
        صحیح interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        اگر (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        وگرنه اگر (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        وگرنه {
            blankline = 1; /* Ignore completely */
        }
    }
    اگر (!blankline && tok->level == 0) {
        col = cont_سطر_col ? cont_سطر_col : col;
        altcol = cont_سطر_col ? cont_سطر_col : altcol;
        اگر (update_indentation(tok, col, altcol) < 0) {
            بازگشت -1;
        }
    }
    بازگشت blankline;
}

صحیح
_PyLexer_IndentationToken(ساختار tok_state *tok, ساختار token *token)
{
    أكد(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    اگر (tok->layout.pending < 0) {
        اگر (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        بازگشت _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    وگرنه {
        اگر (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        بازگشت _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

صحیح
_PyLexer_Newline(ساختار tok_state *tok, ساختار token *token, صحیح blankline)
{
    اگر (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    اگر (blankline || tok->level > 0) {
        اگر (!tok->tok_extra_tokens) {
            بازگشت 0;
        }
    }
    وگرنه اگر (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        بازگشت _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    بازگشت _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

تهی
_PyLexer_ImplyDedents(ساختار tok_state *tok)
{
    اگر (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
