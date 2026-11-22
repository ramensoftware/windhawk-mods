// ==WindhawkMod==
// @id              acrylic-media-pill
// @name            Acrylic Media Pill
// @description     Glass media pill for the taskbar (cover art, title/artist, mini visualizer) with a click-to-open player card. Dark / Light / Liquid Glass themes, smooth anti-aliased edges. Works with dock-style taskbars.
// @version         1.0.0
// @author          you
// @include         explorer.exe
// @compilerOptions -DWINRT_NO_MODULE_LOCK -lole32 -loleaut32 -lruntimeobject -lgdiplus -ldwmapi -lshcore -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Acrylic Media Pill

Shows whatever is playing through Windows' media controls (Edge, Spotify,
Chrome, VLC, Groove, ...) as a small glass pill on the taskbar.

Themes: **Dark**, **Light**, **Liquid Glass**. The glass is rendered by the mod
(blurred snapshot of what is behind it + tint + rim) so the rounded edges are
fully anti-aliased. The snapshot is taken when the pill appears and refreshed
about once a minute (the pill blinks for a split second then).

* Left: cover art (center-cropped, downscaled with high-quality bicubic)
* Middle: 5-bar mini visualizer (mountain / cosine shape, driven by the
  system output peak meter, so it is very cheap)
* Right: title + artist
* **Click the pill** to open a player card with progress bar, seek,
  previous / play-pause / next. Click anywhere else to close it.

The pill is its own small top-level window owned by explorer.exe, NOT a XAML
element inside the taskbar. That is why it does not collide with
"Windows 11 Taskbar Styler" on dock-style taskbars.

Pill height follows the taskbar height (see "Pill height" setting).
Horizontal taskbars only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: right
  $name: Pill position
  $options:
  - left: Far left of the taskbar
  - right: Far right (just left of the system tray)
  - custom: Custom (X offset measured from the taskbar's left edge)
- offsetX: 0
  $name: Horizontal offset (px)
  $description: Added to the chosen position. Negative moves left.
- offsetY: 0
  $name: Vertical offset (px)
- heightPercent: 70
  $name: Pill height (% of taskbar height)
- maxWidth: 240
  $name: Maximum pill width (px)
- theme: dark
  $name: Theme
  $options:
  - dark: Dark
  - light: Light
  - glass: Liquid Glass
- coverTint: false
  $name: Cover color tint
  $description: Tint the glass with the dominant color of the current cover art. Works with every theme. Off by default.
- tintOpacity: 35
  $name: Tint strength (0-100)
- showVisualizer: true
  $name: Show mini visualizer
- hideWhenIdle: true
  $name: Hide when nothing is playing
- hoverEffect: true
  $name: Hover effects
  $description: Highlight the pill and the card buttons when the mouse is over them.
- scrollVolume: true
  $name: Scroll wheel changes volume
  $description: Use the mouse wheel over the pill to change the system volume.
- scrollInvert: false
  $name: Reverse scroll direction
  $description: Off = scroll up raises the volume. On = scroll down raises it.
- scrollStep: 2
  $name: Volume change per scroll notch (%)
- backdropRefresh: 0
  $name: Refresh blurred backdrop every N seconds (0 = never)
  $description: Re-capturing makes the pill blink for a split second, so it is off by default. It is still refreshed when the pill reappears or the wallpaper/display changes.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <propsys.h>
#include <oleauto.h>
#if __has_include(<uiautomationclient.h>)
#include <uiautomationclient.h>
#define HAVE_UIA 1
#endif
#include <shcore.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <gdiplus.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>

#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <cstring>

namespace wmc = winrt::Windows::Media::Control;

// ---------------------------------------------------------------- helpers
template <class T> static T Mn(T a, T b) { return a < b ? a : b; }
template <class T> static T Mx(T a, T b) { return a > b ? a : b; }
template <class T> static T Cl(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

static constexpr UINT WM_APP_STATE = WM_APP + 1;
static constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
static constexpr UINT_PTR TIMER_ID = 1;
static constexpr UINT TIMER_MS = 33;

// High-resolution clock (GetTickCount64 only ticks every ~15 ms -> visible stutter)
static double NowSec() {
    static LARGE_INTEGER f{};
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)f.QuadPart;
}

// ---------------------------------------------------------------- settings
struct Settings {
    int pos = 1;  // 0 left, 1 right, 2 custom
    int offX = 0, offY = 0;
    int heightPct = 70;
    int maxW = 320;
    int theme = 0;  // 0 dark, 1 light, 2 liquid glass
    int tint = 35;
    bool viz = true;
    bool hideIdle = true;
    bool coverTint = false;
    bool hover = true;
    bool scrollVol = true;
    bool scrollInv = false;
    int scrollStep = 2;
    int refreshSec = 0;
};
static Settings g_set;

static void LoadSettings() {
    Settings s;
    PCWSTR p = Wh_GetStringSetting(L"position");
    s.pos = !wcscmp(p, L"left") ? 0 : (!wcscmp(p, L"custom") ? 2 : 1);
    Wh_FreeStringSetting(p);
    p = Wh_GetStringSetting(L"theme");
    s.theme = !wcscmp(p, L"light") ? 1 : (!wcscmp(p, L"glass") ? 2 : 0);
    Wh_FreeStringSetting(p);
    s.offX = Wh_GetIntSetting(L"offsetX");
    s.offY = Wh_GetIntSetting(L"offsetY");
    s.heightPct = Cl(Wh_GetIntSetting(L"heightPercent"), 30, 100);
    s.maxW = Cl(Wh_GetIntSetting(L"maxWidth"), 120, 900);
    s.tint = Cl(Wh_GetIntSetting(L"tintOpacity"), 0, 100);
    s.viz = Wh_GetIntSetting(L"showVisualizer") != 0;
    s.hideIdle = Wh_GetIntSetting(L"hideWhenIdle") != 0;
    s.coverTint = Wh_GetIntSetting(L"coverTint") != 0;
    s.hover = Wh_GetIntSetting(L"hoverEffect") != 0;
    s.scrollVol = Wh_GetIntSetting(L"scrollVolume") != 0;
    s.scrollInv = Wh_GetIntSetting(L"scrollInvert") != 0;
    s.scrollStep = Cl(Wh_GetIntSetting(L"scrollStep"), 1, 20);
    s.refreshSec = Cl(Wh_GetIntSetting(L"backdropRefresh"), 0, 3600);
    g_set = s;
}

// ---------------------------------------------------------------- media state
struct MediaState {
    bool hasSession = false;
    bool playing = false;
    bool canPrev = false, canNext = false, canSeek = false;
    std::wstring title, artist;
    int64_t posTicks = 0, startTicks = 0, endTicks = 0;  // 100 ns units
    winrt::clock::time_point updated{};
    std::shared_ptr<Gdiplus::Bitmap> cover;
    std::wstring sourceId;      // SMTC SourceAppUserModelId (which app is playing)
    unsigned int accent = 0;    // dominant cover color, 0x00BBGGRR
    bool hasAccent = false;
};

static std::mutex g_mtx;          // guards g_state, g_session
static std::mutex g_refreshMtx;   // serializes RefreshAll
static MediaState g_state;
static std::atomic<double> g_graceUntil{0};  // keep last track briefly when the session drops
static wmc::GlobalSystemMediaTransportControlsSession g_session{nullptr};
static wmc::GlobalSystemMediaTransportControlsSessionManager g_mgr{nullptr};
static wmc::GlobalSystemMediaTransportControlsSessionManager::CurrentSessionChanged_revoker g_mgrRev;
static wmc::GlobalSystemMediaTransportControlsSession::PlaybackInfoChanged_revoker g_pbRev;
static wmc::GlobalSystemMediaTransportControlsSession::MediaPropertiesChanged_revoker g_mpRev;
static wmc::GlobalSystemMediaTransportControlsSession::TimelinePropertiesChanged_revoker g_tlRev;

static HWND g_pill = nullptr, g_card = nullptr, g_taskbar = nullptr;
static std::atomic<bool> g_running{false};
static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;

static MediaState Snapshot() {
    std::lock_guard<std::mutex> l(g_mtx);
    return g_state;
}

static wmc::GlobalSystemMediaTransportControlsSession GetSession() {
    std::lock_guard<std::mutex> l(g_mtx);
    return g_session;
}

// dominant (vivid) color of the cover, normalized so it works as a glass tint
static bool ComputeAccent(Gdiplus::Bitmap& src, unsigned int* out) {
    Gdiplus::Bitmap small(16, 16, PixelFormat32bppPARGB);
    {
        Gdiplus::Graphics g(&small);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
        g.DrawImage(&src, Gdiplus::Rect(0, 0, 16, 16), 0, 0, (int)src.GetWidth(), (int)src.GetHeight(),
                    Gdiplus::UnitPixel);
    }
    Gdiplus::BitmapData bd;
    Gdiplus::Rect r(0, 0, 16, 16);
    if (small.LockBits(&r, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &bd) != Gdiplus::Ok) return false;
    double sr = 0, sg = 0, sb = 0, sw = 0;
    for (int y = 0; y < 16; y++) {
        const BYTE* row = (const BYTE*)bd.Scan0 + (ptrdiff_t)y * bd.Stride;
        for (int x = 0; x < 16; x++) {
            double b = row[x * 4], g = row[x * 4 + 1], rr = row[x * 4 + 2];
            double mx = Mx(rr, Mx(g, b)), mn = Mn(rr, Mn(g, b));
            double sat = mx > 0 ? (mx - mn) / mx : 0, val = mx / 255.0;
            double wgt = 0.04 + sat * (0.25 + val);  // favor vivid pixels
            sr += rr * wgt;
            sg += g * wgt;
            sb += b * wgt;
            sw += wgt;
        }
    }
    small.UnlockBits(&bd);
    if (sw <= 0) return false;
    float R = (float)(sr / sw), G = (float)(sg / sw), B = (float)(sb / sw);
    float m = (R + G + B) / 3.f;
    R = Cl(m + (R - m) * 1.35f, 0.f, 255.f);
    G = Cl(m + (G - m) * 1.35f, 0.f, 255.f);
    B = Cl(m + (B - m) * 1.35f, 0.f, 255.f);
    float mx = Mx(R, Mx(G, B));
    if (mx < 1.f) mx = 1.f;
    float f = Cl(mx, 120.f, 215.f) / mx;
    *out = (unsigned int)(R * f) | ((unsigned int)(G * f) << 8) | ((unsigned int)(B * f) << 16);
    return true;
}

static std::shared_ptr<Gdiplus::Bitmap> LoadCover(
    wmc::GlobalSystemMediaTransportControlsSessionMediaProperties const& props, unsigned int* accent,
    bool* hasAccent) {
    try {
        auto ref = props.Thumbnail();
        if (!ref) return nullptr;
        auto rs = ref.OpenReadAsync().get();
        winrt::com_ptr<IStream> st;
        if (FAILED(CreateStreamOverRandomAccessStream(winrt::get_unknown(rs), IID_PPV_ARGS(st.put()))))
            return nullptr;
        Gdiplus::Bitmap src(st.get());
        if (src.GetLastStatus() != Gdiplus::Ok) return nullptr;
        int w = (int)src.GetWidth(), h = (int)src.GetHeight();
        if (w <= 0 || h <= 0) return nullptr;
        int side = Mn(w, h);
        int out = Mn(side, 512);  // square, center-cropped, capped
        auto bmp = std::make_shared<Gdiplus::Bitmap>(out, out, PixelFormat32bppPARGB);
        Gdiplus::Graphics g(bmp.get());
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.DrawImage(&src, Gdiplus::Rect(0, 0, out, out), (w - side) / 2, (h - side) / 2, side, side,
                    Gdiplus::UnitPixel);
        *hasAccent = ComputeAccent(*bmp, accent);
        return bmp;
    } catch (...) {
        return nullptr;
    }
}

static void RefreshAll() {
    std::lock_guard<std::mutex> rl(g_refreshMtx);
    if (!g_running) return;
    auto s = GetSession();
    MediaState ns;
    MediaState old = Snapshot();
    if (s) {
        try {
            ns.hasSession = true;
            ns.sourceId = s.SourceAppUserModelId().c_str();
            auto pb = s.GetPlaybackInfo();
            ns.playing = pb.PlaybackStatus() ==
                         wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            auto c = pb.Controls();
            ns.canPrev = c.IsPreviousEnabled();
            ns.canNext = c.IsNextEnabled();
            ns.canSeek = c.IsPlaybackPositionEnabled();
            auto tl = s.GetTimelineProperties();
            ns.posTicks = tl.Position().count();
            ns.startTicks = tl.StartTime().count();
            ns.endTicks = tl.EndTime().count();
            ns.updated = tl.LastUpdatedTime();
            auto props = s.TryGetMediaPropertiesAsync().get();
            ns.title = props.Title().c_str();
            ns.artist = props.Artist().c_str();
            if (ns.artist.empty()) ns.artist = props.AlbumTitle().c_str();
            if (old.cover && old.title == ns.title && old.artist == ns.artist) {
                ns.cover = old.cover;
                ns.accent = old.accent;
                ns.hasAccent = old.hasAccent;
            } else {
                ns.cover = LoadCover(props, &ns.accent, &ns.hasAccent);
            }
        } catch (...) {
            ns = old;  // transient WinRT failure (e.g. tab switch): keep what we had
        }
    }
    if (!ns.hasSession && old.hasSession) {
        // switching tabs/apps often drops the session for a moment: keep the last track
        // (paused look) for a short grace period instead of hiding/flashing the pill
        g_graceUntil = NowSec() + 1.5;
        ns = old;
        ns.playing = false;
    } else if (ns.hasSession) {
        g_graceUntil = 0;
    }
    {
        std::lock_guard<std::mutex> l(g_mtx);
        g_state = ns;
    }
    if (g_pill) PostMessageW(g_pill, WM_APP_STATE, 0, 0);
}

static void AttachSession(wmc::GlobalSystemMediaTransportControlsSession s) {
    {
        std::lock_guard<std::mutex> l(g_mtx);
        g_session = s;
    }
    g_pbRev = {};
    g_mpRev = {};
    g_tlRev = {};
    if (s) {
        g_pbRev = s.PlaybackInfoChanged(winrt::auto_revoke, [](auto&&, auto&&) { RefreshAll(); });
        g_mpRev = s.MediaPropertiesChanged(winrt::auto_revoke, [](auto&&, auto&&) { RefreshAll(); });
        g_tlRev = s.TimelinePropertiesChanged(winrt::auto_revoke, [](auto&&, auto&&) { RefreshAll(); });
    }
    RefreshAll();
}

static void OnSessionChanged() {
    try {
        if (g_mgr) AttachSession(g_mgr.GetCurrentSession());
    } catch (...) {
    }
}

static void StartMedia() {
    try {
        g_mgr = wmc::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        g_mgrRev = g_mgr.CurrentSessionChanged(winrt::auto_revoke, [](auto&&, auto&&) { OnSessionChanged(); });
        OnSessionChanged();
    } catch (...) {
        Wh_Log(L"SMTC init failed");
    }
}

static void StopMedia() {
    g_mgrRev = {};
    g_pbRev = {};
    g_mpRev = {};
    g_tlRev = {};
    std::lock_guard<std::mutex> rl(g_refreshMtx);
    {
        std::lock_guard<std::mutex> l(g_mtx);
        g_session = nullptr;
        g_state = MediaState{};
    }
    g_mgr = nullptr;
}

// transport commands (fire and forget)
static void CmdPrev() { try { if (auto s = GetSession()) (void)s.TrySkipPreviousAsync(); } catch (...) {} }
static void CmdNext() { try { if (auto s = GetSession()) (void)s.TrySkipNextAsync(); } catch (...) {} }
static void CmdToggle() { try { if (auto s = GetSession()) (void)s.TryTogglePlayPauseAsync(); } catch (...) {} }
static void CmdSeek(int64_t ticks) {
    try { if (auto s = GetSession()) (void)s.TryChangePlaybackPositionAsync(ticks); } catch (...) {}
}

static int64_t CurrentPos(const MediaState& st) {
    int64_t p = st.posTicks;
    if (st.playing) p += (winrt::clock::now() - st.updated).count();
    if (st.endTicks > st.startTicks) p = Cl(p, st.startTicks, st.endTicks);
    return p;
}

// ---------------------------------------------------------------- audio peak (visualizer)
// mingw's endpointvolume.h only forward-declares IAudioMeterInformation, so define it here.
struct IAudioMeterX : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};
static const GUID IID_MeterX = {0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};
static winrt::com_ptr<IAudioMeterX> g_meter;
static float g_bars[5] = {};
static const float kShape[5] = {0.55f, 0.85f, 1.0f, 0.85f, 0.55f};  // mountain

static void AcquireMeter() {
    g_meter = nullptr;
    try {
        winrt::com_ptr<IMMDeviceEnumerator> en;
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator), en.put_void())))
            return;
        winrt::com_ptr<IMMDevice> dev;
        if (FAILED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, dev.put()))) return;
        dev->Activate(IID_MeterX, CLSCTX_ALL, nullptr, g_meter.put_void());
    } catch (...) {
    }
}

static float g_peakS = 0;
static bool UpdateBars(bool playing) {
    static double last = 0;
    double t = NowSec();
    float dt = last ? (float)Cl(t - last, 0.0005, 0.1) : 0.016f;
    last = t;

    float peak = 0.f;
    if (g_meter && FAILED(g_meter->GetPeakValue(&peak))) peak = 0.f;
    // fast attack, slower release, independent of frame rate
    float pk = 1.f - expf(-dt / (peak > g_peakS ? 0.04f : 0.18f));
    g_peakS += (peak - g_peakS) * pk;

    float level = playing ? (0.2f + 0.8f * sqrtf(Cl(g_peakS, 0.f, 1.f))) : 0.f;
    bool moving = playing;
    for (int i = 0; i < 5; i++) {
        float wob = 0.5f + 0.5f * cosf((float)fmod(t * 5.5, 6.2831853) + i * 1.15f);  // cosine wave
        float target = playing ? Mx(0.14f, kShape[i] * level * (0.55f + 0.45f * wob)) : 0.f;
        float d = target - g_bars[i];
        g_bars[i] += d * (1.f - expf(-dt / (d > 0 ? 0.05f : 0.14f)));
        if (fabsf(d) > 0.004f) moving = true;
    }
    return moving;
}

// ---------------------------------------------------------------- system volume (scroll wheel)
// mingw only ships partial headers for this interface, so declare the vtable ourselves.
struct IEndpointVolX : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE RegisterControlChangeNotify(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterControlChangeNotify(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMasterVolumeLevel(float, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMasterVolumeLevelScalar(float, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMasterVolumeLevel(float*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMasterVolumeLevelScalar(float*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetChannelVolumeLevel(UINT, float, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetChannelVolumeLevelScalar(UINT, float, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelVolumeLevel(UINT, float*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelVolumeLevelScalar(UINT, float*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMute(BOOL, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMute(BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVolumeStepInfo(UINT*, UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VolumeStepUp(const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VolumeStepDown(const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVolumeRange(float*, float*, float*) = 0;
};
static const GUID IID_EndpointVolX = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};
static winrt::com_ptr<IEndpointVolX> g_vol;
static float g_volShown = 0;       // value shown in the pill overlay
static double g_volShowUntil = 0;  // overlay visible until (NowSec)

static void AcquireVol() {
    static double last = 0;
    double now = NowSec();
    if (g_vol && now - last < 5.0) return;
    last = now;
    g_vol = nullptr;
    try {
        winrt::com_ptr<IMMDeviceEnumerator> en;
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator), en.put_void())))
            return;
        winrt::com_ptr<IMMDevice> dev;
        if (FAILED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, dev.put()))) return;
        dev->Activate(IID_EndpointVolX, CLSCTX_ALL, nullptr, g_vol.put_void());
    } catch (...) {
    }
}

static void AdjustVolume(int wheelDelta) {
    static int acc = 0;
    acc += wheelDelta;
    int notches = acc / 120;  // precision touchpads send small deltas: accumulate
    if (!notches) return;
    acc -= notches * 120;
    if (g_set.scrollInv) notches = -notches;
    AcquireVol();
    if (!g_vol) return;
    float cur = 0.f;
    if (FAILED(g_vol->GetMasterVolumeLevelScalar(&cur))) return;
    float nv = Cl(cur + notches * g_set.scrollStep / 100.f, 0.f, 1.f);
    if (notches > 0) g_vol->SetMute(FALSE, nullptr);
    g_vol->SetMasterVolumeLevelScalar(nv, nullptr);
    g_volShown = nv;
    g_volShowUntil = NowSec() + 1.3;
}

// ---------------------------------------------------------------- open the playing source
// Finds the window of the app that owns the media session (matched by AppUserModelId / exe
// name, preferring a window whose title contains the media title), brings it to the front and,
// for browsers, tries to select the tab through UI Automation. Runs on a short-lived thread.
struct SrcReq {
    std::wstring sourceId, title;
};
static std::atomic<int> g_srcBusy{0};

static std::wstring Lower(std::wstring t) {
    for (auto& c : t) c = (wchar_t)towlower(c);
    return t;
}

static std::wstring WindowAumid(HWND h) {
    typedef HRESULT(WINAPI * PFN)(HWND, REFIID, void**);
    static PFN fn = (PFN)(void*)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "SHGetPropertyStoreForWindow");
    if (!fn) return L"";
    static const PROPERTYKEY kAumid = {{0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};
    IPropertyStore* ps = nullptr;
    if (FAILED(fn(h, __uuidof(IPropertyStore), (void**)&ps)) || !ps) return L"";
    PROPVARIANT pv;
    PropVariantInit(&pv);
    std::wstring r;
    if (SUCCEEDED(ps->GetValue(kAumid, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) r = pv.pwszVal;
    PropVariantClear(&pv);
    ps->Release();
    return r;
}

struct EnumCtx {
    std::wstring idN, titleN;
    HWND best = nullptr;
    int bestScore = 0;
};

static BOOL CALLBACK EnumSrc(HWND h, LPARAM lp) {
    auto* c = (EnumCtx*)lp;
    if (!IsWindowVisible(h) || GetWindow(h, GW_OWNER)) return TRUE;
    if (GetWindowLongW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    wchar_t title[512];
    if (GetWindowTextW(h, title, 512) <= 0) return TRUE;

    int score = 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hp) {
        wchar_t path[MAX_PATH];
        DWORD n = MAX_PATH;
        if (QueryFullProcessImageNameW(hp, 0, path, &n)) {
            const wchar_t* b = wcsrchr(path, L'\\');
            std::wstring stem = Lower(b ? b + 1 : path);
            if (stem.size() > 4 && stem.compare(stem.size() - 4, 4, L".exe") == 0) stem.resize(stem.size() - 4);
            if (stem.size() >= 3 && stem != L"applicationframehost" && c->idN.find(stem) != std::wstring::npos)
                score = 2;
        }
        CloseHandle(hp);
    }
    std::wstring aumid = Lower(WindowAumid(h));
    if (!aumid.empty() && (aumid == c->idN || c->idN.find(aumid) != std::wstring::npos)) score = 3;
    if (!score) return TRUE;
    if (!c->titleN.empty() && Lower(title).find(c->titleN) != std::wstring::npos) score += 4;
    if (score > c->bestScore) {  // EnumWindows walks top-down, so ties keep the topmost window
        c->bestScore = score;
        c->best = h;
    }
    return TRUE;
}

static void BringToFront(HWND h) {
    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    SetForegroundWindow(h);
    if (GetForegroundWindow() != h) SwitchToThisWindow(h, TRUE);
}

#if HAVE_UIA
static bool SelectTab(HWND win, const std::wstring& titleN) {
    winrt::com_ptr<IUIAutomation> ua;
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                ua.put_void())))
        return false;
    winrt::com_ptr<IUIAutomationElement> root;
    if (FAILED(ua->ElementFromHandle(win, root.put())) || !root) return false;
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = 50019;  // UIA_TabItemControlTypeId
    winrt::com_ptr<IUIAutomationCondition> cond;
    if (FAILED(ua->CreatePropertyCondition(30003 /*UIA_ControlTypePropertyId*/, v, cond.put())) || !cond) return false;
    winrt::com_ptr<IUIAutomationElementArray> arr;
    if (FAILED(root->FindAll((TreeScope)4 /*Descendants*/, cond.get(), arr.put())) || !arr) return false;
    int n = 0;
    arr->get_Length(&n);
    for (int i = 0; i < n; i++) {
        winrt::com_ptr<IUIAutomationElement> el;
        if (FAILED(arr->GetElement(i, el.put())) || !el) continue;
        BSTR name = nullptr;
        if (FAILED(el->get_CurrentName(&name)) || !name) continue;
        std::wstring nm = Lower(name);
        SysFreeString(name);
        if (nm.find(titleN) == std::wstring::npos) continue;
        winrt::com_ptr<IUnknown> pat;
        if (SUCCEEDED(el->GetCurrentPattern(10010 /*SelectionItem*/, pat.put())) && pat) {
            winrt::com_ptr<IUIAutomationSelectionItemPattern> si;
            if (SUCCEEDED(pat->QueryInterface(__uuidof(IUIAutomationSelectionItemPattern), si.put_void())) && si) {
                si->Select();
                return true;
            }
        }
    }
    return false;
}
#endif

static DWORD WINAPI OpenSourceThread(LPVOID p) {
    std::unique_ptr<SrcReq> req((SrcReq*)p);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    EnumCtx c;
    c.idN = Lower(req->sourceId);
    c.titleN = Lower(req->title);
    EnumWindows(EnumSrc, (LPARAM)&c);
    if (c.best) {
        BringToFront(c.best);
#if HAVE_UIA
        if (!c.titleN.empty()) {
            wchar_t t[512] = L"";
            GetWindowTextW(c.best, t, 512);
            if (Lower(t).find(c.titleN) == std::wstring::npos && SelectTab(c.best, c.titleN)) BringToFront(c.best);
        }
#endif
    }
    CoUninitialize();
    g_srcBusy--;
    return 0;
}

static void OpenSource() {
    MediaState st = Snapshot();
    if (!st.hasSession || st.sourceId.empty()) return;
    g_srcBusy++;
    auto* r = new SrcReq{st.sourceId, st.title};
    HANDLE t = CreateThread(nullptr, 0, OpenSourceThread, r, 0, nullptr);
    if (t) {
        CloseHandle(t);
    } else {
        g_srcBusy--;
        delete r;
    }
}

// ---------------------------------------------------------------- glass windows
// Each window is a per-pixel-alpha layered window (UpdateLayeredWindow). The glass look is
// rendered by us: blurred snapshot of what is behind the window + tint + rim, clipped to an
// anti-aliased rounded shape. (SetWindowRgn/acrylic can only give jagged edges.)
struct Layered {
    HWND hwnd = nullptr;
    int x = 0, y = 0, w = 0, h = 0;
    float radius = 0;
    HDC mem = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldObj = nullptr;
    BYTE* bits = nullptr;
    std::unique_ptr<Gdiplus::Bitmap> frame;     // GDI+ view of the DIB
    std::unique_ptr<Gdiplus::Bitmap> backdrop;  // blurred screen snapshot (covers 'cap' rect)
    int capX = 0, capY = 0, capW = 0, capH = 0;
    RECT want{};                                // screen rect the snapshot should cover
    std::vector<BYTE> glassPix;                 // pre-rendered glass (shape+blur+tint+rim)
    bool needCapture = true, glassDirty = true;
    double lastCapture = 0;
};
static Layered g_pillL, g_cardL;

// animated cover tint (0..255 per channel, alpha factor 0..1)
static float g_acc[3] = {0, 0, 0};
static float g_accA = 0;

static bool UpdateAccent(const MediaState& st, float dt) {
    float ta = (g_set.coverTint && st.hasAccent) ? 1.f : 0.f;
    float tc[3] = {(float)(st.accent & 0xFF), (float)((st.accent >> 8) & 0xFF), (float)((st.accent >> 16) & 0xFF)};
    bool changed = false;
    float k = 1.f - expf(-dt / 0.3f);
    if (ta > 0.f && g_accA < 0.03f)
        for (int i = 0; i < 3; i++) g_acc[i] = tc[i];  // fade in using the new color
    if (ta > 0.f)
        for (int i = 0; i < 3; i++)
            if (fabsf(tc[i] - g_acc[i]) > 0.4f) {
                g_acc[i] += (tc[i] - g_acc[i]) * k;
                changed = true;
            }
    if (fabsf(ta - g_accA) > 0.004f) {
        g_accA += (ta - g_accA) * k;
        changed = true;
    } else if (g_accA != ta) {
        g_accA = ta;
        changed = true;
    }
    if (changed) g_pillL.glassDirty = g_cardL.glassDirty = true;
    return changed;
}

static void PresentL(Layered& L, BYTE alpha = 255) {
    if (!L.hwnd || !L.mem) return;
    POINT dst{L.x, L.y}, src{0, 0};
    SIZE sz{L.w, L.h};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(L.hwnd, nullptr, &dst, &sz, L.mem, &src, 0, &bf, ULW_ALPHA);
}

static void FreeSurface(Layered& L) {
    L.frame.reset();
    if (L.mem) {
        if (L.oldObj) SelectObject(L.mem, L.oldObj);
        DeleteDC(L.mem);
    }
    if (L.dib) DeleteObject(L.dib);
    L.mem = nullptr;
    L.dib = nullptr;
    L.oldObj = nullptr;
    L.bits = nullptr;
    L.w = L.h = 0;
}

static BITMAPINFO MakeBmi(int w, int h) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    return bi;
}

// returns true if the surface was (re)created
static bool EnsureSize(Layered& L, int w, int h) {
    if (L.w == w && L.h == h && L.bits) return false;
    FreeSurface(L);
    BITMAPINFO bi = MakeBmi(w, h);
    HDC scr = GetDC(nullptr);
    L.mem = CreateCompatibleDC(scr);
    L.dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void**)&L.bits, nullptr, 0);
    ReleaseDC(nullptr, scr);
    L.oldObj = SelectObject(L.mem, L.dib);
    L.w = w;
    L.h = h;
    L.frame = std::make_unique<Gdiplus::Bitmap>(w, h, w * 4, PixelFormat32bppPARGB, L.bits);
    L.glassPix.assign((size_t)w * h * 4, 0);
    L.glassDirty = true;
    return true;
}

static bool CapOk(const Layered& L) {
    return L.backdrop && L.x >= L.capX && L.y >= L.capY && L.x + L.w <= L.capX + L.capW &&
           L.y + L.h <= L.capY + L.capH;
}

static void BuildBackdrop(Layered& L, Gdiplus::Bitmap& cap, int cw, int ch) {
    auto shrink = [](Gdiplus::Image& src, int sw, int sh, int tw, int th) {
        auto out = std::make_unique<Gdiplus::Bitmap>(tw, th, PixelFormat32bppPARGB);
        Gdiplus::Graphics g(out.get());
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        Gdiplus::ImageAttributes ia;
        ia.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
        g.DrawImage(&src, Gdiplus::Rect(0, 0, tw, th), 0, 0, sw, sh, Gdiplus::UnitPixel, &ia);
        return out;
    };
    int w1 = Mx(8, cw / 4), h1 = Mx(2, ch / 4);
    auto s1 = shrink(cap, cw, ch, w1, h1);
    int w2 = Mx(4, cw / 12), h2 = Mx(2, ch / 12);
    auto s2 = shrink(*s1, w1, h1, w2, h2);

    L.backdrop = std::make_unique<Gdiplus::Bitmap>(cw, ch, PixelFormat32bppPARGB);
    Gdiplus::Graphics g(L.backdrop.get());
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    Gdiplus::ImageAttributes ia;
    ia.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
    if (g_set.theme == 2) {  // liquid glass: richer, more saturated backdrop
        const float s = 1.7f, lr = 0.3086f, lg = 0.6094f, lb = 0.0820f;
        Gdiplus::ColorMatrix cm = {{{(1 - s) * lr + s, (1 - s) * lr, (1 - s) * lr, 0, 0},
                                    {(1 - s) * lg, (1 - s) * lg + s, (1 - s) * lg, 0, 0},
                                    {(1 - s) * lb, (1 - s) * lb, (1 - s) * lb + s, 0, 0},
                                    {0, 0, 0, 1, 0},
                                    {0.03f, 0.03f, 0.03f, 0, 1}}};
        ia.SetColorMatrix(&cm, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
    }
    g.DrawImage(s2.get(), Gdiplus::Rect(0, 0, cw, ch), 0, 0, w2, h2, Gdiplus::UnitPixel, &ia);
}

// Snapshot the screen behind the window. A visible window is made fully transparent for
// ~2 frames so it does not capture itself.
static void CaptureBackdrop(Layered& L) {
    int cx = L.want.left, cy = L.want.top, cw = L.want.right - L.want.left, ch = L.want.bottom - L.want.top;
    if (cw <= 0 || ch <= 0) return;
    bool vis = L.hwnd && IsWindowVisible(L.hwnd);
    if (vis) {
        PresentL(L, 0);
        DwmFlush();
        DwmFlush();
    }
    BITMAPINFO bi = MakeBmi(cw, ch);
    HDC scr = GetDC(nullptr);
    HDC mdc = CreateCompatibleDC(scr);
    BYTE* bits = nullptr;
    HBITMAP dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
    if (dib && bits) {
        HGDIOBJ old = SelectObject(mdc, dib);
        BitBlt(mdc, 0, 0, cw, ch, scr, cx, cy, SRCCOPY | CAPTUREBLT);
        GdiFlush();
        {
            Gdiplus::Bitmap cap(cw, ch, cw * 4, PixelFormat32bppRGB, bits);
            BuildBackdrop(L, cap, cw, ch);
        }
        SelectObject(mdc, old);
        L.capX = cx;
        L.capY = cy;
        L.capW = cw;
        L.capH = ch;
    }
    if (dib) DeleteObject(dib);
    DeleteDC(mdc);
    ReleaseDC(nullptr, scr);
    L.lastCapture = NowSec();
    L.needCapture = false;
    L.glassDirty = true;
}

// ---------------------------------------------------------------- drawing helpers
static Gdiplus::FontFamily* Family() {
    static std::unique_ptr<Gdiplus::FontFamily> f;
    if (!f) {
        f = std::make_unique<Gdiplus::FontFamily>(L"Segoe UI Variable Text");
        if (!f->IsAvailable()) f = std::make_unique<Gdiplus::FontFamily>(L"Segoe UI");
    }
    return f.get();
}

static void RoundRect(Gdiplus::GraphicsPath& p, float x, float y, float w, float h, float r) {
    r = Mn(r, Mn(w, h) / 2);
    float d = r * 2;
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}

static Gdiplus::Color TextColor(int a = 255) {
    return g_set.theme == 1 ? Gdiplus::Color(a, 22, 22, 26) : Gdiplus::Color(a, 255, 255, 255);
}

// Pre-render the static glass: anti-aliased shape filled with the blurred backdrop, tinted,
// with theme-specific highlights and rim.
static void BuildGlass(Layered& L) {
    int w = L.w, h = L.h;
    if (!L.frame || w <= 0 || h <= 0) return;
    {
        Gdiplus::Graphics g(L.frame.get());
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

        Gdiplus::GraphicsPath path;
        RoundRect(path, 0.5f, 0.5f, w - 1.f, h - 1.f, L.radius);

        if (L.backdrop) {
            Gdiplus::TextureBrush tb(L.backdrop.get(), Gdiplus::WrapModeClamp);
            tb.TranslateTransform((float)(L.capX - L.x), (float)(L.capY - L.y));
            g.FillPath(&tb, &path);
        } else {
            Gdiplus::SolidBrush b(Gdiplus::Color(255, 28, 28, 32));
            g.FillPath(&b, &path);
        }

        int th = g_set.theme, t = g_set.tint;
        auto applyAccent = [&]() {
            if (g_accA < 0.01f) return;
            int a = (int)((60 + t * 0.5f + (th == 2 ? 25 : 0)) * g_accA);
            Gdiplus::SolidBrush ab(Gdiplus::Color((BYTE)Cl(a, 0, 255), (BYTE)Cl(g_acc[0], 0.f, 255.f),
                                                  (BYTE)Cl(g_acc[1], 0.f, 255.f), (BYTE)Cl(g_acc[2], 0.f, 255.f)));
            g.FillPath(&ab, &path);
        };
        if (th == 0) {
            Gdiplus::SolidBrush b(Gdiplus::Color((BYTE)Cl(70 + t * 2, 0, 255), 22, 22, 26));
            g.FillPath(&b, &path);
            applyAccent();
        } else if (th == 1) {
            Gdiplus::SolidBrush b(Gdiplus::Color((BYTE)Cl(70 + t * 2, 0, 255), 244, 244, 247));
            g.FillPath(&b, &path);
            applyAccent();
        } else {
            Gdiplus::SolidBrush under(Gdiplus::Color((BYTE)Cl(35 + t / 2, 0, 255), 10, 12, 20));
            g.FillPath(&under, &path);
            Gdiplus::SolidBrush white(Gdiplus::Color((BYTE)Cl(14 + t / 2, 0, 255), 255, 255, 255));
            g.FillPath(&white, &path);
            applyAccent();
            // specular sheen across the top half
            Gdiplus::LinearGradientBrush sheen(Gdiplus::PointF(0, 0), Gdiplus::PointF(0, (float)h),
                                               Gdiplus::Color(90, 255, 255, 255), Gdiplus::Color(0, 255, 255, 255));
            Gdiplus::Color sc[3] = {Gdiplus::Color(100, 255, 255, 255), Gdiplus::Color(14, 255, 255, 255),
                                    Gdiplus::Color(0, 255, 255, 255)};
            Gdiplus::REAL sp[3] = {0.f, 0.5f, 1.f};
            sheen.SetInterpolationColors(sc, sp, 3);
            g.FillPath(&sheen, &path);
        }

        // rim
        if (th == 2) {
            Gdiplus::LinearGradientBrush rb(Gdiplus::PointF(0, 0), Gdiplus::PointF((float)w, (float)h),
                                            Gdiplus::Color(235, 255, 255, 255), Gdiplus::Color(190, 255, 255, 255));
            Gdiplus::Color rc[4] = {Gdiplus::Color(235, 255, 255, 255), Gdiplus::Color(80, 150, 210, 255),
                                    Gdiplus::Color(80, 255, 170, 215), Gdiplus::Color(190, 255, 255, 255)};
            Gdiplus::REAL rp[4] = {0.f, 0.38f, 0.7f, 1.f};
            rb.SetInterpolationColors(rc, rp, 4);
            Gdiplus::Pen pen(&rb, 1.4f);
            g.DrawPath(&pen, &path);
            Gdiplus::GraphicsPath inner;
            RoundRect(inner, 2.f, 2.f, w - 4.f, h - 4.f, Mx(1.f, L.radius - 1.5f));
            Gdiplus::Pen p2(Gdiplus::Color(36, 255, 255, 255), 1.f);
            g.DrawPath(&p2, &inner);
        } else {
            Gdiplus::Pen pen(th == 1 ? Gdiplus::Color(60, 0, 0, 0) : Gdiplus::Color(62, 255, 255, 255), 1.2f);
            g.DrawPath(&pen, &path);
        }
        g.Flush(Gdiplus::FlushIntentionSync);
    }
    memcpy(L.glassPix.data(), L.bits, (size_t)w * h * 4);
    L.glassDirty = false;
}

static void PrepareGlass(Layered& L) {
    if (L.needCapture || !CapOk(L)) CaptureBackdrop(L);
    if (L.glassDirty) BuildGlass(L);
    memcpy(L.bits, L.glassPix.data(), (size_t)L.w * L.h * 4);
}

static void SetupG(Gdiplus::Graphics& g) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
}

static void MakeFmt(Gdiplus::StringFormat& f) {
    f.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsNoClip);
    f.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
}

// cover scaled once to its exact on-screen size (crisp), then drawn through an AA rounded path
struct ScaledSlot {
    std::shared_ptr<Gdiplus::Bitmap> keep;
    int size = 0;
    std::unique_ptr<Gdiplus::Bitmap> bmp;
};
static ScaledSlot g_scaled[2];
static int g_scaledNext = 0;

static Gdiplus::Bitmap* ScaledCover(const std::shared_ptr<Gdiplus::Bitmap>& src, int size) {
    for (auto& s : g_scaled)
        if (s.keep == src && s.size == size && s.bmp) return s.bmp.get();
    auto& s = g_scaled[g_scaledNext];
    g_scaledNext ^= 1;
    s.keep = src;
    s.size = size;
    s.bmp = std::make_unique<Gdiplus::Bitmap>(size, size, PixelFormat32bppPARGB);
    Gdiplus::Graphics g(s.bmp.get());
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    Gdiplus::ImageAttributes ia;
    ia.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
    g.DrawImage(src.get(), Gdiplus::Rect(0, 0, size, size), 0, 0, (int)src->GetWidth(), (int)src->GetHeight(),
                Gdiplus::UnitPixel, &ia);
    return s.bmp.get();
}

static void DrawCover(Gdiplus::Graphics& g, const MediaState& st, float x, float y, float s, float r) {
    int si = Mx(4, (int)lroundf(s));
    float xi = floorf(x + 0.5f), yi = floorf(y + 0.5f);
    Gdiplus::GraphicsPath p;
    RoundRect(p, xi, yi, (float)si, (float)si, r);
    if (st.cover) {
        Gdiplus::TextureBrush tb(ScaledCover(st.cover, si), Gdiplus::WrapModeClamp);
        tb.TranslateTransform(xi, yi);
        g.FillPath(&tb, &p);
    } else {
        Gdiplus::SolidBrush b(Gdiplus::Color(50, 255, 255, 255));
        g.FillPath(&b, &p);
        Gdiplus::Font f(Family(), si * 0.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush t(TextColor(200));
        Gdiplus::StringFormat sf;
        sf.SetAlignment(Gdiplus::StringAlignmentCenter);
        sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(L"\u266A", -1, &f, Gdiplus::RectF(xi, yi, (float)si, (float)si), &sf, &t);
    }
}

static void DrawBars(Gdiplus::Graphics& g, float x, float cy, float barW, float gap, float maxH, int n) {
    Gdiplus::SolidBrush b(TextColor(215));
    for (int i = 0; i < n; i++) {
        int src = n == 5 ? i : (i * 5 / n);
        float bh = Mx(barW, g_bars[src] * maxH);
        Gdiplus::GraphicsPath p;
        RoundRect(p, x + i * (barW + gap), cy - bh / 2, barW, bh, barW / 2);
        g.FillPath(&b, &p);
    }
}

// ---- pill geometry
struct PillGeom {
    float pad, cs, gap, vizBarW, vizGap, vizW, textX, rightPad, titlePx, artistPx;
};
static float g_titleW = 0, g_timeW = 0;           // cached text widths (px)
static double g_marqStart = 0;                    // marquee restart time (NowSec)
static bool g_marquee = false;                    // title currently scrolling
static std::unique_ptr<Gdiplus::Bitmap> g_strip;  // pre-rendered scrolling title
static std::wstring g_stripKey;

// hover animation state
static float g_hover = 0, g_hoverT = 0;                      // pill
static float g_cardHov[4] = {0, 0, 0, 0}, g_cardHovT[4] = {0, 0, 0, 0};  // prev, play, next, header
static int g_cardHit = -1;
static bool g_pillTracking = false, g_cardTracking = false;

static PillGeom GeomFor(int h) {
    PillGeom g;
    g.pad = h * 0.13f;
    g.cs = h - 2 * g.pad;
    g.gap = h * 0.22f;
    g.vizBarW = Mx(2.f, h * 0.085f);
    g.vizGap = g.vizBarW * 0.8f;
    g.vizW = g.vizBarW * 5 + g.vizGap * 4;
    g.textX = g.pad + g.cs + g.gap;
    g.rightPad = h * 0.45f;
    g.titlePx = h * 0.30f;
    g.artistPx = h * 0.23f;
    return g;
}

static std::wstring PillTitle(const MediaState& st) {
    if (!st.hasSession) return L"Nothing playing";
    return st.title.empty() ? L"Unknown" : st.title;
}

static std::wstring FmtTime(int64_t ticks) {
    int64_t s = Mx<int64_t>(0, ticks / 10000000);
    wchar_t b[32];
    if (s >= 3600) swprintf(b, 32, L"%d:%02d:%02d", (int)(s / 3600), (int)(s / 60 % 60), (int)(s % 60));
    else swprintf(b, 32, L"%d:%02d", (int)(s / 60), (int)(s % 60));
    return b;
}

// "0:48 / 2:32"; normalized=true turns digits into 0 so the measured width is stable
static std::wstring TimeStr(const MediaState& st, bool normalized) {
    int64_t span = st.endTicks - st.startTicks;
    if (!st.hasSession || span <= 0) return L"";
    std::wstring t = FmtTime(CurrentPos(st) - st.startTicks) + L" / " + FmtTime(span);
    if (normalized)
        for (auto& c : t)
            if (c >= L'0' && c <= L'9') c = L'0';
    return t;
}

static float MeasureW(const std::wstring& s, float px, bool bold) {
    if (s.empty()) return 0;
    HDC dc = GetDC(nullptr);
    float w = 0;
    {
        Gdiplus::Graphics g(dc);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        Gdiplus::Font f(Family(), px, bold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        MakeFmt(fmt);
        Gdiplus::RectF r;
        g.MeasureString(s.c_str(), -1, &f, Gdiplus::PointF(0, 0), &fmt, &r);
        w = r.Width;
    }
    ReleaseDC(nullptr, dc);
    return w;
}

static void RenderPill() {
    if (!g_pill) return;
    Layered& L = g_pillL;
    if (!L.w || !L.h || !L.frame) return;
    int w = L.w, h = L.h;
    MediaState st = Snapshot();
    PillGeom G = GeomFor(h);
    PrepareGlass(L);
    {
        Gdiplus::Graphics g(L.frame.get());
        SetupG(g);

        if (g_hover > 0.01f) {  // hover highlight
            Gdiplus::GraphicsPath hp;
            RoundRect(hp, 0.5f, 0.5f, w - 1.f, h - 1.f, L.radius);
            Gdiplus::SolidBrush hb(TextColor((int)(g_hover * 30)));
            g.FillPath(&hb, &hp);
            Gdiplus::Pen hpn(TextColor((int)(g_hover * 110)), 1.2f);
            g.DrawPath(&hpn, &hp);
        }

        DrawCover(g, st, G.pad, G.pad, G.cs, G.cs * 0.28f);

        float tw = Mx(1.f, w - G.textX - G.rightPad - (g_set.viz ? G.vizW + G.gap : 0.f));
        if (g_set.viz) DrawBars(g, w - G.rightPad - G.vizW, h / 2.f, G.vizBarW, G.vizGap, h * 0.5f, 5);

        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        MakeFmt(fmt);
        Gdiplus::Font ft(Family(), G.titlePx, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font fa(Family(), G.artistPx, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush bt(TextColor(255)), ba(TextColor(180));
        std::wstring title = PillTitle(st);
        std::wstring tstr = TimeStr(st, false);
        bool volMode = NowSec() < g_volShowUntil;  // scroll-volume overlay
        bool two = !volMode && st.hasSession && (!st.artist.empty() || !tstr.empty());
        float lh1 = G.titlePx * 1.4f, lh2 = (two || volMode) ? G.artistPx * 1.4f : 0;
        float y0 = (h - (lh1 + lh2)) / 2.f;

        Gdiplus::RectF tr(G.textX, y0, tw, lh1);
        g_marquee = g_titleW > tw + 1;
        if (volMode) {
            g_marquee = false;
            std::wstring vt = L"Volume  " + std::to_wstring((int)lroundf(g_volShown * 100.f)) + L"%";
            g.DrawString(vt.c_str(), -1, &ft, tr, &fmt, &bt);
            float bh = Mx(3.f, h * 0.09f), by = y0 + lh1 + (lh2 - bh) / 2.f;
            Gdiplus::GraphicsPath tp, fp;
            RoundRect(tp, G.textX, by, tw, bh, bh / 2);
            Gdiplus::SolidBrush trk(TextColor(60));
            g.FillPath(&trk, &tp);
            RoundRect(fp, G.textX, by, Mx(bh, tw * g_volShown), bh, bh / 2);
            Gdiplus::SolidBrush fl(TextColor(235));
            g.FillPath(&fl, &fp);
        } else if (g_marquee) {
            float gapPx = h * 1.2f, speed = h * 1.1f, pause = 1.5f;
            float cycle = g_titleW + gapPx;
            double period = pause + cycle / speed + 0.2;
            double t = fmod(NowSec() - g_marqStart, period);
            float off = 0.f;
            if (t > pause) {  // ease-in, then constant speed
                double u = t - pause;
                off = (float)(speed * (u < 0.4 ? u * u / 0.8 : u - 0.2));
            }
            off = Mn(off, cycle);
            int sw = (int)ceilf(cycle) + 2, sh = (int)ceilf(lh1) + 1;
            std::wstring skey = title + L"|" + std::to_wstring(sw) + L"|" + std::to_wstring(sh) + L"|" +
                                std::to_wstring(g_set.theme);
            if (!g_strip || g_stripKey != skey) {
                g_strip = std::make_unique<Gdiplus::Bitmap>(sw, sh, PixelFormat32bppPARGB);
                Gdiplus::Graphics sg(g_strip.get());
                sg.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
                sg.Clear(Gdiplus::Color(0, 0, 0, 0));
                sg.DrawString(title.c_str(), -1, &ft, Gdiplus::PointF(0, 0), &fmt, &bt);
                g_stripKey = skey;
            }
            g.SetClip(Gdiplus::Rect((int)floorf(tr.X), (int)floorf(tr.Y), (int)ceilf(tr.Width), (int)ceilf(tr.Height)));
            g.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);  // sub-pixel smooth motion
            for (int k = 0; k < 2; k++)
                g.DrawImage(g_strip.get(), Gdiplus::RectF(G.textX - off + k * cycle, y0, (float)sw, (float)sh), 0.f,
                            0.f, (float)sw, (float)sh, Gdiplus::UnitPixel);
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            g.ResetClip();
        } else {
            g.DrawString(title.c_str(), -1, &ft, tr, &fmt, &bt);
        }

        if (two) {
            float timeW = tstr.empty() ? 0.f : g_timeW;
            float aw = Mx(1.f, tw - (timeW > 0 ? timeW + h * 0.3f : 0.f));
            if (!st.artist.empty())
                g.DrawString(st.artist.c_str(), -1, &fa, Gdiplus::RectF(G.textX, y0 + lh1, aw, lh2), &fmt, &ba);
            if (!tstr.empty()) {
                Gdiplus::StringFormat rf(Gdiplus::StringFormat::GenericTypographic());
                MakeFmt(rf);
                rf.SetAlignment(Gdiplus::StringAlignmentFar);
                g.DrawString(tstr.c_str(), -1, &fa, Gdiplus::RectF(G.textX + tw - timeW, y0 + lh1, timeW, lh2), &rf,
                             &ba);
            }
        }
        g.Flush(Gdiplus::FlushIntentionSync);
    }
    PresentL(L);
}

// ---- card
static const int kCardW = 380, kCardH = 214;
struct CardLayout {
    float sc, pad, cover;
    Gdiplus::RectF title, artist, bars, tPos, tEnd, bar, prev, play, next;
};
static float ScaleFor(HWND ref) {
    UINT d = ref ? GetDpiForWindow(ref) : 96;
    return (d ? d : 96) / 96.f;
}
static CardLayout CardLay(float sc) {
    CardLayout L;
    L.sc = sc;
    L.pad = 20 * sc;
    L.cover = 76 * sc;
    float W = kCardW * sc;
    float tx = L.pad + L.cover + 16 * sc;
    L.title = {tx, 22 * sc, W - tx - L.pad - 30 * sc, 28 * sc};
    L.artist = {tx, 52 * sc, W - tx - L.pad - 30 * sc, 24 * sc};
    L.bars = {W - L.pad - 18 * sc, 24 * sc, 18 * sc, 22 * sc};
    L.tPos = {L.pad, 112 * sc, 44 * sc, 18 * sc};
    L.tEnd = {W - L.pad - 44 * sc, 112 * sc, 44 * sc, 18 * sc};
    L.bar = {L.pad + 50 * sc, 119 * sc, W - 2 * L.pad - 100 * sc, 4 * sc};
    float cy = 168 * sc, bs = 44 * sc;
    L.play = {W / 2 - bs / 2, cy - bs / 2, bs, bs};
    L.prev = {W / 2 - bs / 2 - 76 * sc, cy - 18 * sc, 36 * sc, 36 * sc};
    L.next = {W / 2 + bs / 2 + 40 * sc, cy - 18 * sc, 36 * sc, 36 * sc};
    return L;
}

static void Tri(Gdiplus::Graphics& g, Gdiplus::Brush* b, float x1, float y1, float x2, float y2, float x3, float y3) {
    Gdiplus::PointF p[3] = {{x1, y1}, {x2, y2}, {x3, y3}};
    g.FillPolygon(b, p, 3);
}

static void RenderCard(bool force = false) {
    if (!g_card) return;
    if (!force && !IsWindowVisible(g_card)) return;
    Layered& L = g_cardL;
    if (!L.w || !L.h || !L.frame) return;
    float sc = L.w / (float)kCardW;
    CardLayout CL = CardLay(sc);
    MediaState st = Snapshot();
    PrepareGlass(L);
    {
        Gdiplus::Graphics g(L.frame.get());
        SetupG(g);

        DrawCover(g, st, CL.pad, 20 * sc, CL.cover, 14 * sc);
        if (g_cardHov[3] > 0.01f) {  // cover hover: dim + "open" arrow
            Gdiplus::GraphicsPath cp;
            RoundRect(cp, floorf(CL.pad + 0.5f), floorf(20 * sc + 0.5f), (float)lroundf(CL.cover),
                      (float)lroundf(CL.cover), 14 * sc);
            Gdiplus::SolidBrush dim(Gdiplus::Color((BYTE)(g_cardHov[3] * 90), 0, 0, 0));
            g.FillPath(&dim, &cp);
            float cx = CL.pad + CL.cover / 2, cy = 20 * sc + CL.cover / 2, a = 9 * sc;
            Gdiplus::Pen ap(Gdiplus::Color((BYTE)(g_cardHov[3] * 245), 255, 255, 255), 2.2f * sc);
            ap.SetStartCap(Gdiplus::LineCapRound);
            ap.SetEndCap(Gdiplus::LineCapRound);
            g.DrawLine(&ap, cx - a, cy + a, cx + a, cy - a);
            g.DrawLine(&ap, cx + a * 0.1f, cy - a, cx + a, cy - a);
            g.DrawLine(&ap, cx + a, cy - a, cx + a, cy - a * 0.1f);
        }

        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        MakeFmt(fmt);
        Gdiplus::Font ft(Family(), 20 * sc, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font fa(Family(), 15 * sc, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font fs(Family(), 11.5f * sc, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush bt(TextColor(255)), ba(TextColor(175)), bs(TextColor(160));
        g.DrawString(PillTitle(st).c_str(), -1, &ft, CL.title, &fmt, &bt);
        g.DrawString(st.artist.c_str(), -1, &fa, CL.artist, &fmt, &ba);

        if (g_set.viz)
            DrawBars(g, CL.bars.X, CL.bars.Y + CL.bars.Height / 2, 3 * sc, 2.5f * sc, CL.bars.Height, 4);

        int64_t pos = CurrentPos(st);
        int64_t span = st.endTicks - st.startTicks;
        float frac = span > 0 ? Cl((float)(pos - st.startTicks) / (float)span, 0.f, 1.f) : 0.f;
        Gdiplus::StringFormat right(Gdiplus::StringFormat::GenericTypographic());
        MakeFmt(right);
        right.SetAlignment(Gdiplus::StringAlignmentFar);
        g.DrawString(FmtTime(pos - st.startTicks).c_str(), -1, &fs, CL.tPos, &fmt, &bs);
        g.DrawString(FmtTime(span).c_str(), -1, &fs, CL.tEnd, &right, &bs);
        {
            Gdiplus::GraphicsPath tr, fl;
            RoundRect(tr, CL.bar.X, CL.bar.Y, CL.bar.Width, CL.bar.Height, CL.bar.Height / 2);
            Gdiplus::SolidBrush bg(TextColor(60));
            g.FillPath(&bg, &tr);
            float fw = Mx(CL.bar.Height, CL.bar.Width * frac);
            RoundRect(fl, CL.bar.X, CL.bar.Y, fw, CL.bar.Height, CL.bar.Height / 2);
            Gdiplus::SolidBrush fg(TextColor(235));
            g.FillPath(&fg, &fl);
            float kx = CL.bar.X + CL.bar.Width * frac, ky = CL.bar.Y + CL.bar.Height / 2, kr = 6 * sc;
            g.FillEllipse(&fg, kx - kr, ky - kr, kr * 2, kr * 2);
        }

        {  // button hover discs
            const Gdiplus::RectF* rs[3] = {&CL.prev, &CL.play, &CL.next};
            for (int i = 0; i < 3; i++) {
                if (g_cardHov[i] < 0.01f) continue;
                float cx = rs[i]->X + rs[i]->Width / 2, cy = rs[i]->Y + rs[i]->Height / 2, r = 24 * sc;
                Gdiplus::SolidBrush hb(TextColor((int)(g_cardHov[i] * 40)));
                g.FillEllipse(&hb, cx - r, cy - r, r * 2, r * 2);
            }
        }
        Gdiplus::SolidBrush ic(TextColor(st.canPrev ? 245 : 110));
        Gdiplus::SolidBrush icN(TextColor(st.canNext ? 245 : 110));
        Gdiplus::SolidBrush icP(TextColor(255));
        auto pc = [](const Gdiplus::RectF& r) { return Gdiplus::PointF(r.X + r.Width / 2, r.Y + r.Height / 2); };
        {
            auto c = pc(CL.prev);
            float a = 7 * sc;
            Tri(g, &ic, c.X, c.Y, c.X + a * 1.5f, c.Y - a, c.X + a * 1.5f, c.Y + a);
            Tri(g, &ic, c.X - a * 1.5f, c.Y, c.X, c.Y - a, c.X, c.Y + a);
        }
        {
            auto c = pc(CL.next);
            float a = 7 * sc;
            Tri(g, &icN, c.X, c.Y, c.X - a * 1.5f, c.Y - a, c.X - a * 1.5f, c.Y + a);
            Tri(g, &icN, c.X + a * 1.5f, c.Y, c.X, c.Y - a, c.X, c.Y + a);
        }
        {
            Gdiplus::Pen ring(TextColor(150), 1.5f * sc);
            g.DrawEllipse(&ring, CL.play.X, CL.play.Y, CL.play.Width, CL.play.Height);
            auto c = pc(CL.play);
            float a = 9 * sc;
            if (st.playing) {
                Gdiplus::GraphicsPath p1, p2;
                RoundRect(p1, c.X - a * 0.9f, c.Y - a, a * 0.65f, a * 2, 1.5f * sc);
                RoundRect(p2, c.X + a * 0.25f, c.Y - a, a * 0.65f, a * 2, 1.5f * sc);
                g.FillPath(&icP, &p1);
                g.FillPath(&icP, &p2);
            } else {
                Tri(g, &icP, c.X - a * 0.6f, c.Y - a, c.X - a * 0.6f, c.Y + a, c.X + a, c.Y);
            }
        }
        g.Flush(Gdiplus::FlushIntentionSync);
    }
    PresentL(L);
}

// ---------------------------------------------------------------- layout / visibility
static bool g_suppressed = false;
static ULONGLONG g_cardHiddenAt = 0;
static std::wstring g_wKey;
static float g_wCache = 0;
static bool g_dirty = true;

static void HideCard() {
    if (g_card && IsWindowVisible(g_card)) {
        ShowWindow(g_card, SW_HIDE);
        g_cardHiddenAt = GetTickCount64();
    }
}

static void UpdateSuppression() {
    static int streak = 0;
    QUERY_USER_NOTIFICATION_STATE q;
    bool bad = false;
    if (SUCCEEDED(SHQueryUserNotificationState(&q)))
        bad = (q == QUNS_BUSY || q == QUNS_RUNNING_D3D_FULL_SCREEN || q == QUNS_PRESENTATION_MODE);
    // hide only after two consecutive checks (avoids flashes when apps briefly go fullscreen)
    if (bad) {
        if (++streak >= 2) g_suppressed = true;
    } else {
        streak = 0;
        g_suppressed = false;
    }
}

// If the taskbar got raised above the pill (task switch, Start, tab change...), raise the pill
// again immediately instead of waiting for the next slow tick (that gap was a visible flash).
static void RaiseIfCovered() {
    if (!g_pill || !g_taskbar || !IsWindowVisible(g_pill)) return;
    for (HWND w = GetWindow(g_pill, GW_HWNDPREV); w; w = GetWindow(w, GW_HWNDPREV)) {
        if (w == g_taskbar) {
            SetWindowPos(g_pill, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            break;
        }
    }
}

static void LayoutPill() {
    if (!g_pill) return;
    if (!g_taskbar || !IsWindow(g_taskbar)) g_taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!g_taskbar) return;

    RECT tb;
    GetWindowRect(g_taskbar, &tb);
    int tbW = tb.right - tb.left, tbH = tb.bottom - tb.top;

    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(g_taskbar, MONITOR_DEFAULTTOPRIMARY), &mi);
    RECT vis;
    bool onScreen = IntersectRect(&vis, &tb, &mi.rcMonitor) && (vis.bottom - vis.top) > tbH / 2;

    MediaState st = Snapshot();
    bool want = (st.hasSession || !g_set.hideIdle) && tbW >= tbH && onScreen && !g_suppressed &&
                IsWindowVisible(g_taskbar);
    if (!want) {
        if (IsWindowVisible(g_pill)) ShowWindow(g_pill, SW_HIDE);
        g_hoverT = 0;
        HideCard();
        return;
    }

    float sc = ScaleFor(g_taskbar);
    int h = Mx(20, tbH * g_set.heightPct / 100);
    PillGeom G = GeomFor(h);

    std::wstring title = PillTitle(st);
    std::wstring tnorm = TimeStr(st, true);
    std::wstring key = title + L"\x1" + st.artist + L"\x1" + tnorm + L"\x1" + std::to_wstring(h) + (g_set.viz ? L"v" : L"n");
    static std::wstring lastTitle;
    if (title + st.artist != lastTitle) {
        lastTitle = title + st.artist;
        g_marqStart = NowSec();  // restart scroll for a new track
    }
    if (key != g_wKey) {
        g_wKey = key;
        g_titleW = MeasureW(title, G.titlePx, true);
        float aw = MeasureW(st.artist, G.artistPx, false);
        g_timeW = MeasureW(tnorm, G.artistPx, false);
        float line2 = aw + (aw > 0 && g_timeW > 0 ? h * 0.3f : 0.f) + g_timeW;
        g_wCache = Mx(g_titleW, line2);
    }
    int w = (int)(G.textX + g_wCache + G.rightPad + (g_set.viz ? G.gap + G.vizW : 0.f) + 2);
    w = Cl(w, h * 2, (int)(g_set.maxW * sc));

    int margin = (int)(8 * sc);
    int x;
    if (g_set.pos == 0) {
        x = tb.left + margin;
    } else if (g_set.pos == 1) {
        HWND tray = FindWindowExW(g_taskbar, nullptr, L"TrayNotifyWnd", nullptr);
        RECT tr{};
        int trayLeft = tb.right - (int)(200 * sc);
        if (tray && GetWindowRect(tray, &tr) && tr.right > tr.left && tr.left > tb.left) trayLeft = tr.left;
        x = trayLeft - w - margin;
    } else {
        x = tb.left;
    }
    x += g_set.offX;
    int y = tb.top + (tbH - h) / 2 + g_set.offY;

    Layered& L = g_pillL;
    bool vis0 = IsWindowVisible(g_pill) != 0;
    bool resized = EnsureSize(L, w, h);
    bool moved = (L.x != x || L.y != y);
    L.x = x;
    L.y = y;
    L.radius = h / 2.f;
    // snapshot a band across the whole taskbar so width changes never need a re-capture
    L.want = RECT{Mn((int)tb.left, x), y, Mx((int)tb.right, x + w), y + h};
    if (resized || moved) {
        L.glassDirty = true;
        g_dirty = true;
    }
    if (!vis0) {
        L.needCapture = true;
        RenderPill();  // captures while hidden: no flicker
        ShowWindow(g_pill, SW_SHOWNOACTIVATE);
        SetWindowPos(g_pill, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else {
        RaiseIfCovered();
    }
}

static void ShowCard() {
    if (!g_card || !g_pill) return;
    float sc = ScaleFor(g_pill);
    int W = (int)(kCardW * sc), H = (int)(kCardH * sc);
    RECT pr;
    GetWindowRect(g_pill, &pr);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(g_pill, MONITOR_DEFAULTTOPRIMARY), &mi);
    int gap = (int)(12 * sc);
    int x = (pr.left + pr.right) / 2 - W / 2;
    x = Cl(x, (int)mi.rcMonitor.left + gap, Mx((int)mi.rcMonitor.left + gap, (int)mi.rcMonitor.right - W - gap));
    bool above = pr.top > (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
    int y = above ? pr.top - H - gap : pr.bottom + gap;

    Layered& L = g_cardL;
    EnsureSize(L, W, H);
    L.x = x;
    L.y = y;
    L.radius = 26 * sc;
    L.want = RECT{x, y, x + W, y + H};
    L.needCapture = true;  // card is hidden now, so the snapshot never contains itself
    L.glassDirty = true;
    RenderCard(true);
    ShowWindow(g_card, SW_SHOW);
    SetWindowPos(g_card, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetForegroundWindow(g_card);
}

// ---------------------------------------------------------------- window procs
static LRESULT CALLBACK PillProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_APP_STATE:
            g_dirty = true;
            LayoutPill();
            if (IsWindowVisible(h)) RenderPill();
            RenderCard();
            return 0;
        case WM_APP_SETTINGS:
            LoadSettings();
            g_wKey.clear();
            g_strip.reset();
            for (Layered* L : {&g_pillL, &g_cardL}) {
                L->needCapture = true;
                L->glassDirty = true;
            }
            LayoutPill();
            if (IsWindowVisible(h)) RenderPill();
            RenderCard();
            return 0;
        case WM_DISPLAYCHANGE:
            g_pillL.needCapture = true;
            g_dirty = true;
            return 0;
        case WM_SETTINGCHANGE:
            if (w == SPI_SETDESKWALLPAPER) {
                g_pillL.needCapture = true;
                g_dirty = true;
            }
            return 0;
        case WM_MOUSEMOVE:
            if (!g_pillTracking) {
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, h, 0};
                TrackMouseEvent(&t);
                g_pillTracking = true;
            }
            g_hoverT = 1.f;
            return 0;
        case WM_MOUSELEAVE:
            g_pillTracking = false;
            g_hoverT = 0.f;
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(l) == HTCLIENT) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        case WM_MOUSEWHEEL:
            if (g_set.scrollVol) {
                AdjustVolume(GET_WHEEL_DELTA_WPARAM(w));
                return 0;
            }
            break;
        case WM_LBUTTONUP:
            if (g_card && IsWindowVisible(g_card)) {
                HideCard();
            } else if (GetTickCount64() - g_cardHiddenAt > 250) {
                ShowCard();
            }
            return 0;
        case WM_MBUTTONUP:
            CmdToggle();
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProcW(h, m, w, l);
}

static bool HitIn(const Gdiplus::RectF& r, float x, float y, float grow = 0) {
    return x >= r.X - grow && x <= r.X + r.Width + grow && y >= r.Y - grow && y <= r.Y + r.Height + grow;
}

// 0 prev, 1 play, 2 next, 3 header (cover/title -> open source), 4 progress bar, -1 nothing
static int CardHit(float x, float y, const CardLayout& L) {
    if (HitIn(L.play, x, y)) return 1;
    if (HitIn(L.prev, x, y, 6)) return 0;
    if (HitIn(L.next, x, y, 6)) return 2;
    if (HitIn(L.bar, x, y, 10 * L.sc)) return 4;
    Gdiplus::RectF hd(L.pad, 20 * L.sc, L.title.X + L.title.Width - L.pad, L.cover);
    if (HitIn(hd, x, y)) return 3;
    return -1;
}

static LRESULT CALLBACK CardProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_ACTIVATE:
            if (LOWORD(w) == WA_INACTIVE) HideCard();
            return 0;
        case WM_MOUSEMOVE: {
            CardLayout L = CardLay(g_cardL.w / (float)kCardW);
            int idx = CardHit((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l), L);
            if (!g_cardTracking) {
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, h, 0};
                TrackMouseEvent(&t);
                g_cardTracking = true;
            }
            g_cardHit = idx;
            for (int i = 0; i < 4; i++) g_cardHovT[i] = (idx == i) ? 1.f : 0.f;
            return 0;
        }
        case WM_MOUSELEAVE:
            g_cardTracking = false;
            g_cardHit = -1;
            for (int i = 0; i < 4; i++) g_cardHovT[i] = 0.f;
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(l) == HTCLIENT) {
                SetCursor(LoadCursorW(nullptr, g_cardHit >= 0 ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_LBUTTONUP: {
            float x = (float)GET_X_LPARAM(l), y = (float)GET_Y_LPARAM(l);
            CardLayout L = CardLay(g_cardL.w / (float)kCardW);
            MediaState st = Snapshot();
            int idx = CardHit(x, y, L);
            if (idx == 1) CmdToggle();
            else if (idx == 0 && st.canPrev) CmdPrev();
            else if (idx == 2 && st.canNext) CmdNext();
            else if (idx == 4 && st.canSeek && st.endTicks > st.startTicks) {
                float f = Cl((x - L.bar.X) / L.bar.Width, 0.f, 1.f);
                CmdSeek(st.startTicks + (int64_t)(f * (double)(st.endTicks - st.startTicks)));
            } else if (idx == 3) {
                OpenSource();  // jump to the app / tab that is playing
                HideCard();
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProcW(h, m, w, l);
}

// ---------------------------------------------------------------- frame loop
// One animation frame. Returns true while something is animating (loop then syncs to vsync).
static bool Frame() {
    static double lastSlow = 0, lastMeter = 0, lastMedia = 0, lastRaise = 0, lastT = 0;
    static bool wasVol = false;
    double now = NowSec();
    float dt = lastT ? (float)Cl(now - lastT, 0.0005, 0.1) : 0.016f;
    lastT = now;

    if (!g_mgr && now - lastMedia >= 3.0) {  // media service may not be ready right after login
        lastMedia = now;
        StartMedia();
    }
    // session was gone for longer than the grace period -> really clear the state
    double gu = g_graceUntil;
    if (gu > 0 && now > gu) {
        g_graceUntil = 0;
        if (!GetSession()) {
            {
                std::lock_guard<std::mutex> l(g_mtx);
                g_state = MediaState{};
            }
            g_dirty = true;
            LayoutPill();
        }
    }

    MediaState st = Snapshot();
    bool moving = UpdateBars(st.playing);

    if (now - lastSlow >= 0.7) {
        lastSlow = now;
        UpdateSuppression();
        LayoutPill();
    }
    if (now - lastMeter >= 5.0) {
        lastMeter = now;
        AcquireMeter();
    }
    if (now - lastRaise >= 0.15) {
        lastRaise = now;
        RaiseIfCovered();
    }

    bool pillVis = g_pill && IsWindowVisible(g_pill);
    if (pillVis && g_set.refreshSec > 0 && g_pillL.lastCapture > 0 && now - g_pillL.lastCapture > g_set.refreshSec) {
        g_pillL.needCapture = true;  // optional refresh (makes the pill blink briefly)
        g_dirty = true;
    }

    // hover animation
    bool anim = false;
    float ht = g_set.hover ? g_hoverT : 0.f;
    float kh = 1.f - expf(-dt / 0.09f);
    if (fabsf(ht - g_hover) > 0.004f) {
        g_hover += (ht - g_hover) * kh;
        anim = true;
    } else {
        g_hover = ht;
    }
    for (int i = 0; i < 4; i++) {
        float t = g_set.hover ? g_cardHovT[i] : 0.f;
        if (fabsf(t - g_cardHov[i]) > 0.004f) {
            g_cardHov[i] += (t - g_cardHov[i]) * kh;
            anim = true;
        } else {
            g_cardHov[i] = t;
        }
    }

    bool accAnim = UpdateAccent(st, dt);
    bool volActive = now < g_volShowUntil;
    if (volActive != wasVol) {
        wasVol = volActive;
        g_dirty = true;
    }

    if (pillVis && (moving || anim || accAnim || volActive || g_dirty || g_marquee)) {
        RenderPill();
        g_dirty = false;
    }
    bool cardVis = g_card && IsWindowVisible(g_card);
    if (cardVis && (moving || st.playing || anim || accAnim)) RenderCard();
    return (pillVis && (moving || anim || accAnim || volActive || g_marquee)) || (cardVis && (st.playing || anim));
}

// WinEvent hook: raise the pill the instant the foreground window changes
static HWINEVENTHOOK g_hook = nullptr;
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) { RaiseIfCovered(); }

// ---------------------------------------------------------------- UI thread
static DWORD WINAPI UiThread(LPVOID) {
    // Wait (up to ~5 min) for this process to own the taskbar - needed right after boot/login.
    {
        bool found = false;
        for (int i = 0; i < 600 && g_running; i++) {
            HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (tb) {
                DWORD pid = 0;
                GetWindowThreadProcessId(tb, &pid);
                if (pid != GetCurrentProcessId()) return 0;  // some other explorer.exe process
                found = true;
                break;
            }
            Sleep(500);
        }
        if (!found) return 0;
    }
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    ULONG_PTR gdipTok = 0;
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup(&gdipTok, &gsi, nullptr);

    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSW wc{};
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"GlassMediaPill";
    wc.lpfnWndProc = PillProc;
    RegisterClassW(&wc);
    wc.lpszClassName = L"GlassMediaCard";
    wc.lpfnWndProc = CardProc;
    RegisterClassW(&wc);

    g_taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    g_pill = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"GlassMediaPill",
                             L"", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, inst, nullptr);
    g_card = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"GlassMediaCard", L"", WS_POPUP, 0,
                             0, 10, 10, nullptr, nullptr, inst, nullptr);
    g_pillL.hwnd = g_pill;
    g_cardL.hwnd = g_card;
    g_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0,
                             WINEVENT_OUTOFCONTEXT);

    AcquireMeter();
    StartMedia();
    UpdateSuppression();
    LayoutPill();

    bool quit = false;
    while (!quit) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) break;
        if (Frame()) {
            if (FAILED(DwmFlush())) Sleep(8);  // wait for the compositor = vsync-paced frames
        } else {
            MsgWaitForMultipleObjectsEx(0, nullptr, 100, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }

    if (g_hook) UnhookWinEvent(g_hook);
    g_hook = nullptr;
    StopMedia();
    g_meter = nullptr;
    g_vol = nullptr;
    g_strip.reset();
    for (auto& s : g_scaled) {
        s.bmp.reset();
        s.keep.reset();
    }
    for (Layered* L : {&g_pillL, &g_cardL}) {
        L->backdrop.reset();
        FreeSurface(*L);
        L->hwnd = nullptr;
    }
    DestroyWindow(g_card);
    DestroyWindow(g_pill);
    g_card = g_pill = nullptr;
    UnregisterClassW(L"GlassMediaPill", inst);
    UnregisterClassW(L"GlassMediaCard", inst);
    Gdiplus::GdiplusShutdown(gdipTok);
    winrt::uninit_apartment();
    return 0;
}

// ---------------------------------------------------------------- Windhawk entry points
BOOL Wh_ModInit() {
    // Only run in the explorer.exe instance that owns the taskbar. At boot the taskbar may not
    // exist yet: in that case keep going and let the UI thread wait for it.
    HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tb) {
        DWORD pid = 0;
        GetWindowThreadProcessId(tb, &pid);
        if (pid != GetCurrentProcessId()) return FALSE;
    }

    LoadSettings();
    g_running = true;
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    if (g_pill) PostMessageW(g_pill, WM_APP_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    g_running = false;
    for (int i = 0; i < 60 && g_srcBusy > 0; i++) Sleep(50);  // let a running "open source" finish
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}
