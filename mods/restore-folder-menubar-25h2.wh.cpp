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

Compatible with ExplorerFrame Legacy Redirector.

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
#include <psapi.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shdeprecated.h>
#include <vector>
#include <algorithm>
#include <mutex>
#include <vssym32.h>
#include <windhawk_api.h>
#include <windows.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE __declspec(noinline)
#endif

typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
REGQUERYVALUEEXW pOriginalRegQueryValueExW = nullptr;

// ---------------------------------------------------------------------------
bool g_settingDisplayMenuBar = true;
bool g_settingControlPanelColorFix = true;

std::mutex g_subclassedMutex;
std::vector<HWND> g_subclassedRebars;
std::vector<HWND> g_subclassedListviews;
std::vector<HWND> g_subclassedCabinets;

HANDLE g_hookEvent = NULL;
HANDLE g_workerThread = NULL;
volatile BOOL g_stopWorker = FALSE;

#define WM_MENUBAR_HACKFIX (WM_USER + 1)

// ---------------------------------------------------------------------------
// explorerframe name checks (lock-free, safe inside Ldr hook)
// ---------------------------------------------------------------------------
bool IsEFBaseCounted(const WCHAR* s, int cch)
{
    if (cch == 13) {
        static const WCHAR k[] = L"explorerframe";
        for (int i = 0; i < 13; i++) {
            WCHAR a = s[i], b = k[i];
            if (a >= L'A' && a <= L'Z') a += 32;
            if (b >= L'A' && b <= L'Z') b += 32;
            if (a != b) return false;
        }
        return true;
    }
    if (cch == 17) {
        static const WCHAR k[] = L"explorerframe.dll";
        for (int i = 0; i < 17; i++) {
            WCHAR a = s[i], b = k[i];
            if (a >= L'A' && a <= L'Z') a += 32;
            if (b >= L'A' && b <= L'Z') b += 32;
            if (a != b) return false;
        }
        return true;
    }
    return false;
}

bool IsExplorerFrameDllNameW(LPCWSTR path)
{
    if (!path || !*path) return false;
    LPCWSTR base = path;
    for (LPCWSTR p = path; *p; ++p)
        if (*p == L'\\' || *p == L'/') base = p + 1;
    return lstrcmpiW(base, L"explorerframe.dll") == 0 ||
           lstrcmpiW(base, L"explorerframe") == 0;
}

bool IsExplorerFrameDllNameA(LPCSTR path)
{
    if (!path || !*path) return false;
    LPCSTR base = path;
    for (LPCSTR p = path; *p; ++p)
        if (*p == '\\' || *p == '/') base = p + 1;
    return lstrcmpiA(base, "explorerframe.dll") == 0 ||
           lstrcmpiA(base, "explorerframe") == 0;
}

bool IsExplorerFrameUnicodeString(PUNICODE_STRING s)
{
    if (!s || !s->Buffer || s->Length < 13 * (USHORT)sizeof(WCHAR))
        return false;
    USHORT cch = (USHORT)(s->Length / sizeof(WCHAR));
    int lastSlash = -1;
    for (int i = 0; i < cch; i++) {
        WCHAR c = s->Buffer[i];
        if (c == L'\\' || c == L'/') lastSlash = i;
    }
    int start = lastSlash + 1;
    int len = (int)cch - start;
    if (len <= 0) return false;
    return IsEFBaseCounted(s->Buffer + start, len);
}

// ---------------------------------------------------------------------------
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
        if (SendMessageW(rbbi.hwndChild, TB_GETIMAGELIST, 0, 0) != NULL) continue;
        return i;
    }
    return -1;
}

LRESULT CALLBACK RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        g_subclassedRebars.erase(std::remove(g_subclassedRebars.begin(), g_subclassedRebars.end(), hWnd), g_subclassedRebars.end());
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    if (uMsg == WM_MENUBAR_HACKFIX) {
        HWND workerW = GetParent(hWnd);
        if (!workerW) return 0;
        HWND parent = GetParent(workerW);
        if (!parent) return 0;
        wchar_t cls[256] = {};
        GetClassNameW(parent, cls, 256);
        if (!lstrcmpW(cls, L"ShellTabWindowClass"))
            SendMessageW(parent, WM_WININICHANGE, 0, 0);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        return 0;
    }
    if (uMsg == RB_SHOWBAND) {
        int menuBandIndex = FindMenuBand(hWnd);
        if ((int)wParam == menuBandIndex) {
            LRESULT toRet = DefSubclassProc(hWnd, uMsg, wParam, (LPARAM)g_settingDisplayMenuBar);
            if (g_settingDisplayMenuBar)
                PostMessage(hWnd, WM_MENUBAR_HACKFIX, 0, 0);
            return toRet;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ListviewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        g_subclassedListviews.erase(std::remove(g_subclassedListviews.begin(), g_subclassedListviews.end(), hWnd), g_subclassedListviews.end());
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    if (uMsg == WM_SETREDRAW) wParam = TRUE;
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Fallback for CExplorerFrame::v_WndProc - HWND based, version independent
LRESULT CALLBACK CabinetSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        g_subclassedCabinets.erase(std::remove(g_subclassedCabinets.begin(), g_subclassedCabinets.end(), hWnd), g_subclassedCabinets.end());
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    if (uMsg == WM_SHOWWINDOW) {
        SendMessageW(hWnd, WM_WININICHANGE, 0, 0);
    }
    if (uMsg == WM_ERASEBKGND) {
        if (!IsAppThemed()) {
            if ((GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == 0) {
                HDC hdc = (HDC)wParam;
                if (GetObjectType(hdc) != OBJ_MEMDC) {
                    RECT rc; GetClientRect(hWnd, &rc);
                    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
                    return 1;
                }
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void EnsureRebarSubclassed(HWND h)
{
    if (!h || !IsWindow(h)) return;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        for (HWND x : g_subclassedRebars) if (x == h) return;
    }
    WindhawkUtils::SetWindowSubclassFromAnyThread(h, RebarSubclassProc, 0);
    std::lock_guard<std::mutex> lock(g_subclassedMutex);
    for (HWND x : g_subclassedRebars) if (x == h) return;
    g_subclassedRebars.push_back(h);
}

void EnsureCabinetSubclassed(HWND h)
{
    if (!h || !IsWindow(h)) return;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        for (HWND x : g_subclassedCabinets) if (x == h) return;
    }
    WindhawkUtils::SetWindowSubclassFromAnyThread(h, CabinetSubclassProc, 0);
    std::lock_guard<std::mutex> lock(g_subclassedMutex);
    for (HWND x : g_subclassedCabinets) if (x == h) return;
    g_subclassedCabinets.push_back(h);
}

// ---------------------------------------------------------------------------
// per-copy slots (different HMODULE -> different hook addr -> different orig)
// ---------------------------------------------------------------------------
typedef long (*__cdecl CBSInitialize_t)(void*, HWND);
typedef long (*__cdecl CEFWndProc_t)(void*, HWND, unsigned int, WPARAM, LPARAM);
typedef long (*__cdecl CSBSetFlags_t)(void*, unsigned long, unsigned long);

static const int kMaxEFSlots = 8;
struct EFSlot {
    bool inUse = false;
    HMODULE hMod = nullptr;
    CBSInitialize_t initOrig = nullptr;
    CEFWndProc_t wndProcOrig = nullptr;
    CSBSetFlags_t setFlagsOrig = nullptr;
};
EFSlot g_efSlots[kMaxEFSlots];
std::mutex g_efSlotsMutex;

long __cdecl CommonCBSInitializeHook(int slot, void* pThis, HWND hWnd)
{
    if (GetWindowLong(hWnd, GWL_EXSTYLE) == 0) {
        HWND rb = FindWindowExW(hWnd, NULL, L"ReBarWindow32", NULL);
        if (rb) EnsureRebarSubclassed(rb);
    }
    CBSInitialize_t o = g_efSlots[slot].initOrig;
    return o ? o(pThis, hWnd) : 0;
}
long __cdecl CommonCEFWndProcHook(int slot, void* pThis, HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SHOWWINDOW) SendMessageW(hWnd, WM_WININICHANGE, 0, 0);
    if (uMsg == WM_ERASEBKGND) {
        if (!IsAppThemed() && ((GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == 0)) {
            HDC hdc = (HDC)wParam;
            if (GetObjectType(hdc) != OBJ_MEMDC) {
                RECT rc; GetClientRect(hWnd, &rc);
                FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
                return 1;
            }
        }
    }
    CEFWndProc_t o = g_efSlots[slot].wndProcOrig;
    return o ? o(pThis, hWnd, uMsg, wParam, lParam) : 0;
}
long __cdecl CommonCSBSetFlagsHook(int slot, void* pThis, unsigned long a, unsigned long b)
{
    if (a & BSF_UISETBYAUTOMATION) a &= ~BSF_UISETBYAUTOMATION;
    CSBSetFlags_t o = g_efSlots[slot].setFlagsOrig;
    return o ? o(pThis, a, b) : 0;
}

template<int S> NOINLINE long __cdecl CBSInitializeHookT(void* pThis, HWND hWnd) { return CommonCBSInitializeHook(S, pThis, hWnd); }
template<int S> NOINLINE long __cdecl CEFWndProcHookT(void* pThis, HWND hWnd, unsigned int m, WPARAM w, LPARAM l) { return CommonCEFWndProcHook(S, pThis, hWnd, m, w, l); }
template<int S> NOINLINE long __cdecl CSBSetFlagsHookT(void* pThis, unsigned long a, unsigned long b) { return CommonCSBSetFlagsHook(S, pThis, a, b); }

void* g_CBSHooks[kMaxEFSlots] = {
    (void*)CBSInitializeHookT<0>, (void*)CBSInitializeHookT<1>,
    (void*)CBSInitializeHookT<2>, (void*)CBSInitializeHookT<3>,
    (void*)CBSInitializeHookT<4>, (void*)CBSInitializeHookT<5>,
    (void*)CBSInitializeHookT<6>, (void*)CBSInitializeHookT<7>,
};
void* g_WndProcHooks[kMaxEFSlots] = {
    (void*)CEFWndProcHookT<0>, (void*)CEFWndProcHookT<1>,
    (void*)CEFWndProcHookT<2>, (void*)CEFWndProcHookT<3>,
    (void*)CEFWndProcHookT<4>, (void*)CEFWndProcHookT<5>,
    (void*)CEFWndProcHookT<6>, (void*)CEFWndProcHookT<7>,
};
void* g_SetFlagsHooks[kMaxEFSlots] = {
    (void*)CSBSetFlagsHookT<0>, (void*)CSBSetFlagsHookT<1>,
    (void*)CSBSetFlagsHookT<2>, (void*)CSBSetFlagsHookT<3>,
    (void*)CSBSetFlagsHookT<4>, (void*)CSBSetFlagsHookT<5>,
    (void*)CSBSetFlagsHookT<6>, (void*)CSBSetFlagsHookT<7>,
};

// ---------------------------------------------------------------------------
// real path via mapped file (bypasses LDR masking)
// ---------------------------------------------------------------------------
bool NtPathToDosPath(LPCWSTR ntPath, LPWSTR dosPath, DWORD dosLen)
{
    WCHAR drives[512];
    DWORD drLen = GetLogicalDriveStringsW(ARRAYSIZE(drives), drives);
    if (!drLen) return false;
    for (WCHAR* d = drives; *d; d += wcslen(d) + 1) {
        WCHAR drive[3] = { d[0], d[1], L'\0' };
        WCHAR device[512];
        if (!QueryDosDeviceW(drive, device, ARRAYSIZE(device))) continue;
        size_t devLen = wcslen(device);
        if (_wcsnicmp(ntPath, device, devLen) == 0 &&
            (ntPath[devLen] == L'\\' || ntPath[devLen] == L'\0')) {
            if (FAILED(StringCchPrintfW(dosPath, dosLen, L"%s%s", drive, ntPath + devLen)))
                return false;
            return true;
        }
    }
    return false;
}

bool GetRealModulePath(HMODULE hMod, WCHAR* dosPath, DWORD dosLen)
{
    WCHAR ntPath[1024];
    if (!GetMappedFileNameW(GetCurrentProcess(), hMod, ntPath, ARRAYSIZE(ntPath)))
        return false;
    return NtPathToDosPath(ntPath, dosPath, dosLen);
}

DWORD GetModuleSize(HMODULE hMod)
{
    MODULEINFO mi = {};
    if (GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi)) && mi.SizeOfImage)
        return (DWORD)mi.SizeOfImage;

    DWORD size = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W me;
        me.dwSize = sizeof(me);
        if (Module32FirstW(hSnap, &me)) {
            do {
                if (me.hModule == hMod) { size = me.modBaseSize; break; }
            } while (Module32NextW(hSnap, &me));
        }
        CloseHandle(hSnap);
    }
    return size;
}

// ---------------------------------------------------------------------------
// custom DbgHelp hook for masked copies (uses real path, not LDR path)
// ---------------------------------------------------------------------------
bool TryHookOne(const wchar_t* const cands[], int nCand, DWORD64 modBase, DWORD modSize, void* hookFunc, void** origOut)
{
    HANDLE hProc = GetCurrentProcess();
    const DWORD MAXN = 1024;
    DWORD bufSize = sizeof(SYMBOL_INFOW) + MAXN * sizeof(WCHAR);
    SYMBOL_INFOW* sym = (SYMBOL_INFOW*)HeapAlloc(GetProcessHeap(), 0, bufSize);
    if (!sym) return false;
    sym->SizeOfStruct = sizeof(SYMBOL_INFOW);
    sym->MaxNameLen = MAXN;
    bool hooked = false;
    for (int i = 0; i < nCand; i++) {
        if (SymFromNameW(hProc, cands[i], sym)) {
            DWORD64 addr = sym->Address;
            if (addr >= modBase && addr < modBase + modSize) {
                if (Wh_SetFunctionHook((void*)addr, hookFunc, origOut)) {
                    Wh_Log(L"Hooked %s at %p", cands[i], (void*)addr);
                    hooked = true;
                    break;
                }
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, sym);
    return hooked;
}

bool InstallMaskedCustom(HMODULE hMod, int slot)
{
    WCHAR realDos[MAX_PATH];
    if (!GetRealModulePath(hMod, realDos, ARRAYSIZE(realDos))) {
        Wh_Log(L"GetRealModulePath failed for %p", hMod);
        return false;
    }
    DWORD size = GetModuleSize(hMod);
    if (!size) {
        Wh_Log(L"GetModuleSize failed for %p", hMod);
        return false;
    }
    HANDLE hProc = GetCurrentProcess();
    SymInitializeW(hProc, NULL, FALSE);
    DWORD64 base = (DWORD64)hMod;
    SymUnloadModule64(hProc, base);
    DWORD64 modBase = SymLoadModuleExW(hProc, NULL, realDos, NULL, base, size, NULL, 0);
    if (!modBase) {
        Wh_Log(L"SymLoadModuleEx failed %u for %s", GetLastError(), realDos);
        return false;
    }
    Wh_Log(L"SymLoad ok %p -> %s", hMod, realDos);

    static const wchar_t* wndCands[] = {
        L"private: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)",
        L"protected: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)",
        L"public: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
    };
    static const wchar_t* initCands[] = {
        L"protected: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
        L"private: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
        L"public: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)"
    };
    static const wchar_t* flagsCands[] = {
        L"public: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
        L"protected: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
        L"private: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)"
    };

    bool okWnd = TryHookOne(wndCands, 3, modBase, size, g_WndProcHooks[slot], (void**)&g_efSlots[slot].wndProcOrig);
    bool okInit = TryHookOne(initCands, 3, modBase, size, g_CBSHooks[slot], (void**)&g_efSlots[slot].initOrig);
    bool okFlags = TryHookOne(flagsCands, 3, modBase, size, g_SetFlagsHooks[slot], (void**)&g_efSlots[slot].setFlagsOrig);
    Wh_Log(L"Masked result %p: wnd=%d init=%d flags=%d", hMod, (int)okWnd, (int)okInit, (int)okFlags);
    return okWnd || okInit || okFlags;
}

bool InstallViaWindhawk(HMODULE hMod, int slot)
{
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        { { L"private: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)",
            L"protected: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)",
            L"public: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" },
          (void**)&g_efSlots[slot].wndProcOrig, g_WndProcHooks[slot], true },
        { { L"protected: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
            L"private: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)",
            L"public: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)" },
          (void**)&g_efSlots[slot].initOrig, g_CBSHooks[slot], true },
        { { L"public: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
            L"protected: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)",
            L"private: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)" },
          (void**)&g_efSlots[slot].setFlagsOrig, g_SetFlagsHooks[slot], true },
    };
    WindhawkUtils::HookSymbols(hMod, hooks, ARRAYSIZE(hooks));
    return g_efSlots[slot].wndProcOrig || g_efSlots[slot].initOrig || g_efSlots[slot].setFlagsOrig;
}

bool InstallExplorerFrameHooks(HMODULE hMod)
{
    if (!hMod) return false;
    int slot = -1;
    {
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        for (int i = 0; i < kMaxEFSlots; i++)
            if (g_efSlots[i].inUse && g_efSlots[i].hMod == hMod) return true;
        for (int i = 0; i < kMaxEFSlots; i++)
            if (!g_efSlots[i].inUse) { slot = i; break; }
        if (slot < 0) { Wh_Log(L"No free slots for %p", hMod); return false; }
        g_efSlots[slot].inUse = true;
        g_efSlots[slot].hMod = hMod;
        g_efSlots[slot].initOrig = nullptr;
        g_efSlots[slot].wndProcOrig = nullptr;
        g_efSlots[slot].setFlagsOrig = nullptr;
    }

    WCHAR ldrPath[MAX_PATH] = {};
    WCHAR realPath[MAX_PATH] = {};
    DWORD n1 = GetModuleFileNameW(hMod, ldrPath, ARRAYSIZE(ldrPath));
    bool haveReal = GetRealModulePath(hMod, realPath, ARRAYSIZE(realPath));
    bool masked = (n1 && haveReal && _wcsicmp(ldrPath, realPath) != 0);

    if (masked) Wh_Log(L"Masked copy %p LDR=[%s] real=[%s]", hMod, ldrPath, realPath);
    else Wh_Log(L"Hooking %p LDR=[%s] real=[%s]", hMod, n1 ? ldrPath : L"?", haveReal ? realPath : L"?");

    bool ok = masked ? InstallMaskedCustom(hMod, slot) : InstallViaWindhawk(hMod, slot);
    if (!ok) {
        if (masked) {
            Wh_Log(L"Custom failed, trying Windhawk for %p", hMod);
            ok = InstallViaWindhawk(hMod, slot);
        } else if (haveReal) {
            Wh_Log(L"Windhawk failed, trying custom for %p", hMod);
            ok = InstallMaskedCustom(hMod, slot);
        }
    }

    if (!ok) {
        Wh_Log(L"All symbol hooks failed for %p, fallback subclassing covers UI", hMod);
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        g_efSlots[slot].inUse = false;
        g_efSlots[slot].hMod = nullptr;
        return false;
    }
    Wh_Log(L"Hooked %p slot %d wnd=%p init=%p flags=%p", hMod, slot,
        g_efSlots[slot].wndProcOrig, g_efSlots[slot].initOrig, g_efSlots[slot].setFlagsOrig);
    return true;
}

void HookAllLoadedExplorerFrameCopies()
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (hSnap == INVALID_HANDLE_VALUE) return;
    MODULEENTRY32W me; me.dwSize = sizeof(me);
    std::vector<HMODULE> efMods;
    if (Module32FirstW(hSnap, &me)) {
        do {
            if (!lstrcmpiW(me.szModule, L"explorerframe.dll") || !lstrcmpiW(me.szModule, L"explorerframe"))
                efMods.push_back(me.hModule);
        } while (Module32NextW(hSnap, &me));
    }
    CloseHandle(hSnap);

    {
        std::lock_guard<std::mutex> lock(g_efSlotsMutex);
        for (int i = 0; i < kMaxEFSlots; i++) if (g_efSlots[i].inUse) {
            bool found = false;
            for (HMODULE h : efMods) if (h == g_efSlots[i].hMod) { found = true; break; }
            if (!found) {
                Wh_Log(L"Free slot %d (%p gone)", i, g_efSlots[i].hMod);
                g_efSlots[i].inUse = false;
                g_efSlots[i].hMod = nullptr;
                g_efSlots[i].initOrig = nullptr;
                g_efSlots[i].wndProcOrig = nullptr;
                g_efSlots[i].setFlagsOrig = nullptr;
            }
        }
    }
    if (!efMods.empty())
        Wh_Log(L"Found %zu explorerframe copies", efMods.size());
    for (HMODULE h : efMods) InstallExplorerFrameHooks(h);
}

// ---------------------------------------------------------------------------
// load triggers: only SetEvent, heavy work in worker (no loader-lock deadlock)
// ---------------------------------------------------------------------------
typedef LONG (WINAPI *LdrLoadDll_t)(PWSTR SearchPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* BaseAddress);
LdrLoadDll_t pOriginalLdrLoadDll = nullptr;

LONG WINAPI LdrLoadDll_Hook(PWSTR SearchPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* BaseAddress)
{
    LONG st = pOriginalLdrLoadDll(SearchPath, DllCharacteristics, DllName, BaseAddress);
    if (st >= 0 && BaseAddress && *BaseAddress && DllName && DllName->Buffer) {
        if (IsExplorerFrameUnicodeString(DllName)) {
            if (g_hookEvent) SetEvent(g_hookEvent);
        }
    }
    return st;
}

typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibraryW_t)(LPCWSTR);
typedef HMODULE (WINAPI *LoadLibraryExA_t)(LPCSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibraryA_t)(LPCSTR);
LoadLibraryExW_t pOriginalLoadLibraryExW = nullptr;
LoadLibraryW_t pOriginalLoadLibraryW = nullptr;
LoadLibraryExA_t pOriginalLoadLibraryExA = nullptr;
LoadLibraryA_t pOriginalLoadLibraryA = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR n, HANDLE f, DWORD fl)
{
    HMODULE r = pOriginalLoadLibraryExW(n, f, fl);
    if (r && IsExplorerFrameDllNameW(n) && g_hookEvent) SetEvent(g_hookEvent);
    return r;
}
HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR n)
{
    HMODULE r = pOriginalLoadLibraryW(n);
    if (r && IsExplorerFrameDllNameW(n) && g_hookEvent) SetEvent(g_hookEvent);
    return r;
}
HMODULE WINAPI LoadLibraryExA_Hook(LPCSTR n, HANDLE f, DWORD fl)
{
    HMODULE r = pOriginalLoadLibraryExA(n, f, fl);
    if (r && IsExplorerFrameDllNameA(n) && g_hookEvent) SetEvent(g_hookEvent);
    return r;
}
HMODULE WINAPI LoadLibraryA_Hook(LPCSTR n)
{
    HMODULE r = pOriginalLoadLibraryA(n);
    if (r && IsExplorerFrameDllNameA(n) && g_hookEvent) SetEvent(g_hookEvent);
    return r;
}

void* GetKernelProc(const char* name)
{
    HMODULE hb = GetModuleHandleW(L"kernelbase.dll");
    if (hb) { void* p = (void*)GetProcAddress(hb, name); if (p) return p; }
    HMODULE h32 = GetModuleHandleW(L"kernel32.dll");
    if (h32) { void* p = (void*)GetProcAddress(h32, name); if (p) return p; }
    return nullptr;
}

// ---------------------------------------------------------------------------
// CreateWindow fallback (version independent)
// ---------------------------------------------------------------------------
typedef HWND (WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t pOriginalCreateWindowExW = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
    HWND hwnd = pOriginalCreateWindowExW(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd) {
        wchar_t buf[64];
        if (GetClassNameW(hwnd, buf, 64)) {
            if (!lstrcmpW(buf, L"CabinetWClass") || !lstrcmpW(buf, L"ExploreWClass"))
                EnsureCabinetSubclassed(hwnd);
            else if (!lstrcmpW(buf, L"ReBarWindow32"))
                EnsureRebarSubclassed(hwnd);
        }
    }
    return hwnd;
}

BOOL CALLBACK EnumChildForRebar(HWND h, LPARAM)
{
    wchar_t cls[64];
    if (GetClassNameW(h, cls, 64) && !lstrcmpW(cls, L"ReBarWindow32"))
        EnsureRebarSubclassed(h);
    return TRUE;
}
BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM)
{
    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64)) return TRUE;
    if (!lstrcmpW(cls, L"CabinetWClass") || !lstrcmpW(cls, L"ExploreWClass")) {
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
// shell32 / dui70 / registry (unchanged, copy-independent)
// ---------------------------------------------------------------------------
typedef long (*__cdecl CLVHCreateControl_t)(void*, HWND, void*, void*);
CLVHCreateControl_t CLVHCreateControlOriginal = nullptr;

long __cdecl CLVHCreateControlHook(void* pThis, HWND hWnd, void* a, void* b)
{
    long ret = CLVHCreateControlOriginal(pThis, hWnd, a, b);
    HWND lv = GetWindow(hWnd, GW_CHILD);
    if (lv) {
        wchar_t name[32]; GetClassNameW(lv, name, 32);
        if (!lstrcmpW(name, L"SysListView32")) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(lv, ListviewSubclassProc, NULL);
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassedListviews.push_back(lv);
        }
    }
    return ret;
}

typedef VOID(__cdecl *Element_PaintBgT)(class Element*, HDC, class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
Element_PaintBgT Element_PaintBg = nullptr;

VOID __cdecl Element_PaintBgHook(class Element* This, HDC hdc, class Value* value, LPRECT pRect, LPRECT pClip, LPRECT pEx, LPRECT pTarg)
{
    if (!value || !pRect) { Element_PaintBg(This, hdc, value, pRect, pClip, pEx, pTarg); return; }
    if (g_settingControlPanelColorFix) {
        if ((int)(*(DWORD*)value << 26) >> 26 != 9) {
            auto v44 = *((unsigned __int64*)value + 1);
            if (((v44 + 20) & 7) == 4) {
                HWND wnd = WindowFromDC(hdc);
                HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
                if (hTh) {
                    COLORREF c;
                    if (SUCCEEDED(GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &c))) {
                        HBRUSH br = CreateSolidBrush(c);
                        FillRect(hdc, pRect, br);
                        DeleteObject(br);
                    }
                    CloseThemeData(hTh);
                }
                return;
            }
        }
    }
    Element_PaintBg(This, hdc, value, pRect, pClip, pEx, pTarg);
}

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR v, LPDWORD r, LPDWORD t, LPBYTE d, LPDWORD cb)
{
    if (v && !lstrcmpiW(v, L"AlwaysShowMenus")) {
        if (d && !cb) return ERROR_INVALID_PARAMETER;
        if (t) *t = REG_DWORD;
        if (cb) {
            DWORD buf = *cb; *cb = sizeof(DWORD);
            if (d) {
                if (buf < sizeof(DWORD)) return ERROR_MORE_DATA;
                *(DWORD*)d = g_settingDisplayMenuBar;
            }
        }
        return ERROR_SUCCESS;
    }
    return pOriginalRegQueryValueExW(hKey, v, r, t, d, cb);
}

// ---------------------------------------------------------------------------
DWORD WINAPI HookWorkerThreadProc(LPVOID)
{
    while (!g_stopWorker) {
        WaitForSingleObject(g_hookEvent, 3000);
        if (g_stopWorker) break;
        HookAllLoadedExplorerFrameCopies();
        SubclassExistingWindows();
    }
    return 0;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer Frame Fixes v5 init");
    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
    g_settingControlPanelColorFix = Wh_GetIntSetting(L"ControlPanelColorFix");

    g_hookEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

    HMODULE hInit = LoadLibraryW(L"explorerframe.dll");
    if (hInit) InstallExplorerFrameHooks(hInit);
    HookAllLoadedExplorerFrameCopies();

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) return FALSE;
    WindhawkUtils::SYMBOL_HOOK sh[] = {
        { { L"public: virtual long __cdecl CListViewHost::CreateControl(struct HWND__ *,struct IListControlHost *,struct IViewSettings *)" },
          (void**)&CLVHCreateControlOriginal, (void*)CLVHCreateControlHook, FALSE }
    };
    if (!WindhawkUtils::HookSymbols(hShell32, sh, ARRAYSIZE(sh))) return FALSE;

    HMODULE hDui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hDui70) {
        WindhawkUtils::SYMBOL_HOOK dh = {
            { L"public: void __cdecl DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)" },
            (void**)&Element_PaintBg, (void*)Element_PaintBgHook, false
        };
        WindhawkUtils::HookSymbols(hDui70, &dh, 1);
    }

    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "RegQueryValueExW"),
        (void*)RegQueryValueExWHook, (void**)&pOriginalRegQueryValueExW);

    void* pCW = (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW");
    if (pCW) Wh_SetFunctionHook(pCW, (void*)CreateWindowExW_Hook, (void**)&pOriginalCreateWindowExW);
    SubclassExistingWindows();

    void* pLdr = (void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "LdrLoadDll");
    if (pLdr) Wh_SetFunctionHook(pLdr, (void*)LdrLoadDll_Hook, (void**)&pOriginalLdrLoadDll);

    void* pExW = GetKernelProc("LoadLibraryExW");
    if (pExW) Wh_SetFunctionHook(pExW, (void*)LoadLibraryExW_Hook, (void**)&pOriginalLoadLibraryExW);
    void* pW = GetKernelProc("LoadLibraryW");
    if (pW) Wh_SetFunctionHook(pW, (void*)LoadLibraryW_Hook, (void**)&pOriginalLoadLibraryW);
    void* pExA = GetKernelProc("LoadLibraryExA");
    if (pExA) Wh_SetFunctionHook(pExA, (void*)LoadLibraryExA_Hook, (void**)&pOriginalLoadLibraryExA);
    void* pA = GetKernelProc("LoadLibraryA");
    if (pA) Wh_SetFunctionHook(pA, (void*)LoadLibraryA_Hook, (void**)&pOriginalLoadLibraryA);

    g_workerThread = CreateThread(NULL, 0, HookWorkerThreadProc, NULL, 0, NULL);

    HookAllLoadedExplorerFrameCopies();
    SubclassExistingWindows();
    return TRUE;
}

void Wh_ModUninit()
{
    g_stopWorker = TRUE;
    if (g_hookEvent) SetEvent(g_hookEvent);
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }
    if (g_hookEvent) { CloseHandle(g_hookEvent); g_hookEvent = NULL; }

    std::vector<HWND> rb, lv, cab;
    { std::lock_guard<std::mutex> lock(g_subclassedMutex);
      rb.swap(g_subclassedRebars); lv.swap(g_subclassedListviews); cab.swap(g_subclassedCabinets); }
    for (HWND h : rb) WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, RebarSubclassProc);
    for (HWND h : lv) WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, ListviewSubclassProc);
    for (HWND h : cab) WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, CabinetSubclassProc);
}

void Wh_ModSettingsChanged()
{
    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
    g_settingControlPanelColorFix = Wh_GetIntSetting(L"ControlPanelColorFix");
}
