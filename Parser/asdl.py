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
तः collections आनयन namedtuple
आनयन enum
आनयन re

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
    परिभाषा __repr__(self):
        उत्थापय NotImplementedError

वर्ग Module(AST):
    परिभाषा __init__(self, name, dfns):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.मानम् प्रति type अन्तः dfns}

    परिभाषा __repr__(self):
        प्रत्यागम 'Module({0.name}, {0.dfns})'.format(self)

वर्ग Type(AST):
    परिभाषा __init__(self, name, मानम्):
        self.name = name
        self.मानम् = मानम्

    परिभाषा __repr__(self):
        प्रत्यागम 'Type({0.name}, {0.value})'.format(self)

वर्ग Constructor(AST):
    परिभाषा __init__(self, name, fields=किमपि_न):
        self.name = name
        self.fields = fields वा []

    परिभाषा __repr__(self):
        प्रत्यागम 'Constructor({0.name}, {0.fields})'.format(self)

वर्ग Quantifier(enum.Enum):
    OPTIONAL = enum.auto()
    SEQUENCE = enum.auto()

वर्ग क्षेत्रम्(AST):
    परिभाषा __init__(self, type, name=किमपि_न, quantifiers=किमपि_न):
        self.type = type
        self.name = name
        self.seq = असत्य
        self.opt = असत्य
        self.quantifiers = quantifiers वा []
        यदि दीर्घता(self.quantifiers) > 0:
            self.seq = self.quantifiers[-1] अस्ति Quantifier.SEQUENCE
            self.opt = self.quantifiers[-1] अस्ति Quantifier.OPTIONAL

    परिभाषा __str__(self):
        extra = ""
        प्रति mod अन्तः self.quantifiers:
            यदि mod अस्ति Quantifier.SEQUENCE:
                extra += "*"
            नोचेद्यदि mod अस्ति Quantifier.OPTIONAL:
                extra += "?"

        प्रत्यागम "{}{} {}".format(self.type, extra, self.name)

    परिभाषा __repr__(self):
        यदि self.quantifiers:
            texts = []
            प्रति mod अन्तः self.quantifiers:
                यदि mod अस्ति Quantifier.SEQUENCE:
                    texts.append("SEQUENCE")
                नोचेद्यदि mod अस्ति Quantifier.OPTIONAL:
                    texts.append("OPTIONAL")
            extra = ", quantifiers=[{}]".format(", ".join(texts))
        अन्यथा:
            extra = ""

        यदि self.name अस्ति किमपि_न:
            प्रत्यागम 'Field({0.type}{1})'.format(self, extra)
        अन्यथा:
            प्रत्यागम 'Field({0.type}, {0.name}{1})'.format(self, extra)

वर्ग योग_2(AST):
    परिभाषा __init__(self, types, गुणाः=किमपि_न):
        self.types = types
        self.गुणाः = गुणाः वा []

    परिभाषा __repr__(self):
        यदि self.गुणाः:
            प्रत्यागम 'Sum({0.types}, {0.attributes})'.format(self)
        अन्यथा:
            प्रत्यागम 'Sum({0.types})'.format(self)

वर्ग गुणनफलम्(AST):
    परिभाषा __init__(self, fields, गुणाः=किमपि_न):
        self.fields = fields
        self.गुणाः = गुणाः वा []

    परिभाषा __repr__(self):
        यदि self.गुणाः:
            प्रत्यागम 'Product({0.fields}, {0.attributes})'.format(self)
        अन्यथा:
            प्रत्यागम 'Product({0.fields})'.format(self)

# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.

वर्ग Visitor_आधारः(object):
    """Generic tree visitor for ASTs."""
    परिभाषा __init__(self):
        self.cache = {}

    परिभाषा दर्शय(self, obj, *तर्काः):
        klass = obj.__class__
        meth = self.cache.get(klass)
        यदि meth अस्ति किमपि_न:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, किमपि_न)
            self.cache[klass] = meth
        यदि meth:
            यत्न:
                meth(obj, *तर्काः)
            अपवाद Exception इव e:
                मुद्रय("Error visiting %r: %s" % (obj, e))
                उत्थापय

वर्ग Check(Visitor_आधारः):
    """A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    परिभाषा __init__(self):
        पर(Check, self).__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    परिभाषा दर्शय_Module(self, mod):
        प्रति dfn अन्तः mod.dfns:
            self.दर्शय(dfn)

    परिभाषा दर्शय_Type(self, type):
        self.दर्शय(type.मानम्, str(type.name))

    परिभाषा दर्शय_योग(self, योग_3, name):
        प्रति t अन्तः योग_3.types:
            self.दर्शय(t, name)

    परिभाषा दर्शय_Constructor(self, cons, name):
        कुञ्चिका = str(cons.name)
        conflict = self.cons.get(कुञ्चिका)
        यदि conflict अस्ति किमपि_न:
            self.cons[कुञ्चिका] = name
        अन्यथा:
            मुद्रय('Redefinition of constructor {}'.format(कुञ्चिका))
            मुद्रय('Defined in {} and {}'.format(conflict, name))
            self.errors += 1
        प्रति f अन्तः cons.fields:
            self.दर्शय(f, कुञ्चिका)

    परिभाषा दर्शय_क्षेत्रम्(self, क्षेत्रम्_2, name):
        कुञ्चिका = str(क्षेत्रम्_2.type)
        l = self.types.setdefault(कुञ्चिका, [])
        l.append(name)

    परिभाषा दर्शय_गुणनफलम्(self, prod, name):
        प्रति f अन्तः prod.fields:
            self.दर्शय(f, name)

परिभाषा check(mod):
    """Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    v = Check()
    v.दर्शय(mod)

    प्रति t अन्तः v.types:
        यदि t न अन्तः mod.types च न t अन्तः builtin_types:
            v.errors += 1
            उपयुङ्क्ते = ", ".join(v.types[t])
            मुद्रय('Undefined type {}, used in {}'.format(t, उपयुङ्क्ते))
    प्रत्यागम न v.errors

# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.

परिभाषा parse(filename):
    """Parse ASDL from the given file and return a Module node describing it."""
    सह open(filename, encoding="utf-8") इव f:
        parser = ASDLParser()
        प्रत्यागम parser.parse(f.पठ())

# Types for describing tokens in an ASDL specification.
वर्ग Token_विधा:
    """TokenKind is provides a scope for enumerated token kinds."""
    (ConstructorId, TypeId, Equals, Comma, Question, Pipe, Asterisk,
     LParen, RParen, LBrace, RBrace) = परिसरः(11)

    operator_सारणी = {
        '=': Equals, ',': Comma,    '?': Question, '|': Pipe,    '(': LParen,
        ')': RParen, '*': Asterisk, '{': LBrace,   '}': RBrace}

Token = namedtuple('Token', 'kind value lineno')

वर्ग ASDLSyntaxError(Exception):
    परिभाषा __init__(self, सन्देशः, lineno=किमपि_न):
        self.सन्देशः = सन्देशः
        self.lineno = lineno वा '<unknown>'

    परिभाषा __str__(self):
        प्रत्यागम 'Syntax error on line {0.lineno}: {0.msg}'.format(self)

परिभाषा tokenize_asdl(buf):
    """Tokenize the given buffer. Yield Token objects."""
    प्रति lineno, पङ्क्तिः अन्तः enumerate(buf.splitlines(), 1):
        प्रति m अन्तः re.finditer(r'\s*(\w+|--.*|.)', पङ्क्तिः.strip()):
            c = m.गणः(1)
            यदि c[0].isalpha():
                # Some kind of identifier
                यदि c[0].isupper():
                    दा Token(Token_विधा.ConstructorId, c, lineno)
                अन्यथा:
                    दा Token(Token_विधा.TypeId, c, lineno)
            नोचेद्यदि c[:2] == '--':
                # Comment
                भङ्ग
            अन्यथा:
                # Operators
                यत्न:
                    op_विधा = Token_विधा.operator_सारणी[c]
                अपवाद कुञ्चिका_Error:
                    उत्थापय ASDLSyntaxError('Invalid operator %s' % c, lineno)
                दा Token(op_विधा, c, lineno)

वर्ग ASDLParser:
    """Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    परिभाषा __init__(self):
        self._tokenizer = किमपि_न
        self.cur_token = किमपि_न

    परिभाषा parse(self, buf):
        """Parse the ASDL in the buffer and return an AST with a Module root.
        """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        प्रत्यागम self._parse_module()

    परिभाषा _parse_module(self):
        यदि self._at_keyword('module'):
            self._advance()
        अन्यथा:
            उत्थापय ASDLSyntaxError(
                'Expected "module" (found {})'.format(self.cur_token.मानम्),
                self.cur_token.lineno)
        name = self.मेलनम्(self._id_kinds)
        self.मेलनम्(Token_विधा.LBrace)
        defs = self._parse_definitions()
        self.मेलनम्(Token_विधा.RBrace)
        प्रत्यागम Module(name, defs)

    परिभाषा _parse_definitions(self):
        defs = []
        यावत् self.cur_token.विधा == Token_विधा.TypeId:
            typename = self._advance()
            self.मेलनम्(Token_विधा.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        प्रत्यागम defs

    परिभाषा _parse_type(self):
        यदि self.cur_token.विधा == Token_विधा.LParen:
            # If we see a (, it's a product
            प्रत्यागम self.parse_गुणनफलम्()
        अन्यथा:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(self.मेलनम्(Token_विधा.ConstructorId),
                                   self._parse_optional_fields())]
            यावत् self.cur_token.विधा  == Token_विधा.Pipe:
                # More constructors
                self._advance()
                sumlist.append(Constructor(
                                self.मेलनम्(Token_विधा.ConstructorId),
                                self._parse_optional_fields()))
            प्रत्यागम योग_2(sumlist, self.parse_optional_गुणाः())

    परिभाषा parse_गुणनफलम्(self):
        प्रत्यागम गुणनफलम्(self._parse_fields(), self.parse_optional_गुणाः())

    परिभाषा _parse_fields(self):
        fields = []
        self.मेलनम्(Token_विधा.LParen)
        यावत् self.cur_token.विधा == Token_विधा.TypeId:
            typename = self._advance()
            quantifiers = self.parse_optional_क्षेत्रम्_quantifier()
            id = (self._advance() यदि self.cur_token.विधा अन्तः self._id_kinds
                                  अन्यथा किमपि_न)
            fields.append(क्षेत्रम्(typename, id, quantifiers=quantifiers))
            यदि self.cur_token.विधा == Token_विधा.RParen:
                भङ्ग
            नोचेद्यदि self.cur_token.विधा == Token_विधा.Comma:
                self._advance()
        self.मेलनम्(Token_विधा.RParen)
        प्रत्यागम fields

    परिभाषा _parse_optional_fields(self):
        यदि self.cur_token.विधा == Token_विधा.LParen:
            प्रत्यागम self._parse_fields()
        अन्यथा:
            प्रत्यागम किमपि_न

    परिभाषा parse_optional_गुणाः(self):
        यदि self._at_keyword('attributes'):
            self._advance()
            प्रत्यागम self._parse_fields()
        अन्यथा:
            प्रत्यागम किमपि_न

    परिभाषा parse_optional_क्षेत्रम्_quantifier(self):
        quantifiers = []
        यावत् self.cur_token.विधा अन्तः (Token_विधा.Asterisk, Token_विधा.Question):
            यदि self.cur_token.विधा == Token_विधा.Asterisk:
                quantifiers.append(Quantifier.SEQUENCE)
            नोचेद्यदि self.cur_token.विधा == Token_विधा.Question:
                quantifiers.append(Quantifier.OPTIONAL)
            self._advance()
        प्रत्यागम quantifiers

    परिभाषा _advance(self):
        """ Return the value of the current token and read the next one into
            self.cur_token.
        """
        cur_val = किमपि_न यदि self.cur_token अस्ति किमपि_न अन्यथा self.cur_token.मानम्
        यत्न:
            self.cur_token = अग्र(self._tokenizer)
        अपवाद StopIteration:
            self.cur_token = किमपि_न
        प्रत्यागम cur_val

    _id_kinds = (Token_विधा.ConstructorId, Token_विधा.TypeId)

    परिभाषा मेलनम्(self, विधा):
        """The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        यदि (isinstance(विधा, tuple) च self.cur_token.विधा अन्तः विधा वा
            self.cur_token.विधा == विधा
            ):
            मानम् = self.cur_token.मानम्
            self._advance()
            प्रत्यागम मानम्
        अन्यथा:
            उत्थापय ASDLSyntaxError(
                'Unmatched {} (found {})'.format(विधा, self.cur_token.विधा),
                self.cur_token.lineno)

    परिभाषा _at_keyword(self, keyword):
        प्रत्यागम (self.cur_token.विधा == Token_विधा.TypeId च
                self.cur_token.मानम् == keyword)
