// ==WindhawkMod==
// @id              this-pc-custom-folders
// @name            This PC Custom Folders
// @description     Add folders and native application, website or protocol shortcuts to This PC
// @version         1.6.9
// @author          Casket Pizza
// @github          https://github.com/CasketPizza
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Make This PC your own

Add your favourite folders, programs, websites and app links directly to **This PC** in File Explorer. Keep the usual drives, storage bars and automatically detected USB devices.

![This PC Custom Folders example](https://i.imgur.com/Xap2SRh.png)

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
| Existing shortcut | `C:\Path\Example.lnk` or `.url` | Network locations |

Folders use Windows shell registrations. Applications, websites and protocols use native `.lnk` launchers in **Network locations**. Website/protocol launchers call Windows' registered URL handler, so they follow the user's default browser or protocol association. Existing `.lnk` and `.url` files can also be used directly as the Path and are copied into Network locations as managed shortcuts. App and link entries always appear in **Network locations**, regardless of the Group setting.

### Import shortcuts quickly

Enable **Shortcut capture hotkey**, select a `.lnk` or `.url` shortcut in Explorer, then press **Ctrl + Alt + Shift + C**. Paste the copied JSON into an item's **Import shortcut string** field. A populated import string takes priority over its manual Name, Path and Icon fields; the Group selection remains independent.

### Icons and sorting

- **Icon file:** Supply an `.ico` path, or a `.dll` / `.exe` containing icons. For DLL/EXE files, set **Icon reference** to an index (or a negative resource ID). Leave Icon file blank for the automatic icon.
- **Sort Order:** Lower numbers appear earlier for **folder targets** placed in **Folders** or **Network locations**; numbering is independent per group. Launcher entries (`.lnk` / `.url`) are sorted by Explorer by name, and **Devices and drives** doesn't support this ordering. Default: `100`.
- Folder groups are **Folders** (the default), **Network locations**, and **Devices and drives**. Arbitrary custom group names aren't supported.

### Notes and cleanup

- Up to **64 entries** are supported. `shell:` locations are not supported as folder targets.
- The mod runs in a dedicated Windhawk tool process, so configuration work isn't performed inside Explorer and multiple Explorer processes don't race to manage the same entries.
- To provide native This PC and Network locations integration, the mod writes per-user shell registrations under `HKCU\Software\Classes\CLSID` and `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\MyComputer\NameSpace`, and creates managed `.lnk` / `.url` files in the user's **Network Shortcuts** folder.
- These registrations and shortcut files are normally removed on a clean disable while the tool process is running. Because they're persistent Windows shell state, an abrupt process termination, crash, shutdown, or removal while the tool process isn't running can leave managed entries behind. Re-enabling the mod and then disabling it cleanly will remove entries that are still recorded in Windhawk's mod storage.
- The ownership ledger itself is stored in Windhawk's mod storage and is scoped by Windows user SID; the mod no longer creates its own registry key for bookkeeping.
- Environment variables in folder paths and icons are preserved for Explorer to expand in its native bitness. Paths that the tool process must inspect or use for launchers are expanded using the native 64-bit Program Files view on 64-bit Windows.
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
      $description: 'Folder, .exe, website/protocol URL, or existing .lnk/.url shortcut. Optional when importing a shortcut.'
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
      $description: Lower numbers first for folder targets in Folders and Network locations. Launcher entries are sorted by name. Values are independent per group and can be reused. Not supported in Devices and drives. Defaults to 100.
    - group: folders
      $name: Group (folders only)
      $description: 'Defaults to Folders group if left unselected. Applies only to folder targets; applications and links always appear in Network locations.'
      $options:
        - folders: Folders (default; usually above drives)
        - network: Network locations (below drives)
        - other: Devices and drives
  $name: Custom folders
  $description: Import string overrides manual Name, Path and Icon fields. Group applies only to folders; executable, URL and shortcut-file launchers always appear in Network locations and are sorted by name. Sort Order is not supported in Devices and drives.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <sddl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <shldisp.h>
#include <cwctype>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <initializer_list>
#include <set>
#include <algorithm>
#include <array>

namespace {
// Local CLSID values avoid requiring UUID import libraries in Windhawk's linker.
constexpr GUID kShellWindowsClsid =
    {0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
constexpr GUID kShellLinkClsid =
    {0x00021401, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kNetHoodFolderId =
    {0xC5ABBF53, 0xE17F, 0x4121, {0x89, 0x00, 0x86, 0x62, 0x6F, 0xC2, 0xC9, 0x73}};
constexpr int kMaxFolders = 64;
constexpr wchar_t kMarker[] = L"CasketPizza.ThisPCCustomFolders.v1";
constexpr wchar_t kNamespaceRoot[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace\\";
constexpr wchar_t kClassesRoot[] = L"Software\\Classes\\CLSID\\";
constexpr wchar_t kLegacyLedgerRoot[] = L"Software\\CasketPizza\\ThisPCCustomFolders\\Managed";
constexpr wchar_t kFolderHandler[] = L"{0E5AAE11-A475-4c5b-AB00-C66DE400274E}";

enum class EntryKind { Folder, Executable, Uri, ShortcutFile, Invalid };

struct Folder {
    std::wstring name, path, icon, iconRef, group, args, workingDir;
    DWORD sortOrder = 100;
    EntryKind kind = EntryKind::Invalid;
};


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

bool IsWow64Host() {
    BOOL wow64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &wow64) && wow64;
}

void ReplaceInsensitive(std::wstring& text, const std::wstring& from,
                        const std::wstring& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while (pos + from.size() <= text.size()) {
        if (_wcsnicmp(text.c_str() + pos, from.c_str(), from.size()) == 0) {
            text.replace(pos, from.size(), to);
            pos += to.size();
        } else {
            ++pos;
        }
    }
}

std::wstring ExpandForHost(std::wstring s) {
    if (s.empty()) return s;

    // Tool mods run in a 32-bit host on Windhawk 1.x. Resolve variables that
    // otherwise point at the x86 program directories to their native x64
    // equivalents. In Windhawk 2.0's 64-bit tool host this is a no-op.
    if (IsWow64Host()) {
        ReplaceInsensitive(s, L"%ProgramFiles%", L"%ProgramW6432%");
        ReplaceInsensitive(s, L"%CommonProgramFiles%",
                           L"%CommonProgramW6432%");
    }

    DWORD required = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!required || required > 32768) return s;

    std::wstring out(required, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(s.c_str(), out.data(), required);
    if (!written || written > required) return s;

    out.resize(written - 1);
    return out;
}

DWORD GetAttributesNative(const std::wstring& path) {
    PVOID oldValue = nullptr;
    const bool disabled =
        IsWow64Host() &&
        Wow64DisableWow64FsRedirection(&oldValue) != FALSE;

    DWORD attributes = GetFileAttributesW(path.c_str());

    if (disabled) {
        Wow64RevertWow64FsRedirection(oldValue);
    }

    return attributes;
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
        } else {
            f.name = GetSetting(L"folders[%d].name", i);
            f.path = GetSetting(L"folders[%d].path", i);
            f.icon = GetSetting(L"folders[%d].icon", i);
            f.iconRef = GetSetting(L"folders[%d].iconRef", i);
            f.args = GetSetting(L"folders[%d].args", i);
            f.workingDir = GetSetting(L"folders[%d].workingDir", i);
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
    for (size_t i=1; i<colon; ++i)
        if (!iswalnum(s[i]) && s[i]!=L'+' && s[i]!=L'-' && s[i]!=L'.') return false;
    // No unquoted spaces/control characters in protocol or web URI.
    return s.find_first_of(L" \t\r\n\"", colon) == std::wstring::npos;
}
bool DirectoryExists(const std::wstring& s) {
    const std::wstring path = ExpandForHost(s);
    DWORD a = GetAttributesNative(path);
    return a != INVALID_FILE_ATTRIBUTES &&
           (a & FILE_ATTRIBUTE_DIRECTORY);
}

bool FileExists(const std::wstring& s) {
    const std::wstring path = ExpandForHost(s);
    DWORD a = GetAttributesNative(path);
    return a != INVALID_FILE_ATTRIBUTES &&
           !(a & FILE_ATTRIBUTE_DIRECTORY);
}
bool IsExe(const std::wstring& path) {
    auto dot = path.find_last_of(L'.');
    return dot != std::wstring::npos && !_wcsicmp(path.c_str()+dot, L".exe");
}

bool HasExtension(const std::wstring& path, const wchar_t* extension) {
    const size_t length = wcslen(extension);
    return path.size() >= length &&
           _wcsicmp(path.c_str() + path.size() - length, extension) == 0;
}

bool IsShortcutFile(const std::wstring& path) {
    return HasExtension(path, L".lnk") || HasExtension(path, L".url");
}
EntryKind Kind(const Folder& f) {
    if (IsUri(f.path)) return EntryKind::Uri;

    // UNC paths are folder targets by syntax unless they explicitly name a
    // supported launcher file.
    if (StartsWith(f.path, L"\\\\") &&
        !IsExe(f.path) && !IsShortcutFile(f.path)) {
        return EntryKind::Folder;
    }

    if (IsShortcutFile(f.path))
        return FileExists(f.path) ? EntryKind::ShortcutFile
                                  : EntryKind::Invalid;

    if (IsExe(f.path))
        return FileExists(f.path) ? EntryKind::Executable
                                  : EntryKind::Invalid;

    return DirectoryExists(f.path) ? EntryKind::Folder : EntryKind::Invalid;
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

constexpr REGSAM kRegistryView = KEY_WOW64_64KEY;

bool OpenKey(const std::wstring& path, HKEY* key) {
    return RegCreateKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, nullptr, 0,
                           KEY_READ | KEY_WRITE | kRegistryView, nullptr, key,
                           nullptr) == ERROR_SUCCESS;
}

void SetStringAt(const std::wstring& path, LPCWSTR name,
                 const std::wstring& val, DWORD type = REG_SZ) {
    HKEY key;
    if (OpenKey(path, &key)) {
        if (!PutString(key, name, val, type)) Wh_Log(L"Unable to write %s", path.c_str());
        RegCloseKey(key);
    } else Wh_Log(L"Unable to create %s", path.c_str());
}


bool ReadStringAt(const std::wstring& path, LPCWSTR name,
                  std::wstring* output) {
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0,
                      KEY_READ | kRegistryView, &key) != ERROR_SUCCESS)
        return false;
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
                                KEY_READ | kRegistryView, &key);
    if (status == ERROR_SUCCESS) RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

// Ownership is kept in Windhawk local storage instead of a custom registry
// key. Windhawk storage is machine-wide, so use the current user's SID as part
// of the value name.
struct LedgerState {
    std::set<std::wstring> ids;
    std::array<std::wstring, kMaxFolders> shortcutNames;
};

LedgerState gLedger;
std::wstring gLedgerValueName;

std::wstring CurrentUserSidString() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return L"";

    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    if (!size) {
        CloseHandle(token);
        return L"";
    }

    std::vector<BYTE> buffer(size);
    if (!GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
        CloseHandle(token);
        return L"";
    }
    CloseHandle(token);

    const TOKEN_USER* user =
        reinterpret_cast<const TOKEN_USER*>(buffer.data());
    LPWSTR sidText = nullptr;
    if (!ConvertSidToStringSidW(user->User.Sid, &sidText)) return L"";

    std::wstring result = sidText;
    LocalFree(sidText);
    return result;
}

wchar_t HexDigit(unsigned value) {
    return value < 10 ? static_cast<wchar_t>(L'0' + value)
                      : static_cast<wchar_t>(L'A' + value - 10);
}

std::wstring HexEncode(const std::wstring& value) {
    std::wstring out;
    out.reserve(value.size() * 4);
    for (wchar_t ch : value) {
        unsigned v = static_cast<unsigned>(ch);
        out += HexDigit((v >> 12) & 0xF);
        out += HexDigit((v >> 8) & 0xF);
        out += HexDigit((v >> 4) & 0xF);
        out += HexDigit(v & 0xF);
    }
    return out;
}

int HexValue(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') return ch - L'0';
    if (ch >= L'A' && ch <= L'F') return ch - L'A' + 10;
    if (ch >= L'a' && ch <= L'f') return ch - L'a' + 10;
    return -1;
}

bool HexDecode(const std::wstring& value, std::wstring* output) {
    if (value.size() % 4 != 0) return false;
    output->clear();
    output->reserve(value.size() / 4);
    for (size_t i = 0; i < value.size(); i += 4) {
        int a = HexValue(value[i]);
        int b = HexValue(value[i + 1]);
        int c = HexValue(value[i + 2]);
        int d = HexValue(value[i + 3]);
        if (a < 0 || b < 0 || c < 0 || d < 0) return false;
        output->push_back(static_cast<wchar_t>(
            (a << 12) | (b << 8) | (c << 4) | d));
    }
    return true;
}

void SaveLedger() {
    if (gLedgerValueName.empty()) return;

    std::wstring serialized;
    for (const auto& id : gLedger.ids) {
        serialized += L"I\t" + id + L"\n";
    }
    for (int i = 0; i < kMaxFolders; ++i) {
        if (!gLedger.shortcutNames[i].empty()) {
            serialized += L"S\t" + std::to_wstring(i) + L"\t" +
                          HexEncode(gLedger.shortcutNames[i]) + L"\n";
        }
    }

    if (serialized.empty()) {
        Wh_DeleteValue(gLedgerValueName.c_str());
    } else if (!Wh_SetStringValue(gLedgerValueName.c_str(),
                                  serialized.c_str())) {
        Wh_Log(L"Unable to save ownership ledger");
    }
}

void LoadLedger() {
    gLedger = {};
    const std::wstring sid = CurrentUserSidString();
    if (sid.empty()) {
        Wh_Log(L"Unable to determine current user SID");
        return;
    }

    gLedgerValueName = L"managed-" + sid;

    std::vector<wchar_t> buffer(65536);
    const size_t length = Wh_GetStringValue(
        gLedgerValueName.c_str(), buffer.data(), buffer.size());
    if (!length) return;

    std::wstring serialized(buffer.data(), length);
    size_t at = 0;
    while (at < serialized.size()) {
        size_t end = serialized.find(L'\n', at);
        if (end == std::wstring::npos) end = serialized.size();
        std::wstring line = serialized.substr(at, end - at);
        at = end + 1;

        if (line.rfind(L"I\t", 0) == 0) {
            std::wstring id = line.substr(2);
            if (id.size() == 38 && id.front() == L'{' && id.back() == L'}')
                gLedger.ids.insert(std::move(id));
            continue;
        }

        if (line.rfind(L"S\t", 0) == 0) {
            size_t tab = line.find(L'\t', 2);
            if (tab == std::wstring::npos) continue;
            int index = _wtoi(line.substr(2, tab - 2).c_str());
            if (index < 0 || index >= kMaxFolders) continue;
            std::wstring name;
            if (HexDecode(line.substr(tab + 1), &name))
                gLedger.shortcutNames[index] = std::move(name);
        }
    }
}

// Migrate the development/pre-catalog registry ledger once, then remove it.
void MigrateLegacyLedger() {
    HKEY ledger = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kLegacyLedgerRoot, 0, KEY_READ,
                      &ledger) != ERROR_SUCCESS) {
        return;
    }

    for (DWORD i = 0;; ++i) {
        wchar_t name[256]{};
        DWORD nameLen = ARRAYSIZE(name);
        wchar_t value[512]{};
        DWORD valueBytes = sizeof(value);
        DWORD type = 0;
        LONG result = RegEnumValueW(
            ledger, i, name, &nameLen, nullptr, &type,
            reinterpret_cast<BYTE*>(value), &valueBytes);
        if (result == ERROR_NO_MORE_ITEMS) break;
        if (result != ERROR_SUCCESS) continue;
        if (type != REG_SZ && type != REG_EXPAND_SZ) continue;

        std::wstring valueName(name, nameLen);
        if (valueName.size() == 38 &&
            valueName.front() == L'{' &&
            valueName.back() == L'}') {
            gLedger.ids.insert(valueName);
        } else if (valueName.rfind(L"Shortcut_", 0) == 0) {
            int index = _wtoi(valueName.c_str() + 9);
            if (index >= 0 && index < kMaxFolders && value[0])
                gLedger.shortcutNames[index] = value;
        }
    }
    RegCloseKey(ledger);

    SaveLedger();
    RegDeleteTreeW(HKEY_CURRENT_USER,
                   L"Software\\CasketPizza\\ThisPCCustomFolders");
}

bool ListedAsOurs(const std::wstring& id) {
    return gLedger.ids.count(id) != 0;
}

bool HasOwnerMarker(const std::wstring& cls) {
    std::wstring marker;
    return ReadStringAt(cls, L"ThisPCCustomFoldersOwner", &marker) &&
           marker == kMarker;
}

bool TrackedId(const std::wstring& id) {
    const std::wstring cls = std::wstring(kClassesRoot) + id;
    return ListedAsOurs(id) || HasOwnerMarker(cls);
}

void DeleteTreeLogged(const std::wstring& path) {
    const size_t slash = path.find_last_of(L'\\');
    if (slash == std::wstring::npos) return;

    const std::wstring parentPath = path.substr(0, slash);
    const std::wstring childName = path.substr(slash + 1);

    HKEY parent = nullptr;
    LONG status = RegOpenKeyExW(
        HKEY_CURRENT_USER, parentPath.c_str(), 0,
        DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE |
        kRegistryView, &parent);
    if (status == ERROR_FILE_NOT_FOUND || status == ERROR_PATH_NOT_FOUND)
        return;
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Registry parent open failed (%ld): %s",
               status, parentPath.c_str());
        return;
    }

    status = RegDeleteTreeW(parent, childName.c_str());
    RegCloseKey(parent);
    if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND &&
        status != ERROR_PATH_NOT_FOUND) {
        Wh_Log(L"Registry removal failed (%ld): %s", status, path.c_str());
    }
}

void RemoveId(const std::wstring& id) {
    if (!TrackedId(id)) return;
    const std::wstring cls = std::wstring(kClassesRoot) + id;
    const std::wstring ns = std::wstring(kNamespaceRoot) + id;
    DeleteTreeLogged(ns);
    DeleteTreeLogged(cls);
    if (!KeyExists(cls) && !KeyExists(ns)) {
        gLedger.ids.erase(id);
        SaveLedger();
    }
}

std::vector<std::wstring> ManagedIds() {
    return std::vector<std::wstring>(gLedger.ids.begin(), gLedger.ids.end());
}

bool MarkManaged(const std::wstring& id) {
    gLedger.ids.insert(id);
    SaveLedger();
    return ListedAsOurs(id);
}

std::wstring ShortcutFolder() {
    PWSTR path = nullptr;
    if (FAILED(SHGetKnownFolderPath(kNetHoodFolderId, 0, nullptr, &path)))
        return L"";
    std::wstring result = path;
    CoTaskMemFree(path);
    return result;
}

std::wstring SafeName(std::wstring name) {
    for (auto& ch : name) {
        if (wcschr(L"<>:\"/\\|?*", ch) || ch < 32) ch = L'_';
    }
    while (!name.empty() && (name.back() == L'.' || name.back() == L' '))
        name.pop_back();
    if (name.empty()) name = L"Shortcut";
    if (name.size() > 100) name.resize(100);
    return name;
}

std::wstring ShortcutPath(bool url, const std::wstring& name) {
    const std::wstring dir = ShortcutFolder();
    if (dir.empty()) return L"";
    return dir + L"\\" + SafeName(name) + (url ? L".url" : L".lnk");
}

std::wstring LedgerShortcutName(int i) {
    if (i < 0 || i >= kMaxFolders) return L"";
    return gLedger.shortcutNames[i];
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
        const auto path = ShortcutPath(url, name);
        if (!path.empty() && OwnedShortcut(path, url) && !DeleteFileW(path.c_str()))
            Wh_Log(L"Unable to remove managed shortcut %s (%lu)", path.c_str(), GetLastError());
    }
    // Keep ledger if an owned file persists, to allow retry on next reload.
    bool remaining = false;
    for (bool url : {false, true}) remaining |= OwnedShortcut(ShortcutPath(url,name),url);
    if (!remaining && i >= 0 && i < kMaxFolders) {
        gLedger.shortcutNames[i].clear();
        SaveLedger();
    }
}
void RemoveAll(int i) { RemoveShortcut(i); }
bool CreateLauncher(int i, const Folder& f, EntryKind kind) {
    ScopedCom com;

    const bool sourceShortcut = kind == EntryKind::ShortcutFile;
    const bool outputUrl =
        sourceShortcut && HasExtension(f.path, L".url");

    auto path = ShortcutPath(outputUrl, f.name);
    if (path.empty()) return false;

    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Launcher filename collision (not overwriting): %s",
               path.c_str());
        return false;
    }

    const std::wstring staging =
        path + L".tmp." + std::to_wstring(GetCurrentProcessId()) + L"." +
        std::to_wstring(GetCurrentThreadId());

    if (GetFileAttributesW(staging.c_str()) != INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Staging file already exists: %s", staging.c_str());
        return false;
    }

    bool ok = false;

    if (sourceShortcut && outputUrl) {
        // Preserve an existing Internet Shortcut, but add our ownership marker
        // to the staged copy so cleanup never deletes an unrelated file.
        const std::wstring sourcePath = ExpandForHost(f.path);
        ok = CopyFileW(sourcePath.c_str(), staging.c_str(), TRUE) != FALSE;
        if (ok) {
            ok = WritePrivateProfileStringW(
                     L"InternetShortcut",
                     L"ThisPCCustomFoldersOwner",
                     kMarker,
                     staging.c_str()) != FALSE;
        }
        if (ok) {
            ok = WritePrivateProfileStringW(
                     nullptr, nullptr, nullptr, staging.c_str()) != FALSE;
        }
    } else {
        IShellLinkW* link = nullptr;
        if (FAILED(CoCreateInstance(kShellLinkClsid, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(&link)))) {
            return false;
        }

        IPersistFile* persistence = nullptr;
        HRESULT hr = E_FAIL;

        if (sourceShortcut) {
            // Load an existing .lnk and republish it as a managed copy.
            if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persistence)))) {
                const std::wstring sourcePath = ExpandForHost(f.path);
                hr = persistence->Load(sourcePath.c_str(), STGM_READ);
                if (SUCCEEDED(hr)) {
                    link->SetDescription(kMarker);
                    if (!f.icon.empty()) {
                        const std::wstring iconPath =
                            ExpandForHost(f.icon);
                        link->SetIconLocation(
                            iconPath.c_str(), _wtoi(f.iconRef.c_str()));
                    }
                    hr = persistence->Save(staging.c_str(), TRUE);
                }
            }
        } else {
            if (kind == EntryKind::Uri) {
                // A .lnk works more reliably in Network Shortcuts than a .url.
                // FileProtocolHandler asks Windows to invoke the registered
                // handler for http(s), steam:, mailto:, and other protocols.
                const std::wstring handler =
                    ExpandForHost(L"%SystemRoot%\\System32\\rundll32.exe");
                const std::wstring arguments =
                    L"url.dll,FileProtocolHandler \"" + f.path + L"\"";

                link->SetPath(handler.c_str());
                link->SetArguments(arguments.c_str());
            } else {
                const std::wstring targetPath =
                    ExpandForHost(f.path);
                link->SetPath(targetPath.c_str());
                if (!f.args.empty())
                    link->SetArguments(f.args.c_str());
                if (!f.workingDir.empty()) {
                    const std::wstring workingDirectory =
                        ExpandForHost(f.workingDir);
                    link->SetWorkingDirectory(
                        workingDirectory.c_str());
                }
            }

            link->SetDescription(kMarker);

            if (!f.icon.empty()) {
                const std::wstring iconPath =
                    ExpandForHost(f.icon);
                link->SetIconLocation(
                    iconPath.c_str(), _wtoi(f.iconRef.c_str()));
            }

            if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persistence)))) {
                hr = persistence->Save(staging.c_str(), TRUE);
            }
        }

        if (persistence) persistence->Release();
        link->Release();

        ok = SUCCEEDED(hr);
    }

    if (ok) {
        ok = MoveFileExW(staging.c_str(), path.c_str(),
                         MOVEFILE_WRITE_THROUGH) != FALSE;
    }

    if (!ok) {
        Wh_Log(L"Could not publish launcher: %s (%lu)",
               path.c_str(), GetLastError());
        DeleteFileW(staging.c_str());
        return false;
    }

    if (i >= 0 && i < kMaxFolders) {
        gLedger.shortcutNames[i] = f.name;
        SaveLedger();
    }

    Wh_Log(L"Created managed Network locations shortcut: %s -> %s",
           f.name.c_str(), f.path.c_str());
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

    for (Folder& f : folders) {
        f.kind = Kind(f);
    }

    // Reject duplicate folder identities and launcher filenames before making changes.
    std::set<std::wstring> folderIds;
    std::set<std::wstring> launcherNames;
    for (size_t i = 0; i < folders.size(); ++i) {
        const Folder& f = folders[i];
        const EntryKind kind = f.kind;
        if (kind == EntryKind::Folder) {
            if (!folderIds.insert(StableId(f)).second) {
                Wh_Log(L"Duplicate folder entry %s; keeping existing registrations", f.name.c_str());
                return;
            }
        } else if (kind == EntryKind::Executable ||
                   kind == EntryKind::Uri ||
                   kind == EntryKind::ShortcutFile) {
            const bool outputUrl =
                kind == EntryKind::ShortcutFile &&
                HasExtension(f.path, L".url");
            std::wstring key =
                SafeName(f.name) + (outputUrl ? L".url" : L".lnk");
            std::transform(
                key.begin(), key.end(), key.begin(),
                [](wchar_t c) {
                    return static_cast<wchar_t>(towlower(c));
                });
            if (!launcherNames.insert(key).second) {
                Wh_Log(L"Duplicate launcher name %s; keeping existing registrations",
                       f.name.c_str());
                return;
            }
        }
    }
    std::set<std::wstring> desired;
    for (const Folder& f : folders)
        if (f.kind == EntryKind::Folder) desired.insert(StableId(f));

    // Migrate/remove all old position-based registrations. Launcher shortcuts
    // still use the per-row cleanup ledger but never share the folder CLSID.
    for (int i = 0; i < kMaxFolders; ++i) RemoveAll(i);
    for (const auto& id : ManagedIds())
        if (!desired.count(id)) RemoveId(id);

    for (size_t i = 0; i < folders.size(); ++i) {
        const Folder& f = folders[i];
        EntryKind kind = f.kind;
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

            if (!KeyExists(cls) || !KeyExists(ns)) {
                Wh_Log(L"Repairing partial registration for %s", f.name.c_str());
                RemoveId(id);
            } else {
                // The sort order is intentionally excluded from StableId: update it
                // in place without forcing a new shell identity or cache churn.
                HKEY key = nullptr;
                if (RegOpenKeyExW(HKEY_CURRENT_USER, cls.c_str(), 0,
                                  KEY_SET_VALUE | kRegistryView, &key) == ERROR_SUCCESS) {
                    if (!PutDword(key, L"SortOrderIndex", f.sortOrder))
                        Wh_Log(L"Could not update Sort Order for %s", f.name.c_str());
                    RegCloseKey(key);
                } else {
                    Wh_Log(L"Could not open existing entry to update Sort Order: %s", f.name.c_str());
                }
                continue;
            }
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
        regString(cls + L"\\Instance\\InitPropertyBag",
                  L"TargetFolderPath", f.path, REG_EXPAND_SZ);
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
        std::vector<wchar_t> url(32768), icon(32768);
        wchar_t index[32]{};
        GetPrivateProfileStringW(L"InternetShortcut", L"URL", L"",
                                 url.data(), static_cast<DWORD>(url.size()),
                                 file.c_str());
        GetPrivateProfileStringW(L"InternetShortcut", L"IconFile", L"",
                                 icon.data(), static_cast<DWORD>(icon.size()),
                                 file.c_str());
        GetPrivateProfileStringW(L"InternetShortcut", L"IconIndex", L"0",
                                 index, ARRAYSIZE(index), file.c_str());
        if (!IsUri(url.data())) {
            Wh_Log(L"Shortcut capture: .url is missing a valid URL");
            return;
        }
        std::wstring name = file.substr(file.find_last_of(L"\\/")+1);
        name.resize(name.size()-4);
        std::wstring json = L"{\"name\":\"" + EscapeJson(name) +
            L"\",\"path\":\"" + EscapeJson(url.data()) + L"\",\"icon\":\"" +
            EscapeJson(icon.data()) + L"\",\"ref\":" + std::to_wstring(_wtoi(index)) + L"}";
        if (!PutClipboardText(json)) Wh_Log(L"Shortcut capture: clipboard unavailable");
        return;
    }
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(kShellLinkClsid, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) return;
    IPersistFile* persist = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persist)))) {
        if (SUCCEEDED(persist->Load(file.c_str(), STGM_READ))) {
            std::vector<wchar_t> target(32768);
            std::vector<wchar_t> icon(32768);
            std::vector<wchar_t> arguments(32768);
            std::vector<wchar_t> workingDir(32768);
            int iconIndex = 0;
            WIN32_FIND_DATAW data{};
            link->GetPath(target.data(), static_cast<int>(target.size()),
                          &data, SLGP_RAWPATH);
            link->GetIconLocation(icon.data(), static_cast<int>(icon.size()),
                                  &iconIndex);
            link->GetArguments(arguments.data(),
                               static_cast<int>(arguments.size()));
            link->GetWorkingDirectory(workingDir.data(),
                                      static_cast<int>(workingDir.size()));
            std::wstring name = file.substr(file.find_last_of(L"\\/") + 1);
            if (name.size() >= 4) name.resize(name.size() - 4);
            if (target[0]) {
                std::wstring json = L"{\"name\":\"" + EscapeJson(name) +
                    L"\",\"path\":\"" + EscapeJson(target.data()) + L"\",\"icon\":\"" +
                    EscapeJson(icon.data()) + L"\",\"ref\":" + std::to_wstring(iconIndex) +
                    L",\"args\":\"" + EscapeJson(arguments.data()) + L"\",\"workingDir\":\"" +
                    EscapeJson(workingDir.data()) + L"\"}";
                if (PutClipboardText(json)) Wh_Log(L"Shortcut copied: %s", file.c_str());
                else Wh_Log(L"Shortcut capture: clipboard unavailable");
            } else Wh_Log(L"Shortcut capture: target path unavailable for %s", file.c_str());
        }
        persist->Release();
    }
    link->Release();
}

DWORD WINAPI HotkeyThread(void* parameter) {
    HANDLE readyEvent = static_cast<HANDLE>(parameter);

    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(init)) {
        SetEvent(readyEvent);
        return 0;
    }

    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    SetEvent(readyEvent);

    if (!RegisterHotKey(nullptr, kHotkey, MOD_CONTROL | MOD_ALT | MOD_SHIFT |
                        MOD_NOREPEAT, 'C')) {
        Wh_Log(L"Shortcut hotkey Ctrl+Alt+Shift+C could not be registered");
        CoUninitialize();
        return 0;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == kHotkey)
            CaptureShortcut();
    }

    UnregisterHotKey(nullptr, kHotkey);
    CoUninitialize();
    return 0;
}

void StopHotkey() {
    if (!gHotkeyThread) return;

    while (!PostThreadMessageW(gHotkeyThreadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(gHotkeyThread, 10) == WAIT_TIMEOUT) {
    }

    WaitForSingleObject(gHotkeyThread, INFINITE);
    CloseHandle(gHotkeyThread);
    gHotkeyThread = nullptr;
    gHotkeyThreadId = 0;
}

void UpdateHotkey() {
    const bool enabled = Wh_GetIntSetting(L"enableShortcutCapture") != 0;
    if (!enabled) {
        StopHotkey();
        return;
    }

    if (gHotkeyThread &&
        WaitForSingleObject(gHotkeyThread, 0) == WAIT_TIMEOUT) {
        return;
    }

    StopHotkey();

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        Wh_Log(L"Cannot create hotkey ready event (%lu)", GetLastError());
        return;
    }

    gHotkeyThread = CreateThread(nullptr, 0, HotkeyThread, readyEvent, 0,
                                 &gHotkeyThreadId);
    if (!gHotkeyThread) {
        Wh_Log(L"Cannot start hotkey thread (%lu)", GetLastError());
        CloseHandle(readyEvent);
        return;
    }

    const DWORD waitResult = WaitForSingleObject(readyEvent, 5000);
    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Hotkey thread failed to initialize");
        StopHotkey();
    }

    CloseHandle(readyEvent);
}
} // namespace

BOOL WhTool_ModInit() {
    LoadLedger();
    MigrateLegacyLedger();
    Apply();
    UpdateHotkey();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Apply();
    UpdateHotkey();
}

void WhTool_ModUninit() {
    StopHotkey();
    Cleanup();
}


// --- Windhawk Tool Mod Boilerplate (Do not modify) ---
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
