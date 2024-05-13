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
from pathlib import Path
from typing import Optional, TextIO, Tuple

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
    }
}


def add_warning(file: Path, line: int, message: str):
    # https://github.com/orgs/community/discussions/26736
    def escape_data(s: str) -> str:
        return s.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A')

    def escape_property(s: str) -> str:
        return s.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A').replace(':', '%3A').replace(',', '%2C')

    print(f'::warning file={escape_property(str(file))},'
          f'line={line}::{escape_data(message)}')
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
                    warnings += add_warning(path, line_number,
                                            'Metadata block must start at line 1')
            continue

        if re.fullmatch(r'//[ \t]+==\/WindhawkMod==[ \t]*', line):
            inside_metadata_block = False
            break

        if line.strip() == '':
            continue

        match = re.fullmatch(
            r'//[ \t]+@([a-zA-Z]+)(?::([a-z]{2}(?:-[A-Z]{2})?))?[ \t]+(.*)', line.strip())
        if not match:
            warnings += add_warning(path, line_number,
                                    'Invalid metadata line format')
            continue

        key = match.group(1)
        language = match.group(2)
        value = match.group(3)

        if not any(key in x for x in MOD_METADATA_PARAMS.values()):
            warnings += add_warning(path, line_number,
                                    f'@{key} is not a valid metadata parameter')
            continue

        if key not in MOD_METADATA_PARAMS['singleValueLocalizable'] and language is not None:
            warnings += add_warning(path, line_number,
                                    'Language cannot be specified for this property')
            continue

        if key in MOD_METADATA_PARAMS['multiValue']:
            prefix = properties.get((key, language), ('', 1))[0]
            properties[(key, language)] = f'{prefix}{value}\n', line_number
        else:
            if (key, language) in properties:
                warnings += add_warning(path, line_number,
                                        f'@{key} must be specified only once')
                continue

            properties[(key, language)] = value, line_number

    if inside_metadata_block:
        warnings += add_warning(path, 1, 'Metadata block must be closed')

    return properties, warnings


def parse_file(path: Path, expected_author: str):
    print(f'Checking {path=}')

    with path.open(encoding='utf-8') as file:
        properties, warnings = get_mod_file_metadata(path, file)

    key = ('github', None)
    if key in properties:
        value, line_number = properties[key]
        if value != f'https://github.com/{expected_author}':
            warnings += add_warning(path, line_number,
                                    f'Expected the author to be "{expected_author}"')
    else:
        warnings += add_warning(path, 1, f'Missing @{key[0]}')

    key = ('id', None)
    if key in properties:
        value, line_number = properties[key]
        expected = path.name.removesuffix('.cpp').removesuffix('.wh')
        if value != expected:
            warnings += add_warning(path, line_number,
                                    f'Expected the id to be "{expected}"')

        if not re.fullmatch(r'([0-9a-z]+-)*[0-9a-z]+', value):
            warnings += add_warning(path, line_number,
                                    '@id must contain only letters, numbers and dashes')

        if len(value) < 8 or len(value) > 50:
            warnings += add_warning(path, line_number,
                                    '@id must be between 8 and 50 characters')
    else:
        warnings += add_warning(path, 1, f'Missing @{key[0]}')

    key = ('version', None)
    if key in properties:
        value, line_number = properties[key]
        if not re.fullmatch(r'([0-9]+\.)*[0-9]+', value):
            warnings += add_warning(path, line_number,
                                    'Version must contain only numbers and dots')
    else:
        warnings += add_warning(path, 1, f'Missing @{key[0]}')

    key = ('compilerOptions', None)
    if key in properties:
        value, line_number = properties[key]
        if not re.fullmatch(r'((-[lD]\S+|-Wl,--export-all-symbols)\s+)+', value + ' '):
            warnings += add_warning(path, line_number,
                                    'Compiler options require manual verification')

    key = ('author', None)
    if key not in properties:
        warnings += add_warning(path, 1, f'Missing @{key[0]}')

    # Validate that this file has the required extensions
    if ''.join(path.suffixes) != '.wh.cpp':
        warnings += add_warning(path, 1, 'Filename should end with .wh.cpp')

    # Validate file path
    if path.parent != Path('mods'):
        warnings += add_warning(path, 1,
                                'File is not placed in the mods folder')

    return warnings


def main():
    print('Validating PR...')

    pr_author = os.environ['PR_AUTHOR']
    if pr_author in DISALLOWED_AUTHORS:
        sys.exit(f'Submissions from {pr_author} are not allowed')

    warnings = 0

    paths = [Path(p) for p in
             json.loads(os.environ['ALL_CHANGED_AND_MODIFIED_FILES'])]

    added_count = int(os.environ['ADDED_FILES_COUNT'])
    modified_count = int(os.environ['MODIFIED_FILES_COUNT'])
    all_count = int(os.environ['ALL_CHANGED_AND_MODIFIED_FILES_COUNT'])

    if (added_count, modified_count, all_count) not in [(1, 0, 1), (0, 1, 1)]:
        warnings += add_warning(paths[0], 1, f'Must be one added or one modified file, got '
                                f'{added_count=} {modified_count=} {all_count=}')

    for path in paths:
        warnings += parse_file(path, pr_author)

    if warnings > 0:
        sys.exit(f'Got {warnings} warnings, please inspect the PR')


if __name__ == '__main__':
    main()
