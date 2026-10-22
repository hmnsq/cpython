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
        self.types = {type.name: type.ارزښت لكل type في dfns}

    تعریف __repr__(self):
        أرجع 'Module({0.name}, {0.dfns})'.format(self)

صنف Type(AST):
    تعریف __init__(self, name, ارزښت):
        self.name = name
        self.ارزښت = ارزښت

    تعریف __repr__(self):
        أرجع 'Type({0.name}, {0.value})'.format(self)

صنف Constructor(AST):
    تعریف __init__(self, name, fields=هیڅ):
        self.name = name
        self.fields = fields أو []

    تعریف __repr__(self):
        أرجع 'Constructor({0.name}, {0.fields})'.format(self)

صنف Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

صنف ډګر(AST):
    تعریف __init__(self, type, name=هیڅ, quantifiers=هیڅ):
        self.type = type
        self.name = name
        self.seq = خطأ
        self.opt = خطأ
        self.quantifiers = quantifiers أو []
        إذا اوږدوالی(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] دی Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] دی Quantifier.OPTIONAL

    تعریف __str__(self):
        extra = ""
        لكل mod في self.quantifiers:
            إذا mod دی Quantifier.SEQUENCE:
                extra += "*"
            وإلاإذا mod دی Quantifier.OPTIONAL:
                extra += "?"

        أرجع "{}{} {}".format(self.type, extra, self.name)

    تعریف __repr__(self):
        إذا self.quantifiers:
            texts = []
            لكل mod في self.quantifiers:
                إذا mod دی Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                وإلاإذا mod دی Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        وإلا:
            extra = ""

        إذا self.name دی هیڅ:
            أرجع 'Field({0.type}{1})'.format(self, extra)
        وإلا:
            أرجع 'Field({0.type}, {0.name}{1})'.format(self, extra)

صنف مجموعه(AST):
    تعریف __init__(self, types, ځانګړتیاوې=هیڅ):
        self.types = types
        self.ځانګړتیاوې = ځانګړتیاوې أو []

    تعریف __repr__(self):
        إذا self.ځانګړتیاوې:
            أرجع 'Sum({0.types}, {0.attributes})'.format(self)
        وإلا:
            أرجع 'Sum({0.types})'.format(self)

صنف حاصل_ضرب(AST):
    تعریف __init__(self, fields, ځانګړتیاوې=هیڅ):
        self.fields = fields
        self.ځانګړتیاوې = ځانګړتیاوې أو []

    تعریف __repr__(self):
        إذا self.ځانګړتیاوې:
            أرجع 'Product({0.fields}, {0.attributes})'.format(self)
        وإلا:
            أرجع 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

صنف Visitor_بنسټ(object):
    """Generic tree visitor for ASTs."""
    تعریف __init__(self):
        self.cache = {}

    تعریف لیدنه(self, obj, *دلیلونه):
        klass = obj.__class__
        meth = self.cache.get(klass)
        إذا meth دی هیڅ:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, هیڅ)
            self.cache[klass] = meth
        إذا meth:
            حاول:
                meth(obj, *دلیلونه)
            استثناء Exception كـ e:
                چاپ("Error visiting %r: %s" % (obj, e))
                ارفع

صنف Check(Visitor_بنسټ):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    تعریف __init__(self):
        أعلى(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    تعریف لیدنه_Module(self, mod):
        لكل dfn في mod.dfns:
            self.لیدنه(dfn)

    تعریف لیدنه_Type(self, type):
        self.لیدنه(type.ارزښت, str(type.name))

    تعریف لیدنه_مجموعه(self, مجموعه_2, name):
        لكل t في مجموعه_2.types:
            self.لیدنه(t, name)

    تعریف لیدنه_Constructor(self, cons, name):
        کلی = str(cons.name)
        conflict = self.cons.get(کلی)
        إذا conflict دی هیڅ:
            self.cons[کلی] = name
        وإلا:
            چاپ('Redefinition of constructor {}'.format(کلی))
            چاپ('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        لكل f في cons.fields:
            self.لیدنه(f, کلی)

    تعریف لیدنه_ډګر(self, ډګر_2, name):
        کلی = str(ډګر_2.type)
        l = self.types.setdefault(کلی, [])
        l.append(name)

    تعریف لیدنه_حاصل_ضرب(self, prod, name):
        لكل f في prod.fields:
            self.لیدنه(f, name)

تعریف check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.لیدنه(mod)

    لكل t في v.types:
        إذا t ليس في mod.types و ليس t في builtin_types:
            v.errors += 1
            کاروي = ", ".join(v.types[t])
            چاپ('Undefined type {}, used in {}'.format(t, کاروي))
    أرجع ليس v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

تعریف parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    سره open(filename, encoding="utf-8") كـ f:
        parser = ASDLParser()
        أرجع parser.parse(f.اقرأ())

# Types for describing tokens in an ASDL specification.
صنف Token_ډول:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = لړ(11)

    operator_جدول = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

صنف ASDLSyntaxError(Exception):
    تعریف __init__(self, پیغام, lineno=هیڅ):
        self.پیغام = پیغام
        self.lineno = lineno أو '<unknown>'

    تعریف __str__(self):
        أرجع 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

تعریف tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    لكل lineno, کرښه في enumerate(buf.splitlines(), 1):
        لكل m في re.finditer(r'\s*(\w+|--.*|.)', کرښه.strip()):
            c = m.ډله(1)
            إذا c[0].isalpha():
                # Some kind of identifier
                إذا c[0].isupper():
                    أنتج Token(Token_ډول.ConstructorId, c, lineno)
                وإلا:
                    أنتج Token(Token_ډول.TypeId, c, lineno)
            وإلاإذا c[:2] == '--':
                # Comment
                اكسر
            وإلا:
                # Operators
                حاول:
                    op_ډول = Token_ډول.operator_جدول[c]
                استثناء کلی_Error:
                    ارفع ASDLSyntaxError('Invalid operator %s' % c, lineno)
                أنتج Token(op_ډول, c, lineno)

صنف ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    تعریف __init__(self):
        self._tokenizer = هیڅ
        self.cur_token = هیڅ

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
                'Expected "module" (found {})'.format(self.cur_token.ارزښت),
                self.cur_token.lineno)
        name = self.سمون(self._id_kinds)
        self.سمون(Token_ډول.LBrace)
        defs = self._parse_definitions()
        self.سمون(Token_ډول.RBrace)
        أرجع Module(name, defs)

    تعریف _parse_definitions(self):
        defs = []
        طالما self.cur_token.ډول == Token_ډول.TypeId:
            typename = self._advance()
            self.سمون(Token_ډول.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        أرجع defs

    تعریف _parse_type(self):
        إذا self.cur_token.ډول == Token_ډول.LParen:
            # If we see a (, it's a product
            أرجع self.parse_حاصل_ضرب()
        وإلا:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.سمون(Token_ډول.ConstructorId),
                                   self._parse_optional_fields())]
            طالما self.cur_token.ډول  == Token_ډول.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.سمون(Token_ډول.ConstructorId),
                                self._parse_optional_fields()))
            أرجع مجموعه(sumlist, self.parse_optional_ځانګړتیاوې())

    تعریف parse_حاصل_ضرب(self):
        أرجع حاصل_ضرب(self._parse_fields(), self.parse_optional_ځانګړتیاوې())

    تعریف _parse_fields(self):
        fields = []
        self.سمون(Token_ډول.LParen)
        طالما self.cur_token.ډول == Token_ډول.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_ډګر_quantifier()
            id = (self._advance() إذا self.cur_token.ډول في self._id_kinds
                                  وإلا هیڅ)
            fields.append(ډګر(typename, id, quantifiers=quantifiers))
            إذا self.cur_token.ډول == Token_ډول.RParen:
                اكسر
            وإلاإذا self.cur_token.ډول == Token_ډول.Comma:
                self._advance()
        self.سمون(Token_ډول.RParen)
        أرجع fields

    تعریف _parse_optional_fields(self):
        إذا self.cur_token.ډول == Token_ډول.LParen:
            أرجع self._parse_fields()
        وإلا:
            أرجع هیڅ

    تعریف parse_optional_ځانګړتیاوې(self):
        إذا self._at_keyword('attributes'):
            self._advance()
            أرجع self._parse_fields()
        وإلا:
            أرجع هیڅ

    تعریف parse_optional_ډګر_quantifier(self):
        quantifiers = []
        طالما self.cur_token.ډول في (Token_ډول.Asterisk, Token_ډول.Question):
            إذا self.cur_token.ډول == Token_ډول.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            وإلاإذا self.cur_token.ډول == Token_ډول.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        أرجع quantifiers

    تعریف _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = هیڅ إذا self.cur_token دی هیڅ وإلا self.cur_token.ارزښت
        حاول:
            self.cur_token = بل(self._tokenizer)
        استثناء StopIteration:
            self.cur_token = هیڅ
        أرجع cur_val

    _id_kinds = (Token_ډول.ConstructorId, Token_ډول.TypeId)

    تعریف سمون(self, ډول):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        إذا (isinstance(ډول, tuple) و self.cur_token.ډول في ډول أو
            self.cur_token.ډول == ډول
            ):
            ارزښت = self.cur_token.ارزښت
            self._advance()
            أرجع ارزښت
        وإلا:
            ارفع ASDLSyntaxError(
                'Unmatched {} (found {})'.format(ډول, self.cur_token.ډول),
                self.cur_token.lineno)

    تعریف _at_keyword(self, keyword):
        أرجع (self.cur_token.ډول == Token_ډول.TypeId و
                self.cur_token.ارزښت == keyword)
