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
तः collections आयात namedtuple
आयात enum
आयात re

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

वर्ग AST:
    def __repr__(self):
        उठवा NotImplementedError

वर्ग Module(AST):
    def __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.मूल्य साठी type मध्ये dfns}

    def __repr__(self):
        परत 'Module({0.name}, {0.dfns})'.format(self)

वर्ग Type(AST):
    def __init__(self, name, मूल्य):
        self.name = name
        self.मूल्य = मूल्य

    def __repr__(self):
        परत 'Type({0.name}, {0.value})'.format(self)

वर्ग Constructor(AST):
    def __init__(self, name, fields=None):
        self.name = name
        self.fields = fields किंवा []

    def __repr__(self):
        परत 'Constructor({0.name}, {0.fields})'.format(self)

वर्ग Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

वर्ग Field(AST):
    def __init__(self, type, name=None, quantifiers=None):
        self.type = type
        self.name = name
        self.seq = असत्य
        self.opt = असत्य
        self.quantifiers = quantifiers किंवा []
        जर len(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] is Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] is Quantifier.OPTIONAL

    def __str__(self):
        extra = ""
        साठी mod मध्ये self.quantifiers:
            जर mod is Quantifier.SEQUENCE:
                extra += "*"
            किंवाजर mod is Quantifier.OPTIONAL:
                extra += "?"

        परत "{}{} {}".format(self.type, extra, self.name)

    def __repr__(self):
        जर self.quantifiers:
            texts = []
            साठी mod मध्ये self.quantifiers:
                जर mod is Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                किंवाजर mod is Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        अन्यथा:
            extra = ""

        जर self.name is None:
            परत 'Field({0.type}{1})'.format(self, extra)
        अन्यथा:
            परत 'Field({0.type}, {0.name}{1})'.format(self, extra)

वर्ग Sum(AST):
    def __init__(self, types, attributes=None):
        self.types = types
        self.attributes = attributes किंवा []

    def __repr__(self):
        जर self.attributes:
            परत 'Sum({0.types}, {0.attributes})'.format(self)
        अन्यथा:
            परत 'Sum({0.types})'.format(self)

वर्ग Product(AST):
    def __init__(self, fields, attributes=None):
        self.fields = fields
        self.attributes = attributes किंवा []

    def __repr__(self):
        जर self.attributes:
            परत 'Product({0.fields}, {0.attributes})'.format(self)
        अन्यथा:
            परत 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

वर्ग VisitorBase(object):
    """Generic tree visitor for ASTs."""
    def __init__(self):
        self.cache = {}

    def visit(self, obj, *args):
        klass = obj.__class__
        meth = self.cache.get(klass)
        जर meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, None)
            self.cache[klass] = meth
        जर meth:
            प्रयत्न:
                meth(obj, *args)
            अपवाद Exception इव e:
                print("Error visiting %r: %s" % (obj, e))
                उठवा

वर्ग Check(VisitorBase):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    def __init__(self):
        परम(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    def visitModule(self, mod):
        साठी dfn मध्ये mod.dfns:
            self.visit(dfn)

    def visitType(self, type):
        self.visit(type.मूल्य, str(type.name))

    def visitSum(self, sum, name):
        साठी t मध्ये sum.types:
            self.visit(t, name)

    def visitConstructor(self, cons, name):
        किल्ली = str(cons.name)
        conflict = self.cons.get(किल्ली)
        जर conflict is None:
            self.cons[किल्ली] = name
        अन्यथा:
            print('Redefinition of constructor {}'.format(किल्ली))
            print('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        साठी f मध्ये cons.fields:
            self.visit(f, किल्ली)

    def visitField(self, field, name):
        किल्ली = str(field.type)
        l = self.types.setdefault(किल्ली, [])
        l.append(name)

    def visitProduct(self, prod, name):
        साठी f मध्ये prod.fields:
            self.visit(f, name)

def check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.visit(mod)

    साठी t मध्ये v.types:
        जर t नाही मध्ये mod.types आणि नाही t मध्ये builtin_types:
            v.errors += 1
            uses = ", ".join(v.types[t])
            print('Undefined type {}, used in {}'.format(t, uses))
    परत नाही v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

def parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    with open(filename, encoding="utf-8") इव f:
        parser = ASDLParser()
        परत parser.parse(f.पठ())

# Types for describing tokens in an ASDL specification.
वर्ग TokenKind:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = range(11)

    operator_table = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

वर्ग ASDLSyntaxError(Exception):
    def __init__(self, संदेश, lineno=None):
        self.संदेश = संदेश
        self.lineno = lineno किंवा '<unknown>'

    def __str__(self):
        परत 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

def tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    साठी lineno, ओळ मध्ये enumerate(buf.splitlines(), 1):
        साठी m मध्ये re.finditer(r'\s*(\w+|--.*|.)', ओळ.strip()):
            c = m.group(1)
            जर c[0].isalpha():
                # Some kind of identifier
                जर c[0].isupper():
                    द्या Token(TokenKind.ConstructorId, c, lineno)
                अन्यथा:
                    द्या Token(TokenKind.TypeId, c, lineno)
            किंवाजर c[:2] == '--':
                # Comment
                थांबा
            अन्यथा:
                # Operators
                प्रयत्न:
                    op_kind = TokenKind.operator_table[c]
                अपवाद किल्ली_Error:
                    उठवा ASDLSyntaxError('Invalid operator %s' % c, lineno)
                द्या Token(op_kind, c, lineno)

वर्ग ASDLParser:
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
        परत self._parse_module()

    def _parse_module(self):
        जर self._at_keyword('module'):
            self._advance()
        अन्यथा:
            उठवा ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.मूल्य),
                self.cur_token.lineno)
        name = self._match(self._id_kinds)
        self._match(TokenKind.LBrace)
        defs = self._parse_definitions()
        self._match(TokenKind.RBrace)
        परत Module(name, defs)

    def _parse_definitions(self):
        defs = []
        पर्यंत self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            self._match(TokenKind.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        परत defs

    def _parse_type(self):
        जर self.cur_token.kind == TokenKind.LParen:
            # If we see a (, it's a product
            परत self._parse_product()
        अन्यथा:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self._match(TokenKind.ConstructorId),
                                   self._parse_optional_fields())]
            पर्यंत self.cur_token.kind  == TokenKind.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self._match(TokenKind.ConstructorId),
                                self._parse_optional_fields()))
            परत Sum(sumlist, self._parse_optional_attributes())

    def _parse_product(self):
        परत Product(self._parse_fields(), self._parse_optional_attributes())

    def _parse_fields(self):
        fields = []
        self._match(TokenKind.LParen)
        पर्यंत self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            quantifiers = self._parse_optional_field_quantifier()
            id = (self._advance() जर self.cur_token.kind मध्ये self._id_kinds
                                  अन्यथा None)
            fields.append(Field(typename, id, quantifiers=quantifiers))
            जर self.cur_token.kind == TokenKind.RParen:
                थांबा
            किंवाजर self.cur_token.kind == TokenKind.Comma:
                self._advance()
        self._match(TokenKind.RParen)
        परत fields

    def _parse_optional_fields(self):
        जर self.cur_token.kind == TokenKind.LParen:
            परत self._parse_fields()
        अन्यथा:
            परत None

    def _parse_optional_attributes(self):
        जर self._at_keyword('attributes'):
            self._advance()
            परत self._parse_fields()
        अन्यथा:
            परत None

    def _parse_optional_field_quantifier(self):
        quantifiers = []
        पर्यंत self.cur_token.kind मध्ये (TokenKind.Asterisk, TokenKind.Question):
            जर self.cur_token.kind == TokenKind.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            किंवाजर self.cur_token.kind == TokenKind.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        परत quantifiers

    def _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = None जर self.cur_token is None अन्यथा self.cur_token.मूल्य
        प्रयत्न:
            self.cur_token = next(self._tokenizer)
        अपवाद StopIteration:
            self.cur_token = None
        परत cur_val

    _id_kinds = (TokenKind.ConstructorId, TokenKind.TypeId)

    def _match(self, kind):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        जर (isinstance(kind, tuple) आणि self.cur_token.kind मध्ये kind किंवा
            self.cur_token.kind == kind
            ):
            मूल्य = self.cur_token.मूल्य
            self._advance()
            परत मूल्य
        अन्यथा:
            उठवा ASDLSyntaxError(
                'Unmatched {} (found {})'.format(kind, self.cur_token.kind),
                self.cur_token.lineno)

    def _at_keyword(self, keyword):
        परत (self.cur_token.kind == TokenKind.TypeId आणि
                self.cur_token.मूल्य == keyword)
