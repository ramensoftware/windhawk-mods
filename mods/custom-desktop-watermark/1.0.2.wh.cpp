// ==WindhawkMod==
// @id              custom-desktop-watermark
// @name            Custom Desktop Watermark
// @description     Lets you set your own desktop watermark text
// @version         1.0.2
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

## Examples

**Windows 7 (not genuine)**:
![Windows 7 (not genuine)](https://raw.githubusercontent.com/aubymori/images/main/custom-desktop-watermark-win7.png)

**Windows XP Professional x64 Edition**:
![Windows XP Professional x64 Edition](https://raw.githubusercontent.com/aubymori/images/main/custom-desktop-watermark-winxp.png)
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
};

std::vector<WatermarkLine> g_lines;
bool g_bClassic = false;

bool (*CDesktopWatermark_s_WantWatermark_orig)(void);
bool CDesktopWatermark_s_WantWatermark_hook(void)
{
    return true;
}

int PaintLine(
    HDC            hDC,
    LPCRECT        lprc,
    LPCWSTR        lpszText,
    HFONT          hFont,
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
    rcPaint.left = lprc->right - RECTWIDTH(rcText);
    rcPaint.top = lprc->bottom - RECTHEIGHT(rcText);
    rcPaint.right = lprc->right;
    rcPaint.bottom = lprc->bottom;

    int padding = MulDiv(5, GetDeviceCaps(hDC, LOGPIXELSX), 96);
    OffsetRect(&rcPaint, -padding, -offset);

    DrawTextW(hDC, lpszText, -1, &rcPaint, DT_SINGLELINE);

    SelectObject(hDC, hfOld);
    return newOffset;    
}

void (*CDesktopWatermark_s_DesktopBuildPaint_orig)(HDC, LPCRECT, HFONT);
void CDesktopWatermark_s_DesktopBuildPaint_hook(
    HDC     hDC,
    LPCRECT lprc,
    HFONT   hFont
)
{
    COLORREF cr = SetTextColor(hDC, RGB(255, 255, 255));
    int bk = SetBkMode(hDC, TRANSPARENT);
    int offset = 0;

    NONCLIENTMETRICSW ncm = { sizeof(ncm) };
    int dpi = GetDeviceCaps(hDC, LOGPIXELSX);
    SystemParametersInfoForDpi(
        SPI_GETNONCLIENTMETRICS,
        sizeof(ncm),
        &ncm,
        0,
        dpi
    );
    HFONT hTitleFont = CreateFontIndirectW(&ncm.lfCaptionFont);
    HFONT hMessageFont = CreateFontIndirectW(&ncm.lfMessageFont);
    if (!hTitleFont || !hMessageFont)
        return;
    int padding = MulDiv(3, dpi, 96);
    offset += MulDiv(g_bClassic ? 4 : 1, dpi, 96);

    for (size_t i = g_lines.size(); i--;)
    {
        WatermarkLine line = g_lines.at(i);
        bool bMessageFont = g_bClassic && !line.title;

        offset += PaintLine(
            hDC, lprc, line.text.c_str(),
            bMessageFont ? hMessageFont : hTitleFont,
            offset
        ) + padding;
    }

    DeleteObject(hTitleFont);
    DeleteObject(hMessageFont);
    SetBkMode(hDC, bk);
    SetTextColor(hDC, cr);
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"public: static bool __cdecl CDesktopWatermark::s_WantWatermark(void)"
        },
        &CDesktopWatermark_s_WantWatermark_orig,
        CDesktopWatermark_s_WantWatermark_hook,
        false
    },
    {
        {
            L"private: static void __cdecl CDesktopWatermark::s_DesktopBuildPaint(struct HDC__ *,struct tagRECT const *,struct HFONT__ *)"
        },
        &CDesktopWatermark_s_DesktopBuildPaint_orig,
        CDesktopWatermark_s_DesktopBuildPaint_hook,
        false
    }
};

void LoadSettings(void)
{
    g_lines.clear();
    g_bClassic = Wh_GetIntSetting(L"classic");

    for (int i = 0;; i++)
    {
        LPCWSTR lpszText = Wh_GetStringSetting(L"lines[%i].text", i);
        if (!*lpszText)
        {
            Wh_FreeStringSetting(lpszText);
            break;
        }
        bool title = Wh_GetIntSetting(L"lines[%i].title", i);
        WatermarkLine line = { lpszText, title };
        g_lines.push_back(line);
        Wh_FreeStringSetting(lpszText);
    }
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
