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
स्थिर स्थिरांक अक्षर* type_comment_prefix = "# type: ";

पूर्णांक
_PyLexer_refill(संरचना tok_state *tok)
{
    जर (tok->done != E_OK) {
        परत 0;
    }
    पूर्णांक rc = _PyTok_ReaderUnderflow(tok);
#जर defined(Py_DEBUG)
    जर (tok->debug) {
        ख_लिखो(मानक_त्रुटि, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            मानक_त्रुटि, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        ख_लिखो(मानक_त्रुटि, "  tok->done = %d\n", tok->done);
    }
#endif
    जर (!rc) {
        tok->cur = tok->inp;
        परत 0;
    }
    tok->ओळ_start = tok->cur;
    _PyTok_Span ओळ = {tok->ओळ_start, tok->inp};
    जर (_PyTok_SourceFindByte(&tok->source, ओळ, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        परत 0;
    }
    परत 1;
}

/* Back-up one character */
शून्य
_PyLexer_backup(संरचना tok_state *tok, पूर्णांक c)
{
    जर (c != खातापूर्ण) {
        जर (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        जर (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
स्थिर पूर्णांक
verify_identifier(संरचना tok_state *tok)
{
    जर (tok->tok_extra_tokens) {
        परत 1;
    }
    PyObject *s;
    जर (tok_failed(tok))
        परत 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, रिक्त);
    जर (s == रिक्त) {
        जर (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        अन्यथा {
            tok->done = E_ERROR;
        }
        परत 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    प्रतिज्ञा(invalid >= 0);
    प्रतिज्ञा(PyUnicode_GET_LENGTH(s) > 0);
    जर (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        जर (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            जर (s != रिक्त) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            जर (s == रिक्त) {
                tok->done = E_ERROR;
                परत 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        जर (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->ओळ_start),
                error_cursor - tok->ओळ_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        अन्यथा {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->ओळ_start),
                error_cursor - tok->ओळ_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        परत 0;
    }
    Py_DECREF(s);
    परत 1;
}

पूर्णांक
_PyLexer_get_normal(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == रिक्त ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    पूर्णांक c;
    पूर्णांक blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    जर (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_ओळ(tok);
        जर (blankline < 0) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->ओळ_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    जर (tok->layout.pending != 0) {
        परत _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    करा {
        c = tok_nextc(tok);
    } पर्यंत (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->ओळ_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    जर (c == '#') {

        पर्यंत (c != खातापूर्ण && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        जर (current != रिक्त) {
            _PyTok_Off comment_end = tok->cur;
            जर (c == '\n' || c == '\r') {
                comment_end--;
            }
            जर (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                परत MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        जर (tok->type_comments) {
            स्थिरांक अक्षर *prefix = type_comment_prefix;
            पर्यंत (*prefix && comment < tok->cur) {
                जर (*prefix == ' ') {
                    पर्यंत (comment < tok->cur) {
                        पूर्णांक ch = _PyTok_SourceByte(&tok->source, comment);
                        जर (ch != ' ' && ch != '\t') {
                            थांबा;
                        }
                        comment++;
                    }
                } अन्यथा जर (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } अन्यथा {
                    थांबा;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            जर (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                पूर्णांक is_type_ignore = tok->cur - comment >= 6 &&
                    स_भेद(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                जर (is_type_ignore && comment + 6 < tok->cur) {
                    पूर्णांक ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                पूर्णांक type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                पूर्णांक start_col_offset = tok->start_loc.byte_col +
                    (पूर्णांक)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                जर (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                परत type;
            }
        }
        जर (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        जर (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            परत MAKE_TOKEN(COMMENT);
        }
    }

    जर (tok->done == E_INTERACT_STOP) {
        परत MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    जर (c == खातापूर्ण) {
        जर (tok->level) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
        परत MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    जर (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        पूर्णांक saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        पर्यंत (1) {
            जर (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            अन्यथा जर (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            अन्यथा जर (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            अन्यथा जर (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            अन्यथा जर (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            अन्यथा {
                थांबा;
            }
            c = tok_nextc(tok);
            जर (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                पूर्णांक status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                जर (status < 0) {
                    परत MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                जर (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    परत _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                परत _PyLexer_scan_string(tok, token, c);
            }
        }
        पर्यंत (is_potential_identifier_char(c)) {
            जर (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        जर (nonascii && !verify_identifier(tok)) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        परत MAKE_TOKEN(NAME);
    }

    जर (c == '\r') {
        c = tok_nextc(tok);
    }

    जर (c == '\n') {
        पूर्णांक type = _PyLexer_Newline(tok, token, blankline);
        जर (type == 0) {
            जा nextline;
        }
        परत type;
    }

    /* Period or number starting with period? */
    जर (c == '.') {
        c = tok_nextc(tok);
        जर (Py_ISDIGIT(c)) {
            परत Py_Lexer_scan_संख्या(tok, token, c, 1);
        } अन्यथा जर (c == '.') {
            c = tok_nextc(tok);
            जर (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                परत MAKE_TOKEN(ELLIPSIS);
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
        परत MAKE_TOKEN(DOT);
    }


    /*संख्या*/
    जर (Py_ISDIGIT(c)) {
        परत Py_Lexer_scan_संख्या(tok, token, c, 0);
    }

    /* String */
    जर (c == '\'' || c == '"') {
        परत _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    जर (c == '\\') {
        जर ((c = Py_Lexer_Continue_ओळ(tok)) == -1) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
        जा again; /* Read next line */
    }

    /* Punctuation character */
    पूर्णांक is_punctuation = (c == ':' || c == '}' || c == '!');
    जर (is_punctuation && current != रिक्त) {
        पूर्णांक type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        जर (type < 0) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
        जर (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            परत MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        पूर्णांक c2 = tok_nextc(tok);
        पूर्णांक current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        जर (current_token != OP) {
            पूर्णांक c3 = tok_nextc(tok);
            पूर्णांक current_token3 = _PyToken_ThreeChars(c, c2, c3);
            जर (current_token3 != OP) {
                current_token = current_token3;
            }
            अन्यथा {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            परत MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    निवड (c) {
    स्थिती '(':
    स्थिती '[':
    स्थिती '{':
        जर (tok->level >= MAXLEVEL) {
            परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (पूर्णांक)(tok->start - tok->ओळ_start);
        tok->level++;
        थांबा;
    स्थिती ')':
    स्थिती ']':
    स्थिती '}':
        जर (current != रिक्त &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            जर (c == '}') {
                परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            परत MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        जर (!tok->tok_extra_tokens && !tok->level) {
            परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        जर (tok->level > 0) {
            tok->level--;
            पूर्णांक opening = tok->parenstack[tok->level];
            जर (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                जर (current != रिक्त && opening == '{') {
                    पूर्णांक bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    जर (bracket_depth == current->replacement_depth - 1) {
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                जर (tok->parenlinenostack[tok->level] != tok->lineno) {
                    परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                अन्यथा {
                    परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        जर (current != रिक्त &&
                Py_Lexer_close_ftstring_अभिव्यक्ती(tok, current, c) < 0) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
        थांबा;
    पूर्वनिर्धारित:
        थांबा;
    }

    जर (!Py_UNICODE_ISPRINTABLE(c)) {
        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    जर (c == '=' && current != रिक्त) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    परत MAKE_TOKEN(_PyToken_OneChar(c));
}


शून्य
_PyTokenizer_Get(संरचना tok_state *tok, संरचना token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    पूर्णांक result;
    जर (current == रिक्त) {
        result = _PyLexer_get_normal(tok, रिक्त, token);
    }
    अन्यथा {
        निवड (current->mode) {
        स्थिती FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            थांबा;
        स्थिती FTSTRING_MODE_मधला:
        स्थिती FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            थांबा;
        पूर्वनिर्धारित:
            Py_UNREACHABLE();
        }
    }
    जर (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
