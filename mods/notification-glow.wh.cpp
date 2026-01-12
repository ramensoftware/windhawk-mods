// ==WindhawkMod==
// @id             notification-glow
// @name           Win11 Notifications Glow Effect
// @description    Injects an animated neon glow effect into the Windows 11 Notification glass layer
// @version        1.0
// @github         Frqmelikescheese
// @author         Frqme
// @include        ShellExperienceHost.exe
// @include        StartMenuExperienceHost.exe
// @include        Explorer.exe
// @architecture   x86-64
// @compilerOptions -luser32 -lgdi32 -ldwmapi -luxtheme -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Animated Glass Glow
This mod targets the notification composition layer to create a moving glow.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- color1: "#00FFFF"
  $name: Glow Color 1
- color2: "#FF00FF"
  $name: Glow Color 2
- animSpeed: 2
  $name: Animation Speed
  $description: Higher values make the transition faster
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <string>
#include <vector>
#include <algorithm>

struct {
    COLORREF c1;
    COLORREF c2;
    int speed;
} config;

std::vector<HWND> active_windows;

COLORREF HexToRGB(const wchar_t* hex, COLORREF fallback) {
    if (!hex || !*hex) return fallback;
    std::wstring s = hex;
    if (s[0] == L'#') s = s.substr(1);
    unsigned long v = wcstoul(s.c_str(), nullptr, 16);
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

void SyncSettings() {
    PCWSTR s1 = Wh_GetStringSetting(L"color1");
    PCWSTR s2 = Wh_GetStringSetting(L"color2");
    config.c1 = HexToRGB(s1, RGB(0, 255, 255));
    config.c2 = HexToRGB(s2, RGB(255, 0, 255));
    config.speed = Wh_GetIntSetting(L"animSpeed");
    Wh_FreeStringSetting(s1);
    Wh_FreeStringSetting(s2);
}

COLORREF Blend(COLORREF a, COLORREF b, float t) {
    BYTE r = (BYTE)(GetRValue(a) + t * (GetRValue(b) - GetRValue(a)));
    BYTE g = (BYTE)(GetGValue(a) + t * (GetGValue(b) - GetGValue(a)));
    BYTE b_val = (BYTE)(GetBValue(a) + t * (GetBValue(b) - GetBValue(a)));
    return RGB(r, g, b_val);
}

void UpdateEffect(HWND h, COLORREF col) {
    if (!IsWindow(h)) return;

    BOOL mode = TRUE;
    DwmSetWindowAttribute(h, 20, &mode, sizeof(mode));
    DwmSetWindowAttribute(h, 34, &col, sizeof(col));
    DwmSetWindowAttribute(h, 35, &col, sizeof(col));

    int radius = 2;
    DwmSetWindowAttribute(h, 33, &radius, sizeof(radius));

    MARGINS m = {-1}; 
    DwmExtendFrameIntoClientArea(h, &m);

    SetWindowPos(h, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

DWORD WINAPI RenderLoop(LPVOID lpParam) {
    float frame = 0.0f;
    bool dir = true;

    while (true) {
        float delta = 0.005f * (config.speed > 0 ? (float)config.speed : 1.0f);
        
        if (dir) {
            frame += delta;
            if (frame >= 1.0f) { frame = 1.0f; dir = false; }
        } else {
            frame -= delta;
            if (frame <= 0.0f) { frame = 0.0f; dir = true; }
        }

        COLORREF fx = Blend(config.c1, config.c2, frame);

        for (auto it = active_windows.begin(); it != active_windows.end(); ) {
            if (IsWindow(*it) && IsWindowVisible(*it)) {
                UpdateEffect(*it, fx);
                ++it;
            } else {
                it = active_windows.erase(it);
            }
        }

        Sleep(30);
    }
    return 0;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Orig;

BOOL WINAPI ShowWindow_Proxy(HWND hWnd, int nCmdShow) {
    BOOL ret = ShowWindow_Orig(hWnd, nCmdShow);
    
    if (nCmdShow != SW_HIDE) {
        wchar_t name[256];
        if (GetClassNameW(hWnd, name, 256)) {
            bool match = (wcsstr(name, L"Windows.UI.Core.CoreWindow") || 
                          wcsstr(name, L"Toast") ||
                          wcsstr(name, L"Notification") ||
                          wcsstr(name, L"XamlExplorerHostIslandWindow"));

            if (match) {
                LONG_PTR style = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
                if ((style & WS_EX_TOPMOST)) {
                    if (std::find(active_windows.begin(), active_windows.end(), hWnd) == active_windows.end()) {
                        active_windows.push_back(hWnd);
                    }
                }
            }
        }
    }
    return ret;
}

BOOL Wh_ModInit() {
    SyncSettings();
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Proxy, (void**)&ShowWindow_Orig);
    CreateThread(NULL, 0, RenderLoop, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    SyncSettings();
}
