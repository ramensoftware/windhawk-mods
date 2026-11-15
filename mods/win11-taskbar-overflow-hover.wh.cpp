// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar Overflow on Hover
// @description     Opens the Windows 11 taskbar overflow menu on hover and closes it when you move away or click.
// @version         1.0.0
// @author          Cougar16
// @github         https://github.com/Cougar16
// @include        windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Overflow on Hover 🚀

A zero-click QoL upgrade for Windows 11. Hover over the taskbar overflow button
to open the hidden Apps icons.

![Hover overflow menu](https://raw.githubusercontent.com/Cougar16/win11-taskbar-overflow-hover/main/assets/Hover-Overflow-Menu.png)

✨ Features:
- Zero-Click Open: Auto-expands the hidden icons menu just by hovering.
- Auto-Dismiss: Gracefully closes the menu automatically the moment your mouse leaves the overflow button or the flyout.
- Instant App Launch: Instantly dismisses the overflow flyout the exact millisecond you click an app inside it, completely bypassing the native Windows UI lag.

Developed by Cougar16 for the Windows customization community.
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

// After toggling the button the flyout needs a moment to appear. During this
// grace period the cursor being away from it does not close it.
constexpr ULONGLONG kOpenGraceMs = 600;

// After a click inside the flyout, wait this long before deciding whether the
// click opened a popup menu (keep the flyout) or not (dismiss the flyout).
constexpr ULONGLONG kClickSettleMs = 100;

// The app overflow ("...") flyout and the thumbnail previews are each hosted in
// a window of this class on the taskbar's thread. The tray's "Show hidden icons"
// popup is a different window, TopLevelWindowForOverflowXamlIsland.
constexpr wchar_t kXamlHostClass[] = L"XamlExplorerHostIslandWindow";

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

// Gets the on-screen bounds of a visible XAML host. A host can span the whole
// monitor with a window region limiting it to its UI, so its window rect alone
// says nothing about where that UI is.
bool GetXamlHostBounds(HWND hWnd, RECT* bounds) {
    RECT windowRect = {};
    if (!IsWindowVisible(hWnd) || !GetWindowRect(hWnd, &windowRect)) {
        return false;
    }

    *bounds = windowRect;

    HRGN region = CreateRectRgn(0, 0, 0, 0);
    if (region) {
        // GetWindowRgn fails for a window without a region.
        if (GetWindowRgn(hWnd, region) != ERROR &&
            GetRgnBox(region, bounds) != ERROR) {
            OffsetRect(bounds, windowRect.left, windowRect.top);
        }
        DeleteObject(region);
    }

    return !IsRectEmpty(bounds);
}

// Scans the visible XAML hosts on the taskbar's UI thread. The thread match
// excludes the other XAML islands that live in Explorer (Alt+Tab, Task View...).
// Returns true if the point is over one of them. flyoutBounds receives the
// bounds of the host nearest the overflow button: that is the flyout, since the
// preview of an icon inside the flyout appears beyond it.
bool QueryXamlHosts(HWND hTaskbar,
                    const RECT& chevronRect,
                    POINT pt,
                    RECT* flyoutBounds) {
    *flyoutBounds = {};

    DWORD taskbarPid = 0;
    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbar, &taskbarPid);
    if (!taskbarPid || !taskbarThreadId) {
        return false;
    }

    const LONGLONG chevronCenterX = (chevronRect.left + chevronRect.right) / 2;
    const LONGLONG chevronCenterY = (chevronRect.top + chevronRect.bottom) / 2;

    bool overHost = false;
    LONGLONG nearestDistance = -1;

    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, kXamlHostClass, nullptr)) !=
           nullptr) {
        DWORD pid = 0;
        DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
        if (pid != taskbarPid || threadId != taskbarThreadId) {
            continue;
        }

        RECT bounds = {};
        if (!GetXamlHostBounds(hWnd, &bounds)) {
            continue;
        }

        overHost = overHost || PtInRect(&bounds, pt);

        const LONGLONG dx = (bounds.left + bounds.right) / 2 - chevronCenterX;
        const LONGLONG dy = (bounds.top + bounds.bottom) / 2 - chevronCenterY;
        const LONGLONG distance = dx * dx + dy * dy;
        if (nearestDistance < 0 || distance < nearestDistance) {
            nearestDistance = distance;
            *flyoutBounds = bounds;
        }
    }

    return overHost;
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
    IUIAutomationTogglePattern** toggleOut) {

    *elementOut = nullptr;
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
            IUIAutomationTogglePattern* togglePattern = nullptr;
            node->GetCurrentPatternAs(
                UIA_TogglePatternId, IID_PPV_ARGS(&togglePattern));

            if (togglePattern) {
                *elementOut = node;
                *toggleOut = togglePattern;
                node = nullptr; // Ownership transferred to elementOut.
                found = true;
                break;
            }
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

// The flyout is open exactly while the overflow button is toggled on. A stale
// button reports an error, which counts as closed.
bool IsOverflowOpen(IUIAutomationTogglePattern* toggle) {
    ToggleState state = ToggleState_Off;
    return SUCCEEDED(toggle->get_CurrentToggleState(&state)) &&
           state == ToggleState_On;
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
    bool hoverConsumed = false;   // No opening until the cursor leaves the button.
    bool wasMouseDown = false;
    bool clickPending = false;    // A click inside the flyout is waiting to dismiss it.
    ULONGLONG openedAt = 0;
    ULONGLONG clickAt = 0;
    IUIAutomationTogglePattern* chevron = nullptr; // Set while menuOpened.
    RECT chevronRect = {};                         // Button bounds, captured when opened.

    // Forget everything about the flyout the mod opened.
    auto resetOwnership = [&]() {
        menuOpened = false;
        clickPending = false;
        currentHoverTime = 0;
        currentCloseHoverTime = 0;
        if (chevron) {
            chevron->Release();
            chevron = nullptr;
        }
    };

    auto closeFlyout = [&]() {
        HRESULT closeHr = chevron->Toggle();
        if (FAILED(closeHr)) {
            Wh_Log(L"Failed to close the overflow flyout: 0x%08X", closeHr);
        }
        resetOwnership();
    };

    // The 50 ms wait doubles as the poll interval and is interrupted by the
    // stop event, so unloading never has to wait for a sleep to finish.
    while (WaitForSingleObject(g_stopEvent, 50) == WAIT_TIMEOUT) {
        const ULONGLONG now = GetTickCount64();

        // The flyout was closed some other way (Win key, Alt+Tab, Escape typed
        // by the user, click elsewhere...). Stop considering it ours.
        if (menuOpened && !IsOverflowOpen(chevron)) {
            resetOwnership();
        }

        bool isMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool mouseClicked = wasMouseDown && !isMouseDown;
        wasMouseDown = isMouseDown;

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            currentHoverTime = 0;
            continue;
        }

        RECT flyoutBounds = {};
        bool overXamlHost =
            menuOpened && QueryXamlHosts(FindWindowW(L"Shell_TrayWnd", nullptr),
                                         chevronRect, pt, &flyoutBounds);

        // Only a click inside the flyout counts as "clicked an icon".
        if (mouseClicked && overXamlHost) {
            clickPending = true;
            clickAt = now;
        }

        // A clicked icon that opened a popup menu keeps the flyout open until
        // the menu is gone.
        if (clickPending && now - clickAt >= kClickSettleMs && !IsPopupMenuOpen()) {
            closeFlyout();
            continue;
        }

        DWORD taskbarPid = 0;
        bool overTaskbar = IsTaskbarWindowAtPoint(pt, &taskbarPid);
        bool isOverflow = false;

        if (overTaskbar) {
            IUIAutomationElement* overflowElement = nullptr;
            IUIAutomationTogglePattern* togglePattern = nullptr;

            isOverflow = FindTaskbarOverflowElement(
                automation, pt, taskbarPid, &overflowElement, &togglePattern);

            // Do not toggle a flyout that is already open (for example one the
            // user opened with a click), and do not reopen it for this hover
            // if the user closes it by clicking the button.
            if (isOverflow && IsOverflowOpen(togglePattern)) {
                hoverConsumed = true;
            }

            if (isOverflow && !hoverConsumed) {
                currentHoverTime += 50;

                if (currentHoverTime >= kHoverDelayMs) {
                    HRESULT actionHr = togglePattern->Toggle();

                    if (SUCCEEDED(actionHr)) {
                        menuOpened = true;
                        hoverConsumed = true;
                        openedAt = now;
                        currentCloseHoverTime = 0;
                        clickPending = false;

                        // Keep the button so the flyout can be closed through it.
                        if (chevron) {
                            chevron->Release();
                        }
                        chevron = togglePattern;
                        chevron->AddRef();

                        // Remember where the button is, so the area between the
                        // button and the menu counts as "still hovering".
                        if (FAILED(overflowElement->get_CurrentBoundingRectangle(
                                &chevronRect))) {
                            chevronRect = {};
                        }
                    } else {
                        Wh_Log(L"Overflow action failed: 0x%08X", actionHr);
                    }

                    currentHoverTime = 0;
                }
            } else {
                currentHoverTime = 0;
            }

            if (togglePattern) togglePattern->Release();
            if (overflowElement) overflowElement->Release();
        } else {
            currentHoverTime = 0;
        }

        if (!isOverflow) {
            hoverConsumed = false;
        }

        // Closing logic: while the flyout is open, the safe cursor area is the
        // overflow button, the flyout, and the space between them (their
        // combined bounding box), plus a thumbnail preview that is showing.
        // Nothing is closed while a tray icon's popup menu is showing.
        if (menuOpened) {
            bool flyoutSettled =
                !IsRectEmpty(&flyoutBounds) || now - openedAt >= kOpenGraceMs;

            RECT area = {};
            UnionRect(&area, &chevronRect, &flyoutBounds);
            bool inArea = isOverflow || overXamlHost || PtInRect(&area, pt);

            if (inArea || !flyoutSettled || IsPopupMenuOpen()) {
                currentCloseHoverTime = 0;
            } else {
                currentCloseHoverTime += 50;

                if (currentCloseHoverTime >= kCloseDelayMs) {
                    closeFlyout();
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

// The mod has no settings, but the tool-mod launcher pattern requires the
// settings-changed forwarder to exist.
void WhTool_ModSettingsChanged() {}

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

    WCHAR
    commandLine[MAX_PATH + 2 +
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
