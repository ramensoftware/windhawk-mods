// ==WindhawkMod==
// @id              taskbar-autohide-better
// @name            Better Taskbar Autohide
// @description     Allow taskbar autohide when inactive window notified
// @version         1.0
// @author          Cirn09
// @github          https://github.com/Cirn09
// @homepage        https://blog.cirn09.xyz/
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Better Taskbar Autohide
Windows taskbar is forced to show at the top when there are notifications for inactive windows and auto-hide enabled, which can cover something.
This module allows the taskbar to be shown for a while and then re-hidden.
![](https://imgur.com/BNyrMQc.gif)
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- delay: 1500
*/
// ==/WindhawkModSettings==

#include <minwindef.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <cstdint>
#include <string>
#include <string_view>

using WndProc_t = LRESULT(__thiscall*)(void* pThis,
                                       HWND hWnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam);
using PermitAutoHide_t = BOOL(__thiscall*)(void* pThis);

WndProc_t WndProc_o = nullptr;
PermitAutoHide_t PermitAutoHide_o = nullptr;
FILETIME lastNotifyTime = {0};
int delayMile_g;

static inline uint64_t Filetime2U64(PFILETIME pFiletime) {
    return ((uint64_t)pFiletime->dwHighDateTime << 32) |
           (pFiletime->dwLowDateTime);
}

__thiscall LRESULT WndProc_hook(void* pThis,
                                HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    if (msg == 0xc028 && wParam == 0x8006) {
        GetSystemTimeAsFileTime(&lastNotifyTime);
    }
    return WndProc_o(pThis, hWnd, msg, wParam, lParam);
}

__thiscall BOOL PermitAutoHide_hook(void* pThis) {
    FILETIME currentTime;
    GetSystemTimeAsFileTime(&currentTime);
    if (Filetime2U64(&currentTime) - Filetime2U64(&lastNotifyTime) >
        delayMile_g)
        return TRUE;
    else
        return FALSE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);
    WH_FIND_SYMBOL f;
    HMODULE hExplorer = LoadLibrary(L"explorer.exe");
    delayMile_g = Wh_GetIntSetting(L"delay") * 10 * 1000;  // 100-nanosecond

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {{
          L"protected: virtual __int64 __cdecl CTaskBand::v_WndProc("
           L"struct HWND__ *,unsigned int,unsigned __int64,__int64)",
          L"protected: virtual long __thiscall CTaskBand::v_WndProc("
           L"struct HWND__ *,unsigned int,unsigned int,long)",
         },
         (void**)&WndProc_o,
         (void*)WndProc_hook},
        {{
          L"public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void)",
          L"public: virtual int __stdcall CTaskListWnd::PermitAutoHide(void)",
         },
         (void**)&PermitAutoHide_o,
         (void*)PermitAutoHide_hook}};

    return HookSymbols(hExplorer, symbolHooks, ARRAYSIZE(symbolHooks));
}

void Wh_ModSettingsChanged() {
    delayMile_g = Wh_GetIntSetting(L"delay") * 10 * 1000;  // 100-nanosecond
}
