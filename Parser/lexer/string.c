#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"

#ଅନ୍ତର୍ଭାଵ "lexer_internal.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
string_error_token(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ଫେରନ୍ତୁ type;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
incomplete_string_token(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ଫେରନ୍ତୁ type;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_record_ftstring_comment(ସଂରଚନା tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    ପ୍ରତିଜ୍ଞା(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ଯଦି (state->ପଦସମଷ୍ଟି_span.end >= 0) {
        ଫେରନ୍ତୁ 0;
    }
    ପ୍ରତିଜ୍ଞା(state->ପଦସମଷ୍ଟି_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ଯଦି (comments == ଖାଲି || comments->ଗଣନା_2 == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)ଆକାର(*comments)) /
            (Py_ssize_t)ଆକାର(*comments->spans);
        ଯଦି (comments != ଖାଲି && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ଫେରନ୍ତୁ -1;
        }
        Py_ssize_t capacity = comments == ଖାଲି ? 4 : comments->capacity * 2;
        size_t size = ଆକାର(*comments) +
            (size_t)capacity * ଆକାର(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ଯଦି (resized == ଖାଲି) {
            PyErr_NoMemory();
            ଫେରନ୍ତୁ -1;
        }
        comments = resized;
        ଯଦି (state->comments == ଖାଲି) {
            comments->ଗଣନା_2 = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->ଗଣନା_2++] =
        (_PyTok_Span){start, end};
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
finish_ftstring_ପଦସମଷ୍ଟି(ସଂରଚନା tok_state *tok, ftstring_state *state,
                     ସଂରଚନା token *token)
{
    ପ୍ରତିଜ୍ଞା(token != ଖାଲି && state == _PyLexer_CurrentFTString(tok));
    ପ୍ରତିଜ୍ଞା(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ଯଦି (state->ପଦସମଷ୍ଟି_span.end >= 0) {
        ଫେରନ୍ତୁ 0;
    }
    ପ୍ରତିଜ୍ଞା(state->ପଦସମଷ୍ଟି_span.start >= 0);
    state->ପଦସମଷ୍ଟି_span.end = tok->start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ଯଦି (!(state->debug_ପଦସମଷ୍ଟି || tstring_interpolation) || token->metadata) {
        ଫେରନ୍ତୁ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ଯଦି (comments != ଖାଲି && comments->ଗଣନା_2 > 0) {
        Py_ssize_t stripped_size =
            state->ପଦସମଷ୍ଟି_span.end - state->ପଦସମଷ୍ଟି_span.start;
        Py_ssize_t comment_ଗଣନା = 0;
        ପାଇଁ (Py_ssize_t i = 0; i < comments->ଗଣନା_2; i++) {
            _PyTok_Span comment = comments->spans[i];
            ପ୍ରତିଜ୍ଞା(_PyTok_SpanIsValid(comment));
            ପ୍ରତିଜ୍ଞା(comment.start >= (i == 0 ? state->ପଦସମଷ୍ଟି_span.start
                                           : comments->spans[i - 1].end));
            ଯଦି (comment.start >= state->ପଦସମଷ୍ଟି_span.end) {
                ଭାଙ୍ଗ;
            }
            ପ୍ରତିଜ୍ଞା(comment.end <= state->ପଦସମଷ୍ଟି_span.end);
            stripped_size -= comment.end - comment.start;
            comment_ଗଣନା++;
        }
        ଅକ୍ଷର *stripped = PyMem_Malloc((size_t)stripped_size);
        ଯଦି (stripped == ଖାଲି) {
            PyErr_NoMemory();
            ଫେରନ୍ତୁ -1;
        }
        _PyTok_Span kept = {state->ପଦସମଷ୍ଟି_span.start, state->ପଦସମଷ୍ଟି_span.start};
        Py_ssize_t stripped_len = 0;
        ପାଇଁ (Py_ssize_t i = 0; i < comment_ଗଣନା; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ଅଚଳ ଅକ୍ଷର *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->ପଦସମଷ୍ଟି_span.end;
        Py_ssize_t length;
        ଅଚଳ ଅକ୍ଷର *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, ଖାଲି);
        PyMem_Free(stripped);
    }
    ଅନ୍ୟଥା {
        Py_ssize_t ପଦସମଷ୍ଟି_len;
        ଅଚଳ ଅକ୍ଷର *ପଦସମଷ୍ଟି = _PyTok_SourceSpanView(
            &tok->source, state->ପଦସମଷ୍ଟି_span, &ପଦସମଷ୍ଟି_len);
        res = PyUnicode_DecodeUTF8(ପଦସମଷ୍ଟି, ପଦସମଷ୍ଟି_len, ଖାଲି);
    }

    ଯଦି (!res) {
        ଫେରନ୍ତୁ -1;
    }
    token->metadata = res;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
begin_ftstring_ପଦସମଷ୍ଟି(ସଂରଚନା tok_state *tok, ftstring_state *state,
                    _PyTok_Off ପଦସମଷ୍ଟି_start)
{
    ପ୍ରତିଜ୍ଞା(state->mode != FTSTRING_MODE_EXPRESSION);
    state->ପଦସମଷ୍ଟି_span = (_PyTok_Span){ପଦସମଷ୍ଟି_start, -1};
    ଯଦି (state->comments != ଖାଲି) {
        state->comments->ଗଣନା_2 = 0;
    }
    ଯଦି (state->replacement_depth >= MAX_ପଦସମଷ୍ଟି_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ଫେରନ୍ତୁ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_ପଦସମଷ୍ଟି = 0;
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_ftstring_punctuation(ସଂରଚନା tok_state *tok, ftstring_state *state,
                              ସଂରଚନା token *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପ୍ରତିଜ୍ଞା(state->mode == FTSTRING_MODE_EXPRESSION);
    ପ୍ରତିଜ୍ଞା(c == ':' || c == '}' || c == '!');
    ଯଦି (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (c == '!') {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା next = tok_nextc(tok);
        tok_backup(tok, next);
        ଯଦି (next == '=') {
            ଫେରନ୍ତୁ 0;
        }
    }
    ଯଦି (finish_ftstring_ପଦସମଷ୍ଟି(tok, state, token) < 0) {
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ଫେରନ୍ତୁ COLON;
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Lexer_close_ftstring_ପଦସମଷ୍ଟି(ସଂରଚନା tok_state *tok, ftstring_state *state,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପ୍ରତିଜ୍ଞା(state->mode == FTSTRING_MODE_EXPRESSION);
    ପ୍ରତିଜ୍ଞା(c == ')' || c == ']' || c == '}');
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = _PyLexer_FTStringBracketDepth(tok, state);
    ଯଦି (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_ମଧ୍ୟ;
        state->debug_ପଦସମଷ୍ଟି = 0;
    }
    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ
_PyLexer_mark_ftstring_debug(ସଂରଚନା tok_state *tok, ftstring_state *state)
{
    ପ୍ରତିଜ୍ଞା(state->mode == FTSTRING_MODE_EXPRESSION);
    ଯଦି (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_ପଦସମଷ୍ଟି = 1;
    }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_check_string_prefixes(ସଂରଚନା tok_state *tok,
                                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_u,
                                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_f, ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    କର {                                                                  \
        (ଶୂନ୍ୟ)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tok->start + 1 - tok->ଧାଡ଼ି_start),                 \
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tok->cur - tok->ଧାଡ଼ି_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ଫେରନ୍ତୁ -1;                                                        \
    } ଯେତେବେଳେ (0)

    ଯଦି (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ଯଦି (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ଯଦି (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ଯଦି (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ଯଦି (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ଯଦି (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ଯଦି (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_scan_fstring_start(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote = c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା after_quote = tok_nextc(tok);
    ଯଦି (after_quote == quote) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା after_after_quote = tok_nextc(tok);
        ଯଦି (after_after_quote == quote) {
            quote_size = 3;
        }
        ଅନ୍ୟଥା {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ଯଦି (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_ଠେଲ_FTString(tok);
    ଯଦି (state == ଖାଲି) {
        ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_ମଧ୍ୟ;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->ପଦସମଷ୍ଟି_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ଫେରନ୍ତୁ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_scan_string(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote = c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote_size = 1;             /* 1 or 3 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_quote_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ଯଦି (c == quote) {
        c = tok_nextc(tok);
        ଯଦି (c == quote) {
            quote_size = 3;
        }
        ଅନ୍ୟଥା {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ଯଦି (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ଯେତେବେଳେ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ଯଦି (tok->done == E_ERROR) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
        ଯଦି (tok->done == E_DECODE) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (c == EOF || (quote_size == 1 && c == '\n')) {
            ଯଦି (tok_failed(tok)) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
            ଯଦି (c == EOF && tok->allow_incomplete_ନିବେଶ) {
                ଫେରନ୍ତୁ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ଅଚଳ ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ଯଦି (state != ଖାଲି) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ଯଦି (state->quote == quote && state->quote_size == quote_size) {
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level = state->paren_level + state->replacement_depth - 1;
                    ପ୍ରତିଜ୍ଞା(level >= 0 && level < tok->level);
                    ପ୍ରତିଜ୍ଞା(tok->parenstack[level] == '{');
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno = tok->parenlinenostack[level];
                    ଯଦି (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    ଅନ୍ୟଥା {
                        _PyTokenizer_syntaxerror_at(
                            tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ଫେରନ୍ତୁ string_error_token(tok, token, tok->start, location);
                }
            }

            ଯଦି (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ଯଦି (c != '\n') {
                    tok->done = E_EOFS;
                }
                ଫେରନ୍ତୁ string_error_token(tok, token, tok->start, location);
            }
            ଅନ୍ୟଥା {
                ଯଦି (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } ଅନ୍ୟଥା {
                    _PyTokenizer_syntaxerror_at(
                        tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ଯଦି (c != '\n') {
                    tok->done = E_EOLS;
                }
                ଫେରନ୍ତୁ string_error_token(tok, token, tok->start, location);
            }
        }
        ଯଦି (c == quote) {
            end_quote_size += 1;
        }
        ଅନ୍ୟଥା {
            end_quote_size = 0;
            ଯଦି (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ଯଦି (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ଯଦି (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ଫେରନ୍ତୁ MAKE_TOKEN(STRING);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_get_ftstring(ସଂରଚନା tok_state *tok, ftstring_state *current, ସଂରଚନା token *token)
{
    ପ୍ରତିଜ୍ଞା(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    ପ୍ରତିଜ୍ଞା((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_ପଦସମଷ୍ଟି_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା token_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_quote_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା unicode_escape = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote = current->quote;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote_size = current->quote_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ଯେତେବେଳେ (end_quote_size != quote_size) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = tok_nextc(tok);
        ଯଦି (tok->done == E_ERROR || tok->done == E_DECODE) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }

        ଯଦି (c == EOF || (quote_size == 1 && c == '\n')) {
            ଯଦି (tok_failed(tok)) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
            ଯଦି (c == EOF && tok->allow_incomplete_ନିବେଶ) {
                ଫେରନ୍ତୁ incomplete_string_token(tok, token,
                    FTSTRING_ମଧ୍ୟ(current), current->start, current->start_loc);
            }

            ଯଦି (in_format_spec && c == '\n') {
                ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ଯଦି (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ଯଦି (c != '\n') {
                    tok->done = E_EOFS;
                }
                ଫେରନ୍ତୁ string_error_token(tok, token,
                    current->start, location);
            }
            ଅନ୍ୟଥା {
                _PyTokenizer_syntaxerror_at(
                    tok, ଧାଡ଼ି, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ଫେରନ୍ତୁ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ଯଦି (c == quote) {
            end_quote_size += 1;
            ଜାରି;
        } ଅନ୍ୟଥା {
            end_quote_size = 0;
        }

        ଯଦି (c == '{') {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା peek = tok_nextc(tok);
            ଯଦି (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off ପଦସମଷ୍ଟି_start = tok->cur;
                tok_backup(tok, c);
                ଯଦି (begin_ftstring_ପଦସମଷ୍ଟି(tok, current, ପଦସମଷ୍ଟି_start) < 0) {
                    ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ଯଦି (p_start == p_end) {
                    ଫେରନ୍ତୁ _PyLexer_get_normal(tok, current, token);
                }
            } ଅନ୍ୟଥା {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ଯାଅ emit_ମଧ୍ୟ;
        } ଅନ୍ୟଥା ଯଦି (c == '}') {
            ଯଦି (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                ଯାଅ emit_ମଧ୍ୟ;
            }
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା peek = tok_nextc(tok);

            ପୂର୍ଣ୍ଣସଂଖ୍ୟା bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ଯଦି (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ଅନ୍ୟଥା {
                tok_backup(tok, peek);
                ଯଦି (!in_format_spec && bracket_depth == 0) {
                    ଯଦି (tok->start == tok->cur - 1) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ଯାଅ emit_ମଧ୍ୟ;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            ଯାଅ emit_ମଧ୍ୟ;
        } ଅନ୍ୟଥା ଯଦି (c == '\\') {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା peek = tok_nextc(tok);
            ଯଦି (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ଯଦି (peek == '{' || peek == '}') {
                ଯଦି (!raw) {
                    ଯଦି (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                ଜାରି;
            }

            ଯଦି (!raw) {
                ଯଦି (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ଯଦି (peek == '{') {
                        unicode_escape = 1;
                    } ଅନ୍ୟଥା {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ଯଦି (p_end - quote_size == p_start) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_token = FTSTRING_END(current);
        Py_Lexer_କାଢ଼_FTString(tok);
        ଫେରନ୍ତୁ MAKE_TOKEN(end_token);
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_ମଧ୍ୟ:
    token_type = MAKE_TOKEN(FTSTRING_ମଧ୍ୟ(current));
    token->is_raw = raw;
    ଫେରନ୍ତୁ token_type;
}
