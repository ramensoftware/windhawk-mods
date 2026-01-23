// ==WindhawkMod==
// @id              classic-theme-uifile-fix
// @name            Classic Theme UIFILE Fix
// @description     Fixes Control Panel pages that don't display in Windows Classic theme
// @version         1.2.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Theme UIFILE Fix

Fixes Control Panel pages (Accessibility, Personalization, Storage Spaces, etc.) that fail to load 
when using the Windows Classic theme. This mod is based on the findings of R.O.B., the originator of the WinClassic forum.

![Screenshot](https://i.imgur.com/TVmK9IR.png)

## How it works
- Replaces `gtc()` (GetThemeColor) with `windowtext`
- Wraps bare `dtb()` (DrawThemeBackground) in `themeable(dtb(...), dfc(0, 0))`
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <string>
#include <uxtheme.h>

size_t FindMatchingParen(const std::wstring& str, size_t openPos) {
    int depth = 1;
    for (size_t i = openPos + 1; i < str.length(); i++) {
        if (str[i] == L'(') depth++;
        else if (str[i] == L')') {
            depth--;
            if (depth == 0) return i;
        }
    }
    return std::wstring::npos;
}

std::wstring FixGtcExceptControlPanelStyle(const std::wstring& input) {
    std::wstring result = input;
    size_t pos = 0;
    
    while ((pos = result.find(L"gtc(", pos)) != std::wstring::npos) {
        if (pos > 0 && (iswalnum(result[pos - 1]) || result[pos - 1] == L'_')) {
            pos++;
            continue;
        }
        
        size_t closeParen = FindMatchingParen(result, pos + 3);
        if (closeParen == std::wstring::npos) {
            pos++;
            continue;
        }
        
        std::wstring gtcCall = result.substr(pos, closeParen - pos + 1);
        
        if (gtcCall.find(L"CONTROLPANELSTYLE") != std::wstring::npos) {
            pos = closeParen + 1;
            continue;
        }
        
        result = result.substr(0, pos) + L"window" + result.substr(closeParen + 1);
        pos += 6;
    }
    
    return result;
}

std::wstring WrapBareDtb(const std::wstring& input) {
    std::wstring result = input;
    size_t pos = 0;
    
    while ((pos = result.find(L"dtb(", pos)) != std::wstring::npos) {
        if (pos > 0 && (iswalnum(result[pos - 1]) || result[pos - 1] == L'_')) {
            pos++;
            continue;
        }
        
        size_t themeablePos = result.rfind(L"themeable(", pos);
        if (themeablePos != std::wstring::npos && themeablePos >= (pos > 50 ? pos - 50 : 0)) {
            size_t themeableClose = FindMatchingParen(result, themeablePos + 9);
            if (themeableClose != std::wstring::npos && themeableClose > pos) {
                pos++;
                continue;
            }
        }
        
        size_t closeParen = FindMatchingParen(result, pos + 3);
        if (closeParen == std::wstring::npos) {
            pos++;
            continue;
        }
        
        std::wstring dtbCall = result.substr(pos, closeParen - pos + 1);
        std::wstring wrapped = L"themeable(" + dtbCall + L", dfc(0, 0))";
        
        result = result.substr(0, pos) + wrapped + result.substr(closeParen + 1);
        pos += wrapped.length();
    }
    
    return result;
}

using SetXML_t = HRESULT(WINAPI*)(void*, const WCHAR*, HINSTANCE, HINSTANCE);
SetXML_t g_origSetXML = nullptr;

HRESULT WINAPI Hook_SetXML(void* pThis, const WCHAR* pszXML, HINSTANCE hInstance, HINSTANCE hInstance2) {
    if (!pszXML || IsAppThemed() || (!wcsstr(pszXML, L"gtc(") && !wcsstr(pszXML, L"dtb("))) {
        return g_origSetXML(pThis, pszXML, hInstance, hInstance2);
    }
    
    std::wstring fixed(pszXML);
    fixed = FixGtcExceptControlPanelStyle(fixed);
    fixed = WrapBareDtb(fixed);
    
    return g_origSetXML(pThis, fixed.c_str(), hInstance, hInstance2);
}

BOOL Wh_ModInit() {

HMODULE hDui70 = LoadLibraryW(L"dui70.dll");

return hDui70 && (
        Wh_SetFunctionHook((void*)GetProcAddress(hDui70, 
    // public: long __cdecl DirectUI::DUIXmlParser::SetXML(unsigned short const *,struct HINSTANCE__ *,struct HINSTANCE__ *)
        "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z"), 
         (void*)Hook_SetXML, (void**)&g_origSetXML)||
        Wh_SetFunctionHook((void*)GetProcAddress(hDui70, 
    // public: long __thiscall DirectUI::DUIXmlParser::SetXML(unsigned short const *,struct HINSTANCE__ *,struct HINSTANCE__ *)
        "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z"), 
         (void*)Hook_SetXML, (void**)&g_origSetXML));
}
