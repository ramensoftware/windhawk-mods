from pathlib import Path
import subprocess
import ctypes as C

folder = Path(__file__).resolve().parent
compiler = Path('C:/Applications/Windhawk/Compiler')
flags = (compiler / 'compile_flags.txt').read_text().splitlines()
executable = folder / 'glass.check.exe'
subprocess.run(['rtk', 'proxy', str(compiler / 'bin/clang++.exe'), *flags,
                '-O2', '-Wall', '-Wextra', '-Werror', '-static', str(folder / 'glass.check.cpp'),
                    '-lole32', '-loleaut32', '-lruntimeobject', '-ldwmapi',
                    '-lwindowsapp', '-ld3d11', '-ldxgi', '-ld3dcompiler',
                    '-lgdi32', '-luser32', '-lkernel32',
                    '-o', str(executable)], check=True)
manifest = b'''<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<assemblyIdentity name="Windhawk.Glass.Check" version="1.0.0.0" type="win32"/>
<compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1"><application>
<supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
<maxversiontested Id="10.0.26100.0"/>
</application></compatibility></assembly>'''
kernel = C.WinDLL('kernel32', use_last_error=True)
kernel.BeginUpdateResourceW.argtypes = [C.c_wchar_p, C.c_int]
kernel.BeginUpdateResourceW.restype = C.c_void_p
kernel.UpdateResourceW.argtypes = [C.c_void_p, C.c_void_p, C.c_void_p, C.c_ushort, C.c_void_p, C.c_uint]
kernel.EndUpdateResourceW.argtypes = [C.c_void_p, C.c_int]
handle = kernel.BeginUpdateResourceW(str(executable), False)
assert handle, C.get_last_error()
assert kernel.UpdateResourceW(handle, 24, 1, 0, manifest, len(manifest)), C.get_last_error()
assert kernel.EndUpdateResourceW(handle, False), C.get_last_error()
result = subprocess.run([str(executable)], check=True, capture_output=True, text=True)
print(result.stdout)
assert 'PASS: real XAML' in result.stdout, result.stderr
