#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

स्थिर पूर्णाङ्क
string_error_token(संरचना tok_state *tok, संरचना token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    पूर्णाङ्क type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    पटिनिवत्त type;
}

स्थिर पूर्णाङ्क
incomplete_string_token(संरचना tok_state *tok, संरचना token *token,
                        पूर्णाङ्क type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    पटिनिवत्त type;
}

पूर्णाङ्क
_PyLexer_record_ftstring_comment(संरचना tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    प्रतिज्ञा(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    सचे (state->व्यञ्जकम्_span.end >= 0) {
        पटिनिवत्त 0;
    }
    प्रतिज्ञा(state->व्यञ्जकम्_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    सचे (comments == रिक्त || comments->गणनम् == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)परिमाण(*comments)) /
            (Py_ssize_t)परिमाण(*comments->spans);
        सचे (comments != रिक्त && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            पटिनिवत्त -1;
        }
        Py_ssize_t capacity = comments == रिक्त ? 4 : comments->capacity * 2;
        size_t size = परिमाण(*comments) +
            (size_t)capacity * परिमाण(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        सचे (resized == रिक्त) {
            PyErr_NoMemory();
            पटिनिवत्त -1;
        }
        comments = resized;
        सचे (state->comments == रिक्त) {
            comments->गणनम् = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->गणनम्++] =
        (_PyTok_Span){start, end};
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
finish_ftstring_व्यञ्जकम्(संरचना tok_state *tok, ftstring_state *state,
                     संरचना token *token)
{
    प्रतिज्ञा(token != रिक्त && state == _PyLexer_CurrentFTString(tok));
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    सचे (state->व्यञ्जकम्_span.end >= 0) {
        पटिनिवत्त 0;
    }
    प्रतिज्ञा(state->व्यञ्जकम्_span.start >= 0);
    state->व्यञ्जकम्_span.end = tok->start;
    पूर्णाङ्क tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    सचे (!(state->debug_व्यञ्जकम् || tstring_interpolation) || token->metadata) {
        पटिनिवत्त 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    सचे (comments != रिक्त && comments->गणनम् > 0) {
        Py_ssize_t stripped_size =
            state->व्यञ्जकम्_span.end - state->व्यञ्जकम्_span.start;
        Py_ssize_t comment_गणनम् = 0;
        अत्थाय (Py_ssize_t i = 0; i < comments->गणनम्; i++) {
            _PyTok_Span comment = comments->spans[i];
            प्रतिज्ञा(_PyTok_SpanIsValid(comment));
            प्रतिज्ञा(comment.start >= (i == 0 ? state->व्यञ्जकम्_span.start
                                           : comments->spans[i - 1].end));
            सचे (comment.start >= state->व्यञ्जकम्_span.end) {
                भङ्ग;
            }
            प्रतिज्ञा(comment.end <= state->व्यञ्जकम्_span.end);
            stripped_size -= comment.end - comment.start;
            comment_गणनम्++;
        }
        वर्ण *stripped = PyMem_Malloc((size_t)stripped_size);
        सचे (stripped == रिक्त) {
            PyErr_NoMemory();
            पटिनिवत्त -1;
        }
        _PyTok_Span kept = {state->व्यञ्जकम्_span.start, state->व्यञ्जकम्_span.start};
        Py_ssize_t stripped_len = 0;
        अत्थाय (Py_ssize_t i = 0; i < comment_गणनम्; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ध्रुव वर्ण *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->व्यञ्जकम्_span.end;
        Py_ssize_t length;
        ध्रुव वर्ण *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, रिक्त);
        PyMem_Free(stripped);
    }
    अञ्ञथा {
        Py_ssize_t व्यञ्जकम्_len;
        ध्रुव वर्ण *व्यञ्जकम् = _PyTok_SourceSpanView(
            &tok->source, state->व्यञ्जकम्_span, &व्यञ्जकम्_len);
        res = PyUnicode_DecodeUTF8(व्यञ्जकम्, व्यञ्जकम्_len, रिक्त);
    }

    सचे (!res) {
        पटिनिवत्त -1;
    }
    token->metadata = res;
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
begin_ftstring_व्यञ्जकम्(संरचना tok_state *tok, ftstring_state *state,
                    _PyTok_Off व्यञ्जकम्_start)
{
    प्रतिज्ञा(state->mode != FTSTRING_MODE_EXPRESSION);
    state->व्यञ्जकम्_span = (_PyTok_Span){व्यञ्जकम्_start, -1};
    सचे (state->comments != रिक्त) {
        state->comments->गणनम् = 0;
    }
    सचे (state->replacement_depth >= MAX_व्यञ्जकम्_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        पटिनिवत्त -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_व्यञ्जकम् = 0;
    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyLexer_ftstring_punctuation(संरचना tok_state *tok, ftstring_state *state,
                              संरचना token *token, पूर्णाङ्क c)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा(c == ':' || c == '}' || c == '!');
    सचे (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        पटिनिवत्त 0;
    }
    सचे (c == '!') {
        पूर्णाङ्क next = tok_nextc(tok);
        tok_backup(tok, next);
        सचे (next == '=') {
            पटिनिवत्त 0;
        }
    }
    सचे (finish_ftstring_व्यञ्जकम्(tok, state, token) < 0) {
        पटिनिवत्त -1;
    }
    सचे (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        पटिनिवत्त COLON;
    }
    पटिनिवत्त 0;
}

पूर्णाङ्क
Py_Lexer_close_ftstring_व्यञ्जकम्(संरचना tok_state *tok, ftstring_state *state,
                             पूर्णाङ्क c)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा(c == ')' || c == ']' || c == '}');
    पूर्णाङ्क depth = _PyLexer_FTStringBracketDepth(tok, state);
    सचे (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        पटिनिवत्त -1;
    }
    सचे (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_मध्यः;
        state->debug_व्यञ्जकम् = 0;
    }
    पटिनिवत्त 0;
}

शून्य
_PyLexer_mark_ftstring_debug(संरचना tok_state *tok, ftstring_state *state)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    सचे (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_व्यञ्जकम् = 1;
    }
}

पूर्णाङ्क
_PyLexer_check_string_prefixes(संरचना tok_state *tok,
                                             पूर्णाङ्क saw_b, पूर्णाङ्क saw_r, पूर्णाङ्क saw_u,
                                             पूर्णाङ्क saw_f, पूर्णाङ्क saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    करोहि {                                                                  \
        (शून्य)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (पूर्णाङ्क)(tok->start + 1 - tok->पङ्क्तिः_start),                 \
            (पूर्णाङ्क)(tok->cur - tok->पङ्क्तिः_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        पटिनिवत्त -1;                                                        \
    } याव (0)

    सचे (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    सचे (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    सचे (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    सचे (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    सचे (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    सचे (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    सचे (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyLexer_scan_fstring_start(संरचना tok_state *tok, संरचना token *token,
                            पूर्णाङ्क c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    पूर्णाङ्क quote = c;
    पूर्णाङ्क quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    पूर्णाङ्क after_quote = tok_nextc(tok);
    सचे (after_quote == quote) {
        पूर्णाङ्क after_after_quote = tok_nextc(tok);
        सचे (after_after_quote == quote) {
            quote_size = 3;
        }
        अञ्ञथा {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    सचे (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_प्रक्षिप_FTString(tok);
    सचे (state == रिक्त) {
        पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_मध्यः;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->व्यञ्जकम्_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    पटिनिवत्त _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

पूर्णाङ्क
_PyLexer_scan_string(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    पूर्णाङ्क quote = c;
    पूर्णाङ्क quote_size = 1;             /* 1 or 3 */
    पूर्णाङ्क end_quote_size = 0;
    पूर्णाङ्क has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    सचे (c == quote) {
        c = tok_nextc(tok);
        सचे (c == quote) {
            quote_size = 3;
        }
        अञ्ञथा {
            end_quote_size = 1;     /* empty string found */
        }
    }
    सचे (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    याव (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        सचे (tok->done == E_ERROR) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
        सचे (tok->done == E_DECODE) {
            भङ्ग;
        }
        सचे (c == EOF || (quote_size == 1 && c == '\n')) {
            सचे (tok_failed(tok)) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
            सचे (c == EOF && tok->allow_incomplete_निवेशः) {
                पटिनिवत्त incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            पूर्णाङ्क end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ध्रुव वर्ण *पङ्क्तिः = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ध्रुव ftstring_state *state = _PyLexer_CurrentFTString(tok);
            सचे (state != रिक्त) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                सचे (state->quote == quote && state->quote_size == quote_size) {
                    पूर्णाङ्क level = state->paren_level + state->replacement_depth - 1;
                    प्रतिज्ञा(level >= 0 && level < tok->level);
                    प्रतिज्ञा(tok->parenstack[level] == '{');
                    पूर्णाङ्क lineno = tok->parenlinenostack[level];
                    सचे (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    अञ्ञथा {
                        _PyTokenizer_syntaxerror_at(
                            tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    पटिनिवत्त string_error_token(tok, token, tok->start, location);
                }
            }

            सचे (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                सचे (c != '\n') {
                    tok->done = E_EOFS;
                }
                पटिनिवत्त string_error_token(tok, token, tok->start, location);
            }
            अञ्ञथा {
                सचे (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } अञ्ञथा {
                    _PyTokenizer_syntaxerror_at(
                        tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                सचे (c != '\n') {
                    tok->done = E_EOLS;
                }
                पटिनिवत्त string_error_token(tok, token, tok->start, location);
            }
        }
        सचे (c == quote) {
            end_quote_size += 1;
        }
        अञ्ञथा {
            end_quote_size = 0;
            सचे (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                सचे (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                सचे (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    पटिनिवत्त MAKE_TOKEN(STRING);
}

पूर्णाङ्क
_PyLexer_get_ftstring(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_व्यञ्जकम्_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    पूर्णाङ्क token_type;
    पूर्णाङ्क end_quote_size = 0;
    पूर्णाङ्क unicode_escape = 0;
    पूर्णाङ्क quote = current->quote;
    पूर्णाङ्क quote_size = current->quote_size;
    पूर्णाङ्क in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    पूर्णाङ्क raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    याव (end_quote_size != quote_size) {
        पूर्णाङ्क c = tok_nextc(tok);
        सचे (tok->done == E_ERROR || tok->done == E_DECODE) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }

        सचे (c == EOF || (quote_size == 1 && c == '\n')) {
            सचे (tok_failed(tok)) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
            सचे (c == EOF && tok->allow_incomplete_निवेशः) {
                पटिनिवत्त incomplete_string_token(tok, token,
                    FTSTRING_मध्यः(current), current->start, current->start_loc);
            }

            सचे (in_format_spec && c == '\n') {
                पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            पूर्णाङ्क end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ध्रुव वर्ण *पङ्क्तिः = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            सचे (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                सचे (c != '\n') {
                    tok->done = E_EOFS;
                }
                पटिनिवत्त string_error_token(tok, token,
                    current->start, location);
            }
            अञ्ञथा {
                _PyTokenizer_syntaxerror_at(
                    tok, पङ्क्तिः, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                पटिनिवत्त string_error_token(tok, token,
                    current->start, location);
            }
        }

        सचे (c == quote) {
            end_quote_size += 1;
            अनुवर्त;
        } अञ्ञथा {
            end_quote_size = 0;
        }

        सचे (c == '{') {
            पूर्णाङ्क peek = tok_nextc(tok);
            सचे (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off व्यञ्जकम्_start = tok->cur;
                tok_backup(tok, c);
                सचे (begin_ftstring_व्यञ्जकम्(tok, current, व्यञ्जकम्_start) < 0) {
                    पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                सचे (p_start == p_end) {
                    पटिनिवत्त _PyLexer_get_normal(tok, current, token);
                }
            } अञ्ञथा {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            गच्छ emit_मध्यः;
        } अञ्ञथा सचे (c == '}') {
            सचे (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                गच्छ emit_मध्यः;
            }
            पूर्णाङ्क peek = tok_nextc(tok);

            पूर्णाङ्क bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            सचे (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            अञ्ञथा {
                tok_backup(tok, peek);
                सचे (!in_format_spec && bracket_depth == 0) {
                    सचे (tok->start == tok->cur - 1) {
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    गच्छ emit_मध्यः;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            गच्छ emit_मध्यः;
        } अञ्ञथा सचे (c == '\\') {
            पूर्णाङ्क peek = tok_nextc(tok);
            सचे (peek == '\r') {
                peek = tok_nextc(tok);
            }
            सचे (peek == '{' || peek == '}') {
                सचे (!raw) {
                    सचे (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                अनुवर्त;
            }

            सचे (!raw) {
                सचे (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    सचे (peek == '{') {
                        unicode_escape = 1;
                    } अञ्ञथा {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    सचे (p_end - quote_size == p_start) {
        पूर्णाङ्क end_token = FTSTRING_END(current);
        Py_Lexer_निष्कासय_FTString(tok);
        पटिनिवत्त MAKE_TOKEN(end_token);
    }
    अत्थाय (पूर्णाङ्क i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_मध्यः:
    token_type = MAKE_TOKEN(FTSTRING_मध्यः(current));
    token->is_raw = raw;
    पटिनिवत्त token_type;
}
