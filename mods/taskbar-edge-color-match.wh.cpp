// ==WindhawkMod==
// @id              taskbar-edge-color-match
// @name            Taskbar Edge Color Match
// @description     Standalone mod that matches the Windows 11 taskbar color to the edge strip, majority screen color, or mixed mode, with true transparency on desktop, rounded corners, and top-to-bottom fade animation.
// @version         2.5.69
// @author          Nitin Patel
// @github          https://github.com/nitinp94
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lversion -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Edge Color Match

Matches the Windows 11 taskbar background color in real time—**standalone**, with no `Windows 11 Taskbar Styler` dependency.

## Features
- **Works Out of the Box**: Starts matching the taskbar color right after installation.
- **Maximized App Majority Priority**: Prioritizes maximized windows on the monitor (ignoring minimized windows) and samples the majority color from the maximized app even when a floating/unmaximized window is on top.
- **YouTube Player & Gemini Edge Logic in All Modes**: Automatically uses edge-strip sampling when a YouTube watch/player tab or Gemini window is active across Edge Strip (`0`), Majority (`1`), and Mixed (`2`) modes.
- **Edge Strip, Majority, or Mixed Mode**: Choose Edge Strip (`0`), Majority Screen Color (`1`), or Mixed (`2`, using Edge Strip on YouTube player/Gemini or when a dominant color covers at least 65% of the edge strip on non-white side margins and drops below 45% to return to Majority, and Majority Screen Color otherwise).
- **Side-Margin White Priority**: In Majority and Mixed modes, if both left and right screen/window margins are white/near-white while a non-white majority bucket is calculated, prioritizes white instead of letting a centered card/banner hijack the color.
- **App-Switch Stutter Reduction**: Avoids `PrintWindow` DWM readback stalls during app switches, caches window/side scans cleanly, and minimizes UI thread hook contention.
- **Scroll-Flash Debounce**: Filters out split-second transient color spikes during fast scrolling on image-heavy pages while staying snappy on real transitions and window switches.
- **Top-to-Bottom Color Fade Animation**: Sweeps/fades newly sampled colors smoothly from the top of the taskbar down to the bottom.
- **True Transparency on Desktop**: Controlled by **Transparent when on desktop**—hides the panel on desktop when enabled, and stays visible at your configured panel opacity when disabled.
- **Rounded Corners with Transparent Outer Area**: Rounds the panel corners and hides the underlying rectangular taskbar fill when rounded so the outer corner area stays transparent.
- **Tray-Aware Transparency Contrast**: When transparent on the desktop, adapts system tray and battery icon contrast to whether the screen behind the tray is light or dark.
- **Start / Flyout Stability**: Holds and re-asserts the current color during Start menu / flyout transitions so the bar doesn't flash black or white.
- **Targeted Explorer Restart**: Restarts `explorer.exe` safely only on Windows theme changes (when enabled), when **Transparent when on desktop** changes, and when unapplying the mod; all other settings apply live.
- **Optional Cosmetics & Contrast**: Brightness clamping, panel opacity, panel corner radius, top-to-bottom fade animation, adaptive light/dark taskbar icons (including battery contrast), and fullscreen pause.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- colorSamplingMode: 0
  $name: Color sampling mode (0 = Edge Strip, 1 = Majority, 2 = Mixed)
  $description: Choose 0 for Edge Strip, 1 for Majority Screen Color, or 2 for Mixed (uses Edge Strip on YouTube player/Gemini or uniform edge strips >= 65% with < 45% hysteresis exit, and Majority Screen Color otherwise).
- topToBottomAnimation: true
  $name: Top-to-bottom color fade animation
  $description: Animates color changes from the top edge of the taskbar down to the bottom edge.
- animationSpeedPercent: 75
  $name: Top-to-bottom animation speed (10-100%)
  $description: Controls how fast the top-to-bottom fade/sweep completes (100% is fast and smooth).
- transparentOnDesktop: true
  $name: Transparent when on desktop
  $description: Makes the taskbar transparent in any theme when on the desktop (including when windows are minimized or tray icons are hovered/clicked). When off, the panel stays visible at Panel opacity.
- panelOpacityPercent: 100
  $name: Panel opacity (100-100%)
  $description: Controls the opacity of the color-matched taskbar background panel when active.
- panelCornerRadius: 0
  $name: Panel corner radius (0-24 px)
  $description: Rounds the corners of the color-matched taskbar background panel. When > 0, the rectangular base fill behind the corners is hidden so the outer corner area stays transparent.
- softenExtremeBrightness: false
  $name: Guard against extreme brightness
  $description: Keeps sampled R, G, and B values within a safe range when enabled. Useful if pure black (#000000) or blinding white (#FFFFFF) makes taskbar icons hard to read or looks too harsh.
- minChannel: 16
  $name: Minimum RGB channel (0-100)
  $description: Only used when "Guard against extreme brightness" is ON. Prevents any R, G, or B value from dropping below this number (e.g., 16-30 avoids pitch-black crushing; set to 0 for no lower limit).
- maxChannel: 242
  $name: Maximum RGB channel (150-255)
  $description: Only used when "Guard against extreme brightness" is ON. Prevents any R, G, or B value from going above this number (e.g., 220-242 softens harsh bright white; set to 255 for no upper limit).
- adaptiveIconTheme: true
  $name: Adaptive light/dark taskbar icons
  $description: Automatically switches taskbar text, icons, and battery contrast between dark and light based on the matched background brightness (or screen behind tray when transparent).
- pauseOnFullscreen: true
  $name: Pause on fullscreen windows
  $description: Skips screen sampling when a fullscreen app covers the monitor.
- restartExplorerOnThemeChange: true
  $name: Restart Explorer on Windows theme change
  $description: Automatically restarts explorer.exe when Windows switches between Light and Dark theme.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <winreg.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wu = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;

// Balanced polling avoids UI thread contention during app switches while keeping animations crisp
static constexpr DWORD IDLE_POLL_INTERVAL_MS = 75;
static constexpr DWORD MIXED_POLL_INTERVAL_MS = 34;
static constexpr DWORD EDGE_POLL_INTERVAL_MS = 12;
static constexpr DWORD ANIM_POLL_INTERVAL_MS = 18;
static constexpr ULONGLONG FLYOUT_CLOSE_GRACE_MS = 120;
static constexpr ULONGLONG kCommitCooldownMs = 130;
static constexpr ULONGLONG kEdgeDebounceMs = 55;
static constexpr ULONGLONG kVideoHoldGraceMs = 2000;
static constexpr ULONGLONG kWindowScanCacheMs = 180;
static constexpr ULONGLONG kSideCheckCacheMs = 300;
static constexpr ULONGLONG kMixedMajorityCacheMs = 260;
static constexpr ULONGLONG kAppSwitchSettleMs = 180;
static constexpr ULONGLONG kEarlyStartupWindowMs = 3500;
static constexpr int kLightThreshold = 135;
static constexpr int kThemeHysteresis = 8;

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_stop{false};
static std::atomic<bool> g_symbolsHooked{false};
static std::atomic<bool> g_taskbarViewHooked{false};
static std::atomic<bool> g_systemTrayHooked{false};
static std::atomic<bool> g_hasCachedXamlRoot{false};
static HANDLE g_workerThread = nullptr;
static HANDLE g_wakeEvent = nullptr;

static std::atomic<int> g_colorSamplingMode{0};
static std::atomic<bool> g_topToBottomAnimation{true};
static std::atomic<int> g_animationSpeedPercent{75};
static std::atomic<bool> g_transparentOnDesktop{true};
static std::atomic<int> g_panelOpacityPercent{100};
static std::atomic<int> g_panelCornerRadius{0};
static std::atomic<bool> g_softenExtremeBrightness{false};
static std::atomic<int> g_minChannel{16};
static std::atomic<int> g_maxChannel{242};
static std::atomic<bool> g_adaptiveIconTheme{true};
static std::atomic<bool> g_pauseOnFullscreen{true};
static std::atomic<bool> g_restartExplorerOnThemeChange{true};
static std::atomic<bool> g_restartingExplorer{false};
static std::atomic<bool> g_forceRefresh{false};
static std::atomic<bool> g_animRunningGlobal{false};
static std::atomic<ULONGLONG> g_lastFgChangeTick{0};
static std::atomic<ULONGLONG> g_modInitTick{0};
static std::atomic<HWND> g_lastObservedFg{nullptr};

// Forward declarations
HWND FindCurrentProcessTaskbarWnd();
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);
bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam);
bool PostToWindowThreadAsync(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam);
void WINAPI EnsureTaskbarAsyncDispatcherOnUiThread(void*);
void WINAPI CleanupTaskbarAsyncDispatcherOnUiThread(void*);
void WINAPI CleanupIslandsOnTaskbarThread(void*);

static void SignalWorkerWakeup() {
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

static void SafeRestartExplorer(PCWSTR reason) {
    if (g_restartingExplorer.exchange(true)) return;

    Wh_Log(L"Taskbar Edge Color Match: safely restarting explorer.exe (%s)", reason);
    g_unloading.store(true);
    g_stop.store(true);
    SignalWorkerWakeup();

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, CleanupIslandsOnTaskbarThread, nullptr);
        RunFromWindowThread(hTaskbarWnd, CleanupTaskbarAsyncDispatcherOnUiThread, nullptr);
    }

    WCHAR sysDir[MAX_PATH]{};
    if (GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir)) > 0) {
        WCHAR cmdLine[MAX_PATH * 2]{};
        swprintf_s(
            cmdLine,
            L"\"%s\\cmd.exe\" /c ping -n 2 127.0.0.1 >nul & start \"\" \"%s\\explorer.exe\"",
            sysDir, sysDir);

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        PROCESS_INFORMATION pi{};
        if (CreateProcessW(
                nullptr, cmdLine, nullptr, nullptr, FALSE,
                CREATE_NO_WINDOW | DETACHED_PROCESS, nullptr, nullptr, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

    ExitProcess(0);
}

static void LoadSettings() {
    g_colorSamplingMode.store(
        std::clamp(Wh_GetIntSetting(L"colorSamplingMode"), 0, 2));
    g_topToBottomAnimation.store(
        Wh_GetIntSetting(L"topToBottomAnimation") != 0);
    g_animationSpeedPercent.store(
        std::clamp(Wh_GetIntSetting(L"animationSpeedPercent"), 10, 100));
    g_transparentOnDesktop.store(Wh_GetIntSetting(L"transparentOnDesktop") != 0);
    g_panelOpacityPercent.store(
        std::clamp(Wh_GetIntSetting(L"panelOpacityPercent"), 10, 100));
    g_panelCornerRadius.store(
        std::clamp(Wh_GetIntSetting(L"panelCornerRadius"), 0, 24));

    g_softenExtremeBrightness.store(
        Wh_GetIntSetting(L"softenExtremeBrightness") != 0);

    int lo = std::clamp(Wh_GetIntSetting(L"minChannel"), 0, 100);
    int hi = std::clamp(Wh_GetIntSetting(L"maxChannel"), 150, 255);
    if (lo >= hi) {
        lo = 16;
        hi = 242;
    }
    g_minChannel.store(lo);
    g_maxChannel.store(hi);

    g_adaptiveIconTheme.store(Wh_GetIntSetting(L"adaptiveIconTheme") != 0);
    g_pauseOnFullscreen.store(Wh_GetIntSetting(L"pauseOnFullscreen") != 0);
    g_restartExplorerOnThemeChange.store(
        Wh_GetIntSetting(L"restartExplorerOnThemeChange") != 0);
    g_forceRefresh.store(true);
    SignalWorkerWakeup();
}

// ============================================================================
// Taskbar-thread helpers & XamlRoot resolution
// ============================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            WCHAR className[32]{};
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

struct AsyncTaskbarDispatch {
    RunFromWindowThreadProc_t proc;
    void* procParam;
    bool ownsProcParam;
};

static std::mutex g_asyncDispatchMutex;
static std::vector<std::unique_ptr<AsyncTaskbarDispatch>> g_inflightAsync;
static std::atomic<LONG> g_pendingAsyncUiUpdates{0};
static std::atomic<HWND> g_uiThreadDispatcherWnd{nullptr};

static UINT GetAsyncRunMsg() {
    static const UINT asyncMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThreadAsync_" WH_MOD_ID);
    return asyncMsg;
}

static LRESULT CALLBACK AsyncTaskbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == GetAsyncRunMsg() && lParam) {
        auto* raw = reinterpret_cast<AsyncTaskbarDispatch*>(lParam);
        std::unique_ptr<AsyncTaskbarDispatch> item;
        {
            std::lock_guard lock(g_asyncDispatchMutex);
            for (auto it = g_inflightAsync.begin(); it != g_inflightAsync.end(); ++it) {
                if (it->get() == raw) {
                    item = std::move(*it);
                    g_inflightAsync.erase(it);
                    break;
                }
            }
        }
        if (item) {
            if (item->proc && !g_unloading.load()) {
                try {
                    item->proc(item->procParam);
                } catch (...) {
                }
            }
            if (item->ownsProcParam) {
                delete static_cast<struct PanelColorUpdate*>(item->procParam);
            }
        }
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void WINAPI EnsureTaskbarAsyncDispatcherOnUiThread(void*) {
    if (g_uiThreadDispatcherWnd.load() && IsWindow(g_uiThreadDispatcherWnd.load())) return;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = AsyncTaskbarWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkAsyncUiDispatcher_" WH_MOD_ID;
    RegisterClassExW(&wc);

    HWND disp = CreateWindowExW(
        0, wc.lpszClassName, L"", 0, 0, 0, 0, 0,
        HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (disp) {
        g_uiThreadDispatcherWnd.store(disp);
    }
}

void WINAPI CleanupTaskbarAsyncDispatcherOnUiThread(void*) {
    HWND disp = g_uiThreadDispatcherWnd.exchange(nullptr);
    if (disp && IsWindow(disp)) {
        DestroyWindow(disp);
    }
}

bool PostToWindowThreadAsync(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    if (!hWnd || !IsWindow(hWnd) || !proc || g_unloading.load()) return false;

    HWND disp = g_uiThreadDispatcherWnd.load();
    if (!disp || !IsWindow(disp)) {
        RunFromWindowThread(hWnd, EnsureTaskbarAsyncDispatcherOnUiThread, nullptr);
        disp = g_uiThreadDispatcherWnd.load();
        if (!disp || !IsWindow(disp)) return false;
    }

    auto item = std::make_unique<AsyncTaskbarDispatch>();
    item->proc = proc;
    item->procParam = procParam;
    item->ownsProcParam = true;

    AsyncTaskbarDispatch* raw = item.get();
    {
        std::lock_guard lock(g_asyncDispatchMutex);
        g_inflightAsync.push_back(std::move(item));
    }

    if (!PostMessageW(disp, GetAsyncRunMsg(), 0, reinterpret_cast<LPARAM>(raw))) {
        std::lock_guard lock(g_asyncDispatchMutex);
        for (auto it = g_inflightAsync.begin(); it != g_inflightAsync.end(); ++it) {
            if (it->get() == raw) {
                g_inflightAsync.erase(it);
                break;
            }
        }
        return false;
    }
    return true;
}

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    if (!hWnd || !IsWindow(hWnd) || g_unloading.load()) return false;

    static const UINT runMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runMsg && cwp->lParam) {
                    auto* param = reinterpret_cast<PARAM*>(cwp->lParam);
                    if (param->proc) {
                        param->proc(param->procParam);
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) return false;

    PARAM param{proc, procParam};
    LRESULT res = SendMessageW(hWnd, runMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return res != 0 || GetLastError() == 0;
}

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

void* TaskbarHost_FrameHeight_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original = nullptr;

static std::mutex g_cachedXamlRootsMutex;
static std::vector<winrt::weak_ref<wux::XamlRoot>> g_cachedXamlRoots;

void RegisterDiscoveredXamlRoot(wux::XamlRoot const& xr) {
    if (!xr) return;
    std::lock_guard lock(g_cachedXamlRootsMutex);
    for (auto it = g_cachedXamlRoots.begin(); it != g_cachedXamlRoots.end();) {
        auto existing = it->get();
        if (!existing) {
            it = g_cachedXamlRoots.erase(it);
        } else if (existing == xr) {
            g_hasCachedXamlRoot.store(true);
            return;
        } else {
            ++it;
        }
    }
    g_cachedXamlRoots.emplace_back(xr);
    g_hasCachedXamlRoot.store(true);
}

void CaptureXamlRootFromProduceElement(void* pThis) {
    if (g_unloading.load() || !pThis || g_hasCachedXamlRoot.load()) return;
    try {
        wux::FrameworkElement fe = nullptr;
        ((IUnknown*)pThis)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(),
            winrt::put_abi(fe));
        if (fe) {
            if (auto xr = fe.XamlRoot()) {
                RegisterDiscoveredXamlRoot(xr);
            }
        }
    } catch (...) {
    }
}

using TaskbarFrame_MeasureOverride_t = int(WINAPI*)(void* pThis, wf::Size size, wf::Size* resultSize);
static TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original = nullptr;

static int WINAPI TaskbarFrame_MeasureOverride_Hook(void* pThis, wf::Size size, wf::Size* resultSize) {
    if (!g_hasCachedXamlRoot.load()) {
        CaptureXamlRootFromProduceElement(pThis);
    }
    return TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
}

using SystemTrayFrame_MeasureOverride_t = int(WINAPI*)(void* pThis, wf::Size size, wf::Size* resultSize);
static SystemTrayFrame_MeasureOverride_t SystemTrayFrame_MeasureOverride_Original = nullptr;

static int WINAPI SystemTrayFrame_MeasureOverride_Hook(void* pThis, wf::Size size, wf::Size* resultSize) {
    if (!g_hasCachedXamlRoot.load()) {
        CaptureXamlRootFromProduceElement(pThis);
    }
    return SystemTrayFrame_MeasureOverride_Original(pThis, size, resultSize);
}

wux::XamlRoot FindFallbackXamlRootForTaskbar(HWND hTaskbarWnd) {
    RECT tbRect{};
    if (!hTaskbarWnd || !GetWindowRect(hTaskbarWnd, &tbRect)) return nullptr;

    std::vector<wux::XamlRoot> candidates;
    {
        std::lock_guard lock(g_cachedXamlRootsMutex);
        for (auto it = g_cachedXamlRoots.begin(); it != g_cachedXamlRoots.end();) {
            if (auto xr = it->get()) {
                candidates.push_back(xr);
                ++it;
            } else {
                it = g_cachedXamlRoots.erase(it);
            }
        }
        if (candidates.empty()) {
            g_hasCachedXamlRoot.store(false);
        }
    }

    for (auto const& xr : candidates) {
        try {
            auto content = xr.Content().try_as<wux::FrameworkElement>();
            if (!content) continue;
            if (content.ActualWidth() > 100.0 && content.ActualHeight() > 10.0) {
                return xr;
            }
        } catch (...) {
        }
    }

    return candidates.empty() ? nullptr : candidates.front();
}

wux::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) return nullptr;
    if (!CTaskBand_GetTaskbarHost_Original) return nullptr;

    size_t offset = 0x10;
#if defined(_M_X64)
    const BYTE* b = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC &&
        b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        offset = b[7];
    }
#elif defined(_M_ARM64)
    const DWORD* p = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (p && p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (p[3] >> 12) & 0xFF;
    }
#endif

    auto* taskbarElementIUnknown =
        *reinterpret_cast<IUnknown**>(reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset);

    wux::FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(),
            winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (taskbarHostSharedPtr[1] && std__Ref_count_base__Decref_Original) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }
    return result;
}

wux::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (g_symbolsHooked.load() && CTaskBand_ITaskListWndSite_vftable &&
        CTaskBand_GetTaskbarHost_Original) {
        HWND hTaskSwWnd = reinterpret_cast<HWND>(GetPropW(hTaskbarWnd, L"TaskbandHWND"));
        if (hTaskSwWnd && IsWindow(hTaskSwWnd)) {
            void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
            if (taskBand) {
                void* site = taskBand;
                for (int i = 0; *reinterpret_cast<void**>(site) != CTaskBand_ITaskListWndSite_vftable; i++) {
                    if (i == 20) {
                        site = nullptr;
                        break;
                    }
                    site = reinterpret_cast<void**>(site) + 1;
                }
                if (site) {
                    void* sharedPtr[2]{};
                    CTaskBand_GetTaskbarHost_Original(site, sharedPtr);
                    if (auto xr = XamlRootFromTaskbarHostSharedPtr(sharedPtr)) {
                        RegisterDiscoveredXamlRoot(xr);
                        return xr;
                    }
                }
            }
        }
    }

    return FindFallbackXamlRootForTaskbar(hTaskbarWnd);
}

wux::XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    if (g_symbolsHooked.load() && CSecondaryTaskBand_ITaskListWndSite_vftable &&
        CSecondaryTaskBand_GetTaskbarHost_Original) {
        HWND hTaskSwWnd = FindWindowExW(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
        if (hTaskSwWnd && IsWindow(hTaskSwWnd)) {
            void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
            if (taskBand) {
                void* site = taskBand;
                for (int i = 0; *reinterpret_cast<void**>(site) != CSecondaryTaskBand_ITaskListWndSite_vftable; i++) {
                    if (i == 20) {
                        site = nullptr;
                        break;
                    }
                    site = reinterpret_cast<void**>(site) + 1;
                }
                if (site) {
                    void* sharedPtr[2]{};
                    CSecondaryTaskBand_GetTaskbarHost_Original(site, sharedPtr);
                    if (auto xr = XamlRootFromTaskbarHostSharedPtr(sharedPtr)) {
                        RegisterDiscoveredXamlRoot(xr);
                        return xr;
                    }
                }
            }
        }
    }

    return FindFallbackXamlRootForTaskbar(hSecondaryTaskbarWnd);
}

wux::XamlRoot XamlRootOfTaskbarWindow(HWND hWnd) {
    WCHAR cls[32]{};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return nullptr;
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) return GetTaskbarXamlRoot(hWnd);
    if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) return GetSecondaryTaskbarXamlRoot(hWnd);
    return nullptr;
}

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE mod = GetModuleHandleW(L"Taskbar.View.dll");
    if (!mod) mod = GetModuleHandleW(L"ExplorerExtensions.dll");
    return mod;
}

static HMODULE GetSystemTrayModuleHandle() {
    return GetModuleHandleW(L"SystemTray.dll");
}

static bool HookTaskbarViewMeasureOverride(HMODULE mod) {
    if (!mod) return false;
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
         reinterpret_cast<void**>(&TaskbarFrame_MeasureOverride_Original),
         reinterpret_cast<void*>(TaskbarFrame_MeasureOverride_Hook),
         true},
    };
    return WindhawkUtils::HookSymbols(mod, taskbarViewDllHooks, ARRAYSIZE(taskbarViewDllHooks));
}

static bool HookSystemTrayMeasureOverride(HMODULE mod) {
    if (!mod) return false;
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
         reinterpret_cast<void**>(&SystemTrayFrame_MeasureOverride_Original),
         reinterpret_cast<void*>(SystemTrayFrame_MeasureOverride_Hook),
         true},
    };
    return WindhawkUtils::HookSymbols(mod, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
}

static decltype(&LoadLibraryExW) LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE mod = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!mod || g_unloading.load()) return mod;

    if (g_taskbarViewHooked.load() && g_systemTrayHooked.load()) return mod;

    if (!g_taskbarViewHooked.load() && GetTaskbarViewModuleHandle() == mod) {
        if (HookTaskbarViewMeasureOverride(mod)) {
            g_taskbarViewHooked.store(true);
            Wh_ApplyHookOperations();
        }
    }
    if (!g_systemTrayHooked.load() && GetSystemTrayModuleHandle() == mod) {
        if (HookSystemTrayMeasureOverride(mod)) {
            g_systemTrayHooked.store(true);
            Wh_ApplyHookOperations();
        }
    }
    return mod;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Taskbar Edge Color Match: failed to load taskbar.dll (err=%lu)", GetLastError());
        return false;
    }

    struct SymbolEntry {
        PCWSTR symbol;
        void** target;
        PCWSTR label;
        bool required;
    };

    SymbolEntry entries[] = {
        {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})", &CTaskBand_ITaskListWndSite_vftable, L"CTaskBand vftable", true},
        {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )", reinterpret_cast<void**>(&CTaskBand_GetTaskbarHost_Original), L"CTaskBand::GetTaskbarHost", true},
        {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})", &CSecondaryTaskBand_ITaskListWndSite_vftable, L"CSecondaryTaskBand vftable", false},
        {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )", reinterpret_cast<void**>(&CSecondaryTaskBand_GetTaskbarHost_Original), L"CSecondaryTaskBand::GetTaskbarHost", false},
        {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )", &TaskbarHost_FrameHeight_Original, L"TaskbarHost::FrameHeight", false},
        {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))", reinterpret_cast<void**>(&std__Ref_count_base__Decref_Original), L"std::_Ref_count_base::_Decref", false},
    };

    bool anyHooked = false;
    for (const auto& e : entries) {
        WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[1] = {
            {{e.symbol}, e.target},
        };
        if (WindhawkUtils::HookSymbols(module, taskbarDllHooks, 1)) {
            anyHooked = true;
        }
    }

    return anyHooked;
}

// ============================================================================
// XAML Panel per Taskbar (owned by taskbar UI thread)
// ============================================================================

constexpr int ThemeNone = 0, ThemeLight = 1, ThemeDark = 2;

static wu::Color ColorRefToWuColor(COLORREF c) {
    return wu::Color{255, GetRValue(c), GetGValue(c), GetBValue(c)};
}

struct Island {
    HWND hwnd = nullptr;
    void* frameId = nullptr;
    winrt::weak_ref<wux::FrameworkElement> stroke;
    winrt::weak_ref<wux::FrameworkElement> fill;
    winrt::weak_ref<wux::FrameworkElement> themeRoot;
    std::vector<winrt::weak_ref<wux::DependencyObject>> cachedBatteryNodes;

    wuxc::Border panel{nullptr};
    wuxm::SolidColorBrush brush{nullptr};
    wuxm::SolidColorBrush batteryLightBrush{nullptr};
    wuxm::SolidColorBrush batteryDarkBrush{nullptr};
    wuxm::LinearGradientBrush gradBrush{nullptr};
    wuxm::GradientStop gradStop0{nullptr};
    wuxm::GradientStop gradStop1{nullptr};
    wuxm::GradientStop gradStop2{nullptr};
    wuxm::GradientStop gradStop3{nullptr};
    bool usingGradBrush = false;
    bool strokeHidden = false;
    wux::Visibility strokeOriginal = wux::Visibility::Visible;
    bool fillHidden = false;
    double fillOriginal = 1.0;
    int forcedTheme = ThemeNone;
    wux::ElementTheme originalTheme = wux::ElementTheme::Default;
    COLORREF currentColor = CLR_INVALID;
    COLORREF animFromColor = CLR_INVALID;
    COLORREF animToColor = CLR_INVALID;
    double animProgress = 1.0;
    ULONGLONG animLastTick = 0;
    bool animActive = false;

    int lastAppliedRadius = -1;
    int lastAppliedOpacityPct = -1;
    bool lastAppliedTransparent = false;
    int lastAppliedTheme = -999;
    bool lastAppliedThemeEnabled = false;
};

std::mutex g_islandsMutex;
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<Island>>> g_islands{std::in_place};

void* IdentityOf(wf::IInspectable const& o) {
    if (!o) return nullptr;
    auto unk = o.try_as<wf::IUnknown>();
    return winrt::get_abi(unk);
}

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

bool IsBatteryElement(wux::DependencyObject const& obj) {
    if (!obj) return false;
    auto cls = winrt::get_class_name(obj);
    if (std::wstring_view(cls).find(L"Battery") != std::wstring_view::npos) return true;
    if (auto fe = obj.try_as<wux::FrameworkElement>()) {
        auto name = fe.Name();
        if (std::wstring_view(name).find(L"Battery") != std::wstring_view::npos) return true;
    }
    return false;
}

void StyleBatterySubtree(wux::DependencyObject const& node, wuxm::SolidColorBrush const& contrastBrush, int depth) {
    if (!node || !contrastBrush || depth < 0) return;

    try {
        if (auto shape = node.try_as<wuxs::Shape>()) {
            if (shape.Stroke()) {
                shape.Stroke(contrastBrush);
            }
            if (shape.Fill()) {
                auto solid = shape.Fill().try_as<wuxm::SolidColorBrush>();
                if (solid && solid.Color().A > 20) {
                    shape.Fill(contrastBrush);
                }
            }
        } else if (auto border = node.try_as<wuxc::Border>()) {
            if (border.BorderBrush()) {
                border.BorderBrush(contrastBrush);
            }
        } else if (auto tb = node.try_as<wuxc::TextBlock>()) {
            tb.Foreground(contrastBrush);
        }
    } catch (...) {
    }

    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; i++) {
        StyleBatterySubtree(wuxm::VisualTreeHelper::GetChild(node, i), contrastBrush, depth - 1);
    }
}

void ApplyBatteryContrast(Island& isl, int wantTheme, bool enable) {
    if (!enable || wantTheme == ThemeNone) return;

    auto root = isl.themeRoot.get();
    if (!root && isl.panel) {
        if (auto xr = isl.panel.XamlRoot()) {
            root = xr.Content().try_as<wux::FrameworkElement>();
            isl.themeRoot = root;
        }
    }
    if (!root) return;

    if (!isl.batteryLightBrush) {
        isl.batteryLightBrush = wuxm::SolidColorBrush(wu::Color{255, 255, 255, 255});
    }
    if (!isl.batteryDarkBrush) {
        isl.batteryDarkBrush = wuxm::SolidColorBrush(wu::Color{255, 16, 16, 16});
    }
    auto contrastBrush = (wantTheme == ThemeDark) ? isl.batteryLightBrush : isl.batteryDarkBrush;

    bool hasLiveCache = false;
    for (auto it = isl.cachedBatteryNodes.begin(); it != isl.cachedBatteryNodes.end();) {
        if (auto node = it->get()) {
            hasLiveCache = true;
            StyleBatterySubtree(node, contrastBrush, 5);
            ++it;
        } else {
            it = isl.cachedBatteryNodes.erase(it);
        }
    }

    if (hasLiveCache) return;

    auto scanAndCache = [&](auto& self, wux::DependencyObject const& cur, int depth) -> void {
        if (!cur || depth < 0) return;
        if (IsBatteryElement(cur)) {
            isl.cachedBatteryNodes.emplace_back(cur);
            StyleBatterySubtree(cur, contrastBrush, 5);
            return;
        }
        int count = wuxm::VisualTreeHelper::GetChildrenCount(cur);
        for (int i = 0; i < count; i++) {
            self(self, wuxm::VisualTreeHelper::GetChild(cur, i), depth - 1);
        }
    };

    try {
        scanAndCache(scanAndCache, root, 12);
    } catch (...) {
    }
}

std::shared_ptr<Island> FindIslandByHwnd(HWND hwnd) {
    std::lock_guard lock(g_islandsMutex);
    if (!g_islands) return nullptr;
    for (auto& isl : *g_islands) {
        if (isl->hwnd == hwnd) return isl;
    }
    return nullptr;
}

std::shared_ptr<Island> FindIslandByFrameId(void* frameId) {
    std::lock_guard lock(g_islandsMutex);
    if (!g_islands) return nullptr;
    for (auto& isl : *g_islands) {
        if (isl->frameId == frameId) return isl;
    }
    return nullptr;
}

bool IsPanelParented(Island const& isl) {
    try {
        return isl.panel && isl.panel.Parent();
    } catch (...) {
        return false;
    }
}

void SetStrokeHidden(Island& isl, bool hide) {
    if (isl.strokeHidden == hide) return;
    auto stroke = isl.stroke.get();
    if (!stroke) {
        isl.strokeHidden = false;
        return;
    }
    if (hide) {
        isl.strokeOriginal = stroke.Visibility();
        stroke.Visibility(wux::Visibility::Collapsed);
    } else {
        stroke.Visibility(isl.strokeOriginal);
    }
    isl.strokeHidden = hide;
}

void SetFillHidden(Island& isl, bool hide) {
    if (isl.fillHidden == hide) return;
    auto fill = isl.fill.get();
    if (!fill) {
        isl.fillHidden = false;
        return;
    }
    if (hide) {
        isl.fillOriginal = fill.Opacity();
        fill.Opacity(0.0);
        isl.fillHidden = true;
    } else {
        fill.Opacity(isl.fillOriginal);
        isl.fillHidden = false;
    }
}

int PerceivedBrightness(COLORREF c) {
    return static_cast<int>(std::lround(
        0.299 * GetRValue(c) + 0.587 * GetGValue(c) + 0.114 * GetBValue(c)));
}

void ApplyAdaptiveTheme(Island& isl, COLORREF refColor, bool enable, bool forceApply) {
    int want = ThemeNone;
    if (enable && refColor != CLR_INVALID) {
        int lum = PerceivedBrightness(refColor);
        bool light = (isl.forcedTheme == ThemeLight) ? (lum >= kLightThreshold - kThemeHysteresis)
                   : (isl.forcedTheme == ThemeDark)  ? (lum > kLightThreshold + kThemeHysteresis)
                                                     : (lum > kLightThreshold);
        want = light ? ThemeLight : ThemeDark;
    }

    bool themeChanged = (want != isl.forcedTheme) || (enable != isl.lastAppliedThemeEnabled);
    if (!forceApply && !themeChanged) return;

    if (themeChanged) {
        wux::FrameworkElement root = isl.themeRoot.get();
        if (!root && isl.panel) {
            if (auto xr = isl.panel.XamlRoot()) {
                root = xr.Content().try_as<wux::FrameworkElement>();
                isl.themeRoot = root;
            }
        }
        if (root) {
            if (isl.forcedTheme == ThemeNone && want != ThemeNone) {
                isl.originalTheme = root.RequestedTheme();
            }

            if (want != isl.forcedTheme) {
                isl.forcedTheme = want;
                if (want == ThemeLight) {
                    root.RequestedTheme(wux::ElementTheme::Light);
                } else if (want == ThemeDark) {
                    root.RequestedTheme(wux::ElementTheme::Dark);
                } else {
                    root.RequestedTheme(isl.originalTheme);
                }
            }
        }
        ApplyBatteryContrast(isl, want, enable);
    }
    isl.lastAppliedTheme = want;
    isl.lastAppliedThemeEnabled = enable;
}

void ApplyCosmeticsToPanel(Island& isl, bool transparent, bool forceApply) {
    if (!isl.panel) return;
    int radiusInt = g_panelCornerRadius.load();
    int opacityInt = g_panelOpacityPercent.load();

    if (!forceApply &&
        radiusInt == isl.lastAppliedRadius &&
        opacityInt == isl.lastAppliedOpacityPct &&
        transparent == isl.lastAppliedTransparent) {
        return;
    }

    double radius = static_cast<double>(radiusInt);
    isl.panel.CornerRadius(wux::CornerRadius{radius, radius, radius, radius});
    if (transparent) {
        isl.panel.Opacity(0.0);
    } else {
        double opacity = static_cast<double>(opacityInt) / 100.0;
        isl.panel.Opacity(std::clamp(opacity, 0.1, 1.0));
    }

    isl.lastAppliedRadius = radiusInt;
    isl.lastAppliedOpacityPct = opacityInt;
    isl.lastAppliedTransparent = transparent;
}

static void EnsureGradientBrushInitialized(Island& isl) {
    if (isl.gradBrush) return;
    isl.gradBrush = wuxm::LinearGradientBrush();
    isl.gradBrush.StartPoint(wf::Point{0.5, 0.0});
    isl.gradBrush.EndPoint(wf::Point{0.5, 1.0});

    isl.gradStop0 = wuxm::GradientStop();
    isl.gradStop1 = wuxm::GradientStop();
    isl.gradStop2 = wuxm::GradientStop();
    isl.gradStop3 = wuxm::GradientStop();

    isl.gradStop0.Offset(0.0);
    isl.gradStop1.Offset(0.0);
    isl.gradStop2.Offset(0.0);
    isl.gradStop3.Offset(1.0);

    isl.gradBrush.GradientStops().Append(isl.gradStop0);
    isl.gradBrush.GradientStops().Append(isl.gradStop1);
    isl.gradBrush.GradientStops().Append(isl.gradStop2);
    isl.gradBrush.GradientStops().Append(isl.gradStop3);
}

void ApplyTopToBottomBrush(Island& isl, COLORREF solidColor, bool animateTopToBottom, int speedPct) {
    if (!isl.panel) return;

    if (!isl.brush) {
        isl.brush = wuxm::SolidColorBrush(ColorRefToWuColor(solidColor));
    }

    if (!animateTopToBottom || isl.animFromColor == CLR_INVALID || isl.animToColor == CLR_INVALID) {
        isl.animActive = false;
        g_animRunningGlobal.store(false);
        isl.animProgress = 1.0;
        if (isl.currentColor != solidColor || isl.usingGradBrush) {
            isl.brush.Color(ColorRefToWuColor(solidColor));
        }
        if (isl.usingGradBrush) {
            isl.panel.Background(isl.brush);
            isl.usingGradBrush = false;
        }
        return;
    }

    ULONGLONG now = GetTickCount64();
    double dtMs = (isl.animLastTick > 0 && now >= isl.animLastTick)
        ? static_cast<double>(std::min<ULONGLONG>(now - isl.animLastTick, 100))
        : static_cast<double>(ANIM_POLL_INTERVAL_MS);
    isl.animLastTick = now;

    double normSpeed = std::clamp(static_cast<double>(speedPct) / 100.0, 0.1, 1.0);
    double durationMs = 160.0 - normSpeed * 100.0;
    isl.animProgress = std::min(1.0, isl.animProgress + (dtMs / durationMs));

    if (isl.animProgress >= 1.0) {
        isl.animActive = false;
        g_animRunningGlobal.store(false);
        isl.brush.Color(ColorRefToWuColor(isl.animToColor));
        if (isl.usingGradBrush) {
            isl.panel.Background(isl.brush);
            isl.usingGradBrush = false;
        }
        return;
    }

    g_animRunningGlobal.store(true);
    EnsureGradientBrushInitialized(isl);

    double p = isl.animProgress;
    double eased = 1.0 - (1.0 - p) * (1.0 - p);
    double feather = 0.20;
    double stopTop = std::clamp(eased - feather * 0.5, 0.0, 1.0);
    double stopBottom = std::clamp(eased + feather * 0.5, 0.0, 1.0);

    wu::Color newCol = ColorRefToWuColor(isl.animToColor);
    wu::Color oldCol = ColorRefToWuColor(isl.animFromColor);

    isl.gradStop0.Color(newCol);
    isl.gradStop0.Offset(0.0);

    isl.gradStop1.Color(newCol);
    isl.gradStop1.Offset(stopTop);

    isl.gradStop2.Color(oldCol);
    isl.gradStop2.Offset(stopBottom);

    isl.gradStop3.Color(oldCol);
    isl.gradStop3.Offset(1.0);

    if (!isl.usingGradBrush) {
        isl.panel.Background(isl.gradBrush);
        isl.usingGradBrush = true;
    }
}

void TeardownIsland(Island& isl) try {
    if (auto root = isl.themeRoot.get()) {
        if (isl.forcedTheme != ThemeNone) {
            isl.forcedTheme = ThemeNone;
            root.RequestedTheme(isl.originalTheme);
        }
    }
    SetStrokeHidden(isl, false);
    SetFillHidden(isl, false);
    if (isl.panel) {
        if (auto parent = isl.panel.Parent().try_as<wuxc::Panel>()) {
            uint32_t idx = 0;
            if (parent.Children().IndexOf(isl.panel, idx)) {
                parent.Children().RemoveAt(idx);
            }
        }
    }
    isl.cachedBatteryNodes.clear();
    isl.panel = nullptr;
    isl.brush = nullptr;
    isl.batteryLightBrush = nullptr;
    isl.batteryDarkBrush = nullptr;
    isl.gradBrush = nullptr;
    isl.gradStop0 = nullptr;
    isl.gradStop1 = nullptr;
    isl.gradStop2 = nullptr;
    isl.gradStop3 = nullptr;
    isl.usingGradBrush = false;
} catch (...) {
}

void PruneDeadIslands() {
    std::vector<std::shared_ptr<Island>> dead;
    {
        std::lock_guard lock(g_islandsMutex);
        if (!g_islands) return;
        for (auto it = g_islands->begin(); it != g_islands->end();) {
            if (!IsWindow((*it)->hwnd) || !IsPanelParented(**it)) {
                dead.push_back(*it);
                it = g_islands->erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto& isl : dead) {
        TeardownIsland(*isl);
    }
}

bool AttachIsland(wux::FrameworkElement const& rep, HWND taskbarWnd) {
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
    if (!frame || !grid || !anchor) return false;

    void* frameId = IdentityOf(frame);
    if (auto existing = FindIslandByFrameId(frameId); existing && IsPanelParented(*existing)) {
        return true;
    }

    auto isl = std::make_shared<Island>();
    isl->hwnd = taskbarWnd;
    isl->frameId = frameId;
    isl->brush = wuxm::SolidColorBrush(wu::Color{255, 32, 32, 32});

    wuxc::Border panel;
    panel.Name(L"TaskbarEdgeMatchPanel");
    panel.IsHitTestVisible(false);
    panel.Background(isl->brush);
    isl->panel = panel;
    ApplyCosmeticsToPanel(*isl, false, true);

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
        if (!inserted) return false;
    }

    try {
        isl->stroke = FindDescendantByName(grid, L"BackgroundStroke", 6);
        isl->fill = FindDescendantByName(grid, L"BackgroundFill", 6);
        SetStrokeHidden(*isl, true);
    } catch (...) {
        TeardownIsland(*isl);
        return false;
    }

    {
        std::lock_guard lock(g_islandsMutex);
        if (!g_islands) {
            TeardownIsland(*isl);
            return false;
        }
        g_islands->push_back(isl);
    }

    return true;
}

bool IsTaskbarWindowClass(HWND hWnd) {
    WCHAR cls[32]{};
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

void WINAPI AttachAllTaskbars(void*) try {
    if (g_unloading.load()) return;
    PruneDeadIslands();
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            if (!IsTaskbarWindowClass(hWnd)) return TRUE;
            try {
                auto xamlRoot = XamlRootOfTaskbarWindow(hWnd);
                if (!xamlRoot) return TRUE;
                auto rep = FindDescendantByName(xamlRoot.Content(), L"TaskbarFrameRepeater", 12);
                if (!rep) return TRUE;
                AttachIsland(rep, hWnd);
            } catch (...) {
            }
            return TRUE;
        },
        0);
} catch (...) {
}

struct PanelColorUpdate {
    HWND hwnd;
    COLORREF color;
    COLORREF trayBgColor;
    bool adaptiveTheme;
    bool forceReassert;
    bool transparentNow;
    bool hideRectangularFillForCorners;
    bool startTopToBottomAnim;
    bool topToBottomAnimEnabled;
    int animSpeedPct;
};

void WINAPI ApplyPanelColorOnTaskbarThread(void* param) try {
    struct PendingGuard {
        ~PendingGuard() { g_pendingAsyncUiUpdates.fetch_sub(1); }
    } pendingGuard;

    if (g_unloading.load() || !param) return;
    auto& upd = *static_cast<PanelColorUpdate*>(param);
    auto isl = FindIslandByHwnd(upd.hwnd);
    if (!isl || !IsPanelParented(*isl)) {
        AttachAllTaskbars(nullptr);
        isl = FindIslandByHwnd(upd.hwnd);
    }
    if (!isl || !isl->panel) return;

    if (upd.startTopToBottomAnim && upd.topToBottomAnimEnabled && !upd.transparentNow &&
        isl->currentColor != CLR_INVALID && isl->currentColor != upd.color) {
        isl->animFromColor = isl->currentColor;
        isl->animToColor = upd.color;
        isl->animProgress = 0.0;
        isl->animLastTick = GetTickCount64();
        isl->animActive = true;
        g_animRunningGlobal.store(true);
    } else if (!upd.topToBottomAnimEnabled || upd.transparentNow) {
        isl->animActive = false;
        isl->animProgress = 1.0;
    }

    if (isl->panel.Visibility() != wux::Visibility::Visible) {
        isl->panel.Visibility(wux::Visibility::Visible);
    }
    ApplyCosmeticsToPanel(*isl, upd.transparentNow, upd.forceReassert);
    SetFillHidden(*isl, upd.transparentNow || upd.hideRectangularFillForCorners);

    bool runAnim = upd.topToBottomAnimEnabled && !upd.transparentNow &&
                   (isl->animActive || upd.startTopToBottomAnim);
    if (runAnim && isl->animToColor == CLR_INVALID) {
        isl->animToColor = upd.color;
    }
    ApplyTopToBottomBrush(*isl, upd.color, runAnim, upd.animSpeedPct);

    isl->currentColor = upd.color;
    if (upd.forceReassert) {
        isl->strokeHidden = false;
    }
    SetStrokeHidden(*isl, true);

    COLORREF themeRef = upd.transparentNow ? upd.trayBgColor : upd.color;
    ApplyAdaptiveTheme(*isl, themeRef, upd.adaptiveTheme, upd.forceReassert);
} catch (...) {
}

static void DispatchPanelUpdateNonBlocking(HWND tb, const PanelColorUpdate& upd) {
    int maxQueue = (g_colorSamplingMode.load() == 1) ? 4 : 6;
    if (g_pendingAsyncUiUpdates.load() >= maxQueue) return;
    auto* heapUpd = new PanelColorUpdate(upd);
    g_pendingAsyncUiUpdates.fetch_add(1);
    if (!PostToWindowThreadAsync(tb, ApplyPanelColorOnTaskbarThread, heapUpd)) {
        g_pendingAsyncUiUpdates.fetch_sub(1);
        delete heapUpd;
    }
}

void WINAPI CleanupIslandsOnTaskbarThread(void*) {
    std::vector<std::shared_ptr<Island>> all;
    {
        std::lock_guard lock(g_islandsMutex);
        if (g_islands) {
            all.swap(*g_islands);
            g_islands.reset();
        }
    }
    for (auto& isl : all) {
        TeardownIsland(*isl);
    }
    all.clear();
}

// ============================================================================
// Ultra-Lightweight DIBSection GDI Sampler with Reusable Window Capture Buffer
// ============================================================================

struct FastDibSampler {
    HDC screenDc = nullptr;
    HDC memDc = nullptr;
    HBITMAP dib = nullptr;
    void* bits = nullptr;
    int w = 0;
    int h = 0;

    HDC winMemDc = nullptr;
    HBITMAP winBmp = nullptr;
    int winW = 0;
    int winH = 0;

    ~FastDibSampler() {
        Cleanup();
    }

    void CleanupWinBuf() {
        if (winBmp) {
            DeleteObject(winBmp);
            winBmp = nullptr;
        }
        if (winMemDc) {
            DeleteDC(winMemDc);
            winMemDc = nullptr;
        }
        winW = 0;
        winH = 0;
    }

    void Cleanup() {
        CleanupWinBuf();
        if (dib) {
            DeleteObject(dib);
            dib = nullptr;
            bits = nullptr;
        }
        if (memDc) {
            DeleteDC(memDc);
            memDc = nullptr;
        }
        if (screenDc) {
            ReleaseDC(nullptr, screenDc);
            screenDc = nullptr;
        }
        w = 0;
        h = 0;
    }

    bool EnsureDib(int targetW, int targetH) {
        if (targetW <= 0 || targetH <= 0) return false;
        if (!screenDc) {
            screenDc = GetDC(nullptr);
            if (!screenDc) return false;
        }
        if (!memDc) {
            memDc = CreateCompatibleDC(screenDc);
            if (!memDc) return false;
        }
        if (!dib || w != targetW || h != targetH) {
            if (dib) {
                DeleteObject(dib);
                dib = nullptr;
                bits = nullptr;
            }
            BITMAPINFO bmi{};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = targetW;
            bmi.bmiHeader.biHeight = -targetH;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            dib = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
            if (!dib || !bits) {
                Cleanup();
                return false;
            }
            SelectObject(memDc, dib);
            w = targetW;
            h = targetH;
        }
        return true;
    }

    bool EnsureWinBuf(int targetW, int targetH) {
        if (targetW <= 0 || targetH <= 0) return false;
        if (!screenDc) {
            screenDc = GetDC(nullptr);
            if (!screenDc) return false;
        }
        if (!winMemDc) {
            winMemDc = CreateCompatibleDC(screenDc);
            if (!winMemDc) return false;
        }
        if (!winBmp || winW != targetW || winH != targetH) {
            if (winBmp) {
                DeleteObject(winBmp);
                winBmp = nullptr;
            }
            winBmp = CreateCompatibleBitmap(screenDc, targetW, targetH);
            if (!winBmp) {
                return false;
            }
            SelectObject(winMemDc, winBmp);
            winW = targetW;
            winH = targetH;
        }
        return true;
    }
};

static size_t FindCaseInsensitive(std::wstring_view haystack, std::wstring_view needle) {
    if (needle.empty() || haystack.size() < needle.size()) return std::wstring_view::npos;
    for (size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < needle.size(); ++j) {
            if (std::towlower(haystack[i + j]) != std::towlower(needle[j])) {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return std::wstring_view::npos;
}

static std::wstring_view TrimWhitespace(std::wstring_view sv) {
    while (!sv.empty() && std::iswspace(sv.front())) sv.remove_prefix(1);
    while (!sv.empty() && std::iswspace(sv.back())) sv.remove_suffix(1);
    return sv;
}

// Returns true when active window is a YouTube watch/player tab rather than "YouTube" home
static bool IsYouTubePlayerForegroundWindow(HWND fg) {
    if (!fg || !IsWindow(fg)) return false;
    WCHAR title[256]{};
    if (GetWindowTextW(fg, title, ARRAYSIZE(title)) <= 0) return false;

    std::wstring_view t(title);
    size_t ytPos = FindCaseInsensitive(t, L"youtube");
    if (ytPos == std::wstring_view::npos) return false;

    size_t dashPos = t.rfind(L'-', ytPos);
    if (dashPos == std::wstring_view::npos) return false;

    std::wstring_view prefix = TrimWhitespace(t.substr(0, dashPos));
    if (prefix.empty()) return false;

    if (prefix.front() == L'(') {
        size_t closeParen = prefix.find(L')');
        if (closeParen != std::wstring_view::npos) {
            prefix = TrimWhitespace(prefix.substr(closeParen + 1));
        }
    }

    return !prefix.empty();
}

static bool IsGeminiForegroundWindow(HWND fg) {
    if (!fg || !IsWindow(fg)) return false;
    WCHAR title[256]{};
    if (GetWindowTextW(fg, title, ARRAYSIZE(title)) <= 0) return false;
    return FindCaseInsensitive(title, L"gemini") != std::wstring_view::npos;
}

static int QueryWindowsLightThemeState() {
    HKEY hKey = nullptr;
    LONG status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0,
        KEY_READ,
        &hKey);
    if (status != ERROR_SUCCESS || !hKey) return -1;

    DWORD value = 0;
    DWORD dataSize = sizeof(value);
    DWORD type = 0;
    status = RegQueryValueExW(
        hKey, L"SystemUsesLightTheme", nullptr, &type,
        reinterpret_cast<LPBYTE>(&value), &dataSize);
    if (status != ERROR_SUCCESS || type != REG_DWORD) {
        dataSize = sizeof(value);
        status = RegQueryValueExW(
            hKey, L"AppsUseLightTheme", nullptr, &type,
            reinterpret_cast<LPBYTE>(&value), &dataSize);
    }
    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS || type != REG_DWORD) return -1;
    return (value != 0) ? 1 : 0;
}

static bool IsWindowCloakedLocal(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

static bool IsWindowMinimizedOrOffscreen(HWND hwnd, const RECT& wr) {
    if (IsIconic(hwnd)) return true;
    if (wr.right <= -10000 || wr.bottom <= -10000 || wr.left >= 30000 || wr.top >= 30000) return true;
    return false;
}

static bool IsTrayTooltipOrPopupClass(LPCWSTR cls) {
    if (!cls || !*cls) return false;
    static const PCWSTR kIgnoredClasses[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"tooltips_class32", L"TaskListThumbnailWnd", L"Windows.UI.Core.CoreWindow",
        L"XamlExplorerHostIslandWindow", L"TopLevelWindowForOverflowXamlIsland",
        L"NotifyIconOverflowWindow", L"Xaml_WindowedPopupClass", L"Shell_LightDismissOverlay",
    };
    for (PCWSTR c : kIgnoredClasses) {
        if (_wcsicmp(cls, c) == 0) return true;
    }
    return false;
}

struct MonitorWindowScan {
    bool isDesktopShown = false;
    bool hasActiveNonMinimizedWindow = false;
    bool hasMaximizedWindow = false;
    HWND maximizedWindow = nullptr;
    ULONGLONG lastScanTick = 0;
};

static MonitorWindowScan ScanMonitorWindows(HMONITOR monitor, HWND taskbar) {
    MonitorWindowScan out{};
    if (!monitor) return out;

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) return out;

    RECT tbRect{};
    RECT nearTaskbar = mi.rcWork;
    if (taskbar && GetWindowRect(taskbar, &tbRect)) {
        const int monWidth = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
        const int monHeight = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);
        const bool atBottom = tbRect.top >= mi.rcMonitor.top + monHeight / 2;
        const bool atTop = tbRect.bottom <= mi.rcMonitor.top + monHeight / 2;
        const bool atLeft = tbRect.right <= mi.rcMonitor.left + monWidth / 2;
        const bool atRight = tbRect.left >= mi.rcMonitor.left + monWidth / 2;

        if (atBottom) {
            nearTaskbar.top = std::max<LONG>(mi.rcWork.top, tbRect.top - 48);
            nearTaskbar.bottom = tbRect.top;
        } else if (atTop) {
            nearTaskbar.top = tbRect.bottom;
            nearTaskbar.bottom = std::min<LONG>(mi.rcWork.bottom, tbRect.bottom + 48);
        } else if (atLeft) {
            nearTaskbar.left = tbRect.right;
            nearTaskbar.right = std::min<LONG>(mi.rcWork.right, tbRect.right + 48);
        } else if (atRight) {
            nearTaskbar.left = std::max<LONG>(mi.rcWork.left, tbRect.left - 48);
            nearTaskbar.right = tbRect.left;
        }
    }

    struct ScanCtx {
        RECT monRect;
        RECT workRect;
        RECT checkBand;
        bool hasWindowNearTaskbar = false;
        bool foundActiveOnMon = false;
        bool foundMaximizedOnMon = false;
        HWND firstMaximizedHwnd = nullptr;
    } ctx{mi.rcMonitor, mi.rcWork, nearTaskbar, false, false, false, nullptr};

    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto* c = reinterpret_cast<ScanCtx*>(lParam);
            if (!IsWindowVisible(hwnd)) return TRUE;

            LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
            if (exStyle & WS_EX_TOOLWINDOW) return TRUE;

            RECT wr{};
            if (!GetWindowRect(hwnd, &wr) || wr.right <= wr.left || wr.bottom <= wr.top) return TRUE;
            if (IsWindowMinimizedOrOffscreen(hwnd, wr)) return TRUE;

            RECT interMon{};
            if (!IntersectRect(&interMon, &wr, &c->monRect)) return TRUE;

            WCHAR cls[64]{};
            if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0 && IsTrayTooltipOrPopupClass(cls)) {
                return TRUE;
            }

            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == GetCurrentProcessId() && _wcsicmp(cls, L"CabinetWClass") != 0) {
                return TRUE;
            }

            if (IsWindowCloakedLocal(hwnd)) return TRUE;

            c->foundActiveOnMon = true;
            RECT interBand{};
            if (!c->hasWindowNearTaskbar && IntersectRect(&interBand, &wr, &c->checkBand)) {
                c->hasWindowNearTaskbar = true;
            }

            bool fillsWorkArea =
                (wr.left <= c->workRect.left + 4) &&
                (wr.top <= c->workRect.top + 4) &&
                (wr.right >= c->workRect.right - 4) &&
                (wr.bottom >= c->workRect.bottom - 4);
            bool isMax = IsZoomed(hwnd) || fillsWorkArea;
            if (isMax) {
                c->foundMaximizedOnMon = true;
                if (!c->firstMaximizedHwnd || !IsWindow(c->firstMaximizedHwnd)) {
                    c->firstMaximizedHwnd = hwnd;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    out.isDesktopShown = !ctx.hasWindowNearTaskbar;
    out.hasActiveNonMinimizedWindow = ctx.foundActiveOnMon;
    out.hasMaximizedWindow = ctx.foundMaximizedOnMon;
    out.maximizedWindow = ctx.firstMaximizedHwnd;
    return out;
}

static bool IsStartOrFlyoutActive(HWND fg) {
    if (!fg || !IsWindow(fg)) return false;

    WCHAR cls[128]{};
    if (GetClassNameW(fg, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
            wcscmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
            wcscmp(cls, L"NotifyIconOverflowWindow") == 0) {
            return true;
        }
    }
    return false;
}

static COLORREF ApplyBrightnessGuard(COLORREF c) {
    if (!g_softenExtremeBrightness.load()) return c;
    BYTE lo = static_cast<BYTE>(g_minChannel.load());
    BYTE hi = static_cast<BYTE>(g_maxChannel.load());
    return RGB(
        std::clamp<BYTE>(GetRValue(c), lo, hi),
        std::clamp<BYTE>(GetGValue(c), lo, hi),
        std::clamp<BYTE>(GetBValue(c), lo, hi));
}

static bool IsFullscreenCoveringMonitor(HMONITOR monitor, HWND fg) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!monitor || !GetMonitorInfoW(monitor, &mi)) return false;
    if (!fg || !IsWindow(fg) || fg == GetShellWindow()) return false;
    RECT fgRect{};
    if (!GetWindowRect(fg, &fgRect)) return false;
    return fgRect.left <= mi.rcMonitor.left &&
           fgRect.top <= mi.rcMonitor.top &&
           fgRect.right >= mi.rcMonitor.right &&
           fgRect.bottom >= mi.rcMonitor.bottom;
}

static bool IsSimilarColor(COLORREF a, COLORREF b, int tolerance = 1) {
    if (a == CLR_INVALID || b == CLR_INVALID) return false;
    return std::abs(static_cast<int>(GetRValue(a)) - static_cast<int>(GetRValue(b))) <= tolerance &&
           std::abs(static_cast<int>(GetGValue(a)) - static_cast<int>(GetGValue(b))) <= tolerance &&
           std::abs(static_cast<int>(GetBValue(a)) - static_cast<int>(GetBValue(b))) <= tolerance;
}

// Checks whether both left and right sides of a (width x height) DIB image are predominantly white/near-white
static bool AreLeftAndRightSidesWhite(const uint32_t* pixels, int width, int height) {
    if (!pixels || width < 8 || height <= 0) return false;

    int sideCols = std::max(2, width / 12);
    int leftWhite = 0, leftTotal = 0;
    int rightWhite = 0, rightTotal = 0;

    for (int y = 0; y < height; ++y) {
        const uint32_t* row = pixels + static_cast<size_t>(y) * static_cast<size_t>(width);

        for (int x = 0; x < sideCols; ++x) {
            uint32_t px = row[x];
            int r = static_cast<int>((px >> 16) & 0xFF);
            int g = static_cast<int>((px >> 8) & 0xFF);
            int b = static_cast<int>(px & 0xFF);
            ++leftTotal;
            if (r >= 224 && g >= 224 && b >= 224) {
                ++leftWhite;
            }
        }

        for (int x = width - sideCols; x < width; ++x) {
            uint32_t px = row[x];
            int r = static_cast<int>((px >> 16) & 0xFF);
            int g = static_cast<int>((px >> 8) & 0xFF);
            int b = static_cast<int>(px & 0xFF);
            ++rightTotal;
            if (r >= 224 && g >= 224 && b >= 224) {
                ++rightWhite;
            }
        }
    }

    if (leftTotal <= 0 || rightTotal <= 0) return false;
    return (leftWhite * 10 >= leftTotal * 6) && (rightWhite * 10 >= rightTotal * 6);
}

static COLORREF MajorityFromDibPixels(const uint32_t* pixels, int count, int width = 0, int height = 0) {
    if (!pixels || count <= 0) return RGB(32, 32, 32);

    struct BucketStats {
        int count = 0;
        uint64_t sumR = 0;
        uint64_t sumG = 0;
        uint64_t sumB = 0;
    };

    std::unordered_map<uint32_t, BucketStats> buckets;
    buckets.reserve(256);

    for (int i = 0; i < count; ++i) {
        uint32_t px = pixels[i];
        int r = static_cast<int>((px >> 16) & 0xFF);
        int g = static_cast<int>((px >> 8) & 0xFF);
        int b = static_cast<int>(px & 0xFF);
        uint32_t key = (static_cast<uint32_t>(r >> 2) << 12) |
                       (static_cast<uint32_t>(g >> 2) << 6) |
                       (static_cast<uint32_t>(b >> 2));
        auto& st = buckets[key];
        st.count += 1;
        st.sumR += static_cast<uint64_t>(r);
        st.sumG += static_cast<uint64_t>(g);
        st.sumB += static_cast<uint64_t>(b);
    }

    const BucketStats* best = nullptr;
    for (const auto& kv : buckets) {
        if (!best || kv.second.count > best->count) {
            best = &kv.second;
        }
    }

    if (!best || best->count <= 0) return RGB(32, 32, 32);

    COLORREF maj = RGB(
        std::clamp(static_cast<int>(best->sumR / static_cast<uint64_t>(best->count)), 0, 255),
        std::clamp(static_cast<int>(best->sumG / static_cast<uint64_t>(best->count)), 0, 255),
        std::clamp(static_cast<int>(best->sumB / static_cast<uint64_t>(best->count)), 0, 255));

    // If left and right side margins are white/near-white and the calculated majority isn't already white,
    // use white instead of letting a centered non-white card/banner hijack the majority color.
    if (width > 0 && height > 0 && AreLeftAndRightSidesWhite(pixels, width, height)) {
        bool majIsWhite = (GetRValue(maj) >= 228 && GetGValue(maj) >= 228 && GetBValue(maj) >= 228);
        if (!majIsWhite) {
            return RGB(255, 255, 255);
        }
    }

    return maj;
}

static COLORREF SampleMajorityOnMonitor(FastDibSampler& dib, HWND taskbar, HWND maxHwnd, bool allowPrintWindow) {
    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    if (!monitor) return RGB(32, 32, 32);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) return RGB(32, 32, 32);

    const RECT& r = monitorInfo.rcWork;
    const int w = static_cast<int>(r.right - r.left);
    const int h = static_cast<int>(r.bottom - r.top);
    if (w <= 0 || h <= 0) return RGB(32, 32, 32);

    constexpr int targetW = 48;
    constexpr int targetH = 27;

    // Avoid PrintWindow DWM stalls during active app switching; use it when a background maximized app is under a floating window
    if (allowPrintWindow && maxHwnd && IsWindow(maxHwnd) && !IsIconic(maxHwnd)) {
        RECT winRect{};
        if (GetWindowRect(maxHwnd, &winRect)) {
            int winW = static_cast<int>(winRect.right - winRect.left);
            int winH = static_cast<int>(winRect.bottom - winRect.top);
            if (winW > 0 && winH > 0 && dib.EnsureDib(targetW, targetH) && dib.EnsureWinBuf(winW, winH)) {
                BOOL printed = PrintWindow(maxHwnd, dib.winMemDc, PW_RENDERFULLCONTENT);
                if (!printed) {
                    printed = PrintWindow(maxHwnd, dib.winMemDc, 0);
                }
                if (printed) {
                    int srcX = std::clamp<int>(static_cast<int>(r.left - winRect.left), 0, std::max(0, winW - 1));
                    int srcY = std::clamp<int>(static_cast<int>(r.top - winRect.top), 0, std::max(0, winH - 1));
                    int srcW = std::min(w, winW - srcX);
                    int srcH = std::min(h, winH - srcY);
                    if (srcW > 0 && srcH > 0) {
                        SetStretchBltMode(dib.memDc, COLORONCOLOR);
                        if (StretchBlt(dib.memDc, 0, 0, targetW, targetH, dib.winMemDc, srcX, srcY, srcW, srcH, SRCCOPY)) {
                            const auto* pixels = static_cast<const uint32_t*>(dib.bits);
                            return ApplyBrightnessGuard(MajorityFromDibPixels(pixels, targetW * targetH, targetW, targetH));
                        }
                    }
                }
            }
        }
    }

    if (!dib.EnsureDib(targetW, targetH)) return RGB(32, 32, 32);

    SetStretchBltMode(dib.memDc, COLORONCOLOR);
    if (!StretchBlt(dib.memDc, 0, 0, targetW, targetH, dib.screenDc, r.left, r.top, w, h, SRCCOPY)) {
        return RGB(32, 32, 32);
    }

    const auto* pixels = static_cast<const uint32_t*>(dib.bits);
    return ApplyBrightnessGuard(MajorityFromDibPixels(pixels, targetW * targetH, targetW, targetH));
}

static COLORREF SampleSystemTrayBackground(FastDibSampler& dib, HWND taskbar) {
    RECT taskbarRect{};
    if (!GetWindowRect(taskbar, &taskbarRect)) return RGB(32, 32, 32);

    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    if (!monitor) return RGB(32, 32, 32);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) return RGB(32, 32, 32);

    const RECT& monitorRect = monitorInfo.rcMonitor;
    const int monWidth = static_cast<int>(monitorRect.right - monitorRect.left);
    const int monHeight = static_cast<int>(monitorRect.bottom - monitorRect.top);
    if (monWidth <= 0 || monHeight <= 0) return RGB(32, 32, 32);

    const bool horizontal = (taskbarRect.right - taskbarRect.left) >= (taskbarRect.bottom - taskbarRect.top);
    constexpr int kTapCount = 12;
    if (!dib.EnsureDib(kTapCount, 1)) return RGB(32, 32, 32);

    int r[kTapCount]{}, g[kTapCount]{}, b[kTapCount]{};
    int valid = 0;

    if (horizontal) {
        int trayW = std::max(80, monWidth / 5);
        int stripX = static_cast<int>(std::clamp<LONG>(
            static_cast<LONG>(taskbarRect.right) - trayW - 16,
            monitorRect.left,
            static_cast<LONG>(monitorRect.right - 1)));
        const bool atBottom = taskbarRect.top >= monitorRect.top + monHeight / 2;
        int sampleY = static_cast<int>(std::clamp<LONG>(
            atBottom ? (static_cast<int>(taskbarRect.top) - 4)
                     : (static_cast<int>(taskbarRect.bottom) + 3),
            monitorRect.top,
            static_cast<LONG>(monitorRect.bottom - 1)));

        if (BitBlt(dib.memDc, 0, 0, kTapCount, 1, dib.screenDc, stripX, sampleY, SRCCOPY)) {
            const auto* px = static_cast<const uint32_t*>(dib.bits);
            for (int i = 0; i < kTapCount; ++i) {
                uint32_t c = px[i];
                r[valid] = (c >> 16) & 0xFF;
                g[valid] = (c >> 8) & 0xFF;
                b[valid] = c & 0xFF;
                ++valid;
            }
        }
    } else {
        int trayH = std::max(80, monHeight / 5);
        int stripY = static_cast<int>(std::clamp<LONG>(
            static_cast<LONG>(taskbarRect.bottom) - trayH - 16,
            monitorRect.top,
            static_cast<LONG>(monitorRect.bottom - 1)));
        const bool atLeft = taskbarRect.right <= monitorRect.left + monWidth / 2;
        int sampleX = static_cast<int>(std::clamp<LONG>(
            atLeft ? (static_cast<int>(taskbarRect.right) + 3)
                   : (static_cast<int>(taskbarRect.left) - 4),
            monitorRect.left,
            static_cast<LONG>(monitorRect.right - 1)));

        if (BitBlt(dib.memDc, 0, 0, 1, kTapCount, dib.screenDc, sampleX, stripY, SRCCOPY)) {
            const auto* px = static_cast<const uint32_t*>(dib.bits);
            for (int i = 0; i < kTapCount; ++i) {
                uint32_t c = px[i];
                r[valid] = (c >> 16) & 0xFF;
                g[valid] = (c >> 8) & 0xFF;
                b[valid] = c & 0xFF;
                ++valid;
            }
        }
    }

    if (valid == 0) return RGB(32, 32, 32);
    std::nth_element(r, r + valid / 2, r + valid);
    std::nth_element(g, g + valid / 2, g + valid);
    std::nth_element(b, b + valid / 2, b + valid);
    return RGB(r[valid / 2], g[valid / 2], b[valid / 2]);
}

struct EdgeStripSampleResult {
    COLORREF color = RGB(32, 32, 32);
    float dominance = 0.0f;
};

// Single-blt strip sampler picking the most frequent ("maximum same") color and its dominance ratio
static EdgeStripSampleResult SampleTaskbarEdgeFastWithDominance(FastDibSampler& dib, HWND taskbar) {
    EdgeStripSampleResult out{};
    RECT taskbarRect{};
    if (!GetWindowRect(taskbar, &taskbarRect)) return out;

    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    if (!monitor) return out;

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) return out;

    const RECT& monitorRect = monitorInfo.rcMonitor;
    const int monWidth = static_cast<int>(monitorRect.right - monitorRect.left);
    const int monHeight = static_cast<int>(monitorRect.bottom - monitorRect.top);
    if (monWidth <= 0 || monHeight <= 0) return out;

    const bool atBottom = taskbarRect.top >= monitorRect.top + monHeight / 2;
    const bool taskbarAtTop = taskbarRect.bottom <= monitorRect.top + monHeight / 2;
    const bool taskbarAtLeft = taskbarRect.right <= monitorRect.left + monWidth / 2;
    const bool taskbarAtRight = taskbarRect.left >= monitorRect.left + monWidth / 2;

    struct ModeEntry {
        int count = 0;
        uint64_t sumR = 0;
        uint64_t sumG = 0;
        uint64_t sumB = 0;
    };

    std::unordered_map<uint32_t, ModeEntry> counts;
    counts.reserve(64);

    auto recordPixel = [&](uint32_t argb) {
        int r = static_cast<int>((argb >> 16) & 0xFF);
        int g = static_cast<int>((argb >> 8) & 0xFF);
        int b = static_cast<int>(argb & 0xFF);

        uint32_t key = (static_cast<uint32_t>(r >> 3) << 10) |
                       (static_cast<uint32_t>(g >> 3) << 5) |
                       (static_cast<uint32_t>(b >> 3));

        auto& entry = counts[key];
        entry.count += 1;
        entry.sumR += static_cast<uint64_t>(r);
        entry.sumG += static_cast<uint64_t>(g);
        entry.sumB += static_cast<uint64_t>(b);
    };

    constexpr int kTapCount = 64;
    int totalSamples = 0;

    if (atBottom || taskbarAtTop) {
        int sampleY = static_cast<int>(std::clamp<LONG>(
            atBottom ? (static_cast<int>(taskbarRect.top) - 4)
                     : (static_cast<int>(taskbarRect.bottom) + 3),
            monitorRect.top,
            static_cast<LONG>(monitorRect.bottom - 1)));
        int stripX = static_cast<int>(monitorRect.left) + monWidth / 8;
        int stripW = std::max(1, (monWidth * 3) / 4);

        if (dib.EnsureDib(stripW, 1) &&
            BitBlt(dib.memDc, 0, 0, stripW, 1, dib.screenDc, stripX, sampleY, SRCCOPY)) {
            const auto* px = static_cast<const uint32_t*>(dib.bits);
            for (int i = 0; i < kTapCount; ++i) {
                int idx = std::min(stripW - 1, (stripW * i) / kTapCount);
                recordPixel(px[idx]);
                ++totalSamples;
            }
        }
    } else if (taskbarAtLeft || taskbarAtRight) {
        int sampleX = static_cast<int>(std::clamp<LONG>(
            taskbarAtLeft ? (static_cast<int>(taskbarRect.right) + 3)
                          : (static_cast<int>(taskbarRect.left) - 4),
            monitorRect.left,
            static_cast<LONG>(monitorRect.right - 1)));
        int stripY = static_cast<int>(monitorRect.top) + monHeight / 8;
        int stripH = std::max(1, (monHeight * 3) / 4);

        if (dib.EnsureDib(1, stripH) &&
            BitBlt(dib.memDc, 0, 0, 1, stripH, dib.screenDc, sampleX, stripY, SRCCOPY)) {
            const auto* px = static_cast<const uint32_t*>(dib.bits);
            for (int i = 0; i < kTapCount; ++i) {
                int idx = std::min(stripH - 1, (stripH * i) / kTapCount);
                recordPixel(px[idx]);
                ++totalSamples;
            }
        }
    }

    if (counts.empty() || totalSamples <= 0) return out;

    const ModeEntry* best = nullptr;
    for (const auto& kv : counts) {
        if (!best || kv.second.count > best->count) {
            best = &kv.second;
        }
    }

    if (best && best->count > 0) {
        COLORREF avgCol = RGB(
            std::clamp(static_cast<int>(best->sumR / static_cast<uint64_t>(best->count)), 0, 255),
            std::clamp(static_cast<int>(best->sumG / static_cast<uint64_t>(best->count)), 0, 255),
            std::clamp(static_cast<int>(best->sumB / static_cast<uint64_t>(best->count)), 0, 255));
        out.color = ApplyBrightnessGuard(avgCol);
        out.dominance = static_cast<float>(best->count) / static_cast<float>(totalSamples);
    }
    return out;
}

static COLORREF SampleTaskbarEdgeFast(FastDibSampler& dib, HWND taskbar) {
    return SampleTaskbarEdgeFastWithDominance(dib, taskbar).color;
}

struct ColorStabilizerState {
    ULONGLONG lastCommitTick = 0;
    COLORREF pendingColor = CLR_INVALID;
};

struct CachedMajorityState {
    COLORREF color = CLR_INVALID;
    ULONGLONG lastSampleTick = 0;
};

struct CachedSideWhiteState {
    bool sidesWhite = false;
    ULONGLONG lastTick = 0;
};

static DWORD WINAPI WorkerThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    FastDibSampler dib;
    std::unordered_map<HWND, COLORREF> lastColors;
    std::unordered_map<HWND, COLORREF> lastTrayColors;
    std::unordered_map<HWND, bool> lastDesktopStates;
    std::unordered_map<HWND, MonitorWindowScan> scanCache;
    std::unordered_map<HWND, CachedMajorityState> majorityCache;
    std::unordered_map<HWND, CachedSideWhiteState> sideWhiteCache;
    std::unordered_map<HWND, bool> mixedEdgeActive;
    std::unordered_map<HWND, ColorStabilizerState> stabilizers;
    int attachCheckCounter = 0;
    int themeCheckCounter = 0;
    bool wasFlyoutActive = false;
    ULONGLONG holdUntilTick = 0;
    int lastSystemTheme = QueryWindowsLightThemeState();

    while (!g_stop.load() && !g_unloading.load()) {
        bool forcedThisTick = g_forceRefresh.exchange(false);
        ULONGLONG now = GetTickCount64();

        HWND fg = GetForegroundWindow();
        if (fg != g_lastObservedFg.load()) {
            g_lastObservedFg.store(fg);
            g_lastFgChangeTick.store(now);
        }

        if (forcedThisTick) {
            lastColors.clear();
            lastTrayColors.clear();
            lastDesktopStates.clear();
            scanCache.clear();
            majorityCache.clear();
            sideWhiteCache.clear();
            mixedEdgeActive.clear();
            stabilizers.clear();
            wasFlyoutActive = false;
            holdUntilTick = 0;
            themeCheckCounter = 0;
        }

        bool hideFillForCorners = (g_panelCornerRadius.load() > 0);
        int samplingMode = g_colorSamplingMode.load();
        bool isEdgeStrip = (samplingMode == 0);
        bool isMixedMode = (samplingMode == 2);
        bool onYouTubePlayer = !isEdgeStrip && IsYouTubePlayerForegroundWindow(fg);
        bool onGemini = !isEdgeStrip && IsGeminiForegroundWindow(fg);
        bool forceEdgeForApp = onYouTubePlayer || onGemini;
        bool wantTransparentOnDesktop = g_transparentOnDesktop.load();
        bool topToBottomAnim = g_topToBottomAnimation.load();
        int animSpeedPct = g_animationSpeedPercent.load();
        bool adaptiveTheme = g_adaptiveIconTheme.load();
        bool pauseFullscreen = g_pauseOnFullscreen.load();

        if ((++themeCheckCounter % 45) == 1) {
            int currentSystemTheme = QueryWindowsLightThemeState();
            if (currentSystemTheme != -1) {
                if (lastSystemTheme == -1) {
                    lastSystemTheme = currentSystemTheme;
                } else if (currentSystemTheme != lastSystemTheme &&
                           g_restartExplorerOnThemeChange.load()) {
                    dib.Cleanup();
                    SafeRestartExplorer(L"Windows Light/Dark theme change");
                    return 0;
                } else {
                    lastSystemTheme = currentSystemTheme;
                }
            }
        }

        bool flyoutActive = IsStartOrFlyoutActive(fg);
        if (flyoutActive) {
            wasFlyoutActive = true;
            holdUntilTick = now + FLYOUT_CLOSE_GRACE_MS;
        } else if (wasFlyoutActive) {
            wasFlyoutActive = false;
            holdUntilTick = now + FLYOUT_CLOSE_GRACE_MS;
        }

        bool inHoldWindow = flyoutActive || (now < holdUntilTick);

        std::vector<HWND> taskbars;
        EnumWindows(
            [](HWND hWnd, LPARAM lp) -> BOOL {
                DWORD pid = 0;
                if (GetWindowThreadProcessId(hWnd, &pid) &&
                    pid == GetCurrentProcessId() &&
                    IsTaskbarWindowClass(hWnd)) {
                    reinterpret_cast<std::vector<HWND>*>(lp)->push_back(hWnd);
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&taskbars));

        bool needsAttach = false;
        for (HWND tb : taskbars) {
            auto isl = FindIslandByHwnd(tb);
            if (!isl || !IsPanelParented(*isl)) {
                needsAttach = true;
                break;
            }
        }

        bool fgZoomed = (fg && IsWindow(fg) && IsZoomed(fg));
        bool anyEdgeLogic = isEdgeStrip || forceEdgeForApp;
        bool settlingAppSwitch = !forcedThisTick && !anyEdgeLogic && !fgZoomed &&
                                 (now - g_lastFgChangeTick.load() < kAppSwitchSettleMs);

        bool inEarlyStartup = (now - g_modInitTick.load() < kEarlyStartupWindowMs);
        int attachMod = needsAttach ? (inEarlyStartup ? 3 : 10) : (inEarlyStartup ? 12 : 240);
        if (!settlingAppSwitch && !taskbars.empty() &&
            ((++attachCheckCounter % attachMod) == 1)) {
            RunFromWindowThread(taskbars.front(), AttachAllTaskbars, nullptr);
        }

        for (HWND tb : taskbars) {
            if (!IsWindow(tb) || g_unloading.load()) continue;

            HMONITOR mon = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
            if (pauseFullscreen && IsFullscreenCoveringMonitor(mon, fg)) {
                continue;
            }

            auto& cached = scanCache[tb];
            if (cached.lastScanTick == 0 || (now - cached.lastScanTick >= kWindowScanCacheMs)) {
                MonitorWindowScan fresh = ScanMonitorWindows(mon, tb);
                fresh.lastScanTick = now;
                cached = fresh;
            }

            bool fgOnThisMon = (fg && IsWindow(fg) &&
                                MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST) == mon);
            bool hasMaximized = cached.hasMaximizedWindow || (fgOnThisMon && fgZoomed);
            HWND maxHwnd = cached.maximizedWindow;
            if ((!maxHwnd || !IsWindow(maxHwnd)) && fgOnThisMon && fgZoomed) {
                maxHwnd = fg;
            }

            bool sidesAreWhite = false;
            if (!isEdgeStrip && isMixedMode && !(fgOnThisMon && forceEdgeForApp)) {
                auto& swCache = sideWhiteCache[tb];
                if (forcedThisTick || swCache.lastTick == 0 || (now - swCache.lastTick >= kSideCheckCacheMs)) {
                    MONITORINFO miSide{};
                    miSide.cbSize = sizeof(miSide);
                    if (mon && GetMonitorInfoW(mon, &miSide)) {
                        const RECT& rw = miSide.rcWork;
                        int ww = static_cast<int>(rw.right - rw.left);
                        int wh = static_cast<int>(rw.bottom - rw.top);
                        if (ww > 0 && wh > 0 && dib.EnsureDib(48, 27)) {
                            SetStretchBltMode(dib.memDc, COLORONCOLOR);
                            if (StretchBlt(dib.memDc, 0, 0, 48, 27, dib.screenDc, rw.left, rw.top, ww, wh, SRCCOPY)) {
                                swCache.sidesWhite = AreLeftAndRightSidesWhite(static_cast<const uint32_t*>(dib.bits), 48, 27);
                            }
                        }
                    }
                    swCache.lastTick = now;
                }
                sidesAreWhite = swCache.sidesWhite;
            }

            bool useEdgeLogic = isEdgeStrip || (fgOnThisMon && forceEdgeForApp);
            if (!useEdgeLogic && isMixedMode) {
                bool& edgeActive = mixedEdgeActive[tb];
                if (sidesAreWhite) {
                    edgeActive = false;
                } else {
                    EdgeStripSampleResult edgeSample = SampleTaskbarEdgeFastWithDominance(dib, tb);
                    if (!edgeActive && edgeSample.dominance >= 0.65f) {
                        edgeActive = true;
                    } else if (edgeActive && edgeSample.dominance < 0.45f) {
                        edgeActive = false;
                    }
                }
                if (edgeActive) {
                    useEdgeLogic = true;
                }
            }

            bool onDesktop = wantTransparentOnDesktop && (!hasMaximized || cached.isDesktopShown);
            bool hasActiveWindow = cached.hasActiveNonMinimizedWindow;

            bool desktopStateChanged = false;
            if (auto it = lastDesktopStates.find(tb); it == lastDesktopStates.end() || it->second != onDesktop) {
                lastDesktopStates[tb] = onDesktop;
                desktopStateChanged = true;
            }

            COLORREF prev = CLR_INVALID;
            if (auto it = lastColors.find(tb); it != lastColors.end()) {
                prev = it->second;
            }

            COLORREF prevTray = CLR_INVALID;
            if (auto it = lastTrayColors.find(tb); it != lastTrayColors.end()) {
                prevTray = it->second;
            }

            COLORREF trayBg = prevTray;
            bool trayChanged = false;
            if (onDesktop && !settlingAppSwitch) {
                COLORREF sampledTray = SampleSystemTrayBackground(dib, tb);
                trayChanged = !IsSimilarColor(sampledTray, prevTray, 2);
                if (trayChanged || prevTray == CLR_INVALID) {
                    prevTray = sampledTray;
                    lastTrayColors[tb] = prevTray;
                }
                trayBg = prevTray;
            }

            auto isl = FindIslandByHwnd(tb);
            bool islTransparencyOutOfSync = (isl && isl->lastAppliedTransparent != onDesktop);

            if (inHoldWindow && prev != CLR_INVALID) {
                if (forcedThisTick || desktopStateChanged || trayChanged || islTransparencyOutOfSync) {
                    PanelColorUpdate upd{
                        tb,
                        prev,
                        trayBg != CLR_INVALID ? trayBg : prev,
                        adaptiveTheme,
                        true,
                        onDesktop,
                        hideFillForCorners,
                        false,
                        topToBottomAnim,
                        animSpeedPct};
                    DispatchPanelUpdateNonBlocking(tb, upd);
                }
                continue;
            }

            if (!hasMaximized || (!useEdgeLogic && !hasActiveWindow)) {
                COLORREF holdColor = (prev != CLR_INVALID) ? prev : RGB(32, 32, 32);
                if (forcedThisTick || desktopStateChanged || trayChanged || islTransparencyOutOfSync || prev == CLR_INVALID) {
                    if (prev == CLR_INVALID) {
                        lastColors[tb] = holdColor;
                    }
                    PanelColorUpdate upd{
                        tb,
                        holdColor,
                        trayBg != CLR_INVALID ? trayBg : holdColor,
                        adaptiveTheme,
                        trayChanged || desktopStateChanged || islTransparencyOutOfSync || forcedThisTick,
                        onDesktop,
                        hideFillForCorners,
                        false,
                        topToBottomAnim,
                        animSpeedPct};
                    DispatchPanelUpdateNonBlocking(tb, upd);
                }
                continue;
            }

            if (settlingAppSwitch && prev != CLR_INVALID && !forcedThisTick && !islTransparencyOutOfSync) {
                if (topToBottomAnim && isl && isl->animActive) {
                    PanelColorUpdate upd{
                        tb,
                        prev,
                        trayBg != CLR_INVALID ? trayBg : prev,
                        adaptiveTheme,
                        false,
                        onDesktop,
                        hideFillForCorners,
                        false,
                        topToBottomAnim,
                        animSpeedPct};
                    DispatchPanelUpdateNonBlocking(tb, upd);
                }
                continue;
            }

            bool justSwitchedApp = (now - g_lastFgChangeTick.load() < kAppSwitchSettleMs);
            bool allowPrintWindow = !justSwitchedApp && (maxHwnd && maxHwnd != fg);

            COLORREF targetColor = RGB(32, 32, 32);
            if (useEdgeLogic) {
                targetColor = SampleTaskbarEdgeFast(dib, tb);
            } else if (isMixedMode) {
                auto& mCache = majorityCache[tb];
                if (forcedThisTick || mCache.color == CLR_INVALID ||
                    (now - mCache.lastSampleTick >= kMixedMajorityCacheMs)) {
                    mCache.color = SampleMajorityOnMonitor(dib, tb, maxHwnd, allowPrintWindow);
                    mCache.lastSampleTick = now;
                }
                targetColor = mCache.color;
            } else {
                targetColor = SampleMajorityOnMonitor(dib, tb, maxHwnd, allowPrintWindow);
            }

            auto& stab = stabilizers[tb];
            bool colorChanged = false;
            int simTol = useEdgeLogic ? 1 : 2;
            bool recentFgSwitch = (now - g_lastFgChangeTick.load() < 120);
            ULONGLONG minCooldown = useEdgeLogic ? (recentFgSwitch ? 0 : kEdgeDebounceMs) : kCommitCooldownMs;

            if (prev == CLR_INVALID) {
                prev = targetColor;
                lastColors[tb] = prev;
                stab.lastCommitTick = now;
                stab.pendingColor = CLR_INVALID;
                colorChanged = true;
            } else if (!IsSimilarColor(targetColor, prev, simTol)) {
                bool confirmed = !useEdgeLogic || recentFgSwitch || forcedThisTick ||
                                 IsSimilarColor(targetColor, stab.pendingColor, simTol);
                if (!confirmed) {
                    stab.pendingColor = targetColor;
                } else if (now - stab.lastCommitTick >= minCooldown) {
                    prev = targetColor;
                    lastColors[tb] = prev;
                    stab.lastCommitTick = now;
                    stab.pendingColor = CLR_INVALID;
                    colorChanged = true;
                }
            } else {
                stab.pendingColor = CLR_INVALID;
            }

            COLORREF toApply = (prev != CLR_INVALID) ? prev : targetColor;
            bool colorOutOfSync = (isl && isl->currentColor != CLR_INVALID &&
                                   toApply != CLR_INVALID && isl->currentColor != toApply);
            bool startAnim = colorChanged && topToBottomAnim;
            bool animStillRunning = topToBottomAnim && isl && isl->animActive;
            bool needUiUpdate = forcedThisTick || colorChanged || colorOutOfSync ||
                                islTransparencyOutOfSync || trayChanged ||
                                desktopStateChanged || animStillRunning || !isl;

            if (needUiUpdate && toApply != CLR_INVALID) {
                PanelColorUpdate upd{
                    tb,
                    toApply,
                    trayBg != CLR_INVALID ? trayBg : toApply,
                    adaptiveTheme,
                    colorChanged || colorOutOfSync || islTransparencyOutOfSync ||
                        trayChanged || desktopStateChanged || forcedThisTick,
                    onDesktop,
                    hideFillForCorners,
                    startAnim,
                    topToBottomAnim,
                    animSpeedPct};
                DispatchPanelUpdateNonBlocking(tb, upd);
            }
        }

        DWORD baseIdleMs = anyEdgeLogic ? EDGE_POLL_INTERVAL_MS
                       : isMixedMode  ? MIXED_POLL_INTERVAL_MS
                                      : IDLE_POLL_INTERVAL_MS;
        DWORD waitMs = g_animRunningGlobal.load() ? ANIM_POLL_INTERVAL_MS : baseIdleMs;
        DWORD startWait = GetTickCount();
        while (!g_stop.load() && !g_unloading.load()) {
            DWORD elapsed = GetTickCount() - startWait;
            if (elapsed >= waitMs) break;
            DWORD rem = waitMs - elapsed;
            DWORD res = WaitForSingleObject(g_wakeEvent, rem);
            if (res == WAIT_OBJECT_0 || res == WAIT_TIMEOUT) {
                break;
            }
        }
    }

    dib.Cleanup();
    return 0;
}

// ============================================================================
// Windhawk Lifecycle
// ============================================================================

void Wh_ModSettingsChanged() {
    bool prevTransparentOnDesktop = g_transparentOnDesktop.load();
    LoadSettings();
    if (g_transparentOnDesktop.load() != prevTransparentOnDesktop) {
        SafeRestartExplorer(L"transparentOnDesktop setting changed");
    }
}

BOOL Wh_ModInit() {
    g_modInitTick.store(GetTickCount64());
    LoadSettings();
    g_symbolsHooked.store(HookTaskbarDllSymbols());

    bool needDelayLoadHook = false;
    if (HMODULE tvMod = GetTaskbarViewModuleHandle()) {
        g_taskbarViewHooked.store(HookTaskbarViewMeasureOverride(tvMod));
    } else {
        needDelayLoadHook = true;
    }

    if (HMODULE stMod = GetSystemTrayModuleHandle()) {
        g_systemTrayHooked.store(HookSystemTrayMeasureOverride(stMod));
    } else {
        needDelayLoadHook = true;
    }

    if (needDelayLoadHook) {
        HMODULE kbMod = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLib = reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kbMod, "LoadLibraryExW"));
        if (pLoadLib) {
            WindhawkUtils::SetFunctionHook(
                pLoadLib, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
        }
    }

    g_unloading.store(false);
    g_stop.store(false);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        if (g_wakeEvent) {
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
        Wh_Log(L"Taskbar Edge Color Match: failed to create worker thread");
        return FALSE;
    }

    Wh_Log(L"Taskbar Edge Color Match v2.5.69 initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewHooked.load()) {
        if (HMODULE tvMod = GetTaskbarViewModuleHandle()) {
            if (HookTaskbarViewMeasureOverride(tvMod)) {
                g_taskbarViewHooked.store(true);
                Wh_ApplyHookOperations();
            }
        }
    }
    if (!g_systemTrayHooked.load()) {
        if (HMODULE stMod = GetSystemTrayModuleHandle()) {
            if (HookSystemTrayMeasureOverride(stMod)) {
                g_systemTrayHooked.store(true);
                Wh_ApplyHookOperations();
            }
        }
    }

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, EnsureTaskbarAsyncDispatcherOnUiThread, nullptr);
        RunFromWindowThread(hTaskbarWnd, AttachAllTaskbars, nullptr);
    }
    SignalWorkerWakeup();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
    SignalWorkerWakeup();
}

void Wh_ModUninit() {
    g_stop.store(true);
    SignalWorkerWakeup();

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, CleanupIslandsOnTaskbarThread, nullptr);
        RunFromWindowThread(hTaskbarWnd, CleanupTaskbarAsyncDispatcherOnUiThread, nullptr);
    }

    {
        std::lock_guard lock(g_asyncDispatchMutex);
        for (auto& item : g_inflightAsync) {
            if (item && item->ownsProcParam) {
                delete static_cast<PanelColorUpdate*>(item->procParam);
                item->procParam = nullptr;
            }
        }
        g_inflightAsync.clear();
    }

    {
        std::lock_guard lock(g_cachedXamlRootsMutex);
        g_cachedXamlRoots.clear();
    }

    Wh_Log(L"Taskbar Edge Color Match uninitialized");
    if (!g_restartingExplorer.load()) {
        SafeRestartExplorer(L"mod unapplied");
    }
}
