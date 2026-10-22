#-------------------------------------------------------------------------------
# Parser for ASDL [1] definition files. Reads in an ASDL description and parses
# it into an AST that describes it.
#
# The EBNF we're parsing here: Figure 1 of the paper [1]. Extended to support
# modules and attributes after a product. Words starting with Capital letters
# are terminals. Literal tokens are in "double quotes". Others are
# non-terminals. Id is either TokenId or ConstructorId.
#
# module        ::= "module" Id "{" [definitions] "}"
# definitions   ::= { TypeId "=" type }
# type          ::= product | sum
# product       ::= fields ["attributes" fields]
# fields        ::= "(" { field, "," } field ")"
# field         ::= TypeId { "?" | "*" } [Id]
# sum           ::= constructor { "|" constructor } ["attributes" fields]
# constructor   ::= ConstructorId [fields]
#
# [1] "The Zephyr Abstract Syntax Description Language" by Wang, et. al. See
#     http://asdl.sourceforge.net/
#-------------------------------------------------------------------------------
سے collections درآمد namedtuple
درآمد enum
درآمد re

__all__ = [
    'builtin_types', 'parse', 'AST', 'Module', 'Type', 'Constructor',
    'Field', 'Sum', 'Product', 'VisitorBase', 'Check', 'check']

# The following classes define nodes into which the ASDL description is parsed.
# Note: this is a "meta-AST". ASDL files (such as Python.asdl) describe the AST
# structure used by a programming language. But ASDL files themselves need to be
# parsed. This module parses ASDL files and uses a simple AST to represent them.
# See the EBNF at the top of the file to understand the logical connection
# between the various node types.

builtin_types = {'identifier', 'string', 'int', 'constant'}

جماعت AST:
    تعریف __repr__(self):
        اٹھاؤ NotImplementedError

جماعت Module(AST):
    تعریف __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.قدر برائے type میں dfns}

    تعریف __repr__(self):
        واپس 'Module({0.name}, {0.dfns})'.format(self)

جماعت Type(AST):
    تعریف __init__(self, name, قدر):
        self.name = name
        self.قدر = قدر

    تعریف __repr__(self):
        واپس 'Type({0.name}, {0.value})'.format(self)

جماعت Constructor(AST):
    تعریف __init__(self, name, fields=کچھ_نہیں):
        self.name = name
        self.fields = fields یا []

    تعریف __repr__(self):
        واپس 'Constructor({0.name}, {0.fields})'.format(self)

جماعت Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

جماعت میدان(AST):
    تعریف __init__(self, type, name=کچھ_نہیں, quantifiers=کچھ_نہیں):
        self.type = type
        self.name = name
        self.seq = جھوٹ
        self.opt = جھوٹ
        self.quantifiers = quantifiers یا []
        اگر لمبائی(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] ہے Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] ہے Quantifier.OPTIONAL

    تعریف __str__(self):
        extra = ""
        برائے mod میں self.quantifiers:
            اگر mod ہے Quantifier.SEQUENCE:
                extra += "*"
            وگرنہ_اگر mod ہے Quantifier.OPTIONAL:
                extra += "?"

        واپس "{}{} {}".format(self.type, extra, self.name)

    تعریف __repr__(self):
        اگر self.quantifiers:
            texts = []
            برائے mod میں self.quantifiers:
                اگر mod ہے Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                وگرنہ_اگر mod ہے Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        ورنہ:
            extra = ""

        اگر self.name ہے کچھ_نہیں:
            واپس 'Field({0.type}{1})'.format(self, extra)
        ورنہ:
            واپس 'Field({0.type}, {0.name}{1})'.format(self, extra)

جماعت مجموع(AST):
    تعریف __init__(self, types, اوصاف=کچھ_نہیں):
        self.types = types
        self.اوصاف = اوصاف یا []

    تعریف __repr__(self):
        اگر self.اوصاف:
            واپس 'Sum({0.types}, {0.attributes})'.format(self)
        ورنہ:
            واپس 'Sum({0.types})'.format(self)

جماعت حاصل_ضرب(AST):
    تعریف __init__(self, fields, اوصاف=کچھ_نہیں):
        self.fields = fields
        self.اوصاف = اوصاف یا []

    تعریف __repr__(self):
        اگر self.اوصاف:
            واپس 'Product({0.fields}, {0.attributes})'.format(self)
        ورنہ:
            واپس 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

جماعت Visitor_بنیاد(object):
    """Generic tree visitor for ASTs."""
    تعریف __init__(self):
        self.cache = {}

    تعریف دورہ(self, obj, *دلائل):
        klass = obj.__class__
        meth = self.cache.get(klass)
        اگر meth ہے کچھ_نہیں:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, کچھ_نہیں)
            self.cache[klass] = meth
        اگر meth:
            کوشش:
                meth(obj, *دلائل)
            استثنا Exception بطور e:
                چھاپو("Error visiting %r: %s" % (obj, e))
                اٹھاؤ

جماعت Check(Visitor_بنیاد):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    تعریف __init__(self):
        بالا(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    تعریف دورہ_Module(self, mod):
        برائے dfn میں mod.dfns:
            self.دورہ(dfn)

    تعریف دورہ_Type(self, type):
        self.دورہ(type.قدر, str(type.name))

    تعریف دورہ_مجموع(self, مجموع_2, name):
        برائے t میں مجموع_2.types:
            self.دورہ(t, name)

    تعریف دورہ_Constructor(self, cons, name):
        کلید = str(cons.name)
        conflict = self.cons.get(کلید)
        اگر conflict ہے کچھ_نہیں:
            self.cons[کلید] = name
        ورنہ:
            چھاپو('Redefinition of constructor {}'.format(کلید))
            چھاپو('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        برائے f میں cons.fields:
            self.دورہ(f, کلید)

    تعریف دورہ_میدان(self, میدان_2, name):
        کلید = str(میدان_2.type)
        l = self.types.setdefault(کلید, [])
        l.append(name)

    تعریف دورہ_حاصل_ضرب(self, prod, name):
        برائے f میں prod.fields:
            self.دورہ(f, name)

تعریف check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.دورہ(mod)

    برائے t میں v.types:
        اگر t نہیں میں mod.types اور نہیں t میں builtin_types:
            v.errors += 1
            استعمال_کرتا = ", ".join(v.types[t])
            چھاپو('Undefined type {}, used in {}'.format(t, استعمال_کرتا))
    واپس نہیں v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

تعریف parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    ساتھ open(filename, encoding="utf-8") بطور f:
        parser = ASDLParser()
        واپس parser.parse(f.اقرأ())

# Types for describing tokens in an ASDL specification.
جماعت Token_قسم:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = سلسلہ(11)

    operator_جدول = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

جماعت ASDLSyntaxError(Exception):
    تعریف __init__(self, پیغام, lineno=کچھ_نہیں):
        self.پیغام = پیغام
        self.lineno = lineno یا '<unknown>'

    تعریف __str__(self):
        واپس 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

تعریف tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    برائے lineno, سطر میں enumerate(buf.splitlines(), 1):
        برائے m میں re.finditer(r'\s*(\w+|--.*|.)', سطر.strip()):
            c = m.گروہ(1)
            اگر c[0].isalpha():
                # Some kind of identifier
                اگر c[0].isupper():
                    دو Token(Token_قسم.ConstructorId, c, lineno)
                ورنہ:
                    دو Token(Token_قسم.TypeId, c, lineno)
            وگرنہ_اگر c[:2] == '--':
                # Comment
                توڑو
            ورنہ:
                # Operators
                کوشش:
                    op_قسم = Token_قسم.operator_جدول[c]
                استثنا کلید_Error:
                    اٹھاؤ ASDLSyntaxError('Invalid operator %s' % c, lineno)
                دو Token(op_قسم, c, lineno)

جماعت ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    تعریف __init__(self):
        self._tokenizer = کچھ_نہیں
        self.cur_token = کچھ_نہیں

    تعریف parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        واپس self._parse_module()

    تعریف _parse_module(self):
        اگر self._at_keyword('module'):
            self._advance()
        ورنہ:
            اٹھاؤ ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.قدر),
                self.cur_token.lineno)
        name = self.مطابقت(self._id_kinds)
        self.مطابقت(Token_قسم.LBrace)
        defs = self._parse_definitions()
        self.مطابقت(Token_قسم.RBrace)
        واپس Module(name, defs)

    تعریف _parse_definitions(self):
        defs = []
        جب self.cur_token.قسم == Token_قسم.TypeId:
            typename = self._advance()
            self.مطابقت(Token_قسم.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        واپس defs

    تعریف _parse_type(self):
        اگر self.cur_token.قسم == Token_قسم.LParen:
            # If we see a (, it's a product
            واپس self.parse_حاصل_ضرب()
        ورنہ:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.مطابقت(Token_قسم.ConstructorId),
                                   self._parse_optional_fields())]
            جب self.cur_token.قسم  == Token_قسم.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.مطابقت(Token_قسم.ConstructorId),
                                self._parse_optional_fields()))
            واپس مجموع(sumlist, self.parse_optional_اوصاف())

    تعریف parse_حاصل_ضرب(self):
        واپس حاصل_ضرب(self._parse_fields(), self.parse_optional_اوصاف())

    تعریف _parse_fields(self):
        fields = []
        self.مطابقت(Token_قسم.LParen)
        جب self.cur_token.قسم == Token_قسم.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_میدان_quantifier()
            id = (self._advance() اگر self.cur_token.قسم میں self._id_kinds
                                  ورنہ کچھ_نہیں)
            fields.append(میدان(typename, id, quantifiers=quantifiers))
            اگر self.cur_token.قسم == Token_قسم.RParen:
                توڑو
            وگرنہ_اگر self.cur_token.قسم == Token_قسم.Comma:
                self._advance()
        self.مطابقت(Token_قسم.RParen)
        واپس fields

    تعریف _parse_optional_fields(self):
        اگر self.cur_token.قسم == Token_قسم.LParen:
            واپس self._parse_fields()
        ورنہ:
            واپس کچھ_نہیں

    تعریف parse_optional_اوصاف(self):
        اگر self._at_keyword('attributes'):
            self._advance()
            واپس self._parse_fields()
        ورنہ:
            واپس کچھ_نہیں

    تعریف parse_optional_میدان_quantifier(self):
        quantifiers = []
        جب self.cur_token.قسم میں (Token_قسم.Asterisk, Token_قسم.Question):
            اگر self.cur_token.قسم == Token_قسم.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            وگرنہ_اگر self.cur_token.قسم == Token_قسم.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        واپس quantifiers

    تعریف _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = کچھ_نہیں اگر self.cur_token ہے کچھ_نہیں ورنہ self.cur_token.قدر
        کوشش:
            self.cur_token = اگلا(self._tokenizer)
        استثنا StopIteration:
            self.cur_token = کچھ_نہیں
        واپس cur_val

    _id_kinds = (Token_قسم.ConstructorId, Token_قسم.TypeId)

    تعریف مطابقت(self, قسم):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        اگر (isinstance(قسم, tuple) اور self.cur_token.قسم میں قسم یا
            self.cur_token.قسم == قسم
            ):
            قدر = self.cur_token.قدر
            self._advance()
            واپس قدر
        ورنہ:
            اٹھاؤ ASDLSyntaxError(
                'Unmatched {} (found {})'.format(قسم, self.cur_token.قسم),
                self.cur_token.lineno)

    تعریف _at_keyword(self, keyword):
        واپس (self.cur_token.قسم == Token_قسم.TypeId اور
                self.cur_token.قدر == keyword)
