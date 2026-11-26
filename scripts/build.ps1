[CmdletBinding()]
param([string]$WindhawkRoot)

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot
$source = Join-Path $root 'src/native-audio-router.wh.cpp'
$distribution = Join-Path $root 'dist'
New-Item -ItemType Directory -Force -Path $distribution | Out-Null
$file = Join-Path $distribution 'native-audio-router.wh.cpp'
Copy-Item -LiteralPath $source -Destination $file

if ($WindhawkRoot) {
    $enginePath = (Get-Content (Join-Path $WindhawkRoot 'windhawk.ini') |
        Where-Object { $_ -match '^EnginePath=' } | Select-Object -First 1) -replace '^EnginePath=', ''
    if (!$enginePath) { throw 'Windhawk configuration lacks EnginePath.' }
    $version = (Get-Content $source |
        Where-Object { $_ -match '^// @version ' } | Select-Object -First 1) -replace '^// @version ', ''
    $output = Join-Path $root 'out'
    New-Item -ItemType Directory -Force -Path $output | Out-Null
    $engine = Join-Path $WindhawkRoot $enginePath
    $clang = Join-Path $WindhawkRoot 'Compiler/bin/clang++.exe'
    & $clang '-std=c++23' -O2 -shared -DUNICODE -D_UNICODE -DWH_MOD `
        '-DWH_MOD_ID=L"native-audio-router"' ('-DWH_MOD_VERSION=L"' + $version + '"') `
        (Join-Path $engine '64/windhawk.lib') $file -include windhawk_api.h `
        -target x86_64-w64-mingw32 '-Wl,--export-all-symbols' -lruntimeobject -loleaut32 -lole32 `
        -o (Join-Path $output 'native-audio-router.dll')
    if ($LASTEXITCODE) { throw "Windhawk compiler exited with $LASTEXITCODE" }
}

Write-Output $file
