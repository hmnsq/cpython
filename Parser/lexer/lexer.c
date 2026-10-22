#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "pycore_unicodeobject.h"
#অন্তর্ভাব "errcode.h"

#অন্তর্ভাব "lexer_internal.h"
#অন্তর্ভাব "../tokenizer/helpers.h"
#অন্তর্ভাব "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
স্থির ধ্রুবক অক্ষর* type_comment_prefix = "# type: ";

পূর্ণসংখ্যা
_PyLexer_refill(কাঠামো tok_state *tok)
{
    যদি (tok->done != E_OK) {
        ফেরত 0;
    }
    পূর্ণসংখ্যা rc = _PyTok_ReaderUnderflow(tok);
#যদি defined(Py_DEBUG)
    যদি (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    যদি (!rc) {
        tok->cur = tok->inp;
        ফেরত 0;
    }
    tok->সারি_start = tok->cur;
    _PyTok_Span সারি = {tok->সারি_start, tok->inp};
    যদি (_PyTok_SourceFindByte(&tok->source, সারি, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ফেরত 0;
    }
    ফেরত 1;
}

/* Back-up one character */
শূন্য
_PyLexer_backup(কাঠামো tok_state *tok, পূর্ণসংখ্যা c)
{
    যদি (c != EOF) {
        যদি (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        যদি (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
স্থির পূর্ণসংখ্যা
verify_identifier(কাঠামো tok_state *tok)
{
    যদি (tok->tok_extra_tokens) {
        ফেরত 1;
    }
    PyObject *s;
    যদি (tok_failed(tok))
        ফেরত 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, ফাঁকা);
    যদি (s == ফাঁকা) {
        যদি (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        অন্যথা {
            tok->done = E_ERROR;
        }
        ফেরত 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    প্রতিজ্ঞা(invalid >= 0);
    প্রতিজ্ঞা(Py_Unicode_GET_দৈর্ঘ্য(s) > 0);
    যদি (invalid < Py_Unicode_GET_দৈর্ঘ্য(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        যদি (invalid + 1 < Py_Unicode_GET_দৈর্ঘ্য(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            যদি (s != ফাঁকা) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            যদি (s == ফাঁকা) {
                tok->done = E_ERROR;
                ফেরত 0;
            }
            error_cursor = tok->start + Py_Bytes_GET_আকার(s);
        }
        Py_DECREF(s);
        যদি (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->সারি_start),
                error_cursor - tok->সারি_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        অন্যথা {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->সারি_start),
                error_cursor - tok->সারি_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ফেরত 0;
    }
    Py_DECREF(s);
    ফেরত 1;
}

পূর্ণসংখ্যা
_PyLexer_get_normal(কাঠামো tok_state *tok, ftstring_state *current, কাঠামো token *token)
{
    প্রতিজ্ঞা(current == ফাঁকা ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    পূর্ণসংখ্যা c;
    পূর্ণসংখ্যা blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    যদি (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_সারি(tok);
        যদি (blankline < 0) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->সারি_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    যদি (tok->layout.pending != 0) {
        ফেরত _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    করো {
        c = tok_nextc(tok);
    } যখন (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->সারি_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    যদি (c == '#') {

        যখন (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        যদি (current != ফাঁকা) {
            _PyTok_Off comment_end = tok->cur;
            যদি (c == '\n' || c == '\r') {
                comment_end--;
            }
            যদি (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        যদি (tok->type_comments) {
            ধ্রুবক অক্ষর *prefix = type_comment_prefix;
            যখন (*prefix && comment < tok->cur) {
                যদি (*prefix == ' ') {
                    যখন (comment < tok->cur) {
                        পূর্ণসংখ্যা ch = _PyTok_SourceByte(&tok->source, comment);
                        যদি (ch != ' ' && ch != '\t') {
                            বিরতি;
                        }
                        comment++;
                    }
                } অন্যথা যদি (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } অন্যথা {
                    বিরতি;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            যদি (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                পূর্ণসংখ্যা is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                যদি (is_type_ignore && comment + 6 < tok->cur) {
                    পূর্ণসংখ্যা ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                পূর্ণসংখ্যা type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                পূর্ণসংখ্যা start_col_offset = tok->start_loc.byte_col +
                    (পূর্ণসংখ্যা)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                যদি (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ফেরত type;
            }
        }
        যদি (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        যদি (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ফেরত MAKE_TOKEN(COMMENT);
        }
    }

    যদি (tok->done == E_INTERACT_STOP) {
        ফেরত MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    যদি (c == EOF) {
        যদি (tok->level) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
        ফেরত MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    যদি (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        পূর্ণসংখ্যা saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        যখন (1) {
            যদি (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            অন্যথা যদি (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            অন্যথা যদি (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            অন্যথা যদি (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            অন্যথা যদি (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            অন্যথা {
                বিরতি;
            }
            c = tok_nextc(tok);
            যদি (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                পূর্ণসংখ্যা status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                যদি (status < 0) {
                    ফেরত MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                যদি (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ফেরত _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ফেরত _PyLexer_scan_string(tok, token, c);
            }
        }
        যখন (is_potential_identifier_char(c)) {
            যদি (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        যদি (nonascii && !verify_identifier(tok)) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ফেরত MAKE_TOKEN(NAME);
    }

    যদি (c == '\r') {
        c = tok_nextc(tok);
    }

    যদি (c == '\n') {
        পূর্ণসংখ্যা type = _PyLexer_Newline(tok, token, blankline);
        যদি (type == 0) {
            যাও nextline;
        }
        ফেরত type;
    }

    /* Period or number starting with period? */
    যদি (c == '.') {
        c = tok_nextc(tok);
        যদি (Py_ISDIGIT(c)) {
            ফেরত Py_Lexer_scan_সংখ্যা(tok, token, c, 1);
        } অন্যথা যদি (c == '.') {
            c = tok_nextc(tok);
            যদি (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ফেরত MAKE_TOKEN(ELLIPSIS);
            }
            অন্যথা {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        অন্যথা {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ফেরত MAKE_TOKEN(DOT);
    }


    /*সংখ্যা*/
    যদি (Py_ISDIGIT(c)) {
        ফেরত Py_Lexer_scan_সংখ্যা(tok, token, c, 0);
    }

    /* String */
    যদি (c == '\'' || c == '"') {
        ফেরত _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    যদি (c == '\\') {
        যদি ((c = Py_Lexer_Continue_সারি(tok)) == -1) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
        যাও again; /* Read next line */
    }

    /* Punctuation character */
    পূর্ণসংখ্যা is_punctuation = (c == ':' || c == '}' || c == '!');
    যদি (is_punctuation && current != ফাঁকা) {
        পূর্ণসংখ্যা type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        যদি (type < 0) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
        যদি (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ফেরত MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        পূর্ণসংখ্যা c2 = tok_nextc(tok);
        পূর্ণসংখ্যা current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        যদি (current_token != OP) {
            পূর্ণসংখ্যা c3 = tok_nextc(tok);
            পূর্ণসংখ্যা current_token3 = _PyToken_ThreeChars(c, c2, c3);
            যদি (current_token3 != OP) {
                current_token = current_token3;
            }
            অন্যথা {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ফেরত MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    নির্বাচন (c) {
    ক্ষেত্র '(':
    ক্ষেত্র '[':
    ক্ষেত্র '{':
        যদি (tok->level >= MAXLEVEL) {
            ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (পূর্ণসংখ্যা)(tok->start - tok->সারি_start);
        tok->level++;
        বিরতি;
    ক্ষেত্র ')':
    ক্ষেত্র ']':
    ক্ষেত্র '}':
        যদি (current != ফাঁকা &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            যদি (c == '}') {
                ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        যদি (!tok->tok_extra_tokens && !tok->level) {
            ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        যদি (tok->level > 0) {
            tok->level--;
            পূর্ণসংখ্যা opening = tok->parenstack[tok->level];
            যদি (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                যদি (current != ফাঁকা && opening == '{') {
                    পূর্ণসংখ্যা bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    যদি (bracket_depth == current->replacement_depth - 1) {
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                যদি (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                অন্যথা {
                    ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        যদি (current != ফাঁকা &&
                Py_Lexer_close_ftstring_রাশিমালা(tok, current, c) < 0) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
        বিরতি;
    পূর্বনির্ধারিত:
        বিরতি;
    }

    যদি (!Py_UNICODE_ISPRINTABLE(c)) {
        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    যদি (c == '=' && current != ফাঁকা) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ফেরত MAKE_TOKEN(_PyToken_OneChar(c));
}


শূন্য
_PyTokenizer_Get(কাঠামো tok_state *tok, কাঠামো token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    পূর্ণসংখ্যা ফলাফল;
    যদি (current == ফাঁকা) {
        ফলাফল = _PyLexer_get_normal(tok, ফাঁকা, token);
    }
    অন্যথা {
        নির্বাচন (current->mode) {
        ক্ষেত্র FTSTRING_MODE_EXPRESSION:
            ফলাফল = _PyLexer_get_normal(tok, current, token);
            বিরতি;
        ক্ষেত্র FTSTRING_MODE_মধ্য:
        ক্ষেত্র FTSTRING_MODE_FORMAT_SPEC:
            ফলাফল = _PyLexer_get_ftstring(tok, current, token);
            বিরতি;
        পূর্বনির্ধারিত:
            Py_UNREACHABLE();
        }
    }
    যদি (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        ফলাফল = ERRORTOKEN;
    }
    token->type = ফলাফল;
}
