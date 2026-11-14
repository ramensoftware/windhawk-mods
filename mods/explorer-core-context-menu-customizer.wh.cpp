// ==WindhawkMod==
// @id              explorer-core-context-menu-customizer
// @name            Explorer Core Context Menu Customizer
// @description     Adds customizable icons and dynamic %file% labels to Open, Run as administrator, Delete, Rename, and Properties in the Windows File Explorer context menu without altering row spacing.
// @version         1.0.0
// @author          akiyorm
// @github          https://github.com/akiyorm
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -lshlwapi -lole32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Core Context Menu Customizer

Windows does not provide any registry entries or built-in options to customize the icons or labels of core File Explorer context menu commands. This mod was created to fill that gap cleanly and safely.

> **Windows 11 Note:** This mod customizes the classic File Explorer context menu (accessible via **Show more options** or `Shift+F10`, or when the classic context menu is enabled).

### Screenshots:
| Disabled (Default) | Enabled (Customized) |
| :---: | :---: |
| ![Disabled](https://i.imgur.com/aoglE8P.png) | ![Enabled](https://i.imgur.com/norFuuE.png) |

### Highlights:
- **Canonical Verb Matching:** Identifies core shell commands via `IContextMenu::GetCommandString` (`GCS_VERBW`), working across every Windows display language without touching third-party shell extension items.
- **Zero Layout Distortion:** Attaches icons cleanly during `IContextMenu::QueryContextMenu` so native row height, padding, and bold text remain completely untouched.
- **Clear-to-Reset Behavior:** Clearing an icon path removes the icon for Open, Delete, Rename, or Properties, while Run as administrator reverts to the native Windows UAC shield.
- **Custom Icon Support:** Easily add your own `.ico` files or use any built-in Windows icon with `dll,index` syntax.
- **Per-Monitor DPI & Leak-Free Caching:** Caches 32-bpp ARGB bitmaps per monitor DPI in memory with zero GDI handle leaks.

### Dynamic `%file%` Placeholder:
Include `%file%` (or `%name%`) in any custom label field to dynamically display the right-clicked file, folder, or shortcut target name. Names longer than 30 characters are automatically shortened to the first 30 characters followed by `...`:
- `Rename: '%file%'` -> **Rename: 'Spotify.exe'**
- `Open: %file%` -> **Open: New folder (2)**
- `Delete %file%` -> **Delete Very_Long_File_Name_Over_Thirt...**
- `%file% Properties` -> **report.pdf Properties**

### Supported Context Menu Items:
1. **Open** (`open`) - Top bold default action (default icon: `imageres.dll,-185`)
2. **Run as administrator** (`runas`) - UAC elevation action (keeps native UAC shield and default label unless customized)
3. **Delete** (`delete`) - File and folder deletion action (default icon: Full Recycle Bin `imageres.dll,-54`)
4. **Rename** (`rename`) - Inline rename action (default icon: Clipboard `shell32.dll,-16763`, default label: `Rename: '%file%'`)
5. **Properties** (`properties`) - Bottom item details action (default icon: `imageres.dll,-114`)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableOpenIcon: true
  $name: "1. Enable Open Customization"
  $description: "Enable icon and/or custom label for the top bold Open option."
- openIconPath: "imageres.dll,-185"
  $name: "Open Icon Path or DLL Index"
  $description: "Default is imageres.dll,-185. Leave empty for no icon, or enter a .ico path or DLL index."
- openCustomText: ""
  $name: "Open Custom Label (Supports %file%)"
  $description: "Leave empty for default system name, or enter custom text using %file% (%file% is truncated after 30 chars)."

- enableRunAsIcon: true
  $name: "2. Enable Run as administrator Customization"
  $description: "Enable custom icon and/or custom label for Run as administrator."
- runAsIconPath: ""
  $name: "Run as administrator Icon Path or DLL Index"
  $description: "Leave empty to keep the default Windows UAC shield, or enter a .ico path or DLL index (e.g. shell32.dll,44)."
- runAsCustomText: ""
  $name: "Run as administrator Custom Label (Supports %file%)"
  $description: "Leave empty to keep default system name untouched, or enter custom text using %file% (%file% is truncated after 30 chars)."

- enableDeleteIcon: true
  $name: "3. Enable Delete Customization"
  $description: "Enable icon and/or custom label for the Delete option."
- deleteIconPath: "imageres.dll,-54"
  $name: "Delete Icon Path or DLL Index"
  $description: "Default is Full Recycle Bin (imageres.dll,-54). Leave empty for no icon, or enter a .ico path or DLL index."
- deleteCustomText: ""
  $name: "Delete Custom Label (Supports %file%)"
  $description: "Leave empty for default system name, or enter custom text using %file% (%file% is truncated after 30 chars)."

- enableRenameIcon: true
  $name: "4. Enable Rename Customization"
  $description: "Enable icon and/or custom label for the Rename option."
- renameIconPath: "shell32.dll,-16763"
  $name: "Rename Icon Path or DLL Index"
  $description: "Default is Clipboard (shell32.dll,-16763). Leave empty for no icon, or enter a .ico path or DLL index."
- renameCustomText: "Rename: '%file%'"
  $name: "Rename Custom Label (Supports %file%)"
  $description: "Default is Rename: '%file%'. Leave empty for default system name, or enter custom text (%file% is truncated after 30 chars)."

- enablePropertiesIcon: true
  $name: "5. Enable Properties Customization"
  $description: "Enable icon and/or custom label for the bottom Properties option."
- propertiesIconPath: "imageres.dll,-114"
  $name: "Properties Icon Path or DLL Index"
  $description: "Default is imageres.dll,-114. Leave empty for no icon, or enter a .ico path or DLL index."
- propertiesCustomText: ""
  $name: "Properties Custom Label (Supports %file%)"
  $description: "Leave empty for default system name, or enter custom text using %file% (%file% is truncated after 30 chars)."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windhawk_utils.h>
#include <cwchar>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// Global Configuration & Synchronization
// ----------------------------------------------------------------------------
struct DpiBitmapEntry {
    UINT dpi;
    HBITMAP hBitmap;
};

struct ItemConfig {
    bool enabled = true;
    std::wstring iconPath;
    std::wstring customText;
    std::vector<DpiBitmapEntry> dpiBitmaps;
};

struct ItemBinding {
    ItemConfig* cfg;
    PCWSTR enableSetting;
    PCWSTR iconSetting;
    PCWSTR textSetting;
    LPCWSTR canonicalVerb;
};

static ItemConfig g_openCfg;
static ItemConfig g_runAsCfg;
static ItemConfig g_deleteCfg;
static ItemConfig g_renameCfg;
static ItemConfig g_propsCfg;

static const ItemBinding g_itemBindings[] = {
    { &g_openCfg,   L"enableOpenIcon",       L"openIconPath",       L"openCustomText",       L"open" },
    { &g_runAsCfg,  L"enableRunAsIcon",      L"runAsIconPath",      L"runAsCustomText",      L"runas" },
    { &g_deleteCfg, L"enableDeleteIcon",     L"deleteIconPath",     L"deleteCustomText",     L"delete" },
    { &g_renameCfg, L"enableRenameIcon",     L"renameIconPath",     L"renameCustomText",     L"rename" },
    { &g_propsCfg,  L"enablePropertiesIcon", L"propertiesIconPath", L"propertiesCustomText", L"properties" },
};

static SRWLOCK g_configLock = SRWLOCK_INIT;
// Bitmaps retired on settings reload are kept alive until mod unload so an open
// menu in another Explorer window never loses its icon mid-display.
static std::vector<HBITMAP> g_retiredBitmaps;
static bool g_queryContextMenuHooked = false;
static thread_local bool tls_inSelfProbe = false;

class SharedConfigGuard {
public:
    SharedConfigGuard() { AcquireSRWLockShared(&g_configLock); }
    ~SharedConfigGuard() { ReleaseSRWLockShared(&g_configLock); }
};

class ExclusiveConfigGuard {
public:
    ExclusiveConfigGuard() { AcquireSRWLockExclusive(&g_configLock); }
    ~ExclusiveConfigGuard() { ReleaseSRWLockExclusive(&g_configLock); }
};

// ----------------------------------------------------------------------------
// Per-Monitor DPI Helpers
// ----------------------------------------------------------------------------
static UINT GetContextMenuDpi() {
    POINT pt = {};
    DWORD msgPos = GetMessagePos();
    if (msgPos != 0) {
        pt.x = static_cast<short>(LOWORD(msgPos));
        pt.y = static_cast<short>(HIWORD(msgPos));
    } else {
        GetCursorPos(&pt);
    }

    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (hMon) {
        using GetDpiForMonitor_t = HRESULT (WINAPI*)(HMONITOR, int, UINT*, UINT*);
        static auto pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
            HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
            if (!hShcore) {
                hShcore = LoadLibraryW(L"shcore.dll");
            }
            return hShcore
                ? reinterpret_cast<GetDpiForMonitor_t>(GetProcAddress(hShcore, "GetDpiForMonitor"))
                : nullptr;
        }();

        if (pGetDpiForMonitor) {
            UINT dpiX = 0;
            UINT dpiY = 0;
            // MDT_EFFECTIVE_DPI == 0
            if (SUCCEEDED(pGetDpiForMonitor(hMon, 0, &dpiX, &dpiY)) && dpiX > 0) {
                return dpiX;
            }
        }
    }

    using GetDpiForSystem_t = UINT (WINAPI*)();
    static auto pGetDpiForSystem = []() -> GetDpiForSystem_t {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        return hUser32
            ? reinterpret_cast<GetDpiForSystem_t>(GetProcAddress(hUser32, "GetDpiForSystem"))
            : nullptr;
    }();

    if (pGetDpiForSystem) {
        UINT sysDpi = pGetDpiForSystem();
        if (sysDpi > 0) {
            return sysDpi;
        }
    }

    return 96;
}

static void GetSmallIconSizeForDpi(UINT dpi, int& outCx, int& outCy) {
    using GetSystemMetricsForDpi_t = int (WINAPI*)(int, UINT);
    static auto pGetSystemMetricsForDpi = []() -> GetSystemMetricsForDpi_t {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        return hUser32
            ? reinterpret_cast<GetSystemMetricsForDpi_t>(GetProcAddress(hUser32, "GetSystemMetricsForDpi"))
            : nullptr;
    }();

    if (pGetSystemMetricsForDpi && dpi > 0) {
        outCx = pGetSystemMetricsForDpi(SM_CXSMICON, dpi);
        outCy = pGetSystemMetricsForDpi(SM_CYSMICON, dpi);
    } else {
        outCx = GetSystemMetrics(SM_CXSMICON);
        outCy = GetSystemMetrics(SM_CYSMICON);
    }

    if (outCx <= 0) outCx = 16;
    if (outCy <= 0) outCy = 16;
}

// ----------------------------------------------------------------------------
// String Helpers: Truncation, Escaping & %file% Expansion
// ----------------------------------------------------------------------------
static std::wstring TruncateFileName(std::wstring name, size_t maxChars = 30) {
    if (name.size() > maxChars) {
        size_t cutLen = maxChars;
        if (cutLen > 0 && IS_HIGH_SURROGATE(name[cutLen - 1])) {
            --cutLen;
        }
        name.erase(cutLen);
        name.append(L"...");
    }
    return name;
}

static std::wstring EscapeMenuMnemonics(const std::wstring& input) {
    std::wstring out;
    out.reserve(input.size());
    for (wchar_t ch : input) {
        if (ch == L'&') {
            out.append(L"&&");
        } else {
            out.push_back(ch);
        }
    }
    return out;
}

static std::wstring ResolveDynamicLabel(const std::wstring& tpl, const std::wstring& itemName) {
    if (tpl.empty()) return std::wstring();

    std::wstring result = tpl;
    std::wstring safeItemName = EscapeMenuMnemonics(itemName);

    const wchar_t* tokens[] = { L"%file%", L"%name%" };
    for (const wchar_t* token : tokens) {
        size_t tokenLen = wcslen(token);
        size_t pos = 0;
        while (pos < result.size()) {
            if (pos + tokenLen <= result.size() &&
                _wcsnicmp(result.c_str() + pos, token, tokenLen) == 0) {
                result.replace(pos, tokenLen, safeItemName);
                pos += safeItemName.size();
            } else {
                ++pos;
            }
        }
    }

    // Clean up extra spaces, empty quotes, and trailing colon when %file% is empty
    if (safeItemName.empty()) {
        size_t q = 0;
        while ((q = result.find(L"''", q)) != std::wstring::npos) {
            result.erase(q, 2);
        }
        size_t dq = 0;
        while ((dq = result.find(L"\"\"", dq)) != std::wstring::npos) {
            result.erase(dq, 2);
        }
        size_t dbl = 0;
        while ((dbl = result.find(L"  ", dbl)) != std::wstring::npos) {
            result.erase(dbl, 1);
        }
        while (!result.empty() && result.front() == L' ') result.erase(0, 1);
        while (!result.empty() && (result.back() == L' ' || result.back() == L':')) result.pop_back();
    }

    return result;
}

static size_t FindBindingIndexByVerb(LPCWSTR verb) {
    if (!verb || verb[0] == L'\0') return static_cast<size_t>(-1);

    for (size_t i = 0; i < ARRAYSIZE(g_itemBindings); ++i) {
        if (_wcsicmp(verb, g_itemBindings[i].canonicalVerb) == 0) {
            return i;
        }
    }

    return static_cast<size_t>(-1);
}

// ----------------------------------------------------------------------------
// Extract Clicked Item (or Shortcut Target) Display Name
// ----------------------------------------------------------------------------
static std::wstring ResolveShortcutTargetName(IShellFolder* psf, PCUITEMID_CHILD pidlItem) {
    if (!psf || !pidlItem) return std::wstring();

    SFGAOF attrs = SFGAO_LINK;
    if (FAILED(psf->GetAttributesOf(1, &pidlItem, &attrs)) || !(attrs & SFGAO_LINK)) {
        return std::wstring();
    }

    IShellLinkW* pShellLink = nullptr;
    if (FAILED(psf->BindToObject(pidlItem, nullptr, IID_IShellLinkW, reinterpret_cast<void**>(&pShellLink))) || !pShellLink) {
        return std::wstring();
    }

    std::wstring targetName;

    PIDLIST_ABSOLUTE pidlTarget = nullptr;
    if (SUCCEEDED(pShellLink->GetIDList(&pidlTarget)) && pidlTarget) {
        PWSTR pszParsingName = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(pidlTarget, SIGDN_PARENTRELATIVEPARSING, &pszParsingName)) && pszParsingName) {
            size_t len = wcslen(pszParsingName);
            if (len > 0 &&
                pszParsingName[len - 1] != L'\\' &&
                pszParsingName[len - 1] != L':' &&
                wcsncmp(pszParsingName, L"::{", 3) != 0) {
                targetName = pszParsingName;
            }
            CoTaskMemFree(pszParsingName);
        }

        if (targetName.empty()) {
            PWSTR pszDisplayName = nullptr;
            if (SUCCEEDED(SHGetNameFromIDList(pidlTarget, SIGDN_NORMALDISPLAY, &pszDisplayName)) && pszDisplayName) {
                targetName = pszDisplayName;
                CoTaskMemFree(pszDisplayName);
            }
        }

        CoTaskMemFree(pidlTarget);
    }

    if (targetName.empty()) {
        WCHAR szTargetPath[MAX_PATH] = { 0 };
        if (SUCCEEDED(pShellLink->GetPath(szTargetPath, ARRAYSIZE(szTargetPath), nullptr, 0)) &&
            szTargetPath[0] != L'\0' &&
            !PathIsURLW(szTargetPath)) {
            LPCWSTR fileName = PathFindFileNameW(szTargetPath);
            if (fileName && fileName[0] != L'\0') {
                targetName = fileName;
            }
        }
    }

    pShellLink->Release();
    return targetName;
}

static std::wstring GetItemNameFromShellFolder(IShellFolder* psf, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, PCIDLIST_ABSOLUTE pidlFolder) {
    if (cidl > 0 && psf && apidl && apidl[0]) {
        std::wstring rawName = ResolveShortcutTargetName(psf, apidl[0]);

        if (rawName.empty()) {
            STRRET strret = {};
            if (SUCCEEDED(psf->GetDisplayNameOf(apidl[0], SHGDN_INFOLDER, &strret))) {
                WCHAR szName[MAX_PATH] = { 0 };
                if (SUCCEEDED(StrRetToBufW(&strret, apidl[0], szName, ARRAYSIZE(szName))) && szName[0] != L'\0') {
                    rawName = szName;
                }
            }
        }

        if (!rawName.empty()) {
            std::wstring name = TruncateFileName(std::move(rawName), 30);
            if (cidl > 1) {
                name += L" (+" + std::to_wstring(cidl - 1) + L")";
            }
            return name;
        }
    }

    if (cidl == 0 && pidlFolder) {
        PWSTR pszFolderName = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(pidlFolder, SIGDN_NORMALDISPLAY, &pszFolderName)) && pszFolderName) {
            std::wstring folderName = TruncateFileName(pszFolderName, 30);
            CoTaskMemFree(pszFolderName);
            return folderName;
        }
    }

    return std::wstring();
}

// ----------------------------------------------------------------------------
// Convert HICON into a 32-bit Premultiplied ARGB Top-Down DIBSection
// ----------------------------------------------------------------------------
static HBITMAP IconToPremultipliedBitmap(HICON hIcon, int width, int height) {
    if (!hIcon) return nullptr;

    ICONINFO ii = {};
    if (!GetIconInfo(hIcon, &ii)) {
        return nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hbmDIB = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!hbmDIB || !pvBits) {
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
        return nullptr;
    }

    int totalPixels = width * height;
    ZeroMemory(pvBits, totalPixels * 4);

    HDC hdc = CreateCompatibleDC(nullptr);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbmDIB);
    // DrawIconEx onto a zeroed 32-bpp DIB already produces premultiplied ARGB pixels
    // when the icon has an alpha channel; multiplying again would darken edges.
    DrawIconEx(hdc, 0, 0, hIcon, width, height, 0, nullptr, DI_NORMAL);
    GdiFlush();
    SelectObject(hdc, hbmOld);
    DeleteDC(hdc);

    DWORD* pixels = static_cast<DWORD*>(pvBits);
    bool hasAlpha = false;
    for (int i = 0; i < totalPixels; ++i) {
        if ((pixels[i] >> 24) != 0) {
            hasAlpha = true;
            break;
        }
    }

    if (!hasAlpha && ii.hbmMask) {
        std::vector<DWORD> maskBits(totalPixels, 0);
        HDC hdcMask = CreateCompatibleDC(nullptr);
        GetDIBits(hdcMask, ii.hbmMask, 0, height, maskBits.data(), &bmi, DIB_RGB_COLORS);
        DeleteDC(hdcMask);
        for (int i = 0; i < totalPixels; ++i) {
            if ((maskBits[i] & 0x00FFFFFF) == 0) {
                pixels[i] |= 0xFF000000;
            } else {
                pixels[i] = 0;
            }
        }
    }

    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);

    return hbmDIB;
}

// ----------------------------------------------------------------------------
// Universal Icon Loader: Supports .ico Paths and "dll,index" Syntax
// ----------------------------------------------------------------------------
static HICON LoadIconUniversal(const std::wstring& rawInput, int cx, int cy) {
    if (rawInput.empty()) return nullptr;

    std::wstring path = rawInput;
    int index = 0;

    size_t comma = path.rfind(L',');
    if (comma != std::wstring::npos) {
        index = _wtoi(path.c_str() + comma + 1);
        path.erase(comma);
    }

    while (!path.empty() && (path.front() == L' ' || path.front() == L'"')) path.erase(0, 1);
    while (!path.empty() && (path.back() == L' ' || path.back() == L'"')) path.pop_back();

    if (path.empty()) return nullptr;

    if (path.find(L'\\') == std::wstring::npos && path.find(L'/') == std::wstring::npos) {
        WCHAR sysDir[MAX_PATH] = { 0 };
        UINT len = GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir));
        if (len > 0 && len < ARRAYSIZE(sysDir)) {
            std::wstring candidate = std::wstring(sysDir, len) + L"\\" + path;
            if (GetFileAttributesW(candidate.c_str()) != INVALID_FILE_ATTRIBUTES) {
                path = std::move(candidate);
            }
        }
    }

    HICON hIcon = nullptr;

    if (comma == std::wstring::npos && path.size() >= 4 &&
        _wcsicmp(path.c_str() + path.size() - 4, L".ico") == 0) {
        hIcon = (HICON)LoadImageW(nullptr, path.c_str(), IMAGE_ICON, cx, cy, LR_LOADFROMFILE);
    }

    if (!hIcon) {
        UINT iconId = 0;
        PrivateExtractIconsW(path.c_str(), index, cx, cy, &hIcon, &iconId, 1, LR_LOADFROMFILE);
    }

    if (!hIcon) {
        ExtractIconExW(path.c_str(), index, nullptr, &hIcon, 1);
    }

    return hIcon;
}

static HBITMAP CreateBitmapForIconPath(const std::wstring& iconPath, UINT dpi) {
    if (iconPath.empty()) return nullptr;
    int cx = 16;
    int cy = 16;
    GetSmallIconSizeForDpi(dpi, cx, cy);

    HICON hIcon = LoadIconUniversal(iconPath, cx, cy);
    if (!hIcon) return nullptr;

    HBITMAP hBitmap = IconToPremultipliedBitmap(hIcon, cx, cy);
    DestroyIcon(hIcon);
    return hBitmap;
}

// ----------------------------------------------------------------------------
// Per-DPI Bitmap Lookup, Settings Loading & Final Cleanup
// ----------------------------------------------------------------------------
static HBITMAP GetOrCreateBitmapForBinding(size_t bindingIdx, UINT dpi) {
    if (bindingIdx >= ARRAYSIZE(g_itemBindings)) return nullptr;
    ItemConfig& cfg = *g_itemBindings[bindingIdx].cfg;

    std::wstring iconPathCopy;
    {
        SharedConfigGuard lock;
        if (!cfg.enabled || cfg.iconPath.empty()) {
            return nullptr;
        }
        for (const auto& entry : cfg.dpiBitmaps) {
            if (entry.dpi == dpi) {
                return entry.hBitmap;
            }
        }
        iconPathCopy = cfg.iconPath;
    }

    HBITMAP newBmp = CreateBitmapForIconPath(iconPathCopy, dpi);
    if (!newBmp) {
        return nullptr;
    }

    ExclusiveConfigGuard lock;
    if (!cfg.enabled || cfg.iconPath != iconPathCopy) {
        DeleteObject(newBmp);
        return nullptr;
    }
    for (const auto& entry : cfg.dpiBitmaps) {
        if (entry.dpi == dpi) {
            DeleteObject(newBmp);
            return entry.hBitmap;
        }
    }

    cfg.dpiBitmaps.push_back({ dpi, newBmp });
    return newBmp;
}

static void LoadAllSettings() {
    UINT initialDpi = GetContextMenuDpi();

    constexpr size_t kBindingCount = ARRAYSIZE(g_itemBindings);
    ItemConfig newConfigs[kBindingCount];

    for (size_t i = 0; i < kBindingCount; ++i) {
        const auto& binding = g_itemBindings[i];
        newConfigs[i].enabled = Wh_GetIntSetting(binding.enableSetting) != 0;

        auto iconSetting = WindhawkUtils::StringSetting::make(binding.iconSetting);
        if (iconSetting.get()[0] != L'\0') {
            newConfigs[i].iconPath = iconSetting.get();
            if (newConfigs[i].enabled) {
                HBITMAP hBmp = CreateBitmapForIconPath(newConfigs[i].iconPath, initialDpi);
                if (hBmp) {
                    newConfigs[i].dpiBitmaps.push_back({ initialDpi, hBmp });
                }
            }
        }

        auto textSetting = WindhawkUtils::StringSetting::make(binding.textSetting);
        if (textSetting.get()[0] != L'\0') {
            newConfigs[i].customText = textSetting.get();
        }
    }

    ExclusiveConfigGuard lock;
    for (size_t i = 0; i < kBindingCount; ++i) {
        std::swap(*g_itemBindings[i].cfg, newConfigs[i]);
        for (const auto& entry : newConfigs[i].dpiBitmaps) {
            if (entry.hBitmap) {
                g_retiredBitmaps.push_back(entry.hBitmap);
            }
        }
        newConfigs[i].dpiBitmaps.clear();
    }
}

// ----------------------------------------------------------------------------
// Per-Menu Item Name Cache (Keyed by Canonical IContextMenu* Pointer)
// ----------------------------------------------------------------------------
struct MenuItemNameEntry {
    void* cmIdentity;
    std::wstring itemName;
};

static SRWLOCK g_nameCacheLock = SRWLOCK_INIT;
static std::vector<MenuItemNameEntry> g_menuItemNames;

static void CleanupAllResources() {
    {
        ExclusiveConfigGuard lock;
        for (const auto& binding : g_itemBindings) {
            for (const auto& entry : binding.cfg->dpiBitmaps) {
                if (entry.hBitmap) {
                    DeleteObject(entry.hBitmap);
                }
            }
            binding.cfg->dpiBitmaps.clear();
        }
        for (HBITMAP hBmp : g_retiredBitmaps) {
            if (hBmp) {
                DeleteObject(hBmp);
            }
        }
        g_retiredBitmaps.clear();
    }

    AcquireSRWLockExclusive(&g_nameCacheLock);
    g_menuItemNames.clear();
    ReleaseSRWLockExclusive(&g_nameCacheLock);
}

static void StoreMenuItemNameForObject(IUnknown* pUnk, const std::wstring& itemName) {
    if (!pUnk) return;

    IContextMenu* pCm = nullptr;
    if (FAILED(pUnk->QueryInterface(IID_IContextMenu, reinterpret_cast<void**>(&pCm))) || !pCm) {
        return;
    }

    void* identity = static_cast<void*>(pCm);
    pCm->Release();

    AcquireSRWLockExclusive(&g_nameCacheLock);
    for (auto& entry : g_menuItemNames) {
        if (entry.cmIdentity == identity) {
            entry.itemName = itemName;
            ReleaseSRWLockExclusive(&g_nameCacheLock);
            return;
        }
    }

    if (g_menuItemNames.size() >= 32) {
        g_menuItemNames.erase(g_menuItemNames.begin());
    }
    g_menuItemNames.push_back({ identity, itemName });
    ReleaseSRWLockExclusive(&g_nameCacheLock);
}

static std::wstring LookupMenuItemName(IContextMenu* pThis) {
    if (!pThis) return std::wstring();

    void* identity = static_cast<void*>(pThis);
    AcquireSRWLockShared(&g_nameCacheLock);
    for (auto it = g_menuItemNames.rbegin(); it != g_menuItemNames.rend(); ++it) {
        if (it->cmIdentity == identity) {
            std::wstring found = it->itemName;
            ReleaseSRWLockShared(&g_nameCacheLock);
            return found;
        }
    }
    ReleaseSRWLockShared(&g_nameCacheLock);

    return std::wstring();
}

// ----------------------------------------------------------------------------
// IContextMenu::QueryContextMenu Hook (Installed via Windhawk Hook Engine)
// ----------------------------------------------------------------------------
using QueryContextMenu_t = HRESULT (STDMETHODCALLTYPE*)(
    IContextMenu* pThis,
    HMENU hmenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags
);
static QueryContextMenu_t QueryContextMenu_Original = nullptr;

static HRESULT STDMETHODCALLTYPE QueryContextMenu_Hook(
    IContextMenu* pThis,
    HMENU hmenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags
) {
    HRESULT hr = QueryContextMenu_Original(pThis, hmenu, indexMenu, idCmdFirst, idCmdLast, uFlags);
    if (FAILED(hr) || (uFlags & CMF_DEFAULTONLY) || !hmenu || !pThis) {
        return hr;
    }

    UINT dpi = GetContextMenuDpi();
    std::wstring itemName = LookupMenuItemName(pThis);

    int count = GetMenuItemCount(hmenu);
    for (int i = 0; i < count; ++i) {
        UINT id = GetMenuItemID(hmenu, i);
        if (id == static_cast<UINT>(-1) || id < idCmdFirst || id > idCmdLast) {
            continue;
        }

        WCHAR verb[64] = { 0 };
        if (FAILED(pThis->GetCommandString(id - idCmdFirst, GCS_VERBW, nullptr,
                                           reinterpret_cast<LPSTR>(verb), ARRAYSIZE(verb)))) {
            char verbA[64] = { 0 };
            if (FAILED(pThis->GetCommandString(id - idCmdFirst, GCS_VERBA, nullptr,
                                               verbA, ARRAYSIZE(verbA)))) {
                continue;
            }
            MultiByteToWideChar(CP_ACP, 0, verbA, -1, verb, ARRAYSIZE(verb));
        }

        size_t bindingIdx = FindBindingIndexByVerb(verb);
        if (bindingIdx == static_cast<size_t>(-1)) {
            continue;
        }

        bool enabled = false;
        std::wstring customTextCopy;
        {
            SharedConfigGuard lock;
            const ItemConfig& cfg = *g_itemBindings[bindingIdx].cfg;
            enabled = cfg.enabled;
            customTextCopy = cfg.customText;
        }

        if (!enabled) {
            continue;
        }

        HBITMAP hBitmap = GetOrCreateBitmapForBinding(bindingIdx, dpi);

        MENUITEMINFOW mii = { sizeof(mii) };
        std::wstring resolvedText;

        if (hBitmap) {
            mii.fMask |= MIIM_BITMAP;
            mii.hbmpItem = hBitmap;
        }

        if (!customTextCopy.empty()) {
            resolvedText = ResolveDynamicLabel(customTextCopy, itemName);
            if (!resolvedText.empty()) {
                mii.fMask |= MIIM_STRING;
                mii.dwTypeData = const_cast<LPWSTR>(resolvedText.c_str());
                mii.cch = static_cast<UINT>(resolvedText.length());
            }
        }

        if (mii.fMask != 0) {
            SetMenuItemInfoW(hmenu, i, TRUE, &mii);
        }
    }

    return hr;
}

// Creates a throwaway default context menu to resolve the shell's
// IContextMenu::QueryContextMenu implementation and hooks it via Windhawk.
static bool HookQueryContextMenu() {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    bool hooked = false;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_Windows, 0, nullptr, &pidl)) && pidl) {
        IShellFolder* parentFolder = nullptr;
        PCUITEMID_CHILD childPidl = nullptr;
        if (SUCCEEDED(SHBindToParent(pidl, IID_IShellFolder, reinterpret_cast<void**>(&parentFolder), &childPidl)) &&
            parentFolder && childPidl) {
            PCUITEMID_CHILD children[] = { childPidl };
            DEFCONTEXTMENU dcm = {};
            dcm.psf = parentFolder;
            dcm.cidl = 1;
            dcm.apidl = children;

            IContextMenu* menu = nullptr;
            tls_inSelfProbe = true;
            HRESULT hrMenu = SHCreateDefaultContextMenu(&dcm, IID_IContextMenu, reinterpret_cast<void**>(&menu));
            tls_inSelfProbe = false;

            if (SUCCEEDED(hrMenu) && menu) {
                void** vtable = *reinterpret_cast<void***>(menu);
                if (vtable && vtable[3]) {
                    hooked = WindhawkUtils::SetFunctionHook(
                        reinterpret_cast<QueryContextMenu_t>(vtable[3]),
                        QueryContextMenu_Hook,
                        &QueryContextMenu_Original
                    );
                }
                menu->Release();
            }
            parentFolder->Release();
        }
        CoTaskMemFree(pidl);
    }

    if (SUCCEEDED(hrInit)) CoUninitialize();
    return hooked;
}

// ----------------------------------------------------------------------------
// Shell Context Menu Creation Hooks
// ----------------------------------------------------------------------------
using SHCreateDefaultContextMenu_t = decltype(&SHCreateDefaultContextMenu);
static SHCreateDefaultContextMenu_t SHCreateDefaultContextMenu_Original = nullptr;

static HRESULT STDAPICALLTYPE SHCreateDefaultContextMenu_Hook(const DEFCONTEXTMENU* pdcm, REFIID riid, void** ppv) {
    HRESULT hr = SHCreateDefaultContextMenu_Original(pdcm, riid, ppv);
    if (!tls_inSelfProbe && SUCCEEDED(hr) && ppv && *ppv) {
        std::wstring extracted = pdcm
            ? GetItemNameFromShellFolder(pdcm->psf, pdcm->cidl, pdcm->apidl, pdcm->pidlFolder)
            : std::wstring();
        StoreMenuItemNameForObject(static_cast<IUnknown*>(*ppv), extracted);
    }
    return hr;
}

using CDefFolderMenu_Create2_t = decltype(&CDefFolderMenu_Create2);
static CDefFolderMenu_Create2_t CDefFolderMenu_Create2_Original = nullptr;

static HRESULT STDAPICALLTYPE CDefFolderMenu_Create2_Hook(
    PCIDLIST_ABSOLUTE pidlFolder,
    HWND hwnd,
    UINT cidl,
    PCUITEMID_CHILD_ARRAY apidl,
    IShellFolder* psf,
    LPFNDFMCALLBACK pfn,
    UINT nKeys,
    const HKEY* ahkeys,
    IContextMenu** ppcm
) {
    HRESULT hr = CDefFolderMenu_Create2_Original(pidlFolder, hwnd, cidl, apidl, psf, pfn, nKeys, ahkeys, ppcm);
    if (!tls_inSelfProbe && SUCCEEDED(hr) && ppcm && *ppcm) {
        std::wstring extracted = GetItemNameFromShellFolder(psf, cidl, apidl, pidlFolder);
        StoreMenuItemNameForObject(*ppcm, extracted);
    }
    return hr;
}

// ----------------------------------------------------------------------------
// Windhawk Module Lifecycle Handlers
// ----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadAllSettings();

    if (!WindhawkUtils::SetFunctionHook(
            SHCreateDefaultContextMenu,
            SHCreateDefaultContextMenu_Hook,
            &SHCreateDefaultContextMenu_Original)) {
        CleanupAllResources();
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(
        CDefFolderMenu_Create2,
        CDefFolderMenu_Create2_Hook,
        &CDefFolderMenu_Create2_Original
    );

    g_queryContextMenuHooked = HookQueryContextMenu();
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_queryContextMenuHooked) {
        if (HookQueryContextMenu()) {
            g_queryContextMenuHooked = true;
            Wh_ApplyHookOperations();
        }
    }
}

void Wh_ModUninit() {
    CleanupAllResources();
}

void Wh_ModSettingsChanged() {
    LoadAllSettings();
}
