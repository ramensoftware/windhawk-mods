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

### Supported Context Menu Items:
1. **Open** (`open`) - Top bold default action (default icon: `imageres.dll,-185`, default label: `Open: %file%`)
2. **Run as administrator** (`runas`) - UAC elevation action (keeps native UAC shield and default label unless customized)
3. **Delete** (`delete`) - File and folder deletion action (default icon: Full Recycle Bin `imageres.dll,-54`)
4. **Rename** (`rename`) - Inline rename action (default icon: Clipboard `shell32.dll,-16763`)
5. **Properties** (`properties`) - Bottom item details action (default icon: `imageres.dll,-114`)

### Dynamic `%file%` Placeholder:
Include `%file%` (or `%name%`) in any custom label field to dynamically display the right-clicked file, folder, or shortcut target name. Names longer than 30 characters are automatically shortened to the first 30 characters followed by `...`:
- `Open: %file%` -> **Open: New folder (2)**
- `Delete %file%` -> **Delete Very_Long_File_Name_Over_Thirt...**
- `%file% Properties` -> **report.pdf Properties**

### Highlights:
- **Canonical Verb Matching:** Identifies core shell commands via `IContextMenu::GetCommandString` (`GCS_VERBW`), working across every Windows display language without touching third-party shell extension items.
- **Zero Layout Distortion:** Attaches icons cleanly during `IContextMenu::QueryContextMenu` so native row height, padding, and bold text remain completely untouched.
- **Clear-to-Reset Behavior:** Clearing an icon path removes the icon for Open, Delete, Rename, or Properties, while Run as administrator reverts to the native Windows UAC shield.
- **Custom Icon Support:** Easily add your own `.ico` files or use any built-in Windows icon with `dll,index` syntax.
- **OS-Friendly & Lightweight:** Caches converted 32-bpp ARGB bitmaps in memory with zero GDI handle leaks and automatic DPI scaling support.
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
- openCustomText: "Open: %file%"
  $name: "Open Custom Label (Supports %file%)"
  $description: "Default is Open: %file%. Leave empty for default system name, or enter custom text (%file% is truncated after 30 chars)."

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
  $description: "Leave empty for default name, or enter custom text. Use %file% for clicked item name (truncated after 30 chars)."

- enableRenameIcon: true
  $name: "4. Enable Rename Customization"
  $description: "Enable icon and/or custom label for the Rename option."
- renameIconPath: "shell32.dll,-16763"
  $name: "Rename Icon Path or DLL Index"
  $description: "Default is Clipboard (shell32.dll,-16763). Leave empty for no icon, or enter a .ico path or DLL index."
- renameCustomText: ""
  $name: "Rename Custom Label (Supports %file%)"
  $description: "Leave empty for default name, or enter custom text. Use %file% for clicked item name (truncated after 30 chars)."

- enablePropertiesIcon: true
  $name: "5. Enable Properties Customization"
  $description: "Enable icon and/or custom label for the bottom Properties option."
- propertiesIconPath: "imageres.dll,-114"
  $name: "Properties Icon Path or DLL Index"
  $description: "Default is imageres.dll,-114. Leave empty for no icon, or enter a .ico path or DLL index."
- propertiesCustomText: ""
  $name: "Properties Custom Label (Supports %file%)"
  $description: "Leave empty for default name, or enter custom text. Use %file% for clicked item name (truncated after 30 chars)."
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
struct ItemConfig {
    bool enabled = true;
    HBITMAP hBitmap = nullptr;
    std::wstring customText;
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
static int g_cachedIconCx = 0;
static int g_cachedIconCy = 0;
static thread_local WCHAR tls_currentItemName[64] = { 0 };

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

    // Clean up extra spaces and trailing colon when %file% is empty
    if (safeItemName.empty()) {
        size_t dbl = 0;
        while ((dbl = result.find(L"  ", dbl)) != std::wstring::npos) {
            result.erase(dbl, 1);
        }
        while (!result.empty() && result.front() == L' ') result.erase(0, 1);
        while (!result.empty() && (result.back() == L' ' || result.back() == L':')) result.pop_back();
    }

    return result;
}

static const ItemConfig* FindConfigByVerb(LPCWSTR verb) {
    if (!verb || verb[0] == L'\0') return nullptr;

    for (const auto& binding : g_itemBindings) {
        if (_wcsicmp(verb, binding.canonicalVerb) == 0) {
            const ItemConfig& cfg = *binding.cfg;
            if (cfg.enabled && (cfg.hBitmap || !cfg.customText.empty())) {
                return &cfg;
            }
            return nullptr;
        }
    }

    return nullptr;
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

// ----------------------------------------------------------------------------
// Cleanup & Settings Loading
// ----------------------------------------------------------------------------
static void CleanupSingleConfig(ItemConfig& cfg) {
    if (cfg.hBitmap) {
        DeleteObject(cfg.hBitmap);
        cfg.hBitmap = nullptr;
    }
}

static void CleanupAllResources() {
    ExclusiveConfigGuard lock;
    for (const auto& binding : g_itemBindings) {
        CleanupSingleConfig(*binding.cfg);
    }
    g_cachedIconCx = 0;
    g_cachedIconCy = 0;
}

static void LoadAllSettings() {
    int cx = GetSystemMetrics(SM_CXSMICON);
    int cy = GetSystemMetrics(SM_CYSMICON);
    if (cx <= 0) cx = 16;
    if (cy <= 0) cy = 16;

    constexpr size_t kBindingCount = ARRAYSIZE(g_itemBindings);
    ItemConfig newConfigs[kBindingCount];

    for (size_t i = 0; i < kBindingCount; ++i) {
        const auto& binding = g_itemBindings[i];
        newConfigs[i].enabled = Wh_GetIntSetting(binding.enableSetting) != 0;

        auto iconSetting = WindhawkUtils::StringSetting::make(binding.iconSetting);
        if (newConfigs[i].enabled && iconSetting.get()[0] != L'\0') {
            HICON hIcon = LoadIconUniversal(iconSetting.get(), cx, cy);
            if (hIcon) {
                newConfigs[i].hBitmap = IconToPremultipliedBitmap(hIcon, cx, cy);
                DestroyIcon(hIcon);
            }
        }

        auto textSetting = WindhawkUtils::StringSetting::make(binding.textSetting);
        if (textSetting.get()[0] != L'\0') {
            newConfigs[i].customText = textSetting.get();
        }
    }

    {
        ExclusiveConfigGuard lock;
        for (size_t i = 0; i < kBindingCount; ++i) {
            std::swap(*g_itemBindings[i].cfg, newConfigs[i]);
        }
        g_cachedIconCx = cx;
        g_cachedIconCy = cy;
    }

    for (size_t i = 0; i < kBindingCount; ++i) {
        CleanupSingleConfig(newConfigs[i]);
    }
}

static void EnsureIconsMatchCurrentDpi() {
    int cx = GetSystemMetrics(SM_CXSMICON);
    int cy = GetSystemMetrics(SM_CYSMICON);
    if (cx <= 0) cx = 16;
    if (cy <= 0) cy = 16;

    {
        SharedConfigGuard lock;
        if (cx == g_cachedIconCx && cy == g_cachedIconCy) {
            return;
        }
    }

    LoadAllSettings();
}

// ----------------------------------------------------------------------------
// IContextMenu::QueryContextMenu Vtable Hook (Canonical Verb Identification)
// ----------------------------------------------------------------------------
using QueryContextMenu_t = HRESULT (STDMETHODCALLTYPE*)(
    IContextMenu* pThis,
    HMENU hmenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags
);

struct HookedVtable {
    void** vtable;
    QueryContextMenu_t original;
};

struct MenuItemNameEntry {
    void* cmIdentity;
    std::wstring itemName;
};

static SRWLOCK g_vtableLock = SRWLOCK_INIT;
static std::vector<HookedVtable> g_hookedVtables;
static std::vector<MenuItemNameEntry> g_menuItemNames;

static void* GetCanonicalIContextMenuIdentity(IUnknown* pUnk) {
    if (!pUnk) return nullptr;
    IContextMenu* pCm = nullptr;
    if (SUCCEEDED(pUnk->QueryInterface(IID_IContextMenu, reinterpret_cast<void**>(&pCm))) && pCm) {
        void* identity = pCm;
        pCm->Release();
        return identity;
    }
    return pUnk;
}

static void StoreMenuItemName(void* cmIdentity, const std::wstring& itemName) {
    if (!cmIdentity) return;

    AcquireSRWLockExclusive(&g_vtableLock);
    for (auto& entry : g_menuItemNames) {
        if (entry.cmIdentity == cmIdentity) {
            entry.itemName = itemName;
            ReleaseSRWLockExclusive(&g_vtableLock);
            return;
        }
    }

    if (g_menuItemNames.size() >= 32) {
        g_menuItemNames.erase(g_menuItemNames.begin());
    }
    g_menuItemNames.push_back({ cmIdentity, itemName });
    ReleaseSRWLockExclusive(&g_vtableLock);
}

static std::wstring LookupMenuItemName(IContextMenu* pThis) {
    void* cmIdentity = GetCanonicalIContextMenuIdentity(pThis);

    AcquireSRWLockShared(&g_vtableLock);
    for (auto it = g_menuItemNames.rbegin(); it != g_menuItemNames.rend(); ++it) {
        if (it->cmIdentity == cmIdentity || it->cmIdentity == static_cast<void*>(pThis)) {
            std::wstring found = it->itemName;
            ReleaseSRWLockShared(&g_vtableLock);
            return found;
        }
    }
    ReleaseSRWLockShared(&g_vtableLock);

    return tls_currentItemName;
}

static HRESULT STDMETHODCALLTYPE QueryContextMenu_Hook(
    IContextMenu* pThis,
    HMENU hmenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags
) {
    QueryContextMenu_t original = nullptr;
    if (pThis) {
        void** vtable = *reinterpret_cast<void***>(pThis);
        AcquireSRWLockShared(&g_vtableLock);
        for (const auto& entry : g_hookedVtables) {
            if (entry.vtable == vtable) {
                original = entry.original;
                break;
            }
        }
        ReleaseSRWLockShared(&g_vtableLock);
    }

    if (!original) {
        return E_UNEXPECTED;
    }

    HRESULT hr = original(pThis, hmenu, indexMenu, idCmdFirst, idCmdLast, uFlags);
    if (FAILED(hr) || (uFlags & CMF_DEFAULTONLY) || !hmenu) {
        return hr;
    }

    EnsureIconsMatchCurrentDpi();
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

        SharedConfigGuard lock;
        const ItemConfig* cfg = FindConfigByVerb(verb);
        if (!cfg) {
            continue;
        }

        MENUITEMINFOW mii = { sizeof(mii) };
        std::wstring resolvedText;

        if (cfg->hBitmap) {
            mii.fMask |= MIIM_BITMAP;
            mii.hbmpItem = cfg->hBitmap;
        }

        if (!cfg->customText.empty()) {
            resolvedText = ResolveDynamicLabel(cfg->customText, itemName);
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

static void HookContextMenuVtable(IUnknown* pUnk, const std::wstring& itemName) {
    if (!pUnk) return;

    IContextMenu* pCm = nullptr;
    if (FAILED(pUnk->QueryInterface(IID_IContextMenu, reinterpret_cast<void**>(&pCm))) || !pCm) {
        return;
    }

    StoreMenuItemName(pCm, itemName);

    void** vtable = *reinterpret_cast<void***>(pCm);
    if (vtable && vtable[3] != reinterpret_cast<void*>(QueryContextMenu_Hook)) {
        AcquireSRWLockExclusive(&g_vtableLock);
        bool alreadyHooked = false;
        for (const auto& entry : g_hookedVtables) {
            if (entry.vtable == vtable) {
                alreadyHooked = true;
                break;
            }
        }

        if (!alreadyHooked && vtable[3] != reinterpret_cast<void*>(QueryContextMenu_Hook)) {
            DWORD oldProtect = 0;
            if (VirtualProtect(&vtable[3], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                QueryContextMenu_t orig = reinterpret_cast<QueryContextMenu_t>(vtable[3]);
                vtable[3] = reinterpret_cast<void*>(QueryContextMenu_Hook);
                DWORD dummy = 0;
                VirtualProtect(&vtable[3], sizeof(void*), oldProtect, &dummy);
                g_hookedVtables.push_back({ vtable, orig });
            }
        }
        ReleaseSRWLockExclusive(&g_vtableLock);
    }

    pCm->Release();
}

static void UnhookAllContextMenuVtables() {
    AcquireSRWLockExclusive(&g_vtableLock);
    for (const auto& entry : g_hookedVtables) {
        if (entry.vtable && entry.vtable[3] == reinterpret_cast<void*>(QueryContextMenu_Hook)) {
            DWORD oldProtect = 0;
            if (VirtualProtect(&entry.vtable[3], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                entry.vtable[3] = reinterpret_cast<void*>(entry.original);
                DWORD dummy = 0;
                VirtualProtect(&entry.vtable[3], sizeof(void*), oldProtect, &dummy);
            }
        }
    }
    g_hookedVtables.clear();
    g_menuItemNames.clear();
    ReleaseSRWLockExclusive(&g_vtableLock);
}

// ----------------------------------------------------------------------------
// Shell Context Menu Creation Hooks
// ----------------------------------------------------------------------------
using SHCreateDefaultContextMenu_t = decltype(&SHCreateDefaultContextMenu);
static SHCreateDefaultContextMenu_t SHCreateDefaultContextMenu_Original = nullptr;

static HRESULT STDAPICALLTYPE SHCreateDefaultContextMenu_Hook(const DEFCONTEXTMENU* pdcm, REFIID riid, void** ppv) {
    std::wstring extracted = pdcm
        ? GetItemNameFromShellFolder(pdcm->psf, pdcm->cidl, pdcm->apidl, pdcm->pidlFolder)
        : std::wstring();
    wcsncpy_s(tls_currentItemName, extracted.c_str(), _TRUNCATE);

    HRESULT hr = SHCreateDefaultContextMenu_Original(pdcm, riid, ppv);
    if (SUCCEEDED(hr) && ppv && *ppv) {
        HookContextMenuVtable(static_cast<IUnknown*>(*ppv), extracted);
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
    std::wstring extracted = GetItemNameFromShellFolder(psf, cidl, apidl, pidlFolder);
    wcsncpy_s(tls_currentItemName, extracted.c_str(), _TRUNCATE);

    HRESULT hr = CDefFolderMenu_Create2_Original(pidlFolder, hwnd, cidl, apidl, psf, pfn, nKeys, ahkeys, ppcm);
    if (SUCCEEDED(hr) && ppcm && *ppcm) {
        HookContextMenuVtable(*ppcm, extracted);
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
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(
        CDefFolderMenu_Create2,
        CDefFolderMenu_Create2_Hook,
        &CDefFolderMenu_Create2_Original
    );

    return TRUE;
}

void Wh_ModUninit() {
    UnhookAllContextMenuVtables();
    CleanupAllResources();
}

void Wh_ModSettingsChanged() {
    LoadAllSettings();
}
