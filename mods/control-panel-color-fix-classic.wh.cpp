// ==WindhawkMod==
// @id              control-panel-color-fix-classic
// @name            Control Panel Color Fix (Classic theme)
// @description     Fixes white header and sidebar in Control Panel (Windows 11, Classic theme only)
// @version         1.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @include         systemsettings.exe
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Makes the sidebar in the Control Panel to conform with the color scheme in the Classic Theme.

This is a fork of version 1.0 of the `Control Panel Color Fix` mod by @chip33
as the version 1.0.1 of that mod broke compatibility with the Classic theme.

Only works on Windows 11.

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>             // Needed for TMT_FILLCOLOR
#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#define STDCALL __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL __stdcall
#define SSTDCALL L"__stdcall"
#endif

typedef VOID(STDCALL *Element_PaintBgT)(
    class Element*, HDC, class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
Element_PaintBgT Element_PaintBg;

VOID STDCALL Element_PaintBgHook(
    class Element* This, HDC hdc, class Value* value,
    LPRECT pRect, LPRECT pClipRect, LPRECT pExcludeRect, LPRECT pTargetRect)
{
    // Check if the element type is not 9 (skip certain element type)
    if ((int)(*(DWORD*)value << 26) >> 26 != 9) {
        auto v44 = *((unsigned __int64*)value + 1);
        // Extract subtype (masking with 7). If == 4, likely header/sidebar element.
        auto v45 = (v44 + 20) & 7;
        if (v45 == 4) {
            HWND wnd = WindowFromDC(hdc);
            HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
            if (hTh) {
                COLORREF clrBg;
                if (SUCCEEDED(GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &clrBg))) {
                    HBRUSH brush = CreateSolidBrush(clrBg);
                    FillRect(hdc, pRect, brush);
                    DeleteObject(brush);
                }
                CloseThemeData(hTh);
            }
            return;
        }
    }

    Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
}

typedef LONG (WINAPI *RtlGetVersionPtr)(OSVERSIONINFOEXW*);

BOOL Wh_ModInit() {
    // Windows 11 only: build 22000+
    OSVERSIONINFOEXW osvi = { sizeof(osvi) };

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        Wh_Log(L"Failed to get ntdll.dll handle.");
        return FALSE;
    }

    auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion || pRtlGetVersion(&osvi) != 0) {
        Wh_Log(L"Failed to query Windows version.");
        return FALSE;
    }

    if (osvi.dwMajorVersion != 10 || osvi.dwBuildNumber < 22000) {
        Wh_Log(L"Unsupported Windows version — this mod is for Windows 11 only.");
        return FALSE;
    }

    HMODULE hMod = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hMod) {
        Wh_Log(L"Failed to load dui70.dll.");
        return FALSE;
    }

    // dui70.dll
    WindhawkUtils::SYMBOL_HOOK hook = {
        {L"public: void " SSTDCALL " DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)"},
        &Element_PaintBg,
        Element_PaintBgHook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hMod, &hook, 1)) {
        Wh_Log(L"Failed to hook DirectUI::Element::PaintBackground.");
        return FALSE;
    }

    return TRUE;
}
