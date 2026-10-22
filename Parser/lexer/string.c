#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static จำนวนเต็ม
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    จำนวนเต็ม type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    คืน type;
}

static จำนวนเต็ม
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        จำนวนเต็ม type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    คืน type;
}

จำนวนเต็ม
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ถ้า (state->expr_span.end >= 0) {
        คืน 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ถ้า (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        ถ้า (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            คืน -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ถ้า (resized == NULL) {
            PyErr_NoMemory();
            คืน -1;
        }
        comments = resized;
        ถ้า (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    คืน 0;
}

static จำนวนเต็ม
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ถ้า (state->expr_span.end >= 0) {
        คืน 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    จำนวนเต็ม tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ถ้า (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        คืน 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ถ้า (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        สำหรับ (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ถ้า (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        ถ้า (stripped == NULL) {
            PyErr_NoMemory();
            คืน -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        สำหรับ (Py_ssize_t i = 0; i < comment_count; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            const var_na *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->expr_span.end;
        Py_ssize_t length;
        const var_na *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, NULL);
        PyMem_Free(stripped);
    }
    มิฉะนั้น {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    ถ้า (!res) {
        คืน -1;
    }
    token->metadata = res;
    คืน 0;
}

static จำนวนเต็ม
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ถ้า (state->comments != NULL) {
        state->comments->count = 0;
    }
    ถ้า (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        คืน -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    คืน 0;
}

จำนวนเต็ม
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, จำนวนเต็ม c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    ถ้า (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        คืน 0;
    }
    ถ้า (c == '!') {
        จำนวนเต็ม next = tok_nextc(tok);
        tok_backup(tok, next);
        ถ้า (next == '=') {
            คืน 0;
        }
    }
    ถ้า (finish_ftstring_expr(tok, state, token) < 0) {
        คืน -1;
    }
    ถ้า (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        คืน COLON;
    }
    คืน 0;
}

จำนวนเต็ม
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             จำนวนเต็ม c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    จำนวนเต็ม depth = _PyLexer_FTStringBracketDepth(tok, state);
    ถ้า (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        คืน -1;
    }
    ถ้า (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    คืน 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    ถ้า (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

จำนวนเต็ม
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             จำนวนเต็ม saw_b, จำนวนเต็ม saw_r, จำนวนเต็ม saw_u,
                                             จำนวนเต็ม saw_f, จำนวนเต็ม saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (จำนวนเต็ม)(tok->start + 1 - tok->line_start),                 \
            (จำนวนเต็ม)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        คืน -1;                                                        \
    } ขณะ (0)

    ถ้า (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ถ้า (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ถ้า (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ถ้า (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ถ้า (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ถ้า (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ถ้า (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    คืน 0;
}

จำนวนเต็ม
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            จำนวนเต็ม c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    จำนวนเต็ม quote = c;
    จำนวนเต็ม quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    จำนวนเต็ม after_quote = tok_nextc(tok);
    ถ้า (after_quote == quote) {
        จำนวนเต็ม after_after_quote = tok_nextc(tok);
        ถ้า (after_after_quote == quote) {
            quote_size = 3;
        }
        มิฉะนั้น {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ถ้า (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ถ้า (state == NULL) {
        คืน MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    คืน _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

จำนวนเต็ม
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, จำนวนเต็ม c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    จำนวนเต็ม quote = c;
    จำนวนเต็ม quote_size = 1;             /* 1 or 3 */
    จำนวนเต็ม end_quote_size = 0;
    จำนวนเต็ม has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ถ้า (c == quote) {
        c = tok_nextc(tok);
        ถ้า (c == quote) {
            quote_size = 3;
        }
        มิฉะนั้น {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ถ้า (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ขณะ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ถ้า (tok->done == E_ERROR) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
        ถ้า (tok->done == E_DECODE) {
            bha_nga;
        }
        ถ้า (c == EOF || (quote_size == 1 && c == '\n')) {
            ถ้า (tok_failed(tok)) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
            ถ้า (c == EOF && tok->allow_incomplete_input) {
                คืน incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            จำนวนเต็ม end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ถ้า (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ถ้า (state->quote == quote && state->quote_size == quote_size) {
                    จำนวนเต็ม level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    จำนวนเต็ม lineno = tok->parenlinenostack[level];
                    ถ้า (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    มิฉะนั้น {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    คืน string_error_token(tok, token, tok->start, location);
                }
            }

            ถ้า (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ถ้า (c != '\n') {
                    tok->done = E_EOFS;
                }
                คืน string_error_token(tok, token, tok->start, location);
            }
            มิฉะนั้น {
                ถ้า (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } มิฉะนั้น {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ถ้า (c != '\n') {
                    tok->done = E_EOLS;
                }
                คืน string_error_token(tok, token, tok->start, location);
            }
        }
        ถ้า (c == quote) {
            end_quote_size += 1;
        }
        มิฉะนั้น {
            end_quote_size = 0;
            ถ้า (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ถ้า (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ถ้า (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    คืน MAKE_TOKEN(STRING);
}

จำนวนเต็ม
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    จำนวนเต็ม token_type;
    จำนวนเต็ม end_quote_size = 0;
    จำนวนเต็ม unicode_escape = 0;
    จำนวนเต็ม quote = current->quote;
    จำนวนเต็ม quote_size = current->quote_size;
    จำนวนเต็ม in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    จำนวนเต็ม raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ขณะ (end_quote_size != quote_size) {
        จำนวนเต็ม c = tok_nextc(tok);
        ถ้า (tok->done == E_ERROR || tok->done == E_DECODE) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }

        ถ้า (c == EOF || (quote_size == 1 && c == '\n')) {
            ถ้า (tok_failed(tok)) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
            ถ้า (c == EOF && tok->allow_incomplete_input) {
                คืน incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ถ้า (in_format_spec && c == '\n') {
                คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            จำนวนเต็ม end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ถ้า (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ถ้า (c != '\n') {
                    tok->done = E_EOFS;
                }
                คืน string_error_token(tok, token,
                    current->start, location);
            }
            มิฉะนั้น {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                คืน string_error_token(tok, token,
                    current->start, location);
            }
        }

        ถ้า (c == quote) {
            end_quote_size += 1;
            continue;
        } มิฉะนั้น {
            end_quote_size = 0;
        }

        ถ้า (c == '{') {
            จำนวนเต็ม peek = tok_nextc(tok);
            ถ้า (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ถ้า (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    คืน MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ถ้า (p_start == p_end) {
                    คืน _PyLexer_get_normal(tok, current, token);
                }
            } มิฉะนั้น {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } มิฉะนั้น ถ้า (c == '}') {
            ถ้า (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            จำนวนเต็ม peek = tok_nextc(tok);

            จำนวนเต็ม bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ถ้า (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            มิฉะนั้น {
                tok_backup(tok, peek);
                ถ้า (!in_format_spec && bracket_depth == 0) {
                    ถ้า (tok->start == tok->cur - 1) {
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    goto emit_middle;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            goto emit_middle;
        } มิฉะนั้น ถ้า (c == '\\') {
            จำนวนเต็ม peek = tok_nextc(tok);
            ถ้า (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ถ้า (peek == '{' || peek == '}') {
                ถ้า (!raw) {
                    ถ้า (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        คืน MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            ถ้า (!raw) {
                ถ้า (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ถ้า (peek == '{') {
                        unicode_escape = 1;
                    } มิฉะนั้น {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ถ้า (p_end - quote_size == p_start) {
        จำนวนเต็ม end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        คืน MAKE_TOKEN(end_token);
    }
    สำหรับ (จำนวนเต็ม i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    คืน token_type;
}
