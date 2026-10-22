#অন্তর্ভাৱ "Python.h"
#অন্তর্ভাৱ "errcode.h"
#অন্তর্ভাৱ "pycore_token.h"

#অন্তর্ভাৱ "lexer_internal.h"
#অন্তর্ভাৱ "../tokenizer/helpers.h"
#অন্তর্ভাৱ "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

পূর্ণাঙ্ক
Py_Lexer_Continue_সাৰি(সংরচনা tok_state *tok)
{
    পূর্ণাঙ্ক c = tok_nextc(tok);
    যদি (c == '\r') {
        c = tok_nextc(tok);
    }
    যদি (c != '\n') {
        tok->done = E_LINECONT;
        ঘূৰাই -1;
    }
    c = tok_nextc(tok);
    যদি (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    যদি (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ঘূৰাই -1;
    } অন্যথা {
        tok_backup(tok, c);
    }
    ঘূৰাই c;
}


স্থির পূর্ণাঙ্ক
update_indentation(সংরচনা tok_state *tok, পূর্ণাঙ্ক col, পূর্ণাঙ্ক altcol)
{
    lexer_layout_state *layout = &tok->layout;
    যদি (col == layout->স্তূপ[layout->depth].column) {
        যদি (altcol != layout->স্তূপ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ঘূৰাই -1;
        }
    }
    অন্যথা যদি (col > layout->স্তূপ[layout->depth].column) {
        যদি (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ঘূৰাই -1;
        }
        যদি (altcol <= layout->স্তূপ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ঘূৰাই -1;
        }
        layout->pending++;
        layout->স্তূপ[++layout->depth] = (indentation_level){col, altcol};
    }
    অন্যথা {
        যেতিয়ালৈ (layout->depth > 0 &&
            col < layout->স্তূপ[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        যদি (col != layout->স্তূপ[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ঘূৰাই -1;
        }
        যদি (altcol != layout->স্তূপ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ঘূৰাই -1;
        }
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
Py_Lexer_Begin_সাৰি(সংরচনা tok_state *tok)
{
    প্রতিজ্ঞা(tok->layout.at_bol);
    পূর্ণাঙ্ক c;
    পূর্ণাঙ্ক blankline = 0;
    পূর্ণাঙ্ক col = 0;
    পূর্ণাঙ্ক altcol = 0;
    tok->layout.at_bol = 0;
    পূর্ণাঙ্ক cont_সাৰি_col = 0;
    বাবে (;;) {
        c = tok_nextc(tok);
        যদি (c == ' ') {
            col++, altcol++;
        }
        অন্যথা যদি (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        অন্যথা যদি (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        অন্যথা যদি (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_সাৰি_col = cont_সাৰি_col ? cont_সাৰি_col : col;
            যদি ((c = Py_Lexer_Continue_সাৰি(tok)) == -1) {
                ঘূৰাই -1;
            }
        }
        অন্যথা যদি (c == EOF && PyErr_Occurred()) {
            ঘূৰাই -1;
        }
        অন্যথা {
            ভঙ্গ;
        }
    }
    tok_backup(tok, c);
    যদি (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_সাৰি_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    যদি (c == '#' || c == '\n' || c == '\r') {
        পূর্ণাঙ্ক interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        যদি (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        অন্যথা যদি (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        অন্যথা {
            blankline = 1; /* Ignore completely */
        }
    }
    যদি (!blankline && tok->level == 0) {
        col = cont_সাৰি_col ? cont_সাৰি_col : col;
        altcol = cont_সাৰি_col ? cont_সাৰি_col : altcol;
        যদি (update_indentation(tok, col, altcol) < 0) {
            ঘূৰাই -1;
        }
    }
    ঘূৰাই blankline;
}

পূর্ণাঙ্ক
_PyLexer_IndentationToken(সংরচনা tok_state *tok, সংরচনা token *token)
{
    প্রতিজ্ঞা(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    যদি (tok->layout.pending < 0) {
        যদি (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ঘূৰাই _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    অন্যথা {
        যদি (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ঘূৰাই _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

পূর্ণাঙ্ক
_PyLexer_Newline(সংরচনা tok_state *tok, সংরচনা token *token, পূর্ণাঙ্ক blankline)
{
    যদি (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    যদি (blankline || tok->level > 0) {
        যদি (!tok->tok_extra_tokens) {
            ঘূৰাই 0;
        }
    }
    অন্যথা যদি (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ঘূৰাই _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ঘূৰাই _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

শূন্য
_PyLexer_ImplyDedents(সংরচনা tok_state *tok)
{
    যদি (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
