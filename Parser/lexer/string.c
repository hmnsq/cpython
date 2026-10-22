#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ціле
string_error_token(struct tok_state *tok, struct token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ціле type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    повернути type;
}

static ціле
incomplete_string_token(struct tok_state *tok, struct token *token,
                        ціле type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    повернути type;
}

ціле
_PyLexer_record_ftstring_comment(struct tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    якщо (state->expr_span.end >= 0) {
        повернути 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    якщо (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)sizeof(*comments)) /
            (Py_ssize_t)sizeof(*comments->spans);
        якщо (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            повернути -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = sizeof(*comments) +
            (size_t)capacity * sizeof(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        якщо (resized == NULL) {
            PyErr_NoMemory();
            повернути -1;
        }
        comments = resized;
        якщо (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    повернути 0;
}

static ціле
finish_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                     struct token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    якщо (state->expr_span.end >= 0) {
        повернути 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    ціле tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    якщо (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        повернути 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    якщо (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        для (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            якщо (comment.start >= state->expr_span.end) {
                break;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        char *stripped = PyMem_Malloc((size_t)stripped_size);
        якщо (stripped == NULL) {
            PyErr_NoMemory();
            повернути -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        для (Py_ssize_t i = 0; i < comment_count; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            const char *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->expr_span.end;
        Py_ssize_t length;
        const char *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, NULL);
        PyMem_Free(stripped);
    }
    інакше {
        Py_ssize_t expr_len;
        const char *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    якщо (!res) {
        повернути -1;
    }
    token->metadata = res;
    повернути 0;
}

static ціле
begin_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    якщо (state->comments != NULL) {
        state->comments->count = 0;
    }
    якщо (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        повернути -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    повернути 0;
}

ціле
_PyLexer_ftstring_punctuation(struct tok_state *tok, ftstring_state *state,
                              struct token *token, ціле c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    якщо (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        повернути 0;
    }
    якщо (c == '!') {
        ціле next = tok_nextc(tok);
        tok_backup(tok, next);
        якщо (next == '=') {
            повернути 0;
        }
    }
    якщо (finish_ftstring_expr(tok, state, token) < 0) {
        повернути -1;
    }
    якщо (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        повернути COLON;
    }
    повернути 0;
}

ціле
_PyLexer_close_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                             ціле c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    ціле depth = _PyLexer_FTStringBracketDepth(tok, state);
    якщо (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        повернути -1;
    }
    якщо (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    повернути 0;
}

void
_PyLexer_mark_ftstring_debug(struct tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    якщо (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

ціле
_PyLexer_check_string_prefixes(struct tok_state *tok,
                                             ціле saw_b, ціле saw_r, ціле saw_u,
                                             ціле saw_f, ціле saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ціле)(tok->start + 1 - tok->line_start),                 \
            (ціле)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        повернути -1;                                                        \
    } поки (0)

    якщо (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    якщо (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    якщо (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    якщо (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    якщо (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    якщо (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    якщо (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    повернути 0;
}

ціле
_PyLexer_scan_fstring_start(struct tok_state *tok, struct token *token,
                            ціле c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ціле quote = c;
    ціле quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ціле after_quote = tok_nextc(tok);
    якщо (after_quote == quote) {
        ціле after_after_quote = tok_nextc(tok);
        якщо (after_after_quote == quote) {
            quote_size = 3;
        }
        інакше {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    якщо (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    якщо (state == NULL) {
        повернути MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    повернути _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ціле
_PyLexer_scan_string(struct tok_state *tok, struct token *token, ціле c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ціле quote = c;
    ціле quote_size = 1;             /* 1 or 3 */
    ціле end_quote_size = 0;
    ціле has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    якщо (c == quote) {
        c = tok_nextc(tok);
        якщо (c == quote) {
            quote_size = 3;
        }
        інакше {
            end_quote_size = 1;     /* empty string found */
        }
    }
    якщо (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    поки (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        якщо (tok->done == E_ERROR) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
        якщо (tok->done == E_DECODE) {
            break;
        }
        якщо (c == EOF || (quote_size == 1 && c == '\n')) {
            якщо (tok_failed(tok)) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
            якщо (c == EOF && tok->allow_incomplete_input) {
                повернути incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ціле end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const char *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            якщо (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                якщо (state->quote == quote && state->quote_size == quote_size) {
                    ціле level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    ціле lineno = tok->parenlinenostack[level];
                    якщо (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    інакше {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    повернути string_error_token(tok, token, tok->start, location);
                }
            }

            якщо (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                якщо (c != '\n') {
                    tok->done = E_EOFS;
                }
                повернути string_error_token(tok, token, tok->start, location);
            }
            інакше {
                якщо (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } інакше {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                якщо (c != '\n') {
                    tok->done = E_EOLS;
                }
                повернути string_error_token(tok, token, tok->start, location);
            }
        }
        якщо (c == quote) {
            end_quote_size += 1;
        }
        інакше {
            end_quote_size = 0;
            якщо (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                якщо (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                якщо (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    повернути MAKE_TOKEN(STRING);
}

ціле
_PyLexer_get_ftstring(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ціле token_type;
    ціле end_quote_size = 0;
    ціле unicode_escape = 0;
    ціле quote = current->quote;
    ціле quote_size = current->quote_size;
    ціле in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ціле raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    поки (end_quote_size != quote_size) {
        ціле c = tok_nextc(tok);
        якщо (tok->done == E_ERROR || tok->done == E_DECODE) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }

        якщо (c == EOF || (quote_size == 1 && c == '\n')) {
            якщо (tok_failed(tok)) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
            якщо (c == EOF && tok->allow_incomplete_input) {
                повернути incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            якщо (in_format_spec && c == '\n') {
                повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ціле end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const char *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            якщо (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                якщо (c != '\n') {
                    tok->done = E_EOFS;
                }
                повернути string_error_token(tok, token,
                    current->start, location);
            }
            інакше {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                повернути string_error_token(tok, token,
                    current->start, location);
            }
        }

        якщо (c == quote) {
            end_quote_size += 1;
            continue;
        } інакше {
            end_quote_size = 0;
        }

        якщо (c == '{') {
            ціле peek = tok_nextc(tok);
            якщо (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                якщо (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    повернути MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                якщо (p_start == p_end) {
                    повернути _PyLexer_get_normal(tok, current, token);
                }
            } інакше {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } інакше якщо (c == '}') {
            якщо (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            ціле peek = tok_nextc(tok);

            ціле bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            якщо (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            інакше {
                tok_backup(tok, peek);
                якщо (!in_format_spec && bracket_depth == 0) {
                    якщо (tok->start == tok->cur - 1) {
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } інакше якщо (c == '\\') {
            ціле peek = tok_nextc(tok);
            якщо (peek == '\r') {
                peek = tok_nextc(tok);
            }
            якщо (peek == '{' || peek == '}') {
                якщо (!raw) {
                    якщо (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        повернути MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            якщо (!raw) {
                якщо (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    якщо (peek == '{') {
                        unicode_escape = 1;
                    } інакше {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    якщо (p_end - quote_size == p_start) {
        ціле end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        повернути MAKE_TOKEN(end_token);
    }
    для (ціле i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    повернути token_type;
}
