// ==WindhawkMod==
// @id              tortoisegit-progress-animation-background-fix
// @name            TortoiseGit progress animation background fix for classic dark theme
// @description     Fixes progress animation background in classic dark theme by replacing white background with a classic button face colour
// @version         1.0.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -lgdi32
// @include         TortoiseGitProc.exe
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
# TortoiseGit progress animation background fix for classic dark theme

Under classic dark theme, TortoiseGit progress animation has a white background, thus looking as if something is broken.

This mod fixes the progress animation background in classic dark theme by replacing the white background with a classic button face colour.

Before:

![Before](https://raw.githubusercontent.com/levitation-opensource/my-windhawk-mods/main/screenshots/before-tortoisegit-progress-animation-background.png)

After:

![After](https://raw.githubusercontent.com/levitation-opensource/my-windhawk-mods/main/screenshots/after-tortoisegit-progress-animation-background.png) 


## Known limitations

There is still a brief flash of a white coloured box before the progress animation starts. I have tried various ways, but have been unable to get rid of that. If you have an idea how to fix that, please let me know!
*/
// ==/WindhawkModReadme==


#include <windowsx.h>
#include <atomic>
#include <new>          //std::nothrow


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


const COLORREF white = RGB(255, 255, 255);


std::atomic<size_t> g_hookRefCount;


using BitBlt_t = decltype(&BitBlt);
BitBlt_t pOriginalBitBlt;


bool ControlNeedsBackgroundRepaint(HWND hWnd) {

    WCHAR szClassName[32];
    if (
        hWnd 
        && GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName))
        && _wcsicmp(szClassName, L"SysAnimate32") == 0
    ) {
        return true;
    }
    else {
        return false;
    }
}

void ConditionalFillRect(HDC hdc, const RECT& rect, COLORREF oldColor, COLORREF newColor, int newColorIndex, bool useFloodFill) {

    int pixelCount = (rect.right - rect.left) * (rect.bottom - rect.top);
    if (pixelCount == 0) {
        //Wh_Log(L"pixelCount == 0");
        return;
    }

    //Create a compatible DC and bitmap
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
            if (!pOriginalBitBlt(memDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY)) {
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
                                //start from top left corner
                                0, //left
                                0, //top
                                oldColor,
                                FLOODFILLSURFACE
                            )) {
                                //Wh_Log(L"ExtFloodFill failed");
                            }
                            else {
                                //blit the modified content back to hdc
                                if (!pOriginalBitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY))
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

                    COLORREF* pixels = new (std::nothrow) COLORREF[pixelCount];
                    if (!pixels) {
                        Wh_Log(L"Allocating pixels array failed");
                    }
                    else {
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
                                if (!pOriginalBitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY))
                                    Wh_Log(L"BitBlt to hdc failed");
                            }
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

BOOL WINAPI BitBltHook(
    IN HDC   hdcDest,
    IN int   x,
    IN int   y,
    IN int   cx,
    IN int   cy,
    IN HDC   hdcSrc,
    IN int   xSrc,
    IN int   ySrc,
    IN DWORD rop
) {
    g_hookRefCount++;

    //Wh_Log(L"BitBltHook called");


    if (
        hdcSrc
        && hdcDest
    ) {
        HWND hwndDest = WindowFromDC(hdcDest);

        if (ControlNeedsBackgroundRepaint(hwndDest)) {

            int colorIndex = COLOR_3DFACE;

            if (!GetSysColorBrush(colorIndex)) {        //Verify that the brush is supported by the current system. GetSysColor() does not have return value, so need to use GetSysColorBrush() for verification purposes.
                Wh_Log(L"GetSysColorBrush failed - is the colour supported by current OS?");
            }
            else {
                RECT rcPaint;
                rcPaint.left = x;
                rcPaint.top = y;
                rcPaint.right = x + cx;
                rcPaint.bottom = y + cy;

                COLORREF buttonFace = GetSysColor(colorIndex);
                if (buttonFace != white) {
                    ConditionalFillRect(hdcSrc, rcPaint, white, buttonFace, colorIndex, /*useFloodFill*/true);

                    //Wh_Log(L"ConditionalFillRect called");
                }
            }
        }
    }


    BOOL result = pOriginalBitBlt(
        hdcDest,
        x,
        y,
        cx,
        cy,
        hdcSrc,
        xSrc,
        ySrc,
        rop
    );


    g_hookRefCount--;

    return result;
}

void Wh_ModAfterInit(void) {

    Wh_Log(L"Initialising hooks done");
}

BOOL Wh_ModInit() {

    Wh_Log(L"Init");


    HMODULE hGdi32 = GetModuleHandleW(L"gdi32.dll");
    if (!hGdi32) {
        Wh_Log(L"Loading gdi32.dll failed");
        return FALSE;
    }

    FARPROC pBitBlt = GetProcAddress(hGdi32, "BitBlt");
    if (!pBitBlt) {
        Wh_Log(L"Finding hookable functions from gdi32.dll failed");
        return FALSE;
    }


    Wh_Log(L"Initialising hooks...");

    Wh_SetFunctionHookT(pBitBlt, BitBltHook, &pOriginalBitBlt);

    return TRUE;
}

void Wh_ModUninit() {

    Wh_Log(L"Uniniting...");


    //Wait for the hooked calls to exit. I have seen programs crashing during this mod's unload without this.
    do {    //first sleep, then check g_hookRefCount since some hooked function might have a) entered, but not increased g_hookRefCount yet, or b) has decremented g_hookRefCount but not returned to the caller yet
        if (g_hookRefCount)
            Wh_Log(L"g_hookRefCount: %lli", (long long)g_hookRefCount);

        Sleep(1000);    //NB! Sleep always at least once. See the comment at the "do" keyword.

    } while (g_hookRefCount > 0);


    Wh_Log(L"Uninit complete");
}
