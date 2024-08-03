// ==WindhawkMod==
// @id              fix-qt-scrollbar-grippers
// @name            Fix Qt scrollbar grippers
// @description     Fixes scrollbar grippers in Qt 6.1+
// @version         1.0
// @author          kawapure
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io
// @include         ida64.exe
// @include         ida.exe
// @compilerOptions -lcomdlg32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Qt scrollbar grippers

![Preview](https://raw.githubusercontent.com/kawapure/images/main/fix-qt-scrollbar-grippers.png)

Qt applications compiled after 2021 (against Qt 6.1+) lack the scrollbar grippers (the lines in the middle),
visible on custom visual styles, because the responsible drawing code was removed. Here is a lazy hack around
that.

These were removed in Qt [because the developers thought that these changes wouldn't affect Windows 10](https://github.com/qt/qtbase/commit/5f5c342924a0d9a2856b2f2d6db373e25723f2b0).
They were wrong.

## How to enable the mod for a program

This mod targets the Interactive Disassembler (IDA) by default. Other applications must be manually specified
for injection.

In Windhawk, go to the "Advanced" tab and scroll down to 
"Custom process inclusion list". In that box, put the filename of the `.exe`. 
The mod will immediately apply to those programs after you click "Save". 
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vsstyle.h>

using DrawThemeBackgroundEx_t = decltype(&DrawThemeBackgroundEx);
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_orig;
HRESULT WINAPI DrawThemeBackgroundEx_hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions)
{
    HRESULT result = DrawThemeBackgroundEx_orig(hTheme, hdc, iPartId, iStateId, pRect, pOptions);

    if (iPartId == SBP_THUMBBTNHORZ || iPartId == SBP_THUMBBTNVERT)
    {
        if (iPartId == SBP_THUMBBTNHORZ)
            Wh_Log(L"dtb: is SBP_THUMBBTNHORZ");
        else if (iPartId == SBP_THUMBBTNVERT)
            Wh_Log(L"dtb: is SBP_THUMBBTNVERT");

        int gripperPart = iPartId == SBP_THUMBBTNHORZ
                ? SBP_GRIPPERHORZ
                : SBP_GRIPPERVERT;

        SIZE gripperSize = { 0 };
        GetThemePartSize(hTheme, hdc, gripperPart, iStateId, nullptr, TS_TRUE, &gripperSize);

        if (
            (iPartId == SBP_THUMBBTNVERT && (pRect->bottom - pRect->top) > gripperSize.cy) ||
            (iPartId == SBP_THUMBBTNHORZ && (pRect->right - pRect->left) > gripperSize.cx)
        )
        {
            Wh_Log(L"drawing gripper");

            DrawThemeBackground(hTheme, hdc, gripperPart, iStateId, pRect, nullptr);
        }
    }

    return result;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    Wh_SetFunctionHook(
        (void *)DrawThemeBackgroundEx,
        (void *)DrawThemeBackgroundEx_hook,
        (void **)&DrawThemeBackgroundEx_orig
    );

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}
