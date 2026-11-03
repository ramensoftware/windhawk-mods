// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar Overflow on Hover
// @description     Opens the Windows 11 taskbar overflow menu on hover and closes it when you move away or click.
// @version         1.0
// @author          Cougar16
// @github         https://github.com/Cougar16
// @include        windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Overflow on Hover 🚀

A zero-click QoL upgrade for Windows 11. Hover over the taskbar overflow button
to open the hidden running Apps icons.

![Hover overflow menu](https://raw.githubusercontent.com/Cougar16/win11-taskbar-overflow-hover/main/assets/Hover-Overflow-Menu.png)

Features:
- Opens the overflow menu after a configurable hover delay.
- Closes the flyout after the cursor leaves it.
- Dismisses the flyout after a click.
- UI Automation is queried only when the cursor is over a taskbar window.
- No synthetic mouse-click fallback is used.
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

volatile bool g_modUnloading = false;
HANDLE g_hThread = NULL;

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

void SendEscapeKey() {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_ESCAPE;
    SendInput(1, &input, sizeof(input));

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
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
    bool menuOpened = false;
    bool wasMouseDown = false;

    while (!g_modUnloading) {
        Sleep(50);

        // Dismiss after a click while the mod believes the flyout is open.
        bool isMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool mouseClicked = wasMouseDown && !isMouseDown;
        wasMouseDown = isMouseDown;

        if (menuOpened && mouseClicked) {
            // Allow the clicked tray app/button to process the click first.
            Sleep(30);
            SendEscapeKey();
            menuOpened = false;
            currentCloseHoverTime = 0;
            currentHoverTime = 0;
            continue;
        }

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            currentHoverTime = 0;
            continue;
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

            if (isOverflow && !menuOpened) {
                currentHoverTime += 50;

                if (currentHoverTime >= settings.hoverDelayMs) {
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
                        currentCloseHoverTime = 0;
                    } else {
                        Wh_Log(L"Overflow action failed: 0x%08X", actionHr);
                    }

                    currentHoverTime = 0;
                }
            } else if (!isOverflow) {
                currentHoverTime = 0;
            }

            if (invokePattern) invokePattern->Release();
            if (expandPattern) expandPattern->Release();
            if (togglePattern) togglePattern->Release();
            if (overflowElement) overflowElement->Release();
        } else {
            currentHoverTime = 0;
        }

        // Closing logic: while open, consider the taskbar and Explorer-owned
        // popup windows to be safe cursor locations. Do not run UIA here.
        if (menuOpened) {
            bool inFlyout = false;

            HWND hWndPoint = WindowFromPoint(pt);
            HWND hRootPoint = hWndPoint ? GetAncestor(hWndPoint, GA_ROOT) : nullptr;

            if (hRootPoint) {
                wchar_t rootClass[256] = {};
                GetClassNameW(hRootPoint, rootClass, ARRAYSIZE(rootClass));

                DWORD pointPid = 0;
                GetWindowThreadProcessId(hRootPoint, &pointPid);

                HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
                DWORD explorerPid = 0;
                if (hTaskbar) {
                    GetWindowThreadProcessId(hTaskbar, &explorerPid);
                }

                bool isTaskbarRoot =
                    wcscmp(rootClass, L"Shell_TrayWnd") == 0 ||
                    wcscmp(rootClass, L"Shell_SecondaryTrayWnd") == 0;

                bool isDesktopRoot =
                    wcscmp(rootClass, L"WorkerW") == 0 ||
                    wcscmp(rootClass, L"Progman") == 0;

                if (pointPid == explorerPid && !isTaskbarRoot && !isDesktopRoot) {
                    inFlyout = true;
                }
            }

            if (isOverflow || inFlyout) {
                currentCloseHoverTime = 0;
            } else {
                currentCloseHoverTime += 50;

                if (currentCloseHoverTime >= settings.closeDelayMs) {
                    SendEscapeKey();
                    menuOpened = false;
                    currentCloseHoverTime = 0;
                    currentHoverTime = 0;
                }
            }
        } else {
            currentCloseHoverTime = 0;
        }
    }

    automation->Release();
    CoUninitialize();
    return 0;
}

void LoadSettings() {
    settings.hoverDelayMs = Wh_GetIntSetting(L"hoverDelayMs");
    settings.closeDelayMs = Wh_GetIntSetting(L"closeDelayMs");

    // Avoid zero/negative values causing immediate or confusing behavior.
    if (settings.hoverDelayMs < 0) settings.hoverDelayMs = 0;
    if (settings.closeDelayMs < 0) settings.closeDelayMs = 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Taskbar Overflow on Hover v1.1");
    LoadSettings();

    g_modUnloading = false;
    g_hThread = CreateThread(nullptr, 0, HoverMonitorThread, nullptr, 0, nullptr);
    return g_hThread != nullptr;
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading Taskbar Overflow on Hover");
    g_modUnloading = true;

    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed; reloading delays.");
    LoadSettings();
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
