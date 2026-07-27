import json
import re
import time
from argparse import ArgumentParser
from enum import StrEnum, auto
from pathlib import Path

from preprocessor import (
    Macros,
    blank_comments,
    check_literals_are_fully_expanded,
    concat_adjacent_literals,
    expand_macros,
    find_bracket_group_end,
    get_string_literal_value,
    iter_top_level_characters,
    preprocess_conditionals,
    split_top_level,
)


class Architecture(StrEnum):
    x86 = auto()
    amd64 = auto()
    arm64 = auto()


MOD_PATCHES: dict[str, list[tuple[str, str]]] = {
    'acrylic-effect-radius-changer/1.1.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK symbolHooks\b',
            r'// dwmcore.dll\n\g<0>'
        ),
    ],
    'aero-tray/1.0.2.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// explorer.exe\n\g<0>'
        ),
    ],
    'change-explorer-default-location/1.0.0.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// shell32.dll\n\g<0>'
        ),
    ],
    'classic-explorer-dragdrop/1.1.wh.cpp': [
        (
            r'^#if __WIN64$',
            r'#if _WIN64'
        ),
    ],
    'classic-taskbar-buttons-lite-vs-without-spacing/1.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// explorer.exe\n\g<0>'
        ),
    ],
    'classic-taskdlg-fix/1.1.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// comctl32.dll\n\g<0>'
        ),
    ],
    'classic-uwp-fix/0.3.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// ApplicationFrame.dll\n\g<0>'
        ),
    ],
    'desktop-watermark-tweaks/1.0.0.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// shell32.dll\n\g<0>'
        ),
    ],
    'disable-rounded-corners/1.0.1.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK symbolHooks\b',
            r'// udwm.dll\n\g<0>'
        ),
    ],
    'dwm-ghost-mods/1.2.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// dwmghost.dll\n\g<0>'
        ),
    ],
    'eradicate-immersive-menus/1.1.0.wh.cpp': [
        (
            r'[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            (
                r'// shell32.dll, ExplorerFrame.dll, explorer.exe, twinui.dll,'
                r' twinui.pcshell.dll, SndVolSSO.dll, pnidui.dll,'
                r' SecurityHealthSSO.dll, Narrator.exe\n\g<0>'
            ),
        )
    ],
    'explorer-32px-icons/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// shell32.dll\n\g<0>'
        ),
    ],
    'isretailready-false/1.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// shell32.dll\n\g<0>'
        ),
    ],
    'legacy-search-bar/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// ExplorerFrame.dll\n\g<0>'
        ),
    ],
    'no-run-icon/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// shell32.dll\n\g<0>'
        ),
    ],
    'notepad-remove-launch-new-app-banner/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// notepad.exe\n\g<0>'
        ),
    ],
    'regedit-auto-trim-whitespace-on-navigation-bar/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// regedit.exe\n\g<0>'
        ),
    ],
    'regedit-disable-beep/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// regedit.exe\n\g<0>'
        ),
    ],
    'regedit-fix-copy-key-name/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// regedit.exe\n\g<0>'
        ),
    ],
    'suppress-run-box-error-message/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// shell32.dll\n\g<0>'
        ),
    ],
    'taskbar-autohide-better/1.2.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK symbolHooks\b',
            r'// taskbar.dll, explorer.exe\n\g<0>'
        ),
    ],
    'unlock-taskmgr-server/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// Taskmgr.exe\n\g<0>'
        ),
    ],
    'win32-tray-clock-experience/1.0.0.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// Taskbar.dll, explorer.exe\n\g<0>'
        ),
    ],
    'win7-style-uac-dim/1.0.1.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// consent.exe\n\g<0>'
        ),
    ],
    'windows-7-clock-spacing/1.0.0.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// explorer.exe\n\g<0>'
        ),
    ],
}

SYMBOL_MODULES_SKIP: dict[str, list[str]] = {
    # https://github.com/ramensoftware/windhawk-mods/pull/2905#issuecomment-3703476284
    'old-explorer-sysmenu-behavior': ['explorerframe.dll'],

    # Win7 only.
    'win7-alttab-loader': ['alttab.dll'],

    # Office mods, use noUndecoratedSymbols.
    'office-fix-account-disp-name': ['mso30win32client.dll'],
    'office-ui-reverter-universal': ['mso40uiwin32client.dll'],
    'word-image-resize-anti-flip': ['oart.dll'],
    'word-mathtype-dark-fix': ['wwlib.dll'],
    'word-omath-shade-fix': ['wwlib.dll'],
    'word-pdf-lossless-export': ['mso.dll'],
}

# The architecture macros mods are expected to branch on, with the values
# reported by:
#
#   clang++ -dM -E -std=c++23 -target <triple> -x c++ <file including windows.h>
#
# for the triples Windhawk compiles mods for: i686-w64-mingw32,
# x86_64-w64-mingw32 and aarch64-w64-mingw32. The _M_* macros come from the
# mingw-w64 headers, _WIN64 from clang itself.
#
# Deliberately only these spellings, not every macro a mod could branch on. The
# gcc style __x86_64__ and __aarch64__ are left out so that a mod using them
# fails to extract and gets changed to the constants above, instead of both
# spellings being accommodated here. A macro which isn't listed is unknown, so a
# conditional which depends on one isn't resolved.
ARCH_PREDEFINED_MACROS: dict[Architecture, dict[str, str]] = {
    Architecture.x86: {
        '_M_IX86': '300',
    },
    Architecture.amd64: {
        '_WIN64': '1',
        '_M_X64': '100',
    },
    Architecture.arm64: {
        '_WIN64': '1',
        '_M_ARM64': '1',
    },
}


def get_arch_macros(arch: Architecture):
    """The macros which are known to be defined, and to be undefined, for an arch.

    An architecture macro that some other architecture predefines is known not to
    be defined for this one, which is what lets a conditional on it be resolved.
    """
    defined = ARCH_PREDEFINED_MACROS[arch]
    undefined = {name for predefined in ARCH_PREDEFINED_MACROS.values()
                 for name in predefined} - defined.keys()
    return defined, undefined


def get_mod_metadata(mod_source: str):
    p = r'^\/\/[ \t]+==WindhawkMod==[ \t]*$([\s\S]+?)^\/\/[ \t]+==\/WindhawkMod==[ \t]*$'
    match = re.search(p, mod_source, flags=re.MULTILINE)
    if not match:
        raise Exception(f'No metadata block')

    metadata_block = match.group(1)

    p = r'^\/\/[ \t]+@architecture[ \t]+(.*)$'
    match = re.findall(p, metadata_block, flags=re.MULTILINE)

    architectures: set[Architecture] = set()

    for arch in (match or ['x86', 'amd64', 'arm64']):
        if arch == 'x86':
            architectures.add(Architecture.x86)
        elif arch == 'amd64':
            architectures.add(Architecture.amd64)
        elif arch == 'arm64':
            architectures.add(Architecture.arm64)
        elif arch == 'x86-64':
            # Implies both amd64 and arm64.
            architectures.add(Architecture.amd64)
            architectures.add(Architecture.arm64)
        else:
            raise Exception(f'Unknown architecture: {arch}')

    return {
        'architectures': architectures,
    }


def get_target_module_from_symbol_block_name(symbol_block_name: str):
    p = r'(.*?)_?(exe|dll|cpl)_?hooks?'
    match = re.fullmatch(p, symbol_block_name, flags=re.IGNORECASE)
    if not match:
        return None

    base_name = match.group(1)
    suffix = match.group(2)
    return f'{base_name}.{suffix}'


def get_target_modules_from_previous_line(previous_line: str):
    previous_line = previous_line.lstrip()
    if not previous_line.startswith('//'):
        return []

    comment = previous_line.removeprefix('//').strip()
    if comment == '':
        return []

    names = [x.strip() for x in comment.split(',')]
    if not all(re.search(r'\.(exe|dll|cpl)$', x) for x in names):
        return []

    return names

def deduce_symbol_block_target_modules(mod_source: str, symbol_block_match: re.Match):
    symbol_block_name = symbol_block_match.group(1)

    # Try the new rules as defined in pr_validation.py.
    target_from_name = get_target_module_from_symbol_block_name(symbol_block_name)
    if target_from_name:
        return [target_from_name.lower()]

    line_num = 1 + mod_source[: symbol_block_match.start()].count('\n')
    previous_line = mod_source.splitlines()[line_num - 2]
    targets_from_comment = get_target_modules_from_previous_line(previous_line)
    if targets_from_comment:
        return [x.lower() for x in targets_from_comment]

    raise Exception(f'Unknown module ({symbol_block_name=})')


def check_block_is_not_conditional(mod_source: str, symbol_block_match: re.Match,
                                   unresolved_lines: dict[int, str]):
    """Reject a symbol block which a condition we couldn't evaluate decides.

    Covers both a condition around the block, which decides whether it is
    compiled at all, and one inside it, which decides which of its symbols are.
    """
    first = mod_source[:symbol_block_match.start()].count('\n')
    last = first + symbol_block_match.group(0).count('\n')

    for line in range(first, last + 1):
        if directive := unresolved_lines.get(line):
            raise Exception(
                f'Symbol block under an unresolved condition: {directive}')


def one_line(code: str):
    """A short single line form of a piece of code, for an error message."""
    code = ' '.join(code.split())
    return code if len(code) <= 100 else code[:100] + '...'


def split_conditional_expression(expression: str):
    """The two branches of a conditional expression, or None if it isn't one.

    The condition is dropped, since which branch is taken is decided when the mod
    runs and both branches are therefore reachable.
    """
    question = None
    nested = 0

    for position, character in iter_top_level_characters(expression):
        if character == '?':
            if question is None:
                question = position
            else:
                nested += 1
        elif character == ':' and question is not None:
            # A colon which belongs to the scope operator is one of a pair, and
            # never the colon of a conditional.
            if ':' in (expression[position - 1], expression[position + 1:position + 2]):
                continue

            if nested == 0:
                return expression[question + 1:position], expression[position + 1:]

            nested -= 1

    return None


def get_symbol_names(expression: str):
    """The names which a symbol name expression can stand for.

    A conditional picks one of its branches when the mod runs, so each branch
    holds a name the mod can hook and all of them are taken. Anything else has to
    be a string literal: every macro has been expanded by now, so an expression
    which isn't a literal is a name this script can't read, such as a variable
    holding it, and the symbol would otherwise be left out of the cache without a
    word.
    """
    expression = expression.strip()

    # Parentheses around an expression say nothing about which names it stands
    # for, and a cast keeps its own since its parentheses don't enclose it.
    while expression.startswith('('):
        end = find_bracket_group_end(expression, 0)
        if end is None or expression[end:].strip() != '':
            break

        expression = expression[1:end - 1].strip()

    if branches := split_conditional_expression(expression):
        return [name for branch in branches for name in get_symbol_names(branch)]

    value = get_string_literal_value(expression)
    if value is None:
        raise Exception(f'Symbol name is not a string: {one_line(expression)}')

    return [value]


def get_brace_group_content(expression: str, description: str):
    """The content of an expression which is nothing but a single brace group."""
    end = find_bracket_group_end(expression, 0) if expression.startswith('{') else None
    if end is None or expression[end:].strip() != '':
        raise Exception(f'Unsupported {description}: {one_line(expression)}')

    return expression[1:end - 1]


def get_symbol_block_symbols(symbol_block: str):
    """The names of every symbol which a symbol block hooks.

    A block declares either an array of SYMBOL_HOOK entries or a single entry,
    and the first member of an entry is the braced list of names of the symbol to
    hook. A block which doesn't have that shape is rejected rather than read as
    far as it happens to be readable, so that a name this script can't make out
    is never dropped quietly.
    """
    start = symbol_block.find('{')
    end = find_bracket_group_end(symbol_block, start) if start >= 0 else None
    if end is None:
        raise Exception(f'Unsupported symbol block: {one_line(symbol_block)}')

    # A subscript in the declarator is what makes the initializer a list of
    # entries rather than the members of a single entry.
    is_array = '[' in symbol_block[:start]

    initializer = symbol_block[start + 1:end - 1]
    entries = split_top_level(initializer, ',') if is_array else [initializer]

    symbols = []

    for entry in entries:
        entry = entry.strip()
        if entry == '':
            continue

        if is_array:
            entry = get_brace_group_content(entry, 'symbol hook')

        members = split_top_level(entry, ',')
        names = get_brace_group_content(members[0].strip(), 'symbol name list')

        entry_symbols = [name
                         for expression in split_top_level(names, ',')
                         if expression.strip() != ''
                         for name in get_symbol_names(expression)]
        if entry_symbols == []:
            raise Exception(f'Symbol hook without a name: {one_line(entry)}')

        symbols += entry_symbols

    return symbols


def process_symbol_block(mod_source: str, symbol_block_match: re.Match, macros: Macros):
    symbol_block = blank_comments(symbol_block_match.group(0))

    # Every conditional directive is gone by now, either resolved or rejected,
    # so anything left here is a directive which isn't supported at all.
    p = r'^[ \t]*#.*'
    if match := re.search(p, symbol_block, flags=re.MULTILINE):
        raise Exception(f'Unsupported preprocessor directive: {match.group(0)}')

    symbol_block = expand_macros(symbol_block, macros)
    symbol_block = concat_adjacent_literals(symbol_block)
    check_literals_are_fully_expanded(symbol_block)

    symbols = get_symbol_block_symbols(symbol_block)

    # Every literal of the block is a symbol name, so a string which was given to
    # any other member is caught here instead of being passed over.
    if len(symbols) * 2 != symbol_block.count('"'):
        raise Exception(f'Unsupported strings')

    if symbols == []:
        raise Exception(f'Symbol block without symbols')

    modules = deduce_symbol_block_target_modules(mod_source, symbol_block_match)

    return {
        'symbols': symbols,
        'modules': modules,
    }


def get_mod_symbol_blocks(mod_source: str, arch: Architecture):
    defined, undefined = get_arch_macros(arch)
    preprocessed = preprocess_conditionals(mod_source, defined, undefined)
    mod_source = preprocessed.source

    # Extract symbol blocks.
    symbol_blocks = []
    p = r'^[ \t]*(?:(?:static|const)[ \t]+)*(?:WindhawkUtils::)?SYMBOL_HOOK[ \t]+(\w+)\s*[\[={][\s\S]*?\};[ \t]*$'
    for match in re.finditer(p, mod_source, flags=re.MULTILINE):
        check_block_is_not_conditional(mod_source, match,
                                       preprocessed.unresolved_lines)
        symbol_block = process_symbol_block(mod_source, match,
                                           preprocessed.macros)
        symbol_blocks.append(symbol_block)

    # Verify that no blocks were missed.
    p = r'SYMBOL_HOOK\s+\w'
    if len(symbol_blocks) != len(
        re.findall(p, blank_comments(mod_source), flags=re.MULTILINE)
    ):
        raise Exception(f'Unsupported symbol blocks')

    return symbol_blocks


def get_mod_symbols(path: Path, patches: list[tuple[str, str]]):
    result = {}

    mod_source = path.read_text(encoding='utf-8')

    for patch in patches:
        mod_source = re.sub(patch[0], patch[1], mod_source, flags=re.MULTILINE)

    metadata = get_mod_metadata(mod_source)

    for arch in metadata['architectures']:
        symbol_blocks = get_mod_symbol_blocks(mod_source, arch)
        for block in symbol_blocks:
            for module in block['modules']:
                result_arch = result.setdefault(arch, {})
                # Add unique symbols.
                result_arch[module] = list(
                    dict.fromkeys(result_arch.get(module, []) + block['symbols']))

    return result


def get_relevant_mod_versions(mods_folder: Path, mod_name: str):
    versions_path = mods_folder / mod_name / 'versions.json'
    with versions_path.open() as f:
        versions = json.load(f)

    timestamp_now = time.time()

    for version in reversed(versions):
        yield mods_folder / mod_name / f'{version["version"]}.wh.cpp'

        timestamp = version['timestamp']
        sixty_days = 60 * 60 * 24 * 60
        if timestamp_now - timestamp > sixty_days:
            break


def main():
    parser = ArgumentParser()
    parser.add_argument('mods_folder', type=Path)
    parser.add_argument('output_file', type=Path)
    args = parser.parse_args()

    mods_folder: Path = args.mods_folder
    output_file: Path = args.output_file

    result = {}

    for mod_main_path in mods_folder.glob('*.wh.cpp'):
        mod_name = mod_main_path.name.removesuffix('.wh.cpp')

        for mod_version_path in get_relevant_mod_versions(mods_folder, mod_name):
            relative_path = str(mod_version_path.relative_to(mods_folder).as_posix())

            try:
                mod_symbols = get_mod_symbols(mod_version_path, MOD_PATCHES.get(relative_path, []))
            except Exception as e:
                print(f'Failed to extract symbols from mod {relative_path}: {e}')
                continue

            for arch in mod_symbols:
                for module in mod_symbols[arch]:
                    if module in SYMBOL_MODULES_SKIP.get(mod_name, []):
                        continue

                    result_arch = result.setdefault(mod_name, {}).setdefault(arch, {})
                    # Add unique symbols.
                    result_arch[module] = list(
                        dict.fromkeys(
                            result_arch.get(module, []) + mod_symbols[arch][module]
                        )
                    )

    if str(output_file) == '-':
        print(json.dumps(result, indent=2))
    else:
        with output_file.open('w') as f:
            json.dump(result, f, indent=2)


if __name__ == '__main__':
    main()
