// ==WindhawkMod==
// @id              middle-click-new-tab
// @name            Middle Click New Tab
// @description     Middle-click the empty tab strip area of File Explorer or Notepad to open a new tab
// @version         1.0.3
// @author          venasataa
// @github          https://github.com/venasataa
// @homepage        https://venasataa.github.io
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*

This mod adds the browser-style behavior of opening a new tab when the mouse
wheel (middle button) is clicked on the empty part of the tab strip, i.e. the
area where the tabs are located, but not on a tab itself.

Only two apps are supported, and the mod ignores clicks everywhere else:

- File Explorer: the new tab is opened by sending Ctrl+T.
- Windows 11 Notepad: the new tab is opened by sending Ctrl+N.

How it works:

- The mod runs as a standalone Windhawk Tool, ensuring it executes in a single 
  instance and doesn't inject multiple hooks into different Explorer processes.
- A dedicated low-level mouse hook thread records the click position and immediately 
  posts it to an isolated UI Automation worker thread. Input is never delayed.
- The worker thread uses UI Automation to find out what was clicked. If the
  point is inside a tab strip (a Tab control / TabView) but not on a tab
  item and not on a strip button, the mod sends the app's new tab shortcut.
- Areas that are visually part of the tab strip row but are not descendants
  of the TabView element in the UIA tree are detected by a row fallback check.
- Middle clicking an actual tab is left untouched, so the native "close tab" 
  behavior keeps working perfectly.

![Middle Click New Tab Demo](https://raw.githubusercontent.com/venasataa/windhawk-mod-assets/refs/heads/main/middle-click-new-tab.gif)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <oleauto.h>
#include <shlwapi.h>
#include <uiautomation.h>
#include <cwchar>

// Thread IDs and handles
DWORD g_workerThreadId = 0;
HANDLE g_workerThread = nullptr;

DWORD g_hookThreadId = 0;
HANDLE g_hookThread = nullptr;

// UI Automation interfaces
IUIAutomation* g_pAutomation = nullptr;
IUIAutomationTreeWalker* g_pControlWalker = nullptr;

////////////////////////////////////////////////////////////
// Target applications and shortcut key dispatch

WORD GetNewTabKeyForWindow(HWND hWnd) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        return 0;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return 0;
    }

    WCHAR szPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(szPath);
    BOOL bOk = QueryFullProcessImageNameW(hProcess, 0, szPath, &dwSize);
    CloseHandle(hProcess);
    if (!bOk) {
        return 0;
    }

    PCWSTR fileName = wcsrchr(szPath, L'\\');
    fileName = fileName ? fileName + 1 : szPath;

    if (_wcsicmp(fileName, L"explorer.exe") == 0) {
        return 'T';
    }
    if (_wcsicmp(fileName, L"Notepad.exe") == 0) {
        return 'N';
    }
    return 0;
}

void SendCtrlKey(WORD vk) {
    INPUT inputs[4] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vk;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = vk;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

////////////////////////////////////////////////////////////
// UI Automation Hit-Testing (Executed exclusively on the worker thread)

enum class HitResult {
    Undecided,     
    OnTab,         
    OnAddButton,   
    OnOtherButton, 
    InContainer,   
};

bool IsTabItemElement(IUIAutomationElement* pElement) {
    CONTROLTYPEID controlType = 0;
    if (SUCCEEDED(pElement->get_CurrentControlType(&controlType)) &&
        controlType == UIA_TabItemControlTypeId) {
        return true;
    }

    BSTR className = nullptr;
    if (SUCCEEDED(pElement->get_CurrentClassName(&className)) && className) {
        bool isTabItem = StrStrIW(className, L"TabViewItem") ||
                         StrStrIW(className, L"TabItem");
        SysFreeString(className);
        if (isTabItem) {
            return true;
        }
    }
    return false;
}

bool IsTabStripContainerElement(IUIAutomationElement* pElement) {
    CONTROLTYPEID controlType = 0;
    if (SUCCEEDED(pElement->get_CurrentControlType(&controlType)) &&
        controlType == UIA_TabControlTypeId) {
        return true;
    }

    BSTR className = nullptr;
    if (SUCCEEDED(pElement->get_CurrentClassName(&className)) && className) {
        bool isContainer = !StrStrIW(className, L"TabViewItem") &&
                           (StrStrIW(className, L"TabView") ||
                            StrStrIW(className, L"TabStrip"));
        SysFreeString(className);
        if (isContainer) {
            return true;
        }
    }
    return false;
}

int GetButtonKind(IUIAutomationElement* pElement) {
    CONTROLTYPEID controlType = 0;
    if (FAILED(pElement->get_CurrentControlType(&controlType)) ||
        controlType != UIA_ButtonControlTypeId) {
        return 0;
    }

    int kind = 2;
    BSTR automationId = nullptr;
    if (SUCCEEDED(pElement->get_CurrentAutomationId(&automationId)) &&
        automationId) {
        if (StrStrIW(automationId, L"add") ||
            StrStrIW(automationId, L"newtab")) {
            kind = 1;
        }
        SysFreeString(automationId);
    }
    return kind;
}

bool FindTabStripRect(HWND hRootWnd, RECT* prcTabStrip) {
    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(g_pAutomation->ElementFromHandle(hRootWnd, &pRoot)) || !pRoot) {
        return false;
    }

    bool found = false;
    IUIAutomationElement* pTabStrip = nullptr;

    VARIANT varControlType;
    VariantInit(&varControlType);
    varControlType.vt = VT_I4;
    varControlType.lVal = UIA_TabControlTypeId;

    IUIAutomationCondition* pCondition = nullptr;
    if (SUCCEEDED(g_pAutomation->CreatePropertyCondition(
            UIA_ControlTypePropertyId, varControlType, &pCondition)) && pCondition) {
        if (SUCCEEDED(pRoot->FindFirst(TreeScope_Descendants, pCondition, &pTabStrip)) && pTabStrip) {
            found = true;
        }
        pCondition->Release();
    }
    VariantClear(&varControlType);

    if (!found) {
        static const WCHAR* kTabViewClassNames[] = {
            L"Windows.UI.Xaml.Controls.TabView",
            L"Microsoft.UI.Xaml.Controls.TabView",
        };

        for (PCWSTR className : kTabViewClassNames) {
            VARIANT varClassName;
            VariantInit(&varClassName);
            varClassName.vt = VT_BSTR;
            varClassName.bstrVal = SysAllocString(className);

            IUIAutomationCondition* pClassCondition = nullptr;
            if (varClassName.bstrVal &&
                SUCCEEDED(g_pAutomation->CreatePropertyCondition(
                    UIA_ClassNamePropertyId, varClassName, &pClassCondition)) && pClassCondition) {
                if (SUCCEEDED(pRoot->FindFirst(TreeScope_Descendants, pClassCondition, &pTabStrip)) && pTabStrip) {
                    found = true;
                }
                pClassCondition->Release();
            }
            VariantClear(&varClassName);

            if (found) break;
        }
    }

    bool result = false;
    if (found && pTabStrip) {
        if (SUCCEEDED(pTabStrip->get_CurrentBoundingRectangle(prcTabStrip))) {
            result = !IsRectEmpty(prcTabStrip);
        }
        pTabStrip->Release();
    }

    pRoot->Release();
    return result;
}

bool IsPointOnTabStripRow(HWND hRootWnd, POINT pt) {
    RECT rcTabStrip;
    if (!FindTabStripRect(hRootWnd, &rcTabStrip)) {
        return false;
    }

    if (pt.y < rcTabStrip.top || pt.y > rcTabStrip.bottom) {
        return false;
    }

    RECT rcWindow;
    if (!GetWindowRect(hRootWnd, &rcWindow)) {
        return false;
    }

    return pt.x >= rcWindow.left && pt.x <= rcWindow.right;
}

bool IsEmptyTabStripAtPoint(POINT pt, HWND hRootWnd) {
    IUIAutomationElement* pElement = nullptr;
    if (FAILED(g_pAutomation->ElementFromPoint(pt, &pElement)) || !pElement) {
        return false;
    }

    HitResult hit = HitResult::Undecided;
    IUIAutomationElement* pCurrent = pElement;
    pCurrent->AddRef();

    for (int i = 0; i < 16 && pCurrent; i++) {
        if (IsTabItemElement(pCurrent)) {
            hit = HitResult::OnTab;
            break;
        }

        if (int buttonKind = GetButtonKind(pCurrent)) {
            hit = (buttonKind == 1) ? HitResult::OnAddButton : HitResult::OnOtherButton;
            break;
        }

        if (IsTabStripContainerElement(pCurrent)) {
            hit = HitResult::InContainer;
            break;
        }

        IUIAutomationElement* pParent = nullptr;
        HRESULT hrParent = g_pControlWalker->GetParentElement(pCurrent, &pParent);
        pCurrent->Release();
        pCurrent = SUCCEEDED(hrParent) ? pParent : nullptr;
    }

    if (pCurrent) {
        pCurrent->Release();
    }
    pElement->Release();

    switch (hit) {
        case HitResult::OnAddButton:
        case HitResult::InContainer:
            return true;

        case HitResult::OnTab:
        case HitResult::OnOtherButton:
            return false;

        case HitResult::Undecided:
            return IsPointOnTabStripRow(hRootWnd, pt);
    }
    return false;
}

void HandleMiddleClick(POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return;

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hRootWnd) hRootWnd = hWnd;

    WORD newTabKey = GetNewTabKeyForWindow(hRootWnd);
    if (!newTabKey) return;
    if (!IsEmptyTabStripAtPoint(pt, hRootWnd)) return;
    
    HWND hForeground = GetForegroundWindow();
    if (hForeground != hRootWnd) {
        if (!SetForegroundWindow(hRootWnd)) {
            DWORD foreThreadId = GetWindowThreadProcessId(hForeground, nullptr);
            DWORD currThreadId = GetCurrentThreadId();
            if (foreThreadId != currThreadId) {
                AttachThreadInput(currThreadId, foreThreadId, TRUE);
                SetForegroundWindow(hRootWnd);
                AttachThreadInput(currThreadId, foreThreadId, FALSE);
            }
        }
    }
    
    SendCtrlKey(newTabKey);
}

////////////////////////////////////////////////////////////
// Thread 1: UI Automation (Worker Thread)

DWORD WINAPI WorkerThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;
    g_workerThreadId = GetCurrentThreadId();

    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitialized = SUCCEEDED(hrCom);
    
    if (comInitialized) {
        if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&g_pAutomation)) && g_pAutomation) {
            if (FAILED(g_pAutomation->get_ControlViewWalker(&g_pControlWalker))) {
                g_pControlWalker = nullptr;
            }
        }
    }

    // CRITICAL: Force creation of the message queue before signaling readiness!
    // PostThreadMessage requires the target thread to have a message queue, which is 
    // normally only created when GetMessage is called. Without PeekMessage, clicks 
    // will be lost because PostThreadMessage fails silently.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Signal the main mod thread that COM and UIA are initialized
    SetEvent(hReadyEvent);

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP + 1) {
            if (g_pAutomation && g_pControlWalker) {
                POINT pt;
                pt.x = (LONG)msg.wParam;
                pt.y = (LONG)msg.lParam;
                HandleMiddleClick(pt);
            }
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_pControlWalker) g_pControlWalker->Release();
    if (g_pAutomation) g_pAutomation->Release();

    if (comInitialized) CoUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////
// Thread 2: Low-level mouse hook (Lightweight implementation)

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MBUTTONDOWN) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        PostThreadMessage(g_workerThreadId, WM_APP + 1, (WPARAM)(LONG_PTR)pMouseStruct->pt.x, (LPARAM)(LONG_PTR)pMouseStruct->pt.y);
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;
    g_hookThreadId = GetCurrentThreadId();

    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    // Signal that the hook was successfully installed
    SetEvent(hReadyEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (mouseHook) {
        UnhookWindowsHookEx(mouseHook);
    }
    return 0;
}

////////////////////////////////////////////////////////////
// Windhawk Tool mod callbacks

BOOL WhTool_ModInit() {
    // Initialize and start the UI Automation thread
    HANDLE hWorkerReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hWorkerReady) {
        g_workerThread = CreateThread(nullptr, 0, WorkerThread, hWorkerReady, 0, nullptr);
        if (g_workerThread) {
            WaitForSingleObject(hWorkerReady, INFINITE);
        }
        CloseHandle(hWorkerReady);
    }

    // Initialize and start the isolated mouse hook thread
    HANDLE hHookReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hHookReady) {
        g_hookThread = CreateThread(nullptr, 0, HookThread, hHookReady, 0, nullptr);
        if (g_hookThread) {
            WaitForSingleObject(hHookReady, INFINITE);
        }
        CloseHandle(hHookReady);
    }
    
    return TRUE;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
    // Terminate the mouse hook thread
    if (g_hookThread) {
        PostThreadMessage(g_hookThreadId, WM_APP, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    // Terminate the UIA worker thread
    if (g_workerThread) {
        PostThreadMessage(g_workerThreadId, WM_APP, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
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
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
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
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +\
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
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
