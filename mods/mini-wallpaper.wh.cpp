// ==WindhawkMod==
// @id              mini-wallpaper
// @name            Mini Wallpaper
// @description     Local video, GIF, or image wallpaper hosted behind desktop icons.
// @version         1.0.0
// @author          Mirochill
// @github          https://github.com/Mirochill
// @homepage        https://github.com/Mirochill/mini-wallpaper
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lgdiplus -lshell32 -lcomdlg32 -lole32 -loleaut32 -lstrmiids -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mini Wallpaper

Mini Wallpaper plays a local video, GIF, or image behind the desktop icons. It
is launched automatically by Windhawk in a dedicated helper process, so no
separate startup registry entry is required.

Use the tray menu to choose a file, pause/resume playback, or reload the file
from settings. Supported direct inputs include common video files, animated
GIFs, and static image formats supported by GDI+.

If FFmpeg is available and optimization is enabled, supported video/GIF input
is transcoded into a deterministic MP4 cache under the Windhawk mod storage
directory. The original file is never modified.

## Notes

- Video playback uses the native DirectShow graph available on the system.
- GIF and image playback use GDI+ with cover/contain scaling.
- The default file path matches the standalone app convention:
  `%USERPROFILE%\Documents\Gifs\wallpaper.mp4`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperPath: ""
  $name: Wallpaper path
  $description: Optional explicit media path. Leave empty to use the tray-selected path or default path.

- useStoredWallpaper: true
  $name: Use tray-selected wallpaper
  $description: Use the path chosen from the tray menu when Wallpaper path is empty.

- defaultWallpaperPath: "%USERPROFILE%\\Documents\\Gifs\\wallpaper.mp4"
  $name: Default wallpaper path
  $description: Fallback path used when no setting or stored tray selection exists.

- stretchMode: cover
  $name: Stretch mode
  $description: Use cover to fill the desktop or contain to show the whole image/video.
  $options:
  - cover: Cover
  - contain: Contain

- optimizeMedia: true
  $name: Optimize media with FFmpeg
  $description: Create a cached MP4 copy for video/GIF input when ffmpeg.exe is available.

- ffmpegPath: ""
  $name: FFmpeg path
  $description: Optional full path to ffmpeg.exe. Leave empty to search PATH.

- maxFps: 30
  $name: Optimized max FPS
  $description: FPS cap for generated MP4 wallpaper copies.

- crf: 23
  $name: Optimized CRF
  $description: H.264 CRF quality value for generated MP4 copies.

- muteAudio: true
  $name: Mute audio
  $description: Mute video playback.

- showTrayIcon: true
  $name: Show tray icon
  $description: Show the Mini Wallpaper notification-area menu.

- autoOpenPicker: true
  $name: Open picker on first run
  $description: Open the file picker if no configured, stored, or default wallpaper exists.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <dshow.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <string>
#include <vector>

namespace {

constexpr PCWSTR kWallpaperWindowClass = L"MiniWallpaperWindhawkWindow";
constexpr UINT kMsgTrayCallback = WM_APP + 1;
constexpr UINT kMsgGraphEvent = WM_APP + 2;
constexpr UINT kMsgReloadSettings = WM_APP + 3;
constexpr UINT_PTR kGifTimerId = 1;
constexpr UINT kTrayIconId = 1;
constexpr UINT kMenuChoose = 1001;
constexpr UINT kMenuPause = 1002;
constexpr UINT kMenuReload = 1003;
constexpr UINT kMenuQuit = 1004;
constexpr UINT kMenuOpenWindhawk = 1005;
constexpr UINT kSpawnWorkerW = 0x052C;

struct Settings {
    std::wstring wallpaperPath;
    bool useStoredWallpaper = true;
    std::wstring defaultWallpaperPath;
    std::wstring stretchMode = L"cover";
    bool optimizeMedia = true;
    std::wstring ffmpegPath;
    int maxFps = 30;
    int crf = 23;
    bool muteAudio = true;
    bool showTrayIcon = true;
    bool autoOpenPicker = true;
};

enum class PlaybackKind {
    None,
    Video,
    Image,
    Gif,
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
std::atomic<bool> g_stopWorker = false;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

HWND g_wallpaperWindow = nullptr;
UINT g_taskbarCreatedMessage = 0;
bool g_trayAdded = false;
bool g_paused = false;
std::wstring g_activePath;
PlaybackKind g_playbackKind = PlaybackKind::None;
ULONG_PTR g_gdiplusToken = 0;

IGraphBuilder* g_graph = nullptr;
IMediaControl* g_mediaControl = nullptr;
IMediaEventEx* g_mediaEvent = nullptr;
IVideoWindow* g_videoWindow = nullptr;
IMediaSeeking* g_mediaSeeking = nullptr;
IBasicAudio* g_basicAudio = nullptr;
IBasicVideo* g_basicVideo = nullptr;

Gdiplus::Image* g_image = nullptr;
GUID g_frameDimension{};
UINT g_frameCount = 0;
UINT g_frameIndex = 0;
std::vector<UINT> g_frameDelays;

int ClampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(maxValue, value));
}

std::wstring GetStringSettingValue(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring GetStoredStringValue(PCWSTR name) {
    std::vector<wchar_t> buffer(32768);
    size_t copied = Wh_GetStringValue(name, buffer.data(), buffer.size());
    if (copied == 0) {
        return L"";
    }
    return buffer.data();
}

std::wstring ExpandPath(const std::wstring& path) {
    if (path.empty()) {
        return path;
    }

    DWORD required = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
    if (required == 0) {
        return path;
    }

    std::wstring expanded(required, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(path.c_str(), expanded.data(), required);
    if (written == 0 || written > required) {
        return path;
    }
    expanded.resize(written - 1);
    return expanded;
}

Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings snapshot = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return snapshot;
}

void LoadSettings() {
    Settings next;
    next.wallpaperPath = ExpandPath(GetStringSettingValue(L"wallpaperPath"));
    next.useStoredWallpaper = Wh_GetIntSetting(L"useStoredWallpaper") != 0;
    next.defaultWallpaperPath =
        ExpandPath(GetStringSettingValue(L"defaultWallpaperPath"));
    if (next.defaultWallpaperPath.empty()) {
        next.defaultWallpaperPath =
            ExpandPath(L"%USERPROFILE%\\Documents\\Gifs\\wallpaper.mp4");
    }
    next.stretchMode = GetStringSettingValue(L"stretchMode");
    if (next.stretchMode != L"contain") {
        next.stretchMode = L"cover";
    }
    next.optimizeMedia = Wh_GetIntSetting(L"optimizeMedia") != 0;
    next.ffmpegPath = ExpandPath(GetStringSettingValue(L"ffmpegPath"));
    next.maxFps = ClampInt(Wh_GetIntSetting(L"maxFps"), 1, 120);
    next.crf = ClampInt(Wh_GetIntSetting(L"crf"), 0, 51);
    next.muteAudio = Wh_GetIntSetting(L"muteAudio") != 0;
    next.showTrayIcon = Wh_GetIntSetting(L"showTrayIcon") != 0;
    next.autoOpenPicker = Wh_GetIntSetting(L"autoOpenPicker") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = next;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

int RectWidth(const RECT& rect) {
    return rect.right - rect.left;
}

int RectHeight(const RECT& rect) {
    return rect.bottom - rect.top;
}

RECT GetVirtualScreenRect() {
    RECT rect{};
    rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rect.right = rect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rect.bottom = rect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return rect;
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring GetExtension(const std::wstring& path) {
    PCWSTR ext = PathFindExtensionW(path.c_str());
    return ext ? ToLower(ext) : L"";
}

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool DirectoryExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void EnsureDirectory(const std::wstring& path) {
    if (path.empty() || DirectoryExists(path)) {
        return;
    }

    std::wstring parent = path;
    size_t slash = parent.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        parent.resize(slash);
        EnsureDirectory(parent);
    }
    CreateDirectoryW(path.c_str(), nullptr);
}

std::wstring Quote(const std::wstring& value) {
    std::wstring quoted = L"\"";
    for (wchar_t ch : value) {
        if (ch == L'"') {
            quoted += L"\\\"";
        } else {
            quoted += ch;
        }
    }
    quoted += L"\"";
    return quoted;
}

std::wstring GetModStoragePath() {
    std::vector<wchar_t> buffer(MAX_PATH * 4);
    size_t copied = Wh_GetModStoragePath(buffer.data(), buffer.size());
    if (copied == 0) {
        return L"";
    }
    return buffer.data();
}

std::wstring FindFfmpeg(const Settings& settings) {
    if (!settings.ffmpegPath.empty() && FileExists(settings.ffmpegPath)) {
        return settings.ffmpegPath;
    }

    wchar_t found[MAX_PATH]{};
    DWORD result =
        SearchPathW(nullptr, L"ffmpeg.exe", nullptr, ARRAYSIZE(found), found,
                    nullptr);
    if (result > 0 && result < ARRAYSIZE(found)) {
        return found;
    }

    return L"";
}

bool IsOptimizableMedia(const std::wstring& extension) {
    return extension == L".mp4" || extension == L".wmv" ||
           extension == L".avi" || extension == L".mov" ||
           extension == L".mkv" || extension == L".gif";
}

bool IsVideoExtension(const std::wstring& extension) {
    return extension == L".mp4" || extension == L".wmv" ||
           extension == L".avi" || extension == L".mov" ||
           extension == L".mkv";
}

bool IsGifExtension(const std::wstring& extension) {
    return extension == L".gif";
}

bool IsImageExtension(const std::wstring& extension) {
    return extension == L".jpg" || extension == L".jpeg" ||
           extension == L".png" || extension == L".bmp" ||
           extension == L".webp" || extension == L".tif" ||
           extension == L".tiff";
}

unsigned long long Fnv1a64(const std::wstring& text) {
    unsigned long long hash = 1469598103934665603ULL;
    const unsigned char* bytes =
        reinterpret_cast<const unsigned char*>(text.data());
    size_t byteCount = text.size() * sizeof(wchar_t);
    for (size_t i = 0; i < byteCount; i++) {
        hash ^= bytes[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::wstring OptimizedFileName(const std::wstring& source,
                               const Settings& settings) {
    WIN32_FILE_ATTRIBUTE_DATA data{};
    GetFileAttributesExW(source.c_str(), GetFileExInfoStandard, &data);
    RECT screen = GetVirtualScreenRect();

    wchar_t fingerprint[4096]{};
    swprintf_s(fingerprint, L"%s|%lu|%lu|%lu|%lu|%d|%d|%d|%d",
               source.c_str(), data.nFileSizeHigh, data.nFileSizeLow,
               data.ftLastWriteTime.dwHighDateTime,
               data.ftLastWriteTime.dwLowDateTime, RectWidth(screen),
               RectHeight(screen), settings.maxFps, settings.crf);

    wchar_t fileName[64]{};
    swprintf_s(fileName, L"%016llx.mp4", Fnv1a64(fingerprint));
    return fileName;
}

bool RunHiddenProcess(const std::wstring& commandLine) {
    std::wstring mutableCommandLine = commandLine;
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION processInfo{};
    if (!CreateProcessW(nullptr, mutableCommandLine.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo,
                        &processInfo)) {
        return false;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(processInfo.hProcess, &exitCode);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return exitCode == 0;
}

std::wstring PrepareWallpaper(const std::wstring& source,
                              const Settings& settings) {
    if (!settings.optimizeMedia || !FileExists(source)) {
        return source;
    }

    std::wstring extension = GetExtension(source);
    if (!IsOptimizableMedia(extension)) {
        return source;
    }

    std::wstring ffmpeg = FindFfmpeg(settings);
    if (ffmpeg.empty()) {
        return source;
    }

    std::wstring storage = GetModStoragePath();
    if (storage.empty()) {
        return source;
    }

    std::wstring optimizedDir = storage + L"\\optimized";
    EnsureDirectory(optimizedDir);
    std::wstring optimizedPath =
        optimizedDir + L"\\" + OptimizedFileName(source, settings);
    if (FileExists(optimizedPath)) {
        return optimizedPath;
    }

    std::wstring tempPath = optimizedPath + L".tmp.mp4";
    DeleteFileW(tempPath.c_str());

    RECT screen = GetVirtualScreenRect();
    wchar_t filter[256]{};
    swprintf_s(filter,
               L"scale=w='min(iw,%d)':h='min(ih,%d)':"
               L"force_original_aspect_ratio=decrease,fps=%d",
               RectWidth(screen), RectHeight(screen), settings.maxFps);

    std::wstring commandLine =
        Quote(ffmpeg) + L" -y -i " + Quote(source) + L" -vf " +
        Quote(filter) + L" -c:v libx264 -preset slow -crf " +
        std::to_wstring(settings.crf) +
        L" -pix_fmt yuv420p -movflags +faststart -an " + Quote(tempPath);

    bool ok = RunHiddenProcess(commandLine);
    if (ok && FileExists(tempPath)) {
        DeleteFileW(optimizedPath.c_str());
        if (MoveFileW(tempPath.c_str(), optimizedPath.c_str())) {
            return optimizedPath;
        }
    }

    DeleteFileW(tempPath.c_str());
    return source;
}

void SafeRelease(IUnknown** value) {
    if (*value) {
        (*value)->Release();
        *value = nullptr;
    }
}

void StopVideo() {
    if (g_mediaControl) {
        g_mediaControl->Stop();
    }
    if (g_mediaEvent) {
        g_mediaEvent->SetNotifyWindow(NULL, 0, 0);
    }
    if (g_videoWindow) {
        g_videoWindow->put_Visible(OAFALSE);
        g_videoWindow->put_Owner(NULL);
    }

    SafeRelease(reinterpret_cast<IUnknown**>(&g_basicVideo));
    SafeRelease(reinterpret_cast<IUnknown**>(&g_basicAudio));
    SafeRelease(reinterpret_cast<IUnknown**>(&g_mediaSeeking));
    SafeRelease(reinterpret_cast<IUnknown**>(&g_videoWindow));
    SafeRelease(reinterpret_cast<IUnknown**>(&g_mediaEvent));
    SafeRelease(reinterpret_cast<IUnknown**>(&g_mediaControl));
    SafeRelease(reinterpret_cast<IUnknown**>(&g_graph));
}

void StopImage() {
    KillTimer(g_wallpaperWindow, kGifTimerId);
    delete g_image;
    g_image = nullptr;
    g_frameCount = 0;
    g_frameIndex = 0;
    g_frameDelays.clear();
}

void StopPlayback() {
    StopVideo();
    StopImage();
    g_playbackKind = PlaybackKind::None;
}

RECT CalculateDrawRect(int sourceWidth,
                       int sourceHeight,
                       int targetWidth,
                       int targetHeight,
                       bool contain) {
    if (sourceWidth <= 0 || sourceHeight <= 0 || targetWidth <= 0 ||
        targetHeight <= 0) {
        return RECT{0, 0, targetWidth, targetHeight};
    }

    double scaleX = targetWidth / static_cast<double>(sourceWidth);
    double scaleY = targetHeight / static_cast<double>(sourceHeight);
    double scale = contain ? std::min(scaleX, scaleY) : std::max(scaleX, scaleY);
    int width = static_cast<int>(sourceWidth * scale + 0.5);
    int height = static_cast<int>(sourceHeight * scale + 0.5);
    int left = (targetWidth - width) / 2;
    int top = (targetHeight - height) / 2;
    return RECT{left, top, left + width, top + height};
}

void ResizeVideoWindow() {
    if (!g_videoWindow || !g_wallpaperWindow) {
        return;
    }

    RECT client{};
    GetClientRect(g_wallpaperWindow, &client);
    RECT videoRect = client;

    if (g_basicVideo) {
        long videoWidth = 0;
        long videoHeight = 0;
        if (SUCCEEDED(g_basicVideo->GetVideoSize(&videoWidth, &videoHeight)) &&
            videoWidth > 0 && videoHeight > 0) {
            Settings settings = GetSettingsSnapshot();
            videoRect = CalculateDrawRect(
                static_cast<int>(videoWidth),
                static_cast<int>(videoHeight),
                RectWidth(client),
                RectHeight(client),
                settings.stretchMode == L"contain");
        }
    }

    g_videoWindow->SetWindowPosition(videoRect.left, videoRect.top,
                                     RectWidth(videoRect),
                                     RectHeight(videoRect));
}

bool StartVideo(const std::wstring& path, const Settings& settings) {
    StopPlayback();

    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
                                  reinterpret_cast<void**>(&g_graph));
    if (FAILED(hr) || !g_graph) {
        return false;
    }

    hr = g_graph->RenderFile(path.c_str(), nullptr);
    if (FAILED(hr)) {
        StopPlayback();
        return false;
    }

    g_graph->QueryInterface(IID_IMediaControl,
                            reinterpret_cast<void**>(&g_mediaControl));
    g_graph->QueryInterface(IID_IMediaEventEx,
                            reinterpret_cast<void**>(&g_mediaEvent));
    g_graph->QueryInterface(IID_IVideoWindow,
                            reinterpret_cast<void**>(&g_videoWindow));
    g_graph->QueryInterface(IID_IMediaSeeking,
                            reinterpret_cast<void**>(&g_mediaSeeking));
    g_graph->QueryInterface(IID_IBasicAudio,
                            reinterpret_cast<void**>(&g_basicAudio));
    g_graph->QueryInterface(IID_IBasicVideo,
                            reinterpret_cast<void**>(&g_basicVideo));
    if (!g_mediaControl || !g_videoWindow) {
        StopPlayback();
        return false;
    }

    g_videoWindow->put_Owner(reinterpret_cast<OAHWND>(g_wallpaperWindow));
    g_videoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    g_videoWindow->put_MessageDrain(reinterpret_cast<OAHWND>(g_wallpaperWindow));
    g_videoWindow->put_Visible(OATRUE);
    ResizeVideoWindow();

    if (g_basicAudio) {
        g_basicAudio->put_Volume(settings.muteAudio ? -10000 : 0);
    }
    if (g_mediaEvent) {
        g_mediaEvent->SetNotifyWindow(reinterpret_cast<OAHWND>(g_wallpaperWindow),
                                      kMsgGraphEvent, 0);
    }

    g_playbackKind = PlaybackKind::Video;
    if (!g_paused) {
        g_mediaControl->Run();
    }
    return true;
}

UINT GetGifFrameDelay(Gdiplus::Image* image, UINT frameIndex) {
    UINT itemSize = image->GetPropertyItemSize(PropertyTagFrameDelay);
    if (itemSize == 0) {
        return 100;
    }

    std::vector<BYTE> buffer(itemSize);
    auto* item = reinterpret_cast<Gdiplus::PropertyItem*>(buffer.data());
    if (image->GetPropertyItem(PropertyTagFrameDelay, itemSize, item) !=
        Gdiplus::Ok) {
        return 100;
    }

    UINT* delays = reinterpret_cast<UINT*>(item->value);
    UINT hundredths = delays[frameIndex];
    if (hundredths < 2) {
        hundredths = 10;
    }
    return hundredths * 10;
}

bool StartImageOrGif(const std::wstring& path, bool gif) {
    StopPlayback();
    g_image = Gdiplus::Image::FromFile(path.c_str(), FALSE);
    if (!g_image || g_image->GetLastStatus() != Gdiplus::Ok) {
        StopPlayback();
        return false;
    }

    g_playbackKind = gif ? PlaybackKind::Gif : PlaybackKind::Image;
    if (gif) {
        UINT dimensionCount = g_image->GetFrameDimensionsCount();
        if (dimensionCount > 0) {
            std::vector<GUID> dimensions(dimensionCount);
            if (g_image->GetFrameDimensionsList(dimensions.data(),
                                                dimensionCount) ==
                Gdiplus::Ok) {
                g_frameDimension = dimensions[0];
                g_frameCount = g_image->GetFrameCount(&g_frameDimension);
                g_frameDelays.resize(g_frameCount);
                for (UINT i = 0; i < g_frameCount; i++) {
                    g_frameDelays[i] = GetGifFrameDelay(g_image, i);
                }
            }
        }

        if (g_frameCount > 1 && !g_paused) {
            SetTimer(g_wallpaperWindow, kGifTimerId, g_frameDelays[0], nullptr);
        }
    }

    InvalidateRect(g_wallpaperWindow, nullptr, TRUE);
    return true;
}

void ApplyPlaybackPauseState() {
    if (g_playbackKind == PlaybackKind::Video && g_mediaControl) {
        if (g_paused) {
            g_mediaControl->Pause();
        } else {
            g_mediaControl->Run();
        }
    } else if (g_playbackKind == PlaybackKind::Gif && g_frameCount > 1) {
        if (g_paused) {
            KillTimer(g_wallpaperWindow, kGifTimerId);
        } else {
            SetTimer(g_wallpaperWindow, kGifTimerId,
                     g_frameDelays.empty() ? 100 : g_frameDelays[g_frameIndex],
                     nullptr);
        }
    }
}

bool SetWallpaperPath(const std::wstring& inputPath, bool persist) {
    Settings settings = GetSettingsSnapshot();
    std::wstring path = ExpandPath(inputPath);
    if (!FileExists(path)) {
        return false;
    }

    path = PrepareWallpaper(path, settings);
    std::wstring extension = GetExtension(path);

    bool started = false;
    if (IsGifExtension(extension)) {
        started = StartImageOrGif(path, true);
    } else if (IsVideoExtension(extension)) {
        started = StartVideo(path, settings);
    } else if (IsImageExtension(extension)) {
        started = StartImageOrGif(path, false);
    }

    if (!started) {
        return false;
    }

    g_activePath = path;
    if (persist) {
        Wh_SetStringValue(L"wallpaperPath", path.c_str());
    }
    return true;
}

std::wstring ResolveInitialWallpaperPath(const Settings& settings) {
    if (!settings.wallpaperPath.empty() && FileExists(settings.wallpaperPath)) {
        return settings.wallpaperPath;
    }

    if (settings.useStoredWallpaper) {
        std::wstring stored = ExpandPath(GetStoredStringValue(L"wallpaperPath"));
        if (!stored.empty() && FileExists(stored)) {
            return stored;
        }
    }

    if (!settings.defaultWallpaperPath.empty() &&
        FileExists(settings.defaultWallpaperPath)) {
        return settings.defaultWallpaperPath;
    }

    return L"";
}

void ChooseWallpaperFromDialog() {
    wchar_t fileName[32768]{};
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_wallpaperWindow;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = ARRAYSIZE(fileName);
    ofn.lpstrTitle = L"Choose wallpaper";
    ofn.lpstrFilter =
        L"Wallpaper media\0*.mp4;*.wmv;*.avi;*.mov;*.mkv;*.gif;*.jpg;*.jpeg;*.png;*.bmp;*.webp;*.tif;*.tiff\0"
        L"Video\0*.mp4;*.wmv;*.avi;*.mov;*.mkv\0"
        L"Images and GIF\0*.gif;*.jpg;*.jpeg;*.png;*.bmp;*.webp;*.tif;*.tiff\0"
        L"All files\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;

    if (GetOpenFileNameW(&ofn)) {
        SetWallpaperPath(fileName, true);
    }
}

HWND FindDesktopWorker() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    DWORD_PTR result = 0;
    SendMessageTimeoutW(progman, kSpawnWorkerW, 0, 0, SMTO_NORMAL, 1000,
                        &result);

    HWND worker = nullptr;
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            HWND shellView =
                FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
            if (shellView) {
                HWND candidate =
                    FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);
                if (candidate) {
                    *reinterpret_cast<HWND*>(lParam) = candidate;
                    return FALSE;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&worker));

    return worker ? worker : progman;
}

void AttachWallpaperWindowToDesktop(HWND hwnd) {
    HWND desktop = FindDesktopWorker();
    if (desktop) {
        SetParent(hwnd, desktop);
    }

    RECT screen = GetVirtualScreenRect();
    SetWindowPos(hwnd, HWND_BOTTOM, screen.left, screen.top, RectWidth(screen),
                 RectHeight(screen), SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void AddOrUpdateTrayIcon() {
    Settings settings = GetSettingsSnapshot();
    if (!settings.showTrayIcon || !g_wallpaperWindow) {
        if (g_trayAdded) {
            NOTIFYICONDATAW removeData{sizeof(removeData)};
            removeData.hWnd = g_wallpaperWindow;
            removeData.uID = kTrayIconId;
            Shell_NotifyIconW(NIM_DELETE, &removeData);
            g_trayAdded = false;
        }
        return;
    }

    NOTIFYICONDATAW data{sizeof(data)};
    data.hWnd = g_wallpaperWindow;
    data.uID = kTrayIconId;
    data.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    data.uCallbackMessage = kMsgTrayCallback;
    data.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wcscpy_s(data.szTip, L"Mini Wallpaper");

    Shell_NotifyIconW(g_trayAdded ? NIM_MODIFY : NIM_ADD, &data);
    if (!g_trayAdded) {
        data.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &data);
        g_trayAdded = true;
    }
}

void RemoveTrayIcon() {
    if (!g_trayAdded || !g_wallpaperWindow) {
        return;
    }

    NOTIFYICONDATAW data{sizeof(data)};
    data.hWnd = g_wallpaperWindow;
    data.uID = kTrayIconId;
    Shell_NotifyIconW(NIM_DELETE, &data);
    g_trayAdded = false;
}

void ShowTrayMenu() {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, kMenuChoose, L"Choose wallpaper...");
    AppendMenuW(menu, MF_STRING, kMenuPause,
                g_paused ? L"Resume" : L"Pause");
    AppendMenuW(menu, MF_STRING, kMenuReload, L"Reload");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kMenuOpenWindhawk, L"Open Windhawk");
    AppendMenuW(menu, MF_STRING, kMenuQuit, L"Quit");

    POINT point{};
    GetCursorPos(&point);
    SetForegroundWindow(g_wallpaperWindow);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, point.x, point.y, 0,
                   g_wallpaperWindow, nullptr);
    DestroyMenu(menu);
}

void OpenWindhawk() {
    wchar_t programFiles[MAX_PATH]{};
    ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windhawk\\windhawk.exe",
                              programFiles, ARRAYSIZE(programFiles));
    ShellExecuteW(nullptr, L"open", programFiles, nullptr, nullptr,
                  SW_SHOWNORMAL);
}

void HandleGraphEvent() {
    if (!g_mediaEvent) {
        return;
    }

    long eventCode = 0;
    LONG_PTR param1 = 0;
    LONG_PTR param2 = 0;
    while (SUCCEEDED(g_mediaEvent->GetEvent(&eventCode, &param1, &param2, 0))) {
        g_mediaEvent->FreeEventParams(eventCode, param1, param2);
        if (eventCode == EC_COMPLETE && g_mediaSeeking && g_mediaControl) {
            LONGLONG position = 0;
            g_mediaSeeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning,
                                         nullptr, AM_SEEKING_NoPositioning);
            if (!g_paused) {
                g_mediaControl->Run();
            }
        }
    }
}

void AdvanceGifFrame() {
    if (!g_image || g_frameCount <= 1) {
        return;
    }

    g_frameIndex = (g_frameIndex + 1) % g_frameCount;
    g_image->SelectActiveFrame(&g_frameDimension, g_frameIndex);
    InvalidateRect(g_wallpaperWindow, nullptr, FALSE);
    if (!g_paused) {
        SetTimer(g_wallpaperWindow, kGifTimerId, g_frameDelays[g_frameIndex],
                 nullptr);
    }
}

void PaintImage(HWND hwnd, HDC dc) {
    RECT client{};
    GetClientRect(hwnd, &client);
    HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(dc, &client, black);
    DeleteObject(black);

    if (!g_image) {
        return;
    }

    Settings settings = GetSettingsSnapshot();
    bool contain = settings.stretchMode == L"contain";
    UINT width = g_image->GetWidth();
    UINT height = g_image->GetHeight();
    RECT drawRect =
        CalculateDrawRect(static_cast<int>(width), static_cast<int>(height),
                          RectWidth(client), RectHeight(client), contain);

    Gdiplus::Graphics graphics(dc);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.DrawImage(g_image, drawRect.left, drawRect.top,
                       RectWidth(drawRect), RectHeight(drawRect));
}

void LoadInitialWallpaper() {
    Settings settings = GetSettingsSnapshot();
    std::wstring path = ResolveInitialWallpaperPath(settings);
    if (!path.empty()) {
        SetWallpaperPath(path, false);
        return;
    }

    if (settings.autoOpenPicker) {
        ChooseWallpaperFromDialog();
    }
}

LRESULT CALLBACK WallpaperWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_taskbarCreatedMessage) {
        AddOrUpdateTrayIcon();
        return 0;
    }

    switch (msg) {
        case WM_CREATE:
            return 0;

        case WM_SIZE:
            ResizeVideoWindow();
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);
            if (g_playbackKind == PlaybackKind::Image ||
                g_playbackKind == PlaybackKind::Gif ||
                g_playbackKind == PlaybackKind::None) {
                PaintImage(hwnd, dc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_TIMER:
            if (wParam == kGifTimerId) {
                AdvanceGifFrame();
                return 0;
            }
            break;

        case kMsgGraphEvent:
            HandleGraphEvent();
            return 0;

        case kMsgTrayCallback:
            if (LOWORD(lParam) == WM_CONTEXTMENU ||
                LOWORD(lParam) == WM_RBUTTONUP ||
                LOWORD(lParam) == NIN_SELECT) {
                ShowTrayMenu();
            }
            return 0;

        case kMsgReloadSettings: {
            std::wstring previousSetting = GetSettingsSnapshot().wallpaperPath;
            LoadSettings();
            AddOrUpdateTrayIcon();
            Settings settings = GetSettingsSnapshot();
            if (!settings.wallpaperPath.empty() &&
                settings.wallpaperPath != previousSetting) {
                SetWallpaperPath(settings.wallpaperPath, false);
            } else if (g_playbackKind == PlaybackKind::Video && g_basicAudio) {
                g_basicAudio->put_Volume(settings.muteAudio ? -10000 : 0);
            }
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case kMenuChoose:
                    ChooseWallpaperFromDialog();
                    return 0;
                case kMenuPause:
                    g_paused = !g_paused;
                    ApplyPlaybackPauseState();
                    return 0;
                case kMenuReload: {
                    Settings settings = GetSettingsSnapshot();
                    std::wstring path = ResolveInitialWallpaperPath(settings);
                    if (!path.empty()) {
                        SetWallpaperPath(path, false);
                    }
                    return 0;
                }
                case kMenuOpenWindhawk:
                    OpenWindhawk();
                    return 0;
                case kMenuQuit:
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;

        case WM_DESTROY:
            RemoveTrayIcon();
            StopPlayback();
            g_wallpaperWindow = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool RegisterWallpaperWindowClass() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WallpaperWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWallpaperWindowClass;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    return RegisterClassExW(&wc) ||
           GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

DWORD WINAPI WorkerThreadProc(void*) {
    g_workerThreadId = GetCurrentThreadId();
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    Gdiplus::GdiplusStartupInput gdiplusInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);
    LoadSettings();
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    if (!RegisterWallpaperWindowClass()) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        return 1;
    }

    RECT screen = GetVirtualScreenRect();
    g_wallpaperWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWallpaperWindowClass,
        L"Mini Wallpaper", WS_POPUP | WS_VISIBLE, screen.left, screen.top,
        RectWidth(screen), RectHeight(screen), nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (!g_wallpaperWindow) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return 1;
    }

    AttachWallpaperWindowToDesktop(g_wallpaperWindow);
    AddOrUpdateTrayIcon();
    LoadInitialWallpaper();

    MSG msg{};
    while (!g_stopWorker.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_wallpaperWindow && IsWindow(g_wallpaperWindow)) {
        DestroyWindow(g_wallpaperWindow);
    }
    StopPlayback();
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_stopWorker = false;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_wallpaperWindow) {
        PostMessageW(g_wallpaperWindow, kMsgReloadSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_stopWorker = true;
    if (g_wallpaperWindow) {
        PostMessageW(g_wallpaperWindow, WM_CLOSE, 0, 0);
    }
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 10000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_toolModProcessMutex) {
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
}

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

enum class ToolProcessKind {
    NormalExplorer,
    OtherToolMod,
    CurrentToolMod,
    Excluded,
};

ToolProcessKind GetToolProcessKind() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        return ToolProcessKind::NormalExplorer;
    }

    ToolProcessKind kind = ToolProcessKind::NormalExplorer;
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            kind = ToolProcessKind::Excluded;
            break;
        }
    }

    if (kind == ToolProcessKind::NormalExplorer) {
        for (int i = 1; i < argc - 1; i++) {
            if (wcscmp(argv[i], L"-tool-mod") == 0) {
                kind = wcscmp(argv[i + 1], WH_MOD_ID) == 0
                           ? ToolProcessKind::CurrentToolMod
                           : ToolProcessKind::OtherToolMod;
                break;
            }
        }
    }

    LocalFree(argv);
    return kind;
}

bool LaunchToolModProcess() {
    WCHAR currentProcessPath[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, currentProcessPath,
                                      ARRAYSIZE(currentProcessPath));
    if (length == 0 || length == ARRAYSIZE(currentProcessPath)) {
        return false;
    }

    WCHAR commandLine[MAX_PATH + 64]{};
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
    }
    if (!kernelModule) {
        return false;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW,
        LPPROCESS_INFORMATION, PHANDLE);

    auto createProcessInternal =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternal) {
        return false;
    }

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION processInfo{};
    if (!createProcessInternal(nullptr, currentProcessPath, commandLine, nullptr,
                               nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr,
                               nullptr, &startupInfo, &processInfo, nullptr)) {
        return false;
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    ToolProcessKind kind = GetToolProcessKind();
    if (kind == ToolProcessKind::Excluded ||
        kind == ToolProcessKind::OtherToolMod) {
        return FALSE;
    }

    if (kind == ToolProcessKind::CurrentToolMod) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex ||
            GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        auto* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) +
            ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
        return TRUE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isToolModProcessLauncher) {
        LaunchToolModProcess();
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}
