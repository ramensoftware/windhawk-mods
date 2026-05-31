// ==WindhawkMod==
// @id              taskbar-music-deck
// @name            Taskbar Music Deck
// @description     Event-driven taskbar music panel with GSMTC controls and per-app volume.
// @version         1.0.0
// @author          dav2010ID
// @github          https://github.com/dav2010ID
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Deck

A compact Windows 11 taskbar media panel with album art, playback controls, and app-specific volume control.

This fork uses the unique mod id `taskbar-music-deck` to avoid conflicts with other Taskbar Music Lounge variants.

## Features

* **Event-driven media updates:** Uses Windows GSMTC events instead of constant polling.
* **Media display:** Shows title, artist, and rounded album art.
* **Playback controls:** Previous, play/pause, and next buttons.
* **Safe controls:** Unsupported buttons are disabled based on GSMTC playback capabilities.
* **Per-app volume:** Mouse wheel changes the active media app volume through the Windows audio mixer instead of changing the whole PC volume.
* **Smart visibility:** Optional hiding for fullscreen/presentation mode, paused idle timeout, and taskbar auto-hide.
* **Native Windows 11 look:** Acrylic background, rounded corners, DPI-aware positioning, and automatic light/dark text color.
* **Long title support:** Smooth horizontal ticker for track names that do not fit.

## Controls

* Left click previous/play/pause/next to control the current media session.
* Scroll over the panel to change the current media app volume.

For browser playback, volume control usually affects the browser process, such as Chrome, Edge, or Firefox. It does not control a single browser tab.

## Requirements

* Windows 11.
* A media player or browser that exposes metadata through Windows Global System Media Transport Controls.
* For best layout results, disable the default Widgets taskbar button if it overlaps the panel.

## Credits

* Hashah2311 - original Taskbar Music Lounge concept and base work.
* dav2010ID - fork maintenance, event-driven media handling, bug fixes, and per-app volume control.
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
- ButtonScale: 1.0
  $name: Button Scale (1.0 = Normal, 2.0 = 4K)
- HideFullscreen: false
  $name: Hide when Fullscreen
- IdleTimeout: 0
  $name: Auto-hide when paused (Seconds). Set 0 to disable.
- OffsetX: 2
  $name: X Offset
- OffsetY: 2
  $name: Y Offset
- AutoTheme: true
  $name: Auto Theme
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
- BgOpacity: 0
  $name: Acrylic Tint Opacity (0-255). Keep 0 for pure glass.
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000000
#endif

#include <windows.h>
#include <windowsx.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <audioclient.h>
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
#include <cstdio>
#include <algorithm>
#include <memory>
#include <new>
#include <cassert>
#include <cwctype>

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

extern "C" HRESULT WINAPI CreateStreamOverRandomAccessStream(IUnknown* randomAccessStream, REFIID riid, void** ppv);

// --- Constants ---
const WCHAR* FONT_NAME = L"Segoe UI Variable Display"; 
const WCHAR* NO_MEDIA_TITLE = L"No Media";

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
typedef UINT(WINAPI* pGetDpiForWindow)(HWND);

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

mutex g_SettingsLock;

ModSettings GetSettingsSnapshot() {
    lock_guard<mutex> guard(g_SettingsLock);
    return g_Settings;
}

// --- Global State ---
atomic<HWND> g_hMediaWindow{NULL};
atomic<bool> g_Running{false};
atomic<DWORD> g_MediaThreadId{0};
atomic<DWORD> g_MediaEventsThreadId{0};
atomic<DWORD> g_CachedTextColor{0xFFFFFFFF};
HANDLE g_MediaRefreshEvent = nullptr;
HANDLE g_MediaStopEvent = nullptr;
HANDLE g_MediaCommandVerifyTimer = nullptr;
atomic<bool> g_MediaUiRefreshPending{false};
int g_HoverState = 0; 
HWINEVENTHOOK g_TaskbarHook = nullptr; 
UINT g_TaskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
ULONG_PTR g_GdiplusToken = 0;
bool g_GdiplusStarted = false;
FontFamily* g_FontFamily = nullptr;

// Idle Tracking
atomic<int> g_IdleSecondsCounter{0};
atomic<bool> g_IsHiddenByIdle{false};

// Data Model
struct MediaState {
    wstring title = NO_MEDIA_TITLE;
    wstring artist = L"";
    bool isPlaying = false;
    bool hasMedia = false;
    bool canPrevious = false;
    bool canNext = false;
    bool canTogglePlayPause = false;
    wstring sourceAppUserModelId;
    shared_ptr<Bitmap> albumArt;
    uint64_t revision = 0;
    mutex lock;
} g_MediaState;

// Animation
int g_ScrollOffset = 0;
int g_TextWidth = 0;
bool g_IsScrolling = false;
int g_ScrollWait = 60;
bool g_AnimationTimerActive = false;
uint64_t g_LastSeenMediaRevision = 0;

#define IDT_VISIBILITY      1001
#define IDT_ANIMATION       1002
#define APP_WM_CLOSE        (WM_APP + 100)
#define APP_WM_REPOSITION   (WM_APP + 101)
#define APP_WM_MEDIA_REFRESH (WM_APP + 102)
#define APP_WM_MEDIA_COMMAND (WM_APP + 103)
#define APP_WM_VISIBILITY_REFRESH (WM_APP + 104)

void ResetScrollState() {
    g_ScrollOffset = 0;
    g_TextWidth = 0;
    g_IsScrolling = false;
    g_ScrollWait = 60;
}

void SetAnimationTimerEnabled(HWND hwnd, bool enabled) {
    if (!hwnd) {
        return;
    }

    DWORD mediaThreadId = g_MediaThreadId.load();
    assert(mediaThreadId == 0 || GetCurrentThreadId() == mediaThreadId);

    if (enabled) {
        if (!g_AnimationTimerActive) {
            SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
            g_AnimationTimerActive = true;
        }
    } else if (g_AnimationTimerActive) {
        KillTimer(hwnd, IDT_ANIMATION);
        g_AnimationTimerActive = false;
    }
}

// --- Settings ---
bool IsSystemLightMode();
void RefreshCachedTextColor(const ModSettings& settings);

void LoadSettings() {
    ModSettings settings;
    settings.width = Wh_GetIntSetting(L"PanelWidth");
    settings.height = Wh_GetIntSetting(L"PanelHeight");
    settings.fontSize = Wh_GetIntSetting(L"FontSize");
    settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;
    
    PCWSTR scaleStr = Wh_GetStringSetting(L"ButtonScale");
    if (scaleStr) {
        settings.buttonScale = scaleStr[0] ? _wtof(scaleStr) : 1.0;
        Wh_FreeStringSetting(scaleStr);
    } else {
        settings.buttonScale = 1.0;
    }
    if (settings.buttonScale < 0.5) settings.buttonScale = 0.5;
    if (settings.buttonScale > 4.0) settings.buttonScale = 4.0;

    settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;
    settings.idleTimeout = Wh_GetIntSetting(L"IdleTimeout");
    if (settings.idleTimeout < 0) settings.idleTimeout = 0;

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    settings.manualTextColor = 0xFF000000 | textRGB;
    
    settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    if (settings.bgOpacity < 0) settings.bgOpacity = 0;
    if (settings.bgOpacity > 255) settings.bgOpacity = 255;

    if (settings.width < 100) settings.width = 300;
    if (settings.height < 24) settings.height = 48;
    if (settings.fontSize < 6) settings.fontSize = 11;
    if (settings.fontSize > settings.height - 4) settings.fontSize = max(6, settings.height - 4);

    {
        lock_guard<mutex> guard(g_SettingsLock);
        g_Settings = settings;
    }

    RefreshCachedTextColor(settings);
}

void RefreshCachedTextColor(const ModSettings& settings) {
    DWORD color = settings.autoTheme ? (IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF) : settings.manualTextColor;
    g_CachedTextColor.store(color);
}

// --- WinRT / GSMTC ---
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

struct PlaybackCapabilities {
    bool canPrevious = false;
    bool canNext = false;
    bool canTogglePlayPause = false;
};

PlaybackCapabilities GetPlaybackCapabilities(GlobalSystemMediaTransportControlsSessionPlaybackInfo const& info) {
    PlaybackCapabilities capabilities;
    if (!info) {
        return capabilities;
    }

    try {
        auto controls = info.Controls();
        if (controls) {
            capabilities.canPrevious = controls.IsPreviousEnabled();
            capabilities.canNext = controls.IsNextEnabled();
            capabilities.canTogglePlayPause =
                controls.IsPlayPauseToggleEnabled() ||
                controls.IsPlayEnabled() ||
                controls.IsPauseEnabled();
        }
    } catch (...) {}

    return capabilities;
}

bool IsMediaStateClearedLocked() {
    return !g_MediaState.hasMedia &&
        !g_MediaState.isPlaying &&
        !g_MediaState.canPrevious &&
        !g_MediaState.canNext &&
        !g_MediaState.canTogglePlayPause &&
        g_MediaState.title == NO_MEDIA_TITLE &&
        g_MediaState.artist.empty() &&
        g_MediaState.sourceAppUserModelId.empty() &&
        g_MediaState.albumArt == nullptr;
}

void ClearMediaStateLocked(bool incrementRevision) {
    bool changed = !IsMediaStateClearedLocked();

    g_MediaState.hasMedia = false;
    g_MediaState.isPlaying = false;
    g_MediaState.canPrevious = false;
    g_MediaState.canNext = false;
    g_MediaState.canTogglePlayPause = false;
    g_MediaState.title = NO_MEDIA_TITLE;
    g_MediaState.artist.clear();
    g_MediaState.sourceAppUserModelId.clear();
    g_MediaState.albumArt.reset();

    if (incrementRevision && changed) {
        g_MediaState.revision++;
    }
}

void ClearMediaState(bool incrementRevision) {
    lock_guard<mutex> guard(g_MediaState.lock);
    ClearMediaStateLocked(incrementRevision);
}

void UpdateMediaInfo(GlobalSystemMediaTransportControlsSessionManager sessionManager, bool fullUpdate) {
    try {
        if (!sessionManager) return;

        // Iterate ALL sessions to find one that is actively PLAYING.
        GlobalSystemMediaTransportControlsSession session = nullptr;
        bool foundActive = false;

        auto sessionsList = sessionManager.GetSessions();
        for (auto const& s : sessionsList) {
            auto pb = s.GetPlaybackInfo();
            if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                session = s;
                foundActive = true;
                break;
            }
        }

        if (!foundActive) {
            session = sessionManager.GetCurrentSession();
        }

        if (session) {
            auto info = session.GetPlaybackInfo();
            if (!info) {
                ClearMediaState(true);
                return;
            }

            wstring newSourceAppUserModelId = session.SourceAppUserModelId().c_str();
            bool newIsPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            PlaybackCapabilities capabilities = GetPlaybackCapabilities(info);
            bool needsFullUpdate = fullUpdate;

            {
                lock_guard<mutex> guard(g_MediaState.lock);
                needsFullUpdate = needsFullUpdate ||
                    !g_MediaState.hasMedia ||
                    newSourceAppUserModelId != g_MediaState.sourceAppUserModelId;

                if (!needsFullUpdate) {
                    g_MediaState.isPlaying = newIsPlaying;
                    g_MediaState.canPrevious = capabilities.canPrevious;
                    g_MediaState.canNext = capabilities.canNext;
                    g_MediaState.canTogglePlayPause = capabilities.canTogglePlayPause;
                    return;
                }
            }

            auto props = session.TryGetMediaPropertiesAsync().get();
            wstring newTitle = props.Title().c_str();
            wstring newArtist = props.Artist().c_str();
            bool refreshAlbumArt = false;

            {
                lock_guard<mutex> guard(g_MediaState.lock);
                refreshAlbumArt = newTitle != g_MediaState.title ||
                    newSourceAppUserModelId != g_MediaState.sourceAppUserModelId ||
                    g_MediaState.albumArt == nullptr;
            }

            shared_ptr<Bitmap> newAlbumArt;
            if (refreshAlbumArt) {
                auto thumbRef = props.Thumbnail();
                if (thumbRef) {
                    auto stream = thumbRef.OpenReadAsync().get();
                    newAlbumArt.reset(StreamToBitmap(stream));
                }
            }

            lock_guard<mutex> guard(g_MediaState.lock);
            bool mediaIdentityChanged =
                !g_MediaState.hasMedia ||
                newTitle != g_MediaState.title ||
                newArtist != g_MediaState.artist ||
                newSourceAppUserModelId != g_MediaState.sourceAppUserModelId;

            if (refreshAlbumArt) {
                g_MediaState.albumArt = newAlbumArt;
            }
            g_MediaState.title = newTitle;
            g_MediaState.artist = newArtist;
            g_MediaState.isPlaying = newIsPlaying;
            g_MediaState.hasMedia = true;
            g_MediaState.canPrevious = capabilities.canPrevious;
            g_MediaState.canNext = capabilities.canNext;
            g_MediaState.canTogglePlayPause = capabilities.canTogglePlayPause;
            g_MediaState.sourceAppUserModelId = newSourceAppUserModelId;
            if (mediaIdentityChanged) {
                g_MediaState.revision++;
            }
        } else {
            ClearMediaState(true);
        }
    } catch (...) {
        ClearMediaState(true);
    }
}

void PostMediaUiRefresh() {
    HWND hwnd = g_hMediaWindow.load();
    if (hwnd && !g_MediaUiRefreshPending.exchange(true)) {
        if (!PostMessageW(hwnd, APP_WM_MEDIA_REFRESH, 0, 0)) {
            g_MediaUiRefreshPending.store(false);
        }
    }
}

atomic<bool> g_NeedFullMediaRefresh{true};
atomic<bool> g_NeedRebuildMediaSessions{true};

void RequestMediaRefresh(bool fullUpdate = true, bool rebuildSessions = false) {
    if (!g_Running.load()) {
        return;
    }

    if (fullUpdate) {
        g_NeedFullMediaRefresh.store(true);
    }
    if (rebuildSessions) {
        g_NeedRebuildMediaSessions.store(true);
    }
    if (g_MediaRefreshEvent) {
        SetEvent(g_MediaRefreshEvent);
    }
    PostMediaUiRefresh();
}

void SendMediaCommand(int cmd) {
    bool canSend = false;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (!g_MediaState.hasMedia) {
            return;
        }

        if (cmd == 1) {
            canSend = g_MediaState.canPrevious;
        } else if (cmd == 2) {
            canSend = g_MediaState.canTogglePlayPause;
        } else if (cmd == 3) {
            canSend = g_MediaState.canNext;
        }

        if (!canSend) {
            return;
        }

        if (cmd == 2) {
            g_MediaState.isPlaying = !g_MediaState.isPlaying;
        }
    }

    PostMediaUiRefresh();

    DWORD mediaEventsThreadId = g_MediaEventsThreadId.load();
    if (mediaEventsThreadId) {
        if (!PostThreadMessageW(mediaEventsThreadId, APP_WM_MEDIA_COMMAND, (WPARAM)cmd, 0)) {
            RequestMediaRefresh(false);
        }
    } else {
        RequestMediaRefresh(false);
    }
}

wstring ToLowerCopy(wstring value) {
    transform(value.begin(), value.end(), value.begin(),
        [](wchar_t ch) { return (wchar_t)towlower(ch); });
    return value;
}

wstring FileNameFromPath(const wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == wstring::npos ? path : path.substr(pos + 1);
}

wstring ProcessImageNameFromPid(DWORD pid) {
    if (pid == 0) {
        return L"";
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }

    WCHAR path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    wstring imageName;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        imageName = FileNameFromPath(path);
    }

    CloseHandle(process);
    return imageName;
}

wstring TakeCoTaskMemString(LPWSTR value) {
    wstring result;
    if (value) {
        result = value;
        CoTaskMemFree(value);
    }
    return result;
}

bool SourceLooksLikeProcess(const wstring& source, const wstring& processName) {
    if (source.empty() || processName.empty()) {
        return false;
    }

    wstring sourceLower = ToLowerCopy(source);
    wstring processLower = ToLowerCopy(processName);
    wstring processBase = processLower;
    if (processBase.size() > 4 && processBase.substr(processBase.size() - 4) == L".exe") {
        processBase.resize(processBase.size() - 4);
    }

    if (sourceLower.find(processLower) != wstring::npos ||
        sourceLower.find(processBase) != wstring::npos ||
        processLower.find(sourceLower) != wstring::npos) {
        return true;
    }

    struct KnownSourceProcess {
        const wchar_t* sourceNeedle;
        const wchar_t* processExe;
    };

    static const KnownSourceProcess knownSources[] = {
        { L"chrome", L"chrome.exe" },
        { L"msedge", L"msedge.exe" },
        { L"edge", L"msedge.exe" },
        { L"firefox", L"firefox.exe" },
        { L"spotify", L"spotify.exe" },
        { L"vlc", L"vlc.exe" },
        { L"foobar", L"foobar2000.exe" },
        { L"musicbee", L"musicbee.exe" },
        { L"aimp", L"aimp.exe" },
        { L"winamp", L"winamp.exe" }
    };

    for (const auto& known : knownSources) {
        if (sourceLower.find(known.sourceNeedle) != wstring::npos &&
            processLower == known.processExe) {
            return true;
        }
    }

    return false;
}

bool AudioSessionMatchesSource(
    IAudioSessionControl2* sessionControl2,
    const wstring& processName,
    const wstring& sourceAppUserModelId
) {
    if (!sessionControl2 || sourceAppUserModelId.empty()) {
        return false;
    }

    if (SourceLooksLikeProcess(sourceAppUserModelId, processName)) {
        return true;
    }

    LPWSTR displayNameRaw = nullptr;
    LPWSTR sessionIdentifierRaw = nullptr;
    wstring displayName;
    wstring sessionIdentifier;

    if (SUCCEEDED(sessionControl2->GetDisplayName(&displayNameRaw))) {
        displayName = TakeCoTaskMemString(displayNameRaw);
    }
    if (SUCCEEDED(sessionControl2->GetSessionIdentifier(&sessionIdentifierRaw))) {
        sessionIdentifier = TakeCoTaskMemString(sessionIdentifierRaw);
    }

    wstring sourceLower = ToLowerCopy(sourceAppUserModelId);
    wstring sessionText = ToLowerCopy(processName + L" " + displayName + L" " + sessionIdentifier);
    return !sourceLower.empty() && sessionText.find(sourceLower) != wstring::npos;
}

bool ChangeActiveMediaSessionVolume(short wheelDelta) {
    wstring sourceAppUserModelId;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (!g_MediaState.hasMedia) {
            return false;
        }
        sourceAppUserModelId = g_MediaState.sourceAppUserModelId;
    }

    if (sourceAppUserModelId.empty()) {
        return false;
    }

    try {
        winrt::com_ptr<IMMDeviceEnumerator> deviceEnumerator;
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            deviceEnumerator.put_void()
        );
        if (FAILED(hr) || !deviceEnumerator) {
            return false;
        }

        winrt::com_ptr<IMMDevice> endpoint;
        hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, endpoint.put());
        if (FAILED(hr) || !endpoint) {
            return false;
        }

        winrt::com_ptr<IAudioSessionManager2> sessionManager;
        hr = endpoint->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, sessionManager.put_void());
        if (FAILED(hr) || !sessionManager) {
            return false;
        }

        winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
        hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
        if (FAILED(hr) || !sessionEnumerator) {
            return false;
        }

        int sessionCount = 0;
        if (FAILED(sessionEnumerator->GetCount(&sessionCount))) {
            return false;
        }

        float step = 0.05f * ((float)wheelDelta / (float)WHEEL_DELTA);
        bool changedAny = false;

        for (int i = 0; i < sessionCount; i++) {
            winrt::com_ptr<IAudioSessionControl> sessionControl;
            if (FAILED(sessionEnumerator->GetSession(i, sessionControl.put())) || !sessionControl) {
                continue;
            }

            winrt::com_ptr<IAudioSessionControl2> sessionControl2;
            if (FAILED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), sessionControl2.put_void())) || !sessionControl2) {
                continue;
            }

            if (sessionControl2->IsSystemSoundsSession() == S_OK) {
                continue;
            }

            DWORD pid = 0;
            if (FAILED(sessionControl2->GetProcessId(&pid)) || pid == 0) {
                continue;
            }

            wstring processName = ProcessImageNameFromPid(pid);
            if (!AudioSessionMatchesSource(sessionControl2.get(), processName, sourceAppUserModelId)) {
                continue;
            }

            winrt::com_ptr<ISimpleAudioVolume> simpleVolume;
            if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), simpleVolume.put_void())) || !simpleVolume) {
                continue;
            }

            float currentVolume = 0.0f;
            if (FAILED(simpleVolume->GetMasterVolume(&currentVolume))) {
                continue;
            }

            float newVolume = min(1.0f, max(0.0f, currentVolume + step));
            if (SUCCEEDED(simpleVolume->SetMasterVolume(newVolume, nullptr))) {
                if (newVolume > 0.0f) {
                    simpleVolume->SetMute(FALSE, nullptr);
                }
                changedAny = true;
            }
        }

        return changedAny;
    } catch (...) {
        return false;
    }
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
    return g_CachedTextColor.load();
}

UINT GetPanelDpi(HWND hwnd) {
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        auto GetDpiForWindowFn = (pGetDpiForWindow)GetProcAddress(hUser, "GetDpiForWindow");
        if (GetDpiForWindowFn) {
            UINT dpi = GetDpiForWindowFn(hwnd);
            if (dpi != 0) {
                return dpi;
            }
        }
    }

    return 96;
}

int ScaleForDpi(HWND hwnd, int value) {
    return MulDiv(value, (int)GetPanelDpi(hwnd), 96);
}

void UpdateAppearance(HWND hwnd, const ModSettings& settings) {
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            DWORD tint = 0; 
            if (settings.autoTheme) {
                tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
            } else {
                tint = ((DWORD)settings.bgOpacity << 24) | (settings.manualTextColor & 0x00FFFFFF);
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

class PaintBuffer {
public:
    PaintBuffer(HDC targetDc, int width, int height) {
        memDC = CreateCompatibleDC(targetDc);
        memBitmap = memDC ? CreateCompatibleBitmap(targetDc, width, height) : NULL;
        oldBitmap = memBitmap ? (HBITMAP)SelectObject(memDC, memBitmap) : NULL;
    }

    ~PaintBuffer() {
        if (oldBitmap && memDC) SelectObject(memDC, oldBitmap);
        if (memBitmap) DeleteObject(memBitmap);
        if (memDC) DeleteDC(memDC);
    }

    HDC Target(HDC fallbackDc) const {
        return Ready() ? memDC : fallbackDc;
    }

    bool Ready() const {
        return memDC && memBitmap;
    }

private:
    HDC memDC = NULL;
    HBITMAP memBitmap = NULL;
    HBITMAP oldBitmap = NULL;
};

void DrawMediaPanel(HDC hdc, int width, int height, const ModSettings& settings) {
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
        state.albumArt = g_MediaState.albumArt;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
        state.canPrevious = g_MediaState.canPrevious;
        state.canNext = g_MediaState.canNext;
        state.canTogglePlayPause = g_MediaState.canTogglePlayPause;
    }
    bool showPauseIcon = state.hasMedia && state.isPlaying;
    bool previousEnabled = state.hasMedia && state.canPrevious;
    bool playPauseEnabled = state.hasMedia && state.canTogglePlayPause;
    bool nextEnabled = state.hasMedia && state.canNext;

    // 1. Album Art (Rounded)
    int artSize = height - 12;
    int artX = 6;
    int artY = 6;
    
    GraphicsPath path;
    AddRoundedRect(path, artX, artY, artSize, artSize, 8); 

    if (state.albumArt) {
        graphics.SetClip(&path);
        graphics.DrawImage(state.albumArt.get(), artX, artY, artSize, artSize);
        graphics.ResetClip();
    } else {
        SolidBrush placeBrush{Color(40, 128, 128, 128)};
        graphics.FillPath(&placeBrush, &path);
    }

    // 2. Controls (Scaled)
    double scale = settings.buttonScale;
    int startControlX = artX + artSize + (int)(12 * scale);
    int controlY = height / 2;

    SolidBrush iconBrush{mainColor};
    SolidBrush hoverBrush{Color(255, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
    SolidBrush activeBg{Color(40, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
    SolidBrush disabledBrush{Color(80, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};

    float circleR = 12.0f * (float)scale; 
    float iconW = 8.0f * (float)scale;
    float iconH = 12.0f * (float)scale; 
    float gap = 28.0f * (float)scale;
    
    // Prev
    float pX = (float)startControlX;
    Brush* prevBrush = previousEnabled ? (g_HoverState == 1 ? &hoverBrush : &iconBrush) : &disabledBrush;
    if (previousEnabled && g_HoverState == 1) graphics.FillEllipse(&activeBg, pX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF prevPts[3] = { PointF(pX + iconW, (float)controlY - (iconH/2)), PointF(pX + iconW, (float)controlY + (iconH/2)), PointF(pX, (float)controlY) };
    graphics.FillPolygon(prevBrush, prevPts, 3);
    graphics.FillRectangle(prevBrush, pX, (float)controlY - (iconH/2), 2.0f * (float)scale, iconH);

    // Play/Pause
    float plX = pX + gap;
    Brush* playBrush = playPauseEnabled ? (g_HoverState == 2 ? &hoverBrush : &iconBrush) : &disabledBrush;
    if (playPauseEnabled && g_HoverState == 2) graphics.FillEllipse(&activeBg, plX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    if (showPauseIcon) {
        float barW = 3.0f * (float)scale;
        float barH = 14.0f * (float)scale;
        graphics.FillRectangle(playBrush, plX - (barW + 1), (float)controlY - (barH/2), barW, barH);
        graphics.FillRectangle(playBrush, plX + 1, (float)controlY - (barH/2), barW, barH);
    } else {
        float playW = 10.0f * (float)scale;
        float playH = 16.0f * (float)scale;
        PointF playPts[3] = { PointF(plX - (playW/2), (float)controlY - (playH/2)), PointF(plX - (playW/2), (float)controlY + (playH/2)), PointF(plX + (playW/2), (float)controlY) };
        graphics.FillPolygon(playBrush, playPts, 3);
    }

    // Next
    float nX = plX + gap;
    Brush* nextBrush = nextEnabled ? (g_HoverState == 3 ? &hoverBrush : &iconBrush) : &disabledBrush;
    if (nextEnabled && g_HoverState == 3) graphics.FillEllipse(&activeBg, nX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF nextPts[3] = { PointF(nX - iconW, (float)controlY - (iconH/2)), PointF(nX - iconW, (float)controlY + (iconH/2)), PointF(nX, (float)controlY) };
    graphics.FillPolygon(nextBrush, nextPts, 3);
    graphics.FillRectangle(nextBrush, nX, (float)controlY - (iconH/2), 2.0f * (float)scale, iconH);

    // 3. Text
    int textX = (int)(nX + (20 * scale));
    int textMaxW = width - textX - 10;
    
    wstring fullText = state.title;
    if (!state.artist.empty()) fullText += L" вЂў " + state.artist;

    if (!g_FontFamily) {
        return;
    }

    Font font(g_FontFamily, (REAL)settings.fontSize, FontStyleBold, UnitPixel);
    SolidBrush textBrush{mainColor};

    if (textMaxW <= 4) {
        ResetScrollState();
        return;
    }
    
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
        g_ScrollWait = 60;
        graphics.DrawString(fullText.c_str(), -1, &font, PointF((float)textX, textY), &textBrush);
    }
}

// --- Event Hook ---
bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd) return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

bool IsTaskbarAutoHideEnabled() {
    APPBARDATA appbarData{};
    appbarData.cbSize = sizeof(appbarData);
    return (SHAppBarMessage(ABM_GETSTATE, &appbarData) & ABS_AUTOHIDE) != 0;
}

bool IsTaskbarUsableForPanel(HWND hTaskbar) {
    if (!hTaskbar) return false;
    return IsWindowVisible(hTaskbar) || IsTaskbarAutoHideEnabled();
}

void RefreshVisibility(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    ModSettings settings = GetSettingsSnapshot();
    bool shouldHide = false;

    if (settings.hideFullscreen) {
        QUERY_USER_NOTIFICATION_STATE state;
        if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
            if (state == QUNS_BUSY ||
                state == QUNS_RUNNING_D3D_FULL_SCREEN ||
                state == QUNS_PRESENTATION_MODE) {
                shouldHide = true;
            }
        }
    }

    bool isPlaying = false;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        isPlaying = g_MediaState.isPlaying;
    }

    if (settings.idleTimeout > 0) {
        if (isPlaying) {
            g_IdleSecondsCounter.store(0);
            g_IsHiddenByIdle.store(false);
        } else {
            int idleSeconds = g_IdleSecondsCounter.fetch_add(1) + 1;
            if (idleSeconds >= settings.idleTimeout) {
                g_IsHiddenByIdle.store(true);
            }
        }
    } else {
        g_IdleSecondsCounter.store(0);
        g_IsHiddenByIdle.store(false);
    }

    if (g_IsHiddenByIdle.load()) {
        shouldHide = true;
    }

    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!IsTaskbarUsableForPanel(hTaskbar)) {
        shouldHide = true;
    }

    if (shouldHide && IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_HIDE);
    } else if (!shouldHide && !IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }

    InvalidateRect(hwnd, NULL, FALSE);
}

void CALLBACK TaskbarEventProc(
    HWINEVENTHOOK,
    DWORD,
    HWND hwnd,
    LONG, LONG,
    DWORD, DWORD
) {
    HWND mediaWindow = g_hMediaWindow.load();
    if (!IsTaskbarWindow(hwnd) || !mediaWindow) return;
    PostMessageW(mediaWindow, APP_WM_REPOSITION, 0, 0);
}

// Register Event Hook scoped to Taskbar Thread
void RegisterTaskbarHook(HWND hwnd)
{
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
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
    PostMessageW(hwnd, APP_WM_REPOSITION, 0, 0);
}

// --- Window Procedure ---
LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: 
            UpdateAppearance(hwnd, GetSettingsSnapshot()); 
            SetTimer(hwnd, IDT_VISIBILITY, 1000, NULL); 
            RegisterTaskbarHook(hwnd);
            return 0;

        case WM_ERASEBKGND: 
            return 1;

        case WM_CLOSE:
            // Ignore external close attempts; WhTool_ModUninit closes through APP_WM_CLOSE.
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            SetAnimationTimerEnabled(hwnd, false);
            KillTimer(hwnd, IDT_VISIBILITY);
            if (g_TaskbarHook) {
                UnhookWinEvent(g_TaskbarHook);
                g_TaskbarHook = nullptr;
            }
            g_MediaUiRefreshPending.store(false);
            g_hMediaWindow.store(nullptr);
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE: {
            ModSettings settings = GetSettingsSnapshot();
            RefreshCachedTextColor(settings);
            UpdateAppearance(hwnd, settings);
            ResetScrollState();
            SetAnimationTimerEnabled(hwnd, false);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_TIMER:
            if (wParam == IDT_VISIBILITY) {
                RefreshVisibility(hwnd);
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
                    SetAnimationTimerEnabled(hwnd, false);
                }
            }
            return 0;

        case APP_WM_VISIBILITY_REFRESH:
            RefreshVisibility(hwnd);
            return 0;

        case APP_WM_MEDIA_REFRESH:
            g_MediaUiRefreshPending.store(false);
            {
                uint64_t mediaRevision = 0;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    mediaRevision = g_MediaState.revision;
                }

                if (mediaRevision != g_LastSeenMediaRevision) {
                    ResetScrollState();
                    SetAnimationTimerEnabled(hwnd, false);
                    g_LastSeenMediaRevision = mediaRevision;
                }
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case APP_WM_REPOSITION: {
            ModSettings settings = GetSettingsSnapshot();
            HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (!hTaskbar) break;

            // Merged Logic: Check visibility first
            if (!IsTaskbarUsableForPanel(hTaskbar)) {
                if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                return 0;
            }

            // Restore visibility if we aren't hidden by fullscreen or Idle modes
            // (The Timer loop handles fullscreen/idle hiding, this handles Taskbar hiding)
            if (!g_IsHiddenByIdle.load() && !IsWindowVisible(hwnd)) {
                // Double check fullscreen mode isn't forcing hide
                bool gameModeHide = false;
                if (settings.hideFullscreen) {
                     QUERY_USER_NOTIFICATION_STATE state;
                     if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE) gameModeHide = true;
                     }
                }
                if (!gameModeHide) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }

            RECT rc;
            GetWindowRect(hTaskbar, &rc);

            int panelWidth = ScaleForDpi(hwnd, settings.width);
            int panelHeight = ScaleForDpi(hwnd, settings.height);
            int offsetX = ScaleForDpi(hwnd, settings.offsetX);
            int offsetY = ScaleForDpi(hwnd, settings.offsetY);

            int x = rc.left + offsetX;
            int taskbarHeight = rc.bottom - rc.top;
            int y = rc.top + (taskbarHeight / 2) -
            (panelHeight / 2) + offsetY;
            
            RECT myRc; GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y || 
                (myRc.right - myRc.left) != panelWidth || 
                (myRc.bottom - myRc.top) != panelHeight) {
                    SetWindowPos(
                        hwnd,
                        HWND_TOPMOST,
                        x, y,
                        panelWidth,
                        panelHeight,
                        SWP_NOACTIVATE
                    );
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            ModSettings settings = GetSettingsSnapshot();
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            RECT clientRc;
            GetClientRect(hwnd, &clientRc);
            int panelHeight = clientRc.bottom - clientRc.top;
            int artSize = panelHeight - 12;
            double scale = settings.buttonScale;
            
            // Re-calculate hit targets based on scale
            int startControlX = 6 + artSize + (int)(12 * scale);
            float gap = 28.0f * (float)scale;
            float pX = (float)startControlX;
            float plX = pX + gap;
            float nX = plX + gap;
            float radius = 12.0f * (float)scale;

            bool canPrevious = false;
            bool canPlayPause = false;
            bool canNext = false;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                canPrevious = g_MediaState.hasMedia && g_MediaState.canPrevious;
                canPlayPause = g_MediaState.hasMedia && g_MediaState.canTogglePlayPause;
                canNext = g_MediaState.hasMedia && g_MediaState.canNext;
            }

            int newState = 0;
            if (y > 10 && y < panelHeight - 10) {
                if (canPrevious && x >= pX - radius && x <= pX + radius) newState = 1;
                else if (canPlayPause && x >= plX - radius && x <= plX + radius) newState = 2;
                else if (canNext && x >= nX - radius && x <= nX + radius) newState = 3;
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
        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            ChangeActiveMediaSessionVolume(zDelta);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;

            if (width <= 0 || height <= 0) {
                EndPaint(hwnd, &ps);
                return 0;
            }

            {
                PaintBuffer buffer(hdc, width, height);
                try {
                    HDC paintDC = buffer.Target(hdc);
                    DrawMediaPanel(paintDC, width, height, GetSettingsSnapshot());

                    SetAnimationTimerEnabled(hwnd, g_IsScrolling);
                    if (buffer.Ready()) {
                        BitBlt(hdc, 0, 0, width, height, buffer.Target(hdc), 0, 0, SRCCOPY);
                    }
                } catch (...) {
                    ResetScrollState();
                    SetAnimationTimerEnabled(hwnd, false);
                }
            }

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
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// --- GSMTC Events Thread ---
struct MediaSessionSubscription {
    GlobalSystemMediaTransportControlsSession session = nullptr;
    event_token playbackInfoToken{};
    event_token mediaPropertiesToken{};
    bool hasPlaybackInfoToken = false;
    bool hasMediaPropertiesToken = false;
};

struct MediaEventsContext {
    GlobalSystemMediaTransportControlsSessionManager manager = nullptr;
    event_token currentSessionToken{};
    event_token sessionsToken{};
    bool hasCurrentSessionToken = false;
    bool hasSessionsToken = false;
    vector<MediaSessionSubscription> sessionSubscriptions;
};

void ClearMediaSessionSubscriptions(MediaEventsContext& context) {
    for (auto& subscription : context.sessionSubscriptions) {
        try {
            if (subscription.session) {
                if (subscription.hasPlaybackInfoToken) {
                    subscription.session.PlaybackInfoChanged(subscription.playbackInfoToken);
                }
                if (subscription.hasMediaPropertiesToken) {
                    subscription.session.MediaPropertiesChanged(subscription.mediaPropertiesToken);
                }
            }
        } catch (...) {}
    }

    context.sessionSubscriptions.clear();
}

void ShutdownMediaEvents(MediaEventsContext& context) {
    ClearMediaSessionSubscriptions(context);

    try {
        if (context.manager) {
            if (context.hasCurrentSessionToken) {
                context.manager.CurrentSessionChanged(context.currentSessionToken);
                context.hasCurrentSessionToken = false;
            }
            if (context.hasSessionsToken) {
                context.manager.SessionsChanged(context.sessionsToken);
                context.hasSessionsToken = false;
            }
        }
    } catch (...) {}

    context.manager = nullptr;
}

bool EnsureMediaEventsInitialized(MediaEventsContext& context) {
    if (context.manager) {
        return true;
    }

    try {
        context.manager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!context.manager) {
            return false;
        }

        context.currentSessionToken = context.manager.CurrentSessionChanged(
            [](GlobalSystemMediaTransportControlsSessionManager const&, CurrentSessionChangedEventArgs const&) {
                RequestMediaRefresh(true);
            });
        context.hasCurrentSessionToken = true;

        context.sessionsToken = context.manager.SessionsChanged(
            [](GlobalSystemMediaTransportControlsSessionManager const&, SessionsChangedEventArgs const&) {
                RequestMediaRefresh(true, true);
            });
        context.hasSessionsToken = true;

        g_NeedRebuildMediaSessions.store(true);
        g_NeedFullMediaRefresh.store(true);
        return true;
    } catch (...) {
        ShutdownMediaEvents(context);
        return false;
    }
}

void RebuildMediaSessionSubscriptions(MediaEventsContext& context) {
    ClearMediaSessionSubscriptions(context);
    if (!context.manager) {
        return;
    }

    try {
        for (auto const& session : context.manager.GetSessions()) {
            MediaSessionSubscription subscription;
            subscription.session = session;

            subscription.playbackInfoToken = session.PlaybackInfoChanged(
                [](GlobalSystemMediaTransportControlsSession const&, PlaybackInfoChangedEventArgs const&) {
                    RequestMediaRefresh(false);
                });
            subscription.hasPlaybackInfoToken = true;

            subscription.mediaPropertiesToken = session.MediaPropertiesChanged(
                [](GlobalSystemMediaTransportControlsSession const&, MediaPropertiesChangedEventArgs const&) {
                    RequestMediaRefresh(true);
                });
            subscription.hasMediaPropertiesToken = true;

            context.sessionSubscriptions.push_back(subscription);
        }
    } catch (...) {
        ClearMediaSessionSubscriptions(context);
    }
}

GlobalSystemMediaTransportControlsSession PickBestSession(GlobalSystemMediaTransportControlsSessionManager const& manager) {
    if (!manager) {
        return nullptr;
    }

    try {
        for (auto const& session : manager.GetSessions()) {
            auto playbackInfo = session.GetPlaybackInfo();
            if (playbackInfo &&
                playbackInfo.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                return session;
            }
        }

        return manager.GetCurrentSession();
    } catch (...) {
        return nullptr;
    }
}

bool IsMediaCommandEnabled(GlobalSystemMediaTransportControlsSession const& session, int cmd) {
    if (!session) {
        return false;
    }

    try {
        auto playbackInfo = session.GetPlaybackInfo();
        PlaybackCapabilities capabilities = GetPlaybackCapabilities(playbackInfo);

        if (cmd == 1) {
            return capabilities.canPrevious;
        }
        if (cmd == 2) {
            return capabilities.canTogglePlayPause;
        }
        if (cmd == 3) {
            return capabilities.canNext;
        }
    } catch (...) {}

    return false;
}

void ScheduleMediaCommandVerifyRefresh() {
    if (!g_MediaCommandVerifyTimer) {
        RequestMediaRefresh(false);
        return;
    }

    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -1200000LL; // 120 ms in 100 ns units, relative time.
    if (!SetWaitableTimer(g_MediaCommandVerifyTimer, &dueTime, 0, nullptr, nullptr, FALSE)) {
        RequestMediaRefresh(false);
    }
}

void HandleMediaCommand(MediaEventsContext& context, int cmd) {
    bool commandIssued = false;

    try {
        if (EnsureMediaEventsInitialized(context)) {
            auto session = PickBestSession(context.manager);
            if (session && IsMediaCommandEnabled(session, cmd)) {
                if (cmd == 1) session.TrySkipPreviousAsync().get();
                else if (cmd == 2) session.TryTogglePlayPauseAsync().get();
                else if (cmd == 3) session.TrySkipNextAsync().get();
                commandIssued = true;
            }
        }
    } catch (...) {}

    if (!commandIssued) {
        return;
    }

    if (cmd == 2) {
        ScheduleMediaCommandVerifyRefresh();
    } else {
        RequestMediaRefresh(true);
    }
}

void ProcessMediaRefresh(MediaEventsContext& context) {
    if (!EnsureMediaEventsInitialized(context)) {
        return;
    }

    bool rebuildSessions = g_NeedRebuildMediaSessions.exchange(false);
    bool fullUpdate = g_NeedFullMediaRefresh.exchange(false);

    if (rebuildSessions) {
        RebuildMediaSessionSubscriptions(context);
        fullUpdate = true;
    }

    UpdateMediaInfo(context.manager, fullUpdate);
    PostMediaUiRefresh();
}

void MediaEventsThread() {
    winrt::init_apartment();
    g_MediaEventsThreadId.store(GetCurrentThreadId());

    MSG queueMsg;
    PeekMessageW(&queueMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    MediaEventsContext context;
    ProcessMediaRefresh(context);

    HANDLE waitHandles[3] = { g_MediaStopEvent, g_MediaRefreshEvent, g_MediaCommandVerifyTimer };
    while (g_Running.load()) {
        DWORD waitResult = MsgWaitForMultipleObjects(3, waitHandles, FALSE, INFINITE, QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            ProcessMediaRefresh(context);
            continue;
        }

        if (waitResult == WAIT_OBJECT_0 + 2) {
            ProcessMediaRefresh(context);
            continue;
        }

        if (waitResult == WAIT_OBJECT_0 + 3) {
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    g_Running.store(false);
                    break;
                }

                if (msg.message == APP_WM_MEDIA_COMMAND) {
                    HandleMediaCommand(context, (int)msg.wParam);
                } else if (msg.message == APP_WM_MEDIA_REFRESH) {
                    ProcessMediaRefresh(context);
                }
            }
        }
    }

    ShutdownMediaEvents(context);
    g_MediaEventsThreadId.store(0);
    winrt::uninit_apartment();
}

// --- Main Thread ---
void MediaThread() {
    g_MediaThreadId.store(GetCurrentThreadId());
    MSG queueMsg;
    PeekMessageW(&queueMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    winrt::init_apartment();

    WNDCLASSW wc{};
    wc.lpfnWndProc = MediaWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"WindhawkMusicDeck_GSMTC";
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClassW(&wc);

    ModSettings settings = GetSettingsSnapshot();
    HWND mediaWindow = nullptr;

    // Try to use CreateWindowInBand
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hUser32) {
        CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand");
    }

    if (CreateWindowInBand) {
        mediaWindow = CreateWindowInBand(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, L"MusicDeck",
            WS_POPUP | WS_VISIBLE,
            0, 0, settings.width, settings.height,
            NULL, NULL, wc.hInstance, NULL,
            ZBID_IMMERSIVE_NOTIFICATION
        );
        if (mediaWindow) {
            Wh_Log(L"Created window in ZBID_IMMERSIVE_NOTIFICATION band");
        }
    }

    if (!mediaWindow) {
        Wh_Log(L"Falling back to CreateWindowEx");
        mediaWindow = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, L"MusicDeck",
            WS_POPUP | WS_VISIBLE,
            0, 0, settings.width, settings.height,
            NULL, NULL, wc.hInstance, NULL
        );
    }

    g_hMediaWindow.store(mediaWindow);
    if (!mediaWindow) {
        Wh_Log(L"Failed to create media window");
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_MediaThreadId.store(0);
        winrt::uninit_apartment();
        return;
    }

    SetLayeredWindowAttributes(mediaWindow, 0, 255, LWA_ALPHA);
    PostMessageW(mediaWindow, APP_WM_REPOSITION, 0, 0);
    if (!g_Running.load()) {
        DestroyWindow(mediaWindow);
    }
    
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_hMediaWindow.store(nullptr);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    g_MediaThreadId.store(0);
    winrt::uninit_apartment();
}

unique_ptr<std::thread> g_pMediaThread;
unique_ptr<std::thread> g_pMediaEventsThread;

// --- CALLBACKS ---
BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-music-deck");
    LoadSettings(); 

    if (!g_GdiplusStarted) {
        GdiplusStartupInput gdiplusStartupInput;
        if (GdiplusStartup(&g_GdiplusToken, &gdiplusStartupInput, NULL) != Ok) {
            Wh_Log(L"GdiplusStartup failed");
            return FALSE;
        }
        g_GdiplusStarted = true;
    }

    if (!g_MediaRefreshEvent) {
        g_MediaRefreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_MediaRefreshEvent) {
            Wh_Log(L"CreateEvent failed");
            if (g_GdiplusStarted) {
                GdiplusShutdown(g_GdiplusToken);
                g_GdiplusStarted = false;
                g_GdiplusToken = 0;
            }
            return FALSE;
        }
    }
    if (!g_MediaStopEvent) {
        g_MediaStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_MediaStopEvent) {
            Wh_Log(L"CreateEvent failed");
            CloseHandle(g_MediaRefreshEvent);
            g_MediaRefreshEvent = nullptr;
            if (g_GdiplusStarted) {
                GdiplusShutdown(g_GdiplusToken);
                g_GdiplusStarted = false;
                g_GdiplusToken = 0;
            }
            return FALSE;
        }
    }
    if (!g_MediaCommandVerifyTimer) {
        g_MediaCommandVerifyTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        if (!g_MediaCommandVerifyTimer) {
            Wh_Log(L"CreateWaitableTimer failed");
            CloseHandle(g_MediaRefreshEvent);
            g_MediaRefreshEvent = nullptr;
            CloseHandle(g_MediaStopEvent);
            g_MediaStopEvent = nullptr;
            if (g_GdiplusStarted) {
                GdiplusShutdown(g_GdiplusToken);
                g_GdiplusStarted = false;
                g_GdiplusToken = 0;
            }
            return FALSE;
        }
    }

    if (!g_FontFamily) {
        g_FontFamily = new FontFamily(FONT_NAME, nullptr);
    }

    g_Running.store(true);
    ResetEvent(g_MediaStopEvent);
    g_NeedFullMediaRefresh.store(true);
    g_NeedRebuildMediaSessions.store(true);
    try {
        g_pMediaThread = make_unique<std::thread>(MediaThread);
        g_pMediaEventsThread = make_unique<std::thread>(MediaEventsThread);
    } catch (...) {
        g_Running.store(false);
        if (g_MediaStopEvent) SetEvent(g_MediaStopEvent);
        if (g_MediaRefreshEvent) SetEvent(g_MediaRefreshEvent);

        HWND hwnd = g_hMediaWindow.load();
        if (hwnd && IsWindow(hwnd)) {
            PostMessageW(hwnd, APP_WM_CLOSE, 0, 0);
        } else {
            DWORD mediaThreadId = g_MediaThreadId.load();
            if (mediaThreadId) PostThreadMessageW(mediaThreadId, WM_QUIT, 0, 0);
        }

        if (g_pMediaEventsThread && g_pMediaEventsThread->joinable()) g_pMediaEventsThread->join();
        if (g_pMediaThread && g_pMediaThread->joinable()) g_pMediaThread->join();
        g_pMediaEventsThread.reset();
        g_pMediaThread.reset();

        delete g_FontFamily;
        g_FontFamily = nullptr;
        if (g_MediaCommandVerifyTimer) {
            CloseHandle(g_MediaCommandVerifyTimer);
            g_MediaCommandVerifyTimer = nullptr;
        }
        if (g_MediaRefreshEvent) {
            CloseHandle(g_MediaRefreshEvent);
            g_MediaRefreshEvent = nullptr;
        }
        if (g_MediaStopEvent) {
            CloseHandle(g_MediaStopEvent);
            g_MediaStopEvent = nullptr;
        }
        if (g_GdiplusStarted) {
            GdiplusShutdown(g_GdiplusToken);
            g_GdiplusStarted = false;
            g_GdiplusToken = 0;
        }
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    g_Running.store(false);
    if (g_MediaStopEvent) SetEvent(g_MediaStopEvent);
    if (g_MediaRefreshEvent) SetEvent(g_MediaRefreshEvent);
    if (g_MediaCommandVerifyTimer) CancelWaitableTimer(g_MediaCommandVerifyTimer);

    if (g_pMediaEventsThread) {
        if (g_pMediaEventsThread->joinable()) g_pMediaEventsThread->join();
        g_pMediaEventsThread.reset();
    }

    HWND hwnd = g_hMediaWindow.load();
    if (hwnd && IsWindow(hwnd)) {
        if (!PostMessageW(hwnd, APP_WM_CLOSE, 0, 0)) {
            DWORD mediaThreadId = g_MediaThreadId.load();
            if (mediaThreadId) PostThreadMessageW(mediaThreadId, WM_QUIT, 0, 0);
        }
    } else {
        DWORD mediaThreadId = g_MediaThreadId.load();
        if (mediaThreadId) PostThreadMessageW(mediaThreadId, WM_QUIT, 0, 0);
    }

    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        g_pMediaThread.reset();
    }

    ClearMediaState(true);

    delete g_FontFamily;
    g_FontFamily = nullptr;

    if (g_GdiplusStarted) {
        GdiplusShutdown(g_GdiplusToken);
        g_GdiplusStarted = false;
        g_GdiplusToken = 0;
    }

    if (g_MediaRefreshEvent) {
        CloseHandle(g_MediaRefreshEvent);
        g_MediaRefreshEvent = nullptr;
    }
    if (g_MediaStopEvent) {
        CloseHandle(g_MediaStopEvent);
        g_MediaStopEvent = nullptr;
    }
    if (g_MediaCommandVerifyTimer) {
        CloseHandle(g_MediaCommandVerifyTimer);
        g_MediaCommandVerifyTimer = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    HWND hwnd = g_hMediaWindow.load();
    if (hwnd) {
         PostMessageW(hwnd, WM_SETTINGCHANGE, 0, 0); 
         PostMessageW(hwnd, APP_WM_VISIBILITY_REFRESH, 0, 0);
         PostMessageW(hwnd, APP_WM_REPOSITION, 0, 0);
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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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

    STARTUPINFOW si{};
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
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
