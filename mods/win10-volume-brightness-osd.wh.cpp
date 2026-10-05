// ==WindhawkMod==
// @id              win10-volume-brightness-osd
// @name            Windows 10 Volume & Brightness OSD
// @description     Replaces the Windows 11 volume/brightness OSD with the classic vertical Windows 10 flyout
// @version         0.3.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lgdi32 -luser32 -ldwmapi -ldxva2
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Style Volume & Brightness OSD

Shows the classic vertical Windows 10 flyout (dark panel, grey track, white
thumb, accent fill) for **volume** and **brightness**.

## Volume
* The volume keys (up / down / mute) are captured with `RegisterHotKey`, so the
  flyout also appears when the volume is already 0 or 100.
* Volume changes from any other source (WASAPI) also show the flyout.
* When muted, or at volume 0, a cross icon replaces the number.

## Brightness
* **Laptops:** the mod listens to the system brightness notification. The
  Fn brightness keys keep working normally.
* **Desktop PCs / external monitors:** enable "External monitor hotkeys". The
  mod then uses DDC/CI to change the brightness of the monitor under the mouse
  cursor with **Ctrl+Alt+Up / Ctrl+Alt+Down**.

## Screenshots:

### Volume(Desktop):
![volumedesk](https://raw.githubusercontent.com/AdmXP8/assets/main/volosd8desk.png)

### Brightness(Desktop):
![brightnessdesk](https://raw.githubusercontent.com/AdmXP8/assets/main/bgtosd8desk.png)

### Volume:
![volume](https://raw.githubusercontent.com/AdmXP8/assets/main/volosd8.png)

### Brightness:
![brightness](https://raw.githubusercontent.com/AdmXP8/assets/main/bgtosd8.png)


**issue**:OSD cannot appear over fullscreen UWP apps.

**Special thanks to:** babamohammed2022 for the base version of the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeoutMs: 1500
  $name: Display duration (ms)
- suppressNative: true
  $name: Suppress the native Windows 11 OSD
- brightnessEnabled: true
  $name: Enable the brightness flyout
- externalHotkeys: false
  $name: External monitor hotkeys (Ctrl+Alt+Up / Down, DDC/CI)
- useSystemAccent: false
  $name: Use the system accent colour for the volume bar
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#include <atomic>
#include <vector>

// Settings
struct {
    int  timeoutMs;
    bool suppress;
    bool volKeys;
    bool brightness;
    bool nativeTrigger;
    bool extKeys;
    int  extStep;
    bool useAccent;
    bool debug;
    bool overFullscreen;
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
    g_cfg.debug      = Wh_GetIntSetting(L"debugLog") != 0;
    g_cfg.overFullscreen = Wh_GetIntSetting(L"overFullscreen") != 0;

    PCWSTR trig = Wh_GetStringSetting(L"brightnessTrigger");
    g_cfg.nativeTrigger = trig && wcscmp(trig, L"nativeOsd") == 0;
    Wh_FreeStringSetting(trig);
}

// Global state
constexpr UINT WM_VOL    = WM_APP + 1;  // wParam = level 0-100, lParam = muted
constexpr UINT WM_REBIND = WM_APP + 2;  // default audio device changed
constexpr UINT WM_NATIVE = WM_APP + 3;  // native OSD was detected
constexpr UINT WM_RELOAD = WM_APP + 4;  // settings changed

constexpr UINT_PTR TIMER_HIDE  = 1;
constexpr UINT_PTR TIMER_FADE  = 2;
constexpr UINT_PTR TIMER_APPLY = 3;

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
int  g_dpi   = 96;
int  g_alpha = 255;

// laptop brightness (power notification)
int       g_brightness = -1;
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
static const COLORREF CLR_BRIGHT   = RGB(6, 77, 111);

// GUID_VIDEO_CURRENT_MONITOR_BRIGHTNESS
static const GUID kBrGuid1 = {0x8ffee2c6, 0x2d01, 0x46be,
    {0xad, 0xb9, 0x39, 0x8a, 0xdd, 0xc5, 0xb4, 0xff}};
// GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS
static const GUID kBrGuid2 = {0xaded5e82, 0xb909, 0x4619,
    {0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb}};

// Native OSD detection / suppression
// w/h = -1 -> use the current window size
bool IsNativeOsd(HWND hwnd, int w, int h) {
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

    if (g_cfg.debug)
        Wh_Log(L"OSD candidate: hwnd=%p class=%s size=%dx%d", hwnd, cls, w, h);

    // The native OSD is a small window. Refine for your build if needed.
    return w > 0 && h > 0 && w < 700 && h < 250;
}

// Returns true when the native OSD must be blocked.
bool NativeOsdSeen(HWND hwnd, int w, int h) {
    // Brightness notifications can arrive slightly before or after the native
    // OSD window is created. Use a wider window than volume, otherwise the
    // native brightness flyout can slip through.
    bool trigger = g_cfg.brightness && g_cfg.nativeTrigger;
    if (!g_cfg.suppress && !trigger) return false;

    ULONGLONG now = GetTickCount64();
    bool volRecent = now - g_lastVolTick.load() < 2500;
    bool brRecent  = now - g_lastBrTick.load() < 2500;
    bool anyRecent = volRecent || brRecent;

    // When suppression is enabled, block every matching native OSD. This is
    // more reliable than depending on the order of the volume/brightness
    // notification and the XAML window creation.
    if (!anyRecent && !trigger && !g_cfg.suppress) return false;
    if (!IsNativeOsd(hwnd, w, h)) return false;

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

void ExtApply() {
    if (!g_extMon || g_extMax <= g_extMin) return;
    DWORD v = g_extMin + (g_extMax - g_extMin) * g_extPct / 100;
    ForPhysical(g_extMon, [&](HANDLE hp) { return SetMonitorBrightness(hp, v) != 0; });
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

void ShowOsd(HWND h) {
    KillTimer(h, TIMER_FADE);
    g_alpha = 255;
    SetLayeredWindowAttributes(h, 0, 255, LWA_ALPHA);

    HMONITOR mon = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfoW(mon, &mi);

    // top left, like the Windows 10 OSD
    HWND zOrder = g_cfg.overFullscreen ? HWND_TOPMOST : HWND_TOP;
    UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
    if (g_cfg.overFullscreen) flags |= SWP_NOOWNERZORDER;
    SetWindowPos(h, zOrder,
                 mi.rcWork.left + S(BASE_MARGIN),
                 mi.rcWork.top + S(BASE_MARGIN),
                 S(BASE_W), S(BASE_H), flags);
    InvalidateRect(h, nullptr, FALSE);
    SetTimer(h, TIMER_HIDE, g_cfg.timeoutMs, nullptr);
}

void ShowBrightness(HWND h, int level) {
    g_mode  = MODE_BRIGHTNESS;
    g_level = level;
    g_muted = false;
    ShowOsd(h);
}

// Hotkeys
void UnregisterKeys(HWND h) {
    for (int id = HK_VOL_UP; id <= HK_BR_DOWN; id++) UnregisterHotKey(h, id);
}

void RegisterKeys(HWND h) {
    UnregisterKeys(h);
    if (g_cfg.volKeys) {
        if (!RegisterHotKey(h, HK_VOL_UP,   0, VK_VOLUME_UP) ||
            !RegisterHotKey(h, HK_VOL_DOWN, 0, VK_VOLUME_DOWN) ||
            !RegisterHotKey(h, HK_VOL_MUTE, 0, VK_VOLUME_MUTE))
            Wh_Log(L"Volume hotkey registration failed (another app owns it?)");
    }
    if (g_cfg.extKeys) {
        if (!RegisterHotKey(h, HK_BR_UP,   MOD_CONTROL | MOD_ALT, VK_UP) ||
            !RegisterHotKey(h, HK_BR_DOWN, MOD_CONTROL | MOD_ALT, VK_DOWN))
            Wh_Log(L"Brightness hotkey registration failed");
    }
}

void VolumeKey(HWND h, int id) {
    g_lastVolTick = GetTickCount64();
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

void ExternalBrightnessKey(HWND h, int dir) {
    ULONGLONG now = GetTickCount64();
    POINT pt; GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

    // (re)read the current value when the monitor changed or the cache is old
    if (mon != g_extMon || g_extMax <= g_extMin || now - g_extLast > 5000) {
        DWORD mn = 0, cur = 0, mx = 0;
        bool ok = ForPhysical(mon, [&](HANDLE hp) {
            return GetMonitorBrightness(hp, &mn, &cur, &mx) && mx > mn;
        });
        if (!ok) {
            if (g_cfg.debug) Wh_Log(L"DDC/CI not available on this monitor");
            return;
        }
        g_extMon = mon; g_extMin = mn; g_extMax = mx;
        g_extPct = (int)((cur - mn) * 100 / (mx - mn));
    }

    g_extPct += dir * g_cfg.extStep;
    if (g_extPct < 0)   g_extPct = 0;
    if (g_extPct > 100) g_extPct = 100;
    g_extLast = now;
    g_lastBrTick = now;

    ShowBrightness(h, g_extPct);
    SetTimer(h, TIMER_APPLY, 60, nullptr);   // DDC/CI is slow: coalesce key repeats
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
            DWORD mn = 0, cur = 0, mx = 0;
            if (!ForPhysical(mon, [&](HANDLE hp) {
                    return GetMonitorBrightness(hp, &mn, &cur, &mx) && mx > mn;
                })) return;
            g_extMon = mon;
            g_extMin = mn;
            g_extMax = mx;
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
        RegisterKeys(h);
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
        }
        return 0;
    case WM_CAPTURECHANGED:
        g_dragging = false;
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
DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_dpi = GetDpiForSystem();

    WNDCLASSW wc{};
    wc.lpfnWndProc = OsdProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"WhWin10VolumeBrightnessOsd";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
        WS_EX_LAYERED,
        wc.lpszClassName, L"", WS_POPUP,
        0, 0, S(BASE_W), S(BASE_H),
        nullptr, nullptr, wc.hInstance, nullptr);
    if (hwnd) SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    g_osd = hwnd;

    if (hwnd) {
        RegisterKeys(hwnd);

        // The initial notification sent on registration must not show the flyout
        g_brArmed = GetTickCount64() + 1500;
        g_pn1 = RegisterPowerSettingNotification(hwnd, &kBrGuid1, DEVICE_NOTIFY_WINDOW_HANDLE);
        g_pn2 = RegisterPowerSettingNotification(hwnd, &kBrGuid2, DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    g_volCb = new VolCallback();
    g_devCb = new DeviceNotify();
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                   CLSCTX_ALL, IID_PPV_ARGS(&g_en)))) {
        g_en->RegisterEndpointNotificationCallback(g_devCb);
        BindEndpoint();
    } else {
        Wh_Log(L"MMDeviceEnumerator creation failed");
    }

    SetEvent(g_ready);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // cleanup
    g_osd = nullptr;
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
BOOL Wh_ModInit() {
    LoadSettings();

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    if (!g_thread) return FALSE;
    WaitForSingleObject(g_ready, 3000);

    Wh_SetFunctionHook((void*)ShowWindow,
                       (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig);
    Wh_SetFunctionHook((void*)SetWindowPos,
                       (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Orig);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    HWND osd = g_osd.load();
    if (osd) {
        PostMessageW(osd, WM_RELOAD, 0, 0);   // hotkeys must be (re)registered on the UI thread
        InvalidateRect(osd, nullptr, FALSE);
    }
}

void Wh_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) { CloseHandle(g_ready); g_ready = nullptr; }
}
