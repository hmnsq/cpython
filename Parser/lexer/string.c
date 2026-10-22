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
    प्रत्यागम type;
}

स्थिर पूर्णाङ्क
incomplete_string_token(संरचना tok_state *tok, संरचना token *token,
                        पूर्णाङ्क type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    प्रत्यागम type;
}

पूर्णाङ्क
_PyLexer_record_ftstring_comment(संरचना tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    प्रतिज्ञा(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    यदि (state->व्यंजक_span.end >= 0) {
        प्रत्यागम 0;
    }
    प्रतिज्ञा(state->व्यंजक_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    यदि (comments == रिक्त || comments->गिनती == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)परिमाण(*comments)) /
            (Py_ssize_t)परिमाण(*comments->spans);
        यदि (comments != रिक्त && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            प्रत्यागम -1;
        }
        Py_ssize_t capacity = comments == रिक्त ? 4 : comments->capacity * 2;
        size_t size = परिमाण(*comments) +
            (size_t)capacity * परिमाण(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        यदि (resized == रिक्त) {
            PyErr_NoMemory();
            प्रत्यागम -1;
        }
        comments = resized;
        यदि (state->comments == रिक्त) {
            comments->गिनती = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->गिनती++] =
        (_PyTok_Span){start, end};
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
finish_ftstring_व्यंजक(संरचना tok_state *tok, ftstring_state *state,
                     संरचना token *token)
{
    प्रतिज्ञा(token != रिक्त && state == _PyLexer_CurrentFTString(tok));
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    यदि (state->व्यंजक_span.end >= 0) {
        प्रत्यागम 0;
    }
    प्रतिज्ञा(state->व्यंजक_span.start >= 0);
    state->व्यंजक_span.end = tok->start;
    पूर्णाङ्क tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    यदि (!(state->debug_व्यंजक || tstring_interpolation) || token->metadata) {
        प्रत्यागम 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    यदि (comments != रिक्त && comments->गिनती > 0) {
        Py_ssize_t stripped_size =
            state->व्यंजक_span.end - state->व्यंजक_span.start;
        Py_ssize_t comment_गिनती = 0;
        प्रति (Py_ssize_t i = 0; i < comments->गिनती; i++) {
            _PyTok_Span comment = comments->spans[i];
            प्रतिज्ञा(_PyTok_SpanIsValid(comment));
            प्रतिज्ञा(comment.start >= (i == 0 ? state->व्यंजक_span.start
                                           : comments->spans[i - 1].end));
            यदि (comment.start >= state->व्यंजक_span.end) {
                भङ्ग;
            }
            प्रतिज्ञा(comment.end <= state->व्यंजक_span.end);
            stripped_size -= comment.end - comment.start;
            comment_गिनती++;
        }
        वर्ण *stripped = PyMem_Malloc((size_t)stripped_size);
        यदि (stripped == रिक्त) {
            PyErr_NoMemory();
            प्रत्यागम -1;
        }
        _PyTok_Span kept = {state->व्यंजक_span.start, state->व्यंजक_span.start};
        Py_ssize_t stripped_len = 0;
        प्रति (Py_ssize_t i = 0; i < comment_गिनती; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ध्रुव वर्ण *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->व्यंजक_span.end;
        Py_ssize_t length;
        ध्रुव वर्ण *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, रिक्त);
        PyMem_Free(stripped);
    }
    अन्यथा {
        Py_ssize_t व्यंजक_len;
        ध्रुव वर्ण *व्यंजक = _PyTok_SourceSpanView(
            &tok->source, state->व्यंजक_span, &व्यंजक_len);
        res = PyUnicode_DecodeUTF8(व्यंजक, व्यंजक_len, रिक्त);
    }

    यदि (!res) {
        प्रत्यागम -1;
    }
    token->metadata = res;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
begin_ftstring_व्यंजक(संरचना tok_state *tok, ftstring_state *state,
                    _PyTok_Off व्यंजक_start)
{
    प्रतिज्ञा(state->mode != FTSTRING_MODE_EXPRESSION);
    state->व्यंजक_span = (_PyTok_Span){व्यंजक_start, -1};
    यदि (state->comments != रिक्त) {
        state->comments->गिनती = 0;
    }
    यदि (state->replacement_depth >= MAX_व्यंजक_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        प्रत्यागम -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_व्यंजक = 0;
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyLexer_ftstring_punctuation(संरचना tok_state *tok, ftstring_state *state,
                              संरचना token *token, पूर्णाङ्क c)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा(c == ':' || c == '}' || c == '!');
    यदि (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        प्रत्यागम 0;
    }
    यदि (c == '!') {
        पूर्णाङ्क next = tok_nextc(tok);
        tok_backup(tok, next);
        यदि (next == '=') {
            प्रत्यागम 0;
        }
    }
    यदि (finish_ftstring_व्यंजक(tok, state, token) < 0) {
        प्रत्यागम -1;
    }
    यदि (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        प्रत्यागम COLON;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
Py_Lexer_close_ftstring_व्यंजक(संरचना tok_state *tok, ftstring_state *state,
                             पूर्णाङ्क c)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा(c == ')' || c == ']' || c == '}');
    पूर्णाङ्क depth = _PyLexer_FTStringBracketDepth(tok, state);
    यदि (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        प्रत्यागम -1;
    }
    यदि (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_मध्य;
        state->debug_व्यंजक = 0;
    }
    प्रत्यागम 0;
}

शून्य
_PyLexer_mark_ftstring_debug(संरचना tok_state *tok, ftstring_state *state)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    यदि (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_व्यंजक = 1;
    }
}

पूर्णाङ्क
_PyLexer_check_string_prefixes(संरचना tok_state *tok,
                                             पूर्णाङ्क saw_b, पूर्णाङ्क saw_r, पूर्णाङ्क saw_u,
                                             पूर्णाङ्क saw_f, पूर्णाङ्क saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    कुरु {                                                                  \
        (शून्य)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (पूर्णाङ्क)(tok->start + 1 - tok->पंक्ति_start),                 \
            (पूर्णाङ्क)(tok->cur - tok->पंक्ति_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        प्रत्यागम -1;                                                        \
    } यावत् (0)

    यदि (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    यदि (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    यदि (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    यदि (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    यदि (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    यदि (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    यदि (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    प्रत्यागम 0;
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
    यदि (after_quote == quote) {
        पूर्णाङ्क after_after_quote = tok_nextc(tok);
        यदि (after_after_quote == quote) {
            quote_size = 3;
        }
        अन्यथा {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    यदि (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_धकेलो_FTString(tok);
    यदि (state == रिक्त) {
        प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_मध्य;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->व्यंजक_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    प्रत्यागम _PyLexer_IsTString(kind)
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
    यदि (c == quote) {
        c = tok_nextc(tok);
        यदि (c == quote) {
            quote_size = 3;
        }
        अन्यथा {
            end_quote_size = 1;     /* empty string found */
        }
    }
    यदि (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    यावत् (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        यदि (tok->done == E_ERROR) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        यदि (tok->done == E_DECODE) {
            भङ्ग;
        }
        यदि (c == EOF || (quote_size == 1 && c == '\n')) {
            यदि (tok_failed(tok)) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
            यदि (c == EOF && tok->allow_incomplete_निवेश) {
                प्रत्यागम incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            पूर्णाङ्क end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ध्रुव वर्ण *पंक्ति = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ध्रुव ftstring_state *state = _PyLexer_CurrentFTString(tok);
            यदि (state != रिक्त) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                यदि (state->quote == quote && state->quote_size == quote_size) {
                    पूर्णाङ्क level = state->paren_level + state->replacement_depth - 1;
                    प्रतिज्ञा(level >= 0 && level < tok->level);
                    प्रतिज्ञा(tok->parenstack[level] == '{');
                    पूर्णाङ्क lineno = tok->parenlinenostack[level];
                    यदि (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    अन्यथा {
                        _PyTokenizer_syntaxerror_at(
                            tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    प्रत्यागम string_error_token(tok, token, tok->start, location);
                }
            }

            यदि (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                यदि (c != '\n') {
                    tok->done = E_EOFS;
                }
                प्रत्यागम string_error_token(tok, token, tok->start, location);
            }
            अन्यथा {
                यदि (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } अन्यथा {
                    _PyTokenizer_syntaxerror_at(
                        tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                यदि (c != '\n') {
                    tok->done = E_EOLS;
                }
                प्रत्यागम string_error_token(tok, token, tok->start, location);
            }
        }
        यदि (c == quote) {
            end_quote_size += 1;
        }
        अन्यथा {
            end_quote_size = 0;
            यदि (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                यदि (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                यदि (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    प्रत्यागम MAKE_TOKEN(STRING);
}

पूर्णाङ्क
_PyLexer_get_ftstring(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_व्यंजक_NESTING);
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

    यावत् (end_quote_size != quote_size) {
        पूर्णाङ्क c = tok_nextc(tok);
        यदि (tok->done == E_ERROR || tok->done == E_DECODE) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }

        यदि (c == EOF || (quote_size == 1 && c == '\n')) {
            यदि (tok_failed(tok)) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
            यदि (c == EOF && tok->allow_incomplete_निवेश) {
                प्रत्यागम incomplete_string_token(tok, token,
                    FTSTRING_मध्य(current), current->start, current->start_loc);
            }

            यदि (in_format_spec && c == '\n') {
                प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            पूर्णाङ्क end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ध्रुव वर्ण *पंक्ति = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            यदि (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                यदि (c != '\n') {
                    tok->done = E_EOFS;
                }
                प्रत्यागम string_error_token(tok, token,
                    current->start, location);
            }
            अन्यथा {
                _PyTokenizer_syntaxerror_at(
                    tok, पंक्ति, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                प्रत्यागम string_error_token(tok, token,
                    current->start, location);
            }
        }

        यदि (c == quote) {
            end_quote_size += 1;
            अनुवर्त;
        } अन्यथा {
            end_quote_size = 0;
        }

        यदि (c == '{') {
            पूर्णाङ्क peek = tok_nextc(tok);
            यदि (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off व्यंजक_start = tok->cur;
                tok_backup(tok, c);
                यदि (begin_ftstring_व्यंजक(tok, current, व्यंजक_start) < 0) {
                    प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                यदि (p_start == p_end) {
                    प्रत्यागम _PyLexer_get_normal(tok, current, token);
                }
            } अन्यथा {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            गच्छ emit_मध्य;
        } अन्यथा यदि (c == '}') {
            यदि (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                गच्छ emit_मध्य;
            }
            पूर्णाङ्क peek = tok_nextc(tok);

            पूर्णाङ्क bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            यदि (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            अन्यथा {
                tok_backup(tok, peek);
                यदि (!in_format_spec && bracket_depth == 0) {
                    यदि (tok->start == tok->cur - 1) {
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    गच्छ emit_मध्य;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            गच्छ emit_मध्य;
        } अन्यथा यदि (c == '\\') {
            पूर्णाङ्क peek = tok_nextc(tok);
            यदि (peek == '\r') {
                peek = tok_nextc(tok);
            }
            यदि (peek == '{' || peek == '}') {
                यदि (!raw) {
                    यदि (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                अनुवर्त;
            }

            यदि (!raw) {
                यदि (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    यदि (peek == '{') {
                        unicode_escape = 1;
                    } अन्यथा {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    यदि (p_end - quote_size == p_start) {
        पूर्णाङ्क end_token = FTSTRING_END(current);
        Py_Lexer_निकालो_FTString(tok);
        प्रत्यागम MAKE_TOKEN(end_token);
    }
    प्रति (पूर्णाङ्क i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_मध्य:
    token_type = MAKE_TOKEN(FTSTRING_मध्य(current));
    token->is_raw = raw;
    प्रत्यागम token_type;
}
