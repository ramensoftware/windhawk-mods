// ==WindhawkMod==
// @id              task-manager-tail
// @name            Task Manager Tail
// @description     Automatically keeps Task Manager (or other apps) at the end of the taskbar. (Windows 10 & 11)
// @version         1.1
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         windhawk.exe
// @compilerOptions -luser32 -loleacc -loleaut32 -luuid -lole32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Tail 1.1

This mod ensures that **Task Manager** always stays at the tail end of your taskbar on **Windows 10 and Windows 11**.

When you open or close other applications, this mod detects the change and automatically
moves the Task Manager button to the tail end of the list.

## Features
- **Event Driven:** Uses lightweight hooks to detect window changes instantly.
- **Zero Polling:** Does not waste CPU cycles checking the taskbar constantly.
- **Configurable:** Supports non-English languages and other target applications.
- **Cross-Platform:** Works on both Windows 10 and Windows 11.

## Platform Notes

**Windows 11:** Full functionality. Responds to all window open/close events.

**Windows 10:** The target moves to the tail when new applications are opened.
Due to Windows 10 taskbar limitations, closing apps or manual dragging may not
immediately trigger a reposition - the target will return to the tail on the
next app open event. This decision avoids polling, or alternatively parsing 
many possible (continuous) user interaction event noise.
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

// Windows version enumeration
enum class WinVersion {
    Win10,
    Win11,
    Win11_24H2
};

struct UIAItem {
    IUIAutomationElement* element;
    BSTR name;
};

struct Settings {
    std::wstring targetName;
    std::wstring targetClass;
    int moveDelay;
    int debounceTime;
} g_settings;

// Global thread control
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;
volatile bool g_stopThread = false;
DWORD g_lastAttemptTime = 0;
WinVersion g_winVersion = WinVersion::Win11;

// Forward declarations
void CheckAndMoveWin10(IUIAutomation* pAutomation);

//////////////////////////////////////////////////////////////////////////////
// Windows 10: UIA Structure Changed Event Handler
// This class receives events ONLY when the taskbar structure changes
//////////////////////////////////////////////////////////////////////////////

class TaskbarStructureChangedHandler : public IUIAutomationStructureChangedEventHandler {
private:
    LONG m_refCount;
    IUIAutomation* m_pAutomation;

public:
    TaskbarStructureChangedHandler(IUIAutomation* pAutomation)
        : m_refCount(1), m_pAutomation(pAutomation) {
        if (m_pAutomation) m_pAutomation->AddRef();
    }

    ~TaskbarStructureChangedHandler() {
        if (m_pAutomation) m_pAutomation->Release();
    }

    // IUnknown methods
    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IUIAutomationStructureChangedEventHandler) {
            *ppv = static_cast<IUIAutomationStructureChangedEventHandler*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    // IUIAutomationStructureChangedEventHandler method
    HRESULT STDMETHODCALLTYPE HandleStructureChangedEvent(
        IUIAutomationElement* pSender,
        StructureChangeType changeType,
        SAFEARRAY* pRuntimeId) override {

        // Only care about children added/removed (new buttons appearing/disappearing)
        if (changeType == StructureChangeType_ChildAdded ||
            changeType == StructureChangeType_ChildRemoved ||
            changeType == StructureChangeType_ChildrenReordered) {

            Wh_Log(L"Win10: Taskbar structure changed (type=%d), triggering check", changeType);

            // Post to our thread to do the actual check (avoid blocking the event)
            if (g_dwThreadId != 0) {
                PostThreadMessage(g_dwThreadId, WM_TRIGGER_CHECK, 0, 0);
            }
        }
        return S_OK;
    }
};

// Global handler reference for cleanup
TaskbarStructureChangedHandler* g_pWin10Handler = NULL;
IUIAutomationElement* g_pWin10TaskListElement = NULL;

// Detect Windows version by checking explorer.exe file version
WinVersion DetectWindowsVersion() {
    WCHAR sysDir[MAX_PATH];
    if (!GetSystemDirectoryW(sysDir, MAX_PATH)) {
        Wh_Log(L"GetSystemDirectory failed, assuming Win11");
        return WinVersion::Win11;
    }

    WCHAR explorerPath[MAX_PATH];
    swprintf_s(explorerPath, L"%s\\..\\explorer.exe", sysDir);

    DWORD dummy;
    DWORD size = GetFileVersionInfoSizeW(explorerPath, &dummy);
    if (!size) {
        Wh_Log(L"GetFileVersionInfoSize failed, assuming Win11");
        return WinVersion::Win11;
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(explorerPath, 0, size, data.data())) {
        Wh_Log(L"GetFileVersionInfo failed, assuming Win11");
        return WinVersion::Win11;
    }

    VS_FIXEDFILEINFO* info = NULL;
    UINT len = 0;
    if (!VerQueryValueW(data.data(), L"\\", (void**)&info, &len) || !info) {
        Wh_Log(L"VerQueryValue failed, assuming Win11");
        return WinVersion::Win11;
    }

    WORD major = HIWORD(info->dwFileVersionMS);
    WORD minor = LOWORD(info->dwFileVersionMS);
    WORD build = HIWORD(info->dwFileVersionLS);

    Wh_Log(L"Detected Windows version: %d.%d.%d", major, minor, build);

    if (build < 22000) {
        Wh_Log(L"Running on Windows 10");
        return WinVersion::Win10;
    } else if (build < 26100) {
        Wh_Log(L"Running on Windows 11");
        return WinVersion::Win11;
    } else {
        Wh_Log(L"Running on Windows 11 24H2+");
        return WinVersion::Win11_24H2;
    }
}

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
}

// --- WinEvent Hook Callback (used for Win11) ---
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
                           LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF && g_dwThreadId != 0) {
        PostThreadMessage(g_dwThreadId, WM_TRIGGER_CHECK, 0, 0);
    }
}

void CycleTaskbarTab(HWND hwnd) {
    Wh_Log(L"Cycling Taskbar Tab for HWND %p via ITaskbarList", hwnd);

    ITaskbarList* pTaskbarList = NULL;
    HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList, (void**)&pTaskbarList);

    if (SUCCEEDED(hr) && pTaskbarList) {
        hr = pTaskbarList->HrInit();
        if (SUCCEEDED(hr)) {
            pTaskbarList->DeleteTab(hwnd);
            Sleep(200);
            pTaskbarList->AddTab(hwnd);
            Sleep(50);
            pTaskbarList->ActivateTab(hwnd);
        }
        pTaskbarList->Release();
    }
}

// Find the MSTaskListWClass element for Windows 10
IUIAutomationElement* FindWin10TaskListElement(IUIAutomation* pAutomation) {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return NULL;

    HWND hRebar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (!hRebar) return NULL;

    HWND hTaskSw = FindWindowExW(hRebar, NULL, L"MSTaskSwWClass", NULL);
    if (!hTaskSw) return NULL;

    HWND hTaskList = FindWindowExW(hTaskSw, NULL, L"MSTaskListWClass", NULL);
    if (!hTaskList) return NULL;

    IUIAutomationElement* pElement = NULL;
    pAutomation->ElementFromHandle(hTaskList, &pElement);
    return pElement;
}

// Windows 10 specific taskbar check
void CheckAndMoveWin10(IUIAutomation* pAutomation) {
    HWND hTargetWnd = FindWindowW(g_settings.targetClass.c_str(), NULL);
    if (!hTargetWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return;

    HWND hRebar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (!hRebar) return;

    HWND hTaskSw = FindWindowExW(hRebar, NULL, L"MSTaskSwWClass", NULL);
    if (!hTaskSw) return;

    HWND hTaskList = FindWindowExW(hTaskSw, NULL, L"MSTaskListWClass", NULL);
    if (!hTaskList) return;

    IUIAutomationElement* pRoot = NULL;
    HRESULT hr = pAutomation->ElementFromHandle(hTaskList, &pRoot);
    if (FAILED(hr) || !pRoot) return;

    IUIAutomationCondition* pTrueCondition = NULL;
    pAutomation->CreateTrueCondition(&pTrueCondition);

    IUIAutomationElementArray* pChildren = NULL;
    pRoot->FindAll(TreeScope_Children, pTrueCondition, &pChildren);

    if (pChildren) {
        int count = 0;
        pChildren->get_Length(&count);

        std::vector<UIAItem> buttons;
        int targetIndex = -1;

        for (int i = 0; i < count; i++) {
            IUIAutomationElement* pChild = NULL;
            pChildren->GetElement(i, &pChild);
            if (pChild) {
                CONTROLTYPEID controlType;
                pChild->get_CurrentControlType(&controlType);

                if (controlType == UIA_ButtonControlTypeId) {
                    UIAItem item;
                    item.element = pChild;
                    pChild->AddRef();
                    pChild->get_CurrentName(&item.name);

                    buttons.push_back(item);

                    if (item.name && wcsstr(item.name, g_settings.targetName.c_str())) {
                        targetIndex = (int)buttons.size() - 1;
                    }
                }
                pChild->Release();
            }
        }

        // Only act if found and NOT already at the end
        if (targetIndex != -1 && static_cast<size_t>(targetIndex) < buttons.size() - 1) {
            if (GetTickCount() - g_lastAttemptTime > 1000) {
                Wh_Log(L"Win10: Target at index %d (of %d). Moving to end...", targetIndex, (int)buttons.size());
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

    if (pTrueCondition) pTrueCondition->Release();
    pRoot->Release();
}

// Windows 11 specific taskbar check (original implementation)
void CheckAndMoveWin11(IUIAutomation* pAutomation) {
    HWND hTargetWnd = FindWindowW(g_settings.targetClass.c_str(), NULL);
    if (!hTargetWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return;

    HWND hChild = GetWindow(hTray, GW_CHILD);
    while (hChild) {
        wchar_t text[256] = {0};
        GetWindowTextW(hChild, text, 255);

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

                                if (item.name && wcsstr(item.name, g_settings.targetName.c_str())) {
                                    targetIndex = (int)buttons.size() - 1;
                                }
                            }
                            if (cls) SysFreeString(cls);
                            pChild->Release();
                        }
                    }

                    if (targetIndex != -1 && static_cast<size_t>(targetIndex) < buttons.size() - 1) {
                        if (GetTickCount() - g_lastAttemptTime > 1000) {
                            Wh_Log(L"Target found at index %d (of %d). Moving...", targetIndex, (int)buttons.size());
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

void CheckAndMove(IUIAutomation* pAutomation) {
    if (g_winVersion == WinVersion::Win10) {
        CheckAndMoveWin10(pAutomation);
    } else {
        CheckAndMoveWin11(pAutomation);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Background Thread - Different event sources for Win10 vs Win11
//////////////////////////////////////////////////////////////////////////////

DWORD WINAPI BackgroundThread(LPVOID) {
    Wh_Log(L"Task Manager Tail Thread Started");
    g_dwThreadId = GetCurrentThreadId();

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) return 1;

    IUIAutomation* pAutomation = NULL;
    hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pAutomation);

    if (FAILED(hr) || !pAutomation) {
        Wh_Log(L"Failed to create UI Automation instance");
        CoUninitialize();
        return 1;
    }

    HWINEVENTHOOK hWinEventHook = NULL;

    if (g_winVersion == WinVersion::Win10) {
        // Windows 10: Use UIA StructureChangedEventHandler on the taskbar
        Wh_Log(L"Win10: Setting up UIA StructureChangedEventHandler...");

        g_pWin10TaskListElement = FindWin10TaskListElement(pAutomation);
        if (g_pWin10TaskListElement) {
            g_pWin10Handler = new TaskbarStructureChangedHandler(pAutomation);

            hr = pAutomation->AddStructureChangedEventHandler(
                g_pWin10TaskListElement,
                TreeScope_Subtree,
                NULL,  // No cache request
                g_pWin10Handler
            );

            if (SUCCEEDED(hr)) {
                Wh_Log(L"Win10: UIA StructureChangedEventHandler registered successfully!");
            } else {
                Wh_Log(L"Win10: Failed to register UIA handler (hr=0x%08X), falling back to WinEventHook", hr);
                // Fall back to WinEventHook
                g_pWin10Handler->Release();
                g_pWin10Handler = NULL;
                g_pWin10TaskListElement->Release();
                g_pWin10TaskListElement = NULL;

                hWinEventHook = SetWinEventHook(
                    EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE,
                    NULL, WinEventProc, 0, 0,
                    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
                );
            }
        } else {
            Wh_Log(L"Win10: Could not find taskbar element, falling back to WinEventHook");
            hWinEventHook = SetWinEventHook(
                EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE,
                NULL, WinEventProc, 0, 0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );
        }
    } else {
        // Windows 11: Use WinEventHook (works well there)
        hWinEventHook = SetWinEventHook(
            EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE,
            NULL, WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        );

        if (hWinEventHook) {
            Wh_Log(L"Win11: WinEventHook Registered. Waiting for events...");
        }
    }

    // Message loop with debouncing
    MSG msg;
    UINT_PTR debounceTimer = 0;

    // Do an initial check
    CheckAndMove(pAutomation);

    while (!g_stopThread && GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_TRIGGER_CHECK) {
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

    // Cleanup
    if (hWinEventHook) {
        UnhookWinEvent(hWinEventHook);
    }

    if (g_pWin10Handler) {
        pAutomation->RemoveStructureChangedEventHandler(g_pWin10TaskListElement, g_pWin10Handler);
        g_pWin10Handler->Release();
        g_pWin10Handler = NULL;
    }

    if (g_pWin10TaskListElement) {
        g_pWin10TaskListElement->Release();
        g_pWin10TaskListElement = NULL;
    }

    pAutomation->Release();
    CoUninitialize();
    return 0;
}

bool WhTool_ModInit() {
    g_winVersion = DetectWindowsVersion();
    LoadSettings();
    g_stopThread = false;
    g_hThread = CreateThread(NULL, 0, BackgroundThread, NULL, 0, NULL);
    return g_hThread != NULL;
}

void WhTool_ModUninit() {
    g_stopThread = true;
    if (g_dwThreadId) PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}


////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation
////////////////////////////////////////////////////////////////////////////////

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
