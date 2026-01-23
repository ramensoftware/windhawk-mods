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
import time
import urllib.error
import urllib.request
from functools import cache
from io import StringIO
from pathlib import Path
from typing import Callable, Optional, TextIO, Tuple

from extract_mod_symbols import get_mod_symbols

DISALLOWED_AUTHORS = [
    # https://github.com/ramensoftware/windhawk-mods/pull/676
    'arukateru',
]


ALLOWED_AUTHOR_NAME_CHANGES = {
    'anixx': 'Anixx',
    'kawapure': 'Isabella Lulamoon (kawapure)',
}


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


ModPropertyKey = Tuple[str, Optional[str]]  # (key_name, language)
ModPropertyValue = Tuple[str, int]  # (value, line_number)


def get_mod_file_metadata(
    file: TextIO, warn_callback: Optional[Callable[[int, str], int]] = None
) -> Tuple[dict[ModPropertyKey, ModPropertyValue], int]:
    """
    Parse mod metadata from file content.

    Args:
        file: Text stream to read from
        warn_callback: Optional callback(line_number, message) for warnings

    Returns:
        Tuple of (properties dict, warning count)
    """
    warnings = 0

    def warn(line_number: int, message: str):
        nonlocal warnings
        if warn_callback:
            warnings += warn_callback(line_number, message)
        else:
            warnings += 1

    properties: dict[ModPropertyKey, ModPropertyValue] = {}

    inside_metadata_block = False

    line_number = 0
    while line := file.readline():
        line = line.rstrip('\n')
        line_number += 1

        if not inside_metadata_block:
            if re.fullmatch(r'//[ \t]+==WindhawkMod==[ \t]*', line):
                inside_metadata_block = True
                if line_number != 1:
                    warn(line_number, 'Metadata block must start at line 1')
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
            warn(line_number, 'Invalid metadata line format')
            continue

        key = match.group(1)
        language = match.group(2)
        value = match.group(3)

        if not any(key in x for x in MOD_METADATA_PARAMS.values()):
            warn(line_number, f'{key} is not a valid metadata parameter')
            continue

        if (
            key not in MOD_METADATA_PARAMS['singleValueLocalizable']
            and language is not None
        ):
            warn(line_number, 'Language cannot be specified for this property')
            continue

        if key in MOD_METADATA_PARAMS['multiValue']:
            prefix = properties.get((key, language), ('', 1))[0]
            properties[(key, language)] = f'{prefix}{value}\n', line_number
        else:
            if (key, language) in properties:
                warn(line_number, f'{key} must be specified only once')
                continue

            properties[(key, language)] = value, line_number

    if inside_metadata_block:
        warn(1, 'Metadata block must be closed')

    return properties, warnings


def urlopen_with_retry(url: str, max_retries: int = 5):
    """Open URL with retry logic for 403 errors."""
    attempt = 0
    while True:
        try:
            return urllib.request.urlopen(url)
        except urllib.error.HTTPError as e:
            if e.code == 403 and attempt < max_retries - 1:
                time.sleep(1)
                attempt += 1
                continue
            raise


@cache
def get_mod_author_data():
    url = 'https://mods.windhawk.net/mod_author_data.json'
    response = urlopen_with_retry(url).read()
    return json.loads(response)


@cache
def get_valid_license_identifiers_lowercase():
    url = 'https://spdx.org/licenses/licenses.json'
    response = urllib.request.urlopen(url).read()
    data = json.loads(response)
    return {license['licenseId'].lower() for license in data['licenses']}


def is_valid_license_identifier(license_id: str):
    return license_id.lower() in get_valid_license_identifiers_lowercase()


@cache
def get_existing_mod_metadata(mod_id: str) -> Optional[dict]:
    """Fetch existing mod metadata, or None if mod doesn't exist."""
    try:
        url = f'https://mods.windhawk.net/mods/{mod_id}.wh.cpp'
        response = urlopen_with_retry(url)
        content = response.read().decode('utf-8')

        # Use existing robust metadata parser (no warnings needed for existing mods)
        properties, _ = get_mod_file_metadata(StringIO(content), warn_callback=None)

        # Convert to simple dict with only non-localized single values
        metadata = {}
        for (key, language), (value, _) in properties.items():
            # Only include non-localized properties for validation
            if language is None:
                metadata[key] = value

        return metadata if metadata else None
    except urllib.error.HTTPError as e:
        if e.code == 404:
            return None
        raise


@cache
def get_existing_mod_versions(mod_id: str) -> Optional[list[str]]:
    """Fetch list of existing versions for a mod, or None if mod doesn't exist."""
    try:
        url = f'https://mods.windhawk.net/mods/{mod_id}/versions.json'
        response = urlopen_with_retry(url)
        data = json.loads(response.read())
        return [item['version'] for item in data]
    except urllib.error.HTTPError as e:
        if e.code == 404:
            return None
        raise


def at(key_name: str) -> str:
    """Format property name with Zero Width Non-Joiner to prevent GitHub tagging."""
    return f'@\u200c{key_name}'


class ValidationContext:
    """Manages validation state and warning count."""

    def __init__(self, path: Path):
        self.path = path
        self.__warning_count = 0

    def warn(self, message: str, line_number: Optional[int] = None) -> None:
        """Add a warning to the context."""
        line = line_number if line_number is not None else 1
        add_warning(self.path, line, message)
        self.__warning_count += 1

    def warning_count(self) -> int:
        """Get total number of warnings."""
        return self.__warning_count


class PropertyValidator:
    """Represents a single property with fluent validation methods."""

    def __init__(
        self, ctx: ValidationContext, key_name: str, value: str, line_number: int
    ):
        self.ctx = ctx
        self.key_name = key_name
        self.value = value
        self.line_number = line_number

    def warn(self, message: str) -> 'PropertyValidator':
        """Add a warning for this property. Use @@ as placeholder for property name."""
        message = message.replace('@@', at(self.key_name))
        self.ctx.warn(message, self.line_number)
        return self

    def validate_match(self, pattern: str, error_msg: str) -> 'PropertyValidator':
        """Validate value matches regex pattern."""
        if not re.fullmatch(pattern, self.value):
            self.warn(error_msg)
        return self

    def validate_url_format(self) -> 'PropertyValidator':
        """Validate URL starts with http:// or https://."""
        if not re.match(r'https?://', self.value):
            self.warn('@@ must start with "http://" or "https://"')
        return self


class ModMetadataValidator:
    """High-level validator that orchestrates all metadata validations."""

    def __init__(
        self,
        path: Path,
        properties: dict[ModPropertyKey, ModPropertyValue],
        expected_author: str,
    ):
        self.ctx = ValidationContext(path)
        self.properties = properties
        self.expected_author = expected_author
        self.mod_author_data = get_mod_author_data()

        # Extract mod ID and fetch existing mod data
        id_prop = self.property('id')
        self.mod_id = id_prop.value if id_prop else None
        self.existing_metadata = (
            get_existing_mod_metadata(self.mod_id) if self.mod_id else None
        )
        self.existing_versions = (
            get_existing_mod_versions(self.mod_id) if self.mod_id else None
        )

        # Extract github URL and fetch author data
        github_prop = self.property('github')
        self.github_url = github_prop.value if github_prop else None
        self.author_data = (
            self.mod_author_data.get(self.github_url.lower())
            if self.github_url
            else None
        )

    def property(
        self,
        key_name: str,
        language: Optional[str] = None,
        warn_if_missing: bool = False,
    ) -> Optional[PropertyValidator]:
        """Get a property validator for the given key, or None if property doesn't exist."""
        key = (key_name, language)
        if key in self.properties:
            value, line_number = self.properties[key]
            return PropertyValidator(self.ctx, key_name, value, line_number)
        if warn_if_missing:
            self.ctx.warn(f'Missing {at(key_name)}')
        return None

    def validate_all(self) -> int:
        """Run all validations and return warning count."""
        self.validate_github()
        self.validate_id()
        self.validate_version()
        self.validate_author()
        self.validate_twitter()
        self.validate_homepage()
        self.validate_donate_url()
        self.validate_compiler_options()
        self.validate_license()
        self.validate_name()
        self.validate_architecture()

        return self.ctx.warning_count()

    def validate_github(self):
        """Validate GitHub URL."""
        prop = self.property('github', warn_if_missing=True)
        if not prop:
            return

        # Check if mod already exists - github must not change
        if self.existing_metadata and 'github' in self.existing_metadata:
            if prop.value != self.existing_metadata['github']:
                prop.warn(
                    '@@ cannot be changed for existing mods. Expected'
                    f' "{self.existing_metadata["github"]}", got "{prop.value}"'
                )

        expected = f'https://github.com/{self.expected_author}'
        if not prop.value.startswith('https://github.com/'):
            prop.warn('@@ must start with "https://github.com/"')
        elif prop.value != expected and prop.value.lower() == expected.lower():
            prop.warn(f'Expected @@ to be "{expected}" (case-sensitive)')
        elif prop.value == expected + '/':
            prop.warn(f'Expected @@ to be "{expected}" (no trailing slash)')
        elif prop.value.startswith(expected + '/'):
            prop.warn(f'Expected @@ to be "{expected}" (user profile URL only)')
        elif prop.value != expected:
            prop.warn(
                f'Expected @@ to be "{expected}".\n'
                'Note that only the original author of the mod is allowed to submit'
                ' updates.\n'
                'If you are not the original author, you might want to contact them to'
                ' submit the update instead.\n'
                'For more information about submitting a mod update, refer to the'
                ' "Submitting a Mod Update" section in the repository\'s README.md.'
            )

    def validate_id(self):
        """Validate mod ID."""
        prop = self.property('id', warn_if_missing=True)
        if not prop:
            return

        # Check if mod already exists - id must not change
        if self.existing_metadata and 'id' in self.existing_metadata:
            if prop.value != self.existing_metadata['id']:
                prop.warn(
                    '@@ cannot be changed for existing mods. Expected'
                    f' "{self.existing_metadata["id"]}", got "{prop.value}"'
                )

        expected = self.ctx.path.name.removesuffix('.cpp').removesuffix('.wh')
        if prop.value != expected:
            prop.warn(f'Expected @@ ({prop.value}) to match the file name ({expected})')

        prop.validate_match(
            r'([0-9a-z]+-)*[0-9a-z]+',
            '@@ must contain only lowercase letters, numbers and dashes',
        )

        if len(prop.value) < 8 or len(prop.value) > 50:
            prop.warn('@@ must be between 8 and 50 characters')

    def validate_version(self):
        """Validate version format."""
        prop = self.property('version', warn_if_missing=True)
        if not prop:
            return

        prop.validate_match(
            r'([0-9]+\.)*[0-9]+',
            '@@ must contain only numbers and dots',
        )

        # Check if version is already used
        if self.existing_versions and prop.value in self.existing_versions:
            prop.warn(
                f'@@ "{prop.value}" is already used. Please use a new, unused'
                ' version.\n'
                f'Previous versions: {", ".join(self.existing_versions)}'
            )

    def validate_author(self):
        """Validate author name against existing records."""
        prop = self.property('author', warn_if_missing=True)
        if not prop:
            return

        # Check if mod already exists - author must not change
        if self.existing_metadata and 'author' in self.existing_metadata:
            if prop.value != self.existing_metadata['author'] and (
                ALLOWED_AUTHOR_NAME_CHANGES.get(self.existing_metadata['author'])
                != prop.value
            ):
                prop.warn(
                    '@@ cannot be changed for existing mods. Expected'
                    f' "{self.existing_metadata["author"]}", got "{prop.value}"'
                )

        if self.author_data:
            # Existing author - must match exactly
            if prop.value != self.author_data['author']:
                prop.warn(
                    f'Expected @@ to be "{self.author_data["author"]}" based on'
                    f' previous submissions for {self.github_url}'
                )
        else:
            for other_github, other_data in self.mod_author_data.items():
                if other_data['author'].lower() == prop.value.lower():
                    prop.warn(
                        f'Author name "{prop.value}" is already used by {other_github}.'
                    )
                    break

    def validate_twitter(self):
        """Validate Twitter handle."""
        prop = self.property('twitter')
        if not prop:
            return

        # Check if mod already exists - twitter must not change
        if self.existing_metadata and 'twitter' in self.existing_metadata:
            if prop.value != self.existing_metadata['twitter']:
                prop.warn(
                    '@@ cannot be changed for existing mods. Expected'
                    f' "{self.existing_metadata["twitter"]}", got "{prop.value}"'
                )

        if self.author_data and 'twitter' in self.author_data:
            # Existing author with twitter - must match exactly
            if prop.value != self.author_data['twitter']:
                prop.warn(
                    f'Expected @@ to be "{self.author_data["twitter"]}" based on'
                    f' previous submissions for {self.github_url}'
                )
        else:
            # New twitter value - check it's not used by someone else
            for other_github, other_data in self.mod_author_data.items():
                if (
                    'twitter' in other_data
                    and other_data['twitter'].lower() == prop.value.lower()
                ):
                    prop.warn(
                        f'Twitter account "{prop.value}" is already used by'
                        f' {other_github}.'
                    )
                    break
            else:
                # Not used by anyone else, still requires manual verification
                prop.warn('@@ requires manual verification')

    def validate_homepage(self):
        """Validate homepage URL."""
        prop = self.property('homepage')
        if not prop:
            return

        prop.validate_url_format()

        # Check if this homepage is used by someone else
        homepage_already_used = False
        if self.author_data:
            # For existing authors, check if it's in their list
            homepage_already_used = prop.value in self.author_data.get('homepages', [])

        if not homepage_already_used:
            # New homepage - check it's not used by another author
            for other_github, other_data in self.mod_author_data.items():
                if other_github.lower() != (self.github_url or '').lower():
                    if prop.value.lower() in [
                        h.lower() for h in other_data.get('homepages', [])
                    ]:
                        prop.warn(
                            f'Homepage "{prop.value}" is already used by'
                            f' {other_github}.'
                        )
                        break

    def validate_donate_url(self):
        """Validate donate URL format."""
        prop = self.property('donateUrl')
        if prop:
            prop.validate_url_format()

    def validate_compiler_options(self):
        """Validate compiler options format."""
        prop = self.property('compilerOptions')
        if not prop:
            return

        if not re.fullmatch(
            r'((-[lD]\S+|-Wl,--export-all-symbols)\s+)+', prop.value + ' '
        ):
            prop.warn('@@ require manual verification')

    def validate_license(self):
        """Validate license identifier."""
        prop = self.property('license')
        if not prop:
            return

        if not is_valid_license_identifier(prop.value):
            prop.warn(
                f'Unknown license identifier "{prop.value}". The license must be'
                ' a valid SPDX identifier from https://spdx.org/licenses/.'
            )

    def validate_name(self):
        """Validate name exists."""
        self.property('name', warn_if_missing=True)

    def validate_architecture(self):
        """Validate architecture values."""
        prop = self.property('architecture')
        if not prop:
            return

        for arch in prop.value.split('\n'):
            if arch.strip() == '':
                pass
            elif arch not in {'x86', 'x86-64', 'amd64', 'arm64'}:
                prop.warn(f'Unknown architecture "{arch}"')
            elif arch not in {'x86', 'x86-64'}:
                prop.warn(
                    f'Architecture "{arch}" isn\'t commonly used, manual verification'
                    ' is required'
                )


def validate_metadata(path: Path, expected_author: str) -> int:
    with path.open(encoding='utf-8') as file:
        properties, initial_warnings = get_mod_file_metadata(
            file, warn_callback=lambda line, msg: add_warning(path, line, msg)
        )

    # Validate metadata properties
    validator = ModMetadataValidator(path, properties, expected_author)
    metadata_warnings = validator.validate_all()

    # Validate file path
    file_warnings = 0
    if not path.name.endswith('.wh.cpp'):
        file_warnings += add_warning(path, 1, 'Filename should end with .wh.cpp')
    if path.parent != Path('mods'):
        file_warnings += add_warning(path, 1, 'File is not placed in the mods folder')

    return initial_warnings + metadata_warnings + file_warnings


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

    p = r'^[ \t]*(?:(?:static|const)[ \t]+)*(?:WindhawkUtils::)?SYMBOL_HOOK[ \t]+(\w+)'
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


def test_run():
    if len(sys.argv) != 3:
        print('Test run usage: pr_validation.py <mod_file_path> <pr_author>')
        sys.exit(1)

    print('Test run: Validating single file...')
    path = Path(sys.argv[1])
    pr_author = sys.argv[2]
    warnings = 0
    warnings += validate_metadata(path, pr_author)
    warnings += validate_symbol_hooks(path)
    if warnings > 0:
        print(f'Got {warnings} warnings')


def main():
    if len(sys.argv) > 1:
        test_run()
        return

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
