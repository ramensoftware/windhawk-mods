// ==WindhawkMod==
// @id              gnome-dynamic-desktops
// @name            GNOME-like dynamic virtual desktops
// @description     Makes the Windows 11 virtual desktop system behave like the GNOME desktop environment
// @version         1.2
// @author          Giggig
// @github          https://github.com/Giggigx
// @include         windhawk.exe
// @compilerOptions -lole32 -luuid -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# GNOME-Like dynamic virtual desktops

Makes Windows' virtual desktops behave like the GNOME desktop environment. 

### What is "GNOME behavior"?
In the GNOME desktop environment (popular on Linux), virtual workspaces are managed dynamically:
* There is always exactly *one* empty desktop at the end of your workspace list.
* If you move or open a window in that last empty desktop, a new empty desktop is immediately created to its right.
* If a desktop in the middle of your workspaces becomes completely empty, it is automatically deleted, shifting the others over.

This mod implements this exact logic on Windows 11. It's perfect for laptops with trackpad gestures, allowing for endless, seamless switching between workspaces without ever having to manually create or close a desktop again.

![GNOME mod demo](https://i.imgur.com/wih0nm0.gif)

Works with Windows 11 build 26100+

**Settings**

* **Start with two empty desktops**: When no windows are open in any desktop you can choose to start with a single virtual desktop open or start with two virtual desktops like GNOME does.
* **Don't delete empty desktop immediately**: If you're in an empty virtual desktop it waits for you to switch into another virtual desktop before deleting it. If disabled the desktop will be deleted as soon as it becomes empty.

Special thanks to [VD.ahk](https://github.com/FuPeiJiang/VD.ahk) for providing the correct GUIDs to interface with the virtual desktops system.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- twoInitialDesktops: false
  $name: Start with two empty desktops
  $description: Start with two empty desktops instead of one, more akin to GNOME's behavior.
- delayedDeletion: true
  $name: Don't delete empty desktop immediately
  $description: When a desktop gets empty, it waits for you to switch desktop before deleting it.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <shobjidl.h>
#include <ObjectArray.h>
#include <dwmapi.h>
#include <vector>
#include <algorithm>

// -------------------------------------------------------------------------
// GUID  (Windows 11 26100+)
// -------------------------------------------------------------------------
const GUID CLSID_ImmersiveShell = { 0xc2f03a33, 0x21f5, 0x47fa, { 0xb4, 0xbb, 0x15, 0x63, 0x62, 0xa2, 0xf2, 0x39 } };
const GUID CLSID_VirtualDesktopManagerInternal = { 0xc5e0cdca, 0x7b6e, 0x41b2, { 0x9f, 0xc4, 0xd9, 0x39, 0x75, 0xcc, 0x46, 0x7b } };
const GUID IID_IVirtualDesktop = { 0x3f07f4be, 0xb107, 0x441a, { 0xaf, 0x0f, 0x39, 0xd8, 0x25, 0x29, 0x07, 0x2c } };
const GUID IID_IVirtualDesktopManagerInternal = { 0x53f5ca0b, 0x158f, 0x4124, { 0x90, 0x0c, 0x05, 0x71, 0x58, 0x06, 0x0b, 0x27 } };

DECLARE_INTERFACE_IID_(IVirtualDesktop, IUnknown, "3f07f4be-b107-441a-af0f-39d82529072c") {
    STDMETHOD(Dummy3)() PURE;
    STDMETHOD(GetId)(GUID* pGuid) PURE;
};

DECLARE_INTERFACE_IID_(IVirtualDesktopManagerInternal, IUnknown, "53f5ca0b-158f-4124-900c-057158060b27") {
    STDMETHOD(Dummy3)() PURE;
    STDMETHOD(MoveViewToDesktop)(IUnknown* pView, IVirtualDesktop* pDesktop) PURE;
    STDMETHOD(Dummy5)() PURE;
    STDMETHOD(GetCurrentDesktop)(IVirtualDesktop** desktop) PURE;
    STDMETHOD(GetDesktops)(IObjectArray** desktops) PURE;
    STDMETHOD(Dummy8)() PURE;
    STDMETHOD(SwitchDesktop)(IVirtualDesktop* desktop) PURE;
    STDMETHOD(Dummy10)() PURE;
    STDMETHOD(CreateDesktop)(IVirtualDesktop** newDesktop) PURE;
    STDMETHOD(Dummy12)() PURE;
    STDMETHOD(RemoveDesktop)(IVirtualDesktop* desktop, IVirtualDesktop* fallback) PURE;
};

// Global Variables
struct ModSettings {
    int initialDesktops;
    bool delayedDeletion;
} g_settings;

IVirtualDesktopManagerInternal* g_pDesktopManager = nullptr;
IVirtualDesktopManager* g_pPublicDesktopManager = nullptr;

DWORD g_threadId = 0;
HANDLE g_hThread = nullptr;
HWINEVENTHOOK g_hHookFg = nullptr;
HWINEVENTHOOK g_hHookShowHide = nullptr;
UINT_PTR g_debounceTimer = 0;
GUID g_lastActiveDesktop = {0};

void LoadSettings() {
    bool useTwo = Wh_GetIntSetting(L"twoInitialDesktops") != 0;
    g_settings.initialDesktops = useTwo ? 2 : 1; 
    g_settings.delayedDeletion = Wh_GetIntSetting(L"delayedDeletion") != 0;
    
    Wh_Log(L"twoInitialDesktops: %d, delayedDeletion: %d", g_settings.initialDesktops, g_settings.delayedDeletion);
}

void CleanupCOM() {
    if (g_pDesktopManager) { g_pDesktopManager->Release(); g_pDesktopManager = nullptr; }
    if (g_pPublicDesktopManager) { g_pPublicDesktopManager->Release(); g_pPublicDesktopManager = nullptr; }
}

bool InitializeCOM() {
    if (g_pDesktopManager && g_pPublicDesktopManager) return true;
    
    CleanupCOM();

    IServiceProvider* pServiceProvider = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IServiceProvider), (void**)&pServiceProvider);
    if (SUCCEEDED(hr)) {
        pServiceProvider->QueryService(CLSID_VirtualDesktopManagerInternal, IID_IVirtualDesktopManagerInternal, (void**)&g_pDesktopManager);
        pServiceProvider->Release();
    }
    CoCreateInstance(__uuidof(VirtualDesktopManager), NULL, CLSCTX_INPROC_SERVER, __uuidof(IVirtualDesktopManager), (void**)&g_pPublicDesktopManager);
    
    if (!g_pDesktopManager || !g_pPublicDesktopManager) {
        CleanupCOM();
        return false;
    }
    return true;
}

bool IsValidAppWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) return false;
    
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (GetWindow(hwnd, GW_OWNER) != NULL) return false;

    if (hwnd == GetShellWindow()) return false;

    WCHAR className[64];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) {
            return false;
        }
    }

    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        // 1 = DWM_CLOAKED_APP (phantom UWP background apps)
        if (cloaked & 1) return false;
    }

    return true;
}

struct EnumCtx {
    std::vector<GUID> windowDesktops;
    int candidates = 0;
};

BOOL CALLBACK CollectWindowDesktops(HWND hwnd, LPARAM lParam) {
    auto* ctx = (EnumCtx*)lParam;
    if (IsValidAppWindow(hwnd)) {
        ctx->candidates++;
        GUID id{};
        if (SUCCEEDED(g_pPublicDesktopManager->GetWindowDesktopId(hwnd, &id))) {
            ctx->windowDesktops.push_back(id);
        }
    }
    return TRUE;
}

struct ReentrancyGuard {
    bool& flag;
    ReentrancyGuard(bool& f) : flag(f) { flag = true; }
    ~ReentrancyGuard() { flag = false; }
};

// Core Logic
void EvaluateDesktops() {
    static bool g_evaluating = false;
    if (g_evaluating) return;
    ReentrancyGuard guard(g_evaluating);

    if (!InitializeCOM()) return;

    IObjectArray* pDesktops = nullptr;
    HRESULT hr = g_pDesktopManager->GetDesktops(&pDesktops);
    
    if (FAILED(hr)) {
        Wh_Log(L"COM call failed (0x%08X). Explorer might have restarted. Reinitializing and skipping this cycle...", hr);
        CleanupCOM();
        InitializeCOM();
        return;
    }

    if (!pDesktops) return;

    UINT totalDesktops = 0;
    pDesktops->GetCount(&totalDesktops);
    if (totalDesktops == 0) {
        pDesktops->Release();
        return;
    }

    IVirtualDesktop* pCurrentDesktop = nullptr;
    g_pDesktopManager->GetCurrentDesktop(&pCurrentDesktop);
    GUID currentDesktopId = {0};
    if (pCurrentDesktop) {
        pCurrentDesktop->GetId(&currentDesktopId);
        pCurrentDesktop->Release();
    }

    bool triggeredBySwitch = false;
    if (memcmp(&currentDesktopId, &g_lastActiveDesktop, sizeof(GUID)) != 0) {
        triggeredBySwitch = true;
        g_lastActiveDesktop = currentDesktopId;
        Wh_Log(L"Desktop switched!");
    }

    EnumCtx ctx;
    EnumWindows(CollectWindowDesktops, (LPARAM)&ctx);

    if (ctx.candidates > 0 && ctx.windowDesktops.empty()) {
        Wh_Log(L"Could not map any window to a desktop, skipping evaluation to prevent data loss.");
        pDesktops->Release();
        return;
    }

    std::vector<IVirtualDesktop*> desktops;
    std::vector<GUID> desktopIds;
    std::vector<int> windowCounts;

    for (UINT i = 0; i < totalDesktops; ++i) {
        IVirtualDesktop* pDesktop = nullptr;
        if (SUCCEEDED(pDesktops->GetAt(i, IID_IVirtualDesktop, (void**)&pDesktop)) && pDesktop) {
            GUID id = {0};
            if (SUCCEEDED(pDesktop->GetId(&id))) {
                desktops.push_back(pDesktop);
                desktopIds.push_back(id);
                
                int count = (int)std::count_if(ctx.windowDesktops.begin(), ctx.windowDesktops.end(), 
                    [&id](const GUID& winId) { return memcmp(&winId, &id, sizeof(GUID)) == 0; });
                windowCounts.push_back(count);
            } else {
                pDesktop->Release();
            }
        }
    }
    pDesktops->Release();

    totalDesktops = (UINT)desktops.size();
    if (totalDesktops == 0) {
        return;
    }

    int lastPopulatedIndex = -1;
    for (int i = totalDesktops - 1; i >= 0; --i) {
        if (windowCounts[i] > 0) {
            lastPopulatedIndex = i;
            break;
        }
    }

    // RULE 1
    if (lastPopulatedIndex == -1) {
        Wh_Log(L"No populated desktops.");
        int i = totalDesktops - 1;
        while (totalDesktops > (UINT)g_settings.initialDesktops) {
            bool isCurrent = (memcmp(&desktopIds[i], &currentDesktopId, sizeof(GUID)) == 0);
            if (g_settings.delayedDeletion && isCurrent) {
                Wh_Log(L"Rule 1: Skipping deletion of in-use desktop.");
                break;
            }
            Wh_Log(L"Rule 1: Destroying excess desktop (remaining: %u).", totalDesktops - 1);
            g_pDesktopManager->RemoveDesktop(desktops[i], desktops[i > 0 ? i - 1 : 0]);
            desktops[i]->Release(); 
            totalDesktops--;
            desktops.pop_back();
            desktopIds.pop_back();
            i--;
        }
        while (totalDesktops < (UINT)g_settings.initialDesktops) {
            Wh_Log(L"Creating missing base desktop.");
            IVirtualDesktop* newDesktop = nullptr;
            g_pDesktopManager->CreateDesktop(&newDesktop);
            if (newDesktop) newDesktop->Release();
            totalDesktops++;
        }
        goto Cleanup;
    }

    // RULE 2
    {
        int desiredTotal = lastPopulatedIndex + 2;
        if (totalDesktops < (UINT)desiredTotal) {
            Wh_Log(L"Creating a new empty desktop on the right.");
            IVirtualDesktop* newDesktop = nullptr;
            g_pDesktopManager->CreateDesktop(&newDesktop);
            if (newDesktop) newDesktop->Release();
        } else if (totalDesktops > (UINT)desiredTotal) {
            for (int i = totalDesktops - 1; i >= desiredTotal; --i) {
                bool isCurrent = (memcmp(&desktopIds[i], &currentDesktopId, sizeof(GUID)) == 0);
                if (g_settings.delayedDeletion && isCurrent) {
                    Wh_Log(L"Skipping deletion of in-use desktop (Still on desktop).");
                    break; 
                }
                Wh_Log(L"Destroying excess empty desktop at the end.");
                g_pDesktopManager->RemoveDesktop(desktops[i], desktops[i > 0 ? i - 1 : 0]);
            }
        }
    }

    // RULE 3
    for (int i = 0; i <= lastPopulatedIndex; ++i) {
        if (windowCounts[i] == 0) {
            bool isCurrent = (memcmp(&desktopIds[i], &currentDesktopId, sizeof(GUID)) == 0);
            if (g_settings.delayedDeletion) {
                if (triggeredBySwitch && !isCurrent) {
                    Wh_Log(L"Destroying empty desktop in the middle (Switched to other desktop).");
                    g_pDesktopManager->RemoveDesktop(desktops[i], desktops[i > 0 ? i - 1 : i + 1]);
                }
            } else {
                if (!isCurrent) {
                    Wh_Log(L"Destroying empty desktop in the middle (Immediate).");
                    g_pDesktopManager->RemoveDesktop(desktops[i], desktops[i > 0 ? i - 1 : i + 1]);
                }
            }
        }
    }

Cleanup:
    for (auto* p : desktops) {
        p->Release();
    }
}

// Events
VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(NULL, idEvent);
    g_debounceTimer = 0;
    EvaluateDesktops();
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW || hwnd == NULL) return;
    
    if (g_debounceTimer) KillTimer(NULL, g_debounceTimer);
    g_debounceTimer = SetTimer(NULL, 1, 150, TimerProc);
}

DWORD WINAPI BackgroundEventThread(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    g_hHookFg = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hHookShowHide = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    Wh_Log(L"Listening background thread started");
    
    EvaluateDesktops();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"Closing background thread");

    if (g_hHookFg) UnhookWinEvent(g_hHookFg);
    if (g_hHookShowHide) UnhookWinEvent(g_hHookShowHide);

    CleanupCOM();
    CoUninitialize();
    return 0;
}

// -------------------------------------------------------------------------
// Tool Lifecycle
// -------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    Wh_Log(L"=== INITIALIZATION ===");
    LoadSettings();
    
    g_hThread = CreateThread(NULL, 0, BackgroundEventThread, NULL, 0, &g_threadId);
    return g_hThread != nullptr;
}

void WhTool_ModUninit() {
    Wh_Log(L"=== UNLOADING ===");
    if (g_threadId && g_hThread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
        g_threadId = 0;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"=== SETTING CHANGED ===");
    LoadSettings();
    EvaluateDesktops();
}

// -------------------------------------------------------------------------
// Launcher Boilerplate
// -------------------------------------------------------------------------

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
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

    if (isExcluded) {
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
