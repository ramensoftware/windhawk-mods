// ==WindhawkMod==
// @id            taskbar-media-player
// @name          Taskbar Media Player
// @description   A sleek, floating media player on your taskbar with native volume and playback controls.
// @version       5.8.0
// @author        GR0UD
// @github        https://github.com/GR0UD
// @license       MIT
// @include       explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## 💎 Taskbar Music Player
Clean floating music player that sits on your windows taskbar. shows what’s playing and lets you pause skip and change volume without opening anything. works with spotify youtube chrome edge etc.

## ⚠️ Requirements
- widgets turned off
- windows 11

## ✨ Key Features
changes look with album art (gradients blur or transparent)
clean simple design
hover speaker for volume slider
can hide speaker if you want it minimal
customizable size position colors etc
auto hides in games or when paused

## 📐 Layout
[ 🖼️ ]  [ Song Title - Artist ]  [ 🔊 ]  ⏮️  ▶️  ⏭️

## 🙌 Credits
Big ups to **hashah2311** its a remake of his original taskbar music lounge by hashah2311.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- GeneralGroup:
  - Theme: gradient_lr
    $name: Theme
    $options:
      - windows: Windows Adaptive (Matches Taskbar)
      - transparent: Transparent
      - blurred: Acrylic Blur (Standard)
      - glass: Frosted Glass (Premium Light Blur)
      - neon: Neon Glass (Blur + Border)
      - gradient: "Gradient: Fade to Transparent"
      - gradient_lr: "Gradient: Left to Right"
      - gradient_rl: "Gradient: Right to Left"
      - gradient_vertical: "Gradient: Top to Bottom"
      - gradient_vertical_inv: "Gradient: Bottom to Top"
      - gradient_radial: "Gradient: Radial Glow"
      - split: Sharp Split (Two-Tone)
      - aurora: Aurora Glow (Tri-Color)
  - ShowSpeakerIcon: true
    $name: Enable Volume Control
    $description: Shows the speaker icon and hoverable volume slider. Turn off for a cleaner aesthetic. Hover and scroll or use your mouse to pull it.
  - HideFullscreen: false
    $name: Hide on Fullscreen
    $description: Automatically hide when a fullscreen app or game is running
  - IdleTimeout: 0
    $name: Idle Timeout (seconds)
    $description: Hide after this many seconds of paused playback. 0 = never.
  $name: General & Behavior

- StyleGroup:
  - AutoTheme: true
    $name: Auto Theme (Text Color)
    $description: Use black or white text automatically based on the Windows light/dark setting
  - TextColor: FFFFFF
    $name: Custom Text Color
    $description: Hex RGB when Auto Theme is off (e.g. FFFFFF for white)
  - DynamicHover: true
    $name: Dynamic Hover Color
    $description: Automatically use the album cover's primary color for hover effects. If turned off, it falls back to your custom Hex color below.
  - HoverColor: 1ED760
    $name: Custom Hover Color
    $description: "Hex RGB color for the primary button hover (default is Spotify Green: 1ED760)"
  - RoundedCorners: true
    $name: Rounded Corners
    $description: Enable rounded corners on the panel
  - CornerRadius: 10
    $name: Corner Radius
    $description: Roundness in pixels when Rounded Corners is on (2–32)
  - ShowBorder: false
    $name: Show Window Border
    $description: Toggle the default Windows 11 thin outline border around the panel.
  $name: Colors & Styling

- LayoutGroup:
  - PanelWidth: 360
    $name: Panel Width
    $description: Width of the music panel in pixels (min 150)
  - PanelHeight: 52
    $name: Panel Height
    $description: Height of the music panel in pixels (min 32)
  - FontSize: 11
    $name: Font Size
    $description: Title text size in pixels
  - OffsetX: 12
    $name: Offset X
    $description: Horizontal offset from the left edge of the taskbar
  - OffsetY: 0
    $name: Offset Y
    $description: Vertical offset from the taskbar centre (negative = up)
  - PadLeft: 8
    $name: Padding Left
    $description: Left edge spacing (Set to 0 to flush the album art against the left edge)
  - PadRight: 12
    $name: Padding Right
    $description: Right edge spacing (Pushes the media buttons inwards)
  - PadTop: 8
    $name: Padding Top
    $description: Top spacing (0 = album art touches the top of the panel)
  - PadBottom: 8
    $name: Padding Bottom
    $description: Bottom spacing (0 = album art touches the bottom of the panel)
  $name: Layout & Sizing
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <algorithm>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

const WCHAR* FONT_NAME = L"Segoe UI Variable Display";
const WCHAR* ICON_FONT = L"Segoe MDL2 Assets";

// ── Shared Layout Math ───────────────────────────────────────────────────────
struct ControlLayout { float nX, ppX, pX, vX, cY; };

inline ControlLayout CalcLayout(int W, int H, int padRight) {
    float cY  = H / 2.f;
    float nX  = W - (float)padRight - 12.f;
    float ppX = nX - 34.f;
    float pX  = ppX - 34.f;
    float vX  = pX  - 34.f;
    return { nX, ppX, pX, vX, cY };
}

// ── DWM / Accent ─────────────────────────────────────────────────────────────
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTBACKDROP = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
} ACCENT_STATE;
typedef struct { ACCENT_STATE AccentState; DWORD AccentFlags; DWORD GradientColor; DWORD AnimationId; } ACCENT_POLICY;
typedef struct { WINDOWCOMPOSITIONATTRIB Attribute; PVOID Data; SIZE_T SizeOfData; } WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// ── Z-Band ───────────────────────────────────────────────────────────────────
enum ZBID { ZBID_IMMERSIVE_NOTIFICATION = 4 };
typedef HWND(WINAPI* pCreateWindowInBand)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID,DWORD);

// ── Settings ─────────────────────────────────────────────────────────────────
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif
#ifndef DWMWA_COLOR_DEFAULT
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
#endif

struct ModSettings {
    int   width         = 360;
    int   height        = 52;
    int   fontSize      = 11;
    int   offsetX       = 12;
    int   offsetY       = 0;
    int   padLeft       = 8;
    int   padRight      = 12;
    int   padTop        = 8;
    int   padBottom     = 8;
    int   background    = 4;
    bool  roundedCorners = true;
    int   cornerRadius  = 10;
    bool  showBorder    = false;
    bool  autoTheme     = true;
    bool  dynamicHover  = true;
    DWORD manualText    = 0xFFFFFFFF;
    DWORD hoverColor    = 0xFF1ED760;
    bool  showSpeakerIcon = true;
    bool  hideFullscreen = false;
    int   idleTimeout   = 0;
} g_S;

void LoadSettings() {
    g_S.width          = Wh_GetIntSetting(L"LayoutGroup.PanelWidth");
    g_S.height         = Wh_GetIntSetting(L"LayoutGroup.PanelHeight");
    g_S.fontSize       = Wh_GetIntSetting(L"LayoutGroup.FontSize");
    g_S.offsetX        = Wh_GetIntSetting(L"LayoutGroup.OffsetX");
    g_S.offsetY        = Wh_GetIntSetting(L"LayoutGroup.OffsetY");
    
    g_S.padLeft        = Wh_GetIntSetting(L"LayoutGroup.PadLeft");
    g_S.padRight       = Wh_GetIntSetting(L"LayoutGroup.PadRight");
    g_S.padTop         = Wh_GetIntSetting(L"LayoutGroup.PadTop");
    g_S.padBottom      = Wh_GetIntSetting(L"LayoutGroup.PadBottom");

    PCWSTR themeStr = Wh_GetStringSetting(L"GeneralGroup.Theme");
    if (themeStr) {
        if      (!wcscmp(themeStr, L"windows"))               g_S.background = 0;
        else if (!wcscmp(themeStr, L"transparent"))           g_S.background = 1;
        else if (!wcscmp(themeStr, L"blurred"))               g_S.background = 2;
        else if (!wcscmp(themeStr, L"gradient"))              g_S.background = 3;
        else if (!wcscmp(themeStr, L"gradient_lr"))           g_S.background = 4;
        else if (!wcscmp(themeStr, L"gradient_rl"))           g_S.background = 12; 
        else if (!wcscmp(themeStr, L"gradient_vertical"))     g_S.background = 5;
        else if (!wcscmp(themeStr, L"gradient_radial"))       g_S.background = 6;
        else if (!wcscmp(themeStr, L"gradient_vertical_inv")) g_S.background = 7;
        else if (!wcscmp(themeStr, L"glass"))                 g_S.background = 8;
        else if (!wcscmp(themeStr, L"split"))                 g_S.background = 9;
        else if (!wcscmp(themeStr, L"neon"))                  g_S.background = 10;
        else if (!wcscmp(themeStr, L"aurora"))                g_S.background = 11;
        else                                                  g_S.background = 4;
        Wh_FreeStringSetting(themeStr);
    }

    g_S.showSpeakerIcon = Wh_GetIntSetting(L"GeneralGroup.ShowSpeakerIcon") != 0;
    g_S.hideFullscreen  = Wh_GetIntSetting(L"GeneralGroup.HideFullscreen") != 0;
    g_S.idleTimeout     = Wh_GetIntSetting(L"GeneralGroup.IdleTimeout");

    g_S.roundedCorners  = Wh_GetIntSetting(L"StyleGroup.RoundedCorners") != 0;
    g_S.cornerRadius    = Wh_GetIntSetting(L"StyleGroup.CornerRadius");
    g_S.showBorder      = Wh_GetIntSetting(L"StyleGroup.ShowBorder") != 0;
    g_S.autoTheme       = Wh_GetIntSetting(L"StyleGroup.AutoTheme") != 0;
    g_S.dynamicHover    = Wh_GetIntSetting(L"StyleGroup.DynamicHover") != 0;
    
    PCWSTR hex = Wh_GetStringSetting(L"StyleGroup.TextColor");
    DWORD rgb = 0xFFFFFFFF;
    if (hex) { if (wcslen(hex) > 0) rgb = wcstoul(hex, nullptr, 16); Wh_FreeStringSetting(hex); }
    g_S.manualText = 0xFF000000 | rgb;

    PCWSTR hoverHex = Wh_GetStringSetting(L"StyleGroup.HoverColor");
    DWORD hoverRgb = 0x1ED760; 
    if (hoverHex) { if (wcslen(hoverHex) > 0) hoverRgb = wcstoul(hoverHex, nullptr, 16); Wh_FreeStringSetting(hoverHex); }
    g_S.hoverColor = 0xFF000000 | hoverRgb;

    if (g_S.width  < 150) g_S.width  = 360;
    if (g_S.height < 32)  g_S.height = 52;
    if (g_S.cornerRadius < 2)  g_S.cornerRadius = 2;
    if (g_S.cornerRadius > 32) g_S.cornerRadius = 32;
    if (g_S.background < 0 || g_S.background > 12) g_S.background = 4;
    if (g_S.fontSize < 7) g_S.fontSize = 11;
    if (g_S.padLeft < 0) g_S.padLeft = 0;
    if (g_S.padRight < 0) g_S.padRight = 0;
    if (g_S.padTop < 0) g_S.padTop = 0;
    if (g_S.padBottom < 0) g_S.padBottom = 0;
}

// ── Globals ───────────────────────────────────────────────────────────────────
HWND          g_hWnd             = NULL;
int           g_Hover            = 0;    // 0=none 1=prev 2=pp 3=next 4=volume
bool          g_VolumeHover      = false;
HWINEVENTHOOK g_TbHook           = nullptr;
UINT          g_TbCreatedMsg     = RegisterWindowMessage(L"TaskbarCreated");
int           g_IdleSecs         = 0;
bool          g_IdleHide         = false;
bool          g_AnimTimerRunning = false;

// Hover Animation States
float         g_HoverProgress[5] = {0.f};
bool          g_HoverAnimRunning = false;
#define IDT_HOVER_ANIM 1003

// ── Volume ────────────────────────────────────────────────────────────────────
float g_Volume = 0.5f;
std::wstring g_currentMediaAppAumid;

void ExecuteAppVolumeCommand(float* getVol, float setVol) {
    if (g_currentMediaAppAumid.empty()) return;

    com_ptr<IMMDeviceEnumerator> pEnumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator)))) return;

    com_ptr<IMMDevice> pDevice;
    if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, pDevice.put()))) return;

    com_ptr<IAudioSessionManager2> pSessionManager;
    if (FAILED(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(pSessionManager.put())))) return;

    com_ptr<IAudioSessionEnumerator> pSessionEnumerator;
    if (FAILED(pSessionManager->GetSessionEnumerator(pSessionEnumerator.put()))) return;

    int sessionCount = 0;
    pSessionEnumerator->GetCount(&sessionCount);

    for (int i = 0; i < sessionCount; i++) {
        com_ptr<IAudioSessionControl> pSessionControl;
        if (SUCCEEDED(pSessionEnumerator->GetSession(i, pSessionControl.put()))) {
            com_ptr<IAudioSessionControl2> pSessionControl2 = pSessionControl.as<IAudioSessionControl2>();
            if (pSessionControl2) {
                DWORD processId = 0;
                pSessionControl2->GetProcessId(&processId);
                if (processId != 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
                    if (hProcess) {
                        WCHAR processImagePath[MAX_PATH];
                        DWORD pathSize = MAX_PATH;
                        if (QueryFullProcessImageNameW(hProcess, 0, processImagePath, &pathSize)) {
                            wstring wPath = processImagePath;
                            size_t lastSlash = wPath.find_last_of(L"\\");
                            wstring exeName = (lastSlash == wstring::npos) ? wPath : wPath.substr(lastSlash + 1);
                            size_t dot = exeName.find_last_of(L".");
                            if (dot != wstring::npos) exeName = exeName.substr(0, dot);
                            
                            wstring lowerAumid = g_currentMediaAppAumid;
                            wstring lowerExe = exeName;
                            transform(lowerAumid.begin(), lowerAumid.end(), lowerAumid.begin(), ::towlower);
                            transform(lowerExe.begin(), lowerExe.end(), lowerExe.begin(), ::towlower);

                            if (lowerAumid.find(lowerExe) != wstring::npos || lowerExe.find(lowerAumid) != wstring::npos) {
                                com_ptr<ISimpleAudioVolume> pAppVol = pSessionControl2.as<ISimpleAudioVolume>();
                                if (pAppVol) {
                                    if (getVol) pAppVol->GetMasterVolume(getVol);
                                    if (setVol >= 0.0f) pAppVol->SetMasterVolume(setVol, NULL);
                                }
                                CloseHandle(hProcess);
                                return;
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }
            }
        }
    }
}

void FetchAppVolume() { ExecuteAppVolumeCommand(&g_Volume, -1.0f); }
void SetVolume(float level) { g_Volume = max(0.f, min(1.f, level)); ExecuteAppVolumeCommand(nullptr, g_Volume); }

// ── Media ─────────────────────────────────────────────────────────────────────
struct MediaSnap {
    wstring title  = L"No Media";
    wstring artist = L"";
    bool    playing = false;
    bool    hasMedia = false;
    std::unique_ptr<Bitmap> art;
    Color   primaryColor   = Color(255, 18, 18, 18);
    Color   secondaryColor = Color(255, 45, 45, 45);
    mutex   mtx;
} g_M;

GlobalSystemMediaTransportControlsSessionManager g_Mgr = nullptr;

struct AlbumPalette { Color primary; Color secondary; };

AlbumPalette GetAlbumPalette(Bitmap* bmp) {
    const Color fallbackPrimary(255, 18, 18, 18);
    const Color fallbackSecondary(255, 45, 45, 45);
    
    if (!bmp || bmp->GetLastStatus() != Ok) 
        return { fallbackPrimary, fallbackSecondary };
    
    UINT w = bmp->GetWidth(), h = bmp->GetHeight();
    if (w == 0 || h == 0) return { fallbackPrimary, fallbackSecondary };

    BitmapData data;
    Rect r(0, 0, (INT)w, (INT)h);
    if (bmp->LockBits(&r, ImageLockModeRead, PixelFormat32bppARGB, &data) != Ok)
        return { fallbackPrimary, fallbackSecondary };

    long long r1=0,g1=0,b1=0,s1=0;
    long long r2=0,g2=0,b2=0,s2=0;
    DWORD* pixels = (DWORD*)data.Scan0;
    int stride = data.Stride / 4;
    UINT midX = w / 2;

    for (UINT y = 0; y < h; y += 4) {
        for (UINT x = 0; x < w; x += 4) {
            DWORD p = pixels[y * stride + x];
            BYTE pr = (p >> 16) & 0xFF;
            BYTE pg = (p >> 8)  & 0xFF;
            BYTE pb =  p        & 0xFF;
            if (x < midX) { r1+=pr; g1+=pg; b1+=pb; s1++; }
            else          { r2+=pr; g2+=pg; b2+=pb; s2++; }
        }
    }
    bmp->UnlockBits(&data);

    Color primary   = s1 > 0 ? Color(255, (BYTE)(r1/s1), (BYTE)(g1/s1), (BYTE)(b1/s1)) : fallbackPrimary;
    Color secondary = s2 > 0 ? Color(255, (BYTE)(r2/s2), (BYTE)(g2/s2), (BYTE)(b2/s2)) : fallbackSecondary;

    int dr = abs((int)primary.GetR() - (int)secondary.GetR());
    int dg = abs((int)primary.GetG() - (int)secondary.GetG());
    int db = abs((int)primary.GetB() - (int)secondary.GetB());
    if (dr + dg + db < 60) {
        secondary = Color(255, (BYTE)(primary.GetR() * 0.35f), (BYTE)(primary.GetG() * 0.35f), (BYTE)(primary.GetB() * 0.35f));
    }
    return { primary, secondary };
}

Bitmap* ToBitmap(IRandomAccessStreamWithContentType const& s) {
    if (!s) return nullptr;
    try {
        IStream* ns = nullptr;
        if (SUCCEEDED(CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(winrt::get_abi(s)), IID_PPV_ARGS(&ns)))) {
            Bitmap* b = Bitmap::FromStream(ns); 
            ns->Release();
            if (b && b->GetLastStatus() == Ok) return b;
            if (b) delete b;
        }
    } catch (...) {}
    return nullptr;
}

void FetchMedia() {
    try {
        if (!g_Mgr) g_Mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!g_Mgr) return;

        GlobalSystemMediaTransportControlsSession ses = nullptr;
        // 1. Priority: Find the session that is actually PLAYING (e.g. YouTube while Spotify is paused)
        auto sessions = g_Mgr.GetSessions();
        for (auto const& s : sessions) {
            try {
                if (s && s.GetPlaybackInfo().PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                    ses = s; break;
                }
            } catch (...) {}
        }
        
        // 2. Fallback: If nothing is playing, grab the last active session (paused Spotify/browser)
        if (!ses) try { ses = g_Mgr.GetCurrentSession(); } catch (...) {}
        
        if (ses) {
            // Use explicit type to avoid compiler errors
            GlobalSystemMediaTransportControlsSessionMediaProperties props = nullptr;
            try { props = ses.TryGetMediaPropertiesAsync().get(); } catch (...) { 
                // If fetching properties fails (app closing), just set playing to false and keep last info
                scoped_lock lk(g_M.mtx); g_M.playing = false; return; 
            }
            if (!props) { scoped_lock lk(g_M.mtx); g_M.playing = false; return; }

            try { g_currentMediaAppAumid = ses.SourceAppUserModelId().c_str(); } catch (...) {}
            FetchAppVolume();

            scoped_lock lk(g_M.mtx);
            wstring nt = L"Unknown";
            try { nt = props.Title().c_str(); } catch (...) {}

            // Only update artwork and colors if the song actually changed
            if (nt != g_M.title || !g_M.art) {
                g_M.art.reset();
                try {
                    auto ref = props.Thumbnail();
                    if (ref) {
                        auto stream = ref.OpenReadAsync().get();
                        if (stream) g_M.art.reset(ToBitmap(stream));
                    }
                } catch (...) {} 

                auto palette = GetAlbumPalette(g_M.art.get());
                g_M.primaryColor   = palette.primary;
                g_M.secondaryColor = palette.secondary;
            }
            g_M.title = nt;
            try { g_M.artist = props.Artist().c_str(); } catch (...) { g_M.artist = L""; }
            
            try {
                g_M.playing = (ses.GetPlaybackInfo().PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            } catch (...) { g_M.playing = false; }
            
            g_M.hasMedia = true;
        } else {
            // 3. IDLE STATE: All apps are closed.
            // WE DO NOT CLEAR title/artist/art here. We just set playing to false.
            // This keeps the last played track visible on your taskbar!
            scoped_lock lk(g_M.mtx);
            g_M.playing = false;
        }
    } catch (...) { 
        scoped_lock lk(g_M.mtx); 
        g_M.playing = false; 
    }
}

void Cmd(int c) {
    try {
        if (!g_Mgr) return;
        auto s = g_Mgr.GetCurrentSession(); if (!s) return;
        if (c==1) s.TrySkipPreviousAsync();
        else if (c==2) s.TryTogglePlayPauseAsync();
        else if (c==3) s.TrySkipNextAsync();
    } catch (...) {}
}

// ── Scroll ────────────────────────────────────────────────────────────────────
int  g_ScOff   = 0;
int  g_ScTxtW  = 0;
int  g_ScVisW  = 0;
bool g_Scroll  = false;
bool g_ScDir   = true;
int  g_ScWait  = 80;

// ── Helpers ───────────────────────────────────────────────────────────────────
bool IsLight() {
    DWORD v=0,sz=sizeof(v);
    return RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme",RRF_RT_DWORD,nullptr,&v,&sz)==ERROR_SUCCESS && v;
}

ARGB TxtARGB() {
    if (g_S.autoTheme) return IsLight() ? 0xFF1a1a1a : 0xFFFFFFFF;
    return g_S.manualText;
}

void RoundRect(GraphicsPath& p, int x, int y, int w, int h, int r) {
    if (r <= 0) {
        p.AddRectangle(Rect(x, y, w, h));
        p.CloseFigure();
        return;
    }
    int d=r*2;
    p.AddArc(x,y,d,d,180,90); p.AddArc(x+w-d,y,d,d,270,90);
    p.AddArc(x+w-d,y+h-d,d,d,0,90); p.AddArc(x,y+h-d,d,d,90,90);
    p.CloseFigure();
}

void ApplyAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE cp = g_S.roundedCorners ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cp, sizeof(cp));

    COLORREF borderCol = g_S.showBorder ? DWMWA_COLOR_DEFAULT : DWMWA_COLOR_NONE;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    HMODULE hu = GetModuleHandle(L"user32.dll");
    if (!hu) return;
    auto fn = (pSetWindowCompositionAttribute)GetProcAddress(hu, "SetWindowCompositionAttribute");
    if (!fn) return;

    ACCENT_POLICY pol = {};
    WINDOWCOMPOSITIONATTRIBDATA d = { WCA_ACCENT_POLICY, &pol, sizeof(pol) };

    if (g_S.background == 1 || (g_S.background >= 3 && g_S.background <= 7) || g_S.background == 9 || g_S.background == 11 || g_S.background == 12) {
        pol = { ACCENT_DISABLED, 0, 0, 0 };
        fn(hwnd, &d);
        MARGINS margins = { -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    } else {
        MARGINS margins = { 0 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        if (g_S.background == 0) {
            pol = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
        } else if (g_S.background == 2 || g_S.background == 8 || g_S.background == 10) {
            DWORD tint = (g_S.background >= 8) ? 
                ((g_S.autoTheme && IsLight()) ? 0x20FFFFFF : 0x18000000) : 
                ((g_S.autoTheme && IsLight()) ? 0x50FFFFFF : 0x50000000);
            pol = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
        }
        fn(hwnd, &d);
    }
}

void UpdateBoundsAndRegion(HWND hwnd) {
    HWND tb = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!tb) return;
    RECT tbRect; GetWindowRect(tb, &tbRect);
    int tbX = tbRect.left, tbY = tbRect.top, tbH = tbRect.bottom - tbRect.top;

    int panelW = g_S.width;
    int panelH = g_S.height;
    int posX = tbX + g_S.offsetX;
    int posY = tbY + (tbH / 2) - (panelH / 2) + g_S.offsetY;

    HRGN hRgnPanel;
    if (g_S.roundedCorners) hRgnPanel = CreateRoundRectRgn(0, 0, panelW + 1, panelH + 1, g_S.cornerRadius * 2, g_S.cornerRadius * 2);
    else hRgnPanel = CreateRectRgn(0, 0, panelW, panelH);

    SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, panelW, panelH, SWP_SHOWWINDOW);
    SetWindowRgn(hwnd, hRgnPanel, TRUE);
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void DrawPanel(HDC hdc, int W, int H) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    g.Clear(Color(0,0,0,0));

    Color bgPrimary, bgSecondary;
    {
        scoped_lock lk(g_M.mtx);
        bgPrimary   = g_M.primaryColor;
        bgSecondary = g_M.secondaryColor;
    }

    auto FillShape = [&](Brush* br) {
        if (g_S.roundedCorners) {
            GraphicsPath bp; RoundRect(bp, 0, 0, W, H, g_S.cornerRadius);
            g.FillPath(br, &bp);
        } else {
            g.FillRectangle(br, 0, 0, W, H);
        }
    };

    if (g_S.background == 3) {
        Color left(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
        Color right(0,  bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
        LinearGradientBrush br(Rect(0,0,W,H), left, right, LinearGradientModeHorizontal);
        FillShape(&br);
    } else if (g_S.background == 4) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgSecondary, LinearGradientModeHorizontal);
        FillShape(&br);
    } else if (g_S.background == 12) {
        LinearGradientBrush br(Rect(0,0,W,H), bgSecondary, bgPrimary, LinearGradientModeHorizontal);
        FillShape(&br);
    } else if (g_S.background == 5) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgSecondary, LinearGradientModeVertical);
        FillShape(&br);
    } else if (g_S.background == 6) {
        GraphicsPath gp;
        if (g_S.roundedCorners) RoundRect(gp, 0, 0, W, H, g_S.cornerRadius);
        else gp.AddRectangle(Rect(0, 0, W, H));
        PathGradientBrush pgb(&gp);
        int count = 1;
        pgb.SetCenterColor(bgPrimary);
        pgb.SetSurroundColors(&bgSecondary, &count);
        pgb.SetCenterPoint(PointF(W * 0.3f, H * 0.5f));
        g.FillPath(&pgb, &gp);
    } else if (g_S.background == 7) {
        LinearGradientBrush br(Rect(0,0,W,H), bgSecondary, bgPrimary, LinearGradientModeVertical);
        FillShape(&br);
    } else if (g_S.background == 8) {
        Color glassOverlay = IsLight() ? Color(40, 255, 255, 255) : Color(40, 20, 20, 20);
        SolidBrush b(glassOverlay);
        FillShape(&b);
    } else if (g_S.background == 9) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgSecondary, LinearGradientModeHorizontal);
        Color blend[] = { bgPrimary, bgPrimary, bgSecondary, bgSecondary };
        REAL pos[] = { 0.0f, 0.49f, 0.51f, 1.0f }; 
        br.SetInterpolationColors(blend, pos, 4);
        FillShape(&br);
    } else if (g_S.background == 10) {
        Color glassOverlay = IsLight() ? Color(40, 255, 255, 255) : Color(40, 20, 20, 20);
        SolidBrush b(glassOverlay);
        FillShape(&b);
        Pen p(bgPrimary, 2.0f);
        if (g_S.roundedCorners) {
            GraphicsPath bp; RoundRect(bp, 1, 1, W-2, H-2, g_S.cornerRadius);
            g.DrawPath(&p, &bp);
        } else {
            g.DrawRectangle(&p, 1, 1, W-2, H-2);
        }
    } else if (g_S.background == 11) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgPrimary, LinearGradientModeHorizontal);
        Color darkPrimary(255, max(0, (int)bgPrimary.GetR() - 50), max(0, (int)bgPrimary.GetG() - 50), max(0, (int)bgPrimary.GetB() - 50));
        Color blend[] = { bgPrimary, bgSecondary, darkPrimary };
        REAL pos[] = { 0.0f, 0.5f, 1.0f };
        br.SetInterpolationColors(blend, pos, 3);
        FillShape(&br);
    } else if (g_S.background == 0) {
        Color bg(200, 20, 20, 20);
        SolidBrush b(bg);
        FillShape(&b);
    }

    wstring title, artist; bool playing=false; std::unique_ptr<Bitmap> art;
    {
        scoped_lock lk(g_M.mtx);
        title=g_M.title; artist=g_M.artist; playing=g_M.playing;
        if (g_M.art) art.reset(g_M.art->Clone());
    }

    Color tc(TxtARGB());
    Color artistColor(128, tc.GetR(), tc.GetG(), tc.GetB());

    // ── Album Art
    int aX = g_S.padLeft, aY = g_S.padTop;
    int aS = H - g_S.padTop - g_S.padBottom;
    if (aS < 8) aS = 8;
    int aR = g_S.roundedCorners ? min(g_S.cornerRadius, aS/2) : 0;
    
    GraphicsPath ap; RoundRect(ap,aX,aY,aS,aS,aR);
    if (art) {
        g.SetClip(&ap);
        g.DrawImage(art.get(),aX,aY,aS,aS);
        g.ResetClip();
    } else {
        SolidBrush ph(Color(55,110,110,110)); g.FillPath(&ph,&ap);
    }

    // ── Controls Layout
    auto [nX, ppX, pX, vX, cY] = CalcLayout(W, H, g_S.padRight);
    const float ppBigR = 14.f, pnSmallR = 9.f;

    // Determine Dynamic Hover Color
    Color targetHoverColor;
    if (g_S.dynamicHover) targetHoverColor = Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else targetHoverColor = Color(255, (g_S.hoverColor >> 16) & 0xFF, (g_S.hoverColor >> 8) & 0xFF, g_S.hoverColor & 0xFF);

    auto LerpColor = [](Color a, Color b, float t) {
        return Color(255, 
            (BYTE)(a.GetR() + (b.GetR() - a.GetR()) * t), 
            (BYTE)(a.GetG() + (b.GetG() - a.GetG()) * t), 
            (BYTE)(a.GetB() + (b.GetB() - a.GetB()) * t));
    };

    auto IB = [&](int btn)->Color {
        float t = g_HoverProgress[btn];
        if (t <= 0.0f) return tc;
        if (t >= 1.0f) return targetHoverColor;
        return LerpColor(tc, targetHoverColor, t);
    };

    // Smooth Hover Circles
    for (int i = 1; i <= 3; i++) {
        if (g_HoverProgress[i] > 0.0f) {
            float hX = 0;
            float hR = (i == 2) ? ppBigR : (pnSmallR + 2.f);
            if (i == 1) hX = pX;
            else if (i == 2) hX = ppX;
            else if (i == 3) hX = nX;
            
            int alpha = (int)(28.f * g_HoverProgress[i]); // Gently fade in opacity
            SolidBrush hb(Color(alpha, tc.GetR(), tc.GetG(), tc.GetB()));
            
            float animR = hR * (0.8f + 0.2f * g_HoverProgress[i]); // Smoothly expand outwards
            g.FillEllipse(&hb, hX - animR, cY - animR, animR * 2.f, animR * 2.f);
        }
    }

    auto DrawIcon = [&](int type, float cx, float cy, float s, Color col) {
        SolidBrush b(col);
        auto X = [&](float val) { return cx + val * s; };
        auto Y = [&](float val) { return cy + val * s; };
        auto W_f = [&](float val) { return val * s; };

        if (type == 1) { 
            PointF pts[] = { PointF(X(4.5f), Y(-5.f)), PointF(X(-2.5f), Y(0)), PointF(X(4.5f), Y(5.f)) };
            g.FillPolygon(&b, pts, 3);
            g.FillRectangle(&b, X(-5.5f), Y(-5.f), W_f(3.f), W_f(10.f));
        } else if (type == 2) { 
            PointF pts[] = { PointF(X(-2.5f), Y(-6.f)), PointF(X(6.5f), Y(0)), PointF(X(-2.5f), Y(6.f)) };
            g.FillPolygon(&b, pts, 3);
        } else if (type == 3) { 
            g.FillRectangle(&b, X(-4.f), Y(-6.f), W_f(3.f), W_f(12.f));
            g.FillRectangle(&b, X(1.f), Y(-6.f), W_f(3.f), W_f(12.f));
        } else if (type == 4) { 
            PointF pts[] = { PointF(X(-4.5f), Y(-5.f)), PointF(X(2.5f), Y(0)), PointF(X(-4.5f), Y(5.f)) };
            g.FillPolygon(&b, pts, 3);
            g.FillRectangle(&b, X(2.5f), Y(-5.f), W_f(3.f), W_f(10.f));
        } else if (type == 5) { 
            PointF cone[] = { PointF(X(-7.f), Y(-3.f)), PointF(X(-4.f), Y(-3.f)), PointF(X(0.f), Y(-7.f)), PointF(X(0.f), Y(7.f)), PointF(X(-4.f), Y(3.f)), PointF(X(-7.f), Y(3.f)) };
            g.FillPolygon(&b, cone, 6);
            Pen p(col, W_f(1.5f)); p.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            g.DrawArc(&p, X(-5.5f), Y(-4.f), W_f(8.f), W_f(8.f), -50, 100);
            g.DrawArc(&p, X(-8.5f), Y(-7.f), W_f(14.f), W_f(14.f), -50, 100);
        }
    };

    DrawIcon(playing ? 3 : 2, ppX, cY, 1.15f, IB(2)); 
    DrawIcon(1, pX, cY, 0.9f, IB(1)); 
    DrawIcon(4, nX, cY, 0.9f, IB(3)); 

    // Horizontal Volume Slider
    if (g_S.showSpeakerIcon) {
        DrawIcon(5, vX, cY, 0.9f, IB(4));

        if (g_VolumeHover) {
            float sliderBgX = vX + 18.f;
            float sliderBgW = 74.f;
            float sliderBgH = 12.f;
            float sliderBgY = cY - (sliderBgH / 2.f);

            GraphicsPath vp; RoundRect(vp, sliderBgX, sliderBgY, sliderBgW, sliderBgH, sliderBgH / 2.f);
            Color popupBg = IsLight() ? Color(230, 245, 245, 245) : Color(200, 30, 30, 30);
            SolidBrush pBr(popupBg); g.FillPath(&pBr, &vp);

            float sliderWidth = 60.f;
            float sliderHeight = 4.f;
            float sliderX = sliderBgX + 7.f;
            float sliderTop = cY - (sliderHeight / 2.f);

            SolidBrush track(Color(60, tc.GetR(), tc.GetG(), tc.GetB()));
            GraphicsPath trackP; RoundRect(trackP, sliderX, sliderTop, sliderWidth, sliderHeight, sliderHeight/2.f);
            g.FillPath(&track, &trackP);

            float filledWidth = g_Volume * sliderWidth;
            SolidBrush fill(tc);
            if (filledWidth > 0) {
                GraphicsPath fillP; RoundRect(fillP, sliderX, sliderTop, filledWidth, sliderHeight, sliderHeight/2.f);
                g.FillPath(&fill, &fillP);
            }

            float thumbR = 4.f;
            float thumbX = sliderX + filledWidth - thumbR;
            float thumbY = cY - thumbR;
            g.FillEllipse(&fill, thumbX, thumbY, thumbR * 2.f, thumbR * 2.f);
        }
    }

    // Text
    int tX = aX + aS + 12;
    float leftMostControlX = g_S.showSpeakerIcon ? vX : pX;
    int tW = (int)(leftMostControlX - 12.f) - tX;
    if (tW < 8) tW = 8;

    FontFamily ff(FONT_NAME,nullptr);
    FontFamily artFF(L"Segoe UI Semibold", nullptr); 
    Font titleF(&ff,(REAL)(g_S.fontSize+1),FontStyleBold,    UnitPixel);
    Font artF  (&artFF,(REAL)(g_S.fontSize-1),FontStyleRegular, UnitPixel);
    
    RectF lay(0,0,4000,200),tb,ab;
    g.MeasureString(title.c_str(),-1,&titleF,lay,&tb);
    if (!artist.empty()) g.MeasureString(artist.c_str(),-1,&artF,lay,&ab);

    float totalH = tb.Height + (artist.empty() ? 0.f : ab.Height - 2.f);
    float sY = (H - totalH) / 2.f;

    Region clip(Rect(tX, 0, tW, H)); g.SetClip(&clip);

    g_ScTxtW = (int)tb.Width;
    g_ScVisW = tW; 
    g_Scroll = (g_ScTxtW > tW);

    SolidBrush tb2(tc), ab2(artistColor);
    if (g_Scroll) {
        float dx = (float)tX - g_ScOff;
        g.DrawString(title.c_str(),-1,&titleF,PointF(dx,sY),&tb2);
    } else {
        g_ScOff = 0; g_ScDir = true; 
        g.DrawString(title.c_str(),-1,&titleF,PointF((float)tX,sY),&tb2);
    }

    if (!artist.empty()) g.DrawString(artist.c_str(),-1,&artF,PointF((float)tX, sY+tb.Height-2.f),&ab2);

    g.ResetClip();

    if (g_S.background == 0 && tW > 20) {
        int fw=24, fx=tX+tW-fw;
        Color c0(0, 20, 20, 20), c1(200, 20, 20, 20);
        LinearGradientBrush fade(PointF((float)fx,0),PointF((float)(fx+fw),0),c0,c1);
        g.FillRectangle(&fade,fx,0,fw,H);
    }
}

// ── Hook ──────────────────────────────────────────────────────────────────────
bool IsTbWnd(HWND h){ WCHAR c[64]{}; GetClassNameW(h,c,64); return !wcscmp(c,L"Shell_TrayWnd"); }
void CALLBACK TbEvt(HWINEVENTHOOK,DWORD,HWND h,LONG,LONG,DWORD,DWORD) { if(IsTbWnd(h)&&g_hWnd) PostMessage(g_hWnd,WM_APP+10,0,0); }

void HookTaskbar(HWND hwnd){
    HWND tb=FindWindow(L"Shell_TrayWnd",nullptr);
    if(tb){ DWORD pid=0,tid=GetWindowThreadProcessId(tb,&pid);
        if(tid) g_TbHook=SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,EVENT_OBJECT_LOCATIONCHANGE,
            nullptr,TbEvt,pid,tid,WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS); }
    UpdateBoundsAndRegion(hwnd);
}

// ── Window Proc ───────────────────────────────────────────────────────────────
#define IDT_POLL   1001
#define IDT_ANIM   1002
#define WM_APPCLOSE WM_APP

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
    switch(msg){
    case WM_CREATE:
        ApplyAppearance(hwnd);
        SetTimer(hwnd,IDT_POLL,1000,NULL);
        HookTaskbar(hwnd);
        FetchMedia();
        return 0;
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE:      return 0;
    case WM_APPCLOSE:   DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        if(g_TbHook){UnhookWinEvent(g_TbHook);g_TbHook=nullptr;}
        g_Mgr=nullptr; PostQuitMessage(0); return 0;
    case WM_SETTINGCHANGE:
        ApplyAppearance(hwnd); UpdateBoundsAndRegion(hwnd); InvalidateRect(hwnd,NULL,TRUE); return 0;

    case WM_TIMER:
        if(wp==IDT_POLL){
            FetchMedia(); 
            bool hi = false; 
            
            // Still hide if a game/app is in FULLSCREEN, but NOT when paused
            if(g_S.hideFullscreen){
                QUERY_USER_NOTIFICATION_STATE qs; 
                if(SUCCEEDED(SHQueryUserNotificationState(&qs))) 
                    if(qs==QUNS_BUSY || qs==QUNS_RUNNING_D3D_FULL_SCREEN) hi = true;
            }

            // We removed the 'g_IdleHide' check here so it never disappears on pause
            if(hi) {
                ShowWindow(hwnd, SW_HIDE);
            } else {
                HWND tb = FindWindow(L"Shell_TrayWnd", NULL); 
                if(tb && IsWindowVisible(tb)) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }
            InvalidateRect(hwnd, NULL, FALSE);
        } else if(wp==IDT_ANIM){
            if(g_Scroll){
                if(g_ScWait>0){
                    g_ScWait--;
                } else { 
                    int maxOff = g_ScTxtW - g_ScVisW + 10;
                    if (maxOff < 0) maxOff = 0;

                    if (g_ScDir) {
                        if (++g_ScOff >= maxOff) {
                            g_ScOff = maxOff; g_ScDir = false; g_ScWait = 80;
                        }
                    } else {
                        if (--g_ScOff <= 0) {
                            g_ScOff = 0; g_ScDir = true; g_ScWait = 80;
                        }
                    }
                    InvalidateRect(hwnd,NULL,FALSE); 
                }
            } else {
                KillTimer(hwnd, IDT_ANIM);
                g_AnimTimerRunning = false;
            }
        } else if (wp == IDT_HOVER_ANIM) {
            bool needsAnim = false;
            for (int i = 1; i <= 4; i++) {
                float target = 0.f;
                // Standard buttons neatly fade back to 0 if the volume slider is active
                if (i == 4) target = (g_Hover == 4 || g_VolumeHover) ? 1.f : 0.f;
                else target = (g_Hover == i && !g_VolumeHover) ? 1.f : 0.f;
                
                if (g_HoverProgress[i] != target) {
                    float step = 0.15f; // Animation speed
                    if (g_HoverProgress[i] < target) g_HoverProgress[i] = min(target, g_HoverProgress[i] + step);
                    else g_HoverProgress[i] = max(target, g_HoverProgress[i] - step);
                    needsAnim = true;
                }
            }
            if (needsAnim) InvalidateRect(hwnd, NULL, FALSE);
            else { KillTimer(hwnd, IDT_HOVER_ANIM); g_HoverAnimRunning = false; }
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (g_S.showSpeakerIcon && g_Hover == 4 && g_VolumeHover) {
            SetCapture(hwnd);
            auto [nX, ppX, pX, vX, cY] = CalcLayout(g_S.width, g_S.height, g_S.padRight);
            float sliderX = vX + 18.f + 7.f; 
            float newVolume = (float)(LOWORD(lp) - sliderX) / 60.f;
            SetVolume(newVolume);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_MOUSEMOVE:{
        int mx=LOWORD(lp),my=HIWORD(lp);
        
        auto [nX, ppX, pX, vX, cY] = CalcLayout(g_S.width, g_S.height, g_S.padRight);
        const float ppBigR = 14.f, pnSmallR = 9.f;
        int ns=0; bool newVolumeHover = g_VolumeHover;

        float hitTop = cY - 14.f, hitBot = cY + 14.f;
        
        if (g_S.showSpeakerIcon) {
            float volHitLeft = vX - 12.f;
            float volHitRight = g_VolumeHover ? (vX + 95.f) : (vX + 14.f);
            if (mx >= volHitLeft && mx <= volHitRight && my >= hitTop && my <= hitBot) {
                newVolumeHover = true; ns = 4;
            } else newVolumeHover = false;
        }

        if (ns != 4) { 
            if (mx >= pX - (pnSmallR+4) && mx <= pX + (pnSmallR+4) && my >= cY-(pnSmallR+4) && my <= cY+(pnSmallR+4)) ns = 1;
            else if (mx >= ppX - ppBigR && mx <= ppX + ppBigR && my >= cY-ppBigR && my <= cY+ppBigR) ns = 2;
            else if (mx >= nX - (pnSmallR+4) && mx <= nX + (pnSmallR+4) && my >= cY-(pnSmallR+4) && my <= cY+(pnSmallR+4)) ns = 3;
        }

        if (g_S.showSpeakerIcon && g_VolumeHover && (wp & MK_LBUTTON) && GetCapture() == hwnd) {
            float sliderX = vX + 18.f + 7.f;
            float newVolume = (float)(mx - sliderX) / 60.f;
            SetVolume(newVolume);
        }

        if (ns != g_Hover || newVolumeHover != g_VolumeHover) {
            g_Hover = ns; g_VolumeHover = newVolumeHover;
            if (!g_HoverAnimRunning) { SetTimer(hwnd, IDT_HOVER_ANIM, 16, NULL); g_HoverAnimRunning = true; }
            InvalidateRect(hwnd,NULL,FALSE);
        }
        TRACKMOUSEEVENT tme={sizeof(tme),TME_LEAVE,hwnd};TrackMouseEvent(&tme);
        return 0;}

    case WM_MOUSELEAVE:
        if (GetCapture() != hwnd) {
            g_Hover = 0; g_VolumeHover = false;
            if (!g_HoverAnimRunning) { SetTimer(hwnd, IDT_HOVER_ANIM, 16, NULL); g_HoverAnimRunning = true; }
            InvalidateRect(hwnd,NULL,FALSE); 
        }
        break;

    case WM_LBUTTONUP:
        if (GetCapture() == hwnd) ReleaseCapture();
        if (g_Hover > 0 && g_Hover < 4) Cmd(g_Hover);
        return 0;

    case WM_MOUSEWHEEL:{
        POINT pt = { LOWORD(lp), HIWORD(lp) }; ScreenToClient(hwnd, &pt);
        if (g_S.showSpeakerIcon) {
            auto [nX, ppX, pX, vX, cY] = CalcLayout(g_S.width, g_S.height, g_S.padRight);
            if (pt.x >= vX - 15 && pt.x <= vX + 95) {
                short d=GET_WHEEL_DELTA_WPARAM(wp);
                SetVolume(g_Volume + (d > 0 ? 0.05f : -0.05f));
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;}

    case WM_PAINT:{
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);

        HDC mdc = CreateCompatibleDC(hdc);
        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = rc.right; bmi.bmiHeader.biHeight = -rc.bottom; 
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;

        void* bits; HBITMAP mb = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
        HBITMAP ob = (HBITMAP)SelectObject(mdc, mb);

        DrawPanel(mdc, rc.right, rc.bottom);

        if(g_Scroll && !g_AnimTimerRunning) { SetTimer(hwnd, IDT_ANIM, 16, NULL); g_AnimTimerRunning = true; }
        
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, ob); DeleteObject(mb); DeleteDC(mdc);
        EndPaint(hwnd, &ps); 
        return 0;}

    case WM_APP+10: UpdateBoundsAndRegion(hwnd); return 0;
    default:
        if(msg==g_TbCreatedMsg){ if(g_TbHook){UnhookWinEvent(g_TbHook);g_TbHook=nullptr;} HookTaskbar(hwnd); return 0; }
    }
    return DefWindowProc(hwnd,msg,wp,lp);
}

// ── Media Thread ──────────────────────────────────────────────────────────────
void MediaThread(){
    winrt::init_apartment();
    GdiplusStartupInput gsi; ULONG_PTR tok;
    GdiplusStartup(&tok,&gsi,NULL);

    WNDCLASS wc={}; wc.lpfnWndProc=WndProc; wc.hInstance=GetModuleHandle(NULL);
    wc.lpszClassName=L"WindhawkMusicLounge_v5"; wc.hCursor=LoadCursor(NULL,IDC_HAND);
    RegisterClass(&wc);

    HMODULE hu32=GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CWB=hu32?(pCreateWindowInBand)GetProcAddress(hu32,"CreateWindowInBand"):nullptr;
    if(CWB) g_hWnd=CWB(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST, wc.lpszClassName,L"MusicLounge",WS_POPUP|WS_VISIBLE, 0,0,g_S.width,g_S.height,NULL,NULL,wc.hInstance,NULL,ZBID_IMMERSIVE_NOTIFICATION);
    if(!g_hWnd) g_hWnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST, wc.lpszClassName,L"MusicLounge",WS_POPUP|WS_VISIBLE, 0,0,g_S.width,g_S.height,NULL,NULL,wc.hInstance,NULL);
    SetLayeredWindowAttributes(g_hWnd,0,255,LWA_ALPHA);

    MSG m; while(GetMessage(&m,NULL,0,0)){TranslateMessage(&m);DispatchMessage(&m);}
    UnregisterClass(wc.lpszClassName,wc.hInstance); GdiplusShutdown(tok); winrt::uninit_apartment();
}

std::unique_ptr<std::thread> g_pThread = nullptr;

BOOL  WhTool_ModInit()           { SetCurrentProcessExplicitAppUserModelID(L"taskbar-media-player"); LoadSettings(); g_pThread = std::make_unique<std::thread>(MediaThread); return TRUE; }
void  WhTool_ModUninit()         { if(g_hWnd)SendMessage(g_hWnd,WM_APPCLOSE,0,0); if(g_pThread){if(g_pThread->joinable())g_pThread->join(); g_pThread.reset();} }
void  WhTool_ModSettingsChanged(){ LoadSettings(); if(g_hWnd){SendMessage(g_hWnd,WM_TIMER,IDT_POLL,0);SendMessage(g_hWnd,WM_SETTINGCHANGE,0,0);} }

// ── Boilerplate ───────────────────────────────────────────────────────────────
bool   g_isLauncher=false;
HANDLE g_toolMutex =nullptr;
void WINAPI EntryHook(){ Wh_Log(L">"); ExitThread(0); }

BOOL Wh_ModInit(){
    bool isSvc=false,isTool=false,isCur=false;
    int argc; LPWSTR* argv=CommandLineToArgvW(GetCommandLine(),&argc);
    if(!argv) return FALSE;
    for(int i=1;i<argc;i++) if(!wcscmp(argv[i],L"-service")){isSvc=true;break;}
    for(int i=1;i<argc-1;i++) if(!wcscmp(argv[i],L"-tool-mod")){isTool=true;if(!wcscmp(argv[i+1],WH_MOD_ID))isCur=true;break;}
    LocalFree(argv);
    if(isSvc) return FALSE;
    if(isCur){
        g_toolMutex=CreateMutex(nullptr,TRUE,L"windhawk-tool-mod_" WH_MOD_ID);
        if(!g_toolMutex) ExitProcess(1);
        if(GetLastError()==ERROR_ALREADY_EXISTS) ExitProcess(1);
        if(!WhTool_ModInit()) ExitProcess(1);
        auto* dos=(IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        auto* nt=(IMAGE_NT_HEADERS*)((BYTE*)dos+dos->e_lfanew);
        Wh_SetFunctionHook((BYTE*)dos+nt->OptionalHeader.AddressOfEntryPoint,(void*)EntryHook,nullptr);
        return TRUE;
    }
    if(isTool) return FALSE;
    g_isLauncher=true; return TRUE;
}

void Wh_ModAfterInit(){
    if(!g_isLauncher) return;
    WCHAR path[MAX_PATH]; if(!GetModuleFileName(nullptr,path,MAX_PATH)) return;
    WCHAR cmd[MAX_PATH+64]; swprintf_s(cmd,L"\"%s\" -tool-mod \"%s\"",path,WH_MOD_ID);
    HMODULE km=GetModuleHandle(L"kernelbase.dll"); if(!km) km=GetModuleHandle(L"kernel32.dll"); if(!km) return;
    using CPIW=BOOL(WINAPI*)(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,WINBOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION,PHANDLE);
    auto fn=(CPIW)GetProcAddress(km,"CreateProcessInternalW"); if(!fn) return;
    STARTUPINFO si{.cb=sizeof(si),.dwFlags=STARTF_FORCEOFFFEEDBACK}; PROCESS_INFORMATION pi;
    if(!fn(nullptr,path,cmd,nullptr,nullptr,FALSE,NORMAL_PRIORITY_CLASS,nullptr,nullptr,&si,&pi,nullptr)) return;
    CloseHandle(pi.hProcess);CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged(){ if(!g_isLauncher) WhTool_ModSettingsChanged(); }
void Wh_ModUninit()         { if(!g_isLauncher){ WhTool_ModUninit(); ExitProcess(0); } }