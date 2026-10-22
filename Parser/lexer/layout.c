#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

целое
_PyLexer_ContinueLine(struct tok_state *tok)
{
    целое c = tok_nextc(tok);
    если (c == '\r') {
        c = tok_nextc(tok);
    }
    если (c != '\n') {
        tok->done = E_LINECONT;
        вернуть -1;
    }
    c = tok_nextc(tok);
    если (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    если (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        вернуть -1;
    } иначе {
        tok_backup(tok, c);
    }
    вернуть c;
}


static целое
update_indentation(struct tok_state *tok, целое col, целое altcol)
{
    lexer_layout_state *layout = &tok->layout;
    если (col == layout->stack[layout->depth].column) {
        если (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            вернуть -1;
        }
    }
    иначе если (col > layout->stack[layout->depth].column) {
        если (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            вернуть -1;
        }
        если (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            вернуть -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    иначе {
        пока (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        если (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            вернуть -1;
        }
        если (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            вернуть -1;
        }
    }
    вернуть 0;
}

целое
_PyLexer_BeginLine(struct tok_state *tok)
{
    assert(tok->layout.at_bol);
    целое c;
    целое blankline = 0;
    целое col = 0;
    целое altcol = 0;
    tok->layout.at_bol = 0;
    целое cont_line_col = 0;
    для (;;) {
        c = tok_nextc(tok);
        если (c == ' ') {
            col++, altcol++;
        }
        иначе если (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        иначе если (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        иначе если (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            если ((c = _PyLexer_ContinueLine(tok)) == -1) {
                вернуть -1;
            }
        }
        иначе если (c == EOF && PyErr_Occurred()) {
            вернуть -1;
        }
        иначе {
            break;
        }
    }
    tok_backup(tok, c);
    если (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    если (c == '#' || c == '\n' || c == '\r') {
        целое interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        если (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        иначе если (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        иначе {
            blankline = 1; /* Ignore completely */
        }
    }
    если (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        если (update_indentation(tok, col, altcol) < 0) {
            вернуть -1;
        }
    }
    вернуть blankline;
}

целое
_PyLexer_IndentationToken(struct tok_state *tok, struct token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    если (tok->layout.pending < 0) {
        если (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        вернуть _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    иначе {
        если (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        вернуть _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

целое
_PyLexer_Newline(struct tok_state *tok, struct token *token, целое blankline)
{
    если (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    если (blankline || tok->level > 0) {
        если (!tok->tok_extra_tokens) {
            вернуть 0;
        }
    }
    иначе если (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        вернуть _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    вернуть _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(struct tok_state *tok)
{
    если (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
