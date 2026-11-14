// ==WindhawkMod==
// @id              yandex-music-transparency
// @name            Yandex Music Transparency
// @description     Makes the Yandex Music window transparent
// @version         1.1
// @author          ChatGPT with tester 'NIKTO'
// @github          NIKTO-2
// @include         Яндекс Музыка.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Yandex Music Transparency

Makes the Yandex Music desktop application window transparent. You can adjust the opacity level in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Alpha: 220
  $name: Window Opacity
  $description: Transparency level from 50 (more transparent) to 255 (opaque).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <wchar.h>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

struct Settings {
    int alpha;
};

struct OriginalWindowState {
    bool hadLayeredStyle;
    BYTE originalAlpha;
    bool hasOriginalAlpha;
};

Settings g_settings;
std::mutex g_windowsMutex;
std::unordered_map<HWND, OriginalWindowState> g_trackedWindows;
bool g_unloading = false;

void LoadSettings() {
    g_settings.alpha = Wh_GetIntSetting(L"Alpha");
    if (g_settings.alpha < 50) g_settings.alpha = 50;
    if (g_settings.alpha > 255) g_settings.alpha = 255;
}

bool IsTargetWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    if (GetParent(hwnd) != NULL) return false;

    WCHAR className[256];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 ||
        wcscmp(className, L"Chrome_WidgetWin_1") != 0) {
        return false;
    }

    return true;
}

bool UsesColorKey(HWND hwnd) {
    DWORD flags = 0;
    if (GetLayeredWindowAttributes(hwnd, NULL, NULL, &flags)) {
        return (flags & LWA_COLORKEY) != 0;
    }
    return false;
}

using SetLayeredWindowAttributes_t = decltype(&SetLayeredWindowAttributes);
SetLayeredWindowAttributes_t SetLayeredWindowAttributes_Original;

BOOL WINAPI SetLayeredWindowAttributes_Hook(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) {
    if (!g_unloading && IsTargetWindow(hwnd) && (dwFlags & LWA_ALPHA) && !(dwFlags & LWA_COLORKEY)) {
        {
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            auto it = g_trackedWindows.find(hwnd);
            if (it != g_trackedWindows.end()) {
                it->second.originalAlpha = bAlpha;
                it->second.hasOriginalAlpha = true;
            }
        }
        bAlpha = (BYTE)g_settings.alpha;
    }
    return SetLayeredWindowAttributes_Original(hwnd, crKey, bAlpha, dwFlags);
}

void ApplyTransparency(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        if (UsesColorKey(hwnd)) {
            return;
        }
    } else {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }

    SetLayeredWindowAttributes_Original(hwnd, 0, (BYTE)g_settings.alpha, LWA_ALPHA);
}

void RestoreWindow(HWND hwnd, const OriginalWindowState& state) {
    if (!IsWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (!state.hadLayeredStyle) {
        if ((exStyle & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
            SetLayeredWindowAttributes_Original(hwnd, 0, 255, LWA_ALPHA);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }
    } else if (state.hasOriginalAlpha && (exStyle & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
        SetLayeredWindowAttributes_Original(hwnd, 0, state.originalAlpha, LWA_ALPHA);
    }
}

LRESULT CALLBACK WindowSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_STYLECHANGING: {
            if (!g_unloading && wParam == GWL_EXSTYLE) {
                STYLESTRUCT* ss = (STYLESTRUCT*)lParam;
                if (ss && !(ss->styleNew & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
                    {
                        std::lock_guard<std::mutex> lock(g_windowsMutex);
                        auto it = g_trackedWindows.find(hwnd);
                        if (it != g_trackedWindows.end()) {
                            it->second.hadLayeredStyle = false;
                        }
                    }
                    ss->styleNew |= WS_EX_LAYERED;
                }
            }
            break;
        }
        case WM_STYLECHANGED: {
            if (!g_unloading && wParam == GWL_EXSTYLE) {
                ApplyTransparency(hwnd);
            }
            break;
        }
        case WM_NCDESTROY: {
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            g_trackedWindows.erase(hwnd);
            break;
        }
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AttachToWindow(HWND hwnd) {
    if (g_unloading || !IsTargetWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_LAYERED) && UsesColorKey(hwnd)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (g_trackedWindows.count(hwnd)) {
            ApplyTransparency(hwnd);
            return;
        }

        OriginalWindowState state{};
        state.hadLayeredStyle = (exStyle & WS_EX_LAYERED) != 0;
        if (state.hadLayeredStyle) {
            BYTE alpha = 255;
            DWORD flags = 0;
            if (GetLayeredWindowAttributes(hwnd, NULL, &alpha, &flags) && (flags & LWA_ALPHA)) {
                state.originalAlpha = alpha;
                state.hasOriginalAlpha = true;
            }
        }

        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, WindowSubclassProc, 0)) {
            return;
        }

        g_trackedWindows[hwnd] = state;
    }

    ApplyTransparency(hwnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hwnd && !g_unloading && hWndParent == NULL) {
        AttachToWindow(hwnd);
    }

    return hwnd;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    AttachToWindow(hwnd);
    return TRUE;
}

BOOL Wh_ModInit() {
    LPCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && wcsstr(cmdLine, L"--type=") != NULL) {
        return FALSE;
    }

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)SetLayeredWindowAttributes, (void*)SetLayeredWindowAttributes_Hook, (void**)&SetLayeredWindowAttributes_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        windows.reserve(g_trackedWindows.size());
        for (const auto& pair : g_trackedWindows) {
            windows.push_back(pair.first);
        }
    }

    for (HWND hwnd : windows) {
        ApplyTransparency(hwnd);
    }
}

void Wh_ModUninit() {
    g_unloading = true;

    std::vector<std::pair<HWND, OriginalWindowState>> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        windows.assign(g_trackedWindows.begin(), g_trackedWindows.end());
        g_trackedWindows.clear();
    }

    for (const auto& entry : windows) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(entry.first, WindowSubclassProc);
        RestoreWindow(entry.first, entry.second);
    }
}
