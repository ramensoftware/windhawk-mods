// ==WindhawkMod==
// @id              classic-taskbar-background-fix
// @name            Classic Taskbar background fix
// @description     Fixes Taskbar background in classic theme by replacing black background with a classic button face colour
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -luser32 -lgdi32 -luxtheme
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

Install this mod if you have issues with black background around taskbar buttons and tray icons, like illustrated in the picture below. This mod does not fix the buttons' colour, for that purpose there are other mods available.

This mod can be considered as an alternative to installing the @valinet's modified OpenShell StartMenuDLL.dll.

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
  - highlightOnHover: Yes, and use highlight colour on mouse over
  - windowBackgroundOnHover: Yes, and use window background colour on mouse over
  - no: No
*/
// ==/WindhawkModSettings==


#include <map>
#include <mutex>
#include <windowsx.h>
#include <uxtheme.h>


enum class RepaintDesktopButtonConfig {
    yes,
    highlightOnHover,
    windowBackgroundOnHover,
    no
};


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


const COLORREF black = RGB(0, 0, 0);
const int blackColorIndex = -1;    //mod's internal code for black colour

bool g_retryInitInAThread = false;
HANDLE g_initThread = NULL;
HANDLE g_initThreadStopSignal = NULL;
HWND g_hwndTaskbar = NULL;

HMODULE hUxtheme = NULL;


typedef struct tagMemDCInfo {
    HDC originalHdc;
    HBITMAP memBitmap;
    HGDIOBJ oldBitmap;
    int colorIndex;
} MemDCInfo;


std::mutex g_hdcMapMutex;
std::map<HDC, MemDCInfo> g_hdcMap;      //using map not unordered_map since the latter becomes slow when doing many insertions and removals. Also it is expected that the current map will contain only a few elements at a time

RepaintDesktopButtonConfig g_repaintDesktopButtonConfig;


using BeginPaint_t = decltype(&BeginPaint);
BeginPaint_t pOriginalBeginPaint;
using EndPaint_t = decltype(&EndPaint);
EndPaint_t pOriginalEndPaint;
//using DrawThemeParentBackground_t = decltype(&DrawThemeParentBackground);     //this declaration is a bit different in Windhawk headers than in Visual Studio, so using manual declaration instead to avoid both intellisense and compilation errors
typedef HRESULT(WINAPI* DrawThemeParentBackground_t)(HWND, HDC, const RECT*);
DrawThemeParentBackground_t pOriginalDrawThemeParentBackground;
typedef HRESULT(WINAPI* DrawThemeParentBackgroundEx_t)(HWND, HDC, DWORD, const RECT*);
DrawThemeParentBackgroundEx_t pOriginalDrawThemeParentBackgroundEx;


RepaintDesktopButtonConfig DesktopButtonConfigFromString(PCWSTR string) {
    if (wcscmp(string, L"no") == 0) {
        return RepaintDesktopButtonConfig::no;
    }
    else if (wcscmp(string, L"highlightOnHover") == 0) {
        return RepaintDesktopButtonConfig::highlightOnHover;
    }
    else if (wcscmp(string, L"windowBackgroundOnHover") == 0) {
        return RepaintDesktopButtonConfig::windowBackgroundOnHover;
    }
    else {
        return RepaintDesktopButtonConfig::yes;
    }
}


bool WindowNeedsBackgroundRepaint(OUT int* colorIndex, HWND hWnd, const RECT* paintRect, bool isDrawThemeParentBackgroundCall) {

    WCHAR szClassName[32];
    if (
        hWnd 
        && GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName))
    ) {
        if (
            (
                g_repaintDesktopButtonConfig == RepaintDesktopButtonConfig::highlightOnHover
                || g_repaintDesktopButtonConfig == RepaintDesktopButtonConfig::windowBackgroundOnHover
            )
            && _wcsicmp(szClassName, L"TrayShowDesktopButtonWClass") == 0
        ) {
            POINT cursorPos;
            if (GetCursorPos(&cursorPos)) {
                if (ScreenToClient(
                    hWnd,
                    &cursorPos
                )) {
                    bool mouseIsOverShowDesktopButton = PtInRect(
                        paintRect,
                        cursorPos
                    );

                    if (mouseIsOverShowDesktopButton) {
                        if (g_repaintDesktopButtonConfig == RepaintDesktopButtonConfig::highlightOnHover)
                            *colorIndex = COLOR_HIGHLIGHT;
                        else
                            *colorIndex = COLOR_WINDOW;
                    }
                    else {
                        *colorIndex = COLOR_3DFACE;
                    }

                    return true;
                }
            }
        }
        else if (
            isDrawThemeParentBackgroundCall        //painting the "show desktop" button black agains since withtout it earlier calls to DrawThemeParentBackground would remove the black color from "show desktop" button as a side effect of painting other tray areas
            && g_repaintDesktopButtonConfig == RepaintDesktopButtonConfig::no
            && _wcsicmp(szClassName, L"TrayShowDesktopButtonWClass") == 0   //show desktop button
        ) {
            *colorIndex = blackColorIndex;     
            return true;
        }
        else {

            bool result =
                _wcsicmp(szClassName, L"Shell_TrayWnd") == 0        //around of start button in case OpenShell is active
                || _wcsicmp(szClassName, L"Start") == 0             //around of start button in case OpenShell is NOT active
                || _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0        //secondary taskbar
                || _wcsicmp(szClassName, L"MSTaskListWClass") == 0      //around of taskbar buttons
                || (
                    isDrawThemeParentBackgroundCall
                    && (
                        _wcsicmp(szClassName, L"CiceroUIWndFrame") == 0     //language bar
                        || _wcsicmp(szClassName, L"TrayNotifyWnd") == 0     //around of tray, sometimes may be a very thin line, but could be also a wider area
                        || _wcsicmp(szClassName, L"Button") == 0        //three dots of the "show hidden icons" button
                        || _wcsicmp(szClassName, L"SysPager") == 0      //visible tray icons
                        //|| _wcsicmp(szClassName, L"ToolbarWindow32") == 0      //Background of both visible and hidden tray icons. Commenting out in order to not repaint hidden tray icons popup background. Not needed anyway, since painting SysPager class has already the desired effect.
                        || _wcsicmp(szClassName, L"TrayClockWClass") == 0       //clock
                        || _wcsicmp(szClassName, L"TrayButton") == 0        //action center button
                        || (
                            g_repaintDesktopButtonConfig == RepaintDesktopButtonConfig::yes
                            && _wcsicmp(szClassName, L"TrayShowDesktopButtonWClass") == 0   //show desktop button
                        )
                    )
                );

            *colorIndex = COLOR_3DFACE;     //unused if result == false
            return result;
        }
    }
    
    return false;
}

void ConditionalFillRect(HDC hdc, const RECT& rect, COLORREF oldColor, COLORREF newColor, int newColorIndex, bool useFloodFill) {

    int pixelCount = (rect.right - rect.left) * (rect.bottom - rect.top);
    if (pixelCount == 0) {
        //Wh_Log(L"pixelCount == 0");
        return;
    }

    //Create a compatible DC and bitmap. Even though hdc is already a memDC, we need to create one more memDC, since we need to get access to pixels using GetDIBits and SetDIBits, which does not support providing x coordinates.
    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) {
        Wh_Log(L"CreateCompatibleDC failed");
        return;
    }

    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
    if (!memBitmap) {
        Wh_Log(L"CreateCompatibleBitmap failed");
    }
    else {
        HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);
        if (!oldBitmap) {
            Wh_Log(L"SelectObject for memBitmap failed");
        }
        else {
            //copy the existing content from hdc
            if (!BitBlt(memDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY)) {
                Wh_Log(L"BitBlt to memDC failed");
            }
            else {
                if (useFloodFill) {

                    HBRUSH newBrush = GetSysColorBrush(newColorIndex);
                    if (!newBrush) {
                        Wh_Log(L"GetSysColorBrush failed - is the colour supported by current OS?");
                    }
                    else {
                        HGDIOBJ oldBrush = SelectObject(memDC, newBrush);
                        if (!oldBrush) {
                            Wh_Log(L"SelectObject for newBrush failed");
                        }
                        else {
                            if (!ExtFloodFill(
                                memDC,
                                //start from bottom right corner
                                rect.right - rect.left - 1, //right
                                rect.bottom - rect.top - 1, //bottom
                                oldColor,
                                FLOODFILLSURFACE
                            )) {
                                //Wh_Log(L"ExtFloodFill failed");
                            }
                            else {
                                //blit the modified content back to hdc
                                //TODO: try to blit directly to original hdc, non to memDC from BeginPaint?
                                if (!BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY))
                                    Wh_Log(L"BitBlt to hdc failed");
                            }

                            SelectObject(memDC, oldBrush);
                        }
                    }
                }
                else {      //use conditional colour replacement on all pixels

                    //get pixels array from bitmap
                    BITMAPINFO bmi = {};
                    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = rect.right - rect.left;
                    bmi.bmiHeader.biHeight = rect.bottom - rect.top;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = 32;  //32-bit color depth
                    bmi.bmiHeader.biCompression = BI_RGB;

                    COLORREF* pixels = new COLORREF[pixelCount];

                    int getDIBitsResult = GetDIBits(memDC, memBitmap, 0, rect.bottom - rect.top, pixels, &bmi, DIB_RGB_COLORS);
                    if (getDIBitsResult == NULL || getDIBitsResult == ERROR_INVALID_PARAMETER) {
                        Wh_Log(L"GetDIBits failed");
                        delete[] pixels;
                    }
                    else {
                        //modify the pixels
                        for (int i = 0; i < pixelCount; ++i) {
                            if (pixels[i] == oldColor)
                                pixels[i] = newColor;
                        }

                        //save pixels array back to bitmap
                        int setDIBitsResult = SetDIBits(memDC, memBitmap, 0, rect.bottom - rect.top, pixels, &bmi, DIB_RGB_COLORS);
                        delete[] pixels;

                        if (setDIBitsResult == NULL || setDIBitsResult == ERROR_INVALID_PARAMETER) {
                            Wh_Log(L"SetDIBits failed");
                        }
                        else {
                            //blit the modified content back to hdc
                            //TODO: try to blit directly to original hdc, non to memDC from BeginPaint?
                            if (!BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY))
                                Wh_Log(L"BitBlt to hdc failed");
                        }
                    }
                }
            }

            //clean up
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
        }

        DeleteDC(memDC);
    }
}

HDC WINAPI BeginPaintHook(
    IN  HWND          hWnd,
    OUT LPPAINTSTRUCT lpPaint
) {
    HDC hdc = pOriginalBeginPaint(hWnd, lpPaint);

    if (hdc != lpPaint->hdc)
        Wh_Log(L"hdc != lpPaint->hdc");

    int colorIndex;
    if (
        g_hwndTaskbar   //is the current process the taskbar process?
        && lpPaint
        && lpPaint->hdc
        && WindowNeedsBackgroundRepaint(&colorIndex, hWnd, &lpPaint->rcPaint, /*isDrawThemeParentBackgroundCall*/false)
    ) {
        //Send memDC to the caller to prevent occasional flickering. With memDC we can repaint the pixels before they are updated on screen.
        HDC memDC = CreateCompatibleDC(hdc);
        if (!memDC) {
            Wh_Log(L"CreateCompatibleDC failed");
        }
        else {
            //GetClipBox does not work well here for some reason, causing taskbar buttons to be partially updated
            BITMAP bitmapHeader = {};
            HGDIOBJ hBitmap = GetCurrentObject(lpPaint->hdc, OBJ_BITMAP);
            if (!hBitmap) {
                Wh_Log(L"GetCurrentObject failed");
            }
            else {
                if (!GetObjectW(hBitmap, sizeof(BITMAP), &bitmapHeader)) {
                    Wh_Log(L"GetObjectW failed");
                }
                else {
                    int width = bitmapHeader.bmWidth;
                    int height = bitmapHeader.bmHeight;

                    //need full bitmap copy here - could not create a smaller compatible bitmap with only the width and height of lpPaint->rcPaint since that would mess up ClientToScreen coordinates conversion in the program.
                    HBITMAP memBitmap = CreateCompatibleBitmap(lpPaint->hdc, width, height);
                    if (!memBitmap) {
                        Wh_Log(L"CreateCompatibleBitmap failed");
                    }
                    else {
                        HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);
                        if (!oldBitmap) {
                            Wh_Log(L"SelectObject failed");
                        }
                        else {
                            MemDCInfo memDCInfo;
                            memDCInfo.originalHdc = lpPaint->hdc;
                            memDCInfo.memBitmap = memBitmap;
                            memDCInfo.oldBitmap = oldBitmap;
                            memDCInfo.colorIndex = colorIndex;

                            {
                                std::lock_guard<std::mutex> guard(g_hdcMapMutex);
                                g_hdcMap.insert({ memDC, memDCInfo });
                            }

                            lpPaint->hdc = memDC;
                            return memDC;
                        }

                        DeleteObject(memBitmap);
                    }

                    DeleteDC(memDC);
                }
            }
        }
    }
    
    return hdc;
}

BOOL WINAPI EndPaintHook(
    IN HWND                 hWnd,
    IN const PAINTSTRUCT*   lpPaint
) {
    if (
        g_hwndTaskbar   //is the current process the taskbar process?
        && lpPaint
        && lpPaint->hdc
    ) {
        MemDCInfo memDCInfo;
        bool found = false;
        {
            std::lock_guard<std::mutex> guard(g_hdcMapMutex);

            auto it = g_hdcMap.find(lpPaint->hdc);
            if (it != g_hdcMap.end()) {   //there is a chance that EndPaint gets a call that is paired with BeginPaint from time before hooking or before g_hwndTaskbar was set
                found = true;
                memDCInfo = it->second;
                g_hdcMap.erase(lpPaint->hdc);
            }
        }

        if (found) {

            if (!GetSysColorBrush(memDCInfo.colorIndex)) {        //Verify that the brush is supported by the current system. GetSysColor() does not have return value, so need to use GetSysColorBrush() for verification purposes.
                Wh_Log(L"GetSysColorBrush failed - is the colour supported by current OS?");
            }
            else {
                COLORREF buttonFace = GetSysColor(memDCInfo.colorIndex);
                if (buttonFace != black) {
                    ConditionalFillRect(lpPaint->hdc, lpPaint->rcPaint, black, buttonFace, memDCInfo.colorIndex, /*useFloodFill*/true);
                }
            }


            //blit the mem DC back to original HDC
            HDC memDC = lpPaint->hdc;
            HDC hdc = memDCInfo.originalHdc;
            const RECT* rect = &lpPaint->rcPaint;

            if (!BitBlt(hdc, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, memDC, rect->left, rect->top, SRCCOPY))
                Wh_Log(L"BitBlt failed");

            //clean up
            SelectObject(memDC, memDCInfo.oldBitmap);
            DeleteObject(memDCInfo.memBitmap);

            DeleteDC(memDC);

            //pass original HDC to original EndPaint
            PAINTSTRUCT paintStruct = *lpPaint;
            paintStruct.hdc = hdc;
            return pOriginalEndPaint(hWnd, &paintStruct);
        }
    }
    
    return pOriginalEndPaint(hWnd, lpPaint);
}

bool DrawThemeParentBackgroundInternal(HWND hwnd, HDC hdc) {

    RECT rect;
    if (!GetClipBox(hdc, &rect)) {
        SetRectEmpty(&rect);
    }

    int colorIndex;
    if (
        g_hwndTaskbar   //is the current process the taskbar process?
        && WindowNeedsBackgroundRepaint(&colorIndex, hwnd, &rect, /*isDrawThemeParentBackgroundCall*/true)        //needed in order to not repaint hidden tray icons popup
        ) {
        if (colorIndex == blackColorIndex) {  //Repaint the area as black again. Used for "show desktop" button if the mod settings say so.

            HBRUSH brush = CreateSolidBrush(black);
            if (brush) {            //verify that the brush is supported by the current system
                HGDIOBJ holdbrush = SelectObject(hdc, brush);
                if (holdbrush && holdbrush != HGDI_ERROR) {

                    FillRect(hdc, &rect, brush);
                    SelectObject(hdc, holdbrush);

                    return true;
                }
            }
        }
        else {
            HBRUSH brush = GetSysColorBrush(colorIndex);
            if (brush) {            //verify that the brush is supported by the current system
                HGDIOBJ holdbrush = SelectObject(hdc, brush);
                if (holdbrush && holdbrush != HGDI_ERROR) {

                    FillRect(hdc, &rect, brush);
                    SelectObject(hdc, holdbrush);  // select old brush

                    return true;
                }
            }
        }
    }

    return false;
}

HRESULT WINAPI DrawThemeParentBackgroundHook(
    IN HWND                     hwnd,
    IN HDC                      hdc,
    IN OPTIONAL const RECT*     prc
) {
    if (DrawThemeParentBackgroundInternal(hwnd, hdc)) {
        return S_OK;
    }
    else {
        return pOriginalDrawThemeParentBackground(
            hwnd,
            hdc,
            prc
        );
    }
}

//currently explorer.exe uses only DrawThemeParentBackground, but I am hooking DrawThemeParentBackgroundEx anyway - just in case explorer will switch to this alternative API in the future
HRESULT WINAPI DrawThemeParentBackgroundExHook(
    IN HWND       hwnd,
    IN HDC        hdc,
    IN DWORD      dwFlags,
    IN const RECT* prc
) {
    if (DrawThemeParentBackgroundInternal(hwnd, hdc)) {
        return S_OK;
    }
    else {
        return pOriginalDrawThemeParentBackgroundEx(
            hwnd,
            hdc,
            dwFlags,
            prc
        );
    }
}


void TriggerTaskbarRepaint() {

    HWND hwndTaskbar = g_hwndTaskbar;
    if (hwndTaskbar)
        RedrawWindow(hwndTaskbar, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}

bool TryInit(bool* abort, bool canTriggerRepaint) {

    HWND hwndTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    Wh_Log(L"hwndTaskbar: 0x%llX", (long long)hwndTaskbar);
    if (!hwndTaskbar)
        return false;   //retry

    DWORD taskbarProcessId;
    if (
		!GetWindowThreadProcessId(hwndTaskbar, &taskbarProcessId) 
		|| !taskbarProcessId
	) {
        return false;   //retry
	}

    Wh_Log(L"taskbarProcessId: %u", taskbarProcessId);
    if (taskbarProcessId != GetCurrentProcessId()) {
        Wh_Log(L"Not a taskbar process, not hooking");
        *abort = true;
        return false;
    }
    else {
        g_hwndTaskbar = hwndTaskbar;
    }


    if (canTriggerRepaint) {
        TriggerTaskbarRepaint();
    }


    Wh_Log(L"Initialising taskbar hwnd done");

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
    if (TryInit(&abort, /*canTriggerRepaint*/true)) {
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

    Wh_Log(L"Initialising hooks done");


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
    else {  //if the init was done in Wh_ModInit then hooking was done only after that and taskbar repaint was not yet possible until now

        //apply the updated colour immediately
        TriggerTaskbarRepaint();
    }
}

void LoadSettings() {

    //config option to keep "show desktop" button black
    PCWSTR configString = Wh_GetStringSetting(L"RepaintDesktopButton");
    g_repaintDesktopButtonConfig = DesktopButtonConfigFromString(configString);

    Wh_FreeStringSetting(configString);     
}

BOOL Wh_ModInit() {

    LoadSettings();

    Wh_Log(L"Init");


    HMODULE hUser32 = GetModuleHandle(L"user32.dll"); 	//when we reach here then we can be sure that user32.dll has been already loaded
    if (!hUser32) {
        Wh_Log(L"Loading user32.dll failed");
        return FALSE;
    }

    FARPROC pBeginPaint = GetProcAddress(hUser32, "BeginPaint");
    FARPROC pEndPaint = GetProcAddress(hUser32, "EndPaint");
    if (
        !pEndPaint
        || !pBeginPaint
    ) {
        Wh_Log(L"Finding hookable functions from user32.dll failed");
        return FALSE;
    }


    hUxtheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxtheme) {
        Wh_Log(L"Loading uxtheme.dll failed");
        return FALSE;
    }

    FARPROC pDrawThemeParentBackground = GetProcAddress(hUxtheme, "DrawThemeParentBackground");
    FARPROC pDrawThemeParentBackgroundEx = GetProcAddress(hUxtheme, "DrawThemeParentBackgroundEx");
    if (
        !pDrawThemeParentBackground
        || !pDrawThemeParentBackgroundEx
    ) {
        Wh_Log(L"Finding hookable functions from uxtheme.dll failed");
        return FALSE;
    }


    bool abort = false;
    if (TryInit(&abort, /*canTriggerRepaint*/false)) {    //NB! calling TriggerTaskbarRepaint() would be premature during Wh_ModInit()
        //set hooks at the end and then exit the function
    }
    else if (abort) {
        return FALSE;   //if the taskbar process is already running then subsequent non-taskbar related explorer.exe instances will not be hooked
    }
    else {      
        //Taskbar was not yet found, maybe it is still initialising in the current process, so we need to retry later.
        //Hooking CreateWindowExW would cause potential instabilities during mod unload therefore using polling thread instead.

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
            /*dwCreationFlags = */CREATE_SUSPENDED, 	//The thread does NOT run immediately after creation. This is in order to avoid any race conditions in calling TriggerTaskbarRepaint() from the thread before Wh_ModInit() has completed and hooks are activated
            /*lpThreadId = */NULL
        );

        if (g_initThread) {
            Wh_Log(L"InitThread created");
            g_retryInitInAThread = true;
            //set hooks at the end and then exit the function
        }
        else {
            Wh_Log(L"CreateThread failed");
            CloseHandle(g_initThreadStopSignal);
            g_initThreadStopSignal = NULL;
            return FALSE;
        }
    }


    Wh_Log(L"Initialising hooks...");

    Wh_SetFunctionHookT(pBeginPaint, BeginPaintHook, &pOriginalBeginPaint);
    Wh_SetFunctionHookT(pEndPaint, EndPaintHook, &pOriginalEndPaint);
    Wh_SetFunctionHookT(pDrawThemeParentBackground, DrawThemeParentBackgroundHook, &pOriginalDrawThemeParentBackground);
    Wh_SetFunctionHookT(pDrawThemeParentBackgroundEx, DrawThemeParentBackgroundExHook, &pOriginalDrawThemeParentBackgroundEx);

    return TRUE;
}

void Wh_ModSettingsChanged() {

    Wh_Log(L"Wh_ModSettingsChanged");

    LoadSettings();

    //apply the updated colour immediately
    TriggerTaskbarRepaint();
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
        //we could close the signal handle regardless whether the thread was successfully resumed since if the thread resume failed then the program will crash anyway IF the mod is unloaded AND the thread is manually resumed later by somebody. But just in case hoping that maybe the mod DLL will not be unloaded as long as it has threads, then lets keep the signal handle alive as well.

        CloseHandle(g_initThreadStopSignal);
        g_initThreadStopSignal = NULL;
    }


    if (hUxtheme) {
        FreeLibrary(hUxtheme);
        hUxtheme = NULL;
    }


    //apply the default colour immediately
    TriggerTaskbarRepaint();


    Wh_Log(L"Uninit complete");
}
