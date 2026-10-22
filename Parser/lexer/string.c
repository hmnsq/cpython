#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static 整数
string_error_token(struct tok_state *tok, struct token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    整数 type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    返回 type;
}

static 整数
incomplete_string_token(struct tok_state *tok, struct token *token,
                        整数 type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    返回 type;
}

整数
_PyLexer_record_ftstring_comment(struct tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    如果 (state->expr_span.end >= 0) {
        返回 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    如果 (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)sizeof(*comments)) /
            (Py_ssize_t)sizeof(*comments->spans);
        如果 (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            返回 -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = sizeof(*comments) +
            (size_t)capacity * sizeof(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        如果 (resized == NULL) {
            PyErr_NoMemory();
            返回 -1;
        }
        comments = resized;
        如果 (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    返回 0;
}

static 整数
finish_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                     struct token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    如果 (state->expr_span.end >= 0) {
        返回 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    整数 tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    如果 (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        返回 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    如果 (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        为 (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            如果 (comment.start >= state->expr_span.end) {
                break;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        char *stripped = PyMem_Malloc((size_t)stripped_size);
        如果 (stripped == NULL) {
            PyErr_NoMemory();
            返回 -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        为 (Py_ssize_t i = 0; i < comment_count; i++) {
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
    否则 {
        Py_ssize_t expr_len;
        const char *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    如果 (!res) {
        返回 -1;
    }
    token->metadata = res;
    返回 0;
}

static 整数
begin_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    如果 (state->comments != NULL) {
        state->comments->count = 0;
    }
    如果 (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        返回 -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    返回 0;
}

整数
_PyLexer_ftstring_punctuation(struct tok_state *tok, ftstring_state *state,
                              struct token *token, 整数 c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    如果 (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        返回 0;
    }
    如果 (c == '!') {
        整数 next = tok_nextc(tok);
        tok_backup(tok, next);
        如果 (next == '=') {
            返回 0;
        }
    }
    如果 (finish_ftstring_expr(tok, state, token) < 0) {
        返回 -1;
    }
    如果 (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        返回 COLON;
    }
    返回 0;
}

整数
_PyLexer_close_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                             整数 c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    整数 depth = _PyLexer_FTStringBracketDepth(tok, state);
    如果 (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        返回 -1;
    }
    如果 (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    返回 0;
}

void
_PyLexer_mark_ftstring_debug(struct tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    如果 (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

整数
_PyLexer_check_string_prefixes(struct tok_state *tok,
                                             整数 saw_b, 整数 saw_r, 整数 saw_u,
                                             整数 saw_f, 整数 saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (整数)(tok->start + 1 - tok->line_start),                 \
            (整数)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        返回 -1;                                                        \
    } 当 (0)

    如果 (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    如果 (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    如果 (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    如果 (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    如果 (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    如果 (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    如果 (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    返回 0;
}

整数
_PyLexer_scan_fstring_start(struct tok_state *tok, struct token *token,
                            整数 c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    整数 quote = c;
    整数 quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    整数 after_quote = tok_nextc(tok);
    如果 (after_quote == quote) {
        整数 after_after_quote = tok_nextc(tok);
        如果 (after_after_quote == quote) {
            quote_size = 3;
        }
        否则 {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    如果 (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    如果 (state == NULL) {
        返回 MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    返回 _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

整数
_PyLexer_scan_string(struct tok_state *tok, struct token *token, 整数 c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    整数 quote = c;
    整数 quote_size = 1;             /* 1 or 3 */
    整数 end_quote_size = 0;
    整数 has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    如果 (c == quote) {
        c = tok_nextc(tok);
        如果 (c == quote) {
            quote_size = 3;
        }
        否则 {
            end_quote_size = 1;     /* empty string found */
        }
    }
    如果 (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    当 (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        如果 (tok->done == E_ERROR) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
        如果 (tok->done == E_DECODE) {
            break;
        }
        如果 (c == EOF || (quote_size == 1 && c == '\n')) {
            如果 (tok_failed(tok)) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
            如果 (c == EOF && tok->allow_incomplete_input) {
                返回 incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            整数 end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const char *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            如果 (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                如果 (state->quote == quote && state->quote_size == quote_size) {
                    整数 level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    整数 lineno = tok->parenlinenostack[level];
                    如果 (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    否则 {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    返回 string_error_token(tok, token, tok->start, location);
                }
            }

            如果 (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                如果 (c != '\n') {
                    tok->done = E_EOFS;
                }
                返回 string_error_token(tok, token, tok->start, location);
            }
            否则 {
                如果 (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } 否则 {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                如果 (c != '\n') {
                    tok->done = E_EOLS;
                }
                返回 string_error_token(tok, token, tok->start, location);
            }
        }
        如果 (c == quote) {
            end_quote_size += 1;
        }
        否则 {
            end_quote_size = 0;
            如果 (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                如果 (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                如果 (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    返回 MAKE_TOKEN(STRING);
}

整数
_PyLexer_get_ftstring(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    整数 token_type;
    整数 end_quote_size = 0;
    整数 unicode_escape = 0;
    整数 quote = current->quote;
    整数 quote_size = current->quote_size;
    整数 in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    整数 raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    当 (end_quote_size != quote_size) {
        整数 c = tok_nextc(tok);
        如果 (tok->done == E_ERROR || tok->done == E_DECODE) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }

        如果 (c == EOF || (quote_size == 1 && c == '\n')) {
            如果 (tok_failed(tok)) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
            如果 (c == EOF && tok->allow_incomplete_input) {
                返回 incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            如果 (in_format_spec && c == '\n') {
                返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            整数 end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const char *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            如果 (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                如果 (c != '\n') {
                    tok->done = E_EOFS;
                }
                返回 string_error_token(tok, token,
                    current->start, location);
            }
            否则 {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                返回 string_error_token(tok, token,
                    current->start, location);
            }
        }

        如果 (c == quote) {
            end_quote_size += 1;
            continue;
        } 否则 {
            end_quote_size = 0;
        }

        如果 (c == '{') {
            整数 peek = tok_nextc(tok);
            如果 (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                如果 (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    返回 MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                如果 (p_start == p_end) {
                    返回 _PyLexer_get_normal(tok, current, token);
                }
            } 否则 {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } 否则 如果 (c == '}') {
            如果 (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            整数 peek = tok_nextc(tok);

            整数 bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            如果 (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            否则 {
                tok_backup(tok, peek);
                如果 (!in_format_spec && bracket_depth == 0) {
                    如果 (tok->start == tok->cur - 1) {
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } 否则 如果 (c == '\\') {
            整数 peek = tok_nextc(tok);
            如果 (peek == '\r') {
                peek = tok_nextc(tok);
            }
            如果 (peek == '{' || peek == '}') {
                如果 (!raw) {
                    如果 (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        返回 MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            如果 (!raw) {
                如果 (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    如果 (peek == '{') {
                        unicode_escape = 1;
                    } 否则 {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    如果 (p_end - quote_size == p_start) {
        整数 end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        返回 MAKE_TOKEN(end_token);
    }
    为 (整数 i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    返回 token_type;
}
