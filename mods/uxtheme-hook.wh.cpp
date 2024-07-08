// ==WindhawkMod==
// @id              uxtheme-hook
// @name            UXTheme hook
// @description     Allows you to apply custom themes
// @version         1.1
// @author          rounk-ctrl
// @github          https://github.com/rounk-ctrl
// @include         winlogon.exe
// @include         explorer.exe
// @include         systemsettings.exe
// @include         logonui.exe
// @include         rundll32.exe
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UXTheme hook
apply custom themes.

### ⚠ Important ⚠
in order for this mod to work properly, you must include `winlogon.exe` and `logonui.exe` in windhawk's process inclusion list, in its advanced settings.

### bugs
the colors in control panel might be weird sometimes. works fine if you leave control panel open, then switch themes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cpanelFix: true
  $name: Fix control panel white header/sidebar
  $description: Fixes control panel hardcoding white image for header and sidebar
*/
// ==/WindhawkModSettings==

#include <vssym32.h>
#include <windhawk_utils.h>
#include <vector>
#include <uxtheme.h>
#include <winerror.h>

#ifdef _WIN64
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL  __stdcall
#define SSTDCALL L"__stdcall"
#endif
typedef unsigned __int64 QWORD;

struct {
    bool cpanelFix;
} settings;

#pragma region common
typedef HRESULT (STDCALL *CThemeSignature_Verify_t)(PVOID, PVOID);
CThemeSignature_Verify_t CThemeSignature_Verify;
HRESULT STDCALL CThemeSignature_VerifyHook(
    PVOID pThis,
    PVOID hFile
)
{
    return ERROR_SUCCESS;
}
#pragma endregion

#pragma region DUI
typedef VOID(STDCALL *Element_PaintBgT)(class Element*, HDC , class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
Element_PaintBgT Element_PaintBg;
VOID STDCALL Element_PaintBgHook(class Element* This, HDC hdc, class Value* value, LPRECT pRect, LPRECT pClipRect, LPRECT pExcludeRect, LPRECT pTargetRect)
{
    //unsigned char byteValue = *(reinterpret_cast<unsigned char*>(value) + 8);
    if ((int)(*(DWORD *)value << 26) >> 26 != 9 )
    {
        auto v44 = *((QWORD *)value + 1);
        auto v45 = (v44+20)& 7;
        // 6-> selection
        // 3-> hovered stuff
        // 4-> cpanel top bar and side bar (white image)
        // 1-> some new cp page style (cp_hub_frame)
        if (v45==4)
        { 
            HWND wnd = WindowFromDC(hdc);
            HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
            COLORREF clrBg;
            GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &clrBg);
            HBRUSH SolidBrush = CreateSolidBrush(clrBg);
            FillRect(hdc, pRect, SolidBrush);
            DeleteObject(SolidBrush);
            CloseThemeData(hTh);
        }
        else
        {
            Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
        }
    }
    else
    {
        Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
    }
}
#pragma endregion


using SetSysColors_t = decltype(&SetSysColors);
SetSysColors_t SetSysColors_orig;

int WINAPI SetSysColors_hook(int cElements, const INT *lpaElements, const COLORREF *lpaRgbValues)
{
    // logonui
    if (cElements == 13) return 1;
    return SetSysColors_orig(cElements, lpaElements, lpaRgbValues);
}

void LoadSettings() {
    settings.cpanelFix = Wh_GetIntSetting(L"cpanelFix");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {L"public: long " SSTDCALL " CThemeSignature::Verify(void *)"},
        &CThemeSignature_Verify,
        CThemeSignature_VerifyHook,
        false
    }};

    std::vector<LPCWSTR> files{L"uxtheme.dll", L"uxinit.dll", L"themeui.dll"};

    for (LPCWSTR dll : files)
    {
        HMODULE hMod = LoadLibrary(dll);
        if (!WindhawkUtils::HookSymbols(hMod, hooks, ARRAYSIZE(hooks)))
        {
            Wh_Log(L"Failed to hook one or more symbol functions from %s",dll);
            return FALSE;
        }
    }
    
    // for logonui
    WindhawkUtils::Wh_SetFunctionHookT(SetSysColors, SetSysColors_hook, &SetSysColors_orig);

    WindhawkUtils::SYMBOL_HOOK duiHooks[] =
    {
        {
            {L"public: void " SSTDCALL " DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)"},
            &Element_PaintBg,
            Element_PaintBgHook,
            false
        },
    };
    if (settings.cpanelFix)
    {
        HMODULE hDui = LoadLibraryW(L"dui70.dll");
        if (!WindhawkUtils::HookSymbols(hDui, duiHooks, ARRAYSIZE(duiHooks))) 
        {
            Wh_Log(L"Failed to hook DUI");
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    *bReload = TRUE;
    return TRUE;
}
