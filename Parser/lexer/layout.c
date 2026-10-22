#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#تعریف_کړه TABSIZE 8
#تعریف_کړه ALTTABSIZE 1

صحيح
Py_Lexer_Continue_کرښه(بنية tok_state *tok)
{
    صحيح c = tok_nextc(tok);
    إذا (c == '\r') {
        c = tok_nextc(tok);
    }
    إذا (c != '\n') {
        tok->بشپړ = E_LINECONT;
        أرجع -1;
    }
    c = tok_nextc(tok);
    إذا (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    إذا (c == EOF) {
        tok->بشپړ = E_EOF;
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
    إذا (ستون == layout->ډېری[layout->ژوروالی].ستنه) {
        إذا (altcol != layout->ډېری[layout->ژوروالی].alternate_ستنه) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    وإلا إذا (ستون > layout->ډېری[layout->ژوروالی].ستنه) {
        إذا (layout->ژوروالی + 1 >= MAXINDENT) {
            tok->بشپړ = E_TOODEEP;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol <= layout->ډېری[layout->ژوروالی].alternate_ستنه) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
        layout->pending++;
        layout->ډېری[++layout->ژوروالی] = (indentation_level){ستون, altcol};
    }
    وإلا {
        طالما (layout->ژوروالی > 0 &&
            ستون < layout->ډېری[layout->ژوروالی].ستنه) {
            layout->pending--;
            layout->ژوروالی--;
        }
        إذا (ستون != layout->ډېری[layout->ژوروالی].ستنه) {
            tok->بشپړ = E_DEDENT;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol != layout->ډېری[layout->ژوروالی].alternate_ستنه) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    أرجع 0;
}

صحيح
Py_Lexer_Begin_کرښه(بنية tok_state *tok)
{
    أكد(tok->layout.at_bol);
    صحيح c;
    صحيح blankline = 0;
    صحيح ستون = 0;
    صحيح altcol = 0;
    tok->layout.at_bol = 0;
    صحيح cont_کرښه_ستون = 0;
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
            cont_کرښه_ستون = cont_کرښه_ستون ? cont_کرښه_ستون : ستون;
            إذا ((c = Py_Lexer_Continue_کرښه(tok)) == -1) {
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
        tok->level == 0 && (cont_کرښه_ستون || ستون)) {
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
        ستون = cont_کرښه_ستون ? cont_کرښه_ستون : ستون;
        altcol = cont_کرښه_ستون ? cont_کرښه_ستون : altcol;
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
    _PyTok_Off p_پیل = -1;
    _PyTok_Off p_پای_ټکی = -1;
    إذا (tok->layout.pending < 0) {
        إذا (tok->tok_extra_نښې) {
            p_پیل = tok->cur;
            p_پای_ټکی = tok->cur;
        }
        tok->layout.pending++;
        أرجع Py_Lexer_token_تیاری(tok, token, DEDENT, p_پیل, p_پای_ټکی);
    }
    وإلا {
        إذا (tok->tok_extra_نښې) {
            p_پیل = tok->buf_دننه_واټن;
            p_پای_ټکی = tok->cur;
        }
        tok->layout.pending--;
        أرجع Py_Lexer_token_تیاری(tok, token, INDENT, p_پیل, p_پای_ټکی);
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
        إذا (!tok->tok_extra_نښې) {
            أرجع 0;
        }
    }
    وإلا إذا (!tok->layout.comment_newline || !tok->tok_extra_نښې) {
        أرجع Py_Lexer_token_تیاری(tok, token, NEWLINE,
                                    tok->پیل, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    أرجع Py_Lexer_token_تیاری(tok, token, NL, tok->پیل, tok->cur);
}

فراغ
_PyLexer_ImplyDedents(بنية tok_state *tok)
{
    إذا (tok->layout.ژوروالی != 0) {
        tok->layout.pending = -tok->layout.ژوروالی;
        tok->layout.ژوروالی = 0;
    }
}
