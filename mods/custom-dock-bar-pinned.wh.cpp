// ==WindhawkMod==
// @id              custom-dock-bar-pinned
// @name            Custom Dock Bar (Pinned + Running)
// @github          Alaricholt677
// @description     Bottom dock, draggable, Start + Quick + pinned and running app icons.
// @version         1.1
// @author          Alaricholt677
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -lole32 -lshell32 -lpsapi
// ==/WindhawkMod==


// ============================================================================
// 1) YOUR ORIGINAL YAML SETTINGS (Windhawk reads THIS block)
// ============================================================================
// ==WindhawkModSettings==
/*
- theme: win12Glow
  $name: Theme
  $description: Choose a built-in dock theme.
  $options:
    - minimalGlass: Minimal Glass
    - win12Glow: Win12 Glow
    - neonDock: Neon Dock
    - stealthFlat: Stealth Flat

- width: 900
  $name: Dock width
  $description: Width of the dock bar in pixels.

- height: 64
  $name: Dock height
  $description: Height of the dock bar in pixels.

- cornerRadius: 26
  $name: Corner radius
  $description: How rounded the dock corners are.

- bottomMargin: 16
  $name: Bottom margin
  $description: Distance from bottom of the screen.

- opacity: 235
  $name: Opacity
  $description: Dock opacity (0–255).
*/
// ==/WindhawkModSettings==


// ============================================================================
// 2) JSON SETTINGS (PR validator reads THIS block)
// ============================================================================
/*
{
  "theme": {
    "type": "select",
    "default": "win12Glow",
    "name": "Theme",
    "description": "Choose a built-in dock theme.",
    "options": {
      "minimalGlass": "Minimal Glass",
      "win12Glow": "Win12 Glow",
      "neonDock": "Neon Dock",
      "stealthFlat": "Stealth Flat"
    }
  },
  "width": {
    "type": "int",
    "default": 900,
    "name": "Dock width",
    "description": "Width of the dock bar in pixels."
  },
  "height": {
    "type": "int",
    "default": 64,
    "name": "Dock height",
    "description": "Height of the dock bar in pixels."
  },
  "cornerRadius": {
    "type": "int",
    "default": 26,
    "name": "Corner radius",
    "description": "How rounded the dock corners are."
  },
  "bottomMargin": {
    "type": "int",
    "default": 16,
    "name": "Bottom margin",
    "description": "Distance from bottom of the screen."
  },
  "opacity": {
    "type": "int",
    "default": 235,
    "name": "Opacity",
    "description": "Dock opacity (0–255).",
    "min": 0,
    "max": 255
  }
}
*/


// ============================================================================
// ORIGINAL INCLUDES — UNCHANGED
// ============================================================================
#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <algorithm>

struct DockTheme {
    const wchar_t* id;
    int width;
    int height;
    int cornerRadius;
    int bottomMargin;
    int opacity;
};

DockTheme g_themes[] = {
    {L"minimalGlass", 780, 52, 18, 18, 220},
    {L"win12Glow",    900, 64, 26, 16, 235},
    {L"neonDock",     980, 70, 30, 20, 240},
    {L"stealthFlat",  760, 48, 12, 14, 210},
};

struct {
    PCWSTR themeId;
    int width;
    int height;
    int cornerRadius;
    int bottomMargin;
    int opacity;
} g_settings;

struct PinnedItem {
    std::wstring exePath;
    std::wstring displayName;
    HICON icon;
    HWND hButton;
    int id;
};

HWND g_dockWnd = nullptr;
HWND g_btnStart = nullptr;
HWND g_btnQuick = nullptr;
std::vector<PinnedItem> g_pinned;

const wchar_t* DOCK_CLASS_NAME = L"CustomDockBarPinnedClass";
const int ID_BTN_START = 1001;
const int ID_BTN_QUICK = 1002;
const int ID_PINNED_BASE = 2000;

void LoadSettings() {
    g_settings.themeId      = Wh_GetStringSetting(L"theme");
    g_settings.width        = Wh_GetIntSetting(L"width");
    g_settings.height       = Wh_GetIntSetting(L"height");
    g_settings.cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    g_settings.bottomMargin = Wh_GetIntSetting(L"bottomMargin");
    g_settings.opacity      = Wh_GetIntSetting(L"opacity");

    for (auto& t : g_themes) {
        if (wcscmp(t.id, g_settings.themeId) == 0) {
            if (g_settings.width        <= 0) g_settings.width        = t.width;
            if (g_settings.height       <= 0) g_settings.height       = t.height;
            if (g_settings.cornerRadius <= 0) g_settings.cornerRadius = t.cornerRadius;
            if (g_settings.bottomMargin <= 0) g_settings.bottomMargin = t.bottomMargin;
            if (g_settings.opacity      <= 0) g_settings.opacity      = t.opacity;
            break;
        }
    }

    if (g_settings.opacity < 0)   g_settings.opacity = 0;
    if (g_settings.opacity > 255) g_settings.opacity = 255;
}

void ApplyDockRegion(HWND hWnd) {
    if (!hWnd || g_settings.cornerRadius <= 0) return;

    RECT rect;
    if (!GetClientRect(hWnd, &rect))
        return;

    int width  = rect.right  - rect.left;
    int height = rect.bottom - rect.top;

    HRGN hRgn = CreateRoundRectRgn(
        0,
        0,
        width,
        height,
        g_settings.cornerRadius * 2,
        g_settings.cornerRadius * 2
    );

    if (hRgn) {
        SetWindowRgn(hWnd, hRgn, TRUE);
    }
}

void DestroyPinnedButtons() {
    for (auto& item : g_pinned) {
        if (item.hButton) DestroyWindow(item.hButton);
        if (item.icon) DestroyIcon(item.icon);
    }
    g_pinned.clear();
}

std::wstring GetPinnedFolderPath() {
    wchar_t appData[MAX_PATH];
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appData)))
        return L"";

    std::wstring path = appData;
    path += L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
    return path;
}

void LoadPinnedItems(HWND parent, HINSTANCE hInst) {
    DestroyPinnedButtons();

    std::wstring folder = GetPinnedFolderPath();
    if (folder.empty())
        return;

    std::wstring search = folder + L"\\*.lnk";

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    int nextId = ID_PINNED_BASE;

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        std::wstring lnkPath = folder + L"\\" + fd.cFileName;

        IShellLinkW* pLink = nullptr;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(&pLink))))
            continue;

        IPersistFile* pPersist = nullptr;
        if (FAILED(pLink->QueryInterface(IID_PPV_ARGS(&pPersist)))) {
            pLink->Release();
            continue;
        }

        if (FAILED(pPersist->Load(lnkPath.c_str(), STGM_READ))) {
            pPersist->Release();
            pLink->Release();
            continue;
        }

        WCHAR exePath[MAX_PATH] = {};
        pLink->GetPath(exePath, MAX_PATH, nullptr, SLGP_UNCPRIORITY);

        WCHAR iconPath[MAX_PATH] = {};
        int iconIndex = 0;
        pLink->GetIconLocation(iconPath, MAX_PATH, &iconIndex);

        SHFILEINFOW sfi = {};
        HICON hIcon = nullptr;

        if (wcslen(iconPath) > 0) {
            SHGetFileInfoW(iconPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON);
            hIcon = sfi.hIcon;
        }

        if (!hIcon && wcslen(exePath) > 0) {
            SHGetFileInfoW(exePath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON);
            hIcon = sfi.hIcon;
        }

        std::wstring displayName = fd.cFileName;
        size_t dot = displayName.rfind(L'.');
        if (dot != std::wstring::npos)
            displayName = displayName.substr(0, dot);

        HWND hBtn = CreateWindowExW(
            0,
            L"BUTTON",
            displayName.c_str(),
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            0, 0, 32, 32,
            parent,
            (HMENU)(INT_PTR)nextId,
            hInst,
            nullptr
        );

        PinnedItem item;
        item.exePath = exePath;
        item.displayName = displayName;
        item.icon = hIcon;
        item.hButton = hBtn;
        item.id = nextId;

        g_pinned.push_back(item);

        pPersist->Release();
        pLink->Release();

        nextId++;

    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
}

bool IsProcessRunningForExe(const std::wstring& exePath) {
    if (exePath.empty())
        return false;

    DWORD pids[1024];
    DWORD needed = 0;
    if (!EnumProcesses(pids, sizeof(pids), &needed))
        return false;

    int count = needed / sizeof(DWORD);

    for (int i = 0; i < count; i++) {
        DWORD pid = pids[i];
        if (pid == 0) continue;

        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h) continue;

        wchar_t path[MAX_PATH];
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageNameW(h, 0, path, &size)) {
            if (_wcsicmp(path, exePath.c_str()) == 0) {
                CloseHandle(h);
                return true;
            }
        }
        CloseHandle(h);
    }
    return false;
}

void AddRunningApps(HWND parent, HINSTANCE hInst) {
    DWORD pids[1024];
    DWORD needed = 0;
    if (!EnumProcesses(pids, sizeof(pids), &needed))
        return;

    int count = needed / sizeof(DWORD);

    for (int i = 0; i < count; i++) {
        DWORD pid = pids[i];
        if (pid == 0) continue;

        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h) continue;

        wchar_t path[MAX_PATH];
        DWORD size = MAX_PATH;
        if (!QueryFullProcessImageNameW(h, 0, path, &size)) {
            CloseHandle(h);
            continue;
        }
        CloseHandle(h);

        bool alreadyPinned = false;
        for (auto& p : g_pinned) {
            if (_wcsicmp(p.exePath.c_str(), path) == 0) {
                alreadyPinned = true;
                break;
            }
        }
        if (alreadyPinned) continue;

        SHFILEINFOW sfi = {};
        HICON hIcon = nullptr;
        SHGetFileInfoW(path, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON);
        hIcon = sfi.hIcon;

        int id = ID_PINNED_BASE + (int)g_pinned.size() + 1000;

        HWND hBtn = CreateWindowExW(
            0,
            L"BUTTON",
            L"",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            0, 0, 32, 32,
            parent,
            (HMENU)(INT_PTR)id,
            hInst,
            nullptr
        );

        PinnedItem item;
        item.exePath = path;
        item.displayName = path;
        item.icon = hIcon;
        item.hButton = hBtn;
        item.id = id;

        g_pinned.push_back(item);
    }
}

void LayoutDock(HWND hWnd) {
    if (!hWnd) return;

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi))
        return;

    int screenWidth  = mi.rcWork.right  - mi.rcWork.left;
    int screenHeight = mi.rcWork.bottom - mi.rcWork.top;

    int dockW = g_settings.width;
    int dockH = g_settings.height;

    int x = mi.rcWork.left + (screenWidth - dockW) / 2;
    int y = mi.rcWork.bottom - g_settings.bottomMargin - dockH;

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        x,
        y,
        dockW,
        dockH,
        SWP_SHOWWINDOW | SWP_NOACTIVATE
    );

    ApplyDockRegion(hWnd);

    int padding = 10;
    int btnH = dockH - padding * 2;
    int btnW = btnH;

    int curX = padding;

    if (g_btnStart) {
        SetWindowPos(
            g_btnStart,
            nullptr,
            curX,
            (dockH - btnH) / 2,
            btnW,
            btnH,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
        curX += btnW + padding;
    }

    if (g_btnQuick) {
        SetWindowPos(
            g_btnQuick,
            nullptr,
            curX,
            (dockH - btnH) / 2,
            btnW,
            btnH,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
        curX += btnW + padding * 2;
    }

    for (auto& item : g_pinned) {
        if (!item.hButton) continue;
        SetWindowPos(
            item.hButton,
            nullptr,
            curX,
            (dockH - btnH) / 2,
            btnW,
            btnH,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
        curX += btnW + padding;
    }
}

void SimulateWinKey() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_LWIN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

void SimulateWinA() {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'A';
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'A';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_LWIN;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
}

LRESULT CALLBACK DockWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        ReleaseCapture();
        SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH brush = CreateSolidBrush(RGB(30, 40, 80));
        FillRect(hdc, &ps.rcPaint, brush);
        DeleteObject(brush);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_SIZE:
        ApplyDockRegion(hWnd);
        return 0;

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        int id = (int)dis->CtlID;

        HBRUSH bg = CreateSolidBrush(RGB(40, 50, 100));
        FillRect(dis->hDC, &dis->rcItem, bg);
        DeleteObject(bg);

        PinnedItem* pItem = nullptr;
        for (auto& item : g_pinned) {
            if (item.id == id) {
                pItem = &item;
                break;
            }
        }

        if (pItem) {
            bool running = IsProcessRunningForExe(pItem->exePath);

            if (pItem->icon) {
                int w = dis->rcItem.right - dis->rcItem.left;
                int h = dis->rcItem.bottom - dis->rcItem.top;
                int iconSize = std::min(w, h) - 8;
                int x = dis->rcItem.left + (w - iconSize) / 2;
                int y = dis->rcItem.top + (h - iconSize) / 2;
                DrawIconEx(dis->hDC, x, y, pItem->icon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
            } else {
                SetBkMode(dis->hDC, TRANSPARENT);
                SetTextColor(dis->hDC, RGB(230, 230, 240));
                DrawTextW(dis->hDC, pItem->displayName.c_str(), -1,
                          (LPRECT)&dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }

            if (running) {
                RECT underline = dis->rcItem;
                underline.top = underline.bottom - 4;
                HBRUSH ul = CreateSolidBrush(RGB(0, 200, 255));
                FillRect(dis->hDC, &underline, ul);
                DeleteObject(ul);
            }

            return TRUE;
        }

        return TRUE;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_BTN_START) {
            SimulateWinKey();
            return 0;
        } else if (id == ID_BTN_QUICK) {
            SimulateWinA();
            return 0;
        } else if (id >= ID_PINNED_BASE) {
            for (auto& item : g_pinned) {
                if (item.id == id && !item.exePath.empty()) {
                    ShellExecuteW(nullptr, L"open", item.exePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                    break;
                }
            }
            return 0;
        }
        break;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDockWindow() {
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc   = DockWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = DOCK_CLASS_NAME;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"Failed to register dock window class");
        return false;
    }

    g_dockWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        DOCK_CLASS_NAME,
        L"",
        WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        g_settings.width,
        g_settings.height,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    if (!g_dockWnd) {
        Wh_Log(L"Failed to create dock window");
        return false;
    }

    SetLayeredWindowAttributes(g_dockWnd, 0, (BYTE)g_settings.opacity, LWA_ALPHA);

    g_btnStart = CreateWindowExW(
        0,
        L"BUTTON",
        L"⊞",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 32, 32,
        g_dockWnd,
        (HMENU)(INT_PTR)ID_BTN_START,
        hInst,
        nullptr
    );

    g_btnQuick = CreateWindowExW(
        0,
        L"BUTTON",
        L"⚙",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 32, 32,
        g_dockWnd,
        (HMENU)(INT_PTR)ID_BTN_QUICK,
        hInst,
        nullptr
    );

    LoadPinnedItems(g_dockWnd, hInst);
    AddRunningApps(g_dockWnd, hInst);
    LayoutDock(g_dockWnd);
    ShowWindow(g_dockWnd, SW_SHOWNOACTIVATE);

    return true;
}

void DestroyDockWindow() {
    DestroyPinnedButtons();
    if (g_dockWnd) {
        DestroyWindow(g_dockWnd);
        g_dockWnd = nullptr;
        g_btnStart = nullptr;
        g_btnQuick = nullptr;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Custom Dock Bar (Pinned + Running) init");
    MessageBoxW(nullptr, L"Custom Dock Bar (Pinned + Running) loaded.", L"Dock", MB_OK | MB_TOPMOST);

    CoInitialize(nullptr);
    LoadSettings();
    CreateDockWindow();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Custom Dock Bar (Pinned + Running) uninit");
    DestroyDockWindow();
    CoUninitialize();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Custom Dock Bar (Pinned + Running) settings changed");
    LoadSettings();
    if (g_dockWnd) {
        SetLayeredWindowAttributes(g_dockWnd, 0, (BYTE)g_settings.opacity, LWA_ALPHA);
        LayoutDock(g_dockWnd);
    }
}
