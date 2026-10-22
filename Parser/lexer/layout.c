#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

số_nguyên
_PyLexer_ContinueLine(struct tok_state *tok)
{
    số_nguyên c = tok_nextc(tok);
    nếu (c == '\r') {
        c = tok_nextc(tok);
    }
    nếu (c != '\n') {
        tok->done = E_LINECONT;
        trả_về -1;
    }
    c = tok_nextc(tok);
    nếu (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    nếu (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        trả_về -1;
    } không_thì {
        tok_backup(tok, c);
    }
    trả_về c;
}


static số_nguyên
update_indentation(struct tok_state *tok, số_nguyên col, số_nguyên altcol)
{
    lexer_layout_state *layout = &tok->layout;
    nếu (col == layout->stack[layout->depth].column) {
        nếu (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            trả_về -1;
        }
    }
    không_thì nếu (col > layout->stack[layout->depth].column) {
        nếu (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            trả_về -1;
        }
        nếu (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            trả_về -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    không_thì {
        trong_khi (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        nếu (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            trả_về -1;
        }
        nếu (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            trả_về -1;
        }
    }
    trả_về 0;
}

số_nguyên
_PyLexer_BeginLine(struct tok_state *tok)
{
    assert(tok->layout.at_bol);
    số_nguyên c;
    số_nguyên blankline = 0;
    số_nguyên col = 0;
    số_nguyên altcol = 0;
    tok->layout.at_bol = 0;
    số_nguyên cont_line_col = 0;
    cho (;;) {
        c = tok_nextc(tok);
        nếu (c == ' ') {
            col++, altcol++;
        }
        không_thì nếu (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        không_thì nếu (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        không_thì nếu (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            nếu ((c = _PyLexer_ContinueLine(tok)) == -1) {
                trả_về -1;
            }
        }
        không_thì nếu (c == EOF && PyErr_Occurred()) {
            trả_về -1;
        }
        không_thì {
            break;
        }
    }
    tok_backup(tok, c);
    nếu (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    nếu (c == '#' || c == '\n' || c == '\r') {
        số_nguyên interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        nếu (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        không_thì nếu (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        không_thì {
            blankline = 1; /* Ignore completely */
        }
    }
    nếu (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        nếu (update_indentation(tok, col, altcol) < 0) {
            trả_về -1;
        }
    }
    trả_về blankline;
}

số_nguyên
_PyLexer_IndentationToken(struct tok_state *tok, struct token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    nếu (tok->layout.pending < 0) {
        nếu (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        trả_về _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    không_thì {
        nếu (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        trả_về _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

số_nguyên
_PyLexer_Newline(struct tok_state *tok, struct token *token, số_nguyên blankline)
{
    nếu (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    nếu (blankline || tok->level > 0) {
        nếu (!tok->tok_extra_tokens) {
            trả_về 0;
        }
    }
    không_thì nếu (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        trả_về _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    trả_về _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(struct tok_state *tok)
{
    nếu (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
