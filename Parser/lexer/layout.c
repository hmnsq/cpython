#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

จำนวนเต็ม
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    จำนวนเต็ม c = tok_nextc(tok);
    ถ้า (c == '\r') {
        c = tok_nextc(tok);
    }
    ถ้า (c != '\n') {
        tok->done = E_LINECONT;
        คืน -1;
    }
    c = tok_nextc(tok);
    ถ้า (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ถ้า (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        คืน -1;
    } มิฉะนั้น {
        tok_backup(tok, c);
    }
    คืน c;
}


static จำนวนเต็ม
update_indentation(sa_nracanaa tok_state *tok, จำนวนเต็ม col, จำนวนเต็ม altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ถ้า (col == layout->stack[layout->depth].column) {
        ถ้า (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            คืน -1;
        }
    }
    มิฉะนั้น ถ้า (col > layout->stack[layout->depth].column) {
        ถ้า (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            คืน -1;
        }
        ถ้า (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            คืน -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    มิฉะนั้น {
        ขณะ (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ถ้า (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            คืน -1;
        }
        ถ้า (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            คืน -1;
        }
    }
    คืน 0;
}

จำนวนเต็ม
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    จำนวนเต็ม c;
    จำนวนเต็ม blankline = 0;
    จำนวนเต็ม col = 0;
    จำนวนเต็ม altcol = 0;
    tok->layout.at_bol = 0;
    จำนวนเต็ม cont_line_col = 0;
    สำหรับ (;;) {
        c = tok_nextc(tok);
        ถ้า (c == ' ') {
            col++, altcol++;
        }
        มิฉะนั้น ถ้า (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        มิฉะนั้น ถ้า (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        มิฉะนั้น ถ้า (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            ถ้า ((c = _PyLexer_ContinueLine(tok)) == -1) {
                คืน -1;
            }
        }
        มิฉะนั้น ถ้า (c == EOF && PyErr_Occurred()) {
            คืน -1;
        }
        มิฉะนั้น {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    ถ้า (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ถ้า (c == '#' || c == '\n' || c == '\r') {
        จำนวนเต็ม interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ถ้า (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        มิฉะนั้น ถ้า (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        มิฉะนั้น {
            blankline = 1; /* Ignore completely */
        }
    }
    ถ้า (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        ถ้า (update_indentation(tok, col, altcol) < 0) {
            คืน -1;
        }
    }
    คืน blankline;
}

จำนวนเต็ม
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ถ้า (tok->layout.pending < 0) {
        ถ้า (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        คืน _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    มิฉะนั้น {
        ถ้า (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        คืน _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

จำนวนเต็ม
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, จำนวนเต็ม blankline)
{
    ถ้า (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ถ้า (blankline || tok->level > 0) {
        ถ้า (!tok->tok_extra_tokens) {
            คืน 0;
        }
    }
    มิฉะนั้น ถ้า (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        คืน _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    คืน _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    ถ้า (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
