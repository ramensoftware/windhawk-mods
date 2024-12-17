// ==WindhawkMod==
// @id                  taskbar-autohide-better
// @name                Better Taskbar Autohide
// @description         Allow taskbar autohide when inactive window notified
// @description:zh-CN   非活动窗口有通知时，任务栏仍然可以自动隐藏
// @version             1.2
// @author              Cirn09
// @github              https://github.com/Cirn09
// @homepage            https://blog.cirn09.xyz/
// @include             explorer.exe
// @compilerOptions     -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Better Taskbar Autohide
Windows taskbar is forced to show at the top when there are notifications for
inactive windows and auto-hide enabled, which can cover something. This module
allows the taskbar to be shown for a while and then re-hidden.

Supports Windows 10 and Windows 11, but due to Windows 11 taskbar design,
Hiding the taskbar when there is a notification will make the window
notification completely invisible (at least an orange line can be seen on screen
edge under Windows 10)

![](https://i.imgur.com/BNyrMQc.gif)

---

原版的任务栏在启用了自动隐藏时，非活动窗口有通知时，任务栏会强制显示在最上层，
这会挡住一些窗口。这个模块可以让任务栏显示一段时间后重新隐藏。

支持 Windows 10 和 Windows 11，但是由于 Windows 11 的 任务栏设计问题，
有通知时隐藏任务栏会让人完全注意不到窗口通知（Windows 10
下至少可以看到一条橙色线）
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
enum class ExplorerVersion {
    Unsupported,
    Win10,
    Win11,
};

WndProc_t WndProc_o = nullptr;
PermitAutoHide_t PermitAutoHide_o = nullptr;
FILETIME lastNotifyTime = {0};
int delayMile_g;
ExplorerVersion explorerVer_g;

inline uint64_t Filetime2U64(PFILETIME pFiletime) {
    return ((uint64_t)pFiletime->dwHighDateTime << 32) |
           (pFiletime->dwLowDateTime);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

ExplorerVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo)
        return ExplorerVersion::Unsupported;

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Exploer version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000)
                return ExplorerVersion::Win10;
            else
                return ExplorerVersion::Win11;
            break;
    }

    return ExplorerVersion::Unsupported;
}

__thiscall LRESULT WndProc_hook(void* pThis,
                                HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    static UINT notificationMsgId =
        explorerVer_g == ExplorerVersion::Win10 ? 0xc028 : 0xc029;
    if (msg == notificationMsgId && wParam == 0x8006) {
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
        return PermitAutoHide_o(pThis);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);
    WH_FIND_SYMBOL f;
    HMODULE hModule;
    delayMile_g = Wh_GetIntSetting(L"delay") * 10 * 1000;  // 100-nanosecond
    explorerVer_g = GetExplorerVersion();

    switch (explorerVer_g) {
        case ExplorerVersion::Win10:
            hModule = LoadLibrary(L"explorer.exe");
            break;
        case ExplorerVersion::Win11:
            hModule = LoadLibrary(L"taskbar.dll");
            break;

        case ExplorerVersion::Unsupported:
        default:
            Wh_Log(L"Unsupported Explorer version");
            return FALSE;
    }

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

    return HookSymbols(hModule, symbolHooks, ARRAYSIZE(symbolHooks));
}

void Wh_ModSettingsChanged() {
    delayMile_g = Wh_GetIntSetting(L"delay") * 10 * 1000;  // 100-nanosecond
}
