# one-letter-language-indicator-symbols.ps1
#
# Fills the local symbol cache of the locally compiled "1 Letter Language
# Indicator" mod (local@one-letter-language-indicator) from Windhawk's online
# symbol cache, for builds whose PDBs Microsoft hasn't published.
#
# Local mods skip the online cache and download PDBs from Microsoft's symbol
# server, which returns 404 for these builds. Published mods get the offsets
# from the online cache instead. This script copies those offsets into the
# local mod's cache in the registry, where Windhawk looks first.
#
# It reads the cache keys from the DLLs on this machine, so it works for any
# build the online cache covers. Run it from an elevated PowerShell, then
# disable and re-enable the mod.

$ErrorActionPreference = 'Stop'

# Windows PowerShell 5.1 may not enable TLS 1.2 by default.
[Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12

$LocalModName = 'local@one-letter-language-indicator'
$RegistryPath = "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$LocalModName\SymbolCache"
$OnlineCacheBase = 'https://ramensoftware.github.io/windhawk-mod-symbol-cache'

# Published mods whose cache entries contain the symbols below. Windhawk ignores
# cache symbols a mod doesn't use, so entries of another mod with the same
# symbol names work as well. "Taskbar tray icon spacing and grid" uses the same
# five symbols (this mod's code is based on it).
$SourceMods = @(
    'one-letter-language-indicator',
    'taskbar-notification-icon-spacing'
)

$TaskbarDllSymbols = @(
    'const CTaskBand::`vftable''{for `ITaskListWndSite''}',
    'public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const ',
    'public: int __cdecl TaskbarHost::FrameHeight(void)const ',
    'public: void __cdecl std::_Ref_count_base::_Decref(void)'
)

$SystemTraySymbols = @(
    'public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void)'
)

function Test-Admin {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

# Reads what Windhawk uses for the cache key: the PDB GUID and age from the
# CodeView debug record, plus the image size for validating offsets.
function Get-PeInfo([string]$Path) {
    $bytes = [IO.File]::ReadAllBytes($Path)

    $peOffset = [BitConverter]::ToInt32($bytes, 0x3C)
    if ([BitConverter]::ToUInt32($bytes, $peOffset) -ne 0x00004550) {
        throw "Not a PE file: $Path"
    }

    $fileHeader = $peOffset + 4
    $machine = [BitConverter]::ToUInt16($bytes, $fileHeader)
    $sectionCount = [BitConverter]::ToUInt16($bytes, $fileHeader + 2)
    $timeStamp = [BitConverter]::ToUInt32($bytes, $fileHeader + 4)
    $optionalHeaderSize = [BitConverter]::ToUInt16($bytes, $fileHeader + 16)

    $optionalHeader = $fileHeader + 20
    $magic = [BitConverter]::ToUInt16($bytes, $optionalHeader)
    $imageSize = [BitConverter]::ToUInt32($bytes, $optionalHeader + 56)

    if ($magic -eq 0x20B) {
        $dataDirectories = $optionalHeader + 112
    } else {
        $dataDirectories = $optionalHeader + 96
    }

    # IMAGE_DIRECTORY_ENTRY_DEBUG = 6.
    $debugRva = [BitConverter]::ToUInt32($bytes, $dataDirectories + 6 * 8)
    $debugSize = [BitConverter]::ToUInt32($bytes, $dataDirectories + 6 * 8 + 4)
    if ($debugRva -eq 0) {
        throw "No debug directory: $Path"
    }

    # RVA to file offset.
    $sections = $optionalHeader + $optionalHeaderSize
    $debugOffset = $null
    for ($i = 0; $i -lt $sectionCount; $i++) {
        $section = $sections + $i * 40
        $virtualSize = [BitConverter]::ToUInt32($bytes, $section + 8)
        $virtualAddress = [BitConverter]::ToUInt32($bytes, $section + 12)
        $rawSize = [BitConverter]::ToUInt32($bytes, $section + 16)
        $rawPointer = [BitConverter]::ToUInt32($bytes, $section + 20)
        $size = [Math]::Max($virtualSize, $rawSize)
        if ($debugRva -ge $virtualAddress -and $debugRva -lt $virtualAddress + $size) {
            $debugOffset = $debugRva - $virtualAddress + $rawPointer
            break
        }
    }
    if ($null -eq $debugOffset) {
        throw "Debug directory outside the sections: $Path"
    }

    for ($entry = 0; $entry -lt [Math]::Floor($debugSize / 28); $entry++) {
        $entryOffset = $debugOffset + $entry * 28
        $type = [BitConverter]::ToUInt32($bytes, $entryOffset + 12)
        # IMAGE_DEBUG_TYPE_CODEVIEW = 2.
        if ($type -ne 2) {
            continue
        }

        $cv = [BitConverter]::ToUInt32($bytes, $entryOffset + 24)
        if ([Text.Encoding]::ASCII.GetString($bytes, $cv, 4) -ne 'RSDS') {
            continue
        }

        $data1 = [BitConverter]::ToUInt32($bytes, $cv + 4)
        $data2 = [BitConverter]::ToUInt16($bytes, $cv + 8)
        $data3 = [BitConverter]::ToUInt16($bytes, $cv + 10)
        $data4 = ($bytes[($cv + 12)..($cv + 19)] | ForEach-Object { '{0:X2}' -f $_ }) -join ''
        $age = [BitConverter]::ToUInt32($bytes, $cv + 20)

        # Same format as the engine: %08X%04X%04X, the 8 Data4 bytes as %02X,
        # then the age as lowercase hex (%x).
        $pdbIdentifier = ('{0:X8}{1:X4}{2:X4}' -f $data1, $data2, $data3) + $data4 + ('{0:x}' -f $age)

        return [PSCustomObject]@{
            Machine       = $machine
            TimeStamp     = $timeStamp
            ImageSize     = $imageSize
            PdbIdentifier = $pdbIdentifier
        }
    }

    throw "No CodeView (RSDS) record: $Path"
}

# Parses an online cache entry: "1#<comment>#<comment>#symbol#offset#...".
function Get-CacheSymbols([string]$Content) {
    $parts = $Content.TrimEnd("`r", "`n") -split '#'
    if ($parts.Count -lt 3 -or $parts[0] -ne '1') {
        return $null
    }

    $symbols = @{}
    for ($i = 3; $i + 1 -lt $parts.Count; $i += 2) {
        $symbols[$parts[$i]] = $parts[$i + 1]
    }

    return [PSCustomObject]@{
        Header  = "$($parts[0])#$($parts[1])#$($parts[2])"
        Symbols = $symbols
    }
}

function Get-OnlineCache([string]$ModId, [string]$CacheKey) {
    $url = "$OnlineCacheBase/$ModId/$CacheKey.txt"
    try {
        $response = Invoke-WebRequest -Uri $url -UseBasicParsing
        return [Text.Encoding]::UTF8.GetString($response.RawContentStream.ToArray())
    } catch {
        $status = $null
        if ($_.Exception.Response) {
            $status = [int]$_.Exception.Response.StatusCode
        }
        if ($status -eq 404) {
            Write-Host "    $ModId`: no entry" -ForegroundColor DarkGray
        } else {
            Write-Host "    $ModId`: request failed ($($_.Exception.Message))" -ForegroundColor Yellow
        }
        return $null
    }
}

# Builds a cache entry with all the needed symbols, taken from the first
# source mods which have them. Returns $null if any symbol is missing.
function Build-CacheEntry([string]$ModuleName, [string]$Path, [string[]]$NeededSymbols) {
    Write-Host ""
    Write-Host "$ModuleName ($Path)"

    $info = Get-PeInfo $Path
    if ($info.Machine -ne 0x8664) {
        throw ('{0} isn''t an x64 binary (machine 0x{1:X4}), only x64 is supported' -f $ModuleName, $info.Machine)
    }

    $cacheKey = "pdb_$($info.PdbIdentifier)"
    Write-Host "    Cache key: $cacheKey"

    $header = $null
    $found = [ordered]@{}

    foreach ($modId in $SourceMods) {
        $content = Get-OnlineCache $modId $cacheKey
        if (-not $content) {
            continue
        }

        $cache = Get-CacheSymbols $content
        if (-not $cache) {
            Write-Host "    $modId`: unrecognized entry format" -ForegroundColor Yellow
            continue
        }

        $added = 0
        foreach ($symbol in $NeededSymbols) {
            if ($found.Contains($symbol) -or -not $cache.Symbols.ContainsKey($symbol)) {
                continue
            }

            $offset = $cache.Symbols[$symbol]
            if ($offset -eq '') {
                # Listed as absent in this build.
                continue
            }

            [UInt64]$offsetValue = 0
            if (-not [UInt64]::TryParse($offset, [ref]$offsetValue) -or $offsetValue -ge $info.ImageSize) {
                Write-Host "    $modId`: invalid offset for $symbol" -ForegroundColor Yellow
                continue
            }

            $found[$symbol] = $offset
            $added++
        }

        Write-Host "    $modId`: $added symbol(s) taken"
        if (-not $header -and $added -gt 0) {
            $header = $cache.Header
        }

        if ($found.Count -eq $NeededSymbols.Count) {
            break
        }
    }

    $missing = @($NeededSymbols | Where-Object { -not $found.Contains($_) })
    if ($missing.Count -gt 0) {
        Write-Host "    Missing $($missing.Count) of $($NeededSymbols.Count) symbol(s):" -ForegroundColor Red
        foreach ($symbol in $missing) {
            Write-Host "      $symbol" -ForegroundColor Red
        }
        return $null
    }

    $entry = $header
    foreach ($symbol in $found.Keys) {
        $entry += "#$symbol#$($found[$symbol])"
    }

    return [PSCustomObject]@{
        CacheKey = $cacheKey
        Entry    = $entry
    }
}

if (-not (Test-Admin)) {
    Write-Host 'Run this script from an elevated PowerShell (Run as administrator).' -ForegroundColor Red
    exit 1
}

if ($env:PROCESSOR_ARCHITECTURE -ne 'AMD64') {
    Write-Host "Only x64 Windows is supported (this is $env:PROCESSOR_ARCHITECTURE)." -ForegroundColor Red
    exit 1
}

if (-not (Test-Path 'HKLM:\SOFTWARE\Windhawk\Engine')) {
    Write-Host 'HKLM\SOFTWARE\Windhawk\Engine not found. A portable Windhawk keeps the cache in .ini files, which this script doesn''t handle.' -ForegroundColor Red
    exit 1
}

$build = (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion')
Write-Host "Windows build $($build.CurrentBuild).$($build.UBR)"

# The mod uses SystemTray.dll when it exists, otherwise Taskbar.View.dll
# (before version 2604) or ExplorerExtensions.dll.
$clientCore = "$env:windir\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy"
$systemTrayPath = $null
foreach ($name in 'SystemTray.dll', 'Taskbar.View.dll', 'ExplorerExtensions.dll') {
    $candidate = Join-Path $clientCore $name
    if (Test-Path $candidate) {
        if ($name -eq 'Taskbar.View.dll') {
            $major = (Get-Item $candidate).VersionInfo.FileMajorPart
            if ($major -ge 2604) {
                continue
            }
        }
        $systemTrayPath = $candidate
        break
    }
}

$modules = @(
    @{ Name = 'taskbar.dll'; Path = "$env:windir\System32\taskbar.dll"; Symbols = $TaskbarDllSymbols }
)
if ($systemTrayPath) {
    $modules += @{ Name = (Split-Path $systemTrayPath -Leaf); Path = $systemTrayPath; Symbols = $SystemTraySymbols }
} else {
    Write-Host 'No system tray module found in MicrosoftWindows.Client.Core.' -ForegroundColor Yellow
}

$entries = @()
$failed = $false
foreach ($module in $modules) {
    $result = Build-CacheEntry $module.Name $module.Path $module.Symbols
    if ($result) {
        $entries += $result
    } else {
        $failed = $true
    }
}

if ($entries.Count -gt 0) {
    if (-not (Test-Path $RegistryPath)) {
        New-Item -Path $RegistryPath -Force | Out-Null
    }

    Write-Host ""
    foreach ($entry in $entries) {
        New-ItemProperty -Path $RegistryPath -Name $entry.CacheKey -Value $entry.Entry -PropertyType String -Force | Out-Null
        Write-Host "Written to $RegistryPath\$($entry.CacheKey)" -ForegroundColor Green
    }
}

Write-Host ""
if ($failed) {
    Write-Host 'Not all modules could be filled: the online cache has no offsets for this build yet. Try again later.' -ForegroundColor Red
    exit 1
}

Write-Host 'Done. Disable and re-enable the mod in Windhawk.' -ForegroundColor Green
