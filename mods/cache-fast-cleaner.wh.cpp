// ==WindhawkMod==
// @id              cache-fast-cleaner
// @name            Cache Fast Cleaner
// @description     Adds Clean Temp, Clean %temp%, Clean Prefetch (and Clean All) to the right-click menu
// @version         1.1.0
// @author          ravensrock31
// @github          https://github.com/ravensrock31
// @include         explorer.exe
// @compilerOptions -lole32 -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Cache Fast Cleaner

Adds cleaning entries to the Explorer right-click menu:

* **Clean Temp**      - clears `C:\Windows\Temp` (asks for administrator rights)
* **Clean %temp%**    - clears the current user's `%TEMP%` folder
* **Clean Prefetch**  - clears `C:\Windows\Prefetch` (asks for administrator rights)
* **Clean All**       - all three at once (optional, off by default)

## Settings

* Show / hide each entry, group them in one submenu with a custom title
* Choose where the menu appears (folder background / desktop, folders, drives)
* Menu language (automatic, English, Turkish), icons, position (top / bottom), separators
* Show only while holding **Shift**
* Ask for confirmation before cleaning
* Keep files newer than N days
* Show a result message, show a visible console window

## Windows 11 note

Microsoft only lets packaged (MSIX) apps add items to the new top-level menu,
so entries appear in the classic menu / "Show more options". Turn on
*Always use the classic context menu* (restart Explorer afterwards) to get the
classic menu directly on right-click.

Files that are in use are skipped.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- items:
  - showTemp: true
    $name: Show "Clean Temp" (C:\Windows\Temp)
  - showUserTemp: true
    $name: Show "Clean %temp%" (user temp folder)
  - showPrefetch: true
    $name: Show "Clean Prefetch" (C:\Windows\Prefetch)
  - showAll: false
    $name: Show "Clean All" (runs all three)
  $name: Menu items
- menu:
  - groupInSubmenu: false
    $name: Group the entries in a single submenu
  - submenuTitle: ""
    $name: Submenu title
    $description: Leave empty for the default "Cache Fast Cleaner"
  - language: auto
    $name: Menu language
    $options:
    - auto: Automatic (system language)
    - en: English
    - tr: Türkçe
  - showIcons: true
    $name: Show icons
  - position: default
    $name: Position in the menu (classic menu only)
    $options:
    - default: Default
    - top: Top
    - bottom: Bottom
  - separatorBefore: false
    $name: Separator line before the entries
  - separatorAfter: false
    $name: Separator line after the entries
  - onlyWithShift: false
    $name: Show only while holding Shift
  $name: Menu appearance
- locations:
  - onFolderBackground: true
    $name: Folder background and desktop
  - onFolders: false
    $name: Right-clicking a folder
  - onDrives: false
    $name: Right-clicking a drive
  $name: Where to show the menu
- behavior:
  - askConfirm: false
    $name: Ask for confirmation before cleaning
  - showResult: true
    $name: Show a message with the freed space afterwards
  - olderThanDays: 0
    $name: Only delete files older than N days (0 = delete everything)
  - showConsole: false
    $name: Show a visible PowerShell window while cleaning
  $name: Cleaning behavior
- forceClassicMenu: false
  $name: Always use the classic context menu (Windows 11)
  $description: Restart Explorer after changing this option.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>

struct Item {
    const wchar_t* id;
    const wchar_t* target;
    const wchar_t* setting;
    const wchar_t* titleEn;
    const wchar_t* titleTr;
};

static const Item kItems[] = {
    {L"1_Temp",     L"Temp",     L"items.showTemp",     L"Clean Temp",     L"Temp Klas\u00f6r\u00fcn\u00fc Temizle"},
    {L"2_UserTemp", L"UserTemp", L"items.showUserTemp", L"Clean %temp%",   L"%temp% Klas\u00f6r\u00fcn\u00fc Temizle"},
    {L"3_Prefetch", L"Prefetch", L"items.showPrefetch", L"Clean Prefetch", L"Prefetch Klas\u00f6r\u00fcn\u00fc Temizle"},
    {L"4_All",      L"All",      L"items.showAll",      L"Clean All",      L"Hepsini Temizle"},
};

static const wchar_t kClassesRoot[]  = L"Software\\Classes\\";
static const wchar_t kSubmenuKey[]   = L"CacheFastCleaner.Menu";
static const wchar_t kSubmenuVerb[]  = L"CacheFastCleaner";
static const wchar_t kItemPrefix[]   = L"CacheFastCleaner_";
static const wchar_t kClassicClsid[] = L"{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}";
static const wchar_t kIcon[]         = L"%SystemRoot%\\System32\\cleanmgr.exe,0";

// Written as UTF-16 LE with BOM so Windows PowerShell reads it correctly.
static const wchar_t kScript[] = LR"PS(param(
    [ValidateSet('Temp','UserTemp','Prefetch','All')][string]$Target,
    [int]$Notify = 1,
    [int]$OlderThanDays = 0,
    [int]$AskConfirm = 0,
    [int]$ShowConsole = 0,
    [string]$Lang = 'en',
    [string]$UserTempPath = $env:TEMP
)

$tr = ($Lang -eq 'tr')
$S = @{
    Confirm = if ($tr) { 'Secilen klasorler temizlenecek. Devam edilsin mi?' } else { 'The selected folders will be cleaned. Continue?' }
    Cleaned = if ($tr) { 'Temizlendi' } else { 'Cleaned' }
    Freed   = if ($tr) { 'Kazanilan alan' } else { 'Freed' }
    Total   = if ($tr) { 'Toplam' } else { 'Total' }
}

$paths = [ordered]@{
    Temp     = (Join-Path $env:SystemRoot 'Temp')
    UserTemp = $UserTempPath
    Prefetch = (Join-Path $env:SystemRoot 'Prefetch')
}
$list = if ($Target -eq 'All') { @($paths.Keys) } else { @($Target) }
$needAdmin = @($list | Where-Object { $_ -ne 'UserTemp' }).Count -gt 0

if ($AskConfirm -eq 1) {
    Add-Type -AssemblyName PresentationFramework
    $names = ($list | ForEach-Object { $paths[$_] }) -join "`n"
    $r = [System.Windows.MessageBox]::Show("$($S.Confirm)`n`n$names", 'Cache Fast Cleaner', 'YesNo', 'Question')
    if ($r -ne 'Yes') { exit }
}

$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if ($needAdmin -and -not $isAdmin) {
    $style = if ($ShowConsole -eq 1) { 'Normal' } else { 'Hidden' }
    Start-Process powershell.exe -Verb RunAs -WindowStyle $style -ArgumentList @(
        '-NoProfile','-ExecutionPolicy','Bypass','-WindowStyle',$style,
        '-File', ('"' + $PSCommandPath + '"'),
        '-Target', $Target, '-Notify', $Notify, '-OlderThanDays', $OlderThanDays,
        '-AskConfirm', 0, '-ShowConsole', $ShowConsole, '-Lang', $Lang,
        '-UserTempPath', ('"' + $UserTempPath + '"'))
    exit
}

function Get-Size($p) {
    [long](Get-ChildItem -LiteralPath $p -Recurse -Force -File -ErrorAction SilentlyContinue |
        Measure-Object -Property Length -Sum).Sum
}

function Clear-Folder($p) {
    if (-not $p -or -not (Test-Path -LiteralPath $p)) { return [long]0 }
    $before = Get-Size $p
    if ($OlderThanDays -gt 0) {
        $cutoff = (Get-Date).AddDays(-$OlderThanDays)
        Get-ChildItem -LiteralPath $p -Recurse -Force -File -ErrorAction SilentlyContinue |
            Where-Object { $_.LastWriteTime -lt $cutoff } |
            Remove-Item -Force -ErrorAction SilentlyContinue
        Get-ChildItem -LiteralPath $p -Recurse -Force -Directory -ErrorAction SilentlyContinue |
            Sort-Object FullName -Descending |
            Where-Object { -not (Get-ChildItem -LiteralPath $_.FullName -Force -ErrorAction SilentlyContinue) } |
            Remove-Item -Force -ErrorAction SilentlyContinue
    } else {
        Get-ChildItem -LiteralPath $p -Force -ErrorAction SilentlyContinue |
            Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
    }
    return [long]($before - (Get-Size $p))
}

$lines = @()
$total = [long]0
foreach ($t in $list) {
    $freed = Clear-Folder $paths[$t]
    $total += $freed
    $mb = [math]::Round($freed / 1MB, 2)
    $lines += "$($S.Cleaned): $($paths[$t])  ($mb MB)"
}

if ($Notify -eq 1) {
    Add-Type -AssemblyName PresentationFramework
    $totalMb = [math]::Round($total / 1MB, 2)
    $text = ($lines -join "`n") + "`n`n$($S.Total) $($S.Freed): $totalMb MB"
    [void][System.Windows.MessageBox]::Show($text, 'Cache Fast Cleaner')
}
)PS";

// ---------------------------------------------------------------- config

struct Config {
    bool submenu;
    std::wstring submenuTitle;
    bool turkish;
    bool icons;
    std::wstring position;   // "", "Top", "Bottom"
    bool sepBefore, sepAfter;
    bool shiftOnly;
    bool askConfirm, showResult, showConsole;
    int  olderThanDays;
    std::vector<std::wstring> parents;
    std::vector<const Item*> items;
};

static std::wstring g_scriptDir;
static std::wstring g_scriptPath;
static bool g_classicApplied = false;

static std::wstring GetStr(const wchar_t* name) {
    PCWSTR s = Wh_GetStringSetting(name);
    std::wstring r = s ? s : L"";
    Wh_FreeStringSetting(s);
    return r;
}

static Config LoadConfig() {
    Config c;
    c.submenu       = Wh_GetIntSetting(L"menu.groupInSubmenu") != 0;
    c.submenuTitle  = GetStr(L"menu.submenuTitle");
    if (c.submenuTitle.empty()) c.submenuTitle = L"Cache Fast Cleaner";

    std::wstring lang = GetStr(L"menu.language");
    if (lang == L"tr")      c.turkish = true;
    else if (lang == L"en") c.turkish = false;
    else c.turkish = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_TURKISH;

    c.icons      = Wh_GetIntSetting(L"menu.showIcons") != 0;
    std::wstring pos = GetStr(L"menu.position");
    c.position   = pos == L"top" ? L"Top" : pos == L"bottom" ? L"Bottom" : L"";
    c.sepBefore  = Wh_GetIntSetting(L"menu.separatorBefore") != 0;
    c.sepAfter   = Wh_GetIntSetting(L"menu.separatorAfter") != 0;
    c.shiftOnly  = Wh_GetIntSetting(L"menu.onlyWithShift") != 0;

    c.askConfirm    = Wh_GetIntSetting(L"behavior.askConfirm") != 0;
    c.showResult    = Wh_GetIntSetting(L"behavior.showResult") != 0;
    c.showConsole   = Wh_GetIntSetting(L"behavior.showConsole") != 0;
    c.olderThanDays = Wh_GetIntSetting(L"behavior.olderThanDays");
    if (c.olderThanDays < 0) c.olderThanDays = 0;

    if (Wh_GetIntSetting(L"locations.onFolderBackground")) c.parents.push_back(L"Directory\\Background");
    if (Wh_GetIntSetting(L"locations.onFolders"))          c.parents.push_back(L"Directory");
    if (Wh_GetIntSetting(L"locations.onDrives"))           c.parents.push_back(L"Drive");

    for (const auto& item : kItems) {
        if (Wh_GetIntSetting(item.setting)) c.items.push_back(&item);
    }
    return c;
}

// ---------------------------------------------------------------- helpers

static bool SetStr(const std::wstring& subKey, const wchar_t* name, const std::wstring& value) {
    HKEY key;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, subKey.c_str(), 0, nullptr, 0,
                        KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
        return false;
    }
    LONG r = RegSetValueExW(key, name, 0, REG_SZ,
                            reinterpret_cast<const BYTE*>(value.c_str()),
                            static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(key);
    return r == ERROR_SUCCESS;
}

static void DeleteTree(const std::wstring& subKey) {
    RegDeleteTreeW(HKEY_CURRENT_USER, subKey.c_str());
}

static const wchar_t* AllParents[] = {L"Directory\\Background", L"Directory", L"Drive"};

static bool PrepareScript() {
    wchar_t localAppData[MAX_PATH];
    DWORD len = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return false;
    }
    g_scriptDir = std::wstring(localAppData) + L"\\CacheFastCleaner";
    g_scriptPath = g_scriptDir + L"\\clean.ps1";

    CreateDirectoryW(g_scriptDir.c_str(), nullptr);

    HANDLE file = CreateFileW(g_scriptPath.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Could not write script: %lu", GetLastError());
        return false;
    }
    const BYTE bom[] = {0xFF, 0xFE};
    DWORD written;
    WriteFile(file, bom, sizeof(bom), &written, nullptr);
    WriteFile(file, kScript, static_cast<DWORD>(wcslen(kScript) * sizeof(wchar_t)),
              &written, nullptr);
    CloseHandle(file);
    return true;
}

static std::wstring BuildCommand(const Config& c, const Item& item) {
    return std::wstring(L"powershell.exe -NoProfile -ExecutionPolicy Bypass -WindowStyle ") +
           (c.showConsole ? L"Normal" : L"Hidden") +
           L" -File \"" + g_scriptPath + L"\"" +
           L" -Target " + item.target +
           L" -Notify " + (c.showResult ? L"1" : L"0") +
           L" -OlderThanDays " + std::to_wstring(c.olderThanDays) +
           L" -AskConfirm " + (c.askConfirm ? L"1" : L"0") +
           L" -ShowConsole " + (c.showConsole ? L"1" : L"0") +
           L" -Lang " + (c.turkish ? L"tr" : L"en");
}

static const wchar_t* TitleOf(const Config& c, const Item& item) {
    return c.turkish ? item.titleTr : item.titleEn;
}

static void ApplyVerbOptions(const Config& c, const std::wstring& key,
                             bool topLevel, bool sepBefore, bool sepAfter) {
    if (c.icons) SetStr(key, L"Icon", kIcon);
    if (!topLevel) return;
    if (c.shiftOnly)        SetStr(key, L"Extended", L"");
    if (!c.position.empty()) SetStr(key, L"Position", c.position);
    if (sepBefore)          SetStr(key, L"SeparatorBefore", L"");
    if (sepAfter)           SetStr(key, L"SeparatorAfter", L"");
}

static void AddVerb(const Config& c, const std::wstring& key, const Item& item,
                    bool topLevel, bool sepBefore, bool sepAfter) {
    SetStr(key, nullptr, TitleOf(c, item));
    SetStr(key, L"MUIVerb", TitleOf(c, item));
    ApplyVerbOptions(c, key, topLevel, sepBefore, sepAfter);
    SetStr(key + L"\\command", nullptr, BuildCommand(c, item));
}

// ---------------------------------------------------------------- register

static void UnregisterMenu() {
    for (const auto* parent : AllParents) {
        std::wstring shell = std::wstring(kClassesRoot) + parent + L"\\shell\\";
        DeleteTree(shell + kSubmenuVerb);
        for (const auto& item : kItems) {
            DeleteTree(shell + kItemPrefix + item.id);
        }
    }
    DeleteTree(std::wstring(kClassesRoot) + kSubmenuKey);
}

static void RegisterMenu(const Config& c) {
    UnregisterMenu();
    if (c.items.empty()) return;

    for (const auto& parent : c.parents) {
        std::wstring shell = std::wstring(kClassesRoot) + parent + L"\\shell\\";

        if (c.submenu) {
            std::wstring root = shell + kSubmenuVerb;
            SetStr(root, L"MUIVerb", c.submenuTitle);
            SetStr(root, L"ExtendedSubCommandsKey", kSubmenuKey);
            ApplyVerbOptions(c, root, true, c.sepBefore, c.sepAfter);
        } else {
            for (size_t i = 0; i < c.items.size(); i++) {
                const Item& item = *c.items[i];
                AddVerb(c, shell + kItemPrefix + item.id, item, true,
                        c.sepBefore && i == 0,
                        c.sepAfter && i == c.items.size() - 1);
            }
        }
    }

    if (c.submenu && !c.parents.empty()) {
        std::wstring subShell = std::wstring(kClassesRoot) + kSubmenuKey + L"\\shell\\";
        for (const Item* item : c.items) {
            AddVerb(c, subShell + item->id, *item, false, false, false);
        }
    }
}

static void ApplyClassicMenuSetting() {
    std::wstring clsidKey = std::wstring(kClassesRoot) + L"CLSID\\" + kClassicClsid;
    if (Wh_GetIntSetting(L"forceClassicMenu")) {
        SetStr(clsidKey + L"\\InprocServer32", nullptr, L"");
        g_classicApplied = true;
    } else if (g_classicApplied) {
        DeleteTree(clsidKey);
        g_classicApplied = false;
    }
}

static void Apply() {
    Config c = LoadConfig();
    RegisterMenu(c);
    ApplyClassicMenuSetting();
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}

// ---------------------------------------------------------------- Windhawk

BOOL Wh_ModInit() {
    Wh_Log(L"Cache Fast Cleaner init");
    if (!PrepareScript()) {
        return FALSE;
    }
    Apply();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    PrepareScript();
    Apply();
}

void Wh_ModUninit() {
    Wh_Log(L"Cache Fast Cleaner uninit");
    UnregisterMenu();

    if (g_classicApplied) {
        DeleteTree(std::wstring(kClassesRoot) + L"CLSID\\" + kClassicClsid);
        g_classicApplied = false;
    }

    DeleteFileW(g_scriptPath.c_str());
    RemoveDirectoryW(g_scriptDir.c_str());
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}
