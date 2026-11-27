// ==WindhawkMod==
// @id              alt-tab-monitor-switcher
// @name            Alt+Tab monitor switcher
// @name:ko         Alt+Tab 디스플레이 전환
// @description     Start Alt+Tab on the focused app's monitor, then hold Alt and press D to cycle monitor-specific window lists.
// @description:ko  현재 활성 앱의 화면에서 Alt+Tab을 시작하고, Alt를 누른 채 D로 디스플레이별 창 목록을 전환합니다.
// @version         1.0.3
// @author          Choihyunseok1
// @github          https://github.com/Choihyunseok1
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lversion -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab monitor switcher

**Alt+Tab starts on the focused application's monitor**, regardless of where the
mouse is. Keep holding **Alt**, then press **D** to switch to another monitor's
window list. Moving the mouse doesn't change the selected monitor.

## What this adds to Alt+Tab per monitor

[Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor) chooses the
monitor using the cursor's position. Its Win+Alt+Tab shortcut can show windows
from all monitors together.

This mod starts from the **focused application's monitor** and adds **keyboard
selection of the monitor during the same Alt+Tab session**. Pressing D moves the
switcher to the next monitor and shows only that monitor's windows. The selected
monitor stays fixed until you press D or close the switcher. You can browse a
different display without moving the mouse or searching through a combined list.

## Controls

- Press **Alt+Tab** to show windows on the focused application's monitor.
  - Keep holding **Alt** and press **D** to switch to the next monitor.
  - Press **Shift+D** to switch to the previous monitor.
  - Use **Tab**, **Shift+Tab**, or the arrow keys to select a window.
    - Release **Alt** to activate the selected window. **Esc** cancels as usual.
- In sticky **Ctrl+Alt+Tab** mode, press **D** or **Shift+D**, then **Enter** to
  activate a window.

- Monitors are ordered from left to right, then top to bottom.
  - Monitors without windows in the native Alt+Tab list are skipped.
- The cursor and application windows are never moved.
  - Each new Alt+Tab session starts from the focused application's monitor.
    - Starting from the cursor's monitor is an optional setting.
- D is intercepted only while using the native switcher or changing monitors.
  - Normal D and Alt+D shortcuts in applications keep working.

## Compatibility and settings

The switcher briefly closes and reopens when changing monitors so that Windows
rebuilds its window list. Releasing Alt during that transition cancels the reopen.

**Disable Alt+Tab per monitor before enabling this mod**, as both hook the same
Windows functions. Other Alt+Tab replacements are not supported.

Alt+Tab still initializes from the focused monitor if Windows bypasses the
keyboard hook, for example when an elevated application has focus. D cycling
is disabled when Windows would prevent Explorer from sending input back to that
application. The switcher stays open, and ordinary window selection still works.

Both Alt+Tab and Win+Alt+Tab can be independently configured with:

- **Display location**: Primary monitor or the session's selected monitor.
- **Windows to show**: All monitors, the selected monitor only, or the selected
  monitor with an exception for the primary monitor.

## 한국어

기존 **Alt+Tab per monitor**는 마우스 커서가 있는 화면을 기준으로 창 목록을
보여줍니다. 이 모드는 **현재 활성 프로그램이 있는 화면에서 시작**하고,
**Alt를 누른 채 D로 대상 디스플레이를 직접 전환**합니다. 목록을 연 상태에서
마우스를 다른 화면으로 옮겨도 대상 화면은 바뀌지 않습니다.

- **Alt+Tab**으로 창 목록을 연 뒤 **Alt를 계속 누릅니다**.
  - **D**: 다음 디스플레이의 창 목록으로 전환합니다.
  - **Shift+D**: 이전 디스플레이로 전환합니다.
  - **Tab / Shift+Tab / 방향키**: 해당 화면의 창을 선택합니다.
    - **Alt 놓기**: 선택한 창으로 이동합니다. **Esc**: 취소합니다.
- 기존 **Alt+Tab per monitor**는 끄고 이 모드만 활성화하세요.
  - 앱 창과 마우스 커서의 실제 위치는 바뀌지 않습니다.
  - 전환 가능한 창이 없는 화면은 건너뜁니다.
  - 화면 전환 중에는 목록이 잠깐 닫혔다 다시 열립니다.
  - 관리자 권한 앱처럼 Windows가 입력 전달을 제한하는 창에서는 D 전환을
    생략하고 기존 목록을 유지합니다. 일반 창 선택은 계속 사용할 수 있습니다.
- **Ctrl+Alt+Tab** 고정 모드에서도 **D / Shift+D**로 화면을 바꿀 수 있습니다.
  - **Enter**로 창을 선택합니다.

## Credits

Based on Alt+Tab per monitor 1.2 by L3r0y. Its monitor placement and window
filtering code is reused under the MIT license. Keyboard monitor cycling and
session handling are added by this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- initialMonitorSource: activeWindow
  $name: Initial monitor
  $name:ko: 시작 디스플레이 기준
  $description: Captured when Alt+Tab opens; mouse movement never changes an open session.
  $description:ko: Alt+Tab을 열 때만 결정합니다. 열린 동안 마우스를 옮겨도 화면이 바뀌지 않습니다.
  $options:
  - activeWindow: Focused application's monitor (recommended)
  - cursor: Monitor where cursor is located
  $options:ko:
  - activeWindow: 현재 활성 프로그램이 있는 화면 (권장)
  - cursor: 마우스 커서가 있는 화면
- enableMonitorCycling: true
  $name: Enable D / Shift+D monitor switching
  $name:ko: D / Shift+D 디스플레이 전환 사용
  $description: Switch monitors while the native Alt+Tab switcher is open.
  $description:ko: Alt+Tab 목록에서 Alt를 누른 채 D로 화면을 전환합니다.
- altTabLocation: selectedMonitor
  $name: "Alt+Tab: Display location"
  $name:ko: "Alt+Tab: 표시할 화면"
  $description: Choose which monitor the Alt+Tab switcher appears on
  $description:ko: 처음 Alt+Tab 목록을 표시할 화면을 선택합니다.
  $options:
  - selectedMonitor: Session's selected monitor
  - primary: Primary monitor
  $options:ko:
  - selectedMonitor: 현재 세션에서 선택한 화면
  - primary: 주 디스플레이
- altTabWindows: selectedMonitor
  $name: "Alt+Tab: Windows to show"
  $name:ko: "Alt+Tab: 표시할 창"
  $description: Choose which windows to show when using Alt+Tab
  $description:ko: 처음 Alt+Tab 목록에 포함할 창을 선택합니다.
  $options:
  - selectedMonitor: Windows from the selected monitor
  - allMonitors: Windows from all monitors
  - selectedMonitorExceptPrimary: >-
      Windows from the selected monitor, but all windows when on primary
  $options:ko:
  - selectedMonitor: 선택한 화면의 창만
  - allMonitors: 모든 화면의 창
  - selectedMonitorExceptPrimary: 선택한 화면의 창만, 주 디스플레이에서는 모든 창
- winAltTabLocation: primary
  $name: "Win+Alt+Tab: Display location"
  $name:ko: "Win+Alt+Tab: 표시할 화면"
  $description: >-
    Choose which monitor the Alt+Tab switcher appears on when using Win+Alt+Tab
  $description:ko: Win+Alt+Tab 목록을 표시할 화면을 선택합니다.
  $options:
  - primary: Primary monitor
  - selectedMonitor: Session's selected monitor
  $options:ko:
  - primary: 주 디스플레이
  - selectedMonitor: 현재 세션에서 선택한 화면
- winAltTabWindows: allMonitors
  $name: "Win+Alt+Tab: Windows to show"
  $name:ko: "Win+Alt+Tab: 표시할 창"
  $description: Choose which windows to show when using Win+Alt+Tab
  $description:ko: Win+Alt+Tab 목록에 포함할 창을 선택합니다.
  $options:
  - allMonitors: Windows from all monitors
  - selectedMonitor: Windows from the selected monitor
  - selectedMonitorExceptPrimary: >-
      Windows from the selected monitor, but all windows when on primary
  $options:ko:
  - allMonitors: 모든 화면의 창
  - selectedMonitor: 선택한 화면의 창만
  - selectedMonitorExceptPrimary: 선택한 화면의 창만, 주 디스플레이에서는 모든 창
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <vector>

#include <winrt/windows.foundation.collections.h>

enum class Location {
    primary,
    cursor,
};

enum class WindowsToShow {
    allMonitors,
    cursorMonitor,
    cursorMonitorExceptPrimary,
};

struct {
    Location altTabLocation;
    WindowsToShow altTabWindows;
    Location winAltTabLocation;
    WindowsToShow winAltTabWindows;
} g_settings;

std::atomic<bool> g_enableMonitorCycling;
std::atomic<bool> g_initialMonitorFromCursor;
std::atomic<HMONITOR> g_monitorOverride;
std::atomic<HMONITOR> g_sessionMonitor;
std::atomic<bool> g_filterByMonitor;
std::atomic<bool> g_sessionActive;
std::atomic<bool> g_sessionStartedByHook;
std::atomic<bool> g_stickySession;
std::atomic<bool> g_switchPending;
std::atomic<HWND> g_switcherWindow;
std::atomic<HWND> g_returnWindow;
SRWLOCK g_seenMonitorsLock = SRWLOCK_INIT;
std::vector<HMONITOR> g_seenMonitors;
HANDLE g_keyboardThread;
DWORD g_keyboardThreadId;
HANDLE g_keyboardReady;
bool g_keyboardReadyOk;
DWORD g_inputIntegrityLevel;
bool g_inputHasUIAccess;
constexpr UINT kSwitchMonitorMessage = WM_APP + 1;
constexpr ULONG_PTR kOwnInputTag = 0x41544D53;

HMONITOR GetTargetMonitor() {
    if (HMONITOR monitor = g_monitorOverride.load()) {
        MONITORINFO info{sizeof(info)};
        if (GetMonitorInfo(monitor, &info)) {
            return monitor;
        }
    }
    if (HMONITOR monitor = g_sessionMonitor.load()) {
        MONITORINFO info{sizeof(info)};
        if (GetMonitorInfo(monitor, &info))
            return monitor;
    }
    return MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTOPRIMARY);
}

HMONITOR CaptureInitialMonitor(HWND foreground) {
    if (g_initialMonitorFromCursor) {
        POINT cursor{};
        if (GetCursorPos(&cursor)) {
            return MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
        }
    }
    return MonitorFromWindow(foreground, MONITOR_DEFAULTTOPRIMARY);
}

bool IsSwitcherWindow(HWND window) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return false;
    }
    WCHAR className[128]{};
    if (!GetClassName(window, className, ARRAYSIZE(className)))
        return false;
    if (wcscmp(className, L"XamlExplorerHostIslandWindow") != 0 &&
        wcscmp(className, L"MultitaskingViewFrame") != 0 &&
        wcscmp(className, L"TaskSwitcherWnd") != 0) {
        return false;
    }
    return true;
}

bool BeginSession(HWND foreground) {
    // Reset even if Windows has already activated a host. Such a host must
    // never become the application's return window for a D transition.
    g_monitorOverride = nullptr;
    g_sessionMonitor = nullptr;
    g_switcherWindow = nullptr;
    g_returnWindow = nullptr;
    g_sessionActive = false;
    g_stickySession = false;
    if (!foreground || IsSwitcherWindow(foreground))
        return false;
    g_returnWindow = foreground;
    g_sessionMonitor = CaptureInitialMonitor(foreground);
    g_stickySession = GetAsyncKeyState(VK_CONTROL) < 0;
    g_sessionActive = true;
    return true;
}

bool IsSwitcherForeground() {
    if (!g_sessionActive)
        return false;
    HWND window = GetForegroundWindow();
    if (HWND knownWindow = g_switcherWindow.load()) {
        return window == knownWindow && IsSwitcherWindow(window);
    }
    if (!IsSwitcherWindow(window))
        return false;
    g_switcherWindow = window;
    return true;
}

bool ReadIntegrityLevel(HANDLE token, DWORD* level) {
    DWORD size = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &size);
    if (!size)
        return false;
    std::vector<BYTE> buffer(size);
    if (!GetTokenInformation(token, TokenIntegrityLevel, buffer.data(), size,
                             &size))
        return false;
    PSID sid =
        reinterpret_cast<TOKEN_MANDATORY_LABEL*>(buffer.data())->Label.Sid;
    if (!IsValidSid(sid) || !*GetSidSubAuthorityCount(sid))
        return false;
    *level = *GetSidSubAuthority(sid, *GetSidSubAuthorityCount(sid) - 1);
    return true;
}

bool CanSendInputToWindow(HWND window) {
    if (g_inputHasUIAccess)
        return true;
    DWORD processId = 0;
    if (!GetWindowThreadProcessId(window, &processId) || !processId)
        return false;
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process)
        return false;
    HANDLE token = nullptr;
    bool allowed = false;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        DWORD level = 0;
        allowed =
            ReadIntegrityLevel(token, &level) && level <= g_inputIntegrityLevel;
        CloseHandle(token);
    }
    CloseHandle(process);
    return allowed;
}

HMONITOR GetNextMonitor(HMONITOR current, bool reverse) {
    struct Monitor {
        HMONITOR handle;
        RECT bounds;
    };
    std::vector<Monitor> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT bounds, LPARAM context) -> BOOL {
            auto& list = *reinterpret_cast<std::vector<Monitor>*>(context);
            list.push_back({monitor, *bounds});
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));
    std::sort(monitors.begin(), monitors.end(),
              [](const auto& a, const auto& b) {
                  if (a.bounds.left != b.bounds.left) {
                      return a.bounds.left < b.bounds.left;
                  }
                  return a.bounds.top < b.bounds.top;
              });
    AcquireSRWLockShared(&g_seenMonitorsLock);
    auto seen = g_seenMonitors;
    ReleaseSRWLockShared(&g_seenMonitorsLock);
    if (monitors.empty()) {
        return nullptr;
    }
    size_t index = monitors.size();
    for (size_t i = 0; i < monitors.size(); i++) {
        if (monitors[i].handle == current) {
            index = i;
            break;
        }
    }
    bool currentFound = index != monitors.size();
    size_t steps = monitors.size() - (currentFound ? 1 : 0);
    for (size_t step = 0; step < steps; step++) {
        size_t next =
            !currentFound ? (reverse ? monitors.size() - 1 - step : step)
            : reverse ? (index + monitors.size() - step - 1) % monitors.size()
                      : (index + step + 1) % monitors.size();
        HMONITOR candidate = monitors[next].handle;
        if (std::find(seen.begin(), seen.end(), candidate) != seen.end()) {
            return candidate;
        }
    }
    return nullptr;
}

bool SendKeys(const std::vector<std::pair<WORD, bool>>& keys) {
    std::vector<INPUT> inputs;
    for (const auto& [key, up] : keys) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        input.ki.dwExtraInfo = kOwnInputTag;
        inputs.push_back(input);
    }
    return SendInput(static_cast<UINT>(inputs.size()), inputs.data(),
                     sizeof(INPUT)) == inputs.size();
}

void ReportSwitchState(PCWSTR state) {
    Wh_Log(L"Monitor switch: %s", state);
}

LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
    static bool consumedD = false;
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    auto* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (key->dwExtraInfo == kOwnInputTag) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
    bool alt = (key->flags & LLKHF_ALTDOWN) != 0;
    if (key->vkCode == 'D' && consumedD) {
        if (up) {
            consumedD = false;
        }
        return 1;
    }
    // Holding Tab must not start a new session in the gap between hosts.
    if (g_switchPending && (key->vkCode == VK_TAB || key->vkCode == VK_LEFT ||
                            key->vkCode == VK_RIGHT || key->vkCode == VK_UP ||
                            key->vkCode == VK_DOWN)) {
        return 1;
    }
    if (!up && key->vkCode == VK_TAB && alt && !g_switchPending &&
        !IsSwitcherWindow(GetForegroundWindow())) {
        g_sessionStartedByHook = BeginSession(GetForegroundWindow());
    }
    if (!up && key->vkCode == 'D' && g_enableMonitorCycling &&
        (g_switchPending || IsSwitcherForeground())) {
        consumedD = true;
        PostThreadMessage(g_keyboardThreadId, kSwitchMonitorMessage,
                          GetAsyncKeyState(VK_SHIFT) < 0, 0);
        return 1;
    }
    if ((!up && (key->vkCode == VK_ESCAPE || key->vkCode == VK_RETURN ||
                 key->vkCode == VK_LWIN || key->vkCode == VK_RWIN)) ||
        (up && (key->vkCode == VK_LMENU || key->vkCode == VK_RMENU) &&
         !g_stickySession)) {
        g_sessionActive = false;
        g_sessionStartedByHook = false;
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

DWORD WINAPI KeyboardThread(void*) {
    MSG message;
    PeekMessage(&message, nullptr, 0, 0, PM_NOREMOVE);
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<PCWSTR>(KeyboardHook), &module);
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, module, 0);
    g_keyboardReadyOk = hook != nullptr;
    SetEvent(g_keyboardReady);
    if (!hook) {
        return 0;
    }
    UINT_PTR timer = 0;
    ULONGLONG deadline = 0;
    ULONGLONG hiddenSince = 0;
    HWND dismissedWindow = nullptr;
    HMONITOR nextMonitor = nullptr;
    bool reopening = false;
    int queuedSteps = 0;
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        if (message.message == kSwitchMonitorMessage) {
            if (!g_enableMonitorCycling || !g_sessionActive) {
                continue;
            }
            if (g_switchPending) {
                queuedSteps = std::clamp(
                    queuedSteps + (message.wParam ? -1 : 1), -16, 16);
                continue;
            }
            if (!IsSwitcherForeground()) {
                continue;
            }
            // UIPI can silently drop the reopening input after Escape returns
            // focus to an elevated app. Keep the current switcher instead.
            if (!CanSendInputToWindow(g_returnWindow)) {
                ReportSwitchState(L"input-to-return-window-restricted");
                continue;
            }
            nextMonitor = GetNextMonitor(g_sessionMonitor, message.wParam != 0);
            if (!nextMonitor) {
                ReportSwitchState(L"no-other-monitor-with-windows");
                continue;
            }
            dismissedWindow = GetForegroundWindow();
            g_switchPending = true;
            reopening = false;
            hiddenSince = 0;
            deadline = GetTickCount64() + 1500;
            ReportSwitchState(L"waiting-for-old-switcher-to-hide");
            // Escape cancels the old selection rather than activating it.
            if (!SendKeys({{VK_ESCAPE, false}, {VK_ESCAPE, true}})) {
                g_switchPending = false;
                ReportSwitchState(L"escape-input-failed");
                continue;
            }
            timer = SetTimer(nullptr, 0, 15, nullptr);
            if (!timer) {
                g_switchPending = false;
                ReportSwitchState(L"timer-failed");
            }
        } else if (message.message == WM_TIMER && message.wParam == timer) {
            HWND foreground = GetForegroundWindow();
            ULONGLONG now = GetTickCount64();
            bool canceled =
                !g_sessionActive || !g_enableMonitorCycling ||
                (!g_stickySession && GetAsyncKeyState(VK_MENU) >= 0);
            bool timedOut = now >= deadline;
            bool complete = reopening && IsSwitcherForeground();
            if (!canceled && !timedOut && !complete && !reopening) {
                // Foreground can temporarily be NULL or a shell window during
                // dismissal. Wait for both the old host to hide and focus to
                // return; a foreground change alone doesn't finish dismissal.
                if (IsWindowVisible(dismissedWindow) ||
                    foreground != g_returnWindow) {
                    hiddenSince = 0;
                    continue;
                }
                if (!hiddenSince)
                    hiddenSince = now;
                if (now - hiddenSince < 60)
                    continue;

                g_monitorOverride = nextMonitor;
                g_sessionMonitor = nextMonitor;
                std::vector<std::pair<WORD, bool>> keys;
                bool addControl =
                    g_stickySession && GetAsyncKeyState(VK_CONTROL) >= 0;
                bool addAlt = GetAsyncKeyState(VK_MENU) >= 0;
                if (addControl)
                    keys.push_back({VK_CONTROL, false});
                if (addAlt)
                    keys.push_back({VK_MENU, false});
                keys.push_back({VK_TAB, false});
                keys.push_back({VK_TAB, true});
                if (addAlt)
                    keys.push_back({VK_MENU, true});
                if (addControl)
                    keys.push_back({VK_CONTROL, true});
                g_switcherWindow = nullptr;
                if (!SendKeys(keys)) {
                    std::vector<std::pair<WORD, bool>> release;
                    if (addAlt)
                        release.push_back({VK_MENU, true});
                    if (addControl)
                        release.push_back({VK_CONTROL, true});
                    if (!release.empty())
                        SendKeys(release);
                    canceled = true;
                } else {
                    reopening = true;
                    deadline = now + 1000;
                    ReportSwitchState(L"waiting-for-new-switcher");
                    continue;
                }
            }
            if (!canceled && !timedOut && !complete)
                continue;
            KillTimer(nullptr, timer);
            timer = 0;
            g_switchPending = false;
            ReportSwitchState(complete    ? L"success"
                              : canceled  ? L"canceled"
                              : reopening ? L"reopen-timeout"
                                          : L"dismiss-timeout");
            if (complete && queuedSteps) {
                bool reverse = queuedSteps < 0;
                queuedSteps += reverse ? 1 : -1;
                PostThreadMessage(g_keyboardThreadId, kSwitchMonitorMessage,
                                  reverse, 0);
            } else {
                queuedSteps = 0;
            }
        } else {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    if (timer)
        KillTimer(nullptr, timer);
    UnhookWindowsHookEx(hook);
    g_switchPending = false;
    return 0;
}

bool StartKeyboardThread() {
    g_keyboardReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_keyboardReady)
        return false;
    g_keyboardThread = CreateThread(nullptr, 0, KeyboardThread, nullptr, 0,
                                    &g_keyboardThreadId);
    if (!g_keyboardThread) {
        CloseHandle(g_keyboardReady);
        return false;
    }
    WaitForSingleObject(g_keyboardReady, INFINITE);
    CloseHandle(g_keyboardReady);
    if (!g_keyboardReadyOk) {
        WaitForSingleObject(g_keyboardThread, INFINITE);
        CloseHandle(g_keyboardThread);
        g_keyboardThread = nullptr;
        return false;
    }
    return true;
}

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

std::atomic<DWORD> g_threadIdForAltTabShowWindow;
std::atomic<DWORD> g_lastThreadIdForXamlAltTabViewHost_CreateInstance;
std::atomic<DWORD> g_threadIdForXamlAltTabViewHost_CreateInstance;
std::atomic<ULONGLONG> g_CreateInstance_TickCount;
constexpr ULONGLONG kDeltaThreshold = 200;

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

WinVersion GetWindowsVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

bool IsWinKeyPressed() {
    return GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0;
}

bool HandleAltTabWindow(RECT* rect) {
    auto location = IsWinKeyPressed() ? g_settings.winAltTabLocation
                                      : g_settings.altTabLocation;

    if (!g_monitorOverride && location == Location::primary) {
        return false;
    }

    auto hMon = GetTargetMonitor();

    MONITORINFO monInfo;
    monInfo.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMon, &monInfo)) {
        return false;
    }

    *rect = monInfo.rcWork;
    return true;
}

void* CWin32ApplicationView_vtable;
void* CWinRTApplicationView_vtable;

using CWin32ApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWin32ApplicationView_v_GetNativeWindow_t
    CWin32ApplicationView_v_GetNativeWindow;

using CWinRTApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWinRTApplicationView_v_GetNativeWindow_t
    CWinRTApplicationView_v_GetNativeWindow;

HRESULT GetWindowHandleFromApplicationView(void* applicationView,
                                           HWND* windowHandle) {
    *windowHandle = nullptr;
    void* vtable = *(void**)applicationView;
    HRESULT hr = E_FAIL;

    if (vtable == CWin32ApplicationView_vtable) {
        hr = CWin32ApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    } else if (vtable == CWinRTApplicationView_vtable) {
        hr = CWinRTApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    }

    return hr;
}

bool IsWindowOnCursorMonitor(HWND windowHandle) {
    auto hMon = GetTargetMonitor();
    auto hMonFromWindow =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);

    return hMon == hMonFromWindow;
}

using CVirtualDesktop_IsViewVisible_t = HRESULT(WINAPI*)(void* pThis,
                                                         void* applicationView,
                                                         BOOL* isVisible);
CVirtualDesktop_IsViewVisible_t CVirtualDesktop_IsViewVisible_Original;
HRESULT WINAPI CVirtualDesktop_IsViewVisible_Hook(void* pThis,
                                                  void* applicationView,
                                                  BOOL* isVisible) {
    Wh_Log(L">");
    auto ret = CVirtualDesktop_IsViewVisible_Original(pThis, applicationView,
                                                      isVisible);
    if (FAILED(ret)) {
        return ret;
    }

    if (g_threadIdForXamlAltTabViewHost_CreateInstance !=
        GetCurrentThreadId()) {
        // A focused window might be added after a short period. Filter windows
        // using our monitor rules if the alt tab window was just opened.
        // Otherwise, don't play with the filter anymore, as it's also used by
        // other components such as Win+Tab and the taskbar.
        if ((GetTickCount64() - g_CreateInstance_TickCount) > kDeltaThreshold ||
            g_lastThreadIdForXamlAltTabViewHost_CreateInstance !=
                GetCurrentThreadId()) {
            return ret;
        }
    }

    if (!*isVisible) {
        return ret;
    }

    HWND windowHandle;
    HRESULT hr =
        GetWindowHandleFromApplicationView(applicationView, &windowHandle);

    if (FAILED(hr) || !windowHandle) {
        return ret;
    }

    HMONITOR monitor =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);
    AcquireSRWLockExclusive(&g_seenMonitorsLock);
    if (std::find(g_seenMonitors.begin(), g_seenMonitors.end(), monitor) ==
        g_seenMonitors.end()) {
        g_seenMonitors.push_back(monitor);
    }
    ReleaseSRWLockExclusive(&g_seenMonitorsLock);

    if (g_filterByMonitor && !IsWindowOnCursorMonitor(windowHandle)) {
        *isVisible = FALSE;
    }

    return ret;
}

using XamlAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                   void* param1,
                                                   int param2,
                                                   void* param3);
XamlAltTabViewHost_Show_t XamlAltTabViewHost_Show_Original;
HRESULT WINAPI XamlAltTabViewHost_Show_Hook(void* pThis,
                                            void* param1,
                                            int param2,
                                            void* param3) {
    Wh_Log(L">");
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    HRESULT ret =
        XamlAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    g_threadIdForAltTabShowWindow = 0;
    return ret;
}

using CAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                void* param1,
                                                int param2,
                                                void* param3);
CAltTabViewHost_Show_t CAltTabViewHost_Show_Original;
HRESULT WINAPI CAltTabViewHost_Show_Hook(void* pThis,
                                         void* param1,
                                         int param2,
                                         void* param3) {
    Wh_Log(L">");
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    HRESULT ret = CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    g_threadIdForAltTabShowWindow = 0;
    return ret;
}

using ITaskGroupWindowInformation_Position_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect* rect);
ITaskGroupWindowInformation_Position_t
    ITaskGroupWindowInformation_Position_Original;
HRESULT WINAPI ITaskGroupWindowInformation_Position_Hook(
    void* pThis,
    winrt::Windows::Foundation::Rect* rect) {
    if (g_threadIdForAltTabShowWindow != GetCurrentThreadId()) {
        return ITaskGroupWindowInformation_Position_Original(pThis, rect);
    }
    g_threadIdForAltTabShowWindow = 0;

    RECT newRectNative;
    if (!HandleAltTabWindow(&newRectNative)) {
        return ITaskGroupWindowInformation_Position_Original(pThis, rect);
    }

    winrt::Windows::Foundation::Rect newRect{
        static_cast<float>(newRectNative.left),
        static_cast<float>(newRectNative.top),
        static_cast<float>(newRectNative.right - newRectNative.left),
        static_cast<float>(newRectNative.bottom - newRectNative.top),
    };

    HRESULT ret =
        ITaskGroupWindowInformation_Position_Original(pThis, &newRect);

    return ret;
}

using CMultitaskingViewFrame_CreateFrame_t = HRESULT(WINAPI*)(void* pThis,
                                                              RECT* rect,
                                                              void* param2);
CMultitaskingViewFrame_CreateFrame_t
    CMultitaskingViewFrame_CreateFrame_Original;
HRESULT WINAPI CMultitaskingViewFrame_CreateFrame_Hook(void* pThis,
                                                       RECT* rect,
                                                       void* param2) {
    if (g_threadIdForAltTabShowWindow != GetCurrentThreadId()) {
        return CMultitaskingViewFrame_CreateFrame_Original(pThis, rect, param2);
    }
    g_threadIdForAltTabShowWindow = 0;

    RECT newRect;
    if (!HandleAltTabWindow(&newRect)) {
        return CMultitaskingViewFrame_CreateFrame_Original(pThis, rect, param2);
    }

    HRESULT ret =
        CMultitaskingViewFrame_CreateFrame_Original(pThis, &newRect, param2);

    return ret;
}

HRESULT CreateInstanceHook(std::function<HRESULT()> original) {
    bool startedByHook = g_sessionStartedByHook.exchange(false);
    if (!g_switchPending && !startedByHook) {
        // Creation is reached even if UIPI bypassed the low-level hook. Never
        // reuse the previous session's selected monitor or return window.
        BeginSession(GetForegroundWindow());
    }
    auto windows = IsWinKeyPressed() ? g_settings.winAltTabWindows
                                     : g_settings.altTabWindows;

    if (g_monitorOverride)
        windows = WindowsToShow::cursorMonitor;
    g_sessionMonitor = GetTargetMonitor();
    g_filterByMonitor = windows != WindowsToShow::allMonitors;
    AcquireSRWLockExclusive(&g_seenMonitorsLock);
    g_seenMonitors.clear();
    ReleaseSRWLockExclusive(&g_seenMonitorsLock);

    if (windows == WindowsToShow::cursorMonitorExceptPrimary) {
        HMONITOR hMon = GetTargetMonitor();
        MONITORINFO monInfo{sizeof(monInfo)};
        if (GetMonitorInfo(hMon, &monInfo) &&
            (monInfo.dwFlags & MONITORINFOF_PRIMARY)) {
            g_filterByMonitor = false;
        }
    }

    g_threadIdForXamlAltTabViewHost_CreateInstance = GetCurrentThreadId();
    g_lastThreadIdForXamlAltTabViewHost_CreateInstance = GetCurrentThreadId();
    g_CreateInstance_TickCount = GetTickCount64();
    HRESULT ret = original();
    g_threadIdForXamlAltTabViewHost_CreateInstance = 0;
    return ret;
}

using XamlAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                             void* param1,
                                                             void* param2,
                                                             void* param3);
XamlAltTabViewHost_CreateInstance_t XamlAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI XamlAltTabViewHost_CreateInstance_Hook(void* pThis,
                                                      void* param1,
                                                      void* param2,
                                                      void* param3) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return XamlAltTabViewHost_CreateInstance_Original(pThis, param1, param2,
                                                          param3);
    });
}

using CAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                          void* param1,
                                                          void* param2,
                                                          void* param3,
                                                          void* param4,
                                                          void* param5,
                                                          void* param6,
                                                          void* param7,
                                                          void* param8,
                                                          void* param9,
                                                          void* param10,
                                                          void* param11);
CAltTabViewHost_CreateInstance_t CAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI CAltTabViewHost_CreateInstance_Hook(void* pThis,
                                                   void* param1,
                                                   void* param2,
                                                   void* param3,
                                                   void* param4,
                                                   void* param5,
                                                   void* param6,
                                                   void* param7,
                                                   void* param8,
                                                   void* param9,
                                                   void* param10,
                                                   void* param11) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return CAltTabViewHost_CreateInstance_Original(
            pThis, param1, param2, param3, param4, param5, param6, param7,
            param8, param9, param10, param11);
    });
}

using CAltTabViewHost_CreateInstance_Win11_t = HRESULT(WINAPI*)(void* pThis,
                                                                void* param1,
                                                                void* param2,
                                                                void* param3,
                                                                void* param4,
                                                                void* param5,
                                                                void* param6,
                                                                void* param7,
                                                                void* param8,
                                                                void* param9,
                                                                void* param10);
CAltTabViewHost_CreateInstance_Win11_t
    CAltTabViewHost_CreateInstance_Win11_Original;
HRESULT WINAPI CAltTabViewHost_CreateInstance_Win11_Hook(void* pThis,
                                                         void* param1,
                                                         void* param2,
                                                         void* param3,
                                                         void* param4,
                                                         void* param5,
                                                         void* param6,
                                                         void* param7,
                                                         void* param8,
                                                         void* param9,
                                                         void* param10) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return CAltTabViewHost_CreateInstance_Win11_Original(
            pThis, param1, param2, param3, param4, param5, param6, param7,
            param8, param9, param10);
    });
}

void LoadSettings() {
    g_enableMonitorCycling = Wh_GetIntSetting(L"enableMonitorCycling") != 0;
    PCWSTR initialSource = Wh_GetStringSetting(L"initialMonitorSource");
    g_initialMonitorFromCursor = wcscmp(initialSource, L"cursor") == 0;
    Wh_FreeStringSetting(initialSource);
    auto loadLocation = [](PCWSTR name, Location defaultVal) {
        PCWSTR val = Wh_GetStringSetting(name);
        Location result = defaultVal;
        if (wcscmp(val, L"primary") == 0) {
            result = Location::primary;
        } else if (wcscmp(val, L"selectedMonitor") == 0 ||
                   wcscmp(val, L"cursor") == 0) {
            result = Location::cursor;
        }
        Wh_FreeStringSetting(val);
        return result;
    };

    auto loadWindows = [](PCWSTR name, WindowsToShow defaultVal) {
        PCWSTR val = Wh_GetStringSetting(name);
        WindowsToShow result = defaultVal;
        if (wcscmp(val, L"allMonitors") == 0) {
            result = WindowsToShow::allMonitors;
        } else if (wcscmp(val, L"selectedMonitor") == 0 ||
                   wcscmp(val, L"cursorMonitor") == 0) {
            result = WindowsToShow::cursorMonitor;
        } else if (wcscmp(val, L"selectedMonitorExceptPrimary") == 0 ||
                   wcscmp(val, L"cursorMonitorExceptPrimary") == 0) {
            result = WindowsToShow::cursorMonitorExceptPrimary;
        }
        Wh_FreeStringSetting(val);
        return result;
    };

    g_settings.altTabLocation =
        loadLocation(L"altTabLocation", Location::cursor);
    g_settings.altTabWindows =
        loadWindows(L"altTabWindows", WindowsToShow::cursorMonitor);
    g_settings.winAltTabLocation =
        loadLocation(L"winAltTabLocation", Location::primary);
    g_settings.winAltTabWindows =
        loadWindows(L"winAltTabWindows", WindowsToShow::allMonitors);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetWindowsVersion();

    HANDLE inputToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &inputToken)) {
        ReadIntegrityLevel(inputToken, &g_inputIntegrityLevel);
        DWORD uiAccess = 0, size = 0;
        if (GetTokenInformation(inputToken, TokenUIAccess, &uiAccess,
                                sizeof(uiAccess), &size)) {
            g_inputHasUIAccess = uiAccess != 0;
        }
        CloseHandle(inputToken);
    }
    HMODULE twinuiPcshellModule = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (g_winVersion == WinVersion::Win11) {
        // twinui.pcshell.dll
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
                &CVirtualDesktop_IsViewVisible_Original,
                CVirtualDesktop_IsViewVisible_Hook,
            },
            {
                {LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
                &CWin32ApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWin32ApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
                &CWinRTApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWinRTApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(public: virtual long __cdecl XamlAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &XamlAltTabViewHost_Show_Original,
                XamlAltTabViewHost_Show_Hook,
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_Internal_Shell_TaskGroups_ITaskGroupWindowInformation<struct winrt::Windows::Internal::Shell::TaskGroups::ITaskGroupWindowInformation>::Position(struct winrt::Windows::Foundation::Rect const &)const )"},
                &ITaskGroupWindowInformation_Position_Original,
                ITaskGroupWindowInformation_Position_Hook,
            },
            {
                {LR"(long __cdecl XamlAltTabViewHost_CreateInstance(struct XamlViewHostInitializeArgs const &,struct _GUID const &,void * *))"},
                &XamlAltTabViewHost_CreateInstance_Original,
                XamlAltTabViewHost_CreateInstance_Hook,
            },
            // For the old Win10 (non-XAML) Alt+Tab (can be enabled with
            // ExplorerPatcher):
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
                true,
            },
            {
                {LR"(public: virtual long __cdecl CMultitaskingViewFrame::CreateFrame(struct Geometry::CRect const &,enum MultitaskingViewZOrder))"},
                &CMultitaskingViewFrame_CreateFrame_Original,
                CMultitaskingViewFrame_CreateFrame_Hook,
                true,
            },
            {
                {LR"(long __cdecl CAltTabViewHost_CreateInstance(struct IMultitaskingData *,struct IMultitaskingViewManagerInternal *,struct IApplicationViewSwitcher *,struct IImmersiveAppCrusher *,struct IMultitaskingViewVisibilityServiceInternal *,struct IMultitaskingViewGestureState *,struct IApplicationViewCollection *,struct ITouchGestureSettings *,struct _GUID const &,void * *))"},
                &CAltTabViewHost_CreateInstance_Win11_Original,
                CAltTabViewHost_CreateInstance_Win11_Hook,
                true,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else if (g_winVersion == WinVersion::Win10) {
        // twinui.pcshell.dll
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
                &CVirtualDesktop_IsViewVisible_Original,
                CVirtualDesktop_IsViewVisible_Hook,
            },
            {
                {LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
                &CWin32ApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWin32ApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
                &CWinRTApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWinRTApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CMultitaskingViewFrame::CreateFrame(struct Geometry::CRect const &,enum MultitaskingViewZOrder))"},
                &CMultitaskingViewFrame_CreateFrame_Original,
                CMultitaskingViewFrame_CreateFrame_Hook,
            },
            {
                {LR"(long __cdecl CAltTabViewHost_CreateInstance(struct IMultitaskingData *,struct IMultitaskingViewManagerInternal *,struct IApplicationViewSwitcher *,struct IImmersiveAppCrusher *,struct IMultitaskingViewVisibilityServiceInternal *,struct IMultitaskingViewGestureState *,struct IApplicationViewCollection *,struct Windows::Internal::ComposableShell::Tabs::ITabController *,struct ITabViewManager *,struct _GUID const &,void * *))"},
                &CAltTabViewHost_CreateInstance_Original,
                CAltTabViewHost_CreateInstance_Hook,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (!StartKeyboardThread()) {
        Wh_Log(L"Couldn't install the monitor switching keyboard hook");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    g_sessionActive = false;
    g_sessionStartedByHook = false;
    if (g_keyboardThread) {
        PostThreadMessage(g_keyboardThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_keyboardThread, INFINITE);
        CloseHandle(g_keyboardThread);
        g_keyboardThread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
