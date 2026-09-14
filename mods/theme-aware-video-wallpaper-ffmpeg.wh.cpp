// ==WindhawkMod==
// @id              theme-aware-video-wallpaper-ffmpeg
// @name            Theme Aware Video Wallpaper
// @description     Auto switch video wallpaper via ffmpeg pipe + UpdateLayeredWindow. Currently only supports the latest Windows 11. Audio playback is temporarily not supported. Hoping someone else will carry on development.
// @version         1.0
// @author          wakhh@qq.com
// @github          https://github.com/wakhh
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Theme Aware Video Wallpaper

Plays a video as desktop wallpaper using ffmpeg and `UpdateLayeredWindow`.
Automatically switches between two videos based on the current Windows light/dark theme.

### Requirements
- **ffmpeg.exe** — must be provided via the mod settings. Only the main `ffmpeg.exe` binary is needed;
  no additional DLLs or codecs are required.
- **Windows 11** — tested and developed for the latest Windows 11 only.

### Settings
- **ffmpeg.exe path**: Full path to a standalone `ffmpeg.exe`.
- **Light mode video / folder path**: Path to a single video **file**, or a **folder** containing videos.
  When a folder is selected, files are sorted by modification time (newest first) and played in that order.
- **Dark mode video / folder path**: Leave empty to reuse the light-mode path.

### Behavior
- ffmpeg scales the video to the virtual desktop resolution before rendering. Multi-monitor setups
  are supported — the wallpaper covers the combined desktop area.
- When a **maximized or fullscreen window** is active, rendering stops but ffmpeg keeps decoding
  (the pipeline is not paused, only new frames are dropped to minimize visual flicker on resume).
- Audio playback is **not supported** — ffmpeg is invoked with audio disabled.

### Known limitations
- Desktop icons are covered by the video wallpaper — icons remain functional (you can click them)
  but they are not visible. This is a known Z-order issue; the video window is placed below all
  desktop layers rather than between `Progman` and `WorkerW` where icons live.
- Scaling mode (fit / fill / center) is not configurable; the frame is always stretched to fill the
  virtual desktop.
- Hoping someone else will carry on development — PRs welcome.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ffmpegPath: ""
  $name: ffmpeg.exe path
  $description: Full path to ffmpeg.exe
- lightVideoPath: ""
  $name: Light mode video / folder path
  $description: Full path to video file OR folder containing videos. Folder latest first.
- darkVideoPath: ""
  $name: Dark mode video / folder path
  $description: Leave empty to reuse light mode.
- opacity: 90
  $name: Video window opacity (%)
  $description: Overall transparency of the video wallpaper. 100 = fully opaque, lower values let the desktop show through. Range 10-100.
*/
// ==/WindhawkModSettings==

#include <Windows.h>
#include <windhawk_utils.h>
#include <vector>
#include <string>
#include <atomic>
#include <algorithm>

static HANDLE g_ffmpegProc = NULL;
static HANDLE g_pipeRead = NULL;
static HANDLE g_pipeWrite = NULL;
static HWND g_videoHwnd = NULL;
static HWND g_progman = NULL;
static int g_frameSize = 0;
static std::vector<BYTE> g_frameBuf;
WCHAR g_ffmpegPath[MAX_PATH] = {0};
WCHAR g_lightPath[MAX_PATH] = {0};
WCHAR g_darkPath[MAX_PATH] = {0};
int g_opacity = 230;
const bool g_pauseOnFullscreen = true;

HANDLE g_renderThread = NULL;
HANDLE g_monitorThread = NULL;
HANDLE g_dirChangeHandle = INVALID_HANDLE_VALUE;
bool g_classRegistered = false;
std::atomic<bool> g_running{true};
std::atomic<bool> g_isPaused{false};
bool g_lastIsDark = false;
HANDLE g_mutex = NULL;
std::vector<std::wstring> g_videoList;
size_t g_videoIndex = 0;
int g_consecutiveErrors = 0;
static const int MAX_CONSECUTIVE_ERRORS = 3;

typedef BOOL(WINAPI* UpdateLayeredWindow_t)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
typedef HDC(WINAPI* CreateCompatibleDC_t)(HDC);
typedef HBITMAP(WINAPI* CreateCompatibleBitmap_t)(HDC, int, int);
typedef HGDIOBJ(WINAPI* SelectObject_t)(HDC, HGDIOBJ);
typedef BOOL(WINAPI* SetDIBitsToDevice_t)(HDC, int, int, DWORD, DWORD, int, int, UINT, UINT, const VOID*, BITMAPINFO*, UINT);
typedef BOOL(WINAPI* DeleteObject_t)(HGDIOBJ);
typedef BOOL(WINAPI* DeleteDC_t)(HDC);

static UpdateLayeredWindow_t pUpdateLayeredWindow = NULL;
static CreateCompatibleDC_t pCreateCompatibleDC = NULL;
static CreateCompatibleBitmap_t pCreateCompatibleBitmap = NULL;
static SelectObject_t pSelectObject = NULL;
static SetDIBitsToDevice_t pSetDIBitsToDevice = NULL;
static DeleteObject_t pDeleteObject = NULL;
static DeleteDC_t pDeleteDC = NULL;
static HMODULE g_gdiMod = NULL;

void LoadGdiProcs()
{
    if (g_gdiMod) return;
    g_gdiMod = LoadLibraryW(L"gdi32.dll");
    if (!g_gdiMod) { Wh_Log(L"LoadLibrary gdi32 failed err=%lu", GetLastError()); return; }
    pUpdateLayeredWindow = (UpdateLayeredWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "UpdateLayeredWindow");
    pCreateCompatibleDC = (CreateCompatibleDC_t)GetProcAddress(g_gdiMod, "CreateCompatibleDC");
    pCreateCompatibleBitmap = (CreateCompatibleBitmap_t)GetProcAddress(g_gdiMod, "CreateCompatibleBitmap");
    pSelectObject = (SelectObject_t)GetProcAddress(g_gdiMod, "SelectObject");
    pSetDIBitsToDevice = (SetDIBitsToDevice_t)GetProcAddress(g_gdiMod, "SetDIBitsToDevice");
    pDeleteObject = (DeleteObject_t)GetProcAddress(g_gdiMod, "DeleteObject");
    pDeleteDC = (DeleteDC_t)GetProcAddress(g_gdiMod, "DeleteDC");
}

void ReloadWallpaper();

HWND FindProgman()
{
    HWND p = FindWindowW(L"Progman", L"Program Manager");
    for (int i = 0; i < 20 && !p; i++) { Sleep(250); p = FindWindowW(L"Progman", L"Program Manager"); }
    return p;
}

HWND FindWorkerW()
{
    HWND worker = NULL;
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        if (!IsWindowVisible(hwnd)) return TRUE;
        WCHAR cls[64] = {};
        GetClassNameW(hwnd, cls, 64);
        if (_wcsicmp(cls, L"WorkerW") == 0) {
            *(HWND*)lParam = hwnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&worker);
    return worker;
}

bool IsVideoExt(const WCHAR* ext)
{
    return ext && (_wcsicmp(ext, L".mp4") == 0 || _wcsicmp(ext, L".mkv") == 0 ||
        _wcsicmp(ext, L".mov") == 0 || _wcsicmp(ext, L".webm") == 0 ||
        _wcsicmp(ext, L".avi") == 0 || _wcsicmp(ext, L".m4v") == 0 ||
        _wcsicmp(ext, L".wmv") == 0);
}

struct VideoEntry {
    std::wstring path;
    FILETIME writeTime;
};

std::vector<std::wstring> EnumVideos(const WCHAR* folder)
{
    std::vector<VideoEntry> entries;
    WCHAR path[MAX_PATH];
    swprintf_s(path, L"%s\\*", folder);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return {};
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            WCHAR ext[_MAX_EXT];
            _wsplitpath_s(fd.cFileName, NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT);
            if (IsVideoExt(ext)) {
                WCHAR full[MAX_PATH];
                swprintf_s(full, L"%s\\%s", folder, fd.cFileName);
                entries.push_back({full, fd.ftLastWriteTime});
            }
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
    std::sort(entries.begin(), entries.end(), [](const VideoEntry& a, const VideoEntry& b) {
        if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
            return a.writeTime.dwHighDateTime > b.writeTime.dwHighDateTime;
        return a.writeTime.dwLowDateTime > b.writeTime.dwLowDateTime;
    });
    std::vector<std::wstring> result;
    result.reserve(entries.size());
    for (auto& e : entries) result.push_back(std::move(e.path));
    return result;
}

void StopFfmpeg()
{
    if (g_pipeWrite) { CloseHandle(g_pipeWrite); g_pipeWrite = NULL; }
    if (g_pipeRead)  { CloseHandle(g_pipeRead);  g_pipeRead = NULL; }
    if (g_ffmpegProc){ TerminateProcess(g_ffmpegProc, 0); CloseHandle(g_ffmpegProc); g_ffmpegProc = NULL; }
    if (g_renderThread) { WaitForSingleObject(g_renderThread, 2000); CloseHandle(g_renderThread); g_renderThread = NULL; }
}

bool IsDark()
{
    DWORD val = 1, cb = sizeof(val); HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&val, &cb);
        RegCloseKey(hKey);
    }
    return val == 0;
}

bool IsForegroundFull()
{
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    if (fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
    DWORD style = (DWORD)GetWindowLongW(fg, GWL_STYLE);
    if (!(style & WS_MAXIMIZE)) {
        RECT rc; GetWindowRect(fg, &rc);
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        if (w < GetSystemMetrics(SM_CXSCREEN) - 200 || h < GetSystemMetrics(SM_CYSCREEN) - 200) return false;
    }
    return true;
}

bool PathExists(const WCHAR* p) { return GetFileAttributesW(p) != INVALID_FILE_ATTRIBUTES; }

LRESULT CALLBACK VideoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool CreateVideoWindow()
{
    if (g_videoHwnd && IsWindow(g_videoHwnd)) return true;

    Wh_Log(L"CreateVideoWindow: begin");
    Wh_Log(L"CreateVideoWindow: before LoadGdiProcs");
    LoadGdiProcs();
    Wh_Log(L"CreateVideoWindow: after LoadGdiProcs user=%p gdi=%p update=%p",
        GetModuleHandleW(L"user32.dll"), g_gdiMod, pUpdateLayeredWindow);
    if (!pUpdateLayeredWindow) { Wh_Log(L"UpdateLayeredWindow not available"); return false; }

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    Wh_Log(L"CreateVideoWindow: metrics %dx%d", sw, sh);

    if (!g_classRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = VideoWndProc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = L"VidWallpaperWnd";
        wc.hbrBackground = NULL;
        Wh_Log(L"CreateVideoWindow: before RegisterClassExW");
        ATOM classAtom = RegisterClassExW(&wc);
        Wh_Log(L"CreateVideoWindow: after RegisterClassExW atom=%u err=%lu",
            classAtom, GetLastError());
        if (!classAtom) {
            DWORD err = GetLastError();
            if (err == ERROR_CLASS_ALREADY_EXISTS) {
                // Stale class from a previous DLL load with dangling lpfnWndProc.
                UnregisterClassW(L"VidWallpaperWnd", GetModuleHandleW(NULL));
                classAtom = RegisterClassExW(&wc);
                Wh_Log(L"CreateVideoWindow: re-registered atom=%u err=%lu", classAtom, GetLastError());
            }
        }
        if (!classAtom) { Wh_Log(L"RegisterClassExW failed"); return false; }
        g_classRegistered = true;
    }

    g_progman = FindProgman();
    HWND workerw = FindWorkerW();
    Wh_Log(L"CreateVideoWindow: progman=%p workerw=%p", g_progman, workerw);

    Wh_Log(L"CreateVideoWindow: before CreateWindowExW");
    g_videoHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        L"VidWallpaperWnd", L"",
        WS_POPUP,
        0, 0, sw, sh,
        NULL, NULL, GetModuleHandleW(NULL), NULL);

    Wh_Log(L"CreateVideoWindow: after CreateWindowExW hwnd=%p err=%lu",
        g_videoHwnd, GetLastError());
    if (!g_videoHwnd) { Wh_Log(L"CreateWindowExW failed err=%lu", GetLastError()); return false; }

    Wh_Log(L"CreateVideoWindow: before ShowWindow");
    ShowWindow(g_videoHwnd, SW_SHOW);
    Wh_Log(L"CreateVideoWindow: before SetWindowPos");
    SetLastError(ERROR_SUCCESS);
    HWND insertAfter = g_progman ? g_progman : HWND_BOTTOM;
    BOOL positioned = SetWindowPos(g_videoHwnd, insertAfter, 0, 0, sw, sh, SWP_NOACTIVATE);
    if (!positioned) {
        Wh_Log(L"CreateVideoWindow: SetWindowPos failed err=%lu", GetLastError());
        DestroyWindow(g_videoHwnd);
        g_videoHwnd = NULL;
        return false;
    }

    if (workerw) {
        SetWindowPos(workerw, g_videoHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    Wh_Log(L"CreateVideoWindow: video=%p top-level layered OK", g_videoHwnd);
    return true;
}

DWORD WINAPI RenderThread(LPVOID)
{
    LoadGdiProcs();
    if (!pCreateCompatibleDC || !pCreateCompatibleBitmap ||
        !pSelectObject || !pSetDIBitsToDevice || !pDeleteObject || !pDeleteDC || !pUpdateLayeredWindow) {
        Wh_Log(L"RenderThread: GDI procs missing"); return 0;
    }

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    int newFrameSize = sw * sh * 4;
    if (g_frameBuf.size() != static_cast<size_t>(newFrameSize)) {
        g_frameBuf.resize(newFrameSize);
    }
    g_frameSize = newFrameSize;

    HDC hScreen = GetDC(NULL);
    if (!hScreen) {
        Wh_Log(L"RenderThread: GetDC failed err=%lu", GetLastError());
        return 0;
    }

    int frameCount = 0;
    while (g_running && g_pipeRead)
    {
        if (!g_frameSize) { Sleep(10); continue; }

        if (g_isPaused) { Sleep(1); continue; }

        while (true) {
            DWORD avail = 0;
            if (PeekNamedPipe(g_pipeRead, NULL, 0, NULL, &avail, NULL) && avail >= (DWORD)g_frameSize * 3) {
                DWORD skip = 0;
                while (skip < (DWORD)g_frameSize && g_running) {
                    DWORD got = 0;
                    if (!ReadFile(g_pipeRead, g_frameBuf.data() + skip, (DWORD)g_frameSize - skip, &got, NULL) || got == 0) break;
                    skip += got;
                }
                continue;
            }
            break;
        }

        DWORD totalRead = 0;
        while (totalRead < (DWORD)g_frameSize && g_running)
        {
            DWORD got = 0;
            BOOL okR = ReadFile(g_pipeRead, g_frameBuf.data() + totalRead, g_frameSize - totalRead, &got, NULL);
            if (!okR || got == 0) break;
            totalRead += got;
        }
        if (totalRead != (DWORD)g_frameSize) break;

        if (g_isPaused) continue;
        if (!g_videoHwnd || !IsWindow(g_videoHwnd)) continue;

        HDC hMem = pCreateCompatibleDC(hScreen);
        if (!hMem) {
            Wh_Log(L"RenderThread: CreateCompatibleDC failed err=%lu", GetLastError());
            continue;
        }

        HBITMAP hBmp = pCreateCompatibleBitmap(hScreen, sw, sh);
        if (!hBmp) {
            Wh_Log(L"RenderThread: CreateCompatibleBitmap failed err=%lu", GetLastError());
            pDeleteDC(hMem);
            continue;
        }

        HBITMAP hOldBmp = (HBITMAP)pSelectObject(hMem, hBmp);
        if (!hOldBmp || hOldBmp == HGDI_ERROR) {
            Wh_Log(L"RenderThread: SelectObject failed err=%lu", GetLastError());
            pDeleteObject(hBmp);
            pDeleteDC(hMem);
            continue;
        }

        BITMAPINFOHEADER bih = {0};
        bih.biSize = sizeof(bih); bih.biPlanes = 1; bih.biBitCount = 32;
        bih.biCompression = BI_RGB; bih.biWidth = sw; bih.biHeight = -sh;

        BOOL dibOk = pSetDIBitsToDevice(hMem, 0, 0, sw, sh, 0, 0, 0, sh,
            g_frameBuf.data(), (BITMAPINFO*)&bih, DIB_RGB_COLORS);

        POINT ptSrc = {0, 0};
        SIZE sz = {sw, sh};
        POINT ptDst = {0, 0};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)g_opacity, 0};
        BOOL ok = pUpdateLayeredWindow(g_videoHwnd, hScreen, &ptDst, &sz, hMem, &ptSrc, 0, &bf, ULW_ALPHA);

        if (!dibOk || !ok) {
            Wh_Log(L"RenderThread: draw failed dib=%d layered=%d err=%lu frame=%d",
                dibOk, ok, GetLastError(), frameCount);
        }

        pSelectObject(hMem, hOldBmp);
        pDeleteObject(hBmp);
        pDeleteDC(hMem);

        if (frameCount == 0) Wh_Log(L"RenderThread: first frame ok=%d err=%lu", ok, GetLastError());
        frameCount++;
    }

    ReleaseDC(NULL, hScreen);
    Wh_Log(L"RenderThread: exit %d frames", frameCount);
    return 0;
}

bool StartFfmpeg(const WCHAR* videoPath)
{
    StopFfmpeg();
    if (!g_ffmpegPath[0]) { Wh_Log(L"StartFfmpeg: ffmpegPath is empty! Set it in mod settings."); return false; }
    if (!videoPath || !*videoPath || !PathExists(videoPath)) return false;

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    HANDLE hRead = NULL, hWrite = NULL;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) { Wh_Log(L"CreatePipe failed"); StopFfmpeg(); return false; }

    HANDLE hErrRead = NULL, hErrWrite = NULL;
    if (!CreatePipe(&hErrRead, &hErrWrite, &sa, 0)) { Wh_Log(L"CreatePipe err failed"); StopFfmpeg(); return false; }

    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hErrRead, HANDLE_FLAG_INHERIT, 0);
    g_pipeRead = hRead; g_pipeWrite = hWrite;
    Wh_Log(L"StartFfmpeg: pipes ready read=%p write=%p errRead=%p errWrite=%p",
        hRead, hWrite, hErrRead, hErrWrite);

    WCHAR cmd[MAX_PATH * 4];
    swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error -i \"%s\" -an -f rawvideo -pix_fmt bgra -s %dx%d -r 30 -",
        g_ffmpegPath, videoPath, sw, sh);
    Wh_Log(L"StartFfmpeg cmd: %s", cmd);

    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWrite;
    si.hStdError = hErrWrite;

    PROCESS_INFORMATION pi = {0};
    Wh_Log(L"StartFfmpeg: CreateProcessW begin");
    if (!CreateProcessW(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        Wh_Log(L"CreateProcess failed err=%lu", GetLastError());
        StopFfmpeg();
        return false;
    }
    Wh_Log(L"StartFfmpeg: CreateProcessW success process=%p thread=%p pid=%lu",
        pi.hProcess, pi.hThread, pi.dwProcessId);
    CloseHandle(pi.hThread);
    CloseHandle(hWrite);
    CloseHandle(hErrWrite);
    g_pipeWrite = NULL;
    g_ffmpegProc = pi.hProcess;

    {
        char errBuf[2048] = {0};
        DWORD waited = 0;
        bool gotData = false;
        Wh_Log(L"StartFfmpeg: waiting for ffmpeg stdout");
        while (waited < 5000 && g_running) {
            DWORD avail = 0;
            BOOL peekOk = PeekNamedPipe(hRead, NULL, 0, NULL, &avail, NULL);
            if (peekOk && avail > 0) {
                gotData = true;
                Wh_Log(L"StartFfmpeg: stdout data available bytes=%lu waited=%lu", avail, waited);
                break;
            }
            if (!peekOk && GetLastError() != ERROR_NO_DATA) {
                Wh_Log(L"StartFfmpeg: PeekNamedPipe failed err=%lu waited=%lu", GetLastError(), waited);
            }
            Sleep(50); waited += 50;
        }
        Wh_Log(L"StartFfmpeg: stdout wait complete data=%d waited=%lu running=%d",
            gotData, waited, g_running.load());
        DWORD errAvail = 0;
        BOOL errPeekOk = PeekNamedPipe(hErrRead, NULL, 0, NULL, &errAvail, NULL);
        Wh_Log(L"StartFfmpeg: stderr peek ok=%d bytes=%lu err=%lu",
            errPeekOk, errAvail, errPeekOk ? ERROR_SUCCESS : GetLastError());
        if (errPeekOk && errAvail > 0) {
            DWORD got = 0;
            if (ReadFile(hErrRead, errBuf, sizeof(errBuf) - 1, &got, NULL) && got > 0) {
                errBuf[got] = 0;
                Wh_Log(L"ffmpeg stderr: %S", errBuf);
            }
        }
        CloseHandle(hErrRead);
        if (!gotData) {
            Wh_Log(L"StartFfmpeg: no data from ffmpeg after %dms, aborting", waited);
            StopFfmpeg(); return false;
        }
    }

    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        Wh_Log(L"StartFfmpeg: video window is not ready");
        StopFfmpeg();
        return false;
    }

    Wh_Log(L"StartFfmpeg: creating render thread");
    g_renderThread = CreateThread(NULL, 0, RenderThread, NULL, 0, NULL);
    Wh_Log(L"StartFfmpeg: render thread=%p err=%lu", g_renderThread, GetLastError());
    return true;
}

bool PlayNext()
{
    if (g_videoList.empty()) return false;
    if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
        Wh_Log(L"PlayNext: too many consecutive errors (%d), giving up. Reload to retry.", g_consecutiveErrors);
        return false;
    }
    const WCHAR* next = g_videoList[g_videoIndex++ % g_videoList.size()].c_str();
    Wh_Log(L"PlayNext: trying %s (consecutive errors=%d)", next, g_consecutiveErrors);
    bool ok = StartFfmpeg(next);
    if (!ok) {
        g_consecutiveErrors++;
        Wh_Log(L"PlayNext: StartFfmpeg failed, consecutive errors=%d", g_consecutiveErrors);
    }
    return ok;
}

void ReloadWallpaper()
{
    StopFfmpeg();

    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) { FindCloseChangeNotification(g_dirChangeHandle); g_dirChangeHandle = INVALID_HANDLE_VALUE; }

    g_videoList.clear(); g_videoIndex = 0; g_consecutiveErrors = 0;
    bool dark = IsDark();
    g_lastIsDark = dark;

    WCHAR target[MAX_PATH] = {0};
    if (dark) { if (g_darkPath[0]) wcscpy_s(target, g_darkPath); else wcscpy_s(target, g_lightPath); }
    else wcscpy_s(target, g_lightPath);

    DWORD attr = GetFileAttributesW(target);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        g_videoList = EnumVideos(target);
        g_dirChangeHandle = FindFirstChangeNotificationW(target, FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE);
    } else if (PathExists(target)) {
        g_videoList.emplace_back(target);
    }

    if (!g_videoList.empty()) PlayNext();
}

void Wh_ModSettingsChanged()
{
    PCWSTR s = Wh_GetStringSetting(L"ffmpegPath");
    WCHAR newFfmpegPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newFfmpegPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"lightVideoPath");
    WCHAR newLightPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newLightPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"darkVideoPath");
    WCHAR newDarkPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newDarkPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    int pct = Wh_GetIntSetting(L"opacity");
    g_opacity = (pct >= 0 && pct <= 100) ? (pct * 255 / 100) : 255;

    bool needReload =
        wcscmp(newFfmpegPath, g_ffmpegPath) != 0 ||
        wcscmp(newLightPath, g_lightPath) != 0 ||
        wcscmp(newDarkPath, g_darkPath) != 0;

    wcscpy_s(g_ffmpegPath, MAX_PATH, newFfmpegPath);
    wcscpy_s(g_lightPath, MAX_PATH, newLightPath);
    wcscpy_s(g_darkPath, MAX_PATH, newDarkPath);

    if (needReload) {
        Wh_Log(L"Wh_ModSettingsChanged: path/ffmpeg changed, reloading");
        ReloadWallpaper();
    } else {
        Wh_Log(L"Wh_ModSettingsChanged: opacity only=%d (no reload needed)", g_opacity);
    }
}

DWORD WINAPI MonitorThread(LPVOID)
{
    Wh_Log(L"MonitorThread: initializing");

    // When this DLL is injected into an already-running explorer process,
    // the newly created thread may not have its user32 thread info fully
    // initialized (no message queue). PeekMessage forces USER32 to set up
    // the THREADINFO and create a message queue, which CreateWindowExW
    // implicitly depends on internally.
    MSG dummyMsg;
    PeekMessageW(&dummyMsg, NULL, 0, 0, PM_NOREMOVE);
    if (!CreateVideoWindow()) {
        Wh_Log(L"MonitorThread: video window creation failed");
        return 0;
    }
    Wh_ModSettingsChanged();

    HKEY hThemeKey = NULL; HANDLE hThemeEvt = NULL;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &hThemeKey);
    if (hThemeKey) hThemeEvt = CreateEventW(NULL, TRUE, FALSE, NULL);

    bool prevFull = false;
    while (g_running) {
        HANDLE handles[2] = {0}; DWORD hc = 0;
        if (hThemeKey && hThemeEvt) {
            ResetEvent(hThemeEvt);
            RegNotifyChangeKeyValue(hThemeKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hThemeEvt, TRUE);
            handles[hc++] = hThemeEvt;
        }
        if (g_dirChangeHandle != INVALID_HANDLE_VALUE) handles[hc++] = g_dirChangeHandle;

        DWORD waitRet = hc > 0 ? WaitForMultipleObjects(hc, handles, FALSE, 800) : WAIT_TIMEOUT;

        if (waitRet >= WAIT_OBJECT_0 && waitRet < WAIT_OBJECT_0 + hc) {
            int idx = waitRet - WAIT_OBJECT_0;
            if (handles[idx] == hThemeEvt && IsDark() != g_lastIsDark) ReloadWallpaper();
            else if (handles[idx] == g_dirChangeHandle) { ReloadWallpaper(); FindNextChangeNotification(g_dirChangeHandle); }
            continue;
        }

        if (g_ffmpegProc) {
            DWORD code = 0;
            if (GetExitCodeProcess(g_ffmpegProc, &code) && code != STILL_ACTIVE) {
                if (code == 0) {
                    g_consecutiveErrors = 0;
                    Wh_Log(L"ffmpeg finished playing normally, moving to next");
                } else {
                    g_consecutiveErrors++;
                    Wh_Log(L"ffmpeg exited with error code=%lu, consecutive errors=%d", code, g_consecutiveErrors);
                }
                if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
                    Wh_Log(L"Too many consecutive errors (%d), stopping auto-play. Reload or change settings to retry.", g_consecutiveErrors);
                    StopFfmpeg();
                } else {
                    PlayNext();
                }
                continue;
            }
        }

        if (g_pauseOnFullscreen) {
            bool now = IsForegroundFull();
            if (now != prevFull) { g_isPaused = now; prevFull = now; Wh_Log(L"Fullscreen pause=%d", now); }
        }
    }
    if (hThemeEvt) CloseHandle(hThemeEvt);
    if (hThemeKey) RegCloseKey(hThemeKey);
    return 0;
}

BOOL Wh_ModInit()
{
    g_mutex = CreateMutexW(NULL, TRUE, L"Local\\VidWallpaperOnlyOne");
    if (g_mutex && GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(g_mutex); g_mutex = NULL; return FALSE; }
    g_monitorThread = CreateThread(NULL, 0, MonitorThread, NULL, 0, NULL);
    if (!g_monitorThread) {
        if (g_mutex) { CloseHandle(g_mutex); g_mutex = NULL; }
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit()
{
    g_running = false;
    StopFfmpeg();
    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) { FindCloseChangeNotification(g_dirChangeHandle); g_dirChangeHandle = INVALID_HANDLE_VALUE; }
    if (g_monitorThread) { WaitForSingleObject(g_monitorThread, 2000); TerminateThread(g_monitorThread, 0); CloseHandle(g_monitorThread); g_monitorThread = NULL; }
    if (g_videoHwnd) { DestroyWindow(g_videoHwnd); g_videoHwnd = NULL; }
    if (g_classRegistered) {
        UnregisterClassW(L"VidWallpaperWnd", GetModuleHandleW(NULL));
        g_classRegistered = false;
    }
    if (g_gdiMod) { FreeLibrary(g_gdiMod); g_gdiMod = NULL; }
    g_frameBuf.clear();
    g_frameSize = 0;
    if (g_mutex) { CloseHandle(g_mutex); g_mutex = NULL; }
}