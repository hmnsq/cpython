#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ساكن صحيح
string_error_token(بنية tok_state *tok, بنية token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    صحيح type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    أرجع type;
}

ساكن صحيح
incomplete_string_token(بنية tok_state *tok, بنية token *token,
                        صحيح type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    أرجع type;
}

صحيح
_PyLexer_record_ftstring_comment(بنية tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    إذا (state->اوڠکاڤن_span.end >= 0) {
        أرجع 0;
    }
    أكد(state->اوڠکاڤن_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    إذا (comments == عدم || comments->کيراءن == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)حجم(*comments)) /
            (Py_ssize_t)حجم(*comments->spans);
        إذا (comments != عدم && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            أرجع -1;
        }
        Py_ssize_t capacity = comments == عدم ? 4 : comments->capacity * 2;
        size_t size = حجم(*comments) +
            (size_t)capacity * حجم(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        إذا (resized == عدم) {
            PyErr_NoMemory();
            أرجع -1;
        }
        comments = resized;
        إذا (state->comments == عدم) {
            comments->کيراءن = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->کيراءن++] =
        (_PyTok_Span){start, end};
    أرجع 0;
}

ساكن صحيح
finish_ftstring_اوڠکاڤن(بنية tok_state *tok, ftstring_state *state,
                     بنية token *token)
{
    أكد(token != عدم && state == _PyLexer_CurrentFTString(tok));
    أكد(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    إذا (state->اوڠکاڤن_span.end >= 0) {
        أرجع 0;
    }
    أكد(state->اوڠکاڤن_span.start >= 0);
    state->اوڠکاڤن_span.end = tok->start;
    صحيح tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    إذا (!(state->debug_اوڠکاڤن || tstring_interpolation) || token->metadata) {
        أرجع 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    إذا (comments != عدم && comments->کيراءن > 0) {
        Py_ssize_t stripped_size =
            state->اوڠکاڤن_span.end - state->اوڠکاڤن_span.start;
        Py_ssize_t comment_کيراءن = 0;
        لكل (Py_ssize_t i = 0; i < comments->کيراءن; i++) {
            _PyTok_Span comment = comments->spans[i];
            أكد(_PyTok_SpanIsValid(comment));
            أكد(comment.start >= (i == 0 ? state->اوڠکاڤن_span.start
                                           : comments->spans[i - 1].end));
            إذا (comment.start >= state->اوڠکاڤن_span.end) {
                اكسر;
            }
            أكد(comment.end <= state->اوڠکاڤن_span.end);
            stripped_size -= comment.end - comment.start;
            comment_کيراءن++;
        }
        حرف *stripped = PyMem_Malloc((size_t)stripped_size);
        إذا (stripped == عدم) {
            PyErr_NoMemory();
            أرجع -1;
        }
        _PyTok_Span kept = {state->اوڠکاڤن_span.start, state->اوڠکاڤن_span.start};
        Py_ssize_t stripped_len = 0;
        لكل (Py_ssize_t i = 0; i < comment_کيراءن; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ثابت حرف *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->اوڠکاڤن_span.end;
        Py_ssize_t length;
        ثابت حرف *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, عدم);
        PyMem_Free(stripped);
    }
    وإلا {
        Py_ssize_t اوڠکاڤن_len;
        ثابت حرف *اوڠکاڤن = _PyTok_SourceSpanView(
            &tok->source, state->اوڠکاڤن_span, &اوڠکاڤن_len);
        res = PyUnicode_DecodeUTF8(اوڠکاڤن, اوڠکاڤن_len, عدم);
    }

    إذا (!res) {
        أرجع -1;
    }
    token->metadata = res;
    أرجع 0;
}

ساكن صحيح
begin_ftstring_اوڠکاڤن(بنية tok_state *tok, ftstring_state *state,
                    _PyTok_Off اوڠکاڤن_start)
{
    أكد(state->mode != FTSTRING_MODE_EXPRESSION);
    state->اوڠکاڤن_span = (_PyTok_Span){اوڠکاڤن_start, -1};
    إذا (state->comments != عدم) {
        state->comments->کيراءن = 0;
    }
    إذا (state->replacement_depth >= MAX_اوڠکاڤن_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        أرجع -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_اوڠکاڤن = 0;
    أرجع 0;
}

صحيح
_PyLexer_ftstring_punctuation(بنية tok_state *tok, ftstring_state *state,
                              بنية token *token, صحيح c)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    أكد(c == ':' || c == '}' || c == '!');
    إذا (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        أرجع 0;
    }
    إذا (c == '!') {
        صحيح next = tok_nextc(tok);
        tok_backup(tok, next);
        إذا (next == '=') {
            أرجع 0;
        }
    }
    إذا (finish_ftstring_اوڠکاڤن(tok, state, token) < 0) {
        أرجع -1;
    }
    إذا (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        أرجع COLON;
    }
    أرجع 0;
}

صحيح
Py_Lexer_close_ftstring_اوڠکاڤن(بنية tok_state *tok, ftstring_state *state,
                             صحيح c)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    أكد(c == ')' || c == ']' || c == '}');
    صحيح depth = _PyLexer_FTStringBracketDepth(tok, state);
    إذا (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        أرجع -1;
    }
    إذا (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_تڠه;
        state->debug_اوڠکاڤن = 0;
    }
    أرجع 0;
}

فراغ
_PyLexer_mark_ftstring_debug(بنية tok_state *tok, ftstring_state *state)
{
    أكد(state->mode == FTSTRING_MODE_EXPRESSION);
    إذا (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_اوڠکاڤن = 1;
    }
}

صحيح
_PyLexer_check_string_prefixes(بنية tok_state *tok,
                                             صحيح saw_b, صحيح saw_r, صحيح saw_u,
                                             صحيح saw_f, صحيح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    افعل {                                                                  \
        (فراغ)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (صحيح)(tok->start + 1 - tok->باريس_start),                 \
            (صحيح)(tok->cur - tok->باريس_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        أرجع -1;                                                        \
    } طالما (0)

    إذا (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    إذا (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    إذا (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    إذا (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    إذا (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    إذا (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    إذا (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    أرجع 0;
}

صحيح
_PyLexer_scan_fstring_start(بنية tok_state *tok, بنية token *token,
                            صحيح c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    صحيح quote = c;
    صحيح quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحيح after_quote = tok_nextc(tok);
    إذا (after_quote == quote) {
        صحيح after_after_quote = tok_nextc(tok);
        إذا (after_after_quote == quote) {
            quote_size = 3;
        }
        وإلا {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    إذا (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_تولق_FTString(tok);
    إذا (state == عدم) {
        أرجع MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_تڠه;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->اوڠکاڤن_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    أرجع _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

صحيح
_PyLexer_scan_string(بنية tok_state *tok, بنية token *token, صحيح c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    صحيح quote = c;
    صحيح quote_size = 1;             /* 1 or 3 */
    صحيح end_quote_size = 0;
    صحيح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    إذا (c == quote) {
        c = tok_nextc(tok);
        إذا (c == quote) {
            quote_size = 3;
        }
        وإلا {
            end_quote_size = 1;     /* empty string found */
        }
    }
    إذا (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    طالما (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        إذا (tok->done == E_ERROR) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        إذا (tok->done == E_DECODE) {
            اكسر;
        }
        إذا (c == EOF || (quote_size == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_ماسوقن) {
                أرجع incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            صحيح end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ثابت حرف *باريس = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            إذا (state != عدم) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                إذا (state->quote == quote && state->quote_size == quote_size) {
                    صحيح level = state->paren_level + state->replacement_depth - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحيح lineno = tok->parenlinenostack[level];
                    إذا (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, باريس, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    وإلا {
                        _PyTokenizer_syntaxerror_at(
                            tok, باريس, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    أرجع string_error_token(tok, token, tok->start, location);
                }
            }

            إذا (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, باريس, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                إذا (c != '\n') {
                    tok->done = E_EOFS;
                }
                أرجع string_error_token(tok, token, tok->start, location);
            }
            وإلا {
                إذا (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, باريس, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } وإلا {
                    _PyTokenizer_syntaxerror_at(
                        tok, باريس, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                إذا (c != '\n') {
                    tok->done = E_EOLS;
                }
                أرجع string_error_token(tok, token, tok->start, location);
            }
        }
        إذا (c == quote) {
            end_quote_size += 1;
        }
        وإلا {
            end_quote_size = 0;
            إذا (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                إذا (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                إذا (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    أرجع MAKE_TOKEN(STRING);
}

صحيح
_PyLexer_get_ftstring(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    أكد((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_اوڠکاڤن_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    صحيح token_type;
    صحيح end_quote_size = 0;
    صحيح unicode_escape = 0;
    صحيح quote = current->quote;
    صحيح quote_size = current->quote_size;
    صحيح in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    صحيح raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    طالما (end_quote_size != quote_size) {
        صحيح c = tok_nextc(tok);
        إذا (tok->done == E_ERROR || tok->done == E_DECODE) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        إذا (c == EOF || (quote_size == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_ماسوقن) {
                أرجع incomplete_string_token(tok, token,
                    FTSTRING_تڠه(current), current->start, current->start_loc);
            }

            إذا (in_format_spec && c == '\n') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            صحيح end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ثابت حرف *باريس = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            إذا (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, باريس, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                إذا (c != '\n') {
                    tok->done = E_EOFS;
                }
                أرجع string_error_token(tok, token,
                    current->start, location);
            }
            وإلا {
                _PyTokenizer_syntaxerror_at(
                    tok, باريس, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                أرجع string_error_token(tok, token,
                    current->start, location);
            }
        }

        إذا (c == quote) {
            end_quote_size += 1;
            استمر;
        } وإلا {
            end_quote_size = 0;
        }

        إذا (c == '{') {
            صحيح peek = tok_nextc(tok);
            إذا (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off اوڠکاڤن_start = tok->cur;
                tok_backup(tok, c);
                إذا (begin_ftstring_اوڠکاڤن(tok, current, اوڠکاڤن_start) < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                إذا (p_start == p_end) {
                    أرجع _PyLexer_get_normal(tok, current, token);
                }
            } وإلا {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            اذهب emit_تڠه;
        } وإلا إذا (c == '}') {
            إذا (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                اذهب emit_تڠه;
            }
            صحيح peek = tok_nextc(tok);

            صحيح bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            إذا (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            وإلا {
                tok_backup(tok, peek);
                إذا (!in_format_spec && bracket_depth == 0) {
                    إذا (tok->start == tok->cur - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    اذهب emit_تڠه;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            اذهب emit_تڠه;
        } وإلا إذا (c == '\\') {
            صحيح peek = tok_nextc(tok);
            إذا (peek == '\r') {
                peek = tok_nextc(tok);
            }
            إذا (peek == '{' || peek == '}') {
                إذا (!raw) {
                    إذا (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        أرجع MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                استمر;
            }

            إذا (!raw) {
                إذا (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    إذا (peek == '{') {
                        unicode_escape = 1;
                    } وإلا {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    إذا (p_end - quote_size == p_start) {
        صحيح end_token = FTSTRING_END(current);
        Py_Lexer_کلوار_FTString(tok);
        أرجع MAKE_TOKEN(end_token);
    }
    لكل (صحيح i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_تڠه:
    token_type = MAKE_TOKEN(FTSTRING_تڠه(current));
    token->is_raw = raw;
    أرجع token_type;
}
