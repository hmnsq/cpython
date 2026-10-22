#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

صحيح
Py_Lexer_Continue_سطر(بنية tok_state *tok)
{
    صحيح c = tok_nextc(tok);
    إذا (c == '\r') {
        c = tok_nextc(tok);
    }
    إذا (c != '\n') {
        tok->done = E_LINECONT;
        أرجع -1;
    }
    c = tok_nextc(tok);
    إذا (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    إذا (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        أرجع -1;
    } وإلا {
        tok_backup(tok, c);
    }
    أرجع c;
}


ساكن صحيح
update_indentation(بنية tok_state *tok, صحيح col, صحيح altcol)
{
    lexer_layout_state *layout = &tok->layout;
    إذا (col == layout->ڈھیر[layout->depth].column) {
        إذا (altcol != layout->ڈھیر[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    وإلا إذا (col > layout->ڈھیر[layout->depth].column) {
        إذا (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol <= layout->ڈھیر[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
        layout->pending++;
        layout->ڈھیر[++layout->depth] = (indentation_level){col, altcol};
    }
    وإلا {
        طالما (layout->depth > 0 &&
            col < layout->ڈھیر[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        إذا (col != layout->ڈھیر[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol != layout->ڈھیر[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    أرجع 0;
}

صحيح
Py_Lexer_Begin_سطر(بنية tok_state *tok)
{
    أكد(tok->layout.at_bol);
    صحيح c;
    صحيح blankline = 0;
    صحيح col = 0;
    صحيح altcol = 0;
    tok->layout.at_bol = 0;
    صحيح cont_سطر_col = 0;
    لكل (;;) {
        c = tok_nextc(tok);
        إذا (c == ' ') {
            col++, altcol++;
        }
        وإلا إذا (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        وإلا إذا (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        وإلا إذا (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_سطر_col = cont_سطر_col ? cont_سطر_col : col;
            إذا ((c = Py_Lexer_Continue_سطر(tok)) == -1) {
                أرجع -1;
            }
        }
        وإلا إذا (c == EOF && PyErr_Occurred()) {
            أرجع -1;
        }
        وإلا {
            اكسر;
        }
    }
    tok_backup(tok, c);
    إذا (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_سطر_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    إذا (c == '#' || c == '\n' || c == '\r') {
        صحيح interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        إذا (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        وإلا إذا (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        وإلا {
            blankline = 1; /* Ignore completely */
        }
    }
    إذا (!blankline && tok->level == 0) {
        col = cont_سطر_col ? cont_سطر_col : col;
        altcol = cont_سطر_col ? cont_سطر_col : altcol;
        إذا (update_indentation(tok, col, altcol) < 0) {
            أرجع -1;
        }
    }
    أرجع blankline;
}

صحيح
_PyLexer_IndentationToken(بنية tok_state *tok, بنية token *token)
{
    أكد(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    إذا (tok->layout.pending < 0) {
        إذا (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        أرجع _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    وإلا {
        إذا (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        أرجع _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

صحيح
_PyLexer_Newline(بنية tok_state *tok, بنية token *token, صحيح blankline)
{
    إذا (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    إذا (blankline || tok->level > 0) {
        إذا (!tok->tok_extra_tokens) {
            أرجع 0;
        }
    }
    وإلا إذا (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        أرجع _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    أرجع _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

فراغ
_PyLexer_ImplyDedents(بنية tok_state *tok)
{
    إذا (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
