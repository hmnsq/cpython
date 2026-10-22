#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_token.h"
#അന്തര്ഭാവ "pycore_unicodeobject.h"
#അന്തര്ഭാവ "errcode.h"

#അന്തര്ഭാവ "lexer_internal.h"
#അന്തര്ഭാവ "../tokenizer/helpers.h"
#അന്തര്ഭാവ "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
സ്ഥിതം സ്ഥിരം അക്ഷരം* type_comment_prefix = "# type: ";

പൂർണ്ണസംഖ്യ
_PyLexer_refill(ഘടന tok_state *tok)
{
    എങ്കിൽ (tok->done != E_OK) {
        തിരികെ 0;
    }
    പൂർണ്ണസംഖ്യ rc = _PyTok_ReaderUnderflow(tok);
#എങ്കിൽ defined(Py_DEBUG)
    എങ്കിൽ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    എങ്കിൽ (!rc) {
        tok->cur = tok->inp;
        തിരികെ 0;
    }
    tok->വരി_start = tok->cur;
    _PyTok_Span വരി = {tok->വരി_start, tok->inp};
    എങ്കിൽ (_PyTok_SourceFindByte(&tok->source, വരി, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        തിരികെ 0;
    }
    തിരികെ 1;
}

/* Back-up one character */
ശൂന്യം
_PyLexer_backup(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c != EOF) {
        എങ്കിൽ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        എങ്കിൽ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
സ്ഥിതം പൂർണ്ണസംഖ്യ
verify_identifier(ഘടന tok_state *tok)
{
    എങ്കിൽ (tok->tok_extra_tokens) {
        തിരികെ 1;
    }
    PyObject *s;
    എങ്കിൽ (tok_failed(tok))
        തിരികെ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, ശൂന്യ);
    എങ്കിൽ (s == ശൂന്യ) {
        എങ്കിൽ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        അല്ലെങ്കിൽ {
            tok->done = E_ERROR;
        }
        തിരികെ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    പ്രതിജ്ഞാ(invalid >= 0);
    പ്രതിജ്ഞാ(PyUnicode_GET_LENGTH(s) > 0);
    എങ്കിൽ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        എങ്കിൽ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            എങ്കിൽ (s != ശൂന്യ) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            എങ്കിൽ (s == ശൂന്യ) {
                tok->done = E_ERROR;
                തിരികെ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        എങ്കിൽ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->വരി_start),
                error_cursor - tok->വരി_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        അല്ലെങ്കിൽ {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->വരി_start),
                error_cursor - tok->വരി_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        തിരികെ 0;
    }
    Py_DECREF(s);
    തിരികെ 1;
}

പൂർണ്ണസംഖ്യ
_PyLexer_get_normal(ഘടന tok_state *tok, ftstring_state *current, ഘടന token *token)
{
    പ്രതിജ്ഞാ(current == ശൂന്യ ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    പൂർണ്ണസംഖ്യ c;
    പൂർണ്ണസംഖ്യ blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    എങ്കിൽ (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_വരി(tok);
        എങ്കിൽ (blankline < 0) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->വരി_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    എങ്കിൽ (tok->layout.pending != 0) {
        തിരികെ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    ചെയ്യുക {
        c = tok_nextc(tok);
    } ആകുമ്പോൾ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->വരി_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    എങ്കിൽ (c == '#') {

        ആകുമ്പോൾ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        എങ്കിൽ (current != ശൂന്യ) {
            _PyTok_Off comment_end = tok->cur;
            എങ്കിൽ (c == '\n' || c == '\r') {
                comment_end--;
            }
            എങ്കിൽ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        എങ്കിൽ (tok->type_comments) {
            സ്ഥിരം അക്ഷരം *prefix = type_comment_prefix;
            ആകുമ്പോൾ (*prefix && comment < tok->cur) {
                എങ്കിൽ (*prefix == ' ') {
                    ആകുമ്പോൾ (comment < tok->cur) {
                        പൂർണ്ണസംഖ്യ ch = _PyTok_SourceByte(&tok->source, comment);
                        എങ്കിൽ (ch != ' ' && ch != '\t') {
                            നിർത്തുക;
                        }
                        comment++;
                    }
                } അല്ലെങ്കിൽ എങ്കിൽ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } അല്ലെങ്കിൽ {
                    നിർത്തുക;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            എങ്കിൽ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                പൂർണ്ണസംഖ്യ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                എങ്കിൽ (is_type_ignore && comment + 6 < tok->cur) {
                    പൂർണ്ണസംഖ്യ ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                പൂർണ്ണസംഖ്യ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                പൂർണ്ണസംഖ്യ start_col_offset = tok->start_loc.byte_col +
                    (പൂർണ്ണസംഖ്യ)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                എങ്കിൽ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                തിരികെ type;
            }
        }
        എങ്കിൽ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        എങ്കിൽ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            തിരികെ MAKE_TOKEN(COMMENT);
        }
    }

    എങ്കിൽ (tok->done == E_INTERACT_STOP) {
        തിരികെ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    എങ്കിൽ (c == EOF) {
        എങ്കിൽ (tok->level) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
        തിരികെ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    എങ്കിൽ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        പൂർണ്ണസംഖ്യ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ആകുമ്പോൾ (1) {
            എങ്കിൽ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            അല്ലെങ്കിൽ എങ്കിൽ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            അല്ലെങ്കിൽ എങ്കിൽ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            അല്ലെങ്കിൽ {
                നിർത്തുക;
            }
            c = tok_nextc(tok);
            എങ്കിൽ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                പൂർണ്ണസംഖ്യ status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                എങ്കിൽ (status < 0) {
                    തിരികെ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                എങ്കിൽ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    തിരികെ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                തിരികെ _PyLexer_scan_string(tok, token, c);
            }
        }
        ആകുമ്പോൾ (is_potential_identifier_char(c)) {
            എങ്കിൽ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        എങ്കിൽ (nonascii && !verify_identifier(tok)) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        തിരികെ MAKE_TOKEN(NAME);
    }

    എങ്കിൽ (c == '\r') {
        c = tok_nextc(tok);
    }

    എങ്കിൽ (c == '\n') {
        പൂർണ്ണസംഖ്യ type = _PyLexer_Newline(tok, token, blankline);
        എങ്കിൽ (type == 0) {
            പോകുക nextline;
        }
        തിരികെ type;
    }

    /* Period or number starting with period? */
    എങ്കിൽ (c == '.') {
        c = tok_nextc(tok);
        എങ്കിൽ (Py_ISDIGIT(c)) {
            തിരികെ Py_Lexer_scan_സംഖ്യ(tok, token, c, 1);
        } അല്ലെങ്കിൽ എങ്കിൽ (c == '.') {
            c = tok_nextc(tok);
            എങ്കിൽ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                തിരികെ MAKE_TOKEN(ELLIPSIS);
            }
            അല്ലെങ്കിൽ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        അല്ലെങ്കിൽ {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        തിരികെ MAKE_TOKEN(DOT);
    }


    /*സംഖ്യ*/
    എങ്കിൽ (Py_ISDIGIT(c)) {
        തിരികെ Py_Lexer_scan_സംഖ്യ(tok, token, c, 0);
    }

    /* String */
    എങ്കിൽ (c == '\'' || c == '"') {
        തിരികെ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    എങ്കിൽ (c == '\\') {
        എങ്കിൽ ((c = Py_Lexer_Continue_വരി(tok)) == -1) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
        പോകുക again; /* Read next line */
    }

    /* Punctuation character */
    പൂർണ്ണസംഖ്യ is_punctuation = (c == ':' || c == '}' || c == '!');
    എങ്കിൽ (is_punctuation && current != ശൂന്യ) {
        പൂർണ്ണസംഖ്യ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        എങ്കിൽ (type < 0) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
        എങ്കിൽ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            തിരികെ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        പൂർണ്ണസംഖ്യ c2 = tok_nextc(tok);
        പൂർണ്ണസംഖ്യ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        എങ്കിൽ (current_token != OP) {
            പൂർണ്ണസംഖ്യ c3 = tok_nextc(tok);
            പൂർണ്ണസംഖ്യ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            എങ്കിൽ (current_token3 != OP) {
                current_token = current_token3;
            }
            അല്ലെങ്കിൽ {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            തിരികെ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    തിരഞ്ഞെടുപ്പ് (c) {
    അവസ്ഥ '(':
    അവസ്ഥ '[':
    അവസ്ഥ '{':
        എങ്കിൽ (tok->level >= MAXLEVEL) {
            തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (പൂർണ്ണസംഖ്യ)(tok->start - tok->വരി_start);
        tok->level++;
        നിർത്തുക;
    അവസ്ഥ ')':
    അവസ്ഥ ']':
    അവസ്ഥ '}':
        എങ്കിൽ (current != ശൂന്യ &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            എങ്കിൽ (c == '}') {
                തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        എങ്കിൽ (!tok->tok_extra_tokens && !tok->level) {
            തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        എങ്കിൽ (tok->level > 0) {
            tok->level--;
            പൂർണ്ണസംഖ്യ opening = tok->parenstack[tok->level];
            എങ്കിൽ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                എങ്കിൽ (current != ശൂന്യ && opening == '{') {
                    പൂർണ്ണസംഖ്യ bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    എങ്കിൽ (bracket_depth == current->replacement_depth - 1) {
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                എങ്കിൽ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                അല്ലെങ്കിൽ {
                    തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        എങ്കിൽ (current != ശൂന്യ &&
                Py_Lexer_close_ftstring_വ്യഞ്ജകം(tok, current, c) < 0) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    എങ്കിൽ (!Py_UNICODE_ISPRINTABLE(c)) {
        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    എങ്കിൽ (c == '=' && current != ശൂന്യ) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    തിരികെ MAKE_TOKEN(_PyToken_OneChar(c));
}


ശൂന്യം
_PyTokenizer_Get(ഘടന tok_state *tok, ഘടന token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    പൂർണ്ണസംഖ്യ result;
    എങ്കിൽ (current == ശൂന്യ) {
        result = _PyLexer_get_normal(tok, ശൂന്യ, token);
    }
    അല്ലെങ്കിൽ {
        തിരഞ്ഞെടുപ്പ് (current->mode) {
        അവസ്ഥ FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            നിർത്തുക;
        അവസ്ഥ FTSTRING_MODE_നടുവ്:
        അവസ്ഥ FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            Py_UNREACHABLE();
        }
    }
    എങ്കിൽ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
