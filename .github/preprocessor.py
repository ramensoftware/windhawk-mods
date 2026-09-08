"""A minimal C preprocessor, enough to tell which parts of a mod are compiled
for a given architecture.

Only the conditional directives and macro definitions are interpreted, and only
as far as the macros the caller declares allow. A condition which depends on
anything else - most often a macro that comes from a header - can't be evaluated,
and rather than picking a branch, preprocess_conditionals leaves such a group
exactly as it is, directives included, and reports every line it governs as
unresolved. A caller is expected to reject the input if it needs to read any of
those lines, instead of working with a result that was guessed at. The same
applies to macro expansion: a macro defined inside an unresolved group, or
defined in a way this module doesn't implement, raises when it is used.

A condition is evaluated with the signed 64 bit arithmetic of a real
preprocessor. What is outside of that - an unsigned operand, a result which
doesn't fit, a shift nobody has defined - is treated as one more thing which
can't be evaluated, again leaving the group unresolved.

Comment removal and string literal handling live here too, because both have to
agree with the directive scanner about where the code actually is: a directive
inside a comment is not a directive, a "//" inside a string literal doesn't start
one, and a quote which begins nothing - a digit separator, an apostrophe in prose
- has to be recognized as such, or everything after it is read as the wrong kind
of text. The bracket and separator scanning built on top of that is here for the
same reason, so that a brace or a comma inside a literal isn't taken for one in
the code.
"""

import re
from collections.abc import Iterable
from dataclasses import dataclass, field
from enum import Enum, auto

_MAX_MACRO_EXPANSION_ROUNDS = 10


class MacroState(Enum):
    # Known not to be defined at this point.
    UNDEFINED = auto()
    # Defined, or not, depending on a conditional which wasn't resolved.
    AMBIGUOUS = auto()
    # Defined in a way which isn't supported, such as with stringification.
    UNSUPPORTED = auto()


@dataclass
class FunctionLikeMacro:
    parameters: list[str]
    body: str


# A macro is either an object-like definition string, a function-like macro or
# one of the states above.
type Macros = dict[str, str | FunctionLikeMacro | MacroState]


_RAW_STRING_PREFIX_PATTERN = re.compile(r'(?:u8|u|U|L)?R"([^()\\ \t\n]{0,16})\(')

# A single string literal, raw form first so that it wins over the ordinary form.
_STRING_LITERAL_PATTERN = re.compile(
    r'(?:u8|u|U|L)?R"([^()\\ \t\n]{0,16})\((.*?)\)\1"'
    r'|(?:u8|u|U|L)?"((?:\\.|[^"\\\n])*)"',
    re.DOTALL)

# A single character literal. Multi character and prefixed forms are matched too,
# since all that is wanted here is where the literal ends.
_CHARACTER_LITERAL_PATTERN = re.compile(r"(?:u8|u|U|L)?'(?:\\.|[^'\\\n])+'")

_IDENTIFIER_PATTERN = re.compile(r'[A-Za-z_]\w*')


def _get_string_literal_value(match: re.Match):
    return match.group(2) if match.group(2) is not None else match.group(3)


def _match_character_literal(code: str, position: int):
    # A quote which follows an identifier character is a digit separator, as in
    # 1'000. A prefixed literal such as L'a' is matched at its prefix, so the
    # quote of a literal is never the character examined here.
    previous = code[position - 1:position]
    if previous.isalnum() or previous == '_':
        return None

    return _CHARACTER_LITERAL_PATTERN.match(code, position)


def _match_literal(code: str, position: int):
    """The string or character literal which starts at the position, if any.

    An unterminated literal doesn't match, so a quote which isn't a literal at
    all - a digit separator, or an apostrophe in prose - is left to be treated as
    an ordinary character instead of swallowing the code after it.
    """
    return (_STRING_LITERAL_PATTERN.match(code, position)
            or _match_character_literal(code, position))


def _find_string_literal(code: str, position: int):
    """The first string literal at or after the position.

    Character literals are stepped over, so a quote inside one is never taken for
    the start of a string.
    """
    while position < len(code):
        if character := _match_character_literal(code, position):
            position = character.end()
            continue

        if match := _STRING_LITERAL_PATTERN.match(code, position):
            return match

        position += 1

    return None


def _iter_string_literals(code: str):
    position = 0

    while match := _find_string_literal(code, position):
        yield match
        position = match.end()


def get_string_literals(code: str):
    """The values of the string literals in the code, in order."""
    return [_get_string_literal_value(x) for x in _iter_string_literals(code)]


def get_string_literal_value(code: str):
    """The value of the code if it is a single string literal, otherwise None."""
    match = _STRING_LITERAL_PATTERN.fullmatch(code.strip())
    return _get_string_literal_value(match) if match else None


def _iter_code_characters(code: str):
    """The characters of the code as (position, character, bracket depth).

    Literals are stepped over, so a bracket inside one is never counted. A
    bracket is reported at the depth within itself, which is the depth its
    closing counterpart is reported at as well.

    Angle brackets aren't counted: a less than sign and the start of a template
    argument list can't be told apart without knowing what the names mean, so
    what is inside template arguments counts as being outside any bracket.
    """
    depth = 0
    position = 0

    while position < len(code):
        if match := _match_literal(code, position):
            position = match.end()
            continue

        character = code[position]
        if character in '([{':
            depth += 1
            yield position, character, depth
        elif character in ')]}':
            yield position, character, depth
            depth -= 1
        else:
            yield position, character, depth

        position += 1


def iter_top_level_characters(code: str):
    """The characters of the code which no bracket group or literal encloses."""
    return ((position, character)
            for position, character, depth in _iter_code_characters(code)
            if depth == 0)


def find_bracket_group_end(code: str, position: int):
    """The index just past the bracket which closes the group at the position.

    None if the group isn't closed. The position is expected to hold an opening
    bracket.
    """
    for offset, character, depth in _iter_code_characters(code[position:]):
        if character in ')]}' and depth == 1:
            return position + offset + 1

    return None


def split_top_level(code: str, separator: str):
    """Split the code on separators which no bracket group or literal encloses."""
    parts = []
    start = 0

    for position, character in iter_top_level_characters(code):
        if character == separator:
            parts.append(code[start:position])
            start = position + 1

    parts.append(code[start:])
    return parts


def blank_comments(code: str):
    """Replace comments with spaces, leaving every other character in place.

    Offsets and line numbers are preserved, so positions in the result can be
    used to index into the original code.
    """
    out = list(code)
    position = 0

    while position < len(code):
        # Skip over literals so that their contents are never mistaken for the
        # start of a comment.
        if raw_string := _RAW_STRING_PREFIX_PATTERN.match(code, position):
            terminator = f'){raw_string.group(1)}"'
            end = code.find(terminator, raw_string.end())
            position = len(code) if end < 0 else end + len(terminator)
            continue

        if literal := _match_literal(code, position):
            position = literal.end()
            continue

        character = code[position]

        if character == '/' and code[position + 1:position + 2] == '/':
            while position < len(code):
                # An escaped newline continues the comment onto the next line.
                if (code[position] == '\\'
                        and code[position + 1:position + 2] == '\n'):
                    out[position] = ' '
                    position += 2
                    continue
                if code[position] == '\n':
                    break
                out[position] = ' '
                position += 1
            continue

        if character == '/' and code[position + 1:position + 2] == '*':
            end = code.find('*/', position + 2)
            end = len(code) if end < 0 else end + 2
            for index in range(position, end):
                if out[index] != '\n':
                    out[index] = ' '
            position = end
            continue

        position += 1

    return ''.join(out)


class _UnsupportedExpression(Exception):
    pass


_EXPRESSION_TOKEN_PATTERN = re.compile(
    r"""
      (?P<number>0[xX][0-9a-fA-F]+|\d+)(?P<suffix>[uUlL]*)
    | (?P<name>[A-Za-z_]\w*)
    | (?P<character>'(?:\\.|[^'\\])')
    | (?P<operator>\|\||&&|<<|>>|<=|>=|==|!=|[-+*/%!~^&|()<>])
    | (?P<space>[ \t\r]+)
    """,
    re.VERBOSE)

# A conditional expression is evaluated in intmax_t, and a result which doesn't
# fit is undefined behaviour rather than a wider value.
_INTMAX_MIN = -(1 << 63)
_INTMAX_MAX = (1 << 63) - 1

# From the lowest to the highest precedence.
_BINARY_OPERATOR_LEVELS = (
    ('||',),
    ('&&',),
    ('|',),
    ('^',),
    ('&',),
    ('==', '!='),
    ('<=', '>=', '<', '>'),
    ('<<', '>>'),
    ('+', '-'),
    ('*', '/', '%'),
)


def _truncating_division(left: int, right: int):
    """Integer division which truncates towards zero, as C does."""
    quotient = abs(left) // abs(right)
    return -quotient if (left < 0) != (right < 0) else quotient


_BINARY_OPERATIONS = {
    '|': lambda x, y: x | y,
    '^': lambda x, y: x ^ y,
    '&': lambda x, y: x & y,
    '==': lambda x, y: int(x == y),
    '!=': lambda x, y: int(x != y),
    '<=': lambda x, y: int(x <= y),
    '>=': lambda x, y: int(x >= y),
    '<': lambda x, y: int(x < y),
    '>': lambda x, y: int(x > y),
    '<<': lambda x, y: x << y,
    '>>': lambda x, y: x >> y,
    '+': lambda x, y: x + y,
    '-': lambda x, y: x - y,
    '*': lambda x, y: x * y,
    '/': _truncating_division,
    '%': lambda x, y: x - _truncating_division(x, y) * y,
}


def _macro_is_defined(name: str, macros: Macros):
    """Whether a macro is defined, or None if that can't be determined."""
    state = macros.get(name)
    if state is None or state is MacroState.AMBIGUOUS:
        return None

    return state is not MacroState.UNDEFINED


def _tokenize_expression(expression: str):
    tokens = []
    position = 0

    while position < len(expression):
        match = _EXPRESSION_TOKEN_PATTERN.match(expression, position)
        if not match:
            raise _UnsupportedExpression(expression)

        position = match.end()

        # Unsigned arithmetic isn't implemented, and it isn't only a matter of
        # the range: a single unsigned operand makes a comparison unsigned too.
        suffix = match.group('suffix')
        if suffix and 'u' in suffix.lower():
            raise _UnsupportedExpression(expression)

        for kind in ('number', 'name', 'character', 'operator'):
            if match.group(kind) is not None:
                tokens.append((kind, match.group(kind)))
                break

    return tokens


def _in_intmax_range(value: int):
    return _INTMAX_MIN <= value <= _INTMAX_MAX


def _parse_number(text: str):
    """The value of an integer literal, where a leading zero means octal."""
    base = 8 if text[0] == '0' and len(text) > 1 and text[1] not in 'xX' else 0

    try:
        return int(text, base)
    except ValueError:
        raise _UnsupportedExpression(text)


def _apply_binary_operator(operator: str, left, right):
    # && and || can decide the result even when one side is unknown.
    if operator == '&&':
        if left == 0 or right == 0:
            return 0
        return None if left is None or right is None else 1

    if operator == '||':
        if (left is not None and left != 0) or (right is not None and right != 0):
            return 1
        return None if left is None or right is None else 0

    if left is None or right is None:
        return None

    if operator in ('/', '%') and right == 0:
        return None

    # A shift by a negative or too large a count is undefined behaviour, and the
    # result of one is anybody's guess.
    if operator in ('<<', '>>') and not 0 <= right < 64:
        return None

    value = _BINARY_OPERATIONS[operator](left, right)
    return value if _in_intmax_range(value) else None


def _apply_unary_operator(operator: str, value):
    if value is None:
        return None

    if operator == '!':
        return int(value == 0)
    if operator == '-':
        return -value if _in_intmax_range(-value) else None
    if operator == '+':
        return value

    return ~value


def _is_single_operand(tokens: list[tuple[str, str]]):
    """Whether the tokens bind as one operand wherever they are substituted.

    A single token does, and so does a parenthesized expression. Anything else,
    such as 1+2, would bind differently depending on the operators around it, so
    evaluating it on its own would give the wrong answer for 1+2*3.
    """
    if len(tokens) == 1:
        return True

    if tokens[0] != ('operator', '('):
        return False

    depth = 0
    for index, token in enumerate(tokens):
        if token == ('operator', '('):
            depth += 1
        elif token == ('operator', ')'):
            depth -= 1
            if depth == 0:
                return index == len(tokens) - 1

    return False


class _ExpressionParser:
    """A #if expression parser where an unknown macro yields None.

    None propagates through the operators as "can't be determined", except for
    && and || which can still decide the result from the other operand.
    """

    def __init__(self, tokens: list[tuple[str, str]], macros: Macros,
                 expanding: frozenset[str]):
        self.tokens = tokens
        self.macros = macros
        self.expanding = expanding
        self.position = 0

    def peek(self):
        if self.position >= len(self.tokens):
            return None

        return self.tokens[self.position]

    def take(self):
        token = self.peek()
        if token is None:
            raise _UnsupportedExpression('unexpected end of expression')

        self.position += 1
        return token

    def expect(self, operator: str):
        if self.take() != ('operator', operator):
            raise _UnsupportedExpression(f'expected {operator}')

    def parse(self):
        value = self.parse_binary(0)
        if self.peek() is not None:
            raise _UnsupportedExpression('trailing tokens')

        return value

    def parse_binary(self, level: int):
        if level == len(_BINARY_OPERATOR_LEVELS):
            return self.parse_unary()

        value = self.parse_binary(level + 1)

        while (token := self.peek()) is not None:
            if token[0] != 'operator' or token[1] not in _BINARY_OPERATOR_LEVELS[level]:
                break
            self.take()
            value = _apply_binary_operator(token[1], value,
                                           self.parse_binary(level + 1))

        return value

    def parse_unary(self):
        token = self.peek()
        if token is not None and token[0] == 'operator' and token[1] in '!-+~':
            self.take()
            return _apply_unary_operator(token[1], self.parse_unary())

        return self.parse_primary()

    def parse_primary(self):
        kind, text = self.take()

        if (kind, text) == ('operator', '('):
            value = self.parse_binary(0)
            self.expect(')')
            return value

        if kind == 'number':
            value = _parse_number(text)
            return value if _in_intmax_range(value) else None

        if kind == 'character':
            value = text[1:-1]
            return ord(value) if len(value) == 1 else None

        if kind == 'name':
            return self.parse_name(text)

        raise _UnsupportedExpression(text)

    def parse_name(self, name: str):
        if name == 'defined':
            return self.parse_defined()

        # A call, such as __has_include(...), isn't evaluated, but it still has
        # to be consumed so that the rest of the expression parses.
        if self.peek() == ('operator', '('):
            self.skip_parentheses()
            return None

        state = self.macros.get(name)
        if state is MacroState.UNDEFINED:
            return 0

        # A macro which expands to itself would otherwise recurse forever.
        if not isinstance(state, str) or name in self.expanding:
            return None

        # The name stands for the tokens of its definition, so the definition can
        # only be evaluated on its own where the surrounding expression can't
        # split it: it has to be an operand in itself, or be the whole expression.
        whole_expression = self.position == 1 and len(self.tokens) == 1

        try:
            tokens = _tokenize_expression(state)
            if not tokens:
                return None
            if not whole_expression and not _is_single_operand(tokens):
                return None
            return _ExpressionParser(tokens, self.macros,
                                     self.expanding | {name}).parse()
        except _UnsupportedExpression:
            return None

    def parse_defined(self):
        kind, text = self.take()

        if (kind, text) == ('operator', '('):
            kind, text = self.take()
            if kind != 'name':
                raise _UnsupportedExpression('defined')
            self.expect(')')
        elif kind != 'name':
            raise _UnsupportedExpression('defined')

        defined = _macro_is_defined(text, self.macros)
        return None if defined is None else int(defined)

    def skip_parentheses(self):
        self.expect('(')
        depth = 1
        while depth > 0:
            token = self.take()
            if token == ('operator', '('):
                depth += 1
            elif token == ('operator', ')'):
                depth -= 1


def _evaluate_expression(expression: str, macros: Macros):
    """Evaluate a #if expression, or return None if that can't be done."""
    try:
        tokens = _tokenize_expression(expression)
        if not tokens:
            return None
        return _ExpressionParser(tokens, macros, frozenset()).parse()
    except _UnsupportedExpression:
        return None


_DIRECTIVE_PATTERN = re.compile(r'^[ \t]*#[ \t]*(\w+)\b[ \t]*(.*)$')

_CONDITIONAL_DIRECTIVES = ('if', 'ifdef', 'ifndef', 'elif', 'else', 'endif')

_DEFINE_PATTERN = re.compile(r'(\w+)(\([^)]*\))?[ \t]*(.*)')

# Returned by _select_conditional_branch when the group has a condition which
# can't be evaluated.
_UNRESOLVED = object()


@dataclass
class _ConditionalBranch:
    # 'if', 'ifdef', 'ifndef', 'elif' or 'else'.
    kind: str
    expression: str
    directive_lines: tuple[int, int]
    body: list = field(default_factory=list)


@dataclass
class _ConditionalGroup:
    branches: list[_ConditionalBranch] = field(default_factory=list)
    endif_lines: tuple[int, int] = (0, 0)


@dataclass
class PreprocessedSource:
    # The source with the branches which aren't built blanked out. Line numbers
    # are preserved, so positions in it index into the original source.
    source: str
    # The macros the source defines, on top of the ones the caller gave.
    macros: Macros
    # The lines whose presence depends on a condition which couldn't be
    # evaluated, mapped to the directive which governs them. Those directives are
    # left in place as well. A caller which reads anything from these lines is
    # reading something this module couldn't decide, and is expected to reject
    # the input.
    unresolved_lines: dict[int, str]


def _lines_inside_literals(blanked_source: str):
    """The lines which start inside a string literal.

    A # on such a line is part of the literal rather than a directive.
    """
    # A literal reaches the next line only as a raw string or through an escaped
    # newline, so without either there is nothing to look for.
    if 'R"' not in blanked_source and '\\\n' not in blanked_source:
        return frozenset()

    inside = set()

    for match in _iter_string_literals(blanked_source):
        if '\n' not in match.group(0):
            continue

        first = blanked_source.count('\n', 0, match.start())
        inside.update(range(first + 1, first + 1 + match.group(0).count('\n')))

    return inside


def _split_directive_lines(blanked_source: str):
    """Split code into text lines and preprocessor directives.

    A line continued with a trailing backslash becomes a single item spanning all
    of its lines, whether it is a directive or not: a # which lands in the middle
    of a spliced line isn't a directive.
    """
    lines = blanked_source.split('\n')
    inside_literals = _lines_inside_literals(blanked_source)
    items = []
    index = 0

    while index < len(lines):
        match = _DIRECTIVE_PATTERN.match(lines[index])
        if not match or index in inside_literals:
            first = index
            while (lines[index].rstrip().endswith('\\')
                   and index + 1 < len(lines)):
                index += 1
            items.append(('text', first, index))
            index += 1
            continue

        first = index
        parts = [match.group(2)]
        while parts[-1].rstrip().endswith('\\') and index + 1 < len(lines):
            parts[-1] = parts[-1].rstrip()[:-1]
            index += 1
            parts.append(lines[index])

        items.append(('directive', match.group(1), ' '.join(parts), first, index))
        index += 1

    return items


def _parse_conditional_groups(items: list):
    """Nest the flat directive list into conditional groups."""
    root = []
    bodies = [root]
    groups: list[_ConditionalGroup] = []

    for item in items:
        if item[0] != 'directive' or item[1] not in _CONDITIONAL_DIRECTIVES:
            bodies[-1].append(item)
            continue

        _, kind, expression, first, last = item

        if kind in ('if', 'ifdef', 'ifndef'):
            group = _ConditionalGroup()
            group.branches.append(_ConditionalBranch(kind, expression, (first, last)))
            bodies[-1].append(group)
            groups.append(group)
            bodies.append(group.branches[-1].body)
            continue

        if not groups:
            raise Exception(f'Unbalanced preprocessor directive: #{kind}')

        group = groups[-1]
        bodies.pop()

        if kind == 'endif':
            group.endif_lines = (first, last)
            groups.pop()
            continue

        if group.branches[-1].kind == 'else':
            raise Exception(f'Preprocessor directive #{kind} after #else')

        group.branches.append(_ConditionalBranch(kind, expression, (first, last)))
        bodies.append(group.branches[-1].body)

    if groups:
        raise Exception('Unterminated preprocessor conditional')

    return root


def _evaluate_branch_condition(branch: _ConditionalBranch, macros: Macros):
    if branch.kind in ('ifdef', 'ifndef'):
        name = branch.expression.strip()
        if not re.fullmatch(r'\w+', name):
            return None

        defined = _macro_is_defined(name, macros)
        if defined is None or branch.kind == 'ifdef':
            return defined

        return not defined

    value = _evaluate_expression(branch.expression, macros)
    return None if value is None else value != 0


def _select_conditional_branch(group: _ConditionalGroup, macros: Macros):
    """The index of the branch which is taken.

    Returns None when no branch is taken, or _UNRESOLVED when a condition can't
    be evaluated before one is found.
    """
    for index, branch in enumerate(group.branches):
        if branch.kind == 'else':
            return index

        value = _evaluate_branch_condition(branch, macros)
        if value is None:
            return _UNRESOLVED
        if value:
            return index

    return None


def _get_defined_names(nodes: list):
    names = []

    for node in nodes:
        if isinstance(node, _ConditionalGroup):
            for branch in node.branches:
                names += _get_defined_names(branch.body)
        elif node[0] == 'directive' and node[1] == 'define':
            if match := _DEFINE_PATTERN.fullmatch(node[2].strip()):
                names.append(match.group(1))

    return names


def _body_uses_hash_operator(body: str):
    """Whether the body stringifies or pastes tokens, outside of a literal."""
    position = 0

    while position < len(body):
        if literal := _match_literal(body, position):
            position = literal.end()
            continue

        if body[position] == '#':
            return True

        position += 1

    return False


def _apply_definition(directive: str, text: str, active: bool | None,
                      macros: Macros):
    match = _DEFINE_PATTERN.fullmatch(text.strip())
    if not match:
        return

    name = match.group(1)
    body = match.group(3).strip()

    if active is None:
        # The definition may or may not happen, so anything that uses the macro
        # can't be resolved either.
        macros[name] = MacroState.AMBIGUOUS
    elif directive == 'undef':
        macros[name] = MacroState.UNDEFINED
    elif _body_uses_hash_operator(body):
        # Stringification and token pasting aren't supported.
        macros[name] = MacroState.UNSUPPORTED
    elif match.group(2):
        parameters = [x.strip() for x in match.group(2)[1:-1].split(',') if x.strip()]
        # A variadic parameter, or one which isn't an identifier, isn't supported
        # either.
        if not all(re.fullmatch(r'\w+', x) for x in parameters):
            macros[name] = MacroState.UNSUPPORTED
        else:
            macros[name] = FunctionLikeMacro(parameters, body)
    else:
        macros[name] = body


@dataclass
class _EmitState:
    # Whether each line of the source is part of this build.
    keep: list[bool]
    # The lines governed by a condition which couldn't be evaluated, mapped to
    # the directive which governs them.
    unresolved: dict[int, str] = field(default_factory=dict)

    def blank(self, lines: tuple[int, int]):
        for index in range(lines[0], lines[1] + 1):
            self.keep[index] = False

    def mark_unresolved(self, lines: tuple[int, int], directive: str):
        # The outermost group wins, being the one which explains the whole span.
        for index in range(lines[0], lines[1] + 1):
            self.unresolved.setdefault(index, directive)


def _emit_nodes(nodes: list, active: bool | None, macros: Macros,
                state: _EmitState):
    """Walk the tree, dropping the lines which aren't part of this build.

    active is True inside a region which is definitely built, None inside one
    which depends on an unresolved conditional, and False inside one which is
    definitely not built.
    """
    for node in nodes:
        if isinstance(node, _ConditionalGroup):
            _emit_conditional_group(node, active, macros, state)
        elif active is False:
            state.blank((node[-2], node[-1]))
        elif node[0] == 'directive' and node[1] in ('define', 'undef'):
            _apply_definition(node[1], node[2], active, macros)


def _emit_conditional_group(group: _ConditionalGroup, active: bool | None,
                            macros: Macros, state: _EmitState):
    selected = None if active is False else _select_conditional_branch(group, macros)

    if selected is _UNRESOLVED:
        # Leave the group exactly as it is, directives included, and report every
        # line it governs, so that a caller which reads any of them rejects the
        # input rather than working with an arbitrary branch. Nested groups may
        # still be resolvable.
        opening = group.branches[0]
        state.mark_unresolved(
            (opening.directive_lines[0], group.endif_lines[1]),
            f'#{opening.kind} {opening.expression}'.rstrip())
        for branch in group.branches:
            _emit_nodes(branch.body, None, macros, state)
        return

    for branch in group.branches:
        state.blank(branch.directive_lines)
    state.blank(group.endif_lines)

    for index, branch in enumerate(group.branches):
        _emit_nodes(branch.body, active if index == selected else False, macros,
                    state)

    # The branches which aren't taken tell us that the macros they define are
    # not defined, unless something else already defined them.
    if active is True:
        for index, branch in enumerate(group.branches):
            if index != selected:
                for name in _get_defined_names(branch.body):
                    macros.setdefault(name, MacroState.UNDEFINED)


def preprocess_conditionals(source: str, defined: dict[str, str],
                            undefined: Iterable[str] = ()):
    """Resolve the conditionals which the given macros decide.

    defined are the macros which are known to be defined, with their values, and
    undefined the ones which are known not to be. Any other macro is unknown, so
    a conditional which depends on one is reported as unresolved instead of being
    resolved to an arbitrary branch.
    """
    nodes = _parse_conditional_groups(
        _split_directive_lines(blank_comments(source)))

    macros: Macros = {name: MacroState.UNDEFINED for name in undefined}
    macros.update(defined)

    lines = source.split('\n')
    state = _EmitState([True] * len(lines))
    _emit_nodes(nodes, True, macros, state)

    return PreprocessedSource(
        '\n'.join(line if state.keep[index] else ''
                  for index, line in enumerate(lines)),
        macros,
        state.unresolved)


def _split_macro_arguments(code: str, position: int):
    """Split the arguments of an invocation starting at the opening parenthesis.

    Returns the arguments and the offset just past the closing parenthesis, or
    None if the parentheses aren't balanced. Commas inside nested parentheses
    and inside literals don't separate arguments.
    """
    arguments = []
    current = []
    depth = 0

    while position < len(code):
        if literal := _match_literal(code, position):
            current.append(literal.group(0))
            position = literal.end()
            continue

        character = code[position]

        if character == '(':
            depth += 1
            if depth == 1:
                position += 1
                continue
        elif character == ')':
            depth -= 1
            if depth == 0:
                arguments.append(''.join(current).strip())
                return arguments, position + 1
        elif character == ',' and depth == 1:
            arguments.append(''.join(current).strip())
            current = []
            position += 1
            continue

        current.append(character)
        position += 1

    return None


def _substitute_macro_parameters(macro: FunctionLikeMacro, arguments: list[str]):
    replacements = dict(zip(macro.parameters, arguments))
    parts = []
    position = 0

    while position < len(macro.body):
        if literal := _match_literal(macro.body, position):
            parts.append(literal.group(0))
            position = literal.end()
            continue

        if name := _IDENTIFIER_PATTERN.match(macro.body, position):
            parts.append(replacements.get(name.group(0), name.group(0)))
            position = name.end()
            continue

        parts.append(macro.body[position])
        position += 1

    return ''.join(parts)


def _expand_macros_once(code: str, macros: Macros):
    parts = []
    position = 0
    expanded = False

    while position < len(code):
        if literal := _match_literal(code, position):
            parts.append(literal.group(0))
            position = literal.end()
            continue

        match = _IDENTIFIER_PATTERN.match(code, position)
        if not match:
            parts.append(code[position])
            position += 1
            continue

        name = match.group(0)
        state = macros.get(name)
        position = match.end()

        if state is MacroState.AMBIGUOUS:
            raise Exception(f'Macro defined under an unresolved condition: {name}')
        if state is MacroState.UNSUPPORTED:
            raise Exception(f'Unsupported macro: {name}')

        if isinstance(state, FunctionLikeMacro):
            invocation = position + len(code[position:]) - len(code[position:].lstrip())
            # Only an invocation expands a function-like macro, a bare mention
            # of the name is left as it is.
            if code[invocation:invocation + 1] != '(':
                parts.append(name)
                continue

            split = _split_macro_arguments(code, invocation)
            if split is None:
                raise Exception(f'Unterminated macro invocation: {name}')

            arguments, position = split
            if arguments == [''] and not state.parameters:
                arguments = []
            if len(arguments) != len(state.parameters):
                raise Exception(f'Wrong number of arguments for macro: {name}')

            parts.append(_substitute_macro_parameters(state, arguments))
            expanded = True
            continue

        if isinstance(state, str):
            parts.append(state)
            expanded = True
            continue

        parts.append(name)

    return ''.join(parts), expanded


def expand_macros(code: str, macros: Macros):
    """Replace macros with their definitions, leaving string literals alone."""
    for _ in range(_MAX_MACRO_EXPANSION_ROUNDS):
        code, expanded = _expand_macros_once(code, macros)
        if not expanded:
            return code

    raise Exception('Macro expansion doesn\'t terminate')


def concat_adjacent_literals(code: str):
    """Apply the rule that adjacent string literals form a single literal."""
    parts = []
    position = 0

    while match := _find_string_literal(code, position):
        parts.append(code[position:match.start()])

        values = [_get_string_literal_value(match)]
        position = match.end()
        while True:
            after_space = position + len(code[position:]) - len(code[position:].lstrip())
            adjacent = _STRING_LITERAL_PATTERN.match(code, after_space)
            if not adjacent:
                break
            values.append(_get_string_literal_value(adjacent))
            position = adjacent.end()

        value = ''.join(values)
        # Escapes aren't decoded, so a value which can't be written back as an
        # ordinary single line literal isn't supported.
        if '"' in value or '\\' in value or '\n' in value:
            raise Exception(f'Unsupported strings')

        parts.append(f'L"{value}"')

    parts.append(code[position:])
    return ''.join(parts)


def check_literals_are_fully_expanded(code: str):
    """Reject a string literal which sits next to an identifier.

    Adjacent string literals have already been merged, so an identifier next to
    one is a macro which wasn't substituted, and taking the literal on its own
    would give a truncated value.
    """
    for match in _iter_string_literals(code):
        before = code[:match.start()].rstrip()[-1:]
        after = code[match.end():].lstrip()[:1]

        for neighbour in (before, after):
            if neighbour and _IDENTIFIER_PATTERN.fullmatch(neighbour):
                raise Exception(
                    f'Unexpanded token next to a symbol string: {neighbour}')
