// ==WindhawkMod==
// @id              quick-file2folder
// @name            Quick File2folder
// @description     Instantly create a folder from selected files with customizable colors, naming styles, organization, and full undo support.
// @version         1.7.0
// @author          Dexter Fernandes
// @github          https://github.com/surya467
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick File2folder

**Quick File2folder** is a productivity enhancement for Windows File Explorer and the Desktop that lets you quickly bundle selected files and folders into a new folder with a single click.

It eliminates repetitive workflows: no more manually creating a new folder, naming it, selecting your files, and dragging or cutting/pasting them in.

---

### Features

- **One-Click Folder Creation**: Right-click any selected files or folders in File Explorer or on the Desktop and click **"Create folder with selected items"**.
- **Full Undo Support**: Made a mistake or want to revert? Right-click anywhere in the folder or Desktop background and select **"Undo Last Quick File2folder Move"** to restore all items back to their original locations and delete the created folder.
- **Flexible Naming Styles**:
  - **Sequential Numbers** (*e.g.,* `New Folder`, `New Folder 2`, `New Folder 3`)
  - **Alphabetical Letters** (*e.g.,* `New Folder A`, `New Folder B`, ..., `New Folder Z`, `New Folder AA`)
  - **Date & Timestamp** (*e.g.,* `New Folder 2026-11-08`)
- **Custom Folder Colors**: Choose a custom folder accent color (**Yellow**, **Blue**, **Green**, **Red**, **Purple**, **Orange**, **Grey**, or standard **Default Windows**). The mod dynamically generates a colorized icon and folder configuration.
- **Smart File Type Categorization**: Optionally sort selected files into categorized subfolders (`Images`, `Videos`, `Audio`, `Documents`, `Archives`, `Design`, `Code`, `Folders`, `Other`).
- **Post-Creation Actions**:
  - **Rename Mode**: Automatically highlights and activates inline rename for the new folder so you can name it immediately.
  - **Open Mode**: Automatically opens the new folder in File Explorer.
- **Desktop & Explorer Support**: Works natively inside Explorer windows and on the Windows Desktop.

---

### How to Use

#### 1. Creating a Folder
1. Select one or more files/folders in File Explorer or on your Desktop.
2. Right-click the selection.
3. Select **"Create folder with selected items"**.
4. A new folder is created in the same directory, and your selected items are moved into it.

#### 2. Undoing the Last Operation
1. Right-click the background of the folder or your Desktop.
2. Click **"Undo Last Quick File2folder Move"**.
3. All files are safely moved back to their original locations, and the generated folder is removed.

---

### Settings

- **Folder name (`FolderPrefix`)**: Base name for newly created folders (default: `New Folder`).
- **Naming style (`NamingStyle`)**:
  - `Numbers`: Sequential numbering (`New Folder`, `New Folder 2`, ...)
  - `Letters`: Alphabetical suffixes (`New Folder A`, `New Folder B`, ...)
  - `Date`: Appends today's date (`New Folder YYYY-MM-DD`)
- **Folder icon color (`FolderColor`)**: Pick from 8 accent colors to easily identify generated folders.
- **Rename after creation (`RenameAfterMove`)**: Automatically puts the newly created folder in inline rename mode.
- **Open folder after creation (`OpenAfterCreation`)**: Automatically opens the new folder in Explorer (overrides rename mode if enabled).
- **Organize by file type (`OrganizeByType`)**: Automatically sorts moved files into subfolders by extension.

---

### Compatibility
- Compatible with Windows 10 and Windows 11 (including tabbed File Explorer).
- Supports 64-bit and 32-bit systems.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FolderPrefix: New Folder
  $name: Folder name
  $description: Base name for new folders.
- NamingStyle: Numbers
  $name: Naming style
  $options:
  - Numbers: Sequential numbers
  - Letters: Alphabetical
  - Date: Date and time
- FolderColor: Yellow
  $name: Folder icon color
  $options:
  - Default: Windows default
  - Yellow: Yellow
  - Blue: Blue
  - Green: Green
  - Red: Red
  - Purple: Purple
  - Orange: Orange
  - Grey: Grey
- RenameAfterMove: true
  $name: Rename after creation
  $description: Select and rename the newly created folder.
- OpenAfterCreation: false
  $name: Open folder after creation
  $description: Open the new folder automatically. Overrides rename mode.
- OrganizeByType: false
  $name: Organize by file type
  $description: Sort files into category subfolders.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <string>

static const wchar_t* CREATE_KEY =
    L"Software\\Classes\\AllFilesystemObjects\\shell\\QuickFile2Folder";

static const wchar_t* UNDO_DIR_KEY =
    L"Software\\Classes\\Directory\\Background\\shell\\QuickFile2FolderUndo";

static const wchar_t* UNDO_DESKTOP_KEY =
    L"Software\\Classes\\DesktopBackground\\shell\\QuickFile2FolderUndo";

static bool WriteUnicodeFile(
    const std::wstring& path,
    const wchar_t* content
) {
    HANDLE file = INVALID_HANDLE_VALUE;
    for (int retry = 0; retry < 5; ++retry) {
        file = CreateFileW(
            path.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        if (file != INVALID_HANDLE_VALUE)
            break;
        Sleep(100);
    }

    if (file == INVALID_HANDLE_VALUE)
        return false;

    BYTE bom[] = { 0xFF, 0xFE };
    DWORD written = 0;

    bool ok = WriteFile(file, bom, 2, &written, nullptr) && (written == 2);

    DWORD bytes = (DWORD)(wcslen(content) * sizeof(wchar_t));
    if (ok && bytes > 0) {
        ok = WriteFile(file, content, bytes, &written, nullptr) && (written == bytes);
    }

    CloseHandle(file);
    return ok;
}

static std::wstring GetSettingString(PCWSTR name, PCWSTR fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = (value && *value) ? value : fallback;
    if (value)
        Wh_FreeStringSetting(value);
    return result;
}

static bool GetSettingBool(PCWSTR name, bool fallback) {
    PCWSTR str = Wh_GetStringSetting(name);
    if (!str || !*str) {
        if (str) Wh_FreeStringSetting(str);
        return fallback;
    }
    Wh_FreeStringSetting(str);
    return Wh_GetIntSetting(name) != 0;
}

static std::wstring QuotePS(PCWSTR value) {
    std::wstring result = L"'";
    for (const wchar_t* p = value; *p; ++p) {
        result += *p;
        if (*p == L'\'')
            result += L'\'';
    }
    result += L"'";
    return result;
}

static void ReplaceAll(
    std::wstring& text,
    const std::wstring& from,
    const std::wstring& to
) {
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::wstring::npos) {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }
}

static bool SetString(
    HKEY key,
    const wchar_t* name,
    const std::wstring& value
) {
    return RegSetValueExW(
        key, name, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(value.c_str()),
        (DWORD)((value.size() + 1) * sizeof(wchar_t))
    ) == ERROR_SUCCESS;
}

static bool RegisterMenu(
    const wchar_t* registryPath,
    const wchar_t* title,
    const std::wstring& launcher,
    const wchar_t* icon = nullptr
) {
    HKEY key = nullptr;
    LONG result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        registryPath,
        0, nullptr, REG_OPTION_NON_VOLATILE,
        KEY_WRITE, nullptr, &key, nullptr
    );

    if (result != ERROR_SUCCESS)
        return false;

    bool ok = true;
    ok &= SetString(key, nullptr, title);

    if (wcscmp(registryPath, CREATE_KEY) == 0) {
        ok &= SetString(key, L"MultiSelectModel", L"Player");
        ok &= SetString(key, L"Position", L"Top");
        ok &= SetString(key, L"Icon", L"%SystemRoot%\\System32\\imageres.dll,-3");
    } else if (icon) {
        ok &= SetString(key, L"Icon", icon);
    }

    HKEY commandKey = nullptr;
    result = RegCreateKeyExW(
        key, L"command", 0, nullptr,
        REG_OPTION_NON_VOLATILE, KEY_WRITE,
        nullptr, &commandKey, nullptr
    );

    if (result == ERROR_SUCCESS) {
        std::wstring command = L"\"wscript.exe\" \"" + launcher + L"\"";
        ok &= SetString(commandKey, nullptr, command);
        RegCloseKey(commandKey);
    } else {
        ok = false;
    }

    RegCloseKey(key);
    return ok;
}

static bool WriteHelpers() {
    wchar_t storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage)))
        return false;

    if (!CreateDirectoryW(storage, nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS)
        return false;

    std::wstring root = storage;
    std::wstring psPath = root + L"\\QuickFile2Folder.ps1";
    std::wstring createVbs = root + L"\\Create.vbs";
    std::wstring undoVbs = root + L"\\Undo.vbs";

    std::wstring prefix = GetSettingString(L"FolderPrefix", L"New Folder");
    std::wstring style = GetSettingString(L"NamingStyle", L"Numbers");
    std::wstring color = GetSettingString(L"FolderColor", L"Yellow");

    bool rename = GetSettingBool(L"RenameAfterMove", true);
    bool open = GetSettingBool(L"OpenAfterCreation", false);
    bool organize = GetSettingBool(L"OrganizeByType", false);

    const wchar_t* script = LR"PS(
param(
    [ValidateSet('Create','Undo')]
    [string]$Mode = 'Create'
)

$ErrorActionPreference = 'Stop'

$stateDir = Join-Path $env:LOCALAPPDATA 'QuickFile2folder'
$manifest = Join-Path $stateDir 'last-operation.json'
$debugFile = Join-Path $env:TEMP "QuickFile2folder-$PID.log"

if (-not (Test-Path -LiteralPath $stateDir)) {
    New-Item -ItemType Directory -Path $stateDir -Force | Out-Null
}

$mutex = [System.Threading.Mutex]::new(
    $false,
    'Local\QuickFile2Folder_Run'
)
$ownsMutex = $false

try {
    $ownsMutex = $mutex.WaitOne(0)
    if (-not $ownsMutex) {
        exit 0
    }

    if ($Mode -eq 'Create') {
        $cooldown = Join-Path $stateDir 'last-run.txt'
        if (Test-Path -LiteralPath $cooldown) {
            try {
                $last = [datetime]::Parse((Get-Content -LiteralPath $cooldown -Raw))
                if (((Get-Date) - $last).TotalSeconds -lt 2) {
                    exit 0
                }
            } catch {}
        }
        (Get-Date).ToString('o') | Set-Content -LiteralPath $cooldown
    }

    # ---------------- UNDO MODE ----------------
    if ($Mode -eq 'Undo') {
        if (-not (Test-Path -LiteralPath $manifest)) {
            Add-Type -AssemblyName System.Windows.Forms
            [System.Windows.Forms.MessageBox]::Show(
                'There is no Quick File2folder operation to undo.',
                'Quick File2folder',
                [System.Windows.Forms.MessageBoxButtons]::OK,
                [System.Windows.Forms.MessageBoxIcon]::Information
            ) | Out-Null
            exit 0
        }

        $operation = Get-Content -LiteralPath $manifest -Raw -Encoding UTF8 | ConvertFrom-Json
        $records = @($operation.Items)
        $target = [string]$operation.Target
        $parent = [string]$operation.Parent
        $errors = @()

        foreach ($record in $records) {
            try {
                $source = [string]$record.Destination
                $original = [string]$record.Original

                if (-not (Test-Path -LiteralPath $source)) {
                    continue
                }

                if (Test-Path -LiteralPath $original) {
                    throw "Original location already exists: $original"
                }

                Move-Item -LiteralPath $source -Destination $original -ErrorAction Stop
            } catch {
                $errors += $_.Exception.Message
            }
        }

        if ($errors.Count -gt 0) {
            throw ("Undo was incomplete: " + ($errors -join "`n"))
        }

        if ($target -and (Test-Path -LiteralPath $target)) {
            Get-ChildItem -LiteralPath $target -Recurse -Force -ErrorAction SilentlyContinue | ForEach-Object {
                try { [System.IO.File]::SetAttributes($_.FullName, [System.IO.FileAttributes]::Normal) } catch {}
            }
            try { [System.IO.File]::SetAttributes($target, [System.IO.FileAttributes]::Normal) } catch {}

            foreach ($meta in @('desktop.ini', 'folder.ico')) {
                $metaPath = Join-Path $target $meta
                if (Test-Path -LiteralPath $metaPath) {
                    Remove-Item -LiteralPath $metaPath -Force -ErrorAction SilentlyContinue
                }
            }

            Get-ChildItem -LiteralPath $target -Directory -Recurse -Force -ErrorAction SilentlyContinue |
                Sort-Object FullName -Descending |
                ForEach-Object {
                    if (@(Get-ChildItem -LiteralPath $_.FullName -Force -ErrorAction SilentlyContinue).Count -eq 0) {
                        Remove-Item -LiteralPath $_.FullName -Force -ErrorAction SilentlyContinue
                    }
                }

            $remaining = @(Get-ChildItem -LiteralPath $target -Force -ErrorAction SilentlyContinue)
            if ($remaining.Count -eq 0) {
                Remove-Item -LiteralPath $target -Force -Recurse -ErrorAction SilentlyContinue
            }
        }

        Remove-Item -LiteralPath $manifest -Force -ErrorAction SilentlyContinue

        # Refresh Explorer & Desktop
        $shell = New-Object -ComObject Shell.Application
        $allRefreshViews = [System.Collections.Generic.List[object]]::new()
        foreach ($w in $shell.Windows()) {
            try { $allRefreshViews.Add($w) } catch {}
        }
        try {
            $hwndDesktop = 0
            $desktop = $shell.Windows().FindWindowSW([ref]$null, [ref]$null, 8, [ref]$hwndDesktop, 1)
            if ($desktop) { $allRefreshViews.Add($desktop) }
        } catch {}

        foreach ($window in $allRefreshViews) {
            try {
                $view = [string]$window.Document.Folder.Self.Path
                if ($view.TrimEnd('\') -ieq $parent.TrimEnd('\')) {
                    $window.Refresh()
                }
            } catch {}
        }

        Add-Type -AssemblyName System.Windows.Forms
        [System.Windows.Forms.MessageBox]::Show(
            'The last Quick File2folder operation has been undone.',
            'Quick File2folder',
            [System.Windows.Forms.MessageBoxButtons]::OK,
            [System.Windows.Forms.MessageBoxIcon]::Information
        ) | Out-Null

        exit 0
    }

    # ---------------- CREATE MODE ----------------
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

public static class QFNative {
    [DllImport("user32.dll")]
    public static extern IntPtr GetForegroundWindow();

    [DllImport("user32.dll")]
    public static extern IntPtr GetAncestor(IntPtr hWnd, uint gaFlags);
}
'@

    $fg = [QFNative]::GetForegroundWindow()
    $foreground = $fg.ToInt64()
    $rootFg = [QFNative]::GetAncestor($fg, 2).ToInt64()

    $shell = New-Object -ComObject Shell.Application
    $allViews = [System.Collections.Generic.List[object]]::new()

    foreach ($w in $shell.Windows()) {
        try {
            $exe = [System.IO.Path]::GetFileName([string]$w.FullName)
            if ($exe -ieq 'explorer.exe' -and $w.Document -and $w.Document.Folder) {
                $allViews.Add($w)
            }
        } catch {}
    }

    try {
        $hwndDesktop = 0
        $desktop = $shell.Windows().FindWindowSW([ref]$null, [ref]$null, 8, [ref]$hwndDesktop, 1)
        if ($desktop -and $desktop.Document -and $desktop.Document.Folder) {
            $allViews.Add($desktop)
        }
    } catch {}

    $selected = @()
    $candidateViews = [System.Collections.Generic.List[object]]::new()

    foreach ($view in $allViews) {
        try {
            $items = $view.Document.SelectedItems()
            if ($items -and $items.Count -gt 0) {
                $paths = @(
                    for ($i = 0; $i -lt $items.Count; $i++) {
                        $item = $items.Item($i)
                        if ($item.Path -and (Test-Path -LiteralPath $item.Path)) {
                            [string]$item.Path
                        }
                    }
                )
                if ($paths.Count -gt 0) {
                    $candidateViews.Add([PSCustomObject]@{
                        View = $view
                        HWND = [int64]$view.HWND
                        Paths = $paths
                    })
                }
            }
        } catch {}
    }

    if ($candidateViews.Count -eq 1) {
        $selected = @($candidateViews[0].Paths)
    }
    elseif ($candidateViews.Count -gt 1) {
        $matched = $candidateViews | Where-Object {
            $_.HWND -eq $foreground -or $_.HWND -eq $rootFg
        } | Select-Object -First 1

        if ($matched) {
            $selected = @($matched.Paths)
        } else {
            $selected = @($candidateViews[0].Paths)
        }
    }

    $selected = @($selected | Select-Object -Unique)

    if ($selected.Count -eq 0) {
        throw 'No selected files or folders found.'
    }

    $paths = @(
        $selected | ForEach-Object {
            [System.IO.Path]::GetFullPath($_)
        }
    )

    $parents = @(
        $paths | ForEach-Object {
            [System.IO.Path]::GetDirectoryName($_.TrimEnd([char]92))
        } | Select-Object -Unique
    )

    if ($parents.Count -ne 1 -or !$parents[0]) {
        throw 'Please select items from the same folder.'
    }

    $parent = $parents[0]

    $prefix = __PREFIX__
    $prefix = $prefix -replace '[<>:"/\\|?*\x00-\x1F]', '_'
    $prefix = $prefix.Trim().TrimEnd('.')

    if (!$prefix) {
        $prefix = 'New Folder'
    }

    $style = __STYLE__

    if ($style -eq 'Letters') {
        $alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
        $n = 0
        while ($true) {
            $x = $n
            $suffix = ''
            do {
                $suffix = $alphabet[$x % 26] + $suffix
                $x = [math]::Floor($x / 26) - 1
            } while ($x -ge 0)

            $folderName = "$prefix $suffix"
            $target = Join-Path $parent $folderName

            if (-not (Test-Path -LiteralPath $target)) {
                break
            }
            $n++
        }
    }
    elseif ($style -eq 'Date') {
        $stamp = Get-Date -Format 'yyyy-MM-dd'
        $folderName = "$prefix $stamp"
        $target = Join-Path $parent $folderName
        $n = 2

        while (Test-Path -LiteralPath $target) {
            $folderName = "$prefix $stamp $n"
            $target = Join-Path $parent $folderName
            $n++
        }
    }
    else {
        # Numbers: First folder is "$prefix", then "$prefix 2", "$prefix 3", etc.
        $folderName = $prefix
        $target = Join-Path $parent $folderName
        $n = 2

        while (Test-Path -LiteralPath $target) {
            $folderName = "$prefix $n"
            $target = Join-Path $parent $folderName
            $n++
        }
    }

    New-Item -ItemType Directory -Path $target | Out-Null

    $records = [System.Collections.Generic.List[object]]::new()

    $image = @('.jpg','.jpeg','.png','.gif','.bmp','.webp','.svg','.tif','.tiff','.heic','.raw','.psd','.ai')
    $video = @('.mp4','.mov','.avi','.mkv','.wmv','.webm','.m4v','.flv')
    $audio = @('.mp3','.wav','.flac','.aac','.ogg','.m4a','.wma')
    $documents = @('.pdf','.doc','.docx','.txt','.rtf','.odt','.xls','.xlsx','.csv','.ppt','.pptx')
    $archives = @('.zip','.rar','.7z','.tar','.gz','.bz2')
    $design = @('.fig','.sketch','.xd','.indd','.eps','.blend','.obj','.fbx','.stl','.3mf')
    $code = @('.html','.htm','.css','.js','.ts','.jsx','.tsx','.py','.cpp','.c','.h','.json','.xml','.sql','.php','.ps1','.bat','.cmd')

    $organize = __ORGANIZE__

    foreach ($path in $paths) {
        if (-not (Test-Path -LiteralPath $path)) {
            throw "Item no longer exists: $path"
        }

        $extension = [System.IO.Path]::GetExtension($path).ToLowerInvariant()
        $isContainer = (Test-Path -LiteralPath $path -PathType Container)

        if ($organize) {
            if ($isContainer) {
                $category = 'Folders'
            }
            elseif ($image -contains $extension) {
                $category = 'Images'
            }
            elseif ($video -contains $extension) {
                $category = 'Videos'
            }
            elseif ($audio -contains $extension) {
                $category = 'Audio'
            }
            elseif ($documents -contains $extension) {
                $category = 'Documents'
            }
            elseif ($archives -contains $extension) {
                $category = 'Archives'
            }
            elseif ($design -contains $extension) {
                $category = 'Design'
            }
            elseif ($code -contains $extension) {
                $category = 'Code'
            }
            else {
                $category = 'Other'
            }

            $destinationFolder = Join-Path $target $category
            if (-not (Test-Path -LiteralPath $destinationFolder)) {
                New-Item -ItemType Directory -Path $destinationFolder | Out-Null
            }
        }
        else {
            $destinationFolder = $target
        }

        $destination = Join-Path $destinationFolder ([System.IO.Path]::GetFileName($path))

        if (Test-Path -LiteralPath $destination) {
            throw "Destination already exists: $destination"
        }

        Move-Item -LiteralPath $path -Destination $destination -ErrorAction Stop

        $records.Add([PSCustomObject]@{
            Original = $path
            Destination = $destination
        })

        $operation = [PSCustomObject]@{
            Target = $target
            Parent = $parent
            Items = @($records.ToArray())
        }

        $operation | ConvertTo-Json -Depth 5 |
            Set-Content -LiteralPath $manifest -Encoding UTF8
    }

    # Custom folder icon
    $color = __COLOR__

    if ($color -ne 'Default') {
        try {
            Add-Type -AssemblyName System.Drawing

            $colors = @{
                Yellow = '#E8B943'
                Blue   = '#438CE8'
                Green  = '#43A66D'
                Red    = '#D85B5B'
                Purple = '#9368D7'
                Orange = '#E78A3C'
                Grey   = '#929AA7'
            }

            $hex = $colors[$color]
            if ($hex) {
                $main = [System.Drawing.ColorTranslator]::FromHtml($hex)

                $bitmap = New-Object System.Drawing.Bitmap(256, 256)
                $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
                $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
                $graphics.Clear([System.Drawing.Color]::Transparent)

                $brush = New-Object System.Drawing.SolidBrush($main)
                $tabColor = [System.Drawing.Color]::FromArgb(
                    255,
                    [Math]::Min(255, $main.R + 25),
                    [Math]::Min(255, $main.G + 25),
                    [Math]::Min(255, $main.B + 25)
                )
                $tabBrush = New-Object System.Drawing.SolidBrush($tabColor)

                $graphics.FillPolygon(
                    $tabBrush,
                    [System.Drawing.Point[]]@(
                        [System.Drawing.Point]::new(20, 45),
                        [System.Drawing.Point]::new(110, 45),
                        [System.Drawing.Point]::new(135, 80),
                        [System.Drawing.Point]::new(230, 80),
                        [System.Drawing.Point]::new(230, 110),
                        [System.Drawing.Point]::new(20, 110)
                    )
                )

                $graphics.FillPolygon(
                    $brush,
                    [System.Drawing.Point[]]@(
                        [System.Drawing.Point]::new(20, 90),
                        [System.Drawing.Point]::new(235, 90),
                        [System.Drawing.Point]::new(225, 210),
                        [System.Drawing.Point]::new(20, 210)
                    )
                )

                $png = New-Object System.IO.MemoryStream
                $bitmap.Save($png, [System.Drawing.Imaging.ImageFormat]::Png)
                $bytes = $png.ToArray()

                $ico = New-Object System.IO.MemoryStream
                $writer = New-Object System.IO.BinaryWriter($ico)
                $writer.Write([UInt16]0)
                $writer.Write([UInt16]1)
                $writer.Write([UInt16]1)
                $writer.Write([byte]0)
                $writer.Write([byte]0)
                $writer.Write([byte]0)
                $writer.Write([byte]0)
                $writer.Write([UInt16]1)
                $writer.Write([UInt16]32)
                $writer.Write([UInt32]$bytes.Length)
                $writer.Write([UInt32]22)
                $writer.Write($bytes)
                $writer.Flush()

                $iconPath = Join-Path $target 'folder.ico'
                [System.IO.File]::WriteAllBytes($iconPath, $ico.ToArray())

                $iniPath = Join-Path $target 'desktop.ini'
                $iniContent = "[.ShellClassInfo]`r`nConfirmFileOp=0`r`nIconFile=folder.ico`r`nIconIndex=0`r`n"
                $iniContent | Set-Content -LiteralPath $iniPath -Encoding Unicode

                [System.IO.File]::SetAttributes($iconPath, [System.IO.FileAttributes]::Hidden -bor [System.IO.FileAttributes]::System)
                [System.IO.File]::SetAttributes($iniPath, [System.IO.FileAttributes]::Hidden -bor [System.IO.FileAttributes]::System)
                $dirAttr = [System.IO.File]::GetAttributes($target)
                [System.IO.File]::SetAttributes($target, $dirAttr -bor [System.IO.FileAttributes]::ReadOnly -bor [System.IO.FileAttributes]::System)

                $writer.Dispose()
                $ico.Dispose()
                $png.Dispose()
                $graphics.Dispose()
                $bitmap.Dispose()
                $brush.Dispose()
                $tabBrush.Dispose()
            }
        } catch {
            "ICON WARNING: $_" | Add-Content -LiteralPath $debugFile
        }
    }

    # Refresh Explorer & Desktop
    $shell = New-Object -ComObject Shell.Application
    $allRefreshViews = [System.Collections.Generic.List[object]]::new()
    foreach ($w in $shell.Windows()) {
        try { $allRefreshViews.Add($w) } catch {}
    }
    try {
        $hwndDesktop = 0
        $desktop = $shell.Windows().FindWindowSW([ref]$null, [ref]$null, 8, [ref]$hwndDesktop, 1)
        if ($desktop) { $allRefreshViews.Add($desktop) }
    } catch {}

    foreach ($window in $allRefreshViews) {
        try {
            $view = [string]$window.Document.Folder.Self.Path
            if ($view.TrimEnd('\') -ieq $parent.TrimEnd('\')) {
                $window.Refresh()
            }
        } catch {}
    }

    Start-Sleep -Milliseconds 500

    $openAfter = __OPEN__
    $renameAfter = __RENAME__

    if ($openAfter) {
        Start-Process explorer.exe -ArgumentList ('"{0}"' -f $target)
    }
    elseif ($renameAfter) {
        $shell = New-Object -ComObject Shell.Application
        $allRenameViews = [System.Collections.Generic.List[object]]::new()
        foreach ($w in $shell.Windows()) {
            try { $allRenameViews.Add($w) } catch {}
        }
        try {
            $hwndDesktop = 0
            $desktop = $shell.Windows().FindWindowSW([ref]$null, [ref]$null, 8, [ref]$hwndDesktop, 1)
            if ($desktop) { $allRenameViews.Add($desktop) }
        } catch {}

        foreach ($window in $allRenameViews) {
            try {
                $view = [string]$window.Document.Folder.Self.Path
                if ($view.TrimEnd('\') -ieq $parent.TrimEnd('\')) {
                    for ($attempt = 0; $attempt -lt 10; $attempt++) {
                        $folderItem = $window.Document.Folder.ParseName($folderName)
                        if ($null -ne $folderItem) {
                            $window.Document.SelectItem($folderItem, 3)
                            break
                        }
                        Start-Sleep -Milliseconds 150
                    }
                    break
                }
            } catch {}
        }
    }

    "SUCCESS: $target" | Add-Content -LiteralPath $debugFile
}
catch {
    "ERROR: $($_.Exception.ToString())" | Add-Content -LiteralPath $debugFile

    Add-Type -AssemblyName System.Windows.Forms
    [System.Windows.Forms.MessageBox]::Show(
        $_.Exception.Message,
        'Quick File2folder',
        [System.Windows.Forms.MessageBoxButtons]::OK,
        [System.Windows.Forms.MessageBoxIcon]::Warning
    ) | Out-Null
}
finally {
    if ($ownsMutex) {
        $mutex.ReleaseMutex()
    }
    $mutex.Dispose()
}
)PS";

    std::wstring ps(script);

    ReplaceAll(ps, L"__PREFIX__", QuotePS(prefix.c_str()));
    ReplaceAll(ps, L"__STYLE__", QuotePS(style.c_str()));
    ReplaceAll(ps, L"__COLOR__", QuotePS(color.c_str()));

    ReplaceAll(ps, L"__ORGANIZE__", organize ? L"$true" : L"$false");
    ReplaceAll(ps, L"__OPEN__", open ? L"$true" : L"$false");
    ReplaceAll(ps, L"__RENAME__", rename ? L"$true" : L"$false");

    if (!WriteUnicodeFile(psPath, ps.c_str()))
        return false;

    std::wstring createScript =
        L"Set sh = CreateObject(\"WScript.Shell\")\r\n"
        L"sh.Run \"powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -WindowStyle Hidden -File \"\"" +
        psPath + L"\"\" -Mode Create\", 0, False\r\n";

    std::wstring undoScript =
        L"Set sh = CreateObject(\"WScript.Shell\")\r\n"
        L"sh.Run \"powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -WindowStyle Hidden -File \"\"" +
        psPath + L"\"\" -Mode Undo\", 0, False\r\n";

    return WriteUnicodeFile(createVbs, createScript.c_str()) &&
           WriteUnicodeFile(undoVbs, undoScript.c_str());
}

static bool RegisterAll() {
    wchar_t storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage)))
        return false;

    if (!WriteHelpers())
        return false;

    std::wstring root = storage;

    bool createOK = RegisterMenu(
        CREATE_KEY,
        L"Create folder with selected items",
        root + L"\\Create.vbs"
    );

    bool undoOK1 = RegisterMenu(
        UNDO_DIR_KEY,
        L"Undo Last Quick File2folder Move",
        root + L"\\Undo.vbs",
        L"%SystemRoot%\\System32\\imageres.dll,-5308"
    );

    bool undoOK2 = RegisterMenu(
        UNDO_DESKTOP_KEY,
        L"Undo Last Quick File2folder Move",
        root + L"\\Undo.vbs",
        L"%SystemRoot%\\System32\\imageres.dll,-5308"
    );

    SHChangeNotify(
        SHCNE_ASSOCCHANGED,
        SHCNF_IDLIST,
        nullptr,
        nullptr
    );

    Wh_Log(
        L"Quick File2folder: Registered (Create=%d, UndoDir=%d, UndoDesktop=%d)",
        createOK, undoOK1, undoOK2
    );

    return createOK && undoOK1 && undoOK2;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Quick File2folder v1.7.0 initializing.");
    return RegisterAll() ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Quick File2folder: Settings changed.");
    RegisterAll();
}

void Wh_ModUninit() {
    RegDeleteTreeW(HKEY_CURRENT_USER, CREATE_KEY);
    RegDeleteTreeW(HKEY_CURRENT_USER, UNDO_DIR_KEY);
    RegDeleteTreeW(HKEY_CURRENT_USER, UNDO_DESKTOP_KEY);

    SHChangeNotify(
        SHCNE_ASSOCCHANGED,
        SHCNF_IDLIST,
        nullptr,
        nullptr
    );

    Wh_Log(L"Quick File2folder unregistered.");
}
