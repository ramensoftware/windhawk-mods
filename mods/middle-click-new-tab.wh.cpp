// ==WindhawkMod==
// @id              middle-click-new-tab
// @name            Middle Click New Tab
// @description     Middle-click the empty tab strip area of File Explorer or Notepad to open a new tab
// @version         1.0.0
// @author          venasataa
// @github          https://github.com/venasataa
// @homepage        https://venasataa.github.io
// @include         explorer.exe
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

- A low-level mouse hook only records the click position and posts it to a
  worker thread. No hit testing or other blocking work is done inside the
  hook, so input is never delayed.
- The worker thread uses UI Automation to find out what was clicked. If the
  point is inside a tab strip (a Tab control / TabView) but not on a tab
  item and not on a strip button (scroll arrows, caption buttons), the mod
  sends the app's new tab shortcut to the clicked window.
- Areas that are visually part of the tab strip row but are not descendants
  of the TabView element in the UIA tree (for example the title bar drag
  region to the right of the add button) are detected by a fallback check:
  the tab strip container is located in the window, and any point on the
  same horizontal row is accepted.
- Middle clicking an actual tab is left untouched, so the native
  "close tab" behavior keeps working.
- The TabView "add" button is also accepted: middle clicking it opens a new
  tab just like a left click would.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <oleauto.h>
#include <shlwapi.h>
#include <uiautomation.h>

DWORD g_workerThreadId = 0;
HANDLE g_workerThread = nullptr;

IUIAutomation* g_pAutomation = nullptr;
IUIAutomationTreeWalker* g_pControlWalker = nullptr;

////////////////////////////////////////////////////////////
// Target apps and their new tab shortcuts.

// Returns the virtual key of the app's new tab shortcut, or 0 if the window
// does not belong to a supported app.
WORD GetNewTabKeyForWindow(HWND hWnd) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        return 0;
    }

    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
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
// UI Automation hit test, runs on the worker thread only.

enum class HitResult {
    Undecided,     // neither a tab, a button nor the strip container
    OnTab,         // a tab item: leave the native middle-click behavior
    OnAddButton,   // the TabView add ("plus") button
    OnOtherButton, // caption/scroll and other strip buttons
    InContainer,   // inside the tab strip container, but not on a tab/button
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
        // Note: "TabViewItem" must not be treated as the container.
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

// 0 = not a button, 1 = the add ("plus") button, 2 = some other button.
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

// Finds the bounding rectangle of the tab strip container (TabView) in the
// window: first by UIA control type, then by the WinUI class name fallback.
bool FindTabStripRect(HWND hRootWnd, RECT* prcTabStrip) {
    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(g_pAutomation->ElementFromHandle(hRootWnd, &pRoot)) ||
        !pRoot) {
        return false;
    }

    bool found = false;
    IUIAutomationElement* pTabStrip = nullptr;

    // 1) By control type: the TabView container reports UIA_TabControlTypeId.
    VARIANT varControlType;
    VariantInit(&varControlType);
    varControlType.vt = VT_I4;
    varControlType.lVal = UIA_TabControlTypeId;

    IUIAutomationCondition* pCondition = nullptr;
    if (SUCCEEDED(g_pAutomation->CreatePropertyCondition(
            UIA_ControlTypePropertyId, varControlType, &pCondition)) &&
        pCondition) {
        if (SUCCEEDED(pRoot->FindFirst(TreeScope_Descendants, pCondition,
                                       &pTabStrip)) &&
            pTabStrip) {
            found = true;
        }
        pCondition->Release();
    }
    VariantClear(&varControlType);

    // 2) Fallback by class name (WinUI 2 / WinUI 3 TabView), for apps where
    //    the container does not report the Tab control type (e.g. Notepad).
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
                    UIA_ClassNamePropertyId, varClassName,
                    &pClassCondition)) &&
                pClassCondition) {
                if (SUCCEEDED(pRoot->FindFirst(TreeScope_Descendants,
                                               pClassCondition,
                                               &pTabStrip)) &&
                    pTabStrip) {
                    found = true;
                }
                pClassCondition->Release();
            }
            VariantClear(&varClassName);

            if (found) {
                break;
            }
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

// Fallback for areas that are visually part of the tab strip row but are not
// descendants of the TabView element in the UIA tree (e.g. the title bar drag
// region to the right of the add button in File Explorer): accept any point
// on the same horizontal row as the tab strip container.
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

// Returns true if the point is on the empty area of a tab strip (or on the
// TabView add button), and false if it's on a tab, on a strip button, or
// outside the tab strip row.
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
            hit = (buttonKind == 1) ? HitResult::OnAddButton
                                    : HitResult::OnOtherButton;
            break;
        }

        if (IsTabStripContainerElement(pCurrent)) {
            hit = HitResult::InContainer;
            break;
        }

        IUIAutomationElement* pParent = nullptr;
        HRESULT hrParent =
            g_pControlWalker->GetParentElement(pCurrent, &pParent);
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
            // E.g. the title bar drag region next to the tabs.
            return IsPointOnTabStripRow(hRootWnd, pt);
    }

    return false;
}

void HandleMiddleClick(POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return;
    }

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hRootWnd) {
        hRootWnd = hWnd;
    }

    WORD newTabKey = GetNewTabKeyForWindow(hRootWnd);
    if (!newTabKey) {
        return;
    }

    if (!IsEmptyTabStripAtPoint(pt, hRootWnd)) {
        return;
    }

    // The shortcut goes to the foreground window; make sure it's the
    // clicked window.
    if (GetForegroundWindow() != hRootWnd) {
        if (!SetForegroundWindow(hRootWnd)) {
            return;
        }
    }

    SendCtrlKey(newTabKey);
}

////////////////////////////////////////////////////////////
// Worker thread. The low-level hook only posts the click position here,
// so no blocking work is done inside the hook itself.

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MBUTTONDOWN) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
PostThreadMessage(g_workerThreadId, WM_APP + 1, (WPARAM)pMouseStruct->pt.x, (LPARAM)pMouseStruct->pt.y);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;

    g_workerThreadId = GetCurrentThreadId();

    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitialized = SUCCEEDED(hrCom);

    if (comInitialized) {
        if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    __uuidof(IUIAutomation),
                                    (void**)&g_pAutomation)) ||
            !g_pAutomation) {
            g_pAutomation = nullptr;
        } else if (FAILED(
                       g_pAutomation->get_ControlViewWalker(
                           &g_pControlWalker))) {
            g_pControlWalker = nullptr;
        }
    }

    HHOOK mouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    SetEvent(hReadyEvent);

    MSG msg;
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

    if (mouseHook) {
        UnhookWindowsHookEx(mouseHook);
    }

    if (g_pControlWalker) {
        g_pControlWalker->Release();
        g_pControlWalker = nullptr;
    }
    if (g_pAutomation) {
        g_pAutomation->Release();
        g_pAutomation = nullptr;
    }

    if (comInitialized) {
        CoUninitialize();
    }

    return 0;
}

void WorkerThreadInit() {
    if (g_workerThread) {
        return;
    }

    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent) {
        return;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThread, hReadyEvent, 0, nullptr);
    if (g_workerThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }

    CloseHandle(hReadyEvent);
}

void WorkerThreadUninit() {
    if (!g_workerThread) {
        return;
    }

    PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
    WaitForSingleObject(g_workerThread, INFINITE);
    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

////////////////////////////////////////////////////////////
// Windhawk mod entry points.

BOOL Wh_ModInit() {
    return TRUE;
}

void Wh_ModAfterInit() {
    WorkerThreadInit();
}

void Wh_ModUninit() {
    WorkerThreadUninit();
}
