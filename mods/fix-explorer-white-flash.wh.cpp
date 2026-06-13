// ==WindhawkMod==
// @id              fix-explorer-white-flash
// @name            Fix white flashes in explorer
// @description     Removes the annoying white flash when opening Explorer tabs.
// @version         1.4
// @author          Mgg Sk
// @github          https://github.com/MGGSK
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix white flashes in explorer
Removes the annoying white flash when opening new tabs in File Explorer.
This mod blocks intermediate white frames that appear during Explorer tab creation and redraw, making tab opening look cleaner and less distracting.

### Before
![Before](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/fix-explorer-white-flash-before.png)
### After
![After](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/fix-explorer-white-flash-after.png)

## Notes
This mod is focused specifically on the white flash problem. It can also slightly reduce some of the visual jumping during tab creation, but its main purpose is removing the white flash itself.

#### Credits: [mode0192](https://github.com/mode0192), [Languster](https://github.com/Languster)

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>
#include <algorithm>

static HMODULE g_hUxTheme = nullptr;

using ShouldAppsUseDarkMode_T = bool(WINAPI*)();
static ShouldAppsUseDarkMode_T ShouldAppsUseDarkMode = nullptr;

decltype(&FillRect) FillRect_Original = nullptr;
decltype(&BitBlt) BitBlt_Original = nullptr;

struct RecentWhiteFill
{
    HDC hdc;
    int w;
    int h;
    DWORD tick;
};

struct FollowupBlock
{
    HWND hWndDest;
    HDC hdcSrc;
    int cx;
    int cy;
    DWORD tick;
    int blocksLeft;
    bool armed;
};

thread_local static RecentWhiteFill g_recent[32];
thread_local static FollowupBlock g_followup[8];

bool IsDarkModeActive()
{
    return ShouldAppsUseDarkMode && ShouldAppsUseDarkMode();
}

int AbsI(int v)
{
    return v < 0 ? -v : v;
}

bool IsWindowClass(HWND hWnd, PCWSTR clsName)
{
    if (!hWnd || !clsName || !*clsName)
        return false;

    wchar_t cls[128] = {};
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, clsName) == 0;
}

bool IsWhiteishColor(COLORREF c)
{
    return GetRValue(c) >= 230 &&
           GetGValue(c) >= 230 &&
           GetBValue(c) >= 230;
}

bool IsWhiteishBrush(HBRUSH hBrush)
{
    if (!hBrush)
        return false;

    LOGBRUSH logBrush = {};
    if (GetObjectW(hBrush, sizeof(logBrush), &logBrush) != sizeof(logBrush))
        return false;

    if (logBrush.lbStyle != BS_SOLID)
        return false;

    return IsWhiteishColor(logBrush.lbColor);
}

bool TextContainsI(const wchar_t* text, const wchar_t* needle)
{
    if (!text || !needle || !*needle)
        return false;

    wchar_t textLower[1024] = {};
    wchar_t needleLower[256] = {};

    lstrcpynW(textLower, text, ARRAYSIZE(textLower));
    lstrcpynW(needleLower, needle, ARRAYSIZE(needleLower));

    CharLowerBuffW(textLower, lstrlenW(textLower));
    CharLowerBuffW(needleLower, lstrlenW(needleLower));

    return wcsstr(textLower, needleLower) != nullptr;
}

bool IsControlPanelFactoryProcess()
{
    PCWSTR cmd = GetCommandLineW();
    if (!cmd || !*cmd)
        return false;

    if (TextContainsI(cmd, L"/factory,{5bd95610-9434-43c2-886c-57852cc8a120}"))
        return true;

    return false;
}

bool IsRegularExplorerRootWindow(HWND hWndAny)
{
    if (!hWndAny)
        return false;

    HWND hRoot = GetAncestor(hWndAny, GA_ROOT);
    if (!hRoot)
        hRoot = hWndAny;

    if (IsWindowClass(hRoot, L"CabinetWClass"))
        return true;

    if (IsWindowClass(hRoot, L"ExploreWClass"))
        return true;

    return false;
}

bool ShouldHandleBitBltForWindow(HWND hWndDest)
{
    if (!hWndDest)
        return false;

    if (!IsWindowClass(hWndDest, L"DirectUIHWND"))
        return false;

    if (!IsRegularExplorerRootWindow(hWndDest))
        return false;

    return true;
}

bool IsCandidateRectSize(int w, int h)
{
    if (w < 80 || h < 50)
        return false;

    return (w * h) >= 6000;
}

bool IsNearWholeDirectUISurface(HWND hWndDest, int x, int y, int cx, int cy)
{
    RECT rcClient;
    if (!GetClientRect(hWndDest, &rcClient))
        return false;

    int clientW = rcClient.right - rcClient.left;
    int clientH = rcClient.bottom - rcClient.top;

    if (clientW <= 0 || clientH <= 0)
        return false;

    if (AbsI(x) > 8 || AbsI(y) > 8)
        return false;

    int widthSlack  = std::max(24, clientW / 8);
    int heightSlack = std::max(24, clientH / 8);

    if (cx < clientW - widthSlack)
        return false;

    if (cy < clientH - heightSlack)
        return false;

    long blitArea   = 1LL * cx * cy;
    long clientArea = 1LL * clientW * clientH;

    if (blitArea * 100 < clientArea * 70)
        return false;

    return true;
}

void RememberWhiteFill(HDC hdc, int w, int h)
{
    DWORD now = GetTickCount();

    int best = 0;
    DWORD oldest = g_recent[0].tick;

    for (UINT i = 0; i < ARRAYSIZE(g_recent); i++)
    {
        if (g_recent[i].hdc == hdc)
        {
            g_recent[i].w = w;
            g_recent[i].h = h;
            g_recent[i].tick = now;
            return;
        }

        if (g_recent[i].tick < oldest)
        {
            oldest = g_recent[i].tick;
            best = i;
        }
    }

    g_recent[best].hdc = hdc;
    g_recent[best].w = w;
    g_recent[best].h = h;
    g_recent[best].tick = now;
}

int GetMatchTolerance(int size)
{
    if (size < 180)
        return 24;

    if (size < 300)
        return 18;

    return 12;
}

bool MatchRecentWhiteFill(HDC hdc, int w, int h)
{
    DWORD now = GetTickCount();

    for (UINT i = 0; i < ARRAYSIZE(g_recent); i++)
    {
        if (g_recent[i].hdc != hdc)
            continue;

        DWORD age = now - g_recent[i].tick;
        if (age > 650)
            continue;

        int dw = AbsI(g_recent[i].w - w);
        int dh = AbsI(g_recent[i].h - h);

        int tolW = GetMatchTolerance(w);
        int tolH = GetMatchTolerance(h);

        if (dw <= tolW && dh <= tolH)
            return true;
    }

    return false;
}

void ForgetRecentWhiteFill(HDC hdc)
{
    for (UINT i = 0; i < ARRAYSIZE(g_recent); i++)
    {
        if (g_recent[i].hdc == hdc)
        {
            g_recent[i].hdc = nullptr;
            g_recent[i].w = 0;
            g_recent[i].h = 0;
            g_recent[i].tick = 0;
        }
    }
}

int GetFollowupTolerance(int size)
{
    if (size < 180)
        return 28;

    if (size < 300)
        return 20;

    return 14;
}

void ArmFollowupBlock(HWND hWndDest, HDC hdcSrc, int cx, int cy)
{
    DWORD now = GetTickCount();

    int best = 0;
    DWORD oldest = g_followup[0].tick;

    for (UINT i = 0; i < ARRAYSIZE(g_followup); i++)
    {
        if (g_followup[i].armed &&
            g_followup[i].hWndDest == hWndDest)
        {
            g_followup[i].hdcSrc = hdcSrc;
            g_followup[i].cx = cx;
            g_followup[i].cy = cy;
            g_followup[i].tick = now;
            g_followup[i].blocksLeft = 2;
            return;
        }

        if (!g_followup[i].armed)
        {
            best = i;
            break;
        }

        if (g_followup[i].tick < oldest)
        {
            oldest = g_followup[i].tick;
            best = i;
        }
    }

    g_followup[best].hWndDest = hWndDest;
    g_followup[best].hdcSrc = hdcSrc;
    g_followup[best].cx = cx;
    g_followup[best].cy = cy;
    g_followup[best].tick = now;
    g_followup[best].blocksLeft = 2;
    g_followup[best].armed = true;
}

bool ShouldBlockFollowupBitBlt(HWND hWndDest, HDC hdcSrc, int cx, int cy)
{
    DWORD now = GetTickCount();

    for (UINT i = 0; i < ARRAYSIZE(g_followup); i++)
    {
        if (!g_followup[i].armed)
            continue;

        DWORD age = now - g_followup[i].tick;
        if (age > 120 || g_followup[i].blocksLeft <= 0)
        {
            g_followup[i].armed = false;
            continue;
        }

        if (g_followup[i].hWndDest != hWndDest)
            continue;

        int tolW = GetFollowupTolerance(cx);
        int tolH = GetFollowupTolerance(cy);

        if (AbsI(g_followup[i].cx - cx) > tolW ||
            AbsI(g_followup[i].cy - cy) > tolH)
            continue;

        bool sameSrc = (g_followup[i].hdcSrc == hdcSrc);

        if (!sameSrc && age > 45)
            continue;

        g_followup[i].hdcSrc = hdcSrc;
        g_followup[i].cx = cx;
        g_followup[i].cy = cy;
        g_followup[i].tick = now;
        g_followup[i].blocksLeft--;

        if (g_followup[i].blocksLeft <= 0)
            g_followup[i].armed = false;

        return true;
    }

    return false;
}

int WINAPI FillRect_Hook(HDC hdc, const RECT* lprc, HBRUSH hbr)
{
    if (IsDarkModeActive() && lprc && hbr)
    {
        int w = lprc->right - lprc->left;
        int h = lprc->bottom - lprc->top;

        if (IsCandidateRectSize(w, h))
        {
            HWND hWnd = WindowFromDC(hdc);

            if (!hWnd && IsWhiteishBrush(hbr))
                RememberWhiteFill(hdc, w, h);
        }
    }

    return FillRect_Original(hdc, lprc, hbr);
}

BOOL WINAPI BitBlt_Hook(
    HDC hdcDest,
    int x,
    int y,
    int cx,
    int cy,
    HDC hdcSrc,
    int x1,
    int y1,
    DWORD rop)
{
    if (IsDarkModeActive() &&
        rop == SRCCOPY &&
        IsCandidateRectSize(cx, cy))
    {
        HWND hWndDest = WindowFromDC(hdcDest);

        if (ShouldHandleBitBltForWindow(hWndDest) &&
            IsNearWholeDirectUISurface(hWndDest, x, y, cx, cy))
        {
            if (ShouldBlockFollowupBitBlt(hWndDest, hdcSrc, cx, cy))
                return TRUE;

            if (MatchRecentWhiteFill(hdcSrc, cx, cy))
            {
                ForgetRecentWhiteFill(hdcSrc);
                ArmFollowupBlock(hWndDest, hdcSrc, cx, cy);
                return TRUE;
            }
        }
    }

    return BitBlt_Original(hdcDest, x, y, cx, cy, hdcSrc, x1, y1, rop);
}

BOOL Wh_ModInit()
{
    if (IsControlPanelFactoryProcess())
        return FALSE;

    g_hUxTheme = LoadLibraryExW(L"UxTheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hUxTheme)
    {
        Wh_Log(L"Failed to load UxTheme.dll!");
        return FALSE;
    }

    ShouldAppsUseDarkMode = (ShouldAppsUseDarkMode_T)GetProcAddress(g_hUxTheme, MAKEINTRESOURCEA(132));

    if (!WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &FillRect_Original))
    {
        Wh_Log(L"Failed to hook FillRect!");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(BitBlt, BitBlt_Hook, &BitBlt_Original))
    {
        Wh_Log(L"Failed to hook BitBlt!");
        return FALSE;
    }

    ZeroMemory(g_recent, sizeof(g_recent));
    ZeroMemory(g_followup, sizeof(g_followup));

    return TRUE;
}

void Wh_ModUninit()
{
    if (g_hUxTheme)
    {
        FreeLibrary(g_hUxTheme);
        g_hUxTheme = nullptr;
    }
}
