// ==WindhawkMod==
// @id              xandriw-drag-nearby-delete
// @name            Drag & Trash
// @name:zh-CN      一拖即删
// @description     Drag & Trash: a cursor-following or nearby/top Recycle Bin with custom still/animated icons, batch recycling and Ctrl+Z undo.
// @description:zh-CN 一拖即删：回收站可跟随鼠标或显示在附近/顶部，支持自定义静态/动态图标，并保留批量删除与 Ctrl+Z 撤销。
// @version         1.1.7
// @author          XandriW
// @github          https://github.com/xandri19wang
// @include         explorer.exe
// @compilerOptions -lole32 -lgdi32 -lshell32 -lgdiplus -luuid -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drag & Trash · 一拖即删

![Follow cursor demo](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/follow-cursor.gif)

**Drag less. Trash faster.**

回收站可跟随鼠标或显示在附近/顶部，支持自定义静态/动态图标，并保留批量删除与 Ctrl+Z 撤销。

## How it works / 如何使用

### 01 · Follow the cursor / 跟随鼠标

The target follows your cursor, then pauses as you approach it to make dropping easy.
拖动时跟随鼠标，靠近目标时停住，方便准确放入。

![Follow cursor](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/follow-cursor.gif)

### 02 · Near the drag start / 起点附近

The target appears beside the starting point, so you don't have to drag across the screen.
在拖拽起点附近显示，缩短拖动距离。

![Nearby placement](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/nearby.gif)

### 03 · Top of screen / 屏幕顶部

Prefer the classic placement? Keep the original top-of-screen option.
喜欢原版的位置？可以继续将目标固定在屏幕顶部。

![Top placement](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/screen-top.gif)

### 04 · System or custom icon / 系统或自定义图标

Use the Windows Recycle Bin icon, a local PNG/JPG/BMP/ICO/GIF, or a direct HTTPS image link.
可使用 Windows 回收站图标、本地静态图片或 GIF 动图，也可填写 HTTPS 图片直链。

![Windows system icon](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/system-icon.gif)

## More / 其他功能

- **Batch recycling / 批量回收:** Drop several files together in one Shell operation.
- **Ctrl+Z undo / 撤销:** Restore a supported recycle operation in File Explorer (within the active Shell session).
- **Lightweight / 轻量:** GIF playback runs only while dragging. HTTPS images are downloaded to local cache on configuration/reload, never during a drag.

## Credits & license / 原作者与授权

**Based on [Drag to Delete](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/drag-to-delete.wh.cpp) by [iMAboud](https://github.com/iMAboud).**

This mod reuses and modifies the original MIT-licensed drag-and-drop and recycle implementation, with attribution to its author. The adaptation adds cursor-following and nearby placement, retains the original top placement, and introduces configurable still/animated icons and batch/undo improvements. The original author is not affiliated with or endorsing this adaptation.

本 Mod 基于 iMAboud 的 Drag to Delete 开源代码修改，保留原作者署名和 MIT 授权说明。新版加入鼠标跟随、起点附近模式、自定义静态/动态图标，以及批量回收和撤销改进；原版顶部位置仍可选择。

**GIFs not loading?** [Nearby](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/nearby.gif) · [Follow](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/follow-cursor.gif) · [Top](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/screen-top.gif) · [System icon](https://raw.githubusercontent.com/xandri19wang/drag-and-trash/main/demos/system-icon.gif).

**Note / 注意:** Virtual desktop icons without real file paths cannot be recycled. Ctrl+Z may not work after Explorer restarts or the Recycle Bin is emptied. No third-party GIFs are included with the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Placement: follow
  $name: Target placement
  $name:zh-CN: 回收站位置
  $options:
    - follow: Follow cursor
    - nearby: Near the starting point
    - top: Top of screen (original)
  $options:zh-CN:
    - follow: 跟随鼠标
    - nearby: 拖拽起点附近
    - top: 屏幕顶部（原版）
- IconSource: system
  $name: Icon appearance
  $name:zh-CN: 图标类型
  $options:
    - system: Current Windows Recycle Bin
    - local: Local image / GIF
    - url: HTTPS image link
  $options:zh-CN:
    - system: 当前 Windows 回收站图标
    - local: 本地图片 / GIF 动图
    - url: HTTPS 图片直链
- IconPath: ""
  $name: Local image path
  $name:zh-CN: 本地图片路径
  $description: Full path to a PNG, JPG, BMP, ICO or GIF. Used when Icon appearance is Local.
  $description:zh-CN: 完整 PNG、JPG、BMP、ICO 或 GIF 路径；图标类型选本地时生效。
  #! $format: filePath
  #! $showIf: {IconSource: local}
- IconUrl: ""
  $name: Online image URL
  $name:zh-CN: 在线图片链接
  $description: HTTPS direct image URL. Downloaded and cached once, never during dragging.
  $description:zh-CN: HTTPS 图片直链；下载并缓存，不在拖拽过程中联网。
  #! $showIf: {IconSource: url}
- IconSize: 72
  $name: Icon size (px)
  $name:zh-CN: 图标大小（像素）
  #! $min: 24
  #! $max: 72
  #! $format: slider
- DesktopDistance: 72
  $name: Distance on desktop (px)
  $name:zh-CN: 桌面拖拽距离（像素）
  #! $min: 48
  #! $max: 220
  #! $format: slider
  #! $hideIf: {Placement: top}
- FolderDistance: 100
  $name: Distance in folders (px)
  $name:zh-CN: 文件夹拖拽距离（像素）
  #! $min: 48
  #! $max: 220
  #! $format: slider
  #! $hideIf: {Placement: top}
- TopMargin: 8
  $name: Top margin (px)
  $name:zh-CN: 顶部边距（像素）
  #! $min: 0
  #! $max: 100
  #! $hideIf: {Placement: [follow, nearby]}
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shobjidl.h>
#include <winhttp.h>
#include <algorithm>
#include <string>
#include <cstdint>
#include <gdiplus.h>
#include <vector>
#include <windhawk_utils.h>

using namespace Gdiplus;

static const int BASE_WND_WIDTH    = 96;
static const int BASE_WND_HEIGHT   = 96;
static const float BASE_CENTER_X   = 48.0f;
static const float BASE_CENTER_Y   = 48.0f;
static const float BASE_RADIUS     = 37.0f;
static const float BASE_HIT_MARGIN = 3.0f;

constexpr WCHAR kClassName[] = L"XandriWNearbyRecycleTargetOverlay";

enum class AnimState { HIDDEN, VISIBLE, DROPPED };

static HWND g_hOverlayWnd = nullptr;
static HANDLE g_hUIThread = nullptr;
static DWORD g_dwUIThreadId = 0;
static HANDLE g_readyEvent = nullptr;
static LONG g_activeDrags = 0;
static ULONG_PTR g_gdiplusToken = 0;

static HANDLE g_hDeleteWorker = nullptr;
static HANDLE g_hDeleteQueueEvent = nullptr;
static HANDLE g_hDeleteStopEvent = nullptr;
static CRITICAL_SECTION g_deleteCs;
static std::vector<std::vector<wchar_t>> g_deleteQueue;

static RECT g_targetWorkArea = {};
static POINT g_overlayOrigin = {};
static HICON g_hRecycleIcon = nullptr;
static LONG g_dragFromDesktop = 0;
static Gdiplus::Image* g_gifImage = nullptr;
static std::vector<UINT> g_gifDelaysMs;
static UINT g_gifFrame = 0;
static UINT g_gifFrameCount = 0;
static ULONGLONG g_gifNextFrameAt = 0;
struct UserSettings {
    std::wstring placement = L"follow";
    std::wstring iconSource = L"system";
    std::wstring iconPath, iconUrl;
    int iconSize = 72;
    int desktopDistance = 72;
    int folderDistance = 100;
    int topMargin = 8;
};
static UserSettings g_settings;
static std::wstring g_activeImage;
static HICON g_customIcon = nullptr;
static POINT g_lastCursor = {};
static bool g_haveLastCursor = false;

static std::wstring ReadString(PCWSTR name) {
    PCWSTR s = Wh_GetStringSetting(name);
    std::wstring value = s ? s : L"";
    if (s) Wh_FreeStringSetting(s);
    return value;
}
static int ReadNumber(PCWSTR name, int def, int min, int max) {
    int value = Wh_GetIntSetting(name);
    return value ? std::clamp(value, min, max) : def;
}
static void ReadSettings() {
    g_settings.placement = ReadString(L"Placement");
    if (g_settings.placement != L"follow" &&
        g_settings.placement != L"nearby" &&
        g_settings.placement != L"top") g_settings.placement = L"follow";
    g_settings.iconSource = ReadString(L"IconSource");
    if (g_settings.iconSource != L"local" &&
        g_settings.iconSource != L"url") g_settings.iconSource = L"system";
    g_settings.iconPath = ReadString(L"IconPath");
    g_settings.iconUrl = ReadString(L"IconUrl");
    g_settings.iconSize = ReadNumber(L"IconSize", 72, 24, 72);
    g_settings.desktopDistance = ReadNumber(L"DesktopDistance", 72, 48, 220);
    g_settings.folderDistance = ReadNumber(L"FolderDistance", 100, 48, 220);
    g_settings.topMargin = ReadNumber(L"TopMargin", 8, 0, 100);
}

static bool SafeImageFile(const std::wstring& path) {
    if (path.empty() || path.size() >= 32700) return false;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size = {};
    bool ok = GetFileSizeEx(f, &size) && size.QuadPart > 0 &&
              size.QuadPart <= 8LL * 1024 * 1024;
    CloseHandle(f);
    return ok;
}
static bool HasIcoHeader(const std::wstring& path) {
    BYTE header[4] = {};
    DWORD n = 0;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_DELETE,
                          nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                          nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    bool ok = ReadFile(f, header, 4, &n, nullptr) && n == 4 &&
              header[0] == 0 && header[1] == 0 &&
              header[2] == 1 && header[3] == 0;
    CloseHandle(f);
    return ok;
}
static std::wstring CacheFilePath(const std::wstring& url) {
    wchar_t buffer[32768] = {};
    size_t chars = Wh_GetModStoragePath(buffer, ARRAYSIZE(buffer));
    if (!chars || chars >= ARRAYSIZE(buffer)) return L"";
    uint64_t hash = 14695981039346656037ULL;
    for (wchar_t c : url) {
        hash ^= (uint16_t)c;
        hash *= 1099511628211ULL;
    }
    wchar_t hashName[48] = {};
    swprintf(hashName, ARRAYSIZE(hashName), L"\\image-%016llx.img",
             (unsigned long long)hash);
    return std::wstring(buffer) + hashName;
}
static bool FetchImageHttps(const std::wstring& url,
                            const std::wstring& output) {
    if (url.size() < 10 || url.size() > 4096 ||
        url.compare(0, 8, L"https://") != 0 ||
        output.empty()) return false;
    URL_COMPONENTS parts = {};
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = (DWORD)-1;
    parts.dwHostNameLength = (DWORD)-1;
    parts.dwUrlPathLength = (DWORD)-1;
    parts.dwExtraInfoLength = (DWORD)-1;
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts) ||
        parts.nScheme != INTERNET_SCHEME_HTTPS ||
        !parts.dwHostNameLength) return false;
    std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring resource = parts.dwUrlPathLength ?
        std::wstring(parts.lpszUrlPath, parts.dwUrlPathLength) : L"/";
    if (parts.dwExtraInfoLength)
        resource.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    HINTERNET session = WinHttpOpen(
        L"DragAndTrash/1.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return false;
    WinHttpSetTimeouts(session, 2500, 2500, 3500, 4000);
    HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
    HINTERNET request = connection ?
        WinHttpOpenRequest(connection, L"GET", resource.c_str(), nullptr,
                           WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                           WINHTTP_FLAG_SECURE) : nullptr;
    // Never follow a redirect onto an insecure destination or local endpoint.
    DWORD noRedirect = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    if (request) WinHttpSetOption(request, WINHTTP_OPTION_REDIRECT_POLICY,
                                  &noRedirect, sizeof(noRedirect));
    DWORD response = 0, responseBytes = sizeof(response);
    bool ok = request &&
        WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(request, nullptr) &&
        WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE |
                           WINHTTP_QUERY_FLAG_NUMBER,
                           WINHTTP_HEADER_NAME_BY_INDEX, &response,
                           &responseBytes, WINHTTP_NO_HEADER_INDEX) &&
        response == 200;
    std::wstring temp = output + L"." +
                        std::to_wstring(GetCurrentProcessId()) + L"." +
                        std::to_wstring(GetCurrentThreadId()) + L".tmp";
    HANDLE f = INVALID_HANDLE_VALUE;
    if (ok) {
        f = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        ok = (f != INVALID_HANDLE_VALUE);
    }
    BYTE chunk[8192];
    DWORD total = 0;
    while (ok) {
        DWORD count = 0;
        if (!WinHttpReadData(request, chunk, sizeof(chunk), &count)) {
            ok = false; break;
        }
        if (!count) break;
        if (count > 8 * 1024 * 1024 - total) {
            ok = false; break;
        }
        DWORD written = 0;
        if (!WriteFile(f, chunk, count, &written, nullptr) ||
            written != count) {
            ok = false; break;
        }
        total += count;
    }
    if (f != INVALID_HANDLE_VALUE) CloseHandle(f);
    if (request) WinHttpCloseHandle(request);
    if (connection) WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);
    if (ok && total) {
        ok = !!MoveFileExW(temp.c_str(), output.c_str(),
                            MOVEFILE_REPLACE_EXISTING);
    } else ok = false;
    if (!ok) DeleteFileW(temp.c_str());
    return ok;
}
static void PrepareImageForDrag() {
    g_activeImage.clear();
    if (g_settings.iconSource == L"local") {
        if (SafeImageFile(g_settings.iconPath))
            g_activeImage = g_settings.iconPath;
    } else if (g_settings.iconSource == L"url") {
        std::wstring cache = CacheFilePath(g_settings.iconUrl);
        if (!cache.empty() &&
            (SafeImageFile(cache) || FetchImageHttps(g_settings.iconUrl, cache)) &&
            SafeImageFile(cache))
            g_activeImage = std::move(cache);
    }
}
static float g_currentDpiScale = 1.0f;
static AnimState g_animState = AnimState::HIDDEN;
static float g_currentScale = 0.0f;
static float g_targetScale = 0.0f;
static float g_lastRenderedScale = -1.0f;

#define WM_UPDATE_DRAG_STATE (WM_USER + 101)
#define TIMER_ANIM 1

HMODULE GetCurrentModuleHandle() {
    HMODULE hModule = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)GetCurrentModuleHandle,
        &hModule
    );
    return hModule;
}

static UINT GetMonitorDpi(HMONITOR hMon) {
    if (hMon) {
        static auto pfnGetDpiForMonitor = (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))
            GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor");
        if (!pfnGetDpiForMonitor) {
            HMODULE hShcore = LoadLibraryW(L"shcore.dll");
            if (hShcore) {
                pfnGetDpiForMonitor = (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))
                    GetProcAddress(hShcore, "GetDpiForMonitor");
            }
        }
        if (pfnGetDpiForMonitor) {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(pfnGetDpiForMonitor(hMon, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY))) {
                if (dpiX > 0) return dpiX;
            }
        }
    }
    return 96;
}

bool IsDesktopDragStart() {
    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;
    HWND wnd = WindowFromPoint(pt);
    wchar_t klass[80] = {};
    for (HWND parent = wnd; parent; parent = GetParent(parent)) {
        klass[0] = 0;
        GetClassNameW(parent, klass, ARRAYSIZE(klass));
        if (lstrcmpiW(klass, L"CabinetWClass") == 0) return false;
        if (lstrcmpiW(klass, L"Progman") == 0 ||
            lstrcmpiW(klass, L"WorkerW") == 0) return true;
    }
    return false;
}

void FreeDragGif() {
    if (g_customIcon) {
        DestroyIcon(g_customIcon);
        g_customIcon = nullptr;
    }
    delete g_gifImage;
    g_gifImage = nullptr;
    g_gifDelaysMs.clear();
    g_gifFrame = 0;
    g_gifFrameCount = 0;
    g_gifNextFrameAt = 0;
}

void LoadDragGif() {
    if (g_gifImage) return;
    if (g_activeImage.empty()) return;
    if (HasIcoHeader(g_activeImage)) {
        g_customIcon = (HICON)LoadImageW(
            nullptr, g_activeImage.c_str(), IMAGE_ICON, 0, 0,
            LR_LOADFROMFILE | LR_DEFAULTSIZE);
        return;
    }
    auto* image = Gdiplus::Image::FromFile(g_activeImage.c_str(), FALSE);
    if (!image || image->GetLastStatus() != Gdiplus::Ok) {
        delete image;
        return; // Original Windows icon fallback.
    }
    // Still images do not have a FrameDimensionTime.
    UINT dimensions = image->GetFrameDimensionsCount();
    if (!dimensions) { delete image; return; }
    std::vector<GUID> guidList(dimensions);
    if (image->GetFrameDimensionsList(guidList.data(), dimensions) != Gdiplus::Ok) {
        delete image; return;
    }
    UINT frames = image->GetFrameCount(&guidList[0]);
    if (!frames || frames > 256 || image->GetWidth() > 1024 ||
        image->GetHeight() > 1024) {
        delete image; return;
    }
    bool animated = IsEqualGUID(guidList[0], Gdiplus::FrameDimensionTime) &&
                    frames > 1;
    g_gifImage = image;
    g_gifFrameCount = animated ? frames : 1;
    g_gifFrame = 0;
    g_gifDelaysMs.assign(frames, 80);
    UINT size = animated ? image->GetPropertyItemSize(PropertyTagFrameDelay) : 0;
    if (size > sizeof(Gdiplus::PropertyItem) && size < 65536) {
        std::vector<BYTE> bytes(size);
        auto* item = reinterpret_cast<Gdiplus::PropertyItem*>(bytes.data());
        if (image->GetPropertyItem(PropertyTagFrameDelay, size, item) == Gdiplus::Ok &&
            item->value && item->length >= frames * sizeof(UINT)) {
            const auto* durations = static_cast<const UINT*>(item->value);
            for (UINT i = 0; i < frames; ++i) {
                UINT ms = durations[i] * 10;
                g_gifDelaysMs[i] = ms < 30 ? 30 : ms > 250 ? 250 : ms;
            }
        }
    }
    if (animated)
        image->SelectActiveFrame(&Gdiplus::FrameDimensionTime, 0);
    g_gifNextFrameAt = GetTickCount64() + g_gifDelaysMs[0];
}

bool StepDragGif() {
    if (!g_gifImage || g_gifFrameCount < 2) return false;
    ULONGLONG now = GetTickCount64();
    if (now < g_gifNextFrameAt) return false;
    g_gifFrame = (g_gifFrame + 1) % g_gifFrameCount;
    g_gifImage->SelectActiveFrame(&Gdiplus::FrameDimensionTime, g_gifFrame);
    g_gifNextFrameAt = now + g_gifDelaysMs[g_gifFrame];
    return true;
}

void RenderFrame(HWND hWnd);
void MoveFilesToRecycleBin(const std::vector<wchar_t>& buffer);

void DrawModernTrashIcon(Graphics& g, float cx, float cy, float scale, Color color) {
    SolidBrush brush(color);
    float s = scale;

    Pen handlePen(color, 2.5f * s);
    handlePen.SetStartCap(LineCapRound);
    handlePen.SetEndCap(LineCapRound);
    g.DrawLine(&handlePen, cx - 4.5f * s, cy - 16.5f * s, cx + 4.5f * s, cy - 16.5f * s);

    Pen lidPen(color, 3.5f * s);
    lidPen.SetStartCap(LineCapRound);
    lidPen.SetEndCap(LineCapRound);
    g.DrawLine(&lidPen, cx - 14.0f * s, cy - 12.0f * s, cx + 14.0f * s, cy - 12.0f * s);

    GraphicsPath bodyPath;
    float topW = 12.0f * s;
    float botW = 9.5f * s;
    float topY = cy - 8.0f * s;
    float botY = cy + 15.0f * s;
    float r = 3.5f * s;

    bodyPath.AddLine(cx - topW, topY, cx + topW, topY);
    bodyPath.AddLine(cx + topW, topY, cx + botW, botY - r);
    bodyPath.AddArc(cx + botW - r * 2, botY - r * 2, r * 2, r * 2, 0, 90);
    bodyPath.AddLine(cx + botW - r, botY, cx - botW + r, botY);
    bodyPath.AddArc(cx - botW, botY - r * 2, r * 2, r * 2, 90, 90);
    bodyPath.AddLine(cx - botW, botY - r, cx - topW, topY);
    bodyPath.CloseFigure();
    g.FillPath(&brush, &bodyPath);

    Pen slotPen(Color(180, 22, 22, 26), 1.8f * s);
    slotPen.SetStartCap(LineCapRound);
    slotPen.SetEndCap(LineCapRound);
    for (float off : { -4.5f * s, 0.0f, 4.5f * s }) {
        g.DrawLine(&slotPen, cx + off, topY + 4.0f * s, cx + off, botY - 4.0f * s);
    }
}

void RenderFrame(HWND hWnd) {
    if (g_animState == AnimState::HIDDEN || g_currentScale < 0.05f) return;

    float dpiScale = g_currentDpiScale;
    int wndWidth = (int)(BASE_WND_WIDTH * dpiScale);
    int wndHeight = (int)(BASE_WND_HEIGHT * dpiScale);
    float cx = BASE_CENTER_X * dpiScale;
    float cy = BASE_CENTER_Y * dpiScale;
    float baseRadius = std::min(44.0f,
                    std::max(20.0f, g_settings.iconSize / 2.0f + 3.0f)) * dpiScale;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = wndWidth;
    bmi.bmiHeader.biHeight = -wndHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);

    {
        Bitmap bmp(wndWidth, wndHeight, wndWidth * 4, PixelFormat32bppPARGB, (BYTE*)pBits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        // Keep the transparent canvas and subtle hover highlight.
        if (g_targetScale > 1.02f) {
            float ringRadius = baseRadius * g_currentScale;
            SolidBrush hoverBrush(Color(35, 255, 255, 255));
            g.FillEllipse(&hoverBrush, cx - ringRadius, cy - ringRadius,
                          ringRadius * 2, ringRadius * 2);
        }
        if (g_gifImage) {
            float side = (float)g_settings.iconSize * dpiScale * g_currentScale;
            g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            g.DrawImage(g_gifImage,
                        RectF(cx - side / 2.0f, cy - side / 2.0f, side, side));
        } else if (!g_hRecycleIcon && !g_customIcon) {
            DrawModernTrashIcon(g, cx, cy, g_currentScale * dpiScale,
                                Color(245, 248, 250));
        }
    }
    // Fall back to Windows native icon only if the GIF can't load.
    if (!g_gifImage && (g_customIcon || g_hRecycleIcon)) {
        int iconSize = (int)(g_settings.iconSize * dpiScale * g_currentScale);
        HICON drawIcon = g_customIcon ? g_customIcon : g_hRecycleIcon;
        DrawIconEx(hdcMem, (int)(cx - iconSize / 2.0f),
                   (int)(cy - iconSize / 2.0f), drawIcon,
                   iconSize, iconSize, 0, nullptr, DI_NORMAL);
    }
    int wndLeft = g_overlayOrigin.x;
    int wndTop = g_overlayOrigin.y;

    POINT ptDst = { wndLeft, wndTop };
    POINT ptSrc = { 0, 0 };
    SIZE wndSize = { wndWidth, wndHeight };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &wndSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}


static bool HasFileDragFormat(IDataObject* source) {
    if (!source) return false;
    FORMATETC hdrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&hdrop) == S_OK) return true;
    CLIPFORMAT shellFormat = static_cast<CLIPFORMAT>(
        RegisterClipboardFormatW(CFSTR_SHELLIDLIST));
    if (!shellFormat) return false;
    FORMATETC shell = { shellFormat, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    return source->QueryGetData(&shell) == S_OK;
}

// Convert a complete drag selection to filesystem paths before enqueueing.
// OLE IDataObject references are never passed across threads or retained
// after IDropTarget::Drop returns.
static bool ExtractDragPaths(IDataObject* source, std::vector<wchar_t>& output) {
    if (!source) return false;
    FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&fmt) == S_OK) {
        STGMEDIUM medium = {};
        if (FAILED(source->GetData(&fmt, &medium))) return false;
        bool ok = false;
        HDROP files = reinterpret_cast<HDROP>(GlobalLock(medium.hGlobal));
        if (files) {
            UINT count = DragQueryFileW(files, 0xFFFFFFFF, nullptr, 0);
            if (count && count <= 10000) {
                ok = true;
                for (UINT n = 0; n < count; ++n) {
                    UINT len = DragQueryFileW(files, n, nullptr, 0);
                    if (!len || len >= 32760) { ok = false; break; }
                    size_t start = output.size();
                    output.resize(start + len + 1, 0);
                    if (DragQueryFileW(files, n, output.data() + start,
                                       len + 1) != len) {
                        ok = false; break;
                    }
                }
            }
            GlobalUnlock(medium.hGlobal);
        }
        ReleaseStgMedium(&medium);
        if (!ok) output.clear();
        else output.push_back(L'\0');
        return ok;
    }

    // Desktop namespace selections can provide Shell ID lists without CF_HDROP.
    IShellItemArray* items = nullptr;
    HRESULT hr = SHCreateShellItemArrayFromDataObject(
        source, IID_PPV_ARGS(&items));
    if (FAILED(hr) || !items) return false;
    DWORD count = 0;
    bool ok = SUCCEEDED(items->GetCount(&count)) && count > 0 && count <= 10000;
    if (ok) {
        for (DWORD n = 0; n < count; ++n) {
            IShellItem* item = nullptr;
            PWSTR path = nullptr;
            if (FAILED(items->GetItemAt(n, &item)) || !item) {
                ok = false; break;
            }
            HRESULT pathHr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
            item->Release();
            if (FAILED(pathHr) || !path || !*path) {
                if (path) CoTaskMemFree(path);
                ok = false; break; // Virtual icons are not deletable files.
            }
            size_t len = wcslen(path);
            if (len >= 32760) {
                CoTaskMemFree(path);
                ok = false; break;
            }
            output.insert(output.end(), path, path + len + 1);
            CoTaskMemFree(path);
        }
    }
    items->Release();
    if (!ok) output.clear();
    else output.push_back(L'\0');
    return ok;
}

class CRecycleDropTarget final : public IDropTarget {
    LONG m_refCount = 1;
    bool m_hasHDrop = false;

    bool IsOverTrash(POINTL pt) {
        if (!g_hOverlayWnd) return false;

        float dpiScale = g_currentDpiScale;
        float centerX = BASE_CENTER_X * dpiScale;
        float centerY = BASE_CENTER_Y * dpiScale;
        float baseRadius = std::min(44.0f,
                    std::max(20.0f, g_settings.iconSize / 2.0f + 3.0f)) * dpiScale;
        float hitMargin = BASE_HIT_MARGIN * dpiScale;
        int wndLeft = g_overlayOrigin.x;
        int wndTop = g_overlayOrigin.y;

        float dx = (float)(pt.x - wndLeft) - centerX;
        float dy = (float)(pt.y - wndTop) - centerY;
        float hitRadius = baseRadius * g_currentScale + hitMargin;

        return (dx * dx + dy * dy) <= (hitRadius * hitRadius);
    }

public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) delete this;
        return count;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        m_hasHDrop = HasFileDragFormat(pDataObj);

        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;
            g_targetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override {
        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;
            g_targetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_hasHDrop = false;
        g_targetScale = 1.0f;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        const bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        bool accepted = false;
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            std::vector<wchar_t> buffer;
            if (ExtractDragPaths(pDataObj, buffer) && buffer.size() > 2) {
                EnterCriticalSection(&g_deleteCs);
                g_deleteQueue.push_back(std::move(buffer));
                LeaveCriticalSection(&g_deleteCs);
                SetEvent(g_hDeleteQueueEvent);
                accepted = true;
                g_currentScale = 1.25f;
                g_targetScale = 0.0f;
                g_animState = AnimState::DROPPED;
            } else {
                Wh_Log(L"Drag did not contain a complete recyclable file selection");
            }
        }
        if (pdwEffect) *pdwEffect = accepted ? DROPEFFECT_MOVE : DROPEFFECT_NONE;
        m_hasHDrop = false;
        return S_OK;
    }
};

static CRecycleDropTarget* g_pDropTarget = nullptr;

// Use the Windows Shell operation service. ADDUNDORECORD registers the
// deletion with Explorer's session-wide Undo stack, while RECYCLEONDELETE
// keeps the item in Recycle Bin. A failure never falls back to permanent delete.
void MoveFilesToRecycleBin(const std::vector<wchar_t>& buffer) {
    if (buffer.empty()) return;
    ULONGLONG startMs = GetTickCount64();
    std::vector<PIDLIST_ABSOLUTE> ids;
    std::vector<PCIDLIST_ABSOLUTE> pidls;
    bool valid = true;
    size_t index = 0;
    while (index < buffer.size() && buffer[index] != L'\0') {
        PCWSTR path = buffer.data() + index;
        size_t len = wcslen(path);
        if (!len || len >= 32760) { valid = false; break; }
        PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path);
        if (!pidl) {
            Wh_Log(L"Failed to resolve a Shell file path: 0x%08X", GetLastError());
            valid = false; break;
        }
        ids.push_back(pidl);
        pidls.push_back(pidl);
        index += len + 1;
    }
    if (!valid || pidls.empty()) {
        for (auto pidl : ids) ILFree(pidl);
        Wh_Log(L"Recycle batch aborted: one or more paths couldn't be resolved");
        return;
    }

    IShellItemArray* group = nullptr;
    HRESULT hr = SHCreateShellItemArrayFromIDLists(
        (UINT)pidls.size(), pidls.data(), &group);
    for (auto pidl : ids) ILFree(pidl);
    if (FAILED(hr) || !group) {
        Wh_Log(L"Unable to build grouped Shell selection: 0x%08X", hr);
        return;
    }

    IFileOperation* operation = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&operation));
    if (SUCCEEDED(hr) && operation) {
        const DWORD flags = FOF_ALLOWUNDO | FOF_SILENT | FOF_WANTNUKEWARNING |
                            FOFX_RECYCLEONDELETE | FOFX_ADDUNDORECORD;
        hr = operation->SetOperationFlags(flags);
        if (SUCCEEDED(hr)) hr = operation->DeleteItems(group);
        if (SUCCEEDED(hr)) hr = operation->PerformOperations();
        BOOL aborted = FALSE;
        operation->GetAnyOperationsAborted(&aborted);
        Wh_Log(L"Recycle batch: result=0x%08X aborted=%d items=%u elapsed=%llu ms",
               hr, aborted, (UINT)pidls.size(),
               (unsigned long long)(GetTickCount64() - startMs));
        operation->Release();
    } else {
        Wh_Log(L"IFileOperation unavailable: 0x%08X", hr);
    }
    group->Release();
}

DWORD WINAPI DeleteWorkerThread(LPVOID) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(comResult)) {
        Wh_Log(L"Cannot initialize file operation COM apartment: 0x%08X", comResult);
        return 0;
    }
    HANDLE handles[2] = { g_hDeleteStopEvent, g_hDeleteQueueEvent };
    while (true) {
        // IFileOperation requires STA COM. Pump hidden COM windows while idle
        // so cross-apartment callbacks never starve behind a raw wait.
        DWORD wait = MsgWaitForMultipleObjectsEx(
            2, handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 + 2) {
            MSG msg = {};
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message != WM_QUIT) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            continue;
        }
        if (wait == WAIT_OBJECT_0) {
            EnterCriticalSection(&g_deleteCs);
            auto queueCopy = std::move(g_deleteQueue);
            LeaveCriticalSection(&g_deleteCs);

            for (const auto& item : queueCopy) {
                MoveFilesToRecycleBin(item);
            }
            break;
        } else if (wait == WAIT_OBJECT_0 + 1) {
            while (true) {
                std::vector<wchar_t> item;
                EnterCriticalSection(&g_deleteCs);
                if (!g_deleteQueue.empty()) {
                    item = std::move(g_deleteQueue.front());
                    g_deleteQueue.erase(g_deleteQueue.begin());
                }
                LeaveCriticalSection(&g_deleteCs);

                if (item.empty()) break;
                MoveFilesToRecycleBin(item);
            }
        } else {
            break;
        }
    }
    CoUninitialize();
    return 0;
}

// During cursor-following mode, the target follows only while the cursor
// moves away from it. Approaching the target freezes its position, so it
// remains reachable as a real OLE drop target instead of running away.
static bool FollowCursor(HWND hWnd) {
    if (g_settings.placement != L"follow" ||
        g_animState != AnimState::VISIBLE || !g_haveLastCursor ||
        g_currentScale < 0.5f || g_targetScale > 1.02f)
        return false;
    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;
    int deltaX = pt.x - g_lastCursor.x;
    int deltaY = pt.y - g_lastCursor.y;
    g_lastCursor = pt;
    if (!deltaX && !deltaY) return false;

    float scale = g_currentDpiScale;
    int width = (int)(BASE_WND_WIDTH * scale);
    int height = (int)(BASE_WND_HEIGHT * scale);
    int centerX = g_overlayOrigin.x + width / 2;
    int centerY = g_overlayOrigin.y + height / 2;
    int toCenterX = centerX - pt.x;
    int toCenterY = centerY - pt.y;
    float capture = (g_settings.iconSize / 2.0f + 22.0f) * scale;
    if (toCenterX * toCenterX + toCenterY * toCenterY < capture * capture)
        return false;
    // When moving toward the icon, freeze the target and allow the drop.
    if (deltaX * toCenterX + deltaY * toCenterY > 0) return false;

    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(mon, &mi)) g_targetWorkArea = mi.rcWork;
    int distance = (InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) ?
                    g_settings.desktopDistance : g_settings.folderDistance);
    int offsetX = (int)(distance * scale);
    int offsetY = (int)(distance * scale *
                        (InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) ?
                         1.0f : 0.94f));
    int idealX = pt.x + offsetX;
    int idealY = pt.y - offsetY;
    int margin = (int)(8 * scale);
    if (idealX + width / 2 > g_targetWorkArea.right - margin)
        idealX = pt.x - offsetX;
    if (idealY - height / 2 < g_targetWorkArea.top + margin)
        idealY = pt.y + offsetY;

    int minX = g_targetWorkArea.left + margin;
    int minY = g_targetWorkArea.top + margin;
    int maxX = std::max(minX, (int)(g_targetWorkArea.right - margin - width));
    int maxY = std::max(minY, (int)(g_targetWorkArea.bottom - margin - height));
    int goalX = std::clamp(idealX - width / 2, minX, maxX);
    int goalY = std::clamp(idealY - height / 2, minY, maxY);
    int diffX = goalX - g_overlayOrigin.x;
    int diffY = goalY - g_overlayOrigin.y;
    if (abs(diffX) + abs(diffY) < 3) return false;
    int stepX = (int)(diffX * 0.38f);
    int stepY = (int)(diffY * 0.38f);
    if (!stepX && diffX) stepX = diffX > 0 ? 1 : -1;
    if (!stepY && diffY) stepY = diffY > 0 ? 1 : -1;
    g_overlayOrigin.x += stepX;
    g_overlayOrigin.y += stepY;
    return !!SetWindowPos(hWnd, nullptr, g_overlayOrigin.x, g_overlayOrigin.y,
                          0, 0, SWP_NOZORDER | SWP_NOACTIVATE |
                          SWP_NOSIZE | SWP_NOOWNERZORDER);
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_UPDATE_DRAG_STATE: {
            if (wParam != 0) {
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                HMONITOR hMon = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                if (GetMonitorInfoW(hMon, &mi)) {
                    g_targetWorkArea = mi.rcWork;
                } else {
                    g_targetWorkArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                }

                g_currentDpiScale = (float)GetMonitorDpi(hMon) / 96.0f;
                // Place the target once per drag. Choose other sides at screen edges.
                int w = (int)(BASE_WND_WIDTH * g_currentDpiScale);
                int h = (int)(BASE_WND_HEIGHT * g_currentDpiScale);
                int margin = (int)(8 * g_currentDpiScale);
                bool desktop = InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) != 0;
                int offset = (desktop ? g_settings.desktopDistance :
                                       g_settings.folderDistance);
                int offsetX = (int)(offset * g_currentDpiScale);
                int offsetY = (int)(offset * (desktop ? 1.0f : 0.94f) *
                                    g_currentDpiScale);
                int targetX = ptCursor.x + offsetX;
                int targetY = ptCursor.y - offsetY;
                if (g_settings.placement == L"top") {
                    targetX = (g_targetWorkArea.left + g_targetWorkArea.right) / 2;
                    targetY = g_targetWorkArea.top +
                              (int)(g_settings.topMargin * g_currentDpiScale) +
                              h / 2;
                }
                if (g_settings.placement != L"top" &&
                    targetX + w / 2 > g_targetWorkArea.right - margin)
                    targetX = ptCursor.x - offsetX;
                if (g_settings.placement != L"top" &&
                    targetY - h / 2 < g_targetWorkArea.top + margin)
                    targetY = ptCursor.y + offsetY;
                g_overlayOrigin.x = targetX - w / 2;
                g_overlayOrigin.y = targetY - h / 2;
                g_lastCursor = ptCursor;
                g_haveLastCursor = true;
                if (g_overlayOrigin.x < g_targetWorkArea.left + margin)
                    g_overlayOrigin.x = g_targetWorkArea.left + margin;
                if (g_overlayOrigin.x + w > g_targetWorkArea.right - margin)
                    g_overlayOrigin.x = g_targetWorkArea.right - margin - w;
                if (g_overlayOrigin.y < g_targetWorkArea.top + margin)
                    g_overlayOrigin.y = g_targetWorkArea.top + margin;
                if (g_overlayOrigin.y + h > g_targetWorkArea.bottom - margin)
                    g_overlayOrigin.y = g_targetWorkArea.bottom - margin - h;
                LoadDragGif();
                g_animState = AnimState::VISIBLE;
                g_currentScale = 0.3f;
                g_targetScale = 1.0f;
                g_lastRenderedScale = -1.0f;
                RenderFrame(hWnd);
                g_lastRenderedScale = g_currentScale;
                ShowWindow(hWnd, SW_SHOWNOACTIVATE);
                SetTimer(hWnd, TIMER_ANIM, 25, nullptr);
            } else {
                if (g_animState != AnimState::DROPPED) {
                    g_targetScale = 0.0f;
                }
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_ANIM) {
                if (abs((int)((g_targetScale - g_currentScale) * 1000.0f)) < 5) {
                    g_currentScale = g_targetScale;
                } else {
                    g_currentScale += (g_targetScale - g_currentScale) * 0.25f;
                }

                if (g_targetScale == 0.0f && g_currentScale < 0.05f) {
                    g_animState = AnimState::HIDDEN;
                    KillTimer(hWnd, TIMER_ANIM);
                    ShowWindow(hWnd, SW_HIDE);
                    FreeDragGif();
                    return 0;
                }

                bool frameChanged = StepDragGif();
                bool moved = FollowCursor(hWnd);
                if (frameChanged || moved ||
                    g_currentScale != g_lastRenderedScale) {
                    g_lastRenderedScale = g_currentScale;
                    RenderFrame(hWnd);
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            KillTimer(hWnd, TIMER_ANIM);
            RevokeDragDrop(hWnd);
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI OverlayUIThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    OleInitialize(nullptr);
    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);
    PrepareImageForDrag();
    // Ask Explorer for the real Recycle Bin desktop icon, respecting custom themes.
    PIDLIST_ABSOLUTE recyclePidl = nullptr;
    if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder,
                                         KF_FLAG_DEFAULT, nullptr, &recyclePidl))) {
        SHFILEINFOW iconInfo = {};
        if (SHGetFileInfoW(reinterpret_cast<LPCWSTR>(recyclePidl), 0,
                           &iconInfo, sizeof(iconInfo),
                           SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON)) {
            g_hRecycleIcon = iconInfo.hIcon;
        }
        CoTaskMemFree(recyclePidl);
    }
    if (!g_hRecycleIcon) {
        SHSTOCKICONINFO stock = {};
        stock.cbSize = sizeof(stock);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_RECYCLER,
                                         SHGSI_ICON | SHGSI_LARGEICON, &stock))) {
            g_hRecycleIcon = stock.hIcon;
        }
    }

    HINSTANCE hInstance = GetCurrentModuleHandle();
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kClassName;

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"Failed to register overlay window class (error: %lu)", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 0;
    }

    HMONITOR hPrimary = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    g_currentDpiScale = (float)GetMonitorDpi(hPrimary) / 96.0f;

    int wndWidth = (int)(BASE_WND_WIDTH * g_currentDpiScale);
    int wndHeight = (int)(BASE_WND_HEIGHT * g_currentDpiScale);

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, nullptr, WS_POPUP,
        0, 0,
        wndWidth, wndHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!g_hOverlayWnd) {
        Wh_Log(L"Failed to create overlay window (error: %lu)", GetLastError());
        UnregisterClassW(kClassName, hInstance);
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 0;
    }

    g_pDropTarget = new CRecycleDropTarget();
    HRESULT hrDrop = RegisterDragDrop(g_hOverlayWnd, g_pDropTarget);
    if (FAILED(hrDrop)) {
        Wh_Log(L"RegisterDragDrop failed with HRESULT 0x%08X", hrDrop);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd); // WM_DESTROY revokes the drop target
        g_hOverlayWnd = nullptr;
    }

    UnregisterClassW(kClassName, hInstance);

    if (g_pDropTarget) {
        g_pDropTarget->Release();
        g_pDropTarget = nullptr;
    }
    FreeDragGif();
    if (g_hRecycleIcon) {
        DestroyIcon(g_hRecycleIcon);
        g_hRecycleIcon = nullptr;
    }
    GdiplusShutdown(g_gdiplusToken);
    OleUninitialize();
    return 0;
}
using DoDragDrop_t = decltype(&DoDragDrop);
static DoDragDrop_t pOriginalDoDragDrop = nullptr;

HRESULT WINAPI Hooked_DoDragDrop(IDataObject* pDataObj, IDropSource* pDropSource, DWORD dwOKEffects, LPDWORD pdwEffect) {
    bool bIsFileDrag = false;
    if (pDataObj) {
        bIsFileDrag = HasFileDragFormat(pDataObj);
    }

    if (bIsFileDrag && InterlockedIncrement(&g_activeDrags) == 1 && g_hOverlayWnd) {
        InterlockedExchange(&g_dragFromDesktop, IsDesktopDragStart() ? 1 : 0);
        PostMessageW(g_hOverlayWnd, WM_UPDATE_DRAG_STATE, 1, 0);
    }

    HRESULT hr = pOriginalDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (bIsFileDrag && InterlockedDecrement(&g_activeDrags) == 0 && g_hOverlayWnd) {
        PostMessageW(g_hOverlayWnd, WM_UPDATE_DRAG_STATE, 0, 0);
    }

    return hr;
}

void Wh_ModUninit();

BOOL Wh_ModInit() {
    ReadSettings();
    InitializeCriticalSection(&g_deleteCs);
    g_hDeleteQueueEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_hDeleteStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hDeleteQueueEvent || !g_hDeleteStopEvent) {
        Wh_Log(L"Failed to create deletion worker events");
        Wh_ModUninit();
        return FALSE;
    }

    g_hDeleteWorker = CreateThread(nullptr, 0, DeleteWorkerThread, nullptr, 0, nullptr);
    if (!g_hDeleteWorker) {
        Wh_Log(L"Failed to create deletion worker thread (error: %lu)", GetLastError());
        Wh_ModUninit();
        return FALSE;
    }

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"Failed to create ready event (error: %lu)", GetLastError());
        Wh_ModUninit();
        return FALSE;
    }

    g_hUIThread = CreateThread(nullptr, 0, OverlayUIThread, nullptr, 0, &g_dwUIThreadId);
    if (!g_hUIThread) {
        Wh_Log(L"Failed to create overlay UI thread (error: %lu)", GetLastError());
        Wh_ModUninit();
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DoDragDrop, Hooked_DoDragDrop, &pOriginalDoDragDrop)) {
        Wh_Log(L"Failed to hook DoDragDrop");
        Wh_ModUninit();
        return FALSE;
    }

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hUIThread) {
        if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
        PostThreadMessageW(g_dwUIThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hUIThread, INFINITE);
        CloseHandle(g_hUIThread);
        g_hUIThread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }

    if (g_hDeleteWorker) {
        if (g_hDeleteStopEvent) SetEvent(g_hDeleteStopEvent);
        WaitForSingleObject(g_hDeleteWorker, INFINITE);
        CloseHandle(g_hDeleteWorker);
        g_hDeleteWorker = nullptr;
    }
    if (g_hDeleteStopEvent) {
        CloseHandle(g_hDeleteStopEvent);
        g_hDeleteStopEvent = nullptr;
    }
    if (g_hDeleteQueueEvent) {
        CloseHandle(g_hDeleteQueueEvent);
        g_hDeleteQueueEvent = nullptr;
    }
    DeleteCriticalSection(&g_deleteCs);
}