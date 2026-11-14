// ==WindhawkMod==
// @id              taskbar-font-scale
// @name            Taskbar Font Scale
// @description     Bigger (and optionally bold) text on the classic Windows 10 style taskbar, e.g. the one ExplorerPatcher brings to Windows 11, without changing display scaling
// @version         0.4
// @author          Claude + Claude
// @github          https://github.com/tatarikt-source
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Font Scale

Makes the text on the classic (Windows 10 style) taskbar bigger, and optionally
bold, **without changing the display scaling**. It was written for the old
taskbar that [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher)
brings to Windows 11, which ignores the Windows "Text size" accessibility
setting (see ExplorerPatcher issue #4151). It affects the clock, the button
labels and the tray text. Drag and drop of files onto taskbar icons keeps
working, because ExplorerPatcher itself is not touched.

## How it works

The mod hooks `CreateFontIndirectW` and `CreateFontIndirectExW` in
`explorer.exe` and, only on the thread that owns the taskbar window, scales the
font height (and optionally makes the font bold or switches the font family).
Nothing else in Explorer is changed.

## Notes

* Fonts are created when the taskbar starts, so after enabling the mod or
  changing its settings, **restart Explorer once** (Task Manager > Windows
  Explorer > Restart).
* Large scales can clip the clock or truncate button labels. Lower the scale
  or raise the taskbar height in ExplorerPatcher.
* Tested on Windows 11 build 26300 with the ExplorerPatcher Windows 10
  taskbar at 100% display scaling, using 125%, 135% and 150%. Other setups
  are untested.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scalePercent: 150
  $name: Font scale (%)
  $description: 100 = default size, 150 = one and a half times bigger. Allowed 50-300.
- bold: true
  $name: Bold text
  $description: Make the taskbar text bold.
- fontFace: ''
  $name: Font name (optional)
  $description: Exact name of an installed font, for example Segoe UI, Consolas, Arial or Tahoma. Leave empty to keep the default taskbar font.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <wchar.h>

static int g_scalePercent = 150;
static bool g_bold = true;
static WCHAR g_fontFace[LF_FACESIZE] = L"";

static DWORD GetTrayThreadId() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) {
        return 0;
    }
    return GetWindowThreadProcessId(tray, nullptr);
}

// Cached id of the thread that owns the primary taskbar window.
static volatile DWORD g_trayThreadId = 0;

static bool IsTrayThread() {
    DWORD current = GetCurrentThreadId();
    DWORD cached = g_trayThreadId;
    if (cached == 0) {
        cached = GetTrayThreadId();
        if (cached != 0) {
            g_trayThreadId = cached;
        }
    }
    return cached != 0 && cached == current;
}

static LONG ScaleHeight(LONG height) {
    if (height == 0 || g_scalePercent == 100) {
        return height;
    }
    LONG scaled = MulDiv(height, g_scalePercent, 100);
    if (scaled == 0) {
        scaled = height > 0 ? 1 : -1;
    }
    return scaled;
}

using CreateFontIndirectW_t = HFONT(WINAPI*)(const LOGFONTW*);
static CreateFontIndirectW_t CreateFontIndirectW_Original;

HFONT WINAPI CreateFontIndirectW_Hook(const LOGFONTW* lplf) {
    if (lplf && IsTrayThread()) {
        LOGFONTW copy = *lplf;
        copy.lfHeight = ScaleHeight(copy.lfHeight);
        if (g_bold) {
            copy.lfWeight = FW_BOLD;
        }
        if (g_fontFace[0]) {
            wcscpy_s(copy.lfFaceName, g_fontFace);
        }
        copy.lfWidth = 0;
        return CreateFontIndirectW_Original(&copy);
    }
    return CreateFontIndirectW_Original(lplf);
}

using CreateFontIndirectExW_t = HFONT(WINAPI*)(const ENUMLOGFONTEXDVW*);
static CreateFontIndirectExW_t CreateFontIndirectExW_Original;

HFONT WINAPI CreateFontIndirectExW_Hook(const ENUMLOGFONTEXDVW* lpelfe) {
    if (lpelfe && IsTrayThread()) {
        ENUMLOGFONTEXDVW copy = *lpelfe;
        copy.elfEnumLogfontEx.elfLogFont.lfHeight =
            ScaleHeight(copy.elfEnumLogfontEx.elfLogFont.lfHeight);
        copy.elfEnumLogfontEx.elfLogFont.lfWidth = 0;
        if (g_bold) {
            copy.elfEnumLogfontEx.elfLogFont.lfWeight = FW_BOLD;
        }
        if (g_fontFace[0]) {
            wcscpy_s(copy.elfEnumLogfontEx.elfLogFont.lfFaceName, g_fontFace);
        }
        return CreateFontIndirectExW_Original(&copy);
    }
    return CreateFontIndirectExW_Original(lpelfe);
}

static BOOL CALLBACK NotifyChildProc(HWND hwnd, LPARAM) {
    DWORD_PTR result;
    SendMessageTimeoutW(hwnd, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 0,
                        SMTO_ABORTIFHUNG, 200, &result);
    SendMessageTimeoutW(hwnd, WM_SETTINGCHANGE, SPI_SETICONMETRICS, 0,
                        SMTO_ABORTIFHUNG, 200, &result);
    return TRUE;
}

// Ask the taskbar and its children to re-read system fonts.
static void RefreshTaskbarFonts() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        NotifyChildProc(tray, 0);
        EnumChildWindows(tray, NotifyChildProc, 0);
    }
    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary,
                                      L"Shell_SecondaryTrayWnd", nullptr)) !=
           nullptr) {
        NotifyChildProc(secondary, 0);
        EnumChildWindows(secondary, NotifyChildProc, 0);
    }
}

static void LoadSettings() {
    int value = Wh_GetIntSetting(L"scalePercent");
    if (value < 50) {
        value = 50;
    }
    if (value > 300) {
        value = 300;
    }
    g_scalePercent = value;
    g_bold = Wh_GetIntSetting(L"bold") != 0;
    PCWSTR face = Wh_GetStringSetting(L"fontFace");
    if (face) {
        wcsncpy_s(g_fontFace, face, _TRUNCATE);
        Wh_FreeStringSetting(face);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    Wh_SetFunctionHook((void*)CreateFontIndirectW,
                       (void*)CreateFontIndirectW_Hook,
                       (void**)&CreateFontIndirectW_Original);
    Wh_SetFunctionHook((void*)CreateFontIndirectExW,
                       (void*)CreateFontIndirectExW_Hook,
                       (void**)&CreateFontIndirectExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit, scale %d%%", g_scalePercent);
    RefreshTaskbarFonts();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings changed, scale %d%%", g_scalePercent);
    RefreshTaskbarFonts();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
