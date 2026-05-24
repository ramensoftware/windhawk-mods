// ==WindhawkMod==
// @id              ultimate-custom-tray
// @name            Ultimate Custom Tray
// @description     This mod adds customizable system tray icons in Windows with configurable actions and context menus, support for image files and application icons, and automatic adaptation to the system theme.
// @description:ru-RU   Этот мод добавляет настраиваемые иконки в системный трей Windows с возможностью назначения действий и контекстных меню, поддержкой файлов изображений и иконок приложений, а также автоматической адаптацией под тему системы.
// @version         2.1
// @author          Salyts
// @license         MIT
// @github          https://github.com/Salyts
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -lshlwapi -lcomdlg32 -luuid -lgdiplus -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ultimate Custom Tray 2.1

This mod adds customizable system tray icons in Windows with configurable actions and context menus, support for image files and application icons, and automatic adaptation to the system theme.

### [> Russian documentation <](https://github.com/Salyts/Ultimate-Custom-Tray/blob/main/README_RU.md)

---

## Quick start

1. Open Windhawk settings for this mod.
2. Add one or more **Items** to the list.
3. Fill in **Label**, **Icon** and **Action** for each item.
4. Save - the icons appear in the tray immediately.

---

## Action formats

| Prefix | Example | Description |
|--------|---------|-------------|
| `" "` | `"C:\Program Files\Windhawk\windhawk.exe"` | Opens a file or folder by path. |
| `~` | `~Downloads` and `~windhawk.exe` | Opens a folder or file by name. |
| `cmd:` | `cmd:control` | Runs a command through `cmd.exe`. |
| `shell:` | `shell:shutdown /r /f /t 0` | Runs through `powershell.exe`. |
| `press:` | `press:Win+E` or `press:0x5B;0x45` | Keyboard key press using a [Win32 key code](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes). |
| `web:` | `web:https://windhawk.net/` | Opens a URL in the default browser. |
| `ms-settings:` | `ms-settings:bluetooth` | Opens a Windows Settings page. |

### Modifier signs (prepend to any action)

| Sign | Example | Description |
|--------|---------|-------------|
| `-` | `-"C:\Program Files\app.exe"` or `-shell:shutdown /r /f /t 0` | Runs as administrator. |
| `*` | `*cmd:tasklist` or `*shell:Get-Process` | Execution with a terminal window. (only for `cmd:` and `shell:` prefixes). |

Signs can be combined: `-*cmd:tasklist` runs cmd in a visible window as admin.

---

## Icon field

| Type | Example | Description |
|---|---|---|
| **Glyph** | `E774` | Hex code of a [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font) glyph. 4-digit hex only, no `\u` prefix. |
| **Image file** | `C:\Icons\name.png` | Full path to an image. Supported: `.png` `.ico` `.jpg` `.bmp`. Recommended: 32x32 px, transparent background. |
| **App path** | `C:\Program Files\Windhawk\windhawk.exe` | Full path to an executable file (`.exe`, `.dll`). The icon will be extracted from the application. |

---

## Style Settings

### Icon color

- **Auto** - detects the current Windows theme (light/dark) on every click and redraws the tray icon accordingly.
- **White** - always white (for dark taskbar).
- **Black** - always black (for light taskbar).

### Context menu color

- **Auto** - follows the current Windows theme automatically.
- **Light** - light background, dark icons.
- **Dark** - dark background, light icons.

Controls the background and icon color inside the right-click context menu.

---

## Context Menu Settings

### Open menu near tray icon

When enabled, the context menu opens near the tray icon instead of at the cursor position. This allows precise positioning relative to the icon.

### Menu direction

Choose where the menu appears relative to the tray icon:
- **Above icon** - menu opens above the icon
- **Below icon** - menu opens below the icon
- **Left of icon** - menu opens to the left
- **Right of icon** - menu opens to the right

### Menu alignment

Controls how the menu is aligned relative to the icon:
- **Center** - menu is centered on the icon
- **Left** - menu's left edge aligns with the icon
- **Right** - menu's right edge aligns with the icon

### Menu offset

Distance in pixels between the tray icon and the menu (0-200). Default is 10px.

---

## Tray Items

Each item in the **Tray Items** list becomes a separate icon in the system tray. You can add multiple items to create a collection of custom tray icons.

### Item Settings

- **Label** - Tooltip text shown when hovering over the tray icon.
- **Icon** - The icon to display. Can be a glyph code (e.g., "EC50"), image file path, or executable path.
- **Action** - Command or action to execute when the icon is left-clicked (or right-clicked if buttons are swapped).
- **Swap mouse buttons** - When enabled, left-click opens the context menu and right-click runs the action.
- **Context menu** - List of menu items shown on right-click (or left-click if buttons are swapped).

### Context Menu Items

Each context menu item has the following settings:

- **State** - Controls visibility and interaction (Enabled/Disabled/Hidden).
- **Name** - Text displayed in the menu.
- **Description** - Optional text shown on the right side of the menu item (e.g., keyboard shortcut like "Win+E" or a hint).
- **Icon** - Icon shown next to the menu item (glyph code, image, or executable path).
- **Action** - Command to execute when the menu item is clicked.
- **Separator** - Add a separator line above or below this item.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- StyleSettings:
  - icon_color: auto
    $name: Icons color
    $description: "Auto detects the Windows theme on every click. White for dark taskbar, Black for light taskbar."
    $options:
    - auto: Auto
    - white: White
    - black: Black
  - menu_color: auto
    $name: Context menu color
    $description: "Background and icon color inside the right-click context menu. Auto follows the Windows theme."
    $options:
    - auto: Auto
    - light: Light
    - dark: Dark
  $name: Style Settings
- MenuSettings:
  - menu_near_icon: false
    $name: Open menu near tray icon
    $description: "When enabled, menu opens near the tray icon instead of at cursor position."
  - menu_direction: "top"
    $name: Menu direction
    $description: "Direction from the tray icon where menu appears (only when 'Open menu near tray icon' is enabled)."
    $options:
    - top: Above icon ↑
    - left: Left of icon ←
    - right: Right of icon →
    - bottom: Below icon ↓
  - menu_alignment: center
    $name: Menu alignment
    $description: "How the menu is aligned relative to the icon (only when 'Open menu near tray icon' is enabled)."
    $options:
    - center: Center
    - left: Left
    - right: Right
  - menu_offset: 10
    $name: Menu offset
    $description: "Distance in pixels between the tray icon and menu (0-200, only when 'Open menu near tray icon' is enabled)."
  $name: Context menu settings
- items:
    - - label: "File Explorer"
        $name: Label
        $description: "Tooltip shown when hovering over the tray icon."
      - icon: "EC50"
        $name: Icon
      - action: "cmd:explorer"
        $name: Action
      - swap_buttons: false
        $name: Swap mouse buttons
        $description: "When enabled, left-click opens context menu and right-click runs action."
      - context_menu:
          - - state: "enabled"
              $name: State
              $options:
              - enabled: Enabled (works normally)
              - disabled: Disabled (visible but cannot be clicked)
              - hidden: Hidden (not visible in menu)
            - name: "Open File Explorer"
              $name: Name
            - description: "Win+E"
              $name: Description
            - icon: "C:\\Windows\\explorer.exe"
              $name: Icon
            - action: "cmd:explorer"
              $name: Action
            - separator: "below"
              $name: Separator
              $description: "Add a separator line in the menu."
              $options:
              - none: None
              - above: Above this item ↑
              - below: Below this item ↓
          - - name: "Downloads"
            - description: ""
            - icon: "E896"
            - action: "~downloads"
            - separator: "none"
            - state: "enabled"
          - - name: "Documents"
            - description: ""
            - icon: "E8A5"
            - action: "~documents"
            - separator: "none"
            - state: "enabled"
          - - name: "Pictures"
            - description: ""
            - icon: "E91B"
            - action: "~Pictures"
            - separator: "none"
            - state: "enabled"
          - - name: "Videos"
            - description: ""
            - icon: "E714"
            - action: "~videos"
            - separator: "none"
            - state: "enabled"
          - - name: "Music"
            - description: ""
            - icon: "EC4F"
            - action: "~music"
            - separator: "none"
            - state: "enabled"
          - - name: "Network"
            - description: ""
            - icon: "EC27"
            - action: "~network"
            - separator: "above"
            - state: "enabled"
          - - name: "Personal Folder"
            - description: ""
            - icon: "EC25"
            - action: "~profile"
            - separator: "none"
            - state: "enabled"
        $name: Context menu
  $name: Tray Items
  $description: "Each item becomes a tray icon. Left-click runs Action, right-click opens Context menu."
*/
// ==/WindhawkModSettings==

#define INITGUID
#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <uxtheme.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

struct ContextMenuItem {
    std::wstring name;
    std::wstring icon;
    std::wstring action;
    std::wstring separator;
    std::wstring state;
    std::wstring description;
};

struct TrayItem {
    std::wstring label;
    std::wstring action;
    std::wstring iconRaw;
    HICON        hIcon      = nullptr;
    HICON        hIconLight = nullptr;
    HICON        hIconDark  = nullptr;
    GUID         guid       = {};
    bool         swapButtons = false;
    std::vector<ContextMenuItem> contextMenu;
};

static std::vector<TrayItem> g_items;
static std::mutex            g_itemsMutex;
static HINSTANCE             g_hInstance       = nullptr;
static HWND                  g_trayHwnd        = nullptr;
static const UINT            WM_TRAY_CB        = WM_USER + 100;
static const UINT            WM_RELOAD         = WM_USER + 101;
static const UINT            WM_THEME_CHANGED  = WM_USER + 102;
static ULONG_PTR             g_gdiplusToken    = 0;
static UINT                  WM_TASKBARCREATED = 0;

static std::wstring g_iconColorMode  = L"auto";
static std::wstring g_menuColorMode  = L"auto";
static bool g_menuNearIcon = false;
static std::wstring g_menuDirection = L"top";
static std::wstring g_menuAlignment = L"center";
static int g_menuOffset = 10;
static HMENU g_currentMenu = nullptr;
static UINT g_currentMenuItemId = (UINT)-1;
static DWORD g_lastMenuCloseTime = 0;

static bool IsSystemDarkMode() {
    DWORD value = 1;
    DWORD size  = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme",
                 RRF_RT_DWORD, nullptr, &value, &size);
    return value == 0;
}

static bool TrayIconUseWhite() {
    if (g_iconColorMode == L"white") return true;
    if (g_iconColorMode == L"black") return false;
    return IsSystemDarkMode();
}

static bool MenuUseDark() {
    if (g_menuColorMode == L"dark")  return true;
    if (g_menuColorMode == L"light") return false;
    return IsSystemDarkMode();
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static bool StartsWithCI(const std::wstring& s, const wchar_t* prefix) {
    size_t n = wcslen(prefix);
    return s.size() >= n && _wcsnicmp(s.c_str(), prefix, n) == 0;
}

static std::wstring StripQuotes(const std::wstring& s) {
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
        return s.substr(1, s.size() - 2);
    return s;
}

static bool IsImagePath(const std::wstring& s) {
    if (s.size() < 3) return false;
    if (s.size() >= 2 && s[1] == L':')  return true;
    if (s.size() >= 2 && s[0] == L'\\') return true;
    std::wstring lo = ToLower(s);
    for (auto& ext : { std::wstring(L".png"),  std::wstring(L".ico"),
                       std::wstring(L".jpg"),  std::wstring(L".jpeg"),
                       std::wstring(L".bmp") })
        if (lo.size() >= ext.size() &&
            lo.compare(lo.size() - ext.size(), ext.size(), ext) == 0)
            return true;
    return false;
}

static bool IsExePath(const std::wstring& s) {
    if (s.size() < 5) return false;
    std::wstring lo = ToLower(s);
    for (auto& ext : { std::wstring(L".exe"), std::wstring(L".dll") })
        if (lo.size() >= ext.size() &&
            lo.compare(lo.size() - ext.size(), ext.size(), ext) == 0)
            return true;
    return false;
}

static HICON BitmapToHIcon(Gdiplus::Bitmap* src, int size) {
    if (!src || src->GetLastStatus() != Gdiplus::Ok) return nullptr;
    Gdiplus::Bitmap scaled(size, size, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&scaled);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.DrawImage(src, 0, 0, size, size);
    }
    HICON hIcon = nullptr;
    scaled.GetHICON(&hIcon);
    return hIcon;
}

static HICON LoadImageIconGdiplus(const std::wstring& path, int size) {
    Gdiplus::Bitmap bmp(path.c_str());
    if (bmp.GetLastStatus() != Gdiplus::Ok) {
        Wh_Log(L"GDI+ could not load image: %s  (status %d)",
               path.c_str(), (int)bmp.GetLastStatus());
        return nullptr;
    }
    return BitmapToHIcon(&bmp, size);
}

static HICON LoadIconFromExe(const std::wstring& path, int size) {
    HICON hIconLarge = nullptr;
    HICON hIconSmall = nullptr;

    UINT count = ExtractIconExW(path.c_str(), 0, &hIconLarge, &hIconSmall, 1);

    if (count == 0 || (!hIconLarge && !hIconSmall)) {
        Wh_Log(L"Could not extract icon from: %s", path.c_str());
        return nullptr;
    }

    HICON hIcon = (size > 16) ? hIconLarge : hIconSmall;
    HICON hIconToDestroy = (size > 16) ? hIconSmall : hIconLarge;

    if (hIconToDestroy) DestroyIcon(hIconToDestroy);

    if (!hIcon) {
        Wh_Log(L"Extracted icon is null for: %s", path.c_str());
        return nullptr;
    }

    ICONINFO iconInfo;
    if (!GetIconInfo(hIcon, &iconInfo)) {
        DestroyIcon(hIcon);
        return nullptr;
    }

    BITMAP bm;
    GetObject(iconInfo.hbmColor ? iconInfo.hbmColor : iconInfo.hbmMask, sizeof(bm), &bm);

    bool needResize = (bm.bmWidth != size || bm.bmHeight != size);

    if (needResize) {
        HDC hdc = GetDC(nullptr);
        HDC memDC = CreateCompatibleDC(hdc);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = bm.bmWidth;
        bmi.bmiHeader.biHeight = -bm.bmHeight;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HBITMAP dib = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

        if (dib) {
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, dib);
            DrawIconEx(memDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
            SelectObject(memDC, oldBmp);

            Gdiplus::Bitmap srcBmp(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4,
                                   PixelFormat32bppARGB, (BYTE*)bits);

            if (srcBmp.GetLastStatus() == Gdiplus::Ok) {
                HICON resizedIcon = BitmapToHIcon(&srcBmp, size);
                DeleteObject(dib);
                DeleteDC(memDC);
                ReleaseDC(nullptr, hdc);

                if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
                if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
                DestroyIcon(hIcon);

                return resizedIcon;
            }

            DeleteObject(dib);
        }

        DeleteDC(memDC);
        ReleaseDC(nullptr, hdc);
    }

    if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);

    return hIcon;
}

static HICON CreateGlyphIcon(WCHAR glyph, int size, bool white) {
    HDC hdc   = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = size;
    bmi.bmiHeader.biHeight      = -size;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) { DeleteDC(memDC); ReleaseDC(nullptr, hdc); return nullptr; }

    SelectObject(memDC, dib);
    memset(bits, 0, size * size * 4);

    HFONT font = CreateFontW(
        -size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe Fluent Icons");
        
    HFONT oldFont = (HFONT)SelectObject(memDC, font);
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    RECT rc = { 0, 0, size, size };
    DrawTextW(memDC, &glyph, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

    DWORD* pPx = (DWORD*)bits;
    if (white) {
        for (int i = 0; i < size * size; i++) {
            BYTE b = (BYTE)(pPx[i] & 0xFF);
            pPx[i] = b > 0 ? ((b << 24) | (b << 16) | (b << 8) | b) : 0;
        }
    } else {
        for (int i = 0; i < size * size; i++) {
            BYTE b = (BYTE)(pPx[i] & 0xFF);
            pPx[i] = b > 0 ? ((b << 24) | 0) : 0; 
        }
    }

    SelectObject(memDC, oldFont);
    DeleteObject(font);

    HBITMAP hMask = CreateBitmap(size, size, 1, 1, nullptr);
    ICONINFO ii   = { TRUE, 0, 0, hMask, dib };
    HICON hIcon   = CreateIconIndirect(&ii);
    DeleteObject(hMask);
    DeleteObject(dib);
    DeleteDC(memDC);
    ReleaseDC(nullptr, hdc);
    return hIcon;
}

static HICON MakeIconVariant(const std::wstring& iconRaw, int size, bool white) {
    if (iconRaw.empty())
        return CreateGlyphIcon(L'\uE700', size, white);

    if (IsExePath(iconRaw)) {
        if (GetFileAttributesW(iconRaw.c_str()) == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Executable file not found: %s", iconRaw.c_str());
            return CreateGlyphIcon(L'\uE783', size, white);
        }
        HICON h = LoadIconFromExe(iconRaw, size);
        if (h) return h;
        Wh_Log(L"Icon extraction failed for: %s -- fallback glyph", iconRaw.c_str());
        return CreateGlyphIcon(L'\uE783', size, white);
    }

    if (IsImagePath(iconRaw)) {
        if (GetFileAttributesW(iconRaw.c_str()) == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Icon file not found: %s", iconRaw.c_str());
            return CreateGlyphIcon(L'\uE783', size, white);
        }
        HICON h = LoadImageIconGdiplus(iconRaw, size);
        if (h) return h;
        Wh_Log(L"GDI+ load failed for: %s -- fallback glyph", iconRaw.c_str());
        return CreateGlyphIcon(L'\uE783', size, white);
    }

    WCHAR glyph = (WCHAR)wcstoul(iconRaw.c_str(), nullptr, 16);
    if (glyph == 0) glyph = L'\uE700';
    return CreateGlyphIcon(glyph, size, white);
}

static HICON GetCurrentTrayIcon(TrayItem& item) {
    bool useWhite = TrayIconUseWhite();
    return useWhite ? item.hIconDark : item.hIconLight;
}

static bool GetKnownFolderPath(const wchar_t* name, std::wstring& out) {
    KNOWNFOLDERID fid{};
    std::wstring n = ToLower(name);
    if      (n == L"downloads")                     fid = FOLDERID_Downloads;
    else if (n == L"documents")                     fid = FOLDERID_Documents;
    else if (n == L"music" || n == L"sounds")       fid = FOLDERID_Music;
    else if (n == L"pictures")                      fid = FOLDERID_Pictures;
    else if (n == L"videos")                        fid = FOLDERID_Videos;
    else if (n == L"desktop")                       fid = FOLDERID_Desktop;
    else if (n == L"network" || n == L"networks")   fid = FOLDERID_NetworkFolder;
    else if (n == L"profile" || n == L"home" || n == L"personal" || n == L"personal folder") fid = FOLDERID_Profile;
    else return false;

    PWSTR raw = nullptr;
    bool ok = SUCCEEDED(SHGetKnownFolderPath(fid, 0, nullptr, &raw)) && raw;
    if (ok) out = raw;
    if (raw) CoTaskMemFree(raw);
    return ok;
}

static WORD ResolveKeyToken(const std::wstring& tok) {
    if (tok.empty()) return 0;

    std::wstring lo = ToLower(tok);
    if (lo == L"ctrl")        return VK_CONTROL;
    if (lo == L"alt")         return VK_MENU;
    if (lo == L"shift")       return VK_SHIFT;
    if (lo == L"win")         return VK_LWIN;

    if (lo == L"enter")       return VK_RETURN;
    if (lo == L"esc" || lo == L"escape") return VK_ESCAPE;
    if (lo == L"tab")         return VK_TAB;
    if (lo == L"space")       return VK_SPACE;
    if (lo == L"backspace")   return VK_BACK;
    if (lo == L"delete" || lo == L"del") return VK_DELETE;
    if (lo == L"insert" || lo == L"ins") return VK_INSERT;
    if (lo == L"home")        return VK_HOME;
    if (lo == L"end")         return VK_END;
    if (lo == L"pageup"  || lo == L"pgup") return VK_PRIOR;
    if (lo == L"pagedown"|| lo == L"pgdn") return VK_NEXT;
    if (lo == L"left")        return VK_LEFT;
    if (lo == L"right")       return VK_RIGHT;
    if (lo == L"up")          return VK_UP;
    if (lo == L"down")        return VK_DOWN;
    if (lo == L"printscreen") return VK_SNAPSHOT;
    if (lo == L"pause")       return VK_PAUSE;
    if (lo == L"capslock")    return VK_CAPITAL;
    if (lo == L"numlock")     return VK_NUMLOCK;
    if (lo == L"scrolllock")  return VK_SCROLL;
    if (lo == L"sleep")       return VK_SLEEP;

    if (lo.size() >= 2 && lo[0] == L'f') {
        wchar_t* end = nullptr;
        long n = wcstol(lo.c_str() + 1, &end, 10);
        if (end && *end == L'\0' && n >= 1 && n <= 24)
            return (WORD)(VK_F1 + n - 1);
    }

    if (tok.size() >= 2 && tok[0] == L'0' && (tok[1] == L'x' || tok[1] == L'X')) {
        long v = wcstol(tok.c_str(), nullptr, 16);
        if (v > 0 && v <= 0xFF) return (WORD)v;
        Wh_Log(L"press: hex VK out of range: %s", tok.c_str());
        return 0;
    }

    if (iswdigit(tok[0])) {
        long v = wcstol(tok.c_str(), nullptr, 10);
        if (v > 0 && v <= 0xFF) return (WORD)v;
        Wh_Log(L"press: decimal VK out of range: %s", tok.c_str());
        return 0;
    }

    if (tok.size() == 1) {
        wchar_t c = towupper(tok[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return (WORD)c;
    }

    Wh_Log(L"press: unrecognised token '%s'", tok.c_str());
    return 0;
}

static void SendVirtualKeypress(const std::wstring& keysStr) {
    std::vector<WORD> keys;

    std::wstring normalised = keysStr;
    for (auto& c : normalised) if (c == L';') c = L'+';

    size_t start = 0;
    while (true) {
        size_t sep = normalised.find(L'+', start);
        std::wstring token = normalised.substr(start,
            sep == std::wstring::npos ? sep : sep - start);
        while (!token.empty() && iswspace(token.front())) token.erase(token.begin());
        while (!token.empty() && iswspace(token.back()))  token.pop_back();
        if (!token.empty()) {
            WORD vk = ResolveKeyToken(token);
            if (vk) keys.push_back(vk);
        }
        if (sep == std::wstring::npos) break;
        start = sep + 1;
    }

    if (keys.empty()) {
        Wh_Log(L"press: no valid codes in '%s'", keysStr.c_str());
        return;
    }

    for (size_t i = 0; i < keys.size(); i++) {
        keybd_event((BYTE)keys[i], 0, 0, 0);
        Sleep(10);
    }

    Sleep(100);

    for (size_t i = keys.size(); i > 0; i--) {
        keybd_event((BYTE)keys[i - 1], 0, KEYEVENTF_KEYUP, 0);
        Sleep(10);
    }
}

static void ParseActionSigns(const std::wstring& raw,
                              std::wstring& out,
                              bool& runas,
                              bool& showWindow) {
    runas      = false;
    showWindow = false;
    size_t i   = 0;
    while (i < raw.size()) {
        if (raw[i] == L'-')      { runas      = true; ++i; }
        else if (raw[i] == L'*') { showWindow = true; ++i; }
        else break;
    }
    out = raw.substr(i);
}

static void ExecuteAction(const std::wstring& raw) {
    if (raw.empty()) return;

    std::thread([raw]() {
        bool runas      = false;
        bool showWindow = false;
        std::wstring a;
        ParseActionSigns(raw, a, runas, showWindow);

        const LPCWSTR verb = runas ? L"runas" : L"open";
        const int     nShow = showWindow ? SW_NORMAL : SW_HIDE;

        if (StartsWithCI(a, L"press:")) {
            SendVirtualKeypress(a.substr(6));
            return;
        }

        if (StartsWithCI(a, L"web:")) {
            ShellExecuteW(nullptr, verb, a.substr(4).c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }

        if (StartsWithCI(a, L"ms-settings:")) {
            ShellExecuteW(nullptr, verb, a.c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }

        if (StartsWithCI(a, L"cmd:")) {
            std::wstring flag = showWindow ? L"/K " : L"/C ";
            std::wstring arg  = flag + a.substr(4);
            ShellExecuteW(nullptr, verb, L"cmd.exe",
                          arg.c_str(), nullptr, showWindow ? SW_NORMAL : SW_HIDE);
            return;
        }

        if (StartsWithCI(a, L"shell:")) {
            std::wstring arg =
                L"-NoProfile -ExecutionPolicy Bypass -Command " + a.substr(6);
            if (showWindow)
                arg = L"-NoExit " + arg;
            ShellExecuteW(nullptr, verb, L"powershell.exe",
                          arg.c_str(), nullptr, showWindow ? SW_NORMAL : SW_HIDE);
            return;
        }

        if (!a.empty() && a.front() == L'~') {
            std::wstring target = a.substr(1);
            std::wstring resolved;
            if (GetKnownFolderPath(target.c_str(), resolved)) {
                ShellExecuteW(nullptr, verb, resolved.c_str(),
                              nullptr, nullptr, SW_SHOWNORMAL);
                return;
            }
            wchar_t buf[MAX_PATH * 4]{};
            if (SearchPathW(nullptr, target.c_str(), nullptr,
                            ARRAYSIZE(buf), buf, nullptr)) {
                ShellExecuteW(nullptr, verb, buf,
                              nullptr, nullptr, SW_SHOWNORMAL);
                return;
            }
            Wh_Log(L"~search: '%s' not found", target.c_str());
            return;
        }

        std::wstring path = StripQuotes(a);
        ShellExecuteW(nullptr, verb, path.c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
    }).detach();
}

static HBITMAP MakeMenuBitmap(const std::wstring& iconRaw, bool darkMenu) {
    const int sz = 16;
    if (iconRaw.empty()) return nullptr;

    if (IsExePath(iconRaw)) {
        if (GetFileAttributesW(iconRaw.c_str()) == INVALID_FILE_ATTRIBUTES)
            return nullptr;
        HICON hIcon = LoadIconFromExe(iconRaw, sz);
        if (!hIcon) return nullptr;

        ICONINFO iconInfo;
        if (!GetIconInfo(hIcon, &iconInfo)) {
            DestroyIcon(hIcon);
            return nullptr;
        }

        HDC hdc = GetDC(nullptr);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = sz;
        bmi.bmiHeader.biHeight = -sz;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HBITMAP dib = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

        if (dib) {
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, dib);
            memset(bits, 0, sz * sz * 4);
            DrawIconEx(memDC, 0, 0, hIcon, sz, sz, 0, nullptr, DI_NORMAL);
            SelectObject(memDC, oldBmp);
            DeleteDC(memDC);
        }

        ReleaseDC(nullptr, hdc);
        if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
        DestroyIcon(hIcon);

        return dib;
    }

    if (IsImagePath(iconRaw)) {
        if (GetFileAttributesW(iconRaw.c_str()) == INVALID_FILE_ATTRIBUTES)
            return nullptr;
        Gdiplus::Bitmap bmp(iconRaw.c_str());
        if (bmp.GetLastStatus() != Gdiplus::Ok) return nullptr;

        Gdiplus::Bitmap scaled(sz, sz, PixelFormat32bppARGB);
        {
            Gdiplus::Graphics g(&scaled);
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            g.DrawImage(&bmp, 0, 0, sz, sz);
        }

        HDC hdc   = GetDC(nullptr);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = sz;
        bmi.bmiHeader.biHeight      = -sz;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HBITMAP dib = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        ReleaseDC(nullptr, hdc);

        if (dib) {
            Gdiplus::Rect rect(0, 0, sz, sz);
            Gdiplus::BitmapData bd;
            if (scaled.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bd) == Gdiplus::Ok) {
                DWORD* pDst = (DWORD*)bits;
                DWORD* pSrc = (DWORD*)bd.Scan0;
                for (int i = 0; i < sz * sz; i++) {
                    DWORD c = pSrc[i];
                    BYTE a = (c >> 24) & 0xFF;
                    BYTE r = (c >> 16) & 0xFF;
                    BYTE g = (c >> 8) & 0xFF;
                    BYTE b = c & 0xFF;
                    
                    r = (r * a) / 255;
                    g = (g * a) / 255;
                    b = (b * a) / 255;
                    pDst[i] = (a << 24) | (r << 16) | (g << 8) | b;
                }
                scaled.UnlockBits(&bd);
            }
        }
        return dib;
    }

    WCHAR glyph = (WCHAR)wcstoul(iconRaw.c_str(), nullptr, 16);
    if (glyph == 0) return nullptr;

    HDC hdc   = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hdc);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = sz;
    bmi.bmiHeader.biHeight      = -sz;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) { DeleteDC(memDC); ReleaseDC(nullptr, hdc); return nullptr; }
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, dib);
    memset(bits, 0, sz * sz * 4);

    HFONT font = CreateFontW(
        -sz, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe Fluent Icons");
    
    HFONT oldFont = (HFONT)SelectObject(memDC, font);
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    RECT rc = { 0, 0, sz, sz };
    DrawTextW(memDC, &glyph, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(memDC, oldFont);
    DeleteObject(font);

    DWORD* pPx = (DWORD*)bits;
    if (darkMenu) {
        for (int i = 0; i < sz * sz; i++) {
            BYTE b = (BYTE)(pPx[i] & 0xFF);
            pPx[i] = b > 0 ? ((b << 24) | (b << 16) | (b << 8) | b) : 0;
        }
    } else {
        for (int i = 0; i < sz * sz; i++) {
            BYTE b = (BYTE)(pPx[i] & 0xFF);
            BYTE v = (BYTE)((40 * b) / 255);
            pPx[i] = b > 0 ? ((b << 24) | (v << 16) | (v << 8) | v) : 0;
        }
    }

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, hdc);
    return dib;
}

static void ApplyMenuTheme(HWND hWnd, bool dark) {
    HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
    if (hUxtheme) {
        using fnSetPreferredAppMode = int(WINAPI*)(int);
        auto SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
        if (SetPreferredAppMode) {
            SetPreferredAppMode(dark ? 2 : 0);
        }

        using fnAllowDarkModeForWindow = bool(WINAPI*)(HWND, bool);
        auto AllowDarkModeForWindow = (fnAllowDarkModeForWindow)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133));
        if (AllowDarkModeForWindow) {
            AllowDarkModeForWindow(hWnd, dark);
        }

        using fnFlushMenuThemes = void(WINAPI*)();
        auto FlushMenuThemes = (fnFlushMenuThemes)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136));
        if (FlushMenuThemes) {
            FlushMenuThemes();
        }
    }
}

struct MenuPosition {
    POINT pt;
    UINT flags;
};

static MenuPosition GetMenuPositionNearIcon(UINT itemId) {
    MenuPosition result = {};
    result.flags = TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY;

    if (!g_menuNearIcon) {
        GetCursorPos(&result.pt);
        return result;
    }

    NOTIFYICONIDENTIFIER nii = {};
    nii.cbSize = sizeof(NOTIFYICONIDENTIFIER);
    nii.hWnd = g_trayHwnd;
    nii.uID = itemId;

    {
        std::lock_guard<std::mutex> lk(g_itemsMutex);
        if (itemId < g_items.size()) {
            nii.guidItem = g_items[itemId].guid;
        }
    }

    RECT iconRect = {};
    HRESULT hr = Shell_NotifyIconGetRect(&nii, &iconRect);

    if (FAILED(hr)) {
        GetCursorPos(&result.pt);
        return result;
    }

    int iconCenterX = (iconRect.left + iconRect.right) / 2;
    int iconCenterY = (iconRect.top + iconRect.bottom) / 2;

    int offset = (g_menuOffset >= 0 && g_menuOffset <= 200) ? g_menuOffset : 10;

    if (g_menuDirection == L"top") {
        result.pt.x = iconCenterX;
        result.pt.y = iconRect.top - offset;

        if (g_menuAlignment == L"center") {
            result.flags |= TPM_CENTERALIGN | TPM_BOTTOMALIGN;
        } else if (g_menuAlignment == L"left") {
            result.flags |= TPM_LEFTALIGN | TPM_BOTTOMALIGN;
            result.pt.x = iconRect.left;
        } else if (g_menuAlignment == L"right") {
            result.flags |= TPM_RIGHTALIGN | TPM_BOTTOMALIGN;
            result.pt.x = iconRect.right;
        }
    } else if (g_menuDirection == L"bottom") {
        result.pt.x = iconCenterX;
        result.pt.y = iconRect.bottom + offset;

        if (g_menuAlignment == L"center") {
            result.flags |= TPM_CENTERALIGN | TPM_TOPALIGN;
        } else if (g_menuAlignment == L"left") {
            result.flags |= TPM_LEFTALIGN | TPM_TOPALIGN;
            result.pt.x = iconRect.left;
        } else if (g_menuAlignment == L"right") {
            result.flags |= TPM_RIGHTALIGN | TPM_TOPALIGN;
            result.pt.x = iconRect.right;
        }
    } else if (g_menuDirection == L"left") {
        result.pt.x = iconRect.left - offset;
        result.pt.y = iconCenterY;

        if (g_menuAlignment == L"center") {
            result.flags |= TPM_RIGHTALIGN | TPM_VCENTERALIGN;
        } else if (g_menuAlignment == L"left") {
            result.flags |= TPM_RIGHTALIGN | TPM_TOPALIGN;
            result.pt.y = iconRect.top;
        } else if (g_menuAlignment == L"right") {
            result.flags |= TPM_RIGHTALIGN | TPM_BOTTOMALIGN;
            result.pt.y = iconRect.bottom;
        }
    } else if (g_menuDirection == L"right") {
        result.pt.x = iconRect.right + offset;
        result.pt.y = iconCenterY;

        if (g_menuAlignment == L"center") {
            result.flags |= TPM_LEFTALIGN | TPM_VCENTERALIGN;
        } else if (g_menuAlignment == L"left") {
            result.flags |= TPM_LEFTALIGN | TPM_TOPALIGN;
            result.pt.y = iconRect.top;
        } else if (g_menuAlignment == L"right") {
            result.flags |= TPM_LEFTALIGN | TPM_BOTTOMALIGN;
            result.pt.y = iconRect.bottom;
        }
    } else {
        result.pt.x = iconCenterX;
        result.pt.y = iconRect.top - offset;
        result.flags |= TPM_CENTERALIGN | TPM_BOTTOMALIGN;
    }

    return result;
}

static void ShowContextMenu(HWND hWnd, UINT itemId,
                            const std::vector<ContextMenuItem>& items) {
    if (items.empty()) return;

    DWORD currentTime = GetTickCount();
    if (currentTime - g_lastMenuCloseTime < 300) {
        return;
    }

    if (g_currentMenu != nullptr) {
        EndMenu();
        g_currentMenu = nullptr;
        g_currentMenuItemId = (UINT)-1;
        g_lastMenuCloseTime = GetTickCount();
        return;
    }

    bool dark = MenuUseDark();

    SetForegroundWindow(hWnd);

    ApplyMenuTheme(hWnd, dark);

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    g_currentMenu = hMenu;
    g_currentMenuItemId = itemId;

    std::vector<HBITMAP> bitmaps;
    bitmaps.reserve(items.size());

    UINT menuPosition = 0;

    for (size_t i = 0; i < items.size(); ++i) {
        const auto& mi = items[i];

        if (mi.state == L"hidden") {
            continue;
        }

        if (mi.separator == L"above") {
            MENUITEMINFOW sepAbove = {};
            sepAbove.cbSize = sizeof(sepAbove);
            sepAbove.fMask = MIIM_FTYPE;
            sepAbove.fType = MFT_SEPARATOR;
            InsertMenuItemW(hMenu, menuPosition, TRUE, &sepAbove);
            menuPosition++;
        }

        std::wstring txt = mi.name.empty() ? L"(unnamed)" : mi.name;

        if (!mi.description.empty()) {
            txt += L"\t" + mi.description;
        }

        MENUITEMINFOW mii = {};
        mii.cbSize     = sizeof(mii);
        mii.fMask      = MIIM_ID | MIIM_STRING | MIIM_STATE;
        mii.wID        = (UINT)(1000 + i);
        mii.dwTypeData = txt.data();
        mii.cch        = (UINT)txt.size();

        if (mi.state == L"disabled") {
            mii.fState = MFS_DISABLED;
        } else {
            mii.fState = MFS_ENABLED;
        }

        HBITMAP hBmp = MakeMenuBitmap(mi.icon, dark);
        if (hBmp) {
            mii.fMask    |= MIIM_BITMAP;
            mii.hbmpItem  = hBmp;
            bitmaps.push_back(hBmp);
        }
        InsertMenuItemW(hMenu, menuPosition, TRUE, &mii);
        menuPosition++;

        if (mi.separator == L"below") {
            MENUITEMINFOW sepBelow = {};
            sepBelow.cbSize = sizeof(sepBelow);
            sepBelow.fMask = MIIM_FTYPE;
            sepBelow.fType = MFT_SEPARATOR;
            InsertMenuItemW(hMenu, menuPosition, TRUE, &sepBelow);
            menuPosition++;
        }
    }

    MenuPosition menuPos = GetMenuPositionNearIcon(itemId);
    int cmd = TrackPopupMenu(hMenu, menuPos.flags,
                             menuPos.pt.x, menuPos.pt.y, 0, hWnd, nullptr);

    g_currentMenu = nullptr;
    g_currentMenuItemId = (UINT)-1;
    g_lastMenuCloseTime = GetTickCount();

    DestroyMenu(hMenu);
    for (auto b : bitmaps) DeleteObject(b);

    if (cmd >= 1000) {
        size_t idx = (size_t)(cmd - 1000);
        std::lock_guard<std::mutex> lk(g_itemsMutex);
        if (itemId < g_items.size() && idx < g_items[itemId].contextMenu.size())
            ExecuteAction(g_items[itemId].contextMenu[idx].action);
    }
}

static void AddAllTrayIcons() {
    std::lock_guard<std::mutex> lk(g_itemsMutex);
    for (size_t i = 0; i < g_items.size(); i++) {
        HICON hIco = GetCurrentTrayIcon(g_items[i]);
        NOTIFYICONDATAW nid = {};
        nid.cbSize           = sizeof(nid);
        nid.hWnd             = g_trayHwnd;
        nid.uID              = (UINT)i;
        nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
        nid.guidItem         = g_items[i].guid;
        nid.uCallbackMessage = WM_TRAY_CB;
        nid.hIcon            = hIco;
        lstrcpynW(nid.szTip, g_items[i].label.c_str(), 128);
        if (!Shell_NotifyIconW(NIM_MODIFY, &nid))
            Shell_NotifyIconW(NIM_ADD, &nid);
    }
}

static void UpdateTrayIcons(bool removeAll = false) {
    std::lock_guard<std::mutex> lk(g_itemsMutex);
    for (size_t i = 0; i < g_items.size(); i++) {
        NOTIFYICONDATAW nid = {};
        nid.cbSize   = sizeof(nid);
        nid.hWnd     = g_trayHwnd;
        nid.uID      = (UINT)i;
        nid.uFlags   = NIF_GUID;
        nid.guidItem = g_items[i].guid;

        if (removeAll) {
            Shell_NotifyIconW(NIM_DELETE, &nid);
            continue;
        }

        HICON hIco = GetCurrentTrayIcon(g_items[i]);
        nid.uFlags          |= NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAY_CB;
        nid.hIcon            = hIco;
        lstrcpynW(nid.szTip, g_items[i].label.c_str(), 128);

        if (!Shell_NotifyIconW(NIM_MODIFY, &nid))
            Shell_NotifyIconW(NIM_ADD, &nid);
    }
}

static void RefreshTrayIconTheme() {
    std::lock_guard<std::mutex> lk(g_itemsMutex);
    for (size_t i = 0; i < g_items.size(); i++) {
        HICON hIco = GetCurrentTrayIcon(g_items[i]);
        NOTIFYICONDATAW nid = {};
        nid.cbSize           = sizeof(nid);
        nid.hWnd             = g_trayHwnd;
        nid.uID              = (UINT)i;
        nid.uFlags           = NIF_ICON | NIF_GUID;
        nid.guidItem         = g_items[i].guid;
        nid.hIcon            = hIco;
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    }
}

static void DestroyAllIcons() {
    std::lock_guard<std::mutex> lk(g_itemsMutex);
    for (auto& item : g_items) {
        if (item.hIconLight) { DestroyIcon(item.hIconLight); item.hIconLight = nullptr; }
        if (item.hIconDark)  { DestroyIcon(item.hIconDark);  item.hIconDark  = nullptr; }
        item.hIcon = nullptr;
    }
}

static void LoadAllSettings() {

    PCWSTR pColor = Wh_GetStringSetting(L"StyleSettings.icon_color");
    g_iconColorMode = L"auto";
    if (pColor) {
        if (wcscmp(pColor, L"white") == 0) g_iconColorMode = L"white";
        else if (wcscmp(pColor, L"black") == 0) g_iconColorMode = L"black";
        Wh_FreeStringSetting(pColor);
    }

    PCWSTR pMenu = Wh_GetStringSetting(L"StyleSettings.menu_color");
    g_menuColorMode = L"auto";
    if (pMenu) {
        if (wcscmp(pMenu, L"dark")  == 0) g_menuColorMode = L"dark";
        else if (wcscmp(pMenu, L"light") == 0) g_menuColorMode = L"light";
        Wh_FreeStringSetting(pMenu);
    }

    g_menuNearIcon = Wh_GetIntSetting(L"MenuSettings.menu_near_icon") != 0;

    PCWSTR pDirection = Wh_GetStringSetting(L"MenuSettings.menu_direction");
    g_menuDirection = L"top";
    if (pDirection) {
        if (wcscmp(pDirection, L"bottom") == 0) g_menuDirection = L"bottom";
        else if (wcscmp(pDirection, L"left") == 0) g_menuDirection = L"left";
        else if (wcscmp(pDirection, L"right") == 0) g_menuDirection = L"right";
        Wh_FreeStringSetting(pDirection);
    }

    PCWSTR pAlignment = Wh_GetStringSetting(L"MenuSettings.menu_alignment");
    g_menuAlignment = L"center";
    if (pAlignment) {
        if (wcscmp(pAlignment, L"left") == 0) g_menuAlignment = L"left";
        else if (wcscmp(pAlignment, L"right") == 0) g_menuAlignment = L"right";
        Wh_FreeStringSetting(pAlignment);
    }

    g_menuOffset = Wh_GetIntSetting(L"MenuSettings.menu_offset");
    if (g_menuOffset < 0 || g_menuOffset > 200) g_menuOffset = 10;

    UpdateTrayIcons(true);
    DestroyAllIcons();

    std::vector<TrayItem> newItems;

    for (int i = 0; i <= 127; i++) {
        WCHAR key[256];

        swprintf_s(key, L"items[%d].label", i);
        PCWSTR pLabel = Wh_GetStringSetting(key);
        std::wstring labelStr = pLabel ? pLabel : L"";
        if (pLabel) Wh_FreeStringSetting(pLabel);

        swprintf_s(key, L"items[%d].action", i);
        PCWSTR pAction = Wh_GetStringSetting(key);
        std::wstring actionStr = pAction ? pAction : L"";
        if (pAction) Wh_FreeStringSetting(pAction);

        swprintf_s(key, L"items[%d].icon", i);
        PCWSTR pIcon = Wh_GetStringSetting(key);
        std::wstring iconStr = pIcon ? pIcon : L"";
        if (pIcon) Wh_FreeStringSetting(pIcon);

        swprintf_s(key, L"items[%d].swap_buttons", i);
        bool swapButtons = Wh_GetIntSetting(key) != 0;

        bool allEmpty = labelStr.empty() && actionStr.empty() && iconStr.empty();
        if (allEmpty) {
            if (i >= (int)newItems.size() + 2) break;
            continue;
        }

        TrayItem item;
        item.label   = labelStr.empty() ? L"Tray" : labelStr;
        item.action  = actionStr;
        item.iconRaw = iconStr.empty() ? L"E700" : iconStr;
        item.swapButtons = swapButtons;

        item.hIconLight = MakeIconVariant(item.iconRaw, 32, false);
        item.hIconDark  = MakeIconVariant(item.iconRaw, 32, true);
        item.hIcon      = nullptr;

        item.guid = { 0x57595321, (WORD)i, 0x1234,
                      { 0xAA, 0xBB, 0x11, 0x22, 0x33, 0x44, 0x55, (BYTE)i } };

        for (int j = 0; j < 64; j++) {
            swprintf_s(key, L"items[%d].context_menu[%d].name", i, j);
            PCWSTR pCN = Wh_GetStringSetting(key);
            std::wstring cName = pCN ? pCN : L"";
            if (pCN) Wh_FreeStringSetting(pCN);
            if (cName.empty()) break;

            swprintf_s(key, L"items[%d].context_menu[%d].icon", i, j);
            PCWSTR pCI = Wh_GetStringSetting(key);

            swprintf_s(key, L"items[%d].context_menu[%d].action", i, j);
            PCWSTR pCA = Wh_GetStringSetting(key);

            swprintf_s(key, L"items[%d].context_menu[%d].separator", i, j);
            PCWSTR pSep = Wh_GetStringSetting(key);

            swprintf_s(key, L"items[%d].context_menu[%d].state", i, j);
            PCWSTR pState = Wh_GetStringSetting(key);

            swprintf_s(key, L"items[%d].context_menu[%d].description", i, j);
            PCWSTR pDesc = Wh_GetStringSetting(key);

            ContextMenuItem cmi;
            cmi.name   = cName;
            cmi.icon   = pCI ? pCI : L"";
            cmi.action = pCA ? pCA : L"";
            cmi.separator = pSep ? pSep : L"none";
            cmi.state = pState ? pState : L"enabled";
            cmi.description = pDesc ? pDesc : L"";
            if (pCI) Wh_FreeStringSetting(pCI);
            if (pCA) Wh_FreeStringSetting(pCA);
            if (pSep) Wh_FreeStringSetting(pSep);
            if (pState) Wh_FreeStringSetting(pState);
            if (pDesc) Wh_FreeStringSetting(pDesc);

            item.contextMenu.push_back(std::move(cmi));
        }

        newItems.push_back(std::move(item));
    }

    {
        std::lock_guard<std::mutex> lk(g_itemsMutex);
        g_items = std::move(newItems);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAY_CB) {
        UINT id    = LOWORD(wParam);
        UINT event = (UINT)lParam;

        bool swapped = false;
        {
            std::lock_guard<std::mutex> lk(g_itemsMutex);
            if (id < g_items.size()) swapped = g_items[id].swapButtons;
        }

        UINT actionEvent = swapped ? WM_RBUTTONUP : WM_LBUTTONUP;
        UINT menuEvent   = swapped ? WM_LBUTTONUP : WM_RBUTTONUP;

        if (event == actionEvent) {
            if (g_iconColorMode == L"auto")
                RefreshTrayIconTheme();

            std::wstring act;
            {
                std::lock_guard<std::mutex> lk(g_itemsMutex);
                if (id < g_items.size()) act = g_items[id].action;
            }
            ExecuteAction(act);
        } else if (event == menuEvent) {
            if (g_iconColorMode == L"auto")
                RefreshTrayIconTheme();

            std::vector<ContextMenuItem> menu;
            {
                std::lock_guard<std::mutex> lk(g_itemsMutex);
                if (id < g_items.size()) menu = g_items[id].contextMenu;
            }
            ShowContextMenu(hWnd, id, menu);
        }
        return 0;
    }

    if (msg == WM_RELOAD) {
        LoadAllSettings();
        UpdateTrayIcons();
        return 0;
    }

    if (msg == WM_SETTINGCHANGE) {
        if (lParam && lstrcmpW((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
            if (g_iconColorMode == L"auto")
                RefreshTrayIconTheme();
        }
        return 0;
    }

    if (WM_TASKBARCREATED != 0 && msg == WM_TASKBARCREATED) {
        Wh_Log(L"TaskbarCreated -- restoring tray icons");
        AddAllTrayIcons();
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI TrayThread(LPVOID) {
    Gdiplus::GdiplusStartupInput gdipInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdipInput, nullptr);
    WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc     = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = g_hInstance;
    wc.lpszClassName = L"WCT_TrayWindow_v21";
    RegisterClassW(&wc);

    g_trayHwnd = CreateWindowExW(0, wc.lpszClassName, L"WCT_Host",
                                 0, 0, 0, 0, 0,
                                 nullptr, nullptr, g_hInstance, nullptr);
    if (!g_trayHwnd) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        return 1;
    }

    if (WM_TASKBARCREATED != 0)
        ChangeWindowMessageFilterEx(g_trayHwnd, WM_TASKBARCREATED,
                                    MSGFLT_ALLOW, nullptr);

    LoadAllSettings();
    UpdateTrayIcons();

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0))
        DispatchMessageW(&m);

    UpdateTrayIcons(true);
    DestroyAllIcons();
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    return 0;
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL WhTool_ModInit() {
    g_hInstance = (HINSTANCE)GetModuleHandleW(nullptr);
    HANDLE h = CreateThread(nullptr, 0, TrayThread, nullptr, 0, nullptr);
    if (!h) return FALSE;
    CloseHandle(h);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_trayHwnd)
        PostMessageW(g_trayHwnd, WM_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    if (g_trayHwnd) {
        UpdateTrayIcons(true);
        PostMessageW(g_trayHwnd, WM_QUIT, 0, 0);
        g_trayHwnd = nullptr;
    }
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded             = false;
    bool isToolModProcess       = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0)
                isCurrentToolModProcess = true;
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded)
        return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }
        if (!WhTool_ModInit())
            ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint =
            (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess)
        return FALSE;

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher)
        return;

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"",
               currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR,
        LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, WINBOOL,
        DWORD, LPVOID, LPCWSTR,
        LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si{};
    si.cb      = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi{};
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher)
        return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher)
        return;
    WhTool_ModUninit();
    ExitProcess(0);
}
