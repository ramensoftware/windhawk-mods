import json
import re
import time
from argparse import ArgumentParser
from enum import StrEnum, auto
from pathlib import Path


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
    'basic-themer/1.1.0.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hooks\b',
            r'// uDWM.dll\n\g<0>'
        ),
    ],
    'change-explorer-default-location/1.0.0.wh.cpp': [
        (
            r'^const WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// shell32.dll\n\g<0>'
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
    'no-taskbar-item-glow/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// explorer.exe\n\g<0>'
        ),
    ],
    'notepad-remove-launch-new-app-banner/1.0.0.wh.cpp': [
        (
            r'^[ \t]*WindhawkUtils::SYMBOL_HOOK hook\b',
            r'// notepad.exe\n\g<0>'
        ),
    ],
    'old-regedit-tree-icons/1.0.0.wh.cpp': [
        (
            r'^' + re.escape('#define STDCALL_STR FOR_64_32(L"__cdecl", L"__stdcall")') + r'$',
            r'#ifdef _WIN64\n#define STDCALL_STR L"__cdecl"\n#else\n#define STDCALL_STR L"__stdcall"\n#endif'
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
    'syslistview32-enabler/1.0.2.wh.cpp': [
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
    # Win7 only.
    'win7-alttab-loader': ['alttab.dll'],
}


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


def remove_comments_from_code(code: str):
    added_newline = False
    if not code.endswith('\n'):
        code += '\n'
        added_newline = True

    # https://stackoverflow.com/a/36455937
    p = r'''(?:\/\/(?:\\\n|[^\n])*\n)|(?:\/\*[\s\S]*?\*\/)|((?:R"([^(\\\s]{0,16})\([^)]*\)\2")|(?:@"[^"]*?")|(?:"(?:\?\?'|\\\\|\\"|\\\n|[^"])*?")|(?:'(?:\\\\|\\'|\\\n|[^'])*?'))'''
    code = re.sub(p, r'\g<1>', code)

    if added_newline:
        code = code.rstrip('\n')

    return code


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


def process_symbol_block(mod_source: str, symbol_block_match: re.Match, string_definitions: dict[str, str]):
    symbol_block = remove_comments_from_code(symbol_block_match.group(0))

    # Make sure there are no preprocessor directives.
    p = r'^[ \t]*#.*'
    if match := re.search(p, symbol_block, flags=re.MULTILINE):
        raise Exception(f'Unsupported preprocessor directive: {match.group(0)}')

    # Merge strings spanning over multiple lines.
    p = r'"([ \t]*\n)+[ \t]*L?"'
    symbol_block = re.sub(p, '', symbol_block)

    # Replace string definitions.
    def sub_quoted(match):
        symbol = match.group(1)
        if symbol is None:
            symbol = match.group(2)

        if symbol not in string_definitions:
            raise Exception(f'Unknown string definition {symbol}')

        return string_definitions[symbol]

    p = r'"\s*(\w+)\s*L"|"\s+(\w+)\s+"'
    symbol_block = re.sub(p, sub_quoted, symbol_block)

    def sub_braced(match):
        symbol = match.group(1)

        if symbol not in string_definitions:
            raise Exception(f'Unknown string definition {symbol}')

        return '{L"' + string_definitions[symbol] + '"}'

    p = r'\{\s*(\w+)\s*\}'
    symbol_block = re.sub(p, sub_braced, symbol_block)

    # Sanity check.
    for string_definition in string_definitions:
        if string_definition in symbol_block:
            raise Exception(f'String definition wasn\'t replaced: {string_definition}')

    # Extract symbols.
    p = r'LR"\((.*?)\)"|L"(.*?)"'
    symbols = re.findall(p, symbol_block)
    symbols = list(map(lambda x: x[0] if x[0] else x[1], symbols))

    if any('"' in x or '\\' in x for x in symbols):
        raise Exception(f'Unsupported strings')

    if len(symbols) * 2 != symbol_block.count('"'):
        raise Exception(f'Unsupported strings')

    if symbols == []:
        return None

    modules = deduce_symbol_block_target_modules(mod_source, symbol_block_match)

    return {
        'symbols': symbols,
        'modules': modules,
    }


def get_mod_symbol_blocks(mod_source: str, arch: Architecture):
    # Expand #if architecture conditions.
    def sub(sub_match):
        condition1 = sub_match.group(1)
        body1 = sub_match.group(2)
        condition2 = sub_match.group(3)
        body2 = sub_match.group(4)

        condition1 = remove_comments_from_code(condition1).strip()
        if condition2 is not None:
            condition2 = remove_comments_from_code(condition2).strip()

        if match := re.fullmatch(r'(?:if defined|ifdef|if)\b(.*)', condition1):
            expression = match.group(1).strip()
            negative = False
        elif match := re.fullmatch(r'(?:|if !defined|ifndef|if !)\b(.*)', condition1):
            expression = match.group(1).strip()
            negative = True
        else:
            raise Exception(f'Unsupported condition1: {condition1}')

        if condition2 is not None and condition2 != 'else':
            raise Exception(f'Unsupported condition2: {condition2}')

        if expression.startswith('(') and expression.endswith(')'):
            expression = expression[1:-1].strip()

        if expression == '_WIN64':
            condition_matches = arch in [Architecture.amd64, Architecture.arm64]
        elif expression == '_M_IX86':
            condition_matches = arch == Architecture.x86
        elif expression == '_M_X64':
            condition_matches = arch == Architecture.amd64
        elif expression == '_M_ARM64':
            condition_matches = arch == Architecture.arm64
        else:
            # Not a supported arch condition, return as is.
            return sub_match.group(0)

        if negative:
            condition_matches = not condition_matches

        if condition_matches:
            return body1

        return body2 or ''

    p = r'^[ \t]*#\s*(if.*)$([\s\S]*?)(?:^[ \t]*#\s*(else.*)$([\s\S]*?))?^[ \t]*#endif[ \t]*$'
    mod_source = re.sub(p, sub, mod_source, flags=re.MULTILINE)

    # Extract string definitions.
    p = r'^[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+L"(.*?)"[ \t]*$'
    string_definitions = dict(re.findall(p, mod_source, flags=re.MULTILINE))
    if any('"' in re.sub(r'\\.', '', x) for x in string_definitions.values()):
        raise Exception(f'Unsupported string definitions')

    # Extract symbol blocks.
    symbol_blocks = []
    p = r'^[ \t]*(?:const[ \t]+)?(?:WindhawkUtils::)?SYMBOL_HOOK[ \t]+(\w+)\s*[\[={][\s\S]*?\};[ \t]*$'
    for match in re.finditer(p, mod_source, flags=re.MULTILINE):
        symbol_block = process_symbol_block(mod_source, match, string_definitions)
        symbol_blocks.append(symbol_block)

    # Verify that no blocks were missed.
    p = r'SYMBOL_HOOK\s+\w'
    if len(symbol_blocks) != len(
        re.findall(p, remove_comments_from_code(mod_source), flags=re.MULTILINE)
    ):
        raise Exception(f'Unsupported symbol blocks')

    symbol_blocks = list(filter(lambda x: x is not None, symbol_blocks))

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
