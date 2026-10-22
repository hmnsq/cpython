#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/reader.h"

#ವ್ಯಾಖ್ಯಾನಿಸು TABSIZE 8
#ವ್ಯಾಖ್ಯಾನಿಸು ALTTABSIZE 1

ಪೂರ್ಣಾಂಕ
Py_Lexer_Continue_ಸಾಲು(ರಚನೆ tok_state *tok)
{
    ಪೂರ್ಣಾಂಕ c = tok_nextc(tok);
    ಆದರೆ (c == '\r') {
        c = tok_nextc(tok);
    }
    ಆದರೆ (c != '\n') {
        tok->ಮುಗಿದ = E_LINECONT;
        ಹಿಂತಿರುಗಿ -1;
    }
    c = tok_nextc(tok);
    ಆದರೆ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ಆದರೆ (c == EOF) {
        tok->ಮುಗಿದ = E_EOF;
        tok->cur = tok->inp;
        ಹಿಂತಿರುಗಿ -1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        tok_backup(tok, c);
    }
    ಹಿಂತಿರುಗಿ c;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
update_indentation(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ಆದರೆ (ಲಂಬಸಾಲು == layout->ರಾಶಿ[layout->ಆಳ].ಸ್ತಂಭ) {
        ಆದರೆ (altcol != layout->ರಾಶಿ[layout->ಆಳ].alternate_ಸ್ತಂಭ) {
            _PyTokenizer_indenterror(tok);
            ಹಿಂತಿರುಗಿ -1;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು > layout->ರಾಶಿ[layout->ಆಳ].ಸ್ತಂಭ) {
        ಆದರೆ (layout->ಆಳ + 1 >= MAXINDENT) {
            tok->ಮುಗಿದ = E_TOODEEP;
            tok->cur = tok->inp;
            ಹಿಂತಿರುಗಿ -1;
        }
        ಆದರೆ (altcol <= layout->ರಾಶಿ[layout->ಆಳ].alternate_ಸ್ತಂಭ) {
            _PyTokenizer_indenterror(tok);
            ಹಿಂತಿರುಗಿ -1;
        }
        layout->pending++;
        layout->ರಾಶಿ[++layout->ಆಳ] = (indentation_level){ಲಂಬಸಾಲು, altcol};
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಆಗ (layout->ಆಳ > 0 &&
            ಲಂಬಸಾಲು < layout->ರಾಶಿ[layout->ಆಳ].ಸ್ತಂಭ) {
            layout->pending--;
            layout->ಆಳ--;
        }
        ಆದರೆ (ಲಂಬಸಾಲು != layout->ರಾಶಿ[layout->ಆಳ].ಸ್ತಂಭ) {
            tok->ಮುಗಿದ = E_DEDENT;
            tok->cur = tok->inp;
            ಹಿಂತಿರುಗಿ -1;
        }
        ಆದರೆ (altcol != layout->ರಾಶಿ[layout->ಆಳ].alternate_ಸ್ತಂಭ) {
            _PyTokenizer_indenterror(tok);
            ಹಿಂತಿರುಗಿ -1;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_Begin_ಸಾಲು(ರಚನೆ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->layout.at_bol);
    ಪೂರ್ಣಾಂಕ c;
    ಪೂರ್ಣಾಂಕ blankline = 0;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ altcol = 0;
    tok->layout.at_bol = 0;
    ಪೂರ್ಣಾಂಕ cont_ಸಾಲು_ಲಂಬಸಾಲು = 0;
    ಗಾಗಿ (;;) {
        c = tok_nextc(tok);
        ಆದರೆ (c == ' ') {
            ಲಂಬಸಾಲು++, altcol++;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\t') {
            ಲಂಬಸಾಲು = (ಲಂಬಸಾಲು / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\014')  {/* Control-L (formfeed) */
            ಲಂಬಸಾಲು = altcol = 0; /* For Emacs users */
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_ಸಾಲು_ಲಂಬಸಾಲು = cont_ಸಾಲು_ಲಂಬಸಾಲು ? cont_ಸಾಲು_ಲಂಬಸಾಲು : ಲಂಬಸಾಲು;
            ಆದರೆ ((c = Py_Lexer_Continue_ಸಾಲು(tok)) == -1) {
                ಹಿಂತಿರುಗಿ -1;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == EOF && PyErr_Occurred()) {
            ಹಿಂತಿರುಗಿ -1;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
    }
    tok_backup(tok, c);
    ಆದರೆ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_ಸಾಲು_ಲಂಬಸಾಲು || ಲಂಬಸಾಲು)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ಆದರೆ (c == '#' || c == '\n' || c == '\r') {
        ಪೂರ್ಣಾಂಕ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ಆದರೆ (ಲಂಬಸಾಲು == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            ಲಂಬಸಾಲು = altcol = 0;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            blankline = 1; /* Ignore completely */
        }
    }
    ಆದರೆ (!blankline && tok->level == 0) {
        ಲಂಬಸಾಲು = cont_ಸಾಲು_ಲಂಬಸಾಲು ? cont_ಸಾಲು_ಲಂಬಸಾಲು : ಲಂಬಸಾಲು;
        altcol = cont_ಸಾಲು_ಲಂಬಸಾಲು ? cont_ಸಾಲು_ಲಂಬಸಾಲು : altcol;
        ಆದರೆ (update_indentation(tok, ಲಂಬಸಾಲು, altcol) < 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
    }
    ಹಿಂತಿರುಗಿ blankline;
}

ಪೂರ್ಣಾಂಕ
_PyLexer_IndentationToken(ರಚನೆ tok_state *tok, ರಚನೆ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(tok->layout.pending != 0);
    _PyTok_Off p_ಆರಂಭ = -1;
    _PyTok_Off p_ಕೊನೆ = -1;
    ಆದರೆ (tok->layout.pending < 0) {
        ಆದರೆ (tok->tok_extra_ಟೋಕನುಗಳು) {
            p_ಆರಂಭ = tok->cur;
            p_ಕೊನೆ = tok->cur;
        }
        tok->layout.pending++;
        ಹಿಂತಿರುಗಿ Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, DEDENT, p_ಆರಂಭ, p_ಕೊನೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (tok->tok_extra_ಟೋಕನುಗಳು) {
            p_ಆರಂಭ = tok->buf_ಸ್ಥಾನಾಂತರ;
            p_ಕೊನೆ = tok->cur;
        }
        tok->layout.pending--;
        ಹಿಂತಿರುಗಿ Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, INDENT, p_ಆರಂಭ, p_ಕೊನೆ);
    }
}

ಪೂರ್ಣಾಂಕ
_PyLexer_Newline(ರಚನೆ tok_state *tok, ರಚನೆ token *token, ಪೂರ್ಣಾಂಕ blankline)
{
    ಆದರೆ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ಆದರೆ (blankline || tok->level > 0) {
        ಆದರೆ (!tok->tok_extra_ಟೋಕನುಗಳು) {
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!tok->layout.comment_newline || !tok->tok_extra_ಟೋಕನುಗಳು) {
        ಹಿಂತಿರುಗಿ Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, NEWLINE,
                                    tok->ಆರಂಭ, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ಹಿಂತಿರುಗಿ Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, NL, tok->ಆರಂಭ, tok->cur);
}

ಶೂನ್ಯ
_PyLexer_ImplyDedents(ರಚನೆ tok_state *tok)
{
    ಆದರೆ (tok->layout.ಆಳ != 0) {
        tok->layout.pending = -tok->layout.ಆಳ;
        tok->layout.ಆಳ = 0;
    }
}
