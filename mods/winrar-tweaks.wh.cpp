// ==WindhawkMod==
// @id              winrar-tweaks
// @name            WinRAR Tweaks
// @description     Convert file sizes into readable units (KB, MB, GB), add a VS Code-style Ctrl+F quick filter bar, close WinRAR with Esc, and optionally auto-dismiss the trial reminder popup.
// @version         1.1.1
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @homepage        https://github.com/ArvindSaini978/my-windhawk-mods
// @license         MIT
// @include         WinRAR.exe
// @compilerOptions -lcomctl32 -luser32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# WinRAR Tweaks

Simple quality-of-life improvements that make **WinRAR** cleaner, easier to read, and faster to use—without changing how your archives work.

---

## ✨ Features

* 🔍 **VS Code-Style Quick Filter Bar (`Ctrl+F`):** Press `Ctrl+F` (or just start typing with *Type to Search* enabled) to open a native bottom filter bar with `↑` / `↓` / `✕` buttons, a live match counter (`1 of 12`), `Enter` / `Shift+Enter` navigation, and optional `Alt+Enter` to select all matches at once. Automatically adapts to both Light and Dark themes.
* 📊 **Readable File Sizes (All Languages):** Converts long, hard-to-read byte numbers (like `121,815,400`) in the **Size** and **Packed** columns as well as the bottom **Status Bar** into clean units (`B`, `KB`, `MB`, `GB`, `TB`). Works automatically in all WinRAR display languages.
* 🏷️ **Optional `KiB` / `MiB` Labels:** Prefer technical binary units? Flip a single switch in Settings to display `KiB`, `MiB`, `GiB`, and `TiB` instead of `KB`, `MB`, `GB`, and `TB`.
* ⌨️ **Smart Close on `Esc`:** Press `Escape` to quickly close WinRAR just like modern apps. Smart enough to close the quick search bar, open popups, menus, dropdowns, or file-rename boxes first before closing the main window.
* 🚫 **Auto-Dismiss Trial Popup (Opt-In):** Optionally blocks the *"Please purchase WinRAR license"* and *"Trial Expired"* reminder popups on startup before they appear on screen (can be enabled in Settings).
* ⚙️ **Customizable Settings:** Enable or disable any feature independently at any time in the Settings tab.

---

## 🖼️ Previews

### Quick Filter Bar
![WinRAR Quick Filter Search Bar Preview](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_qsb_preview.png)

### Human-Readable File Sizes
![WinRAR File Size Before and After Preview](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_preview_1.png)

### Smart Close on Esc
![WinRAR Close on Esc Demo Webp](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_preview_2.webp)

### Auto-Dismiss Trial Popup

#### Before (Default)
![WinRAR Auto-Dismiss Trial Popup Webp 1](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_adtp_1.webp)
#### After (WinRAR Tweaks)
![WinRAR Auto-Dismiss Trial Popup Webp 2](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_adtp_2.webp)

---

## 🛠️ How It Works

* **Safe Quick Filtering:** Instead of deleting rows from WinRAR's list (which would break extraction indices), the quick filter bar highlights matching files in place, scrolls the active match into view, and lets you cycle through matches with `Enter` / `Shift+Enter` or `Up` / `Down`. Clicking the empty bar area returns focus to the file list while keeping your match highlighted.
* **Smart Size Formatting:** Formats raw byte numbers on the fly in both the file list and bottom status bar as you browse folders inside an archive, supporting every language automatically.
* **Safe `Esc` Handling:** Checks what you are doing before closing—so pressing `Esc` while using the search bar, renaming a file, dragging items, or browsing a right-click menu won't accidentally close your window.
* **Instant Popup Blocking:** When enabled in Settings, detects WinRAR's startup trial reminder in the background and dismisses it before the window is drawn, so you never see a flicker. All other WinRAR dialogs (Extract, Add, Password, Info, Help, etc.) work normally.

---

## 💡 Good to Know

* **Sorting Still Works Perfectly:** Clicking the **Size**, **Packed**, or **Name** column headers still sorts your files accurately and automatically updates active filter matches.
* **Toggling Settings Live:** Switching between `KB` and `KiB` or toggling the *Always Visible* search bar updates open WinRAR windows right away without freezing. If you turn off *Human-Readable File Sizes* completely while WinRAR is open, press **`F5`** (or open another folder) to switch the file list numbers back to raw bytes.

---

### 🧩 More from this author
* **[Drag to Scroll](https://windhawk.net/mods/drag-to-scroll)** – Hold `Ctrl + Right-Click` and drag to scroll smoothly across desktop applications.
* **[Explorer Custom Shortcuts](https://windhawk.net/mods/explorer-custom-shortcuts)** – Customizable keyboard shortcuts for File Explorer: open terminal, copy paths, create files/folders, toggle hidden items, and launch apps with hotkeys.

### Author & Attribution
* **Created and maintained by ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978)).**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- quick_search_bar:
  - enabled: true
    $name: "Enable Quick Filter Search Bar"
    $description: "Press Ctrl+F to open a bottom filter bar with match counter and Up/Down navigation."
  - always_visible: false
    $name: "Always Visible"
    $description: "Keep the search bar permanently docked at the bottom of the file list instead of hiding it until Ctrl+F or typing."
  - type_to_search: false
    $name: "Type to Search"
    $description: "Automatically show and focus the search bar as soon as you start typing characters (including Shift + capital letters) in the file list."
  - alt_enter_select_all: false
    $name: "Alt+Enter Selects All Matches"
    $description: "Pressing Alt+Enter in the search box selects all matching files at once and moves focus to the file list."
  $name: "Quick Filter Search Bar"
  $description: "Configure the VS Code-style quick filter bar, activation modes, and shortcuts."
- close_on_esc: true
  $name: "Close on Esc"
  $description: "Pressing Escape closes the active WinRAR dialog or main window while respecting menus, dropdowns, and inline text editing."
- readable_file_sizes: true
  $name: "Human-Readable File Sizes"
  $description: "Formats raw byte values in the Size and Packed columns and the bottom status bar into readable units (KB, MB, GB, TB) across all languages."
- use_iec_terms: false
  $name: "Use IEC terms"
  $description: "Use the International Electrotechnical Commission terms, such as KiB instead of KB."
- auto_dismiss_eval_dialog: false
  $name: "Auto-Dismiss Trial Popup"
  $description: "Automatically blocks the 'Please purchase WinRAR license' and 'Trial Expired' popups on startup before they appear on screen."
*/
// ==/WindhawkModSettings==

// clang-format on

#include <commctrl.h>
#include <uxtheme.h>
#include <windhawk_utils.h>
#include <atomic>
#include <climits>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Holds runtime configuration state synced with Windhawk settings
struct ModSettings {
    std::atomic<bool> search_enabled{true};
    std::atomic<bool> search_always_visible{false};
    std::atomic<bool> search_type_to_search{false};
    std::atomic<bool> search_alt_enter_select_all{false};
    std::atomic<bool> close_on_esc{true};
    std::atomic<bool> readable_file_sizes{true};
    std::atomic<bool> use_iec_terms{false};
    std::atomic<bool> auto_dismiss_eval_dialog{false};
};

static ModSettings g_settings;
static std::mutex g_listViewMutex;
static std::unordered_set<HWND> g_subclassedListViews;
static std::unordered_set<HWND> g_subclassedStatusBars;

// Stores the original unformatted status bar text per part (0..7) so live
// setting changes can re-format or restore raw bytes immediately
struct StatusBarState {
    std::wstring rawPartText[8];
    WPARAM rawPartFlags[8] = {};
    bool hasPart[8] = {};
    bool isInternalUpdate = false;
};

static std::unordered_map<HWND, StatusBarState> g_statusBarStates;

// Custom thread-safe messages dispatched to the ListView's own UI thread
static UINT g_msgSettingsChanged = 0;
static UINT g_msgCleanupSearchBar = 0;

// Control IDs for the Quick Filter Search Bar child controls
constexpr int IDC_SEARCH_EDIT = 1001;
constexpr int IDC_SEARCH_COUNT = 1002;
constexpr int IDC_SEARCH_PREV = 1003;
constexpr int IDC_SEARCH_NEXT = 1004;
constexpr int IDC_SEARCH_CLOSE = 1005;

// Base dimensions (in 96-DPI pixels) with 1px reduced top/bottom padding (22px
// controls in 30px bar)
constexpr int kSearchBarBaseHeight = 30;
constexpr int kSearchCtrlBaseHeight = 22;
constexpr int kSearchBtnBaseWidth = 22;
constexpr int kCornerRadiusBase = 6;

// Unified color palette for identical visual styling in Light and Dark mode
struct SearchBarPalette {
    COLORREF barBg;
    COLORREF barBorder;
    COLORREF editBg;
    COLORREF editFocusBg;
    COLORREF editBorder;
    COLORREF editFocusBorder;
    COLORREF editText;
    COLORREF cueText;
    COLORREF labelText;
    COLORREF btnBg;
    COLORREF btnHoverBg;
    COLORREF btnPressBg;
    COLORREF btnBorder;
    COLORREF btnHoverBorder;
    COLORREF btnText;
    COLORREF btnDisabledText;
};

// Per-window state for the Quick Filter Search Bar attached to a main ListView
struct QuickSearchState {
    HWND hMainWnd = nullptr;
    HWND hListView = nullptr;
    HWND hBarContainer = nullptr;
    HWND hEdit = nullptr;
    HWND hCountLabel = nullptr;
    HWND hPrevBtn = nullptr;
    HWND hNextBtn = nullptr;
    HWND hCloseBtn = nullptr;
    HWND hTooltip = nullptr;
    HFONT hFont = nullptr;
    UINT fontDpi = 0;
    HBRUSH hEditBgBrush = nullptr;
    HBRUSH hBarBgBrush = nullptr;
    COLORREF cachedEditBgColor = CLR_INVALID;
    COLORREF cachedBarBgColor = CLR_INVALID;
    SearchBarPalette palette = {};
    bool paletteInitialized = false;
    RECT rcEditFrame = {};
    bool isDark = false;
    bool isVisible = false;
    bool isAdjustingLayout = false;
    bool suppressTextChange = false;
    bool multiSelected = false;
    bool addedShowSelAlways = false;
    std::vector<int> matchedIndices;
    int currentMatchPos = -1;
};

static std::unordered_map<HWND, QuickSearchState> g_searchStates;

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t pfnCreateWindowExW_Original = nullptr;

// Loads user preferences from Windhawk storage into atomic variables
static void LoadSettings() {
    g_settings.search_enabled =
        Wh_GetIntSetting(L"quick_search_bar.enabled") != 0;
    g_settings.search_always_visible =
        Wh_GetIntSetting(L"quick_search_bar.always_visible") != 0;
    g_settings.search_type_to_search =
        Wh_GetIntSetting(L"quick_search_bar.type_to_search") != 0;
    g_settings.search_alt_enter_select_all =
        Wh_GetIntSetting(L"quick_search_bar.alt_enter_select_all") != 0;

    g_settings.close_on_esc = Wh_GetIntSetting(L"close_on_esc") != 0;
    g_settings.readable_file_sizes =
        Wh_GetIntSetting(L"readable_file_sizes") != 0;
    g_settings.use_iec_terms = Wh_GetIntSetting(L"use_iec_terms") != 0;
    g_settings.auto_dismiss_eval_dialog =
        Wh_GetIntSetting(L"auto_dismiss_eval_dialog") != 0;
}

// Performs a case-insensitive substring search on wide strings
static bool ContainsCaseInsensitive(const WCHAR* haystack,
                                    const WCHAR* needle) {
    if (!haystack || !needle || !*needle)
        return false;

    size_t needleLen = wcslen(needle);
    for (const WCHAR* p = haystack; *p; ++p) {
        if (_wcsnicmp(p, needle, needleLen) == 0)
            return true;
    }
    return false;
}

// Queries the DPI for a given window using a cached function pointer
static UINT GetWindowDpiSafe(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static GetDpiForWindow_t pfnGetDpiForWindow = []() -> GetDpiForWindow_t {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        return hUser32 ? reinterpret_cast<GetDpiForWindow_t>(
                             GetProcAddress(hUser32, "GetDpiForWindow"))
                       : nullptr;
    }();

    if (pfnGetDpiForWindow && hWnd) {
        UINT wndDpi = pfnGetDpiForWindow(hWnd);
        if (wndDpi > 0)
            return wndDpi;
    }
    return 96;
}

// Helper to scale pixel dimensions according to the window's current DPI
static int ScaleForDpi(HWND hWnd, int px) {
    return MulDiv(px, static_cast<int>(GetWindowDpiSafe(hWnd)), 96);
}

// Creates a DPI-scaled message font for the search bar
static HFONT CreateSearchBarFontForDpi(UINT dpi) {
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(ncm);
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
        ncm.lfMessageFont.lfHeight = -MulDiv(12, static_cast<int>(dpi), 96);
        HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
        if (hFont)
            return hFont;
    }
    return static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
}

// Ensures the search bar font matches the current monitor DPI
static void EnsureSearchBarFontDpi(QuickSearchState* state) {
    if (!state || !IsWindow(state->hBarContainer))
        return;

    UINT currentDpi = GetWindowDpiSafe(state->hBarContainer);
    if (state->hFont && state->fontDpi == currentDpi)
        return;

    HFONT hNewFont = CreateSearchBarFontForDpi(currentDpi);
    HFONT hOldFont = state->hFont;
    state->hFont = hNewFont;
    state->fontDpi = currentDpi;

    if (IsWindow(state->hEdit))
        SendMessageW(state->hEdit, WM_SETFONT,
                     reinterpret_cast<WPARAM>(hNewFont), FALSE);
    if (IsWindow(state->hCountLabel))
        SendMessageW(state->hCountLabel, WM_SETFONT,
                     reinterpret_cast<WPARAM>(hNewFont), FALSE);
    if (IsWindow(state->hPrevBtn))
        SendMessageW(state->hPrevBtn, WM_SETFONT,
                     reinterpret_cast<WPARAM>(hNewFont), FALSE);
    if (IsWindow(state->hNextBtn))
        SendMessageW(state->hNextBtn, WM_SETFONT,
                     reinterpret_cast<WPARAM>(hNewFont), FALSE);
    if (IsWindow(state->hCloseBtn))
        SendMessageW(state->hCloseBtn, WM_SETFONT,
                     reinterpret_cast<WPARAM>(hNewFont), FALSE);

    if (hOldFont && hOldFont != GetStockObject(DEFAULT_GUI_FONT)) {
        DeleteObject(hOldFont);
    }
}

// Checks whether a child control belongs directly to WinRarWindow
static bool IsChildOfMainWinRarWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd))
        return false;

    HWND hParent = GetParent(hWnd);
    if (!hParent)
        return false;

    WCHAR parentCls[64] = {};
    if (!GetClassNameW(hParent, parentCls, ARRAYSIZE(parentCls)))
        return false;

    return (_wcsicmp(parentCls, L"WinRarWindow") == 0);
}

// Checks whether a ListView is the primary archive file list inside
// WinRarWindow
static bool IsMainWinRarListView(HWND hListView) {
    return IsChildOfMainWinRarWindow(hListView);
}

// Reliably detects whether WinRAR's ListView is using a Dark or Light theme
static bool IsWinRarDarkTheme(HWND hListView) {
    if (hListView && IsWindow(hListView)) {
        COLORREF bk = ListView_GetBkColor(hListView);
        if (bk != CLR_NONE && bk != CLR_DEFAULT) {
            int lum = (GetRValue(bk) * 299 + GetGValue(bk) * 587 +
                       GetBValue(bk) * 114) /
                      1000;
            return (lum < 128);
        }
        COLORREF txt = ListView_GetTextColor(hListView);
        if (txt != CLR_NONE && txt != CLR_DEFAULT) {
            int txtLum = (GetRValue(txt) * 299 + GetGValue(txt) * 587 +
                          GetBValue(txt) * 114) /
                         1000;
            return (txtLum > 160);
        }
    }

    using ShouldAppsUseDarkMode_t = bool(WINAPI*)();
    static ShouldAppsUseDarkMode_t pfnShouldAppsUseDarkMode =
        []() -> ShouldAppsUseDarkMode_t {
        HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
        return hUxTheme ? reinterpret_cast<ShouldAppsUseDarkMode_t>(
                              GetProcAddress(hUxTheme, MAKEINTRESOURCEA(132)))
                        : nullptr;
    }();

    if (pfnShouldAppsUseDarkMode && pfnShouldAppsUseDarkMode()) {
        return true;
    }

    DWORD appsUseLight = 1;
    DWORD cbData = sizeof(appsUseLight);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Pe"
                     L"rsonalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr,
                     &appsUseLight, &cbData) == ERROR_SUCCESS) {
        if (appsUseLight == 0)
            return true;
    }

    return false;
}

// Returns a tasteful focus border color (custom DWM accent or warm orange)
static COLORREF GetFocusBorderColor(bool isDark) {
    DWORD abgr = 0;
    DWORD cbData = sizeof(abgr);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &abgr,
                     &cbData) == ERROR_SUCCESS) {
        BYTE r = static_cast<BYTE>(abgr & 0xFF);
        BYTE g = static_cast<BYTE>((abgr >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>((abgr >> 16) & 0xFF);

        bool isDefaultBlue = (b > 180 && r < 40 && g > 90 && g < 150);
        if (!isDefaultBlue) {
            if (isDark) {
                r = static_cast<BYTE>((r * 4 + 180) / 5);
                g = static_cast<BYTE>((g * 4 + 180) / 5);
                b = static_cast<BYTE>((b * 4 + 180) / 5);
            }
            return RGB(r, g, b);
        }
    }

    return isDark ? RGB(218, 128, 44) : RGB(210, 110, 25);
}

// Computes the unified design palette for either Dark or Light mode
static SearchBarPalette ComputeThemePalette(bool isDark) {
    COLORREF focusClr = GetFocusBorderColor(isDark);
    if (isDark) {
        return {
            RGB(44, 44, 46),     // barBg
            RGB(74, 74, 78),     // barBorder
            RGB(33, 33, 36),     // editBg
            RGB(38, 38, 42),     // editFocusBg
            RGB(86, 86, 92),     // editBorder
            focusClr,            // editFocusBorder
            RGB(244, 244, 246),  // editText
            RGB(148, 148, 154),  // cueText
            RGB(205, 205, 210),  // labelText
            RGB(58, 58, 62),     // btnBg
            RGB(76, 76, 82),     // btnHoverBg
            RGB(34, 34, 36),     // btnPressBg
            RGB(88, 88, 92),     // btnBorder
            RGB(115, 115, 120),  // btnHoverBorder
            RGB(240, 240, 240),  // btnText
            RGB(115, 115, 120)   // btnDisabledText
        };
    } else {
        return {
            RGB(240, 240, 243),  // barBg
            RGB(204, 204, 208),  // barBorder
            RGB(255, 255, 255),  // editBg
            RGB(255, 255, 255),  // editFocusBg
            RGB(190, 190, 196),  // editBorder
            focusClr,            // editFocusBorder
            RGB(24, 24, 24),     // editText
            RGB(128, 128, 132),  // cueText
            RGB(68, 68, 72),     // labelText
            RGB(252, 252, 253),  // btnBg
            RGB(230, 230, 234),  // btnHoverBg
            RGB(214, 214, 218),  // btnPressBg
            RGB(194, 194, 200),  // btnBorder
            RGB(164, 164, 170),  // btnHoverBorder
            RGB(32, 32, 34),     // btnText
            RGB(160, 160, 165)   // btnDisabledText
        };
    }
}

// Updates cached GDI brushes when focus or palette changes (no registry reads)
static void UpdateSearchBarBrushes(QuickSearchState* state) {
    if (!state)
        return;

    if (!state->paletteInitialized) {
        state->isDark = IsWinRarDarkTheme(state->hListView);
        state->palette = ComputeThemePalette(state->isDark);
        state->paletteInitialized = true;
    }

    const SearchBarPalette& pal = state->palette;
    bool hasFocus = (state->hEdit && GetFocus() == state->hEdit);
    COLORREF targetEditBg = hasFocus ? pal.editFocusBg : pal.editBg;

    if (!state->hEditBgBrush || state->cachedEditBgColor != targetEditBg) {
        if (state->hEditBgBrush)
            DeleteObject(state->hEditBgBrush);
        state->hEditBgBrush = CreateSolidBrush(targetEditBg);
        state->cachedEditBgColor = targetEditBg;
    }

    if (!state->hBarBgBrush || state->cachedBarBgColor != pal.barBg) {
        if (state->hBarBgBrush)
            DeleteObject(state->hBarBgBrush);
        state->hBarBgBrush = CreateSolidBrush(pal.barBg);
        state->cachedBarBgColor = pal.barBg;
    }
}

// Re-evaluates Dark/Light theme and accent color (called on theme changes or
// creation)
static void RefreshSearchBarTheme(QuickSearchState* state, bool forceRedraw) {
    if (!state || !IsWindow(state->hListView))
        return;

    bool darkNow = IsWinRarDarkTheme(state->hListView);
    bool changed =
        !state->paletteInitialized || (darkNow != state->isDark) || forceRedraw;

    state->isDark = darkNow;
    if (changed) {
        state->palette = ComputeThemePalette(state->isDark);
        state->paletteInitialized = true;
    }

    UpdateSearchBarBrushes(state);

    if (changed && state->hTooltip && IsWindow(state->hTooltip)) {
        SetWindowTheme(state->hTooltip,
                       state->isDark ? L"DarkMode_Explorer" : nullptr, nullptr);
    }

    if (changed && IsWindow(state->hBarContainer)) {
        RedrawWindow(
            state->hBarContainer, nullptr, nullptr,
            RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    }
}

// Forward declarations for Quick Filter Search Bar helpers
static QuickSearchState* GetSearchStateForListView(HWND hListView);
static QuickSearchState* GetSearchStateForActiveWindow(HWND hActive);
static void ShowSearchBar(QuickSearchState* state, bool focusEdit);
static void HideOrClearSearchBar(QuickSearchState* state, bool forceHide);

// ---------------------------------------------------------------------------
// 1. Smart Esc-to-Close & Ctrl+F Shortcut Logic
// ---------------------------------------------------------------------------

// Checks if keyboard focus is currently inside a text input or combobox
static bool IsFocusInEditableControl(HWND hFocus) {
    if (!hFocus)
        return false;

    WCHAR cls[64] = {};
    if (!GetClassNameW(hFocus, cls, ARRAYSIZE(cls)))
        return false;

    return (_wcsicmp(cls, L"Edit") == 0 || _wcsicmp(cls, L"ComboBox") == 0 ||
            _wcsicmp(cls, L"ComboBoxEx32") == 0 ||
            wcsstr(cls, L"RichEdit") != nullptr ||
            wcsstr(cls, L"RICHEDIT") != nullptr);
}

// Intercepts Ctrl+F in the main WinRAR window to open and focus the search bar
static bool ProcessSearchShortcut(MSG* pMsg) {
    if (!g_settings.search_enabled || !pMsg)
        return false;

    if (pMsg->message != WM_KEYDOWN || pMsg->wParam != 'F')
        return false;

    if (GetKeyState(VK_CONTROL) >= 0 || GetKeyState(VK_MENU) < 0 ||
        GetKeyState(VK_SHIFT) < 0) {
        return false;
    }

    HWND hActive = GetActiveWindow();
    if (!hActive)
        return false;

    HWND hRoot = GetAncestor(hActive, GA_ROOT);
    if (!hRoot)
        hRoot = hActive;

    QuickSearchState* state = GetSearchStateForActiveWindow(hRoot);
    if (!state || !IsWindow(state->hListView))
        return false;

    ShowSearchBar(state, true);
    pMsg->message = WM_NULL;
    return true;
}

// Intercepts Escape keydown to close the search bar first, or close WinRAR
static bool ProcessEscapeKey(MSG* pMsg) {
    if (!pMsg || pMsg->message != WM_KEYDOWN || pMsg->wParam != VK_ESCAPE)
        return false;

    if ((pMsg->lParam & (1LL << 30)) != 0)
        return false;

    GUITHREADINFO gti = {};
    gti.cbSize = sizeof(gti);
    if (GetGUIThreadInfo(GetCurrentThreadId(), &gti)) {
        constexpr DWORD kModalFlags = GUI_INMENUMODE | GUI_POPUPMENUMODE |
                                      GUI_SYSTEMMENUMODE | GUI_INMOVESIZE;
        if ((gti.flags & kModalFlags) != 0 || gti.hwndCapture != nullptr ||
            gti.hwndMenuOwner != nullptr) {
            return false;
        }
    } else if (GetCapture() != nullptr) {
        return false;
    }

    HWND hActive = gti.hwndActive ? gti.hwndActive : GetActiveWindow();
    if (!hActive)
        return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hActive, &pid);
    if (pid != GetCurrentProcessId())
        return false;

    HWND hTarget = GetAncestor(hActive, GA_ROOT);
    if (!hTarget)
        hTarget = hActive;

    if (g_settings.search_enabled) {
        QuickSearchState* state = GetSearchStateForActiveWindow(hTarget);
        if (state && state->isVisible) {
            bool focusInBar = (gti.hwndFocus != nullptr &&
                               (gti.hwndFocus == state->hEdit ||
                                IsChild(state->hBarContainer, gti.hwndFocus)));
            int textLen = state->hEdit ? GetWindowTextLengthW(state->hEdit) : 0;

            if (!g_settings.search_always_visible || focusInBar ||
                textLen > 0) {
                HideOrClearSearchBar(state, false);
                pMsg->message = WM_NULL;
                return true;
            }
        }
    }

    if (IsFocusInEditableControl(gti.hwndFocus))
        return false;

    if (!g_settings.close_on_esc)
        return false;

    PostMessageW(hTarget, WM_CLOSE, 0, 0);
    pMsg->message = WM_NULL;
    return true;
}

using GetMessageW_t = decltype(&GetMessageW);
static GetMessageW_t pfnGetMessageW_Original = nullptr;

// Hooks GetMessageW to process Ctrl+F and Escape key presses
BOOL WINAPI GetMessageW_Hook(LPMSG lpMsg,
                             HWND hWnd,
                             UINT wMsgFilterMin,
                             UINT wMsgFilterMax) {
    BOOL res =
        pfnGetMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (res > 0 && lpMsg) {
        if (!ProcessSearchShortcut(lpMsg)) {
            ProcessEscapeKey(lpMsg);
        }
    }
    return res;
}

using PeekMessageW_t = decltype(&PeekMessageW);
static PeekMessageW_t pfnPeekMessageW_Original = nullptr;

// Hooks PeekMessageW to process Ctrl+F and Escape key presses in modal loops
BOOL WINAPI PeekMessageW_Hook(LPMSG lpMsg,
                              HWND hWnd,
                              UINT wMsgFilterMin,
                              UINT wMsgFilterMax,
                              UINT wRemoveMsg) {
    BOOL res = pfnPeekMessageW_Original(lpMsg, hWnd, wMsgFilterMin,
                                        wMsgFilterMax, wRemoveMsg);
    if (res && lpMsg && (wRemoveMsg & PM_REMOVE)) {
        if (!ProcessSearchShortcut(lpMsg)) {
            ProcessEscapeKey(lpMsg);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------
// 2. Quick Filter Search Bar Engine & Unified Light/Dark UI
// ---------------------------------------------------------------------------

static QuickSearchState* GetSearchStateForListView(HWND hListView) {
    std::lock_guard<std::mutex> lock(g_listViewMutex);
    auto it = g_searchStates.find(hListView);
    return (it != g_searchStates.end()) ? &it->second : nullptr;
}

static QuickSearchState* GetSearchStateForActiveWindow(HWND hRoot) {
    std::lock_guard<std::mutex> lock(g_listViewMutex);
    for (auto& kv : g_searchStates) {
        if (kv.second.hMainWnd == hRoot && IsWindow(kv.second.hListView)) {
            return &kv.second;
        }
    }
    return nullptr;
}

// Clears all selection highlights in the ListView
static void ClearListViewSelection(HWND hListView) {
    LVITEMW lvi = {};
    lvi.stateMask = LVIS_SELECTED;
    lvi.state = 0;
    SendMessageW(hListView, LVM_SETITEMSTATE, static_cast<WPARAM>(-1),
                 reinterpret_cast<LPARAM>(&lvi));
}

// Highlights and scrolls a single active match into view (VS Code style)
static void HighlightSingleMatch(HWND hListView, int itemIndex) {
    if (itemIndex < 0)
        return;

    ClearListViewSelection(hListView);

    LVITEMW lvi = {};
    lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
    SendMessageW(hListView, LVM_SETITEMSTATE, static_cast<WPARAM>(itemIndex),
                 reinterpret_cast<LPARAM>(&lvi));

    ListView_EnsureVisible(hListView, itemIndex, FALSE);
}

// Helper to enable/disable a button and redraw only when its state actually
// changes
static void SetButtonEnabledState(HWND hBtn, BOOL bEnable) {
    if (!hBtn || !IsWindow(hBtn))
        return;
    BOOL wasEnabled = IsWindowEnabled(hBtn);
    if ((wasEnabled != FALSE) != (bEnable != FALSE)) {
        EnableWindow(hBtn, bEnable);
        InvalidateRect(hBtn, nullptr, FALSE);
    }
}

// Updates the match counter text ("1 of 12", "No results", or "") without
// redundant repaints
static void UpdateMatchCounterLabel(QuickSearchState* state, bool hasQuery) {
    if (!state || !IsWindow(state->hCountLabel))
        return;

    if (!hasQuery) {
        SetWindowTextW(state->hCountLabel, L"");
        SetButtonEnabledState(state->hPrevBtn, FALSE);
        SetButtonEnabledState(state->hNextBtn, FALSE);
    } else {
        int total = static_cast<int>(state->matchedIndices.size());
        if (total == 0) {
            SetWindowTextW(state->hCountLabel, L"No results");
            SetButtonEnabledState(state->hPrevBtn, FALSE);
            SetButtonEnabledState(state->hNextBtn, FALSE);
        } else {
            WCHAR buf[64] = {};
            swprintf_s(buf, ARRAYSIZE(buf), L"%d of %d",
                       state->currentMatchPos + 1, total);
            SetWindowTextW(state->hCountLabel, buf);
            SetButtonEnabledState(state->hPrevBtn, total > 1);
            SetButtonEnabledState(state->hNextBtn, total > 1);
        }
    }

    InvalidateRect(state->hCountLabel, nullptr, TRUE);
}

// Scans the Name column (column 0) and updates matchedIndices
static void ExecuteQuickFilter(QuickSearchState* state) {
    if (!state || state->suppressTextChange || !IsWindow(state->hListView) ||
        !IsWindow(state->hEdit)) {
        return;
    }

    WCHAR query[256] = {};
    GetWindowTextW(state->hEdit, query, ARRAYSIZE(query));

    state->matchedIndices.clear();
    state->currentMatchPos = -1;

    if (query[0] == L'\0') {
        state->multiSelected = false;
        UpdateMatchCounterLabel(state, false);
        return;
    }

    state->multiSelected = false;

    int itemCount = ListView_GetItemCount(state->hListView);
    for (int i = 0; i < itemCount; ++i) {
        WCHAR itemName[MAX_PATH] = {};
        LVITEMW lvi = {};
        lvi.iSubItem = 0;
        lvi.pszText = itemName;
        lvi.cchTextMax = ARRAYSIZE(itemName);

        if (SendMessageW(state->hListView, LVM_GETITEMTEXTW,
                         static_cast<WPARAM>(i),
                         reinterpret_cast<LPARAM>(&lvi)) <= 0) {
            continue;
        }

        if (wcscmp(itemName, L"..") == 0)
            continue;

        if (ContainsCaseInsensitive(itemName, query)) {
            state->matchedIndices.push_back(i);
        }
    }

    if (!state->matchedIndices.empty()) {
        state->currentMatchPos = 0;
        HighlightSingleMatch(state->hListView, state->matchedIndices[0]);
    } else {
        ClearListViewSelection(state->hListView);
    }

    UpdateMatchCounterLabel(state, true);
}

// Moves to the next (+1) or previous (-1) matched file with wrap-around
static void StepQuickFilterMatch(QuickSearchState* state, int delta) {
    if (!state || state->matchedIndices.empty())
        return;

    state->multiSelected = false;
    int total = static_cast<int>(state->matchedIndices.size());
    state->currentMatchPos =
        (state->currentMatchPos + delta % total + total) % total;

    int targetIdx = state->matchedIndices[state->currentMatchPos];
    HighlightSingleMatch(state->hListView, targetIdx);
    UpdateMatchCounterLabel(state, true);
}

// Selects all matched items at once and transfers keyboard focus to the
// ListView
static void SelectAllFilterMatches(QuickSearchState* state) {
    if (!state || !IsWindow(state->hListView) || state->matchedIndices.empty())
        return;

    ClearListViewSelection(state->hListView);

    for (size_t i = 0; i < state->matchedIndices.size(); ++i) {
        int idx = state->matchedIndices[i];
        LVITEMW lvi = {};
        lvi.stateMask = LVIS_SELECTED | (i == 0 ? LVIS_FOCUSED : 0);
        lvi.state = LVIS_SELECTED | (i == 0 ? LVIS_FOCUSED : 0);
        SendMessageW(state->hListView, LVM_SETITEMSTATE,
                     static_cast<WPARAM>(idx), reinterpret_cast<LPARAM>(&lvi));
    }

    state->multiSelected = (state->matchedIndices.size() > 1);
    ListView_EnsureVisible(state->hListView, state->matchedIndices[0], FALSE);
    SetFocus(state->hListView);
}

// Positions child controls in order: [Search Box]   [↑] [↓] [✕]  [Match
// Counter]
static void LayoutSearchBarChildren(QuickSearchState* state,
                                    int barWidth,
                                    int barHeight) {
    if (!state || !IsWindow(state->hBarContainer))
        return;

    EnsureSearchBarFontDpi(state);

    int innerTop = 1;
    int innerHeight = (barHeight > 2) ? (barHeight - 2) : barHeight;

    int padX = ScaleForDpi(state->hBarContainer, 6);
    int editToBtnGap = ScaleForDpi(state->hBarContainer, 7);
    int btnGap = ScaleForDpi(state->hBarContainer, 4);
    int countGap = ScaleForDpi(state->hBarContainer, 8);
    int ctrlHeight = ScaleForDpi(state->hBarContainer, kSearchCtrlBaseHeight);
    if (ctrlHeight > innerHeight - 2) {
        ctrlHeight = innerHeight - 2;
    }
    if (ctrlHeight < 14) {
        ctrlHeight = 14;
    }

    int ctrlY = innerTop + (innerHeight - ctrlHeight) / 2;
    int btnWidth = ScaleForDpi(state->hBarContainer, kSearchBtnBaseWidth);
    int countWidth = ScaleForDpi(state->hBarContainer, 80);

    int rightEdge = barWidth - padX;
    int totalRightControls =
        editToBtnGap + (btnWidth * 3) + (btnGap * 2) + countGap + countWidth;
    int availEditWidth = rightEdge - padX - totalRightControls;

    int maxEditWidth = ScaleForDpi(state->hBarContainer, 310);
    if (availEditWidth < ScaleForDpi(state->hBarContainer, 90))
        availEditWidth = ScaleForDpi(state->hBarContainer, 90);
    int editWidth =
        (availEditWidth > maxEditWidth) ? maxEditWidth : availEditWidth;

    int currX = padX;

    // 1. Search Input Frame & inner Edit control (vertically centered by exact
    // font height)
    state->rcEditFrame = {currX, ctrlY, currX + editWidth, ctrlY + ctrlHeight};

    if (state->hEdit) {
        int editPadX = ScaleForDpi(state->hBarContainer, 6);

        // Measure actual font height so top-aligned Win32 Edit text sits
        // dead-center
        int fontHeight = ScaleForDpi(state->hBarContainer, 15);
        HDC hdc = GetDC(state->hEdit);
        if (hdc) {
            HFONT hFont =
                state->hFont
                    ? state->hFont
                    : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
            HGDIOBJ oldFont = SelectObject(hdc, hFont);
            TEXTMETRICW tm = {};
            if (GetTextMetricsW(hdc, &tm)) {
                fontHeight = tm.tmHeight;
            }
            SelectObject(hdc, oldFont);
            ReleaseDC(state->hEdit, hdc);
        }

        int innerEditH = fontHeight;
        if (innerEditH > ctrlHeight - 2)
            innerEditH = ctrlHeight - 2;
        int innerEditY = ctrlY + (ctrlHeight - innerEditH) / 2;

        SetWindowPos(state->hEdit, nullptr, currX + editPadX, innerEditY,
                     editWidth - (editPadX * 2), innerEditH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        currX += editWidth + editToBtnGap;
    }

    // 2. Navigation & Close buttons directly next to the search box
    if (state->hPrevBtn) {
        SetWindowPos(state->hPrevBtn, nullptr, currX, ctrlY, btnWidth,
                     ctrlHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        currX += btnWidth + btnGap;
    }
    if (state->hNextBtn) {
        SetWindowPos(state->hNextBtn, nullptr, currX, ctrlY, btnWidth,
                     ctrlHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        currX += btnWidth + btnGap;
    }
    if (state->hCloseBtn) {
        SetWindowPos(state->hCloseBtn, nullptr, currX, ctrlY, btnWidth,
                     ctrlHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        currX += btnWidth + countGap;
    }

    // 3. Match counter label placed after the buttons so UI stays consistent
    // when empty
    if (state->hCountLabel) {
        int labelHeight = ScaleForDpi(state->hBarContainer, 18);
        if (labelHeight > innerHeight)
            labelHeight = innerHeight;
        int labelY = innerTop + (innerHeight - labelHeight) / 2;
        SetWindowPos(state->hCountLabel, nullptr, currX, labelY, countWidth,
                     labelHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Forces WinRAR to recalculate child window positions (triggers WM_SIZE)
static void TriggerMainLayoutRefresh(QuickSearchState* state) {
    if (!state || !IsWindow(state->hMainWnd))
        return;

    RECT rc = {};
    if (GetClientRect(state->hMainWnd, &rc)) {
        WPARAM sizeType =
            IsZoomed(state->hMainWnd) ? SIZE_MAXIMIZED : SIZE_RESTORED;
        SendMessageW(state->hMainWnd, WM_SIZE, sizeType,
                     MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
        RedrawWindow(state->hMainWnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
}

// Shows the search bar and optionally focuses the text input
static void ShowSearchBar(QuickSearchState* state, bool focusEdit) {
    if (!state || !IsWindow(state->hBarContainer))
        return;

    RefreshSearchBarTheme(state, false);

    if (!state->isVisible) {
        state->isVisible = true;
        ShowWindow(state->hBarContainer, SW_SHOWNOACTIVATE);
        TriggerMainLayoutRefresh(state);
    }

    if (focusEdit && IsWindow(state->hEdit)) {
        SetFocus(state->hEdit);
        SendMessageW(state->hEdit, EM_SETSEL, 0, -1);
    }
}

// Clears the search bar and hides it while preserving Alt+Enter multi-selection
static void HideOrClearSearchBar(QuickSearchState* state, bool forceHide) {
    if (!state)
        return;

    int lastSelectedMatch = -1;
    if (state->currentMatchPos >= 0 &&
        state->currentMatchPos <
            static_cast<int>(state->matchedIndices.size())) {
        lastSelectedMatch = state->matchedIndices[state->currentMatchPos];
    }

    bool preserveMultiSelection = state->multiSelected;
    if (!preserveMultiSelection && IsWindow(state->hListView)) {
        if (ListView_GetSelectedCount(state->hListView) > 1) {
            preserveMultiSelection = true;
        }
    }

    if (IsWindow(state->hEdit)) {
        state->suppressTextChange = true;
        SetWindowTextW(state->hEdit, L"");
        state->suppressTextChange = false;
    }

    state->matchedIndices.clear();
    state->currentMatchPos = -1;
    state->multiSelected = false;
    UpdateMatchCounterLabel(state, false);

    bool shouldHide = forceHide || !g_settings.search_always_visible ||
                      !g_settings.search_enabled;

    if (shouldHide && state->isVisible) {
        state->isVisible = false;
        if (IsWindow(state->hBarContainer)) {
            ShowWindow(state->hBarContainer, SW_HIDE);
        }
        TriggerMainLayoutRefresh(state);
    }

    if (IsWindow(state->hListView)) {
        if (!preserveMultiSelection && lastSelectedMatch >= 0) {
            HighlightSingleMatch(state->hListView, lastSelectedMatch);
        }
        SetFocus(state->hListView);
    }
}

// Registers a tooltip string for a child control inside the search bar
static void AddControlTooltip(HWND hTooltip, HWND hControl, LPCWSTR text) {
    if (!hTooltip || !hControl || !text)
        return;

    TOOLINFOW ti = {};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd = GetParent(hControl);
    ti.uId = reinterpret_cast<UINT_PTR>(hControl);
    ti.lpszText = const_cast<LPWSTR>(text);
    SendMessageW(hTooltip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
}

// Custom-painted non-focus-stealing button subclass (prevents search box focus
// flicker on click)
static LRESULT CALLBACK SearchButtonSubclassProc(HWND hWnd,
                                                 UINT uMsg,
                                                 WPARAM wParam,
                                                 LPARAM lParam,
                                                 DWORD_PTR dwRefData) {
    HWND hListView = reinterpret_cast<HWND>(dwRefData);

    if (uMsg == WM_ERASEBKGND) {
        return 1;
    }

    // Handle mouse clicks without calling DefSubclassProc's SetFocus(hBtn)
    // so the search box never loses focus or flickers when clicking ↑ / ↓ / ✕
    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) {
        if (!IsWindowEnabled(hWnd))
            return 0;
        SetCapture(hWnd);
        SetPropW(hWnd, L"WhBtnDown", reinterpret_cast<HANDLE>(1));
        SetPropW(hWnd, L"WhBtnPushed", reinterpret_cast<HANDLE>(1));
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }

    if (uMsg == WM_MOUSEMOVE) {
        RECT rc = {};
        GetClientRect(hWnd, &rc);
        POINT pt = {static_cast<short>(LOWORD(lParam)),
                    static_cast<short>(HIWORD(lParam))};
        bool inside = PtInRect(&rc, pt) != FALSE;

        if (GetPropW(hWnd, L"WhBtnDown") != nullptr) {
            bool wasPushed = GetPropW(hWnd, L"WhBtnPushed") != nullptr;
            if (inside != wasPushed) {
                if (inside)
                    SetPropW(hWnd, L"WhBtnPushed", reinterpret_cast<HANDLE>(1));
                else
                    RemovePropW(hWnd, L"WhBtnPushed");
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        if (GetPropW(hWnd, L"WhBtnHot") == nullptr) {
            SetPropW(hWnd, L"WhBtnHot", reinterpret_cast<HANDLE>(1));
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
            InvalidateRect(hWnd, nullptr, FALSE);
        }
    } else if (uMsg == WM_LBUTTONUP) {
        if (GetPropW(hWnd, L"WhBtnDown") != nullptr) {
            bool wasPushed = GetPropW(hWnd, L"WhBtnPushed") != nullptr;
            RemovePropW(hWnd, L"WhBtnDown");
            RemovePropW(hWnd, L"WhBtnPushed");
            if (GetCapture() == hWnd) {
                ReleaseCapture();
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            if (wasPushed && IsWindowEnabled(hWnd)) {
                HWND hParent = GetParent(hWnd);
                if (hParent) {
                    int id = GetDlgCtrlID(hWnd);
                    SendMessageW(hParent, WM_COMMAND,
                                 MAKEWPARAM(id, BN_CLICKED),
                                 reinterpret_cast<LPARAM>(hWnd));
                }
            }
            return 0;
        }
    } else if (uMsg == WM_CAPTURECHANGED) {
        if (GetPropW(hWnd, L"WhBtnDown") != nullptr) {
            RemovePropW(hWnd, L"WhBtnDown");
            RemovePropW(hWnd, L"WhBtnPushed");
            InvalidateRect(hWnd, nullptr, FALSE);
        }
    } else if (uMsg == WM_MOUSELEAVE) {
        RemovePropW(hWnd, L"WhBtnHot");
        InvalidateRect(hWnd, nullptr, FALSE);
    } else if (uMsg == WM_ENABLE) {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        InvalidateRect(hWnd, nullptr, FALSE);
        return res;
    } else if (uMsg == WM_NCDESTROY) {
        RemovePropW(hWnd, L"WhBtnHot");
        RemovePropW(hWnd, L"WhBtnDown");
        RemovePropW(hWnd, L"WhBtnPushed");
    }

    if (uMsg == WM_PAINT) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state && !state->paletteInitialized) {
            UpdateSearchBarBrushes(state);
        }
        SearchBarPalette pal =
            (state && state->paletteInitialized)
                ? state->palette
                : ComputeThemePalette(IsWinRarDarkTheme(hListView));

        PAINTSTRUCT ps = {};
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc = {};
        GetClientRect(hWnd, &rc);

        HBRUSH hBarBrush = CreateSolidBrush(pal.barBg);
        FillRect(hdc, &rc, hBarBrush);
        DeleteObject(hBarBrush);

        bool isEnabled = IsWindowEnabled(hWnd) != FALSE;
        bool isHot = GetPropW(hWnd, L"WhBtnHot") != nullptr;
        bool isPressed = GetPropW(hWnd, L"WhBtnPushed") != nullptr;

        COLORREF fillClr = pal.btnBg;
        COLORREF borderClr = pal.btnBorder;
        if (isEnabled) {
            if (isPressed) {
                fillClr = pal.btnPressBg;
                borderClr = pal.btnHoverBorder;
            } else if (isHot) {
                fillClr = pal.btnHoverBg;
                borderClr = pal.btnHoverBorder;
            }
        }

        int radius = ScaleForDpi(hWnd, kCornerRadiusBase);
        HPEN hPen = CreatePen(PS_SOLID, 1, borderClr);
        HBRUSH hBtnBrush = CreateSolidBrush(fillClr);
        HGDIOBJ oldPen = SelectObject(hdc, hPen);
        HGDIOBJ oldBrush = SelectObject(hdc, hBtnBrush);

        RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(hBtnBrush);
        DeleteObject(hPen);

        WCHAR text[16] = {};
        GetWindowTextW(hWnd, text, ARRAYSIZE(text));

        HFONT hFont =
            state && state->hFont
                ? state->hFont
                : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        HGDIOBJ oldFont = SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, isEnabled ? pal.btnText : pal.btnDisabledText);

        DrawTextW(hdc, text, -1, &rc,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        SelectObject(hdc, oldFont);
        EndPaint(hWnd, &ps);
        return 0;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Deletes the previous word (or active selection) on Ctrl+Backspace in the
// search Edit control without flashing a selection highlight or inserting 0x7F
static void DeletePreviousWordInEdit(HWND hEdit) {
    DWORD selStart = 0;
    DWORD selEnd = 0;
    SendMessageW(hEdit, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart),
                 reinterpret_cast<LPARAM>(&selEnd));

    if (selStart != selEnd) {
        SendMessageW(hEdit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));
        return;
    }

    if (selStart == 0)
        return;

    WCHAR text[256] = {};
    int len = GetWindowTextW(hEdit, text, ARRAYSIZE(text));
    if (len <= 0)
        return;

    int pos = static_cast<int>(selStart);
    if (pos > len)
        pos = len;

    // 1. Skip any whitespace immediately preceding the caret
    while (pos > 0 && iswspace(text[pos - 1])) {
        --pos;
    }

    // 2. Delete either a contiguous run of alphanumeric characters or symbols
    if (pos > 0) {
        if (iswalnum(text[pos - 1])) {
            while (pos > 0 && iswalnum(text[pos - 1])) {
                --pos;
            }
        } else {
            while (pos > 0 && !iswalnum(text[pos - 1]) &&
                   !iswspace(text[pos - 1])) {
                --pos;
            }
        }
    }

    // Suppress intermediate selection painting so rapid Ctrl+Backspace never
    // flashes a selection highlight before deleting the word
    SendMessageW(hEdit, WM_SETREDRAW, FALSE, 0);
    SendMessageW(hEdit, EM_SETSEL, static_cast<WPARAM>(pos),
                 static_cast<LPARAM>(selStart));
    SendMessageW(hEdit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));
    SendMessageW(hEdit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hEdit, nullptr, TRUE);
}

// Subclass procedure for the search input Edit control (handles
// Up/Down/Enter/Esc/Ctrl+Backspace & placeholder)
static LRESULT CALLBACK SearchEditSubclassProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam,
                                               DWORD_PTR dwRefData) {
    HWND hListView = reinterpret_cast<HWND>(dwRefData);

    if (uMsg == WM_GETDLGCODE) {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam) | DLGC_WANTALLKEYS;
    }

    if (uMsg == WM_SETFOCUS || uMsg == WM_KILLFOCUS) {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state) {
            UpdateSearchBarBrushes(state);
        }
        InvalidateRect(hWnd, nullptr, FALSE);
        HWND hParent = GetParent(hWnd);
        if (hParent && state) {
            InvalidateRect(hParent, &state->rcEditFrame, FALSE);
        }
        return res;
    }

    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state) {
            bool ctrlDown = (GetKeyState(VK_CONTROL) < 0);
            bool altDown = (GetKeyState(VK_MENU) < 0);
            bool shiftDown = (GetKeyState(VK_SHIFT) < 0);

            if (wParam == 'A' && ctrlDown && !altDown && !shiftDown) {
                SendMessageW(hWnd, EM_SETSEL, 0, -1);
                return 0;
            }
            if (wParam == VK_BACK && ctrlDown && !altDown && !shiftDown) {
                DeletePreviousWordInEdit(hWnd);
                return 0;
            }
            if (wParam == VK_DOWN) {
                if (!ctrlDown && !altDown && !shiftDown) {
                    StepQuickFilterMatch(state, +1);
                }
                return 0;
            }
            if (wParam == VK_UP) {
                if (!ctrlDown && !altDown && !shiftDown) {
                    StepQuickFilterMatch(state, -1);
                }
                return 0;
            }
            if (wParam == VK_RETURN) {
                if (!ctrlDown && !altDown && !shiftDown) {
                    StepQuickFilterMatch(state, +1);
                } else if (!ctrlDown && !altDown && shiftDown) {
                    StepQuickFilterMatch(state, -1);
                } else if (!ctrlDown && altDown && !shiftDown) {
                    if (g_settings.search_alt_enter_select_all) {
                        SelectAllFilterMatches(state);
                    }
                }
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                if (!ctrlDown && !altDown && !shiftDown) {
                    HideOrClearSearchBar(state, false);
                }
                return 0;
            }
        }
    }

    if (uMsg == WM_CHAR || uMsg == WM_SYSCHAR) {
        if (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == 1 ||
            wParam == 0x7F) {
            return 0;
        }
    }

    if (uMsg == WM_PAINT) {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (GetWindowTextLengthW(hWnd) == 0 && GetFocus() != hWnd) {
            QuickSearchState* state = GetSearchStateForListView(hListView);
            if (state && !state->paletteInitialized) {
                UpdateSearchBarBrushes(state);
            }
            SearchBarPalette pal =
                (state && state->paletteInitialized)
                    ? state->palette
                    : ComputeThemePalette(IsWinRarDarkTheme(hListView));

            HDC hdc = GetDC(hWnd);
            if (hdc) {
                RECT rc = {};
                GetClientRect(hWnd, &rc);
                HFONT hFont =
                    state && state->hFont
                        ? state->hFont
                        : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
                HGDIOBJ oldFont = SelectObject(hdc, hFont);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, pal.cueText);
                DrawTextW(hdc, L"Filter files by name...", -1, &rc,
                          DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                SelectObject(hdc, oldFont);
                ReleaseDC(hWnd, hdc);
            }
        }
        return res;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Subclass procedure for the search bar container (handles clicks, commands,
// and double-buffered painting)
static LRESULT CALLBACK SearchBarContainerSubclassProc(HWND hWnd,
                                                       UINT uMsg,
                                                       WPARAM wParam,
                                                       LPARAM lParam,
                                                       DWORD_PTR dwRefData) {
    HWND hListView = reinterpret_cast<HWND>(dwRefData);

    // Clicking inside the Edit frame focuses Edit; clicking empty bar area or
    // match counter focuses the ListView
    if (uMsg == WM_LBUTTONDOWN) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state) {
            POINT pt = {static_cast<short>(LOWORD(lParam)),
                        static_cast<short>(HIWORD(lParam))};
            if (PtInRect(&state->rcEditFrame, pt)) {
                if (IsWindow(state->hEdit)) {
                    SetFocus(state->hEdit);
                }
            } else if (IsWindow(state->hListView)) {
                SetFocus(state->hListView);
            }
            return 0;
        }
    }

    if (uMsg == WM_COMMAND) {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        QuickSearchState* state = GetSearchStateForListView(hListView);

        if (state) {
            if (id == IDC_SEARCH_EDIT && code == EN_CHANGE) {
                ExecuteQuickFilter(state);
                return 0;
            }
            if (id == IDC_SEARCH_PREV && code == BN_CLICKED) {
                StepQuickFilterMatch(state, -1);
                return 0;
            }
            if (id == IDC_SEARCH_NEXT && code == BN_CLICKED) {
                StepQuickFilterMatch(state, +1);
                return 0;
            }
            if (id == IDC_SEARCH_CLOSE && code == BN_CLICKED) {
                HideOrClearSearchBar(state, false);
                return 0;
            }
        }
    }

    if (uMsg == WM_SIZE) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state) {
            LayoutSearchBarChildren(state, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, nullptr, FALSE);
        }
    }

    if (uMsg == WM_ERASEBKGND) {
        return 1;
    }

    if (uMsg == WM_PAINT) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state && !state->paletteInitialized) {
            UpdateSearchBarBrushes(state);
        }
        SearchBarPalette pal =
            (state && state->paletteInitialized)
                ? state->palette
                : ComputeThemePalette(IsWinRarDarkTheme(hListView));

        PAINTSTRUCT ps = {};
        HDC hdcPaint = BeginPaint(hWnd, &ps);
        RECT rc = {};
        GetClientRect(hWnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        // Double-buffer the bar background and edit frame so it never flickers
        HDC hdcMem = CreateCompatibleDC(hdcPaint);
        HBITMAP hbmMem =
            hdcMem ? CreateCompatibleBitmap(hdcPaint, width, height) : nullptr;
        HGDIOBJ oldBm = hbmMem ? SelectObject(hdcMem, hbmMem) : nullptr;
        HDC hdc = hbmMem ? hdcMem : hdcPaint;

        HBRUSH hBgBrush = CreateSolidBrush(pal.barBg);
        FillRect(hdc, &rc, hBgBrush);
        DeleteObject(hBgBrush);

        if (rc.bottom > rc.top) {
            HBRUSH hBorderBrush = CreateSolidBrush(pal.barBorder);
            RECT rcTop = {rc.left, rc.top, rc.right, rc.top + 1};
            FillRect(hdc, &rcTop, hBorderBrush);

            RECT rcBottom = {rc.left, rc.bottom - 1, rc.right, rc.bottom};
            FillRect(hdc, &rcBottom, hBorderBrush);
            DeleteObject(hBorderBrush);
        }

        if (state && state->rcEditFrame.right > state->rcEditFrame.left) {
            bool hasFocus = (GetFocus() == state->hEdit);
            COLORREF frameBorder =
                hasFocus ? pal.editFocusBorder : pal.editBorder;
            COLORREF frameBg = hasFocus ? pal.editFocusBg : pal.editBg;

            int radius = ScaleForDpi(hWnd, kCornerRadiusBase);
            HPEN hEditPen = CreatePen(PS_SOLID, 1, frameBorder);
            HBRUSH hEditFill = CreateSolidBrush(frameBg);
            HGDIOBJ oldPen = SelectObject(hdc, hEditPen);
            HGDIOBJ oldBrush = SelectObject(hdc, hEditFill);

            RoundRect(hdc, state->rcEditFrame.left, state->rcEditFrame.top,
                      state->rcEditFrame.right, state->rcEditFrame.bottom,
                      radius, radius);

            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(hEditFill);
            DeleteObject(hEditPen);
        }

        if (hbmMem) {
            BitBlt(hdcPaint, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, oldBm);
            DeleteObject(hbmMem);
        }
        if (hdcMem) {
            DeleteDC(hdcMem);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    if (uMsg == WM_CTLCOLOREDIT) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state) {
            UpdateSearchBarBrushes(state);
            const SearchBarPalette& pal = state->palette;
            bool hasFocus = (GetFocus() == state->hEdit);
            COLORREF targetEditBg = hasFocus ? pal.editFocusBg : pal.editBg;
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetBkMode(hdc, OPAQUE);
            SetBkColor(hdc, targetEditBg);
            SetTextColor(hdc, pal.editText);
            return reinterpret_cast<LRESULT>(state->hEditBgBrush);
        }
    }

    if (uMsg == WM_CTLCOLORSTATIC) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state) {
            UpdateSearchBarBrushes(state);
            const SearchBarPalette& pal = state->palette;
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetBkMode(hdc, OPAQUE);
            SetBkColor(hdc, pal.barBg);
            SetTextColor(hdc, pal.labelText);
            return reinterpret_cast<LRESULT>(state->hBarBgBrush);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Creates the search bar container, child controls, and tooltips on the UI
// thread
static void EnsureQuickSearchBarCreated(HWND hListView) {
    if (!IsMainWinRarListView(hListView))
        return;

    HWND hMainWnd = GetParent(hListView);
    if (!hMainWnd)
        return;

    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        auto it = g_searchStates.find(hListView);
        if (it != g_searchStates.end() && IsWindow(it->second.hBarContainer)) {
            return;
        }
    }

    bool addedShowSelAlways = false;
    LONG_PTR lvStyle = GetWindowLongPtrW(hListView, GWL_STYLE);
    if ((lvStyle & LVS_SHOWSELALWAYS) == 0) {
        SetWindowLongPtrW(hListView, GWL_STYLE, lvStyle | LVS_SHOWSELALWAYS);
        addedShowSelAlways = true;
    }

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    CreateWindowExW_t createWnd = pfnCreateWindowExW_Original
                                      ? pfnCreateWindowExW_Original
                                      : CreateWindowExW;

    int barHeight = ScaleForDpi(hMainWnd, kSearchBarBaseHeight);
    int ctrlHeight = ScaleForDpi(hMainWnd, kSearchCtrlBaseHeight);
    int btnWidth = ScaleForDpi(hMainWnd, kSearchBtnBaseWidth);

    // WS_CLIPCHILDREN prevents the bar container from painting over the Edit
    // control and buttons
    HWND hBar =
        createWnd(0, L"STATIC", nullptr,
                  WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_NOTIFY, 0,
                  0, 100, barHeight, hMainWnd, nullptr, hInst, nullptr);
    if (!hBar)
        return;

    HWND hEdit = createWnd(
        0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 0,
        0, 200, ctrlHeight, hBar,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SEARCH_EDIT)), hInst,
        nullptr);

    // Match counter label (no SS_NOTIFY so mouse clicks pass straight through
    // to the bar)
    HWND hCount = createWnd(
        0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 0,
        0, 80, ctrlHeight, hBar,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SEARCH_COUNT)), hInst,
        nullptr);

    HWND hPrev = createWnd(
        0, L"BUTTON", L"\x2191", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0,
        btnWidth, ctrlHeight, hBar,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SEARCH_PREV)), hInst,
        nullptr);

    HWND hNext = createWnd(
        0, L"BUTTON", L"\x2193", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0,
        btnWidth, ctrlHeight, hBar,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SEARCH_NEXT)), hInst,
        nullptr);

    HWND hClose = createWnd(
        0, L"BUTTON", L"\x2715", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0,
        btnWidth, ctrlHeight, hBar,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SEARCH_CLOSE)), hInst,
        nullptr);

    UINT currentDpi = GetWindowDpiSafe(hMainWnd);
    HFONT hFont = CreateSearchBarFontForDpi(currentDpi);

    if (hEdit) {
        SendMessageW(hEdit, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        SendMessageW(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                     MAKELPARAM(0, 0));
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hEdit, SearchEditSubclassProc,
            reinterpret_cast<DWORD_PTR>(hListView));
    }
    if (hCount) {
        SendMessageW(hCount, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    }
    if (hPrev) {
        SendMessageW(hPrev, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        EnableWindow(hPrev, FALSE);
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hPrev, SearchButtonSubclassProc,
            reinterpret_cast<DWORD_PTR>(hListView));
    }
    if (hNext) {
        SendMessageW(hNext, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        EnableWindow(hNext, FALSE);
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hNext, SearchButtonSubclassProc,
            reinterpret_cast<DWORD_PTR>(hListView));
    }
    if (hClose) {
        SendMessageW(hClose, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hClose, SearchButtonSubclassProc,
            reinterpret_cast<DWORD_PTR>(hListView));
    }

    HWND hTooltip = createWnd(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                              WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                              CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, hBar, nullptr, hInst, nullptr);
    if (hTooltip) {
        SetWindowPos(hTooltip, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        AddControlTooltip(
            hTooltip, hEdit,
            L"Filter files by name (Enter: Next, Shift+Enter: Previous)");
        AddControlTooltip(hTooltip, hPrev,
                          L"Previous Match (Shift+Enter / Up)");
        AddControlTooltip(hTooltip, hNext, L"Next Match (Enter / Down)");
        AddControlTooltip(hTooltip, hClose, L"Clear / Close Filter Bar (Esc)");
    }

    WindhawkUtils::SetWindowSubclassFromAnyThread(
        hBar, SearchBarContainerSubclassProc,
        reinterpret_cast<DWORD_PTR>(hListView));

    bool startVisible =
        g_settings.search_enabled && g_settings.search_always_visible;

    QuickSearchState state = {};
    state.hMainWnd = hMainWnd;
    state.hListView = hListView;
    state.hBarContainer = hBar;
    state.hEdit = hEdit;
    state.hCountLabel = hCount;
    state.hPrevBtn = hPrev;
    state.hNextBtn = hNext;
    state.hCloseBtn = hClose;
    state.hTooltip = hTooltip;
    state.hFont = hFont;
    state.fontDpi = currentDpi;
    state.isVisible = startVisible;
    state.addedShowSelAlways = addedShowSelAlways;

    RefreshSearchBarTheme(&state, true);

    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        g_searchStates[hListView] = state;
    }

    if (startVisible) {
        ShowWindow(hBar, SW_SHOWNOACTIVATE);
        QuickSearchState* pState = GetSearchStateForListView(hListView);
        TriggerMainLayoutRefresh(pState);
    }
}

// Cleans up the search bar controls on the ListView's UI thread
static void DestroyQuickSearchBar(HWND hListView, bool refreshLayout) {
    QuickSearchState stateCopy = {};
    bool found = false;

    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        auto it = g_searchStates.find(hListView);
        if (it != g_searchStates.end()) {
            stateCopy = it->second;
            g_searchStates.erase(it);
            found = true;
        }
    }

    if (!found)
        return;

    if (stateCopy.addedShowSelAlways && IsWindow(hListView)) {
        LONG_PTR lvStyle = GetWindowLongPtrW(hListView, GWL_STYLE);
        SetWindowLongPtrW(hListView, GWL_STYLE, lvStyle & ~LVS_SHOWSELALWAYS);
    }

    if (IsWindow(stateCopy.hTooltip)) {
        DestroyWindow(stateCopy.hTooltip);
    }
    if (IsWindow(stateCopy.hEdit)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            stateCopy.hEdit, SearchEditSubclassProc);
    }
    if (IsWindow(stateCopy.hPrevBtn)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            stateCopy.hPrevBtn, SearchButtonSubclassProc);
    }
    if (IsWindow(stateCopy.hNextBtn)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            stateCopy.hNextBtn, SearchButtonSubclassProc);
    }
    if (IsWindow(stateCopy.hCloseBtn)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            stateCopy.hCloseBtn, SearchButtonSubclassProc);
    }
    if (IsWindow(stateCopy.hBarContainer)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            stateCopy.hBarContainer, SearchBarContainerSubclassProc);
        DestroyWindow(stateCopy.hBarContainer);
    }
    if (stateCopy.hFont &&
        stateCopy.hFont != GetStockObject(DEFAULT_GUI_FONT)) {
        DeleteObject(stateCopy.hFont);
    }
    if (stateCopy.hEditBgBrush) {
        DeleteObject(stateCopy.hEditBgBrush);
    }
    if (stateCopy.hBarBgBrush) {
        DeleteObject(stateCopy.hBarBgBrush);
    }

    if (refreshLayout && IsWindow(stateCopy.hMainWnd)) {
        RECT rc = {};
        if (GetClientRect(stateCopy.hMainWnd, &rc)) {
            WPARAM sizeType =
                IsZoomed(stateCopy.hMainWnd) ? SIZE_MAXIMIZED : SIZE_RESTORED;
            SendMessageW(stateCopy.hMainWnd, WM_SIZE, sizeType,
                         MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
        }
    }
}

// Syncs search bar visibility on the ListView's own UI thread when settings
// change
static void SyncSearchBarSettingsStateOnUiThread(HWND hListView) {
    if (!IsMainWinRarListView(hListView))
        return;

    if (!g_settings.search_enabled) {
        QuickSearchState* state = GetSearchStateForListView(hListView);
        if (state && state->isVisible) {
            HideOrClearSearchBar(state, true);
        }
        return;
    }

    EnsureQuickSearchBarCreated(hListView);
    QuickSearchState* state = GetSearchStateForListView(hListView);
    if (!state)
        return;

    RefreshSearchBarTheme(state, true);

    if (g_settings.search_always_visible && !state->isVisible) {
        ShowSearchBar(state, false);
    } else if (!g_settings.search_always_visible && state->isVisible) {
        int textLen = state->hEdit ? GetWindowTextLengthW(state->hEdit) : 0;
        if (textLen == 0) {
            HideOrClearSearchBar(state, true);
        }
    }
}

// ---------------------------------------------------------------------------
// 3. Human-Readable File Size Logic (ListView & Status Bar, All Languages)
// ---------------------------------------------------------------------------

// Formats a raw byte value into Base-2 (1024) with KB/MB or KiB/MiB units
static void FormatBytesReadable(ULONGLONG bytes,
                                WCHAR* outBuf,
                                size_t outSize) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;

    if (bytes < 1024ULL) {
        swprintf_s(outBuf, outSize, L"%llu B", bytes);
        return;
    }

    bool useIec = g_settings.use_iec_terms.load();
    const WCHAR* unitK = useIec ? L"KiB" : L"KB";
    const WCHAR* unitM = useIec ? L"MiB" : L"MB";
    const WCHAR* unitG = useIec ? L"GiB" : L"GB";
    const WCHAR* unitT = useIec ? L"TiB" : L"TB";

    double dBytes = static_cast<double>(bytes);

    if (dBytes >= GB * 1023.995) {
        swprintf_s(outBuf, outSize, L"%.2f %s", dBytes / TB, unitT);
    } else if (dBytes >= MB * 1023.995) {
        swprintf_s(outBuf, outSize, L"%.2f %s", dBytes / GB, unitG);
    } else if (dBytes >= KB * 1023.995) {
        swprintf_s(outBuf, outSize, L"%.2f %s", dBytes / MB, unitM);
    } else {
        swprintf_s(outBuf, outSize, L"%.2f %s", dBytes / KB, unitK);
    }
}

// Converts an already-formatted cell between KB/MB/GB/TB and KiB/MiB/GiB/TiB
static bool ConvertFormattedUnitLabels(const WCHAR* str,
                                       WCHAR* outBuf,
                                       size_t outSize) {
    if (!str || !*str)
        return false;

    const WCHAR* space = wcsrchr(str, L' ');
    if (!space || space == str)
        return false;

    for (const WCHAR* p = str; p < space; ++p) {
        if ((*p < L'0' || *p > L'9') && *p != L'.')
            return false;
    }

    const WCHAR* unit = space + 1;
    const WCHAR* newUnit = nullptr;

    if (g_settings.use_iec_terms) {
        if (wcscmp(unit, L"KB") == 0)
            newUnit = L"KiB";
        else if (wcscmp(unit, L"MB") == 0)
            newUnit = L"MiB";
        else if (wcscmp(unit, L"GB") == 0)
            newUnit = L"GiB";
        else if (wcscmp(unit, L"TB") == 0)
            newUnit = L"TiB";
    } else {
        if (wcscmp(unit, L"KiB") == 0)
            newUnit = L"KB";
        else if (wcscmp(unit, L"MiB") == 0)
            newUnit = L"MB";
        else if (wcscmp(unit, L"GiB") == 0)
            newUnit = L"GB";
        else if (wcscmp(unit, L"TiB") == 0)
            newUnit = L"TB";
    }

    if (!newUnit)
        return false;

    int numLen = static_cast<int>(space - str);
    swprintf_s(outBuf, outSize, L"%.*s %s", numLen, str, newUnit);
    return true;
}

// Checks if the ListView column header is CRC32 to avoid formatting 8-digit hex
static bool IsCrcColumn(HWND hListView, int subItem) {
    if (subItem <= 0)
        return false;

    HWND hHeader = ListView_GetHeader(hListView);
    if (!hHeader)
        return false;

    WCHAR textBuf[64] = {};
    HDITEMW hdi = {};
    hdi.mask = HDI_TEXT;
    hdi.pszText = textBuf;
    hdi.cchTextMax = ARRAYSIZE(textBuf);

    if (!SendMessageW(hHeader, HDM_GETITEMW, static_cast<WPARAM>(subItem),
                      reinterpret_cast<LPARAM>(&hdi))) {
        return false;
    }

    return ContainsCaseInsensitive(textBuf, L"CRC");
}

// Excludes the Name column (subItem 0) and CRC32 column across all languages
static bool IsTargetSizeColumn(HWND hListView, int subItem) {
    if (subItem <= 0)
        return false;

    return !IsCrcColumn(hListView, subItem);
}

// Parses a locale-formatted byte string while rejecting decimals, dates,
// ratios, and CRC
static bool ParseNumericBytes(const WCHAR* str, ULONGLONG* outBytes) {
    if (!str || str == LPSTR_TEXTCALLBACKW || !*str || !outBytes)
        return false;

    ULONGLONG val = 0;
    int digitCount = 0;
    int sepCount = 0;
    int digitsSinceLastSep = 0;
    bool hadSep = false;

    for (const WCHAR* p = str; *p; ++p) {
        WCHAR ch = *p;

        if (iswspace(ch) || ch == L',' || ch == L'.' || ch == L'\'' ||
            ch == 0x00A0 || ch == 0x202F || ch == 0x200E || ch == 0x200F) {
            if (ch == L'.' || ch == L',') {
                if (digitCount == 0 || (hadSep && digitsSinceLastSep != 3))
                    return false;
                hadSep = true;
                digitsSinceLastSep = 0;
            }
            sepCount++;
            continue;
        }

        if (ch >= L'0' && ch <= L'9') {
            ULONGLONG digit = static_cast<ULONGLONG>(ch - L'0');
            if (val > (ULLONG_MAX - digit) / 10ULL)
                return false;
            val = (val * 10ULL) + digit;
            digitCount++;
            digitsSinceLastSep++;
        } else {
            return false;
        }
    }

    if (digitCount == 0)
        return false;

    // Thousand separators must be followed by 3 digits (rejects decimals like
    // "1.50")
    if (hadSep && digitsSinceLastSep != 3)
        return false;

    // Reject ungrouped 8-digit numbers or leading-zero numbers (CRC32 hashes)
    if (sepCount == 0 &&
        (digitCount == 8 || (digitCount > 1 && str[0] == L'0'))) {
        return false;
    }

    *outBytes = val;
    return true;
}

// Formats raw byte counts in WinRAR status bar summary strings across all
// languages (e.g. "Selected 1 file, 766,232 bytes" -> "Selected 1 file, 748.27
// KB" and "Total 1 folder, 34 files, 10,746,168 bytes" -> "Total 1 folder, 34
// files, 10.25 MB") while leaving folder-only text ("Selected 1 folder") intact
static bool FormatStatusBarSummaryText(const WCHAR* text,
                                       WCHAR* outBuf,
                                       size_t outSize) {
    if (!text || !*text || !outBuf || outSize == 0)
        return false;

    // Find the last clause separator (e.g. ", " or localized comma) that
    // separates the file/folder count from the trailing byte count clause
    const WCHAR* lastClause = nullptr;
    for (const WCHAR* p = text; *p; ++p) {
        if ((*p == L',' || *p == L'\x060C' || *p == L'\x3001' ||
             *p == L'\xFF0C') &&
            (p[1] == L' ' || p[1] == 0x00A0)) {
            lastClause = p + 1;
            while (*lastClause == L' ' || *lastClause == 0x00A0) {
                ++lastClause;
            }
        }
    }

    if (!lastClause || !*lastClause)
        return false;

    // Locate the first digit inside the final clause
    const WCHAR* numStart = lastClause;
    while (*numStart && (*numStart < L'0' || *numStart > L'9')) {
        if (*numStart == L':' || *numStart == L'/' || *numStart == L'-')
            return false;
        ++numStart;
    }

    if (!*numStart)
        return false;

    // Scan through the grouped byte number (digits + thousand separators)
    const WCHAR* p = numStart;
    const WCHAR* numEnd = numStart;
    while (*p) {
        WCHAR ch = *p;
        if (ch >= L'0' && ch <= L'9') {
            ++p;
            numEnd = p;
        } else if ((ch == L',' || ch == L'.' || ch == L'\'' || ch == L' ' ||
                    ch == 0x00A0 || ch == 0x202F || ch == 0x200E ||
                    ch == 0x200F) &&
                   (p[1] >= L'0' && p[1] <= L'9')) {
            ++p;
        } else {
            break;
        }
    }

    if (numEnd <= numStart)
        return false;

    // After the byte number, there must be a localized unit word (e.g. "bytes",
    // "octets", "байт", "バイト", "字节") with no other digits following it
    const WCHAR* unitStart = numEnd;
    while (*unitStart == L' ' || *unitStart == 0x00A0 || *unitStart == 0x202F) {
        ++unitStart;
    }

    if (!*unitStart)
        return false;

    const WCHAR* unitEnd = unitStart;
    while (*unitEnd && !iswspace(*unitEnd) && *unitEnd != L',' &&
           *unitEnd != L';' && *unitEnd != L')') {
        if (*unitEnd >= L'0' && *unitEnd <= L'9')
            return false;
        ++unitEnd;
    }

    if (unitEnd <= unitStart)
        return false;

    size_t numLen = static_cast<size_t>(numEnd - numStart);
    if (numLen >= 64)
        return false;

    WCHAR numBuf[64] = {};
    wcsncpy_s(numBuf, numStart, numLen);

    ULONGLONG bytes = 0;
    if (!ParseNumericBytes(numBuf, &bytes))
        return false;

    WCHAR readable[64] = {};
    FormatBytesReadable(bytes, readable, ARRAYSIZE(readable));

    int prefixLen = static_cast<int>(numStart - text);
    swprintf_s(outBuf, outSize, L"%.*s%s%s", prefixLen, text, readable,
               unitEnd);
    return true;
}

// Formats raw byte cells or updates unit labels in an already-open ListView on
// the UI thread
static void FormatExistingListViewItems(HWND hListView) {
    if (!g_settings.readable_file_sizes || !IsMainWinRarListView(hListView))
        return;

    if (GetWindowLongPtrW(hListView, GWL_STYLE) & LVS_OWNERDATA)
        return;

    HWND hHeader = ListView_GetHeader(hListView);
    if (!hHeader)
        return;

    int colCount =
        static_cast<int>(SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0));
    int itemCount = ListView_GetItemCount(hListView);
    if (colCount <= 1 || itemCount <= 0)
        return;

    for (int subItem = 1; subItem < colCount; ++subItem) {
        if (!IsTargetSizeColumn(hListView, subItem))
            continue;

        for (int i = 0; i < itemCount; ++i) {
            WCHAR rawText[64] = {};
            LVITEMW lvi = {};
            lvi.iSubItem = subItem;
            lvi.pszText = rawText;
            lvi.cchTextMax = ARRAYSIZE(rawText);

            if (SendMessageW(hListView, LVM_GETITEMTEXTW,
                             static_cast<WPARAM>(i),
                             reinterpret_cast<LPARAM>(&lvi)) <= 0) {
                continue;
            }

            WCHAR formatted[64] = {};
            ULONGLONG bytes = 0;
            if (ParseNumericBytes(rawText, &bytes)) {
                FormatBytesReadable(bytes, formatted, ARRAYSIZE(formatted));
                lvi.pszText = formatted;
                SendMessageW(hListView, LVM_SETITEMTEXTW,
                             static_cast<WPARAM>(i),
                             reinterpret_cast<LPARAM>(&lvi));
            } else if (ConvertFormattedUnitLabels(rawText, formatted,
                                                  ARRAYSIZE(formatted))) {
                lvi.pszText = formatted;
                SendMessageW(hListView, LVM_SETITEMTEXTW,
                             static_cast<WPARAM>(i),
                             reinterpret_cast<LPARAM>(&lvi));
            }
        }
    }
}

// Re-evaluates all status bar parts on the UI thread when settings change or
// when attaching to an already-open WinRAR window
static void RefreshStatusBarItems(HWND hStatusBar) {
    if (!IsChildOfMainWinRarWindow(hStatusBar))
        return;

    int partCount =
        static_cast<int>(SendMessageW(hStatusBar, SB_GETPARTS, 0, 0));
    if (partCount <= 0)
        partCount = 1;
    if (partCount > 8)
        partCount = 8;

    for (int i = 0; i < partCount; ++i) {
        std::wstring rawText;
        WPARAM flags = static_cast<WPARAM>(i);
        bool hasCached = false;

        {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            auto it = g_statusBarStates.find(hStatusBar);
            if (it != g_statusBarStates.end() && it->second.hasPart[i]) {
                rawText = it->second.rawPartText[i];
                flags = it->second.rawPartFlags[i];
                hasCached = true;
            }
        }

        if (!hasCached) {
            LRESULT lenRes = SendMessageW(hStatusBar, SB_GETTEXTLENGTHW, i, 0);
            int len = LOWORD(lenRes);
            int type = HIWORD(lenRes);
            if ((type & SBT_OWNERDRAW) != 0 || len <= 0 || len >= 512)
                continue;

            WCHAR currentText[512] = {};
            SendMessageW(hStatusBar, SB_GETTEXTW, i,
                         reinterpret_cast<LPARAM>(currentText));
            rawText = currentText;
            flags = static_cast<WPARAM>(i | type);

            std::lock_guard<std::mutex> lock(g_listViewMutex);
            StatusBarState& st = g_statusBarStates[hStatusBar];
            st.rawPartText[i] = rawText;
            st.rawPartFlags[i] = flags;
            st.hasPart[i] = true;
        }

        const WCHAR* targetText = rawText.c_str();
        WCHAR formatted[512] = {};
        if (g_settings.readable_file_sizes &&
            FormatStatusBarSummaryText(rawText.c_str(), formatted,
                                       ARRAYSIZE(formatted))) {
            targetText = formatted;
        }

        {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            g_statusBarStates[hStatusBar].isInternalUpdate = true;
        }
        SendMessageW(hStatusBar, SB_SETTEXTW, flags,
                     reinterpret_cast<LPARAM>(targetText));
        {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            g_statusBarStates[hStatusBar].isInternalUpdate = false;
        }
    }
}

// Intercepts SB_SETTEXTW on WinRAR's main status bar to format byte counts
static LRESULT CALLBACK StatusBarSubclassProc(HWND hWnd,
                                              UINT uMsg,
                                              WPARAM wParam,
                                              LPARAM lParam,
                                              DWORD_PTR /*dwRefData*/) {
    if (g_msgSettingsChanged != 0 && uMsg == g_msgSettingsChanged) {
        RefreshStatusBarItems(hWnd);
        return 0;
    }

    if (uMsg == SB_SETTEXTW && lParam != 0 && (wParam & SBT_OWNERDRAW) == 0 &&
        IsChildOfMainWinRarWindow(hWnd)) {
        int part = static_cast<int>(wParam & 0xFF);
        bool isInternal = false;

        {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            auto it = g_statusBarStates.find(hWnd);
            if (it != g_statusBarStates.end()) {
                isInternal = it->second.isInternalUpdate;
            }
            if (!isInternal && part >= 0 && part < 8) {
                StatusBarState& st = g_statusBarStates[hWnd];
                st.rawPartText[part] = reinterpret_cast<LPCWSTR>(lParam);
                st.rawPartFlags[part] = wParam;
                st.hasPart[part] = true;
            }
        }

        if (!isInternal && g_settings.readable_file_sizes) {
            const WCHAR* rawText = reinterpret_cast<LPCWSTR>(lParam);
            WCHAR formatted[512] = {};
            if (FormatStatusBarSummaryText(rawText, formatted,
                                           ARRAYSIZE(formatted))) {
                return DefSubclassProc(hWnd, uMsg, wParam,
                                       reinterpret_cast<LPARAM>(formatted));
            }
        }
    }

    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        g_subclassedStatusBars.erase(hWnd);
        g_statusBarStates.erase(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Attaches our subclass procedure to a msctls_statusbar32 control safely
static void AttachStatusBarSubclass(HWND hWnd, bool deferRefreshToUiThread) {
    if (!hWnd || !IsWindow(hWnd))
        return;

    WCHAR cls[64] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, STATUSCLASSNAMEW) != 0) {
        return;
    }

    bool alreadySubclassed = false;
    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        alreadySubclassed =
            (g_subclassedStatusBars.find(hWnd) != g_subclassedStatusBars.end());
    }

    if (!alreadySubclassed) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd, StatusBarSubclassProc, 0)) {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            g_subclassedStatusBars.insert(hWnd);
            alreadySubclassed = true;
        }
    }

    if (!alreadySubclassed)
        return;

    DWORD wndThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!deferRefreshToUiThread && wndThreadId == GetCurrentThreadId()) {
        RefreshStatusBarItems(hWnd);
    } else if (g_msgSettingsChanged != 0) {
        PostMessageW(hWnd, g_msgSettingsChanged, 0, 0);
    }
}

// Intercepts ListView sizing, theme changes, async settings updates, sorting,
// typing, and item insertions
static LRESULT CALLBACK ListViewSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             DWORD_PTR /*dwRefData*/) {
    // Handle thread-safe settings reload and initial attach formatting on the
    // UI thread
    if (g_msgSettingsChanged != 0 && uMsg == g_msgSettingsChanged) {
        FormatExistingListViewItems(hWnd);
        SyncSearchBarSettingsStateOnUiThread(hWnd);
        return 0;
    }

    // Handle thread-safe search bar cleanup on the UI thread before mod unload
    if (g_msgCleanupSearchBar != 0 && uMsg == g_msgCleanupSearchBar) {
        DestroyQuickSearchBar(hWnd, true);
        return 0;
    }

    // 1. Intercept ListView geometry changes to dock the Quick Filter Bar
    // cleanly at the bottom
    if (uMsg == WM_WINDOWPOSCHANGING && lParam != 0) {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if ((wp->flags & SWP_NOSIZE) == 0 || (wp->flags & SWP_NOMOVE) == 0) {
            QuickSearchState* state = GetSearchStateForListView(hWnd);
            if (state && state->isVisible && !state->isAdjustingLayout &&
                IsWindow(state->hBarContainer)) {
                RECT rcCurrent = {};
                if (((wp->flags & SWP_NOMOVE) != 0 ||
                     (wp->flags & SWP_NOSIZE) != 0) &&
                    GetWindowRect(hWnd, &rcCurrent)) {
                    MapWindowPoints(HWND_DESKTOP, GetParent(hWnd),
                                    reinterpret_cast<LPPOINT>(&rcCurrent), 2);
                }

                int targetX =
                    ((wp->flags & SWP_NOMOVE) == 0) ? wp->x : rcCurrent.left;
                int targetY =
                    ((wp->flags & SWP_NOMOVE) == 0) ? wp->y : rcCurrent.top;
                int targetCx = ((wp->flags & SWP_NOSIZE) == 0)
                                   ? wp->cx
                                   : (rcCurrent.right - rcCurrent.left);
                int targetCy = ((wp->flags & SWP_NOSIZE) == 0)
                                   ? wp->cy
                                   : (rcCurrent.bottom - rcCurrent.top);

                int barHeight = ScaleForDpi(hWnd, kSearchBarBaseHeight);
                if (targetCy > barHeight * 2) {
                    state->isAdjustingLayout = true;
                    if ((wp->flags & SWP_NOSIZE) == 0) {
                        wp->cy -= barHeight;
                        targetCy = wp->cy;
                    }
                    SetWindowPos(state->hBarContainer, HWND_TOP, targetX,
                                 targetY + targetCy, targetCx, barHeight,
                                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    state->isAdjustingLayout = false;
                }
            }
        }
    }

    // 2. Automatically update search bar theme when WinRAR or Windows changes
    // Dark/Light mode
    if (uMsg == LVM_SETBKCOLOR || uMsg == LVM_SETTEXTCOLOR ||
        uMsg == WM_SYSCOLORCHANGE || uMsg == WM_SETTINGCHANGE ||
        uMsg == WM_THEMECHANGED) {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        QuickSearchState* state = GetSearchStateForListView(hWnd);
        if (state) {
            RefreshSearchBarTheme(state, true);
        }
        return res;
    }

    // 3. Re-evaluate filter matches when the user sorts columns so indices stay
    // accurate
    if (uMsg == LVM_SORTITEMS || uMsg == LVM_SORTITEMSEX) {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        QuickSearchState* state = GetSearchStateForListView(hWnd);
        if (state && IsWindow(state->hEdit) &&
            GetWindowTextLengthW(state->hEdit) > 0) {
            ExecuteQuickFilter(state);
        }
        return res;
    }

    // 4. Clear search text and matches when navigating to a different folder
    if (uMsg == LVM_DELETEALLITEMS) {
        QuickSearchState* state = GetSearchStateForListView(hWnd);
        if (state) {
            state->matchedIndices.clear();
            state->currentMatchPos = -1;
            state->multiSelected = false;
            if (IsWindow(state->hEdit) &&
                GetWindowTextLengthW(state->hEdit) > 0) {
                state->suppressTextChange = true;
                SetWindowTextW(state->hEdit, L"");
                state->suppressTextChange = false;
            }
            UpdateMatchCounterLabel(state, false);
        }
    }

    // 5. Type-to-Search: intercept printable characters (including
    // Shift+letter)
    if (uMsg == WM_CHAR && g_settings.search_enabled &&
        g_settings.search_type_to_search) {
        WCHAR ch = static_cast<WCHAR>(wParam);
        if (ch >= 0x20 && ch != 0x7F && GetKeyState(VK_CONTROL) >= 0 &&
            GetKeyState(VK_MENU) >= 0) {
            QuickSearchState* state = GetSearchStateForListView(hWnd);
            if (state && IsWindow(state->hEdit)) {
                if (!state->isVisible) {
                    ShowSearchBar(state, false);
                }

                WCHAR existing[256] = {};
                GetWindowTextW(state->hEdit, existing, ARRAYSIZE(existing));
                size_t len = wcslen(existing);
                if (len + 1 < ARRAYSIZE(existing)) {
                    existing[len] = ch;
                    existing[len + 1] = L'\0';
                    SetWindowTextW(state->hEdit, existing);
                }

                SetFocus(state->hEdit);
                int newLen = GetWindowTextLengthW(state->hEdit);
                SendMessageW(state->hEdit, EM_SETSEL, newLen, newLen);
                return 0;
            }
        }
    }

    // 6. Format raw byte counts inline into KB/MB/GB/TB on the main file list
    if (g_settings.readable_file_sizes && lParam != 0) {
        if (uMsg == LVM_SETITEMTEXTW || uMsg == LVM_SETITEMW ||
            uMsg == LVM_INSERTITEMW) {
            LVITEMW* pItem = reinterpret_cast<LVITEMW*>(lParam);
            bool hasText =
                (uMsg == LVM_SETITEMTEXTW) || ((pItem->mask & LVIF_TEXT) != 0);

            if (hasText && pItem->iSubItem > 0 && pItem->pszText &&
                pItem->pszText != LPSTR_TEXTCALLBACKW &&
                IsMainWinRarListView(hWnd)) {
                ULONGLONG byteCount = 0;
                if (ParseNumericBytes(pItem->pszText, &byteCount) &&
                    IsTargetSizeColumn(hWnd, pItem->iSubItem)) {
                    WCHAR szFormatted[64] = {};
                    FormatBytesReadable(byteCount, szFormatted,
                                        ARRAYSIZE(szFormatted));

                    LPWSTR pOld = pItem->pszText;
                    pItem->pszText = szFormatted;
                    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    pItem->pszText = pOld;
                    return res;
                }
            }
        }
    }

    // 7. Clean up state when the ListView is destroyed
    if (uMsg == WM_NCDESTROY) {
        DestroyQuickSearchBar(hWnd, false);
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        g_subclassedListViews.erase(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Attaches our subclass procedure to a SysListView32 control safely from any
// thread
static void AttachListViewSubclass(HWND hWnd, bool deferRefreshToUiThread) {
    if (!hWnd || !IsWindow(hWnd))
        return;

    WCHAR cls[64] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, WC_LISTVIEWW) != 0) {
        return;
    }

    bool alreadySubclassed = false;
    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        alreadySubclassed =
            (g_subclassedListViews.find(hWnd) != g_subclassedListViews.end());
    }

    if (!alreadySubclassed) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd, ListViewSubclassProc, 0)) {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            g_subclassedListViews.insert(hWnd);
            alreadySubclassed = true;
        }
    }

    if (!alreadySubclassed)
        return;

    DWORD wndThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!deferRefreshToUiThread && wndThreadId == GetCurrentThreadId()) {
        if (g_settings.search_enabled) {
            EnsureQuickSearchBarCreated(hWnd);
        }
    } else if (g_msgSettingsChanged != 0) {
        PostMessageW(hWnd, g_msgSettingsChanged, 0, 0);
    }
}

// ---------------------------------------------------------------------------
// 4. Zero-Flicker Evaluation Popup Blocker
// ---------------------------------------------------------------------------

// Checks if a dialog template name matches WinRAR's trial reminder resources
static bool IsEvaluationTemplateName(LPCWSTR lpTemplateName) {
    if (!lpTemplateName || IS_INTRESOURCE(lpTemplateName))
        return false;

    return (_wcsicmp(lpTemplateName, L"REMINDER") == 0 ||
            _wcsicmp(lpTemplateName, L"RARREMINDER") == 0 ||
            _wcsicmp(lpTemplateName, L"WEBREMINDER") == 0 ||
            _wcsicmp(lpTemplateName, L"RARWEBREMINDER") == 0);
}

// Matches only the classic license dialog title or the web expired popup title
static bool IsEvaluationWindowTitle(const WCHAR* title) {
    if (!title || !*title)
        return false;

    if (ContainsCaseInsensitive(title, L"Expired Notification") &&
        ContainsCaseInsensitive(title, L"free trial period has ended")) {
        return true;
    }

    if (_wcsicmp(title, L"Please purchase WinRAR license") == 0) {
        return true;
    }

    return false;
}

// Verifies the trial dialog by checking its reminder-specific child controls
static bool HasEvaluationDialogSignature(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg))
        return false;

    bool hasBuyOnlineBtn = false;
    bool hasHowToBuyBtn = false;
    bool hasTrialNote = false;

    for (HWND hChild = GetWindow(hDlg, GW_CHILD); hChild != nullptr;
         hChild = GetWindow(hChild, GW_HWNDNEXT)) {
        WCHAR text[256] = {};
        if (GetWindowTextW(hChild, text, ARRAYSIZE(text)) <= 0)
            continue;

        if (_wcsicmp(text, L"Buy online") == 0) {
            hasBuyOnlineBtn = true;
        } else if (_wcsicmp(text, L"How to buy") == 0) {
            hasHowToBuyBtn = true;
        } else if (ContainsCaseInsensitive(text, L"40 day trial period") ||
                   ContainsCaseInsensitive(text,
                                           L"WinRAR is not free software")) {
            hasTrialNote = true;
        }
    }

    return (hasBuyOnlineBtn && hasHowToBuyBtn) ||
           (hasBuyOnlineBtn && hasTrialNote);
}

// Determines if a top-level window is one of the two evaluation popups
static bool IsEvaluationPopupWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd))
        return false;

    if ((GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD) != 0)
        return false;

    WCHAR title[256] = {};
    if (GetWindowTextW(hWnd, title, ARRAYSIZE(title)) > 0) {
        if (IsEvaluationWindowTitle(title))
            return true;
    }

    WCHAR cls[64] = {};
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"RarReminder") == 0 ||
            _wcsicmp(cls, L"RarWebReminder") == 0) {
            return true;
        }

        if (_wcsicmp(cls, L"#32770") == 0 &&
            HasEvaluationDialogSignature(hWnd)) {
            return true;
        }
    }

    return false;
}

// Checks whether a window creation call is creating an explicit reminder class
static bool IsReminderWindowClass(LPCWSTR lpClassName, DWORD dwStyle) {
    if ((dwStyle & WS_CHILD) != 0 || !lpClassName)
        return false;

    WCHAR clsName[128] = {};
    if (IS_INTRESOURCE(lpClassName)) {
        GlobalGetAtomNameW(LOWORD(reinterpret_cast<ULONG_PTR>(lpClassName)),
                           clsName, ARRAYSIZE(clsName));
    } else {
        wcsncpy_s(clsName, lpClassName, _TRUNCATE);
    }

    return (_wcsicmp(clsName, L"RarReminder") == 0 ||
            _wcsicmp(clsName, L"RarWebReminder") == 0);
}

// Thread-local pointer to pass the original DLGPROC into our wrapper
static thread_local DLGPROC g_tlsOriginalDlgProc = nullptr;

// Intercepts WM_INITDIALOG before Windows makes the modal dialog visible,
// then restores WinRAR's original dialog procedure so no mod code stays
// attached
static INT_PTR CALLBACK ModalDialogInterceptorProc(HWND hDlg,
                                                   UINT uMsg,
                                                   WPARAM wParam,
                                                   LPARAM lParam) {
    DLGPROC origProc =
        reinterpret_cast<DLGPROC>(GetPropW(hDlg, L"WhOrigDlgProc"));
    if (!origProc && g_tlsOriginalDlgProc) {
        origProc = g_tlsOriginalDlgProc;
        SetPropW(hDlg, L"WhOrigDlgProc", reinterpret_cast<HANDLE>(origProc));
    }

    INT_PTR res = origProc ? origProc(hDlg, uMsg, wParam, lParam) : FALSE;

    if (uMsg == WM_INITDIALOG) {
        if (g_settings.auto_dismiss_eval_dialog &&
            IsEvaluationPopupWindow(hDlg)) {
            Wh_Log(L"Aborted evaluation dialog inside WM_INITDIALOG");
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        // Not the reminder: restore WinRAR's own dialog procedure so unloading
        // the mod while this modal dialog is open cannot crash WinRAR.
        if (origProc) {
            SetWindowLongPtrW(hDlg, DWLP_DLGPROC,
                              reinterpret_cast<LONG_PTR>(origProc));
        }
        RemovePropW(hDlg, L"WhOrigDlgProc");
    } else if (uMsg == WM_NCDESTROY) {
        RemovePropW(hDlg, L"WhOrigDlgProc");
    }

    return res;
}

using DialogBoxParamW_t = decltype(&DialogBoxParamW);
static DialogBoxParamW_t pfnDialogBoxParamW_Original = nullptr;

// Hooks modal dialog creation to block evaluation dialogs before they render
INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    if (g_settings.auto_dismiss_eval_dialog) {
        if (IsEvaluationTemplateName(lpTemplateName)) {
            Wh_Log(L"Blocked modal evaluation dialog template: %s",
                   lpTemplateName);
            return IDCANCEL;
        }

        if (lpDialogFunc) {
            DLGPROC prevProc = g_tlsOriginalDlgProc;
            g_tlsOriginalDlgProc = lpDialogFunc;
            INT_PTR ret = pfnDialogBoxParamW_Original(
                hInstance, lpTemplateName, hWndParent,
                ModalDialogInterceptorProc, dwInitParam);
            g_tlsOriginalDlgProc = prevProc;
            return ret;
        }
    }

    return pfnDialogBoxParamW_Original(hInstance, lpTemplateName, hWndParent,
                                       lpDialogFunc, dwInitParam);
}

using CreateDialogParamW_t = decltype(&CreateDialogParamW);
static CreateDialogParamW_t pfnCreateDialogParamW_Original = nullptr;

// Blocks modeless evaluation dialogs before they can be shown
HWND WINAPI CreateDialogParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    if (g_settings.auto_dismiss_eval_dialog &&
        IsEvaluationTemplateName(lpTemplateName)) {
        Wh_Log(L"Blocked modeless evaluation dialog template: %s",
               lpTemplateName);
        return nullptr;
    }

    HWND hDlg = pfnCreateDialogParamW_Original(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);

    if (hDlg && g_settings.auto_dismiss_eval_dialog &&
        IsEvaluationPopupWindow(hDlg)) {
        Wh_Log(L"Destroying evaluation dialog before render");
        DestroyWindow(hDlg);
        return nullptr;
    }

    return hDlg;
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
static SetWindowTextW_t pfnSetWindowTextW_Original = nullptr;

// Catches evaluation dialogs when WinRAR sets their title dynamically
BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    BOOL res = pfnSetWindowTextW_Original(hWnd, lpString);

    if (g_settings.auto_dismiss_eval_dialog && hWnd && lpString &&
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD) == 0 &&
        IsEvaluationWindowTitle(lpString)) {
        Wh_Log(L"Dismissing evaluation popup on title set: %s", lpString);
        ShowWindow(hWnd, SW_HIDE);
        if (!EndDialog(hWnd, IDCANCEL)) {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
    }

    return res;
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t pfnShowWindow_Original = nullptr;

// Prevents any remaining evaluation popup from becoming visible on screen
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_settings.auto_dismiss_eval_dialog && nCmdShow != SW_HIDE && hWnd &&
        IsEvaluationPopupWindow(hWnd)) {
        Wh_Log(L"Prevented evaluation popup from showing");
        if (!EndDialog(hWnd, IDCANCEL)) {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        return FALSE;
    }

    return pfnShowWindow_Original(hWnd, nCmdShow);
}

// ---------------------------------------------------------------------------
// 5. Window Creation Hook & Windhawk Lifecycle
// ---------------------------------------------------------------------------

// Hooks window creation to block evaluation windows and subclass new ListViews
// and StatusBars
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    if (g_settings.auto_dismiss_eval_dialog && !(dwStyle & WS_CHILD)) {
        if ((lpWindowName && IsEvaluationWindowTitle(lpWindowName)) ||
            IsReminderWindowClass(lpClassName, dwStyle)) {
            Wh_Log(L"Blocked evaluation window creation in CreateWindowExW");
            return nullptr;
        }
    }

    HWND hWnd = pfnCreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hWnd) {
        AttachListViewSubclass(hWnd, false);
        AttachStatusBarSubclass(hWnd, false);
    }

    return hWnd;
}

// Enumerates child windows to find and subclass existing SysListView32 and
// msctls_statusbar32 controls
static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM /*lParam*/) {
    AttachListViewSubclass(hWnd, true);
    AttachStatusBarSubclass(hWnd, true);
    return TRUE;
}

// Enumerates top-level windows belonging to the current WinRAR process
static BOOL CALLBACK EnumTopWindowsProc(HWND hWnd, LPARAM /*lParam*/) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        if (g_settings.auto_dismiss_eval_dialog &&
            IsEvaluationPopupWindow(hWnd)) {
            if (!EndDialog(hWnd, IDCANCEL)) {
                PostMessageW(hWnd, WM_CLOSE, 0, 0);
            }
            return TRUE;
        }

        AttachListViewSubclass(hWnd, true);
        AttachStatusBarSubclass(hWnd, true);
        EnumChildWindows(hWnd, EnumChildProc, 0);
    }
    return TRUE;
}

// Initializes the mod, registers custom thread messages, and installs API hooks
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing WinRAR Tweaks");
    LoadSettings();

    g_msgSettingsChanged =
        RegisterWindowMessageW(L"Windhawk_WinRarTweaks_SettingsChanged");
    g_msgCleanupSearchBar =
        RegisterWindowMessageW(L"Windhawk_WinRarTweaks_CleanupSearchBar");

    WindhawkUtils::SetFunctionHook(GetMessageW, GetMessageW_Hook,
                                   &pfnGetMessageW_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook,
                                   &pfnPeekMessageW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &pfnCreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(DialogBoxParamW, DialogBoxParamW_Hook,
                                   &pfnDialogBoxParamW_Original);
    WindhawkUtils::SetFunctionHook(CreateDialogParamW, CreateDialogParamW_Hook,
                                   &pfnCreateDialogParamW_Original);
    WindhawkUtils::SetFunctionHook(SetWindowTextW, SetWindowTextW_Hook,
                                   &pfnSetWindowTextW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &pfnShowWindow_Original);

    return TRUE;
}

// Processes any WinRAR windows that were already open before mod initialization
void Wh_ModAfterInit() {
    EnumWindows(EnumTopWindowsProc, 0);
}

// Reloads settings and safely posts async refresh messages to open windows
void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumTopWindowsProc, 0);
}

// Safely removes all window subclasses and search bar controls on mod unload
void Wh_ModUninit() {
    Wh_Log(L"Unloading WinRAR Tweaks");

    std::vector<HWND> listViews;
    std::vector<HWND> statusBars;
    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        listViews.assign(g_subclassedListViews.begin(),
                         g_subclassedListViews.end());
        g_subclassedListViews.clear();

        statusBars.assign(g_subclassedStatusBars.begin(),
                          g_subclassedStatusBars.end());
        g_subclassedStatusBars.clear();
        g_statusBarStates.clear();
    }

    for (HWND hWnd : listViews) {
        if (IsWindow(hWnd)) {
            if (g_msgCleanupSearchBar != 0) {
                SendMessageW(hWnd, g_msgCleanupSearchBar, 0, 0);
            }
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, ListViewSubclassProc);
        }
    }

    for (HWND hWnd : statusBars) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, StatusBarSubclassProc);
        }
    }
}
