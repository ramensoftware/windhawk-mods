"""Build/run isolated production-code regressions; never injects Explorer.

Fixture windows contain synthetic colors/XAML only. Capture checks, when selected,
pass owned fixture HWNDs explicitly rather than discovering desktop windows.
Use --source PATH --baseline to exercise the saved v2.83 implementation.
"""
from pathlib import Path
import argparse
import ctypes as C
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--source', type=Path)
parser.add_argument('--baseline', action='store_true')
parser.add_argument('--mode', default='all')
parser.add_argument('--compile-only', action='store_true')
args = parser.parse_args()
folder = Path(__file__).resolve().parent
compiler = Path('C:/Applications/Windhawk/Compiler')
flags = (compiler / 'compile_flags.txt').read_text().splitlines()
if args.source:
    flags.append(f'-DUW_REVIEW_SOURCE="{args.source.resolve().as_posix()}"')
if args.baseline:
    flags.append('-DUW_BASELINE')
output = Path(tempfile.mkdtemp(prefix='uw-fix-check-')) / 'fix-regressions.exe'
subprocess.run([str(compiler / 'bin/clang++.exe'), *flags,
                '-O2', '-Wall', '-Wextra', '-Werror', '-static',
                str(folder / 'fix-regressions.cpp'),
                '-lole32', '-loleaut32', '-lruntimeobject', '-ldwmapi',
                '-lwindowsapp', '-ld3d11', '-ldxgi', '-ld3dcompiler', '-ld2d1', '-ldxguid',
                '-lwinhttp', '-lwindowscodecs', '-lshell32', '-lshcore', '-luuid',
                '-luser32', '-lkernel32', '-lgdi32', '-o', str(output)], check=True)
manifest = b'''<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<assemblyIdentity name="Windhawk.Ultrawide.FixChecks" version="1.0.0.0" type="win32"/>
<compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1"><application>
<supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
<maxversiontested Id="10.0.26100.0"/>
</application></compatibility></assembly>'''
kernel = C.WinDLL('kernel32', use_last_error=True)
kernel.BeginUpdateResourceW.argtypes = [C.c_wchar_p, C.c_int]
kernel.BeginUpdateResourceW.restype = C.c_void_p
kernel.UpdateResourceW.argtypes = [C.c_void_p, C.c_void_p, C.c_void_p,
                                  C.c_ushort, C.c_void_p, C.c_uint]
kernel.EndUpdateResourceW.argtypes = [C.c_void_p, C.c_int]
handle = kernel.BeginUpdateResourceW(str(output), False)
if not handle:
    raise C.WinError(C.get_last_error())
if not kernel.UpdateResourceW(handle, 24, 1, 0, manifest, len(manifest)):
    error = C.get_last_error()
    kernel.EndUpdateResourceW(handle, True)
    raise C.WinError(error)
if not kernel.EndUpdateResourceW(handle, False):
    raise C.WinError(C.get_last_error())
print(f'BUILD PASS: {output}', flush=True)
if not args.compile_only:
    result = subprocess.run([str(output), args.mode], timeout=45)
    raise SystemExit(result.returncode)
