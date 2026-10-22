#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ساكن صحيح
string_error_token(بنية tok_state *tok, بنية token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    صحيح type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ܦܢܐ type;
}

ساكن صحيح
incomplete_string_token(بنية tok_state *tok, بنية token *token,
                        صحيح type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ܦܢܐ type;
}

صحيح
_PyLexer_record_ftstring_comment(بنية tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ܐܢ (state->expr_span.end >= 0) {
        ܦܢܐ 0;
    }
    أكد(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ܐܢ (comments == عدم || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)حجم(*comments)) /
            (Py_ssize_t)حجم(*comments->spans);
        ܐܢ (comments != عدم && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ܦܢܐ -1;
        }
        Py_ssize_t capacity = comments == عدم ? 4 : comments->capacity * 2;
        size_t size = حجم(*comments) +
            (size_t)capacity * حجم(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ܐܢ (resized == عدم) {
            PyErr_NoMemory();
            ܦܢܐ -1;
        }
        comments = resized;
        ܐܢ (state->comments == عدم) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ܦܢܐ 0;
}

ساكن صحيح
finish_ftstring_expr(بنية tok_state *tok, ftstring_state *state,
                     بنية token *token)
{
    أكد(token != عدم && state == _PyLexer_CurrentFTString(tok));
    أكد(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ܐܢ (state->expr_span.end >= 0) {
        ܦܢܐ 0;
    }
    أكد(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    صحيح tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ܐܢ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ܦܢܐ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ܐܢ (comments != عدم && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        لكل (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            أكد(_PyTok_SpanIsValid(comment));
            أكد(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ܐܢ (comment.start >= state->expr_span.end) {
                اكسر;
            }
            أكد(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        حرف *stripped = PyMem_Malloc((size_t)stripped_size);
        ܐܢ (stripped == عدم) {
            PyErr_NoMemory();
            ܦܢܐ -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        لكل (Py_ssize_t i = 0; i < comment_count; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ثابت حرف *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->expr_span.end;
        Py_ssize_t length;
        ثابت حرف *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, عدم);
        PyMem_Free(stripped);
    }
    ܐܚܪܢܐ {
        Py_ssize_t expr_len;
        ثابت حرف *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, عدم);
    }

    ܐܢ (!res) {
        ܦܢܐ -1;
    }
    token->metadata = res;
    ܦܢܐ 0;
}

ساكن صحيح
begin_ftstring_expr(بنية tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    أكد(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ܐܢ (state->comments != عدم) {
        state->comments->count = 0;
    }
    ܐܢ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ܦܢܐ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ܦܢܐ 0;
}

صحيح
_PyLexer_ftstring_punctuation(بنية tok_state *tok, ftstring_state *state,
                              بنية token *token, صحيح c)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    أكد(c == ':' || c == '}' || c == '!');
    ܐܢ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ܦܢܐ 0;
    }
    ܐܢ (c == '!') {
        صحيح next = tok_nextc(tok);
        tok_backup(tok, next);
        ܐܢ (next == '=') {
            ܦܢܐ 0;
        }
    }
    ܐܢ (finish_ftstring_expr(tok, state, token) < 0) {
        ܦܢܐ -1;
    }
    ܐܢ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ܦܢܐ COLON;
    }
    ܦܢܐ 0;
}

صحيح
_PyLexer_close_ftstring_expr(بنية tok_state *tok, ftstring_state *state,
                             صحيح c)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    أكد(c == ')' || c == ']' || c == '}');
    صحيح depth = _PyLexer_FTStringBracketDepth(tok, state);
    ܐܢ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ܦܢܐ -1;
    }
    ܐܢ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ܦܢܐ 0;
}

فراغ
_PyLexer_mark_ftstring_debug(بنية tok_state *tok, ftstring_state *state)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    ܐܢ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

صحيح
_PyLexer_check_string_prefixes(بنية tok_state *tok,
                                             صحيح saw_b, صحيح saw_r, صحيح saw_u,
                                             صحيح saw_f, صحيح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    ܥܒܕ {                                                                  \
        (فراغ)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (صحيح)(tok->start + 1 - tok->line_start),                 \
            (صحيح)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ܦܢܐ -1;                                                        \
    } ܟܡܐ (0)

    ܐܢ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ܐܢ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ܐܢ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ܐܢ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ܐܢ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ܐܢ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ܐܢ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ܦܢܐ 0;
}

صحيح
_PyLexer_scan_fstring_start(بنية tok_state *tok, بنية token *token,
                            صحيح c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    صحيح quote = c;
    صحيح quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحيح after_quote = tok_nextc(tok);
    ܐܢ (after_quote == quote) {
        صحيح after_after_quote = tok_nextc(tok);
        ܐܢ (after_after_quote == quote) {
            quote_size = 3;
        }
        ܐܚܪܢܐ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ܐܢ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ܐܢ (state == عدم) {
        ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ܦܢܐ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

صحيح
_PyLexer_scan_string(بنية tok_state *tok, بنية token *token, صحيح c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    صحيح quote = c;
    صحيح quote_size = 1;             /* 1 or 3 */
    صحيح end_quote_size = 0;
    صحيح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ܐܢ (c == quote) {
        c = tok_nextc(tok);
        ܐܢ (c == quote) {
            quote_size = 3;
        }
        ܐܚܪܢܐ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ܐܢ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ܟܡܐ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ܐܢ (tok->done == E_ERROR) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
        ܐܢ (tok->done == E_DECODE) {
            اكسر;
        }
        ܐܢ (c == EOF || (quote_size == 1 && c == '\n')) {
            ܐܢ (tok_failed(tok)) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
            ܐܢ (c == EOF && tok->allow_incomplete_input) {
                ܦܢܐ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            صحيح end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ثابت حرف *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ܐܢ (state != عدم) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ܐܢ (state->quote == quote && state->quote_size == quote_size) {
                    صحيح level = state->paren_level + state->replacement_depth - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحيح lineno = tok->parenlinenostack[level];
                    ܐܢ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    ܐܚܪܢܐ {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ܦܢܐ string_error_token(tok, token, tok->start, location);
                }
            }

            ܐܢ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ܐܢ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ܦܢܐ string_error_token(tok, token, tok->start, location);
            }
            ܐܚܪܢܐ {
                ܐܢ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } ܐܚܪܢܐ {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ܐܢ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ܦܢܐ string_error_token(tok, token, tok->start, location);
            }
        }
        ܐܢ (c == quote) {
            end_quote_size += 1;
        }
        ܐܚܪܢܐ {
            end_quote_size = 0;
            ܐܢ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ܐܢ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ܐܢ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ܦܢܐ MAKE_TOKEN(STRING);
}

صحيح
_PyLexer_get_ftstring(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    أكد((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    صحيح token_type;
    صحيح end_quote_size = 0;
    صحيح unicode_escape = 0;
    صحيح quote = current->quote;
    صحيح quote_size = current->quote_size;
    صحيح in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    صحيح raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ܟܡܐ (end_quote_size != quote_size) {
        صحيح c = tok_nextc(tok);
        ܐܢ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }

        ܐܢ (c == EOF || (quote_size == 1 && c == '\n')) {
            ܐܢ (tok_failed(tok)) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
            ܐܢ (c == EOF && tok->allow_incomplete_input) {
                ܦܢܐ incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ܐܢ (in_format_spec && c == '\n') {
                ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            صحيح end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ثابت حرف *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ܐܢ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ܐܢ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ܦܢܐ string_error_token(tok, token,
                    current->start, location);
            }
            ܐܚܪܢܐ {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ܦܢܐ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ܐܢ (c == quote) {
            end_quote_size += 1;
            استمر;
        } ܐܚܪܢܐ {
            end_quote_size = 0;
        }

        ܐܢ (c == '{') {
            صحيح peek = tok_nextc(tok);
            ܐܢ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ܐܢ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ܐܢ (p_start == p_end) {
                    ܦܢܐ _PyLexer_get_normal(tok, current, token);
                }
            } ܐܚܪܢܐ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            اذهب emit_middle;
        } ܐܚܪܢܐ ܐܢ (c == '}') {
            ܐܢ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                اذهب emit_middle;
            }
            صحيح peek = tok_nextc(tok);

            صحيح bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ܐܢ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ܐܚܪܢܐ {
                tok_backup(tok, peek);
                ܐܢ (!in_format_spec && bracket_depth == 0) {
                    ܐܢ (tok->start == tok->cur - 1) {
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    اذهب emit_middle;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            اذهب emit_middle;
        } ܐܚܪܢܐ ܐܢ (c == '\\') {
            صحيح peek = tok_nextc(tok);
            ܐܢ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ܐܢ (peek == '{' || peek == '}') {
                ܐܢ (!raw) {
                    ܐܢ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                استمر;
            }

            ܐܢ (!raw) {
                ܐܢ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ܐܢ (peek == '{') {
                        unicode_escape = 1;
                    } ܐܚܪܢܐ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ܐܢ (p_end - quote_size == p_start) {
        صحيح end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ܦܢܐ MAKE_TOKEN(end_token);
    }
    لكل (صحيح i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ܦܢܐ token_type;
}
