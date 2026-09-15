// ==WindhawkMod==
// @id              theme-aware-video-wallpaper-ffmpeg
// @name            Video Wallpaper (Follow Theme)
// @name:zh-CN      视频壁纸（跟随主题）
// @description     Auto switch video wallpaper based on Windows theme. Currently only tested on the latest Windows 11. Desktop icon layering issue and audio playback not yet resolved. Community contributions welcome.
// @description:zh-CN  根据 Windows 主题自动切换深浅色视频。仅在最新版 Win11 进行测试，尚未解决桌面图标层级问题和声音播放需求。有待大家一起开发。
// @version         1.0
// @author          wakhh@qq.com
// @github          https://github.com/wakhh
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Theme Aware Video Wallpaper

Plays a video as desktop wallpaper via ffmpeg pipe + UpdateLayeredWindow.
Auto-switches between light/dark mode videos based on Windows theme.

### Requirements
- **ffmpeg.exe** (standalone binary, no extra DLLs/codecs needed)
- Better developers.

Hoping someone else will carry on development — PRs welcome.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ffmpegPath: ""
  $name: ffmpeg.exe path
  $name:zh-CN: ffmpeg.exe 路径
  $description: Full path to ffmpeg.exe
  $description:zh-CN: ffmpeg.exe 的完整路径
- lightVideoPath: ""
  $name: Light mode video / folder path
  $name:zh-CN: 浅色模式视频 / 文件夹路径
  $description: "Full path to video file OR folder containing videos (folder plays by latest modified time)"
  $description:zh-CN: "视频文件完整路径，或包含视频的文件夹路径（文件夹按修改时间最新排序播放）"
- darkVideoPath: ""
  $name: Dark mode video / folder path
  $name:zh-CN: 深色模式视频 / 文件夹路径
  $description: "Leave empty to reuse light mode"
  $description:zh-CN: "留空则复用浅色模式设置"
- sortMode: "0"
  $name: Folder playback sort order
  $name:zh-CN: 文件夹播放排序方式
  $description: "Sort order when playing videos from a folder"
  $description:zh-CN: "文件夹模式下视频的排序方式"
  $options:
    - "0": "Modified time (newest first)"
    - "1": "Modified time (oldest first)"
    - "2": "Name (A to Z)"
    - "3": "Name (Z to A)"
    - "4": "Created time (oldest first)"
    - "5": "Created time (newest first)"
    - "6": "Size (largest first)"
    - "7": "Size (smallest first)"
  $options:zh-CN:
    - "0": "修改时间（新到旧）"
    - "1": "修改时间（旧到新）"
    - "2": "文件名（A 到 Z）"
    - "3": "文件名（Z 到 A）"
    - "4": "创建时间（旧到新）"
    - "5": "创建时间（新到旧）"
    - "6": "文件大小（大到小）"
    - "7": "文件大小（小到大）"
- opacity: 90
  $name: Video window opacity (%)
  $name:zh-CN: 视频窗口透明度（%）
  $description: "Overall transparency of the video wallpaper. 100 = fully opaque, lower values let the desktop show through. Range 10-100"
  $description:zh-CN: "视频壁纸的整体透明度。100 = 完全不透明，值越低桌面越明显。范围 10-100"
- fps: 15
  $name: Frame rate (fps)
  $name:zh-CN: 帧率（fps）
  $description: "Output frame rate. Range 10-60. Lower values save CPU. Videos with a lower native fps will have frames duplicated"
  $description:zh-CN: "输出帧率。范围 10-60。值越低越省 CPU。视频原始帧率低于设置值时会重复帧"
- hwaccelMode: "0"
  $name: Hardware acceleration
  $name:zh-CN: 硬件加速
  $description: "D3D11VA hardware decoding via ffmpeg. May fail on some GPUs or codecs"
  $description:zh-CN: "通过 ffmpeg 使用 D3D11VA 硬件解码，部分显卡或编码可能不支持"
  $options:
    - "0": "Disabled (software decode)"
    - "1": "Enabled (D3D11VA)"
  $options:zh-CN:
    - "0": "关闭（纯 CPU 软解）"
    - "1": "开启（D3D11VA）"
- scalingMode: "0"
  $name: Video scaling mode
  $name:zh-CN: 视频缩放模式
  $description: "How the video is scaled to fit the screen"
  $description:zh-CN: "视频如何缩放以适应屏幕"
  $options:
    - "0": "Cover (fill screen, keep ratio, crop edges)"
    - "1": "Contain (fit screen, keep ratio, pad bars)"
    - "2": "Center (original size, centered, pad bars)"
    - "3": "Stretch (fill screen, may distort)"
  $options:zh-CN:
    - "0": "填充（保持比例，裁剪边缘）"
    - "1": "适应（保持比例，可能有空置边缘）"
    - "2": "居中（原尺寸居中，可能有空置边缘）"
    - "3": "拉伸（填满屏幕，可能变形）"
- padColorMode: "black"
  $name: Pad color (for Contain/Center modes)
  $name:zh-CN: 空置边缘颜色（适应/居中模式）
  $description: "Color of the padding area around the video"
  $description:zh-CN: "视频周围空置边缘的颜色"
  $options:
    - "black": "Black"
    - "white": "White"
    - "dynamic": "Dynamic (white in light mode, black in dark mode)"
    - "transparent": "Transparent (shows desktop wallpaper underneath)"
    - "custom": "Custom color (set below)"
  $options:zh-CN:
    - "black": "黑色"
    - "white": "白色"
    - "dynamic": "动态（浅色模式白色，深色模式黑色）"
    - "transparent": "透明（露出底下的桌面壁纸）"
    - "custom": "自定义颜色（在下方设置）"
- padColorCustom: ""
  $name: Custom pad color (hex)
  $name:zh-CN: 自定义边缘颜色（十六进制）
  $description: "Custom color for padding when Pad color is set to Custom. 6-digit RGB or 8-digit ARGB. Examples: #FF0000 for red, #80FF0000 for semi-transparent red, #00000000 for fully transparent"
  $description:zh-CN: "空置边缘颜色设为自定义时使用。6 位 RGB 或 8 位 ARGB。例如 #FF0000 为红色，#80FF0000 为半透明红，#00000000 为全透明"
- applyMode: "on_next"
  $name: Settings/theme apply timing
  $name:zh-CN: 设置/主题生效时机
  $description: "When a setting is changed, theme switches, or files change"
  $description:zh-CN: "当设置改变、主题切换或文件变化时"
  $options:
    - "on_next": "Wait until current video finishes, then apply"
    - "instant": "Instant (stop current video, apply immediately)"
  $options:zh-CN:
    - "on_next": "等当前视频播完后生效"
    - "instant": "立即停止当前视频并生效"
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
int g_fps = 15;
int g_scalingMode = 0;
WCHAR g_padColorMode[32] = {L"black"};
WCHAR g_padColorCustom[32] = {0};
bool g_isPadTransparent = false;
WCHAR g_applyMode[16] = {L"instant"};
int g_sortMode = 0;
int g_hwaccelMode = 0;
volatile bool g_pendingReload = false;
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
void ReloadVideoList();
bool PlayNext();

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
    std::wstring name;
    FILETIME writeTime;
    FILETIME createTime;
    ULONGLONG size;
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
                ULONGLONG size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
                entries.push_back({full, fd.cFileName, fd.ftLastWriteTime, fd.ftCreationTime, size});
            }
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);

    std::sort(entries.begin(), entries.end(), [](const VideoEntry& a, const VideoEntry& b) {
        switch (g_sortMode) {
            case 1:
                if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
                    return a.writeTime.dwHighDateTime < b.writeTime.dwHighDateTime;
                return a.writeTime.dwLowDateTime < b.writeTime.dwLowDateTime;
            case 2: return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
            case 3: return _wcsicmp(a.name.c_str(), b.name.c_str()) > 0;
            case 4:
                if (a.createTime.dwHighDateTime != b.createTime.dwHighDateTime)
                    return a.createTime.dwHighDateTime < b.createTime.dwHighDateTime;
                return a.createTime.dwLowDateTime < b.createTime.dwLowDateTime;
            case 5:
                if (a.createTime.dwHighDateTime != b.createTime.dwHighDateTime)
                    return a.createTime.dwHighDateTime > b.createTime.dwHighDateTime;
                return a.createTime.dwLowDateTime > b.createTime.dwLowDateTime;
            case 6: return a.size > b.size;
            case 7: return a.size < b.size;
            case 0:
            default:
                if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
                    return a.writeTime.dwHighDateTime > b.writeTime.dwHighDateTime;
                return a.writeTime.dwLowDateTime > b.writeTime.dwLowDateTime;
        }
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
    DWORD frameIntervalMs = g_fps > 0 ? 1000 / (DWORD)g_fps : 66;
    DWORD lastTick = GetTickCount();
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
        BYTE alphaFormat = g_isPadTransparent ? AC_SRC_ALPHA : 0;
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)g_opacity, alphaFormat};
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

        DWORD elapsed = GetTickCount() - lastTick;
        if (elapsed < frameIntervalMs) {
            Sleep(frameIntervalMs - elapsed);
        }
        lastTick = GetTickCount();
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

    WCHAR cmd[MAX_PATH * 6];
    const WCHAR* vfArg = nullptr;

    WCHAR padColor[64] = {0};
    bool isPadTransparent = false;
    if (wcscmp(g_padColorMode, L"white") == 0) {
        wcscpy_s(padColor, L"color=white");
    } else if (wcscmp(g_padColorMode, L"black") == 0) {
        wcscpy_s(padColor, L"color=black");
    } else if (wcscmp(g_padColorMode, L"transparent") == 0) {
        wcscpy_s(padColor, L"color=0x00000000");
        isPadTransparent = true;
    } else if (wcscmp(g_padColorMode, L"dynamic") == 0) {
        wcscpy_s(padColor, g_lastIsDark ? L"color=black" : L"color=white");
    } else if (wcscmp(g_padColorMode, L"custom") == 0 && g_padColorCustom[0]) {
        const WCHAR* src = g_padColorCustom[0] == L'#' ? g_padColorCustom + 1 : g_padColorCustom;
        size_t len = wcslen(src);
        bool valid = (len == 6 || len == 8);
        if (valid) {
            for (size_t i = 0; i < len; i++) {
                WCHAR c = src[i];
                if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F'))) { valid = false; break; }
            }
        }
        WCHAR hex[16] = {0};
        if (!valid) {
            Wh_Log(L"padColorCustom: invalid value '%s', fallback to black", g_padColorCustom);
            wcscpy_s(hex, L"0xFF000000");
        } else if (len == 6) {
            swprintf_s(hex, L"0xFF%s", src);
        } else {
            swprintf_s(hex, L"0x%s", src);
            if (src[0] == L'0' && src[1] == L'0') isPadTransparent = true;
        }
        swprintf_s(padColor, L"color=%s", hex);
    } else {
        wcscpy_s(padColor, L"color=black");
    }

    switch (g_scalingMode) {
        case 0: vfArg = L" -vf \"scale=%d:%d:force_original_aspect_ratio=increase,crop=%d:%d\""; break;
        case 1: vfArg = L" -vf \"scale=%d:%d:force_original_aspect_ratio=decrease,pad=%d:%d:(ow-iw)/2:(oh-ih)/2:%s\""; break;
        case 2: vfArg = L" -vf \"color=c=%s:s=%dx%d:r=%d,format=bgra[bg];[0:v]format=bgra[v];[bg][v]overlay=(W-w)/2:(H-h)/2:shortest=1,format=bgra\""; break;
        case 3: vfArg = nullptr; break;
        default: vfArg = nullptr; break;
    }
    const WCHAR* hwaccelArg = (g_hwaccelMode == 1) ? L" -hwaccel d3d11va" : L"";

    if (vfArg) {
        WCHAR vfBuf[512];
        if (g_scalingMode == 0) {
            swprintf_s(vfBuf, vfArg, sw, sh, sw, sh);
        } else if (g_scalingMode == 2) {
            const WCHAR* colorVal = padColor;
            if (wcsncmp(padColor, L"color=", 6) == 0) colorVal = padColor + 6;
            swprintf_s(vfBuf, vfArg, colorVal, sw, sh, g_fps);
        } else {
            swprintf_s(vfBuf, vfArg, sw, sh, sw, sh, padColor);
        }
        swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error%s -i \"%s\" -an -f rawvideo -pix_fmt bgra%s -r %d -",
            g_ffmpegPath, hwaccelArg, videoPath, vfBuf, g_fps);
    } else {
        swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error%s -i \"%s\" -an -f rawvideo -pix_fmt bgra -s %dx%d -r %d -",
            g_ffmpegPath, hwaccelArg, videoPath, sw, sh, g_fps);
    }
    g_isPadTransparent = isPadTransparent;
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

void ReloadVideoList()
{
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
}

void ReloadWallpaper()
{
    StopFfmpeg();
    ReloadVideoList();
    if (!g_videoList.empty()) PlayNext();
}

bool PlayNext()
{
    if (g_pendingReload) {
        g_pendingReload = false;
        ReloadVideoList();
    }
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

    s = Wh_GetStringSetting(L"sortMode");
    WCHAR sortStr[8] = {0};
    if (s) { wcscpy_s(sortStr, 8, s); Wh_FreeStringSetting(s); }
    int newSortMode = _wtoi(sortStr);
    if (newSortMode < 0 || newSortMode > 7) newSortMode = 0;

    int pct = Wh_GetIntSetting(L"opacity");
    g_opacity = (pct >= 0 && pct <= 100) ? (pct * 255 / 100) : 255;

    int fps = Wh_GetIntSetting(L"fps");
    if (fps < 10) fps = 10;
    if (fps > 60) fps = 60;
    int oldFps = g_fps;
    g_fps = fps;

    s = Wh_GetStringSetting(L"scalingMode");
    WCHAR scalingStr[8] = {0};
    if (s) { wcscpy_s(scalingStr, 8, s); Wh_FreeStringSetting(s); }
    int scaling = _wtoi(scalingStr);
    if (scaling < 0 || scaling > 3) scaling = 0;
    int oldScaling = g_scalingMode;
    g_scalingMode = scaling;

    s = Wh_GetStringSetting(L"padColorMode");
    WCHAR newPadMode[32] = {0};
    if (s) { wcscpy_s(newPadMode, 32, s); Wh_FreeStringSetting(s); }
    if (!newPadMode[0]) wcscpy_s(newPadMode, 32, L"black");

    s = Wh_GetStringSetting(L"padColorCustom");
    WCHAR newPadCustom[32] = {0};
    if (s) { wcscpy_s(newPadCustom, 32, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"applyMode");
    WCHAR newApplyMode[16] = {0};
    if (s) { wcscpy_s(newApplyMode, 16, s); Wh_FreeStringSetting(s); }
    if (!newApplyMode[0]) wcscpy_s(newApplyMode, 16, L"instant");

    s = Wh_GetStringSetting(L"hwaccelMode");
    WCHAR hwaccelStr[4] = {0};
    if (s) { wcscpy_s(hwaccelStr, 4, s); Wh_FreeStringSetting(s); }
    int newHwaccel = _wtoi(hwaccelStr);
    if (newHwaccel < 0 || newHwaccel > 1) newHwaccel = 0;

    bool needReload =
        wcscmp(newFfmpegPath, g_ffmpegPath) != 0 ||
        wcscmp(newLightPath, g_lightPath) != 0 ||
        wcscmp(newDarkPath, g_darkPath) != 0 ||
        newSortMode != g_sortMode ||
        newHwaccel != g_hwaccelMode ||
        g_fps != oldFps ||
        g_scalingMode != oldScaling ||
        wcscmp(newPadMode, g_padColorMode) != 0 ||
        wcscmp(newPadCustom, g_padColorCustom) != 0;

    wcscpy_s(g_ffmpegPath, MAX_PATH, newFfmpegPath);
    wcscpy_s(g_lightPath, MAX_PATH, newLightPath);
    wcscpy_s(g_darkPath, MAX_PATH, newDarkPath);
    g_sortMode = newSortMode;
    g_hwaccelMode = newHwaccel;
    wcscpy_s(g_padColorMode, 32, newPadMode);
    wcscpy_s(g_padColorCustom, 32, newPadCustom);
    wcscpy_s(g_applyMode, 16, newApplyMode);

    if (needReload) {
        if (wcscmp(newApplyMode, L"on_next") == 0 && g_ffmpegProc != NULL) {
            Wh_Log(L"Wh_ModSettingsChanged: deferred until next video");
            g_pendingReload = true;
        } else {
            Wh_Log(L"Wh_ModSettingsChanged: path/settings changed, reloading");
            ReloadWallpaper();
        }
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
            bool isOnNext = (wcscmp(g_applyMode, L"on_next") == 0);
            if (handles[idx] == hThemeEvt && IsDark() != g_lastIsDark) {
                if (isOnNext && g_ffmpegProc) { Wh_Log(L"Theme switch deferred until next video"); g_pendingReload = true; }
                else { ReloadWallpaper(); }
            } else if (handles[idx] == g_dirChangeHandle) {
                if (isOnNext && g_ffmpegProc) { Wh_Log(L"File change deferred until next video"); g_pendingReload = true; }
                else { ReloadWallpaper(); }
                FindNextChangeNotification(g_dirChangeHandle);
            }
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