// ==WindhawkMod==
// @id              network-speed-tray-icon
// @name            Network Speed Tray Icon
// @description     Shows current download speed icon in system tray
// @version         1.1
// @author          kar2ner
// @github          https://github.com/kar2ner
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -liphlpapi -lgdi32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

// RU: MIB_IF_TABLE2 / GetIfTable2 доступны начиная с Windows Vista.
// Без явного указания версии некоторые тулчейны берут более старый
// _WIN32_WINNT по умолчанию, и тогда эти типы/функции "не находятся" —
// это самая частая причина сразу пачки ошибок компиляции в таком коде.
//
// EN: MIB_IF_TABLE2 / GetIfTable2 are available starting with Windows
// Vista. Without an explicit version, some toolchains default to an
// older _WIN32_WINNT, so these types/functions become "not found" —
// this is the most common cause of a whole batch of compile errors here.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602  // Windows 8, с запасом / Windows 8, with headroom
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
COLORREF g_fontColor = RGB(255, 255, 255);  // RU: цвет из настроек / EN: color from settings
double g_roundStep = 0.5;                   // RU: шаг округления из настроек / EN: rounding step from settings

// RU: Разбор настройки цвета шрифта (строка -> COLORREF).
// EN: Parses the font color setting (string -> COLORREF).
COLORREF ParseFontColorSetting() {
    COLORREF color = RGB(255, 255, 255);  // RU: по умолчанию — белый / EN: default is white
    PCWSTR value = Wh_GetStringSetting(L"fontColor");
    if (value) {
        if (wcscmp(value, L"black") == 0) {
            color = RGB(0, 0, 0);
        } else if (wcscmp(value, L"red") == 0) {
            color = RGB(255, 70, 70);
        } else if (wcscmp(value, L"green") == 0) {
            color = RGB(80, 220, 80);
        } else if (wcscmp(value, L"yellow") == 0) {
            color = RGB(255, 220, 60);
        } else if (wcscmp(value, L"cyan") == 0) {
            color = RGB(70, 220, 255);
        } else if (wcscmp(value, L"orange") == 0) {
            color = RGB(255, 160, 60);
        }
        // RU: "white" и любое нераспознанное значение — остаются белым.
        // EN: "white" and any unrecognized value keep the white default.
        Wh_FreeStringSetting(value);
    }
    return color;
}

// RU: Разбор настройки шага округления (строка -> double).
// EN: Parses the rounding step setting (string -> double).
double ParseRoundStepSetting() {
    double step = 0.5;  // RU: по умолчанию / EN: default
    PCWSTR value = Wh_GetStringSetting(L"roundStep");
    if (value) {
        if (wcscmp(value, L"0.1") == 0) {
            step = 0.1;
        }
        Wh_FreeStringSetting(value);
    }
    return step;
}

// RU: Входящие байты только с "основного" интерфейса — того, через который
// идёт маршрут по умолчанию (т.е. реальный интернет-трафик).
//
// Раньше здесь суммировались ВСЕ интерфейсы, но в Windows часто есть
// виртуальные адаптеры (Hyper-V, WSL, VPN, Bluetooth PAN и т.п.), которые
// задваивают/учитывают тот же трафик в своих счётчиках — из-за этого
// скорость завышалась в несколько раз. GetBestInterface() отдаёт индекс
// именно того интерфейса, который реально используется для выхода в сеть.
//
// EN: Incoming bytes from only the "primary" interface — the one used
// for the default route (i.e. real internet traffic).
//
// Previously ALL interfaces were summed here, but Windows often has
// virtual adapters (Hyper-V, WSL, VPN, Bluetooth PAN, etc.) that
// double-count/mirror the same traffic in their own counters — this
// inflated the speed several times over. GetBestInterface() returns the
// index of the interface that's actually used to reach the network.
ULONG64 GetTotalBytesIn() {
    DWORD ifIndex = 0;

    // RU: 8.8.8.8 как "типичный" внешний адрес назначения — все октеты
    // одинаковы, поэтому сетевой порядок байт значения не имеет.
    // EN: 8.8.8.8 as a "typical" external destination address — all
    // octets are equal, so byte order doesn't matter here.
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

// RU: Рисует иконку 32x32 с текстом скорости в МБ/с, округлённой согласно
// настройке шага (0.5 или 0.1). Размер шрифта чуть уменьшается для
// длинных значений (3+ знака до точки), чтобы текст не обрезался.
// Текст рисуется выбранным пользователем цветом с плавным альфа-краем
// (не жёсткой маской), поэтому корректно работает и для чёрного цвета.
//
// EN: Draws a 32x32 icon with the speed text in MB/s, rounded according
// to the step setting (0.5 or 0.1). Font size shrinks a bit for longer
// values (3+ digits before the dot) so the text doesn't get clipped.
// The text is rendered in the user-selected color with a smooth alpha
// edge (not a hard mask), so it also works correctly for black.
HICON CreateSpeedIcon(double mbPerSecRounded, COLORREF fontColor) {
    const int size = 32;

    HDC screenDc = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screenDc);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;  // top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP colorBmp = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(dc, colorBmp);

    RECT rect = {0, 0, size, size};
    HBRUSH bg = (HBRUSH)GetStockObject(BLACK_BRUSH);
    // RU: фон = чёрный, но здесь он используется только как база для
    // серошкального покрытия (coverage), а не как признак прозрачности.
    // EN: background = black, but here it's only used as a base for the
    // grayscale coverage value, not as a transparency marker.
    FillRect(dc, &rect, bg);

    wchar_t buf[16];
    swprintf_s(buf, L"%.1f", mbPerSecRounded);

    // RU: Чем короче строка, тем крупнее можно взять шрифт.
    // EN: The shorter the string, the larger the font can be.
    int textLen = (int)wcslen(buf);
    int fontHeight = 20;                     // "0.5" .. "9.9"  — 3 символа / 3 chars
    if (textLen == 4) fontHeight = 16;        // "12.5"
    else if (textLen >= 5) fontHeight = 13;   // "125.5" и длиннее / and longer

    SetBkMode(dc, TRANSPARENT);
    // RU: Текст всегда рисуем белым — это лишь "маска покрытия" (coverage),
    // реальный цвет применяется ниже при сборке ARGB-пикселей.
    // EN: Text is always drawn in white — this is just a "coverage mask",
    // the real color is applied below when composing ARGB pixels.
    SetTextColor(dc, RGB(255, 255, 255));

    // RU: ANTIALIASED_QUALITY (а не ClearType) даёт серошкальное
    // сглаживание, где R=G=B — это нужно, чтобы использовать канал как
    // честную альфу при последующей раскраске текста.
    // EN: ANTIALIASED_QUALITY (not ClearType) gives grayscale
    // anti-aliasing where R=G=B — needed so the channel can be used as
    // a proper alpha value when recoloring the text below.
    HFONT font = CreateFontW(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(dc, font);

    DrawTextW(dc, buf, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(dc, oldFont);
    DeleteObject(font);

    // RU: Собираем итоговые ARGB-пиксели: альфа = яркость (покрытие)
    // отрисованного текста, цвет = выбранный fontColor, "premultiplied"
    // на альфу — так антиалиасинг по краям остаётся плавным для любого
    // выбранного цвета, включая чёрный.
    //
    // EN: Compose final ARGB pixels: alpha = brightness (coverage) of the
    // rendered text, color = the chosen fontColor, premultiplied by
    // alpha — this keeps anti-aliased edges smooth for any selected
    // color, including black.
    BYTE fr = GetRValue(fontColor);
    BYTE fg = GetGValue(fontColor);
    BYTE fb = GetBValue(fontColor);

    DWORD* pixels = static_cast<DWORD*>(bits);
    for (int i = 0; i < size * size; i++) {
        BYTE coverage = GetRValue(pixels[i]);  // RU: R=G=B, серошкальное покрытие / EN: R=G=B, grayscale coverage
        BYTE a = coverage;
        BYTE r = (BYTE)((fr * a) / 255);
        BYTE g = (BYTE)((fg * a) / 255);
        BYTE b = (BYTE)((fb * a) / 255);
        pixels[i] = (static_cast<DWORD>(a) << 24) | (r << 16) | (g << 8) | b;
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
    // RU: Округление до шага из настроек (0.5 или 0.1 МБ/с).
    // EN: Rounding to the step from settings (0.5 or 0.1 MB/s).
    double mbPerSecRounded = round(mbPerSec / g_roundStep) * g_roundStep;

    HICON newIcon = CreateSpeedIcon(mbPerSecRounded, g_fontColor);
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

    // RU: Message-only окно — нам не нужен видимый HWND, только для
    // Shell_NotifyIcon.
    // EN: Message-only window — we don't need a visible HWND, only for
    // Shell_NotifyIcon.
    g_hWnd = CreateWindowExW(0, className, L"NetSpeedTray", 0, 0, 0, 0, 0,
                              HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!g_hWnd) return 1;

    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = g_hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = CreateSpeedIcon(0.0, g_fontColor);
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
    // RU: Явный static_cast нужен, т.к. Wh_GetIntSetting возвращает более
    // широкий целочисленный тип (обычно int64), а без него часть
    // тулчейнов ругается на сужающее преобразование при инициализации UINT.
    // EN: An explicit static_cast is needed because Wh_GetIntSetting
    // returns a wider integer type (usually int64), and without it some
    // toolchains complain about a narrowing conversion when initializing UINT.
    g_updateIntervalMs = static_cast<UINT>(Wh_GetIntSetting(L"updateIntervalMs"));
    if (g_updateIntervalMs < 200) {
        g_updateIntervalMs = 1000;
    }

    g_fontColor = ParseFontColorSetting();
    g_roundStep = ParseRoundStepSetting();

    // RU: Ничего не блокирует запуск при старте системы: мод не делает
    // ничего, что требовало бы "ручного" вмешательства — если он включён
    // (тумблер в списке модов Windhawk), он будет так же автоматически
    // грузиться в explorer.exe при каждом его запуске, как и остальные
    // моды. Если после перезагрузки иконка не появляется сама, дело не
    // в этом файле — см. пояснение в чате про автозапуск самого Windhawk.
    //
    // EN: Nothing here blocks startup: the mod doesn't do anything that
    // would require "manual" intervention — if it's enabled (the toggle
    // in Windhawk's mod list), it will auto-load into explorer.exe on
    // every launch, exactly like other mods. If the icon doesn't appear
    // by itself after a reboot, it's not this file — see the chat
    // explanation about Windhawk's own autostart setting.
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
