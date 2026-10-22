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
از collections واردکن namedtuple
واردکن enum
واردکن re

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

رده AST:
    def __repr__(self):
        بالاببر NotImplementedError

رده Module(AST):
    def __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.مقدار برای type در dfns}

    def __repr__(self):
        بازگشت 'Module({0.name}, {0.dfns})'.format(self)

رده Type(AST):
    def __init__(self, name, مقدار):
        self.name = name
        self.مقدار = مقدار

    def __repr__(self):
        بازگشت 'Type({0.name}, {0.value})'.format(self)

رده Constructor(AST):
    def __init__(self, name, fields=None):
        self.name = name
        self.fields = fields یا []

    def __repr__(self):
        بازگشت 'Constructor({0.name}, {0.fields})'.format(self)

رده Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

رده Field(AST):
    def __init__(self, type, name=None, quantifiers=None):
        self.type = type
        self.name = name
        self.seq = نادرست
        self.opt = نادرست
        self.quantifiers = quantifiers یا []
        اگر len(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] is Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] is Quantifier.OPTIONAL

    def __str__(self):
        extra = ""
        برای mod در self.quantifiers:
            اگر mod is Quantifier.SEQUENCE:
                extra += "*"
            وگرنهاگر mod is Quantifier.OPTIONAL:
                extra += "?"

        بازگشت "{}{} {}".format(self.type, extra, self.name)

    def __repr__(self):
        اگر self.quantifiers:
            texts = []
            برای mod در self.quantifiers:
                اگر mod is Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                وگرنهاگر mod is Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        وگرنه:
            extra = ""

        اگر self.name is None:
            بازگشت 'Field({0.type}{1})'.format(self, extra)
        وگرنه:
            بازگشت 'Field({0.type}, {0.name}{1})'.format(self, extra)

رده Sum(AST):
    def __init__(self, types, attributes=None):
        self.types = types
        self.attributes = attributes یا []

    def __repr__(self):
        اگر self.attributes:
            بازگشت 'Sum({0.types}, {0.attributes})'.format(self)
        وگرنه:
            بازگشت 'Sum({0.types})'.format(self)

رده Product(AST):
    def __init__(self, fields, attributes=None):
        self.fields = fields
        self.attributes = attributes یا []

    def __repr__(self):
        اگر self.attributes:
            بازگشت 'Product({0.fields}, {0.attributes})'.format(self)
        وگرنه:
            بازگشت 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

رده VisitorBase(object):
    """Generic tree visitor for ASTs."""
    def __init__(self):
        self.cache = {}

    def visit(self, obj, *args):
        klass = obj.__class__
        meth = self.cache.get(klass)
        اگر meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, None)
            self.cache[klass] = meth
        اگر meth:
            تلاش:
                meth(obj, *args)
            استثنا Exception بهعنوان e:
                print("Error visiting %r: %s" % (obj, e))
                بالاببر

رده Check(VisitorBase):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    def __init__(self):
        ابر(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    def visitModule(self, mod):
        برای dfn در mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.مقدار, str(type.name))

    def visitSum(self, sum, name):
        برای t در sum.types:
            self.visit(t, name)

    def visitConstructor(self, cons, name):
        کلید = str(cons.name)
        conflict = self.cons.get(کلید)
        اگر conflict is None:
            self.cons[کلید] = name
        وگرنه:
            print('Redefinition of constructor {}'.format(کلید))
            print('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        برای f در cons.fields:
            self.visit(f, کلید)

    def visitField(self, field, name):
        کلید = str(field.type)
        l = self.types.setdefault(کلید, [])
        l.append(name)

    def visitProduct(self, prod, name):
        برای f در prod.fields:
            self.visit(f, name)

def check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.visit(mod)

    برای t در v.types:
        اگر t نه در mod.types و نه t در builtin_types:
            v.errors += 1
            uses = ", ".join(v.types[t])
            print('Undefined type {}, used in {}'.format(t, uses))
    بازگشت نه v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

def parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    with open(filename, encoding="utf-8") بهعنوان f:
        parser = ASDLParser()
        بازگشت parser.parse(f.اقرأ())

# Types for describing tokens in an ASDL specification.
رده TokenKind:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = range(11)

    operator_table = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

رده ASDLSyntaxError(Exception):
    def __init__(self, پیام, lineno=None):
        self.پیام = پیام
        self.lineno = lineno یا '<unknown>'

    def __str__(self):
        بازگشت 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

def tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    برای lineno, سطر در enumerate(buf.splitlines(), 1):
        برای m در re.finditer(r'\s*(\w+|--.*|.)', سطر.strip()):
            c = m.group(1)
            اگر c[0].isalpha():
                # Some kind of identifier
                اگر c[0].isupper():
                    بده Token(TokenKind.ConstructorId, c, lineno)
                وگرنه:
                    بده Token(TokenKind.TypeId, c, lineno)
            وگرنهاگر c[:2] == '--':
                # Comment
                بشکن
            وگرنه:
                # Operators
                تلاش:
                    op_kind = TokenKind.operator_table[c]
                استثنا کلید_Error:
                    بالاببر ASDLSyntaxError('Invalid operator %s' % c, lineno)
                بده Token(op_kind, c, lineno)

رده ASDLParser:
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
        بازگشت self._parse_module()

    def _parse_module(self):
        اگر self._at_keyword('module'):
            self._advance()
        وگرنه:
            بالاببر ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.مقدار),
                self.cur_token.lineno)
        name = self._match(self._id_kinds)
        self._match(TokenKind.LBrace)
        defs = self._parse_definitions()
        self._match(TokenKind.RBrace)
        بازگشت Module(name, defs)

    def _parse_definitions(self):
        defs = []
        وقتی self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            self._match(TokenKind.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        بازگشت defs

    def _parse_type(self):
        اگر self.cur_token.kind == TokenKind.LParen:
            # If we see a (, it's a product
            بازگشت self._parse_product()
        وگرنه:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self._match(TokenKind.ConstructorId),
                                   self._parse_optional_fields())]
            وقتی self.cur_token.kind  == TokenKind.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self._match(TokenKind.ConstructorId),
                                self._parse_optional_fields()))
            بازگشت Sum(sumlist, self._parse_optional_attributes())

    def _parse_product(self):
        بازگشت Product(self._parse_fields(), self._parse_optional_attributes())

    def _parse_fields(self):
        fields = []
        self._match(TokenKind.LParen)
        وقتی self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            quantifiers = self._parse_optional_field_quantifier()
            id = (self._advance() اگر self.cur_token.kind در self._id_kinds
                                  وگرنه None)
            fields.append(Field(typename, id, quantifiers=quantifiers))
            اگر self.cur_token.kind == TokenKind.RParen:
                بشکن
            وگرنهاگر self.cur_token.kind == TokenKind.Comma:
                self._advance()
        self._match(TokenKind.RParen)
        بازگشت fields

    def _parse_optional_fields(self):
        اگر self.cur_token.kind == TokenKind.LParen:
            بازگشت self._parse_fields()
        وگرنه:
            بازگشت None

    def _parse_optional_attributes(self):
        اگر self._at_keyword('attributes'):
            self._advance()
            بازگشت self._parse_fields()
        وگرنه:
            بازگشت None

    def _parse_optional_field_quantifier(self):
        quantifiers = []
        وقتی self.cur_token.kind در (TokenKind.Asterisk, TokenKind.Question):
            اگر self.cur_token.kind == TokenKind.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            وگرنهاگر self.cur_token.kind == TokenKind.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        بازگشت quantifiers

    def _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = None اگر self.cur_token is None وگرنه self.cur_token.مقدار
        تلاش:
            self.cur_token = next(self._tokenizer)
        استثنا StopIteration:
            self.cur_token = None
        بازگشت cur_val

    _id_kinds = (TokenKind.ConstructorId, TokenKind.TypeId)

    def _match(self, kind):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        اگر (isinstance(kind, tuple) و self.cur_token.kind در kind یا
            self.cur_token.kind == kind
            ):
            مقدار = self.cur_token.مقدار
            self._advance()
            بازگشت مقدار
        وگرنه:
            بالاببر ASDLSyntaxError(
                'Unmatched {} (found {})'.format(kind, self.cur_token.kind),
                self.cur_token.lineno)

    def _at_keyword(self, keyword):
        بازگشت (self.cur_token.kind == TokenKind.TypeId و
                self.cur_token.مقدار == keyword)
