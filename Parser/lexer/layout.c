#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "errcode.h"
#അന്തര്ഭാവ "pycore_token.h"

#അന്തര്ഭാവ "lexer_internal.h"
#അന്തര്ഭാവ "../tokenizer/helpers.h"
#അന്തര്ഭാവ "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

പൂർണ്ണസംഖ്യ
Py_Lexer_Continue_വരി(ഘടന tok_state *tok)
{
    പൂർണ്ണസംഖ്യ c = tok_nextc(tok);
    എങ്കിൽ (c == '\r') {
        c = tok_nextc(tok);
    }
    എങ്കിൽ (c != '\n') {
        tok->done = E_LINECONT;
        തിരികെ -1;
    }
    c = tok_nextc(tok);
    എങ്കിൽ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    എങ്കിൽ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        തിരികെ -1;
    } അല്ലെങ്കിൽ {
        tok_backup(tok, c);
    }
    തിരികെ c;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ
update_indentation(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    എങ്കിൽ (col == layout->അട്ടി[layout->depth].column) {
        എങ്കിൽ (altcol != layout->അട്ടി[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            തിരികെ -1;
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (col > layout->അട്ടി[layout->depth].column) {
        എങ്കിൽ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            തിരികെ -1;
        }
        എങ്കിൽ (altcol <= layout->അട്ടി[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            തിരികെ -1;
        }
        layout->pending++;
        layout->അട്ടി[++layout->depth] = (indentation_level){col, altcol};
    }
    അല്ലെങ്കിൽ {
        ആകുമ്പോൾ (layout->depth > 0 &&
            col < layout->അട്ടി[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        എങ്കിൽ (col != layout->അട്ടി[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            തിരികെ -1;
        }
        എങ്കിൽ (altcol != layout->അട്ടി[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            തിരികെ -1;
        }
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
Py_Lexer_Begin_വരി(ഘടന tok_state *tok)
{
    പ്രതിജ്ഞാ(tok->layout.at_bol);
    പൂർണ്ണസംഖ്യ c;
    പൂർണ്ണസംഖ്യ blankline = 0;
    പൂർണ്ണസംഖ്യ col = 0;
    പൂർണ്ണസംഖ്യ altcol = 0;
    tok->layout.at_bol = 0;
    പൂർണ്ണസംഖ്യ cont_വരി_col = 0;
    വേണ്ടി (;;) {
        c = tok_nextc(tok);
        എങ്കിൽ (c == ' ') {
            col++, altcol++;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_വരി_col = cont_വരി_col ? cont_വരി_col : col;
            എങ്കിൽ ((c = Py_Lexer_Continue_വരി(tok)) == -1) {
                തിരികെ -1;
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == EOF && PyErr_Occurred()) {
            തിരികെ -1;
        }
        അല്ലെങ്കിൽ {
            നിർത്തുക;
        }
    }
    tok_backup(tok, c);
    എങ്കിൽ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_വരി_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    എങ്കിൽ (c == '#' || c == '\n' || c == '\r') {
        പൂർണ്ണസംഖ്യ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        എങ്കിൽ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        അല്ലെങ്കിൽ എങ്കിൽ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        അല്ലെങ്കിൽ {
            blankline = 1; /* Ignore completely */
        }
    }
    എങ്കിൽ (!blankline && tok->level == 0) {
        col = cont_വരി_col ? cont_വരി_col : col;
        altcol = cont_വരി_col ? cont_വരി_col : altcol;
        എങ്കിൽ (update_indentation(tok, col, altcol) < 0) {
            തിരികെ -1;
        }
    }
    തിരികെ blankline;
}

പൂർണ്ണസംഖ്യ
_PyLexer_IndentationToken(ഘടന tok_state *tok, ഘടന token *token)
{
    പ്രതിജ്ഞാ(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    എങ്കിൽ (tok->layout.pending < 0) {
        എങ്കിൽ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        തിരികെ _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    അല്ലെങ്കിൽ {
        എങ്കിൽ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        തിരികെ _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

പൂർണ്ണസംഖ്യ
_PyLexer_Newline(ഘടന tok_state *tok, ഘടന token *token, പൂർണ്ണസംഖ്യ blankline)
{
    എങ്കിൽ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    എങ്കിൽ (blankline || tok->level > 0) {
        എങ്കിൽ (!tok->tok_extra_tokens) {
            തിരികെ 0;
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        തിരികെ _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    തിരികെ _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

ശൂന്യം
_PyLexer_ImplyDedents(ഘടന tok_state *tok)
{
    എങ്കിൽ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
