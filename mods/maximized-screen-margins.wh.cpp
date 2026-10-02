// ==WindhawkMod==
// @id              maximized-screen-margins
// @name            Screen Margin Gaps for Maximized Windows
// @description     Reserves invisible desktop margins using AppBar API so all maximized windows naturally leave gaps for rounded corners.
// @version         1.0
// @license MIT
// @author          furkan-o
// @github          https://github.com/furkan-o
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- gapSize: 12
  $name: Margin Gap Size
  $description: Gap in pixels around screen edges for maximized windows.
- topGap: true
  $name: Apply gap to Top edge
  $description: Leave gap at the top of the screen.
- bottomGap: false
  $name: Apply gap to Bottom edge
  $description: Enable if you want extra space above the taskbar. Keep false if taskbar already has enough margin.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Screen Margin Gaps for Maximized Windows

Adds customizable outer margin gaps around your screen so maximized windows leave space along the edges.

This is especially useful when using mods like **Custom Window Corner Radius** with rounded corners enabled for maximized windows. Since Windows normally stretches maximized windows to the exact screen boundaries, rounded corners can get clipped or touch the screen borders. This mod ensures there is always breathing room around maximized apps, keeping rounded corners fully visible and floating smoothly above your desktop.

## Features

- **Native Windows AppBar API:** Uses official Win32 application desktop toolbar APIs (`SHAppBarMessage`) instead of hooking or fighting with window sizing protocols.
- **Universal Window Support:** Works consistently across all apps (Chrome, VS Code, File Explorer, Terminal, Electron apps, etc.).
- **Click-Through & Invisible:** The reserved edge regions are completely transparent and allow mouse input to pass through freely.
- **Configurable Edges:** Customize the gap size (in pixels) and independently toggle margins for the top and bottom borders.
- **Clean Unload:** Disabling or modifying settings instantly restores your original desktop work area without requiring a restart of `explorer.exe`.

## Recommended Pairing

- **Custom Window Corner Radius:** Enable the option to round maximized/snapped windows to achieve a modern, floating-card window aesthetic.
- **Windows 11 Taskbar Styler:** If you use a floating or centered dock-style taskbar, set `bottomGap: false` so this mod preserves your existing taskbar spacing.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <shellapi.h>

struct {
    int gapSize;
    bool topGap;
    bool bottomGap;
} g_settings;

HWND g_hAppBars[4] = { nullptr, nullptr, nullptr, nullptr }; // Left, Top, Right, Bottom

void RemoveAppBars() {
    for (int i = 0; i < 4; i++) {
        if (g_hAppBars[i] && IsWindow(g_hAppBars[i])) {
            APPBARDATA abd = { sizeof(abd) };
            abd.hWnd = g_hAppBars[i];
            SHAppBarMessage(ABM_REMOVE, &abd);
            DestroyWindow(g_hAppBars[i]);
            g_hAppBars[i] = nullptr;
        }
    }
}

HWND CreateSingleAppBar(UINT edge, int gap, int screenW, int screenH) {
    // Tıklamaları geçiren ve odaklanmayan şeffaf pencere
    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        L"Static",
        L"WindhawkScreenMargin",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );

    if (!hWnd) return nullptr;

    // Tamamen görünmez yap
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);

    APPBARDATA abd = { sizeof(abd) };
    abd.hWnd = hWnd;
    abd.uCallbackMessage = WM_USER + 101;
    SHAppBarMessage(ABM_NEW, &abd);

    abd.uEdge = edge;

    if (edge == ABE_LEFT) {
        SetRect(&abd.rc, 0, 0, gap, screenH);
    } else if (edge == ABE_RIGHT) {
        SetRect(&abd.rc, screenW - gap, 0, screenW, screenH);
    } else if (edge == ABE_TOP) {
        SetRect(&abd.rc, 0, 0, screenW, gap);
    } else if (edge == ABE_BOTTOM) {
        SetRect(&abd.rc, 0, screenH - gap, screenW, screenH);
    }

    SHAppBarMessage(ABM_QUERYPOS, &abd);
    SHAppBarMessage(ABM_SETPOS, &abd);

    MoveWindow(hWnd, abd.rc.left, abd.rc.top, 
               abd.rc.right - abd.rc.left, abd.rc.bottom - abd.rc.top, TRUE);
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);

    return hWnd;
}

void ApplyMargins() {
    RemoveAppBars();

    if (g_settings.gapSize <= 0) return;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    // Sol ve Sağ kenar rezervasyonu
    g_hAppBars[0] = CreateSingleAppBar(ABE_LEFT, g_settings.gapSize, screenW, screenH);
    g_hAppBars[2] = CreateSingleAppBar(ABE_RIGHT, g_settings.gapSize, screenW, screenH);

    // İsteğe bağlı Üst kenar rezervasyonu
    if (g_settings.topGap) {
        g_hAppBars[1] = CreateSingleAppBar(ABE_TOP, g_settings.gapSize, screenW, screenH);
    }

    // İsteğe bağlı Alt kenar rezervasyonu
    if (g_settings.bottomGap) {
        g_hAppBars[3] = CreateSingleAppBar(ABE_BOTTOM, g_settings.gapSize, screenW, screenH);
    }
}

void LoadSettings() {
    g_settings.gapSize = Wh_GetIntSetting(L"gapSize");
    g_settings.topGap = Wh_GetIntSetting(L"topGap");
    g_settings.bottomGap = Wh_GetIntSetting(L"bottomGap");

    ApplyMargins();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init Screen Margins Mod");
    LoadSettings();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit Screen Margins Mod");
    RemoveAppBars();
}
