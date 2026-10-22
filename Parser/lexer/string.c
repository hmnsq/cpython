#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
string_error_token(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ಪೂರ್ಣಾಙ್ಕ type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ಪ್ರತ್ಯಾಗಮ type;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
incomplete_string_token(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token,
                        ಪೂರ್ಣಾಙ್ಕ type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ಪ್ರತ್ಯಾಗಮ type;
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_record_ftstring_comment(ಸಂರಚನಾ tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    ಪ್ರತಿಜ್ಞಾ(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ಯದಿ (state->ಅಭಿವ್ಯಕ್ತಿ_span.end >= 0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತಿಜ್ಞಾ(state->ಅಭಿವ್ಯಕ್ತಿ_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ಯದಿ (comments == ರಿಕ್ತ || comments->ಎಣಿಕೆ == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)ಪರಿಮಾಣ(*comments)) /
            (Py_ssize_t)ಪರಿಮಾಣ(*comments->spans);
        ಯದಿ (comments != ರಿಕ್ತ && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        Py_ssize_t capacity = comments == ರಿಕ್ತ ? 4 : comments->capacity * 2;
        size_t size = ಪರಿಮಾಣ(*comments) +
            (size_t)capacity * ಪರಿಮಾಣ(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ಯದಿ (resized == ರಿಕ್ತ) {
            PyErr_NoMemory();
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        comments = resized;
        ಯದಿ (state->comments == ರಿಕ್ತ) {
            comments->ಎಣಿಕೆ = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->ಎಣಿಕೆ++] =
        (_PyTok_Span){start, end};
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
finish_ftstring_ಅಭಿವ್ಯಕ್ತಿ(ಸಂರಚನಾ tok_state *tok, ftstring_state *state,
                     ಸಂರಚನಾ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(token != ರಿಕ್ತ && state == _PyLexer_CurrentFTString(tok));
    ಪ್ರತಿಜ್ಞಾ(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ಯದಿ (state->ಅಭಿವ್ಯಕ್ತಿ_span.end >= 0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತಿಜ್ಞಾ(state->ಅಭಿವ್ಯಕ್ತಿ_span.start >= 0);
    state->ಅಭಿವ್ಯಕ್ತಿ_span.end = tok->start;
    ಪೂರ್ಣಾಙ್ಕ tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ಯದಿ (!(state->debug_ಅಭಿವ್ಯಕ್ತಿ || tstring_interpolation) || token->metadata) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ಯದಿ (comments != ರಿಕ್ತ && comments->ಎಣಿಕೆ > 0) {
        Py_ssize_t stripped_size =
            state->ಅಭಿವ್ಯಕ್ತಿ_span.end - state->ಅಭಿವ್ಯಕ್ತಿ_span.start;
        Py_ssize_t comment_ಎಣಿಕೆ = 0;
        ಪ್ರತಿ (Py_ssize_t i = 0; i < comments->ಎಣಿಕೆ; i++) {
            _PyTok_Span comment = comments->spans[i];
            ಪ್ರತಿಜ್ಞಾ(_PyTok_SpanIsValid(comment));
            ಪ್ರತಿಜ್ಞಾ(comment.start >= (i == 0 ? state->ಅಭಿವ್ಯಕ್ತಿ_span.start
                                           : comments->spans[i - 1].end));
            ಯದಿ (comment.start >= state->ಅಭಿವ್ಯಕ್ತಿ_span.end) {
                ಭಙ್ಗ;
            }
            ಪ್ರತಿಜ್ಞಾ(comment.end <= state->ಅಭಿವ್ಯಕ್ತಿ_span.end);
            stripped_size -= comment.end - comment.start;
            comment_ಎಣಿಕೆ++;
        }
        ವರ್ಣ *stripped = PyMem_Malloc((size_t)stripped_size);
        ಯದಿ (stripped == ರಿಕ್ತ) {
            PyErr_NoMemory();
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        _PyTok_Span kept = {state->ಅಭಿವ್ಯಕ್ತಿ_span.start, state->ಅಭಿವ್ಯಕ್ತಿ_span.start};
        Py_ssize_t stripped_len = 0;
        ಪ್ರತಿ (Py_ssize_t i = 0; i < comment_ಎಣಿಕೆ; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ಧ್ರುವ ವರ್ಣ *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->ಅಭಿವ್ಯಕ್ತಿ_span.end;
        Py_ssize_t length;
        ಧ್ರುವ ವರ್ಣ *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, ರಿಕ್ತ);
        PyMem_Free(stripped);
    }
    ಅನ್ಯಥಾ {
        Py_ssize_t ಅಭಿವ್ಯಕ್ತಿ_len;
        ಧ್ರುವ ವರ್ಣ *ಅಭಿವ್ಯಕ್ತಿ = _PyTok_SourceSpanView(
            &tok->source, state->ಅಭಿವ್ಯಕ್ತಿ_span, &ಅಭಿವ್ಯಕ್ತಿ_len);
        res = PyUnicode_DecodeUTF8(ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ_len, ರಿಕ್ತ);
    }

    ಯದಿ (!res) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    token->metadata = res;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
begin_ftstring_ಅಭಿವ್ಯಕ್ತಿ(ಸಂರಚನಾ tok_state *tok, ftstring_state *state,
                    _PyTok_Off ಅಭಿವ್ಯಕ್ತಿ_start)
{
    ಪ್ರತಿಜ್ಞಾ(state->mode != FTSTRING_MODE_EXPRESSION);
    state->ಅಭಿವ್ಯಕ್ತಿ_span = (_PyTok_Span){ಅಭಿವ್ಯಕ್ತಿ_start, -1};
    ಯದಿ (state->comments != ರಿಕ್ತ) {
        state->comments->ಎಣಿಕೆ = 0;
    }
    ಯದಿ (state->replacement_depth >= MAX_ಅಭಿವ್ಯಕ್ತಿ_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_ಅಭಿವ್ಯಕ್ತಿ = 0;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_ftstring_punctuation(ಸಂರಚನಾ tok_state *tok, ftstring_state *state,
                              ಸಂರಚನಾ token *token, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತಿಜ್ಞಾ(state->mode == FTSTRING_MODE_EXPRESSION);
    ಪ್ರತಿಜ್ಞಾ(c == ':' || c == '}' || c == '!');
    ಯದಿ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (c == '!') {
        ಪೂರ್ಣಾಙ್ಕ next = tok_nextc(tok);
        tok_backup(tok, next);
        ಯದಿ (next == '=') {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಯದಿ (finish_ftstring_ಅಭಿವ್ಯಕ್ತಿ(tok, state, token) < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ಪ್ರತ್ಯಾಗಮ COLON;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
Py_Lexer_close_ftstring_ಅಭಿವ್ಯಕ್ತಿ(ಸಂರಚನಾ tok_state *tok, ftstring_state *state,
                             ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತಿಜ್ಞಾ(state->mode == FTSTRING_MODE_EXPRESSION);
    ಪ್ರತಿಜ್ಞಾ(c == ')' || c == ']' || c == '}');
    ಪೂರ್ಣಾಙ್ಕ depth = _PyLexer_FTStringBracketDepth(tok, state);
    ಯದಿ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_ಮಧ್ಯ;
        state->debug_ಅಭಿವ್ಯಕ್ತಿ = 0;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ
_PyLexer_mark_ftstring_debug(ಸಂರಚನಾ tok_state *tok, ftstring_state *state)
{
    ಪ್ರತಿಜ್ಞಾ(state->mode == FTSTRING_MODE_EXPRESSION);
    ಯದಿ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_ಅಭಿವ್ಯಕ್ತಿ = 1;
    }
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_check_string_prefixes(ಸಂರಚನಾ tok_state *tok,
                                             ಪೂರ್ಣಾಙ್ಕ saw_b, ಪೂರ್ಣಾಙ್ಕ saw_r, ಪೂರ್ಣಾಙ್ಕ saw_u,
                                             ಪೂರ್ಣಾಙ್ಕ saw_f, ಪೂರ್ಣಾಙ್ಕ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    ಕುರು {                                                                  \
        (ಶೂನ್ಯ)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ಪೂರ್ಣಾಙ್ಕ)(tok->start + 1 - tok->ಸಾಲು_start),                 \
            (ಪೂರ್ಣಾಙ್ಕ)(tok->cur - tok->ಸಾಲು_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ಪ್ರತ್ಯಾಗಮ -1;                                                        \
    } ಯಾವತ್ (0)

    ಯದಿ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ಯದಿ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ಯದಿ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ಯದಿ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ಯದಿ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ಯದಿ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ಯದಿ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_scan_fstring_start(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token,
                            ಪೂರ್ಣಾಙ್ಕ c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ಪೂರ್ಣಾಙ್ಕ quote = c;
    ಪೂರ್ಣಾಙ್ಕ quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ಪೂರ್ಣಾಙ್ಕ after_quote = tok_nextc(tok);
    ಯದಿ (after_quote == quote) {
        ಪೂರ್ಣಾಙ್ಕ after_after_quote = tok_nextc(tok);
        ಯದಿ (after_after_quote == quote) {
            quote_size = 3;
        }
        ಅನ್ಯಥಾ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ಯದಿ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_ತಳ್ಳು_FTString(tok);
    ಯದಿ (state == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_ಮಧ್ಯ;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->ಅಭಿವ್ಯಕ್ತಿ_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ಪ್ರತ್ಯಾಗಮ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_scan_string(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token, ಪೂರ್ಣಾಙ್ಕ c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ಪೂರ್ಣಾಙ್ಕ quote = c;
    ಪೂರ್ಣಾಙ್ಕ quote_size = 1;             /* 1 or 3 */
    ಪೂರ್ಣಾಙ್ಕ end_quote_size = 0;
    ಪೂರ್ಣಾಙ್ಕ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ಯದಿ (c == quote) {
        c = tok_nextc(tok);
        ಯದಿ (c == quote) {
            quote_size = 3;
        }
        ಅನ್ಯಥಾ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ಯದಿ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ಯಾವತ್ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ಯದಿ (tok->done == E_ERROR) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
        ಯದಿ (tok->done == E_DECODE) {
            ಭಙ್ಗ;
        }
        ಯದಿ (c == EOF || (quote_size == 1 && c == '\n')) {
            ಯದಿ (tok_failed(tok)) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
            ಯದಿ (c == EOF && tok->allow_incomplete_ಒಳಹರಿವು) {
                ಪ್ರತ್ಯಾಗಮ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ಪೂರ್ಣಾಙ್ಕ end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ಧ್ರುವ ವರ್ಣ *ಸಾಲು = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ಧ್ರುವ ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ಯದಿ (state != ರಿಕ್ತ) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ಯದಿ (state->quote == quote && state->quote_size == quote_size) {
                    ಪೂರ್ಣಾಙ್ಕ level = state->paren_level + state->replacement_depth - 1;
                    ಪ್ರತಿಜ್ಞಾ(level >= 0 && level < tok->level);
                    ಪ್ರತಿಜ್ಞಾ(tok->parenstack[level] == '{');
                    ಪೂರ್ಣಾಙ್ಕ lineno = tok->parenlinenostack[level];
                    ಯದಿ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    ಅನ್ಯಥಾ {
                        _PyTokenizer_syntaxerror_at(
                            tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ಪ್ರತ್ಯಾಗಮ string_error_token(tok, token, tok->start, location);
                }
            }

            ಯದಿ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ಯದಿ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ಪ್ರತ್ಯಾಗಮ string_error_token(tok, token, tok->start, location);
            }
            ಅನ್ಯಥಾ {
                ಯದಿ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } ಅನ್ಯಥಾ {
                    _PyTokenizer_syntaxerror_at(
                        tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ಯದಿ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ಪ್ರತ್ಯಾಗಮ string_error_token(tok, token, tok->start, location);
            }
        }
        ಯದಿ (c == quote) {
            end_quote_size += 1;
        }
        ಅನ್ಯಥಾ {
            end_quote_size = 0;
            ಯದಿ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ಯದಿ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ಯದಿ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(STRING);
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_get_ftstring(ಸಂರಚನಾ tok_state *tok, ftstring_state *current, ಸಂರಚನಾ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    ಪ್ರತಿಜ್ಞಾ((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_ಅಭಿವ್ಯಕ್ತಿ_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ಪೂರ್ಣಾಙ್ಕ token_type;
    ಪೂರ್ಣಾಙ್ಕ end_quote_size = 0;
    ಪೂರ್ಣಾಙ್ಕ unicode_escape = 0;
    ಪೂರ್ಣಾಙ್ಕ quote = current->quote;
    ಪೂರ್ಣಾಙ್ಕ quote_size = current->quote_size;
    ಪೂರ್ಣಾಙ್ಕ in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ಪೂರ್ಣಾಙ್ಕ raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ಯಾವತ್ (end_quote_size != quote_size) {
        ಪೂರ್ಣಾಙ್ಕ c = tok_nextc(tok);
        ಯದಿ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }

        ಯದಿ (c == EOF || (quote_size == 1 && c == '\n')) {
            ಯದಿ (tok_failed(tok)) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
            ಯದಿ (c == EOF && tok->allow_incomplete_ಒಳಹರಿವು) {
                ಪ್ರತ್ಯಾಗಮ incomplete_string_token(tok, token,
                    FTSTRING_ಮಧ್ಯ(current), current->start, current->start_loc);
            }

            ಯದಿ (in_format_spec && c == '\n') {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ಪೂರ್ಣಾಙ್ಕ end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ಧ್ರುವ ವರ್ಣ *ಸಾಲು = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ಯದಿ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ಯದಿ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ಪ್ರತ್ಯಾಗಮ string_error_token(tok, token,
                    current->start, location);
            }
            ಅನ್ಯಥಾ {
                _PyTokenizer_syntaxerror_at(
                    tok, ಸಾಲು, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ಪ್ರತ್ಯಾಗಮ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ಯದಿ (c == quote) {
            end_quote_size += 1;
            ಅನುವರ್ತ;
        } ಅನ್ಯಥಾ {
            end_quote_size = 0;
        }

        ಯದಿ (c == '{') {
            ಪೂರ್ಣಾಙ್ಕ peek = tok_nextc(tok);
            ಯದಿ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off ಅಭಿವ್ಯಕ್ತಿ_start = tok->cur;
                tok_backup(tok, c);
                ಯದಿ (begin_ftstring_ಅಭಿವ್ಯಕ್ತಿ(tok, current, ಅಭಿವ್ಯಕ್ತಿ_start) < 0) {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ಯದಿ (p_start == p_end) {
                    ಪ್ರತ್ಯಾಗಮ _PyLexer_get_normal(tok, current, token);
                }
            } ಅನ್ಯಥಾ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ಗಚ್ಛ emit_ಮಧ್ಯ;
        } ಅನ್ಯಥಾ ಯದಿ (c == '}') {
            ಯದಿ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                ಗಚ್ಛ emit_ಮಧ್ಯ;
            }
            ಪೂರ್ಣಾಙ್ಕ peek = tok_nextc(tok);

            ಪೂರ್ಣಾಙ್ಕ bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ಯದಿ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ಅನ್ಯಥಾ {
                tok_backup(tok, peek);
                ಯದಿ (!in_format_spec && bracket_depth == 0) {
                    ಯದಿ (tok->start == tok->cur - 1) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ಗಚ್ಛ emit_ಮಧ್ಯ;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            ಗಚ್ಛ emit_ಮಧ್ಯ;
        } ಅನ್ಯಥಾ ಯದಿ (c == '\\') {
            ಪೂರ್ಣಾಙ್ಕ peek = tok_nextc(tok);
            ಯದಿ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ಯದಿ (peek == '{' || peek == '}') {
                ಯದಿ (!raw) {
                    ಯದಿ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                ಅನುವರ್ತ;
            }

            ಯದಿ (!raw) {
                ಯದಿ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ಯದಿ (peek == '{') {
                        unicode_escape = 1;
                    } ಅನ್ಯಥಾ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ಯದಿ (p_end - quote_size == p_start) {
        ಪೂರ್ಣಾಙ್ಕ end_token = FTSTRING_END(current);
        Py_Lexer_ತೆಗೆ_FTString(tok);
        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(end_token);
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_ಮಧ್ಯ:
    token_type = MAKE_TOKEN(FTSTRING_ಮಧ್ಯ(current));
    token->is_raw = raw;
    ಪ್ರತ್ಯಾಗಮ token_type;
}
