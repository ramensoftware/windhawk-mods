// ==WindhawkMod==
// @id              ultimate-custom-tray
// @name            Ultimate Custom Tray
// @description     Custom tray icons with actions, context menus and image icon support.
// @version         1.0
// @author          Salyts
// @license         MIT
// @github          https://github.com/Salyts
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -lshlwapi -lcomdlg32 -luuid -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ultimate Custom Tray

Adds your own icons to the system tray. Each icon can run an
action on left-click and/or show a context menu on right-click.

---

## Quick start

1. Open Windhawk settings for this mod.
2. Add one or more **Items** to the list.
3. Fill in **Label**, **Icon** and **Action** for each item.
4. Save — the icons appear in the tray immediately.

---

## Action formats

Actions run on **left-click** (or from a context menu item).

| Prefix | Example | Description |
|--------|---------|-------------|
| `" "` | `"C:\Program Files\app.exe"` | Opens a file or folder by absolute path. |
| `~` | `~Downloads` | Opens a folder or file by name. |
| `cmd:` | `cmd:control` | Runs a command through `cmd.exe`. |
| `shell:` | `shell:shutdown /r /f /t 0` | Runs through `powershell.exe`. |
| `web:` | `web:https://windhawk.net/` | Opens a URL in the default browser. |
| `ms-settings:` | `ms-settings:bluetooth` | Opens a Windows Settings page. |

---

## Icon field

| Type | Example | Description |
|---|---|---|
| **Glyph** | `E774` | Hex code of a [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font) glyph. 4-digit hex only, no `\u` prefix. |
| **Image file** | `C:\Icons\name.png` | Full path to an image. Supported: `.png` `.ico` `.jpg` `.bmp`. Recommended: 32x32 px, transparent background. |

---

## Icon color

Use the **Icon color** setting to choose **White** (for dark taskbar) or **Black** (for light taskbar).

---

## Context menus

Each tray icon can show a right-click context menu.

- Add items under **Context menu** for the icon.
- Each item has its own **Name**, **Icon** and **Action**.
- If no items are defined, right-clicking does nothing.
- Left-click always runs the main **Action**, regardless of whether a context menu exists.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icon_color: white
  $name: Icon color
  $description: "White for dark taskbar, black for light taskbar."
  $options:
  - white: White
  - black: Black
- items:
    - - label: "Explorer"
        $name: Label
        $description: "Tooltip shown when hovering over the tray icon."
      - icon: "EC50"
        $name: Icon
      - action: "cmd:explorer"
        $name: Action
      - context_menu:
          - - name: "Downloads"
            - icon: "EC4F"
            - action: "~Downloads"
          - - name: "Documents"
            - icon: "E8A5"
            - action: "~Documents"
          - - name: "Pictures"
            - icon: "E91B"
            - action: "~Pictures"
          - - name: "Videos"
            - icon: "E714"
            - action: "~Videos"
          - - name: "Music"
            - icon: "E8D6"
            - action: "~Music"
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
#include <gdiplus.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

struct ContextMenuItem {
    std::wstring name;
    std::wstring icon;
    std::wstring action;
};

struct TrayItem {
    std::wstring label;
    std::wstring action;
    std::wstring iconRaw;
    HICON        hIcon = nullptr;
    GUID         guid  = {};
    std::vector<ContextMenuItem> contextMenu;
};

static std::vector<TrayItem> g_items;
static std::mutex            g_itemsMutex;
static HINSTANCE             g_hInstance      = nullptr;
static HWND                  g_trayHwnd       = nullptr;
static const UINT            WM_TRAY_CB       = WM_USER + 100;
static const UINT            WM_RELOAD        = WM_USER + 101;
static ULONG_PTR             g_gdiplusToken   = 0;
static UINT                  WM_TASKBARCREATED = 0;
static bool                  g_iconColorWhite = true;

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
    if (s.size() >= 2 && s[1] == L':')   return true;
    if (s.size() >= 2 && s[0] == L'\\')  return true;
    std::wstring lo = ToLower(s);
    for (auto& ext : { std::wstring(L".png"),  std::wstring(L".ico"),
                       std::wstring(L".jpg"),  std::wstring(L".jpeg"),
                       std::wstring(L".bmp") })
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
        Wh_Log(L"GDI+ could not load image: %s  (GDI+ status %d)",
               path.c_str(), (int)bmp.GetLastStatus());
        return nullptr;
    }
    return BitmapToHIcon(&bmp, size);
}

static HICON CreateGlyphIcon(WCHAR glyph, int size) {
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
    DrawTextW(memDC, &glyph, 1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

    DWORD* pPx = (DWORD*)bits;
    if (g_iconColorWhite) {
        for (int i = 0; i < size * size; i++) {
            BYTE b = (BYTE)(pPx[i] & 0xFF);
            pPx[i] = b > 0 ? (0x00FFFFFF | ((DWORD)b << 24)) : 0;
        }
    } else {
        for (int i = 0; i < size * size; i++) {
            BYTE b = (BYTE)(pPx[i] & 0xFF);
            pPx[i] = b > 0 ? ((DWORD)b << 24) : 0;
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

static HICON MakeIcon(const std::wstring& iconRaw, int size) {
    if (iconRaw.empty())
        return CreateGlyphIcon(L'\uE700', size);

    if (IsImagePath(iconRaw)) {
        if (GetFileAttributesW(iconRaw.c_str()) == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Icon file not found: %s", iconRaw.c_str());
            return CreateGlyphIcon(L'\uE783', size);
        }
        HICON h = LoadImageIconGdiplus(iconRaw, size);
        if (h) return h;
        Wh_Log(L"GDI+ load failed for: %s -- using fallback glyph", iconRaw.c_str());
        return CreateGlyphIcon(L'\uE783', size);
    }

    WCHAR glyph = (WCHAR)wcstoul(iconRaw.c_str(), nullptr, 16);
    if (glyph == 0) glyph = L'\uE700';
    return CreateGlyphIcon(glyph, size);
}

static bool GetKnownFolderPath(const wchar_t* name, std::wstring& out) {
    KNOWNFOLDERID fid{};
    std::wstring n = ToLower(name);
    if      (n == L"downloads")                     fid = FOLDERID_Downloads;
    else if (n == L"documents" || n == L"personal") fid = FOLDERID_Documents;
    else if (n == L"music")                         fid = FOLDERID_Music;
    else if (n == L"pictures")                      fid = FOLDERID_Pictures;
    else if (n == L"videos")                        fid = FOLDERID_Videos;
    else if (n == L"desktop")                       fid = FOLDERID_Desktop;
    else if (n == L"profile" || n == L"home")       fid = FOLDERID_Profile;
    else return false;

    PWSTR raw = nullptr;
    bool ok = SUCCEEDED(SHGetKnownFolderPath(fid, 0, nullptr, &raw)) && raw;
    if (ok) out = raw;
    if (raw) CoTaskMemFree(raw);
    return ok;
}

static void ExecuteAction(const std::wstring& raw) {
    if (raw.empty()) return;

    std::thread([raw]() {
        const std::wstring& a = raw;

        if (StartsWithCI(a, L"web:")) {
            ShellExecuteW(nullptr, L"open", a.substr(4).c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }
        if (StartsWithCI(a, L"ms-settings:")) {
            ShellExecuteW(nullptr, L"open", a.c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }
        if (StartsWithCI(a, L"cmd:")) {
            std::wstring arg = L"/C " + a.substr(4);
            ShellExecuteW(nullptr, L"open", L"cmd.exe",
                          arg.c_str(), nullptr, SW_HIDE);
            return;
        }
        if (StartsWithCI(a, L"shell:")) {
            std::wstring arg =
                L"-NoProfile -ExecutionPolicy Bypass -Command " + a.substr(6);
            ShellExecuteW(nullptr, L"open", L"powershell.exe",
                          arg.c_str(), nullptr, SW_HIDE);
            return;
        }
        if (!a.empty() && a.front() == L'~') {
            std::wstring target = a.substr(1);
            std::wstring resolved;
            if (GetKnownFolderPath(target.c_str(), resolved)) {
                ShellExecuteW(nullptr, L"open", resolved.c_str(),
                              nullptr, nullptr, SW_SHOWNORMAL);
                return;
            }
            wchar_t buf[MAX_PATH * 4]{};
            if (SearchPathW(nullptr, target.c_str(), nullptr,
                            ARRAYSIZE(buf), buf, nullptr)) {
                ShellExecuteW(nullptr, L"open", buf,
                              nullptr, nullptr, SW_SHOWNORMAL);
                return;
            }
            Wh_Log(L"~search: '%s' not found", target.c_str());
            return;
        }

        std::wstring path = StripQuotes(a);
        ShellExecuteW(nullptr, L"open", path.c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
    }).detach();
}

static HBITMAP MakeMenuBitmap(const std::wstring& iconRaw) {
    const int sz = 16;
    if (iconRaw.empty()) return nullptr;

    if (IsImagePath(iconRaw)) {
        if (GetFileAttributesW(iconRaw.c_str()) == INVALID_FILE_ATTRIBUTES)
            return nullptr;

        Gdiplus::Bitmap bmp(iconRaw.c_str());
        if (bmp.GetLastStatus() != Gdiplus::Ok) return nullptr;

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

        Gdiplus::Graphics g(memDC);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.DrawImage(&bmp, 0, 0, sz, sz);

        SelectObject(memDC, oldBmp);
        DeleteDC(memDC);
        ReleaseDC(nullptr, hdc);
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
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe Fluent Icons");
    HFONT oldFont = (HFONT)SelectObject(memDC, font);
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(60, 60, 60));
    RECT rc = { 0, 0, sz, sz };
    DrawTextW(memDC, &glyph, 1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(memDC, oldFont);
    DeleteObject(font);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, hdc);
    return dib;
}

static void ShowContextMenu(HWND hWnd, UINT itemId,
                            const std::vector<ContextMenuItem>& items) {
    if (items.empty()) return;

    SetForegroundWindow(hWnd);
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    std::vector<HBITMAP> bitmaps;
    bitmaps.reserve(items.size());

    for (size_t i = 0; i < items.size(); ++i) {
        const auto& mi = items[i];
        std::wstring txt = mi.name.empty() ? L"(unnamed)" : mi.name;

        MENUITEMINFOW mii = {};
        mii.cbSize     = sizeof(mii);
        mii.fMask      = MIIM_ID | MIIM_STRING;
        mii.wID        = (UINT)(1000 + i);
        mii.dwTypeData = txt.data();
        mii.cch        = (UINT)txt.size();

        HBITMAP hBmp = MakeMenuBitmap(mi.icon);
        if (hBmp) {
            mii.fMask    |= MIIM_BITMAP;
            mii.hbmpItem  = hBmp;
            bitmaps.push_back(hBmp);
        }
        InsertMenuItemW(hMenu, (UINT)i, TRUE, &mii);
    }

    POINT pt;
    GetCursorPos(&pt);
    int cmd = TrackPopupMenu(hMenu,
                             TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
                             pt.x, pt.y, 0, hWnd, nullptr);
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
        NOTIFYICONDATAW nid = {};
        nid.cbSize           = sizeof(nid);
        nid.hWnd             = g_trayHwnd;
        nid.uID              = (UINT)i;
        nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
        nid.guidItem         = g_items[i].guid;
        nid.uCallbackMessage = WM_TRAY_CB;
        nid.hIcon            = g_items[i].hIcon;
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

        nid.uFlags          |= NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAY_CB;
        nid.hIcon            = g_items[i].hIcon;
        lstrcpynW(nid.szTip, g_items[i].label.c_str(), 128);

        if (!Shell_NotifyIconW(NIM_MODIFY, &nid))
            Shell_NotifyIconW(NIM_ADD, &nid);
    }
}

static void DestroyAllIcons() {
    std::lock_guard<std::mutex> lk(g_itemsMutex);
    for (auto& item : g_items)
        if (item.hIcon) { DestroyIcon(item.hIcon); item.hIcon = nullptr; }
}

static void LoadAllSettings() {
    PCWSTR pColor = Wh_GetStringSetting(L"icon_color");
    g_iconColorWhite = true;
    if (pColor) {
        if (wcscmp(pColor, L"black") == 0)
            g_iconColorWhite = false;
        Wh_FreeStringSetting(pColor);
    }

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

        bool allEmpty = labelStr.empty() && actionStr.empty() && iconStr.empty();
        if (allEmpty) {
            if (i >= (int)newItems.size() + 2) break;
            continue;
        }

        TrayItem item;
        item.label   = labelStr.empty() ? L"Tray" : labelStr;
        item.action  = actionStr;
        item.iconRaw = iconStr.empty() ? L"E700" : iconStr;
        item.hIcon   = MakeIcon(item.iconRaw, 32);
        item.guid    = { 0x57595321, (WORD)i, 0x1234,
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

            ContextMenuItem cmi;
            cmi.name   = cName;
            cmi.icon   = pCI ? pCI : L"";
            cmi.action = pCA ? pCA : L"";
            if (pCI) Wh_FreeStringSetting(pCI);
            if (pCA) Wh_FreeStringSetting(pCA);

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

        if (event == WM_LBUTTONUP) {
            std::wstring act;
            {
                std::lock_guard<std::mutex> lk(g_itemsMutex);
                if (id < g_items.size()) act = g_items[id].action;
            }
            ExecuteAction(act);
        } else if (event == WM_RBUTTONUP) {
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

    if (WM_TASKBARCREATED != 0 && msg == WM_TASKBARCREATED) {
        Wh_Log(L"TaskbarCreated received -- restoring tray icons");
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

    bool isExcluded = false;
    bool isToolModProcess = false;
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
    si.cb = sizeof(STARTUPINFOW);
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
