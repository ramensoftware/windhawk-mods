// ==WindhawkMod==
// @id              macmotion
// @name            MacMotion Smooth Maximize
// @description     Smooth macOS-inspired maximize and unmaximize transitions without blocking the app UI thread.
// @version         2.0.1
// @author          Aayush
// @github          https://github.com/Aayushjoshi12
// @include         *
// @exclude         TextInputHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         dwm.exe
// @license         MIT
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacMotion Smooth Maximize

MacMotion adds a focused macOS-inspired transition when a normal desktop window
is maximized or restored from maximized state.

This catalog version intentionally implements only maximize/unmaximize. It does
not include minimize, restore-from-taskbar, close, launch, destruction, or
window-switch effects.

The implementation uses a DWM thumbnail and temporary window cloaking. The
animation loop runs on a worker thread, leaving the application's UI thread free
to process messages during the transition. The animation worker uses a
per-monitor-DPI-aware coordinate context, so physical screen coordinates stay
correct on scaled and mixed-DPI displays.

## Relationship to Windows Animations

MacMotion's earlier prototypes were derived in part from ReDrag's MIT-licensed
**Windows Animations** mod and Abdullah Masood's MIT-licensed
**macos-minimize-animation**. The current catalog build keeps the general
DWM-thumbnail/cloak technique for a maximize transition, but does not include
their minimize/close/window-switch renderers or preview assets. Credit is kept
here because those projects influenced the development of MacMotion.

Do not enable MacMotion and Windows Animations at the same time. Both mods hook
some of the same USER32 window-transition APIs, so running them together can
produce competing animation state.

## Features

- Smooth maximize animation.
- Smooth unmaximize/restore animation.
- Worker-thread rendering; the app UI thread is not held for the animation.
- Click-through animation surface, so the transition doesn't block input.
- Animation surface is limited to the affected window's start/end bounds.
- Mixed-DPI-safe physical coordinate handling.
- Adjustable duration.
- Safe unload: active workers exit and are joined before the mod unloads.
- Does not force-enable or force-disable the target window's native DWM
  transition preference, avoiding side effects on apps and other mods.

Disable the mod to immediately return to normal Windows maximize behavior.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maximize_animation: true
  $name: Animate maximize and unmaximize
  $description: Smoothly animate windows between normal and maximized bounds.
- maximize_duration_ms: 260
  $name: Animation duration (ms)
  $description: Duration of the maximize/unmaximize animation. Clamped between 120 and 700 ms.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <windhawk_utils.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>
#include <new>
#include <string_view>
#include <unordered_set>
#include <vector>

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using DefWindowProcA_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using SetWindowPlacement_t = BOOL(WINAPI*)(HWND, const WINDOWPLACEMENT*);

ShowWindow_t ShowWindow_Original = nullptr;
DefWindowProcW_t DefWindowProcW_Original = nullptr;
DefWindowProcA_t DefWindowProcA_Original = nullptr;
SetWindowPlacement_t SetWindowPlacement_Original = nullptr;

std::atomic<bool> g_maximizeAnimation{true};
std::atomic<int> g_maximizeDurationMs{260};
std::atomic<bool> g_unloading{false};

std::mutex g_stateMutex;
std::unordered_set<HWND> g_animatingWindows;

std::mutex g_workerMutex;
std::vector<HANDLE> g_workerThreads;

struct PendingResizeAnimation {
    HWND hWnd{};
    RECT fromRect{};
    int durationMs{};
    bool active{};
};

struct ResizeAnimationData {
    HWND hWnd{};
    RECT fromRect{};
    RECT toRect{};
    int durationMs{};
};

static constexpr std::wstring_view kExcludedClasses[] = {
    L"CoreWindow",
    L"ApplicationFrameWindow",
    L"XamlExplorerHostIslandWindow",
    L"Xaml_WindowedPopupClass",
    L"Popup",
    L"Overlay",
    L"ToolTip",
};

class ScopedPerMonitorDpiContext {
public:
    ScopedPerMonitorDpiContext() {
        previous_ = SetThreadDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    ~ScopedPerMonitorDpiContext() {
        if (previous_) {
            SetThreadDpiAwarenessContext(previous_);
        }
    }

    ScopedPerMonitorDpiContext(const ScopedPerMonitorDpiContext&) = delete;
    ScopedPerMonitorDpiContext& operator=(const ScopedPerMonitorDpiContext&) =
        delete;

private:
    DPI_AWARENESS_CONTEXT previous_{};
};

static bool IsExactExcludedClass(HWND hWnd) {
    wchar_t className[128]{};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    const std::wstring_view current(className);
    for (const auto excluded : kExcludedClasses) {
        if (current == excluded) {
            return true;
        }
    }
    return false;
}

static bool IsCurrentThreadWindow(HWND hWnd) {
    DWORD processId = 0;
    const DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
    return processId == GetCurrentProcessId() &&
           threadId == GetCurrentThreadId();
}

// GetWindowRect is virtualized for DPI-unaware callers. Pinning the calling
// thread to PMv2 makes the returned coordinates physical, matching the
// coordinate space used by the DWM thumbnail and the worker's ghost window.
static bool GetPhysicalWindowRect(HWND hWnd, RECT* rect) {
    if (!rect) {
        return false;
    }

    ScopedPerMonitorDpiContext dpiContext;
    return GetWindowRect(hWnd, rect) &&
           rect->right > rect->left && rect->bottom > rect->top;
}

static bool IsEligibleWindow(HWND hWnd) {
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd) || IsIconic(hWnd) ||
        GetAncestor(hWnd, GA_ROOT) != hWnd ||
        GetWindow(hWnd, GW_OWNER) != nullptr ||
        IsExactExcludedClass(hWnd)) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    if ((style & WS_CHILD) ||
        (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE))) {
        return false;
    }

    RECT rect{};
    if (!GetPhysicalWindowRect(hWnd, &rect)) {
        return false;
    }

    return (rect.right - rect.left) >= 160 &&
           (rect.bottom - rect.top) >= 100;
}

static bool SetWindowCloak(HWND hWnd, bool cloaked) {
    const BOOL value = cloaked ? TRUE : FALSE;
    return SUCCEEDED(
        DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &value, sizeof(value)));
}

static void RemoveAnimatingWindow(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_animatingWindows.erase(hWnd);
}

static bool ReserveAnimatingWindow(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    try {
        return g_animatingWindows.insert(hWnd).second;
    } catch (const std::bad_alloc&) {
        return false;
    }
}

static void RestoreRealWindow(HWND hWnd) {
    if (IsWindow(hWnd)) {
        SetWindowCloak(hWnd, false);
    }
    RemoveAnimatingWindow(hWnd);
}

static void CompactWorkerHandlesLocked() {
    auto it = g_workerThreads.begin();
    while (it != g_workerThreads.end()) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_workerThreads.erase(it);
        } else {
            ++it;
        }
    }
}

static void PumpWorkerThreadMessages() {
    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

static void DwmFlushWithFallback() {
    if (FAILED(DwmFlush())) {
        Sleep(1);
    }
}

static bool StartWorkerThread(LPTHREAD_START_ROUTINE startRoutine,
                              void* parameter) {
    std::lock_guard<std::mutex> lock(g_workerMutex);

    if (g_unloading.load(std::memory_order_acquire)) {
        return false;
    }

    CompactWorkerHandlesLocked();

    try {
        g_workerThreads.reserve(g_workerThreads.size() + 1);
    } catch (const std::bad_alloc&) {
        return false;
    }

    HANDLE thread =
        CreateThread(nullptr, 0, startRoutine, parameter, 0, nullptr);
    if (!thread) {
        return false;
    }

    g_workerThreads.push_back(thread);
    return true;
}

static void JoinWorkerThreads() {
    std::vector<HANDLE> workers;
    {
        std::lock_guard<std::mutex> lock(g_workerMutex);
        workers.swap(g_workerThreads);
    }

    for (HANDLE worker : workers) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
}

static float EaseInOutCubic(float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);
    if (progress < 0.5f) {
        return 4.0f * progress * progress * progress;
    }

    const float t = -2.0f * progress + 2.0f;
    return 1.0f - (t * t * t) / 2.0f;
}

static RECT UnionAnimationRects(const RECT& a, const RECT& b) {
    return {
        std::min(a.left, b.left),
        std::min(a.top, b.top),
        std::max(a.right, b.right),
        std::max(a.bottom, b.bottom),
    };
}

static RECT MakeLocalRect(const RECT& rect, const RECT& origin) {
    return {
        rect.left - origin.left,
        rect.top - origin.top,
        rect.right - origin.left,
        rect.bottom - origin.top,
    };
}

static RECT InterpolateRect(const RECT& fromRect,
                            const RECT& toRect,
                            float amount) {
    auto lerp = [amount](LONG from, LONG to) {
        return static_cast<LONG>(
            std::lround(from + (to - from) * amount));
    };

    return {
        lerp(fromRect.left, toRect.left),
        lerp(fromRect.top, toRect.top),
        lerp(fromRect.right, toRect.right),
        lerp(fromRect.bottom, toRect.bottom),
    };
}

static DWORD WINAPI ResizeAnimationThread(void* parameter) {
    auto* data = static_cast<ResizeAnimationData*>(parameter);
    const HWND hWnd = data->hWnd;
    const RECT fromRect = data->fromRect;
    const RECT toRect = data->toRect;
    const int durationMs = data->durationMs;
    delete data;

    ScopedPerMonitorDpiContext dpiContext;

    HWND ghost = nullptr;
    HTHUMBNAIL thumbnail = nullptr;

    const RECT ghostRect = UnionAnimationRects(fromRect, toRect);
    const int ghostWidth = ghostRect.right - ghostRect.left;
    const int ghostHeight = ghostRect.bottom - ghostRect.top;

    if (ghostWidth > 0 && ghostHeight > 0 &&
        IsWindow(hWnd) &&
        !g_unloading.load(std::memory_order_acquire)) {
        ghost = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                WS_EX_NOACTIVATE | WS_EX_TRANSPARENT |
                WS_EX_NOREDIRECTIONBITMAP,
            L"STATIC", nullptr, WS_POPUP,
            ghostRect.left, ghostRect.top, ghostWidth, ghostHeight,
            nullptr, nullptr, nullptr, nullptr);

        if (ghost) {
            const HRESULT registerResult =
                DwmRegisterThumbnail(ghost, hWnd, &thumbnail);
            if (SUCCEEDED(registerResult)) {
                DWM_THUMBNAIL_PROPERTIES properties{};
                properties.dwFlags =
                    DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION |
                    DWM_TNP_OPACITY;
                properties.fVisible = TRUE;
                properties.opacity = 255;
                properties.rcDestination =
                    MakeLocalRect(fromRect, ghostRect);

                if (SUCCEEDED(
                        DwmUpdateThumbnailProperties(thumbnail,
                                                     &properties))) {
                    ShowWindow_Original(ghost, SW_SHOWNOACTIVATE);
                    PumpWorkerThreadMessages();
                    DwmFlushWithFallback();

                    LARGE_INTEGER frequency{};
                    LARGE_INTEGER start{};
                    LARGE_INTEGER now{};
                    QueryPerformanceFrequency(&frequency);
                    QueryPerformanceCounter(&start);

                    for (;;) {
                        QueryPerformanceCounter(&now);
                        const double elapsedMs =
                            (now.QuadPart - start.QuadPart) * 1000.0 /
                            frequency.QuadPart;
                        const bool lastFrame =
                            elapsedMs >= durationMs ||
                            g_unloading.load(std::memory_order_acquire) ||
                            !IsWindow(hWnd);

                        const float progress =
                            lastFrame
                                ? 1.0f
                                : static_cast<float>(elapsedMs / durationMs);
                        const float eased = EaseInOutCubic(progress);
                        const RECT frame =
                            InterpolateRect(fromRect, toRect, eased);

                        properties.dwFlags =
                            DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION |
                            DWM_TNP_OPACITY;
                        properties.fVisible = TRUE;
                        properties.opacity = 255;
                        properties.rcDestination =
                            MakeLocalRect(frame, ghostRect);
                        const HRESULT updateResult =
                            DwmUpdateThumbnailProperties(thumbnail,
                                                         &properties);
                        PumpWorkerThreadMessages();

                        if (FAILED(updateResult) || lastFrame ||
                            !IsWindow(ghost)) {
                            break;
                        }

                        DwmFlushWithFallback();
                    }
                }
            }
        }
    }

    // Reveal the real window before removing the thumbnail/ghost. This avoids a
    // composition frame where neither representation is visible.
    RestoreRealWindow(hWnd);
    PumpWorkerThreadMessages();
    DwmFlushWithFallback();

    if (thumbnail) {
        DwmUnregisterThumbnail(thumbnail);
    }
    if (ghost) {
        DestroyWindow(ghost);
        PumpWorkerThreadMessages();
    }

    return 0;
}

static bool PrepareResizeAnimation(HWND hWnd,
                                   PendingResizeAnimation* pending) {
    if (!pending ||
        !g_maximizeAnimation.load(std::memory_order_relaxed) ||
        g_unloading.load(std::memory_order_acquire) ||
        !IsCurrentThreadWindow(hWnd) ||
        !IsEligibleWindow(hWnd)) {
        return false;
    }

    RECT fromRect{};
    if (!GetPhysicalWindowRect(hWnd, &fromRect) ||
        !ReserveAnimatingWindow(hWnd)) {
        return false;
    }

    if (!SetWindowCloak(hWnd, true)) {
        RemoveAnimatingWindow(hWnd);
        return false;
    }
    DwmFlushWithFallback();

    pending->hWnd = hWnd;
    pending->fromRect = fromRect;
    pending->durationMs =
        g_maximizeDurationMs.load(std::memory_order_relaxed);
    pending->active = true;
    return true;
}

static void CancelResizeAnimation(PendingResizeAnimation* pending) {
    if (!pending || !pending->active) {
        return;
    }

    pending->active = false;
    RestoreRealWindow(pending->hWnd);
}

static void CommitResizeAnimation(PendingResizeAnimation* pending,
                                  PCWSTR kind) {
    if (!pending || !pending->active) {
        return;
    }

    RECT toRect{};
    if (g_unloading.load(std::memory_order_acquire) ||
        !IsWindow(pending->hWnd) ||
        !GetPhysicalWindowRect(pending->hWnd, &toRect) ||
        EqualRect(&pending->fromRect, &toRect)) {
        CancelResizeAnimation(pending);
        return;
    }

    auto* data = new (std::nothrow) ResizeAnimationData{
        pending->hWnd,
        pending->fromRect,
        toRect,
        pending->durationMs,
    };

    if (!data || !StartWorkerThread(ResizeAnimationThread, data)) {
        delete data;
        CancelResizeAnimation(pending);
        return;
    }

    Wh_Log(L"%s animation started hwnd=%p duration=%d",
           kind, pending->hWnd, pending->durationMs);
    pending->active = false;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int command) {
    const bool maximize =
        command == SW_SHOWMAXIMIZED &&
        !IsZoomed(hWnd) && !IsIconic(hWnd);
    const bool unmaximize =
        (command == SW_RESTORE || command == SW_SHOWNORMAL) &&
        IsZoomed(hWnd) && !IsIconic(hWnd);

    PendingResizeAnimation pending{};
    if (maximize || unmaximize) {
        PrepareResizeAnimation(hWnd, &pending);
    }

    const BOOL result = ShowWindow_Original(hWnd, command);

    if (pending.active) {
        CommitResizeAnimation(
            &pending, maximize ? L"maximize" : L"unmaximize");
    }

    return result;
}

static LRESULT HandleDefWindowProcSysCommand(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT(WINAPI* original)(HWND, UINT, WPARAM, LPARAM)) {
    if (message == WM_SYSCOMMAND) {
        const UINT command = static_cast<UINT>(wParam) & 0xFFF0;
        const bool maximize =
            command == SC_MAXIMIZE &&
            !IsZoomed(hWnd) && !IsIconic(hWnd);
        const bool unmaximize =
            command == SC_RESTORE &&
            IsZoomed(hWnd) && !IsIconic(hWnd);

        if (maximize || unmaximize) {
            PendingResizeAnimation pending{};
            PrepareResizeAnimation(hWnd, &pending);

            const LRESULT result = original(hWnd, message, wParam, lParam);

            if (pending.active) {
                CommitResizeAnimation(
                    &pending,
                    maximize ? L"maximize" : L"unmaximize");
            }
            return result;
        }
    }

    return original(hWnd, message, wParam, lParam);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    return HandleDefWindowProcSysCommand(
        hWnd, message, wParam, lParam, DefWindowProcW_Original);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    return HandleDefWindowProcSysCommand(
        hWnd, message, wParam, lParam, DefWindowProcA_Original);
}

BOOL WINAPI SetWindowPlacement_Hook(
    HWND hWnd, const WINDOWPLACEMENT* placement) {
    if (!placement) {
        return SetWindowPlacement_Original(hWnd, placement);
    }

    const bool maximize =
        placement->showCmd == SW_SHOWMAXIMIZED &&
        !IsZoomed(hWnd) && !IsIconic(hWnd);
    const bool unmaximize =
        (placement->showCmd == SW_RESTORE ||
         placement->showCmd == SW_SHOWNORMAL) &&
        IsZoomed(hWnd) && !IsIconic(hWnd);

    PendingResizeAnimation pending{};
    if (maximize || unmaximize) {
        PrepareResizeAnimation(hWnd, &pending);
    }

    const BOOL result =
        SetWindowPlacement_Original(hWnd, placement);

    if (pending.active) {
        if (result) {
            CommitResizeAnimation(
                &pending, maximize ? L"maximize" : L"unmaximize");
        } else {
            CancelResizeAnimation(&pending);
        }
    }

    return result;
}

static void LoadSettings() {
    g_maximizeAnimation.store(
        Wh_GetIntSetting(L"maximize_animation") != 0,
        std::memory_order_relaxed);

    const int duration =
        std::clamp(Wh_GetIntSetting(L"maximize_duration_ms"),
                   120, 700);
    g_maximizeDurationMs.store(duration,
                              std::memory_order_relaxed);
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!g_maximizeAnimation.load(std::memory_order_relaxed)) {
        Wh_Log(L"Disabled by setting");
        return FALSE;
    }

    bool hooksInstalled = true;
    hooksInstalled &= WindhawkUtils::SetFunctionHook(
        ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    hooksInstalled &= WindhawkUtils::SetFunctionHook(
        DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original);
    hooksInstalled &= WindhawkUtils::SetFunctionHook(
        DefWindowProcA, DefWindowProcA_Hook, &DefWindowProcA_Original);
    hooksInstalled &= WindhawkUtils::SetFunctionHook(
        SetWindowPlacement, SetWindowPlacement_Hook,
        &SetWindowPlacement_Original);
    if (!hooksInstalled) {
        Wh_Log(L"Failed to install one or more hooks");
        return FALSE;
    }

    Wh_Log(L"Initialized duration=%d",
           g_maximizeDurationMs.load(std::memory_order_relaxed));
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings changed enabled=%d duration=%d",
           g_maximizeAnimation.load(std::memory_order_relaxed),
           g_maximizeDurationMs.load(std::memory_order_relaxed));
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);
    JoinWorkerThreads();
}

void Wh_ModUninit() {
    for (;;) {
        HWND hWnd = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            if (g_animatingWindows.empty()) {
                break;
            }

            const auto it = g_animatingWindows.begin();
            hWnd = *it;
            g_animatingWindows.erase(it);
        }

        if (IsWindow(hWnd)) {
            SetWindowCloak(hWnd, false);
        }
    }

    Wh_Log(L"Unloaded");
}
