// ==WindhawkMod==
// @id             bar-fork
// @name           XP Top Ultimate Customizable Dock Bar - Fork
// @version        0.1
// @author         serg82rus14
// @github         https://github.com/serg8269-cloud
// @description    Удобная верхняя панель задач для Windows в стиле классических док-панелей с гибкой настройкой.
// @include        explorer.exe
// @compilerOptions -luser32 -lshell32 -lgdi32 -lshlwapi -lcomdlg32
// @license        GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# XP Top Ultimate Customizable Dock Bar
Удобная верхняя панель задач для Windows в стиле классических док-панелей с гибкой настройкой.
Этот мод форкает оригинальный проект и добавляет возможности кастомизации для панели задач, размещая её сверху рабочего стола.
## Использование
После установки мода через Windhawk, панель появится в верхней части экрана. Вы можете настроить её поведение через стандартные настройки Windhawk, если таковые предусмотрены.
## Особенности
- Стиль классической док-панели.
- Размещение поверх всех окон.
- Гибкая настройка элементов (в будущих версиях).
*/
// ==/WindhawkModReadme==

#define _WIN32_WINNT 0x0A00 // Для поддержки современных функций Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <commdlg.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// Позиция текста
enum TextPosition {
    TEXT_POS_BOTTOM = 0,
    TEXT_POS_TOP,
    TEXT_POS_LEFT,
    TEXT_POS_RIGHT,
    TEXT_POS_CENTER,
    TEXT_POS_NONE
};

// Визуальные эффекты наведения
enum HoverEffect {
    EFFECT_CLASSIC = 0,
    EFFECT_GLOW,
    EFFECT_LINE,
    EFFECT_BLUR,
    EFFECT_NONE
};

// Единая структура элементов
struct DockItem {
    std::wstring label;
    std::wstring path;
    std::wstring iconPath;
    bool isDrive = false;
    bool isCustom = false;
    HICON hIcon = NULL;
    int x = 0;
};

// Глобальные переменные
static std::vector<DockItem> g_items;
static HWND g_hBarWnd = NULL;
static HANDLE g_hThread = NULL;

static int g_barHeight = 70;
static BYTE g_alpha = 230;
static COLORREF g_bgColor = RGB(35, 35, 35);
static COLORREF g_textColor = RGB(240, 240, 240);
static COLORREF g_hoverColor = RGB(70, 70, 70);
static int g_iconSize = 32;
static int g_itemWidth = 90;
static int g_fontSize = 12;
static TextPosition g_textPos = TEXT_POS_BOTTOM;
static HoverEffect g_hoverEffect = EFFECT_GLOW;

static bool g_autoHide = true;
static bool g_isPinnedIcons = false;
static bool g_isVisible = false;

static int g_draggedIndex = -1;
static int g_hoveredIndex = -1;
static int g_dragOffsetX = 0;
static bool g_isDraggingItem = false;

// Проверка: запущен ли в данный момент полноэкранный режим (игра / приложение)
static bool IsFullscreenAppRunning() {
    HWND hForeground = GetForegroundWindow();
    if (!hForeground || hForeground == GetDesktopWindow() || hForeground == GetShellWindow()) {
        return false;
    }

    DWORD style = GetWindowLong(hForeground, GWL_STYLE);
    if (style & WS_CHILD) {
        return false;
    }

    RECT rcApp;
    GetWindowRect(hForeground, &rcApp);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Если размеры окна приложения совпадают с размерами экрана — оно полноэкранное
    if (rcApp.left <= 0 && rcApp.top <= 0 && rcApp.right >= screenWidth && rcApp.bottom >= screenHeight) {
        // Дополнительно исключаем проводник и некоторые системные панели
        wchar_t className[256];
        if (GetClassNameW(hForeground, className, 256) > 0) {
            if (_wcsicmp(className, L"Progman") == 0 || _wcsicmp(className, L"WorkerW") == 0) {
                return false;
            }
        }
        return true;
    }

    return false;
}

// Вспомогательные функции
static std::wstring GetConfigPath() {
    wchar_t appData[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appData);
    std::wstring dir = std::wstring(appData) + L"\\WindhawkTopBar";
    CreateDirectoryW(dir.c_str(), NULL);
    return dir + L"\\config.ini";
}

static HICON GetFileIcon(const std::wstring& path, int size) {
    SHFILEINFOW sfi = {0};
    UINT flags = SHGFI_ICON | (size <= 16 ? SHGFI_SMALLICON : SHGFI_LARGEICON);
    SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), flags);
    return sfi.hIcon;
}

static std::wstring GetDisplayName(const std::wstring& path) {
    SHFILEINFOW sfi = {0};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME)) {
        if (wcslen(sfi.szDisplayName) > 0) return sfi.szDisplayName;
    }
    wchar_t name[MAX_PATH] = {0};
    wcsncpy(name, path.c_str(), MAX_PATH - 1);
    PathRemoveExtensionW(name);
    return PathFindFileNameW(name);
}

static void ReloadIcons() {
    for (auto& item : g_items) {
        if (item.hIcon) DestroyIcon(item.hIcon);
        std::wstring target = item.iconPath.empty() ? item.path : item.iconPath;
        item.hIcon = GetFileIcon(target, g_iconSize);
    }
}

static void RearrangeItems() {
    int currentX = 15;
    for (auto& item : g_items) {
        item.x = currentX;
        currentX += g_itemWidth + 10;
    }
}

static void SaveConfig() {
    std::wstring ini = GetConfigPath();
    WritePrivateProfileStringW(L"Settings", L"Height", std::to_wstring(g_barHeight).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"Alpha", std::to_wstring(g_alpha).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"BgColor", std::to_wstring(g_bgColor).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"TextColor", std::to_wstring(g_textColor).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"HoverColor", std::to_wstring(g_hoverColor).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"IconSize", std::to_wstring(g_iconSize).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"ItemWidth", std::to_wstring(g_itemWidth).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"FontSize", std::to_wstring(g_fontSize).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"TextPos", std::to_wstring((int)g_textPos).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"HoverEffect", std::to_wstring((int)g_hoverEffect).c_str(), ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"AutoHide", g_autoHide ? L"1" : L"0", ini.c_str());
    WritePrivateProfileStringW(L"Settings", L"PinnedIcons", g_isPinnedIcons ? L"1" : L"0", ini.c_str());

    WritePrivateProfileStringW(L"Items", L"Count", std::to_wstring(g_items.size()).c_str(), ini.c_str());

    for (size_t i = 0; i < g_items.size(); i++) {
        std::wstring sec = L"Item_" + std::to_wstring(i);
        WritePrivateProfileStringW(sec.c_str(), L"Path", g_items[i].path.c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"Label", g_items[i].label.c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"IconPath", g_items[i].iconPath.c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"IsDrive", g_items[i].isDrive ? L"1" : L"0", ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"X", std::to_wstring(g_items[i].x).c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"IsCustom", g_items[i].isCustom ? L"1" : L"0", ini.c_str());
    }
}

static void LoadDefaultDrives() {
    for (auto& item : g_items) if (item.hIcon) DestroyIcon(item.hIcon);
    g_items.clear();

    wchar_t driveBuffer[512] = {0};
    GetLogicalDriveStringsW(512, driveBuffer);

    wchar_t* drive = driveBuffer;
    while (*drive) {
        wchar_t volumeName[MAX_PATH] = L"";
        GetVolumeInformationW(drive, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0);

        DockItem item;
        item.path = drive;
        item.label = wcslen(volumeName) > 0 ? std::wstring(drive, 2) + L" " + volumeName : drive;
        item.iconPath = L"";
        item.isDrive = true;
        item.hIcon = GetFileIcon(drive, g_iconSize);
        item.isCustom = false;
        g_items.push_back(item);

        drive += wcslen(drive) + 1;
    }
    RearrangeItems();
}

static void LoadConfig() {
    std::wstring ini = GetConfigPath();
    g_barHeight = GetPrivateProfileIntW(L"Settings", L"Height", 70, ini.c_str());
    g_alpha = (BYTE)GetPrivateProfileIntW(L"Settings", L"Alpha", 230, ini.c_str());
    g_bgColor = GetPrivateProfileIntW(L"Settings", L"BgColor", RGB(35, 35, 35), ini.c_str());
    g_textColor = GetPrivateProfileIntW(L"Settings", L"TextColor", RGB(240, 240, 240), ini.c_str());
    g_hoverColor = GetPrivateProfileIntW(L"Settings", L"HoverColor", RGB(70, 70, 70), ini.c_str());
    g_iconSize = GetPrivateProfileIntW(L"Settings", L"IconSize", 32, ini.c_str());
    g_itemWidth = GetPrivateProfileIntW(L"Settings", L"ItemWidth", 90, ini.c_str());
    g_fontSize = GetPrivateProfileIntW(L"Settings", L"FontSize", 12, ini.c_str());
    g_textPos = (TextPosition)GetPrivateProfileIntW(L"Settings", L"TextPos", 0, ini.c_str());
    g_hoverEffect = (HoverEffect)GetPrivateProfileIntW(L"Settings", L"HoverEffect", 1, ini.c_str());
    g_autoHide = GetPrivateProfileIntW(L"Settings", L"AutoHide", 1, ini.c_str()) == 1;
    g_isPinnedIcons = GetPrivateProfileIntW(L"Settings", L"PinnedIcons", 0, ini.c_str()) == 1;

    int count = GetPrivateProfileIntW(L"Items", L"Count", -1, ini.c_str());
    if (count <= 0) {
        LoadDefaultDrives();
        SaveConfig();
        return;
    }

    g_items.clear();
    for (int i = 0; i < count; i++) {
        std::wstring sec = L"Item_" + std::to_wstring(i);
        wchar_t path[MAX_PATH] = {0}, label[MAX_PATH] = {0}, iconPath[MAX_PATH] = {0};

        GetPrivateProfileStringW(sec.c_str(), L"Path", L"", path, MAX_PATH, ini.c_str());
        GetPrivateProfileStringW(sec.c_str(), L"Label", L"", label, MAX_PATH, ini.c_str());
        GetPrivateProfileStringW(sec.c_str(), L"IconPath", L"", iconPath, MAX_PATH, ini.c_str());
        int x = GetPrivateProfileIntW(sec.c_str(), L"X", 0, ini.c_str());
        bool isDrive = GetPrivateProfileIntW(sec.c_str(), L"IsDrive", 0, ini.c_str()) == 1;
        bool isCustom = GetPrivateProfileIntW(sec.c_str(), L"IsCustom", 0, ini.c_str()) == 1;

        if (wcslen(path) > 0) {
            DockItem item;
            item.path = path;
            item.label = label;
            item.iconPath = iconPath;
            item.isDrive = isDrive;
            item.x = x;
            item.isCustom = isCustom;
            std::wstring targetIcon = item.iconPath.empty() ? item.path : item.iconPath;
            item.hIcon = GetFileIcon(targetIcon, g_iconSize);
            g_items.push_back(item);
        }
    }
}

struct InputContext {
    HWND hEdit;
    std::wstring result;
    std::wstring prompt;
    bool ok;
};

static LRESULT CALLBACK InputProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    InputContext* ctx = (InputContext*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        ctx = (InputContext*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ctx);

        CreateWindowExW(0, L"STATIC", ctx->prompt.c_str(), WS_CHILD | WS_VISIBLE, 15, 10, 260, 20, hwnd, NULL, NULL, NULL);
        ctx->hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", ctx->result.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 15, 35, 260, 25, hwnd, NULL, NULL, NULL);
        CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 80, 70, 75, 25, hwnd, (HMENU)IDOK, NULL, NULL);
        CreateWindowExW(0, L"BUTTON", L"Отмена", WS_CHILD | WS_VISIBLE, 165, 70, 75, 25, hwnd, (HMENU)IDCANCEL, NULL, NULL);
        SetFocus(ctx->hEdit);
        SendMessage(ctx->hEdit, EM_SETSEL, 0, -1);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buf[256] = {0};
            GetWindowTextW(ctx->hEdit, buf, 256);
            ctx->result = buf;
            ctx->ok = true;
            DestroyWindow(hwnd);
        } else if (LOWORD(wParam) == IDCANCEL) {
            ctx->ok = false;
            DestroyWindow(hwnd);
        }
        break;
    case WM_CLOSE:
        ctx->ok = false;
        DestroyWindow(hwnd);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

static bool ShowInputDialog(HWND hParent, const wchar_t* title, const wchar_t* prompt, std::wstring& val) {
    InputContext ctx;
    ctx.result = val;
    ctx.prompt = prompt;
    ctx.ok = false;

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = InputProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkInputDlg";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hDlg = CreateWindowExW(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, L"WindhawkInputDlg", title,
                                WS_POPUP | WS_CAPTION | WS_SYSMENU,
                                (GetSystemMetrics(SM_CXSCREEN)-310)/2, (GetSystemMetrics(SM_CYSCREEN)-145)/2,
                                310, 145, hParent, NULL, wc.hInstance, &ctx);

    EnableWindow(hParent, FALSE);
    ShowWindow(hDlg, SW_SHOW);

    MSG msg;
    while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    EnableWindow(hParent, TRUE);
    UnregisterClassW(L"WindhawkInputDlg", wc.hInstance);
    if (ctx.ok) {
        val = ctx.result;
        return true;
    }
    return false;
}

static COLORREF ChooseCustomColor(HWND hwnd, COLORREF initialColor) {
    CHOOSECOLORW cc = {0};
    static COLORREF acrCustClr[16] = {0};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;
    cc.lpCustColors = acrCustClr;
    cc.rgbResult = initialColor;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColorW(&cc)) {
        return cc.rgbResult;
    }
    return initialColor;
}

static void AddFileViaDialog(HWND hwnd) {
    wchar_t file[MAX_PATH] = {0};
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Все файлы (*.*)\0*.*\0Ярлыки и программы (*.exe;*.lnk)\0*.exe;*.lnk\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        DockItem item;
        item.path = file;
        item.label = GetDisplayName(file);
        item.iconPath = L"";
        item.isDrive = false;
        item.hIcon = GetFileIcon(file, g_iconSize);
        item.isCustom = true;
        item.x = g_items.empty() ? 15 : g_items.back().x + g_itemWidth + 10;
        g_items.push_back(item);
        SaveConfig();
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

static void OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rcClient;
    GetClientRect(hwnd, &rcClient);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
    HGDIOBJ hbmOld = SelectObject(hdcMem, hbmMem);

    HBRUSH hBgBrush = CreateSolidBrush(g_bgColor);
    FillRect(hdcMem, &rcClient, hBgBrush);
    DeleteObject(hBgBrush);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    HGDIOBJ hOldPen = SelectObject(hdcMem, hPen);
    MoveToEx(hdcMem, 0, rcClient.bottom - 1, NULL);
    LineTo(hdcMem, rcClient.right, rcClient.bottom - 1);
    SelectObject(hdcMem, hOldPen);
    DeleteObject(hPen);

    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, g_textColor);

    HFONT hFont = CreateFontW(g_fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              VARIABLE_PITCH, L"Segoe UI");
    HGDIOBJ hOldFont = SelectObject(hdcMem, hFont);

    for (size_t i = 0; i < g_items.size(); i++) {
        const auto& item = g_items[i];
        RECT itemRc = { item.x, 3, item.x + g_itemWidth, g_barHeight - 3 };

        if ((int)i == g_hoveredIndex) {
            if (g_hoverEffect == EFFECT_CLASSIC) {
                HBRUSH hHover = CreateSolidBrush(g_hoverColor);
                FillRect(hdcMem, &itemRc, hHover);
                DeleteObject(hHover);
                DrawEdge(hdcMem, &itemRc, BDR_RAISEDINNER, BF_RECT);
            } else if (g_hoverEffect == EFFECT_GLOW) {
                HBRUSH hHover = CreateSolidBrush(g_hoverColor);
                FillRect(hdcMem, &itemRc, hHover);
                DeleteObject(hHover);
            } else if (g_hoverEffect == EFFECT_LINE) {
                HPEN hLinePen = CreatePen(PS_SOLID, 2, g_textColor);
                HGDIOBJ hOldLine = SelectObject(hdcMem, hLinePen);
                MoveToEx(hdcMem, item.x + 5, g_barHeight - 3, NULL);
                LineTo(hdcMem, item.x + g_itemWidth - 5, g_barHeight - 3);
                SelectObject(hdcMem, hOldLine);
                DeleteObject(hLinePen);
            } else if (g_hoverEffect == EFFECT_BLUR) {
                HBRUSH hHover = CreateSolidBrush(RGB((GetRValue(g_hoverColor) + 255) / 2,
                                                    (GetGValue(g_hoverColor) + 255) / 2,
                                                    (GetBValue(g_hoverColor) + 255) / 2));
                FillRect(hdcMem, &itemRc, hHover);
                DeleteObject(hHover);
            }
        }

        int iconX = item.x + (g_itemWidth - g_iconSize) / 2;
        int iconY = (g_barHeight - g_iconSize) / 2;

        RECT textRect = itemRc;

        if (g_textPos == TEXT_POS_BOTTOM) {
            iconY = 6;
            textRect.top = iconY + g_iconSize + 2;
        } else if (g_textPos == TEXT_POS_TOP) {
            iconY = g_barHeight - g_iconSize - 6;
            textRect.bottom = iconY - 2;
        } else if (g_textPos == TEXT_POS_LEFT) {
            iconX = item.x + g_itemWidth - g_iconSize - 8;
            textRect.right = iconX - 4;
        } else if (g_textPos == TEXT_POS_RIGHT) {
            iconX = item.x + 8;
            textRect.left = iconX + g_iconSize + 4;
        }

        if (item.hIcon) {
            DrawIconEx(hdcMem, iconX, iconY, item.hIcon, g_iconSize, g_iconSize, 0, NULL, DI_NORMAL);
        }

        if (g_textPos != TEXT_POS_NONE) {
            UINT format = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE;
            if (g_textPos == TEXT_POS_BOTTOM || g_textPos == TEXT_POS_TOP) {
                format = DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS;
            }
            DrawTextW(hdcMem, item.label.c_str(), -1, &textRect, format);
        }
    }

    SelectObject(hdcMem, hOldFont);
    DeleteObject(hFont);

    BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);

    EndPaint(hwnd, &ps);
}

static void ShowContextMenu(HWND hwnd, int x, int y, int clickedIndex) {
    HMENU hMenu = CreatePopupMenu();
    HMENU hColorMenu = CreatePopupMenu();
    HMENU hSizeMenu = CreatePopupMenu();
    HMENU hTextPosMenu = CreatePopupMenu();
    HMENU hAlphaMenu = CreatePopupMenu();
    HMENU hEffectMenu = CreatePopupMenu();
    HMENU hPinMenu = CreatePopupMenu();

    AppendMenuW(hColorMenu, MF_STRING, 401, L"🎨 Цвет фона панели...");
    AppendMenuW(hColorMenu, MF_STRING, 402, L"✏️ Цвет подписи ярлыков...");
    AppendMenuW(hColorMenu, MF_STRING, 403, L"✨ Цвет подсвечивания (Hover)...");

    AppendMenuW(hAlphaMenu, MF_STRING, 301, L"100% (Непрозрачно)");
    AppendMenuW(hAlphaMenu, MF_STRING, 302, L"85%");
    AppendMenuW(hAlphaMenu, MF_STRING, 303, L"70%");
    AppendMenuW(hAlphaMenu, MF_STRING, 304, L"50%");
    AppendMenuW(hAlphaMenu, MF_STRING, 305, L"25%");
    AppendMenuW(hAlphaMenu, MF_STRING, 306, L"🔢 Задать точно (0 - 255)...");

    AppendMenuW(hTextPosMenu, MF_STRING, 601, L"⬇️ Снизу");
    AppendMenuW(hTextPosMenu, MF_STRING, 602, L"⬆️ Сверху");
    AppendMenuW(hTextPosMenu, MF_STRING, 603, L"⬅️ Слева");
    AppendMenuW(hTextPosMenu, MF_STRING, 604, L"➡️ Справа");
    AppendMenuW(hTextPosMenu, MF_STRING, 605, L"🎯 По центру");
    AppendMenuW(hTextPosMenu, MF_STRING, 606, L"🚫 Скрыть подписи");

    AppendMenuW(hSizeMenu, MF_STRING, 501, L"Мелкие иконки (16x16)");
    AppendMenuW(hSizeMenu, MF_STRING, 502, L"Средние иконки (32x32)");
    AppendMenuW(hSizeMenu, MF_STRING, 503, L"Крупные иконки (48x48)");
    AppendMenuW(hSizeMenu, MF_STRING, 504, L"Огромные иконки (64x64)");
    AppendMenuW(hSizeMenu, MF_STRING, 505, L"📐 Свой размер иконок...");
    AppendMenuW(hSizeMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hSizeMenu, MF_STRING, 508, L"🔤 Размер текста ярлыков...");
    AppendMenuW(hSizeMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hSizeMenu, MF_STRING, 506, L"📏 Высота панели...");
    AppendMenuW(hSizeMenu, MF_STRING, 507, L"↔️ Ширина ячейки ярлыка...");

    AppendMenuW(hEffectMenu, MF_STRING, 701, L"Классическая рамка");
    AppendMenuW(hEffectMenu, MF_STRING, 702, L"Мягкое подсвечивание");
    AppendMenuW(hEffectMenu, MF_STRING, 703, L"Нижняя линия");
    AppendMenuW(hEffectMenu, MF_STRING, 704, L"Эффект блюра (Размытие)");
    AppendMenuW(hEffectMenu, MF_STRING, 705, L"Без эффекта");

    AppendMenuW(hPinMenu, MF_STRING | (g_autoHide ? MF_CHECKED : 0), 801, L"Автоскрытие (показывать при наведении)");
    AppendMenuW(hPinMenu, MF_STRING | (!g_autoHide ? MF_CHECKED : 0), 802, L"Закрепить панель вверху (Всегда видна)");
    AppendMenuW(hPinMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hPinMenu, MF_STRING | (g_isPinnedIcons ? MF_CHECKED : 0), 803, L"🔒 Зафиксировать ярлыки (Запретить перетаскивание)");

    if (clickedIndex >= 0) {
        AppendMenuW(hMenu, MF_STRING, 104, (L"✏️ Переименовать: " + g_items[clickedIndex].label).c_str());
        AppendMenuW(hMenu, MF_STRING, 101, L"❌ Удалить этот ярлык");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    }

    AppendMenuW(hMenu, MF_STRING, 105, L"➕ Добавить файл / программу...");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hColorMenu, L"🎨 Цвета и оформление");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hAlphaMenu, L"💧 Прозрачность");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hTextPosMenu, L"🔤 Позиция подписей");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSizeMenu, L"📐 Размеры и высота");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hEffectMenu, L"✨ Визуальные эффекты");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hPinMenu, L"📌 Закрепление и режим");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, 102, L"🔄 Авторасстановка ярлыков");
    AppendMenuW(hMenu, MF_STRING, 103, L"⚠️ Сбросить к дискам по умолчанию");

    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN, x, y, hwnd, NULL);
    DestroyMenu(hMenu);

    if (cmd == 104 && clickedIndex >= 0) {
        if (ShowInputDialog(hwnd, L"Переименование", L"Введите новое название:", g_items[clickedIndex].label)) {
            SaveConfig();
        }
    } else if (cmd == 101 && clickedIndex >= 0) {
        if (g_items[clickedIndex].hIcon) DestroyIcon(g_items[clickedIndex].hIcon);
        g_items.erase(g_items.begin() + clickedIndex);
        RearrangeItems();
        SaveConfig();
    } else if (cmd == 105) {
        AddFileViaDialog(hwnd);
    } else if (cmd == 102) {
        RearrangeItems();
        SaveConfig();
    } else if (cmd == 103) {
        LoadDefaultDrives();
        SaveConfig();
    } else if (cmd == 401) {
        g_bgColor = ChooseCustomColor(hwnd, g_bgColor);
        SaveConfig();
    } else if (cmd == 402) {
        g_textColor = ChooseCustomColor(hwnd, g_textColor);
        SaveConfig();
    } else if (cmd == 403) {
        g_hoverColor = ChooseCustomColor(hwnd, g_hoverColor);
        SaveConfig();
    } else if (cmd >= 301 && cmd <= 305) {
        BYTE alphas[] = { 255, 216, 178, 128, 64 };
        g_alpha = alphas[cmd - 301];
        SetLayeredWindowAttributes(hwnd, 0, g_alpha, LWA_ALPHA);
        SaveConfig();
    } else if (cmd == 306) {
        std::wstring strVal = std::to_wstring((int)g_alpha);
        if (ShowInputDialog(hwnd, L"Прозрачность", L"Введите прозрачность (0 - 255):", strVal)) {
            int val = _wtoi(strVal.c_str());
            if (val >= 0 && val <= 255) {
                g_alpha = (BYTE)val;
                SetLayeredWindowAttributes(hwnd, 0, g_alpha, LWA_ALPHA);
                SaveConfig();
            }
        }
    } else if (cmd >= 601 && cmd <= 606) {
        g_textPos = (TextPosition)(cmd - 601);
        SaveConfig();
    } else if (cmd >= 501 && cmd <= 504) {
        int sizes[] = { 16, 32, 48, 64 };
        g_iconSize = sizes[cmd - 501];
        ReloadIcons();
        SaveConfig();
    } else if (cmd == 505) {
        std::wstring strVal = std::to_wstring(g_iconSize);
        if (ShowInputDialog(hwnd, L"Размер значков", L"Введите размер иконки в пикселях:", strVal)) {
            int val = _wtoi(strVal.c_str());
            if (val >= 8 && val <= 128) {
                g_iconSize = val;
                ReloadIcons();
                SaveConfig();
            }
        }
    } else if (cmd == 508) {
        std::wstring strVal = std::to_wstring(g_fontSize);
        if (ShowInputDialog(hwnd, L"Размер текста", L"Введите размер шрифта (8 - 32):", strVal)) {
            int val = _wtoi(strVal.c_str());
            if (val >= 8 && val <= 32) {
                g_fontSize = val;
                SaveConfig();
            }
        }
    } else if (cmd == 506) {
        std::wstring strVal = std::to_wstring(g_barHeight);
        if (ShowInputDialog(hwnd, L"Высота панели", L"Введите высоту панели (30 - 200 px):", strVal)) {
            int val = _wtoi(strVal.c_str());
            if (val >= 30 && val <= 200) {
                g_barHeight = val;
                SaveConfig();
            }
        }
    } else if (cmd == 507) {
        std::wstring strVal = std::to_wstring(g_itemWidth);
        if (ShowInputDialog(hwnd, L"Ширина ячейки", L"Введите ширину ячейки ярлыка:", strVal)) {
            int val = _wtoi(strVal.c_str());
            if (val >= 40 && val <= 300) {
                g_itemWidth = val;
                RearrangeItems();
                SaveConfig();
            }
        }
    } else if (cmd >= 701 && cmd <= 705) {
        HoverEffect effects[] = { EFFECT_CLASSIC, EFFECT_GLOW, EFFECT_LINE, EFFECT_BLUR, EFFECT_NONE };
        g_hoverEffect = effects[cmd - 701];
        SaveConfig();
    } else if (cmd == 801) {
        g_autoHide = true;
        SaveConfig();
    } else if (cmd == 802) {
        g_autoHide = false;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), g_barHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);
        g_isVisible = true;
        SaveConfig();
    } else if (cmd == 803) {
        g_isPinnedIcons = !g_isPinnedIcons;
        SaveConfig();
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

static LRESULT CALLBACK BarWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        LoadConfig();
        DragAcceptFiles(hwnd, TRUE);
        SetTimer(hwnd, 1, 40, NULL);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd, 0, g_alpha, LWA_ALPHA);
        break;

    case WM_PAINT:
        OnPaint(hwnd);
        break;

    case WM_DROPFILES: {
        HDROP hDrop = (HDROP)wParam;
        UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

        for (UINT i = 0; i < count; i++) {
            wchar_t filePath[MAX_PATH];
            DragQueryFileW(hDrop, i, filePath, MAX_PATH);

            DockItem item;
            item.path = filePath;
            item.label = GetDisplayName(filePath);
            item.iconPath = L"";
            item.isDrive = false;
            item.hIcon = GetFileIcon(filePath, g_iconSize);
            item.isCustom = true;

            g_items.push_back(item);
        }

        DragFinish(hDrop);
        RearrangeItems();
        SaveConfig();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_LBUTTONDOWN: {
        if (g_isPinnedIcons) break;
        int x = LOWORD(lParam);
        for (size_t i = 0; i < g_items.size(); i++) {
            if (x >= g_items[i].x && x <= g_items[i].x + g_itemWidth) {
                g_draggedIndex = (int)i;
                g_dragOffsetX = x - g_items[i].x;
                g_isDraggingItem = false;
                SetCapture(hwnd);
                break;
            }
        }
        break;
    }

    case WM_MOUSEMOVE: {
        int x = LOWORD(lParam);

        if (g_draggedIndex >= 0 && (wParam & MK_LBUTTON)) {
            g_items[g_draggedIndex].x = x - g_dragOffsetX;
            g_isDraggingItem = true;

            int targetIndex = g_draggedIndex;
            for (size_t i = 0; i < g_items.size(); i++) {
                if (i != (size_t)g_draggedIndex) {
                    int center = g_items[i].x + g_itemWidth / 2;
                    if (g_draggedIndex < (int)i && x > center) {
                        targetIndex = (int)i;
                    } else if (g_draggedIndex > (int)i && x < center) {
                        targetIndex = (int)i;
                    }
                }
            }

            if (targetIndex != g_draggedIndex) {
                DockItem draggedItem = g_items[g_draggedIndex];
                g_items.erase(g_items.begin() + g_draggedIndex);
                g_items.insert(g_items.begin() + targetIndex, draggedItem);
                g_draggedIndex = targetIndex;

                int currentX = 15;
                for (size_t i = 0; i < g_items.size(); i++) {
                    if ((int)i != g_draggedIndex) {
                        g_items[i].x = currentX;
                    }
                    currentX += g_itemWidth + 10;
                }
            }

            InvalidateRect(hwnd, NULL, FALSE);
        } else {
            int newHover = -1;
            for (size_t i = 0; i < g_items.size(); i++) {
                if (x >= g_items[i].x && x <= g_items[i].x + g_itemWidth) {
                    newHover = (int)i;
                    break;
                }
            }
            if (newHover != g_hoveredIndex) {
                g_hoveredIndex = newHover;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        break;
    }

    case WM_LBUTTONUP: {
        if (g_draggedIndex >= 0) {
            ReleaseCapture();
            if (!g_isDraggingItem) {
                ShellExecuteW(NULL, L"open", g_items[g_draggedIndex].path.c_str(), NULL, NULL, SW_SHOWNORMAL);
                if (g_autoHide) {
                    ShowWindow(hwnd, SW_HIDE);
                    g_isVisible = false;
                }
            } else {
                RearrangeItems();
                SaveConfig();
            }
            g_draggedIndex = -1;
            g_isDraggingItem = false;
            InvalidateRect(hwnd, NULL, TRUE);
        } else {
            int x = LOWORD(lParam);
            for (size_t i = 0; i < g_items.size(); i++) {
                if (x >= g_items[i].x && x <= g_items[i].x + g_itemWidth) {
                    ShellExecuteW(NULL, L"open", g_items[i].path.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    if (g_autoHide) {
                        ShowWindow(hwnd, SW_HIDE);
                        g_isVisible = false;
                    }
                    break;
                }
            }
        }
        break;
    }

    case WM_RBUTTONUP: {
        POINT pt;
        GetCursorPos(&pt);
        POINT clPt = pt;
        ScreenToClient(hwnd, &clPt);

        int clickedIndex = -1;
        for (size_t i = 0; i < g_items.size(); i++) {
            if (clPt.x >= g_items[i].x && clPt.x <= g_items[i].x + g_itemWidth) {
                clickedIndex = (int)i;
                break;
            }
        }
        ShowContextMenu(hwnd, pt.x, pt.y, clickedIndex);
        break;
    }

    case WM_TIMER: {
        if (!g_autoHide) break;

        // Если запущено полноэкранное приложение (игра) — принудительно прячем панель и блокируем её появление
        if (IsFullscreenAppRunning()) {
            if (g_isVisible) {
                ShowWindow(hwnd, SW_HIDE);
                g_isVisible = false;
                g_hoveredIndex = -1;
            }
            break;
        }

        POINT pt;
        GetCursorPos(&pt);
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);

        if (!g_isVisible) {
            if (pt.y <= 2) {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, screenWidth, g_barHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);
                g_isVisible = true;
            }
        } else {
            if (GetCapture() == hwnd) break;

            if (pt.y > g_barHeight + 15) {
                ShowWindow(hwnd, SW_HIDE);
                g_isVisible = false;
                g_hoveredIndex = -1;
            }
        }
        break;
    }

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        for (auto& item : g_items) {
            if (item.hIcon) DestroyIcon(item.hIcon);
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

static DWORD WINAPI BarThread(LPVOID) {
    UnregisterClassW(L"WindhawkXPUltimateTopBar", GetModuleHandle(NULL));

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = BarWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkXPUltimateTopBar";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    g_hBarWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
        wc.lpszClassName,
        L"XP Top Ultimate Dock Bar",
        WS_POPUP | (g_autoHide ? 0 : WS_VISIBLE),
        0, 0, screenWidth, g_barHeight,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!g_hBarWnd) return 0;

    if (!g_autoHide) {
        ShowWindow(g_hBarWnd, SW_SHOW);
        g_isVisible = true;
    }

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

BOOL Wh_ModInit() {
    g_hThread = CreateThread(NULL, 0, BarThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hBarWnd && IsWindow(g_hBarWnd)) {
        SendMessageW(g_hBarWnd, WM_CLOSE, 0, 0);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 1000);
        CloseHandle(g_hThread);
    }
}
