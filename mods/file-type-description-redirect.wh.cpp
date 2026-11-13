// ==WindhawkMod==
// @id              file-type-description-redirect
// @name            File Type Description Redirect
// @description     Override file type descriptions by extension using text or indirect string resources in an INI file
// @version         1.1.3
// @author          Momo
// @github          https://github.com/FreshP-0325
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lshell32 -lshlwapi -lole32 -loleaut32 -lpropsys -luuid
// ==/WindhawkMod==
// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# File Type Description Redirect

Override file type descriptions by extension without changing the registry,
file associations, icons, or thumbnails. Configuration example:

```ini
[types]
.txt=Text document
.log,.cfg=Configuration file
.md=@%SystemRoot%\System32\notepad.exe,-469
```

Set INI file path to an absolute path. Environment variables are supported in
the INI path and resource module paths. Relative resource paths are resolved
against the INI directory. Resource module paths may be double-quoted.

Use UTF-8 (with or without BOM) or UTF-16 LE with BOM. Extensions include the
dot, are case-insensitive, and match the last extension. Multiple extensions
can be separated by commas. The last valid duplicate entry wins. Empty values
and invalid resources are skipped and logged. Lines starting with ; or # are
comments; inline comments are unsupported. Literal text is trimmed but otherwise
preserved, including percent signs and equals signs. Use @@ for a literal
leading @, for example .example=@@Example file.

Indirect strings use @module,-resourceID and are resolved by Windows, including
its language-specific resource lookup. Missing resources fall back to the
original description. Resource IDs depend on the Windows or application version.

After editing the INI or replacing resources, add 1 to Reload INI file (for
example, 0 to 1) and save the settings. Reopen affected windows or menus to see
the changes. This reads the configuration again without restarting the mod.
These numbers are manual triggers, not timers or statistics. Changing any setting
rereads the INI; Reload INI file provides a convenient way to do so without
changing another setting. Startup and shutdown send no global refresh.
If old descriptions remain, Reload INI and refresh Explorer additionally requests
one cache refresh from the Explorer process that owns the taskbar. Otherwise,
leave that value unchanged.

The mod requires a Shell profile matching your Windows system DLLs. Before
installing, run python scripts/prepare.py --mod description from the repository
on your PC, then compile the prepared source in Windhawk. Repeat after Windows
updates change the relevant DLLs. A mismatched profile prevents the mod from
loading. The profile is embedded in this source; no extra header is needed.

The mod handles the filesystem Type column, System.ItemTypeText through standard
Shell item APIs, SHGetFileInfo type names, and AssocQueryString friendly document
names when queried by extension. It also handles native property stores,
property caches, and formatting of raw System.ItemType for modern Shell views.
Only returned display values are overridden; raw extensions and cache contents
are preserved. Stores without a usable ItemType retain their original values.
The mod also replaces labels in the standard Shell New menu. Each command's
extension comes from IContextMenu::GetCommandString, including packaged-app items.
Only menu text changes; command IDs, templates, icons, internal item data, and
default filenames remain intact. This does not add missing ShellNew entries.
ProgID-only queries cannot identify a specific extension and are left unchanged.
Programs using other property stores, private
caches, or their own type descriptions may be unaffected. Group labels or saved
search indexes may require a refresh and are not rewritten on disk.

By default, hooks run only in explorer.exe. Other apps require both Allow other
processes and a matching Custom process inclusion list entry. With that option
enabled and * included, there is no additional Explorer-only restriction. The
option controls hook setup, not whether Windhawk loads the mod DLL. Compatibility
depends on the application. Windhawk's global exclusions still apply.
File Type Icon Redirect is a separate mod and can be used alongside this one.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iniPath: "%USERPROFILE%\\FileTypeDescriptions\\types.ini"
  $name: INI file path
  $description: Absolute INI path. Environment variables are supported.
- reloadCounter: 0
  $name: Reload INI file
  $description: After editing the INI file, add 1 to this number (for example, 0 to 1) and save the settings. The mod will read the file again without restarting. Reopen affected windows or menus to see the changes.
- refreshCounter: 0
  $name: Reload INI and refresh Explorer
  $description: Use only if old descriptions remain after reloading the INI. Add 1 to this number and save the settings to reload the INI and request an Explorer refresh. This can briefly redraw windows and reload thumbnails. Otherwise, leave this value unchanged.
- allowOtherProcesses: false
  $name: Allow other processes
  $description: Also allow hooks in non-Explorer processes selected by Windhawk's inclusion list. Requires both controls. This switch does not prevent DLL loading. Default is Explorer-only.
*/
// ==/WindhawkModSettings==

#ifndef FILE_TYPE_DESCRIPTION_REDIRECT_TEST
#include <windhawk_api.h>
#endif
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <propsys.h>
#include <propvarutil.h>
#include <wrl/client.h>
#include <atomic>
#include <array>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {
using Microsoft::WRL::ComPtr;
struct HookTarget {
    PCWSTR module;
    DWORD machine, timestamp, imageSize, checksum, rva;
};
// BEGIN GENERATED SHELL PROFILE
#ifdef _WIN64
[[maybe_unused]] constexpr HookTarget k_iconUI{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 236448u};
[[maybe_unused]] constexpr HookTarget k_iconIndex{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 457872u};
[[maybe_unused]] constexpr HookTarget k_iconFactory{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 232288u};
[[maybe_unused]] constexpr HookTarget k_detailsEx{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 1736240u};
[[maybe_unused]] constexpr HookTarget k_detailsOf{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 95040u};
[[maybe_unused]] constexpr HookTarget k_itemString{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 2214896u};
[[maybe_unused]] constexpr HookTarget k_itemProperty{L"windows.storage.dll", 34404u, 3070770283u, 9101312u, 9289426u, 2727136u};
[[maybe_unused]] constexpr HookTarget k_typeFormat{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 158128u};
[[maybe_unused]] constexpr HookTarget k_storeDefault{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 33568u};
[[maybe_unused]] constexpr HookTarget k_storeFast{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 33568u};
[[maybe_unused]] constexpr HookTarget k_storeBest{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 33568u};
[[maybe_unused]] constexpr HookTarget k_storeDelayed{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 33568u};
[[maybe_unused]] constexpr HookTarget k_storeMemory{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 138880u};
[[maybe_unused]] constexpr HookTarget k_storeCacheValue{L"PROPSYS.dll", 34404u, 3169301076u, 1003520u, 1051747u, 345600u};
[[maybe_unused]] constexpr HookTarget k_newMenuQuery{L"SHELL32.dll", 34404u, 292758140u, 7979008u, 8158634u, 373664u};
[[maybe_unused]] constexpr HookTarget k_newMenuMessage{L"SHELL32.dll", 34404u, 292758140u, 7979008u, 8158634u, 5012976u};
[[maybe_unused]] constexpr HookTarget k_newMenuMessage2{L"SHELL32.dll", 34404u, 292758140u, 7979008u, 8158634u, 1506976u};
#else
[[maybe_unused]] constexpr HookTarget k_iconUI{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1358992u};
[[maybe_unused]] constexpr HookTarget k_iconIndex{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1109328u};
[[maybe_unused]] constexpr HookTarget k_iconFactory{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1107856u};
[[maybe_unused]] constexpr HookTarget k_detailsEx{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1361856u};
[[maybe_unused]] constexpr HookTarget k_detailsOf{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 5342832u};
[[maybe_unused]] constexpr HookTarget k_itemString{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1684048u};
[[maybe_unused]] constexpr HookTarget k_itemProperty{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 5152304u};
[[maybe_unused]] constexpr HookTarget k_typeFormat{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 343760u};
[[maybe_unused]] constexpr HookTarget k_storeDefault{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 279152u};
[[maybe_unused]] constexpr HookTarget k_storeFast{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 279152u};
[[maybe_unused]] constexpr HookTarget k_storeBest{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 279152u};
[[maybe_unused]] constexpr HookTarget k_storeDelayed{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 279152u};
[[maybe_unused]] constexpr HookTarget k_storeMemory{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 145056u};
[[maybe_unused]] constexpr HookTarget k_storeCacheValue{L"PROPSYS.dll", 332u, 3837759275u, 823296u, 873650u, 315216u};
[[maybe_unused]] constexpr HookTarget k_newMenuQuery{L"SHELL32.dll", 332u, 1382932507u, 6729728u, 6853355u, 5300464u};
[[maybe_unused]] constexpr HookTarget k_newMenuMessage{L"SHELL32.dll", 332u, 1382932507u, 6729728u, 6853355u, 5297904u};
[[maybe_unused]] constexpr HookTarget k_newMenuMessage2{L"SHELL32.dll", 332u, 1382932507u, 6729728u, 6853355u, 5297840u};
#endif
// END GENERATED SHELL PROFILE
using TypeMap = std::unordered_map<std::wstring, std::wstring>;
std::shared_ptr<const TypeMap> g_types = std::make_shared<const TypeMap>();
thread_local bool g_inHook = false;
struct HookGuard {
    HookGuard() { g_inHook = true; }
    ~HookGuard() { g_inHook = false; }
};

std::wstring Trim(std::wstring_view s) {
    const auto first = s.find_first_not_of(L" \t\r\n");
    if (first == s.npos) return {};
    return std::wstring(s.substr(first, s.find_last_not_of(L" \t\r\n") - first + 1));
}

std::wstring Lower(std::wstring s) {
    // Invariant case folding, independent of the process's C locale.
    if (!s.empty()) {
        std::wstring result(s.size(), L'\0');
        if (LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, s.data(),
                          static_cast<int>(s.size()), result.data(),
                          static_cast<int>(result.size()), nullptr, nullptr, 0)) {
            return result;
        }
    }
    return s;
}

std::optional<std::wstring> Expand(std::wstring s) {
    DWORD size = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!size || size > 32768) return std::nullopt;
    std::wstring result(size, L'\0');
    DWORD written = ExpandEnvironmentStringsW(s.c_str(), result.data(), size);
    if (!written || written > size) return std::nullopt;
    result.resize(written - 1);
    return result;
}

bool ReadText(const std::wstring& path, std::wstring& text) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                             nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size;
    bool ok = GetFileSizeEx(file, &size) && size.QuadPart >= 0 &&
              size.QuadPart <= 4 * 1024 * 1024;
    std::vector<char> bytes(ok ? static_cast<size_t>(size.QuadPart) : 0);
    DWORD read = 0;
    if (ok && !bytes.empty()) {
        ok = ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &read,
                      nullptr) && read == bytes.size();
    }
    CloseHandle(file);
    if (!ok) return false;
    text.clear();
    if (bytes.empty()) return true;
    if (bytes.size() >= 2 && static_cast<unsigned char>(bytes[0]) == 0xff &&
        static_cast<unsigned char>(bytes[1]) == 0xfe) {
        if (bytes.size() % 2) return false;
        text.resize((bytes.size() - 2) / 2);
        std::memcpy(text.data(), bytes.data() + 2, bytes.size() - 2);
    } else {
        size_t skip = bytes.size() >= 3 &&
                      std::memcmp(bytes.data(), "\xEF\xBB\xBF", 3) == 0 ? 3 : 0;
        int n = static_cast<int>(bytes.size() - skip);
        if (n == 0) return true;
        int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        bytes.data() + skip, n, nullptr, 0);
        if (!count) return false;
        text.resize(count);
        if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 bytes.data() + skip, n, text.data(), count)) return false;
    }
    return text.find(L'\0') == text.npos;
}

bool ValidExtension(const std::wstring& ext) {
    return ext.size() > 1 && ext.front() == L'.' &&
           ext.find_first_of(L".\\/:*?\"<>|,; \t", 1) == ext.npos;
}

std::optional<std::wstring> ResolveDescription(std::wstring value,
                                              const std::filesystem::path& base) {
    value = Trim(value);
    if (value.empty()) return std::nullopt;
    if (value.starts_with(L"@@")) return value.substr(1);
    if (value.front() != L'@') return value;
    auto comma = value.rfind(L',');
    if (comma == value.npos) return std::nullopt;
    auto id = Trim(std::wstring_view(value).substr(comma + 1));
    if (id.size() < 2 || id.front() != L'-' ||
        id.find_first_not_of(L"0123456789", 1) != id.npos) return std::nullopt;
    unsigned long long resourceId = 0;
    for (size_t i = 1; i < id.size(); ++i) {
        resourceId = resourceId * 10 + (id[i] - L'0');
        if (resourceId > 2147483647ULL) return std::nullopt;
    }
    if (!resourceId) return std::nullopt;
    auto module = Trim(std::wstring_view(value).substr(1, comma - 1));
    if (module.size() >= 2 && module.front() == L'"' && module.back() == L'"') {
        module = module.substr(1, module.size() - 2);
    }
    auto expanded = Expand(module);
    if (!expanded || expanded->empty() || expanded->find(L'"') != expanded->npos) return std::nullopt;
    std::filesystem::path path(*expanded);
    if (!path.is_absolute()) {
        if (path.has_root_name() || path.has_root_directory()) return std::nullopt;
        path = base / path;
    }
    auto indirect = L"@" + path.lexically_normal().wstring() + L"," + id;
    std::vector<WCHAR> result(32768);
    if (FAILED(SHLoadIndirectString(indirect.c_str(), result.data(),
                                    static_cast<UINT>(result.size()), nullptr)) || !result[0]) {
        return std::nullopt;
    }
    return std::wstring(result.data());
}

TypeMap ParseIni(const std::wstring& text, const std::filesystem::path& base) {
    TypeMap types;
    bool active = false;
    size_t start = 0, number = 0;
    while (start < text.size()) {
        auto end = text.find(L'\n', start);
        auto line = Trim(std::wstring_view(text).substr(start,
            end == text.npos ? text.size() - start : end - start));
        start = end == text.npos ? text.size() : end + 1;
        ++number;
        if (line.empty() || line.front() == L';' || line.front() == L'#') continue;
        if (line.front() == L'[') { active = Lower(line) == L"[types]"; continue; }
        if (!active) continue;
        auto equals = line.find(L'=');
        auto description = equals == line.npos ? std::nullopt :
            ResolveDescription(line.substr(equals + 1), base);
        if (!description) { Wh_Log(L"INI line %zu: invalid description; skipped", number); continue; }
        auto keys = line.substr(0, equals);
        size_t pos = 0;
        do {
            auto comma = keys.find(L',', pos);
            auto ext = Lower(Trim(std::wstring_view(keys).substr(pos,
                comma == keys.npos ? keys.size() - pos : comma - pos)));
            if (ValidExtension(ext)) types[ext] = *description;
            else Wh_Log(L"INI line %zu: invalid extension; skipped", number);
            if (comma == keys.npos) break;
            pos = comma + 1;
        } while (pos <= keys.size());
    }
    return types;
}

void LoadSettings() {
    auto setting = Wh_GetStringSetting(L"iniPath");
    auto path = Expand(Trim(setting ? setting : L""));
    Wh_FreeStringSetting(setting);
    TypeMap types;
    std::wstring text;
    if (path && std::filesystem::path(*path).is_absolute() && ReadText(*path, text)) {
        types = ParseIni(text, std::filesystem::path(*path).parent_path());
    } else Wh_Log(L"Cannot read INI: check absolute path, encoding, and size (max 4 MiB)");
    Wh_Log(L"Loaded %zu type descriptions", types.size());
    std::atomic_store(&g_types, std::make_shared<const TypeMap>(std::move(types)));
}

std::optional<std::wstring> LookupExtension(PCWSTR ext) {
    if (!ext) return std::nullopt;
    auto types = std::atomic_load(&g_types);
    auto it = types->find(Lower(ext));
    return it == types->end() ? std::nullopt : std::optional(it->second);
}

std::optional<std::wstring> LookupPath(PCWSTR path) {
    return path ? LookupExtension(PathFindExtensionW(path)) : std::nullopt;
}

std::optional<std::wstring> LookupItem(IShellFolder* folder, PCUITEMID_CHILD item) {
    if (!item || !item->mkid.cb || std::atomic_load(&g_types)->empty()) return std::nullopt;
    STRRET name{};
    if (FAILED(folder->GetDisplayNameOf(item,
        static_cast<SHGDNF>(SHGDN_FORPARSING | SHGDN_INFOLDER), &name))) return std::nullopt;
    PWSTR path = nullptr;
    if (FAILED(StrRetToStrW(&name, item, &path))) return std::nullopt;
    auto value = LookupPath(path);
    CoTaskMemFree(path);
    if (!value) return std::nullopt;
    SFGAOF attributes = SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM;
    if (FAILED(folder->GetAttributesOf(1, &item, &attributes)) ||
        !(attributes & SFGAO_FILESYSTEM) ||
        ((attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM))) return std::nullopt;
    return value;
}

std::optional<std::wstring> LookupShellItem(IShellItem* item) {
    if (std::atomic_load(&g_types)->empty()) return std::nullopt;
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) return std::nullopt;
    auto value = LookupPath(path);
    CoTaskMemFree(path);
    if (!value) return std::nullopt;
    SFGAOF attributes;
    if (FAILED(item->GetAttributes(SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM, &attributes)) ||
        !(attributes & SFGAO_FILESYSTEM) ||
        ((attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM))) return std::nullopt;
    return value;
}

bool IsTypeKey(REFPROPERTYKEY key) { return IsEqualPropertyKey(key, PKEY_ItemTypeText); }
HRESULT CopyString(const std::wstring& text, PWSTR* result) {
    *result = static_cast<PWSTR>(CoTaskMemAlloc((text.size() + 1) * sizeof(WCHAR)));
    if (!*result) return E_OUTOFMEMORY;
    std::memcpy(*result, text.c_str(), (text.size() + 1) * sizeof(WCHAR));
    return S_OK;
}

using DetailsEx_t = HRESULT (STDMETHODCALLTYPE*)(IShellFolder2*, PCUITEMID_CHILD, const SHCOLUMNID*, VARIANT*);
using DetailsOf_t = HRESULT (STDMETHODCALLTYPE*)(IShellFolder2*, PCUITEMID_CHILD, UINT, SHELLDETAILS*);
using ItemString_t = HRESULT (STDMETHODCALLTYPE*)(IShellItem2*, REFPROPERTYKEY, PWSTR*);
using ItemProperty_t = HRESULT (STDMETHODCALLTYPE*)(IShellItem2*, REFPROPERTYKEY, PROPVARIANT*);
using TypeFormat_t = HRESULT (STDMETHODCALLTYPE*)(IPropertyDescription*, REFPROPVARIANT,
                                                PROPDESC_FORMAT_FLAGS, PWSTR*);
using StoreValue_t = HRESULT (STDMETHODCALLTYPE*)(IPropertyStore*, REFPROPERTYKEY, PROPVARIANT*);
using CacheValue_t = HRESULT (STDMETHODCALLTYPE*)(IPropertyStoreCache*, REFPROPERTYKEY, PROPVARIANT*, PSC_STATE*);
DetailsEx_t g_detailsEx;
DetailsOf_t g_detailsOf;
ItemString_t g_itemString;
ItemProperty_t g_itemProperty;
TypeFormat_t g_typeFormat;
std::array<StoreValue_t, 5> g_storeValues{};
CacheValue_t g_cacheValue;
decltype(&AssocQueryStringW) g_assocW;
decltype(&AssocQueryStringA) g_assocA;
using NewMenuQuery_t = HRESULT (STDMETHODCALLTYPE*)(IContextMenu*, HMENU, UINT, UINT, UINT, UINT);
using NewMenuMessage_t = HRESULT (STDMETHODCALLTYPE*)(IContextMenu2*, UINT, WPARAM, LPARAM);
using NewMenuMessage2_t = HRESULT (STDMETHODCALLTYPE*)(IContextMenu3*, UINT, WPARAM, LPARAM, LRESULT*);
NewMenuQuery_t g_newMenuQuery;
NewMenuMessage_t g_newMenuMessage;
NewMenuMessage2_t g_newMenuMessage2;

struct SavedMenuText { std::wstring original, replacement; };
struct NewMenuContext {
    IContextMenu* owner; // Identity only. Never retained or dereferenced from this cache.
    UINT first, count;
    std::mutex mutex;
    std::unordered_map<UINT, SavedMenuText> labels;
    NewMenuContext(IContextMenu* object, UINT start, UINT length) : owner(object), first(start), count(length) {}
};
std::mutex g_newMenusMutex;
std::unordered_map<HMENU, std::shared_ptr<NewMenuContext>> g_newMenus;
thread_local bool g_inNewMenu = false;

std::optional<std::wstring> ReadMenuText(HMENU menu, UINT position) {
    MENUITEMINFOW info{}; info.cbSize = sizeof(info);
    info.fMask = MIIM_STRING;
    if (!GetMenuItemInfoW(menu, position, TRUE, &info) || info.cch > 32767) return std::nullopt;
    std::wstring value(info.cch + 1, L'\0');
    info.dwTypeData = value.data();
    info.cch = static_cast<UINT>(value.size());
    if (!GetMenuItemInfoW(menu, position, TRUE, &info)) return std::nullopt;
    value.resize(info.cch);
    return value;
}
std::wstring EscapeMenuText(std::wstring_view text) {
    std::wstring result;
    for (WCHAR c : text) { result += c; if (c == L'&') result += c; }
    return result;
}

HRESULT STDMETHODCALLTYPE NewMenuQueryHook(IContextMenu* self, HMENU menu, UINT position,
                                           UINT first, UINT last, UINT flags) {
    HRESULT hr = g_newMenuQuery(self, menu, position, first, last, flags);
    UINT count = HRESULT_CODE(hr);
    MENUITEMINFOW info{}; info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU | MIIM_ID;
    if (SUCCEEDED(hr) && count && GetMenuItemInfoW(menu, position, TRUE, &info) &&
        info.wID == first && info.hSubMenu) {
        auto context = std::make_shared<NewMenuContext>(self, first, count);
        std::lock_guard lock(g_newMenusMutex);
        // Keep only bounded identity/label metadata, with no COM references,
        // proxies, Release detours, or changes to native menu item data.
        // A QueryContextMenu call always replaces metadata for a reused HMENU.
        for (auto it = g_newMenus.begin(); it != g_newMenus.end();) {
            if (!IsMenu(it->first)) it = g_newMenus.erase(it); else ++it;
        }
        if (g_newMenus.size() < 256 || g_newMenus.contains(info.hSubMenu))
            g_newMenus[info.hSubMenu] = std::move(context);
    }
    return hr;
}

void ApplyNewMenuDescriptions(IContextMenu* self, HMENU menu) {
    if (g_inNewMenu) return;
    std::shared_ptr<NewMenuContext> context;
    {
        std::lock_guard lock(g_newMenusMutex);
        auto it = g_newMenus.find(menu);
        if (it == g_newMenus.end() || it->second->owner != self) return;
        context = it->second;
    }
    std::unique_lock lock(context->mutex, std::try_to_lock);
    if (!lock.owns_lock()) return;
    struct Scope { Scope() { g_inNewMenu = true; } ~Scope() { g_inNewMenu = false; } } scope;
    auto types = std::atomic_load(&g_types);
    if (types->empty() && context->labels.empty()) return;
    int count = GetMenuItemCount(menu);
    if (count < 0 || count > 4096) return;
    for (int position = 0; position < count; ++position) {
        MENUITEMINFOW info{}; info.cbSize = sizeof(info);
        info.fMask = MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, position, TRUE, &info) || info.hSubMenu ||
            (info.fType & (MFT_SEPARATOR | MFT_OWNERDRAW)) || info.wID < context->first ||
            info.wID - context->first >= context->count) continue;
        WCHAR verb[1024]{};
        // GetCommandString takes an offset, not the absolute menu command ID.
        // Never query separators/out-of-range IDs: native handlers can reject
        // them unsafely. File-creation verbs identify their exact extension.
        if (FAILED(self->GetCommandString(info.wID - context->first, GCS_VERBW, nullptr,
                         reinterpret_cast<LPSTR>(verb), ARRAYSIZE(verb)))) continue;
        verb[ARRAYSIZE(verb) - 1] = L'\0';
        auto ext = Lower(verb);
        if (!ValidExtension(ext)) continue; // Keep Folder/Shortcut special verbs intact.
        auto current = ReadMenuText(menu, position);
        if (!current) continue;
        auto saved = context->labels.find(info.wID);
        auto original = saved != context->labels.end() && *current == saved->second.replacement ?
            saved->second.original : *current;
        auto mapping = types->find(ext);
        // Menu labels have a much smaller practical limit than INI contents.
        // Skip oversized definitions here without affecting property getters.
        if (mapping != types->end() && mapping->second.size() > 16383) continue;
        auto replacement = mapping == types->end() ? original : EscapeMenuText(mapping->second);
        if (replacement == *current) continue;
        MENUITEMINFOW update{}; update.cbSize = sizeof(update);
        update.fMask = MIIM_STRING;
        update.dwTypeData = replacement.data();
        if (SetMenuItemInfoW(menu, position, TRUE, &update)) {
            if (mapping == types->end()) context->labels.erase(info.wID);
            else context->labels[info.wID] = {std::move(original), std::move(replacement)};
        }
    }
}
HRESULT STDMETHODCALLTYPE NewMenuMessageHook(IContextMenu2* self, UINT message, WPARAM wParam, LPARAM lParam) {
    HRESULT hr = g_newMenuMessage(self, message, wParam, lParam);
    if (SUCCEEDED(hr) && message == WM_INITMENUPOPUP)
        ApplyNewMenuDescriptions(self, reinterpret_cast<HMENU>(wParam));
    return hr;
}
HRESULT STDMETHODCALLTYPE NewMenuMessage2Hook(IContextMenu3* self, UINT message, WPARAM wParam,
                                             LPARAM lParam, LRESULT* result) {
    HRESULT hr = g_newMenuMessage2(self, message, wParam, lParam, result);
    if (SUCCEEDED(hr) && message == WM_INITMENUPOPUP)
        ApplyNewMenuDescriptions(self, reinterpret_cast<HMENU>(wParam));
    return hr;
}
decltype(&SHGetFileInfoW) g_infoW;
decltype(&SHGetFileInfoA) g_infoA;

HRESULT STDMETHODCALLTYPE DetailsExHook(IShellFolder2* folder, PCUITEMID_CHILD item,
                                       const SHCOLUMNID* key, VARIANT* result) {
    if (!g_inHook && key && result && IsTypeKey(*key)) {
        HookGuard guard;
        if (auto value = LookupItem(folder, item)) {
            BSTR text = SysAllocStringLen(value->data(), static_cast<UINT>(value->size()));
            if (!text) return E_OUTOFMEMORY;
            VariantInit(result); result->vt = VT_BSTR; result->bstrVal = text;
            return S_OK;
        }
    }
    return g_detailsEx(folder, item, key, result);
}

HRESULT STDMETHODCALLTYPE DetailsOfHook(IShellFolder2* folder, PCUITEMID_CHILD item,
                                       UINT column, SHELLDETAILS* result) {
    if (!g_inHook && item && result) {
        HookGuard guard;
        SHCOLUMNID key;
        // The legacy Type column maps to ItemType (the extension), but its
        // formatted display text is the friendly description. Only change the
        // formatted column; raw System.ItemType properties retain the extension.
        if (SUCCEEDED(folder->MapColumnToSCID(column, &key)) &&
            (IsTypeKey(key) || IsEqualPropertyKey(key, PKEY_ItemType))) {
            if (auto value = LookupItem(folder, item)) {
                PWSTR text;
                HRESULT hr = CopyString(*value, &text);
                if (FAILED(hr)) return hr;
                result->fmt = LVCFMT_LEFT; result->cxChar = 20;
                result->str.uType = STRRET_WSTR; result->str.pOleStr = text;
                return S_OK;
            }
        }
    }
    return g_detailsOf(folder, item, column, result);
}

HRESULT STDMETHODCALLTYPE ItemStringHook(IShellItem2* item, REFPROPERTYKEY key, PWSTR* result) {
    if (!g_inHook && result && IsTypeKey(key)) {
        HookGuard guard;
        if (auto value = LookupShellItem(item)) return CopyString(*value, result);
    }
    return g_itemString(item, key, result);
}
HRESULT STDMETHODCALLTYPE ItemPropertyHook(IShellItem2* item, REFPROPERTYKEY key, PROPVARIANT* result) {
    if (!g_inHook && result && IsTypeKey(key)) {
        HookGuard guard;
        if (auto value = LookupShellItem(item)) return InitPropVariantFromString(value->c_str(), result);
    }
    return g_itemProperty(item, key, result);
}

HRESULT STDMETHODCALLTYPE TypeFormatHook(IPropertyDescription* description, REFPROPVARIANT raw,
                                         PROPDESC_FORMAT_FLAGS flags, PWSTR* result) {
    // Modern Shell views can retrieve raw ItemType from a property store and
    // format it directly, bypassing GetDetailsOf and ItemTypeText getters.
    // Hook the presentation boundary; never replace the raw extension itself.
    if (!g_inHook && result && !std::atomic_load(&g_types)->empty() &&
        (raw.vt == VT_LPWSTR || raw.vt == VT_BSTR)) {
        PCWSTR extension = raw.vt == VT_LPWSTR ? raw.pwszVal : raw.bstrVal;
        if (auto value = LookupExtension(extension)) {
            HookGuard guard;
            PROPERTYKEY key{};
            if (SUCCEEDED(description->GetPropertyKey(&key)) && IsEqualPropertyKey(key, PKEY_ItemType)) {
                *result = nullptr;
                PROPVARIANT text;
                PropVariantInit(&text);
                HRESULT hr = InitPropVariantFromString(value->c_str(), &text);
                if (SUCCEEDED(hr)) {
                    // Let Windows format the already-friendly text, including
                    // locale, directional markers, and requested display flags.
                    // The guard lets the nested formatter call pass through.
                    hr = PSFormatForDisplayAlloc(PKEY_ItemTypeText, text, flags, result);
                }
                PropVariantClear(&text);
                return hr;
            }
        }
    }
    return g_typeFormat(description, raw, flags, result);
}

HRESULT OverrideStoredType(IPropertyStore* store, PROPVARIANT* result, HRESULT status) {
    // Derive the extension from this same store, never from the old display
    // text (which may be shared by many UWP-associated extensions).
    // The caller holds HookGuard, so the raw lookup cannot recursively override.
    if (FAILED(status) || (result->vt != VT_LPWSTR && result->vt != VT_BSTR)) return status;
    PROPVARIANT raw;
    PropVariantInit(&raw);
    HRESULT hr = store->GetValue(PKEY_ItemType, &raw);
    std::optional<std::wstring> value;
    if (SUCCEEDED(hr)) {
        if (raw.vt == VT_LPWSTR) value = LookupExtension(raw.pwszVal);
        else if (raw.vt == VT_BSTR) value = LookupExtension(raw.bstrVal);
    }
    PropVariantClear(&raw);
    if (!value) return status;
    PropVariantClear(result);
    hr = InitPropVariantFromString(value->c_str(), result);
    return FAILED(hr) ? hr : status;
}

template<size_t slot>
HRESULT STDMETHODCALLTYPE StoreValueHook(IPropertyStore* store, REFPROPERTYKEY key, PROPVARIANT* result) {
    if (!g_inHook && result && IsTypeKey(key) && !std::atomic_load(&g_types)->empty()) {
        HookGuard guard;
        HRESULT hr = g_storeValues[slot](store, key, result);
        return OverrideStoredType(store, result, hr);
    }
    return g_storeValues[slot](store, key, result);
}

HRESULT STDMETHODCALLTYPE CacheValueHook(IPropertyStoreCache* store, REFPROPERTYKEY key,
                                         PROPVARIANT* result, PSC_STATE* state) {
    if (!g_inHook && result && state && IsTypeKey(key) && !std::atomic_load(&g_types)->empty()) {
        HookGuard guard;
        HRESULT hr = g_cacheValue(store, key, result, state);
        // Preserve the cache's state and HRESULT. This modifies only the returned
        // copy; no SetValue/SetState calls or persistent cache mutations occur.
        return OverrideStoredType(store, result, hr);
    }
    return g_cacheValue(store, key, result, state);
}

std::wstring FromAnsi(PCSTR input) {
    if (!input) return {};
    int n = MultiByteToWideChar(CP_ACP, 0, input, -1, nullptr, 0);
    if (!n) return {};
    std::wstring text(n, L'\0');
    MultiByteToWideChar(CP_ACP, 0, input, -1, text.data(), n);
    text.resize(n - 1); return text;
}
std::string ToAnsi(const std::wstring& input) {
    int n = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (!n) return {};
    std::string text(n, '\0');
    WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, text.data(), n, nullptr, nullptr);
    text.resize(n - 1); return text;
}
template<typename String, typename Char>
HRESULT AssocResult(const String& text, Char* result, DWORD* size) {
    if (!size) return E_POINTER;
    DWORD capacity = *size;
    *size = static_cast<DWORD>(text.size() + 1);
    if (!result) return S_FALSE;
    if (capacity < *size) { if (capacity) result[0] = 0; return E_POINTER; }
    std::memcpy(result, text.c_str(), *size * sizeof(Char));
    return S_OK;
}
HRESULT WINAPI AssocWHook(ASSOCF flags, ASSOCSTR kind, PCWSTR assoc, PCWSTR extra,
                         PWSTR result, DWORD* size) {
    if (!g_inHook && kind == ASSOCSTR_FRIENDLYDOCNAME && !(flags & ASSOCF_OPEN_BYEXENAME)) {
        HookGuard guard;
        if (auto value = LookupExtension(assoc)) return AssocResult(*value, result, size);
    }
    return g_assocW(flags, kind, assoc, extra, result, size);
}
HRESULT WINAPI AssocAHook(ASSOCF flags, ASSOCSTR kind, PCSTR assoc, PCSTR extra,
                         PSTR result, DWORD* size) {
    if (!g_inHook && kind == ASSOCSTR_FRIENDLYDOCNAME && !(flags & ASSOCF_OPEN_BYEXENAME)) {
        HookGuard guard;
        if (auto value = LookupExtension(FromAnsi(assoc).c_str())) return AssocResult(ToAnsi(*value), result, size);
    }
    return g_assocA(flags, kind, assoc, extra, result, size);
}

std::optional<std::wstring> LookupInfo(PCWSTR input, DWORD attributes, UINT flags) {
    if (std::atomic_load(&g_types)->empty()) return std::nullopt;
    PWSTR allocated = nullptr;
    PCWSTR path = input;
    if (flags & SHGFI_PIDL) {
        if (!input || FAILED(SHGetNameFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(input),
                                               SIGDN_FILESYSPATH, &allocated))) return std::nullopt;
        path = allocated;
    }
    auto value = LookupPath(path);
    if (value) {
        DWORD actual = (flags & SHGFI_USEFILEATTRIBUTES) ? attributes :
                       (path ? GetFileAttributesW(path) : INVALID_FILE_ATTRIBUTES);
        if (actual == INVALID_FILE_ATTRIBUTES || (actual & FILE_ATTRIBUTE_DIRECTORY)) value.reset();
    }
    CoTaskMemFree(allocated);
    return value;
}
DWORD_PTR WINAPI InfoWHook(PCWSTR path, DWORD attr, SHFILEINFOW* result, UINT size, UINT flags) {
    DWORD_PTR status = g_infoW(path, attr, result, size, flags);
    if (status && !g_inHook && result && size >= sizeof(*result) && (flags & SHGFI_TYPENAME)) {
        HookGuard guard;
        if (auto value = LookupInfo(path, attr, flags)) {
            lstrcpynW(result->szTypeName, value->c_str(), ARRAYSIZE(result->szTypeName));
        }
    }
    return status;
}
DWORD_PTR WINAPI InfoAHook(PCSTR path, DWORD attr, SHFILEINFOA* result, UINT size, UINT flags) {
    DWORD_PTR status = g_infoA(path, attr, result, size, flags);
    if (status && !g_inHook && result && size >= sizeof(*result) && (flags & SHGFI_TYPENAME)) {
        HookGuard guard;
        auto wide = (flags & SHGFI_PIDL) ? std::wstring() : FromAnsi(path);
        auto input = (flags & SHGFI_PIDL) ? reinterpret_cast<PCWSTR>(path) : wide.c_str();
        if (auto value = LookupInfo(input, attr, flags)) {
            auto ansi = ToAnsi(*value);
            lstrcpynA(result->szTypeName, ansi.c_str(), ARRAYSIZE(result->szTypeName));
        }
    }
    return status;
}

// BEGIN SHARED HOOK RUNTIME
// Embedded into each standalone mod by scripts/prepare.py.
struct ModuleReferences {
    std::vector<HMODULE> modules;
} g_hookModules;
bool g_allowOtherProcesses = false;
int g_refreshCounter = 0;

void ReleaseHookModules() {
    // Called by Wh_ModUninit after detours are removed, never by a static
    // destructor running under DllMain's loader lock. If hook registration
    // partially fails, let Windows reclaim the bounded references at exit.
    for (auto module : g_hookModules.modules) FreeLibrary(module);
    g_hookModules.modules.clear();
}

bool MatchesHookTarget(HMODULE module, const HookTarget& target) {
    if (!module) return false;
    auto base = reinterpret_cast<const BYTE*>(module);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != target.machine ||
        nt->FileHeader.TimeDateStamp != target.timestamp ||
        nt->OptionalHeader.SizeOfImage != target.imageSize ||
        nt->OptionalHeader.CheckSum != target.checksum || target.rva >= target.imageSize) return false;
    auto section = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (target.rva >= section[i].VirtualAddress &&
            target.rva - section[i].VirtualAddress < section[i].Misc.VirtualSize &&
            (section[i].Characteristics & IMAGE_SCN_MEM_EXECUTE)) return true;
    }
    return false;
}

void* ResolveHookTarget(const HookTarget& target) {
    if (_wcsicmp(target.module, L"windows.storage.dll") &&
        _wcsicmp(target.module, L"shell32.dll") && _wcsicmp(target.module, L"propsys.dll")) return nullptr;
    // Hold a module reference until Wh_ModUninit, after detours are removed.
    // No COM initialization, folder binding, or Shell object creation.
    HMODULE module = LoadLibraryExW(target.module, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return nullptr;
    g_hookModules.modules.push_back(module);
    if (!MatchesHookTarget(module, target)) {
        Wh_Log(L"Shell DLL profile mismatch. Run scripts/prepare.py on this PC and recompile.");
        return nullptr;
    }
    return reinterpret_cast<BYTE*>(module) + target.rva;
}

bool ShouldNotifyShellForProcess(PCWSTR executable, PCWSTR windowsDirectory) {
    if (!executable || !windowsDirectory) return false;
    auto explorer = (std::filesystem::path(windowsDirectory) / L"explorer.exe").wstring();
    return CompareStringOrdinal(executable, -1, explorer.c_str(), -1, TRUE) == CSTR_EQUAL;
}
bool IsExplorerProcess() {
    WCHAR executable[32768], windows[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, executable, ARRAYSIZE(executable));
    return n && n < ARRAYSIZE(executable) && GetWindowsDirectoryW(windows, ARRAYSIZE(windows)) &&
           ShouldNotifyShellForProcess(executable, windows);
}
bool LoadProcessPolicy() {
    g_allowOtherProcesses = Wh_GetIntSetting(L"allowOtherProcesses") != 0;
    g_refreshCounter = Wh_GetIntSetting(L"refreshCounter");
    if (!IsExplorerProcess() && !g_allowOtherProcesses) {
        Wh_Log(L"Skipped non-Explorer process. Other processes require explicit opt-in.");
        return false;
    }
    return true;
}
void NotifyShell() {
    // Called only by an explicit Refresh counter change. Startup, shutdown,
    // enable/disable, and ordinary INI reloads never broadcast cache invalidation.
    if (!IsExplorerProcess()) return;
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD owner = 0;
    if (!taskbar || !GetWindowThreadProcessId(taskbar, &owner) || owner != GetCurrentProcessId()) return;
    Wh_Log(L"Sending explicitly requested Shell cache refresh from taskbar owner");
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}
// END SHARED HOOK RUNTIME

template<typename Fn>
bool Hook(void* address, Fn detour, Fn* original) {
    return Wh_SetFunctionHook(address, reinterpret_cast<void*>(detour), reinterpret_cast<void**>(original));
}
bool InstallHooks() {
    void* detailsEx = ResolveHookTarget(k_detailsEx);
    void* detailsOf = ResolveHookTarget(k_detailsOf);
    void* itemString = ResolveHookTarget(k_itemString);
    void* itemProperty = ResolveHookTarget(k_itemProperty);
    void* typeFormat = ResolveHookTarget(k_typeFormat);
    std::array<void*, 5> storeValues{ResolveHookTarget(k_storeDefault), ResolveHookTarget(k_storeFast),
        ResolveHookTarget(k_storeBest), ResolveHookTarget(k_storeDelayed), ResolveHookTarget(k_storeMemory)};
    void* cacheValue = ResolveHookTarget(k_storeCacheValue);
    void* newQuery = ResolveHookTarget(k_newMenuQuery);
    void* newMessage = ResolveHookTarget(k_newMenuMessage);
    void* newMessage2 = ResolveHookTarget(k_newMenuMessage2);
    bool valid = detailsEx && detailsOf && itemString && itemProperty && typeFormat && cacheValue &&
                 newQuery && newMessage && newMessage2;
    for (auto address : storeValues) valid = valid && address;
    if (!valid) { ReleaseHookModules(); return false; }
    if (!(Hook(detailsEx, DetailsExHook, &g_detailsEx) &&
           Hook(detailsOf, DetailsOfHook, &g_detailsOf) &&
           Hook(itemString, ItemStringHook, &g_itemString) &&
           Hook(itemProperty, ItemPropertyHook, &g_itemProperty) &&
           Hook(typeFormat, TypeFormatHook, &g_typeFormat) &&
           Hook(reinterpret_cast<void*>(AssocQueryStringW), AssocWHook, &g_assocW) &&
           Hook(reinterpret_cast<void*>(AssocQueryStringA), AssocAHook, &g_assocA) &&
           Hook(reinterpret_cast<void*>(SHGetFileInfoW), InfoWHook, &g_infoW) &&
           Hook(reinterpret_cast<void*>(SHGetFileInfoA), InfoAHook, &g_infoA) &&
           Hook(cacheValue, CacheValueHook, &g_cacheValue) &&
           Hook(newQuery, NewMenuQueryHook, &g_newMenuQuery) &&
           Hook(newMessage, NewMenuMessageHook, &g_newMenuMessage) &&
           Hook(newMessage2, NewMenuMessage2Hook, &g_newMenuMessage2))) return false;
    const std::array<StoreValue_t, 5> detours{StoreValueHook<0>, StoreValueHook<1>,
        StoreValueHook<2>, StoreValueHook<3>, StoreValueHook<4>};
    for (size_t i = 0; i < storeValues.size(); ++i) {
        bool duplicate = false;
        for (size_t j = 0; j < i; ++j) if (storeValues[i] == storeValues[j]) duplicate = true;
        if (!duplicate && !Hook(storeValues[i], detours[i], &g_storeValues[i])) return false;
    }
    return true;
}
} // namespace

BOOL Wh_ModInit() {
    if (!LoadProcessPolicy()) return FALSE;
    LoadSettings();
    bool ok = InstallHooks();
    if (!ok) Wh_Log(L"Hook profile unavailable or incompatible; mod will not load");
    return ok;
}
void Wh_ModAfterInit() {}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    if ((Wh_GetIntSetting(L"allowOtherProcesses") != 0) != g_allowOtherProcesses) {
        *reload = TRUE;
        return TRUE;
    }
    LoadSettings();
    int refresh = Wh_GetIntSetting(L"refreshCounter");
    if (refresh != g_refreshCounter) { g_refreshCounter = refresh; NotifyShell(); }
    return TRUE;
}
void Wh_ModUninit() {
    std::atomic_store(&g_types, std::make_shared<const TypeMap>());
    { std::lock_guard lock(g_newMenusMutex); g_newMenus.clear(); }
    ReleaseHookModules();
}
