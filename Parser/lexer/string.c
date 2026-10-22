#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_token.h"
#അന്തര്ഭാവ "errcode.h"

#അന്തര്ഭാവ "lexer_internal.h"
#അന്തര്ഭാവ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

സ്ഥിതം പൂർണ്ണസംഖ്യ
string_error_token(ഘടന tok_state *tok, ഘടന token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    പൂർണ്ണസംഖ്യ type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    തിരികെ type;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
incomplete_string_token(ഘടന tok_state *tok, ഘടന token *token,
                        പൂർണ്ണസംഖ്യ type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    തിരികെ type;
}

പൂർണ്ണസംഖ്യ
_PyLexer_record_ftstring_comment(ഘടന tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    പ്രതിജ്ഞാ(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    എങ്കിൽ (state->വ്യഞ്ജകം_span.end >= 0) {
        തിരികെ 0;
    }
    പ്രതിജ്ഞാ(state->വ്യഞ്ജകം_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    എങ്കിൽ (comments == ശൂന്യ || comments->എണ്ണം == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)വലുപ്പം(*comments)) /
            (Py_ssize_t)വലുപ്പം(*comments->spans);
        എങ്കിൽ (comments != ശൂന്യ && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            തിരികെ -1;
        }
        Py_ssize_t capacity = comments == ശൂന്യ ? 4 : comments->capacity * 2;
        size_t size = വലുപ്പം(*comments) +
            (size_t)capacity * വലുപ്പം(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        എങ്കിൽ (resized == ശൂന്യ) {
            PyErr_NoMemory();
            തിരികെ -1;
        }
        comments = resized;
        എങ്കിൽ (state->comments == ശൂന്യ) {
            comments->എണ്ണം = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->എണ്ണം++] =
        (_PyTok_Span){start, end};
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
finish_ftstring_വ്യഞ്ജകം(ഘടന tok_state *tok, ftstring_state *state,
                     ഘടന token *token)
{
    പ്രതിജ്ഞാ(token != ശൂന്യ && state == _PyLexer_CurrentFTString(tok));
    പ്രതിജ്ഞാ(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    എങ്കിൽ (state->വ്യഞ്ജകം_span.end >= 0) {
        തിരികെ 0;
    }
    പ്രതിജ്ഞാ(state->വ്യഞ്ജകം_span.start >= 0);
    state->വ്യഞ്ജകം_span.end = tok->start;
    പൂർണ്ണസംഖ്യ tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    എങ്കിൽ (!(state->debug_വ്യഞ്ജകം || tstring_interpolation) || token->metadata) {
        തിരികെ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    എങ്കിൽ (comments != ശൂന്യ && comments->എണ്ണം > 0) {
        Py_ssize_t stripped_size =
            state->വ്യഞ്ജകം_span.end - state->വ്യഞ്ജകം_span.start;
        Py_ssize_t comment_എണ്ണം = 0;
        വേണ്ടി (Py_ssize_t i = 0; i < comments->എണ്ണം; i++) {
            _PyTok_Span comment = comments->spans[i];
            പ്രതിജ്ഞാ(_PyTok_SpanIsValid(comment));
            പ്രതിജ്ഞാ(comment.start >= (i == 0 ? state->വ്യഞ്ജകം_span.start
                                           : comments->spans[i - 1].end));
            എങ്കിൽ (comment.start >= state->വ്യഞ്ജകം_span.end) {
                നിർത്തുക;
            }
            പ്രതിജ്ഞാ(comment.end <= state->വ്യഞ്ജകം_span.end);
            stripped_size -= comment.end - comment.start;
            comment_എണ്ണം++;
        }
        അക്ഷരം *stripped = PyMem_Malloc((size_t)stripped_size);
        എങ്കിൽ (stripped == ശൂന്യ) {
            PyErr_NoMemory();
            തിരികെ -1;
        }
        _PyTok_Span kept = {state->വ്യഞ്ജകം_span.start, state->വ്യഞ്ജകം_span.start};
        Py_ssize_t stripped_len = 0;
        വേണ്ടി (Py_ssize_t i = 0; i < comment_എണ്ണം; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            സ്ഥിരം അക്ഷരം *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->വ്യഞ്ജകം_span.end;
        Py_ssize_t length;
        സ്ഥിരം അക്ഷരം *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, ശൂന്യ);
        PyMem_Free(stripped);
    }
    അല്ലെങ്കിൽ {
        Py_ssize_t വ്യഞ്ജകം_len;
        സ്ഥിരം അക്ഷരം *വ്യഞ്ജകം = _PyTok_SourceSpanView(
            &tok->source, state->വ്യഞ്ജകം_span, &വ്യഞ്ജകം_len);
        res = PyUnicode_DecodeUTF8(വ്യഞ്ജകം, വ്യഞ്ജകം_len, ശൂന്യ);
    }

    എങ്കിൽ (!res) {
        തിരികെ -1;
    }
    token->metadata = res;
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
begin_ftstring_വ്യഞ്ജകം(ഘടന tok_state *tok, ftstring_state *state,
                    _PyTok_Off വ്യഞ്ജകം_start)
{
    പ്രതിജ്ഞാ(state->mode != FTSTRING_MODE_EXPRESSION);
    state->വ്യഞ്ജകം_span = (_PyTok_Span){വ്യഞ്ജകം_start, -1};
    എങ്കിൽ (state->comments != ശൂന്യ) {
        state->comments->എണ്ണം = 0;
    }
    എങ്കിൽ (state->replacement_depth >= MAX_വ്യഞ്ജകം_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        തിരികെ -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_വ്യഞ്ജകം = 0;
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyLexer_ftstring_punctuation(ഘടന tok_state *tok, ftstring_state *state,
                              ഘടന token *token, പൂർണ്ണസംഖ്യ c)
{
    പ്രതിജ്ഞാ(state->mode == FTSTRING_MODE_EXPRESSION);
    പ്രതിജ്ഞാ(c == ':' || c == '}' || c == '!');
    എങ്കിൽ (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        തിരികെ 0;
    }
    എങ്കിൽ (c == '!') {
        പൂർണ്ണസംഖ്യ next = tok_nextc(tok);
        tok_backup(tok, next);
        എങ്കിൽ (next == '=') {
            തിരികെ 0;
        }
    }
    എങ്കിൽ (finish_ftstring_വ്യഞ്ജകം(tok, state, token) < 0) {
        തിരികെ -1;
    }
    എങ്കിൽ (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        തിരികെ COLON;
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
Py_Lexer_close_ftstring_വ്യഞ്ജകം(ഘടന tok_state *tok, ftstring_state *state,
                             പൂർണ്ണസംഖ്യ c)
{
    പ്രതിജ്ഞാ(state->mode == FTSTRING_MODE_EXPRESSION);
    പ്രതിജ്ഞാ(c == ')' || c == ']' || c == '}');
    പൂർണ്ണസംഖ്യ depth = _PyLexer_FTStringBracketDepth(tok, state);
    എങ്കിൽ (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        തിരികെ -1;
    }
    എങ്കിൽ (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_നടുവ്;
        state->debug_വ്യഞ്ജകം = 0;
    }
    തിരികെ 0;
}

ശൂന്യം
_PyLexer_mark_ftstring_debug(ഘടന tok_state *tok, ftstring_state *state)
{
    പ്രതിജ്ഞാ(state->mode == FTSTRING_MODE_EXPRESSION);
    എങ്കിൽ (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_വ്യഞ്ജകം = 1;
    }
}

പൂർണ്ണസംഖ്യ
_PyLexer_check_string_prefixes(ഘടന tok_state *tok,
                                             പൂർണ്ണസംഖ്യ saw_b, പൂർണ്ണസംഖ്യ saw_r, പൂർണ്ണസംഖ്യ saw_u,
                                             പൂർണ്ണസംഖ്യ saw_f, പൂർണ്ണസംഖ്യ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    ചെയ്യുക {                                                                  \
        (ശൂന്യം)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (പൂർണ്ണസംഖ്യ)(tok->start + 1 - tok->വരി_start),                 \
            (പൂർണ്ണസംഖ്യ)(tok->cur - tok->വരി_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        തിരികെ -1;                                                        \
    } ആകുമ്പോൾ (0)

    എങ്കിൽ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    എങ്കിൽ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    എങ്കിൽ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    എങ്കിൽ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    എങ്കിൽ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    എങ്കിൽ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    എങ്കിൽ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyLexer_scan_fstring_start(ഘടന tok_state *tok, ഘടന token *token,
                            പൂർണ്ണസംഖ്യ c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    പൂർണ്ണസംഖ്യ quote = c;
    പൂർണ്ണസംഖ്യ quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    പൂർണ്ണസംഖ്യ after_quote = tok_nextc(tok);
    എങ്കിൽ (after_quote == quote) {
        പൂർണ്ണസംഖ്യ after_after_quote = tok_nextc(tok);
        എങ്കിൽ (after_after_quote == quote) {
            quote_size = 3;
        }
        അല്ലെങ്കിൽ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    എങ്കിൽ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_തള്ളുക_FTString(tok);
    എങ്കിൽ (state == ശൂന്യ) {
        തിരികെ MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_നടുവ്;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->വ്യഞ്ജകം_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    തിരികെ _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

പൂർണ്ണസംഖ്യ
_PyLexer_scan_string(ഘടന tok_state *tok, ഘടന token *token, പൂർണ്ണസംഖ്യ c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    പൂർണ്ണസംഖ്യ quote = c;
    പൂർണ്ണസംഖ്യ quote_size = 1;             /* 1 or 3 */
    പൂർണ്ണസംഖ്യ end_quote_size = 0;
    പൂർണ്ണസംഖ്യ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    എങ്കിൽ (c == quote) {
        c = tok_nextc(tok);
        എങ്കിൽ (c == quote) {
            quote_size = 3;
        }
        അല്ലെങ്കിൽ {
            end_quote_size = 1;     /* empty string found */
        }
    }
    എങ്കിൽ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ആകുമ്പോൾ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        എങ്കിൽ (tok->done == E_ERROR) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
        എങ്കിൽ (tok->done == E_DECODE) {
            നിർത്തുക;
        }
        എങ്കിൽ (c == EOF || (quote_size == 1 && c == '\n')) {
            എങ്കിൽ (tok_failed(tok)) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
            എങ്കിൽ (c == EOF && tok->allow_incomplete_നിവേശം) {
                തിരികെ incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            പൂർണ്ണസംഖ്യ end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            സ്ഥിരം അക്ഷരം *വരി = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            സ്ഥിരം ftstring_state *state = _PyLexer_CurrentFTString(tok);
            എങ്കിൽ (state != ശൂന്യ) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                എങ്കിൽ (state->quote == quote && state->quote_size == quote_size) {
                    പൂർണ്ണസംഖ്യ level = state->paren_level + state->replacement_depth - 1;
                    പ്രതിജ്ഞാ(level >= 0 && level < tok->level);
                    പ്രതിജ്ഞാ(tok->parenstack[level] == '{');
                    പൂർണ്ണസംഖ്യ lineno = tok->parenlinenostack[level];
                    എങ്കിൽ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, വരി, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    അല്ലെങ്കിൽ {
                        _PyTokenizer_syntaxerror_at(
                            tok, വരി, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    തിരികെ string_error_token(tok, token, tok->start, location);
                }
            }

            എങ്കിൽ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, വരി, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                എങ്കിൽ (c != '\n') {
                    tok->done = E_EOFS;
                }
                തിരികെ string_error_token(tok, token, tok->start, location);
            }
            അല്ലെങ്കിൽ {
                എങ്കിൽ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, വരി, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } അല്ലെങ്കിൽ {
                    _PyTokenizer_syntaxerror_at(
                        tok, വരി, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                എങ്കിൽ (c != '\n') {
                    tok->done = E_EOLS;
                }
                തിരികെ string_error_token(tok, token, tok->start, location);
            }
        }
        എങ്കിൽ (c == quote) {
            end_quote_size += 1;
        }
        അല്ലെങ്കിൽ {
            end_quote_size = 0;
            എങ്കിൽ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                എങ്കിൽ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                എങ്കിൽ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    തിരികെ MAKE_TOKEN(STRING);
}

പൂർണ്ണസംഖ്യ
_PyLexer_get_ftstring(ഘടന tok_state *tok, ftstring_state *current, ഘടന token *token)
{
    പ്രതിജ്ഞാ(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    പ്രതിജ്ഞാ((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_വ്യഞ്ജകം_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    പൂർണ്ണസംഖ്യ token_type;
    പൂർണ്ണസംഖ്യ end_quote_size = 0;
    പൂർണ്ണസംഖ്യ unicode_escape = 0;
    പൂർണ്ണസംഖ്യ quote = current->quote;
    പൂർണ്ണസംഖ്യ quote_size = current->quote_size;
    പൂർണ്ണസംഖ്യ in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    പൂർണ്ണസംഖ്യ raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    ആകുമ്പോൾ (end_quote_size != quote_size) {
        പൂർണ്ണസംഖ്യ c = tok_nextc(tok);
        എങ്കിൽ (tok->done == E_ERROR || tok->done == E_DECODE) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }

        എങ്കിൽ (c == EOF || (quote_size == 1 && c == '\n')) {
            എങ്കിൽ (tok_failed(tok)) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
            എങ്കിൽ (c == EOF && tok->allow_incomplete_നിവേശം) {
                തിരികെ incomplete_string_token(tok, token,
                    FTSTRING_നടുവ്(current), current->start, current->start_loc);
            }

            എങ്കിൽ (in_format_spec && c == '\n') {
                തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            പൂർണ്ണസംഖ്യ end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            സ്ഥിരം അക്ഷരം *വരി = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            എങ്കിൽ (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, വരി, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                എങ്കിൽ (c != '\n') {
                    tok->done = E_EOFS;
                }
                തിരികെ string_error_token(tok, token,
                    current->start, location);
            }
            അല്ലെങ്കിൽ {
                _PyTokenizer_syntaxerror_at(
                    tok, വരി, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                തിരികെ string_error_token(tok, token,
                    current->start, location);
            }
        }

        എങ്കിൽ (c == quote) {
            end_quote_size += 1;
            തുടരുക;
        } അല്ലെങ്കിൽ {
            end_quote_size = 0;
        }

        എങ്കിൽ (c == '{') {
            പൂർണ്ണസംഖ്യ peek = tok_nextc(tok);
            എങ്കിൽ (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off വ്യഞ്ജകം_start = tok->cur;
                tok_backup(tok, c);
                എങ്കിൽ (begin_ftstring_വ്യഞ്ജകം(tok, current, വ്യഞ്ജകം_start) < 0) {
                    തിരികെ MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                എങ്കിൽ (p_start == p_end) {
                    തിരികെ _PyLexer_get_normal(tok, current, token);
                }
            } അല്ലെങ്കിൽ {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            പോകുക emit_നടുവ്;
        } അല്ലെങ്കിൽ എങ്കിൽ (c == '}') {
            എങ്കിൽ (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                പോകുക emit_നടുവ്;
            }
            പൂർണ്ണസംഖ്യ peek = tok_nextc(tok);

            പൂർണ്ണസംഖ്യ bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            എങ്കിൽ (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            അല്ലെങ്കിൽ {
                tok_backup(tok, peek);
                എങ്കിൽ (!in_format_spec && bracket_depth == 0) {
                    എങ്കിൽ (tok->start == tok->cur - 1) {
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    പോകുക emit_നടുവ്;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            പോകുക emit_നടുവ്;
        } അല്ലെങ്കിൽ എങ്കിൽ (c == '\\') {
            പൂർണ്ണസംഖ്യ peek = tok_nextc(tok);
            എങ്കിൽ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            എങ്കിൽ (peek == '{' || peek == '}') {
                എങ്കിൽ (!raw) {
                    എങ്കിൽ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        തിരികെ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                തുടരുക;
            }

            എങ്കിൽ (!raw) {
                എങ്കിൽ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    എങ്കിൽ (peek == '{') {
                        unicode_escape = 1;
                    } അല്ലെങ്കിൽ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    എങ്കിൽ (p_end - quote_size == p_start) {
        പൂർണ്ണസംഖ്യ end_token = FTSTRING_END(current);
        Py_Lexer_എടുക്കുക_FTString(tok);
        തിരികെ MAKE_TOKEN(end_token);
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_നടുവ്:
    token_type = MAKE_TOKEN(FTSTRING_നടുവ്(current));
    token->is_raw = raw;
    തിരികെ token_type;
}
