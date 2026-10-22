#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "pycore_token.h"

#অন্তর্ভাব "lexer_internal.h"
#অন্তর্ভাব "../tokenizer/helpers.h"
#অন্তর্ভাব "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

পূর্ণসংখ্যা
Py_Lexer_Continue_সারি(কাঠামো tok_state *tok)
{
    পূর্ণসংখ্যা c = tok_nextc(tok);
    যদি (c == '\r') {
        c = tok_nextc(tok);
    }
    যদি (c != '\n') {
        tok->done = E_LINECONT;
        ফেরত -1;
    }
    c = tok_nextc(tok);
    যদি (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    যদি (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ফেরত -1;
    } অন্যথা {
        tok_backup(tok, c);
    }
    ফেরত c;
}


স্থির পূর্ণসংখ্যা
update_indentation(কাঠামো tok_state *tok, পূর্ণসংখ্যা col, পূর্ণসংখ্যা altcol)
{
    lexer_layout_state *layout = &tok->layout;
    যদি (col == layout->স্তূপ[layout->depth].column) {
        যদি (altcol != layout->স্তূপ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ফেরত -1;
        }
    }
    অন্যথা যদি (col > layout->স্তূপ[layout->depth].column) {
        যদি (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ফেরত -1;
        }
        যদি (altcol <= layout->স্তূপ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ফেরত -1;
        }
        layout->pending++;
        layout->স্তূপ[++layout->depth] = (indentation_level){col, altcol};
    }
    অন্যথা {
        যখন (layout->depth > 0 &&
            col < layout->স্তূপ[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        যদি (col != layout->স্তূপ[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ফেরত -1;
        }
        যদি (altcol != layout->স্তূপ[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ফেরত -1;
        }
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
Py_Lexer_Begin_সারি(কাঠামো tok_state *tok)
{
    প্রতিজ্ঞা(tok->layout.at_bol);
    পূর্ণসংখ্যা c;
    পূর্ণসংখ্যা blankline = 0;
    পূর্ণসংখ্যা col = 0;
    পূর্ণসংখ্যা altcol = 0;
    tok->layout.at_bol = 0;
    পূর্ণসংখ্যা cont_সারি_col = 0;
    জন্য (;;) {
        c = tok_nextc(tok);
        যদি (c == ' ') {
            col++, altcol++;
        }
        অন্যথা যদি (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        অন্যথা যদি (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        অন্যথা যদি (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_সারি_col = cont_সারি_col ? cont_সারি_col : col;
            যদি ((c = Py_Lexer_Continue_সারি(tok)) == -1) {
                ফেরত -1;
            }
        }
        অন্যথা যদি (c == EOF && PyErr_Occurred()) {
            ফেরত -1;
        }
        অন্যথা {
            বিরতি;
        }
    }
    tok_backup(tok, c);
    যদি (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_সারি_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    যদি (c == '#' || c == '\n' || c == '\r') {
        পূর্ণসংখ্যা interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        যদি (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        অন্যথা যদি (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        অন্যথা {
            blankline = 1; /* Ignore completely */
        }
    }
    যদি (!blankline && tok->level == 0) {
        col = cont_সারি_col ? cont_সারি_col : col;
        altcol = cont_সারি_col ? cont_সারি_col : altcol;
        যদি (update_indentation(tok, col, altcol) < 0) {
            ফেরত -1;
        }
    }
    ফেরত blankline;
}

পূর্ণসংখ্যা
_PyLexer_IndentationToken(কাঠামো tok_state *tok, কাঠামো token *token)
{
    প্রতিজ্ঞা(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    যদি (tok->layout.pending < 0) {
        যদি (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ফেরত _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    অন্যথা {
        যদি (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ফেরত _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

পূর্ণসংখ্যা
_PyLexer_Newline(কাঠামো tok_state *tok, কাঠামো token *token, পূর্ণসংখ্যা blankline)
{
    যদি (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    যদি (blankline || tok->level > 0) {
        যদি (!tok->tok_extra_tokens) {
            ফেরত 0;
        }
    }
    অন্যথা যদি (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ফেরত _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ফেরত _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

শূন্য
_PyLexer_ImplyDedents(কাঠামো tok_state *tok)
{
    যদি (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
