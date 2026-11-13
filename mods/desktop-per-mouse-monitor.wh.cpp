// ==WindhawkMod==
// @id              desktop-per-mouse-monitor
// @name            Show Desktop on only Mouse Monitor win10
// @github          https://github.com/AbdAllateefDaief
// @name:ar         win10 إظهار سطح المكتب على شاشة الماوس فقط
// @description     Toggle real minimization on the monitor under the mouse, preserving placement.
// @version         1.1.0
// @author          AbdAllateefDaief
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# إظهار سطح المكتب على شاشة الماوس

اضغط Win + D لتصغير نوافذ الشاشة التي عليها الماوس فقط. اضغطه مجدداً
على الشاشة نفسها لإرجاع النوافذ إلى مواضعها وحالتها العادية أو المكبرة.
لكل شاشة حالة مستقلة. النوافذ المصغرة سابقاً لا تدخل في العملية.

## التثبيت
1. أوقف سكربت AutoHotkey المرفق وأي إضافة أخرى تعدّل Win + D.
2. في Windhawk اختر Create a new mod، واستبدل الكود بهذا الملف كاملاً.
3. اضغط Compile Mod، ثم فعّل الإضافة إذا لم تتفعّل تلقائياً.

## خيارات
يمكن تعطيل غطاء لقطة الشاشة أو زيادة مدة انتظار النوافذ في Settings.
الغطاء مؤقت وفي الذاكرة فقط؛ لا تُحفظ صور على القرص.
إعداد حركات التصغير يُعطّل مؤقتاً أثناء العملية ويُعاد بعدها.

## حدود النسخة
مخصص لويندوز 10/11 بمعمارية x64. تعترض هذه النسخة رسالة Win + D
في Explorer مباشرة، ولا تعتمد على رموز أو دوال Explorer الداخلية.
الغطاء يقلل الوميض لكنه لا يضمن إزالته مع كل البرامج، خصوصاً الألعاب
بملء الشاشة والبرامج التي ترفض التصغير أو تعمل بصلاحيات أعلى.
النافذة الممتدة على شاشتين تُنسب إلى الشاشة التي يقع فيها مركزها.
النوافذ التي أُعيدت يدوياً تُترك كما هي عند استرجاع المجموعة.
عند تعطيل الإضافة بشكل طبيعي، تُستعاد النوافذ التي ما زالت مصغرة.
انهيار Explorer أو إنهاؤه قسراً قد يفقد حالة الاسترجاع؛ تبقى النوافذ
متاحة من شريط المهام. التعديل خاص باختصار Win + D؛ زر شريط المهام
وإيماءات لوحة اللمس يحتفظان بسلوك ويندوز الأصلي.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- screenShield: true
  $name: Use temporary screenshot shield
  $name:ar: استخدام غطاء مؤقت لتقليل الوميض
- disableAnimations: true
  $name: Temporarily disable minimize animations
  $name:ar: تعطيل حركات التصغير مؤقتاً
- settleMs: 250
  $name: Maximum window settling time (milliseconds, 50-1500)
  $name:ar: أقصى مدة انتظار للنوافذ بالميلي ثانية (50 إلى 1500)
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#include <algorithm>
#include <atomic>
#include <map>
#include <string>
#include <vector>

namespace {
constexpr UINT kToggle = WM_APP + 41;
constexpr UINT kStop = WM_APP + 42;
constexpr wchar_t kControlClass[] = L"WH_MouseMonitorDesktop_Control_1";
constexpr wchar_t kShieldClass[] = L"WH_MouseMonitorDesktop_Shield_1";

struct WindowState {
    HWND hwnd{};
    DWORD pid{}, tid{};
    WINDOWPLACEMENT placement{};
    bool topmost{};
    bool minimizedByUs{};
};
struct DesktopState {
    std::vector<WindowState> windows;
    HWND foreground{};
};
struct Runtime {
    // Only the worker thread accesses states or UI/GDI objects.
    std::map<std::wstring, DesktopState> states;
    std::atomic<bool> stopping{false}, queued{false};
    std::atomic<bool> shield{true}, animations{true};
    std::atomic<int> settleMs{250};
    HANDLE thread{}, ready{};
    DWORD threadId{};
    std::atomic<HWND> control{nullptr};
    HINSTANCE instance{};
};
Runtime* g{};
using DispatchMessageFn = LRESULT(WINAPI*)(const MSG*);
using PeekMessageFn = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
DispatchMessageFn DispatchMessageWOriginal{}, DispatchMessageAOriginal{};
PeekMessageFn PeekMessageWOriginal{}, PeekMessageAOriginal{};

class DpiScope {
    DPI_AWARENESS_CONTEXT previous{};
public:
    DpiScope() { previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); }
    ~DpiScope() { if (previous) SetThreadDpiAwarenessContext(previous); }
};

bool SameWindow(const WindowState& s) {
    DWORD pid{};
    DWORD tid = GetWindowThreadProcessId(s.hwnd, &pid);
    return IsWindow(s.hwnd) && pid == s.pid && tid == s.tid;
}

class AnimationScope {
    bool changed{};
public:
    AnimationScope(bool enabled) {
        ANIMATIONINFO info{sizeof(info), 0};
        if (enabled && SystemParametersInfoW(SPI_GETANIMATION, sizeof(info), &info, 0)
            && info.iMinAnimate) {
            info.iMinAnimate = 0;
            changed = SystemParametersInfoW(SPI_SETANIMATION, sizeof(info), &info, 0);
        }
    }
    ~AnimationScope() {
        if (changed) {
            ANIMATIONINFO current{sizeof(current), 0};
            if (SystemParametersInfoW(SPI_GETANIMATION, sizeof(current), &current, 0)
                && !current.iMinAnimate) {
                current.iMinAnimate = 1;
                SystemParametersInfoW(SPI_SETANIMATION, sizeof(current), &current, 0);
            }
        }
    }
};

LRESULT CALLBACK ShieldProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(hwnd, &paint);
        HBITMAP bitmap = reinterpret_cast<HBITMAP>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (bitmap) {
            HDC memory = CreateCompatibleDC(dc);
            if (memory) {
                HGDIOBJ old = SelectObject(memory, bitmap);
                BITMAP size{};
                GetObjectW(bitmap, sizeof(size), &size);
                BitBlt(dc, 0, 0, size.bmWidth, size.bmHeight, memory, 0, 0, SRCCOPY);
                SelectObject(memory, old);
                DeleteDC(memory);
            }
        }
        EndPaint(hwnd, &paint);
        return 0;
    }
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(hwnd, msg, wp, lp);
}

class ScreenShield {
    HWND hwnd{};
    HBITMAP bitmap{};
public:
    ScreenShield(HMONITOR monitor, bool enabled) {
        if (!enabled) return;
        MONITORINFO info{sizeof(info)};
        if (!GetMonitorInfoW(monitor, &info)) return;
        RECT r = info.rcWork;
        int w = r.right - r.left, h = r.bottom - r.top;
        if (w <= 0 || h <= 0) return;
        HDC screen = GetDC(nullptr);
        if (!screen) return;
        HDC memory = CreateCompatibleDC(screen);
        if (memory) {
            bitmap = CreateCompatibleBitmap(screen, w, h);
            if (bitmap) {
                HGDIOBJ old = SelectObject(memory, bitmap);
                BOOL copied = BitBlt(memory, 0, 0, w, h, screen, r.left, r.top, SRCCOPY | CAPTUREBLT);
                SelectObject(memory, old);
                if (!copied) { DeleteObject(bitmap); bitmap = nullptr; }
            }
            DeleteDC(memory);
        }
        ReleaseDC(nullptr, screen);
        if (!bitmap) return;
        hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kShieldClass, L"", WS_POPUP, r.left, r.top, w, h,
            nullptr, nullptr, g->instance, nullptr);
        if (!hwnd) return;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(bitmap));
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
        DwmFlush();
    }
    void Raise() {
        if (hwnd) SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }
    ~ScreenShield() {
        if (hwnd) DestroyWindow(hwnd);
        if (bitmap) DeleteObject(bitmap);
    }
};

struct CaptureContext { HMONITOR monitor; DesktopState* state; bool failed{}; };
BOOL CALLBACK CaptureWindow(HWND hwnd, LPARAM param) {
    auto& context = *reinterpret_cast<CaptureContext*>(param);
    try {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;
        LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (ex & WS_EX_TOOLWINDOW) return TRUE;
        wchar_t cls[256]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        for (PCWSTR excluded : {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"})
            if (wcscmp(cls, excluded) == 0) return TRUE;
        DWORD cloaked{};
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked)
            return TRUE;
        RECT r{};
        if (!GetWindowRect(hwnd, &r) || r.right <= r.left || r.bottom <= r.top) return TRUE;
        POINT center{r.left + (r.right - r.left) / 2, r.top + (r.bottom - r.top) / 2};
        if (MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST) != context.monitor) return TRUE;
        WindowState s{};
        s.hwnd = hwnd;
        s.tid = GetWindowThreadProcessId(hwnd, &s.pid);
        s.placement.length = sizeof(s.placement);
        if (!GetWindowPlacement(hwnd, &s.placement)) return TRUE;
        s.topmost = (ex & WS_EX_TOPMOST) != 0;
        context.state->windows.push_back(s);
    } catch (...) {
        // Exceptions must never unwind through a Win32 callback.
        context.failed = true;
        return FALSE;
    }
    return TRUE;
}

void WaitForWindows(DesktopState& state, bool minimized, ScreenShield& shield) {
    ULONGLONG deadline = GetTickCount64() + g->settleMs.load();
    for (;;) {
        bool done = true;
        for (auto& s : state.windows) {
            if (s.minimizedByUs && SameWindow(s) && bool(IsIconic(s.hwnd)) != minimized) {
                done = false;
                break;
            }
        }
        shield.Raise();
        if (done || GetTickCount64() >= deadline) break;
        Sleep(10); // Worker only; Explorer's shell thread never waits here.
    }
    shield.Raise();
    DwmFlush();
    DwmFlush();
}

bool Restore(DesktopState& state, HMONITOR monitor, bool useShield) {
    ScreenShield shield(monitor, useShield);
    AnimationScope animation(g->animations.load());
    std::vector<HWND> restored;
    restored.reserve(state.windows.size()); // Allocate before changing windows.
    for (auto it = state.windows.rbegin(); it != state.windows.rend(); ++it) {
        auto& s = *it;
        if (!s.minimizedByUs) continue;
        if (!SameWindow(s) || !IsIconic(s.hwnd)) { s.minimizedByUs = false; continue; }
        if (IsHungAppWindow(s.hwnd)) continue;
        WINDOWPLACEMENT placement = s.placement;
        // Complete each restoration before positioning the next window.
        placement.flags &= ~WPF_ASYNCWINDOWPLACEMENT;
        if (SetWindowPlacement(s.hwnd, &placement)) restored.push_back(s.hwnd);
    }
    WaitForWindows(state, false, shield);
    // Focus can change Z-order, so establish focus BEFORE rebuilding it.
    if (std::find(restored.begin(), restored.end(), state.foreground) != restored.end())
        SetForegroundWindow(state.foreground);
    // EnumWindows captured front-to-back. Raising in reverse order rebuilds
    // the relative order of this monitor's windows; other monitors' windows
    // receive no placement, minimization, or activation calls.
    for (auto it = state.windows.rbegin(); it != state.windows.rend(); ++it) {
        auto& s = *it;
        if (!SameWindow(s) || IsIconic(s.hwnd)
            || std::find(restored.begin(), restored.end(), s.hwnd) == restored.end()) continue;
        const UINT flags = SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING;
        if (!s.topmost && (GetWindowLongPtrW(s.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST))
            SetWindowPos(s.hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, flags);
        SetWindowPos(s.hwnd, s.topmost ? HWND_TOPMOST : HWND_TOP, 0, 0, 0, 0, flags);
        s.minimizedByUs = false;
        shield.Raise();
    }
    shield.Raise();
    DwmFlush();
    // Keep a failed restoration available for the next press.
    return std::none_of(state.windows.begin(), state.windows.end(),
        [](const WindowState& s) { return s.minimizedByUs && SameWindow(s) && IsIconic(s.hwnd); });
}

void RestoreAll() {
    for (auto& item : g->states) {
        try { Restore(item.second, nullptr, false); }
        catch (...) { Wh_Log(L"Failed to restore a saved monitor group"); }
    }
    g->states.clear();
}

void Toggle(HMONITOR monitor) {
    DpiScope dpi;
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, reinterpret_cast<MONITORINFO*>(&info))) return;
    std::wstring device(info.szDevice);
    auto saved = g->states.find(device);
    if (saved != g->states.end()) {
        Wh_Log(L"Restoring %llu windows on %s", static_cast<unsigned long long>(saved->second.windows.size()), device.c_str());
        if (Restore(saved->second, monitor, g->shield.load())) g->states.erase(saved);
        return;
    }
    DesktopState state{};
    state.foreground = GetForegroundWindow();
    CaptureContext context{monitor, &state};
    EnumWindows(CaptureWindow, reinterpret_cast<LPARAM>(&context));
    Wh_Log(L"Captured %llu windows on %s", static_cast<unsigned long long>(state.windows.size()), device.c_str());
    if (context.failed || state.windows.empty()) return;
    if (std::none_of(state.windows.begin(), state.windows.end(),
        [&](const WindowState& s) { return s.hwnd == state.foreground; }))
        state.foreground = nullptr; // Never activate an untouched monitor.
    auto [entry, inserted] = g->states.emplace(device, std::move(state));
    auto& captured = entry->second;
    ScreenShield shield(monitor, g->shield.load());
    AnimationScope animation(g->animations.load());
    for (auto& s : captured.windows) {
        if (SameWindow(s) && !IsHungAppWindow(s.hwnd) && !IsIconic(s.hwnd)) {
            ShowWindow(s.hwnd, SW_SHOWMINNOACTIVE);
            s.minimizedByUs = IsIconic(s.hwnd) != FALSE;
        }
    }
    WaitForWindows(captured, true, shield);
    if (std::none_of(captured.windows.begin(), captured.windows.end(),
        [](const WindowState& s) { return s.minimizedByUs; })) g->states.erase(entry);
}

LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == kToggle) {
        if (!g->stopping.load()) {
            try { Toggle(reinterpret_cast<HMONITOR>(wp)); }
            catch (...) { Wh_Log(L"Toggle failed; recovering saved windows"); RestoreAll(); }
        }
        g->queued.store(false);
        return 0;
    }
    if (msg == WM_DISPLAYCHANGE) { RestoreAll(); return 0; }
    if (msg == kStop) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI Worker(void*) {
    DpiScope dpi;
    WNDCLASSW shieldClass{};
    shieldClass.hInstance = g->instance;
    shieldClass.lpfnWndProc = ShieldProc;
    shieldClass.lpszClassName = kShieldClass;
    WNDCLASSW controlClass{};
    controlClass.hInstance = g->instance;
    controlClass.lpfnWndProc = ControlProc;
    controlClass.lpszClassName = kControlClass;
    bool shieldRegistered = RegisterClassW(&shieldClass) != 0;
    bool controlRegistered = RegisterClassW(&controlClass) != 0;
    if (shieldRegistered && controlRegistered) {
        // Hidden top-level window receives display-change notifications.
        g->control = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kControlClass,
            L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g->instance, nullptr);
    }
    SetEvent(g->ready);
    if (g->control) {
        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        RestoreAll();
        DestroyWindow(g->control.load());
        g->control.store(nullptr);
    }
    if (controlRegistered) UnregisterClassW(kControlClass, g->instance);
    if (shieldRegistered) UnregisterClassW(kShieldClass, g->instance);
    return 0;
}

void FilterHotkey(MSG* msg, bool removed) {
    if (!removed || !msg || msg->message != WM_HOTKEY || !g || g->stopping.load()) return;
    const UINT modifiers = LOWORD(msg->lParam) & (MOD_WIN | MOD_ALT | MOD_CONTROL | MOD_SHIFT);
    if (HIWORD(msg->lParam) != 'D' || modifiers != MOD_WIN) return;
    // The system already recognized the shortcut. Suppress its global action
    // and enqueue our per-monitor action instead, without a keyboard hook.
    HWND control = g->control.load();
    if (!control) return;
    if (!g->queued.exchange(true)) {
        DpiScope dpi;
        POINT mouse{};
        if (!GetCursorPos(&mouse)) { g->queued.store(false); return; }
        HMONITOR monitor = MonitorFromPoint(mouse, MONITOR_DEFAULTTONEAREST);
        if (!PostMessageW(control, kToggle, reinterpret_cast<WPARAM>(monitor), 0)) {
            g->queued.store(false);
            return;
        }
        Wh_Log(L"Win+D captured, target monitor=%p", monitor);
    }
    // Repeated hotkeys during a transition must not fall through to Windows.
    msg->message = WM_NULL;
    msg->wParam = 0;
    msg->lParam = 0;
}

LRESULT WINAPI DispatchMessageWHook(const MSG* msg) {
    if (msg && msg->message == WM_HOTKEY) {
        MSG copy = *msg;
        FilterHotkey(&copy, true);
        if (copy.message == WM_NULL) return 0;
    }
    return DispatchMessageWOriginal(msg);
}
LRESULT WINAPI DispatchMessageAHook(const MSG* msg) {
    if (msg && msg->message == WM_HOTKEY) {
        MSG copy = *msg;
        FilterHotkey(&copy, true);
        if (copy.message == WM_NULL) return 0;
    }
    return DispatchMessageAOriginal(msg);
}
BOOL WINAPI PeekMessageWHook(LPMSG msg, HWND hwnd, UINT first, UINT last, UINT flags) {
    BOOL result = PeekMessageWOriginal(msg, hwnd, first, last, flags);
    if (result) FilterHotkey(msg, (flags & PM_REMOVE) != 0);
    return result;
}
BOOL WINAPI PeekMessageAHook(LPMSG msg, HWND hwnd, UINT first, UINT last, UINT flags) {
    BOOL result = PeekMessageAOriginal(msg, hwnd, first, last, flags);
    if (result) FilterHotkey(msg, (flags & PM_REMOVE) != 0);
    return result;
}

void LoadSettings() {
    g->shield.store(Wh_GetIntSetting(L"screenShield") != 0);
    g->animations.store(Wh_GetIntSetting(L"disableAnimations") != 0);
    g->settleMs.store(std::clamp(Wh_GetIntSetting(L"settleMs"), 50, 1500));
}

void StopWorker() {
    g->stopping.store(true);
    if (g->thread) {
        HWND control = g->control.load();
        if (control && !PostMessageW(control, kStop, 0, 0))
            PostThreadMessageW(g->threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g->thread, INFINITE);
        CloseHandle(g->thread);
        g->thread = nullptr;
    }
}
} // namespace

BOOL Wh_ModInit() {
    try { g = new Runtime; } catch (...) { return FALSE; }
    HMODULE module{};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&Wh_ModInit), &module)) {
        delete g; g = nullptr; return FALSE;
    }
    g->instance = module;
    LoadSettings();
    g->ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g->ready) g->thread = CreateThread(nullptr, 0, Worker, nullptr, 0, &g->threadId);
    if (!g->thread) {
        if (g->ready) CloseHandle(g->ready);
        delete g; g = nullptr; return FALSE;
    }
    WaitForSingleObject(g->ready, INFINITE);
    CloseHandle(g->ready);
    g->ready = nullptr;
    if (!g->control) { StopWorker(); delete g; g = nullptr; return FALSE; }
    // Documented exported User32 APIs; no Explorer private symbol dependency.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    struct Hook { PCSTR name; void* replacement; void** original; };
    Hook hooks[] = {
        {"DispatchMessageW", reinterpret_cast<void*>(DispatchMessageWHook), reinterpret_cast<void**>(&DispatchMessageWOriginal)},
        {"DispatchMessageA", reinterpret_cast<void*>(DispatchMessageAHook), reinterpret_cast<void**>(&DispatchMessageAOriginal)},
        {"PeekMessageW", reinterpret_cast<void*>(PeekMessageWHook), reinterpret_cast<void**>(&PeekMessageWOriginal)},
        {"PeekMessageA", reinterpret_cast<void*>(PeekMessageAHook), reinterpret_cast<void**>(&PeekMessageAOriginal)}
    };
    bool success = user32 != nullptr;
    for (const auto& hook : hooks) {
        if (!success) break;
        FARPROC target = GetProcAddress(user32, hook.name);
        success = target && Wh_SetFunctionHook(reinterpret_cast<void*>(target), hook.replacement, hook.original);
    }
    if (!success) {
        Wh_Log(L"Could not install the Win+D message hooks");
        StopWorker(); delete g; g = nullptr; return FALSE;
    }
    return TRUE;
}

void Wh_ModBeforeUninit() { if (g) g->stopping.store(true); }
void Wh_ModUninit() {
    // Hooks are removed before joining; no callback can access freed state.
    if (g) { StopWorker(); delete g; g = nullptr; }
}
void Wh_ModSettingsChanged() { if (g) LoadSettings(); }
