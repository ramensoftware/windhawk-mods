// ==WindhawkMod==
// @id              hide-file-explorer-focus-border
// @name            Hide File Explorer Focus Border
// @description     Hides the white focus border around focused items in File Explorer.
// @version         1.0
// @author          YuUNaO
// @github          https://github.com/YuUNaO7122
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide File Explorer Focus Border

Hides the white focus border around focused items in File Explorer.

Tested environment:
- Windows 11 25H2, OS Build 26200.8328
- Windhawk 1.7.3
- Theme: dark mode

Notes:
- This mod targets the modern File Explorer item focus border.
- It does not target the classic dotted focus rectangle on the desktop.
- The behavior may vary depending on Windows version, display scaling, and File Explorer view mode.
- If the focus border is still visible, adjust MinWidth/MaxWidth/MinHeight/MaxHeight in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- MinWidth: 20
  $name: Minimum width
  $description: Minimum rectangle width to suppress.

- MaxWidth: 3000
  $name: Maximum width
  $description: Maximum rectangle width to suppress.

- MinHeight: 20
  $name: Minimum height
  $description: Minimum rectangle height to suppress.

- MaxHeight: 3000
  $name: Maximum height
  $description: Maximum rectangle height to suppress.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>

using DrawThemeBackgroundEx_t = HRESULT(WINAPI*)(
    HTHEME,
    HDC,
    int,
    int,
    const RECT*,
    const DTBGOPTS*
);

static DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Orig = nullptr;

static int g_minWidth = 20;
static int g_maxWidth = 3000;
static int g_minHeight = 20;
static int g_maxHeight = 3000;

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static void LoadSettings() {
    g_minWidth = Wh_GetIntSetting(L"MinWidth");
    g_maxWidth = Wh_GetIntSetting(L"MaxWidth");
    g_minHeight = Wh_GetIntSetting(L"MinHeight");
    g_maxHeight = Wh_GetIntSetting(L"MaxHeight");

    if (g_minWidth < 0) {
        g_minWidth = 20;
    }

    if (g_maxWidth <= 0) {
        g_maxWidth = 3000;
    }

    if (g_minHeight < 0) {
        g_minHeight = 20;
    }

    if (g_maxHeight <= 0) {
        g_maxHeight = 3000;
    }
}

static bool RectMatches(const RECT* rc) {
    if (!rc) {
        return false;
    }

    const int width = AbsInt(rc->right - rc->left);
    const int height = AbsInt(rc->bottom - rc->top);

    return width >= g_minWidth &&
           width <= g_maxWidth &&
           height >= g_minHeight &&
           height <= g_maxHeight;
}

static HRESULT WINAPI DrawThemeBackgroundEx_Hook(
    HTHEME hTheme,
    HDC hdc,
    int partId,
    int stateId,
    const RECT* rc,
    const DTBGOPTS* options
) {
    // File Explorer item focus border.
    if (partId == 3 && stateId == 1 && RectMatches(rc)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Orig(
        hTheme,
        hdc,
        partId,
        stateId,
        rc,
        options
    );
}

BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    if (!uxtheme) {
        uxtheme = LoadLibraryW(L"uxtheme.dll");
    }

    if (!uxtheme) {
        return FALSE;
    }

    void* target = reinterpret_cast<void*>(
        GetProcAddress(uxtheme, "DrawThemeBackgroundEx")
    );

    if (!target) {
        return FALSE;
    }

    return Wh_SetFunctionHook(
        target,
        reinterpret_cast<void*>(DrawThemeBackgroundEx_Hook),
        reinterpret_cast<void**>(&DrawThemeBackgroundEx_Orig)
    );
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
