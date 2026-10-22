#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#அந்தர்பாவ "object.h"
#அந்தர்பாவ "../tokenizer/source.h"
#அந்தர்பாவ "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_அடுக்கு_INLINE_CAPACITY 1

வகைவரை எண்ணிக்கை {
    FTSTRING_MODE_நடு,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

வகைவரை எண்ணிக்கை {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_கோவை_NESTING 3

வகைவரை கட்டமைப்பு _tokenizer_comments {
    Py_ssize_t மொத்தம்;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

வகைவரை கட்டமைப்பு _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    எழுத்து quote;
    unsigned எழுத்து quote_size;
    unsigned எழுத்து debug_கோவை;
    unsigned எழுத்து replacement_depth;
    முழு paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span கோவை_span;
    tokenizer_comments *comments;
} ftstring_state;

நிலையான உள்வரி முழு
_PyLexer_IsTString(ftstring_kind kind)
{
    திரும்பு kind == TSTRING || kind == RAW_TSTRING;
}

நிலையான உள்வரி முழு
_PyLexer_IsRawString(ftstring_kind kind)
{
    திரும்பு kind == RAW_FSTRING || kind == RAW_TSTRING;
}

வகைவரை கட்டமைப்பு {
    முழு column;
    முழு alternate_column;
} indentation_level;

வகைவரை கட்டமைப்பு {
    முழு depth;
    முழு pending;
    முழு at_bol;
    முழு comment_newline;
    indentation_level அடுக்கு[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
கட்டமைப்பு tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off வரி_start;
    _PyTok_SourceText source;
    முழு done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    முழு lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    முழு level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    எழுத்து parenstack[MAXLEVEL];
    முழு parenlinenostack[MAXLEVEL];
    முழு parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    கட்டமைப்பு _PyTok_Reader *reader;

    முழு type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_அடுக்கு;
    ftstring_state ftstring_அடுக்கு_inline[FTSTRING_அடுக்கு_INLINE_CAPACITY];
    முழு ftstring_depth;
    முழு ftstring_capacity;
    முழு tok_extra_tokens;
    முழு implicit_newline;
#ifdef Py_DEBUG
    முழு debug;
#endif
    முழு barry_as_bdfl;
    unsigned எழுத்து allow_incomplete_உள்ளீடு;
    unsigned எழுத்து needs_newline;
};

நிலையான உள்வரி ftstring_state *
_PyLexer_CurrentFTString(கட்டமைப்பு tok_state *tok)
{
    ப்ரதிஜ்ஞா(tok->ftstring_அடுக்கு != வெற்று);
    ப்ரதிஜ்ஞா(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    எனில் (tok->ftstring_depth == 0) {
        திரும்பு வெற்று;
    }
    திரும்பு &tok->ftstring_அடுக்கு[tok->ftstring_depth - 1];
}

நிலையான உள்வரி எழுத்து
_PyLexer_StringPrefix(ftstring_kind kind)
{
    திரும்பு _PyLexer_IsTString(kind) ? 't' : 'f';
}

நிலையான உள்வரி முழு
_PyLexer_FTStringBracketDepth(மாறிலி கட்டமைப்பு tok_state *tok,
                              மாறிலி ftstring_state *state)
{
    திரும்பு tok->level - state->paren_level;
}

நிலையான உள்வரி முழு
_PyLexer_ByteColumn(மாறிலி கட்டமைப்பு tok_state *tok)
{
    ப்ரதிஜ்ஞா(tok->வரி_start >= 0);
    ப்ரதிஜ்ஞா(tok->cur >= tok->வரி_start);
    Py_ssize_t column = tok->cur - tok->வரி_start;
    ப்ரதிஜ்ஞா(column <= INT_MAX);
    திரும்பு (முழு)column;
}

முழு _PyLexer_token_setup(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token, முழு type, _PyTok_Off start, _PyTok_Off end);

வெறுமை _PyLexer_ImplyDedents(கட்டமைப்பு tok_state *);

வெறுமை _PyTokenizer_Free(கட்டமைப்பு tok_state *);
ftstring_state *Py_Lexer_தள்ளு_FTString(கட்டமைப்பு tok_state *);
வெறுமை Py_Lexer_எடு_FTString(கட்டமைப்பு tok_state *);


#endif
