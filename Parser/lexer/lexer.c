#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"
#ଅନ୍ତର୍ଭାଵ "pycore_unicodeobject.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"

#ଅନ୍ତର୍ଭାଵ "lexer_internal.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/helpers.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* type_comment_prefix = "# type: ";

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_refill(ସଂରଚନା tok_state *tok)
{
    ଯଦି (tok->done != E_OK) {
        ଫେରନ୍ତୁ 0;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = _PyTok_ReaderUnderflow(tok);
#ଯଦି defined(Py_DEBUG)
    ଯଦି (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ଯଦି (!rc) {
        tok->cur = tok->inp;
        ଫେରନ୍ତୁ 0;
    }
    tok->ଧାଡ଼ି_start = tok->cur;
    _PyTok_Span ଧାଡ଼ି = {tok->ଧାଡ଼ି_start, tok->inp};
    ଯଦି (_PyTok_SourceFindByte(&tok->source, ଧାଡ଼ି, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 1;
}

/* Back-up one character */
ଶୂନ୍ୟ
_PyLexer_backup(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c != EOF) {
        ଯଦି (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ଯଦି (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
verify_identifier(ସଂରଚନା tok_state *tok)
{
    ଯଦି (tok->tok_extra_tokens) {
        ଫେରନ୍ତୁ 1;
    }
    PyObject *s;
    ଯଦି (tok_failed(tok))
        ଫେରନ୍ତୁ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, ଖାଲି);
    ଯଦି (s == ଖାଲି) {
        ଯଦି (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        ଅନ୍ୟଥା {
            tok->done = E_ERROR;
        }
        ଫେରନ୍ତୁ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    ପ୍ରତିଜ୍ଞା(invalid >= 0);
    ପ୍ରତିଜ୍ଞା(PyUnicode_GET_LENGTH(s) > 0);
    ଯଦି (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ଯଦି (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ଯଦି (s != ଖାଲି) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ଯଦି (s == ଖାଲି) {
                tok->done = E_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ଯଦି (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->ଧାଡ଼ି_start),
                error_cursor - tok->ଧାଡ଼ି_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ଅନ୍ୟଥା {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->ଧାଡ଼ି_start),
                error_cursor - tok->ଧାଡ଼ି_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ଫେରନ୍ତୁ 0;
    }
    Py_DECREF(s);
    ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_get_normal(ସଂରଚନା tok_state *tok, ftstring_state *current, ସଂରଚନା token *token)
{
    ପ୍ରତିଜ୍ଞା(current == ଖାଲି ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ଯଦି (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_ଧାଡ଼ି(tok);
        ଯଦି (blankline < 0) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->ଧାଡ଼ି_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ଯଦି (tok->layout.pending != 0) {
        ଫେରନ୍ତୁ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    କର {
        c = tok_nextc(tok);
    } ଯେତେବେଳେ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->ଧାଡ଼ି_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ଯଦି (c == '#') {

        ଯେତେବେଳେ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ଯଦି (current != ଖାଲି) {
            _PyTok_Off comment_end = tok->cur;
            ଯଦି (c == '\n' || c == '\r') {
                comment_end--;
            }
            ଯଦି (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ଯଦି (tok->type_comments) {
            ଅଚଳ ଅକ୍ଷର *prefix = type_comment_prefix;
            ଯେତେବେଳେ (*prefix && comment < tok->cur) {
                ଯଦି (*prefix == ' ') {
                    ଯେତେବେଳେ (comment < tok->cur) {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch = _PyTok_SourceByte(&tok->source, comment);
                        ଯଦି (ch != ' ' && ch != '\t') {
                            ଭାଙ୍ଗ;
                        }
                        comment++;
                    }
                } ଅନ୍ୟଥା ଯଦି (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } ଅନ୍ୟଥା {
                    ଭାଙ୍ଗ;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ଯଦି (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ଯଦି (is_type_ignore && comment + 6 < tok->cur) {
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_col_offset = tok->start_loc.byte_col +
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ଯଦି (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ଫେରନ୍ତୁ type;
            }
        }
        ଯଦି (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ଯଦି (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ଫେରନ୍ତୁ MAKE_TOKEN(COMMENT);
        }
    }

    ଯଦି (tok->done == E_INTERACT_STOP) {
        ଫେରନ୍ତୁ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ଯଦି (c == EOF) {
        ଯଦି (tok->level) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
        ଫେରନ୍ତୁ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ଯଦି (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ଯେତେବେଳେ (1) {
            ଯଦି (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ଅନ୍ୟଥା ଯଦି (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ଅନ୍ୟଥା ଯଦି (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ଅନ୍ୟଥା ଯଦି (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ଅନ୍ୟଥା ଯଦି (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
            c = tok_nextc(tok);
            ଯଦି (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ଯଦି (status < 0) {
                    ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ଯଦି (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ଫେରନ୍ତୁ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ଫେରନ୍ତୁ _PyLexer_scan_string(tok, token, c);
            }
        }
        ଯେତେବେଳେ (is_potential_identifier_char(c)) {
            ଯଦି (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ଯଦି (nonascii && !verify_identifier(tok)) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ଫେରନ୍ତୁ MAKE_TOKEN(NAME);
    }

    ଯଦି (c == '\r') {
        c = tok_nextc(tok);
    }

    ଯଦି (c == '\n') {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = _PyLexer_Newline(tok, token, blankline);
        ଯଦି (type == 0) {
            ଯାଅ nextline;
        }
        ଫେରନ୍ତୁ type;
    }

    /* Period or number starting with period? */
    ଯଦି (c == '.') {
        c = tok_nextc(tok);
        ଯଦି (Py_ISDIGIT(c)) {
            ଫେରନ୍ତୁ Py_Lexer_scan_ସଂଖ୍ୟା(tok, token, c, 1);
        } ଅନ୍ୟଥା ଯଦି (c == '.') {
            c = tok_nextc(tok);
            ଯଦି (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ଫେରନ୍ତୁ MAKE_TOKEN(ELLIPSIS);
            }
            ଅନ୍ୟଥା {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ଅନ୍ୟଥା {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ଫେରନ୍ତୁ MAKE_TOKEN(DOT);
    }


    /*ସଂଖ୍ୟା*/
    ଯଦି (Py_ISDIGIT(c)) {
        ଫେରନ୍ତୁ Py_Lexer_scan_ସଂଖ୍ୟା(tok, token, c, 0);
    }

    /* String */
    ଯଦି (c == '\'' || c == '"') {
        ଫେରନ୍ତୁ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ଯଦି (c == '\\') {
        ଯଦି ((c = Py_Lexer_Continue_ଧାଡ଼ି(tok)) == -1) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
        ଯାଅ again; /* Read next line */
    }

    /* Punctuation character */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_punctuation = (c == ':' || c == '}' || c == '!');
    ଯଦି (is_punctuation && current != ଖାଲି) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ଯଦି (type < 0) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
        ଯଦି (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ଫେରନ୍ତୁ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c2 = tok_nextc(tok);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ଯଦି (current_token != OP) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା c3 = tok_nextc(tok);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ଯଦି (current_token3 != OP) {
                current_token = current_token3;
            }
            ଅନ୍ୟଥା {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ଫେରନ୍ତୁ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    ବାଛ (c) {
    ସ୍ଥିତି '(':
    ସ୍ଥିତି '[':
    ସ୍ଥିତି '{':
        ଯଦି (tok->level >= MAXLEVEL) {
            ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tok->start - tok->ଧାଡ଼ି_start);
        tok->level++;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ')':
    ସ୍ଥିତି ']':
    ସ୍ଥିତି '}':
        ଯଦି (current != ଖାଲି &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ଯଦି (c == '}') {
                ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ଯଦି (!tok->tok_extra_tokens && !tok->level) {
            ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ଯଦି (tok->level > 0) {
            tok->level--;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା opening = tok->parenstack[tok->level];
            ଯଦି (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ଯଦି (current != ଖାଲି && opening == '{') {
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ଯଦି (bracket_depth == current->replacement_depth - 1) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ଯଦି (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ଅନ୍ୟଥା {
                    ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ଯଦି (current != ଖାଲି &&
                Py_Lexer_close_ftstring_ପଦସମଷ୍ଟି(tok, current, c) < 0) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    ଯଦି (!Py_UNICODE_ISPRINTABLE(c)) {
        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ଯଦି (c == '=' && current != ଖାଲି) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ଫେରନ୍ତୁ MAKE_TOKEN(_PyToken_OneChar(c));
}


ଶୂନ୍ୟ
_PyTokenizer_Get(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;
    ଯଦି (current == ଖାଲି) {
        result = _PyLexer_get_normal(tok, ଖାଲି, token);
    }
    ଅନ୍ୟଥା {
        ବାଛ (current->mode) {
        ସ୍ଥିତି FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି FTSTRING_MODE_ମଧ୍ୟ:
        ସ୍ଥିତି FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            Py_UNREACHABLE();
        }
    }
    ଯଦି (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
