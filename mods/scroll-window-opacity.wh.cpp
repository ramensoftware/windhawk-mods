// ==WindhawkMod==
// @id              scroll-window-opacity
// @name            Scroll Window Opacity
// @description     Hold a key combination and scroll the mouse wheel to change the opacity of the window under the cursor
// @version         1.0.1
// @author          Sondre Myrmel
// @github          https://github.com/Sondre234
// @twitter         https://x.com/ShaolinLoL
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Scroll Window Opacity

Hold a configurable key combination (default: **Ctrl + Alt**) and scroll the mouse
wheel to adjust the transparency of the top-level window under the cursor.

- **Scroll up** — decrease opacity (more transparent)
- **Scroll down** — increase opacity (more opaque)

The opacity step, minimum opacity, and modifier keys are all configurable in the
settings panel.

Common shell windows such as the taskbar and desktop are excluded. For windows
that this mod makes layered, the original window style is restored when returned
to 100% opacity.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKey:
  - value: ctrl+alt
    $name: Ctrl + Alt
  - value: ctrl+shift
    $name: Ctrl + Shift
  - value: alt+shift
    $name: Alt + Shift
  - value: ctrl
    $name: Ctrl only
  - value: alt
    $name: Alt only
  - value: shift
    $name: Shift only
  $name: Modifier Key(s)
  $description: Key(s) to hold while scrolling the mouse wheel to change opacity

- opacityStep: 2
  $name: Opacity Step (%)
  $description: How much opacity changes per scroll tick (1–50)

- minOpacity: 10
  $name: Minimum Opacity (%)
  $description: Lowest opacity a window can be set to (1–90)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <unordered_map>
#include <mutex>
#include <string>

// ── Settings ─────────────────────────────────────────────────────────────────

struct Settings {
    bool needCtrl  = true;
    bool needAlt   = true;
    bool needShift = false;
    int  step      = 2;   // percent per tick
    int  minPct    = 10;  // minimum opacity percent
} g_settings;

void LoadSettings() {
    PCWSTR raw = Wh_GetStringSetting(L"modifierKey");
    std::wstring key = (raw && raw[0] != L'\0') ? raw : L"ctrl+alt";
    Wh_FreeStringSetting(raw);

    g_settings.needCtrl  = key.find(L"ctrl")  != std::wstring::npos;
    g_settings.needAlt   = key.find(L"alt")   != std::wstring::npos;
    g_settings.needShift = key.find(L"shift") != std::wstring::npos;

    // Safety: if no modifier was parsed, default to Ctrl+Alt to avoid
    // unconditionally intercepting all scroll events.
    if (!g_settings.needCtrl && !g_settings.needAlt && !g_settings.needShift) {
        g_settings.needCtrl = true;
        g_settings.needAlt  = true;
    }

    int step = Wh_GetIntSetting(L"opacityStep");
    g_settings.step = (step >= 1 && step <= 50) ? step : 2;

    int minPct = Wh_GetIntSetting(L"minOpacity");
    g_settings.minPct = (minPct >= 1 && minPct <= 90) ? minPct : 10;
}

// ── Per-window opacity tracking ───────────────────────────────────────────────

struct WindowState {
    int  pct;        // current opacity 1–100
    bool hadLayered; // whether WS_EX_LAYERED was already set before we touched it
};

std::unordered_map<HWND, WindowState> g_states;
std::mutex g_mutex;

// ── Helpers ───────────────────────────────────────────────────────────────────

static bool IsModifierHeld() {
    if (g_settings.needCtrl  && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
    if (g_settings.needAlt   && !(GetAsyncKeyState(VK_MENU)    & 0x8000)) return false;
    if (g_settings.needShift && !(GetAsyncKeyState(VK_SHIFT)   & 0x8000)) return false;
    return true;
}

static bool IsSystemWindow(HWND hwnd) {
    WCHAR cls[256] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return _wcsicmp(cls, L"Shell_TrayWnd")    == 0 ||
           _wcsicmp(cls, L"Progman")          == 0 ||
           _wcsicmp(cls, L"WorkerW")          == 0 ||
           _wcsicmp(cls, L"DV2ControlHost")   == 0 ||
           _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

// Returns the top-level (root) window at the given screen point.
static HWND GetRootWindowAt(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) return GetForegroundWindow();
    HWND root = GetAncestor(hwnd, GA_ROOT);
    return root ? root : hwnd;
}

// ── Core opacity adjustment (called from hook thread) ─────────────────────────

static bool AdjustOpacity(HWND hwnd, int delta) {
    if (!hwnd || !IsWindow(hwnd) || IsSystemWindow(hwnd)) return false;

    std::lock_guard<std::mutex> lock(g_mutex);

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    auto it = g_states.find(hwnd);
    if (it == g_states.end()) {
        // First time we touch this window — snapshot its current state.
        bool hadLayered = (exStyle & WS_EX_LAYERED) != 0;
        int  curPct     = 100;

        if (hadLayered) {
            BYTE  alpha = 255;
            DWORD flags = 0;
            if (GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) &&
                (flags & LWA_ALPHA)) {
                curPct = (int)((alpha * 100) / 255);
                if (curPct < 1)   curPct = 1;
                if (curPct > 100) curPct = 100;
            }
        }

        g_states[hwnd] = {curPct, hadLayered};
        it = g_states.find(hwnd);
    }

    int newPct = it->second.pct + delta;
    if (newPct < g_settings.minPct) newPct = g_settings.minPct;
    if (newPct > 100)               newPct = 100;

    it->second.pct = newPct;

    if (newPct >= 100 && !it->second.hadLayered) {
        // Fully opaque again and we originally added WS_EX_LAYERED — remove it.
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        g_states.erase(it);
        Wh_Log(L"Restored window 0x%p to full opacity", (void*)hwnd);
    } else {
        BYTE alpha = (BYTE)((newPct * 255) / 100);
        if (!(exStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        }
        if (!SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA)) {
            Wh_Log(L"SetLayeredWindowAttributes failed for 0x%p: %u", (void*)hwnd, GetLastError());
            return false;
        }
        Wh_Log(L"Window 0x%p opacity → %d%%", (void*)hwnd, newPct);
    }
    return true;
}

static void RestoreAllWindows() {
    std::lock_guard<std::mutex> lock(g_mutex);
    for (auto& [hwnd, state] : g_states) {
        if (!IsWindow(hwnd)) continue;
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (state.hadLayered) {
            // Window already had WS_EX_LAYERED — just restore alpha to 255.
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        } else {
            // We added WS_EX_LAYERED; remove it entirely.
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }
    }
    g_states.clear();
}

// ── Low-level mouse hook ──────────────────────────────────────────────────────

HHOOK  g_mouseHook    = nullptr;
HANDLE g_hookThread   = nullptr;
DWORD  g_hookThreadId = 0;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL && IsModifierHeld()) {
        auto* ms    = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        int wheelDelta = GET_WHEEL_DELTA_WPARAM(ms->mouseData);
        int delta      = (wheelDelta > 0) ? -g_settings.step : g_settings.step;

        HWND target = GetRootWindowAt(ms->pt);
        if (AdjustOpacity(target, delta)) {
            return 1; // swallow only when opacity was actually changed
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID) {
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        return 1;
    }
    Wh_Log(L"Low-level mouse hook installed");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;
    Wh_Log(L"Low-level mouse hook removed");
    return 0;
}

// ── Windhawk entry points ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"Scroll Window Opacity: init");
    LoadSettings();

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Scroll Window Opacity: settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Scroll Window Opacity: uninit");

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread   = nullptr;
        g_hookThreadId = 0;
    }

    RestoreAllWindows();
}
