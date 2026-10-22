#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

цео
_PyLexer_ContinueLine(struct tok_state *tok)
{
    цео c = tok_nextc(tok);
    ако (c == '\r') {
        c = tok_nextc(tok);
    }
    ако (c != '\n') {
        tok->done = E_LINECONT;
        врати -1;
    }
    c = tok_nextc(tok);
    ако (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ако (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        врати -1;
    } инако {
        tok_backup(tok, c);
    }
    врати c;
}


static цео
update_indentation(struct tok_state *tok, цео col, цео altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ако (col == layout->stack[layout->depth].column) {
        ако (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            врати -1;
        }
    }
    инако ако (col > layout->stack[layout->depth].column) {
        ако (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            врати -1;
        }
        ако (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            врати -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    инако {
        док (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ако (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            врати -1;
        }
        ако (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            врати -1;
        }
    }
    врати 0;
}

цео
_PyLexer_BeginLine(struct tok_state *tok)
{
    assert(tok->layout.at_bol);
    цео c;
    цео blankline = 0;
    цео col = 0;
    цео altcol = 0;
    tok->layout.at_bol = 0;
    цео cont_line_col = 0;
    за (;;) {
        c = tok_nextc(tok);
        ако (c == ' ') {
            col++, altcol++;
        }
        инако ако (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        инако ако (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        инако ако (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            ако ((c = _PyLexer_ContinueLine(tok)) == -1) {
                врати -1;
            }
        }
        инако ако (c == EOF && PyErr_Occurred()) {
            врати -1;
        }
        инако {
            break;
        }
    }
    tok_backup(tok, c);
    ако (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ако (c == '#' || c == '\n' || c == '\r') {
        цео interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ако (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        инако ако (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        инако {
            blankline = 1; /* Ignore completely */
        }
    }
    ако (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        ако (update_indentation(tok, col, altcol) < 0) {
            врати -1;
        }
    }
    врати blankline;
}

цео
_PyLexer_IndentationToken(struct tok_state *tok, struct token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ако (tok->layout.pending < 0) {
        ако (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        врати _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    инако {
        ако (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        врати _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

цео
_PyLexer_Newline(struct tok_state *tok, struct token *token, цео blankline)
{
    ако (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ако (blankline || tok->level > 0) {
        ако (!tok->tok_extra_tokens) {
            врати 0;
        }
    }
    инако ако (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        врати _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    врати _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(struct tok_state *tok)
{
    ако (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
