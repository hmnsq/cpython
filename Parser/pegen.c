#అన్తర్భావ <Python.h>
#అన్తర్భావ "pycore_ast.h"           // _PyAST_Validate(),
#అన్తర్భావ "pycore_pystate.h"       // _PyThreadState_GET()
#అన్తర్భావ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#అన్తర్భావ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#అన్తర్భావ "pycore_runtime.h"       // _PyRuntime
#అన్తర్భావ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#అన్తర్భావ <errcode.h>

#అన్తర్భావ "lexer/state.h"
#అన్తర్భావ "tokenizer/tokenizer.h"
#అన్తర్భావ "pegen.h"

#నిర్వచించు IDENTIFIER_CACHE_కొలత 2048  // Must be a power of two.
#నిర్వచించు IDENTIFIER_CACHE_MAX_PROBES 8
#నిర్వచించు PEGEN_ARRAY_GROWTH_గుణకం 2
#నిర్వచించు TYPE_IGNORE_INITIAL_సామర్థ్యం 10

నిర్మాణం identifier_cache_నమోదు {
    స్థిరరాశి అక్షరం *కీలకం;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *విలువ;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ఒకవేళ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    తిరిగి ఖాళీ;
}

Py_ssize_t
Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం_పంక్తి(PyObject *పంక్తి, Py_ssize_t నిలువు_స్థానభ్రంశం, Py_ssize_t చివర_నిలువు_స్థానభ్రంశం)
{
    స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం = (స్థిరరాశి గుర్తులేని అక్షరం*)PyUnicode_AsUTF8(పంక్తి);
    ఒకవేళ (దత్తాంశం == ఖాళీ) {
        తిరిగి -1;
    }

    Py_ssize_t len = 0;
    అయితే (నిలువు_స్థానభ్రంశం < చివర_నిలువు_స్థానభ్రంశం) {
        Py_UCS4 ch = దత్తాంశం[నిలువు_స్థానభ్రంశం];
        ఒకవేళ (ch < 0x80) {
            నిలువు_స్థానభ్రంశం += 1;
        } లేకపోతే ఒకవేళ ((ch & 0xe0) == 0xc0) {
            నిలువు_స్థానభ్రంశం += 2;
        } లేకపోతే ఒకవేళ ((ch & 0xf0) == 0xe0) {
            నిలువు_స్థానభ్రంశం += 3;
        } లేకపోతే ఒకవేళ ((ch & 0xf8) == 0xf0) {
            నిలువు_స్థానభ్రంశం += 4;
        } లేకపోతే {
            Py_Err_సమితి_String(Py_Exc_విలువ_Error, "Invalid UTF-8 sequence");
            తిరిగి -1;
        }
        len++;
    }
    తిరిగి len;
}

Py_ssize_t
Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం_raw(స్థిరరాశి అక్షరం* str, Py_ssize_t నిలువు_స్థానభ్రంశం)
{
    Py_ssize_t len = (Py_ssize_t)అక్షరశ్రేణి_పొడవు(str);
    ఒకవేళ (నిలువు_స్థానభ్రంశం > len + 1) {
        నిలువు_స్థానభ్రంశం = len + 1;
    }
    ప్రతిజ్ఞా(నిలువు_స్థానభ్రంశం >= 0);
    PyObject *పాఠ్యం = PyUnicode_DecodeUTF8(str, నిలువు_స్థానభ్రంశం, "replace");
    ఒకవేళ (!పాఠ్యం) {
        తిరిగి -1;
    }
    Py_ssize_t కొలత = Py_Unicode_GET_నిడివి(పాఠ్యం);
    Py_DECREF(పాఠ్యం);
    తిరిగి కొలత;
}

Py_ssize_t
Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం(PyObject *పంక్తి, Py_ssize_t నిలువు_స్థానభ్రంశం)
{
    స్థిరరాశి అక్షరం *str = PyUnicode_AsUTF8(పంక్తి);
    ఒకవేళ (!str) {
        తిరిగి -1;
    }
    తిరిగి Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం_raw(str, నిలువు_స్థానభ్రంశం);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
పూర్ణాంకం
Py_Pegen_చొప్పించు_జ్ఞాపిక(Parser *p, పూర్ణాంకం mark, పూర్ణాంకం type, శూన్య *కణుపు)
{
    // Insert in front
    జ్ఞాపిక *m = _PyArena_Malloc(p->arena, పరిమాణం(జ్ఞాపిక));
    ఒకవేళ (m == ఖాళీ) {
        తిరిగి -1;
    }
    m->type = type;
    m->కణుపు = కణుపు;
    m->mark = p->mark;
    m->తదుపరి = p->టోకెన్లు[mark]->జ్ఞాపిక_3;
    p->టోకెన్లు[mark]->జ్ఞాపిక_3 = m;
    p->టోకెన్లు[mark]->జ్ఞాపిక_ముసుగు |= 1ULL << (type & 63);
    తిరిగి 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
జ్ఞాపిక *
Py_Pegen_చొప్పించు_జ్ఞాపిక_ప్రత్యక్ష(Parser *p, పూర్ణాంకం mark, పూర్ణాంకం type)
{
    ఒకవేళ (Py_Pegen_చొప్పించు_జ్ఞాపిక(p, mark, type, ఖాళీ) < 0) {
        తిరిగి ఖాళీ;
    }
    తిరిగి p->టోకెన్లు[mark]->జ్ఞాపిక_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
పూర్ణాంకం
Py_Pegen_update_జ్ఞాపిక(Parser *p, పూర్ణాంకం mark, పూర్ణాంకం type, శూన్య *కణుపు)
{
    కొరకు (జ్ఞాపిక *m = p->టోకెన్లు[mark]->జ్ఞాపిక_3; m != ఖాళీ; m = m->తదుపరి) {
        ఒకవేళ (m->type == type) {
            // Update existing node.
            m->కణుపు = కణుపు;
            m->mark = p->mark;
            తిరిగి 0;
        }
    }
    // Insert new node.
    తిరిగి Py_Pegen_చొప్పించు_జ్ఞాపిక(p, mark, type, కణుపు);
}

స్థిర పూర్ణాంకం
init_normalization(Parser *p)
{
    ఒకవేళ (p->normalize) {
        తిరిగి 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ఒకవేళ (!p->normalize)
    {
        తిరిగి 0;
    }
    తిరిగి 1;
}

పూర్ణాంకం
Py_Pegen_grow_loop_బఫరు(శూన్య ***బఫరు, Py_ssize_t *సామర్థ్యం)
{
    ఒకవేళ ((కొలత_t)*సామర్థ్యం > (కొలత_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_గుణకం * పరిమాణం(**బఫరు))) {
        తిరిగి -1;
    }
    Py_ssize_t new_సామర్థ్యం = *సామర్థ్యం == 0
        ? 1 : *సామర్థ్యం * PEGEN_ARRAY_GROWTH_గుణకం;
    శూన్య **new_బఫరు = PyMem_Realloc(*బఫరు, new_సామర్థ్యం * పరిమాణం(**బఫరు));
    ఒకవేళ (new_బఫరు == ఖాళీ) {
        తిరిగి -1;
    }
    *బఫరు = new_బఫరు;
    *సామర్థ్యం = new_సామర్థ్యం;
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
growable_comment_array_కూడు(growable_comment_array *arr, పూర్ణాంకం lineno, అక్షరం *comment)
{
    ఒకవేళ (arr->అంకె_వస్తువులు >= arr->కొలత) {
        ఒకవేళ (arr->కొలత > (కొలత_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_గుణకం * పరిమాణం(*arr->వస్తువులు))) {
            తిరిగి 0;
        }
        కొలత_t new_కొలత = arr->కొలత == 0
            ? TYPE_IGNORE_INITIAL_సామర్థ్యం
            : arr->కొలత * PEGEN_ARRAY_GROWTH_గుణకం;
        శూన్య *new_వస్తువులు_array = PyMem_Realloc(
            arr->వస్తువులు, new_కొలత * పరిమాణం(*arr->వస్తువులు));
        ఒకవేళ (!new_వస్తువులు_array) {
            తిరిగి 0;
        }
        arr->వస్తువులు = new_వస్తువులు_array;
        arr->కొలత = new_కొలత;
    }

    arr->వస్తువులు[arr->అంకె_వస్తువులు].lineno = lineno;
    arr->వస్తువులు[arr->అంకె_వస్తువులు].comment = comment;  // Take ownership
    arr->అంకె_వస్తువులు++;
    తిరిగి 1;
}

స్థిర శూన్య
growable_comment_array_deallocate(growable_comment_array *arr)
{
    కొరకు (కొలత_t i = 0; i < arr->అంకె_వస్తువులు; i++) {
        PyMem_Free(arr->వస్తువులు[i].comment);
    }
    PyMem_Free(arr->వస్తువులు);
}

స్థిర పూర్ణాంకం
_get_keyword_or_name_type(Parser *p, స్థిరరాశి అక్షరం *పాఠ్యం, Py_ssize_t నిడివి)
{
    ప్రతిజ్ఞా(నిడివి > 0);

    ఒకవేళ (నిడివి >= p->n_keyword_lists ||
        p->keywords[నిడివి] == ఖాళీ ||
        p->keywords[నిడివి]->type == -1) {
        తిరిగి NAME;
    }
    కొరకు (KeywordToken *k = p->keywords[నిడివి]; k != ఖాళీ && k->type != -1; k++) {
        ఒకవేళ (memcmp(k->str, పాఠ్యం, (కొలత_t)నిడివి) == 0) {
            తిరిగి k->type;
        }
    }
    తిరిగి NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
స్థిర అంతఃవరుస పూర్ణాంకం
token_needs_పాఠ్యం(పూర్ణాంకం type)
{
    ఎంపిక (type) {
        స్థితి NAME:
        స్థితి సంఖ్య:
        స్థితి STRING:
        స్థితి FSTRING_ప్రారంభం:
        స్థితి FSTRING_మధ్య:
        స్థితి FSTRING_చివర:
        స్థితి TSTRING_ప్రారంభం:
        స్థితి TSTRING_మధ్య:
        స్థితి TSTRING_చివర:
        స్థితి TYPE_COMMENT:
        స్థితి NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            తిరిగి 1;
        అప్రమేయ:
            తిరిగి 0;
    }
}

స్థిర పూర్ణాంకం
initialize_token(Parser *p, Token *parser_token, నిర్మాణం token *new_token, పూర్ణాంకం token_type) {
    ప్రతిజ్ఞా(parser_token != ఖాళీ);

    parser_token->type = token_type;
    parser_token->బైట్లు = ఖాళీ;
    ఒకవేళ (token_needs_పాఠ్యం(token_type) && new_token->పరిధి.ప్రారంభం >= 0) {
        Py_ssize_t నిడివి;
        స్థిరరాశి అక్షరం *పాఠ్యం = Py_Token_పాఠ్యం_View(p->tok, new_token, &నిడివి);
        ఒకవేళ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, పాఠ్యం, నిడివి);
        }
        // A name reclassified as a keyword does not need its text.
        ఒకవేళ (parser_token->type == token_type) {
            parser_token->బైట్లు = Py_బైట్లు_From_String_And_కొలత(పాఠ్యం, నిడివి);
            ఒకవేళ (parser_token->బైట్లు == ఖాళీ) {
                తిరిగి -1;
            }
            ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, parser_token->బైట్లు) < 0) {
                Py_DECREF(parser_token->బైట్లు);
                తిరిగి -1;
            }
        }
    }

    parser_token->metadata = ఖాళీ;
    ఒకవేళ (new_token->metadata != ఖాళీ) {
        ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, new_token->metadata) < 0) {
            తిరిగి -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ఖాళీ;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->ప్రారంభం_loc.lineno;
    parser_token->నిలువు_స్థానభ్రంశం = new_token->చివర_loc.lineno == p->starting_lineno
        ? p->starting_నిలువు_స్థానభ్రంశం + new_token->ప్రారంభం_loc.byte_నిలువు
        : new_token->ప్రారంభం_loc.byte_నిలువు;
    parser_token->చివర_lineno = new_token->చివర_loc.lineno;
    parser_token->చివర_నిలువు_స్థానభ్రంశం = new_token->చివర_loc.lineno == p->starting_lineno
        ? p->starting_నిలువు_స్థానభ్రంశం + new_token->చివర_loc.byte_నిలువు
        : new_token->చివర_loc.byte_నిలువు;

    p->fill += 1;

    ఒకవేళ (token_type == ERRORTOKEN && Py_Tokenizer_Get_స్థితి(p->tok) == E_DECODE) {
        తిరిగి _Pypegen_raise_decode_error(p);
    }

    తిరిగి (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

స్థిర పూర్ణాంకం
resize_టోకెన్లు_array(Parser *p) {
    ఒకవేళ (p->కొలత > గరిష్ఠ_పూర్ణాంకం / PEGEN_ARRAY_GROWTH_గుణకం ||
            (కొలత_t)p->కొలత > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_గుణకం * పరిమాణం(*p->టోకెన్లు))) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    పూర్ణాంకం newsize = p->కొలత * PEGEN_ARRAY_GROWTH_గుణకం;
    Token **new_టోకెన్లు = PyMem_Realloc(p->టోకెన్లు, (కొలత_t)newsize * పరిమాణం(Token *));
    ఒకవేళ (new_టోకెన్లు == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    p->టోకెన్లు = new_టోకెన్లు;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((కొలత_t)(newsize - p->కొలత), పరిమాణం(Token));
    ఒకవేళ (chunk == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    కొరకు (పూర్ణాంకం i = p->కొలత; i < newsize; i++) {
        p->టోకెన్లు[i] = &chunk[i - p->కొలత];
    }
    p->కొలత = newsize;
    తిరిగి 0;
}

పూర్ణాంకం
_PyPegen_fill_token(Parser *p)
{
    నిర్మాణం token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    అయితే (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        స్థిరరాశి అక్షరం *పాఠ్యం = Py_Token_పాఠ్యం_View(p->tok, &new_token, &len);
        అక్షరం *ట్యాగ్ = PyMem_Malloc((కొలత_t)len + 1);
        ఒకవేళ (ట్యాగ్ == ఖాళీ) {
            Py_Err_No_జ్ఞాపకం();
            వెళ్ళు error;
        }
        జ్ఞాపకం_నకలు(ట్యాగ్, పాఠ్యం, (కొలత_t)len);
        ట్యాగ్[len] = '\0';
        // Ownership of tag passes to the growable array
        ఒకవేళ (!growable_comment_array_కూడు(&p->type_ignore_comments, new_token.చివర_loc.lineno, ట్యాగ్)) {
            PyMem_Free(ట్యాగ్);
            Py_Err_No_జ్ఞాపకం();
            వెళ్ళు error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    పూర్ణాంకం type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ఒకవేళ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం) &&
         Py_Tokenizer_Get_స్థితి(p->tok) == E_EOF)) {
        p->ఆదానం_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ఒకవేళ (p->ప్రారంభం_నియమం == Py_single_ఆదానం && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ఒకవేళ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    లేకపోతే {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ఒకవేళ ((p->fill == p->కొలత) && (resize_టోకెన్లు_array(p) != 0)) {
        వెళ్ళు error;
    }

    Token *t = p->టోకెన్లు[p->fill];
    పూర్ణాంకం ఫలితం = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    తిరిగి ఫలితం;
error:
    _PyToken_Free(&new_token);
    తిరిగి -1;
}

#ఒకవేళ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#నిర్వచించు NSTATISTICS _PYPEGEN_NSTATISTICS
#నిర్వచించు జ్ఞాపిక_statistics _PyRuntime.parser.జ్ఞాపిక_statistics

శూన్య
Py_Pegen_clear_జ్ఞాపిక_statistics(శూన్య)
{
    Py_Mutex_తాళం(&_PyRuntime.parser.mutex);
    కొరకు (పూర్ణాంకం i = 0; i < NSTATISTICS; i++) {
        జ్ఞాపిక_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_జ్ఞాపిక_statistics(శూన్య)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ఒకవేళ (ret == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    Py_Mutex_తాళం(&_PyRuntime.parser.mutex);
    కొరకు (పూర్ణాంకం i = 0; i < NSTATISTICS; i++) {
        PyObject *విలువ = PyLong_FromLong(జ్ఞాపిక_statistics[i]);
        ఒకవేళ (విలువ == ఖాళీ) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            తిరిగి ఖాళీ;
        }
        // PyList_SetItem borrows a reference to value.
        ఒకవేళ (Py_List_సమితి_అంశం(ret, i, విలువ) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            తిరిగి ఖాళీ;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    తిరిగి ret;
}
#అయితే_ముగింపు

పూర్ణాంకం  // bool
_PyPegen_is_memoized(Parser *p, పూర్ణాంకం type, శూన్య *pres)
{
    ఒకవేళ (p->mark == p->fill) {
        ఒకవేళ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            తిరిగి -1;
        }
    }

    Token *t = p->టోకెన్లు[p->mark];

    ఒకవేళ (!(t->జ్ఞాపిక_ముసుగు & (1ULL << (type & 63)))) {
        తిరిగి 0;
    }

    కొరకు (జ్ఞాపిక *m = t->జ్ఞాపిక_3; m != ఖాళీ; m = m->తదుపరి) {
        ఒకవేళ (m->type == type) {
#ఒకవేళ defined(Py_DEBUG)
            ఒకవేళ (0 <= type && type < NSTATISTICS) {
                దీర్ఘ లెక్క = m->mark - p->mark;
                // A memoized negative result counts for one.
                ఒకవేళ (లెక్క <= 0) {
                    లెక్క = 1;
                }
                Py_Mutex_తాళం(&_PyRuntime.parser.mutex);
                జ్ఞాపిక_statistics[type] += లెక్క;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#అయితే_ముగింపు
            p->mark = m->mark;
            *(శూన్య **)(pres) = m->కణుపు;
            తిరిగి 1;
        }
    }
    తిరిగి 0;
}

#నిర్వచించు LOOKAHEAD1(NAME, RES_TYPE)                                  \
    పూర్ణాంకం                                                             \
    NAME (పూర్ణాంకం ధనాత్మక, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        పూర్ణాంకం mark = p->mark;                                         \
        శూన్య *res = func(p);                                        \
        p->mark = mark;                                             \
        తిరిగి (res != ఖాళీ) == ధనాత్మక;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, శూన్య *)
LOOKAHEAD1(Py_Pegen_lookahead_for_సమాసం, సమాసం_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#నిర్వచనం_రద్దు LOOKAHEAD1

#నిర్వచించు LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    పూర్ణాంకం                                                                 \
    NAME (పూర్ణాంకం ధనాత్మక, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        పూర్ణాంకం mark = p->mark;                                             \
        శూన్య *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        తిరిగి (res != ఖాళీ) == ధనాత్మక;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, పూర్ణాంకం)
LOOKAHEAD2(_PyPegen_lookahead_with_string, సమాసం_ty, స్థిరరాశి అక్షరం *)
#నిర్వచనం_రద్దు LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, పూర్ణాంకం type)
{
    ఒకవేళ (p->mark == p->fill) {
        ఒకవేళ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            తిరిగి ఖాళీ;
        }
    }
    Token *t = p->టోకెన్లు[p->mark];
    ఒకవేళ (t->type != type) {
       తిరిగి ఖాళీ;
    }
    p->mark += 1;
    తిరిగి t;
}

శూన్య*
Py_Pegen_expect_forced_ఫలితం(Parser *p, శూన్య* ఫలితం, స్థిరరాశి అక్షరం* expected) {

    ఒకవేళ (p->error_indicator == 1) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (ఫలితం == ఖాళీ) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        తిరిగి ఖాళీ;
    }
    తిరిగి ఫలితం;
}

Token *
_PyPegen_expect_forced_token(Parser *p, పూర్ణాంకం type, స్థిరరాశి అక్షరం* expected) {

    ఒకవేళ (p->error_indicator == 1) {
        తిరిగి ఖాళీ;
    }

    ఒకవేళ (p->mark == p->fill) {
        ఒకవేళ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            తిరిగి ఖాళీ;
        }
    }
    Token *t = p->టోకెన్లు[p->mark];
    ఒకవేళ (t->type != type) {
        RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(t, "expected '%s'", expected);
        తిరిగి ఖాళీ;
    }
    p->mark += 1;
    తిరిగి t;
}

సమాసం_ty
_PyPegen_expect_soft_keyword(Parser *p, స్థిరరాశి అక్షరం *keyword)
{
    ఒకవేళ (p->mark == p->fill) {
        ఒకవేళ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            తిరిగి ఖాళీ;
        }
    }
    Token *t = p->టోకెన్లు[p->mark];
    ఒకవేళ (t->type != NAME) {
        తిరిగి ఖాళీ;
    }
    స్థిరరాశి అక్షరం *s = Py_బైట్లు_As_String(t->బైట్లు);
    ఒకవేళ (!s) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (strcmp(s, keyword) != 0) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyPegen_name_token(p);
}

Token *
Py_Pegen_get_చివరి_nonnwhitespace_token(Parser *p)
{
    ప్రతిజ్ఞా(p->mark >= 0);
    Token *token = ఖాళీ;
    కొరకు (పూర్ణాంకం m = p->mark - 1; m >= 0; m--) {
        token = p->టోకెన్లు[m];
        ఒకవేళ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            ఆపు;
        }
    }
    తిరిగి token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, స్థిరరాశి అక్షరం *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)అక్షరశ్రేణి_పొడవు(n), ఖాళీ);
    ఒకవేళ (!id) {
        వెళ్ళు error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ఒకవేళ (!PyUnicode_IS_ASCII(id))
    {
        ఒకవేళ (!init_normalization(p))
        {
            Py_DECREF(id);
            వెళ్ళు error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ఒకవేళ (form == ఖాళీ)
        {
            Py_DECREF(id);
            వెళ్ళు error;
        }
        PyObject *వాదనలు[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, వాదనలు, 2, ఖాళీ);
        Py_DECREF(id);
        Py_DECREF(form);
        ఒకవేళ (!id2) {
            వెళ్ళు error;
        }

        ఒకవేళ (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            వెళ్ళు error;
        }
        id = id2;
    }
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి forbidden[] = {
        "None",
        "True",
        "False",
        ఖాళీ
    };
    కొరకు (పూర్ణాంకం i = 0; forbidden[i] != ఖాళీ; i++) {
        ఒకవేళ (Py_Unicode_Equal_గమ్యం_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_విలువ_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            వెళ్ళు error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, id) < 0)
    {
        Py_DECREF(id);
        వెళ్ళు error;
    }
    తిరిగి id;

error:
    p->error_indicator = 1;
    తిరిగి ఖాళీ;
}

// Return an arena-owned identifier; callers borrow the reference.
స్థిర PyObject *
get_cached_identifier(Parser *p, PyObject *బైట్లు)
{
    స్థిరరాశి అక్షరం *s = Py_బైట్లు_As_String(బైట్లు);
    ఒకవేళ (!s) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_బైట్లు_GET_కొలత(బైట్లు);
    Py_hash_t hash = PyObject_Hash(బైట్లు);
    ఒకవేళ (hash == -1) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    // Parses without identifiers do not need this cache.
    ఒకవేళ (p->identifier_cache == ఖాళీ) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_కొలత, పరిమాణం(*p->identifier_cache));
        ఒకవేళ (p->identifier_cache == ఖాళీ) {
            p->error_indicator = 1;
            తిరిగి Py_Err_No_జ్ఞాపకం();
        }
    }
    Identifier_Cache_నమోదు *free_స్థానం = ఖాళీ;
    కొలత_t idx = (కొలత_t)hash & (IDENTIFIER_CACHE_కొలత - 1);
    కొరకు (పూర్ణాంకం probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_నమోదు *నమోదు_2 = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_కొలత - 1)];
        ఒకవేళ (నమోదు_2->కీలకం == ఖాళీ) {
            free_స్థానం = నమోదు_2;
            ఆపు;
        }
        ఒకవేళ (నమోదు_2->hash == hash && నమోదు_2->len == len &&
            memcmp(నమోదు_2->కీలకం, s, len) == 0)
        {
            తిరిగి నమోదు_2->విలువ;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ఒకవేళ (id == ఖాళీ) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (free_స్థానం != ఖాళీ) {
        free_స్థానం->కీలకం = s;
        free_స్థానం->len = len;
        free_స్థానం->hash = hash;
        free_స్థానం->విలువ = id;
    }
    తిరిగి id;
}

స్థిర సమాసం_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ఒకవేళ (t == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    పూర్ణాంకం mark = p->mark - 1;
    p->mark = mark;
    సమాసం_ty cached = ఖాళీ;
    ఒకవేళ (_PyPegen_is_memoized(p, NAME, &cached)) {
        తిరిగి cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->బైట్లు);
    ఒకవేళ (id == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    సమాసం_ty ఫలితం = _PyAST_Name(id, ఎక్కించు, t->lineno, t->నిలువు_స్థానభ్రంశం,
                                t->చివర_lineno, t->చివర_నిలువు_స్థానభ్రంశం, p->arena);
    ఒకవేళ (ఫలితం != ఖాళీ && Py_Pegen_చొప్పించు_జ్ఞాపిక(p, mark, NAME, ఫలితం) < 0) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    తిరిగి ఫలితం;
}

సమాసం_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    తిరిగి _PyPegen_name_from_token(p, t);
}

శూన్య *
_PyPegen_string_token(Parser *p)
{
    తిరిగి _PyPegen_expect_token(p, STRING);
}

సమాసం_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ఒకవేళ (t == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    అక్షరం *the_token;
    Py_ssize_t కొలత;
    Py_బైట్లు_As_String_And_కొలత(t->బైట్లు, &the_token, &కొలత);
    కొరకు (అక్షరం **keyword = p->soft_keywords; *keyword != ఖాళీ; keyword++) {
        ఒకవేళ (అక్షరశ్రేణి_పొడవు(*keyword) == (కొలత_t)కొలత &&
            అక్షరశ్రేణి_పోలిక(*keyword, the_token, (కొలత_t)కొలత) == 0) {
            తిరిగి _PyPegen_name_from_token(p, t);
        }
    }
    తిరిగి ఖాళీ;
}

స్థిర PyObject *
parsenumber_raw(స్థిరరాశి అక్షరం *s)
{
    స్థిరరాశి అక్షరం *చివర;
    దీర్ఘ x;
    ద్విగుణ dx;
    Py_complex compl;
    పూర్ణాంకం imflag;

    ప్రతిజ్ఞా(s != ఖాళీ);
    errno = 0;
    చివర = s + అక్షరశ్రేణి_పొడవు(s) - 1;
    imflag = *చివర == 'j' || *చివర == 'J';
    ఒకవేళ (s[0] == '0') {
        x = (దీర్ఘ)PyOS_strtoul(s, (అక్షరం **)&చివర, 0);
        ఒకవేళ (x < 0 && errno == 0) {
            తిరిగి PyLong_FromString(s, (అక్షరం **)0, 0);
        }
    }
    లేకపోతే {
        x = PyOS_strtol(s, (అక్షరం **)&చివర, 0);
    }
    ఒకవేళ (*చివర == '\0') {
        ఒకవేళ (errno != 0) {
            తిరిగి PyLong_FromString(s, (అక్షరం **)0, 0);
        }
        తిరిగి PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ఒకవేళ (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_గమ్యం_double(s, (అక్షరం **)&చివర, ఖాళీ);
        ఒకవేళ (compl.imag == -1.0 && PyErr_Occurred()) {
            తిరిగి ఖాళీ;
        }
        తిరిగి PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_గమ్యం_double(s, ఖాళీ, ఖాళీ);
    ఒకవేళ (dx == -1.0 && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }
    తిరిగి PyFloat_FromDouble(dx);
}

స్థిర PyObject *
parsenumber(స్థిరరాశి అక్షరం *s)
{
    అక్షరం *dup;
    అక్షరం *చివర;
    PyObject *res = ఖాళీ;

    ప్రతిజ్ఞా(s != ఖాళీ);

    ఒకవేళ (strchr(s, '_') == ఖాళీ) {
        తిరిగి parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(అక్షరశ్రేణి_పొడవు(s) + 1);
    ఒకవేళ (dup == ఖాళీ) {
        తిరిగి Py_Err_No_జ్ఞాపకం();
    }
    చివర = dup;
    కొరకు (; *s; s++) {
        ఒకవేళ (*s != '_') {
            *చివర++ = *s;
        }
    }
    *చివర = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    తిరిగి res;
}

సమాసం_ty
Py_Pegen_సంఖ్య_token(Parser *p)
{
    పూర్ణాంకం mark = p->mark;
    Token *t = _PyPegen_expect_token(p, సంఖ్య);
    ఒకవేళ (t == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    సమాసం_ty cached = ఖాళీ;
    ఒకవేళ (_PyPegen_is_memoized(p, సంఖ్య, &cached)) {
        తిరిగి cached;
    }
    p->mark = mark + 1;

    స్థిరరాశి అక్షరం *అంకె_raw = Py_బైట్లు_As_String(t->బైట్లు);
    ఒకవేళ (అంకె_raw == ఖాళీ) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }

    ఒకవేళ (p->feature_సంస్కరణ < 6 && strchr(అంకె_raw, '_') != ఖాళీ) {
        p->error_indicator = 1;
        తిరిగి RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(అంకె_raw);

    ఒకవేళ (c == ఖాళీ) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ఒకవేళ (tstate->current_exception != ఖాళీ &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_విలువ_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_తెలిసిన_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->చివర_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        తిరిగి ఖాళీ;
    }

    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }

    సమాసం_ty ఫలితం = _PyAST_Constant(c, ఖాళీ, t->lineno, t->నిలువు_స్థానభ్రంశం,
                                    t->చివర_lineno, t->చివర_నిలువు_స్థానభ్రంశం, p->arena);
    ఒకవేళ (ఫలితం != ఖాళీ && Py_Pegen_చొప్పించు_జ్ఞాపిక(p, mark, సంఖ్య, ఫలితం) < 0) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    తిరిగి ఫలితం;
}



స్థిర పూర్ణాంకం
compute_parser_flags(PyCompilerFlags *flags)
{
    పూర్ణాంకం parser_flags = 0;
    ఒకవేళ (!flags) {
        తిరిగి 0;
    }
    ఒకవేళ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ఒకవేళ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ఒకవేళ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ఒకవేళ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ఒకవేళ (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_ఆదానం) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_ఆదానం;
    }
    తిరిగి parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(నిర్మాణం tok_state *tok, పూర్ణాంకం ప్రారంభం_నియమం, పూర్ణాంకం flags,
                    పూర్ణాంకం feature_సంస్కరణ, పూర్ణాంకం *errcode, స్థిరరాశి అక్షరం* ఆధారం, PyArena *arena)
{
    Parser *p = PyMem_Malloc(పరిమాణం(Parser));
    ఒకవేళ (p == ఖాళీ) {
        తిరిగి (Parser *) Py_Err_No_జ్ఞాపకం();
    }
    ప్రతిజ్ఞా(tok != ఖాళీ);
    Py_Tokenizer_సమితి_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_ఆదానం(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం);
    p->tok = tok;
    p->keywords = ఖాళీ;
    p->n_keyword_lists = -1;
    p->soft_keywords = ఖాళీ;
    p->టోకెన్లు = PyMem_Malloc(పరిమాణం(Token *));
    ఒకవేళ (!p->టోకెన్లు) {
        PyMem_Free(p);
        తిరిగి (Parser *) Py_Err_No_జ్ఞాపకం();
    }
    p->టోకెన్లు[0] = PyMem_Calloc(1, పరిమాణం(Token));
    ఒకవేళ (!p->టోకెన్లు[0]) {
        PyMem_Free(p->టోకెన్లు);
        PyMem_Free(p);
        తిరిగి (Parser *) Py_Err_No_జ్ఞాపకం();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->కొలత = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->ప్రారంభం_నియమం = ప్రారంభం_నియమం;
    p->parsing_started = 0;
    p->normalize = ఖాళీ;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_నిలువు_స్థానభ్రంశం = 0;
    p->flags = flags;
    p->feature_సంస్కరణ = feature_సంస్కరణ;
    p->తెలిసిన_err_token = ఖాళీ;
    p->identifier_cache = ఖాళీ;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    ప్రతిజ్ఞా(((_PyThreadStateImpl *)p->tstate)->c_దొంతర_hard_హద్దు != 0);
    p->దొంతర_soft_హద్దు = ((_PyThreadStateImpl *)p->tstate)->c_దొంతర_soft_హద్దు;
#ఒకవేళ Py_దొంతర_GROWS_కిందికి
    p->దొంతర_soft_హద్దు += Py_OS_దొంతర_MARGIN_బైట్లు;
#లేకపోతే
    p->దొంతర_soft_హద్దు -= Py_OS_దొంతర_MARGIN_బైట్లు;
#అయితే_ముగింపు
    p->level = 0;
    p->call_invalid_నియమాలు = 0;
    p->ఆదానం_exhausted = 0;
    p->చివరి_stmt_location.lineno = 0;
    p->చివరి_stmt_location.నిలువు_స్థానభ్రంశం = 0;
    p->చివరి_stmt_location.చివర_lineno = 0;
    p->చివరి_stmt_location.చివర_నిలువు_స్థానభ్రంశం = 0;
#నిర్వచితమైతే Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#అయితే_ముగింపు
    తిరిగి p;
}

శూన్య
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->టోకెన్లు[0]);
    కొరకు (పూర్ణాంకం i = 1; i < p->కొలత; i *= PEGEN_ARRAY_GROWTH_గుణకం) {
        PyMem_Free(p->టోకెన్లు[i]);
    }
    PyMem_Free(p->టోకెన్లు);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

స్థిర శూన్య
reset_parser_state_for_error_pass(Parser *p)
{
    p->చివరి_stmt_location.lineno = 0;
    p->చివరి_stmt_location.నిలువు_స్థానభ్రంశం = 0;
    p->చివరి_stmt_location.చివర_lineno = 0;
    p->చివరి_stmt_location.చివర_నిలువు_స్థానభ్రంశం = 0;
    కొరకు (పూర్ణాంకం i = 0; i < p->fill; i++) {
        p->టోకెన్లు[i]->జ్ఞాపిక_3 = ఖాళీ;
        p->టోకెన్లు[i]->జ్ఞాపిక_ముసుగు = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_ఆదానం;
    Py_Tokenizer_Allow_Incomplete_ఆదానం(p->tok, 0);
    p->call_invalid_నియమాలు = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

స్థిర శూన్య
Py_Pegen_సమితి_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ఒకవేళ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_సమితి_Raised_Exception(exc);
        తిరిగి;
    }
    స్థిరరాశి అక్షరం *ఆధారం = Py_Tokenizer_Retained_ఆధారం(p->tok);
    స్థిరరాశి అక్షరం *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_ఆధారం = ఖాళీ;
    ఒకవేళ (ఆధారం) {
        ఒకవేళ (encoding == ఖాళీ) {
            the_ఆధారం = PyUnicode_FromString(ఆధారం);
        } లేకపోతే {
            the_ఆధారం = PyUnicode_Decode(ఆధారం, అక్షరశ్రేణి_పొడవు(ఆధారం), encoding, ఖాళీ);
        }
    }
    ఒకవేళ (!the_ఆధారం) {
        PyErr_Clear();
        the_ఆధారం = Py_None;
        Py_INCREF(the_ఆధారం);
    }
    PyObject* metadata = Py_Build_విలువ(
        "(iiN)",
        p->చివరి_stmt_location.lineno,
        p->చివరి_stmt_location.నిలువు_స్థానభ్రంశం,
        the_ఆధారం // N gives ownership to metadata
    );
    ఒకవేళ (!metadata) {
        PyErr_Clear();
        Py_Err_సమితి_Raised_Exception(exc);
        తిరిగి;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_సమితి_Raised_Exception(exc);
}

శూన్య *
Py_Pegen_నిరంతరం_parser(Parser *p)
{
    శూన్య *res = _PyPegen_parse(p);
    ప్రతిజ్ఞా(p->level == 0);
    ఒకవేళ (res != ఖాళీ && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (res == ఖాళీ) {
        ఒకవేళ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_ఆదానం_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            తిరిగి ఖాళీ;
        }
        ఒకవేళ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం) &&
            p->ఆదానం_exhausted && !PyErr_Occurred()) {
            తిరిగి Py_Pegen_raise_incomplete_ఆదానం(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *చివరి_token = p->టోకెన్లు[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_సమితి_syntax_error(p, చివరి_token);

        // Set the metadata in the exception from p->last_stmt_location
        ఒకవేళ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_సమితి_syntax_error_metadata(p);
        }
       తిరిగి ఖాళీ;
    }

    ఒకవేళ (p->ప్రారంభం_నియమం == Py_single_ఆదానం && _PyTokenizer_HasTrailingStatement(p->tok)) {
        తిరిగి RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ఒకవేళ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ఒకవేళ (p->ప్రారంభం_నియమం == Py_single_ఆదానం ||
        p->ప్రారంభం_నియమం == Py_file_ఆదానం ||
        p->ప్రారంభం_నియమం == Py_eval_ఆదానం)
    {
        ఒకవేళ (!_PyAST_Validate(res)) {
            తిరిగి ఖాళీ;
        }
    }
#అయితే_ముగింపు
    తిరిగి res;
}

mod_ty
Py_Pegen_నిరంతరం_parser_from_file_సూచిక(FILE *fp, పూర్ణాంకం ప్రారంభం_నియమం, PyObject *filename_ob,
                             స్థిరరాశి అక్షరం *enc, స్థిరరాశి అక్షరం *ps1, స్థిరరాశి అక్షరం *ps2,
                             PyCompilerFlags *flags, పూర్ణాంకం *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    పూర్ణాంకం parser_flags = compute_parser_flags(flags);
    నిర్మాణం tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ఒకవేళ (tok == ఖాళీ) {
        ఒకవేళ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        లేకపోతే {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_జ్ఞాపకం();
        }
        తిరిగి ఖాళీ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty ఫలితం = ఖాళీ;

    PyObject *module = PyUnicode_FromString("__main__");
    ఒకవేళ (module == ఖాళీ) {
        వెళ్ళు error;
    }
    Py_Tokenizer_సమితి_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, ప్రారంభం_నియమం, parser_flags, PY_MINOR_సంస్కరణ,
                                    errcode, ఖాళీ, arena);
    ఒకవేళ (p == ఖాళీ) {
        వెళ్ళు error;
    }

    ఫలితం = Py_Pegen_నిరంతరం_parser(p);
    _PyPegen_Parser_Free(p);

    స్థిరరాశి అక్షరం *ఆధారం = Py_Tokenizer_Retained_ఆధారం(tok);
    ఒకవేళ (ఆధారం != ఖాళీ && ఫలితం && interactive_src != ఖాళీ) {
        *interactive_src = PyUnicode_FromString(ఆధారం);
        ఒకవేళ (!*interactive_src || Py_Arena_కూడు_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            ఫలితం = ఖాళీ;
            వెళ్ళు error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    తిరిగి ఫలితం;
}

mod_ty
Py_Pegen_నిరంతరం_parser_from_string(స్థిరరాశి అక్షరం *str, పూర్ణాంకం ప్రారంభం_నియమం, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_బహుళకం = ప్రారంభం_నియమం == Py_file_ఆదానం
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    పూర్ణాంకం parser_flags = compute_parser_flags(flags);
    ఒకవేళ (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం) {
        newline_బహుళకం = _PyTokenizer_APPEND_NEWLINE;
    }

    నిర్మాణం tok_state *tok;
    ఒకవేళ (flags != ఖాళీ && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_బహుళకం, 0);
    } లేకపోతే {
        tok = _PyTokenizer_FromString(str, newline_బహుళకం, 0);
    }
    ఒకవేళ (tok == ఖాళీ) {
        ఒకవేళ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        లేకపోతే {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_జ్ఞాపకం();
        }
        తిరిగి ఖాళీ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_సమితి_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty ఫలితం = ఖాళీ;

    పూర్ణాంకం feature_సంస్కరణ = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_సంస్కరణ : PY_MINOR_సంస్కరణ;
    Parser *p = _PyPegen_Parser_New(tok, ప్రారంభం_నియమం, parser_flags, feature_సంస్కరణ,
                                    ఖాళీ, str, arena);
    ఒకవేళ (p == ఖాళీ) {
        వెళ్ళు error;
    }

    ఫలితం = Py_Pegen_నిరంతరం_parser(p);
    ఒకవేళ (ఫలితం != ఖాళీ && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం)) {
        పూర్ణాంకం needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ఒకవేళ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (ప్రారంభం_నియమం != Py_file_ఆదానం &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (ప్రారంభం_నియమం == Py_single_ఆదానం && p->ఆదానం_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_ఆదానం(p);
            ఒకవేళ (incomplete != ఖాళీ && PyErr_ExceptionMatches(Py_Exc_Incomplete_ఆదానం_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            లేకపోతే {
                ఫలితం = ఖాళీ;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    తిరిగి ఫలితం;
}
