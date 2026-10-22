#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ကိန်းပြည့်
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    ကိန်းပြည့် c = tok_nextc(tok);
    အကယ်၍ (c == '\r') {
        c = tok_nextc(tok);
    }
    အကယ်၍ (c != '\n') {
        tok->done = E_LINECONT;
        ပြန်ပေး -1;
    }
    c = tok_nextc(tok);
    အကယ်၍ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    အကယ်၍ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ပြန်ပေး -1;
    } မဟုတ်ပါက {
        tok_backup(tok, c);
    }
    ပြန်ပေး c;
}


static ကိန်းပြည့်
update_indentation(sa_nracanaa tok_state *tok, ကိန်းပြည့် col, ကိန်းပြည့် altcol)
{
    lexer_layout_state *layout = &tok->layout;
    အကယ်၍ (col == layout->stack[layout->depth].column) {
        အကယ်၍ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ပြန်ပေး -1;
        }
    }
    မဟုတ်ပါက အကယ်၍ (col > layout->stack[layout->depth].column) {
        အကယ်၍ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ပြန်ပေး -1;
        }
        အကယ်၍ (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ပြန်ပေး -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    မဟုတ်ပါက {
        နေစဉ် (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        အကယ်၍ (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ပြန်ပေး -1;
        }
        အကယ်၍ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ပြန်ပေး -1;
        }
    }
    ပြန်ပေး 0;
}

ကိန်းပြည့်
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    ကိန်းပြည့် c;
    ကိန်းပြည့် blankline = 0;
    ကိန်းပြည့် col = 0;
    ကိန်းပြည့် altcol = 0;
    tok->layout.at_bol = 0;
    ကိန်းပြည့် cont_line_col = 0;
    အတွက် (;;) {
        c = tok_nextc(tok);
        အကယ်၍ (c == ' ') {
            col++, altcol++;
        }
        မဟုတ်ပါက အကယ်၍ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        မဟုတ်ပါက အကယ်၍ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        မဟုတ်ပါက အကယ်၍ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            အကယ်၍ ((c = _PyLexer_ContinueLine(tok)) == -1) {
                ပြန်ပေး -1;
            }
        }
        မဟုတ်ပါက အကယ်၍ (c == EOF && PyErr_Occurred()) {
            ပြန်ပေး -1;
        }
        မဟုတ်ပါက {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    အကယ်၍ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    အကယ်၍ (c == '#' || c == '\n' || c == '\r') {
        ကိန်းပြည့် interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        အကယ်၍ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        မဟုတ်ပါက အကယ်၍ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        မဟုတ်ပါက {
            blankline = 1; /* Ignore completely */
        }
    }
    အကယ်၍ (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        အကယ်၍ (update_indentation(tok, col, altcol) < 0) {
            ပြန်ပေး -1;
        }
    }
    ပြန်ပေး blankline;
}

ကိန်းပြည့်
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    အကယ်၍ (tok->layout.pending < 0) {
        အကယ်၍ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ပြန်ပေး _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    မဟုတ်ပါက {
        အကယ်၍ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ပြန်ပေး _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ကိန်းပြည့်
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ကိန်းပြည့် blankline)
{
    အကယ်၍ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    အကယ်၍ (blankline || tok->level > 0) {
        အကယ်၍ (!tok->tok_extra_tokens) {
            ပြန်ပေး 0;
        }
    }
    မဟုတ်ပါက အကယ်၍ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ပြန်ပေး _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ပြန်ပေး _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    အကယ်၍ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
