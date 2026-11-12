// ==WindhawkMod==
// @id              explorer-taskbar-folder-thumbnails
// @name            Folder thumbnails on taskbar buttons
// @description     Each File Explorer taskbar button shows a preview of the folder it has open, the way that folder looks in Explorer's own icon view
// @version         1.0
// @author          Issuch
// @github          https://github.com/Issuch
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -lgdi32 -lcomctl32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Folder thumbnails on taskbar buttons

Every File Explorer window gets a taskbar icon showing the folder it has open,
drawn the way Explorer draws that folder in its own large-icon view. A folder
full of photos shows those photos on its taskbar button.

Without the mod, every File Explorer window has the same icon:

![Taskbar without the mod](https://raw.githubusercontent.com/Issuch/explorer-taskbar-folder-thumbnails/screenshots/taskbar-before.png)

With the mod, each one shows the folder it has open:

![Taskbar with the mod](https://raw.githubusercontent.com/Issuch/explorer-taskbar-folder-thumbnails/screenshots/taskbar-after.png)

## Recommended: FolderThumbnailFix
The mod works best together with
[FolderThumbnailFix](https://github.com/LesFerch/FolderThumbnailFix) by
LesFerch. Windows 11 draws the front of a folder over the folder's preview,
which hides most of the pictures inside. FolderThumbnailFix removes that cover,
so a taskbar button shows the pictures in full, as in the screenshot above.

## No delay
The mod takes over the window procedure of each Explorer window. When Explorer
changes a window's icon after a navigation or a tab switch, the mod substitutes
the folder preview within that very message. The taskbar therefore gets a
single icon update, at the same moment it would have received Explorer's own
icon, and the generic folder icon never appears in between. A newly opened
window gets its preview before it is shown for the first time.

Previews come from an in-memory cache, from the mod's own disk cache or from
the Windows thumbnail cache. The disk cache is kept in the mod's Windhawk
storage, in a folder which only your account can open, and is removed together
with the mod.
When a preview is in none of them, the button keeps Explorer's own folder icon
and the preview is rendered in the background, usually within a fraction of a
second, and then saved for next time.

## No polling
The mod sleeps while nothing happens. It reacts to events: a window being
shown, a navigation, a tab switch, a change among the files of an open folder,
and a window being closed. A folder is re-rendered when its files change, and
once in the background when it is opened again later, so that changes made
while it was closed show up too. That refresh is usually served from the
Windows thumbnail cache.

## Requirements
* **[Disable grouping on the taskbar](https://windhawk.net/mods/taskbar-grouping)
  with its "Use window icons" option.** The mod puts the preview on each
  Explorer window, and that mod gives each window its own taskbar button and
  makes the button show the window's icon instead of the Explorer icon. To use
  window icons for File Explorer only, leave "Use window icons" off and add
  `explorer.exe` to "Use window icons exceptions".
* **Thumbnails enabled.** In File Explorer, under Options -> View, the checkbox
  "Always show icons, never thumbnails" must be unchecked.

The mod has no settings. The preview resolution follows the screen's DPI,
previews are kept on disk so that they are instant after a reboot, and the old
ones are discarded once there are too many of them.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <sddl.h>
#include <aclapi.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cwchar>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// {4C96BE40-915C-11CF-99D3-00AA004AE837}
static const GUID kSID_STopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

// Undocumented but long-standing: an Explorer frame or tab returns its
// IShellBrowser (not AddRef'ed) in response to this message, in-process only.
static const UINT kCwmGetIShellBrowser = WM_USER + 7;

static const LONG kNotifyEvents = SHCNE_CREATE | SHCNE_DELETE |
                                  SHCNE_RENAMEITEM | SHCNE_UPDATEITEM |
                                  SHCNE_UPDATEDIR | SHCNE_MKDIR | SHCNE_RMDIR;

// Messages to the mod's worker window.
static const UINT kMsgRenderJobs = WM_APP + 1;
static const UINT kMsgWindowGone = WM_APP + 2;   // wParam: the closed window
static const UINT kMsgCheckWindow = WM_APP + 3;  // wParam: window to resolve
static const UINT kNotifyMsgFirst = WM_APP + 0x100;
static const UINT kNotifyMsgLast = 0xBFFF;

// A navigation is caught by the window procedure, so these are only a safety
// net for a window which turned up some other way.
static const DWORD kConfirmDelays[] = {150, 600};
static const int kNoConfirms = ARRAYSIZE(kConfirmDelays);

// A burst of file changes in the open folder collapses into one re-render.
static const DWORD kContentDelay = 1500;

// A folder rendered more recently than this is taken from the cache as is.
static const ULONGLONG kRerenderAfter = 30 * 1000;

// The result of a folder lookup is reused for this long, so that the two
// WM_SETICON messages of one navigation don't each repeat the lookup.
static const ULONGLONG kResolveMemo = 100;

static const size_t kMemCacheMax = 128;
static const int kDiskCacheMax = 1000;
static const int kDiskCleanupEvery = 50;  // saves

static const DWORD kCacheMagic = 0x43485446;  // "FTHC"
static const DWORD kCacheVersion = 2;

static const DWORD kSendTimeout = 1000;

template <typename T>
struct ComPtr {
    T* p = nullptr;
    ComPtr() = default;
    ~ComPtr() { reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    void reset() {
        if (p) {
            p->Release();
            p = nullptr;
        }
    }
    T** put() {
        reset();
        return &p;
    }
    T* get() const { return p; }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

struct CacheHeader {
    DWORD magic;
    DWORD version;
    DWORD size;
    DWORD pathLen;
};

// ---------------------------------------------------------------------------
// Shared state. Explorer's window threads and the mod's worker thread both
// reach it, under g_lock. The lock is never held while a message is sent.

struct CacheEntry {
    HICON icon;
    DWORD hash;
    ULONGLONG lastUse;
    ULONGLONG lastRender;  // 0 when it was loaded from a cache, not rendered
    bool onDisk;
};

// A folder handed from an Explorer thread to the worker, so that the worker
// doesn't have to ask the busy Explorer thread where the window is.
struct RenderJob {
    HWND hwnd;
    std::wstring path;
    PIDLIST_ABSOLUTE pidl;  // owned by the job
};

struct ResolveMemo {
    ULONGLONG tick;
    std::wstring path;
};

using IconKey = std::pair<HWND, WPARAM>;  // window, ICON_BIG or ICON_SMALL

static SRWLOCK g_lock = SRWLOCK_INIT;

// Previews by "size|path". Every icon in here is alive and reusable.
static std::map<std::wstring, CacheEntry> g_cache;
// Every icon the mod has created, so that one can be told from Explorer's.
static std::set<HICON> g_owned;
// Icons waiting until nothing may still show them, then freed.
static std::set<HICON> g_retired;

// What the mod has set on each window. A window never holds a cached preview
// itself, only its own copy of one, so that whatever becomes of one window's
// icon (Explorer destroying it, say) can't reach the cache or another window.
// A copy named here is on its window, so it is never destroyed.
struct OurIcon {
    HICON copy;    // the handle the window holds
    HICON source;  // the cached preview, or the folder icon, it was copied from
};
static std::map<IconKey, OurIcon> g_ourIcons;
// Explorer's own icon for each window, to put back when the mod lets go.
static std::map<IconKey, HICON> g_explorerIcons;
// The folder whose preview a window currently shows.
static std::map<HWND, std::wstring> g_shownPath;
// The folder each window was last found to have open. A preview rendered for
// any other folder arrived too late and is not put on the window.
static std::map<HWND, std::wstring> g_currentPath;
// Copies a timed-out WM_SETICON involves: the one it carries, and the one it
// replaces. A timed-out message is still delivered once the window's thread
// catches up, so both stay alive until that delivery, a later send from
// another thread (which is delivered after it), or the window closing.
static std::multimap<IconKey, HICON> g_unconfirmed;
// Copies found destroyed by someone else. Still counted as the mod's, so that
// Explorer is never handed one, but never destroyed again.
static std::set<HICON> g_deadCopies;
// Explorer windows whose window procedure the mod has taken over.
static std::set<HWND> g_subclassed;
static std::set<HWND> g_recheckPending;
static std::map<HWND, ResolveMemo> g_resolveMemo;
static std::vector<RenderJob> g_jobs;
// A plain folder icon, copied for a window whose own Explorer icon isn't
// known. Created once in Wh_ModInit, and never put on a window itself.
static HICON g_folderIcon = nullptr;

static std::atomic<bool> g_enabled{false};
static std::atomic<HWND> g_workerWnd{nullptr};

// Set once in Wh_ModInit and read everywhere after.
static int g_iconSize = 64;
static std::wstring g_cacheDir;
static UINT g_msgRestoreIcons = 0;
static UINT g_msgRecheck = 0;

static thread_local bool g_inSync = false;

static HANDLE g_stopEvent = nullptr;
static HANDLE g_thread = nullptr;

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t ShowWindow_Original = nullptr;

// ---------------------------------------------------------------------------
// Worker-only state

struct WindowState {
    std::wstring path;
    bool known = false;
    ULONG notifyId = 0;
    UINT notifyMsg = 0;
};

struct Pending {
    ULONGLONG due;
    bool forceRender;
    int confirmStep;  // index into kConfirmDelays; kNoConfirms = none
};

struct FoundWindow {
    HWND hwnd;
    PIDLIST_ABSOLUTE pidl;
    std::wstring path;
};

static std::map<HWND, WindowState> g_windows;
static std::map<HWND, Pending> g_pending;
static std::map<UINT, HWND> g_notifyMsgToHwnd;
static UINT g_nextNotifyMsg = kNotifyMsgFirst;
static HWND g_workerWndOwned = nullptr;
static int g_savesSinceCleanup = 0;
static bool g_jobsPosted = false;

// ---------------------------------------------------------------------------
// Pixels and icons

static DWORD HashPixels(const std::vector<DWORD>& px) {
    DWORD hash = 2166136261u;
    const BYTE* bytes = reinterpret_cast<const BYTE*>(px.data());
    for (size_t i = 0; i < px.size() * sizeof(DWORD); i++) {
        hash = (hash ^ bytes[i]) * 16777619u;
    }
    return hash;
}

static HICON PixelsToIcon(const std::vector<DWORD>& px, int size) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP color =
        CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color || !bits) {
        if (color) {
            DeleteObject(color);
        }
        return nullptr;
    }
    memcpy(bits, px.data(), px.size() * sizeof(DWORD));
    GdiFlush();

    std::vector<BYTE> maskBits(((size + 15) / 16) * 2 * size, 0);
    HBITMAP mask = CreateBitmap(size, size, 1, 1, maskBits.data());

    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmMask = mask;
    ii.hbmColor = color;
    HICON icon = CreateIconIndirect(&ii);

    DeleteObject(mask);
    DeleteObject(color);
    return icon;
}

// Turns a shell bitmap into a square buffer of straight-alpha pixels.
static bool BitmapToPixels(HBITMAP src, int size, std::vector<DWORD>& out) {
    BITMAP bm{};
    if (!GetObjectW(src, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0) {
        return false;
    }

    const int w = bm.bmWidth;
    const int h = bm.bmHeight;
    std::vector<DWORD> px(static_cast<size_t>(w) * h);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, src, 0, h, px.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (lines != h) {
        return false;
    }

    // Photo thumbnails often come without alpha; folder previews come with
    // premultiplied alpha.
    bool hasAlpha = false;
    bool premultiplied = true;
    for (DWORD p : px) {
        BYTE a = static_cast<BYTE>(p >> 24);
        BYTE r = static_cast<BYTE>(p >> 16);
        BYTE g = static_cast<BYTE>(p >> 8);
        BYTE b = static_cast<BYTE>(p);
        if (a) {
            hasAlpha = true;
        }
        if (r > a || g > a || b > a) {
            premultiplied = false;
        }
    }

    out.assign(static_cast<size_t>(size) * size, 0);
    const int ox = (size - w) / 2;
    const int oy = (size - h) / 2;
    for (int y = 0; y < h; y++) {
        int dy = y + oy;
        if (dy < 0 || dy >= size) {
            continue;
        }
        for (int x = 0; x < w; x++) {
            int dx = x + ox;
            if (dx < 0 || dx >= size) {
                continue;
            }
            DWORD p = px[static_cast<size_t>(y) * w + x];
            if (!hasAlpha) {
                p |= 0xFF000000;
            } else if (premultiplied) {
                DWORD a = p >> 24;
                if (a && a < 255) {
                    DWORD r = ((p >> 16) & 0xFF) * 255 / a;
                    DWORD g = ((p >> 8) & 0xFF) * 255 / a;
                    DWORD b = (p & 0xFF) * 255 / a;
                    p = (a << 24) | (r << 16) | (g << 8) | b;
                }
            }
            out[static_cast<size_t>(dy) * size + dx] = p;
        }
    }
    return true;
}

// cacheOnly: take the image from the Windows thumbnail cache only, without
// extracting anything, which is cheap enough for an Explorer window thread.
static bool RenderFolderPixels(PCIDLIST_ABSOLUTE pidl,
                               std::vector<DWORD>& out,
                               bool cacheOnly) {
    ComPtr<IShellItemImageFactory> factory;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put())))) {
        return false;
    }

    SIIGBF flags = SIIGBF_RESIZETOFIT;
    if (cacheOnly) {
        flags = static_cast<SIIGBF>(flags | SIIGBF_INCACHEONLY);
    }

    HBITMAP bmp = nullptr;
    SIZE sz = {g_iconSize, g_iconSize};
    if (FAILED(factory->GetImage(sz, flags, &bmp)) || !bmp) {
        return false;
    }

    bool ok = BitmapToPixels(bmp, g_iconSize, out);
    DeleteObject(bmp);
    return ok;
}

// ---------------------------------------------------------------------------
// Disk cache

static std::wstring NormalizePath(const std::wstring& path) {
    std::wstring lower = path;
    if (!lower.empty()) {
        CharLowerBuffW(&lower[0], static_cast<DWORD>(lower.size()));
    }
    return lower;
}

static std::wstring CacheFileFor(const std::wstring& key) {
    uint64_t hash = 14695981039346656037ull;
    for (wchar_t c : key) {
        hash = (hash ^ static_cast<uint16_t>(c)) * 1099511628211ull;
    }
    wchar_t name[32];
    swprintf(name, ARRAYSIZE(name), L"%016llx.bin",
             static_cast<unsigned long long>(hash));
    return g_cacheDir + L"\\" + name;
}

static std::wstring CurrentUserSid() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return {};
    }

    std::wstring result;
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> buffer(size);
    if (size &&
        GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
        PWSTR sid = nullptr;
        auto* user = reinterpret_cast<TOKEN_USER*>(buffer.data());
        if (ConvertSidToStringSidW(user->User.Sid, &sid)) {
            result = sid;
            LocalFree(sid);
        }
    }
    CloseHandle(token);
    return result;
}

// True when the folder's owner is this account (or the default owner of its
// token, e.g. Administrators for an elevated Explorer). An owner can rewrite
// the folder's permissions at any time, so a folder someone else created under
// this account's name must not be used.
static bool OwnedByCurrentUser(const std::wstring& dir) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }
    auto tokenInfo = [&](TOKEN_INFORMATION_CLASS cls) {
        DWORD size = 0;
        GetTokenInformation(token, cls, nullptr, 0, &size);
        std::vector<BYTE> buffer(size);
        if (!size || !GetTokenInformation(token, cls, buffer.data(), size, &size)) {
            buffer.clear();
        }
        return buffer;
    };
    const std::vector<BYTE> user = tokenInfo(TokenUser);
    const std::vector<BYTE> owner = tokenInfo(TokenOwner);
    CloseHandle(token);
    if (user.empty()) {
        return false;
    }

    PSID folderOwner = nullptr;
    PSECURITY_DESCRIPTOR sd = nullptr;
    if (GetNamedSecurityInfoW(dir.c_str(), SE_FILE_OBJECT,
                              OWNER_SECURITY_INFORMATION, &folderOwner,
                              nullptr, nullptr, nullptr, &sd) != ERROR_SUCCESS) {
        return false;
    }
    bool ok =
        folderOwner &&
        (EqualSid(folderOwner,
                  reinterpret_cast<const TOKEN_USER*>(user.data())->User.Sid) ||
         (!owner.empty() &&
          EqualSid(folderOwner,
                   reinterpret_cast<const TOKEN_OWNER*>(owner.data())->Owner)));
    LocalFree(sd);
    return ok;
}

// The cache lives in the mod's own Windhawk storage, which Windhawk removes
// together with the mod. That storage is shared by every account on the PC,
// while a preview shows what is inside one user's folders, so each account
// gets its own subfolder which only that account can open (plus SYSTEM and
// Administrators, so that removing the mod can delete it; an administrator
// could take the folder over anyway). Without a usable folder the disk cache
// is simply off.
static void InitCacheDir() {
    WCHAR storage[MAX_PATH];
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
        return;
    }

    const std::wstring sid = CurrentUserSid();
    if (sid.empty()) {
        return;
    }

    int created = SHCreateDirectoryExW(nullptr, storage, nullptr);
    if (created != ERROR_SUCCESS && created != ERROR_ALREADY_EXISTS &&
        created != ERROR_FILE_EXISTS) {
        return;
    }

    const std::wstring sddl =
        L"D:P(A;OICI;FA;;;" + sid + L")(A;OICI;FA;;;SY)(A;OICI;FA;;;BA)";
    PSECURITY_DESCRIPTOR sd = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &sd, nullptr)) {
        return;
    }

    const std::wstring dir = std::wstring(storage) + L"\\" + sid;
    SECURITY_ATTRIBUTES sa{sizeof(sa), sd, FALSE};
    bool ok = CreateDirectoryW(dir.c_str(), &sa);
    if (!ok && GetLastError() == ERROR_ALREADY_EXISTS) {
        // Use an existing folder only if this account owns it, and make sure
        // it still admits this account alone.
        ok = OwnedByCurrentUser(dir) &&
             SetFileSecurityW(dir.c_str(), DACL_SECURITY_INFORMATION, sd);
    }
    LocalFree(sd);

    if (ok) {
        g_cacheDir = dir;
    } else {
        Wh_Log(L"No private cache folder, the disk cache is off");
    }
}

// Worker only. Drops the least recently used files above the limit.
static void CleanupDiskCache() {
    if (g_cacheDir.empty()) {
        return;
    }

    struct Entry {
        std::wstring name;
        ULONGLONG time;
    };
    std::vector<Entry> entries;

    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileW((g_cacheDir + L"\\*.bin").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        ULARGE_INTEGER t;
        t.LowPart = fd.ftLastWriteTime.dwLowDateTime;
        t.HighPart = fd.ftLastWriteTime.dwHighDateTime;
        entries.push_back({fd.cFileName, t.QuadPart});
    } while (FindNextFileW(find, &fd));
    FindClose(find);

    if (static_cast<int>(entries.size()) <= kDiskCacheMax) {
        return;
    }

    std::sort(entries.begin(), entries.end(),
              [](const Entry& a, const Entry& b) { return a.time < b.time; });
    size_t excess = entries.size() - kDiskCacheMax;
    for (size_t i = 0; i < excess; i++) {
        DeleteFileW((g_cacheDir + L"\\" + entries[i].name).c_str());
    }
}

static bool LoadFromDisk(const std::wstring& path, std::vector<DWORD>& out) {
    if (g_cacheDir.empty() || path.empty()) {
        return false;
    }

    const std::wstring key = NormalizePath(path);
    HANDLE file = CreateFileW(CacheFileFor(key).c_str(),
                              GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                              FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool ok = false;
    DWORD read = 0;
    CacheHeader hdr{};
    if (ReadFile(file, &hdr, sizeof(hdr), &read, nullptr) &&
        read == sizeof(hdr) && hdr.magic == kCacheMagic &&
        hdr.version == kCacheVersion &&
        hdr.size == static_cast<DWORD>(g_iconSize) &&
        hdr.pathLen == key.size() && hdr.pathLen <= 32768) {
        std::wstring stored(hdr.pathLen, L'\0');
        DWORD pathBytes = hdr.pathLen * sizeof(wchar_t);
        if (ReadFile(file, stored.data(), pathBytes, &read, nullptr) &&
            read == pathBytes && stored == key) {
            out.assign(static_cast<size_t>(g_iconSize) * g_iconSize, 0);
            DWORD pixelBytes = static_cast<DWORD>(out.size() * sizeof(DWORD));
            ok = ReadFile(file, out.data(), pixelBytes, &read, nullptr) &&
                 read == pixelBytes;
        }
    }

    if (ok) {
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        SetFileTime(file, nullptr, nullptr, &now);  // least-recently-used mark
    }
    CloseHandle(file);
    return ok;
}

// Worker only.
static void SaveToDisk(const std::wstring& path, const std::vector<DWORD>& px) {
    if (g_cacheDir.empty() || path.empty()) {
        return;
    }

    const std::wstring key = NormalizePath(path);
    const std::wstring target = CacheFileFor(key);
    const std::wstring temp = target + L".tmp";

    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    CacheHeader hdr{kCacheMagic, kCacheVersion, static_cast<DWORD>(g_iconSize),
                    static_cast<DWORD>(key.size())};
    DWORD written = 0;
    bool ok =
        WriteFile(file, &hdr, sizeof(hdr), &written, nullptr) &&
        WriteFile(file, key.data(),
                  static_cast<DWORD>(key.size() * sizeof(wchar_t)), &written,
                  nullptr) &&
        WriteFile(file, px.data(), static_cast<DWORD>(px.size() * sizeof(DWORD)),
                  &written, nullptr);
    CloseHandle(file);

    if (!ok || !MoveFileExW(temp.c_str(), target.c_str(),
                            MOVEFILE_REPLACE_EXISTING)) {
        DeleteFileW(temp.c_str());
        return;
    }

    if (++g_savesSinceCleanup >= kDiskCleanupEvery) {
        g_savesSinceCleanup = 0;
        CleanupDiskCache();
    }
}

// ---------------------------------------------------------------------------
// Icon cache
//
// Two rules keep icons from disappearing. The previews in the cache never
// leave the mod: each window gets its own copy, so nothing outside the mod can
// free a cached preview. And a copy named in g_ourIcons or g_unconfirmed may be
// on a window, so it is never destroyed; one the mod can't prove is unused is
// leaked instead, and the process frees it on exit.

static std::wstring MakeKey(const std::wstring& path) {
    return std::to_wstring(g_iconSize) + L"|" + NormalizePath(path);
}

// Requires g_lock.
static std::set<HICON> IconsInUseLocked() {
    std::set<HICON> inUse;
    for (const auto& [key, our] : g_ourIcons) {
        inUse.insert(our.copy);
    }
    for (const auto& [key, icon] : g_unconfirmed) {
        inUse.insert(icon);
    }
    return inUse;
}

// Requires g_lock. Frees what no window shows and the cache no longer needs.
static void ReleaseUnusedLocked() {
    const std::set<HICON> inUse = IconsInUseLocked();

    for (auto it = g_retired.begin(); it != g_retired.end();) {
        if (g_deadCopies.count(*it)) {
            it = g_retired.erase(it);  // already destroyed, by someone else
            continue;
        }
        if (inUse.count(*it)) {
            ++it;
            continue;
        }
        g_owned.erase(*it);
        DestroyIcon(*it);
        it = g_retired.erase(it);
    }

    // No window holds a cached preview, only copies of one, so the least
    // recently used previews can always go.
    while (g_cache.size() > kMemCacheMax) {
        auto victim = g_cache.begin();
        for (auto it = g_cache.begin(); it != g_cache.end(); ++it) {
            if (it->second.lastUse < victim->second.lastUse) {
                victim = it;
            }
        }
        g_owned.erase(victim->second.icon);
        DestroyIcon(victim->second.icon);
        g_cache.erase(victim);
    }
}

static bool IsOwnedIcon(HICON icon) {
    if (!icon) {
        return false;
    }
    AcquireSRWLockShared(&g_lock);
    bool owned = g_owned.count(icon) != 0;
    ReleaseSRWLockShared(&g_lock);
    return owned;
}

// Any thread. Memory first, then disk. The icon belongs to the cache.
static HICON GetCachedIcon(const std::wstring& path) {
    if (path.empty()) {
        return nullptr;
    }
    const std::wstring key = MakeKey(path);

    AcquireSRWLockExclusive(&g_lock);
    auto it = g_cache.find(key);
    HICON cached = nullptr;
    if (it != g_cache.end()) {
        it->second.lastUse = GetTickCount64();
        cached = it->second.icon;
    }
    ReleaseSRWLockExclusive(&g_lock);

    if (cached) {
        // Never on a window, so nothing outside the mod can have freed it.
        return cached;
    }

    std::vector<DWORD> px;
    if (!LoadFromDisk(path, px)) {
        return nullptr;
    }
    HICON icon = PixelsToIcon(px, g_iconSize);
    if (!icon) {
        return nullptr;
    }
    const DWORD hash = HashPixels(px);

    AcquireSRWLockExclusive(&g_lock);
    it = g_cache.find(key);
    if (it != g_cache.end()) {
        DestroyIcon(icon);
        icon = it->second.icon;
        it->second.lastUse = GetTickCount64();
    } else {
        g_cache[key] = {icon, hash, GetTickCount64(), 0, true};
        g_owned.insert(icon);
        ReleaseUnusedLocked();
    }
    ReleaseSRWLockExclusive(&g_lock);
    return icon;
}

// True when the worker should render this folder rather than trust the cache.
static bool NeedsRender(const std::wstring& path, bool force) {
    AcquireSRWLockShared(&g_lock);
    auto it = g_cache.find(MakeKey(path));
    bool stale = it == g_cache.end() ||
                 GetTickCount64() - it->second.lastRender >= kRerenderAfter;
    ReleaseSRWLockShared(&g_lock);
    return force || stale;
}

// Stores freshly rendered pixels. byWorker also marks the entry as rendered,
// and reports through needSave whether the disk copy has to be written.
static HICON StoreRendered(const std::wstring& path,
                           const std::vector<DWORD>& px,
                           DWORD hash,
                           bool byWorker,
                           bool* needSave) {
    *needSave = false;
    const std::wstring key = MakeKey(path);
    const ULONGLONG now = GetTickCount64();

    HICON icon = nullptr;
    AcquireSRWLockExclusive(&g_lock);
    auto it = g_cache.find(key);
    if (it != g_cache.end() && it->second.hash == hash) {
        it->second.lastUse = now;
        if (byWorker) {
            it->second.lastRender = now;
            if (!it->second.onDisk) {
                it->second.onDisk = true;
                *needSave = true;
            }
        }
        icon = it->second.icon;
    }
    ReleaseSRWLockExclusive(&g_lock);
    if (icon) {
        return icon;
    }

    icon = PixelsToIcon(px, g_iconSize);
    if (!icon) {
        return nullptr;
    }

    AcquireSRWLockExclusive(&g_lock);
    it = g_cache.find(key);
    if (it != g_cache.end() && it->second.hash == hash) {
        DestroyIcon(icon);
        icon = it->second.icon;
        it->second.lastUse = now;
        if (byWorker) {
            it->second.lastRender = now;
        }
        ReleaseSRWLockExclusive(&g_lock);
        return icon;
    }
    if (it != g_cache.end()) {
        g_retired.insert(it->second.icon);  // no window holds a cached preview
        it->second = {icon, hash, now, byWorker ? now : 0, byWorker};
    } else {
        g_cache[key] = {icon, hash, now, byWorker ? now : 0, byWorker};
    }
    g_owned.insert(icon);
    ReleaseUnusedLocked();
    ReleaseSRWLockExclusive(&g_lock);

    *needSave = byWorker;
    return icon;
}

// ---------------------------------------------------------------------------
// Setting and restoring window icons

static bool IsValidIcon(HICON icon) {
    if (!icon) {
        return false;
    }
    ICONINFO ii{};
    if (!GetIconInfo(icon, &ii)) {
        return false;
    }
    if (ii.hbmColor) {
        DeleteObject(ii.hbmColor);
    }
    if (ii.hbmMask) {
        DeleteObject(ii.hbmMask);
    }
    return true;
}

static bool IsWindowThread(HWND hwnd) {
    return GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId();
}

// Requires g_lock. A new copy, for one window, of the preview the cache holds
// for path right now, or of the folder icon. Null when the preview has been
// replaced or evicted in the meantime: under the lock, a cached preview which
// is still current can't be freed while it is being copied.
static HICON MakeCopyLocked(HICON source, const std::wstring& path) {
    bool current = false;
    if (source && source == g_folderIcon) {
        current = true;
    } else if (source) {
        auto it = g_cache.find(MakeKey(path));
        current = it != g_cache.end() && it->second.icon == source;
    }
    if (!current) {
        return nullptr;
    }
    HICON copy = CopyIcon(source);
    if (copy) {
        g_owned.insert(copy);
    }
    return copy;
}

// Requires g_lock. The window's own copy of source, if it has one which still
// works. One which doesn't was destroyed by someone else, which affects this
// window alone: it stops being used, without being destroyed again, and the
// window gets a new copy.
static HICON ReusableCopyLocked(HWND hwnd, WPARAM type, HICON source) {
    auto it = g_ourIcons.find({hwnd, type});
    if (it == g_ourIcons.end() || it->second.source != source) {
        return nullptr;
    }
    if (IsValidIcon(it->second.copy)) {
        return it->second.copy;
    }
    Wh_Log(L"The icon of %p was destroyed, making a new copy", hwnd);
    g_deadCopies.insert(it->second.copy);
    g_retired.erase(it->second.copy);
    g_ourIcons.erase(it);
    return nullptr;
}

// Requires g_lock. Queues a copy for freeing once nothing needs it, unless it
// is still the window's current copy (that one is retired when replaced) or
// was destroyed by someone else.
static void RetireIfReplacedLocked(HWND hwnd, WPARAM type, HICON icon) {
    auto it = g_ourIcons.find({hwnd, type});
    if (g_deadCopies.count(icon) ||
        (it != g_ourIcons.end() && it->second.copy == icon)) {
        return;
    }
    g_retired.insert(icon);
}

// Requires g_lock. Records the copy now on a window, and retires the one it
// replaced (freed once nothing may still put it back).
static void RecordCopyLocked(HWND hwnd, WPARAM type, HICON copy, HICON source) {
    auto it = g_ourIcons.find({hwnd, type});
    if (it != g_ourIcons.end() && it->second.copy != copy &&
        !g_deadCopies.count(it->second.copy)) {
        g_retired.insert(it->second.copy);
    }
    g_ourIcons[{hwnd, type}] = {copy, source};
}

// Requires g_lock. The window no longer holds its copy of this type.
static void RetireCopyLocked(HWND hwnd, WPARAM type) {
    auto it = g_ourIcons.find({hwnd, type});
    if (it == g_ourIcons.end()) {
        return;
    }
    if (!g_deadCopies.count(it->second.copy)) {
        g_retired.insert(it->second.copy);
    }
    g_ourIcons.erase(it);
}

// Requires g_lock. Drops one record of a copy a pending message involves.
static void EraseUnconfirmedLocked(HWND hwnd, WPARAM type, HICON copy) {
    auto range = g_unconfirmed.equal_range({hwnd, type});
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == copy) {
            g_unconfirmed.erase(it);
            break;
        }
    }
    RetireIfReplacedLocked(hwnd, type, copy);
}

// Requires g_lock. Every pending message for this window and type is settled.
static void SettleUnconfirmedLocked(HWND hwnd, WPARAM type) {
    auto range = g_unconfirmed.equal_range({hwnd, type});
    for (auto it = range.first; it != range.second; ++it) {
        RetireIfReplacedLocked(hwnd, type, it->second);
    }
    g_unconfirmed.erase(range.first, range.second);
}

// Works from any thread: on the window's own thread this is a direct call,
// and the window procedure passes it through while g_inSync is set.
static bool SendSetIcon(HWND hwnd, WPARAM type, HICON icon, HICON* previous) {
    DWORD_PTR r = 0;
    if (!SendMessageTimeoutW(hwnd, WM_SETICON, type,
                             reinterpret_cast<LPARAM>(icon), SMTO_ABORTIFHUNG,
                             kSendTimeout, &r)) {
        return false;
    }
    *previous = reinterpret_cast<HICON>(r);
    return true;
}

// Puts a preview on a window, as the window's own copy of source (a cached
// preview or the folder icon), and remembers Explorer's own icon. A type which
// already shows a working copy of the same preview is left alone.
static void SetOurIcon(HWND hwnd, HICON source, const std::wstring& path) {
    // A send to another thread is delivered after every earlier send from
    // this thread, so once it succeeds, an earlier timed-out one is settled.
    // A direct call on the window's own thread settles nothing.
    const bool crossThread = !IsWindowThread(hwnd);

    for (WPARAM type : {static_cast<WPARAM>(ICON_BIG),
                        static_cast<WPARAM>(ICON_SMALL)}) {
        AcquireSRWLockExclusive(&g_lock);
        if (ReusableCopyLocked(hwnd, type, source)) {
            ReleaseSRWLockExclusive(&g_lock);
            continue;
        }
        const HICON copy = MakeCopyLocked(source, path);
        if (!copy) {
            ReleaseSRWLockExclusive(&g_lock);
            continue;
        }
        // Recorded before sending. While the message is on its way, the
        // window's own thread may set another copy, and a send which times out
        // is still delivered later: so the copy it carries is kept alive until
        // it arrives, and the copy it replaces until the outcome is known.
        auto current = g_ourIcons.find({hwnd, type});
        const HICON before =
            current != g_ourIcons.end() ? current->second.copy : nullptr;
        if (before && !g_deadCopies.count(before)) {
            g_unconfirmed.insert({{hwnd, type}, before});
        }
        g_unconfirmed.insert({{hwnd, type}, copy});
        RecordCopyLocked(hwnd, type, copy, source);
        ReleaseSRWLockExclusive(&g_lock);

        HICON previous = nullptr;
        const bool sent = SendSetIcon(hwnd, type, copy, &previous);

        AcquireSRWLockExclusive(&g_lock);
        if (!sent) {
            if (!IsWindow(hwnd)) {
                // Closed meanwhile: nothing will be delivered.
                RetireCopyLocked(hwnd, type);
                SettleUnconfirmedLocked(hwnd, type);
            }
            // Otherwise still to be delivered: both copies stay alive.
        } else {
            if (crossThread) {
                SettleUnconfirmedLocked(hwnd, type);
            } else {
                if (before && !g_deadCopies.count(before)) {
                    EraseUnconfirmedLocked(hwnd, type, before);
                }
                EraseUnconfirmedLocked(hwnd, type, copy);
            }
            // Neither a copy of the mod's nor a dead handle left by an earlier
            // run of the mod may be taken for Explorer's icon.
            if (previous && !g_owned.count(previous) && IsValidIcon(previous)) {
                g_explorerIcons[{hwnd, type}] = previous;
            }
        }
        ReleaseUnusedLocked();
        ReleaseSRWLockExclusive(&g_lock);
    }

    AcquireSRWLockExclusive(&g_lock);
    g_shownPath[hwnd] = path;
    ReleaseSRWLockExclusive(&g_lock);
}

// Requires g_lock. Explorer's icon for a window, or null when there is none.
static HICON ExplorerIconLocked(HWND hwnd, WPARAM type) {
    auto it = g_explorerIcons.find({hwnd, type});
    return it != g_explorerIcons.end() ? it->second : nullptr;
}

static void CreateFolderIcon() {
    SHSTOCKICONINFO info{};
    info.cbSize = sizeof(info);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICON | SHGSI_LARGEICON,
                                     &info)) &&
        info.hIcon) {
        AcquireSRWLockExclusive(&g_lock);
        g_folderIcon = info.hIcon;
        g_owned.insert(g_folderIcon);
        ReleaseSRWLockExclusive(&g_lock);
    }
}

// Hands a window back to Explorer: whichever of its two icons is still the
// mod's preview is replaced by Explorer's own. A window is never left without
// an icon, which the taskbar would draw as a blank window. If Explorer's icon
// is gone, the window gets a plain folder icon, or, when the mod is shutting
// down, keeps its preview, which is then leaked rather than destroyed.
static void RestoreWindowIcons(HWND hwnd, bool shuttingDown) {
    for (WPARAM type : {static_cast<WPARAM>(ICON_BIG),
                        static_cast<WPARAM>(ICON_SMALL)}) {
        bool ours = false;
        HICON theirs = nullptr;
        AcquireSRWLockExclusive(&g_lock);
        if (g_ourIcons.count({hwnd, type})) {
            ours = true;
            theirs = ExplorerIconLocked(hwnd, type);
        }
        ReleaseSRWLockExclusive(&g_lock);

        if (!ours) {
            continue;
        }

        if (!IsWindow(hwnd)) {
            AcquireSRWLockExclusive(&g_lock);
            RetireCopyLocked(hwnd, type);
            SettleUnconfirmedLocked(hwnd, type);
            ReleaseSRWLockExclusive(&g_lock);
            continue;
        }

        HICON replacement = theirs;
        bool replacementIsOurs = false;
        if (!IsValidIcon(replacement)) {
            if (shuttingDown || !g_folderIcon) {
                Wh_Log(L"No Explorer icon for %p, keeping the preview", hwnd);
                continue;
            }
            AcquireSRWLockExclusive(&g_lock);
            replacement = MakeCopyLocked(g_folderIcon, std::wstring());
            ReleaseSRWLockExclusive(&g_lock);
            if (!replacement) {
                continue;
            }
            replacementIsOurs = true;
        }

        if (replacementIsOurs) {
            // Kept alive while its message is on its way, like any copy.
            AcquireSRWLockExclusive(&g_lock);
            g_unconfirmed.insert({{hwnd, type}, replacement});
            ReleaseSRWLockExclusive(&g_lock);
        }

        HICON previous = nullptr;
        if (!SendSetIcon(hwnd, type, replacement, &previous)) {
            Wh_Log(L"Couldn't restore icon %u of %p, leaking the preview",
                   static_cast<unsigned>(type), hwnd);
            continue;
        }

        AcquireSRWLockExclusive(&g_lock);
        if (replacementIsOurs) {
            RecordCopyLocked(hwnd, type, replacement, g_folderIcon);
            EraseUnconfirmedLocked(hwnd, type, replacement);
        } else {
            RetireCopyLocked(hwnd, type);
        }
        ReleaseSRWLockExclusive(&g_lock);
    }

    AcquireSRWLockExclusive(&g_lock);
    // The folder icon belongs to no folder in particular, so Explorer's next
    // icon for this window goes straight through.
    g_shownPath.erase(hwnd);
    ReleaseUnusedLocked();
    ReleaseSRWLockExclusive(&g_lock);
}

// Explorer's own icon goes through for one of the two types, so the mod no
// longer owns that one.
static void ForgetOurIcon(HWND hwnd, WPARAM type, HICON explorerIcon) {
    AcquireSRWLockExclusive(&g_lock);
    RetireCopyLocked(hwnd, type);  // the window holds Explorer's icon now
    if (explorerIcon && !g_owned.count(explorerIcon)) {
        g_explorerIcons[{hwnd, type}] = explorerIcon;
    }
    if (!g_ourIcons.count({hwnd, ICON_BIG}) &&
        !g_ourIcons.count({hwnd, ICON_SMALL})) {
        g_shownPath.erase(hwnd);
    }
    ReleaseUnusedLocked();
    ReleaseSRWLockExclusive(&g_lock);
}

static void DropWindow(HWND hwnd) {
    AcquireSRWLockExclusive(&g_lock);
    for (WPARAM type : {static_cast<WPARAM>(ICON_BIG),
                        static_cast<WPARAM>(ICON_SMALL)}) {
        RetireCopyLocked(hwnd, type);
        SettleUnconfirmedLocked(hwnd, type);  // nothing reaches a closed window
        g_explorerIcons.erase({hwnd, type});
    }
    g_shownPath.erase(hwnd);
    g_currentPath.erase(hwnd);
    g_subclassed.erase(hwnd);
    g_recheckPending.erase(hwnd);
    g_resolveMemo.erase(hwnd);
    ReleaseUnusedLocked();
    ReleaseSRWLockExclusive(&g_lock);
}

static std::wstring ShownPath(HWND hwnd) {
    AcquireSRWLockShared(&g_lock);
    auto it = g_shownPath.find(hwnd);
    std::wstring path = it != g_shownPath.end() ? it->second : std::wstring();
    ReleaseSRWLockShared(&g_lock);
    return path;
}

// ---------------------------------------------------------------------------
// Finding the folder a window has open

static bool GetBrowserFolder(IShellBrowser* sb, PIDLIST_ABSOLUTE* pidlOut) {
    ComPtr<IShellView> view;
    if (FAILED(sb->QueryActiveShellView(view.put())) || !view) {
        return false;
    }

    ComPtr<IFolderView> fv;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(fv.put())))) {
        return false;
    }

    ComPtr<IPersistFolder2> pf;
    if (FAILED(fv->GetFolder(IID_PPV_ARGS(pf.put())))) {
        return false;
    }

    *pidlOut = nullptr;
    return SUCCEEDED(pf->GetCurFolder(pidlOut)) && *pidlOut;
}

static std::wstring PidlToPath(PCIDLIST_ABSOLUTE pidl) {
    std::wstring path;
    PWSTR name = nullptr;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING,
                                      &name))) {
        path = name;
        CoTaskMemFree(name);
    }
    return path;
}

static bool IsExplorerFrame(HWND hwnd) {
    WCHAR cls[32];
    return hwnd && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, L"CabinetWClass") == 0;
}

// Each way of finding the folder is logged the first time it is used, and so
// is the first failure, which says what works on this Windows build.
static std::atomic<unsigned> g_resolveLogged{0};

static void LogResolveOnce(unsigned bit, PCWSTR what) {
    if (!(g_resolveLogged.fetch_or(bit) & bit)) {
        Wh_Log(L"folder lookup: %s", what);
    }
}

// Asked of the window directly, with no COM marshaling. On Windows 11 the
// browser belongs to the active tab rather than to the frame.
static IShellBrowser* GetShellBrowserSync(HWND hwnd) {
    HWND activeTab =
        FindWindowExW(hwnd, nullptr, L"ShellTabWindowClass", nullptr);
    if (activeTab) {
        auto* sb = reinterpret_cast<IShellBrowser*>(
            SendMessageW(activeTab, kCwmGetIShellBrowser, 0, 0));
        if (sb) {
            LogResolveOnce(1, L"active tab");
            return sb;
        }
    }

    auto* sb = reinterpret_cast<IShellBrowser*>(
        SendMessageW(hwnd, kCwmGetIShellBrowser, 0, 0));
    if (sb) {
        LogResolveOnce(2, L"frame window");
    }
    return sb;
}

// Only the direct lookup runs here. Asking the shell's window list from this
// thread would make COM calls into every other Explorer window's thread, and
// pump messages in the middle of Explorer's own navigation code. A window
// this can't resolve is retried after the navigation step (PostRecheck), and
// then handed to the worker, which uses the window list off the UI thread.
static bool ResolveFolderSync(HWND hwnd, std::wstring* path,
                              PIDLIST_ABSOLUTE* pidl) {
    if (GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) {
        return false;
    }

    *pidl = nullptr;
    IShellBrowser* sb = GetShellBrowserSync(hwnd);
    if (!sb || !GetBrowserFolder(sb, pidl)) {
        return false;
    }

    *path = PidlToPath(*pidl);
    if (path->empty()) {
        ILFree(*pidl);
        *pidl = nullptr;
        return false;
    }
    return true;
}

// The last resort for a window the direct lookup can't resolve.
static void HandToWorker(HWND hwnd) {
    LogResolveOnce(8, L"direct lookup failed, the worker resolves the window");
    if (HWND worker = g_workerWnd.load()) {
        PostMessageW(worker, kMsgCheckWindow, reinterpret_cast<WPARAM>(hwnd), 0);
    }
}

// ---------------------------------------------------------------------------
// The instant path, on Explorer's own window thread

// Hands a folder to the worker for rendering. Takes ownership of pidl, and
// keeps only the newest folder per window.
static void PostRenderJob(HWND hwnd, const std::wstring& path,
                          PIDLIST_ABSOLUTE pidl) {
    HWND worker = g_workerWnd.load();
    if (!worker) {
        ILFree(pidl);
        return;
    }

    AcquireSRWLockExclusive(&g_lock);
    for (auto& job : g_jobs) {
        if (job.hwnd == hwnd) {
            ILFree(job.pidl);
            job.path = path;
            job.pidl = pidl;
            pidl = nullptr;
            break;
        }
    }
    if (pidl) {
        g_jobs.push_back({hwnd, path, pidl});
    }
    ReleaseSRWLockExclusive(&g_lock);

    PostMessageW(worker, kMsgRenderJobs, 0, 0);
}

// An icon for a folder without rendering anything: the mod's caches first,
// then the Windows thumbnail cache.
static HICON GetIconSync(const std::wstring& path, PCIDLIST_ABSOLUTE pidl) {
    HICON icon = GetCachedIcon(path);
    if (icon) {
        return icon;
    }

    std::vector<DWORD> px;
    if (!RenderFolderPixels(pidl, px, /*cacheOnly=*/true)) {
        return nullptr;
    }

    bool needSave = false;
    return StoreRendered(path, px, HashPixels(px), /*byWorker=*/false,
                         &needSave);
}

// Finds the window's folder, hands it to the worker when it needs rendering,
// and returns an icon when one can be had at once. pathOut stays empty when
// the folder could not be determined.
static HICON UpdateSync(HWND hwnd, std::wstring* pathOut) {
    pathOut->clear();
    if (g_inSync) {
        return nullptr;
    }

    const ULONGLONG now = GetTickCount64();

    // The two WM_SETICON messages of one navigation arrive together; the
    // second reuses what the first found.
    AcquireSRWLockShared(&g_lock);
    auto memo = g_resolveMemo.find(hwnd);
    bool fresh = memo != g_resolveMemo.end() &&
                 now - memo->second.tick < kResolveMemo;
    if (fresh) {
        *pathOut = memo->second.path;
    }
    ReleaseSRWLockShared(&g_lock);
    if (fresh) {
        // Looked up again rather than remembered: a cached icon may have been
        // freed in the meantime, and this is only a lookup in a map.
        return GetCachedIcon(*pathOut);
    }

    g_inSync = true;
    HICON icon = nullptr;
    std::wstring path;
    PIDLIST_ABSOLUTE pidl = nullptr;
    const bool resolved = ResolveFolderSync(hwnd, &path, &pidl);
    if (!resolved) {
        // Folder unknown here: the worker's scan decides, unhindered.
        AcquireSRWLockExclusive(&g_lock);
        g_currentPath.erase(hwnd);
        ReleaseSRWLockExclusive(&g_lock);
    } else {
        icon = GetIconSync(path, pidl);
        *pathOut = path;

        AcquireSRWLockExclusive(&g_lock);
        auto current = g_currentPath.find(hwnd);
        const bool moved =
            current == g_currentPath.end() || current->second != path;
        g_currentPath[hwnd] = path;
        g_resolveMemo[hwnd] = {now, path};
        ReleaseSRWLockExclusive(&g_lock);

        // The worker also hears about a move to a folder which is fresh in
        // the cache: it renders nothing then, but moves its change watch.
        if (moved || NeedsRender(path, /*force=*/false)) {
            PostRenderJob(hwnd, path, pidl);  // takes the pidl
        } else {
            ILFree(pidl);
        }
    }
    g_inSync = false;
    return icon;
}

// Applies whatever the instant path found. Called from the window procedure.
// Returns false when the window's folder could not be found.
static bool TryApplySync(HWND hwnd) {
    std::wstring path;
    HICON icon = UpdateSync(hwnd, &path);
    if (path.empty()) {
        return false;
    }

    const std::wstring shown = ShownPath(hwnd);

    g_inSync = true;
    if (icon) {
        SetOurIcon(hwnd, icon, path);  // leaves a type which already shows it
    } else if (!shown.empty() && shown != path) {
        // The preview of the previous folder must not stay on the button.
        RestoreWindowIcons(hwnd, /*shuttingDown=*/false);
    }
    g_inSync = false;
    return true;
}

// ---------------------------------------------------------------------------
// The Explorer window procedure

// While Explorer is still inside a navigation step, its view may report the
// old folder. A message posted to the window is handled right after that step,
// when the new folder is certain.
static void PostRecheck(HWND hwnd) {
    AcquireSRWLockExclusive(&g_lock);
    bool first = g_recheckPending.insert(hwnd).second;
    ReleaseSRWLockExclusive(&g_lock);
    if (first && !PostMessageW(hwnd, g_msgRecheck, 0, 0)) {
        AcquireSRWLockExclusive(&g_lock);
        g_recheckPending.erase(hwnd);
        ReleaseSRWLockExclusive(&g_lock);
    }
}

// Explorer destroys the icon WM_SETICON hands back to it, as the icon it
// replaced. So it must get back the icon it set itself last time, never a
// preview of the mod (shared by every window showing that folder), and never
// the icon it is setting now (which the mod keeps for restoring).

// Records the icon Explorer is setting; returns the one it set last time.
static HICON NoteExplorerIcon(HWND hwnd, WPARAM type, HICON explorerIcon) {
    AcquireSRWLockExclusive(&g_lock);
    HICON explorerPrevious = ExplorerIconLocked(hwnd, type);
    if (explorerIcon && !g_owned.count(explorerIcon)) {
        g_explorerIcons[{hwnd, type}] = explorerIcon;
    }
    ReleaseSRWLockExclusive(&g_lock);
    return explorerPrevious;
}

// Must run before the copy the window gave up is freed: once it is, it no
// longer counts as the mod's, and would be handed to Explorer to free again.
static LRESULT AnswerExplorer(LRESULT r, HICON explorerPrevious,
                              HICON explorerIcon) {
    HICON replaced = reinterpret_cast<HICON>(r);
    if (!IsOwnedIcon(replaced)) {
        return r;  // really Explorer's own previous icon
    }
    return explorerPrevious != explorerIcon
               ? reinterpret_cast<LRESULT>(explorerPrevious)
               : 0;
}

// Lets Explorer's own icon through. Also used while the mod is starting or
// stopping, when nothing else is substituted: a preview is never handed back.
static LRESULT PassExplorerSetIcon(HWND hwnd, WPARAM type, LPARAM lParam) {
    const HICON explorerIcon = reinterpret_cast<HICON>(lParam);
    const HICON explorerPrevious = NoteExplorerIcon(hwnd, type, explorerIcon);
    LRESULT r = DefSubclassProc(hwnd, WM_SETICON, type, lParam);
    // Decided first: ForgetOurIcon may free the copy the window just gave up,
    // and only while it is still counted as the mod's is it never handed back.
    const LRESULT answer = AnswerExplorer(r, explorerPrevious, explorerIcon);
    ForgetOurIcon(hwnd, type, explorerIcon);
    return answer;
}

static LRESULT HandleExplorerSetIcon(HWND hwnd, WPARAM type, LPARAM lParam) {
    std::wstring path;
    HICON source = UpdateSync(hwnd, &path);

    HICON copy = nullptr;
    AcquireSRWLockExclusive(&g_lock);
    if (source) {
        copy = ReusableCopyLocked(hwnd, type, source);
        if (!copy) {
            copy = MakeCopyLocked(source, path);
        }
    }
    if (!copy) {
        // Keep the preview already on the window when it belongs to this same
        // folder; otherwise let Explorer's icon through until one is ready.
        auto shown = g_shownPath.find(hwnd);
        auto it = g_ourIcons.find({hwnd, type});
        if (shown != g_shownPath.end() && !shown->second.empty() &&
            (path.empty() || shown->second == path) &&
            it != g_ourIcons.end() && IsValidIcon(it->second.copy)) {
            copy = it->second.copy;
            source = it->second.source;
            path = shown->second;
        }
    }
    ReleaseSRWLockExclusive(&g_lock);

    if (!copy) {
        return PassExplorerSetIcon(hwnd, type, lParam);
    }

    const HICON explorerIcon = reinterpret_cast<HICON>(lParam);
    const HICON explorerPrevious = NoteExplorerIcon(hwnd, type, explorerIcon);

    LRESULT r = DefSubclassProc(hwnd, WM_SETICON, type,
                                reinterpret_cast<LPARAM>(copy));

    // Decided before the bookkeeping below frees the copy the window gave up:
    // only while that one is still counted as the mod's is it never handed
    // back. Explorer gets its own previous icon instead, which is not on the
    // window (the copy is), so it can free that one safely.
    const LRESULT answer = AnswerExplorer(r, explorerPrevious, explorerIcon);

    // Set directly, so the copy it replaced is off the window now. A send of
    // the worker's which timed out may still arrive; the window procedure
    // drops it, since it no longer carries the current copy.
    AcquireSRWLockExclusive(&g_lock);
    RecordCopyLocked(hwnd, type, copy, source);
    g_shownPath[hwnd] = path;
    ReleaseUnusedLocked();
    ReleaseSRWLockExclusive(&g_lock);

    return answer;
}

// A copy of the mod's arrives in a WM_SETICON which this thread didn't set
// directly: a send from the worker. Its message has arrived, so it no longer
// needs keeping alive for that. One which is no longer the window's current
// copy was overtaken while it waited, and is dropped.
static bool DropStaleDelivery(HWND hwnd, WPARAM type, HICON icon) {
    AcquireSRWLockExclusive(&g_lock);
    auto it = g_ourIcons.find({hwnd, type});
    const bool stale = it == g_ourIcons.end() || it->second.copy != icon;
    EraseUnconfirmedLocked(hwnd, type, icon);  // freed if it is stale
    ReleaseUnusedLocked();
    ReleaseSRWLockExclusive(&g_lock);
    return stale;
}

static LRESULT CALLBACK FrameSubclassProc(HWND hwnd,
                                          UINT msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          DWORD_PTR refData) {
    if (msg == WM_NCDESTROY) {
        DropWindow(hwnd);
        // The worker drops its folder change registration for this window.
        if (HWND worker = g_workerWnd.load()) {
            PostMessageW(worker, kMsgWindowGone, reinterpret_cast<WPARAM>(hwnd),
                         0);
        }
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    // From the worker. wParam 0: the mod is stopping, so the window goes back
    // to Explorer. wParam 1: no scan could tell which folder the window has
    // open, so the preview, which shows the folder it was in before, goes
    // too, unless that folder can be found after all.
    if (msg == g_msgRestoreIcons && msg != 0) {
        const bool stopping = wParam == 0;
        if (stopping || (!g_inSync &&
                         g_enabled.load(std::memory_order_relaxed) &&
                         !TryApplySync(hwnd))) {
            g_inSync = true;
            RestoreWindowIcons(hwnd, stopping);
            g_inSync = false;
        }
        return 0;
    }

    if (msg == g_msgRecheck && msg != 0) {
        AcquireSRWLockExclusive(&g_lock);
        g_recheckPending.erase(hwnd);
        g_resolveMemo.erase(hwnd);  // the navigation has finished by now
        ReleaseSRWLockExclusive(&g_lock);
        if (!g_inSync && g_enabled.load(std::memory_order_relaxed) &&
            !TryApplySync(hwnd)) {
            HandToWorker(hwnd);
        }
        return 0;
    }

    if (g_inSync) {
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    if (!g_enabled.load(std::memory_order_relaxed)) {
        // Starting or stopping: Explorer's icon goes through, and Explorer
        // still never gets a preview back to destroy.
        if (msg == WM_SETICON && (wParam == ICON_BIG || wParam == ICON_SMALL) &&
            !IsOwnedIcon(reinterpret_cast<HICON>(lParam))) {
            return PassExplorerSetIcon(hwnd, wParam, lParam);
        }
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_SETTEXT: {
            // A title change means a navigation or a tab switch.
            LRESULT r = DefSubclassProc(hwnd, msg, wParam, lParam);
            TryApplySync(hwnd);
            PostRecheck(hwnd);
            return r;
        }

        case WM_SETICON:
            if (wParam != ICON_BIG && wParam != ICON_SMALL) {
                break;
            }
            if (IsOwnedIcon(reinterpret_cast<HICON>(lParam))) {
                if (DropStaleDelivery(hwnd, wParam,
                                      reinterpret_cast<HICON>(lParam))) {
                    return 0;
                }
                break;
            }
            {
                LRESULT r = HandleExplorerSetIcon(hwnd, wParam, lParam);
                PostRecheck(hwnd);
                return r;
            }
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static bool IsSubclassed(HWND hwnd) {
    AcquireSRWLockShared(&g_lock);
    bool result = g_subclassed.count(hwnd) != 0;
    ReleaseSRWLockShared(&g_lock);
    return result;
}

// Any thread. From another thread this waits for the window's thread.
static void EnsureSubclassed(HWND hwnd) {
    AcquireSRWLockExclusive(&g_lock);
    bool already = !g_subclassed.insert(hwnd).second;
    ReleaseSRWLockExclusive(&g_lock);
    if (already) {
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, FrameSubclassProc,
                                                       0)) {
        AcquireSRWLockExclusive(&g_lock);
        g_subclassed.erase(hwnd);
        ReleaseSRWLockExclusive(&g_lock);
    }
}

static BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    if (cmd != SW_HIDE && !g_inSync &&
        g_enabled.load(std::memory_order_relaxed) && IsExplorerFrame(hwnd) &&
        !IsWindowVisible(hwnd)) {
        // Before the taskbar button exists: take over the window's icon
        // changes and put the preview on it straight away.
        EnsureSubclassed(hwnd);
        if (!TryApplySync(hwnd)) {
            PostRecheck(hwnd);
        }
    }
    return ShowWindow_Original(hwnd, cmd);
}

// ---------------------------------------------------------------------------
// Worker thread

static void Schedule(HWND hwnd, DWORD delay, bool forceRender, int confirmStep) {
    ULONGLONG due = GetTickCount64() + delay;
    auto it = g_pending.find(hwnd);
    if (it == g_pending.end()) {
        g_pending[hwnd] = {due, forceRender, confirmStep};
        return;
    }
    Pending& p = it->second;
    p.due = std::min(p.due, due);
    p.forceRender = p.forceRender || forceRender;
    p.confirmStep = std::min(p.confirmStep, confirmStep);
}

static void RegisterFolderNotify(HWND hwnd, WindowState& st,
                                 PCIDLIST_ABSOLUTE pidl) {
    if (st.notifyId) {
        SHChangeNotifyDeregister(st.notifyId);
        st.notifyId = 0;
    }
    if (!g_workerWndOwned) {
        return;
    }

    if (!st.notifyMsg) {
        st.notifyMsg = g_nextNotifyMsg++;
        if (g_nextNotifyMsg > kNotifyMsgLast) {
            g_nextNotifyMsg = kNotifyMsgFirst;
        }
        g_notifyMsgToHwnd[st.notifyMsg] = hwnd;
    }

    SHChangeNotifyEntry entry{};
    entry.pidl = pidl;
    entry.fRecursive = FALSE;
    st.notifyId = SHChangeNotifyRegister(
        g_workerWndOwned,
        SHCNRF_InterruptLevel | SHCNRF_ShellLevel | SHCNRF_NewDelivery,
        kNotifyEvents, st.notifyMsg, 1, &entry);
}

static void ForgetWindowState(HWND hwnd) {
    // Also for a window which was scheduled but never resolved, and so has
    // no entry in g_windows.
    g_pending.erase(hwnd);

    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) {
        return;
    }
    if (it->second.notifyId) {
        SHChangeNotifyDeregister(it->second.notifyId);
    }
    if (it->second.notifyMsg) {
        g_notifyMsgToHwnd.erase(it->second.notifyMsg);
    }
    g_windows.erase(it);
}

// False when the window has closed, or its own thread has since found it in
// another folder (whose preview then has its own job). Only the window's
// thread records the folder; when it can't tell, a scan's result stands.
static bool StillShows(const FoundWindow& f) {
    if (!IsWindow(f.hwnd)) {
        return false;
    }
    AcquireSRWLockShared(&g_lock);
    auto it = g_currentPath.find(f.hwnd);
    bool same = it == g_currentPath.end() || it->second == f.path;
    ReleaseSRWLockShared(&g_lock);
    return same;
}

static void UpdateExplorerWindow(const FoundWindow& f, bool forceRender) {
    if (!StillShows(f)) {
        return;  // closed, or the window's newer folder has its own job
    }

    // Covers a window which was already open when the mod started.
    EnsureSubclassed(f.hwnd);

    WindowState& st = g_windows[f.hwnd];
    const bool pathChanged = !st.known || st.path != f.path;
    if (pathChanged) {
        st.path = f.path;
        st.known = true;
        RegisterFolderNotify(f.hwnd, st, f.pidl);
    }

    const std::wstring shown = ShownPath(f.hwnd);
    if (pathChanged && shown != f.path) {
        HICON cached = GetCachedIcon(f.path);
        if (cached && StillShows(f)) {
            SetOurIcon(f.hwnd, cached, f.path);
        }
    }

    if (!NeedsRender(f.path, forceRender)) {
        return;
    }

    std::vector<DWORD> px;
    const ULONGLONG start = GetTickCount64();
    if (!RenderFolderPixels(f.pidl, px, /*cacheOnly=*/false)) {
        Wh_Log(L"render failed | %s", f.path.c_str());
        return;
    }

    bool needSave = false;
    HICON rendered = StoreRendered(f.path, px, HashPixels(px),
                                   /*byWorker=*/true, &needSave);
    if (needSave) {
        SaveToDisk(f.path, px);
    }
    Wh_Log(L"rendered in %llu ms | %s", GetTickCount64() - start,
           f.path.c_str());

    // The render took a while: the window may have gone to another folder,
    // whose preview must not be replaced by this one.
    if (!rendered || !StillShows(f)) {
        return;
    }

    SetOurIcon(f.hwnd, rendered, f.path);  // leaves a type which already shows it
}

static bool GetActiveFolder(IWebBrowser2* wb, HWND top,
                            PIDLIST_ABSOLUTE* pidlOut) {
    ComPtr<IServiceProvider> sp;
    if (FAILED(wb->QueryInterface(IID_PPV_ARGS(sp.put())))) {
        return false;
    }

    ComPtr<IShellBrowser> sb;
    if (FAILED(sp->QueryService(kSID_STopLevelBrowser,
                                IID_PPV_ARGS(sb.put())))) {
        return false;
    }

    // With Windows 11 tabs, the active tab is the first ShellTabWindowClass
    // child of the frame.
    HWND activeTab = FindWindowExW(top, nullptr, L"ShellTabWindowClass", nullptr);
    if (activeTab) {
        HWND tab = nullptr;
        if (SUCCEEDED(sb->GetWindow(&tab)) && tab && tab != top &&
            tab != activeTab) {
            return false;
        }
    }

    return GetBrowserFolder(sb.get(), pidlOut);
}

// Walks the shell's window list. Used once at startup, and for a window the
// instant path couldn't resolve. Returns the windows whose folder it found.
static std::set<HWND> Scan(const std::map<HWND, bool>* only) {
    std::set<HWND> resolved;

    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put())))) {
        return resolved;
    }

    long count = 0;
    if (FAILED(shellWindows->get_Count(&count))) {
        return resolved;
    }

    const DWORD myPid = GetCurrentProcessId();
    std::vector<FoundWindow> found;

    for (long i = 0; i < count; i++) {
        VARIANT idx;
        VariantInit(&idx);
        idx.vt = VT_I4;
        idx.lVal = i;

        ComPtr<IDispatch> disp;
        if (shellWindows->Item(idx, disp.put()) != S_OK || !disp) {
            continue;
        }

        ComPtr<IWebBrowser2> wb;
        if (FAILED(disp->QueryInterface(IID_PPV_ARGS(wb.put())))) {
            continue;
        }

        SHANDLE_PTR h = 0;
        if (FAILED(wb->get_HWND(&h)) || !h) {
            continue;
        }

        HWND top = reinterpret_cast<HWND>(h);
        if (only && only->find(top) == only->end()) {
            continue;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(top, &pid);
        if (pid != myPid || !IsWindow(top)) {
            continue;  // another Explorer process looks after its own windows
        }

        bool duplicate = false;
        for (const auto& f : found) {
            if (f.hwnd == top) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            continue;
        }

        PIDLIST_ABSOLUTE pidl = nullptr;
        if (!GetActiveFolder(wb.get(), top, &pidl)) {
            continue;
        }

        found.push_back({top, pidl, PidlToPath(pidl)});
    }

    for (const auto& f : found) {
        bool force = false;
        if (only) {
            auto it = only->find(f.hwnd);
            force = it != only->end() && it->second;
        }
        if (!f.path.empty()) {
            resolved.insert(f.hwnd);
            UpdateExplorerWindow(f, force);
        }
        ILFree(f.pidl);
    }
    return resolved;
}

// Folders handed over by Explorer threads, rendered without asking Explorer
// anything again.
static void ProcessRenderJobs() {
    g_jobsPosted = false;

    std::vector<RenderJob> jobs;
    AcquireSRWLockExclusive(&g_lock);
    jobs.swap(g_jobs);
    ReleaseSRWLockExclusive(&g_lock);

    for (auto& job : jobs) {
        if (IsWindow(job.hwnd)) {
            UpdateExplorerWindow({job.hwnd, job.pidl, job.path}, false);
        }
        ILFree(job.pidl);
    }
}

static LRESULT CALLBACK WorkerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) {
    if (msg == kMsgRenderJobs) {
        g_jobsPosted = true;  // handled by the loop, not re-entrantly
        return 0;
    }

    if (msg == kMsgWindowGone) {
        ForgetWindowState(reinterpret_cast<HWND>(wParam));
        return 0;
    }

    if (msg == kMsgCheckWindow) {
        HWND hwnd = reinterpret_cast<HWND>(wParam);
        if (IsWindow(hwnd)) {
            Schedule(hwnd, 0, false, 0);
        }
        return 0;
    }

    auto it = g_notifyMsgToHwnd.find(msg);
    if (it != g_notifyMsgToHwnd.end()) {
        PIDLIST_ABSOLUTE* pidls = nullptr;
        LONG event = 0;
        HANDLE lock = SHChangeNotification_Lock(reinterpret_cast<HANDLE>(wParam),
                                                static_cast<DWORD>(lParam),
                                                &pidls, &event);
        if (lock) {
            SHChangeNotification_Unlock(lock);
        }
        auto p = g_pending.find(it->second);
        if (p == g_pending.end()) {
            Schedule(it->second, kContentDelay, true, kNoConfirms);
        } else {
            p->second.forceRender = true;
        }
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// The safety net for a window which was shown in some way the hook misses.
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                  LONG idObject, LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (IsExplorerFrame(hwnd) && !IsSubclassed(hwnd)) {
        Schedule(hwnd, kConfirmDelays[0], false, 0);
    }
}

// A window which shows a preview, while its own thread could not determine
// its folder the last time it tried.
static bool ShowsUnknownFolder(HWND hwnd) {
    AcquireSRWLockShared(&g_lock);
    auto shown = g_shownPath.find(hwnd);
    const bool result = shown != g_shownPath.end() && !shown->second.empty() &&
                        !g_currentPath.count(hwnd);
    ReleaseSRWLockShared(&g_lock);
    return result;
}

static void ProcessDue() {
    if (g_jobsPosted) {
        ProcessRenderJobs();
    }

    const ULONGLONG now = GetTickCount64();

    std::map<HWND, bool> due;
    std::map<HWND, int> steps;
    for (auto it = g_pending.begin(); it != g_pending.end();) {
        if (now < it->second.due) {
            ++it;
            continue;
        }
        if (!IsWindow(it->first)) {
            HWND gone = it->first;
            it = g_pending.erase(it);
            ForgetWindowState(gone);
            continue;
        }
        due[it->first] = it->second.forceRender;
        steps[it->first] = it->second.confirmStep;
        it = g_pending.erase(it);
    }

    if (due.empty()) {
        return;
    }

    const std::set<HWND> resolved = Scan(&due);

    // The follow-up scans run even when this one found a folder: while a
    // navigation is still in progress, the window list can report the folder
    // being left, and only a later scan sees the new one.
    for (const auto& [hwnd, step] : steps) {
        if (!IsWindow(hwnd)) {
            continue;
        }
        if (step < kNoConfirms) {
            Schedule(hwnd, kConfirmDelays[step], false, step + 1);
        } else if (!resolved.count(hwnd) && ShowsUnknownFolder(hwnd)) {
            // The last scan, and neither it nor the window's own thread can
            // tell which folder the window has open (a view without a folder).
            // Its preview shows the folder it was in before.
            PostMessageW(hwnd, g_msgRestoreIcons, 1, 0);
        }
    }
}

static DWORD NextTimeout() {
    if (g_jobsPosted) {
        return 0;
    }
    ULONGLONG next = ULLONG_MAX;
    for (const auto& [hwnd, p] : g_pending) {
        next = std::min(next, p.due);
    }
    if (next == ULLONG_MAX) {
        return INFINITE;  // nothing to do until an event arrives
    }
    ULONGLONG now = GetTickCount64();
    return next > now ? static_cast<DWORD>(next - now) : 0;
}

// Hands every window back to Explorer, on the window's own thread where
// possible, so that no window is left holding an icon about to be freed.
static void RestoreEverything() {
    std::set<HWND> hwnds;
    AcquireSRWLockShared(&g_lock);
    for (const auto& [key, icon] : g_ourIcons) {
        hwnds.insert(key.first);
    }
    ReleaseSRWLockShared(&g_lock);

    for (HWND hwnd : hwnds) {
        if (!IsWindow(hwnd)) {
            DropWindow(hwnd);
            continue;
        }
        if (IsSubclassed(hwnd) && g_msgRestoreIcons) {
            if (SendMessageTimeoutW(hwnd, g_msgRestoreIcons, 0, 0,
                                    SMTO_ABORTIFHUNG, kSendTimeout, nullptr)) {
                // Delivered after every earlier send to the window, so
                // nothing pending can put an old copy back any more.
                AcquireSRWLockExclusive(&g_lock);
                SettleUnconfirmedLocked(hwnd, ICON_BIG);
                SettleUnconfirmedLocked(hwnd, ICON_SMALL);
                ReleaseUnusedLocked();
                ReleaseSRWLockExclusive(&g_lock);
            }
        } else {
            RestoreWindowIcons(hwnd, /*shuttingDown=*/true);
        }
    }
}

static void RemoveAllSubclasses() {
    std::vector<HWND> hwnds;
    AcquireSRWLockExclusive(&g_lock);
    hwnds.assign(g_subclassed.begin(), g_subclassed.end());
    g_subclassed.clear();
    ReleaseSRWLockExclusive(&g_lock);

    for (HWND hwnd : hwnds) {
        if (IsWindow(hwnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd,
                                                             FrameSubclassProc);
        }
    }
}

static DWORD WINAPI WorkerThread(LPVOID) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        return 0;
    }

    // The class belongs to the module which contains its window procedure.
    HINSTANCE inst = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WorkerWndProc), &inst);
    WNDCLASSW wc{};
    wc.lpfnWndProc = WorkerWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = L"WhFolderThumbTaskbarWorker";
    ATOM cls = RegisterClassW(&wc);
    if (cls) {
        g_workerWndOwned = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0,
                                           0, HWND_MESSAGE, nullptr, inst,
                                           nullptr);
        g_workerWnd = g_workerWndOwned;
    }

    const DWORD pid = GetCurrentProcessId();
    HWINEVENTHOOK showHook = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, WinEventProc, pid, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);

    Scan(nullptr);  // windows which were open before the mod started

    for (;;) {
        ProcessDue();

        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                            NextTimeout(), QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) {
            break;
        }
        if (r == WAIT_OBJECT_0 + 1) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    if (showHook) {
        UnhookWinEvent(showHook);
    }

    g_workerWnd = nullptr;
    RestoreEverything();
    RemoveAllSubclasses();

    for (auto& [hwnd, st] : g_windows) {
        if (st.notifyId) {
            SHChangeNotifyDeregister(st.notifyId);
        }
    }
    g_windows.clear();

    // Whatever no window holds any more can go. An icon still recorded in
    // g_ourIcons is on a window which couldn't be reached, and is left alone.
    AcquireSRWLockExclusive(&g_lock);
    for (auto& job : g_jobs) {
        ILFree(job.pidl);
    }
    g_jobs.clear();
    g_cache.clear();
    g_retired.clear();
    g_resolveMemo.clear();

    const std::set<HICON> inUse = IconsInUseLocked();
    size_t leaked = 0;
    for (HICON icon : g_owned) {
        if (inUse.count(icon)) {
            leaked++;
        } else if (!g_deadCopies.count(icon)) {
            DestroyIcon(icon);
        }
    }
    g_owned.clear();
    g_deadCopies.clear();
    g_folderIcon = nullptr;
    g_ourIcons.clear();
    g_unconfirmed.clear();
    g_explorerIcons.clear();
    g_shownPath.clear();
    g_currentPath.clear();
    ReleaseSRWLockExclusive(&g_lock);

    if (leaked) {
        Wh_Log(L"%zu icons left alive, windows still show them", leaked);
    }

    if (g_workerWndOwned) {
        DestroyWindow(g_workerWndOwned);
        g_workerWndOwned = nullptr;
    }
    if (cls) {
        UnregisterClassW(wc.lpszClassName, inst);
    }
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    InitCacheDir();

    // A preview is rendered a little larger than the taskbar needs, so that
    // it stays sharp at every DPI. SM_CXICON is 32 at 100% and 64 at 200%.
    g_iconSize = std::clamp(GetSystemMetrics(SM_CXICON) * 2, 64, 128);
    Wh_Log(L"preview size %d px", g_iconSize);

    g_msgRestoreIcons =
        RegisterWindowMessageW(L"WhFolderThumbTaskbar_RestoreIcons");
    g_msgRecheck = RegisterWindowMessageW(L"WhFolderThumbTaskbar_Recheck");

    CreateFolderIcon();

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    // Enabled before the worker starts: its startup scan already puts
    // previews on open windows, and the window procedure must then never
    // hand one of them back to Explorer.
    g_enabled = true;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        g_enabled = false;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_enabled = false;  // the window procedure passes everything through

    if (g_thread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
