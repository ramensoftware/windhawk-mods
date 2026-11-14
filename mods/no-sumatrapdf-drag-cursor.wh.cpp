// ==WindhawkMod==
// @id              no-sumatrapdf-drag-cursor
// @name            No SumatraPDF Drag Cursor
// @description     Remove the drag cursor which is shown when panning a document by dragging with the cursor.
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         SumatraPDF.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# No SumatraPDF Drag Cursor

This mod removes the drag cursor which is shown when panning a document by dragging with the cursor.
![SumatraPDF Drag Cursor](https://raw.githubusercontent.com/ItsProfessional/Screenshots/refs/heads/main/Windhawk/no-sumatrapdf-drag-cursor.png)
*/
// ==/WindhawkModReadme==

#include <Windows.h>

HCURSOR g_hCursor{0};

using LoadCursor_t = decltype(&LoadCursor);
LoadCursor_t LoadCursor_orig;
HCURSOR WINAPI LoadCursor_hook(HINSTANCE hInstance, LPCWSTR lpCursorName) {
    HCURSOR hCursor = LoadCursor_orig(hInstance, lpCursorName);
    
    if(!g_hCursor && IS_INTRESOURCE(lpCursorName) && (ULONG_PTR)lpCursorName == 132)
        g_hCursor = hCursor;

    return hCursor;
}

using SetCursor_t = decltype(&SetCursor);
SetCursor_t SetCursor_orig;
HCURSOR WINAPI SetCursor_hook(HCURSOR hCursor) {
    if(hCursor == g_hCursor) return GetCursor();

    return SetCursor_orig(hCursor);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    if (!Wh_SetFunctionHook((void*)LoadCursor, (void*)LoadCursor_hook, (void**)&LoadCursor_orig))
    {
        Wh_Log(L"Failed to hook LoadCursor");
        return FALSE; 
    }

    if (!Wh_SetFunctionHook((void*)SetCursor, (void*)SetCursor_hook, (void**)&SetCursor_orig))
    {
        Wh_Log(L"Failed to hook SetCursor");
        return FALSE; 
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
