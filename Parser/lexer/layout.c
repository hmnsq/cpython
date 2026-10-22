#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

puur_naa_nka
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c = tok_nextc(tok);
    ꦪꦼꦤ꧀ (c == '\r') {
        c = tok_nextc(tok);
    }
    ꦪꦼꦤ꧀ (c != '\n') {
        tok->done = E_LINECONT;
        ꦮꦭꦶ -1;
    }
    c = tok_nextc(tok);
    ꦪꦼꦤ꧀ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ꦪꦼꦤ꧀ (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ꦮꦭꦶ -1;
    } ꦭꦶꦪ {
        tok_backup(tok, c);
    }
    ꦮꦭꦶ c;
}


static puur_naa_nka
update_indentation(sa_nracanaa tok_state *tok, puur_naa_nka col, puur_naa_nka altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ꦪꦼꦤ꧀ (col == layout->stack[layout->depth].column) {
        ꦪꦼꦤ꧀ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ꦮꦭꦶ -1;
        }
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (col > layout->stack[layout->depth].column) {
        ꦪꦼꦤ꧀ (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ꦮꦭꦶ -1;
        }
        ꦪꦼꦤ꧀ (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ꦮꦭꦶ -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    ꦭꦶꦪ {
        ꦱꦼꦢꦼꦭ (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ꦪꦼꦤ꧀ (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ꦮꦭꦶ -1;
        }
        ꦪꦼꦤ꧀ (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ꦮꦭꦶ -1;
        }
    }
    ꦮꦭꦶ 0;
}

puur_naa_nka
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    puur_naa_nka c;
    puur_naa_nka blankline = 0;
    puur_naa_nka col = 0;
    puur_naa_nka altcol = 0;
    tok->layout.at_bol = 0;
    puur_naa_nka cont_line_col = 0;
    ꦏꦁꦒꦺꦴ (;;) {
        c = tok_nextc(tok);
        ꦪꦼꦤ꧀ (c == ' ') {
            col++, altcol++;
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            ꦪꦼꦤ꧀ ((c = _PyLexer_ContinueLine(tok)) == -1) {
                ꦮꦭꦶ -1;
            }
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (c == EOF && PyErr_Occurred()) {
            ꦮꦭꦶ -1;
        }
        ꦭꦶꦪ {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    ꦪꦼꦤ꧀ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ꦪꦼꦤ꧀ (c == '#' || c == '\n' || c == '\r') {
        puur_naa_nka interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ꦪꦼꦤ꧀ (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        ꦭꦶꦪ {
            blankline = 1; /* Ignore completely */
        }
    }
    ꦪꦼꦤ꧀ (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        ꦪꦼꦤ꧀ (update_indentation(tok, col, altcol) < 0) {
            ꦮꦭꦶ -1;
        }
    }
    ꦮꦭꦶ blankline;
}

puur_naa_nka
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ꦪꦼꦤ꧀ (tok->layout.pending < 0) {
        ꦪꦼꦤ꧀ (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ꦮꦭꦶ _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    ꦭꦶꦪ {
        ꦪꦼꦤ꧀ (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ꦮꦭꦶ _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

puur_naa_nka
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka blankline)
{
    ꦪꦼꦤ꧀ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ꦪꦼꦤ꧀ (blankline || tok->level > 0) {
        ꦪꦼꦤ꧀ (!tok->tok_extra_tokens) {
            ꦮꦭꦶ 0;
        }
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ꦮꦭꦶ _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ꦮꦭꦶ _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    ꦪꦼꦤ꧀ (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
