// ==WindhawkMod==
// @id              msg-box-font-fix
// @name            Message Box Fix
// @description     Fixes the MessageBox font size and background
// @version         1.4.6
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luser32 -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Message Box Fix
Starting with Windows Vista, message boxes render the "Window" color in their upper half.

Starting with Windows 10 1709, message boxes render their font size 1pt less than the
user-defined size.\* You cannot just set this size higher, as many applications still query
it, and will show up with bigger fonts.

This mod fixes both of those things.

**This mod will only work on Windhawk v1.4 and greater.**

**Before:**

![Before](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-before.png)
![Before (classic)](https://raw.githubusercontent.com/aubymori/images/main/message-box-fix-before-classic.png)

**After:**

![After](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-after.png)
![After (classic)](https://raw.githubusercontent.com/aubymori/images/main/message-box-fix-after-classic.png)

*\*Microsoft changed the way the font size was calculator for Per-Monitor V2 DPI awareness. It ALWAYS gets
1pt below the font size, even when on a higher DPI. This is because Microsoft decided to do some weird math
instead of just using `SystemParametersInfoW` like a normal person.*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- font: true
  $name: Fix font size
  $description: Fix font rendering
- background: false
  $name: Remove "Window" background
  $description: Remove the "Window" color from the background, much like XP and before
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

struct {
    BOOL font;
    BOOL background;
} settings;

/* Only available in Windows 10 version 1607 and greater. */
UINT (* WINAPI GetDpiForSystem)(void);

/* Message box text windows that have been
   subclassed for background removal.
   
   See WM_PAINT on MsgBoxTextSubclassProc. */
std::vector<HWND> subclassed;

typedef HFONT (__fastcall *GetMessageBoxFontForDpi_t)(UINT);
GetMessageBoxFontForDpi_t GetMessageBoxFontForDpi_orig;
HFONT __fastcall GetMessageBoxFontForDpi_hook(
    UINT nDpi
)
{
    if (!settings.font)
    {
        return GetMessageBoxFontForDpi_orig(
            nDpi
        );
    }

    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);

    SystemParametersInfoW(
        SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICSW),
        &ncm,
        0
    );

    return CreateFontIndirectW(&(ncm.lfMessageFont));
}

LRESULT CALLBACK MsgBoxTextSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    if (settings.background)
    {
        switch (uMsg)
        {
            /* I literally could not find any style or anything that could
               remove the background on this, so I just paint it myself.
               Sorry. */
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hDC = BeginPaint(hWnd, &ps);

                RECT rcClient;
                GetClientRect(hWnd, &rcClient);

                int len = GetWindowTextLengthW(hWnd) + 1;
                LPWSTR szText = (LPWSTR)malloc(sizeof(WCHAR) * len);
                GetWindowTextW(hWnd, szText, len);

                HFONT hfMsg;
                if (settings.font || !GetDpiForSystem)
                {
                    hfMsg = GetMessageBoxFontForDpi_hook(
                        GetDpiForSystem ? GetDpiForSystem() : 96
                    );
                }
                else
                {
                    hfMsg = GetMessageBoxFontForDpi_orig(GetDpiForSystem());
                }
                HFONT hfOld = (HFONT)SelectObject(hDC, hfMsg);

                SetBkMode(hDC, TRANSPARENT);
                SetTextColor(hDC, GetSysColor(COLOR_BTNTEXT));

                DRAWTEXTPARAMS dtp = { 0 };
                dtp.cbSize = sizeof(DRAWTEXTPARAMS);
                dtp.uiLengthDrawn = wcslen(szText);

                DrawTextExW(
                    hDC,
                    szText,
                    -1,
                    &rcClient,
                    DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL,
                    &dtp
                );

                SelectObject(hDC, hfOld);
                DeleteObject(hfMsg);
                free(szText);
                EndPaint(hWnd, &ps);
                return 0;
            }
            case WM_DESTROY:
                subclassed.erase(std::remove_if(
                    subclassed.begin(),
                    subclassed.end(),
                    [hWnd](HWND hw)
                    {
                        return hw == hWnd;
                    }
                ));
                return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef INT_PTR (* WINAPI MB_DlgProc_t)(HWND, UINT, WPARAM, LPARAM);
MB_DlgProc_t MB_DlgProc_orig;
INT_PTR WINAPI MB_DlgProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (settings.background)
    {
        switch (uMsg)
        {
            case WM_CTLCOLORDLG:
            case WM_CTLCOLORSTATIC:
                return (INT_PTR)GetSysColorBrush(COLOR_3DFACE);
            /* The static window for text itself must handle WM_CTLCOLORSTATIC */
            case WM_INITDIALOG:
            {                   
                HWND hTxt = GetDlgItem(hWnd, 65535);
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hTxt, MsgBoxTextSubclassProc, NULL))
                {
                    subclassed.push_back(hTxt);
                }

                return MB_DlgProc_orig(hWnd, uMsg, wParam, lParam);
            }
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                return TRUE;
            }
        }
    }

    return MB_DlgProc_orig(hWnd, uMsg, wParam, lParam);
}

void LoadSettings(void)
{
    settings.font = Wh_GetIntSetting(L"font");
    settings.background = Wh_GetIntSetting(L"background");
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");

    if (!hUser32)
    {
        hUser32 = LoadLibraryW(L"user32.dll");
    }

    if (!hUser32)
    {
        MessageBoxW(
            NULL,
            L"Failed to load user32.dll. There is something seriously wrong with either your Windows install or Windhawk.",
            L"Windhawk: Message Box Fix",
            MB_ICONERROR
        );
        return FALSE;
    }

    GetDpiForSystem = (UINT (* WINAPI)(void))GetProcAddress(hUser32, "GetDpiForSystem");

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"struct HFONT__ * "
                #ifdef _WIN64
                L"__cdecl"
                #else
                L"__stdcall"
                #endif
                L" GetMessageBoxFontForDpi(unsigned int)"
            },
            &GetMessageBoxFontForDpi_orig,
            GetMessageBoxFontForDpi_hook,
            false
        },
        {
            {
                #ifdef _WIN64
                L"__int64 __cdecl MB_DlgProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
                #else
                L"int __stdcall MB_DlgProc(struct HWND__ *,unsigned int,unsigned int,long)"
                #endif
            },
            &MB_DlgProc_orig,
            MB_DlgProc_hook,
            false
        }
    };

    if (!HookSymbols(hUser32, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }

    return TRUE;
}

/**
  * Remove any subclasses from message box texts that are still there
  * If we don't do this, programs with open message boxes will crash
  */
void Wh_ModUninit(void)
{
    size_t len = subclassed.size();
    for (size_t i = 0; i < len; i++)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            subclassed[i],
            MsgBoxTextSubclassProc
        );
    }
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}