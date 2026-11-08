// ==WindhawkMod==
// @id              middle-click-new-tab
// @name            Middle Click New Tab
// @description     Middle-click the empty tab strip area of File Explorer, Notepad or Terminal to open a new tab
// @version         1.0.0
// @author          venasataa
// @github          https://github.com/venasataa
// @homepage        https://venasataa.github.io
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*

The mod opens a new tab when you release the middle mouse button over the empty space of a tab strip.

Three apps are supported:

- File Explorer: the new tab is opened by invoking the native AddButton (fallback: Ctrl+T).
- Notepad: the new tab is opened by invoking the native AddButton (fallback: Ctrl+N).
- Terminal: the new tab is opened by invoking the native AddButton (fallback: Ctrl+Shift+T). Note: This does not work if Terminal is running as administrator due to privilege isolation.

You can enable or disable each app individually in the mod's settings tab.

How it works:

- The mod runs as a standalone Windhawk Tool, ensuring it executes in a single 
  instance and doesn't inject multiple hooks into different Explorer processes.
- A dedicated low-level mouse hook thread records middle-button press/release events.
  It never performs slow UI Automation work and never blocks global input.
- If the press looks like it happened in a tab strip area, an isolated UI Automation
  worker thread verifies whether the point is on an actual tab, a button, or empty
  tab strip space.
- If the mouse button is released before the check finishes, the mod temporarily
  holds the release event and re-sends it synthetically after the check. This avoids
  the race condition where the native app closes the tab before the mod can inspect it.
- Middle clicking an actual tab or the add button is left untouched, so the native 
  behaviors keep working perfectly.

![Middle Click New Tab Demo](https://raw.githubusercontent.com/venasataa/middle-click-new-tab/refs/heads/main/preview.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableExplorer: true
  $name: File Explorer
  $description: >-
    Enable middle-click new tab for File Explorer.
- enableNotepad: true
  $name: Notepad
  $description: >-
    Enable middle-click new tab for Notepad.
- enableTerminal: true
  $name: Terminal
  $description: >-
    Enable middle-click new tab for Windows Terminal.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <oleauto.h>
#include <shlwapi.h>
#include <uiautomation.h>
#include <cwchar>
#include <atomic>

constexpr UINT WM_QUIT_THREAD = WM_APP;
constexpr UINT WM_WORKER_DOWN = WM_APP + 1;
constexpr UINT WM_WORKER_UP_PENDING = WM_APP + 2;
constexpr UINT WM_WORKER_UP_VALID = WM_APP + 3;

constexpr DWORD RELEASE_TIMEOUT_MS = 500;
constexpr int TOP_BAND_BASE = 160;
constexpr DWORD TAB_STRIP_CACHE_TTL_MS = 2000;

enum ClickState {
    CLICK_NONE = 0,
    CLICK_PENDING = 1,
    CLICK_VALID = 2,
    CLICK_INVALID = 3,
};

// Thread IDs and handles
DWORD g_workerThreadId = 0;
HANDLE g_workerThread = nullptr;

DWORD g_hookThreadId = 0;
HANDLE g_hookThread = nullptr;

// UI Automation interfaces
IUIAutomation* g_pAutomation = nullptr;
IUIAutomationTreeWalker* g_pControlWalker = nullptr;

// Settings
std::atomic<bool> g_enableExplorer = true;
std::atomic<bool> g_enableNotepad = true;
std::atomic<bool> g_enableTerminal = true;

// State synchronization
CRITICAL_SECTION g_stateCs;
bool g_csInitialized = false;

// Tab strip cache
struct TabStripCacheEntry {
    HWND hwnd;
    bool hasTabStrip;
    RECT row;
    LONG stripBottom;
    DWORD timestamp;
};

TabStripCacheEntry g_tabStripCache[32];
int g_tabStripCacheCount = 0;
CRITICAL_SECTION g_tabStripCacheCs;
bool g_tabStripCacheCsInitialized = false;

// Click state
int g_clickState = CLICK_NONE;
LONG g_generation = 0;
bool g_upSwallowed = false;
DWORD g_upSwallowedTime = 0;
POINT g_downPt = {};
POINT g_upPt = {};
HWND g_downRoot = nullptr;

struct ShortcutInfo {
    WORD vk;
    bool shift;
};

ShortcutInfo g_downShortcut = {0, false};

UINT_PTR g_watchdogTimerId = 0;

void LoadSettings() {
    g_enableExplorer = Wh_GetIntSetting(L"enableExplorer") != 0;
    g_enableNotepad = Wh_GetIntSetting(L"enableNotepad") != 0;
    g_enableTerminal = Wh_GetIntSetting(L"enableTerminal") != 0;
    Wh_Log(L"Settings updated: Explorer=%d, Notepad=%d, Terminal=%d", 
           g_enableExplorer.load(), g_enableNotepad.load(), g_enableTerminal.load());
}

////////////////////////////////////////////////////////////
// Helpers

int ScaleByDpi(int value, HWND hWnd) {
    UINT dpi = GetDpiForWindow(hWnd);
    if (dpi <= 0) dpi = 96;
    return MulDiv(value, dpi, 96);
}

void EnterState() {
    EnterCriticalSection(&g_stateCs);
}

void LeaveState() {
    LeaveCriticalSection(&g_stateCs);
}

void EnterTabStripCache() {
    EnterCriticalSection(&g_tabStripCacheCs);
}

void LeaveTabStripCache() {
    LeaveCriticalSection(&g_tabStripCacheCs);
}

bool GetCachedTabStrip(HWND hwnd, RECT* prcTabStrip, LONG* pStripBottom) {
    if (!hwnd || !prcTabStrip || !pStripBottom) return false;
    
    EnterTabStripCache();
    
    DWORD now = GetTickCount();
    for (int i = 0; i < g_tabStripCacheCount; i++) {
        if (g_tabStripCache[i].hwnd == hwnd) {
            if (now - g_tabStripCache[i].timestamp < TAB_STRIP_CACHE_TTL_MS) {
                bool hasTabStrip = g_tabStripCache[i].hasTabStrip;
                if (hasTabStrip) {
                    *prcTabStrip = g_tabStripCache[i].row;
                    *pStripBottom = g_tabStripCache[i].stripBottom;
                }
                LeaveTabStripCache();
                return hasTabStrip;
            } else {
                // Expired, remove from cache
                g_tabStripCache[i] = g_tabStripCache[--g_tabStripCacheCount];
                break;
            }
        }
    }
    
    LeaveTabStripCache();
    return false;
}

void SetCachedTabStrip(HWND hwnd, bool hasTabStrip, const RECT* prcTabStrip, LONG stripBottom) {
    if (!hwnd) return;
    
    EnterTabStripCache();
    
    int index = -1;
    for (int i = 0; i < g_tabStripCacheCount; i++) {
        if (g_tabStripCache[i].hwnd == hwnd) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        if (g_tabStripCacheCount < ARRAYSIZE(g_tabStripCache)) {
            index = g_tabStripCacheCount++;
        } else {
            // Evict oldest entry
            DWORD oldestTime = g_tabStripCache[0].timestamp;
            index = 0;
            for (int i = 1; i < g_tabStripCacheCount; i++) {
                if (g_tabStripCache[i].timestamp < oldestTime) {
                    oldestTime = g_tabStripCache[i].timestamp;
                    index = i;
                }
            }
        }
        g_tabStripCache[index].hwnd = hwnd;
    }
    
    g_tabStripCache[index].hasTabStrip = hasTabStrip;
    if (hasTabStrip && prcTabStrip) {
        g_tabStripCache[index].row = *prcTabStrip;
        g_tabStripCache[index].stripBottom = stripBottom;
    }
    g_tabStripCache[index].timestamp = GetTickCount();
    
    LeaveTabStripCache();
}

////////////////////////////////////////////////////////////
// Target applications and shortcut key dispatch

ShortcutInfo GetShortcutForWindow(HWND hWnd) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        return {0, false};
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return {0, false};
    }

    WCHAR szPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(szPath);
    BOOL bOk = QueryFullProcessImageNameW(hProcess, 0, szPath, &dwSize);
    CloseHandle(hProcess);
    if (!bOk) {
        return {0, false};
    }

    PCWSTR fileName = wcsrchr(szPath, L'\\');
    fileName = fileName ? fileName + 1 : szPath;

    if (_wcsicmp(fileName, L"explorer.exe") == 0) {
        // Ensure it's a standard Explorer window, not the Taskbar or Desktop.
        WCHAR className[256];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"CabinetWClass") != 0) {
            return {0, false};
        }
        if (g_enableExplorer) return {'T', false};
    }

    if (_wcsicmp(fileName, L"Notepad.exe") == 0) {
        if (g_enableNotepad) return {'N', false};
    }

    if (_wcsicmp(fileName, L"WindowsTerminal.exe") == 0) {
        if (g_enableTerminal) return {'T', true};
    }
    
    return {0, false};
}

void SendCtrlKey(WORD vk, bool shift) {
    INPUT inputs[6] = {};
    int count = 0;

    inputs[count].type = INPUT_KEYBOARD;
    inputs[count].ki.wVk = VK_CONTROL;
    count++;

    if (shift) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = VK_SHIFT;
        count++;
    }

    inputs[count].type = INPUT_KEYBOARD;
    inputs[count].ki.wVk = vk;
    count++;

    inputs[count].type = INPUT_KEYBOARD;
    inputs[count].ki.wVk = vk;
    inputs[count].ki.dwFlags = KEYEVENTF_KEYUP;
    count++;

    if (shift) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = VK_SHIFT;
        inputs[count].ki.dwFlags = KEYEVENTF_KEYUP;
        count++;
    }

    inputs[count].type = INPUT_KEYBOARD;
    inputs[count].ki.wVk = VK_CONTROL;
    inputs[count].ki.dwFlags = KEYEVENTF_KEYUP;
    count++;

    SendInput(count, inputs, sizeof(INPUT));
}

void SendMiddleUp() {
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
    SendInput(1, &input, sizeof(INPUT));
}

void FocusAndSendShortcut(HWND hRootWnd, ShortcutInfo shortcut) {
    if (!shortcut.vk || !IsWindow(hRootWnd)) return;

    HWND hForeground = GetForegroundWindow();
    if (hForeground != hRootWnd) {
        // Allows to steal focus.
        INPUT input{};
        SendInput(1, &input, sizeof(INPUT));
        SetForegroundWindow(hRootWnd);
        if (GetForegroundWindow() != hRootWnd) {
            return;
        }
    }

    SendCtrlKey(shortcut.vk, shortcut.shift);
}

bool IsPotentialTabStripPoint(HWND hRootWnd, POINT pt) {
    if (!IsWindow(hRootWnd)) return false;

    RECT rcWindow;
    if (!GetWindowRect(hRootWnd, &rcWindow)) {
        return false;
    }

    if (pt.x < rcWindow.left || pt.x > rcWindow.right ||
        pt.y < rcWindow.top || pt.y > rcWindow.bottom) {
        return false;
    }

    int band = ScaleByDpi(TOP_BAND_BASE, hRootWnd);
    return pt.y <= rcWindow.top + band;
}

////////////////////////////////////////////////////////////
// UI Automation hit-testing (executed exclusively on the worker thread)

enum class HitResult {
    Undecided,      // No specific element identified
    OnTab,          // Clicked on a tab item
    OnOtherButton,  // Clicked on a button other than the add button
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

bool IsAddButton(IUIAutomationElement* pElement) {
    CONTROLTYPEID controlType = 0;
    if (FAILED(pElement->get_CurrentControlType(&controlType)) ||
        (controlType != UIA_ButtonControlTypeId && controlType != UIA_SplitButtonControlTypeId)) {
        return false;
    }

    BSTR automationId = nullptr;
    bool isAdd = false;
    if (SUCCEEDED(pElement->get_CurrentAutomationId(&automationId)) && automationId) {
        if (_wcsicmp(automationId, L"AddButton") == 0 || 
            _wcsicmp(automationId, L"NewTabButton") == 0 ||
            StrStrIW(automationId, L"AddButton") || 
            StrStrIW(automationId, L"NewTab")) {
            isAdd = true;
        }
        SysFreeString(automationId);
    }
    return isAdd;
}

bool IsButton(IUIAutomationElement* pElement) {
    CONTROLTYPEID controlType = 0;
    if (FAILED(pElement->get_CurrentControlType(&controlType)) ||
        (controlType != UIA_ButtonControlTypeId && controlType != UIA_SplitButtonControlTypeId)) {
        return false;
    }
    return true;
}

bool FindTabStripBottom(IUIAutomationElement* pTabView, LONG* pBottom) {
    if (!g_pAutomation || !g_pControlWalker || !pTabView || !pBottom) return false;

    VARIANT varControlType;
    VariantInit(&varControlType);
    varControlType.vt = VT_I4;
    varControlType.lVal = UIA_TabItemControlTypeId;

    IUIAutomationCondition* pCondition = nullptr;
    if (FAILED(g_pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varControlType, &pCondition)) || !pCondition) {
        VariantClear(&varControlType);
        return false;
    }

    IUIAutomationElementArray* pElements = nullptr;
    if (FAILED(pTabView->FindAll(TreeScope_Descendants, pCondition, &pElements)) || !pElements) {
        pCondition->Release();
        VariantClear(&varControlType);
        return false;
    }
    pCondition->Release();
    VariantClear(&varControlType);

    int count = 0;
    pElements->get_Length(&count);
    
    if (count > 0) {
        IUIAutomationElement* pFirstItem = nullptr;
        if (SUCCEEDED(pElements->GetElement(0, &pFirstItem)) && pFirstItem) {
            IUIAutomationElement* pCurrent = nullptr;
            if (SUCCEEDED(g_pControlWalker->GetParentElement(pFirstItem, &pCurrent)) && pCurrent) {
                RECT rcTabView;
                if (SUCCEEDED(pTabView->get_CurrentBoundingRectangle(&rcTabView))) {
                    int tabViewHeight = rcTabView.bottom - rcTabView.top;
                    
                    for (int depth = 0; depth < 5 && pCurrent; depth++) {
                        RECT rc;
                        if (SUCCEEDED(pCurrent->get_CurrentBoundingRectangle(&rc)) && !IsRectEmpty(&rc)) {
                            int height = rc.bottom - rc.top;
                            if (height < tabViewHeight - 10 && height >= 10) {
                                *pBottom = rc.bottom;
                                pCurrent->Release();
                                pFirstItem->Release();
                                pElements->Release();
                                return true;
                            }
                        }
                        IUIAutomationElement* pParent = nullptr;
                        HRESULT hr = g_pControlWalker->GetParentElement(pCurrent, &pParent);
                        pCurrent->Release();
                        pCurrent = SUCCEEDED(hr) ? pParent : nullptr;
                    }
                }
                if (pCurrent) pCurrent->Release();
            }
            pFirstItem->Release();
        }
    }

    LONG maxBottom = 0;
    for (int i = 0; i < count; i++) {
        IUIAutomationElement* pItem = nullptr;
        if (SUCCEEDED(pElements->GetElement(i, &pItem)) && pItem) {
            RECT rc;
            if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&rc))) {
                if (rc.bottom > maxBottom) {
                    maxBottom = rc.bottom;
                }
            }
            pItem->Release();
        }
    }
    pElements->Release();

    if (maxBottom > 0) {
        *pBottom = maxBottom + 20;
        return true;
    }
    return false;
}

IUIAutomationElement* FindTabView(HWND hRootWnd) {
    if (!g_pAutomation) return nullptr;
    
    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(g_pAutomation->ElementFromHandle(hRootWnd, &pRoot)) || !pRoot) {
        return nullptr;
    }

    IUIAutomationElement* pTabView = nullptr;
    bool found = false;

    VARIANT varControlType;
    VariantInit(&varControlType);
    varControlType.vt = VT_I4;
    varControlType.lVal = UIA_TabControlTypeId;

    IUIAutomationCondition* pCondition = nullptr;
    if (SUCCEEDED(g_pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varControlType, &pCondition)) && pCondition) {
        if (SUCCEEDED(pRoot->FindFirst(TreeScope_Descendants, pCondition, &pTabView)) && pTabView) {
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
            if (varClassName.bstrVal && SUCCEEDED(g_pAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId, varClassName, &pClassCondition)) && pClassCondition) {
                if (SUCCEEDED(pRoot->FindFirst(TreeScope_Descendants, pClassCondition, &pTabView)) && pTabView) {
                    found = true;
                }
                pClassCondition->Release();
            }
            VariantClear(&varClassName);
            if (found) break;
        }
    }

    pRoot->Release();
    return pTabView;
}

bool FindTabStripRect(HWND hRootWnd, RECT* prcTabStrip, LONG* pStripBottom) {
    // Check cache first
    if (GetCachedTabStrip(hRootWnd, prcTabStrip, pStripBottom)) {
        return true;
    }
    
    IUIAutomationElement* pTabView = FindTabView(hRootWnd);
    if (!pTabView) {
        SetCachedTabStrip(hRootWnd, false, nullptr, 0);
        return false;
    }

    bool result = false;
    if (SUCCEEDED(pTabView->get_CurrentBoundingRectangle(prcTabStrip))) {
        if (pStripBottom) {
            // Fallback to a reasonable tab strip height if FindTabStripBottom fails
            // to prevent the entire content area from being considered a tab strip
            *pStripBottom = prcTabStrip->top + ScaleByDpi(40, hRootWnd);
            FindTabStripBottom(pTabView, pStripBottom);
        }
        result = !IsRectEmpty(prcTabStrip);
    }
    pTabView->Release();
    
    // Cache the result
    SetCachedTabStrip(hRootWnd, result, result ? prcTabStrip : nullptr, result ? *pStripBottom : 0);
    
    return result;
}

bool IsPointOnTabStripRow(HWND hRootWnd, POINT pt) {
    RECT rcTabStrip;
    LONG stripBottom;
    if (!FindTabStripRect(hRootWnd, &rcTabStrip, &stripBottom)) {
        return false;
    }

    if (pt.y < rcTabStrip.top || pt.y > stripBottom) {
        return false;
    }

    RECT rcWindow;
    if (!GetWindowRect(hRootWnd, &rcWindow)) {
        return false;
    }

    return pt.x >= rcWindow.left && pt.x <= rcWindow.right;
}

bool IsEmptyTabStripAtPoint(POINT pt, HWND hRootWnd) {
    if (!IsPointOnTabStripRow(hRootWnd, pt)) {
        return false;
    }

    IUIAutomationElement* pElement = nullptr;
    if (FAILED(g_pAutomation->ElementFromPoint(pt, &pElement)) || !pElement) {
        return false;
    }

    HitResult hit = HitResult::Undecided;
    IUIAutomationElement* pCurrent = pElement;
    pCurrent->AddRef();

    bool foundAddButton = false;

    for (int i = 0; i < 16 && pCurrent; i++) {
        if (IsTabItemElement(pCurrent)) {
            hit = HitResult::OnTab;
            break;
        }

        if (IsAddButton(pCurrent)) {
            foundAddButton = true;
        } else if (IsButton(pCurrent)) {
            hit = HitResult::OnOtherButton;
            break;
        }

        IUIAutomationElement* pParent = nullptr;
        HRESULT hrParent = g_pControlWalker->GetParentElement(pCurrent, &pParent);
        pCurrent->Release();
        pCurrent = SUCCEEDED(hrParent) ? pParent : nullptr;
    }

    // If we clicked exactly on the AddButton, let the native click handle it
    if (hit == HitResult::Undecided && foundAddButton) {
        hit = HitResult::OnOtherButton;
    }

    if (pCurrent) {
        pCurrent->Release();
    }
    pElement->Release();

    switch (hit) {
        case HitResult::Undecided:
            return true; // Empty space in tab strip!

        case HitResult::OnTab:
        case HitResult::OnOtherButton:
            return false;
    }
    return false;
}

IUIAutomationElement* FindAddButtonInWindow(HWND hRootWnd) {
    IUIAutomationElement* pTabView = FindTabView(hRootWnd);
    if (!pTabView) return nullptr;

    IUIAutomationElement* pAddButton = nullptr;
    
    VARIANT varControlTypes[2];
    VariantInit(&varControlTypes[0]);
    varControlTypes[0].vt = VT_I4;
    varControlTypes[0].lVal = UIA_ButtonControlTypeId;
    VariantInit(&varControlTypes[1]);
    varControlTypes[1].vt = VT_I4;
    varControlTypes[1].lVal = UIA_SplitButtonControlTypeId;

    IUIAutomationCondition* pBtnConditions[2] = {nullptr, nullptr};
    IUIAutomationCondition* pOrCondition = nullptr;

    g_pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varControlTypes[0], &pBtnConditions[0]);
    g_pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varControlTypes[1], &pBtnConditions[1]);
    
    if (pBtnConditions[0] && pBtnConditions[1]) {
        g_pAutomation->CreateOrCondition(pBtnConditions[0], pBtnConditions[1], &pOrCondition);
    }

    if (pOrCondition) {
        IUIAutomationElementArray* pButtons = nullptr;
        if (SUCCEEDED(pTabView->FindAll(TreeScope_Descendants, pOrCondition, &pButtons)) && pButtons) {
            int count = 0;
            pButtons->get_Length(&count);
            for (int i = 0; i < count; i++) {
                IUIAutomationElement* pBtn = nullptr;
                if (SUCCEEDED(pButtons->GetElement(i, &pBtn)) && pBtn) {
                    if (IsAddButton(pBtn)) {
                        pAddButton = pBtn;
                        break;
                    }
                    pBtn->Release();
                }
            }
            pButtons->Release();
        }
        pOrCondition->Release();
    }
    
    if (pBtnConditions[0]) pBtnConditions[0]->Release();
    if (pBtnConditions[1]) pBtnConditions[1]->Release();
    VariantClear(&varControlTypes[0]);
    VariantClear(&varControlTypes[1]);
    pTabView->Release();

    return pAddButton;
}

////////////////////////////////////////////////////////////
// Worker thread: UI Automation checks

void PerformDownCheckIfNeeded(LONG gen) {
    bool needsDownCheck = false;
    POINT downPt = {};
    HWND downRoot = nullptr;

    EnterState();
    if (g_generation == gen && g_clickState == CLICK_PENDING) {
        needsDownCheck = true;
        downPt = g_downPt;
        downRoot = g_downRoot;
    }
    LeaveState();

    if (needsDownCheck && g_pAutomation && g_pControlWalker) {
        bool valid = IsEmptyTabStripAtPoint(downPt, downRoot);

        EnterState();
        if (g_generation == gen && g_clickState == CLICK_PENDING) {
            g_clickState = valid ? CLICK_VALID : CLICK_INVALID;
        }
        LeaveState();
    }
}

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

    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    SetEvent(hReadyEvent);

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_QUIT_THREAD) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.hwnd == nullptr && msg.message == WM_WORKER_DOWN) {
            LONG gen = (LONG)msg.wParam;
            PerformDownCheckIfNeeded(gen);
            continue;
        }

        if (msg.hwnd == nullptr &&
            (msg.message == WM_WORKER_UP_PENDING || msg.message == WM_WORKER_UP_VALID)) {
            LONG gen = (LONG)msg.wParam;

            PerformDownCheckIfNeeded(gen);

            bool validGen = false;
            bool shouldCreate = false;
            HWND root = nullptr;
            ShortcutInfo shortcut = {0, false};
            POINT upPt = {};

            EnterState();
            if (g_generation == gen) {
                validGen = true;
                shouldCreate = (g_clickState == CLICK_VALID);
                root = g_downRoot;
                shortcut = g_downShortcut;
                upPt = g_upPt;

                if (g_upSwallowed) {
                    g_upSwallowed = false;
                }
            }
            LeaveState();

            if (validGen) {
                SendMiddleUp();

                if (shouldCreate) {
                    // Verify release is still in the tab strip
                    if (!IsPointOnTabStripRow(root, upPt)) {
                        shouldCreate = false;
                    }
                }

                if (shouldCreate) {
                    bool invoked = false;
                    IUIAutomationElement* pAddButton = FindAddButtonInWindow(root);
                    if (pAddButton) {
                        IUIAutomationInvokePattern* pInvoke = nullptr;
                        if (SUCCEEDED(pAddButton->GetCurrentPattern(UIA_InvokePatternId, (IUnknown**)&pInvoke)) && pInvoke) {
                            if (SUCCEEDED(pInvoke->Invoke())) {
                                invoked = true;
                            }
                            pInvoke->Release();
                        }
                        pAddButton->Release();
                    }
                    
                    if (!invoked) {
                        FocusAndSendShortcut(root, shortcut);
                    }
                }

                EnterState();
                if (g_generation == gen) {
                    g_clickState = CLICK_NONE;
                }
                LeaveState();
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
// Hook thread: low-level mouse hook

bool ReleasePreviousSwallowedUpIfNeeded() {
    bool release = false;

    EnterState();
    if (g_upSwallowed) {
        release = true;
        g_upSwallowed = false;
        g_clickState = CLICK_NONE;
        g_generation++;
    }
    LeaveState();

    if (release) {
        SendMiddleUp();
        return true;
    }
    return false;
}

void WatchdogCheck() {
    bool release = false;
    LONG gen = 0;

    EnterState();
    if (g_upSwallowed && GetTickCount() - g_upSwallowedTime >= RELEASE_TIMEOUT_MS) {
        release = true;
        gen = g_generation;
        g_upSwallowed = false;
    }
    LeaveState();

    if (release) {
        SendMiddleUp();

        EnterState();
        if (g_generation == gen) {
            g_clickState = CLICK_NONE;
            g_generation++;
        }
        LeaveState();
    }
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

        if (pMouseStruct->flags & LLMHF_INJECTED) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

        if (wParam == WM_MBUTTONDOWN) {
            bool releasedOld = ReleasePreviousSwallowedUpIfNeeded();

            POINT pt = pMouseStruct->pt;
            HWND hWnd = WindowFromPoint(pt);
            bool shouldTrack = false;
            HWND hRootWnd = nullptr;
            ShortcutInfo shortcut = {0, false};

            if (hWnd) {
                hRootWnd = GetAncestor(hWnd, GA_ROOT);
                if (!hRootWnd) hRootWnd = hWnd;

                shortcut = GetShortcutForWindow(hRootWnd);
                if (shortcut.vk && IsPotentialTabStripPoint(hRootWnd, pt)) {
                    RECT rcTabStrip;
                    LONG stripBottom;
                    bool inStrip = true;
                    if (GetCachedTabStrip(hRootWnd, &rcTabStrip, &stripBottom)) {
                        if (pt.y < rcTabStrip.top || pt.y > stripBottom) {
                            inStrip = false;
                        }
                    }
                    
                    if (inStrip) {
                        shouldTrack = true;
                    }
                }
            }

            if (shouldTrack) {
                EnterState();
                g_generation++;
                g_clickState = CLICK_PENDING;
                g_downPt = pt;
                g_upPt = pt;
                g_downRoot = hRootWnd;
                g_downShortcut = shortcut;
                g_upSwallowed = false;
                LONG gen = g_generation;
                LeaveState();

                PostThreadMessage(g_workerThreadId, WM_WORKER_DOWN, (WPARAM)gen, 0);
            } else {
                EnterState();
                if (g_clickState != CLICK_NONE) {
                    g_clickState = CLICK_NONE;
                    g_generation++;
                }
                LeaveState();
            }

            if (releasedOld) {
                // Re-inject the down event so it arrives after the injected up event
                INPUT input{};
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
                SendInput(1, &input, sizeof(INPUT));
                return 1;
            }
        } else if (wParam == WM_MBUTTONUP) {
            POINT pt = pMouseStruct->pt;

            bool swallow = false;
            UINT message = 0;
            LONG gen = 0;

            EnterState();
            if (g_clickState != CLICK_NONE) {
                if (g_clickState == CLICK_INVALID) {
                    g_clickState = CLICK_NONE;
                    g_upSwallowed = false;
                    g_generation++;
                } else {
                    g_upPt = pt;
                    g_upSwallowed = true;
                    g_upSwallowedTime = GetTickCount();

                    if (g_clickState == CLICK_VALID) {
                        message = WM_WORKER_UP_VALID;
                    } else {
                        message = WM_WORKER_UP_PENDING;
                    }

                    gen = g_generation;
                    swallow = true;
                }
            }
            LeaveState();

            if (swallow) {
                if (g_watchdogTimerId != 0) {
                    KillTimer(nullptr, g_watchdogTimerId);
                }
                g_watchdogTimerId = SetTimer(nullptr, 0, RELEASE_TIMEOUT_MS, nullptr);
                PostThreadMessage(g_workerThreadId, message, (WPARAM)gen, 0);
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;
    g_hookThreadId = GetCurrentThreadId();

    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    if (!mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %d", GetLastError());
    }

    SetEvent(hReadyEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_QUIT_THREAD) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.message == WM_TIMER) {
            KillTimer(nullptr, msg.wParam);
            if (g_watchdogTimerId == msg.wParam) {
                g_watchdogTimerId = 0;
            }
            WatchdogCheck();
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
    LoadSettings();

    InitializeCriticalSection(&g_stateCs);
    g_csInitialized = true;

    InitializeCriticalSection(&g_tabStripCacheCs);
    g_tabStripCacheCsInitialized = true;

    HANDLE hWorkerReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hWorkerReady) {
        Wh_Log(L"CreateEvent for worker failed");
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThread, hWorkerReady, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread for worker failed");
        CloseHandle(hWorkerReady);
        return FALSE;
    }

    WaitForSingleObject(hWorkerReady, INFINITE);
    CloseHandle(hWorkerReady);

    if (!g_pAutomation || !g_pControlWalker) {
        Wh_Log(L"UIA failed to initialize, skipping hook");
        return TRUE;
    }

    HANDLE hHookReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hHookReady) {
        Wh_Log(L"CreateEvent for hook failed");
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThread, hHookReady, 0, nullptr);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread for hook failed");
        CloseHandle(hHookReady);
        return FALSE;
    }

    WaitForSingleObject(hHookReady, INFINITE);
    CloseHandle(hHookReady);
    
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_hookThread) {
        PostThreadMessage(g_hookThreadId, WM_QUIT_THREAD, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    if (g_workerThread) {
        PostThreadMessage(g_workerThreadId, WM_QUIT_THREAD, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_csInitialized) {
        DeleteCriticalSection(&g_stateCs);
        g_csInitialized = false;
    }

    if (g_tabStripCacheCsInitialized) {
        DeleteCriticalSection(&g_tabStripCacheCs);
        g_tabStripCacheCsInitialized = false;
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
