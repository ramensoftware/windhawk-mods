// ==WindhawkMod==
// @id              attack-shark-x11-battery
// @name            Attack Shark X11 Battery
// @name:es-ES      Batería del Attack Shark X11
// @description     Shows the battery level of the Attack Shark X11 wireless mouse in the system tray
// @description:es-ES Muestra la batería del mouse inalámbrico Attack Shark X11 en la bandeja del sistema
// @version         1.0
// @author          xtrimdr
// @github          https://github.com/xtrimdr
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lsetupapi -lgdi32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Attack Shark X11 Battery

Adds a system tray icon that shows the battery percentage of the **Attack Shark
X11** wireless mouse (2.4 GHz receiver, USB `1D57:FA60`).

* The number changes color: green (above 50%), yellow, red (at or below the
  low battery threshold) and blue while charging.
* If the mouse stops sending data (for example, because it went to sleep), the
  last known value stays in gray.
* Hover the icon to see the percentage and the charging state.
* Optional notifications when the battery is low and when charging finishes.

Texts are shown in Spanish when Windows is set to Spanish, otherwise in English.

## How it works

The mouse sends a battery report on its own every ~2 seconds through interface
2 of the receiver (usage page `0x0A`, report ID `3`). The mod only **listens**
to these reports; it never sends anything to the mouse or the receiver, so it
can't change the mouse configuration.

If the receiver is unplugged, the mod keeps retrying and reconnects on its own.

The protocol is documented in
[attack-shark-x11-driver](https://github.com/HarukaYamamoto0/attack-shark-x11-driver).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lowThreshold: 20
  $name: Low battery threshold (%)
  $description: At or below this value the number turns red
- notifyLow: true
  $name: Notify on low battery
  $description: Shows a Windows notification when the battery reaches the threshold
- notifyFull: false
  $name: Notify when charging finishes
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <setupapi.h>
#include <shellapi.h>

#include <atomic>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>

// {4D1E55B2-F16F-11CF-88CB-001111000030}
static const GUID kHidInterfaceGuid = {
    0x4d1e55b2,
    0xf16f,
    0x11cf,
    {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}};

constexpr UINT WM_APP_TRAY = WM_APP + 1;
constexpr UINT WM_APP_UPDATE = WM_APP + 2;
constexpr UINT_PTR kStaleTimerId = 1;
constexpr ULONGLONG kStaleAfterMs = 15000;
constexpr UINT kTrayIconId = 1;
constexpr wchar_t kWindowClass[] = L"WhAttackSharkX11BatteryWnd";

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

struct Settings {
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
    L"Waiting for the mouse...",
    L"Receiver not connected",
    L"Receiver not connected (last value)",
    L"No signal from the mouse (last value)",
    L"Charging",
    L"Fully charged",
    L"On battery",
    L"Attack Shark X11: low battery",
    L"%d%% battery left. Put it on the charger.",
    L"Charging finished.",
};

static const Texts kTextsEs = {
    L"Attack Shark X11",
    L"Esperando datos del mouse...",
    L"Receptor no conectado",
    L"Receptor no conectado (último valor)",
    L"Sin señal del mouse (último valor)",
    L"Cargando",
    L"Carga completa",
    L"Con batería",
    L"Attack Shark X11: batería baja",
    L"Queda %d%% de batería. Ponelo a cargar.",
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
static Settings g_settings;

static HANDLE g_stopEvent = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_hidThread = nullptr;
static std::atomic<HWND> g_hwnd{nullptr};

// Only accessed from the UI thread.
static UINT g_taskbarCreatedMsg = 0;
static HICON g_currentIcon = nullptr;
static bool g_iconAdded = false;
static bool g_notifiedLow = false;
static BYTE g_lastStatus = 0;

static void LoadSettings() {
    Settings s;
    int low = Wh_GetIntSetting(L"lowThreshold");
    s.lowThreshold = (low > 0 && low < 100) ? low : 20;
    s.notifyLow = Wh_GetIntSetting(L"notifyLow") != 0;
    s.notifyFull = Wh_GetIntSetting(L"notifyFull") != 0;

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_settings = s;
}

// wParam = 1 resets the low battery notification (settings changed).
static void NotifyUi(WPARAM resetNotifications = 0) {
    HWND hwnd = g_hwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_UPDATE, resetNotifications, 0);
    }
}

// ---------------------------------------------------------------------------
// HID reading
// ---------------------------------------------------------------------------

// Interface 2, collection 3 is the one with usage page 0x0A, where the mouse
// sends its battery reports. PID FA60 is the 2.4 GHz receiver, FA55 is the
// mouse connected by cable.
static bool IsTargetPath(std::wstring path) {
    for (auto& c : path) {
        c = (wchar_t)towlower(c);
    }
    bool vidPid = path.find(L"vid_1d57&pid_fa60") != std::wstring::npos ||
                  path.find(L"vid_1d57&pid_fa55") != std::wstring::npos;
    return vidPid && path.find(L"&mi_02") != std::wstring::npos &&
           path.find(L"&col03") != std::wstring::npos;
}

static std::wstring FindDevicePath() {
    std::wstring result;
    HDEVINFO devs = SetupDiGetClassDevsW(&kHidInterfaceGuid, nullptr, nullptr,
                                         DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (devs == INVALID_HANDLE_VALUE) {
        return result;
    }

    SP_DEVICE_INTERFACE_DATA ifData{};
    ifData.cbSize = sizeof(ifData);
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(
             devs, nullptr, &kHidInterfaceGuid, i, &ifData);
         i++) {
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(devs, &ifData, nullptr, 0, &needed,
                                         nullptr);
        if (needed == 0) {
            continue;
        }
        auto* detail = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)HeapAlloc(
            GetProcessHeap(), HEAP_ZERO_MEMORY, needed);
        if (!detail) {
            continue;
        }
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (SetupDiGetDeviceInterfaceDetailW(devs, &ifData, detail, needed,
                                             nullptr, nullptr) &&
            IsTargetPath(detail->DevicePath)) {
            result = detail->DevicePath;
        }
        HeapFree(GetProcessHeap(), 0, detail);
        if (!result.empty()) {
            break;
        }
    }

    SetupDiDestroyDeviceInfoList(devs);
    return result;
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
    HANDLE readEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readEvent) {
        return;
    }
    BYTE buf[64];
    while (true) {
        OVERLAPPED ov{};
        ov.hEvent = readEvent;
        ResetEvent(readEvent);
        DWORD read = 0;
        if (!ReadFile(dev, buf, sizeof(buf), nullptr, &ov) &&
            GetLastError() != ERROR_IO_PENDING) {
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
            break;
        }
        HandleReport(buf, read);
    }
    CloseHandle(readEvent);
}

static DWORD WINAPI HidThreadProc(LPVOID) {
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
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
            }
        }
        // Retry every 3 seconds (receiver unplugged, etc.).
        WaitForSingleObject(g_stopEvent, 3000);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Tray icon
// ---------------------------------------------------------------------------

static HICON CreateNumberIcon(const wchar_t* text, COLORREF color) {
    UINT dpi = GetDpiForSystem();
    int size = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
    if (size <= 0) {
        size = 16;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP color32 =
        CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!mem || !color32 || !bits) {
        if (color32) {
            DeleteObject(color32);
        }
        if (mem) {
            DeleteDC(mem);
        }
        return nullptr;
    }

    HGDIOBJ oldBmp = SelectObject(mem, color32);
    memset(bits, 0, size * size * 4);

    size_t len = wcslen(text);
    LOGFONTW lf{};
    lf.lfHeight = -(len >= 3 ? size * 17 / 20 : size * 19 / 20);
    lf.lfWeight = FW_BOLD;
    lf.lfQuality = ANTIALIASED_QUALITY;  // grayscale, no ClearType
    // "100" doesn't fit with Segoe UI, use the condensed Bahnschrift variant.
    wcscpy(lf.lfFaceName, len >= 3 ? L"Bahnschrift Condensed" : L"Segoe UI");
    HFONT font = CreateFontIndirectW(&lf);
    HGDIOBJ oldFont = SelectObject(mem, font);

    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, RGB(255, 255, 255));
    RECT rc{0, 0, size, size};
    DrawTextW(mem, text, -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);
    GdiFlush();

    // The white-on-black text is used as an alpha mask, then painted with the
    // requested color using premultiplied alpha.
    BYTE cr = GetRValue(color), cg = GetGValue(color), cb = GetBValue(color);
    auto* px = (BYTE*)bits;
    for (int i = 0; i < size * size; i++, px += 4) {
        BYTE a = px[0] > px[1] ? (px[0] > px[2] ? px[0] : px[2])
                               : (px[1] > px[2] ? px[1] : px[2]);
        px[0] = (BYTE)(cb * a / 255);
        px[1] = (BYTE)(cg * a / 255);
        px[2] = (BYTE)(cr * a / 255);
        px[3] = a;
    }

    SelectObject(mem, oldFont);
    DeleteObject(font);
    SelectObject(mem, oldBmp);

    HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmColor = color32;
    ii.hbmMask = mask;
    HICON icon = CreateIconIndirect(&ii);

    DeleteObject(mask);
    DeleteObject(color32);
    DeleteDC(mem);
    return icon;
}

static void ShowBalloon(HWND hwnd, const wchar_t* title, const wchar_t* text) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = kTrayIconId;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_WARNING;
    wcsncpy(nid.szInfoTitle, title, ARRAYSIZE(nid.szInfoTitle) - 1);
    wcsncpy(nid.szInfo, text, ARRAYSIZE(nid.szInfo) - 1);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static void UpdateTrayIcon(HWND hwnd, bool forceAdd) {
    BatteryState st;
    Settings set;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        st = g_state;
        set = g_settings;
    }
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

    HICON icon = CreateNumberIcon(text, color);

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
            ShowBalloon(hwnd, t.lowTitle, msg);
        }
        if (set.notifyFull && g_lastStatus == kStatusCharging &&
            st.status == kStatusFull) {
            ShowBalloon(hwnd, t.title, t.fullBody);
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
            if (wp) {
                g_notifiedLow = false;
            }
            UpdateTrayIcon(hwnd, false);
            return 0;
        case WM_TIMER:
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            UpdateTrayIcon(hwnd, false);
            return 0;
        case WM_APP_TRAY:
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kStaleTimerId);
            RemoveTrayIcon(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Only the explorer.exe process that hosts the shell (the taskbar) shows the
// icon. File Explorer windows may run in separate explorer.exe processes.
static bool WaitUntilShellProcess() {
    while (true) {
        HWND shell = GetShellWindow();
        if (shell) {
            DWORD pid = 0;
            GetWindowThreadProcessId(shell, &pid);
            return pid == GetCurrentProcessId();
        }
        if (WaitForSingleObject(g_stopEvent, 1000) != WAIT_TIMEOUT) {
            return false;
        }
    }
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    if (!WaitUntilShellProcess()) {
        Wh_Log(L"Not the shell process, the icon won't be shown");
        return 0;
    }

    g_hidThread = CreateThread(nullptr, 0, HidThreadProc, nullptr, 0, nullptr);

    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.lpszClassName = kWindowClass;
    RegisterClassExW(&wc);

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    HWND hwnd = CreateWindowExW(0, kWindowClass, L"Attack Shark X11 Battery", 0,
                                0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        UnregisterClassW(kWindowClass, inst);
        return 0;
    }
    ChangeWindowMessageFilterEx(hwnd, g_taskbarCreatedMsg, MSGFLT_ALLOW,
                                nullptr);
    g_hwnd = hwnd;
    NotifyUi();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_hwnd = nullptr;
    UnregisterClassW(kWindowClass, inst);
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk callbacks
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    return g_uiThread != nullptr;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    HWND hwnd = g_hwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_hidThread) {
        WaitForSingleObject(g_hidThread, 5000);
        CloseHandle(g_hidThread);
        g_hidThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    NotifyUi(1);
}
