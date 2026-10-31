// ==WindhawkMod==
// @id              explorer-color-tags
// @name            Explorer Color Tags
// @description     Finder-like colored tags for File Explorer, with Alt+1...8 shortcuts, dots after file names and colored tag pills in the Tags column
// @version         0.3.1
// @author          renegade
// @github          https://github.com/renegadeFree
// @homepage        https://github.com/renegadeFree/explorer-color-tags
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lshell32 -lshlwapi -lcomctl32 -lgdi32 -lgdiplus -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Color Tags

Explorer Color Tags is a separately maintained mod by renegade. It was built
around the idea of macOS Finder tags and was initially inspired by
[Explorer Tags](https://github.com/buedgik/explorer-tags). Substantial portions
of its underlying tag engine originate from Explorer Tags under its MIT
license, including settings loading, the tag record and file-identity helpers,
shortcut-folder synchronization and index maintenance, the worker thread, and
the classic context-menu submenu.

This is not an upstream extension of Explorer Tags. Unlike Explorer Tags,
which provides a tag browser in the navigation pane, Explorer Color Tags keeps
tagged items in their normal folder position. Each tagged item gets up to three
colored dots directly after its name. If the optional Tags column is enabled,
the same tags are also shown as colored pills in that row. Tags therefore stay
visible in place without opening a separate tag folder.

The mod also provides:

- `Alt+1` through `Alt+8` assign a tag to the selected files.
- `Alt+0` removes every tag from the selected files.
- Colored dots are drawn directly after tagged item names in File Explorer and
  on the Desktop.
- An optional **Tags** column shows the same tags as colored pills in Details
  view.
- A classic context menu provides the same tag commands. The navigation-pane
  Tags section is not shown because it opens cross-folder shortcut views.

## Screenshots

![Colored dots beside file names and colored tag pills in the Tags column](https://raw.githubusercontent.com/renegadeFree/explorer-color-tags/main/docs/explorer-color-tags.png)

![Colored tag dots on the Windows Desktop](https://raw.githubusercontent.com/renegadeFree/explorer-color-tags/main/docs/desktop.png)

![Tagging from the classic context menu](https://raw.githubusercontent.com/renegadeFree/explorer-color-tags/main/docs/menu.png)

![The Tags submenu with colored tag entries](https://raw.githubusercontent.com/renegadeFree/explorer-color-tags/main/docs/submenu.png)

The mod keeps an internal shortcut-folder record as a rebuildable index, so
tags remain valid for every extension and folder. That record is stored in
Windhawk's internal mod storage directory and is not shown in the navigation pane.
Tags are not written to `System.Keywords`, so text files, ZIP archives,
executables and folders all use the same visual path without modifying file contents.

Hotkeys only act while an Explorer file view is in the foreground. The classic
context menu is required for the **Tags** submenu; Windows 11's new menu is not
changed.

Explorer saves column configurations per folder in view state (Bags), so
folders visited while the "Automatically add Tags column" option is enabled
retain the column until manually removed by right-clicking the column header
and unchecking "Tags".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- tags:
  - - name: Important
      $name: Name
    - color: "#E81123"
      $name: Color
      $description: In hex, for example #E81123
  - - name: Work
    - color: "#0078D4"
  - - name: Personal
    - color: "#107C10"
  - - name: Later
    - color: "#FF8C00"
  - - name: Archive
    - color: "#8764B8"
  - - name: Review
    - color: "#C8A000"
  - - name: Home
    - color: "#00B7C3"
  - - name: Temporary
    - color: "#767676"
  $name: Tags
  $description: Each tag has a name and a color.
- folder: ""
  $name: Internal tags storage
  $description: >-
    Where the internal shortcut record goes. It is not shown in the navigation
    pane. Empty uses the mod's internal storage in Windhawk's application data folder.

    Changing this rebuilds the shortcuts under the new folder and leaves the
    old one behind. Avoid synced folders (OneDrive, Dropbox, network shares):
    a shortcut missing mid-sync looks like a tag you removed. The whole tree
    under this folder is watched, so keep it out of busy folders.
- tagColumnWidth: 160
  $name: Tags column width
  $description: Width in logical pixels at 100% DPI (only applies when "Automatically add Tags column" adds the column).
- autoAddTagColumn: false
  $name: Automatically add Tags column
  $description: >-
    If enabled, automatically adds the native Tags column to all folders
    in Details view. If disabled (default), you choose which folders display
    it by right-clicking the column header and checking/unchecking "Tags".
    Note: Explorer saves column configurations per folder in view state (Bags),
    so folders visited while enabled keep the column until manually removed.
- explorerDotSize: 10
  $name: File Explorer dot size
  $description: Diameter of the colored dots next to file names in File Explorer, in logical pixels at 100% DPI (default is 10).
- desktopDotSize: 8
  $name: Desktop dot size
  $description: Diameter of the colored dots next to items on the Desktop, in logical pixels at 100% DPI (default is 8).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <commctrl.h>
#include <oleacc.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <servprov.h>
#include <propkey.h>
#include <propsys.h>
#include <propvarutil.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <windhawk_utils.h>


#define CWM_GETISHELLBROWSER (WM_USER + 7)


// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Reading the settings stops here, and everything else follows this number.
const int MAX_TAGS = 64;

struct TagDef {
    std::wstring name;
    std::wstring folderName;
    COLORREF color;
};

struct Settings {
    std::vector<TagDef> tags;
    std::wstring root;
    int tagColumnWidth = 160;
    int explorerDotSize = 10;
    int desktopDotSize = 8;
    bool autoAddTagColumn = false;
};

using PaintBackground_t = void(void*, HDC, void*, const RECT*,
                               const RECT*, const RECT*, const RECT*);
using DispatchMessageW_t = decltype(&DispatchMessageW);
using IntFn = int (*)(void*);
using ContentStringFn = PCWSTR (*)(void*, void**);
using ReleaseValueFn = void (*)(void*);
using StrToIDFn = ATOM(WINAPI*)(PCWSTR);
using FindDescendentFn = void* (*)(void*, ATOM);
using GetRootRelativeBoundsFn = HRESULT (*)(void*, RECT*);

HMODULE g_dui;
PaintBackground_t* g_paintBackgroundOriginal;
DispatchMessageW_t g_dispatchMessageOriginal;
IntFn g_duiGetAccRole;
ContentStringFn g_duiGetContentString;
ContentStringFn g_duiGetAccName;
ReleaseValueFn g_duiReleaseValue;
StrToIDFn g_duiStrToID;
FindDescendentFn g_duiFindDescendent;
GetRootRelativeBoundsFn g_duiGetRootRelativeBounds;
ATOM g_fileNameAtom;

// Shared by pointer: the window threads read the settings on every mouse move,
// every paint and every drag update, and copying the tag list each time was
// pure waste. The pointed-to value is never modified, only replaced.
std::shared_ptr<const Settings> g_settings = std::make_shared<const Settings>();
SRWLOCK g_settingsLock = SRWLOCK_INIT;

std::shared_ptr<const Settings> GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    std::shared_ptr<const Settings> copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n\"");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n\"");
    return s.substr(a, b - a + 1);
}

// Name usable as a file or folder name.
std::wstring SanitizeFileName(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        if (c < 32 || wcschr(L"\\/:*?\"<>|", c)) {
            out += L'_';
        } else {
            out += c;
        }
    }
    while (!out.empty() && (out.back() == L'.' || out.back() == L' ')) {
        out.pop_back();
    }
    return out;
}

std::wstring ShortHash(const std::wstring& s) {
    UINT32 h = 2166136261u;
    for (wchar_t c : s) {
        h ^= c;
        h *= 16777619u;
    }
    WCHAR buf[16];
    wsprintfW(buf, L"%06X", h & 0xFFFFFF);
    return buf;
}

COLORREF ParseColor(std::wstring s, COLORREF fallback) {
    s = Trim(s);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6) {
        return fallback;
    }
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s.c_str(), &end, 16);
    if (!end || *end) {
        return fallback;
    }
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

// The folder depends only on the name, never on the list position, otherwise
// reordering the tags would swap their folders. A name that had to be altered
// gets a suffix from the exact name ("A/B" and "A:B" give different folders).
// CON, NUL, COM1... can't be folder names: a tag called one of those would
// never get a folder, and tagging would do nothing with no way to tell why.
bool IsReservedDeviceName(const std::wstring& name) {
    static const wchar_t* kReserved[] = {L"CON", L"PRN", L"AUX", L"NUL", L"COM1", L"COM2", L"COM3",
                                         L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", L"COM9",
                                         L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5", L"LPT6",
                                         L"LPT7", L"LPT8", L"LPT9"};
    std::wstring stem = name.substr(0, name.find(L'.'));
    for (const wchar_t* reserved : kReserved) {
        if (_wcsicmp(stem.c_str(), reserved) == 0) {
            return true;
        }
    }
    return false;
}

std::wstring FolderNameFor(const std::wstring& tagName) {
    std::wstring folder = SanitizeFileName(tagName);
    if (folder != tagName || folder.empty() || folder[0] == L'.' || IsReservedDeviceName(folder)) {
        folder = (folder.empty() ? L"Tag" : folder) + L" " + ShortHash(tagName);
    }
    return folder;
}

std::wstring DbFolder();

void LoadSettings() {
    Settings s;

    for (int i = 0; i < MAX_TAGS; i++) {
        auto name = WindhawkUtils::StringSetting::make(L"tags[%d].name", i);
        auto color = WindhawkUtils::StringSetting::make(L"tags[%d].color", i);
        if (!*name.get() && !*color.get()) {
            break;
        }
        std::wstring tagName = Trim(name.get());
        std::wstring tagColor = color.get();
        // The database is one record per line with tab-separated fields, so
        // none of the three may survive in a tag name: a newline would split
        // the record in two and lose every file with that tag on the next
        // load.
        for (wchar_t& c : tagName) {
            if (c == L'\t' || c == L'\r' || c == L'\n') {
                c = L' ';
            }
        }
        if (tagName.empty()) {
            continue;
        }
        bool duplicate = false;
        for (const auto& t : s.tags) {
            if (_wcsicmp(t.name.c_str(), tagName.c_str()) == 0) {
                duplicate = true;
            }
        }
        if (duplicate) {
            continue;
        }

        TagDef tag;
        tag.name = tagName;
        tag.color = ParseColor(tagColor, RGB(0x80, 0x80, 0x80));
        tag.folderName = FolderNameFor(tagName);
        s.tags.push_back(tag);
    }

    auto folder = WindhawkUtils::StringSetting::make(L"folder");
    std::wstring root = Trim(folder.get());
    if (root.empty()) {
        root = DbFolder() + L"\\Tags";
    }
    WCHAR expanded[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(root.c_str(), expanded, ARRAYSIZE(expanded));
    if (n > 0 && n <= ARRAYSIZE(expanded)) {
        root = expanded;
    }
    WCHAR full[MAX_PATH * 2];
    DWORD fullLen = GetFullPathNameW(root.c_str(), ARRAYSIZE(full), full, nullptr);
    // Only truly absolute paths: "X:\..." or "\\server\...". "C:folder"
    // resolved to explorer.exe's current folder.
    bool absolute = (root.size() >= 3 && root[1] == L':' && (root[2] == L'\\' || root[2] == L'/')) ||
                    root.rfind(L"\\\\", 0) == 0;
    bool valid = absolute && fullLen > 0 && fullLen < ARRAYSIZE(full);
    if (valid) {
        root = full;
    }
    while (root.size() > 3 && (root.back() == L'\\' || root.back() == L'/')) {
        root.pop_back();
    }
    // The whole tree under the tags folder is watched, so a root that holds
    // the profile, or the mod's own record, means every unrelated write in
    // there triggers a full re-check, forever.
    if (valid) {
        std::wstring dbFolder = DbFolder();
        std::wstring prefix = root + L"\\";
        bool holdsOwnRecord =
            !dbFolder.empty() &&
            (_wcsicmp(dbFolder.c_str(), root.c_str()) == 0 ||
             (dbFolder.size() > prefix.size() &&
              _wcsnicmp(dbFolder.c_str(), prefix.c_str(), prefix.size()) == 0));
        WCHAR profile[MAX_PATH * 2];
        DWORD profileLen = ExpandEnvironmentStringsW(L"%USERPROFILE%", profile, ARRAYSIZE(profile));
        bool isProfile = profileLen > 0 && profileLen <= ARRAYSIZE(profile) &&
                         _wcsicmp(root.c_str(), profile) == 0;
        if (holdsOwnRecord || isProfile) {
            valid = false;
        }
    }
    // A drive root would watch the entire drive ("C:" alone is
    // explorer.exe's current folder, which is System32).
    if (!valid || PathIsRootW(root.c_str()) || root.size() <= 3) {
        Wh_Log(L"Invalid tags folder (%s), using the default", root.c_str());
        root = DbFolder() + L"\\Tags";
    }
    s.root = root;

    s.tagColumnWidth = Wh_GetIntSetting(L"tagColumnWidth");
    if (s.tagColumnWidth < 80) {
        s.tagColumnWidth = 160;
    }
    s.tagColumnWidth = std::min(s.tagColumnWidth, 500);
    s.explorerDotSize = Wh_GetIntSetting(L"explorerDotSize");
    if (s.explorerDotSize < 4) {
        s.explorerDotSize = 10;
    }
    s.explorerDotSize = std::min(s.explorerDotSize, 32);

    s.desktopDotSize = Wh_GetIntSetting(L"desktopDotSize");
    if (s.desktopDotSize < 4) {
        s.desktopDotSize = 8;
    }
    s.desktopDotSize = std::min(s.desktopDotSize, 32);
    s.autoAddTagColumn = Wh_GetIntSetting(L"autoAddTagColumn") != 0;

    auto loaded = std::make_shared<const Settings>(std::move(s));
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = loaded;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ---------------------------------------------------------------------------
// Database: one row per (file, tag), in tags.tsv inside the storage Windhawk
// keeps for this mod, in a folder per user (that storage is machine-wide, and
// this isn't: its rows point inside one profile). It stays OUTSIDE the tags
// folder: inside it, it would get lost when moving the folder (the sync could
// run mid-copy, with the database already copied but not the shortcuts) or
// when deleting it. Outside, a deleted or new folder gets rebuilt from it.
// It's written to a new file that only then replaces the old one. A named
// mutex serializes all the explorer.exe processes of that user.
//
// Only the worker thread touches the disk and the mutex; the window thread
// never waits.
// ---------------------------------------------------------------------------

// Hidden file that says "all shortcuts in this folder were created by us":
// only then does a missing shortcut mean the user deleted it.
#define MARKER_FILE_NAME L".tag"

struct Row {
    std::wstring volume;  // \\?\Volume{GUID}\ or empty (network, FAT without an ID)
    std::wstring fileId;  // 32 hex digits or empty
    std::wstring tag;
    std::wstring lnk;     // shortcut name inside the tag's folder
    std::wstring path;    // last known path
};

HANDLE g_dbMutex;
HANDLE g_stopEvent;

bool Stopping() {
    return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

class DbLock {
   public:
    DbLock() {
        HANDLE handles[2] = {g_stopEvent, g_dbMutex};
        DWORD r = WaitForMultipleObjects(2, handles, FALSE, 30000);
        m_ok = (r == WAIT_OBJECT_0 + 1 || r == WAIT_ABANDONED_0 + 1);
    }
    ~DbLock() {
        if (m_ok) {
            ReleaseMutex(g_dbMutex);
        }
    }
    bool ok() const { return m_ok; }

   private:
    bool m_ok;
};

// The account this Explorer runs as, as a SID string. Windhawk's storage is
// machine-wide and the record is per user by nature (its rows point inside one
// profile, and the default tags folder is under it), so each user gets a
// folder of their own in there.
std::wstring CurrentUserKey() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return L"";
    }
    DWORD needed = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &needed);
    std::wstring key;
    if (needed) {
        std::vector<BYTE> buf(needed);
        if (GetTokenInformation(token, TokenUser, buf.data(), needed, &needed)) {
            PWSTR sid = nullptr;
            if (ConvertSidToStringSidW(((TOKEN_USER*)buf.data())->User.Sid, &sid)) {
                key = sid;
                LocalFree(sid);
            }
        }
    }
    CloseHandle(token);
    return key;
}

std::wstring DbFolder() {
    // Asked once: the answer can't change while the mod is loaded.
    static std::wstring folder = [] {
        WCHAR base[MAX_PATH * 2];
        size_t n = Wh_GetModStoragePath(base, ARRAYSIZE(base));
        std::wstring user = CurrentUserKey();
        if (n == 0 || user.empty()) {
            Wh_Log(L"No storage path for the record");
            return std::wstring();
        }
        return std::wstring(base) + L"\\" + user;
    }();
    return folder;
}

std::wstring DbPath() {
    std::wstring folder = DbFolder();
    // Not created here: reading a path shouldn't create a folder. The two
    // functions that write call EnsureParentFolder first.
    return folder.empty() ? L"" : folder + L"\\tags.tsv";
}

void EnsureParentFolder(const std::wstring& filePath) {
    size_t slash = filePath.rfind(L'\\');
    if (slash != std::wstring::npos) {
        SHCreateDirectoryExW(nullptr, filePath.substr(0, slash).c_str(), nullptr);
    }
}

std::wstring FromUtf8(const std::string& s) {
    if (s.empty()) {
        return L"";
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

// A path with a lone surrogate doesn't survive being saved as UTF-8 (it comes
// out as U+FFFD and the shortcut stops matching): those aren't tagged.
bool IsUtf8Safe(const std::wstring& w) {
    return w.empty() || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w.data(), (int)w.size(),
                                            nullptr, 0, nullptr, nullptr) > 0;
}

std::string ToUtf8(const std::wstring& w) {
    if (w.empty()) {
        return "";
    }
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}

// A shortcut name coming from the database is only accepted if it's a .lnk
// with no path. It's what decides what gets deleted: "..\..\thesis.docx"
// would delete a real file.
bool IsSafeLinkName(const std::wstring& name) {
    if (name.size() < 5 || name.size() > 255 || name[0] == L'.' ||
        name.find_first_of(L"\\/:") != std::wstring::npos) {
        return false;
    }
    return _wcsicmp(name.c_str() + name.size() - 4, L".lnk") == 0;
}

// Returns false if the file exists but couldn't be read: in that case the
// caller must not save, or it would wipe out the whole database.
bool LoadRows(const std::wstring& dbPath, std::vector<Row>& rows) {
    rows.clear();
    HANDLE h = CreateFileW(dbPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        // Measured 2026-09-18: a missing parent folder gives
        // ERROR_PATH_NOT_FOUND, not ERROR_FILE_NOT_FOUND. Treating that as a
        // read error made every operation bail out on a machine where nothing
        // had been tagged yet, silently and forever.
        return err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND;
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(h, &size) || size.QuadPart > 64 * 1024 * 1024) {
        CloseHandle(h);
        return false;
    }
    std::string data((size_t)size.QuadPart, '\0');
    DWORD read = 0;
    bool ok = data.empty() || ReadFile(h, data.data(), (DWORD)data.size(), &read, nullptr);
    CloseHandle(h);
    if (!ok || read != data.size()) {
        return false;
    }

    std::wstring text = FromUtf8(data);
    size_t pos = 0;
    bool first = true;
    while (pos < text.size()) {
        size_t eol = text.find(L'\n', pos);
        if (eol == std::wstring::npos) {
            eol = text.size();
        }
        std::wstring line = text.substr(pos, eol - pos);
        pos = eol + 1;
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        if (first) {
            first = false;
            if (line != L"explorer-tags v1") {
                return false;
            }
            continue;
        }
        if (line.empty()) {
            continue;
        }
        std::vector<std::wstring> f;
        size_t start = 0;
        while (true) {
            size_t tab = line.find(L'\t', start);
            if (tab == std::wstring::npos) {
                f.push_back(line.substr(start));
                break;
            }
            f.push_back(line.substr(start, tab - start));
            start = tab + 1;
        }
        if (f.size() != 5 || !IsSafeLinkName(f[3]) || f[2].empty() || f[4].empty()) {
            continue;
        }
        rows.push_back(Row{f[0], f[1], f[2], f[3], f[4]});
    }
    return true;
}

bool SaveRows(const std::wstring& dbPath, const std::vector<Row>& rows) {
    std::wstring text = L"explorer-tags v1\r\n";
    for (const auto& r : rows) {
        text += r.volume + L"\t" + r.fileId + L"\t" + r.tag + L"\t" + r.lnk + L"\t" + r.path + L"\r\n";
    }
    std::string data = ToUtf8(text);

    EnsureParentFolder(dbPath);
    std::wstring tmp = dbPath + L".tmp";
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_HIDDEN, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Couldn't create %s: %u", tmp.c_str(), GetLastError());
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(h, data.data(), (DWORD)data.size(), &written, nullptr) &&
              written == data.size() && FlushFileBuffers(h);
    CloseHandle(h);
    if (!ok || !MoveFileExW(tmp.c_str(), dbPath.c_str(),
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"Couldn't save the database: %u", GetLastError());
        DeleteFileW(tmp.c_str());
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// File identity
// ---------------------------------------------------------------------------

enum class Exists { Yes, No, Error };

// "Doesn't exist" only with an explicit answer from the system; any other
// failure (busy disk, access denied, network) is "unknown".
Exists CheckExists(const std::wstring& path) {
    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return Exists::Yes;
    }
    DWORD err = GetLastError();
    return (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) ? Exists::No : Exists::Error;
}

bool PathExists(const std::wstring& path) {
    return CheckExists(path) == Exists::Yes;
}

std::wstring HexBytes(const BYTE* b, size_t n) {
    static const wchar_t* digits = L"0123456789ABCDEF";
    std::wstring s;
    for (size_t i = 0; i < n; i++) {
        s += digits[b[i] >> 4];
        s += digits[b[i] & 0xF];
    }
    return s;
}

bool ParseHexBytes(const std::wstring& s, BYTE* out, size_t n) {
    if (s.size() != n * 2) {
        return false;
    }
    for (size_t i = 0; i < n; i++) {
        wchar_t pair[3] = {s[i * 2], s[i * 2 + 1], 0};
        wchar_t* end = nullptr;
        unsigned long v = wcstoul(pair, &end, 16);
        if (!end || *end) {
            return false;
        }
        out[i] = (BYTE)v;
    }
    return true;
}

void GetFileIdentity(const std::wstring& path, std::wstring& volume, std::wstring& fileId) {
    volume.clear();
    fileId.clear();

    WCHAR mount[MAX_PATH];
    WCHAR volName[64];
    if (!GetVolumePathNameW(path.c_str(), mount, ARRAYSIZE(mount)) ||
        !GetVolumeNameForVolumeMountPointW(mount, volName, ARRAYSIZE(volName))) {
        return;
    }

    HANDLE h = CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                           OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return;
    }
    FILE_ID_INFO info;
    if (GetFileInformationByHandleEx(h, FileIdInfo, &info, sizeof(info))) {
        volume = volName;
        fileId = HexBytes(info.FileId.Identifier, sizeof(info.FileId.Identifier));
    }
    CloseHandle(h);
}

std::wstring StripLongPathPrefix(const std::wstring& p) {
    if (p.rfind(L"\\\\?\\UNC\\", 0) == 0) {
        return L"\\\\" + p.substr(8);
    }
    if (p.rfind(L"\\\\?\\", 0) == 0 && p.size() > 6 && p[5] == L':') {
        return p.substr(4);
    }
    return p;
}

bool IsInRecycleBin(const std::wstring& path) {
    return StrStrIW(path.c_str(), L"\\$Recycle.Bin\\") != nullptr;
}

enum class Resolved { Found, FoundByPath, Gone, Unknown };

struct VolumeHandles {
    std::vector<std::pair<std::wstring, HANDLE>> items;
    ~VolumeHandles() {
        for (auto& it : items) {
            if (it.second != INVALID_HANDLE_VALUE) {
                CloseHandle(it.second);
            }
        }
    }
    HANDLE Get(const std::wstring& volume) {
        for (auto& it : items) {
            if (it.first == volume) {
                return it.second;
            }
        }
        HANDLE h = CreateFileW(volume.c_str(), FILE_READ_ATTRIBUTES,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                               OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        items.push_back({volume, h});
        return h;
    }
};

// "Gone" only when the system explicitly says the file no longer exists.
// When in doubt it's "Unknown", and the row stays as it is.
Resolved ResolveRow(const Row& r, VolumeHandles& volumes, std::wstring& currentPath) {
    BYTE id[16];
    if (!r.volume.empty() && ParseHexBytes(r.fileId, id, sizeof(id))) {
        HANDLE vol = volumes.Get(r.volume);
        if (vol == INVALID_HANDLE_VALUE) {
            // Drive unplugged or locked.
            return Resolved::Unknown;
        }
        FILE_ID_DESCRIPTOR desc = {};
        desc.dwSize = sizeof(desc);
        desc.Type = ExtendedFileIdType;
        memcpy(desc.ExtendedFileId.Identifier, id, sizeof(id));
        HANDLE h = OpenFileById(vol, &desc, FILE_READ_ATTRIBUTES,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                FILE_FLAG_BACKUP_SEMANTICS);
        if (h != INVALID_HANDLE_VALUE) {
            WCHAR buf[4096];
            DWORD n = GetFinalPathNameByHandleW(h, buf, ARRAYSIZE(buf),
                                                FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
            CloseHandle(h);
            if (n == 0 || n >= ARRAYSIZE(buf)) {
                return Resolved::Unknown;
            }
            currentPath = StripLongPathPrefix(buf);
            // In the Recycle Bin the file still exists, so the row and its
            // shortcut are left alone, pointing where the file used to be:
            // restoring it brings its tags back. Emptying the bin is what
            // makes it Gone, by the path below no longer resolving.
            return IsInRecycleBin(currentPath) ? Resolved::Unknown : Resolved::Found;
        }
        // Measured on NTFS (E: and C:, 2026-09-17): a deleted file's ID gives
        // ERROR_INVALID_PARAMETER (87).
        DWORD err = GetLastError();
        if (err != ERROR_INVALID_PARAMETER && err != ERROR_FILE_NOT_FOUND &&
            err != ERROR_PATH_NOT_FOUND) {
            return Resolved::Unknown;
        }
        BY_HANDLE_FILE_INFORMATION volInfo;
        if (!GetFileInformationByHandle(vol, &volInfo)) {
            // The volume handle died (drive removed mid-operation).
            return Resolved::Unknown;
        }
    }

    switch (CheckExists(r.path)) {
        case Exists::Yes:
            currentPath = r.path;
            return r.fileId.empty() ? Resolved::Found : Resolved::FoundByPath;
        case Exists::Error:
            return Resolved::Unknown;
        case Exists::No:
            break;
    }

    if (r.volume.empty()) {
        // Network path with no ID: if the share doesn't respond, it's unknown.
        WCHAR root[MAX_PATH];
        lstrcpynW(root, r.path.c_str(), ARRAYSIZE(root));
        if (!PathStripToRootW(root) || CheckExists(root) != Exists::Yes) {
            return Resolved::Unknown;
        }
    }
    return Resolved::Gone;
}

// ---------------------------------------------------------------------------
// Shortcuts
// ---------------------------------------------------------------------------

// Explorer windows showing a tag folder don't notice plain file API changes:
// measured 2026-09-17, a renamed and then deleted shortcut stayed on screen.
// Every change to a tag folder is announced to the shell.
void NotifyShell(LONG event, const std::wstring& path) {
    SHChangeNotify(event, SHCNF_PATHW, path.c_str(), nullptr);
}

bool WriteShortcut(const std::wstring& lnkPath, const std::wstring& target) {
    bool existed = CheckExists(lnkPath) == Exists::Yes;
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) {
        return false;
    }
    bool ok = false;
    if (SUCCEEDED(link->SetPath(target.c_str()))) {
        std::wstring dir = target;
        PathRemoveFileSpecW(dir.data());
        dir.resize(wcslen(dir.c_str()));
        link->SetWorkingDirectory(dir.c_str());
        IPersistFile* file = nullptr;
        if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file)))) {
            ok = SUCCEEDED(file->Save(lnkPath.c_str(), TRUE));
            file->Release();
        }
    }
    link->Release();
    if (ok) {
        NotifyShell(existed ? SHCNE_UPDATEITEM : SHCNE_CREATE, lnkPath);
    }
    return ok;
}

bool ReadShortcutTarget(const std::wstring& lnkPath, std::wstring& target) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) {
        return false;
    }
    bool ok = false;
    IPersistFile* file = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file)))) {
        if (SUCCEEDED(file->Load(lnkPath.c_str(), STGM_READ))) {
            WCHAR buf[4096];
            if (link->GetPath(buf, ARRAYSIZE(buf), nullptr, SLGP_RAWPATH) == S_OK && *buf) {
                target = buf;
                ok = true;
            }
        }
        file->Release();
    }
    link->Release();
    return ok;
}

// The only function that deletes: only a .lnk directly inside the tag's
// folder. Says whether the shortcut is gone, which the caller has to know:
// one left behind is one the folder still counts as tagged.
bool DeleteLinkIn(const std::wstring& folder, const std::wstring& name) {
    if (!IsSafeLinkName(name)) {
        return false;
    }
    std::wstring lnkPath = folder + L"\\" + name;
    if (!DeleteFileW(lnkPath.c_str())) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            return true;
        }
        // Read-only is worth one more try: a shortcut restored from a backup
        // or a copy carries the attribute and nothing else is in the way.
        if (err == ERROR_ACCESS_DENIED &&
            SetFileAttributesW(lnkPath.c_str(), FILE_ATTRIBUTE_NORMAL) &&
            DeleteFileW(lnkPath.c_str())) {
            NotifyShell(SHCNE_DELETE, lnkPath);
            return true;
        }
        Wh_Log(L"Couldn't delete %s: %u", lnkPath.c_str(), err);
        return false;
    }
    NotifyShell(SHCNE_DELETE, lnkPath);
    return true;
}

std::wstring LinkBaseName(const std::wstring& path) {
    std::wstring p = path;
    while (p.size() > 3 && p.back() == L'\\') {
        p.pop_back();
    }
    std::wstring name = PathFindFileNameW(p.c_str());
    if (name.empty() || name.find(L':') != std::wstring::npos) {
        // Drive root, for example E:\ .
        name = L"Drive " + p.substr(0, 1);
    }
    name = SanitizeFileName(name);
    while (!name.empty() && name[0] == L'.') {
        name.erase(0, 1);
    }
    return name.empty() ? L"Item" : name;
}

std::wstring UniqueLinkName(const std::wstring& folder, const std::wstring& baseName,
                            const std::vector<Row>& rows, const std::wstring& tag,
                            const Row* ignore) {
    std::wstring stem = baseName;
    std::wstring ext;
    size_t dot = baseName.rfind(L'.');
    if (dot != std::wstring::npos && dot > 0 && baseName.size() - dot <= 16) {
        stem = baseName.substr(0, dot);
        ext = baseName.substr(dot);
    }
    // Final name (with " (n)" and ".lnk") always under the 255 that
    // IsSafeLinkName accepts when reading.
    if (stem.size() + ext.size() > 200) {
        stem.resize(200 - ext.size());
        // Don't leave half an emoji at the end (it would come out as U+FFFD when saved).
        if (!stem.empty() && IS_HIGH_SURROGATE(stem.back())) {
            stem.pop_back();
        }
    }
    for (int n = 1;; n++) {
        std::wstring name =
            (n == 1 ? stem + ext : stem + L" (" + std::to_wstring(n) + L")" + ext) + L".lnk";
        bool taken = CheckExists(folder + L"\\" + name) != Exists::No;
        for (const auto& r : rows) {
            if (&r != ignore && r.tag == tag && _wcsicmp(r.lnk.c_str(), name.c_str()) == 0) {
                taken = true;
            }
        }
        if (!taken || n > 10000) {
            return name;
        }
    }
}

// ---------------------------------------------------------------------------
// Synchronization (only on the worker thread, with the DbLock held)
// ---------------------------------------------------------------------------

struct PendingDelete {
    std::wstring folder;
    std::wstring lnk;
};

// What's only done after the database has been saved: if saving fails, the
// database still points to shortcuts that exist, and to folders whose
// markers would make missing shortcuts count as removed tags.
struct Pending {
    std::vector<PendingDelete> deletes;
    std::vector<std::wstring> markers;
};

std::wstring LowerPath(const std::wstring& path) {
    std::wstring lower = path;
    if (!lower.empty()) {
        CharLowerBuffW(lower.data(), (DWORD)lower.size());
    }
    return lower;
}

// Shortcuts in a tag's folder that no row accounts for. Taking them in is how
// a lost record comes back: the folder says which tag, the shortcut says which
// file, and the file says who it is. It is also what makes dropping a shortcut
// into a tag's folder, or moving one there from another tag, count as tagging.
//
// Only in a folder the mod marked as its own, and never for a shortcut that
// points at nothing or that this sync is about to delete.
void AdoptStrays(std::vector<Row>& rows, const std::wstring& tag, const std::wstring& folder,
                 const std::wstring& root, const Pending& pending, bool& changed) {
    std::unordered_set<std::wstring> known;
    std::unordered_set<std::wstring> taggedPaths;
    std::unordered_set<std::wstring> taggedIds;
    for (const auto& r : rows) {
        if (r.tag == tag) {
            known.insert(LowerPath(r.lnk));
            taggedPaths.insert(LowerPath(r.path));
            if (!r.fileId.empty()) {
                taggedIds.insert(r.volume + L"\t" + r.fileId);
            }
        }
    }
    for (const auto& d : pending.deletes) {
        if (_wcsicmp(d.folder.c_str(), folder.c_str()) == 0) {
            known.insert(LowerPath(d.lnk));
        }
    }

    std::wstring rootPrefix = root + L"\\";
    WIN32_FIND_DATAW find;
    HANDLE h = FindFirstFileExW((folder + L"\\*.lnk").c_str(), FindExInfoBasic, &find,
                                FindExSearchNameMatch, nullptr, 0);
    if (h == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        if (Stopping()) {
            break;
        }
        if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        std::wstring name = find.cFileName;
        if (!IsSafeLinkName(name) || !known.insert(LowerPath(name)).second) {
            continue;
        }
        std::wstring target;
        if (!ReadShortcutTarget(folder + L"\\" + name, target) || target.empty() ||
            !IsUtf8Safe(target) || CheckExists(target) != Exists::Yes) {
            // Points nowhere, or at a name that wouldn't survive being
            // saved: left alone, not deleted. It isn't ours to remove.
            continue;
        }
        // The tag folders themselves don't get tagged, the same rule the
        // menu follows: a shortcut to a shortcut, or to another tag's
        // folder, would make the mod manage its own insides.
        if (_wcsicmp(target.c_str(), root.c_str()) == 0 ||
            (target.size() > rootPrefix.size() &&
             _wcsnicmp(target.c_str(), rootPrefix.c_str(), rootPrefix.size()) == 0)) {
            continue;
        }
        // Two shortcuts to the same file under one tag are one tag. By
        // identity first, like everywhere else: the same file reached
        // through another spelling is the same file.
        std::wstring volume, fileId;
        GetFileIdentity(target, volume, fileId);
        if (!fileId.empty() && !taggedIds.insert(volume + L"\t" + fileId).second) {
            continue;
        }
        if (!taggedPaths.insert(LowerPath(target)).second) {
            continue;
        }
        Row row;
        row.tag = tag;
        row.lnk = name;
        row.path = target;
        row.volume = volume;
        row.fileId = fileId;
        rows.push_back(row);
        changed = true;
        Wh_Log(L"Took in %s under %s", name.c_str(), tag.c_str());
    } while (FindNextFileW(h, &find));
    FindClose(h);
}

void SyncTag(std::vector<Row>& rows, const std::wstring& tag, const std::wstring& folder,
             const std::wstring& root, bool ignoreMarkers, VolumeHandles& volumes, bool& changed,
             Pending& pending) {
    Exists folderState = CheckExists(folder);
    if (folderState == Exists::Error) {
        return;
    }
    if (folderState == Exists::No) {
        // A tag with nothing in it gets no folder: enabling the mod, or
        // adding a tag in the settings, shouldn't create folders on its own.
        bool hasRows = false;
        for (const auto& r : rows) {
            if (r.tag == tag) {
                hasRows = true;
                break;
            }
        }
        if (!hasRows) {
            return;
        }
        int err = SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);
        if (err != ERROR_SUCCESS && err != ERROR_ALREADY_EXISTS) {
            return;
        }
        NotifyShell(SHCNE_MKDIR, folder);
    }
    std::wstring marker = folder + L"\\" MARKER_FILE_NAME;
    Exists markerState = CheckExists(marker);
    if (markerState == Exists::Error) {
        return;
    }
    // Without the marker, the folder is new or was recreated empty: missing
    // shortcuts get rebuilt instead of counting as removed tags.
    // Tags folder changed since the last full sync: the markers there are
    // from another time, and the shortcuts may have different names.
    bool trusted = markerState == Exists::Yes && !ignoreMarkers;
    bool complete = true;
    // Whether this folder is the mod's: it made it, or it keeps a tag's
    // shortcuts in it. See the marker at the end.
    bool ours = folderState == Exists::No;

    for (size_t i = 0; i < rows.size();) {
        if (Stopping()) {
            return;
        }
        if (rows[i].tag != tag) {
            i++;
            continue;
        }
        // A row of this tag lives in this folder, so the folder is the
        // mod's whoever created it.
        ours = true;
        Row& r = rows[i];

        Exists lnkState = CheckExists(folder + L"\\" + r.lnk);
        if (lnkState == Exists::Error) {
            complete = false;
            i++;
            continue;
        }
        if (lnkState == Exists::No && trusted) {
            rows.erase(rows.begin() + i);
            changed = true;
            continue;
        }

        std::wstring current;
        Resolved res = ResolveRow(r, volumes, current);
        if ((res == Resolved::Found || res == Resolved::FoundByPath) && !IsUtf8Safe(current)) {
            // Changed to a name that doesn't survive saving: leave it as it was.
            res = Resolved::Unknown;
        }
        if (res == Resolved::Unknown) {
            if (lnkState == Exists::No && !WriteShortcut(folder + L"\\" + r.lnk, r.path)) {
                complete = false;
            }
            i++;
            continue;
        }
        if (res == Resolved::Gone) {
            if (lnkState == Exists::Yes) {
                pending.deletes.push_back({folder, r.lnk});
            }
            rows.erase(rows.begin() + i);
            changed = true;
            continue;
        }
        if (res == Resolved::FoundByPath) {
            std::wstring vol, id;
            GetFileIdentity(current, vol, id);
            if (!id.empty() && (vol != r.volume || id != r.fileId)) {
                r.volume = vol;
                r.fileId = id;
                changed = true;
            }
        }

        if (current != r.path || lnkState == Exists::No) {
            std::wstring newName = r.lnk;
            if (_wcsicmp(LinkBaseName(current).c_str(), LinkBaseName(r.path).c_str()) != 0) {
                newName = UniqueLinkName(folder, LinkBaseName(current), rows, tag, &r);
            }
            if (WriteShortcut(folder + L"\\" + newName, current)) {
                if (_wcsicmp(newName.c_str(), r.lnk.c_str()) != 0 && lnkState == Exists::Yes) {
                    pending.deletes.push_back({folder, r.lnk});
                }
                r.lnk = newName;
                r.path = current;
                changed = true;
            } else if (lnkState == Exists::No) {
                complete = false;
            }
        }
        i++;
    }

    // An empty folder is nobody's: there is nothing in it to hand to the
    // tag, so claiming it costs nothing and is what lets a folder the panel
    // opened — made without a marker, before anything was tagged — take in
    // the first shortcut dropped into it.
    if (!ours && !trusted) {
        WIN32_FIND_DATAW find;
        HANDLE h = FindFirstFileExW((folder + L"\\*.lnk").c_str(), FindExInfoBasic, &find,
                                    FindExSearchNameMatch, nullptr, 0);
        if (h == INVALID_HANDLE_VALUE) {
            // Empty, and not some other reason for failing to look.
            ours = GetLastError() == ERROR_FILE_NOT_FOUND;
        } else {
            FindClose(h);
        }
    }

    // Only in a folder that is ours and whose names are of this time: after
    // the tags folder has been changed in the settings, the shortcuts in
    // there are from another arrangement and the rows above are being
    // rewritten anyway.
    if (trusted) {
        AdoptStrays(rows, tag, folder, root, pending, changed);
    }

    // The marker only goes down after saving: if saving fails, the database
    // still has the shortcuts' old names, and a marked folder would remove
    // those tags.
    //
    // And only on a folder this mod put something in. Claiming one it merely
    // found — the tags folder pointed at a place that already had a folder of
    // that name in it — would hand every shortcut already in there to the
    // tag on the next sync.
    if (!trusted && complete && ours) {
        pending.markers.push_back(marker);
    }
}

void Commit(const std::wstring& dbPath, const std::vector<Row>& rows, bool changed,
            const Pending& pending) {
    if (changed && !SaveRows(dbPath, rows)) {
        return;
    }
    for (const auto& d : pending.deletes) {
        DeleteLinkIn(d.folder, d.lnk);
    }
    for (const auto& marker : pending.markers) {
        HANDLE h = CreateFileW(marker.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                               FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
        }
    }
}

std::wstring TargetIfTagShortcut(const std::wstring& path, const std::wstring& root);

// Lowered path -> its tags. Keyed and lowered by the worker: a right-click
// then costs one lookup per selected file instead of a walk, and an
// allocation, over every row there is.
std::unordered_map<std::wstring, std::vector<std::wstring>> g_taggedByPath;
std::unordered_map<std::wstring, std::vector<std::wstring>> g_taggedByFolderAndStem;
std::unordered_set<std::wstring> g_taggedFolders;
std::unordered_map<std::wstring, std::wstring> g_linkTargets;     // lnk -> target
SRWLOCK g_countsLock = SRWLOCK_INIT;

void UpdateCounts(const std::vector<Row>& rows, const Settings& s) {
    std::unordered_map<std::wstring, std::vector<std::wstring>> taggedByPath;
    taggedByPath.reserve(rows.size());
    std::unordered_map<std::wstring, std::vector<std::wstring>> taggedByFolderAndStem;
    taggedByFolderAndStem.reserve(rows.size());
    std::unordered_set<std::wstring> taggedFolders;
    std::unordered_map<std::wstring, std::wstring> linkTargets;
    linkTargets.reserve(rows.size());

    for (const auto& r : rows) {
        std::wstring lower = LowerPath(r.path);
        taggedByPath[lower].push_back(r.tag);
        linkTargets[LowerPath(s.root + L"\\" + FolderNameFor(r.tag) + L"\\" + r.lnk)] = r.path;

        size_t lastSlash = lower.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            std::wstring dir = lower.substr(0, lastSlash);
            taggedFolders.insert(dir);
            std::wstring filename = lower.substr(lastSlash + 1);
            size_t dot = filename.find_last_of(L'.');
            std::wstring stem = (dot != std::wstring::npos) ? filename.substr(0, dot) : filename;
            taggedByFolderAndStem[dir + L"\\" + stem].push_back(r.tag);
        }
    }

    AcquireSRWLockExclusive(&g_countsLock);
    g_taggedByPath.swap(taggedByPath);
    g_taggedByFolderAndStem.swap(taggedByFolderAndStem);
    g_taggedFolders.swap(taggedFolders);
    g_linkTargets.swap(linkTargets);
    ReleaseSRWLockExclusive(&g_countsLock);
}

// Window thread: a tag folder shortcut stands for the file it points to. Only
// the worker's copy is consulted; an unknown shortcut is left as it is, and
// the worker resolves it from disk when it processes the request.
std::wstring TargetIfTagShortcutCached(const std::wstring& path) {
    std::wstring target;
    AcquireSRWLockShared(&g_countsLock);
    auto it = g_linkTargets.find(LowerPath(path));
    if (it != g_linkTargets.end()) {
        target = it->second;
    }
    ReleaseSRWLockShared(&g_countsLock);
    return target.empty() ? path : target;
}

struct TagHits {
    std::unordered_map<std::wstring, int> perTag;  // tag -> selected items that have it
    int withAnyTag = 0;
};

// A single pass over the registry copy: runs on the window thread when the
// menu opens, and with thousands of rows and selected items, comparing each
// against each would hang Explorer.
TagHits CountTagHits(const std::vector<std::wstring>& paths) {
    TagHits hits;
    std::unordered_set<std::wstring> seen;
    AcquireSRWLockShared(&g_countsLock);
    for (const auto& p : paths) {
        std::wstring lower = LowerPath(p);
        // The same file twice in the selection counts once.
        if (!seen.insert(lower).second) {
            continue;
        }
        auto it = g_taggedByPath.find(lower);
        if (it == g_taggedByPath.end() || it->second.empty()) {
            continue;
        }
        std::unordered_set<std::wstring> perFile;
        for (const auto& tag : it->second) {
            // And so does the same tag twice on one file.
            if (perFile.insert(tag).second) {
                hits.perTag[tag]++;
            }
        }
        hits.withAnyTag++;
    }
    ReleaseSRWLockShared(&g_countsLock);
    return hits;
}


// Only a full SyncAll saves the root; until then, any sync ignores the
// markers (rebuilds instead of removing tags). It lives next to the database,
// in the same per-user folder and for the same reason.
std::wstring LastRootPath() {
    std::wstring folder = DbFolder();
    return folder.empty() ? L"" : folder + L"\\last-root.txt";
}

// "The folder was changed in the settings" and "I have no memory of any
// folder" are not the same thing, and treating them alike deadlocked the
// rebuild: the memory lives beside the record, so whatever takes the record
// takes the memory too, and a folder that is only remembered once there are
// rows could never be remembered again.
enum class RootMemory { Same, Changed, Unknown };

RootMemory LastRootState(const Settings& s) {
    std::wstring file = LastRootPath();
    HANDLE h = file.empty() ? INVALID_HANDLE_VALUE
                            : CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                          OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return RootMemory::Unknown;
    }
    char buf[4096];
    DWORD read = 0;
    bool ok = ReadFile(h, buf, sizeof(buf), &read, nullptr);
    CloseHandle(h);
    if (!ok) {
        return RootMemory::Unknown;
    }
    return FromUtf8(std::string(buf, read)) == s.root ? RootMemory::Same : RootMemory::Changed;
}

void SaveLastRoot(const Settings& s) {
    std::wstring file = LastRootPath();
    if (file.empty()) {
        return;
    }
    std::string data = ToUtf8(s.root);
    EnsureParentFolder(file);
    std::wstring tmp = file + L".tmp";
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD written = 0;
    bool ok = WriteFile(h, data.data(), (DWORD)data.size(), &written, nullptr) && written == data.size();
    CloseHandle(h);
    if (!ok || !MoveFileExW(tmp.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        DeleteFileW(tmp.c_str());
    }
}

void SyncAll() {
    auto s = GetSettings();
    DbLock lock;
    if (!lock.ok()) {
        return;
    }
    std::wstring dbPath = DbPath();
    std::vector<Row> rows;
    if (!LoadRows(dbPath, rows)) {
        Wh_Log(L"Unreadable database, leaving it alone: %s", dbPath.c_str());
        return;
    }
    VolumeHandles volumes;
    bool changed = false;
    Pending pending;
    RootMemory rootState = LastRootState(*s);
    bool stopped = false;
    for (const auto& t : s->tags) {
        if (Stopping()) {
            stopped = true;
            break;
        }
        SyncTag(rows, t.name, s->root + L"\\" + t.folderName, s->root,
                rootState == RootMemory::Changed, volumes, changed, pending);
    }
    stopped |= Stopping();
    Commit(dbPath, rows, changed, pending);
    // Only worth remembering next to a database that exists: otherwise
    // merely enabling the mod would create the folder, which the readme says
    // it doesn't. Not "once there are rows": a record emptied of them, with
    // the folder changed in the settings, would then never be remembered,
    // and never trust a marker again.
    if (rootState != RootMemory::Same && !stopped && CheckExists(dbPath) == Exists::Yes) {
        SaveLastRoot(*s);
    }
    UpdateCounts(rows, *s);
}

// A shortcut from inside a tag's folder stands for the file it targets.
std::wstring TargetIfTagShortcut(const std::wstring& path, const std::wstring& root) {
    std::wstring rootPrefix = root + L"\\";
    std::wstring target;
    if (path.size() > rootPrefix.size() &&
        _wcsnicmp(path.c_str(), rootPrefix.c_str(), rootPrefix.size()) == 0 &&
        _wcsicmp(PathFindExtensionW(path.c_str()), L".lnk") == 0 && ReadShortcutTarget(path, target)) {
        return target;
    }
    return path;
}

void AddPaths(const std::wstring& tag, const std::vector<std::wstring>& paths) {
    auto s = GetSettings();
    std::wstring folder;
    for (const auto& t : s->tags) {
        if (t.name == tag) {
            folder = s->root + L"\\" + t.folderName;
        }
    }
    if (folder.empty()) {
        return;
    }

    DbLock lock;
    if (!lock.ok()) {
        return;
    }
    std::wstring dbPath = DbPath();
    std::vector<Row> rows;
    if (!LoadRows(dbPath, rows)) {
        Wh_Log(L"Unreadable database, leaving it alone: %s", dbPath.c_str());
        return;
    }

    // Put the folder in order first (and marked) before adding shortcuts to it.
    VolumeHandles volumes;
    bool changed = false;
    Pending pending;
    SyncTag(rows, tag, folder, s->root, LastRootState(*s) == RootMemory::Changed, volumes,
            changed, pending);
    if (CheckExists(folder) == Exists::No) {
        // First file of this tag: this is where its folder is born.
        int err = SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);
        if (err == ERROR_SUCCESS) {
            NotifyShell(SHCNE_MKDIR, folder);
        }
    }
    if (CheckExists(folder) != Exists::Yes) {
        Commit(dbPath, rows, changed, pending);
        return;
    }

    std::wstring rootPrefix = s->root + L"\\";
    for (std::wstring path : paths) {
        if (Stopping()) {
            break;
        }
        path = TargetIfTagShortcut(path, s->root);
        if (!IsUtf8Safe(path) || !PathExists(path)) {
            continue;
        }
        if (_wcsnicmp(path.c_str(), rootPrefix.c_str(), rootPrefix.size()) == 0 ||
            _wcsicmp(path.c_str(), s->root.c_str()) == 0) {
            // The tag folders themselves don't get tagged.
            continue;
        }

        Row row;
        GetFileIdentity(path, row.volume, row.fileId);
        row.tag = tag;
        row.path = path;

        bool already = false;
        for (const auto& r : rows) {
            if (r.tag != tag) {
                continue;
            }
            if ((!row.fileId.empty() && r.volume == row.volume && r.fileId == row.fileId) ||
                _wcsicmp(r.path.c_str(), path.c_str()) == 0) {
                already = true;
            }
        }
        if (already) {
            continue;
        }

        row.lnk = UniqueLinkName(folder, LinkBaseName(path), rows, tag, nullptr);
        if (!WriteShortcut(folder + L"\\" + row.lnk, path)) {
            Wh_Log(L"Couldn't create the shortcut for %s", path.c_str());
            continue;
        }
        rows.push_back(row);
        changed = true;
    }
    Commit(dbPath, rows, changed, pending);
    UpdateCounts(rows, *s);
}

// Empty tag: removes all tags from these files.
void RemovePaths(const std::wstring& tag, const std::vector<std::wstring>& paths) {
    auto s = GetSettings();
    DbLock lock;
    if (!lock.ok()) {
        return;
    }
    std::wstring dbPath = DbPath();
    std::vector<Row> rows;
    if (!LoadRows(dbPath, rows)) {
        Wh_Log(L"Unreadable database, leaving it alone: %s", dbPath.c_str());
        return;
    }

    // What the user asked to untag, as files.
    struct Target {
        std::wstring lower, volume, fileId;
    };
    std::vector<Target> targets;
    for (const std::wstring& original : paths) {
        if (Stopping()) {
            return;
        }
        Target t;
        std::wstring path = TargetIfTagShortcut(original, s->root);
        t.lower = LowerPath(path);
        GetFileIdentity(path, t.volume, t.fileId);
        targets.push_back(std::move(t));
    }
    auto wanted = [&targets](const std::wstring& path, const std::wstring& volume,
                             const std::wstring& fileId) {
        std::wstring lower = LowerPath(path);
        for (const auto& t : targets) {
            if ((!fileId.empty() && !t.fileId.empty() && t.volume == volume && t.fileId == fileId) ||
                t.lower == lower) {
                return true;
            }
        }
        return false;
    };

    // The shortcuts go first, and every shortcut in the folder that points at
    // the file goes, not only the one the record names: what is left in the
    // folder is what counts as tagged, so one left behind would put the tag
    // straight back on the next sync.
    std::unordered_set<std::wstring> folders;
    for (const auto& r : rows) {
        if ((tag.empty() || r.tag == tag) && wanted(r.path, r.volume, r.fileId)) {
            folders.insert(s->root + L"\\" + FolderNameFor(r.tag));
        }
    }
    std::unordered_set<std::wstring> leftBehind;  // folder + tab + lowered target
    for (const std::wstring& folder : folders) {
        if (Stopping()) {
            return;
        }
        WIN32_FIND_DATAW find;
        HANDLE h = FindFirstFileExW((folder + L"\\*.lnk").c_str(), FindExInfoBasic, &find,
                                    FindExSearchNameMatch, nullptr, 0);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }
        // What the record already says about this folder's shortcuts. Only
        // the names it doesn't know are worth opening: untagging one file in
        // a folder of a thousand shouldn't read a thousand shortcuts.
        std::unordered_map<std::wstring, const Row*> byName;
        for (const auto& r : rows) {
            if (_wcsicmp((s->root + L"\\" + FolderNameFor(r.tag)).c_str(), folder.c_str()) == 0) {
                byName[LowerPath(r.lnk)] = &r;
            }
        }
        do {
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            std::wstring target, vol, id;
            auto known = byName.find(LowerPath(find.cFileName));
            bool fromRecord = known != byName.end();
            if (fromRecord) {
                target = known->second->path;
                vol = known->second->volume;
                id = known->second->fileId;
            } else if (!ReadShortcutTarget(folder + L"\\" + find.cFileName, target)) {
                continue;
            } else {
                GetFileIdentity(target, vol, id);
            }
            if (!wanted(target, vol, id)) {
                continue;
            }
            if (fromRecord) {
                // The record was enough to rule this shortcut out, but not to
                // delete it: one replaced behind the mod's back keeps its name
                // and points somewhere else, and deleting it on the record's
                // word would take another file's tag with it. So the few that
                // are about to go are read from the disk first.
                std::wstring real, realVol, realId;
                if (!ReadShortcutTarget(folder + L"\\" + find.cFileName, real)) {
                    continue;
                }
                GetFileIdentity(real, realVol, realId);
                if (!wanted(real, realVol, realId)) {
                    continue;
                }
                target = real;
                vol = realVol;
                id = realId;
            }
            if (!DeleteLinkIn(folder, find.cFileName)) {
                // Written both ways, because the row may name the file by
                // another spelling than the shortcut does.
                leftBehind.insert(folder + L"\t" + LowerPath(target));
                if (!id.empty()) {
                    leftBehind.insert(folder + L"\t" + vol + L"\t" + id);
                }
            }
        } while (FindNextFileW(h, &find));
        FindClose(h);
    }

    bool changed = false;
    for (size_t i = 0; i < rows.size();) {
        const Row& r = rows[i];
        std::wstring rowFolder = s->root + L"\\" + FolderNameFor(r.tag);
        bool survived = leftBehind.count(rowFolder + L"\t" + LowerPath(r.path)) > 0 ||
                        (!r.fileId.empty() &&
                         leftBehind.count(rowFolder + L"\t" + r.volume + L"\t" + r.fileId) > 0);
        if ((tag.empty() || r.tag == tag) && wanted(r.path, r.volume, r.fileId) && !survived) {
            rows.erase(rows.begin() + i);
            changed = true;
            continue;
        }
        i++;
    }
    Commit(dbPath, rows, changed, Pending());
    UpdateCounts(rows, *s);
}

// ---------------------------------------------------------------------------
// Worker thread: watches the tags folder and does all the disk work.
// ---------------------------------------------------------------------------

enum class WorkKind { Add, Remove };

struct WorkItem {
    std::wstring tag;
    std::vector<std::wstring> paths;
    WorkKind kind;
};

HANDLE g_workerThread;
HANDLE g_workEvent;
std::vector<WorkItem> g_workQueue;
bool g_syncRequested;
SRWLOCK g_workLock = SRWLOCK_INIT;
std::atomic<bool> g_unloading;

void BroadcastRefresh();

void QueueAdd(const std::wstring& tag, std::vector<std::wstring> paths) {
    if (g_unloading) {
        return;
    }
    AcquireSRWLockExclusive(&g_countsLock);
    for (const auto& p : paths) {
        std::wstring lower = LowerPath(p);
        auto& list = g_taggedByPath[lower];
        if (std::find(list.begin(), list.end(), tag) == list.end()) {
            list.push_back(tag);
        }
        size_t lastSlash = lower.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            std::wstring dir = lower.substr(0, lastSlash);
            g_taggedFolders.insert(dir);
            std::wstring filename = lower.substr(lastSlash + 1);
            size_t dot = filename.find_last_of(L'.');
            std::wstring stem = (dot != std::wstring::npos) ? filename.substr(0, dot) : filename;
            auto& stemList = g_taggedByFolderAndStem[dir + L"\\" + stem];
            if (std::find(stemList.begin(), stemList.end(), tag) == stemList.end()) {
                stemList.push_back(tag);
            }
        }
    }
    ReleaseSRWLockExclusive(&g_countsLock);

    AcquireSRWLockExclusive(&g_workLock);
    g_workQueue.push_back(WorkItem{tag, std::move(paths), WorkKind::Add});
    ReleaseSRWLockExclusive(&g_workLock);
    SetEvent(g_workEvent);

    BroadcastRefresh();
}

void QueueRemove(const std::wstring& tag, std::vector<std::wstring> paths) {
    if (g_unloading) {
        return;
    }
    AcquireSRWLockExclusive(&g_countsLock);
    for (const auto& p : paths) {
        std::wstring lower = LowerPath(p);
        auto it = g_taggedByPath.find(lower);
        if (it != g_taggedByPath.end()) {
            if (tag.empty()) {
                it->second.clear();
            } else {
                it->second.erase(std::remove(it->second.begin(), it->second.end(), tag), it->second.end());
            }
        }
        size_t lastSlash = lower.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            std::wstring dir = lower.substr(0, lastSlash);
            std::wstring filename = lower.substr(lastSlash + 1);
            size_t dot = filename.find_last_of(L'.');
            std::wstring stem = (dot != std::wstring::npos) ? filename.substr(0, dot) : filename;
            auto stemIt = g_taggedByFolderAndStem.find(dir + L"\\" + stem);
            if (stemIt != g_taggedByFolderAndStem.end()) {
                if (tag.empty()) {
                    stemIt->second.clear();
                } else {
                    stemIt->second.erase(std::remove(stemIt->second.begin(), stemIt->second.end(), tag), stemIt->second.end());
                }
            }
        }
    }
    ReleaseSRWLockExclusive(&g_countsLock);

    AcquireSRWLockExclusive(&g_workLock);
    g_workQueue.push_back(WorkItem{tag, std::move(paths), WorkKind::Remove});
    ReleaseSRWLockExclusive(&g_workLock);
    SetEvent(g_workEvent);

    BroadcastRefresh();
}

void QueueSync() {
    if (g_unloading) {
        return;
    }
    AcquireSRWLockExclusive(&g_workLock);
    g_syncRequested = true;
    ReleaseSRWLockExclusive(&g_workLock);
    SetEvent(g_workEvent);
}

// The root is never created here: enabling the mod shouldn't create folders.
// It appears on the first tag, drop or tag click, and until then there is
// nothing to watch.
HANDLE WatchRoot(const std::wstring& root) {
    return FindFirstChangeNotificationW(root.c_str(), TRUE,
                                        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
}

// This thread is a single-threaded apartment (shell links are created here),
// so it has to dispatch messages: an apartment that never pumps can deadlock
// anything COM marshals into it, and both StopWorker and unloading wait on
// this thread.
DWORD WaitPumping(DWORD count, const HANDLE* handles, DWORD timeout) {
    ULONGLONG start = GetTickCount64();
    while (true) {
        // What is left of the deadline, not the whole of it again: messages
        // arriving in a stream would otherwise keep pushing it back.
        DWORD remaining = timeout;
        if (timeout != INFINITE) {
            ULONGLONG elapsed = GetTickCount64() - start;
            remaining = elapsed >= timeout ? 0 : (DWORD)(timeout - elapsed);
        }
        DWORD r = MsgWaitForMultipleObjects(count, handles, FALSE, remaining, QS_ALLINPUT);
        if (r != WAIT_OBJECT_0 + count) {
            return r;
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (timeout != INFINITE && GetTickCount64() - start >= timeout) {
            return WAIT_TIMEOUT;
        }
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    std::wstring root = GetSettings()->root;
    HANDLE change = WatchRoot(root);

    SyncAll();
    BroadcastRefresh();

    while (!Stopping()) {
        // The tags folder setting is picked up here instead of by restarting
        // this thread: a restart makes Windhawk's engine thread wait for
        // whatever file operation is in flight, and changing the folder is
        // the user's way out of a location that has stopped responding.
        std::wstring wanted = GetSettings()->root;
        bool rootSwitched = wanted != root;
        if (rootSwitched) {
            if (change != INVALID_HANDLE_VALUE) {
                FindCloseChangeNotification(change);
            }
            root = wanted;
            change = WatchRoot(root);
        }

        HANDLE handles[3] = {g_stopEvent, g_workEvent, change};
        bool watching = change != INVALID_HANDLE_VALUE;
        // With nothing to watch (no tags folder yet) this only polls to see
        // whether it appeared; a wake every 30 s is enough, and anything the
        // user does signals the work event anyway.
        DWORD r = rootSwitched
                      ? (DWORD)WAIT_TIMEOUT
                      : WaitPumping(watching ? 3 : 2, handles, watching ? INFINITE : 30000);
        if (r == WAIT_OBJECT_0) {
            break;
        }

        bool sync = rootSwitched;
        if (r == WAIT_OBJECT_0 + 2) {
            // Wait for the folder to stay quiet for 400 ms (a copy of many
            // files), but never more than 3 s in a row.
            ULONGLONG start = GetTickCount64();
            bool stop = false;
            while (true) {
                if (!FindNextChangeNotification(change)) {
                    // Folder deleted: release the handle (which would keep it
                    // mid-deletion). The watch is armed again by the next
                    // poll, or by the next piece of work.
                    FindCloseChangeNotification(change);
                    change = INVALID_HANDLE_VALUE;
                    break;
                }
                HANDLE waits[2] = {g_stopEvent, change};
                DWORD w = WaitPumping(2, waits, 400);
                if (w == WAIT_OBJECT_0) {
                    stop = true;
                    break;
                }
                if (w != WAIT_OBJECT_0 + 1 || GetTickCount64() - start > 3000) {
                    break;
                }
            }
            if (stop) {
                break;
            }
            sync = true;
        } else if (r == WAIT_TIMEOUT && !rootSwitched) {
            change = WatchRoot(root);
            sync = change != INVALID_HANDLE_VALUE;
        }

        std::vector<WorkItem> items;
        AcquireSRWLockExclusive(&g_workLock);
        items.swap(g_workQueue);
        sync |= g_syncRequested;
        g_syncRequested = false;
        ReleaseSRWLockExclusive(&g_workLock);

        for (const auto& item : items) {
            if (Stopping()) {
                break;
            }
            switch (item.kind) {
                case WorkKind::Add:
                    AddPaths(item.tag, item.paths);
                    break;
                case WorkKind::Remove:
                    RemovePaths(item.tag, item.paths);
                    break;
            }
        }
        // Work done while there was nothing to watch (a first tag on a clean
        // machine): arm the watch now and sync, otherwise the marker that
        // makes "delete the shortcut to untag" work wouldn't be written until
        // the next poll.
        if (!items.empty() && change == INVALID_HANDLE_VALUE) {
            change = WatchRoot(root);
            sync = sync || change != INVALID_HANDLE_VALUE;
        }
        if (sync && !Stopping()) {
            SyncAll();
        }
        if (sync || !items.empty()) {
            BroadcastRefresh();
        }
    }

    if (change != INVALID_HANDLE_VALUE) {
        FindCloseChangeNotification(change);
    }
    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }
    return 0;
}

void StartWorker() {
    ResetEvent(g_stopEvent);
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        // Without it there are no counts, no sync and no watcher.
        Wh_Log(L"Couldn't start the worker thread: %u", GetLastError());
    }
}

// No deadline: the DLL can't be unloaded while this thread is still running.
void StopWorker() {
    if (g_workerThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Helpers and state
// ---------------------------------------------------------------------------

std::vector<HWND> g_trackedFileViews;
SRWLOCK g_fileViewsLock = SRWLOCK_INIT;

UINT g_msgRefresh;
ULONG_PTR g_gdiplusToken;

std::atomic<int> g_busy;
std::atomic<int> g_hookCalls;

struct BusyScope {
    BusyScope() { g_busy++; }
    ~BusyScope() { g_busy--; }
};

struct HookCall {
    HookCall() { g_hookCalls++; }
    ~HookCall() { g_hookCalls--; }
};

struct DefViewPaintState {
    std::wstring folder;
    DWORD folderChecked = 0;
    bool capturing = false;
    bool detailsMode = false;
    int tagsOffset = -1;
    int tagsWidth = 0;
};
std::unordered_map<HWND, DefViewPaintState> g_paintStates;
SRWLOCK g_paintStatesLock = SRWLOCK_INIT;
thread_local HWND t_lastDefView = nullptr;

HWND FindDesktopListView();

void BroadcastRefresh() {
    std::vector<HWND> views;
    AcquireSRWLockExclusive(&g_fileViewsLock);
    std::erase_if(g_trackedFileViews, [](HWND h) { return !IsWindow(h); });
    views = g_trackedFileViews;
    ReleaseSRWLockExclusive(&g_fileViewsLock);

    AcquireSRWLockExclusive(&g_paintStatesLock);
    std::erase_if(g_paintStates, [](const auto& p) { return !IsWindow(p.first); });
    ReleaseSRWLockExclusive(&g_paintStatesLock);

    for (HWND h : views) {
        HWND target = GetParent(h) ? GetParent(h) : h;
        RedrawWindow(target, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }

    HWND desktop = FindDesktopListView();
    if (desktop && IsWindow(desktop)) {
        PostMessageW(desktop, g_msgRefresh, 0, 0);
    }
}

int Dip(int value, int dpi) {
    return MulDiv(value, dpi, 96);
}

void FillRounded(Gdiplus::Graphics& g, COLORREF color, int x, int y, int w, int h, int r) {
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    Gdiplus::GraphicsPath path;
    int d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d - 1, y, d, d, 270, 90);
    path.AddArc(x + w - d - 1, y + h - d - 1, d, d, 0, 90);
    path.AddArc(x, y + h - d - 1, d, d, 90, 90);
    path.CloseFigure();
    g.FillPath(&brush, &path);
}

bool HasClass(HWND hwnd, PCWSTR name) {
    WCHAR cls[64];
    return hwnd && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, name) == 0;
}

bool IsDesktopDefView(HWND defView) {
    if (!defView || !IsWindow(defView)) return false;
    HWND parent = GetParent(defView);
    return parent && (HasClass(parent, L"Progman") || HasClass(parent, L"WorkerW"));
}

// Add System.Keywords to a Details view without discarding the user's existing
// columns. Windows then handles text, sorting and grouping natively.
void EnsureTagsColumnForView(IUnknown* unknown, const Settings& settings) {
    if (!unknown || !settings.autoAddTagColumn) {
        return;
    }

    IFolderView2* folderView = nullptr;
    IColumnManager* columnManager = nullptr;
    FOLDERVIEWMODE mode = FVM_AUTO;
    int iconSize = 0;
    if (FAILED(unknown->QueryInterface(IID_PPV_ARGS(&folderView))) || !folderView ||
        FAILED(folderView->GetViewModeAndIconSize(&mode, &iconSize)) ||
        mode != FVM_DETAILS ||
        FAILED(unknown->QueryInterface(IID_PPV_ARGS(&columnManager))) || !columnManager) {
        if (folderView) {
            folderView->Release();
        }
        if (columnManager) {
            columnManager->Release();
        }
        return;
    }
    folderView->Release();

    UINT count = 0;
    if (FAILED(columnManager->GetColumnCount(CM_ENUM_VISIBLE, &count)) || count > 128) {
        columnManager->Release();
        return;
    }
    std::vector<PROPERTYKEY> columns(count);
    if (count && FAILED(columnManager->GetColumns(CM_ENUM_VISIBLE, columns.data(), count))) {
        columnManager->Release();
        return;
    }

    const PROPERTYKEY tagsKey = PKEY_Keywords;
    bool present = false;
    size_t insertAt = columns.size();
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i] == tagsKey) {
            present = true;
            break;
        }
        if (columns[i] == PKEY_ItemNameDisplay) {
            insertAt = i + 1;
        }
    }
    if (!present) {
        columns.insert(columns.begin() + std::min(insertAt, columns.size()), tagsKey);
        if (SUCCEEDED(columnManager->SetColumns(columns.data(), (UINT)columns.size()))) {
            UINT dpi = 96;
            IShellView* shellView = nullptr;
            HWND viewWindow = nullptr;
            if (SUCCEEDED(unknown->QueryInterface(IID_PPV_ARGS(&shellView))) && shellView) {
                if (SUCCEEDED(shellView->GetWindow(&viewWindow)) && viewWindow) {
                    dpi = GetDpiForWindow(viewWindow);
                }
                shellView->Release();
            }
            CM_COLUMNINFO info = {};
            info.cbSize = sizeof(info);
            info.dwMask = CM_MASK_WIDTH | CM_MASK_DEFAULTWIDTH;
            info.uWidth = MulDiv(std::max(80, settings.tagColumnWidth), dpi ? dpi : 96, 96);
            info.uDefaultWidth = info.uWidth;
            columnManager->SetColumnInfo(tagsKey, &info);
        }
    }
    columnManager->Release();
}

void SubclassDesktopListView(HWND hwnd);
void TrackFileView(HWND hwnd);
BOOL CALLBACK FindFileViewsProc(HWND hwnd, LPARAM lParam);

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HookCall call;
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth,
                                         nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && hWndParent && !g_unloading &&
        HasClass(hwnd, L"DirectUIHWND") && HasClass(hWndParent, L"SHELLDLL_DefView")) {
        TrackFileView(hwnd);
    }
    if (hwnd && hWndParent && !g_unloading &&
        HasClass(hwnd, L"SysListView32") && HasClass(hWndParent, L"SHELLDLL_DefView") &&
        IsDesktopDefView(hWndParent)) {
        SubclassDesktopListView(hwnd);
    }
    return hwnd;
}

BOOL CALLBACK FindExplorerWindowsProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && HasClass(hwnd, L"CabinetWClass")) {
        EnumChildWindows(hwnd, FindFileViewsProc, 0);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Right-click menu: "Tags" submenu with the selected files.
//
// With the "Classic context menu" mod, Explorer shows the classic menu with
// TrackPopupMenuEx(TPM_RETURNCMD): the entries are added before the menu
// opens, and if the chosen command is ours, it's handled here and 0 is
// returned (Explorer sees a canceled menu). The new Windows 11 menu (Ctrl)
// doesn't go through here.
// ---------------------------------------------------------------------------

// Owners of menus with our entries currently open (under g_menuOwnersLock), so
// they can be closed when unloading.
std::vector<HWND> g_menuOwners;
SRWLOCK g_menuOwnersLock = SRWLOCK_INIT;

const UINT MENU_ID_RANGE = 0x100;
const UINT MENU_ID_REMOVE_ALL = 0xFE;
const UINT MENU_ID_SEPARATOR_BEFORE = 0xFD;
const UINT MENU_ID_SEPARATOR_AFTER = 0xFC;
const UINT MENU_ID_SEPARATOR_INNER = 0xFB;
const UINT MENU_ID_NO_TAGS = 0xFA;


HWND DefViewFor(HWND owner) {
    if (!owner) {
        return nullptr;
    }
    for (HWND h = owner; h; h = GetParent(h)) {
        if (HasClass(h, L"SysHeader32")) {
            return nullptr;
        }
    }
    if (HasClass(owner, L"SHELLDLL_DefView")) {
        return owner;
    }
    HWND parent = GetParent(owner);
    if (parent && HasClass(parent, L"SHELLDLL_DefView") &&
        (HasClass(owner, L"DirectUIHWND") || HasClass(owner, L"SysListView32"))) {
        return parent;
    }
    return nullptr;
}

IShellBrowser* BrowserForDefView(HWND defView) {
    for (HWND h = GetParent(defView); h; h = GetParent(h)) {
        if (HasClass(h, L"ShellTabWindowClass")) {
            auto browser = (IShellBrowser*)SendMessageW(h, CWM_GETISHELLBROWSER, 0, 0);
            if (!browser) {
                browser = (IShellBrowser*)SendMessageW(GetAncestor(h, GA_ROOT), CWM_GETISHELLBROWSER, 0, 0);
            }
            return browser;
        }
    }
    return nullptr;
}

IFolderView2* FolderViewForDefView(HWND defView) {
    if (!defView) {
        return nullptr;
    }
    IShellBrowser* browser = BrowserForDefView(defView);
    if (browser) {
        IShellView* view = nullptr;
        if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
            HWND viewWnd = nullptr;
            view->GetWindow(&viewWnd);
            IFolderView2* fv = nullptr;
            if (viewWnd == defView) {
                view->QueryInterface(IID_PPV_ARGS(&fv));
            }
            view->Release();
            if (fv) {
                return fv;
            }
        }
    }

    IShellWindows* psw = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&psw))) && psw) {
        VARIANT vEmpty = {};
        vEmpty.vt = VT_EMPTY;
        long hwnd = 0;
        IDispatch* pDisp = nullptr;
        if (SUCCEEDED(psw->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, &pDisp)) && pDisp) {
            IServiceProvider* psp = nullptr;
            if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&psp)))) {
                IShellBrowser* psb = nullptr;
                if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&psb)))) {
                    IShellView* psv = nullptr;
                    if (SUCCEEDED(psb->QueryActiveShellView(&psv))) {
                        HWND viewWnd = nullptr;
                        psv->GetWindow(&viewWnd);
                        IFolderView2* fv = nullptr;
                        if (!defView || viewWnd == defView) {
                            psv->QueryInterface(IID_PPV_ARGS(&fv));
                        }
                        psv->Release();
                        psb->Release();
                        psp->Release();
                        pDisp->Release();
                        psw->Release();
                        return fv;
                    }
                    psb->Release();
                }
                psp->Release();
            }
            pDisp->Release();
        }
        psw->Release();
    }
    return nullptr;
}

std::vector<std::wstring> GetSelectedPaths(HWND defView) {
    std::vector<std::wstring> paths;
    IFolderView2* folderView = FolderViewForDefView(defView);
    if (folderView) {
        IShellItemArray* items = nullptr;
        if (SUCCEEDED(folderView->GetSelection(FALSE, &items)) && items) {
            DWORD count = 0;
            items->GetCount(&count);
            for (DWORD i = 0; i < count && i < 10000; i++) {
                IShellItem* item = nullptr;
                if (SUCCEEDED(items->GetItemAt(i, &item))) {
                    PWSTR path = nullptr;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
                        paths.push_back(path);
                        CoTaskMemFree(path);
                    }
                    item->Release();
                }
            }
            items->Release();
        }
        folderView->Release();
    }

    if (paths.empty() && defView) {
        HWND lv = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
        if (lv) {
            int sel = -1;
            WCHAR userDesk[MAX_PATH] = {};
            SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, userDesk);
            WCHAR pubDesk[MAX_PATH] = {};
            SHGetFolderPathW(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY, nullptr, 0, pubDesk);

            while ((sel = (int)SendMessageW(lv, LVM_GETNEXTITEM, sel, LVNI_SELECTED)) != -1) {
                WCHAR name[MAX_PATH] = {};
                LVITEMW lvi = {};
                lvi.mask = LVIF_TEXT;
                lvi.iItem = sel;
                lvi.pszText = name;
                lvi.cchTextMax = MAX_PATH;
                if (SendMessageW(lv, LVM_GETITEMTEXTW, sel, (LPARAM)&lvi) > 0 && name[0]) {
                    std::wstring fullPath = std::wstring(userDesk) + L"\\" + name;
                    if (GetFileAttributesW(fullPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                        paths.push_back(fullPath);
                    } else {
                        std::wstring pubPath = std::wstring(pubDesk) + L"\\" + name;
                        if (GetFileAttributesW(pubPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                            paths.push_back(pubPath);
                        }
                    }
                }
            }
        }
    }
    return paths;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// File view subclassing and hotkeys
// ---------------------------------------------------------------------------



std::vector<std::wstring> TagsForPaintPath(const std::wstring& folder, const std::wstring& name);
HWND g_desktopListView = nullptr;
std::vector<HWND> g_subclassedListViews;
std::vector<RECT> g_lastDesktopDotRects;
SRWLOCK g_listViewsLock = SRWLOCK_INIT;

void PaintDesktopDots(HWND hwnd) {
    if (!hwnd || g_unloading) {
        return;
    }
    int count = (int)SendMessageW(hwnd, LVM_GETITEMCOUNT, 0, 0);
    if (count <= 0) {
        return;
    }

    WCHAR userDesktop[MAX_PATH] = {};
    SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, userDesktop);
    WCHAR publicDesktop[MAX_PATH] = {};
    SHGetFolderPathW(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY, nullptr, 0, publicDesktop);

    AcquireSRWLockShared(&g_countsLock);
    bool desktopHasTags = (g_taggedFolders.count(LowerPath(userDesktop)) > 0) ||
                          (g_taggedFolders.count(LowerPath(publicDesktop)) > 0);
    ReleaseSRWLockShared(&g_countsLock);
    if (!desktopHasTags) {
        return;
    }

    auto settings = GetSettings();
    if (settings->tags.empty()) {
        return;
    }

    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        return;
    }

    int dpi = GetDpiForWindow(hwnd);
    if (!dpi) dpi = 96;
    int diameter = Dip(settings->desktopDotSize, dpi);
    int gap = Dip(3, dpi);

    HFONT hFont = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);

    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    std::vector<RECT> currentDotRects;
    for (int i = 0; i < count; i++) {
        WCHAR itemName[MAX_PATH] = {};
        LVITEMW lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.pszText = itemName;
        lvi.cchTextMax = MAX_PATH;
        if (SendMessageW(hwnd, LVM_GETITEMTEXTW, i, (LPARAM)&lvi) <= 0 || !itemName[0]) {
            continue;
        }

        std::vector<std::wstring> names = TagsForPaintPath(userDesktop, itemName);
        if (names.empty()) {
            names = TagsForPaintPath(publicDesktop, itemName);
        }
        if (names.empty()) {
            continue;
        }

        std::vector<TagDef> activeTags;
        for (const auto& tagName : names) {
            auto found = std::find_if(settings->tags.begin(), settings->tags.end(),
                                      [&](const TagDef& tag) {
                                          return _wcsicmp(tag.name.c_str(), tagName.c_str()) == 0;
                                      });
            if (found != settings->tags.end()) {
                activeTags.push_back(*found);
            }
        }
        if (activeTags.empty()) {
            continue;
        }

        RECT rcLabel = {};
        rcLabel.left = LVIR_LABEL;
        if (!SendMessageW(hwnd, LVM_GETITEMRECT, i, (LPARAM)&rcLabel)) {
            continue;
        }

        HGDIOBJ oldFont = SelectObject(hdc, hFont ? hFont : GetStockObject(DEFAULT_GUI_FONT));
        SIZE textSize = {};
        GetTextExtentPoint32W(hdc, itemName, lstrlenW(itemName), &textSize);
        SelectObject(hdc, oldFont);

        int labelWidth = rcLabel.right - rcLabel.left;
        int curDotX = 0;
        int dotY = 0;
        if (textSize.cx <= labelWidth) {
            int textStartX = rcLabel.left + (labelWidth - textSize.cx) / 2;
            curDotX = textStartX + textSize.cx + Dip(4, dpi);
            dotY = rcLabel.top + ((rcLabel.bottom - rcLabel.top) - diameter) / 2;
        } else {
            curDotX = rcLabel.right + Dip(2, dpi);
            dotY = rcLabel.bottom - diameter - Dip(2, dpi);
        }

        int dotStartX = curDotX;
        for (size_t t = 0; t < activeTags.size() && t < 3; t++) {
            COLORREF tc = activeTags[t].color;
            Gdiplus::SolidBrush dotBrush(
                Gdiplus::Color(255, GetRValue(tc), GetGValue(tc), GetBValue(tc)));
            g.FillEllipse(&dotBrush, (Gdiplus::REAL)curDotX, (Gdiplus::REAL)dotY,
                          (Gdiplus::REAL)diameter, (Gdiplus::REAL)diameter);
            curDotX += diameter + gap;
        }
        RECT itemDotRect = {dotStartX - 2, dotY - 2, curDotX + 2, dotY + diameter + 2};
        currentDotRects.push_back(itemDotRect);
    }

    ReleaseDC(hwnd, hdc);

    AcquireSRWLockExclusive(&g_listViewsLock);
    for (const auto& oldRc : g_lastDesktopDotRects) {
        bool stillPresent = false;
        for (const auto& curRc : currentDotRects) {
            if (EqualRect(&oldRc, &curRc)) {
                stillPresent = true;
                break;
            }
        }
        if (!stillPresent) {
            InvalidateRect(hwnd, &oldRc, TRUE);
        }
    }
    g_lastDesktopDotRects = std::move(currentDotRects);
    ReleaseSRWLockExclusive(&g_listViewsLock);
}

LRESULT CALLBACK DesktopListViewSubclassProc(HWND hwnd, UINT message, WPARAM wParam,
                                            LPARAM lParam, DWORD_PTR) {
    if (message == WM_MOUSEMOVE) {
        static int lastHot = -1;
        int hot = (int)SendMessageW(hwnd, LVM_GETHOTITEM, 0, 0);
        if (hot != lastHot) {
            lastHot = hot;
            AcquireSRWLockShared(&g_listViewsLock);
            for (const auto& rc : g_lastDesktopDotRects) {
                InvalidateRect(hwnd, &rc, TRUE);
            }
            ReleaseSRWLockShared(&g_listViewsLock);
        }
    }
    if (message == g_msgRefresh) {
        AcquireSRWLockShared(&g_listViewsLock);
        for (const auto& rc : g_lastDesktopDotRects) {
            InvalidateRect(hwnd, &rc, TRUE);
        }
        ReleaseSRWLockShared(&g_listViewsLock);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    if (message == WM_NCDESTROY) {
        AcquireSRWLockExclusive(&g_listViewsLock);
        g_subclassedListViews.erase(
            std::remove(g_subclassedListViews.begin(), g_subclassedListViews.end(), hwnd),
            g_subclassedListViews.end());
        ReleaseSRWLockExclusive(&g_listViewsLock);
        if (g_desktopListView == hwnd) {
            g_desktopListView = nullptr;
        }
        return DefSubclassProc(hwnd, message, wParam, lParam);
    }
    LRESULT lr = DefSubclassProc(hwnd, message, wParam, lParam);
    if (message == WM_PAINT) {
        PaintDesktopDots(hwnd);
    }
    return lr;
}

void SubclassDesktopListView(HWND hwnd) {
    if (!hwnd || g_unloading || !IsDesktopDefView(GetParent(hwnd))) {
        return;
    }
    AcquireSRWLockExclusive(&g_listViewsLock);
    bool already = std::find(g_subclassedListViews.begin(), g_subclassedListViews.end(), hwnd) !=
                   g_subclassedListViews.end();
    if (!already) {
        g_subclassedListViews.push_back(hwnd);
    }
    ReleaseSRWLockExclusive(&g_listViewsLock);
    if (already) {
        return;
    }
    g_desktopListView = hwnd;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, DesktopListViewSubclassProc, 0);
    InvalidateRect(hwnd, nullptr, FALSE);
}

HWND FindDesktopListView() {
    if (g_desktopListView && IsWindow(g_desktopListView)) {
        return g_desktopListView;
    }
    HWND defView = nullptr;
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    if (!defView) {
        HWND worker = nullptr;
        while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) != nullptr) {
            defView = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
            if (defView) break;
        }
    }
    if (defView) {
        return FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    }
    return nullptr;
}

void TrackFileView(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    AcquireSRWLockExclusive(&g_fileViewsLock);
    bool already = g_unloading ||
                   std::find(g_trackedFileViews.begin(), g_trackedFileViews.end(), hwnd) !=
                       g_trackedFileViews.end();
    if (!already) {
        g_trackedFileViews.push_back(hwnd);
    }
    ReleaseSRWLockExclusive(&g_fileViewsLock);
    if (!already) {
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

BOOL CALLBACK FindFileViewsProc(HWND hwnd, LPARAM) {
    if (HasClass(hwnd, L"DirectUIHWND") &&
        HasClass(GetParent(hwnd), L"SHELLDLL_DefView")) {
        TrackFileView(hwnd);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Row decoration and tag painting
// ---------------------------------------------------------------------------


HWND FolderViewAncestor(HWND hwnd) {
    for (int i = 0; hwnd && i < 24; i++, hwnd = GetParent(hwnd)) {
        if (HasClass(hwnd, L"SHELLDLL_DefView")) {
            return hwnd;
        }
    }
    return nullptr;
}

std::wstring FolderPathForDefView(HWND defView) {
    std::wstring result;
    IShellBrowser* browser = BrowserForDefView(defView);
    if (!browser) {
        return result;
    }
    IShellView* view = nullptr;
    if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
        HWND viewWindow = nullptr;
        view->GetWindow(&viewWindow);
        if (viewWindow == defView) {
            IFolderView* folderView = nullptr;
            if (SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&folderView)))) {
                IPersistFolder2* folder = nullptr;
                if (SUCCEEDED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    if (SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
                        WCHAR path[4096];
                        if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path),
                                                 GPFIDL_DEFAULT)) {
                            result = path;
                        }
                        CoTaskMemFree(pidl);
                    }
                    folder->Release();
                }
                folderView->Release();
            }
        }
        view->Release();
    }
    return result;
}

void CaptureTagsColumn(HWND defView, DefViewPaintState& state) {
    state.detailsMode = false;
    state.tagsOffset = -1;
    state.tagsWidth = 0;
    IShellBrowser* browser = BrowserForDefView(defView);
    if (!browser) {
        return;
    }
    IShellView* view = nullptr;
    if (FAILED(browser->QueryActiveShellView(&view)) || !view) {
        return;
    }
    HWND viewWindow = nullptr;
    view->GetWindow(&viewWindow);
    if (viewWindow != defView) {
        view->Release();
        return;
    }
    IFolderView2* folderView = nullptr;
    IColumnManager* manager = nullptr;
    FOLDERVIEWMODE mode = FVM_AUTO;
    int iconSize = 0;
    if (SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&folderView))) && folderView &&
        SUCCEEDED(folderView->GetViewModeAndIconSize(&mode, &iconSize)) &&
        mode == FVM_DETAILS &&
        SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&manager))) && manager) {
        state.detailsMode = true;
        UINT count = 0;
        if (SUCCEEDED(manager->GetColumnCount(CM_ENUM_VISIBLE, &count)) &&
            count && count <= 128) {
            std::vector<PROPERTYKEY> columns(count);
            if (SUCCEEDED(manager->GetColumns(CM_ENUM_VISIBLE, columns.data(), count))) {
                int offset = 0;
                for (const auto& column : columns) {
                    CM_COLUMNINFO info = {};
                    info.cbSize = sizeof(info);
                    info.dwMask = CM_MASK_WIDTH;
                    UINT width = 80;
                    if (SUCCEEDED(manager->GetColumnInfo(column, &info)) && info.uWidth) {
                        width = info.uWidth;
                    }
                    if (column == PKEY_Keywords) {
                        state.tagsOffset = offset;
                        state.tagsWidth = (int)width;
                        break;
                    }
                    offset += (int)width;
                }
            }
        }
    }
    if (manager) {
        manager->Release();
    }
    if (folderView) {
        folderView->Release();
    }
    view->Release();
}

LRESULT WINAPI DispatchMessageW_Hook(const MSG* message) {
    if (message) {
        if (message->message == WM_SYSKEYDOWN && !(message->lParam & (1 << 30))) {
            if ((GetKeyState(VK_CONTROL) & 0x8000) == 0 && (GetKeyState(VK_SHIFT) & 0x8000) == 0) {
                HWND focus = GetFocus();
                if (!focus || !HasClass(focus, L"Edit")) {
                    WPARAM key = message->wParam;
                    int tagIdx = -1;
                    if (key >= '1' && key <= '8') {
                        tagIdx = (int)(key - '1');
                    } else if (key == '0') {
                        tagIdx = 1000;
                    }

                    if (tagIdx >= 0) {
                        HWND defView = DefViewFor(message->hwnd);
                        if (defView) {
                            std::vector<std::wstring> paths = GetSelectedPaths(defView);
                            if (!paths.empty()) {
                                if (tagIdx == 1000) {
                                    QueueRemove(L"", std::move(paths));
                                    return 0;
                                } else {
                                    auto settings = GetSettings();
                                    if ((size_t)tagIdx < settings->tags.size()) {
                                        const std::wstring tag = settings->tags[tagIdx].name;
                                        TagHits hits = CountTagHits(paths);
                                        auto found = hits.perTag.find(tag);
                                        if (found != hits.perTag.end() && found->second == (int)paths.size()) {
                                            QueueRemove(tag, std::move(paths));
                                        } else {
                                            QueueAdd(tag, std::move(paths));
                                        }
                                        return 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (message->message == WM_PAINT || message->message == WM_ERASEBKGND) {
            HWND defView = FolderViewAncestor(message->hwnd);
            if (defView) {
                t_lastDefView = defView;
                DWORD now = GetTickCount();
                bool needCapture = false;
                AcquireSRWLockShared(&g_paintStatesLock);
                auto it = g_paintStates.find(defView);
                if (it == g_paintStates.end() || (now - it->second.folderChecked > 250 && !it->second.capturing)) {
                    needCapture = true;
                }
                ReleaseSRWLockShared(&g_paintStatesLock);

                if (needCapture) {
                    AcquireSRWLockExclusive(&g_paintStatesLock);
                    g_paintStates[defView].capturing = true;
                    ReleaseSRWLockExclusive(&g_paintStatesLock);

                    std::wstring folder = FolderPathForDefView(defView);
                    DefViewPaintState newState;
                    newState.folder = std::move(folder);
                    newState.folderChecked = now;
                    newState.capturing = false;
                    CaptureTagsColumn(defView, newState);

                    AcquireSRWLockExclusive(&g_paintStatesLock);
                    g_paintStates[defView] = std::move(newState);
                    ReleaseSRWLockExclusive(&g_paintStatesLock);
                }
            }
        }
    }
    return g_dispatchMessageOriginal(message);
}

std::wstring FileNameForElement(void* element) {
    if (!element) {
        return {};
    }
    if (g_duiFindDescendent && g_duiStrToID && g_duiGetContentString && g_duiReleaseValue) {
        if (!g_fileNameAtom) {
            g_fileNameAtom = g_duiStrToID(L"FileName");
        }
        if (g_fileNameAtom) {
            void* filename = g_duiFindDescendent(element, g_fileNameAtom);
            if (filename) {
                void* value = nullptr;
                PCWSTR text = g_duiGetContentString(filename, &value);
                std::wstring result = text ? text : L"";
                if (value) {
                    g_duiReleaseValue(value);
                }
                if (!result.empty()) {
                    return result;
                }
            }
        }
    }
    if (g_duiGetAccName && g_duiReleaseValue) {
        void* value = nullptr;
        PCWSTR text = g_duiGetAccName(element, &value);
        std::wstring result = text ? text : L"";
        if (value) {
            g_duiReleaseValue(value);
        }
        return result;
    }
    return {};
}

std::vector<std::wstring> TagsForPaintPath(const std::wstring& folder, const std::wstring& name) {
    std::vector<std::wstring> tags;
    if (folder.empty() || name.empty()) {
        return tags;
    }
    std::wstring lowerFolder = LowerPath(folder);
    std::wstring lowerName = LowerPath(name);

    AcquireSRWLockShared(&g_countsLock);
    // 1. If folder has no tagged items at all, return immediately (zero overhead)
    if (g_taggedFolders.count(lowerFolder) == 0) {
        ReleaseSRWLockShared(&g_countsLock);
        return tags;
    }

    // 2. Exact match by full path
    std::wstring fullPath = lowerFolder + L"\\" + lowerName;
    auto it = g_taggedByPath.find(fullPath);
    if (it != g_taggedByPath.end()) {
        tags = it->second;
        ReleaseSRWLockShared(&g_countsLock);
        return tags;
    }

    // 3. In-memory stem match (for hidden extensions)
    size_t dot = lowerName.find_last_of(L'.');
    std::wstring stem = (dot != std::wstring::npos) ? lowerName.substr(0, dot) : lowerName;
    auto stemIt = g_taggedByFolderAndStem.find(lowerFolder + L"\\" + stem);
    if (stemIt != g_taggedByFolderAndStem.end()) {
        tags = stemIt->second;
    }
    ReleaseSRWLockShared(&g_countsLock);
    return tags;
}

void PaintTagDots(void* element, HDC dc, const RECT& row) {
    if (!element || !dc || (row.right - row.left < 150) || (row.bottom - row.top < 14)) {
        return;
    }

    HWND hwnd = WindowFromDC(dc);
    HWND defView = hwnd ? FolderViewAncestor(hwnd) : nullptr;
    if (!defView) {
        defView = t_lastDefView;
    }
    if (!defView || !IsWindow(defView)) {
        return;
    }

    DefViewPaintState st;
    DWORD now = GetTickCount();
    bool needRefresh = false;

    AcquireSRWLockShared(&g_paintStatesLock);
    auto it = g_paintStates.find(defView);
    if (it != g_paintStates.end() && (now - it->second.folderChecked <= 500)) {
        st = it->second;
    } else {
        needRefresh = true;
    }
    ReleaseSRWLockShared(&g_paintStatesLock);

    if (needRefresh) {
        std::wstring folder = FolderPathForDefView(defView);
        st.folder = std::move(folder);
        st.folderChecked = now;
        st.capturing = false;
        CaptureTagsColumn(defView, st);

        AcquireSRWLockExclusive(&g_paintStatesLock);
        g_paintStates[defView] = st;
        ReleaseSRWLockExclusive(&g_paintStatesLock);
    }

    if (st.folder.empty()) {
        return;
    }

    std::wstring lowerFolder = LowerPath(st.folder);
    AcquireSRWLockShared(&g_countsLock);
    bool folderHasTags = g_taggedFolders.count(lowerFolder) > 0;
    ReleaseSRWLockShared(&g_countsLock);
    if (!folderHasTags) {
        return;
    }

    std::wstring name = FileNameForElement(element);
    if (name.empty()) {
        return;
    }

    std::vector<std::wstring> names = TagsForPaintPath(st.folder, name);
    if (names.empty()) {
        return;
    }

    auto settings = GetSettings();
    std::vector<TagDef> activeTags;
    for (const auto& tagName : names) {
        auto found = std::find_if(settings->tags.begin(), settings->tags.end(),
                                  [&](const TagDef& tag) {
                                      return _wcsicmp(tag.name.c_str(), tagName.c_str()) == 0;
                                  });
        if (found != settings->tags.end()) {
            activeTags.push_back(*found);
        }
    }
    if (activeTags.empty()) {
        return;
    }

    int dpi = defView ? GetDpiForWindow(defView) : 96;
    if (!dpi) {
        dpi = 96;
    }

    int diameter = Dip(settings->explorerDotSize, dpi);
    int gap = Dip(3, dpi);

    // Stable text measurement using system icon title font (Segoe UI 9pt)
    LOGFONTW lf = {};
    if (!SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0)) {
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
        lf.lfHeight = -MulDiv(9, dpi, 72);
        lf.lfWeight = FW_NORMAL;
    }
    HFONT itemFont = CreateFontIndirectW(&lf);
    HGDIOBJ oldFont = SelectObject(dc, itemFont);
    SIZE textSize = {};
    GetTextExtentPoint32W(dc, name.c_str(), (int)name.size(), &textSize);
    SelectObject(dc, oldFont);
    if (itemFont) {
        DeleteObject(itemFont);
    }

    // 1. Draw colored dots right after the file name
    void* filename = nullptr;
    if (g_fileNameAtom && g_duiFindDescendent) {
        filename = g_duiFindDescendent(element, g_fileNameAtom);
    }

    RECT rcFile = {};
    RECT rcItem = {};
    int textStartX = 0;
    if (filename && g_duiGetRootRelativeBounds &&
        SUCCEEDED(g_duiGetRootRelativeBounds(filename, &rcFile)) &&
        SUCCEEDED(g_duiGetRootRelativeBounds(element, &rcItem)) &&
        rcFile.right > rcFile.left) {
        int localX = rcFile.left - rcItem.left;
        if (localX > 0 && localX < Dip(120, dpi)) {
            textStartX = row.left + localX;
        }
    }
    if (textStartX == 0) {
        textStartX = row.left + Dip(38, dpi);
    }

    int dotY = row.top + ((row.bottom - row.top) - diameter) / 2;
    int curDotX = textStartX + textSize.cx + Dip(6, dpi);
    int maxDotX = (st.detailsMode && st.tagsOffset > 0)
                      ? (row.left + st.tagsOffset - Dip(6, dpi))
                      : (row.right - Dip(6, dpi));

    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    for (size_t i = 0; i < activeTags.size() && i < 3; i++) {
        if (curDotX + diameter > maxDotX) {
            break;
        }
        COLORREF tc = activeTags[i].color;
        Gdiplus::SolidBrush dotBrush(
            Gdiplus::Color(255, GetRValue(tc), GetGValue(tc), GetBValue(tc)));
        g.FillEllipse(&dotBrush, (Gdiplus::REAL)curDotX, (Gdiplus::REAL)dotY,
                      (Gdiplus::REAL)diameter, (Gdiplus::REAL)diameter);
        curDotX += diameter + gap;
    }

    // 2. In Details view, draw colored tag pills in the Tags column
    if (st.detailsMode && st.tagsOffset >= 0 && st.tagsWidth > 0) {
        int columnLeft = row.left + st.tagsOffset;
        int columnRight = columnLeft + st.tagsWidth;

        int rowH = row.bottom - row.top;
        int pillHeight = std::min(rowH - Dip(4, dpi), Dip(18, dpi));
        if (pillHeight < Dip(14, dpi)) {
            pillHeight = rowH;
        }
        int pillY = row.top + (rowH - pillHeight) / 2;
        int pillRadius = pillHeight / 2;
        int pillPadX = Dip(7, dpi);
        int pillGap = Dip(4, dpi);
        int curPillX = columnLeft + Dip(4, dpi);
        int rightLimit = columnRight - Dip(4, dpi);

        Gdiplus::FontFamily fontFamily(L"Segoe UI");
        Gdiplus::Font pillFont(&fontFamily, (Gdiplus::REAL)Dip(10, dpi),
                               Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

        size_t shown = 0;
        for (const auto& tag : activeTags) {
            Gdiplus::RectF boundRect;
            Gdiplus::RectF layoutRect(0, 0, 1000.0f, (Gdiplus::REAL)pillHeight);
            g.MeasureString(tag.name.c_str(), (INT)tag.name.size(), &pillFont, layoutRect, &format, &boundRect);
            int pillWidth = (int)ceil(boundRect.Width) + pillPadX * 2;
            if (curPillX + pillWidth > rightLimit) {
                break;
            }

            FillRounded(g, tag.color, curPillX, pillY, pillWidth, pillHeight, pillRadius);

            int lum = (GetRValue(tag.color) * 299 + GetGValue(tag.color) * 587 + GetBValue(tag.color) * 114) / 1000;
            Gdiplus::Color textColor = (lum > 155) ? Gdiplus::Color(255, 20, 20, 20) : Gdiplus::Color(255, 255, 255, 255);
            Gdiplus::SolidBrush textBrush(textColor);

            Gdiplus::RectF textRect((Gdiplus::REAL)curPillX, (Gdiplus::REAL)pillY, (Gdiplus::REAL)pillWidth, (Gdiplus::REAL)pillHeight);
            g.DrawString(tag.name.c_str(), (INT)tag.name.size(), &pillFont, textRect, &format, &textBrush);

            curPillX += pillWidth + pillGap;
            shown++;
        }

        if (shown < activeTags.size() && curPillX + Dip(20, dpi) <= rightLimit) {
            WCHAR more[16];
            wsprintfW(more, L"+%u", (UINT)(activeTags.size() - shown));
            Gdiplus::RectF boundRect;
            Gdiplus::RectF layoutRect(0, 0, 1000.0f, (Gdiplus::REAL)pillHeight);
            g.MeasureString(more, lstrlenW(more), &pillFont, layoutRect, &format, &boundRect);
            int moreWidth = (int)ceil(boundRect.Width) + Dip(8, dpi);
            if (curPillX + moreWidth <= rightLimit) {
                FillRounded(g, RGB(120, 120, 120), curPillX, pillY, moreWidth, pillHeight, pillRadius);
                Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
                Gdiplus::RectF moreRect((Gdiplus::REAL)curPillX, (Gdiplus::REAL)pillY, (Gdiplus::REAL)moreWidth, (Gdiplus::REAL)pillHeight);
                g.DrawString(more, lstrlenW(more), &pillFont, moreRect, &format, &whiteBrush);
            }
        }
    }
}

void PaintBackgroundHook(void* element, HDC dc, void* value,
                         const RECT* row, const RECT* a, const RECT* b,
                         const RECT* c) {
    HookCall call;
    g_paintBackgroundOriginal(element, dc, value, row, a, b, c);
    if (row && g_duiGetAccRole && g_duiGetAccRole(element) == ROLE_SYSTEM_LISTITEM) {
        PaintTagDots(element, dc, *row);
    }
}

bool InstallRowPainter() {
    g_dui = LoadLibraryExW(L"DUI70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dui) {
        Wh_Log(L"Dot painter disabled: DUI70.dll is not available");
        return false;
    }
    g_duiGetAccRole = (IntFn)GetProcAddress(
        g_dui, "?GetAccRole@Element@DirectUI@@QEAAHXZ");
    g_duiGetContentString = (ContentStringFn)GetProcAddress(
        g_dui, "?GetContentString@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z");
    g_duiGetAccName = (ContentStringFn)GetProcAddress(
        g_dui, "?GetAccName@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z");
    g_duiReleaseValue = (ReleaseValueFn)GetProcAddress(
        g_dui, "?Release@Value@DirectUI@@QEAAXXZ");
    g_duiStrToID = (StrToIDFn)GetProcAddress(g_dui, "StrToID");
    g_duiFindDescendent = (FindDescendentFn)GetProcAddress(
        g_dui, "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z");
    g_duiGetRootRelativeBounds = (GetRootRelativeBoundsFn)GetProcAddress(
        g_dui, "?GetRootRelativeBounds@Element@DirectUI@@QEAAJPEAUtagRECT@@@Z");
    auto paint = (PaintBackground_t*)GetProcAddress(
        g_dui, "?PaintBackground@Element@DirectUI@@QEAAXPEAUHDC__@@PEAVValue@2@"
               "AEBUtagRECT@@222@Z");

    if (!g_duiGetAccRole || !g_duiReleaseValue || !g_duiStrToID ||
        !g_duiFindDescendent || !paint ||
        !WindhawkUtils::SetFunctionHook(paint, PaintBackgroundHook,
                                        &g_paintBackgroundOriginal)) {
        Wh_Log(L"Dot painter disabled: DirectUI exports changed");
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return false;
    }
    Wh_Log(L"Dot painter initialized successfully");
    return true;
}

// Colored dot for the tag, with pre-multiplied alpha for the menu.
HBITMAP CreateDotBitmap(COLORREF color, int size) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        return bmp;
    }
    auto px = (BYTE*)bits;
    double center = size / 2.0;
    double radius = size * 0.3;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            double dx = x + 0.5 - center;
            double dy = y + 0.5 - center;
            double a = std::clamp(radius + 0.5 - std::sqrt(dx * dx + dy * dy), 0.0, 1.0);
            BYTE* p = px + (y * size + x) * 4;
            p[0] = (BYTE)(GetBValue(color) * a);
            p[1] = (BYTE)(GetGValue(color) * a);
            p[2] = (BYTE)(GetRValue(color) * a);
            p[3] = (BYTE)(255 * a);
        }
    }
    return bmp;
}

void CollectMenuIds(HMENU menu, std::vector<UINT>& ids, int depth) {
    if (depth >= 8) {
        return;
    }
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        if (GetMenuItemInfoW(menu, i, TRUE, &mii)) {
            ids.push_back(mii.wID);
            if (mii.hSubMenu) {
                CollectMenuIds(mii.hSubMenu, ids, depth + 1);
            }
        }
    }
}

// A block of IDs Explorer's menu doesn't use.
UINT FindFreeIdBase(HMENU menu) {
    std::vector<UINT> ids;
    CollectMenuIds(menu, ids, 0);
    for (UINT base = 0xE700; base + MENU_ID_RANGE <= 0xFF00; base += MENU_ID_RANGE) {
        bool used = false;
        for (UINT id : ids) {
            if (id >= base && id < base + MENU_ID_RANGE) {
                used = true;
                break;
            }
        }
        if (!used) {
            return base;
        }
    }
    return 0;
}

std::wstring EscapeMenuText(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        out += c;
        if (c == L'&') {
            out += L'&';
        }
    }
    return out;
}

struct TagMenu {
    HMENU parent = nullptr;
    HMENU sub = nullptr;
    UINT base = 0;
    std::vector<std::wstring> tags;
    std::vector<bool> allHave;  // checkmark: all selected items have the tag
    std::vector<HBITMAP> bitmaps;
};

void RemoveTagMenu(TagMenu& tm);

// InsertMenu ignores the ID for a separator, so removing it later by command
// would never find it. InsertMenuItem does keep it.
void InsertSeparator(HMENU menu, int pos, UINT id) {
    MENUITEMINFOW mii = {sizeof(mii)};
    mii.fMask = MIIM_ID | MIIM_FTYPE;
    mii.fType = MFT_SEPARATOR;
    mii.wID = id;
    InsertMenuItemW(menu, pos, TRUE, &mii);
}

bool AddTagMenu(TagMenu& tm, HMENU menu, HWND defView, const std::vector<std::wstring>& paths) {
    auto s = GetSettings();
    tm.base = FindFreeIdBase(menu);
    if (!tm.base) {
        return false;
    }
    tm.sub = CreatePopupMenu();
    if (!tm.sub) {
        return false;
    }
    tm.parent = menu;

    // No size cap: CountTagHits is one pass over the worker's copy with a
    // hash set, and the shortcut targets come from that same copy, so a big
    // selection costs no disk and no per-item work. A cap here meant a tag
    // could only ever be added for large selections, never removed.
    TagHits hits = CountTagHits(paths);

    int dpi = GetDpiForWindow(defView);
    int dotSize = GetSystemMetricsForDpi(SM_CXSMICON, dpi ? dpi : 96);
    int n = (int)s->tags.size();
    for (int i = 0; i < n; i++) {
        const TagDef& t = s->tags[i];
        tm.tags.push_back(t.name);
        auto hit = hits.perTag.find(t.name);
        bool allHave = hit != hits.perTag.end() && hit->second == (int)paths.size();
        tm.allHave.push_back(allHave);
        std::wstring text = EscapeMenuText(t.name);
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE | MIIM_BITMAP;
        mii.wID = tm.base + i;
        mii.dwTypeData = text.data();
        mii.fState = allHave ? MFS_CHECKED : 0;
        HBITMAP dot = CreateDotBitmap(t.color, dotSize);
        if (dot) {
            tm.bitmaps.push_back(dot);
        }
        mii.hbmpItem = dot;
        InsertMenuItemW(tm.sub, i, TRUE, &mii);
    }
    if (n == 0) {
        AppendMenuW(tm.sub, MF_STRING | MF_GRAYED, tm.base + MENU_ID_NO_TAGS,
                    L"No tags (create them in the mod settings)");
    }
    if (hits.withAnyTag > 0) {
        AppendMenuW(tm.sub, MF_SEPARATOR, tm.base + MENU_ID_SEPARATOR_INNER, nullptr);
        AppendMenuW(tm.sub, MF_STRING, tm.base + MENU_ID_REMOVE_ALL, L"Remove all tags");
    }

    // Before the last item ("Properties"), surrounded by separators.
    int count = GetMenuItemCount(menu);
    int pos = count >= 2 ? count - 1 : std::max(count, 0);
    MENUITEMINFOW prev = {sizeof(prev)};
    prev.fMask = MIIM_FTYPE;
    bool prevIsSeparator = pos > 0 && GetMenuItemInfoW(menu, pos - 1, TRUE, &prev) &&
                           (prev.fType & MFT_SEPARATOR);
    if (pos > 0 && !prevIsSeparator) {
        InsertSeparator(menu, pos++, tm.base + MENU_ID_SEPARATOR_BEFORE);
    }
    MENUITEMINFOW mii = {sizeof(mii)};
    mii.fMask = MIIM_SUBMENU | MIIM_STRING;
    mii.hSubMenu = tm.sub;
    WCHAR label[] = L"Tags";
    mii.dwTypeData = label;
    if (!InsertMenuItemW(menu, pos, TRUE, &mii)) {
        RemoveTagMenu(tm);
        return false;
    }
    if (pos + 1 < GetMenuItemCount(menu)) {
        InsertSeparator(menu, pos + 1, tm.base + MENU_ID_SEPARATOR_AFTER);
    }
    return true;
}

void RemoveTagMenu(TagMenu& tm) {
    if (tm.parent && IsMenu(tm.parent)) {
        int count = GetMenuItemCount(tm.parent);
        for (int i = 0; i < count; i++) {
            if (GetSubMenu(tm.parent, i) == tm.sub) {
                // DeleteMenu also destroys the submenu.
                DeleteMenu(tm.parent, i, MF_BYPOSITION);
                tm.sub = nullptr;
                break;
            }
        }
        DeleteMenu(tm.parent, tm.base + MENU_ID_SEPARATOR_BEFORE, MF_BYCOMMAND);
        DeleteMenu(tm.parent, tm.base + MENU_ID_SEPARATOR_AFTER, MF_BYCOMMAND);
    }
    if (tm.sub && IsMenu(tm.sub)) {
        DestroyMenu(tm.sub);
    }
    for (HBITMAP b : tm.bitmaps) {
        DeleteObject(b);
    }
    tm.bitmaps.clear();
}

void RunTagMenuCommand(const TagMenu& tm, UINT id, const std::vector<std::wstring>& paths) {
    if (id == tm.base + MENU_ID_REMOVE_ALL) {
        QueueRemove(L"", paths);
        return;
    }
    UINT index = id - tm.base;
    if (index >= tm.tags.size()) {
        return;
    }
    const std::wstring& tag = tm.tags[index];
    // Checked (all have it): removes it. Otherwise adds it to all of them
    // (the ones that already had it stay as they were).
    if (tm.allHave[index]) {
        QueueRemove(tag, paths);
    } else {
        QueueAdd(tag, paths);
    }
}

// Explorer menus that aren't the selected-files menu, and where "tag the
// selection" would touch the wrong files:
// - the right-click drag menu ("Copy here", ...). Measured in shell32.dll's
//   resources (menus 195-206): they all have "Cancel" with ID 0, which the
//   files menu doesn't have;
// - menus with no default entry (the files menu has "Open" in bold).
bool LooksLikeItemContextMenu(HMENU menu) {
    if (GetMenuDefaultItem(menu, FALSE, GMDI_USEDISABLED) == (UINT)-1) {
        return false;
    }
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask = MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU;
        if (GetMenuItemInfoW(menu, i, TRUE, &mii) && !(mii.fType & MFT_SEPARATOR) && !mii.hSubMenu &&
            mii.wID == 0) {
            return false;
        }
    }
    return true;
}

// TrackPopupMenu can internally call into TrackPopupMenuEx: only the outer
// one touches the menu.
thread_local int t_trackDepth;

struct TrackDepth {
    TrackDepth() { t_trackDepth++; }
    ~TrackDepth() { t_trackDepth--; }
};

bool IsColumnHeaderMenu(HMENU menu) {
    if (!menu) return false;
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask = MIIM_ID;
        if (GetMenuItemInfoW(menu, i, TRUE, &mii)) {
            // Standard Explorer column header commands (FCIDM_SHVIEW_COLSTART..COLEND)
            if (mii.wID >= 0x7000 && mii.wID <= 0x70FF) {
                return true;
            }
        }
    }
    return false;
}

template <typename Original>
BOOL TrackWithTagMenu(HMENU menu, UINT flags, HWND owner, int x, int y, Original original) {
    TrackDepth depth;
    // Without TPM_RETURNCMD the command would arrive as WM_COMMAND to the
    // view, which doesn't know it: in that case the menu isn't touched.
    HWND defView = (t_trackDepth == 1 && menu && (flags & TPM_RETURNCMD)) ? DefViewFor(owner) : nullptr;
    if (!defView) {
        return original();
    }

    // Never intercept the Details view column header context menu
    if (IsColumnHeaderMenu(menu)) {
        return original();
    }

    int itemCount = GetMenuItemCount(menu);
    bool looksRight = LooksLikeItemContextMenu(menu);
    Wh_Log(L"View menu: %d items, default %d, %s", itemCount,
           (int)GetMenuDefaultItem(menu, FALSE, GMDI_USEDISABLED),
           looksRight ? L"adding the tags" : L"not the files menu");
    if (!looksRight) {
        return original();
    }

    // Count first, then check g_unloading.
    BusyScope busy;
    if (g_unloading) {
        return original();
    }

    std::vector<std::wstring> paths = GetSelectedPaths(defView);
    // A shortcut from a tag folder counts as its file. This is a lookup in the
    // worker's copy: no .lnk is opened here, however many are selected.
    for (auto& path : paths) {
        path = TargetIfTagShortcutCached(path);
    }
    // Two shortcuts to the same file, or a file and its shortcut, are one
    // item here: otherwise the count never reaches the selection size and the
    // tag never shows as checked.
    std::unordered_set<std::wstring> seen;
    paths.erase(std::remove_if(paths.begin(), paths.end(),
                               [&seen](const std::wstring& path) {
                                   return !seen.insert(LowerPath(path)).second;
                               }),
                paths.end());
    TagMenu tm;
    if (paths.empty() || !AddTagMenu(tm, menu, defView, paths)) {
        return original();
    }

    AcquireSRWLockExclusive(&g_menuOwnersLock);
    g_menuOwners.push_back(owner);
    ReleaseSRWLockExclusive(&g_menuOwnersLock);

    BOOL result = original();

    AcquireSRWLockExclusive(&g_menuOwnersLock);
    auto it = std::find(g_menuOwners.begin(), g_menuOwners.end(), owner);
    if (it != g_menuOwners.end()) {
        g_menuOwners.erase(it);
    }
    ReleaseSRWLockExclusive(&g_menuOwnersLock);

    // The reserved block is chosen from the IDs present when the menu opens,
    // but shell submenus (Send to, Open with) fill themselves in later: check
    // the command really belongs to our submenu before swallowing it.
    //
    // This has to happen BEFORE RemoveTagMenu, which destroys the submenu:
    // asking afterwards always answered "not mine", and picking a tag did
    // nothing at all (measured on a real Explorer, 2026-09-18).
    UINT id = (UINT)result;
    MENUITEMINFOW ours = {sizeof(ours)};
    ours.fMask = MIIM_ID;
    bool isOurs = result && id >= tm.base && id < tm.base + MENU_ID_RANGE && tm.sub &&
                  IsMenu(tm.sub) && GetMenuItemInfoW(tm.sub, id, FALSE, &ours);

    RemoveTagMenu(tm);

    if (isOurs) {
        RunTagMenuCommand(tm, id, paths);
        return 0;
    }
    return result;
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm) {
    HookCall call;
    return TrackWithTagMenu(hMenu, uFlags, hwnd, x, y, [&]() {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hwnd, lptpm);
    });
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd,
                                const RECT* prcRect) {
    HookCall call;
    return TrackWithTagMenu(hMenu, uFlags, hWnd, x, y, [&]() {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    });
}

// ---------------------------------------------------------------------------
// Details-view column
// ---------------------------------------------------------------------------

using CDefViewUIActivate_t = HRESULT(__thiscall*)(void*, UINT);
using CDefViewSetCurrentViewMode_t = HRESULT(__thiscall*)(void*, UINT);
CDefViewUIActivate_t g_defViewUIActivateOriginal;
CDefViewSetCurrentViewMode_t g_defViewSetCurrentViewModeOriginal;

HRESULT __thiscall CDefViewUIActivate_Hook(void* pThis, UINT state) {
    HookCall call;
    HRESULT hr = g_defViewUIActivateOriginal(pThis, state);
    if (SUCCEEDED(hr) && state != 0) {
        EnsureTagsColumnForView((IUnknown*)pThis, *GetSettings());
    }
    return hr;
}

HRESULT __thiscall CDefViewSetCurrentViewMode_Hook(void* pThis, UINT viewMode) {
    HookCall call;
    HRESULT hr = g_defViewSetCurrentViewModeOriginal(pThis, viewMode);
    if (SUCCEEDED(hr) && viewMode == FVM_DETAILS) {
        EnsureTagsColumnForView((IUnknown*)pThis, *GetSettings());
    }
    return hr;
}

bool InstallDetailsColumnHooks() {
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        Wh_Log(L"Tags column disabled: shell32.dll is unavailable");
        return false;
    }
    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {
            {
                L"public: virtual long __cdecl CDefView::UIActivate(unsigned int)",
                L"long __cdecl CDefView::UIActivate(unsigned int)",
                L"public: virtual long __thiscall CDefView::UIActivate(unsigned int)",
                L"long __thiscall CDefView::UIActivate(unsigned int)",
            },
            &g_defViewUIActivateOriginal,
            CDefViewUIActivate_Hook,
        },
        {
            {
                L"public: virtual long __cdecl CDefView::SetCurrentViewMode(unsigned int)",
                L"long __cdecl CDefView::SetCurrentViewMode(unsigned int)",
                L"public: virtual long __thiscall CDefView::SetCurrentViewMode(unsigned int)",
                L"long __thiscall CDefView::SetCurrentViewMode(unsigned int)",
            },
            &g_defViewSetCurrentViewModeOriginal,
            CDefViewSetCurrentViewMode_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Tags column disabled: CDefView symbols changed");
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

void CloseHandles() {
    for (HANDLE* h : {&g_dbMutex, &g_stopEvent, &g_workEvent}) {
        if (*h) {
            CloseHandle(*h);
            *h = nullptr;
        }
    }
}

// Returning FALSE from Wh_ModInit means Wh_ModUninit never runs, and Windhawk
// retries after every settings change: the handles have to go back here.
BOOL InitFailed() {
    CloseHandles();
    return FALSE;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_unloading = false;
    g_workerThread = nullptr;
    g_busy = 0;
    g_hookCalls = 0;

    LoadSettings();

    std::wstring mutexName = L"WindhawkExplorerColorTagsDb-" + CurrentUserKey();
    g_dbMutex = CreateMutexW(nullptr, FALSE, (L"Global\\" + mutexName).c_str());
    if (!g_dbMutex) {
        g_dbMutex = CreateMutexW(nullptr, FALSE, (L"Local\\" + mutexName).c_str());
    }
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_dbMutex || !g_stopEvent || !g_workEvent) {
        return InitFailed();
    }

    g_msgRefresh = RegisterWindowMessageW(L"WhExplorerColorTags_Refresh");

    Gdiplus::GdiplusStartupInput gdiplusInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        g_gdiplusToken = 0;
        return InitFailed();
    }

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Couldn't hook CreateWindowExW");
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return InitFailed();
    }

    WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageW_Hook,
                                   &g_dispatchMessageOriginal);
    InstallDetailsColumnHooks();
    InstallRowPainter();
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook,
                                        &TrackPopupMenuEx_Original) ||
        !WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook,
                                        &TrackPopupMenu_Original)) {
        Wh_Log(L"Couldn't hook the menu functions: no Tags submenu");
    }

    StartWorker();
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(FindExplorerWindowsProc, 0);
    HWND desktop = FindDesktopListView();
    if (desktop) {
        SubclassDesktopListView(desktop);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(FindExplorerWindowsProc, 0);
    QueueSync();
    BroadcastRefresh();
}

const DWORD UNLOAD_WAIT_MS = 2000;

bool WaitForZero(const std::atomic<int>& counter, PCWSTR what) {
    ULONGLONG start = GetTickCount64();
    while (counter > 0) {
        if (GetTickCount64() - start > UNLOAD_WAIT_MS) {
            Wh_Log(L"Gave up waiting for %s (%d left); unloading anyway", what, counter.load());
            return false;
        }
        Sleep(20);
    }
    return true;
}

void Wh_ModBeforeUninit() {
    g_unloading = true;

    AcquireSRWLockShared(&g_menuOwnersLock);
    for (HWND owner : g_menuOwners) {
        if (IsWindow(owner)) {
            PostMessageW(owner, WM_CANCELMODE, 0, 0);
        }
    }
    ReleaseSRWLockShared(&g_menuOwnersLock);

    std::vector<HWND> fileViews;
    {
        AcquireSRWLockExclusive(&g_fileViewsLock);
        fileViews.swap(g_trackedFileViews);
        ReleaseSRWLockExclusive(&g_fileViewsLock);
    }
    for (HWND fileView : fileViews) {
        if (IsWindow(fileView)) {
            InvalidateRect(fileView, nullptr, FALSE);
        }
    }
    std::vector<HWND> listViews;
    {
        AcquireSRWLockExclusive(&g_listViewsLock);
        listViews.swap(g_subclassedListViews);
        ReleaseSRWLockExclusive(&g_listViewsLock);
    }
    for (HWND lv : listViews) {
        if (IsWindow(lv)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                lv, DesktopListViewSubclassProc);
            InvalidateRect(lv, nullptr, FALSE);
        }
    }
    if (g_desktopListView && IsWindow(g_desktopListView)) {
        AcquireSRWLockExclusive(&g_listViewsLock);
        for (const auto& rc : g_lastDesktopDotRects) {
            InvalidateRect(g_desktopListView, &rc, TRUE);
        }
        g_lastDesktopDotRects.clear();
        ReleaseSRWLockExclusive(&g_listViewsLock);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_desktopListView, DesktopListViewSubclassProc);
        InvalidateRect(g_desktopListView, nullptr, FALSE);
        g_desktopListView = nullptr;
    }

    WaitForZero(g_busy, L"mod code on a window thread");

    StopWorker();
}

void Wh_ModUninit() {
    WaitForZero(g_hookCalls, L"calls inside the hooks");
    AcquireSRWLockExclusive(&g_paintStatesLock);
    g_paintStates.clear();
    ReleaseSRWLockExclusive(&g_paintStatesLock);
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    if (g_dui) {
        FreeLibrary(g_dui);
        g_dui = nullptr;
    }
    CloseHandles();
    Wh_Log(L"<");
}
