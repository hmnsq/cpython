#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_token.h"
#அந்தர்பாவ "pycore_unicodeobject.h"
#அந்தர்பாவ "errcode.h"

#அந்தர்பாவ "lexer_internal.h"
#அந்தர்பாவ "../tokenizer/helpers.h"
#அந்தர்பாவ "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
நிலையான மாறிலி எழுத்து* type_comment_prefix = "# type: ";

முழு
_PyLexer_refill(கட்டமைப்பு tok_state *tok)
{
    எனில் (tok->done != E_OK) {
        திரும்பு 0;
    }
    முழு rc = _PyTok_ReaderUnderflow(tok);
#எனில் defined(Py_DEBUG)
    எனில் (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    எனில் (!rc) {
        tok->cur = tok->inp;
        திரும்பு 0;
    }
    tok->வரி_start = tok->cur;
    _PyTok_Span வரி = {tok->வரி_start, tok->inp};
    எனில் (_PyTok_SourceFindByte(&tok->source, வரி, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        திரும்பு 0;
    }
    திரும்பு 1;
}

/* Back-up one character */
வெறுமை
_PyLexer_backup(கட்டமைப்பு tok_state *tok, முழு c)
{
    எனில் (c != EOF) {
        எனில் (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        எனில் (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
நிலையான முழு
verify_identifier(கட்டமைப்பு tok_state *tok)
{
    எனில் (tok->tok_extra_tokens) {
        திரும்பு 1;
    }
    PyObject *s;
    எனில் (tok_failed(tok))
        திரும்பு 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, வெற்று);
    எனில் (s == வெற்று) {
        எனில் (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        இல்லையெனில் {
            tok->done = E_ERROR;
        }
        திரும்பு 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    ப்ரதிஜ்ஞா(invalid >= 0);
    ப்ரதிஜ்ஞா(PyUnicode_GET_LENGTH(s) > 0);
    எனில் (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        எனில் (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            எனில் (s != வெற்று) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            எனில் (s == வெற்று) {
                tok->done = E_ERROR;
                திரும்பு 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        எனில் (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->வரி_start),
                error_cursor - tok->வரி_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        இல்லையெனில் {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->வரி_start),
                error_cursor - tok->வரி_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        திரும்பு 0;
    }
    Py_DECREF(s);
    திரும்பு 1;
}

முழு
_PyLexer_get_normal(கட்டமைப்பு tok_state *tok, ftstring_state *current, கட்டமைப்பு token *token)
{
    ப்ரதிஜ்ஞா(current == வெற்று ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    முழு c;
    முழு blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    எனில் (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_வரி(tok);
        எனில் (blankline < 0) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->வரி_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    எனில் (tok->layout.pending != 0) {
        திரும்பு _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    செய் {
        c = tok_nextc(tok);
    } போது (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->வரி_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    எனில் (c == '#') {

        போது (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        எனில் (current != வெற்று) {
            _PyTok_Off comment_end = tok->cur;
            எனில் (c == '\n' || c == '\r') {
                comment_end--;
            }
            எனில் (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        எனில் (tok->type_comments) {
            மாறிலி எழுத்து *prefix = type_comment_prefix;
            போது (*prefix && comment < tok->cur) {
                எனில் (*prefix == ' ') {
                    போது (comment < tok->cur) {
                        முழு ch = _PyTok_SourceByte(&tok->source, comment);
                        எனில் (ch != ' ' && ch != '\t') {
                            நிறுத்து;
                        }
                        comment++;
                    }
                } இல்லையெனில் எனில் (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } இல்லையெனில் {
                    நிறுத்து;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            எனில் (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                முழு is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                எனில் (is_type_ignore && comment + 6 < tok->cur) {
                    முழு ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                முழு type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                முழு start_col_offset = tok->start_loc.byte_col +
                    (முழு)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                எனில் (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                திரும்பு type;
            }
        }
        எனில் (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        எனில் (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            திரும்பு MAKE_TOKEN(COMMENT);
        }
    }

    எனில் (tok->done == E_INTERACT_STOP) {
        திரும்பு MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    எனில் (c == EOF) {
        எனில் (tok->level) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
        திரும்பு MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    எனில் (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        முழு saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        போது (1) {
            எனில் (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            இல்லையெனில் எனில் (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            இல்லையெனில் எனில் (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            இல்லையெனில் எனில் (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            இல்லையெனில் எனில் (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            இல்லையெனில் {
                நிறுத்து;
            }
            c = tok_nextc(tok);
            எனில் (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                முழு status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                எனில் (status < 0) {
                    திரும்பு MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                எனில் (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    திரும்பு _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                திரும்பு _PyLexer_scan_string(tok, token, c);
            }
        }
        போது (is_potential_identifier_char(c)) {
            எனில் (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        எனில் (nonascii && !verify_identifier(tok)) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        திரும்பு MAKE_TOKEN(NAME);
    }

    எனில் (c == '\r') {
        c = tok_nextc(tok);
    }

    எனில் (c == '\n') {
        முழு type = _PyLexer_Newline(tok, token, blankline);
        எனில் (type == 0) {
            செல் nextline;
        }
        திரும்பு type;
    }

    /* Period or number starting with period? */
    எனில் (c == '.') {
        c = tok_nextc(tok);
        எனில் (Py_ISDIGIT(c)) {
            திரும்பு Py_Lexer_scan_எண்(tok, token, c, 1);
        } இல்லையெனில் எனில் (c == '.') {
            c = tok_nextc(tok);
            எனில் (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                திரும்பு MAKE_TOKEN(ELLIPSIS);
            }
            இல்லையெனில் {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        இல்லையெனில் {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        திரும்பு MAKE_TOKEN(DOT);
    }


    /*எண்*/
    எனில் (Py_ISDIGIT(c)) {
        திரும்பு Py_Lexer_scan_எண்(tok, token, c, 0);
    }

    /* String */
    எனில் (c == '\'' || c == '"') {
        திரும்பு _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    எனில் (c == '\\') {
        எனில் ((c = Py_Lexer_Continue_வரி(tok)) == -1) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
        செல் again; /* Read next line */
    }

    /* Punctuation character */
    முழு is_punctuation = (c == ':' || c == '}' || c == '!');
    எனில் (is_punctuation && current != வெற்று) {
        முழு type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        எனில் (type < 0) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
        எனில் (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            திரும்பு MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        முழு c2 = tok_nextc(tok);
        முழு current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        எனில் (current_token != OP) {
            முழு c3 = tok_nextc(tok);
            முழு current_token3 = _PyToken_ThreeChars(c, c2, c3);
            எனில் (current_token3 != OP) {
                current_token = current_token3;
            }
            இல்லையெனில் {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            திரும்பு MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    தேர்வு (c) {
    நிலை '(':
    நிலை '[':
    நிலை '{':
        எனில் (tok->level >= MAXLEVEL) {
            திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (முழு)(tok->start - tok->வரி_start);
        tok->level++;
        நிறுத்து;
    நிலை ')':
    நிலை ']':
    நிலை '}':
        எனில் (current != வெற்று &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            எனில் (c == '}') {
                திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        எனில் (!tok->tok_extra_tokens && !tok->level) {
            திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        எனில் (tok->level > 0) {
            tok->level--;
            முழு opening = tok->parenstack[tok->level];
            எனில் (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                எனில் (current != வெற்று && opening == '{') {
                    முழு bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    எனில் (bracket_depth == current->replacement_depth - 1) {
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                எனில் (tok->parenlinenostack[tok->level] != tok->lineno) {
                    திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                இல்லையெனில் {
                    திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        எனில் (current != வெற்று &&
                Py_Lexer_close_ftstring_கோவை(tok, current, c) < 0) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
        நிறுத்து;
    இயல்பு:
        நிறுத்து;
    }

    எனில் (!Py_UNICODE_ISPRINTABLE(c)) {
        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    எனில் (c == '=' && current != வெற்று) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    திரும்பு MAKE_TOKEN(_PyToken_OneChar(c));
}


வெறுமை
_PyTokenizer_Get(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    முழு result;
    எனில் (current == வெற்று) {
        result = _PyLexer_get_normal(tok, வெற்று, token);
    }
    இல்லையெனில் {
        தேர்வு (current->mode) {
        நிலை FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            நிறுத்து;
        நிலை FTSTRING_MODE_நடு:
        நிலை FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            நிறுத்து;
        இயல்பு:
            Py_UNREACHABLE();
        }
    }
    எனில் (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
