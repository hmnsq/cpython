#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_token.h"
#અન્તર્ભાવ "errcode.h"

#અન્તર્ભાવ "lexer_internal.h"
#અન્તર્ભાવ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

સ્થિર પૂર્ણાંક
string_error_token(સંરચના tok_state *tok, સંરચના token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    પૂર્ણાંક type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    પાછા type;
}

સ્થિર પૂર્ણાંક
incomplete_string_token(સંરચના tok_state *tok, સંરચના token *token,
                        પૂર્ણાંક type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    પાછા type;
}

પૂર્ણાંક
_PyLexer_record_ftstring_comment(સંરચના tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    પ્રતિજ્ઞા(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    જો (state->પદાવલિ_span.end >= 0) {
        પાછા 0;
    }
    પ્રતિજ્ઞા(state->પદાવલિ_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    જો (comments == ખાલી || comments->ગણતરી == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)કદ(*comments)) /
            (Py_ssize_t)કદ(*comments->spans);
        જો (comments != ખાલી && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            પાછા -1;
        }
        Py_ssize_t capacity = comments == ખાલી ? 4 : comments->capacity * 2;
        size_t size = કદ(*comments) +
            (size_t)capacity * કદ(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        જો (resized == ખાલી) {
            PyErr_NoMemory();
            પાછા -1;
        }
        comments = resized;
        જો (state->comments == ખાલી) {
            comments->ગણતરી = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->ગણતરી++] =
        (_PyTok_Span){start, end};
    પાછા 0;
}

સ્થિર પૂર્ણાંક
finish_ftstring_પદાવલિ(સંરચના tok_state *tok, ftstring_state *state,
                     સંરચના token *token)
{
    પ્રતિજ્ઞા(token != ખાલી && state == _PyLexer_CurrentFTString(tok));
    પ્રતિજ્ઞા(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    જો (state->પદાવલિ_span.end >= 0) {
        પાછા 0;
    }
    પ્રતિજ્ઞા(state->પદાવલિ_span.start >= 0);
    state->પદાવલિ_span.end = tok->start;
    પૂર્ણાંક tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    જો (!(state->debug_પદાવલિ || tstring_interpolation) || token->metadata) {
        પાછા 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    જો (comments != ખાલી && comments->ગણતરી > 0) {
        Py_ssize_t stripped_size =
            state->પદાવલિ_span.end - state->પદાવલિ_span.start;
        Py_ssize_t comment_ગણતરી = 0;
        માટે (Py_ssize_t i = 0; i < comments->ગણતરી; i++) {
            _PyTok_Span comment = comments->spans[i];
            પ્રતિજ્ઞા(_PyTok_SpanIsValid(comment));
            પ્રતિજ્ઞા(comment.start >= (i == 0 ? state->પદાવલિ_span.start
                                           : comments->spans[i - 1].end));
            જો (comment.start >= state->પદાવલિ_span.end) {
                તોડો;
            }
            પ્રતિજ્ઞા(comment.end <= state->પદાવલિ_span.end);
            stripped_size -= comment.end - comment.start;
            comment_ગણતરી++;
        }
        અક્ષર *stripped = PyMem_Malloc((size_t)stripped_size);
        જો (stripped == ખાલી) {
            PyErr_NoMemory();
            પાછા -1;
        }
        _PyTok_Span kept = {state->પદાવલિ_span.start, state->પદાવલિ_span.start};
        Py_ssize_t stripped_len = 0;
        માટે (Py_ssize_t i = 0; i < comment_ગણતરી; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            અચલ અક્ષર *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->પદાવલિ_span.end;
        Py_ssize_t length;
        અચલ અક્ષર *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, ખાલી);
        PyMem_Free(stripped);
    }
    નહીંતર {
        Py_ssize_t પદાવલિ_len;
        અચલ અક્ષર *પદાવલિ = _PyTok_SourceSpanView(
            &tok->source, state->પદાવલિ_span, &પદાવલિ_len);
        res = PyUnicode_DecodeUTF8(પદાવલિ, પદાવલિ_len, ખાલી);
    }

    જો (!res) {
        પાછા -1;
    }
    token->metadata = res;
    પાછા 0;
}

સ્થિર પૂર્ણાંક
begin_ftstring_પદાવલિ(સંરચના tok_state *tok, ftstring_state *state,
                    _PyTok_Off પદાવલિ_start)
{
    પ્રતિજ્ઞા(state->mode != FTSTRING_MODE_EXPRESSION);
    state->પદાવલિ_span = (_PyTok_Span){પદાવલિ_start, -1};
    જો (state->comments != ખાલી) {
        state->comments->ગણતરી = 0;
    }
    જો (state->replacement_depth >= MAX_પદાવલિ_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        પાછા -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_પદાવલિ = 0;
    પાછા 0;
}

પૂર્ણાંક
_PyLexer_ftstring_punctuation(સંરચના tok_state *tok, ftstring_state *state,
                              સંરચના token *token, પૂર્ણાંક c)
{
    પ્રતિજ્ઞા(state->mode == FTSTRING_MODE_EXPRESSION);
    પ્રતિજ્ઞા(c == ':' || c == '}' || c == '!');
    જો (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        પાછા 0;
    }
    જો (c == '!') {
        પૂર્ણાંક next = tok_nextc(tok);
        tok_backup(tok, next);
        જો (next == '=') {
            પાછા 0;
        }
    }
    જો (finish_ftstring_પદાવલિ(tok, state, token) < 0) {
        પાછા -1;
    }
    જો (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        પાછા COLON;
    }
    પાછા 0;
}

પૂર્ણાંક
Py_Lexer_close_ftstring_પદાવલિ(સંરચના tok_state *tok, ftstring_state *state,
                             પૂર્ણાંક c)
{
    પ્રતિજ્ઞા(state->mode == FTSTRING_MODE_EXPRESSION);
    પ્રતિજ્ઞા(c == ')' || c == ']' || c == '}');
    પૂર્ણાંક depth = _PyLexer_FTStringBracketDepth(tok, state);
    જો (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        પાછા -1;
    }
    જો (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_મધ્ય;
        state->debug_પદાવલિ = 0;
    }
    પાછા 0;
}

શૂન્ય
_PyLexer_mark_ftstring_debug(સંરચના tok_state *tok, ftstring_state *state)
{
    પ્રતિજ્ઞા(state->mode == FTSTRING_MODE_EXPRESSION);
    જો (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_પદાવલિ = 1;
    }
}

પૂર્ણાંક
_PyLexer_check_string_prefixes(સંરચના tok_state *tok,
                                             પૂર્ણાંક saw_b, પૂર્ણાંક saw_r, પૂર્ણાંક saw_u,
                                             પૂર્ણાંક saw_f, પૂર્ણાંક saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    કરો {                                                                  \
        (શૂન્ય)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (પૂર્ણાંક)(tok->start + 1 - tok->લીટી_start),                 \
            (પૂર્ણાંક)(tok->cur - tok->લીટી_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        પાછા -1;                                                        \
    } જ્યારે (0)

    જો (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    જો (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    જો (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    જો (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    જો (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    જો (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    જો (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    પાછા 0;
}

પૂર્ણાંક
_PyLexer_scan_fstring_start(સંરચના tok_state *tok, સંરચના token *token,
                            પૂર્ણાંક c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    પૂર્ણાંક quote = c;
    પૂર્ણાંક quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    પૂર્ણાંક after_quote = tok_nextc(tok);
    જો (after_quote == quote) {
        પૂર્ણાંક after_after_quote = tok_nextc(tok);
        જો (after_after_quote == quote) {
            quote_size = 3;
        }
        નહીંતર {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    જો (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_ધકેલો_FTString(tok);
    જો (state == ખાલી) {
        પાછા MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_મધ્ય;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->પદાવલિ_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    પાછા _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

પૂર્ણાંક
_PyLexer_scan_string(સંરચના tok_state *tok, સંરચના token *token, પૂર્ણાંક c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    પૂર્ણાંક quote = c;
    પૂર્ણાંક quote_size = 1;             /* 1 or 3 */
    પૂર્ણાંક end_quote_size = 0;
    પૂર્ણાંક has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    જો (c == quote) {
        c = tok_nextc(tok);
        જો (c == quote) {
            quote_size = 3;
        }
        નહીંતર {
            end_quote_size = 1;     /* empty string found */
        }
    }
    જો (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    જ્યારે (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        જો (tok->done == E_ERROR) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
        જો (tok->done == E_DECODE) {
            તોડો;
        }
        જો (c == EOF || (quote_size == 1 && c == '\n')) {
            જો (tok_failed(tok)) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
            જો (c == EOF && tok->allow_incomplete_આગત) {
                પાછા incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            પૂર્ણાંક end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            અચલ અક્ષર *લીટી = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            અચલ ftstring_state *state = _PyLexer_CurrentFTString(tok);
            જો (state != ખાલી) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                જો (state->quote == quote && state->quote_size == quote_size) {
                    પૂર્ણાંક level = state->paren_level + state->replacement_depth - 1;
                    પ્રતિજ્ઞા(level >= 0 && level < tok->level);
                    પ્રતિજ્ઞા(tok->parenstack[level] == '{');
                    પૂર્ણાંક lineno = tok->parenlinenostack[level];
                    જો (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, લીટી, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    નહીંતર {
                        _PyTokenizer_syntaxerror_at(
                            tok, લીટી, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    પાછા string_error_token(tok, token, tok->start, location);
                }
            }

            જો (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, લીટી, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                જો (c != '\n') {
                    tok->done = E_EOFS;
                }
                પાછા string_error_token(tok, token, tok->start, location);
            }
            નહીંતર {
                જો (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, લીટી, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } નહીંતર {
                    _PyTokenizer_syntaxerror_at(
                        tok, લીટી, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                જો (c != '\n') {
                    tok->done = E_EOLS;
                }
                પાછા string_error_token(tok, token, tok->start, location);
            }
        }
        જો (c == quote) {
            end_quote_size += 1;
        }
        નહીંતર {
            end_quote_size = 0;
            જો (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                જો (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                જો (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    પાછા MAKE_TOKEN(STRING);
}

પૂર્ણાંક
_PyLexer_get_ftstring(સંરચના tok_state *tok, ftstring_state *current, સંરચના token *token)
{
    પ્રતિજ્ઞા(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    પ્રતિજ્ઞા((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_પદાવલિ_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    પૂર્ણાંક token_type;
    પૂર્ણાંક end_quote_size = 0;
    પૂર્ણાંક unicode_escape = 0;
    પૂર્ણાંક quote = current->quote;
    પૂર્ણાંક quote_size = current->quote_size;
    પૂર્ણાંક in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    પૂર્ણાંક raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    જ્યારે (end_quote_size != quote_size) {
        પૂર્ણાંક c = tok_nextc(tok);
        જો (tok->done == E_ERROR || tok->done == E_DECODE) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }

        જો (c == EOF || (quote_size == 1 && c == '\n')) {
            જો (tok_failed(tok)) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
            જો (c == EOF && tok->allow_incomplete_આગત) {
                પાછા incomplete_string_token(tok, token,
                    FTSTRING_મધ્ય(current), current->start, current->start_loc);
            }

            જો (in_format_spec && c == '\n') {
                પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            પૂર્ણાંક end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            અચલ અક્ષર *લીટી = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            જો (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, લીટી, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                જો (c != '\n') {
                    tok->done = E_EOFS;
                }
                પાછા string_error_token(tok, token,
                    current->start, location);
            }
            નહીંતર {
                _PyTokenizer_syntaxerror_at(
                    tok, લીટી, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                પાછા string_error_token(tok, token,
                    current->start, location);
            }
        }

        જો (c == quote) {
            end_quote_size += 1;
            ચાલુ;
        } નહીંતર {
            end_quote_size = 0;
        }

        જો (c == '{') {
            પૂર્ણાંક peek = tok_nextc(tok);
            જો (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off પદાવલિ_start = tok->cur;
                tok_backup(tok, c);
                જો (begin_ftstring_પદાવલિ(tok, current, પદાવલિ_start) < 0) {
                    પાછા MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                જો (p_start == p_end) {
                    પાછા _PyLexer_get_normal(tok, current, token);
                }
            } નહીંતર {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            જાઓ emit_મધ્ય;
        } નહીંતર જો (c == '}') {
            જો (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                જાઓ emit_મધ્ય;
            }
            પૂર્ણાંક peek = tok_nextc(tok);

            પૂર્ણાંક bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            જો (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            નહીંતર {
                tok_backup(tok, peek);
                જો (!in_format_spec && bracket_depth == 0) {
                    જો (tok->start == tok->cur - 1) {
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    જાઓ emit_મધ્ય;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            જાઓ emit_મધ્ય;
        } નહીંતર જો (c == '\\') {
            પૂર્ણાંક peek = tok_nextc(tok);
            જો (peek == '\r') {
                peek = tok_nextc(tok);
            }
            જો (peek == '{' || peek == '}') {
                જો (!raw) {
                    જો (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        પાછા MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                ચાલુ;
            }

            જો (!raw) {
                જો (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    જો (peek == '{') {
                        unicode_escape = 1;
                    } નહીંતર {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    જો (p_end - quote_size == p_start) {
        પૂર્ણાંક end_token = FTSTRING_END(current);
        Py_Lexer_કાઢો_FTString(tok);
        પાછા MAKE_TOKEN(end_token);
    }
    માટે (પૂર્ણાંક i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_મધ્ય:
    token_type = MAKE_TOKEN(FTSTRING_મધ્ય(current));
    token->is_raw = raw;
    પાછા token_type;
}
