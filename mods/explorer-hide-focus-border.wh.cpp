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
// @compilerOptions -luxtheme
// @license      MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Focus Border Remover

Removes the white rectangular focus border around selected files, folders and
drives in Windows 11 File Explorer while keeping the normal selection and hover
highlights intact.

This is different from [No Focus Rectangle](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/no-focus-rectangle.wh.cpp):
that mod suppresses the classic dotted `DrawFocusRect`, while this mod targets
the themed focus border used by the modern File Explorer item view.

## Before / After

![Before and after](https://raw.githubusercontent.com/NoMorePlz/windhawk-mods/3e6e0b630dfe388217d4a2acf3e068b14ba456f0/assets/explorer-hide-focus-border-before-after.png)

## Notes

- Designed for Windows 11 File Explorer.
- The effect is applied at draw time, so it takes effect without restarting
  Explorer when the mod is enabled.
- The implementation relies on an undocumented File Explorer theme detail:
  `ItemsView`, part 3, state 1 is used for the focus border on current Windows
  builds. A future Windows update may change this.
- The resolved theme class is matched by an `ItemsView` suffix so dark-mode
  variants such as `DarkMode_ItemsView` are covered without matching unrelated
  classes such as `ListView`.
- The mod is scoped to `explorer.exe`; common Open/Save dialogs hosted by other
  applications are not affected.
- Older Windows versions are untested.
- The intended targets include regular File Explorer items and
  **This PC > Devices and drives**.
- The same focus-border effect can also be changed by the **Rounded Selection**
  option in Win32 UI Modernizer; using both for this effect is redundant.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <uxtheme.h>
#include <windhawk_utils.h>

#include <wchar.h>

static decltype(&DrawThemeBackground) DrawThemeBackground_Original = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_Original = nullptr;

static bool IsItemsViewTheme(HTHEME hTheme) {
    using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);

    static auto getThemeClass = reinterpret_cast<GetThemeClass_t>(
        GetProcAddress(GetModuleHandleW(L"uxtheme.dll"), MAKEINTRESOURCEA(74)));

    if (!getThemeClass || !hTheme) {
        if (!getThemeClass) {
            Wh_Log(L"GetThemeClass (uxtheme ordinal 74) is unavailable");
        }
        return false;
    }

    WCHAR className[256] = {};
    if (FAILED(getThemeClass(hTheme, className, ARRAYSIZE(className)))) {
        return false;
    }

    // Verified on Windows 11 during review: GetThemeClass returned the full
    // resolved class name "ItemsView" for the File Explorer item view.
    constexpr WCHAR kItemsView[] = L"ItemsView";
    constexpr size_t kItemsViewLength = ARRAYSIZE(kItemsView) - 1;
    const size_t classNameLength = wcslen(className);

    return classNameLength >= kItemsViewLength &&
           _wcsicmp(className + classNameLength - kItemsViewLength,
                    kItemsView) == 0;
}

static bool ShouldSuppressFocusBorder(HTHEME hTheme,
                                      int partId,
                                      int stateId) {
    // Current File Explorer DirectUI ItemsView behavior:
    //   part 3, state 1 = focus border
    // Other states include hover/selection overlays and must remain visible.
    return partId == 3 && stateId == 1 && IsItemsViewTheme(hTheme);
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme,
                                         HDC hdc,
                                         int iPartId,
                                         int iStateId,
                                         LPCRECT pRect,
                                         LPCRECT pClipRect) {
    if (ShouldSuppressFocusBorder(hTheme, iPartId, iStateId)) {
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
    if (ShouldSuppressFocusBorder(hTheme, iPartId, iStateId)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId,
                                          pRect, pOptions);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!WindhawkUtils::SetFunctionHook(DrawThemeBackground,
                                        DrawThemeBackground_Hook,
                                        &DrawThemeBackground_Original) ||
        !WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx,
                                        DrawThemeBackgroundEx_Hook,
                                        &DrawThemeBackgroundEx_Original)) {
        Wh_Log(L"Failed to hook uxtheme drawing functions");
        return FALSE;
    }

    return TRUE;
}
