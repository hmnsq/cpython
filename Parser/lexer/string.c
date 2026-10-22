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
    ᯀᯮᯞ type;
}

static puur_naa_nka
incomplete_string_token(sa_nracanaa tok_state *tok, sa_nracanaa token *token,
                        puur_naa_nka type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ᯀᯮᯞ type;
}

puur_naa_nka
_PyLexer_record_ftstring_comment(sa_nracanaa tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    assert(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    ᯀᯮᯝ (state->expr_span.end >= 0) {
        ᯀᯮᯞ 0;
    }
    assert(state->expr_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    ᯀᯮᯝ (comments == NULL || comments->count == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)parimaa_na(*comments)) /
            (Py_ssize_t)parimaa_na(*comments->spans);
        ᯀᯮᯝ (comments != NULL && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ᯀᯮᯞ -1;
        }
        Py_ssize_t capacity = comments == NULL ? 4 : comments->capacity * 2;
        size_t size = parimaa_na(*comments) +
            (size_t)capacity * parimaa_na(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        ᯀᯮᯝ (resized == NULL) {
            PyErr_NoMemory();
            ᯀᯮᯞ -1;
        }
        comments = resized;
        ᯀᯮᯝ (state->comments == NULL) {
            comments->count = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->count++] =
        (_PyTok_Span){start, end};
    ᯀᯮᯞ 0;
}

static puur_naa_nka
finish_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                     sa_nracanaa token *token)
{
    assert(token != NULL && state == _PyLexer_CurrentFTString(tok));
    assert(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    ᯀᯮᯝ (state->expr_span.end >= 0) {
        ᯀᯮᯞ 0;
    }
    assert(state->expr_span.start >= 0);
    state->expr_span.end = tok->start;
    puur_naa_nka tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    ᯀᯮᯝ (!(state->debug_expr || tstring_interpolation) || token->metadata) {
        ᯀᯮᯞ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ᯀᯮᯝ (comments != NULL && comments->count > 0) {
        Py_ssize_t stripped_size =
            state->expr_span.end - state->expr_span.start;
        Py_ssize_t comment_count = 0;
        for (Py_ssize_t i = 0; i < comments->count; i++) {
            _PyTok_Span comment = comments->spans[i];
            assert(_PyTok_SpanIsValid(comment));
            assert(comment.start >= (i == 0 ? state->expr_span.start
                                           : comments->spans[i - 1].end));
            ᯀᯮᯝ (comment.start >= state->expr_span.end) {
                bha_nga;
            }
            assert(comment.end <= state->expr_span.end);
            stripped_size -= comment.end - comment.start;
            comment_count++;
        }
        var_na *stripped = PyMem_Malloc((size_t)stripped_size);
        ᯀᯮᯝ (stripped == NULL) {
            PyErr_NoMemory();
            ᯀᯮᯞ -1;
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
    ᯀᯮᯝᯂᯪ {
        Py_ssize_t expr_len;
        const var_na *expr = _PyTok_SourceSpanView(
            &tok->source, state->expr_span, &expr_len);
        res = PyUnicode_DecodeUTF8(expr, expr_len, NULL);
    }

    ᯀᯮᯝ (!res) {
        ᯀᯮᯞ -1;
    }
    token->metadata = res;
    ᯀᯮᯞ 0;
}

static puur_naa_nka
begin_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                    _PyTok_Off expr_start)
{
    assert(state->mode != FTSTRING_MODE_EXPRESSION);
    state->expr_span = (_PyTok_Span){expr_start, -1};
    ᯀᯮᯝ (state->comments != NULL) {
        state->comments->count = 0;
    }
    ᯀᯮᯝ (state->replacement_depth >= MAX_EXPR_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ᯀᯮᯞ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_expr = 0;
    ᯀᯮᯞ 0;
}

puur_naa_nka
_PyLexer_ftstring_punctuation(sa_nracanaa tok_state *tok, ftstring_state *state,
                              sa_nracanaa token *token, puur_naa_nka c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ':' || c == '}' || c == '!');
    ᯀᯮᯝ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ᯀᯮᯞ 0;
    }
    ᯀᯮᯝ (c == '!') {
        puur_naa_nka next = tok_nextc(tok);
        tok_backup(tok, next);
        ᯀᯮᯝ (next == '=') {
            ᯀᯮᯞ 0;
        }
    }
    ᯀᯮᯝ (finish_ftstring_expr(tok, state, token) < 0) {
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ᯀᯮᯞ COLON;
    }
    ᯀᯮᯞ 0;
}

puur_naa_nka
_PyLexer_close_ftstring_expr(sa_nracanaa tok_state *tok, ftstring_state *state,
                             puur_naa_nka c)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    assert(c == ')' || c == ']' || c == '}');
    puur_naa_nka depth = _PyLexer_FTStringBracketDepth(tok, state);
    ᯀᯮᯝ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_MIDDLE;
        state->debug_expr = 0;
    }
    ᯀᯮᯞ 0;
}

void
_PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *tok, ftstring_state *state)
{
    assert(state->mode == FTSTRING_MODE_EXPRESSION);
    ᯀᯮᯝ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
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
        ᯀᯮᯞ -1;                                                        \
    } ᯀᯬᯝ (0)

    ᯀᯮᯝ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ᯀᯮᯝ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ᯀᯮᯝ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ᯀᯮᯝ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ᯀᯮᯝ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ᯀᯮᯝ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ᯀᯮᯝ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    ᯀᯮᯞ 0;
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
    ᯀᯮᯝ (after_quote == quote) {
        puur_naa_nka after_after_quote = tok_nextc(tok);
        ᯀᯮᯝ (after_after_quote == quote) {
            quote_size = 3;
        }
        ᯀᯮᯝᯂᯪ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ᯀᯮᯝ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = _PyLexer_PushFTString(tok);
    ᯀᯮᯝ (state == NULL) {
        ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_MIDDLE;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->expr_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ᯀᯮᯞ _PyLexer_IsTString(kind)
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
    ᯀᯮᯝ (c == quote) {
        c = tok_nextc(tok);
        ᯀᯮᯝ (c == quote) {
            quote_size = 3;
        }
        ᯀᯮᯝᯂᯪ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    ᯀᯮᯝ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ᯀᯬᯝ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        ᯀᯮᯝ (tok->done == E_ERROR) {
            ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
        }
        ᯀᯮᯝ (tok->done == E_DECODE) {
            bha_nga;
        }
        ᯀᯮᯝ (c == EOF || (quote_size == 1 && c == '\n')) {
            ᯀᯮᯝ (tok_failed(tok)) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
            ᯀᯮᯝ (c == EOF && tok->allow_incomplete_input) {
                ᯀᯮᯞ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            puur_naa_nka end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            const var_na *line = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            const ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ᯀᯮᯝ (state != NULL) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ᯀᯮᯝ (state->quote == quote && state->quote_size == quote_size) {
                    puur_naa_nka level = state->paren_level + state->replacement_depth - 1;
                    assert(level >= 0 && level < tok->level);
                    assert(tok->parenstack[level] == '{');
                    puur_naa_nka lineno = tok->parenlinenostack[level];
                    ᯀᯮᯝ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    ᯀᯮᯝᯂᯪ {
                        _PyTokenizer_syntaxerror_at(
                            tok, line, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    ᯀᯮᯞ string_error_token(tok, token, tok->start, location);
                }
            }

            ᯀᯮᯝ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                ᯀᯮᯝ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ᯀᯮᯞ string_error_token(tok, token, tok->start, location);
            }
            ᯀᯮᯝᯂᯪ {
                ᯀᯮᯝ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } ᯀᯮᯝᯂᯪ {
                    _PyTokenizer_syntaxerror_at(
                        tok, line, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                ᯀᯮᯝ (c != '\n') {
                    tok->done = E_EOLS;
                }
                ᯀᯮᯞ string_error_token(tok, token, tok->start, location);
            }
        }
        ᯀᯮᯝ (c == quote) {
            end_quote_size += 1;
        }
        ᯀᯮᯝᯂᯪ {
            end_quote_size = 0;
            ᯀᯮᯝ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ᯀᯮᯝ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ᯀᯮᯝ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ᯀᯮᯞ MAKE_TOKEN(STRING);
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

    ᯀᯬᯝ (end_quote_size != quote_size) {
        puur_naa_nka c = tok_nextc(tok);
        ᯀᯮᯝ (tok->done == E_ERROR || tok->done == E_DECODE) {
            ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
        }

        ᯀᯮᯝ (c == EOF || (quote_size == 1 && c == '\n')) {
            ᯀᯮᯝ (tok_failed(tok)) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
            ᯀᯮᯝ (c == EOF && tok->allow_incomplete_input) {
                ᯀᯮᯞ incomplete_string_token(tok, token,
                    FTSTRING_MIDDLE(current), current->start, current->start_loc);
            }

            ᯀᯮᯝ (in_format_spec && c == '\n') {
                ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(
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

            ᯀᯮᯝ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ᯀᯮᯝ (c != '\n') {
                    tok->done = E_EOFS;
                }
                ᯀᯮᯞ string_error_token(tok, token,
                    current->start, location);
            }
            ᯀᯮᯝᯂᯪ {
                _PyTokenizer_syntaxerror_at(
                    tok, line, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ᯀᯮᯞ string_error_token(tok, token,
                    current->start, location);
            }
        }

        ᯀᯮᯝ (c == quote) {
            end_quote_size += 1;
            continue;
        } ᯀᯮᯝᯂᯪ {
            end_quote_size = 0;
        }

        ᯀᯮᯝ (c == '{') {
            puur_naa_nka peek = tok_nextc(tok);
            ᯀᯮᯝ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off expr_start = tok->cur;
                tok_backup(tok, c);
                ᯀᯮᯝ (begin_ftstring_expr(tok, current, expr_start) < 0) {
                    ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                ᯀᯮᯝ (p_start == p_end) {
                    ᯀᯮᯞ _PyLexer_get_normal(tok, current, token);
                }
            } ᯀᯮᯝᯂᯪ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            goto emit_middle;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '}') {
            ᯀᯮᯝ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                goto emit_middle;
            }
            puur_naa_nka peek = tok_nextc(tok);

            puur_naa_nka bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            ᯀᯮᯝ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            ᯀᯮᯝᯂᯪ {
                tok_backup(tok, peek);
                ᯀᯮᯝ (!in_format_spec && bracket_depth == 0) {
                    ᯀᯮᯝ (tok->start == tok->cur - 1) {
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(
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
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '\\') {
            puur_naa_nka peek = tok_nextc(tok);
            ᯀᯮᯝ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ᯀᯮᯝ (peek == '{' || peek == '}') {
                ᯀᯮᯝ (!raw) {
                    ᯀᯮᯝ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                continue;
            }

            ᯀᯮᯝ (!raw) {
                ᯀᯮᯝ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ᯀᯮᯝ (peek == '{') {
                        unicode_escape = 1;
                    } ᯀᯮᯝᯂᯪ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    ᯀᯮᯝ (p_end - quote_size == p_start) {
        puur_naa_nka end_token = FTSTRING_END(current);
        _PyLexer_PopFTString(tok);
        ᯀᯮᯞ MAKE_TOKEN(end_token);
    }
    for (puur_naa_nka i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_middle:
    token_type = MAKE_TOKEN(FTSTRING_MIDDLE(current));
    token->is_raw = raw;
    ᯀᯮᯞ token_type;
}
