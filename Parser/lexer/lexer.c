#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "pycore_unicodeobject.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ساكن ثابت حرف* type_comment_prefix = "# type: ";

صحيح
_PyLexer_refill(بنية tok_state *tok)
{
    ܐܢ (tok->done != E_OK) {
        ܦܢܐ 0;
    }
    صحيح rc = _PyTok_ReaderUnderflow(tok);
#ܐܢ defined(Py_DEBUG)
    ܐܢ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ܐܢ (!rc) {
        tok->cur = tok->inp;
        ܦܢܐ 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    ܐܢ (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ܦܢܐ 0;
    }
    ܦܢܐ 1;
}

/* Back-up one character */
فراغ
_PyLexer_backup(بنية tok_state *tok, صحيح c)
{
    ܐܢ (c != EOF) {
        ܐܢ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ܐܢ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ساكن صحيح
verify_identifier(بنية tok_state *tok)
{
    ܐܢ (tok->tok_extra_tokens) {
        ܦܢܐ 1;
    }
    PyObject *s;
    ܐܢ (tok_failed(tok))
        ܦܢܐ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, عدم);
    ܐܢ (s == عدم) {
        ܐܢ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        ܐܚܪܢܐ {
            tok->done = E_ERROR;
        }
        ܦܢܐ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    أكد(invalid >= 0);
    أكد(PyUnicode_GET_LENGTH(s) > 0);
    ܐܢ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ܐܢ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ܐܢ (s != عدم) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ܐܢ (s == عدم) {
                tok->done = E_ERROR;
                ܦܢܐ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ܐܢ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ܐܚܪܢܐ {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ܦܢܐ 0;
    }
    Py_DECREF(s);
    ܦܢܐ 1;
}

صحيح
_PyLexer_get_normal(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == عدم ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    صحيح c;
    صحيح blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ܐܢ (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        ܐܢ (blankline < 0) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ܐܢ (tok->layout.pending != 0) {
        ܦܢܐ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    ܥܒܕ {
        c = tok_nextc(tok);
    } ܟܡܐ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ܐܢ (c == '#') {

        ܟܡܐ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ܐܢ (current != عدم) {
            _PyTok_Off comment_end = tok->cur;
            ܐܢ (c == '\n' || c == '\r') {
                comment_end--;
            }
            ܐܢ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ܐܢ (tok->type_comments) {
            ثابت حرف *prefix = type_comment_prefix;
            ܟܡܐ (*prefix && comment < tok->cur) {
                ܐܢ (*prefix == ' ') {
                    ܟܡܐ (comment < tok->cur) {
                        صحيح ch = _PyTok_SourceByte(&tok->source, comment);
                        ܐܢ (ch != ' ' && ch != '\t') {
                            اكسر;
                        }
                        comment++;
                    }
                } ܐܚܪܢܐ ܐܢ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } ܐܚܪܢܐ {
                    اكسر;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ܐܢ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                صحيح is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ܐܢ (is_type_ignore && comment + 6 < tok->cur) {
                    صحيح ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                صحيح type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                صحيح start_col_offset = tok->start_loc.byte_col +
                    (صحيح)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ܐܢ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ܦܢܐ type;
            }
        }
        ܐܢ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ܐܢ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ܦܢܐ MAKE_TOKEN(COMMENT);
        }
    }

    ܐܢ (tok->done == E_INTERACT_STOP) {
        ܦܢܐ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ܐܢ (c == EOF) {
        ܐܢ (tok->level) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
        ܦܢܐ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ܐܢ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        صحيح saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ܟܡܐ (1) {
            ܐܢ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ܐܚܪܢܐ ܐܢ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ܐܚܪܢܐ ܐܢ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ܐܚܪܢܐ ܐܢ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ܐܚܪܢܐ ܐܢ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ܐܚܪܢܐ {
                اكسر;
            }
            c = tok_nextc(tok);
            ܐܢ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                صحيح status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ܐܢ (status < 0) {
                    ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ܐܢ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ܦܢܐ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ܦܢܐ _PyLexer_scan_string(tok, token, c);
            }
        }
        ܟܡܐ (is_potential_identifier_char(c)) {
            ܐܢ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ܐܢ (nonascii && !verify_identifier(tok)) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ܦܢܐ MAKE_TOKEN(NAME);
    }

    ܐܢ (c == '\r') {
        c = tok_nextc(tok);
    }

    ܐܢ (c == '\n') {
        صحيح type = _PyLexer_Newline(tok, token, blankline);
        ܐܢ (type == 0) {
            اذهب nextline;
        }
        ܦܢܐ type;
    }

    /* Period or number starting with period? */
    ܐܢ (c == '.') {
        c = tok_nextc(tok);
        ܐܢ (Py_ISDIGIT(c)) {
            ܦܢܐ _PyLexer_scan_number(tok, token, c, 1);
        } ܐܚܪܢܐ ܐܢ (c == '.') {
            c = tok_nextc(tok);
            ܐܢ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ܦܢܐ MAKE_TOKEN(ELLIPSIS);
            }
            ܐܚܪܢܐ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ܐܚܪܢܐ {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ܦܢܐ MAKE_TOKEN(DOT);
    }


    /* Number */
    ܐܢ (Py_ISDIGIT(c)) {
        ܦܢܐ _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    ܐܢ (c == '\'' || c == '"') {
        ܦܢܐ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ܐܢ (c == '\\') {
        ܐܢ ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
        اذهب again; /* Read next line */
    }

    /* Punctuation character */
    صحيح is_punctuation = (c == ':' || c == '}' || c == '!');
    ܐܢ (is_punctuation && current != عدم) {
        صحيح type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ܐܢ (type < 0) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
        ܐܢ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ܦܢܐ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        صحيح c2 = tok_nextc(tok);
        صحيح current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ܐܢ (current_token != OP) {
            صحيح c3 = tok_nextc(tok);
            صحيح current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ܐܢ (current_token3 != OP) {
                current_token = current_token3;
            }
            ܐܚܪܢܐ {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ܦܢܐ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    بدل (c) {
    حالة '(':
    حالة '[':
    حالة '{':
        ܐܢ (tok->level >= MAXLEVEL) {
            ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (صحيح)(tok->start - tok->line_start);
        tok->level++;
        اكسر;
    حالة ')':
    حالة ']':
    حالة '}':
        ܐܢ (current != عدم &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ܐܢ (c == '}') {
                ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ܐܢ (!tok->tok_extra_tokens && !tok->level) {
            ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ܐܢ (tok->level > 0) {
            tok->level--;
            صحيح opening = tok->parenstack[tok->level];
            ܐܢ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ܐܢ (current != عدم && opening == '{') {
                    صحيح bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ܐܢ (bracket_depth == current->replacement_depth - 1) {
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ܐܢ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ܐܚܪܢܐ {
                    ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ܐܢ (current != عدم &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    ܐܢ (!Py_UNICODE_ISPRINTABLE(c)) {
        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ܐܢ (c == '=' && current != عدم) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ܦܢܐ MAKE_TOKEN(_PyToken_OneChar(c));
}


فراغ
_PyTokenizer_Get(بنية tok_state *tok, بنية token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    صحيح result;
    ܐܢ (current == عدم) {
        result = _PyLexer_get_normal(tok, عدم, token);
    }
    ܐܚܪܢܐ {
        بدل (current->mode) {
        حالة FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            اكسر;
        حالة FTSTRING_MODE_MIDDLE:
        حالة FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            اكسر;
        افتراضي:
            Py_UNREACHABLE();
        }
    }
    ܐܢ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
