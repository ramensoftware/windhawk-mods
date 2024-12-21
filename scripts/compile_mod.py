import argparse
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import List, Optional

import win32api

MOD_COMPATIBILITY_WIN7_FLAGS = [
    '-DWINVER=0x0601',
    '-D_WIN32_WINNT=0x0601',
    '-D_WIN32_IE=0x0601',
    '-DNTDDI_VERSION=0x06010000',
]

MOD_COMPATIBILITY = {
    'accent-color-sync': [
        {'versions': ['1.1'], 'compiler_flags': ['-include', 'string']},
    ],
    'aerexplorer': [
        {
            'versions': ['1.5.7', '1.5.8', '1.5.9', '1.6.0', '1.6.1', '1.6.2'],
            'compiler_flags': [
                *MOD_COMPATIBILITY_WIN7_FLAGS,
                '-include',
                'vector',
            ],
            'patches': [
                (
                    r'const CMWF_SYMBOL_HOOK (\w+\[\])',
                    r'WindhawkUtils::SYMBOL_HOOK \g<1>',
                ),
                (r'&\w+_addr$', ''),
                (r'if \(!CmwfHookSymbols\(', r'if (!HookSymbols('),
                (r'^bool CmwfHookSymbols\($[\s\S]*?^\}$', r'// Removed.'),
            ],
        },
    ],
    'basic-themer': [
        {'versions': ['1.0.0', '1.1.0'], 'compiler_flags': ['-include', 'vector']},
    ],
    'ce-disable-process-button-flashing': [
        {'versions': ['1.0.0', '1.0.1'], 'compiler_flags': ['-include', 'vector']},
    ],
    'chrome-ui-tweaks': [
        {
            'versions': ['1.0.0'],
            'compiler_flags': ['-include', 'atomic', '-include', 'optional'],
        },
    ],
    'classic-explorer-treeview': [
        {
            'versions': ['1.0', '1.0.1', '1.0.2', '1.0.3', '1.1'],
            'compiler_flags': ['-lruntimeobject'],
        },
    ],
    'classic-maximized-windows-fix': [
        {
            'versions': ['2.0', '2.1'],
            'patches': [
                (r'CMWF_SYMBOL_HOOK (\w+\[\])', r'WindhawkUtils::SYMBOL_HOOK \g<1>'),
                (r'\.(symbols|pOriginalFunction|hookFunction) =', ''),
                (r'.pSharedMemoryCache =.*', r'// \g<0>'),
                (r'^(\s*)CmwfHookSymbols\(', r'\g<1>HookSymbols('),
                (r'^bool CmwfHookSymbols\($[\s\S]*?^\}$', r'// Removed.'),
            ],
        },
    ],
    'classic-taskdlg-fix': [
        {
            'versions': ['1.1.0'],
            'compiler_flags': MOD_COMPATIBILITY_WIN7_FLAGS,
        },
    ],
    'dwm-ghost-mods': [
        {
            'versions': ['1.1'],
            'patches': [
                (
                    re.escape('Wh_FindFirstSymbol(module, server, &symbol)'),
                    'Wh_FindFirstSymbol(module, nullptr, &symbol)',
                ),
            ],
        },
    ],
    'msg-box-font-fix': [
        {
            'versions': ['1.0'],
            'patches': [
                (
                    re.escape('Wh_FindFirstSymbol(module, server, &symbol)'),
                    'Wh_FindFirstSymbol(module, nullptr, &symbol)',
                ),
            ],
        },
        {
            'versions': ['1.4.6', '1.5.0'],
            'compiler_flags': [*MOD_COMPATIBILITY_WIN7_FLAGS, '-include', 'vector'],
        },
    ],
    'sib-plusplus-tweaker': [
        {
            'versions': ['0.4', '0.5', '0.6', '0.7'],
            'compiler_flags': ['-include', 'vector'],
        },
    ],
    'taskbar-button-scroll': [
        {
            'versions': ['1.0', '1.0.1'],
            'compiler_flags': ['-lruntimeobject'],
            'patches': [
                (r'^#pragma region uiaclientinterfaces_p$', r'#if 0  // \g<0>'),
                (r'^#pragma endregion  // uiaclientinterfaces_p$', r'#endif  // \g<0>'),
            ],
        },
        {
            'versions': ['1.0.2', '1.0.3', '1.0.4', '1.0.5', '1.0.6'],
            'compiler_flags': ['-lruntimeobject'],
        },
    ],
    'taskbar-clock-customization': [
        {
            'versions': [
                '1.1',
                '1.1.1',
                '1.2',
                '1.2.1',
                '1.3',
                '1.3.1',
                '1.3.2',
                '1.3.3',
            ],
            'compiler_flags': ['-lruntimeobject'],
        },
    ],
    'taskbar-empty-space-clicks': [
        {'versions': ['1.0', '1.1', '1.2', '1.3'], 'compiler_flags': ['-DUIATYPES_H']},
    ],
    'taskbar-icon-size': [
        {'versions': ['1.2', '1.2.1', '1.2.2'], 'compiler_flags': ['-lruntimeobject']},
        {
            'versions': ['1.2.3', '1.2.4', '1.2.5', '1.2.6', '1.2.7', '1.2.8'],
            'compiler_flags': ['-lruntimeobject', '-include', 'functional'],
        },
    ],
    'taskbar-notification-icon-spacing': [
        {'versions': ['1.0', '1.0.1', '1.0.2'], 'compiler_flags': ['-lruntimeobject']},
    ],
    'taskbar-vertical': [
        {
            'versions': ['1.0'],
            'compiler_flags': [
                '-lruntimeobject',
                '-include',
                'functional',
            ],
            'patches': [
                (
                    re.escape(
                        'HookSymbols(module, symbolHooks, symbolHooksCount, &options)'
                    ),
                    'HookSymbols(module, symbolHooks, symbolHooksCount)',
                ),
                (
                    re.escape('return HookSymbolsWithOnlineCacheFallback('),
                    'return HookSymbols(',
                ),
            ],
        },
    ],
    'taskbar-wheel-cycle': [
        {
            'versions': ['1.0', '1.1', '1.1.1', '1.1.2', '1.1.3'],
            'compiler_flags': ['-lruntimeobject'],
        },
    ],
    'win7-alttab-loader': [
        {
            'versions': ['1.0', '1.0.1', '1.0.2'],
            'patches': [
                (
                    re.escape('Wh_FindFirstSymbol(module, server, &symbol)'),
                    'Wh_FindFirstSymbol(module, nullptr, &symbol)',
                ),
            ],
        },
    ],
    'windows-7-clock-spacing': [
        {'versions': ['1.0.0'], 'compiler_flags': ['-include', 'vector']},
    ],
}


class Architecture(Enum):
    x86 = 1
    x86_64 = 2


@dataclass
class ModInfo:
    id: str
    version: str
    compiler_options: Optional[str]
    architectures: set[Architecture]


def get_engine_path(windhawk_dir: Path) -> Optional[str]:
    config = (windhawk_dir / 'windhawk.ini').read_text(encoding='utf-16')
    p = r'^\s*EnginePath\s*=\s*(.*?)\s*$'
    match = re.search(p, config, flags=re.MULTILINE)
    return match.group(1) if match else None


def get_file_version(path: Path) -> tuple[int, int, int, int]:
    info = win32api.GetFileVersionInfo(str(path), '\\')
    ms = info['FileVersionMS']
    ls = info['FileVersionLS']
    return (
        win32api.HIWORD(ms),
        win32api.LOWORD(ms),
        win32api.HIWORD(ls),
        win32api.LOWORD(ls),
    )


def str_to_file_version(version: str) -> tuple[int, int, int, int]:
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


def compile_mod(
    mod_file: Path, windhawk_dir: Path, output_paths: dict[Architecture, Path]
):
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

    mod_compatibility = MOD_COMPATIBILITY.get(mod_info.id, [])
    mod_compatibility = [
        x for x in mod_compatibility if mod_info.version in x['versions']
    ]
    if len(mod_compatibility) > 1:
        raise RuntimeError(
            f'Multiple mod compatibility entries found for {mod_info.id}'
        )
    mod_compatibility = mod_compatibility[0] if len(mod_compatibility) == 1 else {}

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

        cpp_version = 23
        if windhawk_version < str_to_file_version('1.5.0.0'):
            cpp_version = 20

        version_definitions = [
            '-DWINVER=0x0A00',
            '-D_WIN32_WINNT=0x0A00',
            '-D_WIN32_IE=0x0A00',
            '-DNTDDI_VERSION=0x0A000008',
        ]
        if windhawk_version < str_to_file_version('1.5.0.0'):
            version_definitions = []

        mod_file_for_compilation = mod_file
        mod_file_temp = None

        compatibility_compiler_flags = mod_compatibility.get('compiler_flags', [])
        if compatibility_compiler_flags:
            print(f'Using compatibility compiler flags: {compatibility_compiler_flags}')

        compatibility_patches = mod_compatibility.get('patches')
        if compatibility_patches:
            print(f'Using compatibility patches: {compatibility_patches}')
            mod_code = mod_file.read_text(encoding='utf-8')
            for search, replace in compatibility_patches:
                mod_code = re.sub(search, replace, mod_code, flags=re.MULTILINE)

            with tempfile.NamedTemporaryFile(delete=False, suffix='.wh.cpp') as tmp:
                mod_file_temp = Path(tmp.name)

            mod_file_temp.write_text(mod_code, encoding='utf-8')
            mod_file_for_compilation = mod_file_temp

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
            mod_file_for_compilation,
            '-include',
            'windhawk_api.h',
            '-target',
            compiler_target,
			'-Wl,--export-all-symbols',
            '-o',
            output_paths[arch],
            *extra_args,
            *compatibility_compiler_flags,
        ]

        print(f'Running compiler, extra args: {extra_args}')
        result = subprocess.call([compiler_path, *compiler_args])

        if mod_file_temp:
            mod_file_temp.unlink()

        if result != 0:
            print(f'Failed to compile {mod_file}')
            succeeded = False

    return succeeded


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-w', '--windhawk-dir', type=Path, required=True)

    parser_group = parser.add_mutually_exclusive_group(required=True)
    parser_group.add_argument('-f', '--mod-files', type=Path, nargs='+')
    parser_group.add_argument('-d', '--mods-dir', type=Path)

    parser.add_argument('-o32', '--output-32', type=Path, required=True)
    parser.add_argument('-o64', '--output-64', type=Path, required=True)

    args = parser.parse_args()

    windhawk_dir: Path = args.windhawk_dir

    mod_files: List[Path] = args.mod_files
    if mod_files is None:
        mod_files = sorted(args.mods_dir.rglob('*.wh.cpp'))

    output_paths: dict[Architecture, Path] = {
        Architecture.x86: args.output_32,
        Architecture.x86_64: args.output_64,
    }

    failed = []

    for mod_file in mod_files:
        if not compile_mod(mod_file, windhawk_dir, output_paths):
            failed.append(mod_file)

    if failed:
        print('=' * 80)
        for mod_file in failed:
            print(f'{mod_file} failed')
        sys.exit(1)


if __name__ == '__main__':
    main()
