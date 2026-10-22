#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ཧྲིལ
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    ཧྲིལ c = tok_nextc(tok);
    གལ་ཏེ (c == '\r') {
        c = tok_nextc(tok);
    }
    གལ་ཏེ (c != '\n') {
        tok->done = E_LINECONT;
        ཕྱིར -1;
    }
    c = tok_nextc(tok);
    གལ་ཏེ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    གལ་ཏེ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ཕྱིར -1;
    } དེ་མིན {
        tok_backup(tok, c);
    }
    ཕྱིར c;
}


static ཧྲིལ
update_indentation(sa_nracanaa tok_state *tok, ཧྲིལ col, ཧྲིལ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    གལ་ཏེ (col == layout->stack[layout->depth].column) {
        གལ་ཏེ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ཕྱིར -1;
        }
    }
    དེ་མིན གལ་ཏེ (col > layout->stack[layout->depth].column) {
        གལ་ཏེ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ཕྱིར -1;
        }
        གལ་ཏེ (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ཕྱིར -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    དེ་མིན {
        སྐབས (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        གལ་ཏེ (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ཕྱིར -1;
        }
        གལ་ཏེ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ཕྱིར -1;
        }
    }
    ཕྱིར 0;
}

ཧྲིལ
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    ཧྲིལ c;
    ཧྲིལ blankline = 0;
    ཧྲིལ col = 0;
    ཧྲིལ altcol = 0;
    tok->layout.at_bol = 0;
    ཧྲིལ cont_line_col = 0;
    ཆེད་དུ (;;) {
        c = tok_nextc(tok);
        གལ་ཏེ (c == ' ') {
            col++, altcol++;
        }
        དེ་མིན གལ་ཏེ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        དེ་མིན གལ་ཏེ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        དེ་མིན གལ་ཏེ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            གལ་ཏེ ((c = _PyLexer_ContinueLine(tok)) == -1) {
                ཕྱིར -1;
            }
        }
        དེ་མིན གལ་ཏེ (c == EOF && PyErr_Occurred()) {
            ཕྱིར -1;
        }
        དེ་མིན {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    གལ་ཏེ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    གལ་ཏེ (c == '#' || c == '\n' || c == '\r') {
        ཧྲིལ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        གལ་ཏེ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        དེ་མིན གལ་ཏེ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        དེ་མིན {
            blankline = 1; /* Ignore completely */
        }
    }
    གལ་ཏེ (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        གལ་ཏེ (update_indentation(tok, col, altcol) < 0) {
            ཕྱིར -1;
        }
    }
    ཕྱིར blankline;
}

ཧྲིལ
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    གལ་ཏེ (tok->layout.pending < 0) {
        གལ་ཏེ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ཕྱིར _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    དེ་མིན {
        གལ་ཏེ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ཕྱིར _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ཧྲིལ
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ཧྲིལ blankline)
{
    གལ་ཏེ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    གལ་ཏེ (blankline || tok->level > 0) {
        གལ་ཏེ (!tok->tok_extra_tokens) {
            ཕྱིར 0;
        }
    }
    དེ་མིན གལ་ཏེ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ཕྱིར _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ཕྱིར _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    གལ་ཏེ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
