#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ціле
_PyLexer_ContinueLine(struct tok_state *tok)
{
    ціле c = tok_nextc(tok);
    якщо (c == '\r') {
        c = tok_nextc(tok);
    }
    якщо (c != '\n') {
        tok->done = E_LINECONT;
        повернути -1;
    }
    c = tok_nextc(tok);
    якщо (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    якщо (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        повернути -1;
    } інакше {
        tok_backup(tok, c);
    }
    повернути c;
}


static ціле
update_indentation(struct tok_state *tok, ціле col, ціле altcol)
{
    lexer_layout_state *layout = &tok->layout;
    якщо (col == layout->stack[layout->depth].column) {
        якщо (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            повернути -1;
        }
    }
    інакше якщо (col > layout->stack[layout->depth].column) {
        якщо (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            повернути -1;
        }
        якщо (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            повернути -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    інакше {
        поки (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        якщо (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            повернути -1;
        }
        якщо (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            повернути -1;
        }
    }
    повернути 0;
}

ціле
_PyLexer_BeginLine(struct tok_state *tok)
{
    assert(tok->layout.at_bol);
    ціле c;
    ціле blankline = 0;
    ціле col = 0;
    ціле altcol = 0;
    tok->layout.at_bol = 0;
    ціле cont_line_col = 0;
    для (;;) {
        c = tok_nextc(tok);
        якщо (c == ' ') {
            col++, altcol++;
        }
        інакше якщо (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        інакше якщо (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        інакше якщо (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            якщо ((c = _PyLexer_ContinueLine(tok)) == -1) {
                повернути -1;
            }
        }
        інакше якщо (c == EOF && PyErr_Occurred()) {
            повернути -1;
        }
        інакше {
            break;
        }
    }
    tok_backup(tok, c);
    якщо (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    якщо (c == '#' || c == '\n' || c == '\r') {
        ціле interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        якщо (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        інакше якщо (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        інакше {
            blankline = 1; /* Ignore completely */
        }
    }
    якщо (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        якщо (update_indentation(tok, col, altcol) < 0) {
            повернути -1;
        }
    }
    повернути blankline;
}

ціле
_PyLexer_IndentationToken(struct tok_state *tok, struct token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    якщо (tok->layout.pending < 0) {
        якщо (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        повернути _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    інакше {
        якщо (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        повернути _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ціле
_PyLexer_Newline(struct tok_state *tok, struct token *token, ціле blankline)
{
    якщо (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    якщо (blankline || tok->level > 0) {
        якщо (!tok->tok_extra_tokens) {
            повернути 0;
        }
    }
    інакше якщо (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        повернути _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    повернути _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(struct tok_state *tok)
{
    якщо (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
