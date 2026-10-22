#شامل "Python.h"
#شامل "pycore_token.h"
#شامل "pycore_unicodeobject.h"
#شامل "errcode.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"
#شامل "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ایستا ثابت نویسه* type_comment_prefix = "# type: ";

صحیح
_PyLexer_refill(ساختار tok_state *tok)
{
    اگر (tok->done != E_OK) {
        بازگشت 0;
    }
    صحیح rc = _PyTok_ReaderUnderflow(tok);
#اگر defined(Py_DEBUG)
    اگر (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    اگر (!rc) {
        tok->cur = tok->inp;
        بازگشت 0;
    }
    tok->سطر_start = tok->cur;
    _PyTok_Span سطر = {tok->سطر_start, tok->inp};
    اگر (_PyTok_SourceFindByte(&tok->source, سطر, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        بازگشت 0;
    }
    بازگشت 1;
}

/* Back-up one character */
تهی
_PyLexer_backup(ساختار tok_state *tok, صحیح c)
{
    اگر (c != EOF) {
        اگر (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        اگر (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ایستا صحیح
verify_identifier(ساختار tok_state *tok)
{
    اگر (tok->tok_extra_tokens) {
        بازگشت 1;
    }
    PyObject *s;
    اگر (tok_failed(tok))
        بازگشت 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, پوچ);
    اگر (s == پوچ) {
        اگر (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        وگرنه {
            tok->done = E_ERROR;
        }
        بازگشت 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    أكد(invalid >= 0);
    أكد(PyUnicode_GET_LENGTH(s) > 0);
    اگر (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        اگر (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            اگر (s != پوچ) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            اگر (s == پوچ) {
                tok->done = E_ERROR;
                بازگشت 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        اگر (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->سطر_start),
                error_cursor - tok->سطر_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        وگرنه {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->سطر_start),
                error_cursor - tok->سطر_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        بازگشت 0;
    }
    Py_DECREF(s);
    بازگشت 1;
}

صحیح
_PyLexer_get_normal(ساختار tok_state *tok, ftstring_state *current, ساختار token *token)
{
    أكد(current == پوچ ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    صحیح c;
    صحیح blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    اگر (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_سطر(tok);
        اگر (blankline < 0) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->سطر_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    اگر (tok->layout.pending != 0) {
        بازگشت _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    انجامبده {
        c = tok_nextc(tok);
    } وقتی (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->سطر_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    اگر (c == '#') {

        وقتی (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        اگر (current != پوچ) {
            _PyTok_Off comment_end = tok->cur;
            اگر (c == '\n' || c == '\r') {
                comment_end--;
            }
            اگر (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        اگر (tok->type_comments) {
            ثابت نویسه *prefix = type_comment_prefix;
            وقتی (*prefix && comment < tok->cur) {
                اگر (*prefix == ' ') {
                    وقتی (comment < tok->cur) {
                        صحیح ch = _PyTok_SourceByte(&tok->source, comment);
                        اگر (ch != ' ' && ch != '\t') {
                            بشکن;
                        }
                        comment++;
                    }
                } وگرنه اگر (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } وگرنه {
                    بشکن;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            اگر (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                صحیح is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                اگر (is_type_ignore && comment + 6 < tok->cur) {
                    صحیح ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                صحیح type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                صحیح start_col_offset = tok->start_loc.byte_col +
                    (صحیح)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                اگر (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                بازگشت type;
            }
        }
        اگر (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        اگر (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            بازگشت MAKE_TOKEN(COMMENT);
        }
    }

    اگر (tok->done == E_INTERACT_STOP) {
        بازگشت MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    اگر (c == EOF) {
        اگر (tok->level) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
        بازگشت MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    اگر (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        صحیح saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        وقتی (1) {
            اگر (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            وگرنه اگر (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            وگرنه اگر (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            وگرنه اگر (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            وگرنه اگر (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            وگرنه {
                بشکن;
            }
            c = tok_nextc(tok);
            اگر (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                صحیح status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                اگر (status < 0) {
                    بازگشت MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                اگر (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    بازگشت _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                بازگشت _PyLexer_scan_string(tok, token, c);
            }
        }
        وقتی (is_potential_identifier_char(c)) {
            اگر (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        اگر (nonascii && !verify_identifier(tok)) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        بازگشت MAKE_TOKEN(NAME);
    }

    اگر (c == '\r') {
        c = tok_nextc(tok);
    }

    اگر (c == '\n') {
        صحیح type = _PyLexer_Newline(tok, token, blankline);
        اگر (type == 0) {
            برو nextline;
        }
        بازگشت type;
    }

    /* Period or number starting with period? */
    اگر (c == '.') {
        c = tok_nextc(tok);
        اگر (Py_ISDIGIT(c)) {
            بازگشت Py_Lexer_scan_عدد(tok, token, c, 1);
        } وگرنه اگر (c == '.') {
            c = tok_nextc(tok);
            اگر (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                بازگشت MAKE_TOKEN(ELLIPSIS);
            }
            وگرنه {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        وگرنه {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        بازگشت MAKE_TOKEN(DOT);
    }


    /*عدد*/
    اگر (Py_ISDIGIT(c)) {
        بازگشت Py_Lexer_scan_عدد(tok, token, c, 0);
    }

    /* String */
    اگر (c == '\'' || c == '"') {
        بازگشت _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    اگر (c == '\\') {
        اگر ((c = Py_Lexer_Continue_سطر(tok)) == -1) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
        برو again; /* Read next line */
    }

    /* Punctuation character */
    صحیح is_punctuation = (c == ':' || c == '}' || c == '!');
    اگر (is_punctuation && current != پوچ) {
        صحیح type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        اگر (type < 0) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
        اگر (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            بازگشت MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        صحیح c2 = tok_nextc(tok);
        صحیح current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        اگر (current_token != OP) {
            صحیح c3 = tok_nextc(tok);
            صحیح current_token3 = _PyToken_ThreeChars(c, c2, c3);
            اگر (current_token3 != OP) {
                current_token = current_token3;
            }
            وگرنه {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            بازگشت MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    انتخاب (c) {
    حالت '(':
    حالت '[':
    حالت '{':
        اگر (tok->level >= MAXLEVEL) {
            بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (صحیح)(tok->start - tok->سطر_start);
        tok->level++;
        بشکن;
    حالت ')':
    حالت ']':
    حالت '}':
        اگر (current != پوچ &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            اگر (c == '}') {
                بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        اگر (!tok->tok_extra_tokens && !tok->level) {
            بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        اگر (tok->level > 0) {
            tok->level--;
            صحیح opening = tok->parenstack[tok->level];
            اگر (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                اگر (current != پوچ && opening == '{') {
                    صحیح bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    اگر (bracket_depth == current->replacement_depth - 1) {
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                اگر (tok->parenlinenostack[tok->level] != tok->lineno) {
                    بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                وگرنه {
                    بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        اگر (current != پوچ &&
                Py_Lexer_close_ftstring_عبارت(tok, current, c) < 0) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
        بشکن;
    پیشفرض:
        بشکن;
    }

    اگر (!Py_UNICODE_ISPRINTABLE(c)) {
        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    اگر (c == '=' && current != پوچ) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    بازگشت MAKE_TOKEN(_PyToken_OneChar(c));
}


تهی
_PyTokenizer_Get(ساختار tok_state *tok, ساختار token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    صحیح result;
    اگر (current == پوچ) {
        result = _PyLexer_get_normal(tok, پوچ, token);
    }
    وگرنه {
        انتخاب (current->mode) {
        حالت FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            بشکن;
        حالت FTSTRING_MODE_میانه:
        حالت FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            بشکن;
        پیشفرض:
            Py_UNREACHABLE();
        }
    }
    اگر (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
