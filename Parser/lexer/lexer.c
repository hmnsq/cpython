#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_token.h"
#અન્તર્ભાવ "pycore_unicodeobject.h"
#અન્તર્ભાવ "errcode.h"

#અન્તર્ભાવ "lexer_internal.h"
#અન્તર્ભાવ "../tokenizer/helpers.h"
#અન્તર્ભાવ "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
સ્થિર અચલ અક્ષર* type_comment_prefix = "# type: ";

પૂર્ણાંક
_PyLexer_refill(સંરચના tok_state *tok)
{
    જો (tok->done != E_OK) {
        પાછા 0;
    }
    પૂર્ણાંક rc = _PyTok_ReaderUnderflow(tok);
#જો defined(Py_DEBUG)
    જો (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    જો (!rc) {
        tok->cur = tok->inp;
        પાછા 0;
    }
    tok->લીટી_start = tok->cur;
    _PyTok_Span લીટી = {tok->લીટી_start, tok->inp};
    જો (_PyTok_SourceFindByte(&tok->source, લીટી, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        પાછા 0;
    }
    પાછા 1;
}

/* Back-up one character */
શૂન્ય
_PyLexer_backup(સંરચના tok_state *tok, પૂર્ણાંક c)
{
    જો (c != EOF) {
        જો (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        જો (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
સ્થિર પૂર્ણાંક
verify_identifier(સંરચના tok_state *tok)
{
    જો (tok->tok_extra_tokens) {
        પાછા 1;
    }
    PyObject *s;
    જો (tok_failed(tok))
        પાછા 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, ખાલી);
    જો (s == ખાલી) {
        જો (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        નહીંતર {
            tok->done = E_ERROR;
        }
        પાછા 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    પ્રતિજ્ઞા(invalid >= 0);
    પ્રતિજ્ઞા(PyUnicode_GET_LENGTH(s) > 0);
    જો (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        જો (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            જો (s != ખાલી) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            જો (s == ખાલી) {
                tok->done = E_ERROR;
                પાછા 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        જો (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->લીટી_start),
                error_cursor - tok->લીટી_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        નહીંતર {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->લીટી_start),
                error_cursor - tok->લીટી_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        પાછા 0;
    }
    Py_DECREF(s);
    પાછા 1;
}

પૂર્ણાંક
_PyLexer_get_normal(સંરચના tok_state *tok, ftstring_state *current, સંરચના token *token)
{
    પ્રતિજ્ઞા(current == ખાલી ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    પૂર્ણાંક c;
    પૂર્ણાંક blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    જો (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_લીટી(tok);
        જો (blankline < 0) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->લીટી_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    જો (tok->layout.pending != 0) {
        પાછા _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    કરો {
        c = tok_nextc(tok);
    } જ્યારે (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->લીટી_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    જો (c == '#') {

        જ્યારે (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        જો (current != ખાલી) {
            _PyTok_Off comment_end = tok->cur;
            જો (c == '\n' || c == '\r') {
                comment_end--;
            }
            જો (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        જો (tok->type_comments) {
            અચલ અક્ષર *prefix = type_comment_prefix;
            જ્યારે (*prefix && comment < tok->cur) {
                જો (*prefix == ' ') {
                    જ્યારે (comment < tok->cur) {
                        પૂર્ણાંક ch = _PyTok_SourceByte(&tok->source, comment);
                        જો (ch != ' ' && ch != '\t') {
                            તોડો;
                        }
                        comment++;
                    }
                } નહીંતર જો (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } નહીંતર {
                    તોડો;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            જો (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                પૂર્ણાંક is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                જો (is_type_ignore && comment + 6 < tok->cur) {
                    પૂર્ણાંક ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                પૂર્ણાંક type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                પૂર્ણાંક start_col_offset = tok->start_loc.byte_col +
                    (પૂર્ણાંક)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                જો (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                પાછા type;
            }
        }
        જો (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        જો (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            પાછા MAKE_TOKEN(COMMENT);
        }
    }

    જો (tok->done == E_INTERACT_STOP) {
        પાછા MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    જો (c == EOF) {
        જો (tok->level) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
        પાછા MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    જો (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        પૂર્ણાંક saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        જ્યારે (1) {
            જો (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            નહીંતર જો (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            નહીંતર જો (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            નહીંતર જો (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            નહીંતર જો (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            નહીંતર {
                તોડો;
            }
            c = tok_nextc(tok);
            જો (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                પૂર્ણાંક status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                જો (status < 0) {
                    પાછા MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                જો (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    પાછા _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                પાછા _PyLexer_scan_string(tok, token, c);
            }
        }
        જ્યારે (is_potential_identifier_char(c)) {
            જો (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        જો (nonascii && !verify_identifier(tok)) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        પાછા MAKE_TOKEN(NAME);
    }

    જો (c == '\r') {
        c = tok_nextc(tok);
    }

    જો (c == '\n') {
        પૂર્ણાંક type = _PyLexer_Newline(tok, token, blankline);
        જો (type == 0) {
            જાઓ nextline;
        }
        પાછા type;
    }

    /* Period or number starting with period? */
    જો (c == '.') {
        c = tok_nextc(tok);
        જો (Py_ISDIGIT(c)) {
            પાછા Py_Lexer_scan_સંખ્યા(tok, token, c, 1);
        } નહીંતર જો (c == '.') {
            c = tok_nextc(tok);
            જો (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                પાછા MAKE_TOKEN(ELLIPSIS);
            }
            નહીંતર {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        નહીંતર {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        પાછા MAKE_TOKEN(DOT);
    }


    /*સંખ્યા*/
    જો (Py_ISDIGIT(c)) {
        પાછા Py_Lexer_scan_સંખ્યા(tok, token, c, 0);
    }

    /* String */
    જો (c == '\'' || c == '"') {
        પાછા _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    જો (c == '\\') {
        જો ((c = Py_Lexer_Continue_લીટી(tok)) == -1) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
        જાઓ again; /* Read next line */
    }

    /* Punctuation character */
    પૂર્ણાંક is_punctuation = (c == ':' || c == '}' || c == '!');
    જો (is_punctuation && current != ખાલી) {
        પૂર્ણાંક type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        જો (type < 0) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
        જો (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            પાછા MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        પૂર્ણાંક c2 = tok_nextc(tok);
        પૂર્ણાંક current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        જો (current_token != OP) {
            પૂર્ણાંક c3 = tok_nextc(tok);
            પૂર્ણાંક current_token3 = _PyToken_ThreeChars(c, c2, c3);
            જો (current_token3 != OP) {
                current_token = current_token3;
            }
            નહીંતર {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            પાછા MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    પસંદગી (c) {
    સ્થિતિ '(':
    સ્થિતિ '[':
    સ્થિતિ '{':
        જો (tok->level >= MAXLEVEL) {
            પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (પૂર્ણાંક)(tok->start - tok->લીટી_start);
        tok->level++;
        તોડો;
    સ્થિતિ ')':
    સ્થિતિ ']':
    સ્થિતિ '}':
        જો (current != ખાલી &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            જો (c == '}') {
                પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        જો (!tok->tok_extra_tokens && !tok->level) {
            પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        જો (tok->level > 0) {
            tok->level--;
            પૂર્ણાંક opening = tok->parenstack[tok->level];
            જો (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                જો (current != ખાલી && opening == '{') {
                    પૂર્ણાંક bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    જો (bracket_depth == current->replacement_depth - 1) {
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                જો (tok->parenlinenostack[tok->level] != tok->lineno) {
                    પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                નહીંતર {
                    પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        જો (current != ખાલી &&
                Py_Lexer_close_ftstring_પદાવલિ(tok, current, c) < 0) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
        તોડો;
    મૂળભૂત:
        તોડો;
    }

    જો (!Py_UNICODE_ISPRINTABLE(c)) {
        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    જો (c == '=' && current != ખાલી) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    પાછા MAKE_TOKEN(_PyToken_OneChar(c));
}


શૂન્ય
_PyTokenizer_Get(સંરચના tok_state *tok, સંરચના token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    પૂર્ણાંક result;
    જો (current == ખાલી) {
        result = _PyLexer_get_normal(tok, ખાલી, token);
    }
    નહીંતર {
        પસંદગી (current->mode) {
        સ્થિતિ FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            તોડો;
        સ્થિતિ FTSTRING_MODE_મધ્ય:
        સ્થિતિ FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            તોડો;
        મૂળભૂત:
            Py_UNREACHABLE();
        }
    }
    જો (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
