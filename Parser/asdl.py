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
ತಃ collections ಆಮದು namedtuple
ಆಮದು enum
ಆಮದು re

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

ವರ್ಗ AST:
    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಎತ್ತು NotImplementedError

ವರ್ಗ Module(AST):
    ವ್ಯಾಖ್ಯಾನ __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.ಮೌಲ್ಯ ಗಾಗಿ type ಒಳಗೆ dfns}

    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಹಿಂತಿರುಗಿ 'Module({0.name}, {0.dfns})'.format(self)

ವರ್ಗ Type(AST):
    ವ್ಯಾಖ್ಯಾನ __init__(self, name, ಮೌಲ್ಯ):
        self.name = name
        self.ಮೌಲ್ಯ = ಮೌಲ್ಯ

    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಹಿಂತಿರುಗಿ 'Type({0.name}, {0.value})'.format(self)

ವರ್ಗ Constructor(AST):
    ವ್ಯಾಖ್ಯಾನ __init__(self, name, fields=ಏನೂಇಲ್ಲ):
        self.name = name
        self.fields = fields ಅಥವಾ []

    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಹಿಂತಿರುಗಿ 'Constructor({0.name}, {0.fields})'.format(self)

ವರ್ಗ Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

ವರ್ಗ ಕ್ಷೇತ್ರ(AST):
    ವ್ಯಾಖ್ಯಾನ __init__(self, type, name=ಏನೂಇಲ್ಲ, quantifiers=ಏನೂಇಲ್ಲ):
        self.type = type
        self.name = name
        self.seq = ಸುಳ್ಳು
        self.opt = ಸುಳ್ಳು
        self.quantifiers = quantifiers ಅಥವಾ []
        ಆದರೆ ಉದ್ದ(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] ಆಗಿರುವ Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] ಆಗಿರುವ Quantifier.OPTIONAL

    ವ್ಯಾಖ್ಯಾನ __str__(self):
        extra = ""
        ಗಾಗಿ mod ಒಳಗೆ self.quantifiers:
            ಆದರೆ mod ಆಗಿರುವ Quantifier.SEQUENCE:
                extra += "*"
            ಅಥವಾಆದರೆ mod ಆಗಿರುವ Quantifier.OPTIONAL:
                extra += "?"

        ಹಿಂತಿರುಗಿ "{}{} {}".format(self.type, extra, self.name)

    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಆದರೆ self.quantifiers:
            texts = []
            ಗಾಗಿ mod ಒಳಗೆ self.quantifiers:
                ಆದರೆ mod ಆಗಿರುವ Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                ಅಥವಾಆದರೆ mod ಆಗಿರುವ Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        ಇಲ್ಲದಿದ್ದರೆ:
            extra = ""

        ಆದರೆ self.name ಆಗಿರುವ ಏನೂಇಲ್ಲ:
            ಹಿಂತಿರುಗಿ 'Field({0.type}{1})'.format(self, extra)
        ಇಲ್ಲದಿದ್ದರೆ:
            ಹಿಂತಿರುಗಿ 'Field({0.type}, {0.name}{1})'.format(self, extra)

ವರ್ಗ ಮೊತ್ತ(AST):
    ವ್ಯಾಖ್ಯಾನ __init__(self, types, ಗುಣಲಕ್ಷಣಗಳು=ಏನೂಇಲ್ಲ):
        self.types = types
        self.ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು ಅಥವಾ []

    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಆದರೆ self.ಗುಣಲಕ್ಷಣಗಳು:
            ಹಿಂತಿರುಗಿ 'Sum({0.types}, {0.attributes})'.format(self)
        ಇಲ್ಲದಿದ್ದರೆ:
            ಹಿಂತಿರುಗಿ 'Sum({0.types})'.format(self)

ವರ್ಗ ಗುಣಲಬ್ಧ(AST):
    ವ್ಯಾಖ್ಯಾನ __init__(self, fields, ಗುಣಲಕ್ಷಣಗಳು=ಏನೂಇಲ್ಲ):
        self.fields = fields
        self.ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು ಅಥವಾ []

    ವ್ಯಾಖ್ಯಾನ __repr__(self):
        ಆದರೆ self.ಗುಣಲಕ್ಷಣಗಳು:
            ಹಿಂತಿರುಗಿ 'Product({0.fields}, {0.attributes})'.format(self)
        ಇಲ್ಲದಿದ್ದರೆ:
            ಹಿಂತಿರುಗಿ 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

ವರ್ಗ Visitor_ತಳ(object):
    """Generic tree visitor for ASTs."""
    ವ್ಯಾಖ್ಯಾನ __init__(self):
        self.cache = {}

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ(self, obj, *ವಾದಗಳು):
        klass = obj.__class__
        meth = self.cache.get(klass)
        ಆದರೆ meth ಆಗಿರುವ ಏನೂಇಲ್ಲ:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, ಏನೂಇಲ್ಲ)
            self.cache[klass] = meth
        ಆದರೆ meth:
            ಪ್ರಯತ್ನಿಸು:
                meth(obj, *ವಾದಗಳು)
            ಅಪವಾದ Exception ಇವ e:
                ಮುದ್ರಿಸು("Error visiting %r: %s" % (obj, e))
                ಎತ್ತು

ವರ್ಗ Check(Visitor_ತಳ):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    ವ್ಯಾಖ್ಯಾನ __init__(self):
        ಪರ(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ_Module(self, mod):
        ಗಾಗಿ dfn ಒಳಗೆ mod.dfns:
            self.ಭೇಟಿ(dfn)

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ_Type(self, type):
        self.ಭೇಟಿ(type.ಮೌಲ್ಯ, str(type.name))

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ_ಮೊತ್ತ(self, ಮೊತ್ತ_2, name):
        ಗಾಗಿ t ಒಳಗೆ ಮೊತ್ತ_2.types:
            self.ಭೇಟಿ(t, name)

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ_Constructor(self, cons, name):
        ಕೀಲಿ = str(cons.name)
        conflict = self.cons.get(ಕೀಲಿ)
        ಆದರೆ conflict ಆಗಿರುವ ಏನೂಇಲ್ಲ:
            self.cons[ಕೀಲಿ] = name
        ಇಲ್ಲದಿದ್ದರೆ:
            ಮುದ್ರಿಸು('Redefinition of constructor {}'.format(ಕೀಲಿ))
            ಮುದ್ರಿಸು('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        ಗಾಗಿ f ಒಳಗೆ cons.fields:
            self.ಭೇಟಿ(f, ಕೀಲಿ)

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ_ಕ್ಷೇತ್ರ(self, ಕ್ಷೇತ್ರ_2, name):
        ಕೀಲಿ = str(ಕ್ಷೇತ್ರ_2.type)
        l = self.types.setdefault(ಕೀಲಿ, [])
        l.append(name)

    ವ್ಯಾಖ್ಯಾನ ಭೇಟಿ_ಗುಣಲಬ್ಧ(self, prod, name):
        ಗಾಗಿ f ಒಳಗೆ prod.fields:
            self.ಭೇಟಿ(f, name)

ವ್ಯಾಖ್ಯಾನ check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.ಭೇಟಿ(mod)

    ಗಾಗಿ t ಒಳಗೆ v.types:
        ಆದರೆ t ಅಲ್ಲ ಒಳಗೆ mod.types ಮತ್ತು ಅಲ್ಲ t ಒಳಗೆ builtin_types:
            v.errors += 1
            ಬಳಸುತ್ತದೆ = ", ".join(v.types[t])
            ಮುದ್ರಿಸು('Undefined type {}, used in {}'.format(t, ಬಳಸುತ್ತದೆ))
    ಹಿಂತಿರುಗಿ ಅಲ್ಲ v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

ವ್ಯಾಖ್ಯಾನ parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    ಜೊತೆ open(filename, encoding="utf-8") ಇವ f:
        parser = ASDLParser()
        ಹಿಂತಿರುಗಿ parser.parse(f.ಪಠ())

# Types for describing tokens in an ASDL specification.
ವರ್ಗ Token_ಬಗೆ:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = ಶ್ರೇಣಿ(11)

    operator_ಕೋಷ್ಟಕ = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

ವರ್ಗ ASDLSyntaxError(Exception):
    ವ್ಯಾಖ್ಯಾನ __init__(self, ಸಂದೇಶ, lineno=ಏನೂಇಲ್ಲ):
        self.ಸಂದೇಶ = ಸಂದೇಶ
        self.lineno = lineno ಅಥವಾ '<unknown>'

    ವ್ಯಾಖ್ಯಾನ __str__(self):
        ಹಿಂತಿರುಗಿ 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

ವ್ಯಾಖ್ಯಾನ tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    ಗಾಗಿ lineno, ಸಾಲು ಒಳಗೆ enumerate(buf.splitlines(), 1):
        ಗಾಗಿ m ಒಳಗೆ re.finditer(r'\s*(\w+|--.*|.)', ಸಾಲು.strip()):
            c = m.ಗುಂಪು(1)
            ಆದರೆ c[0].isalpha():
                # Some kind of identifier
                ಆದರೆ c[0].isupper():
                    ನೀಡು Token(Token_ಬಗೆ.ConstructorId, c, lineno)
                ಇಲ್ಲದಿದ್ದರೆ:
                    ನೀಡು Token(Token_ಬಗೆ.TypeId, c, lineno)
            ಅಥವಾಆದರೆ c[:2] == '--':
                # Comment
                ನಿಲ್ಲಿಸು
            ಇಲ್ಲದಿದ್ದರೆ:
                # Operators
                ಪ್ರಯತ್ನಿಸು:
                    op_ಬಗೆ = Token_ಬಗೆ.operator_ಕೋಷ್ಟಕ[c]
                ಅಪವಾದ ಕೀಲಿ_Error:
                    ಎತ್ತು ASDLSyntaxError('Invalid operator %s' % c, lineno)
                ನೀಡು Token(op_ಬಗೆ, c, lineno)

ವರ್ಗ ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    ವ್ಯಾಖ್ಯಾನ __init__(self):
        self._tokenizer = ಏನೂಇಲ್ಲ
        self.cur_token = ಏನೂಇಲ್ಲ

    ವ್ಯಾಖ್ಯಾನ parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        ಹಿಂತಿರುಗಿ self._parse_module()

    ವ್ಯಾಖ್ಯಾನ _parse_module(self):
        ಆದರೆ self._at_keyword('module'):
            self._advance()
        ಇಲ್ಲದಿದ್ದರೆ:
            ಎತ್ತು ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.ಮೌಲ್ಯ),
                self.cur_token.lineno)
        name = self.ಹೊಂದಿಕೆ(self._id_kinds)
        self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.LBrace)
        defs = self._parse_definitions()
        self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.RBrace)
        ಹಿಂತಿರುಗಿ Module(name, defs)

    ವ್ಯಾಖ್ಯಾನ _parse_definitions(self):
        defs = []
        ಆಗ self.cur_token.ಬಗೆ == Token_ಬಗೆ.TypeId:
            typename = self._advance()
            self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        ಹಿಂತಿರುಗಿ defs

    ವ್ಯಾಖ್ಯಾನ _parse_type(self):
        ಆದರೆ self.cur_token.ಬಗೆ == Token_ಬಗೆ.LParen:
            # If we see a (, it's a product
            ಹಿಂತಿರುಗಿ self.parse_ಗುಣಲಬ್ಧ()
        ಇಲ್ಲದಿದ್ದರೆ:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.ConstructorId),
                                   self._parse_optional_fields())]
            ಆಗ self.cur_token.ಬಗೆ  == Token_ಬಗೆ.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.ConstructorId),
                                self._parse_optional_fields()))
            ಹಿಂತಿರುಗಿ ಮೊತ್ತ(sumlist, self.parse_optional_ಗುಣಲಕ್ಷಣಗಳು())

    ವ್ಯಾಖ್ಯಾನ parse_ಗುಣಲಬ್ಧ(self):
        ಹಿಂತಿರುಗಿ ಗುಣಲಬ್ಧ(self._parse_fields(), self.parse_optional_ಗುಣಲಕ್ಷಣಗಳು())

    ವ್ಯಾಖ್ಯಾನ _parse_fields(self):
        fields = []
        self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.LParen)
        ಆಗ self.cur_token.ಬಗೆ == Token_ಬಗೆ.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_ಕ್ಷೇತ್ರ_quantifier()
            id = (self._advance() ಆದರೆ self.cur_token.ಬಗೆ ಒಳಗೆ self._id_kinds
                                  ಇಲ್ಲದಿದ್ದರೆ ಏನೂಇಲ್ಲ)
            fields.append(ಕ್ಷೇತ್ರ(typename, id, quantifiers=quantifiers))
            ಆದರೆ self.cur_token.ಬಗೆ == Token_ಬಗೆ.RParen:
                ನಿಲ್ಲಿಸು
            ಅಥವಾಆದರೆ self.cur_token.ಬಗೆ == Token_ಬಗೆ.Comma:
                self._advance()
        self.ಹೊಂದಿಕೆ(Token_ಬಗೆ.RParen)
        ಹಿಂತಿರುಗಿ fields

    ವ್ಯಾಖ್ಯಾನ _parse_optional_fields(self):
        ಆದರೆ self.cur_token.ಬಗೆ == Token_ಬಗೆ.LParen:
            ಹಿಂತಿರುಗಿ self._parse_fields()
        ಇಲ್ಲದಿದ್ದರೆ:
            ಹಿಂತಿರುಗಿ ಏನೂಇಲ್ಲ

    ವ್ಯಾಖ್ಯಾನ parse_optional_ಗುಣಲಕ್ಷಣಗಳು(self):
        ಆದರೆ self._at_keyword('attributes'):
            self._advance()
            ಹಿಂತಿರುಗಿ self._parse_fields()
        ಇಲ್ಲದಿದ್ದರೆ:
            ಹಿಂತಿರುಗಿ ಏನೂಇಲ್ಲ

    ವ್ಯಾಖ್ಯಾನ parse_optional_ಕ್ಷೇತ್ರ_quantifier(self):
        quantifiers = []
        ಆಗ self.cur_token.ಬಗೆ ಒಳಗೆ (Token_ಬಗೆ.Asterisk, Token_ಬಗೆ.Question):
            ಆದರೆ self.cur_token.ಬಗೆ == Token_ಬಗೆ.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            ಅಥವಾಆದರೆ self.cur_token.ಬಗೆ == Token_ಬಗೆ.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        ಹಿಂತಿರುಗಿ quantifiers

    ವ್ಯಾಖ್ಯಾನ _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = ಏನೂಇಲ್ಲ ಆದರೆ self.cur_token ಆಗಿರುವ ಏನೂಇಲ್ಲ ಇಲ್ಲದಿದ್ದರೆ self.cur_token.ಮೌಲ್ಯ
        ಪ್ರಯತ್ನಿಸು:
            self.cur_token = ಮುಂದಿನ(self._tokenizer)
        ಅಪವಾದ StopIteration:
            self.cur_token = ಏನೂಇಲ್ಲ
        ಹಿಂತಿರುಗಿ cur_val

    _id_kinds = (Token_ಬಗೆ.ConstructorId, Token_ಬಗೆ.TypeId)

    ವ್ಯಾಖ್ಯಾನ ಹೊಂದಿಕೆ(self, ಬಗೆ):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        ಆದರೆ (isinstance(ಬಗೆ, tuple) ಮತ್ತು self.cur_token.ಬಗೆ ಒಳಗೆ ಬಗೆ ಅಥವಾ
            self.cur_token.ಬಗೆ == ಬಗೆ
            ):
            ಮೌಲ್ಯ = self.cur_token.ಮೌಲ್ಯ
            self._advance()
            ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ
        ಇಲ್ಲದಿದ್ದರೆ:
            ಎತ್ತು ASDLSyntaxError(
                'Unmatched {} (found {})'.format(ಬಗೆ, self.cur_token.ಬಗೆ),
                self.cur_token.lineno)

    ವ್ಯಾಖ್ಯಾನ _at_keyword(self, keyword):
        ಹಿಂತಿರುಗಿ (self.cur_token.ಬಗೆ == Token_ಬಗೆ.TypeId ಮತ್ತು
                self.cur_token.ಮೌಲ್ಯ == keyword)
