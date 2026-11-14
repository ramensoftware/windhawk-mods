// ==WindhawkMod==
// @id              multi-printscreen
// @name            Multi PrintScreen
// @name:id-ID      Multi PrintScreen
// @description     Paste several screenshots one by one with a repeated Ctrl+V (FIFO). Works in every app. No Win+V menu, no AutoHotkey.
// @description:id-ID 🇮🇩 Tempel beberapa screenshot satu per satu hanya dengan Ctrl+V berulang (FIFO). Berlaku di semua aplikasi. Tanpa menu Win+V, tanpa AutoHotkey.
// @version         1.0.2
// @author          sharexza
// @github          https://github.com/sharexza
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Multi PrintScreen

Take several screenshots in a row (Win+Shift+S, PrintScreen, ...), then press
**Ctrl+V** repeatedly in a target app: the **first** screenshot is pasted first,
the **second** on the next Ctrl+V, and so on (First In, First Out).

How it works (short version):
* A hidden window in a dedicated tool process listens for clipboard changes
  (`AddClipboardFormatListener`) and copies pure screenshot payloads
  (CF_DIB and PNG) into a private FIFO queue.
* Only pure screenshots are queued. Copying anything else (text, Excel cells,
  files, HTML) automatically clears the queue so Ctrl+V never interferes with
  normal copy/paste.
* A low-level keyboard hook swallows Ctrl+V (in any application) when the
  queue is not empty, puts the oldest screenshot onto the clipboard, then
  re-sends a marked V keystroke so the app pastes it.
* Windows Clipboard History (Win+V) duplicates are suppressed for re-injected
  pastes.
* When the queue is empty, Ctrl+V behaves completely normally.

Runs as a dedicated tool process in Windhawk. Nothing is injected into explorer.exe
or into the apps you paste into, so there is nothing to marshal across processes.

Memory limits:
* Total queue memory is capped at 128 MB (the oldest screenshots are dropped if
  this budget is exceeded). Individual image payloads above 32 MB are skipped.

Note: When pasting, the physical 'V' keystroke is swallowed while the image is
restored to the clipboard, then re-injected. Very fast typing immediately after
Ctrl+V may occasionally land before the pasted image.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxQueue: 10
  $name: Maximum queued screenshots
  $description: Between 1 and 50. If exceeded (or if the 128 MB memory budget is reached), the oldest screenshot is dropped. Images over 32 MB are skipped.
- expireSeconds: 600
  $name: Queue expiry (seconds)
  $description: Screenshots older than this are discarded. 0 = never expire.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <deque>
#include <new>
#include <utility>
#include <vector>

namespace {

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
constexpr UINT kMsgPaste = WM_APP + 1;
constexpr ULONG_PTR kInjectMarker = 0x53585A41;  // "SXZA": tags our own SendInput events
constexpr SIZE_T kMaxItemBytes = 32ull * 1024 * 1024;    // 32 MB max per format payload
constexpr SIZE_T kMaxQueueBytes = 128ull * 1024 * 1024;  // 128 MB total queue budget
constexpr ULONGLONG kDuplicateWindowMs = 1500;
constexpr wchar_t kWndClass[] = L"WH_SeqScreenshotPaste_Window";

// ----------------------------------------------------------------------------
// Data model: a queue item stores image payloads (CF_DIB and PNG) as raw bytes.
// ----------------------------------------------------------------------------
struct FormatBlob {
    UINT fmt = 0;
    std::vector<BYTE> data;
};

struct ClipItem {
    std::vector<FormatBlob> blobs;
    ULONGLONG tickMs = 0;
    uint64_t hash = 0;

    size_t byteSize() const {
        size_t total = 0;
        for (const auto& b : blobs) total += b.data.size();
        return total;
    }
};

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
CRITICAL_SECTION g_cs;  // guards g_queue, g_queueTotalBytes, g_maxQueue, g_expireSec
std::deque<ClipItem> g_queue;
size_t g_queueTotalBytes = 0;
int g_maxQueue = 10;
int g_expireSec = 600;

std::atomic<HWND> g_clipWnd{nullptr};
HANDLE g_clipThread = nullptr;
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
HHOOK g_hook = nullptr;
bool g_active = false;

UINT g_fmtPng = 0;
UINT g_fmtCanIncludeInHistory = 0;
UINT g_fmtCanUploadToCloud = 0;
UINT g_fmtExcludeMonitor = 0;
UINT g_fmtClipboardViewerIgnore = 0;

std::atomic<bool> g_swallowedV{false};
std::atomic<ULONGLONG> g_swallowTick{0};

// Only touched from the clipboard thread.
uint64_t g_lastHash = 0;
ULONGLONG g_lastTick = 0;
bool g_haveLast = false;

struct Lock {
    Lock() { EnterCriticalSection(&g_cs); }
    ~Lock() { LeaveCriticalSection(&g_cs); }
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
};

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------
HMODULE SelfModule() {
    HMODULE m = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&SelfModule), &m);
    return m;
}

uint64_t HashBytes(const std::vector<BYTE>& v) {
    uint64_t h = 1469598103934665603ULL;  // FNV-1a 64-bit
    for (BYTE b : v) {
        h ^= b;
        h *= 1099511628211ULL;
    }
    return h ^ static_cast<uint64_t>(v.size());
}

// Caller must hold the lock. The queue is ordered by time, so the front is
// always the oldest item.
void PurgeExpiredLocked() {
    if (g_expireSec <= 0) return;
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG limit = static_cast<ULONGLONG>(g_expireSec) * 1000ULL;
    while (!g_queue.empty() && now - g_queue.front().tickMs > limit) {
        g_queueTotalBytes -= g_queue.front().byteSize();
        g_queue.pop_front();
    }
}

// Caller must hold the lock. Ensures both item count and memory budget limits.
void EnforceBudgetLocked(size_t additionalBytes) {
    while (!g_queue.empty() &&
           (g_queueTotalBytes + additionalBytes > kMaxQueueBytes ||
            static_cast<int>(g_queue.size()) >= g_maxQueue)) {
        Wh_Log(L"Dropping oldest screenshot (count=%d, total=%zu bytes)",
               static_cast<int>(g_queue.size()), g_queueTotalBytes);
        g_queueTotalBytes -= g_queue.front().byteSize();
        g_queue.pop_front();
    }
}

bool QueueHasItems() {
    Lock l;
    PurgeExpiredLocked();
    return !g_queue.empty();
}

void ClearQueue() {
    Lock l;
    if (!g_queue.empty()) {
        Wh_Log(L"Queue cleared (%d item(s), %zu bytes discarded)",
               static_cast<int>(g_queue.size()), g_queueTotalBytes);
        g_queue.clear();
        g_queueTotalBytes = 0;
    }
}

void LoadSettings() {
    int maxQ = Wh_GetIntSetting(L"maxQueue");
    int expire = Wh_GetIntSetting(L"expireSeconds");

    if (maxQ < 1) maxQ = 1;
    if (maxQ > 50) maxQ = 50;
    if (expire < 0) expire = 0;

    Lock l;
    g_maxQueue = maxQ;
    g_expireSec = expire;
    while (static_cast<int>(g_queue.size()) > g_maxQueue) {
        g_queueTotalBytes -= g_queue.front().byteSize();
        g_queue.pop_front();
    }
}

// The clipboard may be briefly locked by the program that just wrote to it.
bool OpenClipboardRetry(HWND owner) {
    for (int i = 0; i < 20; i++) {
        if (OpenClipboard(owner)) return true;
        Sleep(15);
    }
    return false;
}

// Copies the payload of one clipboard format into our own memory.
// Must be called while the clipboard is open.
bool ReadFormat(UINT fmt, std::vector<BYTE>& out) {
    HANDLE h = GetClipboardData(fmt);
    if (!h) return false;
    const SIZE_T size = GlobalSize(h);
    if (size == 0 || size > kMaxItemBytes) return false;
    const BYTE* p = static_cast<const BYTE*>(GlobalLock(h));
    if (!p) return false;

    bool ok = false;
    try {
        out.assign(p, p + size);
        ok = true;
    } catch (const std::bad_alloc&) {
        Wh_Log(L"Memory allocation failed in ReadFormat (%zu bytes)", size);
        out.clear();
        ok = false;
    }

    GlobalUnlock(h);
    return ok;
}

// Identifies known metadata/helper formats that screenshot tools or Windows
// produce alongside the image, so we know it's not text, HTML, or native app data.
bool IsKnownScreenshotMetadataFormat(UINT fmt) {
    if (fmt == g_fmtCanIncludeInHistory ||
        fmt == g_fmtCanUploadToCloud ||
        fmt == g_fmtExcludeMonitor ||
        fmt == g_fmtClipboardViewerIgnore) {
        return true;
    }

    WCHAR name[256];
    if (GetClipboardFormatNameW(fmt, name, ARRAYSIZE(name)) > 0) {
        if (_wcsicmp(name, L"image/png") == 0) {
            return true;
        }
    }
    return false;
}

// Clipboard must be open. Synthesized formats are enumerated too.
// Returns true ONLY if the clipboard exclusively contains image formats and
// recognized screenshot metadata (i.e. no text, HTML, Excel cells, files, etc.).
bool IsImageOnlyClipboard() {
    bool sawImage = false;
    for (UINT fmt = EnumClipboardFormats(0); fmt; fmt = EnumClipboardFormats(fmt)) {
        if (fmt == CF_BITMAP || fmt == CF_DIB || fmt == CF_DIBV5 ||
            fmt == CF_PALETTE || (g_fmtPng && fmt == g_fmtPng)) {
            sawImage = true;
        } else if (!IsKnownScreenshotMetadataFormat(fmt)) {
            return false;  // text, HTML, native app data, files, ...
        }
    }
    return sawImage;
}

// Checks if the source application requested that this clipboard content be
// excluded from history/monitoring (e.g. password managers or sensitive data).
bool IsContentExcluded() {
    if (g_fmtExcludeMonitor && IsClipboardFormatAvailable(g_fmtExcludeMonitor)) {
        return true;
    }
    if (g_fmtClipboardViewerIgnore && IsClipboardFormatAvailable(g_fmtClipboardViewerIgnore)) {
        return true;
    }
    if (g_fmtCanIncludeInHistory && IsClipboardFormatAvailable(g_fmtCanIncludeInHistory)) {
        HANDLE h = GetClipboardData(g_fmtCanIncludeInHistory);
        if (h && GlobalSize(h) >= sizeof(DWORD)) {
            const DWORD* val = static_cast<const DWORD*>(GlobalLock(h));
            if (val) {
                bool excluded = (*val == 0);
                GlobalUnlock(h);
                if (excluded) return true;
            }
        }
    }
    return false;
}

// Snapshot the current clipboard image as plain bytes.
// Stores CF_DIB (universal compatibility with classic Paint, WordPad, Win32 apps),
// plus PNG when present (preserves alpha and clarity in modern apps).
bool CaptureClipboard(ClipItem& out) {
    // Clipboard is already open by caller.
    std::vector<BYTE> dib;
    if (IsClipboardFormatAvailable(CF_DIB) && ReadFormat(CF_DIB, dib)) {
        out.blobs.push_back(FormatBlob{CF_DIB, std::move(dib)});
    } else {
        std::vector<BYTE> dibv5;
        if (IsClipboardFormatAvailable(CF_DIBV5) && ReadFormat(CF_DIBV5, dibv5)) {
            out.blobs.push_back(FormatBlob{CF_DIBV5, std::move(dibv5)});
        }
    }

    if (g_fmtPng && IsClipboardFormatAvailable(g_fmtPng)) {
        std::vector<BYTE> png;
        if (ReadFormat(g_fmtPng, png)) {
            out.blobs.push_back(FormatBlob{g_fmtPng, std::move(png)});
        }
    }

    if (out.blobs.empty()) return false;

    out.hash = HashBytes(out.blobs.front().data);
    out.tickMs = GetTickCount64();
    return true;
}

// Put an item on the real clipboard. Runs on the clipboard thread, with our
// own window as clipboard owner (used to recognise our own updates).
// Also suppresses duplicate entry creation in Windows Clipboard History (Win+V).
bool RestoreToClipboard(HWND owner, const ClipItem& item) {
    if (!OpenClipboardRetry(owner)) return false;
    EmptyClipboard();

    bool any = false;
    for (const auto& b : item.blobs) {
        HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, b.data.size());
        if (!h) continue;
        void* p = GlobalLock(h);
        if (!p) {
            GlobalFree(h);
            continue;
        }
        std::memcpy(p, b.data.data(), b.data.size());
        GlobalUnlock(h);

        if (SetClipboardData(b.fmt, h)) {
            any = true;
        } else {
            GlobalFree(h);
        }
    }

    // Suppress Win+V clipboard history duplicates for re-injected pastes
    if (g_fmtCanIncludeInHistory) {
        HGLOBAL hHist = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hHist) {
            DWORD* pVal = static_cast<DWORD*>(GlobalLock(hHist));
            if (pVal) {
                *pVal = 0;  // 0 = do not include in history
                GlobalUnlock(hHist);
                if (!SetClipboardData(g_fmtCanIncludeInHistory, hHist)) {
                    GlobalFree(hHist);
                }
            } else {
                GlobalFree(hHist);
            }
        }
    }
    if (g_fmtExcludeMonitor) {
        HGLOBAL hEx = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hEx) {
            DWORD* pVal = static_cast<DWORD*>(GlobalLock(hEx));
            if (pVal) {
                *pVal = 0;
                GlobalUnlock(hEx);
                if (!SetClipboardData(g_fmtExcludeMonitor, hEx)) {
                    GlobalFree(hEx);
                }
            } else {
                GlobalFree(hEx);
            }
        }
    }

    CloseClipboard();
    return any;
}

void SendPasteKeystroke() {
    INPUT in[4] = {};
    int n = 0;
    const bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

    auto add = [&](WORD vk, bool up) {
        in[n].type = INPUT_KEYBOARD;
        in[n].ki.wVk = vk;
        in[n].ki.wScan = static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
        in[n].ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        in[n].ki.dwExtraInfo = kInjectMarker;
        n++;
    };

    // Normally the user is still holding Ctrl, so only V is injected.
    if (!ctrlDown) add(VK_CONTROL, false);
    add('V', false);
    add('V', true);
    if (!ctrlDown) add(VK_CONTROL, true);

    SendInput(static_cast<UINT>(n), in, sizeof(INPUT));
}

// ----------------------------------------------------------------------------
// Clipboard thread: listener window + paste worker
// ----------------------------------------------------------------------------
void OnClipboardUpdate(HWND hwnd) {
    // Our own RestoreToClipboard() also triggers this notification: ignore it.
    if (GetClipboardOwner() == hwnd) return;

    if (!OpenClipboardRetry(hwnd)) {
        // Clipboard couldn't be opened: clear stale queue so Ctrl+V doesn't
        // paste an old screenshot over new uncaptured content.
        ClearQueue();
        return;
    }

    // Check if clipboard contains exclusively image data.
    if (!IsImageOnlyClipboard()) {
        const bool hasFormats = (EnumClipboardFormats(0) != 0);
        CloseClipboard();
        // The user copied something else (text, Excel cells, files, HTML, ...).
        // Drop stale screenshots so Ctrl+V pastes what was copied last.
        // An empty clipboard (transient clear) does not count.
        if (hasFormats) ClearQueue();
        return;
    }

    if (IsContentExcluded()) {
        CloseClipboard();
        ClearQueue();
        return;
    }

    ClipItem item;
    const bool captured = CaptureClipboard(item);
    CloseClipboard();

    if (!captured) {
        Wh_Log(L"Could not capture the clipboard image (too large or read failed)");
        ClearQueue();
        return;
    }

    // Some programs (like Snipping Tool or repeated triggers) fire multiple
    // updates for a single copy.
    if (g_haveLast && item.hash == g_lastHash &&
        item.tickMs - g_lastTick < kDuplicateWindowMs) {
        return;
    }
    g_haveLast = true;
    g_lastHash = item.hash;
    g_lastTick = item.tickMs;

    try {
        Lock l;
        const size_t bytes = item.byteSize();
        EnforceBudgetLocked(bytes);
        g_queue.push_back(std::move(item));
        g_queueTotalBytes += bytes;  // increment only after push_back succeeds
        Wh_Log(L"Screenshot queued (%d in queue, %zu bytes total)",
               static_cast<int>(g_queue.size()), g_queueTotalBytes);
    } catch (const std::bad_alloc&) {
        Wh_Log(L"Memory allocation failed while enqueuing screenshot");
    }
}

void OnPaste(HWND hwnd) {
    ClipItem item;
    bool have = false;
    {
        Lock l;
        PurgeExpiredLocked();
        if (!g_queue.empty()) {
            item = std::move(g_queue.front());
            g_queueTotalBytes -= item.byteSize();
            g_queue.pop_front();
            have = true;
        }
    }

    if (have) {
        if (!RestoreToClipboard(hwnd, item)) {
            // Do not lose the screenshot: put it back at the head of the queue.
            Wh_Log(L"Restore failed, re-queueing the screenshot");
            try {
                Lock l;
                const size_t bytes = item.byteSize();
                g_queue.push_front(std::move(item));
                g_queueTotalBytes += bytes;  // increment only after push_front succeeds
            } catch (const std::bad_alloc&) {
                Wh_Log(L"Memory allocation failed while re-queueing");
            }
        }
    }

    // Always re-send the paste keystroke (even if the queue was emptied by a
    // faster earlier press) so the user's Ctrl+V is never lost.
    SendPasteKeystroke();
}

LRESULT CALLBACK ClipWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLIPBOARDUPDATE:
            OnClipboardUpdate(hwnd);
            return 0;
        case kMsgPaste:
            OnPaste(hwnd);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI ClipThreadProc(LPVOID param) {
    HANDLE ready = static_cast<HANDLE>(param);
    HINSTANCE inst = SelfModule();

    WNDCLASSW wc = {};
    wc.lpfnWndProc = ClipWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = kWndClass;
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, kWndClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                                nullptr, inst, nullptr);
    if (!hwnd || !AddClipboardFormatListener(hwnd)) {
        Wh_Log(L"Failed to create the clipboard window/listener (error %lu)",
               GetLastError());
        if (hwnd) DestroyWindow(hwnd);
        UnregisterClassW(kWndClass, inst);
        SetEvent(ready);
        return 1;
    }

    g_clipWnd = hwnd;
    SetEvent(ready);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_clipWnd = nullptr;
    UnregisterClassW(kWndClass, inst);
    return 0;
}

// ----------------------------------------------------------------------------
// Hook thread: low-level keyboard hook (kept tiny and non-blocking)
// ----------------------------------------------------------------------------
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* k = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

        if (k->vkCode == 'V' && k->dwExtraInfo != kInjectMarker) {
            const bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            const bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

            if (isDown) {
                const ULONGLONG now = GetTickCount64();

                // Auto-repeat of a press we already swallowed: swallow it too,
                // but never longer than 1.5 s (protects against a lost key-up).
                if (g_swallowedV.load() && now - g_swallowTick.load() < 1500) {
                    return 1;
                }

                const bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
                const bool win = ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
                HWND target = g_clipWnd.load();

                if (ctrl && !shift && !alt && !win && target &&
                    QueueHasItems()) {
                    g_swallowedV = true;
                    g_swallowTick = now;
                    PostMessageW(target, kMsgPaste, 0, 0);  // non-blocking
                    return 1;  // swallow the physical Ctrl+V
                }
            } else if (isUp && g_swallowedV.exchange(false)) {
                return 1;  // swallow the matching key-up as well
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID param) {
    HANDLE ready = static_cast<HANDLE>(param);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the queue

    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, SelfModule(), 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed (error %lu)", GetLastError());
    }
    SetEvent(ready);
    if (!g_hook) return 1;

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_hook);
    g_hook = nullptr;
    return 0;
}

void StopThreads() {
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
    if (g_clipThread) {
        HWND w = g_clipWnd.load();
        if (w) PostMessageW(w, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_clipThread, INFINITE);
        CloseHandle(g_clipThread);
        g_clipThread = nullptr;
    }
}

}  // namespace

// ----------------------------------------------------------------------------
// Windhawk tool mod callbacks
// ----------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    Wh_Log(L"Tool Mod Init");

    InitializeCriticalSection(&g_cs);
    LoadSettings();
    g_fmtPng = RegisterClipboardFormatW(L"PNG");
    g_fmtCanIncludeInHistory = RegisterClipboardFormatW(L"CanIncludeInClipboardHistory");
    g_fmtCanUploadToCloud = RegisterClipboardFormatW(L"CanUploadToCloudClipboard");
    g_fmtExcludeMonitor = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    g_fmtClipboardViewerIgnore = RegisterClipboardFormatW(L"Clipboard Viewer Ignore");

    g_swallowedV = false;
    g_haveLast = false;
    g_queueTotalBytes = 0;

    HANDLE clipReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!clipReady || !hookReady) {
        if (clipReady) CloseHandle(clipReady);
        if (hookReady) CloseHandle(hookReady);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    g_clipThread = CreateThread(nullptr, 0, ClipThreadProc, clipReady, 0, nullptr);
    if (g_clipThread) WaitForSingleObject(clipReady, INFINITE);

    if (g_clipThread && g_clipWnd.load()) {
        g_hookThread = CreateThread(nullptr, 0, HookThreadProc, hookReady, 0, &g_hookThreadId);
        if (g_hookThread) WaitForSingleObject(hookReady, INFINITE);
    }

    CloseHandle(clipReady);
    CloseHandle(hookReady);

    if (!g_clipThread || !g_clipWnd.load() || !g_hookThread || !g_hook) {
        Wh_Log(L"Initialisation failed, rolling back");
        StopThreads();
        DeleteCriticalSection(&g_cs);
        g_queue.clear();
        g_queueTotalBytes = 0;
        return FALSE;
    }

    g_active = true;
    Wh_Log(L"Tool Mod Active");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_active) LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Tool Mod Uninit");
    if (g_active) {
        StopThreads();  // hook first, then the clipboard window
        g_queue.clear();
        g_queueTotalBytes = 0;
        DeleteCriticalSection(&g_cs);
        g_active = false;
    }
}

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
