import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
import urllib.request
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Optional, Tuple

import win32api


class Architecture(Enum):
    x86 = 1
    x86_64 = 2


@dataclass
class ModInfo:
    id: str
    version: str
    compiler_options: Optional[str]
    architectures: set[Architecture]


def extract_windhawk(installer_path: Path, target_dir: Path):
    print(f'Extracting {installer_path} to {target_dir}...')

    subprocess.check_call(f'"{installer_path}" /S /PORTABLE /D={target_dir}')

    print(f'Extracted')


def download_and_extract_windhawk(windhawk_version: str, target_dir: Path):
    url = f'https://github.com/ramensoftware/windhawk/releases/download/v{windhawk_version}/windhawk_setup.exe'
    print(f'Downloading {url}...')

    with tempfile.TemporaryDirectory() as tmp:
        target_setup_file = Path(tmp) / 'windhawk_setup.exe'

        with urllib.request.urlopen(url) as response:
            with open(target_setup_file, 'wb') as f:
                f.write(response.read())

        extract_windhawk(target_setup_file, target_dir)


def get_engine_path(windhawk_dir: Path) -> Optional[str]:
    config = (windhawk_dir / 'windhawk.ini').read_text(encoding='utf-16')
    p = r'^\s*EnginePath\s*=\s*(.*?)\s*$'
    match = re.search(p, config, flags=re.MULTILINE)
    return match.group(1) if match else None


def get_file_version(path: Path) -> Tuple[int, int, int, int]:
    info = win32api.GetFileVersionInfo(str(path), '\\')
    ms = info['FileVersionMS']
    ls = info['FileVersionLS']
    return (
        win32api.HIWORD(ms),
        win32api.LOWORD(ms),
        win32api.HIWORD(ls),
        win32api.LOWORD(ls),
    )


def str_to_file_version(version: str) -> Tuple[int, int, int, int]:
    parts = [int(x) for x in version.split('.')]
    if len(parts) != 4:
        raise RuntimeError(f'Invalid file version: {version}')

    return (parts[0], parts[1], parts[2], parts[3])


def get_mod_info(path: Path) -> ModInfo:
    id = None
    version = None
    compiler_options = None
    architectures = set()

    inside_metadata_block = False
    with path.open(encoding='utf-8') as f:
        for line in f:
            line = line.rstrip('\n')

            if not inside_metadata_block:
                if re.fullmatch(r'//[ \t]+==WindhawkMod==[ \t]*', line):
                    inside_metadata_block = True
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
                raise RuntimeError(f'Invalid metadata line format: {line}')

            key = match.group(1)
            language = match.group(2)
            value = match.group(3)

            if key not in ['id', 'version', 'compilerOptions', 'architecture']:
                continue

            if language is not None:
                raise RuntimeError('Language cannot be specified for this property')

            if key == 'id':
                id = value
            elif key == 'version':
                version = value
            elif key == 'compilerOptions':
                compiler_options = value
            elif key == 'architecture':
                if value == 'x86':
                    architectures.add(Architecture.x86)
                elif value == 'x86-64':
                    architectures.add(Architecture.x86_64)
                else:
                    raise RuntimeError(f'Invalid architecture: {value}')

    if id is None:
        raise RuntimeError('@id is not specified')

    if version is None:
        raise RuntimeError('@version is not specified')

    if not architectures:
        architectures = {Architecture.x86, Architecture.x86_64}

    return ModInfo(id, version, compiler_options, architectures)


def check_mod(mod_file: Path, windhawk_dir: Path):
    print(f'Checking {mod_file}')

    windhawk_version = get_file_version(windhawk_dir / 'windhawk.exe')

    compiler_path = windhawk_dir / 'Compiler' / 'bin' / 'g++.exe'

    engine_relative_path = get_engine_path(windhawk_dir)
    if engine_relative_path is None:
        raise RuntimeError('Could not find engine path in windhawk.ini')

    mod_info = get_mod_info(mod_file)

    extra_args = []
    if mod_info.compiler_options is not None:
        if '"' in mod_info.compiler_options:
            raise RuntimeError('Compiler options cannot contain double quotes')

        extra_args = mod_info.compiler_options.split()

    succeeded = True

    for arch in mod_info.architectures:
        engine_lib_path = windhawk_dir / engine_relative_path
        if arch == Architecture.x86:
            engine_lib_path /= '32'
        elif arch == Architecture.x86_64:
            engine_lib_path /= '64'
        engine_lib_path /= 'windhawk.lib'

        if arch == Architecture.x86:
            compiler_target = 'i686-w64-mingw32'
        elif arch == Architecture.x86_64:
            compiler_target = 'x86_64-w64-mingw32'

        with tempfile.TemporaryDirectory() as tmp:
            cpp_version = '20'
            if (
                windhawk_version >= str_to_file_version('1.5.0.0')
                # Temporary compatibility rules:
                and (mod_info.id, mod_info.version)
                not in [
                    ('chrome-ui-tweaks', '1.0.0'),
                    ('taskbar-vertical', '1.0'),
                ],
            ):
                cpp_version = '23'

            print(f'{windhawk_version=}')
            print(f'{str_to_file_version('1.5.0.0')=}')
            print(f'{windhawk_version >= str_to_file_version('1.5.0.0')=}')
            print(f'{(mod_info.id, mod_info.version)}')
            print(f'{cpp_version=}')

            version_definitions = []
            if (
                windhawk_version >= str_to_file_version('1.5.0.0')
                # Temporary compatibility rules:
                and (mod_info.id, mod_info.version)
                not in [
                    ('aerexplorer', '1.6.2'),
                    ('classic-taskdlg-fix', '1.1.0'),
                    ('msg-box-font-fix', '1.5.0'),
                ],
            ):
                version_definitions += [
                    '-DWINVER=0x0A00',
                    '-D_WIN32_WINNT=0x0A00',
                    '-D_WIN32_IE=0x0A00',
                    '-DNTDDI_VERSION=0x0A000008',
                ]

            compiler_args = [
                f'-std=c++{cpp_version}',
                '-O2',
                '-shared',
                '-DUNICODE',
                '-D_UNICODE',
                *version_definitions,
                '-D__USE_MINGW_ANSI_STDIO=0',
                '-DWH_MOD',
                f'-DWH_MOD_ID=L"{mod_info.id}"',
                f'-DWH_MOD_VERSION=L"{mod_info.version}"',
                engine_lib_path,
                mod_file,
                '-include',
                'windhawk_api.h',
                '-target',
                compiler_target,
                '-o',
                os.path.join(tmp, 'compiled_mod.dll'),
                *extra_args,
            ]

            if (mod_info.id, mod_info.version) in [
                ('accent-color-sync', '1.31'),
                ('aerexplorer', '1.6.2'),
                ('basic-themer', '1.1.0'),
                ('classic-maximized-windows-fix', '2.1'),
                ('taskbar-vertical', '1.0'),
                ('win7-alttab-loader', '1.0.2'),
                ('ce-disable-process-button-flashing', '1.0.1'),
                ('msg-box-font-fix', '1.5.0'),
                ('sib-plusplus-tweaker', '0.7'),
                ('windows-7-clock-spacing', '1.0.0'),
            ]:
                compiler_args.append('-DWH_ENABLE_DEPRECATED_PARTS')

            if (mod_info.id, mod_info.version) in [
                ('classic-explorer-treeview', '1.1'),
                ('taskbar-button-scroll', '1.0.6'),
                ('taskbar-clock-customization', '1.3.3'),
                ('taskbar-notification-icon-spacing', '1.0.2'),
                ('taskbar-vertical', '1.0'),
                ('taskbar-wheel-cycle', '1.1.3'),
            ]:
                compiler_args.append('-lruntimeobject')

            if (mod_info.id, mod_info.version) in [
                ('taskbar-empty-space-clicks', '1.3'),
            ]:
                compiler_args.append('-DUIATYPES_H')

            print(f'Running compiler, extra args: {extra_args}')
            result = subprocess.call([compiler_path, *compiler_args])

        print(f'Result: {result}')

        if result != 0:
            succeeded = False

    return succeeded


def main():
    parser = argparse.ArgumentParser()

    parser_group = parser.add_mutually_exclusive_group(required=True)
    parser_group.add_argument('-v', '--windhawk-version')
    parser_group.add_argument('-i', '--windhawk-installer', type=Path)

    parser_group = parser.add_mutually_exclusive_group(required=True)
    parser_group.add_argument('-f', '--mod-files', type=Path, nargs='+')
    parser_group.add_argument('-d', '--mods-dir', type=Path)

    args = parser.parse_args()

    windhawk_version = args.windhawk_version
    windhawk_installer = args.windhawk_installer

    mod_files = args.mod_files
    if mod_files is None:
        mod_files = args.mods_dir.glob('*.wh.cpp')

    windhawk_dir = Path('windhawk_portable').resolve()
    if windhawk_installer is not None:
        extract_windhawk(windhawk_installer, windhawk_dir)
    else:
        download_and_extract_windhawk(windhawk_version, windhawk_dir)

    failed = []

    for mod_file in mod_files:
        if not check_mod(mod_file, windhawk_dir):
            failed.append(mod_file)

    shutil.rmtree(windhawk_dir)

    if failed:
        for mod_file in failed:
            print(f'{mod_file} failed')
        sys.exit(1)


if __name__ == '__main__':
    main()
