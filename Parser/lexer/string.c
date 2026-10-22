#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "errcode.h"

#অন্তর্ভাব "lexer_internal.h"
#অন্তর্ভাব "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

স্থির পূর্ণাঙ্ক
string_error_token(সংরচনা tok_state *tok, সংরচনা token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    পূর্ণাঙ্ক type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    প্রত্যাগম type;
}

স্থির পূর্ণাঙ্ক
incomplete_string_token(সংরচনা tok_state *tok, সংরচনা token *token,
                        পূর্ণাঙ্ক type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    প্রত্যাগম type;
}

পূর্ণাঙ্ক
_PyLexer_record_ftstring_comment(সংরচনা tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    প্রতিজ্ঞা(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    যদি (state->রাশিমালা_span.end >= 0) {
        প্রত্যাগম 0;
    }
    প্রতিজ্ঞা(state->রাশিমালা_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    যদি (comments == রিক্ত || comments->গুনতি == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)পরিমাণ(*comments)) /
            (Py_ssize_t)পরিমাণ(*comments->spans);
        যদি (comments != রিক্ত && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            প্রত্যাগম -1;
        }
        Py_ssize_t capacity = comments == রিক্ত ? 4 : comments->capacity * 2;
        size_t size = পরিমাণ(*comments) +
            (size_t)capacity * পরিমাণ(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, size);
        যদি (resized == রিক্ত) {
            PyErr_NoMemory();
            প্রত্যাগম -1;
        }
        comments = resized;
        যদি (state->comments == রিক্ত) {
            comments->গুনতি = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->গুনতি++] =
        (_PyTok_Span){start, end};
    প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক
finish_ftstring_রাশিমালা(সংরচনা tok_state *tok, ftstring_state *state,
                     সংরচনা token *token)
{
    প্রতিজ্ঞা(token != রিক্ত && state == _PyLexer_CurrentFTString(tok));
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    যদি (state->রাশিমালা_span.end >= 0) {
        প্রত্যাগম 0;
    }
    প্রতিজ্ঞা(state->রাশিমালা_span.start >= 0);
    state->রাশিমালা_span.end = tok->start;
    পূর্ণাঙ্ক tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    যদি (!(state->debug_রাশিমালা || tstring_interpolation) || token->metadata) {
        প্রত্যাগম 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    যদি (comments != রিক্ত && comments->গুনতি > 0) {
        Py_ssize_t stripped_size =
            state->রাশিমালা_span.end - state->রাশিমালা_span.start;
        Py_ssize_t comment_গুনতি = 0;
        প্রতি (Py_ssize_t i = 0; i < comments->গুনতি; i++) {
            _PyTok_Span comment = comments->spans[i];
            প্রতিজ্ঞা(_PyTok_SpanIsValid(comment));
            প্রতিজ্ঞা(comment.start >= (i == 0 ? state->রাশিমালা_span.start
                                           : comments->spans[i - 1].end));
            যদি (comment.start >= state->রাশিমালা_span.end) {
                ভঙ্গ;
            }
            প্রতিজ্ঞা(comment.end <= state->রাশিমালা_span.end);
            stripped_size -= comment.end - comment.start;
            comment_গুনতি++;
        }
        বর্ণ *stripped = PyMem_Malloc((size_t)stripped_size);
        যদি (stripped == রিক্ত) {
            PyErr_NoMemory();
            প্রত্যাগম -1;
        }
        _PyTok_Span kept = {state->রাশিমালা_span.start, state->রাশিমালা_span.start};
        Py_ssize_t stripped_len = 0;
        প্রতি (Py_ssize_t i = 0; i < comment_গুনতি; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t length;
            ধ্রুব বর্ণ *text = _PyTok_SourceSpanView(
                &tok->source, kept, &length);
            memcpy(stripped + stripped_len, text, (size_t)length);
            stripped_len += length;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->রাশিমালা_span.end;
        Py_ssize_t length;
        ধ্রুব বর্ণ *text = _PyTok_SourceSpanView(&tok->source, kept, &length);
        memcpy(stripped + stripped_len, text, (size_t)length);
        res = PyUnicode_DecodeUTF8(stripped, stripped_size, রিক্ত);
        PyMem_Free(stripped);
    }
    অন্যথা {
        Py_ssize_t রাশিমালা_len;
        ধ্রুব বর্ণ *রাশিমালা = _PyTok_SourceSpanView(
            &tok->source, state->রাশিমালা_span, &রাশিমালা_len);
        res = PyUnicode_DecodeUTF8(রাশিমালা, রাশিমালা_len, রিক্ত);
    }

    যদি (!res) {
        প্রত্যাগম -1;
    }
    token->metadata = res;
    প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক
begin_ftstring_রাশিমালা(সংরচনা tok_state *tok, ftstring_state *state,
                    _PyTok_Off রাশিমালা_start)
{
    প্রতিজ্ঞা(state->mode != FTSTRING_MODE_EXPRESSION);
    state->রাশিমালা_span = (_PyTok_Span){রাশিমালা_start, -1};
    যদি (state->comments != রিক্ত) {
        state->comments->গুনতি = 0;
    }
    যদি (state->replacement_depth >= MAX_রাশিমালা_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        প্রত্যাগম -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_রাশিমালা = 0;
    প্রত্যাগম 0;
}

পূর্ণাঙ্ক
_PyLexer_ftstring_punctuation(সংরচনা tok_state *tok, ftstring_state *state,
                              সংরচনা token *token, পূর্ণাঙ্ক c)
{
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION);
    প্রতিজ্ঞা(c == ':' || c == '}' || c == '!');
    যদি (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        প্রত্যাগম 0;
    }
    যদি (c == '!') {
        পূর্ণাঙ্ক next = tok_nextc(tok);
        tok_backup(tok, next);
        যদি (next == '=') {
            প্রত্যাগম 0;
        }
    }
    যদি (finish_ftstring_রাশিমালা(tok, state, token) < 0) {
        প্রত্যাগম -1;
    }
    যদি (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        প্রত্যাগম COLON;
    }
    প্রত্যাগম 0;
}

পূর্ণাঙ্ক
Py_Lexer_close_ftstring_রাশিমালা(সংরচনা tok_state *tok, ftstring_state *state,
                             পূর্ণাঙ্ক c)
{
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION);
    প্রতিজ্ঞা(c == ')' || c == ']' || c == '}');
    পূর্ণাঙ্ক depth = _PyLexer_FTStringBracketDepth(tok, state);
    যদি (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        প্রত্যাগম -1;
    }
    যদি (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_মধ্য;
        state->debug_রাশিমালা = 0;
    }
    প্রত্যাগম 0;
}

শূন্য
_PyLexer_mark_ftstring_debug(সংরচনা tok_state *tok, ftstring_state *state)
{
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION);
    যদি (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_রাশিমালা = 1;
    }
}

পূর্ণাঙ্ক
_PyLexer_check_string_prefixes(সংরচনা tok_state *tok,
                                             পূর্ণাঙ্ক saw_b, পূর্ণাঙ্ক saw_r, পূর্ণাঙ্ক saw_u,
                                             পূর্ণাঙ্ক saw_f, পূর্ণাঙ্ক saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    কুরু {                                                                  \
        (শূন্য)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (পূর্ণাঙ্ক)(tok->start + 1 - tok->সারি_start),                 \
            (পূর্ণাঙ্ক)(tok->cur - tok->সারি_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        প্রত্যাগম -1;                                                        \
    } যাবত্ (0)

    যদি (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    যদি (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    যদি (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    যদি (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    যদি (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    যদি (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    যদি (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#undef RETURN_SYNTAX_ERROR

    প্রত্যাগম 0;
}

পূর্ণাঙ্ক
_PyLexer_scan_fstring_start(সংরচনা tok_state *tok, সংরচনা token *token,
                            পূর্ণাঙ্ক c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    পূর্ণাঙ্ক quote = c;
    পূর্ণাঙ্ক quote_size = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    পূর্ণাঙ্ক after_quote = tok_nextc(tok);
    যদি (after_quote == quote) {
        পূর্ণাঙ্ক after_after_quote = tok_nextc(tok);
        যদি (after_after_quote == quote) {
            quote_size = 3;
        }
        অন্যথা {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    যদি (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_end = tok->cur;
    ftstring_state *state = Py_Lexer_ঠেলো_FTString(tok);
    যদি (state == রিক্ত) {
        প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_মধ্য;
    state->quote = quote;
    state->quote_size = quote_size;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->রাশিমালা_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    প্রত্যাগম _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

পূর্ণাঙ্ক
_PyLexer_scan_string(সংরচনা tok_state *tok, সংরচনা token *token, পূর্ণাঙ্ক c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    পূর্ণাঙ্ক quote = c;
    পূর্ণাঙ্ক quote_size = 1;             /* 1 or 3 */
    পূর্ণাঙ্ক end_quote_size = 0;
    পূর্ণাঙ্ক has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    যদি (c == quote) {
        c = tok_nextc(tok);
        যদি (c == quote) {
            quote_size = 3;
        }
        অন্যথা {
            end_quote_size = 1;     /* empty string found */
        }
    }
    যদি (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    যাবত্ (end_quote_size != quote_size) {
        c = tok_nextc(tok);
        যদি (tok->done == E_ERROR) {
            প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
        }
        যদি (tok->done == E_DECODE) {
            ভঙ্গ;
        }
        যদি (c == EOF || (quote_size == 1 && c == '\n')) {
            যদি (tok_failed(tok)) {
                প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
            }
            যদি (c == EOF && tok->allow_incomplete_প্রবেশ) {
                প্রত্যাগম incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            পূর্ণাঙ্ক end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ধ্রুব বর্ণ *সারি = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ধ্রুব ftstring_state *state = _PyLexer_CurrentFTString(tok);
            যদি (state != রিক্ত) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                যদি (state->quote == quote && state->quote_size == quote_size) {
                    পূর্ণাঙ্ক level = state->paren_level + state->replacement_depth - 1;
                    প্রতিজ্ঞা(level >= 0 && level < tok->level);
                    প্রতিজ্ঞা(tok->parenstack[level] == '{');
                    পূর্ণাঙ্ক lineno = tok->parenlinenostack[level];
                    যদি (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, সারি, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            _PyLexer_StringPrefix(state->kind), lineno);
                    }
                    অন্যথা {
                        _PyTokenizer_syntaxerror_at(
                            tok, সারি, cursor_offset, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            _PyLexer_StringPrefix(state->kind));
                    }
                    প্রত্যাগম string_error_token(tok, token, tok->start, location);
                }
            }

            যদি (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, সারি, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                যদি (c != '\n') {
                    tok->done = E_EOFS;
                }
                প্রত্যাগম string_error_token(tok, token, tok->start, location);
            }
            অন্যথা {
                যদি (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, সারি, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        end_lineno
                    );
                } অন্যথা {
                    _PyTokenizer_syntaxerror_at(
                        tok, সারি, cursor_offset, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", end_lineno
                    );
                }
                যদি (c != '\n') {
                    tok->done = E_EOLS;
                }
                প্রত্যাগম string_error_token(tok, token, tok->start, location);
            }
        }
        যদি (c == quote) {
            end_quote_size += 1;
        }
        অন্যথা {
            end_quote_size = 0;
            যদি (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                যদি (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                যদি (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    প্রত্যাগম MAKE_TOKEN(STRING);
}

পূর্ণাঙ্ক
_PyLexer_get_ftstring(সংরচনা tok_state *tok, ftstring_state *current, সংরচনা token *token)
{
    প্রতিজ্ঞা(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    প্রতিজ্ঞা((current->quote_size == 1 || current->quote_size == 3) &&
           current->replacement_depth <= MAX_রাশিমালা_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    পূর্ণাঙ্ক token_type;
    পূর্ণাঙ্ক end_quote_size = 0;
    পূর্ণাঙ্ক unicode_escape = 0;
    পূর্ণাঙ্ক quote = current->quote;
    পূর্ণাঙ্ক quote_size = current->quote_size;
    পূর্ণাঙ্ক in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    পূর্ণাঙ্ক raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    যাবত্ (end_quote_size != quote_size) {
        পূর্ণাঙ্ক c = tok_nextc(tok);
        যদি (tok->done == E_ERROR || tok->done == E_DECODE) {
            প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
        }

        যদি (c == EOF || (quote_size == 1 && c == '\n')) {
            যদি (tok_failed(tok)) {
                প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
            }
            যদি (c == EOF && tok->allow_incomplete_প্রবেশ) {
                প্রত্যাগম incomplete_string_token(tok, token,
                    FTSTRING_মধ্য(current), current->start, current->start_loc);
            }

            যদি (in_format_spec && c == '\n') {
                প্রত্যাগম MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            পূর্ণাঙ্ক end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ধ্রুব বর্ণ *সারি = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            যদি (quote_size == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, সারি, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                যদি (c != '\n') {
                    tok->done = E_EOFS;
                }
                প্রত্যাগম string_error_token(tok, token,
                    current->start, location);
            }
            অন্যথা {
                _PyTokenizer_syntaxerror_at(
                    tok, সারি, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                প্রত্যাগম string_error_token(tok, token,
                    current->start, location);
            }
        }

        যদি (c == quote) {
            end_quote_size += 1;
            অনুবর্ত;
        } অন্যথা {
            end_quote_size = 0;
        }

        যদি (c == '{') {
            পূর্ণাঙ্ক peek = tok_nextc(tok);
            যদি (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off রাশিমালা_start = tok->cur;
                tok_backup(tok, c);
                যদি (begin_ftstring_রাশিমালা(tok, current, রাশিমালা_start) < 0) {
                    প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                যদি (p_start == p_end) {
                    প্রত্যাগম _PyLexer_get_normal(tok, current, token);
                }
            } অন্যথা {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            গচ্ছ emit_মধ্য;
        } অন্যথা যদি (c == '}') {
            যদি (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                গচ্ছ emit_মধ্য;
            }
            পূর্ণাঙ্ক peek = tok_nextc(tok);

            পূর্ণাঙ্ক bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            যদি (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            অন্যথা {
                tok_backup(tok, peek);
                যদি (!in_format_spec && bracket_depth == 0) {
                    যদি (tok->start == tok->cur - 1) {
                        প্রত্যাগম MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    গচ্ছ emit_মধ্য;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            গচ্ছ emit_মধ্য;
        } অন্যথা যদি (c == '\\') {
            পূর্ণাঙ্ক peek = tok_nextc(tok);
            যদি (peek == '\r') {
                peek = tok_nextc(tok);
            }
            যদি (peek == '{' || peek == '}') {
                যদি (!raw) {
                    যদি (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        প্রত্যাগম MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                অনুবর্ত;
            }

            যদি (!raw) {
                যদি (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    যদি (peek == '{') {
                        unicode_escape = 1;
                    } অন্যথা {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_end = tok->cur;
    যদি (p_end - quote_size == p_start) {
        পূর্ণাঙ্ক end_token = FTSTRING_END(current);
        Py_Lexer_বের_করো_FTString(tok);
        প্রত্যাগম MAKE_TOKEN(end_token);
    }
    প্রতি (পূর্ণাঙ্ক i = 0; i < quote_size; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_মধ্য:
    token_type = MAKE_TOKEN(FTSTRING_মধ্য(current));
    token->is_raw = raw;
    প্রত্যাগম token_type;
}
