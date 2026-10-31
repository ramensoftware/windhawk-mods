// ==WindhawkMod==
// @id              kwin-slide-back-only
// @name            KWin Slide Back Only
// @description     KWin-style Slide Back with DWM overlays, tray filtering, adjustable timing, and lossless latest-transition queuing.
// @version         1.1.4
// @author          Xianshu
// @github          https://github.com/tabris-trees
// @include         explorer.exe
// @license         MIT
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# KWin Slide Back Only v1.1

This build contains only Slide Back.

The important change from v1.0 is architectural:

- Real application HWNDs are NOT moved.
- When window B is raised, the windows that used to cover B are represented
  by live DWM thumbnail overlay windows.
- Those overlays remain visually above B while sliding away, so B is revealed
  progressively instead of appearing completely before the motion begins.
- Once an overlay no longer covers B, the overlays are placed behind B.
- They then return to their original positions behind B and are destroyed.
- Motion uses the same simple dynamics used by KWin WindowMotionManager:
    strength   = 0.12
    smoothness = 2.5

The foreground trigger is global EVENT_SYSTEM_FOREGROUND. Alt+Tab uses a
single ordered WinEvent stream with EVENT_SYSTEM_SWITCHSTART/END: it preserves
the last stable application Z-order at switch start and consumes it only when
the HWND identified by SWITCHEND becomes foreground.

Windows shell/tray surfaces (notification area overflow, Quick Settings,
Start/Search shell hosts, taskbar-owned popups, tooltips and menus) are
explicitly excluded so Slide Back applies to application windows rather than
system tray UI.

Rapid consecutive switches use a preemptible latest-pending-transition
mechanism. If A→B is still animating and the user switches to C, the A→B
ghosts are cancelled and cleaned up, while C is retained as the latest target.
If the user then switches to D, D replaces C. Once cleanup completes, only the
newest pending transition starts.

The mod is injected only into explorer.exe. Application processes, Tailscale,
proxy cores, Electron/X11 apps, Office, etc. are not injected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable Slide Back
- clearance_px: 20
  $name: Clearance (px)
  $description: Extra distance beyond the newly raised window.
- strength_percent: 100
  $name: Motion strength (%)
  $description: 100% corresponds to KWin's default strength 0.12.
- smoothness_percent: 100
  $name: Motion smoothness (%)
  $description: 100% corresponds to KWin's default smoothness 2.5.
- animation_duration_percent: 100
  $name: Animation duration (%)
  $description: 100% is the original KWin-like timing. 150% is about 1.5x slower/longer; 75% is faster/shorter.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#include <windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <new>
#include <utility>
#include <vector>

static constexpr wchar_t kGhostClassName[] =
    L"WindhawkKWinSlideBackGhostWindow";

static std::atomic<bool> g_enabled{true};
static std::atomic<int> g_clearancePx{20};
static std::atomic<double> g_strength{0.12};
static std::atomic<double> g_smoothness{2.5};
static std::atomic<double> g_animationTimeFactor{1.0};
static std::atomic<bool> g_unloading{false};

// A foreground switch must be able to preempt the animation that currently
// owns the ghost windows. An id avoids losing a cancellation request during a
// start/end race, which a single bool cannot represent safely.
static std::atomic<unsigned long long> g_nextAnimationId{1};
static std::atomic<unsigned long long> g_activeAnimationId{0};
static std::atomic<unsigned long long> g_cancelledAnimationId{0};

static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HWINEVENTHOOK g_systemEventHook = nullptr;
static ATOM g_ghostClassAtom = 0;

static std::mutex g_zOrderMutex;
static std::vector<HWND> g_previousZOrder;
static HWND g_previousForeground = nullptr;

// Alt+Tab temporarily foregrounds a shell-owned task switcher. Preserve the
// last stable application stack and consume it only for SWITCHEND's selected
// HWND. Keeping all three events in one WinEvent hook preserves their queued
// order, unlike separate foreground and task-switch hooks.
enum class AltTabState {
    None,
    Switching,
    AwaitingSelectedForeground,
};

static constexpr ULONGLONG kAltTabForegroundTimeoutMs = 1000;
static std::mutex g_altTabMutex;
static AltTabState g_altTabState = AltTabState::None;
static HWND g_altTabExpectedTarget = nullptr;
static ULONGLONG g_altTabForegroundDeadline = 0;
static std::vector<HWND> g_altTabPreviousOrder;

// Only the newest foreground change that occurs while an animation is active
// is retained. This avoids both recursive overlapping animations and the
// previous v1.1.x behavior where rapid transitions were simply discarded.
static constexpr UINT kMsgProcessPendingTransition = WM_APP + 0x2B1;
static std::mutex g_pendingMutex;
static bool g_hasPendingTransition = false;
static HWND g_pendingTarget = nullptr;
static std::vector<HWND> g_pendingPreviousOrder;

struct Vec2 {
    double x{};
    double y{};
};

struct Motion2D {
    Vec2 value{};
    Vec2 target{};
    Vec2 velocity{};
    double strength{0.12};
    double smoothness{2.5};

    void SetTarget(double x, double y) {
        target.x = x;
        target.y = y;
    }

    bool IsStopped() const {
        return std::abs(target.x - value.x) < 0.5 &&
               std::abs(target.y - value.y) < 0.5 &&
               std::abs(velocity.x) < 0.2 &&
               std::abs(velocity.y) < 0.2;
    }

    void Finish() {
        value = target;
        velocity = {};
    }

    void Calculate(int msec) {
        // This is intentionally the same "poor man's time independent"
        // integration used by KWin's Motion<T>::calculate().
        const int steps = std::max(1, msec / 5);

        for (int i = 0; i < steps; ++i) {
            const Vec2 diff{
                target.x - value.x,
                target.y - value.y};

            const Vec2 force{
                diff.x * strength,
                diff.y * strength};

            velocity.x =
                (smoothness * velocity.x + force.x) /
                (smoothness + 1.0);

            velocity.y =
                (smoothness * velocity.y + force.y) /
                (smoothness + 1.0);

            value.x += velocity.x;
            value.y += velocity.y;
        }

        if (IsStopped()) {
            Finish();
        }
    }
};

struct GhostWindow {
    HWND source{};
    HWND ghost{};
    HTHUMBNAIL thumbnail{};

    RECT original{};
    RECT displaced{};

    int width{};
    int height{};

    Motion2D motion{};
};

struct AnimationData {
    unsigned long long id{};
    HWND target{};
    RECT targetRect{};
    std::vector<HWND> coveringSources;
};

static int ClampInt(int value, int low, int high) {
    return value < low ? low : (value > high ? high : value);
}

static bool IsGhostWindow(HWND hWnd) {
    wchar_t className[128]{};

    return GetClassNameW(
               hWnd,
               className,
               ARRAYSIZE(className)) &&
           _wcsicmp(className, kGhostClassName) == 0;
}


static bool IsExcludedShellProcess(HWND hWnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);

    if (!processId) {
        return false;
    }

    HANDLE process =
        OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            processId);

    if (!process) {
        return false;
    }

    wchar_t path[MAX_PATH * 2]{};
    DWORD length = ARRAYSIZE(path);

    const BOOL ok =
        QueryFullProcessImageNameW(
            process,
            0,
            path,
            &length);

    CloseHandle(process);

    if (!ok || !length) {
        return false;
    }

    const wchar_t* base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;

    // These processes host Windows shell UI rather than ordinary application
    // windows. File Explorer itself remains allowed because explorer.exe is
    // intentionally not excluded here.
    static constexpr PCWSTR excludedProcesses[] = {
        L"ShellExperienceHost.exe",
        L"StartMenuExperienceHost.exe",
        L"SearchHost.exe",
        L"SearchApp.exe",
        L"Widgets.exe",
        L"WidgetService.exe",
        L"TextInputHost.exe",
        L"LockApp.exe",
    };

    for (PCWSTR value : excludedProcesses) {
        if (_wcsicmp(base, value) == 0) {
            return true;
        }
    }

    return false;
}

static bool IsExcludedShellClass(HWND hWnd) {
    wchar_t className[128]{};

    if (!GetClassNameW(
            hWnd,
            className,
            ARRAYSIZE(className))) {
        return false;
    }

    static constexpr PCWSTR excluded[] = {
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"NotifyIconOverflowWindow",
        L"TopLevelWindowForOverflowXamlIsland",
        L"ControlCenterWindow",
        L"SystemTray_Main",
        L"Progman",
        L"WorkerW",
        L"MultitaskingViewFrame",
        L"XamlExplorerHostIslandWindow",
        L"Xaml_WindowedPopupClass",
        L"tooltips_class32",
        L"ToolTip",
        L"#32768",
        L"IME",
        L"MSCTFIME UI",
    };

    for (PCWSTR value : excluded) {
        if (_wcsicmp(className, value) == 0) {
            return true;
        }
    }

    return false;
}

static bool IsCloaked(HWND hWnd) {
    DWORD cloaked = 0;

    return SUCCEEDED(
               DwmGetWindowAttribute(
                   hWnd,
                   DWMWA_CLOAKED,
                   &cloaked,
                   sizeof(cloaked))) &&
           cloaked != 0;
}

static bool IsTrackableTopLevelWindow(HWND hWnd) {
    if (!hWnd ||
        !IsWindow(hWnd) ||
        IsGhostWindow(hWnd)) {
        return false;
    }

    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    const LONG_PTR style =
        GetWindowLongPtrW(
            hWnd,
            GWL_STYLE);

    const LONG_PTR exStyle =
        GetWindowLongPtrW(
            hWnd,
            GWL_EXSTYLE);

    if (style & WS_CHILD) {
        return false;
    }

    if (exStyle & WS_EX_NOACTIVATE) {
        return false;
    }

    if (IsExcludedShellClass(hWnd) ||
        IsExcludedShellProcess(hWnd) ||
        IsCloaked(hWnd)) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hWnd, &rect)) {
        return false;
    }

    const LONG width =
        rect.right - rect.left;

    const LONG height =
        rect.bottom - rect.top;

    // Only helper-sized surfaces are rejected. Normal app/tool/dialog windows
    // from Electron, X11 forwarding, Office, Qt, Java, etc. remain eligible.
    return width >= 40 && height >= 40;
}

static bool IsUsableCoveringWindow(HWND hWnd) {
    if (!IsTrackableTopLevelWindow(hWnd) ||
        !IsWindowVisible(hWnd) ||
        IsIconic(hWnd)) {
        return false;
    }

    const LONG_PTR exStyle =
        GetWindowLongPtrW(
            hWnd,
            GWL_EXSTYLE);

    // Corresponds to KWin keepAbove(): don't animate always-on-top surfaces.
    if (exStyle & WS_EX_TOPMOST) {
        return false;
    }

    return true;
}

struct EnumContext {
    std::vector<HWND>* windows;
};

static BOOL CALLBACK EnumTopLevelProc(
    HWND hWnd,
    LPARAM lParam) {

    auto* context =
        reinterpret_cast<EnumContext*>(lParam);

    if (IsTrackableTopLevelWindow(hWnd)) {
        context->windows->push_back(hWnd);
    }

    return TRUE;
}

static std::vector<HWND> CaptureZOrder() {
    std::vector<HWND> windows;
    windows.reserve(48);

    EnumContext context{&windows};

    EnumWindows(
        EnumTopLevelProc,
        reinterpret_cast<LPARAM>(&context));

    // EnumWindows is top -> bottom.
    return windows;
}

static bool RectanglesOverlap(
    const RECT& a,
    const RECT& b) {

    RECT intersection{};

    return IntersectRect(
               &intersection,
               &a,
               &b) != FALSE &&
           intersection.right > intersection.left &&
           intersection.bottom > intersection.top;
}

static RECT ComputeSlideDestination(
    const RECT& windowUnder,
    const RECT& windowOver,
    int clearance) {

    const LONG leftSlide =
        windowUnder.left -
        windowOver.right -
        clearance;

    const LONG rightSlide =
        windowUnder.right -
        windowOver.left +
        clearance;

    const LONG upSlide =
        windowUnder.top -
        windowOver.bottom -
        clearance;

    const LONG downSlide =
        windowUnder.bottom -
        windowOver.top +
        clearance;

    const LONG horizontal =
        std::abs(leftSlide) <=
                std::abs(rightSlide)
            ? leftSlide
            : rightSlide;

    const LONG vertical =
        std::abs(upSlide) <=
                std::abs(downSlide)
            ? upSlide
            : downSlide;

    RECT result = windowOver;

    if (std::abs(horizontal) <
        std::abs(vertical)) {
        OffsetRect(
            &result,
            horizontal,
            0);
    } else {
        OffsetRect(
            &result,
            0,
            vertical);
    }

    return result;
}

static LRESULT CALLBACK GhostWndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {

    switch (message) {
    case WM_NCHITTEST:
        return HTTRANSPARENT;

    case WM_ERASEBKGND:
        return 1;

    default:
        return DefWindowProcW(
            hWnd,
            message,
            wParam,
            lParam);
    }
}

static bool RegisterGhostClass() {
    if (g_ghostClassAtom) {
        return true;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = GhostWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kGhostClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    g_ghostClassAtom =
        RegisterClassExW(&wc);

    if (!g_ghostClassAtom &&
        GetLastError() ==
            ERROR_CLASS_ALREADY_EXISTS) {
        return true;
    }

    return g_ghostClassAtom != 0;
}

static bool ConfigureThumbnail(
    GhostWindow& item) {

    if (!item.thumbnail ||
        !item.ghost) {
        return false;
    }

    RECT client{};
    if (!GetClientRect(
            item.ghost,
            &client)) {
        return false;
    }

    DWM_THUMBNAIL_PROPERTIES props{};

    props.dwFlags =
        DWM_TNP_RECTDESTINATION |
        DWM_TNP_OPACITY |
        DWM_TNP_VISIBLE |
        DWM_TNP_SOURCECLIENTAREAONLY;

    props.rcDestination = client;
    props.opacity = 255;
    props.fVisible = TRUE;
    props.fSourceClientAreaOnly = FALSE;

    return SUCCEEDED(
        DwmUpdateThumbnailProperties(
            item.thumbnail,
            &props));
}

static bool CreateGhostForSource(
    HWND source,
    HWND target,
    GhostWindow* out) {

    if (!out ||
        !IsUsableCoveringWindow(source)) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(
            source,
            &rect)) {
        return false;
    }

    const int width =
        static_cast<int>(
            rect.right - rect.left);

    const int height =
        static_cast<int>(
            rect.bottom - rect.top);

    if (width <= 0 ||
        height <= 0) {
        return false;
    }

    HWND ghost =
        CreateWindowExW(
            WS_EX_TOOLWINDOW |
                WS_EX_NOACTIVATE |
                WS_EX_TRANSPARENT,
            kGhostClassName,
            L"",
            WS_POPUP,
            rect.left,
            rect.top,
            width,
            height,
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr);

    if (!ghost) {
        return false;
    }

    HTHUMBNAIL thumbnail = nullptr;

    const HRESULT hr =
        DwmRegisterThumbnail(
            ghost,
            source,
            &thumbnail);

    if (FAILED(hr) ||
        !thumbnail) {
        DestroyWindow(ghost);
        return false;
    }

    GhostWindow item{};
    item.source = source;
    item.ghost = ghost;
    item.thumbnail = thumbnail;
    item.original = rect;
    item.width = width;
    item.height = height;

    item.motion.value = {
        static_cast<double>(rect.left),
        static_cast<double>(rect.top)};

    item.motion.target =
        item.motion.value;

    // KWin's WindowMotionManager applies the global animation-time factor
    // by weakening acceleration and increasing smoothness for longer timing.
    const double timeFactor =
        g_animationTimeFactor.load(
            std::memory_order_relaxed);

    item.motion.strength =
        g_strength.load(
            std::memory_order_relaxed) /
        timeFactor;

    item.motion.smoothness =
        g_smoothness.load(
            std::memory_order_relaxed) *
        timeFactor;

    if (!ConfigureThumbnail(item)) {
        DwmUnregisterThumbnail(
            item.thumbnail);

        DestroyWindow(
            item.ghost);

        return false;
    }

    // Show without activating. It will be placed in the correct visual order
    // as a separate step after all ghosts are created.
    SetWindowPos(
        item.ghost,
        HWND_TOP,
        rect.left,
        rect.top,
        width,
        height,
        SWP_NOACTIVATE |
            SWP_NOOWNERZORDER |
            SWP_SHOWWINDOW);

    *out = std::move(item);
    return true;
}

static void DestroyGhost(
    GhostWindow& item) {

    if (item.thumbnail) {
        DwmUnregisterThumbnail(
            item.thumbnail);

        item.thumbnail = nullptr;
    }

    if (item.ghost &&
        IsWindow(item.ghost)) {
        DestroyWindow(item.ghost);
    }

    item.ghost = nullptr;
}

static void DestroyGhosts(
    std::vector<GhostWindow>& ghosts) {

    for (auto& item : ghosts) {
        DestroyGhost(item);
    }

    ghosts.clear();
}

static void PutGhostsAboveTarget(
    std::vector<GhostWindow>& ghosts) {

    // ghosts[] is old top -> bottom. Push bottom -> top to HWND_TOP so the
    // final ghost order reproduces the old visual stacking above target.
    for (auto it = ghosts.rbegin();
         it != ghosts.rend();
         ++it) {

        if (!it->ghost ||
            !IsWindow(it->ghost)) {
            continue;
        }

        SetWindowPos(
            it->ghost,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_NOACTIVATE |
                SWP_NOOWNERZORDER |
                SWP_SHOWWINDOW);
    }
}

static void PutGhostsBehindTarget(
    HWND target,
    std::vector<GhostWindow>& ghosts) {

    if (!IsWindow(target)) {
        return;
    }

    // Preserve the old top->bottom order among ghosts, but put the whole
    // chain immediately behind B.
    HWND insertAfter = target;

    for (auto& item : ghosts) {
        if (!item.ghost ||
            !IsWindow(item.ghost)) {
            continue;
        }

        SetWindowPos(
            item.ghost,
            insertAfter,
            0,
            0,
            0,
            0,
            SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_NOACTIVATE |
                SWP_NOOWNERZORDER);

        insertAfter = item.ghost;
    }
}

static void UpdateGhostPositions(
    std::vector<GhostWindow>& ghosts) {

    for (auto& item : ghosts) {
        if (!item.ghost ||
            !IsWindow(item.ghost)) {
            continue;
        }

        SetWindowPos(
            item.ghost,
            nullptr,
            static_cast<int>(
                std::lround(
                    item.motion.value.x)),
            static_cast<int>(
                std::lround(
                    item.motion.value.y)),
            0,
            0,
            SWP_NOSIZE |
                SWP_NOZORDER |
                SWP_NOACTIVATE |
                SWP_NOOWNERZORDER);
    }
}

static bool AllMotionsStopped(
    const std::vector<GhostWindow>& ghosts) {

    for (const auto& item : ghosts) {
        if (!item.motion.IsStopped()) {
            return false;
        }
    }

    return true;
}

static bool StepMotions(
    std::vector<GhostWindow>& ghosts,
    int elapsedMs) {

    for (auto& item : ghosts) {
        item.motion.Calculate(
            elapsedMs);
    }

    UpdateGhostPositions(
        ghosts);

    DwmFlush();

    return AllMotionsStopped(
        ghosts);
}

static bool IsAnimationCancelled(
    unsigned long long animationId) {

    return g_unloading.load(
               std::memory_order_relaxed) ||
           g_cancelledAnimationId.load(
               std::memory_order_acquire) ==
               animationId;
}

static bool AnimateToTargets(
    std::vector<GhostWindow>& ghosts,
    unsigned long long animationId) {

    LARGE_INTEGER frequency{};
    LARGE_INTEGER previous{};

    if (!QueryPerformanceFrequency(
            &frequency) ||
        !QueryPerformanceCounter(
            &previous) ||
        frequency.QuadPart <= 0) {
        return false;
    }

    for (;;) {
        if (IsAnimationCancelled(
                animationId)) {
            return false;
        }

        LARGE_INTEGER now{};
        QueryPerformanceCounter(
            &now);

        int elapsedMs =
            static_cast<int>(
                (now.QuadPart -
                 previous.QuadPart) *
                1000 /
                frequency.QuadPart);

        if (elapsedMs <= 0) {
            Sleep(1);
            continue;
        }

        // Large stalls should not cause an enormous unstable integration step.
        elapsedMs =
            std::min(
                elapsedMs,
                50);

        previous = now;

        if (StepMotions(
                ghosts,
                elapsedMs)) {
            break;
        }

        Sleep(1);
    }

    return true;
}


static void ClearPendingTransition() {
    std::lock_guard<std::mutex>
        lock(g_pendingMutex);

    g_hasPendingTransition = false;
    g_pendingTarget = nullptr;
    g_pendingPreviousOrder.clear();
}

static void QueueLatestPendingTransition(
    HWND target,
    const std::vector<HWND>& previousOrder) {

    std::lock_guard<std::mutex>
        lock(g_pendingMutex);

    g_pendingTarget = target;
    g_pendingPreviousOrder = previousOrder;
    g_hasPendingTransition = true;
}

static bool TakePendingTransition(
    HWND* target,
    std::vector<HWND>* previousOrder) {

    if (!target ||
        !previousOrder) {
        return false;
    }

    std::lock_guard<std::mutex>
        lock(g_pendingMutex);

    if (!g_hasPendingTransition) {
        return false;
    }

    *target = g_pendingTarget;
    *previousOrder =
        std::move(
            g_pendingPreviousOrder);

    g_hasPendingTransition = false;
    g_pendingTarget = nullptr;
    g_pendingPreviousOrder.clear();

    return true;
}

static bool HasPendingTransition() {
    std::lock_guard<std::mutex>
        lock(g_pendingMutex);

    return g_hasPendingTransition;
}

static void RequestPendingTransitionProcessing() {
    if (g_unloading.load(
            std::memory_order_relaxed) ||
        !HasPendingTransition()) {
        return;
    }

    const DWORD threadId =
        g_hookThreadId;

    if (threadId) {
        PostThreadMessageW(
            threadId,
            kMsgProcessPendingTransition,
            0,
            0);
    }
}

static DWORD WINAPI AnimationThreadProc(
    LPVOID parameter) {

    std::unique_ptr<AnimationData> data(
        static_cast<AnimationData*>(
            parameter));

    struct AnimationGuard {
        unsigned long long id{};

        ~AnimationGuard() {
            unsigned long long expected = id;
            g_activeAnimationId.compare_exchange_strong(
                expected,
                0,
                std::memory_order_acq_rel);

            // Keep foreground/z-order transitions serialized on the hook
            // thread. At this point all ghosts have been destroyed.
            RequestPendingTransitionProcessing();
        }
    } animationGuard{data ? data->id : 0};

    if (!data ||
        IsAnimationCancelled(data->id) ||
        !IsWindow(data->target) ||
        data->coveringSources.empty()) {
        return 0;
    }

    if (!RegisterGhostClass()) {
        return 0;
    }

    std::vector<GhostWindow> ghosts;
    ghosts.reserve(
        data->coveringSources.size());

    const int clearance =
        g_clearancePx.load(
            std::memory_order_relaxed);

    for (HWND source :
         data->coveringSources) {

        if (IsAnimationCancelled(
                data->id)) {
            break;
        }

        GhostWindow item{};

        if (!CreateGhostForSource(
                source,
                data->target,
                &item)) {
            continue;
        }

        item.displaced =
            ComputeSlideDestination(
                data->targetRect,
                item.original,
                clearance);

        item.motion.SetTarget(
            static_cast<double>(
                item.displaced.left),
            static_cast<double>(
                item.displaced.top));

        ghosts.push_back(
            std::move(item));
    }

    if (ghosts.empty()) {
        return 0;
    }

    if (IsAnimationCancelled(data->id)) {
        DestroyGhosts(ghosts);
        return 0;
    }

    // B has already been raised logically by Windows. Put the live DWM
    // replicas of the old covering windows above it immediately, reconstructing
    // KWin's temporary elevation. Therefore B is revealed only as the ghosts
    // move out of the way.
    PutGhostsAboveTarget(
        ghosts);

    DwmFlush();

    // Outbound phase: KWin Motion dynamics toward the shortest clear position.
    const bool completedOutbound =
        AnimateToTargets(
            ghosts,
            data->id);

    if (completedOutbound &&
        !IsAnimationCancelled(data->id) &&
        IsWindow(data->target)) {

        // KWin removes the temporary elevation as soon as the covering
        // windows no longer intersect B. From this point onward the returning
        // windows are visually behind B.
        PutGhostsBehindTarget(
            data->target,
            ghosts);

        for (auto& item : ghosts) {
            item.motion.SetTarget(
                static_cast<double>(
                    item.original.left),
                static_cast<double>(
                    item.original.top));
        }

        DwmFlush();

        // Return phase: same motion model, now behind B.
        AnimateToTargets(
            ghosts,
            data->id);
    }

    DestroyGhosts(
        ghosts);

    return 0;
}

static void StartSlideBack(
    HWND target,
    const std::vector<HWND>& previousOrder) {

    if (!g_enabled.load(
            std::memory_order_relaxed) ||
        g_unloading.load(
            std::memory_order_relaxed) ||
        !IsTrackableTopLevelWindow(target) ||
        !IsWindowVisible(target) ||
        IsIconic(target)) {
        return;
    }

    auto targetIt =
        std::find(
            previousOrder.begin(),
            previousOrder.end(),
            target);

    // If the window didn't exist in the old stack, there is no old covering
    // relationship to reconstruct.
    if (targetIt ==
            previousOrder.end() ||
        targetIt ==
            previousOrder.begin()) {
        return;
    }

    RECT targetRect{};
    if (!GetWindowRect(
            target,
            &targetRect)) {
        return;
    }

    const HMONITOR targetMonitor =
        MonitorFromWindow(
            target,
            MONITOR_DEFAULTTONEAREST);

    std::vector<HWND> covering;
    covering.reserve(
        static_cast<size_t>(
            std::distance(
                previousOrder.begin(),
                targetIt)));

    for (auto it =
             previousOrder.begin();
         it != targetIt;
         ++it) {

        const HWND candidate =
            *it;

        if (candidate == target ||
            !IsUsableCoveringWindow(
                candidate)) {
            continue;
        }

        if (MonitorFromWindow(
                candidate,
                MONITOR_DEFAULTTONEAREST) !=
            targetMonitor) {
            continue;
        }

        RECT candidateRect{};

        if (!GetWindowRect(
                candidate,
                &candidateRect) ||
            !RectanglesOverlap(
                targetRect,
                candidateRect)) {
            continue;
        }

        covering.push_back(
            candidate);
    }

    if (covering.empty()) {
        return;
    }

    const unsigned long long animationId =
        g_nextAnimationId.fetch_add(
            1,
            std::memory_order_relaxed);

    unsigned long long expected = 0;

    if (!g_activeAnimationId
             .compare_exchange_strong(
                 expected,
                 animationId,
                 std::memory_order_acq_rel)) {
        return;
    }

    auto* data =
        new (std::nothrow)
            AnimationData{
                animationId,
                target,
                targetRect,
                std::move(covering)};

    if (!data) {
        g_activeAnimationId.store(
            0,
            std::memory_order_release);
        RequestPendingTransitionProcessing();
        return;
    }

    HANDLE thread =
        CreateThread(
            nullptr,
            0,
            AnimationThreadProc,
            data,
            0,
            nullptr);

    if (!thread) {
        delete data;

        g_activeAnimationId.store(
            0,
            std::memory_order_release);
        RequestPendingTransitionProcessing();

        return;
    }

    CloseHandle(thread);
}


static void ProcessPendingTransitionOnHookThread() {
    if (g_unloading.load(
            std::memory_order_relaxed) ||
        g_activeAnimationId.load(
            std::memory_order_acquire) != 0) {
        return;
    }

    HWND target = nullptr;
    std::vector<HWND> previousOrder;

    if (!TakePendingTransition(
            &target,
            &previousOrder)) {
        return;
    }

    if (!target ||
        !IsWindow(target)) {
        return;
    }

    // A queued transition is useful only while its target is still the actual
    // foreground window. If another foreground change superseded it, that
    // newer event either replaced the pending transition or started directly.
    if (GetForegroundWindow() != target) {
        return;
    }

    StartSlideBack(
        target,
        previousOrder);
}

static void CancelActiveAnimation() {
    const unsigned long long activeAnimationId =
        g_activeAnimationId.load(
            std::memory_order_acquire);

    if (activeAnimationId != 0) {
        g_cancelledAnimationId.store(
            activeAnimationId,
            std::memory_order_release);
    }
}

static void ClearAltTabSnapshot() {
    std::lock_guard<std::mutex>
        lock(g_altTabMutex);

    g_altTabState = AltTabState::None;
    g_altTabExpectedTarget = nullptr;
    g_altTabForegroundDeadline = 0;
    g_altTabPreviousOrder.clear();
}

enum class AltTabForegroundAction {
    Normal,
    Ignore,
    UseSnapshot,
};

static AltTabForegroundAction ResolveAltTabForeground(
    HWND hWnd,
    bool isTrackable,
    std::vector<HWND>* previousOrder) {

    std::lock_guard<std::mutex>
        lock(g_altTabMutex);

    if (g_altTabState == AltTabState::None) {
        return AltTabForegroundAction::Normal;
    }

    if (g_altTabState == AltTabState::Switching) {
        // Foreground events generated by the selector or its shell surfaces
        // must not overwrite the baseline captured before Alt+Tab began.
        return AltTabForegroundAction::Ignore;
    }

    const bool expired =
        GetTickCount64() >
            g_altTabForegroundDeadline;

    if (expired ||
        (isTrackable && hWnd != g_altTabExpectedTarget)) {
        g_altTabState = AltTabState::None;
        g_altTabExpectedTarget = nullptr;
        g_altTabForegroundDeadline = 0;
        g_altTabPreviousOrder.clear();
        return AltTabForegroundAction::Normal;
    }

    if (!isTrackable ||
        hWnd != g_altTabExpectedTarget) {
        return AltTabForegroundAction::Ignore;
    }

    if (previousOrder) {
        *previousOrder =
            std::move(g_altTabPreviousOrder);
    }

    g_altTabState = AltTabState::None;
    g_altTabExpectedTarget = nullptr;
    g_altTabForegroundDeadline = 0;
    g_altTabPreviousOrder.clear();

    return AltTabForegroundAction::UseSnapshot;
}

static void ProcessForegroundWindow(HWND hWnd) {
    if (!hWnd ||
        IsGhostWindow(hWnd) ||
        g_unloading.load(
            std::memory_order_relaxed)) {
        return;
    }

    const bool isTrackable =
        IsTrackableTopLevelWindow(hWnd);

    std::vector<HWND> altTabPreviousOrder;
    const AltTabForegroundAction altTabAction =
        ResolveAltTabForeground(
            hWnd,
            isTrackable,
            &altTabPreviousOrder);

    if (altTabAction == AltTabForegroundAction::Ignore) {
        return;
    }

    // Capture the new order immediately, but use the stored previous order for
    // the Slide Back relationship.
    const std::vector<HWND> currentOrder =
        CaptureZOrder();

    std::vector<HWND> previousOrder;
    HWND oldForeground = nullptr;

    {
        std::lock_guard<std::mutex>
            lock(g_zOrderMutex);

        previousOrder = g_previousZOrder;
        oldForeground = g_previousForeground;
        g_previousZOrder = currentOrder;
        g_previousForeground = hWnd;
    }

    if (altTabAction == AltTabForegroundAction::UseSnapshot) {
        previousOrder = std::move(altTabPreviousOrder);
    }

    if (hWnd == oldForeground ||
        !isTrackable) {
        return;
    }

    // Never overlap Slide Back animations. A foreground change while one is
    // active cancels its ghosts, retains the newest target, and runs it after
    // cleanup instead of letting old overlays cover the new foreground window.
    const unsigned long long activeAnimationId =
        g_activeAnimationId.load(
            std::memory_order_acquire);

    if (activeAnimationId != 0) {
        QueueLatestPendingTransition(hWnd, previousOrder);
        g_cancelledAnimationId.store(
            activeAnimationId,
            std::memory_order_release);

        if (g_activeAnimationId.load(
                std::memory_order_acquire) != activeAnimationId) {
            RequestPendingTransitionProcessing();
        }

        return;
    }

    ClearPendingTransition();
    StartSlideBack(hWnd, previousOrder);
}

static void CALLBACK TaskSwitchEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hWnd,
    LONG,
    LONG,
    DWORD,
    DWORD) {

    if (g_unloading.load(
            std::memory_order_relaxed)) {
        return;
    }

    if (event == EVENT_SYSTEM_SWITCHSTART) {
        // Do not enumerate here: this is an asynchronous callback and the
        // selector may already have changed the observed desktop. Copy the
        // last known stable application stack instead.
        std::vector<HWND> previousOrder;

        {
            std::lock_guard<std::mutex>
                lock(g_zOrderMutex);
            previousOrder = g_previousZOrder;
        }

        {
            std::lock_guard<std::mutex>
                lock(g_altTabMutex);
            g_altTabPreviousOrder = std::move(previousOrder);
            g_altTabState = AltTabState::Switching;
            g_altTabExpectedTarget = nullptr;
            g_altTabForegroundDeadline = 0;
        }

        ClearPendingTransition();
        CancelActiveAnimation();
        return;
    }

    if (event != EVENT_SYSTEM_SWITCHEND) {
        return;
    }

    bool shouldProcessImmediately = false;

    {
        std::lock_guard<std::mutex>
            lock(g_altTabMutex);

        if (g_altTabState != AltTabState::Switching ||
            !hWnd) {
            g_altTabState = AltTabState::None;
            g_altTabExpectedTarget = nullptr;
            g_altTabForegroundDeadline = 0;
            g_altTabPreviousOrder.clear();
            return;
        }

        g_altTabState = AltTabState::AwaitingSelectedForeground;
        g_altTabExpectedTarget = hWnd;
        g_altTabForegroundDeadline =
            GetTickCount64() + kAltTabForegroundTimeoutMs;
        shouldProcessImmediately =
            GetForegroundWindow() == hWnd;
    }

    // Normally FOREGROUND follows SWITCHEND. This covers the inverse ordering
    // seen on some builds without consuming the snapshot for a different HWND.
    if (shouldProcessImmediately) {
        ProcessForegroundWindow(hWnd);
    }
}

static void CALLBACK ForegroundEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hWnd,
    LONG idObject,
    LONG idChild,
    DWORD,
    DWORD) {

    if (event != EVENT_SYSTEM_FOREGROUND ||
        idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF ||
        !hWnd) {
        return;
    }

    ProcessForegroundWindow(hWnd);
}

static void CALLBACK SystemEventProc(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hWnd,
    LONG idObject,
    LONG idChild,
    DWORD eventThread,
    DWORD eventTime) {

    switch (event) {
    case EVENT_SYSTEM_FOREGROUND:
        ForegroundEventProc(
            hook,
            event,
            hWnd,
            idObject,
            idChild,
            eventThread,
            eventTime);
        break;

    case EVENT_SYSTEM_SWITCHSTART:
    case EVENT_SYSTEM_SWITCHEND:
        TaskSwitchEventProc(
            hook,
            event,
            hWnd,
            idObject,
            idChild,
            eventThread,
            eventTime);
        break;
    }
}

static DWORD WINAPI HookThreadProc(
    LPVOID) {

    g_hookThreadId =
        GetCurrentThreadId();

    if (!RegisterGhostClass()) {
        Wh_Log(
            L"Failed to register ghost window class");
        g_hookThreadId = 0;
        return 0;
    }

    {
        std::lock_guard<std::mutex>
            lock(g_zOrderMutex);

        g_previousZOrder =
            CaptureZOrder();

        g_previousForeground =
            GetForegroundWindow();
    }

    g_systemEventHook =
        SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_SWITCHEND,
            nullptr,
            SystemEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT);

    if (!g_systemEventHook) {
        Wh_Log(
            L"SetWinEventHook for foreground/task switch failed, error=%lu",
            GetLastError());

        g_hookThreadId = 0;
        return 0;
    }

    MSG message{};

    while (GetMessageW(
               &message,
               nullptr,
               0,
               0) > 0) {

        if (message.message ==
            kMsgProcessPendingTransition) {

            ProcessPendingTransitionOnHookThread();
            continue;
        }

        TranslateMessage(
            &message);

        DispatchMessageW(
            &message);
    }

    if (g_systemEventHook) {
        UnhookWinEvent(g_systemEventHook);

        g_systemEventHook =
            nullptr;
    }

    g_hookThreadId = 0;
    return 0;
}

static void LoadSettings() {
    g_enabled.store(
        Wh_GetIntSetting(
            L"enabled") != 0,
        std::memory_order_relaxed);

    g_clearancePx.store(
        ClampInt(
            Wh_GetIntSetting(
                L"clearance_px"),
            0,
            200),
        std::memory_order_relaxed);

    const int strengthPercent =
        ClampInt(
            Wh_GetIntSetting(
                L"strength_percent"),
            10,
            300);

    const int smoothnessPercent =
        ClampInt(
            Wh_GetIntSetting(
                L"smoothness_percent"),
            10,
            300);

    g_strength.store(
        0.12 *
            static_cast<double>(
                strengthPercent) /
            100.0,
        std::memory_order_relaxed);

    g_smoothness.store(
        2.5 *
            static_cast<double>(
                smoothnessPercent) /
            100.0,
        std::memory_order_relaxed);

    const int durationPercent =
        ClampInt(
            Wh_GetIntSetting(
                L"animation_duration_percent"),
            25,
            400);

    g_animationTimeFactor.store(
        static_cast<double>(
            durationPercent) /
        100.0,
        std::memory_order_relaxed);
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_unloading.store(
        false,
        std::memory_order_relaxed);

    ClearPendingTransition();

    BOOL compositionEnabled = FALSE;

    if (FAILED(
            DwmIsCompositionEnabled(
                &compositionEnabled)) ||
        !compositionEnabled) {

        Wh_Log(
            L"DWM composition is required");
        return FALSE;
    }

    g_hookThread =
        CreateThread(
            nullptr,
            0,
            HookThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_hookThread) {
        Wh_Log(
            L"Failed to create hook thread, error=%lu",
            GetLastError());

        return FALSE;
    }

    Wh_Log(
        L"KWin Slide Back v1.1.4 initialized");

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(
        true,
        std::memory_order_relaxed);

    ClearPendingTransition();
    ClearAltTabSnapshot();

    const DWORD threadId =
        g_hookThreadId;

    if (threadId) {
        PostThreadMessageW(
            threadId,
            WM_QUIT,
            0,
            0);
    }

    if (g_hookThread) {
        WaitForSingleObject(
            g_hookThread,
            3000);

        CloseHandle(
            g_hookThread);

        g_hookThread = nullptr;
    }

    // An active detached animation thread will observe g_unloading and destroy
    // its thumbnail overlays before leaving. Give it a short opportunity to do
    // so before the module is unloaded.
    for (int i = 0;
         i < 100 &&
         g_activeAnimationId.load(
             std::memory_order_acquire) != 0;
         ++i) {
        Sleep(10);
    }

    Wh_Log(
        L"KWin Slide Back v1.1.4 stopped");
}
