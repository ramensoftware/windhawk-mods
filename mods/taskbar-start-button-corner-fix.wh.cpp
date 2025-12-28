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

// Settings
struct {
    int edgeThickness;
    int edgeLength;
    bool debugLogging;
} g_settings;

// Global state
HHOOK g_mouseHook = NULL;
bool g_inputSiteHooked = false;

// Mouse hook state (only accessed from InputSite thread)
bool g_mouseDownInCorner = false;
bool g_menuOpenAtMouseDown = false;

// UI Automation (initialized on InputSite thread)
IUIAutomation* g_pAutomation = nullptr;
bool g_comInitialized = false;
HWND g_inputSiteWnd = nullptr;

// Helper to run code on a window's thread
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                static const UINT msg =
                    RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == msg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// Cleanup UI Automation (called on InputSite thread)
void WINAPI CleanupUIAutomation(void*) {
    if (g_pAutomation) {
        g_pAutomation->Release();
        g_pAutomation = nullptr;
        if (g_comInitialized) {
            CoUninitialize();
            g_comInitialized = false;
        }
        Wh_Log(L"UI Automation cleaned up on InputSite thread");
    }
}

// Initialize UI Automation (call from InputSite thread only)
bool InitUIAutomation() {
    if (g_pAutomation) return true;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return false;
    g_comInitialized = true;

    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                          __uuidof(IUIAutomation), (void**)&g_pAutomation);
    if (FAILED(hr) || !g_pAutomation) {
        CoUninitialize();
        g_comInitialized = false;
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
            g_mouseDownInCorner = IsInCornerRegion(pMouse->pt.x, pMouse->pt.y);

            if (g_mouseDownInCorner) {
                g_menuOpenAtMouseDown = IsStartMenuOpen();
                if (g_settings.debugLogging) {
                    Wh_Log(L"Mouse down at (%d, %d) in corner", pMouse->pt.x, pMouse->pt.y);
                }
            }
        }
        else if (wParam == WM_LBUTTONUP && g_mouseDownInCorner) {
            // Only toggle if menu state hasn't changed (real button didn't handle it)
            if (g_menuOpenAtMouseDown == IsStartMenuOpen()) {
                Wh_Log(L"Corner click at (%d, %d) - toggling Start menu", pMouse->pt.x, pMouse->pt.y);
                ToggleStartMenu();
            }
            g_mouseDownInCorner = false;
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
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

    g_inputSiteWnd = hWnd;
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

// Find taskbar window in current process (other programs may use Shell_TrayWnd class)
HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            wchar_t className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
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
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
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

    // Clean up UI Automation on the InputSite thread
    if (g_inputSiteWnd && IsWindow(g_inputSiteWnd)) {
        RunFromWindowThread(g_inputSiteWnd, CleanupUIAutomation, nullptr);
    }
    g_inputSiteWnd = nullptr;

    g_inputSiteHooked = false;
    Wh_Log(L"Uninitialized");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}