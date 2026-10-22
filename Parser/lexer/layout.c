#شامل "Python.h"
#شامل "errcode.h"
#شامل "pycore_token.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"
#شامل "../tokenizer/reader.h"

#تعریف_کرو TABSIZE 8
#تعریف_کرو ALTTABSIZE 1

صحیح
Py_Lexer_Continue_سطر(ساخت tok_state *tok)
{
    صحیح c = tok_nextc(tok);
    اگر (c == '\r') {
        c = tok_nextc(tok);
    }
    اگر (c != '\n') {
        tok->مکمل = E_LINECONT;
        واپس -1;
    }
    c = tok_nextc(tok);
    اگر (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    اگر (c == EOF) {
        tok->مکمل = E_EOF;
        tok->cur = tok->inp;
        واپس -1;
    } ورنہ {
        tok_backup(tok, c);
    }
    واپس c;
}


ساکن صحیح
update_indentation(ساخت tok_state *tok, صحیح ستون, صحیح altcol)
{
    lexer_layout_state *layout = &tok->layout;
    اگر (ستون == layout->ڈھیر[layout->گہرائی].عمود) {
        اگر (altcol != layout->ڈھیر[layout->گہرائی].alternate_عمود) {
            _PyTokenizer_indenterror(tok);
            واپس -1;
        }
    }
    ورنہ اگر (ستون > layout->ڈھیر[layout->گہرائی].عمود) {
        اگر (layout->گہرائی + 1 >= MAXINDENT) {
            tok->مکمل = E_TOODEEP;
            tok->cur = tok->inp;
            واپس -1;
        }
        اگر (altcol <= layout->ڈھیر[layout->گہرائی].alternate_عمود) {
            _PyTokenizer_indenterror(tok);
            واپس -1;
        }
        layout->pending++;
        layout->ڈھیر[++layout->گہرائی] = (indentation_level){ستون, altcol};
    }
    ورنہ {
        جب (layout->گہرائی > 0 &&
            ستون < layout->ڈھیر[layout->گہرائی].عمود) {
            layout->pending--;
            layout->گہرائی--;
        }
        اگر (ستون != layout->ڈھیر[layout->گہرائی].عمود) {
            tok->مکمل = E_DEDENT;
            tok->cur = tok->inp;
            واپس -1;
        }
        اگر (altcol != layout->ڈھیر[layout->گہرائی].alternate_عمود) {
            _PyTokenizer_indenterror(tok);
            واپس -1;
        }
    }
    واپس 0;
}

صحیح
Py_Lexer_Begin_سطر(ساخت tok_state *tok)
{
    أكد(tok->layout.at_bol);
    صحیح c;
    صحیح blankline = 0;
    صحیح ستون = 0;
    صحیح altcol = 0;
    tok->layout.at_bol = 0;
    صحیح cont_سطر_ستون = 0;
    برائے (;;) {
        c = tok_nextc(tok);
        اگر (c == ' ') {
            ستون++, altcol++;
        }
        ورنہ اگر (c == '\t') {
            ستون = (ستون / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ورنہ اگر (c == '\014')  {/* Control-L (formfeed) */
            ستون = altcol = 0; /* For Emacs users */
        }
        ورنہ اگر (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_سطر_ستون = cont_سطر_ستون ? cont_سطر_ستون : ستون;
            اگر ((c = Py_Lexer_Continue_سطر(tok)) == -1) {
                واپس -1;
            }
        }
        ورنہ اگر (c == EOF && PyErr_Occurred()) {
            واپس -1;
        }
        ورنہ {
            توڑو;
        }
    }
    tok_backup(tok, c);
    اگر (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_سطر_ستون || ستون)) {
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
        اگر (ستون == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ورنہ اگر (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            ستون = altcol = 0;
        }
        ورنہ {
            blankline = 1; /* Ignore completely */
        }
    }
    اگر (!blankline && tok->level == 0) {
        ستون = cont_سطر_ستون ? cont_سطر_ستون : ستون;
        altcol = cont_سطر_ستون ? cont_سطر_ستون : altcol;
        اگر (update_indentation(tok, ستون, altcol) < 0) {
            واپس -1;
        }
    }
    واپس blankline;
}

صحیح
_PyLexer_IndentationToken(ساخت tok_state *tok, ساخت token *token)
{
    أكد(tok->layout.pending != 0);
    _PyTok_Off p_آغاز = -1;
    _PyTok_Off p_انتہا = -1;
    اگر (tok->layout.pending < 0) {
        اگر (tok->tok_extra_علامات) {
            p_آغاز = tok->cur;
            p_انتہا = tok->cur;
        }
        tok->layout.pending++;
        واپس Py_Lexer_token_تیاری(tok, token, DEDENT, p_آغاز, p_انتہا);
    }
    ورنہ {
        اگر (tok->tok_extra_علامات) {
            p_آغاز = tok->buf_انحراف;
            p_انتہا = tok->cur;
        }
        tok->layout.pending--;
        واپس Py_Lexer_token_تیاری(tok, token, INDENT, p_آغاز, p_انتہا);
    }
}

صحیح
_PyLexer_Newline(ساخت tok_state *tok, ساخت token *token, صحیح blankline)
{
    اگر (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    اگر (blankline || tok->level > 0) {
        اگر (!tok->tok_extra_علامات) {
            واپس 0;
        }
    }
    ورنہ اگر (!tok->layout.comment_newline || !tok->tok_extra_علامات) {
        واپس Py_Lexer_token_تیاری(tok, token, NEWLINE,
                                    tok->آغاز, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    واپس Py_Lexer_token_تیاری(tok, token, NL, tok->آغاز, tok->cur);
}

خالی
_PyLexer_ImplyDedents(ساخت tok_state *tok)
{
    اگر (tok->layout.گہرائی != 0) {
        tok->layout.pending = -tok->layout.گہرائی;
        tok->layout.گہرائی = 0;
    }
}
