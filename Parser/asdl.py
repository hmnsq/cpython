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
તઃ collections આયાત namedtuple
આયાત enum
આયાત re

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

વર્ગ AST:
    def __repr__(self):
        ઉઠાવો NotImplementedError

વર્ગ Module(AST):
    def __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.કિંમત માટે type માં dfns}

    def __repr__(self):
        પાછા 'Module({0.name}, {0.dfns})'.format(self)

વર્ગ Type(AST):
    def __init__(self, name, કિંમત):
        self.name = name
        self.કિંમત = કિંમત

    def __repr__(self):
        પાછા 'Type({0.name}, {0.value})'.format(self)

વર્ગ Constructor(AST):
    def __init__(self, name, fields=None):
        self.name = name
        self.fields = fields અથવા []

    def __repr__(self):
        પાછા 'Constructor({0.name}, {0.fields})'.format(self)

વર્ગ Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

વર્ગ Field(AST):
    def __init__(self, type, name=None, quantifiers=None):
        self.type = type
        self.name = name
        self.seq = ખોટું
        self.opt = ખોટું
        self.quantifiers = quantifiers અથવા []
        જો len(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] is Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] is Quantifier.OPTIONAL

    def __str__(self):
        extra = ""
        માટે mod માં self.quantifiers:
            જો mod is Quantifier.SEQUENCE:
                extra += "*"
            અથવાજો mod is Quantifier.OPTIONAL:
                extra += "?"

        પાછા "{}{} {}".format(self.type, extra, self.name)

    def __repr__(self):
        જો self.quantifiers:
            texts = []
            માટે mod માં self.quantifiers:
                જો mod is Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                અથવાજો mod is Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        નહીંતર:
            extra = ""

        જો self.name is None:
            પાછા 'Field({0.type}{1})'.format(self, extra)
        નહીંતર:
            પાછા 'Field({0.type}, {0.name}{1})'.format(self, extra)

વર્ગ Sum(AST):
    def __init__(self, types, attributes=None):
        self.types = types
        self.attributes = attributes અથવા []

    def __repr__(self):
        જો self.attributes:
            પાછા 'Sum({0.types}, {0.attributes})'.format(self)
        નહીંતર:
            પાછા 'Sum({0.types})'.format(self)

વર્ગ Product(AST):
    def __init__(self, fields, attributes=None):
        self.fields = fields
        self.attributes = attributes અથવા []

    def __repr__(self):
        જો self.attributes:
            પાછા 'Product({0.fields}, {0.attributes})'.format(self)
        નહીંતર:
            પાછા 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

વર્ગ VisitorBase(object):
    """Generic tree visitor for ASTs."""
    def __init__(self):
        self.cache = {}

    def visit(self, obj, *args):
        klass = obj.__class__
        meth = self.cache.get(klass)
        જો meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, None)
            self.cache[klass] = meth
        જો meth:
            પ્રયાસ:
                meth(obj, *args)
            અપવાદ Exception ઇવ e:
                print("Error visiting %r: %s" % (obj, e))
                ઉઠાવો

વર્ગ Check(VisitorBase):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    def __init__(self):
        પરમ(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    def visitModule(self, mod):
        માટે dfn માં mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.કિંમત, str(type.name))

    def visitSum(self, sum, name):
        માટે t માં sum.types:
            self.visit(t, name)

    def visitConstructor(self, cons, name):
        ચાવી = str(cons.name)
        conflict = self.cons.get(ચાવી)
        જો conflict is None:
            self.cons[ચાવી] = name
        નહીંતર:
            print('Redefinition of constructor {}'.format(ચાવી))
            print('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        માટે f માં cons.fields:
            self.visit(f, ચાવી)

    def visitField(self, field, name):
        ચાવી = str(field.type)
        l = self.types.setdefault(ચાવી, [])
        l.append(name)

    def visitProduct(self, prod, name):
        માટે f માં prod.fields:
            self.visit(f, name)

def check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.visit(mod)

    માટે t માં v.types:
        જો t નહીં માં mod.types અને નહીં t માં builtin_types:
            v.errors += 1
            uses = ", ".join(v.types[t])
            print('Undefined type {}, used in {}'.format(t, uses))
    પાછા નહીં v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

def parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    with open(filename, encoding="utf-8") ઇવ f:
        parser = ASDLParser()
        પાછા parser.parse(f.પઠ())

# Types for describing tokens in an ASDL specification.
વર્ગ TokenKind:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = range(11)

    operator_table = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

વર્ગ ASDLSyntaxError(Exception):
    def __init__(self, સંદેશ, lineno=None):
        self.સંદેશ = સંદેશ
        self.lineno = lineno અથવા '<unknown>'

    def __str__(self):
        પાછા 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

def tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    માટે lineno, લીટી માં enumerate(buf.splitlines(), 1):
        માટે m માં re.finditer(r'\s*(\w+|--.*|.)', લીટી.strip()):
            c = m.group(1)
            જો c[0].isalpha():
                # Some kind of identifier
                જો c[0].isupper():
                    આપો Token(TokenKind.ConstructorId, c, lineno)
                નહીંતર:
                    આપો Token(TokenKind.TypeId, c, lineno)
            અથવાજો c[:2] == '--':
                # Comment
                તોડો
            નહીંતર:
                # Operators
                પ્રયાસ:
                    op_kind = TokenKind.operator_table[c]
                અપવાદ ચાવી_Error:
                    ઉઠાવો ASDLSyntaxError('Invalid operator %s' % c, lineno)
                આપો Token(op_kind, c, lineno)

વર્ગ ASDLParser:
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
        પાછા self._parse_module()

    def _parse_module(self):
        જો self._at_keyword('module'):
            self._advance()
        નહીંતર:
            ઉઠાવો ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.કિંમત),
                self.cur_token.lineno)
        name = self._match(self._id_kinds)
        self._match(TokenKind.LBrace)
        defs = self._parse_definitions()
        self._match(TokenKind.RBrace)
        પાછા Module(name, defs)

    def _parse_definitions(self):
        defs = []
        જ્યારે self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            self._match(TokenKind.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        પાછા defs

    def _parse_type(self):
        જો self.cur_token.kind == TokenKind.LParen:
            # If we see a (, it's a product
            પાછા self._parse_product()
        નહીંતર:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self._match(TokenKind.ConstructorId),
                                   self._parse_optional_fields())]
            જ્યારે self.cur_token.kind  == TokenKind.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self._match(TokenKind.ConstructorId),
                                self._parse_optional_fields()))
            પાછા Sum(sumlist, self._parse_optional_attributes())

    def _parse_product(self):
        પાછા Product(self._parse_fields(), self._parse_optional_attributes())

    def _parse_fields(self):
        fields = []
        self._match(TokenKind.LParen)
        જ્યારે self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            quantifiers = self._parse_optional_field_quantifier()
            id = (self._advance() જો self.cur_token.kind માં self._id_kinds
                                  નહીંતર None)
            fields.append(Field(typename, id, quantifiers=quantifiers))
            જો self.cur_token.kind == TokenKind.RParen:
                તોડો
            અથવાજો self.cur_token.kind == TokenKind.Comma:
                self._advance()
        self._match(TokenKind.RParen)
        પાછા fields

    def _parse_optional_fields(self):
        જો self.cur_token.kind == TokenKind.LParen:
            પાછા self._parse_fields()
        નહીંતર:
            પાછા None

    def _parse_optional_attributes(self):
        જો self._at_keyword('attributes'):
            self._advance()
            પાછા self._parse_fields()
        નહીંતર:
            પાછા None

    def _parse_optional_field_quantifier(self):
        quantifiers = []
        જ્યારે self.cur_token.kind માં (TokenKind.Asterisk, TokenKind.Question):
            જો self.cur_token.kind == TokenKind.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            અથવાજો self.cur_token.kind == TokenKind.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        પાછા quantifiers

    def _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = None જો self.cur_token is None નહીંતર self.cur_token.કિંમત
        પ્રયાસ:
            self.cur_token = next(self._tokenizer)
        અપવાદ StopIteration:
            self.cur_token = None
        પાછા cur_val

    _id_kinds = (TokenKind.ConstructorId, TokenKind.TypeId)

    def _match(self, kind):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        જો (isinstance(kind, tuple) અને self.cur_token.kind માં kind અથવા
            self.cur_token.kind == kind
            ):
            કિંમત = self.cur_token.કિંમત
            self._advance()
            પાછા કિંમત
        નહીંતર:
            ઉઠાવો ASDLSyntaxError(
                'Unmatched {} (found {})'.format(kind, self.cur_token.kind),
                self.cur_token.lineno)

    def _at_keyword(self, keyword):
        પાછા (self.cur_token.kind == TokenKind.TypeId અને
                self.cur_token.કિંમત == keyword)
