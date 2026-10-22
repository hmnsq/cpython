#شامل "Python.h"
#شامل "pycore_token.h"
#شامل "errcode.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ایستا صحیح
string_error_token(ساختار tok_state *tok, ساختار token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    صحیح type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    بازگشت type;
}

ایستا صحیح
incomplete_string_token(ساختار tok_state *tok, ساختار token *token,
                        صحیح type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    بازگشت type;
}

صحیح
_PyLexer_record_ftstring_comment(ساختار tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    اگر (state->عبارت_span.end >= 0) {
        بازگشت 0;
    }
    أكد(state->عبارت_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    اگر (comments == پوچ || comments->شمار == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)اندازه(*comments)) /
            (Py_ssize_t)اندازه(*comments->spans);
        اگر (comments != پوچ && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            بازگشت -1;
        }
        Py_ssize_t capacity = comments == پوچ ? 4 : comments->capacity * 2;
        size_t size = اندازه(*comments) +
            (size_t)capacity * اندازه(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        اگر (resized == پوچ) {
            PyErr_NoMemory();
            بازگشت -1;
        }
        comments = resized;
        اگر (state->comments == پوچ) {
            comments->شمار = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->شمار++] =
        (_PyTok_Span){start, end};
    بازگشت 0;
}

ایستا صحیح
finish_ftstring_عبارت(ساختار tok_state *tok, ftstring_state *state,
                     ساختار token *token)
{
    أكد(token != پوچ && state == _PyLexer_CurrentFTString(tok));
    أكد(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    اگر (state->عبارت_span.end >= 0) {
        بازگشت 0;
    }
    أكد(state->عبارت_span.start >= 0);
    state->عبارت_span.end = tok->start;
    صحیح tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    اگر (!(state->debug_عبارت || tstring_interpolation) || token->metadata) {
        بازگشت 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    اگر (comments != پوچ && comments->شمار > 0) {
        Py_ssize_t stripped_size =
            state->عبارت_span.end - state->عبارت_span.start;
        Py_ssize_t comment_شمار = 0;
        برای (Py_ssize_t i = 0; i < comments->شمار; i++) {
            _PyTok_Span comment = comments->spans[i];
            أكد(_PyTok_SpanIsValid(comment));
            أكد(comment.start >= (i == 0 ? state->عبارت_span.start
                                           : comments->spans[i - 1].end));
            اگر (comment.start >= state->عبارت_span.end) {
                بشکن;
            }
            أكد(comment.end <= state->عبارت_span.end);
            stripped_size -= comment.end - comment.start;
            comment_شمار++;
        }
        نویسه *stripped = PyMem_Malloc((size_t)stripped_size);
        اگر (stripped == پوچ) {
            PyErr_NoMemory();
            بازگشت -1;
        }
        _PyTok_Span kept = {state->عبارت_span.start, state->عبارت_span.start};
        Py_ssize_t stripped_len = 0;
        برای (Py_ssize_t i = 0; i < comment_شمار; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ثابت نویسه *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->عبارت_span.end;
        Py_ssize_t length;
        ثابت نویسه *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, پوچ);
        PyMem_Free(stripped);
    }
    وگرنه {
        Py_ssize_t عبارت_len;
        ثابت نویسه *عبارت = _PyTok_SourceSpanView(
            &tok->source, state->عبارت_span, &عبارت_len);
        res = PyUnicode_DecodeUTF8(عبارت, عبارت_len, پوچ);
    }

    اگر (!res) {
        بازگشت -1;
    }
    token->metadata = res;
    بازگشت 0;
}

ایستا صحیح
begin_ftstring_عبارت(ساختار tok_state *tok, ftstring_state *state,
                    _PyTok_Off عبارت_start)
{
    أكد(state->mode != FTSTRING_MODE_EXPRESSION);
    state->عبارت_span = (_PyTok_Span){عبارت_start, -1};
    اگر (state->comments != پوچ) {
        state->comments->شمار = 0;
    }
    اگر (state->replacement_depth >= MAX_عبارت_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        بازگشت -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_عبارت = 0;
    بازگشت 0;
}

صحیح
_PyLexer_ftstring_punctuation(ساختار tok_state *tok, ftstring_state *state,
                              ساختار token *token, صحیح c)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    أكد(c == ':' || c == '}' || c == '!');
    اگر (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        بازگشت 0;
    }
    اگر (c == '!') {
        صحیح next = tok_nextc(tok);
        tok_backup(tok, next);
        اگر (next == '=') {
            بازگشت 0;
        }
    }
    اگر (finish_ftstring_عبارت(tok, state, token) < 0) {
        بازگشت -1;
    }
    اگر (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        بازگشت COLON;
    }
    بازگشت 0;
}

صحیح
Py_Lexer_close_ftstring_عبارت(ساختار tok_state *tok, ftstring_state *state,
                             صحیح c)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    أكد(c == ')' || c == ']' || c == '}');
    صحیح depth = _PyLexer_FTStringBracketDepth(tok, state);
    اگر (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        بازگشت -1;
    }
    اگر (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_میانه;
        state->debug_عبارت = 0;
    }
    بازگشت 0;
}

تهی
_PyLexer_mark_ftstring_debug(ساختار tok_state *tok, ftstring_state *state)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    اگر (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_عبارت = 1;
    }
}

صحیح
_PyLexer_check_string_prefixes(ساختار tok_state *tok,
                                             صحیح saw_b, صحیح saw_r, صحیح saw_u,
                                             صحیح saw_f, صحیح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    انجامبده {                                                                  \
        (تهی)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (صحیح)(tok->start + 1 - tok->سطر_start),                 \
            (صحیح)(tok->cur - tok->سطر_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        بازگشت -1;                                                        \
    } وقتی (0)

    اگر (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    اگر (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    اگر (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    اگر (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    اگر (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    اگر (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    اگر (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    بازگشت 0;
}

صحیح
_PyLexer_scan_fstring_start(ساختار tok_state *tok, ساختار token *token,
                            صحیح c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    صحیح quote = c;
    صحیح quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحیح after_quote = tok_nextc(tok);
    اگر (after_quote == quote) {
        صحیح after_after_quote = tok_nextc(tok);
        اگر (after_after_quote == quote) {
            quote_size = 3;
        }
        وگرنه {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    اگر (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_افزودن_FTString(tok);
    اگر (state == پوچ) {
        بازگشت MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_میانه;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->عبارت_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    بازگشت _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

صحیح
_PyLexer_scan_string(ساختار tok_state *tok, ساختار token *token, صحیح c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    صحیح quote = c;
    صحیح quote_size = 1;             /* 1 or 3 */
    صحیح end_quote_size = 0;
    صحیح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    اگر (c == quote) {
        c = tok_nextc(tok);
        اگر (c == quote) {
            quote_size = 3;
        }
        وگرنه {
            end_quote_size = 1;     /* empty string found */
        }
    }
    اگر (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    وقتی (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        اگر (tok->done == E_ERROR) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
        اگر (tok->done == E_DECODE) {
            بشکن;
        }
        اگر (c == EOF || (quote_size == 1 && c == '\n')) {
            اگر (tok_failed(tok)) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
            اگر (c == EOF && tok->allow_incomplete_درون_داد) {
                بازگشت incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            صحیح end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ثابت نویسه *سطر = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            اگر (state != پوچ) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                اگر (state->quote == quote && state->quote_size == quote_size) {
                    صحیح level = state->paren_level + state->replacement_depth - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحیح lineno = tok->parenlinenostack[level];
                    اگر (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    وگرنه {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    بازگشت string_error_token(tok, token, tok->start, location);
                }
            }

            اگر (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                اگر (c != '\n') {
                    tok->done = E_EOFS;
                }
                بازگشت string_error_token(tok, token, tok->start, location);
            }
            وگرنه {
                اگر (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } وگرنه {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                اگر (c != '\n') {
                    tok->done = E_EOLS;
                }
                بازگشت string_error_token(tok, token, tok->start, location);
            }
        }
        اگر (c == quote) {
            end_quote_size += 1;
        }
        وگرنه {
            end_quote_size = 0;
            اگر (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                اگر (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                اگر (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    بازگشت MAKE_TOKEN(STRING);
}

صحیح
_PyLexer_get_ftstring(ساختار tok_state *tok, ftstring_state *current, ساختار token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    أكد((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_عبارت_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    صحیح token_type;
    صحیح end_quote_size = 0;
    صحیح unicode_escape = 0;
    صحیح quote = current->quote;
    صحیح quote_size = current->quote_size;
    صحیح in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    صحیح raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    وقتی (end_quote_size != quote_size) {
        صحیح c = tok_nextc(tok);
        اگر (tok->done == E_ERROR || tok->done == E_DECODE) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }

        اگر (c == EOF || (quote_size == 1 && c == '\n')) {
            اگر (tok_failed(tok)) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
            اگر (c == EOF && tok->allow_incomplete_درون_داد) {
                بازگشت incomplete_string_token(tok, token,
                    FTSTRING_میانه(current), current->start, current->start_loc);
            }

            اگر (in_format_spec && c == '\n') {
                بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            صحیح end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ثابت نویسه *سطر = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            اگر (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                اگر (c != '\n') {
                    tok->done = E_EOFS;
                }
                بازگشت string_error_token(tok, token,
                    current->start, location);
            }
            وگرنه {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                بازگشت string_error_token(tok, token,
                    current->start, location);
            }
        }

        اگر (c == quote) {
            end_quote_size += 1;
            ادامه;
        } وگرنه {
            end_quote_size = 0;
        }

        اگر (c == '{') {
            صحیح peek = tok_nextc(tok);
            اگر (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off عبارت_start = tok->cur;
                tok_backup(tok, c);
                اگر (begin_ftstring_عبارت(tok, current, عبارت_start) < 0) {
                    بازگشت MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                اگر (p_start == p_end) {
                    بازگشت _PyLexer_get_normal(tok, current, token);
                }
            } وگرنه {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            برو emit_میانه;
        } وگرنه اگر (c == '}') {
            اگر (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                برو emit_میانه;
            }
            صحیح peek = tok_nextc(tok);

            صحیح bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            اگر (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            وگرنه {
                tok_backup(tok, peek);
                اگر (!in_format_spec && bracket_depth == 0) {
                    اگر (tok->start == tok->cur - 1) {
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    برو emit_میانه;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            برو emit_میانه;
        } وگرنه اگر (c == '\\') {
            صحیح peek = tok_nextc(tok);
            اگر (peek == '\r') {
                peek = tok_nextc(tok);
            }
            اگر (peek == '{' || peek == '}') {
                اگر (!raw) {
                    اگر (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        بازگشت MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                ادامه;
            }

            اگر (!raw) {
                اگر (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    اگر (peek == '{') {
                        unicode_escape = 1;
                    } وگرنه {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    اگر (p_end - quote_size == p_start) {
        صحیح end_token = FTSTRING_END(current);
        Py_Lexer_برداشت_FTString(tok);
        بازگشت MAKE_TOKEN(end_token);
    }
    برای (صحیح i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_میانه:
    token_type = MAKE_TOKEN(FTSTRING_میانه(current));
    token->is_raw = raw;
    بازگشت token_type;
}
