// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar Overflow on Hover
// @description     A zero-click QoL upgrade. Opens the Windows 11 taskbar overflow menu on hover and smartly closes it when you move away or click.
// @version         1.0
// @author          Cougar16
// @github          https://github.com/Cougar16
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Overflow on Hover 🚀
**Upgrade your Windows 11 experience with this seamless Quality-of-Life enhancement.**

Tired of clicking the small three-dot overflow icon on your taskbar? This mod transforms the overflow menu into a fluid, responsive, zero-click experience. Just hover your mouse over the chevron button, and your hidden system tray icons appear instantly. 

![How It Works](https://raw.githubusercontent.com/Cougar16/win11-taskbar-overflow-hover/main/assets/how-it-works.png)

✨ **Smart Features:**
* **Zero-Click Open:** Auto-expands the hidden icons menu just by resting your cursor on the overflow button (300ms default delay, easily adjustable).
* **Auto-Dismiss:** Gracefully closes the menu automatically when your mouse leaves the overflow button or the flyout menu.
* **Instant App Launch:** Instantly dismisses the overflow flyout the exact moment you click an app inside it, bypassing native Windows UI delay.
* **Customizable Delays:** Fine-tune the hover-to-open and hover-to-close timings to perfectly match your personal workflow.

*Developed by Cougar16 for the Windows customization community.*
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
#include <uiautomation.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>

// -------------------------------------------------------------------------
// Global Configuration & State
// -------------------------------------------------------------------------
struct {
    int hoverDelayMs;
    int closeDelayMs;
} settings;

bool g_modUnloading = false;
HANDLE g_hThread = NULL;

// -------------------------------------------------------------------------
// Core UI Automation Engine
// -------------------------------------------------------------------------
DWORD WINAPI HoverMonitorThread(LPVOID lpParam) {
    CoInitialize(NULL);
    IUIAutomation* pAutomation = NULL;
    
    HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, 
                                  __uuidof(IUIAutomation), (void**)&pAutomation);
    
    if (FAILED(hr)) {
        Wh_Log(L"[Cougar16] Failed to initialize UI Automation engine.");
        CoUninitialize();
        return 1;
    }

    int currentHoverTime = 0;
    int currentCloseHoverTime = 0;
    bool menuOpened = false;
    bool wasMouseDown = false;

    // Main Monitor Loop
    while (!g_modUnloading) {
        Sleep(50); // Engine tick rate (50ms)

        // -----------------------------------------------------------------
        // Phase 1: Instant Close on App Click
        // -----------------------------------------------------------------
        bool isMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool mouseClicked = (wasMouseDown && !isMouseDown); 
        wasMouseDown = isMouseDown;

        if (menuOpened && mouseClicked) {
            // Sleep briefly to ensure the XAML app button receives the click 
            // before we forcefully dismiss the host flyout menu.
            Sleep(30); 

            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = VK_ESCAPE;
            SendInput(1, &input, sizeof(INPUT));
            
            input.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &input, sizeof(INPUT));
            
            menuOpened = false;
            currentCloseHoverTime = 0;
            currentHoverTime = 0;
            continue; 
        }

        // -----------------------------------------------------------------
        // Phase 2: Cursor Tracking & XAML Tree Traversal
        // -----------------------------------------------------------------
        POINT pt;
        GetCursorPos(&pt);

        IUIAutomationElement* pElement = NULL;
        hr = pAutomation->ElementFromPoint(pt, &pElement);
        
        bool isOverflow = false;

        if (SUCCEEDED(hr) && pElement != NULL) {
            IUIAutomationInvokePattern* pInvoke = NULL;
            IUIAutomationExpandCollapsePattern* pExpand = NULL;
            IUIAutomationTogglePattern* pToggle = NULL;

            IUIAutomationTreeWalker* pWalker = NULL;
            pAutomation->get_ControlViewWalker(&pWalker);

            IUIAutomationElement* pNode = pElement;
            pNode->AddRef();

            // Walk up the XAML UI tree (up to 15 levels) to bypass nested graphics 
            // and locate the actual clickable Button container in Win11 26220+ builds.
            for (int i = 0; i < 15 && pNode != NULL; i++) {
                if (!isOverflow) {
                    BSTR autoId = NULL;
                    pNode->get_CurrentAutomationId(&autoId);
                    if (autoId) {
                        if (wcscmp(autoId, L"Chevron") == 0 || wcscmp(autoId, L"OverflowButton") == 0) {
                            isOverflow = true;
                        }
                        SysFreeString(autoId);
                    }
                }

                if (!pInvoke) pNode->GetCurrentPattern(UIA_InvokePatternId, (IUnknown**)&pInvoke);
                if (!pExpand) pNode->GetCurrentPattern(UIA_ExpandCollapsePatternId, (IUnknown**)&pExpand);
                if (!pToggle) pNode->GetCurrentPattern(UIA_TogglePatternId, (IUnknown**)&pToggle);

                if (isOverflow && (pInvoke || pExpand || pToggle)) break; 

                IUIAutomationElement* pParent = NULL;
                if (pWalker) pWalker->GetParentElement(pNode, &pParent);
                pNode->Release();
                pNode = pParent;
            }

            if (pNode) pNode->Release();
            if (pWalker) pWalker->Release();

            // -------------------------------------------------------------
            // Phase 3: Opening Logic
            // -------------------------------------------------------------
            if (isOverflow) {
                if (!menuOpened) {
                    currentHoverTime += 50;
                    
                    if (currentHoverTime >= settings.hoverDelayMs) {
                        if (pInvoke) {
                            pInvoke->Invoke();
                        } else if (pExpand) {
                            pExpand->Expand();
                        } else if (pToggle) {
                            pToggle->Toggle();
                        } else {
                            // Hardware click fallback for deeply nested un-patterned elements
                            INPUT inputDown = {};
                            inputDown.type = INPUT_MOUSE;
                            inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                            SendInput(1, &inputDown, sizeof(INPUT));
                            Sleep(50);
                            INPUT inputUp = {};
                            inputUp.type = INPUT_MOUSE;
                            inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                            SendInput(1, &inputUp, sizeof(INPUT));
                        }
                        
                        menuOpened = true; 
                    }
                }
            } else {
                currentHoverTime = 0;
            }

            if (pInvoke) pInvoke->Release();
            if (pExpand) pExpand->Release();
            if (pToggle) pToggle->Release();
            pElement->Release();
        }

        // -----------------------------------------------------------------
        // Phase 4: Closing Logic (Hover Out)
        // -----------------------------------------------------------------
        if (menuOpened) {
            bool inFlyout = false;
            
            HWND hWndPoint = WindowFromPoint(pt);
            HWND hRootPoint = GetAncestor(hWndPoint, GA_ROOT);
            
            wchar_t rootClass[256] = {0};
            GetClassNameW(hRootPoint, rootClass, ARRAYSIZE(rootClass));

            DWORD pid = 0;
            GetWindowThreadProcessId(hRootPoint, &pid);
            
            DWORD explorerPid = 0;
            HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
            GetWindowThreadProcessId(hTaskbar, &explorerPid);
            
            // Check if cursor is residing inside an active Explorer.exe UI layer 
            // that is acting as the overflow popup menu.
            if (pid == explorerPid) {
                if (wcscmp(rootClass, L"Shell_TrayWnd") == 0 || 
                    wcscmp(rootClass, L"WorkerW") == 0 || 
                    wcscmp(rootClass, L"Progman") == 0) {
                    inFlyout = false; 
                } else {
                    inFlyout = true;
                }
            }

            if (isOverflow || inFlyout) {
                // Cursor is resting safely inside the UI bounds
                currentCloseHoverTime = 0;
            } else {
                // Cursor has exited the UI bounds
                currentCloseHoverTime += 50;
                
                if (currentCloseHoverTime >= settings.closeDelayMs) {
                    // Send an ESCAPE stroke to gracefully dismiss the Windows XAML flyout
                    INPUT input = {};
                    input.type = INPUT_KEYBOARD;
                    input.ki.wVk = VK_ESCAPE;
                    SendInput(1, &input, sizeof(INPUT));
                    
                    input.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &input, sizeof(INPUT));
                    
                    menuOpened = false;
                    currentCloseHoverTime = 0;
                    currentHoverTime = 0;
                }
            }
        } else {
            currentCloseHoverTime = 0;
        }
    }

    pAutomation->Release();
    CoUninitialize();
    return 0;
}

// -------------------------------------------------------------------------
// Windhawk Mod Lifecycle Hooks
// -------------------------------------------------------------------------
void LoadSettings() {
    settings.hoverDelayMs = Wh_GetIntSetting(L"hoverDelayMs");
    settings.closeDelayMs = Wh_GetIntSetting(L"closeDelayMs");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Overflow on Hover v1.0 by Cougar16");
    LoadSettings();

    g_modUnloading = false;
    g_hThread = CreateThread(NULL, 0, HoverMonitorThread, NULL, 0, NULL);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading Taskbar Overflow on Hover mod");
    g_modUnloading = true;
    
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 1000);
        CloseHandle(g_hThread);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[Cougar16] Settings updated. Applying new delay configurations.");
    LoadSettings();
}
