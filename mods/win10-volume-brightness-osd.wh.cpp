// ==WindhawkMod==
// @id              win10-volume-brightness-osd
// @name            Windows 8/10 Style Volume & Brightness OSD
// @description     Replaces the Windows 11 volume/brightness OSD with the vertical Windows 10 flyout
// @version         1.2.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -lwbemuuid -lgdi32 -luser32 -ldwmapi -ldxva2 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8/10 Style Volume & Brightness OSD

Shows the vertical Windows 8/10 flyout for **volume** and **brightness** instead of the Windows 11 one.

## Windows 8/10 Theme

### Volume
![volume10](https://raw.githubusercontent.com/AdmXP8/assets/main/10vol.png)

### Brightness
![brightness10](https://raw.githubusercontent.com/AdmXP8/assets/main/10brg.png)

## Developer Preview Theme

### Volume
![volumeDP](https://raw.githubusercontent.com/AdmXP8/assets/main/dpvol.png)

### Brightness
![brightnessDP](https://raw.githubusercontent.com/AdmXP8/assets/main/dpbrg.png)

## Volume
* With "Capture the volume keys" (on by default) the volume keys (up / down /
  mute) are taken **system-wide** with `RegisterHotKey`, so other applications
  no longer receive them, but the flyout also appears when the volume is
  already 0 or 100. Turn the option off if it conflicts with another program.
* Optional (off by default, "Keyboard hook fallback"): a low-level keyboard
  hook, running on its own thread, is used as a fallback signal when a volume
  key does not reach the hotkey (for example when another app owns the
  hotkey).
* Volume changes from any other source (WASAPI) also show the flyout.
* When muted, or at volume 0, a cross icon replaces the number.

## Brightness
* **Laptops:** the mod listens to the system brightness notification. The
  Fn brightness keys keep working normally.
* **Desktop PCs / external monitors:** enable "External monitor hotkeys". The
  mod then uses DDC/CI to change the brightness of the monitor under the mouse
  cursor with **Ctrl+Alt+Up / Ctrl+Alt+Down**.
* These hotkeys are registered **system-wide** while the option is on.
  Ctrl+Alt+Up / Down is also used by some applications (for example "add
  cursor above/below" in VS Code) and by the screen rotation hotkey of some
  Intel graphics drivers, so leave the option off if it conflicts. The flyout
  is shown on the monitor that is being adjusted.
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
The flyout can be dragged with the mouse to change whatever it is showing:
the volume, the brightness of the internal (laptop) panel - set through WMI,
the same way as the Fn keys - or, after the Ctrl+Alt+Up/Down hotkeys were
used, the brightness of an external monitor (DDC/CI).

## Always on top
Enable **Always on top** to show the flyout above fullscreen UWP apps. The
flyout window is created in the same Z-band as the native Windows OSD. The mod
learns that band the first time the native OSD is detected (this needs
"Suppress the native OSD" or the native-OSD brightness trigger to be on) and
then recreates its window in that band automatically. If the band cannot be
used, the mod falls back to a UIAccess band and then to a normal topmost
window. Enable logging to see which band was used.

## Native OSD suppression
The native OSD is hidden with ShowWindow/SetWindowPos hooks in explorer.exe.
Small `XamlExplorerHostIslandWindow` windows are treated as candidates for the
native OSD. Explorer hosts other flyouts in the same class, so two checks keep
them apart:

* Windows hosted by the `MultitaskingView` UI thread, or by a snap-related
  thread, are never the indicator. This covers the virtual desktop switcher
  shown when hovering the Task View button, the Snap Layouts flyout shown
  when hovering a window's maximize/restore button (or pressing Win+Z), and
  the Snap Assist bar shown when dragging a window to the top of the screen.
* The first candidate that appears together with a real volume/brightness
  event must be the true indicator, so its thread description and Z-band are
  remembered as the indicator's signature. A candidate that appears without
  such an event - a brightness key pressed at the minimum/maximum, for one -
  is only accepted if it matches that signature.

Enable logging in Windhawk to see the class, title, thread description, band
and size of the windows that are treated as the OSD, and why other flyouts
were rejected.

## Known issues
* Other hardware indicators that share the native flyout (for example airplane
  mode) are hidden as well, and on a PC that reports a brightness value the
  brightness flyout then appears in their place, because a native OSD without
  a recent volume event is assumed to be a brightness change.
* The OSD signature is learned from the first volume/brightness event after
  Explorer starts. Until then only the thread name is used to recognise the
  Snap Layouts flyout and similar windows, and "Always on top" has not yet
  learned the Z-band of the native OSD.
* If the Z-band of the native OSD cannot be used, the flyout may not appear
  above fullscreen UWP apps.

## Credits
Special thanks to babamohammed2022 for the base version of the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: win10
  $name: Theme
  $options:
  - win10: Windows 10/8 
  - win8dp: Windows 8 Developer Preview 
- timeoutMs: 3500
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
- useSystemAccent: true
  $name: Use the system accent colour for the bar
- alwaysOnTop: true
  $name: Always on top
- keyboardHook: false
  $name: Keyboard hook fallback
  $description: Extra low-level keyboard hook used when a volume key does not reach the hotkey
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#include <wbemcli.h>
#include <atomic>
#include <vector>

//settings
struct {
    int  timeoutMs;
    std::atomic<bool> suppress;        
    bool volKeys;
    std::atomic<bool> brightness;
    std::atomic<bool> nativeTrigger;
    bool extKeys;
    int  extStep;
    bool useAccent;
    bool dp;                 //Windows 8 DP(8102) theme
    std::atomic<bool> alwaysOnTop;
    std::atomic<int>  osdBand;
    bool kbHook;
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
    g_cfg.alwaysOnTop = Wh_GetIntSetting(L"alwaysOnTop") != 0;
    g_cfg.kbHook = Wh_GetIntSetting(L"keyboardHook") != 0;

    PCWSTR trig = Wh_GetStringSetting(L"brightnessTrigger");
    g_cfg.nativeTrigger = wcscmp(trig, L"nativeOsd") == 0;
    Wh_FreeStringSetting(trig);

    PCWSTR theme = Wh_GetStringSetting(L"theme");
    g_cfg.dp = wcscmp(theme, L"win8dp") == 0;
    Wh_FreeStringSetting(theme);
}

constexpr UINT WM_VOL    = WM_APP + 1;  
constexpr UINT WM_REBIND = WM_APP + 2;  
constexpr UINT WM_NATIVE = WM_APP + 3;  
constexpr UINT WM_RELOAD = WM_APP + 4;  
constexpr UINT WM_VOLKEY = WM_APP + 5;  
constexpr UINT WM_EXT_READ = WM_APP + 6; 
constexpr UINT WM_BANDLEARNED = WM_APP + 7; 
constexpr UINT WM_RECREATE = WM_APP + 8;   

constexpr UINT_PTR TIMER_HIDE  = 1;
constexpr UINT_PTR TIMER_FADE  = 2;
constexpr UINT_PTR TIMER_APPLY = 3;
constexpr UINT_PTR TIMER_VOLKEY = 4;

enum { HK_VOL_UP = 1, HK_VOL_DOWN, HK_VOL_MUTE, HK_BR_UP, HK_BR_DOWN };
enum Mode { MODE_VOLUME, MODE_BRIGHTNESS };

std::atomic<HWND>      g_osd{nullptr};
std::atomic<ULONGLONG> g_lastVolTick{0};   //volume events
std::atomic<ULONGLONG> g_lastBrTick{0};    //brightness events
std::atomic<ULONGLONG> g_lastNativePost{0};
HANDLE g_thread = nullptr;
HANDLE g_ready  = nullptr;
DWORD  g_threadId = 0;

std::atomic<DWORD> g_nativeBand{0};   
DWORD g_osdBand = 0;                  
DWORD g_triedNativeBand = 0;          
WNDCLASSW g_wc{};

HANDLE g_kbThread = nullptr;
DWORD  g_kbThreadId = 0;

Mode g_mode  = MODE_VOLUME;
int  g_level = 0;
bool g_muted = false;
bool g_dragging = false;
HMONITOR g_brTarget = nullptr; 
bool g_applyPending = false;    //a TIMER_APPLY is armed and not yet handled
bool g_applyWmi = false;        
int  g_wmiPending = 0;          
ULONGLONG g_lastHotkeyTick = 0; 
int  g_dpi   = 96;
int  g_alpha = 255;

//laptop brightness
std::atomic<int> g_brightness{-1};
ULONGLONG g_brArmed    = 0;
HPOWERNOTIFY g_pn1 = nullptr, g_pn2 = nullptr;

//external monitor
HMONITOR  g_extMon  = nullptr;
DWORD     g_extMin = 0, g_extMax = 0;
int       g_extPct = 0;
ULONGLONG g_extLast = 0;

static int S(int v) { return MulDiv(v, g_dpi, 96); }

constexpr int BASE_W = 65, BASE_H = 140;
constexpr int BAR_W = 11, BAR_TOP = 20, BAR_H = 79, THUMB_H = 11;
constexpr int TEXT_BOTTOM = 133;
constexpr int BASE_MARGIN = 48;

static const COLORREF CLR_BG       = RGB(21, 26, 28);
static const COLORREF CLR_TRACK    = RGB(106, 106, 106);
static const COLORREF CLR_VOLUME   = RGB(0, 122, 213);

//windows 8 DP(tbh 8102 doesn't have brighness osd so i made it myself)
constexpr int DP_W = 360, DP_H = 60;
constexpr int DP_BAR_X = 76, DP_BAR_Y = 25, DP_BAR_W = 264, DP_BAR_H = 11;
constexpr int DP_MARGIN_X = 60, DP_MARGIN_B = 10;

static const COLORREF CLR_DP_BG          = RGB(0x1D, 0x1D, 0x1D);
static const COLORREF CLR_DP_GREEN       = RGB(0x10, 0xA1, 0x51);
static const COLORREF CLR_DP_TRACK       = RGB(0x6A, 0x6A, 0x6A);
static const COLORREF CLR_DP_FILL_MUTED  = RGB(0x51, 0x51, 0x51);
static const COLORREF CLR_DP_TRACK_MUTED = RGB(0x36, 0x36, 0x36);
static const COLORREF CLR_DP_ICON_DIM    = RGB(0x77, 0x77, 0x77);

static const GUID kBrGuid1 = {0x8ffee2c6, 0x2d01, 0x46be,
    {0xad, 0xb9, 0x39, 0x8a, 0xdd, 0xc5, 0xb4, 0xff}};
static const GUID kBrGuid2 = {0xaded5e82, 0xb909, 0x4619,
    {0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb}};
//supress native osd
using GetWindowBand_t = BOOL(WINAPI*)(HWND, PDWORD);
using GetThreadDescription_t = HRESULT(WINAPI*)(HANDLE, PWSTR*);

static DWORD GetBandOf(HWND hwnd) {
    static GetWindowBand_t fn = (GetWindowBand_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetWindowBand");
    DWORD band = 0;
    if (!fn || !fn(hwnd, &band)) return 0;
    return band;
}

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

static bool ContainsInsensitiveAscii(PCWSTR s, PCWSTR sub) {
    if (!*sub) return true;
    for (PCWSTR p = s; *p; p++) {
        PCWSTR a = p, b = sub;
        while (*a && *b) {
            WCHAR ca = *a, cb = *b;
            if (ca >= L'A' && ca <= L'Z') ca = (WCHAR)(ca - L'A' + L'a');
            if (cb >= L'A' && cb <= L'Z') cb = (WCHAR)(cb - L'A' + L'a');
            if (ca != cb) break;
            a++;
            b++;
        }
        if (!*b) return true;
    }
    return false;
}

static bool IsKnownNonOsdFlyout(HWND hwnd, PCWSTR desc) {
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid != GetCurrentProcessId())
        return false;

    if (wcscmp(desc, L"MultitaskingView") == 0) return true;
    return ContainsInsensitiveAscii(desc, L"Snap");
}

static WCHAR g_nativeDesc[128] = L"";
static std::atomic<DWORD> g_nativeDescBand{0};
static std::atomic<bool> g_nativeSigClaimed{false};
static std::atomic<bool> g_nativeSigReady{false};

static void LearnNativeOsdSignature(HWND hwnd, PCWSTR desc) {
    if (g_nativeSigClaimed.exchange(true)) return;
    lstrcpynW(g_nativeDesc, desc, ARRAYSIZE(g_nativeDesc));
    g_nativeDescBand = GetBandOf(hwnd);
    g_nativeSigReady.store(true, std::memory_order_release);
    Wh_Log(L"Learned the native OSD signature: thread=\"%s\" band=%u",
           g_nativeDesc, g_nativeDescBand.load());
}

static bool MatchesNativeOsdSignature(HWND hwnd, PCWSTR desc) {
    if (!g_nativeSigReady.load(std::memory_order_acquire)) return true;
    if (g_nativeDesc[0]) return wcscmp(desc, g_nativeDesc) == 0;
    DWORD band = g_nativeDescBand.load();
    return !band || GetBandOf(hwnd) == band;
}

static void LogCandidate(HWND hwnd, PCWSTR cls, PCWSTR desc, int w, int h, bool recent) {
    using InternalGetWindowText_t = int(WINAPI*)(HWND, LPWSTR, int);
    static InternalGetWindowText_t pInternalGetWindowText =
        (InternalGetWindowText_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "InternalGetWindowText");
    WCHAR title[128] = L"";
    if (pInternalGetWindowText) pInternalGetWindowText(hwnd, title, ARRAYSIZE(title));

    Wh_Log(L"OSD candidate: hwnd=%p class=%s title=\"%s\" thread=\"%s\" band=%u size=%dx%d recentEvent=%d",
           hwnd, cls, title, desc, GetBandOf(hwnd), w, h, recent ? 1 : 0);
}

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

    if (!(w > 0 && h > 0 && w < 700 && h < 250)) return false;

    WCHAR desc[128];
    GetThreadDescriptionOf(hwnd, desc, ARRAYSIZE(desc));


    if (IsKnownNonOsdFlyout(hwnd, desc)) {
        Wh_Log(L"Not an OSD: shell flyout, hwnd=%p thread=\"%s\" band=%u size=%dx%d",
               hwnd, desc, GetBandOf(hwnd), w, h);
        return false;
    }

    if (recent) {
        LearnNativeOsdSignature(hwnd, desc);
    } else if (!MatchesNativeOsdSignature(hwnd, desc)) {
        Wh_Log(L"Not an OSD: signature mismatch, hwnd=%p thread=\"%s\" band=%u size=%dx%d",
               hwnd, desc, GetBandOf(hwnd), w, h);
        return false;
    }
    LogCandidate(hwnd, cls, desc, w, h, recent);
    if (recent && g_cfg.alwaysOnTop.load() && g_cfg.osdBand.load() == 0) {
        DWORD b = GetBandOf(hwnd);
        if (b >= 2 && b != g_nativeBand.load()) {
            g_nativeBand = b;
            HWND osd = g_osd.load();
            if (osd) PostMessageW(osd, WM_BANDLEARNED, 0, 0);
        }
    }
    return true;
}

bool NativeOsdSeen(HWND hwnd, int w, int h) {

    bool trigger = g_cfg.brightness && g_cfg.nativeTrigger;
    if (!g_cfg.suppress && !trigger) return false;

    ULONGLONG now = GetTickCount64();
    bool volRecent = now - g_lastVolTick.load() < 2500;
    bool brRecent  = now - g_lastBrTick.load() < 2500;
    bool anyRecent = volRecent || brRecent;
    if (!IsNativeOsd(hwnd, w, h, anyRecent)) return false;

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

std::atomic<bool> g_wmiOk{true};

struct DdcState {
    SRWLOCK   lock = SRWLOCK_INIT;
    HANDLE    ev = nullptr; 
    HANDLE    thread = nullptr;
    bool      quit = false;
    bool      doRead = false;  HMONITOR readMon = nullptr;
    bool      doSet = false;   HMONITOR setMon = nullptr;  DWORD setValue = 0;
    bool      doWmiSet = false; int wmiValue = 0;   //internal panel (WMI)
    bool      resOk = false;   HMONITOR resMon = nullptr;
    DWORD     resMin = 0, resCur = 0, resMax = 0;
};
DdcState g_ddc;
IWbemServices* g_wmiSvc = nullptr;

static IWbemServices* WmiConnect() {
    if (g_wmiSvc) return g_wmiSvc;

    IWbemLocator* loc = nullptr;
    HRESULT hrCo = CoCreateInstance(CLSID_WbemLocator, nullptr,
                                    CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                    (void**)&loc);
    if (FAILED(hrCo) || !loc) {
        Wh_Log(L"WMI: CoCreateInstance(WbemLocator) failed: 0x%08X", (unsigned)hrCo);
        return nullptr;
    }

    BSTR ns = SysAllocString(L"root\\WMI");
    IWbemServices* svc = nullptr;
    HRESULT hr = loc->ConnectServer(ns, nullptr, nullptr, nullptr, 0, nullptr,
                                    nullptr, &svc);
    SysFreeString(ns);
    loc->Release();
    if (FAILED(hr) || !svc) {
        Wh_Log(L"WMI: ConnectServer(root\\WMI) failed: 0x%08X", (unsigned)hr);
        return nullptr;
    }
    CoSetProxyBlanket(svc, RPC_C_AUTHN_WINNT, RPC_C_AUTHN_NONE, nullptr,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                      nullptr, EOAC_NONE);
    g_wmiSvc = svc;
    return svc;
}
static int WmiSetBrightnessAll(int level) {
    IWbemServices* svc = WmiConnect();
    if (!svc) return 0;

    BSTR clsName = SysAllocString(L"WmiMonitorBrightnessMethods");
    IWbemClassObject* cls = nullptr;
    HRESULT hr = svc->GetObject(clsName, 0, nullptr, &cls, nullptr);
    if (FAILED(hr) || !cls) {
        Wh_Log(L"WMI: GetObject(WmiMonitorBrightnessMethods) failed: 0x%08X", (unsigned)hr);
        SysFreeString(clsName);
        return -1;
    }

    BSTR mthName = SysAllocString(L"WmiSetBrightness");
    IWbemClassObject* inSig = nullptr;
    hr = cls->GetMethod(mthName, 0, &inSig, nullptr);
    cls->Release();
    if (FAILED(hr) || !inSig) {
        Wh_Log(L"WMI: GetMethod(WmiSetBrightness) failed: 0x%08X", (unsigned)hr);
        SysFreeString(mthName);
        SysFreeString(clsName);
        return -1;
    }

    IEnumWbemClassObject* en = nullptr;
    hr = svc->CreateInstanceEnum(clsName,
                                 WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                 nullptr, &en);
    SysFreeString(clsName);
    if (FAILED(hr) || !en) {
        Wh_Log(L"WMI: CreateInstanceEnum failed: 0x%08X", (unsigned)hr);
        inSig->Release();
        SysFreeString(mthName);
        return 0;
    }

    int found = 0, done = 0;
    for (;;) {
        IWbemClassObject* inst = nullptr;
        ULONG got = 0;
        if (FAILED(en->Next(WBEM_INFINITE, 1, &inst, &got)) || !got || !inst) break;
        found++;

        BSTR pathProp = SysAllocString(L"__PATH");
        VARIANT path;
        VariantInit(&path);
        bool havePath = SUCCEEDED(inst->Get(pathProp, 0, &path, nullptr, nullptr)) &&
                        path.vt == VT_BSTR && path.bstrVal;
        SysFreeString(pathProp);

        if (havePath) {
            IWbemClassObject* inParams = nullptr;
            if (SUCCEEDED(inSig->SpawnInstance(0, &inParams)) && inParams) {
                VARIANT v;
                VariantInit(&v);
                BSTR nTimeout = SysAllocString(L"Timeout");
                v.vt = VT_I4;
                v.lVal = 1;
                HRESULT hrT = inParams->Put(nTimeout, 0, &v, 0);
                SysFreeString(nTimeout);
                BSTR nBright = SysAllocString(L"Brightness");
                v.vt = VT_I4;
                v.lVal = level;
                HRESULT hrB = inParams->Put(nBright, 0, &v, 0);
                SysFreeString(nBright);
                VariantClear(&v);

                HRESULT hrE = svc->ExecMethod(path.bstrVal, mthName, 0, nullptr,
                                              inParams, nullptr, nullptr);
                if (SUCCEEDED(hrE)) {
                    done++;
                } else {
                    Wh_Log(L"WMI: WmiSetBrightness(%d) failed: exec=0x%08X putTimeout=0x%08X putBrightness=0x%08X path=%s",
                           level, (unsigned)hrE, (unsigned)hrT, (unsigned)hrB, path.bstrVal);
                }
                inParams->Release();
            }
        } else {
            Wh_Log(L"WMI: brightness instance without __PATH");
        }
        VariantClear(&path);
        inst->Release();
    }
    en->Release();
    inSig->Release();
    SysFreeString(mthName);

    Wh_Log(L"WMI: set brightness to %d: %d of %d panel(s) ok", level, done, found);
    if (!found) return -1;
    return done ? 1 : 0;
}

DWORD WINAPI DdcThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    for (;;) {
        if (WaitForSingleObject(g_ddc.ev, INFINITE) != WAIT_OBJECT_0) break;

        AcquireSRWLockExclusive(&g_ddc.lock);
        bool quit = g_ddc.quit;
        bool doRead = g_ddc.doRead;  HMONITOR readMon = g_ddc.readMon;
        bool doSet = g_ddc.doSet;    HMONITOR setMon = g_ddc.setMon;
        DWORD setValue = g_ddc.setValue;
        bool doWmiSet = g_ddc.doWmiSet;  int wmiValue = g_ddc.wmiValue;
        g_ddc.doRead = false;
        g_ddc.doSet = false;
        g_ddc.doWmiSet = false;
        ReleaseSRWLockExclusive(&g_ddc.lock);

        if (quit) break;

        if (doWmiSet) {
            int r = WmiSetBrightnessAll(wmiValue);
            if (r < 0) {
                g_wmiOk = false;
                Wh_Log(L"WMI: no WmiMonitorBrightnessMethods instance; internal-panel drag disabled");
            } else if (r == 0) {
                Wh_Log(L"WMI: WmiSetBrightness failed");
                if (g_wmiSvc) { g_wmiSvc->Release(); g_wmiSvc = nullptr; }
            }
        }
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
    if (g_wmiSvc) { g_wmiSvc->Release(); g_wmiSvc = nullptr; }
    CoUninitialize();
    return 0;
}

static bool DdcEnsure() {
    if (g_ddc.thread) return true;
    g_ddc.quit = false;
    g_ddc.ev = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_ddc.ev) return false;
    g_ddc.thread = CreateThread(nullptr, 0, DdcThread, nullptr, 0, nullptr);
    if (!g_ddc.thread) {
        CloseHandle(g_ddc.ev);
        g_ddc.ev = nullptr;
        return false;
    }
    return true;
}

void DdcStop() {
    if (g_ddc.thread) {
        AcquireSRWLockExclusive(&g_ddc.lock);
        g_ddc.quit = true;
        ReleaseSRWLockExclusive(&g_ddc.lock);
        SetEvent(g_ddc.ev);
        WaitForSingleObject(g_ddc.thread, INFINITE);
        CloseHandle(g_ddc.thread);
        g_ddc.thread = nullptr;
    }
    if (g_ddc.ev) { CloseHandle(g_ddc.ev); g_ddc.ev = nullptr; }
}

static bool DdcRequestRead(HMONITOR mon) {
    if (!DdcEnsure()) return false;
    AcquireSRWLockExclusive(&g_ddc.lock);
    g_ddc.readMon = mon;
    g_ddc.doRead = true;
    ReleaseSRWLockExclusive(&g_ddc.lock);
    SetEvent(g_ddc.ev);
    return true;
}

bool      g_extReadBusy = false;
int       g_extPendingDelta = 0;
HMONITOR  g_extPendingMon = nullptr;
HMONITOR  g_extFailMon = nullptr;  
ULONGLONG g_extFailTick = 0;

static void ExtRequestRead(HMONITOR mon) {
    if (g_extReadBusy) return;
    if (mon == g_extFailMon && GetTickCount64() - g_extFailTick < 5000) return;
    g_extReadBusy = DdcRequestRead(mon);
}

void ExtApply() {
    if (!g_extMon || g_extMax <= g_extMin) return;
    if (!DdcEnsure()) return;
    DWORD v = g_extMin + (g_extMax - g_extMin) * g_extPct / 100;
    AcquireSRWLockExclusive(&g_ddc.lock);
    g_ddc.setMon = g_extMon;
    g_ddc.setValue = v;
    g_ddc.doSet = true;
    ReleaseSRWLockExclusive(&g_ddc.lock);
    SetEvent(g_ddc.ev);
}
void WmiApply() {
    if (!DdcEnsure()) return;
    AcquireSRWLockExclusive(&g_ddc.lock);
    g_ddc.wmiValue = g_wmiPending;
    g_ddc.doWmiSet = true;
    ReleaseSRWLockExclusive(&g_ddc.lock);
    SetEvent(g_ddc.ev);
}

static void ScheduleApply(HWND h) {
    g_applyPending = true;
    SetTimer(h, TIMER_APPLY, 60, nullptr);
}

//OSD window
COLORREF AccentColor() {
    if (g_cfg.useAccent) {
        DWORD c = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&c, &opaque)))
            return RGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
    }
    return g_cfg.dp ? CLR_DP_GREEN : CLR_VOLUME;
}

static HFONT g_fontIcon = nullptr, g_fontText = nullptr;
static HFONT g_fontDp = nullptr;   //segoe UI symbol(closest thing i find)
static int   g_fontDpi = 0;

static void FreeFonts() {
    if (g_fontIcon) { DeleteObject(g_fontIcon); g_fontIcon = nullptr; }
    if (g_fontText) { DeleteObject(g_fontText); g_fontText = nullptr; }
    if (g_fontDp) { DeleteObject(g_fontDp); g_fontDp = nullptr; }
    g_fontDpi = 0;
}

static void EnsureFonts() {
    if (g_fontIcon && g_fontText && g_fontDp && g_fontDpi == g_dpi) return;
    FreeFonts();
    g_fontIcon = CreateFontW(-S(16), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                             0, 0, CLEARTYPE_QUALITY, 0, L"Segoe MDL2 Assets");
    g_fontText = CreateFontW(-S(13), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                             0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_fontDp = CreateFontW(-S(32), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                           0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Symbol");
    g_fontDpi = g_dpi;
}

static void FillColor(HDC dc, const RECT* r, COLORREF c) {
    SetDCBrushColor(dc, c);
    FillRect(dc, r, (HBRUSH)GetStockObject(DC_BRUSH));
}

static void DrawCentered(HDC dc, RECT* r, PCWSTR text, HFONT font) {
    HGDIOBJ old = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    DrawTextW(dc, text, -1, r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, old);
}
static void DrawDpSun(HDC dc, int cx, int cy) {
    static const int kDir[8][2] = {{10, 0}, {7, 7}, {0, 10}, {-7, 7},
                                   {-10, 0}, {-7, -7}, {0, -10}, {7, -7}};
    COLORREF white = RGB(255, 255, 255);

    int pw = S(3);
    HPEN pen = CreatePen(PS_SOLID, pw > 1 ? pw : 1, white);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    for (int i = 0; i < 8; i++) {
        MoveToEx(dc, cx + MulDiv(kDir[i][0], S(10), 10),
                     cy + MulDiv(kDir[i][1], S(10), 10), nullptr);
        LineTo(dc, cx + MulDiv(kDir[i][0], S(14), 10),
                   cy + MulDiv(kDir[i][1], S(14), 10));
    }
    SelectObject(dc, oldPen);
    DeleteObject(pen);

    HBRUSH br = CreateSolidBrush(white);
    HGDIOBJ oldBr = SelectObject(dc, br);
    pen = CreatePen(PS_SOLID, 1, white);
    oldPen = SelectObject(dc, pen);
    Ellipse(dc, cx - S(7), cy - S(7), cx + S(7) + 1, cy + S(7) + 1);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBr);
    DeleteObject(pen);
    DeleteObject(br);
}
static void PaintDp(HDC dc, const RECT& rc, int level, bool bright) {
    FillColor(dc, &rc, CLR_DP_BG);

    bool volMuted = !bright && g_muted;
    bool dimIcon = !bright && (g_muted || level == 0);

    RECT bar = { S(DP_BAR_X), S(DP_BAR_Y),
                 S(DP_BAR_X) + S(DP_BAR_W), S(DP_BAR_Y) + S(DP_BAR_H) };
    FillColor(dc, &bar, volMuted ? CLR_DP_TRACK_MUTED : CLR_DP_TRACK);

    int fillW = (bar.right - bar.left) * level / 100;
    if (fillW > 0) {
        RECT fill = { bar.left, bar.top, bar.left + fillW, bar.bottom };
        FillColor(dc, &fill, volMuted ? CLR_DP_FILL_MUTED : AccentColor());
    }

    if (bright) {
        DrawDpSun(dc, S(38), rc.bottom / 2);
        return;
    }
    RECT ir = { 0, 0, S(DP_BAR_X), rc.bottom };
    HGDIOBJ old = SelectObject(dc, g_fontDp);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, dimIcon ? CLR_DP_ICON_DIM : RGB(255, 255, 255));
    DrawTextW(dc, dimIcon ? L"\uE198" : L"\uE15D", 1, &ir,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, old);
}

void Paint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC wdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    EnsureFonts();

    HDC dc = CreateCompatibleDC(wdc);
    HBITMAP bmp = CreateCompatibleBitmap(wdc, rc.right, rc.bottom);
    HGDIOBJ oldBmp = SelectObject(dc, bmp);

    bool bright = g_mode == MODE_BRIGHTNESS;
    int level = g_level;
    if (level < 0) level = 0;
    if (level > 100) level = 100;

    if (g_cfg.dp) {
        PaintDp(dc, rc, level, bright);
    } else {
        FillColor(dc, &rc, CLR_BG);
        int bx = (rc.right - S(BAR_W)) / 2;
        RECT track = { bx, S(BAR_TOP), bx + S(BAR_W), S(BAR_TOP) + S(BAR_H) };
        FillColor(dc, &track, CLR_TRACK);
        int thumbH = S(THUMB_H);
        int travel = (track.bottom - track.top) - thumbH;
        int thumbTop = track.top + travel * (100 - level) / 100;

        RECT fill = { track.left, thumbTop + thumbH, track.right, track.bottom };
        if (fill.bottom > fill.top) {
            FillColor(dc, &fill, AccentColor());
        }

        RECT thumb = { track.left, thumbTop, track.right, thumbTop + thumbH };
        FillColor(dc, &thumb, RGB(255, 255, 255));

        RECT nr = { 0, track.bottom, rc.right, S(TEXT_BOTTOM) };
        if (bright) {
            DrawCentered(dc, &nr, L"\uE706", g_fontIcon);
        } else if (g_muted || g_level == 0) {
            DrawCentered(dc, &nr, L"\uE10A", g_fontIcon);
        } else {
            WCHAR t[8];
            wsprintfW(t, L"%d", g_level);
            DrawCentered(dc, &nr, t, g_fontText);
        }
    }

    BitBlt(wdc, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(hwnd, &ps);
}

void ShowOsd(HWND h, HMONITOR target = nullptr) {
    KillTimer(h, TIMER_FADE);
    g_alpha = 255;
    SetLayeredWindowAttributes(h, 0, 255, LWA_ALPHA);

    HMONITOR mon = target ? target
                          : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfoW(mon, &mi);
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX)
        g_dpi = (int)dpiX;

    if (g_cfg.dp) {
        SetWindowPos(h, HWND_TOPMOST,
                     mi.rcWork.left + S(DP_MARGIN_X),
                     mi.rcWork.bottom - S(DP_H) - S(DP_MARGIN_B),
                     S(DP_W), S(DP_H),
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        SetWindowPos(h, HWND_TOPMOST,
                     mi.rcWork.left + S(BASE_MARGIN),
                     mi.rcWork.top + S(BASE_MARGIN),
                     S(BASE_W), S(BASE_H),
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    InvalidateRect(h, nullptr, FALSE);
    if (g_dragging) KillTimer(h, TIMER_HIDE);
    else            SetTimer(h, TIMER_HIDE, g_cfg.timeoutMs, nullptr);
}

void ShowBrightness(HWND h, int level, HMONITOR target = nullptr) {
    g_mode  = MODE_BRIGHTNESS;
    g_level = level;
    g_muted = false;
    g_brTarget = target;
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
            v += (id == HK_VOL_UP) ? 0.02f : -0.02f;   //2% steps like Windows
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

static void ExtStep(HWND h, int delta) {
    g_extPct += delta;
    if (g_extPct < 0)   g_extPct = 0;
    if (g_extPct > 100) g_extPct = 100;
    g_extLast = GetTickCount64();
    g_lastBrTick = g_extLast;

    ShowBrightness(h, g_extPct, g_extMon);
    g_applyWmi = false;
    ScheduleApply(h);
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
    if (g_extPendingMon != mon) g_extPendingDelta = 0;
    g_extPendingMon = mon;
    g_extPendingDelta += delta;
    ExtRequestRead(mon);
}
void SetLevelFromMouse(HWND h, int x, int y) {
    int level;
    if (g_cfg.dp) {
        level = MulDiv(x - S(DP_BAR_X), 100, S(DP_BAR_W));
    } else {
        RECT rc;
        GetClientRect(h, &rc);
        int top = S(BAR_TOP);
        int bottom = top + S(BAR_H);
        int thumb = S(THUMB_H);
        int travel = (bottom - top) - thumb;
        if (travel <= 0) return;
        level = 100 - MulDiv(y - top - thumb / 2, 100, travel);
    }
    if (level < 0) level = 0;
    if (level > 100) level = 100;
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
    } else if (!g_brTarget && g_wmiOk.load()) {
        g_level = level;
        g_brightness = level;
        g_wmiPending = level;
        g_lastBrTick = GetTickCount64();
        g_applyWmi = true;
        ScheduleApply(h);
    } else {
        POINT pt;
        GetCursorPos(&pt);
        HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        if (mon != g_extMon || g_extMax <= g_extMin) {
            g_extPendingMon = mon;
            g_extPendingDelta = 0;
            ExtRequestRead(mon);
            return;
        }
        g_extPct = level;
        g_extLast = GetTickCount64();
        g_level = level;
        g_applyWmi = false;
        ScheduleApply(h);
    }
    InvalidateRect(h, nullptr, FALSE);
}

//window procedure
static void KbStart();
static void KbStop();

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
                        g_level = v;                       //refresh a flyout already open
                        InvalidateRect(h, nullptr, FALSE);
                    }
                }
            }
        }
        return TRUE;

    case WM_NATIVE://fallback 
        if (g_cfg.brightness && g_brightness >= 0)
            ShowBrightness(h, g_brightness);
        return 0;

    case WM_RELOAD: {
        bool oldTop = g_cfg.alwaysOnTop.load();
        int  oldBand = g_cfg.osdBand.load();
        bool oldDp = g_cfg.dp;
        LoadSettings(); 
        RegisterKeys(h);

        if (g_cfg.kbHook && !g_kbThread) KbStart();
        else if (!g_cfg.kbHook && g_kbThread) KbStop();
        if (oldTop != g_cfg.alwaysOnTop.load() || oldBand != g_cfg.osdBand.load()) {
            PostThreadMessageW(GetCurrentThreadId(), WM_RECREATE, 0, 0);
        } else if (oldDp != g_cfg.dp && IsWindowVisible(h)) {
            ShowOsd(h, g_brTarget);
        }
        InvalidateRect(h, nullptr, FALSE);
        return 0;
    }

    case WM_BANDLEARNED: {
        DWORD b = g_nativeBand.load();
        if (g_cfg.alwaysOnTop.load() && g_cfg.osdBand.load() == 0 &&
            b >= 2 && b != g_triedNativeBand)
            PostThreadMessageW(GetCurrentThreadId(), WM_RECREATE, 0, 0);
        return 0;
    }

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
            ExtRequestRead(g_extPendingMon);
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
        SetTimer(h, TIMER_VOLKEY, 40, nullptr);
        return 0;

    case WM_REBIND:
        BindEndpoint();
        return 0;

    case WM_TIMER:
        if (w == TIMER_HIDE) {
            KillTimer(h, TIMER_HIDE);
            SetTimer(h, TIMER_FADE, 16, nullptr);          //start fade animation
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
            g_applyPending = false;
            if (g_applyWmi) WmiApply();
            else            ExtApply();
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
        bool hit;
        if (g_cfg.dp) {
            hit = x >= S(DP_BAR_X) - S(4) && x <= S(DP_BAR_X) + S(DP_BAR_W) + S(4) &&
                  y >= S(DP_BAR_Y) - S(6) && y <= S(DP_BAR_Y) + S(DP_BAR_H) + S(6);
        } else {
            int left = (rc.right - S(BAR_W)) / 2;
            int top = S(BAR_TOP);
            int bottom = top + S(BAR_H);
            int thumb = S(THUMB_H);
            int thumbTop = top + ((bottom - top) - thumb) * (100 - g_level) / 100;
            hit = x >= left - S(4) && x <= left + S(BAR_W) + S(4) &&
                  y >= thumbTop - S(4) && y <= thumbTop + thumb + S(4);
        }
        if (hit) {
            g_dragging = true;
            SetCapture(h);
            SetLevelFromMouse(h, x, y);
        }
        return 0;
    }
    case WM_MOUSEMOVE:
        if (g_dragging) SetLevelFromMouse(h, (int)(short)LOWORD(l), (int)(short)HIWORD(l));
        return 0;
    case WM_LBUTTONUP:
        if (g_dragging) {
            g_dragging = false;
            ReleaseCapture();
            SetTimer(h, TIMER_HIDE, g_cfg.timeoutMs, nullptr);
        }
        return 0;
    case WM_CAPTURECHANGED:
        if (g_dragging) {
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

static HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)&GetCurrentModuleHandle, &module))
        return module;
    return nullptr;
}
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

DWORD WINAPI KbThread(LPVOID ready) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);   //create the queue
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

static void KbStart() {
    if (g_kbThread) return;
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ready) return;
    g_kbThread = CreateThread(nullptr, 0, KbThread, ready, 0, &g_kbThreadId);
    if (g_kbThread) WaitForSingleObject(ready, INFINITE);
    CloseHandle(ready);
}

static void KbStop() {
    if (!g_kbThread) return;
    PostThreadMessageW(g_kbThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_kbThread, INFINITE);
    CloseHandle(g_kbThread);
    g_kbThread = nullptr;
}
typedef HWND (WINAPI *CreateWindowInBand_t)(DWORD, LPCWSTR, LPCWSTR, DWORD,
    int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, DWORD);

HWND CreateOsdWindow(WNDCLASSW* wc, int w, int h) {
    g_osdBand = 0;
    if (g_cfg.alwaysOnTop.load()) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        auto createInBand = user32
            ? (CreateWindowInBand_t)GetProcAddress(user32, "CreateWindowInBand")
            : nullptr;
        if (createInBand) {
            constexpr DWORD ZBID_UIACCESS = 2;
            DWORD candidates[3];
            int n = 0;
            auto add = [&](DWORD b) {
                if (b < 2) return;
                for (int i = 0; i < n; i++) if (candidates[i] == b) return;
                candidates[n++] = b;
            };
            add((DWORD)g_cfg.osdBand.load());
            DWORD learned = g_nativeBand.load();
            if (learned) g_triedNativeBand = learned;
            add(learned);
            add(ZBID_UIACCESS);

            for (int i = 0; i < n; i++) {
                HWND bandWindow = createInBand(
                    WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
                    wc->lpszClassName, L"", WS_POPUP, 0, 0, w, h,
                    nullptr, nullptr, wc->hInstance, nullptr, candidates[i]);
                if (bandWindow) {
                    g_osdBand = candidates[i];
                    Wh_Log(L"OSD created in band %u", candidates[i]);
                    return bandWindow;
                }
                Wh_Log(L"CreateWindowInBand(band %u) failed: %u",
                       candidates[i], GetLastError());
            }
        }
    }

    return CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wc->lpszClassName, L"", WS_POPUP, 0, 0, w, h,
        nullptr, nullptr, wc->hInstance, nullptr);
}
void RecreateOsd() {
    HWND old = g_osd.load();
    if (!old) return;
    bool wasVisible = IsWindowVisible(old);

    HWND nw = CreateOsdWindow(&g_wc, S(BASE_W), S(BASE_H));
    if (!nw) {
        Wh_Log(L"Recreating the OSD window failed: %u", GetLastError());
        return;
    }
    SetLayeredWindowAttributes(nw, 0, 255, LWA_ALPHA);

    if (g_dragging) { g_dragging = false; ReleaseCapture(); }
    UnregisterKeys(old);
    if (g_pn1) { UnregisterPowerSettingNotification(g_pn1); g_pn1 = nullptr; }
    if (g_pn2) { UnregisterPowerSettingNotification(g_pn2); g_pn2 = nullptr; }

    //the timers die with the old window send a coalesced brightness change
    if (g_applyPending) {
        g_applyPending = false;
        if (g_applyWmi) WmiApply();
        else            ExtApply();
    }

    g_osd = nw;
    DestroyWindow(old);

    RegisterKeys(nw);
    g_brArmed = GetTickCount64() + 1500;
    g_pn1 = RegisterPowerSettingNotification(nw, &kBrGuid1, DEVICE_NOTIFY_WINDOW_HANDLE);
    g_pn2 = RegisterPowerSettingNotification(nw, &kBrGuid2, DEVICE_NOTIFY_WINDOW_HANDLE);

    // The flyout was on screen (the band is usually learned while the native
    // OSD, and with it our flyout, is showing): show it again in the new window.
    if (wasVisible)
        ShowOsd(nw, g_mode == MODE_BRIGHTNESS ? g_brTarget : nullptr);
}

DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_dpi = GetDpiForSystem();
    WNDCLASSW& wc = g_wc;
    wc = WNDCLASSW{};
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

    HWND hwnd = CreateOsdWindow(&wc, S(BASE_W), S(BASE_H));
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        SetEvent(g_ready);
        CoUninitialize();
        return 0;
    }
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    g_osd = hwnd;
    SetEvent(g_ready);

    RegisterKeys(hwnd);
    if (g_cfg.kbHook) KbStart();

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
        if (!msg.hwnd && msg.message == WM_RECREATE) {
            RecreateOsd();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    hwnd = g_osd.exchange(nullptr);
    KbStop();
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
    FreeFonts();
    CoUninitialize();
    return 0;
}

static bool ContainsNoCase(PCWSTR s, PCWSTR needle) {
    size_t n = wcslen(needle);
    for (; *s; s++) {
        if (_wcsnicmp(s, needle, n) == 0) return true;
    }
    return false;
}

BOOL Wh_ModInit() {
    PCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && (ContainsNoCase(cmdLine, L"/factory") ||
                    ContainsNoCase(cmdLine, L"-Embedding") ||
                    ContainsNoCase(cmdLine, L"/separate"))) {
        return FALSE;
    }

    LoadSettings();
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

    if (!g_osd.load()) {
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
    HWND osd = g_osd.load();
    if (osd) PostMessageW(osd, WM_RELOAD, 0, 0);
}

void Wh_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        //thread must be fully joined before the mod is unloaded
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) { CloseHandle(g_ready); g_ready = nullptr; }
}
