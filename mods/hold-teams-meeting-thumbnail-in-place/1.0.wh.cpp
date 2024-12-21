// ==WindhawkMod==
// @id              hold-teams-meeting-thumbnail-in-place
// @name            Hold Teams meeting thumbnail in place
// @description     Prevent Teams from periodically rearranging the meeting thumbnail window
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -lkernel32 -luser32
// @include         ms-teams.exe
// @include         msteams.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/levitation-opensource/my-windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/levitation-opensource/my-windhawk-mods/

// ==WindhawkModReadme==
/*
# Maintain Teams meeting thumbnail window position

By default, Teams has an annoying feature. When Teams meeting window is minimised into a small overlay, and the user manually places that overlay into some location, then Teams later moves the overlay window into another location. If the user moves it back to their desired location, then after some time Teams overrides that again. This obviously interferes with usage of other programs.

Lets stop the micromanagement!

Current mod lets the user change the location of the minimised meeting thumbnail window as usual, but prevents Teams from moving the thumbnail window around on its own.
*/
// ==/WindhawkModReadme==


#include <windowsx.h>
#include <atomic>
#include <memory>
#include <string>


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


constexpr const wchar_t* matchClass = L"TeamsWebView";
constexpr const wchar_t* matchTitle = L"Meeting compact view";
constexpr int matchTitleLen = std::char_traits<wchar_t>::length(matchTitle);


using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t pOriginalSetWindowPos;
using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t pOriginalMoveWindow;
using SetWindowPlacement_t = decltype(&SetWindowPlacement);
SetWindowPlacement_t pOriginalSetWindowPlacement;


std::atomic<size_t> g_hookRefCount;


void IncrementHookRefCountA(LPCSTR tag) {

    g_hookRefCount++;
}

void DecrementHookRefCountA(LPCSTR tag) {

    g_hookRefCount--;
}

auto HookRefCountScopeA(LPCSTR tag) {

    IncrementHookRefCountA(tag);

    return std::unique_ptr<
        const CHAR,
        void(*)(LPCSTR)
    >{
        tag, [](LPCSTR tag) {
            DecrementHookRefCountA(tag);
        }
    };
}

#define AUTO_HOOK_COUNT_SCOPE           auto hookScope = HookRefCountScopeA(__func__);   //deny mod unload until this function exits


bool IsThumbnailWindow(HWND hWnd) {

    WCHAR szClassName[32] = { 0 };
    WCHAR szWindowTitle[32] = { 0 };
    if (
        hWnd
        && IsWindowVisible(hWnd)    //allow setting window position once when it is being shown for the first time
        && GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName))     //nMaxCount - The length of the lpClassName buffer, in characters. The buffer must be large enough to include the terminating null character; otherwise, the class name string is truncated to nMaxCount-1 characters.
        && wcsicmp(szClassName, matchClass) == 0
        && GetWindowTextW(hWnd, szWindowTitle, ARRAYSIZE(szWindowTitle))    //lpString - If the string is as long or longer than the buffer, the string is truncated and terminated with a null character.
        && wcsnicmp(szWindowTitle, matchTitle, matchTitleLen) == 0      //match only the beginning of the title
    ) {
        //Wh_Log(L"IsThumbnailWindow, title: %ls", szWindowTitle);

        return true;
    }
    else {
        //Wh_Log(L"NonThumbnailWindow, class and title: %ls %ls", szClassName, szWindowTitle);

        return false;
    }
}

//currently Teams uses only this API, but hooking other window move API-s as well just in case
BOOL WINAPI SetWindowPosHook(
    IN           HWND hWnd,
    IN OPTIONAL  HWND hWndInsertAfter,
    IN           int  X,
    IN           int  Y,
    IN           int  cx,
    IN           int  cy,
    IN           UINT uFlags
) {
    AUTO_HOOK_COUNT_SCOPE;

    if (
        hWnd
        && !(uFlags & SWP_NOMOVE)
        && IsThumbnailWindow(hWnd)      //match only the beginning of the title
    ) {
        Wh_Log(L"SetWindowPos move overridden");

        uFlags |= SWP_NOMOVE;
    }

    BOOL result = pOriginalSetWindowPos(
        hWnd,
        hWndInsertAfter,
        X,
        Y,
        cx,
        cy,
        uFlags
    );
    return result;
}

BOOL WINAPI MoveWindowHook(
    IN HWND hWnd,
    IN int  X,
    IN int  Y,
    IN int  nWidth,
    IN int  nHeight,
    IN BOOL bRepaint
) {
    AUTO_HOOK_COUNT_SCOPE;

    if (
        hWnd
        && IsThumbnailWindow(hWnd)      //match only the beginning of the title
    ) {
        Wh_Log(L"MoveWindow overridden");

        UINT uFlags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
        if (bRepaint) {   //Indicates whether the window is to be repainted. If this parameter is TRUE, the window receives a message. 
            //uFlags |= SWP_NOCOPYBITS;   //Discards the entire contents of the client area. If this flag is not specified, the valid contents of the client area are saved and copied back into the client area after the window is sized or repositioned.
        }
        else {    //If the parameter is FALSE, no repainting of any kind occurs. This applies to the client area, the nonclient area (including the title bar and scroll bars), and any part of the parent window uncovered as a result of moving a child window.
            uFlags |= SWP_NOREDRAW;     //If this flag is set, no repainting of any kind occurs. This applies to the client area, the nonclient area (including the title bar and scroll bars), and any part of the parent window uncovered as a result of the window being moved.
        }

        BOOL result = pOriginalSetWindowPos(
            hWnd,
            HWND_TOP,   //unused
            0,          //unused
            0,          //unused
            nWidth,
            nHeight,
            uFlags
        );
        return result;
    }
    else {
        BOOL result = pOriginalMoveWindow(
            hWnd,
            X,
            Y,
            nWidth,
            nHeight,
            bRepaint
        );
        return result;
    }
}

BOOL WINAPI SetWindowPlacementHook(
    IN HWND                     hWnd,
    IN const WINDOWPLACEMENT*   lpwndpl
) {
    AUTO_HOOK_COUNT_SCOPE;

    if (
        hWnd
        && lpwndpl
        && IsThumbnailWindow(hWnd)      //match only the beginning of the title
    ) {
        Wh_Log(L"SetWindowPlacement overridden");

        if (lpwndpl->length != sizeof(WINDOWPLACEMENT)) {

            Wh_Log(L"lpwndpl->length != sizeof(WINDOWPLACEMENT)");
            return FALSE;   //block the API regardless
        }
        else {
            WINDOWPLACEMENT currentPlacement;
            currentPlacement.length = sizeof(WINDOWPLACEMENT);
            if (!GetWindowPlacement(
                hWnd,
                &currentPlacement
            )) {
                Wh_Log(L"GetWindowPlacement failed");
                return FALSE;   //block the API regardless
            }
            else {
                WINDOWPLACEMENT overriddenPlacement = *lpwndpl;


                //keep current position
                overriddenPlacement.rcNormalPosition.top = currentPlacement.rcNormalPosition.top;
                overriddenPlacement.rcNormalPosition.left = currentPlacement.rcNormalPosition.left;

                //allow updating the dimensions
                long desiredHeight = lpwndpl->rcNormalPosition.bottom - lpwndpl->rcNormalPosition.top;
                long desiredWidth = lpwndpl->rcNormalPosition.right - lpwndpl->rcNormalPosition.left;
                overriddenPlacement.rcNormalPosition.bottom = currentPlacement.rcNormalPosition.top + desiredHeight;
                overriddenPlacement.rcNormalPosition.right = currentPlacement.rcNormalPosition.left + desiredWidth;


                BOOL result = pOriginalSetWindowPlacement(
                    hWnd,
                    &overriddenPlacement
                );
                return result;
            }
        }
    }
    else {
        BOOL result = pOriginalSetWindowPlacement(
            hWnd,
            lpwndpl
        );
        return result;
    }
}

void Wh_ModAfterInit(void) {

    Wh_Log(L"Initialising hooks done");
}

BOOL Wh_ModInit() {

    Wh_Log(L"Init");


    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"Loading user32.dll failed");
        return FALSE;
    }
    
    FARPROC pSetWindowPos = GetProcAddress(hUser32, "SetWindowPos");
    FARPROC pMoveWindow = GetProcAddress(hUser32, "MoveWindow");
    FARPROC pSetWindowPlacement = GetProcAddress(hUser32, "SetWindowPlacement");
    if (
        !pSetWindowPos
        || !pMoveWindow
        || !pSetWindowPlacement
    ) {
        Wh_Log(L"Finding hookable functions from user32.dll failed");
        return FALSE;
    }
    

    Wh_Log(L"Initialising hooks...");

    Wh_SetFunctionHookT(pSetWindowPos, SetWindowPosHook, &pOriginalSetWindowPos);
    Wh_SetFunctionHookT(pMoveWindow, MoveWindowHook, &pOriginalMoveWindow);
    Wh_SetFunctionHookT(pSetWindowPlacement, SetWindowPlacementHook, &pOriginalSetWindowPlacement);

    return TRUE;
}

void Wh_ModUninit() {

    Wh_Log(L"Uninit");


    //Wait for hooks to exit. I have seen programs crashing during mod unload without this.
    do {    //first sleep, then check g_hookRefCount since some hooked function might have a) entered, but not increased g_hookRefCount yet, or b) has decremented g_hookRefCount but not returned to the caller yet
        if (g_hookRefCount)
            Wh_Log(L"g_hookRefCount: %lli", (long long)g_hookRefCount);

        Sleep(1000);    //NB! Sleep always at least once. See the comment at the "do" keyword.

    } while (g_hookRefCount > 0);
}
