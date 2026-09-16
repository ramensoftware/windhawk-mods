// ==WindhawkMod==
// @id           explorer-hide-focus-border
// @name         File Explorer Focus Border Remover
// @name:vi-VN   Xóa viền focus trong File Explorer
// @description  Removes the white focus border around selected items in File Explorer while preserving the normal selection highlight.
// @description:vi-VN Xóa viền focus màu trắng quanh mục đang chọn trong File Explorer nhưng vẫn giữ nền chọn bình thường.
// @version      1.0.0
// @author       Hùng
// @github       https://github.com/NoMorePlz
// @include      explorer.exe
// @architecture x86-64
// @license      MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Focus Border Remover

Removes the white rectangular focus border around selected files, folders and
drives in Windows 11 File Explorer while keeping the normal selection highlight
intact.

This is different from [No Focus Rectangle](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/no-focus-rectangle.wh.cpp):
that mod suppresses the classic dotted `DrawFocusRect`, while this mod targets
the themed focus border used by the modern File Explorer item view.

## Before / After

![Before and after](https://raw.githubusercontent.com/NoMorePlz/windhawk-mods/main/assets/explorer-hide-focus-border-before-after.svg)

## Notes

- Designed for Windows 11 File Explorer.
- The effect is applied at draw time, so it takes effect without restarting
  Explorer when the mod is enabled.
- The implementation relies on an undocumented File Explorer theme detail:
  `ItemsView`, part 3, states 1 and 2 are used for the focus border on current
  Windows builds. A future Windows update may change this.
- Tested with regular File Explorer items and **This PC > Devices and drives**.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <uxtheme.h>
#include <windhawk_utils.h>

#include <string>

static decltype(&DrawThemeBackground) DrawThemeBackground_Original = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_Original = nullptr;

static std::wstring GetThemeClass(HTHEME hTheme) {
    using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);

    static auto getThemeClass = reinterpret_cast<GetThemeClass_t>(
        GetProcAddress(GetModuleHandleW(L"uxtheme.dll"), MAKEINTRESOURCEA(74)));

    if (!getThemeClass) {
        return L"";
    }

    WCHAR className[64] = {};
    return SUCCEEDED(getThemeClass(hTheme, className, ARRAYSIZE(className)))
               ? className
               : L"";
}

static bool IsFileExplorerWindow(HDC hdc) {
    HWND hwnd = WindowFromDC(hdc);
    if (!hwnd) {
        // ItemsView is specific enough inside explorer.exe, and some draw paths
        // don't expose a useful HWND through the DC.
        return true;
    }

    WCHAR className[64] = {};
    for (HWND current = hwnd; current;
         current = GetAncestor(current, GA_PARENT)) {
        if (GetClassNameW(current, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"CabinetWClass") == 0) {
            return true;
        }
    }

    return false;
}

static bool ShouldSuppressFocusBorder(HTHEME hTheme,
                                      HDC hdc,
                                      int partId,
                                      int stateId) {
    // Current File Explorer DirectUI ItemsView behavior:
    //   part 3, state 1 = normal focus border
    //   part 3, state 2 = hot focus border
    // The selection fill is painted separately, so suppressing this part keeps
    // the normal selected-item highlight intact.
    return partId == 3 && (stateId == 1 || stateId == 2) &&
           GetThemeClass(hTheme) == L"ItemsView" && IsFileExplorerWindow(hdc);
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme,
                                         HDC hdc,
                                         int iPartId,
                                         int iStateId,
                                         LPCRECT pRect,
                                         LPCRECT pClipRect) {
    if (ShouldSuppressFocusBorder(hTheme, hdc, iPartId, iStateId)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(hTheme, hdc, iPartId, iStateId,
                                        pRect, pClipRect);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME hTheme,
                                           HDC hdc,
                                           int iPartId,
                                           int iStateId,
                                           LPCRECT pRect,
                                           const DTBGOPTS* pOptions) {
    if (ShouldSuppressFocusBorder(hTheme, hdc, iPartId, iStateId)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId,
                                          pRect, pOptions);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing File Explorer Focus Border Remover");

    WindhawkUtils::SetFunctionHook(DrawThemeBackground,
                                   DrawThemeBackground_Hook,
                                   &DrawThemeBackground_Original);
    WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx,
                                   DrawThemeBackgroundEx_Hook,
                                   &DrawThemeBackgroundEx_Original);

    return TRUE;
}
