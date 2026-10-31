// ==WindhawkMod==
// @id              fix-missing-taskbar-icons
// @name            Fix missing taskbar icons after sleep
// @description     On resume from sleep or hibernate, momentarily pulses the taskbar height by 1px and reverts it, forcing a re-layout that brings back icons that go missing on wake (Windows 11)
// @version         2.0.0
// @author          therealgorgan
// @github          https://github.com/therealgorgan
// @license         GPL-3.0-or-later
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// This mod is a stripped-down fork of the "Taskbar height and icon size" mod by
// m417z (https://github.com/m417z), reusing its taskbar-height hooking code. It
// keeps only the height plumbing and adds a resume-triggered refresh pulse.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Fix missing taskbar icons after sleep

Works around a Windows 11 bug where some taskbar icons go missing after the PC
wakes from **sleep or hibernate**, normally requiring a manual `explorer.exe`
restart.

Instead of restarting explorer, this mod reproduces the exact refresh mechanism
that the *Taskbar height and icon size* mod (m417z) uses: when triggered, it
**momentarily changes the taskbar height by 1 pixel and immediately reverts it**.
That tiny real change forces the taskbar's XAML layout to fully re-measure and
re-render, which rebuilds the missing icons. A no-op message (which an earlier
version tried) does *not* work — the layout has to actually change.

- In steady state the mod changes **nothing** — all of its hooks pass straight
  through to Windows, so your taskbar height, icon size, and dynamic icon
  scaling are untouched.
- On every resume it fires the 1px pulse a few times on a staggered schedule
  (configurable), to catch the taskbar whenever it settles into the broken state.
- Toggling the mod on triggers one pulse immediately, so the on/off switch
  doubles as a manual "refresh now" button for testing.

This is a stripped-down fork of the *Taskbar height and icon size* mod: only the
taskbar-height plumbing is kept (no icon-size, button-width, or search hooks).

Only Windows 11 is supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Delay1Ms: 4000
  $name: First pulse delay (ms)
  $description: >-
    Milliseconds to wait after resume before the first refresh pulse. 0 to skip.
- Delay2Ms: 9000
  $name: Second pulse delay (ms)
  $description: >-
    Milliseconds after resume for a second pulse, to catch a late-breaking
    taskbar. 0 to skip.
- Delay3Ms: 16000
  $name: Third pulse delay (ms)
  $description: >-
    Milliseconds after resume for a third pulse. 0 to skip.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <limits>
#include <regex>
#include <string>
#include <string_view>

using namespace winrt::Windows::UI::Xaml;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

// Windhawk's bundled MinGW headers declare RegisterSuspendResumeNotification
// but are missing the callback-registration types that go with it.
#ifndef DEVICE_NOTIFY_CALLBACK
#define DEVICE_NOTIFY_CALLBACK 0x00000002

typedef ULONG(CALLBACK* PDEVICE_NOTIFY_CALLBACK_ROUTINE)(PVOID Context,
                                                         ULONG Type,
                                                         PVOID Setting);

typedef struct _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS {
    PDEVICE_NOTIFY_CALLBACK_ROUTINE Callback;
    PVOID Context;
} DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS, *PDEVICE_NOTIFY_SUBSCRIBE_PARAMETERS;
#endif

struct {
    int delay1Ms;
    int delay2Ms;
    int delay3Ms;
} g_settings;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

// 0 = pass through to native height. >0 = momentarily override (during a pulse).
int g_originalTaskbarHeight;
int g_taskbarHeight;
bool g_inSystemTrayController_UpdateFrameSize;

double* double_48_value_Original;

// Resume handling.
std::atomic<bool> g_refreshInProgress;
HANDLE g_stopEvent;
HPOWERNOTIFY g_powerNotify;

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

bool IsVerticalTaskbar() {
    APPBARDATA appBarData = {
        .cbSize = sizeof(APPBARDATA),
    };
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        return false;
    }

    return appBarData.uEdge == ABE_LEFT || appBarData.uEdge == ABE_RIGHT;
}

// ---------------------------------------------------------------------------
// Taskbar height hooks (kept from the original mod). All are gated so that in
// steady state (g_taskbarHeight == 0, g_applyingSettings == false) they pass
// straight through to Windows and change nothing. They only engage during a
// refresh pulse.
// ---------------------------------------------------------------------------

using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis,
                                          HMONITOR monitor,
                                          SIZE* size);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    TrayUI_GetMinSize_Original(pThis, monitor, size);

    if (!IsVerticalTaskbar() && g_taskbarHeight) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);

        size->cy = MulDiv(g_taskbarHeight, dpiY, 96);
    }
}

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    // During a pulse, force the notification-area icon storage to be recreated
    // so missing tray icons are re-added.
    if (g_applyingSettings) {
        return true;
    }

    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}

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
    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);

    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}

using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

using SystemTraySecondaryController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t
    SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI
SystemTraySecondaryController_GetFrameSize_Hook(void* pThis,
                                                int enumTaskbarSize) {
    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}

using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    if (!g_originalTaskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

#ifdef _M_ARM64
// On ARM64, TaskbarConfiguration::GetFrameSize is inlined into UpdateFrameSize,
// so it can't be hooked directly. Hook UpdateFrameSize to capture the frame-size
// field, and override it from the Event callback that runs inside it.
thread_local double* g_TaskbarConfiguration_UpdateFrameSize_frameSize;

using TaskbarConfiguration_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_SymbolAddress;

LONG GetFrameSizeOffset() {
    static LONG frameSizeOffset = []() -> LONG {
        if (!TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
            Wh_Log(L"Error: UpdateFrameSize symbol address is null");
            return 0;
        }

        // str d16, [x19, #0x50]
        const DWORD* start =
            (const DWORD*)TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
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

            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"frameSizeOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }

        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();

    return frameSizeOffset;
}

void TaskbarConfiguration_UpdateFrameSize_InitOffsets() {
    GetFrameSizeOffset();
}

TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    LONG frameSizeOffset = GetFrameSizeOffset();
    if (!frameSizeOffset) {
        Wh_Log(L"Error: frameSizeOffset is invalid");
        TaskbarConfiguration_UpdateFrameSize_Original(pThis);
        return;
    }

    g_TaskbarConfiguration_UpdateFrameSize_frameSize =
        (double*)((BYTE*)pThis + frameSizeOffset);

    TaskbarConfiguration_UpdateFrameSize_Original(pThis);

    g_TaskbarConfiguration_UpdateFrameSize_frameSize = nullptr;
}

using Event_operator_call_t = void(WINAPI*)(void* pThis);
Event_operator_call_t Event_operator_call_Original;
void WINAPI Event_operator_call_Hook(void* pThis) {
    if (g_TaskbarConfiguration_UpdateFrameSize_frameSize) {
        if (!g_originalTaskbarHeight) {
            g_originalTaskbarHeight =
                *g_TaskbarConfiguration_UpdateFrameSize_frameSize;
        }

        if (!IsVerticalTaskbar() && g_taskbarHeight) {
            *g_TaskbarConfiguration_UpdateFrameSize_frameSize = g_taskbarHeight;
        }
    }

    Event_operator_call_Original(pThis);
}
#endif  // _M_ARM64

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;

LONG GetLastHeightOffset() {
    static LONG lastHeightOffset = []() -> LONG {
        if (!SystemTrayController_UpdateFrameSize_SymbolAddress) {
            Wh_Log(L"Error: UpdateFrameSize symbol address is null");
            return 0;
        }

#if defined(_M_X64)
        // 66 0f 2e b3 b0 00 00 00 UCOMISD    uVar4,qword ptr [RBX + 0xb0]
        // 7a 4c                   JP         LAB
        // 75 4a                   JNZ        LAB
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x400;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E &&
                (p[3] & 0xC0) == 0x80 && p[8] == 0x7A &&
                (p[10] == 0x74 || p[10] == 0x75 ||
                 (p[10] == 0x0F && (p[11] == 0x84 || p[11] == 0x85)))) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
#elif defined(_M_ARM64)
        // fd405a70 ldr  d16,[x19,#0xB0]
        // 1e702000 fcmp d0,d16
        // 54000080 b.eq [...]
        const DWORD* start =
            (const DWORD*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x100;
        std::regex regex1(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        std::regex regex2(R"(fcmp\s+d\d+, d\d+)");
        std::regex regex3(R"(b\.eq\s+0x[0-9a-f]+)");
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

            WH_DISASM_RESULT result2;
            if (!Wh_Disasm((void*)(p + 1), &result2)) {
                break;
            }
            std::string_view s2 = result2.text;
            if (!std::regex_match(s2.begin(), s2.end(), regex2)) {
                continue;
            }

            WH_DISASM_RESULT result3;
            if (!Wh_Disasm((void*)(p + 2), &result3)) {
                break;
            }
            std::string_view s3 = result3.text;
            if (!std::regex_match(s3.begin(), s3.end(), regex3)) {
                continue;
            }

            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"lastHeightOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();

    return lastHeightOffset;
}

void SystemTrayController_UpdateFrameSize_InitOffsets() {
    GetLastHeightOffset();
}

SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    if (!g_applyingSettings || IsVerticalTaskbar()) {
        SystemTrayController_UpdateFrameSize_Original(pThis);
        return;
    }

    LONG lastHeightOffset = GetLastHeightOffset();
    if (lastHeightOffset) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    }

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTrayController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    if (!g_applyingSettings) {
        SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
        return;
    }

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (!IsVerticalTaskbar() && g_inSystemTrayController_UpdateFrameSize) {
        // Set the system tray height to NaN, otherwise it may not match the
        // taskbar height during the pulse.
        value = std::numeric_limits<double>::quiet_NaN();
    }

    SystemTrayFrame_Height_Original(pThis, value);
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);

    g_pendingMeasureOverride = false;

    g_hookCallCounter--;

    return ret;
}

using TaskbarFrame_MaxHeight_double_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_MaxHeight_double_t TaskbarFrame_MaxHeight_double_Original;

using TaskbarFrame_Height_double_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_Height_double_t TaskbarFrame_Height_double_Original;
void WINAPI TaskbarFrame_Height_double_Hook(void* pThis, double value) {
    if (!g_applyingSettings || IsVerticalTaskbar()) {
        TaskbarFrame_Height_double_Original(pThis, value);
        return;
    }

    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }

    TaskbarFrame_Height_double_Original(pThis, value);
}

void* TaskbarController_OnGroupingModeChanged_Original;

LONG GetTaskbarFrameOffset() {
    static LONG taskbarFrameOffset = []() -> LONG {
        if (!TaskbarController_OnGroupingModeChanged_Original) {
            Wh_Log(L"Error: OnGroupingModeChanged is null");
            return 0;
        }

#if defined(_M_X64)
        // 48:83EC 28           | sub rsp,28
        // 48:8B81 88020000     | mov rax,qword ptr ds:[rcx+288]
        // or 4C:8B81 80020000  | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p =
            (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#elif defined(_M_ARM64)
        // f9414500 ldr x0,[x8,#0x288]
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged_Original;
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

            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();

    return taskbarFrameOffset;
}

void TaskbarController_OnGroupingModeChanged_InitOffsets() {
    GetTaskbarFrameOffset();
}

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    if (!g_applyingSettings || IsVerticalTaskbar()) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    LONG taskbarFrameOffset = GetTaskbarFrameOffset();
    if (!taskbarFrameOffset) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());

    TaskbarController_UpdateFrameHeight_Original(pThis);

    // Adjust parent grid height if needed.
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            contentGrid.Height(height);
        }
    }
}

using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);

    // Used to position secondary taskbars during the pulse.
    if (dwMessage == ABM_QUERYPOS && ret && !IsVerticalTaskbar() &&
        g_taskbarHeight) {
        pData->rc.top =
            pData->rc.bottom -
            MulDiv(g_taskbarHeight, GetDpiForWindow(pData->hWnd), 96);
    }

    return ret;
}

// ---------------------------------------------------------------------------
// The refresh pulse itself.
// ---------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool ProtectAndMemcpy(DWORD protect, void* dst, const void* src, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
        return false;
    }

    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}

void WaitForMeasureOverride() {
    for (int i = 0; i < 100; i++) {
        if (!g_pendingMeasureOverride) {
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 100) == WAIT_OBJECT_0) {
            break;
        }
    }
}

// Momentarily change the taskbar height by 1px and revert it, forcing the XAML
// taskbar to re-measure and re-render. This is the exact refresh mechanism the
// original Taskbar height/icon-size mod uses.
void PulseRefresh() {
    if (g_unloading || IsVerticalTaskbar()) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    int height = g_originalTaskbarHeight;
    if (height < 3) {
        RECT taskbarRect{};
        if (GetWindowRect(hTaskbarWnd, &taskbarRect)) {
            height = MulDiv(taskbarRect.bottom - taskbarRect.top, 96,
                            GetDpiForWindow(hTaskbarWnd));
        }
    }
    if (height < 3) {
        height = 48;
    }

    Wh_Log(L"Pulsing taskbar refresh (height=%d)", height);

    g_applyingSettings = true;

    // Step 1: shrink by 1px to force a real layout change.
    g_pendingMeasureOverride = true;
    g_taskbarHeight = height - 1;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
        double tempHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original, &tempHeight,
                         sizeof(double));
    }
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    WaitForMeasureOverride();

    // Step 2: revert to native height.
    g_pendingMeasureOverride = true;
    g_taskbarHeight = 0;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
        double tempHeight = height;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original, &tempHeight,
                         sizeof(double));
    }
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    WaitForMeasureOverride();

    // Resync the legacy task band, if present (absent on newer XAML builds).
    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }

    g_applyingSettings = false;
}

DWORD WINAPI ResumeRefreshThread(LPVOID /*param*/) {
    int delays[] = {g_settings.delay1Ms, g_settings.delay2Ms,
                    g_settings.delay3Ms};

    int elapsed = 0;
    for (int delay : delays) {
        if (delay <= 0) {
            continue;
        }

        int wait = delay - elapsed;
        if (wait < 0) {
            wait = 0;
        }
        elapsed = delay;

        if (WaitForSingleObject(g_stopEvent, wait) == WAIT_OBJECT_0 ||
            g_unloading) {
            break;
        }

        PulseRefresh();
    }

    g_refreshInProgress = false;
    return 0;
}

void TriggerRefresh() {
    if (g_unloading) {
        return;
    }

    if (g_refreshInProgress.exchange(true)) {
        return;  // A pulse sequence is already running.
    }

    HANDLE hThread =
        CreateThread(nullptr, 0, ResumeRefreshThread, nullptr, 0, nullptr);
    if (hThread) {
        CloseHandle(hThread);
    } else {
        g_refreshInProgress = false;
    }
}

ULONG CALLBACK PowerNotifyCallback(PVOID /*context*/,
                                   ULONG type,
                                   PVOID /*setting*/) {
    // PBT_APMRESUMEAUTOMATIC fires on resume from BOTH sleep and hibernate.
    if (type == PBT_APMRESUMEAUTOMATIC) {
        Wh_Log(L"Resume detected");
        TriggerRefresh();
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Symbol hooking / module orchestration (height-only subset of the original).
// ---------------------------------------------------------------------------

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually; we need the symbol address.
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed (SystemTray)");
        return false;
    }

    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module,
                               bool hookSystemTraySymbolsInline) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            // For Windows 11 version 21H2.
            {LR"(__real@4048000000000000)"},
            &double_48_value_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &TaskbarConfiguration_GetFrameSize_Original,
            TaskbarConfiguration_GetFrameSize_Hook,
            true,
        },
#ifdef _M_ARM64
        // On ARM64, GetFrameSize is inlined into UpdateFrameSize, which is
        // hooked manually below; the Event callback applies the override.
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateFrameSize(void))"},
            &TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
        },
        {
            {LR"(public: void __cdecl winrt::event<struct winrt::delegate<> >::operator()<>(void))"},
            &Event_operator_call_Original,
            Event_operator_call_Hook,
        },
#endif
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
            &TaskbarFrame_MaxHeight_double_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                // Windows 11 version 21H2.
                LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
            },
            &TaskbarFrame_Height_double_Original,
            TaskbarFrame_Height_double_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
            &TaskbarController_OnGroupingModeChanged_Original,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
        },
    };

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooksSystemTray[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,
        },
    };

    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[ARRAYSIZE(symbolHooks) +
                         ARRAYSIZE(symbolHooksSystemTray)];
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
        Wh_Log(L"HookSymbols failed (Taskbar.View)");
        return false;
    }

#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
        TaskbarConfiguration_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif

    if (hookSystemTraySymbolsInline &&
        SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }

    if (TaskbarController_OnGroupingModeChanged_Original) {
        TaskbarController_OnGroupingModeChanged_InitOffsets();
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))"},
            &IconContainer_IsStorageRecreationRequired_Original,
            IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))"},
            &TrayUI_GetMinSize_Original,
            TrayUI_GetMinSize_Hook,
            true,
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
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar.dll)");
        return false;
    }

    return true;
}

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
            // Starting with Taskbar.View.dll 2604.x, the SystemTray types moved
            // out into SystemTray.dll.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

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

        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == module &&
            !g_systemTrayModuleHooked.exchange(true);

        if (HookTaskbarViewDllSymbols(module, hookSystemTraySymbolsInline)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

void LoadSettings() {
    g_settings.delay1Ms = Wh_GetIntSetting(L"Delay1Ms");
    g_settings.delay2Ms = Wh_GetIntSetting(L"Delay2Ms");
    g_settings.delay3Ms = Wh_GetIntSetting(L"Delay3Ms");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    bool delayLoadingNeeded = false;

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        if (systemTrayModule != GetTaskbarViewModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                return FALSE;
            }
        }
    }

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

    if (!g_systemTrayModuleHooked) {
        delayLoadingNeeded = true;
    }

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    WindhawkUtils::Wh_SetFunctionHookT(SHAppBarMessage, SHAppBarMessage_Hook,
                                       &SHAppBarMessage_Original);

    // Register for resume-from-low-power notifications (sleep AND hibernate).
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS params = {};
    params.Callback = PowerNotifyCallback;
    params.Context = nullptr;
    g_powerNotify = RegisterSuspendResumeNotification((HANDLE)&params,
                                                      DEVICE_NOTIFY_CALLBACK);
    if (!g_powerNotify) {
        Wh_Log(L"RegisterSuspendResumeNotification failed: %u", GetLastError());
        // Not fatal: the manual on/off "refresh now" still works.
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // One pulse on enable, so the on/off toggle doubles as "refresh now".
    TriggerRefresh();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_powerNotify) {
        UnregisterSuspendResumeNotification(g_powerNotify);
        g_powerNotify = nullptr;
    }

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Wait for any in-flight pulse sequence to finish.
    for (int i = 0; i < 100 && g_refreshInProgress; i++) {
        Sleep(50);
    }

    // Make sure no in-flight height override remains.
    g_taskbarHeight = 0;

    // Wait for any MeasureOverride hook calls to drain.
    while (g_hookCallCounter > 0) {
        Sleep(100);
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
