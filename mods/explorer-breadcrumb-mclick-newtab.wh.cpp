// ==WindhawkMod==
// @id              explorer-breadcrumb-mclick-newtab
// @name            Explorer Breadcrumb Middle-Click New Tab
// @description     Opens folders in a new tab by middle-clicking breadcrumb items (Linux style).
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         explorer.exe
// @compilerOptions -luiautomationcore -lole32 -loleaut32 -luser32 -lshlwapi
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
challenges for automation, as standard API calls often return data for inactive/background tabs.
This mod uses a **Pure Input-Driven Approach** to ensure 100% accuracy.

1.  **Threaded Hook:** Installs a Low-Level Mouse Hook (WH_MOUSE_LL) on a 
    dedicated native thread to detect middle-clicks without freezing the UI.
2.  **Active Focus Probe (The Clipboard Proxy):** * Instead of guessing the active tab via complex memory queries (COM), the mod asks Windows directly.
    * It momentarily forces the Address Bar into Edit Mode (`Alt+D`), simulates a Copy command (`Ctrl+C`), 
      and reads the **Clipboard**. 
    * This guarantees retrieval of the path for the **currently visible tab**, bypassing all internal ambiguity.
3.  **Sibling Counting (The Duplicate Fix):** * Instead of unreliable string matching (which fails with duplicate names like `...\Source\Source`), 
      the mod uses **Visual Geometry**.
    * It identifies the clicked item and counts how many folders are visually to its right.
    * *Example:* If you middle-click the 3rd folder from the end, the mod intelligently strips 
      the last 2 folders from the full path retrieved in Step 2.
4.  **Smart Navigation Sequence:**
    * **Silent Clipboard:** It backs up your current clipboard, performs the navigation using a temporary clipboard entry 
      (flagged to be ignored by Clipboard History), and then **restores your original clipboard data**.
    * **Focus Guard:** Simulates `Ctrl+T` -> `Alt+D` -> `Paste` -> `Enter`. The `Alt+D` step is 
      crucial to ensure focus isn't lost to the file view during animation.

## ✨ Key Features & Fixes
* **Duplicate Folder Support:** Perfectly handles paths with repeating names.
* **Zombie Tab Prevention:** Uses strict "Breadcrumb Whitelisting" to ensure middle-clicking 
    Tab Titles or the Window Title Bar allows Windows to close the tab natively.
* **Drive Letter Support:** Detects `(C:)` style items and opens the drive root instantly.
* **Hidden Address Bar Support:** Works even if "Display full path in title bar" is disabled.

## ✅ Compatibility
- Works on Windows 11 (22H2, 23H2, 24H2, 25H2 and Insider builds).
- Compatible with all Explorer view modes (Compact, Touch, etc.).

## ⚠️ Limitations
- **Virtual Folders:** May not work on special locations like "Control Panel" 
  or "This PC" root if no valid filesystem path exists.
- **Input Interference:** Uses `SendInput`. Avoid touching the mouse/keyboard 
  during the split-second navigation sequence (approx. 400ms).
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <windhawk_utils.h>
#include <UIAutomation.h>
#include <string>
#include <cwctype> 
#include <shlwapi.h>

// Manual GUIDs to avoid linker errors
const CLSID CLSID_CUIAutomation_Manual = { 0xff48dba4, 0x60ef, 0x4201, { 0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e } };
const IID IID_IUIAutomation_Manual = { 0x30cbe57d, 0xd9d0, 0x452a, { 0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee } };

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

std::wstring AscendPath(std::wstring path, int levels) {
    if (levels <= 0) return path;
    
    // Safety check
    if (path.find(L":\\") == std::wstring::npos && path.find(L"\\\\") == std::wstring::npos) {
        return L""; 
    }

    WCHAR buffer[MAX_PATH];
    wcsncpy(buffer, path.c_str(), MAX_PATH);
    buffer[MAX_PATH - 1] = 0;
    
    for (int i = 0; i < levels; i++) {
        PathRemoveFileSpecW(buffer);
    }
    return std::wstring(buffer);
}

// ----------------------------------------------------------------------------
// LOGIC: VISUAL GEOMETRY
// ----------------------------------------------------------------------------
int CountVisuallyToRight(IUIAutomation* pAutomation, IUIAutomationElement* pClickedItem) {
    if (!pClickedItem) return -1;

    RECT rcClicked = {};
    if (FAILED(pClickedItem->get_CurrentBoundingRectangle(&rcClicked))) return -1;
    
    IUIAutomationTreeWalker* pWalker = NULL;
    pAutomation->get_ControlViewWalker(&pWalker);
    IUIAutomationElement* pRoot = NULL;
    
    IUIAutomationElement* pTemp = pClickedItem;
    pTemp->AddRef();
    while(true) {
        IUIAutomationElement* pParent = NULL;
        pWalker->GetParentElement(pTemp, &pParent);
        pTemp->Release();
        pTemp = pParent;
        if (pTemp) {
            CONTROLTYPEID tid = 0;
            pTemp->get_CurrentControlType(&tid);
            if (tid == 50032) { // Window
                pRoot = pTemp;
                pRoot->AddRef();
                break;
            }
        } else {
            break;
        }
    }
    if (pTemp) pTemp->Release();
    pWalker->Release();

    if (!pRoot) return -1;

    int itemsToRight = 0;
    IUIAutomationElementArray* pFound = NULL;
    IUIAutomationCondition* pCondition = NULL;
    
    VARIANT varProp;
    VariantInit(&varProp);
    varProp.vt = VT_BSTR;
    varProp.bstrVal = SysAllocString(L"FileExplorerExtensions.BreadcrumbBarItemControl");
    
    if (SUCCEEDED(pAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp, &pCondition))) {
        if (SUCCEEDED(pRoot->FindAll(TreeScope_Descendants, pCondition, &pFound)) && pFound) {
            int count = 0;
            pFound->get_Length(&count);
            for (int i = 0; i < count; i++) {
                IUIAutomationElement* pItem = NULL;
                pFound->GetElement(i, &pItem);
                if (pItem) {
                    RECT rcItem = {};
                    if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&rcItem))) {
                        // Check if strictly right
                        if (abs(rcItem.top - rcClicked.top) < 20) { 
                            if (rcItem.left > rcClicked.left) { 
                                itemsToRight++;
                            }
                        }
                    }
                    pItem->Release();
                }
            }
            pFound->Release();
        }
        pCondition->Release();
    }
    VariantClear(&varProp);
    pRoot->Release();

    return itemsToRight;
}

// ----------------------------------------------------------------------------
// HELPER: BACKUP & RESTORE CLIPBOARD
// ----------------------------------------------------------------------------
struct ClipboardBackup {
    bool hasData;
    HANDLE hData;
    UINT format;
};

ClipboardBackup BackupClipboard() {
    ClipboardBackup backup = { false, NULL, 0 };
    if (OpenClipboard(NULL)) {
        if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            HANDLE hRaw = GetClipboardData(CF_UNICODETEXT);
            if (hRaw) {
                size_t size = GlobalSize(hRaw);
                HANDLE hCopy = GlobalAlloc(GMEM_MOVEABLE, size);
                if (hCopy) {
                    void* pSrc = GlobalLock(hRaw);
                    void* pDst = GlobalLock(hCopy);
                    memcpy(pDst, pSrc, size);
                    GlobalUnlock(hCopy);
                    GlobalUnlock(hRaw);
                    
                    backup.hasData = true;
                    backup.hData = hCopy;
                    backup.format = CF_UNICODETEXT;
                }
            }
        }
        CloseClipboard();
    }
    return backup;
}

void RestoreClipboard(ClipboardBackup backup) {
    if (!backup.hasData || !backup.hData) return;

    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(backup.format, backup.hData);
        CloseClipboard();
    } else {
        GlobalFree(backup.hData);
    }
}

// ----------------------------------------------------------------------------
// CORE: PROBE ADDRESS BAR (CLIPBOARD METHOD)
// ----------------------------------------------------------------------------
std::wstring ProbeAddressBarWithClipboard() {
    std::wstring foundPath = L"";
    
    // 1. Backup User's Clipboard
    ClipboardBackup backup = BackupClipboard();

    // 2. Perform Input Sequence: Alt+D -> Ctrl+C -> Esc
    INPUT inputs[8] = {};
    int idx = 0;
    
    // Alt+D (Focus & Select All)
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(60); 

    // Ctrl+C (Copy)
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'C'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'C'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));

    Sleep(60); 

    // Esc (Restore View)
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_ESCAPE; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_ESCAPE; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(50); 

    // 3. Read Probe Result
    if (OpenClipboard(NULL)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData) {
            WCHAR* pszText = (WCHAR*)GlobalLock(hData);
            if (pszText) {
                foundPath = std::wstring(pszText);
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }

    // 4. Restore User's Clipboard
    RestoreClipboard(backup);

    return foundPath;
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
        
        // Mark as "Do Not Record" in Clipboard History
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
    
    // Ctrl+T
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(450); 

    // Alt+D
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++; 
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(100);

    // Ctrl+V
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
// CORE LOGIC
// ============================================================================
void AnalyzeElement(POINT pt) {
    IUIAutomation* pAutomation = NULL;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation_Manual, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation_Manual, (void**)&pAutomation);
    
    if (SUCCEEDED(hr) && pAutomation) {
        IUIAutomationElement* pElement = NULL;
        if (SUCCEEDED(pAutomation->ElementFromPoint(pt, &pElement)) && pElement) {
            
            bool isBreadcrumb = false;
            IUIAutomationElement* pBreadcrumbItem = NULL;
            IUIAutomationTreeWalker* pWalker = NULL;
            pAutomation->get_ControlViewWalker(&pWalker);
            IUIAutomationElement* pTemp = pElement;
            pTemp->AddRef();
            
            for (int i=0; i<6; i++) {
                BSTR cls = NULL;
                pTemp->get_CurrentClassName(&cls);
                if (cls && wcsstr(cls, L"BreadcrumbBarItemControl") != NULL) {
                    isBreadcrumb = true;
                    pBreadcrumbItem = pTemp; 
                    pBreadcrumbItem->AddRef(); 
                    SysFreeString(cls);
                    break;
                }
                if (cls) SysFreeString(cls);
                IUIAutomationElement* pPar = NULL;
                pWalker->GetParentElement(pTemp, &pPar);
                pTemp->Release();
                pTemp = pPar;
                if (!pTemp) break;
            }
            if (pTemp) pTemp->Release();
            
            if (isBreadcrumb && pBreadcrumbItem) {
                 BSTR nameBstr = NULL;
                 pElement->get_CurrentName(&nameBstr);
                 std::wstring clickedName = nameBstr ? std::wstring(nameBstr) : L"";
                 std::wstring drivePath = ParseDriveLetter(clickedName);
                 if (nameBstr) SysFreeString(nameBstr);

                 if (!drivePath.empty()) {
                     NavigateNewTab(drivePath);
                 } 
                 else {
                     int levelsUp = CountVisuallyToRight(pAutomation, pBreadcrumbItem);
                     
                     if (levelsUp >= 0) {
                         std::wstring fullPath = ProbeAddressBarWithClipboard();
                         if (!fullPath.empty()) {
                             std::wstring targetPath = AscendPath(fullPath, levelsUp);
                             if (!targetPath.empty()) {
                                NavigateNewTab(targetPath);
                             }
                         }
                     }
                 }
                 pBreadcrumbItem->Release();
            }
            
            pWalker->Release();
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