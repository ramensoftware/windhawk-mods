// ==WindhawkMod==
// @id              invisible-borders
// @name            Invisible Window Borders
// @description     Makes window borders invisible while keeping rounded corners
// @version         1.1.0
// @author          Bo0ii
// @github          https://github.com/Bo0ii
// @homepage        https://github.com/Bo0ii/windhawk-mods
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Invisible Window Borders

Makes window borders invisible while preserving rounded corners.

![Comparison](onoff.png)

## Features

- **Invisible Borders**: Removes border color while keeping frame structure
- **Preserves Rounded Corners**: Keeps window frame intact for corner rendering
- **Universal**: Works with all applications
- **Lightweight**: Minimal performance impact

## Compatibility

- Windows 10 (version 1809 and later)
- Windows 11 (all versions)
- Requires DWM (Desktop Window Manager) to be enabled

## Support

Report issues at: https://github.com/Bo0ii/windhawk-mods/issues

## License

MIT License - Feel free to modify and distribute
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- SpecialWindows: false
  $name: Apply to Special Windows
  $description: Also apply to special windows like dialogs
- ExcludeList: ""
  $name: Exclude processes
  $description: >-
    Comma-separated list of process names to exclude (e.g., msedge.exe,chrome.exe).
    Useful for apps like Edge that render their own borders.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windhawk_api.h>

#include <algorithm>
#include <string>
#include <vector>

// Special DWM color values
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

const COLORREF BorderInvisible = DWMWA_COLOR_NONE;  // No border color
const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

bool SpecialWindows = false;
bool g_isExcludedProcess = false;

void ParseExcludeList(LPCWSTR rawList, std::vector<std::wstring>& out) {
    out.clear();
    if (!rawList || !*rawList) return;

    std::wstring list(rawList);
    size_t start = 0;
    while (start < list.size()) {
        size_t end = list.find(L',', start);
        if (end == std::wstring::npos) end = list.size();

        std::wstring item = list.substr(start, end - start);
        size_t first = item.find_first_not_of(L" \t");
        size_t last = item.find_last_not_of(L" \t");
        if (first != std::wstring::npos) {
            item = item.substr(first, last - first + 1);
            std::transform(item.begin(), item.end(), item.begin(), ::towlower);
            out.push_back(item);
        }

        start = end + 1;
    }
}

bool CheckExcludedProcess(const std::vector<std::wstring>& excludeList) {
    if (excludeList.empty()) return false;

    WCHAR exePath[MAX_PATH];
    if (!GetModuleFileNameW(NULL, exePath, MAX_PATH)) return false;

    WCHAR* fileName = wcsrchr(exePath, L'\\');
    if (!fileName) fileName = exePath; else fileName++;

    std::wstring name(fileName);
    std::transform(name.begin(), name.end(), name.begin(), ::towlower);

    for (const auto& excluded : excludeList) {
        if (name == excluded) return true;
    }
    return false;
}

BOOL IsValidWindow(HWND hWnd) {
    if (g_isExcludedProcess) return FALSE;

    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    // Better exclude context menus
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME ||
           (dwStyle & WS_CAPTION) == WS_CAPTION ||
           (SpecialWindows && (dwStyle & WS_OVERLAPPED) == WS_OVERLAPPED && (dwStyle & WS_POPUP) != WS_POPUP);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;
HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    if (dwAttribute == DWMWA_BORDER_COLOR && IsValidWindow(hwnd)) {
        Wh_Log(L"Intercepted DWMWA_BORDER_COLOR - setting to invisible");
        // Override with invisible border
        return DwmSetWindowAttribute_orig(hwnd, dwAttribute, &BorderInvisible, sizeof(BorderInvisible));
    }

    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

void SetBorderInvisible(HWND hWnd)
{
    if (!IsValidWindow(hWnd))
        return;

    Wh_Log(L"Setting invisible border");
    DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &BorderInvisible, sizeof(BorderInvisible));
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
DefDlgProcA_t DefDlgProcA_orig;
LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefDlgProcW_t = decltype(&DefDlgProcW);
DefDlgProcW_t DefDlgProcW_orig;
LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;

    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid) {
        SetBorderInvisible(hWnd);
    }

    return TRUE;
}

BOOL CALLBACK DisableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid && IsValidWindow(hWnd))
        DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &ColorDefault, sizeof(ColorDefault));

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init - Invisible Borders");

    SpecialWindows = Wh_GetIntSetting(L"SpecialWindows");

    std::vector<std::wstring> excludeList;
    LPCWSTR excludeStr = Wh_GetStringSetting(L"ExcludeList");
    ParseExcludeList(excludeStr, excludeList);
    Wh_FreeStringSetting(excludeStr);
    g_isExcludedProcess = CheckExcludedProcess(excludeList);

    if (g_isExcludedProcess) {
        Wh_Log(L"Process excluded from invisible borders");
        return FALSE;
    }

    Wh_SetFunctionHook(
        (void *)DwmSetWindowAttribute,
        (void *)DwmSetWindowAttribute_hook,
        (void **)&DwmSetWindowAttribute_orig);

    Wh_SetFunctionHook(
        (void *)DefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcW,
        (void *)DefDlgProcW_hook,
        (void **)&DefDlgProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcA,
        (void *)DefDlgProcA_hook,
        (void **)&DefDlgProcA_orig
    );

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit - Applying invisible borders");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit - Restoring default borders");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
    SpecialWindows = Wh_GetIntSetting(L"SpecialWindows");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}
