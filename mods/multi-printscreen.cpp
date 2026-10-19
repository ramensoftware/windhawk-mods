// ==WindhawkMod==
// @id              multi-printscreen
// @name            Multi PrintScreen
// @name:id-ID      Multi PrintScreen
// @description     Paste several screenshots one by one with a repeated Ctrl+V (FIFO). Works in every app. No Win+V menu, no AutoHotkey.
// @description:id-ID 🇮🇩 Tempel beberapa screenshot satu per satu hanya dengan Ctrl+V berulang (FIFO). Berlaku di semua aplikasi. Tanpa menu Win+V, tanpa AutoHotkey.
// @version         1.0.0
// @author          @sharexza
// @github          https://github.com/raviadityasetiawan-oss
// @include         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Multi PrintScreen

Take several screenshots in a row (Win+Shift+S, PrintScreen, ...), then press
**Ctrl+V** repeatedly in a target app: the **first** screenshot is pasted first,
the **second** on the next Ctrl+V, and so on (First In, First Out).

How it works (short version):
* A hidden window in explorer.exe listens for clipboard changes
  (`AddClipboardFormatListener`) and copies every new screenshot as raw bytes
  (CF_DIB / PNG) into a private FIFO queue. No GDI handles are ever stored.
* A low-level keyboard hook swallows Ctrl+V (in any application) when the
  queue is not empty, puts the oldest screenshot onto
  the real clipboard, then re-sends a marked V keystroke so the app pastes it.
* When the queue is empty, Ctrl+V behaves completely normally.
* Copying anything that is not an image (text, files, ...) clears the queue,
  so Ctrl+V never pastes a stale screenshot over what you just copied.

Only explorer.exe is injected. Nothing is injected into the apps you paste into,
so there is nothing to marshal across processes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxQueue: 10
  $name: Maximum queued screenshots
  $description: Between 1 and 50. If exceeded, the oldest screenshot is dropped.
- expireSeconds: 600
  $name: Queue expiry (seconds)
  $description: Screenshots older than this are discarded. 0 = never expire.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <deque>
#include <string>
#include <utility>
#include <vector>

namespace {

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
constexpr UINT kMsgPaste = WM_APP + 1;
constexpr ULONG_PTR kInjectMarker = 0x53585A41;  // "SXZA": tags our own SendInput events
constexpr SIZE_T kMaxFormatBytes = 256ull * 1024 * 1024;
constexpr ULONGLONG kDuplicateWindowMs = 1500;
constexpr wchar_t kWndClass[] = L"WH_SeqScreenshotPaste_Window";
constexpr wchar_t kMutexName[] = L"Local\\WH_SeqScreenshotPaste_SingleInstance";

// ----------------------------------------------------------------------------
// Data model: a queue item is a set of raw clipboard payloads (plain bytes).
// ----------------------------------------------------------------------------
struct FormatBlob {
    UINT fmt;
    std::vector<BYTE> data;
};

struct ClipItem {
    std::vector<FormatBlob> blobs;
    ULONGLONG tickMs = 0;
    uint64_t hash = 0;
};

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
CRITICAL_SECTION g_cs;  // guards g_queue, g_maxQueue, g_expireSec
std::deque<ClipItem> g_queue;
int g_maxQueue = 10;
int g_expireSec = 600;

std::atomic<HWND> g_clipWnd{nullptr};
HANDLE g_clipThread = nullptr;
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
HANDLE g_instanceMutex = nullptr;
HHOOK g_hook = nullptr;
UINT g_fmtPng = 0;
bool g_active = false;

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
        Wh_Log(L"Queue cleared (%d item(s) discarded)",
               static_cast<int>(g_queue.size()));
        g_queue.clear();
    }
}

void LoadSettings() {
    int maxQ = Wh_GetIntSetting(L"maxQueue");
    int expire = Wh_GetIntSetting(L"expireSeconds");

    if (maxQ < 1) maxQ = 10;
    if (maxQ > 50) maxQ = 50;
    if (expire < 0) expire = 0;

    Lock l;
    g_maxQueue = maxQ;
    g_expireSec = expire;
    while (static_cast<int>(g_queue.size()) > g_maxQueue) g_queue.pop_front();
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
    if (size == 0 || size > kMaxFormatBytes) return false;
    const BYTE* p = static_cast<const BYTE*>(GlobalLock(h));
    if (!p) return false;
    out.assign(p, p + size);
    GlobalUnlock(h);
    return true;
}

// Snapshot the current clipboard image as plain bytes.
// CF_DIB is preferred (universally supported, no alpha surprises); CF_DIBV5 is
// only used when CF_DIB is missing. PNG is added when present (best for
// Chromium/Electron apps and keeps alpha). CF_BITMAP is never stored: Windows
// synthesizes it from the DIB on demand, and an HBITMAP is only valid while
// the clipboard is open.
bool CaptureClipboard(HWND owner, ClipItem& out) {
    if (!OpenClipboardRetry(owner)) return false;

    std::vector<BYTE> buf;
    if (IsClipboardFormatAvailable(CF_DIB) && ReadFormat(CF_DIB, buf)) {
        out.blobs.push_back(FormatBlob{CF_DIB, std::move(buf)});
    } else {
        buf.clear();
        if (IsClipboardFormatAvailable(CF_DIBV5) && ReadFormat(CF_DIBV5, buf)) {
            out.blobs.push_back(FormatBlob{CF_DIBV5, std::move(buf)});
        }
    }

    if (g_fmtPng && IsClipboardFormatAvailable(g_fmtPng)) {
        std::vector<BYTE> png;
        if (ReadFormat(g_fmtPng, png)) {
            out.blobs.push_back(FormatBlob{g_fmtPng, std::move(png)});
        }
    }

    CloseClipboard();

    if (out.blobs.empty()) return false;
    out.hash = HashBytes(out.blobs.front().data);
    out.tickMs = GetTickCount64();
    return true;
}

// Put an item on the real clipboard. Runs on the clipboard thread, with our
// own window as clipboard owner (used to recognise our own updates).
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
            any = true;  // clipboard now owns h
        } else {
            GlobalFree(h);
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

    const bool hasImage = IsClipboardFormatAvailable(CF_DIB) ||
                          IsClipboardFormatAvailable(CF_DIBV5) ||
                          (g_fmtPng && IsClipboardFormatAvailable(g_fmtPng));

    if (!hasImage) {
        // The user copied something else (text, files, ...). Drop stale
        // screenshots so Ctrl+V pastes what was copied last. An empty clipboard
        // (transient clear) does not count.
        if (CountClipboardFormats() > 0) ClearQueue();
        return;
    }

    ClipItem item;
    if (!CaptureClipboard(hwnd, item)) {
        Wh_Log(L"Could not capture the clipboard image (clipboard busy?)");
        return;
    }

    // Some programs fire more than one update for a single copy.
    if (g_haveLast && item.hash == g_lastHash &&
        item.tickMs - g_lastTick < kDuplicateWindowMs) {
        return;
    }
    g_haveLast = true;
    g_lastHash = item.hash;
    g_lastTick = item.tickMs;

    Lock l;
    g_queue.push_back(std::move(item));
    while (static_cast<int>(g_queue.size()) > g_maxQueue) {
        Wh_Log(L"Queue full: dropping the oldest screenshot");
        g_queue.pop_front();
    }
    Wh_Log(L"Screenshot queued (%d in queue)", static_cast<int>(g_queue.size()));
}

void OnPaste(HWND hwnd) {
    ClipItem item;
    bool have = false;
    {
        Lock l;
        PurgeExpiredLocked();
        if (!g_queue.empty()) {
            item = std::move(g_queue.front());
            g_queue.pop_front();
            have = true;
        }
    }

    if (have) {
        if (!RestoreToClipboard(hwnd, item)) {
            // Do not lose the screenshot: put it back at the head of the queue.
            Wh_Log(L"Restore failed, re-queueing the screenshot");
            Lock l;
            g_queue.push_front(std::move(item));
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
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
    if (g_clipThread) {
        HWND w = g_clipWnd.load();
        if (w) PostMessageW(w, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_clipThread, 5000);
        CloseHandle(g_clipThread);
        g_clipThread = nullptr;
    }
}

}  // namespace

// ----------------------------------------------------------------------------
// Windhawk entry points
// ----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // Several explorer.exe processes can exist; only the first one may act.
    g_instanceMutex = CreateMutexW(nullptr, FALSE, kMutexName);
    if (g_instanceMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Another explorer.exe instance already runs this mod; staying idle");
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
        return TRUE;
    }

    InitializeCriticalSection(&g_cs);
    LoadSettings();
    g_fmtPng = RegisterClipboardFormatW(L"PNG");
    g_swallowedV = false;
    g_haveLast = false;

    HANDLE clipReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!clipReady || !hookReady) {
        if (clipReady) CloseHandle(clipReady);
        if (hookReady) CloseHandle(hookReady);
        DeleteCriticalSection(&g_cs);
        if (g_instanceMutex) { CloseHandle(g_instanceMutex); g_instanceMutex = nullptr; }
        return FALSE;
    }

    g_clipThread = CreateThread(nullptr, 0, ClipThreadProc, clipReady, 0, nullptr);
    if (g_clipThread) WaitForSingleObject(clipReady, 3000);

    if (g_clipThread && g_clipWnd.load()) {
        g_hookThread = CreateThread(nullptr, 0, HookThreadProc, hookReady, 0, &g_hookThreadId);
        if (g_hookThread) WaitForSingleObject(hookReady, 3000);
    }

    CloseHandle(clipReady);
    CloseHandle(hookReady);

    if (!g_clipThread || !g_clipWnd.load() || !g_hookThread || !g_hook) {
        Wh_Log(L"Initialisation failed, rolling back");
        StopThreads();
        DeleteCriticalSection(&g_cs);
        g_queue.clear();
        if (g_instanceMutex) { CloseHandle(g_instanceMutex); g_instanceMutex = nullptr; }
        return FALSE;
    }

    g_active = true;
    Wh_Log(L"Active");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_active) LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_active) {
        StopThreads();  // hook first, then the clipboard window
        g_queue.clear();
        DeleteCriticalSection(&g_cs);
        g_active = false;
    }
    if (g_instanceMutex) {
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
    }
}
