// ==WindhawkMod==
// @id              dark-title-bars
// @name            Dark Title Bars
// @description     Принудительно включает тёмные заголовки окон (Dark Mode Title Bars) для Win32/WPF приложений.
// @version         1.0.1
// @author          Windhawk User
// @include         *
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Dark Title Bars

Many classic Win32 and WPF applications implement custom dark themes for their internal UI and context menus, but their main window title bars remain glaringly white. 

This mod forces the standard Windows Desktop Window Manager (DWM) to render the title bars of these applications in the native **Dark Mode**.

### Features
* Automatically applies the dark title bar to newly opened windows.
* Instantly updates all already open windows when the mod is enabled.
* Safely restores the original bright appearance when the mod is disabled.
* Uses native Windows undocumented APIs for seamless UI integration.

**Compatibility**: Works on Windows 10 (Build 18362 and newer) and Windows 11.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_api.h>

// Атрибуты DWM для тёмного режима
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE_V2
#define DWMWA_USE_IMMERSIVE_DARK_MODE_V2 19
#endif

enum class AppMode {
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
};

// Определения типов для динамической загрузки функций
using SetPreferredAppMode_T = AppMode(WINAPI*)(AppMode);
using AllowDarkModeForWindow_T = bool(WINAPI*)(HWND, bool);
using RtlGetNtVersionNumbers_T = VOID(NTAPI*)(LPDWORD, LPDWORD, LPDWORD);
using DwmSetWindowAttribute_T = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);

// Указатели на функции
SetPreferredAppMode_T SetPreferredAppMode = nullptr;
AllowDarkModeForWindow_T AllowDarkModeForWindow = nullptr;
RtlGetNtVersionNumbers_T pRtlGetNtVersionNumbers = nullptr;
DwmSetWindowAttribute_T pDwmSetWindowAttribute = nullptr;

HMODULE g_hUxtheme = nullptr;
HMODULE g_hDwmapi = nullptr;

// Динамически получаем билд Windows
DWORD GetWindowsBuildNumber() {
    if (!pRtlGetNtVersionNumbers) {
        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
        if (hNtdll) {
            pRtlGetNtVersionNumbers = (RtlGetNtVersionNumbers_T)GetProcAddress(hNtdll, "RtlGetNtVersionNumbers");
        }
    }
    
    if (pRtlGetNtVersionNumbers) {
        DWORD build = 0;
        pRtlGetNtVersionNumbers(nullptr, nullptr, &build);
        return build & ~0xF0000000;
    }
    return 0; // Fallback
}

// Функция для применения тёмного заголовка к конкретному окну
void ApplyDarkTitleBar(HWND hWnd, bool enableDark = true) {
    if (!IsWindow(hWnd)) return;

    // Пропускаем дочерние элементы управления, применяем только к главным окнам
    if (GetWindowLongW(hWnd, GWL_STYLE) & WS_CHILD) return;

    // Разрешаем тёмный режим для окна (недокументированная функция Uxtheme)
    if (AllowDarkModeForWindow) {
        AllowDarkModeForWindow(hWnd, enableDark);
    }

    // Применяем тёмный заголовок через DWM
    if (pDwmSetWindowAttribute) {
        DWORD build = GetWindowsBuildNumber();
        DWORD attribute = (build >= 18985) ? DWMWA_USE_IMMERSIVE_DARK_MODE : DWMWA_USE_IMMERSIVE_DARK_MODE_V2;
        BOOL isDarkMode = enableDark ? TRUE : FALSE;
        
        pDwmSetWindowAttribute(hWnd, attribute, &isDarkMode, sizeof(isDarkMode));
    }
}

// Хук на создание окна (Unicode)
decltype(&CreateWindowExW) CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) 
{
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        ApplyDarkTitleBar(hWnd);
    }
    return hWnd;
}

// Хук на создание окна (ANSI)
decltype(&CreateWindowExA) CreateWindowExA_Original;
HWND WINAPI CreateWindowExA_Hook(
    DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) 
{
    HWND hWnd = CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        ApplyDarkTitleBar(hWnd);
    }
    return hWnd;
}

// Callback для поиска и применения темы к уже существующим окнам при запуске мода
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD processId;
    GetWindowThreadProcessId(hWnd, &processId);
    
    // Применяем только к окнам текущего процесса
    if (processId == GetCurrentProcessId()) {
        ApplyDarkTitleBar(hWnd, (bool)lParam);
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Инициализация Dark Title Bars");

    // Загружаем dwmapi.dll и получаем нужную функцию
    g_hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (g_hDwmapi) {
        pDwmSetWindowAttribute = (DwmSetWindowAttribute_T)GetProcAddress(g_hDwmapi, "DwmSetWindowAttribute");
    }

    // Загружаем uxtheme.dll и получаем скрытые функции
    g_hUxtheme = LoadLibraryW(L"uxtheme.dll");
    if (g_hUxtheme) {
        SetPreferredAppMode = (SetPreferredAppMode_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135));
        AllowDarkModeForWindow = (AllowDarkModeForWindow_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(133));
        
        if (SetPreferredAppMode) {
            SetPreferredAppMode(AppMode::ForceDark);
        }
    }

    // Устанавливаем хуки на создание окон
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExA_Hook, (void**)&CreateWindowExA_Original);

    // Применяем тёмный заголовок ко всем окнам, которые уже открыты
    EnumWindows(EnumWindowsProc, true);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Выгрузка Dark Title Bars, возврат светлой темы");
    
    if (SetPreferredAppMode) {
        SetPreferredAppMode(AppMode::Default);
    }

    // Возвращаем светлые заголовки для существующих окон
    EnumWindows(EnumWindowsProc, false);

    // Освобождаем библиотеки
    if (g_hUxtheme) FreeLibrary(g_hUxtheme);
    if (g_hDwmapi) FreeLibrary(g_hDwmapi);
}
