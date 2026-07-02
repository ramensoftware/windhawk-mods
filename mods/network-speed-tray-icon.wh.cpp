// ==WindhawkMod==
// @id              network-speed-tray-icon
// @name            Network Speed Tray Icon
// @description     Shows current download speed icon in system tray
// @version         1.0
// @author          kar2ner
// @github          https://github.com/kar2ner
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -liphlpapi -lgdi32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Network Speed Tray Icon

Показывает динамическую иконку в системном трее с текущей скоростью
**входящего** трафика (суммарно по всем активным сетевым интерфейсам,
кроме loopback). Иконка и всплывающая подсказка обновляются раз в секунду.

Скорость считается через IP Helper API (GetIfTable2), поэтому мод не
зависит от диспетчера задач и работает даже если тот закрыт.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- updateIntervalMs: 1000
  $name: Интервал обновления (мс)
  $description: Как часто пересчитывать и перерисовывать иконку
*/
// ==/WindhawkModSettings==

// MIB_IF_TABLE2 / GetIfTable2 доступны начиная с Windows Vista.
// Без явного указания версии некоторые тулчейны берут более старый
// _WIN32_WINNT по умолчанию, и тогда эти типы/функции "не находятся" —
// это самая частая причина сразу пачки ошибок компиляции в таком коде.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602  // Windows 8, с запасом
#endif
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include <iphlpapi.h>
#include <cwchar>   // swprintf_s/wcscpy_s
#include <cmath>    // round()

namespace {

constexpr UINT WM_TRAYICON = WM_APP + 1;
constexpr UINT_PTR TIMER_ID = 1;

HWND g_hWnd = nullptr;
NOTIFYICONDATAW g_nid = {};
ULONG64 g_lastBytesIn = 0;
bool g_firstSample = true;
HANDLE g_thread = nullptr;
volatile bool g_running = false;
UINT g_updateIntervalMs = 1000;

// Входящие байты только с "основного" интерфейса — того, через который
// идёт маршрут по умолчанию (т.е. реальный интернет-трафик).
//
// Раньше здесь суммировались ВСЕ интерфейсы, но в Windows часто есть
// виртуальные адаптеры (Hyper-V, WSL, VPN, Bluetooth PAN и т.п.), которые
// задваивают/учитывают тот же трафик в своих счётчиках — из-за этого
// скорость завышалась в несколько раз. GetBestInterface() отдаёт индекс
// именно того интерфейса, который реально используется для выхода в сеть.
ULONG64 GetTotalBytesIn() {
    DWORD ifIndex = 0;

    // 8.8.8.8 как "типичный" внешний адрес назначения — все октеты
    // одинаковы, поэтому сетевой порядок байт значения не имеет.
    if (GetBestInterface(0x08080808, &ifIndex) != NO_ERROR) {
        return 0;
    }

    MIB_IFROW row = {};
    row.dwIndex = ifIndex;
    if (GetIfEntry(&row) != NO_ERROR) {
        return 0;
    }

    return row.dwInOctets;
}

// Рисует иконку 32x32 с текстом скорости в МБ/с, округлённой до 0.5
// (0.5, 1.0, 1.5, 2.0, ...). Размер шрифта чуть уменьшается для длинных
// значений (3+ знака до точки), чтобы текст не обрезался.
HICON CreateSpeedIcon(double mbPerSecRounded) {
    const int size = 32;

    HDC screenDc = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screenDc);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size; // top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP colorBmp = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(dc, colorBmp);

    RECT rect = {0, 0, size, size};
    HBRUSH bg = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(dc, &rect, bg); // фон = чёрный, позже станет прозрачным

    wchar_t buf[16];
    swprintf_s(buf, L"%.1f", mbPerSecRounded);

    // Чем короче строка, тем крупнее можно взять шрифт
    int textLen = (int)wcslen(buf);
    int fontHeight = 20;       // "0.5" .. "9.9"  — 3 символа
    if (textLen == 4) fontHeight = 16;   // "12.5"
    else if (textLen >= 5) fontHeight = 13; // "125.5" и длиннее

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));

    HFONT font = CreateFontW(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(dc, font);

    DrawTextW(dc, buf, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(dc, oldFont);
    DeleteObject(font);

    // Делаем чёрные пиксели прозрачными (простая альфа-маска по тексту)
    DWORD* pixels = static_cast<DWORD*>(bits);
    for (int i = 0; i < size * size; i++) {
        BYTE r = GetRValue(pixels[i]);
        BYTE g = GetGValue(pixels[i]);
        BYTE b = GetBValue(pixels[i]);
        if (r == 0 && g == 0 && b == 0) {
            pixels[i] = 0; // полностью прозрачно
        } else {
            pixels[i] = (255u << 24) | (r << 16) | (g << 8) | b;
        }
    }

    SelectObject(dc, oldBmp);

    HBITMAP maskBmp = CreateBitmap(size, size, 1, 1, nullptr);

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = colorBmp;
    iconInfo.hbmMask = maskBmp;

    HICON icon = CreateIconIndirect(&iconInfo);

    DeleteObject(colorBmp);
    DeleteObject(maskBmp);
    DeleteDC(dc);
    ReleaseDC(nullptr, screenDc);

    return icon;
}

void UpdateTrayIcon() {
    ULONG64 currentBytes = GetTotalBytesIn();

    double kbPerSec = 0.0;
    if (!g_firstSample) {
        ULONG64 delta = (currentBytes >= g_lastBytesIn) ? (currentBytes - g_lastBytesIn) : 0;
        kbPerSec = (delta / 1024.0) / (g_updateIntervalMs / 1000.0);
    }
    g_firstSample = false;
    g_lastBytesIn = currentBytes;

    double mbPerSec = kbPerSec / 1024.0;
    // Округление до ближайших 0.5: 0.5, 1.0, 1.5, 2.0, ...
    double mbPerSecRounded = round(mbPerSec * 2.0) / 2.0;

    HICON newIcon = CreateSpeedIcon(mbPerSecRounded);
    HICON oldIcon = g_nid.hIcon;
    g_nid.hIcon = newIcon;

    wchar_t tip[128];
    swprintf_s(tip, L"Входящая скорость: %.1f МБ/с", mbPerSecRounded);
    wcscpy_s(g_nid.szTip, tip);

    Shell_NotifyIconW(NIM_MODIFY, &g_nid);

    if (oldIcon) DestroyIcon(oldIcon);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                UpdateTrayIcon();
            }
            return 0;

        case WM_TRAYICON:
            return 0;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, TIMER_ID);
            Shell_NotifyIconW(NIM_DELETE, &g_nid);
            if (g_nid.hIcon) {
                DestroyIcon(g_nid.hIcon);
                g_nid.hIcon = nullptr;
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI ThreadProc(LPVOID) {
    const wchar_t* className = L"WindhawkNetSpeedTrayWndClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = className;
    RegisterClassW(&wc);

    // Message-only окно — нам не нужен видимый HWND, только для Shell_NotifyIcon
    g_hWnd = CreateWindowExW(0, className, L"NetSpeedTray", 0, 0, 0, 0, 0,
                              HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!g_hWnd) return 1;

    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = g_hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = CreateSpeedIcon(0.0);
    wcscpy_s(g_nid.szTip, L"Скорость сети: --");

    Shell_NotifyIconW(NIM_ADD, &g_nid);

    g_lastBytesIn = GetTotalBytesIn();
    g_firstSample = false;

    SetTimer(g_hWnd, TIMER_ID, g_updateIntervalMs, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterClassW(className, wc.hInstance);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    // Пример чтения настройки интервала обновления (если задана через Windhawk)
    // Явный static_cast нужен, т.к. Wh_GetIntSetting возвращает более
    // широкий целочисленный тип (обычно int64), а без него часть
    // тулчейнов ругается на сужающее преобразование при инициализации UINT
    g_updateIntervalMs = static_cast<UINT>(Wh_GetIntSetting(L"updateIntervalMs"));
    if (g_updateIntervalMs < 200) {
        g_updateIntervalMs = 1000;
    }

    g_running = true;
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModUninit() {
    g_running = false;
    if (g_hWnd) {
        PostMessageW(g_hWnd, WM_CLOSE, 0, 0);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}