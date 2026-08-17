// ==WindhawkMod==
// @id              valinet-systray-sound-volume
// @name            Systray Sound Volume
// @description     Change system volume by scrolling over the sound icon.
// @version         0.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Change system volume by scrolling over the sound icon. 

This is a re-submission of the mod originally created by Valinet (Valentin Radu, the author of Explorer Patcher).
The original location of the mod:
https://github.com/valinet/wh-mods/blob/master/mods/valinet-systray-sound-volume.wh.cpp

License: [GPL-2.0](https://raw.githubusercontent.com/valinet/wh-mods/refs/heads/master/LICENSE)

This mod supports both Windows 10 and Windows 11 taskbars.
*/
// ==/WindhawkModReadme==

#include <thread>
#include <processthreadsapi.h>
#include <sysinfoapi.h>
#include <windhawk_utils.h>

#define UID_TRAYICONVOLUME  100
#define GUID_TRAYICONVOLUME { 0x7820AE73, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C } };

#define UID_TRAYICONPOWER  1225
#define GUID_TRAYICONPOWER { 0x7820AE75, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C } };

#define UID_TRAYICONHOTPLUG  1226
#define GUID_TRAYICONHOTPLUG { 0x7820AE78, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C } };

bool subclassed = false;
HWND hWnd = nullptr;

LRESULT WINAPI SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEHWHEEL) {
        NOTIFYICONIDENTIFIER nid {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = UID_TRAYICONVOLUME;
        nid.guidItem = GUID_TRAYICONVOLUME;
        RECT rc{};
        POINT pt{};
        GetCursorPos(&pt);
        if (SUCCEEDED(Shell_NotifyIconGetRect(&nid, &rc)) && PtInRect(&rc, pt)) {
            int factor = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
            int normFactor = factor < 0 ? -factor : factor;
            for (int i = 0; i < normFactor; ++i) {
                WPARAM wParam = (WPARAM)FindWindowW(L"ProgMan", NULL);
                LPARAM lParam = factor > 0 ? APPCOMMAND_VOLUME_UP : APPCOMMAND_VOLUME_DOWN;
                SendMessageW(reinterpret_cast<HWND>(wParam), WM_APPCOMMAND, wParam, lParam * 65536);
            }
        }
    } else if (uMsg == WM_DESTROY) {
        subclassed = false;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    std::thread([](){
        auto start = GetTickCount64();
        while (true) {
            hWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (hWnd) {
                hWnd = FindWindowExW(hWnd, nullptr, L"TrayNotifyWnd", nullptr);
                if (hWnd) {
                    hWnd = FindWindowExW(hWnd, nullptr, L"SysPager", nullptr);
                    if (hWnd) {
                        hWnd = FindWindowExW(hWnd, nullptr, L"ToolbarWindow32", nullptr);
                    }
                }
            }
            if (hWnd || GetTickCount64() - start > 5000) break;
        }
        if (hWnd) {
            DWORD dwProcessId = 0;
            GetWindowThreadProcessId(hWnd, &dwProcessId);
            if (dwProcessId == GetCurrentProcessId()) {
                subclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, &SubclassProc, 0);
            }
        }
    }).detach();
    return TRUE;
}

void Wh_ModUninit() {
    if (subclassed) WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, &SubclassProc);
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
}
