// ==WindhawkMod==
// @id              taskbar-horizontal-scroll-media
// @name            Taskbar Horizontal Scroll Media Control
// @description     Change tracks with taskbar wheel tilt and show the genuine Windows media panel control independently.
// @version         1.0.0
// @author          Shreyas J S
// @github          https://github.com/shreyasjswork
// @license         GPL-3.0
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lwindowsapp -lruntimeobject -ldwmapi -lshcore -lshell32 -luser32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Horizontal Scroll Media Control

![Taskbar horizontal scroll media control demo](https://raw.githubusercontent.com/shreyasjswork/taskbar-horizontal-scroll-media/main/assets/Windhawk.gif)

Tilt the mouse wheel horizontally while the pointer is over the taskbar:

- Right: next track
- Left: previous track
- Ctrl + middle-click on empty taskbar space: play/pause

Ctrl is required by default for track-changing wheel tilt. The requirement can
be disabled, or the wheel modifier can be changed to Shift, Alt, or the Windows
key. Play/pause intentionally remains fixed to Ctrl + middle-click.

A held wheel tilt triggers exactly once. Repeated horizontal-wheel packets are
discarded until the configured release gap has passed.

The mod uses Windows' genuine current Quick Settings
`ControlCenter.MediaTransportControls`; it does not draw a replacement card and does not
open a Chromium bubble.

- When Quick Settings is already open, its media card updates in place.
- When Quick Settings is closed, the same Windows media control is hosted alone
  in a non-activating popup beside the taskbar.
- Opening Quick Settings dismisses the independent control.

The implementation is split between Explorer, which owns the taskbar gesture,
and ShellHost, the Windows 11 process that owns Quick Settings on current
builds (ShellExperienceHost on older Windows 11 builds). A visual-tree watcher
captures Windows' system-created control; while
the panel is closed, that same element is temporarily reparented into a desktop
XAML island and then restored to its original parent and child index.

Explorer owns the taskbar input surface and remains available when the Quick
Settings host restarts. Only the Explorer process that owns a taskbar window
acts on a gesture; other Explorer processes remain inert. The input path hooks
the taskbar's own `Windows.UI.Input.InputSite.WindowClass` procedure for
`WM_POINTERHWHEEL` and pointer-down messages; it does not install a system-wide
mouse hook. Keeping both roles in one catalog mod also avoids a separate tool
process and another IPC boundary while supporting pre-24H2 ShellExperienceHost.

On the first use after the owning shell process starts, Windows must create the
Quick Settings visual tree before its genuine media control exists. The mod
asks Windows to create that tree at most once, only after the owning host has
announced itself. A brief Quick Settings flash can occur on that first use, and
opening the system surface can dismiss another open shell flyout.

Requires Windows 11 build 22000 or newer. The native card relies on private
Windows XAML structure, so a future Windows update can require a mod update.

## Compatibility

Windows XAML diagnostics supports one active visual-tree consumer in the Quick
Settings host. Do not run this mod together with Windows 11 Notification Center
Styler: whichever diagnostics TAP attaches first prevents the other standalone
card capture from initializing. Track controls continue to work, but disable
one of the two mods and restart the affected shell host to restore card capture.

## Difference from Taskbar Scroll Actions

Taskbar Scroll Actions provides general vertical taskbar-wheel mappings. This
mod is specifically for horizontal wheel tilt, one-action-per-held-tilt
suppression, Ctrl + middle-click play/pause, and standalone presentation of
Windows' genuine Quick Settings media control.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- requireModifier: true
  $name: Require a modifier key
  $description: Turn off to change tracks with horizontal scrolling without a modifier. Ctrl + middle-click play/pause is unchanged.
- modifier: ctrl
  $name: Modifier key
  $description: Used for track-changing wheel tilt only when "Require a modifier key" is enabled. Play/pause remains Ctrl + middle-click.
  $options:
  - ctrl: Ctrl
  - shift: Shift
  - alt: Alt
  - win: Windows key
- reverseDirection: false
  $name: Reverse left and right
  $description: Enable this if the mouse reports tilt direction in reverse.
- releaseTimeoutMs: 350
  $name: Gesture release gap (milliseconds)
  $description: 'Repeated input is ignored until no packet arrives for this long. Accepted range: 80-2000.'
- nativeFlyoutDelayMs: 600
  $name: Native control delay (milliseconds)
  $description: 'Allows Windows to publish the new track before showing its control. Accepted range: 250-2500.'
- nativeFlyoutTimeoutMs: 3000
  $name: Native control timeout (milliseconds)
  $description: 'How long the independent Windows media control remains visible. Accepted range: 1500-15000.'
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <roapi.h>
#include <shellscalingapi.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <xamlom.h>
#include <ocidl.h>

// winbase.h defines a legacy GetCurrentTime macro that collides with the
// Windows.UI.Xaml media-animation ABI.
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <mutex>
#include <string>

namespace {

constexpr wchar_t kShowEventObjectName[] =
    L"Local\\Windhawk.TaskbarHorizontalScrollMedia.ShowNativeControl.v4";
constexpr wchar_t kCaptureReadyEventObjectName[] =
    L"Local\\Windhawk.TaskbarHorizontalScrollMedia.CaptureReady.v1";
constexpr wchar_t kHostAliveEventObjectName[] =
    L"Local\\Windhawk.TaskbarHorizontalScrollMedia.HostAlive.v1";
constexpr wchar_t kBootstrapAvailableEventObjectName[] =
    L"Local\\Windhawk.TaskbarHorizontalScrollMedia.BootstrapAvailable.v1";
constexpr wchar_t kCapturedHostClass[] =
    L"Windhawk.TaskbarHorizontalScrollMedia.CapturedNativeHost";
constexpr wchar_t kPrivateMediaControlClass[] =
    L"ControlCenter.MediaTransportControls";
constexpr UINT_PTR kDismissTimer = 1;
constexpr UINT kRestoreCapturedMediaMessage = WM_APP + 2;
constexpr UINT kCleanupMediaCaptureMessage = WM_APP + 3;

enum class ProcessRole { Explorer, ShellHost, Unsupported };
enum class ModifierKey { Ctrl, Shift, Alt, Win };

struct Settings {
    std::atomic<bool> requireModifier{true};
    std::atomic<ModifierKey> modifier{ModifierKey::Ctrl};
    std::atomic<bool> reverseDirection{false};
    std::atomic<DWORD> releaseTimeoutMs{350};
    std::atomic<DWORD> flyoutDelayMs{600};
    std::atomic<DWORD> flyoutTimeoutMs{3000};
} g_settings;

ProcessRole g_role = ProcessRole::Unsupported;
HANDLE g_stopEvent;
HANDLE g_showEvent;
HANDLE g_captureReadyEvent;
HANDLE g_hostAliveEvent;
HANDLE g_bootstrapAvailableEvent;

HANDLE g_routerThread;
HANDLE g_routeRequestEvent;
HANDLE g_toggleRequestEvent;
HANDLE g_trackRequestEvent;
std::atomic<int> g_trackDirection{0};
std::atomic<bool> g_windhawkInitialized{false};
std::atomic<bool> g_inputSiteProcHooked{false};
HWND g_taskbarInputSiteWindow = nullptr;
WNDPROC g_inputSiteWindowProcOriginal;
[[clang::no_destroy]] winrt::com_ptr<IUIAutomation> g_taskbarAutomation;

HANDLE g_nativeThread;
HANDLE g_injectRequestEvent;
HANDLE g_panelShownEvent;
HWINEVENTHOOK g_controlCenterWinEventHook;
std::atomic<bool> g_pendingStandalone{false};
std::atomic<bool> g_bootstrapCapture{false};
std::atomic<bool> g_mediaCaptured{false};
std::atomic<bool> g_shuttingDown{false};
std::mutex g_captureMutex;
[[clang::no_destroy]] winrt::Windows::UI::Xaml::FrameworkElement
    g_capturedMedia{nullptr};
[[clang::no_destroy]] winrt::Windows::UI::Core::CoreDispatcher
    g_capturedDispatcher{nullptr};
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Controls::Panel
    g_originalMediaParent{nullptr};
uint32_t g_originalMediaIndex = 0;
uint64_t g_capturedMediaHandle = 0;
std::atomic<bool> g_reparentingCapturedMedia{false};
HWND g_capturedHostWindow = nullptr;
DWORD g_capturedHostThreadId = 0;
HWND g_capturedIslandWindow = nullptr;
HINSTANCE g_capturedHostInstance = nullptr;
bool g_capturedHostClassRegistered = false;
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource
    g_capturedXamlSource{nullptr};
double g_originalMediaWidth = NAN;
double g_originalMediaHeight = NAN;
winrt::Windows::UI::Xaml::Thickness g_originalMediaMargin{};
winrt::Windows::UI::Xaml::HorizontalAlignment g_originalHorizontalAlignment{};
winrt::Windows::UI::Xaml::VerticalAlignment g_originalVerticalAlignment{};

// Taskbar UI-thread-only gesture state.
bool g_gestureLatched;
int g_wheelDeltaAccumulator;
ULONGLONG g_lastHorizontalInputTick;
ULONGLONG g_suppressHorizontalUntil;

DWORD ClampSetting(int value, DWORD minimum, DWORD maximum) {
    if (value < static_cast<int>(minimum)) return minimum;
    if (value > static_cast<int>(maximum)) return maximum;
    return static_cast<DWORD>(value);
}

std::wstring CurrentProcessName() {
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length) return {};
    const wchar_t* name = wcsrchr(path, L'\\');
    return name ? name + 1 : path;
}

DWORD CurrentWindowsBuild() {
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersion_t>(
                                     GetProcAddress(ntdll, "RtlGetVersion"))
                               : nullptr;
    if (!rtlGetVersion) return 0;
    OSVERSIONINFOW version{sizeof(version)};
    return rtlGetVersion(&version) >= 0 ? version.dwBuildNumber : 0;
}

ProcessRole DetectProcessRole() {
    std::wstring name = CurrentProcessName();
    DWORD build = CurrentWindowsBuild();
    if (build < 22000) return ProcessRole::Unsupported;
    if (_wcsicmp(name.c_str(), L"explorer.exe") == 0)
        return ProcessRole::Explorer;
    if (_wcsicmp(name.c_str(), L"ShellHost.exe") == 0 &&
        build >= 26100) {
        return ProcessRole::ShellHost;
    }
    if (_wcsicmp(name.c_str(), L"ShellExperienceHost.exe") == 0 &&
        build < 26100) {
        return ProcessRole::ShellHost;
    }
    return ProcessRole::Unsupported;
}

void LoadSettings() {
    g_settings.requireModifier = Wh_GetIntSetting(L"requireModifier") != 0;
    PCWSTR value = Wh_GetStringSetting(L"modifier");
    ModifierKey modifier = ModifierKey::Ctrl;
    if (value) {
        if (wcscmp(value, L"shift") == 0) modifier = ModifierKey::Shift;
        else if (wcscmp(value, L"alt") == 0) modifier = ModifierKey::Alt;
        else if (wcscmp(value, L"win") == 0) modifier = ModifierKey::Win;
        Wh_FreeStringSetting(value);
    }
    g_settings.modifier = modifier;
    g_settings.reverseDirection = Wh_GetIntSetting(L"reverseDirection") != 0;
    g_settings.releaseTimeoutMs =
        ClampSetting(Wh_GetIntSetting(L"releaseTimeoutMs"), 80, 2000);
    g_settings.flyoutDelayMs =
        ClampSetting(Wh_GetIntSetting(L"nativeFlyoutDelayMs"), 250, 2500);
    g_settings.flyoutTimeoutMs =
        ClampSetting(Wh_GetIntSetting(L"nativeFlyoutTimeoutMs"), 1500, 15000);
}

bool IsModifierPressed() {
    if (!g_settings.requireModifier.load()) return true;
    switch (g_settings.modifier.load()) {
        case ModifierKey::Ctrl:
            return (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
                   (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
        case ModifierKey::Shift:
            return (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ||
                   (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
        case ModifierKey::Alt:
            return (GetAsyncKeyState(VK_LMENU) & 0x8000) ||
                   (GetAsyncKeyState(VK_RMENU) & 0x8000);
        case ModifierKey::Win:
            return (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
                   (GetAsyncKeyState(VK_RWIN) & 0x8000);
    }
    return false;
}

bool IsTaskbarClassName(HWND window) {
    wchar_t className[64];
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) return false;
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsOwnedTaskbarWindow(HWND window) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    return processId == GetCurrentProcessId() && IsTaskbarClassName(window);
}

struct TaskbarBoundsSearch {
    HMONITOR monitor;
    RECT monitorBounds;
    int top;
    bool found;
};

BOOL CALLBACK FindBottomTaskbarBoundsProc(HWND window, LPARAM parameter) {
    auto* search = reinterpret_cast<TaskbarBoundsSearch*>(parameter);
    if (!IsTaskbarClassName(window) ||
        MonitorFromWindow(window, MONITOR_DEFAULTTONULL) != search->monitor) {
        return TRUE;
    }

    RECT bounds{};
    if (!GetWindowRect(window, &bounds)) return TRUE;
    int monitorMidpoint =
        search->monitorBounds.top +
        (search->monitorBounds.bottom - search->monitorBounds.top) / 2;
    if (bounds.top > monitorMidpoint &&
        bounds.bottom >= search->monitorBounds.bottom - 2) {
        search->top = bounds.top;
        search->found = true;
        return FALSE;
    }
    return TRUE;
}

int GetBottomTaskbarTop(HMONITOR monitor, const MONITORINFO& monitorInfo) {
    TaskbarBoundsSearch search{monitor, monitorInfo.rcMonitor,
                               monitorInfo.rcWork.bottom, false};
    EnumWindows(FindBottomTaskbarBoundsProc,
                reinterpret_cast<LPARAM>(&search));
    return search.found && search.top < monitorInfo.rcWork.bottom
               ? search.top
               : monitorInfo.rcWork.bottom;
}

bool IsCtrlPressed() {
    return (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
}

bool IsInteractiveAutomationControl(CONTROLTYPEID type) {
    switch (type) {
        case UIA_ButtonControlTypeId:
        case UIA_CheckBoxControlTypeId:
        case UIA_ComboBoxControlTypeId:
        case UIA_EditControlTypeId:
        case UIA_HyperlinkControlTypeId:
        case UIA_ListItemControlTypeId:
        case UIA_MenuItemControlTypeId:
        case UIA_RadioButtonControlTypeId:
        case UIA_ScrollBarControlTypeId:
        case UIA_SliderControlTypeId:
        case UIA_SpinnerControlTypeId:
        case UIA_SplitButtonControlTypeId:
        case UIA_TabItemControlTypeId:
        case UIA_ThumbControlTypeId:
        case UIA_TreeItemControlTypeId:
        case UIA_DataItemControlTypeId:
            return true;
    }
    return false;
}

bool HasAutomationAction(IUIAutomationElement* element, PROPERTYID propertyId) {
    VARIANT value;
    VariantInit(&value);
    bool available =
        SUCCEEDED(element->GetCurrentPropertyValue(propertyId, &value)) &&
        value.vt == VT_BOOL && value.boolVal == VARIANT_TRUE;
    VariantClear(&value);
    return available;
}

bool IsEmptyTaskbarPoint(IUIAutomation* automation, POINT point) {
    if (!automation) return false;

    HWND taskbar = GetAncestor(WindowFromPoint(point), GA_ROOT);
    if (!taskbar || !IsOwnedTaskbarWindow(taskbar)) return false;

    winrt::com_ptr<IUIAutomationElement> element;
    if (FAILED(automation->ElementFromPoint(point, element.put())) || !element)
        return false;
    winrt::com_ptr<IUIAutomationTreeWalker> walker;
    if (FAILED(automation->get_RawViewWalker(walker.put())) || !walker)
        return false;

    for (int depth = 0; element && depth < 20; ++depth) {
        CONTROLTYPEID type = 0;
        element->get_CurrentControlType(&type);
        if (IsInteractiveAutomationControl(type) ||
            HasAutomationAction(element.get(),
                                UIA_IsInvokePatternAvailablePropertyId) ||
            HasAutomationAction(element.get(),
                                UIA_IsTogglePatternAvailablePropertyId) ||
            HasAutomationAction(element.get(),
                                UIA_IsSelectionItemPatternAvailablePropertyId)) {
            return false;
        }

        UIA_HWND nativeWindow = 0;
        if (SUCCEEDED(
                element->get_CurrentNativeWindowHandle(&nativeWindow)) &&
            reinterpret_cast<HWND>(nativeWindow) == taskbar) {
            return true;
        }

        winrt::com_ptr<IUIAutomationElement> parent;
        if (FAILED(walker->GetParentElement(element.get(), parent.put())) ||
            !parent) {
            // The point is known to be within this process' taskbar and no
            // interactive ancestor was found before the UIA tree ended.
            return true;
        }
        element = std::move(parent);
    }

    // A deep or incomplete UIA ancestry must not turn a real taskbar control
    // into an "empty-space" play/pause target.
    return false;
}

struct PanelSearchContext {
    bool found;
};

BOOL CALLBACK FindOpenPanelProc(HWND window, LPARAM parameter) {
    auto* context = reinterpret_cast<PanelSearchContext*>(parameter);
    wchar_t className[128];
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"ControlCenterWindow") != 0) {
        return TRUE;
    }
    if (!IsWindowVisible(window) || IsIconic(window)) return TRUE;

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) && cloaked)
        return TRUE;

    RECT bounds{};
    if (!GetWindowRect(window, &bounds) || bounds.right - bounds.left < 300 ||
        bounds.bottom - bounds.top < 250) return TRUE;

    // Current Windows 11 Quick Settings uses this dedicated top-level class.
    // Broad process matching falsely classifies unrelated ShellHost and
    // PowerToys windows as an open panel and suppresses every request.
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    Wh_Log(L"Open Quick Settings panel found: process=%u, window=%p",
           processId, window);
    context->found = true;
    return FALSE;
}

bool IsQuickPanelOpen() {
    PanelSearchContext context{false};
    EnumWindows(FindOpenPanelProc, reinterpret_cast<LPARAM>(&context));
    return context.found;
}

using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND window, RunFromWindowThreadProc procedure,
                         void* parameter) {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_taskbar-horizontal-scroll-media");
    struct Invocation {
        RunFromWindowThreadProc procedure;
        void* parameter;
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto* call = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (call->message == message) {
                    auto* invocation =
                        reinterpret_cast<Invocation*>(call->lParam);
                    invocation->procedure(invocation->parameter);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Invocation invocation{procedure, parameter};
    SendMessageW(window, message, 0,
                 reinterpret_cast<LPARAM>(&invocation));
    UnhookWindowsHookEx(hook);
    return true;
}

BOOL CALLBACK FindControlCenterWindowProc(HWND window, LPARAM parameter) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) return TRUE;
    wchar_t className[128];
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) return TRUE;
    if (wcscmp(className, L"ControlCenterWindow") == 0) {
        *reinterpret_cast<HWND*>(parameter) = window;
        return FALSE;
    }
    return TRUE;
}

void RestoreCapturedMedia();
void CleanupMediaCaptureOnUiThread();

void UnregisterCapturedHostClass() {
    if (!g_capturedHostClassRegistered || !g_capturedHostInstance) return;
    if (!UnregisterClassW(kCapturedHostClass, g_capturedHostInstance)) {
        Wh_Log(L"UnregisterClass failed: %u", GetLastError());
        return;
    }
    g_capturedHostClassRegistered = false;
    g_capturedHostInstance = nullptr;
}

LRESULT CALLBACK CapturedHostWndProc(HWND window, UINT message, WPARAM wParam,
                                     LPARAM lParam) {
    switch (message) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_TIMER:
            if (wParam == kDismissTimer) {
                KillTimer(window, kDismissTimer);
                RestoreCapturedMedia();
            }
            return 0;
        case kRestoreCapturedMediaMessage:
            RestoreCapturedMedia();
            return 0;
        case kCleanupMediaCaptureMessage:
            CleanupMediaCaptureOnUiThread();
            return 0;
        case WM_DESTROY:
            KillTimer(window, kDismissTimer);
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool EnsureCapturedHostWindow() {
    if (g_capturedHostWindow) {
        if (g_capturedHostThreadId == GetCurrentThreadId()) return true;
        Wh_Log(L"Captured host belongs to a different UI thread");
        return false;
    }

    HINSTANCE instance = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&CapturedHostWndProc),
                       &instance);
    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.lpfnWndProc = CapturedHostWndProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kCapturedHostClass;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"Native host class registration failed: %u", GetLastError());
        return false;
    }
    g_capturedHostInstance = instance;
    g_capturedHostClassRegistered = true;

    g_capturedHostWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
            WS_EX_NOREDIRECTIONBITMAP,
        kCapturedHostClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        instance, nullptr);
    if (!g_capturedHostWindow) {
        Wh_Log(L"Native host window creation failed: %u", GetLastError());
        UnregisterCapturedHostClass();
        return false;
    }
    g_capturedHostThreadId = GetCurrentThreadId();

    // The captured Windows media control already draws its own rounded outline.
    // Prevent DWM from adding another non-client frame and drop shadow to the
    // transparent XAML-island host, which otherwise appears as a ghost border
    // below and around the genuine card.
    DWMNCRENDERINGPOLICY renderingPolicy = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(g_capturedHostWindow, DWMWA_NCRENDERING_POLICY,
                          &renderingPolicy, sizeof(renderingPolicy));
    DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(g_capturedHostWindow,
                          DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference,
                          sizeof(cornerPreference));

    try {
        g_capturedXamlSource = winrt::Windows::UI::Xaml::Hosting::
            DesktopWindowXamlSource{};
        auto interop = g_capturedXamlSource.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(interop->AttachToWindow(g_capturedHostWindow));
        winrt::check_hresult(interop->get_WindowHandle(&g_capturedIslandWindow));
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Native XAML island creation failed: 0x%08X",
               static_cast<unsigned>(error.code()));
        DestroyWindow(g_capturedHostWindow);
        g_capturedHostWindow = nullptr;
        g_capturedHostThreadId = 0;
        UnregisterCapturedHostClass();
        return false;
    }
    return true;
}

void RestoreCapturedMedia() {
    if (g_capturedHostWindow) {
        KillTimer(g_capturedHostWindow, kDismissTimer);
        ShowWindow(g_capturedHostWindow, SW_HIDE);
    }
    if (g_originalMediaParent && g_capturedMedia) {
        g_reparentingCapturedMedia = true;
        try {
            if (g_capturedXamlSource) g_capturedXamlSource.Content(nullptr);
            auto children = g_originalMediaParent.Children();
            uint32_t index = g_originalMediaIndex;
            if (index > children.Size()) index = children.Size();
            children.InsertAt(index, g_capturedMedia);
            g_capturedMedia.Width(g_originalMediaWidth);
            g_capturedMedia.Height(g_originalMediaHeight);
            g_capturedMedia.Margin(g_originalMediaMargin);
            g_capturedMedia.HorizontalAlignment(g_originalHorizontalAlignment);
            g_capturedMedia.VerticalAlignment(g_originalVerticalAlignment);
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Restoring the captured media control failed: 0x%08X",
                   static_cast<unsigned>(error.code()));
        } catch (...) {
            Wh_Log(L"Restoring the captured media control failed");
        }
        g_reparentingCapturedMedia = false;
    }
    g_originalMediaParent = nullptr;
    Wh_Log(L"Captured media control restored");
}

struct NativeCardBounds {
    double x{};
    double y{};
    double width{};
    double height{};
    double score{-1.0};
};

void FindNativeCardBounds(
    const winrt::Windows::UI::Xaml::DependencyObject& parent,
    const winrt::Windows::UI::Xaml::FrameworkElement& root, double rootWidth,
    double rootHeight, int depth, NativeCardBounds& best) {
    if (!parent || depth > 16) return;

    int count = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(parent);
    for (int index = 0; index < count; ++index) {
        auto child = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::
            GetChild(parent, index);
        if (auto element =
                child.try_as<winrt::Windows::UI::Xaml::FrameworkElement>()) {
            double width = element.ActualWidth();
            double height = element.ActualHeight();
            if (width >= rootWidth * 0.85 && width <= rootWidth + 1.0 &&
                height >= 100.0 && height <= rootHeight - 2.0) {
                try {
                    auto origin = element.TransformToVisual(root).TransformPoint(
                        winrt::Windows::Foundation::Point{0.0f, 0.0f});
                    if (origin.X >= -1.0 && origin.Y >= -1.0 &&
                        origin.X + width <= rootWidth + 1.0 &&
                        origin.Y + height <= rootHeight + 1.0) {
                        double score = width * height;
                        if (winrt::get_class_name(element) ==
                            L"Windows.UI.Xaml.Controls.Border") {
                            // Prefer the full-width outline which actually
                            // paints the card over equally sized layout grids.
                            score += rootWidth * rootHeight * 10.0;
                        }
                        if (score > best.score) {
                            best = {origin.X, origin.Y, width, height, score};
                        }
                    }
                } catch (...) {
                }
            }
        }
        FindNativeCardBounds(child, root, rootWidth, rootHeight, depth + 1,
                             best);
    }
}

NativeCardBounds MeasureNativeCardBounds(
    const winrt::Windows::UI::Xaml::FrameworkElement& root, double rootWidth,
    double rootHeight) {
    NativeCardBounds bounds{0.0, 0.0, rootWidth, rootHeight, -1.0};
    FindNativeCardBounds(root, root, rootWidth, rootHeight, 0, bounds);
    return bounds;
}

void ShowCapturedMediaOnUiThread() {
    if (g_shuttingDown.load() || !g_pendingStandalone.exchange(false) ||
        !g_capturedMedia) {
        return;
    }
    bool bootstrap = g_bootstrapCapture.exchange(false);
    if (g_originalMediaParent) {
        ShowWindow(g_capturedHostWindow, SW_SHOWNOACTIVATE);
        SetTimer(g_capturedHostWindow, kDismissTimer,
                 g_settings.flyoutTimeoutMs.load(), nullptr);
        return;
    }

    try {
        auto parent = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::
                          GetParent(g_capturedMedia)
                              .try_as<winrt::Windows::UI::Xaml::Controls::Panel>();
        if (!parent) {
            Wh_Log(L"Captured media control has no panel parent");
            return;
        }

        auto children = parent.Children();
        uint32_t index = 0;
        bool found = false;
        for (; index < children.Size(); ++index) {
            if (children.GetAt(index) == g_capturedMedia) {
                found = true;
                break;
            }
        }
        if (!found) {
            Wh_Log(L"Captured media control was not found in its parent");
            return;
        }
        if (!EnsureCapturedHostWindow()) return;

        g_originalMediaParent = parent;
        g_originalMediaIndex = index;
        g_originalMediaWidth = g_capturedMedia.Width();
        g_originalMediaHeight = g_capturedMedia.Height();
        g_originalMediaMargin = g_capturedMedia.Margin();
        g_originalHorizontalAlignment =
            g_capturedMedia.HorizontalAlignment();
        g_originalVerticalAlignment = g_capturedMedia.VerticalAlignment();

        double rootWidthDip = g_capturedMedia.ActualWidth();
        double rootHeightDip = g_capturedMedia.ActualHeight();
        if (!(rootWidthDip >= 320.0 && rootWidthDip <= 700.0))
            rootWidthDip = 448.0;
        if (!(rootHeightDip >= 100.0 && rootHeightDip <= 500.0))
            rootHeightDip = 224.0;
        NativeCardBounds cardBounds = MeasureNativeCardBounds(
            g_capturedMedia, rootWidthDip, rootHeightDip);

        g_reparentingCapturedMedia = true;
        children.RemoveAt(index);
        g_capturedMedia.Width(rootWidthDip);
        g_capturedMedia.Height(rootHeightDip);
        g_capturedMedia.Margin({0, 0, 0, 0});
        g_capturedMedia.HorizontalAlignment(
            winrt::Windows::UI::Xaml::HorizontalAlignment::Stretch);
        g_capturedMedia.VerticalAlignment(
            winrt::Windows::UI::Xaml::VerticalAlignment::Stretch);
        g_capturedXamlSource.Content(g_capturedMedia);
        g_reparentingCapturedMedia = false;
        // The first standalone presentation can occur while Control Center is
        // still completing its template pass. Resolve that layout now so the
        // crop is based on the rendered card rather than a transient height.
        g_capturedMedia.UpdateLayout();
        NativeCardBounds laidOutBounds = MeasureNativeCardBounds(
            g_capturedMedia, rootWidthDip, rootHeightDip);
        if (laidOutBounds.score >= 0.0) cardBounds = laidOutBounds;

        if (bootstrap) {
            HWND panelWindow = nullptr;
            EnumWindows(FindControlCenterWindowProc,
                        reinterpret_cast<LPARAM>(&panelWindow));
            if (panelWindow) ShowWindow(panelWindow, SW_HIDE);
        }

        POINT cursor{};
        GetCursorPos(&cursor);
        HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{sizeof(monitorInfo)};
        GetMonitorInfoW(monitor, &monitorInfo);

        // Convert against the destination monitor, not the host's previous
        // monitor. This prevents right/bottom clipping on mixed-DPI setups.
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            dpiX = dpiY = GetDpiForWindow(g_capturedHostWindow);
            if (!dpiX) dpiX = dpiY = 96;
        }
        auto dipToPixelsX = [dpiX](double value) {
            return static_cast<int>(std::lround(value * dpiX / 96.0));
        };
        auto dipToPixelsY = [dpiY](double value) {
            return static_cast<int>(std::lround(value * dpiY / 96.0));
        };
        int rootWidth = dipToPixelsX(rootWidthDip);
        int rootHeight = dipToPixelsY(rootHeightDip);
        int cropLeft = dipToPixelsX(cardBounds.x);
        int cropTop = dipToPixelsY(cardBounds.y);
        int width = dipToPixelsX(cardBounds.width);
        // Preserve a small amount of Windows' transparent root below the
        // measured border. It absorbs late first-frame rasterization changes
        // without moving the visible card or restoring the old ghost frame.
        double availableBelow =
            rootHeightDip - (cardBounds.y + cardBounds.height);
        double bottomCropSlackDip =
            availableBelow > 4.0 ? 4.0
                                 : (availableBelow > 0.0 ? availableBelow : 0.0);
        int visibleHeight = dipToPixelsY(cardBounds.height);
        int height = dipToPixelsY(cardBounds.height + bottomCropSlackDip);
        int edgeMarginX = dipToPixelsX(12.0);
        int verticalGap = dipToPixelsY(24.0);
        int taskbarTop = GetBottomTaskbarTop(monitor, monitorInfo);
        int x = monitorInfo.rcWork.right - width - edgeMarginX;
        int y = taskbarTop - visibleHeight - verticalGap;
        Wh_Log(L"Native card: rootHeight=%d, top=%d, height=%d, dpi=%u, "
               L"taskbarTop=%d, bottomSlack=%d",
               static_cast<int>(std::lround(rootHeightDip)),
               static_cast<int>(std::lround(cardBounds.y)),
               static_cast<int>(std::lround(cardBounds.height)), dpiY,
               taskbarTop,
               static_cast<int>(std::lround(bottomCropSlackDip)));
        // Keep the genuine root at its Windows-provided size, but offset the
        // island so the host clips its transparent layout padding. Positioning
        // is therefore based on the visible native card border.
        // Move the parent into the target monitor's DPI context before sizing
        // its XAML child, then reveal both with their final bounds.
        SetWindowPos(g_capturedHostWindow, HWND_TOPMOST, x, y, width, height,
                     SWP_NOACTIVATE);
        SetWindowPos(g_capturedIslandWindow, nullptr, -cropLeft, -cropTop,
                     rootWidth, rootHeight,
                     SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
        SetWindowPos(g_capturedHostWindow, HWND_TOPMOST, x, y, width, height,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SetTimer(g_capturedHostWindow, kDismissTimer,
                 g_settings.flyoutTimeoutMs.load(), nullptr);
        Wh_Log(L"Standalone native media control shown");
    } catch (const winrt::hresult_error& error) {
        g_reparentingCapturedMedia = false;
        Wh_Log(L"Showing the standalone native media control failed: 0x%08X",
               static_cast<unsigned>(error.code()));
        RestoreCapturedMedia();
    } catch (...) {
        g_reparentingCapturedMedia = false;
        Wh_Log(L"Showing the standalone native media control failed");
        RestoreCapturedMedia();
    }
}

void RequestCapturedStandalone() {
    if (g_shuttingDown.load()) return;
    g_pendingStandalone = true;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    bool captured = false;
    {
        std::lock_guard lock(g_captureMutex);
        captured = g_mediaCaptured.load() && g_capturedMedia &&
                   g_capturedDispatcher;
        if (captured) dispatcher = g_capturedDispatcher;
    }
    if (!captured) g_bootstrapCapture = true;
    if (!captured || !dispatcher) {
        Wh_Log(L"Native media capture is not ready");
        return;
    }
    try {
        dispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [] { ShowCapturedMediaOnUiThread(); });
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Dispatching the standalone media control failed: 0x%08X",
               static_cast<unsigned>(error.code()));
    }
}

void RequestRestoreCapturedMedia() {
    HWND hostWindow = g_capturedHostWindow;
    if (hostWindow && !PostMessageW(hostWindow,
                                    kRestoreCapturedMediaMessage, 0, 0)) {
        Wh_Log(L"Failed to post native-card restore: %u", GetLastError());
    }
}

HRESULT InjectMediaCaptureTAP();

void RequestNativeControl() {
    if (g_routeRequestEvent) SetEvent(g_routeRequestEvent);
}

constexpr auto kMediaOperationTimeout = std::chrono::milliseconds(2000);

template <typename Operation>
bool WaitForMediaOperation(const Operation& operation, PCWSTR label) {
    auto status = operation.wait_for(kMediaOperationTimeout);
    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
        return true;
    }
    try {
        operation.Cancel();
    } catch (...) {
    }
    Wh_Log(L"Media operation timed out: %s, status=%d", label,
           static_cast<int>(status));
    return false;
}

winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSessionManager
RequestMediaManager() {
    auto operation = winrt::Windows::Media::Control::
        GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
    if (!WaitForMediaOperation(operation, L"RequestAsync")) return nullptr;
    return operation.GetResults();
}

bool ToggleCurrentMediaSession() {
    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return false;
    try {
        auto manager = RequestMediaManager();
        if (!manager) return false;
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return false;
        auto session = manager.GetCurrentSession();
        if (!session) {
            Wh_Log(L"No current media session was found");
            return false;
        }

        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return false;
        auto operation = session.TryTogglePlayPauseAsync();
        if (!WaitForMediaOperation(operation, L"TryTogglePlayPauseAsync")) {
            return false;
        }
        bool succeeded = operation.GetResults();
        Wh_Log(L"Play/pause request completed: succeeded=%d", succeeded);
        if (succeeded) RequestNativeControl();
        return succeeded;
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Play/pause request failed: 0x%08X",
               static_cast<unsigned>(error.code()));
        return false;
    }
}

bool SkipCurrentMediaSession(bool nextTrack) {
    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return false;
    try {
        auto manager = RequestMediaManager();
        if (!manager) return false;
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return false;
        auto session = manager.GetCurrentSession();
        if (!session) {
            Wh_Log(L"No current media session was found");
            return false;
        }

        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return false;
        auto operation = nextTrack ? session.TrySkipNextAsync()
                                   : session.TrySkipPreviousAsync();
        if (!WaitForMediaOperation(
                operation,
                nextTrack ? L"TrySkipNextAsync" : L"TrySkipPreviousAsync")) {
            return false;
        }
        bool succeeded = operation.GetResults();
        Wh_Log(L"Track-skip request completed: next=%d, succeeded=%d",
               nextTrack, succeeded);
        if (succeeded) RequestNativeControl();
        return succeeded;
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Track-skip request failed: 0x%08X",
               static_cast<unsigned>(error.code()));
        return false;
    }
}

void ResetGesture() {
    g_gestureLatched = false;
    g_wheelDeltaAccumulator = 0;
    g_lastHorizontalInputTick = 0;
}

IUIAutomation* GetTaskbarAutomation() {
    if (g_taskbarAutomation) return g_taskbarAutomation.get();
    HRESULT result = CoCreateInstance(
        CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(g_taskbarAutomation.put()));
    if (FAILED(result)) {
        result = CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(g_taskbarAutomation.put()));
    }
    Wh_Log(L"Taskbar UI Automation initialization: 0x%08X",
           static_cast<unsigned>(result));
    return g_taskbarAutomation.get();
}

bool HandleHorizontalWheel(WPARAM wParam) {
    ULONGLONG now = GetTickCount64();
    if ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) ||
        (g_suppressHorizontalUntil && now <= g_suppressHorizontalUntil)) {
        ResetGesture();
        return true;
    }
    g_suppressHorizontalUntil = 0;
    if (!g_lastHorizontalInputTick ||
        now - g_lastHorizontalInputTick > g_settings.releaseTimeoutMs.load()) {
        g_gestureLatched = false;
        g_wheelDeltaAccumulator = 0;
    }
    g_lastHorizontalInputTick = now;
    if (g_gestureLatched) return true;
    if (!IsModifierPressed()) {
        g_wheelDeltaAccumulator = 0;
        return false;
    }

    g_wheelDeltaAccumulator += GET_WHEEL_DELTA_WPARAM(wParam);
    if (g_wheelDeltaAccumulator >= WHEEL_DELTA ||
        g_wheelDeltaAccumulator <= -WHEEL_DELTA) {
        bool nextTrack = g_wheelDeltaAccumulator > 0;
        if (g_settings.reverseDirection.load()) nextTrack = !nextTrack;
        g_gestureLatched = true;
        g_wheelDeltaAccumulator = 0;
        g_trackDirection = nextTrack ? 1 : -1;
        if (g_trackRequestEvent) SetEvent(g_trackRequestEvent);
    }
    return true;
}

LRESULT CALLBACK InputSiteWindowProcHook(HWND window, UINT message,
                                         WPARAM wParam, LPARAM lParam) {
    if (g_shuttingDown.load()) {
        return g_inputSiteWindowProcOriginal(window, message, wParam, lParam);
    }
    HWND root = GetAncestor(window, GA_ROOT);
    if (root && IsOwnedTaskbarWindow(root)) {
        if (message == WM_POINTERHWHEEL && HandleHorizontalWheel(wParam)) {
            return 0;
        }
        if (message == WM_POINTERDOWN && IsCtrlPressed() &&
            IS_POINTER_THIRDBUTTON_WPARAM(wParam)) {
            // Some tilting wheels emit a horizontal packet when pressed.
            g_suppressHorizontalUntil = GetTickCount64() + 500;
            ResetGesture();
            POINT point{static_cast<SHORT>(LOWORD(lParam)),
                        static_cast<SHORT>(HIWORD(lParam))};
            if (IsEmptyTaskbarPoint(GetTaskbarAutomation(), point) &&
                g_toggleRequestEvent) {
                SetEvent(g_toggleRequestEvent);
            }
        }
    }
    return g_inputSiteWindowProcOriginal(window, message, wParam, lParam);
}

void HandleIdentifiedInputSiteWindow(HWND window) {
    HWND bridge = GetParent(window);
    wchar_t className[64];
    if (!bridge ||
        !GetClassNameW(bridge, className, ARRAYSIZE(className)) ||
        wcscmp(className,
               L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }
    HWND taskbar = GetParent(bridge);
    if (!taskbar || !IsOwnedTaskbarWindow(taskbar)) return;
    g_taskbarInputSiteWindow = window;

    if (g_inputSiteProcHooked.load()) return;
    auto windowProc = reinterpret_cast<WNDPROC>(
        GetWindowLongPtrW(window, GWLP_WNDPROC));
    if (!windowProc ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(windowProc),
                            reinterpret_cast<void*>(InputSiteWindowProcHook),
                            reinterpret_cast<void**>(
                                &g_inputSiteWindowProcOriginal))) {
        Wh_Log(L"Failed to hook taskbar InputSite window procedure");
        return;
    }
    g_inputSiteProcHooked = true;
    if (g_windhawkInitialized.load()) Wh_ApplyHookOperations();
    Wh_Log(L"Hooked taskbar InputSite window procedure: %p", windowProc);
}

BOOL CALLBACK FindExistingInputSiteProc(HWND window, LPARAM) {
    if (!IsOwnedTaskbarWindow(window)) return TRUE;
    HWND bridge = FindWindowExW(
        window, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    HWND inputSite = bridge ? FindWindowExW(
                                  bridge, nullptr,
                                  L"Windows.UI.Input.InputSite.WindowClass",
                                  nullptr)
                            : nullptr;
    if (inputSite) HandleIdentifiedInputSiteWindow(inputSite);
    return TRUE;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int,
                                           int, int, int, HWND, HMENU,
                                           HINSTANCE, void*, DWORD);
CreateWindowInBand_t CreateWindowInBandOriginal;

HWND WINAPI CreateWindowInBandHook(DWORD exStyle, LPCWSTR className,
                                    LPCWSTR windowName, DWORD style, int x,
                                    int y, int width, int height, HWND parent,
                                    HMENU menu, HINSTANCE instance,
                                    void* parameter, DWORD band) {
    HWND window = CreateWindowInBandOriginal(
        exStyle, className, windowName, style, x, y, width, height, parent,
        menu, instance, parameter, band);
    bool textualClass =
        (reinterpret_cast<ULONG_PTR>(className) & ~ULONG_PTR{0xffff}) != 0;
    if (window && textualClass &&
        _wcsicmp(className, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        HandleIdentifiedInputSiteWindow(window);
    }
    return window;
}

void ShowNativeControlNow() {
    // Never create another surface while the user's panel is visible. The
    // system media session updates its existing card by itself.
    if (IsQuickPanelOpen()) {
        Wh_Log(L"Quick Settings is open; using its existing media control");
        return;
    }
    if (!g_showEvent) return;
    if (!SetEvent(g_showEvent)) {
        DWORD error = GetLastError();
        Wh_Log(L"Signalling the native media host failed: %u", error);
        CloseHandle(g_showEvent);
        g_showEvent = nullptr;
        return;
    }
    Wh_Log(L"Standalone native media control requested");
    if (g_captureReadyEvent &&
        WaitForSingleObject(g_captureReadyEvent, 0) == WAIT_OBJECT_0) {
        return;
    }

    bool hostAlive =
        g_hostAliveEvent &&
        WaitForSingleObject(g_hostAliveEvent, 0) == WAIT_OBJECT_0;
    bool firstBootstrap =
        hostAlive && g_bootstrapAvailableEvent &&
        WaitForSingleObject(g_bootstrapAvailableEvent, 0) == WAIT_OBJECT_0;
    if (firstBootstrap) {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
        HINSTANCE result = ShellExecuteW(nullptr, L"open", L"ms-controlcenter:",
                                         nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(result) <= 32) {
            Wh_Log(L"Opening Quick Settings failed: %Id",
                   reinterpret_cast<INT_PTR>(result));
        }
    } else if (!hostAlive) {
        Wh_Log(L"Native media host is unavailable; skipping standalone "
               L"bootstrap");
    }
}

DWORD WINAPI RouterThreadProc(void*) {
    HRESULT apartmentResult = RoInitialize(RO_INIT_MULTITHREADED);
    bool uninitializeApartment = SUCCEEDED(apartmentResult);
    HANDLE events[] = {g_stopEvent, g_toggleRequestEvent, g_trackRequestEvent,
                       g_routeRequestEvent};
    ULONGLONG showDueAt = 0;
    while (true) {
        DWORD timeout = INFINITE;
        if (showDueAt) {
            ULONGLONG now = GetTickCount64();
            timeout = now >= showDueAt
                          ? 0
                          : static_cast<DWORD>(showDueAt - now);
        }
        DWORD wait = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                            timeout);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_TIMEOUT) {
            showDueAt = 0;
            ShowNativeControlNow();
            continue;
        }
        if (wait == WAIT_OBJECT_0 + 1) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
            Wh_Log(L"Empty-taskbar play/pause requested");
            ToggleCurrentMediaSession();
            continue;
        }
        if (wait == WAIT_OBJECT_0 + 2) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
            int direction = g_trackDirection.exchange(0);
            if (direction) SkipCurrentMediaSession(direction > 0);
            continue;
        }
        if (wait == WAIT_OBJECT_0 + 3) {
            showDueAt =
                GetTickCount64() + g_settings.flyoutDelayMs.load();
            continue;
        }
        break;
    }
    if (uninitializeApartment) RoUninitialize();
    return 0;
}

void CALLBACK ControlCenterWinEventProc(HWINEVENTHOOK, DWORD event,
                                        HWND window, LONG objectId,
                                        LONG childId, DWORD, DWORD) {
    if (event != EVENT_OBJECT_SHOW || objectId != OBJID_WINDOW ||
        childId != CHILDID_SELF || !window) {
        return;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) return;
    wchar_t className[128];
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"ControlCenterWindow") != 0) {
        return;
    }
    if (g_injectRequestEvent) SetEvent(g_injectRequestEvent);
    if (g_panelShownEvent) SetEvent(g_panelShownEvent);
}

DWORD WINAPI NativeHostThreadProc(void*) {
    Wh_Log(L"Native host worker starting");
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        Wh_Log(L"Native host apartment initialized");
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Native host apartment initialization failed: 0x%08X",
               error.code().value);
        return 0;
    }

    g_controlCenterWinEventHook = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr,
        ControlCenterWinEventProc, GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT);
    if (!g_controlCenterWinEventHook) {
        Wh_Log(L"Control Center event hook failed: %u", GetLastError());
    }

    HWND existingPanel = nullptr;
    EnumWindows(FindControlCenterWindowProc,
                reinterpret_cast<LPARAM>(&existingPanel));
    if (existingPanel && g_injectRequestEvent) SetEvent(g_injectRequestEvent);

    HANDLE events[] = {g_stopEvent, g_showEvent, g_injectRequestEvent,
                       g_panelShownEvent};
    bool running = true;
    while (running) {
        DWORD wait = MsgWaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                               INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Native host received a standalone-card request");
            RequestCapturedStandalone();
        } else if (wait == WAIT_OBJECT_0 + 2) {
            if (!g_mediaCaptured.load() && !g_shuttingDown.load()) {
                HRESULT tapResult = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
                for (int attempt = 0;
                     attempt < 30 && !g_mediaCaptured.load(); ++attempt) {
                    if (WaitForSingleObject(g_stopEvent, attempt ? 100 : 0) ==
                        WAIT_OBJECT_0) {
                        break;
                    }
                    tapResult = InjectMediaCaptureTAP();
                    Wh_Log(L"XAML diagnostics injection: 0x%08X",
                           static_cast<unsigned>(tapResult));
                    if (SUCCEEDED(tapResult)) break;
                }
                if (FAILED(tapResult) && !g_mediaCaptured.load()) {
                    Wh_Log(L"XAML diagnostics injection failed: 0x%08X",
                           static_cast<unsigned>(tapResult));
                }
            }
        } else if (wait == WAIT_OBJECT_0 + 3) {
            RequestRestoreCapturedMedia();
        } else if (wait == WAIT_OBJECT_0 + ARRAYSIZE(events)) {
            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        } else {
            break;
        }
    }
    if (g_controlCenterWinEventHook) {
        UnhookWinEvent(g_controlCenterWinEventHook);
        g_controlCenterWinEventHook = nullptr;
    }
    winrt::uninit_apartment();
    return 0;
}

void ClearCapturedMediaOnUiThread() {
    RestoreCapturedMedia();
    std::lock_guard lock(g_captureMutex);
    g_capturedMedia = nullptr;
    g_capturedDispatcher = nullptr;
    g_capturedMediaHandle = 0;
    g_mediaCaptured = false;
    if (g_captureReadyEvent) ResetEvent(g_captureReadyEvent);
}

class MediaVisualTreeWatcher
    : public winrt::implements<MediaVisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    explicit MediaVisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : diagnostics_(site.as<IXamlDiagnostics>()) {
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](void* parameter) -> DWORD {
                auto* watcher =
                    reinterpret_cast<MediaVisualTreeWatcher*>(parameter);
                HRESULT result = watcher->diagnostics_
                                     .as<IVisualTreeService3>()
                                     ->AdviseVisualTreeChange(watcher);
                Wh_Log(L"XAML visual-tree watcher registration: 0x%08X",
                       static_cast<unsigned>(result));
                watcher->Release();
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    void Unadvise() {
        diagnostics_.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation, VisualElement element,
        VisualMutationType mutationType) noexcept override {
        try {
            if (mutationType == Remove) {
                if (!g_reparentingCapturedMedia.load() &&
                    element.Handle == g_capturedMediaHandle) {
                    ClearCapturedMediaOnUiThread();
                    Wh_Log(L"Captured media control was removed");
                }
                return S_OK;
            }
            if (mutationType != Add || !element.Type ||
                wcscmp(element.Type, kPrivateMediaControlClass) != 0) {
                return S_OK;
            }
            if (element.Handle == g_capturedMediaHandle) return S_OK;
            winrt::Windows::Foundation::IInspectable inspectable{nullptr};
            winrt::check_hresult(diagnostics_->GetIInspectableFromHandle(
                element.Handle,
                reinterpret_cast<::IInspectable**>(winrt::put_abi(inspectable))));
            auto media = inspectable.try_as<
                winrt::Windows::UI::Xaml::FrameworkElement>();
            if (!media) return S_OK;

            if (g_capturedMedia) ClearCapturedMediaOnUiThread();
            winrt::Windows::UI::Core::CoreDispatcher dispatcher =
                media.Dispatcher();
            {
                std::lock_guard lock(g_captureMutex);
                g_capturedMedia = media;
                g_capturedDispatcher = dispatcher;
                g_capturedMediaHandle = element.Handle;
                g_mediaCaptured = true;
            }
            if (g_captureReadyEvent) SetEvent(g_captureReadyEvent);
            Wh_Log(L"Captured Windows Quick Settings media control");
            if (g_pendingStandalone.load() && !g_shuttingDown.load()) {
                dispatcher.RunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                    [] { ShowCapturedMediaOnUiThread(); });
            }
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Processing a media-control visual-tree change failed: "
                   L"0x%08X", static_cast<unsigned>(error.code()));
        } catch (...) {
            Wh_Log(L"Processing a media-control visual-tree change failed");
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle, VisualElementState, LPCWSTR) noexcept override {
        return S_OK;
    }

   private:
    winrt::com_ptr<IXamlDiagnostics> diagnostics_;
};

[[clang::no_destroy]] winrt::com_ptr<MediaVisualTreeWatcher>
    g_visualTreeWatcher;

// {81D412E7-94A1-45D9-8997-F87EEB61B332}
constexpr CLSID CLSID_MediaCaptureTAP = {
    0x81d412e7,
    0x94a1,
    0x45d9,
    {0x89, 0x97, 0xf8, 0x7e, 0xeb, 0x61, 0xb3, 0x32}};

class MediaCaptureTAP
    : public winrt::implements<MediaCaptureTAP, IObjectWithSite,
                               winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) noexcept override {
        try {
            if (g_visualTreeWatcher) {
                g_visualTreeWatcher->Unadvise();
                g_visualTreeWatcher = nullptr;
            }
            site_.copy_from(site);
            if (site_) {
                HMODULE module = nullptr;
                GetModuleHandleExW(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(&CapturedHostWndProc), &module);
                if (module) FreeLibrary(module);
                g_visualTreeWatcher =
                    winrt::make_self<MediaVisualTreeWatcher>(site_);
            }
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** result) noexcept override {
        return site_.as(iid, result);
    }

   private:
    winrt::com_ptr<IUnknown> site_;
};

template <typename T>
struct SimpleClassFactory
    : winrt::implements<SimpleClassFactory<T>, IClassFactory,
                        winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid,
                                             void** result) noexcept override {
        try {
            if (outer) return CLASS_E_NOAGGREGATION;
            *result = nullptr;
            return winrt::make<T>().as(iid, result);
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

extern "C" __declspec(dllexport) HRESULT WINAPI
DllGetClassObject(REFCLSID clsid, REFIID iid, void** result) {
    if (clsid != CLSID_MediaCaptureTAP) return CLASS_E_CLASSNOTAVAILABLE;
    *result = nullptr;
    return winrt::make<SimpleClassFactory<MediaCaptureTAP>>().as(iid, result);
}

extern "C" __declspec(dllexport) HRESULT WINAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

HRESULT InjectMediaCaptureTAP() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&InjectMediaCaptureTAP),
                            &module)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    wchar_t location[MAX_PATH];
    DWORD length = GetModuleFileNameW(module, location, ARRAYSIZE(location));
    if (!length || length >= ARRAYSIZE(location))
        return HRESULT_FROM_WIN32(GetLastError());

    HMODULE xaml = LoadLibraryExW(L"Windows.UI.Xaml.dll", nullptr,
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!xaml) return HRESULT_FROM_WIN32(GetLastError());
    using InitializeXamlDiagnosticsEx_t =
        decltype(&InitializeXamlDiagnosticsEx);
    auto initialize = reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
        GetProcAddress(xaml, "InitializeXamlDiagnosticsEx"));
    if (!initialize) return HRESULT_FROM_WIN32(GetLastError());

    HRESULT result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int index = 1; index <= 10000; ++index) {
        wchar_t connection[64];
        swprintf_s(connection, L"VisualDiagConnection%d", index);
        result = initialize(connection, GetCurrentProcessId(), L"", location,
                            CLSID_MediaCaptureTAP, nullptr);
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) break;
    }
    return result;
}

void CloseHandleIfPresent(HANDLE& handle) {
    if (handle) CloseHandle(handle);
    handle = nullptr;
}

HANDLE CreateSharedEvent(PCWSTR name, DWORD flags) {
    // Both injected processes run as the interactive user in this session.
    // Same-user medium-integrity processes can signal these data-free events;
    // the only effect is requesting or announcing the native media control.
    return CreateEventExW(nullptr, name, flags,
                          EVENT_MODIFY_STATE | SYNCHRONIZE);
}

bool InitializeExplorerRole() {
    g_routeRequestEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_toggleRequestEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_trackRequestEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_routeRequestEvent || !g_toggleRequestEvent || !g_trackRequestEvent) {
        return false;
    }
    g_routerThread = CreateThread(nullptr, 0, RouterThreadProc, nullptr, 0,
                                  nullptr);
    if (!g_routerThread) return false;

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto createWindowInBand = user32
                                  ? reinterpret_cast<CreateWindowInBand_t>(
                                        GetProcAddress(user32,
                                                       "CreateWindowInBand"))
                                  : nullptr;
    if (!createWindowInBand ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(createWindowInBand),
                            reinterpret_cast<void*>(CreateWindowInBandHook),
                            reinterpret_cast<void**>(
                                &CreateWindowInBandOriginal))) {
        Wh_Log(L"Failed to hook CreateWindowInBand");
        return false;
    }
    EnumWindows(FindExistingInputSiteProc, 0);
    return true;
}

bool InitializeShellRole() {
    Wh_Log(L"Initializing the native Quick Settings host role");
    ResetEvent(g_captureReadyEvent);
    ResetEvent(g_hostAliveEvent);
    ResetEvent(g_bootstrapAvailableEvent);
    g_injectRequestEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_panelShownEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_injectRequestEvent || !g_panelShownEvent) return false;
    g_nativeThread = CreateThread(nullptr, 0, NativeHostThreadProc, nullptr, 0,
                                  nullptr);
    if (g_nativeThread) {
        SetEvent(g_hostAliveEvent);
        SetEvent(g_bootstrapAvailableEvent);
        Wh_Log(L"Native Quick Settings host role ready");
    }
    return g_nativeThread != nullptr;
}

void CleanupMediaCaptureOnUiThread() {
    try {
        RestoreCapturedMedia();
        if (g_capturedXamlSource) g_capturedXamlSource.Close();
        g_capturedXamlSource = nullptr;
        if (g_capturedHostWindow) {
            DestroyWindow(g_capturedHostWindow);
            g_capturedHostWindow = nullptr;
        }
        g_capturedHostThreadId = 0;
        g_capturedIslandWindow = nullptr;
        std::lock_guard lock(g_captureMutex);
        g_originalMediaParent = nullptr;
        g_capturedMedia = nullptr;
        g_capturedDispatcher = nullptr;
        g_capturedMediaHandle = 0;
        g_mediaCaptured = false;
    } catch (...) {
        Wh_Log(L"Synchronous media-capture cleanup failed");
    }
}

void ShutdownMediaCapture() {
    if (g_role != ProcessRole::ShellHost) return;
    if (g_captureReadyEvent) ResetEvent(g_captureReadyEvent);
    if (g_hostAliveEvent) ResetEvent(g_hostAliveEvent);
    if (g_bootstrapAvailableEvent) ResetEvent(g_bootstrapAvailableEvent);
    if (g_visualTreeWatcher) {
        try {
            g_visualTreeWatcher->Unadvise();
        } catch (...) {
        }
        g_visualTreeWatcher = nullptr;
    }

    HWND hostWindow = g_capturedHostWindow;
    if (hostWindow && IsWindow(hostWindow)) {
        // The private host owns the XAML objects. This message is synchronous
        // and handled by its WndProc on the owning XAML UI thread.
        SendMessageW(hostWindow, kCleanupMediaCaptureMessage, 0, 0);
    } else {
        HWND controlCenterWindow = nullptr;
        EnumWindows(FindControlCenterWindowProc,
                    reinterpret_cast<LPARAM>(&controlCenterWindow));
        if (controlCenterWindow) {
            RunFromWindowThread(
                controlCenterWindow,
                [](void*) { CleanupMediaCaptureOnUiThread(); }, nullptr);
        } else {
            // No XAML window means no live thread-affine capture can be
            // released. The no_destroy holders remain inert until process exit.
        }
    }
    // The synchronous UI-thread call has returned, so no host WndProc is still
    // on the stack and the process-wide class can be removed safely here.
    UnregisterCapturedHostClass();
    g_mediaCaptured = false;
}

void ReleaseTaskbarAutomation() {
    if (!g_taskbarAutomation) return;
    HWND inputSite = g_taskbarInputSiteWindow;
    if (inputSite && IsWindow(inputSite)) {
        RunFromWindowThread(
            inputSite, [](void*) { g_taskbarAutomation = nullptr; }, nullptr);
    } else {
        g_taskbarAutomation = nullptr;
    }
}

void StopRuntime() {
    g_shuttingDown = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_routerThread) WaitForSingleObject(g_routerThread, INFINITE);
    if (g_nativeThread) WaitForSingleObject(g_nativeThread, INFINITE);
    ShutdownMediaCapture();
    if (g_role == ProcessRole::Explorer) ReleaseTaskbarAutomation();
    CloseHandleIfPresent(g_routerThread);
    CloseHandleIfPresent(g_nativeThread);
    CloseHandleIfPresent(g_routeRequestEvent);
    CloseHandleIfPresent(g_toggleRequestEvent);
    CloseHandleIfPresent(g_trackRequestEvent);
    CloseHandleIfPresent(g_injectRequestEvent);
    CloseHandleIfPresent(g_panelShownEvent);
    CloseHandleIfPresent(g_captureReadyEvent);
    CloseHandleIfPresent(g_hostAliveEvent);
    CloseHandleIfPresent(g_bootstrapAvailableEvent);
    CloseHandleIfPresent(g_showEvent);
    CloseHandleIfPresent(g_stopEvent);
}

}  // namespace

BOOL Wh_ModInit() {
    g_role = DetectProcessRole();
    if (g_role == ProcessRole::Unsupported) return FALSE;
    g_shuttingDown = false;
    Wh_Log(L"Initializing process role: %s",
           g_role == ProcessRole::ShellHost ? L"Quick Settings host"
                                            : L"Explorer taskbar");
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_showEvent = CreateSharedEvent(kShowEventObjectName, 0);
    g_captureReadyEvent = CreateSharedEvent(
        kCaptureReadyEventObjectName, CREATE_EVENT_MANUAL_RESET);
    g_hostAliveEvent = CreateSharedEvent(kHostAliveEventObjectName,
                                         CREATE_EVENT_MANUAL_RESET);
    g_bootstrapAvailableEvent =
        CreateSharedEvent(kBootstrapAvailableEventObjectName, 0);
    if (!g_stopEvent || !g_showEvent || !g_captureReadyEvent ||
        !g_hostAliveEvent || !g_bootstrapAvailableEvent) {
        Wh_Log(L"Event creation failed: %u", GetLastError());
        StopRuntime();
        return FALSE;
    }
    bool initialized = false;
    if (g_role == ProcessRole::Explorer)
        initialized = InitializeExplorerRole();
    else if (g_role == ProcessRole::ShellHost)
        initialized = InitializeShellRole();

    if (!initialized) {
        Wh_Log(L"Process-role initialization failed");
        StopRuntime();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_windhawkInitialized = true;
    if (g_role == ProcessRole::Explorer) {
        EnumWindows(FindExistingInputSiteProc, 0);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_role == ProcessRole::Explorer) {
        HWND inputSite = g_taskbarInputSiteWindow;
        if (inputSite && IsWindow(inputSite)) {
            RunFromWindowThread(
                inputSite, [](void*) { ResetGesture(); }, nullptr);
        }
    }
}

void Wh_ModUninit() {
    g_windhawkInitialized = false;
    StopRuntime();
}
