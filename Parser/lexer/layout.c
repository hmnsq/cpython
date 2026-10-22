#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

صحيح
_PyLexer_ContinueLine(بنية tok_state *tok)
{
    صحيح c = tok_nextc(tok);
    אם (c == '\r') {
        c = tok_nextc(tok);
    }
    אם (c != '\n') {
        tok->done = E_LINECONT;
        החזר -1;
    }
    c = tok_nextc(tok);
    אם (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    אם (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        החזר -1;
    } אחרת {
        tok_backup(tok, c);
    }
    החזר c;
}


ساكن صحيح
update_indentation(بنية tok_state *tok, صحيح col, صحيح altcol)
{
    lexer_layout_state *layout = &tok->layout;
    אם (col == layout->stack[layout->depth].column) {
        אם (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            החזר -1;
        }
    }
    אחרת אם (col > layout->stack[layout->depth].column) {
        אם (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            החזר -1;
        }
        אם (altcol <= layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            החזר -1;
        }
        layout->pending++;
        layout->stack[++layout->depth] = (indentation_level){col, altcol};
    }
    אחרת {
        כלעוד (layout->depth > 0 &&
            col < layout->stack[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        אם (col != layout->stack[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            החזר -1;
        }
        אם (altcol != layout->stack[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            החזר -1;
        }
    }
    החזר 0;
}

صحيح
_PyLexer_BeginLine(بنية tok_state *tok)
{
    أكد(tok->layout.at_bol);
    صحيح c;
    صحيح blankline = 0;
    صحيح col = 0;
    صحيح altcol = 0;
    tok->layout.at_bol = 0;
    صحيح cont_line_col = 0;
    עבור (;;) {
        c = tok_nextc(tok);
        אם (c == ' ') {
            col++, altcol++;
        }
        אחרת אם (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        אחרת אם (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        אחרת אם (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_line_col = cont_line_col ? cont_line_col : col;
            אם ((c = _PyLexer_ContinueLine(tok)) == -1) {
                החזר -1;
            }
        }
        אחרת אם (c == EOF && PyErr_Occurred()) {
            החזר -1;
        }
        אחרת {
            اكسر;
        }
    }
    tok_backup(tok, c);
    אם (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_line_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    אם (c == '#' || c == '\n' || c == '\r') {
        صحيح interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        אם (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        אחרת אם (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        אחרת {
            blankline = 1; /* Ignore completely */
        }
    }
    אם (!blankline && tok->level == 0) {
        col = cont_line_col ? cont_line_col : col;
        altcol = cont_line_col ? cont_line_col : altcol;
        אם (update_indentation(tok, col, altcol) < 0) {
            החזר -1;
        }
    }
    החזר blankline;
}

صحيح
_PyLexer_IndentationToken(بنية tok_state *tok, بنية token *token)
{
    أكد(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    אם (tok->layout.pending < 0) {
        אם (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        החזר _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    אחרת {
        אם (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        החזר _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

صحيح
_PyLexer_Newline(بنية tok_state *tok, بنية token *token, صحيح blankline)
{
    אם (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    אם (blankline || tok->level > 0) {
        אם (!tok->tok_extra_tokens) {
            החזר 0;
        }
    }
    אחרת אם (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        החזר _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    החזר _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

فراغ
_PyLexer_ImplyDedents(بنية tok_state *tok)
{
    אם (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
