#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ཧྲིལ
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ཧྲིལ type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ཕྱིར type;
}

static ཧྲིལ
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        ཧྲིལ type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ཕྱིར type;
}

ཧྲིལ
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    གལ་ཏེ (state->expr_span.end >= 0) {
        ཕྱིར 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    གལ་ཏེ (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        གལ་ཏེ (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ཕྱིར -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        གལ་ཏེ (resized == NULL) {
            PyErr_NoMemory();
            ཕྱིར -1;
        }
        comments = resized;
        གལ་ཏེ (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ཕྱིར 0;
}

static ཧྲིལ
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    གལ་ཏེ (state->expr_span.end >= 0) {
        ཕྱིར 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    ཧྲིལ tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    གལ་ཏེ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ཕྱིར 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    གལ་ཏེ (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        ཆེད་དུ (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            གལ་ཏེ (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        གལ་ཏེ (stripped == NULL) {
            PyErr_NoMemory();
            ཕྱིར -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        ཆེད་དུ (Py_ssize_t i = 0; i < comment_count; i++) {
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
    དེ་མིན {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    གལ་ཏེ (!res) {
        ཕྱིར -1;
    }
    token->metadata = res;
    ཕྱིར 0;
}

static ཧྲིལ
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    གལ་ཏེ (state->comments != NULL) {
        state->comments->count = 0;
    }
    གལ་ཏེ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ཕྱིར -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ཕྱིར 0;
}

ཧྲིལ
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, ཧྲིལ c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    གལ་ཏེ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ཕྱིར 0;
    }
    གལ་ཏེ (c == '!') {
        ཧྲིལ next = tok_nextc(tok);
        tok_backup(tok, next);
        གལ་ཏེ (next == '=') {
            ཕྱིར 0;
        }
    }
    གལ་ཏེ (finish_ftstring_expr(tok, state, token) < 0) {
        ཕྱིར -1;
    }
    གལ་ཏེ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ཕྱིར COLON;
    }
    ཕྱིར 0;
}

ཧྲིལ
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             ཧྲིལ c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    ཧྲིལ depth = _PyLexer_FTStringBracketDepth(tok, state);
    གལ་ཏེ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ཕྱིར -1;
    }
    གལ་ཏེ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ཕྱིར 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    གལ་ཏེ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

ཧྲིལ
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             ཧྲིལ saw_b, ཧྲིལ saw_r, ཧྲིལ saw_u,
                                             ཧྲིལ saw_f, ཧྲིལ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ཧྲིལ)(tok->start + 1 - tok->line_start),                 \
            (ཧྲིལ)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ཕྱིར -1;                                                        \
    } སྐབས (0)

    གལ་ཏེ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    གལ་ཏེ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    གལ་ཏེ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    གལ་ཏེ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    གལ་ཏེ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    གལ་ཏེ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    གལ་ཏེ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ཕྱིར 0;
}

ཧྲིལ
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            ཧྲིལ c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ཧྲིལ quote = c;
    ཧྲིལ quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ཧྲིལ after_quote = tok_nextc(tok);
    གལ་ཏེ (after_quote == quote) {
        ཧྲིལ after_after_quote = tok_nextc(tok);
        གལ་ཏེ (after_after_quote == quote) {
            quote_size = 3;
        }
        དེ་མིན {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    གལ་ཏེ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    གལ་ཏེ (state == NULL) {
        ཕྱིར MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ཕྱིར _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ཧྲིལ
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ཧྲིལ c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ཧྲིལ quote = c;
    ཧྲིལ quote_size = 1;             /* 1 or 3 */
    ཧྲིལ end_quote_size = 0;
    ཧྲིལ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    གལ་ཏེ (c == quote) {
        c = tok_nextc(tok);
        གལ་ཏེ (c == quote) {
            quote_size = 3;
        }
        དེ་མིན {
            end_quote_size = 1;     /* empty string found */
        }
    }
    གལ་ཏེ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    སྐབས (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        གལ་ཏེ (tok->done == E_ERROR) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
        གལ་ཏེ (tok->done == E_DECODE) {
            bha_nga;
        }
        གལ་ཏེ (c == EOF || (quote_size == 1 && c == '\n')) {
            གལ་ཏེ (tok_failed(tok)) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
            གལ་ཏེ (c == EOF && tok->allow_incomplete_input) {
                ཕྱིར incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ཧྲིལ end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            གལ་ཏེ (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                གལ་ཏེ (state->quote == quote && state->quote_size == quote_size) {
                    ཧྲིལ level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    ཧྲིལ lineno = tok->parenlinenostack[level];
                    གལ་ཏེ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    དེ་མིན {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ཕྱིར string_error_token(tok, token, tok->start, location);
                }
            }

            གལ་ཏེ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                གལ་ཏེ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ཕྱིར string_error_token(tok, token, tok->start, location);
            }
            དེ་མིན {
                གལ་ཏེ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } དེ་མིན {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                གལ་ཏེ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ཕྱིར string_error_token(tok, token, tok->start, location);
            }
        }
        གལ་ཏེ (c == quote) {
            end_quote_size += 1;
        }
        དེ་མིན {
            end_quote_size = 0;
            གལ་ཏེ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                གལ་ཏེ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                གལ་ཏེ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ཕྱིར MAKE_TOKEN(STRING);
}

ཧྲིལ
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ཧྲིལ token_type;
    ཧྲིལ end_quote_size = 0;
    ཧྲིལ unicode_escape = 0;
    ཧྲིལ quote = current->quote;
    ཧྲིལ quote_size = current->quote_size;
    ཧྲིལ in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ཧྲིལ raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    སྐབས (end_quote_size != quote_size) {
        ཧྲིལ c = tok_nextc(tok);
        གལ་ཏེ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }

        གལ་ཏེ (c == EOF || (quote_size == 1 && c == '\n')) {
            གལ་ཏེ (tok_failed(tok)) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
            གལ་ཏེ (c == EOF && tok->allow_incomplete_input) {
                ཕྱིར incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            གལ་ཏེ (in_format_spec && c == '\n') {
                ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ཧྲིལ end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            གལ་ཏེ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                གལ་ཏེ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ཕྱིར string_error_token(tok, token,
                    current->start, location);
            }
            དེ་མིན {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ཕྱིར string_error_token(tok, token,
                    current->start, location);
            }
        }

        གལ་ཏེ (c == quote) {
            end_quote_size += 1;
            continue;
        } དེ་མིན {
            end_quote_size = 0;
        }

        གལ་ཏེ (c == '{') {
            ཧྲིལ peek = tok_nextc(tok);
            གལ་ཏེ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                གལ་ཏེ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                གལ་ཏེ (p_start == p_end) {
                    ཕྱིར _PyLexer_get_normal(tok, current, token);
                }
            } དེ་མིན {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } དེ་མིན གལ་ཏེ (c == '}') {
            གལ་ཏེ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            ཧྲིལ peek = tok_nextc(tok);

            ཧྲིལ bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            གལ་ཏེ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            དེ་མིན {
                tok_backup(tok, peek);
                གལ་ཏེ (!in_format_spec && bracket_depth == 0) {
                    གལ་ཏེ (tok->start == tok->cur - 1) {
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } དེ་མིན གལ་ཏེ (c == '\\') {
            ཧྲིལ peek = tok_nextc(tok);
            གལ་ཏེ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            གལ་ཏེ (peek == '{' || peek == '}') {
                གལ་ཏེ (!raw) {
                    གལ་ཏེ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            གལ་ཏེ (!raw) {
                གལ་ཏེ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    གལ་ཏེ (peek == '{') {
                        unicode_escape = 1;
                    } དེ་མིན {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    གལ་ཏེ (p_end - quote_size == p_start) {
        ཧྲིལ end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ཕྱིར MAKE_TOKEN(end_token);
    }
    ཆེད་དུ (ཧྲིལ i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ཕྱིར token_type;
}
