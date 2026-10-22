#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ຈຳນວນເຕັມ
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    ຈຳນວນເຕັມ c = tok_nextc(tok);
    ຖ້າ (c == '\r') {
        c = tok_nextc(tok);
    }
    ຖ້າ (c != '\n') {
        tok->done = E_LINECONT;
        ສົ່ງຄືນ -1;
    }
    c = tok_nextc(tok);
    ຖ້າ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ຖ້າ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ສົ່ງຄືນ -1;
    } ບໍ່ດັ່ງນັ້ນ {
        tok_backup(tok, c);
    }
    ສົ່ງຄືນ c;
}


static ຈຳນວນເຕັມ
update_indentation(sa_nracanaa tok_state *tok, ຈຳນວນເຕັມ col, ຈຳນວນເຕັມ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ຖ້າ (col == layout->stack[layout->depth].column) {
        ຖ້າ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ສົ່ງຄືນ -1;
        }
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (col > layout->stack[layout->depth].column) {
        ຖ້າ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ສົ່ງຄືນ -1;
        }
        ຖ້າ (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ສົ່ງຄືນ -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    ບໍ່ດັ່ງນັ້ນ {
        ໃນຂະນະທີ່ (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ຖ້າ (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ສົ່ງຄືນ -1;
        }
        ຖ້າ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ສົ່ງຄືນ -1;
        }
    }
    ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    ຈຳນວນເຕັມ c;
    ຈຳນວນເຕັມ blankline = 0;
    ຈຳນວນເຕັມ col = 0;
    ຈຳນວນເຕັມ altcol = 0;
    tok->layout.at_bol = 0;
    ຈຳນວນເຕັມ cont_line_col = 0;
    ສຳລັບ (;;) {
        c = tok_nextc(tok);
        ຖ້າ (c == ' ') {
            col++, altcol++;
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            ຖ້າ ((c = _PyLexer_ContinueLine(tok)) == -1) {
                ສົ່ງຄືນ -1;
            }
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == EOF && PyErr_Occurred()) {
            ສົ່ງຄືນ -1;
        }
        ບໍ່ດັ່ງນັ້ນ {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    ຖ້າ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ຖ້າ (c == '#' || c == '\n' || c == '\r') {
        ຈຳນວນເຕັມ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ຖ້າ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        ບໍ່ດັ່ງນັ້ນ {
            blankline = 1; /* Ignore completely */
        }
    }
    ຖ້າ (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        ຖ້າ (update_indentation(tok, col, altcol) < 0) {
            ສົ່ງຄືນ -1;
        }
    }
    ສົ່ງຄືນ blankline;
}

ຈຳນວນເຕັມ
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ຖ້າ (tok->layout.pending < 0) {
        ຖ້າ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ສົ່ງຄືນ _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    ບໍ່ດັ່ງນັ້ນ {
        ຖ້າ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ສົ່ງຄືນ _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ຈຳນວນເຕັມ
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ຈຳນວນເຕັມ blankline)
{
    ຖ້າ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ຖ້າ (blankline || tok->level > 0) {
        ຖ້າ (!tok->tok_extra_tokens) {
            ສົ່ງຄືນ 0;
        }
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ສົ່ງຄືນ _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ສົ່ງຄືນ _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    ຖ້າ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
