// ==WindhawkMod==
// @id              classic-theme-uifile-fix
// @name            Classic Theme UIFILE Fix
// @description     Fixes Control Panel pages that don't display in Windows Classic theme
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Theme UIFILE Fix

Fixes Control Panel pages (Accessibility, Personalization, Storage Spaces, etc.) that fail to load 
when using the Windows Classic theme.

![Screenshot](https://i.imgur.com/TVmK9IR.png)

## How it works
- Replaces `gtc()` (GetThemeColor) with `windowtext`
- Wraps bare `dtb()` (DrawThemeBackground) in `themeable(dtb(...), dfc(0, 0))`
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <string>

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

std::wstring ReplaceFunction(const std::wstring& input, const wchar_t* funcName, const wchar_t* replacement) {
    std::wstring result = input;
    std::wstring searchStr = std::wstring(funcName) + L"(";
    size_t pos = 0;
    
    while ((pos = result.find(searchStr, pos)) != std::wstring::npos) {
        if (pos > 0 && (iswalnum(result[pos - 1]) || result[pos - 1] == L'_')) {
            pos++;
            continue;
        }
        
        size_t closeParen = FindMatchingParen(result, pos + wcslen(funcName));
        if (closeParen == std::wstring::npos) {
            pos++;
            continue;
        }
        
        result = result.substr(0, pos) + replacement + result.substr(closeParen + 1);
        pos += wcslen(replacement);
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
        
        // Check if inside themeable()
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
    if (!pszXML || (!wcsstr(pszXML, L"gtc(") && !wcsstr(pszXML, L"dtb("))) {
        return g_origSetXML(pThis, pszXML, hInstance, hInstance2);
    }
    
    std::wstring fixed(pszXML);
    fixed = ReplaceFunction(fixed, L"gtc", L"windowtext");
    fixed = WrapBareDtb(fixed);
    
    return g_origSetXML(pThis, fixed.c_str(), hInstance, hInstance2);
}

BOOL Wh_ModInit() {
    HMODULE hDui70 = LoadLibraryW(L"dui70.dll");
    
    WindhawkUtils::SYMBOL_HOOK dui70DllHooks[] = {
        {
            {L"public: long __cdecl DirectUI::DUIXmlParser::SetXML(unsigned short const *,struct HINSTANCE__ *,struct HINSTANCE__ *)"},
            &g_origSetXML,
            Hook_SetXML,
            false
        }
    };
    
    return hDui70 && (Wh_SetFunctionHook((void*)GetProcAddress(hDui70, 
            "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z"), 
             (void*)Hook_SetXML, (void**)&g_origSetXML)||
             WindhawkUtils::HookSymbols(hDui70, dui70DllHooks, ARRAYSIZE(dui70DllHooks)));
}
