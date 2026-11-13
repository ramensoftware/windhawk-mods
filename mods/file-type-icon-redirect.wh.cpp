// ==WindhawkMod==
// @id              file-type-icon-redirect
// @name            File Type Icon Redirect
// @description     Override file icons by extension using an INI file, preserving system thumbnails
// @version         1.1.1
// @author          Momo
// @github          https://github.com/FreshP-0325
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lshell32 -lshlwapi -lole32 -luuid
// ==/WindhawkMod==

// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# File Type Icon Redirect

Override file icons in File Explorer by extension without changing file
associations or the registry. Windows continues to generate thumbnails for
images, videos, and documents. Mapped icons appear in icon-based views and
when a thumbnail is unavailable.

Set the absolute INI file path in the mod settings. Environment variables are
supported. For example:

```ini
[icons]
.jpg,.jpeg,.png=icons\image.ico
.mp4,.mkv=icons\video.dll,0
.pdf="C:\My Icons\documents.exe",-101
.txt=%SystemRoot%\System32\shell32.dll,70
```

* Supports ICO, DLL, EXE, MUN, and other files from which Windows can extract icons.
* Relative icon paths are resolved against the INI directory.
* Supports UTF-8 with or without a BOM, and UTF-16 LE with a BOM.
* Icon indexes start at 0. Negative indexes specify resource IDs.
  For ICO files, omit the index or use 0.
* Extensions are case-insensitive, must include the dot, and match the last
  extension of a filename.
* Separate multiple extensions with commas. The last valid duplicate entry wins.
* Lines starting with a semicolon or # are comments. Inline comments are unsupported.
* After editing the INI or replacing icon files, add 1 to Reload INI file (for
  example, 0 to 1) and save the settings. Reopen affected windows to see changes.
* Invalid paths or icons that cannot be extracted are skipped and logged.
  Files without a valid mapping keep their original icons.

By default, hooks run in explorer.exe, including the desktop. To apply them to
system Open/Save dialogs in other programs, enable Allow other processes, add
the programs to Custom process inclusion list in Windhawk's advanced settings,
then restart them. Programs that draw their own icons or bypass standard
filesystem Shell interfaces may be unaffected.

Only icon requests are overridden. The mod does not change thumbnail providers
or the "Always show icons, never thumbnails" setting. It sends no global refresh
when loaded or unloaded. Reload INI and refresh Explorer additionally requests
one cache refresh, sent only by the Explorer process that owns the taskbar.
Use it only if old icons remain after reloading. With Resource Redirect,
its resource redirection may also affect the final icon source.

The mod requires a Shell profile matching your Windows system DLLs. Before
installing, run python scripts/prepare.py --mod icon from the repository on your
PC, then compile the prepared source in Windhawk. Repeat after Windows updates
change the relevant DLLs. A mismatched profile prevents the mod from loading.
The profile is embedded in this source; no extra header is needed in Windhawk.

Allow other processes defaults to off. Other apps require both this setting and
a matching Custom process inclusion list entry. This controls hook setup, not
whether Windhawk loads the mod DLL. Compatibility depends on the application.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iniPath: "%USERPROFILE%\\FileTypeIcons\\icons.ini"
  $name: INI file path
  $description: An absolute path, with environment variables supported. Relative icon paths are resolved against the INI directory.
- reloadCounter: 0
  $name: Reload INI file
  $description: After editing the INI file, add 1 to this number (for example, 0 to 1) and save the settings. The mod will read the file again without restarting. Reopen affected windows or menus to see the changes.
- refreshCounter: 0
  $name: Reload INI and refresh Explorer
  $description: Use only if old icons remain after reloading the INI. Add 1 to this number and save the settings to reload the INI and request an Explorer refresh. This can briefly redraw windows and reload thumbnails. Otherwise, leave this value unchanged.
- allowOtherProcesses: false
  $name: Allow other processes
  $description: Also allow hooks in non-Explorer processes selected by Windhawk's inclusion list. Requires both controls. This switch does not prevent DLL loading. Default is Explorer-only.
*/
// ==/WindhawkModSettings==

#ifndef FILE_TYPE_ICON_REDIRECT_TEST
#include <windhawk_api.h>
#endif
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wrl/client.h>

#include <atomic>
#include <climits>
#include <cstring>
#include <filesystem>
#include <memory>
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
#else
[[maybe_unused]] constexpr HookTarget k_iconUI{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1358992u};
[[maybe_unused]] constexpr HookTarget k_iconIndex{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1109328u};
[[maybe_unused]] constexpr HookTarget k_iconFactory{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1107856u};
[[maybe_unused]] constexpr HookTarget k_detailsEx{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1361856u};
[[maybe_unused]] constexpr HookTarget k_detailsOf{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 5342832u};
[[maybe_unused]] constexpr HookTarget k_itemString{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 1684048u};
[[maybe_unused]] constexpr HookTarget k_itemProperty{L"windows.storage.dll", 332u, 3130835471u, 7462912u, 7578709u, 5152304u};
#endif
// END GENERATED SHELL PROFILE

struct IconLocation {
    std::wstring path;
    int index = 0;
};
using IconMap = std::unordered_map<std::wstring, IconLocation>;
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
        if (p.has_root_name() || p.has_root_directory()) return std::nullopt;
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
        if (!icon || !CanExtract(*icon)) {
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
                icons[ext] = *icon;
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

void LoadSettings() {
    PCWSTR setting = Wh_GetStringSetting(L"iniPath");
    auto path = Expand(Trim(setting ? setting : L""));
    Wh_FreeStringSetting(setting);
    IconMap icons;
    std::wstring text;
    if (path && std::filesystem::path(*path).is_absolute() && ReadText(*path, text)) {
        icons = ParseIni(text, std::filesystem::path(*path).parent_path());
    } else {
        Wh_Log(L"INI missing, invalid encoding, too large, or not an absolute path");
    }
    Wh_Log(L"Loaded %zu extension mappings", icons.size());
    std::atomic_store(&g_icons, std::make_shared<const IconMap>(std::move(icons)));
}

std::optional<IconLocation> LookupPath(PCWSTR path) {
    if (!path) return std::nullopt;
    auto icons = std::atomic_load(&g_icons);
    if (icons->empty()) return std::nullopt;
    auto it = icons->find(Lower(PathFindExtensionW(path)));
    if (it == icons->end()) return std::nullopt;
    return it->second;
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
    if (wcscmp(target.module, L"windows.storage.dll") &&
        wcscmp(target.module, L"shell32.dll")) return nullptr;
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

bool InstallHooks() {
    // Resolve and validate all targets before registering any detours.
    void* ui = ResolveHookTarget(k_iconUI);
    void* index = ResolveHookTarget(k_iconIndex);
    void* factory = ResolveHookTarget(k_iconFactory);
    if (!ui || !index || !factory) { ReleaseHookModules(); return false; }
    return Wh_SetFunctionHook(ui, reinterpret_cast<void*>(GetUIObjectOfHook),
                              reinterpret_cast<void**>(&g_getUIObjectOf)) &&
           Wh_SetFunctionHook(index, reinterpret_cast<void*>(GetIconOfHook),
                              reinterpret_cast<void**>(&g_getIconOf)) &&
           Wh_SetFunctionHook(factory, reinterpret_cast<void*>(GetThumbnailHandlerHook),
                              reinterpret_cast<void**>(&g_getThumbnailHandler)) &&
           Wh_SetFunctionHook(reinterpret_cast<void*>(SHCreateFileExtractIconW),
                              reinterpret_cast<void*>(CreateFileExtractIconHook),
                              reinterpret_cast<void**>(&g_createFileExtractIcon));
}
}  // namespace

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
    std::atomic_store(&g_icons, std::make_shared<const IconMap>());
    ReleaseHookModules();
}
