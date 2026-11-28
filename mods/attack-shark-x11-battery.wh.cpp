// ==WindhawkMod==
// @id              attack-shark-x11-battery
// @name            Attack Shark X11 Battery
// @name:es-ES      Batería del Attack Shark X11
// @description     Shows the battery level of the Attack Shark X11 wireless mouse in the system tray
// @description:es-ES Muestra la batería del ratón inalámbrico Attack Shark X11 en la bandeja del sistema
// @version         1.0
// @author          xtrimdr
// @github          https://github.com/xtrimdr
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lsetupapi -lhid -lgdi32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Attack Shark X11 Battery

Adds a system tray icon that shows the battery percentage of the **Attack Shark
X11** wireless mouse, connected through its 2.4 GHz receiver (USB `1D57:FA60`).

![Icon styles on a dark and a light taskbar: 75%, 42%, 12% (low), charging, no signal and no data](https://raw.githubusercontent.com/xtrimdr/windhawk-mods/assets/attack-shark-x11-battery/preview.png)

* Two icon styles: the battery percentage as a number, or a mouse that fills
  up with the battery level.
* The icon changes color: green (above 50%), yellow, red (at or below the low
  battery threshold) and blue while charging.
* If the mouse stops sending data (for example, because it went to sleep), the
  last known value stays in gray.
* Hover the icon to see the percentage and the charging state.
* Optional notifications when the battery is low and when charging finishes.

Texts are shown in Spanish when Windows is set to Spanish, otherwise in English.

The mod also matches the mouse connected by cable (USB `1D57:FA55`), but that
mode is untested.

## How it works

The mouse sends a battery report on its own every ~2 seconds through interface
2 of the receiver (usage page `0x0A`, report ID `3`). The mod only **listens**
to these reports; it never sends anything to the mouse or the receiver, so it
can't change the mouse configuration.

The mod runs in its own `windhawk.exe` process, not inside Explorer. If the
receiver is unplugged, the mod picks it up again as soon as it's plugged back
in.

The protocol is documented in
[attack-shark-x11-driver](https://github.com/HarukaYamamoto0/attack-shark-x11-driver).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconStyle: number
  $name: Icon style
  $name:es-ES: Estilo del icono
  $options:
  - number: Battery percentage (number)
  - mouse: Mouse that fills up with the battery level
  $options:es-ES:
  - number: Porcentaje de batería (número)
  - mouse: Ratón que se llena con el nivel de batería
- lowThreshold: 20
  $name: Low battery threshold (%)
  $name:es-ES: Umbral de batería baja (%)
  $description: At or below this value the icon turns red
  $description:es-ES: En este valor o por debajo, el icono se pone rojo
- notifyLow: true
  $name: Notify on low battery
  $name:es-ES: Avisar con batería baja
  $description: Shows a Windows notification when the battery reaches the threshold
  $description:es-ES: Muestra una notificación de Windows cuando la batería llega al umbral
- notifyFull: false
  $name: Notify when charging finishes
  $name:es-ES: Avisar cuando termina de cargar
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <dbt.h>
#include <hidsdi.h>  // Includes hidpi.h after the types it needs.
#include <setupapi.h>
#include <shellapi.h>

#include <atomic>
#include <cwchar>
#include <mutex>
#include <string>
#include <vector>

constexpr UINT WM_APP_TRAY = WM_APP + 1;
constexpr UINT WM_APP_UPDATE = WM_APP + 2;
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 3;
constexpr UINT_PTR kStaleTimerId = 1;
constexpr ULONGLONG kStaleAfterMs = 15000;
// Slow rescan in case a device arrival notification is missed.
constexpr DWORD kFallbackRescanMs = 60000;
constexpr UINT kTrayIconId = 1;
constexpr wchar_t kWindowClass[] = L"Windhawk_AttackSharkX11Battery_" WH_MOD_ID;

enum BatteryStatus : BYTE {
    kStatusNormal = 0x01,
    kStatusFull = 0x02,
    kStatusCharging = 0x03,
};

struct BatteryState {
    int percent = -1;  // -1: no report received yet
    BYTE status = 0;
    ULONGLONG lastReportTick = 0;
    bool deviceOpen = false;
};

enum class IconStyle {
    kNumber,
    kMouse,
};

struct Settings {
    IconStyle iconStyle = IconStyle::kNumber;
    int lowThreshold = 20;
    bool notifyLow = true;
    bool notifyFull = false;
};

// ---------------------------------------------------------------------------
// Texts (Spanish if the Windows UI language is Spanish, English otherwise)
// ---------------------------------------------------------------------------

struct Texts {
    const wchar_t* title;
    const wchar_t* waiting;
    const wchar_t* noReceiver;
    const wchar_t* noReceiverLast;
    const wchar_t* noSignalLast;
    const wchar_t* charging;
    const wchar_t* full;
    const wchar_t* onBattery;
    const wchar_t* lowTitle;
    const wchar_t* lowBody;  // printf format with %d
    const wchar_t* fullBody;
};

static const Texts kTextsEn = {
    L"Attack Shark X11",
    L"Waiting for data...",
    L"Receiver not connected",
    L"Receiver not connected (last value)",
    L"No signal (last value)",
    L"Charging",
    L"Fully charged",
    L"On battery",
    L"Attack Shark X11: low battery",
    L"%d%% battery left. Put it on the charger.",
    L"Charging finished.",
};

static const Texts kTextsEs = {
    L"Attack Shark X11",
    L"Esperando datos...",
    L"Receptor no conectado",
    L"Receptor no conectado (último valor)",
    L"Sin señal (último valor)",
    L"Cargando",
    L"Carga completa",
    L"Con batería",
    L"Attack Shark X11: batería baja",
    L"Queda %d%% de batería. Conéctalo al cargador.",
    L"Terminó de cargar.",
};

static const Texts& T() {
    static const Texts& texts =
        PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_SPANISH ? kTextsEs
                                                                  : kTextsEn;
    return texts;
}

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------

static std::mutex g_stateMutex;
static BatteryState g_state;

static HANDLE g_stopEvent = nullptr;
static HANDLE g_rescanEvent = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_hidThread = nullptr;
static std::atomic<HWND> g_hwnd{nullptr};

// Only accessed from the UI thread.
static Settings g_settings;
static UINT g_taskbarCreatedMsg = 0;
static HICON g_currentIcon = nullptr;
static bool g_iconAdded = false;
static bool g_notifiedLow = false;
static BYTE g_lastStatus = 0;
static std::wstring g_lastText;
static std::wstring g_lastTip;
static COLORREF g_lastColor = 0;
static int g_lastIconSize = 0;

static Settings ReadSettings() {
    Settings s;
    WindhawkUtils::StringSetting style =
        WindhawkUtils::StringSetting::make(L"iconStyle");
    s.iconStyle =
        wcscmp(style, L"mouse") == 0 ? IconStyle::kMouse : IconStyle::kNumber;
    int low = Wh_GetIntSetting(L"lowThreshold");
    s.lowThreshold = (low > 0 && low < 100) ? low : 20;
    s.notifyLow = Wh_GetIntSetting(L"notifyLow") != 0;
    s.notifyFull = Wh_GetIntSetting(L"notifyFull") != 0;
    return s;
}

static void NotifyUi() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_UPDATE, 0, 0);
    }
}

static HMODULE GetModModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (PCWSTR)&GetModModule, &module);
    return module;
}

// ---------------------------------------------------------------------------
// HID reading
// ---------------------------------------------------------------------------

static bool ContainsNoCase(const std::wstring& haystack, const wchar_t* needle) {
    size_t n = wcslen(needle);
    for (size_t i = 0; i + n <= haystack.size(); i++) {
        if (_wcsnicmp(haystack.c_str() + i, needle, n) == 0) {
            return true;
        }
    }
    return false;
}

// Interface 2, collection 3 is the one with usage page 0x0A, where the mouse
// sends its battery reports. PID FA60 is the 2.4 GHz receiver, FA55 is the
// mouse connected by cable.
static bool IsTargetPath(const std::wstring& path) {
    bool vidPid = ContainsNoCase(path, L"vid_1d57&pid_fa60") ||
                  ContainsNoCase(path, L"vid_1d57&pid_fa55");
    return vidPid && ContainsNoCase(path, L"&mi_02") &&
           ContainsNoCase(path, L"&col03");
}

static std::wstring FindDevicePath() {
    std::wstring result;
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO devs = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr,
                                         DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (devs == INVALID_HANDLE_VALUE) {
        return result;
    }

    SP_DEVICE_INTERFACE_DATA ifData{};
    ifData.cbSize = sizeof(ifData);
    for (DWORD i = 0;
         SetupDiEnumDeviceInterfaces(devs, nullptr, &hidGuid, i, &ifData);
         i++) {
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(devs, &ifData, nullptr, 0, &needed,
                                         nullptr);
        if (needed == 0) {
            continue;
        }
        std::vector<BYTE> buffer(needed);
        auto* detail = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)buffer.data();
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (SetupDiGetDeviceInterfaceDetailW(devs, &ifData, detail, needed,
                                             nullptr, nullptr) &&
            IsTargetPath(detail->DevicePath)) {
            result = detail->DevicePath;
            break;
        }
    }

    SetupDiDestroyDeviceInfoList(devs);
    return result;
}

static DWORD GetInputReportLength(HANDLE dev) {
    DWORD length = 0;
    PHIDP_PREPARSED_DATA preparsed = nullptr;
    if (HidD_GetPreparsedData(dev, &preparsed)) {
        HIDP_CAPS caps{};
        if (HidP_GetCaps(preparsed, &caps) == HIDP_STATUS_SUCCESS) {
            length = caps.InputReportByteLength;
        }
        HidD_FreePreparsedData(preparsed);
    }
    return length;
}

static void HandleReport(const BYTE* buf, DWORD len) {
    // [0] = 0x03 report ID, [1] = 0x55 X11, [2] = 0x40/0x41 battery event,
    // [3] = status, [4] = percentage.
    if (len < 5 || buf[0] != 0x03 || (buf[2] != 0x40 && buf[2] != 0x41)) {
        return;
    }
    int percent = buf[4];
    if (percent > 100) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        bool changed = g_state.percent != percent || g_state.status != buf[3];
        bool wasStale =
            GetTickCount64() - g_state.lastReportTick > kStaleAfterMs;
        g_state.percent = percent;
        g_state.status = buf[3];
        g_state.lastReportTick = GetTickCount64();
        if (!changed && !wasStale) {
            return;
        }
    }
    NotifyUi();
}

static void SetDeviceOpen(bool open) {
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_state.deviceOpen == open) {
            return;
        }
        g_state.deviceOpen = open;
    }
    NotifyUi();
}

// Reads reports until the mod is unloaded or a read fails (receiver
// unplugged).
static void ReadLoop(HANDLE dev) {
    DWORD reportLength = GetInputReportLength(dev);
    if (reportLength == 0) {
        Wh_Log(L"Couldn't get the input report length");
        return;
    }
    std::vector<BYTE> buf(reportLength);

    HANDLE readEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readEvent) {
        return;
    }
    while (true) {
        OVERLAPPED ov{};
        ov.hEvent = readEvent;
        ResetEvent(readEvent);
        DWORD read = 0;
        if (!ReadFile(dev, buf.data(), reportLength, nullptr, &ov) &&
            GetLastError() != ERROR_IO_PENDING) {
            Wh_Log(L"ReadFile failed: %u", GetLastError());
            break;
        }
        HANDLE waits[] = {g_stopEvent, readEvent};
        DWORD w = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (w != WAIT_OBJECT_0 + 1) {
            CancelIoEx(dev, &ov);
            GetOverlappedResult(dev, &ov, &read, TRUE);
            break;
        }
        if (!GetOverlappedResult(dev, &ov, &read, FALSE)) {
            Wh_Log(L"Read failed: %u", GetLastError());
            break;
        }
        HandleReport(buf.data(), read);
    }
    CloseHandle(readEvent);
}

static DWORD WINAPI HidThreadProc(LPVOID) {
    HANDLE waits[] = {g_stopEvent, g_rescanEvent};
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        ResetEvent(g_rescanEvent);
        std::wstring path = FindDevicePath();
        if (!path.empty()) {
            HANDLE dev = CreateFileW(
                path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
            if (dev != INVALID_HANDLE_VALUE) {
                Wh_Log(L"Receiver opened: %s", path.c_str());
                SetDeviceOpen(true);
                ReadLoop(dev);
                CloseHandle(dev);
                SetDeviceOpen(false);
                Wh_Log(L"Receiver closed");
                // Give the device a moment before looking for it again.
                if (WaitForSingleObject(g_stopEvent, 1000) != WAIT_TIMEOUT) {
                    break;
                }
                continue;
            }
            Wh_Log(L"CreateFile failed: %u", GetLastError());
        }
        // Wait for a HID device arrival (signaled by the UI thread), with a
        // slow periodic rescan as a fallback.
        if (WaitForMultipleObjects(2, waits, FALSE, kFallbackRescanMs) ==
            WAIT_OBJECT_0) {
            break;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Tray icon
// ---------------------------------------------------------------------------

static UINT GetTaskbarDpi() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 0;
    return dpi ? dpi : GetDpiForSystem();
}

// Draws white shapes on black with `draw` at `scale` times the icon size, and
// returns the coverage of each icon pixel (0-255). Drawing larger and
// averaging gives antialiased edges to plain GDI shapes.
template <typename DrawFn>
static std::vector<BYTE> RenderCoverage(int size, int scale, DrawFn draw) {
    std::vector<BYTE> coverage;
    int big = size * scale;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = big;
    bi.bmiHeader.biHeight = -big;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP bmp =
        CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!mem || !bmp || !bits) {
        if (bmp) {
            DeleteObject(bmp);
        }
        if (mem) {
            DeleteDC(mem);
        }
        return coverage;
    }

    HGDIOBJ oldBmp = SelectObject(mem, bmp);
    memset(bits, 0, big * big * 4);
    draw(mem, big);
    GdiFlush();

    coverage.resize(size * size);
    auto* px = (const BYTE*)bits;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int sum = 0;
            for (int sy = 0; sy < scale; sy++) {
                const BYTE* row = px + ((y * scale + sy) * big + x * scale) * 4;
                for (int sx = 0; sx < scale; sx++, row += 4) {
                    // Max of the channels, in case the text is drawn with
                    // subpixel colors.
                    BYTE m = row[0] > row[1] ? row[0] : row[1];
                    sum += m > row[2] ? m : row[2];
                }
            }
            coverage[y * size + x] = (BYTE)(sum / (scale * scale));
        }
    }

    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
    return coverage;
}

// Turns a coverage mask into an icon of the given color (premultiplied
// alpha).
static HICON CoverageToIcon(const std::vector<BYTE>& coverage,
                            int size,
                            COLORREF color) {
    if (coverage.size() != (size_t)(size * size)) {
        return nullptr;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP color32 =
        CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color32 || !bits) {
        if (color32) {
            DeleteObject(color32);
        }
        return nullptr;
    }

    BYTE cr = GetRValue(color), cg = GetGValue(color), cb = GetBValue(color);
    auto* px = (BYTE*)bits;
    for (int i = 0; i < size * size; i++, px += 4) {
        BYTE a = coverage[i];
        px[0] = (BYTE)(cb * a / 255);
        px[1] = (BYTE)(cg * a / 255);
        px[2] = (BYTE)(cr * a / 255);
        px[3] = a;
    }

    HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmColor = color32;
    ii.hbmMask = mask;
    HICON icon = CreateIconIndirect(&ii);

    DeleteObject(mask);
    DeleteObject(color32);
    return icon;
}

static void DrawNumber(HDC dc, int size, const wchar_t* text) {
    size_t len = wcslen(text);
    LOGFONTW lf{};
    lf.lfHeight = -(len >= 3 ? size * 17 / 20 : size * 19 / 20);
    lf.lfWeight = FW_BOLD;
    lf.lfQuality = ANTIALIASED_QUALITY;  // grayscale, no ClearType
    // "100" doesn't fit with Segoe UI, use the condensed Bahnschrift variant.
    wcscpy(lf.lfFaceName, len >= 3 ? L"Bahnschrift Condensed" : L"Segoe UI");
    HFONT font = CreateFontIndirectW(&lf);
    HGDIOBJ oldFont = SelectObject(dc, font);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    RECT rc{0, 0, size, size};
    DrawTextW(dc, text, -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

    SelectObject(dc, oldFont);
    DeleteObject(font);
}

// A mouse outline (body and button split) that fills up from the bottom with
// the battery level. `percent` < 0 draws only the outline.
static void DrawMouse(HDC dc, int size, int percent) {
    int w = size * 60 / 100;
    int h = size * 94 / 100;
    int left = (size - w) / 2;
    int top = (size - h) / 2;
    int right = left + w;
    int bottom = top + h;
    int t = size * 10 / 100;  // stroke width
    if (t < 1) {
        t = 1;
    }
    int buttonsY = top + h * 42 / 100;

    HBRUSH white = (HBRUSH)GetStockObject(WHITE_BRUSH);

    HRGN outer = CreateRoundRectRgn(left, top, right + 1, bottom + 1, w, w);
    HRGN inner = CreateRoundRectRgn(left + t, top + t, right - t + 1,
                                    bottom - t + 1, w - 2 * t, w - 2 * t);
    HRGN ring = CreateRectRgn(0, 0, 0, 0);
    CombineRgn(ring, outer, inner, RGN_DIFF);
    FillRgn(dc, ring, white);

    // Button split: a horizontal line and a vertical line above it.
    HRGN split = CreateRectRgn(left, buttonsY - t / 2, right, buttonsY - t / 2 + t);
    HRGN vertical = CreateRectRgn(size / 2 - t / 2, top, size / 2 - t / 2 + t,
                                  buttonsY);
    CombineRgn(split, split, vertical, RGN_OR);
    CombineRgn(split, split, inner, RGN_AND);
    FillRgn(dc, split, white);

    // The battery level fills the body below the buttons, from the bottom up.
    // The buttons are always drawn as an outline.
    if (percent > 0) {
        int areaTop = buttonsY - t / 2 + t;
        int areaBottom = bottom - t;
        int fillTop = areaBottom - (areaBottom - areaTop) *
                                       (percent > 100 ? 100 : percent) / 100;
        HRGN fill = CreateRectRgn(left, fillTop, right + 1, bottom + 1);
        CombineRgn(fill, fill, inner, RGN_AND);
        FillRgn(dc, fill, white);
        DeleteObject(fill);
    }

    DeleteObject(vertical);
    DeleteObject(split);
    DeleteObject(ring);
    DeleteObject(inner);
    DeleteObject(outer);
}

// `percent` < 0 means unknown.
static HICON CreateBatteryIcon(IconStyle style,
                               int percent,
                               COLORREF color,
                               int size) {
    std::vector<BYTE> coverage;
    if (style == IconStyle::kMouse) {
        coverage = RenderCoverage(size, 4, [percent](HDC dc, int big) {
            DrawMouse(dc, big, percent);
        });
    } else {
        wchar_t text[8];
        if (percent < 0) {
            wcscpy(text, L"--");
        } else {
            swprintf(text, ARRAYSIZE(text), L"%d", percent);
        }
        coverage = RenderCoverage(size, 1, [&text](HDC dc, int big) {
            DrawNumber(dc, big, text);
        });
    }
    return CoverageToIcon(coverage, size, color);
}

static void ShowBalloon(HWND hwnd,
                        const wchar_t* title,
                        const wchar_t* text,
                        DWORD infoFlags) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = kTrayIconId;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = infoFlags;
    wcsncpy(nid.szInfoTitle, title, ARRAYSIZE(nid.szInfoTitle) - 1);
    wcsncpy(nid.szInfo, text, ARRAYSIZE(nid.szInfo) - 1);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static void UpdateTrayIcon(HWND hwnd, bool forceAdd) {
    BatteryState st;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        st = g_state;
    }
    const Settings& set = g_settings;
    const Texts& t = T();

    bool fresh = st.percent >= 0 &&
                 GetTickCount64() - st.lastReportTick <= kStaleAfterMs;
    bool charging = st.status == kStatusCharging;

    wchar_t text[8];
    COLORREF color;
    wchar_t tip[128];

    if (st.percent < 0) {
        wcscpy(text, L"--");
        color = RGB(150, 150, 150);
        swprintf(tip, ARRAYSIZE(tip), L"%s\n%s", t.title,
                 st.deviceOpen ? t.waiting : t.noReceiver);
    } else {
        swprintf(text, ARRAYSIZE(text), L"%d", st.percent);
        if (!fresh) {
            color = RGB(150, 150, 150);
        } else if (charging) {
            color = RGB(64, 160, 255);
        } else if (st.percent <= set.lowThreshold) {
            color = RGB(255, 70, 70);
        } else if (st.percent <= 50) {
            color = RGB(255, 193, 7);
        } else {
            color = RGB(76, 217, 100);
        }
        const wchar_t* statusText = !st.deviceOpen ? t.noReceiverLast
                                    : !fresh       ? t.noSignalLast
                                    : charging     ? t.charging
                                    : st.status == kStatusFull ? t.full
                                                               : t.onBattery;
        swprintf(tip, ARRAYSIZE(tip), L"%s: %d%%\n%s", t.title, st.percent,
                 statusText);
    }

    int size = GetSystemMetricsForDpi(SM_CXSMICON, GetTaskbarDpi());
    if (size <= 0) {
        size = 16;
    }

    // The cache key also includes the style, since the same text can be drawn
    // as a number or as a mouse.
    std::wstring key = (set.iconStyle == IconStyle::kMouse ? L"mouse:" : L"num:");
    key += text;
    bool unchanged = g_iconAdded && g_lastText == key && g_lastTip == tip &&
                     g_lastColor == color && g_lastIconSize == size;
    if (forceAdd || !unchanged) {
        HICON icon = CreateBatteryIcon(set.iconStyle,
                                       st.percent < 0 ? -1 : st.percent, color,
                                       size);

        NOTIFYICONDATAW nid{};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = kTrayIconId;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
        nid.uCallbackMessage = WM_APP_TRAY;
        nid.hIcon = icon;
        wcsncpy(nid.szTip, tip, ARRAYSIZE(nid.szTip) - 1);

        if (forceAdd || !g_iconAdded) {
            Shell_NotifyIconW(NIM_DELETE, &nid);
            g_iconAdded = Shell_NotifyIconW(NIM_ADD, &nid);
            if (g_iconAdded) {
                nid.uVersion = NOTIFYICON_VERSION_4;
                Shell_NotifyIconW(NIM_SETVERSION, &nid);
            }
        } else {
            Shell_NotifyIconW(NIM_MODIFY, &nid);
        }

        if (g_currentIcon) {
            DestroyIcon(g_currentIcon);
        }
        g_currentIcon = icon;
        g_lastText = key;
        g_lastTip = tip;
        g_lastColor = color;
        g_lastIconSize = size;
    }

    // Notifications.
    if (fresh) {
        if (charging || st.percent > set.lowThreshold + 5) {
            g_notifiedLow = false;
        }
        if (set.notifyLow && !g_notifiedLow && !charging &&
            st.percent <= set.lowThreshold) {
            g_notifiedLow = true;
            wchar_t msg[96];
            swprintf(msg, ARRAYSIZE(msg), t.lowBody, st.percent);
            ShowBalloon(hwnd, t.lowTitle, msg, NIIF_WARNING);
        }
        if (set.notifyFull && g_lastStatus == kStatusCharging &&
            st.status == kStatusFull) {
            ShowBalloon(hwnd, t.title, t.fullBody, NIIF_INFO);
        }
        g_lastStatus = st.status;
    }
}

static void RemoveTrayIcon(HWND hwnd) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = kTrayIconId;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    g_iconAdded = false;
    if (g_currentIcon) {
        DestroyIcon(g_currentIcon);
        g_currentIcon = nullptr;
    }
}

static bool IsTargetDeviceArrival(WPARAM event, LPARAM data) {
    if (event != DBT_DEVICEARRIVAL || !data) {
        return false;
    }
    auto* header = (const DEV_BROADCAST_HDR*)data;
    if (header->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE ||
        header->dbch_size < sizeof(DEV_BROADCAST_DEVICEINTERFACE_W)) {
        return false;
    }
    auto* info = (const DEV_BROADCAST_DEVICEINTERFACE_W*)header;
    size_t capacity =
        (header->dbch_size - offsetof(DEV_BROADCAST_DEVICEINTERFACE_W, dbcc_name)) /
        sizeof(wchar_t);
    return IsTargetPath(std::wstring(info->dbcc_name,
                                     wcsnlen(info->dbcc_name, capacity)));
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        // Explorer recreated the taskbar, the icon has to be added again.
        UpdateTrayIcon(hwnd, true);
        return 0;
    }
    switch (msg) {
        case WM_CREATE:
            SetTimer(hwnd, kStaleTimerId, 5000, nullptr);
            UpdateTrayIcon(hwnd, true);
            return 0;
        case WM_APP_UPDATE:
        case WM_TIMER:
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
            UpdateTrayIcon(hwnd, false);
            return 0;
        case WM_APP_SETTINGS_CHANGED: {
            Settings old = g_settings;
            g_settings = ReadSettings();
            // Show the low battery notification again only if a setting it
            // depends on changed.
            if (old.lowThreshold != g_settings.lowThreshold ||
                old.notifyLow != g_settings.notifyLow) {
                g_notifiedLow = false;
            }
            UpdateTrayIcon(hwnd, false);
            return 0;
        }
        case WM_DEVICECHANGE:
            if (IsTargetDeviceArrival(wp, lp)) {
                SetEvent(g_rescanEvent);
            }
            return TRUE;
        case WM_POWERBROADCAST:
            if (wp == PBT_APMRESUMEAUTOMATIC) {
                SetEvent(g_rescanEvent);
            }
            return TRUE;
        case WM_APP_TRAY:
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kStaleTimerId);
            RemoveTrayIcon(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    HMODULE module = GetModModule();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = module;
    wc.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassEx failed: %u", GetLastError());
        return 0;
    }

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    HWND hwnd = CreateWindowExW(0, kWindowClass, L"Attack Shark X11 Battery", 0,
                                0, 0, 0, 0, nullptr, nullptr, module, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        UnregisterClassW(kWindowClass, module);
        return 0;
    }
    ChangeWindowMessageFilterEx(hwnd, g_taskbarCreatedMsg, MSGFLT_ALLOW,
                                nullptr);

    DEV_BROADCAST_DEVICEINTERFACE_W filter{};
    filter.dbcc_size = sizeof(filter);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    HidD_GetHidGuid(&filter.dbcc_classguid);
    HDEVNOTIFY devNotify =
        RegisterDeviceNotificationW(hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!devNotify) {
        Wh_Log(L"RegisterDeviceNotification failed: %u", GetLastError());
    }

    g_hwnd = hwnd;
    NotifyUi();

    // The loop ends when the stop event is set, so shutdown doesn't depend on
    // posting a message to the window.
    bool running = true;
    while (running) {
        DWORD w = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                            QS_ALLINPUT);
        if (w == WAIT_OBJECT_0) {
            break;
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_hwnd = nullptr;
    if (devNotify) {
        UnregisterDeviceNotification(devNotify);
    }
    DestroyWindow(hwnd);
    UnregisterClassW(kWindowClass, module);
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod callbacks
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    g_settings = ReadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_rescanEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_rescanEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }
    // g_settings is written before the UI thread starts and only used by it
    // afterwards.
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    g_hidThread = CreateThread(nullptr, 0, HidThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread || !g_hidThread) {
        Wh_Log(L"CreateThread failed");
        SetEvent(g_stopEvent);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    SetEvent(g_stopEvent);
    // The process is dedicated to this mod, so if a thread doesn't exit in
    // time, ending the process is a safe fallback.
    for (HANDLE* thread : {&g_uiThread, &g_hidThread}) {
        if (*thread) {
            if (WaitForSingleObject(*thread, 5000) != WAIT_OBJECT_0) {
                Wh_Log(L"Thread did not exit in time");
                ExitProcess(1);
            }
            CloseHandle(*thread);
            *thread = nullptr;
        }
    }
    CloseHandle(g_rescanEvent);
    g_rescanEvent = nullptr;
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
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
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
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
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
