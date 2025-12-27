// ==WindhawkMod==
// @id              taskbar-start-button-corner-fix
// @name            Start Menu Corner Click Fix
// @description     Fixes the issue where clicking in the corner of the taskbar doesn't open the Start menu on multi monitor setups.
// @version         1.0
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Corner Click Fix

Fixes an issue on Windows 11 where clicking in the very corner of the taskbar doesn't open the Start menu
on left-aligned taskbars when the start menu button is "in a sticky corner".

## The Problem

Windows 11's taskbar has an InputSite window (Windows.UI.Input.InputSite.WindowClass)
that intercepts mouse clicks before they reach the Start button.

This seems to be caused by the "sticky corners" feature, and thus only affects multi-monitor setups.

## The Solution

This mod hooks `CreateWindowInBand` to detect when the InputSite window is created,
then installs a thread-specific mouse hook on its thread. This hook intercepts mouse
clicks at the corner region and invokes the Start menu via UI Automation when the
real button doesn't handle the click.

## Requirements

- Windows 11 64-bit (tested on 25H2)
- Windhawk v1.4 or later
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- edgeThickness: 5
  $name: Edge thickness
  $description: Thickness of the L-shaped edge region in pixels
- edgeLength: 5
  $name: Edge length
  $description: How far the horizontal arm of the L extends from the corner in pixels
- debugLogging: false
  $name: Enable debug logging
  $description: Log click events for debugging
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <objbase.h>
#include <uiautomation.h>

#include <atomic>

// Settings
struct {
    int edgeThickness;
    int edgeLength;
    bool debugLogging;
} g_settings;

// Global state
std::atomic<bool> g_mouseDownInCorner{false};
std::atomic<bool> g_menuOpenAtMouseDown{false};
HHOOK g_mouseHook = NULL;
bool g_inputSiteHooked = false;

// UI Automation (initialized on InputSite thread)
IUIAutomation* g_pAutomation = nullptr;
HWND g_cleanupWindow = nullptr;
const wchar_t* g_cleanupWindowClass = L"StartButtonFix_Cleanup";

#define WM_CLEANUP_COM (WM_USER + 100)

// Cleanup window procedure - runs on InputSite thread
LRESULT CALLBACK CleanupWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLEANUP_COM:
            if (g_pAutomation) {
                g_pAutomation->Release();
                g_pAutomation = nullptr;
                CoUninitialize();
                Wh_Log(L"UI Automation cleaned up on InputSite thread");
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Initialize UI Automation (call from InputSite thread only)
bool InitUIAutomation() {
    if (g_pAutomation) return true;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE && hr != S_FALSE) return false;

    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                          __uuidof(IUIAutomation), (void**)&g_pAutomation);
    if (FAILED(hr) || !g_pAutomation) {
        CoUninitialize();
        return false;
    }

    Wh_Log(L"UI Automation initialized on thread %lu", GetCurrentThreadId());
    return true;
}

// Check if point is in corner region (L-shaped Fitts' Law corner)
bool IsInCornerRegion(int x, int y) {
    int edgeThickness = g_settings.edgeThickness;
    if (edgeThickness <= 0) edgeThickness = 5;

    int edgeLength = g_settings.edgeLength;
    if (edgeLength <= 0) edgeLength = 5;

    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Must be in corner area: within edgeLength of left edge, near bottom of screen
    if (x < 0 || x > edgeLength) return false;
    if (y < screenHeight - edgeLength) return false;

    // L-shape: left edge (vertical arm) OR bottom edge (horizontal arm)
    return (x < edgeThickness) || (y > screenHeight - edgeThickness);
}

// Helper: Find Start button element via UI Automation (caller must Release)
IUIAutomationElement* GetStartButtonElement(IUIAutomation* pAutomation) {
    if (!pAutomation) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(pAutomation->GetRootElement(&pRoot)) || !pRoot) return nullptr;

    VARIANT varProp;
    varProp.vt = VT_BSTR;
    varProp.bstrVal = SysAllocString(L"StartButton");

    IUIAutomationCondition* pCondition = nullptr;
    HRESULT hr = pAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, varProp, &pCondition);
    SysFreeString(varProp.bstrVal);

    if (FAILED(hr) || !pCondition) {
        pRoot->Release();
        return nullptr;
    }

    IUIAutomationElement* pStartButton = nullptr;
    pRoot->FindFirst(TreeScope_Descendants, pCondition, &pStartButton);
    pCondition->Release();
    pRoot->Release();
    return pStartButton;
}

// Check if Start menu is open via toggle state
bool IsStartMenuOpen() {
    if (!InitUIAutomation()) return false;

    IUIAutomationElement* pStartButton = GetStartButtonElement(g_pAutomation);
    if (!pStartButton) return false;

    bool isOpen = false;
    IUIAutomationTogglePattern* pToggle = nullptr;
    if (SUCCEEDED(pStartButton->GetCurrentPatternAs(UIA_TogglePatternId,
            __uuidof(IUIAutomationTogglePattern), (void**)&pToggle)) && pToggle) {
        ToggleState state;
        if (SUCCEEDED(pToggle->get_CurrentToggleState(&state))) {
            isOpen = (state == ToggleState_On);
        }
        pToggle->Release();
    }
    pStartButton->Release();
    return isOpen;
}

// Toggle Start menu via UI Automation
void ToggleStartMenu() {
    if (!InitUIAutomation()) return;

    IUIAutomationElement* pStartButton = GetStartButtonElement(g_pAutomation);
    if (!pStartButton) {
        Wh_Log(L"Start button not found, using Win key fallback");
        INPUT inputs[2] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_LWIN;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK_LWIN;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, inputs, sizeof(INPUT));
        return;
    }

    // Try Toggle pattern first
    IUIAutomationTogglePattern* pToggle = nullptr;
    if (SUCCEEDED(pStartButton->GetCurrentPatternAs(UIA_TogglePatternId,
            __uuidof(IUIAutomationTogglePattern), (void**)&pToggle)) && pToggle) {
        pToggle->Toggle();
        pToggle->Release();
        pStartButton->Release();
        return;
    }

    // Fall back to Invoke pattern
    IUIAutomationInvokePattern* pInvoke = nullptr;
    if (SUCCEEDED(pStartButton->GetCurrentPatternAs(UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&pInvoke)) && pInvoke) {
        pInvoke->Invoke();
        pInvoke->Release();
    }
    pStartButton->Release();
}

// Thread-specific mouse hook on InputSite thread
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && lParam) {
        MOUSEHOOKSTRUCT* pMouse = (MOUSEHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            bool inCorner = IsInCornerRegion(pMouse->pt.x, pMouse->pt.y);
            g_mouseDownInCorner.store(inCorner);

            if (inCorner) {
                g_menuOpenAtMouseDown.store(IsStartMenuOpen());
                if (g_settings.debugLogging) {
                    Wh_Log(L"Mouse down at (%d, %d) in corner", pMouse->pt.x, pMouse->pt.y);
                }
            }
        }
        else if (wParam == WM_LBUTTONUP && g_mouseDownInCorner.load()) {
            bool menuWasOpen = g_menuOpenAtMouseDown.load();
            bool menuIsOpen = IsStartMenuOpen();

            // Only toggle if menu state hasn't changed (real button didn't handle it)
            if (menuWasOpen == menuIsOpen) {
                Wh_Log(L"Corner click at (%d, %d) - toggling Start menu", pMouse->pt.x, pMouse->pt.y);
                ToggleStartMenu();
            }
            g_mouseDownInCorner.store(false);
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// Create cleanup window on InputSite thread (called via SendMessage)
void CreateCleanupWindow() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = CleanupWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = g_cleanupWindowClass;
    RegisterClass(&wc);

    g_cleanupWindow = CreateWindowEx(
        0, g_cleanupWindowClass, NULL, 0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, wc.hInstance, NULL);

    if (g_cleanupWindow) {
        Wh_Log(L"Cleanup window created on thread %lu", GetCurrentThreadId());
    }
}

// Handle InputSite window - install mouse hook on its thread
void HandleInputSiteWindow(HWND hWnd) {
    if (g_inputSiteHooked) {
        return;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, NULL);
    g_mouseHook = SetWindowsHookExW(WH_MOUSE, MouseHookProc, NULL, threadId);
    if (!g_mouseHook) {
        Wh_Log(L"Failed to install mouse hook (error: %lu)", GetLastError());
        return;
    }

    // Create cleanup window on the InputSite thread via a timer callback
    SetTimer(hWnd, (UINT_PTR)CreateCleanupWindow, 0, [](HWND hWnd, UINT, UINT_PTR idEvent, DWORD) {
        KillTimer(hWnd, idEvent);
        CreateCleanupWindow();
    });

    g_inputSiteHooked = true;
    Wh_Log(L"Mouse hook installed on InputSite thread %lu", threadId);
}

// CreateWindowInBand hook
using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;

HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        wcscmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleInputSiteWindow(hWnd);
    }

    return hWnd;
}

// Find existing InputSite window
BOOL CALLBACK FindInputSiteCallback(HWND hWnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassNameW(hWnd, className, 256)) {
        if (wcscmp(className, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
            *(HWND*)lParam = hWnd;
            return FALSE;
        }
    }
    return TRUE;
}

void LoadSettings() {
    g_settings.edgeThickness = Wh_GetIntSetting(L"edgeThickness");
    g_settings.edgeLength = Wh_GetIntSetting(L"edgeLength");
    g_settings.debugLogging = Wh_GetIntSetting(L"debugLogging");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing...");

    LoadSettings();

    // Hook CreateWindowInBand to detect InputSite window creation
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        void* pCreateWindowInBand = (void*)GetProcAddress(user32, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
    }

    // Check for existing InputSite window (taskbar may already exist)
    HWND taskbarWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    if (taskbarWnd) {
        HWND inputSiteWnd = NULL;
        EnumChildWindows(taskbarWnd, FindInputSiteCallback, (LPARAM)&inputSiteWnd);
        if (inputSiteWnd) {
            Wh_Log(L"Found existing InputSite window: %08X", (DWORD)(ULONG_PTR)inputSiteWnd);
            HandleInputSiteWindow(inputSiteWnd);
        }
    }

    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModUninit() {
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }

    // Send cleanup message to the InputSite thread
    if (g_cleanupWindow) {
        SendMessageTimeout(g_cleanupWindow, WM_CLEANUP_COM, 0, 0,
                           SMTO_BLOCK | SMTO_ABORTIFHUNG, 1000, NULL);
        // DestroyWindow must be called from the owning thread, use SendMessage
        SendMessageTimeout(g_cleanupWindow, WM_CLOSE, 0, 0,
                           SMTO_BLOCK | SMTO_ABORTIFHUNG, 1000, NULL);
        g_cleanupWindow = nullptr;
    }

    UnregisterClass(g_cleanupWindowClass, GetModuleHandle(NULL));

    g_inputSiteHooked = false;
    Wh_Log(L"Uninitialized");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}