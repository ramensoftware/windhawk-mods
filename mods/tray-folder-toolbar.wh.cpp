// ==WindhawkMod==
// @id              Tray-folder-toolbar
// @name            Tray Folder Toolbar
// @description     Adds a folder as a tray icon to the taskbar. Clicking it opens the folder's contents as a quick-launch menu—just like the Windows 10 toolbars.
// @version         1.0.0
// @author          Rene Mayer
// @github          https://github.com/renemayer-hb
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// @compilerOptions -lshell32 -lshlwapi -lcomctl32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Folder Toolbar

Adds a folder icon to the system tray area of the Windows 11 taskbar.
Left-clicking opens the configured folder as a menu displaying icons and names.
Clicking an entry opens the file or shortcut directly.

## Setup
1. Create a folder, e.g., `C:\Users\Name\TaskbarLinks`
2. Place shortcuts, programs, or files there
3. Enter the folder path in the Windhawk settings
4. Customize the tooltip text as desired
5. Restart Explorer or reload the mod

## Notes
- `.lnk` and `.url` file extensions are automatically hidden in the menu
- Subfolders are displayed with a ▶ arrow and open the folder in Explorer
- Right-click on the tray icon: Open folder / Refresh
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folderPath: “”
  $name: Folder path
  $description: “Full path to the folder, e.g., C:\\Users\\Name\\TaskbarLinks”
- tooltipLabel: My Toolbar
  $name: Tooltip text
  $description: Label displayed when hovering over the tray icon
- sortMode: type
  $name: Sorting
  $description: “name = alphabetical, type = folders first, date = newest first”
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>



// ---------------------------------------------------------------------------
// Konstanten
// ---------------------------------------------------------------------------
#define WM_TRAYICON        (WM_USER + 100)
#define IDM_OPEN_FOLDER    9000
#define IDM_REFRESH        9001
#define TRAY_UID           42
#define WNDCLASS_NAME      L"Wh_FolderToolbarMsg"
#define MAX_SUBMENU_DEPTH  1   // Unterordner werden nur 1 Ebene tief aufgelöst

// ---------------------------------------------------------------------------
// Datenstrukturen
// ---------------------------------------------------------------------------
struct Settings {
    std::wstring folderPath;
    std::wstring tooltipLabel;
    std::wstring sortMode;   // "name" | "type" | "date"
};

struct XFolderItem {
    std::wstring path;
    std::wstring displayName;
    HICON        icon        = nullptr;
    FILETIME     lastWrite   = {};
    bool         isDirectory = false;
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static Settings           g_settings;
static HWND               g_msgWnd   = nullptr;
static NOTIFYICONDATAW    g_nid      = {};
static bool               g_nidAdded = false;
static std::thread        g_thread;
static std::atomic<bool>  g_running  = false;
static std::vector<XFolderItem> g_items;

// ---------------------------------------------------------------------------
// Hilfsfunktionen
// ---------------------------------------------------------------------------
static void FreeItems(std::vector<XFolderItem>& items) {
    for (auto& i : items) {
        if (i.icon) { DestroyIcon(i.icon); i.icon = nullptr; }
    }
    items.clear();
}

// Dateiname ohne Erweiterung bei .lnk / .url
static std::wstring StripKnownExt(const std::wstring& name) {
    auto dot = name.rfind(L'.');
    if (dot == std::wstring::npos) return name;
    std::wstring ext = name.substr(dot);
    if (_wcsicmp(ext.c_str(), L".lnk") == 0 ||
        _wcsicmp(ext.c_str(), L".url") == 0)
        return name.substr(0, dot);
    return name;
}

// HICON -> HBITMAP (32-bit, für Menüsymbole)
static HBITMAP IconToBitmap(HICON hIcon, int size = 16) {
    if (!hIcon) return nullptr;
    HDC hdcScreen = GetDC(nullptr);
    HDC hdc       = CreateCompatibleDC(hdcScreen);

    BITMAPV5HEADER bi = {};
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = size;
    bi.bV5Height      = -size;   // top-down
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    void* pvBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdc, (BITMAPINFO*)&bi,
        DIB_RGB_COLORS, &pvBits, nullptr, 0);

    if (hBmp) {
        HGDIOBJ old = SelectObject(hdc, hBmp);
        // Hintergrund transparent (ARGB 0)
        RECT rc = { 0, 0, size, size };
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        DrawIconEx(hdc, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);
        SelectObject(hdc, old);
    }

    DeleteDC(hdc);
    ReleaseDC(nullptr, hdcScreen);
    return hBmp;
}

// ---------------------------------------------------------------------------
// Ordner einlesen
// ---------------------------------------------------------------------------
static void LoadFolderItems(const std::wstring& folderPath,
                             std::vector<XFolderItem>& out,
                             int iconSize = 16)
{
    FreeItems(out);
    if (folderPath.empty()) return;

    std::wstring search = folderPath + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) continue;

        XFolderItem item;
        item.path        = folderPath + L"\\" + fd.cFileName;
        item.displayName = StripKnownExt(fd.cFileName);
        item.isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        item.lastWrite   = fd.ftLastWriteTime;

        // Symbol holen
        SHFILEINFOW sfi = {};
        SHGetFileInfoW(item.path.c_str(), 0, &sfi, sizeof(sfi),
            SHGFI_ICON | SHGFI_SMALLICON);
        item.icon = sfi.hIcon;

        out.push_back(std::move(item));
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    // Sortierung
    std::sort(out.begin(), out.end(),
        [](const XFolderItem& a, const XFolderItem& b) -> bool {
            const std::wstring& sort = g_settings.sortMode;
            if (sort == L"type") {
                if (a.isDirectory != b.isDirectory)
                    return a.isDirectory > b.isDirectory;
                return _wcsicmp(a.displayName.c_str(),
                                b.displayName.c_str()) < 0;
            }
            if (sort == L"date") {
                return CompareFileTime(&a.lastWrite, &b.lastWrite) > 0;
            }
            // default: name
            return _wcsicmp(a.displayName.c_str(),
                            b.displayName.c_str()) < 0;
        });
}

// ---------------------------------------------------------------------------
// Menü befüllen (rekursiv für 1 Ebene Unterordner)
// ---------------------------------------------------------------------------
static void PopulateMenu(HMENU hMenu,
                          const std::vector<XFolderItem>& items,
                          std::vector<HBITMAP>& bitmapsToFree,
                          int& idCounter)
{
    for (const auto& item : items) {
        if (item.isDirectory) {
            // Unterordner: Untermenü mit "Öffnen" + Inhalt
            HMENU hSub = CreatePopupMenu();

            // "Öffnen" als ersten Eintrag
            AppendMenuW(hSub, MF_STRING, idCounter++,
                        (L"📂 " + item.displayName + L" öffnen").c_str());
            AppendMenuW(hSub, MF_SEPARATOR, 0, nullptr);

            // 1 Ebene Unterordner einlesen
            std::vector<XFolderItem> subItems;
            LoadFolderItems(item.path, subItems);
            PopulateMenu(hSub, subItems, bitmapsToFree, idCounter);
            FreeItems(subItems);

            // Untermenü anhängen
            MENUITEMINFOW mii = {};
            mii.cbSize    = sizeof(mii);
            mii.fMask     = MIIM_STRING | MIIM_SUBMENU | MIIM_BITMAP;
            std::wstring label = L"▶  " + item.displayName;
            mii.dwTypeData = const_cast<LPWSTR>(label.c_str());
            mii.hSubMenu   = hSub;

            HBITMAP hBmp = IconToBitmap(item.icon);
            if (hBmp) {
                mii.hbmpItem = hBmp;
                bitmapsToFree.push_back(hBmp);
            }
            InsertMenuItemW(hMenu, GetMenuItemCount(hMenu), TRUE, &mii);
        } else {
            // Normale Datei / Verknüpfung
            MENUITEMINFOW mii = {};
            mii.cbSize    = sizeof(mii);
            mii.fMask     = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
            mii.wID       = idCounter++;
            mii.dwTypeData = const_cast<LPWSTR>(item.displayName.c_str());

            HBITMAP hBmp = IconToBitmap(item.icon);
            if (hBmp) {
                mii.hbmpItem = hBmp;
                bitmapsToFree.push_back(hBmp);
            }
            InsertMenuItemW(hMenu, GetMenuItemCount(hMenu), TRUE, &mii);
        }
    }
}

// ---------------------------------------------------------------------------
// Menü anzeigen
// ---------------------------------------------------------------------------
static void ShowFolderMenu(HWND hwnd, bool rightClick) {
    if (g_settings.folderPath.empty()) {
        MessageBoxW(nullptr,
            L"Kein Ordnerpfad konfiguriert.\n\n"
            L"Bitte in den Windhawk-Mod-Einstellungen einen Ordnerpfad eintragen.",
            L"Taskbar Folder Toolbar", MB_OK | MB_ICONINFORMATION);
        return;
    }

    LoadFolderItems(g_settings.folderPath, g_items);

    HMENU hMenu = CreatePopupMenu();
    std::vector<HBITMAP> bitmapsToFree;
    int idCounter = 1;   // IDs 1..N für Ordnereinträge

    if (rightClick) {
        // Rechtsklick: nur Verwaltungseinträge
        AppendMenuW(hMenu, MF_STRING, IDM_OPEN_FOLDER, L"Ordner öffnen...");
        AppendMenuW(hMenu, MF_STRING, IDM_REFRESH,     L"Aktualisieren");
    } else {
        // Linksklick: Ordnerinhalt
        if (g_items.empty()) {
            AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, L"(Ordner ist leer)");
        } else {
            PopulateMenu(hMenu, g_items, bitmapsToFree, idCounter);
        }
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, IDM_OPEN_FOLDER, L"📁 Ordner öffnen...");
        AppendMenuW(hMenu, MF_STRING, IDM_REFRESH,     L"🔄 Aktualisieren");
    }

    // Menü anzeigen: über der Taskleiste (BOTTOMALIGN)
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);

    int cmd = TrackPopupMenuEx(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, hwnd, nullptr);

    DestroyMenu(hMenu);
    for (HBITMAP hBmp : bitmapsToFree) DeleteObject(hBmp);

    // Aktion ausführen
    if (cmd == IDM_OPEN_FOLDER) {
        ShellExecuteW(nullptr, L"explore",
            g_settings.folderPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == IDM_REFRESH) {
        LoadFolderItems(g_settings.folderPath, g_items);
    } else if (cmd >= 1 && cmd < idCounter) {
        // Ordnereintrag: linearer Index in g_items
        // Unterordner-IDs zählen wir nicht nach; Öffnen des Unterordners
        // läuft über den "Öffnen"-Eintrag im Untermenü (erster Eintrag = id 1..N)
        int idx = cmd - 1;
        if (idx >= 0 && idx < (int)g_items.size()) {
            ShellExecuteW(nullptr, L"open",
                g_items[idx].path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
    }

    FreeItems(g_items);
}

// ---------------------------------------------------------------------------
// Tray-Symbol einrichten / aktualisieren
// ---------------------------------------------------------------------------
static void SetupTrayIcon(bool update = false) {
    // Symbol: Icon des konfigurierten Ordners
    HICON hIcon = nullptr;
    if (!g_settings.folderPath.empty()) {
        SHFILEINFOW sfi = {};
        SHGetFileInfoW(g_settings.folderPath.c_str(), 0, &sfi, sizeof(sfi),
            SHGFI_ICON | SHGFI_SMALLICON);
        hIcon = sfi.hIcon;
    }
    if (!hIcon) {
        // Fallback: Shell-Ordner-Icon
        SHFILEINFOW sfi = {};
        SHGetFileInfoW(L"C:\\", FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi),
            SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
        hIcon = sfi.hIcon;
    }

    g_nid.cbSize           = sizeof(g_nid);
    g_nid.hWnd             = g_msgWnd;
    g_nid.uID              = TRAY_UID;
    g_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon            = hIcon;
    g_nid.uVersion         = NOTIFYICON_VERSION_4;
    wcsncpy_s(g_nid.szTip,
              g_settings.tooltipLabel.empty()
                  ? L"Toolbar" : g_settings.tooltipLabel.c_str(),
              _TRUNCATE);

    if (update && g_nidAdded) {
        Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    } else {
        Shell_NotifyIconW(NIM_ADD, &g_nid);
        Shell_NotifyIconW(NIM_SETVERSION, &g_nid);
        g_nidAdded = true;
    }

    if (hIcon) DestroyIcon(hIcon);
}

// ---------------------------------------------------------------------------
// Fensterprozedur für Message-Only-Fenster
// ---------------------------------------------------------------------------
static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg,
                                    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_TRAYICON: {
        UINT event = LOWORD(lParam);
        if (event == WM_LBUTTONUP) {
            ShowFolderMenu(hwnd, false);
        } else if (event == WM_RBUTTONUP) {
            ShowFolderMenu(hwnd, true);
        }
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Message-Loop (läuft in eigenem Thread)
// ---------------------------------------------------------------------------
static void MessageLoop() {
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    // Klasse zuerst abmelden – falls Überrest aus vorheriger Session
    UnregisterClassW(WNDCLASS_NAME, hInst);

    WNDCLASSEXW wc   = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = MsgWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = WNDCLASS_NAME;

    // Klasse ggf. vom vorherigen Lauf erst abmelden
    UnregisterClassW(WNDCLASS_NAME, hInst);

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"Fehler: RegisterClassExW fehlgeschlagen (GLE=%u)", GetLastError());
        return;
    }

    g_msgWnd = CreateWindowExW(0, WNDCLASS_NAME, nullptr, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);

    if (!g_msgWnd) {
        Wh_Log(L"Fehler: CreateWindowExW fehlgeschlagen (GLE=%u)", GetLastError());
        UnregisterClassW(WNDCLASS_NAME, hInst);
        return;
    }

    // Zustand zurücksetzen bei Deaktivierung/Aktivierung
    g_nidAdded = false;
    SetupTrayIcon();

    // Reine WM_QUIT-gesteuerte Loop – kein g_running nötig
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Aufräumen nach WM_QUIT
    if (g_nidAdded) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_nidAdded = false;
    }
    DestroyWindow(g_msgWnd);
    g_msgWnd = nullptr;
    UnregisterClassW(WNDCLASS_NAME, hInst);
}

// ---------------------------------------------------------------------------
// Einstellungen laden
// ---------------------------------------------------------------------------
static void LoadSettings() {
    PCWSTR path  = Wh_GetStringSetting(L"folderPath");
    PCWSTR label = Wh_GetStringSetting(L"tooltipLabel");
    PCWSTR sort  = Wh_GetStringSetting(L"sortMode");

    g_settings.folderPath   = path  ? path  : L"";
    g_settings.tooltipLabel = label ? label : L"Meine Toolbar";
    g_settings.sortMode     = sort  ? sort  : L"type";

    Wh_FreeStringSetting(path);
    Wh_FreeStringSetting(label);
    Wh_FreeStringSetting(sort);
}

// ---------------------------------------------------------------------------
// Windhawk-Einstiegspunkte
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Folder Toolbar: Init");

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    LoadSettings();

    g_running = true;
    g_thread  = std::thread(MessageLoop);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Taskbar Folder Toolbar: Uninit");

    // WM_QUIT beendet GetMessageW sauber
    if (g_msgWnd) {
        PostMessageW(g_msgWnd, WM_QUIT, 0, 0);
    }

    if (g_thread.joinable()) {
        g_thread.join();
    }

    FreeItems(g_items);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Taskbar Folder Toolbar: Einstellungen geändert");
    LoadSettings();

    // Tray-Tooltip aktualisieren
    if (g_msgWnd && g_nidAdded) {
        SetupTrayIcon(true);
    }
}
