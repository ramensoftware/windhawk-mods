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


## 🎵 Features
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
* **Instant Responsive Controls:** Local state caching guarantees instant feedback on play/pause, prev, next, shuffle, and repeat without lag.
* **Seek Lock Protection:** Smooth timeline extrapolation and browser safety locks prevent the tracker from resetting to 0:00 during active seek actions.
* **Isomorphic Music Visualizer:** A beautiful bottom-aligned 4-bar visualizer that bounces to the actual audio output peaks of Windows at 60fps and rests when silent. Fully toggleable between real-time peak audio and smooth mock visualization.
* **Fullscreen Mode:** Hides automatically when running fullscreen applications.
* **No Media Auto-Hide:** Hides when nothing is playing, reappears instantly on playback.
* **Idle Timeout:** Optional auto-hide after pause for X seconds.
* **Volume:** Scroll over compact bar to adjust volume.


## 🖥️ Requirements
* **Disable Widgets:** Taskbar Settings → Widgets → Off.
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
const IID my_IID_IAudioMeterInformation = {0xC0216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

// =======================================================================
// Constants
// =======================================================================
const WCHAR* FONT_NAME           = L"Segoe UI Variable Display";
const int    DEFAULT_POPUP_WIDTH  = 320;
const int    DEFAULT_POPUP_HEIGHT = 380;
const int    POPUP_GAP           = 8;      // gap between compact bar and popup
const int    ANIM_OPEN_MS        = 180;
const int    ANIM_CLOSE_MS       = 120;
const int    ANIM_FPS            = 60;
const int    ANIM_INTERVAL       = 1000 / ANIM_FPS;  // ~16ms

// =======================================================================
// DWM / Composition API
// =======================================================================
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

// =======================================================================
// Z-Band API
// =======================================================================
enum ZBID {
    ZBID_DEFAULT = 0, ZBID_DESKTOP = 1, ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3, ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5, ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7, ZBID_IMMERSIVE_INACTIVEBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9, ZBID_IMMERSIVE_ACTIVEBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11, ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13, ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15, ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17, ZBID_ABOVELOCK_UX = 18,
};
typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID, DWORD);

// =======================================================================
// Settings
// =======================================================================
struct ModSettings {
    int    width            = 300;
    int    height           = 52;
    int    fontSize         = 15;
    double buttonScale      = 1.0;
    bool   hideFullscreen   = false;
    int    idleTimeout      = 0;
    int    offsetX          = 12;
    int    offsetY          = 0;
    bool   autoTheme        = true;
    DWORD  manualTextColor  = 0xFFFFFFFF;
    int    bgOpacity        = 0;
    int    popupWidth       = 320;
    int    popupHeight      = 380;
    int    popupFontSize    = 15;
    int    popupIconSize    = 28;
    bool   showVisualizer   = true;
    bool   realTimeVisualizer = false;
    double visualizerScale  = 1.0;
    int    visualizerHeight = 14;
    bool   glassBackdrop    = false;
    bool   useBlur          = false;
    bool   fetchLyrics      = true;
    int    lyricsFontSize   = 14;
} g_Settings;

// =======================================================================
// Animation State
// =======================================================================
struct AnimState {
    bool  isOpen         = false;   // true = popup visible/opening
    bool  isAnimating    = false;
    float progress       = 0.0f;    // 0.0 = fully closed, 1.0 = fully open
    bool  opening        = true;    // direction
    int   totalFrames    = 0;
    int   currentFrame   = 0;
} g_Anim;

// =======================================================================
// Timeline / Volume State
// =======================================================================
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

// =======================================================================
// Lyrics State & Fetching
// =======================================================================
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
    
    FontFamily emojiFamiliy(L"Segoe UI Emoji", nullptr);
    Font emojiFont(&emojiFamiliy, normalFont->GetSize(), normalFont->GetStyle(), UnitPixel);
    
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
            
            if (g_hExpandedWindow && IsWindowVisible(g_hExpandedWindow)) {
                InvalidateRect(g_hExpandedWindow, nullptr, FALSE);
            }
        } catch (...) {}
        
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

// =======================================================================
// Global State
// =======================================================================
HWND g_hMediaWindow    = NULL;
HWND g_hExpandedWindow = NULL;
bool g_Running         = true;
int  g_HoverState      = 0;
HWINEVENTHOOK g_TaskbarHook = nullptr;
UINT g_TaskbarCreatedMsg    = RegisterWindowMessage(L"TaskbarCreated");

int  g_IdleSecondsCounter = 0;
bool g_IsHiddenByIdle     = false;
bool g_IsHiddenByNoMedia  = false;

// Hover state for expanded panel controls
int  g_ExpHoverBtn   = 0;   // 1=prev 2=play 3=next 4=shuffle 5=repeat 6=appPrev 7=appNext
bool g_ExpHoverSeek  = false;
bool g_ExpHoverVol   = false;

// Optimistic Controls state locks
ULONGLONG g_OptimisticTime  = 0;
bool      g_OptimisticPlaying = false;

// Session Switcher and Swipe State
int       g_CurrentSessionIndex = 0;
bool      g_UserSelectedSession  = false;
bool      g_IsDraggingSession    = false;
int       g_DragStartX           = 0;
float     g_DragOffsetX          = 0.0f;
float     g_ArtSlideOffset       = 0.0f;
float     g_ArtSlideTarget       = 0.0f;

// =======================================================================
// Media State
// =======================================================================
std::atomic<bool> g_UpdatingMedia{false};
std::atomic<bool> g_PendingUpdate{false};

struct MediaState {
    wstring title     = L"Waiting for media...";
    wstring artist    = L"";
    wstring appName   = L"";
    wstring appId     = L"";
    bool    isPlaying = false;
    bool    hasMedia  = false;
    Bitmap* albumArt  = nullptr;
    Bitmap* appIcon   = nullptr;
    bool    shuffle   = false;
    winrt::Windows::Media::MediaPlaybackAutoRepeatMode repeatMode =
        winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
} g_MediaState;

double GetLivePosition() {
    [...]
}

[remainder of the file continues...]
