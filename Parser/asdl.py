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
ta_h collections import namedtuple
import enum
import re

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

class AST:
    def __repr__(self):
        raise NotImplementedError

class Module(AST):
    def __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.value དོན་ལུ type anta_h dfns}

    def __repr__(self):
        ལོག 'Module({0.name}, {0.dfns})'.format(self)

class Type(AST):
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def __repr__(self):
        ལོག 'Type({0.name}, {0.value})'.format(self)

class Constructor(AST):
    def __init__(self, name, fields=None):
        self.name = name
        self.fields = fields or []

    def __repr__(self):
        ལོག 'Constructor({0.name}, {0.fields})'.format(self)

class Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

class Field(AST):
    def __init__(self, type, name=None, quantifiers=None):
        self.type = type
        self.name = name
        self.seq = False
        self.opt = False
        self.quantifiers = quantifiers or []
        གལ་སྲིད len(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] is Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] is Quantifier.OPTIONAL

    def __str__(self):
        extra = ""
        དོན་ལུ mod anta_h self.quantifiers:
            གལ་སྲིད mod is Quantifier.SEQUENCE:
                extra += "*"
            elif mod is Quantifier.OPTIONAL:
                extra += "?"

        ལོག "{}{} {}".format(self.type, extra, self.name)

    def __repr__(self):
        གལ་སྲིད self.quantifiers:
            texts = []
            དོན་ལུ mod anta_h self.quantifiers:
                གལ་སྲིད mod is Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                elif mod is Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        དེ་མིན:
            extra = ""

        གལ་སྲིད self.name is None:
            ལོག 'Field({0.type}{1})'.format(self, extra)
        དེ་མིན:
            ལོག 'Field({0.type}, {0.name}{1})'.format(self, extra)

class Sum(AST):
    def __init__(self, types, attributes=None):
        self.types = types
        self.attributes = attributes or []

    def __repr__(self):
        གལ་སྲིད self.attributes:
            ལོག 'Sum({0.types}, {0.attributes})'.format(self)
        དེ་མིན:
            ལོག 'Sum({0.types})'.format(self)

class Product(AST):
    def __init__(self, fields, attributes=None):
        self.fields = fields
        self.attributes = attributes or []

    def __repr__(self):
        གལ་སྲིད self.attributes:
            ལོག 'Product({0.fields}, {0.attributes})'.format(self)
        དེ་མིན:
            ལོག 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

class VisitorBase(object):
    """Generic tree visitor for ASTs."""
    def __init__(self):
        self.cache = {}

    def visit(self, obj, *args):
        klass = obj.__class__
        meth = self.cache.get(klass)
        གལ་སྲིད meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, None)
            self.cache[klass] = meth
        གལ་སྲིད meth:
            try:
                meth(obj, *args)
            except Exception as e:
                print("Error visiting %r: %s" % (obj, e))
                raise

class Check(VisitorBase):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    def __init__(self):
        super(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    def visitModule(self, mod):
        དོན་ལུ dfn anta_h mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, str(type.name))

    def visitSum(self, sum, name):
        དོན་ལུ t anta_h sum.types:
            self.visit(t, name)

    def visitConstructor(self, cons, name):
        key = str(cons.name)
        conflict = self.cons.get(key)
        གལ་སྲིད conflict is None:
            self.cons[key] = name
        དེ་མིན:
            print('Redefinition of constructor {}'.format(key))
            print('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        དོན་ལུ f anta_h cons.fields:
            self.visit(f, key)

    def visitField(self, field, name):
        key = str(field.type)
        l = self.types.setdefault(key, [])
        l.append(name)

    def visitProduct(self, prod, name):
        དོན་ལུ f anta_h prod.fields:
            self.visit(f, name)

def check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.visit(mod)

    དོན་ལུ t anta_h v.types:
        གལ་སྲིད t na anta_h mod.types ca na t anta_h builtin_types:
            v.errors += 1
            uses = ", ".join(v.types[t])
            print('Undefined type {}, used in {}'.format(t, uses))
    ལོག na v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

def parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    with open(filename, encoding="utf-8") as f:
        parser = ASDLParser()
        ལོག parser.parse(f.read())

# Types for describing tokens in an ASDL specification.
class TokenKind:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = range(11)

    operator_table = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

class ASDLSyntaxError(Exception):
    def __init__(self, msg, lineno=None):
        self.msg = msg
        self.lineno = lineno or '<unknown>'

    def __str__(self):
        ལོག 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

def tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    དོན་ལུ lineno, line anta_h enumerate(buf.splitlines(), 1):
        དོན་ལུ m anta_h re.finditer(r'\s*(\w+|--.*|.)', line.strip()):
            c = m.group(1)
            གལ་སྲིད c[0].isalpha():
                # Some kind of identifier
                གལ་སྲིད c[0].isupper():
                    yield Token(TokenKind.ConstructorId, c, lineno)
                དེ་མིན:
                    yield Token(TokenKind.TypeId, c, lineno)
            elif c[:2] == '--':
                # Comment
                bha_nga
            དེ་མིན:
                # Operators
                try:
                    op_kind = TokenKind.operator_table[c]
                except KeyError:
                    raise ASDLSyntaxError('Invalid operator %s' % c, lineno)
                yield Token(op_kind, c, lineno)

class ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    def __init__(self):
        self._tokenizer = None
        self.cur_token = None

    def parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        ལོག self._parse_module()

    def _parse_module(self):
        གལ་སྲིད self._at_keyword('module'):
            self._advance()
        དེ་མིན:
            raise ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.value),
                self.cur_token.lineno)
        name = self._match(self._id_kinds)
        self._match(TokenKind.LBrace)
        defs = self._parse_definitions()
        self._match(TokenKind.RBrace)
        ལོག Module(name, defs)

    def _parse_definitions(self):
        defs = []
        སྐབས self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            self._match(TokenKind.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        ལོག defs

    def _parse_type(self):
        གལ་སྲིད self.cur_token.kind == TokenKind.LParen:
            # If we see a (, it's a product
            ལོག self._parse_product()
        དེ་མིན:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self._match(TokenKind.ConstructorId),
                                   self._parse_optional_fields())]
            སྐབས self.cur_token.kind  == TokenKind.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self._match(TokenKind.ConstructorId),
                                self._parse_optional_fields()))
            ལོག Sum(sumlist, self._parse_optional_attributes())

    def _parse_product(self):
        ལོག Product(self._parse_fields(), self._parse_optional_attributes())

    def _parse_fields(self):
        fields = []
        self._match(TokenKind.LParen)
        སྐབས self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            quantifiers = self._parse_optional_field_quantifier()
            id = (self._advance() གལ་སྲིད self.cur_token.kind anta_h self._id_kinds
                                  དེ་མིན None)
            fields.append(Field(typename, id, quantifiers=quantifiers))
            གལ་སྲིད self.cur_token.kind == TokenKind.RParen:
                bha_nga
            elif self.cur_token.kind == TokenKind.Comma:
                self._advance()
        self._match(TokenKind.RParen)
        ལོག fields

    def _parse_optional_fields(self):
        གལ་སྲིད self.cur_token.kind == TokenKind.LParen:
            ལོག self._parse_fields()
        དེ་མིན:
            ལོག None

    def _parse_optional_attributes(self):
        གལ་སྲིད self._at_keyword('attributes'):
            self._advance()
            ལོག self._parse_fields()
        དེ་མིན:
            ལོག None

    def _parse_optional_field_quantifier(self):
        quantifiers = []
        སྐབས self.cur_token.kind anta_h (TokenKind.Asterisk, TokenKind.Question):
            གལ་སྲིད self.cur_token.kind == TokenKind.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            elif self.cur_token.kind == TokenKind.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        ལོག quantifiers

    def _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = None གལ་སྲིད self.cur_token is None དེ་མིན self.cur_token.value
        try:
            self.cur_token = next(self._tokenizer)
        except StopIteration:
            self.cur_token = None
        ལོག cur_val

    _id_kinds = (TokenKind.ConstructorId, TokenKind.TypeId)

    def _match(self, kind):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        གལ་སྲིད (isinstance(kind, tuple) ca self.cur_token.kind anta_h kind or
            self.cur_token.kind == kind
            ):
            value = self.cur_token.value
            self._advance()
            ལོག value
        དེ་མིན:
            raise ASDLSyntaxError(
                'Unmatched {} (found {})'.format(kind, self.cur_token.kind),
                self.cur_token.lineno)

    def _at_keyword(self, keyword):
        ལོག (self.cur_token.kind == TokenKind.TypeId ca
                self.cur_token.value == keyword)
