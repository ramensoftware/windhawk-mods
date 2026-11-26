[CmdletBinding()]
param(
    [string]$WindhawkSource
)

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot
if ($WindhawkSource) { $WindhawkSource = (Resolve-Path -LiteralPath $WindhawkSource).Path }
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'Visual Studio Build Tools with the x64 C++ workload is required.' }
$devShell = Join-Path $vs 'Common7/Tools/Launch-VsDevShell.ps1'
& $devShell -Arch amd64 -HostArch amd64 -SkipAutomaticLocation

$output = Join-Path $root 'out'
New-Item -ItemType Directory -Force -Path $output | Out-Null
Push-Location $output
try {
    & cl.exe /nologo /std:c++20 /EHsc /W4 /WX /DUNICODE /D_UNICODE /DNOMINMAX `
        /Fe:audio-probe.exe (Join-Path $root 'research/audio-probe.cpp') `
        ole32.lib runtimeobject.lib mmdevapi.lib uuid.lib dbghelp.lib
    if ($LASTEXITCODE) { throw "cl.exe exited with $LASTEXITCODE" }

    if ($WindhawkSource) {
        $engine = Join-Path $WindhawkSource 'src/windhawk/engine'
        $sdk = $env:WindowsSDKVersion.TrimEnd('\', '/')
        $winmd = Join-Path $env:WindowsSdkDir "UnionMetadata/$sdk/Windows.winmd"
        $projectionTool = Join-Path $env:WindowsSdkDir "bin/$sdk/x64/cppwinrt.exe"
        $projections = Join-Path $output "cppwinrt-$sdk"
        if (!(Test-Path (Join-Path $projections 'winrt/Windows.UI.Xaml.Controls.h'))) {
            & $projectionTool -input $winmd -output $projections
            if ($LASTEXITCODE) { throw "cppwinrt.exe exited with $LASTEXITCODE" }
        }
        & cl.exe /nologo /c /std:c++20 /EHsc /W4 /WX /bigobj /DWH_MOD /DWIN32_LEAN_AND_MEAN /DNOMINMAX `
            "/I$projections" "/FI$engine/mods_api.h" /Fomixer-inspect.obj `
            (Join-Path $root 'research/native-audio-router-inspect.wh.cpp')
        if ($LASTEXITCODE) { throw "Windhawk diagnostic source failed to compile ($LASTEXITCODE)" }
    }
} finally {
    Pop-Location
}
