// ==WindhawkMod==
// @id              classic-theme-transparency-fix
// @name            Classic theme transparency fix
// @description     Fixes transparency glitches in Classic theme
// @version         1.4
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @include         dllhost.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod removes the unwanted transparency from windows under Classic theme.
The option `Remove accent transparency from dialogs` is intended to fix the download dialog in
Internet Download Manager and, maybe, other apps. If you experience black rectangles instead of
shadiows, turn it off.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- removeDialogTransparency: true
  $name: Remove accent transparency from dialogs
  $description: Removes accent transparency from newly created windows of class #32770
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <psapi.h>

// --- Structs and types for dialog transparency removal ---

struct ACCENT_POLICY {
    DWORD AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD  Attrib;
    PVOID  pvData;
    SIZE_T cbData;
};

typedef BOOL (WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// --- Global Avalonia flag ---
// -1 = not checked yet, 0 = not Avalonia, 1 = Avalonia
int g_isAvaloniaProcess = -1;

// --- DWM / composition hooks ---

typedef HRESULT (WINAPI *DwmIsCompositionEnabled_t)(BOOL *);
DwmIsCompositionEnabled_t DwmIsCompositionEnabled_orig;
HRESULT WINAPI DwmIsCompositionEnabled_hook(BOOL *pfEnabled)
{
    if (g_isAvaloniaProcess == -1) {
        g_isAvaloniaProcess = 0;
        HMODULE hMods[1024];
        DWORD cbNeeded;

        if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                WCHAR szModName[MAX_PATH];
                if (GetModuleFileNameW(hMods[i], szModName, sizeof(szModName) / sizeof(WCHAR))) {
                    WCHAR* fileName = wcsrchr(szModName, L'\\');
                    fileName = fileName ? fileName + 1 : szModName;

                    if (_wcsnicmp(fileName, L"av_lib", 6) == 0) {
                        Wh_Log(L"Avalonia library detected");
                        g_isAvaloniaProcess = 1;
                        break;
                    }
                }
            }
        }
    }

    if (g_isAvaloniaProcess == 1) {
        return DwmIsCompositionEnabled_orig(pfEnabled);
    }

    *pfEnabled = FALSE;
    return S_OK;
}

typedef BOOL (WINAPI *IsCompositionActive_t)(void);
IsCompositionActive_t IsCompositionActive_orig;
BOOL WINAPI IsCompositionActive_hook()
{
    return FALSE;
}

// --- ShowWindow hook for dialog transparency removal ---

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow)
{
    if (nCmdShow != SW_HIDE && hWnd) {
        WCHAR className[64];
        if (GetClassName(hWnd, className, 64) && !wcscmp(className, L"#32770")) {
            auto SetWCA = (pSetWindowCompositionAttribute)GetProcAddress(
                GetModuleHandle(L"user32.dll"), "SetWindowCompositionAttribute");

            if (SetWCA) {
                ACCENT_POLICY accent = {1};
                WINDOWCOMPOSITIONATTRIBDATA data = {19, &accent, sizeof(accent)};
                SetWCA(hWnd, &data);
            }
        }
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

// --- Mod init ---

BOOL Wh_ModInit()
{
    HMODULE dwmapiModule = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    FARPROC pFunction = GetProcAddress(dwmapiModule, "DwmIsCompositionEnabled");
    Wh_SetFunctionHook((void*)pFunction, (void*)DwmIsCompositionEnabled_hook, (void**)&DwmIsCompositionEnabled_orig);

    HMODULE uxthemeModule = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    pFunction = GetProcAddress(uxthemeModule, "IsCompositionActive");
    Wh_SetFunctionHook((void*)pFunction, (void*)IsCompositionActive_hook, (void**)&IsCompositionActive_orig);

    if (Wh_GetIntSetting(L"removeDialogTransparency")) {
        Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    }

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload){
  *bReload = TRUE;
  return TRUE;
}
