// ==WindhawkMod==
// @id              explorer-tags
// @name            Explorer Tags
// @description     Colored tags panel at the bottom of File Explorer's navigation pane, plus a Tags submenu in the file context menu
// @version         0.5.1
// @author          buedgik
// @github          https://github.com/buedgik
// @homepage        https://github.com/buedgik/explorer-tags
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lshell32 -lshlwapi -lcomctl32 -lgdi32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Tags

Adds a **Tags** panel at the bottom of File Explorer's navigation pane, below
"This PC" and "Network", and a **Tags ▸** submenu to the file context menu.

Windows' own tags only work for file types with a property handler, so `.txt`,
`.zip`, `.rar` and folders can't be tagged. These work for anything.

![The Tags panel at the bottom of the navigation pane](https://raw.githubusercontent.com/buedgik/explorer-tags/main/docs/panel.png)

![The Tags submenu in the file context menu](https://raw.githubusercontent.com/buedgik/explorer-tags/main/docs/menu.png)

- **Tag something:** drag files or folders onto the tag.
- **See a tag's files:** click it. The tab opens the tag's folder, with a
  shortcut for each file.
- **Remove a tag:** delete the shortcut inside the tag's folder. The original
  file is untouched.
- **Collapse the panel:** click the "Tags" title.
- **Right-click menu:** on selected files, a **Tags ▸** submenu appears, with
  one entry per tag. Checked means all selected items have it; picking it
  removes it from all of them. Unchecked adds it to all of them. "Remove all tags" appears
  when at least one already has tags. Works in the classic menu (for example
  with the "Classic context menu" mod); the new Windows 11 menu isn't
  changed.

Tags are defined in the mod settings (name and color).

## How files are tracked

Each tagged file is recorded by its NTFS ID. If you rename it or move it
within the same drive, the tag follows it and the shortcut is fixed. If it's
overwritten by an editor (which changes the ID), it's recognized by path.
Copied to another drive, the copy doesn't carry the tag.

If you rename a tag in the settings, the old tag stops appearing; putting the
old name back brings it back.

The tag registry lives in `%LOCALAPPDATA%` in the `WindhawkExplorerTags`
folder, outside the tags folder: deleting or moving the tags folder doesn't
lose them, the shortcuts are recreated.

## Worth knowing

- **Don't put the tags folder in a synced location** (OneDrive, Dropbox, a
  network share). A shortcut that is missing while the folder syncs looks
  exactly like a shortcut you deleted, and the tag goes with it. For the same
  reason, keep it somewhere quiet: the whole folder tree is watched, so a busy
  folder means constant rechecking.
- **Deleting a shortcut removes the tag; renaming or moving one isn't
  tracked.** A renamed shortcut is left behind as a file the mod no longer
  knows about, and moving a shortcut from one tag's folder into another's
  removes the first tag without adding the second.
- **The tags folder and the record stay after the mod is disabled**, and so do
  your shortcuts. A tag's folder appears the first time you tag something with
  it, or the first time you click it in the panel; the record folder appears
  with the first tag.
- **Changing the tags folder in the settings leaves the old one behind.** The
  shortcuts are rebuilt under the new folder; the old folders aren't deleted,
  in case something else lives there.
- **The desktop and file dialogs are left alone.** The panel and the submenu
  are for Explorer windows; right-clicking a file on the desktop shows the
  usual menu, without Tags.
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
  $name: Tags
  $description: Each tag has a name and a color.
- folder: ""
  $name: Tags folder
  $description: >-
    Where the shortcut folders go, one per tag. Empty uses %USERPROFILE%\Tags.

    Changing this rebuilds the shortcuts under the new folder and leaves the
    old one behind. Avoid synced folders (OneDrive, Dropbox, network shares):
    a shortcut missing mid-sync looks like a tag you removed. The whole tree
    under this folder is watched, so keep it out of busy folders.
- maxRows: 8
  $name: Visible rows
  $description: With more tags than this, the panel scrolls with the mouse wheel
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <windhawk_utils.h>

extern IMAGE_DOS_HEADER __ImageBase;
#define THIS_MODULE ((HINSTANCE)&__ImageBase)

#define PANEL_CLASS L"WhExplorerTagsPanel"
#define CWM_GETISHELLBROWSER (WM_USER + 7)
#define TIMER_REPAINT 1

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
    int maxRows = 8;
};

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

std::wstring DbPath();

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
        root = L"%USERPROFILE%\\Tags";
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
        std::wstring dbFolder = DbPath();
        size_t slash = dbFolder.rfind(L'\\');
        dbFolder = slash == std::wstring::npos ? L"" : dbFolder.substr(0, slash);
        std::wstring prefix = root + L"\\";
        bool holdsOwnRecord = !dbFolder.empty() && dbFolder.size() > prefix.size() &&
                              _wcsnicmp(dbFolder.c_str(), prefix.c_str(), prefix.size()) == 0;
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
        n = ExpandEnvironmentStringsW(L"%USERPROFILE%\\Tags", expanded, ARRAYSIZE(expanded));
        root = (n > 0 && n <= ARRAYSIZE(expanded)) ? expanded : L"C:\\Tags";
    }
    s.root = root;

    s.maxRows = Wh_GetIntSetting(L"maxRows");
    if (s.maxRows <= 0) {
        s.maxRows = 8;
    }
    s.maxRows = std::min(s.maxRows, 40);

    auto loaded = std::make_shared<const Settings>(std::move(s));
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = loaded;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ---------------------------------------------------------------------------
// Database: one row per (file, tag), in
// %LOCALAPPDATA%\WindhawkExplorerTags\tags.tsv. It belongs to the user and
// stays OUTSIDE the tags folder: inside it, it would get lost when moving the
// folder (the sync could run mid-copy, with the database already copied but
// not the shortcuts) or when deleting it. Outside, a deleted or new folder
// gets rebuilt from it. It's written to a new file that only then replaces
// the old one. A named mutex serializes all explorer.exe processes in the
// session.
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

std::wstring DbPath() {
    WCHAR base[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\WindhawkExplorerTags", base, ARRAYSIZE(base));
    if (n == 0 || n > ARRAYSIZE(base)) {
        return L"";
    }
    // Not created here: reading a path shouldn't create a folder. The two
    // functions that write call EnsureParentFolder first.
    return std::wstring(base) + L"\\tags.tsv";
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
            return IsInRecycleBin(currentPath) ? Resolved::Gone : Resolved::Found;
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

// The only function that deletes: only a .lnk directly inside the tag's folder.
void DeleteLinkIn(const std::wstring& folder, const std::wstring& name) {
    if (IsSafeLinkName(name)) {
        std::wstring lnkPath = folder + L"\\" + name;
        if (DeleteFileW(lnkPath.c_str())) {
            NotifyShell(SHCNE_DELETE, lnkPath);
        }
    }
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

// What's only done after the database has been saved.
struct Pending {
    std::vector<PendingDelete> deletes;
    std::vector<std::wstring> markers;
};

// The shortcuts to delete are only deleted after the database has been
// saved: if saving fails, the database still points to shortcuts that exist.
void SyncTag(std::vector<Row>& rows, const std::wstring& tag, const std::wstring& folder,
             bool ignoreMarkers, VolumeHandles& volumes, bool& changed, Pending& pending) {
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

    for (size_t i = 0; i < rows.size();) {
        if (Stopping()) {
            return;
        }
        if (rows[i].tag != tag) {
            i++;
            continue;
        }
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

    // The marker only goes down after saving: if saving fails, the database
    // still has the shortcuts' old names, and a marked folder would remove
    // those tags.
    if (!trusted && complete) {
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

// Copy of what the worker thread last read, so the window thread (panel
// counts, checkmarks in the menu) never touches the disk or the mutex.
std::vector<std::pair<std::wstring, int>> g_counts;
std::vector<std::pair<std::wstring, std::wstring>> g_tagged;      // (tag, path)
std::unordered_map<std::wstring, std::wstring> g_linkTargets;     // lnk -> target
std::unordered_set<std::wstring> g_readyFolders;  // tag folders known to exist
SRWLOCK g_countsLock = SRWLOCK_INIT;

std::wstring LowerPath(const std::wstring& path) {
    std::wstring lower = path;
    if (!lower.empty()) {
        CharLowerBuffW(lower.data(), (DWORD)lower.size());
    }
    return lower;
}

void UpdateCounts(const std::vector<Row>& rows, const Settings& s) {
    std::vector<std::pair<std::wstring, int>> counts;
    for (const auto& t : s.tags) {
        int n = 0;
        for (const auto& r : rows) {
            if (r.tag == t.name) {
                n++;
            }
        }
        counts.push_back({t.name, n});
    }
    std::vector<std::pair<std::wstring, std::wstring>> tagged;
    tagged.reserve(rows.size());
    std::unordered_map<std::wstring, std::wstring> linkTargets;
    linkTargets.reserve(rows.size());
    for (const auto& r : rows) {
        tagged.push_back({r.tag, r.path});
        // The worker already knows where every shortcut it wrote points, so
        // the window thread never has to open a .lnk to find out.
        linkTargets[LowerPath(s.root + L"\\" + FolderNameFor(r.tag) + L"\\" + r.lnk)] = r.path;
    }
    // Which tag folders exist, so the window thread can decide whether it can
    // navigate to one without asking the disk itself.
    std::unordered_set<std::wstring> ready;
    for (const auto& t : s.tags) {
        std::wstring folder = s.root + L"\\" + t.folderName;
        if (CheckExists(folder) == Exists::Yes) {
            ready.insert(LowerPath(folder));
        }
    }

    AcquireSRWLockExclusive(&g_countsLock);
    g_counts.swap(counts);
    g_tagged.swap(tagged);
    g_linkTargets.swap(linkTargets);
    g_readyFolders.swap(ready);
    ReleaseSRWLockExclusive(&g_countsLock);
}

bool FolderIsReady(const std::wstring& folder) {
    AcquireSRWLockShared(&g_countsLock);
    bool ready = g_readyFolders.count(LowerPath(folder)) > 0;
    ReleaseSRWLockShared(&g_countsLock);
    return ready;
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
    std::unordered_set<std::wstring> selected;
    for (const auto& p : paths) {
        selected.insert(LowerPath(p));
    }
    std::unordered_set<std::wstring> seenPerTag;
    std::unordered_set<std::wstring> seenAny;
    AcquireSRWLockShared(&g_countsLock);
    for (const auto& t : g_tagged) {
        std::wstring lower = LowerPath(t.second);
        if (!selected.count(lower)) {
            continue;
        }
        if (seenPerTag.insert(t.first + L"\t" + lower).second) {
            hits.perTag[t.first]++;
        }
        if (seenAny.insert(lower).second) {
            hits.withAnyTag++;
        }
    }
    ReleaseSRWLockShared(&g_countsLock);
    return hits;
}

int GetCount(const std::wstring& tag) {
    int n = 0;
    AcquireSRWLockShared(&g_countsLock);
    for (const auto& c : g_counts) {
        if (c.first == tag) {
            n = c.second;
        }
    }
    ReleaseSRWLockShared(&g_countsLock);
    return n;
}

// Only a full SyncAll saves the root; until then, any sync ignores the
// markers (rebuilds instead of removing tags). It lives next to the
// database, per user: Windhawk's values are machine-wide, and two accounts
// would overwrite each other's.
std::wstring LastRootPath() {
    std::wstring db = DbPath();
    return db.empty() ? L"" : db.substr(0, db.rfind(L'\\')) + L"\\last-root.txt";
}

bool RootChangedSinceLastSync(const Settings& s) {
    std::wstring file = LastRootPath();
    HANDLE h = file.empty() ? INVALID_HANDLE_VALUE
                            : CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                          OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return true;
    }
    char buf[4096];
    DWORD read = 0;
    bool ok = ReadFile(h, buf, sizeof(buf), &read, nullptr);
    CloseHandle(h);
    return !ok || FromUtf8(std::string(buf, read)) != s.root;
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
    bool rootChanged = RootChangedSinceLastSync(*s);
    bool stopped = false;
    for (const auto& t : s->tags) {
        if (Stopping()) {
            stopped = true;
            break;
        }
        SyncTag(rows, t.name, s->root + L"\\" + t.folderName, rootChanged, volumes, changed, pending);
    }
    stopped |= Stopping();
    Commit(dbPath, rows, changed, pending);
    // Only worth remembering next to a database that exists: otherwise
    // merely enabling the mod would create the folder, which the readme says
    // it doesn't. Until then every sync rebuilds instead of trusting markers,
    // which is the safe direction.
    if (rootChanged && !stopped && !rows.empty()) {
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
    SyncTag(rows, tag, folder, RootChangedSinceLastSync(*s), volumes, changed, pending);
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

// Worker: make a tag's folder exist so a window can navigate to it. No marker
// is written, so the next sync rebuilds the shortcuts into it instead of
// reading an empty folder as "the user removed these tags".
void EnsureTagFolder(const std::wstring& tag) {
    auto s = GetSettings();
    for (const auto& t : s->tags) {
        if (t.name != tag) {
            continue;
        }
        std::wstring folder = s->root + L"\\" + t.folderName;
        if (CheckExists(folder) == Exists::No &&
            SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr) == ERROR_SUCCESS) {
            NotifyShell(SHCNE_MKDIR, folder);
        }
        bool exists = CheckExists(folder) == Exists::Yes;
        AcquireSRWLockExclusive(&g_countsLock);
        if (exists) {
            g_readyFolders.insert(LowerPath(folder));
        } else {
            // Stops the panel from trying to open a folder that isn't there.
            g_readyFolders.erase(LowerPath(folder));
        }
        ReleaseSRWLockExclusive(&g_countsLock);
        return;
    }
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

    bool changed = false;
    Pending pending;
    for (const std::wstring& original : paths) {
        if (Stopping()) {
            break;
        }
        std::wstring path = TargetIfTagShortcut(original, s->root);
        std::wstring vol, id;
        GetFileIdentity(path, vol, id);
        for (size_t i = 0; i < rows.size();) {
            const Row& r = rows[i];
            bool sameFile = (!id.empty() && r.volume == vol && r.fileId == id) ||
                            _wcsicmp(r.path.c_str(), path.c_str()) == 0;
            if (sameFile && (tag.empty() || r.tag == tag)) {
                // The shortcut goes away after saving; the watcher sees it
                // disappear and no longer finds the row, so there's nothing
                // to undo.
                pending.deletes.push_back({s->root + L"\\" + FolderNameFor(r.tag), r.lnk});
                rows.erase(rows.begin() + i);
                changed = true;
                continue;
            }
            i++;
        }
    }
    Commit(dbPath, rows, changed, pending);
    UpdateCounts(rows, *s);
}

// ---------------------------------------------------------------------------
// Worker thread: watches the tags folder and does all the disk work.
// ---------------------------------------------------------------------------

enum class WorkKind { Add, Remove, EnsureFolder };

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
    AcquireSRWLockExclusive(&g_workLock);
    g_workQueue.push_back(WorkItem{tag, std::move(paths), WorkKind::Add});
    ReleaseSRWLockExclusive(&g_workLock);
    SetEvent(g_workEvent);
}

void QueueRemove(const std::wstring& tag, std::vector<std::wstring> paths) {
    if (g_unloading) {
        return;
    }
    AcquireSRWLockExclusive(&g_workLock);
    g_workQueue.push_back(WorkItem{tag, std::move(paths), WorkKind::Remove});
    ReleaseSRWLockExclusive(&g_workLock);
    SetEvent(g_workEvent);
}

// Clicking a tag whose folder doesn't exist yet: the folder is created here,
// on the worker, and the panel navigates when it learns the folder is ready.
void QueueEnsureFolder(const std::wstring& tag) {
    if (g_unloading) {
        return;
    }
    AcquireSRWLockExclusive(&g_workLock);
    g_workQueue.push_back(WorkItem{tag, {}, WorkKind::EnsureFolder});
    ReleaseSRWLockExclusive(&g_workLock);
    SetEvent(g_workEvent);
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
    while (true) {
        DWORD r = MsgWaitForMultipleObjects(count, handles, FALSE, timeout, QS_ALLINPUT);
        if (r != WAIT_OBJECT_0 + count) {
            return r;
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
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
                    // Folder deleted: release the handle (which would keep
                    // it mid-deletion) and try again in 5 s.
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
                case WorkKind::EnsureFolder:
                    EnsureTagFolder(item.tag);
                    break;
            }
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
// Panel
// ---------------------------------------------------------------------------

class TagDropTarget;

struct Panel {
    HWND tree;       // NamespaceTreeControl
    HWND sink;       // CtrlNotifySink, parent of the tree and the panel
    HWND shellTab;   // ShellTabWindowClass
    HWND wnd = nullptr;
    TagDropTarget* drop = nullptr;
    int panelHeight = 0;
    std::wstring currentFolder;  // folder shown in the tab, for the open tag row
    std::wstring pendingOpen;    // tag folder to open once the worker made it
    bool pendingOpenNewWindow = false;
    int hot = -1;
    int pressed = -1;
    int dropHot = -1;
    int scroll = 0;
    int wheelDelta = 0;
    int rowHeight = 0;    // from the tree, cached: see GetMetrics
    int rowHeightDpi = 0;
    bool trackingLeave = false;
    bool inLayout = false;
    HFONT iconFont = nullptr;
    int iconFontDpi = 0;
};

const int HIT_NONE = -1;
const int HIT_HEADER = -2;

std::vector<Panel*> g_panels;
std::vector<HWND> g_subclassedTrees;
SRWLOCK g_panelsLock = SRWLOCK_INIT;

UINT g_msgAttach;
UINT g_msgDetach;
UINT g_msgRefresh;
UINT g_msgLayout;
UINT g_msgCancelMenu;

std::atomic<bool> g_collapsed;
ULONG_PTR g_gdiplusToken;

// Always looked up again: a stored HWND could die and be reused by a window
// on another thread, and SendMessage to it would hang.
HWND TreeViewOf(Panel* p) {
    return FindWindowExW(p->tree, nullptr, L"SysTreeView32", nullptr);
}

// Mod code on a window thread's stack (inside an IDropTarget method, a menu,
// a navigation). Unloading waits, with no deadline, for it to reach zero.
std::atomic<int> g_busy;

// Drags in progress over a panel: between DragEnter and DragLeave/Drop, OLE
// keeps the object and calls it again, so the module can't be unloaded while
// the count is above zero. Unloading releases the mod's own reference first,
// so a drag whose source died can still end when OLE lets go.
std::atomic<int> g_drags;

struct BusyScope {
    BusyScope() { g_busy++; }
    ~BusyScope() { g_busy--; }
};

// Calls in progress inside the hooks. Windhawk removes the hooks before
// Wh_ModUninit, but a call that already entered (an open menu) still returns
// to the hook code: Wh_ModUninit waits for them.
std::atomic<int> g_hookCalls;

struct HookCall {
    HookCall() { g_hookCalls++; }
    ~HookCall() { g_hookCalls--; }
};

Panel* FindPanel(HWND hwnd) {
    Panel* found = nullptr;
    AcquireSRWLockShared(&g_panelsLock);
    for (Panel* p : g_panels) {
        if (p->tree == hwnd || p->sink == hwnd || p->wnd == hwnd) {
            found = p;
            break;
        }
    }
    ReleaseSRWLockShared(&g_panelsLock);
    return found;
}

void BroadcastMessage(UINT msg) {
    AcquireSRWLockShared(&g_panelsLock);
    for (Panel* p : g_panels) {
        if (p->wnd) {
            PostMessageW(p->wnd, msg, 0, 0);
        }
    }
    ReleaseSRWLockShared(&g_panelsLock);
}

void BroadcastRefresh() {
    BroadcastMessage(g_msgRefresh);
}

int Dip(int value, int dpi) {
    return MulDiv(value, dpi, 96);
}

struct Metrics {
    int dpi;
    int rowH;
    int topPad;
    int sepY;
    int bottomPad;
    int tagCount;
    int rowsShown;  // rows that fit in the actual height
};

Metrics GetMetrics(Panel* p, const Settings& s, int height) {
    Metrics m;
    m.dpi = GetDpiForWindow(p->wnd ? p->wnd : p->sink);
    if (!m.dpi) {
        m.dpi = 96;
    }
    // Asking the tree costs a FindWindowEx plus a SendMessage, and this runs
    // on every mouse move; it only changes with the theme or the DPI.
    if (p->rowHeight <= 0 || p->rowHeightDpi != m.dpi) {
        int itemH = TreeView_GetItemHeight(TreeViewOf(p));
        p->rowHeight = itemH > 0 ? itemH : Dip(32, m.dpi);
        p->rowHeightDpi = m.dpi;
    }
    m.rowH = p->rowHeight;
    m.topPad = Dip(12, m.dpi);
    m.sepY = Dip(6, m.dpi);
    m.bottomPad = Dip(6, m.dpi);
    m.tagCount = (int)s.tags.size();
    int wanted = g_collapsed ? 0 : std::min(std::max(m.tagCount, 1), s.maxRows);
    if (height < 0) {
        m.rowsShown = wanted;
    } else {
        int room = (height - m.topPad - m.rowH - m.bottomPad) / m.rowH;
        m.rowsShown = std::max(0, std::min(wanted, room));
    }
    return m;
}

int WantedHeight(Panel* p, const Settings& s) {
    Metrics m = GetMetrics(p, s, -1);
    return m.topPad + m.rowH + m.rowsShown * m.rowH + (g_collapsed ? 0 : m.bottomPad);
}

void ClampScroll(Panel* p, const Metrics& m) {
    int maxScroll = std::max(0, m.tagCount - m.rowsShown);
    p->scroll = std::clamp(p->scroll, 0, maxScroll);
}

void LayoutPanel(Panel* p) {
    if (p->inLayout || !p->wnd || !IsWindow(p->sink) || !IsWindow(p->tree)) {
        return;
    }
    // Touching the tree can make DirectUI touch the container, which calls
    // this again.
    p->inLayout = true;
    auto s = GetSettings();
    RECT rc;
    GetClientRect(p->sink, &rc);
    int width = rc.right;
    int height = rc.bottom;

    Metrics m = GetMetrics(p, *s, -1);
    int wanted = WantedHeight(p, *s);
    // The tree always keeps at least two rows.
    int ph = std::max(0, std::min(wanted, height - m.rowH * 2));
    p->panelHeight = ph;

    SetWindowPos(p->wnd, nullptr, 0, height - ph, width, ph,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

    RECT tr;
    GetWindowRect(p->tree, &tr);
    MapWindowPoints(nullptr, p->sink, (POINT*)&tr, 2);
    int treeH = std::max(0, height - ph - (int)tr.top);
    if (tr.bottom - tr.top != treeH) {
        SetWindowPos(p->tree, nullptr, 0, 0, tr.right - tr.left, treeH,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    InvalidateRect(p->wnd, nullptr, FALSE);
    p->inLayout = false;
}

// No AddRef: the pointer belongs to the tab.
IShellBrowser* GetBrowser(Panel* p) {
    auto browser = (IShellBrowser*)SendMessageW(p->shellTab, CWM_GETISHELLBROWSER, 0, 0);
    if (!browser) {
        browser = (IShellBrowser*)SendMessageW(GetAncestor(p->shellTab, GA_ROOT), CWM_GETISHELLBROWSER, 0, 0);
    }
    return browser;
}

// Folder shown in the tab, to mark the open tag.
std::wstring GetCurrentFolder(Panel* p) {
    std::wstring result;
    IShellBrowser* browser = GetBrowser(p);
    if (!browser) {
        return result;
    }
    IShellView* view = nullptr;
    if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
        IFolderView* folderView = nullptr;
        if (SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&folderView)))) {
            IPersistFolder2* folder = nullptr;
            if (SUCCEEDED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
                PIDLIST_ABSOLUTE pidl = nullptr;
                if (SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
                    WCHAR path[MAX_PATH];
                    if (SHGetPathFromIDListW(pidl, path)) {
                        result = path;
                    }
                    CoTaskMemFree(pidl);
                }
                folder->Release();
            }
            folderView->Release();
        }
        view->Release();
    }
    return result;
}

bool Navigate(Panel* p, const std::wstring& folder) {
    IShellBrowser* browser = GetBrowser(p);
    if (!browser) {
        Wh_Log(L"No IShellBrowser in the tab");
        return false;
    }
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(folder.c_str(), nullptr, &pidl, 0, nullptr))) {
        return false;
    }
    HRESULT hr = browser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
    CoTaskMemFree(pidl);
    return SUCCEEDED(hr);
}

// After Navigate or ShellExecute the Panel may no longer exist (these calls
// pump messages): only the HWND saved beforehand is used.
void OpenTag(Panel* p, int index, bool newWindow) {
    // Count first, then check g_unloading: in the reverse order, unloading
    // could see zero between the two.
    BusyScope busy;
    auto s = GetSettings();
    if (g_unloading || index < 0 || index >= (int)s->tags.size()) {
        return;
    }
    HWND panelWnd = p->wnd;
    std::wstring folder = s->root + L"\\" + s->tags[index].folderName;

    // Not one disk call on this thread: creating the folder here froze the
    // whole Explorer window when the tags folder lived on a drive that had
    // stopped responding. The worker makes the folder and the panel opens it
    // when the answer comes back.
    p->pendingOpen = folder;
    p->pendingOpenNewWindow = newWindow;
    QueueEnsureFolder(s->tags[index].name);

    // Whether the folder exists is the worker's last word on it, which can be
    // out of date: a folder deleted behind its back still reads as ready, and
    // navigating there used to do nothing at all (measured 2026-09-18). So
    // the open is attempted, and on failure it waits for the worker instead.
    if (!FolderIsReady(folder)) {
        return;
    }
    bool opened;
    if (newWindow) {
        opened = (INT_PTR)ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr,
                                        SW_SHOWNORMAL) > 32;
    } else {
        opened = Navigate(p, folder);
        if (opened) {
            SetTimer(panelWnd, TIMER_REPAINT, 350, nullptr);
        }
    }
    if (opened) {
        p->pendingOpen.clear();
        // Fixes shortcuts for files moved since last time.
        QueueSync();
    }
}

struct Colors {
    COLORREF bk, text, dim, sep, hot, selected;
};

COLORREF Blend(COLORREF a, COLORREF b, double t) {
    return RGB((int)(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t),
               (int)(GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t),
               (int)(GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t));
}

Colors GetColors(Panel* p) {
    Colors c;
    HWND tv = TreeViewOf(p);
    c.bk = tv ? TreeView_GetBkColor(tv) : CLR_NONE;
    c.text = tv ? TreeView_GetTextColor(tv) : CLR_NONE;
    if (c.bk == CLR_NONE) {
        c.bk = GetSysColor(COLOR_WINDOW);
    }
    if (c.text == CLR_NONE) {
        c.text = GetSysColor(COLOR_WINDOWTEXT);
    }
    // Measured on the tree in dark theme: background 25, separator 56, selection 51.
    c.sep = Blend(c.bk, c.text, 0.135);
    c.selected = Blend(c.bk, c.text, 0.113);
    c.hot = Blend(c.bk, c.text, 0.075);
    c.dim = Blend(c.bk, c.text, 0.6);
    return c;
}

HFONT GetIconFont(Panel* p, int dpi) {
    if (!p->iconFont || p->iconFontDpi != dpi) {
        if (p->iconFont) {
            DeleteObject(p->iconFont);
        }
        p->iconFont = CreateFontW(-Dip(12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe Fluent Icons");
        p->iconFontDpi = dpi;
    }
    return p->iconFont;
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

void DrawTextAt(HDC hdc, const std::wstring& text, RECT rc, COLORREF color, UINT flags) {
    SetTextColor(hdc, color);
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rc,
              flags | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
}

void PaintPanel(Panel* p, HDC target) {
    RECT rc;
    GetClientRect(p->wnd, &rc);
    int width = rc.right;
    int height = rc.bottom;
    if (width <= 0 || height <= 0) {
        return;
    }

    auto s = GetSettings();
    Metrics m = GetMetrics(p, *s, height);
    ClampScroll(p, m);
    Colors c = GetColors(p);
    int dpi = m.dpi;

    HDC hdc = CreateCompatibleDC(target);
    HBITMAP bmp = CreateCompatibleBitmap(target, width, height);
    HGDIOBJ oldBmp = SelectObject(hdc, bmp);

    HBRUSH bk = CreateSolidBrush(c.bk);
    FillRect(hdc, &rc, bk);
    DeleteObject(bk);

    RECT sep = {Dip(10, dpi), m.sepY, width - Dip(8, dpi), m.sepY + 1};
    HBRUSH sepBrush = CreateSolidBrush(c.sep);
    FillRect(hdc, &sep, sepBrush);
    DeleteObject(sepBrush);

    int selected = HIT_NONE;
    const std::wstring& current = p->currentFolder;
    for (int i = 0; i < m.tagCount; i++) {
        std::wstring folder = s->root + L"\\" + s->tags[i].folderName;
        if (!current.empty() && _wcsicmp(current.c_str(), folder.c_str()) == 0) {
            selected = i;
        }
    }

    int headerTop = m.topPad;
    int rowsTop = headerTop + m.rowH;

    {
        Gdiplus::Graphics g(hdc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        // The tree's selection spans the full width (measured).
        int radius = Dip(4, dpi);

        if (p->hot == HIT_HEADER) {
            FillRounded(g, c.hot, 0, headerTop, width, m.rowH, radius);
        }
        for (int row = 0; row < m.rowsShown; row++) {
            int i = row + p->scroll;
            if (i >= m.tagCount) {
                break;
            }
            int y = rowsTop + row * m.rowH;
            if (i == p->dropHot) {
                FillRounded(g, Blend(c.bk, s->tags[i].color, 0.35), 0, y, width, m.rowH, radius);
            } else if (i == selected) {
                FillRounded(g, c.selected, 0, y, width, m.rowH, radius);
            } else if (i == p->hot) {
                FillRounded(g, c.hot, 0, y, width, m.rowH, radius);
            }

            COLORREF tc = s->tags[i].color;
            Gdiplus::SolidBrush dot(Gdiplus::Color(255, GetRValue(tc), GetGValue(tc), GetBValue(tc)));
            Gdiplus::REAL d = (Gdiplus::REAL)Dip(10, dpi);
            Gdiplus::REAL cx = (Gdiplus::REAL)Dip(47, dpi);
            Gdiplus::REAL cy = (Gdiplus::REAL)y + m.rowH / 2.0f;
            g.FillEllipse(&dot, cx - d / 2, cy - d / 2, d, d);
        }
    }

    SetBkMode(hdc, TRANSPARENT);

    // Title: collapse arrow, tag icon, "Tags".
    HGDIOBJ oldFont = SelectObject(hdc, GetIconFont(p, dpi));
    RECT chevron = {Dip(8, dpi), headerTop, Dip(24, dpi), headerTop + m.rowH};
    DrawTextAt(hdc, g_collapsed ? L"\uE76C" : L"\uE70D", chevron, c.dim, DT_CENTER);
    RECT icon = {Dip(31, dpi), headerTop, Dip(47, dpi), headerTop + m.rowH};
    DrawTextAt(hdc, L"\uE8EC", icon, c.text, DT_CENTER);

    HFONT treeFont = (HFONT)SendMessageW(TreeViewOf(p), WM_GETFONT, 0, 0);
    SelectObject(hdc, treeFont ? treeFont : (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    RECT title = {Dip(53, dpi), headerTop, width - Dip(8, dpi), headerTop + m.rowH};
    DrawTextAt(hdc, L"Tags", title, c.text, DT_LEFT);

    if (!g_collapsed) {
        if (m.tagCount == 0 && m.rowsShown > 0) {
            RECT r = {Dip(61, dpi), rowsTop, width - Dip(8, dpi), rowsTop + m.rowH};
            DrawTextAt(hdc, L"Create tags in the mod settings", r, c.dim, DT_LEFT);
        }
        for (int row = 0; row < m.rowsShown; row++) {
            int i = row + p->scroll;
            if (i >= m.tagCount) {
                break;
            }
            int y = rowsTop + row * m.rowH;
            int count = GetCount(s->tags[i].name);
            int countW = 0;
            if (count > 0) {
                std::wstring countText = std::to_wstring(count);
                RECT measure = {0, 0, 0, 0};
                DrawTextW(hdc, countText.c_str(), -1, &measure, DT_SINGLELINE | DT_CALCRECT);
                countW = measure.right + Dip(8, dpi);
                RECT cr = {width - Dip(12, dpi) - measure.right, y, width - Dip(12, dpi), y + m.rowH};
                DrawTextAt(hdc, countText, cr, c.dim, DT_RIGHT);
            }
            RECT tr = {Dip(61, dpi), y, width - Dip(12, dpi) - countW, y + m.rowH};
            DrawTextAt(hdc, s->tags[i].name, tr, c.text, DT_LEFT);
        }
    }

    SelectObject(hdc, oldFont);
    BitBlt(target, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(hdc);
}

int HitTest(Panel* p, POINT pt) {
    RECT rc;
    GetClientRect(p->wnd, &rc);
    if (pt.x < 0 || pt.x >= rc.right || pt.y < 0 || pt.y >= rc.bottom) {
        return HIT_NONE;
    }
    auto s = GetSettings();
    Metrics m = GetMetrics(p, *s, rc.bottom);
    ClampScroll(p, m);
    if (pt.y >= m.topPad && pt.y < m.topPad + m.rowH) {
        return HIT_HEADER;
    }
    int rowsTop = m.topPad + m.rowH;
    if (g_collapsed || pt.y < rowsTop) {
        return HIT_NONE;
    }
    int row = (pt.y - rowsTop) / m.rowH;
    if (row >= m.rowsShown) {
        return HIT_NONE;
    }
    int i = row + p->scroll;
    return i < m.tagCount ? i : HIT_NONE;
}

void SetHot(Panel* p, int hot) {
    if (p->hot != hot) {
        p->hot = hot;
        InvalidateRect(p->wnd, nullptr, FALSE);
    }
}

void ToggleCollapsed() {
    g_collapsed = !g_collapsed;
    Wh_SetIntValue(L"collapsed", g_collapsed ? 1 : 0);
    BroadcastMessage(g_msgLayout);
}

// ---------------------------------------------------------------------------
// Dropping files onto a tag
// ---------------------------------------------------------------------------

std::vector<std::wstring> GetDroppedPaths(IDataObject* data) {
    std::vector<std::wstring> paths;
    IShellItemArray* items = nullptr;
    if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(data, IID_PPV_ARGS(&items)))) {
        DWORD count = 0;
        items->GetCount(&count);
        for (DWORD i = 0; i < count; i++) {
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
    if (paths.empty()) {
        FORMATETC fe = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM stg;
        if (SUCCEEDED(data->GetData(&fe, &stg))) {
            HDROP drop = (HDROP)GlobalLock(stg.hGlobal);
            if (drop) {
                UINT n = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
                for (UINT i = 0; i < n; i++) {
                    WCHAR buf[4096];
                    if (DragQueryFileW(drop, i, buf, ARRAYSIZE(buf))) {
                        paths.push_back(buf);
                    }
                }
                GlobalUnlock(stg.hGlobal);
            }
            ReleaseStgMedium(&stg);
        }
    }
    return paths;
}

bool HasFiles(IDataObject* data) {
    FORMATETC hdrop = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    if (data->QueryGetData(&hdrop) == S_OK) {
        return true;
    }
    static CLIPFORMAT idList = (CLIPFORMAT)RegisterClipboardFormatW(CFSTR_SHELLIDLIST);
    FORMATETC ids = {idList, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    return data->QueryGetData(&ids) == S_OK;
}

void SetDropDescription(IDataObject* data, DROPIMAGETYPE type, PCWSTR message, PCWSTR insert) {
    static CLIPFORMAT cf = (CLIPFORMAT)RegisterClipboardFormatW(CFSTR_DROPDESCRIPTION);
    FORMATETC fe = {cf, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM stg = {};
    stg.tymed = TYMED_HGLOBAL;
    stg.hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));
    if (!stg.hGlobal) {
        return;
    }
    DROPDESCRIPTION* dd = (DROPDESCRIPTION*)GlobalLock(stg.hGlobal);
    if (!dd) {
        GlobalFree(stg.hGlobal);
        return;
    }
    dd->type = type;
    lstrcpynW(dd->szMessage, message, ARRAYSIZE(dd->szMessage));
    lstrcpynW(dd->szInsert, insert, ARRAYSIZE(dd->szInsert));
    GlobalUnlock(stg.hGlobal);
    if (FAILED(data->SetData(&fe, &stg, TRUE))) {
        ReleaseStgMedium(&stg);
    }
}

class TagDropTarget final : public IDropTarget {
   public:
    explicit TagDropTarget(HWND hwnd) : m_hwnd(hwnd) {
        CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&m_helper));
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) {
            delete this;
        }
        return ref;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data, DWORD, POINTL pt, DWORD* effect) override {
        BusyScope busy;
        // Counted even when refused: OLE keeps this object and keeps calling
        // DragOver/DragLeave until the cursor leaves.
        BeginDrag();
        if (g_unloading) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        if (m_data) {
            m_data->Release();
        }
        m_data = data;
        m_data->AddRef();
        m_hasFiles = HasFiles(data);
        m_described = HIT_NONE - 1;
        POINT p = {pt.x, pt.y};
        Update(pt, effect);
        if (m_helper) {
            m_helper->DragEnter(m_hwnd, data, &p, *effect);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* effect) override {
        BusyScope busy;
        if (!m_inDrag) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        Update(pt, effect);
        if (m_helper) {
            POINT p = {pt.x, pt.y};
            m_helper->DragOver(&p, *effect);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        BusyScope busy;
        if (!m_inDrag) {
            return S_OK;
        }
        if (m_data) {
            SetDropDescription(m_data, DROPIMAGE_INVALID, L"", L"");
        }
        SetDropHot(HIT_NONE);
        if (m_helper) {
            m_helper->DragLeave();
        }
        if (m_data) {
            m_data->Release();
            m_data = nullptr;
        }
        EndDrag();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* data, DWORD, POINTL pt, DWORD* effect) override {
        BusyScope busy;
        if (!m_inDrag) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        int index = Update(pt, effect);
        if (m_helper) {
            POINT p = {pt.x, pt.y};
            m_helper->Drop(data, &p, *effect);
        }
        SetDropDescription(data, DROPIMAGE_INVALID, L"", L"");
        SetDropHot(HIT_NONE);

        if (index >= 0 && *effect != DROPEFFECT_NONE) {
            auto s = GetSettings();
            std::vector<std::wstring> paths = GetDroppedPaths(data);
            if (index < (int)s->tags.size() && !paths.empty()) {
                QueueAdd(s->tags[index].name, std::move(paths));
            } else {
                *effect = DROPEFFECT_NONE;
            }
        }
        if (m_data) {
            m_data->Release();
            m_data = nullptr;
        }
        EndDrag();
        return S_OK;
    }

   private:
    void BeginDrag() {
        if (!m_inDrag) {
            m_inDrag = true;
            g_drags++;
        }
    }
    void EndDrag() {
        if (m_inDrag) {
            m_inDrag = false;
            g_drags--;
        }
    }

    ~TagDropTarget() {
        EndDrag();
        if (m_helper) {
            m_helper->Release();
        }
        if (m_data) {
            m_data->Release();
        }
    }

    void SetDropHot(int index) {
        Panel* p = FindPanel(m_hwnd);
        if (p && p->dropHot != index) {
            p->dropHot = index;
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    int Update(POINTL pt, DWORD* effect) {
        Panel* p = FindPanel(m_hwnd);
        POINT client = {pt.x, pt.y};
        ScreenToClient(m_hwnd, &client);
        int index = (p && m_hasFiles && !g_unloading) ? HitTest(p, client) : HIT_NONE;
        if (index < 0) {
            index = HIT_NONE;
        }

        // Never DROPEFFECT_MOVE: the source would delete the files.
        DWORD allowed = *effect;
        if (index >= 0 && (allowed & DROPEFFECT_LINK)) {
            *effect = DROPEFFECT_LINK;
        } else if (index >= 0 && (allowed & DROPEFFECT_COPY)) {
            *effect = DROPEFFECT_COPY;
        } else {
            *effect = DROPEFFECT_NONE;
            index = HIT_NONE;
        }

        SetDropHot(index);
        if (m_data && index != m_described) {
            m_described = index;
            auto s = GetSettings();
            if (index >= 0 && index < (int)s->tags.size()) {
                SetDropDescription(m_data, *effect == DROPEFFECT_LINK ? DROPIMAGE_LINK : DROPIMAGE_COPY,
                                   L"Tag as %1", s->tags[index].name.c_str());
            } else {
                SetDropDescription(m_data, DROPIMAGE_INVALID, L"", L"");
            }
        }
        return index;
    }

    LONG m_ref = 1;
    HWND m_hwnd;
    IDropTargetHelper* m_helper = nullptr;
    IDataObject* m_data = nullptr;
    bool m_hasFiles = false;
    bool m_inDrag = false;
    int m_described = HIT_NONE - 1;
};

// ---------------------------------------------------------------------------
// Panel window
// ---------------------------------------------------------------------------

LRESULT CALLBACK PanelWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Panel* p = (Panel*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

    if (msg == WM_NCCREATE) {
        auto cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    if (!p) {
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    if (msg == g_msgRefresh) {
        // A click that had to wait for its folder to be created.
        if (!p->pendingOpen.empty() && FolderIsReady(p->pendingOpen)) {
            std::wstring folder = p->pendingOpen;
            bool newWindow = p->pendingOpenNewWindow;
            p->pendingOpen.clear();
            if (newWindow) {
                ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            } else {
                Navigate(p, folder);
                SetTimer(hWnd, TIMER_REPAINT, 350, nullptr);
            }
        }
        p->currentFolder = GetCurrentFolder(p);
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    if (msg == g_msgCancelMenu) {
        // Arrives inside the TrackPopupMenu loop, on the right thread.
        EndMenu();
        return 0;
    }
    if (msg == g_msgLayout) {
        LayoutPanel(p);
        return 0;
    }

    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            PaintPanel(p, hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!p->trackingLeave) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
                p->trackingLeave = TrackMouseEvent(&tme);
            }
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetHot(p, HitTest(p, pt));
            return 0;
        }

        case WM_MOUSELEAVE:
            p->trackingLeave = false;
            SetHot(p, HIT_NONE);
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            p->pressed = HitTest(p, pt);
            SetCapture(hWnd);
            return 0;
        }

        case WM_LBUTTONUP: {
            if (GetCapture() == hWnd) {
                ReleaseCapture();
            }
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hit = HitTest(p, pt);
            int pressed = p->pressed;
            p->pressed = HIT_NONE;
            if (hit != pressed) {
                return 0;
            }
            if (hit == HIT_HEADER) {
                ToggleCollapsed();
            } else if (hit >= 0) {
                OpenTag(p, hit, false);
            }
            return 0;
        }

        case WM_MBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hit = HitTest(p, pt);
            if (hit >= 0) {
                OpenTag(p, hit, true);
            }
            return 0;
        }

        case WM_CONTEXTMENU: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            POINT client = pt;
            if (pt.x == -1 && pt.y == -1) {
                return 0;
            }
            ScreenToClient(hWnd, &client);
            int hit = HitTest(p, client);
            if (hit < 0) {
                return 0;
            }
            int cmd;
            {
                BusyScope busy;
                if (g_unloading) {
                    return 0;
                }
                HMENU menu = CreatePopupMenu();
                AppendMenuW(menu, MF_STRING, 1, L"Open");
                AppendMenuW(menu, MF_STRING, 2, L"Open in new window");
                SetMenuDefaultItem(menu, 1, FALSE);
                cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
                DestroyMenu(menu);
            }
            // The menu pumps messages: the window may have closed meanwhile.
            p = (Panel*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
            if (p && (cmd == 1 || cmd == 2)) {
                OpenTag(p, hit, cmd == 2);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            RECT rc;
            GetClientRect(hWnd, &rc);
            auto s = GetSettings();
            Metrics m = GetMetrics(p, *s, rc.bottom);
            // Collapsed there is nothing to scroll, and scrolling anyway
            // drove p->scroll to the end of the list behind the user's back.
            if (!g_collapsed && m.tagCount > m.rowsShown) {
                // Same feel as the tree above: system lines per notch, and
                // partial notches (precision touchpads) accumulate.
                UINT lines = 3;
                SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &lines, 0);
                p->wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
                int notches = p->wheelDelta / WHEEL_DELTA;
                p->wheelDelta -= notches * WHEEL_DELTA;
                p->scroll -= notches * (int)(lines ? lines : 1);
                ClampScroll(p, m);
                // The rows moved under a cursor that didn't: without this the
                // highlight stays on the row that used to be there.
                POINT cursor;
                if (GetCursorPos(&cursor)) {
                    ScreenToClient(hWnd, &cursor);
                    SetHot(p, HitTest(p, cursor));
                }
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            break;
        }

        case WM_TIMER:
            if (wParam == TIMER_REPAINT) {
                KillTimer(hWnd, TIMER_REPAINT);
                // Only after navigation, not on every repaint: this is a COM
                // round trip and repaints happen on every hover change.
                p->currentFolder = GetCurrentFolder(p);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            break;

        case WM_DPICHANGED_AFTERPARENT:
            p->rowHeight = 0;
            LayoutPanel(p);
            return 0;

        case WM_DESTROY:
            if (p->drop) {
                RevokeDragDrop(hWnd);
                p->drop->Release();
                p->drop = nullptr;
            }
            KillTimer(hWnd, TIMER_REPAINT);
            break;

        case WM_NCDESTROY:
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            if (p->iconFont) {
                DeleteObject(p->iconFont);
                p->iconFont = nullptr;
            }
            p->wnd = nullptr;
            break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Connecting to the navigation tree
// ---------------------------------------------------------------------------

bool HasClass(HWND hwnd, PCWSTR name) {
    WCHAR cls[64];
    return hwnd && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, name) == 0;
}

LRESULT CALLBACK TreeSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK SinkSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

void RemoveTreeSubclass(HWND tree) {
    AcquireSRWLockExclusive(&g_panelsLock);
    g_subclassedTrees.erase(std::remove(g_subclassedTrees.begin(), g_subclassedTrees.end(), tree),
                            g_subclassedTrees.end());
    ReleaseSRWLockExclusive(&g_panelsLock);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(tree, TreeSubclassProc);
}

void SubclassTree(HWND tree) {
    AcquireSRWLockExclusive(&g_panelsLock);
    // g_unloading checked inside the lock: Wh_ModBeforeUninit sets it while
    // holding the lock, so a tree either ends up in the list it iterates or
    // never gets subclassed.
    bool already = g_unloading || std::find(g_subclassedTrees.begin(), g_subclassedTrees.end(),
                                            tree) != g_subclassedTrees.end();
    if (!already) {
        g_subclassedTrees.push_back(tree);
    }
    ReleaseSRWLockExclusive(&g_panelsLock);
    if (already) {
        return;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tree, TreeSubclassProc, 0)) {
        AcquireSRWLockExclusive(&g_panelsLock);
        g_subclassedTrees.erase(std::remove(g_subclassedTrees.begin(), g_subclassedTrees.end(), tree),
                                g_subclassedTrees.end());
        ReleaseSRWLockExclusive(&g_panelsLock);
        return;
    }
    PostMessageW(tree, g_msgAttach, 0, 0);
}

// Runs on the window thread.
void Attach(HWND tree) {
    if (g_unloading || FindPanel(tree)) {
        return;
    }
    HWND sink = GetParent(tree);
    HWND shellTab = nullptr;
    for (HWND h = sink; h; h = GetParent(h)) {
        if (HasClass(h, L"ShellTabWindowClass")) {
            shellTab = h;
            break;
        }
    }
    if (!HasClass(sink, L"CtrlNotifySink") || !shellTab) {
        // Tree belonging to something else (for example a dialog): not ours.
        RemoveTreeSubclass(tree);
        return;
    }

    Panel* p = new Panel();
    p->tree = tree;
    p->sink = sink;
    p->shellTab = shellTab;

    p->wnd = CreateWindowExW(0, PANEL_CLASS, L"Tags", WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0,
                             sink, nullptr, THIS_MODULE, p);
    if (!p->wnd) {
        Wh_Log(L"Couldn't create the panel: %u", GetLastError());
        delete p;
        RemoveTreeSubclass(tree);
        return;
    }

    AcquireSRWLockExclusive(&g_panelsLock);
    g_panels.push_back(p);
    ReleaseSRWLockExclusive(&g_panelsLock);

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(sink, SinkSubclassProc, 0)) {
        // Without it the panel doesn't follow the pane being resized.
        Wh_Log(L"Couldn't subclass the container %p", sink);
    }

    p->drop = new TagDropTarget(p->wnd);
    HRESULT hr = RegisterDragDrop(p->wnd, p->drop);
    if (FAILED(hr)) {
        Wh_Log(L"RegisterDragDrop failed: 0x%08X", hr);
        p->drop->Release();
        p->drop = nullptr;
    }

    p->currentFolder = GetCurrentFolder(p);
    LayoutPanel(p);
    Wh_Log(L"Panel attached to tree %p", tree);
}

// Runs on the window thread.
void Detach(Panel* p) {
    AcquireSRWLockExclusive(&g_panelsLock);
    g_panels.erase(std::remove(g_panels.begin(), g_panels.end(), p), g_panels.end());
    ReleaseSRWLockExclusive(&g_panelsLock);

    if (p->wnd) {
        DestroyWindow(p->wnd);
    }
    // If the panel was already partway through being destroyed (the
    // container closing), DestroyWindow returns without doing anything and
    // its WM_NCDESTROY arrives later: it can't find this Panel.
    if (p->wnd && IsWindow(p->wnd)) {
        SetWindowLongPtrW(p->wnd, GWLP_USERDATA, 0);
    }
    if (p->drop) {
        if (p->wnd && IsWindow(p->wnd)) {
            RevokeDragDrop(p->wnd);
        }
        p->drop->Release();
        p->drop = nullptr;
    }
    if (p->iconFont) {
        DeleteObject(p->iconFont);
        p->iconFont = nullptr;
    }
    if (IsWindow(p->sink)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(p->sink, SinkSubclassProc);
        if (IsWindow(p->tree)) {
            RECT rc;
            GetClientRect(p->sink, &rc);
            RECT tr;
            GetWindowRect(p->tree, &tr);
            MapWindowPoints(nullptr, p->sink, (POINT*)&tr, 2);
            SetWindowPos(p->tree, nullptr, 0, 0, tr.right - tr.left, std::max(0, (int)(rc.bottom - tr.top)),
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    delete p;
}

LRESULT CALLBACK TreeSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (msg == g_msgAttach) {
        Attach(hWnd);
        return 0;
    }
    if (msg == g_msgDetach) {
        if (Panel* p = FindPanel(hWnd)) {
            Detach(p);
        }
        return 0;
    }

    switch (msg) {
        case WM_WINDOWPOSCHANGING: {
            Panel* p = FindPanel(hWnd);
            auto wp = (WINDOWPOS*)lParam;
            if (p && p->wnd && !(wp->flags & SWP_NOSIZE)) {
                RECT rc;
                GetClientRect(p->sink, &rc);
                int top = wp->y;
                if (wp->flags & SWP_NOMOVE) {
                    RECT tr;
                    GetWindowRect(hWnd, &tr);
                    MapWindowPoints(nullptr, p->sink, (POINT*)&tr, 2);
                    top = tr.top;
                }
                int maxH = std::max(0, (int)rc.bottom - p->panelHeight - top);
                if (wp->cy > maxH) {
                    wp->cy = maxH;
                }
            }
            break;
        }

        case WM_NOTIFY: {
            auto hdr = (NMHDR*)lParam;
            if (hdr && hdr->code == TVN_SELCHANGEDW) {
                if (Panel* p = FindPanel(hWnd); p && p->wnd) {
                    SetTimer(p->wnd, TIMER_REPAINT, 350, nullptr);
                }
            }
            break;
        }

        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            if (Panel* p = FindPanel(hWnd); p && p->wnd) {
                // The tree's row height can change with the theme, and with it
                // the height the panel needs.
                p->rowHeight = 0;
                LayoutPanel(p);
            }
            break;

        case WM_NCDESTROY:
            if (Panel* p = FindPanel(hWnd)) {
                Detach(p);
            }
            AcquireSRWLockExclusive(&g_panelsLock);
            g_subclassedTrees.erase(
                std::remove(g_subclassedTrees.begin(), g_subclassedTrees.end(), hWnd),
                g_subclassedTrees.end());
            ReleaseSRWLockExclusive(&g_panelsLock);
            break;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK SinkSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (msg == WM_WINDOWPOSCHANGED) {
        LRESULT result = DefSubclassProc(hWnd, msg, wParam, lParam);
        Panel* p = FindPanel(hWnd);
        if (p && p->sink == hWnd) {
            LayoutPanel(p);
        }
        return result;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HookCall call;
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth,
                                         nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && hWndParent && !g_unloading &&
        (IS_INTRESOURCE(lpClassName) || _wcsicmp(lpClassName, L"NamespaceTreeControl") == 0) &&
        HasClass(hwnd, L"NamespaceTreeControl")) {
        SubclassTree(hwnd);
    }
    return hwnd;
}

BOOL CALLBACK FindTreesProc(HWND hwnd, LPARAM) {
    if (HasClass(hwnd, L"NamespaceTreeControl")) {
        SubclassTree(hwnd);
    }
    return TRUE;
}

BOOL CALLBACK FindExplorerWindowsProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && HasClass(hwnd, L"CabinetWClass")) {
        EnumChildWindows(hwnd, FindTreesProc, 0);
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

// Owners of menus with our entries currently open (under g_panelsLock), so
// they can be closed when unloading.
std::vector<HWND> g_menuOwners;

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
    if (HasClass(owner, L"SHELLDLL_DefView")) {
        return owner;
    }
    HWND parent = GetParent(owner);
    if (HasClass(owner, L"DirectUIHWND") && HasClass(parent, L"SHELLDLL_DefView")) {
        return parent;
    }
    return nullptr;
}

// Only views inside an Explorer tab (the desktop is excluded: WM_USER+7 for
// other windows has no guaranteed meaning).
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

std::vector<std::wstring> GetSelectedPaths(HWND defView) {
    std::vector<std::wstring> paths;
    IShellBrowser* browser = BrowserForDefView(defView);
    if (!browser) {
        return paths;
    }
    IShellView* view = nullptr;
    if (FAILED(browser->QueryActiveShellView(&view)) || !view) {
        return paths;
    }
    HWND viewWnd = nullptr;
    view->GetWindow(&viewWnd);
    IFolderView2* folderView = nullptr;
    // The active view has to be the menu's, otherwise the selection is from another one.
    if (viewWnd == defView && SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&folderView)))) {
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
    view->Release();
    return paths;
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

template <typename Original>
BOOL TrackWithTagMenu(HMENU menu, UINT flags, HWND owner, Original original) {
    TrackDepth depth;
    // Without TPM_RETURNCMD the command would arrive as WM_COMMAND to the
    // view, which doesn't know it: in that case the menu isn't touched.
    HWND defView = (t_trackDepth == 1 && menu && (flags & TPM_RETURNCMD)) ? DefViewFor(owner) : nullptr;
    if (!defView) {
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

    AcquireSRWLockExclusive(&g_panelsLock);
    g_menuOwners.push_back(owner);
    ReleaseSRWLockExclusive(&g_panelsLock);

    BOOL result = original();

    AcquireSRWLockExclusive(&g_panelsLock);
    auto it = std::find(g_menuOwners.begin(), g_menuOwners.end(), owner);
    if (it != g_menuOwners.end()) {
        g_menuOwners.erase(it);
    }
    ReleaseSRWLockExclusive(&g_panelsLock);

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
    return TrackWithTagMenu(hMenu, uFlags, hwnd, [&]() {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hwnd, lptpm);
    });
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd,
                                const RECT* prcRect) {
    HookCall call;
    return TrackWithTagMenu(hMenu, uFlags, hWnd, [&]() {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    });
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

    // Windhawk loads a fresh copy of the image on every enable, so these are
    // already zero; setting them costs nothing and means no future change to
    // the teardown path can leave the mod inert without it being obvious.
    g_unloading = false;
    g_workerThread = nullptr;
    g_busy = 0;
    g_drags = 0;
    g_hookCalls = 0;

    LoadSettings();
    g_collapsed = Wh_GetIntValue(L"collapsed", 0) != 0;

    g_dbMutex = CreateMutexW(nullptr, FALSE, L"Local\\WindhawkExplorerTagsDb");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_dbMutex || !g_stopEvent || !g_workEvent) {
        return InitFailed();
    }

    g_msgAttach = RegisterWindowMessageW(L"WhExplorerTags_Attach");
    g_msgDetach = RegisterWindowMessageW(L"WhExplorerTags_Detach");
    g_msgRefresh = RegisterWindowMessageW(L"WhExplorerTags_Refresh");
    g_msgLayout = RegisterWindowMessageW(L"WhExplorerTags_Layout");
    g_msgCancelMenu = RegisterWindowMessageW(L"WhExplorerTags_CancelMenu");

    WNDCLASSW wc = {};
    wc.lpfnWndProc = PanelWndProc;
    wc.hInstance = THIS_MODULE;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = PANEL_CLASS;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        return InitFailed();
    }

    Gdiplus::GdiplusStartupInput gdiplusInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        g_gdiplusToken = 0;
        UnregisterClassW(PANEL_CLASS, THIS_MODULE);
        return InitFailed();
    }

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Couldn't hook CreateWindowExW: no panel will be created");
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        UnregisterClassW(PANEL_CLASS, THIS_MODULE);
        return InitFailed();
    }
    // The panel works without these two; only the context menu is lost.
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
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    // The worker notices a changed tags folder by itself; stopping it here
    // would block Windhawk's engine thread on whatever it is doing.
    QueueSync();
    BroadcastMessage(g_msgLayout);
}

// A menu the user leaves open would otherwise hold unloading forever. Waiting
// is what keeps code from being unmapped under a live call, so it is worth
// some patience, but not an unbounded hang of Windhawk's engine: past the
// deadline it gives up and says so. Only the menu-driven counts get a
// deadline; the drag count must not, because OLE holds a pointer into this
// image until it releases the drop target.
const DWORD UNLOAD_WAIT_MS = 10000;

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
    AcquireSRWLockExclusive(&g_panelsLock);
    g_unloading = true;
    std::vector<HWND> trees = g_subclassedTrees;
    ReleaseSRWLockExclusive(&g_panelsLock);

    // A drag, a menu, or a navigation in progress will still return to mod
    // code. With g_unloading set, none new can start; an open menu is
    // closed.
    BroadcastMessage(g_msgCancelMenu);
    AcquireSRWLockShared(&g_panelsLock);
    for (HWND owner : g_menuOwners) {
        // DefWindowProc closes the current menu with WM_CANCELMODE; if the
        // view doesn't do it, the user is expected to close it.
        PostMessageW(owner, WM_CANCELMODE, 0, 0);
    }
    ReleaseSRWLockShared(&g_panelsLock);

    // Detach FIRST, then wait. Detach revokes and releases the mod's own
    // reference on each drop target, so when OLE lets go of its reference the
    // object is destroyed and its destructor ends the drag. Waiting first
    // would hang forever in exactly the case being guarded against: a drag
    // whose source died never gets a DragLeave, and the mod's own reference
    // would keep the count above zero.
    //
    // The reorder is safe because Detach runs on the window's own thread via
    // SendMessage, so it can't interleave with an IDropTarget call, and the
    // code already survives the panel disappearing under it (FindPanel then
    // returns null and the drop methods do nothing).
    for (HWND tree : trees) {
        if (IsWindow(tree)) {
            SendMessageW(tree, g_msgDetach, 0, 0);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(tree, TreeSubclassProc);
        }
    }

    WaitForZero(g_busy, L"mod code on a window thread");
    // No deadline here: OLE still holds a pointer into this image, and detach
    // above released the mod's own reference, so this ends as soon as OLE
    // lets go, including when the drag source died.
    while (g_drags > 0) {
        Sleep(20);
    }

    StopWorker();
}

void Wh_ModUninit() {
    // The hooks have already been removed; what's left is exiting calls that
    // had already entered. The count covers every popup menu in the process,
    // including ones the mod never touched, so this waits with a deadline.
    WaitForZero(g_hookCalls, L"calls inside the hooks");
    if (!UnregisterClassW(PANEL_CLASS, THIS_MODULE)) {
        // The next load would then fail with ERROR_CLASS_ALREADY_EXISTS and
        // the mod would be inert in this process with no trace of why.
        Wh_Log(L"UnregisterClass failed: %u", GetLastError());
    }
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    CloseHandles();
    Wh_Log(L"<");
}
