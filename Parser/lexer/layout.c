#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#تعریف_کن TABSIZE 8
#تعریف_کن ALTTABSIZE 1

صحيح
Py_Lexer_Continue_سطر(بنية tok_state *tok)
{
    صحيح c = tok_nextc(tok);
    إذا (c == '\r') {
        c = tok_nextc(tok);
    }
    إذا (c != '\n') {
        tok->انجامشده = E_LINECONT;
        أرجع -1;
    }
    c = tok_nextc(tok);
    إذا (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    إذا (c == EOF) {
        tok->انجامشده = E_EOF;
        tok->cur = tok->inp;
        أرجع -1;
    } وإلا {
        tok_backup(tok, c);
    }
    أرجع c;
}


ساكن صحيح
update_indentation(بنية tok_state *tok, صحيح ستون, صحيح altcol)
{
    lexer_layout_state *layout = &tok->layout;
    إذا (ستون == layout->پشته[layout->عمق].ستونی) {
        إذا (altcol != layout->پشته[layout->عمق].alternate_ستونی) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    وإلا إذا (ستون > layout->پشته[layout->عمق].ستونی) {
        إذا (layout->عمق + 1 >= MAXINDENT) {
            tok->انجامشده = E_TOODEEP;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol <= layout->پشته[layout->عمق].alternate_ستونی) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
        layout->pending++;
        layout->پشته[++layout->عمق] = (indentation_level){ستون, altcol};
    }
    وإلا {
        طالما (layout->عمق > 0 &&
            ستون < layout->پشته[layout->عمق].ستونی) {
            layout->pending--;
            layout->عمق--;
        }
        إذا (ستون != layout->پشته[layout->عمق].ستونی) {
            tok->انجامشده = E_DEDENT;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol != layout->پشته[layout->عمق].alternate_ستونی) {
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
    صحيح ستون = 0;
    صحيح altcol = 0;
    tok->layout.at_bol = 0;
    صحيح cont_سطر_ستون = 0;
    لكل (;;) {
        c = tok_nextc(tok);
        إذا (c == ' ') {
            ستون++, altcol++;
        }
        وإلا إذا (c == '\t') {
            ستون = (ستون / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        وإلا إذا (c == '\014')  {/* Control-L (formfeed) */
            ستون = altcol = 0; /* For Emacs users */
        }
        وإلا إذا (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_سطر_ستون = cont_سطر_ستون ? cont_سطر_ستون : ستون;
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
        tok->level == 0 && (cont_سطر_ستون || ستون)) {
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
        إذا (ستون == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        وإلا إذا (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            ستون = altcol = 0;
        }
        وإلا {
            blankline = 1; /* Ignore completely */
        }
    }
    إذا (!blankline && tok->level == 0) {
        ستون = cont_سطر_ستون ? cont_سطر_ستون : ستون;
        altcol = cont_سطر_ستون ? cont_سطر_ستون : altcol;
        إذا (update_indentation(tok, ستون, altcol) < 0) {
            أرجع -1;
        }
    }
    أرجع blankline;
}

صحيح
_PyLexer_IndentationToken(بنية tok_state *tok, بنية token *token)
{
    أكد(tok->layout.pending != 0);
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_انتها = -1;
    إذا (tok->layout.pending < 0) {
        إذا (tok->tok_extra_نشانهها) {
            p_شروع = tok->cur;
            p_انتها = tok->cur;
        }
        tok->layout.pending++;
        أرجع Py_Lexer_token_راهاندازی(tok, token, DEDENT, p_شروع, p_انتها);
    }
    وإلا {
        إذا (tok->tok_extra_نشانهها) {
            p_شروع = tok->buf_جابه_جایی;
            p_انتها = tok->cur;
        }
        tok->layout.pending--;
        أرجع Py_Lexer_token_راهاندازی(tok, token, INDENT, p_شروع, p_انتها);
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
        إذا (!tok->tok_extra_نشانهها) {
            أرجع 0;
        }
    }
    وإلا إذا (!tok->layout.comment_newline || !tok->tok_extra_نشانهها) {
        أرجع Py_Lexer_token_راهاندازی(tok, token, NEWLINE,
                                    tok->شروع, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    أرجع Py_Lexer_token_راهاندازی(tok, token, NL, tok->شروع, tok->cur);
}

فراغ
_PyLexer_ImplyDedents(بنية tok_state *tok)
{
    إذا (tok->layout.عمق != 0) {
        tok->layout.pending = -tok->layout.عمق;
        tok->layout.عمق = 0;
    }
}
