// ==WindhawkMod==
// @id              live-wallpaper
// @name            🌸 Live Wallpaper
// @description     Play a local video as your desktop wallpaper
// @version         1.0.0
// @author          fusagein
// @github          https://github.com/fusagein
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lmfplay -lmfuuid -lole32 -luuid -luser32 -lgdi32 -lshell32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🌸 Live Wallpaper

Use a local video as your desktop wallpaper

## 🎬 Setup
- Press **Ctrl+Alt+G** to choose a video, or enter its path in Settings
- Your selected video is saved and restored after restart
- Choose **Fill**, **Fit**, or **Stretch**, then adjust mute and volume

## 🖥️ Monitors
- **Primary monitor only** — display the video on your main monitor
- **All monitors** — span one video across the entire desktop (default)
- Other monitors retain their normal wallpaper in primary-only mode
- Switch display modes without restarting playback

## ⏸️ Auto-pause
- Pause video and audio while any visible app is maximized or fullscreen
- Stay paused after Alt+Tab until those windows are minimized, closed, or restored
- Auto-pause checks all monitors, regardless of the selected display mode

## 🔋 Battery (DC)
- **Keep playing** — continue normal playback on battery power
- **Pause wallpaper** — pause video and audio until AC power returns (default)
- Maximized/fullscreen auto-pause still applies in both modes

## 🚀 Startup
Keep the mod enabled and configure **Windhawk** to start with Windows

## 📌 Compatibility
- **Windows 11 x64** — recommended format: **MP4 / H.264 / AAC**
- Windows N requires the **Media Feature Pack**
- No external player required
- Separate videos or mirrored playback on individual monitors are not supported
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- VideoPath: ""
  $name: 🎥 Video file path
  $description: 'Press Ctrl+Alt+G to choose a video file'
- WallpaperMode: fill
  $name: 🪟 Wallpaper mode
  $description: Fill crops edges, Fit may add black bars, Stretch can distort the image
  $options:
  - fill: Fill
  - fit: Fit
  - stretch: Stretch
- MonitorTarget: all
  $name: 🖥️ Display on
  $description: Choose the primary monitor only or span one video across all monitors
  $options:
  - primary: Primary monitor only
  - all: All monitors
- PauseWhenAppFocused: true
  $name: 🚫 Pause while any app is maximized or fullscreen
  $description: Keep video and audio paused while any non-minimized maximized or fullscreen app remains on the current desktop, even after Alt+Tab to a smaller window
- BatteryAction: pause
  $name: 🔋 On battery power (DC)
  $description: Choose what happens on laptop battery. Maximized/fullscreen auto-pause still applies. AC power restores normal behavior
  $options:
  - play: Keep playing
  - pause: Pause wallpaper
- Muted: true
  $name: 🔇 Mute audio
  $description: Turn off to unmute wallpaper audio
- Volume: 50
  $name: 🔊 Audio volume level
  $description: Playback volume from 0 to 100, values outside this range are clamped, muting preserves this level
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <dwmapi.h>
#include <evr.h>
#include <mfplay.h>
#include <mferror.h>
#include <propidl.h>
#include <algorithm>
#include <mutex>
#include <string>

namespace {
struct Settings {
    std::wstring path;
    std::wstring mode = L"fill";
    bool primaryMonitorOnly = false;
    bool muted = true;
    bool pauseOnLargeWindow = true;
    bool pauseOnBattery = true;
    int volume = 50;
    unsigned generation = 0;
};
std::mutex g_lock;
Settings g_settings;
std::wstring g_configuredPath;
bool g_settingsLoaded = false;
constexpr int kChooseVideoHotkey = 0x4C57;
HANDLE g_thread = nullptr;
HANDLE g_stop = nullptr;
HWND g_window = nullptr;             // Accessed only on the playback thread.
IMFPMediaPlayer* g_player = nullptr; // Accessed only on the playback thread.
bool g_mediaReady = false;
bool g_playbackFailed = false;
bool g_loopSeekPending = false;
bool g_shouldPause = false;
bool g_pickerOpen = false;
int g_transportPending = 0; // 1 = Play, 2 = Pause; all transport state is STA-owned.
ULONGLONG g_transportSent = 0;
constexpr UINT kFocusChanged = WM_APP + 71;
// GUID_ACDC_POWER_SOURCE, local definition to avoid an extra import library.
constexpr GUID kPowerSourceGuid = {0x5d3e9a59, 0xe9d5, 0x4b00,
    {0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48}};
constexpr wchar_t kClass[] = L"Windhawk.LiveWallpaper.Video.1";

// Windhawk private persistent storage; no startup tasks, registry Run entries
// or external player are installed by the mod. Called while g_lock is held.
void SaveVideoSelection(const std::wstring& path, const std::wstring& configured) {
    bool ok = Wh_SetStringValue(L"SavedVideoPath", path.c_str()) != FALSE;
    ok = (Wh_SetStringValue(L"SavedVideoConfiguredPath", configured.c_str()) != FALSE) && ok;
    if (ok) ok = Wh_SetIntValue(L"SavedVideoSchema", 1) != FALSE;
    if (!ok) Wh_Log(L"Could not persist video selection to Windhawk storage.");
}
std::wstring StoredString(PCWSTR key) {
    wchar_t buffer[32768]{};
    Wh_GetStringValue(key, buffer, ARRAYSIZE(buffer));
    return buffer;
}
void LoadSettings() {
    Settings s;
    PCWSTR path = Wh_GetStringSetting(L"VideoPath");
    if (path) s.path = path;
    Wh_FreeStringSetting(path);
    PCWSTR mode = Wh_GetStringSetting(L"WallpaperMode");
    if (mode && (wcscmp(mode, L"fit") == 0 || wcscmp(mode, L"stretch") == 0))
        s.mode = mode;
    Wh_FreeStringSetting(mode);
    PCWSTR monitorTarget = Wh_GetStringSetting(L"MonitorTarget");
    s.primaryMonitorOnly = monitorTarget && wcscmp(monitorTarget, L"primary") == 0;
    Wh_FreeStringSetting(monitorTarget);
    // A new key avoids inheriting obsolete BatteryMode/FPS settings.
    PCWSTR batteryAction = Wh_GetStringSetting(L"BatteryAction");
    s.pauseOnBattery = !batteryAction || wcscmp(batteryAction, L"play") != 0;
    Wh_FreeStringSetting(batteryAction);
    s.pauseOnLargeWindow = Wh_GetIntSetting(L"PauseWhenAppFocused") != 0;
    s.muted = Wh_GetIntSetting(L"Muted") != 0;
    s.volume = std::clamp(Wh_GetIntSetting(L"Volume"), 0, 100);
    std::lock_guard<std::mutex> guard(g_lock);
    std::wstring configured = s.path;
    if (!g_settingsLoaded) {
        // A changed Settings path wins. Otherwise restore the most recent picker
        // selection, even if the Settings text box still contains an older path.
        if (Wh_GetIntValue(L"SavedVideoSchema", 0) == 1 &&
            StoredString(L"SavedVideoConfiguredPath") == configured) {
            s.path = StoredString(L"SavedVideoPath");
        }
    } else if (configured == g_configuredPath) {
        s.path = g_settings.path; // audio/layout edits must not reset selection
    }
    g_configuredPath = configured;
    g_settingsLoaded = true;
    s.generation = g_settings.generation + 1;
    g_settings = s;
    SaveVideoSelection(g_settings.path, g_configuredPath);
}
Settings Snapshot() {
    std::lock_guard<std::mutex> guard(g_lock);
    return g_settings;
}
void LogError(PCWSTR operation, HRESULT hr) {
    Wh_Log(L"%s failed: 0x%08X", operation, static_cast<unsigned>(hr));
}
void ApplyAudio(IMFPMediaPlayer* player) {
    Settings s = Snapshot();
    HRESULT hr = player->SetMute(s.muted);
    if (FAILED(hr)) LogError(L"SetMute", hr);
    hr = player->SetVolume(s.volume / 100.0f);
    if (FAILED(hr)) LogError(L"SetVolume", hr);
}
// System events are wakeups only. Every policy check scans all eligible
// top-level windows, not just the foreground app. Explorer folders count too.
bool WindowIsMaximizedOrFullscreen(HWND window) {
    if (!window || !IsWindow(window)) return false;
    if (window == g_window || !IsWindowVisible(window) || IsIconic(window)) return false;
    wchar_t name[128]{}; GetClassNameW(window, name, ARRAYSIZE(name));
    if (!wcscmp(name, L"Progman") || !wcscmp(name, L"WorkerW") ||
        !wcscmp(name, L"Shell_TrayWnd") || !wcscmp(name, L"Shell_SecondaryTrayWnd")) return false;
    LONG_PTR extendedStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if (extendedStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked)
        return false;
    // Maximize is a Windows window state, not monitor-sized geometry. A normal
    // maximized window leaves the taskbar visible and must still pause playback.
    if (IsZoomed(window)) return true;
    WINDOWPLACEMENT placement{};
    placement.length = sizeof(placement);
    if (GetWindowPlacement(window, &placement) && placement.showCmd == SW_SHOWMAXIMIZED)
        return true;
    // DWM bounds are physical pixels. Temporarily use matching coordinates for
    // window, client and monitor queries; restore before any renderer calls.
    struct DpiScope {
        DPI_AWARENESS_CONTEXT old = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ~DpiScope() { if (old) SetThreadDpiAwarenessContext(old); }
    } dpi;
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) return false;
    auto covers = [&](const RECT& rect) {
        constexpr LONG tolerance = 2;
        const RECT& m = monitor.rcMonitor; // Fullscreen fallback; maximized state was already handled above.
        return !IsRectEmpty(&rect) && rect.left <= m.left+tolerance && rect.top <= m.top+tolerance &&
               rect.right >= m.right-tolerance && rect.bottom >= m.bottom-tolerance;
    };
    RECT frame{};
    HRESULT hr = DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame));
    if (SUCCEEDED(hr)) {
        if (covers(frame)) return true;
    } else if (GetWindowRect(window, &frame) && covers(frame)) return true;
    RECT client{};
    if (GetClientRect(window, &client)) {
        POINT origin{client.left, client.top}, end{client.right, client.bottom};
        if (ClientToScreen(window, &origin) && ClientToScreen(window, &end)) {
            client = RECT{origin.x, origin.y, end.x, end.y};
            if (covers(client)) return true;
        }
    }
    return false;
}
BOOL CALLBACK FindLargeWindow(HWND window, LPARAM param) {
    if (!WindowIsMaximizedOrFullscreen(window)) return TRUE;
    *reinterpret_cast<HWND*>(param) = window;
    return FALSE; // One qualifying window is enough to keep playback paused.
}
bool AnyAppIsMaximizedOrFullscreen() {
    HWND found = nullptr;
    EnumWindows(FindLargeWindow, reinterpret_cast<LPARAM>(&found));
    return found != nullptr;
}
void ReconcilePlayback() {
    if (!g_player || !g_mediaReady || g_loopSeekPending) return;
    if (g_transportPending && GetTickCount64() - g_transportSent < 1500) return;
    g_transportPending = 0;
    MFP_MEDIAPLAYER_STATE state{};
    HRESULT hr = g_player->GetState(&state);
    if (FAILED(hr)) return;
    int command = 0;
    if (g_shouldPause && state == MFP_MEDIAPLAYER_STATE_PLAYING) command = 2;
    else if (!g_shouldPause && (state == MFP_MEDIAPLAYER_STATE_PAUSED ||
                               state == MFP_MEDIAPLAYER_STATE_STOPPED)) command = 1;
    if (!command) return;
    // MFPlay operations are asynchronous; completion events drive reconciliation.
    g_transportPending = command;
    g_transportSent = GetTickCount64();
    hr = command == 2 ? g_player->Pause() : g_player->Play();
    if (FAILED(hr)) { g_transportPending = 0; LogError(command == 2 ? L"Pause" : L"Play", hr); }
}
bool IsRunningOnBattery() {
    SYSTEM_POWER_STATUS power{};
    if (!GetSystemPowerStatus(&power)) return false;
    // ACLineStatus: 0 = offline/DC, 1 = online/AC, 255 = unknown.
    // BatteryFlag 128 means no system battery; 255 is unknown.
    return power.ACLineStatus == 0 && power.BatteryFlag != 255 &&
           !(power.BatteryFlag & 128);
}
void EvaluatePausePolicy() {
    Settings s = Snapshot();
    bool largeWindow = s.pauseOnLargeWindow && AnyAppIsMaximizedOrFullscreen();
    bool battery = s.pauseOnBattery && IsRunningOnBattery();
    bool pause = g_pickerOpen || largeWindow || battery;
    if (pause != g_shouldPause) {
        g_shouldPause = pause;
        Wh_Log(L"Auto-pause: %s (large-window=%d, battery=%d, picker=%d)",
               pause ? L"paused" : L"resume", largeWindow, battery, g_pickerOpen);
    }
    ReconcilePlayback();
}
void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    // WINEVENT_OUTOFCONTEXT delivers on the registering message-loop thread.
    // Never run COM transport from inside the system event callback.
    if (g_window) PostMessageW(g_window, kFocusChanged, 0, 0);
}
// All renderer calls run on the playback apartment, including WM_SIZE.
void ApplyLayout(IMFPMediaPlayer* player) {
    if (!player || !IsWindow(g_window)) return;
    Settings s = Snapshot();
    MFVideoNormalizedRect crop{0.0f, 0.0f, 1.0f, 1.0f};
    DWORD aspectMode = s.mode == L"fit" ? MFVideoARMode_PreservePicture : MFVideoARMode_None;
    if (s.mode == L"fill") {
        SIZE native{}, display{};
        RECT client{};
        if (SUCCEEDED(player->GetNativeVideoSize(&native, &display)) &&
            GetClientRect(g_window, &client)) {
            double vw = display.cx > 0 ? display.cx : native.cx;
            double vh = display.cy > 0 ? display.cy : native.cy;
            double dw = client.right - client.left, dh = client.bottom - client.top;
            if (vw > 0 && vh > 0 && dw > 0 && dh > 0) {
                double videoAspect = vw / vh, screenAspect = dw / dh;
                if (videoAspect > screenAspect) {
                    float width = static_cast<float>(screenAspect / videoAspect);
                    crop.left = (1.0f - width) / 2.0f;
                    crop.right = 1.0f - crop.left;
                } else {
                    float height = static_cast<float>(videoAspect / screenAspect);
                    crop.top = (1.0f - height) / 2.0f;
                    crop.bottom = 1.0f - crop.top;
                }
            }
        }
    }
    // Reset the source rect for Fit/Stretch as well, so a previous Fill crop is removed.
    player->SetAspectRatioMode(aspectMode);
    player->SetVideoSourceRect(&crop);
    player->SetBorderColor(RGB(0, 0, 0));
    player->UpdateVideo();
}
class PlayerCallback final : public IMFPMediaPlayerCallback {
    LONG refs_ = 1;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (iid == __uuidof(IUnknown) ||
            iid == __uuidof(IMFPMediaPlayerCallback)) {
            *out = static_cast<IMFPMediaPlayerCallback*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&refs_);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = InterlockedDecrement(&refs_);
        if (!n) delete this;
        return n;
    }
    void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER* e) override {
        if (e->pMediaPlayer != g_player) return;
        if (FAILED(e->hrEvent)) {
            g_playbackFailed = true;
            g_transportPending = 0;
            g_loopSeekPending = false;
            LogError(L"Media playback", e->hrEvent);
            return;
        }
        // No FREE_THREADED_CALLBACK flag: events arrive on our message-loop thread.
        if (e->eEventType == MFP_EVENT_TYPE_MEDIAITEM_SET) {
            if (e->pMediaPlayer != g_player) return;
            ApplyAudio(e->pMediaPlayer);
            ApplyLayout(e->pMediaPlayer);
            ShowWindow(g_window, SW_SHOWNOACTIVATE);
            g_mediaReady = true;
            g_playbackFailed = false;
            g_transportPending = 0;
            EvaluatePausePolicy();
        } else if (e->eEventType == MFP_EVENT_TYPE_PLAYBACK_ENDED) {
            PROPVARIANT start{};
            start.vt = VT_I8;
            start.hVal.QuadPart = 0;
            g_loopSeekPending = true;
            HRESULT hr = e->pMediaPlayer->SetPosition(MFP_POSITIONTYPE_100NS, &start);
            if (FAILED(hr)) { g_loopSeekPending = false; LogError(L"Loop video", hr); }
            // Wait for the seek completion before considering replay.
        } else if (e->eEventType == MFP_EVENT_TYPE_PLAY ||
                   e->eEventType == MFP_EVENT_TYPE_PAUSE ||
                   e->eEventType == MFP_EVENT_TYPE_POSITION_SET) {
            if (e->eEventType == MFP_EVENT_TYPE_POSITION_SET) g_loopSeekPending = false;
            g_transportPending = 0;
            // Correct a queued Play completing after an app becomes maximized/fullscreen.
            EvaluatePausePolicy();
        }
    }
};

BOOL CALLBACK FindDesktopHost(HWND window, LPARAM param) {
    if (!FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr)) return TRUE;
    HWND host = FindWindowExW(nullptr, window, L"WorkerW", nullptr);
    if (host) {
        *reinterpret_cast<HWND*>(param) = host;
        return FALSE;
    }
    return TRUE;
}
HWND DesktopHost() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (!progman) return nullptr;
    DWORD_PTR result = 0;
    // Ask Explorer to create its wallpaper WorkerW (undocumented shell message).
    SendMessageTimeoutW(progman, 0x052C, 0xD, 0, SMTO_ABORTIFHUNG, 1000, &result);
    SendMessageTimeoutW(progman, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000, &result);
    HWND host = nullptr;
    EnumWindows(FindDesktopHost, reinterpret_cast<LPARAM>(&host));
    if (host) return host;
    // Some newer Explorer builds put the wallpaper WorkerW inside Progman.
    HWND child = nullptr;
    while ((child = FindWindowExW(progman, child, L"WorkerW", nullptr))) {
        if (!FindWindowExW(child, nullptr, L"SHELLDLL_DefView", nullptr)) return child;
    }
    // Never put a full-screen video on top of the desktop as a fallback.
    return nullptr;
}
void ResizeVideo() {
    if (!IsWindow(g_window)) return;
    Settings settings = Snapshot();
    // Query monitor and host coordinates in physical pixels, including mixed-DPI
    // layouts and secondary monitors with negative virtual-desktop coordinates
    struct DpiScope {
        DPI_AWARENESS_CONTEXT old = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ~DpiScope() { if (old) SetThreadDpiAwarenessContext(old); }
    } dpi;
    RECT target{};
    if (settings.primaryMonitorOnly) {
        MONITORINFO primary{sizeof(primary)};
        if (!GetMonitorInfoW(MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY), &primary)) {
            Wh_Log(L"Cannot query primary monitor: %u", GetLastError());
            return;
        }
        target = primary.rcMonitor;
    } else {
        target.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        target.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        target.right = target.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        target.bottom = target.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    POINT origin{target.left, target.top};
    if (!ScreenToClient(GetParent(g_window), &origin)) return;
    RECT current{};
    if (!GetWindowRect(g_window, &current)) return;
    POINT topLeft{current.left, current.top};
    if (!ScreenToClient(GetParent(g_window), &topLeft)) return;
    int width = target.right - target.left;
    int height = target.bottom - target.top;
    if (width <= 0 || height <= 0) return;
    if (topLeft.x != origin.x || topLeft.y != origin.y ||
        current.right-current.left != width || current.bottom-current.top != height) {
        // Reuse the same MFPlay video window: no player restart or audio duplicate
        SetWindowPos(g_window, HWND_BOTTOM, origin.x, origin.y, width, height, SWP_NOACTIVATE);
        // WM_SIZE applies Fill/Fit/Stretch to the new presentation area
    }
}
LRESULT CALLBACK VideoProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case kFocusChanged:
        EvaluatePausePolicy();
        return 0;
    case WM_POWERBROADCAST:
        if (wp == PBT_POWERSETTINGCHANGE || wp == PBT_APMPOWERSTATUSCHANGE ||
            wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND)
            EvaluatePausePolicy();
        return TRUE;
    case WM_DISPLAYCHANGE:
    case WM_SETTINGCHANGE:
        ResizeVideo();
        return 0;
    case WM_NCHITTEST: return HTTRANSPARENT;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_SIZE:
        if (g_player) ApplyLayout(g_player);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        if (g_player) g_player->UpdateVideo();
        else FillRect(ps.hdc, &ps.rcPaint, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
void StopPlayer() {
    g_mediaReady = false;
    g_playbackFailed = false;
    g_loopSeekPending = false;
    g_transportPending = 0;
    if (g_player) {
        g_player->Shutdown();
        g_player->Release();
        g_player = nullptr;
    }
    if (IsWindow(g_window)) ShowWindow(g_window, SW_HIDE);
}
void OpenVideo(const Settings& s, PlayerCallback* callback) {
    StopPlayer();
    if (s.path.empty()) return;
    DWORD attributes = GetFileAttributesW(s.path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"Video file not found: %s", s.path.c_str());
        return;
    }
    // Create without a source so mute/volume are set before any playback starts.
    HRESULT hr = MFPCreateMediaPlayer(nullptr, FALSE, 0, callback, g_window, &g_player);
    if (FAILED(hr)) { LogError(L"Create player", hr); return; }
    ApplyAudio(g_player);
    IMFPMediaItem* item = nullptr;
    hr = g_player->CreateMediaItemFromURL(s.path.c_str(), TRUE, 0, &item);
    if (SUCCEEDED(hr)) {
        hr = g_player->SetMediaItem(item);
        item->Release();
    }
    if (FAILED(hr)) { LogError(L"Open video", hr); StopPlayer(); }
}
// The native Common Item Dialog runs a modal STA loop. A thread timer closes
// it on unload, avoiding callbacks into an unloaded Windhawk module.
IFileOpenDialog* g_picker = nullptr; // Playback thread only; borrowed while Show runs.
void CALLBACK PickerStopTimer(HWND, UINT, UINT_PTR, DWORD) {
    if (g_picker && WaitForSingleObject(g_stop, 0) == WAIT_OBJECT_0)
        g_picker->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
}
void ChooseVideo() {
    IFileOpenDialog* dialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&dialog));
    if (FAILED(hr)) { LogError(L"Create native file picker", hr); return; }
    FILEOPENDIALOGOPTIONS options{};
    hr = dialog->GetOptions(&options);
    if (SUCCEEDED(hr)) hr = dialog->SetOptions(options | FOS_FORCEFILESYSTEM |
        FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR | FOS_DONTADDTORECENT);
    const COMDLG_FILTERSPEC filters[] = {
        {L"Video files", L"*.mp4;*.m4v;*.mov;*.wmv;*.avi;*.mkv;*.webm"},
        {L"All files", L"*.*"}
    };
    if (SUCCEEDED(hr)) hr = dialog->SetFileTypes(ARRAYSIZE(filters), filters);
    dialog->SetFileTypeIndex(1);
    dialog->SetTitle(L"Live Wallpaper - Select a video");
    dialog->SetOkButtonLabel(L"Use video");
    // Reopen at the current video's folder when the shell can resolve it.
    Settings current = Snapshot();
    size_t slash = current.path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        IShellItem* folder = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(current.path.substr(0, slash + 1).c_str(),
                                                nullptr, IID_PPV_ARGS(&folder)))) {
            dialog->SetFolder(folder);
            folder->Release();
        }
    }
    g_picker = dialog;
    UINT_PTR timer = SetTimer(nullptr, 0, 100, PickerStopTimer);
    if (!timer) hr = HRESULT_FROM_WIN32(GetLastError() ? GetLastError() : ERROR_NOT_ENOUGH_MEMORY);
    if (SUCCEEDED(hr) && WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0)
        hr = dialog->Show(nullptr);
    else if (SUCCEEDED(hr)) hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
    if (timer) KillTimer(nullptr, timer);
    g_picker = nullptr;
    if (SUCCEEDED(hr) && WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
        IShellItem* item = nullptr;
        hr = dialog->GetResult(&item);
        if (SUCCEEDED(hr)) {
            PWSTR path = nullptr;
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
            if (SUCCEEDED(hr)) {
                std::lock_guard<std::mutex> guard(g_lock);
                g_settings.path = path;
                ++g_settings.generation;
                SaveVideoSelection(g_settings.path, g_configuredPath);
            }
            CoTaskMemFree(path);
            item->Release();
        }
    }
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        LogError(L"Native video picker", hr);
    dialog->Release();
}
DWORD WINAPI PlaybackThread(void*) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) { LogError(L"COM initialization", hr); return 0; }
    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW wc{};
    wc.lpfnWndProc = VideoProc;
    wc.hInstance = instance;
    wc.lpszClassName = kClass;
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    ATOM atom = RegisterClassW(&wc);
    // At sign-in Explorer can load the mod before its desktop WorkerW exists.
    // Retry until ready, with an interruptible wait so unload is never stuck.
    if (!atom) Wh_Log(L"RegisterClass failed: %u", GetLastError());
    bool waitingLogged = false;
    while (atom && WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
        HWND host = DesktopHost();
        if (host) {
            g_window = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                kClass, L"Live Wallpaper", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                0, 0, 1, 1, host, nullptr, instance, nullptr);
        }
        if (g_window) break;
        if (!waitingLogged) {
            Wh_Log(L"Waiting for Explorer desktop host at startup...");
            waitingLogged = true;
        }
        if (WaitForSingleObject(g_stop, 1000) != WAIT_TIMEOUT) break;
    }
    if (!g_window) {
        Wh_Log(L"Wallpaper startup ended without a desktop window (cancelled or class registration failed).");
    } else {
        HPOWERNOTIFY powerNotify = RegisterPowerSettingNotification(
            g_window, &kPowerSourceGuid, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!powerNotify) Wh_Log(L"Power notifications unavailable; 200ms status checks remain active.");
        auto* callback = new PlayerCallback();
        Settings current = Snapshot();
        ResizeVideo();
        EvaluatePausePolicy();
        OpenVideo(current, callback);
        HWINEVENTHOOK focusHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_FOREGROUND, nullptr, ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (!focusHook) Wh_Log(L"Foreground event hook unavailable; periodic focus checks remain active.");
        ULONGLONG nextGeometry = GetTickCount64() + 2000;
        ULONGLONG nextVideoRetry = GetTickCount64() + 10000;
        bool hotkeyRegistered = RegisterHotKey(nullptr, kChooseVideoHotkey,
            MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'G') != FALSE;
        if (!hotkeyRegistered)
            Wh_Log(L"Cannot register Ctrl+Alt+G (error %u). Another app may own it.", GetLastError());
        bool running = true;
        while (running) {
            DWORD wait = MsgWaitForMultipleObjects(1, &g_stop, FALSE, 200, QS_ALLINPUT);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) { running = false; break; }
                if (msg.message == WM_HOTKEY && msg.wParam == kChooseVideoHotkey) {
                    // Temporarily unregister to prevent repeated dialogs in the modal loop.
                    UnregisterHotKey(nullptr, kChooseVideoHotkey);
                    g_pickerOpen = true;
                    EvaluatePausePolicy();
                    ChooseVideo();
                    g_pickerOpen = false;
                    EvaluatePausePolicy();
                    hotkeyRegistered = false;
                    if (WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
                        hotkeyRegistered = RegisterHotKey(nullptr, kChooseVideoHotkey,
                            MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'G') != FALSE;
                        if (!hotkeyRegistered) Wh_Log(L"Could not re-register Ctrl+Alt+G.");
                    }
                    continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (!running || !IsWindow(g_window)) break;
            Settings next = Snapshot();
            if (next.generation != current.generation) {
                // Monitor selection applies immediately without reloading the video
                if (next.primaryMonitorOnly != current.primaryMonitorOnly) ResizeVideo();
                if (next.path != current.path) OpenVideo(next, callback);
                else if (g_player) { ApplyAudio(g_player); ApplyLayout(g_player); }
                current = next;
            }
            // A removable/network drive or codec can be unavailable during sign-in.
            // Retry failed opens without reloading an already healthy player.
            if ((!g_player || g_playbackFailed) && !current.path.empty() &&
                GetTickCount64() >= nextVideoRetry) {
                DWORD attributes = GetFileAttributesW(current.path.c_str());
                if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
                    OpenVideo(current, callback);
                nextVideoRetry = GetTickCount64() + 10000;
            }
            EvaluatePausePolicy();
            // Focus checks do not resize, repaint or reconfigure the renderer.
            if (GetTickCount64() >= nextGeometry) {
                ResizeVideo();
                nextGeometry = GetTickCount64() + 2000;
            }
        }
        if (powerNotify) UnregisterPowerSettingNotification(powerNotify);
        if (focusHook) UnhookWinEvent(focusHook);
        if (hotkeyRegistered) UnregisterHotKey(nullptr, kChooseVideoHotkey);
        StopPlayer();
        // Drain queued MFPlay notifications while the callback and module are alive.
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message != WM_QUIT) DispatchMessageW(&msg);
        }
        callback->Release();
        if (IsWindow(g_window)) DestroyWindow(g_window);
        g_window = nullptr;
    }
    if (atom) UnregisterClassW(kClass, instance);
    CoUninitialize();
    return 0;
}
} // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop) return FALSE;
    g_thread = CreateThread(nullptr, 0, PlaybackThread, nullptr, 0, nullptr);
    if (!g_thread) { CloseHandle(g_stop); g_stop = nullptr; return FALSE; }
    return TRUE;
}
void Wh_ModSettingsChanged() {
    LoadSettings();
}
void Wh_ModUninit() {
    if (g_stop) SetEvent(g_stop);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stop) { CloseHandle(g_stop); g_stop = nullptr; }
}
