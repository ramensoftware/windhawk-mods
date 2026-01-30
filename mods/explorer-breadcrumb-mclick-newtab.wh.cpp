// ==WindhawkMod==
// @id              explorer-breadcrumb-mclick-newtab
// @name            Explorer Breadcrumb Middle-Click New Tab
// @description     Opens folders in a new tab by middle-clicking breadcrumb items (Linux style).
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         explorer.exe
// @compilerOptions -luiautomationcore -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## 🐧 Motivation: The Linux Feel
For users coming from Linux environments (KDE Dolphin, GNOME Files, Thunar), 
middle-clicking a parent folder in the address bar to open it in a new tab 
is pure muscle memory. Windows 11 introduced Tabs, but missed this shortcut.
    
This mod bridges that gap. It allows you to middle-click any folder name 
in the breadcrumb bar to instantly open that location in a new background tab.

## ⚙️ How it Works (Technical Deep Dive)
Windows 11 Explorer (specifically versions using XAML Islands) presents unique 
challenges for automation. Standard window handles (HWND) do not correspond 
directly to breadcrumb buttons.

This mod uses a sophisticated "Universal UI Scraping" approach:
1.  **Threaded Hook:** Installs a Low-Level Mouse Hook (WH_MOUSE_LL) on a 
    dedicated native thread to detect middle-clicks without freezing the UI.
2.  **UI Automation (UIA):** Identifies the clicked element (e.g., "Program Files")
    using Microsoft's UI Automation framework.
3.  **Heuristic Path Discovery:** Instead of relying on specific Automation IDs 
    (which Microsoft changes often), it scans the active window for any element 
    containing a text string that looks like a file path (e.g., "C:\Users\...").
4.  **Focus & Navigation:** - Extracts the full path.
    - Truncates it to the clicked folder.
    - Simulates: Ctrl+T (New Tab) -> Alt+D (Focus Address Bar) -> Paste -> Enter.
    - The "Alt+D" step is crucial to ensure focus isn't lost to the file view.

## ✅ Compatibility
- Works on Windows 11 (22H2, 23H2, 24H2, and Insider builds).
- Agnostic to specific Automation IDs (future-proof).

## ⚠️ Limitations
- **Virtual Folders:** May not work on special locations like "Control Panel" 
  or "This PC" root if no valid filesystem path is displayed in the UI.
- **Input Interference:** Uses `SendInput`. Avoid touching the mouse/keyboard 
  during the split-second navigation sequence (approx. 500ms).
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <windhawk_utils.h>
#include <UIAutomation.h>
#include <comdef.h>
#include <string>
#include <vector>
#include <cwctype> 

// Global variables
HHOOK g_hMouseHook = NULL;
DWORD g_dwCurrentPID = 0;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

std::wstring GetTextFromElement(IUIAutomationElement* pElement) {
    if (!pElement) return L"";
    BSTR val = NULL;
    IUIAutomationValuePattern* pValuePattern = NULL;
    if (SUCCEEDED(pElement->GetCurrentPattern(UIA_ValuePatternId, (IUnknown**)&pValuePattern)) && pValuePattern) {
        pValuePattern->get_CurrentValue(&val);
        pValuePattern->Release();
    }
    if (!val) pElement->get_CurrentName(&val);
    std::wstring result = L"";
    if (val) {
        result = std::wstring(val);
        SysFreeString(val);
    }
    return result;
}

bool IsLooksLikePath(const std::wstring& text) {
    if (text.length() < 3) return false;
    if (text.find(L":\\") != std::wstring::npos) return true; 
    if (text.find(L"\\\\") == 0) return true; 
    return false;
}

std::wstring ParseDriveLetter(const std::wstring& text) {
    size_t len = text.length();
    if (len < 5) return L"";
    if (text[len - 1] == L')' && text[len - 2] == L':') {
        wchar_t driveLetter = text[len - 3];
        if (text[len - 4] == L'(' && iswalpha(driveLetter)) {
            std::wstring drivePath = L"";
            drivePath += driveLetter;
            drivePath += L":\\";
            return drivePath;
        }
    }
    return L"";
}

bool IsChildOfTab(IUIAutomation* pAutomation, IUIAutomationElement* pElement) {
    IUIAutomationTreeWalker* pWalker = NULL;
    pAutomation->get_ControlViewWalker(&pWalker);
    if (!pWalker) return false;

    IUIAutomationElement* pParent = pElement;
    for (int i = 0; i < 4; i++) {
        IUIAutomationElement* pTemp = NULL;
        pWalker->GetParentElement(pParent, &pTemp);
        
        if (pTemp) {
            CONTROLTYPEID typeId = 0;
            pTemp->get_CurrentControlType(&typeId);
            if (typeId == 50019) { // TabItem
                pTemp->Release();
                pWalker->Release();
                if (pParent != pElement) pParent->Release();
                return true; 
            }
            if (pParent != pElement) pParent->Release();
            pParent = pTemp;
        } else {
            break;
        }
    }
    if (pParent && pParent != pElement) pParent->Release();
    pWalker->Release();
    return false;
}

std::wstring FindPathInWindow(IUIAutomation* pAutomation, IUIAutomationElement* pRootElement) {
    if (!pAutomation || !pRootElement) return L"";
    IUIAutomationCondition* pCondition = NULL;
    pAutomation->CreateTrueCondition(&pCondition);
    IUIAutomationElementArray* pFound = NULL;
    
    if (SUCCEEDED(pRootElement->FindAll(TreeScope_Descendants, pCondition, &pFound)) && pFound) {
        int count = 0;
        pFound->get_Length(&count);
        for (int i = 0; i < count; i++) {
            IUIAutomationElement* pItem = NULL;
            if (SUCCEEDED(pFound->GetElement(i, &pItem))) {
                std::wstring text = GetTextFromElement(pItem);
                if (IsLooksLikePath(text)) {
                    pItem->Release();
                    pFound->Release();
                    pCondition->Release();
                    return text;
                }
                pItem->Release();
            }
        }
        pFound->Release();
    }
    pCondition->Release();
    return L"";
}

// ============================================================================
// NAVIGATION (Silent Clipboard)
// ============================================================================

void NavigateNewTab(const std::wstring& targetPath) {
    if (targetPath.length() < 2) return;

    // Register the format that tells Windows 10/11: "Don't show this in Win+V History"
    static UINT cfExclude = RegisterClipboardFormat(L"ExcludeClipboardContentFromMonitorProcessing");
    static UINT cfCanInclude = RegisterClipboardFormat(L"CanIncludeInClipboardHistory");

    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        
        // 1. Set the actual Text Path
        size_t size = (targetPath.length() + 1) * sizeof(WCHAR);
        HGLOBAL hGlobalPath = GlobalAlloc(GMEM_MOVEABLE, size);
        if (hGlobalPath) {
            void* pData = GlobalLock(hGlobalPath);
            memcpy(pData, targetPath.c_str(), size);
            GlobalUnlock(hGlobalPath);
            SetClipboardData(CF_UNICODETEXT, hGlobalPath);
        }

        // 2. Set the "Exclude" flag (Prevents Clipboard History Bloat)
        HGLOBAL hGlobalExclude = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hGlobalExclude) {
            void* pData = GlobalLock(hGlobalExclude);
            *(DWORD*)pData = 0; // Value doesn't matter, existence matters
            GlobalUnlock(hGlobalExclude);
            SetClipboardData(cfExclude, hGlobalExclude);
        }
        
        // 3. Explicitly say "Do not include" (Double safety)
        HGLOBAL hGlobalCanInclude = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hGlobalCanInclude) {
            void* pData = GlobalLock(hGlobalCanInclude);
            *(DWORD*)pData = 0; // 0 = False
            GlobalUnlock(hGlobalCanInclude);
            SetClipboardData(cfCanInclude, hGlobalCanInclude);
        }

        CloseClipboard();
    }

    INPUT inputs[4] = {};
    int idx = 0;

    // Ctrl + T
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(500); 

    // Alt + D
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++; 
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));

    Sleep(100);

    // Ctrl + V
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'V'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'V'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(50);

    // Enter
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
}

// ============================================================================
// ANALYSIS & LOGIC
// ============================================================================

void AnalyzeElement(POINT pt) {
    IUIAutomation* pAutomation = NULL;
    IUIAutomationElement* pElement = NULL;

    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pAutomation);
    
    if (SUCCEEDED(hr) && pAutomation) {
        hr = pAutomation->ElementFromPoint(pt, &pElement);
        if (SUCCEEDED(hr) && pElement) {
            
            // Check Exclusion (Tab Closing)
            if (IsChildOfTab(pAutomation, pElement)) {
                pElement->Release();
                pAutomation->Release();
                return;
            }

            BSTR nameBstr = NULL;
            CONTROLTYPEID typeId = 0;
            pElement->get_CurrentName(&nameBstr);
            pElement->get_CurrentControlType(&typeId);

            if (nameBstr && wcslen(nameBstr) > 0) {
                if (typeId == 50000 || typeId == 50020) { 
                    std::wstring clickedName(nameBstr);
                    
                    // CHECK 1: Drive Letter? "Media (D:)"
                    std::wstring drivePath = ParseDriveLetter(clickedName);
                    if (!drivePath.empty()) {
                        NavigateNewTab(drivePath);
                    } 
                    else {
                        // CHECK 2: Standard Folder
                        IUIAutomationElement* pRoot = NULL;
                        HWND hForeground = GetForegroundWindow();
                        pAutomation->ElementFromHandle(hForeground, &pRoot);

                        if (pRoot) {
                            std::wstring fullPath = FindPathInWindow(pAutomation, pRoot);
                            if (!fullPath.empty()) {
                                size_t found = fullPath.rfind(clickedName);
                                if (found != std::wstring::npos) {
                                    std::wstring targetPath = fullPath.substr(0, found + clickedName.length());
                                    NavigateNewTab(targetPath);
                                }
                            }
                            pRoot->Release();
                        }
                    }
                }
            }
            if (nameBstr) SysFreeString(nameBstr);
            pElement->Release();
        }
        pAutomation->Release();
    }
}

// ============================================================================
// HOOKS
// ============================================================================

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MBUTTONUP) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        HWND hWndTarget = WindowFromPoint(pMouse->pt);
        DWORD targetPID = 0;
        GetWindowThreadProcessId(hWndTarget, &targetPID);
        if (targetPID == g_dwCurrentPID) {
            AnalyzeElement(pMouse->pt);
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    g_dwThreadId = GetCurrentThreadId();
    g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    if (g_hMouseHook) {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        UnhookWindowsHookEx(g_hMouseHook);
    }
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    g_dwCurrentPID = GetCurrentProcessId();
    g_hThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_dwThreadId != 0) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        if (g_hThread) {
            WaitForSingleObject(g_hThread, 1000);
            CloseHandle(g_hThread);
        }
    }
}
