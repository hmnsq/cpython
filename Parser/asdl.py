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
తః collections దిగుమతి namedtuple
దిగుమతి enum
దిగుమతి re

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

తరగతి AST:
    నిర్వచనం __repr__(self):
        లేపు NotImplementedError

తరగతి Module(AST):
    నిర్వచనం __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.విలువ కొరకు type లో dfns}

    నిర్వచనం __repr__(self):
        తిరిగి 'Module({0.name}, {0.dfns})'.format(self)

తరగతి Type(AST):
    నిర్వచనం __init__(self, name, విలువ):
        self.name = name
        self.విలువ = విలువ

    నిర్వచనం __repr__(self):
        తిరిగి 'Type({0.name}, {0.value})'.format(self)

తరగతి Constructor(AST):
    నిర్వచనం __init__(self, name, fields=ఏమీలేదు):
        self.name = name
        self.fields = fields లేదా []

    నిర్వచనం __repr__(self):
        తిరిగి 'Constructor({0.name}, {0.fields})'.format(self)

తరగతి Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

తరగతి క్షేత్రం(AST):
    నిర్వచనం __init__(self, type, name=ఏమీలేదు, quantifiers=ఏమీలేదు):
        self.type = type
        self.name = name
        self.seq = అబద్ధం
        self.opt = అబద్ధం
        self.quantifiers = quantifiers లేదా []
        ఒకవేళ పొడవు(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] అయిన Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] అయిన Quantifier.OPTIONAL

    నిర్వచనం __str__(self):
        extra = ""
        కొరకు mod లో self.quantifiers:
            ఒకవేళ mod అయిన Quantifier.SEQUENCE:
                extra += "*"
            లేదావేళ mod అయిన Quantifier.OPTIONAL:
                extra += "?"

        తిరిగి "{}{} {}".format(self.type, extra, self.name)

    నిర్వచనం __repr__(self):
        ఒకవేళ self.quantifiers:
            texts = []
            కొరకు mod లో self.quantifiers:
                ఒకవేళ mod అయిన Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                లేదావేళ mod అయిన Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        లేకపోతే:
            extra = ""

        ఒకవేళ self.name అయిన ఏమీలేదు:
            తిరిగి 'Field({0.type}{1})'.format(self, extra)
        లేకపోతే:
            తిరిగి 'Field({0.type}, {0.name}{1})'.format(self, extra)

తరగతి కూడిక(AST):
    నిర్వచనం __init__(self, types, లక్షణాలు=ఏమీలేదు):
        self.types = types
        self.లక్షణాలు = లక్షణాలు లేదా []

    నిర్వచనం __repr__(self):
        ఒకవేళ self.లక్షణాలు:
            తిరిగి 'Sum({0.types}, {0.attributes})'.format(self)
        లేకపోతే:
            తిరిగి 'Sum({0.types})'.format(self)

తరగతి లబ్ధం(AST):
    నిర్వచనం __init__(self, fields, లక్షణాలు=ఏమీలేదు):
        self.fields = fields
        self.లక్షణాలు = లక్షణాలు లేదా []

    నిర్వచనం __repr__(self):
        ఒకవేళ self.లక్షణాలు:
            తిరిగి 'Product({0.fields}, {0.attributes})'.format(self)
        లేకపోతే:
            తిరిగి 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

తరగతి Visitor_పునాది(object):
    """Generic tree visitor for ASTs."""
    నిర్వచనం __init__(self):
        self.cache = {}

    నిర్వచనం సందర్శించు(self, obj, *వాదనలు):
        klass = obj.__class__
        meth = self.cache.get(klass)
        ఒకవేళ meth అయిన ఏమీలేదు:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, ఏమీలేదు)
            self.cache[klass] = meth
        ఒకవేళ meth:
            ప్రయత్నించు:
                meth(obj, *వాదనలు)
            మినహాయింపు Exception ఇవ e:
                ముద్రించు("Error visiting %r: %s" % (obj, e))
                లేపు

తరగతి Check(Visitor_పునాది):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    నిర్వచనం __init__(self):
        పర(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    నిర్వచనం సందర్శించు_Module(self, mod):
        కొరకు dfn లో mod.dfns:
            self.సందర్శించు(dfn)

    నిర్వచనం సందర్శించు_Type(self, type):
        self.సందర్శించు(type.విలువ, str(type.name))

    నిర్వచనం సందర్శించు_కూడిక(self, కూడిక_2, name):
        కొరకు t లో కూడిక_2.types:
            self.సందర్శించు(t, name)

    నిర్వచనం సందర్శించు_Constructor(self, cons, name):
        కీలకం = str(cons.name)
        conflict = self.cons.get(కీలకం)
        ఒకవేళ conflict అయిన ఏమీలేదు:
            self.cons[కీలకం] = name
        లేకపోతే:
            ముద్రించు('Redefinition of constructor {}'.format(కీలకం))
            ముద్రించు('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        కొరకు f లో cons.fields:
            self.సందర్శించు(f, కీలకం)

    నిర్వచనం సందర్శించు_క్షేత్రం(self, క్షేత్రం_2, name):
        కీలకం = str(క్షేత్రం_2.type)
        l = self.types.setdefault(కీలకం, [])
        l.append(name)

    నిర్వచనం సందర్శించు_లబ్ధం(self, prod, name):
        కొరకు f లో prod.fields:
            self.సందర్శించు(f, name)

నిర్వచనం check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.సందర్శించు(mod)

    కొరకు t లో v.types:
        ఒకవేళ t కాదు లో mod.types మరియు కాదు t లో builtin_types:
            v.errors += 1
            ఉపయోగిస్తుంది = ", ".join(v.types[t])
            ముద్రించు('Undefined type {}, used in {}'.format(t, ఉపయోగిస్తుంది))
    తిరిగి కాదు v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

నిర్వచనం parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    తోడుగా open(filename, encoding="utf-8") ఇవ f:
        parser = ASDLParser()
        తిరిగి parser.parse(f.పఠ())

# Types for describing tokens in an ASDL specification.
తరగతి Token_రకం:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = శ్రేణి(11)

    operator_పట్టిక = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

తరగతి ASDLSyntaxError(Exception):
    నిర్వచనం __init__(self, సందేశం, lineno=ఏమీలేదు):
        self.సందేశం = సందేశం
        self.lineno = lineno లేదా '<unknown>'

    నిర్వచనం __str__(self):
        తిరిగి 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

నిర్వచనం tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    కొరకు lineno, పంక్తి లో enumerate(buf.splitlines(), 1):
        కొరకు m లో re.finditer(r'\s*(\w+|--.*|.)', పంక్తి.strip()):
            c = m.సమూహం(1)
            ఒకవేళ c[0].isalpha():
                # Some kind of identifier
                ఒకవేళ c[0].isupper():
                    ఇవ్వు Token(Token_రకం.ConstructorId, c, lineno)
                లేకపోతే:
                    ఇవ్వు Token(Token_రకం.TypeId, c, lineno)
            లేదావేళ c[:2] == '--':
                # Comment
                ఆపు
            లేకపోతే:
                # Operators
                ప్రయత్నించు:
                    op_రకం = Token_రకం.operator_పట్టిక[c]
                మినహాయింపు కీలకం_Error:
                    లేపు ASDLSyntaxError('Invalid operator %s' % c, lineno)
                ఇవ్వు Token(op_రకం, c, lineno)

తరగతి ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    నిర్వచనం __init__(self):
        self._tokenizer = ఏమీలేదు
        self.cur_token = ఏమీలేదు

    నిర్వచనం parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        తిరిగి self._parse_module()

    నిర్వచనం _parse_module(self):
        ఒకవేళ self._at_keyword('module'):
            self._advance()
        లేకపోతే:
            లేపు ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.విలువ),
                self.cur_token.lineno)
        name = self.సరిపోలిక(self._id_kinds)
        self.సరిపోలిక(Token_రకం.LBrace)
        defs = self._parse_definitions()
        self.సరిపోలిక(Token_రకం.RBrace)
        తిరిగి Module(name, defs)

    నిర్వచనం _parse_definitions(self):
        defs = []
        అయితే self.cur_token.రకం == Token_రకం.TypeId:
            typename = self._advance()
            self.సరిపోలిక(Token_రకం.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        తిరిగి defs

    నిర్వచనం _parse_type(self):
        ఒకవేళ self.cur_token.రకం == Token_రకం.LParen:
            # If we see a (, it's a product
            తిరిగి self.parse_లబ్ధం()
        లేకపోతే:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.సరిపోలిక(Token_రకం.ConstructorId),
                                   self._parse_optional_fields())]
            అయితే self.cur_token.రకం  == Token_రకం.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.సరిపోలిక(Token_రకం.ConstructorId),
                                self._parse_optional_fields()))
            తిరిగి కూడిక(sumlist, self.parse_optional_లక్షణాలు())

    నిర్వచనం parse_లబ్ధం(self):
        తిరిగి లబ్ధం(self._parse_fields(), self.parse_optional_లక్షణాలు())

    నిర్వచనం _parse_fields(self):
        fields = []
        self.సరిపోలిక(Token_రకం.LParen)
        అయితే self.cur_token.రకం == Token_రకం.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_క్షేత్రం_quantifier()
            id = (self._advance() ఒకవేళ self.cur_token.రకం లో self._id_kinds
                                  లేకపోతే ఏమీలేదు)
            fields.append(క్షేత్రం(typename, id, quantifiers=quantifiers))
            ఒకవేళ self.cur_token.రకం == Token_రకం.RParen:
                ఆపు
            లేదావేళ self.cur_token.రకం == Token_రకం.Comma:
                self._advance()
        self.సరిపోలిక(Token_రకం.RParen)
        తిరిగి fields

    నిర్వచనం _parse_optional_fields(self):
        ఒకవేళ self.cur_token.రకం == Token_రకం.LParen:
            తిరిగి self._parse_fields()
        లేకపోతే:
            తిరిగి ఏమీలేదు

    నిర్వచనం parse_optional_లక్షణాలు(self):
        ఒకవేళ self._at_keyword('attributes'):
            self._advance()
            తిరిగి self._parse_fields()
        లేకపోతే:
            తిరిగి ఏమీలేదు

    నిర్వచనం parse_optional_క్షేత్రం_quantifier(self):
        quantifiers = []
        అయితే self.cur_token.రకం లో (Token_రకం.Asterisk, Token_రకం.Question):
            ఒకవేళ self.cur_token.రకం == Token_రకం.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            లేదావేళ self.cur_token.రకం == Token_రకం.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        తిరిగి quantifiers

    నిర్వచనం _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = ఏమీలేదు ఒకవేళ self.cur_token అయిన ఏమీలేదు లేకపోతే self.cur_token.విలువ
        ప్రయత్నించు:
            self.cur_token = తదుపరి(self._tokenizer)
        మినహాయింపు StopIteration:
            self.cur_token = ఏమీలేదు
        తిరిగి cur_val

    _id_kinds = (Token_రకం.ConstructorId, Token_రకం.TypeId)

    నిర్వచనం సరిపోలిక(self, రకం):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        ఒకవేళ (isinstance(రకం, tuple) మరియు self.cur_token.రకం లో రకం లేదా
            self.cur_token.రకం == రకం
            ):
            విలువ = self.cur_token.విలువ
            self._advance()
            తిరిగి విలువ
        లేకపోతే:
            లేపు ASDLSyntaxError(
                'Unmatched {} (found {})'.format(రకం, self.cur_token.రకం),
                self.cur_token.lineno)

    నిర్వచనం _at_keyword(self, keyword):
        తిరిగి (self.cur_token.రకం == Token_రకం.TypeId మరియు
                self.cur_token.విలువ == keyword)
