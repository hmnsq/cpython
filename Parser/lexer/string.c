#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_token.h"
#అన్తర్భావ "errcode.h"

#అన్తర్భావ "lexer_internal.h"
#అన్తర్భావ "../tokenizer/helpers.h"

#నిర్వచించు MAKE_TOKEN(token_type) Py_Lexer_token_అమరిక(tok, token, token_type, p_ప్రారంభం, p_చివర)

స్థిర పూర్ణాంకం
string_error_token(నిర్మాణం tok_state *tok, నిర్మాణం token *token,
                   _PyTok_Off ప్రారంభం, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_నిలువు + 1},
        .పాఠ్యం_పరిధి = {ప్రారంభం - location.byte_నిలువు, tok->inp},
    };
    పూర్ణాంకం type = Py_Lexer_token_అమరిక(tok, token, ERRORTOKEN, -1, -1);
    token->ప్రారంభం_loc = location;
    token->చివర_loc = (_PyTok_Loc){location.lineno, -1};
    తిరిగి type;
}

స్థిర పూర్ణాంకం
incomplete_string_token(నిర్మాణం tok_state *tok, నిర్మాణం token *token,
                        పూర్ణాంకం type, _PyTok_Off ప్రారంభం, _PyTok_Loc location)
{
    string_error_token(tok, token, ప్రారంభం, location);
    tok->పూర్తి = E_INCOMPLETE;
    తిరిగి type;
}

పూర్ణాంకం
Py_Lexer_రికార్డు_ftstring_comment(నిర్మాణం tok_state *tok, ftstring_state *state,
                                 _PyTok_Off ప్రారంభం, _PyTok_Off చివర)
{
    ప్రతిజ్ఞా(state == _PyLexer_CurrentFTString(tok) && state->బహుళకం == FTSTRING_బహుళకం_వ్యక్తీకరణ);
    ఒకవేళ (state->సమాసం_పరిధి.చివర >= 0) {
        తిరిగి 0;
    }
    ప్రతిజ్ఞా(state->సమాసం_పరిధి.ప్రారంభం >= 0);
    tokenizer_comments *comments = state->comments;
    ఒకవేళ (comments == ఖాళీ || comments->లెక్క == comments->సామర్థ్యం) {
        Py_ssize_t max_సామర్థ్యం = (PY_SSIZE_T_MAX -
            (Py_ssize_t)పరిమాణం(*comments)) /
            (Py_ssize_t)పరిమాణం(*comments->spans);
        ఒకవేళ (comments != ఖాళీ && comments->సామర్థ్యం > max_సామర్థ్యం / 2) {
            Py_Err_No_జ్ఞాపకం();
            తిరిగి -1;
        }
        Py_ssize_t సామర్థ్యం = comments == ఖాళీ ? 4 : comments->సామర్థ్యం * 2;
        కొలత_t కొలత = పరిమాణం(*comments) +
            (కొలత_t)సామర్థ్యం * పరిమాణం(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, కొలత);
        ఒకవేళ (resized == ఖాళీ) {
            Py_Err_No_జ్ఞాపకం();
            తిరిగి -1;
        }
        comments = resized;
        ఒకవేళ (state->comments == ఖాళీ) {
            comments->లెక్క = 0;
        }
        comments->సామర్థ్యం = సామర్థ్యం;
        state->comments = comments;
    }
    comments->spans[comments->లెక్క++] =
        (Py_Tok_పరిధి){ప్రారంభం, చివర};
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
ముగింపు_ftstring_సమాసం(నిర్మాణం tok_state *tok, ftstring_state *state,
                     నిర్మాణం token *token)
{
    ప్రతిజ్ఞా(token != ఖాళీ && state == _PyLexer_CurrentFTString(tok));
    ప్రతిజ్ఞా(state->బహుళకం == FTSTRING_బహుళకం_వ్యక్తీకరణ && tok->ప్రారంభం >= 0);

    ఒకవేళ (state->సమాసం_పరిధి.చివర >= 0) {
        తిరిగి 0;
    }
    ప్రతిజ్ఞా(state->సమాసం_పరిధి.ప్రారంభం >= 0);
    state->సమాసం_పరిధి.చివర = tok->ప్రారంభం;
    పూర్ణాంకం tstring_interpolation = _PyLexer_IsTString(state->రకం) &&
        state->replacement_లోతు == 1;
    ఒకవేళ (!(state->debug_సమాసం || tstring_interpolation) || token->metadata) {
        తిరిగి 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ఒకవేళ (comments != ఖాళీ && comments->లెక్క > 0) {
        Py_ssize_t stripped_కొలత =
            state->సమాసం_పరిధి.చివర - state->సమాసం_పరిధి.ప్రారంభం;
        Py_ssize_t comment_లెక్క = 0;
        కొరకు (Py_ssize_t i = 0; i < comments->లెక్క; i++) {
            Py_Tok_పరిధి comment = comments->spans[i];
            ప్రతిజ్ఞా(Py_Tok_పరిధి_Is_Valid(comment));
            ప్రతిజ్ఞా(comment.ప్రారంభం >= (i == 0 ? state->సమాసం_పరిధి.ప్రారంభం
                                           : comments->spans[i - 1].చివర));
            ఒకవేళ (comment.ప్రారంభం >= state->సమాసం_పరిధి.చివర) {
                ఆపు;
            }
            ప్రతిజ్ఞా(comment.చివర <= state->సమాసం_పరిధి.చివర);
            stripped_కొలత -= comment.చివర - comment.ప్రారంభం;
            comment_లెక్క++;
        }
        అక్షరం *stripped = PyMem_Malloc((కొలత_t)stripped_కొలత);
        ఒకవేళ (stripped == ఖాళీ) {
            Py_Err_No_జ్ఞాపకం();
            తిరిగి -1;
        }
        Py_Tok_పరిధి kept = {state->సమాసం_పరిధి.ప్రారంభం, state->సమాసం_పరిధి.ప్రారంభం};
        Py_ssize_t stripped_len = 0;
        కొరకు (Py_ssize_t i = 0; i < comment_లెక్క; i++) {
            kept.చివర = comments->spans[i].ప్రారంభం;
            Py_ssize_t నిడివి;
            స్థిరరాశి అక్షరం *పాఠ్యం = Py_Tok_ఆధారం_పరిధి_View(
                &tok->ఆధారం, kept, &నిడివి);
            జ్ఞాపకం_నకలు(stripped + stripped_len, పాఠ్యం, (కొలత_t)నిడివి);
            stripped_len += నిడివి;
            kept.ప్రారంభం = comments->spans[i].చివర;
        }
        kept.చివర = state->సమాసం_పరిధి.చివర;
        Py_ssize_t నిడివి;
        స్థిరరాశి అక్షరం *పాఠ్యం = Py_Tok_ఆధారం_పరిధి_View(&tok->ఆధారం, kept, &నిడివి);
        జ్ఞాపకం_నకలు(stripped + stripped_len, పాఠ్యం, (కొలత_t)నిడివి);
        res = PyUnicode_DecodeUTF8(stripped, stripped_కొలత, ఖాళీ);
        PyMem_Free(stripped);
    }
    లేకపోతే {
        Py_ssize_t సమాసం_len;
        స్థిరరాశి అక్షరం *సమాసం = Py_Tok_ఆధారం_పరిధి_View(
            &tok->ఆధారం, state->సమాసం_పరిధి, &సమాసం_len);
        res = PyUnicode_DecodeUTF8(సమాసం, సమాసం_len, ఖాళీ);
    }

    ఒకవేళ (!res) {
        తిరిగి -1;
    }
    token->metadata = res;
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
begin_ftstring_సమాసం(నిర్మాణం tok_state *tok, ftstring_state *state,
                    _PyTok_Off సమాసం_ప్రారంభం)
{
    ప్రతిజ్ఞా(state->బహుళకం != FTSTRING_బహుళకం_వ్యక్తీకరణ);
    state->సమాసం_పరిధి = (Py_Tok_పరిధి){సమాసం_ప్రారంభం, -1};
    ఒకవేళ (state->comments != ఖాళీ) {
        state->comments->లెక్క = 0;
    }
    ఒకవేళ (state->replacement_లోతు >= MAX_సమాసం_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_ఉపసర్గ(state->రకం));
        తిరిగి -1;
    }
    state->replacement_లోతు++;
    state->బహుళకం = FTSTRING_బహుళకం_వ్యక్తీకరణ;
    state->debug_సమాసం = 0;
    తిరిగి 0;
}

పూర్ణాంకం
_PyLexer_ftstring_punctuation(నిర్మాణం tok_state *tok, ftstring_state *state,
                              నిర్మాణం token *token, పూర్ణాంకం c)
{
    ప్రతిజ్ఞా(state->బహుళకం == FTSTRING_బహుళకం_వ్యక్తీకరణ);
    ప్రతిజ్ఞా(c == ':' || c == '}' || c == '!');
    ఒకవేళ (Py_Lexer_FTString_Bracket_లోతు(tok, state) != state->replacement_లోతు) {
        తిరిగి 0;
    }
    ఒకవేళ (c == '!') {
        పూర్ణాంకం తదుపరి = tok_nextc(tok);
        tok_backup(tok, తదుపరి);
        ఒకవేళ (తదుపరి == '=') {
            తిరిగి 0;
        }
    }
    ఒకవేళ (ముగింపు_ftstring_సమాసం(tok, state, token) < 0) {
        తిరిగి -1;
    }
    ఒకవేళ (c == ':') {
        state->బహుళకం = FTSTRING_బహుళకం_FORMAT_SPEC;
        తిరిగి COLON;
    }
    తిరిగి 0;
}

పూర్ణాంకం
Py_Lexer_close_ftstring_సమాసం(నిర్మాణం tok_state *tok, ftstring_state *state,
                             పూర్ణాంకం c)
{
    ప్రతిజ్ఞా(state->బహుళకం == FTSTRING_బహుళకం_వ్యక్తీకరణ);
    ప్రతిజ్ఞా(c == ')' || c == ']' || c == '}');
    పూర్ణాంకం లోతు = Py_Lexer_FTString_Bracket_లోతు(tok, state);
    ఒకవేళ (లోతు < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_ఉపసర్గ(state->రకం), c);
        తిరిగి -1;
    }
    ఒకవేళ (c == '}' && లోతు == state->replacement_లోతు - 1) {
        state->replacement_లోతు--;
        state->బహుళకం = FTSTRING_బహుళకం_మధ్య;
        state->debug_సమాసం = 0;
    }
    తిరిగి 0;
}

శూన్య
_PyLexer_mark_ftstring_debug(నిర్మాణం tok_state *tok, ftstring_state *state)
{
    ప్రతిజ్ఞా(state->బహుళకం == FTSTRING_బహుళకం_వ్యక్తీకరణ);
    ఒకవేళ (Py_Lexer_FTString_Bracket_లోతు(tok, state) == state->replacement_లోతు) {
        state->debug_సమాసం = 1;
    }
}

పూర్ణాంకం
_PyLexer_check_string_prefixes(నిర్మాణం tok_state *tok,
                                             పూర్ణాంకం saw_b, పూర్ణాంకం saw_r, పూర్ణాంకం saw_u,
                                             పూర్ణాంకం saw_f, పూర్ణాంకం saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#నిర్వచించు RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    చేయి {                                                                  \
        (శూన్య)Py_Tokenizer_syntaxerror_తెలిసిన_range(                       \
            tok, (పూర్ణాంకం)(tok->ప్రారంభం + 1 - tok->పంక్తి_ప్రారంభం),                 \
            (పూర్ణాంకం)(tok->cur - tok->పంక్తి_ప్రారంభం),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        తిరిగి -1;                                                        \
    } అయితే (0)

    ఒకవేళ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ఒకవేళ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ఒకవేళ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ఒకవేళ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ఒకవేళ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ఒకవేళ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ఒకవేళ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#నిర్వచనం_రద్దు RETURN_SYNTAX_ERROR

    తిరిగి 0;
}

పూర్ణాంకం
Py_Lexer_పరిశీలన_fstring_ప్రారంభం(నిర్మాణం tok_state *tok, నిర్మాణం token *token,
                            పూర్ణాంకం c, ftstring_రకం రకం)
{
    _PyTok_Off p_ప్రారంభం = -1;
    _PyTok_Off p_చివర = -1;

    పూర్ణాంకం quote = c;
    పూర్ణాంకం quote_కొలత = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    పూర్ణాంకం after_quote = tok_nextc(tok);
    ఒకవేళ (after_quote == quote) {
        పూర్ణాంకం after_after_quote = tok_nextc(tok);
        ఒకవేళ (after_after_quote == quote) {
            quote_కొలత = 3;
        }
        లేకపోతే {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ఒకవేళ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_ప్రారంభం = tok->ప్రారంభం;
    p_చివర = tok->cur;
    ftstring_state *state = Py_Lexer_నెట్టు_FTString(tok);
    ఒకవేళ (state == ఖాళీ) {
        తిరిగి MAKE_TOKEN(ERRORTOKEN);
    }
    state->బహుళకం = FTSTRING_బహుళకం_మధ్య;
    state->quote = quote;
    state->quote_కొలత = quote_కొలత;
    state->paren_level = tok->level;
    state->ప్రారంభం = tok->ప్రారంభం;
    state->ప్రారంభం_loc = tok->ప్రారంభం_loc;
    state->సమాసం_పరిధి = (Py_Tok_పరిధి){-1, -1};

    state->రకం = రకం;
    తిరిగి _PyLexer_IsTString(రకం)
        ? MAKE_TOKEN(TSTRING_ప్రారంభం) : MAKE_TOKEN(FSTRING_ప్రారంభం);
}

పూర్ణాంకం
Py_Lexer_పరిశీలన_string(నిర్మాణం tok_state *tok, నిర్మాణం token *token, పూర్ణాంకం c)
{
    _PyTok_Off p_ప్రారంభం = -1;
    _PyTok_Off p_చివర = -1;

    పూర్ణాంకం quote = c;
    పూర్ణాంకం quote_కొలత = 1;             /* 1 or 3 */
    పూర్ణాంకం చివర_quote_కొలత = 0;
    పూర్ణాంకం has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ఒకవేళ (c == quote) {
        c = tok_nextc(tok);
        ఒకవేళ (c == quote) {
            quote_కొలత = 3;
        }
        లేకపోతే {
            చివర_quote_కొలత = 1;     /* empty string found */
        }
    }
    ఒకవేళ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    అయితే (చివర_quote_కొలత != quote_కొలత) {
        c = tok_nextc(tok);
        ఒకవేళ (tok->పూర్తి == E_ERROR) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
        ఒకవేళ (tok->పూర్తి == E_DECODE) {
            ఆపు;
        }
        ఒకవేళ (c == EOF || (quote_కొలత == 1 && c == '\n')) {
            ఒకవేళ (tok_failed(tok)) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
            ఒకవేళ (c == EOF && tok->allow_incomplete_ఆదానం) {
                తిరిగి incomplete_string_token(tok, token, STRING,
                                               tok->ప్రారంభం, tok->ప్రారంభం_loc);
            }
            పూర్ణాంకం చివర_lineno = tok->lineno;
            _PyTok_Loc location = tok->ప్రారంభం_loc;
            స్థిరరాశి అక్షరం *పంక్తి = Py_Tok_ఆధారం_సూచిక(
                &tok->ఆధారం, tok->ప్రారంభం - location.byte_నిలువు);
            Py_ssize_t cursor_స్థానభ్రంశం = (Py_ssize_t)location.byte_నిలువు + 1;

            స్థిరరాశి ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ఒకవేళ (state != ఖాళీ) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ఒకవేళ (state->quote == quote && state->quote_కొలత == quote_కొలత) {
                    పూర్ణాంకం level = state->paren_level + state->replacement_లోతు - 1;
                    ప్రతిజ్ఞా(level >= 0 && level < tok->level);
                    ప్రతిజ్ఞా(tok->parenstack[level] == '{');
                    పూర్ణాంకం lineno = tok->parenlinenostack[level];
                    ఒకవేళ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_ఉపసర్గ(state->రకం), lineno);
                    }
                    లేకపోతే {
                        _PyTokenizer_syntaxerror_at(
                            tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_ఉపసర్గ(state->రకం));
                    }
                    తిరిగి string_error_token(tok, token, tok->ప్రారంభం, location);
                }
            }

            ఒకవేళ (quote_కొలత == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", చివర_lineno);
                ఒకవేళ (c != '\n') {
                    tok->పూర్తి = E_EOFS;
                }
                తిరిగి string_error_token(tok, token, tok->ప్రారంభం, location);
            }
            లేకపోతే {
                ఒకవేళ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        చివర_lineno
                    );
                } లేకపోతే {
                    _PyTokenizer_syntaxerror_at(
                        tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", చివర_lineno
                    );
                }
                ఒకవేళ (c != '\n') {
                    tok->పూర్తి = E_EOLS;
                }
                తిరిగి string_error_token(tok, token, tok->ప్రారంభం, location);
            }
        }
        ఒకవేళ (c == quote) {
            చివర_quote_కొలత += 1;
        }
        లేకపోతే {
            చివర_quote_కొలత = 0;
            ఒకవేళ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ఒకవేళ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ఒకవేళ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_ప్రారంభం = tok->ప్రారంభం;
    p_చివర = tok->cur;
    తిరిగి MAKE_TOKEN(STRING);
}

పూర్ణాంకం
_PyLexer_get_ftstring(నిర్మాణం tok_state *tok, ftstring_state *current, నిర్మాణం token *token)
{
    ప్రతిజ్ఞా(current == _PyLexer_CurrentFTString(tok) && current->బహుళకం != FTSTRING_బహుళకం_వ్యక్తీకరణ);
    ప్రతిజ్ఞా((current->quote_కొలత == 1 || current->quote_కొలత == 3) &&
           current->replacement_లోతు <= MAX_సమాసం_NESTING);
    _PyTok_Off p_ప్రారంభం = -1;
    _PyTok_Off p_చివర = -1;
    పూర్ణాంకం token_type;
    పూర్ణాంకం చివర_quote_కొలత = 0;
    పూర్ణాంకం unicode_escape = 0;
    పూర్ణాంకం quote = current->quote;
    పూర్ణాంకం quote_కొలత = current->quote_కొలత;
    పూర్ణాంకం లోపల_format_spec = current->బహుళకం == FTSTRING_బహుళకం_FORMAT_SPEC;
    పూర్ణాంకం raw = _PyLexer_IsRawString(current->రకం);

    tok->ప్రారంభం = tok->cur;
    tok->ప్రారంభం_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_స్తంభం(tok)};

    అయితే (చివర_quote_కొలత != quote_కొలత) {
        పూర్ణాంకం c = tok_nextc(tok);
        ఒకవేళ (tok->పూర్తి == E_ERROR || tok->పూర్తి == E_DECODE) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }

        ఒకవేళ (c == EOF || (quote_కొలత == 1 && c == '\n')) {
            ఒకవేళ (tok_failed(tok)) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
            ఒకవేళ (c == EOF && tok->allow_incomplete_ఆదానం) {
                తిరిగి incomplete_string_token(tok, token,
                    FTSTRING_మధ్య(current), current->ప్రారంభం, current->ప్రారంభం_loc);
            }

            ఒకవేళ (లోపల_format_spec && c == '\n') {
                తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_ఉపసర్గ(current->రకం),
                    Py_Lexer_String_ఉపసర్గ(current->రకం)));
            }

            పూర్ణాంకం చివర_lineno = tok->lineno;
            _PyTok_Loc location = current->ప్రారంభం_loc;
            స్థిరరాశి అక్షరం *పంక్తి = Py_Tok_ఆధారం_సూచిక(
                &tok->ఆధారం, current->ప్రారంభం - location.byte_నిలువు);
            Py_ssize_t cursor_స్థానభ్రంశం = (Py_ssize_t)location.byte_నిలువు + 1;

            ఒకవేళ (quote_కొలత == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_ఉపసర్గ(current->రకం), చివర_lineno);
                ఒకవేళ (c != '\n') {
                    tok->పూర్తి = E_EOFS;
                }
                తిరిగి string_error_token(tok, token,
                    current->ప్రారంభం, location);
            }
            లేకపోతే {
                _PyTokenizer_syntaxerror_at(
                    tok, పంక్తి, cursor_స్థానభ్రంశం, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_ఉపసర్గ(current->రకం), చివర_lineno);
                తిరిగి string_error_token(tok, token,
                    current->ప్రారంభం, location);
            }
        }

        ఒకవేళ (c == quote) {
            చివర_quote_కొలత += 1;
            కొనసాగు;
        } లేకపోతే {
            చివర_quote_కొలత = 0;
        }

        ఒకవేళ (c == '{') {
            పూర్ణాంకం peek = tok_nextc(tok);
            ఒకవేళ (peek != '{' || లోపల_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off సమాసం_ప్రారంభం = tok->cur;
                tok_backup(tok, c);
                ఒకవేళ (begin_ftstring_సమాసం(tok, current, సమాసం_ప్రారంభం) < 0) {
                    తిరిగి MAKE_TOKEN(ERRORTOKEN);
                }
                p_ప్రారంభం = tok->ప్రారంభం;
                p_చివర = tok->cur;
                ఒకవేళ (p_ప్రారంభం == p_చివర) {
                    తిరిగి Py_Lexer_get_సాధారణ(tok, current, token);
                }
            } లేకపోతే {
                p_ప్రారంభం = tok->ప్రారంభం;
                p_చివర = tok->cur - 1;
            }
            వెళ్ళు emit_మధ్య;
        } లేకపోతే ఒకవేళ (c == '}') {
            ఒకవేళ (unicode_escape) {
                p_ప్రారంభం = tok->ప్రారంభం;
                p_చివర = tok->cur;
                వెళ్ళు emit_మధ్య;
            }
            పూర్ణాంకం peek = tok_nextc(tok);

            పూర్ణాంకం bracket_లోతు = Py_Lexer_FTString_Bracket_లోతు(tok, current);
            ఒకవేళ (peek == '}' && !లోపల_format_spec && bracket_లోతు == 0) {
                p_ప్రారంభం = tok->ప్రారంభం;
                p_చివర = tok->cur - 1;
            }
            లేకపోతే {
                tok_backup(tok, peek);
                ఒకవేళ (!లోపల_format_spec && bracket_లోతు == 0) {
                    ఒకవేళ (tok->ప్రారంభం == tok->cur - 1) {
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_ఉపసర్గ(current->రకం)));
                    }
                    tok_backup(tok, c);
                    p_ప్రారంభం = tok->ప్రారంభం;
                    p_చివర = tok->cur;
                    వెళ్ళు emit_మధ్య;
                }
                tok_backup(tok, c);
                current->బహుళకం = FTSTRING_బహుళకం_వ్యక్తీకరణ;
                p_ప్రారంభం = tok->ప్రారంభం;
                p_చివర = tok->cur;
            }
            వెళ్ళు emit_మధ్య;
        } లేకపోతే ఒకవేళ (c == '\\') {
            పూర్ణాంకం peek = tok_nextc(tok);
            ఒకవేళ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ఒకవేళ (peek == '{' || peek == '}') {
                ఒకవేళ (!raw) {
                    ఒకవేళ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        తిరిగి MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                కొనసాగు;
            }

            ఒకవేళ (!raw) {
                ఒకవేళ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ఒకవేళ (peek == '{') {
                        unicode_escape = 1;
                    } లేకపోతే {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_ప్రారంభం = tok->ప్రారంభం;
    p_చివర = tok->cur;
    ఒకవేళ (p_చివర - quote_కొలత == p_ప్రారంభం) {
        పూర్ణాంకం చివర_token = FTSTRING_చివర(current);
        Py_Lexer_తీయి_FTString(tok);
        తిరిగి MAKE_TOKEN(చివర_token);
    }
    కొరకు (పూర్ణాంకం i = 0; i < quote_కొలత; i++) {
        tok_backup(tok, quote);
    }
    p_చివర = tok->cur;
emit_మధ్య:
    token_type = MAKE_TOKEN(FTSTRING_మధ్య(current));
    token->is_raw = raw;
    తిరిగి token_type;
}
