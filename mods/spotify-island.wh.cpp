// ==WindhawkMod==
// @id              spotify-island
// @name            Spotify Island
// @description     Dynamic-Island style now-playing pill: album art, art-colored side glow, liquid notch animation, audio-reactive bars, media controls.
// @version         2.4.1
// @author          you
// @github LOKPIYT
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spotify Island
A black Dynamic Island at the top of your screen showing what is playing.

* **Album art** + title / artist, read from Windows media sessions (works with
  the Spotify desktop app or the Microsoft Store version, or any player if you pick "Any app")
* **Liquid notch:** when attached to the top edge, the island flows out of the
  screen edge with curved shoulders and stretches smoothly left and right with a spring.
  The **melt length** of those shoulders is adjustable.
* **Side glow** in the main color of the current album art. It pulses with the
  real audio level of Spotify
* **Screen-side glow (optional):** a soft glow along the left and right edges of the
  whole monitor in the album color. It pulses on the beat of the song and a bright spot
  sweeps up and down the edges (opposite sides move in opposite directions)
* **Hover** to expand: big art, scrolling title, progress bar, prev / play-pause / next
* **Click the art/title** to open Spotify, **scroll** over the island to change volume
* **Right-click** (or Ctrl+Alt+`]`) to enter move mode, drag it, then right-click again to lock
* **Volume slider + seek bar:** in the expanded view, drag the slider next to the controls to set
  the system volume, and click or drag the progress bar to jump to any point of the song
* **Magnetic docking:** in move mode the island is pulled to the top or bottom screen edge when you
  bring it close (and holds on a little before letting go). Anywhere else it floats freely on
  the screen, so you can place it wherever you want.
* **Detach from the top:** in move mode drag the island down to float it away from the screen
  edge (drag it back up to re-attach), or just middle-click it. The notch shoulders melt away
  and the pill drops smoothly.
* Hides over fullscreen apps (optional)

Size follows your Windows display scaling automatically. The Size setting is
applied on top of that.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scale: 125
  $name: Size (%)
  $description: 50 - 300. Multiplied by your Windows display scaling.
- attachTop: true
  $name: Attach to the top edge (liquid notch)
  $description: Flows out of the screen edge. Turn off for a detached, floating island. You can also middle-click the island or drag it down in move mode.
- topGap: 14
  $name: Distance from the edge when detached with middle-click (px)
  $description: 0 - 150
- magnet: true
  $name: Magnetic docking to the top / bottom edge
  $description: While dragging in move mode, the island snaps to the nearest top or bottom edge. Turn off to place it freely.
- magnetRange: 46
  $name: Magnet range (px)
  $description: 10 - 200. How close to an edge the island has to get before it snaps.
- fps: 40
  $name: Animation frame rate cap (fps)
  $description: 15 - 90. Lower this if the island feels laggy, raise it for smoother motion. The screen-side glow is capped at 30 fps.
- meltLength: 13
  $name: Melt length (px)
  $description: How far the notch shoulders stretch sideways into the screen edge (0 - 80). The expanded size is scaled up automatically. Default 13.
- source: spotify
  $name: Media source
  $options:
  - spotify: Spotify only
  - any: Any app (current media session)
- artColor: true
  $name: Take colors from the album art
- accent: "#30D158"
  $name: Fallback accent color
  $description: Hex color used when there is no album art or art colors are off
- glow: 75
  $name: Island side glow strength (0 - 100)
- edgeGlow: false
  $name: Glow on the sides of the screen
- edgeWidth: 140
  $name: Screen-side glow width (px)
- edgeStrength: 60
  $name: Screen-side glow strength (0 - 100)
- edgeMotion: jump
  $name: Screen-side glow motion
  $options:
  - sweep: Up and down, sides opposite
  - sync: Up and down, sides together
  - static: Stay in place (pulse only)
  - jump: Jump on the beat (sides opposite)
- edgeSpeed: 100
  $name: Up/down motion speed (%)
  $description: 10 - 400
- edgeBeat: true
  $name: Pulse the screen-side glow on the beat
- islandBeat: true
  $name: Island bounces on the beat
- indicator: bars
  $name: Indicator style
  $options:
  - bars: Equalizer bars
  - dot: Pulsing dot
  - none: None
- audioReactive: true
  $name: React to the real audio level
- hideWhenIdle: false
  $name: Hide when nothing is playing
- hideFullscreen: true
  $name: Hide over fullscreen apps and games
- moveKey: "]"
  $name: Move-mode hotkey (used with Ctrl+Alt)
  $description: Right-clicking the island also toggles move mode
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <algorithm>
using std::max;
using std::min;
#include <windows.h>
#include <objidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shellscalingapi.h>
#include <gdiplus.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
namespace wf = winrt::Windows::Foundation;
namespace wmc = winrt::Windows::Media::Control;
namespace wss = winrt::Windows::Storage::Streams;

// ================================================================== layout (100%, 96 DPI)
namespace Lay {
constexpr float COL_H = 38, COL_W = 252, IDLE_W = 200, MOVE_W = 340;
constexpr float EXP_W = 450, EXP_H = 188;
constexpr float SIDE = 105;     // room for the side glow
constexpr float BOTTOM = 50;    // room for shadow / under-glow
constexpr float TOPROOM = 28;   // room above when floating
}  // namespace Lay

// ================================================================== small math
static inline float Clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float Smooth(float t) { t = Clamp01(t); return t * t * (3 - 2 * t); }
static inline BYTE A8(float a) { return (BYTE)(Clamp01(a) * 255.f + 0.5f); }
static inline float Stag(float ex, float start) { return Smooth((ex - start) / 0.34f); }

static uint32_t g_rng = 0x9E3779B9u;
static float Rand01() { g_rng = g_rng * 1664525u + 1013904223u; return (g_rng >> 8) / 16777216.f; }

struct Spring {
    float x = 0, v = 0;
    void Step(float target, float k, float c, float dt) {
        int n = (int)ceilf(dt * 240.f);
        n = std::clamp(n, 1, 24);
        float h = dt / n;
        for (int i = 0; i < n; i++) {
            float a = k * (target - x) - c * v;
            v += a * h;
            x += v * h;
        }
    }
    bool Settled(float target, float eps) const { return fabsf(target - x) < eps && fabsf(v) < eps * 8; }
};

static std::wstring Lower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

// ================================================================== settings
struct Settings {
    int scale = 125;
    bool attach = true, any = false, artColor = true, reactive = true, hideIdle = false, hideFs = true;
    int ar = 48, ag = 209, ab = 88;
    int glow = 75;
    int ind = 0;  // 0 bars, 1 dot, 2 none
    wchar_t key = L']';
    int melt = 13;
    bool edgeOn = false;
    int edgeW = 140, edgeStr = 60;
    int topGap = 14;
    int edgeMode = 0;  // 0 sweep (opposite), 1 sweep (together), 2 static, 3 jump on the beat
    int edgeSpd = 100;
    bool edgeBeat = true, islandBeat = true;
    bool magnet = true;
    int magnetR = 46;
    int fps = 40;
};
static Settings g_cfg;
static std::atomic<bool> g_sourceAny{false};

static void LoadSettings() {
    Settings c;
    c.scale = Wh_GetIntSetting(L"scale");
    if (c.scale < 50 || c.scale > 300) c.scale = 125;
    c.attach = Wh_GetIntSetting(L"attachTop") != 0;
    c.artColor = Wh_GetIntSetting(L"artColor") != 0;
    c.reactive = Wh_GetIntSetting(L"audioReactive") != 0;
    c.hideIdle = Wh_GetIntSetting(L"hideWhenIdle") != 0;
    c.hideFs = Wh_GetIntSetting(L"hideFullscreen") != 0;
    c.glow = std::clamp(Wh_GetIntSetting(L"glow"), 0, 100);
    c.melt = std::clamp(Wh_GetIntSetting(L"meltLength"), 0, 80);
    c.edgeOn = Wh_GetIntSetting(L"edgeGlow") != 0;
    c.edgeW = std::clamp(Wh_GetIntSetting(L"edgeWidth"), 20, 600);
    c.edgeStr = std::clamp(Wh_GetIntSetting(L"edgeStrength"), 0, 100);
    c.topGap = std::clamp(Wh_GetIntSetting(L"topGap"), 0, 150);
    c.edgeSpd = std::clamp(Wh_GetIntSetting(L"edgeSpeed"), 10, 400);
    c.edgeBeat = Wh_GetIntSetting(L"edgeBeat") != 0;
    c.islandBeat = Wh_GetIntSetting(L"islandBeat") != 0;
    c.magnet = Wh_GetIntSetting(L"magnet") != 0;
    c.magnetR = std::clamp(Wh_GetIntSetting(L"magnetRange"), 10, 200);
    c.fps = std::clamp(Wh_GetIntSetting(L"fps"), 15, 90);
    PCWSTR em = Wh_GetStringSetting(L"edgeMotion");
    c.edgeMode = (em && wcscmp(em, L"sync") == 0) ? 1 : (em && wcscmp(em, L"static") == 0) ? 2 : (em && wcscmp(em, L"jump") == 0) ? 3 : 0;
    Wh_FreeStringSetting(em);

    PCWSTR s = Wh_GetStringSetting(L"source");
    c.any = s && wcscmp(s, L"any") == 0;
    Wh_FreeStringSetting(s);

    PCWSTR ac = Wh_GetStringSetting(L"accent");
    if (ac && ac[0]) {
        const wchar_t* hx = ac[0] == L'#' ? ac + 1 : ac;
        if (wcslen(hx) == 6) {
            unsigned long v = wcstoul(hx, nullptr, 16);
            c.ar = (v >> 16) & 255; c.ag = (v >> 8) & 255; c.ab = v & 255;
        }
    }
    Wh_FreeStringSetting(ac);

    PCWSTR ind = Wh_GetStringSetting(L"indicator");
    c.ind = (ind && wcscmp(ind, L"dot") == 0) ? 1 : (ind && wcscmp(ind, L"none") == 0) ? 2 : 0;
    Wh_FreeStringSetting(ind);

    PCWSTR k = Wh_GetStringSetting(L"moveKey");
    c.key = (k && k[0]) ? k[0] : L']';
    Wh_FreeStringSetting(k);

    g_cfg = c;
    g_sourceAny = c.any;
}

// ================================================================== shared media state
struct Snap {
    std::wstring title, artist, app;
    int state = 0;  // 0 nothing, 1 paused, 2 playing
    double pos = 0, dur = 0;
    ULONGLONG posTick = 0;
};

static SRWLOCK g_lock = SRWLOCK_INIT;
static Snap g_snap;
static std::atomic<unsigned> g_snapVer{0};
static std::vector<uint8_t> g_artBytes;
static std::atomic<unsigned> g_artVer{0};
static std::atomic<int> g_cmd{0};  // 1 prev, 2 play/pause, 3 next, 4 seek
static std::atomic<double> g_seekSec{0};
static std::atomic<bool> g_quit{false};
static HANDLE g_wake = nullptr;

static bool IsSpotifyPid(DWORD pid) {
    bool ok = false;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        wchar_t buf[MAX_PATH * 2];
        DWORD sz = ARRAYSIZE(buf);
        if (QueryFullProcessImageNameW(h, 0, buf, &sz) && sz >= 11)
            ok = _wcsicmp(buf + sz - 11, L"spotify.exe") == 0;
        CloseHandle(h);
    }
    return ok;
}

// ------------------------------------------------------------------ fallback: Spotify window title
struct EnumData { std::wstring playing; bool found = false; };

static BOOL CALLBACK EnumCb(HWND hwnd, LPARAM lp) {
    auto* d = (EnumData*)lp;
    wchar_t cls[64];
    if (!GetClassNameW(hwnd, cls, 64) || wcscmp(cls, L"Chrome_WidgetWin_1") != 0) return TRUE;
    int len = GetWindowTextLengthW(hwnd);
    if (len <= 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!IsSpotifyPid(pid)) return TRUE;
    std::wstring t(len + 1, L'\0');
    GetWindowTextW(hwnd, &t[0], len + 1);
    t.resize(wcslen(t.c_str()));
    if (t.find(L" - ") != std::wstring::npos) d->playing = t;
    else if (t.rfind(L"Spotify", 0) == 0) d->found = true;
    return TRUE;
}

static void FallbackSpotify(Snap& s) {
    EnumData d;
    EnumWindows(EnumCb, (LPARAM)&d);
    if (!d.playing.empty()) {
        size_t p = d.playing.find(L" - ");
        s.artist = d.playing.substr(0, p);
        s.title = d.playing.substr(p + 3);
        s.state = 2;
    } else if (d.found) {
        s.title = L"Spotify";
        s.artist = L"Paused";
        s.state = 1;
    }
}

// ------------------------------------------------------------------ WinRT media (runs on its own MTA thread)
template <typename Op>
static auto Await(Op const& op, int ms = 2500) {
    if (op.wait_for(std::chrono::milliseconds(ms)) != wf::AsyncStatus::Completed) {
        try { op.Cancel(); } catch (...) {}
        throw winrt::hresult_error(E_ABORT);
    }
    return op.GetResults();
}

static double Sec(wf::TimeSpan t) { return std::chrono::duration<double>(t).count(); }

static uint64_t Fnv(const std::vector<uint8_t>& v) {
    uint64_t h = 1469598103934665603ull;
    for (uint8_t b : v) { h ^= b; h *= 1099511628211ull; }
    return h;
}

static wmc::GlobalSystemMediaTransportControlsSession PickSession(
    wmc::GlobalSystemMediaTransportControlsSessionManager const& mgr) {
    if (g_sourceAny) {
        auto cur = mgr.GetCurrentSession();
        if (cur) return cur;
    }
    wmc::GlobalSystemMediaTransportControlsSession first{nullptr};
    for (auto const& s : mgr.GetSessions()) {
        if (!first) first = s;
        if (Lower(s.SourceAppUserModelId().c_str()).find(L"spotify") != std::wstring::npos) return s;
    }
    return g_sourceAny ? first : nullptr;
}

static std::vector<uint8_t> ReadThumb(wss::IRandomAccessStreamReference const& ref) {
    std::vector<uint8_t> out;
    auto stream = Await(ref.OpenReadAsync());
    uint64_t size = stream.Size();
    if (size == 0 || size > 20ull * 1024 * 1024) return out;
    wss::Buffer buf((uint32_t)size);
    auto res = Await(stream.ReadAsync(buf, (uint32_t)size, wss::InputStreamOptions::None));
    out.resize(res.Length());
    if (!out.empty()) wss::DataReader::FromBuffer(res).ReadBytes(out);
    return out;
}

static void PublishArt(std::vector<uint8_t>&& bytes) {
    AcquireSRWLockExclusive(&g_lock);
    g_artBytes = std::move(bytes);
    ReleaseSRWLockExclusive(&g_lock);
    g_artVer++;
}

struct MediaLocal {
    std::wstring key;
    ULONGLONG keyTime = 0, nextArt = 0;
    int artTries = 0;
    uint64_t artHash = 0;
    bool gotArt = false;
};

static void PollMedia(wmc::GlobalSystemMediaTransportControlsSession const& ses, MediaLocal& L) {
    Snap s;
    ULONGLONG now = GetTickCount64();
    wss::IRandomAccessStreamReference thumb{nullptr};
    bool ok = false;

    if (ses) {
        try {
            using PS = wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus;
            auto st = ses.GetPlaybackInfo().PlaybackStatus();
            s.state = st == PS::Playing ? 2 : (st == PS::Closed ? 0 : 1);
            s.app = ses.SourceAppUserModelId().c_str();

            auto props = Await(ses.TryGetMediaPropertiesAsync());
            s.title = props.Title().c_str();
            s.artist = props.Artist().c_str();
            if (s.artist.empty()) s.artist = props.AlbumArtist().c_str();
            thumb = props.Thumbnail();

            auto tl = ses.GetTimelineProperties();
            double dur = Sec(tl.EndTime() - tl.StartTime());
            double pos = Sec(tl.Position() - tl.StartTime());
            if (s.state == 2) {
                double age = Sec(winrt::clock::now() - tl.LastUpdatedTime());
                if (age > 0 && age < 6 * 3600) pos += age;
            }
            s.dur = dur > 0 ? dur : 0;
            s.pos = dur > 0 ? std::clamp(pos, 0.0, dur) : 0;
            s.posTick = now;
            ok = true;
        } catch (...) {
        }
    }
    if (!ok && !g_sourceAny) FallbackSpotify(s);

    // ---- album art: fetch on track change, retry a few times (Spotify updates the thumbnail late)
    std::wstring key = s.title + L'\x1f' + s.artist;
    if (key != L.key) {
        L.key = key; L.keyTime = now; L.artTries = 0; L.nextArt = now; L.gotArt = false;
    }
    if (thumb && L.artTries < 4 && now >= L.nextArt) {
        L.artTries++;
        L.nextArt = now + (L.artTries == 1 ? 1000 : 2200);
        try {
            auto bytes = ReadThumb(thumb);
            if (!bytes.empty()) {
                L.gotArt = true;
                uint64_t h = Fnv(bytes);
                if (h != L.artHash) { L.artHash = h; PublishArt(std::move(bytes)); }
            }
        } catch (...) {
        }
    }
    if (!L.gotArt && L.artHash != 0 && now - L.keyTime > 3500 && (L.artTries >= 4 || !thumb)) {
        L.artHash = 0;
        PublishArt({});
    }

    AcquireSRWLockExclusive(&g_lock);
    g_snap = std::move(s);
    ReleaseSRWLockExclusive(&g_lock);
    g_snapVer++;
}

static void DoCommand(wmc::GlobalSystemMediaTransportControlsSession const& ses, int cmd) {
    bool done = false;
    if (ses) {
        try {
            if (cmd == 1) done = Await(ses.TrySkipPreviousAsync());
            else if (cmd == 2) done = Await(ses.TryTogglePlayPauseAsync());
            else if (cmd == 3) done = Await(ses.TrySkipNextAsync());
            else if (cmd == 4) done = Await(ses.TryChangePlaybackPositionAsync((int64_t)(g_seekSec.load() * 10000000.0)));
        } catch (...) {
        }
    }
    if (!done && cmd <= 3) {
        BYTE vk = cmd == 1 ? VK_MEDIA_PREV_TRACK : cmd == 2 ? VK_MEDIA_PLAY_PAUSE : VK_MEDIA_NEXT_TRACK;
        keybd_event(vk, 0, 0, 0);
        keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
    }
}

static DWORD WINAPI MediaThread(LPVOID) {
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) {}
    {
        wmc::GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
        ULONGLONG nextMgrTry = 0;
        MediaLocal L;
        while (!g_quit) {
            ULONGLONG now = GetTickCount64();
            if (!mgr && now >= nextMgrTry) {
                nextMgrTry = now + 5000;
                try {
                    mgr = Await(wmc::GlobalSystemMediaTransportControlsSessionManager::RequestAsync(), 5000);
                } catch (...) {
                    mgr = nullptr;
                }
            }
            wmc::GlobalSystemMediaTransportControlsSession ses{nullptr};
            if (mgr) {
                try { ses = PickSession(mgr); } catch (...) { mgr = nullptr; }
            }
            int cmd = g_cmd.exchange(0);
            if (cmd) DoCommand(ses, cmd);
            if (g_quit) break;
            PollMedia(ses, L);
            WaitForSingleObject(g_wake, cmd ? 120 : 350);
        }
    }
    winrt::uninit_apartment();
    return 0;
}

// ================================================================== audio level (Core Audio peak meter)
static const GUID kClsidMMDE = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
static const GUID kIidMMDE = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
static const GUID kIidASM2 = {0x77AA99A0, 0x1BD6, 0x484F, {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};
static const GUID kIidASC2 = {0xBFB7FF88, 0x7239, 0x4FC9, {0x8F, 0xA2, 0x07, 0xC9, 0x50, 0xBE, 0x9C, 0x6D}};
static const GUID kIidMeter = {0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

// declared here so we don't depend on how complete the toolchain's endpointvolume.h is
struct IPeakMeter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count, float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};

struct AudioMeter {
    IMMDeviceEnumerator* en = nullptr;
    IPeakMeter* endpoint = nullptr;
    std::vector<IPeakMeter*> sessions;
    ULONGLONG next = 0;

    void ReleaseMeters() {
        for (auto* m : sessions) m->Release();
        sessions.clear();
        if (endpoint) { endpoint->Release(); endpoint = nullptr; }
    }
    void Release() {
        ReleaseMeters();
        if (en) { en->Release(); en = nullptr; }
    }
    void Refresh(bool spotifyOnly) {
        ReleaseMeters();
        if (!en && FAILED(CoCreateInstance(kClsidMMDE, nullptr, CLSCTX_ALL, kIidMMDE, (void**)&en))) { en = nullptr; return; }
        IMMDevice* dev = nullptr;
        if (FAILED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) || !dev) return;
        if (!spotifyOnly) {
            if (FAILED(dev->Activate(kIidMeter, CLSCTX_ALL, nullptr, (void**)&endpoint))) endpoint = nullptr;
        } else {
            IAudioSessionManager2* mgr = nullptr;
            if (SUCCEEDED(dev->Activate(kIidASM2, CLSCTX_ALL, nullptr, (void**)&mgr)) && mgr) {
                IAudioSessionEnumerator* se = nullptr;
                if (SUCCEEDED(mgr->GetSessionEnumerator(&se)) && se) {
                    int n = 0;
                    se->GetCount(&n);
                    for (int i = 0; i < n; i++) {
                        IAudioSessionControl* c = nullptr;
                        if (FAILED(se->GetSession(i, &c)) || !c) continue;
                        IAudioSessionControl2* c2 = nullptr;
                        DWORD pid = 0;
                        if (SUCCEEDED(c->QueryInterface(kIidASC2, (void**)&c2)) && c2) {
                            c2->GetProcessId(&pid);
                            c2->Release();
                        }
                        if (pid && IsSpotifyPid(pid)) {
                            IPeakMeter* mi = nullptr;
                            if (SUCCEEDED(c->QueryInterface(kIidMeter, (void**)&mi)) && mi) sessions.push_back(mi);
                        }
                        c->Release();
                    }
                    se->Release();
                }
                mgr->Release();
            }
        }
        dev->Release();
    }
    // returns 0..1, or -1 when no meter is available
    float Peak(bool spotifyOnly, ULONGLONG now) {
        if (now >= next) { Refresh(spotifyOnly); next = now + 3000; }
        float p = 0;
        if (spotifyOnly) {
            if (sessions.empty()) return -1;
            for (auto* m : sessions) { float v = 0; if (SUCCEEDED(m->GetPeakValue(&v))) p = max(p, v); }
            return p;
        }
        if (!endpoint) return -1;
        if (FAILED(endpoint->GetPeakValue(&p))) return -1;
        return p;
    }
};

// ================================================================== UI state
static HWND g_hwnd = nullptr;
static HANDLE g_uiThread = nullptr, g_mediaThread = nullptr, g_mutex = nullptr;
static const UINT WM_APP_SETTINGS = WM_APP + 1;

static int g_cx = 0, g_gapPx = 14;
static bool g_attachNow = true;  // runtime attach state (setting, middle-click or drag)
static bool g_dockBottom = false;  // which edge the island docks to
static float g_pillY = 0, g_dragY0 = 0;  // pill top in screen pixels
static float g_vol = 0.5f, g_volX0 = 0, g_volX1 = 1, g_seekX0 = 0, g_seekW = 1, g_seekFrac = 0;
static bool g_volDrag = false, g_seekDrag = false;
static ULONGLONG g_seekHold = 0;
static double g_lastRenderT = 0, g_lastEdgeT = 0;
static float g_sweep = 0, g_beat = 0, g_peakAvg = 0;
static ULONGLONG g_lastBeat = 0;
static double g_nextFakeBeat = 0;
static Spring sJump[2];                      // glow hot-spot positions (0..1 of screen height)
static float g_jumpTarget[2] = {0.3f, 0.7f};
static RECT g_mon = {0, 0, 1920, 1080};
static UINT g_dpi = 96;
static float g_lastS = 1.f;

static bool g_expanded = false, g_move = false, g_dirty = true, g_inside = false, g_clickThrough = true;
static bool g_fullscreen = false, g_firstSnap = true, g_init = false;
static double g_time = 0, g_enterT = 0, g_leaveT = 0, g_marqueeT0 = 0;
static LONGLONG g_lastQpc = 0, g_qpf = 1;
static unsigned g_frame = 0, g_seenSnap = ~0u, g_seenArt = 0;

static int g_state = 0;
static ULONGLONG g_stateHold = 0;
static std::wstring g_title, g_artist, g_appId;
static double g_pos = 0, g_dur = 0;
static ULONGLONG g_posTick = 0;

static Spring sW, sH, sF, sShow, sDetach, sArtFade, sTextIn, sPP, sGlow, sBtn[3], sBar[4];
static float g_barT[4] = {.3f, .5f, .4f, .6f};
static double g_barNext[4] = {};
static float g_lvl = 0;
static float g_col[3] = {48, 209, 88};

static Bitmap *g_art = nullptr, *g_artPrev = nullptr;
static bool g_hasArtColor = false;
static float g_artCol[3] = {255, 255, 255};

static RectF g_btnR[3], g_artR, g_volR, g_seekR;
static int g_hoverBtn = -1;

static bool g_drag = false;
static POINT g_dragStart;
static int g_dragCx = 0;

static AudioMeter g_meter;

static FontFamily* g_fam = nullptr;
static Font *g_fSmall = nullptr, *g_fTitle = nullptr, *g_fArtist = nullptr, *g_fTime = nullptr;
static float g_fontS = 0;

static HDC g_memDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static void* g_bits = nullptr;
static int g_dibW = 0, g_dibH = 0;

// ================================================================== system volume (endpoint volume)
static const GUID kIidEPV = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

// only the methods up to GetMasterVolumeLevelScalar; the vtable order matches IAudioEndpointVolume
struct IEndpointVol : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE RegisterControlChangeNotify(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterControlChangeNotify(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMasterVolumeLevel(float, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMasterVolumeLevelScalar(float, const GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMasterVolumeLevel(float*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMasterVolumeLevelScalar(float*) = 0;
};
static IEndpointVol* g_epv = nullptr;
static ULONGLONG g_epvNext = 0;

static IEndpointVol* GetEpv() {
    if (g_epv) return g_epv;
    IMMDeviceEnumerator* en = nullptr;
    if (FAILED(CoCreateInstance(kClsidMMDE, nullptr, CLSCTX_ALL, kIidMMDE, (void**)&en)) || !en) return nullptr;
    IMMDevice* dev = nullptr;
    if (SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) && dev) {
        if (FAILED(dev->Activate(kIidEPV, CLSCTX_ALL, nullptr, (void**)&g_epv))) g_epv = nullptr;
        dev->Release();
    }
    en->Release();
    return g_epv;
}

static float ReadVolume(ULONGLONG now) {
    if (now >= g_epvNext) {  // re-resolve the default device now and then
        g_epvNext = now + 5000;
        if (g_epv) { g_epv->Release(); g_epv = nullptr; }
    }
    IEndpointVol* e = GetEpv();
    float v = -1.f;
    if (!e || FAILED(e->GetMasterVolumeLevelScalar(&v))) return -1.f;
    return v;
}

static void SetVolFromX(float x) {
    float v = Clamp01((x - g_volX0) / max(1.f, g_volX1 - g_volX0));
    g_vol = v;
    if (IEndpointVol* e = GetEpv()) e->SetMasterVolumeLevelScalar(v, nullptr);
    g_dirty = true;
}

// ================================================================== color helpers
static void RgbToHsv(float r, float g, float b, float& h, float& s, float& v) {
    float mx = max(r, max(g, b)), mn = min(r, min(g, b)), d = mx - mn;
    v = mx;
    s = mx > 0 ? d / mx : 0;
    if (d <= 1e-6f) h = 0;
    else if (mx == r) h = fmodf((g - b) / d, 6.f);
    else if (mx == g) h = (b - r) / d + 2.f;
    else h = (r - g) / d + 4.f;
    h *= 60.f;
    if (h < 0) h += 360.f;
}

static void HsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    float c = v * s, x = c * (1 - fabsf(fmodf(h / 60.f, 2.f) - 1)), m = v - c;
    float rr = 0, gg = 0, bb = 0;
    if (h < 60) { rr = c; gg = x; }
    else if (h < 120) { rr = x; gg = c; }
    else if (h < 180) { gg = c; bb = x; }
    else if (h < 240) { gg = x; bb = c; }
    else if (h < 300) { rr = x; bb = c; }
    else { rr = c; bb = x; }
    r = rr + m; g = gg + m; b = bb + m;
}

// most "characteristic" vivid color of the art, boosted so it glows nicely
static bool DominantColor(Bitmap* src, float out[3]) {
    const int N = 32;
    Bitmap sm(N, N, PixelFormat32bppARGB);
    {
        Graphics g(&sm);
        g.SetInterpolationMode(InterpolationModeHighQualityBilinear);
        g.DrawImage(src, 0, 0, N, N);
    }
    BitmapData bd;
    Gdiplus::Rect rc(0, 0, N, N);
    if (sm.LockBits(&rc, ImageLockModeRead, PixelFormat32bppARGB, &bd) != Ok) return false;
    float Wt[12] = {}, R[12] = {}, G[12] = {}, Bc[12] = {};
    for (int y = 0; y < N; y++) {
        const uint32_t* row = (const uint32_t*)((const BYTE*)bd.Scan0 + y * bd.Stride);
        for (int x = 0; x < N; x++) {
            uint32_t p = row[x];
            float r = ((p >> 16) & 255) / 255.f, gg = ((p >> 8) & 255) / 255.f, b = (p & 255) / 255.f;
            float h, s, v;
            RgbToHsv(r, gg, b, h, s, v);
            if (v < 0.15f || s < 0.12f) continue;
            float w = s * s * (0.4f + v);
            int k = ((int)(h / 30.f)) % 12;
            Wt[k] += w; R[k] += r * w; G[k] += gg * w; Bc[k] += b * w;
        }
    }
    sm.UnlockBits(&bd);
    int best = -1;
    float bw = 0;
    for (int k = 0; k < 12; k++) {
        float sc = Wt[k] + 0.5f * (Wt[(k + 11) % 12] + Wt[(k + 1) % 12]);
        if (sc > bw) { bw = sc; best = k; }
    }
    if (best < 0 || Wt[best] < 4.f) return false;  // basically grayscale art
    float r = R[best] / Wt[best], gg = G[best] / Wt[best], b = Bc[best] / Wt[best];
    float h, s, v;
    RgbToHsv(r, gg, b, h, s, v);
    s = std::clamp(s * 1.15f, 0.45f, 1.f);
    v = max(v, 0.92f);
    HsvToRgb(h, s, v, r, gg, b);
    out[0] = r * 255; out[1] = gg * 255; out[2] = b * 255;
    return true;
}

static Bitmap* MakeSquare(Bitmap* src, int n) {
    int w = (int)src->GetWidth(), h = (int)src->GetHeight();
    int side = min(w, h), sx = (w - side) / 2, sy = (h - side) / 2;
    auto* out = new Bitmap(n, n, PixelFormat32bppPARGB);
    Graphics g(out);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    ImageAttributes ia;
    ia.SetWrapMode(WrapModeTileFlipXY);
    g.DrawImage(src, Gdiplus::Rect(0, 0, n, n), sx, sy, side, side, UnitPixel, &ia);
    return out;
}

static void ApplyArt(const std::vector<uint8_t>& bytes) {
    delete g_artPrev;
    g_artPrev = g_art;
    g_art = nullptr;
    g_hasArtColor = false;
    sArtFade.x = 0; sArtFade.v = 0;
    if (bytes.empty()) return;
    IStream* st = SHCreateMemStream(bytes.data(), (UINT)bytes.size());
    if (!st) return;
    Bitmap* raw = Bitmap::FromStream(st);
    if (raw && raw->GetLastStatus() == Ok && raw->GetWidth() > 0 && raw->GetHeight() > 0) {
        g_art = MakeSquare(raw, 256);  // own copy, so the stream can go away
        g_hasArtColor = DominantColor(g_art, g_artCol);
    }
    delete raw;
    st->Release();
}

// ================================================================== drawing helpers
static void AddRoundRect(GraphicsPath& p, float x, float y, float w, float h, float r) {
    r = min(r, min(w, h) / 2.f);
    if (r < 0.5f) { p.AddRectangle(RectF(x, y, w, h)); return; }
    float d = r * 2;
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}

// rounded rect with separate top (rt) and bottom (rb) corner radii
static void AddRoundRect2(GraphicsPath& p, float x, float y, float w, float h, float rt, float rb) {
    rt = min(rt, min(w, h) / 2.f);
    rb = min(rb, min(w, h) / 2.f);
    if (rt < 0.5f) {
        p.AddLine(x, y, x + w, y);
    } else {
        p.AddArc(x, y, 2 * rt, 2 * rt, 180, 90);
        p.AddArc(x + w - 2 * rt, y, 2 * rt, 2 * rt, 270, 90);
    }
    p.AddArc(x + w - 2 * rb, y + h - 2 * rb, 2 * rb, 2 * rb, 0, 90);
    p.AddArc(x, y + h - 2 * rb, 2 * rb, 2 * rb, 90, 90);
    p.CloseFigure();
}

// Island body. Attached: flat top that "melts" into the screen edge through concave elliptical
// shoulders (fx = horizontal melt length, fy = height). Detached: rounded capsule whose top
// corners have radius rt.
static void BuildShape(GraphicsPath& p, float x, float y, float w, float h, float rb, float rt, float fx,
                       float fy) {
    rb = min(rb, min(w, h) / 2.f);
    rt = min(rt, rb);
    fy = max(0.f, min(fy, h - rb - 0.5f));
    if (fx > 0.5f && fy > 0.5f) {
        p.AddLine(x - fx, y, x + w + fx, y);
        p.AddArc(x + w, y, 2 * fx, 2 * fy, 270, -90);                      // right shoulder (concave)
        p.AddArc(x + w - 2 * rb, y + h - 2 * rb, 2 * rb, 2 * rb, 0, 90);   // bottom right
        p.AddArc(x, y + h - 2 * rb, 2 * rb, 2 * rb, 90, 90);               // bottom left
        p.AddArc(x - 2 * fx, y, 2 * fx, 2 * fy, 0, -90);                   // left shoulder (concave)
        p.CloseFigure();
        return;
    }
    AddRoundRect2(p, x, y, w, h, rt, rb);
}

static void GlowEllipse(Graphics& g, float cx, float cy, float rx, float ry, Color col) {
    if (rx < 1 || ry < 1 || col.GetA() == 0) return;
    GraphicsPath p;
    p.AddEllipse(cx - rx, cy - ry, rx * 2, ry * 2);
    PathGradientBrush b(&p);
    b.SetCenterPoint(PointF(cx, cy));
    b.SetCenterColor(col);
    Color edge(0, col.GetR(), col.GetG(), col.GetB());
    INT n = 1;
    b.SetSurroundColors(&edge, &n);
    REAL fac[4] = {0.f, 0.16f, 0.55f, 1.f}, pos[4] = {0.f, 0.42f, 0.78f, 1.f};
    b.SetBlend(fac, pos, 4);
    g.FillPath(&b, &p);
}

static void EnsureFonts(float S) {
    if (g_fSmall && fabsf(g_fontS - S) < 0.001f) return;
    delete g_fSmall; delete g_fTitle; delete g_fArtist; delete g_fTime;
    if (!g_fam) {
        g_fam = new FontFamily(L"Segoe UI Variable Display");
        if (!g_fam->IsAvailable()) { delete g_fam; g_fam = new FontFamily(L"Segoe UI"); }
    }
    g_fSmall = new Font(g_fam, 14.0f * S, FontStyleBold, UnitPixel);
    g_fTitle = new Font(g_fam, 18.5f * S, FontStyleBold, UnitPixel);
    g_fArtist = new Font(g_fam, 15.0f * S, FontStyleRegular, UnitPixel);
    g_fTime = new Font(g_fam, 12.0f * S, FontStyleRegular, UnitPixel);
    g_fontS = S;
}

static void DrawLabel(Graphics& g, const std::wstring& s, Font* f, Color c, float x, float y, float w, float h,
                      StringAlignment al = StringAlignmentNear) {
    if (w < 4 || c.GetA() == 0 || s.empty()) return;
    StringFormat sf;
    sf.SetTrimming(StringTrimmingEllipsisCharacter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetAlignment(al);
    SolidBrush b(c);
    g.DrawString(s.c_str(), -1, f, RectF(x, y, w, h), &sf, &b);
}

// scrolling text with soft faded edges for long titles
static void Marquee(Graphics& g, int slot, const std::wstring& s, Font* f, Color c, float x, float y, float w,
                    float h, float S) {
    if (s.empty() || w < 8 || c.GetA() == 0) return;
    struct M { std::wstring s; float sz = 0, w = 0, h = 0; };
    static M cache[2];
    M& m = cache[slot];
    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
    if (m.s != s || m.sz != f->GetSize()) {
        RectF bb;
        g.MeasureString(s.c_str(), -1, f, PointF(0, 0), &sf, &bb);
        m.s = s; m.sz = f->GetSize(); m.w = bb.Width; m.h = bb.Height;
    }
    float ty = y + (h - m.h) / 2.f;
    bool scroll = m.w > w;
    float off = 0, gap = 48 * S;
    if (scroll) {
        float cycle = m.w + gap, speed = 38 * S, pause = 2.2f;
        float period = pause + cycle / speed;
        float t = fmodf((float)(g_time - g_marqueeT0), period);
        off = t < pause ? 0 : (t - pause) * speed;
    }
    LinearGradientBrush lb(PointF(x - 1, 0), PointF(x + w + 1, 0), c, c);
    if (scroll) {
        float fe = min(16 * S, w / 4) / (w + 2);
        Color t0(0, c.GetR(), c.GetG(), c.GetB());
        Color cols[4] = {off > 0.5f ? t0 : c, c, c, t0};
        REAL pos[4] = {0.f, fe, 1.f - fe, 1.f};
        lb.SetInterpolationColors(cols, pos, 4);
    }
    GraphicsState st = g.Save();
    g.IntersectClip(RectF(x, y, w, h));
    g.DrawString(s.c_str(), -1, f, PointF(x - off, ty), &sf, &lb);
    if (scroll) g.DrawString(s.c_str(), -1, f, PointF(x - off + m.w + gap, ty), &sf, &lb);
    g.Restore(st);
}

static void Tile(Graphics& g, float x, float y, float s, float r, BYTE a, BYTE cr, BYTE cg, BYTE cb) {
    if (s < 1 || a == 0) return;
    GraphicsPath p;
    AddRoundRect(p, x, y, s, s, r);
    LinearGradientBrush br(PointF(x, y), PointF(x + s, y + s), Color(a, cr, cg, cb),
                           Color(a, cr * 45 / 100, cg * 45 / 100, cb * 45 / 100));
    g.FillPath(&br, &p);
    SolidBrush w(Color(a, 255, 255, 255));
    float u = s / 10.f;
    g.FillEllipse(&w, x + 2.6f * u, y + 6.0f * u, 2.6f * u, 2.0f * u);
    g.FillRectangle(&w, x + 4.9f * u, y + 2.2f * u, 0.5f * u, 5.0f * u);
    PointF fl[3] = {PointF(x + 5.4f * u, y + 2.2f * u), PointF(x + 7.6f * u, y + 4.2f * u),
                    PointF(x + 5.4f * u, y + 4.6f * u)};
    g.FillPolygon(&w, fl, 3);
}

static void FillArt(Graphics& g, Bitmap* art, float x, float y, float s, float r, float alpha) {
    if (!art || s < 1 || alpha <= 0.004f) return;
    GraphicsPath p;
    AddRoundRect(p, x, y, s, s, r);
    float n = (float)art->GetWidth();
    Matrix m(s / n, 0, 0, s / n, x, y);
    if (alpha >= 0.996f) {
        TextureBrush tb(art, WrapModeClamp);
        tb.SetTransform(&m);
        g.FillPath(&tb, &p);
    } else {
        ImageAttributes ia;
        ColorMatrix cm = {{{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, alpha, 0}, {0, 0, 0, 0, 1}}};
        ia.SetColorMatrix(&cm);
        TextureBrush tb(art, RectF(0, 0, n, n), &ia);
        tb.SetWrapMode(WrapModeClamp);
        tb.SetTransform(&m);
        g.FillPath(&tb, &p);
    }
}

static void DrawArt(Graphics& g, float x, float y, float s, float r, float e, BYTE cr, BYTE cg, BYTE cb) {
    float fade = Clamp01(sArtFade.x);
    if (e > 0.05f && g_art) GlowEllipse(g, x + s / 2, y + s * 0.58f, s * 0.78f, s * 0.7f, Color(A8(0.42f * e * fade), cr, cg, cb));
    if (g_artPrev && fade < 1) FillArt(g, g_artPrev, x, y, s, r, 1 - fade);
    float pop = 0.88f + 0.12f * fade, ps = s * pop, off = (s - ps) / 2;
    if (g_art) FillArt(g, g_art, x + off, y + off, ps, r * pop, fade);
    else Tile(g, x + off, y + off, ps, r * pop, A8(fade), cr, cg, cb);
}

static void Eq(Graphics& g, float x, float cy, float maxH, float S, float a, BYTE cr, BYTE cg, BYTE cb) {
    if (g_cfg.ind == 2 || a < 0.01f) return;
    if (g_cfg.ind == 1) {
        float rad = 5.5f * S * (g_state == 2 ? 0.75f + 0.5f * g_lvl : 0.7f);
        GlowEllipse(g, x + 9 * S, cy, rad * 2.4f, rad * 2.4f, Color(A8(a * 0.5f), cr, cg, cb));
        SolidBrush b(Color(A8(a), cr, cg, cb));
        g.FillEllipse(&b, x + 9 * S - rad, cy - rad, rad * 2, rad * 2);
        return;
    }
    float bw = 3.2f * S, gap = 2.4f * S;
    SolidBrush b(Color(A8(a), cr, cg, cb));
    for (int i = 0; i < 4; i++) {
        float h = max(bw, maxH * Clamp01(sBar[i].x));
        GraphicsPath p;
        AddRoundRect(p, x + i * (bw + gap), cy - h / 2, bw, h, bw / 2);
        g.FillPath(&b, &p);
    }
}

static void Tri(Graphics& g, Brush& b, float cx, float cy, float s, bool right) {
    float d = right ? 1.f : -1.f;
    PointF p[3] = {PointF(cx - d * s * 0.5f, cy - s * 0.62f), PointF(cx - d * s * 0.5f, cy + s * 0.62f),
                   PointF(cx + d * s * 0.6f, cy)};
    g.FillPolygon(&b, p, 3);
}

static void FillRound(Graphics& g, float x, float y, float w, float h, Color c) {
    if (w < 0.5f || c.GetA() == 0) return;
    GraphicsPath p;
    AddRoundRect(p, x, y, w, h, h / 2);
    SolidBrush b(c);
    g.FillPath(&b, &p);
}

static std::wstring Fmt(double s) {
    if (!(s > 0)) s = 0;
    int t = (int)s;
    return std::to_wstring(t / 60) + (t % 60 < 10 ? L":0" : L":") + std::to_wstring(t % 60);
}

// ================================================================== backbuffer
static bool EnsureDib(int w, int h) {
    if (g_dib && w == g_dibW && h == g_dibH) return true;
    if (!g_memDC) {
        HDC s = GetDC(nullptr);
        g_memDC = CreateCompatibleDC(s);
        ReleaseDC(nullptr, s);
    }
    if (g_dib) { SelectObject(g_memDC, g_oldBmp); DeleteObject(g_dib); g_dib = nullptr; }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_dib = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, &g_bits, nullptr, 0);
    if (!g_dib) return false;
    g_oldBmp = SelectObject(g_memDC, g_dib);
    g_dibW = w; g_dibH = h;
    return true;
}

static void FreeDib() {
    if (g_dib) { SelectObject(g_memDC, g_oldBmp); DeleteObject(g_dib); g_dib = nullptr; }
    if (g_memDC) { DeleteDC(g_memDC); g_memDC = nullptr; }
}

// ================================================================== screen-side glow (two click-through windows)
struct EdgeWin {
    HWND hwnd = nullptr;
    HDC dc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ old = nullptr;
    void* bits = nullptr;
    int w = 0, h = 0;
};
static EdgeWin g_edge[2];
static Spring sEdge;
static bool g_edgeShown = false;

static void CreateEdgeWins(HINSTANCE inst, PCWSTR cls) {
    for (auto& e : g_edge)
        e.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                                 cls, L"", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, inst, nullptr);
}

static void DestroyEdgeWins() {
    for (auto& e : g_edge) {
        if (e.dib) { SelectObject(e.dc, e.old); DeleteObject(e.dib); e.dib = nullptr; }
        if (e.dc) { DeleteDC(e.dc); e.dc = nullptr; }
        if (e.hwnd) { DestroyWindow(e.hwnd); e.hwnd = nullptr; }
    }
    g_edgeShown = false;
}

static void RenderEdge(EdgeWin& e, bool right, int w, int h, float inten, float r, float g, float b,
                       float center, float sigma, float base) {
    if (!e.hwnd) return;
    if (!e.dc) {
        HDC s = GetDC(nullptr);
        e.dc = CreateCompatibleDC(s);
        ReleaseDC(nullptr, s);
    }
    if (!e.dib || e.w != w || e.h != h) {
        if (e.dib) { SelectObject(e.dc, e.old); DeleteObject(e.dib); e.dib = nullptr; }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        e.dib = CreateDIBSection(e.dc, &bi, DIB_RGB_COLORS, &e.bits, nullptr, 0);
        if (!e.dib) return;
        e.old = SelectObject(e.dc, e.dib);
        e.w = w; e.h = h;
    }
    std::vector<float> prof(w);
    for (int x = 0; x < w; x++) {
        float t = right ? (float)(w - 1 - x) / w : (float)x / w;   // 0 at the screen edge
        prof[x] = powf(1.f - t, 2.4f) * inten;
    }
    // vertical profile: a bright spot (gaussian) at `center` over a dimmer base glow
    std::vector<float> row(h);
    float fadeLen = max(1.f, h * 0.08f);
    for (int y = 0; y < h; y++) {
        float dd = ((float)y / h - center) / sigma;
        float blob = expf(-0.5f * dd * dd);
        row[y] = (base + (1.f - base) * blob) * Smooth(y / fadeLen) * Smooth((h - 1 - y) / fadeLen);
    }
    // premultiplied colour for every alpha level: the pixel loop is then just integer math + a lookup
    uint32_t lut[256];
    for (int a = 0; a < 256; a++) {
        float af = a / 255.f;
        lut[a] = ((uint32_t)a << 24) | ((uint32_t)(r * af + 0.5f) << 16) | ((uint32_t)(g * af + 0.5f) << 8) |
                 (uint32_t)(b * af + 0.5f);
    }
    std::vector<int> profI(w);
    for (int x = 0; x < w; x++) profI[x] = (int)(Clamp01(prof[x]) * 256.f);
    uint32_t* px = (uint32_t*)e.bits;
    for (int y = 0; y < h; y++) {
        int rv = (int)(Clamp01(row[y]) * 255.f);
        uint32_t* line = px + (size_t)y * w;
        if (rv == 0) { ZeroMemory(line, (size_t)w * 4); continue; }
        for (int x = 0; x < w; x++) line[x] = lut[(profI[x] * rv) >> 8];
    }
    POINT pos = {right ? g_mon.right - w : g_mon.left, g_mon.top}, src = {0, 0};
    SIZE sz = {w, h};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    HDC scr = GetDC(nullptr);
    UpdateLayeredWindow(e.hwnd, scr, &pos, &sz, e.dc, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, scr);
}

static void UpdateEdges(float inten, bool live) {
    if (inten < 0.004f) {
        if (g_edgeShown) {
            for (auto& e : g_edge) if (e.hwnd) ShowWindow(e.hwnd, SW_HIDE);
            g_edgeShown = false;
        }
        return;
    }
    bool just = !g_edgeShown;
    if (just) {
        for (auto& e : g_edge) if (e.hwnd) ShowWindow(e.hwnd, SW_SHOWNOACTIVATE);
        g_edgeShown = true;
    }
    if (!just) {
        if (g_time - g_lastEdgeT < 1.0 / min(30, g_cfg.fps)) return;
        if (!live && g_frame % 6 != 0) return;
    }
    g_lastEdgeT = g_time;
    int w = max(20, (int)(g_cfg.edgeW * g_dpi / 96.f));
    int h = g_mon.bottom - g_mon.top;
    // up/down motion: phase advances with the music (faster when loud, surges on every beat)
    bool jump = g_cfg.edgeMode == 3;
    float sL = jump ? sJump[0].x : 0.5f + 0.38f * sinf(g_sweep);
    float sR = jump ? sJump[1].x : (g_cfg.edgeMode == 0 ? 0.5f - 0.38f * sinf(g_sweep) : sL);
    float sigma = jump ? 0.12f + 0.07f * g_beat : 0.17f + 0.06f * g_beat;
    float base = g_cfg.edgeMode == 2 ? 1.f : (jump ? 0.2f : 0.3f);
    float in = min(1.f, inten);
    RenderEdge(g_edge[0], false, w, h, in, g_col[0], g_col[1], g_col[2], sL, sigma, base);
    RenderEdge(g_edge[1], true, w, h, in, g_col[0], g_col[1], g_col[2], sR, sigma, base);
}

// ================================================================== render
static void Render(float S, int W, int H, int wx, int wy, float topRoom, ULONGLONG now) {
    if (!EnsureDib(W, H)) return;
    EnsureFonts(S);
    const Settings& c = g_cfg;
    BYTE cr = (BYTE)g_col[0], cg = (BYTE)g_col[1], cb = (BYTE)g_col[2];
    float d = Clamp01(sDetach.x);  // 0 = attached to the top edge, 1 = detached

    {
        Bitmap bmp(W, H, W * 4, PixelFormat32bppPARGB, (BYTE*)g_bits);
        Graphics g(&bmp);
        g.Clear(Color(0, 0, 0, 0));
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        g.SetInterpolationMode(InterpolationModeHighQualityBilinear);

        float pw = sW.x, ph0 = sH.x;
        float sq = std::clamp(sW.v / (2200.f * S), -0.07f, 0.07f);  // liquid squash while stretching
        float ph = ph0 * (1.f - sq * 0.5f);
        float px = (W - pw) / 2.f, py = topRoom;
        float ex = Clamp01((ph0 - Lay::COL_H * S) / ((Lay::EXP_H - Lay::COL_H) * S));
        float e = Smooth(ex);
        float rb = min(ph / 2.f, Lerp(Lay::COL_H / 2.f, 36.f, e) * S);
        float f = max(0.f, sF.x);                          // horizontal melt length
        float fy = min(f, Lerp(13.f, 22.f, e) * S);        // vertical shoulder height
        float rt = rb * d;  // top corner radius grows as the pill detaches
        float gI = Clamp01(sGlow.x);

        // bottom dock: mirror the body / glow / shadow vertically around the pill's centre
        bool flip = g_dockBottom && d < 0.5f;
        Matrix fm(1.f, 0.f, 0.f, -1.f, 0.f, 2.f * (py + ph / 2.f));
        if (flip) g.SetTransform(&fm);

        // ---- side glow in the album color
        if (gI > 0.01f) {
            for (int side = 0; side < 2; side++) {
                float gx = side ? px + pw - 8 * S : px + 8 * S;
                float rx = Lerp(54.f, 96.f, e) * S * (0.85f + 0.35f * g_lvl);
                float ry = ph * 0.5f + Lerp(20.f, 34.f, e) * S;
                GlowEllipse(g, gx, py + ph * 0.5f, rx, ry, Color(A8(gI * 0.9f), cr, cg, cb));
            }
            GlowEllipse(g, px + pw / 2, py + ph - 4 * S, pw * 0.42f, Lerp(16.f, 30.f, e) * S,
                        Color(A8(gI * 0.32f), cr, cg, cb));
        }

        // ---- shadow
        for (int i = 0; i < 5; i++) {
            float ee = (i + 1) * 2.2f * S;
            GraphicsPath sp;
            BuildShape(sp, px - ee * 0.5f, py + d * (3 * S - ee * 0.3f), pw + ee, ph + ee * (1.f - 0.1f * d),
                       rb + ee * 0.5f, rt + ee * 0.5f * d, f, fy);
            SolidBrush sb(Color(10, 0, 0, 0));
            g.FillPath(&sb, &sp);
        }

        // ---- body
        GraphicsPath body;
        BuildShape(body, px, py, pw, ph, rb, rt, f, fy);
        LinearGradientBrush bb(PointF(0, py - 1), PointF(0, py + ph + 1),
                               Color(255, (BYTE)(22 * d), (BYTE)(22 * d), (BYTE)(25 * d)),
                               Color(255, (BYTE)(14 * (1 - d)), (BYTE)(14 * (1 - d)), (BYTE)(16 * (1 - d))));
        g.FillPath(&bb, &body);
        g.ResetTransform();
        GraphicsPath bodyDev;  // body in device space (mirrored when docked at the bottom)
        bodyDev.AddPath(&body, FALSE);
        if (flip) bodyDev.Transform(&fm);
        g.SetClip(&bodyDev);
        if (gI > 0.01f) {
            if (flip) g.SetTransform(&fm);
            GlowEllipse(g, px + pw / 2, py + ph + 10 * S, pw * 0.55f, Lerp(18.f, 64.f, e) * S,
                        Color(A8(gI * 0.22f), cr, cg, cb));
            g.ResetTransform();
        }

        // ---- album art: morphs from the small collapsed tile to the big expanded one
        float rowH = Lay::COL_H * S;
        float cs = 26 * S, es = 76 * S;
        float as = Lerp(cs, es, e);
        float ax = px + Lerp(7 * S, 20 * S, e);
        float ay = py + Lerp((rowH - cs) / 2, 20 * S, e);
        DrawArt(g, ax, ay, as, Lerp(8 * S, 18 * S, e), e, cr, cg, cb);

        float ti = Clamp01(sTextIn.x);

        // ---- collapsed content
        float ca = Clamp01(1.f - ex * 2.4f);
        if (ca > 0.01f) {
            std::wstring t;
            if (g_move) t = std::wstring(L"Drag to move  ·  right-click to lock");
            else if (g_state == 0) t = L"Nothing playing";
            else t = g_title.empty() ? std::wstring(L"Spotify") : g_title;
            float tx = px + 42 * S, tw = pw - 42 * S - (g_move ? 16 : 50) * S;
            DrawLabel(g, t, g_fSmall, Color(A8(ca * ti), 255, 255, 255), tx, py + (1 - ti) * 5 * S, tw, rowH);
            if (!g_move && g_state != 0) Eq(g, px + pw - 40 * S, py + rowH / 2, 17 * S, S, ca, cr, cg, cb);
        }

        // ---- expanded content (staggered fade + slide-up)
        for (auto& r : g_btnR) r = RectF();
        g_artR = RectF();
        g_volR = RectF();
        g_seekR = RectF();
        if (ex > 0.3f) {
            float a1 = Stag(ex, 0.40f), a2 = Stag(ex, 0.50f), a3 = Stag(ex, 0.58f), a4 = Stag(ex, 0.66f);
            float tx = px + 112 * S, tw = pw - 112 * S - 68 * S;
            Marquee(g, 0, g_title.empty() ? std::wstring(L"Spotify") : g_title, g_fTitle,
                    Color(A8(a1 * ti), 255, 255, 255), tx, py + 24 * S + (1 - a1) * 10 * S + (1 - ti) * 6 * S, tw,
                    30 * S, S);
            Marquee(g, 1, g_artist, g_fArtist, Color(A8(a2 * ti), 162, 162, 170), tx,
                    py + 54 * S + (1 - a2) * 10 * S + (1 - ti) * 6 * S, tw, 24 * S, S);
            if (g_state != 0) Eq(g, px + pw - 54 * S, py + 42 * S, 22 * S, S, a1, cr, cg, cb);

            // progress
            if (g_dur > 1 && a3 > 0.01f) {
                double pos = g_pos;
                if (g_state == 2) pos += (now - g_posTick) / 1000.0;
                pos = std::clamp(pos, 0.0, g_dur);
                float frac = (float)(pos / g_dur);
                if (g_seekDrag) { frac = g_seekFrac; pos = frac * g_dur; }
                float yy = py + 117 * S + (1 - a3) * 8 * S;
                float bx = px + 74 * S, bw = pw - 148 * S, bh = 5 * S;
                g_seekX0 = bx; g_seekW = bw;
                if (a3 > 0.5f) g_seekR = RectF(bx - 8 * S, yy - 12 * S, bw + 16 * S, 24 * S);
                FillRound(g, bx, yy - bh / 2, bw, bh, Color(A8(a3 * 0.16f), 255, 255, 255));
                FillRound(g, bx, yy - bh / 2, max(bh, bw * frac), bh, Color(A8(a3), cr, cg, cb));
                DrawLabel(g, Fmt(pos), g_fTime, Color(A8(a3 * 0.75f), 255, 255, 255), px + 18 * S, yy - 9 * S,
                          50 * S, 18 * S);
                DrawLabel(g, L"-" + Fmt(g_dur - pos), g_fTime, Color(A8(a3 * 0.75f), 255, 255, 255),
                          px + pw - 68 * S, yy - 9 * S, 50 * S, 18 * S, StringAlignmentFar);
            }

            // controls
            if (a4 > 0.01f) {
                float cy = py + 156 * S + (1 - a4) * 8 * S, cxm = px + pw / 2, gap = 74 * S;
                for (int i = 0; i < 3; i++) {
                    float bx = cxm + (i - 1) * gap;
                    float k = 1.f - 0.22f * Clamp01(sBtn[i].x);
                    float s = 22 * S * k;
                    if (g_hoverBtn == i) {
                        SolidBrush hb(Color(A8(a4 * 0.12f), 255, 255, 255));
                        float hr = 25 * S * k;
                        g.FillEllipse(&hb, bx - hr, cy - hr, hr * 2, hr * 2);
                    }
                    SolidBrush wb(Color(A8(a4), 255, 255, 255));
                    if (i == 0) {
                        Tri(g, wb, bx - s * 0.45f, cy, s * 0.95f, false);
                        Tri(g, wb, bx + s * 0.45f, cy, s * 0.95f, false);
                    } else if (i == 2) {
                        Tri(g, wb, bx - s * 0.45f, cy, s * 0.95f, true);
                        Tri(g, wb, bx + s * 0.45f, cy, s * 0.95f, true);
                    } else {
                        float pp = Clamp01(sPP.x);  // 1 = pause icon
                        float ks = 0.65f + 0.35f * pp, kp = 0.65f + 0.35f * (1 - pp);
                        SolidBrush pb(Color(A8(a4 * pp), 255, 255, 255));
                        float ss = s * ks;
                        g.FillRectangle(&pb, bx - ss * 0.52f, cy - ss * 0.62f, ss * 0.36f, ss * 1.24f);
                        g.FillRectangle(&pb, bx + ss * 0.16f, cy - ss * 0.62f, ss * 0.36f, ss * 1.24f);
                        SolidBrush tb(Color(A8(a4 * (1 - pp)), 255, 255, 255));
                        Tri(g, tb, bx + s * 0.06f, cy, s * 1.25f * kp, true);
                    }
                    if (a4 > 0.5f) g_btnR[i] = RectF(bx - 27 * S, cy - 27 * S, 54 * S, 54 * S);
                }

                // volume slider (system volume)
                {
                    float vx0 = px + pw - 98 * S, vx1 = px + pw - 24 * S, vh = 5 * S;
                    float vf = Clamp01(g_vol);
                    g_volX0 = vx0; g_volX1 = vx1;
                    FillRound(g, vx0, cy - vh / 2, vx1 - vx0, vh, Color(A8(a4 * 0.16f), 255, 255, 255));
                    FillRound(g, vx0, cy - vh / 2, max(vh, (vx1 - vx0) * vf), vh, Color(A8(a4), cr, cg, cb));
                    float kr = (g_volDrag ? 7.5f : 5.5f) * S, kx = vx0 + (vx1 - vx0) * vf;
                    SolidBrush kb(Color(A8(a4), 255, 255, 255));
                    g.FillEllipse(&kb, kx - kr, cy - kr, kr * 2, kr * 2);
                    float sx = vx0 - 20 * S;
                    PointF spk[6] = {PointF(sx, cy - 3 * S), PointF(sx + 4 * S, cy - 3 * S), PointF(sx + 9 * S, cy - 7 * S),
                                     PointF(sx + 9 * S, cy + 7 * S), PointF(sx + 4 * S, cy + 3 * S), PointF(sx, cy + 3 * S)};
                    SolidBrush sb2(Color(A8(a4 * 0.9f), 255, 255, 255));
                    g.FillPolygon(&sb2, spk, 6);
                    Pen wp(Color(A8(a4 * 0.9f), 255, 255, 255), 1.6f * S);
                    if (vf > 0.03f) g.DrawArc(&wp, sx + 6 * S, cy - 4.5f * S, 9 * S, 9 * S, -55, 110);
                    if (vf > 0.5f) g.DrawArc(&wp, sx + 4 * S, cy - 8 * S, 16 * S, 16 * S, -55, 110);
                    if (a4 > 0.5f) g_volR = RectF(vx0 - 8 * S, cy - 16 * S, (vx1 - vx0) + 16 * S, 32 * S);
                }
            }
            if (ex > 0.8f) g_artR = RectF(px + 16 * S, py + 16 * S, pw - 84 * S, 84 * S);
        }

        // ---- edge light
        g.ResetClip();
        if (g_move) {
            float pulse = 0.65f + 0.35f * sinf((float)g_time * 4.f);
            Pen mp(Color(A8(pulse), cr, cg, cb), 2.f * S);
            g.DrawPath(&mp, &bodyDev);
        } else {
            Color c0((BYTE)(60 * d), 255, 255, 255), c1(52, 255, 255, 255);
            LinearGradientBrush eb(PointF(0, py - 1), PointF(0, py + ph + 1), flip ? c1 : c0, flip ? c0 : c1);
            Pen ep(&eb, max(1.f, 0.9f * S));
            g.DrawPath(&ep, &bodyDev);
        }
    }
    GdiFlush();

    // slide up into the screen edge while hiding
    float show = Clamp01(sShow.x);
    int slide = (int)((1.f - show) * 26.f * S);
    if (g_dockBottom && d < 0.5f) slide = -slide;
    POINT pos = {wx, wy - slide}, src = {0, 0};
    SIZE sz = {W, H};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, A8(show), AC_SRC_ALPHA};
    HDC scr = GetDC(nullptr);
    UpdateLayeredWindow(g_hwnd, scr, &pos, &sz, g_memDC, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, scr);
}

// ================================================================== logic
static void SetClickThrough(bool on) {
    if (on == g_clickThrough) return;
    g_clickThrough = on;
    LONG_PTR ex = GetWindowLongPtrW(g_hwnd, GWL_EXSTYLE);
    ex = on ? (ex | WS_EX_TRANSPARENT) : (ex & ~(LONG_PTR)WS_EX_TRANSPARENT);
    SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, ex);
}

static void ApplyMonitor(HMONITOR m) {
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(m, &mi)) g_mon = mi.rcMonitor;
    UINT dx = 96, dy = 96;
    if (SUCCEEDED(GetDpiForMonitor(m, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx) g_dpi = dx;
}

static void UpdateMonitor() {
    POINT p = {g_cx, (int)g_pillY + 2};
    ApplyMonitor(MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST));
}

static bool IsFullscreenBusy() {
    QUERY_USER_NOTIFICATION_STATE q;
    if (FAILED(SHQueryUserNotificationState(&q))) return false;
    return q == QUNS_BUSY || q == QUNS_RUNNING_D3D_FULL_SCREEN || q == QUNS_PRESENTATION_MODE;
}

static void SavePos() {
    Wh_SetIntValue(L"cx", g_cx);
    Wh_SetIntValue(L"gap", g_gapPx);
    Wh_SetIntValue(L"attach", g_attachNow ? 1 : 0);
    Wh_SetIntValue(L"bottom", g_dockBottom ? 1 : 0);
}

static void ToggleMove() {
    g_move = !g_move;
    if (!g_move) SavePos();
    g_dirty = true;
}

static void RegisterMoveHotkey() {
    UnregisterHotKey(g_hwnd, 1);
    SHORT r = VkKeyScanW(g_cfg.key);
    if (r == -1) return;
    UINT mods = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
    if (r & 0x100) mods |= MOD_SHIFT;
    if (!RegisterHotKey(g_hwnd, 1, mods, r & 0xFF)) Wh_Log(L"Move hotkey is taken by another app; use right-click.");
}

static void OpenSource() {
    if (!g_cfg.any || Lower(g_appId).find(L"spotify") != std::wstring::npos)
        ShellExecuteW(nullptr, L"open", L"spotify:", nullptr, nullptr, SW_SHOWNORMAL);
}

static void PullMedia(ULONGLONG now) {
    unsigned v = g_snapVer.load();
    if (v != g_seenSnap) {
        g_seenSnap = v;
        Snap s;
        AcquireSRWLockShared(&g_lock);
        s = g_snap;
        ReleaseSRWLockShared(&g_lock);
        if (s.title != g_title || s.artist != g_artist) {
            g_title = s.title;
            g_artist = s.artist;
            if (!g_firstSnap && s.state != 0) {
                sTextIn.x = 0; sTextIn.v = 0;
                sW.v += 520.f * g_lastS;  // liquid "gulp" on song change
            }
            g_marqueeT0 = g_time;
            g_dirty = true;
        }
        if (now >= g_stateHold && s.state != g_state) { g_state = s.state; g_dirty = true; }
        g_appId = s.app;
        g_dur = s.dur;
        if (now >= g_seekHold) { g_pos = s.pos; g_posTick = s.posTick; }
        g_firstSnap = false;
    }
    unsigned av = g_artVer.load();
    if (av != g_seenArt) {
        g_seenArt = av;
        std::vector<uint8_t> b;
        AcquireSRWLockShared(&g_lock);
        b = g_artBytes;
        ReleaseSRWLockShared(&g_lock);
        ApplyArt(b);
        g_dirty = true;
    }
}

static void Tick() {
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    float dt = g_lastQpc ? (float)((q.QuadPart - g_lastQpc) / (double)g_qpf) : 0.016f;
    g_lastQpc = q.QuadPart;
    dt = std::clamp(dt, 0.001f, 0.05f);
    g_time += dt;
    ULONGLONG now = GetTickCount64();
    const Settings& c = g_cfg;

    PullMedia(now);

    if (g_frame % 30 == 0) UpdateMonitor();
    float S = c.scale / 100.f * g_dpi / 96.f;
    g_lastS = S;
    float topRoom = ceilf(40.f * S);  // glow room above and below the pill
    // small window while collapsed (much cheaper to draw and composite), full size while expanded
    bool compact = !g_expanded && sH.x < (Lay::COL_H + 4) * S && sW.x < (Lay::MOVE_W + 4) * S;
    int W = (int)ceilf(((compact ? Lay::MOVE_W : Lay::EXP_W) + 2 * Lay::SIDE) * S);
    int H = (int)ceilf((compact ? Lay::COL_H : Lay::EXP_H) * S + 2 * topRoom);
    int wx = g_cx - W / 2;
    // pill position: blend between the docked edge and the free position (free = anywhere on screen)
    float dd0 = Clamp01(sDetach.x);
    float Ydock = g_dockBottom ? g_mon.bottom - sH.x : (float)g_mon.top;
    float Yfree = std::clamp((float)g_mon.top + g_gapPx * g_dpi / 96.f, (float)g_mon.top,
                             max((float)g_mon.top, g_mon.bottom - sH.x));
    float pillY = Lerp(Ydock, Yfree, dd0);
    g_pillY = pillY;
    int wy = (int)floorf(pillY) - (int)topRoom;

    if (g_frame % 60 == 0) g_fullscreen = c.hideFs && IsFullscreenBusy();
    if (g_frame % 120 == 0) {
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        for (auto& e : g_edge)
            if (e.hwnd) SetWindowPos(e.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    // ---- hover (with intent delay so passing the mouse by doesn't flicker it open)
    float pw = sW.x, ph = sH.x;
    float pxs = wx + (W - pw) / 2.f, pys = pillY;
    POINT cur;
    GetCursorPos(&cur);
    bool inside = sShow.x > 0.5f && cur.x >= pxs && cur.x <= pxs + pw && cur.y >= pys && cur.y <= pys + ph;
    if (inside && !g_inside) g_enterT = g_time;
    if (!inside && g_inside) g_leaveT = g_time;
    g_inside = inside;
    bool canExpand = !g_move && g_state != 0 && !g_fullscreen;
    if (canExpand && inside && g_time - g_enterT > 0.09) {
        if (!g_expanded) g_marqueeT0 = g_time;
        g_expanded = true;
    }
    if (!inside && g_time - g_leaveT > 0.30 && !g_volDrag && !g_seekDrag) g_expanded = false;
    if (!canExpand) g_expanded = false;

    // only catch the mouse over the pill itself; glow and empty space stay click-through
    SetClickThrough(!(inside || g_drag || g_volDrag || g_seekDrag));

    float ex = Clamp01((sH.x - Lay::COL_H * S) / ((Lay::EXP_H - Lay::COL_H) * S));
    g_hoverBtn = -1;
    if (inside && ex > 0.6f)
        for (int i = 0; i < 3; i++)
            if (g_btnR[i].Contains((REAL)(cur.x - wx), (REAL)(cur.y - wy))) g_hoverBtn = i;

    if (!g_volDrag && ex > 0.3f && g_frame % 20 == 0) {  // keep the slider in sync with the system volume
        float v = ReadVolume(now);
        if (v >= 0) g_vol = v;
    }

    // ---- targets
    float tw, th;
    if (g_move) { tw = Lay::MOVE_W; th = Lay::COL_H; }
    else if (g_expanded) { tw = Lay::EXP_W; th = Lay::EXP_H; }
    else if (g_state == 0) { tw = Lay::IDLE_W; th = Lay::COL_H; }
    else { tw = Lay::COL_W; th = Lay::COL_H; }
    tw *= S; th *= S;
    // custom melt length: collapsed = setting, expanded = 1.7x (capped at 100 px)
    // detach sequence: the shoulders melt away first, then the pill drops (and the reverse when attaching)
    float tf = (g_attachNow && sDetach.x < 0.02f)
                   ? min(100.f, g_expanded ? c.melt * 1.7f : (float)c.melt) * S : 0.f;
    float tDetach = g_attachNow ? 0.f : ((g_drag || sF.x < 0.6f) ? 1.f : 0.f);
    bool hide = g_fullscreen || (c.hideIdle && g_state == 0 && !g_move);
    float tShow = hide ? 0.f : 1.f;

    if (!g_init) {  // intro: a dot that stretches out to both sides
        g_init = true;
        sW.x = th; sH.x = th; sF.x = 0; sShow.x = 0; sDetach.x = g_attachNow ? 0.f : 1.f; sJump[0].x = 0.3f; sJump[1].x = 0.7f;
        sArtFade.x = 1; sTextIn.x = 1;
        for (int i = 0; i < 3; i++) g_col[i] = (float)(i == 0 ? c.ar : i == 1 ? c.ag : c.ab);
    }

    bool growingH = th > sH.x;
    sW.Step(tw, 190, 17, dt);                                      // bouncy horizontal stretch
    sH.Step(th, growingH ? 150 : 210, growingH ? 19 : 25, dt);     // height trails a bit
    sF.Step(tf, 160, 19, dt);
    sDetach.Step(tDetach, 110, 17, dt);
    sShow.Step(tShow, 95, 19, dt);
    sArtFade.Step(1, 60, 14, dt);
    sTextIn.Step(1, 110, 17, dt);
    sPP.Step(g_state == 2 ? 1.f : 0.f, 240, 24, dt);
    for (auto& b : sBtn) b.Step(0, 260, 17, dt);

    // ---- audio level → bars + glow
    float lvlT = 0, rawP = -1.f;
    if (g_state == 2 && sShow.x > 0.01f) {
        float p = c.reactive ? g_meter.Peak(!c.any, now) : -1.f;
        rawP = p;
        lvlT = p >= 0 ? min(1.f, sqrtf(p) * 1.1f)
                      : 0.55f + 0.25f * sinf((float)g_time * 3.1f) * sinf((float)g_time * 1.7f);
    }
    // ---- beat detection: onset of the peak level over its running average
    bool beat = false;
    if (g_state == 2) {
        if (rawP >= 0) {
            g_peakAvg += (rawP - g_peakAvg) * (1.f - powf(0.97f, dt * 60.f));
            if (rawP > g_peakAvg * 1.28f + 0.03f && now - g_lastBeat > 200) beat = true;
        } else if (g_time >= g_nextFakeBeat) {  // no meter available: steady pulse
            beat = true;
            g_nextFakeBeat = g_time + 0.5;
        }
    }
    if (beat) {
        g_beat = 1.f; g_lastBeat = now;
        // jump: pick a new spot well away from the current one; the right side mirrors the left
        float cur = g_jumpTarget[0], nt = cur;
        for (int k = 0; k < 6 && fabsf(nt - cur) < 0.25f; k++) nt = 0.12f + 0.76f * Rand01();
        g_jumpTarget[0] = nt;
        g_jumpTarget[1] = 1.f - nt;
        if (c.islandBeat && !g_move) sW.v += 180.f * g_lastS;  // island hops on the beat
    }
    g_beat *= expf(-dt * 6.f);
    for (int i = 0; i < 2; i++) sJump[i].Step(g_jumpTarget[i], 420, 16, dt);  // bouncy landing
    // phase of the up/down glow motion: faster when loud, surges on every beat
    g_sweep += dt * (c.edgeSpd / 100.f) * (0.8f + 1.2f * g_lvl + 4.f * g_beat);

    float kk = lvlT > g_lvl ? 0.55f : 0.10f;
    g_lvl += (lvlT - g_lvl) * (1.f - powf(1.f - kk, dt * 60.f));

    for (int i = 0; i < 4; i++) {
        if (g_time >= g_barNext[i]) {
            g_barT[i] = 0.35f + 0.65f * Rand01();
            g_barNext[i] = g_time + 0.07 + 0.09 * Rand01();
        }
        float t = g_state == 2 ? max(0.16f, g_barT[i] * (0.25f + 0.95f * g_lvl)) : 0.16f;
        sBar[i].Step(min(t, 1.f), 600, 28, dt);
    }
    float glowT = c.glow / 100.f * (g_state == 2 ? 0.55f + 0.65f * g_lvl : g_state == 1 ? 0.3f : 0.f);
    sGlow.Step(glowT, 120, 18, dt);

    // ---- color follows the album art
    float tc[3];
    if (c.artColor && g_hasArtColor && g_art) { tc[0] = g_artCol[0]; tc[1] = g_artCol[1]; tc[2] = g_artCol[2]; }
    else { tc[0] = (float)c.ar; tc[1] = (float)c.ag; tc[2] = (float)c.ab; }
    float ca = 1.f - expf(-dt * 4.f);
    bool colAnim = false;
    for (int i = 0; i < 3; i++) {
        g_col[i] += (tc[i] - g_col[i]) * ca;
        if (fabsf(tc[i] - g_col[i]) > 0.5f) colAnim = true;
    }

    // ---- glow on the screen sides
    float edgeT = 0;
    if (c.edgeOn && !g_fullscreen && g_state != 0)
        edgeT = c.edgeStr / 100.f * (g_state == 2 ? 0.35f + 0.55f * g_lvl + (c.edgeBeat ? 0.5f * g_beat : 0.f) : 0.2f);
    sEdge.Step(edgeT, 260, 24, dt);
    UpdateEdges(sEdge.x * Clamp01(sShow.x), g_state == 2 || colAnim);

    // ---- render only when something moves
    bool springs = !sW.Settled(tw, 0.3f) || !sH.Settled(th, 0.3f) || !sF.Settled(tf, 0.3f) ||
                   !sShow.Settled(tShow, 0.002f) || sArtFade.x < 0.995f || sTextIn.x < 0.995f ||
                   !sPP.Settled(g_state == 2 ? 1.f : 0.f, 0.003f) || colAnim || fabsf(sGlow.x - glowT) > 0.003f;
    for (auto& b : sBtn) if (b.x > 0.003f) springs = true;
    bool visible = sShow.x > 0.004f || tShow > 0;
    bool live = visible && (g_state == 2 || g_move || g_expanded);
    bool due = g_time - g_lastRenderT >= 1.0 / c.fps - 0.003;  // frame-rate cap
    if (visible && due && (springs || g_dirty || live)) {
        Render(S, W, H, wx, wy, topRoom, now);
        g_dirty = false;
        g_lastRenderT = g_time;
    }
    g_frame++;
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_TIMER:
        Tick();
        return 0;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_LBUTTONDOWN: {
        REAL x = (REAL)(short)LOWORD(l), y = (REAL)(short)HIWORD(l);
        if (g_move) {
            g_drag = true;
            GetCursorPos(&g_dragStart);
            g_dragCx = g_cx; g_dragY0 = g_pillY;
            SetCapture(h);
            return 0;
        }
        if (g_volR.Contains(x, y)) {  // volume slider
            g_volDrag = true;
            SetVolFromX(x);
            SetCapture(h);
            return 0;
        }
        if (g_seekR.Contains(x, y) && g_dur > 1) {  // seek bar
            g_seekDrag = true;
            g_seekFrac = Clamp01((x - g_seekX0) / g_seekW);
            g_dirty = true;
            SetCapture(h);
            return 0;
        }
        for (int i = 0; i < 3; i++) {
            if (g_btnR[i].Contains(x, y)) {
                sBtn[i].x = 1; sBtn[i].v = 0;
                if (i == 1 && g_state != 0) {  // flip the icon right away, don't wait for Spotify
                    g_state = g_state == 2 ? 1 : 2;
                    g_stateHold = GetTickCount64() + 900;
                }
                g_cmd = i + 1;
                SetEvent(g_wake);
                return 0;
            }
        }
        if (g_artR.Contains(x, y)) OpenSource();
        return 0;
    }
    case WM_RBUTTONUP:
        ToggleMove();
        return 0;
    case WM_MBUTTONUP:  // middle-click: detach from / attach to the edge
        g_attachNow = !g_attachNow;
        if (!g_attachNow) {  // float a little way in from the edge it was docked to
            int gapSet = max(14, g_cfg.topGap);
            if (g_dockBottom)
                g_gapPx = max(0, (int)((g_mon.bottom - g_mon.top - Lay::COL_H * g_lastS) * 96.f / g_dpi) - gapSet);
            else
                g_gapPx = gapSet;
        }
        SavePos();
        g_dirty = true;
        return 0;
    case WM_HOTKEY:
        if (w == 1) ToggleMove();
        return 0;
    case WM_MOUSEMOVE: {
        float mx = (float)(short)LOWORD(l);
        if (g_volDrag) { SetVolFromX(mx); return 0; }
        if (g_seekDrag) { g_seekFrac = Clamp01((mx - g_seekX0) / g_seekW); g_dirty = true; return 0; }
        if (g_drag) {
            POINT c;
            GetCursorPos(&c);
            g_cx = g_dragCx + (int)(c.x - g_dragStart.x);
            HMONITOR mon = MonitorFromPoint(c, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = {sizeof(mi)};
            if (GetMonitorInfoW(mon, &mi)) {
                int mid = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
                if (abs(g_cx - mid) < (int)(16 * g_lastS)) g_cx = mid;  // snap to center
                ApplyMonitor(mon);
                // vertical: free placement with a magnet pulling the island to the top / bottom edge
                float Yd = g_dragY0 + (float)(c.y - g_dragStart.y);
                float th = Lay::COL_H * g_lastS;
                float dTop = Yd - mi.rcMonitor.top, dBot = mi.rcMonitor.bottom - (Yd + th);
                float catchR = g_cfg.magnet ? g_cfg.magnetR * g_dpi / 96.f : 2.f;
                float R = g_attachNow ? catchR * 2.f : catchR;  // holds on longer than it catches
                if (dTop < R && dTop <= dBot) { g_attachNow = true; g_dockBottom = false; }
                else if (dBot < R) { g_attachNow = true; g_dockBottom = true; }
                else {
                    g_attachNow = false;
                    g_gapPx = std::clamp((int)((Yd - mi.rcMonitor.top) * 96.f / g_dpi), 0, 4000);
                }
            }
            g_dirty = true;
        }
        return 0;
    }
    case WM_LBUTTONUP:
        if (g_seekDrag) {  // commit the seek
            g_seekDrag = false;
            ReleaseCapture();
            double t = (double)g_seekFrac * g_dur;
            g_seekSec = t;
            g_cmd = 4;
            SetEvent(g_wake);
            g_pos = t;
            g_posTick = GetTickCount64();
            g_seekHold = g_posTick + 1200;
            g_dirty = true;
        }
        if (g_volDrag) { g_volDrag = false; ReleaseCapture(); g_dirty = true; }
        if (g_drag) { g_drag = false; SavePos(); ReleaseCapture(); }
        return 0;
    case WM_MOUSEWHEEL: {
        int d = GET_WHEEL_DELTA_WPARAM(w);
        int n = max(1, abs(d) / WHEEL_DELTA);
        BYTE vk = d > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN;
        for (int i = 0; i < n; i++) { keybd_event(vk, 0, 0, 0); keybd_event(vk, 0, KEYEVENTF_KEYUP, 0); }
        return 0;
    }
    case WM_APP_SETTINGS: {
        Settings old = g_cfg;
        LoadSettings();
        if (old.attach != g_cfg.attach || old.topGap != g_cfg.topGap) {
            g_attachNow = g_cfg.attach;
            g_gapPx = g_cfg.topGap;
            SavePos();
        }
        RegisterMoveHotkey();
        UpdateMonitor();
        g_meter.next = 0;
        g_dirty = true;
        return 0;
    }
    case WM_DISPLAYCHANGE:
    case WM_DPICHANGED:
    case WM_SETTINGCHANGE:
        UpdateMonitor();
        g_dirty = true;
        return 0;
    case WM_DESTROY:
        if (h == g_hwnd) {  // the glow windows share this class; only the main window ends the loop
            SavePos();
            KillTimer(h, 1);
            UnregisterHotKey(h, 1);
            PostQuitMessage(0);
        }
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static DWORD WINAPI UiThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    GdiplusStartupInput in;
    ULONG_PTR tok = 0;
    GdiplusStartup(&tok, &in, nullptr);
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    g_qpf = f.QuadPart;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WhSpotifyIsland2";
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    RegisterClassW(&wc);

    g_cx = Wh_GetIntValue(L"cx", GetSystemMetrics(SM_CXSCREEN) / 2);
    g_attachNow = Wh_GetIntValue(L"attach", g_cfg.attach ? 1 : 0) != 0;
    g_gapPx = std::clamp(Wh_GetIntValue(L"gap", g_cfg.topGap), 0, 4000);
    g_dockBottom = Wh_GetIntValue(L"bottom", 0) != 0;
    UpdateMonitor();

    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                             wc.lpszClassName, L"Spotify Island", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr,
                             wc.hInstance, nullptr);
    if (g_hwnd) {
        CreateEdgeWins(wc.hInstance, wc.lpszClassName);
        RegisterMoveHotkey();
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        SetTimer(g_hwnd, 1, 15, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    DestroyEdgeWins();
    if (g_epv) { g_epv->Release(); g_epv = nullptr; }
    g_meter.Release();
    delete g_art; delete g_artPrev; g_art = g_artPrev = nullptr;
    delete g_fSmall; delete g_fTitle; delete g_fArtist; delete g_fTime; delete g_fam;
    g_fSmall = g_fTitle = g_fArtist = g_fTime = nullptr; g_fam = nullptr;
    FreeDib();
    GdiplusShutdown(tok);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    g_hwnd = nullptr;
    return 0;
}

// ================================================================== windhawk entry points
BOOL Wh_ModInit() {
    // only in the real shell explorer.exe, not in separate file-manager processes
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD pid = 0;
        GetWindowThreadProcessId(tray, &pid);
        if (pid != GetCurrentProcessId()) return TRUE;
    }
    g_mutex = CreateMutexW(nullptr, TRUE, L"Local\\WhSpotifyIslandMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_mutex);
        g_mutex = nullptr;
        return TRUE;
    }
    LoadSettings();
    g_quit = false;
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mediaThread = CreateThread(nullptr, 0, MediaThread, nullptr, 0, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
    else LoadSettings();
}

void Wh_ModUninit() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    g_quit = true;
    if (g_wake) SetEvent(g_wake);
    if (g_mediaThread) {
        WaitForSingleObject(g_mediaThread, 8000);
        CloseHandle(g_mediaThread);
        g_mediaThread = nullptr;
    }
    if (g_wake) { CloseHandle(g_wake); g_wake = nullptr; }
    if (g_mutex) { CloseHandle(g_mutex); g_mutex = nullptr; }
}
