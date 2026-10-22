#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

#परिभाषय TABSIZE 8
#परिभाषय ALTTABSIZE 1

पूर्णाङ्क
Py_Lexer_Continue_पङ्क्तिः(संरचना tok_state *tok)
{
    पूर्णाङ्क c = tok_nextc(tok);
    यदि (c == '\r') {
        c = tok_nextc(tok);
    }
    यदि (c != '\n') {
        tok->समाप्तम् = E_LINECONT;
        प्रत्यागम -1;
    }
    c = tok_nextc(tok);
    यदि (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    यदि (c == EOF) {
        tok->समाप्तम् = E_EOF;
        tok->cur = tok->inp;
        प्रत्यागम -1;
    } अन्यथा {
        tok_backup(tok, c);
    }
    प्रत्यागम c;
}


स्थिर पूर्णाङ्क
update_indentation(संरचना tok_state *tok, पूर्णाङ्क col, पूर्णाङ्क altcol)
{
    lexer_layout_state *layout = &tok->layout;
    यदि (col == layout->राशिः[layout->गभीरता].column) {
        यदि (altcol != layout->राशिः[layout->गभीरता].alternate_column) {
            _PyTokenizer_indenterror(tok);
            प्रत्यागम -1;
        }
    }
    अन्यथा यदि (col > layout->राशिः[layout->गभीरता].column) {
        यदि (layout->गभीरता + 1 >= MAXINDENT) {
            tok->समाप्तम् = E_TOODEEP;
            tok->cur = tok->inp;
            प्रत्यागम -1;
        }
        यदि (altcol <= layout->राशिः[layout->गभीरता].alternate_column) {
            _PyTokenizer_indenterror(tok);
            प्रत्यागम -1;
        }
        layout->pending++;
        layout->राशिः[++layout->गभीरता] = (indentation_level){col, altcol};
    }
    अन्यथा {
        यावत् (layout->गभीरता > 0 &&
            col < layout->राशिः[layout->गभीरता].column) {
            layout->pending--;
            layout->गभीरता--;
        }
        यदि (col != layout->राशिः[layout->गभीरता].column) {
            tok->समाप्तम् = E_DEDENT;
            tok->cur = tok->inp;
            प्रत्यागम -1;
        }
        यदि (altcol != layout->राशिः[layout->गभीरता].alternate_column) {
            _PyTokenizer_indenterror(tok);
            प्रत्यागम -1;
        }
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
Py_Lexer_Begin_पङ्क्तिः(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->layout.at_bol);
    पूर्णाङ्क c;
    पूर्णाङ्क blankline = 0;
    पूर्णाङ्क col = 0;
    पूर्णाङ्क altcol = 0;
    tok->layout.at_bol = 0;
    पूर्णाङ्क cont_पङ्क्तिः_col = 0;
    प्रति (;;) {
        c = tok_nextc(tok);
        यदि (c == ' ') {
            col++, altcol++;
        }
        अन्यथा यदि (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        अन्यथा यदि (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        अन्यथा यदि (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_पङ्क्तिः_col = cont_पङ्क्तिः_col ? cont_पङ्क्तिः_col : col;
            यदि ((c = Py_Lexer_Continue_पङ्क्तिः(tok)) == -1) {
                प्रत्यागम -1;
            }
        }
        अन्यथा यदि (c == EOF && PyErr_Occurred()) {
            प्रत्यागम -1;
        }
        अन्यथा {
            भङ्ग;
        }
    }
    tok_backup(tok, c);
    यदि (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_पङ्क्तिः_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    यदि (c == '#' || c == '\n' || c == '\r') {
        पूर्णाङ्क interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        यदि (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        अन्यथा यदि (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        अन्यथा {
            blankline = 1; /* Ignore completely */
        }
    }
    यदि (!blankline && tok->level == 0) {
        col = cont_पङ्क्तिः_col ? cont_पङ्क्तिः_col : col;
        altcol = cont_पङ्क्तिः_col ? cont_पङ्क्तिः_col : altcol;
        यदि (update_indentation(tok, col, altcol) < 0) {
            प्रत्यागम -1;
        }
    }
    प्रत्यागम blankline;
}

पूर्णाङ्क
_PyLexer_IndentationToken(संरचना tok_state *tok, संरचना token *token)
{
    प्रतिज्ञा(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_अन्तः = -1;
    यदि (tok->layout.pending < 0) {
        यदि (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_अन्तः = tok->cur;
        }
        tok->layout.pending++;
        प्रत्यागम Py_Lexer_token_सज्जता(tok, token, DEDENT, p_start, p_अन्तः);
    }
    अन्यथा {
        यदि (tok->tok_extra_tokens) {
            p_start = tok->buf_विस्थापनम्;
            p_अन्तः = tok->cur;
        }
        tok->layout.pending--;
        प्रत्यागम Py_Lexer_token_सज्जता(tok, token, INDENT, p_start, p_अन्तः);
    }
}

पूर्णाङ्क
_PyLexer_Newline(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क blankline)
{
    यदि (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    यदि (blankline || tok->level > 0) {
        यदि (!tok->tok_extra_tokens) {
            प्रत्यागम 0;
        }
    }
    अन्यथा यदि (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        प्रत्यागम Py_Lexer_token_सज्जता(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    प्रत्यागम Py_Lexer_token_सज्जता(tok, token, NL, tok->start, tok->cur);
}

शून्य
_PyLexer_ImplyDedents(संरचना tok_state *tok)
{
    यदि (tok->layout.गभीरता != 0) {
        tok->layout.pending = -tok->layout.गभीरता;
        tok->layout.गभीरता = 0;
    }
}
