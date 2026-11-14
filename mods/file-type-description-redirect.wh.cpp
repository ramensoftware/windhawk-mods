// ==WindhawkMod==
// @id              file-type-description-redirect
// @name            File Type Description Redirect
// @description     Customize file type descriptions by extension with text or string resources
// @version         1.2.0
// @author          Momo
// @github          https://github.com/FreshP-0325
// @license         MIT
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lshell32 -lshlwapi -lole32 -loleaut32 -lpropsys -luuid
// ==/WindhawkMod==
// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# File Type Description Redirect

Customize file descriptions by extension without changing file associations or default
applications. Descriptions appear in the Type column, file properties, details, and
native Shell New menus. Icons and thumbnails are preserved.

Before:

![Before](https://raw.githubusercontent.com/FreshP-0325/windhawk-mod-assets/refs/heads/master/windows-file-type-icon-redirect/1.2.0/before.png)

After:

![After](https://raw.githubusercontent.com/FreshP-0325/windhawk-mod-assets/refs/heads/master/windows-file-type-icon-redirect/1.2.0/after.png)

## Configuration

Choose Configuration source:

* Windhawk settings (default): add entries to the descriptions list. Each entry accepts
  comma-separated extensions, including the dot. Save settings to apply changes.
  Windhawk also provides a Textual mode for bulk YAML editing.
* INI file: set an absolute INI file path. Only that file is read; the settings
  list is ignored. After editing the file, save a changed mod setting or disable
  and re-enable the mod, then reopen affected windows or menus.

Extensions are case-insensitive and match the last extension of a filename.
Fill in the first entry. An entry with both fields empty ends the settings list;
entries after it are ignored. No empty final entry is required.
INI files support UTF-8 (with or without BOM) or UTF-16 LE with BOM, full-line
comments starting with ; or #, and paths relative to the INI directory.
In Windhawk settings, resource paths must be absolute. Environment variables
are supported in paths. No external preparation script is required.

Descriptions can be Unicode text or indirect strings such as
@%SystemRoot%\System32\notepad.exe,-469. Relative module paths are supported in
INI mode. Double-quote paths if needed. Use @@ for literal text starting with @.
Windows resolves localized resources; IDs depend on the module version.

```ini
[types]
.txt=Text document
.log,.cfg=Configuration file
.md=@%SystemRoot%\System32\notepad.exe,-469
```

The last valid definition wins. Missing resources retain the system description
or an earlier valid definition. Only display text is overridden. Raw extensions,
stored properties, cache states, menu command IDs, templates, and default filenames
are preserved. The mod does not add missing ShellNew entries. Ordinary directories
and ProgID-only association queries are excluded.

## Refreshing and other applications

Refresh Explorer when mappings change is off by default. Enable it only if you
want a global cache refresh after saving changed mappings. It may briefly redraw
windows and reload thumbnails. Only the taskbar-owning Explorer sends it.
Startup, shutdown, unchanged mappings, and normal queries send no notifications.

The default target is explorer.exe. For standard Open/Save dialogs in another
application, add its executable to Custom process inclusion list in Windhawk's
advanced settings and restart the application. No additional process toggle is
required. Applications using private interfaces or caches may be unaffected.

The mod uses Windhawk symbol resolution for the installed system DLLs. Missing
required symbols prevent loading and are reported in the log. The mod targets
x86, x64, and ARM64. ARM64 has not been tested on a physical device.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- configurationSource: settings
  $name: Configuration source
  $description: Use the list below, or read a separate INI file. Only the selected source is used.
  $options:
    - settings: Windhawk settings
    - ini: INI file
- types:
  - - extensions: ""
      $name: Extensions
      $description: Comma-separated file extensions, including the dot. For example, .txt,.log
    - description: ""
      $name: Description
      $description: Unicode text, or an indirect resource such as @%SystemRoot%\System32\notepad.exe,-469
  $name: File type descriptions
  $description: Used only in Windhawk settings mode. Fill in the first entry; the list ends at the first entry with both fields empty. Later entries are ignored.
- iniPath: "%USERPROFILE%\\FileTypeDescriptions\\types.ini"
  $name: INI file path
  $description: Used only in INI file mode. Absolute path; environment variables are supported. Re-enable the mod after editing the file, or save a changed mod setting.
- refreshExplorer: false
  $name: Refresh Explorer when mappings change
  $description: Request one global cache refresh after saving changed mappings. May redraw windows and reload thumbnails. Usually leave off. Startup and shutdown do not request a refresh.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <propsys.h>
#include <propvarutil.h>
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
        if (base.empty() || path.has_root_name() || path.has_root_directory()) return std::nullopt;
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

bool LoadSettings() {
    TypeMap mappings;
    auto mode = WindhawkUtils::StringSetting::make(L"configurationSource");
    if (std::wstring_view(mode.get()) == L"ini") {
        auto setting = WindhawkUtils::StringSetting::make(L"iniPath");
        auto path = Expand(Trim(setting.get()));
        std::wstring text;
        if (path && std::filesystem::path(*path).is_absolute() && ReadText(*path, text))
            mappings = ParseIni(text, std::filesystem::path(*path).parent_path());
        else Wh_Log(L"Cannot read INI: check absolute path, encoding, and size (max 4 MiB)");
    } else {
        for (int i = 0; ; ++i) {
            auto extensions = WindhawkUtils::StringSetting::make(L"types[%d].extensions", i);
            auto setting = WindhawkUtils::StringSetting::make(L"types[%d].description", i);
            auto keys = Trim(extensions.get());
            auto text = Trim(setting.get());
            if (keys.empty() && text.empty()) break;
            auto value = ResolveDescription(text, {});
            if (!value) { Wh_Log(L"Entry %d: invalid description; skipped", i + 1); continue; }
            size_t pos = 0;
            do {
                auto comma = keys.find(L',', pos);
                auto ext = Lower(Trim(std::wstring_view(keys).substr(pos,
                    comma == keys.npos ? keys.size() - pos : comma - pos)));
                if (ValidExtension(ext)) mappings[ext] = *value;
                else Wh_Log(L"Entry %d: invalid extension; skipped", i + 1);
                if (comma == keys.npos) break;
                pos = comma + 1;
            } while (pos <= keys.size());
        }
    }
    Wh_Log(L"Loaded %zu extension mappings", mappings.size());
    bool changed = mappings != *std::atomic_load(&g_types);
    std::atomic_store(&g_types, std::make_shared<const TypeMap>(std::move(mappings)));
    return changed;
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
std::array<StoreValue_t, 2> g_storeValues{};
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
HRESULT AssocResult(ASSOCF flags, const String& text, Char* result, DWORD* size) {
    if (!size) return E_POINTER;
    DWORD capacity = *size;
    *size = static_cast<DWORD>(text.size() + 1);
    if (!result) return S_FALSE;
    if (capacity < *size) {
        if (flags & ASSOCF_NOTRUNCATE) {
            if (capacity) result[0] = 0;
            return E_POINTER;
        }
        if (capacity) {
            auto copied = static_cast<size_t>(capacity - 1);
            std::memcpy(result, text.data(), copied * sizeof(Char));
            result[copied] = 0;
        }
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    std::memcpy(result, text.c_str(), *size * sizeof(Char));
    return S_OK;
}
HRESULT WINAPI AssocWHook(ASSOCF flags, ASSOCSTR kind, PCWSTR assoc, PCWSTR extra,
                         PWSTR result, DWORD* size) {
    if (!g_inHook && kind == ASSOCSTR_FRIENDLYDOCNAME && !(flags & ASSOCF_OPEN_BYEXENAME)) {
        HookGuard guard;
        if (auto value = LookupExtension(assoc)) return AssocResult(flags, *value, result, size);
    }
    return g_assocW(flags, kind, assoc, extra, result, size);
}
HRESULT WINAPI AssocAHook(ASSOCF flags, ASSOCSTR kind, PCSTR assoc, PCSTR extra,
                         PSTR result, DWORD* size) {
    if (!g_inHook && kind == ASSOCSTR_FRIENDLYDOCNAME && !(flags & ASSOCF_OPEN_BYEXENAME)) {
        HookGuard guard;
        if (auto value = LookupExtension(FromAnsi(assoc).c_str())) return AssocResult(flags, ToAnsi(*value), result, size);
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

struct ModuleReferences { std::vector<HMODULE> modules; } g_hookModules;

HMODULE LoadHookModule(PCWSTR name) {
    HMODULE module = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module) g_hookModules.modules.push_back(module);
    return module;
}
void ReleaseHookModules() {
    // Windhawk removes detours before this callback. Never FreeLibrary from a
    // static destructor under the loader lock. On partial setup failure, keep
    // the bounded module references until process exit while hooks roll back.
    for (HMODULE module : g_hookModules.modules) FreeLibrary(module);
    g_hookModules.modules.clear();
}
void NotifyShell() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD owner = 0;
    if (!taskbar || !GetWindowThreadProcessId(taskbar, &owner) || owner != GetCurrentProcessId()) return;
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}

bool InstallHooks() {
    HMODULE windowsStorage = LoadHookModule(L"windows.storage.dll");
    HMODULE propsys = LoadHookModule(L"propsys.dll");
    HMODULE shell32 = LoadHookModule(L"shell32.dll");
    if (!(windowsStorage && propsys && shell32)) { ReleaseHookModules(); return false; }
    // windows.storage.dll
    const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *)"
#else
                L"public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *)"
#endif
            },
            &g_detailsEx, DetailsExHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CFSFolder::GetDetailsOf(struct _ITEMID_CHILD const __unaligned *,unsigned int,struct _SHELLDETAILS *)"
#else
                L"public: virtual long __stdcall CFSFolder::GetDetailsOf(struct _ITEMID_CHILD const *,unsigned int,struct _SHELLDETAILS *)"
#endif
            },
            &g_detailsOf, DetailsOfHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CShellItem::GetString(struct _tagpropertykey const &,unsigned short * *)"
#else
                L"public: virtual long __stdcall CShellItem::GetString(struct _tagpropertykey const &,unsigned short * *)"
#endif
            },
            &g_itemString, ItemStringHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CShellItem::GetProperty(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#else
                L"public: virtual long __stdcall CShellItem::GetProperty(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#endif
            },
            &g_itemProperty, ItemPropertyHook
        },
    };
    std::array<StoreValue_t, 2> storeTargets{};
    const WindhawkUtils::SYMBOL_HOOK propsysDllHooks[] = {
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CPropertyDescription::FormatForDisplay(struct tagPROPVARIANT const &,enum PROPDESC_FORMAT_FLAGS,unsigned short * *)"
#else
                L"public: virtual long __stdcall CPropertyDescription::FormatForDisplay(struct tagPROPVARIANT const &,enum PROPDESC_FORMAT_FLAGS,unsigned short * *)"
#endif
            },
            &g_typeFormat, TypeFormatHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CPropertyProvider::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#else
                L"public: virtual long __stdcall CPropertyProvider::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#endif
            },
            &storeTargets[0], nullptr
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CMemPropStore::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#else
                L"public: virtual long __stdcall CMemPropStore::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#endif
            },
            &storeTargets[1], nullptr
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CMemPropStore::GetValueAndState(struct _tagpropertykey const &,struct tagPROPVARIANT *,enum PSC_STATE *)"
#else
                L"public: virtual long __stdcall CMemPropStore::GetValueAndState(struct _tagpropertykey const &,struct tagPROPVARIANT *,enum PSC_STATE *)"
#endif
            },
            &g_cacheValue, CacheValueHook
        },
    };
    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CNewMenu::QueryContextMenu(struct HMENU__ *,unsigned int,unsigned int,unsigned int,unsigned int)"
#else
                L"public: virtual long __stdcall CNewMenu::QueryContextMenu(struct HMENU__ *,unsigned int,unsigned int,unsigned int,unsigned int)"
#endif
            },
            &g_newMenuQuery, NewMenuQueryHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CNewMenu::HandleMenuMsg(unsigned int,unsigned __int64,__int64)"
#else
                L"public: virtual long __stdcall CNewMenu::HandleMenuMsg(unsigned int,unsigned int,long)"
#endif
            },
            &g_newMenuMessage, NewMenuMessageHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CNewMenu::HandleMenuMsg2(unsigned int,unsigned __int64,__int64,__int64 *)"
#else
                L"public: virtual long __stdcall CNewMenu::HandleMenuMsg2(unsigned int,unsigned int,long,long *)"
#endif
            },
            &g_newMenuMessage2, NewMenuMessage2Hook
        },
    };
    if (!(WindhawkUtils::HookSymbols(windowsStorage, windowsStorageHooks, ARRAYSIZE(windowsStorageHooks)) &&
          WindhawkUtils::HookSymbols(propsys, propsysDllHooks, ARRAYSIZE(propsysDllHooks)) &&
          WindhawkUtils::HookSymbols(shell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks)))) return false;
    // Multiple native store flags share CPropertyProvider::GetValue. Resolve
    // each implementation once, and also handle identical-code folding.
    const std::array<StoreValue_t, 2> detours{StoreValueHook<0>, StoreValueHook<1>};
    for (size_t i = 0; i < storeTargets.size(); ++i) {
        bool duplicate = false;
        for (size_t j = 0; j < i; ++j) if (storeTargets[i] == storeTargets[j]) duplicate = true;
        if (!duplicate && !WindhawkUtils::SetFunctionHook(storeTargets[i], detours[i], &g_storeValues[i])) return false;
    }
    return WindhawkUtils::SetFunctionHook(AssocQueryStringW, AssocWHook, &g_assocW) &&
           WindhawkUtils::SetFunctionHook(AssocQueryStringA, AssocAHook, &g_assocA) &&
           WindhawkUtils::SetFunctionHook(SHGetFileInfoW, InfoWHook, &g_infoW) &&
           WindhawkUtils::SetFunctionHook(SHGetFileInfoA, InfoAHook, &g_infoA);
}
} // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    bool ok = InstallHooks();
    if (!ok) Wh_Log(L"Required Shell symbols or hooks unavailable; mod will not load");
    return ok;
}
void Wh_ModSettingsChanged() {
    bool changed = LoadSettings();
    bool refreshExplorer = Wh_GetIntSetting(L"refreshExplorer") != 0;
    if (changed && refreshExplorer) NotifyShell();
}
void Wh_ModUninit() {
    std::atomic_store(&g_types, std::make_shared<const TypeMap>());
    { std::lock_guard lock(g_newMenusMutex); g_newMenus.clear(); }
    ReleaseHookModules();
}
