// ==WindhawkMod==
// @id              taskbar-music-lounge-ae
// @name            Taskbar Music Lounge AE
// @description     A native-style music ticker with media controls, with Libby audiobook support.
// @version         1.0.0
// @author          StarlightDaemon
// @github          https://github.com/StarlightDaemon
// @homepage        https://www.starlightdaemon.dev/
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lwinhttp
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge AE

**Audiobook Edition** — a fork of Taskbar Music Lounge v4.0.1 by Hashah2311, extended with
Libby audiobook support and upstream bug fixes. Original mod:
https://windhawk.net/mods/taskbar-music-lounge

A media controller that uses Windows 11 native DWM styling for a seamless look.

## ✨ Features
* **Universal Support:** Smart scanning detects active playback from any app, not just the "focused" one.
* **Album Art:** Displays current track cover art.
* **Libby Support:** Automatically fetches audiobook cover art from Open Library for Libby (libbyapp.com) sessions. Right-click the cover to try a different cover, remove it, lock it, or restore it.
* **Fullscreen Mode:** Hides automatically when running full-screen applications.
* **Native Look:** Uses Windows 11 hardware-accelerated rounding and acrylic blur.
* **Idle Timeout:** Optional setting to fade out the widget when music is paused for X seconds.
* **Controls:** Play/Pause, Next, Previous.
* **Volume:** Scroll over widget to adjust volume.

## ⚠️ Requirements
* **Disable Widgets:** Taskbar Settings -> Widgets -> Off.
* **Windows 11:** Required for rounded corners.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 300
  $name: Panel Width
- PanelHeight: 48
  $name: Panel Height
- FontSize: 11
  $name: Font Size
- ButtonScale: "1.0"
  $name: Button Scale (1.0 = Normal, 2.0 = 4K)
- HideFullscreen: false
  $name: Hide when Fullscreen
- IdleTimeout: 0
  $name: Auto-hide when paused (Seconds). Set 0 to disable.
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- AutoTheme: true
  $name: Auto Theme
- TextColor: "0xFFFFFF"
  $name: Manual Text Color (Hex)
- BgOpacity: 0
  $name: Acrylic Tint Opacity (0-255). Keep 0 for pure glass.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <winhttp.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h> 
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstdio>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

// --- Constants ---
const WCHAR* FONT_NAME = L"Segoe UI Variable Display"; 

// --- DWM API ---
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

// --- Z-Band API ---
enum ZBID {
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam,
    DWORD dwBand
);

// --- Configurable State ---
struct ModSettings {
    int width = 300;
    int height = 48;
    int fontSize = 11;
    double buttonScale = 1.0; 
    bool hideFullscreen = false;
    int idleTimeout = 0; 
    int offsetX = 12;
    int offsetY = 0;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF; 
    int bgOpacity = 0;   
} g_Settings;

// --- Global State ---
HWND g_hMediaWindow = NULL;
int g_HoverState = 0;
HWINEVENTHOOK g_TaskbarHook = nullptr; 
UINT g_TaskbarCreatedMsg = 0;

// Idle Tracking
int g_IdleSecondsCounter = 0;
bool g_IsHiddenByIdle = false;

// Data Model
struct MediaState {
    wstring title   = L"Waiting for media...";
    wstring artist  = L"";
    bool isPlaying  = false;
    bool hasMedia   = false;
    bool isLibby    = false;
    Bitmap* albumArt = nullptr;
    mutex lock;
} g_MediaState;

// Animation
int g_ScrollOffset = 0;
int g_TextWidth = 0;
bool g_IsScrolling = false;
int g_ScrollWait = 60;

// --- Settings ---
void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    g_Settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;
    
    PCWSTR scaleStr = Wh_GetStringSetting(L"ButtonScale");
    if (scaleStr) {
        g_Settings.buttonScale = _wtof(scaleStr);
        Wh_FreeStringSetting(scaleStr);
    } else {
        g_Settings.buttonScale = 1.0;
    }
    if (g_Settings.buttonScale < 0.5) g_Settings.buttonScale = 0.5;
    if (g_Settings.buttonScale > 4.0) g_Settings.buttonScale = 4.0;

    g_Settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;
    g_Settings.idleTimeout = Wh_GetIntSetting(L"IdleTimeout");

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;
    
    g_Settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    if (g_Settings.bgOpacity < 0) g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255) g_Settings.bgOpacity = 255;

    if (g_Settings.width < 100) g_Settings.width = 300;
    if (g_Settings.height < 24) g_Settings.height = 48;
}

// --- WinRT / GSMTC ---
GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;
GlobalSystemMediaTransportControlsSession g_ActiveSession = nullptr;
std::mutex g_SessionMutex;
std::atomic<bool> g_MediaUpdatePending{false};

// --- Libby cover cache ---
#define MAX_COVER_CACHE 5
struct CoverCacheEntry {
    wstring title;
    Bitmap* bmp             = nullptr;
    bool    fetched         = false;
    bool    fetchInProgress = false; // HandleCoverWrong thread is running; poll must not re-fetch
    bool    suppressed      = false; // user chose "Remove Cover"
    bool    locked          = false; // user chose "Lock Cover" — no auto-updates
    string  currentCoverId;          // OL cover_i currently displayed
    vector<string> triedCoverIds;    // OL cover_i values already shown
};
static vector<CoverCacheEntry> g_CoverCache;
static mutex g_CoverCacheMutex;
static HINTERNET g_hWinHttpSession = nullptr;

struct CoverResult { Bitmap* bmp = nullptr; string coverId; };

Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* nativeStream = nullptr;
    if (SUCCEEDED(CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(winrt::get_abi(stream)), IID_PPV_ARGS(&nativeStream)))) {
        Bitmap* bmp = Bitmap::FromStream(nativeStream);
        nativeStream->Release();
        if (bmp && bmp->GetLastStatus() == Ok) return bmp;
        delete bmp;
    }
    return nullptr;
}

// Percent-encodes a title string for use in a URL query parameter.
static wstring UrlEncodeTitle(const wstring& s) {
    wstring out;
    out.reserve(s.size() * 3);
    for (size_t i = 0; i < s.size(); ) {
        wchar_t c = s[i];
        if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
            (c >= L'0' && c <= L'9') ||
            c == L'-' || c == L'_' || c == L'.' || c == L'~') {
            out += c; ++i;
        } else if (c == L' ') {
            out += L'+'; ++i;
        } else {
            // Detect surrogate pair and encode both wchar_t as one Unicode code point.
            int len = (c >= 0xD800 && c <= 0xDBFF &&
                       i + 1 < s.size() &&
                       s[i+1] >= 0xDC00 && s[i+1] <= 0xDFFF) ? 2 : 1;
            char utf8[5] = {};
            int n = WideCharToMultiByte(CP_UTF8, 0, &s[i], len, utf8, 4, nullptr, nullptr);
            for (int j = 0; j < n; j++) {
                wchar_t buf[4];
                swprintf(buf, 4, L"%%%02X", (unsigned char)utf8[j]);
                out += buf;
            }
            i += len;
        }
    }
    return out;
}

// Synchronous HTTPS GET — returns raw response body, empty on any failure.
static vector<BYTE> HttpsGet(LPCWSTR host, LPCWSTR path) {
    vector<BYTE> result;
    HINTERNET hSession = g_hWinHttpSession;
    if (!hSession) return result;

    HINTERNET hConnect = WinHttpConnect(hSession, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) return result;

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path, nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        return result;
    }

    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           nullptr, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, nullptr)) {
        DWORD status = 0; DWORD sz = sizeof(status);
        WinHttpQueryHeaders(hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            nullptr, &status, &sz, nullptr);
        if (status == 200) {
            DWORD read;
            do {
                DWORD avail = 0;
                WinHttpQueryDataAvailable(hRequest, &avail);
                if (!avail) break;
                size_t prev = result.size();
                result.resize(prev + avail);
                if (!WinHttpReadData(hRequest, result.data() + prev, avail, &read)) break;
                result.resize(prev + read);
            } while (read > 0);
        }
    }
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    return result;
}

// Strip subtitle after first ':' or ' - ' for cleaner search queries.
static wstring StripSubtitle(const wstring& title) {
    auto pos = title.find(L':');
    if (pos != wstring::npos) return title.substr(0, pos);
    pos = title.find(L" - ");
    if (pos != wstring::npos) return title.substr(0, pos);
    return title;
}

// Scan Open Library docs JSON for a cover_i that matches either the stripped
// or full title, skipping any cover IDs already tried. No fallback to first
// result — returns empty string rather than show the wrong cover.
static string PickCoverId(const string& json,
                           const string& strippedLower,
                           const string& fullLower,
                           const vector<string>& excluded) {
    auto docsPos = json.find("\"docs\":[");
    if (docsPos == string::npos) return {};
    size_t pos = docsPos + strlen("\"docs\":[");

    while (pos < json.size()) {
        auto objStart = json.find('{', pos);
        if (objStart == string::npos) break;
        auto objEnd = json.find('}', objStart);
        if (objEnd == string::npos) break;
        string doc = json.substr(objStart, objEnd - objStart + 1);

        string coverId;
        auto cp = doc.find("\"cover_i\":");
        if (cp != string::npos) {
            cp += strlen("\"cover_i\":");
            while (cp < doc.size() && (doc[cp] == ' ' || doc[cp] == '\t')) ++cp;
            while (cp < doc.size() && isdigit((unsigned char)doc[cp])) coverId += doc[cp++];
        }

        string docTitle;
        auto tp = doc.find("\"title\":\"");
        if (tp != string::npos) {
            tp += strlen("\"title\":\"");
            while (tp < doc.size() && doc[tp] != '"') {
                if (doc[tp] == '\\' && tp + 1 < doc.size()) {
                    ++tp;                    // skip backslash
                    docTitle += doc[tp++];   // include escaped character literally
                } else {
                    docTitle += doc[tp++];
                }
            }
            transform(docTitle.begin(), docTitle.end(), docTitle.begin(), ::tolower);
        }

        if (!coverId.empty() &&
            find(excluded.begin(), excluded.end(), coverId) == excluded.end()) {
            // Accept: exact stripped match, exact full match, or OL title is
            // "Stripped Title: Subtitle" / "Stripped Title - Subtitle" form.
            bool match = (docTitle == strippedLower) ||
                         (docTitle == fullLower) ||
                         (docTitle.find(strippedLower + ":") == 0) ||
                         (docTitle.find(strippedLower + " -") == 0);
            if (match) return coverId;
        }
        pos = objEnd + 1;
        if (pos < json.size() && json[pos] == ']') break;
    }
    return {};
}

// Decode raw JPEG bytes into a GDI+ Bitmap.
static Bitmap* DecodeCoverBytes(const vector<BYTE>& imgBytes) {
    if (imgBytes.empty()) return nullptr;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, imgBytes.size());
    if (!hMem) return nullptr;
    void* p = GlobalLock(hMem);
    if (!p) { GlobalFree(hMem); return nullptr; }
    memcpy(p, imgBytes.data(), imgBytes.size());
    GlobalUnlock(hMem);
    IStream* stream = nullptr;
    if (FAILED(CreateStreamOnHGlobal(hMem, TRUE, &stream))) { GlobalFree(hMem); return nullptr; }
    Bitmap* bmp = Bitmap::FromStream(stream);
    stream->Release();
    if (!bmp || bmp->GetLastStatus() != Ok) { delete bmp; return nullptr; }
    return bmp;
}

// Single Open Library search pass: searchParam is either "title" or "q".
static CoverResult OLSearchPass(const wstring& param, const wstring& query,
                                 const string& strippedLower, const string& fullLower,
                                 const vector<string>& excluded) {
    wstring path = L"/search.json?" + param + L"=" + UrlEncodeTitle(query)
                 + L"&fields=cover_i,title&limit=5";
    auto jsonBytes = HttpsGet(L"openlibrary.org", path.c_str());
    if (jsonBytes.empty()) return {};
    string json(jsonBytes.begin(), jsonBytes.end());
    string coverId = PickCoverId(json, strippedLower, fullLower, excluded);
    if (coverId.empty()) return {};
    wstring wId(coverId.begin(), coverId.end());
    auto imgBytes = HttpsGet(L"covers.openlibrary.org",
                             (L"/b/id/" + wId + L"-L.jpg?default=false").c_str());
    return { DecodeCoverBytes(imgBytes), coverId };
}

// Two-pass Open Library fetch: title= (precise) then q= (broader).
// Skips cover IDs in 'excluded'. Returns nullptr bmp on complete miss.
static CoverResult FetchOpenLibraryCover(const wstring& title,
                                          const vector<string>& excluded) {
    wstring stripped = StripSubtitle(title);

    string strippedLower(stripped.begin(), stripped.end());
    transform(strippedLower.begin(), strippedLower.end(), strippedLower.begin(), ::tolower);
    string fullLower(title.begin(), title.end());
    transform(fullLower.begin(), fullLower.end(), fullLower.begin(), ::tolower);

    CoverResult r = OLSearchPass(L"title", stripped, strippedLower, fullLower, excluded);
    if (r.bmp) return r;
    return OLSearchPass(L"q", stripped, strippedLower, fullLower, excluded);
}

// Cache-aware fetch. Returns Bitmap* owned by caller, or nullptr on miss/suppressed.
// If the entry exists but fetched=false (HandleCoverWrong cleared it), the existing
// triedCoverIds are passed to the fetch so the poll thread cannot race back to the
// same wrong cover. Updates the existing entry in-place rather than pushing a duplicate.
static Bitmap* GetOrFetchCover(const wstring& title) {
    vector<string> tried;
    {
        lock_guard<mutex> lk(g_CoverCacheMutex);
        for (auto& e : g_CoverCache) {
            if (e.title != title) continue;
            if (e.suppressed) return nullptr;
            if (e.locked || e.fetched) return e.bmp ? e.bmp->Clone() : nullptr;
            // HandleCoverWrong's dedicated thread is already fetching — skip to
            // avoid a duplicate fetch with the same exclusion list.
            if (e.fetchInProgress) return nullptr;
            tried = e.triedCoverIds; // carry tried IDs from HandleCoverWrong
            break;
        }
    }
    CoverResult r = FetchOpenLibraryCover(title, tried);
    {
        lock_guard<mutex> lk(g_CoverCacheMutex);
        // Update existing entry if present; otherwise create a new one.
        CoverCacheEntry* target = nullptr;
        for (auto& e : g_CoverCache)
            if (e.title == title) { target = &e; break; }
        if (!target) {
            if (g_CoverCache.size() >= MAX_COVER_CACHE) {
                delete g_CoverCache.front().bmp;
                g_CoverCache.erase(g_CoverCache.begin());
            }
            g_CoverCache.push_back({});
            target = &g_CoverCache.back();
            target->title = title;
        }
        delete target->bmp;
        target->bmp            = r.bmp ? r.bmp->Clone() : nullptr;
        target->fetched        = true;
        target->currentCoverId = r.coverId;
        if (!r.coverId.empty()) target->triedCoverIds.push_back(r.coverId);
    }
    return r.bmp;
}

void UpdateMediaInfo() {
    GlobalSystemMediaTransportControlsSession session = nullptr;
    try {
        {
            lock_guard<mutex> slock(g_SessionMutex);
            if (!g_SessionManager) {
                g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            }
            if (!g_SessionManager) return;

            // Iterate ALL sessions to find one that is actively PLAYING.
            bool foundActive = false;
            auto sessionsList = g_SessionManager.GetSessions();
            for (auto const& s : sessionsList) {
                auto pb = s.GetPlaybackInfo();
                if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                    session = s;
                    foundActive = true;
                    break;
                }
            }
            if (!foundActive) {
                session = g_SessionManager.GetCurrentSession();
            }
            g_ActiveSession = session;
        }

        if (session) {
            // All blocking IO runs outside every lock.
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info  = session.GetPlaybackInfo();
            wstring sourceApp = session.SourceAppUserModelId().c_str();

            wstring newTitle = props.Title().c_str();

            // Libby (Chrome extension bbcjjjnjadekjghhbjddadjgfc) only populates
            // Title with the browser tab title: "Libby - <Action>: <BookTitle>".
            // No other fields are set. Strip the prefix to surface just the book title.
            bool isLibby = sourceApp.find(L"bbcjjjnjadekjghhbjddadjgfc") != wstring::npos;
            if (isLibby) {
                auto pos = newTitle.rfind(L": ");
                if (pos != wstring::npos)
                    newTitle = newTitle.substr(pos + 2);
            }

            // Briefly check under lock whether we actually need a new cover.
            bool needCover;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                needCover = (newTitle != g_MediaState.title || g_MediaState.albumArt == nullptr);
            }

            // Issue 4 short-circuit: if Libby title is unchanged and the cache
            // entry is already suppressed (user removed cover) or is a confirmed
            // OL miss (fetched=true, bmp==nullptr), skip the GetOrFetchCover call.
            // The two lock acquisitions are sequential — never nested.
            if (needCover && isLibby && newTitle == g_MediaState.title) {
                lock_guard<mutex> clk(g_CoverCacheMutex);
                for (auto& e : g_CoverCache) {
                    if (e.title != newTitle) continue;
                    if (e.suppressed || (e.fetched && e.bmp == nullptr))
                        needCover = false;
                    break;
                }
            }

            // Fetch cover outside all locks — may block for HTTP or WinRT IO.
            Bitmap* newCover = nullptr;
            if (needCover) {
                if (isLibby) {
                    newCover = GetOrFetchCover(newTitle);
                } else {
                    auto thumbRef = props.Thumbnail();
                    if (thumbRef) {
                        auto stream = thumbRef.OpenReadAsync().get();
                        newCover = StreamToBitmap(stream);
                    }
                }
            }

            {
                lock_guard<mutex> guard(g_MediaState.lock);
                // Only commit the fetched cover if the title hasn't changed while
                // the HTTP request was running; otherwise discard the stale bitmap.
                if (needCover && g_MediaState.title == newTitle) {
                    if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                    g_MediaState.albumArt = newCover;
                    newCover = nullptr;
                }
                g_MediaState.title    = newTitle;
                g_MediaState.artist   = props.Artist().c_str();
                g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                g_MediaState.hasMedia  = true;
                g_MediaState.isLibby   = isLibby;
            }
            if (newCover) delete newCover; // discarded if track changed mid-fetch
        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.isLibby  = false;
            g_MediaState.title    = L"No Media";
            g_MediaState.artist   = L"";
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
        }
    } catch (...) {
        // Issue 5: mark no-media under its own lock, then separately clear the
        // session objects so the next successful poll can re-init via RequestAsync().
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
        }
        {
            lock_guard<mutex> slock(g_SessionMutex);
            g_SessionManager = nullptr;
            g_ActiveSession  = nullptr;
        }
    }
}

void SendMediaCommand(int cmd) {
    try {
        lock_guard<mutex> slock(g_SessionMutex);
        if (!g_ActiveSession) return;
        if (cmd == 1) g_ActiveSession.TrySkipPreviousAsync();
        else if (cmd == 2) g_ActiveSession.TryTogglePlayPauseAsync();
        else if (cmd == 3) g_ActiveSession.TrySkipNextAsync();
    } catch (...) {}
}

// --- Visuals ---
bool IsSystemLightMode() {
    DWORD value = 0; DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

DWORD GetCurrentTextColor() {
    if (g_Settings.autoTheme) return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    return g_Settings.manualTextColor;
}

void UpdateAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            DWORD tint = 0; 
            if (g_Settings.autoTheme) {
                tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
            } else {
                tint = (g_Settings.bgOpacity << 24) | (0xFFFFFF); 
            }
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
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

void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0)); 

    Color mainColor{GetCurrentTextColor()};
    
    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title = g_MediaState.title;
        state.artist = g_MediaState.artist;
        state.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
    }

    // 1. Album Art (Rounded)
    int artSize = height - 12;
    int artX = 6;
    int artY = 6;
    
    GraphicsPath path;
    AddRoundedRect(path, artX, artY, artSize, artSize, 8); 

    if (state.albumArt) {
        graphics.SetClip(&path);
        graphics.DrawImage(state.albumArt, artX, artY, artSize, artSize);
        graphics.ResetClip();
        delete state.albumArt;
    } else {
        SolidBrush placeBrush{Color(40, 128, 128, 128)};
        graphics.FillPath(&placeBrush, &path);
    }

    // 2. Controls (Scaled)
    double scale = g_Settings.buttonScale;
    int startControlX = artX + artSize + (int)(12 * scale);
    int controlY = height / 2;

    SolidBrush iconBrush{mainColor};
    SolidBrush hoverBrush{Color(255, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
    SolidBrush activeBg{Color(40, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};

    float circleR = 12.0f * (float)scale; 
    float iconW = 8.0f * (float)scale;
    float iconH = 12.0f * (float)scale; 
    float gap = 28.0f * (float)scale;
    
    // Prev
    float pX = (float)startControlX;
    if (g_HoverState == 1) graphics.FillEllipse(&activeBg, pX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF prevPts[3] = { PointF(pX + iconW, (float)controlY - (iconH/2)), PointF(pX + iconW, (float)controlY + (iconH/2)), PointF(pX, (float)controlY) };
    graphics.FillPolygon(g_HoverState == 1 ? &hoverBrush : &iconBrush, prevPts, 3);
    graphics.FillRectangle(g_HoverState == 1 ? &hoverBrush : &iconBrush, pX, (float)controlY - (iconH/2), 2.0f * (float)scale, iconH);

    // Play/Pause
    float plX = pX + gap;
    if (g_HoverState == 2) graphics.FillEllipse(&activeBg, plX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    if (state.isPlaying) {
        float barW = 3.0f * (float)scale;
        float barH = 14.0f * (float)scale;
        graphics.FillRectangle(g_HoverState == 2 ? &hoverBrush : &iconBrush, plX - (barW + 1), (float)controlY - (barH/2), barW, barH);
        graphics.FillRectangle(g_HoverState == 2 ? &hoverBrush : &iconBrush, plX + 1, (float)controlY - (barH/2), barW, barH);
    } else {
        float playW = 10.0f * (float)scale;
        float playH = 16.0f * (float)scale;
        PointF playPts[3] = { PointF(plX - (playW/2), (float)controlY - (playH/2)), PointF(plX - (playW/2), (float)controlY + (playH/2)), PointF(plX + (playW/2), (float)controlY) };
        graphics.FillPolygon(g_HoverState == 2 ? &hoverBrush : &iconBrush, playPts, 3);
    }

    // Next
    float nX = plX + gap;
    if (g_HoverState == 3) graphics.FillEllipse(&activeBg, nX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF nextPts[3] = { PointF(nX - iconW, (float)controlY - (iconH/2)), PointF(nX - iconW, (float)controlY + (iconH/2)), PointF(nX, (float)controlY) };
    graphics.FillPolygon(g_HoverState == 3 ? &hoverBrush : &iconBrush, nextPts, 3);
    graphics.FillRectangle(g_HoverState == 3 ? &hoverBrush : &iconBrush, nX, (float)controlY - (iconH/2), 2.0f * (float)scale, iconH);

    // 3. Text
    int textX = (int)(nX + (20 * scale));
    int textMaxW = width - textX - 10;
    
    wstring fullText = state.title;
    if (!state.artist.empty()) fullText += L" • " + state.artist;

    FontFamily fontFamily(FONT_NAME, nullptr);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);
    SolidBrush textBrush{mainColor};
    
    RectF layoutRect(0, 0, 2000, 100); 
    RectF boundRect;
    graphics.MeasureString(fullText.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    Region textClip(Rect(textX, 0, textMaxW, height));
    graphics.SetClip(&textClip);

    float textY = (height - boundRect.Height) / 2.0f;

    if (g_TextWidth > textMaxW) {
        g_IsScrolling = true;
        float drawX = (float)(textX - g_ScrollOffset);
        graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX, textY), &textBrush);
        if (drawX + g_TextWidth < width) {
             graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX + g_TextWidth + 40, textY), &textBrush);
        }
    } else {
        g_IsScrolling = false;
        g_ScrollOffset = 0;
        graphics.DrawString(fullText.c_str(), -1, &font, PointF((float)textX, textY), &textBrush);
    }
}

// --- Cover art right-click actions ---
#define IDM_COVER_WRONG  2001
#define IDM_COVER_REMOVE 2002
#define IDM_COVER_RESET  2003
#define IDM_COVER_LOCK   2004

static void HandleCoverWrong(HWND hwnd, const wstring& title) {
    vector<string> tried;
    {
        lock_guard<mutex> lk(g_CoverCacheMutex);
        for (auto& e : g_CoverCache) {
            if (e.title != title) continue;
            tried = e.triedCoverIds;
            delete e.bmp; e.bmp = nullptr;
            e.fetched         = false;
            e.fetchInProgress = true; // block poll-driven GetOrFetchCover until we finish
            break;
        }
    }
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
    }
    InvalidateRect(hwnd, NULL, FALSE);

    thread([hwnd, title, tried]() {
        CoverResult r = FetchOpenLibraryCover(title, tried);
        {
            lock_guard<mutex> lk(g_CoverCacheMutex);
            for (auto& e : g_CoverCache) {
                if (e.title != title) continue;
                delete e.bmp;
                e.bmp             = r.bmp ? r.bmp->Clone() : nullptr;
                e.fetched         = true;
                e.fetchInProgress = false; // poll may resume
                if (!r.coverId.empty()) {
                    e.currentCoverId = r.coverId;
                    e.triedCoverIds.push_back(r.coverId);
                }
                break;
            }
        }
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            if (g_MediaState.title == title) {
                delete g_MediaState.albumArt;
                g_MediaState.albumArt = r.bmp;
                r.bmp = nullptr;
            }
        }
        delete r.bmp;
        PostMessage(hwnd, WM_APP + 11, 0, 0);
    }).detach();
}

static void HandleCoverRemove(HWND hwnd, const wstring& title) {
    {
        lock_guard<mutex> lk(g_CoverCacheMutex);
        bool found = false;
        for (auto& e : g_CoverCache) {
            if (e.title != title) continue;
            delete e.bmp; e.bmp = nullptr;
            e.fetched    = true;
            e.suppressed = true;
            found = true;
            break;
        }
        if (!found) {
            CoverCacheEntry entry;
            entry.title      = title;
            entry.fetched    = true;
            entry.suppressed = true;
            if (g_CoverCache.size() >= MAX_COVER_CACHE) {
                delete g_CoverCache.front().bmp;
                g_CoverCache.erase(g_CoverCache.begin());
            }
            g_CoverCache.push_back(move(entry));
        }
    }
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
    }
    InvalidateRect(hwnd, NULL, FALSE);
}

static void HandleCoverReset(HWND hwnd, const wstring& title) {
    {
        lock_guard<mutex> lk(g_CoverCacheMutex);
        g_CoverCache.erase(
            remove_if(g_CoverCache.begin(), g_CoverCache.end(),
                      [&](const CoverCacheEntry& e){ return e.title == title; }),
            g_CoverCache.end());
    }
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
    }
    InvalidateRect(hwnd, NULL, FALSE);

    // Issue 6: don't wait for the next 1-second poll — kick off an immediate
    // UpdateMediaInfo fetch so the new cover appears as soon as the OL HTTP
    // response returns.  If a poll is already in flight, skip (it will see
    // albumArt==nullptr and call GetOrFetchCover on its own).
    if (!g_MediaUpdatePending.exchange(true)) {
        std::thread([hwnd]() {
            winrt::init_apartment();
            UpdateMediaInfo();
            g_MediaUpdatePending = false;
            winrt::uninit_apartment();
            PostMessage(hwnd, WM_APP + 11, 0, 0);
        }).detach();
    }
}

static void HandleCoverLock(HWND hwnd, const wstring& title) {
    {
        lock_guard<mutex> lk(g_CoverCacheMutex);
        bool found = false;
        for (auto& e : g_CoverCache) {
            if (e.title == title) { e.locked = !e.locked; found = true; break; }
        }
        if (!found) {
            // Entry was evicted or never created; treat missing as unlocked → lock it.
            CoverCacheEntry entry;
            entry.title   = title;
            entry.locked  = true;
            entry.fetched = false; // allow next poll to attempt a cover fetch
            if (g_CoverCache.size() >= MAX_COVER_CACHE) {
                delete g_CoverCache.front().bmp;
                g_CoverCache.erase(g_CoverCache.begin());
            }
            g_CoverCache.push_back(move(entry));
        }
    }
    InvalidateRect(hwnd, NULL, FALSE);
}

// --- Event Hook ---
bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd) return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

void CALLBACK TaskbarEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG, LONG,
    DWORD, DWORD
) {
    if (!IsTaskbarWindow(hwnd) || !g_hMediaWindow) return;
    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

// Register Event Hook scoped to Taskbar Thread
void RegisterTaskbarHook(HWND hwnd)
{
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(hTaskbar, &pid);
        if (tid != 0) {
            g_TaskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE,
                EVENT_OBJECT_LOCATIONCHANGE,
                nullptr,
                TaskbarEventProc,
                pid, tid,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );
        }
    }
    PostMessage(hwnd, WM_APP + 10, 0, 0);
}

// --- Window Procedure ---
#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define APP_WM_CLOSE   (WM_APP + 1)

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            g_TaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
            UpdateAppearance(hwnd);
            SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL);
            RegisterTaskbarHook(hwnd);
            return 0;

        case WM_ERASEBKGND: 
            return 1;

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_TaskbarHook) {
                UnhookWinEvent(g_TaskbarHook);
                g_TaskbarHook = nullptr;
            }
            {
                lock_guard<mutex> slock(g_SessionMutex);
                g_ActiveSession = nullptr;
                g_SessionManager = nullptr;
            }
            {
                lock_guard<mutex> lk(g_CoverCacheMutex);
                for (auto& e : g_CoverCache) delete e.bmp;
                g_CoverCache.clear();
            }
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                if (!g_MediaUpdatePending.exchange(true)) {
                    std::thread([hwnd] {
                        winrt::init_apartment();
                        UpdateMediaInfo();
                        g_MediaUpdatePending = false;
                        winrt::uninit_apartment();
                        PostMessage(hwnd, WM_APP + 11, 0, 0);
                    }).detach();
                }
                
                bool shouldHide = false;

                // 1. Check Fullscreen
                if (g_Settings.hideFullscreen) {
                    QUERY_USER_NOTIFICATION_STATE state;
                    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE) {
                            shouldHide = true;
                        }
                    }
                }

                // 2. Check Idle Timeout
                bool isPlaying = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    isPlaying = g_MediaState.isPlaying;
                }

                if (g_Settings.idleTimeout > 0) {
                    if (isPlaying) {
                        g_IdleSecondsCounter = 0;
                        g_IsHiddenByIdle = false;
                    } else {
                        g_IdleSecondsCounter++;
                        if (g_IdleSecondsCounter >= g_Settings.idleTimeout) {
                            g_IsHiddenByIdle = true;
                        }
                    }
                } else {
                    g_IsHiddenByIdle = false;
                }

                if (g_IsHiddenByIdle) shouldHide = true;

                // 3. Final Visibility Check
                if (shouldHide && IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_HIDE);
                } else if (!shouldHide && !IsWindowVisible(hwnd)) {
                    // Only restore if Taskbar is also visible
                    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                    if (hTaskbar && IsWindowVisible(hTaskbar)) {
                        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    }
                }
                
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if (wParam == IDT_ANIMATION) {
                if (g_IsScrolling) {
                    if (g_ScrollWait > 0) {
                        g_ScrollWait--;
                    } else {
                        g_ScrollOffset++;
                        if (g_ScrollOffset > g_TextWidth + 40) {
                            g_ScrollOffset = 0;
                            g_ScrollWait = 60; 
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                } else {
                    KillTimer(hwnd, IDT_ANIMATION); 
                }
            }
            return 0;

        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar) break;

            // Merged Logic: Check visibility first
            if (!IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                return 0;
            }

            // Restore visibility if we aren't hidden by fullscreen or Idle modes
            // (The Timer loop handles fullscreen/idle hiding, this handles Taskbar hiding)
            if (!g_IsHiddenByIdle && !IsWindowVisible(hwnd)) {
                // Double check fullscreen mode isn't forcing hide
                bool gameModeHide = false;
                if (g_Settings.hideFullscreen) {
                     QUERY_USER_NOTIFICATION_STATE state;
                     if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE) gameModeHide = true;
                     }
                }
                if (!gameModeHide) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }

            RECT rc;
            GetWindowRect(hTaskbar, &rc);

            int x = rc.left + g_Settings.offsetX;
            int taskbarHeight = rc.bottom - rc.top;
            int y = rc.top + (taskbarHeight / 2) -
            (g_Settings.height / 2) + g_Settings.offsetY;
            
            RECT myRc; GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y || 
                (myRc.right - myRc.left) != g_Settings.width || 
                (myRc.bottom - myRc.top) != g_Settings.height) {
                    SetWindowPos(
                        hwnd,
                        HWND_TOPMOST,
                        x, y,
                        g_Settings.width,
                        g_Settings.height,
                        SWP_NOACTIVATE
                    );
            }
            return 0;
        }

        case WM_APP + 11:
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int artSize = g_Settings.height - 12;
            double scale = g_Settings.buttonScale;
            
            // Re-calculate hit targets based on scale
            int startControlX = 6 + artSize + (int)(12 * scale);
            float gap = 28.0f * (float)scale;
            float pX = (float)startControlX;
            float plX = pX + gap;
            float nX = plX + gap;
            float radius = 12.0f * (float)scale;

            int newState = 0;
            if (y > 10 && y < g_Settings.height - 10) {
                if (x >= pX - radius && x <= pX + radius) newState = 1;
                else if (x >= plX - radius && x <= plX + radius) newState = 2;
                else if (x >= nX - radius && x <= nX + radius) newState = 3;
            }
            
            if (newState != g_HoverState) {
                g_HoverState = newState;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_HoverState = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case WM_LBUTTONUP:
            if (g_HoverState > 0) SendMediaCommand(g_HoverState);
            return 0;

        case WM_RBUTTONUP: {
            bool libby = false;
            wstring curTitle;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                libby    = g_MediaState.isLibby;
                curTitle = g_MediaState.title;
            }
            if (!libby) break;

            int rx = GET_X_LPARAM(lParam);
            int ry = GET_Y_LPARAM(lParam);
            int artSize = g_Settings.height - 12;
            if (rx < 6 || rx > 6 + artSize || ry < 6 || ry > 6 + artSize) break;

            bool hasCover = false, suppressed = false, locked = false;
            {
                lock_guard<mutex> lk(g_CoverCacheMutex);
                for (auto& e : g_CoverCache) {
                    if (e.title != curTitle) continue;
                    hasCover   = (e.bmp != nullptr);
                    suppressed = e.suppressed;
                    locked     = e.locked;
                    break;
                }
            }

            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING | ((hasCover && !locked) ? MF_ENABLED : MF_GRAYED),
                        IDM_COVER_WRONG,  L"Try Different Cover");
            AppendMenuW(hMenu, MF_STRING | ((!suppressed && !locked) ? MF_ENABLED : MF_GRAYED),
                        IDM_COVER_REMOVE, L"Remove Cover");
            AppendMenuW(hMenu, MF_STRING | (hasCover || locked ? MF_ENABLED : MF_GRAYED),
                        IDM_COVER_LOCK,
                        locked ? L"\U0001F512 Unlock Cover" : L"\U0001F513 Lock Cover");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING | (suppressed ? MF_ENABLED : MF_GRAYED),
                        IDM_COVER_RESET,  L"Restore Cover");

            POINT pt; GetCursorPos(&pt);
            int cmd = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                     pt.x, pt.y, 0, hwnd, nullptr);
            DestroyMenu(hMenu);
            if      (cmd == IDM_COVER_WRONG)  HandleCoverWrong(hwnd, curTitle);
            else if (cmd == IDM_COVER_REMOVE) HandleCoverRemove(hwnd, curTitle);
            else if (cmd == IDM_COVER_LOCK)   HandleCoverLock(hwnd, curTitle);
            else if (cmd == IDM_COVER_RESET)  HandleCoverReset(hwnd, curTitle);
            return 0;
        }

        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            BYTE vk = zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN;
            INPUT inputs[2] = {};
            inputs[0].type       = INPUT_KEYBOARD;
            inputs[0].ki.wVk     = vk;
            inputs[1].type       = INPUT_KEYBOARD;
            inputs[1].ki.wVk     = vk;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
            
            DrawMediaPanel(memDC, rc.right, rc.bottom);
            
            if (g_IsScrolling) SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
            else KillTimer(hwnd, IDT_ANIMATION);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap); DeleteObject(memBitmap); DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            if (msg == g_TaskbarCreatedMsg) {
                if (g_TaskbarHook) {
                    UnhookWinEvent(g_TaskbarHook);
                    g_TaskbarHook = nullptr;
                }
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// --- Main Thread ---
void MediaThread() {
    winrt::init_apartment();

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = MediaWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("WindhawkMusicLounge_GSMTC");
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);

    // Try to use CreateWindowInBand
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hUser32) {
        CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand");
    }

    if (CreateWindowInBand) {
        g_hMediaWindow = CreateWindowInBand(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL,
            ZBID_IMMERSIVE_NOTIFICATION
        );
        if (g_hMediaWindow) {
            Wh_Log(L"Created window in ZBID_IMMERSIVE_NOTIFICATION band");
        }
    }

    if (!g_hMediaWindow) {
        Wh_Log(L"Falling back to CreateWindowEx");
        g_hMediaWindow = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL
        );
    }

    if (!g_hMediaWindow) {
        Wh_Log(L"Failed to create media window — aborting media thread");
        GdiplusShutdown(gdiplusToken);
        winrt::uninit_apartment();
        return;
    }

    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);

    g_hWinHttpSession = WinHttpOpen(L"TaskbarMusicLoungeAE/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
    winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

// --- CALLBACKS ---
BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-music-lounge");
    LoadSettings(); 
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
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
         SendMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
         SendMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0); 
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
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
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
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

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
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

