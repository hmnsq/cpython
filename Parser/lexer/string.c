#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ຈຳນວນເຕັມ
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    ຈຳນວນເຕັມ type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ສົ່ງຄືນ type;
}

static ຈຳນວນເຕັມ
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        ຈຳນວນເຕັມ type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ສົ່ງຄືນ type;
}

ຈຳນວນເຕັມ
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ຖ້າ (state->expr_span.end >= 0) {
        ສົ່ງຄືນ 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ຖ້າ (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        ຖ້າ (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ສົ່ງຄືນ -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ຖ້າ (resized == NULL) {
            PyErr_NoMemory();
            ສົ່ງຄືນ -1;
        }
        comments = resized;
        ຖ້າ (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ຖ້າ (state->expr_span.end >= 0) {
        ສົ່ງຄືນ 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    ຈຳນວນເຕັມ tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ຖ້າ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ສົ່ງຄືນ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ຖ້າ (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        ສຳລັບ (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ຖ້າ (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        ຖ້າ (stripped == NULL) {
            PyErr_NoMemory();
            ສົ່ງຄືນ -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        ສຳລັບ (Py_ssize_t i = 0; i < comment_count; i++) {
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
    ບໍ່ດັ່ງນັ້ນ {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    ຖ້າ (!res) {
        ສົ່ງຄືນ -1;
    }
    token->metadata = res;
    ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ຖ້າ (state->comments != NULL) {
        state->comments->count = 0;
    }
    ຖ້າ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ສົ່ງຄືນ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, ຈຳນວນເຕັມ c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    ຖ້າ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ສົ່ງຄືນ 0;
    }
    ຖ້າ (c == '!') {
        ຈຳນວນເຕັມ next = tok_nextc(tok);
        tok_backup(tok, next);
        ຖ້າ (next == '=') {
            ສົ່ງຄືນ 0;
        }
    }
    ຖ້າ (finish_ftstring_expr(tok, state, token) < 0) {
        ສົ່ງຄືນ -1;
    }
    ຖ້າ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ສົ່ງຄືນ COLON;
    }
    ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             ຈຳນວນເຕັມ c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    ຈຳນວນເຕັມ depth = _PyLexer_FTStringBracketDepth(tok, state);
    ຖ້າ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ສົ່ງຄືນ -1;
    }
    ຖ້າ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ສົ່ງຄືນ 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    ຖ້າ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

ຈຳນວນເຕັມ
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             ຈຳນວນເຕັມ saw_b, ຈຳນວນເຕັມ saw_r, ຈຳນວນເຕັມ saw_u,
                                             ຈຳນວນເຕັມ saw_f, ຈຳນວນເຕັມ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (ຈຳນວນເຕັມ)(tok->start + 1 - tok->line_start),                 \
            (ຈຳນວນເຕັມ)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ສົ່ງຄືນ -1;                                                        \
    } ໃນຂະນະທີ່ (0)

    ຖ້າ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ຖ້າ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ຖ້າ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ຖ້າ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ຖ້າ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ຖ້າ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ຖ້າ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            ຈຳນວນເຕັມ c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ຈຳນວນເຕັມ quote = c;
    ຈຳນວນເຕັມ quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ຈຳນວນເຕັມ after_quote = tok_nextc(tok);
    ຖ້າ (after_quote == quote) {
        ຈຳນວນເຕັມ after_after_quote = tok_nextc(tok);
        ຖ້າ (after_after_quote == quote) {
            quote_size = 3;
        }
        ບໍ່ດັ່ງນັ້ນ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ຖ້າ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ຖ້າ (state == NULL) {
        ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ສົ່ງຄືນ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

ຈຳນວນເຕັມ
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ຈຳນວນເຕັມ c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ຈຳນວນເຕັມ quote = c;
    ຈຳນວນເຕັມ quote_size = 1;             /* 1 or 3 */
    ຈຳນວນເຕັມ end_quote_size = 0;
    ຈຳນວນເຕັມ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ຖ້າ (c == quote) {
        c = tok_nextc(tok);
        ຖ້າ (c == quote) {
            quote_size = 3;
        }
        ບໍ່ດັ່ງນັ້ນ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ຖ້າ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ໃນຂະນະທີ່ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ຖ້າ (tok->done == E_ERROR) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
        ຖ້າ (tok->done == E_DECODE) {
            bha_nga;
        }
        ຖ້າ (c == EOF || (quote_size == 1 && c == '\n')) {
            ຖ້າ (tok_failed(tok)) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
            ຖ້າ (c == EOF && tok->allow_incomplete_input) {
                ສົ່ງຄືນ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            ຈຳນວນເຕັມ end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ຖ້າ (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ຖ້າ (state->quote == quote && state->quote_size == quote_size) {
                    ຈຳນວນເຕັມ level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    ຈຳນວນເຕັມ lineno = tok->parenlinenostack[level];
                    ຖ້າ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    ບໍ່ດັ່ງນັ້ນ {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ສົ່ງຄືນ string_error_token(tok, token, tok->start, location);
                }
            }

            ຖ້າ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ຖ້າ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ສົ່ງຄືນ string_error_token(tok, token, tok->start, location);
            }
            ບໍ່ດັ່ງນັ້ນ {
                ຖ້າ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } ບໍ່ດັ່ງນັ້ນ {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ຖ້າ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ສົ່ງຄືນ string_error_token(tok, token, tok->start, location);
            }
        }
        ຖ້າ (c == quote) {
            end_quote_size += 1;
        }
        ບໍ່ດັ່ງນັ້ນ {
            end_quote_size = 0;
            ຖ້າ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ຖ້າ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ຖ້າ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ສົ່ງຄືນ MAKE_TOKEN(STRING);
}

ຈຳນວນເຕັມ
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ຈຳນວນເຕັມ token_type;
    ຈຳນວນເຕັມ end_quote_size = 0;
    ຈຳນວນເຕັມ unicode_escape = 0;
    ຈຳນວນເຕັມ quote = current->quote;
    ຈຳນວນເຕັມ quote_size = current->quote_size;
    ຈຳນວນເຕັມ in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    ຈຳນວນເຕັມ raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ໃນຂະນະທີ່ (end_quote_size != quote_size) {
        ຈຳນວນເຕັມ c = tok_nextc(tok);
        ຖ້າ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }

        ຖ້າ (c == EOF || (quote_size == 1 && c == '\n')) {
            ຖ້າ (tok_failed(tok)) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
            ຖ້າ (c == EOF && tok->allow_incomplete_input) {
                ສົ່ງຄືນ incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ຖ້າ (in_format_spec && c == '\n') {
                ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            ຈຳນວນເຕັມ end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ຖ້າ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ຖ້າ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ສົ່ງຄືນ string_error_token(tok, token,
                    current->start, location);
            }
            ບໍ່ດັ່ງນັ້ນ {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ສົ່ງຄືນ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ຖ້າ (c == quote) {
            end_quote_size += 1;
            continue;
        } ບໍ່ດັ່ງນັ້ນ {
            end_quote_size = 0;
        }

        ຖ້າ (c == '{') {
            ຈຳນວນເຕັມ peek = tok_nextc(tok);
            ຖ້າ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ຖ້າ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ຖ້າ (p_start == p_end) {
                    ສົ່ງຄືນ _PyLexer_get_normal(tok, current, token);
                }
            } ບໍ່ດັ່ງນັ້ນ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '}') {
            ຖ້າ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            ຈຳນວນເຕັມ peek = tok_nextc(tok);

            ຈຳນວນເຕັມ bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ຖ້າ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ບໍ່ດັ່ງນັ້ນ {
                tok_backup(tok, peek);
                ຖ້າ (!in_format_spec && bracket_depth == 0) {
                    ຖ້າ (tok->start == tok->cur - 1) {
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\\') {
            ຈຳນວນເຕັມ peek = tok_nextc(tok);
            ຖ້າ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ຖ້າ (peek == '{' || peek == '}') {
                ຖ້າ (!raw) {
                    ຖ້າ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            ຖ້າ (!raw) {
                ຖ້າ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ຖ້າ (peek == '{') {
                        unicode_escape = 1;
                    } ບໍ່ດັ່ງນັ້ນ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ຖ້າ (p_end - quote_size == p_start) {
        ຈຳນວນເຕັມ end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ສົ່ງຄືນ MAKE_TOKEN(end_token);
    }
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ສົ່ງຄືນ token_type;
}
