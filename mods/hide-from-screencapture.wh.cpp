// ==WindhawkMod==
// @id              hide-from-screencapture
// @name            Capture Toggle
// @description     Toggle screen-capture exclusion for Windows 11 taskbar apps, with an optional hidden-window border.
// @version         1.1.0
// @author          AjaxFNC
// @github          https://github.com/AjaxFNC-YT
// @include         *
// @exclude         valorant-win64-shipping.exe
// @exclude         fortniteclient-win64-shipping.exe
// @exclude         easyanticheat.exe
// @exclude         easyanticheat_eos.exe
// @exclude         beservice.exe
// @exclude         beservice_x64.exe
// @exclude         vgc.exe
// @exclude         vgtray.exe
// @exclude         riotclientservices.exe
// @exclude         start_protected_game.exe
// @compilerOptions -lole32 -loleaut32 -luuid -ldwmapi -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Capture Toggle
Hide applications from your screen capture (e.g. OBS, Zoom, Teams, Discord) by
middle-clicking the app on your taskbar or by using a hover hotkey. This mod is
designed for Windows 11 and may not work or be buggy on Windows 10.

## How a taskbar button is matched
The hovered taskbar button is resolved to its Application User Model ID using
the same app resolver the shell itself uses, then every top-level window that
belongs to that AppId is collected. This makes grouped buttons work: a button
labeled "File Explorer - 2 running windows" toggles both windows at once.

When the taskbar exposes a concrete window handle for the hovered item (for
example when hovering a thumbnail in a group flyout), that window is used
directly instead.

## Window scope
Choose **All windows from the app** to toggle every window that belongs to the
taskbar button, or **Only the top window** to toggle just the topmost window of
that group.

## Safety
Using capture-hiding on games can cause instability, crashes, or even
anti-cheat trouble such as an in-game ban. Windhawk decides which processes the
mod is injected into before this mod's code runs.

The metadata excludes several known anti-cheat executables from injection, and
Windhawk globally excludes many common game and anti-cheat installation paths.
These exclusions are best-effort: renamed executables, other anti-cheat
components, and games in non-standard locations may still be injected. Add your
own exclusions in the mod's advanced settings if needed.

## Trigger options
You can keep the original middle-click behavior, switch to a hover hotkey, or
allow both from the mod settings.

## Compatibility
This mod is designed for Windows 11. Windows 10 support is untested and is most
likely broken. Windows running at a higher integrity level than Explorer (apps
started as administrator) can only be toggled when Windhawk is able to inject
into them.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShowHiddenBorder: true
  $name: Show a colored border around hidden windows
  $description: Applies a colored border to windows that this mod currently hides from capture.

- HiddenBorderColor:
  - Red: 255
  - Green: 0
  - Blue: 0
  $name: Hidden window border color
  $description: RGB color used for the border around windows hidden from capture.

- TargetScope: AllWindows
  $name: Windows to hide
  $description: Choose whether a taskbar trigger toggles every window of the selected app or only the topmost one.
  $options:
  - AllWindows: All windows from the app
  - SingleWindow: Only the top window

- TriggerMode: MiddleClick
  $name: Trigger mode
  $options:
  - MiddleClick: Middle click
  - HoverHotkey: Hover hotkey
  - Both: Middle click and hover hotkey

- TriggerModifier: None
  $name: Modifier for middle click
  $description: Optional modifier required for the middle-click trigger.
  $options:
  - None: None
  - Ctrl: Ctrl
  - Shift: Shift
  - Alt: Alt
  - Win: Win

- HoverHotkey: F8
  $name: Hover hotkey
  $description: Press this key while hovering a taskbar app button when hover hotkey mode is enabled.
  $options:
  - F8: F8
  - F9: F9
  - F10: F10
  - F11: F11
  - F12: F12
  - Pause: Pause
  - ScrollLock: Scroll Lock
*/
// ==/WindhawkModSettings==

#define _WIN32_WINNT 0x0A00
#define WINRT_LEAN_AND_MEAN

#include <windows.h>
#include <appmodel.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <uiautomation.h>
#include <wrl.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

extern "C" IMAGE_DOS_HEADER __ImageBase;

constexpr wchar_t kToggleMessageName[] =
    L"Windhawk.HideFromScreenCapture.ToggleMessage.v1";
constexpr wchar_t kReceiverWindowClassName[] =
    L"Windhawk.HideFromScreenCapture.Receiver.v1";
constexpr UINT kSwallowWindowMs = 500;
constexpr UINT kToggleSendTimeoutMs = 700;
constexpr UINT kExplorerHookRetryIntervalMs = 10000;
constexpr UINT kExplorerToggleMessage = WM_APP + 1;
constexpr UINT kStopReceiverMessage = WM_APP + 2;
constexpr COLORREF kDwmDefaultColor = 0xFFFFFFFF;

constexpr DWORD kWdaExcludeFromCapture = 0x00000011;
constexpr DWORD kWdaMonitor = 0x00000001;

// Depth limit while walking up the UI Automation tree from the hovered point.
constexpr int kMaxUiaWalkDepth = 16;

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

// Sent as the lParam of the toggle message.
enum class WindowAction : LPARAM {
    Query = 0,
    Unhide = 1,
    Hide = 2,
};

enum class ToggleResult : ULONG_PTR {
    Failed = 0,
    Hidden = 1,
    Unhidden = 2,
    HiddenCompatibility = 3,
};

struct UiaButtonDescriptor {
    std::wstring name;
    std::wstring automationId;
    std::wstring className;
    std::wstring frameworkId;
};

enum class TriggerMode {
    MiddleClick = 0,
    HoverHotkey = 1,
    Both = 2,
};

enum class TargetScope {
    AllWindows = 0,
    SingleWindow = 1,
};

enum class TriggerModifier {
    None = 0,
    Ctrl = 1,
    Shift = 2,
    Alt = 3,
    Win = 4,
};

enum class HoverHotkey {
    F8 = 0,
    F9 = 1,
    F10 = 2,
    F11 = 3,
    F12 = 4,
    Pause = 5,
    ScrollLock = 6,
};

struct ModSettings {
    bool showHiddenBorder = true;
    COLORREF hiddenBorderColor = RGB(255, 0, 0);
    TargetScope targetScope = TargetScope::AllWindows;
    TriggerMode triggerMode = TriggerMode::MiddleClick;
    TriggerModifier triggerModifier = TriggerModifier::None;
    HoverHotkey hoverHotkey = HoverHotkey::F8;
};

struct ManagedWindowState {
    DWORD originalAffinity = WDA_NONE;
    bool borderApplied = false;
};

UINT g_toggleMessage = 0;

std::atomic<bool> g_processIsExplorer{false};
std::atomic<bool> g_unloading{false};
std::mutex g_managedWindowsMutex;
std::unordered_map<HWND, ManagedWindowState> g_managedWindows;
ModSettings g_settings;

HANDLE g_receiverThread = nullptr;
DWORD g_receiverThreadId = 0;
HANDLE g_receiverReadyEvent = nullptr;
std::atomic<HWND> g_receiverWindow{nullptr};

HANDLE g_explorerHookThread = nullptr;
DWORD g_explorerHookThreadId = 0;
HANDLE g_explorerWorkerThread = nullptr;
DWORD g_explorerWorkerThreadId = 0;
HANDLE g_explorerWorkerReadyEvent = nullptr;
HHOOK g_explorerMouseHook = nullptr;
HHOOK g_explorerKeyboardHook = nullptr;

// Worker-thread-only COM objects (single-threaded apartment bound).
[[clang::no_destroy]] ComPtr<IUIAutomation> g_uia;

std::atomic<DWORD> g_swallowStartTick{0};
std::atomic<bool> g_hotkeyDown{false};
std::atomic<bool> g_hotkeySwallowed{false};

template <size_t N>
bool SettingEquals(PCWSTR value, const wchar_t (&literal)[N]) {
    return wcscmp(value, literal) == 0;
}

void LoadSettings() {
    g_settings.showHiddenBorder = Wh_GetIntSetting(L"ShowHiddenBorder") != 0;
    const BYTE borderRed = static_cast<BYTE>(
        std::clamp(Wh_GetIntSetting(L"HiddenBorderColor.Red"), 0, 255));
    const BYTE borderGreen = static_cast<BYTE>(
        std::clamp(Wh_GetIntSetting(L"HiddenBorderColor.Green"), 0, 255));
    const BYTE borderBlue = static_cast<BYTE>(
        std::clamp(Wh_GetIntSetting(L"HiddenBorderColor.Blue"), 0, 255));
    g_settings.hiddenBorderColor = RGB(borderRed, borderGreen, borderBlue);

    auto targetScopeSetting = WindhawkUtils::StringSetting::make(L"TargetScope");
    g_settings.targetScope = SettingEquals(targetScopeSetting, L"SingleWindow")
                                 ? TargetScope::SingleWindow
                                 : TargetScope::AllWindows;

    auto triggerModeSetting = WindhawkUtils::StringSetting::make(L"TriggerMode");
    if (SettingEquals(triggerModeSetting, L"HoverHotkey")) {
        g_settings.triggerMode = TriggerMode::HoverHotkey;
    } else if (SettingEquals(triggerModeSetting, L"Both")) {
        g_settings.triggerMode = TriggerMode::Both;
    } else {
        g_settings.triggerMode = TriggerMode::MiddleClick;
    }

    auto triggerModifierSetting =
        WindhawkUtils::StringSetting::make(L"TriggerModifier");
    if (SettingEquals(triggerModifierSetting, L"Ctrl")) {
        g_settings.triggerModifier = TriggerModifier::Ctrl;
    } else if (SettingEquals(triggerModifierSetting, L"Shift")) {
        g_settings.triggerModifier = TriggerModifier::Shift;
    } else if (SettingEquals(triggerModifierSetting, L"Alt")) {
        g_settings.triggerModifier = TriggerModifier::Alt;
    } else if (SettingEquals(triggerModifierSetting, L"Win")) {
        g_settings.triggerModifier = TriggerModifier::Win;
    } else {
        g_settings.triggerModifier = TriggerModifier::None;
    }

    auto hoverHotkeySetting = WindhawkUtils::StringSetting::make(L"HoverHotkey");
    if (SettingEquals(hoverHotkeySetting, L"F9")) {
        g_settings.hoverHotkey = HoverHotkey::F9;
    } else if (SettingEquals(hoverHotkeySetting, L"F10")) {
        g_settings.hoverHotkey = HoverHotkey::F10;
    } else if (SettingEquals(hoverHotkeySetting, L"F11")) {
        g_settings.hoverHotkey = HoverHotkey::F11;
    } else if (SettingEquals(hoverHotkeySetting, L"F12")) {
        g_settings.hoverHotkey = HoverHotkey::F12;
    } else if (SettingEquals(hoverHotkeySetting, L"Pause")) {
        g_settings.hoverHotkey = HoverHotkey::Pause;
    } else if (SettingEquals(hoverHotkeySetting, L"ScrollLock")) {
        g_settings.hoverHotkey = HoverHotkey::ScrollLock;
    } else {
        g_settings.hoverHotkey = HoverHotkey::F8;
    }

    Wh_Log(L"LoadSettings: showHiddenBorder=%d hiddenBorderColor=0x%08X targetScope=%d triggerMode=%d triggerModifier=%d hoverHotkey=%d",
        g_settings.showHiddenBorder ? 1 : 0, g_settings.hiddenBorderColor,
        static_cast<int>(g_settings.targetScope),
        static_cast<int>(g_settings.triggerMode),
        static_cast<int>(g_settings.triggerModifier),
        static_cast<int>(g_settings.hoverHotkey));
}

bool TriggerModeHasMouse() {
    return g_settings.triggerMode == TriggerMode::MiddleClick ||
           g_settings.triggerMode == TriggerMode::Both;
}

bool TriggerModeHasHotkey() {
    return g_settings.triggerMode == TriggerMode::HoverHotkey ||
           g_settings.triggerMode == TriggerMode::Both;
}

int GetConfiguredHotkeyVk() {
    switch (g_settings.hoverHotkey) {
        case HoverHotkey::F9:
            return VK_F9;
        case HoverHotkey::F10:
            return VK_F10;
        case HoverHotkey::F11:
            return VK_F11;
        case HoverHotkey::F12:
            return VK_F12;
        case HoverHotkey::Pause:
            return VK_PAUSE;
        case HoverHotkey::ScrollLock:
            return VK_SCROLL;
        case HoverHotkey::F8:
        default:
            return VK_F8;
    }
}

bool IsModifierSatisfied() {
    switch (g_settings.triggerModifier) {
        case TriggerModifier::Ctrl:
            return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        case TriggerModifier::Shift:
            return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        case TriggerModifier::Alt:
            return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        case TriggerModifier::Win:
            return ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) &
                    0x8000) != 0;
        case TriggerModifier::None:
        default:
            return true;
    }
}

UINT EnsureToggleMessageRegistered() {
    if (g_toggleMessage != 0) {
        return g_toggleMessage;
    }

    const UINT msg = RegisterWindowMessageW(kToggleMessageName);
    if (msg == 0) {
        Wh_Log(L"EnsureToggleMessageRegistered: RegisterWindowMessageW failed gle=%lu",
            GetLastError());
        return 0;
    }

    g_toggleMessage = msg;
    return msg;
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring GetWindowTextString(HWND hwnd) {
    const int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) {
        return L"";
    }

    std::wstring text(length + 1, L'\0');
    const int copied = GetWindowTextW(hwnd, &text[0], length + 1);
    if (copied <= 0) {
        return L"";
    }

    text.resize(copied);
    return text;
}

std::wstring GetModuleBaseNameForPid(DWORD pid) {
    std::wstring result;

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return result;
    }

    wchar_t path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        PCWSTR fileName = PathFindFileNameW(path);
        if (fileName && *fileName) {
            result.assign(fileName);
            const size_t dot = result.rfind(L'.');
            if (dot != std::wstring::npos) {
                result.resize(dot);
            }
        }
    }

    CloseHandle(process);
    return result;
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

bool IsTaskbarWindowClass(const wchar_t* className) {
    return className && (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
                         _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0);
}

bool IsShellOwnedWindowClass(const wchar_t* className) {
    if (!className || !*className) {
        return false;
    }

    static const wchar_t* const kShellClasses[] = {
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Progman",
        L"WorkerW",
        L"Windows.UI.Core.CoreWindow",
        L"XamlExplorerHostIslandWindow",
        L"MultitaskingViewFrame",
        L"ForegroundStaging",
        L"TaskListThumbnailWnd",
        L"Shell_InputSwitchTopLevelWindow",
        L"NarratorHelperWindow",
    };

    for (const wchar_t* candidate : kShellClasses) {
        if (_wcsicmp(className, candidate) == 0) {
            return true;
        }
    }
    return false;
}

bool IsTaskbarHostWindow(HWND hwnd) {
    wchar_t className[128];
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return IsTaskbarWindowClass(className);
}

bool IsPointOnTaskbar(POINT pt, HWND* taskbarRoot) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return false;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (root && IsTaskbarHostWindow(root)) {
        if (taskbarRoot) {
            *taskbarRoot = root;
        }
        return true;
    }

    // Taskbar flyouts (jump lists, group previews) live in their own top-level
    // windows owned by the taskbar, so also accept those.
    HWND owner = root ? GetWindow(root, GW_OWNER) : nullptr;
    if (owner && IsTaskbarHostWindow(owner)) {
        if (taskbarRoot) {
            *taskbarRoot = owner;
        }
        return true;
    }

    return false;
}

// The standard "would this window get a taskbar button" test, matching the
// alt-tab / taskbar rules closely enough for grouping.
bool IsTaskbarPresentWindow(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }

    if (GetAncestor(hwnd, GA_ROOTOWNER) != hwnd) {
        return false;
    }

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }

    if (IsWindowCloaked(hwnd)) {
        return false;
    }

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc) || (rc.right - rc.left) <= 0 ||
        (rc.bottom - rc.top) <= 0) {
        return false;
    }

    wchar_t className[128]{};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        IsShellOwnedWindowClass(className)) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Application User Model ID resolution.
//
// The shell's own app resolver is used so that the AppId reported for a window
// matches the AppId the taskbar exposes on its buttons (including the
// synthesized IDs it invents for plain unpackaged executables).
// ---------------------------------------------------------------------------

const CLSID CLSID_StartMenuCacheAndAppResolver = {
    0x660b90c8,
    0x73a9,
    0x4b58,
    {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

const IID IID_IAppResolver_7 = {
    0x46a6eeff,
    0x908e,
    0x4dc6,
    {0x92, 0xa6, 0x64, 0xbe, 0x91, 0x77, 0xb4, 0x1c}};

const IID IID_IAppResolver_8 = {
    0xde25675a,
    0x72de,
    0x44b4,
    {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

struct IAppResolver_7 : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut(void*, WCHAR**) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetAppIDForWindow(HWND, WCHAR**, void*, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetAppIDForProcess(DWORD, WCHAR**, void*, void*, void*) = 0;
};

struct IAppResolver_8 : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut(void*, WCHAR**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject(void*,
                                                               WCHAR**) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetAppIDForWindow(HWND, WCHAR**, void*, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetAppIDForProcess(DWORD, WCHAR**, void*, void*, void*) = 0;
};

[[clang::no_destroy]] ComPtr<IAppResolver_7> g_appResolver7;
[[clang::no_destroy]] ComPtr<IAppResolver_8> g_appResolver8;
bool g_appResolverInitialized = false;

void EnsureAppResolver() {
    if (g_appResolverInitialized) {
        return;
    }
    g_appResolverInitialized = true;

    HRESULT hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                                  CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                                  IID_IAppResolver_8,
                                  reinterpret_cast<void**>(
                                      g_appResolver8.GetAddressOf()));
    if (SUCCEEDED(hr) && g_appResolver8) {
        Wh_Log(L"EnsureAppResolver: using IAppResolver_8");
        return;
    }

    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_7,
                          reinterpret_cast<void**>(
                              g_appResolver7.GetAddressOf()));
    if (SUCCEEDED(hr) && g_appResolver7) {
        Wh_Log(L"EnsureAppResolver: using IAppResolver_7");
        return;
    }

    Wh_Log(L"EnsureAppResolver: no app resolver available hr=0x%08X", hr);
}

void ReleaseAppResolver() {
    g_appResolver7.Reset();
    g_appResolver8.Reset();
    g_appResolverInitialized = false;
}

std::wstring GetProcessAppUserModelId(DWORD pid) {
    std::wstring appId;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return appId;
    }

    UINT length = 0;
    LONG result = GetApplicationUserModelId(process, &length, nullptr);
    if (result == ERROR_INSUFFICIENT_BUFFER && length > 1) {
        std::wstring buffer(length, L'\0');
        result = GetApplicationUserModelId(process, &length, buffer.data());
        if (result == ERROR_SUCCESS) {
            if (length && buffer[length - 1] == L'\0') {
                --length;
            }
            buffer.resize(length);
            appId = std::move(buffer);
        }
    }

    CloseHandle(process);
    return appId;
}

// Must be called on the worker thread (COM apartment bound).
std::wstring GetWindowAppId(HWND hwnd) {
    EnsureAppResolver();

    WCHAR* rawAppId = nullptr;
    HRESULT hr = E_FAIL;
    if (g_appResolver8) {
        hr = g_appResolver8->GetAppIDForWindow(hwnd, &rawAppId, nullptr,
                                               nullptr, nullptr);
    } else if (g_appResolver7) {
        hr = g_appResolver7->GetAppIDForWindow(hwnd, &rawAppId, nullptr,
                                               nullptr, nullptr);
    }

    if (SUCCEEDED(hr) && rawAppId) {
        std::wstring appId(rawAppId);
        CoTaskMemFree(rawAppId);
        if (!appId.empty()) {
            return appId;
        }
    }

    // Packaged apps still report a usable AppId through the process even when
    // the resolver is unavailable.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid ? GetProcessAppUserModelId(pid) : std::wstring();
}

bool AppIdEquals(const std::wstring& left, const std::wstring& right) {
    return !left.empty() && !right.empty() &&
           _wcsicmp(left.c_str(), right.c_str()) == 0;
}

struct AppIdCollectContext {
    std::wstring appId;
    std::vector<HWND> windows;
};

BOOL CALLBACK EnumWindowsForAppId(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<AppIdCollectContext*>(lParam);
    if (!context || !IsTaskbarPresentWindow(hwnd)) {
        return TRUE;
    }

    if (AppIdEquals(GetWindowAppId(hwnd), context->appId)) {
        context->windows.push_back(hwnd);
    }
    return TRUE;
}

// EnumWindows enumerates in Z-order, so the returned list is top-most first.
std::vector<HWND> CollectWindowsForAppId(const std::wstring& appId) {
    AppIdCollectContext context;
    context.appId = appId;
    if (appId.empty()) {
        return context.windows;
    }

    EnumWindows(EnumWindowsForAppId, reinterpret_cast<LPARAM>(&context));
    return std::move(context.windows);
}

std::vector<HWND> CollectSiblingWindowsOf(HWND hwnd) {
    std::vector<HWND> windows;
    const std::wstring appId = GetWindowAppId(hwnd);
    if (!appId.empty()) {
        windows = CollectWindowsForAppId(appId);
    }

    if (std::find(windows.begin(), windows.end(), hwnd) == windows.end()) {
        windows.insert(windows.begin(), hwnd);
    }
    return windows;
}

// ---------------------------------------------------------------------------
// UI Automation helpers.
// ---------------------------------------------------------------------------

std::wstring GetElementString(
    IUIAutomationElement* element,
    HRESULT(STDMETHODCALLTYPE IUIAutomationElement::*getter)(BSTR*)) {
    if (!element) {
        return L"";
    }

    BSTR value = nullptr;
    std::wstring result;
    if (SUCCEEDED((element->*getter)(&value)) && value) {
        result.assign(value, SysStringLen(value));
        SysFreeString(value);
    }
    return result;
}

HWND TryGetDirectWindowFromElement(IUIAutomationElement* element,
                                   HWND taskbarRoot) {
    if (!element) {
        return nullptr;
    }

    UIA_HWND nativeHwnd = 0;
    if (FAILED(element->get_CurrentNativeWindowHandle(&nativeHwnd)) ||
        !nativeHwnd) {
        return nullptr;
    }

    HWND hwnd = reinterpret_cast<HWND>(nativeHwnd);
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root || root == taskbarRoot || IsTaskbarHostWindow(root)) {
        return nullptr;
    }

    return IsTaskbarPresentWindow(root) ? root : nullptr;
}

// Reads a value that follows a "<prefix>" marker in an automation id, e.g.
// "Appid: Microsoft.Windows.Explorer" or "Window: 0x10154".
bool TryReadAutomationIdValue(const std::wstring& automationId,
                              const wchar_t* prefix,
                              std::wstring* value) {
    const size_t prefixLength = wcslen(prefix);
    if (automationId.size() <= prefixLength ||
        _wcsnicmp(automationId.c_str(), prefix, prefixLength) != 0) {
        return false;
    }

    const wchar_t* rest = automationId.c_str() + prefixLength;
    while (iswspace(*rest)) {
        ++rest;
    }

    std::wstring result(rest);
    while (!result.empty() && iswspace(result.back())) {
        result.pop_back();
    }

    if (result.empty()) {
        return false;
    }

    *value = std::move(result);
    return true;
}

HWND TryGetWindowFromAutomationId(const std::wstring& automationId,
                                  HWND taskbarRoot) {
    std::wstring value;
    if (!TryReadAutomationIdValue(automationId, L"Window:", &value)) {
        return nullptr;
    }

    wchar_t* end = nullptr;
    const unsigned long long rawHandle = wcstoull(value.c_str(), &end, 0);
    if (!rawHandle || end == value.c_str() || (end && *end)) {
        return nullptr;
    }

    HWND hwnd = reinterpret_cast<HWND>(static_cast<ULONG_PTR>(rawHandle));
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root || root == taskbarRoot || IsTaskbarHostWindow(root)) {
        return nullptr;
    }

    return IsTaskbarPresentWindow(root) ? root : nullptr;
}

std::wstring NormalizeIdentityText(const std::wstring& value) {
    std::wstring normalized;
    normalized.reserve(value.size());
    bool pendingSpace = false;
    for (wchar_t ch : value) {
        if (iswalnum(ch)) {
            if (pendingSpace && !normalized.empty()) {
                normalized.push_back(L' ');
            }
            normalized.push_back(static_cast<wchar_t>(towlower(ch)));
            pendingSpace = false;
        } else {
            pendingSpace = true;
        }
    }
    return normalized;
}

// "File Explorer - 2 running windows pinned" -> "file explorer".
std::wstring GetTaskbarAppName(const std::wstring& accessibleName) {
    std::wstring name = accessibleName;
    const std::wstring lower = ToLower(name);
    const size_t runningSuffix = lower.rfind(L" - ");
    if (runningSuffix != std::wstring::npos) {
        const std::wstring suffix = lower.substr(runningSuffix + 3);
        if (suffix.find(L"running window") != std::wstring::npos ||
            suffix.find(L"pinned") != std::wstring::npos ||
            suffix.find(L"new notification") != std::wstring::npos) {
            name.resize(runningSuffix);
        }
    }
    return NormalizeIdentityText(name);
}

bool ContainsWholeIdentityPhrase(const std::wstring& text,
                                 const std::wstring& phrase) {
    size_t position = text.find(phrase);
    while (position != std::wstring::npos) {
        const bool startsAtBoundary = position == 0 || text[position - 1] == L' ';
        const size_t end = position + phrase.size();
        const bool endsAtBoundary = end == text.size() || text[end] == L' ';
        if (startsAtBoundary && endsAtBoundary) {
            return true;
        }
        position = text.find(phrase, position + 1);
    }
    return false;
}

struct NameMatchContext {
    std::wstring appName;
    HWND exeMatch = nullptr;
    HWND titleMatch = nullptr;
};

BOOL CALLBACK EnumWindowsForNameMatch(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<NameMatchContext*>(lParam);
    if (!context || !IsTaskbarPresentWindow(hwnd)) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return TRUE;
    }

    if (!context->exeMatch) {
        const std::wstring normalizedExe =
            NormalizeIdentityText(GetModuleBaseNameForPid(pid));
        if (!normalizedExe.empty() && normalizedExe == context->appName) {
            context->exeMatch = hwnd;
            return FALSE;
        }
    }

    if (!context->titleMatch) {
        const std::wstring normalizedTitle =
            NormalizeIdentityText(GetWindowTextString(hwnd));
        if (ContainsWholeIdentityPhrase(normalizedTitle, context->appName)) {
            context->titleMatch = hwnd;
        }
    }

    return TRUE;
}

// Last-resort matching when the taskbar button exposes no usable AppId: find a
// window whose executable or title matches the button label, then expand that
// window back into its full AppId group.
std::vector<HWND> CollectWindowsFromButtonName(const std::wstring& buttonName) {
    NameMatchContext context;
    context.appName = GetTaskbarAppName(buttonName);
    if (context.appName.empty()) {
        return {};
    }

    EnumWindows(EnumWindowsForNameMatch, reinterpret_cast<LPARAM>(&context));

    HWND seed = context.exeMatch ? context.exeMatch : context.titleMatch;
    if (!seed) {
        return {};
    }

    return CollectSiblingWindowsOf(seed);
}

std::wstring DescribeWindowForUser(HWND hwnd) {
    std::wstring title = GetWindowTextString(hwnd);
    if (!title.empty()) {
        return title;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    std::wstring exeBase = GetModuleBaseNameForPid(pid);
    return exeBase.empty() ? L"Application" : exeBase;
}

bool EnsureUiAutomation() {
    if (g_uia) {
        return true;
    }

    if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation8, nullptr,
                                   CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_uia)))) {
        return true;
    }

    g_uia.Reset();
    if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                   CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_uia)))) {
        return true;
    }

    g_uia.Reset();
    Wh_Log(L"EnsureUiAutomation: failed to create UI Automation");
    return false;
}

// Resolves the taskbar button under `pt` into every window it represents.
// The returned list is in Z-order, top-most first.
std::vector<HWND> ResolveTaskbarButtonTargets(POINT pt) {
    HWND taskbarRoot = nullptr;
    if (!IsPointOnTaskbar(pt, &taskbarRoot)) {
        Wh_Log(L"ResolveTaskbarButtonTargets: point (%ld,%ld) is not on taskbar",
            pt.x, pt.y);
        return {};
    }

    if (!EnsureUiAutomation()) {
        return {};
    }

    ComPtr<IUIAutomationElement> elementAtPoint;
    if (FAILED(g_uia->ElementFromPoint(pt, &elementAtPoint)) || !elementAtPoint) {
        Wh_Log(L"ResolveTaskbarButtonTargets: ElementFromPoint failed at (%ld,%ld)",
            pt.x, pt.y);
        return {};
    }

    ComPtr<IUIAutomationTreeWalker> controlWalker;
    if (FAILED(g_uia->get_ControlViewWalker(&controlWalker)) || !controlWalker) {
        Wh_Log(L"ResolveTaskbarButtonTargets: failed to get control-view walker");
        return {};
    }

    std::wstring taskbarButtonName;
    std::wstring xamlElementName;
    ComPtr<IUIAutomationElement> current = elementAtPoint;
    for (int depth = 0; current && depth < kMaxUiaWalkDepth; ++depth) {
        // A concrete window handle (group flyout thumbnail, or a button that
        // exposes its window directly) is always the most precise answer.
        if (HWND directWindow =
                TryGetDirectWindowFromElement(current.Get(), taskbarRoot)) {
            Wh_Log(L"ResolveTaskbarButtonTargets: native hwnd match hwnd=0x%p",
                directWindow);
            return {directWindow};
        }

        UiaButtonDescriptor descriptor;
        descriptor.name =
            GetElementString(current.Get(), &IUIAutomationElement::get_CurrentName);
        descriptor.automationId = GetElementString(
            current.Get(), &IUIAutomationElement::get_CurrentAutomationId);
        descriptor.className = GetElementString(
            current.Get(), &IUIAutomationElement::get_CurrentClassName);
        descriptor.frameworkId = GetElementString(
            current.Get(), &IUIAutomationElement::get_CurrentFrameworkId);

        if (HWND automationIdWindow = TryGetWindowFromAutomationId(
                descriptor.automationId, taskbarRoot)) {
            Wh_Log(L"ResolveTaskbarButtonTargets: automationId hwnd match hwnd=0x%p id='%s'",
                automationIdWindow, descriptor.automationId.c_str());
            return {automationIdWindow};
        }

        // The Windows 11 taskbar tags each app button with its AppId. This is
        // the path that makes grouped buttons work.
        std::wstring appId;
        if (TryReadAutomationIdValue(descriptor.automationId, L"Appid:",
                                     &appId)) {
            std::vector<HWND> windows = CollectWindowsForAppId(appId);
            Wh_Log(L"ResolveTaskbarButtonTargets: appId='%s' matched %zu window(s)",
                appId.c_str(), windows.size());
            if (!windows.empty()) {
                return windows;
            }
        }

        if (!descriptor.name.empty()) {
            const bool looksLikeTaskbarButton =
                ToLower(descriptor.className).find(L"taskbar") !=
                    std::wstring::npos ||
                ToLower(descriptor.automationId).find(L"taskbar") !=
                    std::wstring::npos;
            if (looksLikeTaskbarButton && taskbarButtonName.empty()) {
                taskbarButtonName = descriptor.name;
            } else if (xamlElementName.empty() &&
                       _wcsicmp(descriptor.frameworkId.c_str(), L"XAML") == 0) {
                xamlElementName = descriptor.name;
            }
        }

        ComPtr<IUIAutomationElement> parent;
        if (FAILED(controlWalker->GetParentElement(current.Get(), &parent)) ||
            !parent) {
            break;
        }
        current = parent;
    }

    const std::wstring& buttonName =
        !taskbarButtonName.empty() ? taskbarButtonName : xamlElementName;
    std::vector<HWND> windows = CollectWindowsFromButtonName(buttonName);
    if (!windows.empty()) {
        Wh_Log(L"ResolveTaskbarButtonTargets: name fallback '%s' matched %zu window(s)",
            buttonName.c_str(), windows.size());
        return windows;
    }

    Wh_Log(L"ResolveTaskbarButtonTargets: no target found for button name='%s'",
        buttonName.c_str());
    return {};
}

// ---------------------------------------------------------------------------
// Display affinity.
// ---------------------------------------------------------------------------

bool IsWindowManagedByMod(HWND hwnd) {
    std::scoped_lock lock(g_managedWindowsMutex);
    return g_managedWindows.find(hwnd) != g_managedWindows.end();
}

void RememberManagedWindow(HWND hwnd, DWORD originalAffinity) {
    std::scoped_lock lock(g_managedWindowsMutex);
    g_managedWindows[hwnd].originalAffinity = originalAffinity;
}

void SetManagedWindowBorderApplied(HWND hwnd, bool applied) {
    std::scoped_lock lock(g_managedWindowsMutex);
    auto it = g_managedWindows.find(hwnd);
    if (it != g_managedWindows.end()) {
        it->second.borderApplied = applied;
    }
}

bool TakeManagedWindowState(HWND hwnd, ManagedWindowState* state) {
    std::scoped_lock lock(g_managedWindowsMutex);
    auto it = g_managedWindows.find(hwnd);
    if (it == g_managedWindows.end()) {
        return false;
    }

    if (state) {
        *state = it->second;
    }
    g_managedWindows.erase(it);
    return true;
}

bool ApplyHiddenBorderIndicator(HWND hwnd, bool hidden) {
    const COLORREF color = hidden ? g_settings.hiddenBorderColor : kDwmDefaultColor;
    const HRESULT hr =
        DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
    if (FAILED(hr)) {
        Wh_Log(L"ApplyHiddenBorderIndicator: failed hwnd=0x%p hidden=%d hr=0x%08X",
            hwnd, hidden ? 1 : 0, hr);
        return false;
    }
    return true;
}

bool IsWindowCurrentlyHiddenFromCapture(HWND hwnd) {
    DWORD affinity = WDA_NONE;
    if (!GetWindowDisplayAffinity(hwnd, &affinity)) {
        return false;
    }

    return affinity == kWdaExcludeFromCapture || affinity == kWdaMonitor;
}

// Applies the affinity change to a window. Works both for windows of the
// current process and, when permitted by UIPI, for windows of other processes.
ToggleResult ApplyDisplayAffinityForWindow(HWND hwnd, bool hide) {
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!IsWindow(hwnd)) {
        return ToggleResult::Failed;
    }

    DWORD currentAffinity = WDA_NONE;
    if (!GetWindowDisplayAffinity(hwnd, &currentAffinity)) {
        currentAffinity = WDA_NONE;
    }

    if (!hide) {
        ManagedWindowState state;
        const bool wasManaged = TakeManagedWindowState(hwnd, &state);
        const DWORD desiredAffinity =
            wasManaged ? state.originalAffinity : WDA_NONE;

        if (!SetWindowDisplayAffinity(hwnd, desiredAffinity)) {
            const DWORD gle = GetLastError();
            Wh_Log(L"ApplyDisplayAffinityForWindow: unhide failed hwnd=0x%p gle=%lu",
                hwnd, gle);
            if (wasManaged) {
                RememberManagedWindow(hwnd, state.originalAffinity);
                SetManagedWindowBorderApplied(hwnd, state.borderApplied);
            }
            SetLastError(gle);
            return ToggleResult::Failed;
        }

        if (!wasManaged || state.borderApplied) {
            ApplyHiddenBorderIndicator(hwnd, false);
        }
        Wh_Log(L"ApplyDisplayAffinityForWindow: unhidden hwnd=0x%p restored=0x%08X",
            hwnd, desiredAffinity);
        return ToggleResult::Unhidden;
    }

    const bool alreadyManaged = IsWindowManagedByMod(hwnd);
    if (!alreadyManaged) {
        // Never remember an exclusion value as the "original" state, otherwise
        // unhiding a window that something else already hid would be a no-op.
        RememberManagedWindow(hwnd,
                              (currentAffinity == kWdaExcludeFromCapture ||
                               currentAffinity == kWdaMonitor)
                                  ? WDA_NONE
                                  : currentAffinity);
    }

    ToggleResult result = ToggleResult::Failed;
    if (SetWindowDisplayAffinity(hwnd, kWdaExcludeFromCapture)) {
        result = ToggleResult::Hidden;
    } else {
        const DWORD gle = GetLastError();
        Wh_Log(L"ApplyDisplayAffinityForWindow: WDA_EXCLUDEFROMCAPTURE failed hwnd=0x%p gle=%lu",
            hwnd, gle);
        // Older compositors and some layered windows only accept WDA_MONITOR.
        if (SetWindowDisplayAffinity(hwnd, kWdaMonitor)) {
            result = ToggleResult::HiddenCompatibility;
        } else {
            Wh_Log(L"ApplyDisplayAffinityForWindow: WDA_MONITOR fallback failed hwnd=0x%p gle=%lu",
                hwnd, GetLastError());
            if (!alreadyManaged) {
                TakeManagedWindowState(hwnd, nullptr);
            }
            SetLastError(gle);
            return ToggleResult::Failed;
        }
    }

    if (g_settings.showHiddenBorder && ApplyHiddenBorderIndicator(hwnd, true)) {
        SetManagedWindowBorderApplied(hwnd, true);
    }

    Wh_Log(L"ApplyDisplayAffinityForWindow: hidden hwnd=0x%p compatibility=%d",
        hwnd, result == ToggleResult::HiddenCompatibility ? 1 : 0);
    return result;
}

void RestoreAllManagedWindows() {
    std::vector<HWND> windows;
    {
        std::scoped_lock lock(g_managedWindowsMutex);
        windows.reserve(g_managedWindows.size());
        for (const auto& entry : g_managedWindows) {
            windows.push_back(entry.first);
        }
    }

    Wh_Log(L"RestoreAllManagedWindows: restoring %zu window(s) pid=%lu",
        windows.size(), GetCurrentProcessId());
    for (HWND hwnd : windows) {
        ApplyDisplayAffinityForWindow(hwnd, false);
    }
}

// ---------------------------------------------------------------------------
// Cross-process plumbing.
// ---------------------------------------------------------------------------

std::wstring GetReceiverWindowName(DWORD pid) {
    wchar_t name[96];
    swprintf_s(name, L"Windhawk.HideFromScreenCapture.Receiver.%lu", pid);
    return name;
}

LRESULT CALLBACK ReceiverWindowProc(HWND hwnd,
                                    UINT msg,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    if (g_toggleMessage && msg == g_toggleMessage) {
        if (g_unloading.load()) {
            return static_cast<LRESULT>(ToggleResult::Failed);
        }

        HWND targetWindow = reinterpret_cast<HWND>(wParam);
        DWORD targetPid = 0;
        GetWindowThreadProcessId(targetWindow, &targetPid);
        if (targetPid != GetCurrentProcessId()) {
            return static_cast<LRESULT>(ToggleResult::Failed);
        }

        const auto action = static_cast<WindowAction>(lParam);
        if (action == WindowAction::Query) {
            const bool hidden = IsWindowManagedByMod(targetWindow) ||
                                IsWindowCurrentlyHiddenFromCapture(targetWindow);
            return static_cast<LRESULT>(hidden ? ToggleResult::Hidden
                                               : ToggleResult::Unhidden);
        }

        return static_cast<LRESULT>(ApplyDisplayAffinityForWindow(
            targetWindow, action == WindowAction::Hide));
    }

    if (msg == kStopReceiverMessage) {
        DestroyWindow(hwnd);
        return 0;
    }

    if (msg == WM_DESTROY) {
        if (g_receiverWindow.load() == hwnd) {
            g_receiverWindow.store(nullptr);
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI ReceiverThreadMain(LPVOID) {
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = ReceiverWindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = kReceiverWindowClassName;

    ATOM classAtom = RegisterClassW(&windowClass);
    if (!classAtom) {
        Wh_Log(L"ReceiverThreadMain: RegisterClassW failed gle=%lu", GetLastError());
        SetEvent(g_receiverReadyEvent);
        return 0;
    }

    const std::wstring windowName = GetReceiverWindowName(GetCurrentProcessId());
    HWND receiverWindow =
        CreateWindowExW(0, kReceiverWindowClassName, windowName.c_str(), 0, 0, 0,
                        0, 0, HWND_MESSAGE, nullptr, instance, nullptr);
    g_receiverWindow.store(receiverWindow);
    if (!receiverWindow) {
        Wh_Log(L"ReceiverThreadMain: CreateWindowExW failed gle=%lu", GetLastError());
        UnregisterClassW(kReceiverWindowClassName, instance);
        SetEvent(g_receiverReadyEvent);
        return 0;
    }

    if (!ChangeWindowMessageFilterEx(receiverWindow, g_toggleMessage, MSGFLT_ALLOW,
                                     nullptr)) {
        Wh_Log(L"ReceiverThreadMain: ChangeWindowMessageFilterEx failed msg=0x%X gle=%lu",
            g_toggleMessage, GetLastError());
    }

    SetEvent(g_receiverReadyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (receiverWindow && IsWindow(receiverWindow)) {
        DestroyWindow(receiverWindow);
    }
    g_receiverWindow.store(nullptr);
    UnregisterClassW(kReceiverWindowClassName, instance);
    return 0;
}

bool StartReceiverThread() {
    g_receiverReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_receiverReadyEvent) {
        Wh_Log(L"StartReceiverThread: CreateEventW failed gle=%lu", GetLastError());
        return false;
    }

    g_receiverThread =
        CreateThread(nullptr, 0, ReceiverThreadMain, nullptr, 0, &g_receiverThreadId);
    if (!g_receiverThread) {
        Wh_Log(L"StartReceiverThread: CreateThread failed gle=%lu", GetLastError());
        CloseHandle(g_receiverReadyEvent);
        g_receiverReadyEvent = nullptr;
        return false;
    }

    WaitForSingleObject(g_receiverReadyEvent, INFINITE);
    CloseHandle(g_receiverReadyEvent);
    g_receiverReadyEvent = nullptr;
    return g_receiverWindow.load() != nullptr;
}

void StopReceiverThread() {
    HWND receiverWindow = g_receiverWindow.load();
    if (receiverWindow) {
        PostMessageW(receiverWindow, kStopReceiverMessage, 0, 0);
    } else if (g_receiverThreadId) {
        PostThreadMessageW(g_receiverThreadId, WM_QUIT, 0, 0);
    }

    if (g_receiverThread) {
        WaitForSingleObject(g_receiverThread, INFINITE);
        CloseHandle(g_receiverThread);
        g_receiverThread = nullptr;
    }
    g_receiverThreadId = 0;
    g_receiverWindow.store(nullptr);
}

HWND FindReceiverForWindow(HWND hwnd) {
    if (!EnsureToggleMessageRegistered()) {
        return nullptr;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return nullptr;
    }

    const std::wstring receiverName = GetReceiverWindowName(pid);
    return FindWindowExW(HWND_MESSAGE, nullptr, kReceiverWindowClassName,
                         receiverName.c_str());
}

bool SendActionToWindow(HWND hwnd, WindowAction action, ToggleResult* resultOut) {
    HWND receiver = FindReceiverForWindow(hwnd);
    if (!receiver) {
        return false;
    }

    ULONG_PTR result = 0;
    if (!SendMessageTimeoutW(receiver, g_toggleMessage,
                             reinterpret_cast<WPARAM>(hwnd),
                             static_cast<LPARAM>(action),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, kToggleSendTimeoutMs,
                             &result)) {
        Wh_Log(L"SendActionToWindow: send failed hwnd=0x%p gle=%lu", hwnd,
            GetLastError());
        return false;
    }

    if (static_cast<ToggleResult>(result) == ToggleResult::Failed &&
        action != WindowAction::Query) {
        return false;
    }

    if (resultOut) {
        *resultOut = static_cast<ToggleResult>(result);
    }
    return true;
}

// True when the window is currently hidden from capture. Prefers the target
// process's own view of the state, falling back to a direct read.
bool IsTargetWindowHidden(HWND hwnd) {
    ToggleResult queried = ToggleResult::Failed;
    if (SendActionToWindow(hwnd, WindowAction::Query, &queried) &&
        queried != ToggleResult::Failed) {
        return queried == ToggleResult::Hidden;
    }

    return IsWindowManagedByMod(hwnd) || IsWindowCurrentlyHiddenFromCapture(hwnd);
}

// Applies the requested state to a window. The in-process receiver is
// preferred because it always has the rights to change its own windows; the
// direct call covers processes Windhawk could not inject into.
ToggleResult ApplyStateToTargetWindow(HWND hwnd, bool hide) {
    ToggleResult result = ToggleResult::Failed;
    const WindowAction action = hide ? WindowAction::Hide : WindowAction::Unhide;
    if (SendActionToWindow(hwnd, action, &result) &&
        result != ToggleResult::Failed) {
        return result;
    }

    Wh_Log(L"ApplyStateToTargetWindow: falling back to direct call hwnd=0x%p hide=%d",
        hwnd, hide ? 1 : 0);
    return ApplyDisplayAffinityForWindow(hwnd, hide);
}

bool ToggleWindowsFromTaskbarPoint(POINT pt) {
    std::vector<HWND> targets = ResolveTaskbarButtonTargets(pt);
    if (targets.empty()) {
        return false;
    }

    if (g_settings.targetScope == TargetScope::AllWindows && targets.size() == 1) {
        // A precise single-window hit still toggles the whole app group when
        // the user asked for that scope.
        targets = CollectSiblingWindowsOf(targets.front());
    } else if (g_settings.targetScope == TargetScope::SingleWindow) {
        targets.resize(1);
    }

    // Decide once for the whole group so a mixed group converges to one state.
    bool anyHidden = false;
    for (HWND hwnd : targets) {
        if (IsTargetWindowHidden(hwnd)) {
            anyHidden = true;
            break;
        }
    }

    const bool hide = !anyHidden;
    size_t succeeded = 0;
    for (HWND hwnd : targets) {
        if (ApplyStateToTargetWindow(hwnd, hide) != ToggleResult::Failed) {
            ++succeeded;
        } else {
            Wh_Log(L"ToggleWindowsFromTaskbarPoint: failed hwnd=0x%p label='%s'",
                hwnd, DescribeWindowForUser(hwnd).c_str());
        }
    }

    Wh_Log(L"ToggleWindowsFromTaskbarPoint: hide=%d applied %zu/%zu window(s)",
        hide ? 1 : 0, succeeded, targets.size());
    return true;
}

// ---------------------------------------------------------------------------
// Explorer-side input hooks.
// ---------------------------------------------------------------------------

bool PostToggleRequest(POINT pt) {
    return g_explorerWorkerThreadId != 0 &&
           PostThreadMessageW(g_explorerWorkerThreadId, kExplorerToggleMessage,
                              static_cast<WPARAM>(pt.x),
                              static_cast<LPARAM>(pt.y)) != FALSE;
}

LRESULT CALLBACK ExplorerMouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0 || g_unloading.load()) {
        return CallNextHookEx(g_explorerMouseHook, code, wParam, lParam);
    }

    auto* mouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    if (!mouse) {
        return CallNextHookEx(g_explorerMouseHook, code, wParam, lParam);
    }

    if (wParam == WM_MBUTTONUP) {
        const DWORD swallowStart = g_swallowStartTick.exchange(0);
        if (swallowStart && GetTickCount() - swallowStart <= kSwallowWindowMs) {
            return 1;
        }
        return CallNextHookEx(g_explorerMouseHook, code, wParam, lParam);
    }

    if (wParam != WM_MBUTTONDOWN || !TriggerModeHasMouse() ||
        !IsModifierSatisfied() || !IsPointOnTaskbar(mouse->pt, nullptr)) {
        return CallNextHookEx(g_explorerMouseHook, code, wParam, lParam);
    }

    if (!PostToggleRequest(mouse->pt)) {
        return CallNextHookEx(g_explorerMouseHook, code, wParam, lParam);
    }

    g_swallowStartTick.store(GetTickCount());
    return 1;
}

LRESULT CALLBACK ExplorerKeyboardHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0 || g_unloading.load() || !TriggerModeHasHotkey()) {
        return CallNextHookEx(g_explorerKeyboardHook, code, wParam, lParam);
    }

    auto* keyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (!keyboard || static_cast<int>(keyboard->vkCode) != GetConfiguredHotkeyVk()) {
        return CallNextHookEx(g_explorerKeyboardHook, code, wParam, lParam);
    }

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        g_hotkeyDown.store(false);
        if (g_hotkeySwallowed.exchange(false)) {
            return 1;
        }
        return CallNextHookEx(g_explorerKeyboardHook, code, wParam, lParam);
    }

    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) {
        return CallNextHookEx(g_explorerKeyboardHook, code, wParam, lParam);
    }

    if (g_hotkeyDown.exchange(true)) {
        // Auto-repeat: keep swallowing if the initial press was swallowed.
        if (g_hotkeySwallowed.load()) {
            return 1;
        }
        return CallNextHookEx(g_explorerKeyboardHook, code, wParam, lParam);
    }

    POINT pt{};
    if (!GetCursorPos(&pt) || !IsPointOnTaskbar(pt, nullptr) ||
        !PostToggleRequest(pt)) {
        return CallNextHookEx(g_explorerKeyboardHook, code, wParam, lParam);
    }

    g_hotkeySwallowed.store(true);
    return 1;
}

void EnsureExplorerHooksInstalled(HMODULE thisModule) {
    if (!TriggerModeHasMouse()) {
        if (g_explorerMouseHook) {
            UnhookWindowsHookEx(g_explorerMouseHook);
            g_explorerMouseHook = nullptr;
        }
    } else if (!g_explorerMouseHook) {
        g_explorerMouseHook =
            SetWindowsHookExW(WH_MOUSE_LL, ExplorerMouseHookProc, thisModule, 0);
        if (!g_explorerMouseHook) {
            Wh_Log(L"EnsureExplorerHooksInstalled: mouse hook failed gle=%lu",
                GetLastError());
        }
    }

    if (!TriggerModeHasHotkey()) {
        if (g_explorerKeyboardHook) {
            UnhookWindowsHookEx(g_explorerKeyboardHook);
            g_explorerKeyboardHook = nullptr;
        }
        return;
    }

    if (!g_explorerKeyboardHook) {
        g_explorerKeyboardHook = SetWindowsHookExW(
            WH_KEYBOARD_LL, ExplorerKeyboardHookProc, thisModule, 0);
        if (!g_explorerKeyboardHook) {
            Wh_Log(L"EnsureExplorerHooksInstalled: keyboard hook failed gle=%lu",
                GetLastError());
        }
    }
}

DWORD WINAPI ExplorerWorkerThreadMain(LPVOID) {
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitCo = SUCCEEDED(hr);

    // Force creation of the thread message queue before hooks can post work.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_explorerWorkerReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != kExplorerToggleMessage || g_unloading.load()) {
            continue;
        }

        POINT pt{static_cast<LONG>(msg.wParam), static_cast<LONG>(msg.lParam)};
        ToggleWindowsFromTaskbarPoint(pt);
    }

    ReleaseAppResolver();
    g_uia.Reset();
    if (uninitCo) {
        CoUninitialize();
    }
    return 0;
}

DWORD WINAPI ExplorerHookThreadMain(LPVOID) {
    EnsureToggleMessageRegistered();

    HMODULE thisModule = reinterpret_cast<HMODULE>(&__ImageBase);
    const UINT_PTR retryTimer =
        SetTimer(nullptr, 0, kExplorerHookRetryIntervalMs, nullptr);
    EnsureExplorerHooksInstalled(thisModule);

    MSG msg;
    while (!g_unloading.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (retryTimer && msg.message == WM_TIMER && msg.wParam == retryTimer) {
            EnsureExplorerHooksInstalled(thisModule);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (retryTimer) {
        KillTimer(nullptr, retryTimer);
    }

    if (g_explorerMouseHook) {
        UnhookWindowsHookEx(g_explorerMouseHook);
        g_explorerMouseHook = nullptr;
    }

    if (g_explorerKeyboardHook) {
        UnhookWindowsHookEx(g_explorerKeyboardHook);
        g_explorerKeyboardHook = nullptr;
    }

    return 0;
}

bool IsCurrentProcessExplorer() {
    wchar_t modulePath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath))) {
        return false;
    }

    const wchar_t* fileName = PathFindFileNameW(modulePath);
    return fileName && _wcsicmp(fileName, L"explorer.exe") == 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_unloading.store(false);
    g_processIsExplorer.store(IsCurrentProcessExplorer());
    Wh_Log(L"Wh_ModInit: pid=%lu explorer=%d", GetCurrentProcessId(),
        g_processIsExplorer.load() ? 1 : 0);

    if (EnsureToggleMessageRegistered() && !StartReceiverThread()) {
        Wh_Log(L"Wh_ModInit: receiver startup failed pid=%lu", GetCurrentProcessId());
    }

    if (g_processIsExplorer.load()) {
        g_explorerWorkerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (g_explorerWorkerReadyEvent) {
            g_explorerWorkerThread =
                CreateThread(nullptr, 0, ExplorerWorkerThreadMain, nullptr, 0,
                             &g_explorerWorkerThreadId);
        }

        if (g_explorerWorkerThread) {
            WaitForSingleObject(g_explorerWorkerReadyEvent, INFINITE);
            g_explorerHookThread = CreateThread(
                nullptr, 0, ExplorerHookThreadMain, nullptr, 0, &g_explorerHookThreadId);
        } else {
            Wh_Log(L"Wh_ModInit: explorer worker thread startup failed gle=%lu",
                GetLastError());
        }

        if (g_explorerWorkerReadyEvent) {
            CloseHandle(g_explorerWorkerReadyEvent);
            g_explorerWorkerReadyEvent = nullptr;
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    g_unloading.store(true);
    Wh_Log(L"Wh_ModUninit: pid=%lu explorer=%d", GetCurrentProcessId(),
        g_processIsExplorer.load() ? 1 : 0);

    if (g_explorerHookThreadId) {
        PostThreadMessageW(g_explorerHookThreadId, WM_QUIT, 0, 0);
    }

    if (g_explorerHookThread) {
        WaitForSingleObject(g_explorerHookThread, INFINITE);
        CloseHandle(g_explorerHookThread);
        g_explorerHookThread = nullptr;
        g_explorerHookThreadId = 0;
    }

    if (g_explorerWorkerThreadId) {
        PostThreadMessageW(g_explorerWorkerThreadId, WM_QUIT, 0, 0);
    }

    if (g_explorerWorkerThread) {
        WaitForSingleObject(g_explorerWorkerThread, INFINITE);
        CloseHandle(g_explorerWorkerThread);
        g_explorerWorkerThread = nullptr;
    }
    g_explorerWorkerThreadId = 0;

    RestoreAllManagedWindows();
    StopReceiverThread();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    if (bReload) {
        *bReload = TRUE;
    }

    Wh_Log(L"Wh_ModSettingsChanged: reloading");
    return TRUE;
}
