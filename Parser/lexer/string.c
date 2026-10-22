#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static puur_naa_nka
string_error_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    puur_naa_nka type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ᤀᤡᤴ type;
}

static puur_naa_nka
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        puur_naa_nka type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ᤀᤡᤴ type;
}

puur_naa_nka
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ᤀᤡ᤺ (state->expr_span.end >= 0) {
        ᤀᤡᤴ 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ᤀᤡ᤺ (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        ᤀᤡ᤺ (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ᤀᤡᤴ -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ᤀᤡ᤺ (resized == NULL) {
            PyErr_NoMemory();
            ᤀᤡᤴ -1;
        }
        comments = resized;
        ᤀᤡ᤺ (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ᤀᤡᤴ 0;
}

static puur_naa_nka
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ᤀᤡ᤺ (state->expr_span.end >= 0) {
        ᤀᤡᤴ 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    puur_naa_nka tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ᤀᤡ᤺ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ᤀᤡᤴ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ᤀᤡ᤺ (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        for (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ᤀᤡ᤺ (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        ᤀᤡ᤺ (stripped == NULL) {
            PyErr_NoMemory();
            ᤀᤡᤴ -1;
        }
        _PyTok_Span kept = {state->expr_span.start, state->expr_span.start};
        Py_ssize_t stripped_len = 0;
        for (Py_ssize_t i = 0; i < comment_count; i++) {
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
    ᤀᤡ᤺ᤆᤡ {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    ᤀᤡ᤺ (!res) {
        ᤀᤡᤴ -1;
    }
    token->metadata = res;
    ᤀᤡᤴ 0;
}

static puur_naa_nka
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ᤀᤡ᤺ (state->comments != NULL) {
        state->comments->count = 0;
    }
    ᤀᤡ᤺ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ᤀᤡᤴ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ᤀᤡᤴ 0;
}

puur_naa_nka
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, puur_naa_nka c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    ᤀᤡ᤺ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺ (c == '!') {
        puur_naa_nka next = tok_nextc(tok);
        tok_backup(tok, next);
        ᤀᤡ᤺ (next == '=') {
            ᤀᤡᤴ 0;
        }
    }
    ᤀᤡ᤺ (finish_ftstring_expr(tok, state, token) < 0) {
        ᤀᤡᤴ -1;
    }
    ᤀᤡ᤺ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ᤀᤡᤴ COLON;
    }
    ᤀᤡᤴ 0;
}

puur_naa_nka
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             puur_naa_nka c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    puur_naa_nka depth = _PyLexer_FTStringBracketDepth(tok, state);
    ᤀᤡ᤺ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ᤀᤡᤴ -1;
    }
    ᤀᤡ᤺ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ᤀᤡᤴ 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    ᤀᤡ᤺ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_expr = 1;
    }
}

puur_naa_nka
_PyLexer_check_string_prefixes(sa_nracanaa tok_state *tok,
                                             puur_naa_nka saw_b, puur_naa_nka saw_r, puur_naa_nka saw_u,
                                             puur_naa_nka saw_f, puur_naa_nka saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    do {                                                                  \
        (void)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),                 \
            (puur_naa_nka)(tok->cur - tok->line_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ᤀᤡᤴ -1;                                                        \
    } ᤀᤠ᤺ (0)

    ᤀᤡ᤺ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ᤀᤡ᤺ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ᤀᤡ᤺ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ᤀᤡ᤺ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ᤀᤡ᤺ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ᤀᤡ᤺ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ᤀᤡ᤺ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ᤀᤡᤴ 0;
}

puur_naa_nka
_PyLexer_scan_fstring_start(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                            puur_naa_nka c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    puur_naa_nka quote = c;
    puur_naa_nka quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    puur_naa_nka after_quote = tok_nextc(tok);
    ᤀᤡ᤺ (after_quote == quote) {
        puur_naa_nka after_after_quote = tok_nextc(tok);
        ᤀᤡ᤺ (after_after_quote == quote) {
            quote_size = 3;
        }
        ᤀᤡ᤺ᤆᤡ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ᤀᤡ᤺ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ᤀᤡ᤺ (state == NULL) {
        ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ᤀᤡᤴ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

puur_naa_nka
_PyLexer_scan_string(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    puur_naa_nka quote = c;
    puur_naa_nka quote_size = 1;             /* 1 or 3 */
    puur_naa_nka end_quote_size = 0;
    puur_naa_nka has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ᤀᤡ᤺ (c == quote) {
        c = tok_nextc(tok);
        ᤀᤡ᤺ (c == quote) {
            quote_size = 3;
        }
        ᤀᤡ᤺ᤆᤡ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ᤀᤡ᤺ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ᤀᤠ᤺ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ᤀᤡ᤺ (tok->done == E_ERROR) {
            ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
        }
        ᤀᤡ᤺ (tok->done == E_DECODE) {
            bha_nga;
        }
        ᤀᤡ᤺ (c == EOF || (quote_size == 1 && c == '\n')) {
            ᤀᤡ᤺ (tok_failed(tok)) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
            ᤀᤡ᤺ (c == EOF && tok->allow_incomplete_input) {
                ᤀᤡᤴ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            puur_naa_nka end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ᤀᤡ᤺ (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ᤀᤡ᤺ (state->quote == quote && state->quote_size == quote_size) {
                    puur_naa_nka level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    puur_naa_nka lineno = tok->parenlinenostack[level];
                    ᤀᤡ᤺ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    ᤀᤡ᤺ᤆᤡ {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ᤀᤡᤴ string_error_token(tok, token, tok->start, location);
                }
            }

            ᤀᤡ᤺ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ᤀᤡ᤺ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ᤀᤡᤴ string_error_token(tok, token, tok->start, location);
            }
            ᤀᤡ᤺ᤆᤡ {
                ᤀᤡ᤺ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } ᤀᤡ᤺ᤆᤡ {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ᤀᤡ᤺ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ᤀᤡᤴ string_error_token(tok, token, tok->start, location);
            }
        }
        ᤀᤡ᤺ (c == quote) {
            end_quote_size += 1;
        }
        ᤀᤡ᤺ᤆᤡ {
            end_quote_size = 0;
            ᤀᤡ᤺ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ᤀᤡ᤺ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ᤀᤡ᤺ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ᤀᤡᤴ MAKE_TOKEN(STRING);
}

puur_naa_nka
_PyLexer_get_ftstring(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    assert((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_EXPR_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    puur_naa_nka token_type;
    puur_naa_nka end_quote_size = 0;
    puur_naa_nka unicode_escape = 0;
    puur_naa_nka quote = current->quote;
    puur_naa_nka quote_size = current->quote_size;
    puur_naa_nka in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    puur_naa_nka raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ᤀᤠ᤺ (end_quote_size != quote_size) {
        puur_naa_nka c = tok_nextc(tok);
        ᤀᤡ᤺ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
        }

        ᤀᤡ᤺ (c == EOF || (quote_size == 1 && c == '\n')) {
            ᤀᤡ᤺ (tok_failed(tok)) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
            ᤀᤡ᤺ (c == EOF && tok->allow_incomplete_input) {
                ᤀᤡᤴ incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ᤀᤡ᤺ (in_format_spec && c == '\n') {
                ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            puur_naa_nka end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ᤀᤡ᤺ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ᤀᤡ᤺ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ᤀᤡᤴ string_error_token(tok, token,
                    current->start, location);
            }
            ᤀᤡ᤺ᤆᤡ {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ᤀᤡᤴ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ᤀᤡ᤺ (c == quote) {
            end_quote_size += 1;
            continue;
        } ᤀᤡ᤺ᤆᤡ {
            end_quote_size = 0;
        }

        ᤀᤡ᤺ (c == '{') {
            puur_naa_nka peek = tok_nextc(tok);
            ᤀᤡ᤺ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ᤀᤡ᤺ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ᤀᤡ᤺ (p_start == p_end) {
                    ᤀᤡᤴ _PyLexer_get_normal(tok, current, token);
                }
            } ᤀᤡ᤺ᤆᤡ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == '}') {
            ᤀᤡ᤺ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            puur_naa_nka peek = tok_nextc(tok);

            puur_naa_nka bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ᤀᤡ᤺ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ᤀᤡ᤺ᤆᤡ {
                tok_backup(tok, peek);
                ᤀᤡ᤺ (!in_format_spec && bracket_depth == 0) {
                    ᤀᤡ᤺ (tok->start == tok->cur - 1) {
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == '\\') {
            puur_naa_nka peek = tok_nextc(tok);
            ᤀᤡ᤺ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ᤀᤡ᤺ (peek == '{' || peek == '}') {
                ᤀᤡ᤺ (!raw) {
                    ᤀᤡ᤺ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            ᤀᤡ᤺ (!raw) {
                ᤀᤡ᤺ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ᤀᤡ᤺ (peek == '{') {
                        unicode_escape = 1;
                    } ᤀᤡ᤺ᤆᤡ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ᤀᤡ᤺ (p_end - quote_size == p_start) {
        puur_naa_nka end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ᤀᤡᤴ MAKE_TOKEN(end_token);
    }
    for (puur_naa_nka i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ᤀᤡᤴ token_type;
}
