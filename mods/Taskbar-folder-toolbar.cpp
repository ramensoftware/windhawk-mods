// ==WindhawkMod==
// @id              taskbar-folder-toolbar
// @name            Taskbar Folder Toolbar
// @description     Adds one or more folders as tray icons to the taskbar. Clicking opens the folder as a quick-launch menu—just like the Windows 10 toolbars.
// @version         2.0.0
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

Adds one or more folder icons to the system tray area of the Windows 11 taskbar.
Left-clicking opens the configured folder as a menu showing icons and names.
Clicking an entry launches the file or shortcut directly.

## Setup
1. Open the Windhawk mod settings
2. Add one entry per folder you want as a tray icon
3. For each entry configure:
   - **Folder path** – e.g. `C:\Users\Name\TaskbarLinks`
   - **Tooltip text** – shown when hovering over the tray icon
   - **Sort mode** – `name`, `type` (folders first), or `date`
   - **Custom icon** *(optional)* – see below
4. Restart Explorer or reload the mod

## Choosing a custom icon
Right-click any tray icon and select **"🎨 Choose icon..."**.
This opens the standard Windows icon picker, pre-loaded with `shell32.dll`
(or the currently configured icon file). Browse to any `.ico`, `.exe`, or `.dll`.
After clicking OK, the resulting value (e.g. `C:\Windows\System32\shell32.dll,13`)
is **automatically copied to the clipboard** — just paste it into the
"Custom icon" field in the mod settings.

> Note: Windhawk settings panels only support plain form fields; custom buttons
> or dialogs cannot be embedded there. The right-click approach is the intended
> workflow for selecting icons.

## Notes
- `.lnk` and `.url` extensions are hidden automatically in the menu
- Subfolders show a ▶ arrow and expand one level deep
- Right-click a tray icon: Open folder / Refresh / Choose icon
- Each folder gets its own independent tray icon
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folders:
  - - folderPath: ""
      $name: Folder path
      $description: "Full path, e.g. C:\\Users\\Name\\TaskbarLinks"
    - tooltipLabel: My Toolbar
      $name: Tooltip text
      $description: Text shown when hovering over the tray icon
    - sortMode: type
      $name: Sort mode
      $description: "name = alphabetical, type = folders first, date = newest first"
    - iconPath: ""
      $name: Custom icon (optional)
      $description: "Right-click the tray icon and choose 'Choose icon...' to pick one visually — the result is copied to your clipboard automatically. Or enter manually: path to a .ico file, or C:\\Windows\\System32\\shell32.dll,4 syntax for icons inside EXEs/DLLs. Leave empty to use the folder's own icon."
  $name: Folders
  $description: "Add one entry per folder. Each gets its own tray icon."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
#define WM_TRAYICON      (WM_USER + 100)
#define IDM_OPEN_FOLDER  9000
#define IDM_REFRESH      9001
#define IDM_PICK_ICON    9002
#define BASE_TRAY_UID    100   // tray UID = BASE + index; supports up to 16 folders

// ---------------------------------------------------------------------------
// Data structures
// ---------------------------------------------------------------------------
struct FolderSettings {
    std::wstring folderPath;
    std::wstring tooltipLabel;
    std::wstring sortMode;   // "name" | "type" | "date"
    std::wstring iconPath;   // optional: .ico path or exe,index
};

struct XFolderItem {
    std::wstring path;
    std::wstring displayName;
    HICON        icon        = nullptr;
    FILETIME     lastWrite   = {};
    bool         isDirectory = false;
};

// One instance per configured folder
struct FolderInstance {
    FolderSettings           settings;
    int                      index      = 0;
    HWND                     msgWnd     = nullptr;
    NOTIFYICONDATAW          nid        = {};
    bool                     nidAdded   = false;
    std::thread              thread;
    std::wstring             wndClassName;
    std::vector<XFolderItem> items;          // rebuilt on each menu open

    FolderInstance() = default;
    FolderInstance(const FolderInstance&) = delete;
    FolderInstance& operator=(const FolderInstance&) = delete;
};

// ---------------------------------------------------------------------------
// Global instance list
// ---------------------------------------------------------------------------
static std::vector<FolderInstance*> g_instances;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static void FreeItems(std::vector<XFolderItem>& items) {
    for (auto& i : items) {
        if (i.icon) { DestroyIcon(i.icon); i.icon = nullptr; }
    }
    items.clear();
}

// Strip .lnk / .url from display name
static std::wstring StripKnownExt(const std::wstring& name) {
    auto dot = name.rfind(L'.');
    if (dot == std::wstring::npos) return name;
    std::wstring ext = name.substr(dot);
    if (_wcsicmp(ext.c_str(), L".lnk") == 0 ||
        _wcsicmp(ext.c_str(), L".url") == 0)
        return name.substr(0, dot);
    return name;
}

// Convert HICON to a 32-bit HBITMAP suitable for menu item bitmaps
static HBITMAP IconToBitmap(HICON hIcon, int size = 16) {
    if (!hIcon) return nullptr;
    HDC hdcScreen = GetDC(nullptr);
    HDC hdc       = CreateCompatibleDC(hdcScreen);

    BITMAPV5HEADER bi = {};
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = size;
    bi.bV5Height      = -size;
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
        RECT rc = { 0, 0, size, size };
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        DrawIconEx(hdc, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);
        SelectObject(hdc, old);
    }

    DeleteDC(hdc);
    ReleaseDC(nullptr, hdcScreen);
    return hBmp;
}

// Load a custom icon from a .ico file or "exe,index" path
static HICON LoadCustomIcon(const std::wstring& iconPath) {
    if (iconPath.empty()) return nullptr;

    size_t comma = iconPath.rfind(L',');
    if (comma != std::wstring::npos) {
        std::wstring filePath = iconPath.substr(0, comma);
        int idx = _wtoi(iconPath.substr(comma + 1).c_str());
        HICON hSmall = nullptr;
        ExtractIconExW(filePath.c_str(), idx, nullptr, &hSmall, 1);
        return hSmall;
    }

    return (HICON)LoadImageW(nullptr, iconPath.c_str(),
        IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
}

// ---------------------------------------------------------------------------
// Read and sort folder contents
// ---------------------------------------------------------------------------
static void LoadFolderItems(const std::wstring& folderPath,
                             const std::wstring& sortMode,
                             std::vector<XFolderItem>& out)
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

        SHFILEINFOW sfi = {};
        SHGetFileInfoW(item.path.c_str(), 0, &sfi, sizeof(sfi),
            SHGFI_ICON | SHGFI_SMALLICON);
        item.icon = sfi.hIcon;

        out.push_back(std::move(item));
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    std::sort(out.begin(), out.end(),
        [&sortMode](const XFolderItem& a, const XFolderItem& b) -> bool {
            if (sortMode == L"type") {
                if (a.isDirectory != b.isDirectory)
                    return a.isDirectory > b.isDirectory;
                return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
            }
            if (sortMode == L"date")
                return CompareFileTime(&a.lastWrite, &b.lastWrite) > 0;
            return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
        });
}

// ---------------------------------------------------------------------------
// Populate menu recursively (one subfolder level deep).
// Every menu ID is mapped to its full path in idToPath.
// ---------------------------------------------------------------------------
static void PopulateMenu(HMENU hMenu,
                          const std::vector<XFolderItem>& items,
                          const std::wstring& sortMode,
                          std::vector<HBITMAP>& bitmapsToFree,
                          std::map<int, std::wstring>& idToPath,
                          int& idCounter)
{
    for (const auto& item : items) {
        if (item.isDirectory) {
            HMENU hSub = CreatePopupMenu();

            int openId = idCounter++;
            idToPath[openId] = item.path;
            std::wstring openLabel = L"📂 Open " + item.displayName;
            AppendMenuW(hSub, MF_STRING, openId, openLabel.c_str());
            AppendMenuW(hSub, MF_SEPARATOR, 0, nullptr);

            std::vector<XFolderItem> subItems;
            LoadFolderItems(item.path, sortMode, subItems);
            PopulateMenu(hSub, subItems, sortMode, bitmapsToFree, idToPath, idCounter);
            FreeItems(subItems);

            MENUITEMINFOW mii = {};
            mii.cbSize     = sizeof(mii);
            mii.fMask      = MIIM_STRING | MIIM_SUBMENU | MIIM_BITMAP;
            std::wstring label = L"▶  " + item.displayName;
            mii.dwTypeData = const_cast<LPWSTR>(label.c_str());
            mii.hSubMenu   = hSub;

            HBITMAP hBmp = IconToBitmap(item.icon);
            if (hBmp) { mii.hbmpItem = hBmp; bitmapsToFree.push_back(hBmp); }
            InsertMenuItemW(hMenu, GetMenuItemCount(hMenu), TRUE, &mii);

        } else {
            int fileId = idCounter++;
            idToPath[fileId] = item.path;

            MENUITEMINFOW mii = {};
            mii.cbSize     = sizeof(mii);
            mii.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
            mii.wID        = fileId;
            mii.dwTypeData = const_cast<LPWSTR>(item.displayName.c_str());

            HBITMAP hBmp = IconToBitmap(item.icon);
            if (hBmp) { mii.hbmpItem = hBmp; bitmapsToFree.push_back(hBmp); }
            InsertMenuItemW(hMenu, GetMenuItemCount(hMenu), TRUE, &mii);
        }
    }
}

// ---------------------------------------------------------------------------
// Icon picker dialog
// Uses the Windows built-in PickIconDlg from shell32.dll.
// Pre-loads shell32.dll (or the currently configured file).
// Copies the "path,index" result to the clipboard automatically.
// ---------------------------------------------------------------------------
typedef BOOL (WINAPI *PfnPickIconDlg)(HWND, LPWSTR, UINT, int*);

static void ShowIconPickerDialog(FolderInstance* inst) {
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) return;

    PfnPickIconDlg pfnPickIcon =
        reinterpret_cast<PfnPickIconDlg>(
            GetProcAddress(hShell32, "PickIconDlg"));
    if (!pfnPickIcon) {
        MessageBoxW(inst->msgWnd,
            L"PickIconDlg could not be loaded from shell32.dll.",
            L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    wchar_t iconFile[MAX_PATH] = {};
    int iconIndex = 0;

    // Pre-fill with current iconPath, or default to shell32.dll
    if (!inst->settings.iconPath.empty()) {
        size_t comma = inst->settings.iconPath.rfind(L',');
        if (comma != std::wstring::npos) {
            wcsncpy_s(iconFile,
                      inst->settings.iconPath.substr(0, comma).c_str(),
                      MAX_PATH - 1);
            iconIndex = _wtoi(inst->settings.iconPath.substr(comma + 1).c_str());
        } else {
            wcsncpy_s(iconFile, inst->settings.iconPath.c_str(), MAX_PATH - 1);
        }
    } else {
        ExpandEnvironmentStringsW(
            L"%SystemRoot%\\System32\\shell32.dll",
            iconFile, MAX_PATH);
    }

    if (!pfnPickIcon(inst->msgWnd, iconFile, MAX_PATH, &iconIndex))
        return;   // user cancelled

    std::wstring result = std::wstring(iconFile)
                        + L","
                        + std::to_wstring(iconIndex);

    // Copy result to clipboard
    if (OpenClipboard(inst->msgWnd)) {
        EmptyClipboard();
        size_t bytes = (result.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (hMem) {
            wchar_t* p = static_cast<wchar_t*>(GlobalLock(hMem));
            if (p) {
                wcscpy_s(p, result.size() + 1, result.c_str());
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
        CloseClipboard();
    }

    std::wstring msg =
        L"Icon value copied to clipboard:\n\n"
        + result +
        L"\n\nPaste it into the \"Custom icon\" field in the\n"
        L"Windhawk mod settings (Ctrl+V), then save.";
    MessageBoxW(inst->msgWnd, msg.c_str(), L"Choose icon", MB_OK | MB_ICONINFORMATION);
}

// ---------------------------------------------------------------------------
// Show the tray icon context menu
// ---------------------------------------------------------------------------
static void ShowFolderMenu(FolderInstance* inst, bool rightClick) {
    if (inst->settings.folderPath.empty()) {
        MessageBoxW(nullptr,
            L"No folder path configured.\n\n"
            L"Please enter a folder path in the Windhawk mod settings.",
            L"Taskbar Folder Toolbar", MB_OK | MB_ICONINFORMATION);
        return;
    }

    LoadFolderItems(inst->settings.folderPath, inst->settings.sortMode, inst->items);

    HMENU hMenu = CreatePopupMenu();
    std::vector<HBITMAP> bitmapsToFree;
    std::map<int, std::wstring> idToPath;
    int idCounter = 1;

    if (rightClick) {
        AppendMenuW(hMenu, MF_STRING, IDM_OPEN_FOLDER, L"Open folder...");
        AppendMenuW(hMenu, MF_STRING, IDM_REFRESH,     L"Refresh");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, IDM_PICK_ICON,   L"🎨 Choose icon...");
    } else {
        if (inst->items.empty()) {
            AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, L"(Folder is empty)");
        } else {
            PopulateMenu(hMenu, inst->items, inst->settings.sortMode,
                         bitmapsToFree, idToPath, idCounter);
        }
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, IDM_OPEN_FOLDER, L"📁 Open folder...");
        AppendMenuW(hMenu, MF_STRING, IDM_REFRESH,     L"🔄 Refresh");
    }

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(inst->msgWnd);

    int cmd = TrackPopupMenuEx(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, inst->msgWnd, nullptr);

    DestroyMenu(hMenu);
    for (HBITMAP hBmp : bitmapsToFree) DeleteObject(hBmp);
    FreeItems(inst->items);

    if (cmd == IDM_OPEN_FOLDER) {
        ShellExecuteW(nullptr, L"explore",
            inst->settings.folderPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == IDM_REFRESH) {
        // Items are reloaded fresh on the next menu open — nothing to do here
    } else if (cmd == IDM_PICK_ICON) {
        ShowIconPickerDialog(inst);
    } else if (cmd > 0) {
        auto it = idToPath.find(cmd);
        if (it != idToPath.end()) {
            ShellExecuteW(nullptr, L"open",
                it->second.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
    }
}

// ---------------------------------------------------------------------------
// Set up or update the tray icon for one instance
// ---------------------------------------------------------------------------
static void SetupTrayIcon(FolderInstance* inst, bool update = false) {
    HICON hIcon = nullptr;

    // 1. Custom icon (if configured)
    if (!inst->settings.iconPath.empty())
        hIcon = LoadCustomIcon(inst->settings.iconPath);

    // 2. Icon of the configured folder path
    if (!hIcon && !inst->settings.folderPath.empty()) {
        SHFILEINFOW sfi = {};
        SHGetFileInfoW(inst->settings.folderPath.c_str(), 0, &sfi, sizeof(sfi),
            SHGFI_ICON | SHGFI_SMALLICON);
        hIcon = sfi.hIcon;
    }

    // 3. Generic folder icon fallback
    if (!hIcon) {
        SHFILEINFOW sfi = {};
        SHGetFileInfoW(L"C:\\", FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi),
            SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
        hIcon = sfi.hIcon;
    }

    inst->nid.cbSize           = sizeof(inst->nid);
    inst->nid.hWnd             = inst->msgWnd;
    inst->nid.uID              = BASE_TRAY_UID + inst->index;
    inst->nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    inst->nid.uCallbackMessage = WM_TRAYICON;
    inst->nid.hIcon            = hIcon;
    inst->nid.uVersion         = NOTIFYICON_VERSION_4;

    const std::wstring& tip = inst->settings.tooltipLabel;
    wcsncpy_s(inst->nid.szTip,
              tip.empty() ? L"Toolbar" : tip.c_str(),
              _TRUNCATE);

    if (update && inst->nidAdded) {
        Shell_NotifyIconW(NIM_MODIFY, &inst->nid);
    } else {
        Shell_NotifyIconW(NIM_ADD, &inst->nid);
        Shell_NotifyIconW(NIM_SETVERSION, &inst->nid);
        inst->nidAdded = true;
    }

    if (hIcon) DestroyIcon(hIcon);
}

// ---------------------------------------------------------------------------
// Window procedure for the message-only window.
// FolderInstance* is stored via GWLP_USERDATA.
// ---------------------------------------------------------------------------
static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg,
                                    WPARAM wParam, LPARAM lParam)
{
    FolderInstance* inst =
        reinterpret_cast<FolderInstance*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_TRAYICON: {
        if (!inst) break;
        UINT event = LOWORD(lParam);
        if (event == WM_LBUTTONUP)
            ShowFolderMenu(inst, false);
        else if (event == WM_RBUTTONUP)
            ShowFolderMenu(inst, true);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Message loop — runs in its own thread, one per folder instance
// ---------------------------------------------------------------------------
static void MessageLoop(FolderInstance* inst) {
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    wchar_t clsName[64];
    swprintf_s(clsName, L"Wh_FolderTB_%d", inst->index);
    inst->wndClassName = clsName;

    UnregisterClassW(inst->wndClassName.c_str(), hInst);

    WNDCLASSEXW wc   = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = MsgWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = inst->wndClassName.c_str();

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed for instance %d (GLE=%u)",
               inst->index, GetLastError());
        return;
    }

    inst->msgWnd = CreateWindowExW(0, inst->wndClassName.c_str(), nullptr, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);

    if (!inst->msgWnd) {
        Wh_Log(L"CreateWindowExW failed for instance %d (GLE=%u)",
               inst->index, GetLastError());
        UnregisterClassW(inst->wndClassName.c_str(), hInst);
        return;
    }

    SetWindowLongPtrW(inst->msgWnd, GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(inst));

    inst->nidAdded = false;
    SetupTrayIcon(inst);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (inst->nidAdded) {
        Shell_NotifyIconW(NIM_DELETE, &inst->nid);
        inst->nidAdded = false;
    }
    DestroyWindow(inst->msgWnd);
    inst->msgWnd = nullptr;
    UnregisterClassW(inst->wndClassName.c_str(), hInst);
}

// ---------------------------------------------------------------------------
// Load all folder settings from Windhawk
// ---------------------------------------------------------------------------
static std::vector<FolderSettings> LoadAllSettings() {
    std::vector<FolderSettings> result;
    wchar_t key[128];

    for (int i = 0; i < 16; i++) {
        swprintf_s(key, L"folders[%d].folderPath", i);
        PCWSTR path = Wh_GetStringSetting(key);
        bool empty = (!path || !*path);
        Wh_FreeStringSetting(path);
        if (empty) break;

        path = Wh_GetStringSetting(key);
        FolderSettings s;
        s.folderPath = path ? path : L"";
        Wh_FreeStringSetting(path);

        swprintf_s(key, L"folders[%d].tooltipLabel", i);
        PCWSTR label = Wh_GetStringSetting(key);
        s.tooltipLabel = (label && *label) ? label : L"My Toolbar";
        Wh_FreeStringSetting(label);

        swprintf_s(key, L"folders[%d].sortMode", i);
        PCWSTR sort = Wh_GetStringSetting(key);
        s.sortMode = (sort && *sort) ? sort : L"type";
        Wh_FreeStringSetting(sort);

        swprintf_s(key, L"folders[%d].iconPath", i);
        PCWSTR icon = Wh_GetStringSetting(key);
        s.iconPath = icon ? icon : L"";
        Wh_FreeStringSetting(icon);

        result.push_back(std::move(s));
    }

    return result;
}

// ---------------------------------------------------------------------------
// Instance management
// ---------------------------------------------------------------------------
static void StopAllInstances() {
    for (FolderInstance* inst : g_instances) {
        if (inst->msgWnd)
            PostMessageW(inst->msgWnd, WM_QUIT, 0, 0);
        if (inst->thread.joinable())
            inst->thread.join();
        FreeItems(inst->items);
        delete inst;
    }
    g_instances.clear();
}

static void StartInstances(const std::vector<FolderSettings>& settings) {
    for (int i = 0; i < static_cast<int>(settings.size()); i++) {
        FolderInstance* inst = new FolderInstance();
        inst->settings = settings[i];
        inst->index    = i;
        inst->thread   = std::thread(MessageLoop, inst);
        g_instances.push_back(inst);
    }
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Folder Toolbar v2: Init");

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    StartInstances(LoadAllSettings());
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Taskbar Folder Toolbar v2: Uninit");
    StopAllInstances();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Taskbar Folder Toolbar v2: Settings changed");
    StopAllInstances();
    StartInstances(LoadAllSettings());
}