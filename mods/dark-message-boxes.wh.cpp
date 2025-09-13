// ==WindhawkMod==
// @id              dark-message-boxes
// @name            Dark mode message boxes
// @description     Enables dark mode for message boxes.
// @version         1.0
// @author          Mgg Sk
// @github          https://github.com/MGGSK
// @include         *
// @compilerOptions -lGdi32 -lComCtl32 -lDwmApi -lUxTheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark mode message boxes
Forces dark mode for all win32 message / shell message boxes.

### Before:
![Before](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/dark-message-boxes-before.png)

### After:
![After](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/dark-message-boxes-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windhawk_api.h>

#include <Windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <thread>

#define HASFLAG(value, flag) ((value & flag) == flag)
#define DPIVALUE(value) (IsProcessDPIAware() ? (value * ((double)GetDpiForSystem() / 96)) : value)

struct MSGBOXDATA
{
    UINT                cbSize;
    HWND                hwndOwner;
    HINSTANCE           hInstance;
    LPCWSTR             lpszText;
    LPCWSTR             lpszCaption;
    DWORD               dwStyle;
    LPCWSTR             lpszIcon;
    DWORD_PTR           dwContextHelpId;
    MSGBOXCALLBACK      lpfnMsgBoxCallback;
    DWORD               dwLanguageId;
    HWND                HWNDOwner;
    DWORD               dwPadding;
    WORD                wLanguageId;
    const INT*          pidButton;
    LPCWSTR*            ppszButtonText;
    DWORD               cButtons;
    UINT                DefButton;
    UINT                CancelId;
    DWORD               dwTimeout;
    HWND*               phwndList;
    DWORD               dwReserved[20];
};

constexpr int IDI_MODERN_INFORMATION = 81;
constexpr int IDI_MODERN_WARNING = 84;
constexpr int IDI_MODERN_QUESTION = 99;
constexpr int IDI_MODERN_ERROR = 98;

const HBRUSH g_background = CreateSolidBrush(0x191919);
const HBRUSH g_btnBackground = CreateSolidBrush(0x262626);
const HBRUSH g_barBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);

HMODULE g_hUser32 = nullptr;
HMODULE g_hShlwApi = nullptr;
HMODULE g_hImageres = nullptr;

HFONT g_hTextFont = nullptr;

void PaintBackground(HWND hWnd, HDC hdc)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    FillRect(hdc, &rcClient, g_background);
}

LRESULT CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    switch (uMsg)
    {
    case WM_CTLCOLORBTN:
        return (LRESULT)g_barBackground;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
    
        RECT rcBottomBar;
        GetClientRect(hWnd, &rcBottomBar);
        rcBottomBar.top = rcBottomBar.bottom - DPIVALUE(45);

        FillRect(hdc, &rcBottomBar, g_barBackground);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
    {
        PaintBackground(hWnd, (HDC)wParam);
        return 0;
    }

    case WM_DPICHANGED:
    {
        DeleteObject(g_hTextFont);
        break;
    }

    default:break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK IconProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HICON hIcon = nullptr;

        if(HASFLAG(dwRefData,MB_ICONINFORMATION))
            hIcon = LoadIconW(g_hImageres, MAKEINTRESOURCEW(IDI_MODERN_INFORMATION));
        else if(HASFLAG(dwRefData, MB_ICONWARNING))
            hIcon = LoadIconW(g_hImageres, MAKEINTRESOURCEW(IDI_MODERN_WARNING));
        else if(HASFLAG(dwRefData, MB_ICONQUESTION))
            hIcon = LoadIconW(g_hImageres, MAKEINTRESOURCEW(IDI_MODERN_QUESTION));
        else if(HASFLAG(dwRefData, MB_ICONERROR))
            hIcon = LoadIconW(g_hImageres, MAKEINTRESOURCEW(IDI_MODERN_ERROR));

        DrawIcon(hdc, 0, 0, hIcon);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
    {
        PaintBackground(hWnd, (HDC)wParam);
        return 0;
    }

    default:break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TextProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        wchar_t text[1024];
        GetWindowTextW(hWnd, text, _countof(text));

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, 0xFFFFFF);

        if(!g_hTextFont)
        {
            g_hTextFont = CreateFontW(
                DPIVALUE(16), 0, 0, 0, FW_NORMAL,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        }
        SelectObject(hdc, g_hTextFont);

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        DrawTextW(hdc, text, -1, &rcClient, DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
    {
        PaintBackground(hWnd, (HDC)wParam);
        return 0;
    }

    default:break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK MsgBoxWindowEnumProc(HWND hWnd, LPARAM lParam)
{
    BOOL enableDarkMode = TRUE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enableDarkMode, sizeof(enableDarkMode));
    SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);

    DWORD dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
    if(HASFLAG(dwStyle,SS_ICON))
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, IconProc, lParam);
    else if(HASFLAG(dwStyle, SS_EDITCONTROL))
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TextProc, NULL);

    return true;
}

void SetDarkMode(LPCWSTR title, DWORD dwStyle)
{   
    HWND hWnd = nullptr;
    while (!(hWnd = FindWindowW(L"#32770", title)));
    
    MsgBoxWindowEnumProc(hWnd, NULL);
    EnumChildWindows(hWnd, MsgBoxWindowEnumProc, (LPARAM)dwStyle);
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DialogProc, NULL);
    
    InvalidateRect(hWnd, nullptr, TRUE);
    UpdateWindow(hWnd);
}

using SoftModalMessageBox_T = int(WINAPI*)(MSGBOXDATA*);
SoftModalMessageBox_T SoftModalMessageBox;
SoftModalMessageBox_T SoftModalMessageBox_Original;

int WINAPI SoftModalMessageBox_hook(MSGBOXDATA* lpData)
{
    std::thread darkModeThread{SetDarkMode, lpData->lpszCaption, lpData->dwStyle};
    int result = SoftModalMessageBox_Original(lpData);

    darkModeThread.join();
    return result;
}

using ConstructMessageStringW_T = LPWSTR(__fastcall*)(HINSTANCE hInstance, LPCWSTR lpszMessage, va_list* args);
ConstructMessageStringW_T ConstructMessageStringW;

using ShellMessageBoxW_T = decltype(&ShellMessageBoxW);
ShellMessageBoxW_T ShellMessageBoxW_Original;

int WINAPI ShellMessageBoxW_Hook(HINSTANCE hAppInst, HWND hWnd, LPCWSTR lpcText, LPCWSTR lpcTitle, UINT fuStyle, ...)
{
    va_list args;
    va_start(args, fuStyle);

    PWSTR lpcMessage = ConstructMessageStringW(hAppInst, lpcText, &args);
    
    MSGBOXPARAMSW params{ sizeof(params), hWnd, hAppInst, lpcText, lpcTitle, fuStyle };
    int result = MessageBoxIndirectW(&params);

    LocalFree(lpcMessage);
    va_end(args);
    return result;
}

const WindhawkUtils::SYMBOL_HOOK ShlwapiDllHooks[] = 
{
    {
        {
#ifdef _WIN64
            L"ConstructMessageStringW"
#else
            L"_ConstructMessageStringW@12"
#endif
        },
        &ConstructMessageStringW,
        nullptr,
        false
    }
};

BOOL Wh_ModInit()
{
    g_hUser32 = LoadLibraryExW(L"User32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hUser32)
        return FALSE;

    g_hShlwApi = LoadLibraryExW(L"Shlwapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!g_hShlwApi)
        return FALSE;

    g_hImageres = LoadLibraryExW(L"Imageres.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hImageres)
        return FALSE;
        
    if (!WindhawkUtils::HookSymbols(
        g_hShlwApi,
        ShlwapiDllHooks,
        ARRAYSIZE(ShlwapiDllHooks)))
        return FALSE;

    SoftModalMessageBox = (SoftModalMessageBox_T)GetProcAddress(g_hUser32, "SoftModalMessageBox");
    if (!SoftModalMessageBox)
        return FALSE;

    if(!WindhawkUtils::SetFunctionHook(ShellMessageBoxW, ShellMessageBoxW_Hook, &ShellMessageBoxW_Original))
        return FALSE;

    return WindhawkUtils::SetFunctionHook(SoftModalMessageBox, SoftModalMessageBox_hook, &SoftModalMessageBox_Original);
}

void Wh_ModUninit()
{
    FreeLibrary(g_hUser32);
    FreeLibrary(g_hShlwApi);
    FreeLibrary(g_hImageres);

    DeleteObject(g_background);
    DeleteObject(g_btnBackground);

    DeleteObject(g_hTextFont);
}
