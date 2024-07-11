// ==WindhawkMod==
// @id              classic-taskbar-background-fix
// @name            Classic Taskbar background fix
// @description     Fixes Taskbar background in classic theme by replacing black background with a classic button face colour
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -luser32 -lgdi32
// @include         explorer.exe
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
# Classic Taskbar background fix

Fixes Taskbar background in classic theme by replacing black background with a classic button face colour.

Before:

![Before](https://raw.githubusercontent.com/levitation-opensource/my-windhawk-mods/main/screenshots/before-classic-taskbar-background-fix.png)

After:

![After](https://raw.githubusercontent.com/levitation-opensource/my-windhawk-mods/main/screenshots/after-classic-taskbar-background-fix.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RepaintDesktopButton: yes
  $name: Repaint the "Show desktop button"
  $options:
  - yes: Yes
  - no: No
*/
// ==/WindhawkModSettings==


#include <windowsx.h>


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


bool g_retryInitInAThread = false;
HANDLE g_initThread = NULL;
HANDLE g_initThreadStopSignal = NULL;

bool g_repaintDesktopButton = false;


typedef BOOL(WINAPI* EndPaint_t)(HWND, const PAINTSTRUCT*);
EndPaint_t pOriginalEndPaint;


bool WindowNeedsBackgroundRepaint(HWND hWnd) {

    WCHAR szClassName[32];
    if (hWnd && GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return
            _wcsicmp(szClassName, L"Shell_TrayWnd") == 0        //around of start button
            || _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0        //secondary taskbar
            || _wcsicmp(szClassName, L"MSTaskListWClass") == 0      //around of taskbar buttons
            || _wcsicmp(szClassName, L"TrayNotifyWnd") == 0     //around of tray
            || _wcsicmp(szClassName, L"Button") == 0        //three dots 
            || _wcsicmp(szClassName, L"ToolbarWindow32") == 0      //tray icons
            || _wcsicmp(szClassName, L"TrayClockWClass") == 0       //clock
            || _wcsicmp(szClassName, L"TrayButton") == 0        //action center button
            || (g_repaintDesktopButton && _wcsicmp(szClassName, L"TrayShowDesktopButtonWClass") == 0)   //show desktop button
            ;
    }
    else {
        return false;
    }
}

void ConditionalFillRect(HDC hdc, const RECT& rect, COLORREF oldColor, COLORREF newColor) {

    //create a compatible DC and bitmap
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
    HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

    //copy the existing content from hdc
    BitBlt(memDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);

    //get pixels array from bitmap
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rect.right - rect.left;
    bmi.bmiHeader.biHeight = -(rect.bottom - rect.top);     //negative height to ensure top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;  //32-bit color depth
    bmi.bmiHeader.biCompression = BI_RGB;

    int pixelCount = (rect.right - rect.left) * (rect.bottom - rect.top);
    COLORREF* pixels = new COLORREF[pixelCount];
    GetDIBits(memDC, memBitmap, 0, rect.bottom - rect.top, pixels, &bmi, DIB_RGB_COLORS);

    //modify the pixels
    for (int i = 0; i < pixelCount; ++i) {
        if (pixels[i] == oldColor)
            pixels[i] = newColor;
    }

    //save pixels array back to bitmap
    SetDIBits(memDC, memBitmap, 0, rect.bottom - rect.top, pixels, &bmi, DIB_RGB_COLORS);
    delete[] pixels;

    //blit the modified content back to hdc
    BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

    //clean up
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

BOOL WINAPI EndPaintHook(
    IN HWND                 hWnd,
    IN const PAINTSTRUCT*   lpPaint
) {
    if (lpPaint && lpPaint->hdc && WindowNeedsBackgroundRepaint(hWnd)) {

        COLORREF black = RGB(0, 0, 0);
        COLORREF buttonFace = GetSysColor(COLOR_3DFACE);
        if (buttonFace != black)
            ConditionalFillRect(lpPaint->hdc, lpPaint->rcPaint, black, buttonFace);
    }

    return pOriginalEndPaint(hWnd, lpPaint);
}


void TriggerTaskbarRepaint(HWND hwndTaskbar) {

    if (!hwndTaskbar) {     //is taskbar hwnd already detected?
        hwndTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
        Wh_Log(L"hwndTaskbar: 0x%llX", (long long)hwndTaskbar);
    }

    if (hwndTaskbar)
        RedrawWindow(hwndTaskbar, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}

bool TryInit(bool* abort, bool callApplyHookOperations) {

    HWND hwndTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    Wh_Log(L"hwndTaskbar: 0x%llX", (long long)hwndTaskbar);
    if (!hwndTaskbar)
        return false;   //retry

    DWORD taskbarProcessId;
    if (!GetWindowThreadProcessId(hwndTaskbar, &taskbarProcessId))
        return false;   //retry

    Wh_Log(L"taskbarProcessId: %u", taskbarProcessId);
    if (taskbarProcessId != GetCurrentProcessId()) {
        Wh_Log(L"Not a taskbar process, not hooking");
        *abort = true;
        return false;
    }


    Wh_Log(L"Initialising hooks...");


    HMODULE hUser32 = GetModuleHandle(L"user32.dll"); 	//when we reach here then we can be sure that user32.dll has been already loaded
    if (!hUser32) {
        Wh_Log(L"Loading user32.dll failed");
        *abort = true;
        return false;
    }

    FARPROC pEndPaint = GetProcAddress(hUser32, "EndPaint");
    if (!pEndPaint) {
        Wh_Log(L"Finding hookable functions from user32.dll failed");
        *abort = true;
        return false;
    }


    Wh_SetFunctionHookT(pEndPaint, EndPaintHook, &pOriginalEndPaint);


    if (callApplyHookOperations)
        Wh_ApplyHookOperations();   //we are running in a separate thread outside of Wh_ModInit() therefore need to call Wh_ApplyHookOperations manually


    //apply the new colour immediately
    TriggerTaskbarRepaint(hwndTaskbar);


    Wh_Log(L"Initialising hooks done");

    return true;
}

DWORD WINAPI InitThreadFunc(LPVOID param) {

    Wh_Log(L"InitThreadFunc enter");

    bool isRetry = false;
retry:  //wait until taskbar has properly initialised in order to detect whether current process will become taskbar or not
    if (isRetry) {
        if (WaitForSingleObject(g_initThreadStopSignal, 1000) != WAIT_TIMEOUT) {
            Wh_Log(L"Shutting down InitThreadFunc before success");
            return FALSE;
        }
    }
    isRetry = true;

    bool abort = false;
    if (TryInit(&abort, /*callApplyHookOperations*/true)) {
        return TRUE;  //hooks done
    }
    else if (abort) {
        return FALSE;   //if the taskbar process is already running then subsequent non-taskbar related explorer.exe instances will not be hooked
    }
    else {      //taskbar was not yet found, so we need to retry later
        goto retry;
    }
}

void Wh_ModAfterInit(void) {
    //Run the hook thread only after Wh_ModAfterInit() has been called. This is in order to avoid race condition in calling Wh_ApplyHookOperations().
    if (g_retryInitInAThread) {

        if (ResumeThread(g_initThread)) {
            Wh_Log(L"ResumeThread successful");
            g_retryInitInAThread = false;
        }
        else {
            Wh_Log(L"ResumeThread failed");
        }
    }
}

void LoadSettings() {

    //config option to keep "show desktop" button black
    //TODO: option to use some other colour, for example the colour of hidden tray icons popup panel background
    PCWSTR configString = Wh_GetStringSetting(L"RepaintDesktopButton");
    g_repaintDesktopButton = (wcscmp(configString, L"no") != 0);        //default is yes, so if it is not "no" then it is yes
    Wh_FreeStringSetting(configString);     
}

BOOL Wh_ModInit() {

    LoadSettings();

    Wh_Log(L"Init");


    bool abort = false;
    if (TryInit(&abort, /*callApplyHookOperations*/false)) {    //NB! calling Wh_ApplyHookOperations() is not allowed during Wh_ModInit()
        return TRUE;  //hooks done
    }
    else if (abort) {
        return FALSE;   //if the taskbar process is already running then subsequent non-taskbar related explorer.exe instances will not be hooked
    }
    else {      //taskbar was not yet found, maybe it is still initialising in the current process, so we need to retry later
        g_initThreadStopSignal = CreateEvent(
            /*lpEventAttributes = */NULL,           // default security attributes
            /*bManualReset = */TRUE,				// manual-reset event
            /*bInitialState = */FALSE,              // initial state is nonsignaled
            /*lpName = */NULL						// object name
        );

        if (!g_initThreadStopSignal) {
            Wh_Log(L"CreateEvent failed");
            return FALSE;
        }

        g_initThread = CreateThread(
            /*lpThreadAttributes = */NULL,
            /*dwStackSize = */0,
            InitThreadFunc,
            /*lpParameter = */NULL,
            /*dwCreationFlags = */CREATE_SUSPENDED, 	//The thread does NOT run immediately after creation. This is in order to avoid any race conditions in calling Wh_ApplyHookOperations() before Wh_ModInit() has completed and Wh_ModAfterInit() has started     //TODO: is this concern important?
            /*lpThreadId = */NULL
        );

        if (g_initThread) {
            Wh_Log(L"InitThread created");
            g_retryInitInAThread = true;
            return TRUE;
        }
        else {
            Wh_Log(L"CreateThread failed");
            CloseHandle(g_initThreadStopSignal);
            g_initThreadStopSignal = NULL;
            return FALSE;
        }
    }
}

void Wh_ModSettingsChanged() {

    Wh_Log(L"Wh_ModSettingsChanged");

    LoadSettings();

    //apply the updated colour immediately
    TriggerTaskbarRepaint(NULL);
}

void Wh_ModUninit() {

    Wh_Log(L"Uniniting...");


    if (g_initThread) {

        if (g_retryInitInAThread) {     //was the thread successfully resumed?
            if (ResumeThread(g_initThread))
                g_retryInitInAThread = false;
        }

        if (!g_retryInitInAThread) {     //was the thread successfully resumed?
            SetEvent(g_initThreadStopSignal);
            WaitForSingleObject(g_initThread, INFINITE);
            CloseHandle(g_initThread);
            g_initThread = NULL;
        }
    }

    if (
        g_initThreadStopSignal
        && !g_retryInitInAThread     //was the thread successfully resumed?
    ) {
        CloseHandle(g_initThreadStopSignal);
        g_initThreadStopSignal = NULL;
    }


    //apply the default colour immediately
    TriggerTaskbarRepaint(NULL);


    Wh_Log(L"Uninit complete");
}
