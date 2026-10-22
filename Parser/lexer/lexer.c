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
    𐤀𐤌 (tok->done != E_OK) {
        أرجع 0;
    }
    صحيح rc = _PyTok_ReaderUnderflow(tok);
#𐤀𐤌 defined(Py_DEBUG)
    𐤀𐤌 (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    𐤀𐤌 (!rc) {
        tok->cur = tok->inp;
        أرجع 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    𐤀𐤌 (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        أرجع 0;
    }
    أرجع 1;
}

/* Back-up one character */
فراغ
_PyLexer_backup(بنية tok_state *tok, صحيح c)
{
    𐤀𐤌 (c != EOF) {
        𐤀𐤌 (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        𐤀𐤌 (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ساكن صحيح
verify_identifier(بنية tok_state *tok)
{
    𐤀𐤌 (tok->tok_extra_tokens) {
        أرجع 1;
    }
    PyObject *s;
    𐤀𐤌 (tok_failed(tok))
        أرجع 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, عدم);
    𐤀𐤌 (s == عدم) {
        𐤀𐤌 (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        وإلا {
            tok->done = E_ERROR;
        }
        أرجع 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    أكد(invalid >= 0);
    أكد(PyUnicode_GET_LENGTH(s) > 0);
    𐤀𐤌 (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        𐤀𐤌 (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            𐤀𐤌 (s != عدم) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            𐤀𐤌 (s == عدم) {
                tok->done = E_ERROR;
                أرجع 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        𐤀𐤌 (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        وإلا {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        أرجع 0;
    }
    Py_DECREF(s);
    أرجع 1;
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


    𐤀𐤌 (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        𐤀𐤌 (blankline < 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    𐤀𐤌 (tok->layout.pending != 0) {
        أرجع _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    𐤐𐤏𐤋 {
        c = tok_nextc(tok);
    } طالما (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    𐤀𐤌 (c == '#') {

        طالما (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        𐤀𐤌 (current != عدم) {
            _PyTok_Off comment_end = tok->cur;
            𐤀𐤌 (c == '\n' || c == '\r') {
                comment_end--;
            }
            𐤀𐤌 (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        𐤀𐤌 (tok->type_comments) {
            ثابت حرف *prefix = type_comment_prefix;
            طالما (*prefix && comment < tok->cur) {
                𐤀𐤌 (*prefix == ' ') {
                    طالما (comment < tok->cur) {
                        صحيح ch = _PyTok_SourceByte(&tok->source, comment);
                        𐤀𐤌 (ch != ' ' && ch != '\t') {
                            اكسر;
                        }
                        comment++;
                    }
                } وإلا 𐤀𐤌 (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } وإلا {
                    اكسر;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            𐤀𐤌 (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                صحيح is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                𐤀𐤌 (is_type_ignore && comment + 6 < tok->cur) {
                    صحيح ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                صحيح type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                صحيح start_col_offset = tok->start_loc.byte_col +
                    (صحيح)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                𐤀𐤌 (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                أرجع type;
            }
        }
        𐤀𐤌 (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        𐤀𐤌 (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            أرجع MAKE_TOKEN(COMMENT);
        }
    }

    𐤀𐤌 (tok->done == E_INTERACT_STOP) {
        أرجع MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    𐤀𐤌 (c == EOF) {
        𐤀𐤌 (tok->level) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        أرجع MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    𐤀𐤌 (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        صحيح saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        طالما (1) {
            𐤀𐤌 (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            وإلا 𐤀𐤌 (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            وإلا 𐤀𐤌 (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            وإلا 𐤀𐤌 (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            وإلا 𐤀𐤌 (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            وإلا {
                اكسر;
            }
            c = tok_nextc(tok);
            𐤀𐤌 (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                صحيح status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                𐤀𐤌 (status < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                𐤀𐤌 (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    أرجع _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                أرجع _PyLexer_scan_string(tok, token, c);
            }
        }
        طالما (is_potential_identifier_char(c)) {
            𐤀𐤌 (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        𐤀𐤌 (nonascii && !verify_identifier(tok)) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        أرجع MAKE_TOKEN(NAME);
    }

    𐤀𐤌 (c == '\r') {
        c = tok_nextc(tok);
    }

    𐤀𐤌 (c == '\n') {
        صحيح type = _PyLexer_Newline(tok, token, blankline);
        𐤀𐤌 (type == 0) {
            اذهب nextline;
        }
        أرجع type;
    }

    /* Period or number starting with period? */
    𐤀𐤌 (c == '.') {
        c = tok_nextc(tok);
        𐤀𐤌 (Py_ISDIGIT(c)) {
            أرجع _PyLexer_scan_number(tok, token, c, 1);
        } وإلا 𐤀𐤌 (c == '.') {
            c = tok_nextc(tok);
            𐤀𐤌 (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                أرجع MAKE_TOKEN(ELLIPSIS);
            }
            وإلا {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        وإلا {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        أرجع MAKE_TOKEN(DOT);
    }


    /* Number */
    𐤀𐤌 (Py_ISDIGIT(c)) {
        أرجع _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    𐤀𐤌 (c == '\'' || c == '"') {
        أرجع _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    𐤀𐤌 (c == '\\') {
        𐤀𐤌 ((c = _PyLexer_ContinueLine(tok)) == -1) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        اذهب again; /* Read next line */
    }

    /* Punctuation character */
    صحيح is_punctuation = (c == ':' || c == '}' || c == '!');
    𐤀𐤌 (is_punctuation && current != عدم) {
        صحيح type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        𐤀𐤌 (type < 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        𐤀𐤌 (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            أرجع MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        صحيح c2 = tok_nextc(tok);
        صحيح current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        𐤀𐤌 (current_token != OP) {
            صحيح c3 = tok_nextc(tok);
            صحيح current_token3 = _PyToken_ThreeChars(c, c2, c3);
            𐤀𐤌 (current_token3 != OP) {
                current_token = current_token3;
            }
            وإلا {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            أرجع MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    بدل (c) {
    حالة '(':
    حالة '[':
    حالة '{':
        𐤀𐤌 (tok->level >= MAXLEVEL) {
            أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (صحيح)(tok->start - tok->line_start);
        tok->level++;
        اكسر;
    حالة ')':
    حالة ']':
    حالة '}':
        𐤀𐤌 (current != عدم &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            𐤀𐤌 (c == '}') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        𐤀𐤌 (!tok->tok_extra_tokens && !tok->level) {
            أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        𐤀𐤌 (tok->level > 0) {
            tok->level--;
            صحيح opening = tok->parenstack[tok->level];
            𐤀𐤌 (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                𐤀𐤌 (current != عدم && opening == '{') {
                    صحيح bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    𐤀𐤌 (bracket_depth == current->replacement_depth - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                𐤀𐤌 (tok->parenlinenostack[tok->level] != tok->lineno) {
                    أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                وإلا {
                    أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        𐤀𐤌 (current != عدم &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    𐤀𐤌 (!Py_UNICODE_ISPRINTABLE(c)) {
        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    𐤀𐤌 (c == '=' && current != عدم) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    أرجع MAKE_TOKEN(_PyToken_OneChar(c));
}


فراغ
_PyTokenizer_Get(بنية tok_state *tok, بنية token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    صحيح result;
    𐤀𐤌 (current == عدم) {
        result = _PyLexer_get_normal(tok, عدم, token);
    }
    وإلا {
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
    𐤀𐤌 (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
