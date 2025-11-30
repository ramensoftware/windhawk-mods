'''
PROJECT:     PR Validation for Windhawk
LICENSE:     MIT (https://spdx.org/licenses/MIT)
PURPOSE:     Verifies the mod information in the modified mods.
COPYRIGHT:   Copyright 2023 Mark Jansen <mark.jansen@reactos.org>
'''

import json
import os
import re
import sys
import urllib.request
from functools import cache
from pathlib import Path
from typing import Optional, TextIO, Tuple

from extract_mod_symbols import get_mod_symbols

DISALLOWED_AUTHORS = [
    # https://github.com/ramensoftware/windhawk-mods/pull/676
    'arukateru',
]


MOD_METADATA_PARAMS = {
    'singleValue': {
        'id',
        'version',
        'github',
        'twitter',
        'homepage',
        'compilerOptions',
        'license',
        'donateUrl',
    },
    'singleValueLocalizable': {
        'name',
        'description',
        'author',
    },
    'multiValue': {
        'include',
        'exclude',
        'architecture',
    },
}


def add_warning(file: Path, line: int, message: str):
    # https://github.com/orgs/community/discussions/26736
    def escape_data(s: str) -> str:
        return s.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A')

    def escape_property(s: str) -> str:
        return (
            s.replace('%', '%25')
            .replace('\r', '%0D')
            .replace('\n', '%0A')
            .replace(':', '%3A')
            .replace(',', '%2C')
        )

    print(
        f'::warning file={escape_property(str(file))},'
        f'line={line}::{escape_data(message)}'
    )
    return 1


def get_mod_file_metadata(path: Path, file: TextIO):
    warnings = 0

    properties: dict[Tuple[str, Optional[str]], Tuple[str, int]] = {}

    inside_metadata_block = False

    line_number = 0
    while line := file.readline():
        line = line.rstrip('\n')
        line_number += 1

        if not inside_metadata_block:
            if re.fullmatch(r'//[ \t]+==WindhawkMod==[ \t]*', line):
                inside_metadata_block = True
                if line_number != 1:
                    warnings += add_warning(
                        path, line_number, 'Metadata block must start at line 1'
                    )
            continue

        if re.fullmatch(r'//[ \t]+==\/WindhawkMod==[ \t]*', line):
            inside_metadata_block = False
            break

        if line.strip() == '':
            continue

        match = re.fullmatch(
            r'//[ \t]+@([a-zA-Z]+)(?::([a-z]{2}(?:-[A-Z]{2})?))?[ \t]+(.*)',
            line.strip(),
        )
        if not match:
            warnings += add_warning(path, line_number, 'Invalid metadata line format')
            continue

        key = match.group(1)
        language = match.group(2)
        value = match.group(3)

        if not any(key in x for x in MOD_METADATA_PARAMS.values()):
            warnings += add_warning(
                path, line_number, f'{key} is not a valid metadata parameter'
            )
            continue

        if (
            key not in MOD_METADATA_PARAMS['singleValueLocalizable']
            and language is not None
        ):
            warnings += add_warning(
                path, line_number, 'Language cannot be specified for this property'
            )
            continue

        if key in MOD_METADATA_PARAMS['multiValue']:
            prefix = properties.get((key, language), ('', 1))[0]
            properties[(key, language)] = f'{prefix}{value}\n', line_number
        else:
            if (key, language) in properties:
                warnings += add_warning(
                    path, line_number, f'{key} must be specified only once'
                )
                continue

            properties[(key, language)] = value, line_number

    if inside_metadata_block:
        warnings += add_warning(path, 1, 'Metadata block must be closed')

    return properties, warnings


@cache
def get_mod_author_data():
    url = 'https://mods.windhawk.net/mod_author_data.json'
    response = urllib.request.urlopen(url).read()
    return json.loads(response)


@cache
def get_valid_license_identifiers_lowercase():
    url = 'https://spdx.org/licenses/licenses.json'
    response = urllib.request.urlopen(url).read()
    data = json.loads(response)
    return {license['licenseId'].lower() for license in data['licenses']}


def is_valid_license_identifier(license_id: str):
    return license_id.lower() in get_valid_license_identifiers_lowercase()


def validate_metadata(path: Path, expected_author: str):
    with path.open(encoding='utf-8') as file:
        properties, warnings = get_mod_file_metadata(path, file)

    # Print properties with Zero Width Non-Joiner (ZWNJ) to prevent GitHub from
    # tagging users.
    at = '@\u200c'

    github = None

    key = ('github', None)
    if key in properties:
        value, line_number = properties[key]
        github = value
        expected = f'https://github.com/{expected_author}'
        if value != expected and value.lower() == expected.lower():
            warning_msg = f'Expected {at}{key[0]} to be "{expected}" (case-sensitive)'
            warnings += add_warning(path, line_number, warning_msg)
        elif value != expected:
            warning_msg = (
                f'Expected {at}{key[0]} to be "{expected}".\n'
                'Note that only the original author of the mod is allowed to submit'
                ' updates.\n'
                'If you are not the original author, you might want to contact them to'
                ' submit the update instead.\n'
                'For more information about submitting a mod update, refer to the'
                ' "Submitting a Mod Update" section in the repository\'s README.md.'
            )
            warnings += add_warning(path, line_number, warning_msg)
    else:
        warnings += add_warning(path, 1, f'Missing {at}{key[0]}')

    key = ('id', None)
    if key in properties:
        value, line_number = properties[key]
        expected = path.name.removesuffix('.cpp').removesuffix('.wh')
        if value != expected:
            warnings += add_warning(
                path,
                line_number,
                f'Expected {at}{key[0]} ({value}) to match the file name ({expected})',
            )

        if not re.fullmatch(r'([0-9a-z]+-)*[0-9a-z]+', value):
            warnings += add_warning(
                path,
                line_number,
                f'{at}{key[0]} must contain only letters, numbers and dashes',
            )

        if len(value) < 8 or len(value) > 50:
            warnings += add_warning(
                path, line_number, f'{at}{key[0]} must be between 8 and 50 characters'
            )
    else:
        warnings += add_warning(path, 1, f'Missing {at}{key[0]}')

    key = ('version', None)
    if key in properties:
        value, line_number = properties[key]
        if not re.fullmatch(r'([0-9]+\.)*[0-9]+(-\w+)?', value):
            warnings += add_warning(
                path,
                line_number,
                f'{at}{key[0]} must contain only numbers and dots, and optionally a'
                ' prerelease suffix (e.g. 1.2.3-beta)',
            )
    else:
        warnings += add_warning(path, 1, f'Missing {at}{key[0]}')

    # Validate author data against existing records
    mod_author_data = get_mod_author_data()
    author_data = github and mod_author_data.get(github.lower())

    key = ('author', None)
    if key in properties:
        value, line_number = properties[key]
        if author_data:
            # Existing author - must match exactly
            if value != author_data['author']:
                warning_msg = (
                    f'Expected {at}{key[0]} to be "{author_data["author"]}" based on'
                    f' previous submissions for {github}'
                )
                warnings += add_warning(path, line_number, warning_msg)
        else:
            # New author - make sure this author name isn't used by someone else
            for other_github, other_data in mod_author_data.items():
                if other_data['author'].lower() == value.lower():
                    warning_msg = (
                        f'Author name "{value}" is already used by {other_github}. '
                        'Please use a different author name.'
                    )
                    warnings += add_warning(path, line_number, warning_msg)
                    break
    else:
        warnings += add_warning(path, 1, f'Missing {at}{key[0]}')

    key = ('twitter', None)
    if key in properties:
        value, line_number = properties[key]
        if author_data and 'twitter' in author_data:
            # Existing author with twitter - must match exactly
            if value != author_data['twitter']:
                warning_msg = (
                    f'Expected {at}{key[0]} to be "{author_data["twitter"]}" based'
                    f' on previous submissions for {github}'
                )
                warnings += add_warning(path, line_number, warning_msg)
        else:
            # New twitter value - check it's not used by someone else
            for other_github, other_data in mod_author_data.items():
                if 'twitter' in other_data and other_data['twitter'].lower() == value.lower():
                    warning_msg = (
                        f'Twitter account "{value}" is already used by {other_github}. '
                        'If this is correct, manual verification is required.'
                    )
                    warnings += add_warning(path, line_number, warning_msg)
                    break
            else:
                # Not used by anyone else, still requires manual verification
                warnings += add_warning(
                    path, line_number, f'{at}{key[0]} requires manual verification'
                )

    key = ('homepage', None)
    if key in properties:
        value, line_number = properties[key]
        if not re.match(r'https?://', value):
            warnings += add_warning(
                path,
                line_number,
                f'{at}{key[0]} must start with "http://" or "https://"',
            )
        else:
            # Check if this homepage is used by someone else
            homepage_already_used = False
            if author_data:
                # For existing authors, check if it's in their list
                homepage_already_used = value in author_data.get('homepages', [])

            if not homepage_already_used:
                # New homepage - check it's not used by another author
                for other_github, other_data in mod_author_data.items():
                    if other_github.lower() != (github or '').lower():
                        if value.lower() in [h.lower() for h in other_data.get('homepages', [])]:
                            warning_msg = (
                                f'Homepage "{value}" is already used by {other_github}. '
                                'Please use a different homepage or verify this is correct.'
                            )
                            warnings += add_warning(path, line_number, warning_msg)
                            break

    key = ('donateUrl', None)
    if key in properties:
        value, line_number = properties[key]
        if not re.match(r'https?://', value):
            warnings += add_warning(
                path,
                line_number,
                f'{at}{key[0]} must start with "http://" or "https://"',
            )

    key = ('compilerOptions', None)
    if key in properties:
        value, line_number = properties[key]
        if not re.fullmatch(r'((-[lD]\S+|-Wl,--export-all-symbols)\s+)+', value + ' '):
            warnings += add_warning(
                path, line_number, f'{at}{key[0]} require manual verification'
            )

    key = ('license', None)
    if key in properties:
        value, line_number = properties[key]
        if not is_valid_license_identifier(value):
            warning_msg = (
                f'Unknown license identifier "{value}". The license must be'
                ' a valid SPDX identifier from https://spdx.org/licenses/.'
            )
            warnings += add_warning(path, line_number, warning_msg)

    key = ('name', None)
    if key not in properties:
        warnings += add_warning(path, 1, f'Missing {at}{key[0]}')

    key = ('architecture', None)
    if key in properties:
        value, line_number = properties[key]
        for arch in value.split('\n'):
            if arch.strip() == '':
                pass
            elif arch not in {'x86', 'x86-64', 'amd64', 'arm64'}:
                warnings += add_warning(
                    path, line_number, f'Unknown architecture "{arch}"'
                )
            elif arch not in {'x86', 'x86-64'}:
                warnings += add_warning(
                    path,
                    line_number,
                    f'Architecture "{arch}" isn\'t commonly used, manual verification'
                    ' is required',
                )

    # Validate that this file has the required extensions
    if not path.name.endswith('.wh.cpp'):
        warnings += add_warning(path, 1, 'Filename should end with .wh.cpp')

    # Validate file path
    if path.parent != Path('mods'):
        warnings += add_warning(path, 1, 'File is not placed in the mods folder')

    return warnings


@cache
def get_existing_windows_file_names():
    url = 'https://winbindex.m417z.com/data/filenames.json'
    response = urllib.request.urlopen(url).read()
    return json.loads(response)


def is_existing_windows_file_name(name: str):
    return name.lower() in get_existing_windows_file_names()


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


def validate_symbol_hooks(path: Path):
    warnings = 0

    mod_source = path.read_text(encoding='utf-8')
    mod_source_lines = mod_source.splitlines()

    p = r'^[ \t]*(?:const[ \t]+)?(?:WindhawkUtils::)?SYMBOL_HOOK[ \t]+(\w+)'
    for match in re.finditer(p, mod_source, re.MULTILINE):
        symbol_block_name = match.group(1)

        line_num = 1 + mod_source[: match.start()].count('\n')

        target_from_name = get_target_module_from_symbol_block_name(symbol_block_name)

        previous_line = mod_source_lines[line_num - 2]
        targets_from_comment = get_target_modules_from_previous_line(previous_line)

        if target_from_name and targets_from_comment:
            warning_msg = (
                'Use either a comment or a variable name, not both. For example, you'
                ' can rename the variable from "user32DllHooks" to "user32Hooks".'
            )
            warnings += add_warning(path, line_num, warning_msg)
        elif target_from_name or targets_from_comment:
            if target_from_name and not is_existing_windows_file_name(target_from_name):
                warning_msg = (
                    f'"{target_from_name}" is not recognized as a Windows file name.'
                    ' If the target module name can\'t be represented by a variable'
                    ' name, add the target module in a comment above the symbol hooks'
                    ' variable. Example:\n'
                    '// Taskbar.View.dll\n'
                    'WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {...};'
                )
                warnings += add_warning(path, line_num, warning_msg)

            for target in targets_from_comment:
                if not is_existing_windows_file_name(target):
                    warning_msg = f'"{target}" is not recognized as a Windows file name'
                    warnings += add_warning(path, line_num - 1, warning_msg)
            continue

        warning_msg = (
            'Please rename the symbol hooks variable to indicate the target module.'
            ' Examples (can end with "hook" or "hooks"):\n'
            '* user32DllHooks\n'
            '* user32dll_hooks\n'
            '* user32_dll_hooks\n'
            'If the target module name can\'t be represented by a variable name, or'
            ' if there is more than one target module, add all target modules in a'
            ' comment above the symbol hooks variable, separated with commas.'
            ' Example:\n'
            '// explorer.exe, taskbar.dll\n'
            'WindhawkUtils::SYMBOL_HOOK hooks[] = {...};'
        )
        warnings += add_warning(path, line_num, warning_msg)

    return warnings


def main():
    print('Validating PR...')

    pr_author = os.environ['PR_AUTHOR']
    if pr_author in DISALLOWED_AUTHORS:
        sys.exit(f'Submissions from {pr_author} are not allowed')

    warnings = 0

    paths = [Path(p) for p in json.loads(os.environ['ALL_CHANGED_AND_MODIFIED_FILES'])]
    if len(paths) == 0:
        sys.exit('No files changed')

    added_count = int(os.environ['ADDED_FILES_COUNT'])
    modified_count = int(os.environ['MODIFIED_FILES_COUNT'])
    all_count = int(os.environ['ALL_CHANGED_AND_MODIFIED_FILES_COUNT'])

    if (added_count, modified_count, all_count) not in [(1, 0, 1), (0, 1, 1)]:
        warnings += add_warning(
            paths[0],
            1,
            'Must be one added or one modified file, got '
            f'{added_count=} {modified_count=} {all_count=}',
        )

    for path in paths:
        print(f'Checking {path=}')

        path_warnings = validate_metadata(path, pr_author)
        path_warnings += validate_symbol_hooks(path)
        warnings += path_warnings

        if path_warnings == 0:
            try:
                mod_symbols = get_mod_symbols(path, [])
                print('Extracted symbols:\n' + json.dumps(mod_symbols, indent=2))
            except Exception as e:
                print(f'Symbol extraction error: {e}')
                warnings += add_warning(
                    path, 1, 'Failed to extract symbols, manual inspection required'
                )

    if warnings > 0:
        sys.exit(f'Got {warnings} warnings, please inspect the PR')


if __name__ == '__main__':
    main()
