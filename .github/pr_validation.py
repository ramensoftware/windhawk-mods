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
from typing import Optional, Tuple

DISALLOWED_AUTHORS = [
    # https://github.com/ramensoftware/windhawk-mods/pull/676
    'arukateru',
]


def add_warning(file: Path, line: int, message: str):
    # https://github.com/orgs/community/discussions/26736
    def escape_data(s: str) -> str:
        return s.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A')

    def escape_property(s: str) -> str:
        return s.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A').replace(':', '%3A').replace(',', '%2C')

    print(f'::warning file={escape_property(str(file))},'
          f'line={line}::{escape_data(message)}')
    return 1


def parse_file(file: Path, expected_author: str):
    warnings = 0
    print(f'Checking {file=}')

    core_properties: dict[str, Optional[Tuple[str, int]]] = {
        'id': None,
        'github': None,
        'version': None,
        'compilerOptions': None,
    }

    all_properties: set[str] = set()

    for idx, line in enumerate(file.read_text().splitlines()):
        if idx == 0:
            if not re.fullmatch(r'//[ \t]+==WindhawkMod==[ \t]*', line):
                warnings += add_warning(file, idx + 1,
                                        'First line must be "// ==WindhawkMod=="')
                break
            continue

        if re.fullmatch(r'//[ \t]+==\/WindhawkMod==[ \t]*', line):
            break

        if line.strip() == '':
            continue

        match = re.fullmatch(
            r'//[ \t]+@([a-zA-Z]+)(?::([a-z]{2}(?:-[A-Z]{2})?))?[ \t]+(.*)', line.strip())
        if not match:
            warnings += add_warning(file, idx + 1,
                                    'Invalid metadata line format')
            continue

        key = match.group(1)
        language = match.group(2)
        value = match.group(3)

        all_properties.add(key)

        if key not in core_properties:
            continue

        if language is not None:
            warnings += add_warning(file, idx + 1,
                                    'Language cannot be specified for this property')
            continue

        if core_properties[key] is not None:
            warnings += add_warning(file, idx + 1,
                                    f'{key} must be specified only once')
            continue

        core_properties[key] = value, idx

    # Validate the github id specified against the PR author
    if core_properties['github'] is not None:
        value, idx = core_properties['github']
        if value != f'https://github.com/{expected_author}':
            warnings += add_warning(file, idx + 1,
                                    f'Expected the author to be "{expected_author}"')
    else:
        warnings += add_warning(file, 1, 'Missing @github')

    # Validate the mod ID against the filename
    if core_properties['id'] is not None:
        value, idx = core_properties['id']
        # Strip the two extensions
        expected = file.stem
        if value != expected:
            warnings += add_warning(file, idx + 1,
                                    f'Expected the id to be "{expected}"')
    else:
        warnings += add_warning(file, 1, 'Missing @id')

    # Validate the version
    if core_properties['version'] is not None:
        value, idx = core_properties['version']
        if not re.fullmatch(r'([0-9]+\.)*[0-9]+', value):
            warnings += add_warning(file, idx + 1,
                                    'Version must contain only numbers and dots')
    else:
        warnings += add_warning(file, 1, 'Missing @version')

    if core_properties['compilerOptions'] is not None:
        value, idx = core_properties['compilerOptions']
        if not re.fullmatch(r'((-[lD]\S+|-Wl,--export-all-symbols)\s+)+', value + ' '):
            warnings += add_warning(file, idx + 1,
                                    'Compiler options require manual verification')

    if 'author' not in all_properties:
        warnings += add_warning(file, 1, 'Missing @author')

    # Validate that this file has the required extensions
    if ''.join(file.suffixes) != '.wh.cpp':
        warnings += add_warning(file, 1, 'Filename should end with .wh.cpp')

    # Validate file name
    if not re.fullmatch(r'([0-9a-z]+-)*[0-9a-z]+', file.with_suffix('').with_suffix('').name):
        warnings += add_warning(file, 1,
                                'Filename must contain only letters, numbers and dashes')

    # Validate file path
    if file.parent != Path('mods'):
        warnings += add_warning(file, 1,
                                'File is not placed in the mods folder')

    return warnings


def main():
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
        # Validate everything as if it were a mod, if it has the .cpp extension
        if path.suffix == '.cpp':
            warnings += parse_file(path, pr_author)

    if warnings > 0:
        sys.exit(f'Got {warnings} warnings, please inspect the PR')


if __name__ == '__main__':
    main()
