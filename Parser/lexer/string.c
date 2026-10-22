#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_token.h"
#அந்தர்பாவ "errcode.h"

#அந்தர்பாவ "lexer_internal.h"
#அந்தர்பாவ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

நிலையான முழு
string_error_token(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    முழு type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    திரும்பு type;
}

நிலையான முழு
incomplete_string_token(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token,
                        முழு type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    திரும்பு type;
}

முழு
_PyLexer_record_ftstring_comment(கட்டமைப்பு tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    ப்ரதிஜ்ஞா(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    எனில் (state->கோவை_span.end >= 0) {
        திரும்பு 0;
    }
    ப்ரதிஜ்ஞா(state->கோவை_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    எனில் (comments == வெற்று || comments->மொத்தம் == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)அளவு(*comments)) /
            (Py_ssize_t)அளவு(*comments->spans);
        எனில் (comments != வெற்று && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            திரும்பு -1;
        }
        Py_ssize_t capacity = comments == வெற்று ? 4 : comments->capacity * 2;
        size_t size = அளவு(*comments) +
            (size_t)capacity * அளவு(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        எனில் (resized == வெற்று) {
            PyErr_NoMemory();
            திரும்பு -1;
        }
        comments = resized;
        எனில் (state->comments == வெற்று) {
            comments->மொத்தம் = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->மொத்தம்++] =
        (_PyTok_Span){start, end};
    திரும்பு 0;
}

நிலையான முழு
finish_ftstring_கோவை(கட்டமைப்பு tok_state *tok, ftstring_state *state,
                     கட்டமைப்பு token *token)
{
    ப்ரதிஜ்ஞா(token != வெற்று && state == _PyLexer_CurrentFTString(tok));
    ப்ரதிஜ்ஞா(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    எனில் (state->கோவை_span.end >= 0) {
        திரும்பு 0;
    }
    ப்ரதிஜ்ஞா(state->கோவை_span.start >= 0);
    state->கோவை_span.end = tok->start;
    முழு tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    எனில் (!(state->debug_கோவை || tstring_interpolation) || token->metadata) {
        திரும்பு 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    எனில் (comments != வெற்று && comments->மொத்தம் > 0) {
        Py_ssize_t stripped_size =
            state->கோவை_span.end - state->கோவை_span.start;
        Py_ssize_t comment_மொத்தம் = 0;
        க்கு (Py_ssize_t i = 0; i < comments->மொத்தம்; i++) {
            _PyTok_Span comment = comments->spans[i];
            ப்ரதிஜ்ஞா(_PyTok_SpanIsValid(comment));
            ப்ரதிஜ்ஞா(comment.start >= (i == 0 ? state->கோவை_span.start
                                           : comments->spans[i - 1].end));
            எனில் (comment.start >= state->கோவை_span.end) {
                நிறுத்து;
            }
            ப்ரதிஜ்ஞா(comment.end <= state->கோவை_span.end);
            stripped_size -= comment.end - comment.start;
            comment_மொத்தம்++;
        }
        எழுத்து *stripped = PyMem_Malloc((size_t)stripped_size);
        எனில் (stripped == வெற்று) {
            PyErr_NoMemory();
            திரும்பு -1;
        }
        _PyTok_Span kept = {state->கோவை_span.start, state->கோவை_span.start};
        Py_ssize_t stripped_len = 0;
        க்கு (Py_ssize_t i = 0; i < comment_மொத்தம்; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            மாறிலி எழுத்து *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->கோவை_span.end;
        Py_ssize_t length;
        மாறிலி எழுத்து *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, வெற்று);
        PyMem_Free(stripped);
    }
    இல்லையெனில் {
        Py_ssize_t கோவை_len;
        மாறிலி எழுத்து *கோவை = _PyTok_SourceSpanView(
            &tok->source, state->கோவை_span, &கோவை_len);
        res = PyUnicode_DecodeUTF8(கோவை, கோவை_len, வெற்று);
    }

    எனில் (!res) {
        திரும்பு -1;
    }
    token->metadata = res;
    திரும்பு 0;
}

நிலையான முழு
begin_ftstring_கோவை(கட்டமைப்பு tok_state *tok, ftstring_state *state,
                    _PyTok_Off கோவை_start)
{
    ப்ரதிஜ்ஞா(state->mode != FTSTRING_MODE_EXPRESSION);
    state->கோவை_span = (_PyTok_Span){கோவை_start, -1};
    எனில் (state->comments != வெற்று) {
        state->comments->மொத்தம் = 0;
    }
    எனில் (state->replacement_depth >= MAX_கோவை_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        திரும்பு -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_கோவை = 0;
    திரும்பு 0;
}

முழு
_PyLexer_ftstring_punctuation(கட்டமைப்பு tok_state *tok, ftstring_state *state,
                              கட்டமைப்பு token *token, முழு c)
{
    ப்ரதிஜ்ஞா(state->mode == FTSTRING_MODE_EXPRESSION);
    ப்ரதிஜ்ஞா(c == ':' || c == '}' || c == '!');
    எனில் (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        திரும்பு 0;
    }
    எனில் (c == '!') {
        முழு next = tok_nextc(tok);
        tok_backup(tok, next);
        எனில் (next == '=') {
            திரும்பு 0;
        }
    }
    எனில் (finish_ftstring_கோவை(tok, state, token) < 0) {
        திரும்பு -1;
    }
    எனில் (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        திரும்பு COLON;
    }
    திரும்பு 0;
}

முழு
Py_Lexer_close_ftstring_கோவை(கட்டமைப்பு tok_state *tok, ftstring_state *state,
                             முழு c)
{
    ப்ரதிஜ்ஞா(state->mode == FTSTRING_MODE_EXPRESSION);
    ப்ரதிஜ்ஞா(c == ')' || c == ']' || c == '}');
    முழு depth = _PyLexer_FTStringBracketDepth(tok, state);
    எனில் (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        திரும்பு -1;
    }
    எனில் (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_நடு;
        state->debug_கோவை = 0;
    }
    திரும்பு 0;
}

வெறுமை
_PyLexer_mark_ftstring_debug(கட்டமைப்பு tok_state *tok, ftstring_state *state)
{
    ப்ரதிஜ்ஞா(state->mode == FTSTRING_MODE_EXPRESSION);
    எனில் (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_கோவை = 1;
    }
}

முழு
_PyLexer_check_string_prefixes(கட்டமைப்பு tok_state *tok,
                                             முழு saw_b, முழு saw_r, முழு saw_u,
                                             முழு saw_f, முழு saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    செய் {                                                                  \
        (வெறுமை)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (முழு)(tok->start + 1 - tok->வரி_start),                 \
            (முழு)(tok->cur - tok->வரி_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        திரும்பு -1;                                                        \
    } போது (0)

    எனில் (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    எனில் (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    எனில் (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    எனில் (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    எனில் (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    எனில் (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    எனில் (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    திரும்பு 0;
}

முழு
_PyLexer_scan_fstring_start(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token,
                            முழு c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    முழு quote = c;
    முழு quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    முழு after_quote = tok_nextc(tok);
    எனில் (after_quote == quote) {
        முழு after_after_quote = tok_nextc(tok);
        எனில் (after_after_quote == quote) {
            quote_size = 3;
        }
        இல்லையெனில் {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    எனில் (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_தள்ளு_FTString(tok);
    எனில் (state == வெற்று) {
        திரும்பு MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_நடு;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->கோவை_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    திரும்பு _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

முழு
_PyLexer_scan_string(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token, முழு c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    முழு quote = c;
    முழு quote_size = 1;             /* 1 or 3 */
    முழு end_quote_size = 0;
    முழு has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    எனில் (c == quote) {
        c = tok_nextc(tok);
        எனில் (c == quote) {
            quote_size = 3;
        }
        இல்லையெனில் {
            end_quote_size = 1;     /* empty string found */
        }
    }
    எனில் (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    போது (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        எனில் (tok->done == E_ERROR) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
        எனில் (tok->done == E_DECODE) {
            நிறுத்து;
        }
        எனில் (c == EOF || (quote_size == 1 && c == '\n')) {
            எனில் (tok_failed(tok)) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
            எனில் (c == EOF && tok->allow_incomplete_உள்ளீடு) {
                திரும்பு incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            முழு end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            மாறிலி எழுத்து *வரி = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            மாறிலி ftstring_state *state = _PyLexer_CurrentFTString(tok);
            எனில் (state != வெற்று) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                எனில் (state->quote == quote && state->quote_size == quote_size) {
                    முழு level = state->paren_level + state->replacement_depth - 1;
                    ப்ரதிஜ்ஞா(level >= 0 && level < tok->level);
                    ப்ரதிஜ்ஞா(tok->parenstack[level] == '{');
                    முழு lineno = tok->parenlinenostack[level];
                    எனில் (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, வரி, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    இல்லையெனில் {
                        _PyTokenizer_syntaxerror_at(
                            tok, வரி, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    திரும்பு string_error_token(tok, token, tok->start, location);
                }
            }

            எனில் (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, வரி, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                எனில் (c != '\n') {
                    tok->done = E_EOFS;
                }
                திரும்பு string_error_token(tok, token, tok->start, location);
            }
            இல்லையெனில் {
                எனில் (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, வரி, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } இல்லையெனில் {
                    _PyTokenizer_syntaxerror_at(
                        tok, வரி, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                எனில் (c != '\n') {
                    tok->done = E_EOLS;
                }
                திரும்பு string_error_token(tok, token, tok->start, location);
            }
        }
        எனில் (c == quote) {
            end_quote_size += 1;
        }
        இல்லையெனில் {
            end_quote_size = 0;
            எனில் (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                எனில் (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                எனில் (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    திரும்பு MAKE_TOKEN(STRING);
}

முழு
_PyLexer_get_ftstring(கட்டமைப்பு tok_state *tok, ftstring_state *current, கட்டமைப்பு token *token)
{
    ப்ரதிஜ்ஞா(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    ப்ரதிஜ்ஞா((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_கோவை_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    முழு token_type;
    முழு end_quote_size = 0;
    முழு unicode_escape = 0;
    முழு quote = current->quote;
    முழு quote_size = current->quote_size;
    முழு in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    முழு raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    போது (end_quote_size != quote_size) {
        முழு c = tok_nextc(tok);
        எனில் (tok->done == E_ERROR || tok->done == E_DECODE) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }

        எனில் (c == EOF || (quote_size == 1 && c == '\n')) {
            எனில் (tok_failed(tok)) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
            எனில் (c == EOF && tok->allow_incomplete_உள்ளீடு) {
                திரும்பு incomplete_string_token(tok, token,
                    FTSTRING_நடு(current), current->start, current->start_loc);
            }

            எனில் (in_format_spec && c == '\n') {
                திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            முழு end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            மாறிலி எழுத்து *வரி = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            எனில் (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, வரி, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                எனில் (c != '\n') {
                    tok->done = E_EOFS;
                }
                திரும்பு string_error_token(tok, token,
                    current->start, location);
            }
            இல்லையெனில் {
                _PyTokenizer_syntaxerror_at(
                    tok, வரி, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                திரும்பு string_error_token(tok, token,
                    current->start, location);
            }
        }

        எனில் (c == quote) {
            end_quote_size += 1;
            தொடர்;
        } இல்லையெனில் {
            end_quote_size = 0;
        }

        எனில் (c == '{') {
            முழு peek = tok_nextc(tok);
            எனில் (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off கோவை_start = tok->cur;
                tok_backup(tok, c);
                எனில் (begin_ftstring_கோவை(tok, current, கோவை_start) < 0) {
                    திரும்பு MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                எனில் (p_start == p_end) {
                    திரும்பு _PyLexer_get_normal(tok, current, token);
                }
            } இல்லையெனில் {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            செல் emit_நடு;
        } இல்லையெனில் எனில் (c == '}') {
            எனில் (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                செல் emit_நடு;
            }
            முழு peek = tok_nextc(tok);

            முழு bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            எனில் (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            இல்லையெனில் {
                tok_backup(tok, peek);
                எனில் (!in_format_spec && bracket_depth == 0) {
                    எனில் (tok->start == tok->cur - 1) {
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    செல் emit_நடு;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            செல் emit_நடு;
        } இல்லையெனில் எனில் (c == '\\') {
            முழு peek = tok_nextc(tok);
            எனில் (peek == '\r') {
                peek = tok_nextc(tok);
            }
            எனில் (peek == '{' || peek == '}') {
                எனில் (!raw) {
                    எனில் (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        திரும்பு MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                தொடர்;
            }

            எனில் (!raw) {
                எனில் (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    எனில் (peek == '{') {
                        unicode_escape = 1;
                    } இல்லையெனில் {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    எனில் (p_end - quote_size == p_start) {
        முழு end_token = FTSTRING_END(current);
        Py_Lexer_எடு_FTString(tok);
        திரும்பு MAKE_TOKEN(end_token);
    }
    க்கு (முழு i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_நடு:
    token_type = MAKE_TOKEN(FTSTRING_நடு(current));
    token->is_raw = raw;
    திரும்பு token_type;
}
