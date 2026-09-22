'''
PROJECT:     PR Validation for Windhawk
LICENSE:     MIT (https://spdx.org/licenses/MIT)
PURPOSE:     Verifies the mod information in the modified mods.
COPYRIGHT:   Copyright 2023 Mark Jansen <mark.jansen@reactos.org>
'''

import http.client
import json
import math
import os
import re
import sys
import time
import unicodedata
import urllib.error
import urllib.parse
import urllib.request
from functools import cache
from io import StringIO
from pathlib import Path
from typing import Callable, Optional, TextIO, Tuple

import yaml

from extract_mod_symbols import get_mod_symbols

DISALLOWED_AUTHORS = [
    # https://github.com/ramensoftware/windhawk-mods/pull/676
    'arukateru',
]


# A reviewer adds this label to a pull request once they've confirmed that the
# X (Twitter) account and the GitHub account belong to the same person.
TWITTER_VERIFIED_LABEL = 'twitter-verified'


@cache
def get_pr_labels() -> set[str]:
    """Labels that are on the pull request, as passed in by the workflow."""
    return set(json.loads(os.environ.get('PR_LABELS', '[]')))


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


CALLBACK_SIGNATURES: dict[str, list[str]] = {
    'Wh_ModInit': ['BOOL Wh_ModInit()'],
    'Wh_ModAfterInit': ['void Wh_ModAfterInit()'],
    'Wh_ModBeforeUninit': ['void Wh_ModBeforeUninit()'],
    'Wh_ModUninit': ['void Wh_ModUninit()'],
    'Wh_ModSettingsChanged': [
        'void Wh_ModSettingsChanged()',
        'BOOL Wh_ModSettingsChanged(BOOL* bReload)',
    ],
    'WhTool_ModInit': ['BOOL WhTool_ModInit()'],
    'WhTool_ModUninit': ['void WhTool_ModUninit()'],
    'WhTool_ModSettingsChanged': ['void WhTool_ModSettingsChanged()'],
    'WhTool_ModEntryPoint': ['void WhTool_ModEntryPoint()'],
}


# A tool mod runs in Windhawk's own processes rather than being injected into
# other programs. Its Windhawk targets must be exactly one of these sets.
TOOL_MOD_ALLOWED_INCLUDES = [
    ['windhawk.exe'],
    ['windhawk.exe', 'windhawk-mod.exe'],
    ['windhawk.exe', 'windhawk-mod.exe', 'windhawk-mod-uiaccess.exe'],
    ['windhawk.exe', 'windhawk-mod.exe', 'windhawk-mod-elevated.exe'],
    [
        'windhawk.exe',
        'windhawk-mod.exe',
        'windhawk-mod-uiaccess.exe',
        'windhawk-mod-elevated.exe',
    ],
]


# RFC 3986 unreserved and reserved characters, plus % for percent-encoding.
# Anything else, whitespace included, has to be percent-encoded.
URL_ALLOWED_CHARS = r"0-9A-Za-z\-._~:/?#\[\]@!$&'()*+,;=%"

# Scheme, dotted host, optional port, optional path/query/fragment.
URL_PATTERN = (
    r'https?://'
    r'[0-9A-Za-z]([0-9A-Za-z-]*[0-9A-Za-z])?'
    r'(\.[0-9A-Za-z]([0-9A-Za-z-]*[0-9A-Za-z])?)+'
    r'(:[0-9]+)?'
    rf'([/?#][{URL_ALLOWED_CHARS}]*)?'
)


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


FETCH_ATTEMPTS = 3


class FetchError(Exception):
    """A fetch failed even after retries, most likely due to a transient network
    problem rather than anything in the mod being validated."""


def fetch_url(url: str) -> bytes:
    """Fetch a URL, retrying connection errors and 5xx responses. 4xx responses
    are raised as HTTPError right away so callers can treat 404 as "not found"."""
    last_error: Optional[Exception] = None
    for attempt in range(1, FETCH_ATTEMPTS + 1):
        try:
            with urllib.request.urlopen(url) as response:
                return response.read()
        except urllib.error.HTTPError as e:
            if e.code < 500:
                raise
            last_error = e
        except (OSError, http.client.HTTPException) as e:
            last_error = e

        if attempt < FETCH_ATTEMPTS:
            print(f'Fetching {url} failed ({last_error!r}), retrying...')
            time.sleep(2)

    raise FetchError(
        f'Failed to fetch {url} after {FETCH_ATTEMPTS} attempts ({last_error!r}).'
        ' This is most likely a temporary network error unrelated to the mod'
        ' itself, re-run the workflow to try again.'
    )


@cache
def get_mod_author_data():
    url = 'https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/pages/mod_author_data.json'
    return json.loads(fetch_url(url))


@cache
def get_valid_license_identifiers_lowercase():
    url = 'https://spdx.org/licenses/licenses.json'
    data = json.loads(fetch_url(url))
    return {license['licenseId'].lower() for license in data['licenses']}


def is_valid_license_identifier(license_id: str):
    return license_id.lower() in get_valid_license_identifiers_lowercase()


@cache
def get_existing_mod_metadata(mod_id: str) -> Optional[dict]:
    """Fetch existing mod metadata from mods.windhawk.net, or None if mod doesn't exist."""
    try:
        url = f'https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/pages/mods/{urllib.parse.quote(mod_id)}.wh.cpp'
        content = fetch_url(url).decode('utf-8')

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
        url = f'https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/pages/mods/{urllib.parse.quote(mod_id)}/versions.json'
        data = json.loads(fetch_url(url))
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
        """Validate value is a well-formed http(s) URL."""
        if not re.match(r'https?://', self.value):
            self.warn('@@ must start with "http://" or "https://"')
            return self

        disallowed = set(re.findall(f'[^{URL_ALLOWED_CHARS}]', self.value))
        if disallowed:
            chars = ', '.join(f'U+{ord(c):04X}' for c in sorted(disallowed))
            self.warn(
                f'@@ contains characters which are not allowed in a URL ({chars}),'
                ' they must be percent-encoded'
            )
        elif not re.fullmatch(URL_PATTERN, self.value):
            self.warn(f'@@ is not a valid URL: "{self.value}"')

        return self

    def validate_no_tabs(self) -> 'PropertyValidator':
        """Validate value contains no tab characters."""
        if '\t' in self.value:
            self.warn('@@ must not contain tab characters')
        return self


class ModMetadataValidator:
    """High-level validator that orchestrates all metadata validations."""

    def __init__(
        self,
        path: Path,
        properties: dict[ModPropertyKey, ModPropertyValue],
        expected_author: str,
        mod_source: str,
    ):
        self.ctx = ValidationContext(path)
        self.properties = properties
        self.expected_author = expected_author
        self.mod_source = mod_source
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

    def property_variants(self, key_name: str) -> list[PropertyValidator]:
        """Get validators for the given key and all of its language variants."""
        return [
            PropertyValidator(
                self.ctx,
                key_name if language is None else f'{key_name}:{language}',
                value,
                line_number,
            )
            for (key, language), (value, line_number) in self.properties.items()
            if key == key_name
        ]

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
        self.validate_description()
        self.validate_architecture()
        self.validate_tool_mod()

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
                    f' "{self.existing_metadata["github"]}", got "{prop.value}"\n'
                    'Note that only the original author of the mod is allowed to'
                    ' submit updates.\n'
                    'If you are not the original author, you might want to contact'
                    ' them to submit the update instead.\n'
                    'For more information about submitting a mod update, refer to the'
                    ' "Submitting a Mod Update" section in the repository\'s README.md.'
                )

        expected = f'https://github.com/{self.expected_author}'
        if not prop.value.startswith('https://github.com/'):
            prop.warn('@@ must start with https://github.com/')
        elif prop.value != expected and prop.value.lower() == expected.lower():
            prop.warn(f'Expected @@ to be {expected} (case-sensitive)')
        elif prop.value == expected + '/':
            prop.warn(f'Expected @@ to be {expected} (no trailing slash)')
        elif prop.value.startswith(expected + '/'):
            prop.warn(f'Expected @@ to be {expected} (user profile URL only)')
        elif prop.value != expected:
            prop.warn(
                f'Expected @@ ({prop.value}) to match the pull request author'
                f' ({expected}).\n'
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

        min_len = 6
        max_len = 48
        if len(prop.value) < min_len or len(prop.value) > max_len:
            prop.warn(f'@@ must be between {min_len} and {max_len} characters')

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
        for variant in self.property_variants('author'):
            variant.validate_no_tabs()

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

        min_len = 3
        max_len = 28
        if len(prop.value) < min_len or len(prop.value) > max_len:
            prop.warn(f'@@ must be between {min_len} and {max_len} characters')

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
                # Not used by anyone else, so it takes a manual check that the
                # same person owns both accounts.
                if TWITTER_VERIFIED_LABEL not in get_pr_labels():
                    prop.warn(
                        '@@ requires manual verification\n\n'
                        'To verify your X (Twitter) account, please send me'
                        ' (https://x.com/m417z) a direct message with the following'
                        ' content:\n\n'
                        'I attest that I\'m the sole owner of both this Twitter account'
                        f' ({prop.value}) and the following GitHub account:'
                        f' {self.github_url}'
                    )

        prop.validate_url_format()

        if not re.match(r'https://(x|twitter)\.com/', prop.value):
            prop.warn('@@ must start with https://x.com/ or https://twitter.com/')
        elif not re.match(r'https://(x|twitter)\.com/[^/]+$', prop.value):
            prop.warn('@@ must be an X (Twitter) profile URL with no extra slashes')

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

        def is_allowed_option(option: str) -> bool:
            return bool(
                option.startswith('-l')
                or option
                in [
                    '-DWIN32_LEAN_AND_MEAN',
                    '-fms-extensions',
                    '-ffp-exception-behavior=maytrap',
                ]
            )

        options = prop.value.split()
        disallowed_options = [opt for opt in options if not is_allowed_option(opt)]
        if disallowed_options:
            prop.warn(f'@@ require manual verification: {" ".join(disallowed_options)}')

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
        """Validate name exists and is unique."""
        for variant in self.property_variants('name'):
            variant.validate_no_tabs()

        prop = self.property('name', warn_if_missing=True)
        if not prop:
            return

        min_len = 6
        max_len = 68
        if len(prop.value) < min_len or len(prop.value) > max_len:
            prop.warn(f'@@ must be between {min_len} and {max_len} characters')

        # Check for duplicate names across existing mods
        filename_mod_id = self.ctx.path.name.removesuffix('.cpp').removesuffix('.wh')
        all_names = get_all_mod_names()
        for other_mod_id, other_name in all_names.items():
            if (
                other_mod_id != filename_mod_id
                and other_name.lower() == prop.value.lower()
            ):
                prop.warn(f'@@ "{prop.value}" is already used by mod "{other_mod_id}"')
                break

    def validate_description(self):
        """Validate description exists."""
        for variant in self.property_variants('description'):
            variant.validate_no_tabs()

        prop = self.property('description', warn_if_missing=True)
        if not prop:
            return

        min_len = 30
        max_len = 250
        if len(prop.value) < min_len or len(prop.value) > max_len:
            prop.warn(f'@@ must be between {min_len} and {max_len} characters')

    def validate_architecture(self):
        """Validate architecture values."""
        prop = self.property('architecture')
        if not prop:
            return

        msg = ''
        for arch in prop.value.split('\n'):
            if arch.strip() == '':
                pass
            elif arch not in {'x86', 'x86-64', 'amd64', 'arm64'}:
                msg += f'Unknown architecture "{arch}"\n'
            elif arch not in {'x86', 'x86-64'}:
                msg += (
                    f'Architecture "{arch}" isn\'t commonly used, manual verification'
                    ' is required\n'
                )

        if msg:
            prop.warn(msg.rstrip('\n'))

    def validate_tool_mod(self):
        """Validate the metadata of a tool mod: one that targets windhawk.exe
        with a WhTool_ModInit entry point, or any windhawk-*.exe process."""
        prop = self.property('include')
        if not prop:
            return

        includes = {x.lower() for x in prop.value.split('\n') if x != ''}
        windhawk_includes = {
            x for x in includes if re.fullmatch(r'windhawk(-[\w-]+)?\.exe', x)
        }
        is_tool_mod = any(x != 'windhawk.exe' for x in windhawk_includes) or (
            'windhawk.exe' in windhawk_includes
            and re.search(r'\bWhTool_ModInit\b', self.mod_source) is not None
        )
        if not is_tool_mod:
            return

        if windhawk_includes not in [set(x) for x in TOOL_MOD_ALLOWED_INCLUDES]:
            prop.warn(
                'Tool mods must @@ exactly one of the following combinations of'
                ' Windhawk processes:\n'
                + '\n'.join(f'* {", ".join(x)}' for x in TOOL_MOD_ALLOWED_INCLUDES)
            )

        arch_prop = self.property('architecture')
        if arch_prop:
            arch_prop.warn('@@ must not be specified for tool mods')


def validate_metadata(path: Path, mod_source: str, expected_author: str) -> int:
    properties, initial_warnings = get_mod_file_metadata(
        StringIO(mod_source),
        warn_callback=lambda line, msg: add_warning(path, line, msg),
    )

    # Validate metadata properties
    validator = ModMetadataValidator(path, properties, expected_author, mod_source)
    metadata_warnings = validator.validate_all()

    # Validate file path
    file_warnings = 0
    if not path.name.endswith('.wh.cpp'):
        file_warnings += add_warning(path, 1, 'Filename should end with .wh.cpp')
    if path.parent != Path('mods'):
        file_warnings += add_warning(path, 1, 'File is not placed in the mods folder')

    return initial_warnings + metadata_warnings + file_warnings


def validate_marker_block(
    path: Path,
    source: str,
    marker_name: str,
    label: str,
    *,
    required: bool,
) -> int:
    """Validate a `// ==X== \\n /* ... */ \\n // ==/X==` block in a mod source.

    Args:
        path: source file path (used in warning output).
        source: full file contents.
        marker_name: the marker text without `==` decoration, e.g.
            `WindhawkModReadme`.
        label: human-friendly name used in warning messages, e.g. `README`.
        required: if True, warn when the block is absent.

    Returns:
        Total number of warnings emitted.
    """
    open_marker = f'=={marker_name}=='
    close_marker = f'==/{marker_name}=='

    # The body capture group keeps its surrounding whitespace so we can check
    # for the required newlines around /* and */.
    block_re = re.compile(
        r'^//[ \t]+' + re.escape(open_marker) + r'[ \t]*$'
        r'\s*/\*([\s\S]+?)\*/\s*'
        r'^//[ \t]+' + re.escape(close_marker) + r'[ \t]*$',
        re.MULTILINE,
    )
    match = block_re.search(source)

    warnings = 0

    def line_of(pos: int) -> int:
        return source.count('\n', 0, pos) + 1

    def find_all(haystack: str, needle: str):
        start = 0
        while (pos := haystack.find(needle, start)) != -1:
            yield pos
            start = pos + len(needle)

    # Stray-marker scan: warn about any line that looks like a marker comment
    # line but isn't the one consumed by the matched block. The pattern is
    # intentionally more permissive than block_re (it tolerates leading
    # whitespace) so improperly indented marker lines are still caught, while
    # incidental occurrences of the marker text inside other comments (e.g. "//
    # must conform to ==X== above") are not. This runs even when the block is
    # missing or optional.
    open_marker_line_re = re.compile(
        r'^[ \t]*//[ \t]+' + re.escape(open_marker) + r'[ \t]*$',
        re.MULTILINE,
    )
    close_marker_line_re = re.compile(
        r'^[ \t]*//[ \t]+' + re.escape(close_marker) + r'[ \t]*$',
        re.MULTILINE,
    )

    # Sentinel -1 (str.find's not-found value) when there is no matched block,
    # so the skip-the-matched-line check below naturally fails.
    matched_open_pos = -1
    matched_close_pos = -1
    if match is not None:
        matched_open_pos = source.find(open_marker, match.start(), match.end())
        matched_close_pos = source.rfind(close_marker, match.start(), match.end())

    for m in open_marker_line_re.finditer(source):
        if m.start() <= matched_open_pos < m.end():
            continue
        warnings += add_warning(
            path, line_of(m.start()), f'Unexpected extra "{open_marker}" marker'
        )

    for m in close_marker_line_re.finditer(source):
        if m.start() <= matched_close_pos < m.end():
            continue
        warnings += add_warning(
            path, line_of(m.start()), f'Unexpected extra "{close_marker}" marker'
        )

    if match is None:
        if required:
            warnings += add_warning(
                path,
                1,
                f'Mod source must contain a {label} block ({open_marker})',
            )
        return warnings

    body = match.group(1)
    body_start = match.start(1)
    open_line = line_of(matched_open_pos)

    # The body must not contain another */ - that would actually terminate the
    # comment block at that point in C/C++.
    for idx in find_all(body, '*/'):
        warnings += add_warning(
            path,
            line_of(body_start + idx),
            f'{label} body must not contain an additional "*/"',
        )

    # /* must be followed by a newline and */ must be preceded by one
    # (trailing/leading spaces or tabs on those lines are tolerated).
    if not re.match(r'[ \t]*\n', body):
        warnings += add_warning(
            path, open_line, f'{label} /* must be followed by a newline'
        )
    if not re.search(r'\n[ \t]*\Z', body):
        warnings += add_warning(
            path, open_line, f'{label} */ must be preceded by a newline'
        )

    # The body must contain non-whitespace content.
    if body.strip() == '':
        warnings += add_warning(path, open_line, f'{label} block must not be empty')

    return warnings


def validate_readme(path: Path, mod_source: str) -> int:
    """Validate the mod's README block."""
    return validate_marker_block(
        path, mod_source, 'WindhawkModReadme', 'README', required=True
    )


def validate_settings(path: Path, mod_source: str) -> int:
    """Validate the mod's settings block, if present."""
    warnings = validate_marker_block(
        path, mod_source, 'WindhawkModSettings', 'Settings', required=False
    )
    warnings += validate_settings_yaml(path, mod_source)
    return warnings


class SettingsYamlLoader(yaml.SafeLoader):
    """PyYAML loader that behaves like js-yaml's JSON_SCHEMA, which Windhawk
    parses the settings block with.

    Plain scalars resolve only to null, bool, int and float, with js-yaml's
    patterns (so e.g. `yes`, `2024-01-01` and `<<` are plain strings), explicit
    tags are limited to the same set, mapping keys are strings, and duplicate
    keys are an error.
    """

    # The failsafe types and the fallback for unknown tags; the JSON_SCHEMA
    # scalar types are registered with add_scalar_type below.
    yaml_constructors = {
        tag: yaml.SafeLoader.yaml_constructors[tag]
        for tag in (
            None,
            'tag:yaml.org,2002:str',
            'tag:yaml.org,2002:seq',
            'tag:yaml.org,2002:map',
        )
    }
    yaml_implicit_resolvers = {}

    @classmethod
    def add_scalar_type(cls, tag: str, regexp: re.Pattern, construct: Callable):
        """Register a scalar type that plain scalars matching regexp resolve to.

        Like js-yaml, an explicit tag on a scalar that doesn't match is an error.
        """

        def construct_checked(loader, node):
            value = loader.construct_scalar(node)
            if not regexp.match(value):
                raise yaml.constructor.ConstructorError(
                    None,
                    None,
                    f'cannot resolve {value!r} with explicit tag {node.tag}',
                    node.start_mark,
                )
            return construct(loader, node)

        cls.add_implicit_resolver(tag, regexp, None)
        cls.add_constructor(tag, construct_checked)

    def construct_mapping(self, node, deep=False):
        if not isinstance(node, yaml.MappingNode):
            raise yaml.constructor.ConstructorError(
                None,
                None,
                f'expected a mapping node, but found {node.id}',
                node.start_mark,
            )
        mapping = {}
        for key_node, value_node in node.value:
            key = self.key_string(key_node)
            if key in mapping:
                raise yaml.constructor.ConstructorError(
                    None, None, f'found duplicate key {key!r}', key_node.start_mark
                )
            mapping[key] = self.construct_object(value_node, deep=deep)
        return mapping

    def key_string(self, key_node) -> str:
        """The mapping key as JavaScript's String() would render it."""
        key = self.construct_object(key_node)
        if key is None:
            return 'null'
        if isinstance(key, bool):
            return 'true' if key else 'false'
        # Integral floats print without a fraction, up to where JavaScript
        # switches to exponent notation.
        if isinstance(key, float) and key.is_integer() and abs(key) < 1e21:
            return str(int(key))
        if isinstance(key, (int, float, str)):
            return str(key)
        raise yaml.constructor.ConstructorError(
            None, None, 'complex mapping keys are not supported', key_node.start_mark
        )


def construct_js_number(loader: SettingsYamlLoader, node):
    """Number construction with js-yaml's semantics for both int and float."""
    value = loader.construct_scalar(node).replace('_', '')
    sign = -1 if value.startswith('-') else 1
    value = value.lstrip('+-')
    if value.lower() == '.inf':
        return sign * math.inf
    if value.lower() == '.nan':
        return math.nan
    if value[:2] in ('0b', '0x', '0o'):
        return sign * int(value, 0)
    if value.isdigit():
        return sign * int(value)
    return sign * float(value)


# js-yaml's JSON_SCHEMA scalar types, in its resolution order.
for _tag, _pattern, _construct in [
    ('null', r'~|null|Null|NULL|', lambda loader, node: None),
    (
        'bool',
        r'true|True|TRUE|false|False|FALSE',
        lambda loader, node: loader.construct_scalar(node) in ('true', 'True', 'TRUE'),
    ),
    (
        'int',
        r'[-+]?(?:0b[01_]*[01]|0x[0-9a-fA-F_]*[0-9a-fA-F]|0o[0-7_]*[0-7]'
        r'|[1-9][0-9_]*(?<!_)|0(?:[0-9][0-9_]*(?<!_))?)',
        construct_js_number,
    ),
    (
        'float',
        r'(?:[-+]?[0-9][0-9_]*(?:\.[0-9_]*)?(?:[eE][-+]?[0-9]+)?'
        r'|\.[0-9_]+(?:[eE][-+]?[0-9]+)?)(?<!_)'
        r'|[-+]?\.(?:inf|Inf|INF)|\.(?:nan|NaN|NAN)',
        construct_js_number,
    ),
]:
    SettingsYamlLoader.add_scalar_type(
        f'tag:yaml.org,2002:{_tag}', re.compile(rf'(?:{_pattern})\Z'), _construct
    )


def is_js_number(value) -> bool:
    """A finite number, as JSON schema's "number" type accepts it. Unlike in
    Python, a boolean is not a number."""
    return (
        isinstance(value, (int, float))
        and not isinstance(value, bool)
        and math.isfinite(value)
    )


class SettingsSchemaError(Exception):
    """A settings structure that Windhawk rejects, located by its YAML node."""

    def __init__(self, node, message: str):
        super().__init__(message)
        self.node = node


class SettingsSchemaChecker:
    """Checks a parsed settings node tree against the structure Windhawk
    accepts: a non-empty array of objects, each with exactly one setting key
    plus optional $name, $description and $options (each optionally suffixed
    with a :language), where a setting's value is a boolean, number, string,
    array of numbers, array of strings, nested settings, or array of nested
    settings.
    """

    SETTING_KEY_RE = re.compile(r'[0-9A-Za-z_-]+')
    TEXT_META_KEY_RE = re.compile(r'\$(?:name|description)(?::[a-z]{2}(?:-[A-Z]{2})?)?')
    OPTIONS_META_KEY_RE = re.compile(r'\$options(?::[a-z]{2}(?:-[A-Z]{2})?)?')

    def __init__(self, loader: SettingsYamlLoader):
        self.loader = loader

    def value(self, node):
        return self.loader.construct_object(node)

    def check_settings(self, node):
        if not isinstance(node, yaml.SequenceNode):
            raise SettingsSchemaError(node, 'Settings must be a YAML array')
        if not node.value:
            raise SettingsSchemaError(
                node, 'Settings array must have at least one item'
            )
        for item_node in node.value:
            self.check_settings_object(item_node)

    def check_settings_object(self, node):
        if not isinstance(node, yaml.MappingNode):
            raise SettingsSchemaError(node, 'Settings array items must be objects')
        if not node.value:
            raise SettingsSchemaError(
                node, 'Settings object must have at least one property'
            )

        setting_keys = []
        for key_node, value_node in node.value:
            key = self.loader.key_string(key_node)
            if self.SETTING_KEY_RE.fullmatch(key):
                setting_keys.append(key)
                self.check_setting_value(key, value_node)
            elif self.TEXT_META_KEY_RE.fullmatch(key):
                if not isinstance(self.value(value_node), str):
                    raise SettingsSchemaError(
                        value_node, f'Settings property "{key}" must be a string'
                    )
            elif self.OPTIONS_META_KEY_RE.fullmatch(key):
                self.check_options(key, value_node)
            elif key.startswith('$'):
                raise SettingsSchemaError(
                    key_node,
                    f'Unsupported settings property "{key}"; only $name, $description'
                    ' and $options are allowed, optionally with a :language suffix',
                )
            else:
                raise SettingsSchemaError(
                    key_node,
                    f'Invalid settings key "{key}"; keys may only contain letters,'
                    ' digits, "_" and "-"',
                )

        if not setting_keys:
            raise SettingsSchemaError(
                node, 'Settings object has no setting key, only $ properties'
            )
        if len(setting_keys) > 1:
            raise SettingsSchemaError(
                node,
                'Settings object has more than one setting key: '
                + ', '.join(setting_keys),
            )

    def check_setting_value(self, key: str, node):
        value = self.value(node)
        if isinstance(value, (bool, str)) or is_js_number(value):
            return
        if value is None:
            raise SettingsSchemaError(node, f'Setting "{key}" has no value')
        if isinstance(value, float):
            raise SettingsSchemaError(node, f'Setting "{key}" must be a finite number')
        if isinstance(value, dict):
            raise SettingsSchemaError(
                node,
                f'Setting "{key}" must not be an object; nested settings are an'
                ' array of objects',
            )
        if not isinstance(value, list):
            raise SettingsSchemaError(
                node, f'Setting "{key}" must be a boolean, number, string or array'
            )

        if not value:
            raise SettingsSchemaError(
                node, f'Setting "{key}" array must have at least one item'
            )
        if all(is_js_number(x) for x in value) or all(
            isinstance(x, str) for x in value
        ):
            return
        if all(isinstance(x, dict) for x in value):
            self.check_settings(node)
        elif all(isinstance(x, list) for x in value):
            for item_node in node.value:
                self.check_settings(item_node)
        else:
            raise SettingsSchemaError(
                node,
                f'Setting "{key}" array must contain only numbers, only strings, or'
                ' only nested settings',
            )

    def check_options(self, key: str, node):
        value = self.value(node)
        if not isinstance(value, list):
            raise SettingsSchemaError(
                node, f'Settings property "{key}" must be an array'
            )
        if len(value) < 2:
            raise SettingsSchemaError(
                node, f'Settings property "{key}" must have at least two items'
            )
        for item_node in node.value:
            if not isinstance(self.value(item_node), dict):
                raise SettingsSchemaError(
                    item_node, f'Settings property "{key}" items must be objects'
                )
            if len(item_node.value) != 1:
                raise SettingsSchemaError(
                    item_node,
                    f'Settings property "{key}" items must have exactly one property',
                )
            ((_, label_node),) = item_node.value
            if not isinstance(self.value(label_node), str):
                raise SettingsSchemaError(
                    label_node, f'Settings property "{key}" labels must be strings'
                )


def validate_settings_yaml(path: Path, mod_source: str) -> int:
    """Validate that the settings block parses as the structure Windhawk expects.

    Mirrors Windhawk's extraction (windhawk-vscode modSourceUtils.ts,
    extractInitialSettings): the block body is parsed like js-yaml with its
    JSON_SCHEMA and then checked against Windhawk's settings schema, so that
    anything Windhawk rejects is reported here. The structural integrity of the
    comment block itself (markers, /* */ placement, etc.) is reported separately
    by validate_marker_block, so a block that doesn't match the pattern below is
    simply skipped here.
    """
    # Use the same extraction Windhawk uses. The surrounding \s* consumes the
    # whitespace around the body, so we parse exactly the text Windhawk feeds to
    # its YAML parser.
    block_re = re.compile(
        r'^//[ \t]+==WindhawkModSettings==[ \t]*$'
        r'\s*/\*\s*([\s\S]+?)\s*\*/\s*'
        r'^//[ \t]+==/WindhawkModSettings==[ \t]*$',
        re.MULTILINE,
    )
    match = block_re.search(mod_source)
    if match is None:
        return 0

    body = match.group(1)
    body_start_line = mod_source.count('\n', 0, match.start(1)) + 1

    loader = SettingsYamlLoader(body)
    try:
        node = loader.get_single_node()
        if node is None:
            return add_warning(path, body_start_line, 'Settings must be a YAML array')
        # Constructing the whole document reports duplicate keys and bad tags,
        # and caches every node's value for the checker to read.
        loader.construct_object(node, deep=True)
        SettingsSchemaChecker(loader).check_settings(node)
    except yaml.YAMLError as e:
        line = body_start_line
        mark = getattr(e, 'problem_mark', None)
        if mark is not None:
            # problem_mark.line is 0-based and relative to the parsed body.
            line = body_start_line + mark.line
        return add_warning(path, line, f'Settings block is not valid YAML: {e}')
    except SettingsSchemaError as e:
        return add_warning(path, body_start_line + e.node.start_mark.line, str(e))
    finally:
        loader.dispose()

    return 0


@cache
def get_all_mod_names() -> dict[str, str]:
    """Scan all mod files and return a mapping of mod_id (from filename) to @name."""
    result = {}
    for path in Path('mods').glob('*.wh.cpp'):
        mod_id = path.name.removesuffix('.wh.cpp')
        with path.open(encoding='utf-8') as f:
            properties, _ = get_mod_file_metadata(f)
        name_prop = properties.get(('name', None))
        if name_prop:
            result[mod_id] = name_prop[0]
    return result


@cache
def get_existing_windows_file_names():
    url = 'https://winbindex.m417z.com/data/filenames.json'
    return json.loads(fetch_url(url))


def is_existing_windows_file_name(name: str):
    # Temporary special case - not yet in stable Windows builds.
    if name.lower() in ['systemtray.dll']:
        return True

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


def validate_symbol_hooks(path: Path, mod_source: str):
    warnings = 0

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


def validate_encoding(path: Path):
    """Validate that the file is valid UTF-8 without BOM."""
    raw = path.read_bytes()

    try:
        raw.decode('utf-8')
    except UnicodeDecodeError as e:
        return add_warning(path, 1, f'File is not valid UTF-8: {e}')

    warnings = 0

    if raw.startswith(b'\xef\xbb\xbf'):
        warnings += add_warning(path, 1, 'File must not start with a UTF-8 BOM')

    return warnings


def validate_specific_keywords(path: Path, mod_source: str):
    """Check for specific keywords in mod source code."""
    warnings = 0

    # Split on newlines only; splitlines() would also split on vertical tab,
    # form feed and similar, hiding them from the control character check.
    mod_source_lines = mod_source.split('\n')

    # fmt: off
    keyword_patterns = [
        (r'\bInternalWh', 'InternalWh', 'Avoid using internal API unless absolutely necessary'),
        (r'\bWH_EDITING\b', 'WH_EDITING', 'Avoid using WH_EDITING unless absolutely necessary'),
        (r'\bWH_MOD\b', 'WH_MOD', 'Avoid using WH_MOD unless absolutely necessary'),
        (r'(^|,)\s*GWL_WNDPROC\b', 'GWL_WNDPROC', '`WindhawkUtils::SetWindowSubclassFromAnyThread` is usually preferred for subclassing'),
        (r'(^|,)\s*GWLP_WNDPROC\b', 'GWLP_WNDPROC', '`WindhawkUtils::SetWindowSubclassFromAnyThread` is usually preferred for subclassing'),
        (r'\bWh_FindFirstSymbol\b', 'Wh_FindFirstSymbol', '`WindhawkUtils::HookSymbols` is usually preferred for symbol hooking'),
        (r'\bWh_FindNextSymbol\b', 'Wh_FindNextSymbol', '`WindhawkUtils::HookSymbols` is usually preferred for symbol hooking'),
        (r'\bWh_FindCloseSymbol\b', 'Wh_FindCloseSymbol', '`WindhawkUtils::HookSymbols` is usually preferred for symbol hooking'),
        (r'\bnoUndecoratedSymbols\b', 'noUndecoratedSymbols', 'Decorated symbols don\'t support online caching, undecorated symbols are usually preferred'),
        (r'\bWh_SetFunctionHookT\b', 'Wh_SetFunctionHookT', 'Deprecated, use `WindhawkUtils::SetFunctionHook` instead'),
    ]
    # fmt: on

    for line_num, line in enumerate(mod_source_lines, start=1):
        for pattern, word, description in keyword_patterns:
            if re.search(pattern, line):
                # Skip GWL(P)_WNDPROC when used with GetWindowLong(Ptr)
                if word in ('GWL_WNDPROC', 'GWLP_WNDPROC') and re.search(
                    r'GetWindowLong(Ptr)?[AW]?\s*\([^,]+,\s*GWLP?_WNDPROC\s*\)', line
                ):
                    continue

                warnings += add_warning(
                    path,
                    line_num,
                    f'Line requires manual inspection for "{word}": {description}',
                )

        hidden_ws = [
            c
            for c in line
            if unicodedata.category(c) == 'Cf'
            or (unicodedata.category(c) == 'Zs' and c != ' ')
        ]
        if hidden_ws:
            chars = ', '.join(f'U+{ord(c):04X}' for c in sorted(set(hidden_ws)))
            warnings += add_warning(
                path,
                line_num,
                f'Line contains {len(hidden_ws)} non-standard whitespace characters'
                f' ({chars}), requires manual inspection',
            )

        control_chars = [
            c for c in line if unicodedata.category(c) == 'Cc' and c != '\t'
        ]
        if control_chars:
            chars = ', '.join(f'U+{ord(c):04X}' for c in sorted(set(control_chars)))
            warnings += add_warning(
                path,
                line_num,
                f'Line contains {len(control_chars)} control characters ({chars}),'
                ' which are not allowed',
            )

    return warnings


def normalize_callback_param_types(params: str) -> str:
    """Strip parameter names and normalize whitespace/pointer style in a C parameter list."""
    p = re.sub(r'\s+', ' ', params.strip())
    if p in ('', 'void', 'VOID'):
        return ''

    types = []
    for arg in p.split(','):
        arg = re.sub(r'\s*\*\s*', '* ', arg.strip())
        tokens = arg.split()
        # Drop the trailing parameter name if present.
        if len(tokens) > 1 and re.fullmatch(r'\w+', tokens[-1]):
            tokens = tokens[:-1]
        types.append(' '.join(tokens))

    return ', '.join(types)


def normalize_return_type(return_type: str) -> str:
    """Treat VOID (the Windows macro) as a synonym for void."""
    return 'void' if return_type == 'VOID' else return_type


def validate_callback_signatures(path: Path, mod_source: str):
    """Validate signatures of well-known Windhawk mod callback functions."""
    warnings = 0

    for callback_name, expected_signatures in CALLBACK_SIGNATURES.items():
        # Parse expected signatures once: (return_type, normalized_param_types).
        expected = []
        for sig in expected_signatures:
            m = re.fullmatch(rf'(\w+)\s+{re.escape(callback_name)}\s*\((.*)\)', sig)
            assert m, sig
            expected.append((m.group(1), normalize_callback_param_types(m.group(2))))

        # Match: optional specifiers + return type + callback name + ( params ).
        # Requiring a bare identifier before the name naturally skips function
        # calls (e.g. "= Wh_Mod..." or "(Wh_Mod...").
        pattern = (
            r'((?:\b(?:static|extern(?:\s+"C")?|inline)\s+)*)\b(\w+)\s+'
            + re.escape(callback_name)
            + r'\s*\(([^)]*)\)'
        )
        for match in re.finditer(pattern, mod_source):
            # Skip if inside a single-line comment.
            line_start = mod_source.rfind('\n', 0, match.start()) + 1
            if '//' in mod_source[line_start : match.start()]:
                continue

            line_num = 1 + mod_source[: match.start()].count('\n')
            specifiers, return_type, params = match.groups()

            if specifiers:
                warnings += add_warning(
                    path,
                    line_num,
                    f'Unexpected "{" ".join(specifiers.split())}" before'
                    f' {callback_name}',
                )

            normalized_return_type = normalize_return_type(return_type)
            normalized_params = normalize_callback_param_types(params)

            if any(
                normalized_return_type == exp_ret and normalized_params == exp_params
                for exp_ret, exp_params in expected
            ):
                continue

            expected_list = ' or '.join(f'"{s}"' for s in expected_signatures)
            warnings += add_warning(
                path,
                line_num,
                f'Unexpected {callback_name} signature:'
                f' "{return_type} {callback_name}({params.strip()})".'
                f' Expected: {expected_list}',
            )

    return warnings


def validate_mod_file(path: Path, pr_author: str) -> int:
    mod_source = path.read_text(encoding='utf-8', errors='ignore').removeprefix(
        '\ufeff'
    )

    warnings = validate_encoding(path)
    warnings += validate_metadata(path, mod_source, pr_author)
    warnings += validate_readme(path, mod_source)
    warnings += validate_settings(path, mod_source)
    warnings += validate_symbol_hooks(path, mod_source)
    warnings += validate_specific_keywords(path, mod_source)
    warnings += validate_callback_signatures(path, mod_source)

    return warnings


def test_run():
    if len(sys.argv) != 3:
        print('Test run usage: pr_validation.py <mod_file_path> <pr_author>')
        sys.exit(1)

    print('Test run: Validating single file...')
    path = Path(sys.argv[1])
    pr_author = sys.argv[2]
    warnings = validate_mod_file(path, pr_author)
    if warnings > 0:
        print(f'Got {warnings} warnings')


def validate_pr_changelog(pr_body: str) -> int:
    """Mod updates must describe the changes in the PR description."""
    markers_re = re.compile(
        r'<!--\s*changelog:start\s*-->(.*?)<!--\s*changelog:end\s*-->',
        re.DOTALL | re.IGNORECASE,
    )

    # The sample items of the pull request template, which are to be replaced
    # with the actual changes.
    placeholder_item_re = re.compile(
        r'^[ \t]*\*[ \t]*Changelog item \d+\.\.\.[ \t]*$', re.MULTILINE
    )

    template_hint = (
        ' See the pull request template'
        ' (https://github.com/ramensoftware/windhawk-mods/blob/main/.github/pull_request_template.md?plain=1).'
    )

    matches = markers_re.findall(pr_body)
    if len(matches) != 1:
        return add_warning(
            Path('.github/pull_request_template.md'),
            1,
            'Mod updates must have a changelog in the PR description, between a single'
            ' pair of the "<!-- changelog:start -->" and "<!-- changelog:end -->"'
            f' markers, found {len(matches)} such pairs.' + template_hint,
        )

    changelog = placeholder_item_re.sub('', matches[0]).strip()
    if changelog == '':
        return add_warning(
            Path('.github/pull_request_template.md'),
            1,
            'The changelog between the "<!-- changelog:start -->" and'
            ' "<!-- changelog:end -->" markers in the PR description is empty,'
            ' please describe the changes of this mod update.'
            + template_hint,
        )

    print(f'Changelog:\n{changelog}')
    return 0


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

    # The PR body is sent with CRLF line endings.
    pr_body = os.environ.get('PR_BODY', '').replace('\r\n', '\n')

    if added_count != 0 and '## Mod authorship' not in pr_body:
        warnings += add_warning(
            Path('.github/pull_request_template.md'),
            1,
            'New mod submissions must keep the "## Mod authorship" section from the'
            ' pull request template'
            ' (https://github.com/ramensoftware/windhawk-mods/blob/main/.github/pull_request_template.md?plain=1)'
            ' in the PR description, so reviewers know how the mod was authored.'
            ' Please restore that section and fill it in.',
        )

    if modified_count != 0:
        warnings += validate_pr_changelog(pr_body)

    for path in paths:
        print(f'Checking {path=}')

        path_warnings = validate_mod_file(path, pr_author)
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
    try:
        main()
    except FetchError as e:
        print(f'::error::{e}')
        sys.exit(1)
