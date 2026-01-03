import argparse
import ctypes
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import List, Optional

MOD_COMPATIBILITY_WIN7_FLAGS = [
    '-DWINVER=0x0601',
    '-D_WIN32_WINNT=0x0601',
    '-D_WIN32_IE=0x0601',
    '-DNTDDI_VERSION=0x06010000',
]

HOOK_SYMBOLS_ARM64_PATCH = (
    re.escape(
        R'''#if defined(_M_IX86)
        L"symbol-x86-cache-";
#elif defined(_M_X64)
        L"symbol-cache-";
#else
#error "Unsupported architecture"
#endif
'''
    ),
    R'''#if defined(_M_IX86)
        L"symbol-x86-cache-";
#elif defined(_M_X64)
        L"symbol-cache-";
#elif defined(_M_ARM64)
        L"symbol-arm64-cache-";
#else
#error "Unsupported architecture"
#endif
''',
)

MOD_COMPATIBILITY = {
    'accent-color-sync': [
        {'versions': ['1.1'], 'compiler_flags': ['-include', 'string']},
        {'versions': ['1.3', '1.31'], 'compiler_flags': ['-include', 'cmath']},
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
    'alt-tab-delayer': [
        {'versions': ['1.0.0', '1.1.0'], 'compiler_flags': ['-include', 'atomic']},
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
        {'versions': ['1.1.3'], 'compiler_flags': ['-include', 'cmath']},
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
        {'versions': ['1.1.0'], 'compiler_flags': MOD_COMPATIBILITY_WIN7_FLAGS},
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
    'pinned-items-double-click': [
        {'versions': ['1.0.1'], 'patches': [HOOK_SYMBOLS_ARM64_PATCH]},
    ],
    'sib-plusplus-tweaker': [
        {
            'versions': ['0.4', '0.5', '0.6', '0.7'],
            'compiler_flags': ['-include', 'vector'],
        },
        {'versions': ['0.7.1'], 'compiler_flags': ['-include', 'atomic']},
    ],
    'sysdm-general-tab': [
        {'versions': ['1.0', '1.1'], 'compiler_flags': ['-include', 'cmath']},
    ],
    'taskbar-button-click': [
        {'versions': ['1.0.6', '1.0.7'], 'patches': [HOOK_SYMBOLS_ARM64_PATCH]},
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
            'versions': ['1.0.2', '1.0.3', '1.0.4', '1.0.5'],
            'compiler_flags': ['-lruntimeobject'],
        },
        {
            'versions': ['1.0.6'],
            'compiler_flags': ['-lruntimeobject'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
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
            ],
            'compiler_flags': ['-lruntimeobject'],
        },
        {
            'versions': ['1.3.3'],
            'compiler_flags': ['-lruntimeobject'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
        {'versions': ['1.4'], 'patches': [HOOK_SYMBOLS_ARM64_PATCH]},
    ],
    'taskbar-empty-space-clicks': [
        {'versions': ['1.0', '1.1', '1.2', '1.3'], 'compiler_flags': ['-DUIATYPES_H']},
    ],
    'taskbar-grouping': [
        {
            'versions': ['1.3.2', '1.3.3', '1.3.4', '1.3.5', '1.3.6', '1.3.7'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
    ],
    'taskbar-icon-size': [
        {'versions': ['1.2', '1.2.1', '1.2.2'], 'compiler_flags': ['-lruntimeobject']},
        {
            'versions': ['1.2.3', '1.2.4', '1.2.5'],
            'compiler_flags': ['-lruntimeobject', '-include', 'functional'],
        },
        {
            'versions': [
                '1.2.6',
                '1.2.7',
                '1.2.8',
            ],
            'compiler_flags': ['-lruntimeobject', '-include', 'functional'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
        {
            'versions': [
                '1.2.9',
                '1.2.10',
                '1.2.11',
                '1.2.12',
                '1.2.13',
                '1.2.14',
                '1.2.15',
                '1.2.16',
            ],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
    ],
    'taskbar-labels': [
        {
            'versions': ['1.2.5', '1.3', '1.3.1', '1.3.2', '1.3.3', '1.3.4', '1.3.5'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
    ],
    'taskbar-notification-icon-spacing': [
        {'versions': ['1.0', '1.0.1'], 'compiler_flags': ['-lruntimeobject']},
        {
            'versions': ['1.0.2'],
            'compiler_flags': ['-lruntimeobject'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
    ],
    'taskbar-thumbnail-reorder': [
        {'versions': ['1.0.7', '1.0.8'], 'patches': [HOOK_SYMBOLS_ARM64_PATCH]},
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
                HOOK_SYMBOLS_ARM64_PATCH,
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
            'versions': ['1.0', '1.1', '1.1.1', '1.1.2'],
            'compiler_flags': ['-lruntimeobject'],
        },
        {
            'versions': ['1.1.3'],
            'compiler_flags': ['-lruntimeobject'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
        },
        {'versions': ['1.1.4', '1.1.5'], 'patches': [HOOK_SYMBOLS_ARM64_PATCH]},
    ],
    'virtual-desktop-taskbar-order': [
        {
            'versions': ['1.0.2', '1.0.3', '1.0.4'],
            'patches': [HOOK_SYMBOLS_ARM64_PATCH],
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
    'windows-11-taskbar-styler': [
        {'versions': ['1.3.2'], 'patches': [HOOK_SYMBOLS_ARM64_PATCH]},
    ],
    'windows-7-clock-spacing': [
        {'versions': ['1.0.0'], 'compiler_flags': ['-include', 'vector']},
    ],
}


class Architecture(Enum):
    x86 = 1
    x86_64 = 2
    ARM64 = 3


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


# returns the requested version information from the given file
#
# if language, codepage are None, the first translation in the translation table
# is used instead, as well as common fallback translations
#
# Reference: https://stackoverflow.com/a/56266129
def get_file_version_info(pathname: Path, prop_names: List[str],
                          language: int | None = None, codepage: int | None = None):
    # VerQueryValue() returns an array of that for VarFileInfo\Translation
    #
    class LANGANDCODEPAGE(ctypes.Structure):
        _fields_ = [
            ("wLanguage", ctypes.c_uint16),
            ("wCodePage", ctypes.c_uint16)]

    # avoid some path length limitations by using a resolved path
    wstr_file = ctypes.wstring_at(str(pathname.resolve(strict=True)))

    # getting the size in bytes of the file version info buffer
    size = ctypes.windll.version.GetFileVersionInfoSizeExW(2, wstr_file, None)
    if size == 0:
        e = ctypes.WinError()
        if e.winerror == 1813:
            # ERROR_RESOURCE_TYPE_NOT_FOUND
            return {}
        raise e

    buffer = ctypes.create_string_buffer(size)

    # getting the file version info data
    if ctypes.windll.version.GetFileVersionInfoExW(2, wstr_file, None, size, buffer) == 0:
        raise ctypes.WinError()

    # VerQueryValue() wants a pointer to a void* and DWORD; used both for
    # getting the default translation (if necessary) and getting the actual data
    # below
    value = ctypes.c_void_p(0)
    value_size = ctypes.c_uint(0)

    translations = []

    if language is None and codepage is None:
        # file version information can contain much more than the version
        # number (copyright, application name, etc.) and these are all
        # translatable
        #
        # the following arbitrarily gets the first language and codepage from
        # the list
        ret = ctypes.windll.version.VerQueryValueW(
            buffer, ctypes.wstring_at(R"\VarFileInfo\Translation"),
            ctypes.byref(value), ctypes.byref(value_size))

        if ret == 0:
            e = ctypes.WinError()
            if e.winerror == 1813:
                # ERROR_RESOURCE_TYPE_NOT_FOUND
                first_language, first_codepage = None, None
            else:
                raise e
        else:
            # value points to a byte inside buffer, value_size is the size in bytes
            # of that particular section

            # casting the void* to a LANGANDCODEPAGE*
            lcp = ctypes.cast(value, ctypes.POINTER(LANGANDCODEPAGE))

            first_language, first_codepage = lcp.contents.wLanguage, lcp.contents.wCodePage

            translation = first_language, first_codepage
            translations.append(translation)

        # use fallback values the same way sigcheck does
        translation = first_language, 1252
        if first_language and translation not in translations:
            translations.append(translation)

        translation = 1033, 1252
        if translation not in translations:
            translations.append(translation)

        translation = 1033, first_codepage
        if first_codepage and translation not in translations:
            translations.append(translation)
    else:
        assert language is not None and codepage is not None
        translation = language, codepage
        translations.append(translation)

    # getting the actual data
    result = {}
    for prop_name in prop_names:
        for language_id, codepage_id in translations:
            # formatting language and codepage to something like "040904b0"
            translation = "{0:04x}{1:04x}".format(language_id, codepage_id)

            res = ctypes.windll.version.VerQueryValueW(
                buffer, ctypes.wstring_at("\\StringFileInfo\\" + translation + "\\" + prop_name),
                ctypes.byref(value), ctypes.byref(value_size))

            if res == 0:
                e = ctypes.WinError()
                if e.winerror == 1813:
                    # ERROR_RESOURCE_TYPE_NOT_FOUND
                    continue
                raise e

            # value points to a string of value_size characters, minus one for the
            # terminating null
            prop = ctypes.wstring_at(value.value, value_size.value - 1)

            # some resource strings contain null characters, but they indicate the
            # end of the string for most tools; removing them
            #
            # example:
            # imjppsgf.fil
            # https://www.virustotal.com/gui/file/42deb76551bc087d791eac266a6570032246ec78f4471e7a8922ceb7eb2e91c3/details
            # FileVersion: '15.0.2271.1000\x001000'
            # FileDescription: '\u5370[...]\u3002\x00System Dictionary File'
            prop = prop.split('\0', 1)[0]

            result[prop_name] = prop
            break

    return result


def get_file_version(path: Path) -> tuple[int, int, int, int]:
    info = get_file_version_info(path, ['FileVersion'])
    parts = [int(x) for x in info['FileVersion'].split('.')]
    if len(parts) < 1 or len(parts) > 4:
        raise RuntimeError(f'Invalid file version: {info["FileVersion"]}')
    while len(parts) < 4:
        parts.append(0)
    return (parts[0], parts[1], parts[2], parts[3])


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
                elif value == 'amd64':
                    architectures.add(Architecture.x86_64)
                elif value == 'arm64':
                    architectures.add(Architecture.ARM64)
                elif value == 'x86-64':
                    architectures.add(Architecture.x86_64)
                    architectures.add(Architecture.ARM64)
                else:
                    raise RuntimeError(f'Invalid architecture: {value}')

    if id is None:
        raise RuntimeError('@id is not specified')

    if version is None:
        raise RuntimeError('@version is not specified')

    if not architectures:
        architectures = {
            Architecture.x86,
            Architecture.x86_64,
            Architecture.ARM64,
        }

    return ModInfo(id, version, compiler_options, architectures)


def compile_mod(
    mod_file: Path, windhawk_dir: Path, output_paths: dict[Architecture, Path]
):
    print(f'Checking {mod_file}')

    windhawk_version = get_file_version(windhawk_dir / 'windhawk.exe')

    compiler_path = windhawk_dir / 'Compiler' / 'bin'
    if windhawk_version < str_to_file_version('1.6.0.0'):
        compiler_path /= 'g++.exe'
    else:
        compiler_path /= 'clang++.exe'

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
        if arch == Architecture.ARM64 and windhawk_version < str_to_file_version(
            '1.6.0.0'
        ):
            continue

        engine_lib_path = windhawk_dir / engine_relative_path
        if arch == Architecture.x86:
            engine_lib_path /= '32'
        elif arch == Architecture.x86_64:
            engine_lib_path /= '64'
        else:
            assert arch == Architecture.ARM64
            engine_lib_path /= 'arm64'
        engine_lib_path /= 'windhawk.lib'

        if arch == Architecture.x86:
            compiler_target = 'i686-w64-mingw32'
        elif arch == Architecture.x86_64:
            compiler_target = 'x86_64-w64-mingw32'
        else:
            assert arch == Architecture.ARM64
            compiler_target = 'aarch64-w64-mingw32'

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
			'-x',
			'c++',
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

        print(f'Running compiler, target: {compiler_target}, extra args: {extra_args}')
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
    parser.add_argument('-oarm64', '--output-arm64', type=Path, required=True)

    args = parser.parse_args()

    windhawk_dir: Path = args.windhawk_dir

    mod_files: List[Path] = args.mod_files
    if mod_files is None:
        mod_files = sorted(args.mods_dir.rglob('*.wh.cpp'))

    output_paths: dict[Architecture, Path] = {
        Architecture.x86: args.output_32,
        Architecture.x86_64: args.output_64,
        Architecture.ARM64: args.output_arm64,
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
