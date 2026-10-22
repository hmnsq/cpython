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

صنف AST:
    تعریف __repr__(self):
        ارفع NotImplementedError

صنف Module(AST):
    تعریف __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.قدر لكل type في dfns}

    تعریف __repr__(self):
        أرجع 'Module({0.name}, {0.dfns})'.format(self)

صنف Type(AST):
    تعریف __init__(self, name, قدر):
        self.name = name
        self.قدر = قدر

    تعریف __repr__(self):
        أرجع 'Type({0.name}, {0.value})'.format(self)

صنف Constructor(AST):
    تعریف __init__(self, name, fields=کجھ_نئیں):
        self.name = name
        self.fields = fields أو []

    تعریف __repr__(self):
        أرجع 'Constructor({0.name}, {0.fields})'.format(self)

صنف Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

صنف میدان(AST):
    تعریف __init__(self, type, name=کجھ_نئیں, quantifiers=کجھ_نئیں):
        self.type = type
        self.name = name
        self.seq = خطأ
        self.opt = خطأ
        self.quantifiers = quantifiers أو []
        إذا لمبائی(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] اے Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] اے Quantifier.OPTIONAL

    تعریف __str__(self):
        extra = ""
        لكل mod في self.quantifiers:
            إذا mod اے Quantifier.SEQUENCE:
                extra += "*"
            وإلاإذا mod اے Quantifier.OPTIONAL:
                extra += "?"

        أرجع "{}{} {}".format(self.type, extra, self.name)

    تعریف __repr__(self):
        إذا self.quantifiers:
            texts = []
            لكل mod في self.quantifiers:
                إذا mod اے Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                وإلاإذا mod اے Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        وإلا:
            extra = ""

        إذا self.name اے کجھ_نئیں:
            أرجع 'Field({0.type}{1})'.format(self, extra)
        وإلا:
            أرجع 'Field({0.type}, {0.name}{1})'.format(self, extra)

صنف جوڑ(AST):
    تعریف __init__(self, types, گن=کجھ_نئیں):
        self.types = types
        self.گن = گن أو []

    تعریف __repr__(self):
        إذا self.گن:
            أرجع 'Sum({0.types}, {0.attributes})'.format(self)
        وإلا:
            أرجع 'Sum({0.types})'.format(self)

صنف ضرب_حاصل(AST):
    تعریف __init__(self, fields, گن=کجھ_نئیں):
        self.fields = fields
        self.گن = گن أو []

    تعریف __repr__(self):
        إذا self.گن:
            أرجع 'Product({0.fields}, {0.attributes})'.format(self)
        وإلا:
            أرجع 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

صنف Visitor_ادھار(object):
    """Generic tree visitor for ASTs."""
    تعریف __init__(self):
        self.cache = {}

    تعریف ویکھو(self, obj, *دلیلاں):
        klass = obj.__class__
        meth = self.cache.get(klass)
        إذا meth اے کجھ_نئیں:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, کجھ_نئیں)
            self.cache[klass] = meth
        إذا meth:
            حاول:
                meth(obj, *دلیلاں)
            استثناء Exception كـ e:
                چھاپو("Error visiting %r: %s" % (obj, e))
                ارفع

صنف Check(Visitor_ادھار):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    تعریف __init__(self):
        أعلى(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    تعریف ویکھو_Module(self, mod):
        لكل dfn في mod.dfns:
            self.ویکھو(dfn)

    تعریف ویکھو_Type(self, type):
        self.ویکھو(type.قدر, str(type.name))

    تعریف ویکھو_جوڑ(self, جوڑ_2, name):
        لكل t في جوڑ_2.types:
            self.ویکھو(t, name)

    تعریف ویکھو_Constructor(self, cons, name):
        کلید = str(cons.name)
        conflict = self.cons.get(کلید)
        إذا conflict اے کجھ_نئیں:
            self.cons[کلید] = name
        وإلا:
            چھاپو('Redefinition of constructor {}'.format(کلید))
            چھاپو('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        لكل f في cons.fields:
            self.ویکھو(f, کلید)

    تعریف ویکھو_میدان(self, میدان_2, name):
        کلید = str(میدان_2.type)
        l = self.types.setdefault(کلید, [])
        l.append(name)

    تعریف ویکھو_ضرب_حاصل(self, prod, name):
        لكل f في prod.fields:
            self.ویکھو(f, name)

تعریف check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.ویکھو(mod)

    لكل t في v.types:
        إذا t ليس في mod.types و ليس t في builtin_types:
            v.errors += 1
            ورتدا = ", ".join(v.types[t])
            چھاپو('Undefined type {}, used in {}'.format(t, ورتدا))
    أرجع ليس v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

تعریف parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    نال open(filename, encoding="utf-8") كـ f:
        parser = ASDLParser()
        أرجع parser.parse(f.اقرأ())

# Types for describing tokens in an ASDL specification.
صنف Token_قسم:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = سلسلہ(11)

    operator_جدول = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

صنف ASDLSyntaxError(Exception):
    تعریف __init__(self, پیغام, lineno=کجھ_نئیں):
        self.پیغام = پیغام
        self.lineno = lineno أو '<unknown>'

    تعریف __str__(self):
        أرجع 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

تعریف tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    لكل lineno, سطر في enumerate(buf.splitlines(), 1):
        لكل m في re.finditer(r'\s*(\w+|--.*|.)', سطر.strip()):
            c = m.گروہ(1)
            إذا c[0].isalpha():
                # Some kind of identifier
                إذا c[0].isupper():
                    أنتج Token(Token_قسم.ConstructorId, c, lineno)
                وإلا:
                    أنتج Token(Token_قسم.TypeId, c, lineno)
            وإلاإذا c[:2] == '--':
                # Comment
                اكسر
            وإلا:
                # Operators
                حاول:
                    op_قسم = Token_قسم.operator_جدول[c]
                استثناء کلید_Error:
                    ارفع ASDLSyntaxError('Invalid operator %s' % c, lineno)
                أنتج Token(op_قسم, c, lineno)

صنف ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    تعریف __init__(self):
        self._tokenizer = کجھ_نئیں
        self.cur_token = کجھ_نئیں

    تعریف parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        أرجع self._parse_module()

    تعریف _parse_module(self):
        إذا self._at_keyword('module'):
            self._advance()
        وإلا:
            ارفع ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.قدر),
                self.cur_token.lineno)
        name = self.میل(self._id_kinds)
        self.میل(Token_قسم.LBrace)
        defs = self._parse_definitions()
        self.میل(Token_قسم.RBrace)
        أرجع Module(name, defs)

    تعریف _parse_definitions(self):
        defs = []
        طالما self.cur_token.قسم == Token_قسم.TypeId:
            typename = self._advance()
            self.میل(Token_قسم.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        أرجع defs

    تعریف _parse_type(self):
        إذا self.cur_token.قسم == Token_قسم.LParen:
            # If we see a (, it's a product
            أرجع self.parse_ضرب_حاصل()
        وإلا:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.میل(Token_قسم.ConstructorId),
                                   self._parse_optional_fields())]
            طالما self.cur_token.قسم  == Token_قسم.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.میل(Token_قسم.ConstructorId),
                                self._parse_optional_fields()))
            أرجع جوڑ(sumlist, self.parse_optional_گن())

    تعریف parse_ضرب_حاصل(self):
        أرجع ضرب_حاصل(self._parse_fields(), self.parse_optional_گن())

    تعریف _parse_fields(self):
        fields = []
        self.میل(Token_قسم.LParen)
        طالما self.cur_token.قسم == Token_قسم.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_میدان_quantifier()
            id = (self._advance() إذا self.cur_token.قسم في self._id_kinds
                                  وإلا کجھ_نئیں)
            fields.append(میدان(typename, id, quantifiers=quantifiers))
            إذا self.cur_token.قسم == Token_قسم.RParen:
                اكسر
            وإلاإذا self.cur_token.قسم == Token_قسم.Comma:
                self._advance()
        self.میل(Token_قسم.RParen)
        أرجع fields

    تعریف _parse_optional_fields(self):
        إذا self.cur_token.قسم == Token_قسم.LParen:
            أرجع self._parse_fields()
        وإلا:
            أرجع کجھ_نئیں

    تعریف parse_optional_گن(self):
        إذا self._at_keyword('attributes'):
            self._advance()
            أرجع self._parse_fields()
        وإلا:
            أرجع کجھ_نئیں

    تعریف parse_optional_میدان_quantifier(self):
        quantifiers = []
        طالما self.cur_token.قسم في (Token_قسم.Asterisk, Token_قسم.Question):
            إذا self.cur_token.قسم == Token_قسم.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            وإلاإذا self.cur_token.قسم == Token_قسم.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        أرجع quantifiers

    تعریف _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = کجھ_نئیں إذا self.cur_token اے کجھ_نئیں وإلا self.cur_token.قدر
        حاول:
            self.cur_token = اگلا(self._tokenizer)
        استثناء StopIteration:
            self.cur_token = کجھ_نئیں
        أرجع cur_val

    _id_kinds = (Token_قسم.ConstructorId, Token_قسم.TypeId)

    تعریف میل(self, قسم):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        إذا (isinstance(قسم, tuple) و self.cur_token.قسم في قسم أو
            self.cur_token.قسم == قسم
            ):
            قدر = self.cur_token.قدر
            self._advance()
            أرجع قدر
        وإلا:
            ارفع ASDLSyntaxError(
                'Unmatched {} (found {})'.format(قسم, self.cur_token.قسم),
                self.cur_token.lineno)

    تعریف _at_keyword(self, keyword):
        أرجع (self.cur_token.قسم == Token_قسم.TypeId و
                self.cur_token.قدر == keyword)
