// ==WindhawkMod==
// @id              taskbar-music-lounge-pro
// @name            Taskbar Music Lounge Pro
// @description     A native-style music ticker with media controls, multi-band spectrogram audio visualizer, live taskbar lyric streaming, and expanded Now Playing popup.
// @version         5.0.4
// @author          Amit
// @github          https://github.com/AmitJaiswal001
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lmmdevapi -lpropsys
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge Pro

A media controller that uses Windows 11 native DWM styling for a seamless taskbar experience.


## ✨ Features
* **Universal Support:** Smart scanning detects active playback from any app.
* **Album Art:** Displays current track cover art with rounded corners.
* **Expanded Now Playing Popup:** Click the compact bar to open a Mac-style popup with:
  - Large album art, track title, artist
  - Source app icon and name
  - Seekable progress bar (drag to seek)
  - Shuffle and Repeat controls
  - Real volume controls (per-app volume adjusting)
  - Smooth 180ms ease-out open / 120ms close animation
* **Multiple Media Switcher:** Switch between multiple active media sources (e.g. Spotify, Browser, VLC) via top-row app switcher buttons or a natural hold-and-drag horizontal swipe gesture on the popup card.
* **Instant Responsive Controls**: Local state caching guarantees instant feedback on play/pause, prev, next, shuffle, and repeat without lag.
* **Seek Lock Protection**: Smooth timeline extrapolation and browser safety locks prevent the tracker from resetting to 0:00 during active seek actions.
* **Isomorphic Music Visualizer:** A beautiful bottom-aligned 4-bar visualizer that bounces to the actual audio output peaks of Windows at 60fps and rests when silent. Fully togglable between real-time peak audio and smooth mock visualization.
* **Fullscreen Mode:** Hides automatically when running full-screen applications.
* **No Media Auto-Hide:** Hides when nothing is playing, reappears instantly on playback.
* **Idle Timeout:** Optional auto-hide after pause for X seconds.
* **Volume:** Scroll over compact bar to adjust volume.


## ⚠️ Requirements
* **Disable Widgets:** Taskbar Settings -> Widgets -> Off.
* **Windows 11:** Required for rounded corners and acrylic blur.
* **VLC Media Player SMTC Integration:** VLC does not support Windows system media transport controls by default. To display VLC media in the compact bar, install the open-source `vlc-win10smtc` plugin DLL inside VLC's `plugins\misc\` directory and check the plugin box under Tools > Preferences > Show Settings: All > Interface > Control interfaces.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 300
  $name: Panel Width
- PanelHeight: 52
  $name: Panel Height
- FontSize: 15
  $name: Font Size
- ButtonScale: 1.0
  $name: Button Scale (1.0 = Normal, 2.0 = 4K)
- HideFullscreen: true
  $name: Hide when Fullscreen
- IdleTimeout: 0
  $name: Auto-hide when paused (Seconds). Set 0 to disable.
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- AutoTheme: true
  $name: Auto Theme
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
- BgOpacity: 0
  $name: Acrylic Tint Opacity (0-255). Keep 0 for pure glass.
- UseBlur: false
  $name: Use Acrylic Blur (glass mode)
- PopupWidth: 320
  $name: Popup Width
- PopupHeight: 380
  $name: Popup Height
- PopupFontSize: 15
  $name: Popup Font Size
- PopupIconSize: 28
  $name: Popup App Icon Size
- ShowVisualizer: true
  $name: Show Music Visualizer
- RealTimeVisualizer: false
  $name: Real-time sound reactive visualizer
- VisualizerScale: 1.0
  $name: Visualizer Bar Scale
- VisualizerHeight: 14
  $name: Visualizer Bar Height
- FetchLyrics: true
  $name: Fetch and Display Lyrics
- LyricsFontSize: 14
  $name: Lyrics Font Size
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <cstdio>
#include <cmath>
#include <ctime>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Data.Json.h>
#include <sstream>
#include <algorithm>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

extern HWND g_hExpandedWindow;

// Define missing IAudioMeterInformation interface for MinGW compatibility
struct IAudioMeterInformation : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float *pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT32 *pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float *afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD *pdwHardwareSupport) = 0;
};
const IID my_IID_IAudioMeterInformation = {0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

// ============================================================
// Constants
// ============================================================
const WCHAR* FONT_NAME      = L"Segoe UI Variable Display";
const int    DEFAULT_POPUP_WIDTH    = 320;
const int    DEFAULT_POPUP_HEIGHT   = 380;
const int    POPUP_GAP      = 8;       // gap between compact bar and popup
const int    ANIM_OPEN_MS   = 180;
const int    ANIM_CLOSE_MS  = 120;
const int    ANIM_FPS       = 60;
const int    ANIM_INTERVAL  = 1000 / ANIM_FPS;  // ~16ms

// ============================================================
// DWM / Composition API
// ============================================================
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;
typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// ============================================================
// Z-Band API
// ============================================================
enum ZBID {
    ZBID_DEFAULT = 0, ZBID_DESKTOP = 1, ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3, ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5, ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7, ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9, ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11, ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13, ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15, ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17, ZBID_ABOVELOCK_UX = 18,
};
typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID, DWORD);

// ============================================================
// Settings
// ============================================================
struct ModSettings {
    int    width         = 300;
    int    height        = 52;
    int    fontSize      = 15;
    double buttonScale   = 1.0;
    bool   hideFullscreen = false;
    int    idleTimeout   = 0;
    int    offsetX       = 12;
    int    offsetY       = 0;
    bool   autoTheme     = true;
    DWORD  manualTextColor = 0xFFFFFFFF;
    int    bgOpacity     = 0;
    int    popupWidth    = 320;
    int    popupHeight   = 380;
    int    popupFontSize = 15;
    int    popupIconSize = 28;
    bool   showVisualizer  = true;
    bool   realTimeVisualizer = false;
    double visualizerScale = 1.0;
    int    visualizerHeight = 14;
    bool   glassBackdrop = false;
    bool   useBlur       = false;
    bool   fetchLyrics = true;
    int    lyricsFontSize = 14;
} g_Settings;

// ============================================================
// Animation State
// ============================================================
struct AnimState {
    bool  isOpen        = false;   // true = popup visible/opening
    bool  isAnimating   = false;
    float progress      = 0.0f;    // 0.0 = fully closed, 1.0 = fully open
    bool  opening       = true;    // direction
    int   totalFrames   = 0;
    int   currentFrame  = 0;
} g_Anim;

// ============================================================
// Timeline / Volume State
// ============================================================
struct TimelineState {
    double positionSec  = 0.0;
    double durationSec  = 0.0;
    bool   valid        = false;
    bool   canSeek      = false;
} g_Timeline;

int  g_AppScrollOffset = 0;
int  g_AppScrollWait   = 60;
bool g_IsAppScrolling  = false;
int  g_AppTextWidth    = 0;

int  g_TitleScrollOffset = 0;
int  g_TitleScrollWait   = 60;
bool g_IsTitleScrolling  = false;
int  g_TitleTextWidth    = 0;

int  g_ArtistScrollOffset = 0;
int  g_ArtistScrollWait   = 60;
bool g_IsArtistScrolling  = false;
int  g_ArtistTextWidth    = 0;

float g_LyricsScrollOffset = 0.0f;
float g_LyricsTargetScroll  = 0.0f;

// ============================================================
// Lyrics State & Fetching
// ============================================================
struct LyricLine {
    double timeSec;
    wstring text;
};

struct LyricState {
    mutex lock;
    wstring trackTitle;
    wstring trackArtist;
    vector<LyricLine> lines;
    wstring plainText;
    bool hasLyrics = false;
    bool showLyrics = false;
    bool streamLyrics = false;
} g_Lyrics;

void ParseLrc(const wstring& lrcStr, const wstring& targetTitle) {
    lock_guard<mutex> guard(g_Lyrics.lock);
    
    // Safety check: if the song was changed while we were fetching, discard the results
    if (g_Lyrics.trackTitle != targetTitle) {
        return;
    }
    
    g_Lyrics.lines.clear();
    g_Lyrics.plainText.clear();
    
    wstringstream ss(lrcStr);
    wstring line;
    while (getline(ss, line)) {
        if (line.empty()) continue;
        size_t start = line.find(L'[');
        size_t end = line.find(L']');
        if (start != wstring::npos && end != wstring::npos && end > start + 1) {
            wstring timeStr = line.substr(start + 1, end - start - 1);
            wstring text = line.substr(end + 1);
            
            // Trim leading/trailing spaces from text
            size_t first = text.find_first_not_of(L" \t\r\n");
            if (first != wstring::npos) {
                size_t last = text.find_last_not_of(L" \t\r\n");
                text = text.substr(first, (last - first + 1));
            } else {
                text.clear();
            }
            
            double min = 0, sec = 0;
            if (swscanf_s(timeStr.c_str(), L"%lf:%lf", &min, &sec) == 2) {
                LyricLine ll;
                ll.timeSec = min * 60.0 + sec;
                ll.text = text;
                g_Lyrics.lines.push_back(ll);
            }
        }
    }
    
    // Sort lines by time
    std::sort(g_Lyrics.lines.begin(), g_Lyrics.lines.end(), [](const LyricLine& a, const LyricLine& b) {
        return a.timeSec < b.timeSec;
    });
    g_Lyrics.hasLyrics = !g_Lyrics.lines.empty();
}

wstring CleanTrackTitle(wstring title) {
    auto IsHash = [](const wstring& s) -> bool {
        if (s.length() < 5 || s.length() > 15) return false;
        int alnumCount = 0;
        int digitCount = 0;
        for (wchar_t c : s) {
            if (iswalnum(c)) {
                alnumCount++;
                if (iswdigit(c)) digitCount++;
            }
        }
        return (alnumCount > (int)s.length() * 0.7 && digitCount > 0);
    };

    wstring result;
    size_t i = 0;
    while (i < title.length()) {
        wchar_t c = title[i];
        if (c == L'(' || c == L'[') {
            wchar_t closeChar = (c == L'(') ? L')' : L']';
            size_t closePos = title.find(closeChar, i);
            if (closePos != wstring::npos) {
                wstring inside = title.substr(i + 1, closePos - i - 1);
                wstring lowerInside = inside;
                for (auto& ch : lowerInside) ch = towlower(ch);
                
                if (lowerInside.find(L"feat") != wstring::npos ||
                    lowerInside.find(L"with") != wstring::npos ||
                    lowerInside.find(L"slowed") != wstring::npos ||
                    lowerInside.find(L"reverb") != wstring::npos ||
                    lowerInside.find(L"speed") != wstring::npos ||
                    lowerInside.find(L"lofi") != wstring::npos ||
                    lowerInside.find(L"remix") != wstring::npos ||
                    lowerInside.find(L"cover") != wstring::npos ||
                    lowerInside.find(L"video") != wstring::npos ||
                    lowerInside.find(L"audio") != wstring::npos ||
                    lowerInside.find(L"music") != wstring::npos ||
                    lowerInside.find(L"lyrics") != wstring::npos ||
                    lowerInside.find(L"remaster") != wstring::npos ||
                    lowerInside.find(L"version") != wstring::npos ||
                    IsHash(inside)) {
                    i = closePos + 1;
                    continue;
                }
            }
        }
        result += c;
        i++;
    }
    
    size_t dashPos = result.find(L" - ");
    if (dashPos != wstring::npos) {
        wstring suffix = result.substr(dashPos + 3);
        wstring lowerSuffix = suffix;
        for (auto& ch : lowerSuffix) ch = towlower(ch);
        if (lowerSuffix.find(L"slowed") != wstring::npos ||
            lowerSuffix.find(L"reverb") != wstring::npos ||
            lowerSuffix.find(L"remaster") != wstring::npos ||
            lowerSuffix.find(L"lyrics") != wstring::npos ||
            lowerSuffix.find(L"video") != wstring::npos ||
            lowerSuffix.find(L"cover") != wstring::npos ||
            lowerSuffix.find(L"remix") != wstring::npos) {
            result = result.substr(0, dashPos);
        }
    }
    
    size_t first = result.find_first_not_of(L" \t\r\n");
    if (first != wstring::npos) {
        size_t last = result.find_last_not_of(L" \t\r\n");
        result = result.substr(first, (last - first + 1));
    } else {
        result.clear();
    }
    
    return result;
}

std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &str[0], size, nullptr, nullptr);
    return str;
}

std::string UrlEncodeUtf8(const std::string& value) {
    std::string encoded;
    for (char c : value) {
        unsigned char uc = (unsigned char)c;
        if (isalnum(uc) || uc == '-' || uc == '_' || uc == '.' || uc == '~') {
            encoded += c;
        } else if (uc == ' ') {
            encoded += "%20";
        } else {
            char buf[10];
            sprintf_s(buf, "%%%02X", (int)uc);
            encoded += buf;
        }
    }
    return encoded;
}

bool IsEmoji(wchar_t c) {
    if (c >= 0xD800 && c <= 0xDFFF) return true;
    if (c >= 0x2600 && c <= 0x27BF) return true;
    if (c >= 0x2300 && c <= 0x23FF) return true;
    if (c >= 0x2900 && c <= 0x2BFF) return true;
    return false;
}

void DrawStringWithEmoji(Graphics& g, const wstring& text, Font* normalFont, const RectF& rect, StringFormat* sf, Color textColor) {
    if (text.empty()) return;
    
    FontFamily emojiFamily(L"Segoe UI Emoji", nullptr);
    Font emojiFont(&emojiFamily, normalFont->GetSize(), normalFont->GetStyle(), UnitPixel);
    
    SolidBrush brush(textColor);
    StringFormat layoutFormat;
    layoutFormat.SetAlignment(StringAlignmentNear);
    if (sf) {
        layoutFormat.SetLineAlignment(sf->GetLineAlignment());
        layoutFormat.SetFormatFlags(sf->GetFormatFlags());
        layoutFormat.SetTrimming(sf->GetTrimming());
    } else {
        layoutFormat.SetLineAlignment(StringAlignmentNear);
    }
    
    StringAlignment align = sf ? sf->GetAlignment() : StringAlignmentNear;
    
    vector<pair<wstring, bool>> runs;
    wstring currentRun;
    bool currentIsEmoji = false;
    
    for (size_t i = 0; i < text.size(); i++) {
        wchar_t c = text[i];
        bool isE = IsEmoji(c);
        
        wstring ch(1, c);
        if (c >= 0xD800 && c <= 0xDBFF && i + 1 < text.size()) {
            ch += text[i + 1];
            i++;
            isE = true;
        }
        
        if (runs.empty()) {
            currentRun = ch;
            currentIsEmoji = isE;
            runs.push_back({ currentRun, currentIsEmoji });
        } else {
            if (isE == currentIsEmoji) {
                runs.back().first += ch;
            } else {
                currentRun = ch;
                currentIsEmoji = isE;
                runs.push_back({ currentRun, currentIsEmoji });
            }
        }
    }
    
    float totalWidth = 0.0f;
    vector<float> widths;
    for (const auto& run : runs) {
        RectF layoutRect(0, 0, 2000.0f, rect.Height);
        RectF boundingBox;
        Font* activeFont = run.second ? &emojiFont : normalFont;
        g.MeasureString(run.first.c_str(), -1, activeFont, layoutRect, &layoutFormat, &boundingBox);
        widths.push_back(boundingBox.Width);
        totalWidth += boundingBox.Width;
    }
    
    float startX = rect.X;
    if (align == StringAlignmentCenter) {
        startX = rect.X + (rect.Width - totalWidth) / 2.0f;
    } else if (align == StringAlignmentFar) {
        startX = rect.X + rect.Width - totalWidth;
    }
    
    float currentX = startX;
    for (size_t i = 0; i < runs.size(); i++) {
        Font* activeFont = runs[i].second ? &emojiFont : normalFont;
        RectF charRect(currentX, rect.Y, widths[i] + 4.0f, rect.Height);
        g.DrawString(runs[i].first.c_str(), -1, activeFont, charRect, &layoutFormat, &brush);
        currentX += widths[i];
    }
}

void FetchLyrics(wstring artist, wstring title, double durationSec) {
    std::thread([artist, title, durationSec]() {
        try {
            winrt::init_apartment();
            winrt::Windows::Web::Http::HttpClient client;
            client.DefaultRequestHeaders().UserAgent().TryParseAdd(L"TaskbarMusicLoungePro/5.0.1 (https://github.com/AmitJaiswal001)");
            
            wstring cleanArtist = CleanTrackTitle(artist);
            wstring cleanTitle = CleanTrackTitle(title);
            
            std::string utf8Artist = UrlEncodeUtf8(WideToUtf8(cleanArtist));
            std::string utf8Title = UrlEncodeUtf8(WideToUtf8(cleanTitle));
            
            wstring url = L"https://lrclib.net/api/get?artist_name=" + wstring(utf8Artist.begin(), utf8Artist.end()) + 
                          L"&track_name=" + wstring(utf8Title.begin(), utf8Title.end());
            
            if (durationSec > 0.0) {
                url += L"&duration=" + to_wstring((int)durationSec);
            }
            
            winrt::Windows::Foundation::Uri uri(url.c_str());
            auto response = client.GetAsync(uri).get();
            bool loaded = false;
            
            if (response.IsSuccessStatusCode()) {
                wstring body = response.Content().ReadAsStringAsync().get().c_str();
                winrt::Windows::Data::Json::JsonObject json = winrt::Windows::Data::Json::JsonObject::Parse(body);
                
                bool gotSynced = false;
                if (json.HasKey(L"syncedLyrics") && json.GetNamedValue(L"syncedLyrics").ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
                    wstring synced = json.GetNamedString(L"syncedLyrics").c_str();
                    if (!synced.empty()) {
                        ParseLrc(synced, title);
                        gotSynced = g_Lyrics.hasLyrics;
                    }
                }
                
                if (!gotSynced && json.HasKey(L"plainLyrics") && json.GetNamedValue(L"plainLyrics").ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
                    wstring plain = json.GetNamedString(L"plainLyrics").c_str();
                    if (!plain.empty()) {
                        lock_guard<mutex> guard(g_Lyrics.lock);
                        if (g_Lyrics.trackTitle == title) {
                            g_Lyrics.plainText = plain;
                            g_Lyrics.hasLyrics = true;
                        }
                    }
                }
                loaded = true;
            }
            
            // If failed to load with clean title & duration, try a search to match the closest duration
            if (!loaded || !g_Lyrics.hasLyrics) {
                wstring searchUrl = L"https://lrclib.net/api/search?q=" + wstring(utf8Title.begin(), utf8Title.end()) + 
                                    L"+" + wstring(utf8Artist.begin(), utf8Artist.end());
                
                winrt::Windows::Foundation::Uri searchUri(searchUrl.c_str());
                auto responseSearch = client.GetAsync(searchUri).get();
                if (responseSearch.IsSuccessStatusCode()) {
                    wstring body = responseSearch.Content().ReadAsStringAsync().get().c_str();
                    winrt::Windows::Data::Json::JsonArray arr = winrt::Windows::Data::Json::JsonArray::Parse(body);
                    
                    int bestIndex = -1;
                    double minDiff = 99999.0;
                    
                    for (uint32_t i = 0; i < arr.Size(); i++) {
                        auto item = arr.GetAt(i).GetObject();
                        if (item.HasKey(L"duration")) {
                            double itemDur = item.GetNamedNumber(L"duration");
                            double diff = fabs(itemDur - durationSec);
                            if (diff < minDiff) {
                                minDiff = diff;
                                bestIndex = (int)i;
                            }
                        }
                    }
                    
                    // Choose the closest duration match if it is within 15 seconds
                    if (bestIndex != -1 && (durationSec <= 0.0 || minDiff < 15.0)) {
                        auto bestItem = arr.GetAt(bestIndex).GetObject();
                        bool gotSynced = false;
                        if (bestItem.HasKey(L"syncedLyrics") && bestItem.GetNamedValue(L"syncedLyrics").ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
                            wstring synced = bestItem.GetNamedString(L"syncedLyrics").c_str();
                            if (!synced.empty()) {
                                ParseLrc(synced, title);
                                gotSynced = g_Lyrics.hasLyrics;
                            }
                        }
                        
                        if (!gotSynced && bestItem.HasKey(L"plainLyrics") && bestItem.GetNamedValue(L"plainLyrics").ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
                            wstring plain = bestItem.GetNamedString(L"plainLyrics").c_str();
                            if (!plain.empty()) {
                                lock_guard<mutex> guard(g_Lyrics.lock);
                                if (g_Lyrics.trackTitle == title) {
                                    g_Lyrics.plainText = plain;
                                    g_Lyrics.hasLyrics = true;
                                }
                            }
                        }
                        loaded = true;
                    }
                }
            }
            
            // Final fallback to raw original title if still nothing
            if (!loaded || !g_Lyrics.hasLyrics) {
                std::string utf8OrigArtist = UrlEncodeUtf8(WideToUtf8(artist));
                std::string utf8OrigTitle = UrlEncodeUtf8(WideToUtf8(title));
                
                wstring origUrl = L"https://lrclib.net/api/get?artist_name=" + wstring(utf8OrigArtist.begin(), utf8OrigArtist.end()) + 
                                  L"&track_name=" + wstring(utf8OrigTitle.begin(), utf8OrigTitle.end());
                if (durationSec > 0.0) {
                    origUrl += L"&duration=" + to_wstring((int)durationSec);
                }
                
                winrt::Windows::Foundation::Uri origUri(origUrl.c_str());
                auto responseOrig = client.GetAsync(origUri).get();
                if (responseOrig.IsSuccessStatusCode()) {
                    wstring body = responseOrig.Content().ReadAsStringAsync().get().c_str();
                    winrt::Windows::Data::Json::JsonObject json = winrt::Windows::Data::Json::JsonObject::Parse(body);
                    
                    bool gotSynced = false;
                    if (json.HasKey(L"syncedLyrics") && json.GetNamedValue(L"syncedLyrics").ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
                        wstring synced = json.GetNamedString(L"syncedLyrics").c_str();
                        if (!synced.empty()) {
                            ParseLrc(synced, title);
                            gotSynced = g_Lyrics.hasLyrics;
                        }
                    }
                    
                    if (!gotSynced && json.HasKey(L"plainLyrics") && json.GetNamedValue(L"plainLyrics").ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
                        wstring plain = json.GetNamedString(L"plainLyrics").c_str();
                        if (!plain.empty()) {
                            lock_guard<mutex> guard(g_Lyrics.lock);
                            if (g_Lyrics.trackTitle == title) {
                                g_Lyrics.plainText = plain;
                                g_Lyrics.hasLyrics = true;
                            }
                        }
                    }
                }
            }
        } catch (...) {}
        
        if (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) {
            InvalidateRect(g_hExpandedWindow, nullptr, FALSE);
        }
    }).detach();
}

ULONGLONG g_TimelineLastUpdated = 0;
ULONGLONG g_LastSeekTime        = 0;
int64_t g_LastTimelineUpdatedTicks = 0;
double g_PendingSeekPosition    = 0.0;
bool g_SeekPending              = false;
BYTE g_MediaWindowAlpha         = 255;

float   g_VolumeLevel    = 0.5f;   // 0.0 – 1.0
bool    g_IsMuted        = false;
bool    g_SeekDragging   = false;
bool    g_VolDragging    = false;
int     g_SeekDragX      = 0;
int     g_VolDragX       = 0;

IAudioEndpointVolume* g_pAudioVolume = nullptr;
IAudioMeterInformation* g_pMeter = nullptr;

// ============================================================
// Global State
// ============================================================
HWND g_hMediaWindow    = NULL;
HWND g_hExpandedWindow = NULL;
bool g_Running         = true;
int  g_HoverState      = 0;
HWINEVENTHOOK g_TaskbarHook  = nullptr;
UINT g_TaskbarCreatedMsg     = RegisterWindowMessage(L"TaskbarCreated");

int  g_IdleSecondsCounter = 0;
bool g_IsHiddenByIdle     = false;
bool g_IsHiddenByNoMedia  = false;

// Hover state for expanded panel controls
int  g_ExpHoverBtn    = 0;   // 1=prev 2=play 3=next 4=shuffle 5=repeat 6=appPrev 7=appNext
bool g_ExpHoverSeek   = false;
bool g_ExpHoverVol    = false;

// Optimistic Controls state locks
ULONGLONG g_OptimisticTime   = 0;
bool      g_OptimisticPlaying = false;

// Session Switcher and Swipe State
int       g_CurrentSessionIndex = 0;
bool      g_UserSelectedSession  = false;
bool      g_IsDraggingSession   = false;
int       g_DragStartX          = 0;
float     g_DragOffsetX         = 0.0f;
float     g_ArtSlideOffset      = 0.0f;
float     g_ArtSlideTarget      = 0.0f;

// ============================================================
// Media State
// ============================================================
std::atomic<bool> g_UpdatingMedia{false};
std::atomic<bool> g_PendingUpdate{false};

struct MediaState {
    wstring title      = L"Waiting for media...";
    wstring artist     = L"";
    wstring appName    = L"";
    wstring appId      = L"";
    bool    isPlaying  = false;
    bool    hasMedia   = false;
    Bitmap* albumArt   = nullptr;
    Bitmap* appIcon    = nullptr;
    bool    shuffle    = false;
    winrt::Windows::Media::MediaPlaybackAutoRepeatMode repeatMode =
        winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
    bool    canGoNext  = true;
    bool    canGoPrev  = true;
    bool    canPlayPause = true;
    bool    canShuffle = true;
    bool    canRepeat  = true;
    mutex   lock;
} g_MediaState;

double GetLivePosition() {
    if (g_SeekDragging) return g_Timeline.positionSec;
    bool isPlaying = false;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        isPlaying = g_MediaState.isPlaying;
    }
    if (isPlaying && g_Timeline.valid && g_Timeline.durationSec > 0.0) {
        double elapsed = (double)(GetTickCount64() - g_TimelineLastUpdated) / 1000.0;
        double pos = g_Timeline.positionSec + elapsed;
        if (pos > g_Timeline.durationSec) pos = g_Timeline.durationSec;
        return pos;
    }
    return g_Timeline.positionSec;
}

// ============================================================
// Scrolling (compact bar)
// ============================================================
float g_ScrollOffset = 0.0f;
float g_LyricScrollSpeed = 1.0f;
int  g_TextWidth    = 0;
bool g_IsScrolling  = false;
int  g_ScrollWait   = 60;

// Lyrics manual scroll state
bool      g_LyricsIsUserScrolling = false;
bool      g_LyricsScrollDragging = false;
ULONGLONG g_LyricsLastUserScrollTime = 0;
float     g_LyricsUserScrollOffset = 0.0f;

// ============================================================
// Timer IDs
// ============================================================
#define IDT_POLL_MEDIA   1001
#define IDT_ANIMATION    1002
#define IDT_ANIM_POPUP   1003
#define APP_WM_CLOSE     WM_APP
#define WM_TOGGLE_POPUP  (WM_APP + 20)
#define WM_CHECK_VISIBILITY (WM_APP + 30)

// ============================================================
// Easing
// ============================================================
// Ease-out cubic: fast start, gentle arrival
static float EaseOutCubic(float t) {
    float f = 1.0f - t;
    return 1.0f - f * f * f;
}

// ============================================================
// Volume API
// ============================================================
bool IsSessionMatch(const wstring& mediaAppId, const wstring& mediaAppName, const wstring& sessionExeName) {
    wstring appId = mediaAppId;
    for (auto& c : appId) c = towlower(c);
    wstring appName = mediaAppName;
    for (auto& c : appName) c = towlower(c);
    wstring exeName = sessionExeName;
    for (auto& c : exeName) c = towlower(c);

    size_t dot = exeName.rfind(L".exe");
    if (dot != wstring::npos) {
        exeName = exeName.substr(0, dot);
    }

    if (appId.find(exeName) != wstring::npos || exeName.find(appId) != wstring::npos) return true;
    if (appName.find(exeName) != wstring::npos || exeName.find(appName) != wstring::npos) return true;
    return false;
}

ISimpleAudioVolume* GetActiveSessionVolume() {
    IMMDeviceEnumerator* pEnum = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (FAILED(hr)) return nullptr;

    IMMDevice* pDevice = nullptr;
    hr = pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnum->Release();
    if (FAILED(hr)) return nullptr;

    IAudioSessionManager2* pManager = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&pManager);
    pDevice->Release();
    if (FAILED(hr)) return nullptr;

    IAudioSessionEnumerator* pSessionEnum = nullptr;
    hr = pManager->GetSessionEnumerator(&pSessionEnum);
    pManager->Release();
    if (FAILED(hr)) return nullptr;

    int count = 0;
    pSessionEnum->GetCount(&count);

    wstring mediaAppId, mediaAppName;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        mediaAppId = g_MediaState.appId;
        mediaAppName = g_MediaState.appName;
    }

    ISimpleAudioVolume* pTargetVolume = nullptr;

    for (int i = 0; i < count; i++) {
        IAudioSessionControl* pSessionControl = nullptr;
        if (SUCCEEDED(pSessionEnum->GetSession(i, &pSessionControl))) {
            IAudioSessionControl2* pSessionControl2 = nullptr;
            if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2))) {
                bool matched = false;
                DWORD pid = 0;
                if (SUCCEEDED(pSessionControl2->GetProcessId(&pid)) && pid != 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                    if (hProcess) {
                        wchar_t path[MAX_PATH];
                        DWORD size = MAX_PATH;
                        if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
                            wstring exePath = path;
                            size_t lastSlash = exePath.rfind(L'\\');
                            wstring exeName = (lastSlash != wstring::npos) ? exePath.substr(lastSlash + 1) : exePath;

                            if (IsSessionMatch(mediaAppId, mediaAppName, exeName)) {
                                matched = true;
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }

                // Fallback: If pid match failed, match using instance identifier (robust match even for Admin apps)
                if (!matched) {
                    LPWSTR pIdStr = nullptr;
                    if (SUCCEEDED(pSessionControl2->GetSessionInstanceIdentifier(&pIdStr)) && pIdStr) {
                        wstring idStr = pIdStr;
                        CoTaskMemFree(pIdStr);
                        for (auto& c : idStr) c = towlower(c);
                        
                        wstring lowerAppId = mediaAppId;
                        for (auto& c : lowerAppId) c = towlower(c);
                        wstring lowerAppName = mediaAppName;
                        for (auto& c : lowerAppName) c = towlower(c);
                        
                        if (!lowerAppId.empty() && idStr.find(lowerAppId) != wstring::npos) matched = true;
                        else if (!lowerAppName.empty() && idStr.find(lowerAppName) != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"chrome") != wstring::npos && idStr.find(L"chrome.exe") != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"spotify") != wstring::npos && idStr.find(L"spotify.exe") != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"firefox") != wstring::npos && idStr.find(L"firefox.exe") != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"vlc") != wstring::npos && idStr.find(L"vlc.exe") != wstring::npos) matched = true;
                    }
                }

                if (matched) {
                    hr = pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pTargetVolume);
                    if (SUCCEEDED(hr)) {
                        pSessionControl2->Release();
                        pSessionControl->Release();
                        break;
                    }
                }
                pSessionControl2->Release();
            }
            pSessionControl->Release();
        }
    }

    pSessionEnum->Release();
    return pTargetVolume;
}

void InitVolumeAPI() {
    if (g_pAudioVolume) return;
    IMMDeviceEnumerator* pEnum = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                     __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (!pEnum) return;
    IMMDevice* pDevice = nullptr;
    pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnum->Release();
    if (!pDevice) return;
    pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                      nullptr, (void**)&g_pAudioVolume);
    pDevice->Release();
}

void InitMeterAPI() {
    if (g_pMeter) return;
    IMMDeviceEnumerator* pEnum = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                     __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (!pEnum) return;
    IMMDevice* pDevice = nullptr;
    pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnum->Release();
    if (!pDevice) return;
    pDevice->Activate(my_IID_IAudioMeterInformation, CLSCTX_ALL,
                      nullptr, (void**)&g_pMeter);
    pDevice->Release();
}

void ReadVolume() {
    ISimpleAudioVolume* pAppVol = GetActiveSessionVolume();
    if (pAppVol) {
        float level = 0.5f;
        pAppVol->GetMasterVolume(&level);
        g_VolumeLevel = level;
        BOOL muted = FALSE;
        pAppVol->GetMute(&muted);
        g_IsMuted = (muted != FALSE);
        pAppVol->Release();
    } else {
        if (g_pAudioVolume) {
            g_pAudioVolume->GetMasterVolumeLevelScalar(&g_VolumeLevel);
            BOOL muted = FALSE;
            g_pAudioVolume->GetMute(&muted);
            g_IsMuted = (muted != FALSE);
        }
    }
}

void SetVolume(float level) {
    level = max(0.0f, min(1.0f, level));
    g_VolumeLevel = level;

    ISimpleAudioVolume* pAppVol = GetActiveSessionVolume();
    if (pAppVol) {
        pAppVol->SetMasterVolume(level, nullptr);
        if (g_IsMuted && level > 0.0f) {
            pAppVol->SetMute(FALSE, nullptr);
            g_IsMuted = false;
        }
        pAppVol->Release();
    } else {
        if (g_pAudioVolume) {
            g_pAudioVolume->SetMasterVolumeLevelScalar(level, nullptr);
            if (g_IsMuted && level > 0.0f) {
                g_pAudioVolume->SetMute(FALSE, nullptr);
                g_IsMuted = false;
            }
        }
    }
}

// ============================================================
// Settings
// ============================================================
void LoadSettings() {
    g_Settings.width       = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height      = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize    = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX     = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY     = Wh_GetIntSetting(L"OffsetY");
    g_Settings.autoTheme   = Wh_GetIntSetting(L"AutoTheme") != 0;

    PCWSTR scaleStr = Wh_GetStringSetting(L"ButtonScale");
    if (scaleStr) {
        g_Settings.buttonScale = _wtof(scaleStr);
        Wh_FreeStringSetting(scaleStr);
    } else {
        g_Settings.buttonScale = 1.0;
    }
    g_Settings.buttonScale = max(0.5, min(4.0, g_Settings.buttonScale));

    g_Settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;
    g_Settings.idleTimeout    = Wh_GetIntSetting(L"IdleTimeout");

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;

    g_Settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    g_Settings.bgOpacity = max(0, min(255, g_Settings.bgOpacity));
    g_Settings.useBlur = Wh_GetIntSetting(L"UseBlur") != 0;
    g_Settings.glassBackdrop = g_Settings.useBlur;

    g_Settings.popupWidth    = Wh_GetIntSetting(L"PopupWidth");
    g_Settings.popupHeight   = Wh_GetIntSetting(L"PopupHeight");
    g_Settings.popupFontSize = Wh_GetIntSetting(L"PopupFontSize");
    g_Settings.popupIconSize = Wh_GetIntSetting(L"PopupIconSize");
    g_Settings.lyricsFontSize = Wh_GetIntSetting(L"LyricsFontSize");
    g_Settings.fetchLyrics = Wh_GetIntSetting(L"FetchLyrics") != 0;
    g_Settings.showVisualizer  = Wh_GetIntSetting(L"ShowVisualizer") != 0;
    g_Settings.realTimeVisualizer = Wh_GetIntSetting(L"RealTimeVisualizer") != 0;

    PCWSTR visScaleStr = Wh_GetStringSetting(L"VisualizerScale");
    if (visScaleStr) {
        g_Settings.visualizerScale = _wtof(visScaleStr);
        Wh_FreeStringSetting(visScaleStr);
    } else {
        g_Settings.visualizerScale = 1.0;
    }
    g_Settings.visualizerScale = max(0.5, min(3.0, g_Settings.visualizerScale));
    g_Settings.visualizerHeight = Wh_GetIntSetting(L"VisualizerHeight");

    if (g_Settings.width  < 100) g_Settings.width  = 300;
    if (g_Settings.height < 24)  g_Settings.height = 52;
    if (g_Settings.popupWidth < 200) g_Settings.popupWidth = 320;
    if (g_Settings.popupHeight < 200) g_Settings.popupHeight = 380;
    if (g_Settings.fontSize < 6) g_Settings.fontSize = 15;
    if (g_Settings.popupFontSize < 6) g_Settings.popupFontSize = 15;
    if (g_Settings.popupIconSize < 8) g_Settings.popupIconSize = 28;
    if (g_Settings.visualizerHeight < 4) g_Settings.visualizerHeight = 14;
    if (g_Settings.lyricsFontSize < 6) g_Settings.lyricsFontSize = 14;
}

// ============================================================
// WinRT / GSMTC
// ============================================================
GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;

Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    try {
        auto abi = winrt::get_abi(stream);
        if (!abi) return nullptr;
        IStream* nativeStream = nullptr;
        if (SUCCEEDED(CreateStreamOverRandomAccessStream(
                reinterpret_cast<IUnknown*>(abi),
                IID_PPV_ARGS(&nativeStream)))) {
            if (nativeStream) {
                Bitmap* bmp = Bitmap::FromStream(nativeStream);
                nativeStream->Release();
                if (bmp && bmp->GetLastStatus() == Ok) return bmp;
                delete bmp;
            }
        }
    } catch (...) {}
    return nullptr;
}

bool IsBrowserApp(const wstring& appId) {
    wstring id = appId;
    for (auto& c : id) c = towlower(c);
    return (id.find(L"chrome") != wstring::npos ||
            id.find(L"brave") != wstring::npos ||
            id.find(L"msedge") != wstring::npos ||
            id.find(L"firefox") != wstring::npos ||
            id.find(L"opera") != wstring::npos ||
            id.find(L"browser") != wstring::npos);
}

wstring CleanBrowserSource(wstring artist) {
    if (artist.empty()) return L"";
    if (artist.find(L"www.") == 0) {
        artist = artist.substr(4);
    }
    size_t dot = artist.find(L".");
    if (dot != wstring::npos) {
        artist = artist.substr(0, dot);
    }
    if (!artist.empty() && artist[0] >= L'a' && artist[0] <= L'z') {
        artist[0] = towupper(artist[0]);
    }
    if (artist == L"Youtube") artist = L"YouTube";
    else if (artist == L"Soundcloud") artist = L"SoundCloud";
    return artist;
}

struct FindWindowData {
    DWORD pid;
    HWND hwnd;
};

BOOL CALLBACK FindWindowByPidCallback(HWND hwnd, LPARAM lParam) {
    FindWindowData* data = reinterpret_cast<FindWindowData*>(lParam);
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hwnd, &windowPid);
    if (windowPid == data->pid) {
        if (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL) {
            data->hwnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

HWND FindTopLevelWindowByPid(DWORD pid) {
    FindWindowData data = { pid, nullptr };
    EnumWindows(FindWindowByPidCallback, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

DWORD GetActiveSessionPid() {
    wstring mediaAppId;
    wstring mediaAppName;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        mediaAppId = g_MediaState.appId;
        mediaAppName = g_MediaState.appName;
    }
    if (mediaAppId.empty() && mediaAppName.empty()) return 0;

    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return 0;

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnumerator->Release();
    if (FAILED(hr)) return 0;

    IAudioSessionManager2* pManager = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&pManager);
    pDevice->Release();
    if (FAILED(hr)) return 0;

    IAudioSessionEnumerator* pSessionEnum = nullptr;
    hr = pManager->GetSessionEnumerator(&pSessionEnum);
    pManager->Release();
    if (FAILED(hr)) return 0;

    int count = 0;
    pSessionEnum->GetCount(&count);
    DWORD targetPid = 0;

    for (int i = 0; i < count; i++) {
        IAudioSessionControl* pSessionControl = nullptr;
        if (SUCCEEDED(pSessionEnum->GetSession(i, &pSessionControl))) {
            IAudioSessionControl2* pSessionControl2 = nullptr;
            if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2))) {
                DWORD pid = 0;
                if (SUCCEEDED(pSessionControl2->GetProcessId(&pid)) && pid != 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                    if (hProcess) {
                        WCHAR path[MAX_PATH];
                        DWORD size = MAX_PATH;
                        if (QueryFullProcessImageName(hProcess, 0, path, &size)) {
                            wstring exePath = path;
                            wstring exeName = exePath.substr(exePath.rfind(L'\\') + 1);
                            if (IsSessionMatch(mediaAppId, mediaAppName, exeName)) {
                                targetPid = pid;
                                CloseHandle(hProcess);
                                pSessionControl2->Release();
                                pSessionControl->Release();
                                break;
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }
                if (targetPid == 0) {
                    LPWSTR pIdStr = nullptr;
                    if (SUCCEEDED(pSessionControl2->GetSessionInstanceIdentifier(&pIdStr)) && pIdStr) {
                        wstring idStr = pIdStr;
                        CoTaskMemFree(pIdStr);
                        for (auto& c : idStr) c = towlower(c);
                        wstring lowerAppId = mediaAppId;
                        for (auto& c : lowerAppId) c = towlower(c);
                        wstring lowerAppName = mediaAppName;
                        for (auto& c : lowerAppName) c = towlower(c);
                        bool matched = false;
                        if (!lowerAppId.empty() && idStr.find(lowerAppId) != wstring::npos) matched = true;
                        else if (!lowerAppName.empty() && idStr.find(lowerAppName) != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"chrome") != wstring::npos && idStr.find(L"chrome.exe") != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"spotify") != wstring::npos && idStr.find(L"spotify.exe") != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"firefox") != wstring::npos && idStr.find(L"firefox.exe") != wstring::npos) matched = true;
                        else if (lowerAppId.find(L"vlc") != wstring::npos && idStr.find(L"vlc.exe") != wstring::npos) matched = true;
                        if (matched) {
                            pSessionControl2->GetProcessId(&targetPid);
                        }
                    }
                }
                pSessionControl2->Release();
            }
            pSessionControl->Release();
            if (targetPid != 0) break;
        }
    }
    pSessionEnum->Release();
    return targetPid;
}

void RedirectToMediaSource() {
    DWORD pid = GetActiveSessionPid();
    if (pid == 0) return;
    HWND hwnd = FindTopLevelWindowByPid(pid);
    if (hwnd) {
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        SetForegroundWindow(hwnd);
    }
}

#ifndef SIIGBF_ICONONLY
#define SIIGBF_ICONONLY 0x00000004
#endif

wstring ResolveAumidToPath(const wstring& aumid) {
    if (aumid.find(L":\\") != wstring::npos) {
        return aumid;
    }
    
    size_t exePos = aumid.rfind(L".exe");
    if (exePos == wstring::npos) return L"";
    
    wstring subpath = aumid;
    size_t bracePos = aumid.rfind(L'}');
    if (bracePos != wstring::npos && bracePos + 1 < aumid.length()) {
        subpath = aumid.substr(bracePos + 1);
        if (subpath[0] == L'\\' || subpath[0] == L'/') {
            subpath = subpath.substr(1);
        }
    }
    
    std::vector<wstring> rootDirs = {
        L"C:\\Program Files",
        L"C:\\Program Files (x86)",
        L"D:\\Program Files",
        L"D:\\Program Files (x86)"
    };
    
    wchar_t* localAppData = _wgetenv(L"LOCALAPPDATA");
    if (localAppData) {
        rootDirs.push_back(wstring(localAppData) + L"\\Programs");
    }
    
    for (const auto& root : rootDirs) {
        wstring fullPath = root + L"\\" + subpath;
        if (GetFileAttributesW(fullPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return fullPath;
        }
    }
    
    size_t lastSlash = subpath.rfind(L'\\');
    wstring filename = (lastSlash != wstring::npos) ? subpath.substr(lastSlash + 1) : subpath;
    
    for (const auto& root : rootDirs) {
        wstring fullPath = root + L"\\" + filename;
        if (GetFileAttributesW(fullPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return fullPath;
        }
        
        if (filename == L"vlc.exe") {
            wstring vlcPath = root + L"\\VideoLAN\\VLC\\vlc.exe";
            if (GetFileAttributesW(vlcPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                return vlcPath;
            }
        }
    }
    
    return L"";
}

void ResolveAppInfo(const wstring& aumid, wstring& friendlyName, Bitmap*& pIconBmp) {
    friendlyName = L"";
    pIconBmp = nullptr;

    if (aumid.empty()) return;

    wstring parsingName = L"shell:AppsFolder\\" + aumid;
    IShellItem* pShellItem = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(parsingName.c_str(), nullptr, IID_PPV_ARGS(&pShellItem));
    if (SUCCEEDED(hr)) {
        LPWSTR pszName = nullptr;
        if (SUCCEEDED(pShellItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName))) {
            friendlyName = pszName;
            CoTaskMemFree(pszName);
        }

        IShellItemImageFactory* pFactory = nullptr;
        if (SUCCEEDED(pShellItem->QueryInterface(IID_PPV_ARGS(&pFactory)))) {
            SIZE size = { 32, 32 };
            HBITMAP hBmp = nullptr;
            if (SUCCEEDED(pFactory->GetImage(size, SIIGBF_ICONONLY, &hBmp))) {
                pIconBmp = Bitmap::FromHBITMAP(hBmp, nullptr);
                DeleteObject(hBmp);
            }
            pFactory->Release();
        }
        pShellItem->Release();
    }

    if (friendlyName.empty()) {
        size_t bang = aumid.rfind(L'!');
        size_t slash = aumid.rfind(L'\\');
        size_t dot   = aumid.rfind(L'.');
        size_t start = max({ bang, slash, dot });
        if (start != wstring::npos && start + 1 < aumid.size())
            friendlyName = aumid.substr(start + 1);
        else
            friendlyName = aumid;

        if (friendlyName.size() >= 4 && friendlyName.compare(friendlyName.size() - 4, 4, L".exe") == 0) {
            friendlyName = friendlyName.substr(0, friendlyName.size() - 4);
        }
    }

    if (!pIconBmp) {
        wstring resolvedPath = ResolveAumidToPath(aumid);
        if (!resolvedPath.empty()) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW(resolvedPath.c_str(), 0, &sfi, sizeof(sfi),
                               SHGFI_ICON | SHGFI_SMALLICON)) {
                pIconBmp = Bitmap::FromHICON(sfi.hIcon);
                DestroyIcon(sfi.hIcon);
            }
        }
    }

    if (!pIconBmp) {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(aumid.c_str(), 0, &sfi, sizeof(sfi),
                           SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES)) {
            pIconBmp = Bitmap::FromHICON(sfi.hIcon);
            DestroyIcon(sfi.hIcon);
        }
    }
}

GlobalSystemMediaTransportControlsSession GetCompactSession() {
    if (!g_SessionManager) return nullptr;
    try {
        auto sessionsList = g_SessionManager.GetSessions();
        for (auto const& s : sessionsList) {
            auto pb = s.GetPlaybackInfo();
            if (pb && pb.PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                return s;
            }
        }
        return g_SessionManager.GetCurrentSession();
    } catch (...) {
        return nullptr;
    }
}

GlobalSystemMediaTransportControlsSession GetPopupSession() {
    if (!g_SessionManager) return nullptr;
    try {
        auto sessionsList = g_SessionManager.GetSessions();
        int count = (int)sessionsList.Size();
        if (count == 0) return nullptr;

        if (!g_UserSelectedSession) {
            auto compactSession = GetCompactSession();
            if (compactSession) {
                try {
                    wstring compactId = compactSession.SourceAppUserModelId().c_str();
                    for (int i = 0; i < count; i++) {
                        if (sessionsList.GetAt(i).SourceAppUserModelId() == compactId) {
                            g_CurrentSessionIndex = i;
                            break;
                        }
                    }
                } catch (...) {}
            }
        }

        if (g_CurrentSessionIndex >= count) g_CurrentSessionIndex = 0;
        if (g_CurrentSessionIndex < 0) g_CurrentSessionIndex = count - 1;

        return sessionsList.GetAt(g_CurrentSessionIndex);
    } catch (...) {
        return nullptr;
    }
}

void SwitchSession(int direction) {
    if (!g_SessionManager) return;
    try {
        auto sessionsList = g_SessionManager.GetSessions();
        int count = (int)sessionsList.Size();
        if (count <= 1) return;

        g_UserSelectedSession = true;
        g_CurrentSessionIndex += direction;
        if (g_CurrentSessionIndex >= count) g_CurrentSessionIndex = 0;
        if (g_CurrentSessionIndex < 0) g_CurrentSessionIndex = count - 1;

        // Animate Swipe Card transition
        g_ArtSlideOffset = (direction > 0) ? 60.0f : -60.0f;
        g_ArtSlideTarget = 0.0f;

        PostMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
    } catch (...) {}
}

void UpdateMediaInfo();

void UpdateMediaInfoBackground() {
    try {
        if (!g_SessionManager) {
            g_SessionManager =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (g_SessionManager) {
                try {
                    g_SessionManager.SessionsChanged([](auto const& sender, auto const& args) {
                        UpdateMediaInfo();
                    });
                } catch (...) {}
            }
        }
        if (!g_SessionManager) return;

        // Choose session depending on context (Compact vs Popup switcher override)
        GlobalSystemMediaTransportControlsSession session =
            (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) ? GetPopupSession() : GetCompactSession();

        if (!session && GetTickCount64() - g_LastSeekTime <= 1500) {
            return;
        }

        if (session) {
            GlobalSystemMediaTransportControlsSessionMediaProperties props = nullptr;
            try { props = session.TryGetMediaPropertiesAsync().get(); } catch (...) {}

            GlobalSystemMediaTransportControlsSessionPlaybackInfo info = nullptr;
            try { info = session.GetPlaybackInfo(); } catch (...) {}

            GlobalSystemMediaTransportControlsSessionTimelineProperties tl = nullptr;
            try { tl = session.GetTimelineProperties(); } catch (...) {}

            // Timeline (with Seek lock protection and LastUpdatedTime check to prevent browser stale progress snaps)
            const ULONGLONG SEEK_SETTLE_MS = 1500;
            if (tl) {
                try {
                    auto lut = tl.LastUpdatedTime();
                    int64_t lutTicks = lut.time_since_epoch().count();

                    // Only query SMTC properties if the player has actually pushed new progress data
                    if (lutTicks != g_LastTimelineUpdatedTicks) {
                        auto pos = tl.Position();
                        auto end = tl.EndTime();
                        double newPos = pos.count() / 1e7;   // 100ns → seconds
                        double newDur = end.count() / 1e7;

                        bool skipUpdate = false;

                        // Seek pending protection check: wait for player to catch up to target position
                        if (g_SeekPending) {
                            if (GetTickCount64() - g_LastSeekTime > 3000) {
                                g_SeekPending = false;
                            } else {
                                if (fabs(newPos - g_PendingSeekPosition) > 2.0) {
                                    skipUpdate = true;
                                } else {
                                    g_SeekPending = false;
                                }
                            }
                        }

                        // Also ignore updates during the hard post-seek window if the position is far from expected
                        if (GetTickCount64() - g_LastSeekTime <= SEEK_SETTLE_MS) {
                            if (fabs(newPos - g_Timeline.positionSec) > 2.0) {
                                skipUpdate = true;
                            }
                        }

                        if (!skipUpdate) {
                            g_LastTimelineUpdatedTicks = lutTicks;
                            g_Timeline.positionSec = newPos;
                            g_Timeline.durationSec = newDur;
                            g_Timeline.valid       = (newDur > 0.0);
                            g_TimelineLastUpdated  = GetTickCount64();
                        }
                    }
                } catch (...) {}
            }

            // App identity
            wstring newAppId;
            try { newAppId = session.SourceAppUserModelId().c_str(); } catch (...) {}

            // Seek capability check: Enable for Spotify, Chrome, Firefox, Edge, Opera, VLC, or when SMTC claims IsSeekEnabled()
            bool canSeekVal = false;
            if (info) {
                try {
                    auto caps = info.Controls();
                    if (caps) canSeekVal = caps.IsPlaybackPositionEnabled();
                } catch (...) {}
            }
            wstring lowerAppId = newAppId;
            for (auto& c : lowerAppId) c = towlower(c);
            if (lowerAppId.find(L"spotify") != wstring::npos ||
                lowerAppId.find(L"chrome") != wstring::npos ||
                lowerAppId.find(L"firefox") != wstring::npos ||
                lowerAppId.find(L"msedge") != wstring::npos ||
                lowerAppId.find(L"opera") != wstring::npos ||
                lowerAppId.find(L"vlc") != wstring::npos) {
                canSeekVal = true;
            }
            g_Timeline.canSeek = canSeekVal;

            bool shuffleVal = false;
            winrt::Windows::Media::MediaPlaybackAutoRepeatMode repeatVal =
                winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
            bool canGoNextVal = true;
            bool canGoPrevVal = true;
            bool canPlayPauseVal = true;
            bool canShuffleVal = true;
            bool canRepeatVal = true;
            if (info) {
                try {
                    auto shRef = info.IsShuffleActive();
                    if (shRef) shuffleVal = shRef.Value();
                    auto rpRef = info.AutoRepeatMode();
                    if (rpRef) repeatVal = rpRef.Value();
                    
                    auto caps = info.Controls();
                    if (caps) {
                        canGoNextVal = caps.IsNextEnabled();
                        canGoPrevVal = caps.IsPreviousEnabled();
                        canPlayPauseVal = caps.IsPlayEnabled() || caps.IsPauseEnabled();
                        canShuffleVal = caps.IsShuffleEnabled();
                        canRepeatVal = caps.IsRepeatEnabled();
                    }
                } catch (...) {}
            }

            wstring newTitle;
            wstring newArtist;
            if (props) {
                try { newTitle = props.Title().c_str(); } catch (...) {}
                try { newArtist = props.Artist().c_str(); } catch (...) {}
            }
            if (newTitle.empty()) {
                // If we just seeked, do NOT clear the media state! The player is just busy seeking.
                if (GetTickCount64() - g_LastSeekTime <= 1500) {
                    return;
                }

                // Verify if the playback is actually stopped/closed.
                // If it is still Playing, Paused, or Changing, do NOT wipe the metadata, just ignore!
                winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus statusVal = 
                    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Closed;
                if (info) {
                    try { statusVal = info.PlaybackStatus(); } catch (...) {}
                }

                if (statusVal != winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Closed &&
                    statusVal != winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Stopped) {
                    return; // Ignore temporary empty title while loading/buffering/seeking
                }

                lock_guard<mutex> guard(g_MediaState.lock);
                g_MediaState.hasMedia = false;
                g_MediaState.title    = L"No media playing";
                g_MediaState.artist   = L"";
                g_MediaState.appName  = L"";
                if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                if (g_MediaState.appIcon)  { delete g_MediaState.appIcon; g_MediaState.appIcon = nullptr; }
                g_Timeline.valid = false;
                g_TimelineLastUpdated = GetTickCount64();
                g_MediaState.shuffle = false;
                g_MediaState.repeatMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                
                if (g_hMediaWindow) {
                    InvalidateRect(g_hMediaWindow, nullptr, FALSE);
                    PostMessage(g_hMediaWindow, WM_CHECK_VISIBILITY, 0, 0);
                }
                return;
            }

            // Retrieve current values inside a very fast lock
            wstring currentTitle;
            wstring currentAppId;
            bool hasAlbumArt = false;
            bool hasAppIcon = false;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                currentTitle = g_MediaState.title;
                currentAppId = g_MediaState.appId;
                hasAlbumArt  = (g_MediaState.albumArt != nullptr);
                hasAppIcon   = (g_MediaState.appIcon != nullptr);
            }

            // Firefox / Browser Thumbnail Update Fix:
            // Force reload thumbnail stream for 4 consecutive seconds after a title change,
            // to allow browsers to finish rendering and update their SMTC graphics.
            static int g_ThumbnailRefreshCount = 0;
            bool titleChanged = (newTitle != currentTitle);
            if (titleChanged) {
                g_ThumbnailRefreshCount = 4;
            }

            bool forceArtFetch = (titleChanged || !hasAlbumArt || g_ThumbnailRefreshCount > 0);
            if (g_ThumbnailRefreshCount > 0) {
                g_ThumbnailRefreshCount--;
            }

            // Perform UWP blocking asynchronous calls completely OUTSIDE the mutex lock
            Bitmap* newArt = nullptr;
            if (props && forceArtFetch) {
                try {
                    auto thumbRef = props.Thumbnail();
                    if (thumbRef) {
                        auto stream = thumbRef.OpenReadAsync().get();
                        newArt = StreamToBitmap(stream);
                    }
                } catch (...) {}
            }

            wstring friendlyName;
            Bitmap* pIconBmp = nullptr;
            bool appChanged = (!newAppId.empty() && (newAppId != currentAppId || !hasAppIcon));
            if (appChanged) {
                ResolveAppInfo(newAppId, friendlyName, pIconBmp);
            }

            // Lock the mutex only to assign the fetched pointers and states instantly
            {
                lock_guard<mutex> guard(g_MediaState.lock);

                // Switch slide animation offset when new track loads
                if (titleChanged && !g_MediaState.title.empty() && g_MediaState.title != L"Waiting for media...") {
                    if (g_ArtSlideTarget == 0.0f && fabs(g_ArtSlideOffset) < 1.0f) {
                        g_ArtSlideOffset = 40.0f;
                        g_ArtSlideTarget = 0.0f;
                    }
                }

                if (forceArtFetch) {
                    if (g_MediaState.albumArt) {
                        delete g_MediaState.albumArt;
                        g_MediaState.albumArt = nullptr;
                    }
                    g_MediaState.albumArt = newArt;
                }

                if (appChanged) {
                    if (g_MediaState.appIcon) {
                        delete g_MediaState.appIcon;
                        g_MediaState.appIcon = nullptr;
                    }
                    g_MediaState.appId   = newAppId;
                    g_MediaState.appName = friendlyName;
                    g_MediaState.appIcon = pIconBmp;
                }

                g_MediaState.title    = newTitle;
                g_MediaState.artist   = newArtist;

                // Trigger lyrics search if track title changes (and fetchLyrics setting is enabled)
                if (titleChanged) {
                    if (g_Settings.fetchLyrics) {
                        {
                            lock_guard<mutex> lguard(g_Lyrics.lock);
                            g_Lyrics.trackTitle = newTitle;
                            g_Lyrics.trackArtist = newArtist;
                            g_Lyrics.lines.clear();
                            g_Lyrics.plainText.clear();
                            g_Lyrics.hasLyrics = false;
                            g_Lyrics.showLyrics = false; // reset display toggle
                        }
                        FetchLyrics(newArtist, newTitle, g_Timeline.durationSec);
                    } else {
                        lock_guard<mutex> lguard(g_Lyrics.lock);
                        g_Lyrics.trackTitle = L"";
                        g_Lyrics.trackArtist = L"";
                        g_Lyrics.lines.clear();
                        g_Lyrics.plainText.clear();
                        g_Lyrics.hasLyrics = false;
                        g_Lyrics.showLyrics = false;
                    }
                }

                bool isPlaying = false;
                if (info) {
                    try {
                        isPlaying = (info.PlaybackStatus() ==
                            GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                    } catch (...) {}
                }

                if (GetTickCount64() - g_OptimisticTime > 800) {
                    g_MediaState.isPlaying = isPlaying;
                } else {
                    g_MediaState.isPlaying = g_OptimisticPlaying;
                }

                g_MediaState.hasMedia = true;
                g_MediaState.shuffle    = shuffleVal;
                g_MediaState.repeatMode = repeatVal;
                g_MediaState.canGoNext  = canGoNextVal;
                g_MediaState.canGoPrev  = canGoPrevVal;
                g_MediaState.canPlayPause = canPlayPauseVal;
                g_MediaState.canShuffle = canShuffleVal;
                g_MediaState.canRepeat  = canRepeatVal;
            }

        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.title    = L"No media playing";
            g_MediaState.artist   = L"";
            g_MediaState.appName  = L"";
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
            if (g_MediaState.appIcon)  { delete g_MediaState.appIcon; g_MediaState.appIcon = nullptr; }
            g_Timeline.valid = false;
            g_TimelineLastUpdated = GetTickCount64();
            g_MediaState.shuffle = false;
            g_MediaState.repeatMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
            g_MediaState.canGoNext = true;
            g_MediaState.canGoPrev = true;
            g_MediaState.canPlayPause = true;
            g_MediaState.canShuffle = true;
            g_MediaState.canRepeat = true;
        }
    } catch (...) {
        lock_guard<mutex> guard(g_MediaState.lock);
        g_MediaState.hasMedia = false;
        g_Timeline.valid = false;
        g_TimelineLastUpdated = GetTickCount64();
        g_MediaState.canGoNext = true;
        g_MediaState.canGoPrev = true;
        g_MediaState.canPlayPause = true;
        g_MediaState.canShuffle = true;
        g_MediaState.canRepeat = true;
    }

    ReadVolume();

    // Trigger repaints on UI threads and run immediate visibility checks
    if (g_hMediaWindow) {
        InvalidateRect(g_hMediaWindow, nullptr, FALSE);
        PostMessage(g_hMediaWindow, WM_CHECK_VISIBILITY, 0, 0);
    }
    if (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) {
        InvalidateRect(g_hExpandedWindow, nullptr, FALSE);
    }
}

void UpdateMediaInfo() {
    if (g_UpdatingMedia) {
        g_PendingUpdate = true;
        return;
    }
    g_UpdatingMedia = true;
    std::thread([]() {
        try {
            winrt::init_apartment();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            do {
                g_PendingUpdate = false;
                UpdateMediaInfoBackground();
            } while (g_PendingUpdate);
            CoUninitialize();
            winrt::uninit_apartment();
        } catch (...) {}
        g_UpdatingMedia = false;
    }).detach();
}

void SendMediaCommand(int cmd) {
    std::thread([cmd]() {
        try {
            winrt::init_apartment();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            auto session = (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) ? GetPopupSession() : GetCompactSession();
            if (session) {
                if (cmd == 1) session.TrySkipPreviousAsync().get();
                else if (cmd == 2) session.TryTogglePlayPauseAsync().get();
                else if (cmd == 3) session.TrySkipNextAsync().get();
            }
            CoUninitialize();
            winrt::uninit_apartment();
        } catch (...) {}
    }).detach();
}

void SendSeekCommand(double seconds) {
    std::thread([seconds]() {
        try {
            winrt::init_apartment();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            auto session = (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) ? GetPopupSession() : GetCompactSession();
            if (session) {
                double targetSec = seconds;
                if (targetSec < 0.1) targetSec = 0.1; // Prevent exact 0 seek issues in VLC/browsers
                int64_t ticks = (int64_t)(targetSec * 1e7);
                session.TryChangePlaybackPositionAsync(ticks).get();
            }
            CoUninitialize();
            winrt::uninit_apartment();
        } catch (...) {}
    }).detach();
}

void ToggleShuffle() {
    std::thread([]() {
        try {
            winrt::init_apartment();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            auto session = (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) ? GetPopupSession() : GetCompactSession();
            if (session) {
                auto info = session.GetPlaybackInfo();
                if (info) {
                    auto shRef = info.IsShuffleActive();
                    bool currentShuffle = shRef ? shRef.Value() : false;
                    session.TryChangeShuffleActiveAsync(!currentShuffle).get();
                }
            }
            CoUninitialize();
            winrt::uninit_apartment();
        } catch (...) {}
    }).detach();
}

void ToggleRepeat() {
    std::thread([]() {
        try {
            winrt::init_apartment();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            auto session = (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) ? GetPopupSession() : GetCompactSession();
            if (session) {
                auto info = session.GetPlaybackInfo();
                if (info) {
                    auto rpRef = info.AutoRepeatMode();
                    winrt::Windows::Media::MediaPlaybackAutoRepeatMode currentRepeat =
                        rpRef ? rpRef.Value() : winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                    
                    winrt::Windows::Media::MediaPlaybackAutoRepeatMode nextRepeat;
                    if (currentRepeat == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None) {
                        nextRepeat = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List;
                    } else if (currentRepeat == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List) {
                        nextRepeat = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track;
                    } else {
                        nextRepeat = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                    }
                    session.TryChangeAutoRepeatModeAsync(nextRepeat).get();
                }
            }
            CoUninitialize();
            winrt::uninit_apartment();
        } catch (...) {}
    }).detach();
}

// ============================================================
// Visuals helpers
// ============================================================
bool IsSystemLightMode() {
    if (!g_Settings.useBlur) return false; // Force dark mode if blur is disabled!
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
        return value != 0;
    return false;
}

DWORD GetCurrentTextColor() {
    if (g_Settings.autoTheme) return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    return g_Settings.manualTextColor;
}

void ApplyAcrylicBlur(HWND hwnd, DWORD tintOverride = 0, bool useTintOverride = false) {
    DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));

    bool isMediaWnd = (hwnd == g_hMediaWindow);

    // 1. Force light backdrop when GlassBackdrop is enabled for macOS clear glass style, otherwise use dark mode based on theme
    BOOL darkMode = (g_Settings.glassBackdrop && !isMediaWnd) ? FALSE : !IsSystemLightMode();
    DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode)); // DWMWA_USE_IMMERSIVE_DARK_MODE

    // 2. Enable Windows 11 official Acrylic Backdrop if GlassBackdrop is active, otherwise solid none
    DWORD systemBackdropType = (g_Settings.glassBackdrop && !isMediaWnd) ? 3 : 1; // 3 = Acrylic, 1 = None (Opaque)
    HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &systemBackdropType, sizeof(systemBackdropType));

    // 3. Fallback for Windows 10 SetWindowCompositionAttribute (only run if native Win11 backdrop fails or is not glass)
    if (FAILED(hr) || !g_Settings.glassBackdrop || isMediaWnd) {
        HMODULE hUser = GetModuleHandle(L"user32.dll");
        if (hUser) {
            auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
            if (SetComp) {
                DWORD tint = useTintOverride ? tintOverride :
                             ((g_Settings.glassBackdrop && !isMediaWnd) ? (IsSystemLightMode() ? 0x01FFFFFF : 0x01000000) :
                             (g_Settings.autoTheme ? (IsSystemLightMode() ? 0x60FFFFFF : 0x60000000)
                                                   : ((g_Settings.bgOpacity << 24) | 0xFFFFFF)));
                ACCENT_POLICY policy;
                if (g_Settings.glassBackdrop && !isMediaWnd) {
                    policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
                } else {
                    policy = { ACCENT_DISABLED, 0, 0, 0 };
                }
                WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
                SetComp(hwnd, &data);
            }
        }
    }
}

void AddRoundedRect(GraphicsPath& path, int x, int y, int w, int h, int r) {
    int d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

// Helper: format seconds as M:SS
wstring FormatTime(double sec) {
    if (isnan(sec) || isinf(sec) || sec < 0) sec = 0;
    if (sec > 360000) sec = 360000;
    int total = (int)sec;
    int m = total / 60;
    int s = total % 60;
    wchar_t buf[16];
    swprintf_s(buf, L"%d:%02d", m, s);
    return buf;
}


// ============================================================
// Music Visualizer (Real-Time vs mock GDI+)
// ============================================================
std::thread g_CaptureThread;
std::atomic<bool> g_CaptureRunning{ false };
float g_VisRealtimeTargets[4] = { 0.15f, 0.15f, 0.15f, 0.15f };
float g_VisPeaks[4] = { 0.15f, 0.15f, 0.15f, 0.15f };
float g_VisPeakVel[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
std::mutex g_VisMutex;

void AudioCaptureThread() {
    Wh_Log(L"[Taskbar Music Lounge] AudioCaptureThread started");
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo)) {
        Wh_Log(L"[Taskbar Music Lounge] CoInitializeEx failed: 0x%08X", hrCo);
    }
    
    float targetFreqs[4] = { 80.0f, 350.0f, 1500.0f, 4000.0f };
    const int N = 256;
    std::vector<float> sampleRing(N, 0.0f);
    int ringIdx = 0;

    while (g_CaptureRunning) {
        IMMDeviceEnumerator* pEnum = nullptr;
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                         __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
        if (FAILED(hr)) {
            Wh_Log(L"[Taskbar Music Lounge] CoCreateInstance enumerator failed: 0x%08X", hr);
            Sleep(1000);
            continue;
        }
        
        IMMDevice* pDevice = nullptr;
        hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
        if (FAILED(hr) || !pDevice) {
            hr = pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        }
        pEnum->Release();
        if (FAILED(hr) || !pDevice) {
            Wh_Log(L"[Taskbar Music Lounge] GetDefaultAudioEndpoint failed: 0x%08X", hr);
            Sleep(1000);
            continue;
        }
        
        IAudioClient* pAudioClient = nullptr;
        hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);
        pDevice->Release();
        if (FAILED(hr) || !pAudioClient) {
            Wh_Log(L"[Taskbar Music Lounge] Activate IAudioClient failed: 0x%08X", hr);
            Sleep(1000);
            continue;
        }
        
        WAVEFORMATEX* pwfx = nullptr;
        hr = pAudioClient->GetMixFormat(&pwfx);
        if (FAILED(hr) || !pwfx) {
            Wh_Log(L"[Taskbar Music Lounge] GetMixFormat failed: 0x%08X", hr);
            pAudioClient->Release();
            Sleep(1000);
            continue;
        }
        
        bool isFloatFormat = true;
        int bitsPerSample = 32;
        
        if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
            WAVEFORMATEXTENSIBLE* pExt = (WAVEFORMATEXTENSIBLE*)pwfx;
            const GUID guidFloat = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
            const GUID guidPcm   = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
            
            if (IsEqualGUID(pExt->SubFormat, guidFloat)) {
                isFloatFormat = true;
            } else if (IsEqualGUID(pExt->SubFormat, guidPcm)) {
                isFloatFormat = false;
            }
            bitsPerSample = pwfx->wBitsPerSample;
        } else {
            if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
                isFloatFormat = true;
            } else if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
                isFloatFormat = false;
            }
            bitsPerSample = pwfx->wBitsPerSample;
        }
        
        Wh_Log(L"[Taskbar Music Lounge] Mix format info: channels=%d, rate=%d, bits=%d, float=%d",
               pwfx->nChannels, pwfx->nSamplesPerSec, bitsPerSample, isFloatFormat ? 1 : 0);
        
        HRESULT hrInit = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                                 AUDCLNT_STREAMFLAGS_LOOPBACK,
                                                 0, 0, pwfx, nullptr);
        if (FAILED(hrInit)) {
            Wh_Log(L"[Taskbar Music Lounge] Initialize 0-dur failed (0x%08X), trying 100ms...", hrInit);
            hrInit = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                             AUDCLNT_STREAMFLAGS_LOOPBACK,
                                             10000000 / 10, 0, pwfx, nullptr);
        }
        if (FAILED(hrInit)) {
            Wh_Log(L"[Taskbar Music Lounge] Initialize 100ms failed (0x%08X), trying 500ms...", hrInit);
            hrInit = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                             AUDCLNT_STREAMFLAGS_LOOPBACK,
                                             10000000 / 2, 0, pwfx, nullptr);
        }
        if (FAILED(hrInit)) {
            Wh_Log(L"[Taskbar Music Lounge] Initialize failed completely: 0x%08X", hrInit);
            CoTaskMemFree(pwfx);
            pAudioClient->Release();
            Sleep(1000);
            continue;
        }
        
        IAudioCaptureClient* pCaptureClient = nullptr;
        hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);
        if (FAILED(hr) || !pCaptureClient) {
            Wh_Log(L"[Taskbar Music Lounge] GetService capture failed: 0x%08X", hr);
            CoTaskMemFree(pwfx);
            pAudioClient->Release();
            Sleep(1000);
            continue;
        }
        
        int channels = pwfx->nChannels;
        int sampleRate = pwfx->nSamplesPerSec;
        CoTaskMemFree(pwfx);
        
        hr = pAudioClient->Start();
        if (FAILED(hr)) {
            Wh_Log(L"[Taskbar Music Lounge] Start audio client failed: 0x%08X", hr);
            pCaptureClient->Release();
            pAudioClient->Release();
            Sleep(1000);
            continue;
        }
        
        Wh_Log(L"[Taskbar Music Lounge] WASAPI loopback capture initialized and started successfully!");
        
        bool deviceActive = true;
        int logThrottle = 0;
        
        while (g_CaptureRunning && deviceActive) {
            Sleep(10);
            
            UINT32 numFramesAvailable = 0;
            BYTE* pData = nullptr;
            DWORD flags = 0;
            
            hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, nullptr, nullptr);
            if (SUCCEEDED(hr)) {
                if (numFramesAvailable > 0) {
                    if (logThrottle++ < 10) {
                        Wh_Log(L"[Taskbar Music Lounge] Capture active: received %d frames", numFramesAvailable);
                    }
                    if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData) {
                        for (UINT32 i = 0; i < numFramesAvailable; i++) {
                            float sum = 0.0f;
                            for (int c = 0; c < channels; c++) {
                                float sample = 0.0f;
                                if (isFloatFormat) {
                                    sample = ((float*)pData)[i * channels + c];
                                } else if (bitsPerSample == 16) {
                                    short val = ((short*)pData)[i * channels + c];
                                    sample = (float)val / 32768.0f;
                                } else if (bitsPerSample == 24) {
                                    BYTE* pSample = &pData[(i * channels + c) * 3];
                                    int val = (pSample[0]) | (pSample[1] << 8) | (pSample[2] << 16);
                                    if (val & 0x800000) val |= ~0xffffff;
                                    sample = (float)val / 8388608.0f;
                                } else if (bitsPerSample == 32) {
                                    int val = ((int*)pData)[i * channels + c];
                                    sample = (float)val / 2147483648.0f;
                                }
                                sum += sample;
                            }
                            sum /= channels;
                            
                            sampleRing[ringIdx] = sum;
                            ringIdx = (ringIdx + 1) % N;
                        }
                    } else {
                        for (UINT32 i = 0; i < numFramesAvailable; i++) {
                            sampleRing[ringIdx] = 0.0f;
                            ringIdx = (ringIdx + 1) % N;
                        }
                    }
                    pCaptureClient->ReleaseBuffer(numFramesAvailable);
                }
            } else {
                Wh_Log(L"[Taskbar Music Lounge] GetBuffer failed: 0x%08X", hr);
                if (hr == AUDCLNT_E_DEVICE_INVALIDATED || hr == AUDCLNT_E_SERVICE_NOT_RUNNING) {
                    deviceActive = false;
                }
            }
            
            float magnitudes[4] = { 0 };
            for (int b = 0; b < 4; b++) {
                float freq = targetFreqs[b];
                float k = freq * N / sampleRate;
                float omega = 2.0f * 3.14159265f * k / N;
                
                float real = 0.0f;
                float imag = 0.0f;
                for (int n = 0; n < N; n++) {
                    float val = sampleRing[(ringIdx + n) % N];
                    float win = 0.5f * (1.0f - cos(2.0f * 3.14159265f * n / (N - 1)));
                    real += val * win * cos(omega * n);
                    imag += val * win * sin(omega * n);
                }
                magnitudes[b] = sqrt(real * real + imag * imag) / N;
            }
            
            float scales[4] = { 25.0f, 35.0f, 60.0f, 150.0f };
            {
                std::lock_guard<std::mutex> guard(g_VisMutex);
                for (int b = 0; b < 4; b++) {
                    float targetVal = magnitudes[b] * scales[b];
                    targetVal = max(0.15f, min(1.0f, targetVal));
                    g_VisRealtimeTargets[b] = targetVal;
                }
            }
        }
        
        Wh_Log(L"[Taskbar Music Lounge] Exiting capture stream loop. deviceActive=%d", deviceActive ? 1 : 0);
        pAudioClient->Stop();
        pCaptureClient->Release();
        pAudioClient->Release();
    }
    
    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
    Wh_Log(L"[Taskbar Music Lounge] AudioCaptureThread stopped");
}

float g_VisBars[4] = { 0.15f, 0.15f, 0.15f, 0.15f };
float g_VisTargets[4] = { 0.15f, 0.15f, 0.15f, 0.15f };

void UpdateVisualizerFrame() {
    bool isPlaying = false;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        isPlaying = g_MediaState.isPlaying;
    }
    
    InitMeterAPI();
    
    float masterPeak = 0.0f;
    if (g_pMeter) {
        g_pMeter->GetPeakValue(&masterPeak);
    }

    if (g_Settings.realTimeVisualizer) {
        if (masterPeak > 0.01f || isPlaying) {
            std::lock_guard<std::mutex> guard(g_VisMutex);
            for (int i = 0; i < 4; i++) {
                g_VisTargets[i] = g_VisRealtimeTargets[i];
                g_VisBars[i] += (g_VisTargets[i] - g_VisBars[i]) * 0.4f;
            }
        } else {
            for (int i = 0; i < 4; i++) {
                g_VisBars[i] += (0.15f - g_VisBars[i]) * 0.25f;
            }
        }
    } else {
        // Old randomized mock visualization
        for (int i = 0; i < 4; i++) {
            if (isPlaying) {
                if (fabs(g_VisBars[i] - g_VisTargets[i]) < 0.05f) {
                    g_VisTargets[i] = 0.2f + (float)(rand() % 80) / 100.0f;
                }
                g_VisBars[i] += (g_VisTargets[i] - g_VisBars[i]) * 0.2f;
            } else {
                g_VisBars[i] += (0.15f - g_VisBars[i]) * 0.1f;
            }
        }
    }

    // Visualizer peak gravity physics
    const float gravity = 0.0018f;
    for (int i = 0; i < 4; i++) {
        float h = g_VisBars[i];
        if (h > g_VisPeaks[i]) {
            g_VisPeaks[i] = h;
            g_VisPeakVel[i] = 0.0f;
        } else {
            g_VisPeakVel[i] += gravity;
            g_VisPeaks[i] -= g_VisPeakVel[i];
            if (g_VisPeaks[i] < h) {
                g_VisPeaks[i] = h;
                g_VisPeakVel[i] = 0.0f;
            }
        }
    }

    // Swipe Card transition animation interpolation
    if (fabs(g_ArtSlideOffset - g_ArtSlideTarget) > 0.1f) {
        g_ArtSlideOffset += (g_ArtSlideTarget - g_ArtSlideOffset) * 0.15f;
    } else {
        g_ArtSlideOffset = g_ArtSlideTarget;
    }
    
    if (fabs(g_DragOffsetX) > 0.1f) {
        g_DragOffsetX += (0.0f - g_DragOffsetX) * 0.15f;
    } else {
        g_DragOffsetX = 0.0f;
    }
}

bool IsVisualizerActive() {
    for (int i = 0; i < 4; i++) {
        if (g_VisBars[i] > 0.16f) return true;
    }
    return false;
}

void DrawVisualizer(Graphics& g, float x, float y, float maxWidth, float maxHeight, Color color, float barScale = 1.0f) {
    int numBars = 4;
    float gap = 2.0f * barScale;
    float barW = 3.0f * barScale;
    
    SolidBrush brush(color);
    SolidBrush shadowBrush(Color(120, 0, 0, 0));
    
    for (int i = 0; i < numBars; i++) {
        float h = g_VisBars[i] * maxHeight;
        if (h < 2.0f) h = 2.0f;
        
        float bx = x + i * (barW + gap);
        float by = y + maxHeight - h; // bottom-aligned
        
        GraphicsPath path;
        AddRoundedRect(path, (int)bx, (int)by, (int)barW, (int)h, (int)(barW / 2));
        
        if (g_Settings.glassBackdrop) {
            GraphicsPath shadowPath;
            AddRoundedRect(shadowPath, (int)(bx + 1.0f), (int)(by + 1.0f), (int)barW, (int)h, (int)(barW / 2));
            g.FillPath(&shadowBrush, &shadowPath);
        }
        
        g.FillPath(&brush, &path);

        // Draw peak dot/dash
        float peakH = g_VisPeaks[i] * maxHeight;
        float pby = y + maxHeight - peakH;
        if (pby > by - 2.0f * barScale) pby = by - 2.0f * barScale;
        
        GraphicsPath peakPath;
        float peakSizeH = max(1.5f * barScale, 2.0f);
        AddRoundedRect(peakPath, (int)bx, (int)pby, (int)barW, (int)peakSizeH, (int)(barW / 2));
        
        if (g_Settings.glassBackdrop) {
            GraphicsPath peakShadowPath;
            AddRoundedRect(peakShadowPath, (int)(bx + 1.0f), (int)(pby + 1.0f), (int)barW, (int)peakSizeH, (int)(barW / 2));
            g.FillPath(&shadowBrush, &peakShadowPath);
        }
        g.FillPath(&brush, &peakPath);
    }
}

// ============================================================
// Vector Icon Drawers
// ============================================================
void DrawShuffleIcon(Graphics& g, float x, float y, float w, float h, Color color, bool active) {
    Pen pen(color, 1.5f);
    SolidBrush brush(color);

    PointF p1_start(x, y + h * 0.2f);
    PointF p1_c1(x + w * 0.4f, y + h * 0.2f);
    PointF p1_c2(x + w * 0.6f, y + h * 0.8f);
    PointF p1_end(x + w, y + h * 0.8f);

    PointF p2_start(x, y + h * 0.8f);
    PointF p2_c1(x + w * 0.4f, y + h * 0.8f);
    PointF p2_c2(x + w * 0.6f, y + h * 0.2f);
    PointF p2_end(x + w, y + h * 0.2f);

    g.DrawBezier(&pen, p1_start, p1_c1, p1_c2, p1_end);
    g.DrawBezier(&pen, p2_start, p2_c1, p2_c2, p2_end);

    PointF arrow1[3] = {
        { x + w, y + h * 0.8f },
        { x + w - 4.0f, y + h * 0.8f - 3.0f },
        { x + w - 4.0f, y + h * 0.8f + 3.0f }
    };
    g.FillPolygon(&brush, arrow1, 3);

    PointF arrow2[3] = {
        { x + w, y + h * 0.2f },
        { x + w - 4.0f, y + h * 0.2f - 3.0f },
        { x + w - 4.0f, y + h * 0.2f + 3.0f }
    };
    g.FillPolygon(&brush, arrow2, 3);

    if (active) {
        SolidBrush activeBr(Color(255, 29, 185, 84));
        g.FillEllipse(&activeBr, x + w / 2.0f - 1.5f, y + h + 3.0f, 3.0f, 3.0f);
    }
}

void DrawRepeatIcon(Graphics& g, float x, float y, float w, float h, float r, Color color, bool active, bool repeatOne) {
    Pen pen(color, 1.5f);
    SolidBrush brush(color);

    g.DrawLine(&pen, x + r + 2.0f, y, x + w - r, y);
    g.DrawArc(&pen, x + w - r * 2, y, r * 2, r * 2, 270, 90);
    g.DrawLine(&pen, x + w, y + r, x + w, y + h - r - 2.0f);
    g.DrawArc(&pen, x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    g.DrawLine(&pen, x + w - r - 2.0f, y + h, x + r, y + h);
    g.DrawArc(&pen, x, y + h - r * 2, r * 2, r * 2, 90, 90);
    g.DrawLine(&pen, x, y + h - r, x, y + r + 2.0f);
    g.DrawArc(&pen, x, y, r * 2, r * 2, 180, 90);

    PointF arrow1[3] = {
        { x + r + 2.0f, y },
        { x + r - 2.0f, y - 3.0f },
        { x + r - 2.0f, y + 3.0f }
    };
    g.FillPolygon(&brush, arrow1, 3);

    PointF arrow2[3] = {
        { x + w - r - 2.0f, y + h },
        { x + w - r + 2.0f, y + h - 3.0f },
        { x + w - r + 2.0f, y + h + 3.0f }
    };
    g.FillPolygon(&brush, arrow2, 3);

    if (repeatOne) {
        FontFamily ff(FONT_NAME, nullptr);
        Font font(&ff, 7.0f, FontStyleBold, UnitPixel);
        g.DrawString(L"1", -1, &font, PointF(x + w/2.0f - 2.5f, y + h/2.0f - 4.5f), &brush);
    }

    if (active) {
        SolidBrush activeBr(Color(255, 29, 185, 84));
        g.FillEllipse(&activeBr, x + w / 2.0f - 1.5f, y + h + 3.0f, 3.0f, 3.0f);
    }
}

// ============================================================
// Draw Backdrop Card & Text Shadow Helpers
// ============================================================
void DrawBackdropCard(Graphics& g, int w, int h, bool isPopup) {
    GraphicsPath path;
    int cornerRadius = isPopup ? 16 : 12;
    AddRoundedRect(path, 0, 0, w, h, cornerRadius);
    
    if (g_Settings.glassBackdrop) {
        bool isLight = IsSystemLightMode();
        // Translucent white for light mode, translucent dark gray/black for dark mode to guarantee legibility
        Color fillColor = isLight ? Color(100, 255, 255, 255) : Color(120, 20, 20, 20);
        SolidBrush fillBrush(fillColor);
        g.FillPath(&fillBrush, &path);
        
        Color borderColor = isLight ? Color(80, 255, 255, 255) : Color(40, 255, 255, 255);
        Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &path);
    } else {
        bool isLight = IsSystemLightMode();
        Color fillColor = isLight ? Color(255, 240, 240, 240) : Color(255, 20, 20, 20);
        SolidBrush fillBrush(fillColor);
        g.FillPath(&fillBrush, &path);
        
        Color borderColor = isLight ? Color(255, 200, 200, 200) : Color(255, 45, 45, 45);
        Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &path);
    }
}

void DrawTextWithShadow(Graphics& g, const wstring& text, Font* font, const RectF& rect, StringFormat* sf, Color textColor) {
    BYTE r = textColor.GetRed(), g_ = textColor.GetGreen(), b = textColor.GetBlue();
    float luminance = 0.299f * r + 0.587f * g_ + 0.114f * b;
    Color shadowColor = (luminance > 128) ? Color(160, 0, 0, 0) : Color(160, 255, 255, 255);
    
    if (g_Settings.glassBackdrop) {
        RectF shL(rect.X - 1.0f, rect.Y, rect.Width, rect.Height);
        RectF shR(rect.X + 1.0f, rect.Y, rect.Width, rect.Height);
        RectF shT(rect.X, rect.Y - 1.0f, rect.Width, rect.Height);
        RectF shB(rect.X, rect.Y + 1.0f, rect.Width, rect.Height);
        DrawStringWithEmoji(g, text, font, shL, sf, shadowColor);
        DrawStringWithEmoji(g, text, font, shR, sf, shadowColor);
        DrawStringWithEmoji(g, text, font, shT, sf, shadowColor);
        DrawStringWithEmoji(g, text, font, shB, sf, shadowColor);
    } else {
        RectF shadowRect(rect.X + 1.0f, rect.Y + 1.0f, rect.Width, rect.Height);
        DrawStringWithEmoji(g, text, font, shadowRect, sf, shadowColor);
    }
    DrawStringWithEmoji(g, text, font, rect, sf, textColor);
}

void DrawTextWithShadow(Graphics& g, const wstring& text, Font* font, const PointF& pt, Color textColor) {
    BYTE r = textColor.GetRed(), g_ = textColor.GetGreen(), b = textColor.GetBlue();
    float luminance = 0.299f * r + 0.587f * g_ + 0.114f * b;
    Color shadowColor = (luminance > 128) ? Color(160, 0, 0, 0) : Color(160, 255, 255, 255);
    
    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentNear);
    
    if (g_Settings.glassBackdrop) {
        DrawStringWithEmoji(g, text, font, RectF(pt.X - 1.0f, pt.Y, 2000.0f, 100.0f), &sf, shadowColor);
        DrawStringWithEmoji(g, text, font, RectF(pt.X + 1.0f, pt.Y, 2000.0f, 100.0f), &sf, shadowColor);
        DrawStringWithEmoji(g, text, font, RectF(pt.X, pt.Y - 1.0f, 2000.0f, 100.0f), &sf, shadowColor);
        DrawStringWithEmoji(g, text, font, RectF(pt.X, pt.Y + 1.0f, 2000.0f, 100.0f), &sf, shadowColor);
    } else {
        DrawStringWithEmoji(g, text, font, RectF(pt.X + 1.0f, pt.Y + 1.0f, 2000.0f, 100.0f), &sf, shadowColor);
    }
    DrawStringWithEmoji(g, text, font, RectF(pt.X, pt.Y, 2000.0f, 100.0f), &sf, textColor);
}

void DrawTextWithShadowNormal(Graphics& g, const wstring& text, Font* font, const RectF& rect, StringFormat* sf, Color textColor) {
    BYTE r = textColor.GetRed(), g_ = textColor.GetGreen(), b = textColor.GetBlue();
    float luminance = 0.299f * r + 0.587f * g_ + 0.114f * b;
    Color shadowColor = (luminance > 128) ? Color(160, 0, 0, 0) : Color(160, 255, 255, 255);
    
    SolidBrush shadowBrush(shadowColor);
    SolidBrush textBrush(textColor);
    
    if (g_Settings.glassBackdrop) {
        g.DrawString(text.c_str(), -1, font, RectF(rect.X - 1.0f, rect.Y, rect.Width, rect.Height), sf, &shadowBrush);
        g.DrawString(text.c_str(), -1, font, RectF(rect.X + 1.0f, rect.Y, rect.Width, rect.Height), sf, &shadowBrush);
        g.DrawString(text.c_str(), -1, font, RectF(rect.X, rect.Y - 1.0f, rect.Width, rect.Height), sf, &shadowBrush);
        g.DrawString(text.c_str(), -1, font, RectF(rect.X, rect.Y + 1.0f, rect.Width, rect.Height), sf, &shadowBrush);
    } else {
        g.DrawString(text.c_str(), -1, font, RectF(rect.X + 1.0f, rect.Y + 1.0f, rect.Width, rect.Height), sf, &shadowBrush);
    }
    g.DrawString(text.c_str(), -1, font, rect, sf, &textBrush);
}

void DrawScaledLyricLine(Graphics& g, const wstring& text, Font* baseFont, const RectF& rect, StringFormat* sf, Color textColor) {
    if (text.empty()) return;
    
    StringFormat sfWrap;
    sfWrap.SetAlignment(StringAlignmentCenter);
    sfWrap.SetLineAlignment(StringAlignmentCenter);
    sfWrap.SetTrimming(StringTrimmingNone); // Never truncate with ...
    
    // Create a tall drawing rect centered on the line Y center to allow multi-line expansion without clipping
    float centerY = rect.Y + rect.Height / 2.0f;
    float largeH = 120.0f; 
    RectF drawRect(rect.X, centerY - largeH / 2.0f, rect.Width, largeH);
    
    RectF layoutRect(0, 0, 2000.0f, largeH);
    RectF boundRect;
    g.MeasureString(text.c_str(), -1, baseFont, layoutRect, &sfWrap, &boundRect);
    
    float maxW = rect.Width - 6.0f;
    Font* fontToUse = baseFont;
    std::unique_ptr<Font> scaledFont;
    
    bool isBold = (baseFont->GetStyle() & FontStyleBold) != 0;
    
    if (isBold) {
        // Active line: allows wrapping up to 3 lines. Scale down only if extremely long (> 3 lines of width)
        if (boundRect.Width > maxW * 3.0f) {
            float scaleFactor = (maxW * 3.0f) / boundRect.Width;
            float newSize = baseFont->GetSize() * scaleFactor;
            if (newSize < 9.5f) newSize = 9.5f;
            
            FontFamily family;
            baseFont->GetFamily(&family);
            scaledFont = std::make_unique<Font>(&family, newSize, baseFont->GetStyle(), UnitPixel);
            fontToUse = scaledFont.get();
        }
    } else {
        // Inactive line: keeps on a single line by scaling down
        sfWrap.SetFormatFlags(StringFormatFlagsNoWrap);
        if (boundRect.Width > maxW && maxW > 20.0f) {
            float scaleFactor = maxW / boundRect.Width;
            float newSize = baseFont->GetSize() * scaleFactor;
            if (newSize < 9.5f) newSize = 9.5f;
            
            FontFamily family;
            baseFont->GetFamily(&family);
            scaledFont = std::make_unique<Font>(&family, newSize, baseFont->GetStyle(), UnitPixel);
            fontToUse = scaledFont.get();
        }
    }
    
    // Draw with a clean, crisp, soft dark drop shadow (+1px, +1px offset) instead of a blurry 4-way halo
    SolidBrush shadowBrush(Color(140, 0, 0, 0));
    SolidBrush textBrush(textColor);
    
    RectF shadowRect(drawRect.X + 1.0f, drawRect.Y + 1.0f, drawRect.Width, drawRect.Height);
    g.DrawString(text.c_str(), -1, fontToUse, shadowRect, &sfWrap, &shadowBrush);
    g.DrawString(text.c_str(), -1, fontToUse, drawRect, &sfWrap, &textBrush);
}


// ============================================================
// Draw Compact Bar
// ============================================================
void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAlias);
    g.Clear(Color(0, 0, 0, 0));
    DrawBackdropCard(g, width, height, false);

    // Calculate alpha ratio based on current window width (collapse state)
    float ratio = 1.0f;
    int collapsedWidth = height; // equal to panel height
    if (width < g_Settings.width) {
        ratio = (float)(width - collapsedWidth) / (g_Settings.width - collapsedWidth);
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
    }
    BYTE alpha = (BYTE)(ratio * 255);

    Color mainColor{ GetCurrentTextColor() };
    Color drawIconColor(alpha, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color drawHoverColor(alpha, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color drawActiveBgColor((BYTE)(ratio * 40), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color drawTextColor((BYTE)(ratio * mainColor.GetAlpha()), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());

    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title     = g_MediaState.title;
        state.artist    = g_MediaState.artist;
        state.albumArt  = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        state.hasMedia  = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
        state.canGoNext = g_MediaState.canGoNext;
        state.canGoPrev = g_MediaState.canGoPrev;
        state.canPlayPause = g_MediaState.canPlayPause;
        state.canShuffle = g_MediaState.canShuffle;
        state.canRepeat = g_MediaState.canRepeat;
    }

    // Album Art
    int artSize = height - 12;
    int artX = 6, artY = 6;
    GraphicsPath artPath;
    AddRoundedRect(artPath, artX, artY, artSize, artSize, 8);
    if (state.albumArt) {
        g.SetClip(&artPath);
        g.DrawImage(state.albumArt, artX, artY, artSize, artSize);
        g.ResetClip();
        delete state.albumArt;
    } else {
        SolidBrush ph{ Color((BYTE)(ratio * 40), 128, 128, 128) };
        g.FillPath(&ph, &artPath);
    }

    // Controls
    double scale = g_Settings.buttonScale;
    int startControlX = artX + artSize + (int)(12 * scale);
    int controlY = height / 2;

    SolidBrush iconBrush{ drawIconColor };
    SolidBrush hoverBrush{ drawHoverColor };
    SolidBrush activeBg{ drawActiveBgColor };

    float circleR = 12.0f * (float)scale;
    float iconW   = 8.0f  * (float)scale;
    float iconH   = 12.0f * (float)scale;
    float gap     = 28.0f * (float)scale;

    float pX = (float)startControlX;
    SolidBrush shadowBrush{ Color((BYTE)(alpha * 0.45f), 0, 0, 0) }; // High contrast drop shadow

    Color prevCol = state.canGoPrev ? (g_HoverState == 1 ? drawHoverColor : drawIconColor) : Color((BYTE)(alpha * 0.2f), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color playCol = state.canPlayPause ? (g_HoverState == 2 ? drawHoverColor : drawIconColor) : Color((BYTE)(alpha * 0.2f), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color nextCol = state.canGoNext ? (g_HoverState == 3 ? drawHoverColor : drawIconColor) : Color((BYTE)(alpha * 0.2f), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());

    SolidBrush prevBrush(prevCol);
    SolidBrush playBrush(playCol);
    SolidBrush nextBrush(nextCol);

    if (g_HoverState == 1 && state.canGoPrev) g.FillEllipse(&activeBg, pX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF prev[3] = { {pX+iconW,(float)controlY-(iconH/2)}, {pX+iconW,(float)controlY+(iconH/2)}, {pX,(float)controlY} };
    if (g_Settings.glassBackdrop && state.canGoPrev) {
        PointF prevShadow[3] = { {prev[0].X + 1.0f, prev[0].Y + 1.0f}, {prev[1].X + 1.0f, prev[1].Y + 1.0f}, {prev[2].X + 1.0f, prev[2].Y + 1.0f} };
        g.FillPolygon(&shadowBrush, prevShadow, 3);
        g.FillRectangle(&shadowBrush, pX + 1.0f, (float)controlY-(iconH/2) + 1.0f, 2.0f*(float)scale, iconH);
    }
    g.FillPolygon(&prevBrush, prev, 3);
    g.FillRectangle(&prevBrush, pX, (float)controlY-(iconH/2), 2.0f*(float)scale, iconH);

    float plX = pX + gap;
    if (g_HoverState == 2 && state.canPlayPause) g.FillEllipse(&activeBg, plX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    if (state.isPlaying) {
        float bW = 3.0f*(float)scale, bH = 14.0f*(float)scale;
        if (g_Settings.glassBackdrop && state.canPlayPause) {
            g.FillRectangle(&shadowBrush, plX-(bW+1) + 1.0f, (float)controlY-(bH/2) + 1.0f, bW, bH);
            g.FillRectangle(&shadowBrush, plX+1 + 1.0f,      (float)controlY-(bH/2) + 1.0f, bW, bH);
        }
        g.FillRectangle(&playBrush, plX-(bW+1), (float)controlY-(bH/2), bW, bH);
        g.FillRectangle(&playBrush, plX+1,      (float)controlY-(bH/2), bW, bH);
    } else {
        float pW = 10.0f*(float)scale, pH = 16.0f*(float)scale;
        PointF play[3] = { {plX-(pW/2),(float)controlY-(pH/2)}, {plX-(pW/2),(float)controlY+(pH/2)}, {plX+(pW/2),(float)controlY} };
        if (g_Settings.glassBackdrop && state.canPlayPause) {
            PointF playShadow[3] = { {play[0].X + 1.0f, play[0].Y + 1.0f}, {play[1].X + 1.0f, play[1].Y + 1.0f}, {play[2].X + 1.0f, play[2].Y + 1.0f} };
            g.FillPolygon(&shadowBrush, playShadow, 3);
        }
        g.FillPolygon(&playBrush, play, 3);
    }

    float nX = plX + gap;
    if (g_HoverState == 3 && state.canGoNext) g.FillEllipse(&activeBg, nX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF next[3] = { {nX-iconW,(float)controlY-(iconH/2)}, {nX-iconW,(float)controlY+(iconH/2)}, {nX,(float)controlY} };
    if (g_Settings.glassBackdrop && state.canGoNext) {
        PointF nextShadow[3] = { {next[0].X + 1.0f, next[0].Y + 1.0f}, {next[1].X + 1.0f, next[1].Y + 1.0f}, {next[2].X + 1.0f, next[2].Y + 1.0f} };
        g.FillPolygon(&shadowBrush, nextShadow, 3);
        g.FillRectangle(&shadowBrush, nX + 1.0f, (float)controlY-(iconH/2) + 1.0f, 2.0f*(float)scale, iconH);
    }
    g.FillPolygon(&nextBrush, next, 3);
    g.FillRectangle(&nextBrush, nX, (float)controlY-(iconH/2), 2.0f*(float)scale, iconH);

    // Text & Visualizer
    int textX = (int)(nX + (20 * scale));
    
    int visWidth = 0;
    if (g_Settings.showVisualizer && width > g_Settings.height + 40) {
        float vScale = (float)g_Settings.visualizerScale;
        float vHeight = (float)g_Settings.visualizerHeight;
        float vWidth = 4 * 3.0f * vScale + 3 * 2.0f * vScale;
        
        // Far right placement (fixed)
        float vy = (height - vHeight) / 2.0f;
        float vx = (float)(width - vWidth - 12);
        
        DrawVisualizer(g, vx, vy, vWidth, vHeight, drawIconColor, vScale);
        visWidth = (int)vWidth + 12;
    }

    int textMaxW = width - textX - visWidth - 8;

    wstring fullText = state.title;
    if (!state.artist.empty()) fullText += L" \u2022 " + state.artist;

    bool streamActive = false;
    {
        lock_guard<mutex> lguard(g_Lyrics.lock);
        streamActive = g_Lyrics.streamLyrics;
    }
    if (streamActive && state.hasMedia) {
        wstring activeLine = L"";
        double livePos = GetLivePosition();
        double lineDuration = 4.0;
        {
            lock_guard<mutex> lguard(g_Lyrics.lock);
            if (g_Lyrics.hasLyrics && !g_Lyrics.lines.empty()) {
                int activeIndex = -1;
                for (size_t i = 0; i < g_Lyrics.lines.size(); i++) {
                    if (livePos >= g_Lyrics.lines[i].timeSec) {
                        activeIndex = (int)i;
                    } else {
                        break;
                    }
                }
                if (activeIndex != -1) {
                    double lineStart = g_Lyrics.lines[activeIndex].timeSec;
                    double nextStart = 999999.0;
                    if (activeIndex + 1 < (int)g_Lyrics.lines.size()) {
                        nextStart = g_Lyrics.lines[activeIndex + 1].timeSec;
                    }
                    
                    if (livePos - lineStart > 5.0 && nextStart - livePos > 2.0) {
                        activeLine = L"";
                    } else {
                        int foundIdx = activeIndex;
                        while (foundIdx >= 0 && g_Lyrics.lines[foundIdx].text.empty()) {
                            foundIdx--;
                        }
                        if (foundIdx >= 0) {
                            activeLine = g_Lyrics.lines[foundIdx].text;
                        }
                    }
                    
                    if (activeIndex + 1 < (int)g_Lyrics.lines.size()) {
                        lineDuration = g_Lyrics.lines[activeIndex + 1].timeSec - g_Lyrics.lines[activeIndex].timeSec;
                    } else {
                        if (g_Timeline.valid && g_Timeline.durationSec > g_Lyrics.lines[activeIndex].timeSec) {
                            lineDuration = g_Timeline.durationSec - g_Lyrics.lines[activeIndex].timeSec;
                        }
                    }
                }
            }
        }
        if (!activeLine.empty()) {
            fullText = activeLine;
            
            // Measure string to calculate dynamic scroll speed
            float fs = (float)g_Settings.fontSize;
            FontFamily ffText(FONT_NAME, nullptr);
            Font fontText(&ffText, fs, FontStyleBold, UnitPixel);
            RectF layoutRect(0, 0, 10000, 100), boundRect;
            g.MeasureString(fullText.c_str(), -1, &fontText, layoutRect, &boundRect);
            float textW = boundRect.Width;
            
            float scrollMax = textW - (float)textMaxW;
            if (scrollMax > 0.0f) {
                if (lineDuration < 1.0) lineDuration = 1.0;
                if (lineDuration > 15.0) lineDuration = 15.0;
                
                float frames = (float)(lineDuration * 60.0 * 0.70) - 20.0f;
                if (frames < 20.0f) frames = 20.0f;
                
                g_LyricScrollSpeed = scrollMax / frames;
                if (g_LyricScrollSpeed < 1.5f) g_LyricScrollSpeed = 1.5f;
                if (g_LyricScrollSpeed > 10.0f) g_LyricScrollSpeed = 10.0f;
            } else {
                g_LyricScrollSpeed = 1.5f;
            }
        } else {
            g_LyricScrollSpeed = 1.5f;
        }
    } else {
        g_LyricScrollSpeed = 1.5f;
    }

    static wstring lastFullText = L"";
    if (fullText != lastFullText) {
        lastFullText = fullText;
        g_ScrollOffset = 0.0f;
        g_ScrollWait = streamActive ? 12 : 60;
    }

    FontFamily ff(FONT_NAME, nullptr);
    Font font(&ff, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);

    RectF layoutRect(0, 0, 2000, 100), boundRect;
    g.MeasureString(fullText.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width + 8;

    Region clip(Rect(textX, 0, textMaxW, height));
    g.SetClip(&clip);
    float textY = (height - boundRect.Height) / 2.0f;

    if (g_TextWidth > textMaxW) {
        g_IsScrolling = true;
        float drawX = (float)(textX - g_ScrollOffset);
        DrawTextWithShadow(g, fullText, &font, PointF(drawX, textY), drawTextColor);
        if (drawX + g_TextWidth < width)
            DrawTextWithShadow(g, fullText, &font, PointF(drawX + g_TextWidth + 40, textY), drawTextColor);
    } else {
        g_IsScrolling = false;
        g_ScrollOffset = 0.0f;
        DrawTextWithShadow(g, fullText, &font, PointF((float)textX, textY), drawTextColor);
    }
}

// ============================================================
// Draw Expanded Popup  (Mac-style Now Playing card)
// ============================================================
void DrawExpandedPanel(HDC hdc, int width, int height) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    g.Clear(Color(0, 0, 0, 0));
    DrawBackdropCard(g, width, height, true);

    bool lightMode = IsSystemLightMode();
    Color textMain  { GetCurrentTextColor() };
    BYTE dimA = 160;
    Color textDim { dimA, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue() };

    MediaState state;
    Bitmap* appIcon = nullptr;
    bool shuffle = false;
    winrt::Windows::Media::MediaPlaybackAutoRepeatMode repeatMode =
        winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title    = g_MediaState.title;
        state.artist   = g_MediaState.artist;
        state.appName  = g_MediaState.appName;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying= g_MediaState.isPlaying;
        state.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        appIcon        = g_MediaState.appIcon ? (Bitmap*)g_MediaState.appIcon->Clone() : nullptr;
        shuffle        = g_MediaState.shuffle;
        repeatMode     = g_MediaState.repeatMode;
        state.canGoNext = g_MediaState.canGoNext;
        state.canGoPrev = g_MediaState.canGoPrev;
        state.canPlayPause = g_MediaState.canPlayPause;
        state.canShuffle = g_MediaState.canShuffle;
        state.canRepeat = g_MediaState.canRepeat;
    }

    int sessionCount = 0;
    try {
        if (g_SessionManager) sessionCount = (int)g_SessionManager.GetSessions().Size();
    } catch (...) {}

    const int pad = 16;

    // ---- Row 1: Opposite Sides App Switcher + Centered Streaming App Name ----
    int rowY = pad;
    int iconSize = g_Settings.popupIconSize;
    
    // Draw Session arrow switchers if multiple sessions exist
    if (sessionCount > 1) {
        SolidBrush arrowBr{ (g_ExpHoverBtn == 6 || g_ExpHoverBtn == 7) ? textMain : textDim };
        
        // Left Switcher arrow [<] (Far Left)
        if (g_ExpHoverBtn == 6) {
            SolidBrush activeBr{ Color(40, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) };
            g.FillEllipse(&activeBr, pad - 4, rowY - 4, 24, 24);
        }
        PointF leftArrow[3] = {
            { (float)pad + 10.0f, (float)rowY + 2.0f },
            { (float)pad + 4.0f, (float)rowY + 8.0f },
            { (float)pad + 10.0f, (float)rowY + 14.0f }
        };
        g.FillPolygon(&arrowBr, leftArrow, 3);
        
        // Right Switcher arrow [>] (Far Right)
        int rightArrowX = width - pad - 20;
        if (g_ExpHoverBtn == 7) {
            SolidBrush activeBr{ Color(40, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) };
            g.FillEllipse(&activeBr, rightArrowX - 4, rowY - 4, 24, 24);
        }
        PointF rightArrow[3] = {
            { (float)rightArrowX + 4.0f, (float)rowY + 2.0f },
            { (float)rightArrowX + 10.0f, (float)rowY + 8.0f },
            { (float)rightArrowX + 4.0f, (float)rowY + 14.0f }
        };
        g.FillPolygon(&arrowBr, rightArrow, 3);
    }

    // App header bounds (centered between switchers)
    int midStartX = (sessionCount > 1) ? pad + 24 + 6 : pad;
    int midEndX   = (sessionCount > 1) ? width - pad - 24 - 6 : width - pad;
    int midWidth  = midEndX - midStartX;

    wstring appLabel = state.appName.empty() ? L"Now Playing" : state.appName;
    appLabel += L" \u2197";

    if (sessionCount > 1) {
        wchar_t countBuf[32];
        swprintf_s(countBuf, L" (%d/%d)", g_CurrentSessionIndex + 1, sessionCount);
        appLabel += countBuf;
    }

    FontFamily ffHeader(FONT_NAME, nullptr);
    float appNameSize = (float)g_Settings.popupFontSize - 1.0f;
    if (appNameSize < 8.0f) appNameSize = 8.0f;
    Font fontApp(&ffHeader, appNameSize, FontStyleRegular, UnitPixel);

    RectF layoutRectApp(0, 0, 2000, 100), boundRectApp;
    g.MeasureString(appLabel.c_str(), -1, &fontApp, layoutRectApp, &boundRectApp);
    int appTextWidth = (int)boundRectApp.Width;
    g_AppTextWidth = appTextWidth; // store for scrolling calculations

    int totalHeaderWidth = (appIcon ? iconSize + 6 : 0) + appTextWidth;
    int headerX = midStartX + (midWidth - totalHeaderWidth) / 2;
    if (headerX < midStartX) headerX = midStartX;

    if (g_ExpHoverBtn == 9) {
        SolidBrush activeBr{ Color(45, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) };
        GraphicsPath hdrPath;
        AddRoundedRect(hdrPath, headerX - 6, rowY - 4, totalHeaderWidth + 12, iconSize + 8, 6);
        g.FillPath(&activeBr, &hdrPath);
    }

    int drawIconX = headerX;
    int drawTextX = headerX + (appIcon ? iconSize + 6 : 0);
    int drawTextMaxW = midEndX - drawTextX;

    if (appIcon) {
        GraphicsPath iconClip;
        AddRoundedRect(iconClip, drawIconX, rowY, iconSize, iconSize, iconSize / 2);
        g.SetClip(&iconClip);
        g.DrawImage(appIcon, drawIconX, rowY, iconSize, iconSize);
        g.ResetClip();
    }

    // Draw scrolling App Name if too long
    if (appTextWidth > drawTextMaxW) {
        g_IsAppScrolling = true;
        Region clip(Rect(drawTextX, 0, drawTextMaxW, height));
        g.SetClip(&clip);
        
        float textYOffset = (float)rowY + (iconSize - appNameSize) / 2.0f - 1.0f;
        float drawX = (float)(drawTextX - g_AppScrollOffset);
        DrawTextWithShadow(g, appLabel, &fontApp, PointF(drawX, textYOffset), textDim);
        if (drawX + appTextWidth < midEndX) {
            DrawTextWithShadow(g, appLabel, &fontApp, PointF(drawX + appTextWidth + 40, textYOffset), textDim);
        }
        g.ResetClip();
    } else {
        g_IsAppScrolling = false;
        g_AppScrollOffset = 0;
        float textYOffset = (float)rowY + (iconSize - appNameSize) / 2.0f - 1.0f;
        DrawTextWithShadow(g, appLabel, &fontApp, PointF((float)drawTextX, textYOffset), textDim);
    }

    // Visualizer top-right (offset to avoid switcher overlap if needed)
    int visMargin = (sessionCount > 1) ? pad + 24 + 10 : pad;
    if (IsVisualizerActive()) {
        float vScale = 1.0f;
        float vHeight = 12.0f;
        float vWidth = 4 * 3.0f * vScale + 3 * 2.0f * vScale;
        float vx = (float)(width - visMargin - vWidth);
        float vy = (float)rowY + (iconSize - vHeight) / 2.0f;
        DrawVisualizer(g, vx, vy, vWidth, vHeight, textMain, vScale);
    }
    rowY += iconSize + 10;

    // Apply swipe card offset and slide transition transitions only to specific Card components
    g.TranslateTransform(g_DragOffsetX + g_ArtSlideOffset, 0.0f);

    // ---- Row 2: Large Album Art or Lyrics (centered) ----
    int artSize = width - pad * 2;
    int maxArtSize = height - rowY - 180;
    if (maxArtSize < 80) maxArtSize = 80;
    artSize = min(artSize, maxArtSize);
    int artX = (width - artSize) / 2;
    int artY = rowY;

    bool showLyrics = false;
    bool hasLyrics = false;
    wstring lyricsText;
    vector<LyricLine> lyricsLines;
    {
        lock_guard<mutex> lguard(g_Lyrics.lock);
        showLyrics = g_Lyrics.showLyrics;
        hasLyrics = g_Lyrics.hasLyrics;
        lyricsText = g_Lyrics.plainText;
        lyricsLines = g_Lyrics.lines;
    }

    if (g_Settings.fetchLyrics && showLyrics && hasLyrics) {
        GraphicsPath lyricPath;
        AddRoundedRect(lyricPath, artX, artY, artSize, artSize, 12);
        
        SolidBrush lyricBg(Color(180, 10, 10, 10));
        g.FillPath(&lyricBg, &lyricPath);
        
        if (!lyricsLines.empty()) {
            double livePos = GetLivePosition();
            int activeIndex = -1;
            for (size_t i = 0; i < lyricsLines.size(); i++) {
                if (livePos >= lyricsLines[i].timeSec) {
                    activeIndex = (int)i;
                } else {
                    break;
                }
            }
            
            float lineHeight = 38.0f;
            if (activeIndex != -1) {
                g_LyricsTargetScroll = activeIndex * lineHeight;
            } else {
                g_LyricsTargetScroll = 0.0f;
            }
            
            // Trigger animation timer to slide smoothly if offsets mismatch
            if (fabs(g_LyricsScrollOffset - g_LyricsTargetScroll) > 0.1f && g_hExpandedWindow) {
                SetTimer(g_hExpandedWindow, IDT_ANIMATION, 16, NULL);
            }

            float fs = (float)g_Settings.lyricsFontSize;
            FontFamily ff(FONT_NAME, nullptr);
            Font lyricFont(&ff, fs, FontStyleRegular, UnitPixel);
            Font lyricBoldFont(&ff, fs + 2.0f, FontStyleBold, UnitPixel);
            
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            
            g.SetClip(&lyricPath);
            
            int boxCenterY = artY + artSize / 2;
            
            float activeShift = 0.0f;
            if (activeIndex >= 0 && activeIndex < (int)lyricsLines.size()) {
                StringFormat sfWrap;
                sfWrap.SetAlignment(StringAlignmentCenter);
                sfWrap.SetLineAlignment(StringAlignmentCenter);
                float maxW = (float)artSize - 6.0f;
                RectF layoutRect(0, 0, maxW, 200.0f);
                RectF boundRect;
                g.MeasureString(lyricsLines[activeIndex].text.c_str(), -1, &lyricBoldFont, layoutRect, &sfWrap, &boundRect);
                if (boundRect.Height > 22.0f) {
                    activeShift = (boundRect.Height - 16.0f) / 2.0f + 14.0f; // Gap size dynamically proportional to height
                }
            }

            for (int i = 0; i < (int)lyricsLines.size(); i++) {
                float lineY = (float)boxCenterY + (i * lineHeight) - g_LyricsScrollOffset;
                if (i < activeIndex) {
                    lineY -= activeShift;
                } else if (i > activeIndex) {
                    lineY += activeShift;
                }
                
                if (lineY < artY - 30.0f || lineY > artY + artSize + 30.0f) {
                    continue;
                }
                
                float distance = fabs((float)i - (g_LyricsScrollOffset / lineHeight));
                float factor = 1.0f - (distance / 2.5f);
                if (factor < 0.25f) factor = 0.25f;
                if (factor > 1.0f) factor = 1.0f;
                
                Color lineCol;
                Font* lineFont = &lyricFont;
                if (i == activeIndex) {
                    lineCol = Color(255, 29, 185, 84); // Spotify Green
                    lineFont = &lyricBoldFont;
                } else {
                    BYTE alpha = (BYTE)(factor * 160.0f);
                    lineCol = Color(alpha, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue());
                }
                
                RectF lineRect((float)artX, lineY - lineHeight / 2.0f, (float)artSize, lineHeight);
                DrawScaledLyricLine(g, lyricsLines[i].text, lineFont, lineRect, &sf, lineCol);
            }
            g.ResetClip();

            // Draw Scroll Bar in Lyrics Card
            int L = (int)lyricsLines.size();
            if (L > 1) {
                float totalVirtualH = L * lineHeight;
                float visibleH = (float)artSize;
                if (totalVirtualH > visibleH) {
                    float visibleRatio = visibleH / totalVirtualH;
                    float trackH = (float)artSize - 40.0f;
                    float thumbH = trackH * visibleRatio;
                    if (thumbH < 15.0f) thumbH = 15.0f;
                    
                    float pct = g_LyricsScrollOffset / ((L - 1) * lineHeight);
                    pct = max(0.0f, min(1.0f, pct));
                    float thumbY = (float)artY + 20.0f + pct * (trackH - thumbH);
                    
                    SolidBrush thumbBrush(Color(90, 255, 255, 255));
                    GraphicsPath thumbPath;
                    AddRoundedRect(thumbPath, artX + artSize - 10, (int)thumbY, 6, (int)thumbH, 3);
                    g.FillPath(&thumbBrush, &thumbPath);
                }
            }
        } else if (!lyricsText.empty()) {
            // Plain Lyrics (Draw first 5 lines)
            float fs = (float)g_Settings.lyricsFontSize - 1.0f;
            FontFamily ff(FONT_NAME, nullptr);
            Font lyricFont(&ff, fs, FontStyleRegular, UnitPixel);
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            
            wstringstream ss(lyricsText);
            wstring line;
            int lineY = artY + 12;
            int drawCount = 0;
            while (getline(ss, line) && drawCount < 6) {
                if (line.empty()) continue;
                RectF rect((float)artX, (float)lineY, (float)artSize, 20.0f);
                DrawScaledLyricLine(g, line, &lyricFont, rect, &sf, textMain);
                lineY += 24;
                drawCount++;
            }
        }
        
        if (state.albumArt) {
            delete state.albumArt;
        }
    } else {
        // Draw standard Album Art
        GraphicsPath artPath;
        AddRoundedRect(artPath, artX, artY, artSize, artSize, 12);
        if (state.albumArt) {
            g.SetClip(&artPath);
            g.DrawImage(state.albumArt, artX, artY, artSize, artSize);
            g.ResetClip();
            delete state.albumArt;
            
            // Draw a corner badge if lyrics are available for this track
            if (hasLyrics) {
                int badgeW = 60, badgeH = 18;
                int badgeX = artX + artSize - badgeW - 8;
                int badgeY = artY + artSize - badgeH - 8;
                
                GraphicsPath badgePath;
                AddRoundedRect(badgePath, badgeX, badgeY, badgeW, badgeH, 4);
                SolidBrush badgeBg(Color(180, 0, 0, 0));
                g.FillPath(&badgeBg, &badgePath);
                
                FontFamily ffBadge(FONT_NAME, nullptr);
                Font badgeFont(&ffBadge, 8.5f, FontStyleBold, UnitPixel);
                StringFormat sfBadge;
                sfBadge.SetAlignment(StringAlignmentCenter);
                sfBadge.SetLineAlignment(StringAlignmentCenter);
                RectF badgeRect((float)badgeX, (float)badgeY, (float)badgeW, (float)badgeH);
                DrawStringWithEmoji(g, L"Lyrics 💬", &badgeFont, badgeRect, &sfBadge, Color(255, 255, 255, 255));
            }
        } else {
            SolidBrush ph{ Color(60, 128, 128, 128) };
            g.FillPath(&ph, &artPath);
            FontFamily ff(L"Segoe UI Emoji", nullptr);
            Font noteFont(&ff, (float)(artSize / 4), FontStyleRegular, UnitPixel);
            SolidBrush noteBrush{ Color(100, 200, 200, 200) };
            RectF noteRect((float)artX, (float)artY, (float)artSize, (float)artSize);
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(L"\U0001F3B5", -1, &noteFont, noteRect, &sf, &noteBrush);
        }
    }
    rowY += artSize + 12;

    // ---- Row 3: Title ----
    {
        FontFamily ff(FONT_NAME, nullptr);
        Font titleFont(&ff, (float)g_Settings.popupFontSize + 2.0f, FontStyleBold, UnitPixel);
        int maxW = width - pad * 2;
        
        RectF layoutRect(0, 0, 2000, 100), boundRect;
        g.MeasureString(state.title.c_str(), -1, &titleFont, layoutRect, &boundRect);
        g_TitleTextWidth = (int)boundRect.Width;
        
        if (g_TitleTextWidth > maxW) {
            g_IsTitleScrolling = true;
            Region clip(Rect(pad, rowY, maxW, 25));
            g.SetClip(&clip);
            
            float drawX = (float)(pad - g_TitleScrollOffset);
            DrawTextWithShadow(g, state.title, &titleFont, PointF(drawX, (float)rowY), textMain);
            if (drawX + g_TitleTextWidth < pad + maxW) {
                DrawTextWithShadow(g, state.title, &titleFont, PointF(drawX + g_TitleTextWidth + 40, (float)rowY), textMain);
            }
            g.ResetClip();
        } else {
            g_IsTitleScrolling = false;
            g_TitleScrollOffset = 0;
            RectF titleRect((float)pad, (float)rowY, (float)maxW, 22.0f);
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetFormatFlags(StringFormatFlagsNoWrap);
            DrawTextWithShadow(g, state.title, &titleFont, titleRect, &sf, textMain);
        }
    }
    rowY += 22;

    // ---- Row 4: Artist ----
    {
        FontFamily ff(FONT_NAME, nullptr);
        Font artFont(&ff, (float)g_Settings.popupFontSize - 1.0f, FontStyleRegular, UnitPixel);
        int maxW = width - pad * 2;
        wstring artistStr = state.artist.empty() ? L"Unknown Artist" : state.artist;
        
        RectF layoutRect(0, 0, 2000, 100), boundRect;
        g.MeasureString(artistStr.c_str(), -1, &artFont, layoutRect, &boundRect);
        g_ArtistTextWidth = (int)boundRect.Width;

        int pillW = g_ArtistTextWidth + 24;
        if (pillW > maxW) pillW = maxW;
        int pillX = (width - pillW) / 2;
        
        int textHeight = (int)boundRect.Height;
        if (textHeight <= 0) textHeight = (int)(g_Settings.popupFontSize - 1.0f);
        int verticalPadding = 6;
        int pillH = textHeight + verticalPadding;
        int pillY = rowY;

        GraphicsPath pillPath;
        AddRoundedRect(pillPath, pillX, pillY, pillW, pillH, 6);
        Color plateColor = Color(200, 15, 15, 15);
        SolidBrush plateBr(plateColor);
        g.FillPath(&plateBr, &pillPath);
        
        Color borderColor = Color(40, 255, 255, 255);
        Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &pillPath);

        float textYOffset = (float)pillY + (pillH - textHeight) / 2.0f - 0.5f;
        Color artistTextColor = Color(255, 245, 245, 245);

        if (g_ArtistTextWidth > pillW - 16) {
            g_IsArtistScrolling = true;
            Region clip(Rect(pillX + 8, pillY, pillW - 16, pillH));
            g.SetClip(&clip);
            
            float drawX = (float)(pillX + 8 - g_ArtistScrollOffset);
            DrawTextWithShadow(g, artistStr, &artFont, PointF(drawX, textYOffset), artistTextColor);
            if (drawX + g_ArtistTextWidth < pillX + pillW - 8) {
                DrawTextWithShadow(g, artistStr, &artFont, PointF(drawX + g_ArtistTextWidth + 30, textYOffset), artistTextColor);
            }
            g.ResetClip();
        } else {
            g_IsArtistScrolling = false;
            g_ArtistScrollOffset = 0;
            RectF artRect((float)pillX, textYOffset, (float)pillW, (float)textHeight + 2.0f);
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetFormatFlags(StringFormatFlagsNoWrap);
            DrawTextWithShadow(g, artistStr, &artFont, artRect, &sf, artistTextColor);
        }
        rowY = pillY + pillH;
    }

    // ---- Row 4.5: Lyrics & Stream to Taskbar Buttons ----
    if (g_Settings.fetchLyrics) {
        int btnGap = 8;
        int btnW = 90;
        int btnH = 20;
        int totalW = btnW + btnGap + btnW;
        int startX = (width - totalW) / 2;
        
        int toggleX = startX;
        int lrcBtnX = startX + btnW + btnGap;
        int btnY = rowY + 2;

        bool hasLrc = false;
        bool activeLrc = false;
        wstring lrcStatus = L"Lyrics 💬";
        
        wstring currentTitle;
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            currentTitle = g_MediaState.title;
        }

        // Check lyrics status for all players
        {
            lock_guard<mutex> lguard(g_Lyrics.lock);
            if (g_Lyrics.trackTitle == currentTitle) {
                if (g_Lyrics.hasLyrics) {
                    hasLrc = true;
                    activeLrc = g_Lyrics.showLyrics;
                    lrcStatus = L"Lyrics 💬";
                } else if (g_Lyrics.lines.empty() && g_Lyrics.plainText.empty() && g_Lyrics.trackTitle.empty()) {
                    lrcStatus = L"Loading... ⏳";
                } else {
                    lrcStatus = L"No Lyrics 🚫";
                }
            } else {
                lrcStatus = L"Loading... ⏳";
            }
        }

        // 1. Draw Stream to Taskbar button (on the left)
        GraphicsPath tPath;
        AddRoundedRect(tPath, toggleX, btnY, btnW, btnH, 5);
        
        bool hoverToggle = (g_ExpHoverBtn == 10);
        bool streamActive = false;
        {
            lock_guard<mutex> lguard(g_Lyrics.lock);
            streamActive = g_Lyrics.streamLyrics;
        }
        
        Color tBgColor = streamActive ? Color(255, 29, 185, 84) : ((g_ExpHoverBtn == 10) ? Color(60, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) : Color(20, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()));
        Color tBorderColor = streamActive ? Color(255, 29, 185, 84) : textDim;
        Color tTxtColor = streamActive ? Color(255, 255, 255, 255) : textMain;
        
        SolidBrush tBgBrush(tBgColor);
        g.FillPath(&tBgBrush, &tPath);
        Pen tBorderPen(tBorderColor, 1.0f);
        g.DrawPath(&tBorderPen, &tPath);
        
        FontFamily ffBtn(FONT_NAME, nullptr);
        Font btnFont(&ffBtn, 8.5f, FontStyleBold, UnitPixel);
        StringFormat sfBtn;
        sfBtn.SetAlignment(StringAlignmentCenter);
        sfBtn.SetLineAlignment(StringAlignmentCenter);
        
        wstring toggleText = streamActive ? L"Live Ticker 🟢" : L"Live Ticker ⚪";
        RectF tRect((float)toggleX, (float)btnY, (float)btnW, (float)btnH);
        DrawTextWithShadow(g, toggleText, &btnFont, tRect, &sfBtn, tTxtColor);

        // 2. Draw Lyrics button (on the right)
        GraphicsPath btnPath;
        AddRoundedRect(btnPath, lrcBtnX, btnY, btnW, btnH, 5);
        
        Color btnBgColor;
        Color btnBorderColor;
        Color btnTxtColor;
        
        if (hasLrc) {
            if (activeLrc) {
                btnBgColor = Color(255, 29, 185, 84); // Spotify Green
                btnBorderColor = Color(255, 29, 185, 84);
                btnTxtColor = Color(255, 255, 255, 255);
            } else {
                btnBgColor = (g_ExpHoverBtn == 8) ? Color(60, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) : Color(20, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue());
                btnBorderColor = textDim;
                btnTxtColor = textMain;
            }
        } else {
            // Disabled style
            btnBgColor = Color(15, 128, 128, 128);
            btnBorderColor = Color(30, 128, 128, 128);
            btnTxtColor = textDim;
        }

        SolidBrush btnBgBrush(btnBgColor);
        g.FillPath(&btnBgBrush, &btnPath);
        
        Pen btnBorderPen(btnBorderColor, 1.0f);
        g.DrawPath(&btnBorderPen, &btnPath);
        
        RectF btnRect((float)lrcBtnX, (float)btnY, (float)btnW, (float)btnH);
        DrawTextWithShadow(g, lrcStatus, &btnFont, btnRect, &sfBtn, btnTxtColor);
        rowY += 24;
    }

    // ---- Row 5: Seeker bar ----
    {
        int seekTrackH = 4;
        int seekY = rowY + 18;
        int seekX = pad;
        int seekW = width - pad * 2;

        float seekPct = 0.0f;
        double livePos = GetLivePosition();
        if (g_Timeline.valid && g_Timeline.durationSec > 0.0)
            seekPct = (float)(livePos / g_Timeline.durationSec);
        seekPct = max(0.0f, min(1.0f, seekPct));

        bool canSeek = g_Timeline.canSeek;

        GraphicsPath trackPath;
        AddRoundedRect(trackPath, seekX, seekY, seekW, seekTrackH, seekTrackH/2);
        
        Color trackColor = canSeek ? 
            Color(60, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) :
            (lightMode ? Color(40, 128, 128, 128) : Color(40, 80, 80, 80));
        SolidBrush trackBrush{ trackColor };
        g.FillPath(&trackBrush, &trackPath);

        int fillW = (int)(seekW * seekPct);
        if (fillW > seekTrackH) {
            GraphicsPath fillPath;
            AddRoundedRect(fillPath, seekX, seekY, fillW, seekTrackH, seekTrackH/2);
            
            Color fillColor = canSeek ? textMain : (lightMode ? Color(100, 128, 128, 128) : Color(100, 180, 180, 180));
            SolidBrush fillBrush{ fillColor };
            g.FillPath(&fillBrush, &fillPath);
        }

        if (canSeek && (g_ExpHoverSeek || g_SeekDragging)) {
            int thumbX = seekX + (int)(seekW * seekPct);
            int thumbR = 6;
            SolidBrush thumbBrush{ textMain };
            g.FillEllipse(&thumbBrush,
                          thumbX - thumbR, seekY + seekTrackH/2 - thumbR,
                          thumbR*2, thumbR*2);
        }

        FontFamily ff(FONT_NAME, nullptr);
        float timeFontSize = (float)g_Settings.popupFontSize - 2.0f;
        if (timeFontSize < 8.0f) timeFontSize = 8.0f;
        Font timeFont(&ff, timeFontSize, FontStyleRegular, UnitPixel);

        wstring posStr = g_Timeline.valid ? FormatTime(livePos) : L"0:00";
        wstring durStr = g_Timeline.valid ? FormatTime(g_Timeline.durationSec) : L"0:00";

        DrawTextWithShadow(g, posStr, &timeFont, PointF((float)seekX, (float)(seekY + seekTrackH + 3)), textDim);
        
        RectF durMeasure(0,0,200,20);
        RectF durBound;
        g.MeasureString(durStr.c_str(), -1, &timeFont, durMeasure, &durBound);
        DrawTextWithShadow(g, durStr, &timeFont,
                     PointF((float)(seekX + seekW - durBound.Width), (float)(seekY + seekTrackH + 3)),
                     textDim);
    }
    rowY += 28;

    // Reset Translate offset for static panel UI controls (buttons, volume slider)
    g.ResetTransform();

    // ---- Row 6: Shuffle / Prev / Play-Pause / Next / Repeat controls ----
    {
        int controlY = rowY + 16;
        float btnScale = 1.2f;
        float btnCircR = 14.0f * btnScale;
        float btnIconW = 9.0f  * btnScale;
        float btnIconH = 13.0f * btnScale;
        float btnGap   = 36.0f * btnScale;

        float centerX = width / 2.0f;
        float shX  = centerX - btnGap * 2.0f;
        float pX   = centerX - btnGap;
        float plX  = centerX;
        float nX   = centerX + btnGap;
        float rpX  = centerX + btnGap * 2.0f;

        SolidBrush iconBr{ textMain };
        SolidBrush hoverBr{ Color(255, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) };
        SolidBrush activeBr{ Color(50, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) };
        
        Color plateColor = lightMode ? Color(160, 220, 220, 220) : Color(160, 45, 45, 45);
        SolidBrush plateBr{ plateColor };

        // 1. Shuffle
        bool shActive = shuffle && state.canShuffle;
        Color shColor = state.canShuffle ? (shActive ? Color(255, 29, 185, 84) : (g_ExpHoverBtn == 4 ? textMain : textDim)) : Color(30, textDim.GetRed(), textDim.GetGreen(), textDim.GetBlue());
        g.FillEllipse(&plateBr, shX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        if (g_ExpHoverBtn == 4 && state.canShuffle) g.FillEllipse(&activeBr, shX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        DrawShuffleIcon(g, shX - 8.0f, (float)controlY - 6.0f, 16.0f, 12.0f, shColor, shActive);

        // 2. Prev
        Color prevBtnColor = state.canGoPrev ? (g_ExpHoverBtn == 1 ? textMain : textDim) : Color(30, textDim.GetRed(), textDim.GetGreen(), textDim.GetBlue());
        SolidBrush prevBtnBrush(prevBtnColor);
        g.FillEllipse(&plateBr, pX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        if (g_ExpHoverBtn == 1 && state.canGoPrev) g.FillEllipse(&activeBr, pX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        {
            float totalW = btnIconW + 2.5f * btnScale;
            float iconLeft = pX - totalW / 2.0f;
            float triLeft = iconLeft + 2.5f * btnScale;
            PointF prev[3] = { {iconLeft + totalW, (float)controlY - (btnIconH / 2.0f)}, {iconLeft + totalW, (float)controlY + (btnIconH / 2.0f)}, {triLeft, (float)controlY} };
            g.FillPolygon(&prevBtnBrush, prev, 3);
            g.FillRectangle(&prevBtnBrush, iconLeft, (float)controlY - (btnIconH / 2.0f), 2.5f * btnScale, btnIconH);
        }

        // 3. Play/Pause
        Color playBtnColor = state.canPlayPause ? (g_ExpHoverBtn == 2 ? textMain : textDim) : Color(30, textDim.GetRed(), textDim.GetGreen(), textDim.GetBlue());
        SolidBrush playBtnBrush(playBtnColor);
        g.FillEllipse(&plateBr, plX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        if (g_ExpHoverBtn == 2 && state.canPlayPause) g.FillEllipse(&activeBr, plX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        if (state.isPlaying) {
            float bW = 3.5f*btnScale, bH = 15.0f*btnScale;
            g.FillRectangle(&playBtnBrush, plX-(bW+1.5f), (float)controlY-(bH/2), bW, bH);
            g.FillRectangle(&playBtnBrush, plX+1.5f,      (float)controlY-(bH/2), bW, bH);
        } else {
            float pW = 11.0f*btnScale, pH = 17.0f*btnScale;
            PointF play[3] = { {plX-(pW/2),(float)controlY-(pH/2)}, {plX-(pW/2),(float)controlY+(pH/2)}, {plX+(pW/2),(float)controlY} };
            g.FillPolygon(&playBtnBrush, play, 3);
        }

        // 4. Next
        Color nextBtnColor = state.canGoNext ? (g_ExpHoverBtn == 3 ? textMain : textDim) : Color(30, textDim.GetRed(), textDim.GetGreen(), textDim.GetBlue());
        SolidBrush nextBtnBrush(nextBtnColor);
        g.FillEllipse(&plateBr, nX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        if (g_ExpHoverBtn == 3 && state.canGoNext) g.FillEllipse(&activeBr, nX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        {
            float totalW = btnIconW + 2.5f * btnScale;
            float iconLeft = nX - totalW / 2.0f;
            float triRight = iconLeft + btnIconW;
            PointF nxt[3] = { {iconLeft, (float)controlY - (btnIconH / 2.0f)}, {iconLeft, (float)controlY + (btnIconH / 2.0f)}, {triRight, (float)controlY} };
            g.FillPolygon(&nextBtnBrush, nxt, 3);
            g.FillRectangle(&nextBtnBrush, triRight, (float)controlY - (btnIconH / 2.0f), 2.5f * btnScale, btnIconH);
        }

        // 5. Repeat
        bool rpActive = (repeatMode != winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None) && state.canRepeat;
        bool rpOne = (repeatMode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track) && state.canRepeat;
        Color rpColor = state.canRepeat ? (rpActive ? Color(255, 29, 185, 84) : (g_ExpHoverBtn == 5 ? textMain : textDim)) : Color(30, textDim.GetRed(), textDim.GetGreen(), textDim.GetBlue());
        g.FillEllipse(&plateBr, rpX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        if (g_ExpHoverBtn == 5 && state.canRepeat) g.FillEllipse(&activeBr, rpX-btnCircR, (float)controlY-btnCircR, btnCircR*2, btnCircR*2);
        DrawRepeatIcon(g, rpX - 8.0f, (float)controlY - 6.0f, 16.0f, 12.0f, 4.0f, rpColor, rpActive, rpOne);
    }
    rowY += 44;

    // ---- Row 7: Volume slider ----
    {
        int volY = rowY + 4;
        int volX = pad + 20;
        int volW = width - pad*2 - 40;
        int volH = 4;

        float volPct = g_IsMuted ? 0.0f : g_VolumeLevel;

        FontFamily ff(L"Segoe UI Emoji", nullptr);
        Font iconFont(&ff, 11.0f, FontStyleRegular, UnitPixel);
        const wchar_t* spkIcon = (g_IsMuted || g_VolumeLevel < 0.01f) ? L"\U0001F507"
                               : (g_VolumeLevel < 0.4f)               ? L"\U0001F508"
                               : (g_VolumeLevel < 0.7f)               ? L"\U0001F509"
                                                                        : L"\U0001F50A";
        DrawTextWithShadow(g, spkIcon, &iconFont, PointF((float)pad, (float)volY - 2.0f), textDim);

        // Track
        GraphicsPath volTrack;
        AddRoundedRect(volTrack, volX, volY, volW, volH, volH/2);
        SolidBrush trackBrush{ Color(60, textMain.GetRed(), textMain.GetGreen(), textMain.GetBlue()) };
        g.FillPath(&trackBrush, &volTrack);

        // Fill
        int fillW = (int)(volW * volPct);
        if (fillW > volH) {
            GraphicsPath volFill;
            AddRoundedRect(volFill, volX, volY, fillW, volH, volH/2);
            SolidBrush fillBrush{ textMain };
            g.FillPath(&fillBrush, &volFill);
        }

        // Thumb
        if (g_ExpHoverVol || g_VolDragging) {
            int thumbX = volX + (int)(volW * volPct);
            int thumbR = 6;
            SolidBrush thumbBrush{ textMain };
            g.FillEllipse(&thumbBrush,
                          thumbX - thumbR, volY + volH/2 - thumbR,
                          thumbR*2, thumbR*2);
        }
    }
    if (appIcon) {
        delete appIcon;
    }
}


struct PopupLayout {
    int width;
    int height;
    int pad;
    int iconSize;
    int artSize;
    int artX;
    int artY;
    int titleY;
    int artistY;
    int pillH;
    int lrcBtnY;
    int seekY;
    int controlRowY;
    int volRowY;
};

PopupLayout GetPopupLayout(HWND hwnd) {
    RECT rc; GetClientRect(hwnd, &rc);
    PopupLayout lay;
    lay.width = rc.right;
    lay.height = rc.bottom;
    
    lay.pad = 16;
    lay.iconSize = g_Settings.popupIconSize;
    
    int rowY = lay.pad + lay.iconSize + 10;
    int maxArtSize = lay.height - rowY - 180;
    if (maxArtSize < 80) maxArtSize = 80;
    lay.artSize = min(lay.width - lay.pad * 2, maxArtSize);
    lay.artX = (lay.width - lay.artSize) / 2;
    lay.artY = rowY;
    
    rowY = lay.artY + lay.artSize + 12;
    lay.titleY = rowY;
    
    rowY = lay.titleY + 22;
    lay.artistY = rowY;
    
    wstring artistStr;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        artistStr = g_MediaState.artist.empty() ? L"Unknown Artist" : g_MediaState.artist;
    }
    
    float fsArt = (float)g_Settings.popupFontSize - 1.0f;
    FontFamily ffArt(FONT_NAME, nullptr);
    Font artFont(&ffArt, fsArt, FontStyleRegular, UnitPixel);
    
    HDC hdc = GetDC(hwnd);
    Graphics gMeasure(hdc);
    
    RectF layoutRectArt(0, 0, 2000, 100), boundRectArt;
    gMeasure.MeasureString(artistStr.c_str(), -1, &artFont, layoutRectArt, &boundRectArt);
    int textHeight = (int)boundRectArt.Height;
    if (textHeight <= 0) textHeight = (int)fsArt;
    lay.pillH = textHeight + 6;
    ReleaseDC(hwnd, hdc);
    
    rowY = lay.artistY + lay.pillH;
    
    if (g_Settings.fetchLyrics) {
        lay.lrcBtnY = rowY + 2;
        rowY += 24;
    } else {
        lay.lrcBtnY = 0;
    }
    
    lay.seekY = rowY + 18;
    lay.controlRowY = lay.seekY + 28 + 16;
    lay.volRowY = lay.seekY + 28 + 44 + 4;
    
    return lay;
}


bool IsCursorOverInteractivePopupElement(HWND hwnd, int mx, int my) {
    PopupLayout lay = GetPopupLayout(hwnd);
    
    int sessionCount = 0;
    try {
        if (g_SessionManager) sessionCount = (int)g_SessionManager.GetSessions().Size();
    } catch (...) {}
    
    if (sessionCount > 1 && my >= lay.pad - 4 && my <= lay.pad + 20) {
        if (mx >= lay.pad - 4 && mx <= lay.pad + 20) return true;
        if (mx >= lay.width - lay.pad - 24 && mx <= lay.width - lay.pad + 4) return true;
    }
    
    int midStartX = (sessionCount > 1) ? lay.pad + 24 + 6 : lay.pad;
    int midEndX   = (sessionCount > 1) ? lay.width - lay.pad - 24 - 6 : lay.width - lay.pad;
    int midWidth  = midEndX - midStartX;
    int totalHeaderWidth = (lay.iconSize + 6) + g_AppTextWidth;
    int headerX = midStartX + (midWidth - totalHeaderWidth) / 2;
    if (headerX < midStartX) headerX = midStartX;
    if (mx >= headerX - 6 && mx <= headerX + totalHeaderWidth + 6 &&
        my >= lay.pad - 4 && my <= lay.pad + lay.iconSize + 4) {
        return true;
    }
    
    float btnScale = 1.2f;
    float btnCircR = 14.0f * btnScale;
    float btnGap   = 36.0f * btnScale;
    float centerX  = lay.width / 2.0f;
    float shX  = centerX - btnGap * 2.0f;
    float pX   = centerX - btnGap;
    float plX  = centerX;
    float nX   = centerX + btnGap;
    float rpX  = centerX + btnGap * 2.0f;
    
    float dy = (float)my - (float)lay.controlRowY;
    if (fabsf(dy) <= btnCircR) {
        if      (fabsf((float)mx - pX)   <= btnCircR) return true;
        else if (fabsf((float)mx - plX)  <= btnCircR) return true;
        else if (fabsf((float)mx - nX)   <= btnCircR) return true;
        else if (fabsf((float)mx - shX)  <= btnCircR) return true;
        else if (fabsf((float)mx - rpX)  <= btnCircR) return true;
    }
    
    if (g_Settings.fetchLyrics) {
        int btnW = 90;
        int btnH = 20;
        int totalW = btnW + 8 + btnW;
        int startX = (lay.width - totalW) / 2;
        int toggleX = startX;
        int lrcBtnX = startX + btnW + 8;
        if (my >= lay.lrcBtnY && my <= lay.lrcBtnY + btnH) {
            if (mx >= toggleX && mx <= toggleX + btnW) return true;
            if (mx >= lrcBtnX && mx <= lrcBtnX + btnW) return true;
        }
    }
    
    if (mx >= lay.pad && mx <= lay.width - lay.pad && my >= lay.seekY - 6 && my <= lay.seekY + 10 && g_Timeline.canSeek) {
        return true;
    }
    int volX = lay.pad + 20, volW = lay.width - lay.pad*2 - 40;
    if (mx >= volX && mx <= volX + volW && my >= lay.volRowY - 6 && my <= lay.volRowY + 10) {
        return true;
    }
    
    bool showLyrics = false, hasLyrics = false;
    vector<LyricLine> lyricsLines;
    {
        lock_guard<mutex> lguard(g_Lyrics.lock);
        showLyrics = g_Lyrics.showLyrics;
        hasLyrics = g_Lyrics.hasLyrics;
        lyricsLines = g_Lyrics.lines;
    }
    if (g_Settings.fetchLyrics && showLyrics && hasLyrics) {
        if (mx >= lay.artX + lay.artSize - 16 && mx <= lay.artX + lay.artSize && my >= lay.artY && my <= lay.artY + lay.artSize) {
            return true;
        }
        if (!lyricsLines.empty() && mx >= lay.artX && mx <= lay.artX + lay.artSize - 16 && my >= lay.artY && my <= lay.artY + lay.artSize) {
            double livePos = GetLivePosition();
            int activeIndex = -1;
            for (size_t idx = 0; idx < lyricsLines.size(); idx++) {
                if (livePos >= lyricsLines[idx].timeSec) {
                    activeIndex = (int)idx;
                } else {
                    break;
                }
            }
            float activeShift = 0.0f;
            if (activeIndex >= 0 && activeIndex < (int)lyricsLines.size()) {
                HDC hdc = GetDC(hwnd);
                Graphics gMeasure(hdc);
                float fs = (float)g_Settings.lyricsFontSize;
                FontFamily ff(FONT_NAME, nullptr);
                Font lyricBoldFont(&ff, fs + 2.0f, FontStyleBold, UnitPixel);
                StringFormat sfWrap;
                sfWrap.SetAlignment(StringAlignmentCenter);
                sfWrap.SetLineAlignment(StringAlignmentCenter);
                float maxW = (float)lay.artSize - 6.0f;
                RectF layoutRect(0, 0, maxW, 200.0f);
                RectF boundRect;
                gMeasure.MeasureString(lyricsLines[activeIndex].text.c_str(), -1, &lyricBoldFont, layoutRect, &sfWrap, &boundRect);
                if (boundRect.Height > 22.0f) {
                    activeShift = (boundRect.Height - 16.0f) / 2.0f + 14.0f;
                }
                ReleaseDC(hwnd, hdc);
            }
            int boxCenterY = lay.artY + lay.artSize / 2;
            float lineHeight = 38.0f;
            for (int i = 0; i < (int)lyricsLines.size(); i++) {
                float lineY = (float)boxCenterY + (i * lineHeight) - g_LyricsScrollOffset;
                if (i < activeIndex) lineY -= activeShift;
                else if (i > activeIndex) lineY += activeShift;
                if (lineY >= lay.artY && lineY <= lay.artY + lay.artSize) {
                    if (my >= lineY - lineHeight / 2.0f && my <= lineY + lineHeight / 2.0f) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}


LRESULT CALLBACK ExpandedWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            ApplyAcrylicBlur(hwnd); // uses default theme-aware glass backdrop path
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                if (IsCursorOverInteractivePopupElement(hwnd, pt.x, pt.y)) {
                    SetCursor(LoadCursor(NULL, IDC_HAND));
                    return TRUE;
                }
            }
            break;
        }

        case WM_KILLFOCUS:
            if ((HWND)wParam != g_hMediaWindow) {
                g_Anim.opening   = false;
                g_Anim.isAnimating = true;
                g_Anim.totalFrames = (ANIM_CLOSE_MS * ANIM_FPS) / 1000;
                if (g_Anim.totalFrames < 1) g_Anim.totalFrames = 1;
                g_Anim.currentFrame = 0;
                SetTimer(hwnd, IDT_ANIM_POPUP, ANIM_INTERVAL, NULL);
            }
            return 0;

        case WM_TIMER:
            if (wParam == IDT_ANIM_POPUP) {
                g_Anim.currentFrame++;
                float rawT = (float)g_Anim.currentFrame / (float)g_Anim.totalFrames;
                rawT = min(rawT, 1.0f);
                float easedT = EaseOutCubic(rawT);

                if (g_Anim.opening) {
                    g_Anim.progress = easedT;
                } else {
                    g_Anim.progress = 1.0f - easedT;
                }

                // Alpha is set via UpdateLayeredWindow inside WM_PAINT

                RECT rc; GetWindowRect(hwnd, &rc);
                HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                if (hTaskbar) {
                    RECT tb; GetWindowRect(hTaskbar, &tb);
                    int targetX = tb.left + g_Settings.offsetX;
                    int targetY = tb.top - g_Settings.popupHeight - POPUP_GAP;
                    int slideOffset = (int)((1.0f - g_Anim.progress) * 12.0f);
                    SetWindowPos(hwnd, HWND_TOPMOST,
                                 targetX, targetY + slideOffset,
                                 g_Settings.popupWidth, g_Settings.popupHeight,
                                 SWP_NOACTIVATE);
                }

                if (g_hMediaWindow) {
                    BYTE mediaAlpha = (BYTE)((1.0f - g_Anim.progress) * 255.0f);
                    g_MediaWindowAlpha = mediaAlpha;
                    InvalidateRect(g_hMediaWindow, nullptr, FALSE);
                    
                    if (g_Anim.progress >= 1.0f && !g_Anim.isAnimating) {
                        ShowWindow(g_hMediaWindow, SW_HIDE);
                    } else if (g_Anim.progress < 1.0f) {
                        if (!IsWindowVisible(g_hMediaWindow) && !g_IsHiddenByIdle && !g_IsHiddenByNoMedia) {
                            ShowWindow(g_hMediaWindow, SW_SHOWNOACTIVATE);
                        }
                    }
                    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
                }

                InvalidateRect(hwnd, NULL, FALSE);

                if (g_Anim.currentFrame >= g_Anim.totalFrames) {
                    KillTimer(hwnd, IDT_ANIM_POPUP);
                    g_Anim.isAnimating = false;
                    if (!g_Anim.opening) {
                        ShowWindow(hwnd, SW_HIDE);
                        g_Anim.isOpen = false;
                        g_Anim.progress = 0.0f;
                    } else {
                        g_Anim.isOpen = true;
                        g_Anim.progress = 1.0f;
                    }
                    if (g_hMediaWindow) {
                        if (g_Anim.isOpen) {
                            ShowWindow(g_hMediaWindow, SW_HIDE);
                        } else {
                            g_MediaWindowAlpha = 255;
                            InvalidateRect(g_hMediaWindow, nullptr, FALSE);
                            if (!g_IsHiddenByIdle && !g_IsHiddenByNoMedia) {
                                  ShowWindow(g_hMediaWindow, SW_SHOWNOACTIVATE);
                            }
                        }
                        PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
                    }
                }
            }
            else if (wParam == IDT_ANIMATION) {
                UpdateVisualizerFrame();
                
                if (g_IsAppScrolling) {
                    if (g_AppScrollWait > 0) { g_AppScrollWait--; }
                    else {
                        g_AppScrollOffset++;
                        if (g_AppScrollOffset > g_AppTextWidth + 40) {
                            g_AppScrollOffset = 0;
                            g_AppScrollWait = 60;
                        }
                    }
                }
                
                if (g_IsTitleScrolling) {
                    if (g_TitleScrollWait > 0) { g_TitleScrollWait--; }
                    else {
                        g_TitleScrollOffset++;
                        if (g_TitleScrollOffset > g_TitleTextWidth + 40) {
                            g_TitleScrollOffset = 0;
                            g_TitleScrollWait = 60;
                        }
                    }
                }
                
                if (g_IsArtistScrolling) {
                    if (g_ArtistScrollWait > 0) { g_ArtistScrollWait--; }
                    else {
                        g_ArtistScrollOffset++;
                        if (g_ArtistScrollOffset > g_ArtistTextWidth + 40) {
                            g_ArtistScrollOffset = 0;
                            g_ArtistScrollWait = 60;
                        }
                    }
                }
                
                // Handle manual vs automatic lyrics scrolling
                if (g_LyricsIsUserScrolling) {
                    if (GetTickCount64() - g_LyricsLastUserScrollTime > 3000 && !g_LyricsScrollDragging) {
                        g_LyricsIsUserScrolling = false;
                    } else {
                        g_LyricsTargetScroll = g_LyricsUserScrollOffset;
                    }
                }
                
                // Smooth slide transition for lyrics scrolling (increased speed coefficient to 0.25f)
                if (fabs(g_LyricsScrollOffset - g_LyricsTargetScroll) > 0.1f) {
                    g_LyricsScrollOffset += (g_LyricsTargetScroll - g_LyricsScrollOffset) * 0.25f;
                } else {
                    g_LyricsScrollOffset = g_LyricsTargetScroll;
                }
                
                InvalidateRect(hwnd, NULL, FALSE);
                
                bool isPlaying = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    isPlaying = g_MediaState.isPlaying;
                }
                bool lyricsScrolling = (fabs(g_LyricsScrollOffset - g_LyricsTargetScroll) > 0.1f);
                if (!isPlaying && !IsVisualizerActive() && fabs(g_ArtSlideOffset) < 0.1f && fabs(g_DragOffsetX) < 0.1f &&
                    !g_IsAppScrolling && !g_IsTitleScrolling && !g_IsArtistScrolling && !lyricsScrolling) {
                    KillTimer(hwnd, IDT_ANIMATION);
                }
            }
            return 0;

        case WM_MOUSEMOVE: {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);

            int sessionCount = 0;
            try {
                if (g_SessionManager) sessionCount = (int)g_SessionManager.GetSessions().Size();
            } catch (...) {}

            float btnScale = 1.2f;
            float btnCircR = 14.0f * btnScale;
            float btnGap   = 36.0f * btnScale;
            float centerX  = g_Settings.popupWidth / 2.0f;

            PopupLayout lay = GetPopupLayout(hwnd);
            int pad = lay.pad;
            int iconSize = lay.iconSize;
            int artSize = lay.artSize;
            int artX = lay.artX;
            int artY = lay.artY;
            int rowY = lay.artY;
            int seekY = lay.seekY;
            int controlRowY = lay.controlRowY;
            int volRowY = lay.volRowY;
            int lrcBtnY = lay.lrcBtnY;

            float shX  = centerX - btnGap * 2.0f;
            float pX   = centerX - btnGap;
            float plX  = centerX;
            float nX   = centerX + btnGap;
            float rpX  = centerX + btnGap * 2.0f;

            // Session switchers hover hit test (opposite sides)
            int hoverSwitch = 0;
            if (sessionCount > 1 && my >= pad - 4 && my <= pad + 20) {
                if (mx >= pad - 4 && mx <= pad + 20) hoverSwitch = 6;
                else if (mx >= g_Settings.popupWidth - pad - 24 && mx <= g_Settings.popupWidth - pad + 4) hoverSwitch = 7;
            }

            int newBtn = 0;
            float dy = (float)my - (float)controlRowY;
            if (fabsf(dy) <= btnCircR) {
                if      (fabsf((float)mx - pX)   <= btnCircR) newBtn = 1;
                else if (fabsf((float)mx - plX)  <= btnCircR) newBtn = 2;
                else if (fabsf((float)mx - nX)   <= btnCircR) newBtn = 3;
                else if (fabsf((float)mx - shX)  <= btnCircR) newBtn = 4;
                else if (fabsf((float)mx - rpX)  <= btnCircR) newBtn = 5;
            }
            if (hoverSwitch > 0) newBtn = hoverSwitch;

            // Hover hit test for Lyrics Button (ID 8) & Stream to Taskbar (ID 10)
            if (g_Settings.fetchLyrics) {
                int btnGap = 8;
                int btnW = 90;
                int btnH = 20;
                int totalW = btnW + btnGap + btnW;
                int startX = (g_Settings.popupWidth - totalW) / 2;
                int toggleX = startX;
                int lrcBtnX = startX + btnW + btnGap;
                
                if (my >= lrcBtnY && my <= lrcBtnY + btnH) {
                    if (mx >= toggleX && mx <= toggleX + btnW) {
                        newBtn = 10;
                    } else if (mx >= lrcBtnX && mx <= lrcBtnX + btnW) {
                        newBtn = 8;
                    }
                }
            }

            // Hover hit test for Redirect App Header Button (ID 9)
            int midStartX = (sessionCount > 1) ? pad + 24 + 6 : pad;
            int midEndX   = (sessionCount > 1) ? g_Settings.popupWidth - pad - 24 - 6 : g_Settings.popupWidth - pad;
            int midWidth  = midEndX - midStartX;
            int totalHeaderWidth = (iconSize + 6) + g_AppTextWidth;
            int headerX = midStartX + (midWidth - totalHeaderWidth) / 2;
            if (headerX < midStartX) headerX = midStartX;
            if (mx >= headerX - 6 && mx <= headerX + totalHeaderWidth + 6 &&
                my >= pad - 4 && my <= pad + iconSize + 4) {
                newBtn = 9;
            }

            bool onSeek = (mx >= pad && mx <= g_Settings.popupWidth - pad &&
                           my >= seekY - 6 && my <= seekY + 10 && g_Timeline.canSeek);

            int volX = pad + 20, volW = g_Settings.popupWidth - pad*2 - 40;
            bool onVol = (mx >= volX && mx <= volX + volW &&
                          my >= volRowY - 6 && my <= volRowY + 10);

            if (g_IsDraggingSession || g_SeekDragging || g_VolDragging || g_LyricsScrollDragging) {
                newBtn = 0;
                onSeek = g_SeekDragging;
                onVol = g_VolDragging;
            }

            if (newBtn != g_ExpHoverBtn || onSeek != g_ExpHoverSeek || onVol != g_ExpHoverVol) {
                g_ExpHoverBtn  = newBtn;
                g_ExpHoverSeek = onSeek;
                g_ExpHoverVol  = onVol;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            // Drag operations
            if (g_LyricsScrollDragging) {
                int L = 0;
                {
                    lock_guard<mutex> lguard(g_Lyrics.lock);
                    L = (int)g_Lyrics.lines.size();
                }
                float trackH = (float)artSize - 40.0f;
                float relativeY = (float)(my - rowY - 20.0f);
                float pct = relativeY / trackH;
                pct = max(0.0f, min(1.0f, pct));
                
                float maxScroll = (L - 1) * 38.0f;
                g_LyricsUserScrollOffset = pct * maxScroll;
                g_LyricsIsUserScrolling = true;
                g_LyricsLastUserScrollTime = GetTickCount64();
                
                InvalidateRect(hwnd, NULL, FALSE);
            }
            if (g_SeekDragging) {
                int seekTrackX = pad;
                int seekTrackW = g_Settings.popupWidth - pad*2;
                float pct = (float)(mx - seekTrackX) / (float)seekTrackW;
                pct = max(0.0f, min(1.0f, pct));
                g_Timeline.positionSec = pct * g_Timeline.durationSec;
                g_TimelineLastUpdated = GetTickCount64();
                g_LastSeekTime = GetTickCount64();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            if (g_VolDragging) {
                float pct = (float)(mx - volX) / (float)volW;
                SetVolume(pct);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            if (g_IsDraggingSession) {
                g_DragOffsetX = (float)(mx - g_DragStartX);
                InvalidateRect(hwnd, NULL, FALSE);
            }

            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_ExpHoverBtn  = 0;
            g_ExpHoverSeek = false;
            g_ExpHoverVol  = false;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);

            int sessionCount = 0;
            try {
                if (g_SessionManager) sessionCount = (int)g_SessionManager.GetSessions().Size();
            } catch (...) {}

            PopupLayout lay = GetPopupLayout(hwnd);
            int pad = lay.pad;
            int iconSize = lay.iconSize;
            int artSize = lay.artSize;
            int artX = lay.artX;
            int artY = lay.artY;
            int rowY = lay.artY;
            int seekY = lay.seekY;
            int controlRowY = lay.controlRowY;
            int volRowY = lay.volRowY;
            int lrcBtnY = lay.lrcBtnY;
            int volX = pad + 20, volW = g_Settings.popupWidth - pad*2 - 40;

            float btnScale = 1.2f;
            float btnCircR = 14.0f * btnScale;

            bool onSeek = (mx >= pad && mx <= g_Settings.popupWidth - pad && my >= seekY - 6 && my <= seekY + 10 && g_Timeline.canSeek);
            bool onVol = (mx >= volX && mx <= volX + volW && my >= volRowY - 6 && my <= volRowY + 10);
            bool onControls = (my >= controlRowY - btnCircR && my <= controlRowY + btnCircR);
            
            // Session switcher arrows click check
            bool onSwitchers = false;
            if (sessionCount > 1 && my >= pad - 4 && my <= pad + 20) {
                if ((mx >= pad - 4 && mx <= pad + 20) || 
                    (mx >= g_Settings.popupWidth - pad - 24 && mx <= g_Settings.popupWidth - pad + 4)) {
                    onSwitchers = true;
                }
            }

            // App header redirect check
            bool onHeader = false;
            {
                int midStartX = (sessionCount > 1) ? pad + 24 + 6 : pad;
                int midEndX   = (sessionCount > 1) ? g_Settings.popupWidth - pad - 24 - 6 : g_Settings.popupWidth - pad;
                int midWidth  = midEndX - midStartX;
                int totalHeaderWidth = (iconSize + 6) + g_AppTextWidth;
                int headerX = midStartX + (midWidth - totalHeaderWidth) / 2;
                if (headerX < midStartX) headerX = midStartX;
                if (mx >= headerX - 6 && mx <= headerX + totalHeaderWidth + 6 &&
                    my >= pad - 4 && my <= pad + iconSize + 4) {
                    onHeader = true;
                }
            }

            int btnGap = 8;
            int btnW = 90;
            int btnH = 20;
            int totalW = btnW + btnGap + btnW;
            int startX = (g_Settings.popupWidth - totalW) / 2;
            int toggleX = startX;
            int lrcBtnX = startX + btnW + btnGap;
            bool onToggleBtn = false;
            bool onLrcBtn = false;
            if (g_Settings.fetchLyrics && my >= lrcBtnY && my <= lrcBtnY + btnH) {
                if (mx >= toggleX && mx <= toggleX + btnW) onToggleBtn = true;
                else if (mx >= lrcBtnX && mx <= lrcBtnX + btnW) onLrcBtn = true;
            }

            bool onLyricsBox = false;
            bool showLyrics = false;
            if (g_Settings.fetchLyrics) {
                {
                    lock_guard<mutex> lguard(g_Lyrics.lock);
                    showLyrics = g_Lyrics.showLyrics;
                }
                if (showLyrics && mx >= artX && mx <= artX + artSize && my >= rowY && my <= rowY + artSize) {
                    onLyricsBox = true;
                }
            }

            bool hasLyrics = false;
            {
                lock_guard<mutex> lguard(g_Lyrics.lock);
                hasLyrics = g_Lyrics.hasLyrics;
            }

            bool onScrollbar = false;
            if (g_Settings.fetchLyrics && onLyricsBox && hasLyrics) {
                if (mx >= artX + artSize - 16 && mx <= artX + artSize &&
                    my >= rowY && my <= rowY + artSize) {
                    onScrollbar = true;
                }
            }

            if (g_Settings.fetchLyrics && onLrcBtn && hasLyrics) {
                {
                    lock_guard<mutex> lguard(g_Lyrics.lock);
                    g_Lyrics.showLyrics = !g_Lyrics.showLyrics;
                }
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            if (onScrollbar) {
                g_LyricsScrollDragging = true;
                SetCapture(hwnd);
                
                int L = 0;
                {
                    lock_guard<mutex> lguard(g_Lyrics.lock);
                    L = (int)g_Lyrics.lines.size();
                }
                
                float trackH = (float)artSize - 40.0f;
                float relativeY = (float)(my - rowY - 20.0f);
                float pct = relativeY / trackH;
                pct = max(0.0f, min(1.0f, pct));
                
                float maxScroll = (L - 1) * 38.0f;
                g_LyricsUserScrollOffset = pct * maxScroll;
                g_LyricsIsUserScrolling = true;
                g_LyricsLastUserScrollTime = GetTickCount64();
                
                SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            if (onSeek) {
                g_SeekDragging = true;
                SetCapture(hwnd);
                int seekTrackX = pad;
                int seekTrackW = g_Settings.popupWidth - pad*2;
                float pct = (float)(mx - seekTrackX) / (float)seekTrackW;
                pct = max(0.0f, min(1.0f, pct));
                g_Timeline.positionSec = pct * g_Timeline.durationSec;
                g_TimelineLastUpdated = GetTickCount64();
                g_LastSeekTime = GetTickCount64();
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (onVol) {
                g_VolDragging = true;
                SetCapture(hwnd);
                float pct = (float)(mx - volX) / (float)volW;
                SetVolume(pct);
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (!onControls && !onSwitchers && !onHeader && !onToggleBtn && !onLyricsBox) {
                // Drag Swipe operation started
                g_IsDraggingSession = true;
                g_DragStartX = mx;
                g_DragOffsetX = 0.0f;
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);

            if (g_LyricsScrollDragging) {
                g_LyricsScrollDragging = false;
                ReleaseCapture();
            }
            if (g_SeekDragging) {
                g_SeekDragging = false;
                ReleaseCapture();
                g_LastSeekTime = GetTickCount64();
                g_TimelineLastUpdated = GetTickCount64();
                g_PendingSeekPosition = g_Timeline.positionSec;
                g_SeekPending = true;
                SendSeekCommand(g_Timeline.positionSec);
            }
            if (g_VolDragging) {
                g_VolDragging = false;
                ReleaseCapture();
            }
            if (g_IsDraggingSession) {
                g_IsDraggingSession = false;
                ReleaseCapture();
                if (g_DragOffsetX > 60.0f) {
                    g_DragOffsetX = 0.0f;
                    SwitchSession(-1); // Swipe Right -> Prev App
                } else if (g_DragOffsetX < -60.0f) {
                    g_DragOffsetX = 0.0f;
                    SwitchSession(1);  // Swipe Left -> Next App
                } else {
                    g_DragOffsetX = 0.0f;
                    g_ArtSlideOffset = 0.0f;
                    g_ArtSlideTarget = 0.0f;
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }

            PopupLayout lay = GetPopupLayout(hwnd);
            int pad = lay.pad;
            int iconSize = lay.iconSize;
            int artSize = lay.artSize;
            int artX = lay.artX;
            int artY = lay.artY;
            int rowY = lay.artY;

            bool showLyrics = false, hasLyrics = false;
            vector<LyricLine> lyricsLines;
            {
                lock_guard<mutex> lguard(g_Lyrics.lock);
                showLyrics = g_Lyrics.showLyrics;
                hasLyrics = g_Lyrics.hasLyrics;
                lyricsLines = g_Lyrics.lines;
            }

            if (g_Settings.fetchLyrics && showLyrics && hasLyrics && !lyricsLines.empty() && g_ExpHoverBtn == 0 &&
                !g_SeekDragging && !g_VolDragging && !g_IsDraggingSession &&
                mx >= artX && mx <= artX + artSize && my >= artY && my <= artY + artSize) {
                
                double livePos = GetLivePosition();
                int activeIndex = -1;
                for (size_t idx = 0; idx < lyricsLines.size(); idx++) {
                    if (livePos >= lyricsLines[idx].timeSec) {
                        activeIndex = (int)idx;
                    } else {
                        break;
                    }
                }
                
                float activeShift = 0.0f;
                if (activeIndex >= 0 && activeIndex < (int)lyricsLines.size()) {
                    HDC hdc = GetDC(hwnd);
                    Graphics g(hdc);
                    float fs = (float)g_Settings.lyricsFontSize;
                    FontFamily ff(FONT_NAME, nullptr);
                    Font lyricBoldFont(&ff, fs + 2.0f, FontStyleBold, UnitPixel);
                    StringFormat sfWrap;
                    sfWrap.SetAlignment(StringAlignmentCenter);
                    sfWrap.SetLineAlignment(StringAlignmentCenter);
                    float maxW = (float)artSize - 6.0f;
                    RectF layoutRect(0, 0, maxW, 200.0f);
                    RectF boundRect;
                    g.MeasureString(lyricsLines[activeIndex].text.c_str(), -1, &lyricBoldFont, layoutRect, &sfWrap, &boundRect);
                    if (boundRect.Height > 22.0f) {
                        activeShift = (boundRect.Height - 16.0f) / 2.0f + 14.0f;
                    }
                    ReleaseDC(hwnd, hdc);
                }
                
                int boxCenterY = artY + artSize / 2;
                float lineHeight = 38.0f;
                int clickedIdx = -1;
                
                for (int i = 0; i < (int)lyricsLines.size(); i++) {
                    float lineY = (float)boxCenterY + (i * lineHeight) - g_LyricsScrollOffset;
                    if (i < activeIndex) {
                        lineY -= activeShift;
                    } else if (i > activeIndex) {
                        lineY += activeShift;
                    }
                    
                    if (my >= lineY - lineHeight / 2.0f && my <= lineY + lineHeight / 2.0f) {
                        clickedIdx = i;
                        break;
                    }
                }
                
                if (clickedIdx != -1) {
                    g_Timeline.positionSec = lyricsLines[clickedIdx].timeSec;
                    g_TimelineLastUpdated = GetTickCount64();
                    g_LastSeekTime = GetTickCount64();
                    g_PendingSeekPosition = g_Timeline.positionSec;
                    g_SeekPending = true;
                    SendSeekCommand(g_Timeline.positionSec);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }

            if (g_ExpHoverBtn > 0) {
                bool canPrev = true, canPlay = true, canNext = true, canShuffle = true, canRepeat = true;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    canPrev = g_MediaState.canGoPrev;
                    canPlay = g_MediaState.canPlayPause;
                    canNext = g_MediaState.canGoNext;
                    canShuffle = g_MediaState.canShuffle;
                    canRepeat = g_MediaState.canRepeat;
                }

                bool execute = false;
                if (g_ExpHoverBtn == 1) execute = canPrev;
                else if (g_ExpHoverBtn == 2) execute = canPlay;
                else if (g_ExpHoverBtn == 3) execute = canNext;
                else if (g_ExpHoverBtn == 4) execute = canShuffle;
                else if (g_ExpHoverBtn == 5) execute = canRepeat;
                else execute = true;

                if (execute) {
                    if (g_ExpHoverBtn == 10) {
                        lock_guard<mutex> lguard(g_Lyrics.lock);
                        g_Lyrics.streamLyrics = !g_Lyrics.streamLyrics;
                        InvalidateRect(g_hMediaWindow, NULL, FALSE);
                    } else if (g_ExpHoverBtn == 9) {
                        RedirectToMediaSource();
                    } else if (g_ExpHoverBtn == 6) {
                        SwitchSession(-1);
                    } else if (g_ExpHoverBtn == 7) {
                        SwitchSession(1);
                    } else if (g_ExpHoverBtn == 4) {
                        ToggleShuffle();
                        lock_guard<mutex> guard(g_MediaState.lock);
                        g_MediaState.shuffle = !g_MediaState.shuffle;
                    } else if (g_ExpHoverBtn == 5) {
                        ToggleRepeat();
                        lock_guard<mutex> guard(g_MediaState.lock);
                        if (g_MediaState.repeatMode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None)
                            g_MediaState.repeatMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List;
                        else if (g_MediaState.repeatMode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List)
                            g_MediaState.repeatMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track;
                        else
                            g_MediaState.repeatMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                    } else {
                        if (g_ExpHoverBtn == 2) {
                            lock_guard<mutex> guard(g_MediaState.lock);
                            g_MediaState.isPlaying = !g_MediaState.isPlaying;
                            g_OptimisticTime = GetTickCount64();
                            g_OptimisticPlaying = g_MediaState.isPlaying;
                        }
                        SendMediaCommand(g_ExpHoverBtn);
                    }
                    PostMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                g_Anim.opening     = false;
                g_Anim.isAnimating = true;
                g_Anim.totalFrames = (ANIM_CLOSE_MS * ANIM_FPS) / 1000;
                if (g_Anim.totalFrames < 1) g_Anim.totalFrames = 1;
                g_Anim.currentFrame = 0;
                SetTimer(hwnd, IDT_ANIM_POPUP, ANIM_INTERVAL, NULL);
            }
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            
            HDC memDC = CreateCompatibleDC(hdc);
            
            // Create a 32-bit ARGB DIB Section to preserve GDI+ per-pixel transparency
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = rc.right;
            bmi.bmiHeader.biHeight = -rc.bottom; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* pBits = nullptr;
            HBITMAP memBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
            HBITMAP old = (HBITMAP)SelectObject(memDC, memBmp);
            
            DrawExpandedPanel(memDC, rc.right, rc.bottom);
            
            bool isPlaying = false;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                isPlaying = g_MediaState.isPlaying;
            }
            bool showLyrics = false, hasLyrics = false;
            {
                lock_guard<mutex> lguard(g_Lyrics.lock);
                showLyrics = g_Lyrics.showLyrics;
                hasLyrics = g_Lyrics.hasLyrics;
            }
            bool lyricsScrolling = (fabs(g_LyricsScrollOffset - g_LyricsTargetScroll) > 0.1f);
            if (isPlaying || IsVisualizerActive() || fabs(g_ArtSlideOffset) > 0.5f || fabs(g_DragOffsetX) > 0.5f || (showLyrics && hasLyrics) || lyricsScrolling) {
                SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
            }
            
            // Use UpdateLayeredWindow to paint with per-pixel alpha transparency
            RECT winRect;
            GetWindowRect(hwnd, &winRect);
            POINT ptDst = { winRect.left, winRect.top };
            SIZE sizeWnd = { rc.right, rc.bottom };
            POINT ptSrc = { 0, 0 };
            BYTE alpha = (BYTE)(g_Anim.progress * 255.0f);
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);
            
            SelectObject(memDC, old);
            DeleteObject(memBmp);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            POINT pt = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
            ScreenToClient(hwnd, &pt);
            int mx = pt.x;
            int my = pt.y;

            bool showLyrics = false, hasLyrics = false;
            int L = 0;
            {
                lock_guard<mutex> lguard(g_Lyrics.lock);
                showLyrics = g_Lyrics.showLyrics;
                hasLyrics = g_Lyrics.hasLyrics;
                L = (int)g_Lyrics.lines.size();
            }

            int pad = 16;
            int iconSize = g_Settings.popupIconSize;
            int rowY = pad + iconSize + 10;
            int maxArtSize = g_Settings.popupHeight - rowY - 180;
            if (maxArtSize < 80) maxArtSize = 80;
            int artSize = min(g_Settings.popupWidth - pad * 2, maxArtSize);
            int artX = (g_Settings.popupWidth - artSize) / 2;
            int artY = rowY;

            if (g_Settings.fetchLyrics && showLyrics && hasLyrics && L > 0 &&
                mx >= artX && mx <= artX + artSize && my >= artY && my <= artY + artSize) {
                
                float scrollAmount = (float)(-zDelta);
                float maxScroll = (L - 1) * 38.0f;
                if (maxScroll < 0) maxScroll = 0;
                
                g_LyricsUserScrollOffset = g_LyricsScrollOffset + scrollAmount;
                g_LyricsUserScrollOffset = max(0.0f, min(maxScroll, g_LyricsUserScrollOffset));
                
                g_LyricsIsUserScrolling = true;
                g_LyricsLastUserScrollTime = GetTickCount64();
                
                SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
                InvalidateRect(hwnd, NULL, FALSE);
            } else {
                float delta = (zDelta > 0) ? 0.05f : -0.05f;
                SetVolume(g_VolumeLevel + delta);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ToggleExpandedPopup(HWND hCompact) {
    if (!g_hExpandedWindow) return;

    if (g_Anim.isAnimating && !g_Anim.opening) {
        g_Anim.opening = true;
        return;
    }
    if (g_Anim.isOpen || (g_Anim.isAnimating && g_Anim.opening)) {
        g_Anim.opening     = false;
        g_Anim.isAnimating = true;
        g_Anim.totalFrames = (ANIM_CLOSE_MS * ANIM_FPS) / 1000;
        if (g_Anim.totalFrames < 1) g_Anim.totalFrames = 1;
        g_Anim.currentFrame = 0;
        SetTimer(g_hExpandedWindow, IDT_ANIM_POPUP, ANIM_INTERVAL, NULL);
        return;
    }

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return;
    RECT tb; GetWindowRect(hTaskbar, &tb);
    int targetX = tb.left + g_Settings.offsetX;
    int targetY = tb.top - g_Settings.popupHeight - POPUP_GAP;
    int slideOffset = 12;

    SetWindowPos(g_hExpandedWindow, HWND_TOPMOST,
                 targetX, targetY + slideOffset,
                 g_Settings.popupWidth, g_Settings.popupHeight,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);

    // Opacity is handled by UpdateLayeredWindow on next paint
    ShowWindow(g_hExpandedWindow, SW_SHOWNOACTIVATE);
    SetForegroundWindow(g_hExpandedWindow);
    SetFocus(g_hExpandedWindow);

    g_Anim.opening     = true;
    g_Anim.isAnimating = true;
    g_Anim.isOpen      = false;
    g_Anim.progress    = 0.0f;
    g_Anim.totalFrames = (ANIM_OPEN_MS * ANIM_FPS) / 1000;
    if (g_Anim.totalFrames < 1) g_Anim.totalFrames = 1;
    g_Anim.currentFrame = 0;
    SetTimer(g_hExpandedWindow, IDT_ANIM_POPUP, ANIM_INTERVAL, NULL);
}

// ============================================================
// Taskbar Event Hook
// ============================================================
bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd) return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

void CALLBACK TaskbarEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hMediaWindow) return;
    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

void RegisterTaskbarHook(HWND hwnd) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        DWORD pid = 0, tid = GetWindowThreadProcessId(hTaskbar, &pid);
        if (tid != 0) {
            g_TaskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, TaskbarEventProc, pid, tid,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        }
    }
    PostMessage(hwnd, WM_APP + 10, 0, 0);
}
void CheckVisibility(HWND hwnd) {
    bool shouldHide = false;
    
    // 1. Fullscreen check
    if (g_Settings.hideFullscreen) {
        QUERY_USER_NOTIFICATION_STATE state;
        if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
            if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE)
                shouldHide = true;
        }
    }

    // 2. Read media state
    bool isPlaying = false, hasMedia = false;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        isPlaying = g_MediaState.isPlaying;
        hasMedia  = g_MediaState.hasMedia;
    }

    // 3. No media → auto-hide
    if (!hasMedia) {
        g_IsHiddenByNoMedia = true;
        shouldHide = true;
        if (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow))
            ShowWindow(g_hExpandedWindow, SW_HIDE);
    } else {
        g_IsHiddenByNoMedia = false;
    }

    // 4. Idle timeout / Paused check: Hide instantly when not playing
    if (g_Settings.idleTimeout > 0) {
        if (isPlaying) {
            g_IdleSecondsCounter = 0;
            g_IsHiddenByIdle = false;
        } else {
            g_IsHiddenByIdle = true;
        }
    } else {
        g_IsHiddenByIdle = false;
    }

    if (g_IsHiddenByIdle) shouldHide = true;

    // 5. Visibility
    if (shouldHide && IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
    else if (!shouldHide && !IsWindowVisible(hwnd)) {
        HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
        if (hTaskbar && IsWindowVisible(hTaskbar)) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }

    if (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow))
        InvalidateRect(g_hExpandedWindow, NULL, FALSE);

    InvalidateRect(hwnd, NULL, FALSE);
}


LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            ApplyAcrylicBlur(hwnd);
            SetTimer(hwnd, IDT_POLL_MEDIA, 500, NULL);
            RegisterTaskbarHook(hwnd);
            return 0;

        case WM_ERASEBKGND: return 1;
        case WM_CLOSE:      return 0;

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                // If compact bar hovered, use hand cursor as it acts as popup trigger
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
            break;
        }

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_TaskbarHook) { UnhookWinEvent(g_TaskbarHook); g_TaskbarHook = nullptr; }
            if (g_hExpandedWindow) { DestroyWindow(g_hExpandedWindow); g_hExpandedWindow = nullptr; }
            g_SessionManager = nullptr;
            if (g_pAudioVolume) { g_pAudioVolume->Release(); g_pAudioVolume = nullptr; }
            if (g_pMeter) { g_pMeter->Release(); g_pMeter = nullptr; }
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            ApplyAcrylicBlur(hwnd);
            if (g_hExpandedWindow) {
                ApplyAcrylicBlur(g_hExpandedWindow);
                InvalidateRect(g_hExpandedWindow, nullptr, TRUE);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TOGGLE_POPUP:
            ToggleExpandedPopup(hwnd);
            return 0;

        case WM_CHECK_VISIBILITY:
            CheckVisibility(hwnd);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                UpdateMediaInfo();
                CheckVisibility(hwnd);
            }
            else if (wParam == IDT_ANIMATION) {
                bool needsTimer = g_IsScrolling;
                bool isPlaying = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    isPlaying = g_MediaState.isPlaying;
                }
                bool streamActive = false;
                {
                    lock_guard<mutex> lguard(g_Lyrics.lock);
                    streamActive = g_Lyrics.streamLyrics;
                }
                if (isPlaying || IsVisualizerActive() || streamActive) needsTimer = true;

                if (needsTimer) {
                    if (g_IsScrolling) {
                        if (g_ScrollWait > 0) { g_ScrollWait--; }
                        else {
                            g_ScrollOffset += g_LyricScrollSpeed;
                            if (g_ScrollOffset > g_TextWidth + 40) { g_ScrollOffset = 0.0f; g_ScrollWait = 60; }
                        }
                    }
                    UpdateVisualizerFrame();
                    InvalidateRect(hwnd, NULL, FALSE);
                } else {
                    KillTimer(hwnd, IDT_ANIMATION);
                }
            }
            return 0;

        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar) break;
            if (!IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            if (!g_IsHiddenByIdle && !g_IsHiddenByNoMedia && !IsWindowVisible(hwnd)) {
                bool gameModeHide = false;
                if (g_Settings.hideFullscreen) {
                    QUERY_USER_NOTIFICATION_STATE state;
                    if (SUCCEEDED(SHQueryUserNotificationState(&state)))
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE)
                            gameModeHide = true;
                }
                if (!gameModeHide) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }
            RECT rc; GetWindowRect(hTaskbar, &rc);
            int x = rc.left + g_Settings.offsetX;
            int tbH = rc.bottom - rc.top;
            int y = rc.top + (tbH / 2) - (g_Settings.height / 2) + g_Settings.offsetY;
            
            // Calculate animated width based on popup animation progress
            int currentWidth = g_Settings.width;
            if (g_Anim.isOpen || g_Anim.isAnimating) {
                int collapsedWidth = g_Settings.height;
                currentWidth = g_Settings.width - (int)(g_Anim.progress * (g_Settings.width - collapsedWidth));
            }

            RECT myRc; GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y ||
                (myRc.right - myRc.left) != currentWidth ||
                (myRc.bottom - myRc.top) != g_Settings.height) {
                SetWindowPos(hwnd, HWND_TOPMOST, x, y, currentWidth, g_Settings.height, SWP_NOACTIVATE);
            }
            if (g_hExpandedWindow) {
                SetWindowPos(g_hExpandedWindow, nullptr, 0, 0, g_Settings.popupWidth, g_Settings.popupHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                InvalidateRect(g_hExpandedWindow, nullptr, TRUE);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam), y = HIWORD(lParam);
            int artSize = g_Settings.height - 12;
            double scale = g_Settings.buttonScale;
            int startControlX = 6 + artSize + (int)(12 * scale);
            float gap = 28.0f * (float)scale;
            float pX  = (float)startControlX;
            float plX = pX + gap;
            float nX  = plX + gap;
            float radius = 12.0f * (float)scale;
            bool canPrev = true, canPlay = true, canNext = true;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                canPrev = g_MediaState.canGoPrev;
                canPlay = g_MediaState.canPlayPause;
                canNext = g_MediaState.canGoNext;
            }

            int newState = 0;
            if (y > 10 && y < g_Settings.height - 10) {
                if      (x >= pX  - radius && x <= pX  + radius && canPrev) newState = 1;
                else if (x >= plX - radius && x <= plX + radius && canPlay) newState = 2;
                else if (x >= nX  - radius && x <= nX  + radius && canNext) newState = 3;
            }
            if (newState != g_HoverState) { g_HoverState = newState; InvalidateRect(hwnd, NULL, FALSE); }
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_HoverState = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        case WM_LBUTTONUP: {
            if (g_HoverState > 0) {
                bool canPrev = true, canPlay = true, canNext = true;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    canPrev = g_MediaState.canGoPrev;
                    canPlay = g_MediaState.canPlayPause;
                    canNext = g_MediaState.canGoNext;
                }
                bool execute = false;
                if (g_HoverState == 1) execute = canPrev;
                else if (g_HoverState == 2) execute = canPlay;
                else if (g_HoverState == 3) execute = canNext;
                
                if (execute) {
                    SendMediaCommand(g_HoverState);
                }
            } else {
                PostMessage(hwnd, WM_TOGGLE_POPUP, 0, 0);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, 0, 0);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            
            HDC memDC = CreateCompatibleDC(hdc);
            
            // Always create a 32-bit ARGB DIB Section for translucency rendering
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = rc.right;
            bmi.bmiHeader.biHeight = -rc.bottom; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* pBits = nullptr;
            HBITMAP memBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
            
            HBITMAP old = (HBITMAP)SelectObject(memDC, memBmp);
            DrawMediaPanel(memDC, rc.right, rc.bottom);
            
            bool isPlaying = false;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                isPlaying = g_MediaState.isPlaying;
            }
            bool streamActive = false;
            {
                lock_guard<mutex> lguard(g_Lyrics.lock);
                streamActive = g_Lyrics.streamLyrics;
            }
            if (g_IsScrolling || isPlaying || IsVisualizerActive() || streamActive) {
                SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
            }
            
            RECT winRect;
            GetWindowRect(hwnd, &winRect);
            POINT ptDst = { winRect.left, winRect.top };
            SIZE sizeWnd = { rc.right, rc.bottom };
            POINT ptSrc = { 0, 0 };
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, g_MediaWindowAlpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);
            
            SelectObject(memDC, old);
            DeleteObject(memBmp);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }

        default:
            if (msg == g_TaskbarCreatedMsg) {
                if (g_TaskbarHook) { UnhookWinEvent(g_TaskbarHook); g_TaskbarHook = nullptr; }
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ============================================================
// Main Thread
// ============================================================
void MediaThread() {
    winrt::init_apartment();
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput gdipInput;
    ULONG_PTR gdipToken;
    GdiplusStartup(&gdipToken, &gdipInput, NULL);

    InitVolumeAPI();
    InitMeterAPI();

    // Register compact bar window class
    WNDCLASS wc = {};
    wc.lpfnWndProc   = MediaWndProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("WindhawkMusicLounge_GSMTC");
    wc.hCursor       = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);

    // Register expanded popup window class
    WNDCLASS wcExp = {};
    wcExp.lpfnWndProc   = ExpandedWndProc;
    wcExp.hInstance     = GetModuleHandle(NULL);
    wcExp.lpszClassName = TEXT("WindhawkMusicLounge_Expanded");
    wcExp.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wcExp);

    // Create compact bar
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hUser32) CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand");

    if (CreateWindowInBand) {
        g_hMediaWindow = CreateWindowInBand(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL,
            ZBID_IMMERSIVE_NOTIFICATION);
    }
    if (!g_hMediaWindow) {
        g_hMediaWindow = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL);
    }
    ApplyAcrylicBlur(g_hMediaWindow);

    // Create expanded popup (hidden initially)
    g_hExpandedWindow = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        wcExp.lpszClassName, TEXT("MusicLoungeExpanded"),
        WS_POPUP,
        0, 0, g_Settings.popupWidth, g_Settings.popupHeight,
        NULL, NULL, wcExp.hInstance, NULL);
    if (g_hExpandedWindow) {
        // Opacity is handled by UpdateLayeredWindow on next paint
        ApplyAcrylicBlur(g_hExpandedWindow);
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName,    wc.hInstance);
    UnregisterClass(wcExp.lpszClassName, wcExp.hInstance);
    GdiplusShutdown(gdipToken);
    CoUninitialize();
    winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

// ============================================================
// Windhawk Callbacks
// ============================================================
BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-music-lounge-pro");
    LoadSettings();
    g_Running = true;

    // Start Audio Capture Thread for Spectrogram visualizer
    g_CaptureRunning = true;
    g_CaptureThread = std::thread(AudioCaptureThread);

    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    g_Running = false;

    // Stop Audio Capture Thread
    g_CaptureRunning = false;
    if (g_CaptureThread.joinable()) {
        g_CaptureThread.join();
    }

    if (g_hMediaWindow) SendMessage(g_hMediaWindow, APP_WM_CLOSE, 0, 0);
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread;
        g_pMediaThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hMediaWindow) {
        PostMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
        PostMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0);
        PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod boilerplate
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() { Wh_Log(L">"); ExitThread(0); }

BOOL Wh_ModInit() {
    bool isService = false, isToolModProcess = false, isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) { Wh_Log(L"CommandLineToArgvW failed"); return FALSE; }

    for (int i = 1; i < argc; i++)
        if (wcscmp(argv[i], L"-service") == 0) { isService = true; break; }

    for (int i = 1; i < argc - 1; i++)
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i+1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }

    LocalFree(argv);
    if (isService) return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) { Wh_Log(L"CreateMutex failed"); ExitProcess(1); }
        if (GetLastError() == ERROR_ALREADY_EXISTS) { Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID); ExitProcess(1); }
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint = (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

    WCHAR currentProcessPath[MAX_PATH];
    if (!GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) return;

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule) { Wh_Log(L"No kernelbase.dll/kernel32.dll"); return; }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto pCreateProcessInternalW = (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) { Wh_Log(L"No CreateProcessInternalW"); return; }

    STARTUPINFO si{ .cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
            nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
