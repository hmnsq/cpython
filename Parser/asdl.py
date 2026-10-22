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
ਤਃ collections ਆਯਾਤ namedtuple
ਆਯਾਤ enum
ਆਯਾਤ re

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

ਵਰਗ AST:
    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਉਠਾਓ NotImplementedError

ਵਰਗ Module(AST):
    ਪਰਿਭਾਸ਼ਾ __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.ਮੁੱਲ ਲਈ type ਵਿੱਚ dfns}

    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਵਾਪਸ 'Module({0.name}, {0.dfns})'.format(self)

ਵਰਗ Type(AST):
    ਪਰਿਭਾਸ਼ਾ __init__(self, name, ਮੁੱਲ):
        self.name = name
        self.ਮੁੱਲ = ਮੁੱਲ

    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਵਾਪਸ 'Type({0.name}, {0.value})'.format(self)

ਵਰਗ Constructor(AST):
    ਪਰਿਭਾਸ਼ਾ __init__(self, name, fields=ਕੁਝ_ਨਹੀਂ):
        self.name = name
        self.fields = fields ਜਾਂ []

    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਵਾਪਸ 'Constructor({0.name}, {0.fields})'.format(self)

ਵਰਗ Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

ਵਰਗ ਖੇਤਰ(AST):
    ਪਰਿਭਾਸ਼ਾ __init__(self, type, name=ਕੁਝ_ਨਹੀਂ, quantifiers=ਕੁਝ_ਨਹੀਂ):
        self.type = type
        self.name = name
        self.seq = ਝੂਠ
        self.opt = ਝੂਠ
        self.quantifiers = quantifiers ਜਾਂ []
        ਜੇ ਲੰਬਾਈ(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] ਹੈ Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] ਹੈ Quantifier.OPTIONAL

    ਪਰਿਭਾਸ਼ਾ __str__(self):
        extra = ""
        ਲਈ mod ਵਿੱਚ self.quantifiers:
            ਜੇ mod ਹੈ Quantifier.SEQUENCE:
                extra += "*"
            ਜਾਂਜੇ mod ਹੈ Quantifier.OPTIONAL:
                extra += "?"

        ਵਾਪਸ "{}{} {}".format(self.type, extra, self.name)

    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਜੇ self.quantifiers:
            texts = []
            ਲਈ mod ਵਿੱਚ self.quantifiers:
                ਜੇ mod ਹੈ Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                ਜਾਂਜੇ mod ਹੈ Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        ਨਹੀਂਤਾਂ:
            extra = ""

        ਜੇ self.name ਹੈ ਕੁਝ_ਨਹੀਂ:
            ਵਾਪਸ 'Field({0.type}{1})'.format(self, extra)
        ਨਹੀਂਤਾਂ:
            ਵਾਪਸ 'Field({0.type}, {0.name}{1})'.format(self, extra)

ਵਰਗ ਜੋੜ(AST):
    ਪਰਿਭਾਸ਼ਾ __init__(self, types, ਗੁਣ=ਕੁਝ_ਨਹੀਂ):
        self.types = types
        self.ਗੁਣ = ਗੁਣ ਜਾਂ []

    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਜੇ self.ਗੁਣ:
            ਵਾਪਸ 'Sum({0.types}, {0.attributes})'.format(self)
        ਨਹੀਂਤਾਂ:
            ਵਾਪਸ 'Sum({0.types})'.format(self)

ਵਰਗ ਗੁਣਨਫਲ(AST):
    ਪਰਿਭਾਸ਼ਾ __init__(self, fields, ਗੁਣ=ਕੁਝ_ਨਹੀਂ):
        self.fields = fields
        self.ਗੁਣ = ਗੁਣ ਜਾਂ []

    ਪਰਿਭਾਸ਼ਾ __repr__(self):
        ਜੇ self.ਗੁਣ:
            ਵਾਪਸ 'Product({0.fields}, {0.attributes})'.format(self)
        ਨਹੀਂਤਾਂ:
            ਵਾਪਸ 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

ਵਰਗ Visitor_ਅਧਾਰ(object):
    """Generic tree visitor for ASTs."""
    ਪਰਿਭਾਸ਼ਾ __init__(self):
        self.cache = {}

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ(self, obj, *ਦਲੀਲਾਂ):
        klass = obj.__class__
        meth = self.cache.get(klass)
        ਜੇ meth ਹੈ ਕੁਝ_ਨਹੀਂ:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, ਕੁਝ_ਨਹੀਂ)
            self.cache[klass] = meth
        ਜੇ meth:
            ਕੋਸ਼ਿਸ਼:
                meth(obj, *ਦਲੀਲਾਂ)
            ਅਪਵਾਦ Exception ਇਵ e:
                ਛਾਪੋ("Error visiting %r: %s" % (obj, e))
                ਉਠਾਓ

ਵਰਗ Check(Visitor_ਅਧਾਰ):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    ਪਰਿਭਾਸ਼ਾ __init__(self):
        ਪਰਮ(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ_Module(self, mod):
        ਲਈ dfn ਵਿੱਚ mod.dfns:
            self.ਦੇਖੋ(dfn)

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ_Type(self, type):
        self.ਦੇਖੋ(type.ਮੁੱਲ, str(type.name))

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ_ਜੋੜ(self, ਜੋੜ_2, name):
        ਲਈ t ਵਿੱਚ ਜੋੜ_2.types:
            self.ਦੇਖੋ(t, name)

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ_Constructor(self, cons, name):
        ਕੁੰਜੀ = str(cons.name)
        conflict = self.cons.get(ਕੁੰਜੀ)
        ਜੇ conflict ਹੈ ਕੁਝ_ਨਹੀਂ:
            self.cons[ਕੁੰਜੀ] = name
        ਨਹੀਂਤਾਂ:
            ਛਾਪੋ('Redefinition of constructor {}'.format(ਕੁੰਜੀ))
            ਛਾਪੋ('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        ਲਈ f ਵਿੱਚ cons.fields:
            self.ਦੇਖੋ(f, ਕੁੰਜੀ)

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ_ਖੇਤਰ(self, ਖੇਤਰ_2, name):
        ਕੁੰਜੀ = str(ਖੇਤਰ_2.type)
        l = self.types.setdefault(ਕੁੰਜੀ, [])
        l.append(name)

    ਪਰਿਭਾਸ਼ਾ ਦੇਖੋ_ਗੁਣਨਫਲ(self, prod, name):
        ਲਈ f ਵਿੱਚ prod.fields:
            self.ਦੇਖੋ(f, name)

ਪਰਿਭਾਸ਼ਾ check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.ਦੇਖੋ(mod)

    ਲਈ t ਵਿੱਚ v.types:
        ਜੇ t ਨਹੀਂ ਵਿੱਚ mod.types ਅਤੇ ਨਹੀਂ t ਵਿੱਚ builtin_types:
            v.errors += 1
            ਵਰਤਦਾ = ", ".join(v.types[t])
            ਛਾਪੋ('Undefined type {}, used in {}'.format(t, ਵਰਤਦਾ))
    ਵਾਪਸ ਨਹੀਂ v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

ਪਰਿਭਾਸ਼ਾ parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    ਨਾਲ open(filename, encoding="utf-8") ਇਵ f:
        parser = ASDLParser()
        ਵਾਪਸ parser.parse(f.ਪਠ())

# Types for describing tokens in an ASDL specification.
ਵਰਗ Token_ਕਿਸਮ:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = ਸੀਮਾ(11)

    operator_ਸਾਰਣੀ = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

ਵਰਗ ASDLSyntaxError(Exception):
    ਪਰਿਭਾਸ਼ਾ __init__(self, ਸੁਨੇਹਾ, lineno=ਕੁਝ_ਨਹੀਂ):
        self.ਸੁਨੇਹਾ = ਸੁਨੇਹਾ
        self.lineno = lineno ਜਾਂ '<unknown>'

    ਪਰਿਭਾਸ਼ਾ __str__(self):
        ਵਾਪਸ 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

ਪਰਿਭਾਸ਼ਾ tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    ਲਈ lineno, ਸਤਰ ਵਿੱਚ enumerate(buf.splitlines(), 1):
        ਲਈ m ਵਿੱਚ re.finditer(r'\s*(\w+|--.*|.)', ਸਤਰ.strip()):
            c = m.ਸਮੂਹ(1)
            ਜੇ c[0].isalpha():
                # Some kind of identifier
                ਜੇ c[0].isupper():
                    ਦਿਓ Token(Token_ਕਿਸਮ.ConstructorId, c, lineno)
                ਨਹੀਂਤਾਂ:
                    ਦਿਓ Token(Token_ਕਿਸਮ.TypeId, c, lineno)
            ਜਾਂਜੇ c[:2] == '--':
                # Comment
                ਰੋਕੋ
            ਨਹੀਂਤਾਂ:
                # Operators
                ਕੋਸ਼ਿਸ਼:
                    op_ਕਿਸਮ = Token_ਕਿਸਮ.operator_ਸਾਰਣੀ[c]
                ਅਪਵਾਦ ਕੁੰਜੀ_Error:
                    ਉਠਾਓ ASDLSyntaxError('Invalid operator %s' % c, lineno)
                ਦਿਓ Token(op_ਕਿਸਮ, c, lineno)

ਵਰਗ ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    ਪਰਿਭਾਸ਼ਾ __init__(self):
        self._tokenizer = ਕੁਝ_ਨਹੀਂ
        self.cur_token = ਕੁਝ_ਨਹੀਂ

    ਪਰਿਭਾਸ਼ਾ parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        ਵਾਪਸ self._parse_module()

    ਪਰਿਭਾਸ਼ਾ _parse_module(self):
        ਜੇ self._at_keyword('module'):
            self._advance()
        ਨਹੀਂਤਾਂ:
            ਉਠਾਓ ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.ਮੁੱਲ),
                self.cur_token.lineno)
        name = self.ਮੇਲ(self._id_kinds)
        self.ਮੇਲ(Token_ਕਿਸਮ.LBrace)
        defs = self._parse_definitions()
        self.ਮੇਲ(Token_ਕਿਸਮ.RBrace)
        ਵਾਪਸ Module(name, defs)

    ਪਰਿਭਾਸ਼ਾ _parse_definitions(self):
        defs = []
        ਜਦੋਂ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.TypeId:
            typename = self._advance()
            self.ਮੇਲ(Token_ਕਿਸਮ.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        ਵਾਪਸ defs

    ਪਰਿਭਾਸ਼ਾ _parse_type(self):
        ਜੇ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.LParen:
            # If we see a (, it's a product
            ਵਾਪਸ self.parse_ਗੁਣਨਫਲ()
        ਨਹੀਂਤਾਂ:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.ਮੇਲ(Token_ਕਿਸਮ.ConstructorId),
                                   self._parse_optional_fields())]
            ਜਦੋਂ self.cur_token.ਕਿਸਮ  == Token_ਕਿਸਮ.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.ਮੇਲ(Token_ਕਿਸਮ.ConstructorId),
                                self._parse_optional_fields()))
            ਵਾਪਸ ਜੋੜ(sumlist, self.parse_optional_ਗੁਣ())

    ਪਰਿਭਾਸ਼ਾ parse_ਗੁਣਨਫਲ(self):
        ਵਾਪਸ ਗੁਣਨਫਲ(self._parse_fields(), self.parse_optional_ਗੁਣ())

    ਪਰਿਭਾਸ਼ਾ _parse_fields(self):
        fields = []
        self.ਮੇਲ(Token_ਕਿਸਮ.LParen)
        ਜਦੋਂ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_ਖੇਤਰ_quantifier()
            id = (self._advance() ਜੇ self.cur_token.ਕਿਸਮ ਵਿੱਚ self._id_kinds
                                  ਨਹੀਂਤਾਂ ਕੁਝ_ਨਹੀਂ)
            fields.append(ਖੇਤਰ(typename, id, quantifiers=quantifiers))
            ਜੇ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.RParen:
                ਰੋਕੋ
            ਜਾਂਜੇ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.Comma:
                self._advance()
        self.ਮੇਲ(Token_ਕਿਸਮ.RParen)
        ਵਾਪਸ fields

    ਪਰਿਭਾਸ਼ਾ _parse_optional_fields(self):
        ਜੇ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.LParen:
            ਵਾਪਸ self._parse_fields()
        ਨਹੀਂਤਾਂ:
            ਵਾਪਸ ਕੁਝ_ਨਹੀਂ

    ਪਰਿਭਾਸ਼ਾ parse_optional_ਗੁਣ(self):
        ਜੇ self._at_keyword('attributes'):
            self._advance()
            ਵਾਪਸ self._parse_fields()
        ਨਹੀਂਤਾਂ:
            ਵਾਪਸ ਕੁਝ_ਨਹੀਂ

    ਪਰਿਭਾਸ਼ਾ parse_optional_ਖੇਤਰ_quantifier(self):
        quantifiers = []
        ਜਦੋਂ self.cur_token.ਕਿਸਮ ਵਿੱਚ (Token_ਕਿਸਮ.Asterisk, Token_ਕਿਸਮ.Question):
            ਜੇ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            ਜਾਂਜੇ self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        ਵਾਪਸ quantifiers

    ਪਰਿਭਾਸ਼ਾ _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = ਕੁਝ_ਨਹੀਂ ਜੇ self.cur_token ਹੈ ਕੁਝ_ਨਹੀਂ ਨਹੀਂਤਾਂ self.cur_token.ਮੁੱਲ
        ਕੋਸ਼ਿਸ਼:
            self.cur_token = ਅਗਲਾ(self._tokenizer)
        ਅਪਵਾਦ StopIteration:
            self.cur_token = ਕੁਝ_ਨਹੀਂ
        ਵਾਪਸ cur_val

    _id_kinds = (Token_ਕਿਸਮ.ConstructorId, Token_ਕਿਸਮ.TypeId)

    ਪਰਿਭਾਸ਼ਾ ਮੇਲ(self, ਕਿਸਮ):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        ਜੇ (isinstance(ਕਿਸਮ, tuple) ਅਤੇ self.cur_token.ਕਿਸਮ ਵਿੱਚ ਕਿਸਮ ਜਾਂ
            self.cur_token.ਕਿਸਮ == ਕਿਸਮ
            ):
            ਮੁੱਲ = self.cur_token.ਮੁੱਲ
            self._advance()
            ਵਾਪਸ ਮੁੱਲ
        ਨਹੀਂਤਾਂ:
            ਉਠਾਓ ASDLSyntaxError(
                'Unmatched {} (found {})'.format(ਕਿਸਮ, self.cur_token.ਕਿਸਮ),
                self.cur_token.lineno)

    ਪਰਿਭਾਸ਼ਾ _at_keyword(self, keyword):
        ਵਾਪਸ (self.cur_token.ਕਿਸਮ == Token_ਕਿਸਮ.TypeId ਅਤੇ
                self.cur_token.ਮੁੱਲ == keyword)
