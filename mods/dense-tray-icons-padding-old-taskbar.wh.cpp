// ==WindhawkMod==
// @id dense-tray-icons-padding-old-taskbar
// @name Adjustable tray icons padding for Win10 taskbar
// @description Makes padding of tray icons adjustable in legacy taskbar
// @version 1.0
// @author anixx
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

static int g_padding;

thread_local bool g_pending = false;
thread_local BYTE g_prevSupported = 0;
thread_local BYTE g_prevValid = 0;
thread_local LONG_PTR g_lpTrayNotify = 0;

static ULONG_PTR g_expBase = 0, g_expEnd = 0;

using GetSystemMetrics_t = decltype(&GetSystemMetrics);
GetSystemMetrics_t GetSystemMetrics_Orig;

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

int WINAPI GetSystemMetrics_Hook(int nIndex) {
    int r = GetSystemMetrics_Orig(nIndex);
    if (nIndex == SM_CXSMICON && g_pending && IsFromExplorer()) {
        g_pending = false;
        if (g_lpTrayNotify) {
            int off = GetPtrDevOffset();
            BYTE* pSup = (BYTE*)(g_lpTrayNotify + off);
            BYTE* pValid = (BYTE*)(g_lpTrayNotify + off + 1);
            *pSup = g_prevSupported;
            *pValid = g_prevValid;
            g_lpTrayNotify = 0;
        }
        return (r + g_padding) / 2;
    }
    return r;
}

LRESULT CALLBACK NewTrayToolbarProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR ref)
{
    switch(uMsg) {
    case TB_GETBUTTONSIZE: {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (g_padding!= 0) {
            HWND hTrayNotify = nullptr;
            HWND p = GetParent(hWnd);
            while(p){
                wchar_t cls[64]; GetClassNameW(p, cls, 64);
                if (!wcscmp(cls, L"TrayNotifyWnd")) { hTrayNotify = p; break; }
                p = GetParent(p);
            }
            if (!hTrayNotify) {
                HWND hShell = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (hShell) hTrayNotify = FindWindowExW(hShell, nullptr, L"TrayNotifyWnd", nullptr);
            }
            if (hTrayNotify) {
                LONG_PTR lp = GetWindowLongPtrW(hTrayNotify, 0);
                if (lp) {
                    int off = GetPtrDevOffset();
                    BYTE* pSup = (BYTE*)(lp + off);
                    BYTE* pValid = (BYTE*)(lp + off + 1);
                    g_prevSupported = *pSup;
                    g_prevValid = *pValid;
                    g_lpTrayNotify = lp;
                    *pSup = 1;
                    *pValid = 1;
                }
            }
            g_pending = true;
        }
        return res;
    }
    case TB_SETPADDING: {
        lParam &= ~0xFFFF;
        lParam |= ((g_padding / 2 * 2) & 0xFFFF);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD st, int X,int Y,int W,int H,HWND par,HMENU menu,HINSTANCE inst,LPVOID lp) {
    HWND h = CreateWindowExW_Orig(ex,cls,name,st,X,Y,W,H,par,menu,inst,lp);
    if (!h) return h;
    if (((ULONG_PTR)cls & ~(ULONG_PTR)0xffff)!=0) {
        if (!wcscmp(cls, L"TrayNotifyWnd")) {
            HWND hPager = FindWindowExW(h, nullptr, L"SysPager", nullptr);
            if (hPager) {
                HWND hTB = FindWindowExW(hPager, nullptr, L"ToolbarWindow32", nullptr);
                if (hTB) SetWindowSubclass(hTB, NewTrayToolbarProc, 0, 0);
            }
        } else if (!wcscmp(cls, L"ToolbarWindow32")) {
            HWND p = par;
            while(p){
                wchar_t b[64]; GetClassNameW(p,b,64);
                if (!wcscmp(b, L"TrayNotifyWnd") ||!wcscmp(b, L"SysPager")) { SetWindowSubclass(h, NewTrayToolbarProc, 0, 0); break; }
                p = GetParent(p);
            }
        }
    }
    return h;
}

void RemoveOurSubclass() {
    HWND hShell = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hShell) return;
    HWND hTrayNotify = FindWindowExW(hShell, nullptr, L"TrayNotifyWnd", nullptr);
    if (!hTrayNotify) return;
    HWND hPager = FindWindowExW(hTrayNotify, nullptr, L"SysPager", nullptr);
    if (hPager) {
        HWND hTB = FindWindowExW(hPager, nullptr, L"ToolbarWindow32", nullptr);
        if (hTB) {
            RemoveWindowSubclass(hTB, NewTrayToolbarProc, 0);
            RemoveWindowSubclass(hTB, NewTrayToolbarProc, 1);
        }
    }
    HWND hTBdirect = FindWindowExW(hTrayNotify, nullptr, L"ToolbarWindow32", nullptr);
    if (hTBdirect) {
        RemoveWindowSubclass(hTBdirect, NewTrayToolbarProc, 0);
        RemoveWindowSubclass(hTBdirect, NewTrayToolbarProc, 1);
    }
}

BOOL Wh_ModInit(void){
    g_padding = Wh_GetIntSetting(L"padding");
    HMODULE hMod = GetModuleHandleW(nullptr);
    MODULEINFO mi{}; GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi));
    g_expBase = (ULONG_PTR)mi.lpBaseOfDll;
    g_expEnd = g_expBase + mi.SizeOfImage;

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);
    Wh_SetFunctionHook((void*)GetSystemMetrics, (void*)GetSystemMetrics_Hook, (void**)&GetSystemMetrics_Orig);
    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_lpTrayNotify && g_pending) {
        int off = GetPtrDevOffset();
        BYTE* pSup = (BYTE*)(g_lpTrayNotify + off);
        BYTE* pValid = (BYTE*)(g_lpTrayNotify + off + 1);
        *pSup = g_prevSupported;
        *pValid = g_prevValid;
    }
    g_pending = false;
    g_lpTrayNotify = 0;
    RemoveOurSubclass();
}
