// ==WindhawkMod==
// @id              regedit-fix-copy-key-name
// @name            RegEdit Fix "Copy Key Name"
// @description     Makes the "Copy Key Name" context menu option actually copy the name of the key instead of the path.
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         regedit.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# RegEdit Fix "Copy Key Name"
Makes the "Copy Key Name" context menu option actually copy the name of the key instead of the path.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#define THISCALL  __cdecl
#define STHISCALL L"__cdecl"
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define THISCALL  __thiscall
#define STHISCALL L"__thiscall"
#define STDCALL  __stdcall
#define SSTDCALL L"__stdcall"
#endif

BOOL g_flag;
BOOL g_isKeyNameStored;
WCHAR g_keyName[2048]; // I don't think there's a way to define this buffer without hardcoding the size (using the size from KeyTree_BuildKeyPath()'s cchKey parameter).

typedef void (THISCALL *OnCopyKeyName_t)(void*, HWND, _TREEITEM*);
OnCopyKeyName_t OnCopyKeyName_orig;
void THISCALL OnCopyKeyName_hook(void *pThis, HWND hWnd, _TREEITEM *pTreeItem) {
    g_flag = true;

    OnCopyKeyName_orig(pThis, hWnd, pTreeItem);

    g_flag = false;
}

typedef HKEY (STDCALL *KeyTree_BuildKeyPath_t)(HWND, _TREEITEM*, LPWSTR, size_t, unsigned int);
KeyTree_BuildKeyPath_t KeyTree_BuildKeyPath_orig;
HKEY STDCALL KeyTree_BuildKeyPath_hook
(
    HWND hWnd,
    _TREEITEM *pTreeItem,
    LPWSTR pszKeyPath, // key path buffer
    size_t cchKeyPath,
    unsigned int param5 // I don't know what this parameter is for, I didn't bother reversing it further.
) {
    HKEY ret = KeyTree_BuildKeyPath_orig(hWnd, pTreeItem, pszKeyPath, cchKeyPath, param5); // the function must be called first, as it will write to pszKeyPath

    if(g_flag) {
        wcscpy_s(pszKeyPath, cchKeyPath, g_keyName); // copy the key name into the key path buffer (which is afterwards copied to the clipboard)
        g_isKeyNameStored = false;
    }

    return ret;
}

typedef HRESULT (STDCALL *StringCchCatW_t)(LPWSTR, size_t, LPCWSTR);
StringCchCatW_t StringCchCatW_orig;
HRESULT STDCALL StringCchCatW_hook(LPWSTR pszDest, size_t cchDest, LPCWSTR pszSrc) {
    if(g_flag && !g_isKeyNameStored && _wcsicmp(pszSrc, L"\\")) {
        wcscpy_s(g_keyName, pszSrc); // store the key name into g_keyName
        g_isKeyNameStored = true;
    }

    return StringCchCatW_orig(pszDest, cchDest, pszSrc);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    HMODULE hRegEdit = LoadLibraryW(L"regedit.exe");
    if (!hRegEdit)
    {
        Wh_Log(L"Failed to load regedit.exe");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"private: void " STHISCALL " RegEdit::OnCopyKeyName(struct HWND__ *,struct _TREEITEM *)"
            },
            &OnCopyKeyName_orig,
            OnCopyKeyName_hook,
            false
        },
        {
            {
                L"struct HKEY__ * " SSTDCALL " KeyTree_BuildKeyPath(struct HWND__ *,struct _TREEITEM *,unsigned short *,unsigned long,unsigned int)"
            },
            &KeyTree_BuildKeyPath_orig,
            KeyTree_BuildKeyPath_hook,
            false
        },
        {
            {
                L"long " SSTDCALL " StringCchCatW(unsigned short *,unsigned __int64,unsigned short const *)"
            },
            &StringCchCatW_orig,
            StringCchCatW_hook,
            false
        },
    };

    if (!WindhawkUtils::HookSymbols(hRegEdit, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook one or more functions");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
