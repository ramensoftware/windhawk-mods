'''
PROJECT:     PR Validation for WindHawk
LICENSE:     MIT (https://spdx.org/licenses/MIT)
PURPOSE:     Verifies the mod information in the modified mods.
COPYRIGHT:   Copyright 2023 Mark Jansen <mark.jansen@reactos.org>
'''
from optparse import OptionParser
from pathlib import Path
import sys


def add_warning(file: Path, line: int, message: str):
    print(f'::warning file={file!s},line={line}::{message}')
    return 1


def parse_file(file: Path, expected_author):
    warnings = 0
    print(f'Checking {file=!s}')
    for idx, line in enumerate(file.read_text().splitlines()):
        if not line.startswith('//'):
            continue
        line = line[2:].strip()
        # Validate the github id specified against the PR author
        if line.startswith('@github'):
            line = line[7:].strip()
            if line != f'https://github.com/{expected_author}':
                warnings += add_warning(file, idx + 1,
                                        f'Expected the author to be "{expected_author}"')
        # Validate the mod ID against the filename
        if line.startswith('@id'):
            line = line[3:].strip()
            expected = file.with_suffix('').with_suffix(
                '').name    # Strip the two extensions
            if line != expected:
                warnings += add_warning(file, idx + 1,
                                        f'Expected the id to be "{expected}"')

    # Validate that this file has the required extensions
    if ''.join(file.suffixes) != '.wh.cpp':
        warnings += add_warning(file, 1, 'Filename should end with .wh.cpp')

    return warnings


def main():
    parser = OptionParser()
    parser.add_option("--author", help="Specify the PR author")
    (options, args) = parser.parse_args()
    if not options.author:
        parser.error("Please specify the PR author")

    warnings = 0
    for idx, path in enumerate([Path(filename) for filename in args]):
        if not path or len(path.parts) == 0:
            # Not sure if this could ever happen, but we should not crash
            warnings += add_warning(Path(__file__), 1,
                                    f'Unable to parse arguments: {args}')
            continue
        # We expect relative paths from the repo root
        if path.parts[0] != 'mods':
            warnings += add_warning(path, 1,
                                    'File is not placed in the mods folder')
        # Validate everything as if it were a mod, if it has the .cpp extension
        if path.name.endswith('.cpp'):
            warnings += parse_file(path, options.author)
        if idx > 0:
            warnings += add_warning(path, 1, 'More than one file was changed')
    if warnings > 0:
        sys.exit(f'Got {warnings} warnings, please inspect the PR')


if __name__ == '__main__':
    main()
