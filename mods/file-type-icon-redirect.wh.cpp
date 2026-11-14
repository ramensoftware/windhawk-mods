// ==WindhawkMod==
// @id              file-type-icon-redirect
// @name            File Type Icon Redirect
// @description     Customize file icons by extension while preserving system thumbnails
// @version         1.2.0
// @author          Momo
// @github          https://github.com/FreshP-0325
// @license         MIT
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lshell32 -lshlwapi -lole32 -luuid
// ==/WindhawkMod==

// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# File Type Icon Redirect

Customize file icons by extension without changing file associations or default
applications. System thumbnails remain enabled and use their original providers.

Before:

![Before](https://raw.githubusercontent.com/FreshP-0325/windhawk-mod-assets/refs/heads/master/windows-file-type-icon-redirect/1.2.0/before.png)

After:

![After](https://raw.githubusercontent.com/FreshP-0325/windhawk-mod-assets/refs/heads/master/windows-file-type-icon-redirect/1.2.0/after.png)

## Configuration

Choose Configuration source:

* Windhawk settings (default): add entries to the icons list. Each entry accepts
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

Icon values support ICO, DLL, EXE, MUN, and other Windows icon resources.
Use path,index; indexes start at 0, while negative values select resource IDs.
For ICO files, omit the index or use 0. Quote a path containing a comma.

```ini
[icons]
.txt,.log=%SystemRoot%\System32\shell32.dll,70
.jpg,.jpeg,.png=icons\image.ico
.pdf="C:\My Icons\documents.exe",-101
```

Icon resources are validated on their first mapped icon request, not while the
mod initializes. Missing or invalid icons fall back to Windows. If an extension
has repeated definitions, the last extractable icon wins. Ordinary directories
are excluded. Existing application icon caches may require reopening windows.

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
- icons:
  - - extensions: ""
      $name: Extensions
      $description: Comma-separated file extensions, including the dot. For example, .txt,.log
    - icon: ""
      $name: Icon
      $description: Absolute icon path, optionally followed by an index. Environment variables are supported.
  $name: File type icons
  $description: Used only in Windhawk settings mode. Fill in the first entry; the list ends at the first entry with both fields empty. Later entries are ignored.
- iniPath: "%USERPROFILE%\\FileTypeIcons\\icons.ini"
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
#include <wrl/client.h>

#include <atomic>
#include <climits>
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
struct IconValidation { std::once_flag once; bool usable = false; };
struct IconLocation {
    std::wstring path;
    int index = 0;
    std::shared_ptr<IconValidation> validation = std::make_shared<IconValidation>();
    bool operator==(const IconLocation& other) const { return path == other.path && index == other.index; }
};
using IconMap = std::unordered_map<std::wstring, std::vector<IconLocation>>;
std::shared_ptr<const IconMap> g_icons = std::make_shared<const IconMap>();
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

bool ParseIndex(std::wstring_view text, int& result) {
    auto s = Trim(text);
    if (s.empty()) return false;
    size_t start = (s[0] == L'-' || s[0] == L'+') ? 1 : 0;
    if (start == s.size()) return false;
    unsigned long long n = 0;
    const auto limit = s[0] == L'-' ? 2147483648ULL : 2147483647ULL;
    for (size_t i = start; i < s.size(); i++) {
        if (s[i] < L'0' || s[i] > L'9') return false;
        n = n * 10 + (s[i] - L'0');
        if (n > limit) return false;
    }
    result = s[0] == L'-' ? static_cast<int>(-static_cast<long long>(n))
                           : static_cast<int>(n);
    return true;
}

std::optional<IconLocation> ParseLocation(std::wstring text,
                                        const std::filesystem::path& base) {
    text = Trim(text);
    IconLocation icon;
    std::wstring path;
    if (!text.empty() && text.front() == L'"') {
        const auto end = text.find(L'"', 1);
        if (end == text.npos) return std::nullopt;
        path = text.substr(1, end - 1);
        auto tail = Trim(std::wstring_view(text).substr(end + 1));
        if (!tail.empty() && (tail.front() != L',' ||
                             !ParseIndex(std::wstring_view(tail).substr(1), icon.index))) {
            return std::nullopt;
        }
    } else {
        const auto comma = text.rfind(L',');
        if (comma != text.npos) {
            if (!ParseIndex(std::wstring_view(text).substr(comma + 1), icon.index)) {
                return std::nullopt;
            }
            text.resize(comma);
        }
        path = Trim(text);
    }
    if (path.empty() || path.find(L'"') != path.npos || icon.index == INT_MIN) {
        return std::nullopt;
    }
    auto expanded = Expand(path);
    if (!expanded || expanded->empty()) return std::nullopt;
    std::filesystem::path p(*expanded);
    // Reject drive-relative and root-relative paths; they depend on host state.
    if (!p.is_absolute()) {
        if (base.empty() || p.has_root_name() || p.has_root_directory()) return std::nullopt;
        p = base / p;
    }
    icon.path = p.lexically_normal().wstring();
    return icon;
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

bool CanExtract(const IconLocation& icon) {
    HICON large = nullptr;
    HRESULT hr = SHDefExtractIconW(icon.path.c_str(), icon.index, 0, &large,
                                   nullptr, 32);
    bool ok = SUCCEEDED(hr) && large;
    if (large) DestroyIcon(large);
    return ok;
}

IconMap ParseIni(const std::wstring& text, const std::filesystem::path& base) {
    IconMap icons;
    bool inSection = false;
    size_t start = 0;
    size_t lineNumber = 0;
    while (start < text.size()) {
        const auto end = text.find(L'\n', start);
        auto line = Trim(std::wstring_view(text).substr(
            start, end == text.npos ? text.size() - start : end - start));
        start = end == text.npos ? text.size() : end + 1;
        ++lineNumber;
        if (line.empty() || line.front() == L';' || line.front() == L'#') continue;
        if (line.front() == L'[') {
            inSection = Lower(line) == L"[icons]";
            continue;
        }
        if (!inSection) continue;
        const auto equals = line.find(L'=');
        auto icon = equals == line.npos ? std::nullopt :
            ParseLocation(line.substr(equals + 1), base);
        if (!icon) {
            Wh_Log(L"INI line %zu: invalid or unreadable icon; skipped", lineNumber);
            continue;
        }
        const auto keys = line.substr(0, equals);
        size_t pos = 0;
        do {
            const auto comma = keys.find(L',', pos);
            auto ext = Lower(Trim(std::wstring_view(keys).substr(
                pos, comma == keys.npos ? keys.size() - pos : comma - pos)));
            if (ValidExtension(ext)) {
                icons[ext].push_back(*icon);
            } else {
                Wh_Log(L"INI line %zu: invalid extension '%s'; skipped", lineNumber,
                       ext.c_str());
            }
            if (comma == keys.npos) break;
            pos = comma + 1;
        } while (pos <= keys.size());
    }
    return icons;
}

bool LoadSettings() {
    IconMap mappings;
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
            auto extensions = WindhawkUtils::StringSetting::make(L"icons[%d].extensions", i);
            auto setting = WindhawkUtils::StringSetting::make(L"icons[%d].icon", i);
            auto keys = Trim(extensions.get());
            auto text = Trim(setting.get());
            if (keys.empty() && text.empty()) break;
            auto value = ParseLocation(text, {});
            if (!value) { Wh_Log(L"Entry %d: invalid icon; skipped", i + 1); continue; }
            size_t pos = 0;
            do {
                auto comma = keys.find(L',', pos);
                auto ext = Lower(Trim(std::wstring_view(keys).substr(pos,
                    comma == keys.npos ? keys.size() - pos : comma - pos)));
                if (ValidExtension(ext)) mappings[ext].push_back(*value);
                else Wh_Log(L"Entry %d: invalid extension; skipped", i + 1);
                if (comma == keys.npos) break;
                pos = comma + 1;
            } while (pos <= keys.size());
        }
    }
    Wh_Log(L"Loaded %zu extension mappings", mappings.size());
    bool changed = mappings != *std::atomic_load(&g_icons);
    std::atomic_store(&g_icons, std::make_shared<const IconMap>(std::move(mappings)));
    return changed;
}

std::optional<IconLocation> LookupPath(PCWSTR path) {
    if (!path) return std::nullopt;
    auto icons = std::atomic_load(&g_icons);
    if (icons->empty()) return std::nullopt;
    auto it = icons->find(Lower(PathFindExtensionW(path)));
    if (it == icons->end()) return std::nullopt;
    for (auto candidate = it->second.rbegin(); candidate != it->second.rend(); ++candidate) {
        std::call_once(candidate->validation->once, [&] { candidate->validation->usable = CanExtract(*candidate); });
        if (candidate->validation->usable) return *candidate;
    }
    return std::nullopt;
}

std::optional<IconLocation> LookupItem(IShellFolder* folder, PCUITEMID_CHILD pidl) {
    if (!pidl || !pidl->mkid.cb || std::atomic_load(&g_icons)->empty()) {
        return std::nullopt;
    }
    STRRET name{};
    if (FAILED(folder->GetDisplayNameOf(pidl,
                static_cast<SHGDNF>(SHGDN_FORPARSING | SHGDN_INFOLDER), &name))) {
        return std::nullopt;
    }
    PWSTR path = nullptr;
    if (FAILED(StrRetToStrW(&name, pidl, &path))) return std::nullopt;
    auto icon = LookupPath(path);
    CoTaskMemFree(path);
    // Do not ask filesystem/cloud providers for attributes of unmapped items.
    if (!icon) return std::nullopt;
    SFGAOF attributes = SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM;
    if (FAILED(folder->GetAttributesOf(1, &pidl, &attributes)) ||
        !(attributes & SFGAO_FILESYSTEM) ||
        ((attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM))) return std::nullopt;
    return icon;
}

bool IsIconIID(REFIID iid) {
    return IsEqualIID(iid, IID_IExtractIconW) || IsEqualIID(iid, IID_IExtractIconA);
}

HRESULT CreateExtractor(const IconLocation& icon, REFIID iid, void** result) {
    ComPtr<IDefaultExtractIconInit> init;
    HRESULT hr = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&init));
    if (FAILED(hr)) return hr;
    // System-owned COM object: no mod-owned vtable survives mod unloading.
    // A filename/index pair is already a stable cache key. Do not mark every
    // file as a per-instance icon: that can create redundant cache identities.
    if (FAILED(hr = init->SetFlags(0)) ||
        FAILED(hr = init->SetNormalIcon(icon.path.c_str(), icon.index)) ||
        FAILED(hr = init->SetOpenIcon(icon.path.c_str(), icon.index)) ||
        FAILED(hr = init->SetDefaultIcon(icon.path.c_str(), icon.index)) ||
        FAILED(hr = init->SetShortcutIcon(icon.path.c_str(), icon.index))) return hr;
    return init->QueryInterface(iid, result);
}

using GetUIObjectOf_t = HRESULT (STDMETHODCALLTYPE*)(
    IShellFolder*, HWND, UINT, PCUITEMID_CHILD_ARRAY, REFIID, UINT*, void**);
using GetIconOf_t = HRESULT (STDMETHODCALLTYPE*)(IShellIcon*, PCUITEMID_CHILD, UINT, int*);
using GetThumbnailHandler_t = HRESULT (STDMETHODCALLTYPE*)(
    IThumbnailHandlerFactory*, PCUITEMID_CHILD, IBindCtx*, REFIID, void**);
GetUIObjectOf_t g_getUIObjectOf = nullptr;
GetIconOf_t g_getIconOf = nullptr;
GetThumbnailHandler_t g_getThumbnailHandler = nullptr;
decltype(&SHCreateFileExtractIconW) g_createFileExtractIcon = nullptr;

HRESULT STDMETHODCALLTYPE GetUIObjectOfHook(IShellFolder* folder, HWND owner,
    UINT count, PCUITEMID_CHILD_ARRAY items, REFIID iid, UINT* reserved, void** result) {
    if (!g_inHook && result && count == 1 && items && IsIconIID(iid)) {
        HookGuard guard;
        if (auto icon = LookupItem(folder, items[0])) {
            if (SUCCEEDED(CreateExtractor(*icon, iid, result))) return S_OK;
        }
    }
    return g_getUIObjectOf(folder, owner, count, items, iid, reserved, result);
}

HRESULT STDMETHODCALLTYPE GetIconOfHook(IShellIcon* shellIcon, PCUITEMID_CHILD item,
                                       UINT flags, int* result) {
    if (!g_inHook && result) {
        HookGuard guard;
        ComPtr<IShellFolder> folder;
        // 'this' is the IShellIcon subobject. QI avoids version-dependent offsets.
        if (SUCCEEDED(shellIcon->QueryInterface(IID_PPV_ARGS(&folder)))) {
            if (auto icon = LookupItem(folder.Get(), item)) {
                int index = Shell_GetCachedImageIndexW(icon->path.c_str(), icon->index, 0);
                if (index >= 0) {
                    *result = index;
                    return S_OK;
                }
            }
        }
    }
    return g_getIconOf(shellIcon, item, flags, result);
}

HRESULT WINAPI CreateFileExtractIconHook(PCWSTR file, DWORD attributes,
                                        REFIID iid, void** result) {
    if (!g_inHook && result && !(attributes & FILE_ATTRIBUTE_DIRECTORY) && IsIconIID(iid)) {
        HookGuard guard;
        if (auto icon = LookupPath(file)) {
            if (SUCCEEDED(CreateExtractor(*icon, iid, result))) return S_OK;
        }
    }
    return g_createFileExtractIcon(file, attributes, iid, result);
}

HRESULT STDMETHODCALLTYPE GetThumbnailHandlerHook(IThumbnailHandlerFactory* factory,
    PCUITEMID_CHILD item, IBindCtx* bindCtx, REFIID iid, void** result) {
    // Despite its name, this factory also serves IExtractIcon requests on modern
    // Windows (including SHGetFileInfo and IShellItemImageFactory icon fallback).
    // Never substitute IThumbnailProvider, IExtractImage, or IUnknown requests.
    if (!g_inHook && result && IsIconIID(iid)) {
        HookGuard guard;
        ComPtr<IShellFolder> folder;
        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&folder)))) {
            if (auto icon = LookupItem(folder.Get(), item)) {
                if (SUCCEEDED(CreateExtractor(*icon, iid, result))) return S_OK;
            }
        }
    }
    return g_getThumbnailHandler(factory, item, bindCtx, iid, result);
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
    if (!windowsStorage) { ReleaseHookModules(); return false; }
    // windows.storage.dll
    const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CFSFolder::GetUIObjectOf(struct HWND__ *,unsigned int,struct _ITEMID_CHILD const __unaligned * const *,struct _GUID const &,unsigned int *,void * *)"
#else
                L"public: virtual long __stdcall CFSFolder::GetUIObjectOf(struct HWND__ *,unsigned int,struct _ITEMID_CHILD const * const *,struct _GUID const &,unsigned int *,void * *)"
#endif
            },
            &g_getUIObjectOf, GetUIObjectOfHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CFSFolder::GetIconOf(struct _ITEMID_CHILD const __unaligned *,unsigned int,int *)"
#else
                L"public: virtual long __stdcall CFSFolder::GetIconOf(struct _ITEMID_CHILD const *,unsigned int,int *)"
#endif
            },
            &g_getIconOf, GetIconOfHook
        },
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl CFSFolder::GetThumbnailHandler(struct _ITEMID_CHILD const __unaligned *,struct IBindCtx *,struct _GUID const &,void * *)"
#else
                L"public: virtual long __stdcall CFSFolder::GetThumbnailHandler(struct _ITEMID_CHILD const *,struct IBindCtx *,struct _GUID const &,void * *)"
#endif
            },
            &g_getThumbnailHandler, GetThumbnailHandlerHook
        },
    };
    if (!WindhawkUtils::HookSymbols(windowsStorage, windowsStorageHooks, ARRAYSIZE(windowsStorageHooks))) return false;
    return WindhawkUtils::SetFunctionHook(SHCreateFileExtractIconW, CreateFileExtractIconHook, &g_createFileExtractIcon);
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
    std::atomic_store(&g_icons, std::make_shared<const IconMap>());
    ReleaseHookModules();
}
