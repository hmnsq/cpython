#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

स्थिर पूर्णांक
string_error_token(संरचना tok_state *tok, संरचना token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    पूर्णांक type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    परत type;
}

स्थिर पूर्णांक
incomplete_string_token(संरचना tok_state *tok, संरचना token *token,
                        पूर्णांक type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    परत type;
}

पूर्णांक
_PyLexer_record_ftstring_comment(संरचना tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    प्रतिज्ञा(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    जर (state->अभिव्यक्ती_span.end >= 0) {
        परत 0;
    }
    प्रतिज्ञा(state->अभिव्यक्ती_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    जर (comments == रिक्त || comments->मोजणी == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)आकार(*comments)) /
            (Py_ssize_t)आकार(*comments->spans);
        जर (comments != रिक्त && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            परत -1;
        }
        Py_ssize_t capacity = comments == रिक्त ? 4 : comments->capacity * 2;
        माप_प्रकार size = आकार(*comments) +
            (माप_प्रकार)capacity * आकार(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        जर (resized == रिक्त) {
            PyErr_NoMemory();
            परत -1;
        }
        comments = resized;
        जर (state->comments == रिक्त) {
            comments->मोजणी = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->मोजणी++] =
        (_PyTok_Span){start, end};
    परत 0;
}

स्थिर पूर्णांक
finish_ftstring_अभिव्यक्ती(संरचना tok_state *tok, ftstring_state *state,
                     संरचना token *token)
{
    प्रतिज्ञा(token != रिक्त && state == _PyLexer_CurrentFTString(tok));
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    जर (state->अभिव्यक्ती_span.end >= 0) {
        परत 0;
    }
    प्रतिज्ञा(state->अभिव्यक्ती_span.start >= 0);
    state->अभिव्यक्ती_span.end = tok->start;
    पूर्णांक tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    जर (!(state->debug_अभिव्यक्ती || tstring_interpolation) || token->metadata) {
        परत 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    जर (comments != रिक्त && comments->मोजणी > 0) {
        Py_ssize_t stripped_size =
            state->अभिव्यक्ती_span.end - state->अभिव्यक्ती_span.start;
        Py_ssize_t comment_मोजणी = 0;
        साठी (Py_ssize_t i = 0; i < comments->मोजणी; i++) {
            _PyTok_Span comment = comments->spans[i];
            प्रतिज्ञा(_PyTok_SpanIsValid(comment));
            प्रतिज्ञा(comment.start >= (i == 0 ? state->अभिव्यक्ती_span.start
                                           : comments->spans[i - 1].end));
            जर (comment.start >= state->अभिव्यक्ती_span.end) {
                थांबा;
            }
            प्रतिज्ञा(comment.end <= state->अभिव्यक्ती_span.end);
            stripped_size -= comment.end - comment.start;
            comment_मोजणी++;
        }
        अक्षर *stripped = PyMem_Malloc((माप_प्रकार)stripped_size);
        जर (stripped == रिक्त) {
            PyErr_NoMemory();
            परत -1;
        }
        _PyTok_Span kept = {state->अभिव्यक्ती_span.start, state->अभिव्यक्ती_span.start};
        Py_ssize_t stripped_len = 0;
        साठी (Py_ssize_t i = 0; i < comment_मोजणी; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            स्थिरांक अक्षर *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            स_नकल(stripped + stripped_len, text, (माप_प्रकार)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->अभिव्यक्ती_span.end;
        Py_ssize_t length;
        स्थिरांक अक्षर *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        स_नकल(stripped + stripped_len, text, (माप_प्रकार)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, रिक्त);
        PyMem_Free(stripped);
    }
    अन्यथा {
        Py_ssize_t अभिव्यक्ती_len;
        स्थिरांक अक्षर *अभिव्यक्ती = _PyTok_SourceSpanView(
            &tok->source, state->अभिव्यक्ती_span, &अभिव्यक्ती_len);
        res = PyUnicode_DecodeUTF8(अभिव्यक्ती, अभिव्यक्ती_len, रिक्त);
    }

    जर (!res) {
        परत -1;
    }
    token->metadata = res;
    परत 0;
}

स्थिर पूर्णांक
begin_ftstring_अभिव्यक्ती(संरचना tok_state *tok, ftstring_state *state,
                    _PyTok_Off अभिव्यक्ती_start)
{
    प्रतिज्ञा(state->mode != FTSTRING_MODE_EXPRESSION);
    state->अभिव्यक्ती_span = (_PyTok_Span){अभिव्यक्ती_start, -1};
    जर (state->comments != रिक्त) {
        state->comments->मोजणी = 0;
    }
    जर (state->replacement_depth >= MAX_अभिव्यक्ती_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        परत -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_अभिव्यक्ती = 0;
    परत 0;
}

पूर्णांक
_PyLexer_ftstring_punctuation(संरचना tok_state *tok, ftstring_state *state,
                              संरचना token *token, पूर्णांक c)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा(c == ':' || c == '}' || c == '!');
    जर (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        परत 0;
    }
    जर (c == '!') {
        पूर्णांक next = tok_nextc(tok);
        tok_backup(tok, next);
        जर (next == '=') {
            परत 0;
        }
    }
    जर (finish_ftstring_अभिव्यक्ती(tok, state, token) < 0) {
        परत -1;
    }
    जर (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        परत COLON;
    }
    परत 0;
}

पूर्णांक
Py_Lexer_close_ftstring_अभिव्यक्ती(संरचना tok_state *tok, ftstring_state *state,
                             पूर्णांक c)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा(c == ')' || c == ']' || c == '}');
    पूर्णांक depth = _PyLexer_FTStringBracketDepth(tok, state);
    जर (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        परत -1;
    }
    जर (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_मधला;
        state->debug_अभिव्यक्ती = 0;
    }
    परत 0;
}

शून्य
_PyLexer_mark_ftstring_debug(संरचना tok_state *tok, ftstring_state *state)
{
    प्रतिज्ञा(state->mode == FTSTRING_MODE_EXPRESSION);
    जर (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_अभिव्यक्ती = 1;
    }
}

पूर्णांक
_PyLexer_check_string_prefixes(संरचना tok_state *tok,
                                             पूर्णांक saw_b, पूर्णांक saw_r, पूर्णांक saw_u,
                                             पूर्णांक saw_f, पूर्णांक saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    करा {                                                                  \
        (शून्य)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (पूर्णांक)(tok->start + 1 - tok->ओळ_start),                 \
            (पूर्णांक)(tok->cur - tok->ओळ_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        परत -1;                                                        \
    } पर्यंत (0)

    जर (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    जर (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    जर (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    जर (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    जर (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    जर (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    जर (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    परत 0;
}

पूर्णांक
_PyLexer_scan_fstring_start(संरचना tok_state *tok, संरचना token *token,
                            पूर्णांक c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    पूर्णांक quote = c;
    पूर्णांक quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    पूर्णांक after_quote = tok_nextc(tok);
    जर (after_quote == quote) {
        पूर्णांक after_after_quote = tok_nextc(tok);
        जर (after_after_quote == quote) {
            quote_size = 3;
        }
        अन्यथा {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    जर (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_ढकला_FTString(tok);
    जर (state == रिक्त) {
        परत MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_मधला;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->अभिव्यक्ती_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    परत _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

पूर्णांक
_PyLexer_scan_string(संरचना tok_state *tok, संरचना token *token, पूर्णांक c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    पूर्णांक quote = c;
    पूर्णांक quote_size = 1;             /* 1 or 3 */
    पूर्णांक end_quote_size = 0;
    पूर्णांक has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    जर (c == quote) {
        c = tok_nextc(tok);
        जर (c == quote) {
            quote_size = 3;
        }
        अन्यथा {
            end_quote_size = 1;     /* empty string found */
        }
    }
    जर (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    पर्यंत (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        जर (tok->done == E_ERROR) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
        जर (tok->done == E_DECODE) {
            थांबा;
        }
        जर (c == खातापूर्ण || (quote_size == 1 && c == '\n')) {
            जर (tok_failed(tok)) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
            जर (c == खातापूर्ण && tok->allow_incomplete_आदान) {
                परत incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            पूर्णांक end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            स्थिरांक अक्षर *ओळ = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            स्थिरांक ftstring_state *state = _PyLexer_CurrentFTString(tok);
            जर (state != रिक्त) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                जर (state->quote == quote && state->quote_size == quote_size) {
                    पूर्णांक level = state->paren_level + state->replacement_depth - 1;
                    प्रतिज्ञा(level >= 0 && level < tok->level);
                    प्रतिज्ञा(tok->parenstack[level] == '{');
                    पूर्णांक lineno = tok->parenlinenostack[level];
                    जर (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, ओळ, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    अन्यथा {
                        _PyTokenizer_syntaxerror_at(
                            tok, ओळ, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    परत string_error_token(tok, token, tok->start, location);
                }
            }

            जर (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ओळ, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                जर (c != '\n') {
                    tok->done = E_EOFS;
                }
                परत string_error_token(tok, token, tok->start, location);
            }
            अन्यथा {
                जर (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, ओळ, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } अन्यथा {
                    _PyTokenizer_syntaxerror_at(
                        tok, ओळ, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                जर (c != '\n') {
                    tok->done = E_EOLS;
                }
                परत string_error_token(tok, token, tok->start, location);
            }
        }
        जर (c == quote) {
            end_quote_size += 1;
        }
        अन्यथा {
            end_quote_size = 0;
            जर (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                जर (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                जर (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    परत MAKE_TOKEN(STRING);
}

पूर्णांक
_PyLexer_get_ftstring(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    प्रतिज्ञा((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_अभिव्यक्ती_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    पूर्णांक token_type;
    पूर्णांक end_quote_size = 0;
    पूर्णांक unicode_escape = 0;
    पूर्णांक quote = current->quote;
    पूर्णांक quote_size = current->quote_size;
    पूर्णांक in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    पूर्णांक raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    पर्यंत (end_quote_size != quote_size) {
        पूर्णांक c = tok_nextc(tok);
        जर (tok->done == E_ERROR || tok->done == E_DECODE) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }

        जर (c == खातापूर्ण || (quote_size == 1 && c == '\n')) {
            जर (tok_failed(tok)) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
            जर (c == खातापूर्ण && tok->allow_incomplete_आदान) {
                परत incomplete_string_token(tok, token,
                    FTSTRING_मधला(current), current->start, current->start_loc);
            }

            जर (in_format_spec && c == '\n') {
                परत MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            पूर्णांक end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            स्थिरांक अक्षर *ओळ = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            जर (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ओळ, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                जर (c != '\n') {
                    tok->done = E_EOFS;
                }
                परत string_error_token(tok, token,
                    current->start, location);
            }
            अन्यथा {
                _PyTokenizer_syntaxerror_at(
                    tok, ओळ, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                परत string_error_token(tok, token,
                    current->start, location);
            }
        }

        जर (c == quote) {
            end_quote_size += 1;
            सुरूठेवा;
        } अन्यथा {
            end_quote_size = 0;
        }

        जर (c == '{') {
            पूर्णांक peek = tok_nextc(tok);
            जर (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off अभिव्यक्ती_start = tok->cur;
                tok_backup(tok, c);
                जर (begin_ftstring_अभिव्यक्ती(tok, current, अभिव्यक्ती_start) < 0) {
                    परत MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                जर (p_start == p_end) {
                    परत _PyLexer_get_normal(tok, current, token);
                }
            } अन्यथा {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            जा emit_मधला;
        } अन्यथा जर (c == '}') {
            जर (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                जा emit_मधला;
            }
            पूर्णांक peek = tok_nextc(tok);

            पूर्णांक bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            जर (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            अन्यथा {
                tok_backup(tok, peek);
                जर (!in_format_spec && bracket_depth == 0) {
                    जर (tok->start == tok->cur - 1) {
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    जा emit_मधला;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            जा emit_मधला;
        } अन्यथा जर (c == '\\') {
            पूर्णांक peek = tok_nextc(tok);
            जर (peek == '\r') {
                peek = tok_nextc(tok);
            }
            जर (peek == '{' || peek == '}') {
                जर (!raw) {
                    जर (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        परत MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                सुरूठेवा;
            }

            जर (!raw) {
                जर (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    जर (peek == '{') {
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
    जर (p_end - quote_size == p_start) {
        पूर्णांक end_token = FTSTRING_END(current);
        Py_Lexer_काढा_FTString(tok);
        परत MAKE_TOKEN(end_token);
    }
    साठी (पूर्णांक i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_मधला:
    token_type = MAKE_TOKEN(FTSTRING_मधला(current));
    token->is_raw = raw;
    परत token_type;
}
