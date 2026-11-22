// ==WindhawkMod==
// @id              now-playing-windhawk
// @name            Now Playing...
// @description     A sleek now-playing bar that drops down from the top edge of your screen when you hover there, then shows whatever music or audio you're listening to
// @version         1.0.0
// @author          sultansaad-dev
// @github          https://github.com/sultansaad-dev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -ld2d1 -ldwrite -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luuid -lshell32 -lshlwapi -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Now Playing...

Push your mouse to the top of your screen and a now-playing bar drops down,
kind of like the taskbar popping up but upside down. It shows whatever you're
listening to: Spotify, SoundCloud, YouTube in your browser, Apple Music, VLC,
pretty much anything that shows up in Windows' media controls.

![Now Playing... close-ups](https://raw.githubusercontent.com/sultansaad-dev/now-playing-windhawk/main/screenshots/showcase.png)

![On the desktop](https://raw.githubusercontent.com/sultansaad-dev/now-playing-windhawk/main/screenshots/desktop-youtube.png)

![In Fortnite](https://raw.githubusercontent.com/sultansaad-dev/now-playing-windhawk/main/screenshots/in-game-fortnite.png)

## What's on it
- The album art or video thumbnail, with the app's icon in the corner
- Song title and artist (fancy fonts in names show up properly, no boxes)
- A little button next to the artist that jumps you straight to the app
- Play/pause, plus 10 second skip back and forward
- A progress line you can click or drag to jump anywhere
- Previous and next, but only when the app actually lets you skip
- A wave line that moves with the music

## Stuff you can change
- Put it top center, top left or top right, on any monitor
- Background: auto (takes its colour from the album art), translucent, a solid
  colour from the list, or light
- The drip, which is the little liquid neck that makes it look like it's
  hanging off the top of your screen
- **Persistent mode:** press Ctrl + Shift + ? and the bar pops up instantly
  and stays up, even in games. Press it again to hide it
- It can peek for a few seconds when a new song starts

It stays hidden over full screen stuff by default so it won't get in the way
of your games or videos (use **Persistent mode** if you want it in a game).

There's a lot more in the **Settings** tab to play around with: size, width,
corner roundness, animation speed, hover delays, colours, the visualizer
style and more. Tweak it however you like, everything applies straight away.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: center
  $name: Position
  $options:
  - center: Top center
  - left: Top left
  - right: Top right
- allMonitors: true
  $name: Work on all monitors
  $description: Appears on whichever monitor your cursor is on. Turn off to use only the main monitor.
- width: 420
  $name: Width (px)
- scale: 100
  $name: Size (%)
- offsetTop: 14
  $name: Gap from top edge (px)
- offsetX: 0
  $name: Horizontal offset (px)
  $description: Fine-tune the position left (negative) or right (positive)
- sideMargin: 16
  $name: Side margin (px)
  $description: Distance from the screen edge when using Top left or Top right
- background: auto
  $name: Background style
  $options:
  - auto: Auto (colours from album art)
  - translucent: Translucent dark
  - solid: Solid colour
  - light: Light
- solidPreset: black
  $name: Solid colour
  $description: Used by the Solid colour style
  $options:
  - black: Black
  - graphite: Graphite
  - navy: Navy
  - blue: Blue
  - purple: Purple
  - pink: Pink
  - red: Red
  - orange: Orange
  - green: Green
  - teal: Teal
  - mint: Mint
  - sky: Sky blue
  - lavender: Lavender
  - maroon: Maroon
  - gold: Gold
  - silver: Silver
  - white: White
- opacity: 88
  $name: Opacity (%)
  $description: Used by the Auto, Translucent and Light styles
- cornerRadius: 22
  $name: Corner roundness (px)
- animation: normal
  $name: Animation speed
  $options:
  - slow: Slow
  - normal: Normal
  - fast: Fast
- revealDelay: 120
  $name: Hover delay before showing (ms)
- hideDelay: 450
  $name: Delay before hiding (ms)
- wideTrigger: false
  $name: Trigger along the whole top edge
  $description: By default only the top edge above the bar triggers it
- peekOnTrackChange: true
  $name: Peek when a new track starts
- peekDuration: 3000
  $name: Peek duration (ms)
- showWhenIdle: false
  $name: Show even when nothing is playing
- alwaysVisible: false
  $name: Always visible while media is playing
- hideInFullscreen: true
  $name: Don't show over full-screen apps and games
- visualizerStyle: wave
  $name: Audio visualizer
  $options:
  - wave: Wave line
  - bars: Bars
  - off: Off
- drip: true
  $name: Drip connector
  $description: A liquid neck joins the bar to the top edge, like a drop hanging from the screen
- hotkey: ctrl_shift_slash
  $name: Persistent mode hotkey
  $description: Press once and the bar pops up instantly and stays up (works in games). Press again to hide it
  $options:
  - ctrl_shift_slash: Ctrl + Shift + ?
  - shift_slash: Shift + ? (blocks typing ?)
  - alt_n: Alt + N
  - ctrl_alt_m: Ctrl + Alt + M
  - off: Off
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace Gdiplus {
using std::max;
using std::min;
}  // namespace Gdiplus
#include <gdiplus.h>

#include <d2d1.h>
#include <dwrite.h>
#include <mmdeviceapi.h>
#include <shellscalingapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace winrt::Windows::Media::Control;
using SteadyClock = std::chrono::steady_clock;

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int position = 0;  // 0 center, 1 left, 2 right
    bool allMonitors = true;
    int width = 420;
    int scalePct = 100;
    int offsetTop = 10;
    int offsetX = 0;
    int sideMargin = 16;
    int bgMode = 0;  // 0 auto, 1 translucent, 2 solid, 3 light
    Color solid{255, 30, 30, 36};
    int opacityPct = 88;
    int cornerRadius = 22;
    int animMs = 320;
    int revealDelay = 120;
    int hideDelay = 450;
    bool wideTrigger = false;
    bool peek = true;
    int peekMs = 3000;
    bool showIdle = false;
    bool alwaysVisible = false;
    bool hideFullscreen = true;
    int visualizer = 1;  // 0 off, 1 wave, 2 bars
    bool drip = true;
    UINT hotkeyMods = MOD_CONTROL | MOD_SHIFT;
    UINT hotkeyVk = VK_OEM_2;
};

static Settings g_s;

static std::wstring GetStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static void LoadSettings() {
    Settings s;

    std::wstring pos = GetStr(L"position");
    s.position = pos == L"left" ? 1 : pos == L"right" ? 2 : 0;
    s.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    s.width = std::clamp(Wh_GetIntSetting(L"width"), 260, 1200);
    s.scalePct = std::clamp(Wh_GetIntSetting(L"scale"), 50, 250);
    s.offsetTop = std::clamp(Wh_GetIntSetting(L"offsetTop"), 0, 400);
    s.offsetX = Wh_GetIntSetting(L"offsetX");
    s.sideMargin = std::clamp(Wh_GetIntSetting(L"sideMargin"), 0, 2000);

    std::wstring bg = GetStr(L"background");
    s.bgMode = bg == L"translucent" ? 1 : bg == L"solid" ? 2 : bg == L"light" ? 3 : 0;
    std::wstring preset = GetStr(L"solidPreset");
    static const struct {
        const wchar_t* name;
        BYTE r, g, b;
    } kPresets[] = {
        {L"black", 12, 12, 14},    {L"graphite", 46, 46, 52},
        {L"navy", 20, 32, 72},     {L"blue", 28, 82, 186},
        {L"purple", 92, 46, 168},  {L"pink", 196, 62, 132},
        {L"red", 176, 32, 44},     {L"orange", 206, 104, 22},
        {L"green", 30, 124, 64},   {L"teal", 14, 122, 122},
        {L"mint", 62, 180, 140},   {L"sky", 70, 150, 220},
        {L"lavender", 150, 128, 210}, {L"maroon", 110, 22, 40},
        {L"gold", 196, 152, 40},   {L"silver", 176, 180, 188},
        {L"white", 244, 244, 246},
    };
    s.solid = Color(255, 12, 12, 14);
    for (auto const& pr : kPresets) {
        if (preset == pr.name) {
            s.solid = Color(255, pr.r, pr.g, pr.b);
        }
    }
    s.opacityPct = std::clamp(Wh_GetIntSetting(L"opacity"), 10, 100);
    s.cornerRadius = std::clamp(Wh_GetIntSetting(L"cornerRadius"), 0, 60);

    std::wstring anim = GetStr(L"animation");
    s.animMs = anim == L"slow" ? 480 : anim == L"fast" ? 200 : 320;
    s.revealDelay = std::clamp(Wh_GetIntSetting(L"revealDelay"), 0, 3000);
    s.hideDelay = std::clamp(Wh_GetIntSetting(L"hideDelay"), 0, 10000);
    s.wideTrigger = Wh_GetIntSetting(L"wideTrigger") != 0;
    s.peek = Wh_GetIntSetting(L"peekOnTrackChange") != 0;
    s.peekMs = std::clamp(Wh_GetIntSetting(L"peekDuration"), 500, 30000);
    s.showIdle = Wh_GetIntSetting(L"showWhenIdle") != 0;
    s.alwaysVisible = Wh_GetIntSetting(L"alwaysVisible") != 0;
    s.hideFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    std::wstring vis = GetStr(L"visualizerStyle");
    s.visualizer = vis == L"off" ? 0 : vis == L"bars" ? 2 : 1;
    s.drip = Wh_GetIntSetting(L"drip") != 0;
    std::wstring hk = GetStr(L"hotkey");
    if (hk == L"shift_slash") {
        s.hotkeyMods = MOD_SHIFT;
        s.hotkeyVk = VK_OEM_2;
    } else if (hk == L"alt_n") {
        s.hotkeyMods = MOD_ALT;
        s.hotkeyVk = 'N';
    } else if (hk == L"ctrl_alt_m") {
        s.hotkeyMods = MOD_CONTROL | MOD_ALT;
        s.hotkeyVk = 'M';
    } else if (hk == L"off") {
        s.hotkeyMods = 0;
        s.hotkeyVk = 0;
    } else {
        s.hotkeyMods = MOD_CONTROL | MOD_SHIFT;
        s.hotkeyVk = VK_OEM_2;
    }

    g_s = s;
}

////////////////////////////////////////////////////////////////////////////////
// Shared media state (written by the media thread, read by the UI thread)

struct MediaSnapshot {
    bool hasSession = false;
    bool playing = false;
    std::wstring title;
    std::wstring artist;
    std::shared_ptr<Bitmap> art;
    std::shared_ptr<Bitmap> icon;
    Color accent{255, 60, 60, 72};
    bool hasAccent = false;
    double posSec = 0;
    double durSec = 0;
    SteadyClock::time_point posStamp{};
    unsigned trackSerial = 0;
    bool canPrev = false;
    bool canNext = false;
    bool canSeek = false;
    bool keySeek = false;  // seek by sending keys to the player's window
    std::wstring aumid;
};

static std::mutex g_mediaMutex;
static MediaSnapshot g_media;

static std::atomic<bool> g_stop{false};
// 1 prev, 2 play/pause, 3 next, 4 back 10s, 5 forward 10s, 7 jump to
// g_seekTo seconds
static std::atomic<int> g_command{0};
static std::atomic<double> g_seekTo{0};
static std::mutex g_wakeMutex;
static std::condition_variable g_wakeCv;

static std::thread g_uiThread;
static std::thread g_mediaThread;
static HWND g_hwnd = nullptr;
static ULONG_PTR g_gdiplusToken = 0;

static const UINT WM_APP_SETTINGS = WM_APP + 1;

////////////////////////////////////////////////////////////////////////////////
// Image helpers

static std::shared_ptr<Bitmap> BitmapFromPixels(const std::vector<uint32_t>& px,
                                                int w,
                                                int h) {
    auto b = std::make_shared<Bitmap>(w, h, PixelFormat32bppPARGB);
    if (b->GetLastStatus() != Ok) {
        return nullptr;
    }
    BitmapData d;
    Rect r(0, 0, w, h);
    if (b->LockBits(&r, ImageLockModeWrite, PixelFormat32bppPARGB, &d) != Ok) {
        return nullptr;
    }
    for (int y = 0; y < h; y++) {
        memcpy((BYTE*)d.Scan0 + y * d.Stride, &px[(size_t)y * w], (size_t)w * 4);
    }
    b->UnlockBits(&d);
    return b;
}

static std::shared_ptr<Bitmap> BitmapFromHBitmap(HBITMAP hb) {
    BITMAP bm{};
    if (!GetObject(hb, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0) {
        return nullptr;
    }
    int w = bm.bmWidth;
    int h = bm.bmHeight;
    std::vector<uint32_t> px((size_t)w * h);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, hb, 0, h, px.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (lines != h) {
        return nullptr;
    }
    bool anyAlpha = false;
    for (uint32_t p : px) {
        if (p >> 24) {
            anyAlpha = true;
            break;
        }
    }
    if (!anyAlpha) {
        for (uint32_t& p : px) {
            p |= 0xFF000000;
        }
    }
    return BitmapFromPixels(px, w, h);
}

static std::shared_ptr<Bitmap> ShellImage(const std::wstring& parsingName,
                                          int size) {
    winrt::com_ptr<IShellItemImageFactory> factory;
    if (FAILED(SHCreateItemFromParsingName(parsingName.c_str(), nullptr,
                                           IID_PPV_ARGS(factory.put())))) {
        return nullptr;
    }
    HBITMAP hb = nullptr;
    if (FAILED(factory->GetImage(SIZE{size, size}, SIIGBF_ICONONLY, &hb)) ||
        !hb) {
        return nullptr;
    }
    auto result = BitmapFromHBitmap(hb);
    DeleteObject(hb);
    return result;
}

static std::wstring FindRunningExePath(const std::wstring& exeName) {
    std::wstring result;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return result;
    }
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, exeName.c_str()) == 0) {
                HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                       pe.th32ProcessID);
                if (h) {
                    WCHAR buf[MAX_PATH];
                    DWORD n = MAX_PATH;
                    if (QueryFullProcessImageNameW(h, 0, buf, &n)) {
                        result = buf;
                    }
                    CloseHandle(h);
                }
                if (!result.empty()) {
                    break;
                }
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return result;
}

// Gets the icon of the app that owns the media session, e.g. Spotify, Chrome.
static std::shared_ptr<Bitmap> AppIconForAumid(const std::wstring& aumid) {
    if (aumid.empty()) {
        return nullptr;
    }
    if (PathFileExistsW(aumid.c_str())) {
        if (auto b = ShellImage(aumid, 64)) {
            return b;
        }
    }
    if (auto b = ShellImage(L"shell:AppsFolder\\" + aumid, 64)) {
        return b;
    }
    // Desktop apps such as Spotify often report just "Spotify.exe".
    std::wstring exe = aumid;
    size_t slash = exe.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        exe = exe.substr(slash + 1);
    }
    if (exe.size() < 4 || _wcsicmp(exe.c_str() + exe.size() - 4, L".exe") != 0) {
        exe += L".exe";
    }
    std::wstring path = FindRunningExePath(exe);
    if (!path.empty()) {
        return ShellImage(path, 64);
    }
    return nullptr;
}

// Loads album art / video thumbnail, square-cropped, and picks an accent colour.
static std::shared_ptr<Bitmap> LoadArt(
    winrt::Windows::Storage::Streams::IRandomAccessStreamReference const& ref,
    Color& accent,
    bool& hasAccent) {
    hasAccent = false;
    if (!ref) {
        return nullptr;
    }
    using namespace winrt::Windows::Storage::Streams;
    auto stream = ref.OpenReadAsync().get();
    uint64_t size64 = stream.Size();
    if (size64 == 0 || size64 > 32 * 1024 * 1024) {
        return nullptr;
    }
    uint32_t size = (uint32_t)size64;
    Buffer buffer(size);
    auto result = stream.ReadAsync(buffer, size, InputStreamOptions::None).get();
    std::vector<uint8_t> data(result.Length());
    if (data.empty()) {
        return nullptr;
    }
    DataReader::FromBuffer(result).ReadBytes(data);
    winrt::com_ptr<IStream> istream;
    istream.attach(SHCreateMemStream(data.data(), (UINT)data.size()));
    if (!istream) {
        return nullptr;
    }

    const int N = 160;
    std::shared_ptr<Bitmap> out;
    {
        Bitmap src(istream.get());
        if (src.GetLastStatus() != Ok || src.GetWidth() == 0 ||
            src.GetHeight() == 0) {
            return nullptr;
        }
        int sw = (int)src.GetWidth();
        int sh = (int)src.GetHeight();
        int side = (std::min)(sw, sh);
        out = std::make_shared<Bitmap>(N, N, PixelFormat32bppPARGB);
        Graphics g(out.get());
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        g.DrawImage(&src, Rect(0, 0, N, N), (sw - side) / 2, (sh - side) / 2,
                    side, side, UnitPixel);
    }

    // Accent: average colour weighted towards vivid pixels.
    BitmapData d;
    Rect r(0, 0, N, N);
    if (out->LockBits(&r, ImageLockModeRead, PixelFormat32bppPARGB, &d) == Ok) {
        double sr = 0, sg = 0, sb = 0, sw = 0;
        for (int y = 0; y < N; y += 2) {
            const uint32_t* row = (const uint32_t*)((BYTE*)d.Scan0 + y * d.Stride);
            for (int x = 0; x < N; x += 2) {
                uint32_t p = row[x];
                double R = (p >> 16) & 255, G = (p >> 8) & 255, B = p & 255;
                double mx = (std::max)({R, G, B}), mn = (std::min)({R, G, B});
                double sat = mx > 0 ? (mx - mn) / mx : 0;
                double val = mx / 255.0;
                double w = sat * sat * val + 0.02;
                sr += R * w;
                sg += G * w;
                sb += B * w;
                sw += w;
            }
        }
        out->UnlockBits(&d);
        if (sw > 0) {
            accent = Color(255, (BYTE)(sr / sw), (BYTE)(sg / sw), (BYTE)(sb / sw));
            hasAccent = true;
        }
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Keyboard seeking for players that don't let Windows change the position
// (YouTube and most browser videos). We find the window whose title contains
// the track title, which is the browser tab that's playing, and press the
// player's own skip keys.

struct FindTitleCtx {
    std::wstring needle;
    HWND found = nullptr;
};

static BOOL CALLBACK FindTitleProc(HWND hwnd, LPARAM lp) {
    auto* ctx = (FindTitleCtx*)lp;
    if (!IsWindowVisible(hwnd) || hwnd == g_hwnd) {
        return TRUE;
    }
    WCHAR buf[512];
    if (GetWindowTextW(hwnd, buf, ARRAYSIZE(buf)) > 0 &&
        wcsstr(buf, ctx->needle.c_str())) {
        ctx->found = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindPlayerWindow(const std::wstring& title) {
    if (title.size() < 3) {
        return nullptr;
    }
    FindTitleCtx ctx;
    ctx.needle = title.substr(0, 40);
    EnumWindows(FindTitleProc, (LPARAM)&ctx);
    return ctx.found;
}

// Posts a key press straight to the window, without bringing it to the front.
static void PostKey(HWND hwnd, WORD vk) {
    UINT sc = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    bool extended = vk == VK_LEFT || vk == VK_RIGHT;
    LPARAM down = 1 | ((LPARAM)sc << 16) | (extended ? (1 << 24) : 0);
    LPARAM up = down | (1u << 30) | (1u << 31);
    PostMessage(hwnd, WM_KEYDOWN, vk, down);
    if (vk >= 'A' && vk <= 'Z') {
        PostMessage(hwnd, WM_CHAR, (WPARAM)towlower(vk), down);
    } else if (vk >= '0' && vk <= '9') {
        PostMessage(hwnd, WM_CHAR, (WPARAM)vk, down);
    }
    PostMessage(hwnd, WM_KEYUP, vk, up);
}

static void KeyboardSeek(std::wstring title, bool forward) {
    std::thread([title, forward] {
        HWND player = FindPlayerWindow(title);
        if (!player) {
            Wh_Log(L"Keyboard seek: player window not found");
            return;
        }
        WCHAR buf[512] = L"";
        GetWindowTextW(player, buf, ARRAYSIZE(buf));
        // YouTube: J / L jump 10 seconds. Other players: arrow keys, pressed
        // twice since most of them jump 5 seconds per press.
        bool youtube = wcsstr(buf, L"YouTube") != nullptr;
        WORD vk = youtube ? (forward ? 'L' : 'J')
                          : (forward ? VK_RIGHT : VK_LEFT);
        int presses = youtube ? 1 : 2;
        for (int i = 0; i < presses; i++) {
            PostKey(player, vk);
            Sleep(40);
        }
    }).detach();
}

// Jump to any point using the player's keys, for apps that don't let Windows
// change the position. YouTube: number keys jump to 0%, 10% ... 90%, then L
// steps 10 seconds at a time. Other players: arrow keys, 5 seconds each.
static void KeyboardSeekTo(std::wstring title, double from, double to,
                           double dur) {
    std::thread([title, from, to, dur] {
        HWND player = FindPlayerWindow(title);
        if (!player) {
            return;
        }
        WCHAR buf[512] = L"";
        GetWindowTextW(player, buf, ARRAYSIZE(buf));
        if (wcsstr(buf, L"YouTube") && dur > 1) {
            int d = std::clamp((int)(to / dur * 10), 0, 9);
            double base = d * dur / 10.0;
            PostKey(player, (WORD)('0' + d));
            Sleep(80);
            int n = (int)std::lround((to - base) / 10.0);
            for (int i = 0; i < n && i < 60; i++) {
                PostKey(player, 'L');
                Sleep(25);
            }
        } else {
            int n = (int)std::lround((to - from) / 5.0);
            WORD vk = n > 0 ? VK_RIGHT : VK_LEFT;
            n = (std::min)(std::abs(n), 120);
            for (int i = 0; i < n; i++) {
                PostKey(player, vk);
                Sleep(20);
            }
        }
    }).detach();
}

static void KeyboardPlayPause(std::wstring title) {
    std::thread([title] {
        HWND player = FindPlayerWindow(title);
        if (!player) {
            return;
        }
        WCHAR buf[512] = L"";
        GetWindowTextW(player, buf, ARRAYSIZE(buf));
        // YouTube: K toggles play/pause. Most other players: space.
        PostKey(player, wcsstr(buf, L"YouTube") ? 'K' : VK_SPACE);
    }).detach();
}

struct FindPidCtx {
    DWORD pid;
    HWND found = nullptr;
};

static BOOL CALLBACK FindPidProc(HWND hwnd, LPARAM lp) {
    auto* ctx = (FindPidCtx*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == ctx->pid && IsWindowVisible(hwnd) && !GetWindow(hwnd, GW_OWNER) &&
        GetWindowTextLengthW(hwnd) > 0) {
        ctx->found = hwnd;
        return FALSE;
    }
    return TRUE;
}

// Brings the app that's playing to the front.
static void FocusPlayer(std::wstring title, std::wstring aumid) {
    std::thread([title, aumid] {
        HWND player = FindPlayerWindow(title);
        if (!player && !aumid.empty()) {
            // Fall back to the app's main window, e.g. Spotify while paused.
            std::wstring exe = aumid;
            size_t slash = exe.find_last_of(L"\\/");
            if (slash != std::wstring::npos) {
                exe = exe.substr(slash + 1);
            }
            if (exe.size() < 4 ||
                _wcsicmp(exe.c_str() + exe.size() - 4, L".exe") != 0) {
                exe += L".exe";
            }
            HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snap != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32W pe{};
                pe.dwSize = sizeof(pe);
                if (Process32FirstW(snap, &pe)) {
                    do {
                        if (_wcsicmp(pe.szExeFile, exe.c_str()) == 0) {
                            FindPidCtx ctx{pe.th32ProcessID};
                            EnumWindows(FindPidProc, (LPARAM)&ctx);
                            if (ctx.found) {
                                player = ctx.found;
                                break;
                            }
                        }
                    } while (Process32NextW(snap, &pe));
                }
                CloseHandle(snap);
            }
        }
        if (!player) {
            Wh_Log(L"Open player: window not found");
            return;
        }
        if (IsIconic(player)) {
            ShowWindow(player, SW_RESTORE);
        }
        SetForegroundWindow(player);
    }).detach();
}

////////////////////////////////////////////////////////////////////////////////
// Media thread

static GlobalSystemMediaTransportControlsSession PickSession(
    GlobalSystemMediaTransportControlsSessionManager const& mgr) {
    for (auto const& s : mgr.GetSessions()) {
        try {
            if (s.GetPlaybackInfo().PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                return s;
            }
        } catch (...) {
        }
    }
    return mgr.GetCurrentSession();
}

static void MediaThreadProc() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    std::map<std::wstring, std::shared_ptr<Bitmap>> iconCache;
    std::wstring lastKey;
    std::shared_ptr<Bitmap> art;
    Color accent(255, 60, 60, 72);
    bool hasAccent = false;
    int artRetries = 0;
    unsigned serial = 0;
    // Our own playback clock (see below).
    double clockPos = 0;
    SteadyClock::time_point clockStamp = SteadyClock::now();
    bool clockPlaying = false;
    double lastRawPos = -1;
    std::wstring clockKey;

    while (!g_stop) {
        try {
            if (!mgr) {
                mgr = GlobalSystemMediaTransportControlsSessionManager::
                          RequestAsync()
                              .get();
            }

            auto session = PickSession(mgr);
            int cmd = g_command.exchange(0);

            if (!session) {
                static int noSessionLogs = 0;
                if (noSessionLogs < 3) {
                    noSessionLogs++;
                    Wh_Log(L"Media: no media session found");
                }
                std::lock_guard<std::mutex> lock(g_mediaMutex);
                g_media.hasSession = false;
                g_media.playing = false;
            } else {
                if (cmd == 1) {
                    session.TrySkipPreviousAsync().get();
                } else if (cmd == 2) {
                    if (!session.TryTogglePlayPauseAsync().get()) {
                        // Some players only answer to Play or Pause directly.
                        bool isPlaying =
                            session.GetPlaybackInfo().PlaybackStatus() ==
                            GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                Playing;
                        bool ok = isPlaying ? session.TryPauseAsync().get()
                                            : session.TryPlayAsync().get();
                        if (!ok) {
                            auto p = session.TryGetMediaPropertiesAsync().get();
                            if (p) {
                                KeyboardPlayPause(p.Title().c_str());
                            }
                        }
                    }
                } else if (cmd == 3) {
                    session.TrySkipNextAsync().get();
                }

                auto props = session.TryGetMediaPropertiesAsync().get();
                auto info = session.GetPlaybackInfo();
                bool playing =
                    info.PlaybackStatus() ==
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                // Which buttons the app actually supports right now.
                bool canPrev = false, canNext = false, canSeek = false;
                if (auto controls = info.Controls()) {
                    canPrev = controls.IsPreviousEnabled();
                    canNext = controls.IsNextEnabled();
                    canSeek = controls.IsPlaybackPositionEnabled();
                }
                std::wstring title = props ? props.Title().c_str() : L"";
                std::wstring artist = props ? props.Artist().c_str() : L"";
                std::wstring aumid = session.SourceAppUserModelId().c_str();

                auto tl = session.GetTimelineProperties();
                double rawPos =
                    std::chrono::duration<double>(tl.Position()).count();
                double dur = std::chrono::duration<double>(tl.EndTime() -
                                                           tl.StartTime())
                                 .count();
                std::wstring trackKey = aumid + L"\x1F" + title;

                // Browsers often only report the position when you play,
                // pause or seek, so the mod keeps its own clock running in
                // between and resyncs whenever the app reports something new.
                auto nowS = SteadyClock::now();
                double pos = clockPos;
                if (clockPlaying) {
                    pos += std::chrono::duration<double>(nowS - clockStamp)
                               .count();
                }
                if (trackKey != clockKey || std::abs(rawPos - lastRawPos) > 0.25) {
                    pos = rawPos;
                    if (playing) {
                        double age =
                            std::chrono::duration<double>(winrt::clock::now() -
                                                          tl.LastUpdatedTime())
                                .count();
                        if (age > 0 && age < 30) {
                            pos += age;
                        }
                    }
                }
                lastRawPos = rawPos;
                clockKey = trackKey;

                // 10-second skip back / forward. Try the proper Windows way
                // first; if the app doesn't take it (YouTube in a browser),
                // press the player's own skip keys in the background.
                if (cmd == 4 || cmd == 5) {
                    double target = pos + (cmd == 5 ? 10.0 : -10.0);
                    target = (std::max)(0.0, target);
                    if (dur > 1) {
                        target = (std::min)(target, dur - 0.5);
                    }
                    int64_t ticks = (int64_t)(target * 10000000.0) +
                                    tl.StartTime().count();
                    bool done = false;
                    if (dur > 1) {
                        try {
                            done =
                                session.TryChangePlaybackPositionAsync(ticks).get();
                        } catch (...) {
                            done = false;
                        }
                    }
                    if (done) {
                        pos = target;
                    } else {
                        KeyboardSeek(title, cmd == 5);
                        pos = target;
                    }
                }

                // Jump to a point picked on the progress line.
                if (cmd == 7 && dur > 1) {
                    double target = std::clamp(g_seekTo.load(), 0.0, dur - 0.5);
                    int64_t ticks = (int64_t)(target * 10000000.0) +
                                    tl.StartTime().count();
                    bool done = false;
                    try {
                        done = session.TryChangePlaybackPositionAsync(ticks).get();
                    } catch (...) {
                        done = false;
                    }
                    if (!done) {
                        KeyboardSeekTo(title, pos, target, dur);
                    }
                    pos = target;
                }

                if (dur > 1) {
                    pos = std::clamp(pos, 0.0, dur);
                }
                clockPos = pos;
                clockStamp = nowS;
                clockPlaying = playing;

                std::wstring key = aumid + L"\x1F" + title + L"\x1F" + artist;
                bool newTrack = key != lastKey;
                if (newTrack) {
                    Wh_Log(L"Media: app=%s title=%s playing=%d", aumid.c_str(),
                           title.c_str(), (int)playing);
                    lastKey = key;
                    art = nullptr;
                    hasAccent = false;
                    artRetries = 0;
                    if (!title.empty() && playing) {
                        serial++;
                    }
                }
                // Browsers often deliver the thumbnail a moment after the
                // title, so keep trying for a few seconds.
                if (!art && artRetries < 12 && props) {
                    artRetries++;
                    try {
                        art = LoadArt(props.Thumbnail(), accent, hasAccent);
                    } catch (...) {
                        art = nullptr;
                    }
                }

                std::shared_ptr<Bitmap> icon;
                auto it = iconCache.find(aumid);
                if (it != iconCache.end()) {
                    icon = it->second;
                } else {
                    icon = AppIconForAumid(aumid);
                    iconCache[aumid] = icon;
                }

                // Looked up outside the lock: it talks to other windows.
                bool keySeek = !(canSeek && dur > 1) &&
                               FindPlayerWindow(title) != nullptr;

                std::lock_guard<std::mutex> lock(g_mediaMutex);
                g_media.hasSession = true;
                g_media.playing = playing;
                g_media.title = title;
                g_media.artist = artist;
                g_media.art = art;
                g_media.icon = icon;
                g_media.hasAccent = hasAccent;
                if (hasAccent) {
                    g_media.accent = accent;
                }
                g_media.posSec = pos;
                g_media.durSec = dur;
                g_media.canPrev = canPrev;
                g_media.canNext = canNext;
                g_media.canSeek = canSeek && dur > 1;
                g_media.keySeek = keySeek;
                g_media.aumid = aumid;
                g_media.posStamp = nowS;
                g_media.trackSerial = serial;
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Media error %08X: %s", (unsigned)e.code(),
                   e.message().c_str());
            mgr = nullptr;
            std::lock_guard<std::mutex> lock(g_mediaMutex);
            g_media.hasSession = false;
        } catch (...) {
            Wh_Log(L"Media: unknown error");
            mgr = nullptr;
            std::lock_guard<std::mutex> lock(g_mediaMutex);
            g_media.hasSession = false;
        }

        std::unique_lock<std::mutex> lock(g_wakeMutex);
        g_wakeCv.wait_for(lock, std::chrono::milliseconds(400), [] {
            return g_stop.load() || g_command.load() != 0;
        });
    }

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        g_media = MediaSnapshot{};
    }
    iconCache.clear();
    art = nullptr;
    mgr = nullptr;
    winrt::uninit_apartment();
}

static void SendMediaCommand(int cmd) {
    g_command = cmd;
    g_wakeCv.notify_all();
}

////////////////////////////////////////////////////////////////////////////////
// Audio level meter

// Our own declaration of IAudioMeterInformation: the compiler's
// endpointvolume.h only forward-declares it.
static const GUID kIID_AudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

struct IPeakMeter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count,
                                                            float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};

static winrt::com_ptr<IPeakMeter> g_meter;
static DWORD g_meterTick = 0;

static float GetAudioPeak() {
    DWORD now = GetTickCount();
    if (!g_meter || now - g_meterTick > 5000) {
        // Re-acquire periodically so we follow default device changes.
        g_meterTick = now;
        g_meter = nullptr;
        winrt::com_ptr<IMMDeviceEnumerator> en;
        winrt::com_ptr<IMMDevice> dev;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                       CLSCTX_ALL, IID_PPV_ARGS(en.put()))) &&
            SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                  dev.put()))) {
            dev->Activate(kIID_AudioMeterInformation, CLSCTX_ALL, nullptr,
                          g_meter.put_void());
        }
    }
    float peak = 0;
    if (g_meter && FAILED(g_meter->GetPeakValue(&peak))) {
        g_meter = nullptr;
        peak = 0;
    }
    return peak;
}

////////////////////////////////////////////////////////////////////////////////
// UI state

struct Rgba {
    float a, r, g, b;
};

static Rgba ToRgba(Color c) {
    return {(float)c.GetA(), (float)c.GetR(), (float)c.GetG(), (float)c.GetB()};
}

static Color ToColor(Rgba c) {
    auto cl = [](float v) { return (BYTE)std::clamp(v + 0.5f, 0.0f, 255.0f); };
    return Color(cl(c.a), cl(c.r), cl(c.g), cl(c.b));
}

static Rgba Lerp(Rgba a, Rgba b, float t) {
    return {a.a + (b.a - a.a) * t, a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t};
}

static Color WithAlpha(Color c, float alphaMul) {
    return Color((BYTE)std::clamp(c.GetA() * alphaMul, 0.0f, 255.0f), c.GetR(),
                 c.GetG(), c.GetB());
}

static Color Mix(Color a, Color b, float t) {
    return ToColor(Lerp(ToRgba(a), ToRgba(b), t));
}

enum class Phase { Hidden, Shown };

struct UiState {
    Phase target = Phase::Hidden;
    float anim = 0;  // 0 hidden .. 1 shown
    SteadyClock::time_point lastFrame{};
    SteadyClock::time_point triggerSince{};
    bool inTrigger = false;
    SteadyClock::time_point lastInside{};
    SteadyClock::time_point peekUntil{};
    unsigned seenSerial = 0;
    bool serialInit = false;

    HMONITOR monitor = nullptr;
    RECT monRect{};
    float scale = 1;
    int pad = 0;
    int pillW = 0, pillH = 0;
    int winX = 0, winY = 0, winW = 0, winH = 0;
    bool windowVisible = false;

    Rgba bg1{}, bg2{};
    bool bgInit = false;
    float bars[5]{};

    struct Button {
        int cmd;
        RectF r;
    };
    Button buttons[6]{};
    int buttonCount = 0;
    int hoverCmd = 0;
    bool pinned = false;  // shown with the hotkey
    // Progress line you can click or drag to jump anywhere.
    RectF trackR;
    bool hasTrack = false;
    bool trackHover = false;
    bool scrubbing = false;
    float scrubFrac = 0;
    double trackDur = 0;

    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void* bits = nullptr;
    int dibW = 0, dibH = 0;
    bool timerFast = false;
};

static UiState g_ui;

static float EaseOutCubic(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    float u = 1 - t;
    return 1 - u * u * u;
}

static float EaseOutBack(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    const float c1 = 1.1f;
    const float c3 = c1 + 1;
    float u = t - 1;
    return 1 + c3 * u * u * u + c1 * u * u;
}

static bool IsFullscreenBusy() {
    QUERY_USER_NOTIFICATION_STATE st;
    if (SUCCEEDED(SHQueryUserNotificationState(&st))) {
        return st == QUNS_BUSY || st == QUNS_RUNNING_D3D_FULL_SCREEN ||
               st == QUNS_PRESENTATION_MODE;
    }
    return false;
}

static HMONITOR PrimaryMonitor() {
    return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

// Computes geometry for the given monitor. Returns the final pill rect in
// screen coordinates.
static RECT LayoutForMonitor(HMONITOR mon) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(mon, &mi);
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    float s = dpiX / 96.0f * g_s.scalePct / 100.0f;

    g_ui.monitor = mon;
    g_ui.monRect = mi.rcMonitor;
    g_ui.scale = s;
    g_ui.pad = (int)std::ceil(18 * s);
    g_ui.pillW = (int)(g_s.width * s);
    g_ui.pillH = (int)(72 * s);

    const RECT& m = mi.rcMonitor;
    int left;
    if (g_s.position == 1) {
        left = m.left + (int)(g_s.sideMargin * s);
    } else if (g_s.position == 2) {
        left = m.right - (int)(g_s.sideMargin * s) - g_ui.pillW;
    } else {
        left = m.left + (m.right - m.left - g_ui.pillW) / 2;
    }
    left += (int)(g_s.offsetX * s);
    int top = m.top + (int)(g_s.offsetTop * s);

    g_ui.winX = left - g_ui.pad;
    g_ui.winY = m.top;
    g_ui.winW = g_ui.pillW + g_ui.pad * 2;
    g_ui.winH = (top - m.top) + g_ui.pillH + g_ui.pad;

    return RECT{left, top, left + g_ui.pillW, top + g_ui.pillH};
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

static void RoundRectPath(GraphicsPath& p, RectF r, float rad) {
    float d = (std::min)({rad * 2, r.Width, r.Height});
    if (d < 1) {
        p.AddRectangle(r);
        return;
    }
    p.AddArc(r.X, r.Y, d, d, 180, 90);
    p.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    p.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    p.CloseFigure();
}

static void TargetColors(const MediaSnapshot& m,
                         Color& c1,
                         Color& c2,
                         Color& text,
                         Color& accentFg) {
    BYTE op = (BYTE)(g_s.opacityPct * 255 / 100);
    switch (g_s.bgMode) {
        case 1:
            c1 = Color(op, 22, 22, 26);
            c2 = Color(op, 12, 12, 14);
            text = Color(255, 255, 255, 255);
            accentFg = Color(255, 255, 255, 255);
            break;
        case 2: {
            c1 = g_s.solid;
            c2 = Mix(g_s.solid, Color(g_s.solid.GetA(), 0, 0, 0), 0.25f);
            float lum = 0.2126f * g_s.solid.GetR() + 0.7152f * g_s.solid.GetG() +
                        0.0722f * g_s.solid.GetB();
            text = lum > 150 ? Color(255, 20, 20, 24) : Color(255, 255, 255, 255);
            accentFg = text;
            break;
        }
        case 3:
            c1 = Color(op, 248, 248, 250);
            c2 = Color(op, 232, 232, 238);
            text = Color(255, 22, 22, 26);
            accentFg = m.hasAccent ? Mix(m.accent, Color(255, 0, 0, 0), 0.25f)
                                   : Color(255, 40, 40, 48);
            break;
        default: {
            Color a = m.hasAccent ? m.accent : Color(255, 70, 70, 84);
            // Darken the accent so white text always stays readable.
            float mx = (float)(std::max)({a.GetR(), a.GetG(), a.GetB(), (BYTE)1});
            float k = 78.0f / mx;
            Color dark(op, (BYTE)std::clamp(a.GetR() * k, 0.0f, 255.0f),
                       (BYTE)std::clamp(a.GetG() * k, 0.0f, 255.0f),
                       (BYTE)std::clamp(a.GetB() * k, 0.0f, 255.0f));
            c1 = dark;
            c2 = Mix(dark, Color(op, 8, 8, 10), 0.6f);
            text = Color(255, 255, 255, 255);
            accentFg = Mix(a, Color(255, 255, 255, 255), 0.45f);
            break;
        }
    }
}

static void EnsureDib(int w, int h) {
    if (g_ui.memDC && g_ui.dibW == w && g_ui.dibH == h) {
        return;
    }
    if (g_ui.memDC) {
        SelectObject(g_ui.memDC, g_ui.oldBmp);
        DeleteObject(g_ui.dib);
        DeleteDC(g_ui.memDC);
        g_ui.memDC = nullptr;
    }
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_ui.memDC = CreateCompatibleDC(nullptr);
    g_ui.dib = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &g_ui.bits,
                                nullptr, 0);
    g_ui.oldBmp = SelectObject(g_ui.memDC, g_ui.dib);
    g_ui.dibW = w;
    g_ui.dibH = h;
}

static void FreeDib() {
    if (g_ui.memDC) {
        SelectObject(g_ui.memDC, g_ui.oldBmp);
        DeleteObject(g_ui.dib);
        DeleteDC(g_ui.memDC);
        g_ui.memDC = nullptr;
        g_ui.dib = nullptr;
        g_ui.bits = nullptr;
        g_ui.dibW = g_ui.dibH = 0;
    }
}

static FontFamily* g_iconFamily = nullptr;

static const FontFamily* IconFontFamily() {
    if (!g_iconFamily) {
        g_iconFamily = new FontFamily(L"Segoe Fluent Icons");
        if (g_iconFamily->GetLastStatus() != Ok) {
            delete g_iconFamily;
            g_iconFamily = new FontFamily(L"Segoe MDL2 Assets");
        }
    }
    return g_iconFamily;
}

static void DrawIconGlyph(Graphics& g,
                          const wchar_t* glyph,
                          RectF r,
                          float size,
                          Color c) {
    Font font(IconFontFamily(), size, FontStyleRegular, UnitPixel);
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    SolidBrush br(c);
    g.DrawString(glyph, -1, &font, r, &sf, &br);
}

// Text goes through DirectWrite instead of GDI+, because DirectWrite falls
// back to other fonts for characters Segoe UI doesn't have. That's what makes
// fancy Unicode names, symbols and emoji show up instead of empty boxes.
static winrt::com_ptr<ID2D1Factory> g_d2d;
static winrt::com_ptr<IDWriteFactory> g_dwrite;
static winrt::com_ptr<ID2D1DCRenderTarget> g_dcrt;

struct QueuedText {
    winrt::com_ptr<IDWriteTextLayout> layout;
    float x, y;
    Color color;
};
static std::vector<QueuedText> g_textQueue;

static void EnsureTextFactories() {
    if (!g_d2d) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                          __uuidof(ID2D1Factory), nullptr, g_d2d.put_void());
    }
    if (!g_dwrite) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                            reinterpret_cast<::IUnknown**>(g_dwrite.put()));
    }
}

// Lays out a line of text (ellipsis if it's too long) and queues it to be
// drawn. Returns the width it takes up.
static float DrawLabel(Graphics& g,
                       const std::wstring& text,
                       RectF r,
                       bool semibold,
                       float size,
                       Color c) {
    EnsureTextFactories();
    if (!g_dwrite || text.empty() || r.Width <= 0) {
        return 0;
    }
    winrt::com_ptr<IDWriteTextFormat> fmt;
    if (FAILED(g_dwrite->CreateTextFormat(
            L"Segoe UI", nullptr,
            semibold ? DWRITE_FONT_WEIGHT_SEMI_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us",
            fmt.put()))) {
        return 0;
    }
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    winrt::com_ptr<IDWriteInlineObject> ellipsis;
    g_dwrite->CreateEllipsisTrimmingSign(fmt.get(), ellipsis.put());
    fmt->SetTrimming(&trim, ellipsis.get());

    winrt::com_ptr<IDWriteTextLayout> layout;
    if (FAILED(g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                          fmt.get(), r.Width, r.Height,
                                          layout.put()))) {
        return 0;
    }
    DWRITE_TEXT_METRICS tm{};
    layout->GetMetrics(&tm);
    g_textQueue.push_back({layout, r.X, r.Y, c});
    return (std::min)(tm.widthIncludingTrailingWhitespace, r.Width);
}

// Draws the queued text on top of what GDI+ drew, clipped to the bar.
static void FlushText(HDC dc, int w, int h, RectF clip) {
    if (g_textQueue.empty()) {
        return;
    }
    EnsureTextFactories();
    if (g_d2d && !g_dcrt) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);
        g_d2d->CreateDCRenderTarget(&props, g_dcrt.put());
    }
    if (!g_dcrt) {
        g_textQueue.clear();
        return;
    }
    RECT rc{0, 0, w, h};
    if (SUCCEEDED(g_dcrt->BindDC(dc, &rc))) {
        g_dcrt->BeginDraw();
        g_dcrt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        g_dcrt->PushAxisAlignedClip(
            D2D1::RectF(clip.X, clip.Y, clip.X + clip.Width,
                        clip.Y + clip.Height),
            D2D1_ANTIALIAS_MODE_ALIASED);
        for (auto const& t : g_textQueue) {
            winrt::com_ptr<ID2D1SolidColorBrush> brush;
            g_dcrt->CreateSolidColorBrush(
                D2D1::ColorF(t.color.GetR() / 255.0f, t.color.GetG() / 255.0f,
                             t.color.GetB() / 255.0f, t.color.GetA() / 255.0f),
                brush.put());
            if (brush) {
                g_dcrt->DrawTextLayout(D2D1::Point2F(t.x, t.y), t.layout.get(),
                                       brush.get(),
                                       D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            }
        }
        g_dcrt->PopAxisAlignedClip();
        if (g_dcrt->EndDraw() == D2DERR_RECREATE_TARGET) {
            g_dcrt = nullptr;
        }
    }
    g_textQueue.clear();
}

// Circular arrow with "10" in the middle, the usual "skip 10 seconds" icon.
static void DrawSeekIcon(Graphics& g, RectF r, bool forward, float s, Color c) {
    float cx = r.X + r.Width / 2, cy = r.Y + r.Height / 2;
    float rad = 9.5f * s;
    Pen pen(c, 1.7f * s);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    // Angles are clockwise from the right, so 270 is the top.
    float start = forward ? 325.0f : 215.0f;
    float sweep = forward ? 300.0f : -300.0f;
    float endA = (start + sweep) * 3.14159265f / 180.0f;
    g.DrawArc(&pen, cx - rad, cy - rad, rad * 2, rad * 2, start, sweep);
    // Arrow head at the end of the arc, pointing along the direction of travel.
    float ex = cx + rad * std::cos(endA), ey = cy + rad * std::sin(endA);
    float dir = forward ? 1.0f : -1.0f;
    PointF tri[3] = {PointF(ex + dir * 3.4f * s, ey),
                     PointF(ex - dir * 1.2f * s, ey - 3.0f * s),
                     PointF(ex - dir * 1.2f * s, ey + 3.0f * s)};
    SolidBrush br(c);
    g.FillPolygon(&br, tri, 3);
    // "10" label.
    FontFamily fam(L"Segoe UI Semibold");
    FontFamily fallback(L"Segoe UI");
    Font font(fam.GetLastStatus() == Ok ? &fam : &fallback, 7.5f * s,
              FontStyleRegular, UnitPixel);
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(L"10", -1, &font, RectF(cx - rad, cy - rad + 0.5f * s, rad * 2, rad * 2),
                 &sf, &br);
}

// A liquid "neck" joining the top edge of the screen to the bar, so it looks
// like a drop hanging from the edge.
static void AddDripPath(GraphicsPath& p, const RectF& pill, float s) {
    float gap = pill.Y;
    if (gap < 1.5f) {
        return;
    }
    float cx = pill.X + pill.Width / 2;
    float neck = (std::min)(pill.Width * 0.22f, 96 * s);
    float topW = neck * 1.9f;
    float botW = neck * 1.7f;
    float yb = pill.Y + 2 * s;
    p.StartFigure();
    p.AddBezier(PointF(cx - topW / 2, -1), PointF(cx - neck / 2, -1 + gap * 0.25f),
                PointF(cx - neck / 2, yb - gap * 0.35f), PointF(cx - botW / 2, yb));
    p.AddLine(PointF(cx - botW / 2, yb), PointF(cx + botW / 2, yb));
    p.AddBezier(PointF(cx + botW / 2, yb), PointF(cx + neck / 2, yb - gap * 0.35f),
                PointF(cx + neck / 2, -1 + gap * 0.25f), PointF(cx + topW / 2, -1));
    p.CloseFigure();
}

static void Render() {
    const float s = g_ui.scale;
    const int W = g_ui.winW, H = g_ui.winH;
    if (W <= 0 || H <= 0) {
        return;
    }
    EnsureDib(W, H);
    if (!g_ui.bits) {
        return;
    }

    MediaSnapshot m;
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        m = g_media;
    }

    Bitmap canvas(W, H, W * 4, PixelFormat32bppPARGB, (BYTE*)g_ui.bits);
    Graphics g(&canvas);
    g.Clear(Color(0, 0, 0, 0));
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.SetCompositingQuality(CompositingQualityHighQuality);

    // Animation: first drop down, then stretch out to full width.
    float t = g_ui.anim;
    float drop = EaseOutCubic((std::min)(1.0f, t * 1.5f));
    float expand = EaseOutBack((t - 0.2f) / 0.8f);
    float contentA = std::clamp((t - 0.5f) / 0.5f, 0.0f, 1.0f);

    const float fullW = (float)g_ui.pillW;
    const float pillH = (float)g_ui.pillH;
    float curW = (std::max)(pillH, fullW * (0.28f + 0.72f * expand));
    const float pad = (float)g_ui.pad;
    // Final top of the pill inside the window (= the gap from the top edge).
    float finalTop = (float)(g_ui.winH - g_ui.pillH - g_ui.pad);
    float y = -pillH - pad + (finalTop + pillH + pad) * drop;

    float x;
    if (g_s.position == 1) {
        x = pad;
    } else if (g_s.position == 2) {
        x = pad + fullW - curW;
    } else {
        x = pad + (fullW - curW) / 2;
    }

    RectF pill(x, y, curW, pillH);
    float radius = (std::min)((float)g_s.cornerRadius * s, pillH / 2);

    // Colours ease towards their target so track changes blend smoothly.
    Color tc1, tc2, textC, accentFg;
    TargetColors(m, tc1, tc2, textC, accentFg);
    if (!g_ui.bgInit) {
        g_ui.bg1 = ToRgba(tc1);
        g_ui.bg2 = ToRgba(tc2);
        g_ui.bgInit = true;
    } else {
        g_ui.bg1 = Lerp(g_ui.bg1, ToRgba(tc1), 0.08f);
        g_ui.bg2 = Lerp(g_ui.bg2, ToRgba(tc2), 0.08f);
    }
    Color c1 = ToColor(g_ui.bg1), c2 = ToColor(g_ui.bg2);

    // Soft shadow.
    for (int i = 6; i >= 1; i--) {
        float grow = i * 1.6f * s;
        RectF sr(pill.X - grow, pill.Y - grow + 3 * s, pill.Width + grow * 2,
                 pill.Height + grow * 2);
        GraphicsPath sp;
        RoundRectPath(sp, sr, radius + grow);
        SolidBrush sb(Color((BYTE)(7 + (6 - i) * 3), 0, 0, 0));
        g.FillPath(&sb, &sp);
    }

    // Background.
    GraphicsPath path;
    RoundRectPath(path, pill, radius);
    RectF gradR(pill.X - 1, (std::min)(0.0f, pill.Y) - 1, pill.Width + 2,
                pill.Y + pill.Height + 2 - (std::min)(0.0f, pill.Y));
    LinearGradientBrush lb(gradR, c1, c2, LinearGradientModeForwardDiagonal);
    g.FillPath(&lb, &path);
    // Hairline edge for definition on busy wallpapers.
    {
        Pen edge(WithAlpha(textC, 0.10f), 1.0f * s);
        g.DrawPath(&edge, &path);
    }
    if (g_s.drip) {
        GraphicsPath drip;
        AddDripPath(drip, pill, s);
        g.FillPath(&lb, &drip);
    }

    if (contentA <= 0.01f) {
        return;
    }
    g.SetClip(&path);

    // Content is laid out on the final (fully expanded) rect.
    const float X = pad;
    const float Y = y;
    const float W2 = fullW;
    const float H2 = pillH;
    Color textMain = WithAlpha(textC, contentA);
    Color textDim = WithAlpha(textC, contentA * 0.62f);

    // Album art (or app icon if there's no art).
    float artS = H2 - 20 * s;
    RectF artR(X + 10 * s, Y + 10 * s, artS, artS);
    GraphicsPath artPath;
    RoundRectPath(artPath, artR, 13 * s);
    if (m.art) {
        TextureBrush tb(m.art.get(), WrapModeClamp);
        float k = artS / (float)m.art->GetWidth();
        Matrix mat(k, 0, 0, k, artR.X, artR.Y);
        tb.SetTransform(&mat);
        g.FillPath(&tb, &artPath);
    } else {
        SolidBrush ab(WithAlpha(textC, 0.10f * contentA));
        g.FillPath(&ab, &artPath);
        if (m.icon) {
            float is = artS * 0.6f;
            g.DrawImage(m.icon.get(), RectF(artR.X + (artS - is) / 2,
                                            artR.Y + (artS - is) / 2, is, is));
        } else {
            DrawIconGlyph(g, L"\uE8D6", artR, 22 * s, textDim);  // music note
        }
    }
    // App badge on the corner of the art.
    if (m.art && m.icon) {
        float bs = 22 * s;
        RectF br(artR.X + artS - bs + 4 * s, artR.Y + artS - bs + 4 * s, bs, bs);
        GraphicsPath bp;
        RoundRectPath(bp, br, 7 * s);
        SolidBrush bb(Color((BYTE)(230 * contentA), 18, 18, 22));
        g.FillPath(&bb, &bp);
        float is = bs - 6 * s;
        g.DrawImage(m.icon.get(), RectF(br.X + 3 * s, br.Y + 3 * s, is, is));
    }

    // Right side: visualizer, then the controls the app supports.
    float visW = g_s.visualizer == 2 ? 3.5f * s * 5 + 3 * s * 4
                 : g_s.visualizer == 1 ? 44 * s
                                       : 0;
    float visX = X + W2 - 16 * s - visW;
    float btn = 30 * s;
    float ctlRight = g_s.visualizer ? visX - 10 * s : X + W2 - 12 * s;

    g_ui.buttonCount = 0;
    g_ui.hasTrack = false;
    if (m.hasSession) {
        int cmds[5];
        int n = 0;
        if (m.canPrev) cmds[n++] = 1;
        bool seekable = m.canSeek || m.keySeek;
        if (seekable) cmds[n++] = 4;
        cmds[n++] = 2;
        if (seekable) cmds[n++] = 5;
        if (m.canNext) cmds[n++] = 3;
        float bx = ctlRight - btn * n;
        for (int i = 0; i < n; i++) {
            RectF r(bx + btn * i, Y + (H2 - btn) / 2, btn, btn);
            g_ui.buttons[g_ui.buttonCount++] = {cmds[i], r};
            if (g_ui.hoverCmd == cmds[i]) {
                GraphicsPath hp;
                hp.AddEllipse(r);
                SolidBrush hb(WithAlpha(textC, 0.14f * contentA));
                g.FillPath(&hb, &hp);
            }
            switch (cmds[i]) {
                case 1:
                    DrawIconGlyph(g, L"\uE892", r, 12 * s, textMain);
                    break;
                case 3:
                    DrawIconGlyph(g, L"\uE893", r, 12 * s, textMain);
                    break;
                case 2:
                    DrawIconGlyph(g, m.playing ? L"\uE769" : L"\uE768", r,
                                  16 * s, textMain);
                    break;
                case 4:
                case 5:
                    DrawSeekIcon(g, r, cmds[i] == 5, s, textMain);
                    break;
            }
        }
    }
    float ctlLeft = g_ui.buttonCount ? g_ui.buttons[0].r.X : ctlRight;

    if (g_s.visualizer == 2) {
        float barW = 3.5f * s, barGap = 3 * s;
        float maxH = 24 * s, minH = 3.5f * s;
        SolidBrush bb(WithAlpha(accentFg, contentA));
        for (int i = 0; i < 5; i++) {
            float h = minH + (maxH - minH) * g_ui.bars[i];
            RectF r(visX + i * (barW + barGap), Y + (H2 - h) / 2, barW, h);
            GraphicsPath bp;
            RoundRectPath(bp, r, barW / 2);
            g.FillPath(&bb, &bp);
        }
    } else if (g_s.visualizer == 1) {
        // One continuous wave line that swells with the music.
        const int N = 17;
        PointF pts[N];
        double t = std::chrono::duration<double>(
                       SteadyClock::now().time_since_epoch())
                       .count();
        float cy = Y + H2 / 2;
        float amp = 12 * s;
        for (int i = 0; i < N; i++) {
            float u = (float)i / (N - 1);
            // Blend the five level values across the width.
            float f = u * 4;
            int k = (std::min)(3, (int)f);
            float e = g_ui.bars[k] + (g_ui.bars[k + 1] - g_ui.bars[k]) * (f - k);
            float taper = (float)std::sin(u * 3.14159265);
            float wv = (float)std::sin(t * 7.0 + i * 1.15) * 0.75f +
                       (float)std::sin(t * 11.0 - i * 0.7) * 0.25f;
            pts[i] = PointF(visX + u * visW, cy + wv * e * amp * taper);
        }
        Pen wp(WithAlpha(accentFg, contentA), 2.0f * s);
        wp.SetStartCap(LineCapRound);
        wp.SetEndCap(LineCapRound);
        wp.SetLineJoin(LineJoinRound);
        g.DrawCurve(&wp, pts, N, 0.5f);
    }

    // Text.
    float tx = artR.X + artS + 12 * s;
    float tw = ctlLeft - 8 * s - tx;
    std::wstring title = m.hasSession
                             ? (m.title.empty() ? L"Unknown title" : m.title)
                             : L"Nothing playing";
    std::wstring sub = m.hasSession ? m.artist : L"Play something to see it here";
    bool hasProgress = m.hasSession && m.durSec > 1;
    float titleY = Y + (hasProgress ? 11 : 15) * s;
    DrawLabel(g, title, RectF(tx, titleY, tw, 20 * s), true, 14.5f * s,
              textMain);
    float subW = 0;
    float linkS = 18 * s;
    if (!sub.empty()) {
        float room = m.hasSession ? tw - linkS - 4 * s : tw;
        subW = DrawLabel(g, sub, RectF(tx, titleY + 19 * s, room, 18 * s),
                         false, 12.5f * s, textDim);
    }
    // Small "open" button after the artist: brings the player to the front.
    if (m.hasSession && g_ui.buttonCount < 6) {
        RectF lr(tx + subW + (subW > 0 ? 3 * s : 0), titleY + 19 * s, linkS,
                 linkS);
        g_ui.buttons[g_ui.buttonCount++] = {6, lr};
        if (g_ui.hoverCmd == 6) {
            GraphicsPath hp;
            RoundRectPath(hp, lr, 5 * s);
            SolidBrush hb(WithAlpha(textC, 0.16f * contentA));
            g.FillPath(&hb, &hp);
        }
        DrawIconGlyph(g, L"\uE8A7", lr, 10.5f * s,
                      g_ui.hoverCmd == 6 ? textMain : textDim);
    }

    if (hasProgress) {
        double pos = m.posSec;
        if (m.playing) {
            pos += std::chrono::duration<double>(SteadyClock::now() - m.posStamp)
                       .count();
        }
        float frac = (float)std::clamp(pos / m.durSec, 0.0, 1.0);
        if (g_ui.scrubbing) {
            frac = g_ui.scrubFrac;
        }
        bool active = g_ui.scrubbing || g_ui.trackHover;
        float ph = (active ? 5 : 3) * s;
        float py = Y + H2 - 14.5f * s - ph / 2;
        RectF track(tx, py, tw, ph);
        // Generous hit area so it's easy to grab.
        g_ui.trackR = RectF(tx - 4 * s, py - 7 * s, tw + 8 * s, ph + 14 * s);
        g_ui.hasTrack = true;
        g_ui.trackDur = m.durSec;
        GraphicsPath tp;
        RoundRectPath(tp, track, ph / 2);
        SolidBrush tbr(WithAlpha(textC, 0.18f * contentA));
        g.FillPath(&tbr, &tp);
        if (frac > 0.005f) {
            RectF fill(tx, py, (std::max)(ph, tw * frac), ph);
            GraphicsPath fp;
            RoundRectPath(fp, fill, ph / 2);
            SolidBrush fbr(WithAlpha(accentFg, contentA));
            g.FillPath(&fbr, &fp);
        }
        if (active) {
            float kr = 6 * s;
            float kx = tx + tw * frac;
            GraphicsPath kp;
            kp.AddEllipse(kx - kr, py + ph / 2 - kr, kr * 2, kr * 2);
            SolidBrush kb(WithAlpha(textC, contentA));
            g.FillPath(&kb, &kp);
        }
    }

    g.ResetClip();
    g.Flush(FlushIntentionSync);
    FlushText(g_ui.memDC, W, H, pill);
}

static void Present() {
    POINT dst{g_ui.winX, g_ui.winY};
    SIZE sz{g_ui.winW, g_ui.winH};
    POINT src{0, 0};
    float a = (std::min)(1.0f, g_ui.anim * 3.0f);
    BLENDFUNCTION bf{AC_SRC_OVER, 0, (BYTE)(a * 255), AC_SRC_ALPHA};
    HDC screen = GetDC(nullptr);
    UpdateLayeredWindow(g_hwnd, screen, &dst, &sz, g_ui.memDC, &src, 0, &bf,
                        ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

////////////////////////////////////////////////////////////////////////////////
// Behaviour

static void SetTimerSpeed(bool fast) {
    if (g_ui.timerFast == fast) {
        return;
    }
    g_ui.timerFast = fast;
    SetTimer(g_hwnd, 1, fast ? 15 : 45, nullptr);
}

static void BeginShow(HMONITOR mon) {
    if (!g_ui.windowVisible) {
        Wh_Log(L"Showing bar");
    }
    if (g_ui.target != Phase::Shown || !g_ui.windowVisible) {
        LayoutForMonitor(mon);
    }
    g_ui.target = Phase::Shown;
    g_ui.lastInside = SteadyClock::now();
    if (!g_ui.windowVisible) {
        g_ui.windowVisible = true;
        Render();
        Present();
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    }
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetTimerSpeed(true);
}

static void UpdateBars(bool playing, float dt) {
    float peak = g_s.visualizer ? GetAudioPeak() : 0;
    if (!playing) {
        peak = 0;
    }
    float level = std::pow(std::clamp(peak, 0.0f, 1.0f), 0.6f);
    double now = std::chrono::duration<double>(
                     SteadyClock::now().time_since_epoch())
                     .count();
    for (int i = 0; i < 5; i++) {
        float wave = 0.5f + 0.5f * (float)std::sin(now * (5.1 + i * 1.73) +
                                                   i * 1.9);
        float wave2 = 0.5f + 0.5f * (float)std::sin(now * (8.3 - i * 0.9) +
                                                    i * 0.7);
        float shape = i == 2 ? 1.0f : (i == 1 || i == 3) ? 0.85f : 0.65f;
        float target = level * shape * (0.35f + 0.45f * wave + 0.2f * wave2);
        float k = target > g_ui.bars[i] ? 0.55f : 0.18f;
        k = 1 - std::pow(1 - k, dt * 60);
        g_ui.bars[i] += (target - g_ui.bars[i]) * k;
    }
}

static void Tick() {
    auto now = SteadyClock::now();
    float dt = g_ui.lastFrame.time_since_epoch().count() == 0
                   ? 0.016f
                   : std::chrono::duration<float>(now - g_ui.lastFrame).count();
    dt = std::clamp(dt, 0.0f, 0.1f);
    g_ui.lastFrame = now;

    bool hasSession, playing;
    unsigned serial;
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        hasSession = g_media.hasSession;
        playing = g_media.playing;
        serial = g_media.trackSerial;
    }
    bool hasContent = hasSession || g_s.showIdle;
    bool blocked = g_s.hideFullscreen && IsFullscreenBusy();

    POINT pt;
    GetCursorPos(&pt);

    HMONITOR curMon = g_s.allMonitors
                          ? MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST)
                          : PrimaryMonitor();

    // Peek when a new track starts.
    if (!g_ui.serialInit) {
        g_ui.seenSerial = serial;
        g_ui.serialInit = true;
    } else if (serial != g_ui.seenSerial) {
        g_ui.seenSerial = serial;
        if (g_s.peek && hasSession && !blocked) {
            BeginShow(g_ui.windowVisible ? g_ui.monitor : curMon);
            g_ui.peekUntil = now + std::chrono::milliseconds(g_s.peekMs);
        }
    }

    // Hover trigger at the top edge.
    bool trigger = false;
    if (!blocked && hasContent) {
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(curMon, &mi);
        if (pt.y <= mi.rcMonitor.top + 1 && pt.x >= mi.rcMonitor.left &&
            pt.x < mi.rcMonitor.right) {
            if (g_s.wideTrigger) {
                trigger = true;
            } else {
                // Only the strip above where the bar will appear.
                UiState saved = g_ui;
                RECT r = LayoutForMonitor(curMon);
                int slack = (int)(24 * g_ui.scale);
                trigger = pt.x >= r.left - slack && pt.x <= r.right + slack;
                // Restore geometry if the bar is currently shown elsewhere.
                if (g_ui.windowVisible) {
                    g_ui.monitor = saved.monitor;
                    g_ui.monRect = saved.monRect;
                    g_ui.scale = saved.scale;
                    g_ui.pad = saved.pad;
                    g_ui.pillW = saved.pillW;
                    g_ui.pillH = saved.pillH;
                    g_ui.winX = saved.winX;
                    g_ui.winY = saved.winY;
                    g_ui.winW = saved.winW;
                    g_ui.winH = saved.winH;
                }
            }
        }
    }

    {
        // Diagnostics: log when the cursor reaches the top edge.
        static bool wasAtTop = false;
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(curMon, &mi);
        bool atTop = pt.y <= mi.rcMonitor.top + 1;
        if (atTop && !wasAtTop) {
            Wh_Log(L"Top edge: cursor=%d,%d monTop=%d session=%d blocked=%d "
                   L"trigger=%d",
                   pt.x, pt.y, mi.rcMonitor.top, (int)hasSession, (int)blocked,
                   (int)trigger);
        }
        wasAtTop = atTop;
    }

    if (trigger) {
        if (!g_ui.inTrigger) {
            g_ui.inTrigger = true;
            g_ui.triggerSince = now;
        }
        if (now - g_ui.triggerSince >=
            std::chrono::milliseconds(g_s.revealDelay)) {
            if (g_ui.target != Phase::Shown) {
                BeginShow(curMon);
            }
            g_ui.lastInside = now;
        }
    } else {
        g_ui.inTrigger = false;
    }

    // Games can push themselves on top, so keep the hotkey bar above them.
    if (g_ui.pinned && g_ui.windowVisible) {
        static SteadyClock::time_point lastTop{};
        if (now - lastTop > std::chrono::milliseconds(500)) {
            lastTop = now;
            SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    if (g_s.alwaysVisible && hasSession && playing && !blocked &&
        g_ui.target != Phase::Shown) {
        BeginShow(PrimaryMonitor());
    }

    // Hide logic.
    if (g_ui.target == Phase::Shown && !g_ui.pinned) {
        int slack = (int)(8 * g_ui.scale);
        bool inside = pt.x >= g_ui.winX - slack &&
                      pt.x < g_ui.winX + g_ui.winW + slack &&
                      pt.y >= g_ui.winY && pt.y < g_ui.winY + g_ui.winH + slack;
        if (inside) {
            g_ui.lastInside = now;
        }
        bool keep = inside || trigger || now < g_ui.peekUntil ||
                    (g_s.alwaysVisible && hasSession && playing);
        bool expired =
            now - g_ui.lastInside > std::chrono::milliseconds(g_s.hideDelay);
        if (blocked || !hasContent || (!keep && expired)) {
            g_ui.target = Phase::Hidden;
            g_ui.peekUntil = {};
        }
    }

    if (!g_ui.windowVisible) {
        SetTimerSpeed(false);
        return;
    }

    // Hover highlight on buttons.
    POINT cp = pt;
    cp.x -= g_ui.winX;
    cp.y -= g_ui.winY;
    PointF p((REAL)cp.x, (REAL)cp.y);
    g_ui.hoverCmd = 0;
    for (int i = 0; i < g_ui.buttonCount; i++) {
        if (g_ui.buttons[i].r.Contains(p)) {
            g_ui.hoverCmd = g_ui.buttons[i].cmd;
        }
    }
    g_ui.trackHover = g_ui.hasTrack && g_ui.trackR.Contains(p);

    float step = dt * 1000.0f / (float)g_s.animMs;
    if (g_ui.target == Phase::Shown) {
        g_ui.anim = (std::min)(1.0f, g_ui.anim + step);
    } else {
        g_ui.anim = (std::max)(0.0f, g_ui.anim - step * 1.15f);
    }

    UpdateBars(playing, dt);
    Render();
    Present();

    if (g_ui.target == Phase::Hidden && g_ui.anim <= 0) {
        ShowWindow(g_hwnd, SW_HIDE);
        g_ui.windowVisible = false;
        g_ui.hoverCmd = 0;
        SetTimerSpeed(false);
    }
}

static float ScrubFracAt(float x) {
    // trackR has 4 px (scaled) of padding on each side of the line.
    float pad = 4 * g_ui.scale;
    float w = g_ui.trackR.Width - pad * 2;
    if (w <= 0) {
        return 0;
    }
    return std::clamp((x - g_ui.trackR.X - pad) / w, 0.0f, 1.0f);
}

static const int kHotkeyId = 0x4E50;  // "NP"

static void RegisterToggleHotkey() {
    UnregisterHotKey(g_hwnd, kHotkeyId);
    if (g_s.hotkeyVk) {
        if (!RegisterHotKey(g_hwnd, kHotkeyId, g_s.hotkeyMods | MOD_NOREPEAT,
                            g_s.hotkeyVk)) {
            Wh_Log(L"Hotkey is already used by another app: %u",
                   GetLastError());
        } else {
            Wh_Log(L"Hotkey registered");
        }
    }
}

// Persistent mode (hotkey): show instantly with no slide, press again to hide. Works in games
// and over full-screen apps.
static void ToggleWithHotkey() {
    if (g_ui.pinned && g_ui.windowVisible) {
        g_ui.pinned = false;
        g_ui.target = Phase::Hidden;
        g_ui.anim = 0;
        ShowWindow(g_hwnd, SW_HIDE);
        g_ui.windowVisible = false;
        g_ui.hoverCmd = 0;
        SetTimerSpeed(false);
        return;
    }
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR mon = g_s.allMonitors
                       ? MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST)
                       : PrimaryMonitor();
    g_ui.pinned = true;
    g_ui.peekUntil = {};
    LayoutForMonitor(mon);
    g_ui.target = Phase::Shown;
    g_ui.anim = 1;
    g_ui.lastFrame = SteadyClock::now();
    g_ui.windowVisible = true;
    Render();
    Present();
    ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetTimerSpeed(true);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            if (g_stop) {
                DestroyWindow(hwnd);
                return 0;
            }
            if (wp == 1) {
                Tick();
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_HOTKEY:
            if ((int)wp == kHotkeyId) {
                Wh_Log(L"Hotkey pressed");
                ToggleWithHotkey();
            }
            return 0;
        case WM_LBUTTONDOWN: {
            PointF p((REAL)(short)LOWORD(lp), (REAL)(short)HIWORD(lp));
            if (g_ui.hasTrack && g_ui.trackR.Contains(p)) {
                g_ui.scrubbing = true;
                g_ui.scrubFrac = ScrubFracAt(p.X);
                SetCapture(hwnd);
            }
            return 0;
        }
        case WM_MOUSEMOVE:
            if (g_ui.scrubbing) {
                g_ui.scrubFrac = ScrubFracAt((REAL)(short)LOWORD(lp));
                g_ui.lastInside = SteadyClock::now();
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_ui.scrubbing = false;
            return 0;
        case WM_LBUTTONUP: {
            PointF p((REAL)(short)LOWORD(lp), (REAL)(short)HIWORD(lp));
            if (g_ui.scrubbing) {
                float frac = ScrubFracAt(p.X);
                g_ui.scrubbing = false;
                ReleaseCapture();
                if (g_ui.trackDur > 1) {
                    g_seekTo = frac * g_ui.trackDur;
                    SendMediaCommand(7);
                    // Move the line right away instead of waiting for the app.
                    std::lock_guard<std::mutex> lock(g_mediaMutex);
                    g_media.posSec = g_seekTo.load();
                    g_media.posStamp = SteadyClock::now();
                }
                g_ui.lastInside = SteadyClock::now();
                return 0;
            }
            for (int i = 0; i < g_ui.buttonCount; i++) {
                if (g_ui.buttons[i].r.Contains(p)) {
                    int cmd = g_ui.buttons[i].cmd;
                    bool useKeys = false;
                    std::wstring title;
                    {
                        std::lock_guard<std::mutex> lock(g_mediaMutex);
                        useKeys = false;
                        title = g_media.title;
                    }
                    if (cmd == 6) {
                        std::wstring aumid;
                        {
                            std::lock_guard<std::mutex> lock(g_mediaMutex);
                            aumid = g_media.aumid;
                        }
                        FocusPlayer(title, aumid);
                    } else if (useKeys) {
                        KeyboardSeek(title, cmd == 5);
                    } else {
                        SendMediaCommand(cmd);
                    }
                    break;
                }
            }
            g_ui.lastInside = SteadyClock::now();
            g_ui.peekUntil = {};
            return 0;
        }
        case WM_APP_SETTINGS:
            LoadSettings();
            RegisterToggleHotkey();
            g_ui.bgInit = false;
            if (g_ui.windowVisible) {
                LayoutForMonitor(g_ui.monitor);
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            UnregisterHotKey(hwnd, kHotkeyId);
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static void UiThreadProc() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    HMODULE self = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&WndProc, &self);
    wc.hInstance = self;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"NowPlaying_" WH_MOD_ID;
    RegisterClassEx(&wc);

    g_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"Now Playing...", WS_POPUP, 0, 0, 1, 1, nullptr,
        nullptr, wc.hInstance, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        CoUninitialize();
        return;
    }

    LayoutForMonitor(PrimaryMonitor());
    RegisterToggleHotkey();
    Wh_Log(L"Window created, UI running");
    g_ui.timerFast = true;
    SetTimerSpeed(false);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_textQueue.clear();
    g_dcrt = nullptr;
    g_dwrite = nullptr;
    g_d2d = nullptr;
    FreeDib();
    delete g_iconFamily;
    g_iconFamily = nullptr;
    g_meter = nullptr;
    g_hwnd = nullptr;
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
}

////////////////////////////////////////////////////////////////////////////////
// Startup: the mod loads into explorer.exe. Only the explorer process that owns
// the taskbar runs the bar, so folder windows in other explorer processes
// don't create duplicates.

static std::thread g_starterThread;

static bool IsTaskbarProcess() {
    HWND tray = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!tray) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(tray, &pid);
    return pid == GetCurrentProcessId();
}

static void StarterThreadProc() {
    // Explorer may still be starting up: wait for the taskbar for up to a
    // minute.
    for (int i = 0; i < 600 && !g_stop; i++) {
        HWND tray = FindWindow(L"Shell_TrayWnd", nullptr);
        if (tray) {
            if (!IsTaskbarProcess()) {
                Wh_Log(L"Not the taskbar's explorer process, staying idle");
                return;
            }
            break;
        }
        Sleep(100);
    }
    if (g_stop || !IsTaskbarProcess()) {
        Wh_Log(L"Taskbar not found in this process, staying idle");
        return;
    }

    Wh_Log(L"Taskbar process found, starting the bar");
    g_mediaThread = std::thread(MediaThreadProc);
    g_uiThread = std::thread(UiThreadProc);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Now Playing... starting");
    LoadSettings();

    GdiplusStartupInput gsi;
    if (GdiplusStartup(&g_gdiplusToken, &gsi, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return FALSE;
    }

    g_stop = false;
    g_starterThread = std::thread(StarterThreadProc);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    // The UI thread reloads settings itself, so it never reads them mid-change.
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_APP_SETTINGS, 0, 0);
    } else {
        LoadSettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Now Playing... stopping");
    g_stop = true;
    g_wakeCv.notify_all();
    if (g_starterThread.joinable()) {
        g_starterThread.join();
    }
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_CLOSE, 0, 0);
    }
    if (g_uiThread.joinable()) {
        g_uiThread.join();
    }
    if (g_mediaThread.joinable()) {
        g_mediaThread.join();
    }
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        g_media = MediaSnapshot{};
    }
    GdiplusShutdown(g_gdiplusToken);
}
