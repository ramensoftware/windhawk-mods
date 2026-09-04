// ==WindhawkMod==
// @id dense-tray-icons-padding-old-taskbar
// @name Adjustable tray icons padding for Win10 taskbar
// @description Makes padding of tray icons adjustable in legacy taskbar
// @version 1.0
// @author Anixx
// @github          https://github.com/Anixx
// @include explorer.exe
// @compilerOptions -lcomctl32 -lpsapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Dense tray icons padding for Win10 taskbar

This is a port of functionality of 7+ Taskbar Tweaker that allows to adjust the padding of the tray icons
in legacy (Win10) taskbar (running either under Windows 10 or Windows 11).

![screenshot](https://i.imgur.com/NjGhXuc.png)

The default padding value is set to 2, the same as under Windows 95-Windows XP, more dense than under Windows 7, but can be changed in mod's options.

Explorer should be restarted after enabling this mod.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- padding: 2
  $name: Tray icon padding
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <psapi.h>
#include <vector>
#include <algorithm>
#include <windhawk_utils.h>

static int g_padding;
static UINT g_wmPrivRestore = 0;
static ULONG_PTR g_expBase = 0, g_expEnd = 0;

using GetSystemMetrics_t = decltype(&GetSystemMetrics);
GetSystemMetrics_t GetSystemMetrics_Orig = nullptr;
using GetSystemMetricsForDpi_t = decltype(&GetSystemMetricsForDpi);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_Orig = nullptr;

inline bool IsFromExplorer() {
    void* ret = __builtin_return_address(0);
    ULONG_PTR a = (ULONG_PTR)ret;
    return a >= g_expBase && a < g_expEnd;
}
inline int GetPtrDevOffset() {
#ifdef _WIN64
    return 0x168;
#else
    return 0xD0;
#endif
}

struct TrayEntry {
    HWND hTB = nullptr;
    LONG_PTR lpTrayNotify = 0;
    BYTE prevSupported = 0;
    BYTE prevValid = 0;
    bool pending = false;
};
static std::vector<TrayEntry> g_entries;
static CRITICAL_SECTION g_cs;

static TrayEntry* FindEntryByTb(HWND hTB) {
    for (auto &e : g_entries) if (e.hTB == hTB) return &e;
    return nullptr;
}

inline bool IsWindowInCurrentProcess(HWND hWnd) {
    if (!hWnd) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}

inline HWND FindTrayNotifyForToolbar(HWND hTB) {
    HWND p = GetParent(hTB);
    while (p) {
        wchar_t cls[64];
        if (GetClassNameW(p, cls, 64) && wcscmp(cls, L"TrayNotifyWnd")==0) {
            return IsWindowInCurrentProcess(p)? p : nullptr;
        }
        p = GetParent(p);
    }
    return nullptr;
}

inline bool IsReadablePointer(void* ptr, SIZE_T size) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(ptr, &mbi, sizeof(mbi))==0) return false;
    if (mbi.State!= MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_GUARD|PAGE_NOACCESS)) return false;
    // check if range stays inside same region
    SIZE_T offset = (BYTE*)ptr - (BYTE*)mbi.BaseAddress;
    return offset + size <= mbi.RegionSize;
}

inline bool GetTrayFlagsPointers(LONG_PTR lp, BYTE** ppSup, BYTE** ppValid) {
    if (lp < 0x10000) return false;
    int off = GetPtrDevOffset();
    BYTE* pSup = (BYTE*)(lp + off);
    BYTE* pValid = (BYTE*)(lp + off + 1);
    if (!IsReadablePointer(pSup, 2)) return false;
    *ppSup = pSup;
    *ppValid = pValid;
    return true;
}

static void RestoreEntry(TrayEntry &e) {
    if (!e.pending ||!e.lpTrayNotify) return;
    BYTE *pSup=nullptr,*pValid=nullptr;
    if (!GetTrayFlagsPointers(e.lpTrayNotify, &pSup, &pValid)) return;
    *pSup = e.prevSupported;
    *pValid = e.prevValid;
    e.pending = false;
    e.lpTrayNotify = 0;
}

// WindhawkUtils subclass proc = 5 args, no uId
LRESULT CALLBACK NewTrayToolbarProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRef)
{
    if (uMsg == g_wmPrivRestore) {
        EnterCriticalSection(&g_cs);
        if (auto* e = FindEntryByTb(hWnd)) RestoreEntry(*e);
        LeaveCriticalSection(&g_cs);
        return 0;
    }
    if (uMsg == WM_NCDESTROY) {
        EnterCriticalSection(&g_cs);
        if (auto* e = FindEntryByTb(hWnd)) {
            if (e->pending) RestoreEntry(*e);
        }
        g_entries.erase(std::remove_if(g_entries.begin(), g_entries.end(),
            [&](const TrayEntry& x){ return x.hTB == hWnd; }), g_entries.end());
        LeaveCriticalSection(&g_cs);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == TB_GETBUTTONSIZE) {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (g_padding!=0) {
            HWND hTrayNotify = FindTrayNotifyForToolbar(hWnd);
            if (hTrayNotify) {
                LONG_PTR lp = GetWindowLongPtrW(hTrayNotify, 0);
                if (lp) {
                    BYTE *pSup=nullptr,*pValid=nullptr;
                    if (!GetTrayFlagsPointers(lp, &pSup, &pValid)) {
                        Wh_Log(L"Invalid TrayNotify pointer %p", (void*)lp);
                    } else {
                        BYTE curSup = *pSup;
                        BYTE curValid = *pValid;
                        EnterCriticalSection(&g_cs);
                        TrayEntry* e = FindEntryByTb(hWnd);
                        if (!e) {
                            TrayEntry ne; ne.hTB = hWnd;
                            g_entries.push_back(ne);
                            e = &g_entries.back();
                        }
                        bool alreadyForced = e->pending && e->lpTrayNotify==lp;
                        if (!alreadyForced) {
                            if (curSup>1 || curValid>1) {
                                Wh_Log(L"CTrayNotify flags sanity check failed: %d %d at offset 0x%X lp=%p - skipping", curSup, curValid, GetPtrDevOffset(), (void*)lp);
                                LeaveCriticalSection(&g_cs);
                                return res;
                            }
                            e->prevSupported = curSup;
                            e->prevValid = curValid;
                            e->lpTrayNotify = lp;
                            e->pending = true;
                        }
                        *pSup = 1;
                        *pValid = 1;
                        LeaveCriticalSection(&g_cs);
                    }
                }
            }
        }
        return res;
    }
    if (uMsg == TB_SETPADDING) {
        lParam &= ~0xFFFF;
        lParam |= ((g_padding / 2 * 2) & 0xFFFF);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SubclassToolbar(HWND hTB) {
    if (!hTB ||!IsWindowInCurrentProcess(hTB)) return;
    EnterCriticalSection(&g_cs);
    bool already = FindEntryByTb(hTB)!=nullptr;
    LeaveCriticalSection(&g_cs);
    if (already) return;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hTB, NewTrayToolbarProc, 0);
    EnterCriticalSection(&g_cs);
    if (!FindEntryByTb(hTB)) {
        TrayEntry e; e.hTB=hTB;
        g_entries.push_back(e);
    }
    LeaveCriticalSection(&g_cs);
}

static void TrySubclassTree(HWND hTrayNotify) {
    if (!hTrayNotify ||!IsWindowInCurrentProcess(hTrayNotify)) return;
    HWND hPager = FindWindowExW(hTrayNotify, nullptr, L"SysPager", nullptr);
    if (hPager) {
        HWND hTB = FindWindowExW(hPager, nullptr, L"ToolbarWindow32", nullptr);
        if (hTB) SubclassToolbar(hTB);
    }
    HWND hTBdirect = FindWindowExW(hTrayNotify, nullptr, L"ToolbarWindow32", nullptr);
    if (hTBdirect) SubclassToolbar(hTBdirect);
}

static BOOL CALLBACK EnumSecondaryTrayWndProc(HWND hWnd, LPARAM lParam) {
    wchar_t cls[64];
    if (GetClassNameW(hWnd, cls, 64) && wcscmp(cls, L"Shell_SecondaryTrayWnd")==0) {
        if (!IsWindowInCurrentProcess(hWnd)) return TRUE;
        HWND hTrayNotify = FindWindowExW(hWnd, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotify) TrySubclassTree(hTrayNotify);
    }
    return TRUE;
}

static void FindAndSubclassAll() {
    HWND hShell = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hShell && IsWindowInCurrentProcess(hShell)) {
        HWND hTrayNotify = FindWindowExW(hShell, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotify) TrySubclassTree(hTrayNotify);
    }
    EnumWindows(EnumSecondaryTrayWndProc, 0);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD st, int X,int Y,int W,int H,HWND par,HMENU menu,HINSTANCE inst,LPVOID lp) {
    HWND h = CreateWindowExW_Orig(ex,cls,name,st,X,Y,W,H,par,menu,inst,lp);
    if (!h) return h;
    wchar_t winCls[64];
    if (!GetClassNameW(h, winCls, 64)) return h;
    if (wcscmp(winCls, L"ToolbarWindow32")==0) {
        HWND p = par;
        while (p) {
            wchar_t b[64];
            if (!GetClassNameW(p, b, 64)) break;
            if (wcscmp(b, L"TrayNotifyWnd")==0 || wcscmp(b, L"SysPager")==0 ||
                wcscmp(b, L"Shell_TrayWnd")==0 || wcscmp(b, L"Shell_SecondaryTrayWnd")==0) {
                SubclassToolbar(h);
                break;
            }
            p = GetParent(p);
        }
    }
    return h;
}

static int HandleMetricsResult(int r) {
    EnterCriticalSection(&g_cs);
    for (auto &e : g_entries) {
        if (e.pending) RestoreEntry(e);
    }
    LeaveCriticalSection(&g_cs);
    return (r + g_padding) / 2;
}

int WINAPI GetSystemMetrics_Hook(int nIndex) {
    int r = GetSystemMetrics_Orig(nIndex);
    if (nIndex==SM_CXSMICON && g_padding!=0 && IsFromExplorer()) {
        bool hasPending=false;
        EnterCriticalSection(&g_cs);
        for (auto &e : g_entries) if (e.pending) { hasPending=true; break; }
        LeaveCriticalSection(&g_cs);
        if (hasPending) return HandleMetricsResult(r);
    }
    return r;
}
int WINAPI GetSystemMetricsForDpi_Hook(int nIndex, UINT dpi) {
    int r = GetSystemMetricsForDpi_Orig? GetSystemMetricsForDpi_Orig(nIndex, dpi) : GetSystemMetrics_Orig(nIndex);
    if (nIndex==SM_CXSMICON && g_padding!=0 && IsFromExplorer()) {
        bool hasPending=false;
        EnterCriticalSection(&g_cs);
        for (auto &e : g_entries) if (e.pending) { hasPending=true; break; }
        LeaveCriticalSection(&g_cs);
        if (hasPending) return HandleMetricsResult(r);
    }
    return r;
}

void RemoveOurSubclass() {
    std::vector<HWND> toRemove;
    EnterCriticalSection(&g_cs);
    for (auto &e : g_entries) toRemove.push_back(e.hTB);
    LeaveCriticalSection(&g_cs);
    for (HWND hTB : toRemove) {
        if (hTB && IsWindow(hTB)) {
            SendMessageW(hTB, g_wmPrivRestore, 0, 0);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTB, NewTrayToolbarProc);
        }
    }
    EnterCriticalSection(&g_cs);
    g_entries.clear();
    LeaveCriticalSection(&g_cs);
}

BOOL Wh_ModInit(void){
    InitializeCriticalSection(&g_cs);
    g_wmPrivRestore = RegisterWindowMessageW(L"Anixx_DenseTray_Restore_7B1F2A");
    g_padding = Wh_GetIntSetting(L"padding");
    HMODULE hMod = GetModuleHandleW(nullptr);
    MODULEINFO mi{}; GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi));
    g_expBase = (ULONG_PTR)mi.lpBaseOfDll;
    g_expEnd = g_expBase + mi.SizeOfImage;
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);
    Wh_SetFunctionHook((void*)GetSystemMetrics, (void*)GetSystemMetrics_Hook, (void**)&GetSystemMetrics_Orig);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pForDpi = (void*)GetProcAddress(hUser32, "GetSystemMetricsForDpi");
        if (pForDpi) Wh_SetFunctionHook(pForDpi, (void*)GetSystemMetricsForDpi_Hook, (void**)&GetSystemMetricsForDpi_Orig);
    }
    FindAndSubclassAll();
    return TRUE;
}
void Wh_ModUninit(void) {
    RemoveOurSubclass();
    DeleteCriticalSection(&g_cs);
}
