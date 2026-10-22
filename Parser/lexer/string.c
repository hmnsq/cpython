#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "errcode.h"

#অন্তর্ভাব "lexer_internal.h"
#অন্তর্ভাব "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

স্থির পূর্ণসংখ্যা
string_error_token(কাঠামো tok_state *tok, কাঠামো token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_span = {start - location.byte_col, tok->inp},
    };
    পূর্ণসংখ্যা type = _PyLexer_token_setup(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->end_loc = (_PyTok_Loc){location.lineno, -1};
    ফেরত type;
}

স্থির পূর্ণসংখ্যা
incomplete_string_token(কাঠামো tok_state *tok, কাঠামো token *token,
                        পূর্ণসংখ্যা type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->done = E_INCOMPLETE;
    ফেরত type;
}

পূর্ণসংখ্যা
_PyLexer_record_ftstring_comment(কাঠামো tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off end)
{
    প্রতিজ্ঞা(state == _PyLexer_CurrentFTString(tok) && state->mode == FTSTRING_MODE_EXPRESSION);
    যদি (state->রাশিমালা_span.end >= 0) {
        ফেরত 0;
    }
    প্রতিজ্ঞা(state->রাশিমালা_span.start >= 0);
    tokenizer_comments *comments = state->comments;
    যদি (comments == ফাঁকা || comments->গুনতি == comments->capacity) {
        Py_ssize_t max_capacity = (PY_SSIZE_T_MAX -
            (Py_ssize_t)আকার(*comments)) /
            (Py_ssize_t)আকার(*comments->spans);
        যদি (comments != ফাঁকা && comments->capacity > max_capacity / 2) {
            PyErr_NoMemory();
            ফেরত -1;
        }
        Py_ssize_t capacity = comments == ফাঁকা ? 4 : comments->capacity * 2;
        আকার_t আকার_2 = আকার(*comments) +
            (আকার_t)capacity * আকার(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, আকার_2);
        যদি (resized == ফাঁকা) {
            PyErr_NoMemory();
            ফেরত -1;
        }
        comments = resized;
        যদি (state->comments == ফাঁকা) {
            comments->গুনতি = 0;
        }
        comments->capacity = capacity;
        state->comments = comments;
    }
    comments->spans[comments->গুনতি++] =
        (_PyTok_Span){start, end};
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
finish_ftstring_রাশিমালা(কাঠামো tok_state *tok, ftstring_state *state,
                     কাঠামো token *token)
{
    প্রতিজ্ঞা(token != ফাঁকা && state == _PyLexer_CurrentFTString(tok));
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION && tok->start >= 0);

    যদি (state->রাশিমালা_span.end >= 0) {
        ফেরত 0;
    }
    প্রতিজ্ঞা(state->রাশিমালা_span.start >= 0);
    state->রাশিমালা_span.end = tok->start;
    পূর্ণসংখ্যা tstring_interpolation = _PyLexer_IsTString(state->kind) &&
        state->replacement_depth == 1;
    যদি (!(state->debug_রাশিমালা || tstring_interpolation) || token->metadata) {
        ফেরত 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    যদি (comments != ফাঁকা && comments->গুনতি > 0) {
        Py_ssize_t stripped_আকার =
            state->রাশিমালা_span.end - state->রাশিমালা_span.start;
        Py_ssize_t comment_গুনতি = 0;
        জন্য (Py_ssize_t i = 0; i < comments->গুনতি; i++) {
            _PyTok_Span comment = comments->spans[i];
            প্রতিজ্ঞা(_PyTok_SpanIsValid(comment));
            প্রতিজ্ঞা(comment.start >= (i == 0 ? state->রাশিমালা_span.start
                                           : comments->spans[i - 1].end));
            যদি (comment.start >= state->রাশিমালা_span.end) {
                বিরতি;
            }
            প্রতিজ্ঞা(comment.end <= state->রাশিমালা_span.end);
            stripped_আকার -= comment.end - comment.start;
            comment_গুনতি++;
        }
        অক্ষর *stripped = PyMem_Malloc((আকার_t)stripped_আকার);
        যদি (stripped == ফাঁকা) {
            PyErr_NoMemory();
            ফেরত -1;
        }
        _PyTok_Span kept = {state->রাশিমালা_span.start, state->রাশিমালা_span.start};
        Py_ssize_t stripped_len = 0;
        জন্য (Py_ssize_t i = 0; i < comment_গুনতি; i++) {
            kept.end = comments->spans[i].start;
            Py_ssize_t দৈর্ঘ্য;
            ধ্রুবক অক্ষর *text = _PyTok_SourceSpanView(
                &tok->source, kept, &দৈর্ঘ্য);
            memcpy(stripped + stripped_len, text, (আকার_t)দৈর্ঘ্য);
            stripped_len += দৈর্ঘ্য;
            kept.start = comments->spans[i].end;
        }
        kept.end = state->রাশিমালা_span.end;
        Py_ssize_t দৈর্ঘ্য;
        ধ্রুবক অক্ষর *text = _PyTok_SourceSpanView(&tok->source, kept, &দৈর্ঘ্য);
        memcpy(stripped + stripped_len, text, (আকার_t)দৈর্ঘ্য);
        res = PyUnicode_DecodeUTF8(stripped, stripped_আকার, ফাঁকা);
        PyMem_Free(stripped);
    }
    অন্যথা {
        Py_ssize_t রাশিমালা_len;
        ধ্রুবক অক্ষর *রাশিমালা = _PyTok_SourceSpanView(
            &tok->source, state->রাশিমালা_span, &রাশিমালা_len);
        res = PyUnicode_DecodeUTF8(রাশিমালা, রাশিমালা_len, ফাঁকা);
    }

    যদি (!res) {
        ফেরত -1;
    }
    token->metadata = res;
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
begin_ftstring_রাশিমালা(কাঠামো tok_state *tok, ftstring_state *state,
                    _PyTok_Off রাশিমালা_start)
{
    প্রতিজ্ঞা(state->mode != FTSTRING_MODE_EXPRESSION);
    state->রাশিমালা_span = (_PyTok_Span){রাশিমালা_start, -1};
    যদি (state->comments != ফাঁকা) {
        state->comments->গুনতি = 0;
    }
    যদি (state->replacement_depth >= MAX_রাশিমালা_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            _PyLexer_StringPrefix(state->kind));
        ফেরত -1;
    }
    state->replacement_depth++;
    state->mode = FTSTRING_MODE_EXPRESSION;
    state->debug_রাশিমালা = 0;
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyLexer_ftstring_punctuation(কাঠামো tok_state *tok, ftstring_state *state,
                              কাঠামো token *token, পূর্ণসংখ্যা c)
{
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION);
    প্রতিজ্ঞা(c == ':' || c == '}' || c == '!');
    যদি (_PyLexer_FTStringBracketDepth(tok, state) != state->replacement_depth) {
        ফেরত 0;
    }
    যদি (c == '!') {
        পূর্ণসংখ্যা পরবর্তী = tok_nextc(tok);
        tok_backup(tok, পরবর্তী);
        যদি (পরবর্তী == '=') {
            ফেরত 0;
        }
    }
    যদি (finish_ftstring_রাশিমালা(tok, state, token) < 0) {
        ফেরত -1;
    }
    যদি (c == ':') {
        state->mode = FTSTRING_MODE_FORMAT_SPEC;
        ফেরত COLON;
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
Py_Lexer_close_ftstring_রাশিমালা(কাঠামো tok_state *tok, ftstring_state *state,
                             পূর্ণসংখ্যা c)
{
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION);
    প্রতিজ্ঞা(c == ')' || c == ']' || c == '}');
    পূর্ণসংখ্যা depth = _PyLexer_FTStringBracketDepth(tok, state);
    যদি (depth < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                _PyLexer_StringPrefix(state->kind), c);
        ফেরত -1;
    }
    যদি (c == '}' && depth == state->replacement_depth - 1) {
        state->replacement_depth--;
        state->mode = FTSTRING_MODE_মধ্য;
        state->debug_রাশিমালা = 0;
    }
    ফেরত 0;
}

শূন্য
_PyLexer_mark_ftstring_debug(কাঠামো tok_state *tok, ftstring_state *state)
{
    প্রতিজ্ঞা(state->mode == FTSTRING_MODE_EXPRESSION);
    যদি (_PyLexer_FTStringBracketDepth(tok, state) == state->replacement_depth) {
        state->debug_রাশিমালা = 1;
    }
}

পূর্ণসংখ্যা
_PyLexer_check_string_prefixes(কাঠামো tok_state *tok,
                                             পূর্ণসংখ্যা saw_b, পূর্ণসংখ্যা saw_r, পূর্ণসংখ্যা saw_u,
                                             পূর্ণসংখ্যা saw_f, পূর্ণসংখ্যা saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#define RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    করো {                                                                  \
        (শূন্য)_PyTokenizer_syntaxerror_known_range(                       \
            tok, (পূর্ণসংখ্যা)(tok->start + 1 - tok->সারি_start),                 \
            (পূর্ণসংখ্যা)(tok->cur - tok->সারি_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ফেরত -1;                                                        \
    } যখন (0)

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

    ফেরত 0;
}

পূর্ণসংখ্যা
_PyLexer_scan_fstring_start(কাঠামো tok_state *tok, কাঠামো token *token,
                            পূর্ণসংখ্যা c, ftstring_kind kind)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    পূর্ণসংখ্যা quote = c;
    পূর্ণসংখ্যা quote_আকার = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    পূর্ণসংখ্যা after_quote = tok_nextc(tok);
    যদি (after_quote == quote) {
        পূর্ণসংখ্যা after_after_quote = tok_nextc(tok);
        যদি (after_after_quote == quote) {
            quote_আকার = 3;
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
    যদি (state == ফাঁকা) {
        ফেরত MAKE_TOKEN(ERRORTOKEN);
    }
    state->mode = FTSTRING_MODE_মধ্য;
    state->quote = quote;
    state->quote_আকার = quote_আকার;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->রাশিমালা_span = (_PyTok_Span){-1, -1};

    state->kind = kind;
    ফেরত _PyLexer_IsTString(kind)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

পূর্ণসংখ্যা
_PyLexer_scan_string(কাঠামো tok_state *tok, কাঠামো token *token, পূর্ণসংখ্যা c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    পূর্ণসংখ্যা quote = c;
    পূর্ণসংখ্যা quote_আকার = 1;             /* 1 or 3 */
    পূর্ণসংখ্যা end_quote_আকার = 0;
    পূর্ণসংখ্যা has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    যদি (c == quote) {
        c = tok_nextc(tok);
        যদি (c == quote) {
            quote_আকার = 3;
        }
        অন্যথা {
            end_quote_আকার = 1;     /* empty string found */
        }
    }
    যদি (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    যখন (end_quote_আকার != quote_আকার) {
        c = tok_nextc(tok);
        যদি (tok->done == E_ERROR) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
        যদি (tok->done == E_DECODE) {
            বিরতি;
        }
        যদি (c == EOF || (quote_আকার == 1 && c == '\n')) {
            যদি (tok_failed(tok)) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
            যদি (c == EOF && tok->allow_incomplete_প্রবেশ) {
                ফেরত incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            পূর্ণসংখ্যা end_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ধ্রুবক অক্ষর *সারি = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            ধ্রুবক ftstring_state *state = _PyLexer_CurrentFTString(tok);
            যদি (state != ফাঁকা) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                যদি (state->quote == quote && state->quote_আকার == quote_আকার) {
                    পূর্ণসংখ্যা level = state->paren_level + state->replacement_depth - 1;
                    প্রতিজ্ঞা(level >= 0 && level < tok->level);
                    প্রতিজ্ঞা(tok->parenstack[level] == '{');
                    পূর্ণসংখ্যা lineno = tok->parenlinenostack[level];
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
                    ফেরত string_error_token(tok, token, tok->start, location);
                }
            }

            যদি (quote_আকার == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, সারি, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", end_lineno);
                যদি (c != '\n') {
                    tok->done = E_EOFS;
                }
                ফেরত string_error_token(tok, token, tok->start, location);
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
                ফেরত string_error_token(tok, token, tok->start, location);
            }
        }
        যদি (c == quote) {
            end_quote_আকার += 1;
        }
        অন্যথা {
            end_quote_আকার = 0;
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
    ফেরত MAKE_TOKEN(STRING);
}

পূর্ণসংখ্যা
_PyLexer_get_ftstring(কাঠামো tok_state *tok, ftstring_state *current, কাঠামো token *token)
{
    প্রতিজ্ঞা(current == _PyLexer_CurrentFTString(tok) && current->mode != FTSTRING_MODE_EXPRESSION);
    প্রতিজ্ঞা((current->quote_আকার == 1 || current->quote_আকার == 3) &&
           current->replacement_depth <= MAX_রাশিমালা_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    পূর্ণসংখ্যা token_type;
    পূর্ণসংখ্যা end_quote_আকার = 0;
    পূর্ণসংখ্যা unicode_escape = 0;
    পূর্ণসংখ্যা quote = current->quote;
    পূর্ণসংখ্যা quote_আকার = current->quote_আকার;
    পূর্ণসংখ্যা in_format_spec = current->mode == FTSTRING_MODE_FORMAT_SPEC;
    পূর্ণসংখ্যা raw = _PyLexer_IsRawString(current->kind);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    যখন (end_quote_আকার != quote_আকার) {
        পূর্ণসংখ্যা c = tok_nextc(tok);
        যদি (tok->done == E_ERROR || tok->done == E_DECODE) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }

        যদি (c == EOF || (quote_আকার == 1 && c == '\n')) {
            যদি (tok_failed(tok)) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
            যদি (c == EOF && tok->allow_incomplete_প্রবেশ) {
                ফেরত incomplete_string_token(tok, token,
                    FTSTRING_মধ্য(current), current->start, current->start_loc);
            }

            যদি (in_format_spec && c == '\n') {
                ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    _PyLexer_StringPrefix(current->kind),
                    _PyLexer_StringPrefix(current->kind)));
            }

            পূর্ণসংখ্যা end_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ধ্রুবক অক্ষর *সারি = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_offset = (Py_ssize_t)location.byte_col + 1;

            যদি (quote_আকার == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, সারি, cursor_offset, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                যদি (c != '\n') {
                    tok->done = E_EOFS;
                }
                ফেরত string_error_token(tok, token,
                    current->start, location);
            }
            অন্যথা {
                _PyTokenizer_syntaxerror_at(
                    tok, সারি, cursor_offset, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    _PyLexer_StringPrefix(current->kind), end_lineno);
                ফেরত string_error_token(tok, token,
                    current->start, location);
            }
        }

        যদি (c == quote) {
            end_quote_আকার += 1;
            চালু;
        } অন্যথা {
            end_quote_আকার = 0;
        }

        যদি (c == '{') {
            পূর্ণসংখ্যা peek = tok_nextc(tok);
            যদি (peek != '{' || in_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off রাশিমালা_start = tok->cur;
                tok_backup(tok, c);
                যদি (begin_ftstring_রাশিমালা(tok, current, রাশিমালা_start) < 0) {
                    ফেরত MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_end = tok->cur;
                যদি (p_start == p_end) {
                    ফেরত _PyLexer_get_normal(tok, current, token);
                }
            } অন্যথা {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            যাও emit_মধ্য;
        } অন্যথা যদি (c == '}') {
            যদি (unicode_escape) {
                p_start = tok->start;
                p_end = tok->cur;
                যাও emit_মধ্য;
            }
            পূর্ণসংখ্যা peek = tok_nextc(tok);

            পূর্ণসংখ্যা bracket_depth = _PyLexer_FTStringBracketDepth(tok, current);
            যদি (peek == '}' && !in_format_spec && bracket_depth == 0) {
                p_start = tok->start;
                p_end = tok->cur - 1;
            }
            অন্যথা {
                tok_backup(tok, peek);
                যদি (!in_format_spec && bracket_depth == 0) {
                    যদি (tok->start == tok->cur - 1) {
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            _PyLexer_StringPrefix(current->kind)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_end = tok->cur;
                    যাও emit_মধ্য;
                }
                tok_backup(tok, c);
                current->mode = FTSTRING_MODE_EXPRESSION;
                p_start = tok->start;
                p_end = tok->cur;
            }
            যাও emit_মধ্য;
        } অন্যথা যদি (c == '\\') {
            পূর্ণসংখ্যা peek = tok_nextc(tok);
            যদি (peek == '\r') {
                peek = tok_nextc(tok);
            }
            যদি (peek == '{' || peek == '}') {
                যদি (!raw) {
                    যদি (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ফেরত MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                চালু;
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
    যদি (p_end - quote_আকার == p_start) {
        পূর্ণসংখ্যা end_token = FTSTRING_END(current);
        Py_Lexer_বের_করো_FTString(tok);
        ফেরত MAKE_TOKEN(end_token);
    }
    জন্য (পূর্ণসংখ্যা i = 0; i < quote_আকার; i++) {
        tok_backup(tok, quote);
    }
    p_end = tok->cur;
emit_মধ্য:
    token_type = MAKE_TOKEN(FTSTRING_মধ্য(current));
    token->is_raw = raw;
    ফেরত token_type;
}
