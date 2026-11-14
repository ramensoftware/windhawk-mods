// ==WindhawkMod==
// @id              explorer-core-context-menu-customizer
// @name            Explorer Core System Menu Customization
// @description     Adds customizable icons and dynamic %file% labels to Open, Run as administrator, Delete, Rename, and Properties in the Windows File Explorer context menu without altering row spacing.
// @version         1.0.2
// @author          akiyorm
// @github          https://github.com/akiyorm
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -lshlwapi -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Context Menu Icons & Dynamic Labels

Windows does not provide any registry entries or built-in options to customize the icons or labels of core File Explorer context menu commands. This mod was created to fill that gap cleanly and safely.

### Supported Context Menu Items:
1. **Open** - Top bold default action (default icon: `imageres.dll,-185`, default label: `Open: %file%`)
2. **Run as administrator** - UAC elevation action (keeps native UAC shield and default label unless customized)
3. **Delete** - File and folder deletion action (default icon: Full Recycle Bin `imageres.dll,-54`)
4. **Rename** - Inline rename action (default icon: Clipboard `shell32.dll,-16763`)
5. **Properties** - Bottom item details action (default icon: `imageres.dll,-114`)

### Dynamic `%file%` Placeholder:
Include `%file%` (or `%name%`) in any custom label field to dynamically display the right-clicked file or folder name. Names longer than 30 characters are automatically shortened to the first 30 characters followed by `...`:
- `Open: %file%` -> **Open: New folder (2)**
- `Delete %file%` -> **Delete Very_Long_File_Name_Over_Thirt...**
- `%file% Properties` -> **report.pdf Properties**

### Highlights:
- **Zero Layout Distortion:** Clean, lightweight hooks attach icons before menu rendering so native row height, padding, and bold text remain completely untouched.
- **Clear-to-Reset Behavior:** Clearing an icon path removes the icon for Open, Delete, Rename, or Properties, while Run as administrator reverts to the native Windows UAC shield.
- **Custom Icon Support:** Easily add your own `.ico` files or use any built-in Windows icon with `dll,index` syntax.
- **OS-Friendly & Lightweight:** Caches converted icons in memory with zero GDI handle leaks and minimal resource overhead.
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
  $description: "Default is Open: %file%. Leave empty for default system name, or enter custom text (truncated after 30 chars)."

- enableRunAsIcon: true
  $name: "2. Enable Run as administrator Customization"
  $description: "Enable custom icon and/or custom label for Run as administrator."
- runAsIconPath: ""
  $name: "Run as administrator Icon Path or DLL Index"
  $description: "Leave empty to keep the default Windows UAC shield, or enter a .ico path or DLL index (e.g. shell32.dll,44)."
- runAsCustomText: ""
  $name: "Run as administrator Custom Label (Supports %file%)"
  $description: "Leave empty to keep default system name untouched, or enter custom text using %file%."

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
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Global State, Synchronization & Thread-Local Context Menu Item Name
// ----------------------------------------------------------------------------
enum class TargetItemType {
    None,
    Open,
    RunAsAdmin,
    Delete,
    Rename,
    Properties
};

struct ItemConfig {
    bool enabled = true;
    HICON hIcon = nullptr;
    HBITMAP hBitmap = nullptr;
    std::wstring customText;
    std::wstring systemLocalizedText;
};

struct ItemBinding {
    TargetItemType type;
    ItemConfig* cfg;
    PCWSTR enableSetting;
    PCWSTR iconSetting;
    PCWSTR textSetting;
    LPCWSTR canonicalEn;
};

static ItemConfig g_openCfg;
static ItemConfig g_runAsCfg;
static ItemConfig g_deleteCfg;
static ItemConfig g_renameCfg;
static ItemConfig g_propsCfg;

static const ItemBinding g_itemBindings[] = {
    { TargetItemType::Open,       &g_openCfg,   L"enableOpenIcon",       L"openIconPath",       L"openCustomText",       L"Open" },
    { TargetItemType::RunAsAdmin, &g_runAsCfg,  L"enableRunAsIcon",      L"runAsIconPath",      L"runAsCustomText",      L"Run as administrator" },
    { TargetItemType::Delete,     &g_deleteCfg, L"enableDeleteIcon",     L"deleteIconPath",     L"deleteCustomText",     L"Delete" },
    { TargetItemType::Rename,     &g_renameCfg, L"enableRenameIcon",     L"renameIconPath",     L"renameCustomText",     L"Rename" },
    { TargetItemType::Properties, &g_propsCfg,  L"enablePropertiesIcon", L"propertiesIconPath", L"propertiesCustomText", L"Properties" },
};

static SRWLOCK g_configLock = SRWLOCK_INIT;
static thread_local std::wstring tls_currentItemName;

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
// String Helpers: Truncation, Mnemonic Cleaning, Escaping & %file% Expansion
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

static std::wstring CleanMenuText(LPCWSTR text) {
    std::wstring result;
    if (!text) return result;

    for (const wchar_t* p = text; *p; ++p) {
        if (*p != L'&') {
            result.push_back(*p);
        }
    }

    // Strip trailing keyboard accelerators such as "(R)" or "(D)"
    size_t paren = result.rfind(L'(');
    if (paren != std::wstring::npos && !result.empty() && result.back() == L')') {
        result.erase(paren);
    }

    while (!result.empty() && (result.back() == L' ' || result.back() == L':')) {
        result.pop_back();
    }

    return result;
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

static bool HasDynamicPlaceholder(const std::wstring& tpl) {
    if (tpl.empty()) return false;
    std::wstring lower;
    lower.reserve(tpl.size());
    for (wchar_t ch : tpl) {
        lower.push_back(static_cast<wchar_t>(towlower(ch)));
    }
    return lower.find(L"%file%") != std::wstring::npos ||
           lower.find(L"%name%") != std::wstring::npos;
}

static std::wstring ResolveDynamicLabel(const std::wstring& tpl) {
    if (tpl.empty()) return std::wstring();

    std::wstring result = tpl;
    std::wstring safeItemName = EscapeMenuMnemonics(tls_currentItemName);

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

// ----------------------------------------------------------------------------
// Target Menu Item Identification & Application
// ----------------------------------------------------------------------------
static bool MatchesConfigText(const std::wstring& cleanIncoming, const ItemConfig& cfg, LPCWSTR canonicalEn) {
    if (!cfg.customText.empty()) {
        std::wstring cleanTpl = CleanMenuText(cfg.customText.c_str());
        if (_wcsicmp(cleanIncoming.c_str(), cleanTpl.c_str()) == 0) {
            return true;
        }
        if (HasDynamicPlaceholder(cfg.customText)) {
            std::wstring resolved = CleanMenuText(ResolveDynamicLabel(cfg.customText).c_str());
            if (!resolved.empty() && _wcsicmp(cleanIncoming.c_str(), resolved.c_str()) == 0) {
                return true;
            }
        }
    }

    if (canonicalEn && _wcsicmp(cleanIncoming.c_str(), canonicalEn) == 0) {
        return true;
    }

    if (!cfg.systemLocalizedText.empty() &&
        _wcsicmp(cleanIncoming.c_str(), cfg.systemLocalizedText.c_str()) == 0) {
        return true;
    }

    return false;
}

static const ItemConfig* FindMatchingConfig(LPCWSTR rawText) {
    if (!rawText) return nullptr;

    std::wstring cleanText = CleanMenuText(rawText);
    if (cleanText.empty()) return nullptr;

    for (const auto& binding : g_itemBindings) {
        const ItemConfig& cfg = *binding.cfg;
        if (cfg.enabled && (cfg.hBitmap || !cfg.customText.empty())) {
            if (MatchesConfigText(cleanText, cfg, binding.canonicalEn)) {
                return &cfg;
            }
        }
    }

    return nullptr;
}

static const ItemConfig* FindMatchingMenuItem(LPCMENUITEMINFOW lpmi) {
    if (!lpmi || lpmi->hSubMenu != nullptr) return nullptr;

    LPCWSTR rawText = nullptr;
    if ((lpmi->fMask & MIIM_STRING) && lpmi->dwTypeData) {
        rawText = lpmi->dwTypeData;
    } else if ((lpmi->fMask & MIIM_TYPE) &&
               !(lpmi->fType & (MFT_BITMAP | MFT_SEPARATOR | MFT_OWNERDRAW)) &&
               lpmi->dwTypeData) {
        rawText = lpmi->dwTypeData;
    }

    return FindMatchingConfig(rawText);
}

static void ApplyItemCustomization(MENUITEMINFOW& mii, std::wstring& outResolvedText, const ItemConfig& cfg) {
    if (cfg.hBitmap) {
        mii.fMask |= MIIM_BITMAP;
        mii.hbmpItem = cfg.hBitmap;
    }

    if (!cfg.customText.empty()) {
        outResolvedText = ResolveDynamicLabel(cfg.customText);
        if (!outResolvedText.empty()) {
            mii.fMask |= MIIM_STRING;
            mii.dwTypeData = const_cast<LPWSTR>(outResolvedText.c_str());
            mii.cch = static_cast<UINT>(outResolvedText.length());
        }
    }
}

// ----------------------------------------------------------------------------
// Extract Clicked Item Display Name from Shell Context Menu Parameters
// ----------------------------------------------------------------------------
static std::wstring GetItemNameFromShellFolder(IShellFolder* psf, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, PCIDLIST_ABSOLUTE pidlFolder) {
    if (cidl > 0 && psf && apidl && apidl[0]) {
        STRRET strret = {};
        if (SUCCEEDED(psf->GetDisplayNameOf(apidl[0], SHGDN_INFOLDER, &strret))) {
            WCHAR szName[MAX_PATH] = { 0 };
            if (SUCCEEDED(StrRetToBufW(&strret, apidl[0], szName, ARRAYSIZE(szName))) && szName[0] != L'\0') {
                std::wstring name = TruncateFileName(szName, 30);
                if (cidl > 1) {
                    name += L" (+" + std::to_wstring(cidl - 1) + L")";
                }
                return name;
            }
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

    if (hasAlpha) {
        for (int i = 0; i < totalPixels; ++i) {
            DWORD px = pixels[i];
            BYTE a = (px >> 24) & 0xFF;
            if (a == 0) {
                pixels[i] = 0;
            } else if (a < 255) {
                BYTE r = static_cast<BYTE>(((px >> 16) & 0xFF) * a / 255);
                BYTE g = static_cast<BYTE>(((px >> 8) & 0xFF) * a / 255);
                BYTE b = static_cast<BYTE>((px & 0xFF) * a / 255);
                pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    } else if (ii.hbmMask) {
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
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        using PrivateExtractIconsW_t = UINT (WINAPI *)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
        auto pfnPrivateExtractIconsW = (PrivateExtractIconsW_t)GetProcAddress(hUser32, "PrivateExtractIconsW");
        if (pfnPrivateExtractIconsW) {
            UINT iconId = 0;
            pfnPrivateExtractIconsW(path.c_str(), index, cx, cy, &hIcon, &iconId, 1, LR_LOADFROMFILE);
        }
    }

    if (!hIcon) {
        ExtractIconExW(path.c_str(), index, nullptr, &hIcon, 1);
    }

    return hIcon;
}

// ----------------------------------------------------------------------------
// Localized System String Helpers
// ----------------------------------------------------------------------------
static std::wstring LoadShellString(HMODULE hShell32, UINT resourceId) {
    if (!hShell32) return std::wstring();
    WCHAR szBuffer[128] = { 0 };
    if (LoadStringW(hShell32, resourceId, szBuffer, ARRAYSIZE(szBuffer)) > 0) {
        return CleanMenuText(szBuffer);
    }
    return std::wstring();
}

static std::wstring ResolveRegistryVerbName(HKEY hKeyRoot, LPCWSTR subKey, LPCWSTR valueName) {
    WCHAR szRegIndirect[128] = { 0 };
    DWORD cbSize = sizeof(szRegIndirect);
    if (RegGetValueW(
            hKeyRoot,
            subKey,
            valueName,
            RRF_RT_REG_SZ,
            nullptr,
            szRegIndirect,
            &cbSize
        ) == ERROR_SUCCESS && szRegIndirect[0] != L'\0') {
        WCHAR szResolved[128] = { 0 };
        if (SUCCEEDED(SHLoadIndirectString(szRegIndirect, szResolved, ARRAYSIZE(szResolved), nullptr))) {
            return CleanMenuText(szResolved);
        }
        return CleanMenuText(szRegIndirect);
    }
    return std::wstring();
}

// ----------------------------------------------------------------------------
// Cleanup & Load All Configured Icons
// ----------------------------------------------------------------------------
static void CleanupSingleConfig(ItemConfig& cfg) {
    if (cfg.hBitmap) {
        DeleteObject(cfg.hBitmap);
        cfg.hBitmap = nullptr;
    }
    if (cfg.hIcon) {
        DestroyIcon(cfg.hIcon);
        cfg.hIcon = nullptr;
    }
}

static void CleanupAllResources() {
    ExclusiveConfigGuard lock;
    for (const auto& binding : g_itemBindings) {
        CleanupSingleConfig(*binding.cfg);
    }
}

static void LoadAllSettings() {
    ExclusiveConfigGuard lock;

    int cx = GetSystemMetrics(SM_CXSMICON);
    int cy = GetSystemMetrics(SM_CYSMICON);
    if (cx <= 0) cx = 16;
    if (cy <= 0) cy = 16;

    for (const auto& binding : g_itemBindings) {
        ItemConfig& cfg = *binding.cfg;
        CleanupSingleConfig(cfg);

        cfg.enabled = Wh_GetIntSetting(binding.enableSetting) != 0;

        PCWSTR iconSetting = Wh_GetStringSetting(binding.iconSetting);
        if (iconSetting && iconSetting[0] != L'\0') {
            cfg.hIcon = LoadIconUniversal(iconSetting, cx, cy);
            if (cfg.hIcon) {
                cfg.hBitmap = IconToPremultipliedBitmap(cfg.hIcon, cx, cy);
            }
        }
        Wh_FreeStringSetting(iconSetting);

        PCWSTR textSetting = Wh_GetStringSetting(binding.textSetting);
        if (textSetting && textSetting[0] != L'\0') {
            cfg.customText = textSetting;
        } else {
            cfg.customText.clear();
        }
        Wh_FreeStringSetting(textSetting);
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    g_openCfg.systemLocalizedText = LoadShellString(hShell32, 8496);
    g_propsCfg.systemLocalizedText = LoadShellString(hShell32, 12849);

    g_runAsCfg.systemLocalizedText = ResolveRegistryVerbName(
        HKEY_CLASSES_ROOT,
        L"exefile\\shell\\runas",
        nullptr
    );
    if (g_runAsCfg.systemLocalizedText.empty()) {
        g_runAsCfg.systemLocalizedText = LoadShellString(hShell32, 37417);
    }

    g_deleteCfg.systemLocalizedText = ResolveRegistryVerbName(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CommandStore\\shell\\Windows.delete",
        L"MUIVerb"
    );
    if (g_deleteCfg.systemLocalizedText.empty()) {
        g_deleteCfg.systemLocalizedText = LoadShellString(hShell32, 31271);
    }

    g_renameCfg.systemLocalizedText = ResolveRegistryVerbName(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CommandStore\\shell\\Windows.rename",
        L"MUIVerb"
    );
    if (g_renameCfg.systemLocalizedText.empty()) {
        g_renameCfg.systemLocalizedText = LoadShellString(hShell32, 31272);
    }
}

// ----------------------------------------------------------------------------
// Shell Context Menu Hooks (Capture Clicked File / Folder Name)
// ----------------------------------------------------------------------------
using SHCreateDefaultContextMenu_t = decltype(&SHCreateDefaultContextMenu);
static SHCreateDefaultContextMenu_t SHCreateDefaultContextMenu_Original = nullptr;

static HRESULT STDAPICALLTYPE SHCreateDefaultContextMenu_Hook(const DEFCONTEXTMENU* pdcm, REFIID riid, void** ppv) {
    if (pdcm) {
        std::wstring extracted = GetItemNameFromShellFolder(pdcm->psf, pdcm->cidl, pdcm->apidl, pdcm->pidlFolder);
        if (!extracted.empty()) {
            tls_currentItemName = std::move(extracted);
        }
    }
    return SHCreateDefaultContextMenu_Original(pdcm, riid, ppv);
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
    if (!extracted.empty()) {
        tls_currentItemName = std::move(extracted);
    }
    return CDefFolderMenu_Create2_Original(pidlFolder, hwnd, cidl, apidl, psf, pfn, nKeys, ahkeys, ppcm);
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original = nullptr;

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm) {
    BOOL res = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hwnd, lptpm);
    tls_currentItemName.clear();
    return res;
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
static TrackPopupMenu_t TrackPopupMenu_Original = nullptr;

static BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect) {
    BOOL res = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    tls_currentItemName.clear();
    return res;
}

// ----------------------------------------------------------------------------
// Menu Insertion & Modification Hooks
// ----------------------------------------------------------------------------
using InsertMenuItemW_t = decltype(&InsertMenuItemW);
static InsertMenuItemW_t InsertMenuItemW_Original = nullptr;

static BOOL WINAPI InsertMenuItemW_Hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi) {
    if (lpmi && (!(lpmi->fMask & MIIM_FTYPE) || !(lpmi->fType & MFT_OWNERDRAW))) {
        SharedConfigGuard lock;
        const ItemConfig* cfg = FindMatchingMenuItem(lpmi);
        if (cfg) {
            MENUITEMINFOW mii = *lpmi;
            std::wstring resolvedText;
            ApplyItemCustomization(mii, resolvedText, *cfg);
            return InsertMenuItemW_Original(hMenu, item, fByPosition, &mii);
        }
    }
    return InsertMenuItemW_Original(hMenu, item, fByPosition, lpmi);
}

using InsertMenuW_t = decltype(&InsertMenuW);
static InsertMenuW_t InsertMenuW_Original = nullptr;

static BOOL WINAPI InsertMenuW_Hook(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) {
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR | MF_POPUP)) && lpNewItem) {
        SharedConfigGuard lock;
        const ItemConfig* cfg = FindMatchingConfig(lpNewItem);
        if (cfg) {
            MENUITEMINFOW mii = { sizeof(mii) };
            mii.fMask = MIIM_ID | MIIM_STATE | MIIM_FTYPE | MIIM_STRING;
            mii.fType = MFT_STRING;
            mii.fState = (uFlags & MF_GRAYED) ? MFS_GRAYED : ((uFlags & MF_DISABLED) ? MFS_DISABLED : MFS_ENABLED);
            if (uFlags & MF_CHECKED) {
                mii.fState |= MFS_CHECKED;
            }
            mii.wID = static_cast<UINT>(uIDNewItem);
            mii.dwTypeData = const_cast<LPWSTR>(lpNewItem);

            std::wstring resolvedText;
            ApplyItemCustomization(mii, resolvedText, *cfg);
            BOOL fByPosition = (uFlags & MF_BYPOSITION) != 0;
            return InsertMenuItemW_Original(hMenu, uPosition, fByPosition, &mii);
        }
    }
    return InsertMenuW_Original(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);
}

using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
static SetMenuItemInfoW_t SetMenuItemInfoW_Original = nullptr;

static BOOL WINAPI SetMenuItemInfoW_Hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi) {
    if (lpmi && (lpmi->fMask & (MIIM_BITMAP | MIIM_STRING | MIIM_TYPE))) {
        if (!(lpmi->fMask & MIIM_FTYPE) || !(lpmi->fType & MFT_OWNERDRAW)) {
            WCHAR szExisting[256] = { 0 };
            MENUITEMINFOW query = { sizeof(query) };
            query.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
            query.dwTypeData = szExisting;
            query.cch = ARRAYSIZE(szExisting);

            if (GetMenuItemInfoW(hMenu, item, fByPosition, &query) &&
                !(query.fType & MFT_OWNERDRAW) &&
                query.hSubMenu == nullptr) {
                LPCWSTR textToCheck = szExisting;
                if ((lpmi->fMask & MIIM_STRING) && lpmi->dwTypeData) {
                    textToCheck = lpmi->dwTypeData;
                } else if ((lpmi->fMask & MIIM_TYPE) &&
                           !(lpmi->fType & (MFT_BITMAP | MFT_SEPARATOR | MFT_OWNERDRAW)) &&
                           lpmi->dwTypeData) {
                    textToCheck = lpmi->dwTypeData;
                }

                SharedConfigGuard lock;
                const ItemConfig* cfg = FindMatchingConfig(textToCheck);
                if (cfg) {
                    MENUITEMINFOW mii = *lpmi;
                    std::wstring resolvedText;
                    ApplyItemCustomization(mii, resolvedText, *cfg);
                    return SetMenuItemInfoW_Original(hMenu, item, fByPosition, &mii);
                }
            }
        }
    }
    return SetMenuItemInfoW_Original(hMenu, item, fByPosition, lpmi);
}

// ----------------------------------------------------------------------------
// Windhawk Module Lifecycle Handlers
// ----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadAllSettings();

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        return FALSE;
    }

    void* pInsertMenuItemW = (void*)GetProcAddress(hUser32, "InsertMenuItemW");
    void* pInsertMenuW = (void*)GetProcAddress(hUser32, "InsertMenuW");
    void* pSetMenuItemInfoW = (void*)GetProcAddress(hUser32, "SetMenuItemInfoW");
    void* pTrackPopupMenuEx = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
    void* pTrackPopupMenu = (void*)GetProcAddress(hUser32, "TrackPopupMenu");

    if (!pInsertMenuItemW || !pInsertMenuW || !pSetMenuItemInfoW) {
        return FALSE;
    }

    if (!Wh_SetFunctionHook(pInsertMenuItemW, (void*)InsertMenuItemW_Hook, (void**)&InsertMenuItemW_Original)) {
        return FALSE;
    }
    if (!Wh_SetFunctionHook(pInsertMenuW, (void*)InsertMenuW_Hook, (void**)&InsertMenuW_Original)) {
        return FALSE;
    }
    if (!Wh_SetFunctionHook(pSetMenuItemInfoW, (void*)SetMenuItemInfoW_Hook, (void**)&SetMenuItemInfoW_Original)) {
        return FALSE;
    }
    if (pTrackPopupMenuEx) {
        Wh_SetFunctionHook(pTrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Original);
    }
    if (pTrackPopupMenu) {
        Wh_SetFunctionHook(pTrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&TrackPopupMenu_Original);
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32) {
        void* pSHCreateDefaultContextMenu = (void*)GetProcAddress(hShell32, "SHCreateDefaultContextMenu");
        void* pCDefFolderMenu_Create2 = (void*)GetProcAddress(hShell32, "CDefFolderMenu_Create2");

        if (pSHCreateDefaultContextMenu) {
            Wh_SetFunctionHook(
                pSHCreateDefaultContextMenu,
                (void*)SHCreateDefaultContextMenu_Hook,
                (void**)&SHCreateDefaultContextMenu_Original
            );
        }
        if (pCDefFolderMenu_Create2 && pCDefFolderMenu_Create2 != pSHCreateDefaultContextMenu) {
            Wh_SetFunctionHook(
                pCDefFolderMenu_Create2,
                (void*)CDefFolderMenu_Create2_Hook,
                (void**)&CDefFolderMenu_Create2_Original
            );
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    CleanupAllResources();
}

void Wh_ModSettingsChanged() {
    LoadAllSettings();
}
