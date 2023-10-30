// ==WindhawkMod==
// @id              notepad-clientedge
// @name            Notepad Clientedge
// @description     Applies client edge to newer versions of Notepad
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         notepad.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notepad Clientedge
In a Windows 10 update, the `WS_EX_CLIENTEDGE` style was removed from Notepad's
edit control, resulting in an inaccurate appearance on classic theme or other
themes of old Windows versions. This mod adds that back.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/notepad-clientedge-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/notepad-clientedge-after.png)
*/
// ==/WindhawkModReadme==

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig;
HWND WINAPI CreateWindowExW_hook(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
)
{
    if (hWndParent != NULL
    && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0
    &&  !wcsicmp(lpClassName, L"EDIT"))
    {
        WCHAR szParentClass[256];
        if (GetClassNameW(hWndParent, szParentClass, 256))
        {
            if (!wcsicmp(szParentClass, L"Notepad"))
            {
                dwExStyle |= WS_EX_CLIENTEDGE;
            }
        }
    }

    return CreateWindowExW_orig(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );
}

BOOL Wh_ModInit(void)
{
    if (!Wh_SetFunctionHook(
        (void *)CreateWindowExW,
        (void *)CreateWindowExW_hook,
        (void **)&CreateWindowExW_orig
    ))
    {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}