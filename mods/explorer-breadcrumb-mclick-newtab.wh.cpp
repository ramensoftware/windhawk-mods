// ==WindhawkMod==
// @id              explorer-breadcrumb-mclick-newtab
// @name            Explorer Breadcrumb Middle-Click New Tab
// @description     Opens folders in a new tab by middle-clicking breadcrumb items and their dropdown menus (Linux style).
// @version         1.2.0
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
challenges for automation. Version 1.2.0 completely abandons the Windows Clipboard 
to ensure absolute stability and preserve the user's copied files.

1.  **Threaded Hook:** Installs a Low-Level Mouse Hook (WH_MOUSE_LL) on a 
    dedicated native thread to detect middle-clicks without freezing the UI.
2.  **UIA Focus Probe:** * Simulates `Alt+D` to focus the address bar.
    * Uses `UIAutomation` to grab the currently focused element and extract its `ValuePattern` or `Name`.
    * Simulates `Esc` to safely exit edit mode.
3.  **Sibling Counting (The Duplicate Fix):** * Identifies the clicked item and counts how many folders are visually to its right to resolve duplicate folder names (e.g., `...\Source\Source`).
4.  **Unicode Injection Navigation:**
    * Simulates `Ctrl+T` (New Tab) -> `Alt+D` (Focus Address Bar).
    * Converts the target path into an array of `KEYEVENTF_UNICODE` input structures.
    * "Types" the path directly into the address bar at machine speed, bypassing the clipboard entirely.
    * Simulates `Enter`.

## ✨ Key Features & Fixes
* **Dropdown Menu Support (NEW in 1.2.0):** Middle-clicking items inside the breadcrumb dropdown menus (the `>` arrows) now opens them in a new tab!
* **100% Clipboard-Free:** Will not interfere with your copied files, images, or formatting.
* **Crash-Free:** Removes legacy clipboard polling that caused Explorer instability.
* **Duplicate Folder Support:** Perfectly handles paths with repeating names.
* **Zombie Tab Prevention:** Strict "Breadcrumb Whitelisting" ensures middle-clicking other areas doesn't trigger false positives.
* **Drive Letter Support:** Detects `(C:)` style items and opens the drive root instantly.

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
#include <vector>
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
// CORE: PROBE ADDRESS BAR (UIA METHOD - NO CLIPBOARD)
// ----------------------------------------------------------------------------
std::wstring ProbeAddressBarWithUIA(IUIAutomation* pAutomation) {
    std::wstring foundPath = L"";
    
    // 1. Alt+D to force focus on the address bar
    INPUT inputs[4] = {};
    int idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    // Wait for XAML animation and focus shift
    Sleep(150); 

    // 2. Read the focused element using UIA
    IUIAutomationElement* pFocusedElement = NULL;
    if (SUCCEEDED(pAutomation->GetFocusedElement(&pFocusedElement)) && pFocusedElement) {
        
        // Try to get the Value Pattern (standard for edit boxes)
        IUIAutomationValuePattern* pValuePattern = NULL;
        if (SUCCEEDED(pFocusedElement->GetCurrentPattern(UIA_ValuePatternId, (IUnknown**)&pValuePattern)) && pValuePattern) {
            BSTR valBstr = NULL;
            if (SUCCEEDED(pValuePattern->get_CurrentValue(&valBstr)) && valBstr) {
                foundPath = std::wstring(valBstr);
                SysFreeString(valBstr);
            }
            pValuePattern->Release();
        }
        
        // Fallback: If Value Pattern fails, try getting the Name property
        if (foundPath.empty()) {
            BSTR nameBstr = NULL;
            if (SUCCEEDED(pFocusedElement->get_CurrentName(&nameBstr)) && nameBstr) {
                foundPath = std::wstring(nameBstr);
                SysFreeString(nameBstr);
            }
        }
        pFocusedElement->Release();
    }

    // 3. Esc to exit address bar edit mode
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_ESCAPE; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_ESCAPE; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    
    Sleep(50); 

    return foundPath;
}

// ============================================================================
// NAVIGATION (UNICODE INJECTION - NO CLIPBOARD)
// ============================================================================
void TypeUnicodeString(const std::wstring& text) {
    if (text.empty()) return;

    std::vector<INPUT> inputs;
    inputs.reserve(text.length() * 2);

    for (wchar_t c : text) {
        INPUT inDown = {};
        inDown.type = INPUT_KEYBOARD;
        inDown.ki.wVk = 0; // wVk must be 0 for Unicode
        inDown.ki.wScan = c;
        inDown.ki.dwFlags = KEYEVENTF_UNICODE;
        inputs.push_back(inDown);

        INPUT inUp = {};
        inUp.type = INPUT_KEYBOARD;
        inUp.ki.wVk = 0;
        inUp.ki.wScan = c;
        inUp.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs.push_back(inUp);
    }

    if (!inputs.empty()) {
        SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
    }
}

void NavigateNewTab(const std::wstring& targetPath) {
    if (targetPath.length() < 2) return;

    INPUT inputs[4] = {};
    int idx = 0;
    
    // 1. Ctrl+T (New Tab)
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(450); // Wait for tab creation

    // 2. Alt+D (Focus Address Bar)
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++; 
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(150); // Wait for focus

    // 3. Inject Path directly (Bypasses Clipboard)
    TypeUnicodeString(targetPath);
    Sleep(50);

    // 4. Enter
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
}

// ----------------------------------------------------------------------------
// LOGIC: DROPDOWN MENU SUPPORT
// ----------------------------------------------------------------------------
IUIAutomationElement* FindExpandedBreadcrumbElement(IUIAutomation* pAutomation, IUIAutomationElement* pMainWin) {
    IUIAutomationCondition* pCondExpanded = NULL;
    VARIANT varState;
    VariantInit(&varState);
    varState.vt = VT_I4;
    varState.lVal = ExpandCollapseState_Expanded;
    
    IUIAutomationElement* pResult = NULL;
    if (SUCCEEDED(pAutomation->CreatePropertyCondition(UIA_ExpandCollapseExpandCollapseStatePropertyId, varState, &pCondExpanded))) {
        IUIAutomationElementArray* pFound = NULL;
        if (SUCCEEDED(pMainWin->FindAll(TreeScope_Descendants, pCondExpanded, &pFound)) && pFound) {
            int count = 0;
            pFound->get_Length(&count);
            
            IUIAutomationTreeWalker* pWalker = NULL;
            pAutomation->get_ControlViewWalker(&pWalker);

            for (int i = 0; i < count; i++) {
                IUIAutomationElement* pItem = NULL;
                pFound->GetElement(i, &pItem);
                if (pItem) {
                    bool inBreadcrumb = false;
                    IUIAutomationElement* pTemp = pItem;
                    pTemp->AddRef();
                    
                    // Traverse up to verify it belongs to the BreadcrumbBar
                    for (int j = 0; j < 10; j++) {
                        BSTR clsName = NULL;
                        pTemp->get_CurrentClassName(&clsName);
                        if (clsName && wcsstr(clsName, L"BreadcrumbBar") != NULL) {
                            inBreadcrumb = true;
                            SysFreeString(clsName);
                            break;
                        }
                        if (clsName) SysFreeString(clsName);
                        
                        IUIAutomationElement* pPar = NULL;
                        if (pWalker) pWalker->GetParentElement(pTemp, &pPar);
                        pTemp->Release();
                        pTemp = pPar;
                        if (!pTemp) break;
                    }
                    if (pTemp) pTemp->Release();

                    if (inBreadcrumb) {
                        pResult = pItem; // Found the expanded chevron/breadcrumb
                        break;
                    } else {
                        pItem->Release();
                    }
                }
            }
            if (pWalker) pWalker->Release();
            pFound->Release();
        }
        pCondExpanded->Release();
    }
    VariantClear(&varState);
    return pResult;
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
            
            for (int i = 0; i < 6; i++) {
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
            
            BSTR nameBstr = NULL;
            pElement->get_CurrentName(&nameBstr);
            std::wstring clickedName = nameBstr ? std::wstring(nameBstr) : L"";
            if (nameBstr) SysFreeString(nameBstr);

            // PATH 1: Direct Breadcrumb Click
            if (isBreadcrumb && pBreadcrumbItem) {
                 std::wstring drivePath = ParseDriveLetter(clickedName);
                 if (!drivePath.empty()) {
                     NavigateNewTab(drivePath);
                 } 
                 else {
                     int levelsUp = CountVisuallyToRight(pAutomation, pBreadcrumbItem);
                     if (levelsUp >= 0) {
                         std::wstring fullPath = ProbeAddressBarWithUIA(pAutomation);
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
            // PATH 2: Dropdown Menu Click
            else if (!clickedName.empty()) {
                CONTROLTYPEID tid = 0;
                pElement->get_CurrentControlType(&tid);
                
                // Verify it's a menu/list item to prevent random text clicks
                if (tid == UIA_TextControlTypeId || tid == UIA_ListItemControlTypeId || tid == UIA_MenuItemControlTypeId) {
                    HWND hMainWnd = GetForegroundWindow();
                    wchar_t cls[256] = {0};
                    GetClassNameW(hMainWnd, cls, 256);
                    
                    // Ensure we are operating within Explorer
                    if (wcscmp(cls, L"CabinetWClass") == 0) {
                        IUIAutomationElement* pMainWin = NULL;
                        if (SUCCEEDED(pAutomation->ElementFromHandle(hMainWnd, &pMainWin)) && pMainWin) {
                            
                            // Find which breadcrumb chevron is currently expanded
                            IUIAutomationElement* pExpandedBreadcrumb = FindExpandedBreadcrumbElement(pAutomation, pMainWin);
                            if (pExpandedBreadcrumb) {
                                int levelsUp = CountVisuallyToRight(pAutomation, pExpandedBreadcrumb);
                                if (levelsUp >= 0) {
                                    std::wstring fullPath = ProbeAddressBarWithUIA(pAutomation);
                                    // Note: ProbeAddressBarWithUIA triggers Alt+D, automatically closing the dropdown!
                                    
                                    if (!fullPath.empty()) {
                                        std::wstring targetPath = AscendPath(fullPath, levelsUp);
                                        if (!targetPath.empty()) {
                                            // Combine with clicked subfolder
                                            if (targetPath.back() != L'\\') targetPath += L"\\";
                                            targetPath += clickedName;
                                            
                                            NavigateNewTab(targetPath);
                                        }
                                    }
                                }
                                pExpandedBreadcrumb->Release();
                            }
                            pMainWin->Release();
                        }
                    }
                }
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