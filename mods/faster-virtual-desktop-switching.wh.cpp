// ==WindhawkMod==
// @id              faster-virtual-desktop-switching
// @name            Faster Virtual Desktop Switching
// @description     Makes virtual desktop switches and touchpad swipes start faster: caches wallpaper images, skips hidden windows, and requests a short performance boost.
// @version         1.5.0
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luser32 -lshell32 -lgdi32 -ladvapi32 -luuid -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Faster Virtual Desktop Switching

Makes virtual desktop switches, especially touchpad swipes, start faster and
stutter less.

## What it does

- **Wallpaper cache:** the wallpaper images Windows shows during a switch are
  prepared in the background and kept in memory, so a switch doesn't wait for
  Windows to load them. Nearby desktops are prepared ahead of time, including
  right after Explorer starts.
- **Smaller wallpaper images:** limits their size (1024 px by default), so they
  load and copy faster.
- **Faster swipe start:** when a swipe starts, Windows checks every window on
  the system, most of them hidden. The mod skips the hidden ones, which Windows
  would ignore anyway. The animation looks the same.
- **Performance boost:** asks Windows for a short burst of performance (Low
  Latency Profile) when a switch starts.

## Settings tips

- With **Maximum thumbnail size** set to 0 on a high-resolution display, each
  wallpaper image can take 20-40 MiB. Raise **Cache memory limit** to match;
  with logging on, the log states the value needed.
- To prepare every desktop, set **Neighboring desktops to prepare** to half
  your desktop count or more.

## Notes

- Only wallpapers stored on local drives are cached. Changed files are
  detected automatically.
- If a Windows update changes the parts this mod relies on, the affected
  feature turns itself off and Windows behaves as usual.
- Turn on logging (Advanced tab) to see cache hits and timings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maximumThumbnailSize: 1024
  $name: Maximum thumbnail size
  $description: Size limit for wallpaper images used during switches (64-1024 px). 0 means no limit.
- neighborsEachDirection: 2
  $name: Neighboring desktops to prepare
  $description: Desktops prepared ahead on each side. Half your desktop count or more prepares all of them. 0 turns this off.
- cacheMemoryMb: 64
  $name: Cache memory limit (MiB)
  $description: Memory for cached wallpaper images (16-1024 MiB).
- prefetchQuietMs: 100
  $name: Background quiet period (ms)
  $description: How long to wait after a switch before preparing images in the background (0-1000 ms).
- disableLowLatencyBoost: false
  $name: Disable performance boost
  $description: Don't request the short performance boost when a switch starts.
- disableHiddenWindowFilter: false
  $name: Disable hidden window filter
  $description: Let Windows check hidden windows too when a swipe starts.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <objbase.h>
#include <roapi.h>
#include <shobjidl.h>
#include <thumbcache.h>
#include <winstring.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace {

using ThumbnailCacheGetThumbnail = HRESULT(STDMETHODCALLTYPE*)(
    IThumbnailCache*, IShellItem*, UINT, WTS_FLAGS, ISharedBitmap**,
    WTS_CACHEFLAGS*, WTS_THUMBNAILID*);

ThumbnailCacheGetThumbnail g_getThumbnailOriginal = nullptr;
HMODULE g_twinuiPcShell = nullptr;
HMODULE g_thumbnailCacheServer = nullptr;
uintptr_t g_targetFunctionStart = 0;
uintptr_t g_targetFunctionEnd = 0;
std::atomic<bool> g_stopping{false}, g_suspended{false}, g_workerReady{false};
std::atomic<bool> g_lowLatencyBoostEnabled{true};
// 0 disables the size limit, neighbor prefetching, or the respective wait.
std::atomic<UINT> g_maximumThumbnailSize{1024};
std::atomic<int> g_neighbors{2};
// Background extraction starts only this long after the last transition
// request or desktop change. This absorbs the burst of registry writes and
// hook calls of one switch, so the plan reflects the new desktop, and keeps
// prefetching from competing with the switch's own extraction.
std::atomic<ULONGLONG> g_quietMs{100};
std::atomic<ULONGLONG> g_sequence{0}, g_epoch{0}, g_lastActivity{0};
std::atomic<ULONGLONG> g_hits{0}, g_misses{0}, g_prepared{0}, g_spareHits{0};
HANDLE g_stopEvent = nullptr, g_workEvent = nullptr, g_worker = nullptr;
SRWLOCK g_cacheLock = SRWLOCK_INIT, g_queueLock = SRWLOCK_INIT;

constexpr size_t kCacheEntries = 64;
// Image bytes plus each entry's ready copy.
std::atomic<size_t> g_cacheLimitBytes{64u * 1024 * 1024};
// A swipe is in progress from gesture start until its end animation's cleanup.
// Background work also waits for this, bounded by a timeout in case no
// cleanup arrives.
constexpr ULONGLONG kTransitionTimeoutMs = 3000;
std::atomic<bool> g_transitionActive{false};
std::atomic<ULONGLONG> g_transitionStart{0};
// Windows cleans up any previous gesture at the start of a new one; that
// call must not end the transition that is beginning.
std::atomic<DWORD> g_initializingThread{0};
// Startup warm-up waits this long after the worker starts.
constexpr ULONGLONG kStartupDelayMs = 2000;
std::atomic<ULONGLONG> g_workerStartTick{0};
// Set when a cache entry needs a new ready copy.
std::atomic<bool> g_spareRefill{false};
// Size and flags of real requests, remembered across restarts.
constexpr wchar_t kVariantsValueName[] = L"requestVariants";
constexpr wchar_t kDesktopRoot[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";

struct FileStamp {
    DWORD sizeHigh{}, sizeLow{};
    FILETIME modified{};
    bool operator==(const FileStamp& other) const {
        return sizeHigh == other.sizeHigh && sizeLow == other.sizeLow &&
            modified.dwHighDateTime == other.modified.dwHighDateTime &&
            modified.dwLowDateTime == other.modified.dwLowDateTime;
    }
};

struct Request {
    std::wstring path;
    UINT size{};
    WTS_FLAGS flags{};
};

// This state belongs to the worker and never retains Shell objects.
struct DesktopHistory {
    std::vector<GUID> order;
    GUID current{};
    int direction{};
};

struct CachedBitmap {
    Request request;
    FileStamp stamp;
    HBITMAP bitmap{};
    WTS_ALPHATYPE alpha{};
    WTS_CACHEFLAGS outFlags{};
    WTS_THUMBNAILID thumbnailId{};
    size_t bytes{};  // image, plus its ready copy when one is reserved
    ULONGLONG epoch{};
    std::atomic<ULONGLONG> lastUsed{0};
    // Set when the budget had room for a ready copy. Large images skip it and
    // copy on each hit instead, so they stay cacheable within the 1.4 limit.
    bool keepsSpare{};
    // A ready, independent copy that the next hit hands to Windows. Taken and
    // replaced atomically; any thread may own it once exchanged out.
    std::atomic<HBITMAP> spare{nullptr};
    ~CachedBitmap() {
        if (bitmap) DeleteObject(bitmap);
        if (HBITMAP ready = spare.exchange(nullptr)) DeleteObject(ready);
    }
};

struct RequestVariant {
    UINT size{};
    WTS_FLAGS flags{};
};

std::array<std::shared_ptr<CachedBitmap>, kCacheEntries> g_cache;
std::array<Request, 8> g_recentRequests;
size_t g_recentCount = 0, g_cacheBytes = 0;

// Images too large for the cache budget, by request and file version, so the
// worker does not extract them again after every switch. Guarded by
// g_cacheLock; cleared with the cache, so a new limit retries them.
struct TooLarge {
    Request request;
    FileStamp stamp;
};
constexpr size_t kTooLargeEntries = 16;
std::array<TooLarge, kTooLargeEntries> g_tooLarge;  // ring; empty path = unused
size_t g_tooLargeNext = 0;
// Guarded by g_queueLock. Used to plan work before any real request is seen.
std::vector<RequestVariant> g_persistedVariants;

// Hidden window filter. Windows' transition background walks every top-level
// window; its callback reads a window property (a system call) before it
// checks visibility, so hidden windows are rejected only after that read.
using EnumWindowsFunction = BOOL(WINAPI*)(WNDENUMPROC, LPARAM);
EnumWindowsFunction g_enumWindowsOriginal = nullptr;
uintptr_t g_backgroundFunctionStart = 0;
uintptr_t g_backgroundFunctionEnd = 0;
std::atomic<bool> g_hiddenWindowFilterEnabled{true};
std::atomic<ULONGLONG> g_windowWalks{0}, g_windowsSkipped{0};

// Restrict speculative work to local fixed-drive files and retain a file
// version in each cache key so an overwritten wallpaper cannot hit stale data.
bool ReadFileStamp(const std::wstring& path, FileStamp& stamp) {
    if (path.size() < 3 || path[1] != L':' ||
        (path[2] != L'\\' && path[2] != L'/')) return false;
    wchar_t root[] = {path[0], L':', L'\\', L'\0'};
    if (GetDriveTypeW(root) != DRIVE_FIXED) return false;
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data) ||
        (data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS |
            FILE_ATTRIBUTE_RECALL_ON_OPEN | FILE_ATTRIBUTE_REPARSE_POINT))) return false;
    stamp = {data.nFileSizeHigh, data.nFileSizeLow, data.ftLastWriteTime};
    return true;
}

// Compare the full extraction contract; desktop/monitor identities are not
// part of an image's identity when the source, size and flags are identical.
bool SameRequest(const Request& a, const Request& b) {
    return a.size == b.size && a.flags == b.flags &&
        _wcsicmp(a.path.c_str(), b.path.c_str()) == 0;
}

// Drop owned GDI bitmaps without retaining any Shell or composition objects.
void ClearCache() {
    // Destroy outside the lock; resetting a string never allocates.
    std::array<std::shared_ptr<CachedBitmap>, kCacheEntries> old;
    AcquireSRWLockExclusive(&g_cacheLock);
    old.swap(g_cache);
    g_cacheBytes = 0;
    for (auto& item : g_tooLarge) item.request.path.clear();
    g_tooLargeNext = 0;
    ReleaseSRWLockExclusive(&g_cacheLock);
}

bool IsTooLarge(const Request& request, const FileStamp& stamp);

// Record an image that cannot fit, so the worker skips it until the cache is
// cleared (for example by a new memory limit) or the file changes.
void RememberTooLarge(const Request& request, const FileStamp& stamp,
                      size_t imageBytes) {
    if (IsTooLarge(request, stamp)) return;
    TooLarge item{request, stamp};  // copy outside the lock
    AcquireSRWLockExclusive(&g_cacheLock);
    g_tooLarge[g_tooLargeNext] = std::move(item);
    g_tooLargeNext = (g_tooLargeNext + 1) % kTooLargeEntries;
    ReleaseSRWLockExclusive(&g_cacheLock);
    const size_t limit = g_cacheLimitBytes.load();
    Wh_Log(L"Not cached: %.1f MiB image (size=%u) exceeds half the cache memory "
           L"limit (%zu MiB). Set Cache memory limit to at least %zu MiB to cache "
           L"it, or %zu MiB to also keep a ready copy.",
           imageBytes / 1048576.0, request.size, limit >> 20,
           ((imageBytes * 2) >> 20) + 1, ((imageBytes * 4) >> 20) + 1);
}

bool IsTooLarge(const Request& request, const FileStamp& stamp) {
    bool found = false;
    AcquireSRWLockShared(&g_cacheLock);
    for (const auto& item : g_tooLarge) {
        if (!item.request.path.empty() && item.stamp == stamp &&
            SameRequest(item.request, request)) {
            found = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_cacheLock);
    return found;
}

// Notify the worker without posting to Explorer's UI thread. Incrementing the
// epoch prevents an extraction already in progress from publishing stale data.
void InvalidateCache() {
    ++g_epoch;
    ++g_sequence;
    if (g_workEvent) SetEvent(g_workEvent);
}

// Record a foreground transition request or desktop-state change. Background
// extraction waits for a quiet period after the latest activity.
void NoteActivity() {
    g_lastActivity = GetTickCount64();
}

// Background work may start at this tick: after the quiet period, after a
// swipe in progress has finished (bounded), and after the startup delay.
ULONGLONG QuietUntil() {
    ULONGLONG until = g_lastActivity.load() + g_quietMs.load();
    if (g_transitionActive.load())
        until = std::max(until, g_transitionStart.load() + kTransitionTimeoutMs);
    return std::max(until, g_workerStartTick.load() + kStartupDelayMs);
}

void RequestSpareRefill() {
    g_spareRefill = true;
    if (g_workEvent) SetEvent(g_workEvent);
}

struct WalkContext {
    WNDENUMPROC callback;
    LPARAM param;
    UINT total, skipped;
};

// Windows' own callback rejects a window that is not visible, after reading
// its property. Checking visibility first (no system call) rejects the same
// windows; every visible window goes to Windows' callback unchanged.
BOOL CALLBACK FilterWindow(HWND window, LPARAM param) {
    auto* context = reinterpret_cast<WalkContext*>(param);
    ++context->total;
    if (!IsWindowVisible(window)) {
        ++context->skipped;
        return TRUE;
    }
    return context->callback(window, context->param);
}

// Filter only the transition background's walk; every other EnumWindows
// caller in Explorer goes straight to Windows.
__attribute__((noinline))
BOOL WINAPI EnumWindowsHook(WNDENUMPROC callback, LPARAM param) {
    const uintptr_t caller =
        reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (caller < g_backgroundFunctionStart || caller >= g_backgroundFunctionEnd ||
        !callback || g_stopping || !g_hiddenWindowFilterEnabled) {
        return g_enumWindowsOriginal(callback, param);
    }
    NoteActivity();
    WalkContext context{callback, param, 0, 0};
    const auto start = std::chrono::steady_clock::now();
    const BOOL result =
        g_enumWindowsOriginal(FilterWindow, reinterpret_cast<LPARAM>(&context));
    ++g_windowWalks;
    g_windowsSkipped += context.skipped;
    Wh_Log(L"Window walk: %u windows, %u hidden skipped, %.3f ms",
           context.total, context.skipped,
           std::chrono::duration<double, std::milli>(
               std::chrono::steady_clock::now() - start).count());
    return result;
}

// Return a retained image only when its file version and generation match.
// There is no age limit: the file stamp rejects overwritten wallpapers, and
// the epoch rejects display, settings and power changes, so entries remain
// useful while the user stays on one desktop for a long time.
std::shared_ptr<CachedBitmap> FindBitmap(const Request& request,
                                       const FileStamp& stamp) {
    std::shared_ptr<CachedBitmap> found;
    const ULONGLONG now = GetTickCount64(), epoch = g_epoch.load();
    AcquireSRWLockShared(&g_cacheLock);
    for (const auto& entry : g_cache) {
        if (entry && entry->epoch == epoch && entry->stamp == stamp &&
            SameRequest(entry->request, request)) {
            found = entry;
            entry->lastUsed = now;
            break;
        }
    }
    ReleaseSRWLockShared(&g_cacheLock);
    return found;
}

// Duplicate an uncompressed 24/32-bit DIB section at the same bit depth.
// GetObject reports a positive biHeight even for top-down DIB sections, so
// the source orientation cannot be read back and its rows must not be copied
// raw. Instead the copy is always top-down and GetDIBits converts from the
// source's real orientation. GetDIBits does not select either bitmap into a
// device context, so several threads may copy the same cached bitmap at once.
HBITMAP DuplicateDib(HBITMAP source, size_t* copiedBytes) {
    DIBSECTION section{};
    if (!source ||
        GetObjectW(source, sizeof(section), &section) != sizeof(section) ||
        !section.dsBm.bmBits || section.dsBmih.biCompression != BI_RGB ||
        (section.dsBmih.biBitCount != 32 && section.dsBmih.biBitCount != 24) ||
        section.dsBm.bmWidth <= 0 || section.dsBm.bmHeight <= 0 ||
        section.dsBm.bmWidth > 4096 || section.dsBm.bmHeight > 4096) {
        return nullptr;
    }
    // DIB rows are DWORD-aligned; compute the stride rather than trusting
    // BITMAP::bmWidthBytes, which documents only WORD alignment.
    const size_t stride =
        ((static_cast<size_t>(section.dsBm.bmWidth) *
          section.dsBmih.biBitCount + 31) / 32) * 4;
    const size_t bytes = stride * static_cast<size_t>(section.dsBm.bmHeight);
    const int height = section.dsBm.bmHeight;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = section.dsBm.bmWidth;
    info.bmiHeader.biHeight = -height;  // Top-down, explicitly.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = section.dsBmih.biBitCount;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP copy = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits,
                                    nullptr, 0);
    if (!copy || !bits) {
        if (copy) DeleteObject(copy);
        return nullptr;
    }
    GdiFlush();
    // Same-format GetDIBits copies all 32 bits per pixel, including alpha.
    HDC screen = GetDC(nullptr);
    const int lines = screen ?
        GetDIBits(screen, source, 0, static_cast<UINT>(height), bits, &info,
                  DIB_RGB_COLORS) : 0;
    if (screen) ReleaseDC(nullptr, screen);
    if (lines != height) {
        DeleteObject(copy);
        return nullptr;
    }
    if (copiedBytes) *copiedBytes = bytes;
    return copy;
}

// Copy a cached image, which DuplicateDib created: always top-down,
// uncompressed 24/32-bit, DWORD-aligned rows. Its layout is known, so a plain
// memory copy of the pixels is exact and avoids GDI's conversion path. Never
// use this for a bitmap from Windows, whose orientation cannot be read back.
// Cached images are never drawn into, so concurrent readers are safe.
HBITMAP CopyOwnDib(HBITMAP source) {
    DIBSECTION section{};
    if (!source ||
        GetObjectW(source, sizeof(section), &section) != sizeof(section) ||
        !section.dsBm.bmBits || section.dsBmih.biCompression != BI_RGB ||
        (section.dsBmih.biBitCount != 32 && section.dsBmih.biBitCount != 24) ||
        section.dsBm.bmWidth <= 0 || section.dsBm.bmHeight <= 0) {
        return nullptr;
    }
    const size_t stride =
        ((static_cast<size_t>(section.dsBm.bmWidth) *
          section.dsBmih.biBitCount + 31) / 32) * 4;
    const size_t bytes = stride * static_cast<size_t>(section.dsBm.bmHeight);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = section.dsBm.bmWidth;
    info.bmiHeader.biHeight = -section.dsBm.bmHeight;  // same top-down layout
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = section.dsBmih.biBitCount;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP copy = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits,
                                    nullptr, 0);
    if (!copy || !bits) {
        if (copy) DeleteObject(copy);
        return nullptr;
    }
    std::memcpy(bits, section.dsBm.bmBits, bytes);
    return copy;
}

// Copy a successful native thumbnail while still in its owning apartment.
// Only a private DIB and value metadata enter the process-wide cache.
// Returns whether the image entered the cache.
bool StoreBitmap(const Request& request, const FileStamp& stamp,
                 ISharedBitmap* shared, WTS_CACHEFLAGS outFlags,
                 const WTS_THUMBNAILID& id, ULONGLONG epoch) {
    if (!shared || g_stopping || epoch != g_epoch) return false;
    HBITMAP borrowed = nullptr;
    WTS_ALPHATYPE alpha{};
    if (FAILED(shared->GetSharedBitmap(&borrowed)) || !borrowed ||
        FAILED(shared->GetFormat(&alpha))) return false;
    auto entry = std::make_shared<CachedBitmap>();
    size_t bytes = 0;
    HBITMAP copy = DuplicateDib(borrowed, &bytes);
    if (!copy) return false;
    // Own the copy before any further check so every early return frees it.
    entry->bitmap = copy;
    // An image may use up to half the budget, as before ready copies existed.
    // A ready copy is reserved only when image and copy fit in that half.
    const size_t limit = g_cacheLimitBytes.load();
    if (bytes > limit / 2) {
        RememberTooLarge(request, stamp, bytes);
        return false;
    }
    entry->keepsSpare = bytes * 2 <= limit / 2;
    const size_t entryBytes = entry->keepsSpare ? bytes * 2 : bytes;
    entry->request = request;
    entry->stamp = stamp;
    entry->alpha = alpha;
    entry->outFlags = outFlags;
    entry->thumbnailId = id;
    entry->bytes = entryBytes;
    entry->lastUsed = GetTickCount64();
    entry->epoch = epoch;
    const bool entryKeepsSpare = entry->keepsSpare;  // entry is moved below

    bool stored = false;
    AcquireSRWLockExclusive(&g_cacheLock);
    if (!g_stopping && epoch == g_epoch) {
        size_t slot = kCacheEntries;
        for (size_t i = 0; i < kCacheEntries; ++i) {
            if (g_cache[i] && SameRequest(g_cache[i]->request, request)) {
                slot = i;
                break;
            }
            if (!g_cache[i]) slot = i;
        }
        if (slot == kCacheEntries) {
            slot = 0;
            for (size_t i = 1; i < kCacheEntries; ++i)
                if (g_cache[i]->lastUsed < g_cache[slot]->lastUsed) slot = i;
        }
        if (g_cache[slot]) g_cacheBytes -= g_cache[slot]->bytes;
        g_cache[slot].reset();
        while (g_cacheBytes + entryBytes > limit) {
            size_t oldest = kCacheEntries;
            for (size_t i = 0; i < kCacheEntries; ++i)
                if (g_cache[i] && (oldest == kCacheEntries ||
                    g_cache[i]->lastUsed < g_cache[oldest]->lastUsed)) oldest = i;
            if (oldest == kCacheEntries) break;
            g_cacheBytes -= g_cache[oldest]->bytes;
            g_cache[oldest].reset();
        }
        g_cache[slot] = std::move(entry);
        g_cacheBytes += entryBytes;
        stored = true;
    }
    ReleaseSRWLockExclusive(&g_cacheLock);
    // The worker makes the ready copy right away.
    if (stored && entryKeepsSpare) RequestSpareRefill();
    return stored;
}

// Return an independent Windows-owned bitmap. Its vtable and lifetime do not
// depend on this mod, even if Explorer retains it across a mod reload. The
// entry's ready copy is handed over when present; otherwise a copy is made
// here, as before. Either way the worker is asked to prepare the next one.
HRESULT MakeSharedBitmap(CachedBitmap& entry, ISharedBitmap** output) {
    ISharedBitmap* shared = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(SharedBitmap), nullptr,
                                 CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shared));
    if (FAILED(hr)) return hr;
    HBITMAP copy = entry.keepsSpare ? entry.spare.exchange(nullptr) : nullptr;
    if (copy) ++g_spareHits;
    else copy = CopyOwnDib(entry.bitmap);
    if (entry.keepsSpare) RequestSpareRefill();
    if (!copy) {
        shared->Release();
        return E_OUTOFMEMORY;
    }
    hr = shared->InitializeBitmap(copy, entry.alpha);
    if (FAILED(hr)) {
        DeleteObject(copy);
        shared->Release();
        return hr;
    }
    // InitializeBitmap transfers ownership on success.
    *output = shared;
    return S_OK;
}

// Runs on the worker only, as soon as a copy is needed. A copy that loses a
// race (another refill, or an entry already holding one) is freed.
void RefillSpares() {
    g_spareRefill = false;
    std::vector<std::shared_ptr<CachedBitmap>> missing;
    // Reserve before locking so nothing under the lock can throw.
    missing.reserve(kCacheEntries);
    const ULONGLONG epoch = g_epoch.load();
    AcquireSRWLockShared(&g_cacheLock);
    for (const auto& entry : g_cache)
        if (entry && entry->keepsSpare && entry->epoch == epoch &&
            !entry->spare.load())
            missing.push_back(entry);
    ReleaseSRWLockShared(&g_cacheLock);
    for (const auto& entry : missing) {
        if (g_stopping || g_suspended || epoch != g_epoch) return;
        HBITMAP copy = CopyOwnDib(entry->bitmap);
        if (!copy) continue;
        HBITMAP expected = nullptr;
        if (!entry->spare.compare_exchange_strong(expected, copy)) DeleteObject(copy);
    }
}

// Keep recent real request variants so speculative requests reproduce Windows'
// sizing/flags instead of guessing a monitor-dependent extraction contract.
void QueueRequest(const Request& request) {
    Request pending = request;
    AcquireSRWLockExclusive(&g_queueLock);
    size_t index = 0;
    while (index < g_recentCount &&
           !SameRequest(g_recentRequests[index], request)) ++index;
    if (index == g_recentCount && g_recentCount < g_recentRequests.size())
        ++g_recentCount;
    if (index >= g_recentRequests.size()) index = g_recentRequests.size() - 1;
    for (; index > 0; --index)
        g_recentRequests[index] = std::move(g_recentRequests[index - 1]);
    g_recentRequests[0] = std::move(pending);
    ReleaseSRWLockExclusive(&g_queueLock);
    ++g_sequence;
    if (g_workEvent) SetEvent(g_workEvent);
}

// Limit only the original wallpaper helper. A failed cache lookup or native
// object creation always falls back to Windows with the same clamped size.
__attribute__((noinline))
HRESULT STDMETHODCALLTYPE GetThumbnailHook(
    IThumbnailCache* self, IShellItem* shellItem, UINT requestedSize,
    WTS_FLAGS flags, ISharedBitmap** sharedBitmap, WTS_CACHEFLAGS* outFlags,
    WTS_THUMBNAILID* thumbnailId) {
    const uintptr_t caller =
        reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (caller < g_targetFunctionStart || caller >= g_targetFunctionEnd ||
        g_stopping) {
        return g_getThumbnailOriginal(self, shellItem, requestedSize, flags,
                                      sharedBitmap, outFlags, thumbnailId);
    }
    NoteActivity();
    if (const UINT limit = g_maximumThumbnailSize.load())
        requestedSize = std::min(requestedSize, limit);
    constexpr DWORD knownFlags = 0x1 | 0x2 | 0x4 | 0x8 | 0x20 | 0x40 |
        0x80 | 0x100 | 0x200 | 0x400 | 0x800 | 0x2000 | 0x4000 | 0x8000 | 0x10000;
    const bool cacheable = g_workerReady && !g_suspended && shellItem &&
        sharedBitmap && !(static_cast<DWORD>(flags) &
            (WTS_FORCEEXTRACTION | WTS_EXTRACTDONOTCACHE | ~knownFlags));
    const auto start = std::chrono::steady_clock::now();
    const ULONGLONG epoch = g_epoch.load();
    Request request;
    FileStamp before{};
    bool haveRequest = false;
    try {
        if (cacheable) {
            PWSTR name = nullptr;
            if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_FILESYSPATH, &name)) && name) {
                // Ensure the Shell allocation is released even if std::wstring throws.
                std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> owned(name, CoTaskMemFree);
                request = {name, requestedSize, flags};
                haveRequest = ReadFileStamp(request.path, before);
            }
            if (haveRequest) {
                auto cached = FindBitmap(request, before);
                if (cached && SUCCEEDED(MakeSharedBitmap(*cached, sharedBitmap))) {
                    if (outFlags) *outFlags = static_cast<WTS_CACHEFLAGS>(
                        cached->outFlags | WTS_CACHED);
                    if (thumbnailId) *thumbnailId = cached->thumbnailId;
                    try { QueueRequest(request); } catch (...) {}
                    ++g_hits;
                    Wh_Log(L"Cache hit: %.3f ms, size=%u flags=0x%X",
                            std::chrono::duration<double, std::milli>(
                                std::chrono::steady_clock::now() - start).count(),
                            requestedSize, flags);
                    return S_OK;
                }
            }
        }
    } catch (...) {
        // Allocation failures must not escape into Explorer.
        haveRequest = false;
    }

    WTS_CACHEFLAGS actualFlags{};
    WTS_THUMBNAILID actualId{};
    const HRESULT hr = g_getThumbnailOriginal(
        self, shellItem, requestedSize, flags, sharedBitmap,
        outFlags ? outFlags : &actualFlags,
        thumbnailId ? thumbnailId : &actualId);
    // Measure the quiet period from the end of the foreground extraction.
    NoteActivity();
    if (haveRequest) {
        ++g_misses;
        try {
            FileStamp after{};
            if (SUCCEEDED(hr) && sharedBitmap && *sharedBitmap &&
                ReadFileStamp(request.path, after) && before == after) {
                StoreBitmap(request, before, *sharedBitmap,
                    outFlags ? *outFlags : actualFlags,
                    thumbnailId ? *thumbnailId : actualId, epoch);
            }
            // Start speculative work after this foreground request completes,
            // rather than racing a second extraction against its cold miss.
            QueueRequest(request);
        } catch (...) {}
        Wh_Log(L"Cache miss: %.3f ms, size=%u flags=0x%X hr=0x%08lX",
                   std::chrono::duration<double, std::milli>(
                       std::chrono::steady_clock::now() - start).count(),
                   requestedSize, flags,
                   static_cast<unsigned long>(hr));
    }
    return hr;
}

// Up to four distinct size/flag contracts, most recent first.
std::vector<RequestVariant> VariantsOf(const std::vector<Request>& requests) {
    std::vector<RequestVariant> variants;
    for (const auto& request : requests) {
        if (std::none_of(variants.begin(), variants.end(), [&](const RequestVariant& old) {
            return old.size == request.size && old.flags == request.flags;
        })) variants.push_back({request.size, request.flags});
        if (variants.size() == 4) break;
    }
    return variants;
}

// Read the latest desktop order and prioritize nearby wallpapers in the last
// observed direction. Unknown registry state falls back to recent real
// requests. Before any real request, remembered contracts plan the warm-up.
std::vector<Request> BuildWorkList(DesktopHistory& history) {
    std::vector<Request> recent, work;
    std::vector<RequestVariant> persisted;
    AcquireSRWLockShared(&g_queueLock);
    try {
        recent.assign(g_recentRequests.begin(), g_recentRequests.begin() + g_recentCount);
        persisted = g_persistedVariants;
    } catch (...) {
        ReleaseSRWLockShared(&g_queueLock);
        throw;
    }
    ReleaseSRWLockShared(&g_queueLock);
    const std::vector<RequestVariant> variants =
        recent.empty() ? persisted : VariantsOf(recent);
    if (variants.empty()) return work;

    auto append = [&](const Request& request) {
        if (work.size() < kCacheEntries && std::none_of(work.begin(), work.end(),
            [&](const Request& old) { return SameRequest(old, request); }))
            work.push_back(request);
    };
    // Keep registry failures local so recent requests are still appended.
    auto appendNeighbors = [&] {
        GUID desktops[128]{}, current{};
        DWORD bytes = sizeof(desktops), currentBytes = sizeof(current);
        if (RegGetValueW(HKEY_CURRENT_USER, kDesktopRoot, L"VirtualDesktopIDs",
                        RRF_RT_REG_BINARY, nullptr, desktops, &bytes) != ERROR_SUCCESS ||
            !bytes || bytes % sizeof(GUID)) return;
        LONG status = RegGetValueW(HKEY_CURRENT_USER, kDesktopRoot,
            L"CurrentVirtualDesktop", RRF_RT_REG_BINARY, nullptr, &current, &currentBytes);
        if (status != ERROR_SUCCESS || currentBytes != sizeof(GUID)) {
            DWORD session = 0;
            if (!ProcessIdToSessionId(GetCurrentProcessId(), &session)) return;
            std::wstring sessionKey =
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\" +
                std::to_wstring(session) + L"\\VirtualDesktops";
            currentBytes = sizeof(current);
            if (RegGetValueW(HKEY_CURRENT_USER, sessionKey.c_str(),
                L"CurrentVirtualDesktop", RRF_RT_REG_BINARY, nullptr, &current,
                &currentBytes) != ERROR_SUCCESS || currentBytes != sizeof(GUID)) return;
        }
        const int count = bytes / sizeof(GUID);
        int index = 0;
        while (index < count && !IsEqualGUID(desktops[index], current)) ++index;
        if (index == count) return;

        const bool sameOrder = history.order.size() == static_cast<size_t>(count) &&
            std::equal(history.order.begin(), history.order.end(), desktops,
                       [](const GUID& a, const GUID& b) { return IsEqualGUID(a, b); });
        if (sameOrder && !IsEqualGUID(history.current, current)) {
            auto previous = std::find_if(history.order.begin(), history.order.end(),
                [&](const GUID& id) { return IsEqualGUID(id, history.current); });
            if (previous != history.order.end()) {
                int delta = index - static_cast<int>(previous - history.order.begin());
                // Treat desktops as a ring: last -> first is one step forward,
                // not a long jump backward. Take the shorter way around.
                if (count > 2) {
                    if (delta > count / 2) delta -= count;
                    else if (delta < -count / 2) delta += count;
                }
                history.direction = delta > 0 ? 1 : delta < 0 ? -1 : 0;
            } else {
                history.direction = 0;
            }
        } else if (!sameOrder) {
            history.direction = 0;
        }
        history.order.assign(desktops, desktops + count);
        history.current = current;
        auto appendDesktop = [&](int desktop) {
            wchar_t guid[40]{}, path[32768]{};
            if (!StringFromGUID2(desktops[desktop], guid, ARRAYSIZE(guid))) return;
            const std::wstring key = std::wstring(kDesktopRoot) + L"\\Desktops\\" + guid;
            DWORD pathBytes = sizeof(path);
            if (RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Wallpaper",
                RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, path,
                &pathBytes) != ERROR_SUCCESS || !path[0]) return;
            for (const auto& variant : variants)
                append({path, variant.size, variant.flags});
        };
        const int preferred = history.direction < 0 ? -1 : 1;
        // On a ring, distances beyond half the desktop count repeat desktops.
        const int neighbors = std::min(g_neighbors.load(), count / 2);
        for (int distance = 1; distance <= neighbors; ++distance) {
            for (int direction : {preferred, -preferred}) {
                // Desktops form a ring, so the ends are each other's neighbors.
                const int neighbor =
                    ((index + direction * distance) % count + count) % count;
                if (neighbor == index) continue;
                appendDesktop(neighbor);
            }
        }
        // Lowest priority: switching back to where you are now.
        if (neighbors > 0) appendDesktop(index);
    };
    appendNeighbors();
    // Recent requests cover alternate monitor images and unavailable registry
    // entries, but must not displace the next likely desktop from the queue.
    for (const auto& request : recent) append(request);
    return work;
}

// Stored as "limit;size,flags;size,flags...". The size limit is part of the
// record because requests are clamped by it: contracts saved under another
// limit would never match a real request.
std::wstring FormatVariants(const std::vector<RequestVariant>& variants) {
    std::wstring text = std::to_wstring(g_maximumThumbnailSize.load());
    for (const auto& variant : variants) {
        text += L';';
        text += std::to_wstring(variant.size);
        text += L',';
        text += std::to_wstring(static_cast<DWORD>(variant.flags));
    }
    return text;
}

// Worker only: seed the warm-up plan from the last session's contracts.
// Returns the stored text so unchanged contracts are not rewritten.
std::wstring LoadPersistedVariants() {
    wchar_t stored[256]{};
    Wh_GetStringValue(kVariantsValueName, stored, ARRAYSIZE(stored));
    std::vector<RequestVariant> variants;
    wchar_t* cursor = stored;
    wchar_t* end = nullptr;
    const unsigned long limit = wcstoul(cursor, &end, 10);
    if (end == cursor || limit != g_maximumThumbnailSize.load()) return L"";
    cursor = end;
    while (*cursor == L';' && variants.size() < 4) {
        const unsigned long size = wcstoul(cursor + 1, &end, 10);
        if (end == cursor + 1 || *end != L',') break;
        cursor = end + 1;
        const unsigned long flags = wcstoul(cursor, &end, 10);
        if (end == cursor) break;
        cursor = end;
        // Only contracts the cache can serve.
        if (!size || size > 4096 ||
            (flags & (WTS_FORCEEXTRACTION | WTS_EXTRACTDONOTCACHE))) continue;
        variants.push_back({static_cast<UINT>(size), static_cast<WTS_FLAGS>(flags)});
    }
    if (variants.empty()) return L"";
    const size_t count = variants.size();
    AcquireSRWLockExclusive(&g_queueLock);
    g_persistedVariants.swap(variants);
    ReleaseSRWLockExclusive(&g_queueLock);
    // The worker's first pass plans with these.
    Wh_Log(L"Restored %zu request contract(s) for startup warm-up: %ls", count, stored);
    return stored;
}

// Worker only: remember real contracts, writing only when they change.
void PersistVariants(std::wstring& persistedText) {
    std::vector<Request> recent;
    AcquireSRWLockShared(&g_queueLock);
    try {
        recent.assign(g_recentRequests.begin(), g_recentRequests.begin() + g_recentCount);
    } catch (...) {
        ReleaseSRWLockShared(&g_queueLock);
        throw;
    }
    ReleaseSRWLockShared(&g_queueLock);
    if (recent.empty()) return;
    const std::wstring text = FormatVariants(VariantsOf(recent));
    if (text == persistedText) return;
    if (Wh_SetStringValue(kVariantsValueName, text.c_str())) persistedText = text;
}

// The worker owns the notification window; message callbacks never run on an
// Explorer thread and no window-procedure pointer survives worker shutdown.
LRESULT CALLBACK WorkerWindowProc(HWND window, UINT message,
                                 WPARAM wParam, LPARAM lParam) {
    if (message == WM_DISPLAYCHANGE ||
        (message == WM_SETTINGCHANGE && wParam != SPI_SETDESKWALLPAPER)) {
        InvalidateCache();
    } else if (message == WM_SETTINGCHANGE) {
        ++g_sequence;
        if (g_workEvent) SetEvent(g_workEvent);
    } else if (message == WM_POWERBROADCAST) {
        if (wParam == PBT_APMSUSPEND) {
            g_suspended = true;
            InvalidateCache();
        } else if (wParam == PBT_APMRESUMEAUTOMATIC ||
                   wParam == PBT_APMRESUMESUSPEND ||
                   wParam == PBT_APMRESUMECRITICAL) {
            g_suspended = false;
            InvalidateCache();
        }
        return TRUE;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

// Run one STA with a message pump. Replan on every update and between files,
// retaining valid results completed during another switch.
DWORD WINAPI CacheWorker(void*) {
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(initialized)) {
        Wh_Log(L"Cache worker COM initialization failed: 0x%08lX",
               static_cast<unsigned long>(initialized));
        return 0;
    }
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&CacheWorker), &module);
    wchar_t className[96]{};
    swprintf_s(className, L"WindhawkDesktopThumbnailCache_%llu",
               static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(module)));
    WNDCLASSW wc{};
    wc.hInstance = module;
    wc.lpfnWndProc = WorkerWindowProc;
    wc.lpszClassName = className;
    const ATOM atom = RegisterClassW(&wc);
    HWND window = atom ? CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        className, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        module, nullptr) : nullptr;
    if (!window) {
        Wh_Log(L"Cache notification window unavailable; using the size fix only");
        if (atom) UnregisterClassW(className, module);
        CoUninitialize();
        return 0;
    }

    g_workerStartTick = GetTickCount64();
    std::wstring persistedText = LoadPersistedVariants();
    g_workerReady = true;
    struct Watch { HKEY key{}; HANDLE event{}; wchar_t path[256]{}; };
    std::array<Watch, 3> watches{};
    wcscpy_s(watches[0].path, kDesktopRoot);
    wcscpy_s(watches[1].path, L"Control Panel\\Desktop");
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    swprintf_s(watches[2].path,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops",
        session);
    IThumbnailCache* cache = nullptr;
    ULONGLONG clearedEpoch = g_epoch.load();
    ULONGLONG observedSequence = g_sequence.load();
    bool pending = true;
    DesktopHistory history;
    std::vector<Request> work;
    size_t nextRequest = 0;

    try {
        while (!g_stopping) {
            HANDLE handles[5] = {g_stopEvent, g_workEvent};
            DWORD handleCount = 2;
            for (auto& watch : watches) {
                if (!watch.key && RegOpenKeyExW(HKEY_CURRENT_USER, watch.path,
                    0, KEY_NOTIFY, &watch.key) == ERROR_SUCCESS) {
                    watch.event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                    if (!watch.event || RegNotifyChangeKeyValue(watch.key, TRUE,
                        REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
                        watch.event, TRUE) != ERROR_SUCCESS) {
                        if (watch.event) CloseHandle(watch.event);
                        RegCloseKey(watch.key);
                        watch.key = nullptr;
                        watch.event = nullptr;
                    }
                }
                if (watch.event) handles[handleCount++] = watch.event;
            }
            if (observedSequence != g_sequence.load()) {
                observedSequence = g_sequence.load();
                pending = true;
            }
            // With work queued, sleep until the quiet period after the most
            // recent transition request or desktop change has elapsed, and any
            // swipe in progress has finished. The cleanup hook wakes this wait.
            const ULONGLONG now = GetTickCount64();
            const ULONGLONG quietUntil = QuietUntil();
            // A ready-copy refill runs at once: it is a memory copy on this
            // low-priority thread, and the next swipe may come within half a
            // second. Refills are skipped while suspended, so they must not
            // shorten the wait then.
            const bool refillNow = g_spareRefill.load() && !g_suspended.load();
            const DWORD timeout = pending || refillNow ? 0 :
                (nextRequest < work.size() ?
                    static_cast<DWORD>(quietUntil > now ? quietUntil - now : 0) :
                    INFINITE);
            const DWORD wait = MsgWaitForMultipleObjectsEx(handleCount, handles,
                timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (wait == WAIT_OBJECT_0 || g_stopping) break;
            if (wait == WAIT_FAILED) break;
            if (wait >= WAIT_OBJECT_0 + 2 && wait < WAIT_OBJECT_0 + handleCount) {
                HANDLE triggered = handles[wait - WAIT_OBJECT_0];
                for (auto& watch : watches) {
                    if (watch.event == triggered) {
                        // Desktop-order/current-desktop changes schedule fresh work
                        // without discarding useful wallpaper images.
                        // Windows may rewrite the current wallpaper setting on
                        // every switch. Path/version keys already handle it.
                        if (RegNotifyChangeKeyValue(watch.key, TRUE,
                            REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
                            watch.event, TRUE) != ERROR_SUCCESS) {
                            RegCloseKey(watch.key);
                            CloseHandle(watch.event);
                            watch.key = nullptr;
                            watch.event = nullptr;
                        }
                        break;
                    }
                }
                // A desktop switch writes this state as it begins, possibly
                // before Explorer requests the transition wallpapers.
                NoteActivity();
                ++g_sequence;
            }
            MSG message{};
            // Bound each drain so a message stream cannot postpone useful work.
            for (int drained = 0; drained < 64 &&
                 PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE); ++drained) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            if (observedSequence != g_sequence.load()) {
                observedSequence = g_sequence.load();
                pending = true;
            }
            if (clearedEpoch != g_epoch.load()) {
                ClearCache();
                clearedEpoch = g_epoch.load();
                work.clear();
                nextRequest = 0;
            }
            // Planning is a few registry reads, so replan on every update.
            // Extraction itself waits for the quiet period, which absorbs the
            // burst of events a single switch produces.
            if (pending) {
                pending = false;
                work = BuildWorkList(history);
                nextRequest = 0;
                PersistVariants(persistedText);
            }
            if (g_suspended) {
                work.clear();
                nextRequest = 0;
                continue;
            }
            // Before extraction: the next hit needs the copy soon, while a new
            // image still waits for the quiet period anyway.
            if (g_spareRefill.load()) RefillSpares();
            const ULONGLONG sequence = g_sequence.load(), epoch = g_epoch.load();
            while (nextRequest < work.size()) {
                if (g_stopping || g_suspended ||
                    sequence != g_sequence || epoch != g_epoch) break;
                const auto& request = work[nextRequest];
                FileStamp before{};
                // Skip images already cached, and those known not to fit.
                if (!ReadFileStamp(request.path, before) || FindBitmap(request, before) ||
                    IsTooLarge(request, before)) {
                    ++nextRequest;
                    continue;
                }
                // Leave this request queued until foreground activity settles;
                // the outer wait sleeps for the remaining quiet period.
                if (GetTickCount64() < QuietUntil())
                    break;
                ++nextRequest;
                if (!cache && FAILED(CoCreateInstance(__uuidof(LocalThumbnailCache),
                    nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&cache)))) break;
                IShellItem* item = nullptr;
                if (FAILED(SHCreateItemFromParsingName(request.path.c_str(), nullptr,
                    IID_PPV_ARGS(&item)))) continue;
                ISharedBitmap* bitmap = nullptr;
                WTS_CACHEFLAGS outFlags{};
                WTS_THUMBNAILID id{};
                const auto start = std::chrono::steady_clock::now();
                const HRESULT hr = g_getThumbnailOriginal(cache, item, request.size,
                    request.flags, &bitmap, &outFlags, &id);
                item->Release();
                try {
                    FileStamp after{};
                    // A newer switch changes priority, not this image's validity.
                    if (SUCCEEDED(hr) && bitmap &&
                        epoch == g_epoch && ReadFileStamp(request.path, after) &&
                        before == after) {
                        const bool stored =
                            StoreBitmap(request, before, bitmap, outFlags, id, epoch);
                        if (stored) ++g_prepared;
                        Wh_Log(L"%ls thumbnail: %.3f ms, size=%u flags=0x%X",
                                stored ? L"Prepared" : L"Extracted but not cached",
                                std::chrono::duration<double, std::milli>(
                                    std::chrono::steady_clock::now() - start).count(),
                                request.size, request.flags);
                    }
                } catch (...) {
                    if (bitmap) bitmap->Release();
                    throw;
                }
                if (bitmap) bitmap->Release();
                // Return to event handling after one extraction. Useful pending
                // requests remain in the plan unless a newer plan replaces it.
                break;
            }
        }
    } catch (...) {
        Wh_Log(L"Cache worker stopped after an allocation failure");
    }
    g_workerReady = false;
    if (cache) cache->Release();
    for (auto& watch : watches) {
        if (watch.key) RegCloseKey(watch.key);
        if (watch.event) CloseHandle(watch.event);
    }
    DestroyWindow(window);
    UnregisterClassW(className, module);
    CoUninitialize();
    return 0;
}

// Keep routine settings atomic. A size-limit change invalidates entries and
// forgets observed requests, whose clamped size can no longer match. Other
// changes merely replan the queue.
void LoadSettings(bool initial) {
    // 0 disables the limit; other values are clamped to the supported range.
    const int sizeSetting = Wh_GetIntSetting(L"maximumThumbnailSize");
    const UINT maximumSize = sizeSetting <= 0 ? 0 :
        static_cast<UINT>(std::clamp(sizeSetting, 64, 1024));
    const bool sizeChanged = initial || maximumSize != g_maximumThumbnailSize.load();

    g_maximumThumbnailSize = maximumSize;
    // No artificial cap: the planner stops at half the desktop count, which
    // already covers every desktop, and the memory limit bounds the cache.
    g_neighbors = std::clamp(Wh_GetIntSetting(L"neighborsEachDirection"), 0, 1024);
    g_quietMs = static_cast<ULONGLONG>(
        std::clamp(Wh_GetIntSetting(L"prefetchQuietMs"), 0, 1000));
    g_lowLatencyBoostEnabled = !Wh_GetIntSetting(L"disableLowLatencyBoost");
    g_hiddenWindowFilterEnabled = !Wh_GetIntSetting(L"disableHiddenWindowFilter");
    const size_t limitBytes = static_cast<size_t>(
        std::clamp(Wh_GetIntSetting(L"cacheMemoryMb"), 16, 1024)) * 1024 * 1024;
    const bool limitChanged = limitBytes != g_cacheLimitBytes.load();
    g_cacheLimitBytes = limitBytes;

    if (sizeChanged) {
        AcquireSRWLockExclusive(&g_queueLock);
        for (auto& request : g_recentRequests) request = {};
        g_recentCount = 0;
        // Remembered contracts were clamped by the old limit.
        g_persistedVariants.clear();
        ReleaseSRWLockExclusive(&g_queueLock);
        InvalidateCache();
    } else if (limitChanged) {
        // Rebuild within the new limit rather than evicting piecemeal.
        InvalidateCache();
    } else {
        ++g_sequence;
        if (g_workEvent) SetEvent(g_workEvent);
    }
}

// Use the same private WinRT interface and boost type as Explorer's screen
// clipping experience. These interfaces are an optional feature on Windows 11.
void RequestLowLatencyBoost() {
    if (!g_lowLatencyBoostEnabled || g_stopping) {
        return;
    }

    // Some paths can nest or enter twice across monitors during one switch.
    constexpr ULONGLONG kMinimumRequestIntervalMs = 250;
    static std::atomic<ULONGLONG> lastRequest{0};
    const ULONGLONG now = GetTickCount64();
    ULONGLONG previous = lastRequest.load(std::memory_order_relaxed);
    if ((previous && (now <= previous || now - previous < kMinimumRequestIntervalMs)) ||
        !lastRequest.compare_exchange_strong(previous, now, std::memory_order_relaxed)) {
        return;
    }

    struct LowLatencyAction : IInspectable {
        virtual HRESULT STDMETHODCALLTYPE TriggerBoost(int boostType) = 0;
    };
    struct LowLatencyActionStatics : IInspectable {
        virtual HRESULT STDMETHODCALLTYPE Create(HSTRING name, LowLatencyAction** action) = 0;
    };
    constexpr GUID kLowLatencyActionStaticsId{0x4be1f989, 0xfd14, 0x593d,
        {0x96, 0x50, 0x16, 0xd2, 0x9a, 0x36, 0x80, 0xb7}};
    constexpr int kScreenClippingBoostType = 4;
    constexpr wchar_t kClassName[] = L"WindowsUdk.UI.Shell.LowLatencyAction";
    constexpr wchar_t kActionName[] = L"FasterVirtualDesktopSwitching";
    HSTRING_HEADER classNameHeader{}, actionNameHeader{};
    HSTRING className = nullptr, actionName = nullptr;
    if (FAILED(WindowsCreateStringReference(kClassName, ARRAYSIZE(kClassName) - 1,
                                            &classNameHeader, &className)) ||
        FAILED(WindowsCreateStringReference(kActionName, ARRAYSIZE(kActionName) - 1,
                                            &actionNameHeader, &actionName))) {
        return;
    }

    // Time each step: this runs on Explorer's UI thread at transition start.
    using Clock = std::chrono::steady_clock;
    auto elapsedMs = [](Clock::time_point from, Clock::time_point to) {
        return std::chrono::duration<double, std::milli>(to - from).count();
    };
    const auto start = Clock::now();
    auto factoryDone = start, createDone = start;
    LowLatencyActionStatics* factory = nullptr;
    HRESULT result = RoGetActivationFactory(className, kLowLatencyActionStaticsId,
                                            reinterpret_cast<void**>(&factory));
    factoryDone = Clock::now();
    createDone = factoryDone;
    if (SUCCEEDED(result) && factory) {
        LowLatencyAction* action = nullptr;
        result = factory->Create(actionName, &action);
        factory->Release();
        createDone = Clock::now();
        if (SUCCEEDED(result) && action) {
            result = action->TriggerBoost(kScreenClippingBoostType);
            action->Release();
        }
    }
    const auto end = Clock::now();
    Wh_Log(L"Desktop switch Low Latency Profile request: 0x%08lX, %.3f ms "
           L"(factory %.3f, create %.3f, trigger %.3f)",
           static_cast<unsigned long>(result), elapsedMs(start, end),
           elapsedMs(start, factoryDone), elapsedMs(factoryDone, createDone),
           elapsedMs(createDone, end));
}

HRESULT (*g_switchByHotkeyOriginal)(void*, int);
HRESULT (*g_switchWithAnimationOriginal)(void*, void*);
HRESULT (*g_initializeGestureOriginal)(void*, unsigned int);
void (*g_gestureCleanupOriginal)(void*, unsigned int);

// A transition start also counts as activity, so background extraction stays
// out of the way of the switch that is beginning. Keyboard and animated
// switches have no end signal here, so their quiet period also restarts when
// the synchronous part of the switch returns.
HRESULT SwitchByHotkeyHook(void* self, int direction) {
    NoteActivity();
    RequestLowLatencyBoost();
    const HRESULT hr = g_switchByHotkeyOriginal(self, direction);
    NoteActivity();
    return hr;
}

HRESULT SwitchWithAnimationHook(void* self, void* desktop) {
    NoteActivity();
    RequestLowLatencyBoost();
    const HRESULT hr = g_switchWithAnimationOriginal(self, desktop);
    NoteActivity();
    return hr;
}

// A swipe runs from here until its cleanup after the end animation. Without
// the cleanup hook there is no end signal, so the transition is not tracked.
HRESULT InitializeGestureHook(void* self, unsigned int operation) {
    NoteActivity();
    RequestLowLatencyBoost();
    const bool track = g_gestureCleanupOriginal != nullptr;
    if (track) {
        g_transitionStart = GetTickCount64();
        g_transitionActive = true;
    }
    g_initializingThread = GetCurrentThreadId();
    const HRESULT hr = g_initializeGestureOriginal(self, operation);
    g_initializingThread = 0;
    if (track && FAILED(hr)) g_transitionActive = false;
    return hr;
}

// Windows also calls this at the start of a gesture to discard the previous
// one; only a cleanup outside gesture initialization ends the transition.
void GestureCleanupHook(void* self, unsigned int reason) {
    g_gestureCleanupOriginal(self, reason);
    if (g_initializingThread.load() == GetCurrentThreadId()) return;
    g_transitionActive = false;
    // The quiet period now counts from the end of the animation.
    NoteActivity();
    if (g_workEvent) SetEvent(g_workEvent);
}

// Return true for the shell process, including early Explorer startup before
// its shell window exists. Separate folder processes are skipped once known.
bool IsOrMayBecomeShellExplorer() {
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        return true;
    }

    DWORD shellProcessId = 0;
    GetWindowThreadProcessId(shellWindow, &shellProcessId);
    return shellProcessId == GetCurrentProcessId();
}

// Obtain a function's exact primary code range from the x64 unwind table.
bool GetFunctionRange(void* function, uintptr_t& start, uintptr_t& end) {
    DWORD64 imageBase = 0;
    PRUNTIME_FUNCTION runtimeFunction = RtlLookupFunctionEntry(
        reinterpret_cast<DWORD64>(function), &imageBase, nullptr);
    if (!runtimeFunction || !imageBase) return false;
    const uintptr_t functionStart =
        static_cast<uintptr_t>(imageBase + runtimeFunction->BeginAddress);
    const uintptr_t functionEnd =
        static_cast<uintptr_t>(imageBase + runtimeFunction->EndAddress);
    const uintptr_t symbolAddress = reinterpret_cast<uintptr_t>(function);
    if (symbolAddress < functionStart || symbolAddress >= functionEnd ||
        functionEnd - functionStart > 0x4000) {
        return false;
    }
    start = functionStart;
    end = functionEnd;
    return true;
}

// Resolve the targets for the loaded Windows build, then obtain their exact
// code ranges without detouring either helper.
bool ResolveTargetFunctionRange() {
    g_twinuiPcShell = LoadLibraryExW(
        L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_twinuiPcShell) {
        Wh_Log(L"Failed to load twinui.pcshell.dll: %lu", GetLastError());
        return false;
    }

    void* targetFunction = nullptr;
    void* backgroundFunction = nullptr;
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                L"protected: long __cdecl VirtualDesktopGestureWindow::MakeBackgroundThumbnailFromThumbnailCache(struct IDCompThumbnail *,struct IVirtualDesktop *,struct HSTRING__ *,struct tagRECT,struct IDCompThumbnail * *)",
            },
            &targetFunction,
            nullptr,
            true,
        },
        {
            {
                L"protected: long __cdecl VirtualDesktopGestureWindow::MakeBackgroundThumbnail(struct IDCompThumbnail *,class CCoSimpleArray<class Microsoft::WRL::ComPtr<struct IDCompThumbnail>,4294967294,class CSimpleArrayStandardCompareHelper<class Microsoft::WRL::ComPtr<struct IDCompThumbnail> > > &,struct IDCompThumbnail * *)",
            },
            &backgroundFunction,
            nullptr,
            true,
        },
    };

    // Both are optional so either feature works without the other.
    if (!WindhawkUtils::HookSymbols(
            g_twinuiPcShell, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to resolve virtual desktop symbols");
        return false;
    }

    if (!backgroundFunction ||
        !GetFunctionRange(backgroundFunction, g_backgroundFunctionStart,
                          g_backgroundFunctionEnd)) {
        g_backgroundFunctionStart = g_backgroundFunctionEnd = 0;
        Wh_Log(L"Transition background helper unavailable; hidden window filter off");
    }

    if (!targetFunction) {
        Wh_Log(L"Failed to resolve the virtual desktop wallpaper helper");
        return false;
    }
    if (!GetFunctionRange(targetFunction, g_targetFunctionStart,
                          g_targetFunctionEnd)) {
        Wh_Log(L"Resolved an invalid virtual desktop helper range");
        return false;
    }
    return true;
}

// Independent of the thumbnail cache and its worker: the filter keeps no state.
bool HookHiddenWindowFilter() {
    if (!g_backgroundFunctionStart) return false;
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    void* enumWindows = user32 ?
        reinterpret_cast<void*>(GetProcAddress(user32, "EnumWindows")) : nullptr;
    const bool hooked = enumWindows &&
        Wh_SetFunctionHook(enumWindows, reinterpret_cast<void*>(EnumWindowsHook),
                           reinterpret_cast<void**>(&g_enumWindowsOriginal));
    if (!hooked) g_enumWindowsOriginal = nullptr;
    Wh_Log(L"Hidden window filter hook: %ls", hooked ? L"installed" : L"unavailable");
    return hooked;
}

// Activate the public thumbnail-cache COM class long enough to discover its
// implementation, then pin only the implementing DLL for the hook lifetime.
bool HookThumbnailCache() {
    const HRESULT initializeResult =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitializeCom = SUCCEEDED(initializeResult);
    if (FAILED(initializeResult) &&
        initializeResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"COM initialization failed: 0x%08lX",
               static_cast<unsigned long>(initializeResult));
        return false;
    }

    IThumbnailCache* cache = nullptr;
    const HRESULT activationResult = CoCreateInstance(
        __uuidof(LocalThumbnailCache),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IThumbnailCache),
        reinterpret_cast<void**>(&cache));
    if (FAILED(activationResult) || !cache) {
        Wh_Log(L"LocalThumbnailCache activation failed: 0x%08lX",
               static_cast<unsigned long>(activationResult));
        if (uninitializeCom) {
            CoUninitialize();
        }
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(cache);
    void* getThumbnailImplementation = vtable[3];
    const BOOL modulePinned = GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(getThumbnailImplementation),
        &g_thumbnailCacheServer);
    const DWORD modulePinError = modulePinned ? ERROR_SUCCESS : GetLastError();

    bool hookRegistered = false;
    if (modulePinned) {
        hookRegistered = Wh_SetFunctionHook(
            getThumbnailImplementation,
            reinterpret_cast<void*>(GetThumbnailHook),
            reinterpret_cast<void**>(&g_getThumbnailOriginal)) != FALSE;
    }

    cache->Release();
    if (uninitializeCom) {
        CoUninitialize();
    }

    if (!modulePinned) {
        Wh_Log(L"Failed to pin the thumbnail-cache implementation: %lu",
               modulePinError);
        return false;
    }

    if (!hookRegistered || !g_getThumbnailOriginal) {
        Wh_Log(L"Failed to hook IThumbnailCache::GetThumbnail");
        FreeLibrary(g_thumbnailCacheServer);
        g_thumbnailCacheServer = nullptr;
        return false;
    }

    return true;
}

// Balance local module references on every initialization failure. Hook
// removal is owned by Windhawk and occurs before Wh_ModUninit.
void ReleaseModuleReferences() {
    if (g_thumbnailCacheServer) {
        FreeLibrary(g_thumbnailCacheServer);
        g_thumbnailCacheServer = nullptr;
    }

    if (g_twinuiPcShell) {
        FreeLibrary(g_twinuiPcShell);
        g_twinuiPcShell = nullptr;
    }
}


}  // namespace

BOOL Wh_ModInit() {
    LoadSettings(true);
    if (!IsOrMayBecomeShellExplorer()) return TRUE;

    const bool thumbnailHookInstalled = ResolveTargetFunctionRange() && HookThumbnailCache();
    if (!g_twinuiPcShell) {
        ReleaseModuleReferences();
        return FALSE;
    }
    const bool filterHookInstalled = HookHiddenWindowFilter();

    WindhawkUtils::SYMBOL_HOOK boostHooks[] = {
        {{L"private: long __cdecl CVirtualDesktopHotkeyHandler::InvokeSwitchDesktopWithAnimation(enum VirtualDesktopSwitchDirection)"},
         &g_switchByHotkeyOriginal, SwitchByHotkeyHook, true},
        {{L"public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktopWithAnimation(struct IVirtualDesktop *)"},
         &g_switchWithAnimationOriginal, SwitchWithAnimationHook, true},
        {{L"private: long __cdecl VirtualDesktopGestureHandler::InitializeContext(unsigned int)"},
         &g_initializeGestureOriginal, InitializeGestureHook, true},
        {{L"public: virtual void __cdecl VirtualDesktopGestureHandler::Cleanup(unsigned int)"},
         &g_gestureCleanupOriginal, GestureCleanupHook, true},
    };
    WindhawkUtils::HookSymbols(g_twinuiPcShell, boostHooks, ARRAYSIZE(boostHooks));
    Wh_Log(L"Transition hooks: keyboard=%d animated=%d gesture=%d gesture-end=%d",
           g_switchByHotkeyOriginal != nullptr, g_switchWithAnimationOriginal != nullptr,
           g_initializeGestureOriginal != nullptr, g_gestureCleanupOriginal != nullptr);
    if (!thumbnailHookInstalled) {
        if (filterHookInstalled || g_switchByHotkeyOriginal ||
            g_switchWithAnimationOriginal || g_initializeGestureOriginal) {
            return TRUE;
        }
        ReleaseModuleReferences();
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_workEvent)
        Wh_Log(L"Background events unavailable; using the size fix only");
    return TRUE;
}

// Start only after Windhawk has applied the hook and its trampoline is ready.
void Wh_ModAfterInit() {
    if (!g_getThumbnailOriginal || !g_stopEvent || !g_workEvent) return;
    g_worker = CreateThread(nullptr, 0, CacheWorker, nullptr, 0, nullptr);
    if (!g_worker) Wh_Log(L"Cache worker creation failed: %lu", GetLastError());
}

void Wh_ModSettingsChanged() {
    LoadSettings(false);
}

// Join before hook removal so background extraction never calls a removed
// trampoline. Do not terminate a thread while Shell/COM code owns resources.
void Wh_ModBeforeUninit() {
    g_stopping = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_worker) WaitForSingleObject(g_worker, INFINITE);
}

void Wh_ModUninit() {
    ClearCache();
    if (g_worker) CloseHandle(g_worker);
    if (g_workEvent) CloseHandle(g_workEvent);
    if (g_stopEvent) CloseHandle(g_stopEvent);
    g_worker = g_workEvent = g_stopEvent = nullptr;
    Wh_Log(L"Cache totals: hits=%llu (ready copy %llu) misses=%llu prepared=%llu",
               g_hits.load(), g_spareHits.load(), g_misses.load(), g_prepared.load());
    Wh_Log(L"Hidden window filter totals: walks=%llu skipped=%llu",
           g_windowWalks.load(), g_windowsSkipped.load());
    ReleaseModuleReferences();
}

