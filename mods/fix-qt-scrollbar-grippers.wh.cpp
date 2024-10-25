// ==WindhawkMod==
// @id              fix-qt-scrollbar-grippers
// @name            Fix Qt scrollbar grippers
// @description     Fixes scrollbar grippers in Qt 6.1+
// @version         1.1
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
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vector>
#include <algorithm>

std::vector<HTHEME> g_hScrollbarThemes;

using OpenThemeData_t = decltype(&OpenThemeData);
OpenThemeData_t OpenThemeData_orig;
HTHEME OpenThemeData_hook(HWND hwnd, LPCWSTR pszClassList)
{
    HTHEME result = OpenThemeData_orig(hwnd, pszClassList);

    if (pszClassList && wcscmp(pszClassList, L"SCROLLBAR") == 0)
    {
        g_hScrollbarThemes.push_back(result);
    }

    return result;
}

using CloseThemeData_t = decltype(&CloseThemeData);
CloseThemeData_t CloseThemeData_orig;
HRESULT CloseThemeData_hook(HTHEME hTheme)
{
    if (std::find(g_hScrollbarThemes.begin(), g_hScrollbarThemes.end(), hTheme) != g_hScrollbarThemes.end())
    {
        g_hScrollbarThemes.erase(std::remove(g_hScrollbarThemes.begin(), g_hScrollbarThemes.end(), hTheme), g_hScrollbarThemes.end());
    }

    return CloseThemeData_orig(hTheme);
}

using DrawThemeBackgroundEx_t = decltype(&DrawThemeBackgroundEx);
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_orig;
HRESULT WINAPI DrawThemeBackgroundEx_hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions)
{
    HRESULT result = DrawThemeBackgroundEx_orig(hTheme, hdc, iPartId, iStateId, pRect, pOptions);

    if (
        !g_hScrollbarThemes.empty() &&
        std::find(g_hScrollbarThemes.begin(), g_hScrollbarThemes.end(), hTheme) != g_hScrollbarThemes.end() &&
        (iPartId == SBP_THUMBBTNHORZ || iPartId == SBP_THUMBBTNVERT)
    )
    {
        if (iPartId == SBP_THUMBBTNHORZ)
            Wh_Log(L"dtb: is SBP_THUMBBTNHORZ");
        else if (iPartId == SBP_THUMBBTNVERT)
            Wh_Log(L"dtb: is SBP_THUMBBTNVERT");

        Wh_Log(L"iStateId: %d", iStateId);

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
            Wh_Log(L"is dimensions %dx%d", pRect->right - pRect->left, pRect->bottom - pRect->top);

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
        (void *)OpenThemeData,
        (void *)OpenThemeData_hook,
        (void **)&OpenThemeData_orig
    );

    Wh_SetFunctionHook(
        (void *)CloseThemeData,
        (void *)CloseThemeData_hook,
        (void **)&CloseThemeData_orig
    );

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
