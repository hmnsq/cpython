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
स्थिर अचर वर्ण* type_comment_prefix = "# type: ";

पूर्णाङ्क
_PyLexer_refill(संरचना tok_state *tok)
{
    यदि (tok->done != E_OK) {
        फर्काऊ 0;
    }
    पूर्णाङ्क rc = _PyTok_ReaderUnderflow(tok);
#यदि defined(Py_DEBUG)
    यदि (tok->debug) {
        ख_लिखो(मानक_त्रुटि, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            मानक_त्रुटि, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        ख_लिखो(मानक_त्रुटि, "  tok->done = %d\n", tok->done);
    }
#endif
    यदि (!rc) {
        tok->cur = tok->inp;
        फर्काऊ 0;
    }
    tok->पङ्क्ति_start = tok->cur;
    _PyTok_Span पङ्क्ति = {tok->पङ्क्ति_start, tok->inp};
    यदि (_PyTok_SourceFindByte(&tok->source, पङ्क्ति, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        फर्काऊ 0;
    }
    फर्काऊ 1;
}

/* Back-up one character */
शून्य
_PyLexer_backup(संरचना tok_state *tok, पूर्णाङ्क c)
{
    यदि (c != खातापूर्ण) {
        यदि (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        यदि (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
स्थिर पूर्णाङ्क
verify_identifier(संरचना tok_state *tok)
{
    यदि (tok->tok_extra_tokens) {
        फर्काऊ 1;
    }
    PyObject *s;
    यदि (tok_failed(tok))
        फर्काऊ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, रिक्त);
    यदि (s == रिक्त) {
        यदि (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        अन्यथा {
            tok->done = E_ERROR;
        }
        फर्काऊ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    प्रतिज्ञा(invalid >= 0);
    प्रतिज्ञा(PyUnicode_GET_LENGTH(s) > 0);
    यदि (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        यदि (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            यदि (s != रिक्त) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            यदि (s == रिक्त) {
                tok->done = E_ERROR;
                फर्काऊ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        यदि (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्ति_start),
                error_cursor - tok->पङ्क्ति_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        अन्यथा {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्ति_start),
                error_cursor - tok->पङ्क्ति_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        फर्काऊ 0;
    }
    Py_DECREF(s);
    फर्काऊ 1;
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


    यदि (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_पङ्क्ति(tok);
        यदि (blankline < 0) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->पङ्क्ति_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    यदि (tok->layout.pending != 0) {
        फर्काऊ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    गर {
        c = tok_nextc(tok);
    } जबसम्म (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->पङ्क्ति_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    यदि (c == '#') {

        जबसम्म (c != खातापूर्ण && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        यदि (current != रिक्त) {
            _PyTok_Off comment_end = tok->cur;
            यदि (c == '\n' || c == '\r') {
                comment_end--;
            }
            यदि (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                फर्काऊ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        यदि (tok->type_comments) {
            अचर वर्ण *prefix = type_comment_prefix;
            जबसम्म (*prefix && comment < tok->cur) {
                यदि (*prefix == ' ') {
                    जबसम्म (comment < tok->cur) {
                        पूर्णाङ्क ch = _PyTok_SourceByte(&tok->source, comment);
                        यदि (ch != ' ' && ch != '\t') {
                            रोक;
                        }
                        comment++;
                    }
                } अन्यथा यदि (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } अन्यथा {
                    रोक;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            यदि (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                पूर्णाङ्क is_type_ignore = tok->cur - comment >= 6 &&
                    स_भेद(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                यदि (is_type_ignore && comment + 6 < tok->cur) {
                    पूर्णाङ्क ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                पूर्णाङ्क type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                पूर्णाङ्क start_col_offset = tok->start_loc.byte_col +
                    (पूर्णाङ्क)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                यदि (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                फर्काऊ type;
            }
        }
        यदि (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        यदि (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            फर्काऊ MAKE_TOKEN(COMMENT);
        }
    }

    यदि (tok->done == E_INTERACT_STOP) {
        फर्काऊ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    यदि (c == खातापूर्ण) {
        यदि (tok->level) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }
        फर्काऊ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    यदि (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        पूर्णाङ्क saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        जबसम्म (1) {
            यदि (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            अन्यथा यदि (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            अन्यथा यदि (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            अन्यथा यदि (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            अन्यथा यदि (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            अन्यथा {
                रोक;
            }
            c = tok_nextc(tok);
            यदि (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                पूर्णाङ्क status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                यदि (status < 0) {
                    फर्काऊ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                यदि (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    फर्काऊ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                फर्काऊ _PyLexer_scan_string(tok, token, c);
            }
        }
        जबसम्म (is_potential_identifier_char(c)) {
            यदि (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        यदि (nonascii && !verify_identifier(tok)) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        फर्काऊ MAKE_TOKEN(NAME);
    }

    यदि (c == '\r') {
        c = tok_nextc(tok);
    }

    यदि (c == '\n') {
        पूर्णाङ्क type = _PyLexer_Newline(tok, token, blankline);
        यदि (type == 0) {
            गच्छ nextline;
        }
        फर्काऊ type;
    }

    /* Period or number starting with period? */
    यदि (c == '.') {
        c = tok_nextc(tok);
        यदि (Py_ISDIGIT(c)) {
            फर्काऊ Py_Lexer_scan_सङ्ख्या(tok, token, c, 1);
        } अन्यथा यदि (c == '.') {
            c = tok_nextc(tok);
            यदि (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                फर्काऊ MAKE_TOKEN(ELLIPSIS);
            }
            अन्यथा {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        अन्यथा {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        फर्काऊ MAKE_TOKEN(DOT);
    }


    /*सङ्ख्या*/
    यदि (Py_ISDIGIT(c)) {
        फर्काऊ Py_Lexer_scan_सङ्ख्या(tok, token, c, 0);
    }

    /* String */
    यदि (c == '\'' || c == '"') {
        फर्काऊ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    यदि (c == '\\') {
        यदि ((c = Py_Lexer_Continue_पङ्क्ति(tok)) == -1) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }
        गच्छ again; /* Read next line */
    }

    /* Punctuation character */
    पूर्णाङ्क is_punctuation = (c == ':' || c == '}' || c == '!');
    यदि (is_punctuation && current != रिक्त) {
        पूर्णाङ्क type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        यदि (type < 0) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }
        यदि (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            फर्काऊ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        पूर्णाङ्क c2 = tok_nextc(tok);
        पूर्णाङ्क current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        यदि (current_token != OP) {
            पूर्णाङ्क c3 = tok_nextc(tok);
            पूर्णाङ्क current_token3 = _PyToken_ThreeChars(c, c2, c3);
            यदि (current_token3 != OP) {
                current_token = current_token3;
            }
            अन्यथा {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            फर्काऊ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    विकल्प (c) {
    पक्ष '(':
    पक्ष '[':
    पक्ष '{':
        यदि (tok->level >= MAXLEVEL) {
            फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (पूर्णाङ्क)(tok->start - tok->पङ्क्ति_start);
        tok->level++;
        रोक;
    पक्ष ')':
    पक्ष ']':
    पक्ष '}':
        यदि (current != रिक्त &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            यदि (c == '}') {
                फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        यदि (!tok->tok_extra_tokens && !tok->level) {
            फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        यदि (tok->level > 0) {
            tok->level--;
            पूर्णाङ्क opening = tok->parenstack[tok->level];
            यदि (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                यदि (current != रिक्त && opening == '{') {
                    पूर्णाङ्क bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    यदि (bracket_depth == current->replacement_depth - 1) {
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                यदि (tok->parenlinenostack[tok->level] != tok->lineno) {
                    फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                अन्यथा {
                    फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        यदि (current != रिक्त &&
                Py_Lexer_close_ftstring_अभिव्यक्ति(tok, current, c) < 0) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }
        रोक;
    स्वभाव:
        रोक;
    }

    यदि (!Py_UNICODE_ISPRINTABLE(c)) {
        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    यदि (c == '=' && current != रिक्त) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    फर्काऊ MAKE_TOKEN(_PyToken_OneChar(c));
}


शून्य
_PyTokenizer_Get(संरचना tok_state *tok, संरचना token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    पूर्णाङ्क result;
    यदि (current == रिक्त) {
        result = _PyLexer_get_normal(tok, रिक्त, token);
    }
    अन्यथा {
        विकल्प (current->mode) {
        पक्ष FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            रोक;
        पक्ष FTSTRING_MODE_बीच:
        पक्ष FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            रोक;
        स्वभाव:
            Py_UNREACHABLE();
        }
    }
    यदि (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
