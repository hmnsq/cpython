#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "errcode.h"
#અન્તર્ભાવ "pycore_token.h"

#અન્તર્ભાવ "lexer_internal.h"
#અન્તર્ભાવ "../tokenizer/helpers.h"
#અન્તર્ભાવ "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

પૂર્ણાંક
Py_Lexer_Continue_લીટી(સંરચના tok_state *tok)
{
    પૂર્ણાંક c = tok_nextc(tok);
    જો (c == '\r') {
        c = tok_nextc(tok);
    }
    જો (c != '\n') {
        tok->done = E_LINECONT;
        પાછા -1;
    }
    c = tok_nextc(tok);
    જો (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    જો (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        પાછા -1;
    } નહીંતર {
        tok_backup(tok, c);
    }
    પાછા c;
}


સ્થિર પૂર્ણાંક
update_indentation(સંરચના tok_state *tok, પૂર્ણાંક col, પૂર્ણાંક altcol)
{
    lexer_layout_state *layout = &tok->layout;
    જો (col == layout->ઢગલો[layout->depth].column) {
        જો (altcol != layout->ઢગલો[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            પાછા -1;
        }
    }
    નહીંતર જો (col > layout->ઢગલો[layout->depth].column) {
        જો (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            પાછા -1;
        }
        જો (altcol <= layout->ઢગલો[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            પાછા -1;
        }
        layout->pending++;
        layout->ઢગલો[++layout->depth] = (indentation_level){col, altcol};
    }
    નહીંતર {
        જ્યારે (layout->depth > 0 &&
            col < layout->ઢગલો[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        જો (col != layout->ઢગલો[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            પાછા -1;
        }
        જો (altcol != layout->ઢગલો[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            પાછા -1;
        }
    }
    પાછા 0;
}

પૂર્ણાંક
Py_Lexer_Begin_લીટી(સંરચના tok_state *tok)
{
    પ્રતિજ્ઞા(tok->layout.at_bol);
    પૂર્ણાંક c;
    પૂર્ણાંક blankline = 0;
    પૂર્ણાંક col = 0;
    પૂર્ણાંક altcol = 0;
    tok->layout.at_bol = 0;
    પૂર્ણાંક cont_લીટી_col = 0;
    માટે (;;) {
        c = tok_nextc(tok);
        જો (c == ' ') {
            col++, altcol++;
        }
        નહીંતર જો (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        નહીંતર જો (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        નહીંતર જો (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_લીટી_col = cont_લીટી_col ? cont_લીટી_col : col;
            જો ((c = Py_Lexer_Continue_લીટી(tok)) == -1) {
                પાછા -1;
            }
        }
        નહીંતર જો (c == EOF && PyErr_Occurred()) {
            પાછા -1;
        }
        નહીંતર {
            તોડો;
        }
    }
    tok_backup(tok, c);
    જો (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_લીટી_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    જો (c == '#' || c == '\n' || c == '\r') {
        પૂર્ણાંક interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        જો (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        નહીંતર જો (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        નહીંતર {
            blankline = 1; /* Ignore completely */
        }
    }
    જો (!blankline && tok->level == 0) {
        col = cont_લીટી_col ? cont_લીટી_col : col;
        altcol = cont_લીટી_col ? cont_લીટી_col : altcol;
        જો (update_indentation(tok, col, altcol) < 0) {
            પાછા -1;
        }
    }
    પાછા blankline;
}

પૂર્ણાંક
_PyLexer_IndentationToken(સંરચના tok_state *tok, સંરચના token *token)
{
    પ્રતિજ્ઞા(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    જો (tok->layout.pending < 0) {
        જો (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        પાછા _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    નહીંતર {
        જો (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        પાછા _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

પૂર્ણાંક
_PyLexer_Newline(સંરચના tok_state *tok, સંરચના token *token, પૂર્ણાંક blankline)
{
    જો (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    જો (blankline || tok->level > 0) {
        જો (!tok->tok_extra_tokens) {
            પાછા 0;
        }
    }
    નહીંતર જો (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        પાછા _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    પાછા _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

શૂન્ય
_PyLexer_ImplyDedents(સંરચના tok_state *tok)
{
    જો (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
