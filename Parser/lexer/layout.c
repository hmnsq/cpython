#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

පූර්ණ_සංඛ්‍යා
_PyLexer_ContinueLine(sa_nracanaa tok_state *tok)
{
    පූර්ණ_සංඛ්‍යා c = tok_nextc(tok);
    නම් (c == '\r') {
        c = tok_nextc(tok);
    }
    නම් (c != '\n') {
        tok->done = E_LINECONT;
        ආපසු -1;
    }
    c = tok_nextc(tok);
    නම් (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    නම් (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ආපසු -1;
    } නැතහොත් {
        tok_backup(tok, c);
    }
    ආපසු c;
}


static පූර්ණ_සංඛ්‍යා
update_indentation(sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා altcol)
{
    lexer_layout_state *layout = &tok->layout;
    නම් (col == layout->stack[layout->depth].column) {
        නම් (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ආපසු -1;
        }
    }
    නැතහොත් නම් (col > layout->stack[layout->depth].column) {
        නම් (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ආපසු -1;
        }
        නම් (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ආපසු -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    නැතහොත් {
        අතරතුර (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        නම් (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ආපසු -1;
        }
        නම් (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ආපසු -1;
        }
    }
    ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_BeginLine(sa_nracanaa tok_state *tok)
{
    assert(tok->layout.at_bol);
    පූර්ණ_සංඛ්‍යා c;
    පූර්ණ_සංඛ්‍යා blankline = 0;
    පූර්ණ_සංඛ්‍යා col = 0;
    පූර්ණ_සංඛ්‍යා altcol = 0;
    tok->layout.at_bol = 0;
    පූර්ණ_සංඛ්‍යා cont_line_col = 0;
    සඳහා (;;) {
        c = tok_nextc(tok);
        නම් (c == ' ') {
            col++, altcol++;
        }
        නැතහොත් නම් (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        නැතහොත් නම් (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        නැතහොත් නම් (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            නම් ((c = _PyLexer_ContinueLine(tok)) == -1) {
                ආපසු -1;
            }
        }
        නැතහොත් නම් (c == EOF && PyErr_Occurred()) {
            ආපසු -1;
        }
        නැතහොත් {
            bha_nga;
        }
    }
    tok_backup(tok, c);
    නම් (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    නම් (c == '#' || c == '\n' || c == '\r') {
        පූර්ණ_සංඛ්‍යා interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        නම් (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        නැතහොත් නම් (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        නැතහොත් {
            blankline = 1; /* Ignore completely */
        }
    }
    නම් (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        නම් (update_indentation(tok, col, altcol) < 0) {
            ආපසු -1;
        }
    }
    ආපසු blankline;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_IndentationToken(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    assert(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    නම් (tok->layout.pending < 0) {
        නම් (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ආපසු _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    නැතහොත් {
        නම් (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ආපසු _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

පූර්ණ_සංඛ්‍යා
_PyLexer_Newline(sa_nracanaa tok_state *tok, sa_nracanaa token *token, පූර්ණ_සංඛ්‍යා blankline)
{
    නම් (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    නම් (blankline || tok->level > 0) {
        නම් (!tok->tok_extra_tokens) {
            ආපසු 0;
        }
    }
    නැතහොත් නම් (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ආපසු _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ආපසු _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

void
_PyLexer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    නම් (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
