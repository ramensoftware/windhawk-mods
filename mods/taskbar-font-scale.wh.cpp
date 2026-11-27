// ==WindhawkMod==
// @id              taskbar-font-scale
// @name            Taskbar Font Scale
// @description     Bigger (and optionally bold) text on the classic Windows 10 style taskbar, e.g. the one ExplorerPatcher brings to Windows 11, without changing display scaling
// @version         0.5
// @author          tatarikt-source
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

Written with the help of Claude (AI) and tested by hand on a real machine.

![Before and after: default taskbar text and 150% bold](https://raw.githubusercontent.com/tatarikt-source/windhawk-mods/main/screenshots/taskbar-font-scale-before-after.png)

## How it works

The mod hooks `CreateFontIndirectW` and `CreateFontIndirectExW` in
`explorer.exe` and, only on the thread that owns the taskbar window of the
shell process, scales the font height and optionally makes the font bold or
switches the font family. A per-thread guard makes sure a font is scaled only
once even if one of these functions calls the other. Nothing else in Explorer
is changed.

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

// Returns the id of the thread that owns the taskbar window, but only if that
// window belongs to the current process. Other explorer.exe processes (folder
// windows, "show in folder" launches) must not touch the shell's taskbar.
static DWORD GetTrayThreadId() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) {
        return 0;
    }
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(tray, &pid);
    return pid == GetCurrentProcessId() ? tid : 0;
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

// Set while CreateFontIndirectW_Hook runs, so that a nested call into
// CreateFontIndirectExW (if the system does that internally) does not scale
// the same font a second time.
static thread_local bool g_inCreateFontIndirectW = false;

using CreateFontIndirectW_t = HFONT(WINAPI*)(const LOGFONTW*);
static CreateFontIndirectW_t CreateFontIndirectW_Original;

HFONT WINAPI CreateFontIndirectW_Hook(const LOGFONTW* lplf) {
    if (lplf && !g_inCreateFontIndirectW && IsTrayThread()) {
        LOGFONTW copy = *lplf;
        copy.lfHeight = ScaleHeight(copy.lfHeight);
        copy.lfWidth = 0;
        if (g_bold) {
            copy.lfWeight = FW_BOLD;
        }
        if (g_fontFace[0]) {
            wcscpy_s(copy.lfFaceName, g_fontFace);
        }
        g_inCreateFontIndirectW = true;
        HFONT result = CreateFontIndirectW_Original(&copy);
        g_inCreateFontIndirectW = false;
        return result;
    }
    return CreateFontIndirectW_Original(lplf);
}

using CreateFontIndirectExW_t = HFONT(WINAPI*)(const ENUMLOGFONTEXDVW*);
static CreateFontIndirectExW_t CreateFontIndirectExW_Original;

HFONT WINAPI CreateFontIndirectExW_Hook(const ENUMLOGFONTEXDVW* lpelfe) {
    if (lpelfe && !g_inCreateFontIndirectW && IsTrayThread()) {
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

// Ask the taskbar and its children to re-read system fonts, so that the new
// (scaled) fonts get created through our hook without restarting Explorer.
static void RefreshTaskbarFonts() {
    if (!GetTrayThreadId()) {
        return;  // Not the shell process, nothing to refresh.
    }
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
