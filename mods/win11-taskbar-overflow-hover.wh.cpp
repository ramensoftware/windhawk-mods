// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar Overflow on Hover
// @description     Opens the Windows 11 taskbar overflow menu on hover and closes it when you move away or click.
// @version         1.2
// @author          Cougar16
// @github         https://github.com/Cougar16
// @include        windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Overflow on Hover 🚀

A zero-click QoL upgrade for Windows 11. Hover over the taskbar overflow button
to open the hidden Apps icons.

![Hover overflow menu](https://raw.githubusercontent.com/Cougar16/win11-taskbar-overflow-hover/main/assets/Hover-Overflow-Menu.png)

Features:
- Opens the overflow menu shortly after you hover the overflow button.
- Closes the flyout as soon as the cursor leaves the combined area of the
  button and the menu (the gap between them counts as part of the area).
- Dismisses the flyout after you click an icon inside it.
- Keeps the flyout open while a tray icon's popup menu is showing.
- UI Automation is queried only when the cursor is over a taskbar window.
- The flyout is closed through the overflow button itself (UI Automation),
  no keystrokes are sent to other applications.
- No synthetic mouse-click fallback is used.
- Runs as a dedicated Windhawk tool process, not inside Explorer.

Developed by Cougar16.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>

#include <cwchar>

// Time the cursor must rest on the overflow button before the menu opens.
constexpr int kHoverDelayMs = 100;

// Time the cursor must stay outside the button + menu area before the menu
// closes (a tiny debounce so a brief overshoot does not make it flicker).
constexpr int kCloseDelayMs = 150;

// Signalled on unload to stop the monitor thread (also interrupts its sleep).
HANDLE g_stopEvent = nullptr;
HANDLE g_hThread = nullptr;

// After invoking the chevron the flyout needs a moment to appear. During this
// grace period a missing flyout window is not treated as "closed".
constexpr ULONGLONG kOpenGraceMs = 600;

// After a click inside the flyout, wait this long before deciding whether the
// click opened a popup menu (keep the flyout) or not (dismiss the flyout).
constexpr ULONGLONG kClickSettleMs = 100;

constexpr wchar_t kOverflowFlyoutClass[] = L"TopLevelWindowForOverflowXamlIsland";

// Return true only when the point belongs to a taskbar top-level window
// owned by the same process as the primary taskbar.
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

// Returns the real overflow flyout window if it is currently visible and owned
// by the taskbar's process, otherwise nullptr.
HWND GetVisibleFlyout(DWORD taskbarPid) {
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, kOverflowFlyoutClass, nullptr)) !=
           nullptr) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid != taskbarPid || !IsWindowVisible(hWnd)) {
            continue;
        }

        RECT rc = {};
        if (GetWindowRect(hWnd, &rc) && !IsRectEmpty(&rc)) {
            return hWnd;
        }
    }

    return nullptr;
}

// True when the point lies inside the flyout window.
bool IsPointInFlyout(HWND hFlyout, POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    if (hWnd && GetAncestor(hWnd, GA_ROOT) == hFlyout) {
        return true;
    }

    RECT rc = {};
    return GetWindowRect(hFlyout, &rc) && PtInRect(&rc, pt);
}

// True while a popup menu (for example a tray icon's context menu) is showing.
bool IsPopupMenuOpen() {
    GUITHREADINFO gti = {};
    gti.cbSize = sizeof(gti);
    if (GetGUIThreadInfo(0, &gti) &&
        (gti.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE))) {
        return true;
    }

    HWND hMenu = nullptr;
    while ((hMenu = FindWindowExW(nullptr, hMenu, L"#32768", nullptr)) != nullptr) {
        if (IsWindowVisible(hMenu)) {
            return true;
        }
    }

    return false;
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
                UIA_InvokePatternId, reinterpret_cast<IUnknown**>(&invokePattern));
            node->GetCurrentPattern(
                UIA_ExpandCollapsePatternId, reinterpret_cast<IUnknown**>(&expandPattern));
            node->GetCurrentPattern(
                UIA_TogglePatternId, reinterpret_cast<IUnknown**>(&togglePattern));

            if (invokePattern || expandPattern || togglePattern) {
                *elementOut = node;
                *invokeOut = invokePattern;
                *expandOut = expandPattern;
                *toggleOut = togglePattern;
                node = nullptr; // Ownership transferred to elementOut.
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

// Looks the overflow button up again from the primary taskbar window. Used when
// the cached element is gone or stale. Only elements owned by the taskbar's
// process are accepted.
bool FindChevronInTaskbar(
    IUIAutomation* automation,
    HWND hTaskbar,
    DWORD taskbarPid,
    IUIAutomationElement** elementOut) {

    *elementOut = nullptr;

    if (!hTaskbar) {
        return false;
    }

    IUIAutomationElement* root = nullptr;
    HRESULT hr = automation->ElementFromHandle(hTaskbar, &root);
    if (FAILED(hr) || !root) {
        return false;
    }

    VARIANT v1;
    VARIANT v2;
    VariantInit(&v1);
    VariantInit(&v2);
    v1.vt = VT_BSTR;
    v1.bstrVal = SysAllocString(L"Chevron");
    v2.vt = VT_BSTR;
    v2.bstrVal = SysAllocString(L"OverflowButton");

    IUIAutomationCondition* cond1 = nullptr;
    IUIAutomationCondition* cond2 = nullptr;
    IUIAutomationCondition* condOr = nullptr;
    IUIAutomationElement* found = nullptr;

    if (v1.bstrVal && v2.bstrVal &&
        SUCCEEDED(automation->CreatePropertyCondition(
            UIA_AutomationIdPropertyId, v1, &cond1)) &&
        SUCCEEDED(automation->CreatePropertyCondition(
            UIA_AutomationIdPropertyId, v2, &cond2)) &&
        SUCCEEDED(automation->CreateOrCondition(cond1, cond2, &condOr)) &&
        condOr) {
        root->FindFirst(TreeScope_Descendants, condOr, &found);
    }

    if (condOr) condOr->Release();
    if (cond2) cond2->Release();
    if (cond1) cond1->Release();
    VariantClear(&v1);
    VariantClear(&v2);
    root->Release();

    if (!found) {
        return false;
    }

    int elementPid = 0;
    if (FAILED(found->get_CurrentProcessId(&elementPid)) ||
        elementPid != static_cast<int>(taskbarPid)) {
        found->Release();
        return false;
    }

    *elementOut = found;
    return true;
}

// Performs the "close" counterpart of the open action on the overflow button,
// using the same pattern priority as opening:
// Invoke (toggles) -> ExpandCollapse::Collapse -> Toggle.
bool CloseOverflowElement(IUIAutomationElement* element) {
    if (!element) {
        return false;
    }

    IUIAutomationInvokePattern* invokePattern = nullptr;
    IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
    IUIAutomationTogglePattern* togglePattern = nullptr;

    element->GetCurrentPattern(
        UIA_InvokePatternId, reinterpret_cast<IUnknown**>(&invokePattern));
    element->GetCurrentPattern(
        UIA_ExpandCollapsePatternId, reinterpret_cast<IUnknown**>(&expandPattern));
    element->GetCurrentPattern(
        UIA_TogglePatternId, reinterpret_cast<IUnknown**>(&togglePattern));

    HRESULT hr = E_FAIL;
    if (invokePattern) {
        hr = invokePattern->Invoke();
    } else if (expandPattern) {
        hr = expandPattern->Collapse();
    } else if (togglePattern) {
        hr = togglePattern->Toggle();
    }

    if (invokePattern) invokePattern->Release();
    if (expandPattern) expandPattern->Release();
    if (togglePattern) togglePattern->Release();

    return SUCCEEDED(hr);
}

// Closes the flyout through the overflow button. Tries the cached element first
// and falls back to looking the button up again if the cached one went stale.
bool CloseOverflowFlyout(
    IUIAutomation* automation,
    IUIAutomationElement** cachedChevron,
    HWND hTaskbar,
    DWORD taskbarPid) {

    if (*cachedChevron && CloseOverflowElement(*cachedChevron)) {
        return true;
    }

    if (*cachedChevron) {
        (*cachedChevron)->Release();
        *cachedChevron = nullptr;
    }

    if (FindChevronInTaskbar(automation, hTaskbar, taskbarPid, cachedChevron) &&
        CloseOverflowElement(*cachedChevron)) {
        return true;
    }

    return false;
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
    bool menuOpened = false;      // True only while the mod owns an open flyout.
    bool wasMouseDown = false;
    bool clickPending = false;    // A click inside the flyout is waiting to dismiss it.
    ULONGLONG openedAt = 0;
    ULONGLONG clickAt = 0;
    IUIAutomationElement* chevron = nullptr; // Cached overflow button used to close.
    RECT chevronRect = {};                   // Button bounds, captured when opened.
    bool haveChevronRect = false;

    // Forget everything about the flyout the mod opened.
    auto resetOwnership = [&]() {
        menuOpened = false;
        clickPending = false;
        haveChevronRect = false;
        currentHoverTime = 0;
        currentCloseHoverTime = 0;
        if (chevron) {
            chevron->Release();
            chevron = nullptr;
        }
    };

    // The 50 ms wait doubles as the poll interval and is interrupted by the
    // stop event, so unloading never has to wait for a sleep to finish.
    while (WaitForSingleObject(g_stopEvent, 50) == WAIT_TIMEOUT) {
        const ULONGLONG now = GetTickCount64();

        // Read the real flyout state instead of trusting a local guess.
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD flyoutOwnerPid = 0;
        if (hTaskbar) {
            GetWindowThreadProcessId(hTaskbar, &flyoutOwnerPid);
        }
        HWND hFlyout = flyoutOwnerPid ? GetVisibleFlyout(flyoutOwnerPid) : nullptr;

        // The flyout was closed some other way (Win key, Alt+Tab, Escape typed
        // by the user, click elsewhere...). Stop considering it ours.
        if (menuOpened && !hFlyout && now - openedAt >= kOpenGraceMs) {
            resetOwnership();
        }

        auto closeFlyout = [&]() {
            if (!CloseOverflowFlyout(automation, &chevron, hTaskbar, flyoutOwnerPid)) {
                Wh_Log(L"Failed to close the overflow flyout");
            }
            resetOwnership();
        };

        bool isMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool mouseClicked = wasMouseDown && !isMouseDown;
        wasMouseDown = isMouseDown;

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            currentHoverTime = 0;
            continue;
        }

        // Only a click inside the flyout counts as "clicked an icon".
        if (menuOpened && hFlyout && mouseClicked && IsPointInFlyout(hFlyout, pt)) {
            clickPending = true;
            clickAt = now;
        }

        if (clickPending) {
            if (!menuOpened || !hFlyout) {
                clickPending = false;
            } else if (now - clickAt >= kClickSettleMs && !IsPopupMenuOpen()) {
                // The clicked icon did not open a popup menu: dismiss the flyout.
                // (While a popup menu is showing, the flyout is kept open and
                // is dismissed once the menu is gone.)
                closeFlyout();
                continue;
            }
        }

        DWORD taskbarPid = 0;
        bool overTaskbar = IsTaskbarWindowAtPoint(pt, &taskbarPid);
        bool isOverflow = false;

        if (overTaskbar) {
            IUIAutomationElement* overflowElement = nullptr;
            IUIAutomationInvokePattern* invokePattern = nullptr;
            IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
            IUIAutomationTogglePattern* togglePattern = nullptr;

            isOverflow = FindTaskbarOverflowElement(
                automation, pt, taskbarPid, &overflowElement,
                &invokePattern, &expandPattern, &togglePattern);

            // Do not toggle a flyout that is already open (for example one the
            // user opened with a click).
            if (isOverflow && !menuOpened && !hFlyout) {
                currentHoverTime += 50;

                if (currentHoverTime >= kHoverDelayMs) {
                    HRESULT actionHr = E_FAIL;

                    if (invokePattern) {
                        actionHr = invokePattern->Invoke();
                    } else if (expandPattern) {
                        actionHr = expandPattern->Expand();
                    } else if (togglePattern) {
                        actionHr = togglePattern->Toggle();
                    }

                    if (SUCCEEDED(actionHr)) {
                        menuOpened = true;
                        openedAt = now;
                        currentCloseHoverTime = 0;
                        clickPending = false;

                        // Keep the button so the flyout can be closed through it.
                        if (chevron) {
                            chevron->Release();
                        }
                        chevron = overflowElement;
                        chevron->AddRef();

                        // Remember where the button is, so the area between the
                        // button and the menu counts as "still hovering".
                        RECT rc = {};
                        haveChevronRect =
                            SUCCEEDED(chevron->get_CurrentBoundingRectangle(&rc)) &&
                            !IsRectEmpty(&rc);
                        if (haveChevronRect) {
                            chevronRect = rc;
                        }
                    } else {
                        Wh_Log(L"Overflow action failed: 0x%08X", actionHr);
                    }

                    currentHoverTime = 0;
                }
            } else {
                currentHoverTime = 0;
            }

            if (invokePattern) invokePattern->Release();
            if (expandPattern) expandPattern->Release();
            if (togglePattern) togglePattern->Release();
            if (overflowElement) overflowElement->Release();
        } else {
            currentHoverTime = 0;
        }

        // Closing logic: while the flyout is open, the safe cursor area is the
        // overflow button, the flyout, and the space between them (their
        // combined bounding box). Nothing is closed while a tray icon's popup
        // menu is showing. Do not run UIA here.
        if (menuOpened) {
            if (hFlyout) {
                bool inArea = isOverflow || IsPointInFlyout(hFlyout, pt);

                if (!inArea && haveChevronRect) {
                    RECT flyoutRect = {};
                    if (GetWindowRect(hFlyout, &flyoutRect)) {
                        RECT area = {};
                        UnionRect(&area, &chevronRect, &flyoutRect);
                        inArea = PtInRect(&area, pt) != FALSE;
                    }
                }

                if (inArea || IsPopupMenuOpen()) {
                    currentCloseHoverTime = 0;
                } else {
                    currentCloseHoverTime += 50;

                    if (currentCloseHoverTime >= kCloseDelayMs) {
                        closeFlyout();
                    }
                }
            }
        } else {
            currentCloseHoverTime = 0;
        }
    }

    if (chevron) {
        chevron->Release();
        chevron = nullptr;
    }

    automation->Release();
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Taskbar Overflow on Hover v1.2");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, HoverMonitorThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading Taskbar Overflow on Hover");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Wait without a timeout: the thread must be fully finished before the
    // process continues to tear down.
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

// -----------------------------------------------------------------------------
// Windhawk tool-mod launcher implementation.
// Based on the official Windhawk wiki:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
// -----------------------------------------------------------------------------

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandle(nullptr));
        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<IMAGE_NT_HEADERS*>(
                reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook), nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
