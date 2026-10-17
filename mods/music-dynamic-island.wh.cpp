// ==WindhawkMod==
// @id              music-dynamic-island
// @name            Music Dynamic Island 
// @description     Music Dynamic Island inspired by iPhone's one and Matcha external music player
// @version         1.1.0
// @author          MrsWorthless
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lgdiplus -lole32 -lshlwapi -lshell32 -lwininet -lwindowsapp -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Music Dynamic Island
A smooth, DPI-aware desktop music island inspired by the iPhone Dynamic Island and the Matcha external music player. It features an atmospheric blurred album-art mesh backdrop, an expandable bottom lyrics drawer, and a full two-column Apple Music / Matcha studio view with word-by-word karaoke lyric illumination.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TopOffset: 8
  $name: Default Top Offset (px)
  $description: Distance from the top of the primary monitor work area (minimum 0).
- CompactWidth: 196
  $name: Compact Width (px)
  $description: Width of the collapsed pill in logical pixels at 96 DPI (minimum 150).
- CompactHeight: 34
  $name: Compact Height (px)
  $description: Height of the collapsed pill in logical pixels at 96 DPI (minimum 28).
- NormalWidth: 328
  $name: Normal Expanded Width (px)
  $description: Width of the hovered media card in logical pixels at 96 DPI (minimum 290).
- NormalHeight: 128
  $name: Normal Expanded Height (px)
  $description: Height of the hovered media card before the lyrics drawer opens (minimum 118).
- BigWidth: 760
  $name: Full Studio Width (px)
  $description: Width of the expanded two-column studio window in logical pixels (minimum 680).
- BigHeight: 460
  $name: Full Studio Height (px)
  $description: Height of the expanded two-column studio window in logical pixels (minimum 430).
- GlassOpacity: 240
  $name: Glass Richness (180-255)
  $description: Opacity of the frosted glass backdrop (valid range 180 to 255).
- PreferSpotify: true
  $name: Prefer Playing Spotify Session
  $description: When enabled, an actively playing Spotify session takes priority over background browser tabs.
- AutoHideFullscreen: true
  $name: Auto-Hide in Fullscreen Apps
  $description: Automatically hides the island while a fullscreen game or video is active.
- HideHotkeyModCtrl: true
  $name: Require Ctrl for F12 Hide Hotkey
  $description: Uses Ctrl+F12 when enabled (recommended so F12 in browsers still opens DevTools), or bare F12 when disabled.
- EnableOnlineLyrics: false
  $name: Enable Online Lyrics Lookup (LRCLIB)
  $description: Opt-in setting that sends the current track title and artist to lrclib.net over HTTPS to fetch synced lyrics. Off by default.
- LocalLyricsFolder: ""
  $name: Local .lrc Folder (Optional)
  $description: Optional folder path to load offline synced .lrc files (matches Artist - Title.lrc or Title.lrc).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <objidl.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <wininet.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <gdiplus.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <thread>
#include <optional>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <algorithm>

extern "C" IMAGE_DOS_HEADER __ImageBase;

using namespace Gdiplus;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;

constexpr UINT WM_ISLAND_RELOAD_SETTINGS = WM_APP + 1;
constexpr UINT WM_ISLAND_SHUTDOWN        = WM_APP + 2;
constexpr UINT WM_ISLAND_MEDIA_CHANGED   = WM_APP + 3;
constexpr UINT_PTR TIMER_ID_FRAME        = 1;
constexpr int HOTKEY_ID_HIDE             = 1;

constexpr UINT TIMER_INTERVAL_ANIM         = 8;
constexpr UINT TIMER_INTERVAL_IDLE_PLAYING = 250;

struct IslandSettings {
    int topOffset = 8, compactW = 196, compactH = 34;
    int expandedW = 328, expandedH = 128, bigW = 760, bigH = 460, glassOpacity = 240;
    bool preferSpotify = true, autoHideFullscreen = true, hideHotkeyCtrl = true, enableOnlineLyrics = false;
    std::wstring localLyricsFolder;
} g_set;

struct LyricLine { double timeSec; std::wstring text; };
struct LyricHitBox { int lineIdx; RectF rect; };

[[clang::no_destroy]] std::optional<std::thread> g_uiThread;
HANDLE            g_uiReadyEvent  = nullptr;
HANDLE            g_stopEvent     = nullptr;
HANDLE            g_smtcWakeEvent = nullptr;
HANDLE            g_audioWakeEvent = nullptr;
HANDLE            g_lyricsWakeEvent = nullptr;

std::atomic<HWND>  g_hWnd{nullptr};
std::atomic<bool>  g_audioCaptureNeeded{false};
std::atomic<bool>  g_audioDeviceChanged{false};
std::atomic<int>   g_pendingCmd{0};
std::atomic<float> g_pendingSeek{-1.0f};
std::atomic<float> g_audioBands[5]{0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

std::mutex g_inetMutex;
HINTERNET  g_activeInetNet = nullptr;
HINTERNET  g_activeInetUrl = nullptr;

std::mutex g_mediaMutex;
struct MediaData {
    std::wstring title = L"Nothing Playing", subtitle = L"Play music to start", rawArtist;
    std::wstring prevTitle, prevSubtitle, lyricsKey;
    bool isPlaying = false, isShuffle = false;
    int repeatMode = 0;
    double posSec = 0.0, durSec = 0.0;
    ULONGLONG posTickMs = 0;
    uint32_t trackSeq = 0;
    Color accent = Color(255, 220, 90, 140), prevAccent = Color(255, 220, 90, 140);
    std::unique_ptr<Bitmap> bigArt, prevBigArt, expArt, prevExpArt, miniArt, prevMiniArt, blurBg, prevBlurBg;
    std::vector<LyricLine> lyrics;
    std::wstring lyricStatus = L"Lyrics lookup disabled in settings";
} g_media;

struct CachedRenderResources {
    HDC     memDC  = nullptr;
    HBITMAP dib    = nullptr;
    HBITMAP oldBmp = nullptr;
    int     width  = 0;
    int     height = 0;

    std::unique_ptr<FontFamily> fontFamily;
    std::unique_ptr<Font>       titleFont;
    std::unique_ptr<Font>       subFont;
    std::unique_ptr<Font>       timeFont;
    std::unique_ptr<Font>       drawerActFont;
    std::unique_ptr<Font>       drawerNextFont;
    std::unique_ptr<Font>       bigTitleFont;
    std::unique_ptr<Font>       bigSubFont;
    std::unique_ptr<Font>       bigLrcFont;

    uint32_t          cachedLyricSeq   = 0;
    size_t            cachedLyricCount = 0;
    REAL              cachedLyricWidth = 0.0f;
    std::vector<REAL> cachedLineH;
    std::vector<REAL> cachedLineY;

    void EnsureSurface(HDC hdcScreen, int w, int h) {
        if (memDC && dib && width == w && height == h) {
            return;
        }
        ReleaseSurface();
        memDC = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = w;
        bmi.bmiHeader.biHeight      = -h;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        dib = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (dib) {
            oldBmp = (HBITMAP)SelectObject(memDC, dib);
            width  = w;
            height = h;
        }
    }

    void EnsureFonts() {
        if (fontFamily && titleFont) return;
        fontFamily     = std::make_unique<FontFamily>(L"Segoe UI");
        titleFont      = std::make_unique<Font>(fontFamily.get(), 13.5f, FontStyleBold, UnitPixel);
        subFont        = std::make_unique<Font>(fontFamily.get(), 12.0f, FontStyleBold, UnitPixel);
        timeFont       = std::make_unique<Font>(fontFamily.get(), 10.5f, FontStyleBold, UnitPixel);
        drawerActFont  = std::make_unique<Font>(fontFamily.get(), 12.5f, FontStyleBold, UnitPixel);
        drawerNextFont = std::make_unique<Font>(fontFamily.get(), 11.5f, FontStyleBold, UnitPixel);
        bigTitleFont   = std::make_unique<Font>(fontFamily.get(), 15.0f, FontStyleBold, UnitPixel);
        bigSubFont     = std::make_unique<Font>(fontFamily.get(), 13.0f, FontStyleBold, UnitPixel);
        bigLrcFont     = std::make_unique<Font>(fontFamily.get(), 21.0f, FontStyleBold, UnitPixel);
    }

    void ReleaseSurface() {
        if (memDC) {
            if (oldBmp) { SelectObject(memDC, oldBmp); oldBmp = nullptr; }
            if (dib)    { DeleteObject(dib); dib = nullptr; }
            DeleteDC(memDC);
            memDC = nullptr;
        }
        width = height = 0;
    }

    void ReleaseAll() {
        ReleaseSurface();
        titleFont.reset(); subFont.reset(); timeFont.reset();
        drawerActFont.reset(); drawerNextFont.reset();
        bigTitleFont.reset(); bigSubFont.reset(); bigLrcFont.reset();
        fontFamily.reset();
        cachedLineH.clear(); cachedLineY.clear();
    }
} g_res;

struct UIState {
    float curW = 196.0f, curH = 34.0f, expandT = 0.0f, bigModeT = 0.0f, drawerT = 0.0f, bigLyricsT = 1.0f;
    float dpiScale = 1.0f;
bool hovered = false, hidden = false, fullscreenHidden = false, wasF12Down = false;
    bool bigMode = false, showLyrics = false;
    bool hasSavedBigPos = false, draggingWin = false, draggingSeek = false, hoveringSeek = false;
    float savedBigX = 0.0f, savedBigY = 80.0f, bigPosT = 0.0f;
    int curWinX = 0, curWinY = 8;
    POINT dragStartMouse{}, dragStartWin{};
    int hoveredBtn = -1, hoveredLyricIdx = -1;
    float btnHoverT[8]{}, seekHoverT = 0.0f;
    float dragSeekRatio = 0.0f, dispRatio = 0.0f, holdSeekRatio = -1.0f;
    ULONGLONG holdSeekUntil = 0;
    float smoothBands[5]{}, trackAnimT = 1.0f, bounceW = 0.0f;
    float marqueeTime = 0.0f, smoothLyricIdx = 0.0f;
    bool marqueeActive = false;
    int lastActiveLyric = -1;
    uint32_t seenSeq = 0;
    UINT currentTimerInterval = 0;
    ULONGLONG lastFullscreenCheckMs = 0;
    std::vector<LyricHitBox> lyricHits;
    LONGLONG lastQpc = 0;
    double qpcFreq = 1.0;
} g_ui;

void LoadModSettings() {
    g_set.topOffset          = std::max(0, Wh_GetIntSetting(L"TopOffset"));
    g_set.compactW           = std::max(150, Wh_GetIntSetting(L"CompactWidth"));
    g_set.compactH           = std::max(28, Wh_GetIntSetting(L"CompactHeight"));
    g_set.expandedW          = std::max(290, Wh_GetIntSetting(L"NormalWidth"));
    g_set.expandedH          = std::max(118, Wh_GetIntSetting(L"NormalHeight"));
    g_set.bigW               = std::max(680, Wh_GetIntSetting(L"StudioWidth"));
    g_set.bigH               = std::max(430, Wh_GetIntSetting(L"StudioHeight"));
    g_set.glassOpacity       = std::clamp(Wh_GetIntSetting(L"GlassOpacity"), 180, 255);
    g_set.preferSpotify      = Wh_GetIntSetting(L"PreferSpotify") != 0;
    g_set.autoHideFullscreen = Wh_GetIntSetting(L"AutoHideFullscreen") != 0;
    g_set.hideHotkeyCtrl     = Wh_GetIntSetting(L"HideHotkeyModCtrl") != 0;
    g_set.enableOnlineLyrics = Wh_GetIntSetting(L"EnableOnlineLyrics") != 0;

    PCWSTR folderPtr = Wh_GetStringSetting(L"LocalLyricsFolder");
    g_set.localLyricsFolder = folderPtr ? folderPtr : L"";
    if (folderPtr) Wh_FreeStringSetting(folderPtr);
}

void RegisterIslandHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_ID_HIDE);
    UINT mods = g_set.hideHotkeyCtrl ? MOD_CONTROL : 0;
    RegisterHotKey(hwnd, HOTKEY_ID_HIDE, mods | MOD_NOREPEAT, VK_F12);
}

void SetTimerIntervalIfNeeded(HWND hwnd, UINT intervalMs) {
    if (g_ui.currentTimerInterval == intervalMs) return;
    if (intervalMs == 0) {
        KillTimer(hwnd, TIMER_ID_FRAME);
        g_ui.currentTimerInterval = 0;
    } else {
        LARGE_INTEGER n{};
        QueryPerformanceCounter(&n);
        g_ui.lastQpc = n.QuadPart;
        SetTimer(hwnd, TIMER_ID_FRAME, intervalMs, nullptr);
        g_ui.currentTimerInterval = intervalMs;
    }
}

void WakeAnimationTimer(HWND hwnd) {
    if (!g_ui.hidden && !g_ui.fullscreenHidden) {
        SetTimerIntervalIfNeeded(hwnd, TIMER_INTERVAL_ANIM);
    }
}

void UpdateAudioCaptureNeeded() {
    bool isPlaying = false;
    {
        std::lock_guard<std::mutex> lk(g_mediaMutex);
        isPlaying = g_media.isPlaying;
    }
    bool needed = isPlaying && !g_ui.hidden && !g_ui.fullscreenHidden && (g_ui.bigModeT < 0.95f);
    bool prev   = g_audioCaptureNeeded.exchange(needed);
    if (needed && !prev && g_audioWakeEvent) {
        SetEvent(g_audioWakeEvent);
    }
}

void GetRoundRect(GraphicsPath& p, REAL x, REAL y, REAL w, REAL h, REAL r) {
    p.Reset();
    if (w <= 0.5f || h <= 0.5f) return;
    REAL d = std::min({r * 2.0f, w, h});
    if (d <= 0.5f) { p.AddRectangle(RectF(x, y, w, h)); return; }
    p.AddArc(x, y, d, d, 180.0f, 90.0f);
    p.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    p.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    p.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    p.CloseFigure();
}

void FormatMMSS(double s, wchar_t* b, size_t n, bool neg = false) {
    int sec = static_cast<int>(std::max(0.0, std::isfinite(s) ? s : 0.0) + 0.5);
    std::swprintf(b, n, neg ? L"-%d:%02d" : L"%d:%02d", sec / 60, sec % 60);
}

Color LerpCol(Color a, Color b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return Color(255,
        static_cast<BYTE>(a.GetR() + (b.GetR() - a.GetR()) * t),
        static_cast<BYTE>(a.GetG() + (b.GetG() - a.GetG()) * t),
        static_cast<BYTE>(a.GetB() + (b.GetB() - a.GetB()) * t));
}

void DrawBmpAlpha(Graphics& g, Bitmap* b, REAL x, REAL y, REAL w, REAL h, float a) {
    if (!b || a <= 0.01f) return;
    a = std::clamp(a, 0.0f, 1.0f);
    if (a >= 0.99f) {
        g.DrawImage(b, x, y, w, h);
        return;
    }
    ImageAttributes attr;
    ColorMatrix cm = {{{1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0}, {0,0,0,a,0}, {0,0,0,0,1}}};
    attr.SetColorMatrix(&cm, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
    g.DrawImage(b, RectF(x, y, w, h), 0, 0, (REAL)b->GetWidth(), (REAL)b->GetHeight(), UnitPixel, &attr);
}

std::unique_ptr<Bitmap> MakeRoundBmp(Bitmap* src, int sz, float r) {
    auto dst = std::make_unique<Bitmap>(sz, sz, PixelFormat32bppPARGB);
    Graphics g(dst.get());
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    GraphicsPath cp; GetRoundRect(cp, 0, 0, (REAL)sz, (REAL)sz, r);
    g.SetClip(&cp); g.DrawImage(src, 0, 0, sz, sz);
    return dst;
}

std::unique_ptr<Bitmap> MakeBlurMesh(Bitmap* src) {
    Bitmap s1(12, 12, PixelFormat32bppPARGB);
    {
        Graphics g1(&s1);
        g1.SetInterpolationMode(InterpolationModeHighQualityBilinear);
        g1.SetPixelOffsetMode(PixelOffsetModeHalf);
        g1.DrawImage(src, Rect(0, 0, 12, 12), 0, 0, src->GetWidth(), src->GetHeight(), UnitPixel);
    }
    Bitmap s2(5, 5, PixelFormat32bppPARGB);
    {
        Graphics g2(&s2);
        g2.SetInterpolationMode(InterpolationModeHighQualityBilinear);
        g2.SetPixelOffsetMode(PixelOffsetModeHalf);
        ImageAttributes ia; ia.SetWrapMode(WrapModeTileFlipXY);
        g2.DrawImage(&s1, Rect(0, 0, 5, 5), 0, 0, 12, 12, UnitPixel, &ia);
    }
    auto mesh = std::make_unique<Bitmap>(96, 96, PixelFormat32bppPARGB);
    Graphics gm(mesh.get());
    gm.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    gm.SetPixelOffsetMode(PixelOffsetModeHalf);
    ImageAttributes attr; attr.SetWrapMode(WrapModeTileFlipXY);
    gm.DrawImage(&s2, Rect(0, 0, 96, 96), 0, 0, 5, 5, UnitPixel, &attr);
    SolidBrush wash(Color(115, 18, 16, 20));
    gm.FillRectangle(&wash, 0, 0, 96, 96);
    return mesh;
}

Color ExtractAccent(Bitmap* b) {
    if (!b) return Color(255, 220, 90, 140);
    float best = -1.0f; BYTE br = 220, bg = 90, bb = 140;
    for (UINT y = 1; y < b->GetHeight() - 1; y += 2) {
        for (UINT x = 1; x < b->GetWidth() - 1; x += 2) {
            Color c; if (b->GetPixel(x, y, &c) != Ok || c.GetA() < 128) continue;
            float r = c.GetR() / 255.f, g = c.GetG() / 255.f, bl = c.GetB() / 255.f;
            float mx = std::max({r, g, bl}), mn = std::min({r, g, bl});
            float sat = (mx > 0.01f) ? ((mx - mn) / mx) : 0.f;
            if (mx > 0.18f && mx < 0.98f && (sat + 0.20f) * mx > best) {
                best = (sat + 0.20f) * mx;
                float boost = (mx < 0.85f) ? (0.90f / mx) : 1.f;
                br = (BYTE)std::min(255.f, c.GetR() * boost);
                bg = (BYTE)std::min(255.f, c.GetG() * boost);
                bb = (BYTE)std::min(255.f, c.GetB() * boost);
            }
        }
    }
    return Color(255, br, bg, bb);
}

bool DecodeArt(IRandomAccessStreamReference const& ref, std::unique_ptr<Bitmap>& bigB,
               std::unique_ptr<Bitmap>& expB, std::unique_ptr<Bitmap>& miniB,
               std::unique_ptr<Bitmap>& blurB, Color& acc) {
    if (!ref) return false;
    try {
        auto s = ref.OpenReadAsync().get();
        if (!s || s.Size() == 0) return false;
        uint32_t sz = (uint32_t)s.Size();
        DataReader rd(s); rd.LoadAsync(sz).get();
        std::vector<uint8_t> buf(sz); rd.ReadBytes(buf); rd.Close(); s.Close();
        IStream* ms = SHCreateMemStream(buf.data(), sz);
        if (!ms) return false;
        std::unique_ptr<Bitmap> raw(Bitmap::FromStream(ms));
        bool ok = false;
        if (raw && raw->GetLastStatus() == Ok) {
            bigB  = MakeRoundBmp(raw.get(), 230, 14.0f);
            expB  = MakeRoundBmp(raw.get(), 48, 10.0f);
            miniB = MakeRoundBmp(raw.get(), 20, 6.0f);
            blurB = MakeBlurMesh(raw.get());
            acc   = ExtractAccent(miniB.get());
            ok = true;
        }
        raw.reset(); ms->Release();
        return ok;
    } catch (...) { return false; }
}

std::string UrlEncodeUtf8(const std::wstring& w) {
    if (w.empty()) return "";
    int sz = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (sz <= 1) return "";
    std::string u(sz - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, u.data(), sz, nullptr, nullptr);
    std::string out; char hex[4];
    for (unsigned char c : u) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') out += (char)c;
        else if (c == ' ') out += '+';
        else { std::snprintf(hex, sizeof(hex), "%%%02X", c); out += hex; }
    }
    return out;
}

void AbortActiveHttpRequests() {
    std::lock_guard<std::mutex> lk(g_inetMutex);
    if (g_activeInetUrl) {
        InternetCloseHandle(g_activeInetUrl);
        g_activeInetUrl = nullptr;
    }
    if (g_activeInetNet) {
        InternetCloseHandle(g_activeInetNet);
        g_activeInetNet = nullptr;
    }
}

std::string HttpGetAbortable(const std::string& url) {
    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return "";
    HINTERNET hNet = InternetOpenA("MusicDynamicIsland/1.1", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hNet) return "";
    {
        std::lock_guard<std::mutex> lk(g_inetMutex);
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            InternetCloseHandle(hNet);
            return "";
        }
        g_activeInetNet = hNet;
    }

    DWORD timeout = 2500;
    InternetSetOptionA(hNet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hNet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hNet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    HINTERNET hUrl = InternetOpenUrlA(hNet, url.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hUrl) {
        std::lock_guard<std::mutex> lk(g_inetMutex);
        if (g_activeInetNet == hNet) {
            InternetCloseHandle(hNet);
            g_activeInetNet = nullptr;
        }
        return "";
    }
    {
        std::lock_guard<std::mutex> lk(g_inetMutex);
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            InternetCloseHandle(hUrl);
            if (g_activeInetNet == hNet) {
                InternetCloseHandle(hNet);
                g_activeInetNet = nullptr;
            }
            return "";
        }
        g_activeInetUrl = hUrl;
    }

    std::string resp;
    char buf[2048];
    DWORD read = 0;
    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0 &&
           InternetReadFile(hUrl, buf, sizeof(buf), &read) && read > 0) {
        resp.append(buf, read);
    }

    {
        std::lock_guard<std::mutex> lk(g_inetMutex);
        if (g_activeInetUrl == hUrl) {
            InternetCloseHandle(hUrl);
            g_activeInetUrl = nullptr;
        }
        if (g_activeInetNet == hNet) {
            InternetCloseHandle(hNet);
            g_activeInetNet = nullptr;
        }
    }
    return resp;
}

bool ParseLrcContent(const std::wstring& wlrc, std::vector<LyricLine>& outLines) {
    size_t start = 0;
    while (start < wlrc.size()) {
        size_t end = wlrc.find(L'\n', start);
        if (end == std::wstring::npos) end = wlrc.size();
        std::wstring line = wlrc.substr(start, end - start);
        start = end + 1;
        int m = 0; float s = 0.f;
        size_t brk = line.find(L']');
        if (line.size() > 6 && line[0] == L'[' && brk != std::wstring::npos &&
            std::swscanf(line.c_str(), L"[%d:%f]", &m, &s) == 2) {
            std::wstring rawTxt = line.substr(brk + 1);
            std::wstring txt;
            bool inTag = false;
            for (wchar_t ch : rawTxt) {
                if (ch == L'<') inTag = true;
                else if (ch == L'>') inTag = false;
                else if (!inTag && ch != L'\r') txt += ch;
            }
            while (!txt.empty() && (txt.front() == L' ' || txt.front() == L'\t')) txt.erase(txt.begin());
            while (!txt.empty() && (txt.back() == L' ' || txt.back() == L'\t')) txt.pop_back();
            if (!txt.empty()) outLines.push_back({m * 60.0 + s, txt});
        }
    }
    return !outLines.empty();
}

bool TryLoadLocalLrc(const std::wstring& folder, const std::wstring& artist, const std::wstring& title, std::vector<LyricLine>& outLines) {
    if (folder.empty()) return false;
    auto sanitize = [](std::wstring s) {
        for (wchar_t& c : s) {
            if (wcschr(L"\\/:*?\"<>|", c)) c = L'_';
        }
        return s;
    };
    std::wstring base = folder;
    if (base.back() != L'\\' && base.back() != L'/') base += L'\\';

    std::wstring candidates[2] = {
        base + sanitize(artist + L" - " + title) + L".lrc",
        base + sanitize(title) + L".lrc"
    };

    for (auto const& path : candidates) {
        std::ifstream f(path.c_str(), std::ios::binary);
        if (!f) continue;
        std::string raw((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        if (raw.empty()) continue;
        int wlen = MultiByteToWideChar(CP_UTF8, 0, raw.data(), (int)raw.size(), nullptr, 0);
        if (wlen <= 0) continue;
        std::wstring wlrc(wlen, 0);
        MultiByteToWideChar(CP_UTF8, 0, raw.data(), (int)raw.size(), wlrc.data(), wlen);
        if (ParseLrcContent(wlrc, outLines)) return true;
    }
    return false;
}

bool ParseSyncedLyricsJson(const std::string& resp, std::vector<LyricLine>& outLines) {
    size_t pos = 0;
    while ((pos = resp.find("\"syncedLyrics\"", pos)) != std::string::npos) {
        pos += 14;
        while (pos < resp.size() && (resp[pos] == ' ' || resp[pos] == ':' || resp[pos] == '\t')) ++pos;
        if (pos >= resp.size() || resp[pos] != '\"') continue;
        ++pos;

        std::wstring wlrc;
        std::string utf8Chunk;
        auto flushUtf8 = [&]() {
            if (utf8Chunk.empty()) return;
            int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Chunk.data(), (int)utf8Chunk.size(), nullptr, 0);
            if (wlen > 0) {
                size_t oldSz = wlrc.size();
                wlrc.resize(oldSz + wlen);
                MultiByteToWideChar(CP_UTF8, 0, utf8Chunk.data(), (int)utf8Chunk.size(), &wlrc[oldSz], wlen);
            }
            utf8Chunk.clear();
        };

        size_t i = pos;
        for (; i < resp.size(); ++i) {
            char c = resp[i];
            if (c == '\"') {
                ++i;
                break;
            }
            if (c == '\\' && i + 1 < resp.size()) {
                char nx = resp[++i];
                if (nx == 'n') utf8Chunk += '\n';
                else if (nx == 'r') utf8Chunk += '\r';
                else if (nx == 't') utf8Chunk += ' ';
                else if (nx == '\"' || nx == '\\' || nx == '/') utf8Chunk += nx;
                else if (nx == 'u' && i + 4 < resp.size()) {
                    flushUtf8();
                    unsigned int cp = 0;
                    if (std::sscanf(resp.c_str() + i + 1, "%4x", &cp) == 1) {
                        wlrc += static_cast<wchar_t>(cp);
                    }
                    i += 4;
                }
            } else {
                utf8Chunk += c;
            }
        }
        flushUtf8();
        pos = i;

        if (ParseLrcContent(wlrc, outLines)) return true;
    }
    return false;
}

void LyricsWorkerThread() {
    std::wstring fetchedKey;
    bool lastOnlineSetting = false;
    std::wstring lastFolderSetting;

    HANDLE waitHandles[2] = { g_stopEvent, g_lyricsWakeEvent };

    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        std::wstring curKey, title, artist, localFolder;
        bool onlineEnabled = false;
        {
            std::lock_guard<std::mutex> lk(g_mediaMutex);
            curKey        = g_media.lyricsKey;
            title         = g_media.title;
            artist        = g_media.rawArtist;
            onlineEnabled = g_set.enableOnlineLyrics;
            localFolder   = g_set.localLyricsFolder;
        }

        bool settingsChanged = (onlineEnabled != lastOnlineSetting) || (localFolder != lastFolderSetting);
        if (settingsChanged) {
            lastOnlineSetting = onlineEnabled;
            lastFolderSetting = localFolder;
            fetchedKey.clear();
        }

        if (!curKey.empty() && curKey != fetchedKey && title != L"Nothing Playing") {
            fetchedKey = curKey;
            std::vector<LyricLine> lines;
            std::wstring statusText;

            if (TryLoadLocalLrc(localFolder, artist, title, lines)) {
                statusText = L"";
            } else if (!onlineEnabled) {
                statusText = L"Online lyrics disabled in settings";
            } else {
                {
                    std::lock_guard<std::mutex> lk(g_mediaMutex);
                    if (g_media.lyricsKey == curKey) g_media.lyricStatus = L"Searching lyrics...";
                }
                if (HWND h = g_hWnd.load()) PostMessageW(h, WM_ISLAND_MEDIA_CHANGED, 0, 0);

                std::string tEnc = UrlEncodeUtf8(title), aEnc = UrlEncodeUtf8(artist);
                std::string resp = HttpGetAbortable("https://lrclib.net/api/get?track_name=" + tEnc + "&artist_name=" + aEnc);
                if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
                if (!ParseSyncedLyricsJson(resp, lines)) {
                    resp = HttpGetAbortable("https://lrclib.net/api/search?q=" + tEnc + "+" + aEnc);
                    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
                    ParseSyncedLyricsJson(resp, lines);
                }
                statusText = lines.empty() ? L"No synced lyrics" : L"";
            }

            {
                std::lock_guard<std::mutex> lk(g_mediaMutex);
                if (g_media.lyricsKey == curKey) {
                    g_media.lyrics      = std::move(lines);
                    g_media.lyricStatus = std::move(statusText);
                }
            }
            if (HWND h = g_hWnd.load()) PostMessageW(h, WM_ISLAND_MEDIA_CHANGED, 0, 0);
        }

        WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
    }
}

class AudioEndpointNotificationClient final : public IMMNotificationClient {
    LONG _ref = 1;
public:
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&_ref);
        if (r == 0) delete this;
        return r;
    }
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eConsole) {
            g_audioDeviceChanged.store(true);
            if (g_audioWakeEvent) SetEvent(g_audioWakeEvent);
        }
        return S_OK;
    }
    STDMETHODIMP OnDeviceAdded(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
};

void AudioLoopbackThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IMMDeviceEnumerator* pEnum = nullptr;
    IMMDevice*           pDev = nullptr;
    IAudioClient*        pClient = nullptr;
    IAudioCaptureClient* pCap = nullptr;
    WAVEFORMATEX*        pwfx = nullptr;

    AudioEndpointNotificationClient* pNotify = new AudioEndpointNotificationClient();
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum)) && pEnum) {
        pEnum->RegisterEndpointNotificationCallback(pNotify);
    }

    auto cleanupClient = [&]() {
        if (pClient) pClient->Stop();
        if (pwfx)    { CoTaskMemFree(pwfx); pwfx = nullptr; }
        if (pCap)    { pCap->Release(); pCap = nullptr; }
        if (pClient) { pClient->Release(); pClient = nullptr; }
        if (pDev)    { pDev->Release(); pDev = nullptr; }
    };

    float lp[4]{}, peak = 0.04f;
    HANDLE waitHandles[2] = { g_stopEvent, g_audioWakeEvent };

    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        if (!g_audioCaptureNeeded.load()) {
            cleanupClient();
            for (int i = 0; i < 5; ++i) g_audioBands[i].store(0.0f);
            WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
            continue;
        }

        if (g_audioDeviceChanged.exchange(false)) {
            cleanupClient();
        }

        if (!pCap) {
            cleanupClient();
            if (pEnum &&
                SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev)) &&
                SUCCEEDED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pClient)) &&
                SUCCEEDED(pClient->GetMixFormat(&pwfx)) &&
                SUCCEEDED(pClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 150000, 0, pwfx, nullptr)) &&
                SUCCEEDED(pClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCap))) {
                pClient->Start();
            } else {
                cleanupClient();
                if (WaitForSingleObject(g_stopEvent, 250) == WAIT_OBJECT_0) break;
                continue;
            }
        }

        UINT32 pkt = 0;
        if (FAILED(pCap->GetNextPacketSize(&pkt))) {
            cleanupClient();
            continue;
        }

        float eng[5]{};
        UINT32 count = 0;
        while (pkt != 0) {
            BYTE* data = nullptr; UINT32 frames = 0; DWORD flags = 0;
            if (FAILED(pCap->GetBuffer(&data, &frames, &flags, nullptr, nullptr))) break;
            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && data && pwfx->wBitsPerSample == 32) {
                const float* smp = (const float*)data; int ch = pwfx->nChannels;
                for (UINT32 f = 0; f < frames; ++f) {
                    float m = (ch > 1) ? 0.5f * (smp[f * ch] + smp[f * ch + 1]) : smp[f * ch];
                    lp[0] += 0.018f * (m - lp[0]); lp[1] += 0.055f * (m - lp[1]);
                    lp[2] += 0.145f * (m - lp[2]); lp[3] += 0.360f * (m - lp[3]);
                    float b0 = lp[0], b1 = lp[1] - lp[0], b2 = lp[2] - lp[1], b3 = lp[3] - lp[2], b4 = m - lp[3];
                    eng[0] += b0 * b0; eng[1] += b1 * b1 * 1.6f; eng[2] += b2 * b2 * 2.2f;
                    eng[3] += b3 * b3 * 2.8f; eng[4] += b4 * b4 * 3.5f;
                }
                count += frames;
            }
            pCap->ReleaseBuffer(frames);
            if (FAILED(pCap->GetNextPacketSize(&pkt))) break;
        }

        if (count > 0) {
            float fmax = 0.f, rms[5];
            for (int i = 0; i < 5; ++i) { rms[i] = std::sqrt(eng[i] / count); fmax = std::max(fmax, rms[i]); }
            peak = std::max({0.015f, peak * 0.988f, fmax});
            for (int i = 0; i < 5; ++i) g_audioBands[i].store(rms[i] > 0.0004f ? std::min(1.f, rms[i] / (peak * 0.85f)) : 0.f);
        } else {
            for (int i = 0; i < 5; ++i) g_audioBands[i].store(g_audioBands[i].load() * 0.86f);
        }

        if (WaitForSingleObject(g_stopEvent, 12) == WAIT_OBJECT_0) break;
    }

    cleanupClient();
    if (pEnum) {
        pEnum->UnregisterEndpointNotificationCallback(pNotify);
        pEnum->Release();
    }
    pNotify->Release();
    CoUninitialize();
}

void SendMediaVk(WORD vk) {
    INPUT inputs[2]{};
    inputs[0].type       = INPUT_KEYBOARD;
    inputs[0].ki.wVk     = vk;
    inputs[1].type       = INPUT_KEYBOARD;
    inputs[1].ki.wVk     = vk;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

void SmtcWorkerThread() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    std::wstring lastKey;
    double lastRawPos = -1.0;
    bool lastPlayingState = false;

    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    winrt::event_token tokSessions{}, tokCurSession{};
    bool mgrSubscribed = false;

    struct SubscribedSession {
        GlobalSystemMediaTransportControlsSession session{nullptr};
        winrt::event_token tokProps{}, tokPlay{}, tokTime{};
    };
    std::vector<SubscribedSession> sessionSubs;

    auto clearSessionSubs = [&]() {
        for (auto& sub : sessionSubs) {
            try {
                if (sub.session) {
                    sub.session.MediaPropertiesChanged(sub.tokProps);
                    sub.session.PlaybackInfoChanged(sub.tokPlay);
                    sub.session.TimelinePropertiesChanged(sub.tokTime);
                }
            } catch (...) {}
        }
        sessionSubs.clear();
    };

    auto clearMgrSubs = [&]() {
        clearSessionSubs();
        if (mgr && mgrSubscribed) {
            try {
                mgr.SessionsChanged(tokSessions);
                mgr.CurrentSessionChanged(tokCurSession);
            } catch (...) {}
        }
        mgrSubscribed = false;
        mgr = nullptr;
    };

    auto refreshSessionSubscriptions = [&]() {
        clearSessionSubs();
        if (!mgr) return;
        try {
            auto all = mgr.GetSessions();
            for (auto const& s : all) {
                SubscribedSession sub;
                sub.session = s;
                sub.tokProps = s.MediaPropertiesChanged([](auto&&, auto&&) {
                    if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                });
                sub.tokPlay = s.PlaybackInfoChanged([](auto&&, auto&&) {
                    if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                });
                sub.tokTime = s.TimelinePropertiesChanged([](auto&&, auto&&) {
                    if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                });
                sessionSubs.push_back(std::move(sub));
            }
        } catch (...) {}
    };

    HANDLE waitHandles[2] = { g_stopEvent, g_smtcWakeEvent };
    std::atomic<bool> needResubscribeSessions{true};

    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        try {
            if (!mgr) {
                mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
                if (mgr) {
                    tokSessions = mgr.SessionsChanged([&needResubscribeSessions](auto&&, auto&&) {
                        needResubscribeSessions.store(true);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    });
                    tokCurSession = mgr.CurrentSessionChanged([](auto&&, auto&&) {
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    });
                    mgrSubscribed = true;
                    needResubscribeSessions.store(true);
                }
            }

            if (needResubscribeSessions.exchange(false)) {
                refreshSessionSubscriptions();
            }

            GlobalSystemMediaTransportControlsSession sess{nullptr};
            if (mgr) {
                sess = mgr.GetCurrentSession();
                if (g_set.preferSpotify) {
                    auto all = mgr.GetSessions();
                    for (auto const& s : all) {
                        std::wstring appId = s.SourceAppUserModelId().c_str();
                        auto pb = s.GetPlaybackInfo();
                        bool pl = pb && (pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                        if (pl && (appId.find(L"Spotify") != std::wstring::npos || appId.find(L"spotify") != std::wstring::npos)) {
                            sess = s;
                            break;
                        }
                    }
                }
            }

            int cmd = g_pendingCmd.exchange(0);
            if (cmd != 0) {
                bool handled = false;
                if (sess) {
                    if (cmd == 1) handled = sess.TrySkipPreviousAsync().get();
                    else if (cmd == 2) handled = sess.TryTogglePlayPauseAsync().get();
                    else if (cmd == 3) handled = sess.TrySkipNextAsync().get();
                    else if (cmd == 4) {
                        bool curShuf = false;
                        { std::lock_guard<std::mutex> lk(g_mediaMutex); curShuf = g_media.isShuffle; }
                        handled = sess.TryChangeShuffleActiveAsync(!curShuf).get();
                    } else if (cmd == 5) {
                        int curRep = 0;
                        { std::lock_guard<std::mutex> lk(g_mediaMutex); curRep = g_media.repeatMode; }
                        int nextRep = (curRep + 1) % 3;
                        auto mode = (nextRep == 1) ? MediaPlaybackAutoRepeatMode::List
                                  : (nextRep == 2) ? MediaPlaybackAutoRepeatMode::Track
                                                   : MediaPlaybackAutoRepeatMode::None;
                        handled = sess.TryChangeAutoRepeatModeAsync(mode).get();
                    }
                }
                if (!handled && cmd >= 1 && cmd <= 3) {
                    SendMediaVk(cmd == 1 ? VK_MEDIA_PREV_TRACK : (cmd == 2 ? VK_MEDIA_PLAY_PAUSE : VK_MEDIA_NEXT_TRACK));
                }
            }

            float seekR = g_pendingSeek.exchange(-1.0f);
            if (seekR >= 0.0f && sess) {
                auto tl = sess.GetTimelineProperties();
                if (tl && tl.EndTime() > tl.StartTime()) {
                    int64_t ticks = tl.StartTime().count() + (int64_t)((double)(tl.EndTime() - tl.StartTime()).count() * seekR);
                    sess.TryChangePlaybackPositionAsync(ticks).get();
                    double dur = std::chrono::duration<double>(tl.EndTime() - tl.StartTime()).count();
                    std::lock_guard<std::mutex> lk(g_mediaMutex);
                    g_media.posSec = dur * seekR;
                    g_media.posTickMs = GetTickCount64();
                    lastRawPos = g_media.posSec;
                }
            }

            if (sess) {
                auto pb = sess.GetPlaybackInfo();
                bool playing = pb && (pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                bool shuf = (pb && pb.IsShuffleActive()) ? pb.IsShuffleActive().Value() : false;
                int rep = 0;
                if (pb && pb.AutoRepeatMode()) {
                    auto rm = pb.AutoRepeatMode().Value();
                    rep = (rm == MediaPlaybackAutoRepeatMode::List) ? 1 : (rm == MediaPlaybackAutoRepeatMode::Track) ? 2 : 0;
                }

                auto pr = sess.TryGetMediaPropertiesAsync().get();
                std::wstring t  = (pr && !pr.Title().empty()) ? pr.Title().c_str() : L"Unknown Track";
                std::wstring a  = (pr && !pr.Artist().empty()) ? pr.Artist().c_str() : L"Active Media";
                std::wstring al = (pr && !pr.AlbumTitle().empty()) ? pr.AlbumTitle().c_str() : L"";
                std::wstring sub = (!al.empty() && al != a) ? (a + L" \x2014 " + al) : (a + L" \x2014 " + t);
                std::wstring key = t + L"|" + a;
                bool changed = (key != lastKey);

                double rawPos = 0.0, dur = 0.0;
                bool hasNewPosAnchor = false;
                auto tl = sess.GetTimelineProperties();
                if (tl) {
                    double st = std::chrono::duration<double>(tl.StartTime()).count();
                    double en = std::chrono::duration<double>(tl.EndTime()).count();
                    dur = std::max(0.0, en - st);
                    rawPos = std::clamp(std::chrono::duration<double>(tl.Position()).count() - st, 0.0, std::max(1.0, dur));
                    if (changed || playing != lastPlayingState || std::fabs(rawPos - lastRawPos) > 0.08) {
                        lastRawPos = rawPos;
                        hasNewPosAnchor = true;
                        if (playing && dur > 0.0 && tl.LastUpdatedTime().time_since_epoch().count() > 0) {
                            double el = std::chrono::duration<double>(winrt::clock::now() - tl.LastUpdatedTime()).count();
                            if (el >= 0.0 && el < 4.0) rawPos = std::clamp(rawPos + el, 0.0, dur);
                        }
                    }
                }
                lastPlayingState = playing;

                std::unique_ptr<Bitmap> nBig, nExp, nMini, nBlur;
                Color nAcc(255, 220, 90, 140);
                if (changed && pr) {
                    DecodeArt(pr.Thumbnail(), nBig, nExp, nMini, nBlur, nAcc);
                    lastKey = key;
                }

                {
                    std::lock_guard<std::mutex> lk(g_mediaMutex);
                    if (changed) {
                        g_media.prevTitle    = std::move(g_media.title);
                        g_media.prevSubtitle = std::move(g_media.subtitle);
                        g_media.prevAccent   = g_media.accent;
                        g_media.prevBigArt   = std::move(g_media.bigArt);
                        g_media.prevExpArt   = std::move(g_media.expArt);
                        g_media.prevMiniArt  = std::move(g_media.miniArt);
                        g_media.prevBlurBg   = std::move(g_media.blurBg);
                        g_media.bigArt       = std::move(nBig);
                        g_media.expArt       = std::move(nExp);
                        g_media.miniArt      = std::move(nMini);
                        g_media.blurBg       = std::move(nBlur);
                        g_media.accent       = nAcc;
                        g_media.lyrics.clear();
                        g_media.lyricStatus  = g_set.enableOnlineLyrics ? L"Searching lyrics..." : L"Online lyrics disabled in settings";
                        g_media.lyricsKey    = key;
                        g_media.trackSeq++;
                        if (g_lyricsWakeEvent) SetEvent(g_lyricsWakeEvent);
                    }
                    g_media.title     = std::move(t);
                    g_media.rawArtist = std::move(a);
                    g_media.subtitle  = std::move(sub);
                    g_media.isPlaying = playing;
                    g_media.isShuffle = shuf;
                    g_media.repeatMode = rep;
                    g_media.durSec    = dur;
                    if (hasNewPosAnchor) {
                        g_media.posSec    = rawPos;
                        g_media.posTickMs = GetTickCount64();
                    }
                }
            } else {
                lastKey.clear();
                std::lock_guard<std::mutex> lk(g_mediaMutex);
                g_media.title      = L"Nothing Playing";
                g_media.subtitle   = L"Play music to start";
                g_media.isPlaying  = false;
                g_media.posSec     = 0.0;
                g_media.durSec     = 0.0;
                g_media.lyricsKey.clear();
            }

            if (HWND h = g_hWnd.load()) PostMessageW(h, WM_ISLAND_MEDIA_CHANGED, 0, 0);
        } catch (...) {
            clearMgrSubs();
        }

        WaitForMultipleObjects(2, waitHandles, FALSE, mgr ? INFINITE : 1000);
    }

    clearMgrSubs();
    winrt::uninit_apartment();
}

void DrawVisualizer(Graphics& g, REAL rightX, REAL cy, REAL maxH, BYTE a, Color acc) {
    constexpr REAL bw = 2.0f, gap = 1.65f, totW = 5 * bw + 4 * gap;
    REAL sx = rightX - totW;
    LinearGradientBrush br(PointF(sx, cy - maxH), PointF(sx, cy + maxH),
        Color(a, (BYTE)std::min(255, acc.GetR() + 50), (BYTE)std::min(255, acc.GetG() + 50), (BYTE)std::min(255, acc.GetB() + 50)),
        Color(a, acc.GetR(), acc.GetG(), acc.GetB()));
    br.SetWrapMode(WrapModeTileFlipXY);
    for (int i = 0; i < 5; ++i) {
        REAL h = 2.2f + g_ui.smoothBands[i] * (maxH - 2.2f);
        GraphicsPath p; GetRoundRect(p, sx + i * (bw + gap), cy - h * 0.5f, bw, h, bw * 0.5f);
        g.FillPath(&br, &p);
    }
}

void DrawMarqueeString(Graphics& g, const std::wstring& txt, Font* font, RectF box, Brush* br) {
    if (txt.empty() || !font) return;
    RectF meas;
    StringFormat nf(StringFormatFlagsNoWrap);
    nf.SetLineAlignment(StringAlignmentCenter);
    g.MeasureString(txt.c_str(), -1, font, PointF(0, 0), &nf, &meas);

    if (meas.Width <= box.Width + 2.0f) {
        g.DrawString(txt.c_str(), -1, font, box, &nf, br);
        return;
    }
    g_ui.marqueeActive = true;
    float overflow = meas.Width - box.Width + 12.0f;
    float cycle = std::fmod(g_ui.marqueeTime, 8.0f);
    float offset = 0.0f;
    if (cycle > 1.5f && cycle < 4.0f) offset = ((cycle - 1.5f) / 2.5f) * overflow;
    else if (cycle >= 4.0f && cycle < 5.5f) offset = overflow;
    else if (cycle >= 5.5f && cycle < 8.0f) offset = (1.0f - (cycle - 5.5f) / 2.5f) * overflow;

    GraphicsState st = g.Save();
    g.SetClip(box);
    RectF drawR(box.X - offset, box.Y, meas.Width + 20.0f, box.Height);
    g.DrawString(txt.c_str(), -1, font, drawR, &nf, br);
    g.Restore(st);
}

REAL DrawKaraokeLyricBlock(Graphics& g, const std::wstring& text, Font* font, RectF bounds,
                           float lineProg, bool isActive, BYTE alpha, bool centerAlign, bool measureOnly = false) {
    if (!font) return 24.0f;
    if (text.empty()) return font->GetHeight(&g) + 6.0f;

    const StringFormat* typoFmt = StringFormat::GenericTypographic();
    REAL emSize = font->GetSize();

    RectF rA, rAA;
    g.MeasureString(L"A", -1, font, PointF(0, 0), typoFmt, &rA);
    g.MeasureString(L"A A", -1, font, PointF(0, 0), typoFmt, &rAA);
    REAL spaceW = std::max(emSize * 0.28f, rAA.Width - rA.Width * 2.0f);
    REAL lineH  = std::ceil(rA.Height * 1.22f);

    struct WordToken { std::wstring str; REAL width; int row; REAL x; float startF, endF; };
    std::vector<WordToken> words;
    int totalChars = 0;
    size_t i = 0;
    while (i < text.size()) {
        while (i < text.size() && text[i] == L' ') ++i;
        if (i >= text.size()) break;
        size_t j = i;
        while (j < text.size() && text[j] != L' ') ++j;
        std::wstring w = text.substr(i, j - i);
        RectF wb;
        g.MeasureString(w.c_str(), -1, font, PointF(0, 0), typoFmt, &wb);
        words.push_back({w, wb.Width, 0, 0.0f, 0.0f, 1.0f});
        totalChars += (int)w.size() + 1;
        i = j;
    }
    if (words.empty()) return lineH;

    int cumChars = 0;
    int curRow = 0;
    REAL curX = 0.0f;
    std::vector<REAL> rowWidths(1, 0.0f);

    for (auto& w : words) {
        int wLen = (int)w.str.size() + 1;
        w.startF = (float)cumChars / (float)std::max(1, totalChars);
        cumChars += wLen;
        w.endF   = (float)cumChars / (float)std::max(1, totalChars);

        if (curX > 0.0f && curX + w.width > bounds.Width) {
            curRow++;
            rowWidths.push_back(0.0f);
            curX = 0.0f;
        }
        w.row = curRow;
        w.x   = curX;
        curX += w.width + spaceW;
        rowWidths[curRow] = curX - spaceW;
    }

    int numRows = (int)rowWidths.size();
    REAL totalH = numRows * lineH;
    if (measureOnly) return totalH;

    REAL startY = centerAlign ? (bounds.Y + (bounds.Height - totalH) * 0.5f) : bounds.Y;

    for (auto const& w : words) {
        REAL rowStartX = centerAlign ? (bounds.X + (bounds.Width - rowWidths[w.row]) * 0.5f) : bounds.X;
        REAL wx = rowStartX + w.x;
        REAL wy = startY + w.row * lineH;

        if (!isActive) {
            SolidBrush dimBr(Color(alpha, 255, 255, 255));
            g.DrawString(w.str.c_str(), -1, font, PointF(wx, wy), typoFmt, &dimBr);
        } else {
            float span = std::max(0.02f, w.endF - w.startF);
            float wProg = std::clamp((lineProg - w.startF) / span, 0.0f, 1.0f);
            float liftY = -2.0f * std::sin(wProg * 3.14159265f) - (wProg >= 1.0f ? 0.8f : 0.0f);

            BYTE unSungA = (BYTE)(alpha * 0.36f);
            SolidBrush unSungBr(Color(unSungA, 255, 255, 255));
            g.DrawString(w.str.c_str(), -1, font, PointF(wx, wy + liftY), typoFmt, &unSungBr);

            if (wProg > 0.01f) {
                GraphicsState st = g.Save();
                g.SetClip(RectF(wx - 2.0f, wy - 6.0f, (w.width + 4.0f) * wProg, lineH + 12.0f));
                if (wProg < 0.99f) {
                    SolidBrush glowBr(Color((BYTE)(alpha * 0.28f), 255, 255, 255));
                    g.DrawString(w.str.c_str(), -1, font, PointF(wx, wy + liftY + 0.5f), typoFmt, &glowBr);
                }
                SolidBrush sungBr(Color(alpha, 255, 255, 255));
                g.DrawString(w.str.c_str(), -1, font, PointF(wx, wy + liftY), typoFmt, &sungBr);
                g.Restore(st);
            }
        }
    }
    return totalH;
}

void DrawAppleIcon(Graphics& g, int id, bool playing, bool active, int repMode, REAL cx, REAL cy, REAL sc, BYTE a, float hovT) {
    BYTE iconA = (BYTE)std::min(255.0f, a * (active ? 1.0f : (id == 3 ? 0.95f : 0.62f + 0.38f * hovT)));
    Color col(iconA, 255, 255, 255);
    SolidBrush br(col);
    Pen pen(col, 1.5f * sc);
    pen.SetStartCap(LineCapRound); pen.SetEndCap(LineCapRound); pen.SetLineJoin(LineJoinRound);
    GraphicsPath p;

    if (id >= 0 && hovT > 0.02f) {
        SolidBrush hGlow(Color((BYTE)(28 * hovT * (a / 255.0f)), 255, 255, 255));
        REAL hr = (id == 3 ? 15.0f : 12.5f) * sc;
        g.FillEllipse(&hGlow, cx - hr, cy - hr, hr * 2.0f, hr * 2.0f);
    }
    if (active && (id == 1 || id == 5 || id == 6)) {
        SolidBrush actBg(Color((BYTE)(55 * (a / 255.0f)), 255, 255, 255));
        g.FillEllipse(&actBg, cx - 12.0f * sc, cy - 12.0f * sc, 24.0f * sc, 24.0f * sc);
    }

    if (id == 0) {
        g.DrawLine(&pen, cx - 4.5f * sc, cy - 4.5f * sc, cx - 1.2f * sc, cy - 1.2f * sc);
        g.DrawLine(&pen, cx - 4.5f * sc, cy - 4.5f * sc, cx - 1.5f * sc, cy - 4.5f * sc);
        g.DrawLine(&pen, cx - 4.5f * sc, cy - 4.5f * sc, cx - 4.5f * sc, cy - 1.5f * sc);
        g.DrawLine(&pen, cx + 4.5f * sc, cy + 4.5f * sc, cx + 1.2f * sc, cy + 1.2f * sc);
        g.DrawLine(&pen, cx + 4.5f * sc, cy + 4.5f * sc, cx + 1.5f * sc, cy + 4.5f * sc);
        g.DrawLine(&pen, cx + 4.5f * sc, cy + 4.5f * sc, cx + 4.5f * sc, cy + 1.5f * sc);
    } else if (id == 1) {
        g.DrawLine(&pen, cx - 5.2f * sc, cy - 3.2f * sc, cx + 4.2f * sc, cy + 3.2f * sc);
        g.DrawLine(&pen, cx - 5.2f * sc, cy + 3.2f * sc, cx + 4.2f * sc, cy - 3.2f * sc);
        PointF a1[3] = {{cx + 5.5f * sc, cy - 3.2f * sc}, {cx + 2.8f * sc, cy - 5.0f * sc}, {cx + 2.8f * sc, cy - 1.4f * sc}};
        PointF a2[3] = {{cx + 5.5f * sc, cy + 3.2f * sc}, {cx + 2.8f * sc, cy + 1.4f * sc}, {cx + 2.8f * sc, cy + 5.0f * sc}};
        p.AddPolygon(a1, 3); p.AddPolygon(a2, 3); g.FillPath(&br, &p);
    } else if (id == 2) {
        PointF t1[3] = {{cx - 6.2f * sc, cy}, {cx - 0.2f * sc, cy - 4.4f * sc}, {cx - 0.2f * sc, cy + 4.4f * sc}};
        PointF t2[3] = {{cx - 0.4f * sc, cy}, {cx + 5.6f * sc, cy - 4.4f * sc}, {cx + 5.6f * sc, cy + 4.4f * sc}};
        p.AddPolygon(t1, 3); p.AddPolygon(t2, 3); g.FillPath(&br, &p);
    } else if (id == 3) {
        if (playing) {
            GetRoundRect(p, cx - 4.8f * sc, cy - 5.8f * sc, 3.2f * sc, 11.6f * sc, 1.3f * sc); g.FillPath(&br, &p);
            GetRoundRect(p, cx + 1.6f * sc, cy - 5.8f * sc, 3.2f * sc, 11.6f * sc, 1.3f * sc); g.FillPath(&br, &p);
        } else {
            PointF pts[3] = {{cx - 3.8f * sc, cy - 6.0f * sc}, {cx + 6.2f * sc, cy}, {cx - 3.8f * sc, cy + 6.0f * sc}};
            p.AddPolygon(pts, 3); g.FillPath(&br, &p);
        }
    } else if (id == 4) {
        PointF t1[3] = {{cx + 6.2f * sc, cy}, {cx + 0.2f * sc, cy - 4.4f * sc}, {cx + 0.2f * sc, cy + 4.4f * sc}};
        PointF t2[3] = {{cx + 0.4f * sc, cy}, {cx - 5.6f * sc, cy - 4.4f * sc}, {cx - 5.6f * sc, cy + 4.4f * sc}};
        p.AddPolygon(t1, 3); p.AddPolygon(t2, 3); g.FillPath(&br, &p);
    } else if (id == 5) {
        GetRoundRect(p, cx - 5.2f * sc, cy - 3.4f * sc, 10.4f * sc, 6.8f * sc, 3.2f * sc);
        g.DrawPath(&pen, &p);
        if (repMode == 2) g.FillEllipse(&br, cx - 1.4f * sc, cy - 1.4f * sc, 2.8f * sc, 2.8f * sc);
    } else if (id == 6) {
        GetRoundRect(p, cx - 5.5f * sc, cy - 4.6f * sc, 11.0f * sc, 8.2f * sc, 2.6f * sc);
        g.DrawPath(&pen, &p);
        g.FillRectangle(&br, cx - 2.6f * sc, cy - 1.8f * sc, 1.6f * sc, 2.8f * sc);
        g.FillRectangle(&br, cx + 1.0f * sc, cy - 1.8f * sc, 1.6f * sc, 2.8f * sc);
    } else if (id == 7) {
        g.DrawLine(&pen, cx - 4.5f * sc, cy - 4.5f * sc, cx + 4.5f * sc, cy + 4.5f * sc);
        g.DrawLine(&pen, cx + 4.5f * sc, cy - 4.5f * sc, cx - 4.5f * sc, cy + 4.5f * sc);
    }
}

void DrawSeekBarSection(Graphics& g, REAL bx, REAL by, REAL bw, double pos, double dur, float ratio, BYTE a) {
    REAL bh = 3.8f + 1.8f * g_ui.seekHoverT;
    GraphicsPath tp; GetRoundRect(tp, bx, by - bh * 0.5f, bw, bh, bh * 0.5f);
    SolidBrush tbg(Color((BYTE)(55 * (a / 255.0f)), 255, 255, 255));
    g.FillPath(&tbg, &tp);

    float pr = std::clamp(ratio, 0.0f, 1.0f);
    if (pr > 0.002f) {
        GraphicsPath fp; GetRoundRect(fp, bx, by - bh * 0.5f, std::max(bh, bw * pr), bh, bh * 0.5f);
        SolidBrush fbr(Color((BYTE)(225 * (a / 255.0f)), 255, 255, 255));
        g.FillPath(&fbr, &fp);
    }
    if (g_ui.seekHoverT > 0.02f) {
        REAL dr = 4.8f * g_ui.seekHoverT, dx = bx + bw * pr;
        SolidBrush dbr(Color((BYTE)(250 * (a / 255.0f)), 255, 255, 255));
        g.FillEllipse(&dbr, dx - dr, by - dr, dr * 2.0f, dr * 2.0f);
    }

    SolidBrush mBr(Color((BYTE)(155 * (a / 255.0f)), 255, 255, 255));
    wchar_t pStr[16], rStr[16];
    FormatMMSS(pos, pStr, 16, false);
    FormatMMSS(std::max(0.0, dur - pos), rStr, 16, true);

    StringFormat rFmt; rFmt.SetAlignment(StringAlignmentFar);
    g.DrawString(pStr, -1, g_res.timeFont.get(), RectF(bx, by + 6.0f, 48.0f, 14.0f), nullptr, &mBr);
    g.DrawString(rStr, -1, g_res.timeFont.get(), RectF(bx + bw - 48.0f, by + 6.0f, 48.0f, 14.0f), &rFmt, &mBr);
}

REAL GetStudioPlayerColX(REAL px, REAL pw, REAL colW) {
    REAL leftX   = px + 56.0f;
    REAL centerX = px + (pw - colW) * 0.5f;
    return centerX + (leftX - centerX) * g_ui.bigLyricsT;
}

RECT GetTargetWorkArea(POINT anchorPt) {
    HMONITOR hMon = MonitorFromPoint(anchorPt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(MONITORINFO);
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        return mi.rcWork;
    }
    RECT rc{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0);
    return rc;
}

void RenderIslandFrame(HWND hwnd, float dt) {
    if (g_ui.hidden || g_ui.fullscreenHidden) return;

    UINT dpi = GetDpiForWindow(hwnd);
    float scale = (dpi > 0) ? ((float)dpi / 96.0f) : 1.0f;
    g_ui.dpiScale = scale;
    g_ui.marqueeActive = false;

    float logWinW = std::ceil(g_ui.curW) + 8.0f;
    float logWinH = std::ceil(g_ui.curH) + 8.0f;
    int winW = std::max(1, (int)std::ceil(logWinW * scale));
    int winH = std::max(1, (int)std::ceil(logWinH * scale));

    RECT primaryWork{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &primaryWork, 0);
    int workW = primaryWork.right - primaryWork.left;
    int workH = primaryWork.bottom - primaryWork.top;

    float stdX = primaryWork.left + (workW - winW) * 0.5f;
    float stdY = primaryWork.top + (float)g_set.topOffset * scale;

    int bigPhysW = (int)std::ceil((g_set.bigW + 8.0f) * scale);
    int bigPhysH = (int)std::ceil((g_set.bigH + 8.0f) * scale);
    float defBigX = primaryWork.left + (workW - bigPhysW) * 0.5f;
    float defBigY = primaryWork.top + std::max(32.0f * scale, (workH - bigPhysH) * 0.30f);

    if (g_ui.hasSavedBigPos && !g_ui.draggingWin) {
        POINT pt{(LONG)g_ui.savedBigX, (LONG)g_ui.savedBigY};
        RECT monWork = GetTargetWorkArea(pt);
        float minX = (float)monWork.left;
        float maxX = (float)std::max((LONG)minX, monWork.right - winW);
        float minY = (float)monWork.top;
        float maxY = (float)std::max((LONG)minY, monWork.bottom - winH);
        g_ui.savedBigX = std::clamp(g_ui.savedBigX, minX, maxX);
        g_ui.savedBigY = std::clamp(g_ui.savedBigY, minY, maxY);
    }

    float targBigX = g_ui.hasSavedBigPos ? g_ui.savedBigX : defBigX;
    float targBigY = g_ui.hasSavedBigPos ? g_ui.savedBigY : defBigY;

    int posX = (int)std::lround(stdX + (targBigX - stdX) * g_ui.bigPosT);
    int posY = (int)std::lround(stdY + (targBigY - stdY) * g_ui.bigPosT);
    g_ui.curWinX = posX;
    g_ui.curWinY = posY;

    REAL px = 4.0f, py = 4.0f, pw = g_ui.curW, ph = g_ui.curH;
    REAL rad = 17.0f + 5.0f * g_ui.expandT;

    HDC hdcScr = GetDC(nullptr);
    g_res.EnsureSurface(hdcScr, winW, winH);
    g_res.EnsureFonts();

    if (g_res.memDC && g_res.dib) {
        Graphics g(g_res.memDC);
        g.Clear(Color(0, 0, 0, 0));
        g.ScaleTransform(scale, scale);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

        std::lock_guard<std::mutex> lk(g_mediaMutex);
        g_ui.lyricHits.clear();

        if (g_media.trackSeq != g_ui.seenSeq) {
            if (g_ui.seenSeq != 0) {
                g_ui.trackAnimT = 0.0f; g_ui.bounceW = 12.0f;
                g_ui.marqueeTime = 0.0f; g_ui.smoothLyricIdx = 0.0f; g_ui.lastActiveLyric = -1;
            }
            g_ui.seenSeq = g_media.trackSeq;
        }
        float inv = 1.0f - std::clamp(g_ui.trackAnimT, 0.0f, 1.0f);
        float easeT = 1.0f - inv * inv * inv;

        Color acc = LerpCol(g_media.prevAccent, g_media.accent, easeT);
        double dur = g_media.durSec, pos = g_media.posSec;
        ULONGLONG nowMs = GetTickCount64();
        if (g_media.isPlaying && dur > 0.0 && g_media.posTickMs > 0)
            pos = std::clamp(pos + (nowMs - g_media.posTickMs) / 1000.0, 0.0, dur);

        float targR = 0.0f;
        if (g_ui.draggingSeek) { targR = g_ui.dragSeekRatio; pos = dur * targR; }
        else if (nowMs < g_ui.holdSeekUntil && g_ui.holdSeekRatio >= 0.0f) { targR = g_ui.holdSeekRatio; pos = dur * targR; }
        else if (dur > 0.1) targR = (float)std::clamp(pos / dur, 0.0, 1.0);

        float skRate = g_ui.draggingSeek ? std::min(1.f, dt * 45.f) : std::min(1.f, dt * 20.f);
        g_ui.dispRatio = (std::fabs(targR - g_ui.dispRatio) > 0.35f && !g_ui.draggingSeek)
            ? targR : (g_ui.dispRatio + (targR - g_ui.dispRatio) * skRate);

        int activeLrc = 0;
        float activeLineProg = 0.0f;
        if (!g_media.lyrics.empty()) {
            for (int i = 0; i < (int)g_media.lyrics.size(); ++i) {
                if (g_media.lyrics[i].timeSec <= pos + 0.18) activeLrc = i; else break;
            }
            if (g_ui.lastActiveLyric < 0) g_ui.smoothLyricIdx = (float)activeLrc;
            g_ui.lastActiveLyric = activeLrc;
            float lrcRate = 1.0f - std::exp(-11.0f * dt);
            g_ui.smoothLyricIdx += ((float)activeLrc - g_ui.smoothLyricIdx) * lrcRate;

            double t0 = g_media.lyrics[activeLrc].timeSec;
            double t1 = (activeLrc + 1 < (int)g_media.lyrics.size())
                ? g_media.lyrics[activeLrc + 1].timeSec
                : (dur > t0 ? dur : t0 + 4.0);
            double naturalSpan = std::max(1.2, g_media.lyrics[activeLrc].text.size() * 0.082 + 0.55);
            double lineSpan    = std::max(0.35, std::min(t1 - t0 - 0.10, naturalSpan));
            activeLineProg     = (float)std::clamp((pos + 0.12 - t0) / lineSpan, 0.0, 1.0);
        }

        GraphicsPath pillPath; GetRoundRect(pillPath, px, py, pw, ph, rad);
        SolidBrush baseBr(Color((BYTE)g_set.glassOpacity, 24, 21, 26));
        g.FillPath(&baseBr, &pillPath);

        g.SetClip(&pillPath);
        auto drawMesh = [&](Bitmap* b, float op) {
            if (!b || op <= 0.01f) return;
            g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            ImageAttributes ma; ma.SetWrapMode(WrapModeTileFlipXY);
            ColorMatrix cm = {{{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,0.88f*op,0},{0,0,0,0,1}}};
            ma.SetColorMatrix(&cm, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
            g.DrawImage(b, RectF(px - 8, py - 8, pw + 16, ph + 16), 0, 0, 96, 96, UnitPixel, &ma);
        };
        if (easeT < 0.99f) drawMesh(g_media.prevBlurBg.get(), 1.0f - easeT);
        drawMesh(g_media.blurBg.get(), easeT);
        g.ResetClip();

        Pen rimPen(Color(42, 255, 255, 255), 1.0f);
        g.DrawPath(&rimPen, &pillPath);

        if (g_ui.expandT < 0.45f) {
            float mA = 1.0f - (g_ui.expandT / 0.45f);
            BYTE a = (BYTE)(mA * 255.0f);
            REAL iy = py + (ph - 20.0f) * 0.5f;
            if (easeT < 0.99f && g_media.prevMiniArt) {
                DrawBmpAlpha(g, g_media.prevMiniArt.get(), px + 8.0f, iy, 20.0f, 20.0f, mA * (1.0f - easeT));
            }
            if (g_media.miniArt) {
                DrawBmpAlpha(g, g_media.miniArt.get(), px + 8.0f, iy, 20.0f, 20.0f, mA * easeT);
            }
            SolidBrush wBr(Color((BYTE)(a * easeT), 248, 250, 252));
            DrawMarqueeString(g, g_media.title, g_res.subFont.get(), RectF(px + 34.0f, py + 7.0f * (1.0f - easeT), pw - 64.0f, ph), &wBr);
            DrawVisualizer(g, px + pw - 11.0f, py + ph * 0.5f, 10.0f, a, acc);
        }

        float normVis = g_ui.expandT * (1.0f - g_ui.bigModeT);
        if (normVis > 0.05f && g_ui.bigModeT < 0.65f) {
            float mA = std::clamp((normVis - 0.05f) / 0.95f, 0.0f, 1.0f);
            BYTE a = (BYTE)(mA * 255.0f);

            REAL artSz = 48.0f, artX = px + 16.0f, artY = py + 14.0f;
            if (g_media.expArt || g_media.prevExpArt) {
                if (easeT < 0.99f && g_media.prevExpArt) {
                    REAL s = artSz * (1.0f - 0.20f * easeT), o = (artSz - s) * 0.5f;
                    DrawBmpAlpha(g, g_media.prevExpArt.get(), artX + o, artY + o, s, s, mA * (1.0f - easeT));
                }
                if (g_media.expArt) {
                    REAL s = artSz * (0.78f + 0.22f * easeT), o = (artSz - s) * 0.5f;
                    DrawBmpAlpha(g, g_media.expArt.get(), artX + o, artY + o, s, s, mA * easeT);
                }
            }

            REAL infoX = artX + artSz + 12.0f, infoW = pw - (infoX - px) - 42.0f;
            if (easeT < 0.99f && !g_media.prevTitle.empty()) {
                BYTE oa = (BYTE)(a * (1.0f - easeT));
                SolidBrush ow(Color(oa, 255, 255, 255)), om(Color((BYTE)(oa * 0.65f), 255, 255, 255));
                REAL sy = -8.0f * easeT;
                DrawMarqueeString(g, g_media.prevTitle, g_res.titleFont.get(), RectF(infoX, py + 19.0f + sy, infoW, 18.0f), &ow);
                DrawMarqueeString(g, g_media.prevSubtitle, g_res.subFont.get(), RectF(infoX, py + 38.0f + sy, infoW, 17.0f), &om);
            }
            {
                BYTE na = (BYTE)(a * easeT);
                SolidBrush nw(Color(na, 255, 255, 255)), nm(Color((BYTE)(na * 0.65f), 255, 255, 255));
                REAL sy = 8.0f * (1.0f - easeT);
                DrawMarqueeString(g, g_media.title, g_res.titleFont.get(), RectF(infoX, py + 19.0f + sy, infoW, 18.0f), &nw);
                DrawMarqueeString(g, g_media.subtitle, g_res.subFont.get(), RectF(infoX, py + 38.0f + sy, infoW, 17.0f), &nm);
            }
            DrawVisualizer(g, px + pw - 16.0f, py + 38.0f, 11.0f, a, acc);

            DrawSeekBarSection(g, px + 16.0f, py + 74.0f, pw - 32.0f, pos, dur, g_ui.dispRatio, a);

            REAL btnY = py + 109.0f, cx = px + pw * 0.5f;
            REAL xs[7] = {px + 26.0f, cx - 72.0f, cx - 36.0f, cx, cx + 36.0f, cx + 72.0f, px + pw - 26.0f};
            for (int i = 0; i < 7; ++i) {
                bool act = (i == 1 && g_media.isShuffle) || (i == 5 && g_media.repeatMode > 0) || (i == 6 && g_ui.showLyrics);
                DrawAppleIcon(g, i, g_media.isPlaying, act, g_media.repeatMode, xs[i], btnY, 1.0f, a, g_ui.btnHoverT[i]);
            }

            if (g_ui.drawerT > 0.02f) {
                float dA = mA * g_ui.drawerT;
                BYTE da = (BYTE)(dA * 255.0f);
                REAL dTop = py + (REAL)g_set.expandedH;
                REAL dH   = ph - (REAL)g_set.expandedH - 6.0f;

                Pen divPen(Color((BYTE)(28 * dA), 255, 255, 255), 1.0f);
                g.DrawLine(&divPen, px + 18.0f, dTop - 2.0f, px + pw - 18.0f, dTop - 2.0f);

                GraphicsState st = g.Save();
                g.SetClip(RectF(px + 12.0f, dTop, pw - 24.0f, std::max(1.0f, dH)));

                if (g_media.lyrics.empty()) {
                    StringFormat cFmt; cFmt.SetAlignment(StringAlignmentCenter); cFmt.SetLineAlignment(StringAlignmentCenter);
                    SolidBrush stBr(Color((BYTE)(da * 0.65f), 255, 255, 255));
                    g.DrawString(g_media.lyricStatus.c_str(), -1, g_res.drawerNextFont.get(), RectF(px + 16.0f, dTop, pw - 32.0f, dH), &cFmt, &stBr);
                } else {
                    float slideOffset = std::clamp((float)activeLrc - g_ui.smoothLyricIdx, -1.0f, 1.0f) * 28.0f;
                    REAL curH = DrawKaraokeLyricBlock(g, g_media.lyrics[activeLrc].text, g_res.drawerActFont.get(),
                                                      RectF(px + 16.0f, 0, pw - 32.0f, 100.0f),
                                                      0.0f, false, 255, true, true);
                    REAL curY = dTop + 6.0f + slideOffset;
                    RectF curRect(px + 16.0f, curY, pw - 32.0f, curH);
                    DrawKaraokeLyricBlock(g, g_media.lyrics[activeLrc].text, g_res.drawerActFont.get(), curRect,
                                          activeLineProg, true, da, true, false);

                    if (activeLrc + 1 < (int)g_media.lyrics.size()) {
                        int nextIdx = activeLrc + 1;
                        REAL nextH = DrawKaraokeLyricBlock(g, g_media.lyrics[nextIdx].text, g_res.drawerNextFont.get(),
                                                           RectF(px + 16.0f, 0, pw - 32.0f, 100.0f),
                                                           0.0f, false, 255, true, true);
                        REAL nextY = curY + curH + 6.0f;
                        RectF nextRect(px + 16.0f, nextY, pw - 32.0f, nextH);

                        bool isNextHov = (g_ui.hoveredLyricIdx == nextIdx);
                        if (isNextHov) {
                            GraphicsPath hp;
                            GetRoundRect(hp, px + 14.0f, nextY - 2.0f, pw - 28.0f, nextH + 4.0f, 6.0f);
                            SolidBrush hBr(Color((BYTE)(26 * dA), 255, 255, 255));
                            g.FillPath(&hBr, &hp);
                        }
                        BYTE nextAlpha = (BYTE)(da * (isNextHov ? 0.88f : 0.45f));
                        DrawKaraokeLyricBlock(g, g_media.lyrics[nextIdx].text, g_res.drawerNextFont.get(), nextRect,
                                              0.0f, false, nextAlpha, true, false);
                        g_ui.lyricHits.push_back({nextIdx, RectF(px + 12.0f, nextY - 3.0f, pw - 24.0f, nextH + 6.0f)});
                    }
                }
                g.Restore(st);
            }
        }

        if (g_ui.bigModeT > 0.35f) {
            float mA = std::clamp((g_ui.bigModeT - 0.35f) / 0.65f, 0.0f, 1.0f);
            BYTE a = (BYTE)(mA * 255.0f);

            DrawAppleIcon(g, 7, false, false, 0, px + pw - 24.0f, py + 22.0f, 0.95f, a, g_ui.btnHoverT[7]);

            REAL colW = 230.0f, artSz = 230.0f;
            REAL colX = GetStudioPlayerColX(px, pw, colW);
            REAL artY = py + 42.0f;

            for (int s = 3; s >= 1; --s) {
                REAL sp = (REAL)s * 3.0f;
                GraphicsPath spPath; GetRoundRect(spPath, colX - sp * 0.5f, artY + sp * 0.7f, artSz + sp, artSz + sp, 15.0f);
                SolidBrush shBr(Color((BYTE)(16 * mA), 0, 0, 0));
                g.FillPath(&shBr, &spPath);
            }
            if (g_media.bigArt || g_media.prevBigArt) {
                if (easeT < 0.99f && g_media.prevBigArt) {
                    REAL s = artSz * (1.0f - 0.12f * easeT), o = (artSz - s) * 0.5f;
                    DrawBmpAlpha(g, g_media.prevBigArt.get(), colX + o, artY + o, s, s, mA * (1.0f - easeT));
                }
                if (g_media.bigArt) {
                    REAL s = artSz * (0.88f + 0.12f * easeT), o = (artSz - s) * 0.5f;
                    DrawBmpAlpha(g, g_media.bigArt.get(), colX + o, artY + o, s, s, mA * easeT);
                }
            } else {
                GraphicsPath fbPath; GetRoundRect(fbPath, colX, artY, artSz, artSz, 14.0f);
                SolidBrush fbBr(Color((BYTE)(60 * mA), acc.GetR(), acc.GetG(), acc.GetB()));
                g.FillPath(&fbBr, &fbPath);
            }

            SolidBrush nw(Color((BYTE)(a * easeT), 255, 255, 255)), nm(Color((BYTE)(a * easeT * 0.65f), 255, 255, 255));
            REAL sy = 6.0f * (1.0f - easeT);
            DrawMarqueeString(g, g_media.title, g_res.bigTitleFont.get(), RectF(colX, py + 286.0f + sy, colW, 22.0f), &nw);
            DrawMarqueeString(g, g_media.subtitle, g_res.bigSubFont.get(), RectF(colX, py + 308.0f + sy, colW, 19.0f), &nm);

            DrawSeekBarSection(g, colX, py + 342.0f, colW, pos, dur, g_ui.dispRatio, a);

            REAL bCy = py + 386.0f, bCx = colX + colW * 0.5f;
            REAL bXs[5] = {bCx - 92.0f, bCx - 46.0f, bCx, bCx + 46.0f, bCx + 92.0f};
            for (int i = 1; i <= 5; ++i) {
                bool act = (i == 1 && g_media.isShuffle) || (i == 5 && g_media.repeatMode > 0);
                DrawAppleIcon(g, i, g_media.isPlaying, act, g_media.repeatMode, bXs[i - 1], bCy, 1.05f, a, g_ui.btnHoverT[i]);
            }

            DrawAppleIcon(g, 6, false, g_ui.showLyrics, 0, px + 26.0f, py + ph - 24.0f, 1.0f, a, g_ui.btnHoverT[6]);

            if (g_ui.bigLyricsT > 0.03f && pw > 520.0f) {
                float lrcVisA = mA * g_ui.bigLyricsT;
                BYTE laBase = (BYTE)(lrcVisA * 255.0f);
                REAL lrcX = px + 336.0f, lrcW = std::max(280.0f, pw - 376.0f), lrcTop = py + 42.0f, lrcH = ph - 76.0f;

                GraphicsState st = g.Save();
                g.SetClip(RectF(lrcX - 6.0f, lrcTop, lrcW + 12.0f, lrcH));

                if (g_media.lyrics.empty()) {
                    SolidBrush stBr(Color((BYTE)(laBase * 0.50f), 255, 255, 255));
                    g.DrawString(g_media.lyricStatus.c_str(), -1, g_res.bigLrcFont.get(), RectF(lrcX, lrcTop + lrcH * 0.42f, lrcW, 40.0f), nullptr, &stBr);
                } else {
                    int nLines = (int)g_media.lyrics.size();
                    if (g_res.cachedLyricSeq != g_media.trackSeq ||
                        g_res.cachedLyricCount != g_media.lyrics.size() ||
                        std::fabs(g_res.cachedLyricWidth - lrcW) > 1.0f) {
                        g_res.cachedLyricSeq   = g_media.trackSeq;
                        g_res.cachedLyricCount = g_media.lyrics.size();
                        g_res.cachedLyricWidth = lrcW;
                        g_res.cachedLineH.assign(nLines, 36.0f);
                        g_res.cachedLineY.assign(nLines, 0.0f);
                        for (int i = 0; i < nLines; ++i) {
                            REAL h = DrawKaraokeLyricBlock(g, g_media.lyrics[i].text, g_res.bigLrcFont.get(),
                                                           RectF(lrcX, 0, lrcW, 400.0f), 0.0f, false, 255, false, true);
                            g_res.cachedLineH[i] = h + 22.0f;
                            if (i > 0) g_res.cachedLineY[i] = g_res.cachedLineY[i - 1] + g_res.cachedLineH[i - 1];
                        }
                    }

                    int idx0 = std::clamp((int)std::floor(g_ui.smoothLyricIdx), 0, nLines - 1);
                    int idx1 = std::clamp(idx0 + 1, 0, nLines - 1);
                    float frac = g_ui.smoothLyricIdx - (float)idx0;
                    float focusOffset = g_res.cachedLineY[idx0] + (g_res.cachedLineY[idx1] - g_res.cachedLineY[idx0]) * frac;
                    REAL anchorY = lrcTop + lrcH * 0.34f;

                    for (int i = std::max(0, activeLrc - 5); i <= std::min(nLines - 1, activeLrc + 7); ++i) {
                        REAL drawY = anchorY + (g_res.cachedLineY[i] - focusOffset);
                        REAL lh    = g_res.cachedLineH[i];
                        if (drawY + lh < lrcTop - 10.0f || drawY > lrcTop + lrcH + 10.0f) continue;

                        float dist = std::fabs((float)i - g_ui.smoothLyricIdx);
                        float edgeFade = 1.0f;
                        if (drawY < lrcTop + 32.0f) edgeFade = std::clamp((drawY - lrcTop + 10.0f) / 42.0f, 0.0f, 1.0f);
                        else if (drawY > lrcTop + lrcH - 54.0f) edgeFade = std::clamp((lrcTop + lrcH - drawY) / 54.0f, 0.0f, 1.0f);

                        bool isAct = (i == activeLrc);
                        float baseOp = isAct ? 1.0f : std::max(0.18f, 0.48f - dist * 0.09f);
                        if (g_ui.hoveredLyricIdx == i && !isAct) baseOp = std::min(0.85f, baseOp + 0.25f);

                        BYTE lineAlpha = (BYTE)(laBase * baseOp * edgeFade);
                        RectF rLine(lrcX, drawY, lrcW, lh - 18.0f);
                        DrawKaraokeLyricBlock(g, g_media.lyrics[i].text, g_res.bigLrcFont.get(), rLine,
                                              isAct ? activeLineProg : 0.0f, isAct, lineAlpha, false, false);
                        g_ui.lyricHits.push_back({i, rLine});
                    }
                }
                g.Restore(st);
            }
        }

        POINT ptSrc{0, 0}, ptDst{posX, posY};
        SIZE sz{winW, winH};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, hdcScr, &ptDst, &sz, g_res.memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    }

    ReleaseDC(nullptr, hdcScr);
}

float GetSeekRatio(float mx) {
    float px = 4.0f, pw = g_ui.curW;
    float bx = g_ui.bigMode ? GetStudioPlayerColX(px, pw, 230.0f) : (px + 16.0f);
    float bw = g_ui.bigMode ? 230.0f : (pw - 32.0f);
    return (bw <= 1.0f) ? 0.0f : std::clamp((mx - bx) / bw, 0.0f, 1.0f);
}

LRESULT CALLBACK IslandWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            LARGE_INTEGER f{}, n{};
            QueryPerformanceFrequency(&f);
            QueryPerformanceCounter(&n);
            g_ui.qpcFreq = (double)f.QuadPart;
            g_ui.lastQpc = n.QuadPart;
            SetTimer(hwnd, TIMER_ID_FRAME, 7, nullptr);
            return 0;
        }

        case WM_TIMER:
            if (wParam == TIMER_ID_FRAME) {
                bool f12Down = (GetAsyncKeyState(VK_F12) & 0x8000) != 0;
                if (f12Down && !g_ui.wasF12Down) {
                    g_ui.hidden = !g_ui.hidden;
                    ShowWindow(hwnd, (g_ui.hidden || g_ui.fullscreenHidden) ? SW_HIDE : SW_SHOWNOACTIVATE);
                }
                g_ui.wasF12Down = f12Down;
                if (g_ui.hidden) return 0;

                LARGE_INTEGER n{};
                QueryPerformanceCounter(&n);
                float dt = std::clamp((float)((n.QuadPart - g_ui.lastQpc) / g_ui.qpcFreq), 0.001f, 0.050f);
                g_ui.lastQpc = n.QuadPart;
                g_ui.marqueeTime += dt;

                bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
                if (ctrl && !(ex & WS_EX_TRANSPARENT)) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_TRANSPARENT);
                else if (!ctrl && (ex & WS_EX_TRANSPARENT)) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_TRANSPARENT);

                if (g_ui.trackAnimT < 1.0f) g_ui.trackAnimT = std::min(1.0f, g_ui.trackAnimT + dt * 2.4f);
                g_ui.bounceW *= std::exp(-10.0f * dt);
                if (g_ui.bounceW < 0.05f) g_ui.bounceW = 0.0f;

                bool exp = g_ui.hovered || g_ui.draggingSeek || g_ui.draggingWin || g_ui.bigMode;
                float mf = 1.0f - std::exp(-16.0f * dt);

                float targDrawer    = (g_ui.showLyrics && !g_ui.bigMode && exp) ? 1.0f : 0.0f;
                float targBigMode   = g_ui.bigMode ? 1.0f : 0.0f;
                float targBigLyrics = g_ui.showLyrics ? 1.0f : 0.0f;
                float targExpand    = exp ? 1.0f : 0.0f;

                g_ui.drawerT    += (targDrawer - g_ui.drawerT) * mf;
                g_ui.bigModeT   += (targBigMode - g_ui.bigModeT) * mf;
                g_ui.bigLyricsT += (targBigLyrics - g_ui.bigLyricsT) * mf;

                float normW = (float)g_set.expandedW;
                float normH = (float)g_set.expandedH + 86.0f * g_ui.drawerT;
                float expW  = normW + ((float)g_set.bigW - normW) * g_ui.bigModeT;
                float expH  = normH + ((float)g_set.bigH - normH) * g_ui.bigModeT;

                float tw = (exp ? expW : (float)g_set.compactW) + g_ui.bounceW;
                float th = exp ? expH : (float)g_set.compactH;

                g_ui.curW    += (tw - g_ui.curW) * mf;
                g_ui.curH    += (th - g_ui.curH) * mf;
                g_ui.expandT += (targExpand - g_ui.expandT) * mf;
                g_ui.bigPosT  = g_ui.draggingWin ? 1.0f : (g_ui.bigPosT + (targBigMode - g_ui.bigPosT) * mf);

                float uf = 1.0f - std::exp(-24.0f * dt);
                bool anyHoverAnim = false;
                for (int i = 0; i < 8; ++i) {
                    float tb = (g_ui.hoveredBtn == i && !g_ui.draggingSeek) ? 1.f : 0.f;
                    g_ui.btnHoverT[i] += (tb - g_ui.btnHoverT[i]) * uf;
                    if (std::fabs(tb - g_ui.btnHoverT[i]) > 0.01f) anyHoverAnim = true;
                }
                float tSeek = (g_ui.hoveringSeek || g_ui.draggingSeek) ? 1.f : 0.f;
                g_ui.seekHoverT += (tSeek - g_ui.seekHoverT) * uf;
                if (std::fabs(tSeek - g_ui.seekHoverT) > 0.01f) anyHoverAnim = true;

                float af = 1.0f - std::exp(-42.0f * dt), df = 1.0f - std::exp(-16.0f * dt);
                bool anyAudioAnim = false;
                for (int i = 0; i < 5; ++i) {
                    float tb = g_audioBands[i].load();
                    g_ui.smoothBands[i] += (tb - g_ui.smoothBands[i]) * (tb > g_ui.smoothBands[i] ? af : df);
                    if (g_ui.smoothBands[i] > 0.01f || tb > 0.01f) anyAudioAnim = true;
                }

                UpdateAudioCaptureNeeded();
                RenderIslandFrame(hwnd, dt);

                bool isPlaying = false;
                bool hasLyrics = false;
                {
                    std::lock_guard<std::mutex> lk(g_mediaMutex);
                    isPlaying = g_media.isPlaying;
                    hasLyrics = !g_media.lyrics.empty();
                }

                bool isMorphing = std::fabs(tw - g_ui.curW) > 0.4f ||
                                  std::fabs(th - g_ui.curH) > 0.4f ||
                                  std::fabs(targExpand - g_ui.expandT) > 0.01f ||
                                  std::fabs(targBigMode - g_ui.bigModeT) > 0.01f ||
                                  std::fabs(targDrawer - g_ui.drawerT) > 0.01f ||
                                  g_ui.trackAnimT < 0.99f;

                bool lyricsAnimating = isPlaying && hasLyrics && exp && (g_ui.showLyrics || g_ui.bigMode);
                bool needHighFps = isMorphing || anyHoverAnim || anyAudioAnim ||
                                   g_ui.marqueeActive || g_ui.draggingSeek || g_ui.draggingWin || lyricsAnimating;

                if (needHighFps) {
                    SetTimerIntervalIfNeeded(hwnd, TIMER_INTERVAL_ANIM);
                } else if (isPlaying && exp) {
                    SetTimerIntervalIfNeeded(hwnd, TIMER_INTERVAL_IDLE_PLAYING);
                } else {
                    SetTimerIntervalIfNeeded(hwnd, g_set.autoHideFullscreen ? 500 : 0);
                }
            }
            return 0;

        case WM_LBUTTONDOWN: {
            WakeAnimationTimer(hwnd);
            float scale = std::max(0.5f, g_ui.dpiScale);
            float mx = (float)GET_X_LPARAM(lParam) / scale;
            if (g_ui.expandT > 0.8f && g_ui.hoveringSeek) {
                g_ui.draggingSeek = true;
                g_ui.dragSeekRatio = g_ui.dispRatio = GetSeekRatio(mx);
                SetCapture(hwnd);
            } else if (g_ui.bigMode && g_ui.hoveredBtn < 0 && g_ui.hoveredLyricIdx < 0) {
                g_ui.draggingWin = true;
                g_ui.hasSavedBigPos = true;
                GetCursorPos(&g_ui.dragStartMouse);
                g_ui.dragStartWin = {g_ui.curWinX, g_ui.curWinY};
                g_ui.savedBigX = (float)g_ui.curWinX;
                g_ui.savedBigY = (float)g_ui.curWinY;
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!g_ui.hovered) {
                g_ui.hovered = true;
                TRACKMOUSEEVENT tme{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
            }
            WakeAnimationTimer(hwnd);

            if (g_ui.draggingWin) {
                POINT pt{}; GetCursorPos(&pt);
                g_ui.savedBigX = (float)(g_ui.dragStartWin.x + (pt.x - g_ui.dragStartMouse.x));
                g_ui.savedBigY = (float)(g_ui.dragStartWin.y + (pt.y - g_ui.dragStartMouse.y));
                return 0;
            }

            float scale = std::max(0.5f, g_ui.dpiScale);
            float mx = (float)GET_X_LPARAM(lParam) / scale;
            float my = (float)GET_Y_LPARAM(lParam) / scale;
            if (g_ui.draggingSeek) { g_ui.dragSeekRatio = GetSeekRatio(mx); return 0; }

            g_ui.hoveredBtn = -1; g_ui.hoveredLyricIdx = -1; g_ui.hoveringSeek = false;
            if (g_ui.expandT > 0.8f) {
                float px = 4.0f, py = 4.0f, pw = g_ui.curW, ph = g_ui.curH;
                if (!g_ui.bigMode) {
                    float bx = px + 16.0f, bw = pw - 32.0f, by = py + 74.0f;
                    if (mx >= bx - 6 && mx <= bx + bw + 6 && my >= by - 10 && my <= by + 10) {
                        g_ui.hoveringSeek = true;
                    }
                    float btnY = py + 109.0f, cx = px + pw * 0.5f;
                    float xs[7] = {px + 26.0f, cx - 72.0f, cx - 36.0f, cx, cx + 36.0f, cx + 72.0f, px + pw - 26.0f};
                    for (int i = 0; i < 7; ++i) {
                        float hr = (i == 3) ? 15.0f : 13.0f;
                        if ((mx - xs[i]) * (mx - xs[i]) + (my - btnY) * (my - btnY) <= hr * hr) { g_ui.hoveredBtn = i; break; }
                    }
                    if (g_ui.hoveredBtn < 0 && !g_ui.hoveringSeek && g_ui.showLyrics) {
                        for (auto const& hb : g_ui.lyricHits) {
                            if (hb.rect.Contains(mx, my)) { g_ui.hoveredLyricIdx = hb.lineIdx; break; }
                        }
                    }
                } else {
                    float colW = 230.0f;
                    float bx = GetStudioPlayerColX(px, pw, colW), bw = colW, by = py + 342.0f;
                    if (mx >= bx - 6 && mx <= bx + bw + 6 && my >= by - 10 && my <= by + 10) {
                        g_ui.hoveringSeek = true;
                    }
                    float bCy = py + 386.0f, bCx = bx + bw * 0.5f;
                    float bXs[5] = {bCx - 92.0f, bCx - 46.0f, bCx, bCx + 46.0f, bCx + 92.0f};
                    for (int i = 1; i <= 5; ++i) {
                        float hr = (i == 3) ? 16.0f : 14.0f;
                        if ((mx - bXs[i - 1]) * (mx - bXs[i - 1]) + (my - bCy) * (my - bCy) <= hr * hr) { g_ui.hoveredBtn = i; break; }
                    }
                    if ((mx - (px + 26.0f)) * (mx - (px + 26.0f)) + (my - (py + ph - 24.0f)) * (my - (py + ph - 24.0f)) <= 196.0f)
                        g_ui.hoveredBtn = 6;
                    if ((mx - (px + pw - 24.0f)) * (mx - (px + pw - 24.0f)) + (my - (py + 22.0f)) * (my - (py + 22.0f)) <= 196.0f)
                        g_ui.hoveredBtn = 7;

                    if (g_ui.hoveredBtn < 0 && !g_ui.hoveringSeek && g_ui.showLyrics) {
                        for (auto const& hb : g_ui.lyricHits) {
                            if (hb.rect.Contains(mx, my)) { g_ui.hoveredLyricIdx = hb.lineIdx; break; }
                        }
                    }
                }
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            WakeAnimationTimer(hwnd);
            float scale = std::max(0.5f, g_ui.dpiScale);
            float mx = (float)GET_X_LPARAM(lParam) / scale;
            if (g_ui.draggingWin) {
                g_ui.draggingWin = false; ReleaseCapture();
            } else if (g_ui.draggingSeek) {
                g_ui.dragSeekRatio = GetSeekRatio(mx); g_ui.draggingSeek = false;
                g_ui.holdSeekRatio = g_ui.dragSeekRatio; g_ui.holdSeekUntil = GetTickCount64() + 900;
                ReleaseCapture();
                g_pendingSeek.store(g_ui.dragSeekRatio);
                if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
            } else if (g_ui.hovered || g_ui.bigMode) {
                if (g_ui.hoveringSeek) {
                    float r = GetSeekRatio(mx);
                    g_ui.holdSeekRatio = r; g_ui.holdSeekUntil = GetTickCount64() + 900;
                    g_pendingSeek.store(r);
                    if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                } else if (g_ui.hoveredBtn >= 0) {
                    int b = g_ui.hoveredBtn;
                    if (b == 0) {
                        g_ui.bigMode = true;
                        g_ui.showLyrics = true;
                    } else if (b == 1) {
                        g_pendingCmd.store(4);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    } else if (b == 2) {
                        g_pendingCmd.store(1);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    } else if (b == 3) {
                        {
                            std::lock_guard<std::mutex> lk(g_mediaMutex);
                            g_media.isPlaying = !g_media.isPlaying;
                            g_media.posTickMs = GetTickCount64();
                        }
                        UpdateAudioCaptureNeeded();
                        g_pendingCmd.store(2);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    } else if (b == 4) {
                        g_pendingCmd.store(3);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    } else if (b == 5) {
                        g_pendingCmd.store(5);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    } else if (b == 6) {
                        g_ui.showLyrics = !g_ui.showLyrics;
                    } else if (b == 7) {
                        g_ui.bigMode = false;
                    }
                } else if (g_ui.showLyrics && g_ui.hoveredLyricIdx >= 0) {
                    std::lock_guard<std::mutex> lk(g_mediaMutex);
                    if (g_ui.hoveredLyricIdx < (int)g_media.lyrics.size() && g_media.durSec > 0.1) {
                        float r = (float)std::clamp(g_media.lyrics[g_ui.hoveredLyricIdx].timeSec / g_media.durSec, 0.0, 1.0);
                        g_ui.holdSeekRatio = r;
                        g_ui.holdSeekUntil = GetTickCount64() + 900;
                        g_ui.lastActiveLyric = g_ui.hoveredLyricIdx;
                        g_pendingSeek.store(r);
                        if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
                    }
                }
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            g_ui.draggingSeek = g_ui.draggingWin = false;
            return 0;

        case WM_MOUSELEAVE:
            if (!g_ui.draggingSeek && !g_ui.draggingWin) {
                g_ui.hovered = g_ui.hoveringSeek = false;
                g_ui.hoveredBtn = -1;
                g_ui.hoveredLyricIdx = -1;
                WakeAnimationTimer(hwnd);
            }
            return 0;

        case WM_ISLAND_RELOAD_SETTINGS:
            LoadModSettings();
            RegisterIslandHotkey(hwnd);
            if (g_smtcWakeEvent) SetEvent(g_smtcWakeEvent);
            if (g_lyricsWakeEvent) SetEvent(g_lyricsWakeEvent);
            WakeAnimationTimer(hwnd);
            return 0;

        case WM_ISLAND_SHUTDOWN:
            UnregisterHotKey(hwnd, HOTKEY_ID_HIDE);
            SetTimerIntervalIfNeeded(hwnd, 0);
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_res.ReleaseAll();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void OverlayUIThread() {
    ULONG_PTR token = 0;
    GdiplusStartupInput gsi;
    GdiplusStartup(&token, &gsi, nullptr);

    HINSTANCE hInst = reinterpret_cast<HINSTANCE>(&__ImageBase);
    const wchar_t* CLS = L"MusicDynamicIslandWindowClass";
    WNDCLASSEXW wc{sizeof(WNDCLASSEXW), 0, IslandWndProc, 0, 0, hInst, nullptr,
                   LoadCursor(nullptr, IDC_HAND), nullptr, nullptr, CLS, nullptr};
    ATOM clsAtom = RegisterClassExW(&wc);

    HWND hwnd = nullptr;
    if (clsAtom) {
        hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            CLS, L"Music Dynamic Island", WS_POPUP, 0, 0, g_set.compactW, g_set.compactH,
            nullptr, nullptr, hInst, nullptr);
    }

    g_hWnd.store(hwnd);
    SetEvent(g_uiReadyEvent);

    if (hwnd) {
        std::thread smtcThread(SmtcWorkerThread);
        std::thread audioThread(AudioLoopbackThread);
        std::thread lyricsThread(LyricsWorkerThread);

        ShowWindow(hwnd, SW_SHOWNOACTIVATE);

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        SetEvent(g_stopEvent);
        AbortActiveHttpRequests();

        if (smtcThread.joinable())   smtcThread.join();
        if (audioThread.joinable())  audioThread.join();
        if (lyricsThread.joinable()) lyricsThread.join();
    }

    {
        std::lock_guard<std::mutex> lk(g_mediaMutex);
        g_media.bigArt.reset(); g_media.prevBigArt.reset();
        g_media.expArt.reset(); g_media.prevExpArt.reset();
        g_media.miniArt.reset(); g_media.prevMiniArt.reset();
        g_media.blurBg.reset(); g_media.prevBlurBg.reset();
    }

    g_hWnd.store(nullptr);
    if (clsAtom) UnregisterClassW(CLS, hInst);
    GdiplusShutdown(token);
}

BOOL WhTool_ModInit() {
    LoadModSettings();
    g_ui.curW = (float)g_set.compactW;
    g_ui.curH = (float)g_set.compactH;

    g_uiReadyEvent    = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_stopEvent       = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_smtcWakeEvent   = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    g_audioWakeEvent  = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_lyricsWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_uiThread.emplace(OverlayUIThread);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (HWND h = g_hWnd.load()) {
        PostMessageW(h, WM_ISLAND_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    AbortActiveHttpRequests();

    if (g_uiThread) {
        if (g_uiReadyEvent) WaitForSingleObject(g_uiReadyEvent, INFINITE);
        if (HWND h = g_hWnd.load()) PostMessageW(h, WM_ISLAND_SHUTDOWN, 0, 0);
        g_uiThread->join();
        g_uiThread.reset();
    }

    if (g_uiReadyEvent)    { CloseHandle(g_uiReadyEvent);    g_uiReadyEvent = nullptr; }
    if (g_stopEvent)       { CloseHandle(g_stopEvent);       g_stopEvent = nullptr; }
    if (g_smtcWakeEvent)   { CloseHandle(g_smtcWakeEvent);   g_smtcWakeEvent = nullptr; }
    if (g_audioWakeEvent)  { CloseHandle(g_audioWakeEvent);  g_audioWakeEvent = nullptr; }
    if (g_lyricsWakeEvent) { CloseHandle(g_lyricsWakeEvent); g_lyricsWakeEvent = nullptr; }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as-is at the end of your mod file, and implement the
// following:
// - WhTool_ModInit
// - WhTool_ModSettingsChanged
// - WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isParentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isToolModProcess = true;
            } else {
                isExcluded = true;
            }
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-parent-tool-mod") == 0) {
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isParentToolModProcess = true;
                break;
            }
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isToolModProcess) {
     g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutexW failed");
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
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isParentToolModProcess) {
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
    case 0:
  case 1:
    case ARRAYSIZE(currentProcessPath):
        TCHAR szDummy[1] = {0};
        (void)szDummy;
        Wh_Log(L"GetModuleFileNameW failed");
        return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll or kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
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

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed");
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