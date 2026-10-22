#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static цяло
string_error_token(struct tok_state *tok, struct token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    цяло type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    върни type;
}

static цяло
incomplete_string_token(struct tok_state *tok, struct token *token,
                        цяло type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    върни type;
}

цяло
_PyLexer_record_ftstring_comment(struct tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ако (state->expr_span.end >= 0) {
        върни 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ако (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)sizeof(*comments)) /
            (Py_ssize_t)sizeof(*comments->spans);
        ако (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            върни -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = sizeof(*comments) +
            (size_t)capacity * sizeof(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ако (resized == NULL) {
            PyErr_NoMemory();
            върни -1;
        }
        comments = resized;
        ако (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    върни 0;
}

static цяло
finish_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                     struct token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ако (state->expr_span.end >= 0) {
        върни 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    цяло tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ако (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        върни 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ако (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        за (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ако (comment.start >= state->expr_span.end) {
                break;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        char *stripped = PyMem_Malloc((size_t)stripped_size);
        ако (stripped == NULL) {
            PyErr_NoMemory();
            върни -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        за (Py_ssize_t i = 0; i < comment_count; i++) {
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
    иначе {
        Py_ssize_t expr_len;
        const char *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    ако (!res) {
        върни -1;
    }
    token->metadata = res;
    върни 0;
}

static цяло
begin_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ако (state->comments != NULL) {
        state->comments->count = 0;
    }
    ако (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        върни -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    върни 0;
}

цяло
_PyLexer_ftstring_punctuation(struct tok_state *tok, ftstring_state *state,
                              struct token *token, цяло c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    ако (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        върни 0;
    }
    ако (c == '!') {
        цяло next = tok_nextc(tok);
        tok_backup(tok, next);
        ако (next == '=') {
            върни 0;
        }
    }
    ако (finish_ftstring_expr(tok, state, token) < 0) {
        върни -1;
    }
    ако (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        върни COLON;
    }
    върни 0;
}

цяло
_PyLexer_close_ftstring_expr(struct tok_state *tok, ftstring_state *state,
                             цяло c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    цяло depth = _PyLexer_FTStringBracketDepth(tok, state);
    ако (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        върни -1;
    }
    ако (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    върни 0;
}

void
_PyLexer_mark_ftstring_debug(struct tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    ако (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

цяло
_PyLexer_check_string_prefixes(struct tok_state *tok,
                                             цяло saw_b, цяло saw_r, цяло saw_u,
                                             цяло saw_f, цяло saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (цяло)(tok->start + 1 - tok->line_start),                 \
            (цяло)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        върни -1;                                                        \
    } докато (0)

    ако (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ако (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ако (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ако (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ако (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ако (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ако (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    върни 0;
}

цяло
_PyLexer_scan_fstring_start(struct tok_state *tok, struct token *token,
                            цяло c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    цяло quote = c;
    цяло quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    цяло after_quote = tok_nextc(tok);
    ако (after_quote == quote) {
        цяло after_after_quote = tok_nextc(tok);
        ако (after_after_quote == quote) {
            quote_size = 3;
        }
        иначе {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ако (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ако (state == NULL) {
        върни MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    върни _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

цяло
_PyLexer_scan_string(struct tok_state *tok, struct token *token, цяло c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    цяло quote = c;
    цяло quote_size = 1;             /* 1 or 3 */
    цяло end_quote_size = 0;
    цяло has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ако (c == quote) {
        c = tok_nextc(tok);
        ако (c == quote) {
            quote_size = 3;
        }
        иначе {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ако (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    докато (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ако (tok->done == E_ERROR) {
            върни MAKE_TOKEN(ERRORTOKEN);
        }
        ако (tok->done == E_DECODE) {
            break;
        }
        ако (c == EOF || (quote_size == 1 && c == '\n')) {
            ако (tok_failed(tok)) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
            ако (c == EOF && tok->allow_incomplete_input) {
                върни incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            цяло end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const char *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ако (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ако (state->quote == quote && state->quote_size == quote_size) {
                    цяло level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    цяло lineno = tok->parenlinenostack[level];
                    ако (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    иначе {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    върни string_error_token(tok, token, tok->start, location);
                }
            }

            ако (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ако (c != '\n') {
                    tok->done = E_EOFS;
                }
                върни string_error_token(tok, token, tok->start, location);
            }
            иначе {
                ако (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } иначе {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ако (c != '\n') {
                    tok->done = E_EOLS;
                }
                върни string_error_token(tok, token, tok->start, location);
            }
        }
        ако (c == quote) {
            end_quote_size += 1;
        }
        иначе {
            end_quote_size = 0;
            ако (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ако (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ако (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    върни MAKE_TOKEN(STRING);
}

цяло
_PyLexer_get_ftstring(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    цяло token_type;
    цяло end_quote_size = 0;
    цяло unicode_escape = 0;
    цяло quote = current->quote;
    цяло quote_size = current->quote_size;
    цяло in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    цяло raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    докато (end_quote_size != quote_size) {
        цяло c = tok_nextc(tok);
        ако (tok->done == E_ERROR || tok->done == E_DECODE) {
            върни MAKE_TOKEN(ERRORTOKEN);
        }

        ако (c == EOF || (quote_size == 1 && c == '\n')) {
            ако (tok_failed(tok)) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
            ако (c == EOF && tok->allow_incomplete_input) {
                върни incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ако (in_format_spec && c == '\n') {
                върни MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            цяло end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const char *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ако (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ако (c != '\n') {
                    tok->done = E_EOFS;
                }
                върни string_error_token(tok, token,
                    current->start, location);
            }
            иначе {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                върни string_error_token(tok, token,
                    current->start, location);
            }
        }

        ако (c == quote) {
            end_quote_size += 1;
            continue;
        } иначе {
            end_quote_size = 0;
        }

        ако (c == '{') {
            цяло peek = tok_nextc(tok);
            ако (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ако (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    върни MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ако (p_start == p_end) {
                    върни _PyLexer_get_normal(tok, current, token);
                }
            } иначе {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } иначе ако (c == '}') {
            ако (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            цяло peek = tok_nextc(tok);

            цяло bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ако (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            иначе {
                tok_backup(tok, peek);
                ако (!in_format_spec && bracket_depth == 0) {
                    ако (tok->start == tok->cur - 1) {
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } иначе ако (c == '\\') {
            цяло peek = tok_nextc(tok);
            ако (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ако (peek == '{' || peek == '}') {
                ако (!raw) {
                    ако (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        върни MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            ако (!raw) {
                ако (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ако (peek == '{') {
                        unicode_escape = 1;
                    } иначе {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ако (p_end - quote_size == p_start) {
        цяло end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        върни MAKE_TOKEN(end_token);
    }
    за (цяло i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    върни token_type;
}
