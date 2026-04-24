// ==WindhawkMod==
// @id              custom-desktop-watermark
// @name            Custom Desktop Watermark
// @description     Lets you set your own desktop watermark text
// @version         1.2.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -DWINVER=0x0A00
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Desktop Watermark
This mod allows you to completely customize the desktop watermark,
putting in your own lines of text.

# NOTICE
Due to a limitation with the Windhawk settings API, an empty line will stop
the mod from reading lines. If you want a blank line, use a space character.

## Examples

**Windows 7 (not genuine)**:

![Windows 7 (not genuine)](https://raw.githubusercontent.com/aubymori/images/main/custom-desktop-watermark-win7.png)

**Windows XP Professional x64 Edition**:

![Windows XP Professional x64 Edition](https://raw.githubusercontent.com/aubymori/images/main/custom-desktop-watermark-winxp.png)

**Windows Technical Preview**:

![Windows Technical Preview](https://raw.githubusercontent.com/aubymori/images/main/custom-desktop-watermark-wintp.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lines:
  - - text: ""
      $name: Text
    - title: false
      $name: Use caption font
      $description: Use caption font instead of message font when classic fonts are enabled.
    - bold: false
      $name: Bold text
      $description: Force this line to use bold text always.
    - align: right
      $name: Align
      $description: The edge to align the line of text to.
      $options:
      - left: Left
      - center: Center
      - right: Right
  $name: Text lines
- classic: false
  $name: Use classic fonts
  $description: Use caption/message font instead of just caption font.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <vector>

#define RECTWIDTH(rc)   ((rc).right - (rc).left)
#define RECTHEIGHT(rc)  ((rc).bottom - (rc).top)

WINUSERAPI BOOL WINAPI SystemParametersInfoForDpi(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);

struct WatermarkLine
{
    std::wstring text;
    bool title;
    bool bold;
    int align;
};

std::vector<WatermarkLine> g_lines;
bool g_fClassic = false;

bool (*CDesktopWatermark_s_WantWatermark_orig)(void);
bool CDesktopWatermark_s_WantWatermark_hook(void)
{
    return true;
}

int CalculateTextWidth(HDC hdc, LPCWSTR pszText)
{
    RECT rcText = { 0 };
    DrawTextW(
        hdc, pszText, -1,
        &rcText, DT_CALCRECT | DT_SINGLELINE
    );
    return RECTWIDTH(rcText);
}

int PaintLine(
    HDC            hDC,
    LPCRECT        lprc,
    LPCWSTR        lpszText,
    HFONT          hFont,
    int            align,
    int            width,
    int            offset
)
{
    HFONT hfOld = (HFONT)SelectObject(hDC, hFont);
        
    RECT rcText = { 0 };
    int newOffset = DrawTextW(
        hDC, lpszText, -1,
        &rcText, DT_CALCRECT | DT_SINGLELINE
    );

    RECT rcPaint = { 0 };
    rcPaint.left = lprc->right - width;
    rcPaint.top = lprc->bottom - RECTHEIGHT(rcText);
    rcPaint.right = lprc->right;
    rcPaint.bottom = lprc->bottom;

    int padding = MulDiv(5, GetDeviceCaps(hDC, LOGPIXELSX), 96);
    OffsetRect(&rcPaint, -padding, -offset);

    DrawTextW(hDC, lpszText, -1, &rcPaint, DT_SINGLELINE | align);

    SelectObject(hDC, hfOld);
    return newOffset;    
}

void (*CDesktopWatermark_s_DesktopBuildPaint_orig)(HDC, LPCRECT, HFONT);
void CDesktopWatermark_s_DesktopBuildPaint_hook(
    HDC     hdc,
    LPCRECT lprc,
    HFONT   hFont
)
{
    COLORREF cr = SetTextColor(hdc, RGB(255, 255, 255));
    int bk = SetBkMode(hdc, TRANSPARENT);
    int offset = 0;

    NONCLIENTMETRICSW ncm = { sizeof(ncm) };
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    
    using SystemParametersInfoForDpi_t = decltype(&SystemParametersInfoForDpi);
    static SystemParametersInfoForDpi_t pfnSystemParametersInfoForDpi
        = (SystemParametersInfoForDpi_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SystemParametersInfoForDpi");
    if (pfnSystemParametersInfoForDpi)
    {
        pfnSystemParametersInfoForDpi(
            SPI_GETNONCLIENTMETRICS,
            sizeof(ncm),
            &ncm,
            0,
            dpi
        );
    }
    else
    {
        SystemParametersInfoW(
            SPI_GETNONCLIENTMETRICS,
            sizeof(ncm),
            &ncm,
            0
        );
    }
    
    HFONT hfontTitle = CreateFontIndirectW(&ncm.lfCaptionFont);
    HFONT hfontMessage = CreateFontIndirectW(&ncm.lfMessageFont);
    ncm.lfCaptionFont.lfWeight = FW_BOLD;
    ncm.lfMessageFont.lfWeight = FW_BOLD;
    HFONT hfontTitleBold = CreateFontIndirectW(&ncm.lfCaptionFont);
    HFONT hfontMessageBold = CreateFontIndirectW(&ncm.lfMessageFont);
    if (!hfontTitle || !hfontMessage || !hfontTitleBold || !hfontMessageBold)
        return;
    int padding = MulDiv(3, dpi, 96);
    offset += MulDiv(g_fClassic ? 4 : 1, dpi, 96);

    int maxWidth = 0;
    for (const WatermarkLine &line : g_lines)
    {
        bool fMessageFont = g_fClassic && !line.title;
        HFONT hfont = fMessageFont
            ? (line.bold ? hfontMessageBold : hfontMessage)
            : (line.bold ? hfontTitleBold : hfontTitle);

        HFONT hfOld = (HFONT)SelectObject(hdc, hfont);
        int width = CalculateTextWidth(hdc, line.text.c_str());
        SelectObject(hdc, hfOld);

        if (width > maxWidth)
            maxWidth = width;
    }

    for (size_t i = g_lines.size(); i--;)
    {
        const WatermarkLine &line = g_lines.at(i);
        bool fMessageFont = g_fClassic && !line.title;
        HFONT hfont = fMessageFont
            ? (line.bold ? hfontMessageBold : hfontMessage)
            : (line.bold ? hfontTitleBold : hfontTitle);

        offset += PaintLine(
            hdc, lprc, line.text.c_str(),
            hfont,
            line.align,
            maxWidth,
            offset
        ) + padding;
    }

    DeleteObject(hfontTitle);
    DeleteObject(hfontMessage);
    DeleteObject(hfontTitleBold);
    DeleteObject(hfontMessageBold);
    SetBkMode(hdc, bk);
    SetTextColor(hdc, cr);
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"public: static bool __cdecl CDesktopWatermark::s_WantWatermark(void)",
            L"private: static bool __cdecl CDesktopWatermark::s_IsDrawVersionAlwaysSet(void)"
        },
        &CDesktopWatermark_s_WantWatermark_orig,
        CDesktopWatermark_s_WantWatermark_hook,
        false
    },
    {
        {
            L"private: static void __cdecl CDesktopWatermark::s_DesktopBuildPaint(struct HDC__ *,struct tagRECT const *,struct HFONT__ *)",
            L"private: static void __cdecl CDesktopWatermark::s_DesktopBuildPaint(struct HDC__ *,struct tagRECT const *,struct HFONT__ *,bool)"
        },
        &CDesktopWatermark_s_DesktopBuildPaint_orig,
        CDesktopWatermark_s_DesktopBuildPaint_hook,
        false
    }
};

void LoadSettings(void)
{
    g_lines.clear();
    g_fClassic = Wh_GetIntSetting(L"classic");

    for (int i = 0;; i++)
    {
        LPCWSTR pszText = Wh_GetStringSetting(L"lines[%i].text", i);
        if (!*pszText)
        {
            Wh_FreeStringSetting(pszText);
            break;
        }
        bool title = Wh_GetIntSetting(L"lines[%i].title", i);
        bool bold = Wh_GetIntSetting(L"lines[%i].bold", i);
        int align = DT_RIGHT;
        LPCWSTR pszAlign = Wh_GetStringSetting(L"lines[%i].align", i);
        if (0 == wcscmp(pszAlign, L"left"))
        {
            align = DT_LEFT;
        }
        else if (0 == wcscmp(pszAlign, L"center"))
        {
            align = DT_CENTER;
        }
        Wh_FreeStringSetting(pszAlign);
        WatermarkLine line = { pszText, title, bold, align };
        g_lines.push_back(line);
        Wh_FreeStringSetting(pszText);
    }
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hmodShell = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hmodShell)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hmodShell,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    return TRUE;
}

#define FCIDM_REFRESH  0xA220

void RefreshDesktop(void)
{
    HWND hwndProgman = FindWindowW(L"Progman", nullptr);
    if (hwndProgman)
    {
        SendMessageW(hwndProgman, WM_COMMAND, FCIDM_REFRESH, 0);
    }
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
    RefreshDesktop();
}

void Wh_ModAfterInit(void)
{
    RefreshDesktop();
}

void Wh_ModUninit(void)
{
    RefreshDesktop();
}