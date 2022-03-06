// ==WindhawkMod==
// @id              text-replace
// @name            Text Replace
// @description     Replace any text with any other text in any program
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Text Replace

Replace any text with any other text in any program. The mod is able to
replace some texts in some programs, while some other programs and
elements are not supported. The replacement works best in native elements,
and usually doesn't work in custom ones.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PerProgramConfig:
  - - Name: notepad.exe
      $name: Program name or path
    - Search: Notepad
      $name: The text to be replaced
    - Replace: WindPad
      $name: The replacement text
  - - Name: mspaint.exe
    - Search: Paint
    - Replace: PhotoHawk
  $name: Per-program configuration
*/
// ==/WindhawkModSettings==

#include <string>
#include <vector>

struct ReplacementItem {
    std::string searchA;
    std::string replaceA;
    std::wstring searchW;
    std::wstring replaceW;
};

std::vector<ReplacementItem> g_replacementItems;

template<typename T>
T ReplaceAll(T str, const T& from, const T& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != T::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string ReplaceStringA(PCSTR string, size_t len = -1)
{
    if (len == -1) {
        len = strlen(string);
    }

    std::string result(string, len);

    for (const auto& item : g_replacementItems) {
        result = ReplaceAll<std::string>(std::move(result), item.searchA, item.replaceA);
    }

    return result;
}

std::wstring ReplaceStringW(PCWSTR string, size_t len = -1)
{
    if (len == -1) {
        len = wcslen(string);
    }

    std::wstring result(string, len);

    for (const auto& item : g_replacementItems) {
        result = ReplaceAll<std::wstring>(std::move(result), item.searchW, item.replaceW);
    }

    return result;
}

using SetWindowTextA_t = decltype(&SetWindowTextA);
SetWindowTextA_t pOriginalSetWindowTextA;
BOOL WINAPI SetWindowTextAHook(HWND hWnd, LPCSTR lpString)
{
    if (lpString) {
        std::string str = ReplaceStringA(lpString);
        return pOriginalSetWindowTextA(hWnd, str.c_str());
    }

    return pOriginalSetWindowTextA(hWnd, lpString);
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t pOriginalSetWindowTextW;
BOOL WINAPI SetWindowTextWHook(HWND hWnd, LPCWSTR lpString)
{
    if (lpString) {
        std::wstring str = ReplaceStringW(lpString);
        return pOriginalSetWindowTextW(hWnd, str.c_str());
    }

    return pOriginalSetWindowTextW(hWnd, lpString);
}

using InsertMenuA_t = decltype(&InsertMenuA);
InsertMenuA_t pOriginalInsertMenuA;
BOOL WINAPI InsertMenuAHook(HMENU hMenu,UINT uPosition,UINT uFlags,UINT_PTR uIDNewItem,LPCSTR lpNewItem)
{
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW)) && lpNewItem) {
        std::string str = ReplaceStringA(lpNewItem);
        return pOriginalInsertMenuA(hMenu,uPosition,uFlags,uIDNewItem,str.c_str());
    }

    return pOriginalInsertMenuA(hMenu,uPosition,uFlags,uIDNewItem,lpNewItem);
}

using InsertMenuW_t = decltype(&InsertMenuW);
InsertMenuW_t pOriginalInsertMenuW;
BOOL WINAPI InsertMenuWHook(HMENU hMenu,UINT uPosition,UINT uFlags,UINT_PTR uIDNewItem,LPCWSTR lpNewItem)
{
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW)) && lpNewItem) {
        std::wstring str = ReplaceStringW(lpNewItem);
        return pOriginalInsertMenuW(hMenu,uPosition,uFlags,uIDNewItem,str.c_str());
    }

    return pOriginalInsertMenuW(hMenu,uPosition,uFlags,uIDNewItem,lpNewItem);
}

using AppendMenuA_t = decltype(&AppendMenuA);
AppendMenuA_t pOriginalAppendMenuA;
BOOL WINAPI AppendMenuAHook(HMENU hMenu,UINT uFlags,UINT_PTR uIDNewItem,LPCSTR lpNewItem)
{
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW)) && lpNewItem) {
        std::string str = ReplaceStringA(lpNewItem);
        return pOriginalAppendMenuA(hMenu,uFlags,uIDNewItem,str.c_str());
    }

    return pOriginalAppendMenuA(hMenu,uFlags,uIDNewItem,lpNewItem);
}

using AppendMenuW_t = decltype(&AppendMenuW);
AppendMenuW_t pOriginalAppendMenuW;
BOOL WINAPI AppendMenuWHook(HMENU hMenu,UINT uFlags,UINT_PTR uIDNewItem,LPCWSTR lpNewItem)
{
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW)) && lpNewItem) {
        std::wstring str = ReplaceStringW(lpNewItem);
        return pOriginalAppendMenuW(hMenu,uFlags,uIDNewItem,str.c_str());
    }

    return pOriginalAppendMenuW(hMenu,uFlags,uIDNewItem,lpNewItem);
}

using ModifyMenuA_t = decltype(&ModifyMenuA);
ModifyMenuA_t pOriginalModifyMenuA;
BOOL WINAPI ModifyMenuAHook(HMENU hMenu,UINT uPosition,UINT uFlags,UINT_PTR uIDNewItem,LPCSTR lpNewItem)
{
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW)) && lpNewItem) {
        std::string str = ReplaceStringA(lpNewItem);
        return pOriginalModifyMenuA(hMenu,uPosition,uFlags,uIDNewItem,str.c_str());
    }

    return pOriginalModifyMenuA(hMenu,uPosition,uFlags,uIDNewItem,lpNewItem);
}

using ModifyMenuW_t = decltype(&ModifyMenuW);
ModifyMenuW_t pOriginalModifyMenuW;
BOOL WINAPI ModifyMenuWHook(HMENU hMenu,UINT uPosition,UINT uFlags,UINT_PTR uIDNewItem,LPCWSTR lpNewItem)
{
    if (!(uFlags & (MF_BITMAP | MF_OWNERDRAW)) && lpNewItem) {
        std::wstring str = ReplaceStringW(lpNewItem);
        return pOriginalModifyMenuW(hMenu,uPosition,uFlags,uIDNewItem,str.c_str());
    }

    return pOriginalModifyMenuW(hMenu,uPosition,uFlags,uIDNewItem,lpNewItem);
}

using InsertMenuItemA_t = decltype(&InsertMenuItemA);
InsertMenuItemA_t pOriginalInsertMenuItemA;
BOOL WINAPI InsertMenuItemAHook(HMENU hmenu,UINT item,WINBOOL fByPosition,LPCMENUITEMINFOA lpmi)
{
    if ((
        (lpmi->fMask & MIIM_STRING) ||
        ((lpmi->fMask & MIIM_TYPE) && (lpmi->fType & MFT_STRING))
    ) && lpmi->dwTypeData) {
        std::string str = ReplaceStringA(lpmi->dwTypeData);
        MENUITEMINFOA mi = *lpmi;
        mi.dwTypeData = str.data();
        return pOriginalInsertMenuItemA(hmenu,item,fByPosition,&mi);
    }

    return pOriginalInsertMenuItemA(hmenu,item,fByPosition,lpmi);
}

using InsertMenuItemW_t = decltype(&InsertMenuItemW);
InsertMenuItemW_t pOriginalInsertMenuItemW;
BOOL WINAPI InsertMenuItemWHook(HMENU hmenu,UINT item,WINBOOL fByPosition,LPCMENUITEMINFOW lpmi)
{
    if ((
        (lpmi->fMask & MIIM_STRING) ||
        ((lpmi->fMask & MIIM_TYPE) && (lpmi->fType & MFT_STRING))
    ) && lpmi->dwTypeData) {
        std::wstring str = ReplaceStringW(lpmi->dwTypeData);
        MENUITEMINFOW mi = *lpmi;
        mi.dwTypeData = str.data();
        return pOriginalInsertMenuItemW(hmenu,item,fByPosition,&mi);
    }

    return pOriginalInsertMenuItemW(hmenu,item,fByPosition,lpmi);
}

using SetMenuItemInfoA_t = decltype(&SetMenuItemInfoA);
SetMenuItemInfoA_t pOriginalSetMenuItemInfoA;
BOOL WINAPI SetMenuItemInfoAHook(HMENU hmenu,UINT item,WINBOOL fByPosition,LPCMENUITEMINFOA lpmi)
{
    if ((
        (lpmi->fMask & MIIM_STRING) ||
        ((lpmi->fMask & MIIM_TYPE) && (lpmi->fType & MFT_STRING))
    ) && lpmi->dwTypeData) {
        std::string str = ReplaceStringA(lpmi->dwTypeData);
        MENUITEMINFOA mi = *lpmi;
        mi.dwTypeData = str.data();
        return pOriginalSetMenuItemInfoA(hmenu,item,fByPosition,&mi);
    }

    return pOriginalSetMenuItemInfoA(hmenu,item,fByPosition,lpmi);
}

using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
SetMenuItemInfoW_t pOriginalSetMenuItemInfoW;
BOOL WINAPI SetMenuItemInfoWHook(HMENU hmenu,UINT item,WINBOOL fByPosition,LPCMENUITEMINFOW lpmi)
{
    if ((
        (lpmi->fMask & MIIM_STRING) ||
        ((lpmi->fMask & MIIM_TYPE) && (lpmi->fType & MFT_STRING))
    ) && lpmi->dwTypeData) {
        std::wstring str = ReplaceStringW(lpmi->dwTypeData);
        MENUITEMINFOW mi = *lpmi;
        mi.dwTypeData = str.data();
        return pOriginalSetMenuItemInfoW(hmenu,item,fByPosition,&mi);
    }

    return pOriginalSetMenuItemInfoW(hmenu,item,fByPosition,lpmi);
}

using TextOutA_t = decltype(&TextOutA);
TextOutA_t pOriginalTextOutA;
BOOL WINAPI TextOutAHook(HDC hdc,int x,int y,LPCSTR lpString,int c)
{
    if (lpString) {
        std::string str = ReplaceStringA(lpString, c);
        return pOriginalTextOutA(hdc,x,y,str.c_str(),str.length());
    }

    return pOriginalTextOutA(hdc,x,y,lpString,c);
}

using TextOutW_t = decltype(&TextOutW);
TextOutW_t pOriginalTextOutW;
BOOL WINAPI TextOutWHook(HDC hdc,int x,int y,LPCWSTR lpString,int c)
{
    if (lpString) {
        std::wstring str = ReplaceStringW(lpString, c);
        return pOriginalTextOutW(hdc,x,y,str.c_str(),str.length());
    }

    return pOriginalTextOutW(hdc,x,y,lpString,c);
}

using ExtTextOutA_t = decltype(&ExtTextOutA);
ExtTextOutA_t pOriginalExtTextOutA;
BOOL WINAPI ExtTextOutAHook(HDC hdc,int x,int y,UINT options,CONST RECT *lprect,LPCSTR lpString,UINT c,CONST INT *lpDx)
{
    if (!(options & ETO_GLYPH_INDEX) && lpString) {
        std::string str = ReplaceStringA(lpString, c);
        return pOriginalExtTextOutA(hdc,x,y,options,lprect,str.c_str(),str.length(),lpDx);
    }

    return pOriginalExtTextOutA(hdc,x,y,options,lprect,lpString,c,lpDx);
}

using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t pOriginalExtTextOutW;
BOOL WINAPI ExtTextOutWHook(HDC hdc,int x,int y,UINT options,CONST RECT *lprect,LPCWSTR lpString,UINT c,CONST INT *lpDx)
{
    if (!(options & ETO_GLYPH_INDEX) && lpString) {
        std::wstring str = ReplaceStringW(lpString, c);
        return pOriginalExtTextOutW(hdc,x,y,options,lprect,str.c_str(),str.length(),lpDx);
    }

    return pOriginalExtTextOutW(hdc,x,y,options,lprect,lpString,c,lpDx);
}

using DrawTextA_t = decltype(&DrawTextA);
DrawTextA_t pOriginalDrawTextA;
int WINAPI DrawTextAHook(HDC hdc,LPCSTR lpchText,int cchText,LPRECT lprc,UINT format)
{
    if (lpchText) {
        std::string str = ReplaceStringA(lpchText, cchText);
        int len = str.length();
        if (format & DT_MODIFYSTRING) {
            str.resize(len + 4);
        }
        return pOriginalDrawTextA(hdc,str.c_str(),len,lprc,format);
    }

    return pOriginalDrawTextA(hdc,lpchText,cchText,lprc,format);
}

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t pOriginalDrawTextW;
int WINAPI DrawTextWHook(HDC hdc,LPCWSTR lpchText,int cchText,LPRECT lprc,UINT format)
{
    if (lpchText) {
        std::wstring str = ReplaceStringW(lpchText, cchText);
        int len = str.length();
        if (format & DT_MODIFYSTRING) {
            str.resize(len + 4);
        }
        return pOriginalDrawTextW(hdc,str.c_str(),len,lprc,format);
    }

    return pOriginalDrawTextW(hdc,lpchText,cchText,lprc,format);
}

using DrawTextExA_t = decltype(&DrawTextExA);
DrawTextExA_t pOriginalDrawTextExA;
int WINAPI DrawTextExAHook(HDC hdc,LPSTR lpchText,int cchText,LPRECT lprc,UINT format,LPDRAWTEXTPARAMS lpdtp)
{
    if (lpchText) {
        std::string str = ReplaceStringA(lpchText, cchText);
        int len = str.length();
        if (format & DT_MODIFYSTRING) {
            str.resize(len + 4);
        }
        return pOriginalDrawTextExA(hdc,str.data(),len,lprc,format,lpdtp);
    }

    return pOriginalDrawTextExA(hdc,lpchText,cchText,lprc,format,lpdtp);
}

using DrawTextExW_t = decltype(&DrawTextExW);
DrawTextExW_t pOriginalDrawTextExW;
int WINAPI DrawTextExWHook(HDC hdc,LPWSTR lpchText,int cchText,LPRECT lprc,UINT format,LPDRAWTEXTPARAMS lpdtp)
{
    if (lpchText) {
        std::wstring str = ReplaceStringW(lpchText, cchText);
        int len = str.length();
        if (format & DT_MODIFYSTRING) {
            str.resize(len + 4);
        }
        return pOriginalDrawTextExW(hdc,str.data(),len,lprc,format,lpdtp);
    }

    return pOriginalDrawTextExW(hdc,lpchText,cchText,lprc,format,lpdtp);
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t pOriginalCreateWindowExA;
HWND WINAPI CreateWindowExAHook(DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
    if (lpWindowName) {
        std::string str = ReplaceStringA(lpWindowName);
        return pOriginalCreateWindowExA(dwExStyle,lpClassName,str.c_str(),dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
    }

    return pOriginalCreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
    if (lpWindowName) {
        std::wstring str = ReplaceStringW(lpWindowName);
        return pOriginalCreateWindowExW(dwExStyle,lpClassName,str.c_str(),dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
    }

    return pOriginalCreateWindowExW(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

using SendMessageA_t = decltype(&SendMessageA);
SendMessageA_t pOriginalSendMessageA;
LRESULT WINAPI SendMessageAHook(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
    if (Msg == WM_SETTEXT && lParam) {
        std::string str = ReplaceStringA((PCSTR)lParam);
        return pOriginalSendMessageA(hWnd,Msg,wParam,(LPARAM)str.c_str());
    }

    return pOriginalSendMessageA(hWnd,Msg,wParam,lParam);
}

using SendMessageW_t = decltype(&SendMessageW);
SendMessageW_t pOriginalSendMessageW;
LRESULT WINAPI SendMessageWHook(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
    if (Msg == WM_SETTEXT && lParam) {
        std::wstring str = ReplaceStringW((PCWSTR)lParam);
        return pOriginalSendMessageW(hWnd,Msg,wParam,(LPARAM)str.c_str());
    }

    return pOriginalSendMessageW(hWnd,Msg,wParam,lParam);
}

void LoadSettings()
{
    g_replacementItems.clear();

    WCHAR programPath[1024];
    DWORD dwSize = ARRAYSIZE(programPath);
    if (!QueryFullProcessImageName(GetCurrentProcess(), 0, programPath, &dwSize)) {
        *programPath = L'\0';
    }

    PCWSTR programFileName = wcsrchr(programPath, L'\\');
    if (programFileName) {
        programFileName++;
        if (!*programFileName) {
            programFileName = nullptr;
        }
    }

    for (int i = 0; ; i++) {
        bool matched = false;

        PCWSTR name = Wh_GetStringSetting(L"PerProgramConfig[%d].Name", i);
        bool hasName = *name;
        if (hasName) {
            if (programFileName && wcsicmp(programFileName, name) == 0) {
                matched = true;
            }
            else if (wcsicmp(programPath, name) == 0) {
                matched = true;
            }
        }

        Wh_FreeStringSetting(name);

        if (!hasName) {
            break;
        }

        if (matched) {
            PCWSTR search = Wh_GetStringSetting(L"PerProgramConfig[%d].Search", i);
            PCWSTR replace = Wh_GetStringSetting(L"PerProgramConfig[%d].Replace", i);

            if (*search) {
                g_replacementItems.push_back({
                    std::string(search, search + wcslen(search)),
                    std::string(replace, replace + wcslen(replace)),
                    search, replace
                });
            }

            Wh_FreeStringSetting(search);
            Wh_FreeStringSetting(replace);
        }
    }
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    LoadSettings();

    // Covers SetDlgItemText and SetDlgItemInt.
    Wh_SetFunctionHook((void*)SetWindowTextA, (void*)SetWindowTextAHook, (void**)&pOriginalSetWindowTextA);
    Wh_SetFunctionHook((void*)SetWindowTextW, (void*)SetWindowTextWHook, (void**)&pOriginalSetWindowTextW);

    Wh_SetFunctionHook((void*)InsertMenuA, (void*)InsertMenuAHook, (void**)&pOriginalInsertMenuA);
    Wh_SetFunctionHook((void*)InsertMenuW, (void*)InsertMenuWHook, (void**)&pOriginalInsertMenuW);

    Wh_SetFunctionHook((void*)AppendMenuA, (void*)AppendMenuAHook, (void**)&pOriginalAppendMenuA);
    Wh_SetFunctionHook((void*)AppendMenuW, (void*)AppendMenuWHook, (void**)&pOriginalAppendMenuW);

    Wh_SetFunctionHook((void*)ModifyMenuA, (void*)ModifyMenuAHook, (void**)&pOriginalModifyMenuA);
    Wh_SetFunctionHook((void*)ModifyMenuW, (void*)ModifyMenuWHook, (void**)&pOriginalModifyMenuW);

    Wh_SetFunctionHook((void*)InsertMenuItemA, (void*)InsertMenuItemAHook, (void**)&pOriginalInsertMenuItemA);
    Wh_SetFunctionHook((void*)InsertMenuItemW, (void*)InsertMenuItemWHook, (void**)&pOriginalInsertMenuItemW);

    Wh_SetFunctionHook((void*)SetMenuItemInfoA, (void*)SetMenuItemInfoAHook, (void**)&pOriginalSetMenuItemInfoA);
    Wh_SetFunctionHook((void*)SetMenuItemInfoW, (void*)SetMenuItemInfoWHook, (void**)&pOriginalSetMenuItemInfoW);

    Wh_SetFunctionHook((void*)TextOutA, (void*)TextOutAHook, (void**)&pOriginalTextOutA);
    Wh_SetFunctionHook((void*)TextOutW, (void*)TextOutWHook, (void**)&pOriginalTextOutW);

    Wh_SetFunctionHook((void*)ExtTextOutA, (void*)ExtTextOutAHook, (void**)&pOriginalExtTextOutA);
    Wh_SetFunctionHook((void*)ExtTextOutW, (void*)ExtTextOutWHook, (void**)&pOriginalExtTextOutW);

    Wh_SetFunctionHook((void*)DrawTextA, (void*)DrawTextAHook, (void**)&pOriginalDrawTextA);
    Wh_SetFunctionHook((void*)DrawTextW, (void*)DrawTextWHook, (void**)&pOriginalDrawTextW);

    Wh_SetFunctionHook((void*)DrawTextExA, (void*)DrawTextExAHook, (void**)&pOriginalDrawTextExA);
    Wh_SetFunctionHook((void*)DrawTextExW, (void*)DrawTextExWHook, (void**)&pOriginalDrawTextExW);

    Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExAHook, (void**)&pOriginalCreateWindowExA);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);

    Wh_SetFunctionHook((void*)SendMessageA, (void*)SendMessageAHook, (void**)&pOriginalSendMessageA);
    Wh_SetFunctionHook((void*)SendMessageW, (void*)SendMessageWHook, (void**)&pOriginalSendMessageW);

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
