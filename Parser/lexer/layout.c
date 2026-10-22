#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "lexer_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/helpers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/reader.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TABSIZE 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALTTABSIZE 1

ਪੂਰਨ
Py_Lexer_Continue_ਸਤਰ(ਬਣਤਰ tok_state *tok)
{
    ਪੂਰਨ c = tok_nextc(tok);
    ਜੇ (c == '\r') {
        c = tok_nextc(tok);
    }
    ਜੇ (c != '\n') {
        tok->ਪੂਰਾ = E_LINECONT;
        ਵਾਪਸ -1;
    }
    c = tok_nextc(tok);
    ਜੇ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ਜੇ (c == EOF) {
        tok->ਪੂਰਾ = E_EOF;
        tok->cur = tok->inp;
        ਵਾਪਸ -1;
    } ਨਹੀਂਤਾਂ {
        tok_backup(tok, c);
    }
    ਵਾਪਸ c;
}


ਸਥਿਰ ਪੂਰਨ
update_indentation(ਬਣਤਰ tok_state *tok, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ਜੇ (ਖਾਨਾ == layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].ਕਾਲਮ) {
        ਜੇ (altcol != layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].alternate_ਕਾਲਮ) {
            _PyTokenizer_indenterror(tok);
            ਵਾਪਸ -1;
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨਾ > layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].ਕਾਲਮ) {
        ਜੇ (layout->ਡੂੰਘਾਈ + 1 >= MAXINDENT) {
            tok->ਪੂਰਾ = E_TOODEEP;
            tok->cur = tok->inp;
            ਵਾਪਸ -1;
        }
        ਜੇ (altcol <= layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].alternate_ਕਾਲਮ) {
            _PyTokenizer_indenterror(tok);
            ਵਾਪਸ -1;
        }
        layout->pending++;
        layout->ਢੇਰ[++layout->ਡੂੰਘਾਈ] = (indentation_level){ਖਾਨਾ, altcol};
    }
    ਨਹੀਂਤਾਂ {
        ਜਦੋਂ (layout->ਡੂੰਘਾਈ > 0 &&
            ਖਾਨਾ < layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].ਕਾਲਮ) {
            layout->pending--;
            layout->ਡੂੰਘਾਈ--;
        }
        ਜੇ (ਖਾਨਾ != layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].ਕਾਲਮ) {
            tok->ਪੂਰਾ = E_DEDENT;
            tok->cur = tok->inp;
            ਵਾਪਸ -1;
        }
        ਜੇ (altcol != layout->ਢੇਰ[layout->ਡੂੰਘਾਈ].alternate_ਕਾਲਮ) {
            _PyTokenizer_indenterror(tok);
            ਵਾਪਸ -1;
        }
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
Py_Lexer_Begin_ਸਤਰ(ਬਣਤਰ tok_state *tok)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->layout.at_bol);
    ਪੂਰਨ c;
    ਪੂਰਨ blankline = 0;
    ਪੂਰਨ ਖਾਨਾ = 0;
    ਪੂਰਨ altcol = 0;
    tok->layout.at_bol = 0;
    ਪੂਰਨ cont_ਸਤਰ_ਖਾਨਾ = 0;
    ਲਈ (;;) {
        c = tok_nextc(tok);
        ਜੇ (c == ' ') {
            ਖਾਨਾ++, altcol++;
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == '\t') {
            ਖਾਨਾ = (ਖਾਨਾ / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == '\014')  {/* Control-L (formfeed) */
            ਖਾਨਾ = altcol = 0; /* For Emacs users */
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_ਸਤਰ_ਖਾਨਾ = cont_ਸਤਰ_ਖਾਨਾ ? cont_ਸਤਰ_ਖਾਨਾ : ਖਾਨਾ;
            ਜੇ ((c = Py_Lexer_Continue_ਸਤਰ(tok)) == -1) {
                ਵਾਪਸ -1;
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == EOF && PyErr_Occurred()) {
            ਵਾਪਸ -1;
        }
        ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
    }
    tok_backup(tok, c);
    ਜੇ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_ਸਤਰ_ਖਾਨਾ || ਖਾਨਾ)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ਜੇ (c == '#' || c == '\n' || c == '\r') {
        ਪੂਰਨ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ਜੇ (ਖਾਨਾ == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ਨਹੀਂਤਾਂ ਜੇ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            ਖਾਨਾ = altcol = 0;
        }
        ਨਹੀਂਤਾਂ {
            blankline = 1; /* Ignore completely */
        }
    }
    ਜੇ (!blankline && tok->level == 0) {
        ਖਾਨਾ = cont_ਸਤਰ_ਖਾਨਾ ? cont_ਸਤਰ_ਖਾਨਾ : ਖਾਨਾ;
        altcol = cont_ਸਤਰ_ਖਾਨਾ ? cont_ਸਤਰ_ਖਾਨਾ : altcol;
        ਜੇ (update_indentation(tok, ਖਾਨਾ, altcol) < 0) {
            ਵਾਪਸ -1;
        }
    }
    ਵਾਪਸ blankline;
}

ਪੂਰਨ
_PyLexer_IndentationToken(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->layout.pending != 0);
    _PyTok_Off p_ਸ਼ੁਰੂ = -1;
    _PyTok_Off p_ਸਿਰਾ = -1;
    ਜੇ (tok->layout.pending < 0) {
        ਜੇ (tok->tok_extra_ਟੋਕਨ) {
            p_ਸ਼ੁਰੂ = tok->cur;
            p_ਸਿਰਾ = tok->cur;
        }
        tok->layout.pending++;
        ਵਾਪਸ Py_Lexer_token_ਤਿਆਰੀ(tok, token, DEDENT, p_ਸ਼ੁਰੂ, p_ਸਿਰਾ);
    }
    ਨਹੀਂਤਾਂ {
        ਜੇ (tok->tok_extra_ਟੋਕਨ) {
            p_ਸ਼ੁਰੂ = tok->buf_ਵਿੱਥ;
            p_ਸਿਰਾ = tok->cur;
        }
        tok->layout.pending--;
        ਵਾਪਸ Py_Lexer_token_ਤਿਆਰੀ(tok, token, INDENT, p_ਸ਼ੁਰੂ, p_ਸਿਰਾ);
    }
}

ਪੂਰਨ
_PyLexer_Newline(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token, ਪੂਰਨ blankline)
{
    ਜੇ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ਜੇ (blankline || tok->level > 0) {
        ਜੇ (!tok->tok_extra_ਟੋਕਨ) {
            ਵਾਪਸ 0;
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (!tok->layout.comment_newline || !tok->tok_extra_ਟੋਕਨ) {
        ਵਾਪਸ Py_Lexer_token_ਤਿਆਰੀ(tok, token, NEWLINE,
                                    tok->ਸ਼ੁਰੂ, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ਵਾਪਸ Py_Lexer_token_ਤਿਆਰੀ(tok, token, NL, tok->ਸ਼ੁਰੂ, tok->cur);
}

ਖਾਲੀ
_PyLexer_ImplyDedents(ਬਣਤਰ tok_state *tok)
{
    ਜੇ (tok->layout.ਡੂੰਘਾਈ != 0) {
        tok->layout.pending = -tok->layout.ਡੂੰਘਾਈ;
        tok->layout.ਡੂੰਘਾਈ = 0;
    }
}
