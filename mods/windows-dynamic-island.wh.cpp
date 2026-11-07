// ==WindhawkMod==
// @id              windows-dynamic-island
// @name            Windows Dynamic Island
// @description     Apple-style Dynamic Island for Windows 11. A small blank pill that expands into Clock, Calendar, Media and Quick Settings, in Black or Frosted Glass.
// @version         1.3.0
// @author          saahas-karthik
// @github          https://github.com/saahas-karthik
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -ldwmapi -lole32 -loleaut32 -luuid -lwlanapi -lshell32 -lshlwapi -lruntimeobject -DWINRT_NO_MODULE_LOCK
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Dynamic Island

A small blank pill sits at the top centre of your primary monitor
(solid black, or clear frosted glass, depending on the theme setting).

* **Hover the pill** - it grows into the icon bar (or click it if you turn
  "Expand on hover" off).
* **Click an icon** - the island grows into that panel. Click another icon and
  the content cross-fades inside the SAME island.
* **Click the active icon again** - back to the icon bar.
* **Move the mouse away** - the icon bar collapses to the pill instantly;
  an open panel collapses after the auto-collapse delay. Right-click collapses
  immediately.
* **Equalizer** - while any audio is playing, the collapsed pill shows tiny
  equalizer bars that move with the real output level.
* **Adaptive accent** - the accent colour (outline, highlights, sliders,
  today's date...) is taken from whatever is behind the island. Over a blue
  wallpaper or window it turns blue, over orange it turns orange. Neutral grey,
  black or white backgrounds give a soft silver accent. Any hue works. Turn it
  off in the settings to keep green.

| Icon                                  | Panel |
|---------------------------------------|-------|
| Clock                                 | Analog + digital clock, date |
| Calendar                              | Month view, prev/next (mouse wheel works too), today in green |
| Media                                 | Real now-playing info for ANY app |
| Brightness / Volume / Wi-Fi / Hotspot | Quick Settings |

## Media
Uses the same Windows media session that powers the system volume flyout
(`GlobalSystemMediaTransportControls`), so it works with Spotify, Apple Music,
YouTube Music, browsers, VLC, Windows Media Player and anything else that shows
up in the Windows media overlay.
* Real title, artist, album artwork and source app name
* Play/pause, previous and next control the actual session
* Live progress bar; click or drag it to seek (when the app supports seeking)
* Volume slider controls system volume

## Notes
* Brightness uses WMI (laptop panels). External monitors are not supported.
* Wi-Fi toggles the radio through the WLAN API.
* Mobile Hotspot opens Windows' hotspot settings page.
* The Glass theme draws its own blur (capture, blur, grade, sheen, grain), so
  the rounded edges stay perfectly smooth at every size. For that to work the
  island is hidden from screenshots and screen recordings while Glass is
  selected; use the Black theme if you need it to show up in captures. Adjust
  "Glass blur amount" to taste. The glass is brightness- and contrast-levelled,
  so it looks the same over any wallpaper or window; only the accent tint changes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: black
  $name: Theme
  $options:
  - black: Regular (Black)
  - glass: Frosted glass
- blurAmount: 6
  $name: Glass blur amount (1-10)
- topMargin: 8
  $name: Distance from top of screen (px)
- scalePct: 100
  $name: Size (%)
- hour24: false
  $name: 24-hour clock
- autoCollapse: 5
  $name: Auto-collapse open panels after mouse leaves (seconds, 0 = never)
- hoverExpand: true
  $name: Expand the pill on mouse hover
- dynamicAccent: true
  $name: Accent colour follows what is behind the island
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <map>
#include <string>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <objidl.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
using std::min;
using std::max;
#include <gdiplus.h>
#include <dwmapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <wlanapi.h>
#include <shellapi.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>

using namespace Gdiplus;
namespace wmc = winrt::Windows::Media::Control;

// ============================================================ settings
struct Settings {
    bool glass = false;
    int blur = 6;
    int topMargin = 8;
    int scalePct = 100;
    bool hour24 = false;
    int autoCollapse = 5;
    bool hoverExpand = true;
    bool dynamicAccent = true;
} S;

static void LoadSettings() {
    PCWSTR t = Wh_GetStringSetting(L"theme");
    S.glass = t && wcscmp(t, L"glass") == 0;
    Wh_FreeStringSetting(t);
    S.blur = Wh_GetIntSetting(L"blurAmount");
    if (S.blur < 1) S.blur = 1;
    if (S.blur > 10) S.blur = 10;
    S.topMargin = Wh_GetIntSetting(L"topMargin");
    S.scalePct = Wh_GetIntSetting(L"scalePct");
    if (S.scalePct < 60) S.scalePct = 60;
    if (S.scalePct > 250) S.scalePct = 250;
    S.hour24 = Wh_GetIntSetting(L"hour24") != 0;
    S.autoCollapse = Wh_GetIntSetting(L"autoCollapse");
    S.hoverExpand = Wh_GetIntSetting(L"hoverExpand") != 0;
    S.dynamicAccent = Wh_GetIntSetting(L"dynamicAccent") != 0;
}

// ============================================================ small helpers
static inline float Cl(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static inline float Mx(float a, float b) { return a > b ? a : b; }
static inline float Mn(float a, float b) { return a < b ? a : b; }
static inline float Ease(float t) { t = Cl(t, 0, 1); return t * t * (3 - 2 * t); }

// ============================================================ layout constants (logical px)
static const float LW = 560, LH = 440;   // whole overlay window
static const float TOP = 10;             // island top inside window
static const float HH = 54;              // icon bar height
static const float HW = 446;             // icon bar width
static const float CW = 120, CH = 32;    // collapsed pill

enum { T_NONE = -1, T_CLOCK = 0, T_CAL, T_MEDIA, T_BRIGHT, T_VOL, T_WIFI, T_HOTSPOT };
static int PanelOf(int tab) { return tab == T_CLOCK ? 0 : tab == T_CAL ? 1 : tab == T_MEDIA ? 2 : 3; }
static const float PW[4] = {420, 380, 470, 470};
static const float PH[4] = {196, 340, 254, 284};

enum {
    H_TAB = 100,       // +tab
    H_PREVMONTH = 200, H_NEXTMONTH,
    H_MPREV = 210, H_MPLAY, H_MNEXT,
    H_SL_BRIGHT = 220, H_SL_VOL, H_SL_MVOL, H_SL_SEEK,
    H_Q_BRIGHT = 230, H_Q_VOL, H_Q_WIFI, H_Q_HOTSPOT
};

// ============================================================ runtime state
static HWND g_hwnd = nullptr;
static HANDLE g_thread = nullptr;

static float sc = 1.0f;
static int Wd = 0, Hd = 0;
static BYTE* g_bits = nullptr;
static HBITMAP g_dib = nullptr;
static HDC g_memDC = nullptr;
static Bitmap* g_bmp = nullptr;
static ULONG_PTR g_gdipTok = 0;

static Graphics* gfx = nullptr;
static float gA = 1.0f;

static bool g_expanded = false;          // icon bar visible
static int g_tab = T_NONE;               // active icon (panel open)
static int g_panel = 0;
static int g_prevPanel = -1;
static float g_xfade = 1.0f;
static float curW = CW, curH = CH, velW = 0, velH = 0;
static int g_calOffset = 0;
static std::map<int, float> g_hov;
static int g_hoverId = 0;
static bool g_hoverIsland = false;
static bool g_suppressHover = false;     // after a manual collapse, don't re-expand until the pointer leaves
static ULONGLONG g_lastSample = 0;
static int g_drag = 0;
static float g_dragX = 0, g_dragW = 1, g_seekFrac = 0;
static bool g_dirty = true, g_tracking = false;
static ULONGLONG g_lastTick = 0, g_outsideSince = 0, g_lastPoll = 0;
static time_t g_lastSec = 0;
static UINT g_timerPeriod = 0;
// self-rendered glass: small DIB that receives the captured backdrop, plus a grain texture
static HBITMAP g_gDib = nullptr;
static HDC g_gDC = nullptr;
static BYTE* g_gBits = nullptr;
static int g_gW = 0, g_gH = 0;
static std::vector<BYTE> g_gTmp;
static Bitmap* g_noise = nullptr;
static ULONGLONG g_lastGlass = 0;

struct Hit { float x, y, w, h; int id; };
static std::vector<Hit> g_hits;
static void AddHit(float x, float y, float w, float h, int id) { g_hits.push_back({x, y, w, h, id}); }

// ============================================================ system data
static int g_volume = 40;
static bool g_muted = false;
static int g_bright = 60;
static bool g_brightKnown = false;
static bool g_wifiOn = true, g_wifiAvail = false;
static wchar_t g_ssid[64] = L"";

// ---- audio
static IAudioEndpointVolume* g_epv = nullptr;
// The Windhawk toolchain only forward-declares IAudioMeterInformation, so define it here (same vtable as the SDK).
struct IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};
static const GUID kIID_AudioMeterInformation = {0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};
static IAudioMeterInformation* g_meter = nullptr;   // real output level, drives the equalizer
static float g_eqLevel = 0, g_eqAlpha = 0, g_eqH[5] = {0, 0, 0, 0, 0};
static ULONGLONG g_lastDev = 0;
static float g_glassLum = -1;
static IAudioEndpointVolume* EPV() {
    if (g_epv) return g_epv;
    IMMDeviceEnumerator* en = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&en)) || !en)
        return nullptr;
    IMMDevice* dev = nullptr;
    en->GetDefaultAudioEndpoint(eRender, eConsole, &dev);
    if (dev) {
        dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&g_epv);
        dev->Activate(kIID_AudioMeterInformation, CLSCTX_ALL, nullptr, (void**)&g_meter);
        dev->Release();
    }
    en->Release();
    return g_epv;
}
static void AudioPoll() {
    IAudioEndpointVolume* e = EPV();
    if (!e) return;
    float v = 0; BOOL m = FALSE;
    if (SUCCEEDED(e->GetMasterVolumeLevelScalar(&v))) g_volume = (int)(v * 100 + 0.5f);
    if (SUCCEEDED(e->GetMute(&m))) g_muted = m != FALSE;
}
static void SetVolumePct(int p) {
    p = (int)Cl((float)p, 0, 100);
    g_volume = p;
    if (IAudioEndpointVolume* e = EPV()) {
        e->SetMasterVolumeLevelScalar(p / 100.0f, nullptr);
        if (p > 0 && g_muted) { e->SetMute(FALSE, nullptr); g_muted = false; }
    }
}
static void ToggleMute() {
    if (IAudioEndpointVolume* e = EPV()) { g_muted = !g_muted; e->SetMute(g_muted, nullptr); }
}
static void AudioRelease() {
    if (g_epv) { g_epv->Release(); g_epv = nullptr; }
    if (g_meter) { g_meter->Release(); g_meter = nullptr; }
}
static float PeakNow() {
    EPV();
    float p = 0;
    if (g_meter && SUCCEEDED(g_meter->GetPeakValue(&p))) return p;
    return 0;
}

// ---- brightness (WMI through hidden powershell, throttled)
static int g_brightPending = -1, g_brightApplied = -2;
static HANDLE g_brightProc = nullptr;
static void PumpBrightness() {
    if (g_brightProc) {
        if (WaitForSingleObject(g_brightProc, 0) == WAIT_OBJECT_0) { CloseHandle(g_brightProc); g_brightProc = nullptr; }
        else return;
    }
    if (g_brightPending >= 0 && g_brightPending != g_brightApplied) {
        wchar_t cmd[400];
        swprintf(cmd, 400,
                 L"powershell.exe -NoProfile -NonInteractive -WindowStyle Hidden -Command "
                 L"\"(Get-CimInstance -Namespace root/WMI -ClassName WmiMonitorBrightnessMethods).WmiSetBrightness(1,%d)\"",
                 g_brightPending);
        STARTUPINFOW si = {sizeof(si)};
        PROCESS_INFORMATION pi = {};
        g_brightApplied = g_brightPending;
        if (CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            CloseHandle(pi.hThread);
            g_brightProc = pi.hProcess;
        }
    }
}
static DWORD WINAPI ReadBrightnessThread(LPVOID) {
    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, TRUE};
    HANDLE rd = nullptr, wr = nullptr;
    if (!CreatePipe(&rd, &wr, &sa, 0)) return 0;
    SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);
    wchar_t cmd[] = L"powershell.exe -NoProfile -NonInteractive -Command "
                    L"\"(Get-CimInstance -Namespace root/WMI -ClassName WmiMonitorBrightness).CurrentBrightness\"";
    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = wr; si.hStdError = wr;
    PROCESS_INFORMATION pi = {};
    if (CreateProcessW(nullptr, cmd, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        CloseHandle(wr); wr = nullptr;
        WaitForSingleObject(pi.hProcess, 8000);
        char buf[64] = {};
        DWORD n = 0;
        ReadFile(rd, buf, sizeof(buf) - 1, &n, nullptr);
        int v = atoi(buf);
        if (v > 0 || (n > 0 && buf[0] == '0')) { g_bright = (int)Cl((float)v, 0, 100); g_brightKnown = true; g_dirty = true; }
        CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    }
    if (wr) CloseHandle(wr);
    CloseHandle(rd);
    return 0;
}

// ---- Wi-Fi (WLAN API)
struct WlanSession {
    HANDLE h = nullptr; GUID guid = {}; bool ok = false; PWLAN_INTERFACE_INFO_LIST list = nullptr;
    WlanSession() {
        DWORD ver;
        if (WlanOpenHandle(2, nullptr, &ver, &h) != ERROR_SUCCESS) return;
        if (WlanEnumInterfaces(h, nullptr, &list) == ERROR_SUCCESS && list && list->dwNumberOfItems > 0) {
            guid = list->InterfaceInfo[0].InterfaceGuid; ok = true;
        }
    }
    ~WlanSession() {
        if (list) WlanFreeMemory(list);
        if (h) WlanCloseHandle(h, nullptr);
    }
};
static void WifiPoll() {
    WlanSession w;
    g_wifiAvail = w.ok;
    g_ssid[0] = 0;
    if (!w.ok) return;
    DWORD sz = 0;
    PWLAN_RADIO_STATE rs = nullptr;
    if (WlanQueryInterface(w.h, &w.guid, wlan_intf_opcode_radio_state, nullptr, &sz, (PVOID*)&rs, nullptr) == ERROR_SUCCESS && rs) {
        if (rs->dwNumberOfPhys > 0)
            g_wifiOn = rs->PhyRadioState[0].dot11SoftwareRadioState == dot11_radio_state_on &&
                       rs->PhyRadioState[0].dot11HardwareRadioState == dot11_radio_state_on;
        WlanFreeMemory(rs);
    }
    PWLAN_CONNECTION_ATTRIBUTES ca = nullptr;
    if (WlanQueryInterface(w.h, &w.guid, wlan_intf_opcode_current_connection, nullptr, &sz, (PVOID*)&ca, nullptr) == ERROR_SUCCESS && ca) {
        if (ca->isState == wlan_interface_state_connected) {
            auto& s = ca->wlanAssociationAttributes.dot11Ssid;
            char tmp[40] = {};
            memcpy(tmp, s.ucSSID, s.uSSIDLength < 32 ? s.uSSIDLength : 32);
            MultiByteToWideChar(CP_UTF8, 0, tmp, -1, g_ssid, 63);
        }
        WlanFreeMemory(ca);
    }
}
static void WifiSet(bool on) {
    WlanSession w;
    if (!w.ok) return;
    WLAN_PHY_RADIO_STATE s = {};
    s.dwPhyIndex = 0;
    s.dot11SoftwareRadioState = on ? dot11_radio_state_on : dot11_radio_state_off;
    s.dot11HardwareRadioState = dot11_radio_state_on;
    WlanSetInterface(w.h, &w.guid, wlan_intf_opcode_radio_state, sizeof(s), &s, nullptr);
    g_wifiOn = on;
}

// ============================================================ media (Windows media session)
struct MediaInfo {
    bool has = false, playing = false, canSeek = false, canNext = true, canPrev = true;
    std::wstring title, artist, app;
    long long posMs = 0, durMs = 0;
    ULONGLONG tick = 0;                  // GetTickCount64 when posMs was captured
};
static MediaInfo g_mi;
static std::vector<BYTE> g_thumbBytes;
static volatile LONG g_thumbGen = 0;
static CRITICAL_SECTION g_mcs;
static HANDLE g_mediaEvt = nullptr;
static HANDLE g_mediaThread = nullptr;
static volatile bool g_mediaRun = false;
static std::atomic<int> g_cmd{0};        // 1 toggle, 2 next, 3 previous
static std::atomic<long long> g_seekReq{-1};

static MediaInfo MediaSnap() {
    EnterCriticalSection(&g_mcs);
    MediaInfo m = g_mi;
    LeaveCriticalSection(&g_mcs);
    return m;
}
static long long CurPosMs(const MediaInfo& m) {
    long long p = m.posMs;
    if (m.playing && m.tick) p += (long long)(GetTickCount64() - m.tick);
    if (m.durMs > 0 && p > m.durMs) p = m.durMs;
    return p < 0 ? 0 : p;
}
static void MediaKey(WORD vk) {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD; in[0].ki.wVk = vk;
    in[1] = in[0]; in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}
static void MediaWake() { if (g_mediaEvt) SetEvent(g_mediaEvt); }

static std::wstring PrettyApp(std::wstring id) {
    auto endsExe = [](const std::wstring& s) { return s.size() > 4 && _wcsicmp(s.c_str() + s.size() - 4, L".exe") == 0; };
    size_t b = id.find(L'!');
    if (b != std::wstring::npos && b + 1 < id.size()) id = id.substr(b + 1);
    else if (endsExe(id)) id.resize(id.size() - 4);
    else {
        size_t d = id.rfind(L'.');
        if (d != std::wstring::npos && d + 1 < id.size()) id = id.substr(d + 1);
    }
    if (endsExe(id)) id.resize(id.size() - 4);
    if (!id.empty()) id[0] = towupper(id[0]);
    return id;
}

static DWORD WINAPI MediaThread(LPVOID) {
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) {}
    try {
        auto mgr = wmc::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        std::wstring lastKey;
        int thumbTries = 0;
        bool haveThumb = false;
        while (g_mediaRun) {
            try {
                auto ses = mgr.GetCurrentSession();
                if (ses) {
                    int c = g_cmd.exchange(0);
                    if (c == 1) ses.TryTogglePlayPauseAsync().get();
                    else if (c == 2) ses.TrySkipNextAsync().get();
                    else if (c == 3) ses.TrySkipPreviousAsync().get();
                    long long sk = g_seekReq.exchange(-1);
                    if (sk >= 0) ses.TryChangePlaybackPositionAsync(sk * 10000).get();

                    auto pb = ses.GetPlaybackInfo();
                    auto tl = ses.GetTimelineProperties();
                    auto props = ses.TryGetMediaPropertiesAsync().get();

                    MediaInfo ni;
                    ni.has = true;
                    ni.playing = pb.PlaybackStatus() == wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                    auto ctl = pb.Controls();
                    ni.canSeek = ctl.IsPlaybackPositionEnabled();
                    ni.canNext = ctl.IsNextEnabled();
                    ni.canPrev = ctl.IsPreviousEnabled();
                    ni.title = props.Title().c_str();
                    ni.artist = props.Artist().c_str();
                    ni.app = PrettyApp(ses.SourceAppUserModelId().c_str());

                    using namespace std::chrono;
                    long long dur = duration_cast<milliseconds>(tl.EndTime() - tl.StartTime()).count();
                    long long pos = duration_cast<milliseconds>(tl.Position() - tl.StartTime()).count();
                    if (ni.playing) {
                        long long age = duration_cast<milliseconds>(winrt::clock::now() - tl.LastUpdatedTime()).count();
                        if (age > 0 && age < 6LL * 3600 * 1000) pos += age;
                    }
                    if (dur < 0) dur = 0;
                    if (dur > 0 && pos > dur) pos = dur;
                    if (pos < 0) pos = 0;
                    ni.durMs = dur; ni.posMs = pos; ni.tick = GetTickCount64();

                    std::wstring key = ni.title + L"\x1f" + ni.artist + L"\x1f" + ni.app;
                    bool keyChanged = key != lastKey;
                    if (keyChanged) {
                        lastKey = key; thumbTries = 0; haveThumb = false;
                        EnterCriticalSection(&g_mcs);
                        g_thumbBytes.clear();
                        InterlockedIncrement(&g_thumbGen);
                        LeaveCriticalSection(&g_mcs);
                    }
                    if (!haveThumb && thumbTries < 8) {
                        thumbTries++;
                        std::vector<BYTE> bytes;
                        try {
                            auto ref = props.Thumbnail();
                            if (ref) {
                                auto st = ref.OpenReadAsync().get();
                                uint32_t size = (uint32_t)st.Size();
                                if (size > 0 && size < 8u * 1024 * 1024) {
                                    winrt::Windows::Storage::Streams::DataReader rd(st.GetInputStreamAt(0));
                                    rd.LoadAsync(size).get();
                                    bytes.resize(size);
                                    rd.ReadBytes(winrt::array_view<uint8_t>(bytes.data(), bytes.data() + size));
                                }
                            }
                        } catch (...) {}
                        if (!bytes.empty()) {
                            haveThumb = true;
                            EnterCriticalSection(&g_mcs);
                            g_thumbBytes = std::move(bytes);
                            InterlockedIncrement(&g_thumbGen);
                            LeaveCriticalSection(&g_mcs);
                        }
                    }

                    EnterCriticalSection(&g_mcs);
                    g_mi = ni;
                    LeaveCriticalSection(&g_mcs);
                } else {
                    EnterCriticalSection(&g_mcs);
                    if (g_mi.has) {
                        g_mi = MediaInfo();
                        g_thumbBytes.clear();
                        InterlockedIncrement(&g_thumbGen);
                    }
                    LeaveCriticalSection(&g_mcs);
                    lastKey.clear();
                    g_cmd = 0; g_seekReq = -1;
                }
            } catch (...) {
                // session vanished mid-query; try again on the next loop
            }
            WaitForSingleObject(g_mediaEvt, 450);
        }
    } catch (...) {}
    try { winrt::uninit_apartment(); } catch (...) {}
    return 0;
}

// artwork bitmap, built on the UI thread from the bytes the media thread fetched
static Bitmap* g_thumb = nullptr;
static LONG g_uiThumbGen = 0;
static void SyncThumb() {
    if (g_thumbGen == g_uiThumbGen) return;
    std::vector<BYTE> bytes;
    LONG gen;
    EnterCriticalSection(&g_mcs);
    bytes = g_thumbBytes; gen = g_thumbGen;
    LeaveCriticalSection(&g_mcs);
    g_uiThumbGen = gen;
    delete g_thumb; g_thumb = nullptr;
    if (bytes.empty()) return;
    IStream* st = SHCreateMemStream(bytes.data(), (UINT)bytes.size());
    if (!st) return;
    {
        Bitmap src(st);
        if (src.GetLastStatus() == Ok && src.GetWidth() > 0 && src.GetHeight() > 0) {
            const int N = 256;
            Bitmap* dst = new Bitmap(N, N, PixelFormat32bppPARGB);
            Graphics g(dst);
            g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            g.SetPixelOffsetMode(PixelOffsetModeHalf);
            int w = (int)src.GetWidth(), h = (int)src.GetHeight();
            int s = w < h ? w : h;
            g.DrawImage(&src, RectF(0, 0, (REAL)N, (REAL)N), (REAL)((w - s) / 2), (REAL)((h - s) / 2), (REAL)s, (REAL)s, UnitPixel);
            g_thumb = dst;
        }
    }
    st->Release();
}

// ============================================================ drawing primitives
static Color Col(int r, int g, int b, int a = 255) { return Color((BYTE)Cl(a * gA, 0, 255), (BYTE)r, (BYTE)g, (BYTE)b); }
// accent colour: g_at is the target (from the backdrop), g_af eases toward it, g_ar/g_ag/g_ab are what gets drawn
static float g_af[3] = {52, 230, 100}, g_at[3] = {52, 230, 100};
static int g_ar = 52, g_ag = 230, g_ab = 100;
#define GREEN g_ar, g_ag, g_ab

struct RPen : Pen {
    RPen(Color c, float w) : Pen(c, w) { SetStartCap(LineCapRound); SetEndCap(LineCapRound); SetLineJoin(LineJoinRound); }
};

static void RR(GraphicsPath& p, float x, float y, float w, float h, float r) {
    r = Mn(r, Mn(w, h) / 2); float d = r * 2;
    p.AddArc(x, y, d, d, 180, 90); p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90); p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}
static void FillRR(float x, float y, float w, float h, float r, Color c) {
    GraphicsPath p; RR(p, x, y, w, h, r); SolidBrush b(c); gfx->FillPath(&b, &p);
}
static void StrokeRR(float x, float y, float w, float h, float r, Color c, float wd) {
    GraphicsPath p; RR(p, x, y, w, h, r); Pen pen(c, wd); gfx->DrawPath(&pen, &p);
}
static void Disc(float cx, float cy, float r, Color c) { SolidBrush b(c); gfx->FillEllipse(&b, cx - r, cy - r, 2 * r, 2 * r); }
static void Ring(float cx, float cy, float r, Color c, float wd) { Pen p(c, wd); gfx->DrawEllipse(&p, cx - r, cy - r, 2 * r, 2 * r); }

static FontFamily* g_ff = nullptr;
static std::map<int, Font*> g_fonts;
static Font* GetFont(float size, bool bold) {
    int key = (int)(size * 10) * 2 + (bold ? 1 : 0);
    auto it = g_fonts.find(key);
    if (it != g_fonts.end()) return it->second;
    Font* f = new Font(g_ff, size, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    g_fonts[key] = f;
    return f;
}
static void Txt(const std::wstring& s, float x, float y, float size, Color c, int align = 0, bool bold = false) {
    StringFormat sf(StringFormatFlagsNoWrap | StringFormatFlagsNoClip);
    sf.SetAlignment(align == 0 ? StringAlignmentNear : align == 1 ? StringAlignmentCenter : StringAlignmentFar);
    float rx = align == 0 ? x : (align == 1 ? x - 300 : x - 600);
    RectF r(rx, y, 600, size * 1.6f);
    SolidBrush b(c);
    gfx->DrawString(s.c_str(), -1, GetFont(size, bold), r, &sf, &b);
}
// left-aligned text that ends in an ellipsis when it doesn't fit maxW
static void TxtFit(const std::wstring& s, float x, float y, float size, Color c, float maxW, bool bold = false) {
    StringFormat sf(StringFormatFlagsNoWrap);
    sf.SetTrimming(StringTrimmingEllipsisCharacter);
    RectF r(x, y, maxW, size * 1.45f);
    SolidBrush b(c);
    gfx->DrawString(s.c_str(), -1, GetFont(size, bold), r, &sf, &b);
}
static std::wstring FmtTime(long long ms) {
    long long s = ms / 1000;
    wchar_t b[32];
    if (s >= 3600) swprintf(b, 32, L"%lld:%02lld:%02lld", s / 3600, (s / 60) % 60, s % 60);
    else swprintf(b, 32, L"%lld:%02lld", s / 60, s % 60);
    return b;
}

// ---- icons (drawn around cx,cy in a box of size s)
static void IClock(float cx, float cy, float s, Color c) {
    RPen p(c, s * 0.085f);
    gfx->DrawEllipse(&p, cx - s * 0.46f, cy - s * 0.46f, s * 0.92f, s * 0.92f);
    gfx->DrawLine(&p, cx, cy, cx, cy - s * 0.27f);
    gfx->DrawLine(&p, cx, cy, cx + s * 0.19f, cy + s * 0.1f);
}
static void ICalendar(float cx, float cy, float s, Color c) {
    RPen p(c, s * 0.085f);
    StrokeRR(cx - s * 0.44f, cy - s * 0.4f, s * 0.88f, s * 0.84f, s * 0.16f, c, s * 0.085f);
    gfx->DrawLine(&p, cx - s * 0.44f, cy - s * 0.12f, cx + s * 0.44f, cy - s * 0.12f);
    gfx->DrawLine(&p, cx - s * 0.22f, cy - s * 0.5f, cx - s * 0.22f, cy - s * 0.3f);
    gfx->DrawLine(&p, cx + s * 0.22f, cy - s * 0.5f, cx + s * 0.22f, cy - s * 0.3f);
    for (int r = 0; r < 3; r++) for (int k = 0; k < 4; k++)
        Disc(cx - s * 0.27f + k * s * 0.18f, cy + s * 0.04f + r * s * 0.15f, s * 0.04f, c);
}
// album art: the real artwork if the app provided any, else a neutral placeholder
static void DrawArt(float x, float y, float s, float r) {
    GraphicsPath clip; RR(clip, x, y, s, s, r);
    gfx->SetClip(&clip, CombineModeIntersect);
    if (g_thumb) {
        ImageAttributes ia;
        ColorMatrix cm = {{{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, gA, 0}, {0, 0, 0, 0, 1}}};
        ia.SetColorMatrix(&cm);
        gfx->DrawImage(g_thumb, RectF(x, y, s, s), 0, 0, 256, 256, UnitPixel, &ia);
    } else {
        LinearGradientBrush bg(PointF(x, y), PointF(x, y + s), Col(34, 52, 42), Col(10, 16, 13));
        gfx->FillPath(&bg, &clip);
        RPen p(Col(GREEN, 220), s * 0.07f);
        float cx = x + s / 2, cy = y + s / 2;
        gfx->DrawLine(&p, cx + s * 0.12f, cy - s * 0.22f, cx + s * 0.12f, cy + s * 0.14f);
        gfx->DrawLine(&p, cx + s * 0.12f, cy - s * 0.22f, cx + s * 0.26f, cy - s * 0.14f);
        Disc(cx + s * 0.02f, cy + s * 0.15f, s * 0.11f, Col(GREEN, 220));
    }
    gfx->ResetClip();
    StrokeRR(x, y, s, s, r, Col(255, 255, 255, 26), 1.0f);
}
static void ISun(float cx, float cy, float s, Color c) {
    RPen p(c, s * 0.085f);
    gfx->DrawEllipse(&p, cx - s * 0.19f, cy - s * 0.19f, s * 0.38f, s * 0.38f);
    for (int i = 0; i < 8; i++) {
        float a = i * 3.14159265f / 4;
        gfx->DrawLine(&p, cx + cosf(a) * s * 0.32f, cy + sinf(a) * s * 0.32f, cx + cosf(a) * s * 0.46f, cy + sinf(a) * s * 0.46f);
    }
}
static void ISpeaker(float cx, float cy, float s, Color c, bool muted) {
    PointF pts[] = {{cx - s * 0.46f, cy - s * 0.15f}, {cx - s * 0.26f, cy - s * 0.15f}, {cx - s * 0.02f, cy - s * 0.38f},
                    {cx - s * 0.02f, cy + s * 0.38f}, {cx - s * 0.26f, cy + s * 0.15f}, {cx - s * 0.46f, cy + s * 0.15f}};
    SolidBrush b(c); gfx->FillPolygon(&b, pts, 6);
    RPen p(c, s * 0.085f);
    if (muted) {
        gfx->DrawLine(&p, cx + s * 0.12f, cy - s * 0.18f, cx + s * 0.44f, cy + s * 0.18f);
        gfx->DrawLine(&p, cx + s * 0.44f, cy - s * 0.18f, cx + s * 0.12f, cy + s * 0.18f);
    } else {
        gfx->DrawArc(&p, cx - s * 0.18f, cy - s * 0.2f, s * 0.4f, s * 0.4f, -50, 100);
        gfx->DrawArc(&p, cx - s * 0.3f, cy - s * 0.34f, s * 0.68f, s * 0.68f, -50, 100);
    }
}
static void IWifi(float cx, float cy, float s, Color c, bool off) {
    RPen p(c, s * 0.09f);
    float py = cy + s * 0.3f;
    for (int i = 1; i <= 3; i++) {
        float r = s * (0.17f * i + 0.03f);
        gfx->DrawArc(&p, cx - r, py - r, 2 * r, 2 * r, 228, 84);
    }
    Disc(cx, py, s * 0.055f, c);
    if (off) gfx->DrawLine(&p, cx - s * 0.42f, cy - s * 0.38f, cx + s * 0.42f, cy + s * 0.42f);
}
static void IHotspot(float cx, float cy, float s, Color c) {
    RPen p(c, s * 0.085f);
    Disc(cx, cy - s * 0.04f, s * 0.07f, c);
    gfx->DrawLine(&p, cx, cy + s * 0.02f, cx, cy + s * 0.44f);
    for (int i = 1; i <= 2; i++) {
        float r = s * 0.2f * i;
        gfx->DrawArc(&p, cx - r, cy - s * 0.04f - r, 2 * r, 2 * r, 140, 80);
        gfx->DrawArc(&p, cx - r, cy - s * 0.04f - r, 2 * r, 2 * r, -40, 80);
    }
}
static void ITri(float cx, float cy, float s, bool right, Color c) {
    float d = right ? 1.0f : -1.0f;
    PointF t[] = {{cx - d * s * 0.5f, cy - s * 0.5f}, {cx - d * s * 0.5f, cy + s * 0.5f}, {cx + d * s * 0.5f, cy}};
    SolidBrush b(c); gfx->FillPolygon(&b, t, 3);
}
static void ISkip(float cx, float cy, float s, bool next, Color c) {
    ITri(cx + (next ? -s * 0.3f : s * 0.3f), cy, s * 0.62f, next, c);
    ITri(cx + (next ? s * 0.3f : -s * 0.3f), cy, s * 0.62f, next, c);
}
static void IPlayPause(float cx, float cy, float s, bool playing, Color c) {
    if (playing) { FillRR(cx - s * 0.36f, cy - s * 0.42f, s * 0.26f, s * 0.84f, s * 0.08f, c); FillRR(cx + s * 0.10f, cy - s * 0.42f, s * 0.26f, s * 0.84f, s * 0.08f, c); }
    else ITri(cx + s * 0.04f, cy, s * 0.8f, true, c);
}
static void IChevron(float cx, float cy, float s, bool right, Color c) {
    RPen p(c, 2.0f);
    float d = right ? 1.f : -1.f;
    gfx->DrawLine(&p, cx - d * s * 0.2f, cy - s * 0.35f, cx + d * s * 0.2f, cy);
    gfx->DrawLine(&p, cx + d * s * 0.2f, cy, cx - d * s * 0.2f, cy + s * 0.35f);
}

static float Hov(int id) { auto it = g_hov.find(id); return it == g_hov.end() ? (g_hov[id] = 0) : it->second; }

// ---- slider (track geometry is recovered from the hit rect: x+8 .. x+w-8)
static void Slider(float x, float y, float w, int pct, int id) {
    float f = Cl(pct / 100.0f, 0, 1);
    FillRR(x, y - 3, w, 6, 3, Col(255, 255, 255, S.glass ? 60 : 38));
    FillRR(x, y - 3, Mx(6.0f, w * f), 6, 3, Col(GREEN));
    float h = Hov(id);
    Disc(x + w * f, y + 1, 11, Col(0, 0, 0, 40));
    Disc(x + w * f, y, 8.5f + h * 1.5f, Col(255, 255, 255));
    AddHit(x - 8, y - 16, w + 16, 32, id);
}

// ============================================================ panels
static void DrawClockPanel(float px, float py, float pw, float ph) {
    time_t t = time(nullptr); tm* lt = localtime(&t);
    float cx = px + 112, cy = py + ph / 2, R = 74;
    Ring(cx, cy, R, Col(255, 255, 255, 36), 1.4f);
    for (int i = 0; i < 60; i++) {
        float a = i * 3.14159265f / 30 - 3.14159265f / 2;
        bool major = i % 5 == 0;
        float r1 = R - (major ? 12 : 7), r2 = R - 3;
        RPen p(Col(255, 255, 255, major ? 230 : 90), major ? 2.6f : 1.2f);
        gfx->DrawLine(&p, cx + cosf(a) * r1, cy + sinf(a) * r1, cx + cosf(a) * r2, cy + sinf(a) * r2);
    }
    float hh = (lt->tm_hour % 12 + lt->tm_min / 60.0f) * 3.14159265f / 6 - 3.14159265f / 2;
    float mm = (lt->tm_min + lt->tm_sec / 60.0f) * 3.14159265f / 30 - 3.14159265f / 2;
    float ss = lt->tm_sec * 3.14159265f / 30 - 3.14159265f / 2;
    { RPen p(Col(255, 255, 255), 4.4f); gfx->DrawLine(&p, cx, cy, cx + cosf(hh) * R * 0.5f, cy + sinf(hh) * R * 0.5f); }
    { RPen p(Col(255, 255, 255), 3.2f); gfx->DrawLine(&p, cx, cy, cx + cosf(mm) * R * 0.74f, cy + sinf(mm) * R * 0.74f); }
    { RPen p(Col(GREEN), 1.8f); gfx->DrawLine(&p, cx - cosf(ss) * 14, cy - sinf(ss) * 14, cx + cosf(ss) * R * 0.82f, cy + sinf(ss) * R * 0.82f); }
    Disc(cx, cy, 4.5f, Col(GREEN)); Disc(cx, cy, 1.8f, Col(0, 0, 0));

    wchar_t buf[64];
    int h = S.hour24 ? lt->tm_hour : (lt->tm_hour % 12 == 0 ? 12 : lt->tm_hour % 12);
    if (S.hour24) swprintf(buf, 64, L"%02d:%02d", h, lt->tm_min); else swprintf(buf, 64, L"%d:%02d", h, lt->tm_min);
    Txt(buf, px + 214, py + 44, 58, Col(255, 255, 255), 0, true);
    if (!S.hour24) {   // AM/PM as a small chip on the weekday row, clear of the big digits
        float cw2 = 42, ch2 = 24, cx2 = px + pw - 24 - cw2, cy2 = py + 120;
        FillRR(cx2, cy2, cw2, ch2, ch2 / 2, Col(GREEN, 38));
        StrokeRR(cx2, cy2, cw2, ch2, ch2 / 2, Col(GREEN, 110), 1.0f);
        Txt(lt->tm_hour >= 12 ? L"PM" : L"AM", cx2 + cw2 / 2, cy2 + 4.5f, 13, Col(GREEN), 1, true);
    }
    static const wchar_t* wd[] = {L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday"};
    static const wchar_t* mn[] = {L"January", L"February", L"March", L"April", L"May", L"June", L"July", L"August", L"September", L"October", L"November", L"December"};
    Txt(wd[lt->tm_wday], px + 216, py + 118, 19, Col(255, 255, 255), 0, true);
    swprintf(buf, 64, L"%ls %d, %d", mn[lt->tm_mon], lt->tm_mday, lt->tm_year + 1900);
    Txt(buf, px + 216, py + 144, 15, Col(170, 180, 174));
}

static int DaysIn(int y, int m) {
    static const int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 1 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return d[m];
}
static void DrawCalendarPanel(float px, float py, float pw, float ph) {
    time_t t = time(nullptr); tm now = *localtime(&t);
    int mIdx = now.tm_year * 12 + now.tm_mon + g_calOffset;
    int y = mIdx / 12 + 1900, m = mIdx % 12;
    static const wchar_t* mn[] = {L"January", L"February", L"March", L"April", L"May", L"June", L"July", L"August", L"September", L"October", L"November", L"December"};
    wchar_t buf[64];
    swprintf(buf, 64, L"%ls %d", mn[m], y);
    ICalendar(px + 34, py + 30, 22, Col(GREEN));
    Txt(buf, px + 54, py + 20, 17, Col(255, 255, 255), 0, true);
    for (int i = 0; i < 2; i++) {
        int id = i == 0 ? H_PREVMONTH : H_NEXTMONTH;
        float bx = px + pw - 76 + i * 40, by = py + 30, h = Hov(id);
        Disc(bx, by, 16, Col(255, 255, 255, (int)(34 * h)));
        IChevron(bx, by, 18, i == 1, Col(255, 255, 255, 200 + (int)(55 * h)));
        AddHit(bx - 18, by - 18, 36, 36, id);
    }
    float cw = (pw - 48) / 7;
    static const wchar_t* dn[] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};
    for (int i = 0; i < 7; i++) Txt(dn[i], px + 24 + (i + 0.5f) * cw, py + 62, 13, Col(150, 160, 154), 1, true);
    tm first = {}; first.tm_year = y - 1900; first.tm_mon = m; first.tm_mday = 1; mktime(&first);
    int dim = DaysIn(y, m);
    for (int d = 1; d <= dim; d++) {
        int idx = first.tm_wday + d - 1;
        int col = idx % 7, row = idx / 7;
        float cx = px + 24 + (col + 0.5f) * cw, cy = py + 100 + row * 38;
        bool today = g_calOffset == 0 && d == now.tm_mday;
        if (today) Disc(cx, cy, 16, Col(GREEN));
        swprintf(buf, 64, L"%d", d);
        Txt(buf, cx, cy - 9, 15, today ? Col(0, 0, 0) : Col(255, 255, 255, col == 0 || col == 6 ? 190 : 255), 1, today);
    }
}

static void DrawMediaPanel(float px, float py, float pw, float ph) {
    MediaInfo m = MediaSnap();
    DrawArt(px + 24, py + 20, 112, 16);

    float tx = px + 154, tw = pw - 154 - 24;
    Color grey = Col(160, 170, 164);
    if (!m.has) {
        TxtFit(L"Nothing playing", tx, py + 26, 21, Col(255, 255, 255), tw, true);
        TxtFit(L"Start music or a video in any app", tx, py + 58, 13.5f, grey, tw);
    } else {
        TxtFit(m.title.empty() ? L"Unknown title" : m.title, tx, py + 22, 22, Col(255, 255, 255), tw, true);
        TxtFit(m.artist.empty() ? m.app : m.artist, tx, py + 54, 15, Col(GREEN), tw);
        if (!m.artist.empty() && !m.app.empty()) TxtFit(m.app, tx, py + 80, 12, grey, tw);
    }

    // progress / seek bar
    float bx = tx, bw = tw, by = py + 112;
    bool seeking = g_drag == H_SL_SEEK && m.durMs > 0;
    long long pos = seeking ? (long long)(g_seekFrac * m.durMs) : CurPosMs(m);
    float frac = m.durMs > 0 ? Cl((float)pos / (float)m.durMs, 0, 1) : 0;
    float hs = Hov(H_SL_SEEK);
    float th = 5 + 2 * hs;
    FillRR(bx, by - th / 2, bw, th, th / 2, Col(255, 255, 255, 45));
    if (m.durMs > 0) {
        FillRR(bx, by - th / 2, Mx(th, bw * frac), th, th / 2, Col(GREEN));
        Disc(bx + bw * frac, by, 5.5f + 2 * hs, Col(255, 255, 255));
    }
    if (m.has && m.durMs > 0 && m.canSeek) AddHit(bx - 8, by - 16, bw + 16, 32, H_SL_SEEK);
    Txt(m.has ? FmtTime(pos) : L"0:00", bx, by + 10, 12, grey);
    Txt(m.durMs > 0 ? FmtTime(m.durMs) : L"", bx + bw, by + 10, 12, grey, 2);

    // transport controls
    float cx = px + pw / 2, cy = py + 176;
    float hp = Hov(H_MPREV), hn = Hov(H_MNEXT), hm = Hov(H_MPLAY);
    int ap = (!m.has || m.canPrev) ? 255 : 90, an = (!m.has || m.canNext) ? 255 : 90;
    ISkip(cx - 84, cy, 24 * (1 + 0.14f * hp), false, Col(255, 255, 255, ap)); AddHit(cx - 112, cy - 24, 56, 48, H_MPREV);
    ISkip(cx + 84, cy, 24 * (1 + 0.14f * hn), true, Col(255, 255, 255, an)); AddHit(cx + 56, cy - 24, 56, 48, H_MNEXT);
    Disc(cx, cy, 27 + hm * 2, Col(255, 255, 255, 30 + (int)(26 * hm)));
    IPlayPause(cx, cy, 24, m.playing, Col(255, 255, 255)); AddHit(cx - 30, cy - 30, 60, 60, H_MPLAY);

    // system volume
    float vy = py + 222;
    ISpeaker(px + 38, vy, 20, Col(255, 255, 255, 200), g_muted);
    Slider(px + 66, vy, pw - 66 - 28, g_muted ? 0 : g_volume, H_SL_MVOL);
}

static void QBtn(float cx, float cy, int id, bool focus) {
    float h = Hov(id);
    Disc(cx, cy, 31 + h * 1.5f, S.glass ? Col(255, 255, 255, 30 + (int)(22 * h)) : Col(30, 34, 32, 255));
    if (!S.glass) Disc(cx, cy, 31 + h * 1.5f, Col(255, 255, 255, (int)(14 * h)));
    if (focus) Ring(cx, cy, 33.5f, Col(GREEN, 170), 1.6f);
    AddHit(cx - 33, cy - 33, 66, 66, id);
}
static void DrawQuickPanel(float px, float py, float pw, float ph) {
    float colw = pw / 4, cy = py + 50;
    float c0 = px + colw * 0.5f, c1 = px + colw * 1.5f, c2 = px + colw * 2.5f, c3 = px + colw * 3.5f;
    wchar_t buf[96];
    QBtn(c0, cy, H_Q_BRIGHT, g_tab == T_BRIGHT);
    ISun(c0, cy, 28 * (1 + 0.1f * Hov(H_Q_BRIGHT)), Col(255, 255, 255));
    Txt(L"Brightness", c0, py + 94, 12.5f, Col(255, 255, 255), 1, true);
    swprintf(buf, 96, L"%d%%", g_bright); Txt(buf, c0, py + 111, 11.5f, Col(160, 170, 164), 1);
    QBtn(c1, cy, H_Q_VOL, g_tab == T_VOL);
    ISpeaker(c1, cy, 28 * (1 + 0.1f * Hov(H_Q_VOL)), Col(255, 255, 255), g_muted);
    Txt(L"Volume", c1, py + 94, 12.5f, Col(255, 255, 255), 1, true);
    if (g_muted) Txt(L"Muted", c1, py + 111, 11.5f, Col(160, 170, 164), 1);
    else { swprintf(buf, 96, L"%d%%", g_volume); Txt(buf, c1, py + 111, 11.5f, Col(160, 170, 164), 1); }
    QBtn(c2, cy, H_Q_WIFI, g_tab == T_WIFI);
    IWifi(c2, cy, 28 * (1 + 0.1f * Hov(H_Q_WIFI)), g_wifiOn ? Col(GREEN) : Col(255, 255, 255, 170), !g_wifiOn);
    Txt(L"Wi-Fi", c2, py + 94, 12.5f, Col(255, 255, 255), 1, true);
    std::wstring ws = !g_wifiAvail ? L"Unavailable" : !g_wifiOn ? L"Off" : (g_ssid[0] ? g_ssid : L"Not connected");
    if (ws.size() > 14) ws = ws.substr(0, 13) + L"…";
    Txt(ws, c2, py + 111, 11.5f, g_wifiOn ? Col(GREEN) : Col(160, 170, 164), 1);
    QBtn(c3, cy, H_Q_HOTSPOT, g_tab == T_HOTSPOT);
    IHotspot(c3, cy, 28 * (1 + 0.1f * Hov(H_Q_HOTSPOT)), Col(255, 255, 255));
    Txt(L"Hotspot", c3, py + 94, 12.5f, Col(255, 255, 255), 1, true);
    Txt(L"Open settings", c3, py + 111, 11.5f, Col(160, 170, 164), 1);

    ISun(px + 38, py + 170, 22, Col(255, 255, 255, 210));
    Slider(px + 68, py + 170, pw - 68 - 64, g_bright, H_SL_BRIGHT);
    swprintf(buf, 96, L"%d", g_bright); Txt(buf, px + pw - 26, py + 161, 13, Col(160, 170, 164), 2);
    ISpeaker(px + 38, py + 226, 22, Col(255, 255, 255, 210), g_muted);
    Slider(px + 68, py + 226, pw - 68 - 64, g_muted ? 0 : g_volume, H_SL_VOL);
    swprintf(buf, 96, L"%d", g_muted ? 0 : g_volume); Txt(buf, px + pw - 26, py + 217, 13, Col(160, 170, 164), 2);
}

static void DrawPanel(int panel, float cx, float top, float offsetX, float alpha) {
    if (alpha <= 0.01f) return;
    float oldA = gA; gA = oldA * alpha;
    float px = cx - PW[panel] / 2 + offsetX;
    switch (panel) {
        case 0: DrawClockPanel(px, top, PW[0], PH[0]); break;
        case 1: DrawCalendarPanel(px, top, PW[1], PH[1]); break;
        case 2: DrawMediaPanel(px, top, PW[2], PH[2]); break;
        default: DrawQuickPanel(px, top, PW[3], PH[3]); break;
    }
    gA = oldA;
}

// ============================================================ icon bar
struct Slot { int tab; float w; };
static const Slot kSlots[] = {{T_CLOCK, 126}, {T_CAL, 46}, {T_MEDIA, 46}, {-2, 16}, {T_BRIGHT, 46}, {T_VOL, 46}, {T_WIFI, 46}, {T_HOTSPOT, 46}};

static void DrawHeader(float cx, float top) {
    float x = cx - HW / 2 + 14;
    time_t t = time(nullptr); tm* lt = localtime(&t);
    for (const Slot& s : kSlots) {
        if (s.tab == -2) {
            RPen sepPen(Col(255, 255, 255, 40), 1.0f);
            gfx->DrawLine(&sepPen, x + s.w / 2, top + 14, x + s.w / 2, top + HH - 14);
            x += s.w; continue;
        }
        int id = H_TAB + s.tab;
        float h = Hov(id);
        bool active = g_tab == s.tab;
        float icx = s.tab == T_CLOCK ? x + 26 : x + s.w / 2, icy = top + HH / 2;
        float bw = s.w - 2, bh = 40;
        if (active) FillRR(x + 1, top + (HH - bh) / 2, bw, bh, 20, Col(GREEN, 40));
        if (h > 0.01f) FillRR(x + 1, top + (HH - bh) / 2, bw, bh, 20, Col(255, 255, 255, (int)(30 * h)));
        float is = 22 * (1 + 0.12f * h);
        Color white = Col(255, 255, 255), green = Col(GREEN);
        Color ic = active ? green : white;
        switch (s.tab) {
            case T_CLOCK: {
                IClock(icx, icy, is, ic);
                wchar_t b[48];
                int hr = S.hour24 ? lt->tm_hour : (lt->tm_hour % 12 == 0 ? 12 : lt->tm_hour % 12);
                if (S.hour24) swprintf(b, 48, L"%02d:%02d", hr, lt->tm_min); else swprintf(b, 48, L"%d:%02d", hr, lt->tm_min);
                Txt(b, x + 46, top + 10, 16, white, 0, true);
                static const wchar_t* wdA[] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
                static const wchar_t* mnA[] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
                swprintf(b, 48, L"%ls, %ls %d", wdA[lt->tm_wday], mnA[lt->tm_mon], lt->tm_mday);
                Txt(b, x + 46, top + 29, 10.5f, Col(165, 175, 169));
                break;
            }
            case T_CAL: ICalendar(icx, icy, is, ic); break;
            case T_MEDIA: {
                float a = 30 * (1 + 0.1f * h);
                DrawArt(icx - a / 2, icy - a / 2, a, 7);
                if (active) StrokeRR(icx - a / 2 - 2.5f, icy - a / 2 - 2.5f, a + 5, a + 5, 9.5f, Col(GREEN), 1.6f);
                break;
            }
            case T_BRIGHT: ISun(icx, icy, is, ic); break;
            case T_VOL: ISpeaker(icx, icy, is, ic, g_muted); break;
            case T_WIFI: IWifi(icx, icy, is, g_wifiOn ? green : Col(255, 255, 255, 150), !g_wifiOn); break;
            case T_HOTSPOT: IHotspot(icx, icy, is, ic); break;
        }
        AddHit(x, top + 4, s.w, HH - 8, id);
        x += s.w;
    }
}

// ============================================================ frosted glass
static void FreeGlass() {
    if (g_gDC) { DeleteDC(g_gDC); g_gDC = nullptr; }
    if (g_gDib) { DeleteObject(g_gDib); g_gDib = nullptr; }
    g_gBits = nullptr; g_gW = g_gH = 0;
    g_gTmp.clear(); g_gTmp.shrink_to_fit();
}
static void EnsureGlassBuf() {
    int aw = Wd / 4 + 2, ah = Hd / 4 + 2;
    if (g_gDib && aw == g_gW && ah == g_gH) return;
    FreeGlass();
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = aw; bi.bmiHeader.biHeight = -ah;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    HDC scr = GetDC(nullptr);
    g_gDib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void**)&g_gBits, nullptr, 0);
    g_gDC = CreateCompatibleDC(scr);
    ReleaseDC(nullptr, scr);
    if (g_gDib && g_gDC && g_gBits) {
        SelectObject(g_gDC, g_gDib);
        g_gW = aw; g_gH = ah;
        g_gTmp.assign((size_t)aw * ah * 4, 0);
    } else FreeGlass();
}
// one-dimensional box blur with edge clamping; also writes opaque alpha
static void Blur1D(const BYTE* src, BYTE* dst, int len, int count, int pstep, int lstep, int r) {
    int div = 2 * r + 1;
    for (int l = 0; l < count; l++) {
        const BYTE* s = src + (size_t)l * lstep;
        BYTE* d = dst + (size_t)l * lstep;
        int sb = 0, sg = 0, sr = 0;
        for (int i = -r; i <= r; i++) {
            int k = i < 0 ? 0 : (i >= len ? len - 1 : i);
            const BYTE* p = s + (size_t)k * pstep;
            sb += p[0]; sg += p[1]; sr += p[2];
        }
        for (int x = 0; x < len; x++) {
            BYTE* q = d + (size_t)x * pstep;
            q[0] = (BYTE)(sb / div); q[1] = (BYTE)(sg / div); q[2] = (BYTE)(sr / div); q[3] = 255;
            int a = x + r + 1; if (a >= len) a = len - 1;
            int b = x - r; if (b < 0) b = 0;
            const BYTE* pa = s + (size_t)a * pstep;
            const BYTE* pb = s + (size_t)b * pstep;
            sb += pa[0] - pb[0]; sg += pa[1] - pb[1]; sr += pa[2] - pb[2];
        }
    }
}
// Captures the screen behind the island (the window itself is excluded from capture), blurs it heavily,
// darkens and saturates it for readability, then adds tint, sheen, grain, highlight and a glass edge.
static void DrawGlassBody(const GraphicsPath& body, float x, float top, float w, float h, float r) {
    const float pad = 24;
    bool ok = false;
    int sw = 0, sh = 0;
    float lumNow = -1;
    RectF destR(x - pad, top - pad, w + 2 * pad, h + 2 * pad);
    if (g_hwnd) {
        EnsureGlassBuf();
        if (g_gDib) {
            RECT wr; GetWindowRect(g_hwnd, &wr);
            MONITORINFO mi = {sizeof(mi)};
            GetMonitorInfoW(MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
            int dx = wr.left + (int)floorf((x - pad) * sc), dy = wr.top + (int)floorf((top - pad) * sc);
            int dw = (int)ceilf((w + 2 * pad) * sc), dh = (int)ceilf((h + 2 * pad) * sc);
            int cx0 = std::max(dx, (int)mi.rcMonitor.left), cy0 = std::max(dy, (int)mi.rcMonitor.top);
            int cx1 = std::min(dx + dw, (int)mi.rcMonitor.right), cy1 = std::min(dy + dh, (int)mi.rcMonitor.bottom);
            if (cx1 - cx0 >= 8 && cy1 - cy0 >= 8) {
                dx = cx0; dy = cy0; dw = cx1 - cx0; dh = cy1 - cy0;
                destR = RectF((cx0 - wr.left) / sc, (cy0 - wr.top) / sc, dw / sc, dh / sc);
                sw = std::min(g_gW, std::max(4, dw / 4)); sh = std::min(g_gH, std::max(4, dh / 4));
                HDC scr = GetDC(nullptr);
                SetStretchBltMode(g_gDC, HALFTONE);
                SetBrushOrgEx(g_gDC, 0, 0, nullptr);
                ok = StretchBlt(g_gDC, 0, 0, sw, sh, scr, dx, dy, dw, dh, SRCCOPY) != 0;
                ReleaseDC(nullptr, scr);
                if (ok) {
                    int rad = std::max(1, (int)(S.blur * 0.7f * sc + 0.5f));
                    int stride = g_gW * 4;
                    BYTE* a = g_gBits; BYTE* b = g_gTmp.data();
                    for (int i = 0; i < 3; i++) {
                        Blur1D(a, b, sw, sh, 4, stride, rad);
                        Blur1D(b, a, sh, sw, stride, 4, rad);
                    }
                    // mean luminance of the blurred backdrop, used to level the glass
                    long long lsum = 0;
                    for (int yy = 0; yy < sh; yy++) {
                        const BYTE* row = g_gBits + (size_t)yy * stride;
                        for (int xx = 0; xx < sw; xx++) lsum += (row[xx * 4 + 2] * 54 + row[xx * 4 + 1] * 183 + row[xx * 4] * 18) >> 8;
                    }
                    lumNow = (float)lsum / (float)(sw * sh) / 255.0f;
                }
            }
        }
    }

    gfx->SetClip(&body);
    if (ok) {
        Bitmap bm(sw, sh, g_gW * 4, PixelFormat32bppPARGB, g_gBits);
        // Level the backdrop so the glass looks the same over any wallpaper: normalise mean brightness to a fixed
        // target, pull contrast toward that mean, and mute the colour (the accent supplies the colour instead).
        if (lumNow >= 0) g_glassLum = g_glassLum < 0 ? lumNow : g_glassLum + (lumNow - g_glassLum) * 0.2f;
        const float target = 0.20f, contrast = 0.50f, sat = 0.45f;
        float gain = Cl(target / Mx(g_glassLum, 0.03f), 0.10f, 3.5f);
        float kc = gain * contrast, off = target * (1.0f - contrast);
        const float lr = 0.3086f, lg = 0.6094f, lb = 0.0820f;
        ColorMatrix cm = {{
            {((1 - sat) * lr + sat) * kc, (1 - sat) * lr * kc, (1 - sat) * lr * kc, 0, 0},
            {(1 - sat) * lg * kc, ((1 - sat) * lg + sat) * kc, (1 - sat) * lg * kc, 0, 0},
            {(1 - sat) * lb * kc, (1 - sat) * lb * kc, ((1 - sat) * lb + sat) * kc, 0, 0},
            {0, 0, 0, 1, 0}, {off, off, off, 0, 1}}};
        ImageAttributes ia;
        ia.SetColorMatrix(&cm);
        ia.SetWrapMode(WrapModeTileFlipXY);
        gfx->DrawImage(&bm, destR, 0, 0, (REAL)sw, (REAL)sh, UnitPixel, &ia);
    } else {
        SolidBrush flat(Col(10, 18, 14, 235)); gfx->FillPath(&flat, &body);
    }
    // dark accent-tinted wash keeps text crisp on any wallpaper
    SolidBrush wash(Col(g_ar * 17 / 100 + 3, g_ag * 17 / 100 + 3, g_ab * 17 / 100 + 3, 132));
    gfx->FillPath(&wash, &body);
    // gentle vertical sheen: lighter at the top, deeper at the bottom
    LinearGradientBrush sheen(PointF(x, top), PointF(x, top + h), Col(255, 255, 255, 24), Col(0, 0, 0, 46));
    gfx->FillPath(&sheen, &body);
    // fine grain, the thing that makes acrylic feel like material instead of a smear
    if (g_noise) { TextureBrush tb(g_noise, WrapModeTile); gfx->FillPath(&tb, &body); }
    // soft light catching the top edge
    float hlH = Mn(h, 64.0f);
    GraphicsPath hl; RR(hl, x, top, w, hlH, r);
    LinearGradientBrush hb(PointF(x, top), PointF(x, top + hlH), Col(255, 255, 255, 40), Col(255, 255, 255, 0));
    gfx->FillPath(&hb, &hl);
    gfx->ResetClip();
    // reflective rim: bright at the top, accent-tinted at the bottom, plus an inner hairline
    LinearGradientBrush bb(PointF(x, top), PointF(x, top + h), Col(255, 255, 255, 150), Col(GREEN, 85));
    Pen bp(&bb, 1.2f); gfx->DrawPath(&bp, &body);
    StrokeRR(x + 1.5f, top + 1.5f, w - 3, h - 3, r - 1.5f, Col(255, 255, 255, 16), 1.0f);
}

// ---- equalizer bars shown inside the collapsed pill while audio plays
static void DrawEq(float cx, float cy) {
    const int N = 5;
    const float bw = 3.0f, gap = 3.2f;
    float total = N * bw + (N - 1) * gap, x0 = cx - total / 2;
    for (int i = 0; i < N; i++) {
        float hh = 3.0f + 11.0f * Cl(g_eqH[i], 0, 1);
        FillRR(x0 + i * (bw + gap), cy - hh / 2, bw, hh, bw / 2, Col(GREEN));
    }
}

// ============================================================ island
static void DrawIsland() {
    float cx = LW / 2, top = TOP;
    float w = curW, h = curH, x = cx - w / 2;
    float r = Mn(h / 2, 34.0f);

    // soft shadow, drawn only outside the island
    {
        GraphicsPath ip; RR(ip, x, top, w, h, r);
        gfx->SetClip(&ip, CombineModeExclude);
        for (int i = 1; i <= 12; i++)
            FillRR(x - i, top + i * 1.3f, w + 2 * i, h + 2 * i * 0.9f, r + i, Col(0, 0, 0, (int)((S.glass ? 17 : 22) - i * 1.4f)));
        gfx->ResetClip();
    }

    GraphicsPath body; RR(body, x, top, w, h, r);
    if (!S.glass) {
        SolidBrush b(Col(3, 5, 4, 255)); gfx->FillPath(&b, &body);
        StrokeRR(x, top, w, h, r, Col(GREEN, 95), 1.2f);
        StrokeRR(x + 1.5f, top + 1.5f, w - 3, h - 3, r - 1.5f, Col(GREEN, 22), 1.0f);
    } else {
        DrawGlassBody(body, x, top, w, h, r);
    }

    gfx->SetClip(&body);

    // equalizer: only in the collapsed pill, fading out as it opens
    float eqA = g_eqAlpha * Cl(1.0f - (curH - CH - 1.0f) / 10.0f, 0, 1);
    if (eqA > 0.01f) {
        float oldA = gA; gA = oldA * eqA;
        DrawEq(cx, top + curH / 2);
        gA = oldA;
    }

    // icon bar fades in as the pill grows
    float hdr = Ease(Cl((curH - CH - 6) / (HH - CH - 6), 0, 1));
    hdr *= hdr;
    if (hdr > 0.01f) {
        size_t mark = g_hits.size();
        float oldA = gA; gA = oldA * hdr;
        DrawHeader(cx, top);
        gA = oldA;
        if (!g_expanded) g_hits.resize(mark);
    }

    float pTop = top + HH;
    float exp = Cl((curH - HH) / Mx(40.0f, PH[g_panel] * 0.7f), 0, 1);
    float ea = exp * exp;
    if (ea > 0.01f) {
        float oldA = gA; gA = oldA * ea;
        RPen linePen(Col(255, 255, 255, S.glass ? 50 : 28), 1.0f);
        gfx->DrawLine(&linePen, cx - curW / 2 + 22, pTop - 1, cx + curW / 2 - 22, pTop - 1);
        gA = oldA;
    }
    float xf = Ease(g_xfade);
    if (g_prevPanel >= 0 && xf < 1) DrawPanel(g_prevPanel, cx, pTop, -xf * 18, (1 - xf) * ea);
    size_t hitMark = g_hits.size();
    DrawPanel(g_panel, cx, pTop, (1 - xf) * 18, xf * ea);
    if (g_tab == T_NONE) g_hits.resize(hitMark);
    gfx->ResetClip();
}

// ============================================================ window plumbing
static void FreeSurface() {
    FreeGlass();
    delete g_bmp; g_bmp = nullptr;
    if (g_dib) { DeleteObject(g_dib); g_dib = nullptr; }
    if (g_memDC) { DeleteDC(g_memDC); g_memDC = nullptr; }
    g_bits = nullptr;
}
static void AllocSurface() {
    FreeSurface();
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = Wd; bi.bmiHeader.biHeight = -Hd;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    HDC scr = GetDC(nullptr);
    g_dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void**)&g_bits, nullptr, 0);
    g_memDC = CreateCompatibleDC(scr);
    ReleaseDC(nullptr, scr);
    SelectObject(g_memDC, g_dib);
    g_bmp = new Bitmap(Wd, Hd, Wd * 4, PixelFormat32bppPARGB, g_bits);
}

static void PlaceWindow() {
    MONITORINFO mi = {sizeof(mi)};
    HMONITOR mon = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(mon, &mi);
    UINT dpi = GetDpiForWindow(g_hwnd);
    if (!dpi) dpi = 96;
    float newSc = dpi / 96.0f * S.scalePct / 100.0f;
    int nw = (int)ceilf(LW * newSc), nh = (int)ceilf(LH * newSc);
    if (newSc != sc || nw != Wd || nh != Hd || !g_bits) { sc = newSc; Wd = nw; Hd = nh; AllocSurface(); }
    int x = mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left - Wd) / 2;
    int y = mi.rcMonitor.top + (int)(S.topMargin * dpi / 96.0f) - (int)(TOP * sc);
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, Wd, Hd, SWP_NOACTIVATE);
    // the glass theme captures the screen behind it, so it must not capture itself
    SetWindowDisplayAffinity(g_hwnd, S.glass ? 0x11 /*WDA_EXCLUDEFROMCAPTURE*/ : 0x0);
    g_dirty = true;
}

static void Render() {
    if (!g_bits || !g_bmp) return;
    SyncThumb();
    memset(g_bits, 0, (size_t)Wd * Hd * 4);
    {
        Graphics g(g_bmp);
        gfx = &g; gA = 1.0f;
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAlias);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.ScaleTransform(sc, sc);
        g_hits.clear();
        DrawIsland();
        gfx = nullptr;
    }
    RECT wr; GetWindowRect(g_hwnd, &wr);
    POINT pos = {wr.left, wr.top}, src = {0, 0};
    SIZE size = {Wd, Hd};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    HDC scr = GetDC(nullptr);
    UpdateLayeredWindow(g_hwnd, scr, &pos, &size, g_memDC, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, scr);
}

// ============================================================ adaptive accent
static void HsvToRgb(float h, float s, float v, float* o) {
    float c = v * s, hp = h / 60.0f, x = c * (1 - fabsf(fmodf(hp, 2.0f) - 1)), m = v - c, r = 0, g = 0, b = 0;
    if (hp < 1) { r = c; g = x; } else if (hp < 2) { r = x; g = c; } else if (hp < 3) { g = c; b = x; }
    else if (hp < 4) { g = x; b = c; } else if (hp < 5) { r = x; b = c; } else { r = c; b = x; }
    o[0] = (r + m) * 255; o[1] = (g + m) * 255; o[2] = (b + m) * 255;
}
// Samples the screen just to the left and right of the island window (so it can never see the island's
// own pixels), finds the dominant hue of that backdrop and turns it into the accent colour. Every hue works.
// Neutral backdrops (grey / black / white) give a softly tinted light neutral.
static void SampleBackdrop() {
    float tgt[3] = {52, 230, 100};
    if (S.dynamicAccent && g_hwnd && sc > 0) {
        RECT wr; GetWindowRect(g_hwnd, &wr);
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfoW(MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
        int stripW = (int)(260 * sc), sy = wr.top + (int)(TOP * sc), shh = (int)(110 * sc);
        int lx1 = wr.left, lx0 = std::max((int)mi.rcMonitor.left, lx1 - stripW);
        int rx0 = wr.right, rx1 = std::min((int)mi.rcMonitor.right, rx0 + stripW);
        bool gotL = lx1 - lx0 > 8, gotR = rx1 - rx0 > 8;
        if (sy < mi.rcMonitor.top) sy = mi.rcMonitor.top;

        const int SW = 32, SH = 12, HALF = SW / 2;
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = SW; bi.bmiHeader.biHeight = -SH;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
        HDC scr = GetDC(nullptr);
        HDC mdc = CreateCompatibleDC(scr);
        void* bits = nullptr;
        HBITMAP dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (dib && bits && (gotL || gotR)) {
            HGDIOBJ old = SelectObject(mdc, dib);
            SetStretchBltMode(mdc, HALFTONE);
            SetBrushOrgEx(mdc, 0, 0, nullptr);
            bool okL = gotL && StretchBlt(mdc, 0, 0, HALF, SH, scr, lx0, sy, lx1 - lx0, shh, SRCCOPY);
            bool okR = gotR && StretchBlt(mdc, HALF, 0, HALF, SH, scr, rx0, sy, rx1 - rx0, shh, SRCCOPY);
            if (okL || okR) {
                const BYTE* px = (const BYTE*)bits;
                double hist[36] = {0};
                double wsum = 0, ssum = 0, ar = 0, ag = 0, ab = 0;
                int n = 0;
                auto hueOf = [](float r, float g, float b, float mx, float d) {
                    float hue;
                    if (mx == r) hue = fmodf((g - b) / d + 6.0f, 6.0f);
                    else if (mx == g) hue = (b - r) / d + 2.0f;
                    else hue = (r - g) / d + 4.0f;
                    return hue * 60.0f;
                };
                for (int yy = 0; yy < SH; yy++) for (int xx = 0; xx < SW; xx++) {
                    bool left = xx < HALF;
                    if ((left && !okL) || (!left && !okR)) continue;
                    const BYTE* p = px + ((size_t)yy * SW + xx) * 4;
                    float b = p[0] / 255.0f, g = p[1] / 255.0f, r = p[2] / 255.0f;
                    ar += r; ag += g; ab += b; n++;
                    float mx = Mx(r, Mx(g, b)), mn = Mn(r, Mn(g, b)), d = mx - mn;
                    if (mx < 0.10f || d < 0.05f) continue;
                    float sat = d / mx;
                    float wgt = sat * sat * mx;
                    int bin = ((int)(hueOf(r, g, b, mx, d) / 10.0f)) % 36;
                    hist[bin] += wgt; wsum += wgt; ssum += sat * wgt;
                }
                if (n > 0 && wsum / n > 0.02) {
                    int best = 0; double bv = -1;
                    for (int i = 0; i < 36; i++) {
                        double v = hist[(i + 35) % 36] + hist[i] * 1.5 + hist[(i + 1) % 36];
                        if (v > bv) { bv = v; best = i; }
                    }
                    double cs = 0, sn = 0;
                    for (int k = -2; k <= 2; k++) {
                        int bn = (best + k + 36) % 36;
                        double ang = (bn * 10 + 5) * 3.14159265358979 / 180.0;
                        cs += cos(ang) * hist[bn]; sn += sin(ang) * hist[bn];
                    }
                    float hue = (float)(atan2(sn, cs) * 180.0 / 3.14159265358979);
                    if (hue < 0) hue += 360.0f;
                    float s = Cl((float)(ssum / wsum) + 0.15f, 0.62f, 0.90f);
                    HsvToRgb(hue, s, 0.96f, tgt);
                } else if (n > 0) {
                    float r = (float)(ar / n), g = (float)(ag / n), b = (float)(ab / n);
                    float mx = Mx(r, Mx(g, b)), mn = Mn(r, Mn(g, b)), d = mx - mn;
                    float hue = d > 0.004f ? hueOf(r, g, b, mx, d) : 0.0f;
                    float s = mx > 0.001f ? Cl(d / mx * 1.5f, 0.0f, 0.22f) : 0.0f;
                    HsvToRgb(hue, s, 0.90f, tgt);
                }
            }
            SelectObject(mdc, old);
        }
        if (dib) DeleteObject(dib);
        DeleteDC(mdc);
        ReleaseDC(nullptr, scr);
    }
    for (int i = 0; i < 3; i++) g_at[i] = tgt[i];
}
// reads the real output level and animates the five bars; returns true while anything is visible
static bool StepEq(float dt) {
    dt = Cl(dt, 0.001f, 0.033f);
    float p = PeakNow();
    bool active = p > 0.004f;
    float tgt = active ? Cl(p * 1.8f, 0.18f, 1.0f) : 0.0f;
    g_eqLevel += (tgt - g_eqLevel) * Mn(1.0f, dt * (tgt > g_eqLevel ? 18.0f : 5.0f));
    g_eqAlpha += ((g_eqLevel > 0.04f ? 1.0f : 0.0f) - g_eqAlpha) * Mn(1.0f, dt * 8.0f);
    float t = (float)(GetTickCount64() % 100000) / 1000.0f;
    static const float ph[5] = {0.0f, 1.7f, 3.1f, 4.4f, 5.9f}, sp[5] = {7.3f, 9.1f, 6.2f, 10.4f, 8.0f};
    for (int i = 0; i < 5; i++) {
        float wave = 0.30f + 0.70f * (0.5f + 0.5f * sinf(t * sp[i] + ph[i]));
        g_eqH[i] += (g_eqLevel * wave - g_eqH[i]) * Mn(1.0f, dt * 22.0f);
    }
    return g_eqAlpha > 0.01f || g_eqLevel > 0.01f;
}
static bool StepAccent(float dt) {
    bool anim = false;
    for (int i = 0; i < 3; i++) {
        float d = g_at[i] - g_af[i];
        if (fabsf(d) > 0.4f) { g_af[i] += d * Mn(1.0f, dt * 3.5f); anim = true; } else g_af[i] = g_at[i];
    }
    int nr = (int)(g_af[0] + 0.5f), ng = (int)(g_af[1] + 0.5f), nb = (int)(g_af[2] + 0.5f);
    if (nr != g_ar || ng != g_ag || nb != g_ab) {
        g_ar = nr; g_ag = ng; g_ab = nb; g_dirty = true;
    }
    return anim;
}

// ============================================================ behaviour
static void KickTimer(HWND h) {
    if (!g_timerPeriod || g_timerPeriod > 16) { g_timerPeriod = 16; SetTimer(h, 1, 16, nullptr); }
}
static void SetTarget(int tab) {
    int np = PanelOf(tab);
    if (g_tab == T_NONE) { g_panel = np; g_prevPanel = -1; g_xfade = 1.0f; }
    else if (np != g_panel) { g_prevPanel = g_panel; g_panel = np; g_xfade = 0.0f; }
    g_tab = tab;
    g_expanded = true;
    if (np == 3 || tab == T_MEDIA) { AudioPoll(); WifiPoll(); }
    if (tab == T_MEDIA) MediaWake();
}
static void CollapseAll() { g_tab = T_NONE; g_expanded = false; g_outsideSince = 0; g_suppressHover = g_hoverIsland; g_dirty = true; }
static void OnTab(int tab) {
    if (g_tab == tab) g_tab = T_NONE;      // back to the icon bar
    else SetTarget(tab);
    g_dirty = true;
}
static bool InIsland(float lx, float ly) {
    float x = LW / 2 - curW / 2;
    return lx >= x && lx <= x + curW && ly >= TOP && ly <= TOP + curH;
}
static void SliderSet(int id, float lx) {
    float f = Cl((lx - g_dragX) / g_dragW, 0, 1);
    int pct = (int)(f * 100 + 0.5f);
    if (id == H_SL_SEEK) g_seekFrac = f;
    else if (id == H_SL_BRIGHT) { g_bright = pct; g_brightPending = pct; g_brightKnown = true; }
    else SetVolumePct(pct);
    g_dirty = true;
}
static const Hit* HitAt(float lx, float ly) {
    for (size_t i = g_hits.size(); i-- > 0;) {
        const Hit& h = g_hits[i];
        if (lx >= h.x && lx <= h.x + h.w && ly >= h.y && ly <= h.y + h.h) return &h;
    }
    return nullptr;
}
static void OnClickId(const Hit& h, float lx) {
    int id = h.id;
    if (id >= H_TAB && id < H_TAB + 7) { OnTab(id - H_TAB); return; }
    switch (id) {
        case H_PREVMONTH: g_calOffset--; break;
        case H_NEXTMONTH: g_calOffset++; break;
        case H_MPREV: case H_MNEXT: {
            MediaInfo m = MediaSnap();
            if (m.has) { g_cmd = id == H_MNEXT ? 2 : 3; MediaWake(); }
            else MediaKey(id == H_MNEXT ? VK_MEDIA_NEXT_TRACK : VK_MEDIA_PREV_TRACK);
            break;
        }
        case H_MPLAY: {
            MediaInfo m = MediaSnap();
            if (m.has) {
                EnterCriticalSection(&g_mcs);      // optimistic flip; the next poll corrects it if needed
                g_mi.posMs = CurPosMs(g_mi); g_mi.tick = GetTickCount64(); g_mi.playing = !g_mi.playing;
                LeaveCriticalSection(&g_mcs);
                g_cmd = 1; MediaWake();
            } else MediaKey(VK_MEDIA_PLAY_PAUSE);
            break;
        }
        case H_SL_BRIGHT: case H_SL_VOL: case H_SL_MVOL: case H_SL_SEEK:
            g_drag = id; g_dragX = h.x + 8; g_dragW = h.w - 16;
            SliderSet(id, lx); SetCapture(g_hwnd);
            break;
        case H_Q_BRIGHT: g_tab = T_BRIGHT; break;
        case H_Q_VOL: ToggleMute(); g_tab = T_VOL; break;
        case H_Q_WIFI: WifiSet(!g_wifiOn); g_tab = T_WIFI; break;
        case H_Q_HOTSPOT: g_tab = T_HOTSPOT; ShellExecuteW(nullptr, L"open", L"ms-settings:network-mobilehotspot", nullptr, nullptr, SW_SHOWNORMAL); break;
    }
    g_dirty = true;
}

static bool Tick(float dt) {
    dt = Cl(dt, 0.001f, 0.033f);
    float tw, th;
    if (g_tab != T_NONE) { tw = Mx(HW, PW[g_panel]); th = HH + PH[g_panel]; }
    else if (g_expanded) { tw = HW; th = HH; }
    else { tw = g_hoverIsland ? CW + 14 : CW; th = g_hoverIsland ? CH + 2 : CH; }
    const float k = 300.0f, c = 26.0f;
    velW += ((tw - curW) * k - c * velW) * dt; curW += velW * dt;
    velH += ((th - curH) * k - c * velH) * dt; curH += velH * dt;
    if (curH < CH) { curH = CH; if (velH < 0) velH = 0; }
    if (curW < CW) { curW = CW; if (velW < 0) velW = 0; }
    bool anim = fabsf(tw - curW) > 0.15f || fabsf(th - curH) > 0.15f || fabsf(velW) > 1 || fabsf(velH) > 1;
    if (g_xfade < 1.0f) { g_xfade = Mn(1.0f, g_xfade + dt / 0.28f); anim = true; if (g_xfade >= 1) g_prevPanel = -1; }
    for (auto& kv : g_hov) {
        float target = kv.first == g_hoverId ? 1.0f : 0.0f;
        float d = target - kv.second;
        if (fabsf(d) > 0.003f) { kv.second += d * Mn(1.0f, dt * 14.0f); anim = true; } else kv.second = target;
    }
    if (g_drag) anim = true;
    return anim;
}

static void OnTimer() {
    ULONGLONG now = GetTickCount64();
    float dt = g_lastTick ? (now - g_lastTick) / 1000.0f : 0.016f;
    g_lastTick = now;
    bool anim = Tick(dt);
    if (now - g_lastSample > (g_expanded ? 1000u : 2000u)) { g_lastSample = now; SampleBackdrop(); }
    if (StepAccent(dt)) anim = true;
    if (S.glass && now - g_lastGlass > (g_expanded ? 500u : 1000u)) { g_lastGlass = now; g_dirty = true; }   // keep the backdrop fresh
    bool eqAnim = StepEq(dt);
    if (eqAnim && !g_expanded) g_dirty = true;
    if (!g_expanded && now - g_lastDev > 8000) { g_lastDev = now; AudioRelease(); }   // follow default-device changes

    if (g_expanded && !g_drag && now - g_lastPoll > 1500) {
        g_lastPoll = now;
        AudioRelease(); AudioPoll();
        if (g_panel == 3 || g_tab == T_NONE) WifiPoll();
        g_dirty = true;
    }
    if (g_tab == T_MEDIA) g_dirty = true;                       // keeps the progress bar moving
    if (g_expanded && g_thumbGen != g_uiThumbGen) g_dirty = true; // new artwork arrived
    time_t sec = time(nullptr);
    if (sec != g_lastSec) { g_lastSec = sec; if (g_expanded) g_dirty = true; }
    PumpBrightness();

    // icon bar alone collapses quickly after the pointer leaves; an open panel waits for the setting
    if (g_expanded && (g_tab == T_NONE || S.autoCollapse > 0) && !g_drag) {
        ULONGLONG limit = g_tab == T_NONE ? 40 : (ULONGLONG)S.autoCollapse * 1000;
        float mg = g_tab == T_NONE ? 1.0f : 12.0f;
        POINT p; GetCursorPos(&p);
        RECT wr; GetWindowRect(g_hwnd, &wr);
        float lx = (p.x - wr.left) / sc, ly = (p.y - wr.top) / sc;
        float x = LW / 2 - curW / 2;
        bool inside = lx >= x - mg && lx <= x + curW + mg && ly >= TOP - mg && ly <= TOP + curH + mg;
        if (inside) g_outsideSince = 0;
        else if (!g_outsideSince) g_outsideSince = now;
        else if (now - g_outsideSince > limit) CollapseAll();
    }

    if (anim || g_dirty) { Render(); g_dirty = false; }
    UINT want = (anim || g_tab == T_MEDIA) ? 16 : ((g_expanded || eqAnim) ? 33 : 200);
    if (want != g_timerPeriod) { g_timerPeriod = want; SetTimer(g_hwnd, 1, want, nullptr); }
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
        case WM_TIMER: if (wp == 1) OnTimer(); return 0;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            float lx = GET_X_LPARAM(lp) / sc, ly = GET_Y_LPARAM(lp) / sc;
            if (!g_tracking) { TRACKMOUSEEVENT te = {sizeof(te), TME_LEAVE, h, 0}; TrackMouseEvent(&te); g_tracking = true; }
            if (g_drag) { SliderSet(g_drag, lx); return 0; }
            const Hit* hit = HitAt(lx, ly);
            int id = hit ? hit->id : 0;
            bool inIs = InIsland(lx, ly);
            if (!inIs) g_suppressHover = false;
            if (inIs && !g_expanded && S.hoverExpand && !g_suppressHover) {   // hover: pill -> icon bar
                g_expanded = true; g_outsideSince = 0; g_dirty = true; KickTimer(h);
            }
            if (id != g_hoverId || inIs != g_hoverIsland) {
                g_hoverId = id; g_hoverIsland = inIs; g_dirty = true;
                if (id && !g_hov.count(id)) g_hov[id] = 0;
                KickTimer(h);
            }
            SetCursor(LoadCursorW(nullptr, (id || (inIs && (!g_expanded || g_tab == T_NONE))) ? IDC_HAND : IDC_ARROW));
            return 0;
        }
        case WM_MOUSELEAVE:
            g_tracking = false; g_suppressHover = false; g_hoverId = 0; g_hoverIsland = false; g_dirty = true;
            if (g_expanded && g_tab == T_NONE && !g_drag) CollapseAll();   // icon bar alone: close the instant the pointer leaves
            KickTimer(h);
            return 0;
        case WM_LBUTTONDOWN: {
            float lx = GET_X_LPARAM(lp) / sc, ly = GET_Y_LPARAM(lp) / sc;
            const Hit* hit = HitAt(lx, ly);
            if (hit) { Hit c = *hit; OnClickId(c, lx); }
            else if (InIsland(lx, ly)) {
                if (!g_expanded) { g_expanded = true; g_outsideSince = 0; }   // click also opens it
                g_dirty = true;
            }
            KickTimer(h);
            return 0;
        }
        case WM_RBUTTONUP:
            if (g_expanded) { CollapseAll(); KickTimer(h); }
            return 0;
        case WM_LBUTTONUP:
            if (g_drag) {
                if (g_drag == H_SL_BRIGHT) g_brightPending = g_bright;
                if (g_drag == H_SL_SEEK) {
                    MediaInfo mi = MediaSnap();
                    if (mi.has && mi.durMs > 0) {
                        long long ms = (long long)(g_seekFrac * mi.durMs);
                        EnterCriticalSection(&g_mcs);
                        g_mi.posMs = ms; g_mi.tick = GetTickCount64();
                        LeaveCriticalSection(&g_mcs);
                        g_seekReq = ms; MediaWake();
                    }
                }
                g_drag = 0; ReleaseCapture(); g_dirty = true;
            }
            return 0;
        case WM_MOUSEWHEEL:
            if (g_tab == T_CAL) { g_calOffset += GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1; g_dirty = true; }
            return 0;
        case WM_DISPLAYCHANGE: case WM_DPICHANGED: PlaceWindow(); return 0;
        case WM_APP + 1: PlaceWindow(); g_dirty = true; return 0;
        case WM_DESTROY: KillTimer(h, 1); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(h, m, wp, lp);
}

static DWORD WINAPI UIThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    GdiplusStartupInput gsi;
    GdiplusStartup(&g_gdipTok, &gsi, nullptr);
    {   // tiny tiled grain for the glass surface
        g_noise = new Bitmap(96, 96, PixelFormat32bppPARGB);
        unsigned seed = 1337;
        for (int yy = 0; yy < 96; yy++) for (int xx = 0; xx < 96; xx++) {
            seed = seed * 1664525u + 1013904223u;
            int v = (seed >> 16) & 255, a = 1 + ((seed >> 8) & 7);
            g_noise->SetPixel(xx, yy, v > 128 ? Color((BYTE)a, 255, 255, 255) : Color((BYTE)a, 0, 0, 0));
        }
    }
    g_ff = new FontFamily(L"Segoe UI");
    if (!g_ff->IsAvailable()) { delete g_ff; g_ff = new FontFamily(L"Arial"); }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WH_DynamicIsland";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY), &mi);
    g_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE, wc.lpszClassName, L"Dynamic Island",
                             WS_POPUP, mi.rcMonitor.left, mi.rcMonitor.top, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);
    if (g_hwnd) {
        PlaceWindow();
        AudioPoll(); WifiPoll();
        CreateThread(nullptr, 0, ReadBrightnessThread, nullptr, 0, nullptr);
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        Render();
        g_timerPeriod = 200; SetTimer(g_hwnd, 1, 200, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }

    AudioRelease();
    delete g_thumb; g_thumb = nullptr;
    delete g_noise; g_noise = nullptr;
    FreeSurface();
    for (auto& kv : g_fonts) delete kv.second;
    g_fonts.clear();
    delete g_ff; g_ff = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(g_gdipTok);
    CoUninitialize();
    return 0;
}

// ============================================================ Windhawk entry points
BOOL Wh_ModInit() {
    LoadSettings();
    InitializeCriticalSection(&g_mcs);
    g_mediaEvt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mediaRun = true;
    g_mediaThread = CreateThread(nullptr, 0, MediaThread, nullptr, 0, nullptr);
    g_thread = CreateThread(nullptr, 0, UIThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP + 1, 0, 0);
}

void Wh_ModUninit() {
    g_mediaRun = false;
    MediaWake();
    if (g_hwnd) PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    if (g_thread) { WaitForSingleObject(g_thread, 4000); CloseHandle(g_thread); g_thread = nullptr; }
    if (g_mediaThread) { WaitForSingleObject(g_mediaThread, 4000); CloseHandle(g_mediaThread); g_mediaThread = nullptr; }
    if (g_mediaEvt) { CloseHandle(g_mediaEvt); g_mediaEvt = nullptr; }
    if (g_brightProc) { CloseHandle(g_brightProc); g_brightProc = nullptr; }
    DeleteCriticalSection(&g_mcs);
}
