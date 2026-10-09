// ==WindhawkMod==
// @id              win10-volume-brightness-osd
// @name            Windows 10 Style Volume & Brightness OSD
// @description     Replaces the Windows 11 volume/brightness OSD with the classic vertical Windows 10 flyout
// @version         1.0.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lgdi32 -luser32 -ldwmapi -ldxva2 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Style Volume & Brightness OSD

Shows the classic vertical Windows 10 flyout (dark panel, grey track, white
thumb, accent fill) for **volume** and **brightness**.

## Screenshots

### Volume (desktop)
![volumedesk](https://raw.githubusercontent.com/AdmXP8/assets/main/volosd8desk.png)

### Brightness (desktop)
![brightnessdesk](https://raw.githubusercontent.com/AdmXP8/assets/main/bgtosd8desk.png)

### Volume
![volume](https://raw.githubusercontent.com/AdmXP8/assets/main/volosd8.png)

### Brightness
![brightness](https://raw.githubusercontent.com/AdmXP8/assets/main/bgtosd8.png)

## Volume
* The volume keys (up / down / mute) are captured with `RegisterHotKey`, so the
  flyout also appears when the volume is already 0 or 100.
* A low-level keyboard hook is used as a fallback signal. It makes the flyout
  appear (and the native OSD be suppressed) when a key does not reach the
  hotkey.
* Volume changes from any other source (WASAPI) also show the flyout.
* When muted, or at volume 0, a cross icon replaces the number.

## Brightness
* **Laptops:** the mod listens to the system brightness notification. The
  Fn brightness keys keep working normally.
* **Desktop PCs / external monitors:** enable "External monitor hotkeys". The
  mod then uses DDC/CI to change the brightness of the monitor under the mouse
  cursor with **Ctrl+Alt+Up / Ctrl+Alt+Down**.
* The hotkeys are registered system-wide while the option is on. Ctrl+Alt+Up /
  Down is also used by some applications (for example "add cursor above/below"
  in VS Code) and by the screen rotation hotkey of some Intel graphics drivers,
  so leave the option off if it conflicts. The flyout is shown on the monitor
  that is being adjusted.
* Brightness flyout trigger:
  * *On every brightness change* - shows the flyout whenever the system reports
    a new brightness (this includes automatic changes such as the ambient light
    sensor or an AC/battery switch). When the brightness is already at its
    minimum or maximum no change is reported, so the native OSD is used as a
    trigger instead; this needs "Suppress the native OSD" to be on.
  * *Only when the native OSD appears* - shows the flyout only when Windows
    would have shown its own OSD. Works at the minimum/maximum and ignores
    automatic changes. Depends on the native OSD detection described below.

## Mouse
The flyout can be dragged with the mouse to change the volume, or the
brightness of an external monitor (DDC/CI).

## Native OSD suppression
The native OSD is hidden with ShowWindow/SetWindowPos hooks in explorer.exe.
Every small `XamlExplorerHostIslandWindow` is treated as the native OSD, with
one exception: the virtual desktop switcher shown when hovering the Task View
button is recognised (band 7 and the thread description `MultitaskingView`, as
in the taskbar-thumbnails mod) and left alone.

Other small popups of that class, and hardware indicators that share the native
flyout (for example airplane mode), can still be hidden as well. On a PC that
reports a brightness value, the brightness flyout then appears in their place,
because a native OSD without a recent volume event is assumed to be a
brightness change.

Enable logging in Windhawk to see the class, title, thread description, band
and size of the windows that are treated as the OSD.

## Known issues
* The OSD cannot appear over fullscreen UWP apps.

## Credits
Special thanks to babamohammed2022 for the base version of the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeoutMs: 1500
  $name: Display duration (ms)
- suppressNative: true
  $name: Suppress the native Windows 11 OSD
- captureVolumeKeys: true
  $name: Capture the volume keys
  $description: Shows the flyout even when the volume is already at 0 or 100
- brightnessEnabled: true
  $name: Enable the brightness flyout
- brightnessTrigger: change
  $name: Brightness flyout trigger
  $options:
  - change: On every brightness change (also at min/max if the native OSD is suppressed)
  - nativeOsd: Only when the native Windows OSD appears
- externalHotkeys: false
  $name: External monitor hotkeys (Ctrl+Alt+Up / Down, DDC/CI)
- externalStep: 10
  $name: External monitor step (%)
- useSystemAccent: false
  $name: Use the system accent colour for the bar
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#include <atomic>
#include <vector>

// Settings
struct {
    int  timeoutMs;
    std::atomic<bool> suppress;        // read from the hooks of other threads
    bool volKeys;
    std::atomic<bool> brightness;
    std::atomic<bool> nativeTrigger;
    bool extKeys;
    int  extStep;
    bool useAccent;
} g_cfg;

void LoadSettings() {
    g_cfg.timeoutMs = Wh_GetIntSetting(L"timeoutMs");
    if (g_cfg.timeoutMs < 300) g_cfg.timeoutMs = 300;
    g_cfg.suppress   = Wh_GetIntSetting(L"suppressNative") != 0;
    g_cfg.volKeys    = Wh_GetIntSetting(L"captureVolumeKeys") != 0;
    g_cfg.brightness = Wh_GetIntSetting(L"brightnessEnabled") != 0;
    g_cfg.extKeys    = Wh_GetIntSetting(L"externalHotkeys") != 0;
    g_cfg.extStep    = Wh_GetIntSetting(L"externalStep");
    if (g_cfg.extStep < 1)  g_cfg.extStep = 1;
    if (g_cfg.extStep > 50) g_cfg.extStep = 50;
    g_cfg.useAccent  = Wh_GetIntSetting(L"useSystemAccent") != 0;

    PCWSTR trig = Wh_GetStringSetting(L"brightnessTrigger");
    g_cfg.nativeTrigger = wcscmp(trig, L"nativeOsd") == 0;
    Wh_FreeStringSetting(trig);
}

// Global state
constexpr UINT WM_VOL    = WM_APP + 1;  // wParam = level 0-100, lParam = muted
constexpr UINT WM_REBIND = WM_APP + 2;  // default audio device changed
constexpr UINT WM_NATIVE = WM_APP + 3;  // native OSD was detected
constexpr UINT WM_RELOAD = WM_APP + 4;  // settings changed
constexpr UINT WM_VOLKEY = WM_APP + 5;  // a volume key was pressed (keyboard hook)
constexpr UINT WM_EXT_READ = WM_APP + 6; // the DDC/CI worker finished reading a monitor

constexpr UINT_PTR TIMER_HIDE  = 1;
constexpr UINT_PTR TIMER_FADE  = 2;
constexpr UINT_PTR TIMER_APPLY = 3;
constexpr UINT_PTR TIMER_VOLKEY = 4;

enum { HK_VOL_UP = 1, HK_VOL_DOWN, HK_VOL_MUTE, HK_BR_UP, HK_BR_DOWN };
enum Mode { MODE_VOLUME, MODE_BRIGHTNESS };

std::atomic<HWND>      g_osd{nullptr};
std::atomic<ULONGLONG> g_lastVolTick{0};   // volume events
std::atomic<ULONGLONG> g_lastBrTick{0};    // brightness events
std::atomic<ULONGLONG> g_lastNativePost{0};
HANDLE g_thread = nullptr;
HANDLE g_ready  = nullptr;
DWORD  g_threadId = 0;

Mode g_mode  = MODE_VOLUME;
int  g_level = 0;
bool g_muted = false;
bool g_dragging = false;
ULONGLONG g_lastHotkeyTick = 0;   // last time RegisterHotKey handled a volume key
int  g_dpi   = 96;
int  g_alpha = 255;

// laptop brightness (power notification)
std::atomic<int> g_brightness{-1};
ULONGLONG g_brArmed    = 0;
HPOWERNOTIFY g_pn1 = nullptr, g_pn2 = nullptr;

// external monitor (DDC/CI)
HMONITOR  g_extMon  = nullptr;
DWORD     g_extMin = 0, g_extMax = 0;
int       g_extPct = 0;
ULONGLONG g_extLast = 0;

static int S(int v) { return MulDiv(v, g_dpi, 96); }

// Measurements at 96 DPI, taken from the reference screenshots
constexpr int BASE_W = 65, BASE_H = 140;
constexpr int BAR_W = 11, BAR_TOP = 20, BAR_H = 79, THUMB_H = 11;
constexpr int TEXT_BOTTOM = 133;
constexpr int BASE_MARGIN = 48;

static const COLORREF CLR_BG       = RGB(21, 26, 28);
static const COLORREF CLR_TRACK    = RGB(106, 106, 106);
static const COLORREF CLR_VOLUME   = RGB(0, 122, 213);

// GUID_VIDEO_CURRENT_MONITOR_BRIGHTNESS
static const GUID kBrGuid1 = {0x8ffee2c6, 0x2d01, 0x46be,
    {0xad, 0xb9, 0x39, 0x8a, 0xdd, 0xc5, 0xb4, 0xff}};
// GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS
static const GUID kBrGuid2 = {0xaded5e82, 0xb909, 0x4619,
    {0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb}};

// Native OSD detection / suppression
using GetWindowBand_t = BOOL(WINAPI*)(HWND, PDWORD);
using GetThreadDescription_t = HRESULT(WINAPI*)(HANDLE, PWSTR*);

// Band of a window (0 if it cannot be queried).
static DWORD GetBandOf(HWND hwnd) {
    static GetWindowBand_t fn = (GetWindowBand_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetWindowBand");
    DWORD band = 0;
    if (!fn || !fn(hwnd, &band)) return 0;
    return band;
}

// Description of the thread that owns the window (empty if unavailable).
static void GetThreadDescriptionOf(HWND hwnd, WCHAR* out, int cch) {
    out[0] = 0;
    static GetThreadDescription_t fn = (GetThreadDescription_t)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"), "GetThreadDescription");
    if (!fn) return;

    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid) return;
    HANDLE ht = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!ht) return;

    PWSTR d = nullptr;
    if (SUCCEEDED(fn(ht, &d)) && d) {
        lstrcpynW(out, d, cch);
        LocalFree(d);
    }
    CloseHandle(ht);
}

// Detects the virtual desktop switcher flyout shown when hovering the Task View
// button. Same criteria as taskbar-thumbnails: band ZBID_IMMERSIVE_EDGY (7) and
// the thread description "MultitaskingView".
static bool IsVirtualDesktopSwitcherHoverWindow(HWND hwnd) {
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid != GetCurrentProcessId())
        return false;

    constexpr DWORD ZBID_IMMERSIVE_EDGY = 7;
    if (GetBandOf(hwnd) != ZBID_IMMERSIVE_EDGY) return false;

    WCHAR desc[64];
    GetThreadDescriptionOf(hwnd, desc, ARRAYSIZE(desc));
    return wcscmp(desc, L"MultitaskingView") == 0;
}

// Logs everything that can help to identify the native OSD window precisely.
static void LogCandidate(HWND hwnd, PCWSTR cls, int w, int h, bool recent) {
    // InternalGetWindowText reads the title without sending a message to the
    // window, so it cannot stall the caller of ShowWindow/SetWindowPos.
    using InternalGetWindowText_t = int(WINAPI*)(HWND, LPWSTR, int);
    static InternalGetWindowText_t pInternalGetWindowText =
        (InternalGetWindowText_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "InternalGetWindowText");
    WCHAR title[128] = L"";
    if (pInternalGetWindowText) pInternalGetWindowText(hwnd, title, ARRAYSIZE(title));

    WCHAR desc[128];
    GetThreadDescriptionOf(hwnd, desc, ARRAYSIZE(desc));

    Wh_Log(L"OSD candidate: hwnd=%p class=%s title=\"%s\" thread=\"%s\" band=%u size=%dx%d recentEvent=%d",
           hwnd, cls, title, desc, GetBandOf(hwnd), w, h, recent ? 1 : 0);
}

// w/h = -1 -> use the current window size
bool IsNativeOsd(HWND hwnd, int w, int h, bool recent) {
    if (!hwnd || hwnd == g_osd.load()) return false;

    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, 64)) return false;
    if (wcscmp(cls, L"XamlExplorerHostIslandWindow") != 0) return false;

    if (w < 0 || h < 0) {
        RECT rc;
        if (!GetWindowRect(hwnd, &rc)) return false;
        w = rc.right - rc.left;
        h = rc.bottom - rc.top;
    }

    // The native OSD is a small window.
    bool match = w > 0 && h > 0 && w < 700 && h < 250;

    // The virtual desktop switcher (hover over Task View) is a small window of
    // the same class, but it is not an OSD: leave it alone, and never replace
    // it with the brightness flyout.
    if (match && IsVirtualDesktopSwitcherHoverWindow(hwnd)) {
        Wh_Log(L"Not an OSD: virtual desktop switcher, hwnd=%p", hwnd);
        return false;
    }

    // Only the windows that are really treated as the OSD are logged, so the
    // (relatively expensive) logging never runs for unrelated windows.
    if (match) LogCandidate(hwnd, cls, w, h, recent);
    return match;
}

// Returns true when the native OSD must be blocked.
bool NativeOsdSeen(HWND hwnd, int w, int h) {
    // A volume or brightness event counts as "recent" for 2.5 s: the event and
    // the native OSD window can appear in either order.
    bool trigger = g_cfg.brightness && g_cfg.nativeTrigger;
    if (!g_cfg.suppress && !trigger) return false;

    ULONGLONG now = GetTickCount64();
    bool volRecent = now - g_lastVolTick.load() < 2500;
    bool brRecent  = now - g_lastBrTick.load() < 2500;
    bool anyRecent = volRecent || brRecent;

    // When suppression is enabled, block every matching native OSD. This is
    // more reliable than depending on the order of the volume/brightness
    // notification and the XAML window creation.
    if (!IsNativeOsd(hwnd, w, h, anyRecent)) return false;

    // Native OSD without a recent volume event -> assume brightness
    if (g_cfg.brightness && !volRecent && g_brightness >= 0 &&
        now - g_lastNativePost.load() > 250) {
        g_lastNativePost = now;
        HWND osd = g_osd.load();
        if (osd) PostMessageW(osd, WM_NATIVE, 0, 0);
    }
    return g_cfg.suppress;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Orig;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE &&
        NativeOsdSeen(hWnd, -1, -1)) {
        return FALSE;
    }
    return ShowWindow_Orig(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Orig;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    if (uFlags & SWP_SHOWWINDOW) {
        int w = (uFlags & SWP_NOSIZE) ? -1 : cx;
        int h = (uFlags & SWP_NOSIZE) ? -1 : cy;
        if (NativeOsdSeen(hWnd, w, h))
            uFlags = (uFlags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW;
    }
    return SetWindowPos_Orig(hWnd, hInsertAfter, X, Y, cx, cy, uFlags);
}

// WASAPI: volume callback + default device notification
class VolCallback : public IAudioEndpointVolumeCallback {
    LONG m_ref = 1;
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = static_cast<IAudioEndpointVolumeCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA d) override {
        g_lastVolTick = GetTickCount64();
        HWND osd = g_osd.load();
        if (osd && d) {
            PostMessageW(osd, WM_VOL,
                         (WPARAM)(d->fMasterVolume * 100.0f + 0.5f),
                         d->bMuted ? 1 : 0);
        }
        return S_OK;
    }
};

class DeviceNotify : public IMMNotificationClient {
    LONG m_ref = 1;
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    STDMETHODIMP OnDeviceAdded(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) {
            HWND osd = g_osd.load();
            if (osd) PostMessageW(osd, WM_REBIND, 0, 0);
        }
        return S_OK;
    }
};

// Objects used only by the UI thread
IMMDeviceEnumerator*  g_en  = nullptr;
IMMDevice*            g_dev = nullptr;
IAudioEndpointVolume* g_ep  = nullptr;
VolCallback*          g_volCb = nullptr;
DeviceNotify*         g_devCb = nullptr;

void UnbindEndpoint() {
    if (g_ep) {
        g_ep->UnregisterControlChangeNotify(g_volCb);
        g_ep->Release();
        g_ep = nullptr;
    }
    if (g_dev) { g_dev->Release(); g_dev = nullptr; }
}

void BindEndpoint() {
    UnbindEndpoint();
    if (!g_en) return;
    if (FAILED(g_en->GetDefaultAudioEndpoint(eRender, eMultimedia, &g_dev))) {
        Wh_Log(L"No default render device");
        return;
    }
    if (FAILED(g_dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                               nullptr, (void**)&g_ep))) {
        Wh_Log(L"Activate(IAudioEndpointVolume) failed");
        g_dev->Release();
        g_dev = nullptr;
        return;
    }
    g_ep->RegisterControlChangeNotify(g_volCb);
}

// External monitors (DDC/CI)
template <class F>
bool ForPhysical(HMONITOR mon, F fn) {
    DWORD n = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(mon, &n) || !n) return false;
    std::vector<PHYSICAL_MONITOR> pm(n);
    if (!GetPhysicalMonitorsFromHMONITOR(mon, n, pm.data())) return false;
    bool ok = false;
    for (DWORD i = 0; i < n && !ok; i++) ok = fn(pm[i].hPhysicalMonitor);
    DestroyPhysicalMonitors(n, pm.data());
    return ok;
}

// DDC/CI calls can take hundreds of milliseconds, so they never run on the UI
// thread: the UI thread posts a request, the worker thread executes it and
// posts WM_EXT_READ back when a monitor was read.
struct DdcState {
    SRWLOCK   lock = SRWLOCK_INIT;
    HANDLE    ev = nullptr;          // auto-reset: "there is work"
    HANDLE    thread = nullptr;
    bool      quit = false;
    bool      doRead = false;  HMONITOR readMon = nullptr;
    bool      doSet = false;   HMONITOR setMon = nullptr;  DWORD setValue = 0;
    bool      resOk = false;   HMONITOR resMon = nullptr;
    DWORD     resMin = 0, resCur = 0, resMax = 0;
};
DdcState g_ddc;

DWORD WINAPI DdcThread(LPVOID) {
    for (;;) {
        if (WaitForSingleObject(g_ddc.ev, INFINITE) != WAIT_OBJECT_0) break;

        AcquireSRWLockExclusive(&g_ddc.lock);
        bool quit = g_ddc.quit;
        bool doRead = g_ddc.doRead;  HMONITOR readMon = g_ddc.readMon;
        bool doSet = g_ddc.doSet;    HMONITOR setMon = g_ddc.setMon;
        DWORD setValue = g_ddc.setValue;
        g_ddc.doRead = false;
        g_ddc.doSet = false;
        ReleaseSRWLockExclusive(&g_ddc.lock);

        if (quit) break;

        if (doSet) {
            ForPhysical(setMon, [&](HANDLE hp) {
                return SetMonitorBrightness(hp, setValue) != 0;
            });
        }
        if (doRead) {
            DWORD mn = 0, cur = 0, mx = 0;
            bool ok = ForPhysical(readMon, [&](HANDLE hp) {
                return GetMonitorBrightness(hp, &mn, &cur, &mx) && mx > mn;
            });
            AcquireSRWLockExclusive(&g_ddc.lock);
            g_ddc.resOk = ok;  g_ddc.resMon = readMon;
            g_ddc.resMin = mn; g_ddc.resCur = cur; g_ddc.resMax = mx;
            ReleaseSRWLockExclusive(&g_ddc.lock);
            HWND osd = g_osd.load();
            if (osd) PostMessageW(osd, WM_EXT_READ, 0, 0);
        }
    }
    return 0;
}

void DdcStart() {
    g_ddc.quit = false;
    g_ddc.ev = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_ddc.ev)
        g_ddc.thread = CreateThread(nullptr, 0, DdcThread, nullptr, 0, nullptr);
}

void DdcStop() {
    if (g_ddc.thread) {
        AcquireSRWLockExclusive(&g_ddc.lock);
        g_ddc.quit = true;
        ReleaseSRWLockExclusive(&g_ddc.lock);
        SetEvent(g_ddc.ev);
        WaitForSingleObject(g_ddc.thread, INFINITE);   // joined before unload
        CloseHandle(g_ddc.thread);
        g_ddc.thread = nullptr;
    }
    if (g_ddc.ev) { CloseHandle(g_ddc.ev); g_ddc.ev = nullptr; }
}

static void DdcRequestRead(HMONITOR mon) {
    AcquireSRWLockExclusive(&g_ddc.lock);
    g_ddc.readMon = mon;
    g_ddc.doRead = true;
    ReleaseSRWLockExclusive(&g_ddc.lock);
    SetEvent(g_ddc.ev);
}

// UI-thread state of the external monitor
bool      g_extReadBusy = false;
int       g_extPendingDelta = 0;
HMONITOR  g_extPendingMon = nullptr;
HMONITOR  g_extFailMon = nullptr;     // monitor without DDC/CI support (cached)
ULONGLONG g_extFailTick = 0;

// Asks the worker for the current brightness of `mon` (one request at a time;
// monitors without DDC/CI support are not asked again for a few seconds).
static void ExtRequestRead(HMONITOR mon) {
    if (g_extReadBusy) return;
    if (mon == g_extFailMon && GetTickCount64() - g_extFailTick < 5000) return;
    g_extReadBusy = true;
    DdcRequestRead(mon);
}

void ExtApply() {
    if (!g_extMon || g_extMax <= g_extMin) return;
    DWORD v = g_extMin + (g_extMax - g_extMin) * g_extPct / 100;
    AcquireSRWLockExclusive(&g_ddc.lock);
    g_ddc.setMon = g_extMon;
    g_ddc.setValue = v;
    g_ddc.doSet = true;
    ReleaseSRWLockExclusive(&g_ddc.lock);
    SetEvent(g_ddc.ev);
}

// OSD window
COLORREF AccentColor() {
    if (g_cfg.useAccent) {
        DWORD c = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&c, &opaque)))
            return RGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
    }
    return CLR_VOLUME;
}

static void DrawGlyph(HDC dc, RECT* r, PCWSTR glyph, int size) {
    HFONT icon = CreateFontW(-S(size), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                             0, 0, CLEARTYPE_QUALITY, 0, L"Segoe MDL2 Assets");
    HGDIOBJ old = SelectObject(dc, icon);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    DrawTextW(dc, glyph, -1, r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, old);
    DeleteObject(icon);
}

void Paint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC wdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);

    // double buffer to avoid flicker
    HDC dc = CreateCompatibleDC(wdc);
    HBITMAP bmp = CreateCompatibleBitmap(wdc, rc.right, rc.bottom);
    HGDIOBJ oldBmp = SelectObject(dc, bmp);

    HBRUSH bg = CreateSolidBrush(CLR_BG);
    FillRect(dc, &rc, bg);
    DeleteObject(bg);

    // the whole track is grey
    int bx = (rc.right - S(BAR_W)) / 2;
    RECT track = { bx, S(BAR_TOP), bx + S(BAR_W), S(BAR_TOP) + S(BAR_H) };
    HBRUSH tb = CreateSolidBrush(CLR_TRACK);
    FillRect(dc, &track, tb);
    DeleteObject(tb);

    // white thumb + fill below it
    bool bright = g_mode == MODE_BRIGHTNESS;
    int level = g_level;
    if (level < 0) level = 0;
    if (level > 100) level = 100;

    int thumbH = S(THUMB_H);
    int travel = (track.bottom - track.top) - thumbH;
    int thumbTop = track.top + travel * (100 - level) / 100;

    RECT fill = { track.left, thumbTop + thumbH, track.right, track.bottom };
    if (fill.bottom > fill.top) {
        // Use the same accent for both volume and brightness so the two
        // flyouts have a consistent appearance.
        HBRUSH fb = CreateSolidBrush(AccentColor());
        FillRect(dc, &fill, fb);
        DeleteObject(fb);
    }

    RECT thumb = { track.left, thumbTop, track.right, thumbTop + thumbH };
    HBRUSH wb = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(dc, &thumb, wb);
    DeleteObject(wb);

    // number, mute cross or brightness icon below the bar
    RECT nr = { 0, track.bottom, rc.right, S(TEXT_BOTTOM) };
    if (bright) {
        DrawGlyph(dc, &nr, L"\uE706", 16);          // Brightness
    } else if (g_muted || g_level == 0) {
        DrawGlyph(dc, &nr, L"\uE711", 16);          // Cancel (cross)
    } else {
        HFONT f = CreateFontW(-S(13), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                              0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        HGDIOBJ oldF = SelectObject(dc, f);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        WCHAR t[8];
        wsprintfW(t, L"%d", g_level);
        DrawTextW(dc, t, -1, &nr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(dc, oldF);
        DeleteObject(f);
    }

    BitBlt(wdc, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(hwnd, &ps);
}

// `target` = monitor to show the flyout on (primary monitor when null).
void ShowOsd(HWND h, HMONITOR target = nullptr) {
    KillTimer(h, TIMER_FADE);
    g_alpha = 255;
    SetLayeredWindowAttributes(h, 0, 255, LWA_ALPHA);

    HMONITOR mon = target ? target
                          : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfoW(mon, &mi);

    // Explorer is per-monitor DPI aware: size the flyout for the target monitor
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX)
        g_dpi = (int)dpiX;

    // top left, like the Windows 10 OSD
    SetWindowPos(h, HWND_TOPMOST,
                 mi.rcWork.left + S(BASE_MARGIN),
                 mi.rcWork.top + S(BASE_MARGIN),
                 S(BASE_W), S(BASE_H),
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(h, nullptr, FALSE);

    // While the thumb is being dragged the flyout must stay; the timer is
    // restarted when the drag ends.
    if (g_dragging) KillTimer(h, TIMER_HIDE);
    else            SetTimer(h, TIMER_HIDE, g_cfg.timeoutMs, nullptr);
}

void ShowBrightness(HWND h, int level, HMONITOR target = nullptr) {
    g_mode  = MODE_BRIGHTNESS;
    g_level = level;
    g_muted = false;
    ShowOsd(h, target);
}

// Hotkeys
void UnregisterKeys(HWND h) {
    for (int id = HK_VOL_UP; id <= HK_BR_DOWN; id++) UnregisterHotKey(h, id);
}

static bool RegisterOne(HWND h, int id, UINT mods, UINT vk, PCWSTR name) {
    if (RegisterHotKey(h, id, mods, vk)) return true;
    Wh_Log(L"Hotkey %s could not be registered (another app owns it?): %u",
           name, GetLastError());
    return false;
}

void RegisterKeys(HWND h) {
    UnregisterKeys(h);
    if (g_cfg.volKeys) {
        bool a = RegisterOne(h, HK_VOL_UP,   0, VK_VOLUME_UP,   L"Volume up");
        bool b = RegisterOne(h, HK_VOL_DOWN, 0, VK_VOLUME_DOWN, L"Volume down");
        bool c = RegisterOne(h, HK_VOL_MUTE, 0, VK_VOLUME_MUTE, L"Volume mute");
        if (a && b && c) Wh_Log(L"Volume hotkeys registered");
    }
    if (g_cfg.extKeys) {
        RegisterOne(h, HK_BR_UP,   MOD_CONTROL | MOD_ALT, VK_UP,   L"Ctrl+Alt+Up");
        RegisterOne(h, HK_BR_DOWN, MOD_CONTROL | MOD_ALT, VK_DOWN, L"Ctrl+Alt+Down");
    }
}

void VolumeKey(HWND h, int id) {
    g_lastVolTick = GetTickCount64();
    g_lastHotkeyTick = g_lastVolTick;
    Wh_Log(L"Volume hotkey fired: id=%d", id);
    if (g_ep) {
        float v = 0; BOOL m = FALSE;
        g_ep->GetMasterVolumeLevelScalar(&v);
        g_ep->GetMute(&m);

        if (id == HK_VOL_MUTE) {
            m = !m;
            g_ep->SetMute(m, nullptr);
        } else {
            v += (id == HK_VOL_UP) ? 0.02f : -0.02f;   // 2% steps like Windows
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;
            g_ep->SetMasterVolumeLevelScalar(v, nullptr);
            if (m && id == HK_VOL_UP) { m = FALSE; g_ep->SetMute(FALSE, nullptr); }
        }
        g_level = (int)(v * 100.0f + 0.5f);
        g_muted = m != 0;
    }
    g_mode = MODE_VOLUME;
    ShowOsd(h);
}

// Shows the current volume without changing it (used when the key was pressed
// but not handled by the hotkey path).
void ShowVolumeState(HWND h) {
    if (g_ep) {
        float v = 0; BOOL m = FALSE;
        if (SUCCEEDED(g_ep->GetMasterVolumeLevelScalar(&v))) {
            g_level = (int)(v * 100.0f + 0.5f);
            g_ep->GetMute(&m);
            g_muted = m != 0;
        }
    }
    g_mode = MODE_VOLUME;
    ShowOsd(h);
}

// Applies a relative change to the cached external brightness and shows it.
static void ExtStep(HWND h, int delta) {
    g_extPct += delta;
    if (g_extPct < 0)   g_extPct = 0;
    if (g_extPct > 100) g_extPct = 100;
    g_extLast = GetTickCount64();
    g_lastBrTick = g_extLast;

    ShowBrightness(h, g_extPct, g_extMon);   // on the monitor being adjusted
    SetTimer(h, TIMER_APPLY, 60, nullptr);   // coalesce key repeats
}

void ExternalBrightnessKey(HWND h, int dir) {
    ULONGLONG now = GetTickCount64();
    POINT pt; GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    int delta = dir * g_cfg.extStep;

    if (mon == g_extMon && g_extMax > g_extMin && now - g_extLast <= 5000) {
        ExtStep(h, delta);
        return;
    }

    // The current value must be read first. Remember the key presses; they are
    // applied when the worker thread reports the value (WM_EXT_READ).
    if (g_extPendingMon != mon) g_extPendingDelta = 0;
    g_extPendingMon = mon;
    g_extPendingDelta += delta;
    ExtRequestRead(mon);
}

// Mouse dragging
void SetLevelFromMouse(HWND h, int y) {
    RECT rc;
    GetClientRect(h, &rc);
    int top = S(BAR_TOP);
    int bottom = top + S(BAR_H);
    int thumb = S(THUMB_H);
    int travel = (bottom - top) - thumb;
    if (travel <= 0) return;

    int level = 100 - MulDiv(y - top - thumb / 2, 100, travel);
    if (level < 0) level = 0;
    if (level > 100) level = 100;

    // keep the flyout visible while it is being dragged (even if the mouse is
    // held still): no hide timer runs until the drag ends
    KillTimer(h, TIMER_HIDE);
    KillTimer(h, TIMER_FADE);
    g_alpha = 255;
    SetLayeredWindowAttributes(h, 0, 255, LWA_ALPHA);

    if (g_mode == MODE_VOLUME) {
        if (!g_ep) return;
        g_ep->SetMasterVolumeLevelScalar(level / 100.0f, nullptr);
        g_ep->SetMute(FALSE, nullptr);
        g_level = level;
        g_muted = false;
    } else {
        // External monitors use DDC/CI. Laptop brightness notifications are
        // read-only here, so the existing native Fn keys remain available.
        POINT pt;
        GetCursorPos(&pt);
        HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        if (mon != g_extMon || g_extMax <= g_extMin) {
            // read the monitor first (worker thread); the next mouse move applies
            g_extPendingMon = mon;
            g_extPendingDelta = 0;
            ExtRequestRead(mon);
            return;
        }
        g_extPct = level;
        g_extLast = GetTickCount64();
        g_level = level;
        SetTimer(h, TIMER_APPLY, 60, nullptr);
    }
    InvalidateRect(h, nullptr, FALSE);
}

// Window procedure
LRESULT CALLBACK OsdProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_VOL:
        g_mode  = MODE_VOLUME;
        g_level = (int)w;
        g_muted = l != 0;
        ShowOsd(h);
        return 0;

    case WM_HOTKEY:
        if (w >= HK_VOL_UP && w <= HK_VOL_MUTE)      VolumeKey(h, (int)w);
        else if (w == HK_BR_UP)                      ExternalBrightnessKey(h, +1);
        else if (w == HK_BR_DOWN)                    ExternalBrightnessKey(h, -1);
        return 0;

    case WM_POWERBROADCAST:
        if (w == PBT_POWERSETTINGCHANGE && l && g_cfg.brightness) {
            auto* p = (POWERBROADCAST_SETTING*)l;
            if ((IsEqualGUID(p->PowerSetting, kBrGuid1) ||
                 IsEqualGUID(p->PowerSetting, kBrGuid2)) &&
                p->DataLength >= sizeof(DWORD)) {
                DWORD raw = 0;
                memcpy(&raw, p->Data, sizeof(raw));
                int v = raw > 100 ? 100 : (int)raw;

                bool changed = v != g_brightness;
                g_brightness = v;
                g_lastBrTick = GetTickCount64();

                if (GetTickCount64() >= g_brArmed) {
                    if (!g_cfg.nativeTrigger) {
                        if (changed) ShowBrightness(h, v);
                    } else if (IsWindowVisible(h) && g_mode == MODE_BRIGHTNESS) {
                        g_level = v;                       // refresh a flyout already open
                        InvalidateRect(h, nullptr, FALSE);
                    }
                }
            }
        }
        return TRUE;

    case WM_NATIVE:
        // native OSD appeared with no volume event: show the brightness flyout
        if (g_cfg.brightness && g_brightness >= 0)
            ShowBrightness(h, g_brightness);
        return 0;

    case WM_RELOAD:
        LoadSettings();                     // on the thread that reads most fields
        RegisterKeys(h);
        InvalidateRect(h, nullptr, FALSE);
        return 0;

    case WM_EXT_READ: {
        bool ok; HMONITOR rmon; DWORD mn, cur, mx;
        AcquireSRWLockExclusive(&g_ddc.lock);
        ok = g_ddc.resOk;  rmon = g_ddc.resMon;
        mn = g_ddc.resMin; cur = g_ddc.resCur; mx = g_ddc.resMax;
        ReleaseSRWLockExclusive(&g_ddc.lock);
        g_extReadBusy = false;

        if (!ok || mx <= mn) {
            Wh_Log(L"DDC/CI not available on this monitor");
            g_extFailMon = rmon;
            g_extFailTick = GetTickCount64();
            g_extPendingDelta = 0;
            g_extPendingMon = nullptr;
            return 0;
        }
        g_extFailMon = nullptr;
        if (cur < mn) cur = mn;
        g_extMon = rmon; g_extMin = mn; g_extMax = mx;
        g_extPct = (int)((cur - mn) * 100 / (mx - mn));
        g_extLast = GetTickCount64();

        if (g_extPendingMon && g_extPendingMon != rmon) {
            ExtRequestRead(g_extPendingMon);       // the cursor moved to another monitor
        } else if (g_extPendingDelta != 0) {
            int d = g_extPendingDelta;
            g_extPendingDelta = 0;
            g_extPendingMon = nullptr;
            ExtStep(h, d);
        } else if (g_mode == MODE_BRIGHTNESS && IsWindowVisible(h)) {
            g_level = g_extPct;
            InvalidateRect(h, nullptr, FALSE);
        }
        return 0;
    }

    case WM_VOLKEY:
        // wait a moment: if the hotkey path handles the key it shows the flyout itself
        SetTimer(h, TIMER_VOLKEY, 40, nullptr);
        return 0;

    case WM_REBIND:
        BindEndpoint();
        return 0;

    case WM_TIMER:
        if (w == TIMER_HIDE) {
            KillTimer(h, TIMER_HIDE);
            SetTimer(h, TIMER_FADE, 16, nullptr);          // start the fade
        } else if (w == TIMER_FADE) {
            g_alpha -= 8;
            if (g_alpha <= 0) {
                KillTimer(h, TIMER_FADE);
                ShowWindow(h, SW_HIDE);
                g_alpha = 255;
            } else {
                SetLayeredWindowAttributes(h, 0, (BYTE)g_alpha, LWA_ALPHA);
            }
        } else if (w == TIMER_APPLY) {
            KillTimer(h, TIMER_APPLY);
            ExtApply();
        } else if (w == TIMER_VOLKEY) {
            KillTimer(h, TIMER_VOLKEY);
            if (GetTickCount64() - g_lastHotkeyTick > 250) {
                Wh_Log(L"Volume key not handled by the hotkey path: showing current level");
                ShowVolumeState(h);
            }
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        Paint(h);
        return 0;
    case WM_LBUTTONDOWN: {
        int x = (int)(short)LOWORD(l);
        int y = (int)(short)HIWORD(l);
        RECT rc;
        GetClientRect(h, &rc);
        int left = (rc.right - S(BAR_W)) / 2;
        int top = S(BAR_TOP);
        int bottom = top + S(BAR_H);
        int thumb = S(THUMB_H);
        int thumbTop = top + ((bottom - top) - thumb) * (100 - g_level) / 100;
        if (x >= left - S(4) && x <= left + S(BAR_W) + S(4) &&
            y >= thumbTop - S(4) && y <= thumbTop + thumb + S(4)) {
            g_dragging = true;
            SetCapture(h);
            SetLevelFromMouse(h, y);
        }
        return 0;
    }
    case WM_MOUSEMOVE:
        if (g_dragging) SetLevelFromMouse(h, (int)(short)HIWORD(l));
        return 0;
    case WM_LBUTTONUP:
        if (g_dragging) {
            g_dragging = false;
            ReleaseCapture();
            SetTimer(h, TIMER_HIDE, g_cfg.timeoutMs, nullptr);
        }
        return 0;
    case WM_CAPTURECHANGED:
        if (g_dragging) {                   // capture lost: end the drag
            g_dragging = false;
            SetTimer(h, TIMER_HIDE, g_cfg.timeoutMs, nullptr);
        }
        return 0;
    case WM_SETCURSOR:
        SetCursor(LoadCursorW(nullptr, IDC_ARROW));
        return TRUE;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_NCHITTEST:
        return HTCLIENT;
    }
    return DefWindowProcW(h, m, w, l);
}

// UI thread: window + COM + message loop
static HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)&GetCurrentModuleHandle, &module))
        return module;
    return nullptr;
}

// Low-level keyboard hook: used only as a "volume key pressed" signal. It sees
// the key even when another app owns the hotkey or the key bypasses
// RegisterHotKey, so the native OSD is suppressed and our flyout is shown also
// at 0% and 100%, where no volume change event exists.
//
// The hook lives on its own thread and only touches atomics and PostMessage, so
// it always returns immediately and can never delay typing, even while the UI
// thread is busy with audio (COM) calls.
LRESULT CALLBACK KbProc(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION && (w == WM_KEYDOWN || w == WM_SYSKEYDOWN)) {
        auto* k = (KBDLLHOOKSTRUCT*)l;
        if (k->vkCode == VK_VOLUME_UP || k->vkCode == VK_VOLUME_DOWN ||
            k->vkCode == VK_VOLUME_MUTE) {
            g_lastVolTick = GetTickCount64();
            HWND osd = g_osd.load();
            if (osd) PostMessageW(osd, WM_VOLKEY, 0, 0);
        }
    }
    return CallNextHookEx(nullptr, code, w, l);
}

HANDLE g_kbThread = nullptr;
DWORD  g_kbThreadId = 0;

DWORD WINAPI KbThread(LPVOID ready) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);   // create the queue
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, GetCurrentModuleHandle(), 0);
    if (!hook) Wh_Log(L"Keyboard hook failed: %u", GetLastError());
    SetEvent((HANDLE)ready);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (hook) UnhookWindowsHookEx(hook);
    return 0;
}

DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_dpi = GetDpiForSystem();

    // Register the class under the mod's own module, so it can never refer to
    // a window procedure of an unloaded copy of the mod.
    WNDCLASSW wc{};
    wc.lpfnWndProc = OsdProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"WhWin10VolumeBrightnessOsd";
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        SetEvent(g_ready);
        CoUninitialize();
        return 0;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
        WS_EX_LAYERED,
        wc.lpszClassName, L"", WS_POPUP,
        0, 0, S(BASE_W), S(BASE_H),
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        SetEvent(g_ready);
        CoUninitialize();
        return 0;
    }
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    g_osd = hwnd;

    // The thread has a message queue now: a WM_QUIT posted from here on cannot
    // be lost, so Wh_ModUninit can safely wait for this thread without a timeout.
    SetEvent(g_ready);

    RegisterKeys(hwnd);
    DdcStart();

    HANDLE kbReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_kbThread = CreateThread(nullptr, 0, KbThread, kbReady, 0, &g_kbThreadId);
    if (g_kbThread) WaitForSingleObject(kbReady, INFINITE);
    CloseHandle(kbReady);

    // The initial notification sent on registration must not show the flyout
    g_brArmed = GetTickCount64() + 1500;
    g_pn1 = RegisterPowerSettingNotification(hwnd, &kBrGuid1, DEVICE_NOTIFY_WINDOW_HANDLE);
    g_pn2 = RegisterPowerSettingNotification(hwnd, &kBrGuid2, DEVICE_NOTIFY_WINDOW_HANDLE);

    g_volCb = new VolCallback();
    g_devCb = new DeviceNotify();
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                   CLSCTX_ALL, IID_PPV_ARGS(&g_en)))) {
        g_en->RegisterEndpointNotificationCallback(g_devCb);
        BindEndpoint();
    } else {
        Wh_Log(L"MMDeviceEnumerator creation failed");
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // cleanup
    g_osd = nullptr;
    if (g_kbThread) {
        PostThreadMessageW(g_kbThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_kbThread, INFINITE);
        CloseHandle(g_kbThread);
        g_kbThread = nullptr;
    }
    DdcStop();
    if (g_pn1) { UnregisterPowerSettingNotification(g_pn1); g_pn1 = nullptr; }
    if (g_pn2) { UnregisterPowerSettingNotification(g_pn2); g_pn2 = nullptr; }
    if (hwnd) UnregisterKeys(hwnd);
    UnbindEndpoint();
    if (g_en) {
        g_en->UnregisterEndpointNotificationCallback(g_devCb);
        g_en->Release();
        g_en = nullptr;
    }
    if (g_volCb) { g_volCb->Release(); g_volCb = nullptr; }
    if (g_devCb) { g_devCb->Release(); g_devCb = nullptr; }
    if (hwnd) DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
    return 0;
}

// Windhawk entry points
static bool ContainsNoCase(PCWSTR s, PCWSTR needle) {
    size_t n = wcslen(needle);
    for (; *s; s++) {
        if (_wcsnicmp(s, needle, n) == 0) return true;
    }
    return false;
}

BOOL Wh_ModInit() {
    // Secondary explorer.exe processes (e.g. "Launch folder windows in a
    // separate process": explorer.exe /factory,{...} -Embedding, or
    // explorer.exe /separate) must not get their own OSD, audio callbacks and
    // hooks.
    PCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && (ContainsNoCase(cmdLine, L"/factory") ||
                    ContainsNoCase(cmdLine, L"-Embedding") ||
                    ContainsNoCase(cmdLine, L"/separate"))) {
        return FALSE;
    }

    LoadSettings();

    // The hooks are only applied after Wh_ModInit returns, so the pointers used
    // by the hook functions are valid before the UI thread even exists.
    if (!Wh_SetFunctionHook((void*)ShowWindow,
                            (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig) ||
        !Wh_SetFunctionHook((void*)SetWindowPos,
                            (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Orig)) {
        Wh_Log(L"Failed to hook ShowWindow/SetWindowPos");
        return FALSE;
    }

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_ready);
        g_ready = nullptr;
        return FALSE;
    }
    WaitForSingleObject(g_ready, INFINITE);

    if (!g_osd.load()) {            // the UI thread failed to create its window
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        CloseHandle(g_ready);
        g_ready = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    // Settings are reloaded on the UI thread (WM_RELOAD), which also has to
    // re-register the hotkeys.
    HWND osd = g_osd.load();
    if (osd) PostMessageW(osd, WM_RELOAD, 0, 0);
}

void Wh_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        // The thread must be fully joined before the mod is unloaded
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) { CloseHandle(g_ready); g_ready = nullptr; }
}
