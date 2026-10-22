#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ಪೂರ್ಣಾಙ್ಕ
Py_Lexer_Continue_ಸಾಲು(ಸಂರಚನಾ tok_state *tok)
{
    ಪೂರ್ಣಾಙ್ಕ c = tok_nextc(tok);
    ಯದಿ (c == '\r') {
        c = tok_nextc(tok);
    }
    ಯದಿ (c != '\n') {
        tok->done = E_LINECONT;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    c = tok_nextc(tok);
    ಯದಿ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ಯದಿ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ಪ್ರತ್ಯಾಗಮ -1;
    } ಅನ್ಯಥಾ {
        tok_backup(tok, c);
    }
    ಪ್ರತ್ಯಾಗಮ c;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
update_indentation(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ಯದಿ (col == layout->ರಾಶಿ[layout->depth].column) {
        ಯದಿ (altcol != layout->ರಾಶಿ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }
    ಅನ್ಯಥಾ ಯದಿ (col > layout->ರಾಶಿ[layout->depth].column) {
        ಯದಿ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        ಯದಿ (altcol <= layout->ರಾಶಿ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        layout->pending++;
        layout->ರಾಶಿ[++layout->depth] = (indentation_level){col, altcol};
    }
    ಅನ್ಯಥಾ {
        ಯಾವತ್ (layout->depth > 0 &&
            col < layout->ರಾಶಿ[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ಯದಿ (col != layout->ರಾಶಿ[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        ಯದಿ (altcol != layout->ರಾಶಿ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
Py_Lexer_Begin_ಸಾಲು(ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->layout.at_bol);
    ಪೂರ್ಣಾಙ್ಕ c;
    ಪೂರ್ಣಾಙ್ಕ blankline = 0;
    ಪೂರ್ಣಾಙ್ಕ col = 0;
    ಪೂರ್ಣಾಙ್ಕ altcol = 0;
    tok->layout.at_bol = 0;
    ಪೂರ್ಣಾಙ್ಕ cont_ಸಾಲು_col = 0;
    ಪ್ರತಿ (;;) {
        c = tok_nextc(tok);
        ಯದಿ (c == ' ') {
            col++, altcol++;
        }
        ಅನ್ಯಥಾ ಯದಿ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ಅನ್ಯಥಾ ಯದಿ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        ಅನ್ಯಥಾ ಯದಿ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_ಸಾಲು_col = cont_ಸಾಲು_col ? cont_ಸಾಲು_col : col;
            ಯದಿ ((c = Py_Lexer_Continue_ಸಾಲು(tok)) == -1) {
                ಪ್ರತ್ಯಾಗಮ -1;
            }
        }
        ಅನ್ಯಥಾ ಯದಿ (c == EOF && PyErr_Occurred()) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        ಅನ್ಯಥಾ {
            ಭಙ್ಗ;
        }
    }
    tok_backup(tok, c);
    ಯದಿ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_ಸಾಲು_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ಯದಿ (c == '#' || c == '\n' || c == '\r') {
        ಪೂರ್ಣಾಙ್ಕ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ಯದಿ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ಅನ್ಯಥಾ ಯದಿ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        ಅನ್ಯಥಾ {
            blankline = 1; /* Ignore completely */
        }
    }
    ಯದಿ (!blankline && tok->level == 0) {
        col = cont_ಸಾಲು_col ? cont_ಸಾಲು_col : col;
        altcol = cont_ಸಾಲು_col ? cont_ಸಾಲು_col : altcol;
        ಯದಿ (update_indentation(tok, col, altcol) < 0) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }
    ಪ್ರತ್ಯಾಗಮ blankline;
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_IndentationToken(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ಯದಿ (tok->layout.pending < 0) {
        ಯದಿ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ಪ್ರತ್ಯಾಗಮ _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    ಅನ್ಯಥಾ {
        ಯದಿ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ಪ್ರತ್ಯಾಗಮ _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_Newline(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token, ಪೂರ್ಣಾಙ್ಕ blankline)
{
    ಯದಿ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ಯದಿ (blankline || tok->level > 0) {
        ಯದಿ (!tok->tok_extra_tokens) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಅನ್ಯಥಾ ಯದಿ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ಪ್ರತ್ಯಾಗಮ _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ಪ್ರತ್ಯಾಗಮ _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

ಶೂನ್ಯ
_PyLexer_ImplyDedents(ಸಂರಚನಾ tok_state *tok)
{
    ಯದಿ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
