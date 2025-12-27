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

This mod installs a thread-specific mouse hook on the InputSite window's thread.
This hook intercepts mouse clicks at the corner region and invokes the Start menu via
UI Automation when the real button doesn't handle the click.

The hook can only be installed on threads within the same process, so only the explorer.exe
that owns the taskbar will successfully activate the mod. A background worker thread waits
for the taskbar to appear (without blocking explorer startup) and shuts itself down when the
taskbar has been aquired. A mutex prevents subequent / invalid explorer processes from infinitely running this worker.

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
#include <mutex>

// Settings
struct {
    int edgeThickness;
    int edgeLength;
    bool debugLogging;
} g_settings;

// Global state
std::atomic<bool> g_mouseDownInCorner{false};
std::atomic<bool> g_menuOpenAtMouseDown{false};
std::atomic<bool> g_shuttingDown{false};
HHOOK g_mouseHook = NULL;
HANDLE g_workerThread = NULL;

// Mutex to indicate an instance has claimed the taskbar
#define TASKBAR_MUTEX_NAME L"Local\\TaskbarStartMenuCornerFix"
HANDLE g_taskbarMutex = NULL;

// For signaling main thread to install hook
#define HOOK_INSTALL_TIMER_ID 0x5442  // "TB" in hex
std::atomic<DWORD> g_pendingHookThreadId{0};
std::atomic<HWND> g_taskbarWnd{NULL};

// UI Automation
IUIAutomation* g_pAutomation = nullptr;
std::mutex g_automationMutex;

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

// Initialize UI Automation
bool InitUIAutomation() {
    std::lock_guard<std::mutex> lock(g_automationMutex);
    if (g_pAutomation) return true;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE && hr != S_FALSE) return false;

    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                          __uuidof(IUIAutomation), (void**)&g_pAutomation);
    return SUCCEEDED(hr) && g_pAutomation;
}

// Helper: Find Start button element via UI Automation (caller must Release)
// Must be called with g_automationMutex held
IUIAutomationElement* GetStartButtonElement() {
    if (!g_pAutomation) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(g_pAutomation->GetRootElement(&pRoot)) || !pRoot) return nullptr;

    VARIANT varProp;
    varProp.vt = VT_BSTR;
    varProp.bstrVal = SysAllocString(L"StartButton");

    IUIAutomationCondition* pCondition = nullptr;
    HRESULT hr = g_pAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, varProp, &pCondition);
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
    std::lock_guard<std::mutex> lock(g_automationMutex);
    IUIAutomationElement* pStartButton = GetStartButtonElement();
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
    std::lock_guard<std::mutex> lock(g_automationMutex);
    IUIAutomationElement* pStartButton = GetStartButtonElement();
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

// Find InputSite window
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

// Check if mutex is already owned by another instance
bool IsMutexOwned() {
    HANDLE testMutex = OpenMutexW(SYNCHRONIZE, FALSE, TASKBAR_MUTEX_NAME);
    if (testMutex) {
        CloseHandle(testMutex);
        return true;  // Mutex exists = another instance owns it
    }
    return false;
}

// Timer callback - runs on taskbar thread to install hook
VOID CALLBACK HookInstallTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    (void)uMsg; (void)dwTime;
    KillTimer(hwnd, idEvent);

    DWORD inputSiteThreadId = g_pendingHookThreadId.load();
    if (inputSiteThreadId == 0) {
        Wh_Log(L"Timer fired but no pending thread ID");
        return;
    }

    g_mouseHook = SetWindowsHookExW(WH_MOUSE, MouseHookProc, NULL, inputSiteThreadId);
    if (!g_mouseHook) {
        Wh_Log(L"Failed to install mouse hook from timer (error: %lu)", GetLastError());
        if (g_taskbarMutex) {
            ReleaseMutex(g_taskbarMutex);
            CloseHandle(g_taskbarMutex);
            g_taskbarMutex = NULL;
        }
        return;
    }

    Wh_Log(L"Mouse hook installed on InputSite thread %lu (from taskbar thread)", inputSiteThreadId);
}

// Worker thread that waits for taskbar + InputSite and signals main thread
DWORD WINAPI WorkerThread(LPVOID lpParam) {
    (void)lpParam;
    DWORD ourPid = GetCurrentProcessId();

    // Wait for taskbar AND InputSite to appear
    HWND taskbarWnd = NULL;
    HWND inputSiteWnd = NULL;

    while (!g_shuttingDown.load()) {
        // Check if another instance already has the mutex
        if (IsMutexOwned()) {
            Wh_Log(L"Another instance already active - mod inactive");
            return 0;
        }

        // Find taskbar
        taskbarWnd = FindWindowW(L"Shell_TrayWnd", NULL);
        if (taskbarWnd) {
            DWORD taskbarPid = 0;
            GetWindowThreadProcessId(taskbarWnd, &taskbarPid);

            if (taskbarPid != ourPid) {
                Wh_Log(L"Taskbar owned by different process - mod inactive");
                return 0;
            }

            // Taskbar is ours - now find InputSite
            inputSiteWnd = NULL;
            EnumChildWindows(taskbarWnd, FindInputSiteCallback, (LPARAM)&inputSiteWnd);

            if (inputSiteWnd) {
                break;  // Found both taskbar and InputSite
            }
        }

        Sleep(500);
    }

    if (g_shuttingDown.load()) return 0;

    // Acquire mutex now that we're ready to install the hook
    g_taskbarMutex = CreateMutexW(NULL, TRUE, TASKBAR_MUTEX_NAME);
    if (!g_taskbarMutex) {
        Wh_Log(L"Failed to create mutex");
        return 0;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_taskbarMutex);
        g_taskbarMutex = NULL;
        Wh_Log(L"Another instance claimed mutex first - mod inactive");
        return 0;
    }

    DWORD inputSiteThreadId = GetWindowThreadProcessId(inputSiteWnd, NULL);

    // Store thread ID and taskbar window for timer callback
    g_pendingHookThreadId.store(inputSiteThreadId);
    g_taskbarWnd.store(taskbarWnd);

    // Schedule hook installation on taskbar thread via timer
    // SetTimer posts WM_TIMER to the window's thread, so the callback runs there
    if (!SetTimer(taskbarWnd, HOOK_INSTALL_TIMER_ID, 0, HookInstallTimerProc)) {
        Wh_Log(L"Failed to set timer for hook installation (error: %lu)", GetLastError());
        ReleaseMutex(g_taskbarMutex);
        CloseHandle(g_taskbarMutex);
        g_taskbarMutex = NULL;
        return 0;
    }

    Wh_Log(L"Scheduled hook installation on taskbar thread for InputSite thread %lu", inputSiteThreadId);
    return 1;
}

void LoadSettings() {
    g_settings.edgeThickness = Wh_GetIntSetting(L"edgeThickness");
    g_settings.edgeLength = Wh_GetIntSetting(L"edgeLength");
    g_settings.debugLogging = Wh_GetIntSetting(L"debugLogging");
}

void CleanupUIAutomation() {
    std::lock_guard<std::mutex> lock(g_automationMutex);
    if (g_pAutomation) {
        g_pAutomation->Release();
        g_pAutomation = nullptr;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing...");

    LoadSettings();

    if (!InitUIAutomation()) {
        Wh_Log(L"Warning: UI Automation init failed");
    }

    // Spawn worker thread to wait for taskbar and install hook
    g_workerThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread");
        return FALSE;
    }

    Wh_Log(L"Worker thread spawned, waiting for taskbar...");
    return TRUE;
}

void Wh_ModUninit() {
    // Signal worker thread to stop
    g_shuttingDown.store(true);

    // Wait for worker thread to finish
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }

    // Kill any pending timer
    HWND taskbarWnd = g_taskbarWnd.load();
    if (taskbarWnd) {
        KillTimer(taskbarWnd, HOOK_INSTALL_TIMER_ID);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }
    if (g_taskbarMutex) {
        ReleaseMutex(g_taskbarMutex);
        CloseHandle(g_taskbarMutex);
        g_taskbarMutex = NULL;
    }
    CleanupUIAutomation();
    Wh_Log(L"Uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    LoadSettings();
    *bReload = FALSE;
    return TRUE;
}
