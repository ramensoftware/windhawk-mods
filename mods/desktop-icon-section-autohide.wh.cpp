// ==WindhawkMod==
// @id              desktop-icon-section-autohide
// @name            Desktop Icon Section Auto-Hide & Fluent Hover Reveal
// @description     Auto-hides desktop icons with zero wallpaper dimming. Features 60 FPS Fluent alpha fade, per-app pinning whitelist, 4-state modes, multi-anchor detection, middle-click toggle, peek mode, and drag/rename shields.
// @version         1.3.4
// @author          Piyush Das
// @github          https://github.com/Piyushdas1624
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -lshell32 -luser32 -lgdi32 -lwinmm -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Section Auto-Hide & Fluent Hover Reveal (Zero Wallpaper Dimming)

Provides a clean, distraction-free desktop experience by automatically hiding desktop icons while keeping your wallpaper, Rainmeter widgets, and live wallpapers completely untouched and crystal clear.

![Desktop Icon Section Auto-Hide Preview](https://raw.githubusercontent.com/Piyushdas1624/windhawk-mods/assets/desktop-icon-section-autohide-preview.jpg)

## Key Features

- **Zero Wallpaper Dimming**: Operates by modulating icon and label rendering directly in GDI. No dark overlays, no duplicated wallpaper snapshots, and no DirectComposition layers.
- **Smooth 60 FPS Fluent Eased Alpha Fade**: Fluid, flicker-free alpha transitions using Ken Perlin's quintic smootherstep curve.
- **Specific App Pinning (Always-Visible Whitelist)**: Keep essential shortcuts (e.g. Recycle Bin, This PC, or work apps) permanently visible at 100% opacity while all other shortcuts auto-hide.
- **Independent Pinned App Interaction**: Clicking, hovering, or launching pinned apps never accidentally unhides the rest of your desktop icons.
- **Three-Way Double-Click Mode Cycle**: Double-clicking empty desktop wallpaper cycles between:
  1. *Auto Mode*: Unpinned icons reveal on section hover and auto-hide on exit or inactivity; pinned icons stay visible.
  2. *Show All*: Keeps all desktop icons visible indefinitely (ideal for organizing files).
  3. *Hide All*: Completely locks all unpinned icons hidden (ideal for presentations or clean wallpaper viewing).
- **Middle-Click Quick Toggle**: Clicking the middle mouse button on empty wallpaper instantly toggles icon visibility.
- **Wallpaper Click-and-Hold Peek Mode**: Press and hold the left mouse button on empty wallpaper for 400 ms to peek at your icons temporarily; releasing smoothly hides them again.
- **Active Rename & File Drag Focus Shield**: Freezes all auto-hide timeouts while actively renaming an icon, dragging a shortcut, or rubber-band selecting multiple files.
- **Multi-Anchor Section Detection**: Configure which edge houses your icons (Left edge, Right edge for secondary monitors, Top edge, or Custom bounding box).
- **Subtle Acoustic Feedback**: Optional subtle Windows sound cues when switching modes.

## Pinning Apps & Shortcuts

In the mod settings, you can enable built-in pinning for the **Recycle Bin** and **This PC**, or provide your own custom app names under **Custom Pinned Apps & Shortcuts**.

- Enter comma-separated names (e.g., `Google Chrome, Discord, Steam, Visual Studio Code`).
- Case-insensitive and extension-agnostic (`.lnk`, `.url` extensions are handled automatically).
- Supports wildcards like `*` and `?` (e.g., `Project*`, `Doc?`).
- Audit and verify all detected and pinned desktop icons in the Windhawk Mod Log tab.

## Credits & Attribution

This mod builds upon and refines ideas and code patterns established in the Windhawk community:
- [zen-desktop-toggle-icons](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/zen-desktop-toggle-icons.wh.cpp) by Lanbo (Liset999) — Desktop window subclassing, window creation hook, property tracking, and clean lifecycle management.
- [desktop-icons-transparency](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/desktop-icons-transparency.wh.cpp) by zed712969-crypto — GDI icon and text alpha blending hooks enabling transparency without wallpaper dimming.
- [transparent-desktop-icons-spotlight](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/transparent-desktop-icons-spotlight.wh.cpp) by drgutman — Desktop hover-reveal interaction concepts.

### Why this is a standalone mod
While related mods focus either on full-desktop toggle hiding or uniform transparency, this mod introduces:
1. **Section-based triggering**: Confines auto-hide and hover-reveal to a configurable screen edge or custom bounding box (ideal for dock-style icon arrangements and multi-monitor setups).
2. **Selective per-icon whitelist pinning**: Keeps essential icons (like Recycle Bin, This PC, or user-selected work apps) permanently visible and interactable while temporary desktop clutter smoothly auto-hides.
3. **Four-state workflow cycle & interaction shields**: Combines Auto, Show All, and Hide All modes with a peek-on-hold gesture, middle-click toggle, and focus protection shields during file renaming, shortcut dragging, and rubber-band selection.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableAutoHide: true
  $name: "Enable Auto-Hide and Hover Reveal"
  $description: "Automatically hide unpinned desktop icons when idle or outside the icon section."
- initialMode: "auto"
  $name: "Startup Desktop Mode"
  $description: "Initial desktop mode when Explorer starts. Select 'Auto Mode' so pinned apps stay permanently visible while unpinned icons auto-hide."
  $options:
    - auto: "Auto Mode (Recommended: Pinned apps always visible; unpinned icons reveal on hover, auto-hide on exit)"
    - pinned_show: "Show All (All icons stay visible indefinitely; auto-hide disabled)"
    - pinned_hide: "Hide All (All unpinned icons stay locked hidden; pinned apps stay visible)"
- threeWayCycle: true
  $name: "Three-Way Double-Click Cycle"
  $description: "Double-clicking empty wallpaper cycles between Auto Mode -> Show All Icons -> Hide All Unpinned Icons."
- middleClickToggle: true
  $name: "Middle-Click (Scroll Wheel) Desktop Quick Toggle"
  $description: "Clicking the middle mouse button anywhere on empty wallpaper instantly toggles icon visibility."
- peekMode: true
  $name: "Wallpaper Click-and-Hold Peek Mode"
  $description: "Press and hold the left mouse button on empty wallpaper to temporarily reveal icons."
- peekDelayMs: 400
  $name: "Peek Mode Hold Delay (milliseconds)"
  $description: "Duration to hold left mouse button before icons peek into view (100 to 1000 ms)."
- smoothFade: true
  $name: "Smooth Eased Alpha Fade (60 FPS Fluent)"
  $description: "Silky smooth 60 FPS eased alpha transition using Perlin's Smootherstep curve and zero-flicker double buffering."
- fadeDurationMs: 220
  $name: "Alpha Fade Duration (milliseconds)"
  $description: "Time taken to fade icons in or out (50 to 1000 ms)."
- anchorSide: "left"
  $name: "Icon Section Anchor Side"
  $description: "Which edge of your desktop monitor contains the icon section."
  $options:
    - left: "Left edge"
    - right: "Right edge (Multi-monitor / Secondary screen)"
    - top: "Top edge"
    - custom: "Custom / Bounding box"
- autoDetectBoundary: true
  $name: "Auto-Detect Icon Section Boundary"
  $description: "Automatically calculate the boundary rect enclosing only unpinned desktop icons."
- fixedBoundaryWidth: 450
  $name: "Fixed Boundary Size (pixels)"
  $description: "Fallback trigger width/height from the anchor edge if auto-detect is disabled."
- boundaryMargin: 60
  $name: "Boundary Margin (pixels)"
  $description: "Extra trigger padding in pixels beyond the unpinned icon boundary."
- autoHideDelay: 3
  $name: "Inactivity Auto-Hide Delay (seconds)"
  $description: "Seconds of inactivity before icons auto-hide while mouse is stationary inside the section (1 to 60 seconds)."
- leaveDelayMs: 600
  $name: "Cursor Exit Delay (milliseconds)"
  $description: "Delay before auto-hiding after cursor leaves the icon section (100 to 3000 ms)."
- focusShield: true
  $name: "Active Rename & File Drag Focus Shield"
  $description: "Freezes auto-hide while actively renaming an icon, dragging a shortcut, or marquee selecting files."
- whitelistPinned: true
  $name: "Specific App Pinning (Always-Visible Whitelist)"
  $description: "Keep specific pinned/whitelisted icons permanently visible on the desktop at all times while all other unpinned icons auto-hide."
- whitelistRecycleBin: true
  $name: "Whitelist: Keep Recycle Bin Always Visible"
  $description: "Recycle Bin remains visible on the desktop at all times."
- whitelistThisPC: true
  $name: "Whitelist: Keep This PC Always Visible"
  $description: "This PC / Computer shortcut remains visible on the desktop at all times."
- pinnedAppList: ""
  $name: "Custom Pinned Apps & Shortcuts (Whitelist)"
  $description: "Comma-separated list of app names, shortcuts, or file names to keep permanently visible (e.g. 'Google Chrome, Discord, Steam, Visual Studio Code'). Case-insensitive, extension-agnostic, and supports wildcards (* and ?). Check the Mod Log tab for a live audit."
- audioFeedback: false
  $name: "Subtle Windows 11 Audio Feedback"
  $description: "Play subtle native acoustic chimes when toggling modes or peeking."
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00
#define _WIN32_IE    0x0A00

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <mmsystem.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <cwctype>
#include <windhawk_utils.h>

// ─────────────────────────────────────────────────────────────────────────────
// Timer Identifiers & Intervals (Unique high values to avoid colliding with Explorer)
// ─────────────────────────────────────────────────────────────────────────────
#define TIMER_TRACK_CURSOR           0x7A01
#define TIMER_TRACK_INTERVAL_MS         150
#define TIMER_FADE_ANIMATION         0x7A02
#define TIMER_FADE_INTERVAL_MS           16
#define TIMER_PEEK_HOLD              0x7A03

// ─────────────────────────────────────────────────────────────────────────────
// Registered Window Messages
// ─────────────────────────────────────────────────────────────────────────────
static UINT g_msgRefreshSettings   = 0;
static UINT g_msgSetupDesktop      = 0;
static UINT g_msgModeCycle         = 0;
static UINT g_msgMiddleClickToggle = 0;
static UINT g_msgAutoHide          = 0;
static UINT g_msgAutoRestore       = 0;
static UINT g_msgUninit            = 0;

// ─────────────────────────────────────────────────────────────────────────────
// 4-State Machine Definition
// ─────────────────────────────────────────────────────────────────────────────
enum DesktopState {
    STATE_AUTO_HIDDEN,      // Auto mode: unpinned icons hidden; pinned apps visible
    STATE_AUTO_REVEALED,    // Auto mode: cursor in section, unpinned icons revealed
    STATE_PINNED_VISIBLE,   // Show All: all icons stay visible indefinitely
    STATE_PINNED_HIDDEN     // Hide All: all unpinned icons locked hidden
};
static DesktopState g_currentState = STATE_AUTO_HIDDEN;

// ─────────────────────────────────────────────────────────────────────────────
// Mod Settings & Cache
// ─────────────────────────────────────────────────────────────────────────────
struct Settings {
    bool enableAutoHide;
    int  initialMode;
    bool threeWayCycle;
    bool middleClickToggle;
    bool peekMode;
    int  peekDelayMs;
    bool smoothFade;
    int  fadeDurationMs;
    int  anchorSide;
    bool autoDetectBoundary;
    int  fixedBoundaryWidth;
    int  boundaryMargin;
    int  autoHideDelay;
    int  leaveDelayMs;
    bool focusShield;
    bool whitelistPinned;
    bool whitelistRecycleBin;
    bool whitelistThisPC;
    bool audioFeedback;
};
static Settings g_settings = {};

static std::vector<std::wstring> g_customPinnedNames;

// Desktop Window Handles (Strictly isolated to current process)
static HWND g_hDesktopDefView  = NULL;
static HWND g_hDesktopListView = NULL;

// Item Cache & Whitelist
struct WhitelistedItem {
    int  index;
    RECT rcBounds;
    RECT rcIcon;
    RECT rcLabel;
    std::wstring name;
};
static std::vector<WhitelistedItem> g_whitelistedItems;
static std::vector<bool>            g_isIndexWhitelisted;

struct UnpinnedBounds {
    RECT rcBoundingBox;
    int  count;
};
static UnpinnedBounds g_unpinnedBounds = { { 0, 0, 0, 0 }, 0 };
static bool g_desktopItemsCacheDirty = true;

// Fade State
static int  g_currentOpacity = 0;
static int  g_startOpacity   = 0;
static int  g_targetOpacity  = 0;
static bool g_isFading       = false;
static LARGE_INTEGER g_qpcFadeStart = {};
static LARGE_INTEGER g_qpcFreq      = {};
static double g_fadeDurationSec     = 0.22;
static bool g_timerPrecisionActive  = false;

// Per-Thread Drawing State Flags
static thread_local bool g_inDesktopPaint          = false;
static thread_local bool g_isWhitelistedItemDrawing = false;
static thread_local bool g_inDirectHook            = false;
static thread_local bool g_inTextHook              = false;

// Whitelist system name cache
static WCHAR g_szRecycleBinName[MAX_PATH] = L"";
static WCHAR g_szThisPCName[MAX_PATH]     = L"";

// Forward Declarations
static void RepaintDesktop(HWND hListView);
static void StartFadeTransition(HWND hwndShellView, int targetOpacity);
static bool IsCursorInTriggerSection(HWND hwndShell, HWND hwndListView);
static void RefreshDesktopItemsCache(HWND hListView);
static bool IsItemWhitelisted(int itemIndex);
static bool IsTextWhitelisted(LPCWSTR pszText, int cch);
static void LoadSettings();

// ─────────────────────────────────────────────────────────────────────────────
// DPI Scaling Helpers
// ─────────────────────────────────────────────────────────────────────────────
static int GetListViewDpi(HWND hwnd)
{
    if (hwnd) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            using GetDpiForWindow_t = UINT (WINAPI *)(HWND);
            GetDpiForWindow_t pfn = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
            if (pfn) {
                UINT dpi = pfn(hwnd);
                if (dpi > 0) return (int)dpi;
            }
        }
    }
    return 96;
}

static int ScaleForDpi(int px, int dpi)
{
    return MulDiv(px, dpi, 96);
}

// ─────────────────────────────────────────────────────────────────────────────
// Resolve System Display Names for Known Folders (Cached once)
// ─────────────────────────────────────────────────────────────────────────────
static void InitWhitelistNames()
{
    if (g_szRecycleBinName[0] != L'\0' && g_szThisPCName[0] != L'\0') {
        return;
    }

    bool coInit = SUCCEEDED(CoInitialize(NULL));

    if (g_szRecycleBinName[0] == L'\0') {
        PIDLIST_ABSOLUTE pidlRecycle = NULL;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0, NULL, &pidlRecycle)) && pidlRecycle) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW((LPCWSTR)pidlRecycle, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME)) {
                wcsncpy_s(g_szRecycleBinName, sfi.szDisplayName, _TRUNCATE);
            }
            CoTaskMemFree(pidlRecycle);
        }
        if (g_szRecycleBinName[0] == L'\0') {
            wcscpy_s(g_szRecycleBinName, L"Recycle Bin");
        }
    }

    if (g_szThisPCName[0] == L'\0') {
        PIDLIST_ABSOLUTE pidlComputer = NULL;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, NULL, &pidlComputer)) && pidlComputer) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW((LPCWSTR)pidlComputer, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME)) {
                wcsncpy_s(g_szThisPCName, sfi.szDisplayName, _TRUNCATE);
            }
            CoTaskMemFree(pidlComputer);
        }
        if (g_szThisPCName[0] == L'\0') {
            wcscpy_s(g_szThisPCName, L"This PC");
        }
    }

    if (coInit) CoUninitialize();
}

// ─────────────────────────────────────────────────────────────────────────────
// Text Normalization & Wildcard Pattern Matching
// ─────────────────────────────────────────────────────────────────────────────
static bool CleanAndNormalizeText(LPCWSTR pszText, int cch, std::wstring& outClean, std::wstring& outRaw)
{
    if (!pszText) return false;

    WCHAR buf[MAX_PATH] = {};
    if (cch < 0) {
        wcsncpy_s(buf, pszText, _TRUNCATE);
    } else {
        int len = std::min<int>(cch, MAX_PATH - 1);
        wcsncpy_s(buf, pszText, len);
        buf[len] = L'\0';
    }

    for (int i = 0; buf[i]; ++i) {
        if (buf[i] == L'\r' || buf[i] == L'\n' || buf[i] == L'\t') {
            buf[i] = L' ';
        }
    }

    WCHAR* start = buf;
    while (*start == L' ' || *start == L'\"' || *start == L'\'') {
        start++;
    }
    int len = (int)wcslen(start);

    while (len > 0) {
        WCHAR ch = start[len - 1];
        if (ch == L' ' || ch == L'\"' || ch == L'\'' || ch == 0x2026) {
            start[--len] = L'\0';
        } else if (len >= 3 && start[len - 1] == L'.' && start[len - 2] == L'.' && start[len - 3] == L'.') {
            len -= 3;
            start[len] = L'\0';
        } else {
            break;
        }
    }
    if (len == 0) return false;

    std::wstring collapsed;
    collapsed.reserve(len);
    bool prevSpace = false;
    for (int i = 0; start[i]; ++i) {
        if (start[i] == L' ') {
            if (!prevSpace) {
                collapsed.push_back(L' ');
                prevSpace = true;
            }
        } else {
            collapsed.push_back(start[i]);
            prevSpace = false;
        }
    }

    outRaw = collapsed;
    outClean = collapsed;

    // Strip trailing extensions (.lnk, .url)
    if (outClean.length() >= 4) {
        std::wstring tail4 = outClean.substr(outClean.length() - 4);
        if (_wcsicmp(tail4.c_str(), L".lnk") == 0 || _wcsicmp(tail4.c_str(), L".url") == 0) {
            outClean.resize(outClean.length() - 4);
        }
    }
    return !outClean.empty();
}

static bool WildcardMatch(const WCHAR* pat, const WCHAR* str)
{
    while (*str) {
        if (*pat == L'*') {
            pat++;
            if (!*pat) return true;
            while (*str) {
                if (WildcardMatch(pat, str)) return true;
                str++;
            }
            return false;
        } else if (*pat == L'?' || towlower(*pat) == towlower(*str)) {
            pat++;
            str++;
        } else {
            return false;
        }
    }
    while (*pat == L'*') pat++;
    return !*pat;
}

static bool MatchPattern(const std::wstring& text, const std::wstring& pattern)
{
    if (pattern.empty() || text.empty()) return false;
    if (_wcsicmp(text.c_str(), pattern.c_str()) == 0) return true;

    if (pattern.find_first_of(L"*?") != std::wstring::npos) {
        return WildcardMatch(pattern.c_str(), text.c_str());
    }

    std::wstring textLower = text;
    std::wstring patLower  = pattern;
    std::transform(textLower.begin(), textLower.end(), textLower.begin(), ::towlower);
    std::transform(patLower.begin(), patLower.end(), patLower.begin(), ::towlower);

    if (textLower == patLower) return true;

    // Whole-word / token matching across all occurrences in the label
    size_t pos = 0;
    while ((pos = textLower.find(patLower, pos)) != std::wstring::npos) {
        bool leftOk  = (pos == 0) || !iswalnum(textLower[pos - 1]);
        bool rightOk = (pos + patLower.length() == textLower.length()) || !iswalnum(textLower[pos + patLower.length()]);
        if (leftOk && rightOk) return true;
        pos++;
    }

    return false;
}

static bool IsTextWhitelisted(LPCWSTR pszText, int cch = -1)
{
    if (!pszText || !g_settings.whitelistPinned) return false;

    std::wstring clean, raw;
    if (!CleanAndNormalizeText(pszText, cch, clean, raw)) return false;

    // 1. Recycle Bin (System name & common fallbacks)
    if (g_settings.whitelistRecycleBin) {
        if (g_szRecycleBinName[0] && _wcsicmp(clean.c_str(), g_szRecycleBinName) == 0) return true;
        if (_wcsicmp(clean.c_str(), L"Recycle Bin") == 0 || _wcsicmp(clean.c_str(), L"Corbeille") == 0 ||
            _wcsicmp(clean.c_str(), L"Papierkorb") == 0 || _wcsicmp(clean.c_str(), L"Papelera de reciclaje") == 0 ||
            _wcsicmp(clean.c_str(), L"Cestino") == 0 || _wcsicmp(clean.c_str(), L"Lixeira") == 0 ||
            _wcsicmp(clean.c_str(), L"Корзина") == 0 || _wcsicmp(clean.c_str(), L"ゴミ箱") == 0 ||
            _wcsicmp(clean.c_str(), L"휴지통") == 0 || _wcsicmp(clean.c_str(), L"回收站") == 0) return true;
    }

    // 2. This PC / Computer (System name & common fallbacks)
    if (g_settings.whitelistThisPC) {
        if (g_szThisPCName[0] && _wcsicmp(clean.c_str(), g_szThisPCName) == 0) return true;
        if (_wcsicmp(clean.c_str(), L"This PC") == 0 || _wcsicmp(clean.c_str(), L"Computer") == 0 ||
            _wcsicmp(clean.c_str(), L"Ce PC") == 0 || _wcsicmp(clean.c_str(), L"Dieser PC") == 0 ||
            _wcsicmp(clean.c_str(), L"Este equipo") == 0 || _wcsicmp(clean.c_str(), L"Questo PC") == 0 ||
            _wcsicmp(clean.c_str(), L"Este Computador") == 0 || _wcsicmp(clean.c_str(), L"Этот компьютер") == 0 ||
            _wcsicmp(clean.c_str(), L"PC") == 0 || _wcsicmp(clean.c_str(), L"此电脑") == 0) return true;
    }

    // 3. Custom user pinned apps & shortcuts
    for (const auto& pattern : g_customPinnedNames) {
        if (MatchPattern(clean, pattern) || MatchPattern(raw, pattern)) {
            return true;
        }
    }

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Desktop Item Cache & Bounds Refresh
// ─────────────────────────────────────────────────────────────────────────────
static void RefreshDesktopItemsCache(HWND hListView)
{
    if (!hListView || !IsWindow(hListView)) return;

    InitWhitelistNames();

    g_whitelistedItems.clear();
    g_unpinnedBounds.count = 0;
    SetRectEmpty(&g_unpinnedBounds.rcBoundingBox);

    int totalCount = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
    if (totalCount <= 0) {
        g_isIndexWhitelisted.clear();
        return;
    }

    g_isIndexWhitelisted.assign(totalCount, false);
    bool firstUnpinned = true;

    for (int i = 0; i < totalCount; ++i) {
        WCHAR text[MAX_PATH] = {};
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = text;
        item.cchTextMax = MAX_PATH;
        if (SendMessageW(hListView, LVM_GETITEMTEXTW, i, (LPARAM)&item) <= 0 && text[0] == L'\0') {
            SendMessageW(hListView, LVM_GETITEMW, 0, (LPARAM)&item);
        }

        RECT rcBounds = {};
        SendMessageW(hListView, LVM_GETITEMRECT, i, (LPARAM)&rcBounds);

        RECT rcIcon = {};
        rcIcon.left = LVIR_ICON;
        SendMessageW(hListView, LVM_GETITEMRECT, i, (LPARAM)&rcIcon);

        RECT rcLabel = {};
        rcLabel.left = LVIR_LABEL;
        SendMessageW(hListView, LVM_GETITEMRECT, i, (LPARAM)&rcLabel);

        bool isPinned = g_settings.whitelistPinned && IsTextWhitelisted(text);

        if (isPinned) {
            g_isIndexWhitelisted[i] = true;
            WhitelistedItem wItem;
            wItem.index = i;
            wItem.rcBounds = rcBounds;
            wItem.rcIcon = rcIcon;
            wItem.rcLabel = rcLabel;
            wItem.name = text;
            g_whitelistedItems.push_back(wItem);
        } else {
            g_unpinnedBounds.count++;
            if (firstUnpinned) {
                g_unpinnedBounds.rcBoundingBox = rcBounds;
                firstUnpinned = false;
            } else {
                g_unpinnedBounds.rcBoundingBox.left   = std::min(g_unpinnedBounds.rcBoundingBox.left, rcBounds.left);
                g_unpinnedBounds.rcBoundingBox.top    = std::min(g_unpinnedBounds.rcBoundingBox.top, rcBounds.top);
                g_unpinnedBounds.rcBoundingBox.right  = std::max(g_unpinnedBounds.rcBoundingBox.right, rcBounds.right);
                g_unpinnedBounds.rcBoundingBox.bottom = std::max(g_unpinnedBounds.rcBoundingBox.bottom, rcBounds.bottom);
            }
        }
    }
}

static bool IsItemWhitelisted(int itemIndex)
{
    if (!g_settings.whitelistPinned || itemIndex < 0 || itemIndex >= (int)g_isIndexWhitelisted.size()) {
        return false;
    }
    return g_isIndexWhitelisted[itemIndex];
}

static bool IsCursorOverOrNearPinnedItem(POINT ptClient, int margin)
{
    if (!g_settings.whitelistPinned || g_whitelistedItems.empty()) return false;
    for (const auto& item : g_whitelistedItems) {
        RECT rcInflated = item.rcBounds;
        if (margin > 0) {
            InflateRect(&rcInflated, margin, margin);
        }
        if (PtInRect(&rcInflated, ptClient)) {
            return true;
        }
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Real-Time Pinned Apps Audit (Mod Log Output)
// ─────────────────────────────────────────────────────────────────────────────
static void LogWhitelistedItemsAudit(HWND hListView)
{
    if (!hListView || !IsWindow(hListView)) return;

    int totalCount  = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
    int pinnedCount = (int)g_whitelistedItems.size();

    Wh_Log(L"═══════════════ PINNED APPS AUDIT ═══════════════");
    Wh_Log(L"Specific App Pinning Enabled: %s", g_settings.whitelistPinned ? L"YES" : L"NO (Whitelist Disabled)");
    Wh_Log(L"Recycle Bin: %s, This PC: %s, Custom Rules: %zu",
           g_settings.whitelistRecycleBin ? L"ON" : L"OFF",
           g_settings.whitelistThisPC ? L"ON" : L"OFF",
           g_customPinnedNames.size());

    for (int i = 0; i < totalCount; ++i) {
        WCHAR text[MAX_PATH] = {};
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = text;
        item.cchTextMax = MAX_PATH;
        if (SendMessageW(hListView, LVM_GETITEMTEXTW, i, (LPARAM)&item) <= 0 && text[0] == L'\0') {
            SendMessageW(hListView, LVM_GETITEMW, 0, (LPARAM)&item);
        }

        bool isPinned = IsItemWhitelisted(i);

        if (isPinned) {
            Wh_Log(L"  [#%02d] [PINNED]    \"%s\"", i + 1, text);
        } else {
            Wh_Log(L"  [#%02d] [AUTO-HIDE] \"%s\"", i + 1, text);
        }
    }

    Wh_Log(L"Total Desktop Icons: %d | Pinned: %d | Auto-Hide: %d",
           totalCount, pinnedCount, totalCount - pinnedCount);
    Wh_Log(L"══════════════════════════════════════════════════");
}

// ─────────────────────────────────────────────────────────────────────────────
// Acoustic Feedback
// ─────────────────────────────────────────────────────────────────────────────
static void PlayDesktopSound(int eventType)
{
    if (!g_settings.audioFeedback) return;
    switch (eventType) {
        case 0: PlaySoundW(L"Notification.Default", NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT); break;
        case 1: PlaySoundW(L"CCSelect",             NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT); break;
        case 2: PlaySoundW(L"Notification.Default", NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT); break;
        case 3: PlaySoundW(L"CCSelect",             NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT); break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Desktop Parent Verification
// ─────────────────────────────────────────────────────────────────────────────
static BOOL IsDesktopParent(HWND hWnd)
{
    if (!hWnd) return FALSE;
    WCHAR className[256] = {};
    if (!GetClassNameW(hWnd, className, 256)) return FALSE;
    return (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0);
}

// ─────────────────────────────────────────────────────────────────────────────
// Load Settings
// ─────────────────────────────────────────────────────────────────────────────
static void LoadSettings()
{
    g_settings.enableAutoHide = Wh_GetIntSetting(L"enableAutoHide") != 0;

    auto strMode = WindhawkUtils::StringSetting::make(L"initialMode");
    if (wcscmp(strMode.get(), L"pinned_show") == 0) {
        g_settings.initialMode = 1;
    } else if (wcscmp(strMode.get(), L"pinned_hide") == 0) {
        g_settings.initialMode = 2;
    } else {
        g_settings.initialMode = 0; // auto
    }

    g_settings.threeWayCycle      = Wh_GetIntSetting(L"threeWayCycle") != 0;
    g_settings.middleClickToggle  = Wh_GetIntSetting(L"middleClickToggle") != 0;
    g_settings.peekMode           = Wh_GetIntSetting(L"peekMode") != 0;
    g_settings.peekDelayMs        = std::clamp(Wh_GetIntSetting(L"peekDelayMs"), 100, 1000);
    g_settings.smoothFade         = Wh_GetIntSetting(L"smoothFade") != 0;
    g_settings.fadeDurationMs     = std::clamp(Wh_GetIntSetting(L"fadeDurationMs"), 50, 1000);

    auto strAnchor = WindhawkUtils::StringSetting::make(L"anchorSide");
    if (wcscmp(strAnchor.get(), L"right") == 0) {
        g_settings.anchorSide = 1;
    } else if (wcscmp(strAnchor.get(), L"top") == 0) {
        g_settings.anchorSide = 2;
    } else if (wcscmp(strAnchor.get(), L"custom") == 0) {
        g_settings.anchorSide = 3;
    } else {
        g_settings.anchorSide = 0; // left
    }

    g_settings.autoDetectBoundary  = Wh_GetIntSetting(L"autoDetectBoundary") != 0;
    g_settings.fixedBoundaryWidth  = std::clamp(Wh_GetIntSetting(L"fixedBoundaryWidth"), 100, 3840);
    g_settings.boundaryMargin      = std::clamp(Wh_GetIntSetting(L"boundaryMargin"), 0, 500);
    g_settings.autoHideDelay       = std::clamp(Wh_GetIntSetting(L"autoHideDelay"), 1, 60);
    g_settings.leaveDelayMs        = std::clamp(Wh_GetIntSetting(L"leaveDelayMs"), 100, 3000);
    g_settings.focusShield         = Wh_GetIntSetting(L"focusShield") != 0;
    g_settings.whitelistPinned     = Wh_GetIntSetting(L"whitelistPinned") != 0;
    g_settings.whitelistRecycleBin = Wh_GetIntSetting(L"whitelistRecycleBin") != 0;
    g_settings.whitelistThisPC     = Wh_GetIntSetting(L"whitelistThisPC") != 0;
    g_settings.audioFeedback       = Wh_GetIntSetting(L"audioFeedback") != 0;

    g_customPinnedNames.clear();
    auto strPinned = WindhawkUtils::StringSetting::make(L"pinnedAppList");
    std::wstring s = strPinned.get();
    size_t start = 0;
    while (start < s.length()) {
        size_t delim = s.find_first_of(L",;\r\n", start);
        if (delim == std::wstring::npos) delim = s.length();
        std::wstring token = s.substr(start, delim - start);
        size_t first = token.find_first_not_of(L" \t\r\n\"'");
        size_t last  = token.find_last_not_of(L" \t\r\n\"'");
        if (first != std::wstring::npos && last != std::wstring::npos) {
            token = token.substr(first, last - first + 1);
            if (!token.empty()) {
                g_customPinnedNames.push_back(token);
            }
        }
        start = delim + 1;
    }

    Wh_Log(L"Settings: autoHide=%d, mode=%d, smoothFade=%d, fadeDuration=%dms, anchor=%d, whitelist=%d, customPinnedCount=%zu",
           (int)g_settings.enableAutoHide, g_settings.initialMode, (int)g_settings.smoothFade,
           g_settings.fadeDurationMs, g_settings.anchorSide, (int)g_settings.whitelistPinned,
           g_customPinnedNames.size());
}

// ─────────────────────────────────────────────────────────────────────────────
// Full-Screen Application Guard
// ─────────────────────────────────────────────────────────────────────────────
static bool IsFullscreenWindowActive()
{
    QUERY_USER_NOTIFICATION_STATE state;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }

    switch (state) {
        case QUNS_NOT_PRESENT:
        case QUNS_BUSY:
        case QUNS_RUNNING_D3D_FULL_SCREEN:
        case QUNS_PRESENTATION_MODE:
            return true;
        default:
            return false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Multi-Anchor Section Boundary Helper (DPI-scaled)
// ─────────────────────────────────────────────────────────────────────────────
static RECT GetSectionTriggerRect(HWND hwndShell, HWND hwndListView)
{
    RECT rcListView = {};
    if (hwndListView && IsWindow(hwndListView)) {
        GetClientRect(hwndListView, &rcListView);
    } else if (hwndShell && IsWindow(hwndShell)) {
        GetClientRect(hwndShell, &rcListView);
    } else {
        rcListView = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    }

    int viewWidth  = rcListView.right - rcListView.left;
    int viewHeight = rcListView.bottom - rcListView.top;
    if (viewWidth <= 0)  viewWidth  = 1920;
    if (viewHeight <= 0) viewHeight = 1080;

    int dpi = GetListViewDpi(hwndListView);
    int fixedBoundaryWidth = ScaleForDpi(g_settings.fixedBoundaryWidth, dpi);
    int boundaryMargin     = ScaleForDpi(g_settings.boundaryMargin, dpi);

    if (g_unpinnedBounds.count == 0 && !g_whitelistedItems.empty()) {
        return { 0, 0, 0, 0 };
    }

    int minLeft   = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.left : 0;
    int minTop    = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.top : 0;
    int maxRight  = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.right : fixedBoundaryWidth;
    int maxBottom = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.bottom : viewHeight;

    RECT rcTrigger = {};

    switch (g_settings.anchorSide) {
        case 1: // Right edge
        {
            int triggerLeft = 0;
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && minLeft < viewWidth) {
                triggerLeft = minLeft - boundaryMargin;
            } else {
                triggerLeft = viewWidth - fixedBoundaryWidth - boundaryMargin;
            }
            if (triggerLeft < 0) triggerLeft = 0;
            int triggerTop = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::max(0, minTop - boundaryMargin) : 0;
            int triggerBottom = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::min(viewHeight, maxBottom + boundaryMargin) : viewHeight;
            rcTrigger = { triggerLeft, triggerTop, viewWidth, triggerBottom };
            break;
        }
        case 2: // Top edge
        {
            int triggerBottom = 0;
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && maxBottom > 0) {
                triggerBottom = maxBottom + boundaryMargin;
            } else {
                triggerBottom = fixedBoundaryWidth + boundaryMargin;
            }
            if (triggerBottom > viewHeight) triggerBottom = viewHeight;
            int triggerLeft = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::max(0, minLeft - boundaryMargin) : 0;
            int triggerRight = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::min(viewWidth, maxRight + boundaryMargin) : viewWidth;
            rcTrigger = { triggerLeft, 0, triggerRight, triggerBottom };
            break;
        }
        case 3: // Custom bounding box
        {
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && maxRight > 0) {
                rcTrigger = {
                    std::max<int>(0, minLeft - boundaryMargin),
                    std::max<int>(0, minTop - boundaryMargin),
                    std::min<int>(viewWidth, maxRight + boundaryMargin),
                    std::min<int>(viewHeight, maxBottom + boundaryMargin)
                };
            } else {
                rcTrigger = { 0, 0, fixedBoundaryWidth + boundaryMargin, viewHeight };
            }
            break;
        }
        case 0: // Left edge (default)
        default:
        {
            int triggerRight = 0;
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && maxRight > 0) {
                triggerRight = maxRight + boundaryMargin;
            } else {
                triggerRight = fixedBoundaryWidth + boundaryMargin;
            }
            if (triggerRight > viewWidth) triggerRight = viewWidth;
            int triggerTop = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::max(0, minTop - boundaryMargin) : 0;
            int triggerBottom = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::min(viewHeight, maxBottom + boundaryMargin) : viewHeight;
            rcTrigger = { 0, triggerTop, triggerRight, triggerBottom };
            break;
        }
    }

    return rcTrigger;
}

static bool IsCursorInTriggerSection(HWND hwndShell, HWND hwndListView)
{
    POINT ptScreen = {};
    if (!GetCursorPos(&ptScreen)) return false;

    HWND hwndUnderCursor = WindowFromPoint(ptScreen);
    if (!hwndUnderCursor) return false;

    bool isDesktop = (hwndUnderCursor == hwndShell ||
                      hwndUnderCursor == hwndListView ||
                      IsChild(hwndShell, hwndUnderCursor) ||
                      hwndUnderCursor == GetAncestor(hwndShell, GA_ROOT));
    if (!isDesktop) return false;

    HWND hList = (hwndListView && IsWindow(hwndListView)) ? hwndListView : g_hDesktopListView;
    POINT ptClient = ptScreen;
    ScreenToClient((hList && IsWindow(hList)) ? hList : hwndShell, &ptClient);

    int dpi = GetListViewDpi(hList);
    int proximityMargin = ScaleForDpi(24, dpi);

    // If cursor is over or near a pinned icon, it never triggers section reveal of unpinned icons
    if (IsCursorOverOrNearPinnedItem(ptClient, proximityMargin)) {
        return false;
    }

    if (g_desktopItemsCacheDirty && hList) {
        g_desktopItemsCacheDirty = false;
        RefreshDesktopItemsCache(hList);
    }

    if (g_unpinnedBounds.count == 0 && !g_whitelistedItems.empty()) {
        return false;
    }

    RECT rcTrigger = GetSectionTriggerRect(hwndShell, hList);
    if (IsRectEmpty(&rcTrigger)) return false;

    return PtInRect(&rcTrigger, ptClient) != 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Focus Shield Helper
// ─────────────────────────────────────────────────────────────────────────────
static bool IsFocusShieldActive(HWND hwndShell, HWND hwndListView)
{
    if (!g_settings.focusShield) return false;

    if (GetPropW(hwndShell, L"ZenShieldRename") != NULL) {
        if (hwndListView && IsWindow(hwndListView)) {
            HWND hEdit = (HWND)SendMessageW(hwndListView, LVM_GETEDITCONTROL, 0, 0);
            if (!hEdit || !IsWindow(hEdit) || !IsWindowVisible(hEdit)) {
                RemovePropW(hwndShell, L"ZenShieldRename");
            } else {
                return true;
            }
        } else {
            return true;
        }
    }

    if (hwndListView && IsWindow(hwndListView)) {
        if (SendMessageW(hwndListView, LVM_GETEDITCONTROL, 0, 0) != 0) return true;
    }

    bool isMouseDown = ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) ||
                       ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);

    if (GetPropW(hwndShell, L"ZenShieldDrag") != NULL) {
        if (!isMouseDown) {
            RemovePropW(hwndShell, L"ZenShieldDrag");
        } else {
            return true;
        }
    }

    if (GetPropW(hwndShell, L"ZenShieldMarquee") != NULL) {
        if (!isMouseDown) {
            RemovePropW(hwndShell, L"ZenShieldMarquee");
        } else {
            return true;
        }
    }

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Unified Parent-Anchored Click Tracker
// ─────────────────────────────────────────────────────────────────────────────
static bool CheckAndRegisterDesktopClick(HWND hwndDefView)
{
    if (!hwndDefView || !IsWindow(hwndDefView)) return false;

    DWORD now = GetTickCount();
    DWORD dblClickTime = GetDoubleClickTime();

    POINT ptScreen = {};
    GetCursorPos(&ptScreen);

    DWORD lastClickTime = HandleToUlong(GetPropW(hwndDefView, L"ZenClickTime"));
    int lastClickX = (int)(short)HandleToUlong(GetPropW(hwndDefView, L"ZenClickX"));
    int lastClickY = (int)(short)HandleToUlong(GetPropW(hwndDefView, L"ZenClickY"));

    int maxDeltaX = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
    int maxDeltaY = GetSystemMetrics(SM_CYDOUBLECLK) / 2;
    if (maxDeltaX < 4) maxDeltaX = 4;
    if (maxDeltaY < 4) maxDeltaY = 4;

    bool isDblClick = false;
    if (lastClickTime > 0 && (now - lastClickTime) <= dblClickTime) {
        if (abs((int)ptScreen.x - lastClickX) <= maxDeltaX &&
            abs((int)ptScreen.y - lastClickY) <= maxDeltaY) {
            isDblClick = true;
        }
    }

    if (isDblClick) {
        SetPropW(hwndDefView, L"ZenClickTime", 0);
        return true;
    } else {
        SetPropW(hwndDefView, L"ZenClickTime", UlongToHandle(now));
        SetPropW(hwndDefView, L"ZenClickX",    UlongToHandle((DWORD)(short)ptScreen.x));
        SetPropW(hwndDefView, L"ZenClickY",    UlongToHandle((DWORD)(short)ptScreen.y));
        return false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Smooth 60 FPS Fluent Eased Alpha Transition Engine
// ─────────────────────────────────────────────────────────────────────────────
static void RepaintDesktop(HWND hListView)
{
    if (!hListView || !IsWindow(hListView)) return;
    InvalidateRect(hListView, NULL, FALSE);
    UpdateWindow(hListView);
}

static void StartFadeTransition(HWND hwndShellView, int targetOpacity)
{
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    if (!hwndListView) return;

    if (!g_settings.smoothFade) {
        g_currentOpacity = targetOpacity;
        g_targetOpacity  = targetOpacity;
        g_isFading       = false;
        KillTimer(hwndShellView, TIMER_FADE_ANIMATION);
        if (g_timerPrecisionActive) {
            timeEndPeriod(1);
            g_timerPrecisionActive = false;
        }
        if (targetOpacity == 0) {
            LVITEMW lvi = {};
            lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;
            SendMessageW(hwndListView, LVM_SETITEMSTATE, (WPARAM)-1, (LPARAM)&lvi);
        }
        RepaintDesktop(hwndListView);
        return;
    }

    if (g_targetOpacity == targetOpacity && g_isFading) {
        return;
    }

    g_startOpacity  = g_currentOpacity;
    g_targetOpacity = targetOpacity;

    int delta = abs(targetOpacity - g_startOpacity);
    double baseDuration = (double)g_settings.fadeDurationMs / 1000.0;
    if (baseDuration < 0.05) baseDuration = 0.05;
    g_fadeDurationSec = baseDuration * ((double)delta / 100.0);
    if (g_fadeDurationSec < 0.04) g_fadeDurationSec = 0.04;

    if (g_qpcFreq.QuadPart == 0) {
        QueryPerformanceFrequency(&g_qpcFreq);
    }
    QueryPerformanceCounter(&g_qpcFadeStart);
    g_isFading = true;

    if (!g_timerPrecisionActive) {
        timeBeginPeriod(1);
        g_timerPrecisionActive = true;
    }

    SetTimer(hwndShellView, TIMER_FADE_ANIMATION, TIMER_FADE_INTERVAL_MS, NULL);
}

// ─────────────────────────────────────────────────────────────────────────────
// Mode Cycle & Toggle Operations
// ─────────────────────────────────────────────────────────────────────────────
static void CycleDesktopMode(HWND hwndShellView)
{
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);

    if (g_settings.threeWayCycle) {
        if (g_currentState == STATE_AUTO_HIDDEN || g_currentState == STATE_AUTO_REVEALED) {
            g_currentState = STATE_PINNED_VISIBLE;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 100);
            PlayDesktopSound(0);
            Wh_Log(L"Mode -> SHOW_ALL (indefinite visibility of all icons)");
        }
        else if (g_currentState == STATE_PINNED_VISIBLE) {
            g_currentState = STATE_PINNED_HIDDEN;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 0);
            PlayDesktopSound(1);
            Wh_Log(L"Mode -> HIDE_ALL (all unpinned icons locked hidden)");
        }
        else {
            PlayDesktopSound(2);
            if (!g_settings.enableAutoHide) {
                g_currentState = STATE_PINNED_VISIBLE;
                StartFadeTransition(hwndShellView, 100);
                Wh_Log(L"Mode -> SHOW_ALL (auto-hide disabled)");
            } else {
                bool inside = IsCursorInTriggerSection(hwndShellView, hwndListView);
                if (inside) {
                    g_currentState = STATE_AUTO_REVEALED;
                    DWORD now = GetTickCount();
                    SetPropW(hwndShellView, L"ZenLastInsideTick", UlongToHandle(now));
                    SetPropW(hwndShellView, L"ZenLastActiveTick", UlongToHandle(now));
                    StartFadeTransition(hwndShellView, 100);
                    SetTimer(hwndShellView, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
                } else {
                    g_currentState = STATE_AUTO_HIDDEN;
                    StartFadeTransition(hwndShellView, 0);
                    KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
                }
                Wh_Log(L"Mode -> AUTO (inside=%d)", (int)inside);
            }
        }
    } else {
        if (g_currentState == STATE_PINNED_VISIBLE || g_currentState == STATE_AUTO_REVEALED || g_currentOpacity > 0) {
            g_currentState = STATE_PINNED_HIDDEN;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 0);
            PlayDesktopSound(1);
            Wh_Log(L"Toggle -> HIDE_ALL");
        } else {
            g_currentState = STATE_PINNED_VISIBLE;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 100);
            PlayDesktopSound(0);
            Wh_Log(L"Toggle -> SHOW_ALL");
        }
    }
}

static void QuickToggleDesktop(HWND hwndShellView)
{
    if (g_currentState == STATE_PINNED_VISIBLE || g_currentState == STATE_AUTO_REVEALED || g_currentOpacity > 0) {
        g_currentState = STATE_PINNED_HIDDEN;
        KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
        StartFadeTransition(hwndShellView, 0);
        PlayDesktopSound(1);
        Wh_Log(L"Middle-Click -> HIDE_ALL");
    } else {
        g_currentState = STATE_PINNED_VISIBLE;
        KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
        StartFadeTransition(hwndShellView, 100);
        PlayDesktopSound(0);
        Wh_Log(L"Middle-Click -> SHOW_ALL");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Reusable High-Performance GDI Memory DC & Alpha Blending Cache
// ─────────────────────────────────────────────────────────────────────────────
struct CachedTextDC {
    HDC     hdcMem = NULL;
    HBITMAP hBmp   = NULL;
    HBITMAP hbmOld = NULL;
    int     curW   = 0;
    int     curH   = 0;

    void EnsureSize(HDC hdcRef, int w, int h) {
        if (!hdcMem) {
            hdcMem = CreateCompatibleDC(hdcRef);
        }
        if (!hBmp || w > curW || h > curH) {
            if (hdcMem && hbmOld) {
                SelectObject(hdcMem, hbmOld);
                hbmOld = NULL;
            }
            if (hBmp) {
                DeleteObject(hBmp);
                hBmp = NULL;
            }
            curW = std::max(w, curW > 0 ? curW : 512);
            curH = std::max(h, curH > 0 ? curH : 256);
            hBmp = CreateCompatibleBitmap(hdcRef, curW, curH);
            hbmOld = (HBITMAP)SelectObject(hdcMem, hBmp);
        }
    }

    void Cleanup() {
        if (hdcMem) {
            if (hbmOld) {
                SelectObject(hdcMem, hbmOld);
                hbmOld = NULL;
            }
            if (hBmp) {
                DeleteObject(hBmp);
                hBmp = NULL;
            }
            DeleteDC(hdcMem);
            hdcMem = NULL;
        }
        curW = curH = 0;
    }
};
static thread_local CachedTextDC tl_cachedDC;

// ─────────────────────────────────────────────────────────────────────────────
// Native GDI Alpha Blending Hooks (Zero Wallpaper Dimming)
// ─────────────────────────────────────────────────────────────────────────────
using ImageList_DrawIndirect_t = decltype(&ImageList_DrawIndirect);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original = nullptr;

BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp)
{
    if (pimldp && g_inDesktopPaint) {
        if (g_isWhitelistedItemDrawing) {
            g_inDirectHook = true;
            BOOL result = ImageList_DrawIndirect_Original(pimldp);
            g_inDirectHook = false;
            return result;
        }

        if (g_currentOpacity <= 0) {
            return TRUE;
        }
        if (g_currentOpacity >= 100) {
            return ImageList_DrawIndirect_Original(pimldp);
        }

        IMAGELISTDRAWPARAMS params = {};
        size_t copySize = pimldp->cbSize < sizeof(params) ? (size_t)pimldp->cbSize : sizeof(params);
        memcpy(&params, pimldp, copySize);
        params.cbSize = sizeof(params);

        if (!(params.fState & ILS_ALPHA)) {
            params.Frame = 255;
        }
        params.fState |= ILS_ALPHA;
        params.Frame = (DWORD)((params.Frame * g_currentOpacity) / 100);

        g_inDirectHook = true;
        BOOL result = ImageList_DrawIndirect_Original(&params);
        g_inDirectHook = false;
        return result;
    }
    return ImageList_DrawIndirect_Original(pimldp);
}

using GdiAlphaBlend_t = BOOL (WINAPI *)(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
    BLENDFUNCTION ftn
);
GdiAlphaBlend_t GdiAlphaBlend_Original = nullptr;

BOOL WINAPI GdiAlphaBlend_Hook(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
    BLENDFUNCTION ftn
) {
    if (!g_inDirectHook && !g_inTextHook && g_inDesktopPaint) {
        if (g_isWhitelistedItemDrawing) {
            return GdiAlphaBlend_Original(hdcDest, xoriginDest, yoriginDest, wDest, hDest,
                                         hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
        }

        if (g_currentOpacity <= 0) {
            return TRUE;
        }

        if (g_currentOpacity < 100) {
            ftn.SourceConstantAlpha = (BYTE)((ftn.SourceConstantAlpha * g_currentOpacity) / 100);
        }
    }
    return GdiAlphaBlend_Original(hdcDest, xoriginDest, yoriginDest, wDest, hDest,
                                 hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
}

using DrawShadowText_t = decltype(&DrawShadowText);
DrawShadowText_t DrawShadowText_Original = nullptr;

int WINAPI DrawShadowText_Hook(
    HDC hdc, LPCWSTR pszText, UINT cch, RECT *prc, DWORD dwFlags,
    COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset
) {
    if (g_inDesktopPaint) {
        // Measurement call pass-through must precede opacity suppression
        if (dwFlags & DT_CALCRECT) {
            return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
        }

        if (g_isWhitelistedItemDrawing) {
            return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
        }

        if (g_currentOpacity <= 0) {
            return 1;
        }
        if (g_currentOpacity >= 100) {
            return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
        }

        RECT rc = *prc;
        int padX = std::max(4, std::abs(ixOffset) + 2);
        int padY = std::max(4, std::abs(iyOffset) + 2);
        rc.right  += padX;
        rc.bottom += padY;

        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) return 0;

        tl_cachedDC.EnsureSize(hdc, w, h);
        HDC hdcMem = tl_cachedDC.hdcMem;

        HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);

        BitBlt(hdcMem, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);

        RECT rcMem = { 0, 0, w - padX, h - padY };
        g_inTextHook = true;
        int result = DrawShadowText_Original(hdcMem, pszText, cch, &rcMem, dwFlags, crText, crShadow, ixOffset, iyOffset);
        g_inTextHook = false;

        BLENDFUNCTION bf = {};
        bf.BlendOp             = AC_SRC_OVER;
        bf.BlendFlags          = 0;
        bf.SourceConstantAlpha = (BYTE)((255 * g_currentOpacity) / 100);
        bf.AlphaFormat         = 0;

        GdiAlphaBlend_Original(hdc, rc.left, rc.top, w, h, hdcMem, 0, 0, w, h, bf);

        SelectObject(hdcMem, hOldFont);
        return result;
    }
    return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
}

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original = nullptr;

int WINAPI DrawTextW_Hook(
    HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format
) {
    if (g_inDesktopPaint && !g_inTextHook) {
        // Measurement call pass-through must precede opacity suppression
        if (format & DT_CALCRECT) {
            return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
        }

        if (g_isWhitelistedItemDrawing) {
            return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
        }

        if (g_currentOpacity <= 0) {
            return 1;
        }
        if (g_currentOpacity >= 100) {
            return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
        }

        RECT rc = *lprc;
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) return 0;

        tl_cachedDC.EnsureSize(hdc, w, h);
        HDC hdcMem = tl_cachedDC.hdcMem;

        HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);

        BitBlt(hdcMem, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);

        SetTextColor(hdcMem, GetTextColor(hdc));
        SetBkMode(hdcMem, TRANSPARENT);

        RECT rcMem = { 0, 0, w, h };
        g_inTextHook = true;
        int result = DrawTextW_Original(hdcMem, lpchText, cchText, &rcMem, format);
        g_inTextHook = false;

        BLENDFUNCTION bf = {};
        bf.BlendOp             = AC_SRC_OVER;
        bf.BlendFlags          = 0;
        bf.SourceConstantAlpha = (BYTE)((255 * g_currentOpacity) / 100);
        bf.AlphaFormat         = 0;

        GdiAlphaBlend_Original(hdc, rc.left, rc.top, w, h, hdcMem, 0, 0, w, h, bf);

        SelectObject(hdcMem, hOldFont);
        return result;
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t ExtTextOutW_Original = nullptr;

BOOL WINAPI ExtTextOutW_Hook(
    HDC hdc, int x, int y, UINT options, const RECT *lprect,
    LPCWSTR lpString, UINT c, const INT *lpDx
) {
    if (g_inDesktopPaint && !g_inTextHook) {
        if (g_isWhitelistedItemDrawing) {
            return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
        }

        if (g_currentOpacity <= 0) {
            return TRUE;
        }
        if (g_currentOpacity >= 100) {
            return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
        }

        RECT rc;
        if (lprect) {
            rc = *lprect;
        } else {
            SIZE sz;
            GetTextExtentPoint32W(hdc, lpString, c, &sz);
            rc = { x, y, x + sz.cx, y + sz.cy };
        }

        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) return TRUE;

        tl_cachedDC.EnsureSize(hdc, w, h);
        HDC hdcMem = tl_cachedDC.hdcMem;

        HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);

        BitBlt(hdcMem, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);

        SetTextColor(hdcMem, GetTextColor(hdc));
        UINT oldAlign = SetTextAlign(hdcMem, GetTextAlign(hdc));
        SetBkMode(hdcMem, TRANSPARENT);

        g_inTextHook = true;
        ExtTextOutW_Original(hdcMem, x - rc.left, y - rc.top, options & ~ETO_OPAQUE, NULL, lpString, c, lpDx);
        g_inTextHook = false;

        BLENDFUNCTION bf = {};
        bf.BlendOp             = AC_SRC_OVER;
        bf.BlendFlags          = 0;
        bf.SourceConstantAlpha = (BYTE)((255 * g_currentOpacity) / 100);
        bf.AlphaFormat         = 0;

        GdiAlphaBlend_Original(hdc, rc.left, rc.top, w, h, hdcMem, 0, 0, w, h, bf);

        SetTextAlign(hdcMem, oldAlign);
        SelectObject(hdcMem, hOldFont);
        return TRUE;
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SysListView32 (Desktop Icon List)
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopListViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    if (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) {
        g_hDesktopListView = hWnd;
        if (g_desktopItemsCacheDirty) {
            g_desktopItemsCacheDirty = false;
            RefreshDesktopItemsCache(hWnd);
        }
        g_inDesktopPaint = true;
        g_isWhitelistedItemDrawing = false;
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_inDesktopPaint = false;
        g_isWhitelistedItemDrawing = false;
        return result;
    }

    if (uMsg == WM_WINDOWPOSCHANGED || uMsg == WM_STYLECHANGED ||
        uMsg == LVM_SORTITEMS || uMsg == LVM_SORTITEMSEX || uMsg == LVM_ARRANGE) {
        g_desktopItemsCacheDirty = true;
    }

    if (uMsg == WM_NCDESTROY) {
        if (hWnd == g_hDesktopListView) g_hDesktopListView = NULL;
    }

    HWND hwndParent = GetParent(hWnd);
    POINT ptMouseClient = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
    bool isOverPinned = IsCursorOverOrNearPinnedItem(ptMouseClient, 0);

    // Direct Interaction with Pinned Icons:
    // Clicks on pinned items execute natively without waking unpinned dock or cycling mode
    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN) {
        if (isOverPinned) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }

    // Update active timestamps on mouse activity over icons/desktop
    if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN) {
        if (hwndParent) {
            DWORD now = GetTickCount();
            SetPropW(hwndParent, L"ZenLastInsideTick", UlongToHandle(now));
            SetPropW(hwndParent, L"ZenLastActiveTick", UlongToHandle(now));

            // If cursor moved into unpinned section while Auto-Hidden, trigger restore
            if (g_currentState == STATE_AUTO_HIDDEN && g_settings.enableAutoHide) {
                if (!isOverPinned && IsCursorInTriggerSection(hwndParent, hWnd)) {
                    PostMessageW(hwndParent, g_msgAutoRestore, 0, 0);
                }
            }
        }
    }

    // Middle-click desktop quick toggle
    if (g_settings.middleClickToggle) {
        if (uMsg == WM_MBUTTONDOWN) {
            LVHITTESTINFO ht = {};
            ht.pt = ptMouseClient;
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            bool onEmptySpace = (ht.iItem == -1) ||
                                (g_currentOpacity == 0 && (!g_settings.whitelistPinned || !IsItemWhitelisted(ht.iItem)));
            if (onEmptySpace) {
                return 0; // Prevent middle-click scroll anchor
            }
        }
        else if (uMsg == WM_MBUTTONUP) {
            LVHITTESTINFO ht = {};
            ht.pt = ptMouseClient;
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            bool onEmptySpace = (ht.iItem == -1) ||
                                (g_currentOpacity == 0 && (!g_settings.whitelistPinned || !IsItemWhitelisted(ht.iItem)));
            if (onEmptySpace) {
                if (hwndParent) {
                    PostMessageW(hwndParent, g_msgMiddleClickToggle, 0, 0);
                }
                return 0;
            }
        }
    }

    // Peek Mode & Double-Click handling on empty/hidden space
    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) {
        LVHITTESTINFO ht = {};
        ht.pt = ptMouseClient;
        SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

        bool isOverWhitelisted = (ht.iItem != -1 && g_settings.whitelistPinned && IsItemWhitelisted(ht.iItem));
        if (isOverWhitelisted || isOverPinned) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        bool onEmptySpace = (ht.iItem == -1) || (g_currentOpacity == 0);

        if (onEmptySpace && hwndParent) {
            if (g_settings.peekMode && (g_currentState == STATE_AUTO_HIDDEN || g_currentState == STATE_PINNED_HIDDEN || g_currentOpacity == 0)) {
                SetTimer(hwndParent, TIMER_PEEK_HOLD, g_settings.peekDelayMs, NULL);
                SetPropW(hwndParent, L"ZenPeekArmed", UlongToHandle(1));
            }

            bool isDbl = (uMsg == WM_LBUTTONDBLCLK);
            if (!isDbl) {
                isDbl = CheckAndRegisterDesktopClick(hwndParent);
            } else {
                SetPropW(hwndParent, L"ZenClickTime", 0);
            }

            if (isDbl) {
                KillTimer(hwndParent, TIMER_PEEK_HOLD);
                RemovePropW(hwndParent, L"ZenPeekArmed");
                PostMessageW(hwndParent, g_msgModeCycle, 0, 0);
                return 0;
            }

            // If unpinned icons are hidden, deselect and prevent activating invisible icons
            if (g_currentOpacity == 0) {
                LVITEMW lvi = {};
                lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
                lvi.state = 0;
                SendMessageW(hWnd, LVM_SETITEMSTATE, (WPARAM)-1, (LPARAM)&lvi);
                return 0;
            }
        }
    }

    // Context menu redirection when icons are hidden
    if (uMsg == WM_CONTEXTMENU && hwndParent) {
        POINT ptScreen = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        POINT ptClient = ptScreen;
        ScreenToClient(hWnd, &ptClient);

        if (IsCursorOverOrNearPinnedItem(ptClient, 0)) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        LVHITTESTINFO ht = {};
        ht.pt = ptClient;
        SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

        if (g_currentOpacity == 0 && ht.iItem != -1) {
            // Forward right-click on invisible icon to DefView so desktop wallpaper context menu opens
            return SendMessageW(hwndParent, WM_CONTEXTMENU, (WPARAM)hwndParent, lParam);
        }
    }

    if (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP || uMsg == WM_CAPTURECHANGED || uMsg == WM_CANCELMODE) {
        if (hwndParent) {
            KillTimer(hwndParent, TIMER_PEEK_HOLD);
            RemovePropW(hwndParent, L"ZenPeekArmed");

            if (uMsg == WM_LBUTTONUP && GetPropW(hwndParent, L"ZenIsPeeking") != NULL) {
                RemovePropW(hwndParent, L"ZenIsPeeking");
                StartFadeTransition(hwndParent, 0);
                PlayDesktopSound(3);
                Wh_Log(L"Peek released (ListView) -> hide");
                return 0;
            }

            RemovePropW(hwndParent, L"ZenShieldDrag");
            RemovePropW(hwndParent, L"ZenShieldMarquee");
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SHELLDLL_DefView (Desktop Background View)
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopShellViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    if (uMsg == g_msgSetupDesktop) {
        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (!hwndListView) return 0;

        InitWhitelistNames();
        g_desktopItemsCacheDirty = false;
        RefreshDesktopItemsCache(hwndListView);
        LogWhitelistedItemsAudit(hwndListView);

        if (!g_settings.enableAutoHide) {
            g_currentState   = STATE_PINNED_VISIBLE;
            g_currentOpacity = 100;
            g_targetOpacity  = 100;
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            RepaintDesktop(hwndListView);
            return 0;
        }

        if (g_settings.initialMode == 1) {
            g_currentState   = STATE_PINNED_VISIBLE;
            g_currentOpacity = 100;
            g_targetOpacity  = 100;
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            RepaintDesktop(hwndListView);
        } else if (g_settings.initialMode == 2) {
            g_currentState   = STATE_PINNED_HIDDEN;
            g_currentOpacity = 0;
            g_targetOpacity  = 0;
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            RepaintDesktop(hwndListView);
        } else {
            bool inside = IsCursorInTriggerSection(hWnd, hwndListView);
            if (inside) {
                g_currentState   = STATE_AUTO_REVEALED;
                g_currentOpacity = 100;
                g_targetOpacity  = 100;
                DWORD now = GetTickCount();
                SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
                SetPropW(hWnd, L"ZenLastActiveTick", UlongToHandle(now));
                SetTimer(hWnd, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
            } else {
                g_currentState   = STATE_AUTO_HIDDEN;
                g_currentOpacity = 0;
                g_targetOpacity  = 0;
            }
            RepaintDesktop(hwndListView);
        }
        return 0;
    }

    if (uMsg == g_msgUninit) {
        KillTimer(hWnd, TIMER_TRACK_CURSOR);
        KillTimer(hWnd, TIMER_FADE_ANIMATION);
        KillTimer(hWnd, TIMER_PEEK_HOLD);

        if (g_timerPrecisionActive) {
            timeEndPeriod(1);
            g_timerPrecisionActive = false;
        }

        RemovePropW(hWnd, L"ZenClickTime");
        RemovePropW(hWnd, L"ZenClickX");
        RemovePropW(hWnd, L"ZenClickY");
        RemovePropW(hWnd, L"ZenLastInsideTick");
        RemovePropW(hWnd, L"ZenLastActiveTick");
        RemovePropW(hWnd, L"ZenShieldRename");
        RemovePropW(hWnd, L"ZenShieldDrag");
        RemovePropW(hWnd, L"ZenShieldMarquee");
        RemovePropW(hWnd, L"ZenPeekArmed");
        RemovePropW(hWnd, L"ZenIsPeeking");

        tl_cachedDC.Cleanup();

        HWND lv = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (lv) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, DesktopListViewSubclassProc);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, DesktopShellViewSubclassProc);

        g_currentOpacity = 100;
        g_targetOpacity  = 100;
        g_isFading       = false;
        if (lv && IsWindow(lv)) {
            RepaintDesktop(lv);
        }
        return 0;
    }

    if (uMsg == g_msgModeCycle) {
        CycleDesktopMode(hWnd);
        return 0;
    }
    if (uMsg == g_msgMiddleClickToggle) {
        QuickToggleDesktop(hWnd);
        return 0;
    }
    if (uMsg == g_msgAutoHide) {
        if (g_currentState == STATE_AUTO_REVEALED) {
            g_currentState = STATE_AUTO_HIDDEN;
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            StartFadeTransition(hWnd, 0);
            Wh_Log(L"AutoHide: unpinned icons hidden");
        }
        return 0;
    }
    if (uMsg == g_msgAutoRestore) {
        if (g_currentState == STATE_AUTO_HIDDEN) {
            g_currentState = STATE_AUTO_REVEALED;
            DWORD now = GetTickCount();
            SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
            SetPropW(hWnd, L"ZenLastActiveTick", UlongToHandle(now));
            StartFadeTransition(hWnd, 100);

            if (g_settings.enableAutoHide) {
                SetTimer(hWnd, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
            }
            Wh_Log(L"AutoRestore: unpinned icons revealed in icon section");
        }
        return 0;
    }

    if (uMsg == g_msgRefreshSettings) {
        LoadSettings();
        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (hwndListView) {
            g_desktopItemsCacheDirty = false;
            RefreshDesktopItemsCache(hwndListView);
            LogWhitelistedItemsAudit(hwndListView);
        }
        if (!g_settings.enableAutoHide) {
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            if (g_currentState == STATE_AUTO_HIDDEN) {
                g_currentState = STATE_AUTO_REVEALED;
                StartFadeTransition(hWnd, 100);
            }
        } else if (g_currentState == STATE_AUTO_REVEALED) {
            SetTimer(hWnd, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
        } else {
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
        }
        if (hwndListView) {
            RepaintDesktop(hwndListView);
        }
        return 0;
    }

    // ── Item-Level Whitelist Detection via ListView CustomDraw ────────────────
    if (uMsg == WM_NOTIFY && g_settings.whitelistPinned) {
        NMHDR* pnm = (NMHDR*)lParam;
        if (pnm && pnm->code == NM_CUSTOMDRAW) {
            NMCUSTOMDRAW* pnmcd = (NMCUSTOMDRAW*)lParam;
            LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (pnmcd->dwDrawStage == CDDS_PREPAINT) {
                return lr | CDRF_NOTIFYITEMDRAW;
            }
            if ((pnmcd->dwDrawStage & CDDS_ITEMPREPAINT) == CDDS_ITEMPREPAINT) {
                int itemIdx = (int)pnmcd->dwItemSpec;
                g_isWhitelistedItemDrawing = IsItemWhitelisted(itemIdx);
                return lr | CDRF_NOTIFYPOSTPAINT;
            }
            if ((pnmcd->dwDrawStage & CDDS_ITEMPOSTPAINT) == CDDS_ITEMPOSTPAINT) {
                g_isWhitelistedItemDrawing = false;
                return lr;
            }
            return lr;
        }
    }

    // ── Active Rename & File Drag Focus Shield Notifications ──────────────────
    if (uMsg == WM_NOTIFY) {
        NMHDR* pnm = (NMHDR*)lParam;
        if (pnm) {
            if (pnm->code == LVN_INSERTITEM || pnm->code == LVN_DELETEITEM || pnm->code == LVN_ITEMCHANGED) {
                g_desktopItemsCacheDirty = true;
            }
            else if (g_settings.focusShield && (pnm->code == LVN_BEGINLABELEDITW || pnm->code == LVN_BEGINLABELEDITA)) {
                SetPropW(hWnd, L"ZenShieldRename", UlongToHandle(1));
                Wh_Log(L"FocusShield: Rename active");
            }
            else if (pnm->code == LVN_ENDLABELEDITW || pnm->code == LVN_ENDLABELEDITA) {
                if (g_settings.focusShield) RemovePropW(hWnd, L"ZenShieldRename");
                g_desktopItemsCacheDirty = true;
                Wh_Log(L"FocusShield: Rename ended");
                HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
                if (hwndListView) RefreshDesktopItemsCache(hwndListView);
            }
            else if (g_settings.focusShield && (pnm->code == LVN_BEGINDRAG || pnm->code == LVN_BEGINRDRAG)) {
                SetPropW(hWnd, L"ZenShieldDrag", UlongToHandle(1));
                Wh_Log(L"FocusShield: Shortcut Drag active");
            }
            else if (g_settings.focusShield && pnm->code == LVN_MARQUEEBEGIN) {
                SetPropW(hWnd, L"ZenShieldMarquee", UlongToHandle(1));
                Wh_Log(L"FocusShield: Marquee selection active");
            }
        }
    }

    // ── Smooth 60 FPS Fluent Eased Alpha Fade Animation Timer ────────────────
    if (uMsg == WM_TIMER && wParam == TIMER_FADE_ANIMATION) {
        LARGE_INTEGER qpcNow;
        QueryPerformanceCounter(&qpcNow);
        double elapsedSec = (double)(qpcNow.QuadPart - g_qpcFadeStart.QuadPart) / (double)g_qpcFreq.QuadPart;

        int oldOpacity = g_currentOpacity;
        bool finished = false;

        if (elapsedSec >= g_fadeDurationSec || g_fadeDurationSec <= 0.001) {
            g_currentOpacity = g_targetOpacity;
            g_isFading = false;
            KillTimer(hWnd, TIMER_FADE_ANIMATION);
            if (g_timerPrecisionActive) {
                timeEndPeriod(1);
                g_timerPrecisionActive = false;
            }
            finished = true;
        } else {
            float t = (float)(elapsedSec / g_fadeDurationSec);
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            // Fluent Quintic Smootherstep
            float ease = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
            g_currentOpacity = (int)std::round(g_startOpacity + (g_targetOpacity - g_startOpacity) * ease);
            g_currentOpacity = std::clamp(g_currentOpacity, 0, 100);
        }

        if (GetPropW(hWnd, L"ZenIsPeeking") != NULL) {
            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
                RemovePropW(hWnd, L"ZenIsPeeking");
                StartFadeTransition(hWnd, 0);
                PlayDesktopSound(3);
                Wh_Log(L"Peek released (Safety Async Check) -> hide");
            }
        }

        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (hwndListView && IsWindow(hwndListView)) {
            if (finished && g_targetOpacity == 0) {
                LVITEMW lvi = {};
                lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
                lvi.state = 0;
                SendMessageW(hwndListView, LVM_SETITEMSTATE, (WPARAM)-1, (LPARAM)&lvi);
            }
            if (g_currentOpacity != oldOpacity || finished) {
                RepaintDesktop(hwndListView);
            }
        }
        return 0;
    }

    // ── Wallpaper Click-and-Hold Peek Mode Timer ──────────────────────────────
    if (uMsg == WM_TIMER && wParam == TIMER_PEEK_HOLD) {
        KillTimer(hWnd, TIMER_PEEK_HOLD);
        if (GetPropW(hWnd, L"ZenPeekArmed") != NULL) {
            RemovePropW(hWnd, L"ZenPeekArmed");
            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) {
                SetPropW(hWnd, L"ZenIsPeeking", UlongToHandle(1));
                StartFadeTransition(hWnd, 100);
                PlayDesktopSound(3);
                Wh_Log(L"Peek activated -> temporary reveal");
            }
        }
        return 0;
    }

    // ── Cursor Tracking Timer (Active while in STATE_AUTO_REVEALED) ───────────
    if (uMsg == WM_TIMER && wParam == TIMER_TRACK_CURSOR) {
        if (!g_settings.enableAutoHide || g_currentState != STATE_AUTO_REVEALED) {
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            return 0;
        }

        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (!hwndListView) {
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
            return 0;
        }

        if (IsFullscreenWindowActive()) {
            return 0;
        }

        if (IsFocusShieldActive(hWnd, hwndListView)) {
            DWORD now = GetTickCount();
            SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
            SetPropW(hWnd, L"ZenLastActiveTick", UlongToHandle(now));
            return 0;
        }

        DWORD now = GetTickCount();
        bool inside = IsCursorInTriggerSection(hWnd, hwndListView);

        if (inside) {
            SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
            DWORD lastActive = HandleToUlong(GetPropW(hWnd, L"ZenLastActiveTick"));
            if (lastActive == 0) {
                SetPropW(hWnd, L"ZenLastActiveTick", UlongToHandle(now));
                lastActive = now;
            }
            DWORD idleLimitMs = (DWORD)g_settings.autoHideDelay * 1000;
            if ((now - lastActive) >= idleLimitMs) {
                PostMessageW(hWnd, g_msgAutoHide, 0, 0);
            }
        } else {
            DWORD lastInside = HandleToUlong(GetPropW(hWnd, L"ZenLastInsideTick"));
            if (lastInside == 0) {
                SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
                lastInside = now;
            }
            if ((now - lastInside) >= (DWORD)g_settings.leaveDelayMs) {
                PostMessageW(hWnd, g_msgAutoHide, 0, 0);
            }
        }
        return 0;
    }

    if (uMsg == WM_MOUSEMOVE) {
        DWORD now = GetTickCount();
        SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
        SetPropW(hWnd, L"ZenLastActiveTick", UlongToHandle(now));

        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (g_currentState == STATE_AUTO_HIDDEN && g_settings.enableAutoHide) {
            if (IsCursorInTriggerSection(hWnd, hwndListView)) {
                PostMessageW(hWnd, g_msgAutoRestore, 0, 0);
            }
        }
    }

    if (g_settings.middleClickToggle) {
        if (uMsg == WM_MBUTTONDOWN) {
            return 0;
        }
        else if (uMsg == WM_MBUTTONUP) {
            PostMessageW(hWnd, g_msgMiddleClickToggle, 0, 0);
            return 0;
        }
    }

    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) {
        if (g_settings.peekMode && (g_currentState == STATE_AUTO_HIDDEN || g_currentState == STATE_PINNED_HIDDEN || g_currentOpacity == 0)) {
            SetTimer(hWnd, TIMER_PEEK_HOLD, g_settings.peekDelayMs, NULL);
            SetPropW(hWnd, L"ZenPeekArmed", UlongToHandle(1));
        }

        bool isDbl = (uMsg == WM_LBUTTONDBLCLK);
        if (!isDbl) {
            isDbl = CheckAndRegisterDesktopClick(hWnd);
        } else {
            SetPropW(hWnd, L"ZenClickTime", 0);
        }

        if (isDbl) {
            KillTimer(hWnd, TIMER_PEEK_HOLD);
            RemovePropW(hWnd, L"ZenPeekArmed");
            PostMessageW(hWnd, g_msgModeCycle, 0, 0);
            return 0;
        }
    }

    if (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP || uMsg == WM_CAPTURECHANGED || uMsg == WM_CANCELMODE) {
        KillTimer(hWnd, TIMER_PEEK_HOLD);
        RemovePropW(hWnd, L"ZenPeekArmed");

        if (uMsg == WM_LBUTTONUP && GetPropW(hWnd, L"ZenIsPeeking") != NULL) {
            RemovePropW(hWnd, L"ZenIsPeeking");
            StartFadeTransition(hWnd, 0);
            PlayDesktopSound(3);
            Wh_Log(L"Peek released (DefView) -> hide");
            return 0;
        }

        RemovePropW(hWnd, L"ZenShieldDrag");
        RemovePropW(hWnd, L"ZenShieldMarquee");
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Shared Desktop View Setup (Strictly in-process, dispatched to UI thread)
// ─────────────────────────────────────────────────────────────────────────────
static void SetupDesktopView(HWND hwndShell, HWND hwndListView)
{
    if (!hwndShell || !hwndListView) return;

    DWORD pidShell = 0, pidList = 0;
    GetWindowThreadProcessId(hwndShell, &pidShell);
    GetWindowThreadProcessId(hwndListView, &pidList);
    if (pidShell != GetCurrentProcessId() || pidList != GetCurrentProcessId()) return;

    g_hDesktopDefView  = hwndShell;
    g_hDesktopListView = hwndListView;

    WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc, 0);
    WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, DesktopListViewSubclassProc, 0);

    // Dispatch cache build, initial mode application, and timers to the desktop UI thread
    if (g_msgSetupDesktop) {
        SendMessageW(hwndShell, g_msgSetupDesktop, 0, 0);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Window Enumeration & Subclassing
// ─────────────────────────────────────────────────────────────────────────────
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    WCHAR className[256] = {};
    if (!GetClassNameW(hWnd, className, 256)) return TRUE;

    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
        HWND hwndShell = FindWindowExW(hWnd, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) {
            HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            if (hwndListView) {
                Wh_Log(L"Found desktop: Shell=%p, ListView=%p (parent=%s)", hwndShell, hwndListView, className);
                SetupDesktopView(hwndShell, hwndListView);
            }
        }
    }
    return TRUE;
}

static void SubclassExistingWindows() { EnumWindows(EnumWindowsProc, 0); }

static void UnsubclassWindows()
{
    if (g_hDesktopDefView && IsWindow(g_hDesktopDefView)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(g_hDesktopDefView, &pid);
        if (pid == GetCurrentProcessId() && g_msgUninit) {
            SendMessageW(g_hDesktopDefView, g_msgUninit, 0, 0);
        }
        g_hDesktopDefView = NULL;
    }
    g_hDesktopListView = NULL;

    if (g_timerPrecisionActive) {
        timeEndPeriod(1);
        g_timerPrecisionActive = false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// CreateWindowExW Hook for Dynamic Explorer Restarts
// ─────────────────────────────────────────────────────────────────────────────
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Real_CreateWindowExW = nullptr;

HWND WINAPI Hook_CreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = Real_CreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
            if (IsDesktopParent(hWndParent)) {
                DWORD pid = 0;
                GetWindowThreadProcessId(hWnd, &pid);
                if (pid == GetCurrentProcessId()) {
                    Wh_Log(L"Hook: new desktop SHELLDLL_DefView=%p created", hWnd);
                    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopShellViewSubclassProc, 0);
                }
            }
        }
        else if (wcscmp(lpClassName, L"SysListView32") == 0 && hWndParent) {
            WCHAR parentClass[256] = {};
            if (GetClassNameW(hWndParent, parentClass, 256) &&
                wcscmp(parentClass, L"SHELLDLL_DefView") == 0)
            {
                HWND hGrandParent = GetAncestor(hWndParent, GA_PARENT);
                if (IsDesktopParent(hGrandParent)) {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(hWnd, &pid);
                    if (pid == GetCurrentProcessId()) {
                        Wh_Log(L"Hook: new desktop ListView=%p created", hWnd);
                        SetupDesktopView(hWndParent, hWnd);
                    }
                }
            }
        }
    }
    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk Lifecycle
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit()
{
    Wh_Log(L"=== Wh_ModInit v1.3.4 ===");

    QueryPerformanceFrequency(&g_qpcFreq);

    g_msgRefreshSettings   = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.RefreshSettings");
    g_msgSetupDesktop      = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.SetupDesktop");
    g_msgModeCycle         = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.ModeCycle");
    g_msgMiddleClickToggle = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.MiddleClickToggle");
    g_msgAutoHide          = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.AutoHide");
    g_msgAutoRestore       = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.AutoRestore");
    g_msgUninit            = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.Uninit");

    if (!g_msgRefreshSettings || !g_msgSetupDesktop || !g_msgModeCycle ||
        !g_msgMiddleClickToggle || !g_msgAutoHide || !g_msgAutoRestore || !g_msgUninit) {
        Wh_Log(L"FAILED to register private window messages");
        return FALSE;
    }

    LoadSettings();

    // Hook CreateWindowExW for dynamic explorer restarts
    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW,
            Hook_CreateWindowExW,
            &Real_CreateWindowExW)) {
        Wh_Log(L"FAILED to hook CreateWindowExW");
        return FALSE;
    }

    // Hook native GDI icon and text drawing functions
    WindhawkUtils::SetFunctionHook(
        ImageList_DrawIndirect,
        ImageList_DrawIndirect_Hook,
        &ImageList_DrawIndirect_Original);

    WindhawkUtils::SetFunctionHook(
        DrawShadowText,
        DrawShadowText_Hook,
        &DrawShadowText_Original);

    WindhawkUtils::SetFunctionHook(
        DrawTextW,
        DrawTextW_Hook,
        &DrawTextW_Original);

    WindhawkUtils::SetFunctionHook(
        ExtTextOutW,
        ExtTextOutW_Hook,
        &ExtTextOutW_Original);

    HMODULE hGdi = GetModuleHandleW(L"gdi32full.dll");
    if (!hGdi) hGdi = GetModuleHandleW(L"gdi32.dll");
    if (hGdi) {
        void* pGdiAlphaBlend = (void*)GetProcAddress(hGdi, "GdiAlphaBlend");
        if (pGdiAlphaBlend) {
            WindhawkUtils::SetFunctionHook(
                (GdiAlphaBlend_t)pGdiAlphaBlend,
                GdiAlphaBlend_Hook,
                &GdiAlphaBlend_Original);
        }
    }

    Wh_Log(L"Init hooks registered successfully");
    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L"Wh_ModAfterInit: Subclassing existing desktop windows");
    SubclassExistingWindows();
}

void Wh_ModUninit()
{
    Wh_Log(L"=== Wh_ModUninit ===");
    UnsubclassWindows();
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"=== Settings changed ===");
    if (g_hDesktopDefView && IsWindow(g_hDesktopDefView)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(g_hDesktopDefView, &pid);
        if (pid == GetCurrentProcessId() && g_msgRefreshSettings) {
            SendMessageW(g_hDesktopDefView, g_msgRefreshSettings, 0, 0);
            return;
        }
    }
    LoadSettings();
}
