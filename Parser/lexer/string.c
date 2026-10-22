#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ကိန်းပြည့်
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ကိန်းပြည့် type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ပြန်ပေး type;
}

static ကိန်းပြည့်
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        ကိန်းပြည့် type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ပြန်ပေး type;
}

ကိန်းပြည့်
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    အကယ်၍ (state->expr_span.end >= 0) {
        ပြန်ပေး 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    အကယ်၍ (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        အကယ်၍ (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ပြန်ပေး -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        အကယ်၍ (resized == NULL) {
            PyErr_NoMemory();
            ပြန်ပေး -1;
        }
        comments = resized;
        အကယ်၍ (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    အကယ်၍ (state->expr_span.end >= 0) {
        ပြန်ပေး 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    ကိန်းပြည့် tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    အကယ်၍ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ပြန်ပေး 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    အကယ်၍ (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        အတွက် (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            အကယ်၍ (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        အကယ်၍ (stripped == NULL) {
            PyErr_NoMemory();
            ပြန်ပေး -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        အတွက် (Py_ssize_t i = 0; i < comment_count; i++) {
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
    မဟုတ်ပါက {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    အကယ်၍ (!res) {
        ပြန်ပေး -1;
    }
    token->metadata = res;
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    အကယ်၍ (state->comments != NULL) {
        state->comments->count = 0;
    }
    အကယ်၍ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ပြန်ပေး -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ပြန်ပေး 0;
}

ကိန်းပြည့်
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, ကိန်းပြည့် c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    အကယ်၍ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ပြန်ပေး 0;
    }
    အကယ်၍ (c == '!') {
        ကိန်းပြည့် next = tok_nextc(tok);
        tok_backup(tok, next);
        အကယ်၍ (next == '=') {
            ပြန်ပေး 0;
        }
    }
    အကယ်၍ (finish_ftstring_expr(tok, state, token) < 0) {
        ပြန်ပေး -1;
    }
    အကယ်၍ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ပြန်ပေး COLON;
    }
    ပြန်ပေး 0;
}

ကိန်းပြည့်
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             ကိန်းပြည့် c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    ကိန်းပြည့် depth = _PyLexer_FTStringBracketDepth(tok, state);
    အကယ်၍ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ပြန်ပေး -1;
    }
    အကယ်၍ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ပြန်ပေး 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    အကယ်၍ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

ကိန်းပြည့်
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             ကိန်းပြည့် saw_b, ကိန်းပြည့် saw_r, ကိန်းပြည့် saw_u,
                                             ကိန်းပြည့် saw_f, ကိန်းပြည့် saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ကိန်းပြည့်)(tok->start + 1 - tok->line_start),                 \
            (ကိန်းပြည့်)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ပြန်ပေး -1;                                                        \
    } နေစဉ် (0)

    အကယ်၍ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    အကယ်၍ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    အကယ်၍ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    အကယ်၍ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    အကယ်၍ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    အကယ်၍ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    အကယ်၍ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ပြန်ပေး 0;
}

ကိန်းပြည့်
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            ကိန်းပြည့် c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ကိန်းပြည့် quote = c;
    ကိန်းပြည့် quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ကိန်းပြည့် after_quote = tok_nextc(tok);
    အကယ်၍ (after_quote == quote) {
        ကိန်းပြည့် after_after_quote = tok_nextc(tok);
        အကယ်၍ (after_after_quote == quote) {
            quote_size = 3;
        }
        မဟုတ်ပါက {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    အကယ်၍ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    အကယ်၍ (state == NULL) {
        ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ပြန်ပေး _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ကိန်းပြည့်
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ကိန်းပြည့် c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ကိန်းပြည့် quote = c;
    ကိန်းပြည့် quote_size = 1;             /* 1 or 3 */
    ကိန်းပြည့် end_quote_size = 0;
    ကိန်းပြည့် has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    အကယ်၍ (c == quote) {
        c = tok_nextc(tok);
        အကယ်၍ (c == quote) {
            quote_size = 3;
        }
        မဟုတ်ပါက {
            end_quote_size = 1;     /* empty string found */
        }
    }
    အကယ်၍ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    နေစဉ် (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        အကယ်၍ (tok->done == E_ERROR) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
        အကယ်၍ (tok->done == E_DECODE) {
            bha_nga;
        }
        အကယ်၍ (c == EOF || (quote_size == 1 && c == '\n')) {
            အကယ်၍ (tok_failed(tok)) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
            အကယ်၍ (c == EOF && tok->allow_incomplete_input) {
                ပြန်ပေး incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ကိန်းပြည့် end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            အကယ်၍ (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                အကယ်၍ (state->quote == quote && state->quote_size == quote_size) {
                    ကိန်းပြည့် level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    ကိန်းပြည့် lineno = tok->parenlinenostack[level];
                    အကယ်၍ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    မဟုတ်ပါက {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ပြန်ပေး string_error_token(tok, token, tok->start, location);
                }
            }

            အကယ်၍ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                အကယ်၍ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ပြန်ပေး string_error_token(tok, token, tok->start, location);
            }
            မဟုတ်ပါက {
                အကယ်၍ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } မဟုတ်ပါက {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                အကယ်၍ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ပြန်ပေး string_error_token(tok, token, tok->start, location);
            }
        }
        အကယ်၍ (c == quote) {
            end_quote_size += 1;
        }
        မဟုတ်ပါက {
            end_quote_size = 0;
            အကယ်၍ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                အကယ်၍ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                အကယ်၍ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ပြန်ပေး MAKE_TOKEN(STRING);
}

ကိန်းပြည့်
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ကိန်းပြည့် token_type;
    ကိန်းပြည့် end_quote_size = 0;
    ကိန်းပြည့် unicode_escape = 0;
    ကိန်းပြည့် quote = current->quote;
    ကိန်းပြည့် quote_size = current->quote_size;
    ကိန်းပြည့် in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ကိန်းပြည့် raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    နေစဉ် (end_quote_size != quote_size) {
        ကိန်းပြည့် c = tok_nextc(tok);
        အကယ်၍ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }

        အကယ်၍ (c == EOF || (quote_size == 1 && c == '\n')) {
            အကယ်၍ (tok_failed(tok)) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
            အကယ်၍ (c == EOF && tok->allow_incomplete_input) {
                ပြန်ပေး incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            အကယ်၍ (in_format_spec && c == '\n') {
                ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ကိန်းပြည့် end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            အကယ်၍ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                အကယ်၍ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ပြန်ပေး string_error_token(tok, token,
                    current->start, location);
            }
            မဟုတ်ပါက {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ပြန်ပေး string_error_token(tok, token,
                    current->start, location);
            }
        }

        အကယ်၍ (c == quote) {
            end_quote_size += 1;
            continue;
        } မဟုတ်ပါက {
            end_quote_size = 0;
        }

        အကယ်၍ (c == '{') {
            ကိန်းပြည့် peek = tok_nextc(tok);
            အကယ်၍ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                အကယ်၍ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                အကယ်၍ (p_start == p_end) {
                    ပြန်ပေး _PyLexer_get_normal(tok, current, token);
                }
            } မဟုတ်ပါက {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } မဟုတ်ပါက အကယ်၍ (c == '}') {
            အကယ်၍ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            ကိန်းပြည့် peek = tok_nextc(tok);

            ကိန်းပြည့် bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            အကယ်၍ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            မဟုတ်ပါက {
                tok_backup(tok, peek);
                အကယ်၍ (!in_format_spec && bracket_depth == 0) {
                    အကယ်၍ (tok->start == tok->cur - 1) {
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } မဟုတ်ပါက အကယ်၍ (c == '\\') {
            ကိန်းပြည့် peek = tok_nextc(tok);
            အကယ်၍ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            အကယ်၍ (peek == '{' || peek == '}') {
                အကယ်၍ (!raw) {
                    အကယ်၍ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            အကယ်၍ (!raw) {
                အကယ်၍ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    အကယ်၍ (peek == '{') {
                        unicode_escape = 1;
                    } မဟုတ်ပါက {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    အကယ်၍ (p_end - quote_size == p_start) {
        ကိန်းပြည့် end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ပြန်ပေး MAKE_TOKEN(end_token);
    }
    အတွက် (ကိန်းပြည့် i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ပြန်ပေး token_type;
}
