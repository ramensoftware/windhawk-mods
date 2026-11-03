// ==WindhawkMod==
// @id              restore-folder-menubar-25h2
// @name            ExplorerFrame and menubar fixes fork
// @description     Fixes explorer problems: menu bar, listview redraw, classic background color, Control Panel header/sidebar color for dark themes
// @version         4.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luxtheme -lpsapi -ldbghelp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

This is a fork of the mod `Explorerframe fixes for Win11 22H2+` by Waldemar made due to the author's apparent inactivity.
It addresses several issues with the original mod. Additionally, it incorporates the `Control Panel Color Fix` mod version 1.0 by chip33
because the version 1.0.1 does not support Classic Theme.

**The original mod description**

This mod fixes a couple of issues with File Explorer windows appearance in Windows 11 releases 22H2 and later.

Unpredictable menu bar appearance: On Windows 11 22H2 and 23H2, the menu bar may appear unpredictably. This mod allows the user to disable the menu bar entirely, or have it be shown at all times.
You can force show/hide the menu bar in the mod settings. Forcing the menu bar to be shown is intended for the Classic theme.

Delay on navigation to a new folder when using SysListView32: this is caused by a extra erroneous message sent to the list view window. This mod supresses it, fixing the problem.

Black artifacts briefly visible in new file explorer windows under Classic theme: this is caused by incorrect handling of WM_ERASEBKGND in CabinetWClass window procedure. This mod adds a option to correct this.

**Changes from the original**

* This mod restores the menubar in recent builds of 24H2 and in 25H2 with which the original mod was not compatible.
**!Important!** On newer releases of Windows 11 24H2 and later, you may need to use the [ViVeTool](https://github.com/thebookisclosed/ViVe/releases) utility
with the following command so to enable the menu:

    `vivetool /disable /id:55063786`

    Alternatively, you can use the [ViVeToolGUI](https://apps.microsoft.com/detail/9P1BW5WB82MH) from
    Windows Store.

    On even newer builds, replacing the file Explorerframe.dll with an older one may be required.

* This mod fixes the compatibility with the mod ```Fluid Window Engine Pro```, in cases where the previous mod would
whiten out the window content on restore after the minimized state if the option `Classic CabinetWClass background color` was enabled.

* The fix for the folders' background in Classic theme is applied authomatically when Classic theme is detected.

* This mod supports dynamic loading of masked explorerframe.dll for its spoofing.

**Control Panel Color Fix**

For custom dark themes, fixes white header and sidebar in Control Panel. This is compatible with dark-toned Classic Theme.
Can be disabled in the mod settings.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DisplayMenuBar: true
  $name: Display menu bar
  $description: Show the classic menu bar in Explorer windows.
- ControlPanelColorFix: true
  $name: Control Panel color fix
  $description: Fix white header and sidebar in Control Panel when using dark themes.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <Windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <strsafe.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shdeprecated.h>
#include <vector>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <unordered_set>
#include <vssym32.h>

typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

REGQUERYVALUEEXW pOriginalRegQueryValueExW = nullptr;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

bool g_settingDisplayMenuBar = true;
bool g_settingControlPanelColorFix = true;

std::mutex g_subclassedMutex;
std::vector<HWND> g_subclassedRebars;
std::vector<HWND> g_subclassedListviews;
std::vector<HWND> g_subclassedCabinets;

HANDLE g_hookEvent = NULL;
HANDLE g_workerThread = NULL;
std::atomic<bool> g_stopWorker{false};
std::atomic<bool> g_initDone{false};
UINT g_msgMenubarHackfix = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

bool NtPathToDosPath(LPCWSTR ntPath, LPWSTR dosPath, DWORD dosLen)
{
    WCHAR drives[512];
    DWORD drLen = GetLogicalDriveStringsW(ARRAYSIZE(drives), drives);
    if (!drLen)
        return false;

    for (WCHAR* d = drives; *d; d += wcslen(d) + 1)
    {
        WCHAR drive[3] = { d[0], d[1], L'\0' };
        WCHAR device[512];
        if (!QueryDosDeviceW(drive, device, ARRAYSIZE(device)))
            continue;

        size_t devLen = wcslen(device);
        if (_wcsnicmp(ntPath, device, devLen) == 0 &&
            (ntPath[devLen] == L'\\' || ntPath[devLen] == L'\0'))
        {
            if (FAILED(StringCchPrintfW(dosPath, dosLen, L"%s%s", drive, ntPath + devLen)))
                return false;
            return true;
        }
    }
    return false;
}

// Real path via mapped file, bypasses LDR FullDllName spoofing.
// Uses K32GetMappedFileNameW from kernel32, so no -lpsapi needed
// (psapi.dll just forwards to kernel32 anyway).
bool GetRealModulePath(HMODULE hMod, WCHAR* dosPath, DWORD dosLen)
{
    using K32GetMappedFileNameW_t = DWORD (WINAPI *)(HANDLE, LPVOID, LPWSTR, DWORD);

    static K32GetMappedFileNameW_t pK32 = nullptr;
    static bool tried = false;
    if (!tried)
    {
        tried = true;
        HMODULE hK32 = GetModuleHandleW(L"kernel32.dll");
        if (hK32)
            pK32 = (K32GetMappedFileNameW_t)GetProcAddress(hK32, "K32GetMappedFileNameW");
    }

    if (!pK32)
        return false;

    WCHAR ntPath[1024];
    if (!pK32(GetCurrentProcess(), hMod, ntPath, ARRAYSIZE(ntPath)))
        return false;

    return NtPathToDosPath(ntPath, dosPath, dosLen);
}

int FindMenuBand(HWND hReBar)
{
    int bandCount = (int)SendMessage(hReBar, RB_GETBANDCOUNT, 0, 0);
    for (int i = 0; i < bandCount; i++)
    {
        REBARBANDINFO rbbi = { sizeof(REBARBANDINFO) };
        rbbi.fMask = RBBIM_CHILD | RBBIM_STYLE;
        SendMessage(hReBar, RB_GETBANDINFO, i, (LPARAM)&rbbi);

        if (rbbi.hwndChild == NULL) continue;

        wchar_t className[256];
        GetClassNameW(rbbi.hwndChild, className, 256);
        if (lstrcmpW(className, L"ToolbarWindow32") != 0) continue;

        // Menu band toolbar has no image list
        if (SendMessageW(rbbi.hwndChild, TB_GETIMAGELIST, 0, 0) != NULL) continue;

        return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Rebar subclass
// ---------------------------------------------------------------------------

LRESULT CALLBACK RebarSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
    _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        {
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassedRebars.erase(
                std::remove(g_subclassedRebars.begin(), g_subclassedRebars.end(), hWnd),
                g_subclassedRebars.end());
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (g_msgMenubarHackfix && uMsg == g_msgMenubarHackfix)
    {
        HWND workerW = GetParent(hWnd);
        if (!workerW) return 0;

        HWND parent = GetParent(workerW);
        if (!parent) return 0;

        wchar_t cls[256] = {};
        GetClassNameW(parent, cls, 256);

        if (!lstrcmpW(cls, L"ShellTabWindowClass"))
        {
            SendMessageW(parent, WM_WININICHANGE, 0, 0);
        }

        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        return 0;
    }

    if (uMsg == RB_SHOWBAND)
    {
        int menuBandIndex = FindMenuBand(hWnd);
        if ((int)wParam == menuBandIndex)
        {
            LRESULT toRet = DefSubclassProc(hWnd, uMsg, wParam, (LPARAM)g_settingDisplayMenuBar);
            if (g_settingDisplayMenuBar && g_msgMenubarHackfix)
                PostMessage(hWnd, g_msgMenubarHackfix, 0, 0);
            return toRet;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Listview subclass
// ---------------------------------------------------------------------------

LRESULT CALLBACK ListviewSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
    _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        {
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassedListviews.erase(
                std::remove(g_subclassedListviews.begin(), g_subclassedListviews.end(), hWnd),
                g_subclassedListviews.end());
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_SETREDRAW)
    {
        wParam = TRUE;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Cabinet subclass (single mechanism for frame behavior,
// replaces CExplorerFrame::v_WndProc symbol hook)
// ---------------------------------------------------------------------------

LRESULT CALLBACK CabinetSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
    _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY)
    {
        {
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassedCabinets.erase(
                std::remove(g_subclassedCabinets.begin(), g_subclassedCabinets.end(), hWnd),
                g_subclassedCabinets.end());
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_SHOWWINDOW)
    {
        SendMessageW(hWnd, WM_WININICHANGE, 0, 0);
    }

    if (uMsg == WM_ERASEBKGND)
    {
        if (!IsAppThemed())
        {
            if ((GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == 0)
            {
                HDC hdc = (HDC)wParam;

                if (GetObjectType(hdc) != OBJ_MEMDC)
                {
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                    HBRUSH hbr = (HBRUSH)(COLOR_WINDOW + 1);
                    FillRect(hdc, &rc, hbr);
                    return 1;
                }
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool EnsureRebarSubclassed(HWND h)
{
    if (!h || !IsWindow(h))
        return false;

    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        for (HWND x : g_subclassedRebars)
            if (x == h)
                return true;
    }

    BOOL ok = WindhawkUtils::SetWindowSubclassFromAnyThread(h, RebarSubclassProc, 0);
    if (!ok)
        return false;

    std::lock_guard<std::mutex> lock(g_subclassedMutex);
    for (HWND x : g_subclassedRebars)
        if (x == h)
            return true;

    g_subclassedRebars.push_back(h);
    return true;
}

bool EnsureCabinetSubclassed(HWND h)
{
    if (!h || !IsWindow(h))
        return false;

    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        for (HWND x : g_subclassedCabinets)
            if (x == h)
                return true;
    }

    BOOL ok = WindhawkUtils::SetWindowSubclassFromAnyThread(h, CabinetSubclassProc, 0);
    if (!ok)
        return false;

    std::lock_guard<std::mutex> lock(g_subclassedMutex);
    for (HWND x : g_subclassedCabinets)
        if (x == h)
            return true;

    g_subclassedCabinets.push_back(h);
    return true;
}

// ---------------------------------------------------------------------------
// explorerframe originals: per-copy slots.
// Realistically 1-2 copies (system + redirected), 4 slots leave headroom.
// Only Init + SetFlags need per-copy originals, frame is covered by HWND.
// ---------------------------------------------------------------------------

typedef long (*__cdecl CBSInitialize_t)(void*, HWND);
typedef long (*__cdecl CSBSetFlags_t)(void*, unsigned long, unsigned long);

static const int kMaxEFSlots = 4;

struct EFSlot {
    bool inUse = false;
    HMODULE hMod = nullptr;
    CBSInitialize_t initOrig = nullptr;
    CSBSetFlags_t setFlagsOrig = nullptr;
    bool hookOk = false;
};

EFSlot g_efSlots[kMaxEFSlots];
std::mutex g_efSlotsMutex;
std::unordered_set<HMODULE> g_efTried;

long __cdecl CommonCBSInitializeHook(int slot, void* pThis, HWND hWnd)
{
    if (GetWindowLong(hWnd, GWL_EXSTYLE) == 0)
    {
        HWND rb = FindWindowExW(hWnd, NULL, L"ReBarWindow32", NULL);
        if (rb != NULL)
            EnsureRebarSubclassed(rb);
    }

    CBSInitialize_t orig = g_efSlots[slot].initOrig;
    return orig ? orig(pThis, hWnd) : 0;
}

long __cdecl CommonCSBSetFlagsHook(int slot, void* pThis, unsigned long a, unsigned long b)
{
    if (a & BSF_UISETBYAUTOMATION)
        a &= ~BSF_UISETBYAUTOMATION;

    CSBSetFlags_t orig = g_efSlots[slot].setFlagsOrig;
    return orig ? orig(pThis, a, b) : 0;
}

template<int S> __attribute__((noinline)) long __cdecl CBSInitializeHookT(void* pThis, HWND hWnd)
{
    return CommonCBSInitializeHook(S, pThis, hWnd);
}

template<int S> __attribute__((noinline)) long __cdecl CSBSetFlagsHookT(void* pThis, unsigned long a, unsigned long b)
{
    return CommonCSBSetFlagsHook(S, pThis, a, b);
}

void* g_CBSHooks[kMaxEFSlots] = {
    (void*)CBSInitializeHookT<0>, (void*)CBSInitializeHookT<1>,
    (void*)CBSInitializeHookT<2>, (void*)CBSInitializeHookT<3>,
};

void* g_SetFlagsHooks[kMaxEFSlots] = {
    (void*)CSBSetFlagsHookT<0>, (void*)CSBSetFlagsHookT<1>,
    (void*)CSBSetFlagsHookT<2>, (void*)CSBSetFlagsHookT<3>,
};

bool InstallViaWindhawk(HMODULE hMod, int slot)
{
    if (g_stopWorker.load())
        return false;

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] =
    {
        {
            {
                L"protected: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
                L"private: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
                L"public: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
            },
            (void**)&g_efSlots[slot].initOrig,
            g_CBSHooks[slot],
            true,
        },
        {
            {
                L"public: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
                L"protected: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
                L"private: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
            },
            (void**)&g_efSlots[slot].setFlagsOrig,
            g_SetFlagsHooks[slot],
            true,
        },
    };

    WindhawkUtils::HookSymbols(hMod, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks));

    bool ok = (g_efSlots[slot].initOrig != nullptr) || (g_efSlots[slot].setFlagsOrig != nullptr);

    // Hooks registered outside Wh_ModInit are not enabled until applied.
    if (ok && g_initDone.load() && !g_stopWorker.load())
    {
        Wh_ApplyHookOperations();
    }

    return ok;
}

bool InstallExplorerFrameHooks(HMODULE hMod)
{
    if (!hMod || g_stopWorker.load())
        return false;

    {
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        for (int i = 0; i < kMaxEFSlots; i++)
        {
            if (g_efSlots[i].inUse && g_efSlots[i].hMod == hMod)
                return g_efSlots[i].hookOk;
        }

        // Remember attempted modules, HookSymbols once per module.
        if (g_efTried.find(hMod) != g_efTried.end())
            return false;
    }

    WCHAR ldrPath[MAX_PATH] = {};
    WCHAR realPath[MAX_PATH] = {};
    DWORD n1 = GetModuleFileNameW(hMod, ldrPath, ARRAYSIZE(ldrPath));
    bool haveReal = GetRealModulePath(hMod, realPath, ARRAYSIZE(realPath));
    bool masked = (n1 && haveReal && _wcsicmp(ldrPath, realPath) != 0);

    if (masked)
    {
        // Masked copy: LDR path lies, symbol resolution would target
        // the wrong file. Skip symbols, HWND fallback covers UI.
        Wh_Log(L"Masked copy %p LDR=[%s] real=[%s], skip symbols, HWND fallback covers", hMod, ldrPath, realPath);
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        g_efTried.insert(hMod);
        return true;
    }

    int slot = -1;
    {
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        for (int i = 0; i < kMaxEFSlots; i++)
        {
            if (!g_efSlots[i].inUse)
            {
                slot = i;
                break;
            }
        }

        if (slot < 0)
        {
            Wh_Log(L"No free slots for %p", hMod);
            return false;
        }

        g_efSlots[slot].inUse = true;
        g_efSlots[slot].hMod = hMod;
        g_efSlots[slot].initOrig = nullptr;
        g_efSlots[slot].setFlagsOrig = nullptr;
        g_efSlots[slot].hookOk = false;
        g_efTried.insert(hMod);
    }

    Wh_Log(L"Hooking %p LDR=[%s]", hMod, n1 ? ldrPath : L"?");

    bool ok = InstallViaWindhawk(hMod, slot);

    {
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        g_efSlots[slot].hookOk = ok;
        // Keep slot even on failure to avoid retry loop.
    }

    if (!ok)
        Wh_Log(L"Symbol hooks failed for %p, HWND fallback covers", hMod);
    else
        Wh_Log(L"Hooked %p slot %d init=%p flags=%p", hMod, slot, g_efSlots[slot].initOrig, g_efSlots[slot].setFlagsOrig);

    return ok;
}

void HookAllLoadedExplorerFrameCopies()
{
    if (g_stopWorker.load())
        return;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (hSnap == INVALID_HANDLE_VALUE)
        return;

    MODULEENTRY32W me;
    me.dwSize = sizeof(me);
    std::vector<HMODULE> efMods;
    if (Module32FirstW(hSnap, &me))
    {
        do
        {
            if (!lstrcmpiW(me.szModule, L"explorerframe.dll") || !lstrcmpiW(me.szModule, L"explorerframe"))
                efMods.push_back(me.hModule);
        } while (Module32NextW(hSnap, &me));
    }
    CloseHandle(hSnap);

    {
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        for (int i = 0; i < kMaxEFSlots; i++)
        {
            if (!g_efSlots[i].inUse)
                continue;

            bool found = false;
            for (HMODULE h : efMods)
            {
                if (h == g_efSlots[i].hMod)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                Wh_Log(L"Free slot %d (%p gone)", i, g_efSlots[i].hMod);
                g_efSlots[i].inUse = false;
                g_efSlots[i].hMod = nullptr;
                g_efSlots[i].initOrig = nullptr;
                g_efSlots[i].setFlagsOrig = nullptr;
                g_efSlots[i].hookOk = false;
            }
        }

        for (auto it = g_efTried.begin(); it != g_efTried.end(); )
        {
            bool found = false;
            for (HMODULE h : efMods)
            {
                if (h == *it)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                it = g_efTried.erase(it);
            else
                ++it;
        }
    }

    for (HMODULE h : efMods)
        InstallExplorerFrameHooks(h);
}

// ---------------------------------------------------------------------------
// Load trigger: only SetEvent, heavy work in worker (no loader-lock deadlock).
// Single hook: every LoadLibrary in kernelbase ends up in LdrLoadDll,
// and the redirector bypasses LoadLibrary hooks via LoadLibraryExW_Original.
// ---------------------------------------------------------------------------

typedef LONG (WINAPI *LdrLoadDll_t)(PWSTR SearchPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* BaseAddress);
LdrLoadDll_t pOriginalLdrLoadDll = nullptr;

LONG WINAPI LdrLoadDll_Hook(PWSTR SearchPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* BaseAddress)
{
    LONG st = pOriginalLdrLoadDll(SearchPath, DllCharacteristics, DllName, BaseAddress);

    if (st >= 0 && BaseAddress && *BaseAddress)
    {
        // Signal on every successful load, no name checks here.
        // Auto-reset event merges bursts into one rescan, worker
        // finds explorerframe copies via Toolhelp by base name.
        if (g_hookEvent && !g_stopWorker.load())
            SetEvent(g_hookEvent);
    }

    return st;
}

// ---------------------------------------------------------------------------
// CreateWindow fallback (version independent)
// ---------------------------------------------------------------------------

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t pOriginalCreateWindowExW = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
    HWND hwnd = pOriginalCreateWindowExW(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);

    if (hwnd)
    {
        wchar_t buf[64];
        if (GetClassNameW(hwnd, buf, 64))
        {
            if (!lstrcmpW(buf, L"CabinetWClass") || !lstrcmpW(buf, L"ExploreWClass"))
            {
                EnsureCabinetSubclassed(hwnd);
            }
            else if (!lstrcmpW(buf, L"ReBarWindow32"))
            {
                HWND root = GetAncestor(hwnd, GA_ROOT);
                if (root)
                {
                    wchar_t rootCls[64];
                    if (GetClassNameW(root, rootCls, 64) &&
                        (!lstrcmpW(rootCls, L"CabinetWClass") || !lstrcmpW(rootCls, L"ExploreWClass")))
                    {
                        EnsureRebarSubclassed(hwnd);
                    }
                }
            }
        }
    }

    return hwnd;
}

BOOL CALLBACK EnumChildForRebar(HWND h, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId())
        return TRUE;

    wchar_t cls[64];
    if (GetClassNameW(h, cls, 64) && !lstrcmpW(cls, L"ReBarWindow32"))
        EnsureRebarSubclassed(h);

    return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId())
        return TRUE;

    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64))
        return TRUE;

    if (!lstrcmpW(cls, L"CabinetWClass") || !lstrcmpW(cls, L"ExploreWClass"))
    {
        EnsureCabinetSubclassed(h);
        EnumChildWindows(h, EnumChildForRebar, 0);
    }

    return TRUE;
}

void SubclassExistingWindows()
{
    EnumWindows(EnumWindowsProc, 0);
}

// ---------------------------------------------------------------------------
// Hook: CListViewHost::CreateControl
// ---------------------------------------------------------------------------

typedef long (*__cdecl CLVHCreateControl_t)(void*, HWND, void*, void*);
CLVHCreateControl_t CLVHCreateControlOriginal = nullptr;

long __cdecl CLVHCreateControlHook(void* pThis, HWND hWnd, void* a, void* b)
{
    long ret = CLVHCreateControlOriginal(pThis, hWnd, a, b);

    HWND listview = GetWindow(hWnd, GW_CHILD);
    if (listview != NULL)
    {
        wchar_t name[32];
        GetClassNameW(listview, name, 32);
        if (lstrcmpW(name, L"SysListView32") == 0)
        {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(listview, ListviewSubclassProc, NULL))
            {
                std::lock_guard<std::mutex> lock(g_subclassedMutex);
                g_subclassedListviews.push_back(listview);
            }
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------
// Hook: DirectUI::Element::PaintBackground (Control Panel Color Fix)
// ---------------------------------------------------------------------------

typedef VOID(__cdecl *Element_PaintBgT)(
    class Element*, HDC, class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
Element_PaintBgT Element_PaintBg = nullptr;

VOID __cdecl Element_PaintBgHook(
    class Element* This, HDC hdc, class Value* value,
    LPRECT pRect, LPRECT pClipRect, LPRECT pExcludeRect, LPRECT pTargetRect)
{
    if (!value || !pRect) {
        Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
        return;
    }
    if (g_settingControlPanelColorFix)
    {
        if ((int)(*(DWORD*)value << 26) >> 26 != 9)
        {
            auto v44 = *((unsigned __int64*)value + 1);
            auto v45 = (v44 + 20) & 7;
            if (v45 == 4)
            {
                HWND wnd = WindowFromDC(hdc);
                HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
                if (hTh)
                {
                    COLORREF clrBg;
                    if (SUCCEEDED(GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &clrBg)))
                    {
                        HBRUSH brush = CreateSolidBrush(clrBg);
                        FillRect(hdc, pRect, brush);
                        DeleteObject(brush);
                    }
                    CloseThemeData(hTh);
                }
                return;
            }
        }
    }

    Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
}

// ---------------------------------------------------------------------------
// Also hook registry query for greater reliability
// ---------------------------------------------------------------------------

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    if (lpValueName && !lstrcmpiW(lpValueName, L"AlwaysShowMenus")) {
        if (lpData && !lpcbData) return ERROR_INVALID_PARAMETER;

        if (lpType) *lpType = REG_DWORD;

        if (lpcbData)
        {
            DWORD cbBuffer = *lpcbData;
            *lpcbData = sizeof(DWORD);
            if (lpData)
            {
                if (cbBuffer < sizeof(DWORD)) return ERROR_MORE_DATA;
                *(DWORD*)lpData = g_settingDisplayMenuBar;
            }
        }
        return ERROR_SUCCESS;
    }
    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

// ---------------------------------------------------------------------------
// Worker (event-driven, no polling, no EnumWindows here)
// ---------------------------------------------------------------------------

DWORD WINAPI HookWorkerThreadProc(LPVOID)
{
    while (!g_stopWorker.load())
    {
        WaitForSingleObject(g_hookEvent, INFINITE);

        if (g_stopWorker.load())
            break;

        HookAllLoadedExplorerFrameCopies();
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer Frame Fixes + Control Panel Color Fix Init");

    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
    g_settingControlPanelColorFix = Wh_GetIntSetting(L"ControlPanelColorFix");
    g_msgMenubarHackfix = RegisterWindowMessageW(L"Windhawk_MenubarHackfix_restore-folder-menubar-25h2");

    // --- explorerframe.dll hooks ---
    // SEARCH_SYSTEM32 as hardening (not a KnownDLL, bare name would
    // check app dir first). Still compatible with the redirector:
    // it matches by base name and redirects this call too, and any
    // extra copies are picked up by HookAllLoadedExplorerFrameCopies.
    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hExplorerFrame)
        InstallExplorerFrameHooks(hExplorerFrame);

    HookAllLoadedExplorerFrameCopies();

    // --- shell32.dll hooks ---
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK shell32_dll_hooks[] =
    {
        {
            { L"public: virtual long __cdecl CListViewHost::CreateControl(struct HWND__ *,struct IListControlHost *,struct IViewSettings *)" },
            (void**)&CLVHCreateControlOriginal,
            (void*)CLVHCreateControlHook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32_dll_hooks, ARRAYSIZE(shell32_dll_hooks)))
    {
        Wh_Log(L"Failed to install shell32.dll hooks");
        return FALSE;
    }

    // --- dui70.dll hook (Control Panel Color Fix) ---
    HMODULE hDui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hDui70)
    {
        Wh_Log(L"Failed to load dui70.dll — Control Panel Color Fix will be skipped.");
    }
    else
    {
        WindhawkUtils::SYMBOL_HOOK dui70_dll_hook = {
            { L"public: void __cdecl DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)" },
            (void**)&Element_PaintBg,
            (void*)Element_PaintBgHook,
            false
        };

        if (!WindhawkUtils::HookSymbols(hDui70, &dui70_dll_hook, 1))
        {
            Wh_Log(L"Failed to hook DirectUI::Element::PaintBackground — Control Panel Color Fix will be skipped.");
        }
        else
        {
            Wh_Log(L"Control Panel Color Fix hook installed.");
        }
    }

    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "RegQueryValueExW"), (void*)RegQueryValueExWHook, (void**)&pOriginalRegQueryValueExW);

    void* pCW = (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW");
    if (pCW)
        Wh_SetFunctionHook(pCW, (void*)CreateWindowExW_Hook, (void**)&pOriginalCreateWindowExW);

    // One-time sweep for pre-existing windows, new ones via CreateWindow hook.
    SubclassExistingWindows();

    g_hookEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!g_hookEvent)
    {
        Wh_Log(L"Failed to create event");
        return FALSE;
    }

    void* pLdr = (void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "LdrLoadDll");
    if (pLdr && !Wh_SetFunctionHook(pLdr, (void*)LdrLoadDll_Hook, (void**)&pOriginalLdrLoadDll))
        Wh_Log(L"Failed to hook LdrLoadDll");

    g_workerThread = CreateThread(NULL, 0, HookWorkerThreadProc, NULL, 0, NULL);
    if (!g_workerThread)
    {
        Wh_Log(L"Failed to create worker thread");
        CloseHandle(g_hookEvent);
        g_hookEvent = NULL;
        return FALSE;
    }

    g_initDone.store(true);

    return TRUE;
}

void Wh_ModBeforeUninit()
{
    g_stopWorker.store(true);

    if (g_hookEvent)
        SetEvent(g_hookEvent);

    if (g_workerThread)
    {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer Frame Fixes + Control Panel Color Fix Uninit");

    // Take a snapshot of the vectors under the lock and clear the globals,
    // then remove the subclasses outside the lock. This is important because
    // RemoveWindowSubclassFromAnyThread sends a message to the target
    // window's thread, which could be blocked waiting on g_subclassedMutex
    // (e.g. in a WM_DESTROY handler) — locking around it could deadlock.
    std::vector<HWND> rebars;
    std::vector<HWND> listviews;
    std::vector<HWND> cabinets;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        rebars.swap(g_subclassedRebars);
        listviews.swap(g_subclassedListviews);
        cabinets.swap(g_subclassedCabinets);
    }

    Wh_Log(L"Removing subclasses from %zu rebars.", rebars.size());
    for (HWND h : rebars)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, RebarSubclassProc);

    Wh_Log(L"Removing subclasses from %zu listviews.", listviews.size());
    for (HWND h : listviews)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, ListviewSubclassProc);

    Wh_Log(L"Removing subclasses from %zu cabinets.", cabinets.size());
    for (HWND h : cabinets)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, CabinetSubclassProc);

    if (g_hookEvent)
    {
        CloseHandle(g_hookEvent);
        g_hookEvent = NULL;
    }
}

void Wh_ModSettingsChanged()
{
    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
    g_settingControlPanelColorFix = Wh_GetIntSetting(L"ControlPanelColorFix");
}
