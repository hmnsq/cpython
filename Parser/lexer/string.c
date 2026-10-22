#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static පූර්ණ_සංඛ්‍යා
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    පූර්ණ_සංඛ්‍යා type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ආපසු type;
}

static පූර්ණ_සංඛ්‍යා
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        පූර්ණ_සංඛ්‍යා type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ආපසු type;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    නම් (state->expr_span.end >= 0) {
        ආපසු 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    නම් (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        නම් (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ආපසු -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        නම් (resized == NULL) {
            PyErr_NoMemory();
            ආපසු -1;
        }
        comments = resized;
        නම් (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    නම් (state->expr_span.end >= 0) {
        ආපසු 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    පූර්ණ_සංඛ්‍යා tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    නම් (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ආපසු 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    නම් (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        සඳහා (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            නම් (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        නම් (stripped == NULL) {
            PyErr_NoMemory();
            ආපසු -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        සඳහා (Py_ssize_t i = 0; i < comment_count; i++) {
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
    නැතහොත් {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    නම් (!res) {
        ආපසු -1;
    }
    token->metadata = res;
    ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    නම් (state->comments != NULL) {
        state->comments->count = 0;
    }
    නම් (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ආපසු -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, පූර්ණ_සංඛ්‍යා c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    නම් (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ආපසු 0;
    }
    නම් (c == '!') {
        පූර්ණ_සංඛ්‍යා next = tok_nextc(tok);
        tok_backup(tok, next);
        නම් (next == '=') {
            ආපසු 0;
        }
    }
    නම් (finish_ftstring_expr(tok, state, token) < 0) {
        ආපසු -1;
    }
    නම් (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ආපසු COLON;
    }
    ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             පූර්ණ_සංඛ්‍යා c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    පූර්ණ_සංඛ්‍යා depth = _PyLexer_FTStringBracketDepth(tok, state);
    නම් (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ආපසු -1;
    }
    නම් (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ආපසු 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    නම් (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

පූර්ණ_සංඛ්‍යා
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             පූර්ණ_සංඛ්‍යා saw_b, පූර්ණ_සංඛ්‍යා saw_r, පූර්ණ_සංඛ්‍යා saw_u,
                                             පූර්ණ_සංඛ්‍යා saw_f, පූර්ණ_සංඛ්‍යා saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (පූර්ණ_සංඛ්‍යා)(tok->start + 1 - tok->line_start),                 \
            (පූර්ණ_සංඛ්‍යා)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ආපසු -1;                                                        \
    } අතරතුර (0)

    නම් (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    නම් (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    නම් (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    නම් (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    නම් (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    නම් (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    නම් (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            පූර්ණ_සංඛ්‍යා c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    පූර්ණ_සංඛ්‍යා quote = c;
    පූර්ණ_සංඛ්‍යා quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    පූර්ණ_සංඛ්‍යා after_quote = tok_nextc(tok);
    නම් (after_quote == quote) {
        පූර්ණ_සංඛ්‍යා after_after_quote = tok_nextc(tok);
        නම් (after_after_quote == quote) {
            quote_size = 3;
        }
        නැතහොත් {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    නම් (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    නම් (state == NULL) {
        ආපසු MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ආපසු _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

පූර්ණ_සංඛ්‍යා
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, පූර්ණ_සංඛ්‍යා c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    පූර්ණ_සංඛ්‍යා quote = c;
    පූර්ණ_සංඛ්‍යා quote_size = 1;             /* 1 or 3 */
    පූර්ණ_සංඛ්‍යා end_quote_size = 0;
    පූර්ණ_සංඛ්‍යා has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    නම් (c == quote) {
        c = tok_nextc(tok);
        නම් (c == quote) {
            quote_size = 3;
        }
        නැතහොත් {
            end_quote_size = 1;     /* empty string found */
        }
    }
    නම් (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    අතරතුර (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        නම් (tok->done == E_ERROR) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
        නම් (tok->done == E_DECODE) {
            bha_nga;
        }
        නම් (c == EOF || (quote_size == 1 && c == '\n')) {
            නම් (tok_failed(tok)) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
            නම් (c == EOF && tok->allow_incomplete_input) {
                ආපසු incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            පූර්ණ_සංඛ්‍යා end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            නම් (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                නම් (state->quote == quote && state->quote_size == quote_size) {
                    පූර්ණ_සංඛ්‍යා level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    පූර්ණ_සංඛ්‍යා lineno = tok->parenlinenostack[level];
                    නම් (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    නැතහොත් {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ආපසු string_error_token(tok, token, tok->start, location);
                }
            }

            නම් (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                නම් (c != '\n') {
                    tok->done = E_EOFS;
                }
                ආපසු string_error_token(tok, token, tok->start, location);
            }
            නැතහොත් {
                නම් (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } නැතහොත් {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                නම් (c != '\n') {
                    tok->done = E_EOLS;
                }
                ආපසු string_error_token(tok, token, tok->start, location);
            }
        }
        නම් (c == quote) {
            end_quote_size += 1;
        }
        නැතහොත් {
            end_quote_size = 0;
            නම් (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                නම් (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                නම් (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ආපසු MAKE_TOKEN(STRING);
}

පූර්ණ_සංඛ්‍යා
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    පූර්ණ_සංඛ්‍යා token_type;
    පූර්ණ_සංඛ්‍යා end_quote_size = 0;
    පූර්ණ_සංඛ්‍යා unicode_escape = 0;
    පූර්ණ_සංඛ්‍යා quote = current->quote;
    පූර්ණ_සංඛ්‍යා quote_size = current->quote_size;
    පූර්ණ_සංඛ්‍යා in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    පූර්ණ_සංඛ්‍යා raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    අතරතුර (end_quote_size != quote_size) {
        පූර්ණ_සංඛ්‍යා c = tok_nextc(tok);
        නම් (tok->done == E_ERROR || tok->done == E_DECODE) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }

        නම් (c == EOF || (quote_size == 1 && c == '\n')) {
            නම් (tok_failed(tok)) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
            නම් (c == EOF && tok->allow_incomplete_input) {
                ආපසු incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            නම් (in_format_spec && c == '\n') {
                ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            පූර්ණ_සංඛ්‍යා end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            නම් (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                නම් (c != '\n') {
                    tok->done = E_EOFS;
                }
                ආපසු string_error_token(tok, token,
                    current->start, location);
            }
            නැතහොත් {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ආපසු string_error_token(tok, token,
                    current->start, location);
            }
        }

        නම් (c == quote) {
            end_quote_size += 1;
            continue;
        } නැතහොත් {
            end_quote_size = 0;
        }

        නම් (c == '{') {
            පූර්ණ_සංඛ්‍යා peek = tok_nextc(tok);
            නම් (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                නම් (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ආපසු MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                නම් (p_start == p_end) {
                    ආපසු _PyLexer_get_normal(tok, current, token);
                }
            } නැතහොත් {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } නැතහොත් නම් (c == '}') {
            නම් (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            පූර්ණ_සංඛ්‍යා peek = tok_nextc(tok);

            පූර්ණ_සංඛ්‍යා bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            නම් (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            නැතහොත් {
                tok_backup(tok, peek);
                නම් (!in_format_spec && bracket_depth == 0) {
                    නම් (tok->start == tok->cur - 1) {
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } නැතහොත් නම් (c == '\\') {
            පූර්ණ_සංඛ්‍යා peek = tok_nextc(tok);
            නම් (peek == '\r') {
                peek = tok_nextc(tok);
            }
            නම් (peek == '{' || peek == '}') {
                නම් (!raw) {
                    නම් (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ආපසු MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            නම් (!raw) {
                නම් (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    නම් (peek == '{') {
                        unicode_escape = 1;
                    } නැතහොත් {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    නම් (p_end - quote_size == p_start) {
        පූර්ණ_සංඛ්‍යා end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ආපසු MAKE_TOKEN(end_token);
    }
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ආපසු token_type;
}
