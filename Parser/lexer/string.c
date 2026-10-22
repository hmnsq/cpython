#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ចំនួនគត់
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ចំនួនគត់ type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ត្រឡប់ type;
}

static ចំនួនគត់
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        ចំនួនគត់ type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ត្រឡប់ type;
}

ចំនួនគត់
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ប្រសិនបើ (state->expr_span.end >= 0) {
        ត្រឡប់ 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ប្រសិនបើ (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        ប្រសិនបើ (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ត្រឡប់ -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ប្រសិនបើ (resized == NULL) {
            PyErr_NoMemory();
            ត្រឡប់ -1;
        }
        comments = resized;
        ប្រសិនបើ (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ត្រឡប់ 0;
}

static ចំនួនគត់
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ប្រសិនបើ (state->expr_span.end >= 0) {
        ត្រឡប់ 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    ចំនួនគត់ tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ប្រសិនបើ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ត្រឡប់ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ប្រសិនបើ (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        សម្រាប់ (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ប្រសិនបើ (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        ប្រសិនបើ (stripped == NULL) {
            PyErr_NoMemory();
            ត្រឡប់ -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        សម្រាប់ (Py_ssize_t i = 0; i < comment_count; i++) {
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
    បើមិនដូច្នេះ {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    ប្រសិនបើ (!res) {
        ត្រឡប់ -1;
    }
    token->metadata = res;
    ត្រឡប់ 0;
}

static ចំនួនគត់
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ប្រសិនបើ (state->comments != NULL) {
        state->comments->count = 0;
    }
    ប្រសិនបើ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ត្រឡប់ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, ចំនួនគត់ c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    ប្រសិនបើ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ (c == '!') {
        ចំនួនគត់ next = tok_nextc(tok);
        tok_backup(tok, next);
        ប្រសិនបើ (next == '=') {
            ត្រឡប់ 0;
        }
    }
    ប្រសិនបើ (finish_ftstring_expr(tok, state, token) < 0) {
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ត្រឡប់ COLON;
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             ចំនួនគត់ c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    ចំនួនគត់ depth = _PyLexer_FTStringBracketDepth(tok, state);
    ប្រសិនបើ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ត្រឡប់ 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    ប្រសិនបើ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

ចំនួនគត់
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             ចំនួនគត់ saw_b, ចំនួនគត់ saw_r, ចំនួនគត់ saw_u,
                                             ចំនួនគត់ saw_f, ចំនួនគត់ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ចំនួនគត់)(tok->start + 1 - tok->line_start),                 \
            (ចំនួនគត់)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ត្រឡប់ -1;                                                        \
    } ខណៈពេល (0)

    ប្រសិនបើ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ប្រសិនបើ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ប្រសិនបើ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ប្រសិនបើ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ប្រសិនបើ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ប្រសិនបើ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ប្រសិនបើ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ត្រឡប់ 0;
}

ចំនួនគត់
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            ចំនួនគត់ c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ចំនួនគត់ quote = c;
    ចំនួនគត់ quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ចំនួនគត់ after_quote = tok_nextc(tok);
    ប្រសិនបើ (after_quote == quote) {
        ចំនួនគត់ after_after_quote = tok_nextc(tok);
        ប្រសិនបើ (after_after_quote == quote) {
            quote_size = 3;
        }
        បើមិនដូច្នេះ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ប្រសិនបើ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ប្រសិនបើ (state == NULL) {
        ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ត្រឡប់ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ចំនួនគត់
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ចំនួនគត់ c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ចំនួនគត់ quote = c;
    ចំនួនគត់ quote_size = 1;             /* 1 or 3 */
    ចំនួនគត់ end_quote_size = 0;
    ចំនួនគត់ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ប្រសិនបើ (c == quote) {
        c = tok_nextc(tok);
        ប្រសិនបើ (c == quote) {
            quote_size = 3;
        }
        បើមិនដូច្នេះ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ប្រសិនបើ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ខណៈពេល (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ប្រសិនបើ (tok->done == E_ERROR) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
        ប្រសិនបើ (tok->done == E_DECODE) {
            bha_nga;
        }
        ប្រសិនបើ (c == EOF || (quote_size == 1 && c == '\n')) {
            ប្រសិនបើ (tok_failed(tok)) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
            ប្រសិនបើ (c == EOF && tok->allow_incomplete_input) {
                ត្រឡប់ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ចំនួនគត់ end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ប្រសិនបើ (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ប្រសិនបើ (state->quote == quote && state->quote_size == quote_size) {
                    ចំនួនគត់ level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    ចំនួនគត់ lineno = tok->parenlinenostack[level];
                    ប្រសិនបើ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    បើមិនដូច្នេះ {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ត្រឡប់ string_error_token(tok, token, tok->start, location);
                }
            }

            ប្រសិនបើ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ប្រសិនបើ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ត្រឡប់ string_error_token(tok, token, tok->start, location);
            }
            បើមិនដូច្នេះ {
                ប្រសិនបើ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } បើមិនដូច្នេះ {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ប្រសិនបើ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ត្រឡប់ string_error_token(tok, token, tok->start, location);
            }
        }
        ប្រសិនបើ (c == quote) {
            end_quote_size += 1;
        }
        បើមិនដូច្នេះ {
            end_quote_size = 0;
            ប្រសិនបើ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ប្រសិនបើ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ប្រសិនបើ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ត្រឡប់ MAKE_TOKEN(STRING);
}

ចំនួនគត់
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ចំនួនគត់ token_type;
    ចំនួនគត់ end_quote_size = 0;
    ចំនួនគត់ unicode_escape = 0;
    ចំនួនគត់ quote = current->quote;
    ចំនួនគត់ quote_size = current->quote_size;
    ចំនួនគត់ in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ចំនួនគត់ raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ខណៈពេល (end_quote_size != quote_size) {
        ចំនួនគត់ c = tok_nextc(tok);
        ប្រសិនបើ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }

        ប្រសិនបើ (c == EOF || (quote_size == 1 && c == '\n')) {
            ប្រសិនបើ (tok_failed(tok)) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
            ប្រសិនបើ (c == EOF && tok->allow_incomplete_input) {
                ត្រឡប់ incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ប្រសិនបើ (in_format_spec && c == '\n') {
                ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ចំនួនគត់ end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ប្រសិនបើ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ប្រសិនបើ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ត្រឡប់ string_error_token(tok, token,
                    current->start, location);
            }
            បើមិនដូច្នេះ {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ត្រឡប់ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ប្រសិនបើ (c == quote) {
            end_quote_size += 1;
            continue;
        } បើមិនដូច្នេះ {
            end_quote_size = 0;
        }

        ប្រសិនបើ (c == '{') {
            ចំនួនគត់ peek = tok_nextc(tok);
            ប្រសិនបើ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ប្រសិនបើ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ប្រសិនបើ (p_start == p_end) {
                    ត្រឡប់ _PyLexer_get_normal(tok, current, token);
                }
            } បើមិនដូច្នេះ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } បើមិនដូច្នេះ ប្រសិនបើ (c == '}') {
            ប្រសិនបើ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            ចំនួនគត់ peek = tok_nextc(tok);

            ចំនួនគត់ bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ប្រសិនបើ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            បើមិនដូច្នេះ {
                tok_backup(tok, peek);
                ប្រសិនបើ (!in_format_spec && bracket_depth == 0) {
                    ប្រសិនបើ (tok->start == tok->cur - 1) {
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } បើមិនដូច្នេះ ប្រសិនបើ (c == '\\') {
            ចំនួនគត់ peek = tok_nextc(tok);
            ប្រសិនបើ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ប្រសិនបើ (peek == '{' || peek == '}') {
                ប្រសិនបើ (!raw) {
                    ប្រសិនបើ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            ប្រសិនបើ (!raw) {
                ប្រសិនបើ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ប្រសិនបើ (peek == '{') {
                        unicode_escape = 1;
                    } បើមិនដូច្នេះ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ប្រសិនបើ (p_end - quote_size == p_start) {
        ចំនួនគត់ end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ត្រឡប់ MAKE_TOKEN(end_token);
    }
    សម្រាប់ (ចំនួនគត់ i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ត្រឡប់ token_type;
}
