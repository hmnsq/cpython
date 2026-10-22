#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "errcode.h"
#அந்தர்பாவ "pycore_token.h"

#அந்தர்பாவ "lexer_internal.h"
#அந்தர்பாவ "../tokenizer/helpers.h"
#அந்தர்பாவ "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

முழு
Py_Lexer_Continue_வரி(கட்டமைப்பு tok_state *tok)
{
    முழு c = tok_nextc(tok);
    எனில் (c == '\r') {
        c = tok_nextc(tok);
    }
    எனில் (c != '\n') {
        tok->done = E_LINECONT;
        திரும்பு -1;
    }
    c = tok_nextc(tok);
    எனில் (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    எனில் (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        திரும்பு -1;
    } இல்லையெனில் {
        tok_backup(tok, c);
    }
    திரும்பு c;
}


நிலையான முழு
update_indentation(கட்டமைப்பு tok_state *tok, முழு col, முழு altcol)
{
    lexer_layout_state *layout = &tok->layout;
    எனில் (col == layout->அடுக்கு[layout->depth].column) {
        எனில் (altcol != layout->அடுக்கு[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            திரும்பு -1;
        }
    }
    இல்லையெனில் எனில் (col > layout->அடுக்கு[layout->depth].column) {
        எனில் (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            திரும்பு -1;
        }
        எனில் (altcol <= layout->அடுக்கு[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            திரும்பு -1;
        }
        layout->pending++;
        layout->அடுக்கு[++layout->depth] = (indentation_level){col, altcol};
    }
    இல்லையெனில் {
        போது (layout->depth > 0 &&
            col < layout->அடுக்கு[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        எனில் (col != layout->அடுக்கு[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            திரும்பு -1;
        }
        எனில் (altcol != layout->அடுக்கு[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            திரும்பு -1;
        }
    }
    திரும்பு 0;
}

முழு
Py_Lexer_Begin_வரி(கட்டமைப்பு tok_state *tok)
{
    ப்ரதிஜ்ஞா(tok->layout.at_bol);
    முழு c;
    முழு blankline = 0;
    முழு col = 0;
    முழு altcol = 0;
    tok->layout.at_bol = 0;
    முழு cont_வரி_col = 0;
    க்கு (;;) {
        c = tok_nextc(tok);
        எனில் (c == ' ') {
            col++, altcol++;
        }
        இல்லையெனில் எனில் (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        இல்லையெனில் எனில் (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        இல்லையெனில் எனில் (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_வரி_col = cont_வரி_col ? cont_வரி_col : col;
            எனில் ((c = Py_Lexer_Continue_வரி(tok)) == -1) {
                திரும்பு -1;
            }
        }
        இல்லையெனில் எனில் (c == EOF && PyErr_Occurred()) {
            திரும்பு -1;
        }
        இல்லையெனில் {
            நிறுத்து;
        }
    }
    tok_backup(tok, c);
    எனில் (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_வரி_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    எனில் (c == '#' || c == '\n' || c == '\r') {
        முழு interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        எனில் (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        இல்லையெனில் எனில் (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        இல்லையெனில் {
            blankline = 1; /* Ignore completely */
        }
    }
    எனில் (!blankline && tok->level == 0) {
        col = cont_வரி_col ? cont_வரி_col : col;
        altcol = cont_வரி_col ? cont_வரி_col : altcol;
        எனில் (update_indentation(tok, col, altcol) < 0) {
            திரும்பு -1;
        }
    }
    திரும்பு blankline;
}

முழு
_PyLexer_IndentationToken(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token)
{
    ப்ரதிஜ்ஞா(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    எனில் (tok->layout.pending < 0) {
        எனில் (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        திரும்பு _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    இல்லையெனில் {
        எனில் (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        திரும்பு _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

முழு
_PyLexer_Newline(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token, முழு blankline)
{
    எனில் (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    எனில் (blankline || tok->level > 0) {
        எனில் (!tok->tok_extra_tokens) {
            திரும்பு 0;
        }
    }
    இல்லையெனில் எனில் (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        திரும்பு _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    திரும்பு _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

வெறுமை
_PyLexer_ImplyDedents(கட்டமைப்பு tok_state *tok)
{
    எனில் (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
