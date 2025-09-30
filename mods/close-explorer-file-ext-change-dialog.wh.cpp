// ==WindhawkMod==
// @id           close-explorer-file-ext-change-dialog
// @name         Close explorer file ext change dialog
// @description  Automatically clicks "Yes" on the confirmation dialog when changing a file extension in Explorer.
// @version      1.0
// @author       CandyTek
// @github       https://github.com/CandyTek
// @include      explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -luiautomationcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Close explorer file ext change dialog

This mod automatically clicks "Yes" on the "Rename" confirmation prompt that appears when you change a file's extension in Windows File Explorer. (Windows 10 EN and ZH-CN)

*/
// ==/WindhawkModReadme==

#include <UIAutomation.h>
#include <dwmapi.h>
#include <windhawk_utils.h>
#include <iostream>
#include <string>

// Global variables to manage thread
HANDLE g_hWinEventHookThread = NULL;
DWORD g_dwWinEventHookThreadId = 0;

// Global UIA interface pointer
IUIAutomation* g_pAutomation = NULL;

bool FindTextInWindow(HWND hwnd, const wchar_t* targetText) {
    if (g_pAutomation == NULL) {
        // UIA not initialized, cannot proceed.
        return false;
    }

    IUIAutomationElement* pRootElement = NULL;
    HRESULT hr = g_pAutomation->ElementFromHandle(hwnd, &pRootElement);
    if (FAILED(hr) || pRootElement == NULL) {
        return false;
    }

    IUIAutomationCondition* pCondition = NULL;
    VARIANT varProp;
    varProp.vt = VT_I4;
    varProp.lVal = UIA_TextControlTypeId;
    hr = g_pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pCondition);

    if (FAILED(hr)) {
        pRootElement->Release();
        return false;
    }

    // Find all text elements, not just the first one.
    IUIAutomationElementArray* pFoundElements = NULL;
    hr = pRootElement->FindAll(TreeScope_Descendants, pCondition, &pFoundElements);

    pRootElement->Release();
    pCondition->Release();

    if (FAILED(hr) || pFoundElements == NULL) {
        return false;
    }

    bool bFound = false;
    int elementCount = 0;
    pFoundElements->get_Length(&elementCount);

    for (int i = 0; i < elementCount; ++i) {
        IUIAutomationElement* pElement = NULL;
        pFoundElements->GetElement(i, &pElement);

        if (pElement) {
            BSTR bstrName = NULL;
            pElement->get_CurrentName(&bstrName);
            pElement->Release();

            if (bstrName) {
                // For English UI: "If you change a file name extension, the file might become unusable."
                // For Chinese UI: "如果改变文件扩展名，可能会导致文件不可用。"
                // We check for a unique part of the string.
                if (wcsstr(bstrName, targetText) != NULL) {
                    bFound = true;
                    SysFreeString(bstrName);
                    break; 
                }
                SysFreeString(bstrName);
            }
        }
    }

    pFoundElements->Release();
    return bFound;
}

BOOL CALLBACK EnumChildProcClickYes(HWND hwndChild, LPARAM lParam) {
    // Check control type
    wchar_t className[256];
    GetClassName(hwndChild, className, sizeof(className));

    // If it's a button control
    if (wcscmp(className, L"Button") == 0) {
        // Get button text
        wchar_t buttonText[256];
        GetWindowText(hwndChild, buttonText,
                      sizeof(buttonText) / sizeof(wchar_t));
        // Check if button text contains &Y
        if (wcsstr(buttonText, L"&Y") != NULL) {
            // Output button handle and text
            Wh_Log(L"Button with '&Y' found! %s", buttonText);

            // Simulate button click
            SendMessage(hwndChild, BM_CLICK, 0, 0);

            // Return FALSE to end enumeration
            return FALSE;
        }
    }
    // Continue enumerating other controls
    return TRUE;
}

// A callback function that will be called when a relevant event occurs.
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD dwEventThread,
                           DWORD dwmsEventTime) {
    if (!IsWindow(hwnd)) {
        return;
    }

    wchar_t className[256];
    if (GetClassName(hwnd, className, ARRAYSIZE(className)) == 0) {
        return;  // Failed to get class name
    }
    // We only care about valid window handles and the correct event.
    Wh_Log(L"WinEventHook HWND: %p", hwnd);

    // Standard Windows dialogs often use the class name "#32770"
    if (wcscmp(className, L"#32770") == 0) {
        wchar_t windowTitle[256];
        if (GetWindowText(hwnd, windowTitle, ARRAYSIZE(windowTitle)) > 0) {
            // Check if the title contains "Rename" (or "重命名" for Chinese UI)
            // Using wcsstr for partial matching is more robust.
            if (wcscmp(windowTitle, L"Rename") == 0 ||
                wcscmp(windowTitle, L"重命名") == 0) {
                // Now, use the pre-initialized UIA to find the text.
                // This is much lighter than initializing UIA every time.
                // We check for unique text from both English and Chinese
                // versions.
                if (FindTextInWindow(hwnd, L"如果改变文件扩展名") ||
                    FindTextInWindow(hwnd, L"change a file name extension")) {
                    Wh_Log(L"Detected file extension change dialog (HWND: %p).",
                           hwnd);
                    // If the text is found, find and click the "Yes" button.
                    EnumChildWindows(hwnd, EnumChildProcClickYes, 0);
                }
            }
        }
    }
}

// Event monitoring thread function
DWORD WINAPI MyWinEventHookThread(LPVOID lpParam) {
    // OPTIMIZATION 1: Initialize COM once for this thread.
    // COINIT_APARTMENTTHREADED is often required for UI frameworks. If it
    // causes issues, COINIT_MULTITHREADED can be tried.
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to initialize COM library. Error code = 0x%X", hr);
        return 1;
    }

    // OPTIMIZATION 2: Create the UI Automation object once.
    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                          __uuidof(IUIAutomation), (void**)&g_pAutomation);
    if (FAILED(hr) || g_pAutomation == NULL) {
        Wh_Log(L"Failed to create UI Automation instance.");
        CoUninitialize();
        return 1;
    }

    // Register events you care about. You can call this multiple times to
    // monitor different event ranges. Here we monitor window creation and
    // destruction events.
    HWINEVENTHOOK hHook1 =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
                        EVENT_SYSTEM_FOREGROUND,  // Event range
                        NULL,          // Monitor events from all processes
                        WinEventProc,  // Your callback function
                        GetCurrentProcessId(),  // Just hook current process
                        0,  // Monitor all processes and threads
                        WINEVENT_OUTOFCONTEXT);

    if (!hHook1) {
        g_pAutomation->Release();
        CoUninitialize();
        Wh_Log(L"Failed to set event hook 1.");
        return 1;
    }

    // Start message loop to receive events
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        // The example code adds a custom message to gracefully exit the thread
        // When we want to stop the thread, we can send it a WM_APP message
        if (msg.hwnd == NULL && msg.message == WM_APP) {
            PostQuitMessage(0);  // This will cause GetMessage to return 0, thus
                                 // exiting the loop
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up hooks before thread exits
    if (hHook1) {
        UnhookWinEvent(hHook1);
    }
    // OPTIMIZATION 4: Clean up UIA and COM only when the thread exits.
    if (g_pAutomation) {
        g_pAutomation->Release();
        g_pAutomation = NULL;
    }
    CoUninitialize();

    Wh_Log(L"WinEvent hook thread is exiting.");
    return 0;
}

// Function to start thread
void StartEventHookThread() {
    if (g_hWinEventHookThread == NULL) {
        g_hWinEventHookThread = CreateThread(
            NULL, 0, MyWinEventHookThread, NULL, 0, &g_dwWinEventHookThreadId);
        if (g_hWinEventHookThread) {
            Wh_Log(L"WinEvent hook thread started successfully.");
        } else {
            Wh_Log(L"Failed to start WinEvent hook thread.");
        }
    }
}

// Function to stop thread
void StopEventHookThread() {
    if (g_hWinEventHookThread != NULL) {
        // Send an exit message to the thread
        PostThreadMessage(g_dwWinEventHookThreadId, WM_APP, 0, 0);

        // Wait for thread to completely exit, wait up to 5 seconds
        WaitForSingleObject(g_hWinEventHookThread, 5000);

        // Close handle
        CloseHandle(g_hWinEventHookThread);

        g_hWinEventHookThread = NULL;
        g_dwWinEventHookThreadId = 0;
        Wh_Log(L"WinEvent hook thread stopped.");
    }
}

// Mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"My Mod Initializing...");
    StartEventHookThread();
    return TRUE;
}

// Mod unloading
void Wh_ModUninit() {
    Wh_Log(L"My Mod Uninitializing...");
    StopEventHookThread();
}
