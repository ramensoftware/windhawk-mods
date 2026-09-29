// ==WindhawkMod==
// @id              desktop-icon-section-autohide
// @name            Desktop Icon Section Auto-Hide & Fluent Hover Reveal
// @description     Auto-hides desktop icons with zero wallpaper dimming. Features 60 FPS Fluent alpha fade, per-app pinning whitelist with preview, 4-state modes, multi-anchor detection, middle-click toggle, peek mode, and drag/rename shields.
// @version         1.3.2
// @author          Piyush Das
// @github          https://github.com/Piyushdas1624
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -luser32 -lgdi32 -lmsimg32 -lwinmm -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Section Auto-Hide & Fluent Hover Reveal (Zero Wallpaper Dimming)

Provides the ultimate clean desktop experience:
- **Zero Wallpaper Dimming**: No duplicate wallpaper overlays, no DirectX composite dimming, and no dark layers. Your wallpaper, Rainmeter widgets, and live wallpapers remain 100% untouched and crystal clear.
- **Smooth 60 FPS Fluent Eased Alpha Fade**: Replaces instant visual snaps, flicker, and stutter with a silky-smooth 60 FPS eased alpha transition using Ken Perlin's C2 continuous Smootherstep curve. Driven by high-resolution QueryPerformanceCounter microsecond timing, 1ms multimedia timer scheduling (`timeBeginPeriod`), and non-erasing double-buffered invalidation.
- **Specific App Pinning (Always-Visible Whitelist)**: Pin individual apps and shortcuts (Recycle Bin, This PC, Chrome, Discord, Steam, etc.) so they stay permanently visible at 100% opacity at all times, while all other unpinned icons auto-hide.
- **Real-Time Pinned Apps Audit & Preview**: Immediate visual feedback showing all detected desktop icons, which ones are pinned, and which ones auto-hide (via native Windows Notification Banners, detailed Windhawk mod logs, and Ctrl + Middle-Click shortcut).
- **Independent Pinned App Interaction**: Interacting with, hovering over, clicking, or launching pinned whitelisted apps never accidentally restores or flashes the auto-hidden icons.
- **4-State Machine Architecture**: Eliminates premature hide bugs by cleanly separating `STATE_AUTO_HIDDEN`, `STATE_AUTO_REVEALED`, `STATE_PINNED_VISIBLE`, and `STATE_PINNED_HIDDEN`.
- **Three-Way Double-Click Mode Cycle**: Double-clicking empty desktop wallpaper seamlessly cycles between:
  1. *Auto Mode*: Unpinned icons reveal on section hover, auto-hide on exit or idle. Pinned icons stay visible.
  2. *Show All*: Keeps ALL icons visible indefinitely (ideal for organizing files).
  3. *Hide All*: Completely locks ALL unpinned icons hidden (ideal for presentations).
- **Middle-Click Desktop Quick Toggle**: Clicking the middle mouse button anywhere on empty wallpaper instantly toggles icon visibility.
- **Ctrl + Middle-Click Live Preview**: Pressing Ctrl + Middle-Click anywhere on wallpaper instantly shows the Pinned Apps Preview notification.
- **Wallpaper Click-and-Hold Peek Mode**: Pressing and holding the left mouse button on empty wallpaper for 400ms reveals icons temporarily; releasing the button smoothly hides them back.
- **Active Rename & File Drag Focus Shield**: Freezes all auto-hide timeouts while actively renaming a desktop icon (`LVN_BEGINLABELEDIT`), dragging a shortcut, or marquee selecting multiple files.
- **Multi-Anchor Section Detection**: Configure which edge houses your icons (Left edge, Right edge for dual monitors, Top edge, or Custom bounding box).
- **Subtle Windows 11 Audio Feedback**: Plays native Windows acoustic chimes when toggling modes or peeking.
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
  $description: "Clicking the middle mouse button anywhere on empty wallpaper instantly toggles icon visibility. Tip: Ctrl + Middle-Click shows the Pinned Apps Preview notification on demand!"
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
  $description: "Comma-separated list of app names, shortcuts, or file names to keep permanently visible (e.g. 'Google Chrome, Discord, Steam, Visual Studio Code'). Case-insensitive, extension-agnostic, and supports partial matches. Check the Mod Log tab or notification banner for a live preview."
- showPinnedPreviewToast: true
  $name: "Show Pinned Apps Preview Notification"
  $description: "Displays a desktop notification showing all currently detected and pinned apps whenever settings are saved or reloaded. You can also press Ctrl + Middle-Click on empty wallpaper anytime."
- audioFeedback: true
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
// Timer Identifiers & Intervals
// ─────────────────────────────────────────────────────────────────────────────
#define TIMER_TRACK_CURSOR           1002
#define TIMER_TRACK_INTERVAL_MS       150
#define TIMER_FADE_ANIMATION         1003
#define TIMER_FADE_INTERVAL_MS         16
#define TIMER_PEEK_HOLD              1004
#define TIMER_DISMISS_TOAST          1005

// ─────────────────────────────────────────────────────────────────────────────
// Registered Window Messages
// ─────────────────────────────────────────────────────────────────────────────
static UINT g_msgRefreshTimer      = 0;
static UINT g_msgModeCycle         = 0;
static UINT g_msgMiddleClickToggle = 0;
static UINT g_msgAutoHide          = 0;
static UINT g_msgAutoRestore       = 0;

// ─────────────────────────────────────────────────────────────────────────────
// 4-State Machine Architecture
// ─────────────────────────────────────────────────────────────────────────────
enum DesktopIconState {
    STATE_AUTO_HIDDEN    = 0, // Auto mode: unpinned icons hidden, awaiting section hover
    STATE_AUTO_REVEALED  = 1, // Auto mode: unpinned icons revealed due to section hover or peek
    STATE_PINNED_VISIBLE = 2, // All icons pinned visible indefinitely (auto-hide frozen)
    STATE_PINNED_HIDDEN  = 3  // All unpinned icons locked hidden indefinitely (hover reveal disabled)
};

static DesktopIconState g_currentState = STATE_AUTO_HIDDEN;

// ─────────────────────────────────────────────────────────────────────────────
// Global Settings
// ─────────────────────────────────────────────────────────────────────────────
struct Settings {
    bool enableAutoHide;
    int  initialMode;          // 0 = Auto, 1 = Pinned Show, 2 = Pinned Hide
    bool threeWayCycle;
    bool middleClickToggle;
    bool peekMode;
    int  peekDelayMs;          // 100 to 1000
    bool smoothFade;
    int  fadeDurationMs;       // 50 to 1000
    int  anchorSide;           // 0 = Left, 1 = Right, 2 = Top, 3 = Custom
    bool autoDetectBoundary;
    int  fixedBoundaryWidth;
    int  boundaryMargin;
    int  autoHideDelay;        // seconds, 1 to 60
    int  leaveDelayMs;         // ms, 100 to 3000
    bool focusShield;
    bool whitelistPinned;
    bool whitelistRecycleBin;
    bool whitelistThisPC;
    bool showPinnedPreviewToast;
    bool audioFeedback;
} g_settings;

static std::vector<std::wstring> g_customPinnedNames;

// ─────────────────────────────────────────────────────────────────────────────
// High-Performance Cached Whitelist & Item Architecture
// ─────────────────────────────────────────────────────────────────────────────
struct WhitelistedItem {
    int index;
    std::wstring canonicalName;
    RECT rcBounds;
    RECT rcIcon;
    RECT rcLabel;
};

struct UnpinnedBounds {
    RECT rcBoundingBox;
    int count;
};

static std::vector<WhitelistedItem> g_whitelistedItems;
static UnpinnedBounds g_unpinnedBounds = {};

// Forward Declarations
LRESULT CALLBACK DesktopListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK DesktopShellViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
BOOL    CALLBACK EnumWindowsProc(HWND, LPARAM);
static void SetupDesktopView(HWND hwndShell, HWND hwndListView);
static void RepaintDesktop(HWND hListView, bool erase = false);
static void RefreshDesktopItemsCache(HWND hListView);
static bool IsCursorOverOrNearPinnedItem(HWND hwndListView, POINT ptClient, int margin = 24);
static void LogAndPreviewWhitelistedItems(HWND hListView, bool showNotification);

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Real_CreateWindowExW = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// Smooth 60 FPS Fluent Eased Fade State
// ─────────────────────────────────────────────────────────────────────────────
static int           g_targetOpacity        = 100; // 0 to 100
static int           g_currentOpacity       = 100; // 0 to 100
static int           g_startOpacity         = 100;
static LARGE_INTEGER g_qpcFreq              = {};
static LARGE_INTEGER g_qpcFadeStart         = {};
static double        g_fadeDurationSec      = 0.22;
static bool          g_isFading             = false;
static bool          g_timerPrecisionActive = false;

static thread_local bool g_inDesktopPaint          = false;
static thread_local bool g_inDirectHook             = false;
static thread_local bool g_inTextHook               = false;
static thread_local bool g_isWhitelistedItemDrawing = false;

static HWND g_hDesktopListView = NULL;

// Whitelist system name cache
static WCHAR g_szRecycleBinName[MAX_PATH] = L"";
static WCHAR g_szThisPCName[MAX_PATH]     = L"";

// KnownFolder GUIDs defined locally to eliminate external linker dependencies
// FOLDERID_RecycleBinFolder: {B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC}
static const GUID CLSID_LocalRecycleBin =
    { 0xB7534046, 0x3ECB, 0x4C18, { 0xBE, 0x4E, 0x64, 0xCD, 0x4C, 0xB7, 0xD6, 0xAC } };

// FOLDERID_ComputerFolder: {0AC0837C-BBF8-452A-850D-79D08E667CA7}
static const GUID CLSID_LocalComputer =
    { 0x0AC0837C, 0xBBF8, 0x452A, { 0x85, 0x0D, 0x79, 0xD0, 0x8E, 0x66, 0x7C, 0xA7 } };

static void InitWhitelistNames()
{
    // Recycle Bin display name
    PIDLIST_ABSOLUTE pidlRecycle = NULL;
    if (SUCCEEDED(SHGetKnownFolderIDList(CLSID_LocalRecycleBin, 0, NULL, &pidlRecycle)) && pidlRecycle) {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW((LPCWSTR)pidlRecycle, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME)) {
            wcsncpy_s(g_szRecycleBinName, sfi.szDisplayName, _TRUNCATE);
        }
        CoTaskMemFree(pidlRecycle);
    }
    if (g_szRecycleBinName[0] == L'\0') {
        wcscpy_s(g_szRecycleBinName, L"Recycle Bin");
    }

    // This PC display name
    PIDLIST_ABSOLUTE pidlComputer = NULL;
    if (SUCCEEDED(SHGetKnownFolderIDList(CLSID_LocalComputer, 0, NULL, &pidlComputer)) && pidlComputer) {
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

// ─────────────────────────────────────────────────────────────────────────────
// Robust Multi-Line & Formatted Text Normalization
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

    // Replace internal newlines (\r, \n) and tabs with space to support multi-line desktop labels
    for (int i = 0; buf[i]; ++i) {
        if (buf[i] == L'\r' || buf[i] == L'\n' || buf[i] == L'\t') {
            buf[i] = L' ';
        }
    }

    // Trim leading whitespace and quotes
    WCHAR* start = buf;
    while (*start == L' ' || *start == L'\"' || *start == L'\'') {
        start++;
    }
    int len = (int)wcslen(start);

    // Trim trailing whitespace, quotes, ellipsis (...) and Unicode horizontal ellipsis (U+2026)
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

    // Collapse multiple consecutive spaces
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

    // Create clean normalized version (strip .lnk / .url / .exe extension if present)
    std::wstring s = collapsed;
    if (s.length() >= 4) {
        std::wstring ext = s.substr(s.length() - 4);
        if (_wcsicmp(ext.c_str(), L".lnk") == 0 || _wcsicmp(ext.c_str(), L".url") == 0 || _wcsicmp(ext.c_str(), L".exe") == 0) {
            s = s.substr(0, s.length() - 4);
        }
    }
    outClean = s;
    return true;
}

static bool MatchPattern(const std::wstring& text, const std::wstring& pattern)
{
    if (pattern.empty() || text.empty()) return false;
    // Exact match case-insensitive
    if (_wcsicmp(text.c_str(), pattern.c_str()) == 0) return true;

    std::wstring textLower = text;
    std::wstring patLower = pattern;
    std::transform(textLower.begin(), textLower.end(), textLower.begin(), ::towlower);
    std::transform(patLower.begin(), patLower.end(), patLower.begin(), ::towlower);

    if (textLower == patLower) return true;

    // Substring match case-insensitive if pattern length >= 3
    if (patLower.length() >= 3 && textLower.find(patLower) != std::wstring::npos) {
        return true;
    }
    // Reverse substring match if text length >= 3 (e.g. user typed "Google Chrome", text is "Chrome")
    if (textLower.length() >= 3 && patLower.find(textLower) != std::wstring::npos) {
        return true;
    }

    return false;
}

static bool IsTextWhitelisted(LPCWSTR pszText, int cch = -1)
{
    if (!pszText || !g_settings.whitelistPinned) return false;

    std::wstring clean, raw;
    if (!CleanAndNormalizeText(pszText, cch, clean, raw)) return false;

    // 1. Recycle Bin (Localized system display name and common fallbacks)
    if (g_settings.whitelistRecycleBin) {
        if (g_szRecycleBinName[0] && _wcsicmp(clean.c_str(), g_szRecycleBinName) == 0) return true;
        if (_wcsicmp(clean.c_str(), L"Recycle Bin") == 0 || _wcsicmp(clean.c_str(), L"Corbeille") == 0 ||
            _wcsicmp(clean.c_str(), L"Papierkorb") == 0 || _wcsicmp(clean.c_str(), L"Papelera de reciclaje") == 0 ||
            _wcsicmp(clean.c_str(), L"Cestino") == 0 || _wcsicmp(clean.c_str(), L"Lixeira") == 0 ||
            _wcsicmp(clean.c_str(), L"Корзина") == 0 || _wcsicmp(clean.c_str(), L"ゴミ箱") == 0 ||
            _wcsicmp(clean.c_str(), L"휴지통") == 0 || _wcsicmp(clean.c_str(), L"回收站") == 0) return true;
    }

    // 2. This PC / Computer (Localized system display name and common fallbacks)
    if (g_settings.whitelistThisPC) {
        if (g_szThisPCName[0] && _wcsicmp(clean.c_str(), g_szThisPCName) == 0) return true;
        if (_wcsicmp(clean.c_str(), L"This PC") == 0 || _wcsicmp(clean.c_str(), L"Computer") == 0 ||
            _wcsicmp(clean.c_str(), L"Ce PC") == 0 || _wcsicmp(clean.c_str(), L"Dieser PC") == 0 ||
            _wcsicmp(clean.c_str(), L"Este equipo") == 0 || _wcsicmp(clean.c_str(), L"Questo PC") == 0 ||
            _wcsicmp(clean.c_str(), L"Este Computador") == 0 || _wcsicmp(clean.c_str(), L"Этот компьютер") == 0 ||
            _wcsicmp(clean.c_str(), L"PC") == 0 || _wcsicmp(clean.c_str(), L"此电脑") == 0) return true;
    }

    // 3. Custom user pinned apps & shortcuts list
    for (const auto& pattern : g_customPinnedNames) {
        if (MatchPattern(clean, pattern) || MatchPattern(raw, pattern)) {
            return true;
        }
    }

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Desktop Items & Whitelist Cache (Runs outside of paint loop)
// ─────────────────────────────────────────────────────────────────────────────
static void RefreshDesktopItemsCache(HWND hListView)
{
    g_whitelistedItems.clear();
    g_unpinnedBounds = {};
    if (!hListView || !IsWindow(hListView)) return;

    int totalCount = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
    if (totalCount <= 0) return;

    int minLeft = 999999, minTop = 999999, maxRight = -999999, maxBottom = -999999;
    int unpinnedCount = 0;

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

        bool whitelisted = (g_settings.whitelistPinned && text[0] != L'\0' && IsTextWhitelisted(text));

        RECT rcBounds = { LVIR_BOUNDS, 0, 0, 0 };
        RECT rcIcon   = { LVIR_ICON, 0, 0, 0 };
        RECT rcLabel  = { LVIR_LABEL, 0, 0, 0 };
        SendMessageW(hListView, LVM_GETITEMRECT, i, (LPARAM)&rcBounds);
        SendMessageW(hListView, LVM_GETITEMRECT, i, (LPARAM)&rcIcon);
        SendMessageW(hListView, LVM_GETITEMRECT, i, (LPARAM)&rcLabel);

        if (whitelisted) {
            WhitelistedItem wi;
            wi.index = i;
            wi.canonicalName = text;
            wi.rcBounds = rcBounds;
            wi.rcIcon = rcIcon;
            wi.rcLabel = rcLabel;
            g_whitelistedItems.push_back(wi);
        } else {
            if (rcBounds.right > rcBounds.left && rcBounds.bottom > rcBounds.top) {
                if (rcBounds.left < minLeft)     minLeft = rcBounds.left;
                if (rcBounds.top < minTop)       minTop = rcBounds.top;
                if (rcBounds.right > maxRight)   maxRight = rcBounds.right;
                if (rcBounds.bottom > maxBottom) maxBottom = rcBounds.bottom;
                unpinnedCount++;
            }
        }
    }

    if (unpinnedCount > 0) {
        g_unpinnedBounds.rcBoundingBox = { minLeft, minTop, maxRight, maxBottom };
        g_unpinnedBounds.count = unpinnedCount;
    }
}

static bool IsItemWhitelisted(HWND hListView, int iItem)
{
    if (!g_settings.whitelistPinned || iItem < 0) return false;
    for (const auto& item : g_whitelistedItems) {
        if (item.index == iItem) return true;
    }
    // Dynamic text fallback if cache indices shifted or haven't refreshed yet
    if (hListView && IsWindow(hListView)) {
        WCHAR text[MAX_PATH] = {};
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = iItem;
        item.iSubItem = 0;
        item.pszText = text;
        item.cchTextMax = MAX_PATH;
        if (SendMessageW(hListView, LVM_GETITEMTEXTW, iItem, (LPARAM)&item) > 0 || text[0] != L'\0') {
            return IsTextWhitelisted(text);
        }
    }
    return false;
}

static bool IsCurrentIconWhitelisted(const IMAGELISTDRAWPARAMS* pimldp)
{
    if (!g_settings.whitelistPinned || !pimldp || g_whitelistedItems.empty()) return false;

    int cx = pimldp->cx > 0 ? (int)pimldp->cx : 32;
    int cy = pimldp->cy > 0 ? (int)pimldp->cy : 32;
    POINT ptCenter = { pimldp->x + cx / 2, pimldp->y + cy / 2 };
    POINT ptOrigin = { pimldp->x, pimldp->y };
    RECT rcDraw = { pimldp->x, pimldp->y, pimldp->x + cx, pimldp->y + cy };

    for (const auto& item : g_whitelistedItems) {
        RECT rcInflatedBounds = item.rcBounds;
        InflateRect(&rcInflatedBounds, 64, 64);
        RECT rcInflatedIcon = item.rcIcon;
        InflateRect(&rcInflatedIcon, 64, 64);

        RECT rcOverlap = {};
        if (IntersectRect(&rcOverlap, &rcInflatedBounds, &rcDraw) ||
            IntersectRect(&rcOverlap, &rcInflatedIcon, &rcDraw) ||
            PtInRect(&rcInflatedBounds, ptCenter) ||
            PtInRect(&rcInflatedIcon, ptCenter) ||
            PtInRect(&rcInflatedBounds, ptOrigin) ||
            (abs(item.rcIcon.left - pimldp->x) <= 64 && abs(item.rcIcon.top - pimldp->y) <= 64)) {
            return true;
        }
    }
    return false;
}

static bool IsCurrentLabelWhitelisted(LPCWSTR pszText, int cch, const RECT* prc)
{
    if (!g_settings.whitelistPinned || g_whitelistedItems.empty()) return false;

    if (prc) {
        POINT ptCenter = { (prc->left + prc->right) / 2, (prc->top + prc->bottom) / 2 };
        POINT ptOrigin = { prc->left, prc->top };
        for (const auto& item : g_whitelistedItems) {
            RECT rcInflatedBounds = item.rcBounds;
            InflateRect(&rcInflatedBounds, 64, 64);
            RECT rcInflatedLabel = item.rcLabel;
            InflateRect(&rcInflatedLabel, 64, 64);
            RECT rcOverlap = {};
            if (IntersectRect(&rcOverlap, &rcInflatedBounds, prc) ||
                IntersectRect(&rcOverlap, &rcInflatedLabel, prc) ||
                PtInRect(&rcInflatedBounds, ptCenter) ||
                PtInRect(&rcInflatedLabel, ptCenter) ||
                PtInRect(&rcInflatedBounds, ptOrigin)) {
                return true;
            }
        }
    }
    return IsTextWhitelisted(pszText, cch);
}

static bool IsCursorOverOrNearPinnedItem(HWND /*hwndListView*/, POINT ptClient, int margin)
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
// Real-Time Pinned Apps Audit & Notification Preview
// ─────────────────────────────────────────────────────────────────────────────
static void LogAndPreviewWhitelistedItems(HWND hListView, bool showNotification)
{
    if (!hListView || !IsWindow(hListView)) return;

    RefreshDesktopItemsCache(hListView);

    int totalCount = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
    int pinnedCount = (int)g_whitelistedItems.size();

    Wh_Log(L"[SectionAutoHide] ═══════════════ PINNED APPS AUDIT ═══════════════");
    Wh_Log(L"[SectionAutoHide] Specific App Pinning Enabled: %s", g_settings.whitelistPinned ? L"YES" : L"NO (Whitelist Disabled)");
    Wh_Log(L"[SectionAutoHide] Recycle Bin: %s, This PC: %s, Custom Rules: %zu",
           g_settings.whitelistRecycleBin ? L"ON" : L"OFF",
           g_settings.whitelistThisPC ? L"ON" : L"OFF",
           g_customPinnedNames.size());

    std::wstring pinnedSummaryList;

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

        bool isPinned = false;
        for (const auto& wItem : g_whitelistedItems) {
            if (wItem.index == i) {
                isPinned = true;
                break;
            }
        }

        if (isPinned) {
            Wh_Log(L"[SectionAutoHide]   [#%02d] [PINNED]    \"%s\"", i + 1, text);
            if (!pinnedSummaryList.empty()) pinnedSummaryList += L", ";
            pinnedSummaryList += text;
        } else {
            Wh_Log(L"[SectionAutoHide]   [#%02d] [AUTO-HIDE] \"%s\"", i + 1, text);
        }
    }

    Wh_Log(L"[SectionAutoHide] Total Desktop Icons: %d | Pinned: %d | Auto-Hide: %d",
           totalCount, pinnedCount, totalCount - pinnedCount);
    Wh_Log(L"[SectionAutoHide] ══════════════════════════════════════════════════");

    // Display Windows Notification Banner if enabled
    if (showNotification && g_settings.showPinnedPreviewToast && g_settings.whitelistPinned) {
        HWND hwndShell = GetShellWindow();
        if (!hwndShell || !IsWindow(hwndShell)) hwndShell = GetParent(hListView);
        if (!hwndShell || !IsWindow(hwndShell)) hwndShell = hListView;

        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwndShell;
        nid.uID = 0x50494E; // 'PIN'
        nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
        nid.uCallbackMessage = WM_APP + 101;
        nid.hIcon = LoadIconW(NULL, IDI_INFORMATION);
        wcsncpy_s(nid.szTip, L"Windhawk Desktop Auto-Hide", _TRUNCATE);
        wcsncpy_s(nid.szInfoTitle, L"Pinned Desktop Apps Preview", _TRUNCATE);

        WCHAR infoMsg[512] = {};
        if (pinnedCount == 0) {
            swprintf_s(infoMsg, L"No apps currently pinned.\nAll %d desktop icons will auto-hide.\nAdd app names in Windhawk Settings -> Custom Pinned Apps.", totalCount);
        } else {
            swprintf_s(infoMsg, L"%d Pinned (Always-Visible):\n%s\n(%d other icons will auto-hide)",
                       pinnedCount, pinnedSummaryList.c_str(), totalCount - pinnedCount);
        }
        wcsncpy_s(nid.szInfo, infoMsg, _TRUNCATE);
        nid.dwInfoFlags = NIIF_INFO | NIIF_LARGE_ICON;

        Shell_NotifyIconW(NIM_DELETE, &nid);
        Shell_NotifyIconW(NIM_ADD, &nid);

        SetTimer(hwndShell, TIMER_DISMISS_TOAST, 5000, NULL);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Audio Feedback (Windows 11 Native Chimes)
// ─────────────────────────────────────────────────────────────────────────────
static void PlayDesktopSound(int soundEvent)
{
    if (!g_settings.audioFeedback) return;

    LPCWSTR pszSound = nullptr;
    switch (soundEvent) {
        case 0: pszSound = L"DeviceConnect";        break; // Show All
        case 1: pszSound = L"DeviceDisconnect";     break; // Hide All
        case 2: pszSound = L"Notification.Default"; break; // Auto Mode
        case 3: pszSound = L"CCSelect";             break; // Peek / Quick Toggle / Preview
        default: pszSound = L"Notification.Default";break;
    }
    if (!PlaySoundW(pszSound, NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT)) {
        if (soundEvent == 3) {
            PlaySoundW(L"Notification.Default", NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Desktop Verification Helpers
// ─────────────────────────────────────────────────────────────────────────────
static bool IsDesktopParent(HWND hWnd)
{
    if (!hWnd) return false;
    if (hWnd == GetShellWindow()) return true;

    WCHAR className[64] = {};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Progman") == 0 || _wcsicmp(className, L"WorkerW") == 0) {
            return true;
        }
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Load Settings
// ─────────────────────────────────────────────────────────────────────────────
static void LoadSettings()
{
    g_settings.enableAutoHide = Wh_GetIntSetting(L"enableAutoHide") != 0;

    PCWSTR strMode = Wh_GetStringSetting(L"initialMode");
    if (strMode) {
        if (wcscmp(strMode, L"pinned_show") == 0) {
            g_settings.initialMode = 1;
        } else if (wcscmp(strMode, L"pinned_hide") == 0) {
            g_settings.initialMode = 2;
        } else {
            g_settings.initialMode = 0; // auto
        }
        Wh_FreeStringSetting(strMode);
    } else {
        g_settings.initialMode = 0;
    }

    g_settings.threeWayCycle      = Wh_GetIntSetting(L"threeWayCycle") != 0;
    g_settings.middleClickToggle  = Wh_GetIntSetting(L"middleClickToggle") != 0;
    g_settings.peekMode           = Wh_GetIntSetting(L"peekMode") != 0;
    g_settings.peekDelayMs        = std::clamp(Wh_GetIntSetting(L"peekDelayMs"), 100, 1000);
    g_settings.smoothFade         = Wh_GetIntSetting(L"smoothFade") != 0;
    g_settings.fadeDurationMs     = std::clamp(Wh_GetIntSetting(L"fadeDurationMs"), 50, 1000);

    PCWSTR strAnchor = Wh_GetStringSetting(L"anchorSide");
    if (strAnchor) {
        if (wcscmp(strAnchor, L"right") == 0) {
            g_settings.anchorSide = 1;
        } else if (wcscmp(strAnchor, L"top") == 0) {
            g_settings.anchorSide = 2;
        } else if (wcscmp(strAnchor, L"custom") == 0) {
            g_settings.anchorSide = 3;
        } else {
            g_settings.anchorSide = 0; // left
        }
        Wh_FreeStringSetting(strAnchor);
    } else {
        g_settings.anchorSide = 0;
    }

    g_settings.autoDetectBoundary     = Wh_GetIntSetting(L"autoDetectBoundary") != 0;
    g_settings.fixedBoundaryWidth     = std::clamp(Wh_GetIntSetting(L"fixedBoundaryWidth"), 100, 3840);
    g_settings.boundaryMargin         = std::clamp(Wh_GetIntSetting(L"boundaryMargin"), 0, 500);
    g_settings.autoHideDelay          = std::clamp(Wh_GetIntSetting(L"autoHideDelay"), 1, 60);
    g_settings.leaveDelayMs           = std::clamp(Wh_GetIntSetting(L"leaveDelayMs"), 100, 3000);
    g_settings.focusShield            = Wh_GetIntSetting(L"focusShield") != 0;
    g_settings.whitelistPinned        = Wh_GetIntSetting(L"whitelistPinned") != 0;
    g_settings.whitelistRecycleBin    = Wh_GetIntSetting(L"whitelistRecycleBin") != 0;
    g_settings.whitelistThisPC        = Wh_GetIntSetting(L"whitelistThisPC") != 0;
    g_settings.showPinnedPreviewToast = Wh_GetIntSetting(L"showPinnedPreviewToast") != 0;
    g_settings.audioFeedback          = Wh_GetIntSetting(L"audioFeedback") != 0;

    // Parse custom pinned app list (comma, semicolon, or newline separated)
    g_customPinnedNames.clear();
    PCWSTR strPinned = Wh_GetStringSetting(L"pinnedAppList");
    if (strPinned) {
        std::wstring s = strPinned;
        size_t start = 0;
        while (start < s.length()) {
            size_t delim = s.find_first_of(L",;\r\n", start);
            if (delim == std::wstring::npos) delim = s.length();
            std::wstring token = s.substr(start, delim - start);
            size_t first = token.find_first_not_of(L" \t\r\n\"'");
            size_t last = token.find_last_not_of(L" \t\r\n\"'");
            if (first != std::wstring::npos && last != std::wstring::npos) {
                token = token.substr(first, last - first + 1);
                if (!token.empty()) {
                    g_customPinnedNames.push_back(token);
                }
            }
            start = delim + 1;
        }
        Wh_FreeStringSetting(strPinned);
    }

    InitWhitelistNames();

    Wh_Log(L"[SectionAutoHide] Settings: autoHide=%d, mode=%d, smoothFade=%d, fadeDuration=%dms, anchor=%d, whitelist=%d, customPinnedCount=%zu",
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
// Multi-Anchor Section Boundary Helper (Scoped strictly to unpinned icons)
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

    // If all icons are whitelisted/pinned, no unpinned auto-hide trigger rect is needed
    if (g_unpinnedBounds.count == 0 && !g_whitelistedItems.empty()) {
        return { 0, 0, 0, 0 };
    }

    int minLeft   = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.left : 0;
    int minTop    = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.top : 0;
    int maxRight  = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.right : g_settings.fixedBoundaryWidth;
    int maxBottom = g_unpinnedBounds.count > 0 ? g_unpinnedBounds.rcBoundingBox.bottom : viewHeight;

    RECT rcTrigger = {};

    switch (g_settings.anchorSide) {
        case 1: // Right edge
        {
            int triggerLeft = 0;
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && minLeft < viewWidth) {
                triggerLeft = minLeft - g_settings.boundaryMargin;
            } else {
                triggerLeft = viewWidth - g_settings.fixedBoundaryWidth - g_settings.boundaryMargin;
            }
            if (triggerLeft < 0) triggerLeft = 0;
            int triggerTop = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::max(0, minTop - g_settings.boundaryMargin) : 0;
            int triggerBottom = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::min(viewHeight, maxBottom + g_settings.boundaryMargin) : viewHeight;
            rcTrigger = { triggerLeft, triggerTop, viewWidth, triggerBottom };
            break;
        }
        case 2: // Top edge
        {
            int triggerBottom = 0;
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && maxBottom > 0) {
                triggerBottom = maxBottom + g_settings.boundaryMargin;
            } else {
                triggerBottom = g_settings.fixedBoundaryWidth + g_settings.boundaryMargin;
            }
            if (triggerBottom > viewHeight) triggerBottom = viewHeight;
            int triggerLeft = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::max(0, minLeft - g_settings.boundaryMargin) : 0;
            int triggerRight = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::min(viewWidth, maxRight + g_settings.boundaryMargin) : viewWidth;
            rcTrigger = { triggerLeft, 0, triggerRight, triggerBottom };
            break;
        }
        case 3: // Custom bounding box
        {
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && maxRight > 0) {
                rcTrigger = {
                    std::max<int>(0, minLeft - g_settings.boundaryMargin),
                    std::max<int>(0, minTop - g_settings.boundaryMargin),
                    std::min<int>(viewWidth, maxRight + g_settings.boundaryMargin),
                    std::min<int>(viewHeight, maxBottom + g_settings.boundaryMargin)
                };
            } else {
                rcTrigger = { 0, 0, g_settings.fixedBoundaryWidth + g_settings.boundaryMargin, viewHeight };
            }
            break;
        }
        case 0: // Left edge (default)
        default:
        {
            int triggerRight = 0;
            if (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0 && maxRight > 0) {
                triggerRight = maxRight + g_settings.boundaryMargin;
            } else {
                triggerRight = g_settings.fixedBoundaryWidth + g_settings.boundaryMargin;
            }
            if (triggerRight > viewWidth) triggerRight = viewWidth;
            int triggerTop = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::max(0, minTop - g_settings.boundaryMargin) : 0;
            int triggerBottom = (g_settings.autoDetectBoundary && g_unpinnedBounds.count > 0) ? std::min(viewHeight, maxBottom + g_settings.boundaryMargin) : viewHeight;
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

    // If cursor is over or near a whitelisted pinned icon, it NEVER triggers section reveal of unpinned icons!
    if (IsCursorOverOrNearPinnedItem(hList, ptClient, 24)) {
        return false;
    }

    // If there are no unpinned items, nothing needs to reveal
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

    // Check rename shield
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

    // Check drag / marquee shields: verify mouse button is actually still physically held
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
    if (lastClickTime > 0 &&
        (now - lastClickTime) <= dblClickTime &&
        abs(ptScreen.x - lastClickX) <= maxDeltaX &&
        abs(ptScreen.y - lastClickY) <= maxDeltaY)
    {
        isDblClick = true;
        SetPropW(hwndDefView, L"ZenClickTime", 0);
    } else {
        SetPropW(hwndDefView, L"ZenClickTime", UlongToHandle(now));
        SetPropW(hwndDefView, L"ZenClickX", UlongToHandle((DWORD)(short)ptScreen.x));
        SetPropW(hwndDefView, L"ZenClickY", UlongToHandle((DWORD)(short)ptScreen.y));
    }

    return isDblClick;
}

// ─────────────────────────────────────────────────────────────────────────────
// Smooth 60 FPS Fluent Eased Alpha Transition Engine
// ─────────────────────────────────────────────────────────────────────────────
static void RepaintDesktop(HWND hListView, bool erase)
{
    if (!hListView || !IsWindow(hListView)) return;
    // Always pass erase = FALSE to prevent wallpaper erase flicker and background redraw stutter
    InvalidateRect(hListView, NULL, erase ? TRUE : FALSE);
    UpdateWindow(hListView);
}

static void StartFadeTransition(HWND hwndShellView, int targetOpacity)
{
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    if (!hwndListView) return;

    if (!IsWindowVisible(hwndListView)) {
        ShowWindow(hwndListView, SW_SHOW);
    }

    if (!g_settings.smoothFade) {
        g_currentOpacity = targetOpacity;
        g_targetOpacity  = targetOpacity;
        g_isFading       = false;
        KillTimer(hwndShellView, TIMER_FADE_ANIMATION);
        if (g_timerPrecisionActive) {
            timeEndPeriod(1);
            g_timerPrecisionActive = false;
        }
        RepaintDesktop(hwndListView, false);
        return;
    }

    if (g_targetOpacity == targetOpacity && g_isFading) {
        return; // Already animating smoothly towards this target
    }

    g_startOpacity  = g_currentOpacity;
    g_targetOpacity = targetOpacity;

    // Proportional duration for fluid interruptions
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
            // Auto -> Show All (Indefinite visibility of all icons)
            g_currentState = STATE_PINNED_VISIBLE;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 100);
            PlayDesktopSound(0);
            Wh_Log(L"[SectionAutoHide] Mode -> SHOW_ALL (indefinite visibility of all icons)");
        }
        else if (g_currentState == STATE_PINNED_VISIBLE) {
            // Show All -> Hide All (Completely locked hidden)
            g_currentState = STATE_PINNED_HIDDEN;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 0);
            PlayDesktopSound(1);
            Wh_Log(L"[SectionAutoHide] Mode -> HIDE_ALL (all unpinned icons locked hidden)");
        }
        else {
            // Hide All -> Auto Mode
            PlayDesktopSound(2);
            bool inside = IsCursorInTriggerSection(hwndShellView, hwndListView);
            if (inside) {
                g_currentState = STATE_AUTO_REVEALED;
                DWORD now = GetTickCount();
                SetPropW(hwndShellView, L"ZenLastInsideTick", UlongToHandle(now));
                SetPropW(hwndShellView, L"ZenLastActiveTick", UlongToHandle(now));
                StartFadeTransition(hwndShellView, 100);
                if (g_settings.enableAutoHide) {
                    SetTimer(hwndShellView, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
                }
            } else {
                g_currentState = STATE_AUTO_HIDDEN;
                StartFadeTransition(hwndShellView, 0);
                KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            }
            Wh_Log(L"[SectionAutoHide] Mode -> AUTO (inside=%d)", (int)inside);
        }
    } else {
        // Simple Toggle: Show <-> Hide
        if (g_currentState == STATE_PINNED_VISIBLE || g_currentState == STATE_AUTO_REVEALED || g_currentOpacity > 0) {
            g_currentState = STATE_PINNED_HIDDEN;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 0);
            PlayDesktopSound(1);
            Wh_Log(L"[SectionAutoHide] Toggle -> HIDE_ALL");
        } else {
            g_currentState = STATE_PINNED_VISIBLE;
            KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
            StartFadeTransition(hwndShellView, 100);
            PlayDesktopSound(0);
            Wh_Log(L"[SectionAutoHide] Toggle -> SHOW_ALL");
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
        Wh_Log(L"[SectionAutoHide] QuickToggle (MiddleClick) -> HIDE_ALL");
    } else {
        g_currentState = STATE_PINNED_VISIBLE;
        KillTimer(hwndShellView, TIMER_TRACK_CURSOR);
        StartFadeTransition(hwndShellView, 100);
        PlayDesktopSound(0);
        Wh_Log(L"[SectionAutoHide] QuickToggle (MiddleClick) -> SHOW_ALL");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Reusable High-Performance GDI Memory DC & Alpha Blending Cache
// ─────────────────────────────────────────────────────────────────────────────
struct CachedTextDC {
    HDC hdcMem = NULL;
    HBITMAP hBmp = NULL;
    int curW = 0;
    int curH = 0;

    void EnsureSize(HDC hdcRef, int w, int h) {
        if (!hdcMem) {
            hdcMem = CreateCompatibleDC(hdcRef);
        }
        if (!hBmp || w > curW || h > curH) {
            if (hBmp) DeleteObject(hBmp);
            curW = std::max(w, curW > 0 ? curW : 512);
            curH = std::max(h, curH > 0 ? curH : 256);
            hBmp = CreateCompatibleBitmap(hdcRef, curW, curH);
            SelectObject(hdcMem, hBmp);
        }
    }

    void Cleanup() {
        if (hBmp) { DeleteObject(hBmp); hBmp = NULL; }
        if (hdcMem) { DeleteDC(hdcMem); hdcMem = NULL; }
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
        bool isWhitelisted = g_isWhitelistedItemDrawing;
        if (!isWhitelisted && g_settings.whitelistPinned) {
            isWhitelisted = IsCurrentIconWhitelisted(pimldp);
        }

        // Whitelisted pinned items always draw at 100% full opacity.
        // g_inDirectHook = true protects internal GdiAlphaBlend calls from getting suppressed!
        if (isWhitelisted) {
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
        bool isWhitelisted = g_isWhitelistedItemDrawing;
        if (!isWhitelisted && g_settings.whitelistPinned && !g_whitelistedItems.empty()) {
            POINT ptCenter = { xoriginDest + wDest / 2, yoriginDest + hDest / 2 };
            POINT ptOrigin = { xoriginDest, yoriginDest };
            RECT rcDest = { xoriginDest, yoriginDest, xoriginDest + wDest, yoriginDest + hDest };

            for (const auto& item : g_whitelistedItems) {
                RECT rcInflatedBounds = item.rcBounds;
                InflateRect(&rcInflatedBounds, 64, 64);
                RECT rcInflatedIcon = item.rcIcon;
                InflateRect(&rcInflatedIcon, 64, 64);

                RECT rcOverlap = {};
                if (IntersectRect(&rcOverlap, &rcInflatedBounds, &rcDest) ||
                    IntersectRect(&rcOverlap, &rcInflatedIcon, &rcDest) ||
                    PtInRect(&rcInflatedBounds, ptCenter) ||
                    PtInRect(&rcInflatedIcon, ptCenter) ||
                    PtInRect(&rcInflatedBounds, ptOrigin) ||
                    (abs(item.rcIcon.left - xoriginDest) <= 64 && abs(item.rcIcon.top - yoriginDest) <= 64))
                {
                    isWhitelisted = true;
                    break;
                }
            }
        }
        if (isWhitelisted) {
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
    if (!g_inDesktopPaint || g_inTextHook) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    if (dwFlags & DT_CALCRECT) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    bool isWhitelisted = g_settings.whitelistPinned && (g_isWhitelistedItemDrawing || IsCurrentLabelWhitelisted(pszText, (int)cch, prc));
    if (isWhitelisted) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    if (g_currentOpacity >= 100) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    if (g_currentOpacity <= 0) {
        return (prc ? (prc->bottom - prc->top) : 1);
    }

    if (!prc) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    int pad = 4;
    int x = prc->left - pad;
    int y = prc->top - pad;
    int w = (prc->right - prc->left) + pad * 2;
    int h = (prc->bottom - prc->top) + pad * 2;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }

    if (w <= 0 || h <= 0) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    tl_cachedDC.EnsureSize(hdc, w, h);
    if (!tl_cachedDC.hdcMem) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    HDC hMemDC = tl_cachedDC.hdcMem;
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);

    BitBlt(hMemDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

    RECT localRect = { pad, pad, pad + (prc->right - prc->left), pad + (prc->bottom - prc->top) };

    g_inTextHook = true;
    int result = DrawShadowText_Original(hMemDC, pszText, cch, &localRect, dwFlags, crText, crShadow, ixOffset, iyOffset);

    BLENDFUNCTION bf = {};
    bf.BlendOp             = AC_SRC_OVER;
    bf.SourceConstantAlpha = (BYTE)((g_currentOpacity * 255) / 100);

    AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, w, h, bf);
    g_inTextHook = false;

    SelectObject(hMemDC, hOldFont);
    return result;
}

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original = nullptr;

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    if (!g_inDesktopPaint || g_inTextHook) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (format & DT_CALCRECT) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    bool isWhitelisted = g_settings.whitelistPinned && (g_isWhitelistedItemDrawing || IsCurrentLabelWhitelisted(lpchText, cchText, lprc));
    if (isWhitelisted) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (g_currentOpacity >= 100) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (g_currentOpacity <= 0) {
        return (lprc ? (lprc->bottom - lprc->top) : 1);
    }

    if (!lprc) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    int pad = 4;
    int x = lprc->left - pad;
    int y = lprc->top - pad;
    int w = (lprc->right - lprc->left) + pad * 2;
    int h = (lprc->bottom - lprc->top) + pad * 2;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }

    if (w <= 0 || h <= 0) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    tl_cachedDC.EnsureSize(hdc, w, h);
    if (!tl_cachedDC.hdcMem) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HDC hMemDC = tl_cachedDC.hdcMem;
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);

    SetTextColor(hMemDC, GetTextColor(hdc));
    SetBkMode(hMemDC, TRANSPARENT);

    BitBlt(hMemDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

    RECT localRect = { pad, pad, pad + (lprc->right - lprc->left), pad + (lprc->bottom - lprc->top) };

    g_inTextHook = true;
    int result = DrawTextW_Original(hMemDC, lpchText, cchText, &localRect, format);

    BLENDFUNCTION bf = {};
    bf.BlendOp             = AC_SRC_OVER;
    bf.SourceConstantAlpha = (BYTE)((g_currentOpacity * 255) / 100);

    AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, w, h, bf);
    g_inTextHook = false;

    SelectObject(hMemDC, hOldFont);
    return result;
}

using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t ExtTextOutW_Original = nullptr;

BOOL WINAPI ExtTextOutW_Hook(
    HDC hdc, int x, int y, UINT options, const RECT *lprect,
    LPCWSTR lpString, UINT c, const INT *lpDx
) {
    if (g_inDesktopPaint && !g_inTextHook) {
        if (g_settings.whitelistPinned && (g_isWhitelistedItemDrawing || IsCurrentLabelWhitelisted(lpString, (int)c, lprect))) {
            return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
        }

        if (g_currentOpacity <= 0) {
            return TRUE;
        }

        if (g_currentOpacity >= 100 || !lprect) {
            return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
        }

        int w = lprect->right - lprect->left;
        int h = lprect->bottom - lprect->top;
        if (w <= 0 || h <= 0) {
            return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
        }

        tl_cachedDC.EnsureSize(hdc, w, h);
        if (tl_cachedDC.hdcMem) {
            HDC hMemDC = tl_cachedDC.hdcMem;
            HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
            HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);
            SetTextColor(hMemDC, GetTextColor(hdc));
            SetBkMode(hMemDC, TRANSPARENT);

            BitBlt(hMemDC, 0, 0, w, h, hdc, lprect->left, lprect->top, SRCCOPY);

            RECT localRect = { 0, 0, w, h };
            int localX = x - lprect->left;
            int localY = y - lprect->top;

            g_inTextHook = true;
            ExtTextOutW_Original(hMemDC, localX, localY, options, &localRect, lpString, c, lpDx);

            BLENDFUNCTION bf = {};
            bf.BlendOp             = AC_SRC_OVER;
            bf.SourceConstantAlpha = (BYTE)((g_currentOpacity * 255) / 100);

            AlphaBlend(hdc, lprect->left, lprect->top, w, h, hMemDC, 0, 0, w, h, bf);
            g_inTextHook = false;

            SelectObject(hMemDC, hOldFont);
            return TRUE;
        }
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SysListView32 (Desktop Icon ListView)
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK DesktopListViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    // Paint hook activation: zero SendMessage overhead during paint!
    if (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) {
        g_hDesktopListView = hWnd;
        if (g_settings.whitelistPinned) {
            int currentItemCount = (int)SendMessageW(hWnd, LVM_GETITEMCOUNT, 0, 0);
            if (currentItemCount > 0 && (g_whitelistedItems.empty() || currentItemCount != (int)g_whitelistedItems.size() + g_unpinnedBounds.count)) {
                RefreshDesktopItemsCache(hWnd);
            }
        }
        g_inDesktopPaint = true;
        g_isWhitelistedItemDrawing = false;
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_inDesktopPaint = false;
        g_isWhitelistedItemDrawing = false;
        return result;
    }

    // Item-level whitelist tracking via ListView CustomDraw (if reflected)
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
                if (IsItemWhitelisted(hWnd, itemIdx)) {
                    g_isWhitelistedItemDrawing = true;
                } else {
                    g_isWhitelistedItemDrawing = false;
                }
                return lr | CDRF_NOTIFYPOSTPAINT;
            }
            if ((pnmcd->dwDrawStage & CDDS_ITEMPOSTPAINT) == CDDS_ITEMPOSTPAINT) {
                g_isWhitelistedItemDrawing = false;
                return lr;
            }
            return lr;
        }
    }

    if (uMsg == WM_NCDESTROY) {
        if (hWnd == g_hDesktopListView) g_hDesktopListView = NULL;
    }

    HWND hwndParent = GetParent(hWnd);
    POINT ptMouseClient = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
    bool isOverPinned = IsCursorOverOrNearPinnedItem(hWnd, ptMouseClient, 0);

    // Direct Interaction with Whitelisted Pinned Icons:
    // Clicks on pinned items must execute natively without waking unpinned dock or cycling mode!
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
            // (Strictly isolated: moving cursor over a pinned icon never restores unpinned icons!)
            if (g_currentState == STATE_AUTO_HIDDEN && g_settings.enableAutoHide) {
                if (!isOverPinned && IsCursorInTriggerSection(hwndParent, hWnd)) {
                    PostMessageW(hwndParent, g_msgAutoRestore, 0, 0);
                }
            }
        }
    }

    // Middle-click desktop quick toggle & Ctrl+Middle-Click Live Preview
    if (g_settings.middleClickToggle) {
        if (uMsg == WM_MBUTTONDOWN) {
            LVHITTESTINFO ht = {};
            ht.pt = ptMouseClient;
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            bool onEmptySpace = (ht.iItem == -1) ||
                                (g_currentOpacity == 0 && (!g_settings.whitelistPinned || !IsItemWhitelisted(hWnd, ht.iItem)));
            if (onEmptySpace) {
                return 0; // Eat middle mouse down on empty space to prevent scroll cursor
            }
        }
        else if (uMsg == WM_MBUTTONUP) {
            LVHITTESTINFO ht = {};
            ht.pt = ptMouseClient;
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            bool onEmptySpace = (ht.iItem == -1) ||
                                (g_currentOpacity == 0 && (!g_settings.whitelistPinned || !IsItemWhitelisted(hWnd, ht.iItem)));
            if (onEmptySpace) {
                if (hwndParent) {
                    if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
                        // Ctrl + Middle-Click: Show Pinned Apps Preview on demand!
                        LogAndPreviewWhitelistedItems(hWnd, true);
                        PlayDesktopSound(3);
                    } else {
                        PostMessageW(hwndParent, g_msgMiddleClickToggle, 0, 0);
                    }
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

        bool isOverWhitelisted = (ht.iItem != -1 && g_settings.whitelistPinned && IsItemWhitelisted(hWnd, ht.iItem));
        if (isOverWhitelisted || isOverPinned) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        bool onEmptySpace = (ht.iItem == -1) || (g_currentOpacity == 0);

        if (onEmptySpace && hwndParent) {
            // Peek Mode armed on wallpaper press
            if (g_settings.peekMode && (g_currentState == STATE_AUTO_HIDDEN || g_currentState == STATE_PINNED_HIDDEN || g_currentOpacity == 0)) {
                SetTimer(hwndParent, TIMER_PEEK_HOLD, g_settings.peekDelayMs, NULL);
                SetPropW(hwndParent, L"ZenPeekArmed", UlongToHandle(1));
            }

            // Unified Parent-Anchored Double-Click Check
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

            // If unpinned icons are hidden, prevent selecting or activating invisible icons
            if (g_currentOpacity == 0) {
                SendMessageW(hWnd, LVM_SETITEMSTATE, -1, 0);
                return 0;
            }
        }
    }

    // Context menu redirection when icons are hidden
    if (uMsg == WM_CONTEXTMENU && hwndParent) {
        POINT ptScreen = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        POINT ptClient = ptScreen;
        ScreenToClient(hWnd, &ptClient);

        if (IsCursorOverOrNearPinnedItem(hWnd, ptClient, 0)) {
            // Pinned item context menu works normally (Empty Recycle Bin, Open, Properties)
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
                Wh_Log(L"[SectionAutoHide] Peek released (ListView) -> hide");
                return 0;
            }

            // Clear drag/marquee focus shield
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
    // Async custom operations
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
            Wh_Log(L"[SectionAutoHide] AutoHide: unpinned icons hidden (zero wallpaper dimming)");
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
            Wh_Log(L"[SectionAutoHide] AutoRestore: unpinned icons revealed in icon section");
        }
        return 0;
    }

    if (uMsg == g_msgRefreshTimer) {
        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (hwndListView) {
            RefreshDesktopItemsCache(hwndListView);
        }
        if (hwndListView && (g_currentState == STATE_AUTO_REVEALED) && g_settings.enableAutoHide) {
            SetTimer(hWnd, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
        } else if (g_currentState != STATE_AUTO_REVEALED) {
            KillTimer(hWnd, TIMER_TRACK_CURSOR);
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
                HWND hwndListView = pnm->hwndFrom ? pnm->hwndFrom : g_hDesktopListView;
                if (IsItemWhitelisted(hwndListView, itemIdx)) {
                    g_isWhitelistedItemDrawing = true;
                } else {
                    g_isWhitelistedItemDrawing = false;
                }
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
    if (uMsg == WM_NOTIFY && g_settings.focusShield) {
        NMHDR* pnm = (NMHDR*)lParam;
        if (pnm) {
            if (pnm->code == LVN_BEGINLABELEDITW || pnm->code == LVN_BEGINLABELEDITA) {
                SetPropW(hWnd, L"ZenShieldRename", UlongToHandle(1));
                Wh_Log(L"[SectionAutoHide] FocusShield: Rename active");
            }
            else if (pnm->code == LVN_ENDLABELEDITW || pnm->code == LVN_ENDLABELEDITA) {
                RemovePropW(hWnd, L"ZenShieldRename");
                Wh_Log(L"[SectionAutoHide] FocusShield: Rename ended");
                HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
                if (hwndListView) RefreshDesktopItemsCache(hwndListView);
            }
            else if (pnm->code == LVN_BEGINDRAG || pnm->code == LVN_BEGINRDRAG) {
                SetPropW(hWnd, L"ZenShieldDrag", UlongToHandle(1));
                Wh_Log(L"[SectionAutoHide] FocusShield: Shortcut Drag active");
            }
            else if (pnm->code == LVN_MARQUEEBEGIN) {
                SetPropW(hWnd, L"ZenShieldMarquee", UlongToHandle(1));
                Wh_Log(L"[SectionAutoHide] FocusShield: Marquee selection active");
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

            // Fluent Quintic Smootherstep (Ken Perlin's C2 continuous curve)
            // Starts with zero initial velocity (no pop-in) and lands smoothly with zero final deceleration (no snap)
            float ease = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
            g_currentOpacity = (int)std::round(g_startOpacity + (g_targetOpacity - g_startOpacity) * ease);
            g_currentOpacity = std::clamp(g_currentOpacity, 0, 100);
        }

        // Safety check: ensure stuck peek mode is cleared if left mouse button was released
        if (GetPropW(hWnd, L"ZenIsPeeking") != NULL) {
            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
                RemovePropW(hWnd, L"ZenIsPeeking");
                StartFadeTransition(hWnd, 0);
                PlayDesktopSound(3);
                Wh_Log(L"[SectionAutoHide] Peek released (Safety Async Check) -> hide");
            }
        }

        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (hwndListView && IsWindow(hwndListView)) {
            if (g_currentOpacity != oldOpacity || finished) {
                RepaintDesktop(hwndListView, false);
            }
        }
        return 0;
    }

    // ── Toast Notification Auto-Dismiss Timer ─────────────────────────────────
    if (uMsg == WM_TIMER && wParam == TIMER_DISMISS_TOAST) {
        KillTimer(hWnd, TIMER_DISMISS_TOAST);
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hWnd;
        nid.uID = 0x50494E;
        Shell_NotifyIconW(NIM_DELETE, &nid);
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
                Wh_Log(L"[SectionAutoHide] Peek activated -> temporary reveal");
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

        // Full-screen application guard
        if (IsFullscreenWindowActive()) {
            return 0;
        }

        // Focus shield guard (rename, drag, marquee)
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

            // Check inactivity timeout while cursor is stationary inside the section
            if (g_settings.autoHideDelay > 0) {
                DWORD lastActive = HandleToUlong(GetPropW(hWnd, L"ZenLastActiveTick"));
                if (lastActive == 0) lastActive = now;
                if ((now - lastActive) >= (DWORD)g_settings.autoHideDelay * 1000) {
                    Wh_Log(L"[SectionAutoHide] Inactivity in icon section (%ums) -> AutoHide", now - lastActive);
                    PostMessageW(hWnd, g_msgAutoHide, 0, 0);
                }
            }
        } else {
            // Cursor is outside the icon section (or over/near a pinned icon)
            DWORD lastInside = HandleToUlong(GetPropW(hWnd, L"ZenLastInsideTick"));
            if (lastInside == 0) {
                SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
            } else if ((now - lastInside) >= (DWORD)g_settings.leaveDelayMs) {
                Wh_Log(L"[SectionAutoHide] Cursor left icon section for %ums -> AutoHide", now - lastInside);
                PostMessageW(hWnd, g_msgAutoHide, 0, 0);
            }
        }
        return 0;
    }

    // ── Mouse Activity & Hover Detection ─────────────────────────────────────
    if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN) {
        DWORD now = GetTickCount();
        SetPropW(hWnd, L"ZenLastInsideTick", UlongToHandle(now));
        SetPropW(hWnd, L"ZenLastActiveTick", UlongToHandle(now));

        if (uMsg == WM_MOUSEMOVE && g_currentState == STATE_AUTO_HIDDEN && g_settings.enableAutoHide) {
            HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
            if (IsCursorInTriggerSection(hWnd, hwndListView)) {
                Wh_Log(L"[SectionAutoHide] Cursor entered icon section -> AutoRestore");
                PostMessageW(hWnd, g_msgAutoRestore, 0, 0);
            }
        }
    }

    // ── Middle-Click Desktop Quick Toggle on Wallpaper ─────────────────────────
    if (g_settings.middleClickToggle) {
        if (uMsg == WM_MBUTTONDOWN) {
            return 0; // Prevent scroll cursor
        }
        if (uMsg == WM_MBUTTONUP) {
            if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
                HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
                if (hwndListView) {
                    LogAndPreviewWhitelistedItems(hwndListView, true);
                    PlayDesktopSound(3);
                }
            } else {
                PostMessageW(hWnd, g_msgMiddleClickToggle, 0, 0);
            }
            return 0;
        }
    }

    // ── Wallpaper Left-Button Press & Double-Click Cycle ───────────────────────
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
            Wh_Log(L"[SectionAutoHide] Peek released (DefView) -> hide");
            return 0;
        }

        RemovePropW(hWnd, L"ZenShieldDrag");
        RemovePropW(hWnd, L"ZenShieldMarquee");
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Shared Desktop View Setup
// ─────────────────────────────────────────────────────────────────────────────
static void SetupDesktopView(HWND hwndShell, HWND hwndListView)
{
    if (!hwndShell || !hwndListView) return;

    g_hDesktopListView = hwndListView;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc, 0);
    WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, DesktopListViewSubclassProc, 0);

    RefreshDesktopItemsCache(hwndListView);
    LogAndPreviewWhitelistedItems(hwndListView, false);

    if (!g_settings.enableAutoHide) {
        g_currentState = STATE_PINNED_VISIBLE;
        g_currentOpacity = 100;
        g_targetOpacity  = 100;
        RepaintDesktop(hwndListView, false);
        return;
    }

    if (g_settings.initialMode == 1) { // Show All
        g_currentState = STATE_PINNED_VISIBLE;
        g_currentOpacity = 100;
        g_targetOpacity  = 100;
        RepaintDesktop(hwndListView, false);
    } else if (g_settings.initialMode == 2) { // Hide All
        g_currentState = STATE_PINNED_HIDDEN;
        g_currentOpacity = 0;
        g_targetOpacity  = 0;
        RepaintDesktop(hwndListView, false);
    } else { // Auto Mode
        bool inside = IsCursorInTriggerSection(hwndShell, hwndListView);
        if (inside) {
            g_currentState = STATE_AUTO_REVEALED;
            g_currentOpacity = 100;
            g_targetOpacity  = 100;
            SetTimer(hwndShell, TIMER_TRACK_CURSOR, TIMER_TRACK_INTERVAL_MS, NULL);
        } else {
            g_currentState = STATE_AUTO_HIDDEN;
            g_currentOpacity = 0;
            g_targetOpacity  = 0;
        }
        RepaintDesktop(hwndListView, false);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Window Enumeration & Subclassing
// ─────────────────────────────────────────────────────────────────────────────
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
{
    WCHAR className[256] = {};
    if (!GetClassNameW(hWnd, className, 256)) return TRUE;

    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
        HWND hwndShell = FindWindowExW(hWnd, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) {
            HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            if (hwndListView) {
                Wh_Log(L"[SectionAutoHide] Found desktop: Shell=%p, ListView=%p (parent=%s)", hwndShell, hwndListView, className);
                SetupDesktopView(hwndShell, hwndListView);
            }
        }
    }
    return TRUE;
}

static void SubclassExistingWindows() { EnumWindows(EnumWindowsProc, 0); }

static void UnsubclassWindows()
{
    auto Cleanup = [](HWND hwndShell) {
        if (!hwndShell) return;
        KillTimer(hwndShell, TIMER_TRACK_CURSOR);
        KillTimer(hwndShell, TIMER_FADE_ANIMATION);
        KillTimer(hwndShell, TIMER_PEEK_HOLD);
        KillTimer(hwndShell, TIMER_DISMISS_TOAST);

        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwndShell;
        nid.uID = 0x50494E;
        Shell_NotifyIconW(NIM_DELETE, &nid);

        RemovePropW(hwndShell, L"ZenClickTime");
        RemovePropW(hwndShell, L"ZenClickX");
        RemovePropW(hwndShell, L"ZenClickY");
        RemovePropW(hwndShell, L"ZenLastInsideTick");
        RemovePropW(hwndShell, L"ZenLastActiveTick");
        RemovePropW(hwndShell, L"ZenShieldRename");
        RemovePropW(hwndShell, L"ZenShieldDrag");
        RemovePropW(hwndShell, L"ZenShieldMarquee");
        RemovePropW(hwndShell, L"ZenPeekArmed");
        RemovePropW(hwndShell, L"ZenIsPeeking");

        HWND lv = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (lv) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, DesktopListViewSubclassProc);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc);
    };

    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman)
        Cleanup(FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL));

    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL)
        Cleanup(FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL));

    if (g_timerPrecisionActive) {
        timeEndPeriod(1);
        g_timerPrecisionActive = false;
    }

    tl_cachedDC.Cleanup();

    g_currentOpacity = 100;
    g_targetOpacity  = 100;
    g_isFading       = false;
    if (g_hDesktopListView && IsWindow(g_hDesktopListView)) {
        if (!IsWindowVisible(g_hDesktopListView)) {
            ShowWindow(g_hDesktopListView, SW_SHOW);
        }
        RepaintDesktop(g_hDesktopListView, false);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// CreateWindowExW Hook for Dynamic Explorer Restarts
// ─────────────────────────────────────────────────────────────────────────────
HWND WINAPI Hook_CreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = Real_CreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
            // Strictly check that this SHELLDLL_DefView belongs to the desktop
            if (IsDesktopParent(hWndParent)) {
                Wh_Log(L"[SectionAutoHide] Hook: new desktop SHELLDLL_DefView=%p created", hWnd);
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopShellViewSubclassProc, 0);
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
                        Wh_Log(L"[SectionAutoHide] Hook: new desktop ListView=%p created", hWnd);
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
    Wh_Log(L"[SectionAutoHide] === Wh_ModInit v1.3.2 ===");

    QueryPerformanceFrequency(&g_qpcFreq);

    g_msgRefreshTimer      = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.RefreshTimer");
    g_msgModeCycle         = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.ModeCycle");
    g_msgMiddleClickToggle = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.MiddleClickToggle");
    g_msgAutoHide          = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.AutoHide");
    g_msgAutoRestore       = RegisterWindowMessageW(L"Windhawk.SectionAutoHide.DesktopIcon.AutoRestore");

    if (!g_msgRefreshTimer || !g_msgModeCycle || !g_msgMiddleClickToggle || !g_msgAutoHide || !g_msgAutoRestore) {
        Wh_Log(L"[SectionAutoHide] FAILED to register private window messages");
        return FALSE;
    }

    LoadSettings();

    // Hook CreateWindowExW for dynamic explorer restarts
    if (!Wh_SetFunctionHook(
            (void*)CreateWindowExW,
            (void*)Hook_CreateWindowExW,
            (void**)&Real_CreateWindowExW)) {
        Wh_Log(L"[SectionAutoHide] FAILED to hook CreateWindowExW");
        return FALSE;
    }

    // Hook native GDI icon and text drawing functions (Zero Wallpaper Dimming)
    Wh_SetFunctionHook(
        (void*)ImageList_DrawIndirect,
        (void*)ImageList_DrawIndirect_Hook,
        (void**)&ImageList_DrawIndirect_Original);

    HMODULE hGdi = GetModuleHandleW(L"gdi32full.dll");
    if (!hGdi) hGdi = GetModuleHandleW(L"gdi32.dll");
    if (hGdi) {
        void* pGdiAlphaBlend = (void*)GetProcAddress(hGdi, "GdiAlphaBlend");
        if (pGdiAlphaBlend) {
            Wh_SetFunctionHook(
                pGdiAlphaBlend,
                (void*)GdiAlphaBlend_Hook,
                (void**)&GdiAlphaBlend_Original);
        }

        void* pExtTextOutW = (void*)GetProcAddress(hGdi, "ExtTextOutW");
        if (pExtTextOutW) {
            Wh_SetFunctionHook(
                pExtTextOutW,
                (void*)ExtTextOutW_Hook,
                (void**)&ExtTextOutW_Original);
        }
    }

    HMODULE hComctl32 = GetModuleHandleW(L"comctl32.dll");
    if (hComctl32) {
        void* pDrawShadowText = (void*)GetProcAddress(hComctl32, "DrawShadowText");
        if (pDrawShadowText) {
            Wh_SetFunctionHook(
                pDrawShadowText,
                (void*)DrawShadowText_Hook,
                (void**)&DrawShadowText_Original);
        }
    }

    void* pDrawTextW = (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "DrawTextW");
    if (pDrawTextW) {
        Wh_SetFunctionHook(
            pDrawTextW,
            (void*)DrawTextW_Hook,
            (void**)&DrawTextW_Original);
    }

    SubclassExistingWindows();
    Wh_Log(L"[SectionAutoHide] Init complete (Fluent 60 FPS Engine, Custom App Pinning & 4-State Machine active)");
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"[SectionAutoHide] === Wh_ModUninit ===");
    UnsubclassWindows();
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"[SectionAutoHide] === Settings changed ===");
    LoadSettings();

    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman) {
        HWND hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell && g_msgRefreshTimer) PostMessageW(hwndShell, g_msgRefreshTimer, 0, 0);
    }
    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL) {
        HWND hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell && g_msgRefreshTimer) PostMessageW(hwndShell, g_msgRefreshTimer, 0, 0);
    }

    if (g_hDesktopListView && IsWindow(g_hDesktopListView)) {
        RefreshDesktopItemsCache(g_hDesktopListView);
        LogAndPreviewWhitelistedItems(g_hDesktopListView, true);
        RepaintDesktop(g_hDesktopListView, false);
    }
}
