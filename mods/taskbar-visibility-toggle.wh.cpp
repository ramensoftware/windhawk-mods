// ==WindhawkMod==
// @id              taskbar-visibility-toggle
// @name            Taskbar visibility toggle
// @description     Hide or unhide the taskbar with the keyboard (Ctrl+Esc).
// @version         0.1
// @author          roosmsg
// @github          https://github.com/roosmsg
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar visibility toggle

Press Ctrl+Esc to toggle the taskbar visibility. The state is sticky (shown or hidden) until you press Ctrl+Esc again.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keepWindowSizesFixed: false
  $name: Keep window sizes fixed
  $description: >-
    When enabled, the windows keep their size and the taskbar overlays them. The Windows setting "Auto Hide Taskbar" must be enabled for this to work.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <windows.h>

#include <atomic>
#include <type_traits>
#include <vector>

struct {
    bool keepWindowSizesFixed;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

enum class ToggleState {
    Default,
    ForcedShown,
    ForcedHidden,
};

std::atomic<ToggleState> g_toggleState{ToggleState::Default};
std::atomic<DWORD> g_lastToggleTick{0};
std::atomic<DWORD> g_lastWinKeyTick{0};
std::atomic<bool> g_autoHideStateCaptured{false};
std::atomic<bool> g_autoHideOriginalEnabled{false};
std::atomic<bool> g_initialized{false};
std::atomic<bool> g_explorerPatcherInitialized{false};
HHOOK g_keyboardHook = nullptr;
HANDLE g_keyboardHookThread = nullptr;
DWORD g_keyboardHookThreadId = 0;
HANDLE g_keyboardHookReadyEvent = nullptr;

enum {
    kTrayUITimerHide = 2,
    kTrayUITimerUnhide = 3,
};

static const UINT g_setAutoHideStateRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_SetTaskbarAutoHide_" WH_MOD_ID);

// TrayUI::_HandleTrayPrivateSettingMessage.
constexpr UINT kHandleTrayPrivateSettingMessage = WM_USER + 0x1CA;

enum {
    kTrayPrivateSettingAutoHideGet = 3,
    kTrayPrivateSettingAutoHideSet = 4,
};

constexpr DWORD kToggleDebounceMs = 250;
constexpr DWORD kWinKeyRecentMs = 500;
constexpr UINT kKeyboardHookThreadQuitMsg = WM_APP + 0x217;
constexpr UINT kKeyboardHookThreadToggleMsg = WM_APP + 0x218;

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR className[32];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

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

HWND FindTaskbarWindows(std::vector<HWND>* secondaryTaskbarWindows) {
    secondaryTaskbarWindows->clear();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    auto enumWindowsProc = [&secondaryTaskbarWindows](HWND hWnd) -> BOOL {
        WCHAR className[32];
        if (GetClassName(hWnd, className, ARRAYSIZE(className)) == 0) {
            return TRUE;
        }

        if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            secondaryTaskbarWindows->push_back(hWnd);
        }

        return TRUE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hTaskbarWnd;
}

bool IsTaskbarShown(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hWnd, &rect)) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{
        .cbSize = sizeof(monitorInfo),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return false;
    }

    RECT intersect{};
    if (!IntersectRect(&intersect, &rect, &monitorInfo.rcMonitor)) {
        return false;
    }

    int width = intersect.right - intersect.left;
    int height = intersect.bottom - intersect.top;
    if (width <= 0 || height <= 0) {
        return false;
    }

    int taskbarWidth = rect.right - rect.left;
    int taskbarHeight = rect.bottom - rect.top;
    int taskbarThickness =
        taskbarWidth < taskbarHeight ? taskbarWidth : taskbarHeight;
    if (taskbarThickness <= 0) {
        return false;
    }

    int visibleThickness = width < height ? width : height;
    return visibleThickness * 2 >= taskbarThickness;
}

bool AreAnyTaskbarsShown() {
    std::vector<HWND> secondaryTaskbarWindows;
    HWND taskbarWindow = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (!taskbarWindow) {
        return false;
    }

    if (IsTaskbarShown(taskbarWindow)) {
        return true;
    }

    for (HWND hWnd : secondaryTaskbarWindows) {
        if (IsTaskbarShown(hWnd)) {
            return true;
        }
    }

    return false;
}

void HideAllTaskbars() {
    std::vector<HWND> secondaryTaskbarWindows;
    HWND taskbarWindow = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (taskbarWindow) {
        SetTimer(taskbarWindow, kTrayUITimerHide, 0, nullptr);
    }

    for (HWND hWnd : secondaryTaskbarWindows) {
        SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
    }
}

void CancelHideTimers() {
    std::vector<HWND> secondaryTaskbarWindows;
    HWND taskbarWindow = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (taskbarWindow) {
        KillTimer(taskbarWindow, kTrayUITimerHide);
    }

    for (HWND hWnd : secondaryTaskbarWindows) {
        KillTimer(hWnd, kTrayUITimerHide);
    }
}

bool GetTaskbarAutoHideEnabled(bool* enabled) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    *enabled = SendMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                           kTrayPrivateSettingAutoHideGet, 0) != 0;
    return true;
}

void SetTaskbarAutoHideEnabled(bool enabled) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    if (SendNotifyMessage(hTaskbarWnd, g_setAutoHideStateRegisteredMsg,
                          enabled ? TRUE : FALSE, 0)) {
        return;
    }

    SendNotifyMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                      kTrayPrivateSettingAutoHideSet, enabled ? TRUE : FALSE);
}

void CaptureAutoHideOriginalState() {
    if (g_autoHideStateCaptured.exchange(true, std::memory_order_relaxed)) {
        return;
    }

    bool enabled = false;
    if (!GetTaskbarAutoHideEnabled(&enabled)) {
        g_autoHideStateCaptured.store(false, std::memory_order_relaxed);
        return;
    }

    g_autoHideOriginalEnabled.store(enabled, std::memory_order_relaxed);
}

void RestoreAutoHideOriginalState() {
    if (!g_autoHideStateCaptured.load(std::memory_order_relaxed)) {
        return;
    }

    SetTaskbarAutoHideEnabled(
        g_autoHideOriginalEnabled.load(std::memory_order_relaxed));
    g_autoHideStateCaptured.store(false, std::memory_order_relaxed);
}

void UpdateAutoHideForToggleState(ToggleState state) {
    if (g_settings.keepWindowSizesFixed) {
        return;
    }

    if (state == ToggleState::ForcedHidden ||
        state == ToggleState::ForcedShown) {
        CaptureAutoHideOriginalState();
        SetTaskbarAutoHideEnabled(state == ToggleState::ForcedHidden);
    }
}

bool IsCtrlEscPressed() {
    bool ctrlDown =
        (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
        (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
        (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
    return ctrlDown && (GetAsyncKeyState(VK_ESCAPE) & 0x8000);
}

bool IsWinKeyPressed() {
    return (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

bool IsWinKeyAllowedToUnhide() {
    if (IsWinKeyPressed()) {
        return true;
    }

    DWORD lastTick = g_lastWinKeyTick.load(std::memory_order_relaxed);
    if (!lastTick) {
        return false;
    }

    DWORD now = GetTickCount();
    return now - lastTick < kWinKeyRecentMs;
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    ToggleState state = g_toggleState.load(std::memory_order_relaxed);

    if (nIDEvent == kTrayUITimerHide && IsTaskbarWindow(hWnd) &&
        state == ToggleState::ForcedShown) {
        return 1;
    }

    if (nIDEvent == kTrayUITimerUnhide && IsTaskbarWindow(hWnd) &&
        state == ToggleState::ForcedHidden && !IsWinKeyAllowedToUnhide()) {
        return 1;
    }

    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

void ShowAllTaskbars() {
    std::vector<HWND> secondaryTaskbarWindows;
    HWND taskbarWindow = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (taskbarWindow) {
        PostMessage(taskbarWindow, WM_TIMER, kTrayUITimerUnhide, 0);
    }

    for (HWND hWnd : secondaryTaskbarWindows) {
        PostMessage(hWnd, WM_TIMER, kTrayUITimerUnhide, 0);
    }
}

enum class ToggleAction {
    Show,
    Hide,
};

struct ToggleDecision {
    ToggleAction action;
    bool isNewToggle;
};

ToggleDecision DecideToggleAction(DWORD now) {
    ToggleState state = g_toggleState.load(std::memory_order_relaxed);
    DWORD lastTick = g_lastToggleTick.load(std::memory_order_relaxed);
    if (lastTick && now - lastTick < kToggleDebounceMs) {
        if (state == ToggleState::ForcedShown) {
            return {ToggleAction::Show, false};
        }
        return {ToggleAction::Hide, false};
    }

    bool shouldHide = false;
    if (state == ToggleState::ForcedShown) {
        shouldHide = true;
    } else if (state == ToggleState::ForcedHidden) {
        shouldHide = false;
    } else {
        shouldHide = AreAnyTaskbarsShown();
    }

    return {shouldHide ? ToggleAction::Hide : ToggleAction::Show, true};
}

void HandleStickyToggleFromKeyboard() {
    DWORD now = GetTickCount();
    ToggleDecision decision = DecideToggleAction(now);
    if (decision.action == ToggleAction::Hide) {
        if (decision.isNewToggle) {
            g_toggleState.store(ToggleState::ForcedHidden,
                                std::memory_order_relaxed);
            g_lastToggleTick.store(now, std::memory_order_relaxed);
            UpdateAutoHideForToggleState(ToggleState::ForcedHidden);
        }
        HideAllTaskbars();
        return;
    }

    if (decision.isNewToggle) {
        g_toggleState.store(ToggleState::ForcedShown,
                            std::memory_order_relaxed);
        g_lastToggleTick.store(now, std::memory_order_relaxed);
    }

    CancelHideTimers();
    ShowAllTaskbars();
    if (decision.isNewToggle) {
        UpdateAutoHideForToggleState(ToggleState::ForcedShown);
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    ToggleState state = g_toggleState.load(std::memory_order_relaxed);
    if (nCode == HC_ACTION &&
        (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        const auto* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (info &&
            (info->vkCode == VK_LWIN || info->vkCode == VK_RWIN)) {
            g_lastWinKeyTick.store(GetTickCount(), std::memory_order_relaxed);
        }

        if (state != ToggleState::ForcedHidden && info &&
            info->vkCode == VK_ESCAPE) {
            bool ctrlDown =
                (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
                (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
            if (ctrlDown) {
                if (g_keyboardHookThreadId) {
                    PostThreadMessage(g_keyboardHookThreadId,
                                      kKeyboardHookThreadToggleMsg, 0, 0);
                } else {
                    HandleStickyToggleFromKeyboard();
                }
                // Swallow Ctrl+Esc so the Start menu doesn't open.
                return 1;
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI KeyboardHookThreadProc(LPVOID) {
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_keyboardHook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (!g_keyboardHook) {
        Wh_Log(L"Failed to install keyboard hook");
        if (g_keyboardHookReadyEvent) {
            SetEvent(g_keyboardHookReadyEvent);
        }
        return 0;
    }

    if (g_keyboardHookReadyEvent) {
        SetEvent(g_keyboardHookReadyEvent);
    }

    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == nullptr &&
            msg.message == kKeyboardHookThreadQuitMsg) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.hwnd == nullptr &&
            msg.message == kKeyboardHookThreadToggleMsg) {
            HandleStickyToggleFromKeyboard();
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    return 0;
}

void InstallKeyboardHook() {
    if (g_keyboardHookThread) {
        return;
    }

    g_keyboardHookReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_keyboardHookThread = CreateThread(nullptr, 0, KeyboardHookThreadProc,
                                        nullptr, 0, &g_keyboardHookThreadId);
    if (!g_keyboardHookThread) {
        Wh_Log(L"Failed to start keyboard hook thread");
        if (g_keyboardHookReadyEvent) {
            CloseHandle(g_keyboardHookReadyEvent);
            g_keyboardHookReadyEvent = nullptr;
        }
        return;
    }

    if (g_keyboardHookReadyEvent) {
        WaitForSingleObject(g_keyboardHookReadyEvent, 2000);
        CloseHandle(g_keyboardHookReadyEvent);
        g_keyboardHookReadyEvent = nullptr;
    }

    if (!g_keyboardHook) {
        PostThreadMessage(g_keyboardHookThreadId, kKeyboardHookThreadQuitMsg, 0,
                          0);
        WaitForSingleObject(g_keyboardHookThread, 1000);
        CloseHandle(g_keyboardHookThread);
        g_keyboardHookThread = nullptr;
        g_keyboardHookThreadId = 0;
    }
}

void UninstallKeyboardHook() {
    if (!g_keyboardHookThread) {
        return;
    }

    PostThreadMessage(g_keyboardHookThreadId, kKeyboardHookThreadQuitMsg, 0, 0);
    WaitForSingleObject(g_keyboardHookThread, 1000);
    CloseHandle(g_keyboardHookThread);
    g_keyboardHookThread = nullptr;
    g_keyboardHookThreadId = 0;
}

template <typename UnhideFn>
void HandleUnhideRequest(UnhideFn unhideOriginal,
                         void* pThis,
                         int trayUnhideFlags,
                         int unhideRequest) {
    if (IsCtrlEscPressed()) {
        DWORD now = GetTickCount();
        ToggleDecision decision = DecideToggleAction(now);
        if (decision.action == ToggleAction::Hide) {
            if (decision.isNewToggle) {
                g_toggleState.store(ToggleState::ForcedHidden,
                                    std::memory_order_relaxed);
                g_lastToggleTick.store(now, std::memory_order_relaxed);
                UpdateAutoHideForToggleState(ToggleState::ForcedHidden);
            }
            HideAllTaskbars();
            return;
        }

        if (decision.isNewToggle) {
            g_toggleState.store(ToggleState::ForcedShown,
                                std::memory_order_relaxed);
            g_lastToggleTick.store(now, std::memory_order_relaxed);
        }

        CancelHideTimers();
        unhideOriginal(pThis, trayUnhideFlags, unhideRequest);
        if (decision.isNewToggle) {
            UpdateAutoHideForToggleState(ToggleState::ForcedShown);
        }
        return;
    }

    if (g_toggleState.load(std::memory_order_relaxed) ==
            ToggleState::ForcedHidden &&
        !IsWinKeyAllowedToUnhide()) {
        return;
    }

    unhideOriginal(pThis, trayUnhideFlags, unhideRequest);
}

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;
void WINAPI TrayUI_Unhide_Hook(void* pThis,
                               int trayUnhideFlags,
                               int unhideRequest) {
    HandleUnhideRequest(TrayUI_Unhide_Original, pThis, trayUnhideFlags,
                        unhideRequest);
}

using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;
void WINAPI CSecondaryTray__Unhide_Hook(void* pThis,
                                        int trayUnhideFlags,
                                        int unhideRequest) {
    HandleUnhideRequest(CSecondaryTray__Unhide_Original, pThis,
                        trayUnhideFlags, unhideRequest);
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
    if (Msg == g_setAutoHideStateRegisteredMsg) {
        SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                    kTrayPrivateSettingAutoHideSet, (BOOL)wParam);
        return 1;
    }

    return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (g_toggleState.load(std::memory_order_relaxed) ==
        ToggleState::ForcedShown) {
        CancelHideTimers();
        return;
    }

    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    if (g_toggleState.load(std::memory_order_relaxed) ==
        ToggleState::ForcedShown) {
        CancelHideTimers();
        return;
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
            TrayUI_Unhide_Hook,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
            CSecondaryTray__Unhide_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
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

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?_Hide@TrayUI@@QEAAXXZ)", &TrayUI__Hide_Original,
         TrayUI__Hide_Hook, true},
        {R"(?_AutoHide@CSecondaryTray@@AEAAX_N@Z)",
         &CSecondaryTray__AutoHide_Original, CSecondaryTray__AutoHide_Hook,
         true},
        {R"(?Unhide@TrayUI@@UEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &TrayUI_Unhide_Original, TrayUI_Unhide_Hook, true},
        {R"(?WndProc@TrayUI@@UEAA_JPEAUHWND__@@I_K_JPEA_N@Z)",
         &TrayUI_WndProc_Original, TrayUI_WndProc_Hook},
        {R"(?_Unhide@CSecondaryTray@@AEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &CSecondaryTray__Unhide_Original, CSecondaryTray__Unhide_Hook, true},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

void LoadSettings() {
    g_settings.keepWindowSizesFixed =
        Wh_GetIntSetting(L"keepWindowSizesFixed");
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_toggleState.store(ToggleState::Default, std::memory_order_relaxed);
    g_lastToggleTick.store(0, std::memory_order_relaxed);

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    WindhawkUtils::Wh_SetFunctionHookT(SetTimer, SetTimer_Hook,
                                       &SetTimer_Original);

    g_initialized.store(true, std::memory_order_relaxed);
    return TRUE;
}

void Wh_ModAfterInit() {
    InstallKeyboardHook();
}

void Wh_ModUninit() {
    UninstallKeyboardHook();
    RestoreAutoHideOriginalState();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    bool prevKeepWindowSizesFixed = g_settings.keepWindowSizesFixed;
    LoadSettings();

    if (g_settings.keepWindowSizesFixed != prevKeepWindowSizesFixed) {
        if (g_settings.keepWindowSizesFixed) {
            RestoreAutoHideOriginalState();
            g_autoHideStateCaptured.store(false, std::memory_order_relaxed);
        } else {
            g_autoHideStateCaptured.store(false, std::memory_order_relaxed);
            UpdateAutoHideForToggleState(
                g_toggleState.load(std::memory_order_relaxed));
        }
    }

    if (bReload) {
        *bReload = FALSE;
    }

    return TRUE;
}
