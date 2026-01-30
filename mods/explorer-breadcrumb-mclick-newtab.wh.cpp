// ==WindhawkMod==
// @id              explorer-breadcrumb-mclick-newtab
// @name            Explorer Breadcrumb Middle-Click New Tab
// @description     Opens folders in a new tab by middle-clicking breadcrumb items (Linux style).
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         explorer.exe
// @compilerOptions -luiautomationcore -lole32 -loleaut32 -lshlwapi
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
  during the split-second navigation sequence (approx. 300ms).
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <windhawk_utils.h>
#include <UIAutomation.h>
#include <comdef.h>
#include <string>
#include <cwctype> 
#include <shlobj.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <shldisp.h>

const IID IID_Folder2_Manual = {0xF0D2D8EF, 0x3890, 0x11D2, {0xBF, 0x8B, 0x00, 0xC0, 0x4F, 0xB9, 0x36, 0x61}};

#ifndef MAX_URL_LENGTH
#define MAX_URL_LENGTH 2084
#endif

HHOOK g_hMouseHook = NULL;
DWORD g_dwCurrentPID = 0;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;

// ============================================================================
// UTILS
// ============================================================================

std::wstring UrlToPath(std::wstring url) {
    DWORD len = MAX_URL_LENGTH;
    WCHAR buffer[MAX_URL_LENGTH];
    if (PathCreateFromUrlW(url.c_str(), buffer, &len, 0) == S_OK) {
        return std::wstring(buffer);
    }
    return url;
}

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

// ----------------------------------------------------------------------------
// THE TAB CLOSE FIX: BREADCRUMB WHITELIST
// Instead of trying to detect Tabs (which look like generic windows),
// we strictly verify if the clicked element is part of the Breadcrumb Bar.
// ----------------------------------------------------------------------------
bool IsBreadcrumb(IUIAutomation* pAutomation, IUIAutomationElement* pElement) {
    IUIAutomationTreeWalker* pWalker = NULL;
    pAutomation->get_ControlViewWalker(&pWalker);
    if (!pWalker) return false;

    IUIAutomationElement* pParent = pElement;
    // Walk up 5 levels (Breadcrumb control is usually at Level 1 or 2)
    for (int i = 0; i < 5; i++) {
        IUIAutomationElement* pTemp = NULL;
        pWalker->GetParentElement(pParent, &pTemp);
        
        if (pTemp) {
            BSTR className = NULL;
            BSTR autoId = NULL;
            pTemp->get_CurrentClassName(&className);
            pTemp->get_CurrentAutomationId(&autoId);
            
            bool isMatch = false;

            // CHECK 1: Class Name contains "BreadcrumbBarItemControl"
            if (className && wcsstr(className, L"BreadcrumbBarItemControl") != NULL) {
                isMatch = true;
            }
            
            // CHECK 2: Automation ID is "PART_BreadcrumbBar"
            if (autoId && wcscmp(autoId, L"PART_BreadcrumbBar") == 0) {
                isMatch = true;
            }

            if (className) SysFreeString(className);
            if (autoId) SysFreeString(autoId);

            if (isMatch) { 
                pTemp->Release();
                pWalker->Release();
                if (pParent != pElement) pParent->Release();
                return true; // IT IS A BREADCRUMB! PROCEED.
            }
            
            if (pParent != pElement) pParent->Release();
            pParent = pTemp;
        } else {
            break;
        }
    }
    if (pParent && pParent != pElement) pParent->Release();
    pWalker->Release();
    return false; // Not a breadcrumb (likely a Tab or Title Bar)
}

// ============================================================================
// CORE: GET PATH FROM COM
// ============================================================================
std::wstring GetPathFromCOM() {
    std::wstring currentPath = L"";
    IShellWindows* pShellWindows = NULL;
    
    HWND hForeground = GetForegroundWindow();
    DWORD dwForegroundThread = GetWindowThreadProcessId(hForeground, NULL);

    if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&pShellWindows))) {
        long count = 0;
        pShellWindows->get_Count(&count);
        
        for (long i = 0; i < count; i++) {
            IDispatch* pDisp = NULL;
            if (SUCCEEDED(pShellWindows->Item(variant_t(i), &pDisp))) {
                IWebBrowserApp* pApp = NULL;
                if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pApp))) {
                    HWND hWndApp = NULL;
                    pApp->get_HWND((LONG_PTR*)&hWndApp);
                    DWORD dwAppThread = GetWindowThreadProcessId(hWndApp, NULL);

                    if (dwAppThread == dwForegroundThread) {
                        bool found = false;
                        IDispatch* pDoc = NULL;
                        if (SUCCEEDED(pApp->get_Document(&pDoc)) && pDoc) {
                            IShellFolderViewDual* pView = NULL;
                            if (SUCCEEDED(pDoc->QueryInterface(IID_IShellFolderViewDual, (void**)&pView))) {
                                Folder* pFolder = NULL;
                                if (SUCCEEDED(pView->get_Folder(&pFolder))) {
                                    Folder2* pFolder2 = NULL;
                                    if (SUCCEEDED(pFolder->QueryInterface(IID_Folder2_Manual, (void**)&pFolder2))) {
                                        FolderItem* pSelf = NULL;
                                        if (SUCCEEDED(pFolder2->get_Self(&pSelf))) {
                                            BSTR pathBstr = NULL;
                                            pSelf->get_Path(&pathBstr);
                                            if (pathBstr) {
                                                currentPath = std::wstring(pathBstr);
                                                SysFreeString(pathBstr);
                                                found = true;
                                            }
                                            pSelf->Release();
                                        }
                                        pFolder2->Release();
                                    }
                                    pFolder->Release();
                                }
                                pView->Release();
                            }
                            pDoc->Release();
                        }

                        if (!found) {
                            BSTR urlBstr = NULL;
                            pApp->get_LocationURL(&urlBstr);
                            if (urlBstr) {
                                currentPath = UrlToPath(std::wstring(urlBstr));
                                SysFreeString(urlBstr);
                            }
                        }
                    }
                    pApp->Release();
                }
                pDisp->Release();
            }
            if (!currentPath.empty()) break;
        }
        pShellWindows->Release();
    }
    return currentPath;
}

std::wstring GetPathFromUI(IUIAutomation* pAutomation, IUIAutomationElement* pRootElement) {
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
// NAVIGATION
// ============================================================================

void NavigateNewTab(const std::wstring& targetPath) {
    if (targetPath.length() < 2) return;

    static UINT cfExclude = RegisterClipboardFormat(L"ExcludeClipboardContentFromMonitorProcessing");
    static UINT cfCanInclude = RegisterClipboardFormat(L"CanIncludeInClipboardHistory");

    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        
        size_t size = (targetPath.length() + 1) * sizeof(WCHAR);
        HGLOBAL hGlobalPath = GlobalAlloc(GMEM_MOVEABLE, size);
        if (hGlobalPath) {
            void* pData = GlobalLock(hGlobalPath);
            memcpy(pData, targetPath.c_str(), size);
            GlobalUnlock(hGlobalPath);
            SetClipboardData(CF_UNICODETEXT, hGlobalPath);
        }

        HGLOBAL hGlobalExclude = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hGlobalExclude) {
            void* pData = GlobalLock(hGlobalExclude);
            *(DWORD*)pData = 0; 
            GlobalUnlock(hGlobalExclude);
            SetClipboardData(cfExclude, hGlobalExclude);
        }
        
        HGLOBAL hGlobalCanInclude = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hGlobalCanInclude) {
            void* pData = GlobalLock(hGlobalCanInclude);
            *(DWORD*)pData = 0; 
            GlobalUnlock(hGlobalCanInclude);
            SetClipboardData(cfCanInclude, hGlobalCanInclude);
        }

        CloseClipboard();
    }

    INPUT inputs[4] = {};
    int idx = 0;

    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(300); 

    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++; 
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));

    Sleep(100);

    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'V'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'V'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(50);

    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
}

// ============================================================================
// CORE LOGIC
// ============================================================================

void AnalyzeElement(POINT pt) {
    IUIAutomation* pAutomation = NULL;
    IUIAutomationElement* pElement = NULL;

    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pAutomation);
    
    if (SUCCEEDED(hr) && pAutomation) {
        hr = pAutomation->ElementFromPoint(pt, &pElement);
        if (SUCCEEDED(hr) && pElement) {
            
            // ----------------------------------------------------------------
            // WHITELIST CHECK: Only proceed if it is a BREADCRUMB
            // ----------------------------------------------------------------
            if (!IsBreadcrumb(pAutomation, pElement)) {
                // Not a breadcrumb (likely a Tab or Title Bar). Abort.
                pElement->Release();
                pAutomation->Release();
                return;
            }

            BSTR nameBstr = NULL;
            CONTROLTYPEID typeId = 0;
            pElement->get_CurrentName(&nameBstr);
            pElement->get_CurrentControlType(&typeId);

            if (nameBstr && wcslen(nameBstr) > 0) {
                // Button or Text
                if (typeId == 50000 || typeId == 50020) { 
                    std::wstring clickedName(nameBstr);
                    
                    std::wstring drivePath = ParseDriveLetter(clickedName);
                    if (!drivePath.empty()) {
                        NavigateNewTab(drivePath);
                    } 
                    else {
                        std::wstring fullPath = GetPathFromCOM();

                        if (fullPath.empty()) {
                            IUIAutomationElement* pRoot = NULL;
                            HWND hForeground = GetForegroundWindow();
                            pAutomation->ElementFromHandle(hForeground, &pRoot);
                            if (pRoot) {
                                fullPath = GetPathFromUI(pAutomation, pRoot);
                                pRoot->Release();
                            }
                        }

                        if (!fullPath.empty()) {
                            size_t found = fullPath.rfind(clickedName);
                            if (found != std::wstring::npos) {
                                std::wstring targetPath = fullPath.substr(0, found + clickedName.length());
                                NavigateNewTab(targetPath);
                            }
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
