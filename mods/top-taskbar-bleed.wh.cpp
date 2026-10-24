// ==WindhawkMod==
// @id              top-taskbar-bleed
// @name            Top Taskbar Bleed
// @description     Bleeds the color of a maximized window's top edge into a top-docked taskbar behind the task buttons
// @version         1.0.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @license         GPL-3.0
// @architecture    x86-64
// @compilerOptions -lgdi32 -ldwmapi -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Top Taskbar Bleed

When a maximized window sits under a top-docked taskbar, the taskbar gets a
full-width color panel behind its buttons that matches the window's top edge
(optionally as a gradient, for Mica title bars), so the window's title bar
appears to continue up into the bar.

![Demo](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/top-taskbar-bleed/demo.gif)

**Requires a taskbar docked at the top of the screen** (via the taskbar
position setting on builds that have it, or a mod such as *Taskbar on top for
Windows 11*). On other dock positions this mod stays idle.

How it works:

- A tracker thread watches window events and finds, per monitor, the foremost
  maximized window (the first maximized window in z-order on that monitor -
  not necessarily the foreground window, so a small dialog on top of a
  maximized app doesn't cancel the bleed).
- The window's top edge row is captured from the screen and reduced to its
  dominant color (so a bit of text or an icon can't skew it); that color is
  the panel and decides the taskbar theme. Pixels covered by another window -
  a flyout, a thumbnail, a tooltip, a dialog near the top edge - are left out,
  so floating windows never tint the bleed. Hovering a taskbar thumbnail
  (Aero Peek) bleeds the peeked window if it is maximized on that monitor and
  hides the panel otherwise, since the desktop is what shows through.
- Experimental **Gradient bleed**: instead of one color, the dominant colors
  of four windows at 20/40/60/80% of the width - away from the app icon on the
  left, the caption buttons on the right and a centered title - become a
  horizontal gradient, so a Mica or gradient title bar continues into the bar
  (a flat one stays flat). A point that doesn't fit the rest (a toolbar
  reaching that far, a hovered control) is bridged from its neighbors. Off by
  default: apps that put content along their very top edge can still show it.
- One panel per taskbar is inserted into the taskbar's XAML tree, above the
  taskbar background and below the buttons. The panel mirrors the edge: window
  changes (switching, maximizing, minimizing) are picked up from window events
  and applied on the next composed frame, and the edge is re-sampled in
  between - every **Poll interval** (250 ms by default) while it is static,
  every 25 ms while it is changing - so a title bar that recolors in place
  (focus fade, tab switch, app theme change) is followed frame by frame. With
  Windows animations on, a window being zoomed into place isn't committed
  until its edge has actually landed, so the panel never flashes the background
  behind it; with animations off the first sample is applied as is.

Supports multiple monitors (each taskbar follows its own monitor).

Settings: **Poll interval** (how often a static edge is re-checked; window
changes are event-driven regardless), **Excluded programs** (executables whose
windows should never trigger the bleed) and the experimental **Gradient bleed**
toggle described above.

The panel is a regular XAML `Border` named `TaskbarBleedPanel`, so its shape can
be styled with *Windows 11 Taskbar Styler* like any other taskbar element.
Target `Border#TaskbarBleedPanel` and set, for example, `Margin=4,4,4,0` or
`CornerRadius=8,8,0,0` (the panel's bottom edge meets the window, so the
rounded corners are normally the top ones). Leave `Background` and `Opacity`
alone - the mod drives those (the background is a gradient brush).

## Notes

- The taskbar's XAML tree is reached through `taskbar.dll` and
  `Taskbar.View.dll` symbols (downloaded by Windhawk on first run), not through
  XAML diagnostics, so the mod coexists with Windows 11 Taskbar Styler and other
  XAML-diagnostics consumers. The bleed applies as soon as the mod is enabled.
- Cost: with a maximized window present, each poll is one small screen
  readback (a 1px-high strip) plus a window enumeration; it doesn't register in
  Task Manager at either rate, and the fast rate only runs during a transition.
  With no maximized window the poll timer is off, and window events are only
  acted on when they come from a maximized window or the window being
  followed, so tooltips, thumbnails and overlays cost nothing.
- Windows that exclude themselves from capture (protected media, some
  remote-desktop clients) return no usable pixels; the panel then keeps its
  last color.
- The 1px taskbar border line (`BackgroundStroke`) breaks the illusion; it is
  hidden while a panel is showing and restored when the panel hides.
- Readability: a light title bar bled into a dark-mode taskbar leaves white
  labels on a light panel (and vice versa). While a panel is showing, the mod
  switches that monitor's taskbar to the theme that matches the bled color
  (light panel -> light mode, dark panel -> dark mode) and restores the
  previous theme when the panel hides. This is done per taskbar through XAML's
  `RequestedTheme`, so only the affected monitor changes and the system theme
  itself is untouched.

## Credits

The taskbar-thread helpers (`RunFromWindowThread`, `FindCurrentProcessTaskbarWnd`)
and the `taskbar.dll` XAML-root lookup are ported from
[Taskbar multi-row](https://windhawk.net/mods/taskbar-multirow) by m417z,
licensed under the GNU GPL v3.0. This mod is published under the same license.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PollIntervalMs: 250
  $name: Poll interval (ms)
  $description: How often the window edge is re-checked while nothing is happening. Window changes (switching, maximizing, minimizing) are picked up from events immediately regardless; this only decides how soon a title bar that recolors on its own (a tab switch, an app theme change) is followed. Lower is more responsive, higher costs less.
- ExcludedProcesses: [""]
  $name: Excluded programs
  $description: Executable names (e.g. game.exe) whose windows never trigger the bleed.
- Experimental:
  - Gradient: false
    $name: Gradient bleed
    $description: Follow a title bar's horizontal gradient (Mica) instead of its single dominant color. Samples four points across the window's width; apps that place toolbar icons or other content along their very top edge may show it in the gradient.
  $name: Experimental
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wu = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// ============================================================================
// Settings
// ============================================================================

struct Rgb {
    int r = 0, g = 0, b = 0;
    bool operator==(const Rgb& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const Rgb& o) const { return !(*this == o); }
};

// The panel's horizontal gradient: dominant colors at 20/40/60/80% of the
// window's width (see SampleGradient), continued flat to the edges.
constexpr int kGradientPoints = 4;
using Gradient = std::array<Rgb, kGradientPoints>;

struct Settings {
    int pollIntervalMs = 250;                     // steady-state re-check of a static edge
    std::vector<std::wstring> excludedProcesses;  // lowercase basenames
    bool gradient = false;                        // experimental: four-point gradient instead of one color
};

std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::wstring ToLowerBasename(std::wstring s) {
    size_t pos = s.find_last_of(L"\\/");
    if (pos != std::wstring::npos) s = s.substr(pos + 1);
    for (auto& c : s) c = (wchar_t)std::towlower(c);
    return s;
}

void LoadSettings() {
    Settings s;

    for (int i = 0;; i++) {
        auto p = WindhawkUtils::StringSetting::make(L"ExcludedProcesses[%d]", i);
        if (!*p.get()) break;
        s.excludedProcesses.push_back(ToLowerBasename(p.get()));
    }
    s.pollIntervalMs = std::clamp(Wh_GetIntSetting(L"PollIntervalMs"), 50, 60000);
    s.gradient = Wh_GetIntSetting(L"Experimental.Gradient") != 0;

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(s);
}

// ============================================================================
// Taskbar-thread helpers (from taskbar-multirow)
// ============================================================================

std::atomic<bool> g_unloading{false};

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) && dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) && _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Runs `proc` synchronously on the thread that owns `hWnd`.
bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// ---- taskbar.dll: XAML root of a taskbar window ----------------------------

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

wux::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 &&
            b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown = *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);

    wux::FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {  // null while the host is still being set up (hook path)
        taskbarElementIUnknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                               winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

wux::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;  // taskbar still being constructed (hook path at logon)
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

wux::XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;  // taskbar still being constructed (hook path)
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CSecondaryTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// XAML root of a Shell_TrayWnd / Shell_SecondaryTrayWnd window; nullptr for other windows.
wux::XamlRoot XamlRootOfTaskbarWindow(HWND hWnd) {
    WCHAR cls[32];
    if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) return nullptr;
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) return GetTaskbarXamlRoot(hWnd);
    if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) return GetSecondaryTaskbarXamlRoot(hWnd);
    return nullptr;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar.dll)");
        return false;
    }

    return true;
}

// ============================================================================
// Islands: one per taskbar, owned by the taskbar thread
// ============================================================================

constexpr int ThemeNone = 0, ThemeLight = 1, ThemeDark = 2;

// Mutated only on the taskbar thread. The tracker thread reads `hwnd` under
// g_islandsMutex and sends its updates through RunFromWindowThread.
struct Island {
    HWND hwnd = nullptr;      // Shell_TrayWnd / Shell_SecondaryTrayWnd
    void* frameId = nullptr;  // identity of the Taskbar.TaskbarFrame element
    winrt::weak_ref<wux::FrameworkElement> stroke;
    winrt::weak_ref<wux::FrameworkElement> themeRoot;  // island root content; RequestedTheme is set here
    wuxc::Border panel{nullptr};
    wuxm::LinearGradientBrush brush{nullptr};
    std::vector<wuxm::GradientStop> stops;  // kGradientPoints + 2: edge copies of the outer points
    bool shown = false;
    bool strokeHidden = false;
    int forcedTheme = ThemeNone;
    wux::ElementTheme originalTheme = wux::ElementTheme::Default;  // RequestedTheme before we forced one
    Rgb color{};         // dominant color of the edge (theme decision)
    Gradient gradient{};  // what the panel shows
};

std::mutex g_islandsMutex;
// Holds strong XAML refs. Released with g_islands.reset() on the taskbar
// thread in Wh_ModUninit, never by a destructor at process exit.
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<Island>>> g_islands{std::in_place};

std::vector<HWND> SnapshotIslandWindows() {
    std::vector<HWND> out;
    std::lock_guard lock(g_islandsMutex);
    if (!g_islands) return out;
    for (auto& isl : *g_islands) {
        if (isl->hwnd && IsWindow(isl->hwnd)) out.push_back(isl->hwnd);
    }
    return out;
}

void* IdentityOf(wf::IInspectable const& o) {
    if (!o) return nullptr;
    auto unk = o.try_as<wf::IUnknown>();
    return winrt::get_abi(unk);
}

wu::Color ToColor(Rgb c) {
    return wu::Color{255, (uint8_t)std::clamp(c.r, 0, 255), (uint8_t)std::clamp(c.g, 0, 255),
                     (uint8_t)std::clamp(c.b, 0, 255)};
}

// Tracker -> taskbar thread, synchronously. The tracker is joined before the
// islands are torn down, so no update can arrive after cleanup.
// A hide is sent in two steps: the panel's opacity alone first, then - one
// composed frame later - the stroke and the theme restore. Both of those make
// XAML do more than a render: the stroke's Visibility is a layout change and
// restoring RequestedTheme re-resolves the island's theme resources, and when
// the Opacity change sits in the same pass the panel stays on screen until
// that pass is done.
enum class UpdateStep { Full, PanelOnly, StrokeAndTheme };

struct IslandUpdate {
    HWND hwnd;
    bool active;
    Rgb color;          // dominant edge color: theme decision and log
    Gradient gradient;  // what to paint
    UpdateStep step;
};

void WINAPI ApplyIslandUpdateOnTaskbarThread(void* param);

// Millisecond clock for the tracker's timers and its latency log line.
double NowMs() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)freq.QuadPart;
}

double g_lastEventMs = 0;  // tracker thread: when the most recent window event was delivered (for the log)

void SendIslandUpdate(HWND hwnd, bool active, Rgb color, const Gradient& gradient,
                      UpdateStep step = UpdateStep::Full) {
    if (g_unloading) return;
    IslandUpdate upd{hwnd, active, color, gradient, step};
    RunFromWindowThread(hwnd, ApplyIslandUpdateOnTaskbarThread, &upd);
}

double MsSinceLastEvent() {
    return g_lastEventMs ? NowMs() - g_lastEventMs : 0.0;
}

// ============================================================================
// Tracker thread: window events, target resolution, sampling
// ============================================================================

constexpr UINT WM_TRACKER_TICK = WM_APP + 1;
constexpr UINT WM_TRACKER_SETTINGS = WM_APP + 2;  // re-run the color pipeline on the next tick

// ---- Timing ----------------------------------------------------------------
// Window events (switch, maximize, minimize, move) trigger a tick kDebounceMs
// after the FIRST event of a burst; later events in the burst are folded into
// that tick and never postpone it (a restore fires events for 100-300 ms, and a
// trailing debounce would wait for all of them). kDebounceMs only coalesces;
// the event tick then waits for DWM's next composed frame (DwmFlush) before
// sampling, so the window is on screen when the strip is read, without a fixed
// margin. After an event tick, polling continues for kEventFollowUpMs even
// with no target, so a window whose state hasn't settled is still picked up -
// that also covers events generated while the tracker was busy, which the
// hook delivers only once the loop pumps messages again.
//
// While a maximized window is present the edge is re-sampled: every
// Poll interval (setting) when the strip is static, every kFastPollMs while it is changing
// (a focus fade, a window landing after an animation), so the panel follows a
// transition frame by frame instead of in coarse steps. The slow poll only has
// to catch an edge that recolors on its own (a tab switch, an app theme
// change) - every window change already gets the event tick - so it can be
// leisurely; it is the steady-state cost of the mod, one EnumWindows pass and
// one screen readback per poll. Fast polling is capped at kFastMaxMs per
// transition so content that never settles (video at the top edge) can't keep
// it on; it re-arms once the strip has been still for kFastRearmTicks slow
// polls.
//
// Both schedules run on one high-resolution waitable timer rather than
// SetTimer, whose 15.6 ms granularity would turn these values into 16-47 ms.
constexpr double kDebounceMs = 3;
constexpr ULONGLONG kEventFollowUpMs = 200;
constexpr double kFastPollMs = 25;
constexpr ULONGLONG kFastMaxMs = 1000;
constexpr int kFastRearmTicks = 3;

// ---- New-target hold (window animations on) --------------------------------
// With window animations on, a window that has just become the target - by
// being restored, launched, maximized from a restored state, uncloaked, or
// shown by Peek - is
// being zoomed into place, and until the zoom lands the strip shows what is
// BEHIND it. That background is perfectly stable, so "the color stopped
// changing" would commit the wrong thing. Such a target is therefore held: the
// panel keeps its previous state until
//   1. most of the strip's PIXELS differ from the hold's first sample, i.e. the
//      strip has changed from "behind the window" to "the window" (pixels, not
//      color: a flyout fading over part of the strip, or a photo wallpaper,
//      shifts the dominant color a few levels without the window arriving), and
//   2. the dominant color has read the same on kNewTargetStableSamples
//      consecutive fast polls (color, not pixels: a thumbnail flyout fading out
//      over the strip changes pixels every frame without changing the color).
// A target that was already maximized and visible at the previous tick (a
// z-order switch) is exempt: nothing is in flight, and holding would only delay
// its focus fade. With animations off there is no zoom and no hold at all: the
// first sample is committed and fast polling corrects a late repaint on the
// next poll. kMaxHoldMs caps every hold, so a strip that never settles, or a
// window whose edge happens to match what was behind it, still gets a panel.
constexpr int kNewTargetStableSamples = 2;
constexpr ULONGLONG kMaxHoldMs = 400;

HANDLE g_trackerThread = nullptr;
DWORD g_trackerThreadId = 0;
HANDLE g_trackerReady = nullptr;

// Tracker-thread-only state.
HANDLE g_timer = nullptr;              // one waitable timer for both schedules below
double g_tickDueMs = 0;                // event-triggered tick due time (NowMs), 0 = none
double g_pollDueMs = 0;                // next poll due time (NowMs), 0 = polling off
ULONGLONG g_followUpUntil = 0;         // keep polling until then even with no target
bool g_eventTick = false;              // the tick about to run was triggered by a window event
ULONGLONG g_fastSince = 0;             // nonzero while polling fast
bool g_fastBlockedUntilStill = false;  // fast polling hit its cap; wait for stillness
int g_stillTicks = 0;                  // consecutive ticks with no strip change
bool g_animationsOn = false;           // SPI_GETANIMATION at the last tick
bool g_animationsKnown = false;
std::vector<HWINEVENTHOOK> g_winEventHooks;
// Visible, uncloaked, maximized top-level windows at this tick and the previous
// one; a new target found in the previous set arrived by a z-order switch.
std::unordered_set<HWND> g_maximizedNow, g_maximizedPrev;

struct TrackState {
    HWND target = nullptr;
    bool active = false;        // panel currently showing
    bool notTopLogged = false;
    Rgb committed{};            // dominant color of what the panel shows
    Gradient committedGradient{};
    Gradient lastGradient{};    // of the last sample
    uint64_t lastHash = 0;      // of the last captured strip, to skip recomputing the color
    bool haveLastSample = false;
    Rgb lastSample{};
    bool blockedLogged = false;
    bool changedThisTick = false;  // strip pixels differed from the previous sample
    int stableSamples = 0;         // consecutive samples with the same dominant color
    ULONGLONG holdSince = 0;       // new-target hold in progress (0 = none)
    std::vector<uint32_t> holdStartPx;  // first strip of the hold, i.e. what was behind the window
    bool peekFrozen = false;       // commits frozen while a taskbar flyout is hovered (Aero Peek)
};
std::unordered_map<HWND, TrackState> g_track;  // keyed by taskbar window

bool WindowAnimationsEnabled() {
    ANIMATIONINFO ai{sizeof(ai)};
    return SystemParametersInfoW(SPI_GETANIMATION, sizeof(ai), &ai, 0) && ai.iMinAnimate != 0;
}

void RequestTick() {
    if (g_trackerThreadId) PostThreadMessageW(g_trackerThreadId, WM_TRACKER_TICK, 0, 0);
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) return cloaked != 0;
    return false;
}

bool GetFrameBounds(HWND hwnd, RECT& rc) {
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc)))) return true;
    return GetWindowRect(hwnd, &rc) != FALSE;
}

std::wstring ProcessBasename(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE h = pid ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid) : nullptr;
    if (!h) return L"";
    WCHAR path[MAX_PATH * 2];
    DWORD len = ARRAYSIZE(path);
    std::wstring base;
    if (QueryFullProcessImageNameW(h, 0, path, &len)) base = ToLowerBasename(path);
    CloseHandle(h);
    return base;
}

bool IsProcessExcluded(HWND hwnd, const Settings& s) {
    if (s.excludedProcesses.empty()) return false;
    std::wstring base = ProcessBasename(hwnd);
    return !base.empty() &&
           std::find(s.excludedProcesses.begin(), s.excludedProcesses.end(), base) != s.excludedProcesses.end();
}

std::wstring DescribeWindow(HWND hwnd) {
    WCHAR cls[64] = L"?";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    std::wstring exe = ProcessBasename(hwnd);
    RECT wr{};
    GetFrameBounds(hwnd, wr);
    WCHAR buf[256];
    swprintf_s(buf, L"%p %s [%s] frame=(%d,%d,%d,%d)", hwnd, cls, exe.empty() ? L"?" : exe.c_str(), wr.left, wr.top,
               wr.right, wr.bottom);
    return buf;
}

struct ZWindow {
    HWND hwnd;
    RECT rect;
};

struct EnumCtx {
    const Settings* settings;
    std::unordered_map<HMONITOR, HWND> foremost;  // first maximized window in z-order, per monitor
    std::vector<ZWindow> zorder;                  // every visible top-level window, top first
};

BOOL CALLBACK EnumWindowsForemostProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumCtx*>(lParam);

    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;
    RECT rc;
    if (GetWindowRect(hwnd, &rc) && rc.right > rc.left && rc.bottom > rc.top) ctx->zorder.push_back({hwnd, rc});

    if (!IsZoomed(hwnd)) return TRUE;
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (IsWindowCloaked(hwnd)) return TRUE;

    g_maximizedNow.insert(hwnd);

    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!hmon || ctx->foremost.count(hmon)) return TRUE;
    if (IsProcessExcluded(hwnd, *ctx->settings)) return TRUE;

    ctx->foremost[hmon] = hwnd;
    return TRUE;
}

// ---- Aero Peek ---------------------------------------------------------------
// Hovering a taskbar thumbnail starts Aero Peek: DWM shows the hovered window
// and turns every other window into a glass sheet, while no window's state
// changes. Explorer starts and stops it through dwmapi's DwmpActivateLivePreview
// (export #113, undocumented but stable since Windows 7), passing the peeked
// window; hooking it tells the tracker exactly what is on screen. If the
// peeked window is maximized on a taskbar's monitor, the strip shows its edge
// and sampling simply follows it; otherwise the strip shows the wallpaper and
// that taskbar's panel is hidden until Peek ends. Without the hook (export not
// found) the fallback below freezes the panel while a taskbar flyout is
// hovered.
std::atomic<HWND> g_peekWindow{nullptr};
std::atomic<bool> g_peekActive{false};
bool g_peekHooked = false;

using DwmpActivateLivePreview_t = HRESULT(WINAPI*)(BOOL fActivate, HWND hPeekWnd, HWND hTopmostWnd, UINT uPeekType,
                                                   UINT_PTR param5);
DwmpActivateLivePreview_t DwmpActivateLivePreview_Original;
HRESULT WINAPI DwmpActivateLivePreview_Hook(BOOL fActivate, HWND hPeekWnd, HWND hTopmostWnd, UINT uPeekType,
                                            UINT_PTR param5) {
    HRESULT hr = DwmpActivateLivePreview_Original(fActivate, hPeekWnd, hTopmostWnd, uPeekType, param5);
    g_peekWindow = fActivate ? hPeekWnd : nullptr;
    g_peekActive = fActivate != 0;
    RequestTick();
    return hr;
}

bool HookLivePreview() {
    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    auto target = dwmapi ? (DwmpActivateLivePreview_t)GetProcAddress(dwmapi, MAKEINTRESOURCEA(113)) : nullptr;
    if (!target) {
        Wh_Log(L"dwmapi!DwmpActivateLivePreview (#113) not found, using the flyout-hover fallback");
        return false;
    }
    WindhawkUtils::SetFunctionHook(target, DwmpActivateLivePreview_Hook, &DwmpActivateLivePreview_Original);
    return true;
}

// Whether Peek of `peeked` fills `hmon` with that window's edge. A window that
// was minimized while maximized (Win+D, minimize) is shown by Peek at full
// size, but IsZoomed is false for it and its rect is parked off screen; its
// placement still says WPF_RESTORETOMAXIMIZED and where it lives.
bool PeekShowsMaximizedOn(HWND peeked, HMONITOR hmon) {
    if (!peeked || !IsWindow(peeked) || !IsWindowVisible(peeked)) return false;
    if (!IsIconic(peeked)) return IsZoomed(peeked) && MonitorFromWindow(peeked, MONITOR_DEFAULTTONULL) == hmon;
    WINDOWPLACEMENT wp{sizeof(wp)};
    if (!GetWindowPlacement(peeked, &wp) || !(wp.flags & WPF_RESTORETOMAXIMIZED)) return false;
    return MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTONULL) == hmon;
}

// Fallback fingerprint of Peek: the cursor over a popup owned by the taskbar's
// thread (the thumbnail flyout; the taskbar window itself doesn't count, so
// hovering buttons never freezes anything).
bool CursorOverTaskbarPopup(HWND taskbar) {
    POINT pt;
    HWND under = GetCursorPos(&pt) ? WindowFromPoint(pt) : nullptr;
    if (!under) return false;
    HWND root = GetAncestor(under, GA_ROOT);
    if (!root || root == taskbar) return false;
    if (GetWindowThreadProcessId(root, nullptr) != GetWindowThreadProcessId(taskbar, nullptr)) return false;
    WCHAR cls[32];
    if (!GetClassNameW(root, cls, ARRAYSIZE(cls))) return false;
    return _wcsicmp(cls, L"Shell_TrayWnd") != 0 && _wcsicmp(cls, L"Shell_SecondaryTrayWnd") != 0;
}

bool IsTaskbarTopDocked(HWND taskbar, const RECT& mon) {
    RECT tb;
    if (!taskbar || !GetWindowRect(taskbar, &tb)) return false;
    bool horizontal = (tb.right - tb.left) >= (tb.bottom - tb.top);
    return horizontal && std::abs(tb.top - mon.top) < std::abs(tb.bottom - mon.bottom);
}

// Rows sampled along the window's top edge, starting at the very first visible
// row: a maximized window's 1px DWM border sits in the off-screen frame zone, so
// there is nothing to skip. One row only: it is still thousands of pixels for
// the histogram, and going deeper starts to include toolbar icons and caption
// buttons that some apps place right at the top edge. (An app that paints a
// 1px line of its own along the edge would be sampled as that line; none seen
// so far, and 2 would be the fix.)
constexpr LONG kSampleRows = 1;

// The frame is clipped to the work area first (some hosts report a maximized
// frame that extends under the taskbar by the invisible border width), and then
// kept off the taskbar window itself, so taskbar pixels - including the panel's
// own color - can never feed back into the sampler.
RECT ComputeStrip(const RECT& frame, const RECT& work, HWND taskbar) {
    RECT wr;
    if (!IntersectRect(&wr, &frame, &work)) return RECT{0, 0, 0, 0};
    LONG top = wr.top;
    RECT tb;
    if (taskbar && GetWindowRect(taskbar, &tb) && top < tb.bottom && wr.left < tb.right && wr.right > tb.left) {
        top = tb.bottom;
    }
    RECT r{wr.left, top, wr.right, top + kSampleRows};
    RECT clipped;
    if (!IntersectRect(&clipped, &r, &work)) return RECT{0, 0, 0, 0};
    return clipped;
}

// Capture target, kept between polls (tracker thread only, one per taskbar so
// monitors of different widths don't evict each other): the memory DC and DIB
// are recreated only when the strip size changes. Deliberately no destructor:
// the caches live in a global map, and at process exit (no Wh_ModUninit) its
// destructor would make GDI calls under the loader lock. Release() is called
// explicitly on the tracker thread; a trivially destructible value leaves the
// map's destructor freeing memory only, and the OS reclaims the handles.
struct CaptureCache {
    HDC mem = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    void* bits = nullptr;
    int w = 0, h = 0;

    bool Prepare(HDC screen, int width, int height) {
        if (bmp && w == width && h == height) return true;
        Release();
        mem = CreateCompatibleDC(screen);
        if (!mem) return false;
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;  // top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmp = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp || !bits) {
            Release();
            return false;
        }
        old = SelectObject(mem, bmp);
        w = width;
        h = height;
        return true;
    }

    void Release() {
        if (mem && old) SelectObject(mem, old);
        if (bmp) DeleteObject(bmp);
        if (mem) DeleteDC(mem);
        mem = nullptr;
        bmp = nullptr;
        old = nullptr;
        bits = nullptr;
        w = h = 0;
    }
};
std::unordered_map<HWND, CaptureCache> g_captures;  // keyed by taskbar window

bool CaptureStrip(const RECT& strip, CaptureCache& cache, std::vector<uint32_t>& px) {
    int w = strip.right - strip.left, h = strip.bottom - strip.top;
    if (w <= 0 || h <= 0 || (long long)w * h > 8192LL * 64) return false;

    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    bool ok = cache.Prepare(screen, w, h) && BitBlt(cache.mem, 0, 0, w, h, screen, strip.left, strip.top, SRCCOPY) != FALSE;
    if (ok) {
        GdiFlush();
        auto* bits = static_cast<uint32_t*>(cache.bits);
        px.assign(bits, bits + (size_t)w * h);
    }
    ReleaseDC(nullptr, screen);
    return ok;
}

uint64_t HashPixels(const std::vector<uint32_t>& px) {
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t v : px) {
        h ^= v & 0x00FFFFFFu;
        h *= 1099511628211ULL;
    }
    return h;
}

// Columns of the strip covered by windows above the target in z-order (flyouts,
// thumbnails, tooltips, a dialog near the top edge). Pixels in those columns
// are left out of the dominant color, so a floating window never tints the
// bleed. Click-through and fully transparent overlays don't cover anything,
// and a window covering the whole strip is ignored (an overlay that slipped
// the checks, or something that should have been the target itself).
std::vector<uint8_t> OcclusionMask(const std::vector<ZWindow>& zorder, HWND target, const RECT& strip) {
    int w = strip.right - strip.left;
    std::vector<uint8_t> mask(w, 0);
    int covered = 0;
    for (const ZWindow& zw : zorder) {
        if (zw.hwnd == target) break;
        const RECT& r = zw.rect;
        if (r.bottom <= strip.top || r.top >= strip.bottom || r.right <= strip.left || r.left >= strip.right) continue;
        LONG_PTR ex = GetWindowLongPtrW(zw.hwnd, GWL_EXSTYLE);
        if (ex & WS_EX_TRANSPARENT) continue;
        if (ex & WS_EX_LAYERED) {
            BYTE alpha = 255;
            DWORD flags = 0;
            if (GetLayeredWindowAttributes(zw.hwnd, nullptr, &alpha, &flags) && (flags & LWA_ALPHA) && alpha == 0) continue;
        }
        if (IsWindowCloaked(zw.hwnd)) continue;
        int x0 = (int)(std::max(r.left, strip.left) - strip.left), x1 = (int)(std::min(r.right, strip.right) - strip.left);
        for (int x = x0; x < x1; x++) {
            if (!mask[x]) covered++;
            mask[x] = 1;
        }
    }
    if (covered >= w) std::fill(mask.begin(), mask.end(), 0);
    return mask;
}

// Share of pixels that differ noticeably between two strips of the same size.
// Different sizes (the window's frame changed) count as fully changed.
double FractionChanged(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
    if (a.size() != b.size() || a.empty()) return 1.0;
    size_t changed = 0;
    for (size_t i = 0; i < a.size(); i++) {
        uint32_t x = a[i], y = b[i];
        int d = std::abs((int)(x & 0xFF) - (int)(y & 0xFF)) + std::abs((int)((x >> 8) & 0xFF) - (int)((y >> 8) & 0xFF)) +
                std::abs((int)((x >> 16) & 0xFF) - (int)((y >> 16) & 0xFF));
        if (d > 24) changed++;
    }
    return (double)changed / (double)a.size();
}

// Dominant color: 5-bit-per-channel histogram mode, refined by two mean-shift
// passes so a tight cluster straddling a bin edge (Mica, acrylic, dithered flat
// colors) still yields its true center. Tracker thread only (static buffers).
Rgb DominantColor(const std::vector<uint32_t>& px) {
    struct Bin {
        uint32_t count = 0;
        uint64_t r = 0, g = 0, b = 0;
    };
    static std::vector<Bin> bins(32768);
    static std::vector<uint32_t> touched;
    touched.clear();

    for (uint32_t v : px) {
        uint32_t b = v & 0xFF, g = (v >> 8) & 0xFF, r = (v >> 16) & 0xFF;
        uint32_t key = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        Bin& bin = bins[key];
        if (bin.count == 0) touched.push_back(key);
        bin.count++;
        bin.r += r;
        bin.g += g;
        bin.b += b;
    }
    if (touched.empty()) return Rgb{};

    std::sort(touched.begin(), touched.end(),
              [](uint32_t a, uint32_t b) { return bins[a].count > bins[b].count; });

    Rgb result;
    const Bin& top = bins[touched[0]];
    if ((double)top.count / (double)px.size() >= 0.15 || touched.size() == 1) {
        result = Rgb{(int)(top.r / top.count), (int)(top.g / top.count), (int)(top.b / top.count)};
    } else {
        // No clear winner (gradient, photo): population-weighted mean of the top bins.
        uint64_t cnt = 0, r = 0, g = 0, b = 0;
        size_t k = std::min<size_t>(3, touched.size());
        for (size_t i = 0; i < k; i++) {
            const Bin& bin = bins[touched[i]];
            cnt += bin.count;
            r += bin.r;
            g += bin.g;
            b += bin.b;
        }
        result = Rgb{(int)(r / cnt), (int)(g / cnt), (int)(b / cnt)};
    }
    for (uint32_t key : touched) bins[key] = Bin{};

    for (int pass = 0; pass < 2; pass++) {
        uint64_t cnt = 0, r = 0, g = 0, b = 0;
        for (uint32_t v : px) {
            int pb = v & 0xFF, pg = (v >> 8) & 0xFF, pr = (v >> 16) & 0xFF;
            if (std::abs(pr - result.r) <= 12 && std::abs(pg - result.g) <= 12 && std::abs(pb - result.b) <= 12) {
                cnt++;
                r += pr;
                g += pg;
                b += pb;
            }
        }
        if (cnt == 0) break;
        Rgb refined{(int)std::lround((double)r / cnt), (int)std::lround((double)g / cnt),
                    (int)std::lround((double)b / cnt)};
        if (refined == result) break;
        result = refined;
    }
    return result;
}

int ColorDistance(Rgb a, Rgb b) {
    return std::max({std::abs(a.r - b.r), std::abs(a.g - b.g), std::abs(a.b - b.b)});
}

// Gradient along the edge: the dominant color of a window of kGradientWindow x
// the width, centered at 20/40/60/80% of it. Those positions avoid the app
// icon and menu on the left, the caption buttons on the right, and most of a
// centered title; a Mica gradient is far smoother than four points need. A
// point that is more than kGradientMaxDeviation off the row's overall dominant
// color is content after all (a toolbar reaching that far, a hovered control)
// and is replaced by its neighbors; so is a point under another window. Four
// equal points - a flat title bar - give the flat panel.
constexpr double kGradientWindow = 0.05;
constexpr int kGradientMaxDeviation = 32;

Gradient SampleGradient(const std::vector<uint32_t>& px, int w, int h, const std::vector<uint8_t>& mask, Rgb overall) {
    Gradient g;
    std::array<bool, kGradientPoints> valid{};
    int half = std::max(8, (int)(w * kGradientWindow / 2));
    std::vector<uint32_t> window;
    for (int i = 0; i < kGradientPoints; i++) {
        int center = (int)((double)w * (i + 1) / (kGradientPoints + 1));
        int x0 = std::max(0, center - half), x1 = std::min(w, center + half);
        window.clear();
        for (int y = 0; y < h; y++)
            for (int x = x0; x < x1; x++)
                if (!mask[x]) window.push_back(px[(size_t)y * w + x]);
        if ((int)window.size() < half) continue;  // mostly covered by another window
        Rgb c = DominantColor(window);
        if (ColorDistance(c, overall) > kGradientMaxDeviation) continue;
        g[i] = c;
        valid[i] = true;
    }
    // Fill rejected points from their nearest valid neighbors (linear between
    // two, flat from one), or fall back to the overall color.
    for (int i = 0; i < kGradientPoints; i++) {
        if (valid[i]) continue;
        int l = i - 1, r = i + 1;
        while (l >= 0 && !valid[l]) l--;
        while (r < kGradientPoints && !valid[r]) r++;
        if (l < 0 && r >= kGradientPoints) g[i] = overall;
        else if (l < 0) g[i] = g[r];
        else if (r >= kGradientPoints) g[i] = g[l];
        else {
            int num = i - l, den = r - l;
            g[i] = Rgb{g[l].r + (g[r].r - g[l].r) * num / den, g[l].g + (g[r].g - g[l].g) * num / den,
                       g[l].b + (g[r].b - g[l].b) * num / den};
        }
    }
    return g;
}

// The panel mirrors the edge: no color thresholds or settle times. For the
// current target, any difference between the sampled gradient (four equal
// points when the gradient option is off) and the one showing is applied on
// the spot. The only wait is the new-target hold described above, and it waits
// for the window to land, not for a color.
void Commit(TrackState& st, HWND taskbar, Rgb c, const Gradient& g) {
    bool wasActive = st.active;
    bool colorChanged = !wasActive || c != st.committed;
    st.active = true;
    st.committed = c;
    st.committedGradient = g;
    SendIslandUpdate(taskbar, true, c, g);
    if (colorChanged) {  // gradient-only changes (a Mica fade) would flood the log
        Wh_Log(L"taskbar %p: %s #%02X%02X%02X [#%02X%02X%02X #%02X%02X%02X #%02X%02X%02X #%02X%02X%02X] (target %p), "
               L"%.0f ms after the last window event",
               taskbar, wasActive ? L"recolor" : L"show", c.r, c.g, c.b, g[0].r, g[0].g, g[0].b, g[1].r, g[1].g,
               g[1].b, g[2].r, g[2].g, g[2].b, g[3].r, g[3].g, g[3].b, st.target, MsSinceLastEvent());
    }
}

void Deactivate(TrackState& st, HWND taskbar) {
    if (st.active) {
        SendIslandUpdate(taskbar, false, st.committed, st.committedGradient, UpdateStep::PanelOnly);
        Wh_Log(L"taskbar %p: hide, %.0f ms after the last window event", taskbar, MsSinceLastEvent());
        DwmFlush();  // the frame without the panel is composed before the island relayouts/rethemes
        SendIslandUpdate(taskbar, false, st.committed, st.committedGradient, UpdateStep::StrokeAndTheme);
    }
    bool notTopLogged = st.notTopLogged;
    st = TrackState{};
    st.notTopLogged = notTopLogged;
}

// Capture failed (protected content, capture-excluded window, transient GDI
// failure): log once per target and keep whatever the panel shows.
void HandleBlocked(TrackState& st, HWND taskbar, HWND target) {
    st.changedThisTick = false;
    if (!st.blockedLogged) {
        Wh_Log(L"taskbar %p: capture blocked for target %p, keeping last color", taskbar, target);
        st.blockedLogged = true;
    }
}

void ProcessTaskbar(HWND taskbar, const EnumCtx& ctx, bool& anyTarget) {
    const auto& foremost = ctx.foremost;
    TrackState& st = g_track[taskbar];

    HMONITOR hmon = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!hmon || !GetMonitorInfoW(hmon, &mi)) {
        Deactivate(st, taskbar);
        return;
    }

    if (!IsTaskbarTopDocked(taskbar, mi.rcMonitor)) {
        if (!st.notTopLogged) Wh_Log(L"taskbar %p: not docked at top, idle", taskbar);
        Deactivate(st, taskbar);
        st.notTopLogged = true;
        return;
    }
    st.notTopLogged = false;

    auto it = foremost.find(hmon);
    HWND target = it == foremost.end() ? nullptr : it->second;

    // During Peek the peeked window is what is on screen, whatever the z-order
    // says: it becomes the target if it fills this monitor (even if it is
    // minimized - Peek shows it at full size, framed by the work area), and
    // otherwise the desktop shows through the glass sheets and there is
    // nothing to bleed.
    bool peekedFrame = false;
    if (g_peekHooked && g_peekActive) {
        HWND peeked = g_peekWindow;
        if (PeekShowsMaximizedOn(peeked, hmon)) {
            target = peeked;
            peekedFrame = IsIconic(peeked);
        } else {
            if (st.active) Wh_Log(L"taskbar %p: peeking %p, panel hidden", taskbar, peeked);
            Deactivate(st, taskbar);
            return;
        }
    }

    if (!target) {
        if (st.target) Wh_Log(L"taskbar %p: no maximized window", taskbar);
        Deactivate(st, taskbar);
        return;
    }

    anyTarget = true;
    ULONGLONG now = GetTickCount64();
    bool targetChanged = target != st.target;
    bool zOrderSwitch = false;
    if (targetChanged) {
        st.target = target;
        st.haveLastSample = false;
        st.lastHash = 0;
        st.blockedLogged = false;
        zOrderSwitch = g_maximizedPrev.count(target) != 0;
        st.holdSince = (zOrderSwitch || !g_animationsOn) ? 0 : now;
        st.holdStartPx.clear();
    }

    RECT wr;
    if (peekedFrame) {
        wr = mi.rcWork;  // a minimized window's own rect is parked off screen
    } else if (!GetFrameBounds(target, wr)) {
        HandleBlocked(st, taskbar, target);
        return;
    }
    RECT strip = ComputeStrip(wr, mi.rcWork, taskbar);
    if (targetChanged) {
        Wh_Log(L"taskbar %p: target %s strip=(%d,%d,%d,%d)%s%s", taskbar, DescribeWindow(target).c_str(),
               strip.left, strip.top, strip.right, strip.bottom, zOrderSwitch ? L" (z-order switch)" : L"",
               st.holdSince ? L" (holding for landing)" : L"");
    }

    std::vector<uint32_t> px;
    if (!CaptureStrip(strip, g_captures[taskbar], px) || px.empty()) {
        HandleBlocked(st, taskbar, target);
        return;
    }

    // Pixel changes drive the poll rate; the dominant color drives stability
    // (see the hold notes above for why the two are kept apart). Color and
    // gradient are taken from the pixels not covered by other windows.
    uint64_t hash = HashPixels(px);
    bool pixelsChanged = !(st.haveLastSample && hash == st.lastHash);
    Rgb sampled = st.lastSample;
    if (pixelsChanged) {
        int w = strip.right - strip.left, h = strip.bottom - strip.top;
        auto mask = OcclusionMask(ctx.zorder, target, strip);
        std::vector<uint32_t> visible;
        visible.reserve(px.size());
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                if (!mask[x]) visible.push_back(px[(size_t)y * w + x]);
        sampled = DominantColor(visible.empty() ? px : visible);
        if (ctx.settings->gradient) st.lastGradient = SampleGradient(px, w, h, mask, sampled);
        else st.lastGradient.fill(sampled);  // flat: four equal stops
    }
    bool colorChanged = !st.haveLastSample || sampled != st.lastSample;
    st.changedThisTick = pixelsChanged;
    st.stableSamples = colorChanged ? 1 : st.stableSamples + 1;
    st.lastHash = hash;
    st.lastSample = sampled;
    st.haveLastSample = true;

    if (st.holdSince) {
        ULONGLONG held = now - st.holdSince;
        if (st.holdStartPx.empty()) st.holdStartPx = px;
        bool stable = st.stableSamples >= kNewTargetStableSamples;
        bool changedFromBehind = FractionChanged(px, st.holdStartPx) > 0.5;  // the zoom reached the strip
        bool landed = stable && changedFromBehind;
        if (!landed && held < kMaxHoldMs) return;  // keep whatever the panel shows
        Wh_Log(L"taskbar %p: target %s after %llu ms", taskbar, landed ? L"landed" : L"hold expired", held);
        st.holdSince = 0;
        st.holdStartPx.clear();
        Commit(st, taskbar, sampled, st.lastGradient);
        return;
    }

    // Peek fallback without the hook: keep the panel while a taskbar flyout is
    // hovered, and until the edge has settled again afterwards.
    if (!g_peekHooked && !targetChanged && st.active) {
        if (CursorOverTaskbarPopup(taskbar)) {
            if (!st.peekFrozen) Wh_Log(L"taskbar %p: taskbar flyout hovered, holding the panel", taskbar);
            st.peekFrozen = true;
            return;
        }
        if (st.peekFrozen) {
            if (st.stableSamples < kNewTargetStableSamples) return;
            st.peekFrozen = false;
        }
    }

    if (!st.active || targetChanged || st.lastGradient != st.committedGradient) {
        Commit(st, taskbar, sampled, st.lastGradient);
    }
}

// Arms the waitable timer for whichever of the two due times comes first.
void ArmTimer() {
    double due = g_tickDueMs;
    if (g_pollDueMs && (!due || g_pollDueMs < due)) due = g_pollDueMs;
    if (!due) {
        CancelWaitableTimer(g_timer);
        return;
    }
    double delayMs = std::max(0.0, due - NowMs());
    LARGE_INTEGER rel;
    rel.QuadPart = -(LONGLONG)(delayMs * 10000.0);  // 100 ns units, relative
    SetWaitableTimer(g_timer, &rel, 0, nullptr, nullptr, FALSE);
}

// 0 turns polling off. The next poll is scheduled relative to now, i.e. after
// the tick that called this, so a slow screen readback never piles polls up.
void SetPollTimer(double intervalMs) {
    g_pollDueMs = intervalMs ? NowMs() + intervalMs : 0;
    ArmTimer();
}

// Picks the poll rate for the next tick: fast while something is changing or
// an event just happened, slow once still, off with nothing to follow.
void UpdatePollRate(bool anyTarget, bool anyChanging, ULONGLONG now, double slowPollMs) {
    g_stillTicks = anyChanging ? 0 : g_stillTicks + 1;
    bool followUp = now < g_followUpUntil;
    bool busy = anyChanging || followUp;

    if (g_fastSince) {
        if (now - g_fastSince >= kFastMaxMs) {
            g_fastSince = 0;
            g_fastBlockedUntilStill = true;  // never-settling content; back off until it is still
        } else if (!busy && g_stillTicks >= 2) {
            g_fastSince = 0;  // transition over
        }
    } else if (busy && !(g_fastBlockedUntilStill && g_stillTicks < kFastRearmTicks)) {
        g_fastSince = now;
        g_fastBlockedUntilStill = false;
    }

    bool wantPoll = anyTarget || followUp;
    SetPollTimer(!wantPoll ? 0 : g_fastSince ? kFastPollMs : slowPollMs);
}

void Tick() {
    if (g_unloading) return;
    if (g_eventTick) {
        g_eventTick = false;
        DwmFlush();  // next composed frame: the window the event is about is on screen now
    }
    Settings s = GetSettings();
    auto taskbars = SnapshotIslandWindows();

    if (bool anim = WindowAnimationsEnabled(); anim != g_animationsOn || !g_animationsKnown) {
        Wh_Log(L"window animations %s", anim ? L"on" : L"off");
        g_animationsOn = anim;
        g_animationsKnown = true;
    }

    g_maximizedPrev.swap(g_maximizedNow);
    g_maximizedNow.clear();
    EnumCtx ctx{&s, {}};
    EnumWindows(EnumWindowsForemostProc, reinterpret_cast<LPARAM>(&ctx));

    // Taskbars that are about to hide go first: a hide is a cheap send, while a
    // taskbar with a target costs a screen readback before its turn is over.
    std::stable_partition(taskbars.begin(), taskbars.end(), [&](HWND tb) {
        HMONITOR hmon = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
        return !hmon || !ctx.foremost.count(hmon);
    });

    bool anyTarget = false;
    for (HWND tb : taskbars) ProcessTaskbar(tb, ctx, anyTarget);

    // Drop state for taskbars that disappeared.
    bool anyChanging = false;
    for (auto it = g_track.begin(); it != g_track.end();) {
        bool alive = std::find(taskbars.begin(), taskbars.end(), it->first) != taskbars.end();
        if (alive && (it->second.changedThisTick || it->second.holdSince)) anyChanging = true;
        if (!alive) {
            if (auto c = g_captures.find(it->first); c != g_captures.end()) {
                c->second.Release();
                g_captures.erase(c);
            }
        }
        it = alive ? std::next(it) : g_track.erase(it);
    }

    UpdatePollRate(anyTarget, anyChanging, GetTickCount64(), (double)s.pollIntervalMs);
}

// Window events arrive in bursts (a restore fires many over 100-300 ms). The
// tick is scheduled by the first one and not pushed back by the rest; the
// event follow-up polling catches anything that settles after it.
void ScheduleDebouncedTick() {
    if (g_tickDueMs) return;
    g_tickDueMs = NowMs() + kDebounceMs;
    ArmTimer();
}

bool IsTrackedTarget(HWND hwnd) {
    for (auto& kv : g_track)
        if (kv.second.target == hwnd) return true;
    return false;
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event >= EVENT_OBJECT_CREATE) {
        // Object events: top-level windows only, and only those that can change
        // what the panel shows. Tooltips, thumbnails and overlays show, hide
        // and move constantly; a window that isn't maximized and isn't a
        // current target can't become or stop being the foremost maximized
        // window by doing so. (A target minimizing, restoring down or being
        // destroyed fires these on the target itself.)
        if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
        if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return;
        if (!IsZoomed(hwnd) && !IsTrackedTarget(hwnd)) return;
    }
    g_lastEventMs = NowMs();
    ScheduleDebouncedTick();
}

void AddWinEventHook(DWORD minEvent, DWORD maxEvent) {
    HWINEVENTHOOK h = SetWinEventHook(minEvent, maxEvent, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (h) g_winEventHooks.push_back(h);
    else Wh_Log(L"SetWinEventHook(%04X-%04X) failed", minEvent, maxEvent);
}

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

DWORD WINAPI TrackerThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);  // physical pixels everywhere

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the queue
    g_timer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!g_timer) g_timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);  // pre-1803 fallback
    SetEvent(g_trackerReady);
    if (!g_timer) {
        Wh_Log(L"CreateWaitableTimerEx failed");
        return 0;
    }

    AddWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND);
    AddWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND);
    AddWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND);
    AddWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH);
    AddWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE);  // DESTROY, SHOW, HIDE
    AddWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE);
    AddWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED);

    ScheduleDebouncedTick();

    // Pump messages (the out-of-context WinEvent hooks deliver through the
    // queue) and run ticks when the timer says so.
    for (bool quit = false; !quit;) {
        MsgWaitForMultipleObjectsEx(1, &g_timer, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            if (msg.message == WM_TRACKER_TICK) {
                ScheduleDebouncedTick();
            } else if (msg.message == WM_TRACKER_SETTINGS) {
                // The gradient option changes what a static strip maps to;
                // drop the cached samples so the next tick recomputes them.
                for (auto& kv : g_track) kv.second.haveLastSample = false;
                ScheduleDebouncedTick();
            } else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if (quit) break;

        double now = NowMs();
        bool due = false;
        if (g_tickDueMs && now >= g_tickDueMs) {
            g_tickDueMs = 0;
            g_followUpUntil = GetTickCount64() + kEventFollowUpMs;
            g_eventTick = true;
            due = true;
        }
        if (g_pollDueMs && now >= g_pollDueMs) {
            g_pollDueMs = 0;
            due = true;
        }
        if (due) Tick();  // re-arms the poll through SetPollTimer
        else ArmTimer();  // woke for a message; make sure a pending due time is still armed
    }

    for (HWINEVENTHOOK h : g_winEventHooks) UnhookWinEvent(h);
    g_winEventHooks.clear();
    CancelWaitableTimer(g_timer);
    CloseHandle(g_timer);
    g_timer = nullptr;
    g_tickDueMs = g_pollDueMs = 0;
    g_track.clear();
    g_maximizedNow.clear();
    g_maximizedPrev.clear();
    for (auto& [hwnd, cache] : g_captures) cache.Release();
    g_captures.clear();
    return 0;
}

bool StartTracker() {
    g_trackerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_trackerReady) return false;
    g_trackerThread = CreateThread(nullptr, 0, TrackerThreadProc, nullptr, 0, &g_trackerThreadId);
    if (!g_trackerThread) {
        CloseHandle(g_trackerReady);
        g_trackerReady = nullptr;
        return false;
    }
    WaitForSingleObject(g_trackerReady, INFINITE);
    return true;
}

// Joins the tracker. It only ever blocks on the taskbar thread (SendMessage in
// RunFromWindowThread), never on the engine thread, so INFINITE cannot deadlock.
void StopTracker() {
    if (g_trackerThread) {
        PostThreadMessageW(g_trackerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_trackerThread, INFINITE);
        CloseHandle(g_trackerThread);
        g_trackerThread = nullptr;
        g_trackerThreadId = 0;
    }
    if (g_trackerReady) {
        CloseHandle(g_trackerReady);
        g_trackerReady = nullptr;
    }
}

// ============================================================================
// Painter: XAML panel per taskbar (taskbar thread only)
// ============================================================================

wux::FrameworkElement FindDescendantByName(wux::DependencyObject const& root, std::wstring_view name, int depth) {
    if (!root || depth < 0) return nullptr;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>(); fe && fe.Name() == name) return fe;
        if (auto found = FindDescendantByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

std::shared_ptr<Island> FindIslandByHwnd(HWND hwnd) {
    std::lock_guard lock(g_islandsMutex);
    if (!g_islands) return nullptr;
    for (auto& isl : *g_islands)
        if (isl->hwnd == hwnd) return isl;
    return nullptr;
}

std::shared_ptr<Island> FindIslandByFrameId(void* frameId) {
    std::lock_guard lock(g_islandsMutex);
    if (!g_islands) return nullptr;
    for (auto& isl : *g_islands)
        if (isl->frameId == frameId) return isl;
    return nullptr;
}

// The panel's background is a horizontal LinearGradientBrush: stops at 20/40/
// 60/80% carry the sampled points and the stops at 0% and 100% repeat the outer
// ones, so the gradient continues flat to the edges. Only the stop colors are
// ever touched. No animations: brush and opacity are plain local values. (A
// storyboard's held value would override local sets, so none are ever used.)
void SetPanelGradient(Island& isl, const Gradient& g) {
    if (!isl.brush || isl.stops.size() != kGradientPoints + 2) return;
    isl.stops[0].Color(ToColor(g[0]));
    for (int i = 0; i < kGradientPoints; i++) isl.stops[i + 1].Color(ToColor(g[i]));
    isl.stops[kGradientPoints + 1].Color(ToColor(g[kGradientPoints - 1]));
}

void SetPanelVisible(Island& isl, bool visible) {
    if (isl.panel) isl.panel.Opacity(visible ? 1.0 : 0.0);
}

// ---- Adaptive theme ---------------------------------------------------------
// RequestedTheme on the island's root content re-resolves every theme-resource
// brush under it (labels, monochrome icons, hover highlights, the tray) for
// this island only, i.e. for this monitor's taskbar. The root's previous
// RequestedTheme is saved the first time a theme is forced and restored when
// the panel hides, so a value set by another mod survives.

// Perceived brightness above which a bled color counts as light (black text
// starts out-contrasting white around here), with an 8-level hysteresis band.
// The panel color itself is applied unsmoothed, so this band is the only thing
// keeping the theme from flapping when a color sits right on the threshold.
constexpr int kLightThreshold = 120;
constexpr int kThemeHysteresis = 8;

int PerceivedBrightness(Rgb c) {
    return (int)std::lround(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
}

wux::FrameworkElement ThemeRootOf(Island& isl) {
    if (auto r = isl.themeRoot.get()) return r;
    try {
        if (isl.panel) {
            if (auto xr = isl.panel.XamlRoot()) {
                if (auto root = xr.Content().try_as<wux::FrameworkElement>()) {
                    isl.themeRoot = root;
                    return root;
                }
            }
        }
    } catch (...) {
    }
    return nullptr;
}

void ApplyTheme(Island& isl, bool active, Rgb color) {
    int want = ThemeNone;
    int lum = 0;
    if (active) {
        lum = PerceivedBrightness(color);
        bool light = isl.forcedTheme == ThemeLight  ? lum >= kLightThreshold - kThemeHysteresis
                     : isl.forcedTheme == ThemeDark ? lum > kLightThreshold + kThemeHysteresis
                                                    : lum > kLightThreshold;
        want = light ? ThemeLight : ThemeDark;
    }
    if (want == isl.forcedTheme) return;

    auto root = ThemeRootOf(isl);
    if (!root) return;
    if (isl.forcedTheme == ThemeNone) isl.originalTheme = root.RequestedTheme();
    // Forced unconditionally while the panel shows (identical to the original
    // when it happens to match, and immune to the system theme changing
    // underneath); the original value again when the panel hides.
    root.RequestedTheme(want == ThemeLight  ? wux::ElementTheme::Light
                        : want == ThemeDark ? wux::ElementTheme::Dark
                                            : isl.originalTheme);
    Wh_Log(L"taskbar %p: theme %s (brightness %d)", isl.hwnd,
           want == ThemeLight ? L"light" : want == ThemeDark ? L"dark" : L"restored", lum);
    isl.forcedTheme = want;
}

void SetStrokeHidden(Island& isl, bool hide) {
    if (isl.strokeHidden == hide) return;
    auto stroke = isl.stroke.get();
    if (!stroke) {
        isl.strokeHidden = false;
        return;
    }
    stroke.Visibility(hide ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    isl.strokeHidden = hide;
}

bool IsPanelParented(Island const& isl) {
    try {
        return isl.panel && isl.panel.Parent();
    } catch (...) {
        return false;
    }
}

void WINAPI ApplyIslandUpdateOnTaskbarThread(void* param) try {
    if (g_unloading) return;
    auto& upd = *static_cast<IslandUpdate*>(param);
    auto isl = FindIslandByHwnd(upd.hwnd);
    if (!isl || !IsPanelParented(*isl)) return;

    bool panel = upd.step != UpdateStep::StrokeAndTheme;
    bool rest = upd.step != UpdateStep::PanelOnly;
    if (upd.active) {
        if (panel) {
            if (!isl->shown || isl->gradient != upd.gradient) SetPanelGradient(*isl, upd.gradient);
            if (!isl->shown) SetPanelVisible(*isl, true);
            isl->color = upd.color;
            isl->gradient = upd.gradient;
            isl->shown = true;
        }
        if (rest) {
            SetStrokeHidden(*isl, true);
            ApplyTheme(*isl, true, upd.color);
        }
    } else {
        if (panel && isl->shown) {
            SetPanelVisible(*isl, false);
            isl->shown = false;
        }
        if (rest) {
            SetStrokeHidden(*isl, false);
            ApplyTheme(*isl, false, upd.color);
        }
    }
} catch (winrt::hresult_error const& e) {
    Wh_Log(L"ApplyIslandUpdate failed: %08X", e.code().value);
} catch (...) {
    Wh_Log(L"ApplyIslandUpdate failed");
}

void TeardownIsland(Island& isl) try {
    if (isl.forcedTheme != ThemeNone) {
        if (auto root = ThemeRootOf(isl)) root.RequestedTheme(isl.originalTheme);
        isl.forcedTheme = ThemeNone;
    }
    SetStrokeHidden(isl, false);
    if (isl.panel) {
        if (auto parent = isl.panel.Parent().try_as<wuxc::Panel>()) {
            uint32_t idx = 0;
            if (parent.Children().IndexOf(isl.panel, idx)) parent.Children().RemoveAt(idx);
        }
    }
    isl.panel = nullptr;
    isl.brush = nullptr;
    isl.stops.clear();
    isl.shown = false;
} catch (winrt::hresult_error const& e) {
    Wh_Log(L"teardown failed: %08X", e.code().value);
} catch (...) {
    Wh_Log(L"teardown failed");
}

// Drops islands whose taskbar window is gone (monitor unplugged) or whose
// panel lost its parent (frame re-templated), so a rebuilt frame on the same
// window can be attached again and the tracker stops serving dead windows.
void PruneDeadIslands() {
    std::vector<std::shared_ptr<Island>> dead;
    {
        std::lock_guard lock(g_islandsMutex);
        if (!g_islands) return;
        for (auto it = g_islands->begin(); it != g_islands->end();) {
            auto& isl = *it;
            if (!IsWindow(isl->hwnd) || !IsPanelParented(*isl)) {
                dead.push_back(isl);
                it = g_islands->erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto& isl : dead) {
        Wh_Log(L"taskbar %p: island dropped (window gone or panel detached)", isl->hwnd);
        TeardownIsland(*isl);
    }
}

// ----------------------------------------------------------------------------
// Island creation (taskbar thread)
// ----------------------------------------------------------------------------

// Frames whose attachment failed (taskbar window not ready yet at logon, or an
// unexpected tree), with the tick of the last attempt, so hover/state events
// don't re-walk the tree continuously. Taskbar thread only.
std::unordered_map<void*, ULONGLONG> g_failedFrames;
constexpr ULONGLONG kFailedRetryMs = 3000;

bool AttachIsland(wux::FrameworkElement const& rep, HWND taskbarWnd) {
    // Nearest Grid ancestor hosts the panel; `anchor` is that grid's direct child on the path.
    wuxc::Grid grid{nullptr};
    wux::UIElement anchor{nullptr};
    wux::DependencyObject frame{nullptr};
    wux::DependencyObject prev = rep;
    wux::DependencyObject cur = wuxm::VisualTreeHelper::GetParent(rep);
    for (int hop = 0; cur && hop < 8; hop++) {
        if (!grid) {
            if (auto g = cur.try_as<wuxc::Grid>()) {
                grid = g;
                anchor = prev.try_as<wux::UIElement>();
            }
        }
        if (winrt::get_class_name(cur) == L"Taskbar.TaskbarFrame") {
            frame = cur;
            break;
        }
        prev = cur;
        cur = wuxm::VisualTreeHelper::GetParent(cur);
    }
    if (!frame || !grid || !anchor) {
        Wh_Log(L"taskbar %p: unexpected tree around TaskbarFrameRepeater (frame=%d grid=%d)", taskbarWnd,
               frame ? 1 : 0, grid ? 1 : 0);
        return false;
    }

    void* frameId = IdentityOf(frame);

    if (auto existing = FindIslandByFrameId(frameId); existing && IsPanelParented(*existing)) {
        return true;  // already attached
    }

    auto isl = std::make_shared<Island>();
    isl->hwnd = taskbarWnd;
    isl->frameId = frameId;
    isl->brush = wuxm::LinearGradientBrush();
    isl->brush.StartPoint(wf::Point{0, 0});
    isl->brush.EndPoint(wf::Point{1, 0});
    for (int i = 0; i < kGradientPoints + 2; i++) {
        wuxm::GradientStop stop;
        stop.Offset(i == 0 ? 0.0 : i == kGradientPoints + 1 ? 1.0 : (double)i / (kGradientPoints + 1));
        stop.Color(wu::Color{255, 0, 0, 0});
        isl->brush.GradientStops().Append(stop);
        isl->stops.push_back(stop);
    }

    // A Border stretches to fill its grid cell by default, so the panel spans
    // the whole taskbar. Its name lets Taskbar Styler target it
    // (Border#TaskbarBleedPanel) for Margin / CornerRadius.
    wuxc::Border panel;
    panel.Name(L"TaskbarBleedPanel");
    panel.IsHitTestVisible(false);
    panel.Background(isl->brush);
    panel.Opacity(0.0);
    isl->panel = panel;

    auto children = grid.Children();
    uint32_t idx = 0;
    if (children.IndexOf(anchor, idx)) {
        children.InsertAt(idx, panel);
    } else {
        bool inserted = false;
        for (uint32_t i = 0; i < children.Size(); i++) {
            if (winrt::get_class_name(children.GetAt(i)) == L"Taskbar.TaskbarBackground") {
                children.InsertAt(i + 1, panel);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            Wh_Log(L"taskbar %p: no insertion point in the taskbar grid", taskbarWnd);
            return false;
        }
    }

    isl->stroke = FindDescendantByName(grid, L"BackgroundStroke", 6);

    {
        std::lock_guard lock(g_islandsMutex);
        if (!g_islands) {
            TeardownIsland(*isl);
            return false;
        }
        g_islands->push_back(isl);
    }

    Wh_Log(L"taskbar %p: island attached (grid=%s stroke=%d)", taskbarWnd, grid.Name().c_str(),
           isl->stroke.get() ? 1 : 0);
    RequestTick();
    return true;
}

// Attach to every taskbar owned by this (the taskbar) thread, straight from the
// taskbar windows. Used at enable time and after settings changes.
void WINAPI AttachAllTaskbars(void*) try {
    if (g_unloading) return;
    PruneDeadIslands();
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            try {
                auto xamlRoot = XamlRootOfTaskbarWindow(hWnd);
                if (!xamlRoot) return TRUE;
                auto rep = FindDescendantByName(xamlRoot.Content(), L"TaskbarFrameRepeater", 12);
                if (!rep) {
                    Wh_Log(L"taskbar %p: TaskbarFrameRepeater not found", hWnd);
                    return TRUE;
                }
                AttachIsland(rep, hWnd);
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"taskbar %p: attach failed: %08X", hWnd, e.code().value);
            } catch (...) {
                Wh_Log(L"taskbar %p: attach failed", hWnd);
            }
            return TRUE;
        },
        0);
} catch (...) {
    Wh_Log(L"AttachAllTaskbars failed");
}

// Hook path: a TaskbarFrame announced a layout change. Covers taskbars created
// later (monitor plugged in) and the logon case, where the frame exists before
// Wh_ModAfterInit. The frame's window is found by matching its XamlRoot against
// each taskbar window's root. Runs directly in the hook, on the taskbar thread.
void DiscoverFromFrame(wux::DependencyObject const& frame) {
    if (g_unloading || !frame) return;
    void* frameId = IdentityOf(frame);
    if (auto isl = FindIslandByFrameId(frameId); isl && IsPanelParented(*isl)) return;

    ULONGLONG now = GetTickCount64();
    if (auto it = g_failedFrames.find(frameId); it != g_failedFrames.end() && now - it->second < kFailedRetryMs) {
        return;
    }

    try {
        PruneDeadIslands();

        auto fe = frame.try_as<wux::FrameworkElement>();
        auto xr = fe ? fe.XamlRoot() : nullptr;
        void* rootId = xr ? IdentityOf(xr.Content()) : nullptr;
        HWND taskbarWnd = nullptr;
        if (rootId) {
            struct Ctx {
                void* rootId;
                HWND found;
            } ctx{rootId, nullptr};
            EnumThreadWindows(
                GetCurrentThreadId(),
                [](HWND hWnd, LPARAM lp) -> BOOL {
                    auto* c = reinterpret_cast<Ctx*>(lp);
                    try {
                        auto root = XamlRootOfTaskbarWindow(hWnd);
                        if (root && IdentityOf(root.Content()) == c->rootId) {
                            c->found = hWnd;
                            return FALSE;
                        }
                    } catch (...) {
                    }
                    return TRUE;
                },
                reinterpret_cast<LPARAM>(&ctx));
            taskbarWnd = ctx.found;
        }

        auto rep = fe ? FindDescendantByName(fe, L"TaskbarFrameRepeater", 10) : nullptr;
        if (!taskbarWnd || !rep) {
            g_failedFrames[frameId] = now;
            Wh_Log(L"frame %p: attach deferred (window %s, repeater %s)", frameId,
                   taskbarWnd ? L"found" : L"not found", rep ? L"found" : L"not found");
            return;
        }
        if (AttachIsland(rep, taskbarWnd)) g_failedFrames.erase(frameId);
        else g_failedFrames[frameId] = now;
    } catch (winrt::hresult_error const& e) {
        g_failedFrames[frameId] = now;
        Wh_Log(L"frame %p: discovery failed: %08X", frameId, e.code().value);
    } catch (...) {
        g_failedFrames[frameId] = now;
        Wh_Log(L"frame %p: discovery failed", frameId);
    }
}

// Final teardown on the taskbar thread: remove every panel, restore themes and
// strokes, and release the island container itself.
void WINAPI CleanupIslandsOnTaskbarThread(void*) {
    std::vector<std::shared_ptr<Island>> all;
    {
        std::lock_guard lock(g_islandsMutex);
        if (g_islands) {
            all.swap(*g_islands);
            g_islands.reset();
        }
    }
    for (auto& isl : all) TeardownIsland(*isl);
    all.clear();  // XAML objects released here, on their thread
    g_failedFrames.clear();
}

// ============================================================================
// Taskbar.View.dll hooks (layout-driven discovery for taskbars created later)
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded{false};

// winrt::implements objects used by the taskbar: the IUnknown of the projected
// element sits three pointers past the implementation `this` (same resolver as
// the catalog taskbar mods).
wux::FrameworkElement ElementFromImplThis(void* pThis) {
    try {
        void* unkPtr = (void**)pThis + 3;
        wf::IUnknown unk;
        winrt::copy_from_abi(unk, unkPtr);
        return unk.try_as<wux::FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

wux::DependencyObject FindTaskbarFrameAncestor(wux::DependencyObject const& start) {
    wux::DependencyObject cur = start;
    for (int hop = 0; cur && hop < 16; hop++) {
        if (winrt::get_class_name(cur) == L"Taskbar.TaskbarFrame") return cur;
        cur = wuxm::VisualTreeHelper::GetParent(cur);
    }
    return nullptr;
}

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t = void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);
    if (g_unloading) return;
    try {  // never let a winrt exception unwind into Taskbar.View.dll frames
        if (auto frame = ElementFromImplThis(pThis)) DiscoverFromFrame(frame);
    } catch (...) {
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;
    try {  // never let a winrt exception unwind into Taskbar.View.dll frames
        if (auto btn = ElementFromImplThis(pThis)) {
            if (auto frame = FindTaskbarFrameAncestor(btn)) DiscoverFromFrame(frame);
        }
    } catch (...) {
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
            &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
            TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed (Taskbar.View.dll)");
        return false;
    }
    if (!TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original && !TaskListButton_UpdateVisualStates_Original) {
        Wh_Log(L"no discovery symbol resolved");
        return false;
    }
    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandleW(L"ExplorerExtensions.dll");
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR name) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"loaded %s", name ? name : L"?");
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    return module;
}

// ============================================================================
// Mod lifecycle
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    if (!HookTaskbarDllSymbols()) return FALSE;

    if (HMODULE taskbarView = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarView)) return FALSE;
    } else {
        Wh_Log(L"Taskbar.View.dll not loaded yet, hooking LoadLibraryExW");
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW =
            kernelBase ? (decltype(&LoadLibraryExW))GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
        if (!pLoadLibraryExW) {
            Wh_Log(L"kernelbase!LoadLibraryExW not found");
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }

    g_peekHooked = HookLivePreview();

    if (!StartTracker()) {
        Wh_Log(L"failed to start tracker thread");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Taskbar.View.dll may have loaded between Wh_ModInit and the hook taking effect.
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarView = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"got Taskbar.View.dll after init");
                if (HookTaskbarViewDllSymbols(taskbarView)) Wh_ApplyHookOperations();
            }
        }
    }

    // Attach to the taskbars that already exist; later ones arrive via the hooks.
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, AttachAllTaskbars, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;  // hooks are still installed; they bail on this
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Hooks are gone, so nothing new can arrive. The tracker is joined first
    // (its updates are synchronous SendMessages, so joining it covers them),
    // then the panels are removed synchronously on the taskbar thread.
    StopTracker();
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, CleanupIslandsOnTaskbarThread, nullptr);
    } else {
        // No taskbar window (Explorer is going away): there is no tree to clean
        // and no taskbar thread to release XAML objects on. g_islands is
        // no_destroy, so leaving it alone leaks the few refs instead of
        // releasing them from this thread.
        Wh_Log(L"no taskbar window, skipping UI teardown");
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();

    if (g_trackerThreadId) PostThreadMessageW(g_trackerThreadId, WM_TRACKER_SETTINGS, 0, 0);

    // Nothing in the settings touches the panels themselves; re-running the
    // attach pass just picks up taskbars added since and prunes dead ones.
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, AttachAllTaskbars, nullptr);
    }
}
