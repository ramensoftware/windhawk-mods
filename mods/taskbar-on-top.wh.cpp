// ==WindhawkMod==
// @id              taskbar-on-top
// @name            Taskbar on top for Windows 11
// @description     Moves the Windows 11 taskbar to the top of the screen
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar on top for Windows 11

Moves the Windows 11 taskbar to the top of the screen.

## Known limitations

* With the non-native taskbar on top, the Action Center (Win+A) stays on the
  bottom. For now, you can use [this alternative
  solution](https://github.com/ramensoftware/windhawk-mods/issues/1053#issuecomment-2405461863).

![Screenshot](https://i.imgur.com/LqBwGVn.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarLocation: top
  $name: Taskbar location
  $options:
  - top: Top
  - bottom: Bottom
- taskbarLocationSecondary: sameAsPrimary
  $name: Taskbar location on secondary monitors
  $description: >-
    Left and right are only supported with the native taskbar, otherwise the
    taskbar stays on the bottom.
  $options:
  - sameAsPrimary: Same as on primary monitor
  - top: Top
  - bottom: Bottom
  - left: Left
  - right: Right
- runningIndicatorsOnTop: false
  $name: Running indicators on top
  $description: Show running indicators above the taskbar icons.
- startMenuAnimationAdjust: false
  $name: Adjust Start menu animation
  $description: >-
    Adjust the Start menu opening animation to match the taskbar on top
    position. This option doesn't work with the redesigned Start menu, and might
    not work with the Phone Link sidebar and with some Start Menu Styler themes.
- useNativeTaskbar: true
  $name: Use the native taskbar when possible
  $description: >-
    Newer Windows 11 builds include a native option to show the taskbar on top.
    If disabled, or if taskbar auto-hide is enabled, the mod's own
    implementation is used instead, and the taskbar position in Windows settings
    should be kept at Bottom.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <roapi.h>
#include <shellapi.h>
#include <windowsx.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <unordered_set>
#include <vector>

#ifdef _M_ARM64
#include <regex>
#endif

using namespace winrt::Windows::UI::Xaml;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

enum class TaskbarLocation {
    top,
    bottom,
    left,
    right,
};

struct {
    TaskbarLocation taskbarLocation;
    TaskbarLocation taskbarLocationSecondary;
    bool runningIndicatorsOnTop;
    bool startMenuAnimationAdjust;
    bool useNativeTaskbar;
} g_settings;

enum class Target {
    Explorer,
    StartMenuExperienceHost,
    ShellExperienceHost,
    ShellHost,  // From Win11 24H2.
};

Target g_target;

// Builds with the native taskbar on top lay the taskbar out for the location
// they read from the registry. The mod overrides that location instead of
// moving a bottom taskbar.
bool g_hasNativeTaskbarOnTop;

// Defines data shared by all instances of the library, even across processes
// and sessions.
#define SHARED_SECTION __attribute__((section(".shared")))
asm(".section .shared,\"dws\"\n");

enum class NativeTaskbarOnTop : char {
    unknown,
    unavailable,
    available,
};

// Detected in explorer.exe and shared with the other processes of its session,
// since it depends on per-user settings. Indexed by session id.
volatile NativeTaskbarOnTop g_nativeTaskbarOnTop[1024] SHARED_SECTION = {};

// Null if the session id is out of range, the state is then unknown.
volatile NativeTaskbarOnTop* g_sessionNativeTaskbarOnTop;

NativeTaskbarOnTop GetNativeTaskbarOnTop() {
    if (g_target == Target::Explorer) {
        return g_hasNativeTaskbarOnTop ? NativeTaskbarOnTop::available
                                       : NativeTaskbarOnTop::unavailable;
    }

    if (!g_sessionNativeTaskbarOnTop) {
        return NativeTaskbarOnTop::unknown;
    }

    return *g_sessionNativeTaskbarOnTop;
}

// Auto-hide trigger height in pixels. You must approach within this many pixels
// of the monitor top to show the taskbar when hidden.
constexpr int kAutoHideTriggerHeight = 2;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

bool g_inCTaskListThumbnailWnd_DisplayUI;
bool g_inCTaskListThumbnailWnd_LayoutThumbnails;
bool g_inOverflowFlyoutModel_Show;
thread_local bool g_inMenuFlyout_ShowAt;
constexpr WCHAR kMenuFlyoutPopupPropName[] =
    L"MenuFlyoutPopup_Windhawk_" WH_MOD_ID;
int g_lastTaskbarAlignment;

std::atomic<DWORD> g_UpdateFlyoutPosition_threadId;
void* g_UpdateFlyoutPosition_pThis;

winrt::Windows::Foundation::Size g_lastFlyoutPositionSize;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_elementLoadedAutoRevokerList;

WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);

// Available since Windows 10 version 1607, missing in older MinGW headers.
using GetThreadDescription_t =
    WINBASEAPI HRESULT(WINAPI*)(HANDLE hThread, PWSTR* ppszThreadDescription);
GetThreadDescription_t pGetThreadDescription;

// Private API for window band (z-order band).
// https://blog.adeltax.com/window-z-order-in-windows-10/
using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand;

constexpr DWORD ZBID_SYSTEM_TOOLS = 16;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

bool IsMainModuleVersionAtLeast(WORD major, WORD minor, WORD build, WORD qfe) {
    static VS_FIXEDFILEINFO* fixedFileInfo =
        GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return false;
    }

    WORD moduleMajor = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleMinor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleBuild = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD moduleQfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    if (moduleMajor != major) {
        return moduleMajor > major;
    }

    if (moduleMinor != minor) {
        return moduleMinor > minor;
    }

    if (moduleBuild != build) {
        return moduleBuild > build;
    }

    return moduleQfe >= qfe;
}

bool GetMonitorRect(HMONITOR monitor, RECT* rc) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    return GetMonitorInfo(monitor, &monitorInfo) &&
           CopyRect(rc, &monitorInfo.rcMonitor);
}

bool IsTaskbarAutoHideEnabled() {
    APPBARDATA abd = {sizeof(APPBARDATA)};
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    return (state & ABS_AUTOHIDE) != 0;
}

// Reads the state saved by the taskbar, available before the taskbar is
// created. The state is a DWORD at offset 8.
bool IsStoredTaskbarAutoHideEnabled() {
    BYTE settings[256];
    DWORD size = sizeof(settings);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\StuckRects3)",
            L"Settings", RRF_RT_REG_BINARY, nullptr, settings,
            &size) != ERROR_SUCCESS ||
        size < 12) {
        return false;
    }

    DWORD state = *(DWORD*)(settings + 8);
    return (state & ABS_AUTOHIDE) != 0;
}

HWND FindCurrentProcessWindow(PCWSTR className) {
    struct ENUM_WINDOWS_PARAM {
        PCWSTR className;
        HWND hWnd;
    };

    ENUM_WINDOWS_PARAM param = {className, nullptr};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& param = *reinterpret_cast<ENUM_WINDOWS_PARAM*>(lParam);
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, param.className) == 0) {
                param.hWnd = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));

    return param.hWnd;
}

HWND FindCurrentProcessTaskbarWnd() {
    return FindCurrentProcessWindow(L"Shell_TrayWnd");
}

static const UINT g_getTaskbarRectRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_GetTaskbarRect_" WH_MOD_ID);

bool GetTaskbarRectForMonitor(HMONITOR monitor, RECT* rect) {
    SetRectEmpty(rect);

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    SendMessage(hTaskbarWnd, g_getTaskbarRectRegisteredMsg, (WPARAM)monitor,
                (LPARAM)rect);
    return true;
}

bool GetMonitorWorkAreaWithoutTaskbar(HMONITOR monitor, RECT* rc) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        SetRectEmpty(rc);
        return false;
    }

    RECT taskbarRect;
    if (!GetTaskbarRectForMonitor(monitor, &taskbarRect)) {
        return CopyRect(rc, &monitorInfo.rcWork);
    }

    return SubtractRect(rc, &monitorInfo.rcWork, &taskbarRect);
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        if (parent.Name() == name) {
            return true;
        }
    }
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        if (winrt::get_class_name(parent) == className) {
            return true;
        }
    }
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Left and right are only supported with the native taskbar on top.
TaskbarLocation GetTaskbarLocationSecondary() {
    if ((g_settings.taskbarLocationSecondary == TaskbarLocation::left ||
         g_settings.taskbarLocationSecondary == TaskbarLocation::right) &&
        GetNativeTaskbarOnTop() != NativeTaskbarOnTop::available) {
        return TaskbarLocation::bottom;
    }

    return g_settings.taskbarLocationSecondary;
}

TaskbarLocation GetTaskbarLocationForMonitor(HMONITOR monitor) {
    if (g_unloading) {
        return TaskbarLocation::bottom;
    }

    TaskbarLocation taskbarLocationSecondary = GetTaskbarLocationSecondary();
    if (g_settings.taskbarLocation == taskbarLocationSecondary) {
        return g_settings.taskbarLocation;
    }

    HMONITOR primaryMonitor =
        MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);

    return monitor == primaryMonitor ? g_settings.taskbarLocation
                                     : taskbarLocationSecondary;
}

using TrayUI_GetStuckRectForMonitor_t = bool(WINAPI*)(void* pThis,
                                                      HMONITOR hMonitor,
                                                      RECT* rect);
TrayUI_GetStuckRectForMonitor_t TrayUI_GetStuckRectForMonitor_Original;

using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis,
                                                    void* param1,
                                                    void* param2,
                                                    void* param3,
                                                    void* param4);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* param1,
                                             void* param2,
                                             void* param3,
                                             void* param4) {
    Wh_Log(L">");

    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);

    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}

using TrayUI_GetDockedRect_t = DWORD(WINAPI*)(void* pThis,
                                              RECT* rect,
                                              BOOL param2);
TrayUI_GetDockedRect_t TrayUI_GetDockedRect_Original;
DWORD WINAPI TrayUI_GetDockedRect_Hook(void* pThis, RECT* rect, BOOL param2) {
    Wh_Log(L">");

    DWORD ret = TrayUI_GetDockedRect_Original(pThis, rect, param2);

    if (g_hasNativeTaskbarOnTop) {
        return ret;
    }

    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

    RECT monitorRect;
    GetMonitorRect(monitor, &monitorRect);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    int height = rect->bottom - rect->top;

    switch (GetTaskbarLocationForMonitor(monitor)) {
        case TaskbarLocation::top:
            rect->top = monitorRect.top;
            rect->bottom = monitorRect.top + height;
            break;

        case TaskbarLocation::bottom:
            // rect->top = monitorRect.bottom - height;
            // rect->bottom = monitorRect.bottom;
            break;

        case TaskbarLocation::left:
        case TaskbarLocation::right:
            break;
    }

    return ret;
}

using TrayUI_MakeStuckRect_t = void(WINAPI*)(void* pThis,
                                             RECT* rect,
                                             RECT* param2,
                                             SIZE param3,
                                             DWORD taskbarPos);
TrayUI_MakeStuckRect_t TrayUI_MakeStuckRect_Original;
void WINAPI TrayUI_MakeStuckRect_Hook(void* pThis,
                                      RECT* rect,
                                      RECT* param2,
                                      SIZE param3,
                                      DWORD taskbarPos) {
    Wh_Log(L">");

    TrayUI_MakeStuckRect_Original(pThis, rect, param2, param3, taskbarPos);

    if (g_hasNativeTaskbarOnTop || taskbarPos != ABE_BOTTOM) {
        return;
    }

    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

    RECT monitorRect;
    GetMonitorRect(monitor, &monitorRect);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    int height = rect->bottom - rect->top;

    switch (GetTaskbarLocationForMonitor(monitor)) {
        case TaskbarLocation::top:
            rect->top = monitorRect.top;
            rect->bottom = monitorRect.top + height;
            break;

        case TaskbarLocation::bottom:
            // rect->top = monitorRect.bottom - height;
            // rect->bottom = monitorRect.bottom;
            break;

        case TaskbarLocation::left:
        case TaskbarLocation::right:
            break;
    }
}

using TrayUI_GetStuckInfo_t = void(WINAPI*)(void* pThis,
                                            RECT* rect,
                                            DWORD* taskbarPos);
TrayUI_GetStuckInfo_t TrayUI_GetStuckInfo_Original;
void WINAPI TrayUI_GetStuckInfo_Hook(void* pThis,
                                     RECT* rect,
                                     DWORD* taskbarPos) {
    Wh_Log(L">");

    TrayUI_GetStuckInfo_Original(pThis, rect, taskbarPos);

    if (g_hasNativeTaskbarOnTop) {
        return;
    }

    switch (g_settings.taskbarLocation) {
        case TaskbarLocation::top:
            *taskbarPos = ABE_TOP;
            break;

        case TaskbarLocation::bottom:
            // *taskbarPos = ABE_BOTTOM;
            break;

        case TaskbarLocation::left:
        case TaskbarLocation::right:
            break;
    }
}

DWORD TaskbarLocationToEdge(TaskbarLocation taskbarLocation) {
    switch (taskbarLocation) {
        case TaskbarLocation::top:
            return ABE_TOP;

        case TaskbarLocation::bottom:
            return ABE_BOTTOM;

        case TaskbarLocation::left:
            return ABE_LEFT;

        case TaskbarLocation::right:
            return ABE_RIGHT;
    }
}

constexpr WCHAR kTaskbarSettingsSubKey[] =
    LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)";
// The native taskbar location, stored as a screen edge (ABE_*).
constexpr WCHAR kTaskbarLocationValueName[] = L"TaskbarLocation";
// Whether the taskbar is shown on all displays.
constexpr WCHAR kMultiMonTaskbarValueName[] = L"MMTaskbarEnabled";

// Makes explorer.exe read that the taskbar isn't shown on all displays.
std::atomic<bool> g_hidingSecondaryTaskbars;

// The taskbar windows read the location from the registry, the primary one
// when it's created, and the secondary ones for themselves. Other readers in
// explorer.exe, such as the taskbar settings, get the stored location, which is
// what's restored when the mod is unloaded.
thread_local bool g_inTrayUI__GetSaveStateAndInitRects;
thread_local bool g_inCSecondaryTray__LoadSettings;

using TrayUI__GetSaveStateAndInitRects_t = void(WINAPI*)(void* pThis);
TrayUI__GetSaveStateAndInitRects_t TrayUI__GetSaveStateAndInitRects_Original;
void WINAPI TrayUI__GetSaveStateAndInitRects_Hook(void* pThis) {
    Wh_Log(L">");

    g_inTrayUI__GetSaveStateAndInitRects = true;

    TrayUI__GetSaveStateAndInitRects_Original(pThis);

    g_inTrayUI__GetSaveStateAndInitRects = false;
}

using CSecondaryTray__LoadSettings_t = HRESULT(WINAPI*)(void* pThis);
CSecondaryTray__LoadSettings_t CSecondaryTray__LoadSettings_Original;
HRESULT WINAPI CSecondaryTray__LoadSettings_Hook(void* pThis) {
    Wh_Log(L">");

    g_inCSecondaryTray__LoadSettings = true;

    HRESULT ret = CSecondaryTray__LoadSettings_Original(pThis);

    g_inCSecondaryTray__LoadSettings = false;

    return ret;
}

bool ShouldOverrideTaskbarLocation() {
    // Quick Settings and the notification center, in other processes, read the
    // primary taskbar location from the registry too.
    if (g_target != Target::Explorer) {
        return GetNativeTaskbarOnTop() == NativeTaskbarOnTop::available;
    }

    return g_inTrayUI__GetSaveStateAndInitRects ||
           g_inCSecondaryTray__LoadSettings;
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY hkey,
                              LPCWSTR lpSubKey,
                              LPCWSTR lpValue,
                              DWORD dwFlags,
                              LPDWORD pdwType,
                              PVOID pvData,
                              LPDWORD pcbData) {
    DWORD cbData = pcbData ? *pcbData : 0;

    LONG ret = RegGetValueW_Original(hkey, lpSubKey, lpValue, dwFlags, pdwType,
                                     pvData, pcbData);

    if (!lpSubKey || _wcsicmp(lpSubKey, kTaskbarSettingsSubKey) != 0 ||
        !lpValue || !(dwFlags & RRF_RT_REG_DWORD) || !pvData ||
        cbData < sizeof(DWORD)) {
        return ret;
    }

    DWORD value;
    if (g_hidingSecondaryTaskbars &&
        _wcsicmp(lpValue, kMultiMonTaskbarValueName) == 0) {
        value = 0;
    } else if (!g_unloading && ShouldOverrideTaskbarLocation() &&
               hkey == HKEY_CURRENT_USER &&
               _wcsicmp(lpValue, kTaskbarLocationValueName) == 0) {
        TaskbarLocation taskbarLocation = g_inCSecondaryTray__LoadSettings
                                              ? GetTaskbarLocationSecondary()
                                              : g_settings.taskbarLocation;
        value = TaskbarLocationToEdge(taskbarLocation);
    } else {
        return ret;
    }

    if (ret == ERROR_SUCCESS) {
        Wh_Log(L"Overriding %s: %u->%u", lpValue, *(DWORD*)pvData, value);
    } else {
        Wh_Log(L"Overriding %s: %u", lpValue, value);
    }

    *(DWORD*)pvData = value;
    *pcbData = sizeof(DWORD);
    if (pdwType) {
        *pdwType = REG_DWORD;
    }

    return ERROR_SUCCESS;
}

void HookRegGetValueW() {
    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseRegGetValueW = (decltype(&RegGetValueW))GetProcAddress(
        kernelBaseModule, "RegGetValueW");
    WindhawkUtils::SetFunctionHook(pKernelBaseRegGetValueW, RegGetValueW_Hook,
                                   &RegGetValueW_Original);
}

DWORD GetStoredTaskbarLocation() {
    // Not hooked yet while the mod initializes.
    auto pRegGetValueW =
        RegGetValueW_Original ? RegGetValueW_Original : RegGetValueW;

    DWORD edge = ABE_BOTTOM;
    DWORD size = sizeof(edge);
    if (pRegGetValueW(HKEY_CURRENT_USER, kTaskbarSettingsSubKey,
                      kTaskbarLocationValueName, RRF_RT_REG_DWORD, nullptr,
                      &edge, &size) != ERROR_SUCCESS ||
        edge > ABE_BOTTOM) {
        edge = ABE_BOTTOM;
    }

    return edge;
}

// Each taskbar has its own settings, which natively all return the same
// location. The settings of secondary taskbars are identified when they're
// created, so ones created before the mod was loaded aren't included. Destroyed
// settings aren't removed, since an address is classified again when new
// settings reuse it.
std::mutex g_secondaryTaskbarSettingsMutex;
std::unordered_set<void*> g_secondaryTaskbarSettings;
// The location the secondary taskbars are laid out for, unknown for ones
// created before the mod was loaded.
std::optional<TaskbarLocation> g_secondaryTaskbarsLocation;

bool IsSecondaryTaskListWndSite(IUnknown* taskListWndSite) {
    winrt::com_ptr<IUnknown> site;
    site.copy_from(taskListWndSite);

    auto oleWindow = site.try_as<IOleWindow>();
    HWND hWnd;
    if (!oleWindow || FAILED(oleWindow->GetWindow(&hWnd))) {
        return false;
    }

    WCHAR szClassName[32];
    return GetClassName(GetAncestor(hWnd, GA_ROOT), szClassName,
                        ARRAYSIZE(szClassName)) &&
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

using TaskbarSettings_TaskbarSettings_t = void*(
    WINAPI*)(void* pThis, void* trayComponentHost, IUnknown* taskListWndSite);
TaskbarSettings_TaskbarSettings_t TaskbarSettings_TaskbarSettings_Original;
void* WINAPI TaskbarSettings_TaskbarSettings_Hook(void* pThis,
                                                  void* trayComponentHost,
                                                  IUnknown* taskListWndSite) {
    Wh_Log(L">");

    void* ret = TaskbarSettings_TaskbarSettings_Original(
        pThis, trayComponentHost, taskListWndSite);

    bool secondary =
        taskListWndSite && IsSecondaryTaskListWndSite(taskListWndSite);
    Wh_Log(L"Secondary taskbar settings: %d", secondary);

    std::lock_guard<std::mutex> guard(g_secondaryTaskbarSettingsMutex);
    if (secondary) {
        g_secondaryTaskbarSettings.insert(pThis);
        g_secondaryTaskbarsLocation = GetTaskbarLocationSecondary();
    } else {
        g_secondaryTaskbarSettings.erase(pThis);
    }

    return ret;
}

// Returns the previous location.
std::optional<TaskbarLocation> SetSecondaryTaskbarsLocation(
    TaskbarLocation taskbarLocation) {
    std::lock_guard<std::mutex> guard(g_secondaryTaskbarSettingsMutex);
    return std::exchange(g_secondaryTaskbarsLocation, taskbarLocation);
}

bool IsSecondaryTaskbarSettings(void* taskbarSettings) {
    std::lock_guard<std::mutex> guard(g_secondaryTaskbarSettingsMutex);
    return g_secondaryTaskbarSettings.contains(taskbarSettings);
}

// The location the taskbar content is laid out for, which it reads again
// whenever the taskbar settings change. The TaskbarLocation values match the
// screen edges.
using TaskbarSettings_Location_t = int(WINAPI*)(void* pThis);
TaskbarSettings_Location_t TaskbarSettings_Location_Original;
int WINAPI TaskbarSettings_Location_Hook(void* pThis) {
    if (!g_hasNativeTaskbarOnTop || g_unloading) {
        return TaskbarSettings_Location_Original(pThis);
    }

    TaskbarLocation taskbarLocation = IsSecondaryTaskbarSettings(pThis)
                                          ? GetTaskbarLocationSecondary()
                                          : g_settings.taskbarLocation;
    return TaskbarLocationToEdge(taskbarLocation);
}

void TaskbarWndProcPreProcess(HWND hWnd,
                              UINT Msg,
                              WPARAM* wParam,
                              LPARAM* lParam) {
    switch (Msg) {
        case 0x5C3: {
            // On Windows 11 23H2, setting the taskbar location here also causes
            // the start menu to be opened on the left of the screen, even if
            // the icons on the taskbar are centered. Therefore, only set it if
            // the icons are aligned to left, not centered. The drawback is that
            // the jump list animations won't be correct in this case.
            if (g_lastTaskbarAlignment == 1 &&
                !IsMainModuleVersionAtLeast(10, 0, 26100, 0)) {
                break;
            }

            // The taskbar location that affects the jump list animations. Only
            // change if primary taskbar is on top, otherwise the primary
            // taskbar won't have jump lists.
            if (!g_hasNativeTaskbarOnTop && *wParam == ABE_BOTTOM &&
                g_settings.taskbarLocation == TaskbarLocation::top) {
                HMONITOR monitor = (HMONITOR)lParam;
                if (GetTaskbarLocationForMonitor(monitor) ==
                    TaskbarLocation::top) {
                    *wParam = ABE_TOP;
                }
            }
            break;
        }

        case 0x5CA: {
            // A taskbar setting change, 6 being the location, which the
            // Settings app sends along with storing the location it's set to.
            if (g_hasNativeTaskbarOnTop && !g_unloading && *wParam == 6) {
                *lParam = TaskbarLocationToEdge(g_settings.taskbarLocation);
            }
            break;
        }
    }
}

LRESULT TaskbarWndProcPostProcess(HWND hWnd,
                                  UINT Msg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  bool secondaryTaskbar,
                                  LRESULT result) {
    if (g_hasNativeTaskbarOnTop) {
        return result;
    }

    switch (Msg) {
        case WM_SIZING: {
            Wh_Log(L"WM_SIZING: %08X", (DWORD)(ULONG_PTR)hWnd);

            if (!g_unloading) {
                RECT* rect = (RECT*)lParam;

                HMONITOR monitor =
                    MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

                RECT monitorRect;
                GetMonitorRect(monitor, &monitorRect);

                int height = rect->bottom - rect->top;

                switch (GetTaskbarLocationForMonitor(monitor)) {
                    case TaskbarLocation::top:
                        rect->top = monitorRect.top;
                        rect->bottom = monitorRect.top + height;
                        break;

                    case TaskbarLocation::bottom:
                        // rect->top = monitorRect.bottom - height;
                        // rect->bottom = monitorRect.bottom;
                        break;

                    case TaskbarLocation::left:
                    case TaskbarLocation::right:
                        break;
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* windowpos = (WINDOWPOS*)lParam;
            if ((windowpos->flags & (SWP_NOSIZE | SWP_NOMOVE)) !=
                (SWP_NOSIZE | SWP_NOMOVE)) {
                Wh_Log(L"WM_WINDOWPOSCHANGING (size or move): %08X",
                       (DWORD)(ULONG_PTR)hWnd);

                if (!g_unloading) {
                    RECT rect{
                        .left = windowpos->x,
                        .top = windowpos->y,
                        .right = windowpos->x + windowpos->cx,
                        .bottom = windowpos->y + windowpos->cy,
                    };
                    HMONITOR monitor =
                        MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);

                    if (!(windowpos->flags & SWP_NOMOVE) &&
                        GetTaskbarLocationForMonitor(monitor) ==
                            TaskbarLocation::top) {
                        RECT monitorRect;
                        GetMonitorRect(monitor, &monitorRect);

                        // Normal positioning without auto-hide adjustment.
                        int yPosition = monitorRect.top;

                        // Auto-hide positioning: move taskbar mostly off-screen
                        // when hiding.
                        if (!secondaryTaskbar && IsTaskbarAutoHideEnabled()) {
                            // Check if cursor is within the taskbar's current
                            // bounds.
                            DWORD messagePos = GetMessagePos();
                            POINT pt{
                                GET_X_LPARAM(messagePos),
                                GET_Y_LPARAM(messagePos),
                            };

                            RECT currentRect;
                            GetWindowRect(hWnd, &currentRect);

                            // Check if cursor is in the taskbar area (on screen
                            // and over taskbar).
                            int currentHeight =
                                currentRect.bottom - currentRect.top;
                            bool cursorInShownTaskbarArea =
                                pt.x >= monitorRect.left &&
                                pt.x < monitorRect.right &&
                                pt.y >= monitorRect.top &&
                                pt.y < monitorRect.top + currentHeight;
                            if (!cursorInShownTaskbarArea) {
                                // Cursor is not in taskbar - hide it by moving
                                // mostly off-screen.
                                UINT monitorDpiX = 96;
                                UINT monitorDpiY = 96;
                                GetDpiForMonitor(monitor, MDT_DEFAULT,
                                                 &monitorDpiX, &monitorDpiY);
                                int triggerHeight = MulDiv(
                                    kAutoHideTriggerHeight, monitorDpiY, 96);
                                yPosition -= currentHeight - triggerHeight;
                            }
                        }

                        windowpos->y = yPosition;
                    }
                }
            }
            break;
        }
    }

    return result;
}

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    if (Msg == g_getTaskbarRectRegisteredMsg) {
        HMONITOR monitor = (HMONITOR)wParam;
        RECT* rect = (RECT*)lParam;
        if (TrayUI_GetStuckRectForMonitor_Original) {
            if (!TrayUI_GetStuckRectForMonitor_Original(pThis, monitor, rect)) {
                SetRectEmpty(rect);
            }
        } else {
            SetRectEmpty(rect);
        }
        return 0;
    }

    g_hookCallCounter++;

    TaskbarWndProcPreProcess(hWnd, Msg, &wParam, &lParam);

    LRESULT ret =
        TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);

    ret = TaskbarWndProcPostProcess(hWnd, Msg, wParam, lParam,
                                    /*secondaryTaskbar=*/false, ret);

    g_hookCallCounter--;

    return ret;
}

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    g_hookCallCounter++;

    TaskbarWndProcPreProcess(hWnd, Msg, &wParam, &lParam);

    LRESULT ret =
        CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    ret = TaskbarWndProcPostProcess(hWnd, Msg, wParam, lParam,
                                    /*secondaryTaskbar=*/true, ret);

    g_hookCallCounter--;

    return ret;
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* taskBtnGroup,
                     int param2,
                     winrt::Windows::Foundation::Point* point,
                     HorizontalAlignment* horizontalAlignment,
                     VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t
    CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(
    void* pThis,
    void* taskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    HorizontalAlignment* horizontalAlignment,
    VerticalAlignment* verticalAlignment) {
    Wh_Log(L">");

    HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original(
        pThis, taskBtnGroup, param2, point, horizontalAlignment,
        verticalAlignment);

    if (g_hasNativeTaskbarOnTop) {
        return ret;
    }

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        return ret;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    // Place at the center of the monitor, will reposition later in
    // SetWindowPos.
    int centerY = monitorInfo.rcWork.top +
                  (monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) / 2;
    point->Y = centerY;

    return ret;
}

using CTaskListThumbnailWnd_DisplayUI_t = void*(WINAPI*)(void* pThis,
                                                         void* param1,
                                                         void* param2,
                                                         void* param3,
                                                         void* param4);
CTaskListThumbnailWnd_DisplayUI_t CTaskListThumbnailWnd_DisplayUI_Original;
void* WINAPI CTaskListThumbnailWnd_DisplayUI_Hook(void* pThis,
                                                  void* param1,
                                                  void* param2,
                                                  void* param3,
                                                  void* param4) {
    Wh_Log(L">");

    g_inCTaskListThumbnailWnd_DisplayUI = true;

    void* ret = CTaskListThumbnailWnd_DisplayUI_Original(pThis, param1, param2,
                                                         param3, param4);

    g_inCTaskListThumbnailWnd_DisplayUI = false;

    return ret;
}

using CTaskListThumbnailWnd_LayoutThumbnails_t = void(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_LayoutThumbnails_t
    CTaskListThumbnailWnd_LayoutThumbnails_Original;
void WINAPI CTaskListThumbnailWnd_LayoutThumbnails_Hook(void* pThis) {
    Wh_Log(L">");

    g_inCTaskListThumbnailWnd_LayoutThumbnails = true;

    CTaskListThumbnailWnd_LayoutThumbnails_Original(pThis);

    g_inCTaskListThumbnailWnd_LayoutThumbnails = false;
}

// This hook is unnecessary with XAML refresh (new thumbnails and other UI
// updates).
using XamlExplorerHostWindow_XamlExplorerHostWindow_t =
    void*(WINAPI*)(void* pThis,
                   unsigned int param1,
                   winrt::Windows::Foundation::Rect* rect,
                   unsigned int param3);
XamlExplorerHostWindow_XamlExplorerHostWindow_t
    XamlExplorerHostWindow_XamlExplorerHostWindow_Original;
void* WINAPI XamlExplorerHostWindow_XamlExplorerHostWindow_Hook(
    void* pThis,
    unsigned int param1,
    winrt::Windows::Foundation::Rect* rect,
    unsigned int param3) {
    Wh_Log(L">");

    if (g_inOverflowFlyoutModel_Show) {
        RECT rc{
            .left = static_cast<LONG>(rect->X),
            .top = static_cast<LONG>(rect->Y),
            .right = static_cast<LONG>(rect->X + rect->Width),
            .bottom = static_cast<LONG>(rect->Y + rect->Height),
        };

        HMONITOR monitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::top) {
            RECT workAreaRect;
            GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);
            UINT monitorDpiX = 96;
            UINT monitorDpiY = 96;
            GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

            winrt::Windows::Foundation::Rect rectNew = *rect;
            rectNew.Y = workAreaRect.top + MulDiv(12, monitorDpiY, 96);

            return XamlExplorerHostWindow_XamlExplorerHostWindow_Original(
                pThis, param1, &rectNew, param3);
        }
    }

    return XamlExplorerHostWindow_XamlExplorerHostWindow_Original(pThis, param1,
                                                                  rect, param3);
}

using ITaskbarSettings_get_Alignment_t = HRESULT(WINAPI*)(void* pThis,
                                                          int* alignment);
ITaskbarSettings_get_Alignment_t ITaskbarSettings_get_Alignment_Original;
HRESULT WINAPI ITaskbarSettings_get_Alignment_Hook(void* pThis,
                                                   int* alignment) {
    Wh_Log(L">");

    HRESULT ret = ITaskbarSettings_get_Alignment_Original(pThis, alignment);
    if (SUCCEEDED(ret)) {
        Wh_Log(L"alignment=%d", *alignment);
        g_lastTaskbarAlignment = *alignment;
    }

    return ret;
}

bool IsSecondaryTaskbar(XamlRoot xamlRoot) {
    FrameworkElement systemIconsPresenter = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid")) &&
        (child = FindChildByName(child, L"ControlCenterButton")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter"))) {
        systemIconsPresenter = child;
    }

    if (!systemIconsPresenter) {
        return false;
    }

    // Secondary taskbars have no system icons. The presenter is either left
    // without any children, or has an empty panel.
    if (Media::VisualTreeHelper::GetChildrenCount(systemIconsPresenter) == 0) {
        return true;
    }

    auto systemIconsPanel =
        EnumChildElements(systemIconsPresenter, [](FrameworkElement child) {
            return !!child.try_as<Controls::Panel>();
        });
    return systemIconsPanel &&
           Media::VisualTreeHelper::GetChildrenCount(systemIconsPanel) == 0;
}

void ApplyTaskbarFrameStyle(FrameworkElement taskbarFrame) {
    bool isSecondaryTaskbar = IsSecondaryTaskbar(taskbarFrame.XamlRoot());

    TaskbarLocation taskbarLocation = isSecondaryTaskbar
                                          ? GetTaskbarLocationSecondary()
                                          : g_settings.taskbarLocation;
    if (taskbarLocation != TaskbarLocation::top) {
        return;
    }

    Thickness margin{};
    if (!g_unloading) {
        // Fix the edge of the taskbar being non-clickable by moving the edge
        // pixels out of the screen.
        margin.Left -= 1;
        margin.Top -= 1;
        margin.Right -= 1;
    }

    if (auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid")) {
        rootGrid.Margin(margin);
    }

    // Align the background stroke to the bottom.
    FrameworkElement backgroundStroke = nullptr;

    FrameworkElement child = taskbarFrame;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"BackgroundControl")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"BackgroundStroke"))) {
        backgroundStroke = child;
    }

    if (backgroundStroke) {
        backgroundStroke.VerticalAlignment(
            g_unloading ? VerticalAlignment::Top : VerticalAlignment::Bottom);
    }
}

void* TaskbarController_OnGroupingModeChanged;

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    Wh_Log(L">");

    static LONG taskbarFrameOffset = []() -> LONG {
#if defined(_M_X64)
        // 48:83EC 28               | sub rsp,28
        // 48:8B81 88020000         | mov rax,qword ptr ds:[rcx+288]
        // or
        // 4C:8B81 80020000         | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }
#elif defined(_M_ARM64)
        // 00000001`806b1810 a9bf7bfd stp fp,lr,[sp,#-0x10]!
        // 00000001`806b1814 910003fd mov fp,sp
        // 00000001`806b1818 aa0003e8 mov x8,x0
        // 00000001`806b181c f9414500 ldr x0,[x8,#0x288]
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();

    if (taskbarFrameOffset <= 0) {
        Wh_Log(L"taskbarFrameOffset <= 0");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"!taskbarFrame");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"!taskbarFrameElement");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    // A workaround to issues related to tablet mode.
    // https://github.com/ramensoftware/windhawk-mods/issues/529#issuecomment-2419371239
    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());

    TaskbarController_UpdateFrameHeight_Original(pThis);

    // Adjust parent grid height if needed.
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            Wh_Log(L"Adjusting contentGrid.Height: %f->%f", contentGridHeight,
                   height);
            contentGrid.Height(height);
        }
    }
}

using TaskbarFrame_TaskbarFrame_t = void*(WINAPI*)(void* pThis);
TaskbarFrame_TaskbarFrame_t TaskbarFrame_TaskbarFrame_Original;
void* WINAPI TaskbarFrame_TaskbarFrame_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = TaskbarFrame_TaskbarFrame_Original(pThis);

    FrameworkElement taskbarFrame = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarFrame));
    if (!taskbarFrame) {
        return ret;
    }

    g_elementLoadedAutoRevokerList.emplace_back();
    auto autoRevokerIt = g_elementLoadedAutoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = taskbarFrame.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_elementLoadedAutoRevokerList.erase(autoRevokerIt);

            auto taskbarFrame = sender.try_as<FrameworkElement>();
            if (!taskbarFrame) {
                return;
            }

            auto className = winrt::get_class_name(taskbarFrame);
            Wh_Log(L"className: %s", className.c_str());

            try {
                ApplyTaskbarFrameStyle(taskbarFrame);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        });

    return ret;
}

void ApplySystemTrayChevronIconViewStyle(
    FrameworkElement systemTrayChevronIconViewElement) {
    if (g_settings.taskbarLocation != TaskbarLocation::top) {
        return;
    }

    FrameworkElement baseTextBlock = nullptr;

    FrameworkElement child = systemTrayChevronIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent")) &&
        (child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"Base"))) {
        baseTextBlock = child;
    }

    if (!baseTextBlock) {
        return;
    }

    double angle = g_unloading ? 0 : 180;
    Media::RotateTransform transform;
    transform.Angle(angle);
    baseTextBlock.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    baseTextBlock.RenderTransformOrigin({origin, origin});
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_elementLoadedAutoRevokerList.emplace_back();
    auto autoRevokerIt = g_elementLoadedAutoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_elementLoadedAutoRevokerList.erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.ChevronIconView") {
                if (IsChildOfElementByName(iconView, L"NotifyIconStack")) {
                    ApplySystemTrayChevronIconViewStyle(iconView);
                }
            }
        });

    return ret;
}

void UpdateTaskListButton(FrameworkElement taskListButtonElement) {
    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (!iconPanelElement) {
        return;
    }

    TaskbarLocation taskbarLocationSecondary = GetTaskbarLocationSecondary();
    bool verticalSecondaryTaskbar =
        taskbarLocationSecondary == TaskbarLocation::left ||
        taskbarLocationSecondary == TaskbarLocation::right;

    bool indicatorsOnTop = false;
    if (!g_unloading &&
        (g_settings.runningIndicatorsOnTop || verticalSecondaryTaskbar)) {
        auto taskbarFrameRepeaterElement =
            Media::VisualTreeHelper::GetParent(taskListButtonElement)
                .as<FrameworkElement>();

        bool isSecondaryTaskbar = false;
        if (!taskbarFrameRepeaterElement ||
            taskbarFrameRepeaterElement.Name() != L"TaskbarFrameRepeater") {
            // TODO: Can also be "OverflowFlyoutListRepeater".
        } else {
            isSecondaryTaskbar =
                IsSecondaryTaskbar(taskListButtonElement.XamlRoot());
        }

        TaskbarLocation taskbarLocation = isSecondaryTaskbar
                                              ? taskbarLocationSecondary
                                              : g_settings.taskbarLocation;

        // Vertical taskbars place their indicators themselves.
        if (taskbarLocation == TaskbarLocation::left ||
            taskbarLocation == TaskbarLocation::right) {
            return;
        }

        if (g_settings.runningIndicatorsOnTop &&
            taskbarLocation == TaskbarLocation::top) {
            indicatorsOnTop = true;
        }
    }

    PCWSTR indicatorClassNames[] = {
        L"RunningIndicator",
        L"ProgressIndicator",
    };
    for (auto indicatorClassName : indicatorClassNames) {
        auto indicatorElement =
            FindChildByName(iconPanelElement, indicatorClassName);
        if (!indicatorElement) {
            continue;
        }

        indicatorElement.VerticalAlignment(indicatorsOnTop
                                               ? VerticalAlignment::Top
                                               : VerticalAlignment::Bottom);
    }
}

// With the native taskbar on top, buttons update their visual states when the
// taskbar location changes, but not when only the settings change, so the
// buttons are kept to be updated when the settings are applied.
std::vector<winrt::weak_ref<FrameworkElement>> g_nativeTaskListButtons;

void TrackNativeTaskListButton(FrameworkElement taskListButtonElement) {
    std::erase_if(g_nativeTaskListButtons,
                  [](const auto& button) { return !button.get(); });

    for (const auto& button : g_nativeTaskListButtons) {
        if (button.get() == taskListButtonElement) {
            return;
        }
    }

    g_nativeTaskListButtons.push_back(winrt::make_weak(taskListButtonElement));
}

void UpdateNativeTaskListButtons() {
    for (const auto& button : g_nativeTaskListButtons) {
        if (auto element = button.get()) {
            UpdateTaskListButton(element);
        }
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateVisualStates_Original(pThis);

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    try {
        if (g_hasNativeTaskbarOnTop) {
            TrackNativeTaskListButton(taskListButtonElement);
        }

        UpdateTaskListButton(taskListButtonElement);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }
}

// With the native taskbar on top, taskbar backgrounds lay out their stroke for
// the location they get from their taskbar. Secondary taskbars created before
// the mod was loaded pass the primary taskbar location, so secondary taskbar
// backgrounds get their own location instead. The backgrounds are kept to be
// updated when the settings are applied, since only a change of the taskbar
// location updates them.
struct NativeTaskbarBackground {
    winrt::weak_ref<FrameworkElement> element;
    // Valid as long as the element is alive.
    void* taskbarBackground;
    // The location the taskbar gave the background.
    int location;
};

std::vector<NativeTaskbarBackground> g_nativeTaskbarBackgrounds;

using TaskbarBackground_Location_t = void(WINAPI*)(void* pThis, int location);
TaskbarBackground_Location_t TaskbarBackground_Location_Original;

int GetNativeTaskbarBackgroundLocation(FrameworkElement element, int location) {
    if (g_unloading) {
        return location;
    }

    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot || !IsSecondaryTaskbar(xamlRoot)) {
        return location;
    }

    return TaskbarLocationToEdge(GetTaskbarLocationSecondary());
}

NativeTaskbarBackground* FindNativeTaskbarBackground(FrameworkElement element) {
    std::erase_if(g_nativeTaskbarBackgrounds, [](const auto& background) {
        return !background.element.get();
    });

    for (auto& background : g_nativeTaskbarBackgrounds) {
        if (background.element.get() == element) {
            return &background;
        }
    }

    return nullptr;
}

void UpdateNativeTaskbarBackground(const NativeTaskbarBackground& background) {
    if (auto element = background.element.get()) {
        TaskbarBackground_Location_Original(
            background.taskbarBackground,
            GetNativeTaskbarBackgroundLocation(element, background.location));
    }
}

void UpdateNativeTaskbarBackgrounds() {
    for (const auto& background : g_nativeTaskbarBackgrounds) {
        UpdateNativeTaskbarBackground(background);
    }
}

// A secondary taskbar is detected by its system tray, which can be loaded after
// the background gets its location.
void UpdateNativeTaskbarBackgroundWhenLoaded(FrameworkElement element) {
    FrameworkElement pendingElement = element;
    if (element.IsLoaded()) {
        auto xamlRoot = element.XamlRoot();
        auto content =
            xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
        pendingElement = content ? FindChildByClassName(
                                       content, L"SystemTray.SystemTrayFrame")
                                 : nullptr;
        if (!pendingElement || pendingElement.IsLoaded()) {
            return;
        }
    }

    g_elementLoadedAutoRevokerList.emplace_back();
    auto autoRevokerIt = g_elementLoadedAutoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = pendingElement.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt, elementWeak = winrt::make_weak(element)](
            winrt::Windows::Foundation::IInspectable const& sender,
            RoutedEventArgs const& e) {
            Wh_Log(L">");

            auto element = elementWeak.get();

            g_elementLoadedAutoRevokerList.erase(autoRevokerIt);

            if (!element) {
                return;
            }

            try {
                if (auto background = FindNativeTaskbarBackground(element)) {
                    UpdateNativeTaskbarBackground(*background);
                }

                UpdateNativeTaskbarBackgroundWhenLoaded(element);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        });
}

int TrackNativeTaskbarBackground(void* taskbarBackground, int location) {
    void* taskbarBackgroundIUnknownPtr = (void**)taskbarBackground + 3;
    winrt::Windows::Foundation::IUnknown taskbarBackgroundIUnknown;
    winrt::copy_from_abi(taskbarBackgroundIUnknown,
                         taskbarBackgroundIUnknownPtr);

    auto element = taskbarBackgroundIUnknown.try_as<FrameworkElement>();
    if (!element) {
        return location;
    }

    if (auto background = FindNativeTaskbarBackground(element)) {
        background->location = location;
        return GetNativeTaskbarBackgroundLocation(element, location);
    }

    g_nativeTaskbarBackgrounds.push_back({
        .element = winrt::make_weak(element),
        .taskbarBackground = taskbarBackground,
        .location = location,
    });

    UpdateNativeTaskbarBackgroundWhenLoaded(element);

    return GetNativeTaskbarBackgroundLocation(element, location);
}

void WINAPI TaskbarBackground_Location_Hook(void* pThis, int location) {
    Wh_Log(L"> %d", location);

    try {
        location = TrackNativeTaskbarBackground(pThis, location);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }

    TaskbarBackground_Location_Original(pThis, location);
}

using OverflowFlyoutModel_Show_t = void(WINAPI*)(void* pThis);
OverflowFlyoutModel_Show_t OverflowFlyoutModel_Show_Original;
void WINAPI OverflowFlyoutModel_Show_Hook(void* pThis) {
    Wh_Log(L">");

    g_inOverflowFlyoutModel_Show = true;

    OverflowFlyoutModel_Show_Original(pThis);

    g_inOverflowFlyoutModel_Show = false;
}

using FlyoutFrame_UpdateFlyoutPosition_t = void(WINAPI*)(void* pThis);
FlyoutFrame_UpdateFlyoutPosition_t FlyoutFrame_UpdateFlyoutPosition_Original;
void WINAPI FlyoutFrame_UpdateFlyoutPosition_Hook(void* pThis) {
    Wh_Log(L">");

    g_UpdateFlyoutPosition_threadId = GetCurrentThreadId();
    g_UpdateFlyoutPosition_pThis = pThis;

    FlyoutFrame_UpdateFlyoutPosition_Original(pThis);

    g_UpdateFlyoutPosition_threadId = 0;
    g_UpdateFlyoutPosition_pThis = nullptr;
}

using MenuFlyout_ShowAt_t =
    void*(WINAPI*)(void* pThis,
                   DependencyObject* placementTarget,
                   Controls::Primitives::FlyoutShowOptions* showOptions);
MenuFlyout_ShowAt_t MenuFlyout_ShowAt_Original;
void* WINAPI
MenuFlyout_ShowAt_Hook(void* pThis,
                       DependencyObject* placementTarget,
                       Controls::Primitives::FlyoutShowOptions* showOptions) {
    Wh_Log(L">");

    auto original = [=]() {
        g_inMenuFlyout_ShowAt = true;
        void* ret =
            MenuFlyout_ShowAt_Original(pThis, placementTarget, showOptions);
        g_inMenuFlyout_ShowAt = false;
        return ret;
    };

    if (!showOptions) {
        return original();
    }

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        return original();
    }

    auto placement = showOptions->Placement();
    Wh_Log(L"Placement=%d", (int)placement);
    if (placement == Controls::Primitives::FlyoutPlacementMode::Top) {
        showOptions->Placement(
            Controls::Primitives::FlyoutPlacementMode::Bottom);
    }

    auto point =
        showOptions->Position().try_as<winrt::Windows::Foundation::Point>();
    if (point) {
        Wh_Log(L"Point=%fx%f", point->X, point->Y);
        if (point->Y < 0) {
            point->Y = -point->Y;

            FrameworkElement targetElement =
                placementTarget->try_as<FrameworkElement>();
            if (targetElement) {
                point->Y += targetElement.ActualHeight();
            }

            showOptions->Position(point);
        }
    }

    return original();
}

bool HandleSystemTrayContextMenu(FrameworkElement element) {
    Wh_Log(L">");

    FrameworkElement childElement = FindChildByName(element, L"ContainerGrid");
    if (!childElement) {
        Wh_Log(L"No child element");
        return false;
    }

    auto flyout =
        Controls::Primitives::FlyoutBase::GetAttachedFlyout(childElement);
    if (!flyout) {
        Wh_Log(L"No flyout");
        return false;
    }

    Controls::Primitives::FlyoutShowOptions options;
    options.Position(winrt::Windows::Foundation::Point{
        static_cast<float>(childElement.ActualWidth()),
        static_cast<float>(childElement.ActualHeight())});
    flyout.ShowAt(childElement, options);
    return true;
}

using TextIconContent_ShowContextMenu_t = void(WINAPI*)(void* pThis);
TextIconContent_ShowContextMenu_t TextIconContent_ShowContextMenu_Original;
void WINAPI TextIconContent_ShowContextMenu_Hook(void* pThis) {
    Wh_Log(L">");

    FrameworkElement element = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(element));
    if (!element || !HandleSystemTrayContextMenu(element)) {
        TextIconContent_ShowContextMenu_Original(pThis);
    }
}

using DateTimeIconContent_ShowContextMenu_t = void(WINAPI*)(void* pThis);
DateTimeIconContent_ShowContextMenu_t
    DateTimeIconContent_ShowContextMenu_Original;
void WINAPI DateTimeIconContent_ShowContextMenu_Hook(void* pThis) {
    Wh_Log(L">");

    FrameworkElement element = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(element));
    if (!element || !HandleSystemTrayContextMenu(element)) {
        DateTimeIconContent_ShowContextMenu_Original(pThis);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !g_inMenuFlyout_ShowAt) {
        return hWnd;
    }

    // XAML creates the windowed popup of a menu flyout during ShowAt, but
    // positions it later, so mark it here for SetWindowPos_Hook.
    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"Xaml_WindowedPopupClass") == 0) {
        Wh_Log(L"Menu flyout popup window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        SetProp(hWnd, kMenuFlyoutPopupPropName, (HANDLE)1);
    }

    return hWnd;
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    auto original = [=]() {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"TaskListThumbnailWnd") == 0) {
        if (uFlags & SWP_NOMOVE) {
            return original();
        }

        if (!g_inCTaskListThumbnailWnd_DisplayUI &&
            !g_inCTaskListThumbnailWnd_LayoutThumbnails) {
            return original();
        }

        DWORD messagePos = GetMessagePos();
        POINT pt{
            GET_X_LPARAM(messagePos),
            GET_Y_LPARAM(messagePos),
        };

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        RECT workAreaRect;
        GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        if (g_inCTaskListThumbnailWnd_DisplayUI) {
            Y = workAreaRect.top + MulDiv(12, monitorDpiY, 96);
        } else {
            // Keep current position.
            RECT rc;
            GetWindowRect(hWnd, &rc);
            Y = rc.top;
        }
    } else if (_wcsicmp(szClassName, L"TopLevelWindowForOverflowXamlIsland") ==
                   0 ||
               (_wcsicmp(szClassName, L"Xaml_WindowedPopupClass") == 0 &&
                !GetProp(hWnd, kMenuFlyoutPopupPropName))) {
        if (uFlags & (SWP_NOMOVE | SWP_NOSIZE)) {
            return original();
        }

        DWORD messagePos = GetMessagePos();
        POINT pt{
            GET_X_LPARAM(messagePos),
            GET_Y_LPARAM(messagePos),
        };

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        RECT workAreaRect;
        GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        bool adjusted = false;

        WCHAR rootOwnerClassName[64];
        if (_wcsicmp(szClassName, L"Xaml_WindowedPopupClass") == 0 &&
            GetWindowThreadProcessId(hWnd, nullptr) ==
                GetWindowThreadProcessId(FindCurrentProcessTaskbarWnd(),
                                         nullptr) &&
            GetClassName(GetAncestor(hWnd, GA_ROOTOWNER), rootOwnerClassName,
                         ARRAYSIZE(rootOwnerClassName))) {
            if (_wcsicmp(rootOwnerClassName, L"XamlExplorerHostIslandWindow") ==
                0) {
                // Probably hovering a XAML thumbnail preview, make it so that
                // the tooltip doesn't cover the thumbnail preview.
                Y = workAreaRect.top +
                    MulDiv(10 + g_lastFlyoutPositionSize.Height, monitorDpiY,
                           96);
                adjusted = true;
            } else if (_wcsicmp(rootOwnerClassName,
                                L"TopLevelWindowForOverflowXamlIsland") == 0) {
                // Don't adjust to prevent tooltips from covering the overflow
                // flyout.
                adjusted = true;
            }
        }

        if (!adjusted) {
            if (Y < workAreaRect.top) {
                Y = workAreaRect.top;
            } else if (Y > workAreaRect.bottom - cy) {
                Y = workAreaRect.bottom - cy;
            }
        }
    } else if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        if (uFlags & SWP_NOMOVE) {
            return original();
        }

        DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (!threadId) {
            return original();
        }

        HANDLE thread =
            OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (!thread) {
            return original();
        }

        PWSTR threadDescription;
        HRESULT hr = pGetThreadDescription
                         ? pGetThreadDescription(thread, &threadDescription)
                         : E_FAIL;
        CloseHandle(thread);
        if (FAILED(hr)) {
            return original();
        }

        bool isJumpViewUI = wcscmp(threadDescription, L"JumpViewUI") == 0;

        LocalFree(threadDescription);

        if (!isJumpViewUI) {
            return original();
        }

        POINT pt;
        GetCursorPos(&pt);

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        RECT workAreaRect;
        GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);

        Y = workAreaRect.top;

        // If hovering over the overflow window, exclude it.
        HWND windowFromPoint = WindowFromPoint(pt);
        if (windowFromPoint &&
            GetWindowThreadProcessId(windowFromPoint, nullptr) ==
                GetWindowThreadProcessId(FindCurrentProcessTaskbarWnd(),
                                         nullptr)) {
            WCHAR szClassNameFromPoint[64];
            if (GetClassName(windowFromPoint, szClassNameFromPoint,
                             ARRAYSIZE(szClassNameFromPoint)) &&
                _wcsicmp(szClassNameFromPoint,
                         L"XamlExplorerHostIslandWindow") == 0) {
                UINT monitorDpiX = 96;
                UINT monitorDpiY = 96;
                GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                 &monitorDpiY);

                int overflowHeight = MulDiv(54 + 12, monitorDpiY, 96);

                Y += overflowHeight;
            }
        }
    } else {
        return original();
    }

    Wh_Log(L"Adjusting pos for %s: %dx%d, %dx%d", szClassName, X, Y, X + cx,
           Y + cy);

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;
BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            BOOL bRepaint) {
    auto original = [=]() {
        return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
    };

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"XamlExplorerHostIslandWindow") == 0) {
        DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (!threadId) {
            return original();
        }

        HANDLE thread =
            OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (!thread) {
            return original();
        }

        PWSTR threadDescription;
        HRESULT hr = pGetThreadDescription
                         ? pGetThreadDescription(thread, &threadDescription)
                         : E_FAIL;
        CloseHandle(thread);
        if (FAILED(hr)) {
            return original();
        }

        bool isMultitaskingView =
            wcscmp(threadDescription, L"MultitaskingView") == 0;

        LocalFree(threadDescription);

        if (!isMultitaskingView) {
            return original();
        }

        // Only handle the virtual desktop switcher, which shows up when
        // hovering over the task view button in the taskbar. Skip Alt+Tab
        // window, which uses band ZBID_SYSTEM_TOOLS. The virtual desktop
        // switcher uses band ZBID_IMMERSIVE_EDGY.
        DWORD band = 0;
        if (pGetWindowBand && pGetWindowBand(hWnd, &band) &&
            band == ZBID_SYSTEM_TOOLS) {
            return original();
        }

        RECT rect{
            .left = X,
            .top = Y,
            .right = X + nWidth,
            .bottom = Y + nHeight,
        };

        HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        RECT workAreaRect;
        GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        Y = workAreaRect.top + MulDiv(12, monitorDpiY, 96);
    } else {
        return original();
    }

    Wh_Log(L"Adjusting pos for %s: %dx%d, %dx%d", szClassName, X, Y, X + nWidth,
           Y + nHeight);

    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

using MapWindowPoints_t = decltype(&MapWindowPoints);
MapWindowPoints_t MapWindowPoints_Original;
int WINAPI MapWindowPoints_Hook(HWND hWndFrom,
                                HWND hWndTo,
                                LPPOINT lpPoints,
                                UINT cPoints) {
    int ret = MapWindowPoints_Original(hWndFrom, hWndTo, lpPoints, cPoints);

    if (GetCurrentThreadId() != g_UpdateFlyoutPosition_threadId ||
        !g_UpdateFlyoutPosition_pThis || cPoints != 1) {
        return ret;
    }

    Wh_Log(L">");

    // For build 26120.4733 or newer with the 56848060 feature flag which
    // enables thumbnail transition animations.
    FrameworkElement flyoutFrame = nullptr;
    ((IUnknown*)g_UpdateFlyoutPosition_pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(flyoutFrame));
    if (!flyoutFrame) {
        // For previous builds.
        ((IUnknown**)g_UpdateFlyoutPosition_pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(flyoutFrame));
        if (!flyoutFrame) {
            Wh_Log(L"Error getting flyoutFrame");
            return ret;
        }
    }

    FrameworkElement hoverFlyoutCanvas =
        FindChildByName(flyoutFrame, L"HoverFlyoutCanvas");
    if (!hoverFlyoutCanvas) {
        Wh_Log(L"No HoverFlyoutCanvas");
        return ret;
    }

    Controls::Grid hoverFlyoutGrid =
        FindChildByName(hoverFlyoutCanvas, L"HoverFlyoutGrid")
            .try_as<Controls::Grid>();
    if (!hoverFlyoutGrid) {
        Wh_Log(L"No HoverFlyoutGrid");
        return ret;
    }

    auto flyoutPositionSize = hoverFlyoutGrid.DesiredSize();
    g_lastFlyoutPositionSize = flyoutPositionSize;

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) != TaskbarLocation::top) {
        return ret;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT workAreaRect;
    GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    int flyoutHeight = MulDiv(flyoutPositionSize.Height, monitorDpiY, 96);

    // Align to bottom instead of top.
    int offsetToAdd = flyoutHeight;

    // Add work area space.
    offsetToAdd += workAreaRect.top - monitorInfo.rcMonitor.top;

    // Add margin.
    offsetToAdd += MulDiv(12, monitorDpiY, 96);

    lpPoints->y += std::min(
        offsetToAdd, (int)(workAreaRect.bottom - monitorInfo.rcMonitor.top));

    return ret;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }

    BOOL cloak = *(BOOL*)pvAttribute;
    if (cloak) {
        return original();
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    TaskbarLocation taskbarLocation = GetTaskbarLocationForMonitor(monitor);

    // Windows places the windows for a bottom taskbar, and the native taskbar
    // on top places them for the primary taskbar location.
    TaskbarLocation windowsTaskbarLocation =
        g_hasNativeTaskbarOnTop && !g_unloading ? g_settings.taskbarLocation
                                                : TaskbarLocation::bottom;
    if (taskbarLocation == windowsTaskbarLocation) {
        return original();
    }

    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);

    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
        return original();
    }

    std::wstring processFileName = GetProcessFileName(processId);

    enum class DwmTarget {
        StartMenu,
        SearchHost,
    };
    DwmTarget target;

    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") ==
        0) {
        target = DwmTarget::StartMenu;
    } else if (!g_hasNativeTaskbarOnTop &&
               _wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    } else {
        return original();
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }

    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;

    if (target == DwmTarget::StartMenu) {
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        int xNew = x;
        int yNew = y;
        if (g_hasNativeTaskbarOnTop) {
            // Only move towards the taskbar.
            switch (taskbarLocation) {
                case TaskbarLocation::top:
                    yNew = monitorInfo.rcWork.top;
                    break;

                case TaskbarLocation::bottom:
                    yNew = monitorInfo.rcWork.bottom - cy;
                    break;

                case TaskbarLocation::left:
                    xNew = monitorInfo.rcWork.left;
                    break;

                case TaskbarLocation::right:
                    xNew = monitorInfo.rcWork.right - cx;
                    break;
            }
        } else {
            // Only change x. This is a workaround for ExplorerPatcher, see:
            // https://github.com/ramensoftware/windhawk-mods/issues/2401
            xNew = monitorInfo.rcWork.left;
        }

        if (xNew == x && yNew == y) {
            return original();
        }

        x = xNew;
        y = yNew;
    } else if (target == DwmTarget::SearchHost) {
        // Only change y.
        RECT workAreaRect;
        GetMonitorWorkAreaWithoutTaskbar(monitor, &workAreaRect);

        int yNew = workAreaRect.top;

        if (yNew == y) {
            return original();
        }

        y = yNew;
    }

    // SetWindowPos isn't hooked for the native taskbar on top.
    auto pSetWindowPos =
        SetWindowPos_Original ? SetWindowPos_Original : SetWindowPos;
    pSetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

    return original();
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
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
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
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

namespace StartMenuUI {

// Overrides a property's local value, keeping the latest local value set by
// Windows to restore it, or to clear the property if there's none.
template <typename T>
class PropertyOverride {
   public:
    using PropertyGetter = DependencyProperty (*)();

    explicit PropertyOverride(PropertyGetter property) : m_property(property) {}

    void Set(DependencyObject element, T value) {
        auto property = m_property();
        auto localValue = element.ReadLocalValue(property).try_as<T>();
        if (m_element.get() != element || localValue != m_value) {
            m_element = element;
            m_windowsValue = localValue;
        }

        m_value = value;
        element.SetValue(property, winrt::box_value(value));
    }

    void Restore() {
        auto element = m_element.get();
        m_element = nullptr;
        if (!element) {
            return;
        }

        auto property = m_property();
        if (element.ReadLocalValue(property).try_as<T>() != m_value) {
            return;
        }

        if (m_windowsValue) {
            element.SetValue(property, winrt::box_value(*m_windowsValue));
        } else {
            element.ClearValue(property);
        }
    }

   private:
    PropertyGetter m_property;
    winrt::weak_ref<DependencyObject> m_element;
    std::optional<T> m_windowsValue;
    T m_value{};
};

bool g_inApplyStyle;
bool g_startMenuAnimationAdjusted;
PropertyOverride<double> g_canvasTopOverride{&Controls::Canvas::TopProperty};
PropertyOverride<double> g_canvasLeftOverride{&Controls::Canvas::LeftProperty};
PropertyOverride<VerticalAlignment> g_verticalAlignmentOverride{
    &FrameworkElement::VerticalAlignmentProperty};
PropertyOverride<HorizontalAlignment> g_horizontalAlignmentOverride{
    &FrameworkElement::HorizontalAlignmentProperty};
PropertyOverride<Thickness> g_marginOverride{&FrameworkElement::MarginProperty};
winrt::weak_ref<DependencyObject> g_startSizingFrameWeakRef;
int64_t g_canvasTopPropertyChangedToken;
int64_t g_canvasLeftPropertyChangedToken;
winrt::weak_ref<DependencyObject> g_frameRootWeakRef;
int64_t g_verticalAlignmentPropertyChangedToken;
int64_t g_horizontalAlignmentPropertyChangedToken;
winrt::event_token g_visibilityChangedToken;

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

void ApplyStyle();

// The part of the Start menu placement set by the mod, the rest is left to
// Windows. The full adjustment also covers the animation.
enum class StartMenuAdjustment {
    none,
    position,
    full,
};

void ApplyStyleClassicStartMenu(FrameworkElement content,
                                TaskbarLocation taskbarLocation,
                                HMONITOR monitor,
                                StartMenuAdjustment adjustment) {
    FrameworkElement startSizingFrame =
        FindChildByClassName(content, L"StartDocked.StartSizingFrame");
    if (!startSizingFrame) {
        Wh_Log(L"Failed to find StartDocked.StartSizingFrame");
        return;
    }

    bool adjustAnimation = adjustment == StartMenuAdjustment::full &&
                           g_settings.startMenuAnimationAdjust;
    if (adjustAnimation || g_startMenuAnimationAdjusted) {
        g_startMenuAnimationAdjusted = adjustAnimation;

        FrameworkElement child = startSizingFrame;
        if ((child = FindChildByClassName(
                 child, L"StartDocked.StartSizingFramePanel")) &&
            (child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.ContentPresenter")) &&
            (child = FindChildByClassName(child,
                                          L"Windows.UI.Xaml.Controls.Frame")) &&
            (child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.ContentPresenter")) &&
            (child =
                 FindChildByClassName(child, L"StartDocked.LauncherFrame"))) {
            auto launcherFrame = child;

            FrameworkElement rootGridContent = nullptr;
            if ((child = FindChildByName(launcherFrame, L"RootPanel")) &&
                (child = FindChildByName(child, L"RootGrid")) &&
                (child = FindChildByName(child, L"RootContent"))) {
                rootGridContent = child;
            } else if ((child = FindChildByName(launcherFrame, L"RootGrid")) &&
                       (child = FindChildByName(child, L"RootContent"))) {
                rootGridContent = child;
            }

            if (rootGridContent) {
                FrameworkElement rootGridShadow = nullptr;
                if ((child = FindChildByName(launcherFrame, L"RootPanel")) &&
                    (child = FindChildByName(child, L"RootGridDropShadow"))) {
                    rootGridShadow = child;
                } else if ((child = FindChildByClassName(
                                startSizingFrame,
                                L"StartDocked.StartSizingFramePanel")) &&
                           (child = FindChildByName(child, L"DropShadow"))) {
                    rootGridShadow = child;
                }

                double angle = 0;
                if (adjustAnimation) {
                    angle = 180;
                }

                Media::RotateTransform transform;
                transform.Angle(angle);
                startSizingFrame.RenderTransform(transform);
                Media::RotateTransform transform2;
                transform2.Angle(-angle);
                rootGridContent.RenderTransform(transform2);
                if (rootGridShadow) {
                    Media::RotateTransform transform3;
                    transform3.Angle(-angle);
                    rootGridShadow.RenderTransform(transform3);
                }

                auto origin = adjustAnimation
                                  ? winrt::Windows::Foundation::Point{0.5, 0.5}
                                  : winrt::Windows::Foundation::Point{};
                startSizingFrame.RenderTransformOrigin(origin);
                rootGridContent.RenderTransformOrigin(origin);
                if (rootGridShadow) {
                    rootGridShadow.RenderTransformOrigin(origin);
                }
            }
        }
    }

    Wh_Log(L"Invalidating measure");
    startSizingFrame.InvalidateMeasure();

    if (adjustment == StartMenuAdjustment::none) {
        g_canvasTopOverride.Restore();
        g_canvasLeftOverride.Restore();
    } else {
        double canvasWidth = content.ActualWidth();
        double canvasHeight = content.ActualHeight();

        constexpr int kStartMenuMargin = 12;

        double newTop;
        switch (taskbarLocation) {
            case TaskbarLocation::top:
            case TaskbarLocation::left:
            case TaskbarLocation::right:
                newTop = kStartMenuMargin;
                break;

            case TaskbarLocation::bottom:
                newTop = canvasHeight - startSizingFrame.ActualHeight() -
                         kStartMenuMargin;
                break;
        }

        Wh_Log(L"Setting Canvas.Top to %f", newTop);
        g_canvasTopOverride.Set(startSizingFrame, newTop);

        std::optional<double> newLeft;
        switch (taskbarLocation) {
            case TaskbarLocation::top:
            case TaskbarLocation::bottom:
                break;

            case TaskbarLocation::left:
                newLeft = kStartMenuMargin;
                break;

            case TaskbarLocation::right:
                newLeft = canvasWidth - startSizingFrame.ActualWidth() -
                          kStartMenuMargin;
                break;
        }

        if (newLeft) {
            Wh_Log(L"Setting Canvas.Left to %f", *newLeft);
            g_canvasLeftOverride.Set(startSizingFrame, *newLeft);
        } else {
            g_canvasLeftOverride.Restore();
        }

        // Subscribe to Canvas.Top and Canvas.Left property changes to apply
        // custom styles right when that happens. Without it, the start menu may
        // end up truncated. A simple reproduction is to open the start menu on
        // different monitors, each with a different resolution/DPI/taskbar
        // side.
        if (!g_startSizingFrameWeakRef.get()) {
            auto startSizingFrameDo = startSizingFrame.as<DependencyObject>();

            g_startSizingFrameWeakRef = startSizingFrameDo;

            g_canvasTopPropertyChangedToken =
                startSizingFrameDo.RegisterPropertyChangedCallback(
                    Controls::Canvas::TopProperty(),
                    [](DependencyObject sender, DependencyProperty property) {
                        double top = Controls::Canvas::GetTop(
                            sender.as<FrameworkElement>());
                        Wh_Log(L"Canvas.Top changed to %f", top);
                        if (!g_inApplyStyle) {
                            ApplyStyle();
                        }
                    });

            g_canvasLeftPropertyChangedToken =
                startSizingFrameDo.RegisterPropertyChangedCallback(
                    Controls::Canvas::LeftProperty(),
                    [](DependencyObject sender, DependencyProperty property) {
                        double left = Controls::Canvas::GetLeft(
                            sender.as<FrameworkElement>());
                        Wh_Log(L"Canvas.Left changed to %f", left);
                        if (!g_inApplyStyle) {
                            ApplyStyle();
                        }
                    });
        }
    }
}

void ApplyStyleRedesignedStartMenu(FrameworkElement content,
                                   TaskbarLocation taskbarLocation,
                                   StartMenuAdjustment adjustment) {
    if (adjustment == StartMenuAdjustment::none) {
        g_verticalAlignmentOverride.Restore();
        g_horizontalAlignmentOverride.Restore();
        g_marginOverride.Restore();
        return;
    }

    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find Start menu frame root");
        return;
    }

    // Adjust the margin set by Windows.
    g_marginOverride.Restore();
    auto margin = frameRoot.Margin();
    auto marginVertical = margin.Top + margin.Bottom;

    VerticalAlignment verticalAlignment;
    switch (taskbarLocation) {
        case TaskbarLocation::top:
        case TaskbarLocation::left:
        case TaskbarLocation::right:
            verticalAlignment = VerticalAlignment::Top;
            margin.Top = 0;
            margin.Bottom = marginVertical;
            break;

        case TaskbarLocation::bottom:
            verticalAlignment = VerticalAlignment::Bottom;
            margin.Top = marginVertical;
            margin.Bottom = 0;
            break;
    }

    g_verticalAlignmentOverride.Set(frameRoot, verticalAlignment);

    std::optional<HorizontalAlignment> horizontalAlignment;
    switch (taskbarLocation) {
        case TaskbarLocation::top:
        case TaskbarLocation::bottom:
            break;

        case TaskbarLocation::left:
            horizontalAlignment = HorizontalAlignment::Left;
            break;

        case TaskbarLocation::right:
            horizontalAlignment = HorizontalAlignment::Right;
            break;
    }

    if (horizontalAlignment) {
        margin.Left = 0;
        margin.Right = 0;
        g_horizontalAlignmentOverride.Set(frameRoot, *horizontalAlignment);
    } else {
        g_horizontalAlignmentOverride.Restore();
    }

    g_marginOverride.Set(frameRoot, margin);

    if (!g_frameRootWeakRef.get()) {
        auto frameRootDo = frameRoot.as<DependencyObject>();

        g_frameRootWeakRef = frameRootDo;

        g_verticalAlignmentPropertyChangedToken =
            frameRootDo.RegisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto alignment =
                        sender.as<FrameworkElement>().VerticalAlignment();
                    Wh_Log(L"FrameRoot VerticalAlignment changed to %d",
                           static_cast<int>(alignment));
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });

        g_horizontalAlignmentPropertyChangedToken =
            frameRootDo.RegisterPropertyChangedCallback(
                FrameworkElement::HorizontalAlignmentProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto alignment =
                        sender.as<FrameworkElement>().HorizontalAlignment();
                    Wh_Log(L"FrameRoot HorizontalAlignment changed to %d",
                           static_cast<int>(alignment));
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });
    }
}

StartMenuAdjustment GetStartMenuAdjustment(TaskbarLocation taskbarLocation) {
    if (g_unloading) {
        return StartMenuAdjustment::none;
    }

    // The Start menu is left to Windows until explorer.exe determines whether
    // the native taskbar on top is available. The native taskbar places it for
    // the primary taskbar location, so it's moved on monitors with a different
    // location.
    switch (GetNativeTaskbarOnTop()) {
        case NativeTaskbarOnTop::unknown:
            return StartMenuAdjustment::none;

        case NativeTaskbarOnTop::unavailable:
            return taskbarLocation == TaskbarLocation::top
                       ? StartMenuAdjustment::full
                       : StartMenuAdjustment::none;

        case NativeTaskbarOnTop::available:
            return taskbarLocation == g_settings.taskbarLocation
                       ? StartMenuAdjustment::none
                       : StartMenuAdjustment::position;
    }

    return StartMenuAdjustment::none;
}

void ApplyStyle() {
    g_inApplyStyle = true;

    HWND coreWnd = GetCoreWnd();
    HMONITOR monitor = MonitorFromWindow(coreWnd, MONITOR_DEFAULTTONEAREST);

    Wh_Log(L"Applying Start menu style for monitor %p", monitor);

    TaskbarLocation taskbarLocation = GetTaskbarLocationForMonitor(monitor);
    StartMenuAdjustment adjustment = GetStartMenuAdjustment(taskbarLocation);

    auto window = Window::Current();
    FrameworkElement content = window.Content().as<FrameworkElement>();

    winrt::hstring contentClassName = winrt::get_class_name(content);
    Wh_Log(L"Start menu content class name: %s", contentClassName.c_str());

    if (contentClassName == L"Windows.UI.Xaml.Controls.Canvas") {
        ApplyStyleClassicStartMenu(content, taskbarLocation, monitor,
                                   adjustment);
    } else if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content, taskbarLocation, adjustment);
    } else {
        Wh_Log(L"Error: Unsupported Start menu content class name");
    }

    g_inApplyStyle = false;
}

void Init() {
    if (g_visibilityChangedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    g_visibilityChangedToken = window.VisibilityChanged(
        [](winrt::Windows::Foundation::IInspectable const& sender,
           winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args) {
            Wh_Log(L"Window visibility changed: %d", args.Visible());
            if (args.Visible()) {
                ApplyStyle();
            }
        });

    ApplyStyle();
}

void Uninit() {
    if (!g_visibilityChangedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    window.VisibilityChanged(g_visibilityChangedToken);
    g_visibilityChangedToken = {};

    auto startSizingFrameDo = g_startSizingFrameWeakRef.get();
    if (startSizingFrameDo) {
        if (g_canvasTopPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::TopProperty(),
                g_canvasTopPropertyChangedToken);
            g_canvasTopPropertyChangedToken = 0;
        }

        if (g_canvasLeftPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::LeftProperty(),
                g_canvasLeftPropertyChangedToken);
            g_canvasLeftPropertyChangedToken = 0;
        }
    }

    g_startSizingFrameWeakRef = nullptr;

    auto frameRootDo = g_frameRootWeakRef.get();
    if (frameRootDo) {
        if (g_verticalAlignmentPropertyChangedToken) {
            frameRootDo.UnregisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                g_verticalAlignmentPropertyChangedToken);
            g_verticalAlignmentPropertyChangedToken = 0;
        }

        if (g_horizontalAlignmentPropertyChangedToken) {
            frameRootDo.UnregisterPropertyChangedCallback(
                FrameworkElement::HorizontalAlignmentProperty(),
                g_horizontalAlignmentPropertyChangedToken);
            g_horizontalAlignmentPropertyChangedToken = 0;
        }
    }

    g_frameRootWeakRef = nullptr;

    ApplyStyle();
}

void SettingsChanged() {
    ApplyStyle();
}

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t RoGetActivationFactory_Original;
HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING activatableClassId,
                                           REFIID iid,
                                           void** factory) {
    thread_local static bool isInHook;

    if (isInHook) {
        return RoGetActivationFactory_Original(activatableClassId, iid,
                                               factory);
    }

    isInHook = true;

    if (wcscmp(WindowsGetStringRawBuffer(activatableClassId, nullptr),
               L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        try {
            Init();
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }

    HRESULT ret =
        RoGetActivationFactory_Original(activatableClassId, iid, factory);

    isInHook = false;

    return ret;
}

}  // namespace StartMenuUI

void LoadSettings() {
    PCWSTR taskbarLocation = Wh_GetStringSetting(L"taskbarLocation");
    g_settings.taskbarLocation = TaskbarLocation::top;
    if (wcscmp(taskbarLocation, L"bottom") == 0) {
        g_settings.taskbarLocation = TaskbarLocation::bottom;
    }
    Wh_FreeStringSetting(taskbarLocation);

    PCWSTR taskbarLocationSecondary =
        Wh_GetStringSetting(L"taskbarLocationSecondary");
    g_settings.taskbarLocationSecondary = g_settings.taskbarLocation;
    if (wcscmp(taskbarLocationSecondary, L"top") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::top;
    } else if (wcscmp(taskbarLocationSecondary, L"bottom") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::bottom;
    } else if (wcscmp(taskbarLocationSecondary, L"left") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::left;
    } else if (wcscmp(taskbarLocationSecondary, L"right") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::right;
    }
    Wh_FreeStringSetting(taskbarLocationSecondary);

    g_settings.runningIndicatorsOnTop =
        Wh_GetIntSetting(L"runningIndicatorsOnTop");
    g_settings.startMenuAnimationAdjust =
        Wh_GetIntSetting(L"startMenuAnimationAdjust");
    g_settings.useNativeTaskbar = Wh_GetIntSetting(L"useNativeTaskbar");
}

// Explorer destroys the secondary taskbars when it reads that the taskbar isn't
// shown on all displays, and creates them again when it reads that it is. The
// Settings app has it read the value with the same setting change.
void RecreateSecondaryTaskbars(HWND hTaskbarWnd) {
    if (!FindCurrentProcessWindow(L"Shell_SecondaryTrayWnd")) {
        return;
    }

    Wh_Log(L"Recreating secondary taskbars");

    g_hidingSecondaryTaskbars = true;
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");

    // They might be destroyed asynchronously.
    for (int i = 0;
         i < 100 && FindCurrentProcessWindow(L"Shell_SecondaryTrayWnd"); i++) {
        Sleep(20);
    }

    g_hidingSecondaryTaskbars = false;
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

void ApplySettingsNative(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](PVOID) {
            try {
                UpdateNativeTaskListButtons();
                UpdateNativeTaskbarBackgrounds();
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        },
        nullptr);

    // Move the primary taskbar with the message the Settings app sends, which
    // also has the taskbar content read the location again.
    DWORD edge = g_unloading
                     ? GetStoredTaskbarLocation()
                     : TaskbarLocationToEdge(g_settings.taskbarLocation);
    SendMessage(hTaskbarWnd, 0x5CA, 6, edge);

    // Make the secondary taskbars reload their location.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");

    // Secondary taskbars only get a vertical layout when they're created.
    TaskbarLocation taskbarLocationSecondary = GetTaskbarLocationSecondary();
    auto previousTaskbarLocationSecondary =
        SetSecondaryTaskbarsLocation(taskbarLocationSecondary);
    if (!g_unloading &&
        (taskbarLocationSecondary == TaskbarLocation::left ||
         taskbarLocationSecondary == TaskbarLocation::right) &&
        previousTaskbarLocationSecondary != taskbarLocationSecondary) {
        RecreateSecondaryTaskbars(hTaskbarWnd);
    }
}

void ApplySettings() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    if (g_hasNativeTaskbarOnTop) {
        ApplySettingsNative(hTaskbarWnd);
        return;
    }

    g_applyingSettings = true;

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    g_applyingSettings = false;

    // Update the taskbar location that affects the jump list animations.
    auto monitorEnumProc = [hTaskbarWnd](HMONITOR hMonitor) -> BOOL {
        PostMessage(hTaskbarWnd, 0x5C3, ABE_BOTTOM, (WPARAM)hMonitor);
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));
}

bool HookSystemTraySymbols(HMODULE module) {
    // The native taskbar on top lays out the system tray for its location.
    if (g_hasNativeTaskbarOnTop) {
        return true;
    }

    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::TextIconContent::ShowContextMenu(void))"},
            &TextIconContent_ShowContextMenu_Original,
            TextIconContent_ShowContextMenu_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::ShowContextMenu(void))"},
            &DateTimeIconContent_ShowContextMenu_Original,
            DateTimeIconContent_ShowContextMenu_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbolsNative(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarBackground::Location(enum winrt::WindowsUdk::UI::Shell::TaskbarLocation))"},
            &TaskbarBackground_Location_Original,
            TaskbarBackground_Location_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module,
                               bool hookSystemTraySymbolsInline) {
    if (g_hasNativeTaskbarOnTop) {
        return HookTaskbarViewDllSymbolsNative(module);
    }

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
            &TaskbarController_OnGroupingModeChanged,
            nullptr,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
            &TaskbarFrame_TaskbarFrame_Original,
            TaskbarFrame_TaskbarFrame_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowFlyoutModel::Show(void))"},
            &OverflowFlyoutModel_Show_Original,
            OverflowFlyoutModel_Show_Hook,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::UpdateFlyoutPosition(void))",
            },
            &FlyoutFrame_UpdateFlyoutPosition_Original,
            FlyoutFrame_UpdateFlyoutPosition_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_Controls_Primitives_IFlyoutBase5<struct winrt::Windows::UI::Xaml::Controls::MenuFlyout>::ShowAt(struct winrt::Windows::UI::Xaml::DependencyObject const &,struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions const &)const )"},
            &MenuFlyout_ShowAt_Original,
            MenuFlyout_ShowAt_Hook,
        },
    };

    // On older Taskbar.View.dll versions (before the SystemTray types moved out
    // into SystemTray.dll), these SystemTray symbols live in Taskbar.View.dll
    // itself, so include them in the same hook batch when
    // hookSystemTraySymbolsInline is set.

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooksSystemTray[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::TextIconContent::ShowContextMenu(void))"},
            &TextIconContent_ShowContextMenu_Original,
            TextIconContent_ShowContextMenu_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::ShowContextMenu(void))"},
            &DateTimeIconContent_ShowContextMenu_Original,
            DateTimeIconContent_ShowContextMenu_Hook,
        },
    };

    // Alias for the extract_mod_symbols.py script.
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[  //
        ARRAYSIZE(symbolHooks) + ARRAYSIZE(symbolHooksSystemTray)];
    int index = 0;

    for (auto& hook : symbolHooks) {
        allHooks[index++] = std::move(hook);
    }

    if (hookSystemTraySymbolsInline) {
        for (auto& hook : symbolHooksSystemTray) {
            allHooks[index++] = std::move(hook);
        }
    }

    if (!HookSymbols(module, allHooks, index)) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // treat Taskbar.View.dll as the host at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfSystemTrayOrTaskbarView(HMODULE module,
                                                 LPCWSTR lpLibFileName) {
    // SystemTray.dll - skipped here when the resolved module is actually an
    // older Taskbar.View.dll (the block below hooks both in a single batch).
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        module != GetTaskbarViewModuleHandle() &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        // If SystemTray.dll wasn't loaded above and this Taskbar.View.dll is an
        // older version that hosts SystemTray symbols inline, hook them in the
        // same batch.
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == module &&
            !g_systemTrayModuleHooked.exchange(true);

        if (HookTaskbarViewDllSymbols(module, hookSystemTraySymbolsInline)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfSystemTrayOrTaskbarView(module, lpLibFileName);
    }

    return module;
}

void* wil_Feature_59213768_GetImpl_impl;

using WilFeatureImpl_IsEnabled_t = bool(WINAPI*)(void* pThis,
                                                 int reportingKind);
WilFeatureImpl_IsEnabled_t WilFeatureImpl_59213768_IsEnabled;

void* TrayUI_VerifySize_WithoutMonitor;
void* TrayUI__HandleSizing_WithoutMonitor;

bool IsNativeTaskbarOnTopEnabled() {
    if (wil_Feature_59213768_GetImpl_impl &&
        WilFeatureImpl_59213768_IsEnabled) {
        constexpr int kReportingKindNone = 0;
        return WilFeatureImpl_59213768_IsEnabled(
            wil_Feature_59213768_GetImpl_impl, kReportingKindNone);
    }

    // The feature added a monitor parameter to these functions, so builds
    // without the flag and without the old overloads always have the feature.
    return !TrayUI_VerifySize_WithoutMonitor &&
           !TrayUI__HandleSizing_WithoutMonitor;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    // Hooks for both taskbar implementations are set, since the symbols of the
    // module are looked up in a single call. Hooks that would change the other
    // implementation pass through for it.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *))"},
            &TrayUI_GetStuckRectForMonitor_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetDockedRect(struct tagRECT *,int))"},
            &TrayUI_GetDockedRect_Original,
            TrayUI_GetDockedRect_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::MakeStuckRect(struct tagRECT *,struct tagRECT const *,struct tagSIZE,unsigned int))"},
            &TrayUI_MakeStuckRect_Original,
            TrayUI_MakeStuckRect_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetStuckInfo(struct tagRECT *,unsigned int *))"},
            &TrayUI_GetStuckInfo_Original,
            TrayUI_GetStuckInfo_Hook,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &CTaskListWnd_ComputeJumpViewPosition_Original,
            CTaskListWnd_ComputeJumpViewPosition_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))"},
            &CTaskListThumbnailWnd_DisplayUI_Original,
            CTaskListThumbnailWnd_DisplayUI_Hook,
            true,  // Classic thumbnails, removed in or near 10.0.26100.8491.
        },
        {
            {LR"(public: virtual void __cdecl CTaskListThumbnailWnd::LayoutThumbnails(void))"},
            &CTaskListThumbnailWnd_LayoutThumbnails_Original,
            CTaskListThumbnailWnd_LayoutThumbnails_Hook,
            true,  // Classic thumbnails, removed in or near 10.0.26100.8491.
        },
        {
            {LR"(public: __cdecl winrt::Windows::Internal::Shell::XamlExplorerHost::XamlExplorerHostWindow::XamlExplorerHostWindow(unsigned int,struct winrt::Windows::Foundation::Rect const &,unsigned int))"},
            &XamlExplorerHostWindow_XamlExplorerHostWindow_Original,
            XamlExplorerHostWindow_XamlExplorerHostWindow_Hook,
            true,  // Inlined and no longer needed in or near 10.0.26100.8491.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"},
            &ITaskbarSettings_get_Alignment_Original,
            ITaskbarSettings_get_Alignment_Hook,
        },
        {
            {
                LR"(class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_59213768> `private: static class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_59213768> & __cdecl wil::Feature<struct __WilFeatureTraits_Feature_59213768>::GetImpl(void)'::`2'::impl)",
                LR"(class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_59213768> `private: static class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_59213768> & __cdecl wil::Feature<struct __WilExternalFeatureTraits_Feature_59213768>::GetImpl(void)'::`2'::impl)",
            },
            &wil_Feature_59213768_GetImpl_impl,
            nullptr,
            true,
        },
        {
            {
                LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_59213768>::__private_IsEnabled(enum wil::ReportingKind))",
                LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_59213768>::__private_IsEnabled(enum wil::ReportingKind))",
            },
            &WilFeatureImpl_59213768_IsEnabled,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::VerifySize(bool,bool))"},
            &TrayUI_VerifySize_WithoutMonitor,
            nullptr,
            true,  // Only in builds from before the native taskbar on top.
        },
        {
            {LR"(public: int __cdecl TrayUI::_HandleSizing(unsigned __int64,struct tagRECT *,unsigned int,bool))"},
            &TrayUI__HandleSizing_WithoutMonitor,
            nullptr,
            true,  // Only in builds from before the native taskbar on top.
        },
        {
            {LR"(public: void __cdecl TrayUI::_GetSaveStateAndInitRects(void))"},
            &TrayUI__GetSaveStateAndInitRects_Original,
            TrayUI__GetSaveStateAndInitRects_Hook,
            true,  // Only needed for the native taskbar on top.
        },
        {
            {LR"(private: long __cdecl CSecondaryTray::_LoadSettings(void))"},
            &CSecondaryTray__LoadSettings_Original,
            CSecondaryTray__LoadSettings_Hook,
            true,  // Only needed for the native taskbar on top.
        },
        {
            {LR"(public: enum winrt::WindowsUdk::UI::Shell::TaskbarLocation __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::Location(void))"},
            &TaskbarSettings_Location_Original,
            TaskbarSettings_Location_Hook,
            true,  // Only needed for the native taskbar on top.
        },
        {
            {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::TaskbarSettings(struct ITrayComponentHost *,struct ITaskListWndSite *))"},
            &TaskbarSettings_TaskbarSettings_Original,
            TaskbarSettings_TaskbarSettings_Hook,
            true,  // Only needed for the native taskbar on top.
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    g_hasNativeTaskbarOnTop = IsNativeTaskbarOnTopEnabled();
    Wh_Log(L"Native taskbar on top: %d", g_hasNativeTaskbarOnTop);

    if (g_hasNativeTaskbarOnTop && !g_settings.useNativeTaskbar) {
        Wh_Log(L"Not using native taskbar on top due to settings");
        g_hasNativeTaskbarOnTop = false;
    }

    // The native taskbar doesn't implement auto-hide for non-bottom taskbars.
    // If auto-hide is enabled, the native taskbar isn't used to avoid breaking
    // the auto-hide behavior.
    if (g_hasNativeTaskbarOnTop && IsStoredTaskbarAutoHideEnabled()) {
        Wh_Log(L"Not using native taskbar on top with auto-hide");
        g_hasNativeTaskbarOnTop = false;
    }

    if (g_hasNativeTaskbarOnTop &&
        (!TrayUI__GetSaveStateAndInitRects_Original ||
         !CSecondaryTray__LoadSettings_Original ||
         !TaskbarSettings_Location_Original)) {
        Wh_Log(L"Error: Missing native taskbar on top symbols");
        return false;
    }

    if (g_sessionNativeTaskbarOnTop) {
        *g_sessionNativeTaskbarOnTop = g_hasNativeTaskbarOnTop
                                           ? NativeTaskbarOnTop::available
                                           : NativeTaskbarOnTop::unavailable;
    }

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_target = Target::Explorer;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"StartMenuExperienceHost.exe") ==
                    0) {
                    g_target = Target::StartMenuExperienceHost;
                } else if (_wcsicmp(moduleFileName,
                                    L"ShellExperienceHost.exe") == 0) {
                    g_target = Target::ShellExperienceHost;
                } else if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId < ARRAYSIZE(g_nativeTaskbarOnTop)) {
        g_sessionNativeTaskbarOnTop = &g_nativeTaskbarOnTop[sessionId];
    } else {
        Wh_Log(L"No shared native taskbar on top state for this session");
    }

    if (g_target == Target::StartMenuExperienceHost) {
        HMODULE winrtModule =
            GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
        auto pRoGetActivationFactory =
            (decltype(&RoGetActivationFactory))GetProcAddress(
                winrtModule, "RoGetActivationFactory");
        WindhawkUtils::SetFunctionHook(
            pRoGetActivationFactory, StartMenuUI::RoGetActivationFactory_Hook,
            &StartMenuUI::RoGetActivationFactory_Original);

        return TRUE;
    }

    if (g_target == Target::ShellExperienceHost ||
        g_target == Target::ShellHost) {
        HookRegGetValueW();
        return TRUE;
    }

    if (HMODULE kernel32Module = LoadLibraryEx(L"kernel32.dll", nullptr,
                                               LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pGetThreadDescription = (GetThreadDescription_t)GetProcAddress(
            kernel32Module, "GetThreadDescription");
    }

    if (HMODULE user32Module = LoadLibraryEx(L"user32.dll", nullptr,
                                             LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pGetWindowBand =
            (GetWindowBand_t)GetProcAddress(user32Module, "GetWindowBand");
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        // For older Taskbar.View.dll builds the resolved module is the same
        // Taskbar.View.dll handle - in that case, defer hooking SystemTray
        // symbols until HookTaskbarViewDllSymbols runs below so it can do them
        // in a single HookSymbols batch.
        if (systemTrayModule != GetTaskbarViewModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                return FALSE;
            }
        }
    }

    bool delayLoadingNeeded = false;

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == taskbarViewModule;
        if (hookSystemTraySymbolsInline) {
            g_systemTrayModuleHooked = true;
        }
        if (!HookTaskbarViewDllSymbols(taskbarViewModule,
                                       hookSystemTraySymbolsInline)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }

    // SystemTray.dll may load after Taskbar.View.dll on newer Windows 11
    // builds, so make sure the LoadLibraryExW hook is installed to catch it.
    if (!g_systemTrayModuleHooked) {
        delayLoadingNeeded = true;
    }

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    // The native taskbar on top places its flyouts and windows itself, other
    // than the Start menu on monitors with a different taskbar location.
    if (g_hasNativeTaskbarOnTop) {
        HookRegGetValueW();
    } else {
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                       &CreateWindowExW_Original);

        WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                       &SetWindowPos_Original);

        WindhawkUtils::SetFunctionHook(MoveWindow, MoveWindow_Hook,
                                       &MoveWindow_Original);

        WindhawkUtils::SetFunctionHook(MapWindowPoints, MapWindowPoints_Hook,
                                       &MapWindowPoints_Original);
    }

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::SetFunctionHook(pDwmSetWindowAttribute,
                                           DwmSetWindowAttribute_Hook,
                                           &DwmSetWindowAttribute_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
        if (!g_systemTrayModuleHooked) {
            if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
                if (systemTrayModule != GetTaskbarViewModuleHandle() &&
                    !g_systemTrayModuleHooked.exchange(true)) {
                    Wh_Log(L"Got system tray module");

                    if (HookSystemTraySymbols(systemTrayModule)) {
                        Wh_ApplyHookOperations();
                    }
                }
            }
        }

        if (!g_taskbarViewDllLoaded) {
            if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
                if (!g_taskbarViewDllLoaded.exchange(true)) {
                    Wh_Log(L"Got Taskbar.View.dll");

                    bool hookSystemTraySymbolsInline =
                        !g_systemTrayModuleHooked &&
                        GetSystemTrayModuleHandle() == taskbarViewModule &&
                        !g_systemTrayModuleHooked.exchange(true);

                    if (HookTaskbarViewDllSymbols(
                            taskbarViewModule, hookSystemTraySymbolsInline)) {
                        Wh_ApplyHookOperations();
                    }
                }
            }
        }

        ApplySettings();
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Initializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Init(); }, nullptr);
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        ApplySettings();
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Uninitializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Uninit(); }, nullptr);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevUseNativeTaskbar = g_settings.useNativeTaskbar;

    LoadSettings();

    if (g_target == Target::Explorer) {
        // The taskbar implementation is chosen when the mod is initialized.
        if (g_settings.useNativeTaskbar != prevUseNativeTaskbar) {
            *bReload = TRUE;
            return TRUE;
        }

        ApplySettings();
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Applying settings - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::SettingsChanged(); },
                nullptr);
        }
    }

    return TRUE;
}
