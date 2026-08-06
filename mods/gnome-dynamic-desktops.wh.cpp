// ==WindhawkMod==
// @id              gnome-dynamic-desktops
// @name            GNOME-like dynamic virtual desktops
// @description     Makes the Windows 11 virtual desktop system behave like the GNOME desktop environment
// @version         1.0
// @author          Giggig
// @github          https://github.com/Giggigx
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# GNOME-Like dynamic virtual desktops

Makes Windows' virtual desktops behave like the GNOME desktop environment. Perfect for laptops with trackpad gestures for easy switching between virtual desktops.

Works with Windows 11 build 26100+

**Settings**

* **Start with two empty desktops**: When no windows are open in any desktop you can choose to start with a single virtual desktop open or start with two virtual desktops like GNOME does.

* **Don't delete empty desktop immediately**: if you're in an empty virtual desktop it waits for you to switch into another virtual desktop before deleting it. If disabled the desktop will be deleted as soon as it becomes empty.


Special thanks to  [VD.ahk](https://github.com/FuPeiJiang/VD.ahk) for providing the correct GUIDs to interface with the virtual desktops system.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- two_initial_desktops: false
  $name: Start with two empty desktops
  $description: Start with two empty desktops instead of one, more akin to GNOME's behavior.
- delayed_deletion: true
  $name: Don't delete empty desktop immediately
  $description: When a desktop gets empty, it waits for you to switch desktop before deleting it.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <shobjidl.h>
#include <ObjectArray.h>
#include <vector>
#include <atomic>

// -------------------------------------------------------------------------
// GUID  (Windows 11 25H2)
// -------------------------------------------------------------------------
const GUID IID_IVirtualDesktop = { 0x3f07f4be, 0xb107, 0x441a, { 0xaf, 0x0f, 0x39, 0xd8, 0x25, 0x29, 0x07, 0x2c } };
const GUID IID_IVirtualDesktopManagerInternal = { 0x53f5ca0b, 0x158f, 0x4124, { 0x90, 0x0c, 0x05, 0x71, 0x58, 0x06, 0x0b, 0x27 } };
const GUID CLSID_ImmersiveShell = { 0xc2f03a33, 0x21f5, 0x47fa, { 0xb4, 0xbb, 0x15, 0x63, 0x62, 0xa2, 0xf2, 0x39 } };
const GUID CLSID_VirtualDesktopManagerInternal = { 0xc5e0cdca, 0x7b6e, 0x41b2, { 0x9f, 0xc4, 0xd9, 0x39, 0x75, 0xcc, 0x46, 0x7b } };

DECLARE_INTERFACE_IID_(IVirtualDesktop, IUnknown, "3f07f4be-b107-441a-af0f-39d82529072c") {
    STDMETHOD(Dummy3)() PURE;
    STDMETHOD(GetId)(GUID* pGuid) PURE;
};

DECLARE_INTERFACE_IID_(IVirtualDesktopManagerInternal, IUnknown, "53f5ca0b-158f-4124-900c-057158060b27") {
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG, AddRef)() PURE;
    STDMETHOD_(ULONG, Release)() PURE;
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
    int initial_desktops;
    bool delayed_deletion;
} g_settings;

IVirtualDesktopManagerInternal* g_pDesktopManager = nullptr;
IVirtualDesktopManager* g_pPublicDesktopManager = nullptr;

DWORD g_threadId = 0;
HWINEVENTHOOK g_hHookFg = nullptr;
HWINEVENTHOOK g_hHookCreate = nullptr;
HWINEVENTHOOK g_hHookDestroy = nullptr;
UINT_PTR g_debounceTimer = 0;
GUID g_lastActiveDesktop = {0};

void LoadSettings() {
    // Legge il valore del toggle: true (1) o false (0)
    bool use_two = Wh_GetIntSetting(L"two_initial_desktops") != 0;
    
    // Se è acceso imposta 2 desktop, altrimenti 1
    g_settings.initial_desktops = use_two ? 2 : 1; 
    
    g_settings.delayed_deletion = Wh_GetIntSetting(L"delayed_deletion") != 0;
    
    Wh_Log(L"[Config] initial_desktops: %d, delayed_deletion: %d", g_settings.initial_desktops, g_settings.delayed_deletion);
}

bool InitializeCOM() {
    if (g_pDesktopManager && g_pPublicDesktopManager) return true;
    
    IServiceProvider* pServiceProvider = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IServiceProvider), (void**)&pServiceProvider);
    if (SUCCEEDED(hr)) {
        pServiceProvider->QueryService(CLSID_VirtualDesktopManagerInternal, IID_IVirtualDesktopManagerInternal, (void**)&g_pDesktopManager);
        pServiceProvider->Release();
    }
    CoCreateInstance(__uuidof(VirtualDesktopManager), NULL, CLSCTX_INPROC_SERVER, __uuidof(IVirtualDesktopManager), (void**)&g_pPublicDesktopManager);
    
    return (g_pDesktopManager != nullptr && g_pPublicDesktopManager != nullptr);
}

int CountWindowsInDesktop(GUID desktopId) {
    if (!g_pPublicDesktopManager) return 0;
    int count = 0;
    HWND hwnd = GetTopWindow(NULL);
    while (hwnd) {
        if (IsWindowVisible(hwnd)) {
            GUID winDesktopId = {0};
            if (SUCCEEDED(g_pPublicDesktopManager->GetWindowDesktopId(hwnd, &winDesktopId))) {
                if (memcmp(&winDesktopId, &desktopId, sizeof(GUID)) == 0) {
                    count++;
                }
            }
        }
        hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
    }
    return count;
}


// Core Logic
void EvaluateDesktops() {
    if (!InitializeCOM()) return;

    IObjectArray* pDesktops = nullptr;
    if (FAILED(g_pDesktopManager->GetDesktops(&pDesktops)) || !pDesktops) return;

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

    // Desktop switch trigger
    bool triggeredBySwitch = false;
    if (memcmp(&currentDesktopId, &g_lastActiveDesktop, sizeof(GUID)) != 0) {
        triggeredBySwitch = true;
        g_lastActiveDesktop = currentDesktopId;
        Wh_Log(L"[Logic] Desktop switched!");
    }

    std::vector<IVirtualDesktop*> desktops;
    std::vector<GUID> desktopIds;
    std::vector<int> windowCounts;

    for (UINT i = 0; i < totalDesktops; ++i) {
        IVirtualDesktop* pDesktop = nullptr;
        pDesktops->GetAt(i, IID_IVirtualDesktop, (void**)&pDesktop);
        if (pDesktop) {
            GUID id = {0};
            pDesktop->GetId(&id);
            desktops.push_back(pDesktop);
            desktopIds.push_back(id);
            windowCounts.push_back(CountWindowsInDesktop(id));
        }
    }
    pDesktops->Release();

    int lastPopulatedIndex = -1;
    for (int i = totalDesktops - 1; i >= 0; --i) {
        if (windowCounts[i] > 0) {
            lastPopulatedIndex = i;
            break;
        }
    }

if (lastPopulatedIndex == -1) {
        Wh_Log(L"[Logic] No populated desktops.");
        while (totalDesktops > g_settings.initial_desktops) {
            Wh_Log(L"[Logic] Rule 1: Destroying excess desktop (remaining: %u).", totalDesktops - 1);
            g_pDesktopManager->RemoveDesktop(desktops.back(), desktops.front());
            totalDesktops--;
            desktops.pop_back();
        }
        while (totalDesktops < g_settings.initial_desktops) {
            Wh_Log(L"[Logic] Creating missing base desktop.");
            IVirtualDesktop* newDesktop = nullptr;
            g_pDesktopManager->CreateDesktop(&newDesktop);
            if (newDesktop) newDesktop->Release();
            totalDesktops++;
        }
        goto Cleanup;
    }

    {
        int desiredTotal = lastPopulatedIndex + 2;
        if (totalDesktops < desiredTotal) {
            Wh_Log(L"[Logic] Creating a new empty desktop on the right.");
            IVirtualDesktop* newDesktop = nullptr;
            g_pDesktopManager->CreateDesktop(&newDesktop);
            if (newDesktop) newDesktop->Release();
        } else if (totalDesktops > desiredTotal) {
            for (int i = totalDesktops - 1; i >= desiredTotal; --i) {
                if (g_settings.delayed_deletion && memcmp(&desktopIds[i], &currentDesktopId, sizeof(GUID)) == 0 && !triggeredBySwitch) {
                    Wh_Log(L"[Logic] Skipping deletion of in-use desktop (Still on desktop).");
                    continue; 
                }
                Wh_Log(L"[Logic] Destroying excess empty desktop at the end.");
                g_pDesktopManager->RemoveDesktop(desktops[i], desktops[i-1]);
            }
        }
    }

    for (int i = 0; i <= lastPopulatedIndex; ++i) {
        if (windowCounts[i] == 0) {
            bool isCurrent = (memcmp(&desktopIds[i], &currentDesktopId, sizeof(GUID)) == 0);
            if (g_settings.delayed_deletion) {
                if (triggeredBySwitch && !isCurrent) {
                    Wh_Log(L"[Logic] Destroying empty desktop in the middle (Switched to other desktop).");
                    g_pDesktopManager->RemoveDesktop(desktops[i], desktops[i > 0 ? i - 1 : i + 1]);
                }
            } else {
                if (!isCurrent) {
                    Wh_Log(L"[Logic] Destroying empty desktop in the middle (Immediate).");
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
    
    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_USER + 1, 0, 0);
    }
}

DWORD WINAPI BackgroundEventThread(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    g_hHookFg = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hHookCreate = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hHookDestroy = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    Wh_Log(L"[Thread] listening background thread");
    
    // First boot desktop check
    EvaluateDesktops();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_USER + 1) {
            // Debouncer
            if (g_debounceTimer) KillTimer(NULL, g_debounceTimer);
            g_debounceTimer = SetTimer(NULL, 1, 150, TimerProc);
        } else if (msg.message == WM_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"[Thread] closing background thread");

    if (g_hHookFg) UnhookWinEvent(g_hHookFg);
    if (g_hHookCreate) UnhookWinEvent(g_hHookCreate);
    if (g_hHookDestroy) UnhookWinEvent(g_hHookDestroy);

    if (g_pDesktopManager) { g_pDesktopManager->Release(); g_pDesktopManager = nullptr; }
    if (g_pPublicDesktopManager) { g_pPublicDesktopManager->Release(); g_pPublicDesktopManager = nullptr; }

    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"=== INITIALIZATION ===");
    LoadSettings();
    
    CreateThread(NULL, 0, BackgroundEventThread, NULL, 0, &g_threadId);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"=== UNLOADING ===");
    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"=== SETTING CHANGED ===");
    LoadSettings();
    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_USER + 1, 0, 0);
    }
}