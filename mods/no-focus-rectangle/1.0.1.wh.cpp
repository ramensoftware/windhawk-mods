// ==WindhawkMod==
// @id              no-focus-rectangle
// @name            No Focus Rectangle
// @description     Removes the focus rectangle
// @version         1.0.1
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# No Focus Rectangle
Removes the dotted focus rectangle, that it sometimes shown when deselecting files, especially on the desktop. This is appearently an intended feature, but it is pretty annoying.

The code is based on [NoDrawFocusRectW11](https://github.com/NoDrawFocusRectW11/NoDrawFocusRectW11).

This is how the focus rectangle looks like, by default:

![Screenshot](https://raw.githubusercontent.com/NoDrawFocusRectW11/NoDrawFocusRectW11/main/SelectedWithFocusRect.png)

And, this is the focus rectangle removed, with the mod installed:

![Screenshot](https://raw.githubusercontent.com/NoDrawFocusRectW11/NoDrawFocusRectW11/main/NoFocusRect.png)
*/
// ==/WindhawkModReadme==



BOOL(*pOriginalDrawFocusRect)(
    HDC hDC,
    const RECT *lprc
);

BOOL DrawFocusRectHook(
    HDC hDC,
    const RECT *lprc
) {
    return TRUE; // Returning TRUE because returning FALSE would indicate that the function has failed.
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "DrawFocusRect");
    Wh_SetFunctionHook(origFunc, (void*)DrawFocusRectHook, (void**)&pOriginalDrawFocusRect);

    return TRUE;
}


void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}