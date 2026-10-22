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
من collections استورد namedtuple
استورد enum
استورد re

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

מחלקה AST:
    def __repr__(self):
        ارفع NotImplementedError

מחלקה Module(AST):
    def __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.value עבור type في dfns}

    def __repr__(self):
        החזר 'Module({0.name}, {0.dfns})'.format(self)

מחלקה Type(AST):
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def __repr__(self):
        החזר 'Type({0.name}, {0.value})'.format(self)

מחלקה Constructor(AST):
    def __init__(self, name, fields=None):
        self.name = name
        self.fields = fields או []

    def __repr__(self):
        החזר 'Constructor({0.name}, {0.fields})'.format(self)

מחלקה Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

מחלקה Field(AST):
    def __init__(self, type, name=None, quantifiers=None):
        self.type = type
        self.name = name
        self.seq = שקר
        self.opt = שקר
        self.quantifiers = quantifiers או []
        אם len(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] is Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] is Quantifier.OPTIONAL

    def __str__(self):
        extra = ""
        עבור mod في self.quantifiers:
            אם mod is Quantifier.SEQUENCE:
                extra += "*"
            وإلاإذا mod is Quantifier.OPTIONAL:
                extra += "?"

        החזר "{}{} {}".format(self.type, extra, self.name)

    def __repr__(self):
        אם self.quantifiers:
            texts = []
            עבור mod في self.quantifiers:
                אם mod is Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                وإلاإذا mod is Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        אחרת:
            extra = ""

        אם self.name is None:
            החזר 'Field({0.type}{1})'.format(self, extra)
        אחרת:
            החזר 'Field({0.type}, {0.name}{1})'.format(self, extra)

מחלקה Sum(AST):
    def __init__(self, types, attributes=None):
        self.types = types
        self.attributes = attributes או []

    def __repr__(self):
        אם self.attributes:
            החזר 'Sum({0.types}, {0.attributes})'.format(self)
        אחרת:
            החזר 'Sum({0.types})'.format(self)

מחלקה Product(AST):
    def __init__(self, fields, attributes=None):
        self.fields = fields
        self.attributes = attributes או []

    def __repr__(self):
        אם self.attributes:
            החזר 'Product({0.fields}, {0.attributes})'.format(self)
        אחרת:
            החזר 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

מחלקה VisitorBase(object):
    """Generic tree visitor for ASTs."""
    def __init__(self):
        self.cache = {}

    def visit(self, obj, *args):
        klass = obj.__class__
        meth = self.cache.get(klass)
        אם meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, None)
            self.cache[klass] = meth
        אם meth:
            حاول:
                meth(obj, *args)
            استثناء Exception كـ e:
                print("Error visiting %r: %s" % (obj, e))
                ارفع

מחלקה Check(VisitorBase):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    def __init__(self):
        أعلى(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    def visitModule(self, mod):
        עבור dfn في mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.value, str(type.name))

    def visitSum(self, sum, name):
        עבור t في sum.types:
            self.visit(t, name)

    def visitConstructor(self, cons, name):
        key = str(cons.name)
        conflict = self.cons.get(key)
        אם conflict is None:
            self.cons[key] = name
        אחרת:
            print('Redefinition of constructor {}'.format(key))
            print('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        עבור f في cons.fields:
            self.visit(f, key)

    def visitField(self, field, name):
        key = str(field.type)
        l = self.types.setdefault(key, [])
        l.append(name)

    def visitProduct(self, prod, name):
        עבור f في prod.fields:
            self.visit(f, name)

def check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.visit(mod)

    עבור t في v.types:
        אם t לא في mod.types וגם לא t في builtin_types:
            v.errors += 1
            uses = ", ".join(v.types[t])
            print('Undefined type {}, used in {}'.format(t, uses))
    החזר לא v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

def parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    with open(filename, encoding="utf-8") كـ f:
        parser = ASDLParser()
        החזר parser.parse(f.اقرأ())

# Types for describing tokens in an ASDL specification.
מחלקה TokenKind:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = range(11)

    operator_table = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

מחלקה ASDLSyntaxError(Exception):
    def __init__(self, msg, lineno=None):
        self.msg = msg
        self.lineno = lineno או '<unknown>'

    def __str__(self):
        החזר 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

def tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    עבור lineno, line في enumerate(buf.splitlines(), 1):
        עבור m في re.finditer(r'\s*(\w+|--.*|.)', line.strip()):
            c = m.group(1)
            אם c[0].isalpha():
                # Some kind of identifier
                אם c[0].isupper():
                    أنتج Token(TokenKind.ConstructorId, c, lineno)
                אחרת:
                    أنتج Token(TokenKind.TypeId, c, lineno)
            وإلاإذا c[:2] == '--':
                # Comment
                اكسر
            אחרת:
                # Operators
                حاول:
                    op_kind = TokenKind.operator_table[c]
                استثناء KeyError:
                    ارفع ASDLSyntaxError('Invalid operator %s' % c, lineno)
                أنتج Token(op_kind, c, lineno)

מחלקה ASDLParser:
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
        החזר self._parse_module()

    def _parse_module(self):
        אם self._at_keyword('module'):
            self._advance()
        אחרת:
            ارفع ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.value),
                self.cur_token.lineno)
        name = self._match(self._id_kinds)
        self._match(TokenKind.LBrace)
        defs = self._parse_definitions()
        self._match(TokenKind.RBrace)
        החזר Module(name, defs)

    def _parse_definitions(self):
        defs = []
        כלעוד self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            self._match(TokenKind.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        החזר defs

    def _parse_type(self):
        אם self.cur_token.kind == TokenKind.LParen:
            # If we see a (, it's a product
            החזר self._parse_product()
        אחרת:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self._match(TokenKind.ConstructorId),
                                   self._parse_optional_fields())]
            כלעוד self.cur_token.kind  == TokenKind.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self._match(TokenKind.ConstructorId),
                                self._parse_optional_fields()))
            החזר Sum(sumlist, self._parse_optional_attributes())

    def _parse_product(self):
        החזר Product(self._parse_fields(), self._parse_optional_attributes())

    def _parse_fields(self):
        fields = []
        self._match(TokenKind.LParen)
        כלעוד self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            quantifiers = self._parse_optional_field_quantifier()
            id = (self._advance() אם self.cur_token.kind في self._id_kinds
                                  אחרת None)
            fields.append(Field(typename, id, quantifiers=quantifiers))
            אם self.cur_token.kind == TokenKind.RParen:
                اكسر
            وإلاإذا self.cur_token.kind == TokenKind.Comma:
                self._advance()
        self._match(TokenKind.RParen)
        החזר fields

    def _parse_optional_fields(self):
        אם self.cur_token.kind == TokenKind.LParen:
            החזר self._parse_fields()
        אחרת:
            החזר None

    def _parse_optional_attributes(self):
        אם self._at_keyword('attributes'):
            self._advance()
            החזר self._parse_fields()
        אחרת:
            החזר None

    def _parse_optional_field_quantifier(self):
        quantifiers = []
        כלעוד self.cur_token.kind في (TokenKind.Asterisk, TokenKind.Question):
            אם self.cur_token.kind == TokenKind.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            وإلاإذا self.cur_token.kind == TokenKind.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        החזר quantifiers

    def _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = None אם self.cur_token is None אחרת self.cur_token.value
        حاول:
            self.cur_token = next(self._tokenizer)
        استثناء StopIteration:
            self.cur_token = None
        החזר cur_val

    _id_kinds = (TokenKind.ConstructorId, TokenKind.TypeId)

    def _match(self, kind):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        אם (isinstance(kind, tuple) וגם self.cur_token.kind في kind או
            self.cur_token.kind == kind
            ):
            value = self.cur_token.value
            self._advance()
            החזר value
        אחרת:
            ارفع ASDLSyntaxError(
                'Unmatched {} (found {})'.format(kind, self.cur_token.kind),
                self.cur_token.lineno)

    def _at_keyword(self, keyword):
        החזר (self.cur_token.kind == TokenKind.TypeId וגם
                self.cur_token.value == keyword)
