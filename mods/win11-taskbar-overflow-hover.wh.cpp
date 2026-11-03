// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar Overflow on Hover
// @description     Opens the Windows 11 taskbar overflow menu on hover and closes it when you move away or click.
// @version         1.2
// @author          Cougar16
// @github          https://github.com/Cougar16
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Overflow on Hover

A zero-click QoL upgrade for Windows 11. Hover over the taskbar overflow button
 to open the hidden system tray icons.

Features:
- Opens the overflow menu after a configurable hover delay.
- Closes the flyout after the cursor leaves it.
- Closes the flyout after a click inside it, while allowing a tray icon popup menu
  to finish its interaction first.
- Uses the actual taskbar-owned overflow XAML island to determine whether the
  flyout is open.
- Collapses the overflow through its own UI Automation chevron; never sends a
  global Escape keystroke.
- UI Automation is queried only when the cursor is over a taskbar window.
- Runs as a dedicated Windhawk tool process, not inside Explorer.

Developed by Cougar16.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hoverDelayMs: 100
  $name: Hover Delay (ms)
  $description: Time in milliseconds to wait while hovering before opening the menu.
- closeDelayMs: 150
  $name: Close Delay (ms)
  $description: Time in milliseconds to wait after moving the cursor away before closing the menu.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>

#include <cwchar>

struct {
    int hoverDelayMs;
    int closeDelayMs;
} settings;

volatile LONG g_modUnloading = 0;
HANDLE g_hThread = nullptr;

// Return true only when the point belongs to a taskbar top-level window owned
// by the same process as the primary taskbar.
bool IsTaskbarWindowAtPoint(POINT pt, DWORD* taskbarPidOut = nullptr) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return false;
    }

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) {
        return false;
    }

    wchar_t className[256] = {};
    if (!GetClassNameW(hRoot, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"Shell_TrayWnd") != 0 &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return false;
    }

    HWND hPrimaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hPrimaryTaskbar) {
        return false;
    }

    DWORD primaryPid = 0;
    DWORD pointPid = 0;
    GetWindowThreadProcessId(hPrimaryTaskbar, &primaryPid);
    GetWindowThreadProcessId(hRoot, &pointPid);

    if (!primaryPid || pointPid != primaryPid) {
        return false;
    }

    if (taskbarPidOut) {
        *taskbarPidOut = primaryPid;
    }

    return true;
}

bool IsOverflowAutomationId(IUIAutomationElement* element) {
    if (!element) {
        return false;
    }

    BSTR autoId = nullptr;
    HRESULT hr = element->get_CurrentAutomationId(&autoId);
    bool matches = SUCCEEDED(hr) && autoId &&
                   (wcscmp(autoId, L"Chevron") == 0 ||
                    wcscmp(autoId, L"OverflowButton") == 0);

    if (autoId) {
        SysFreeString(autoId);
    }

    return matches;
}

// Walk the UIA control tree only after the Win32 taskbar check passed.
// Every element is verified to belong to the taskbar process.
bool FindTaskbarOverflowElement(
    IUIAutomation* automation,
    POINT pt,
    DWORD taskbarPid,
    IUIAutomationElement** elementOut,
    IUIAutomationInvokePattern** invokeOut,
    IUIAutomationExpandCollapsePattern** expandOut,
    IUIAutomationTogglePattern** toggleOut) {

    *elementOut = nullptr;
    *invokeOut = nullptr;
    *expandOut = nullptr;
    *toggleOut = nullptr;

    IUIAutomationElement* pointElement = nullptr;
    HRESULT hr = automation->ElementFromPoint(pt, &pointElement);
    if (FAILED(hr) || !pointElement) {
        return false;
    }

    IUIAutomationTreeWalker* walker = nullptr;
    hr = automation->get_ControlViewWalker(&walker);
    if (FAILED(hr) || !walker) {
        pointElement->Release();
        return false;
    }

    IUIAutomationElement* node = pointElement;
    node->AddRef();

    bool found = false;

    for (int i = 0; i < 15 && node; ++i) {
        int elementPid = 0;
        hr = node->get_CurrentProcessId(&elementPid);

        // Do not inspect or interact with elements from another process.
        if (FAILED(hr) || elementPid != static_cast<int>(taskbarPid)) {
            node->Release();
            node = nullptr;
            break;
        }

        if (IsOverflowAutomationId(node)) {
            IUIAutomationInvokePattern* invokePattern = nullptr;
            IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
            IUIAutomationTogglePattern* togglePattern = nullptr;

            node->GetCurrentPattern(
                UIA_InvokePatternId,
                reinterpret_cast<IUnknown**>(&invokePattern));
            node->GetCurrentPattern(
                UIA_ExpandCollapsePatternId,
                reinterpret_cast<IUnknown**>(&expandPattern));
            node->GetCurrentPattern(
                UIA_TogglePatternId,
                reinterpret_cast<IUnknown**>(&togglePattern));

            if (invokePattern || expandPattern || togglePattern) {
                *elementOut = node;
                *invokeOut = invokePattern;
                *expandOut = expandPattern;
                *toggleOut = togglePattern;
                node = nullptr;  // Ownership transferred to elementOut.
                found = true;
                break;
            }

            if (invokePattern) invokePattern->Release();
            if (expandPattern) expandPattern->Release();
            if (togglePattern) togglePattern->Release();
        }

        IUIAutomationElement* parent = nullptr;
        hr = walker->GetParentElement(node, &parent);
        node->Release();
        node = SUCCEEDED(hr) ? parent : nullptr;
    }

    if (node) {
        node->Release();
    }

    walker->Release();
    pointElement->Release();
    return found;
}

// Locate the real overflow XAML island. Visibility and PID are authoritative;
// our own record of having invoked the chevron is not used as proof it is open.
HWND GetVisibleFlyout(DWORD taskbarPid) {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(
                nullptr, hwnd, L"TopLevelWindowForOverflowXamlIsland", nullptr))) {
        if (!IsWindowVisible(hwnd)) {
            continue;
        }

        DWORD windowPid = 0;
        GetWindowThreadProcessId(hwnd, &windowPid);
        if (windowPid == taskbarPid) {
            return hwnd;
        }
    }

    return nullptr;
}

bool PtOverWindow(HWND hwnd, POINT pt) {
    if (!hwnd) {
        return false;
    }

    RECT rect = {};
    return GetWindowRect(hwnd, &rect) && PtInRect(&rect, pt);
}

bool IsMenuWindow(HWND hwnd) {
    if (!hwnd || !IsWindowVisible(hwnd)) {
        return false;
    }

    wchar_t className[64] = {};
    return GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"#32768") == 0;
}

// Detect a tray-icon popup menu associated with the current overflow flyout.
// This is deliberately not a global "any menu anywhere" test: unrelated menus
// elsewhere on the desktop must not keep the taskbar overflow open.
bool IsPopupMenuOpen(HWND flyout, DWORD taskbarPid, POINT cursorPt) {
    HWND underCursor = WindowFromPoint(cursorPt);
    HWND cursorRoot = underCursor ? GetAncestor(underCursor, GA_ROOT) : nullptr;
    if (IsMenuWindow(cursorRoot)) {
        return true;
    }

    RECT flyoutRect = {};
    if (!flyout || !GetWindowRect(flyout, &flyoutRect)) {
        return false;
    }

    // Tray icon menus are normally opened adjacent to the flyout. A bounded
    // neighborhood catches menus hosted by tray applications as well as Explorer.
    InflateRect(&flyoutRect, 300, 240);

    HWND menu = nullptr;
    while ((menu = FindWindowExW(nullptr, menu, L"#32768", nullptr))) {
        if (!IsWindowVisible(menu)) {
            continue;
        }

        RECT menuRect = {};
        if (!GetWindowRect(menu, &menuRect)) {
            continue;
        }

        DWORD menuPid = 0;
        GetWindowThreadProcessId(menu, &menuPid);

        HWND owner = GetWindow(menu, GW_OWNER);
        HWND ownerRoot = owner ? GetAncestor(owner, GA_ROOT) : nullptr;
        bool taskbarOwned = menuPid == taskbarPid || ownerRoot == flyout ||
                            owner == flyout;
        RECT intersection = {};
        bool nearFlyout = IntersectRect(
            &intersection, &menuRect, &flyoutRect) != FALSE;

        if (taskbarOwned || nearFlyout) {
            return true;
        }
    }

    return false;
}

// Collapse through the chevron's UIA patterns, never by sending Escape.
// If Collapse is unsupported or fails on a particular Windows build, Invoke
// is attempted as a toggle fallback.
HRESULT CollapseOverflow(IUIAutomationElement* chevron) {
    if (!chevron) {
        return E_POINTER;
    }

    HRESULT collapseHr = E_NOINTERFACE;
    IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
    HRESULT hr = chevron->GetCurrentPatternAs(
        UIA_ExpandCollapsePatternId,
        __uuidof(IUIAutomationExpandCollapsePattern),
        reinterpret_cast<void**>(&expandPattern));

    if (SUCCEEDED(hr) && expandPattern) {
        ExpandCollapseState state = ExpandCollapseState_LeafNode;
        hr = expandPattern->get_CurrentExpandCollapseState(&state);
        if (SUCCEEDED(hr) && state == ExpandCollapseState_Expanded) {
            collapseHr = expandPattern->Collapse();
        } else if (SUCCEEDED(hr) && state == ExpandCollapseState_Collapsed) {
            collapseHr = S_OK;
        } else {
            collapseHr = FAILED(hr) ? hr : E_FAIL;
        }
        expandPattern->Release();

        if (SUCCEEDED(collapseHr)) {
            return collapseHr;
        }
    } else {
        collapseHr = FAILED(hr) ? hr : E_NOINTERFACE;
    }

    IUIAutomationInvokePattern* invokePattern = nullptr;
    hr = chevron->GetCurrentPatternAs(
        UIA_InvokePatternId,
        __uuidof(IUIAutomationInvokePattern),
        reinterpret_cast<void**>(&invokePattern));

    if (SUCCEEDED(hr) && invokePattern) {
        hr = invokePattern->Invoke();
        invokePattern->Release();
        return hr;
    }

    return FAILED(hr) ? hr : collapseHr;
}

DWORD WINAPI HoverMonitorThread(LPVOID) {
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coHr)) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", coHr);
        return 1;
    }

    IUIAutomation* automation = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
        IID_IUIAutomation, reinterpret_cast<void**>(&automation));

    if (FAILED(hr) || !automation) {
        Wh_Log(L"Failed to initialize UI Automation: 0x%08X", hr);
        CoUninitialize();
        return 1;
    }

    int currentHoverTime = 0;
    int currentCloseHoverTime = 0;
    bool collapseRequested = false;
    bool clickClosePending = false;
    DWORD clickReleaseTime = 0;
    bool wasLeftDown = false;
    bool wasRightDown = false;
    IUIAutomationElement* openedChevron = nullptr;

    while (InterlockedCompareExchange(&g_modUnloading, 0, 0) == 0) {
        Sleep(50);

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            currentHoverTime = 0;
            continue;
        }

        HWND primaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD taskbarPid = 0;
        if (primaryTaskbar) {
            GetWindowThreadProcessId(primaryTaskbar, &taskbarPid);
        }

        // This inexpensive Win32 gate is the only path to ElementFromPoint/UIA.
        DWORD pointTaskbarPid = 0;
        bool overTaskbar = IsTaskbarWindowAtPoint(pt, &pointTaskbarPid) &&
                           pointTaskbarPid == taskbarPid;

        bool isOverflow = false;
        IUIAutomationElement* overflowElement = nullptr;
        IUIAutomationInvokePattern* invokePattern = nullptr;
        IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
        IUIAutomationTogglePattern* togglePattern = nullptr;

        if (overTaskbar) {
            isOverflow = FindTaskbarOverflowElement(
                automation, pt, taskbarPid, &overflowElement,
                &invokePattern, &expandPattern, &togglePattern);
        }

        HWND flyout = taskbarPid ? GetVisibleFlyout(taskbarPid) : nullptr;
        bool flyoutVisible = flyout != nullptr;

        // Recognize release-inside-flyout clicks. Wait briefly for the tray app
        // to receive the click and for any standard popup menu to appear.
        SHORT leftState = GetAsyncKeyState(VK_LBUTTON);
        SHORT rightState = GetAsyncKeyState(VK_RBUTTON);
        bool leftDown = (leftState & 0x8000) != 0;
        bool rightDown = (rightState & 0x8000) != 0;
        bool leftPressedSincePoll = (leftState & 0x0001) != 0;
        bool rightPressedSincePoll = (rightState & 0x0001) != 0;
        bool mouseReleased = (wasLeftDown && !leftDown) ||
                             (wasRightDown && !rightDown) ||
                             (!leftDown && leftPressedSincePoll) ||
                             (!rightDown && rightPressedSincePoll);
        wasLeftDown = leftDown;
        wasRightDown = rightDown;

        if (mouseReleased && flyoutVisible && PtOverWindow(flyout, pt)) {
            clickClosePending = true;
            clickReleaseTime = GetTickCount();
        }

        // Synchronize our state with Windows. If Windows has already dismissed
        // the flyout, discard our state and do not send any keyboard input.
        if (!flyoutVisible) {
            collapseRequested = false;
            clickClosePending = false;
            currentCloseHoverTime = 0;

            if (openedChevron) {
                openedChevron->Release();
                openedChevron = nullptr;
            }
        }

        // Open only when the actual overflow XAML island is not already visible.
        if (overTaskbar && isOverflow && !flyoutVisible) {
            currentHoverTime += 50;

            if (currentHoverTime >= settings.hoverDelayMs) {
                HRESULT actionHr = E_FAIL;

                if (expandPattern) {
                    actionHr = expandPattern->Expand();
                }
                if (FAILED(actionHr) && invokePattern) {
                    actionHr = invokePattern->Invoke();
                }
                if (FAILED(actionHr) && togglePattern) {
                    actionHr = togglePattern->Toggle();
                }

                if (SUCCEEDED(actionHr)) {
                    if (openedChevron) {
                        openedChevron->Release();
                    }
                    openedChevron = overflowElement;
                    openedChevron->AddRef();
                    collapseRequested = false;
                    currentCloseHoverTime = 0;
                } else {
                    Wh_Log(L"Overflow open action failed: 0x%08X", actionHr);
                }

                currentHoverTime = 0;
            }
        } else {
            currentHoverTime = 0;
        }

        // Auto-close only a flyout Windows confirms is visible. A click inside
        // the flyout schedules a close after a short grace period. If the click
        // opens a tray popup menu, wait until that menu is dismissed first.
        if (flyoutVisible && !collapseRequested) {
            bool overFlyout = PtOverWindow(flyout, pt);
            bool popupMenuOpen = IsPopupMenuOpen(flyout, taskbarPid, pt);
            bool shouldCollapse = false;

            if (clickClosePending) {
                if (!popupMenuOpen &&
                    static_cast<DWORD>(GetTickCount() - clickReleaseTime) >= 150) {
                    shouldCollapse = true;
                }
                currentCloseHoverTime = 0;
            } else if (isOverflow || overFlyout || popupMenuOpen) {
                currentCloseHoverTime = 0;
            } else {
                currentCloseHoverTime += 50;
                shouldCollapse = currentCloseHoverTime >= settings.closeDelayMs;
            }

            if (shouldCollapse) {
                HRESULT closeHr = CollapseOverflow(openedChevron);
                if (SUCCEEDED(closeHr)) {
                    collapseRequested = true;
                    clickClosePending = false;
                    currentCloseHoverTime = 0;
                } else {
                    Wh_Log(L"Overflow collapse failed: 0x%08X", closeHr);
                    // Retry after the configured close delay if the reference
                    // was temporarily unavailable or the UIA pattern was stale.
                    currentCloseHoverTime = 0;
                }
            }
        } else {
            currentCloseHoverTime = 0;
        }

        if (invokePattern) invokePattern->Release();
        if (expandPattern) expandPattern->Release();
        if (togglePattern) togglePattern->Release();
        if (overflowElement) overflowElement->Release();
    }

    if (openedChevron) {
        openedChevron->Release();
    }

    automation->Release();
    CoUninitialize();
    return 0;
}

void LoadSettings() {
    settings.hoverDelayMs = Wh_GetIntSetting(L"hoverDelayMs");
    settings.closeDelayMs = Wh_GetIntSetting(L"closeDelayMs");

    if (settings.hoverDelayMs < 0) settings.hoverDelayMs = 0;
    if (settings.hoverDelayMs > 5000) settings.hoverDelayMs = 5000;
    if (settings.closeDelayMs < 0) settings.closeDelayMs = 0;
    if (settings.closeDelayMs > 5000) settings.closeDelayMs = 5000;
}

// The dedicated tool process calls these callbacks from the launcher below.
BOOL WhTool_ModInit() {
    LoadSettings();
    InterlockedExchange(&g_modUnloading, 0);

    g_hThread = CreateThread(nullptr, 0, HoverMonitorThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"Failed to start hover monitor thread: %u", GetLastError());
        return FALSE;
    }

    Wh_Log(L"Taskbar Overflow on Hover tool process initialized");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    InterlockedExchange(&g_modUnloading, 1);

    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
