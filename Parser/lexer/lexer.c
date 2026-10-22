#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "pycore_unicodeobject.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
स्थिर ध्रुव वर्ण* type_comment_prefix = "# type: ";

पूर्णाङ्क
_PyLexer_refill(संरचना tok_state *tok)
{
    सचे (tok->done != E_OK) {
        पटिनिवत्त 0;
    }
    पूर्णाङ्क rc = _PyTok_ReaderUnderflow(tok);
#सचे defined(Py_DEBUG)
    सचे (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    सचे (!rc) {
        tok->cur = tok->inp;
        पटिनिवत्त 0;
    }
    tok->पङ्क्तिः_start = tok->cur;
    _PyTok_Span पङ्क्तिः = {tok->पङ्क्तिः_start, tok->inp};
    सचे (_PyTok_SourceFindByte(&tok->source, पङ्क्तिः, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        पटिनिवत्त 0;
    }
    पटिनिवत्त 1;
}

/* Back-up one character */
शून्य
_PyLexer_backup(संरचना tok_state *tok, पूर्णाङ्क c)
{
    सचे (c != EOF) {
        सचे (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        सचे (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
स्थिर पूर्णाङ्क
verify_identifier(संरचना tok_state *tok)
{
    सचे (tok->tok_extra_tokens) {
        पटिनिवत्त 1;
    }
    PyObject *s;
    सचे (tok_failed(tok))
        पटिनिवत्त 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, रिक्त);
    सचे (s == रिक्त) {
        सचे (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        अञ्ञथा {
            tok->done = E_ERROR;
        }
        पटिनिवत्त 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    प्रतिज्ञा(invalid >= 0);
    प्रतिज्ञा(PyUnicode_GET_LENGTH(s) > 0);
    सचे (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        सचे (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            सचे (s != रिक्त) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            सचे (s == रिक्त) {
                tok->done = E_ERROR;
                पटिनिवत्त 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        सचे (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्तिः_start),
                error_cursor - tok->पङ्क्तिः_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        अञ्ञथा {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्तिः_start),
                error_cursor - tok->पङ्क्तिः_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        पटिनिवत्त 0;
    }
    Py_DECREF(s);
    पटिनिवत्त 1;
}

पूर्णाङ्क
_PyLexer_get_normal(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == रिक्त ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    पूर्णाङ्क c;
    पूर्णाङ्क blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    सचे (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_पङ्क्तिः(tok);
        सचे (blankline < 0) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->पङ्क्तिः_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    सचे (tok->layout.pending != 0) {
        पटिनिवत्त _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    करोहि {
        c = tok_nextc(tok);
    } याव (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->पङ्क्तिः_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    सचे (c == '#') {

        याव (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        सचे (current != रिक्त) {
            _PyTok_Off comment_end = tok->cur;
            सचे (c == '\n' || c == '\r') {
                comment_end--;
            }
            सचे (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        सचे (tok->type_comments) {
            ध्रुव वर्ण *prefix = type_comment_prefix;
            याव (*prefix && comment < tok->cur) {
                सचे (*prefix == ' ') {
                    याव (comment < tok->cur) {
                        पूर्णाङ्क ch = _PyTok_SourceByte(&tok->source, comment);
                        सचे (ch != ' ' && ch != '\t') {
                            भङ्ग;
                        }
                        comment++;
                    }
                } अञ्ञथा सचे (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } अञ्ञथा {
                    भङ्ग;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            सचे (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                पूर्णाङ्क is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                सचे (is_type_ignore && comment + 6 < tok->cur) {
                    पूर्णाङ्क ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                पूर्णाङ्क type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                पूर्णाङ्क start_col_offset = tok->start_loc.byte_col +
                    (पूर्णाङ्क)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                सचे (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                पटिनिवत्त type;
            }
        }
        सचे (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        सचे (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            पटिनिवत्त MAKE_TOKEN(COMMENT);
        }
    }

    सचे (tok->done == E_INTERACT_STOP) {
        पटिनिवत्त MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    सचे (c == EOF) {
        सचे (tok->level) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
        पटिनिवत्त MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    सचे (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        पूर्णाङ्क saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        याव (1) {
            सचे (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            अञ्ञथा सचे (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            अञ्ञथा सचे (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            अञ्ञथा सचे (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            अञ्ञथा सचे (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            अञ्ञथा {
                भङ्ग;
            }
            c = tok_nextc(tok);
            सचे (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                पूर्णाङ्क status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                सचे (status < 0) {
                    पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                सचे (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    पटिनिवत्त _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                पटिनिवत्त _PyLexer_scan_string(tok, token, c);
            }
        }
        याव (is_potential_identifier_char(c)) {
            सचे (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        सचे (nonascii && !verify_identifier(tok)) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        पटिनिवत्त MAKE_TOKEN(NAME);
    }

    सचे (c == '\r') {
        c = tok_nextc(tok);
    }

    सचे (c == '\n') {
        पूर्णाङ्क type = _PyLexer_Newline(tok, token, blankline);
        सचे (type == 0) {
            गच्छ nextline;
        }
        पटिनिवत्त type;
    }

    /* Period or number starting with period? */
    सचे (c == '.') {
        c = tok_nextc(tok);
        सचे (Py_ISDIGIT(c)) {
            पटिनिवत्त Py_Lexer_scan_सङ्ख्या(tok, token, c, 1);
        } अञ्ञथा सचे (c == '.') {
            c = tok_nextc(tok);
            सचे (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                पटिनिवत्त MAKE_TOKEN(ELLIPSIS);
            }
            अञ्ञथा {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        अञ्ञथा {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        पटिनिवत्त MAKE_TOKEN(DOT);
    }


    /*सङ्ख्या*/
    सचे (Py_ISDIGIT(c)) {
        पटिनिवत्त Py_Lexer_scan_सङ्ख्या(tok, token, c, 0);
    }

    /* String */
    सचे (c == '\'' || c == '"') {
        पटिनिवत्त _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    सचे (c == '\\') {
        सचे ((c = Py_Lexer_Continue_पङ्क्तिः(tok)) == -1) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
        गच्छ again; /* Read next line */
    }

    /* Punctuation character */
    पूर्णाङ्क is_punctuation = (c == ':' || c == '}' || c == '!');
    सचे (is_punctuation && current != रिक्त) {
        पूर्णाङ्क type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        सचे (type < 0) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
        सचे (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            पटिनिवत्त MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        पूर्णाङ्क c2 = tok_nextc(tok);
        पूर्णाङ्क current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        सचे (current_token != OP) {
            पूर्णाङ्क c3 = tok_nextc(tok);
            पूर्णाङ्क current_token3 = _PyToken_ThreeChars(c, c2, c3);
            सचे (current_token3 != OP) {
                current_token = current_token3;
            }
            अञ्ञथा {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            पटिनिवत्त MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    विकल्प (c) {
    पक्ष '(':
    पक्ष '[':
    पक्ष '{':
        सचे (tok->level >= MAXLEVEL) {
            पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (पूर्णाङ्क)(tok->start - tok->पङ्क्तिः_start);
        tok->level++;
        भङ्ग;
    पक्ष ')':
    पक्ष ']':
    पक्ष '}':
        सचे (current != रिक्त &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            सचे (c == '}') {
                पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        सचे (!tok->tok_extra_tokens && !tok->level) {
            पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        सचे (tok->level > 0) {
            tok->level--;
            पूर्णाङ्क opening = tok->parenstack[tok->level];
            सचे (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                सचे (current != रिक्त && opening == '{') {
                    पूर्णाङ्क bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    सचे (bracket_depth == current->replacement_depth - 1) {
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                सचे (tok->parenlinenostack[tok->level] != tok->lineno) {
                    पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                अञ्ञथा {
                    पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        सचे (current != रिक्त &&
                Py_Lexer_close_ftstring_व्यञ्जकम्(tok, current, c) < 0) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    सचे (!Py_UNICODE_ISPRINTABLE(c)) {
        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    सचे (c == '=' && current != रिक्त) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    पटिनिवत्त MAKE_TOKEN(_PyToken_OneChar(c));
}


शून्य
_PyTokenizer_Get(संरचना tok_state *tok, संरचना token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    पूर्णाङ्क result;
    सचे (current == रिक्त) {
        result = _PyLexer_get_normal(tok, रिक्त, token);
    }
    अञ्ञथा {
        विकल्प (current->mode) {
        पक्ष FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            भङ्ग;
        पक्ष FTSTRING_MODE_मध्यः:
        पक्ष FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            भङ्ग;
        स्वभाव:
            Py_UNREACHABLE();
        }
    }
    सचे (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
