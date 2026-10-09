// ==WindhawkMod==
// @id              this-pc-custom-folders
// @name            This PC Custom Folders
// @description     Add folders and native application, website or protocol shortcuts to This PC
// @version         1.6.6
// @author          Casket Pizza
// @github          https://github.com/CasketPizza
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Make This PC your own

Add your favourite folders, programs, websites and app links directly to **This PC** in File Explorer. Keep the usual drives, storage bars and automatically detected USB devices.

### Getting started

1. Open the mod's **Settings** tab and add an item under **Custom folders**.
2. Enter a **Name** and **Path** (or paste an **Import shortcut string**).
3. For folders, choose a **Group** and optional **Sort Order**. Save your settings and reopen **This PC** if needed.

### What you can add

| Target | Example | Appears in |
| --- | --- | --- |
| Local folder | `C:\Users\Public\Documents` | Selected group |
| Network share | `\\SERVER\Shared` | Selected group |
| Application | `C:\Program Files\Example\app.exe` | Network locations |
| Website | `https://example.com` | Network locations |
| App protocol | `steam://open/main` | Network locations |

Folders use Windows shell registrations. Applications use native `.lnk` shortcuts, while websites and protocols use `.url` shortcuts for normal double-click and Enter behaviour. App and link entries always appear in **Network locations**, regardless of the Group setting.

### Import shortcuts quickly

Enable **Shortcut capture hotkey**, select a `.lnk` or `.url` shortcut in Explorer, then press **Ctrl + Alt + Shift + C**. Paste the copied JSON into an item's **Import shortcut string** field. A populated import string takes priority over its manual Name, Path and Icon fields; the Group selection remains independent.

### Icons and sorting

- **Icon file:** Supply an `.ico` path, or a `.dll` / `.exe` containing icons. For DLL/EXE files, set **Icon reference** to an index (or a negative resource ID). Leave Icon file blank for the automatic icon.
- **Sort Order:** Lower numbers appear earlier within **Folders** and **Network locations**; numbering is independent per group. **Devices and drives** does not support this ordering. Default: `100`.
- Folder groups are **Folders** (the default), **Network locations**, and **Devices and drives**. Arbitrary custom group names aren't supported.

### Notes and cleanup

- Up to **64 entries** are supported. Folder paths must point to real directories; `shell:` locations are not supported as folder targets.
- Entries are managed per Windows user and removed when the mod is disabled or uninstalled. An ownership ledger supports cleanup of older registrations.
- If Explorer displays stale names or icons, close and reopen **This PC**. The mod does not fix file-list refresh issues inside destination folders.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableShortcutCapture: false
  $name: Enable shortcut capture hotkey
  $description: In Explorer, select a .lnk or .url shortcut and press Ctrl+Alt+Shift+C to copy its Name, Target, Icon and Icon Reference as JSON.
- folders:
  - - import: ''
      $name: Import shortcut string
      $description: Paste shortcut JSON copied with Ctrl+Alt+Shift+C. Overrides manual Name, Path and Icon fields; leave empty for manual entry.
    - name: ''
      $name: Name
      $description: 'Example: Documents. Optional when importing a shortcut.'
    - path: ''
      $name: Path
      $description: 'Example: C:\Users\YourName\Documents. Optional when importing a shortcut.'
    - icon: ''
      $name: Icon file
      $description: 'Example: %SystemRoot%\System32\imageres.dll. Supports .ico, .dll and .exe; leave empty to use the automatic icon.'
    - iconRef: ''
      $name: Icon reference
      $description: 'Example: 15 (or -184 for a resource ID). Used for DLL/EXE icons; ignored for .ico files.'
    - args: ''
      $name: Launch arguments
      $description: 'Example: --minimized. Optional for applications; shortcut imports may supply arguments.'
    - workingDir: ''
      $name: Working directory
      $description: 'Example: C:\Program Files\Example. Optional for application launchers.'
    - sortOrder: '100'
      $name: Sort Order
      $description: Lower numbers first within Folders and Network locations. Values are independent per group and can be reused. Not supported in Devices and drives. Defaults to 100.
    - group: folders
      $name: Group (folders only)
      $description: 'Defaults to Folders group if left unselected. Applies only to folder targets; applications and links always appear in Network locations.'
      $options:
        - folders: Folders (default; usually above drives)
        - network: Network locations (below drives)
        - other: Devices and drives
  $name: Custom folders
  $description: Import string overrides manual Name, Path and Icon fields. Group applies only to folders; executable and URL shortcuts always appear in Network locations. Sort Order is not supported in Devices and drives.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <shldisp.h>
#include <objbase.h>
#include <cwctype>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <initializer_list>
#include <set>
#include <algorithm>
#include <unordered_set>

namespace {
// Local CLSID values avoid requiring UUID import libraries in Windhawk's linker.
constexpr GUID kShellWindowsClsid =
    {0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
constexpr GUID kShellLinkClsid =
    {0x00021401, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr int kMaxFolders = 64;
constexpr wchar_t kMarker[] = L"CasketPizza.ThisPCCustomFolders.v1";
constexpr wchar_t kNamespaceRoot[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace\\";
constexpr wchar_t kClassesRoot[] = L"Software\\Classes\\CLSID\\";
constexpr wchar_t kLedgerRoot[] = L"Software\\CasketPizza\\ThisPCCustomFolders\\Managed";
constexpr wchar_t kFolderHandler[] = L"{0E5AAE11-A475-4c5b-AB00-C66DE400274E}";

struct Folder { std::wstring name, path, icon, iconRef, group, args, workingDir; DWORD sortOrder = 100; };

// Legacy IDs (v1.0-v1.5.1) are position-based; retained solely for migration/cleanup.
std::wstring GetId(int i) {
    wchar_t buf[64];
    swprintf_s(buf, L"{EFB8C3A1-82E1-4D91-B4E0-%012llX}",
              0xC45700000000ULL + static_cast<unsigned>(i));
    return buf;
}

// A different shell identity for different item metadata prevents Explorer from
// retaining the display name/icon of a previous occupant of a settings row.
// Deterministic across restarts and insensitive to list reordering.
std::wstring StableId(const Folder& f) {
    std::wstring fingerprint = f.name + L"\x1f" + f.path + L"\x1f" +
        f.icon + L"\x1f" + f.iconRef + L"\x1f" + f.group;
    unsigned long long hash = 14695981039346656037ULL;
    for (wchar_t c : fingerprint) {
        unsigned v = static_cast<unsigned>(towlower(c));
        hash ^= v & 0xff; hash *= 1099511628211ULL;
        hash ^= (v >> 8) & 0xff; hash *= 1099511628211ULL;
    }
    wchar_t buf[64];
    // Full 64-bit hash; separate prefix from previously shipped positional GUIDs.
    swprintf_s(buf, L"{D0B7E152-9C52-458A-%04X-%012llX}",
               static_cast<unsigned>((hash >> 48) & 0xFFFF),
               hash & 0xFFFFFFFFFFFFULL);
    return buf;
}

std::wstring GetSetting(PCWSTR key, int i) {
    PCWSTR s = Wh_GetStringSetting(key, i);
    std::wstring result = s ? s : L"";
    if (s) Wh_FreeStringSetting(s);
    return result;
}

std::wstring Expand(std::wstring s) {
    if (s.empty()) return s;
    DWORD required = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!required || required > 32768) return s;
    std::wstring out(required, L'\0');
    DWORD written = ExpandEnvironmentStringsW(s.c_str(), out.data(), required);
    if (!written || written > required) return s;
    out.resize(written - 1);
    return out;
}

// A small strict JSON reader for shortcut records. No third-party dependency.
// Accepted keys: name, path, icon, ref, iconRef, args, workingDir.
struct JsonReader {
    const std::wstring& input;
    size_t at = 0;
    explicit JsonReader(const std::wstring& s) : input(s) {}
    void Space() { while (at < input.size() && iswspace(input[at])) ++at; }
    bool Eat(wchar_t ch) { Space(); if (at < input.size() && input[at] == ch) { ++at; return true; } return false; }
    static int Hex(wchar_t c) {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    }
    bool String(std::wstring& value) {
        Space();
        if (at >= input.size() || input[at++] != L'"') return false;
        value.clear();
        while (at < input.size()) {
            wchar_t ch = input[at++];
            if (ch == L'"') return true;
            if (ch < 0x20) return false;
            if (ch != L'\\') { value += ch; continue; }
            if (at == input.size()) return false;
            wchar_t escaped = input[at++];
            switch (escaped) {
                case L'"': value += L'"'; break;
                case L'\\': value += L'\\'; break;
                case L'/': value += L'/'; break;
                case L'b': value += L'\b'; break;
                case L'f': value += L'\f'; break;
                case L'n': value += L'\n'; break;
                case L'r': value += L'\r'; break;
                case L't': value += L'\t'; break;
                case L'u': {
                    if (at + 4 > input.size()) return false;
                    int n = 0;
                    for (int i = 0; i < 4; ++i) {
                        int x = Hex(input[at++]);
                        if (x < 0) return false;
                        n = n * 16 + x;
                    }
                    value += static_cast<wchar_t>(n);
                    break;
                }
                default: return false;
            }
        }
        return false;
    }
    bool Value(std::wstring& value) {
        Space();
        if (at < input.size() && input[at] == L'"') return String(value);
        size_t begin = at;
        if (at < input.size() && input[at] == L'-') ++at;
        size_t digits = at;
        while (at < input.size() && input[at] >= L'0' && input[at] <= L'9') ++at;
        if (at == digits) return false;
        value = input.substr(begin, at - begin);
        return true;
    }
};

bool ParseImport(const std::wstring& json, Folder& f) {
    JsonReader r(json);
    if (!r.Eat(L'{')) return false;
    bool seenName = false, seenPath = false;
    f.name.clear(); f.path.clear(); f.icon.clear(); f.iconRef.clear();
    f.args.clear(); f.workingDir.clear();
    if (r.Eat(L'}')) return false;
    do {
        std::wstring key, value;
        if (!r.String(key) || !r.Eat(L':') || !r.Value(value)) return false;
        if (key == L"name") { f.name = value; seenName = true; }
        else if (key == L"path") { f.path = value; seenPath = true; }
        else if (key == L"icon") f.icon = value;
        else if (key == L"ref" || key == L"iconRef") f.iconRef = value;
        else if (key == L"args") f.args = value;
        else if (key == L"workingDir") f.workingDir = value;
        if (r.Eat(L'}')) break;
        if (!r.Eat(L',')) return false;
    } while (true);
    r.Space();
    return r.at == json.size() && seenName && seenPath && !f.name.empty() && !f.path.empty();
}

bool Load(std::vector<Folder>& folders) {
    folders.clear();
    for (int i = 0; i < kMaxFolders; ++i) {
        Folder f;
        const std::wstring imported = GetSetting(L"folders[%d].import", i);
        f.group = GetSetting(L"folders[%d].group", i);
        {
            const std::wstring raw = GetSetting(L"folders[%d].sortOrder", i);
            if (!raw.empty()) {
                const wchar_t* begin = raw.c_str();
                wchar_t* end = nullptr;
                errno = 0;
                unsigned long value = wcstoul(begin, &end, 10);
                while (end && (*end == L' ' || *end == L'\t')) ++end;
                if (begin == end || !end || *end || errno == ERANGE || value > 65535 || raw[0] == L'-') {
                    Wh_Log(L"Invalid Sort Order for item %d; using 100", i + 1);
                } else {
                    f.sortOrder = static_cast<DWORD>(value);
                }
            }
        }
        if (!imported.empty()) {
            if (!ParseImport(imported, f)) {
                Wh_Log(L"Invalid import JSON at folder %d: existing registry configuration preserved", i + 1);
                return false;
            }
            f.group = GetSetting(L"folders[%d].group", i);
            f.path = Expand(f.path);
            f.icon = Expand(f.icon);
            f.workingDir = Expand(f.workingDir);
        } else {
            f.name = GetSetting(L"folders[%d].name", i);
            f.path = Expand(GetSetting(L"folders[%d].path", i));
            f.icon = Expand(GetSetting(L"folders[%d].icon", i));
            f.iconRef = GetSetting(L"folders[%d].iconRef", i);
            f.args = GetSetting(L"folders[%d].args", i);
            f.workingDir = Expand(GetSetting(L"folders[%d].workingDir", i));
        }
        if (f.path.empty()) continue;
        // The Windhawk UI can store an empty group for new or migrated rows.
        // Never silently place a folder in Network locations as a fallback.
        if (f.group != L"folders" && f.group != L"network" &&
            f.group != L"other") {
            Wh_Log(L"Unknown or empty group at item %d; using Folders", i + 1);
            f.group = L"folders";
        }
        if (f.name.empty()) {
            size_t end = f.path.find_last_not_of(L"\\/");
            if (end != std::wstring::npos) {
                size_t begin = f.path.find_last_of(L"\\/", end);
                f.name = f.path.substr(begin == std::wstring::npos ? 0 : begin + 1,
                    end - (begin == std::wstring::npos ? 0 : begin + 1) + 1);
            }
        }
        if (f.name.empty()) f.name = L"Folder";
        folders.push_back(std::move(f));
    }
    return true;
}

// Resolve an icon file + optional DLL/EXE reference into a shell icon location.
// .ico files use their path only; the reference is intentionally ignored.
std::wstring IconLocation(const Folder& f) {
    std::wstring path = f.icon;
    if (path.empty()) return L"";
    const size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos) return path;
    std::wstring extension = path.substr(dot);
    if (_wcsicmp(extension.c_str(), L".dll") != 0 &&
        _wcsicmp(extension.c_str(), L".exe") != 0) return path;

    // Windhawk string settings are user-entered; only accept a valid integer.
    std::wstring ref = f.iconRef;
    const size_t first = ref.find_first_not_of(L" \t");
    if (first == std::wstring::npos) return path;
    const size_t last = ref.find_last_not_of(L" \t");
    ref = ref.substr(first, last - first + 1);
    size_t pos = (ref[0] == L'-' || ref[0] == L'+') ? 1 : 0;
    if (pos == ref.size()) return path;
    for (; pos < ref.size(); ++pos) {
        if (ref[pos] < L'0' || ref[pos] > L'9') return path;
    }
    return path + L"," + ref;
}

// File-system directories keep shell namespace registration; launchers use real
// shortcuts under Network Shortcuts. Unknown targets are rejected safely.
bool StartsWith(const std::wstring& s, const wchar_t* prefix) {
    return _wcsnicmp(s.c_str(), prefix, wcslen(prefix)) == 0;
}
bool IsUri(const std::wstring& s) {
    size_t colon = s.find(L':');
    if (colon == std::wstring::npos || colon < 2 || colon > 32 || !iswalpha(s[0])) return false;
    if (colon == 1) return false;
    for (size_t i=1; i<colon; ++i)
        if (!iswalnum(s[i]) && s[i]!=L'+' && s[i]!=L'-' && s[i]!=L'.') return false;
    // No unquoted spaces/control characters in protocol or web URI.
    return s.find_first_of(L" \t\r\n\"", colon) == std::wstring::npos;
}
bool DirectoryExists(const std::wstring& s) {
    DWORD a = GetFileAttributesW(s.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}
bool FileExists(const std::wstring& s) {
    DWORD a = GetFileAttributesW(s.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}
bool IsExe(const std::wstring& path) {
    auto dot = path.find_last_of(L'.');
    return dot != std::wstring::npos && !_wcsicmp(path.c_str()+dot, L".exe");
}
// Never turn missing UNC paths into accidental shortcuts.
enum class EntryKind { Folder, Executable, Uri, Invalid };
EntryKind Kind(const Folder& f) {
    if (IsUri(f.path)) return EntryKind::Uri;
    if (DirectoryExists(f.path)) return EntryKind::Folder;
    if (FileExists(f.path) && IsExe(f.path)) return EntryKind::Executable;
    if (StartsWith(f.path, L"\\\\") && !FileExists(f.path)) return EntryKind::Folder;
    return EntryKind::Invalid;
}

bool PutString(HKEY key, LPCWSTR name, const std::wstring& value,
               DWORD type = REG_SZ) {
    return RegSetValueExW(key, name, 0, type,
                          reinterpret_cast<const BYTE*>(value.c_str()),
                          static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t))) == ERROR_SUCCESS;
}

bool PutDword(HKEY key, LPCWSTR name, DWORD value) {
    return RegSetValueExW(key, name, 0, REG_DWORD,
                          reinterpret_cast<const BYTE*>(&value), sizeof(value)) == ERROR_SUCCESS;
}

bool OpenKey(const std::wstring& path, HKEY* key) {
    return RegCreateKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, nullptr, 0,
                           KEY_READ | KEY_WRITE, nullptr, key, nullptr) == ERROR_SUCCESS;
}

void SetStringAt(const std::wstring& path, LPCWSTR name,
                 const std::wstring& val, DWORD type = REG_SZ) {
    HKEY key;
    if (OpenKey(path, &key)) {
        if (!PutString(key, name, val, type)) Wh_Log(L"Unable to write %s", path.c_str());
        RegCloseKey(key);
    } else Wh_Log(L"Unable to create %s", path.c_str());
}

void SetDwordAt(const std::wstring& path, LPCWSTR name, DWORD val) {
    HKEY key;
    if (OpenKey(path, &key)) {
        if (!PutDword(key, name, val)) Wh_Log(L"Unable to write %s", path.c_str());
        RegCloseKey(key);
    } else Wh_Log(L"Unable to create %s", path.c_str());
}

bool ReadStringAt(const std::wstring& path, LPCWSTR name,
                  std::wstring* output) {
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, KEY_READ,
                      &key) != ERROR_SUCCESS) return false;
    wchar_t value[2048]{};
    DWORD cb = sizeof(value), type = 0;
    LONG result = RegQueryValueExW(key, name, nullptr, &type,
                                   reinterpret_cast<BYTE*>(value), &cb);
    RegCloseKey(key);
    if (result != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ) ||
        cb < sizeof(wchar_t) || cb > sizeof(value)) return false;
    value[(cb / sizeof(wchar_t)) - 1] = L'\0';
    *output = value;
    return true;
}

bool KeyExists(const std::wstring& path) {
    HKEY key;
    LONG status = RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0,
                                KEY_READ, &key);
    if (status == ERROR_SUCCESS) RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

// The ledger survives settings edits: changing or removing a row does not
// erase the information needed to clean up its previous registration.
bool ListedAsOurs(const std::wstring& id) {
    std::wstring marker;
    return ReadStringAt(kLedgerRoot, id.c_str(), &marker) && marker == kMarker;
}

bool HasOwnerMarker(const std::wstring& cls) {
    std::wstring marker;
    return ReadStringAt(cls, L"ThisPCCustomFoldersOwner", &marker) &&
           marker == kMarker;
}

// For v1.0-v1.1 leftovers, accept only our reserved GUID *and* their actual
// shell-folder handler/TargetFolderPath layout.  Never remove an arbitrary
// registry class merely because it has an ID in our numeric range.
bool LooksLikeLegacyEntry(const std::wstring& id) {
    std::wstring cls = std::wstring(kClassesRoot) + id;
    std::wstring ns = std::wstring(kNamespaceRoot) + id;
    std::wstring handler, target;
    return KeyExists(ns) &&
           ReadStringAt(cls + L"\\Instance", L"CLSID", &handler) &&
           _wcsicmp(handler.c_str(), kFolderHandler) == 0 &&
           ReadStringAt(cls + L"\\Instance\\InitPropertyBag",
                        L"TargetFolderPath", &target) && !target.empty();
}

bool TrackedId(const std::wstring& id) {
    const std::wstring cls = std::wstring(kClassesRoot) + id;
    return ListedAsOurs(id) || HasOwnerMarker(cls) || LooksLikeLegacyEntry(id);
}

void DeleteTreeLogged(const std::wstring& path) {
    LONG status = RegDeleteTreeW(HKEY_CURRENT_USER, path.c_str());
    if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND &&
        status != ERROR_PATH_NOT_FOUND)
        Wh_Log(L"Registry removal failed (%ld): %s", status, path.c_str());
}

void RemoveId(const std::wstring& id) {
    if (!TrackedId(id)) return;
    const std::wstring cls = std::wstring(kClassesRoot) + id;
    const std::wstring ns = std::wstring(kNamespaceRoot) + id;
    DeleteTreeLogged(ns);
    DeleteTreeLogged(cls);
    if (!KeyExists(cls) && !KeyExists(ns)) {
        HKEY ledger = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kLedgerRoot, 0, KEY_SET_VALUE,
                          &ledger) == ERROR_SUCCESS) {
            RegDeleteValueW(ledger, id.c_str());
            RegCloseKey(ledger);
        }
    }
}
void RemoveEntry(int i) { RemoveId(GetId(i)); }

// Read managed GUIDs from the independent ownership ledger. Never enumerate
// arbitrary HKCU CLSIDs for deletion. Keep the list separate while deleting.
std::vector<std::wstring> ManagedIds() {
    std::vector<std::wstring> ids;
    HKEY ledger = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kLedgerRoot, 0, KEY_READ,
                      &ledger) != ERROR_SUCCESS) return ids;
    for (DWORD i = 0; ; ++i) {
        wchar_t name[256]{}; DWORD len = ARRAYSIZE(name);
        LONG result = RegEnumValueW(ledger, i, name, &len, nullptr,
                                    nullptr, nullptr, nullptr);
        if (result == ERROR_NO_MORE_ITEMS) break;
        if (result != ERROR_SUCCESS) { Wh_Log(L"Ledger enumeration error: %ld", result); break; }
        std::wstring id(name, len);
        if (id.size() == 38 && id.front() == L'{' && id.back() == L'}')
            ids.push_back(id);
    }
    RegCloseKey(ledger);
    return ids;
}

bool MarkManaged(const std::wstring& id) {
    HKEY key = nullptr;
    if (!OpenKey(kLedgerRoot, &key)) return false;
    bool ok = PutString(key, id.c_str(), kMarker);
    RegCloseKey(key);
    return ok;
}

// Launcher filenames follow the display name. An independent per-slot ledger
// tracks their previous filenames for cleanup when names change.
std::wstring ShortcutFolder() {
    wchar_t path[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path))) return L"";
    return std::wstring(path) + L"\\Microsoft\\Windows\\Network Shortcuts";
}
std::wstring SafeName(std::wstring name) {
    for (auto& ch : name) {
        if (wcschr(L"<>:\"/\\|?*", ch) || ch < 32) ch = L'_';
    }
    while (!name.empty() && (name.back() == L'.' || name.back() == L' ')) name.pop_back();
    if (name.empty()) name = L"Shortcut";
    if (name.size() > 100) name.resize(100);
    return name;
}
std::wstring ShortcutPath(int index, bool url, const std::wstring& name) {
    const std::wstring dir = ShortcutFolder();
    if (dir.empty()) return L"";
    // Per-slot ledger holds exact last created filename; no GUID exposed in UI.
    return dir + L"\\" + SafeName(name) + (url ? L".url" : L".lnk");
}
std::wstring LedgerShortcutName(int i) {
    std::wstring name;
    ReadStringAt(kLedgerRoot, (L"Shortcut_" + std::to_wstring(i)).c_str(), &name);
    return name;
}
// Use the shortcut's own metadata as an ownership guard. Even a user-created
// file with the same filename is not removed unless it carries our marker.
struct ScopedCom {
    HRESULT hr;
    ScopedCom() : hr(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)) {}
    ~ScopedCom() { if (hr == S_OK || hr == S_FALSE) CoUninitialize(); }
};
bool OwnedShortcut(const std::wstring& path, bool url) {
    ScopedCom com;
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return false;
    if (url) {
        wchar_t marker[128]{};
        GetPrivateProfileStringW(L"InternetShortcut", L"ThisPCCustomFoldersOwner", L"",
                                 marker, ARRAYSIZE(marker), path.c_str());
        return wcscmp(marker, kMarker) == 0;
    }
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(kShellLinkClsid, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) return false;
    bool owned = false;
    IPersistFile* persistence = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persistence)))) {
        if (SUCCEEDED(persistence->Load(path.c_str(), STGM_READ))) {
            wchar_t description[256]{};
            if (SUCCEEDED(link->GetDescription(description, ARRAYSIZE(description))))
                owned = wcscmp(description, kMarker) == 0;
        }
        persistence->Release();
    }
    link->Release();
    return owned;
}
void RemoveShortcut(int i) {
    auto name = LedgerShortcutName(i);
    if (name.empty()) return;
    for (bool url : {false, true}) {
        const auto path = ShortcutPath(i, url, name);
        if (!path.empty() && OwnedShortcut(path, url) && !DeleteFileW(path.c_str()))
            Wh_Log(L"Unable to remove managed shortcut %s (%lu)", path.c_str(), GetLastError());
    }
    // Keep ledger if an owned file persists, to allow retry on next reload.
    bool remaining = false;
    for (bool url : {false, true}) remaining |= OwnedShortcut(ShortcutPath(i,url,name),url);
    if (!remaining) {
        HKEY key = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,kLedgerRoot,0,KEY_SET_VALUE,&key)==ERROR_SUCCESS) {
            RegDeleteValueW(key,(L"Shortcut_"+std::to_wstring(i)).c_str());
            RegCloseKey(key);
        }
    }
}
void RemoveAll(int i) { RemoveEntry(i); RemoveShortcut(i); }
bool CreateLauncher(int i, const Folder& f, EntryKind kind) {
    ScopedCom com;
    const bool url = kind == EntryKind::Uri;
    auto path = ShortcutPath(i, url, f.name);
    if (path.empty()) return false;
    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Launcher filename collision (not overwriting): %s", path.c_str());
        return false;
    }
    // Never overwrite another user shortcut with the same name.
    if (url) {
        // Windows Internet Shortcut .url handles http(s) and installed protocols.
        // Stage the entire internet shortcut first. Failed writes never leave an
        // unowned or half-populated file at the final destination.
        const std::wstring staging = path + L".tmp." + std::to_wstring(GetCurrentProcessId()) +
            L"." + std::to_wstring(GetCurrentThreadId());
        if (GetFileAttributesW(staging.c_str()) != INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Staging file already exists: %s", staging.c_str());
            return false;
        }
        bool ok = WritePrivateProfileStringW(L"InternetShortcut", L"URL", f.path.c_str(), staging.c_str()) &&
            WritePrivateProfileStringW(L"InternetShortcut", L"ThisPCCustomFoldersOwner", kMarker, staging.c_str());
        if (ok && !f.icon.empty()) {
            ok = WritePrivateProfileStringW(L"InternetShortcut", L"IconFile", f.icon.c_str(), staging.c_str()) &&
                 WritePrivateProfileStringW(L"InternetShortcut", L"IconIndex",
                     f.iconRef.empty() ? L"0" : f.iconRef.c_str(), staging.c_str());
        }
        if (ok) ok = WritePrivateProfileStringW(nullptr, nullptr, nullptr, staging.c_str());
        if (ok) ok = MoveFileExW(staging.c_str(), path.c_str(), MOVEFILE_WRITE_THROUGH);
        if (!ok) {
            Wh_Log(L"Could not publish URL shortcut: %s (%lu)", path.c_str(), GetLastError());
            DeleteFileW(staging.c_str());
            return false;
        }
        // Shortcut display name uses filename; Windows hides .url extension.
    } else {
        IShellLinkW* link = nullptr;
        if (FAILED(CoCreateInstance(kShellLinkClsid, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(&link)))) return false;
        link->SetPath(f.path.c_str());
        link->SetDescription(kMarker);
        if (!f.args.empty()) link->SetArguments(f.args.c_str());
        if (!f.workingDir.empty()) link->SetWorkingDirectory(f.workingDir.c_str());
        if (!f.icon.empty()) link->SetIconLocation(f.icon.c_str(), _wtoi(f.iconRef.c_str()));
        IPersistFile* persistence = nullptr;
        HRESULT hr = E_FAIL;
        if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persistence)))) {
            // Save to a staged file and publish only when the shortcut is complete.
            const std::wstring staging = path + L".tmp." +
                std::to_wstring(GetCurrentProcessId()) + L"." +
                std::to_wstring(GetCurrentThreadId());
            if (GetFileAttributesW(staging.c_str()) == INVALID_FILE_ATTRIBUTES) {
                hr = persistence->Save(staging.c_str(), TRUE);
                if (SUCCEEDED(hr) && !MoveFileExW(staging.c_str(), path.c_str(), MOVEFILE_WRITE_THROUGH))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                if (FAILED(hr)) DeleteFileW(staging.c_str());
            }
            persistence->Release();
        }
        link->Release();
        if (FAILED(hr)) return false;
    }
    SetStringAt(kLedgerRoot, (L"Shortcut_" + std::to_wstring(i)).c_str(), f.name);
    Wh_Log(L"Created managed Network locations shortcut: %s -> %s", f.name.c_str(), f.path.c_str());
    return true;
}
void NotifyExplorer() {
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    // Request an update for This PC itself, not only the shortcut directory.
    PIDLIST_ABSOLUTE computer = nullptr;
    if (SUCCEEDED(SHParseDisplayName(
            L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}",
            nullptr, &computer, 0, nullptr))) {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST, computer, nullptr);
        CoTaskMemFree(computer);
    }
    const auto dir = ShortcutFolder();
    if (!dir.empty()) SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, dir.c_str(), nullptr);
}
void Apply() {
    std::vector<Folder> folders;
    if (!Load(folders)) return; // Keep valid current entries on invalid JSON.

    // Reject duplicate folder identities and launcher filenames before making changes.
    std::set<std::wstring> folderIds;
    std::set<std::wstring> launcherNames;
    for (size_t i = 0; i < folders.size(); ++i) {
        const Folder& f = folders[i];
        const EntryKind kind = Kind(f);
        if (kind == EntryKind::Folder) {
            if (!folderIds.insert(StableId(f)).second) {
                Wh_Log(L"Duplicate folder entry %s; keeping existing registrations", f.name.c_str());
                return;
            }
        } else if (kind == EntryKind::Executable || kind == EntryKind::Uri) {
            std::wstring key = SafeName(f.name) +
                (kind == EntryKind::Uri ? L".url" : L".lnk");
            std::transform(key.begin(), key.end(), key.begin(), [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
            if (!launcherNames.insert(key).second) {
                Wh_Log(L"Duplicate launcher name %s; keeping existing registrations", f.name.c_str());
                return;
            }
        }
    }
    std::set<std::wstring> desired;
    for (const Folder& f : folders)
        if (Kind(f) == EntryKind::Folder) desired.insert(StableId(f));

    // Migrate/remove all old position-based registrations. Launcher shortcuts
    // still use the per-row cleanup ledger but never share the folder CLSID.
    for (int i = 0; i < kMaxFolders; ++i) RemoveAll(i);
    for (const auto& id : ManagedIds())
        if (!desired.count(id)) RemoveId(id);

    for (size_t i = 0; i < folders.size(); ++i) {
        const Folder& f = folders[i];
        EntryKind kind = Kind(f);
        if (kind == EntryKind::Invalid) {
            Wh_Log(L"Skipped invalid or unsupported target: %s", f.path.c_str());
            continue;
        }
        if (kind != EntryKind::Folder) {
            if (f.group != L"network")
                Wh_Log(L"Group ignored for launcher '%s': Network locations only", f.name.c_str());
            CreateLauncher(static_cast<int>(i), f, kind);
            continue;
        }
        const std::wstring id = StableId(f);
        const std::wstring cls = std::wstring(kClassesRoot) + id;
        const std::wstring ns = std::wstring(kNamespaceRoot) + id;
        if (KeyExists(cls) || KeyExists(ns)) {
            if (!TrackedId(id)) {
                Wh_Log(L"Unowned CLSID collision (skipped): %s", id.c_str());
                continue;
            }
            // The sort order is intentionally excluded from StableId: update it
            // in place without forcing a new shell identity or cache churn.
            if (KeyExists(cls) && KeyExists(ns)) {
                HKEY key = nullptr;
                if (RegOpenKeyExW(HKEY_CURRENT_USER, cls.c_str(), 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
                    if (!PutDword(key, L"SortOrderIndex", f.sortOrder))
                        Wh_Log(L"Could not update Sort Order for %s", f.name.c_str());
                    RegCloseKey(key);
                } else {
                    Wh_Log(L"Could not open existing entry to update Sort Order: %s", f.name.c_str());
                }
            }
            continue;
        }
        if (!MarkManaged(id)) {
            Wh_Log(L"Cannot record ownership of %s; skipping", id.c_str());
            continue;
        }
        bool ok = true;
        auto regString = [&](const std::wstring& path, LPCWSTR name,
                             const std::wstring& value, DWORD type = REG_SZ) {
            HKEY key = nullptr;
            if (!OpenKey(path, &key)) { ok = false; return; }
            if (!PutString(key, name, value, type)) ok = false;
            RegCloseKey(key);
        };
        auto regDword = [&](const std::wstring& path, LPCWSTR name, DWORD value) {
            HKEY key = nullptr;
            if (!OpenKey(path, &key)) { ok = false; return; }
            if (!PutDword(key, name, value)) ok = false;
            RegCloseKey(key);
        };
        regString(cls, nullptr, f.name);
        regString(cls, L"ThisPCCustomFoldersOwner", kMarker);
        regDword(cls, L"System.IsPinnedToNameSpaceTree", 0);
        regDword(cls, L"SortOrderIndex", f.sortOrder);
        DWORD descriptionId = f.group == L"network" ? 9 : f.group == L"other" ? 0 : 3;
        regDword(cls, L"DescriptionID", descriptionId);
        regString(cls + L"\\InProcServer32", nullptr,
                    L"%SystemRoot%\\System32\\shell32.dll", REG_EXPAND_SZ);
        regString(cls + L"\\InProcServer32", L"ThreadingModel", L"Both");
        regString(cls + L"\\Instance", L"CLSID", kFolderHandler);
        regDword(cls + L"\\Instance\\InitPropertyBag", L"Attributes", 0x11);
        regString(cls + L"\\Instance\\InitPropertyBag", L"TargetFolderPath", f.path);
        regDword(cls + L"\\ShellFolder", L"Attributes", 0xF080004D);
        regDword(cls + L"\\ShellFolder", L"FolderValueFlags", 0x28);
        const std::wstring icon = IconLocation(f);
        regString(cls + L"\\DefaultIcon", nullptr, icon.empty() ?
                    L"%SystemRoot%\\System32\\shell32.dll,3" : icon, REG_EXPAND_SZ);
        if (ok) regString(ns, nullptr, f.name); // Publish only when class is complete.
        if (!ok) {
            Wh_Log(L"Failed to register %s; rolling back partial registration", id.c_str());
            RemoveId(id);
        }
    }
    NotifyExplorer();
}
void Cleanup() {
    for (int i = 0; i < kMaxFolders; ++i) RemoveAll(i);
    for (const auto& id : ManagedIds()) RemoveId(id);
    NotifyExplorer();
}

// Optional per-user hotkey. COM access stays on this dedicated STA thread.
constexpr int kHotkey = 0x4C43;
HANDLE gHotkeyThread = nullptr;
DWORD gHotkeyThreadId = 0;

std::wstring EscapeJson(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        if (c == L'"' || c == L'\\') { out += L'\\'; out += c; }
        else if (c == L'\n') out += L"\\n";
        else if (c == L'\r') out += L"\\r";
        else if (c == L'\t') out += L"\\t";
        else if (c < 0x20) {
            wchar_t buf[7]; swprintf_s(buf, L"\\u%04X", static_cast<unsigned>(c)); out += buf;
        } else out += c;
    }
    return out;
}

bool PutClipboardText(const std::wstring& text) {
    if (!OpenClipboard(nullptr)) return false;
    if (!EmptyClipboard()) { CloseClipboard(); return false; }
    SIZE_T bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!mem) { CloseClipboard(); return false; }
    void* ptr = GlobalLock(mem);
    if (!ptr) { GlobalFree(mem); CloseClipboard(); return false; }
    memcpy(ptr, text.c_str(), bytes);
    GlobalUnlock(mem);
    if (!SetClipboardData(CF_UNICODETEXT, mem)) { GlobalFree(mem); CloseClipboard(); return false; }
    CloseClipboard();
    return true;
}

// The shell's selected item is retrieved from the foreground Explorer window.
bool SelectedShortcut(std::wstring& shortcutPath) {
    IShellWindows* windows = nullptr;
    HRESULT hr = CoCreateInstance(kShellWindowsClsid, nullptr, CLSCTX_LOCAL_SERVER,
                                  IID_PPV_ARGS(&windows));
    if (FAILED(hr)) return false;
    HWND foreground = GetAncestor(GetForegroundWindow(), GA_ROOT);
    long count = 0;
    windows->get_Count(&count);
    bool found = false;
    for (long n = 0; n < count && !found; ++n) {
        VARIANT index; VariantInit(&index); index.vt = VT_I4; index.lVal = n;
        IDispatch* dispatch = nullptr;
        if (FAILED(windows->Item(index, &dispatch)) || !dispatch) continue;
        IWebBrowser2* browser = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS(&browser)))) {
            SHANDLE_PTR browserHandle = 0;
            if (SUCCEEDED(browser->get_HWND(&browserHandle)) &&
                GetAncestor(reinterpret_cast<HWND>(browserHandle), GA_ROOT) == foreground) {
                IDispatch* document = nullptr;
                if (SUCCEEDED(browser->get_Document(&document)) && document) {
                    IShellFolderViewDual* view = nullptr;
                    if (SUCCEEDED(document->QueryInterface(IID_PPV_ARGS(&view)))) {
                        FolderItems* items = nullptr;
                        if (SUCCEEDED(view->SelectedItems(&items)) && items) {
                            long selectedCount = 0;
                            items->get_Count(&selectedCount);
                            if (selectedCount == 1) {
                                VARIANT zero; VariantInit(&zero); zero.vt = VT_I4; zero.lVal = 0;
                                FolderItem* item = nullptr;
                                if (SUCCEEDED(items->Item(zero, &item)) && item) {
                                    BSTR path = nullptr;
                                    if (SUCCEEDED(item->get_Path(&path)) && path) {
                                        shortcutPath.assign(path, SysStringLen(path));
                                        SysFreeString(path);
                                        found = true;
                                    }
                                    item->Release();
                                }
                            }
                            items->Release();
                        }
                        view->Release();
                    }
                    document->Release();
                }
            }
            browser->Release();
        }
        dispatch->Release();
    }
    windows->Release();
    if (found && shortcutPath.size() >= 4 &&
        (_wcsicmp(shortcutPath.c_str() + shortcutPath.size() - 4, L".lnk") == 0 ||
         _wcsicmp(shortcutPath.c_str() + shortcutPath.size() - 4, L".url") == 0)) return true;
    return false;
}

void CaptureShortcut() {
    std::wstring file;
    if (!SelectedShortcut(file)) {
        Wh_Log(L"Shortcut capture: select exactly one .lnk or .url file in Explorer");
        return;
    }
    if (file.size() >= 4 && _wcsicmp(file.c_str() + file.size()-4, L".url") == 0) {
        wchar_t url[32768]{}, icon[32768]{}, index[32]{};
        GetPrivateProfileStringW(L"InternetShortcut", L"URL", L"", url, ARRAYSIZE(url), file.c_str());
        GetPrivateProfileStringW(L"InternetShortcut", L"IconFile", L"", icon, ARRAYSIZE(icon), file.c_str());
        GetPrivateProfileStringW(L"InternetShortcut", L"IconIndex", L"0", index, ARRAYSIZE(index), file.c_str());
        if (!IsUri(url)) { Wh_Log(L"Shortcut capture: .url is missing a valid URL"); return; }
        std::wstring name = file.substr(file.find_last_of(L"\\/")+1);
        name.resize(name.size()-4);
        std::wstring json = L"{\"name\":\"" + EscapeJson(name) +
            L"\",\"path\":\"" + EscapeJson(url) + L"\",\"icon\":\"" +
            EscapeJson(icon) + L"\",\"ref\":" + std::to_wstring(_wtoi(index)) +
            L",\"type\":\"launcher\"}";
        if (!PutClipboardText(json)) Wh_Log(L"Shortcut capture: clipboard unavailable");
        return;
    }
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(kShellLinkClsid, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) return;
    IPersistFile* persist = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persist)))) {
        if (SUCCEEDED(persist->Load(file.c_str(), STGM_READ))) {
            wchar_t target[32768]{};
            wchar_t icon[32768]{};
            int iconIndex = 0;
            wchar_t arguments[32768]{}, workingDir[32768]{};
            WIN32_FIND_DATAW data{};
            link->GetPath(target, ARRAYSIZE(target), &data, SLGP_RAWPATH);
            link->GetIconLocation(icon, ARRAYSIZE(icon), &iconIndex);
            link->GetArguments(arguments, ARRAYSIZE(arguments));
            link->GetWorkingDirectory(workingDir, ARRAYSIZE(workingDir));
            std::wstring name = file.substr(file.find_last_of(L"\\/") + 1);
            if (name.size() >= 4) name.resize(name.size() - 4);
            if (target[0]) {
                std::wstring json = L"{\"name\":\"" + EscapeJson(name) +
                    L"\",\"path\":\"" + EscapeJson(target) + L"\",\"icon\":\"" +
                    EscapeJson(icon) + L"\",\"ref\":" + std::to_wstring(iconIndex) +
                    L",\"args\":\"" + EscapeJson(arguments) + L"\",\"workingDir\":\"" +
                    EscapeJson(workingDir) + L"\"}";
                if (PutClipboardText(json)) Wh_Log(L"Shortcut copied: %s", file.c_str());
                else Wh_Log(L"Shortcut capture: clipboard unavailable");
            } else Wh_Log(L"Shortcut capture: target path unavailable for %s", file.c_str());
        }
        persist->Release();
    }
    link->Release();
}

DWORD WINAPI HotkeyThread(void*) {
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(init)) return 0;
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    if (!RegisterHotKey(nullptr, kHotkey, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'C')) {
        Wh_Log(L"Shortcut hotkey Ctrl+Alt+Shift+C could not be registered");
        CoUninitialize(); return 0;
    }
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == kHotkey) CaptureShortcut();
    }
    UnregisterHotKey(nullptr, kHotkey);
    CoUninitialize();
    return 0;
}

void StopHotkey() {
    if (!gHotkeyThread) return;
    if (WaitForSingleObject(gHotkeyThread, 0) == WAIT_TIMEOUT) {
        if (!PostThreadMessageW(gHotkeyThreadId, WM_QUIT, 0, 0))
            Wh_Log(L"Hotkey thread quit message failed (%lu)", GetLastError());
        // Never unload a DLL while its thread is still executing mod code.
        // A shortcut capture already in progress must finish first.
        WaitForSingleObject(gHotkeyThread, INFINITE);
    }
    CloseHandle(gHotkeyThread);
    gHotkeyThread = nullptr;
    gHotkeyThreadId = 0;
}

void UpdateHotkey() {
    const bool enabled = Wh_GetIntSetting(L"enableShortcutCapture") != 0;
    if (!enabled) { StopHotkey(); return; }
    if (gHotkeyThread && WaitForSingleObject(gHotkeyThread, 0) == WAIT_TIMEOUT) return;
    StopHotkey();
    gHotkeyThread = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, &gHotkeyThreadId);
    if (!gHotkeyThread) Wh_Log(L"Cannot start hotkey thread (%lu)", GetLastError());
}

} // namespace

BOOL Wh_ModInit() {
    Apply();
    UpdateHotkey();
    return TRUE;
}

void Wh_ModSettingsChanged() { Apply(); UpdateHotkey(); }

void Wh_ModUninit() { StopHotkey(); Cleanup(); }
