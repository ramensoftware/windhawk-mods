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
full-width flat color panel behind its buttons that matches the window's top
edge, so the window's title bar appears to continue up into the bar.

![Demo](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/top-taskbar-bleed/demo.gif)

**Requires a taskbar docked at the top of the screen** (via the taskbar
position setting on builds that have it, or a mod such as *Taskbar on top for
Windows 11*). On other dock positions this mod stays idle.

How it works:

- A tracker thread watches window events and finds, per monitor, the foremost
  maximized window (the first maximized window in z-order on that monitor -
  not necessarily the foreground window, so a small dialog on top of a
  maximized app doesn't cancel the bleed).
- The strip of pixels along the window's top edge is captured from the screen
  and reduced to its dominant color, then smoothed so scrolling and video don't
  make the bar flicker.
- One panel per taskbar is inserted into the taskbar's XAML tree, above the
  taskbar background and below the buttons; color changes apply instantly.

Supports multiple monitors (each taskbar follows its own monitor).

The panel's shape is adjustable: **Margins** takes "left,top,right,bottom"
(positive insets the panel from that taskbar edge, negative extends it) and
**Corner radius** takes "top-left,top-right,bottom-right,bottom-left", both in
screen orientation (the bottom edge of the panel meets the window, so the
rounded corners are normally the top ones).

## Notes

- The taskbar's XAML tree is reached through `taskbar.dll` and
  `Taskbar.View.dll` symbols (downloaded by Windhawk on first run), not through
  XAML diagnostics, so the mod coexists with Windows 11 Taskbar Styler and other
  XAML-diagnostics consumers. The bleed applies as soon as the mod is enabled.
- With a maximized window present, the mod samples a few thousand pixels every
  few hundred milliseconds. This is cheap, but set **Poll interval** higher if
  you want it even lighter.
- Windows that exclude themselves from capture (protected media, some
  remote-desktop clients) return no usable pixels; the panel then keeps its
  last color. Flyouts that open right under the taskbar (Start, tray flyouts,
  thumbnails) are sampled like anything else on screen.
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
- Panel:
  - Margins: "0,0,0,0"
    $name: Margins
    $description: Four values in pixels as left,top,right,bottom (a single value applies to all four), measured from the taskbar edges. Positive insets that edge of the panel, negative extends it.
  - CornerRadius: "0,0,0,0"
    $name: Corner radius
    $description: Four values in pixels as top-left,top-right,bottom-right,bottom-left (a single value applies to all four), screen orientation. Keep the bottom corners (the ones touching the window) at 0 for a seamless bleed.
  $name: Panel
- Sampling:
  - SampleDepth: 4
    $name: Sample depth (px)
    $description: How many rows of pixels are sampled along the window's top edge.
  - SampleInset: 1
    $name: Sample inset (px)
    $description: Skip this many pixels from the window edge before sampling. 1 skips the 1px window border Windows draws on the edge.
  - PollIntervalMs: 300
    $name: Poll interval (ms)
    $description: How often the edge is re-sampled while a maximized window is present.
  $name: Sampling
- Smoothing:
  - ChangeThreshold: 10
    $name: Change threshold
    $description: Color changes smaller than this (0-255 per channel) are treated as jitter and ignored while they fluctuate; a small change that stays put is still applied after the settle time below, so the panel always converges on the exact edge color.
  - SettleMs: 1000
    $name: Settle time (ms)
    $description: How long a small color change must persist before it is applied.
  - ConfirmSamples: 2
    $name: Confirm samples
    $description: A new color must be seen this many consecutive times before the panel changes.
  - MinCommitIntervalMs: 400
    $name: Minimum time between changes (ms)
  $name: Smoothing
- Behavior:
  - ExcludedProcesses: [""]
    $name: Excluded programs
    $description: Executable names (e.g. game.exe) whose windows never trigger the bleed.
  $name: Behavior
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

struct Settings {
    int marginLeft = 0, marginTop = 0, marginRight = 0, marginBottom = 0;
    int radiusTopLeft = 0, radiusTopRight = 0, radiusBottomRight = 0, radiusBottomLeft = 0;

    int sampleDepth = 4;
    int sampleInset = 1;
    int pollIntervalMs = 300;

    int changeThreshold = 10;
    int settleMs = 1000;
    int confirmSamples = 2;
    int minCommitIntervalMs = 400;

    std::vector<std::wstring> excludedProcesses;  // lowercase basenames
};

std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

// Parse "a,b,c,d" (or a single "a" applied to all four). Returns false and
// leaves `out` untouched on any other shape.
bool ParseFour(std::wstring_view text, int (&out)[4]) {
    int vals[4];
    int n = 0;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t comma = text.find(L',', pos);
        std::wstring_view tok =
            text.substr(pos, comma == std::wstring_view::npos ? std::wstring_view::npos : comma - pos);
        while (!tok.empty() && iswspace(tok.front())) tok.remove_prefix(1);
        while (!tok.empty() && iswspace(tok.back())) tok.remove_suffix(1);
        if (tok.empty() || n == 4) return false;
        std::wstring tmp(tok);
        wchar_t* end = nullptr;
        long v = wcstol(tmp.c_str(), &end, 10);
        if (!end || *end != L'\0') return false;
        vals[n++] = (int)v;
        if (comma == std::wstring_view::npos) break;
        pos = comma + 1;
    }
    if (n == 1) {
        for (int i = 0; i < 4; i++) out[i] = vals[0];
        return true;
    }
    if (n == 4) {
        for (int i = 0; i < 4; i++) out[i] = vals[i];
        return true;
    }
    return false;
}

std::wstring ToLowerBasename(std::wstring s) {
    size_t pos = s.find_last_of(L"\\/");
    if (pos != std::wstring::npos) s = s.substr(pos + 1);
    for (auto& c : s) c = (wchar_t)std::towlower(c);
    return s;
}

void LoadSettings() {
    Settings s;

    {
        int m[4] = {0, 0, 0, 0};
        auto text = WindhawkUtils::StringSetting::make(L"Panel.Margins");
        if (!ParseFour(text.get(), m)) Wh_Log(L"Margins '%s' not understood, using defaults", text.get());
        s.marginLeft = m[0];
        s.marginTop = m[1];
        s.marginRight = m[2];
        s.marginBottom = m[3];
    }
    {
        int r[4] = {0, 0, 0, 0};
        auto text = WindhawkUtils::StringSetting::make(L"Panel.CornerRadius");
        if (!ParseFour(text.get(), r)) Wh_Log(L"CornerRadius '%s' not understood, using defaults", text.get());
        s.radiusTopLeft = std::max(0, r[0]);
        s.radiusTopRight = std::max(0, r[1]);
        s.radiusBottomRight = std::max(0, r[2]);
        s.radiusBottomLeft = std::max(0, r[3]);
    }

    s.sampleDepth = std::clamp(Wh_GetIntSetting(L"Sampling.SampleDepth"), 1, 64);
    s.sampleInset = std::clamp(Wh_GetIntSetting(L"Sampling.SampleInset"), 0, 256);
    s.pollIntervalMs = std::clamp(Wh_GetIntSetting(L"Sampling.PollIntervalMs"), 50, 60000);

    s.changeThreshold = std::clamp(Wh_GetIntSetting(L"Smoothing.ChangeThreshold"), 0, 255);
    s.settleMs = std::clamp(Wh_GetIntSetting(L"Smoothing.SettleMs"), 0, 60000);
    s.confirmSamples = std::clamp(Wh_GetIntSetting(L"Smoothing.ConfirmSamples"), 1, 100);
    s.minCommitIntervalMs = std::max(0, Wh_GetIntSetting(L"Smoothing.MinCommitIntervalMs"));

    for (int i = 0;; i++) {
        auto p = WindhawkUtils::StringSetting::make(L"Behavior.ExcludedProcesses[%d]", i);
        if (!*p.get()) break;
        s.excludedProcesses.push_back(ToLowerBasename(p.get()));
    }

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
    wuxm::SolidColorBrush brush{nullptr};
    bool shown = false;
    bool strokeHidden = false;
    int forcedTheme = ThemeNone;
    wux::ElementTheme originalTheme = wux::ElementTheme::Default;  // RequestedTheme before we forced one
    Rgb color{};
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
struct IslandUpdate {
    HWND hwnd;
    bool active;
    Rgb color;
};

void WINAPI ApplyIslandUpdateOnTaskbarThread(void* param);

void SendIslandUpdate(HWND hwnd, bool active, Rgb color) {
    if (g_unloading) return;
    IslandUpdate upd{hwnd, active, color};
    RunFromWindowThread(hwnd, ApplyIslandUpdateOnTaskbarThread, &upd);
}

// ============================================================================
// Tracker thread: window events, target resolution, sampling, smoothing
// ============================================================================

constexpr UINT WM_TRACKER_TICK = WM_APP + 1;
constexpr UINT WM_TRACKER_SETTINGS = WM_APP + 2;

HANDLE g_trackerThread = nullptr;
DWORD g_trackerThreadId = 0;
HANDLE g_trackerReady = nullptr;

// Tracker-thread-only state.
UINT_PTR g_debounceTimer = 0;
UINT_PTR g_pollTimer = 0;
int g_pollTimerInterval = 0;
std::vector<HWINEVENTHOOK> g_winEventHooks;

struct TrackState {
    HWND target = nullptr;
    bool active = false;
    bool notTopLogged = false;
    Rgb committed{};
    Rgb candidate{};
    int candidateCount = 0;
    ULONGLONG lastCommitTick = 0;
    // Drift correction: a sub-threshold difference from the committed color,
    // and since when it has been seen continuously (0 = none).
    Rgb drift{};
    ULONGLONG driftSinceTick = 0;
    uint64_t lastHash = 0;
    bool haveLastSample = false;
    Rgb lastSample{};
    bool blockedLogged = false;
};
std::unordered_map<HWND, TrackState> g_track;  // keyed by taskbar window

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

struct EnumCtx {
    const Settings* settings;
    std::unordered_map<HMONITOR, HWND> foremost;  // first maximized window in z-order, per monitor
};

BOOL CALLBACK EnumWindowsForemostProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumCtx*>(lParam);

    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || !IsZoomed(hwnd)) return TRUE;
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (IsWindowCloaked(hwnd)) return TRUE;

    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!hmon || ctx->foremost.count(hmon)) return TRUE;
    if (IsProcessExcluded(hwnd, *ctx->settings)) return TRUE;

    ctx->foremost[hmon] = hwnd;
    return TRUE;
}

bool IsTaskbarTopDocked(HWND taskbar, const RECT& mon) {
    RECT tb;
    if (!taskbar || !GetWindowRect(taskbar, &tb)) return false;
    bool horizontal = (tb.right - tb.left) >= (tb.bottom - tb.top);
    return horizontal && std::abs(tb.top - mon.top) < std::abs(tb.bottom - mon.bottom);
}

// Rows to sample along the window's top edge. The frame is clipped to the work
// area first (some hosts report a maximized frame that extends under the
// taskbar by the invisible border width), and then kept off the taskbar
// window itself, so taskbar pixels - including the panel's own color - can
// never feed back into the sampler.
RECT ComputeStrip(const RECT& frame, const RECT& work, HWND taskbar, const Settings& s) {
    RECT wr;
    if (!IntersectRect(&wr, &frame, &work)) return RECT{0, 0, 0, 0};
    LONG top = wr.top + s.sampleInset;
    RECT tb;
    if (taskbar && GetWindowRect(taskbar, &tb) && top < tb.bottom && wr.left < tb.right && wr.right > tb.left) {
        top = std::max(top, tb.bottom + s.sampleInset);
    }
    RECT r{wr.left, top, wr.right, top + s.sampleDepth};
    RECT clipped;
    if (!IntersectRect(&clipped, &r, &work)) return RECT{0, 0, 0, 0};
    return clipped;
}

bool CaptureStrip(const RECT& strip, std::vector<uint32_t>& px) {
    int w = strip.right - strip.left, h = strip.bottom - strip.top;
    if (w <= 0 || h <= 0 || (long long)w * h > 8192LL * 64) return false;

    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    HDC mem = CreateCompatibleDC(screen);
    if (!mem) {
        ReleaseDC(nullptr, screen);
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    bool ok = false;
    if (bmp && bits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        ok = BitBlt(mem, 0, 0, w, h, screen, strip.left, strip.top, SRCCOPY) != FALSE;
        GdiFlush();
        if (ok) px.assign(static_cast<uint32_t*>(bits), static_cast<uint32_t*>(bits) + (size_t)w * h);
        SelectObject(mem, old);
    }
    if (bmp) DeleteObject(bmp);
    DeleteDC(mem);
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

void Commit(TrackState& st, HWND taskbar, Rgb c, ULONGLONG now) {
    Wh_Log(L"taskbar %p: commit #%02X%02X%02X (target %p)", taskbar, c.r, c.g, c.b, st.target);
    st.active = true;
    st.committed = c;
    st.candidateCount = 0;
    st.driftSinceTick = 0;
    st.lastCommitTick = now;
    SendIslandUpdate(taskbar, true, c);
}

void Deactivate(TrackState& st, HWND taskbar) {
    if (st.active) SendIslandUpdate(taskbar, false, st.committed);
    bool notTopLogged = st.notTopLogged;
    st = TrackState{};
    st.notTopLogged = notTopLogged;
}

void Smooth(TrackState& st, HWND taskbar, Rgb c, bool targetChanged, const Settings& s) {
    ULONGLONG now = GetTickCount64();
    if (!st.active || targetChanged) {
        Commit(st, taskbar, c, now);
        return;
    }
    if (ColorDistance(c, st.committed) <= s.changeThreshold) {
        st.candidateCount = 0;
        // Sub-threshold difference: not a change worth applying on its own,
        // but if it persists it is the real edge color (the first commit may
        // have landed mid-transition), so converge on it after the settle time.
        if (c == st.committed) {
            st.driftSinceTick = 0;
        } else if (st.driftSinceTick == 0 || ColorDistance(c, st.drift) > 2) {
            st.drift = c;
            st.driftSinceTick = now;
        } else if (now - st.driftSinceTick >= (ULONGLONG)s.settleMs) {
            Commit(st, taskbar, c, now);
        }
        return;
    }
    st.driftSinceTick = 0;
    if (st.candidateCount > 0 && ColorDistance(c, st.candidate) <= s.changeThreshold) {
        st.candidateCount++;
    } else {
        st.candidate = c;
        st.candidateCount = 1;
    }
    if (st.candidateCount >= s.confirmSamples && now - st.lastCommitTick >= (ULONGLONG)s.minCommitIntervalMs) {
        Commit(st, taskbar, st.candidate, now);
    }
}

// Capture failed (protected content, capture-excluded window, transient GDI
// failure): log once per target and keep whatever the panel shows.
void HandleBlocked(TrackState& st, HWND taskbar, HWND target) {
    if (!st.blockedLogged) {
        Wh_Log(L"taskbar %p: capture blocked for target %p, keeping last color", taskbar, target);
        st.blockedLogged = true;
    }
}

void ProcessTaskbar(HWND taskbar, const Settings& s, const std::unordered_map<HMONITOR, HWND>& foremost,
                    bool& anyTarget) {
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
    if (!target) {
        if (st.target) Wh_Log(L"taskbar %p: no maximized window", taskbar);
        Deactivate(st, taskbar);
        return;
    }

    anyTarget = true;
    bool targetChanged = target != st.target;
    if (targetChanged) {
        st.target = target;
        st.haveLastSample = false;
        st.lastHash = 0;
        st.blockedLogged = false;
    }

    RECT wr;
    if (!GetFrameBounds(target, wr)) {
        HandleBlocked(st, taskbar, target);
        return;
    }
    RECT strip = ComputeStrip(wr, mi.rcWork, taskbar, s);
    if (targetChanged) {
        Wh_Log(L"taskbar %p: target %s strip=(%d,%d,%d,%d)", taskbar, DescribeWindow(target).c_str(), strip.left,
               strip.top, strip.right, strip.bottom);
    }

    std::vector<uint32_t> px;
    if (!CaptureStrip(strip, px) || px.empty()) {
        HandleBlocked(st, taskbar, target);
        return;
    }

    uint64_t hash = HashPixels(px);
    Rgb sampled;
    if (st.haveLastSample && hash == st.lastHash) {
        sampled = st.lastSample;
    } else {
        sampled = DominantColor(px);
        st.lastHash = hash;
        st.lastSample = sampled;
        st.haveLastSample = true;
    }

    Smooth(st, taskbar, sampled, targetChanged, s);
}

void SetPollTimer(bool enable, int intervalMs) {
    if (enable) {
        if (g_pollTimer && g_pollTimerInterval == intervalMs) return;
        if (g_pollTimer) KillTimer(nullptr, g_pollTimer);
        g_pollTimer = SetTimer(nullptr, 0, (UINT)intervalMs, nullptr);
        g_pollTimerInterval = intervalMs;
    } else if (g_pollTimer) {
        KillTimer(nullptr, g_pollTimer);
        g_pollTimer = 0;
        g_pollTimerInterval = 0;
    }
}

void Tick() {
    if (g_unloading) return;
    Settings s = GetSettings();
    auto taskbars = SnapshotIslandWindows();

    EnumCtx ctx{&s, {}};
    EnumWindows(EnumWindowsForemostProc, reinterpret_cast<LPARAM>(&ctx));

    bool anyTarget = false;
    for (HWND tb : taskbars) ProcessTaskbar(tb, s, ctx.foremost, anyTarget);

    // Drop state for taskbars that disappeared.
    for (auto it = g_track.begin(); it != g_track.end();) {
        bool alive = std::find(taskbars.begin(), taskbars.end(), it->first) != taskbars.end();
        it = alive ? std::next(it) : g_track.erase(it);
    }

    SetPollTimer(anyTarget, s.pollIntervalMs);
}

void ScheduleDebouncedTick() {
    if (g_debounceTimer) KillTimer(nullptr, g_debounceTimer);
    g_debounceTimer = SetTimer(nullptr, 0, 60, nullptr);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event >= EVENT_OBJECT_CREATE) {  // object events: top-level windows only
        if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
        if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return;
    }
    ScheduleDebouncedTick();
}

void AddWinEventHook(DWORD minEvent, DWORD maxEvent) {
    HWINEVENTHOOK h = SetWinEventHook(minEvent, maxEvent, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (h) g_winEventHooks.push_back(h);
    else Wh_Log(L"SetWinEventHook(%04X-%04X) failed", minEvent, maxEvent);
}

DWORD WINAPI TrackerThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);  // physical pixels everywhere

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the queue
    SetEvent(g_trackerReady);

    AddWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND);
    AddWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND);
    AddWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND);
    AddWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH);
    AddWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE);  // DESTROY, SHOW, HIDE
    AddWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE);
    AddWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED);

    ScheduleDebouncedTick();

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER && !msg.hwnd) {
            if (msg.wParam == g_debounceTimer) {
                KillTimer(nullptr, g_debounceTimer);
                g_debounceTimer = 0;
                Tick();
            } else if (msg.wParam == g_pollTimer) {
                Tick();
            }
        } else if (msg.message == WM_TRACKER_TICK) {
            ScheduleDebouncedTick();
        } else if (msg.message == WM_TRACKER_SETTINGS) {
            SetPollTimer(false, 0);
            for (auto& kv : g_track) kv.second.haveLastSample = false;  // re-run the color pipeline
            Tick();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    for (HWINEVENTHOOK h : g_winEventHooks) UnhookWinEvent(h);
    g_winEventHooks.clear();
    if (g_debounceTimer) KillTimer(nullptr, g_debounceTimer);
    if (g_pollTimer) KillTimer(nullptr, g_pollTimer);
    g_debounceTimer = g_pollTimer = 0;
    g_track.clear();
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

// The panel fills the taskbar; margins are in screen orientation, measured from
// the taskbar edges (positive insets, negative extends).
void UpdateExtent(Island& isl, const Settings& s) {
    if (!isl.panel) return;
    auto& p = isl.panel;
    p.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    p.VerticalAlignment(wux::VerticalAlignment::Stretch);
    p.Margin(wux::Thickness{(double)s.marginLeft, (double)s.marginTop, (double)s.marginRight, (double)s.marginBottom});
    // CornerRadius{TopLeft, TopRight, BottomRight, BottomLeft}, screen orientation.
    p.CornerRadius(wux::CornerRadius{(double)s.radiusTopLeft, (double)s.radiusTopRight, (double)s.radiusBottomRight,
                                     (double)s.radiusBottomLeft});
}

// No animations: the panel's brush and opacity are plain local values. (A
// storyboard's held value would override local sets, so none are ever used.)
void SetPanelColor(Island& isl, wu::Color c) {
    if (isl.brush) isl.brush.Color(c);
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
// starts out-contrasting white around here), with an 8-level hysteresis band so
// a color sitting on the threshold can't flip the theme back and forth.
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

void WINAPI ApplyIslandUpdateOnTaskbarThread(void* param) try {
    if (g_unloading) return;
    auto& upd = *static_cast<IslandUpdate*>(param);
    auto isl = FindIslandByHwnd(upd.hwnd);
    if (!isl || !isl->panel || !isl->panel.Parent()) return;

    Settings s = GetSettings();
    if (!isl->shown) UpdateExtent(*isl, s);

    if (upd.active) {
        if (!isl->shown || isl->color != upd.color) SetPanelColor(*isl, ToColor(upd.color));
        if (!isl->shown) SetPanelVisible(*isl, true);
        isl->color = upd.color;
        SetStrokeHidden(*isl, true);
        ApplyTheme(*isl, true, upd.color);
    } else if (isl->shown) {
        SetPanelVisible(*isl, false);
        SetStrokeHidden(*isl, false);
        ApplyTheme(*isl, false, upd.color);
    }
    isl->shown = upd.active;
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
    isl.shown = false;
} catch (winrt::hresult_error const& e) {
    Wh_Log(L"teardown failed: %08X", e.code().value);
} catch (...) {
    Wh_Log(L"teardown failed");
}

// Drops islands whose taskbar window is gone (monitor unplugged) or whose
// panel lost its parent (frame re-templated), so a rebuilt frame on the same
// window can be attached again and the tracker stops serving dead windows.
bool IsPanelParented(Island const& isl) {
    try {
        return isl.panel && isl.panel.Parent();
    } catch (...) {
        return false;
    }
}

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

    Settings s = GetSettings();
    auto isl = std::make_shared<Island>();
    isl->hwnd = taskbarWnd;
    isl->frameId = frameId;
    isl->brush = wuxm::SolidColorBrush(wu::Color{255, 0, 0, 0});

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
    UpdateExtent(*isl, s);

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

void WINAPI RefreshIslandsOnTaskbarThread(void*) {
    if (g_unloading) return;
    AttachAllTaskbars(nullptr);  // picks up taskbars added since, prunes dead ones
    std::vector<std::shared_ptr<Island>> all;
    {
        std::lock_guard lock(g_islandsMutex);
        if (g_islands) all = *g_islands;
    }
    Settings s = GetSettings();
    for (auto& isl : all) {
        try {
            UpdateExtent(*isl, s);
        } catch (...) {
        }
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

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, RefreshIslandsOnTaskbarThread, nullptr);
    }
}
