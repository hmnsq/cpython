#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ចំនួនគត់
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    ចំនួនគត់ c = tok_nextc(tok);
    ប្រសិនបើ (c == '\r') {
        c = tok_nextc(tok);
    }
    ប្រសិនបើ (c != '\n') {
        tok->done = E_LINECONT;
        ត្រឡប់ -1;
    }
    c = tok_nextc(tok);
    ប្រសិនបើ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ប្រសិនបើ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ត្រឡប់ -1;
    } បើមិនដូច្នេះ {
        tok_backup(tok, c);
    }
    ត្រឡប់ c;
}


static ចំនួនគត់
update_indentation(sa_nracanaa tok_state *tok, ចំនួនគត់ col, ចំនួនគត់ altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ប្រសិនបើ (col == layout->stack[layout->depth].column) {
        ប្រសិនបើ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ត្រឡប់ -1;
        }
    }
    បើមិនដូច្នេះ ប្រសិនបើ (col > layout->stack[layout->depth].column) {
        ប្រសិនបើ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ត្រឡប់ -1;
        }
        ប្រសិនបើ (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ត្រឡប់ -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    បើមិនដូច្នេះ {
        ខណៈពេល (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ប្រសិនបើ (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ត្រឡប់ -1;
        }
        ប្រសិនបើ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ត្រឡប់ -1;
        }
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    ចំនួនគត់ c;
    ចំនួនគត់ blankline = 0;
    ចំនួនគត់ col = 0;
    ចំនួនគត់ altcol = 0;
    tok->layout.at_bol = 0;
    ចំនួនគត់ cont_line_col = 0;
    សម្រាប់ (;;) {
        c = tok_nextc(tok);
        ប្រសិនបើ (c == ' ') {
            col++, altcol++;
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            ប្រសិនបើ ((c = _PyLexer_ContinueLine(tok)) == -1) {
                ត្រឡប់ -1;
            }
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == EOF && PyErr_Occurred()) {
            ត្រឡប់ -1;
        }
        បើមិនដូច្នេះ {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    ប្រសិនបើ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ប្រសិនបើ (c == '#' || c == '\n' || c == '\r') {
        ចំនួនគត់ interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ប្រសិនបើ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        បើមិនដូច្នេះ ប្រសិនបើ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        បើមិនដូច្នេះ {
            blankline = 1; /* Ignore completely */
        }
    }
    ប្រសិនបើ (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        ប្រសិនបើ (update_indentation(tok, col, altcol) < 0) {
            ត្រឡប់ -1;
        }
    }
    ត្រឡប់ blankline;
}

ចំនួនគត់
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ប្រសិនបើ (tok->layout.pending < 0) {
        ប្រសិនបើ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ត្រឡប់ _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    បើមិនដូច្នេះ {
        ប្រសិនបើ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ត្រឡប់ _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ចំនួនគត់
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ចំនួនគត់ blankline)
{
    ប្រសិនបើ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ប្រសិនបើ (blankline || tok->level > 0) {
        ប្រសិនបើ (!tok->tok_extra_tokens) {
            ត្រឡប់ 0;
        }
    }
    បើមិនដូច្នេះ ប្រសិនបើ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ត្រឡប់ _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ត្រឡប់ _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    ប្រសិនបើ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
