// ==WindhawkMod==
// @id              task-manager-tail
// @name            Task Manager Tail
// @description     Automatically keeps Task Manager (or other apps) at the end of the taskbar. (Windows 11)
// @version         1.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @compilerOptions -luser32 -loleacc -loleaut32 -luuid -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Tail 1.0

This mod ensures that **Task Manager** always stays at the tail end of your taskbar on **Windows 11**.

When you open or close other applications, this mod detects the change and automatically
moves the Task Manager button to the tail end of the list.

**Features:**
- **Event Driven:** Uses lightweight hooks to detect window changes instantly.
- **Zero Polling:** Does not waste CPU cycles checking the taskbar constantly.
- **Configurable:** Supports non-English languages and other target applications.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetName: Task Manager
  $name: Target Button Name
  $description: The text on the taskbar button (partial match). Change this for other languages (e.g., "Gestionnaire").
- targetClass: TaskManagerWindow
  $name: Target Window Class
  $description: The internal class name of the window. Use "Notepad" to tail Notepad, etc.
- moveDelay: 100
  $name: Move Delay (ms)
  $description: How long to wait before moving the button after a check confirms it is out of place.
- debounceTime: 300
  $name: Event Debounce Time (ms)
  $description: How long to wait after window events stop occurring before checking the taskbar.
- enableLogging: false
  $name: Enable Logging
  $description: Show debug messages in the Windhawk log window.
*/
// ==/WindhawkModSettings==

#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <objbase.h>
#include <uiautomation.h>
#include <shobjidl.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <stdarg.h>
#include <wchar.h>

// Custom Message to wake up the thread
#define WM_TRIGGER_CHECK (WM_USER + 1)

struct UIAItem {
    IUIAutomationElement* element;
    BSTR name;
};

struct Settings {
    std::wstring targetName;
    std::wstring targetClass;
    int moveDelay;
    int debounceTime;
    bool enableLogging;
} g_settings;

// Global thread control
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;
volatile bool g_stopThread = false;
DWORD g_lastAttemptTime = 0;

void LoadSettings() {
    PCWSTR target = Wh_GetStringSetting(L"targetName");
    g_settings.targetName = target ? target : L"Task Manager";
    Wh_FreeStringSetting(target);

    PCWSTR cls = Wh_GetStringSetting(L"targetClass");
    g_settings.targetClass = cls ? cls : L"TaskManagerWindow";
    Wh_FreeStringSetting(cls);

    g_settings.moveDelay = Wh_GetIntSetting(L"moveDelay");
    if (g_settings.moveDelay < 0) g_settings.moveDelay = 0;

    g_settings.debounceTime = Wh_GetIntSetting(L"debounceTime");
    if (g_settings.debounceTime < 50) g_settings.debounceTime = 50;

    g_settings.enableLogging = Wh_GetIntSetting(L"enableLogging");
}

void Log(const wchar_t* fmt, ...) {
    if (!g_settings.enableLogging) return;
    va_list args;
    va_start(args, fmt);
    wchar_t buffer[1024];
    vswprintf(buffer, 1024, fmt, args);
    va_end(args);
    Wh_Log(L"%s", buffer);
}

// --- WinEvent Hook Callback ---
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
                           LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    // Filter: We only care about top-level window events (OBJID_WINDOW)
    if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF && g_dwThreadId != 0) {
        // Post message to trigger check (debounced in main loop)
        PostThreadMessage(g_dwThreadId, WM_TRIGGER_CHECK, 0, 0);
    }
}

void CycleTaskbarTab(HWND hwnd) {
    Log(L"Cycling Taskbar Tab for HWND %p via ITaskbarList", hwnd);

    ITaskbarList* pTaskbarList = NULL;
    HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (void**)&pTaskbarList);
    
    if (SUCCEEDED(hr) && pTaskbarList) {
        hr = pTaskbarList->HrInit();
        if (SUCCEEDED(hr)) {
            pTaskbarList->DeleteTab(hwnd);
            Sleep(200); // Wait for removal animation
            pTaskbarList->AddTab(hwnd);
            Sleep(50);
            pTaskbarList->ActivateTab(hwnd);
        }
        pTaskbarList->Release();
    }
}

void CheckAndMove(IUIAutomation* pAutomation) {
    // Find the actual target window to move
    HWND hTargetWnd = FindWindowW(g_settings.targetClass.c_str(), NULL);
    if (!hTargetWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return;

    HWND hChild = GetWindow(hTray, GW_CHILD);
    while (hChild) {
        wchar_t text[256] = {0};
        GetWindowTextW(hChild, text, 255);
        
        // Find the XAML Island containing the buttons
        if (wcsstr(text, L"DesktopWindowXamlSource")) {
            IUIAutomationElement* pRoot = NULL;
            HRESULT hr = pAutomation->ElementFromHandle(hChild, &pRoot);
            if (SUCCEEDED(hr) && pRoot) {
                IUIAutomationCondition* pTrueCondition = NULL;
                pAutomation->CreateTrueCondition(&pTrueCondition);
                
                IUIAutomationElementArray* pChildren = NULL;
                pRoot->FindAll(TreeScope_Descendants, pTrueCondition, &pChildren);
                
                if (pChildren) {
                    int count = 0;
                    pChildren->get_Length(&count);
                    
                    std::vector<UIAItem> buttons;
                    int targetIndex = -1;

                    for (int i = 0; i < count; i++) {
                        IUIAutomationElement* pChild = NULL;
                        pChildren->GetElement(i, &pChild);
                        if (pChild) {
                            BSTR cls = NULL;
                            pChild->get_CurrentClassName(&cls);
                            
                            if (cls && wcsstr(cls, L"TaskListButtonAutomationPeer")) {
                                UIAItem item;
                                item.element = pChild;
                                pChild->AddRef();
                                pChild->get_CurrentName(&item.name);
                                
                                buttons.push_back(item);
                                
                                // Check if this button matches our target name
                                if (item.name && wcsstr(item.name, g_settings.targetName.c_str())) {
                                    targetIndex = (int)buttons.size() - 1;
                                }
                            }
                            if (cls) SysFreeString(cls);
                            pChild->Release();
                        }
                    }
                    
                    // If found and NOT at the end
                    if (targetIndex != -1 && static_cast<size_t>(targetIndex) < buttons.size() - 1) {
                        // Safety cooldown to prevent loops
                        if (GetTickCount() - g_lastAttemptTime > 1000) {
                            Log(L"Target found at index %d (of %d). Moving...", targetIndex, buttons.size());
                            if (hTargetWnd && IsWindow(hTargetWnd)) {
                                if (g_settings.moveDelay > 0) Sleep(g_settings.moveDelay);
                                CycleTaskbarTab(hTargetWnd);
                                g_lastAttemptTime = GetTickCount();
                            }
                        }
                    }

                    for (auto& b : buttons) {
                        if (b.name) SysFreeString(b.name);
                        if (b.element) b.element->Release();
                    }
                    pChildren->Release();
                }
                pTrueCondition->Release();
                pRoot->Release();
            }
        }
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }
}

DWORD WINAPI BackgroundThread(LPVOID) {
    Log(L"Task Manager Tail Thread Started");
    g_dwThreadId = GetCurrentThreadId();

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) return 1;

    IUIAutomation* pAutomation = NULL;
    hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pAutomation);
    
    if (SUCCEEDED(hr) && pAutomation) {
        // Register WinEventHook for Window Creation/Destruction/Show/Hide
        HWINEVENTHOOK hHook = SetWinEventHook(
            EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE,
            NULL, WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        );

        if (hHook) {
            Log(L"WinEventHook Registered. Waiting for events...");
            
            MSG msg;
            UINT_PTR debounceTimer = 0;

            while (!g_stopThread && GetMessage(&msg, NULL, 0, 0)) {
                if (msg.message == WM_TRIGGER_CHECK) {
                    // Debounce: Reset timer to fire after debounceTime
                    if (debounceTimer) KillTimer(NULL, debounceTimer);
                    debounceTimer = SetTimer(NULL, 0, g_settings.debounceTime, NULL);
                }
                else if (msg.message == WM_TIMER && msg.wParam == debounceTimer) {
                    KillTimer(NULL, debounceTimer);
                    debounceTimer = 0;
                    CheckAndMove(pAutomation);
                }
                
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            UnhookWinEvent(hHook);
        }
        pAutomation->Release();
    }
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopThread = false;
    g_hThread = CreateThread(NULL, 0, BackgroundThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_stopThread = true;
    if (g_dwThreadId) PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
