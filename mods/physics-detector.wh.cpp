// ==WindhawkMod==
// @id              physics-detector
// @name            Physics for Panels - Detector
// @description     Part 1 of 2: Detects taskbar auto-hide states and overlays it like a search menu when hovered.
// @version         1.0.0
// @author          Zicronium
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lkernel32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Physics for Panels — Detector

This is **Mod 1 of 2** in the Physics for Panels suite.

It intercepts taskbar hiding synchronously. If the mouse is over the taskbar,
or the custom Cryonix Dynamic Island canvas, it acts like the Search Menu—forcing 
the window to stay rendered on top of applications without causing visual layout stutter.

## Known Limitations
- Note: This specific release baseline handles standard cursor interactions cleanly, 
  but does not natively suppress or translate layouts when opening flyouts directly via 
  Win+A or Win+N hotkeys. This remains an area for future architectural investigation.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#define PHYSICS_SHMEM_NAME L"Local\\PhysicsForPanels_Signal"
#define PHYSICS_SHMEM_SIZE sizeof(PhysicsSignal)

struct PhysicsSignal {
    volatile LONG  version;       
    volatile BOOL  taskbarHiding; 
};

static HANDLE         g_hMapFile = nullptr;
static PhysicsSignal* g_pSignal  = nullptr;
static volatile BOOL  g_isOverridingState = FALSE;

static bool OpenOrCreateSharedMemory() {
    g_hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, PHYSICS_SHMEM_NAME);
    if (!g_hMapFile) {
        g_hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, PHYSICS_SHMEM_SIZE, PHYSICS_SHMEM_NAME);
    }
    if (!g_hMapFile) return false;
    g_pSignal = (PhysicsSignal*)MapViewOfFile(g_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, PHYSICS_SHMEM_SIZE);
    return (g_pSignal != nullptr);
}

static void CloseSharedMemory() {
    if (g_pSignal)  { UnmapViewOfFile(g_pSignal); g_pSignal = nullptr; }
    if (g_hMapFile) { CloseHandle(g_hMapFile);    g_hMapFile = nullptr; }
}

static DWORD g_shellHostPid = 0;

static void FindShellHostPid() {
    struct Ctx { DWORD pid; } ctx = { 0 };
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        auto* ctx = reinterpret_cast<Ctx*>(lParam);
        WCHAR name[MAX_PATH];
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProc) {
            DWORD len = ARRAYSIZE(name);
            if (QueryFullProcessImageNameW(hProc, 0, name, &len)) {
                WCHAR* slash = wcsrchr(name, L'\\');
                WCHAR* fname = slash ? slash + 1 : name;
                if (_wcsicmp(fname, L"ShellHost.exe") == 0) {
                    ctx->pid = pid;
                    CloseHandle(hProc);
                    return FALSE;
                }
            }
            CloseHandle(hProc);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));
    g_shellHostPid = ctx.pid;
}

static bool IsMouseOverShellPanel() {
    POINT pt;
    if (!GetCursorPos(&pt)) return false;

    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return false;
    hWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hWnd) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);

    if (g_shellHostPid && pid == g_shellHostPid) return true;

    WCHAR cls[64]; 
    if (GetClassName(hWnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||           
            wcscmp(cls, L"ControlCenterWindow") == 0 ||      
            wcscmp(cls, L"NotifyIconOverflowWindow") == 0 || 
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||   
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 || 
            wcscmp(cls, L"CryonixDynamicIslandWnd") == 0)    
        {
            return true;
        }
    }
    return false;
}

static DWORD WINAPI MenuOverlayThread(LPVOID) {
    g_isOverridingState = TRUE;
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    
    while (IsMouseOverShellPanel()) {
        if (hTaskbar) {
            // Repositions z-order accurately via standard HWND topmost token parameters
            SetWindowPos(hTaskbar, HWND_TOPMOST, 0, 0, 0, 0, 
                         SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
        }
        Sleep(50); 
    }
    
    g_isOverridingState = FALSE;
    return 0;
}

static DWORD WINAPI SignalThread(LPVOID) {
    Sleep(500); 
    if (!g_pSignal) return 0;
    g_pSignal->taskbarHiding = TRUE;
    InterlockedIncrement(&g_pSignal->version);
    return 0;
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;

void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (g_isOverridingState) {
        return;
    }

    if (IsMouseOverShellPanel()) {
        HANDLE hOverlay = CreateThread(nullptr, 0, MenuOverlayThread, nullptr, 0, nullptr);
        if (hOverlay) CloseHandle(hOverlay);
        return; 
    }

    HANDLE hSignalThread = CreateThread(nullptr, 0, SignalThread, nullptr, 0, nullptr);
    if (hSignalThread) CloseHandle(hSignalThread);

    TrayUI__Hide_Original(pThis);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Physics-Detector init");
    if (!OpenOrCreateSharedMemory()) return FALSE;
    FindShellHostPid();

    HMODULE hTaskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hTaskbarDll) {
        CloseSharedMemory();
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: virtual void __cdecl TrayUI::_Hide(void))",
                LR"(public: void __cdecl TrayUI::_Hide(void))",
            },
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(hTaskbarDll, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook TrayUI::_Hide");
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_pSignal) g_pSignal->taskbarHiding = FALSE;
    CloseSharedMemory();
}

void Wh_ModSettingsChanged() {}
