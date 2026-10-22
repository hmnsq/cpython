#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

정수
_PyLexer_ContinueLine(struct tok_state *tok)
{
    정수 c = tok_nextc(tok);
    만약 (c == '\r') {
        c = tok_nextc(tok);
    }
    만약 (c != '\n') {
        tok->done = E_LINECONT;
        반환 -1;
    }
    c = tok_nextc(tok);
    만약 (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    만약 (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        반환 -1;
    } 아니면 {
        tok_backup(tok, c);
    }
    반환 c;
}


static 정수
update_indentation(struct tok_state *tok, 정수 col, 정수 altcol)
{
    lexer_layout_state *layout = &tok->layout;
    만약 (col == layout->stack[layout->depth].column) {
        만약 (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            반환 -1;
        }
    }
    아니면 만약 (col > layout->stack[layout->depth].column) {
        만약 (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            반환 -1;
        }
        만약 (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            반환 -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    아니면 {
        동안 (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        만약 (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            반환 -1;
        }
        만약 (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            반환 -1;
        }
    }
    반환 0;
}

정수
_PyLexer_BeginLine(struct tok_state *tok)
{
    assert(tok->layout.at_bol);
    정수 c;
    정수 blankline = 0;
    정수 col = 0;
    정수 altcol = 0;
    tok->layout.at_bol = 0;
    정수 cont_line_col = 0;
    위해 (;;) {
        c = tok_nextc(tok);
        만약 (c == ' ') {
            col++, altcol++;
        }
        아니면 만약 (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        아니면 만약 (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        아니면 만약 (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            만약 ((c = _PyLexer_ContinueLine(tok)) == -1) {
                반환 -1;
            }
        }
        아니면 만약 (c == EOF && PyErr_Occurred()) {
            반환 -1;
        }
        아니면 {
            break;
        }
    }
    tok_backup(tok, c);
    만약 (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    만약 (c == '#' || c == '\n' || c == '\r') {
        정수 interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        만약 (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        아니면 만약 (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        아니면 {
            blankline = 1; /* Ignore completely */
        }
    }
    만약 (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        만약 (update_indentation(tok, col, altcol) < 0) {
            반환 -1;
        }
    }
    반환 blankline;
}

정수
_PyLexer_IndentationToken(struct tok_state *tok, struct token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    만약 (tok->layout.pending < 0) {
        만약 (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        반환 _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    아니면 {
        만약 (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        반환 _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

정수
_PyLexer_Newline(struct tok_state *tok, struct token *token, 정수 blankline)
{
    만약 (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    만약 (blankline || tok->level > 0) {
        만약 (!tok->tok_extra_tokens) {
            반환 0;
        }
    }
    아니면 만약 (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        반환 _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    반환 _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(struct tok_state *tok)
{
    만약 (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
