// ==WindhawkMod==
// @id              explorer-taskbar-folder-thumbnails
// @name            Folder thumbnails on taskbar buttons
// @description     Each File Explorer taskbar button shows a preview of the folder it has open, the way that folder looks in Explorer's own icon view
// @version         1.0
// @author          Issuch
// @github          https://github.com/Issuch
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-3.0-only
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -lgdi32 -lversion -lcomctl32 -ladvapi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The code which makes the taskbar draw window icons is based on the
// "Use window icons" option of the "Disable grouping on the taskbar" mod by
// m417z, used under the terms of the GPL-3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-grouping.wh.cpp

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
and a window being closed. A folder which has not changed is never re-rendered.

## Window icons on the taskbar
The taskbar normally draws the application's icon on a button rather than the
icon of the window, which would hide the preview. The mod makes it use the
window icon instead. That can be limited to File Explorer windows, which is the
default, or applied to every program.

## Requirements
* **Ungrouped taskbar buttons.** While several Explorer windows share one
  button, that button shows the Explorer icon. On Windows 10, set Taskbar
  settings -> "Combine taskbar buttons" to "Never"; on Windows 11, a mod which
  disables taskbar grouping has the same effect.
* **Thumbnails enabled.** In File Explorer, under Options -> View, the checkbox
  "Always show icons, never thumbnails" must be unchecked.
* Windows 10 or Windows 11, 64-bit, with the standard taskbar, not
  ExplorerPatcher. Windhawk downloads the taskbar symbols the first time the
  mod is enabled, which can take a while.

Everything else takes care of itself. The preview resolution follows the
screen's DPI, previews are kept on disk so that they are instant after a
reboot, and the old ones are discarded once there are too many of them.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarWindowIcons: explorer
  $name: Window icons on the taskbar
  $description: >-
    The taskbar draws the icon of the window instead of the icon of the
    application, which is what makes the folder previews visible. A new value
    takes effect on a button the next time its icon is updated, so reopening
    the windows is the quickest way to apply it everywhere.
  $options:
  - explorer: File Explorer windows only
  - all: All programs
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <sddl.h>

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

enum class WindowIconsMode { Explorer = 0, All = 1 };

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
// Replaced previews, waiting until no window shows them any more.
static std::vector<HICON> g_retired;

// What the mod has set on each window. An icon named here is on a window and
// must never be destroyed, which is what keeps icons from vanishing.
static std::map<IconKey, HICON> g_ourIcons;
// Explorer's own icon for each window, to put back when the mod lets go.
static std::map<IconKey, HICON> g_explorerIcons;
// The folder whose preview a window currently shows.
static std::map<HWND, std::wstring> g_shownPath;
// Explorer windows whose window procedure the mod has taken over.
static std::set<HWND> g_subclassed;
static std::set<HWND> g_recheckPending;
static std::map<HWND, ResolveMemo> g_resolveMemo;
static std::vector<RenderJob> g_jobs;

static std::atomic<bool> g_enabled{false};
static std::atomic<int> g_windowIconsMode{
    static_cast<int>(WindowIconsMode::Explorer)};
static std::atomic<HWND> g_workerWnd{nullptr};
static std::atomic<bool> g_taskbarHooksReady{false};

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
static bool g_startupScanDone = false;

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

// The cache lives in the mod's own Windhawk storage, which Windhawk removes
// together with the mod. That storage is shared by every account on the PC,
// while a preview shows what is inside one user's folders, so each account
// gets its own subfolder which only that account (and SYSTEM) can open.
// Without a usable folder the disk cache is simply off.
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

    const std::wstring sddl = L"D:P(A;OICI;FA;;;" + sid + L")(A;OICI;FA;;;SY)";
    PSECURITY_DESCRIPTOR sd = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &sd, nullptr)) {
        return;
    }

    const std::wstring dir = std::wstring(storage) + L"\\" + sid;
    SECURITY_ATTRIBUTES sa{sizeof(sa), sd, FALSE};
    bool ok = CreateDirectoryW(dir.c_str(), &sa);
    if (!ok && GetLastError() == ERROR_ALREADY_EXISTS) {
        // Make sure the folder still admits this account alone.
        ok = SetFileSecurityW(dir.c_str(), DACL_SECURITY_INFORMATION, sd);
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
// The one rule which keeps icons from disappearing: an icon named in
// g_ourIcons is set on a window, so it is never destroyed. An icon the mod
// can't prove is unused is leaked instead, and the process frees it on exit.

static std::wstring MakeKey(const std::wstring& path) {
    return std::to_wstring(g_iconSize) + L"|" + NormalizePath(path);
}

static bool IsValidIcon(HICON icon);

// Requires g_lock.
static std::set<HICON> IconsInUseLocked() {
    std::set<HICON> inUse;
    for (const auto& [key, icon] : g_ourIcons) {
        inUse.insert(icon);
    }
    return inUse;
}

// Requires g_lock. Frees what no window shows and the cache no longer needs.
static void ReleaseUnusedLocked() {
    const std::set<HICON> inUse = IconsInUseLocked();

    for (auto it = g_retired.begin(); it != g_retired.end();) {
        if (inUse.count(*it)) {
            ++it;
            continue;
        }
        g_owned.erase(*it);
        DestroyIcon(*it);
        it = g_retired.erase(it);
    }

    while (g_cache.size() > kMemCacheMax) {
        auto victim = g_cache.end();
        for (auto it = g_cache.begin(); it != g_cache.end(); ++it) {
            if (inUse.count(it->second.icon)) {
                continue;
            }
            if (victim == g_cache.end() ||
                it->second.lastUse < victim->second.lastUse) {
                victim = it;
            }
        }
        if (victim == g_cache.end()) {
            break;  // every preview is on a window right now
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
        if (IsValidIcon(cached)) {
            return cached;
        }
        // Destroyed by someone else. Never hand it out again: drop it, without
        // destroying it a second time, and rebuild it from the disk copy.
        Wh_Log(L"cached preview was destroyed, rebuilding | %s", path.c_str());
        AcquireSRWLockExclusive(&g_lock);
        auto stale = g_cache.find(key);
        if (stale != g_cache.end() && stale->second.icon == cached) {
            g_cache.erase(stale);
        }
        g_owned.erase(cached);
        ReleaseSRWLockExclusive(&g_lock);
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
        g_retired.push_back(it->second.icon);  // freed once no window shows it
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

// Puts the mod's preview on a window and remembers Explorer's own icon.
static void SetOurIcon(HWND hwnd, HICON icon, const std::wstring& path) {
    for (WPARAM type : {static_cast<WPARAM>(ICON_BIG),
                        static_cast<WPARAM>(ICON_SMALL)}) {
        HICON previous = nullptr;
        if (!SendSetIcon(hwnd, type, icon, &previous)) {
            continue;  // the icon stays recorded as ours, so it isn't freed
        }

        // A dead handle left by an earlier run of the mod must not be taken
        // for Explorer's icon, or restoring would put it back.
        const bool explorers = previous && !IsOwnedIcon(previous) &&
                               IsValidIcon(previous);

        AcquireSRWLockExclusive(&g_lock);
        g_ourIcons[{hwnd, type}] = icon;
        if (explorers) {
            g_explorerIcons[{hwnd, type}] = previous;
        }
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

// A plain folder icon of the mod's own, for a window whose own Explorer icon
// isn't known. Created once; freed at the end only if no window shows it.
static HICON g_folderIcon = nullptr;

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
        HICON ours = nullptr;
        HICON theirs = nullptr;
        AcquireSRWLockExclusive(&g_lock);
        auto it = g_ourIcons.find({hwnd, type});
        if (it != g_ourIcons.end()) {
            ours = it->second;
            theirs = ExplorerIconLocked(hwnd, type);
        }
        ReleaseSRWLockExclusive(&g_lock);

        if (!ours) {
            continue;
        }

        if (!IsWindow(hwnd)) {
            AcquireSRWLockExclusive(&g_lock);
            g_ourIcons.erase({hwnd, type});
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
            replacement = g_folderIcon;
            replacementIsOurs = true;
        }

        HICON previous = nullptr;
        if (!SendSetIcon(hwnd, type, replacement, &previous)) {
            Wh_Log(L"Couldn't restore icon %u of %p, leaking the preview",
                   static_cast<unsigned>(type), hwnd);
            continue;
        }

        AcquireSRWLockExclusive(&g_lock);
        if (replacementIsOurs) {
            g_ourIcons[{hwnd, type}] = replacement;  // protected while shown
        } else {
            g_ourIcons.erase({hwnd, type});
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
    g_ourIcons.erase({hwnd, type});
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
    g_ourIcons.erase({hwnd, ICON_BIG});
    g_ourIcons.erase({hwnd, ICON_SMALL});
    g_explorerIcons.erase({hwnd, ICON_BIG});
    g_explorerIcons.erase({hwnd, ICON_SMALL});
    g_shownPath.erase(hwnd);
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
    if (ResolveFolderSync(hwnd, &path, &pidl)) {
        icon = GetIconSync(path, pidl);
        if (NeedsRender(path, /*force=*/false)) {
            PostRenderJob(hwnd, path, pidl);  // takes the pidl
        } else {
            ILFree(pidl);
        }
        *pathOut = path;

        AcquireSRWLockExclusive(&g_lock);
        g_resolveMemo[hwnd] = {now, path};
        ReleaseSRWLockExclusive(&g_lock);
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
        AcquireSRWLockShared(&g_lock);
        auto it = g_ourIcons.find({hwnd, ICON_BIG});
        bool same = it != g_ourIcons.end() && it->second == icon;
        ReleaseSRWLockShared(&g_lock);
        if (!same) {
            SetOurIcon(hwnd, icon, path);
        }
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

static LRESULT HandleExplorerSetIcon(HWND hwnd, WPARAM type, LPARAM lParam) {
    std::wstring path;
    HICON ours = UpdateSync(hwnd, &path);

    if (!ours) {
        // Keep the preview already on the window when it belongs to this same
        // folder; otherwise let Explorer's icon through until one is ready.
        const std::wstring shown = ShownPath(hwnd);
        if (!shown.empty() && (path.empty() || shown == path)) {
            AcquireSRWLockShared(&g_lock);
            auto it = g_ourIcons.find({hwnd, type});
            if (it != g_ourIcons.end()) {
                ours = it->second;
            }
            ReleaseSRWLockShared(&g_lock);
            path = shown;
        }
    }

    // Explorer destroys the icon WM_SETICON hands back to it, as the icon it
    // replaced. So it must get back the icon it set itself last time, never
    // a preview of the mod (shared by every window showing that folder), and
    // never the icon it is setting now (which the mod keeps for restoring).
    const HICON explorerIcon = reinterpret_cast<HICON>(lParam);
    HICON explorerPrevious = nullptr;
    AcquireSRWLockExclusive(&g_lock);
    explorerPrevious = ExplorerIconLocked(hwnd, type);
    if (explorerIcon && !g_owned.count(explorerIcon)) {
        g_explorerIcons[{hwnd, type}] = explorerIcon;
    }
    ReleaseSRWLockExclusive(&g_lock);

    auto answerExplorer = [&](LRESULT r) -> LRESULT {
        HICON replaced = reinterpret_cast<HICON>(r);
        if (!IsOwnedIcon(replaced)) {
            return r;  // really Explorer's own previous icon
        }
        return explorerPrevious != explorerIcon
                   ? reinterpret_cast<LRESULT>(explorerPrevious)
                   : 0;
    };

    if (!ours) {
        LRESULT r = DefSubclassProc(hwnd, WM_SETICON, type, lParam);
        ForgetOurIcon(hwnd, type, explorerIcon);
        return answerExplorer(r);
    }

    LRESULT r = DefSubclassProc(hwnd, WM_SETICON, type,
                                reinterpret_cast<LPARAM>(ours));

    AcquireSRWLockExclusive(&g_lock);
    g_ourIcons[{hwnd, type}] = ours;
    g_shownPath[hwnd] = path;
    ReleaseSRWLockExclusive(&g_lock);

    // The window still holds the preview, not Explorer's previous icon, so
    // handing that one back is safe: Explorer frees what it no longer uses.
    return answerExplorer(r);
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

    // Sent by the worker to hand the window back to Explorer.
    if (msg == g_msgRestoreIcons && msg != 0) {
        g_inSync = true;
        RestoreWindowIcons(hwnd, /*shuttingDown=*/true);
        g_inSync = false;
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

    if (g_inSync || !g_enabled.load(std::memory_order_relaxed)) {
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
            if ((wParam == ICON_BIG || wParam == ICON_SMALL) &&
                !IsOwnedIcon(reinterpret_cast<HICON>(lParam))) {
                LRESULT r = HandleExplorerSetIcon(hwnd, wParam, lParam);
                PostRecheck(hwnd);
                return r;
            }
            break;
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
// Taskbar: draw window icons instead of application icons
//
// While the taskbar works out a button's icon, CTaskGroup::GetShortcutIDList
// returns null for that button's group, so the taskbar falls back to the icon
// of the window, which is the preview this mod has set. Same approach as the
// "Use window icons" option of the "Disable grouping on the taskbar" mod.

// The group whose icon is being worked out on this thread.
static thread_local PVOID g_iconTaskGroup = nullptr;

using CTaskGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
using CTaskGroup_GetAppID_t = PCWSTR(WINAPI*)(PVOID pThis);
using CTaskGroup_IsImmersiveGroup_t = bool(WINAPI*)(PVOID pThis);
using CTaskGroup_GetFlags_t = DWORD(WINAPI*)(PVOID pThis);
using CTaskGroup_GetShortcutIDList_t = const ITEMIDLIST*(WINAPI*)(PVOID pThis);
using CTaskBtnGroup_GetGroup_t = PVOID(WINAPI*)(PVOID pThis);
using CTaskBtnGroup_GetIcon_t = HRESULT(WINAPI*)(PVOID pThis,
                                                 PVOID taskItem,
                                                 HICON* icon);
using CTaskBtnGroup__DrawRegularButton_t = void(WINAPI*)(PVOID pThis,
                                                         PVOID hdc,
                                                         PVOID renderInfo);
using CTaskListWnd_GroupChanged_t = void(WINAPI*)(PVOID pThis,
                                                  PVOID taskGroup,
                                                  int property);
using CTaskListWnd_TaskDestroyed_t = LONG_PTR(WINAPI*)(PVOID pThis,
                                                       PVOID taskGroup,
                                                       PVOID taskItem,
                                                       int flags);
using CTaskListWnd_TaskDestroyed_2_t = LONG_PTR(WINAPI*)(PVOID pThis,
                                                         PVOID taskGroup,
                                                         PVOID taskItem);

static CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems_Original;
static CTaskGroup_GetAppID_t CTaskGroup_GetAppID_Original;
static CTaskGroup_IsImmersiveGroup_t CTaskGroup_IsImmersiveGroup_Original;
static CTaskGroup_GetFlags_t CTaskGroup_GetFlags_Original;
static CTaskGroup_GetShortcutIDList_t CTaskGroup_GetShortcutIDList_Original;
static CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;
static CTaskBtnGroup_GetIcon_t CTaskBtnGroup_GetIcon_Original;
static CTaskBtnGroup__DrawRegularButton_t
    CTaskBtnGroup__DrawRegularButton_Original;
static CTaskListWnd_GroupChanged_t CTaskListWnd_GroupChanged_Original;
static CTaskListWnd_TaskDestroyed_t CTaskListWnd_TaskDestroyed_Original;
static CTaskListWnd_TaskDestroyed_2_t CTaskListWnd_TaskDestroyed_2_Original;

static bool IsExplorerAppId(PCWSTR appId) {
    static const WCHAR kExplorerAppId[] = L"Microsoft.Windows.Explorer";
    const size_t len = ARRAYSIZE(kExplorerAppId) - 1;
    // A mod which disables grouping appends a "~Wh~..." suffix to app IDs.
    return appId && _wcsnicmp(appId, kExplorerAppId, len) == 0 &&
           (appId[len] == L'\0' || appId[len] == L'~');
}

static bool TaskGroupWantsWindowIcon(PVOID taskGroup) {
    if (!taskGroup || !g_taskbarHooksReady.load() ||
        !g_enabled.load(std::memory_order_relaxed)) {
        return false;
    }

    // A store app has no window icon worth showing.
    if (CTaskGroup_IsImmersiveGroup_Original(taskGroup)) {
        return false;
    }

    if (static_cast<WindowIconsMode>(g_windowIconsMode.load()) ==
        WindowIconsMode::All) {
        return true;
    }

    return IsExplorerAppId(CTaskGroup_GetAppID_Original(taskGroup));
}

static const ITEMIDLIST* WINAPI CTaskGroup_GetShortcutIDList_Hook(PVOID pThis) {
    // The decision follows the button's group rather than pThis, because a
    // grouping mod may forward this call to a different group.
    PVOID group = g_iconTaskGroup;
    if (group && g_taskbarHooksReady.load() &&
        CTaskGroup_GetNumItems_Original(group) > 0 &&
        TaskGroupWantsWindowIcon(group)) {
        return nullptr;
    }

    return CTaskGroup_GetShortcutIDList_Original(pThis);
}

// Windows 11.
static HRESULT WINAPI CTaskBtnGroup_GetIcon_Hook(PVOID pThis,
                                                 PVOID taskItem,
                                                 HICON* icon) {
    PVOID prev = g_iconTaskGroup;
    g_iconTaskGroup = g_taskbarHooksReady.load()
                          ? CTaskBtnGroup_GetGroup_Original(pThis)
                          : nullptr;
    HRESULT ret = CTaskBtnGroup_GetIcon_Original(pThis, taskItem, icon);
    g_iconTaskGroup = prev;
    return ret;
}

// Windows 10, where the icon is worked out while the button is drawn.
static void WINAPI CTaskBtnGroup__DrawRegularButton_Hook(PVOID pThis,
                                                         PVOID hdc,
                                                         PVOID renderInfo) {
    PVOID prev = g_iconTaskGroup;
    g_iconTaskGroup = g_taskbarHooksReady.load()
                          ? CTaskBtnGroup_GetGroup_Original(pThis)
                          : nullptr;
    CTaskBtnGroup__DrawRegularButton_Original(pThis, hdc, renderInfo);
    g_iconTaskGroup = prev;
}

// When the last window of a pinned group closes, the button has to go back to
// the application icon, which the taskbar doesn't do by itself.
// https://github.com/ramensoftware/windhawk-mods/issues/644
template <typename Original>
static LONG_PTR OnTaskDestroyed(Original original,
                                PVOID taskList,
                                PVOID taskGroup,
                                PVOID taskItem) {
    if (!taskItem || !taskGroup || !g_taskbarHooksReady.load()) {
        return original();
    }

    const int numItems = CTaskGroup_GetNumItems_Original(taskGroup);
    const bool pinned = (CTaskGroup_GetFlags_Original(taskGroup) & 1) != 0;

    LONG_PTR ret = original();

    if (pinned && numItems == 1 && CTaskListWnd_GroupChanged_Original &&
        TaskGroupWantsWindowIcon(taskGroup)) {
        const int kTaskGroupPropertyIcon = 4;
        CTaskListWnd_GroupChanged_Original(taskList, taskGroup,
                                           kTaskGroupPropertyIcon);
    }
    return ret;
}

static LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_Hook(PVOID pThis,
                                                       PVOID taskGroup,
                                                       PVOID taskItem,
                                                       int flags) {
    return OnTaskDestroyed(
        [=]() {
            return CTaskListWnd_TaskDestroyed_Original(pThis, taskGroup,
                                                       taskItem, flags);
        },
        pThis, taskGroup, taskItem);
}

static LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_2_Hook(PVOID pThis,
                                                         PVOID taskGroup,
                                                         PVOID taskItem) {
    return OnTaskDestroyed(
        [=]() {
            return CTaskListWnd_TaskDestroyed_2_Original(pThis, taskGroup,
                                                         taskItem);
        },
        pThis, taskGroup, taskItem);
}

static bool IsWindows10Taskbar() {
    // explorer.exe's version: a build below 22000 is Windows 10.
    HRSRC res = FindResourceW(nullptr, MAKEINTRESOURCEW(VS_VERSION_INFO),
                              MAKEINTRESOURCEW(16) /* RT_VERSION */);
    HGLOBAL global = res ? LoadResource(nullptr, res) : nullptr;
    void* data = global ? LockResource(global) : nullptr;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT len = 0;
    if (!data ||
        !VerQueryValueW(data, L"\\", reinterpret_cast<void**>(&info), &len) ||
        !info || len == 0) {
        return false;
    }
    WORD build = HIWORD(info->dwFileVersionLS);
    Wh_Log(L"explorer.exe build %u", build);
    return build < 22000;
}

// Failing here only turns this part off; the folder previews keep working.
// The symbols live in taskbar.dll on Windows 11 and in explorer.exe on
// Windows 10.
static void HookTaskbarSymbols() {
    // taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems_Original,
        },
        {
            {LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"},
            &CTaskGroup_GetAppID_Original,
        },
        {
            {LR"(public: virtual bool __cdecl CTaskGroup::IsImmersiveGroup(void))"},
            &CTaskGroup_IsImmersiveGroup_Original,
        },
        {
            {LR"(public: virtual unsigned long __cdecl CTaskGroup::GetFlags(void)const )"},
            &CTaskGroup_GetFlags_Original,
        },
        {
            {LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE const * __cdecl CTaskGroup::GetShortcutIDList(void))"},
            &CTaskGroup_GetShortcutIDList_Original,
            CTaskGroup_GetShortcutIDList_Hook,
        },
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup_Original,
        },
        {
            // Available from Windows 11.
            {LR"(public: virtual long __cdecl CTaskBtnGroup::GetIcon(struct ITaskItem *,struct HICON__ * *))"},
            &CTaskBtnGroup_GetIcon_Original,
            CTaskBtnGroup_GetIcon_Hook,
            true,
        },
        {
            // Available until Windows 10.
            {LR"(private: void __cdecl CTaskBtnGroup::_DrawRegularButton(struct HDC__ *,struct BUTTONRENDERINFO const &))"},
            &CTaskBtnGroup__DrawRegularButton_Original,
            CTaskBtnGroup__DrawRegularButton_Hook,
            true,
        },
        {
            // Available from Windows 11.
            {LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup *,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty))"},
            &CTaskListWnd_GroupChanged_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *,enum TaskDestroyedFlags))"},
            &CTaskListWnd_TaskDestroyed_Original,
            CTaskListWnd_TaskDestroyed_Hook,
            true,
        },
        {
            // A newer variant seen in insider builds.
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_TaskDestroyed_2_Original,
            CTaskListWnd_TaskDestroyed_2_Hook,
            true,
        },
    };

    HMODULE module;
    if (IsWindows10Taskbar()) {
        module = GetModuleHandleW(nullptr);
    } else {
        module = LoadLibraryExW(L"taskbar.dll", nullptr,
                                LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll, window icons are off");
            return;
        }
    }

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Taskbar symbols not found, window icons are off");
        return;
    }

    if (!CTaskBtnGroup_GetIcon_Original &&
        !CTaskBtnGroup__DrawRegularButton_Original) {
        Wh_Log(L"No icon function found, window icons are off");
        return;
    }

    g_taskbarHooksReady = true;
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

// Puts an icon on a window from the worker thread.
static void ApplyIconAsync(HWND hwnd, HICON icon, const std::wstring& path) {
    SetOurIcon(hwnd, icon, path);
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

static void UpdateExplorerWindow(const FoundWindow& f, bool forceRender) {
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
        if (cached) {
            ApplyIconAsync(f.hwnd, cached, f.path);
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
    HICON fresh = StoreRendered(f.path, px, HashPixels(px), /*byWorker=*/true,
                                &needSave);
    if (needSave) {
        SaveToDisk(f.path, px);
    }
    Wh_Log(L"rendered in %llu ms | %s", GetTickCount64() - start,
           f.path.c_str());

    if (!fresh) {
        return;
    }

    AcquireSRWLockShared(&g_lock);
    auto it = g_ourIcons.find({f.hwnd, ICON_BIG});
    bool same = it != g_ourIcons.end() && it->second == fresh;
    ReleaseSRWLockShared(&g_lock);
    if (!same) {
        ApplyIconAsync(f.hwnd, fresh, f.path);
    }
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
// instant path couldn't resolve.
static void Scan(const std::map<HWND, bool>* only) {
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put())))) {
        return;
    }

    long count = 0;
    if (FAILED(shellWindows->get_Count(&count))) {
        return;
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
            UpdateExplorerWindow(f, force);
        }
        ILFree(f.pidl);
    }
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

    Scan(&due);

    for (const auto& [hwnd, step] : steps) {
        if (step < kNoConfirms && IsWindow(hwnd)) {
            Schedule(hwnd, kConfirmDelays[step], false, step + 1);
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
            SendMessageTimeoutW(hwnd, g_msgRestoreIcons, 0, 0,
                                SMTO_ABORTIFHUNG, kSendTimeout, nullptr);
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

    HINSTANCE inst = GetModuleHandleW(nullptr);
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

    if (!g_startupScanDone) {
        g_startupScanDone = true;
        Scan(nullptr);  // windows which were open before the mod started
    }

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
        } else {
            DestroyIcon(icon);
        }
    }
    g_owned.clear();
    g_folderIcon = nullptr;
    g_ourIcons.clear();
    g_explorerIcons.clear();
    g_shownPath.clear();
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

static void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"taskbarWindowIcons");
    g_windowIconsMode = static_cast<int>(mode && wcscmp(mode, L"all") == 0
                                             ? WindowIconsMode::All
                                             : WindowIconsMode::Explorer);
    Wh_FreeStringSetting(mode);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    InitCacheDir();

    // A preview is rendered a little larger than the taskbar needs, so that
    // it stays sharp at every DPI. SM_CXICON is 32 at 100% and 64 at 200%.
    g_iconSize = std::clamp(GetSystemMetrics(SM_CXICON) * 2, 64, 128);
    Wh_Log(L"preview size %d px", g_iconSize);

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return FALSE;
    }
    void* showWindow =
        reinterpret_cast<void*>(GetProcAddress(user32, "ShowWindow"));
    if (!showWindow) {
        return FALSE;
    }

    g_msgRestoreIcons =
        RegisterWindowMessageW(L"WhFolderThumbTaskbar_RestoreIcons");
    g_msgRecheck = RegisterWindowMessageW(L"WhFolderThumbTaskbar_Recheck");

    CreateFolderIcon();

    Wh_SetFunctionHook(showWindow, reinterpret_cast<void*>(ShowWindow_Hook),
                       reinterpret_cast<void**>(&ShowWindow_Original));

    HookTaskbarSymbols();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        return FALSE;
    }

    g_enabled = true;
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

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
