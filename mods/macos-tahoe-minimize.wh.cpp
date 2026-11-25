// ==WindhawkMod==
// @id              macos-tahoe-minimize
// @name            macOS 26 Tahoe Minimize Animations
// @description     Adds smooth macOS Genie (Modern Direct2D & Classic) and Scale window minimize/restore/launch/close animations with optional macOS 26 Tahoe Dock sync
// @version         2.0
// @author          Badalemo
// @github          https://github.com/Badalemo
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -lole32 -loleaut32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS 26 Tahoe Minimize Animations
Brings smooth macOS **Genie** (Modern Direct2D lamp-mesh & Classic scanline) and **Scale** animations for window minimize, restore, launch, and close on Windows 11.

## Features
- **Three Animation Styles**:
  - **Modern Genie** (default): Direct2D mesh warp driven by the macOS lamp curve across Bottom, Left, and Right Dock positions.
  - **Classic Genie**: Fast scanline GDI strip warp (automatically delegates Left/Right side Docks to Modern Genie).
  - **Scale**: Smooth macOS cubic-eased scale-down/up effect with rounded silhouette clipping.
- **macOS 26 Tahoe Dock Sync + UIA Taskbar Fallback**: Queries **macOS 26 Tahoe Dock** (`macos-tahoe-dock`) via IPC (with a deadlock-free cross-process RPC bridge for Dock-triggered minimizes/restores) and automatically falls back to Windows Taskbar UI Automation targeting or monitor bottom edges when the Dock is disabled.
- **Flash-Free Restore & Launch Animations**: Temporarily disables DWM transitions and uses DWM cloaking (`DWMWA_CLOAK`) + cached pre-minimize DIB snapshots so restored and newly launched windows never flash before animating.
- **Optional Close & Auto-Hide Support**: Smooth edge-flowing close genie and automatic Windows taskbar reveal when auto-hide is active.

## Credits
The Direct2D **Modern Genie** mesh-warp engine, animation hooks, UI Automation taskbar fallback, and close-direction handling build upon the MIT-licensed [`macos-minimize-animation`](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/macos-minimize-animation.wh.cpp) mod by **Abdullah Masood** (<https://github.com/Abdullah-Masood-05>), incorporating contributions by **Potassiumuncher** (<https://github.com/Potassiumuncher>, Direct2D genie engine), **MrCrustyToast** (<https://github.com/MrCrustyToast>, close triggers), and **ReDrag** (`windows-animations`, Firefox first-window cloak check). **macOS 26 Tahoe Minimize Animations** extends this base with a rounded macOS **Scale** effect, Left/Right/Bottom side-dock genie orientations, and a deadlock-free IPC/RPC sync bridge for `macos-tahoe-dock`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minimizeEffect: genie
  $name: Minimize animation effect
  $description: Animation style played when minimizing, restoring, launching, and closing windows.
  $options:
  - genie: Modern Genie (Direct2D lamp mesh)
  - classicGenie: Classic Genie (scanline warp)
  - scale: Scale effect
- durationMs: 400
  $name: Animation duration (ms)
  $description: How long the animation lasts in milliseconds (50-2000).
- tileCount: 35
  $name: Modern Genie mesh tile count
  $description: Mesh resolution for Modern Genie (8-96; higher is smoother but heavier per frame).
- animateRestore: true
  $name: Animate window restore
  $description: Play the reverse animation when restoring a minimized window.
- animateLaunch: true
  $name: Animate app launch
  $description: Play the rising animation when an application window first opens.
- animateClose: false
  $name: Animate window close
  $description: Play the animation flowing toward a screen edge when closing a window.
- closeDirection: up
  $name: Close direction
  $description: Screen edge closing windows flow toward when "Animate window close" is enabled.
  $options:
  - up: Up (top center)
  - down: Down (bottom center)
  - left: Left (center)
  - right: Right (center)
- hideAsClose: false
  $name: Animate apps that hide to the tray on close
  $description: Play the close animation when an app hides its main window instead of destroying it.
- multiMonitor: true
  $name: Multi-monitor support
  $description: Animate on the window's own monitor instead of always targeting the primary monitor.
- fallbackTarget: taskbar
  $name: Fallback target (when Dock mod is not running)
  $description: Where animations target when macOS 26 Tahoe Dock is not active.
  $options:
  - taskbar: Windows taskbar button (UI Automation)
  - bottomCenter: Bottom center of monitor
  - bottomRight: Bottom right of monitor
- unhideTaskbar: true
  $name: Unhide auto-hide Windows taskbar
  $description: Briefly reveal an auto-hidden Windows taskbar during minimize when macOS Dock is not running.
- unhideDurationMs: 450
  $name: Taskbar unhide duration (ms)
  $description: How long to keep the auto-hidden taskbar revealed before completing deferred minimize (0-5000).
- excludedPrograms: [""]
  $name: Excluded programs
  $description: Executables whose windows should never animate (e.g. WindowsTerminal.exe or WindowsTerminal).
*/
// ==/WindhawkModSettings==

// MIT Attribution & License Notice:
// Built upon macos-minimize-animation (MIT) by Abdullah Masood
// (https://github.com/Abdullah-Masood-05), incorporating Potassiumuncher's
// Modern Genie Direct2D engine (https://github.com/Potassiumuncher),
// MrCrustyToast's close triggers (https://github.com/MrCrustyToast), and
// ReDrag's Firefox cloak guard (windows-animations, MIT). Extended for
// macOS 26 Tahoe with the Scale effect, Left/Right/Bottom side-dock geometry,
// and macos-tahoe-dock IPC/RPC sync.

#include <windows.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <uiautomation.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <mutex>
#include <new>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

namespace {

constexpr float kPi = 3.14159265f;
constexpr int kTileCountMin = 8;
constexpr int kTileCountMax = 96;
constexpr int kTileCountDefault = 35;
constexpr float kCompatTaskbarHeight = 70.0f;
constexpr float kCompatIconSize = 33.0f;
constexpr UINT kAsyncHideMsg = WM_APP + 0x4A1;
constexpr long long kMaxCanvasPixels = 64LL * 1024 * 1024;
constexpr size_t kMaxCachedSnapshots = 32;

constexpr WORD kRemoteCmdMinimize = 1;
constexpr WORD kRemoteCmdRestore = 2;

constexpr WCHAR kDockClassName[] = L"WindhawkMacOS27DockWnd";

enum class AnimEffect { ModernGenie = 0, ClassicGenie = 1, Scale = 2 };
enum class FallbackTarget { Taskbar = 0, BottomCenter = 1, BottomRight = 2 };
enum DockEdge { EdgeBottom = 0, EdgeTop = 1, EdgeLeft = 2, EdgeRight = 3 };

struct Geometry {
    float x;
    float y;
    float width;
    float height;
};

struct AnimData {
    HWND hRealWnd = nullptr;
    HBITMAP hBitmap = nullptr;
    void* pBits = nullptr;
    RECT targetRect{};
    HMONITOR hMon = nullptr;
    int width = 0;
    int height = 0;
    int targetDockX = 0;
    int targetDockY = 0;
    BOOL hasDockTarget = FALSE;
    int dockEdge = EdgeBottom;
    AnimEffect effect = AnimEffect::ModernGenie;
    BOOL isRising = FALSE;
    LONG_PTR originalExStyle = 0;
    BOOL hiddenByCloak = FALSE;
    HANDLE hFirstFrameShown = nullptr;
    int durationMs = 400;
    BOOL requestedUnhide = FALSE;
    HWND hNextApp = nullptr;
    int unhideDurationMs = 450;
    BOOL deferredMinimize = FALSE;
    BOOL isClosing = FALSE;
    UINT closeMsg = WM_DESTROY;
};

struct LaunchData {
    HWND hWnd = nullptr;
    LONG_PTR originalExStyle = 0;
    BOOL hiddenByCloak = FALSE;
};

struct SnapCache {
    HBITMAP hBmp = nullptr;
    void* pBits = nullptr;
    int w = 0;
    int h = 0;
    ULONGLONG tick = 0;
};

struct GhostCanvas {
    int left = 0;
    int top = 0;
    int width = 1;
    int height = 1;
    HWND hGhost = nullptr;
    HDC hScreenDC = nullptr;
    HDC hMemDC = nullptr;
    HBITMAP hTargetBmp = nullptr;
    HBITMAP hOldBmp = nullptr;
    void* pTargetBits = nullptr;
    bool ok = false;
};

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using DefWindowProcA_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using DestroyWindow_t = BOOL(WINAPI*)(HWND);
using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
using ShowWindowAsync_t = BOOL(WINAPI*)(HWND, int);
using SetWindowPlacement_t = BOOL(WINAPI*)(HWND, const WINDOWPLACEMENT*);
using CloseWindow_t = BOOL(WINAPI*)(HWND);
using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);

DefWindowProcW_t DefWindowProcW_Original = nullptr;
DefWindowProcA_t DefWindowProcA_Original = nullptr;
DestroyWindow_t DestroyWindow_Original = nullptr;
ShowWindow_t ShowWindow_Original = nullptr;
ShowWindowAsync_t ShowWindowAsync_Original = nullptr;
SetWindowPlacement_t SetWindowPlacement_Original = nullptr;
CloseWindow_t CloseWindow_Original = nullptr;
SetWindowPos_t SetWindowPos_Original = nullptr;

std::mutex g_D2DFactoryMutex;
ID2D1Factory* g_d2dFactory = nullptr;

std::mutex g_CacheMutex;
std::unordered_map<HWND, SnapCache> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions;
std::unordered_map<std::wstring, int> g_ProcessIconPositions;
std::unordered_map<HWND, uint32_t> g_DockTargetCache;
std::unordered_set<HWND> g_LaunchSeen;
std::unordered_set<HWND> g_AnimActive;
std::vector<std::wstring> g_ExcludedPrograms;

std::atomic<int> g_effect{static_cast<int>(AnimEffect::ModernGenie)};
std::atomic<int> g_durationMs{400};
std::atomic<int> g_tileCount{kTileCountDefault};
std::atomic<bool> g_openAnimation{true};
std::atomic<bool> g_launchAnimation{true};
std::atomic<bool> g_closeAnimation{false};
std::atomic<int> g_closeDirection{2};  // 0=left, 1=right, 2=up, 3=down
std::atomic<bool> g_hideAsClose{false};
std::atomic<bool> g_multiMonitor{true};
std::atomic<int> g_fallbackTarget{static_cast<int>(FallbackTarget::Taskbar)};
std::atomic<bool> g_unhideEnabled{true};
std::atomic<int> g_unhideDurationMs{450};

std::atomic<bool> g_unloading{false};
std::mutex g_WorkerThreadsMutex;
std::vector<HANDLE> g_WorkerThreads;

UINT g_msgQueryIconCenter = 0;
UINT g_msgRemoteAnim = 0;

ID2D1Factory* EnsureD2DFactory() {
    std::lock_guard<std::mutex> lock(g_D2DFactoryMutex);
    if (!g_d2dFactory && !g_unloading.load(std::memory_order_relaxed)) {
        HRESULT hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory),
            reinterpret_cast<void**>(&g_d2dFactory));
        if (FAILED(hr)) {
            g_d2dFactory = nullptr;
        }
    }
    return g_d2dFactory;
}

std::wstring ToLowerTrimmed(std::wstring s) {
    size_t start = 0;
    while (start < s.size() && std::iswspace(s[start])) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && std::iswspace(s[end - 1])) {
        --end;
    }
    s = s.substr(start, end - start);
    for (wchar_t& ch : s) {
        ch = static_cast<wchar_t>(std::towlower(ch));
    }
    return s;
}

std::wstring ReadWhStringSetting(PCWSTR name) {
    PCWSTR val = Wh_GetStringSetting(name);
    std::wstring res = val ? val : L"";
    if (val) {
        Wh_FreeStringSetting(val);
    }
    return res;
}

void LoadSettings() {
    std::wstring eff = ReadWhStringSetting(L"minimizeEffect");
    if (eff == L"classicGenie") {
        g_effect.store(static_cast<int>(AnimEffect::ClassicGenie),
                       std::memory_order_relaxed);
    } else if (eff == L"scale") {
        g_effect.store(static_cast<int>(AnimEffect::Scale),
                       std::memory_order_relaxed);
    } else {
        g_effect.store(static_cast<int>(AnimEffect::ModernGenie),
                       std::memory_order_relaxed);
    }

    int ms = Wh_GetIntSetting(L"durationMs");
    if (ms <= 0) {
        ms = 400;
    }
    g_durationMs.store(std::clamp(ms, 50, 2000), std::memory_order_relaxed);

    int tiles = Wh_GetIntSetting(L"tileCount");
    if (tiles <= 0) {
        tiles = kTileCountDefault;
    }
    g_tileCount.store(std::clamp(tiles, kTileCountMin, kTileCountMax),
                      std::memory_order_relaxed);

    g_openAnimation.store(Wh_GetIntSetting(L"animateRestore") != 0,
                          std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"animateLaunch") != 0,
                            std::memory_order_relaxed);
    g_closeAnimation.store(Wh_GetIntSetting(L"animateClose") != 0,
                           std::memory_order_relaxed);
    g_hideAsClose.store(Wh_GetIntSetting(L"hideAsClose") != 0,
                        std::memory_order_relaxed);
    g_multiMonitor.store(Wh_GetIntSetting(L"multiMonitor") != 0,
                         std::memory_order_relaxed);
    g_unhideEnabled.store(Wh_GetIntSetting(L"unhideTaskbar") != 0,
                          std::memory_order_relaxed);

    int unhideMs = Wh_GetIntSetting(L"unhideDurationMs");
    g_unhideDurationMs.store(std::clamp(unhideMs, 0, 5000),
                             std::memory_order_relaxed);

    std::wstring dir = ReadWhStringSetting(L"closeDirection");
    int dirCode = (dir == L"left") ? 0 : (dir == L"right") ? 1 : (dir == L"down") ? 3 : 2;
    g_closeDirection.store(dirCode, std::memory_order_relaxed);

    std::wstring fb = ReadWhStringSetting(L"fallbackTarget");
    if (fb == L"bottomCenter") {
        g_fallbackTarget.store(static_cast<int>(FallbackTarget::BottomCenter),
                               std::memory_order_relaxed);
    } else if (fb == L"bottomRight") {
        g_fallbackTarget.store(static_cast<int>(FallbackTarget::BottomRight),
                               std::memory_order_relaxed);
    } else {
        g_fallbackTarget.store(static_cast<int>(FallbackTarget::Taskbar),
                               std::memory_order_relaxed);
    }

    std::vector<std::wstring> excluded;
    for (int i = 0;; ++i) {
        PCWSTR raw = Wh_GetStringSetting(L"excludedPrograms[%d]", i);
        bool has = raw && *raw != L'\0';
        if (has) {
            std::wstring s = ToLowerTrimmed(raw);
            if (!s.empty()) {
                excluded.push_back(std::move(s));
            }
        }
        if (raw) {
            Wh_FreeStringSetting(raw);
        }
        if (!has) {
            break;
        }
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    g_ExcludedPrograms.swap(excluded);
}

bool IsExplorerProcess() {
    static const bool isExplorer = [] {
        WCHAR path[MAX_PATH]{};
        DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
        if (!len || len >= ARRAYSIZE(path)) {
            return false;
        }
        const WCHAR* name = wcsrchr(path, L'\\');
        return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
    }();
    return isExplorer;
}

bool SpawnWorker(LPTHREAD_START_ROUTINE proc, void* param) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    for (size_t i = g_WorkerThreads.size(); i-- > 0;) {
        if (WaitForSingleObject(g_WorkerThreads[i], 0) == WAIT_OBJECT_0) {
            CloseHandle(g_WorkerThreads[i]);
            g_WorkerThreads.erase(g_WorkerThreads.begin() + i);
        }
    }
    HANDLE hThread = CreateThread(nullptr, 0, proc, param, 0, nullptr);
    if (!hThread) {
        return false;
    }
    g_WorkerThreads.push_back(hThread);
    return true;
}

bool IsExcludedProcess(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_ExcludedPrograms.empty()) {
            return false;
        }
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) {
        return false;
    }
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        return false;
    }
    WCHAR path[MAX_PATH]{};
    DWORD len = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(hProc, 0, path, &len) != 0;
    CloseHandle(hProc);
    if (!ok || !len) {
        return false;
    }

    std::wstring full = ToLowerTrimmed(path);
    size_t slash = full.find_last_of(L"\\/");
    std::wstring base =
        (slash == std::wstring::npos) ? full : full.substr(slash + 1);
    std::wstring baseNoExt = base;
    if (baseNoExt.size() > 4 &&
        baseNoExt.compare(baseNoExt.size() - 4, 4, L".exe") == 0) {
        baseNoExt.resize(baseNoExt.size() - 4);
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (const auto& e : g_ExcludedPrograms) {
        if (e == full || e == base || e == baseNoExt) {
            return true;
        }
    }
    return false;
}

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable,
                          sizeof(disable));
}

bool SetCloak(HWND hWnd, BOOL cloak) {
    return SUCCEEDED(
        DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak)));
}

bool IsWindowCloaked(HWND hWnd) {
    UINT cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

void UndoRisingHide(HWND hWnd, LONG_PTR originalExStyle, BOOL cloakHidden) {
    if (!IsWindow(hWnd)) {
        return;
    }
    if (cloakHidden) {
        SetCloak(hWnd, FALSE);
    } else {
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, originalExStyle);
        }
    }
    SetDwmTransitions(hWnd, TRUE);
}

bool IsOwnOverlayClass(HWND hWnd) {
    static constexpr const WCHAR* kOwnClasses[] = {
        L"WindhawkMacOS27DockWnd",
        L"WindhawkMacOSTahoeAppsHost",
        L"WindhawkMacOSTahoeFinderBar",
        L"WindhawkMacOSTahoeMinHost",
        L"WindhawkMacOSTahoeMinOverlay",
    };
    WCHAR cls[128]{};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    for (const WCHAR* name : kOwnClasses) {
        if (_wcsicmp(cls, name) == 0) {
            return true;
        }
    }
    return false;
}

bool ShouldAnimateWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) {
        return false;
    }
    if (IsOwnOverlayClass(hWnd)) {
        return false;
    }

    RECT r{};
    if (IsIconic(hWnd)) {
        WINDOWPLACEMENT wp{};
        wp.length = sizeof(wp);
        if (!GetWindowPlacement(hWnd, &wp)) {
            return false;
        }
        r = wp.rcNormalPosition;
        if ((wp.flags & WPF_RESTORETOMAXIMIZED) != 0) {
            return true;
        }
    } else if (!GetWindowRect(hWnd, &r)) {
        return false;
    }
    return (r.right - r.left) >= 40 && (r.bottom - r.top) >= 40;
}

bool IsLaunchWindow(HWND hWnd) {
    if (!hWnd || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CAPTION)) {
        return false;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }
    return ShouldAnimateWindow(hWnd);
}

bool IsAnimActive(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    return g_AnimActive.count(hWnd) != 0;
}

ID2D1PathGeometry* CreateQuadGeo(ID2D1Factory* factory,
                                 D2D1_POINT_2F p0,
                                 D2D1_POINT_2F p1,
                                 D2D1_POINT_2F p2,
                                 D2D1_POINT_2F p3) {
    ID2D1PathGeometry* geo = nullptr;
    factory->CreatePathGeometry(&geo);
    if (!geo) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geo->Open(&sink)) || !sink) {
        if (sink) {
            sink->Release();
        }
        geo->Release();
        return nullptr;
    }
    sink->BeginFigure(p0, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(p1);
    sink->AddLine(p2);
    sink->AddLine(p3);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geo;
}

ID2D1PathGeometry* CreateRoundedRectGeo(ID2D1Factory* factory,
                                        float L,
                                        float T,
                                        float R,
                                        float B) {
    ID2D1PathGeometry* geo = nullptr;
    factory->CreatePathGeometry(&geo);
    if (!geo) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geo->Open(&sink)) || !sink) {
        if (sink) {
            sink->Release();
        }
        geo->Release();
        return nullptr;
    }
    float w = R - L;
    float h = B - T;
    float r = fminf(12.0f, fminf(w, h) * 0.5f);
    constexpr float K = 0.5523f;

    sink->BeginFigure(D2D1::Point2F(L + r, T), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(R - r, T));
    {
        D2D1_BEZIER_SEGMENT s{
            D2D1::Point2F(R - r + K * r, T),
            D2D1::Point2F(R, T + r - K * r),
            D2D1::Point2F(R, T + r),
        };
        sink->AddBezier(s);
    }
    sink->AddLine(D2D1::Point2F(R, B - r));
    {
        D2D1_BEZIER_SEGMENT s{
            D2D1::Point2F(R, B - r + K * r),
            D2D1::Point2F(R - r + K * r, B),
            D2D1::Point2F(R - r, B),
        };
        sink->AddBezier(s);
    }
    sink->AddLine(D2D1::Point2F(L + r, B));
    {
        D2D1_BEZIER_SEGMENT s{
            D2D1::Point2F(L + r - K * r, B),
            D2D1::Point2F(L, B - r + K * r),
            D2D1::Point2F(L, B - r),
        };
        sink->AddBezier(s);
    }
    sink->AddLine(D2D1::Point2F(L, T + r));
    {
        D2D1_BEZIER_SEGMENT s{
            D2D1::Point2F(L, T + r - K * r),
            D2D1::Point2F(L + r - K * r, T),
            D2D1::Point2F(L + r, T),
        };
        sink->AddBezier(s);
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geo;
}

D2D1_POINT_2F BloatPoint(D2D1_POINT_2F p,
                         D2D1_POINT_2F c,
                         float amount = 0.5f) {
    float dx = p.x - c.x;
    float dy = p.y - c.y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f) {
        return p;
    }
    return D2D1::Point2F(p.x + (dx / len) * amount, p.y + (dy / len) * amount);
}

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hMainTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    HMONITOR mainMon = MonitorFromWindow(hMainTray, MONITOR_DEFAULTTOPRIMARY);
    if (hMon == mainMon || !hMon) {
        return hMainTray;
    }
    HWND hSecTray = nullptr;
    while ((hSecTray = FindWindowExW(nullptr, hSecTray,
                                     L"Shell_SecondaryTrayWnd", nullptr)) !=
           nullptr) {
        if (MonitorFromWindow(hSecTray, MONITOR_DEFAULTTONULL) == hMon) {
            return hSecTray;
        }
    }
    return hMainTray;
}

int GetTaskbarButtonX(HWND hWndApp, int fallbackX, HMONITOR hMon) {
    int targetX = fallbackX;
    bool uiaFound = false;

    std::wstring procNameLower;
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWndApp, &ownerPid);
    if (ownerPid) {
        HANDLE hProc =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ownerPid);
        if (hProc) {
            WCHAR exePath[MAX_PATH]{};
            DWORD exePathLen = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, exePath, &exePathLen)) {
                WCHAR* name = wcsrchr(exePath, L'\\');
                if (name) {
                    procNameLower = (name + 1);
                    size_t dotPos = procNameLower.find(L'.');
                    if (dotPos != std::wstring::npos) {
                        procNameLower = procNameLower.substr(0, dotPos);
                    }
                    std::transform(procNameLower.begin(), procNameLower.end(),
                                   procNameLower.begin(), ::towlower);
                }
            }
            CloseHandle(hProc);
        }
    }

    std::wstring processKey = procNameLower;
    if (!processKey.empty() && hMon) {
        processKey += L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
    }

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool coInit = (hr == S_OK || hr == S_FALSE);

    if (hr == S_OK || hr == S_FALSE || hr == RPC_E_CHANGED_MODE) {
        IUIAutomation* pAutomation = nullptr;
        HRESULT hrUia = CoCreateInstance(
            __uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IUIAutomation), reinterpret_cast<void**>(&pAutomation));
        if (FAILED(hrUia)) {
            hrUia = CoCreateInstance(__uuidof(CUIAutomation), nullptr,
                                     CLSCTX_INPROC_SERVER,
                                     __uuidof(IUIAutomation),
                                     reinterpret_cast<void**>(&pAutomation));
        }

        if (SUCCEEDED(hrUia) && pAutomation) {
            HWND hTray = FindTaskbarForMonitor(hMon);
            if (hTray) {
                IUIAutomationElement* pTrayElement = nullptr;
                if (SUCCEEDED(
                        pAutomation->ElementFromHandle(hTray, &pTrayElement)) &&
                    pTrayElement) {
                    WCHAR titleW[512]{};
                    GetWindowTextW(hWndApp, titleW, ARRAYSIZE(titleW));
                    std::wstring titleLower = titleW;
                    std::transform(titleLower.begin(), titleLower.end(),
                                   titleLower.begin(), ::towlower);

                    std::wstring procHintLower = procNameLower;
                    if (procNameLower == L"chrome") {
                        procHintLower = L"google chrome";
                    } else if (procNameLower == L"msedge") {
                        procHintLower = L"microsoft edge";
                    }

                    IUIAutomationCondition* pButtonCond = nullptr;
                    IUIAutomationCondition* pListItemCond = nullptr;
                    IUIAutomationCondition* pOrCond = nullptr;

                    VARIANT varBtn{};
                    varBtn.vt = VT_I4;
                    varBtn.lVal = UIA_ButtonControlTypeId;
                    pAutomation->CreatePropertyCondition(
                        UIA_ControlTypePropertyId, varBtn, &pButtonCond);

                    VARIANT varList{};
                    varList.vt = VT_I4;
                    varList.lVal = UIA_ListItemControlTypeId;
                    pAutomation->CreatePropertyCondition(
                        UIA_ControlTypePropertyId, varList, &pListItemCond);

                    if (pButtonCond && pListItemCond) {
                        pAutomation->CreateOrCondition(pButtonCond, pListItemCond,
                                                       &pOrCond);
                    }

                    IUIAutomationElementArray* pArray = nullptr;
                    if (pOrCond &&
                        SUCCEEDED(pTrayElement->FindAll(TreeScope_Descendants,
                                                        pOrCond, &pArray)) &&
                        pArray) {
                        int length = 0;
                        pArray->get_Length(&length);

                        MONITORINFO mi{};
                        mi.cbSize = sizeof(mi);
                        GetMonitorInfoW(hMon, &mi);
                        int monRight = mi.rcMonitor.right;

                        int bestScore = 0;
                        for (int i = 0; i < length; ++i) {
                            IUIAutomationElement* pItem = nullptr;
                            if (SUCCEEDED(pArray->GetElement(i, &pItem)) &&
                                pItem) {
                                BSTR name = nullptr;
                                if (SUCCEEDED(pItem->get_CurrentName(&name)) &&
                                    name) {
                                    std::wstring uiaNameLower = name;
                                    std::transform(uiaNameLower.begin(),
                                                   uiaNameLower.end(),
                                                   uiaNameLower.begin(),
                                                   ::towlower);

                                    if (!uiaNameLower.empty()) {
                                        int score = 0;
                                        if (titleLower == uiaNameLower) {
                                            score += 1000;
                                        }
                                        if (!titleLower.empty() &&
                                            titleLower.find(uiaNameLower) !=
                                                std::wstring::npos) {
                                            score += 500;
                                        }
                                        if (!titleLower.empty() &&
                                            uiaNameLower.find(titleLower) !=
                                                std::wstring::npos) {
                                            score += 500;
                                        }
                                        if (!procNameLower.empty() &&
                                            uiaNameLower.find(procNameLower) !=
                                                std::wstring::npos) {
                                            score += 400;
                                        }
                                        if (!procHintLower.empty() &&
                                            procHintLower != procNameLower &&
                                            uiaNameLower.find(procHintLower) !=
                                                std::wstring::npos) {
                                            score += 900;
                                        }

                                        std::wstring currentWord;
                                        for (wchar_t c : titleLower) {
                                            if (iswalnum(c)) {
                                                currentWord += c;
                                            } else {
                                                if (currentWord.length() >= 4 &&
                                                    uiaNameLower.find(
                                                        currentWord) !=
                                                        std::wstring::npos) {
                                                    score += 50;
                                                }
                                                currentWord.clear();
                                            }
                                        }
                                        if (currentWord.length() >= 4 &&
                                            uiaNameLower.find(currentWord) !=
                                                std::wstring::npos) {
                                            score += 50;
                                        }

                                        if (uiaNameLower.find(L"start") !=
                                            std::wstring::npos) {
                                            score -= 500;
                                        }
                                        if (uiaNameLower.find(L"search") !=
                                            std::wstring::npos) {
                                            score -= 500;
                                        }
                                        if (uiaNameLower.find(L"task view") !=
                                            std::wstring::npos) {
                                            score -= 500;
                                        }
                                        if (uiaNameLower.find(L"widgets") !=
                                            std::wstring::npos) {
                                            score -= 500;
                                        }

                                        if (score > bestScore) {
                                            RECT bRect{};
                                            if (SUCCEEDED(
                                                    pItem
                                                        ->get_CurrentBoundingRectangle(
                                                            &bRect))) {
                                                if (bRect.right > bRect.left &&
                                                    bRect.left <
                                                        monRight - 50) {
                                                    bestScore = score;
                                                    targetX =
                                                        bRect.left +
                                                        (bRect.right -
                                                         bRect.left) /
                                                            2;
                                                    uiaFound = true;
                                                }
                                            }
                                        }
                                    }
                                    SysFreeString(name);
                                }
                                pItem->Release();
                            }
                        }
                        pArray->Release();
                    }
                    if (pButtonCond) {
                        pButtonCond->Release();
                    }
                    if (pListItemCond) {
                        pListItemCond->Release();
                    }
                    if (pOrCond) {
                        pOrCond->Release();
                    }
                    pTrayElement->Release();
                }
            }
            pAutomation->Release();
        }
        if (coInit) {
            CoUninitialize();
        }
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    if (uiaFound) {
        g_IconPositions[hWndApp] = targetX;
        if (!processKey.empty()) {
            g_ProcessIconPositions[processKey] = targetX;
        }
    } else if (g_IconPositions.count(hWndApp)) {
        targetX = g_IconPositions[hWndApp];
    } else if (!processKey.empty() &&
               g_ProcessIconPositions.count(processKey)) {
        targetX = g_ProcessIconPositions[processKey];
    }

    return targetX;
}

uint32_t QueryDockIconPacked(HWND targetHwnd) {
    if (!targetHwnd || !g_msgQueryIconCenter) {
        return 0;
    }
    HWND dock = FindWindowW(kDockClassName, nullptr);
    if (!dock) {
        return 0;
    }
    DWORD_PTR packed = 0;
    if (SendMessageTimeoutW(dock, g_msgQueryIconCenter,
                            reinterpret_cast<WPARAM>(targetHwnd), 0,
                            SMTO_ABORTIFHUNG | SMTO_NORMAL, 50, &packed)) {
        return static_cast<uint32_t>(packed);
    }
    return 0;
}

void FinishClose(AnimData* data) {
    HWND hWnd = data->hRealWnd;
    if (!IsWindow(hWnd)) {
        return;
    }
    auto showAsyncFn =
        ShowWindowAsync_Original ? ShowWindowAsync_Original : ShowWindowAsync;

    if (g_unloading.load(std::memory_order_relaxed)) {
        SetCloak(hWnd, FALSE);
        SetDwmTransitions(hWnd, TRUE);
        RemovePropW(hWnd, L"GenieCloseAnimating");
        RemovePropW(hWnd, L"GenieCloseBypass");
        if (data->closeMsg == kAsyncHideMsg) {
            showAsyncFn(hWnd, SW_HIDE);
        } else if (data->closeMsg == WM_SYSCOMMAND) {
            PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        } else if (data->closeMsg == WM_CLOSE) {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        return;
    }
    if (data->closeMsg == WM_DESTROY) {
        return;
    }

    SetPropW(hWnd, L"GenieCloseHandled", reinterpret_cast<HANDLE>(1));
    int waitSteps = 0;
    if (data->closeMsg == kAsyncHideMsg) {
        showAsyncFn(hWnd, SW_HIDE);
        waitSteps = 50;
    } else {
        SetPropW(hWnd, L"GenieCloseBypass", reinterpret_cast<HANDLE>(1));
        if (data->closeMsg == WM_SYSCOMMAND) {
            PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        } else {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        waitSteps = 200;
    }

    for (int i = 0; i < waitSteps && IsWindow(hWnd) && IsWindowVisible(hWnd) &&
                    !g_unloading.load(std::memory_order_relaxed);
         ++i) {
        Sleep(10);
    }
    if (IsWindow(hWnd)) {
        SetCloak(hWnd, FALSE);
        RemovePropW(hWnd, L"GenieCloseAnimating");
        SetDwmTransitions(hWnd, TRUE);
        if (IsWindowVisible(hWnd)) {
            RemovePropW(hWnd, L"GenieCloseHandled");
            RemovePropW(hWnd, L"GenieCloseBypass");
        }
    }
}

void SkipRender(AnimData* data) {
    if (!data->isClosing || !data->hFirstFrameShown) {
        return;
    }
    SetEvent(data->hFirstFrameShown);
    int remaining = data->durationMs;
    while (remaining > 0 && !g_unloading.load(std::memory_order_relaxed)) {
        int chunk = remaining < 20 ? remaining : 20;
        Sleep(chunk);
        remaining -= chunk;
    }
}

void CalculateLampVertexMacOS(float tx,
                              float ty,
                              float p,
                              const Geometry& w,
                              const Geometry& i,
                              float* outX,
                              float* outY) {
    constexpr float split = 0.3f;
    float k = (p <= split) ? (p / split) : 1.0f;
    float j = (p > split) ? ((p - split) / (1.0f - split)) : 0.0f;

    float expandHeight = (i.y - w.y - w.height);
    float fullHeight = (i.y - w.y) - (expandHeight * (1.0f - k));
    float height = fullHeight - (j * fullHeight);

    float y = ty * height;
    float x = tx * (i.width) +
              tx * (w.width - i.width) * (1.0f - j) * (1.0f - ty) +
              tx * (w.width - i.width) * (1.0f - k) * ty;

    float offsetX =
        (i.x - w.x) * (y / (fullHeight + 0.1f)) * k + (i.x - w.x) * j;
    float offsetY = i.y - w.y - height - (expandHeight * (1.0f - k));

    float denom = (fabsf(fullHeight) < 0.001f)
                      ? (fullHeight < 0.0f ? -0.001f : 0.001f)
                      : fullHeight;
    float effectX = sinf(((height - y) / denom) * 2.0f * kPi + kPi) *
                    (w.x + w.width * tx - (i.x + i.width * tx)) / 7.0f * k;

    *outX = w.x + x + offsetX + effectX;
    *outY = w.y + y + offsetY;
}

void CalculateLampVertexForEdge(float tx,
                                float ty,
                                float p,
                                const Geometry& w,
                                const Geometry& i,
                                int edge,
                                float* outX,
                                float* outY) {
    if (edge == EdgeRight) {
        Geometry wt{w.y, w.x, w.height, w.width};
        Geometry it{i.y, i.x, i.height, i.width};
        CalculateLampVertexMacOS(ty, tx, p, wt, it, outY, outX);
        return;
    }
    if (edge == EdgeLeft) {
        Geometry wt{w.y, -(w.x + w.width), w.height, w.width};
        Geometry it{i.y, -(i.x + i.width), i.height, i.width};
        float xM = 0.0f;
        CalculateLampVertexMacOS(ty, 1.0f - tx, p, wt, it, outY, &xM);
        *outX = -xM;
        return;
    }
    if (edge == EdgeTop || i.y + i.height * 0.5f < w.y + w.height * 0.5f) {
        Geometry wm{w.x, -(w.y + w.height), w.width, w.height};
        Geometry im{i.x, -(i.y + i.height), i.width, i.height};
        float yM = 0.0f;
        CalculateLampVertexMacOS(tx, 1.0f - ty, p, wm, im, outX, &yM);
        *outY = -yM;
        return;
    }
    CalculateLampVertexMacOS(tx, ty, p, w, i, outX, outY);
}

RECT GetMonitorUnion(const RECT& winRect, HMONITOR hDockMon) {
    RECT winMon{};
    HMONITOR hWinMon = MonitorFromRect(&winRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO wmi{};
    wmi.cbSize = sizeof(wmi);
    if (hWinMon && GetMonitorInfoW(hWinMon, &wmi)) {
        winMon = wmi.rcMonitor;
    } else {
        winMon = winRect;
    }

    RECT dockMon{};
    MONITORINFO dmi{};
    dmi.cbSize = sizeof(dmi);
    if (hDockMon && GetMonitorInfoW(hDockMon, &dmi)) {
        dockMon = dmi.rcMonitor;
    } else {
        dockMon = winMon;
    }

    RECT u{};
    u.left = std::min(winMon.left, dockMon.left);
    u.top = std::min(winMon.top, dockMon.top);
    u.right = std::max(winMon.right, dockMon.right);
    u.bottom = std::max(winMon.bottom, dockMon.bottom);
    return u;
}

void ClampGenieCanvas(int& left,
                      int& top,
                      int& right,
                      int& bottom,
                      const RECT& winRect,
                      const RECT& iconRect,
                      const RECT& monUnion) {
    left = std::max<int>(left, monUnion.left);
    right = std::min<int>(right, monUnion.right);
    top = std::max<int>(top, monUnion.top);
    bottom = std::min<int>(bottom, monUnion.bottom);

    left = std::min<int>(left, std::min<int>(winRect.left, iconRect.left));
    right = std::max<int>(right, std::max<int>(winRect.right, iconRect.right));
    top = std::min<int>(top, std::min<int>(winRect.top, iconRect.top));
    bottom =
        std::max<int>(bottom, std::max<int>(winRect.bottom, iconRect.bottom));
}

bool ClipGenieCanvasToDesktop(int& left, int& top, int& right, int& bottom) {
    const int vsLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vsTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vsWidth > 0 && vsHeight > 0) {
        left = std::max(left, vsLeft);
        top = std::max(top, vsTop);
        right = std::min(right, vsLeft + vsWidth);
        bottom = std::min(bottom, vsTop + vsHeight);
    }
    if (right <= left || bottom <= top) {
        return false;
    }
    return static_cast<long long>(right - left) * (bottom - top) <=
           kMaxCanvasPixels;
}

double AcquireFrameIntervalMs() {
    double frameIntervalMs = 1000.0 / 240.0;
    DWM_TIMING_INFO ti{};
    ti.cbSize = sizeof(ti);
    if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &ti)) &&
        ti.rateRefresh.uiDenominator > 0 && ti.rateRefresh.uiNumerator > 0) {
        double hz = static_cast<double>(ti.rateRefresh.uiNumerator) /
                    static_cast<double>(ti.rateRefresh.uiDenominator);
        frameIntervalMs = 1000.0 / (hz * 2.0);
    }
    return frameIntervalMs;
}

HANDLE CreateFramePacerTimer() {
    HANDLE hTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!hTimer) {
        hTimer = CreateWaitableTimerExW(nullptr, nullptr, 0,
                                        TIMER_MODIFY_STATE | SYNCHRONIZE);
    }
    return hTimer;
}

void PaceAnimationFrame(HANDLE hFrameTimer,
                        double frameIntervalMs,
                        LARGE_INTEGER qpcFreq,
                        LARGE_INTEGER qpcFrameStart) {
    if (hFrameTimer) {
        LARGE_INTEGER qpcAfter{};
        QueryPerformanceCounter(&qpcAfter);
        double spentMs =
            static_cast<double>(qpcAfter.QuadPart - qpcFrameStart.QuadPart) *
            1000.0 / static_cast<double>(qpcFreq.QuadPart);
        double waitMs = frameIntervalMs - spentMs;
        if (waitMs > 0.1) {
            LARGE_INTEGER due{};
            due.QuadPart = -static_cast<LONGLONG>(waitMs * 10000.0);
            SetWaitableTimer(hFrameTimer, &due, 0, nullptr, nullptr, FALSE);
            WaitForSingleObject(hFrameTimer, 100);
        } else {
            SwitchToThread();
        }
    } else {
        Sleep(1);
    }
}

void CreateGhostCanvas(GhostCanvas& gc,
                       int left,
                       int top,
                       int right,
                       int bottom,
                       bool noActivate) {
    gc.left = left;
    gc.top = top;
    gc.width = std::max(1, right - left);
    gc.height = std::max(1, bottom - top);

    gc.hScreenDC = GetDC(nullptr);
    if (gc.hScreenDC) {
        gc.hMemDC = CreateCompatibleDC(gc.hScreenDC);
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = gc.width;
    bmi.bmiHeader.biHeight = -gc.height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    if (gc.hMemDC) {
        gc.hTargetBmp = CreateDIBSection(gc.hScreenDC, &bmi, DIB_RGB_COLORS,
                                         &gc.pTargetBits, nullptr, 0);
    }
    if (gc.hTargetBmp) {
        gc.hOldBmp =
            static_cast<HBITMAP>(SelectObject(gc.hMemDC, gc.hTargetBmp));
    }

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                    WS_EX_TRANSPARENT | (noActivate ? WS_EX_NOACTIVATE : 0);
    gc.hGhost = CreateWindowExW(exStyle, L"STATIC", nullptr, WS_POPUP, gc.left,
                                gc.top, gc.width, gc.height, nullptr, nullptr,
                                nullptr, nullptr);

    gc.ok = gc.hScreenDC && gc.hMemDC && gc.hTargetBmp && gc.pTargetBits &&
            gc.hOldBmp && gc.hGhost;
}

void DestroyGhostCanvas(GhostCanvas& gc) {
    auto showFn = ShowWindow_Original ? ShowWindow_Original : ShowWindow;
    auto destroyFn =
        DestroyWindow_Original ? DestroyWindow_Original : DestroyWindow;
    if (gc.hGhost) {
        showFn(gc.hGhost, SW_HIDE);
        destroyFn(gc.hGhost);
        gc.hGhost = nullptr;
    }
    if (gc.hMemDC) {
        if (gc.hOldBmp) {
            SelectObject(gc.hMemDC, gc.hOldBmp);
        }
        DeleteDC(gc.hMemDC);
        gc.hMemDC = nullptr;
    }
    if (gc.hTargetBmp) {
        DeleteObject(gc.hTargetBmp);
        gc.hTargetBmp = nullptr;
    }
    if (gc.hScreenDC) {
        ReleaseDC(nullptr, gc.hScreenDC);
        gc.hScreenDC = nullptr;
    }
}

void FinishAnimationTeardown(AnimData* data) {
    if (data->isRising && IsWindow(data->hRealWnd)) {
        if (data->hiddenByCloak) {
            SetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE,
                                  data->originalExStyle);
            }
        }
    }

    if (!data->deferredMinimize && !data->isClosing &&
        IsWindow(data->hRealWnd)) {
        SetDwmTransitions(data->hRealWnd, TRUE);
    }
    if (data->isClosing) {
        FinishClose(data);
    }

    if (data->hBitmap) {
        DeleteObject(data->hBitmap);
        data->hBitmap = nullptr;
    }

    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
        data->hFirstFrameShown = nullptr;
    }

    if (data->deferredMinimize) {
        int unhideMs = data->unhideDurationMs;
        int animMs = data->durationMs;
        if (data->requestedUnhide && unhideMs > animMs) {
            int remaining = unhideMs - animMs;
            while (remaining > 0 &&
                   !g_unloading.load(std::memory_order_relaxed)) {
                int chunk = remaining < 20 ? remaining : 20;
                Sleep(chunk);
                remaining -= chunk;
            }
        }
        if (!g_unloading.load(std::memory_order_relaxed) &&
            IsWindow(data->hRealWnd)) {
            SetPropW(data->hRealWnd, L"GenieBypass",
                     reinterpret_cast<HANDLE>(1));
            SendMessageTimeoutW(data->hRealWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0,
                                SMTO_NORMAL, 1000, nullptr);
            RemovePropW(data->hRealWnd, L"GenieBypass");
        }
        if (IsWindow(data->hRealWnd)) {
            SetCloak(data->hRealWnd, FALSE);
            SetDwmTransitions(data->hRealWnd, TRUE);
        }
        if (data->requestedUnhide &&
            !g_unloading.load(std::memory_order_relaxed)) {
            if (data->hNextApp && IsWindow(data->hNextApp) &&
                IsWindowVisible(data->hNextApp)) {
                SetForegroundWindow(data->hNextApp);
            } else {
                SetForegroundWindow(FindWindowW(L"Progman", nullptr));
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
}

Geometry ComputeTargetGeometry(const AnimData* data, const MONITORINFO& mi) {
    if (data->isClosing) {
        const int dir = g_closeDirection.load(std::memory_order_relaxed);
        Geometry ig{0.0f, 0.0f, 22.0f, 22.0f};
        if (dir == 0) {
            ig.x = static_cast<float>(mi.rcMonitor.left);
            ig.y = static_cast<float>(mi.rcMonitor.top + mi.rcMonitor.bottom) *
                   0.5f;
        } else if (dir == 1) {
            ig.x = static_cast<float>(mi.rcMonitor.right - 22);
            ig.y = static_cast<float>(mi.rcMonitor.top + mi.rcMonitor.bottom) *
                   0.5f;
        } else if (dir == 3) {
            ig.x =
                static_cast<float>(mi.rcMonitor.left + mi.rcMonitor.right) *
                    0.5f -
                11.0f;
            ig.y = static_cast<float>(mi.rcMonitor.bottom) - ig.height;
        } else {
            ig.x =
                static_cast<float>(mi.rcMonitor.left + mi.rcMonitor.right) *
                    0.5f -
                11.0f;
            ig.y = static_cast<float>(mi.rcMonitor.top);
        }
        return ig;
    }

    if (data->hasDockTarget) {
        return {static_cast<float>(data->targetDockX - 24),
                static_cast<float>(data->targetDockY - 24), 48.0f, 48.0f};
    }

    UINT dpiX = 96;
    UINT dpiY = 96;
    HMODULE hShcore =
        LoadLibraryExW(L"Shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hShcore) {
        using GetDpiForMonitor_t =
            HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
        auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
            GetProcAddress(hShcore, "GetDpiForMonitor"));
        if (pGetDpiForMonitor && data->hMon) {
            pGetDpiForMonitor(data->hMon, 0, &dpiX, &dpiY);
        }
        FreeLibrary(hShcore);
    }

    float dpiScale = static_cast<float>(dpiY) / 96.0f;
    float scaledTaskbarHeight = kCompatTaskbarHeight * dpiScale;
    float scaledIconSize = kCompatIconSize * dpiScale;
    float taskbarTop =
        static_cast<float>(mi.rcMonitor.bottom) - scaledTaskbarHeight;

    if (g_fallbackTarget.load(std::memory_order_relaxed) ==
        static_cast<int>(FallbackTarget::Taskbar)) {
        if (HWND hTray = FindTaskbarForMonitor(data->hMon)) {
            RECT tr{};
            if (GetWindowRect(hTray, &tr) && (tr.bottom - tr.top) > 0) {
                taskbarTop = static_cast<float>(tr.top);
            }
        }
    }

    return {static_cast<float>(data->targetDockX) - 11.0f, taskbarTop, 22.0f,
            scaledIconSize};
}

int EffectiveCloseOrDockEdge(const AnimData* data) {
    if (data->isClosing) {
        const int dir = g_closeDirection.load(std::memory_order_relaxed);
        if (dir == 0) {
            return EdgeLeft;
        }
        if (dir == 1) {
            return EdgeRight;
        }
        if (dir == 3) {
            return EdgeBottom;
        }
        return EdgeTop;
    }
    return data->dockEdge;
}

DWORD WINAPI AnimThreadModernGenie(LPVOID lpParam) {
    AnimData* data = static_cast<AnimData*>(lpParam);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    auto showFn = ShowWindow_Original ? ShowWindow_Original : ShowWindow;

    Geometry wGeom{
        static_cast<float>(data->targetRect.left),
        static_cast<float>(data->targetRect.top),
        static_cast<float>(data->width),
        static_cast<float>(data->height),
    };

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!data->hMon || !GetMonitorInfoW(data->hMon, &mi)) {
        mi.rcMonitor = data->targetRect;
    }
    Geometry iGeom = ComputeTargetGeometry(data, mi);
    int edge = EffectiveCloseOrDockEdge(data);

    int xTiles = g_tileCount.load(std::memory_order_relaxed);
    int yTiles = xTiles;

    int boundLeft = 0;
    int boundRight = 0;
    int boundTop = 0;
    int boundBottom = 0;

    if (edge == EdgeLeft || edge == EdgeRight) {
        float swayY =
            fmaxf(fabsf(wGeom.y - iGeom.y),
                  fabsf((wGeom.y + wGeom.height) - (iGeom.y + iGeom.height))) /
            7.0f;
        int padY = static_cast<int>(fmaxf(wGeom.height * 0.5f, swayY) + 2.0f);
        boundLeft = static_cast<int>(fminf(wGeom.x, iGeom.x));
        boundRight =
            static_cast<int>(fmaxf(wGeom.x + wGeom.width, iGeom.x + iGeom.width));
        boundTop = static_cast<int>(fminf(wGeom.y, iGeom.y)) - padY;
        boundBottom = static_cast<int>(
                          fmaxf(wGeom.y + wGeom.height, iGeom.y + iGeom.height)) +
                      padY;
    } else {
        float swayX =
            fmaxf(fabsf(wGeom.x - iGeom.x),
                  fabsf((wGeom.x + wGeom.width) - (iGeom.x + iGeom.width))) /
            7.0f;
        int padX = static_cast<int>(fmaxf(wGeom.width * 0.5f, swayX) + 2.0f);
        boundLeft = static_cast<int>(fminf(wGeom.x, iGeom.x)) - padX;
        boundRight =
            static_cast<int>(fmaxf(wGeom.x + wGeom.width, iGeom.x + iGeom.width)) +
            padX;
        boundTop = static_cast<int>(fminf(wGeom.y, iGeom.y));
        boundBottom = static_cast<int>(
            fmaxf(wGeom.y + wGeom.height, iGeom.y + iGeom.height));
    }

    RECT iconRect{
        static_cast<LONG>(std::floor(iGeom.x)),
        static_cast<LONG>(std::floor(iGeom.y)),
        static_cast<LONG>(std::ceil(iGeom.x + iGeom.width)),
        static_cast<LONG>(std::ceil(iGeom.y + iGeom.height)),
    };
    RECT monUnion = GetMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom,
                     data->targetRect, iconRect, monUnion);
    bool gdiOk =
        ClipGenieCanvasToDesktop(boundLeft, boundTop, boundRight, boundBottom);

    GhostCanvas gc;
    if (gdiOk) {
        CreateGhostCanvas(gc, boundLeft, boundTop, boundRight, boundBottom,
                          true);
        gdiOk = gc.ok;
    }

    ID2D1Factory* factory = gdiOk ? EnsureD2DFactory() : nullptr;
    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1Bitmap* snapshotBmp = nullptr;
    ID2D1BitmapBrush* bmpBrush = nullptr;

    if (factory && gdiOk) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
            D2D1_FEATURE_LEVEL_DEFAULT);
        factory->CreateDCRenderTarget(&rtProps, &rt);
        if (rt) {
            rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            rt->CreateBitmap(D2D1::SizeU(static_cast<UINT32>(data->width),
                                         static_cast<UINT32>(data->height)),
                             data->pBits, static_cast<UINT32>(data->width * 4),
                             bmpProps, &snapshotBmp);
            if (snapshotBmp) {
                D2D1_BITMAP_BRUSH_PROPERTIES brushProps =
                    D2D1::BitmapBrushProperties(
                        D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                rt->CreateBitmapBrush(snapshotBmp, &brushProps, nullptr,
                                      &bmpBrush);
            }
        }
    }
    bool d2dOk = (gdiOk && rt && bmpBrush);

    const double animDur = static_cast<double>(data->durationMs);
    LARGE_INTEGER qpcFreq{};
    LARGE_INTEGER qpcStart{};
    LARGE_INTEGER qpcNow{};
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    std::vector<std::vector<D2D1_POINT_2F>> grid(
        static_cast<size_t>(yTiles) + 1,
        std::vector<D2D1_POINT_2F>(static_cast<size_t>(xTiles) + 1));

    ID2D1PathGeometry* cachedOutlineGeo = nullptr;
    float cachedLeft = -1e9f;
    float cachedRight = -1e9f;
    float cachedTop = -1e9f;
    float cachedBottom = -1e9f;

    BOOL firstFrame = TRUE;
    double frameIntervalMs = AcquireFrameIntervalMs();
    HANDLE hFrameTimer = CreateFramePacerTimer();

    if (d2dOk) {
        for (;;) {
            QueryPerformanceCounter(&qpcNow);
            const LARGE_INTEGER qpcFrameStart = qpcNow;
            double elapsedMs =
                static_cast<double>(qpcNow.QuadPart - qpcStart.QuadPart) *
                1000.0 / static_cast<double>(qpcFreq.QuadPart);
            BOOL lastFrame = (elapsedMs >= animDur);

            float rawP =
                static_cast<float>(fmin(elapsedMs / animDur, 1.0));
            float easedP = rawP * rawP * (3.0f - 2.0f * rawP);
            float t = data->isRising ? (1.0f - easedP) : easedP;

            RECT bindRect{0, 0, gc.width, gc.height};
            rt->BindDC(gc.hMemDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

            for (int y = 0; y <= yTiles; ++y) {
                for (int x = 0; x <= xTiles; ++x) {
                    float tx = static_cast<float>(x) / xTiles;
                    float ty = static_cast<float>(y) / yTiles;
                    float px = 0.0f;
                    float py = 0.0f;
                    CalculateLampVertexForEdge(tx, ty, t, wGeom, iGeom, edge,
                                               &px, &py);
                    grid[y][x] = D2D1::Point2F(px - gc.left, py - gc.top);
                }
            }

            float left = grid[0][0].x;
            float right = grid[0][0].x;
            float top = grid[0][0].y;
            float bottom = grid[0][0].y;
            for (int gy = 0; gy <= yTiles; ++gy) {
                for (int gx = 0; gx <= xTiles; ++gx) {
                    left = fminf(left, grid[gy][gx].x);
                    right = fmaxf(right, grid[gy][gx].x);
                    top = fminf(top, grid[gy][gx].y);
                    bottom = fmaxf(bottom, grid[gy][gx].y);
                }
            }

            bool degenerate = (right - left) < 2.0f || (bottom - top) < 2.0f;

            bool needRebuild = (cachedOutlineGeo == nullptr) ||
                               fabsf(left - cachedLeft) > 0.5f ||
                               fabsf(right - cachedRight) > 0.5f ||
                               fabsf(top - cachedTop) > 0.5f ||
                               fabsf(bottom - cachedBottom) > 0.5f;
            if (needRebuild) {
                if (cachedOutlineGeo) {
                    cachedOutlineGeo->Release();
                    cachedOutlineGeo = nullptr;
                }
                cachedLeft = left;
                cachedRight = right;
                cachedTop = top;
                cachedBottom = bottom;
                cachedOutlineGeo =
                    CreateRoundedRectGeo(factory, left, top, right, bottom);
            }

            if (cachedOutlineGeo && !degenerate) {
                D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                layerParams.geometricMask = cachedOutlineGeo;
                layerParams.maskAntialiasMode =
                    D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
                rt->PushLayer(&layerParams, nullptr);

                for (int y = 0; y < yTiles; ++y) {
                    for (int x = 0; x < xTiles; ++x) {
                        D2D1_POINT_2F p1 = grid[y][x];
                        D2D1_POINT_2F p2 = grid[y][x + 1];
                        D2D1_POINT_2F p3 = grid[y + 1][x];
                        D2D1_POINT_2F p4 = grid[y + 1][x + 1];

                        D2D1_POINT_2F c = D2D1::Point2F(
                            (p1.x + p2.x + p3.x + p4.x) / 4.0f,
                            (p1.y + p2.y + p3.y + p4.y) / 4.0f);

                        float quadW = fmaxf(fabsf(p2.x - p1.x),
                                            fabsf(p4.x - p3.x));
                        float quadH = fmaxf(fabsf(p3.y - p1.y),
                                            fabsf(p4.y - p2.y));
                        float bloatAmt = fminf(quadW, quadH) * 0.05f;
                        bloatAmt = fmaxf(0.20f, fminf(0.40f, bloatAmt));

                        ID2D1PathGeometry* quadGeo = CreateQuadGeo(
                            factory, BloatPoint(p1, c, bloatAmt),
                            BloatPoint(p2, c, bloatAmt),
                            BloatPoint(p4, c, bloatAmt),
                            BloatPoint(p3, c, bloatAmt));
                        if (quadGeo) {
                            float sx =
                                (static_cast<float>(x) / xTiles) * wGeom.width;
                            float sy =
                                (static_cast<float>(y) / yTiles) * wGeom.height;
                            float sw = wGeom.width / xTiles;
                            float sh = wGeom.height / yTiles;

                            float m11 = (p2.x - p1.x) / sw;
                            float m12 = (p2.y - p1.y) / sw;
                            float m21 = (p3.x - p1.x) / sh;
                            float m22 = (p3.y - p1.y) / sh;
                            float m31 = p1.x - sx * m11 - sy * m21;
                            float m32 = p1.y - sx * m12 - sy * m22;

                            bmpBrush->SetTransform(D2D1::Matrix3x2F(
                                m11, m12, m21, m22, m31, m32));
                            rt->FillGeometry(quadGeo, bmpBrush);
                            quadGeo->Release();
                        }
                    }
                }
                rt->PopLayer();
            }
            rt->EndDraw();

            if (!degenerate) {
                POINT ptDst{gc.left, gc.top};
                SIZE sz{gc.width, gc.height};
                POINT ptSrc{0, 0};
                BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                UpdateLayeredWindow(gc.hGhost, gc.hScreenDC, &ptDst, &sz,
                                    gc.hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
                if (firstFrame) {
                    showFn(gc.hGhost, SW_SHOWNOACTIVATE);
                }
            }

            if (lastFrame || g_unloading.load(std::memory_order_relaxed)) {
                break;
            }

            if (firstFrame && !degenerate) {
                DwmFlush();
                firstFrame = FALSE;
                if (data->hFirstFrameShown) {
                    SetEvent(data->hFirstFrameShown);
                }
            }

            PaceAnimationFrame(hFrameTimer, frameIntervalMs, qpcFreq,
                               qpcFrameStart);
        }
    }

    if (hFrameTimer) {
        CloseHandle(hFrameTimer);
    }
    if (cachedOutlineGeo) {
        cachedOutlineGeo->Release();
    }
    if (!d2dOk) {
        SkipRender(data);
    }

    if (bmpBrush) {
        bmpBrush->Release();
    }
    if (snapshotBmp) {
        snapshotBmp->Release();
    }
    if (rt) {
        rt->Release();
    }
    DestroyGhostCanvas(gc);
    FinishAnimationTeardown(data);
    return 0;
}

DWORD WINAPI AnimThreadScale(LPVOID lpParam) {
    AnimData* data = static_cast<AnimData*>(lpParam);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    auto showFn = ShowWindow_Original ? ShowWindow_Original : ShowWindow;

    Geometry wGeom{
        static_cast<float>(data->targetRect.left),
        static_cast<float>(data->targetRect.top),
        static_cast<float>(data->width),
        static_cast<float>(data->height),
    };

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!data->hMon || !GetMonitorInfoW(data->hMon, &mi)) {
        mi.rcMonitor = data->targetRect;
    }
    Geometry iGeom = ComputeTargetGeometry(data, mi);

    int boundLeft = static_cast<int>(floorf(fminf(wGeom.x, iGeom.x))) - 4;
    int boundTop = static_cast<int>(floorf(fminf(wGeom.y, iGeom.y))) - 4;
    int boundRight = static_cast<int>(ceilf(
                         fmaxf(wGeom.x + wGeom.width, iGeom.x + iGeom.width))) +
                     4;
    int boundBottom =
        static_cast<int>(
            ceilf(fmaxf(wGeom.y + wGeom.height, iGeom.y + iGeom.height))) +
        4;

    RECT iconRect{
        static_cast<LONG>(std::floor(iGeom.x)),
        static_cast<LONG>(std::floor(iGeom.y)),
        static_cast<LONG>(std::ceil(iGeom.x + iGeom.width)),
        static_cast<LONG>(std::ceil(iGeom.y + iGeom.height)),
    };
    RECT monUnion = GetMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom,
                     data->targetRect, iconRect, monUnion);
    bool gdiOk =
        ClipGenieCanvasToDesktop(boundLeft, boundTop, boundRight, boundBottom);

    GhostCanvas gc;
    if (gdiOk) {
        CreateGhostCanvas(gc, boundLeft, boundTop, boundRight, boundBottom,
                          true);
        gdiOk = gc.ok;
    }

    ID2D1Factory* factory = gdiOk ? EnsureD2DFactory() : nullptr;
    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1Bitmap* snapshotBmp = nullptr;

    if (factory && gdiOk) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
            D2D1_FEATURE_LEVEL_DEFAULT);
        factory->CreateDCRenderTarget(&rtProps, &rt);
        if (rt) {
            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f);
            rt->CreateBitmap(D2D1::SizeU(static_cast<UINT32>(data->width),
                                         static_cast<UINT32>(data->height)),
                             data->pBits, static_cast<UINT32>(data->width * 4),
                             bmpProps, &snapshotBmp);
        }
    }
    bool d2dOk = (gdiOk && rt && snapshotBmp);

    const double animDur = static_cast<double>(data->durationMs);
    LARGE_INTEGER qpcFreq{};
    LARGE_INTEGER qpcStart{};
    LARGE_INTEGER qpcNow{};
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    double frameIntervalMs = AcquireFrameIntervalMs();
    HANDLE hFrameTimer = CreateFramePacerTimer();

    if (d2dOk) {
        for (;;) {
            QueryPerformanceCounter(&qpcNow);
            const LARGE_INTEGER qpcFrameStart = qpcNow;
            double elapsedMs =
                static_cast<double>(qpcNow.QuadPart - qpcStart.QuadPart) *
                1000.0 / static_cast<double>(qpcFreq.QuadPart);
            BOOL lastFrame = (elapsedMs >= animDur);

            float rawP =
                static_cast<float>(fmin(elapsedMs / animDur, 1.0));
            float easedP =
                rawP < 0.5f
                    ? 4.0f * rawP * rawP * rawP
                    : 1.0f - powf(-2.0f * rawP + 2.0f, 3.0f) * 0.5f;
            float t = data->isRising ? (1.0f - easedP) : easedP;
            float alpha =
                (t > 0.8f) ? std::clamp((1.0f - t) / 0.2f, 0.0f, 1.0f) : 1.0f;

            float l = (wGeom.x + (iGeom.x - wGeom.x) * t) - gc.left;
            float tp = (wGeom.y + (iGeom.y - wGeom.y) * t) - gc.top;
            float r =
                ((wGeom.x + wGeom.width) +
                 ((iGeom.x + iGeom.width) - (wGeom.x + wGeom.width)) * t) -
                gc.left;
            float b =
                ((wGeom.y + wGeom.height) +
                 ((iGeom.y + iGeom.height) - (wGeom.y + wGeom.height)) * t) -
                gc.top;

            bool degenerate =
                (r - l) < 2.0f || (b - tp) < 2.0f || alpha < 0.01f;

            RECT bindRect{0, 0, gc.width, gc.height};
            rt->BindDC(gc.hMemDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));

            if (!degenerate) {
                ID2D1PathGeometry* maskGeo =
                    CreateRoundedRectGeo(factory, l, tp, r, b);
                if (maskGeo) {
                    D2D1_LAYER_PARAMETERS lp = D2D1::LayerParameters();
                    lp.geometricMask = maskGeo;
                    lp.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
                    rt->PushLayer(&lp, nullptr);
                    rt->DrawBitmap(snapshotBmp, D2D1::RectF(l, tp, r, b), alpha,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    rt->PopLayer();
                    maskGeo->Release();
                } else {
                    rt->DrawBitmap(snapshotBmp, D2D1::RectF(l, tp, r, b), alpha,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }
            rt->EndDraw();

            if (!degenerate) {
                POINT ptDst{gc.left, gc.top};
                SIZE sz{gc.width, gc.height};
                POINT ptSrc{0, 0};
                BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                UpdateLayeredWindow(gc.hGhost, gc.hScreenDC, &ptDst, &sz,
                                    gc.hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
                if (firstFrame) {
                    showFn(gc.hGhost, SW_SHOWNOACTIVATE);
                }
            }

            if (lastFrame || g_unloading.load(std::memory_order_relaxed)) {
                break;
            }

            if (firstFrame && !degenerate) {
                DwmFlush();
                firstFrame = FALSE;
                if (data->hFirstFrameShown) {
                    SetEvent(data->hFirstFrameShown);
                }
            }

            PaceAnimationFrame(hFrameTimer, frameIntervalMs, qpcFreq,
                               qpcFrameStart);
        }
    }

    if (hFrameTimer) {
        CloseHandle(hFrameTimer);
    }
    if (!d2dOk) {
        SkipRender(data);
    }

    if (snapshotBmp) {
        snapshotBmp->Release();
    }
    if (rt) {
        rt->Release();
    }
    DestroyGhostCanvas(gc);
    FinishAnimationTeardown(data);
    return 0;
}

DWORD WINAPI AnimThreadClassicGenie(LPVOID lpParam) {
    AnimData* data = static_cast<AnimData*>(lpParam);
    int edge = EffectiveCloseOrDockEdge(data);
    if (edge == EdgeLeft || edge == EdgeRight) {
        return AnimThreadModernGenie(lpParam);
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    auto showFn = ShowWindow_Original ? ShowWindow_Original : ShowWindow;

    const int W = data->width;
    const int H = data->height;
    RECT mon{};
    MONITORINFO mmi{};
    mmi.cbSize = sizeof(mmi);
    if (data->hMon && GetMonitorInfoW(data->hMon, &mmi)) {
        mon = mmi.rcMonitor;
    } else {
        mon.left = 0;
        mon.top = 0;
        mon.right = GetSystemMetrics(SM_CXSCREEN);
        mon.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    const int origLeft = data->targetRect.left;
    const int origTop = data->targetRect.top;
    const float origCenterX = static_cast<float>(origLeft) + W * 0.5f;

    int dockX = data->targetDockX;
    dockX = std::clamp<int>(dockX, mon.left, mon.right);
    float dockY = static_cast<float>(mon.bottom);

    if (data->isClosing) {
        dockX = (mon.left + mon.right) / 2;
        dockY = (edge == EdgeTop) ? static_cast<float>(mon.top)
                                  : static_cast<float>(mon.bottom);
    } else if (data->hasDockTarget) {
        dockX = data->targetDockX;
        dockY = static_cast<float>(data->targetDockY);
    } else if (g_fallbackTarget.load(std::memory_order_relaxed) ==
               static_cast<int>(FallbackTarget::Taskbar)) {
        if (HWND hTray = FindTaskbarForMonitor(data->hMon)) {
            RECT tr{};
            if (GetWindowRect(hTray, &tr) && tr.bottom > tr.top &&
                (tr.top + tr.bottom) < (mon.top + mon.bottom)) {
                dockY = static_cast<float>(mon.top);
            }
        }
    }

    const float dockXf = static_cast<float>(dockX);
    const float origBottomF = static_cast<float>(origTop + H);
    const bool dockAbove =
        dockY < (static_cast<float>(origTop) + origBottomF) * 0.5f;
    if (dockAbove) {
        if (dockY > static_cast<float>(origTop)) {
            dockY = static_cast<float>(origTop);
        }
    } else {
        if (dockY < origBottomF) {
            dockY = origBottomF;
        }
    }

    float neckW = std::clamp(static_cast<float>(W) * 0.03f, 12.0f, 60.0f);

    int boundLeft = (origLeft < dockX ? origLeft : dockX) - W / 2;
    int boundRight = ((origLeft + W) > dockX ? (origLeft + W) : dockX) + W / 2;
    int boundTop = std::min<int>(origTop, static_cast<int>(dockY));
    int boundBottom = std::max<int>(origTop + H, static_cast<int>(dockY));
    RECT iconRect{dockX - 24, static_cast<int>(dockY) - 24, dockX + 24,
                  static_cast<int>(dockY) + 24};
    RECT monUnion = GetMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom,
                     data->targetRect, iconRect, monUnion);
    bool renderOk =
        ClipGenieCanvasToDesktop(boundLeft, boundTop, boundRight, boundBottom);

    GhostCanvas gc;
    float* yb = nullptr;
    if (renderOk) {
        CreateGhostCanvas(gc, boundLeft, boundTop, boundRight, boundBottom,
                          false);
        yb = new (std::nothrow) float[static_cast<size_t>(H) + 1];
        renderOk = gc.ok && yb;
    }

    const BYTE* srcBits = static_cast<const BYTE*>(data->pBits);
    const int srcStride = W * 4;
    const int canvasStride = gc.width * 4;
    const size_t canvasBytes =
        static_cast<size_t>(gc.width) * 4 * static_cast<size_t>(gc.height);
    constexpr float kSpread = 0.65f;
    const double totalMs = static_cast<double>(data->durationMs);

    auto morphAt = [&](float v, float tt) -> float {
        float m = tt * (1.0f + kSpread) - (1.0f - v) * kSpread;
        m = std::clamp(m, 0.0f, 1.0f);
        return m * m * (3.0f - 2.0f * m);
    };

    LARGE_INTEGER qpcFreq{};
    LARGE_INTEGER qpcStart{};
    LARGE_INTEGER qpcNow{};
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    double frameIntervalMs = AcquireFrameIntervalMs();
    HANDLE hFrameTimer = CreateFramePacerTimer();

    BOOL firstFrame = TRUE;
    if (renderOk) {
        BYTE* pBits = static_cast<BYTE*>(gc.pTargetBits);
        for (;;) {
            QueryPerformanceCounter(&qpcNow);
            const LARGE_INTEGER qpcFrameStart = qpcNow;
            double elapsedMs =
                static_cast<double>(qpcNow.QuadPart - qpcStart.QuadPart) *
                1000.0 / static_cast<double>(qpcFreq.QuadPart);
            BOOL lastFrame = (elapsedMs >= totalMs);
            float progress =
                lastFrame ? 1.0f : static_cast<float>(elapsedMs / totalMs);
            float tt = data->isRising ? (1.0f - progress) : progress;

            float fade = 1.0f;
            if (tt > 0.8f) {
                fade = std::clamp((1.0f - tt) / 0.2f, 0.0f, 1.0f);
            }

            memset(pBits, 0, canvasBytes);

            for (int k = 0; k <= H; ++k) {
                float v = static_cast<float>(k) / static_cast<float>(H);
                float e = morphAt(dockAbove ? (1.0f - v) : v, tt);
                float idY =
                    static_cast<float>(origTop) + static_cast<float>(H) * v;
                yb[k] = idY + (dockY - idY) * e;
            }

            int kSeg = 0;
            for (int yC = 0; yC < gc.height; ++yC) {
                float screenY = static_cast<float>(yC + gc.top) + 0.5f;
                if (screenY < yb[0] || screenY >= yb[H]) {
                    continue;
                }
                while (kSeg < H - 1 && yb[kSeg + 1] <= screenY) {
                    ++kSeg;
                }
                float segH = yb[kSeg + 1] - yb[kSeg];
                float frac =
                    segH > 1e-4f ? (screenY - yb[kSeg]) / segH : 0.0f;
                float v =
                    (static_cast<float>(kSeg) + frac) / static_cast<float>(H);

                float em = morphAt(dockAbove ? (1.0f - v) : v, tt);
                float width =
                    static_cast<float>(W) + (neckW - static_cast<float>(W)) * em;
                if (width < 1.0f) {
                    width = 1.0f;
                }
                float cx = origCenterX + (dockXf - origCenterX) * em;
                float leftCanvas = (cx - width * 0.5f) - static_cast<float>(gc.left);

                int srcRow =
                    std::clamp(static_cast<int>(v * static_cast<float>(H)), 0,
                               H - 1);
                const BYTE* srcRowPtr =
                    srcBits + static_cast<size_t>(srcRow) * srcStride;
                BYTE* dstRowPtr =
                    pBits + static_cast<size_t>(yC) * canvasStride;

                int xStart = std::max(0, static_cast<int>(leftCanvas));
                int xEnd =
                    std::min(gc.width, static_cast<int>(leftCanvas + width) + 1);
                float invW = 1.0f / width;
                for (int xC = xStart; xC < xEnd; ++xC) {
                    float u = (static_cast<float>(xC) + 0.5f - leftCanvas) * invW;
                    if (u < 0.0f || u >= 1.0f) {
                        continue;
                    }
                    int srcX =
                        std::clamp(static_cast<int>(u * static_cast<float>(W)),
                                   0, W - 1);
                    const BYTE* sp = srcRowPtr + static_cast<size_t>(srcX) * 4;
                    BYTE* dp = dstRowPtr + static_cast<size_t>(xC) * 4;
                    dp[0] = sp[0];
                    dp[1] = sp[1];
                    dp[2] = sp[2];
                    dp[3] = sp[3];
                }
            }

            POINT ptDst{gc.left, gc.top};
            SIZE sz{gc.width, gc.height};
            POINT ptSrc{0, 0};
            BLENDFUNCTION bf{};
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = static_cast<BYTE>(255.0f * fade);
            bf.AlphaFormat = AC_SRC_ALPHA;
            UpdateLayeredWindow(gc.hGhost, gc.hScreenDC, &ptDst, &sz, gc.hMemDC,
                                &ptSrc, 0, &bf, ULW_ALPHA);

            if (firstFrame) {
                showFn(gc.hGhost, SW_SHOWNOACTIVATE);
            }

            if (lastFrame || g_unloading.load(std::memory_order_relaxed)) {
                break;
            }

            if (firstFrame) {
                DwmFlush();
                firstFrame = FALSE;
                if (data->hFirstFrameShown) {
                    SetEvent(data->hFirstFrameShown);
                }
            }

            PaceAnimationFrame(hFrameTimer, frameIntervalMs, qpcFreq,
                               qpcFrameStart);
        }
    }

    if (hFrameTimer) {
        CloseHandle(hFrameTimer);
    }
    if (!renderOk) {
        SkipRender(data);
    }

    delete[] yb;
    DestroyGhostCanvas(gc);
    FinishAnimationTeardown(data);
    return 0;
}

bool StartMinimizeAnim(HWND hWnd,
                       BOOL rising,
                       LONG_PTR originalExStyle,
                       BOOL cloakHidden = FALSE,
                       BOOL deferredMinimize = FALSE,
                       BOOL requestedUnhide = FALSE,
                       HWND hNextApp = nullptr,
                       BOOL isClosing = FALSE,
                       UINT closeMsg = WM_DESTROY,
                       uint32_t precomputedDockPacked = 0,
                       bool skipDockQuery = false) {
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        for (auto it = g_SnapshotCache.begin(); it != g_SnapshotCache.end();) {
            if (!IsWindow(it->first)) {
                if (it->second.hBmp) {
                    DeleteObject(it->second.hBmp);
                }
                g_DockTargetCache.erase(it->first);
                g_IconPositions.erase(it->first);
                it = g_SnapshotCache.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (IsOwnOverlayClass(hWnd) || IsExcludedProcess(hWnd)) {
        if (rising) {
            UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            SetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }

    RECT winRect{};
    if (!GetWindowRect(hWnd, &winRect)) {
        if (rising) {
            UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            SetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }
    RECT rect = winRect;
    RECT extRect{};
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &extRect, sizeof(extRect)))) {
        rect = extRect;
    }
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int offsetX = rect.left - winRect.left;
    int offsetY = rect.top - winRect.top;
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;

    if (w <= 0 || h <= 0 || rawW <= 0 || rawH <= 0 ||
        static_cast<long long>(w) * h > kMaxCanvasPixels ||
        static_cast<long long>(rawW) * rawH > kMaxCanvasPixels) {
        if (rising) {
            UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            SetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }

    bool blocked = g_unloading.load(std::memory_order_relaxed);
    if (!blocked) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        blocked = !g_AnimActive.insert(hWnd).second;
        if (!blocked) {
            g_LaunchSeen.insert(hWnd);
        }
    }
    if (blocked) {
        if (rising) {
            UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            if (!IsAnimActive(hWnd)) {
                SetDwmTransitions(hWnd, TRUE);
            }
        }
        return false;
    }

    bool multiMon = isClosing || g_multiMonitor.load(std::memory_order_relaxed);
    HMONITOR hMon =
        multiMon ? MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)
                 : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0;
        mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;

    int learnedTargetX = mi.rcMonitor.left + monWidth / 2;
    int learnedTargetY = mi.rcMonitor.bottom - 36;
    BOOL hasDockTarget = FALSE;
    int dockEdge = EdgeBottom;

    if (!isClosing) {
        uint32_t dockPacked = precomputedDockPacked;
        if (dockPacked == 0 && !skipDockQuery) {
            dockPacked = QueryDockIconPacked(hWnd);
        }

        HWND dock = FindWindowW(kDockClassName, nullptr);
        if (dockPacked != 0) {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            g_DockTargetCache[hWnd] = dockPacked;
        } else if (dock) {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (auto it = g_DockTargetCache.find(hWnd);
                it != g_DockTargetCache.end()) {
                dockPacked = it->second;
            }
        }

        if (dockPacked != 0) {
            int cx = static_cast<int>(dockPacked & 0xFFFFu) - 16384;
            int cy = static_cast<int>((dockPacked >> 16) & 0xFFFFu) - 16384;
            learnedTargetX = cx;
            learnedTargetY = cy;
            hasDockTarget = TRUE;
            hMon = MonitorFromPoint(POINT{cx, cy}, MONITOR_DEFAULTTONEAREST);
            MONITORINFO dmi{};
            dmi.cbSize = sizeof(dmi);
            if (GetMonitorInfoW(hMon, &dmi)) {
                mi = dmi;
            }
        } else if (dock && IsWindowVisible(dock)) {
            RECT dockRc{};
            if (GetWindowRect(dock, &dockRc)) {
                HMONITOR dockMon =
                    MonitorFromRect(&dockRc, MONITOR_DEFAULTTONEAREST);
                if (dockMon == hMon || !multiMon) {
                    int dw = dockRc.right - dockRc.left;
                    int dh = dockRc.bottom - dockRc.top;
                    if (dw > 16 && dh > 16) {
                        if (dw >= dh &&
                            g_fallbackTarget.load(std::memory_order_relaxed) ==
                                static_cast<int>(FallbackTarget::BottomRight)) {
                            learnedTargetX =
                                dockRc.right - std::min(96, dw / 4);
                        } else {
                            learnedTargetX = (dockRc.left + dockRc.right) / 2;
                        }
                        learnedTargetY = (dockRc.top + dockRc.bottom) / 2;
                        hasDockTarget = TRUE;
                        hMon = dockMon;
                        MONITORINFO dmi{};
                        dmi.cbSize = sizeof(dmi);
                        if (GetMonitorInfoW(hMon, &dmi)) {
                            mi = dmi;
                        }
                    }
                }
            }
        }

        if (hasDockTarget) {
            if (learnedTargetX - mi.rcMonitor.left < 140 &&
                mi.rcMonitor.bottom - learnedTargetY > 140) {
                dockEdge = EdgeLeft;
            } else if (mi.rcMonitor.right - learnedTargetX < 140 &&
                       mi.rcMonitor.bottom - learnedTargetY > 140) {
                dockEdge = EdgeRight;
            } else if (learnedTargetY < (rect.top + rect.bottom) / 2) {
                dockEdge = EdgeTop;
            } else {
                dockEdge = EdgeBottom;
            }
        } else {
            const auto fb = static_cast<FallbackTarget>(
                g_fallbackTarget.load(std::memory_order_relaxed));
            if (fb == FallbackTarget::BottomRight) {
                learnedTargetX = mi.rcMonitor.right - 96;
                learnedTargetY = mi.rcMonitor.bottom - 36;
            } else if (fb == FallbackTarget::BottomCenter) {
                learnedTargetX = mi.rcMonitor.left + monWidth / 2;
                learnedTargetY = mi.rcMonitor.bottom - 36;
            } else {
                DWORD alignVal = 1;
                DWORD dataSize = sizeof(alignVal);
                RegGetValueW(
                    HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                    L"Advanced",
                    L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &alignVal,
                    &dataSize);
                learnedTargetX = (alignVal == 0)
                                     ? (mi.rcMonitor.left + 160)
                                     : (mi.rcMonitor.left + monWidth / 2);

                POINT pt{};
                GetCursorPos(&pt);
                RECT workArea{};
                MONITORINFO cursorMi{};
                cursorMi.cbSize = sizeof(cursorMi);
                if (GetMonitorInfoW(
                        MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                        &cursorMi)) {
                    workArea = cursorMi.rcWork;
                } else {
                    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
                }

                if (!PtInRect(&workArea, pt)) {
                    learnedTargetX = pt.x;
                    std::lock_guard<std::mutex> lock(g_CacheMutex);
                    g_IconPositions[hWnd] = learnedTargetX;
                } else {
                    learnedTargetX =
                        GetTaskbarButtonX(hWnd, learnedTargetX, hMon);
                }

                if (HWND hTray = FindTaskbarForMonitor(hMon)) {
                    RECT tr{};
                    if (GetWindowRect(hTray, &tr) && tr.bottom > tr.top &&
                        (tr.top + tr.bottom) <
                            (mi.rcMonitor.top + mi.rcMonitor.bottom)) {
                        dockEdge = EdgeTop;
                    }
                }
            }
        }
    }

    AnimData* data = new AnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->hMon = hMon;
    data->width = w;
    data->height = h;
    data->targetDockX = learnedTargetX;
    data->targetDockY = learnedTargetY;
    data->hasDockTarget = hasDockTarget;
    data->dockEdge = dockEdge;
    data->effect = static_cast<AnimEffect>(
        g_effect.load(std::memory_order_relaxed));
    data->isRising = rising;
    data->originalExStyle = originalExStyle;
    data->hiddenByCloak = cloakHidden;
    data->hFirstFrameShown = nullptr;
    data->durationMs = g_durationMs.load(std::memory_order_relaxed);
    data->requestedUnhide = requestedUnhide;
    data->hNextApp = hNextApp;
    data->unhideDurationMs =
        g_unhideDurationMs.load(std::memory_order_relaxed);
    data->deferredMinimize = deferredMinimize;
    data->isClosing = isClosing;
    data->closeMsg = closeMsg;
    data->pBits = nullptr;
    data->hBitmap = nullptr;

    HDC hScreenDC = GetDC(nullptr);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    data->hBitmap = hScreenDC
                        ? CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS,
                                           &data->pBits, nullptr, 0)
                        : nullptr;

    auto bailOut = [&]() {
        if (hScreenDC) {
            ReleaseDC(nullptr, hScreenDC);
        }
        if (rising) {
            UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            SetDwmTransitions(hWnd, TRUE);
        }
        if (data->hBitmap) {
            DeleteObject(data->hBitmap);
        }
        delete data;
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
    };

    if (!data->hBitmap || !data->pBits) {
        bailOut();
        return false;
    }

    auto copySnapshot = [&](void* srcBits) {
        DWORD* src = static_cast<DWORD*>(srcBits);
        DWORD* dst = static_cast<DWORD*>(data->pBits);
        int inBounds = 0;
        int zeroAlpha = 0;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int sx = x + offsetX;
                int sy = y + offsetY;
                if (sx >= 0 && sx < rawW && sy >= 0 && sy < rawH) {
                    DWORD p = src[static_cast<size_t>(sy) * rawW + sx];
                    BYTE a = static_cast<BYTE>((p >> 24) & 0xFF);
                    ++inBounds;
                    if (a == 0) {
                        ++zeroAlpha;
                    }
                    dst[static_cast<size_t>(y) * w + x] = (a == 0) ? 0 : p;
                } else {
                    dst[static_cast<size_t>(y) * w + x] = 0;
                }
            }
        }
        if (inBounds > 0 && zeroAlpha * 2 > inBounds) {
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    int sx = x + offsetX;
                    int sy = y + offsetY;
                    if (sx >= 0 && sx < rawW && sy >= 0 && sy < rawH) {
                        DWORD p = src[static_cast<size_t>(sy) * rawW + sx];
                        if (((p >> 24) & 0xFF) == 0) {
                            dst[static_cast<size_t>(y) * w + x] =
                                0xFF000000u | (p & 0x00FFFFFFu);
                        }
                    }
                }
            }
        }
    };

    auto captureNow = [&]() -> bool {
        HDC hTempDC = CreateCompatibleDC(hScreenDC);
        if (!hTempDC) {
            return false;
        }
        BITMAPINFO bmiTemp = bmi;
        bmiTemp.bmiHeader.biWidth = rawW;
        bmiTemp.bmiHeader.biHeight = -rawH;
        void* pTempBits = nullptr;
        HBITMAP hTempBmp = CreateDIBSection(hScreenDC, &bmiTemp, DIB_RGB_COLORS,
                                            &pTempBits, nullptr, 0);
        if (!hTempBmp || !pTempBits) {
            if (hTempBmp) {
                DeleteObject(hTempBmp);
            }
            DeleteDC(hTempDC);
            return false;
        }
        HBITMAP hOldTempBmp =
            static_cast<HBITMAP>(SelectObject(hTempDC, hTempBmp));
        if (!hOldTempBmp) {
            DeleteObject(hTempBmp);
            DeleteDC(hTempDC);
            return false;
        }
        BOOL screenGrab =
            isClosing && GetForegroundWindow() == hWnd &&
            BitBlt(hTempDC, 0, 0, rawW, rawH, hScreenDC, winRect.left,
                   winRect.top, SRCCOPY);
        if (!screenGrab) {
            PrintWindow(hWnd, hTempDC, PW_RENDERFULLCONTENT);
        }
        GdiFlush();
        copySnapshot(pTempBits);
        SelectObject(hTempDC, hOldTempBmp);
        DeleteObject(hTempBmp);
        DeleteDC(hTempDC);
        return true;
    };

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            auto it = g_SnapshotCache.find(hWnd);
            if (it != g_SnapshotCache.end()) {
                SnapCache& c = it->second;
                if (c.w == w && c.h == h && c.pBits) {
                    memcpy(data->pBits, c.pBits,
                           static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
                    fromCache = TRUE;
                }
                if (c.hBmp) {
                    DeleteObject(c.hBmp);
                }
                g_SnapshotCache.erase(it);
            }
        }
        if (!fromCache && !captureNow()) {
            bailOut();
            return false;
        }
    } else {
        if (!captureNow()) {
            bailOut();
            return false;
        }
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (auto it = g_SnapshotCache.find(hWnd); it != g_SnapshotCache.end()) {
            if (it->second.hBmp) {
                DeleteObject(it->second.hBmp);
            }
            g_SnapshotCache.erase(it);
        }
        void* pCacheBits = nullptr;
        HBITMAP hCacheBmp =
            (isClosing || g_unloading.load(std::memory_order_relaxed))
                ? nullptr
                : CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCacheBits,
                                   nullptr, 0);
        if (hCacheBmp && pCacheBits) {
            memcpy(pCacheBits, data->pBits,
                   static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
            g_SnapshotCache[hWnd] = {hCacheBmp, pCacheBits, w, h,
                                     GetTickCount64()};
            while (g_SnapshotCache.size() > kMaxCachedSnapshots) {
                auto oldest = g_SnapshotCache.begin();
                for (auto it = g_SnapshotCache.begin();
                     it != g_SnapshotCache.end(); ++it) {
                    if (it->second.tick < oldest->second.tick) {
                        oldest = it;
                    }
                }
                if (oldest->second.hBmp) {
                    DeleteObject(oldest->second.hBmp);
                }
                g_SnapshotCache.erase(oldest);
            }
        } else if (hCacheBmp) {
            DeleteObject(hCacheBmp);
        }
    }

    ReleaseDC(nullptr, hScreenDC);

    HANDLE hFirstShown = nullptr;
    if (!rising) {
        hFirstShown = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (hFirstShown &&
            !DuplicateHandle(GetCurrentProcess(), hFirstShown,
                             GetCurrentProcess(), &data->hFirstFrameShown, 0,
                             FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hFirstFrameShown = nullptr;
        }
    }
    bool waitForFirstFrame = (data->hFirstFrameShown != nullptr);

    LPTHREAD_START_ROUTINE renderProc = AnimThreadModernGenie;
    if (data->effect == AnimEffect::Scale) {
        renderProc = AnimThreadScale;
    } else if (data->effect == AnimEffect::ClassicGenie) {
        renderProc = AnimThreadClassicGenie;
    }

    if (SpawnWorker(renderProc, data)) {
        if (hFirstShown) {
            if (waitForFirstFrame) {
                WaitForSingleObject(hFirstShown, 200);
            }
            CloseHandle(hFirstShown);
        }
        return true;
    }

    if (rising) {
        UndoRisingHide(hWnd, data->originalExStyle, data->hiddenByCloak);
    } else {
        SetDwmTransitions(hWnd, TRUE);
    }
    if (hFirstShown) {
        CloseHandle(hFirstShown);
    }
    if (data->hFirstFrameShown) {
        CloseHandle(data->hFirstFrameShown);
    }
    DeleteObject(data->hBitmap);
    delete data;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
    }
    return false;
}

DWORD WINAPI LaunchThread(LPVOID lpParam);

struct GeckoSearch {
    HWND self;
    DWORD pid;
    bool found;
};

BOOL CALLBACK FindOtherGeckoWindow(HWND w, LPARAM lp) {
    GeckoSearch* s = reinterpret_cast<GeckoSearch*>(lp);
    if (w == s->self || (!IsWindowVisible(w) && !IsIconic(w)) ||
        GetAncestor(w, GA_ROOT) != w) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    WCHAR cls[64]{};
    if (pid == s->pid && GetClassNameW(w, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"MozillaWindowClass") == 0) {
        s->found = true;
        return FALSE;
    }
    return TRUE;
}

bool IsFirstGeckoWindow(HWND hWnd) {
    WCHAR cls[64]{};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"MozillaWindowClass") != 0) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    GeckoSearch search{hWnd, pid, false};
    EnumWindows(FindOtherGeckoWindow, reinterpret_cast<LPARAM>(&search));
    return !search.found;
}

bool LaunchPrepare(HWND hWnd,
                   int nCmdShow,
                   LONG_PTR* origExOut,
                   BOOL* cloakedOut) {
    if (g_unloading.load(std::memory_order_relaxed) ||
        !g_launchAnimation.load(std::memory_order_relaxed)) {
        return false;
    }
    if (nCmdShow != SW_SHOW && nCmdShow != SW_SHOWNORMAL &&
        nCmdShow != SW_SHOWDEFAULT && nCmdShow != SW_SHOWMAXIMIZED) {
        return false;
    }
    if (IsWindowVisible(hWnd) || IsIconic(hWnd) || !IsLaunchWindow(hWnd)) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (!g_LaunchSeen.insert(hWnd).second) {
            return false;
        }
    }
    if (IsExcludedProcess(hWnd)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    *origExOut = exStyle;
    SetDwmTransitions(hWnd, FALSE);

    UINT cloaked = 0;
    bool appCloaked =
        SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        (cloaked & DWM_CLOAKED_APP);
    if (!appCloaked && !IsFirstGeckoWindow(hWnd) && SetCloak(hWnd, TRUE)) {
        *cloakedOut = TRUE;
        return true;
    }
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    *cloakedOut = FALSE;
    return true;
}

void LaunchCommit(HWND hWnd, LONG_PTR originalExStyle, BOOL cloaked) {
    LaunchData* ld = new LaunchData();
    ld->hWnd = hWnd;
    ld->originalExStyle = originalExStyle;
    ld->hiddenByCloak = cloaked;
    if (!SpawnWorker(LaunchThread, ld)) {
        delete ld;
        UndoRisingHide(hWnd, originalExStyle, cloaked);
    }
}

void ClearCloseHandled(HWND hWnd) {
    if (GetPropW(hWnd, L"GenieCloseHandled")) {
        RemovePropW(hWnd, L"GenieCloseHandled");
    }
}

bool MarkCloseAndCloak(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    SetPropW(hWnd, L"GenieCloseHandled", reinterpret_cast<HANDLE>(1));
    SetCloak(hWnd, TRUE);
    return true;
}

void WaitCloseAnim(HWND hWnd, DWORD timeoutMs) {
    const DWORD start = GetTickCount();
    while (IsWindow(hWnd) && !g_unloading.load(std::memory_order_relaxed)) {
        if (!IsAnimActive(hWnd)) {
            break;
        }
        const DWORD elapsed = GetTickCount() - start;
        if (elapsed >= timeoutMs) {
            break;
        }
        DWORD slice = std::min<DWORD>(10, timeoutMs - elapsed);
        if (MsgWaitForMultipleObjectsEx(0, nullptr, slice, QS_SENDMESSAGE,
                                        MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0) {
            MSG msg{};
            PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
        }
    }
}

struct OtherWindowSearch {
    HWND self;
    DWORD pid;
    bool found;
};

BOOL CALLBACK FindOtherAppWindow(HWND w, LPARAM lp) {
    OtherWindowSearch* s = reinterpret_cast<OtherWindowSearch*>(lp);
    if (w == s->self || !IsWindowVisible(w) || GetAncestor(w, GA_ROOT) != w ||
        GetWindow(w, GW_OWNER) ||
        (GetWindowLongPtrW(w, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid == s->pid && !IsWindowCloaked(w)) {
        s->found = true;
        return FALSE;
    }
    return TRUE;
}

bool HasOtherAppWindow(HWND hWnd) {
    OtherWindowSearch search{hWnd, GetCurrentProcessId(), false};
    EnumWindows(FindOtherAppWindow, reinterpret_cast<LPARAM>(&search));
    return search.found;
}

bool StartClose(HWND hWnd, UINT closeMsg) {
    SetDwmTransitions(hWnd, FALSE);
    SetPropW(hWnd, L"GenieCloseAnimating", reinterpret_cast<HANDLE>(1));
    if (StartMinimizeAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                          FALSE, FALSE, FALSE, nullptr, TRUE, closeMsg)) {
        if (MarkCloseAndCloak(hWnd)) {
            return true;
        }
        RemovePropW(hWnd, L"GenieCloseAnimating");
        return false;
    }
    RemovePropW(hWnd, L"GenieCloseAnimating");
    SetDwmTransitions(hWnd, TRUE);
    return false;
}

bool IsPopupClass(const WCHAR* cls) {
    static constexpr const WCHAR* kClasses[] = {
        L"Xaml_WindowedPopupClass",
        L"Popup",
        L"Overlay",
        L"ToolTip",
        L"CoreWindow",
        L"ApplicationFrameWindow",
        L"XamlExplorerHostIslandWindow",
        L"Windows.Internal.Shell.TabProxyWindow",
    };
    for (const WCHAR* c : kClasses) {
        if (_wcsicmp(cls, c) == 0) {
            return true;
        }
    }
    return false;
}

bool IsBlockedShellWindow(const WCHAR* cls) {
    return IsExplorerProcess() && _wcsicmp(cls, L"CabinetWClass") != 0 &&
           _wcsicmp(cls, L"ExploreWClass") != 0;
}

bool IsCloseTarget(HWND hWnd) {
    if (!g_closeAnimation.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!hWnd || !IsWindowVisible(hWnd) || IsIconic(hWnd) ||
        GetAncestor(hWnd, GA_ROOT) != hWnd ||
        GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId()) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) ||
        GetWindow(hWnd, GW_OWNER) || IsOwnOverlayClass(hWnd)) {
        return false;
    }
    WCHAR cls[128]{};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || IsPopupClass(cls) ||
        IsBlockedShellWindow(cls)) {
        return false;
    }
    if (IsWindowCloaked(hWnd)) {
        return false;
    }
    RECT r{};
    return GetWindowRect(hWnd, &r) && (r.right - r.left) >= 300 &&
           (r.bottom - r.top) >= 300;
}

bool IsCloseMessageClass(HWND hWnd) {
    WCHAR cls[128]{};
    if (!hWnd || !GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return _wcsicmp(cls, L"ConsoleWindowClass") == 0 ||
           _wcsicmp(cls, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0 ||
           _wcsicmp(cls, L"TaskManagerWindow") == 0 ||
           _wcsicmp(cls, L"WinUIDesktopWin32WindowClass") == 0;
}

bool ShouldTreatHideAsClose(HWND hWnd) {
    if (!g_closeAnimation.load(std::memory_order_relaxed) ||
        !g_hideAsClose.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd) || IsIconic(hWnd) ||
        GetAncestor(hWnd, GA_ROOT) != hWnd || IsOwnOverlayClass(hWnd)) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) ||
        GetWindow(hWnd, GW_OWNER) || IsCloseMessageClass(hWnd)) {
        return false;
    }
    WCHAR cls[128]{};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || IsPopupClass(cls) ||
        IsBlockedShellWindow(cls)) {
        return false;
    }
    if (IsWindowCloaked(hWnd)) {
        return false;
    }
    RECT r{};
    return GetWindowRect(hWnd, &r) && (r.right - r.left) >= 300 &&
           (r.bottom - r.top) >= 300;
}

bool CloseBlocked(HWND hWnd) {
    return GetPropW(hWnd, L"GenieCloseBypass") ||
           GetPropW(hWnd, L"GenieCloseHandled") ||
           GetPropW(hWnd, L"GenieCloseAnimating") ||
           g_unloading.load(std::memory_order_relaxed);
}

bool AnimateBeforeNativeHide(HWND hWnd) {
    if (!ShouldTreatHideAsClose(hWnd) || CloseBlocked(hWnd)) {
        return false;
    }
    if (!StartClose(hWnd, WM_DESTROY)) {
        return false;
    }
    WaitCloseAnim(hWnd, 3500);
    return true;
}

void FinishNativeHide(HWND hWnd, bool animated) {
    if (!animated || !IsWindow(hWnd)) {
        return;
    }
    SetCloak(hWnd, FALSE);
    RemovePropW(hWnd, L"GenieCloseAnimating");
    SetDwmTransitions(hWnd, TRUE);
}

bool IsMinimizeCmd(int nCmdShow) {
    return nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED ||
           nCmdShow == SW_SHOWMINNOACTIVE;
}

bool IsRestoreCmd(int nCmdShow) {
    return nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL ||
           nCmdShow == SW_SHOWMAXIMIZED;
}

BOOL TryCrossProcessRemoteAnim(HWND hWnd, WORD cmd, int nCmdShow) {
    if (!g_msgRemoteAnim) {
        return FALSE;
    }
    const uint32_t packed = QueryDockIconPacked(hWnd);
    DWORD_PTR handled = 0;
    if (SendMessageTimeoutW(
            hWnd, g_msgRemoteAnim,
            MAKEWPARAM(cmd, static_cast<WORD>(static_cast<short>(nCmdShow))),
            static_cast<LPARAM>(static_cast<uintptr_t>(packed)),
            SMTO_ABORTIFHUNG | SMTO_NORMAL, 250, &handled) &&
        handled == 1) {
        return TRUE;
    }
    return FALSE;
}

void TryMinimizeAnim(HWND hWnd,
                     uint32_t precomputedDockPacked,
                     bool skipDockQuery) {
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) {
        return;
    }
    if (!ShouldAnimateWindow(hWnd)) {
        return;
    }
    SetDwmTransitions(hWnd, FALSE);
    StartMinimizeAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE,
                      FALSE, FALSE, nullptr, FALSE, WM_DESTROY,
                      precomputedDockPacked, skipDockQuery);
}

BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    bool animated = false;
    if (IsCloseTarget(hWnd) && !CloseBlocked(hWnd)) {
        animated = StartClose(hWnd, WM_DESTROY);
        if (animated && !HasOtherAppWindow(hWnd)) {
            WaitCloseAnim(hWnd, 4000);
        }
    }
    BOOL result = DestroyWindow_Original(hWnd);
    if (!result && animated && IsWindow(hWnd)) {
        RemovePropW(hWnd, L"GenieCloseHandled");
        RemovePropW(hWnd, L"GenieCloseAnimating");
        SetCloak(hWnd, FALSE);
        SetDwmTransitions(hWnd, TRUE);
    }
    return result;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        bool animated = AnimateBeforeNativeHide(hWnd);
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        FinishNativeHide(hWnd, animated);
        return res;
    }
    ClearCloseHandled(hWnd);

    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWnd, &ownerPid);
    const bool sameProcess = (ownerPid == GetCurrentProcessId());

    if (IsMinimizeCmd(nCmdShow)) {
        if (ShouldAnimateWindow(hWnd)) {
            if (!sameProcess) {
                if (TryCrossProcessRemoteAnim(hWnd, kRemoteCmdMinimize,
                                              nCmdShow)) {
                    return TRUE;
                }
                const uint32_t packed = QueryDockIconPacked(hWnd);
                SetDwmTransitions(hWnd, FALSE);
                StartMinimizeAnim(hWnd, FALSE,
                                  GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE,
                                  FALSE, FALSE, nullptr, FALSE, WM_DESTROY,
                                  packed, true);
                return ShowWindow_Original(hWnd, nCmdShow);
            }
            SetDwmTransitions(hWnd, FALSE);
            StartMinimizeAnim(hWnd, FALSE,
                              GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (IsRestoreCmd(nCmdShow) && IsIconic(hWnd)) {
        if (g_openAnimation.load(std::memory_order_relaxed) &&
            ShouldAnimateWindow(hWnd)) {
            if (!sameProcess) {
                if (TryCrossProcessRemoteAnim(hWnd, kRemoteCmdRestore,
                                              nCmdShow)) {
                    return TRUE;
                }
                const uint32_t packed = QueryDockIconPacked(hWnd);
                LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                BOOL res = ShowWindow_Original(hWnd, nCmdShow);
                StartMinimizeAnim(hWnd, TRUE, origEx, FALSE, FALSE, FALSE,
                                  nullptr, FALSE, WM_DESTROY, packed, true);
                return res;
            }
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            BOOL cloaked = SetCloak(hWnd, TRUE) ? TRUE : FALSE;
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            StartMinimizeAnim(hWnd, TRUE, origEx, cloaked);
            return res;
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    LONG_PTR launchOrigEx = 0;
    BOOL launchCloaked = FALSE;
    if (LaunchPrepare(hWnd, nCmdShow, &launchOrigEx, &launchCloaked)) {
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        LaunchCommit(hWnd, launchOrigEx, launchCloaked);
        return res;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

template <typename OrigFn>
LRESULT HandleDefWindowProc(HWND hWnd,
                            UINT Msg,
                            WPARAM wParam,
                            LPARAM lParam,
                            OrigFn origFn) {
    if (g_msgRemoteAnim && Msg == g_msgRemoteAnim) {
        const WORD cmd = LOWORD(wParam);
        const int nCmdShow =
            static_cast<int>(static_cast<short>(HIWORD(wParam)));
        const uint32_t packed =
            static_cast<uint32_t>(static_cast<uintptr_t>(lParam));
        auto showFn = ShowWindow_Original ? ShowWindow_Original : ShowWindow;

        if (cmd == kRemoteCmdMinimize) {
            if (ShouldAnimateWindow(hWnd) && !IsIconic(hWnd)) {
                SetDwmTransitions(hWnd, FALSE);
                StartMinimizeAnim(hWnd, FALSE,
                                  GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE,
                                  FALSE, FALSE, nullptr, FALSE, WM_DESTROY,
                                  packed, true);
            }
            showFn(hWnd, nCmdShow);
            return 1;
        }
        if (cmd == kRemoteCmdRestore) {
            if (IsIconic(hWnd) &&
                g_openAnimation.load(std::memory_order_relaxed) &&
                ShouldAnimateWindow(hWnd)) {
                SetDwmTransitions(hWnd, FALSE);
                LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                BOOL cloaked = SetCloak(hWnd, TRUE) ? TRUE : FALSE;
                showFn(hWnd, nCmdShow);
                StartMinimizeAnim(hWnd, TRUE, origEx, cloaked, FALSE, FALSE,
                                  nullptr, FALSE, WM_DESTROY, packed, true);
                return 1;
            }
            return 0;
        }
    }

    if (Msg == WM_SHOWWINDOW && wParam && !IsIconic(hWnd)) {
        ClearCloseHandled(hWnd);
    }

    bool closeMessage =
        Msg == WM_CLOSE ||
        (Msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE);
    if (closeMessage && GetPropW(hWnd, L"GenieCloseBypass")) {
        if (Msg == WM_CLOSE) {
            RemovePropW(hWnd, L"GenieCloseBypass");
        }
        return origFn(hWnd, Msg, wParam, lParam);
    }
    if (closeMessage && IsWindowVisible(hWnd) &&
        GetPropW(hWnd, L"GenieCloseAnimating")) {
        return 0;
    }
    if (closeMessage && g_closeAnimation.load(std::memory_order_relaxed) &&
        IsCloseMessageClass(hWnd) && IsWindowVisible(hWnd) &&
        !IsIconic(hWnd) && GetAncestor(hWnd, GA_ROOT) == hWnd &&
        !IsWindowCloaked(hWnd) && !CloseBlocked(hWnd)) {
        if (StartClose(hWnd, Msg)) {
            return 0;
        }
    }

    if (Msg == WM_DESTROY) {
        RemovePropW(hWnd, L"GenieCloseHandled");
        RemovePropW(hWnd, L"GenieCloseAnimating");
        RemovePropW(hWnd, L"GenieCloseBypass");
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (auto it = g_SnapshotCache.find(hWnd); it != g_SnapshotCache.end()) {
            if (it->second.hBmp) {
                DeleteObject(it->second.hBmp);
            }
            g_SnapshotCache.erase(it);
        }
        g_IconPositions.erase(hWnd);
        g_DockTargetCache.erase(hWnd);
        g_LaunchSeen.erase(hWnd);
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = static_cast<UINT>(wParam) & 0xFFF0u;
        if (cmd == SC_MINIMIZE) {
            if (GetPropW(hWnd, L"GenieBypass")) {
                return origFn(hWnd, Msg, wParam, lParam);
            }
            if (ShouldAnimateWindow(hWnd)) {
                BOOL requestedUnhide = FALSE;
                HWND hNext = nullptr;
                const bool dockActive =
                    FindWindowW(kDockClassName, nullptr) != nullptr;
                if (!dockActive &&
                    g_unhideEnabled.load(std::memory_order_relaxed)) {
                    HWND hTray = FindTaskbarForMonitor(
                        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
                    if (hTray) {
                        APPBARDATA abd{sizeof(APPBARDATA)};
                        abd.hWnd = hTray;
                        UINT uState = static_cast<UINT>(
                            SHAppBarMessage(ABM_GETSTATE, &abd));
                        if (uState & ABS_AUTOHIDE) {
                            requestedUnhide = TRUE;
                            HWND hwndIter = GetWindow(hWnd, GW_HWNDNEXT);
                            while (hwndIter) {
                                if (IsWindowVisible(hwndIter) &&
                                    !IsIconic(hwndIter) &&
                                    GetAncestor(hwndIter, GA_ROOT) ==
                                        hwndIter &&
                                    GetWindowTextLengthW(hwndIter) > 0 &&
                                    hwndIter != hWnd && hwndIter != hTray) {
                                    LONG_PTR exStyle =
                                        GetWindowLongPtrW(hwndIter, GWL_EXSTYLE);
                                    if (!(exStyle & WS_EX_TOOLWINDOW)) {
                                        hNext = hwndIter;
                                        break;
                                    }
                                }
                                hwndIter = GetWindow(hwndIter, GW_HWNDNEXT);
                            }
                            SetForegroundWindow(hTray);
                        }
                    }
                }

                SetDwmTransitions(hWnd, FALSE);
                if (requestedUnhide) {
                    bool ok = StartMinimizeAnim(
                        hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                        FALSE, TRUE, TRUE, hNext);
                    if (ok) {
                        SetCloak(hWnd, TRUE);
                        return 0;
                    }
                    return origFn(hWnd, Msg, wParam, lParam);
                }
                StartMinimizeAnim(hWnd, FALSE,
                                  GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
            }
            return origFn(hWnd, Msg, wParam, lParam);
        }
        if (cmd == SC_RESTORE && IsIconic(hWnd) &&
            g_openAnimation.load(std::memory_order_relaxed) &&
            ShouldAnimateWindow(hWnd)) {
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            BOOL cloaked = SetCloak(hWnd, TRUE) ? TRUE : FALSE;
            LRESULT res = origFn(hWnd, Msg, wParam, lParam);
            StartMinimizeAnim(hWnd, TRUE, origEx, cloaked);
            return res;
        }
    }

    return origFn(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    return HandleDefWindowProc(hWnd, Msg, wParam, lParam,
                               DefWindowProcW_Original);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    return HandleDefWindowProc(hWnd, Msg, wParam, lParam,
                               DefWindowProcA_Original);
}

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        if (ShouldTreatHideAsClose(hWnd) && !CloseBlocked(hWnd) &&
            StartClose(hWnd, kAsyncHideMsg)) {
            return TRUE;
        }
        return ShowWindowAsync_Original(hWnd, nCmdShow);
    }
    ClearCloseHandled(hWnd);

    DWORD ownerPid = 0;
    const DWORD ownerTid = GetWindowThreadProcessId(hWnd, &ownerPid);
    const bool sameProcess = (ownerPid == GetCurrentProcessId());
    const bool sameThread =
        (sameProcess && ownerTid == GetCurrentThreadId());

    if (IsMinimizeCmd(nCmdShow)) {
        if (ShouldAnimateWindow(hWnd)) {
            if (!sameProcess) {
                if (TryCrossProcessRemoteAnim(hWnd, kRemoteCmdMinimize,
                                              nCmdShow)) {
                    return TRUE;
                }
                const uint32_t packed = QueryDockIconPacked(hWnd);
                TryMinimizeAnim(hWnd, packed, true);
                return ShowWindowAsync_Original(hWnd, nCmdShow);
            }
            TryMinimizeAnim(hWnd, 0, false);
        }
        return ShowWindowAsync_Original(hWnd, nCmdShow);
    }

    if (IsRestoreCmd(nCmdShow) && IsIconic(hWnd)) {
        if (g_openAnimation.load(std::memory_order_relaxed) &&
            ShouldAnimateWindow(hWnd)) {
            if (!sameThread) {
                if (TryCrossProcessRemoteAnim(hWnd, kRemoteCmdRestore,
                                              nCmdShow)) {
                    return TRUE;
                }
                return ShowWindowAsync_Original(hWnd, nCmdShow);
            }
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            BOOL cloaked = SetCloak(hWnd, TRUE) ? TRUE : FALSE;
            BOOL res = ShowWindowAsync_Original(hWnd, nCmdShow);
            StartMinimizeAnim(hWnd, TRUE, origEx, cloaked);
            return res;
        }
        return ShowWindowAsync_Original(hWnd, nCmdShow);
    }

    LONG_PTR launchOrigEx = 0;
    BOOL launchCloaked = FALSE;
    if (LaunchPrepare(hWnd, nCmdShow, &launchOrigEx, &launchCloaked)) {
        BOOL res = ShowWindowAsync_Original(hWnd, nCmdShow);
        LaunchCommit(hWnd, launchOrigEx, launchCloaked);
        return res;
    }

    return ShowWindowAsync_Original(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd,
                                    const WINDOWPLACEMENT* lpwndpl) {
    if (lpwndpl && lpwndpl->showCmd == SW_HIDE &&
        !(lpwndpl->flags & WPF_ASYNCWINDOWPLACEMENT)) {
        bool animated = AnimateBeforeNativeHide(hWnd);
        BOOL res = SetWindowPlacement_Original(hWnd, lpwndpl);
        FinishNativeHide(hWnd, animated);
        return res;
    }
    if (lpwndpl && lpwndpl->showCmd != SW_HIDE &&
        !IsMinimizeCmd(static_cast<int>(lpwndpl->showCmd))) {
        ClearCloseHandled(hWnd);
    }
    if (lpwndpl) {
        const int cmd = static_cast<int>(lpwndpl->showCmd);
        DWORD ownerPid = 0;
        GetWindowThreadProcessId(hWnd, &ownerPid);
        const bool sameProcess = (ownerPid == GetCurrentProcessId());

        if (IsMinimizeCmd(cmd)) {
            if (sameProcess) {
                TryMinimizeAnim(hWnd, 0, false);
            } else if (ShouldAnimateWindow(hWnd)) {
                const uint32_t packed = QueryDockIconPacked(hWnd);
                TryMinimizeAnim(hWnd, packed, true);
            }
            return SetWindowPlacement_Original(hWnd, lpwndpl);
        }

        if (IsRestoreCmd(cmd) && IsIconic(hWnd) &&
            g_openAnimation.load(std::memory_order_relaxed) &&
            ShouldAnimateWindow(hWnd)) {
            if (sameProcess) {
                SetDwmTransitions(hWnd, FALSE);
                LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                BOOL cloaked = SetCloak(hWnd, TRUE) ? TRUE : FALSE;
                BOOL res = SetWindowPlacement_Original(hWnd, lpwndpl);
                StartMinimizeAnim(hWnd, TRUE, origEx, cloaked);
                return res;
            }
            const uint32_t packed = QueryDockIconPacked(hWnd);
            LONG_PTR origEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            BOOL res = SetWindowPlacement_Original(hWnd, lpwndpl);
            StartMinimizeAnim(hWnd, TRUE, origEx, FALSE, FALSE, FALSE, nullptr,
                              FALSE, WM_DESTROY, packed, true);
            return res;
        }
    }
    return SetWindowPlacement_Original(hWnd, lpwndpl);
}

BOOL WINAPI CloseWindow_Hook(HWND hWnd) {
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWnd, &ownerPid);
    if (ownerPid == GetCurrentProcessId()) {
        TryMinimizeAnim(hWnd, 0, false);
    } else if (ShouldAnimateWindow(hWnd)) {
        const uint32_t packed = QueryDockIconPacked(hWnd);
        TryMinimizeAnim(hWnd, packed, true);
    }
    return CloseWindow_Original(hWnd);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    if ((uFlags & SWP_HIDEWINDOW) && !(uFlags & SWP_ASYNCWINDOWPOS)) {
        bool animated = AnimateBeforeNativeHide(hWnd);
        BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                         uFlags);
        FinishNativeHide(hWnd, animated);
        return res;
    }
    if (uFlags & SWP_SHOWWINDOW) {
        ClearCloseHandled(hWnd);
        LONG_PTR launchOrigEx = 0;
        BOOL launchCloaked = FALSE;
        if (LaunchPrepare(hWnd, SW_SHOW, &launchOrigEx, &launchCloaked)) {
            BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx,
                                             cy, uFlags);
            LaunchCommit(hWnd, launchOrigEx, launchCloaked);
            return res;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

DWORD WINAPI LaunchThread(LPVOID lpParam) {
    LaunchData* ld = static_cast<LaunchData*>(lpParam);
    HWND hWnd = ld->hWnd;
    LONG_PTR originalExStyle = ld->originalExStyle;
    BOOL hiddenByCloak = ld->hiddenByCloak;
    delete ld;

    UINT ownCloak = hiddenByCloak ? static_cast<UINT>(DWM_CLOAKED_APP) : 0;
    Sleep(60);
    for (int i = 0; i < 30; ++i) {
        if (!IsWindow(hWnd) || g_unloading.load(std::memory_order_relaxed)) {
            break;
        }
        UINT cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                         sizeof(cloaked))) ||
            !(cloaked & ~ownCloak)) {
            break;
        }
        Sleep(50);
    }

    if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(hWnd) ||
        IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        if (IsWindow(hWnd)) {
            UndoRisingHide(hWnd, originalExStyle, hiddenByCloak);
        }
        return 0;
    }

    StartMinimizeAnim(hWnd, TRUE, originalExStyle, hiddenByCloak);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    g_unloading.store(false, std::memory_order_relaxed);
    g_msgQueryIconCenter =
        RegisterWindowMessageW(L"WindhawkMacOSTahoe_QueryIconCenter");
    g_msgRemoteAnim = RegisterWindowMessageW(L"WindhawkMacOSTahoe_AnimCmd");

    if (IsExplorerProcess()) {
        int savedMinAnim = Wh_GetIntValue(L"restore_minanim", -1);
        if (savedMinAnim != -1) {
            ANIMATIONINFO ai{sizeof(ai), savedMinAnim};
            SystemParametersInfoW(SPI_SETANIMATION, sizeof(ai), &ai,
                                  SPIF_SENDCHANGE);
            Wh_SetIntValue(L"restore_minanim", -1);
        }
    }

    LoadSettings();

    Wh_SetFunctionHook(reinterpret_cast<void*>(DefWindowProcW),
                       reinterpret_cast<void*>(DefWindowProcW_Hook),
                       reinterpret_cast<void**>(&DefWindowProcW_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(DefWindowProcA),
                       reinterpret_cast<void*>(DefWindowProcA_Hook),
                       reinterpret_cast<void**>(&DefWindowProcA_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(DestroyWindow),
                       reinterpret_cast<void*>(DestroyWindow_Hook),
                       reinterpret_cast<void**>(&DestroyWindow_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(ShowWindow),
                       reinterpret_cast<void*>(ShowWindow_Hook),
                       reinterpret_cast<void**>(&ShowWindow_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(ShowWindowAsync),
                       reinterpret_cast<void*>(ShowWindowAsync_Hook),
                       reinterpret_cast<void**>(&ShowWindowAsync_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(SetWindowPlacement),
                       reinterpret_cast<void*>(SetWindowPlacement_Hook),
                       reinterpret_cast<void**>(&SetWindowPlacement_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(CloseWindow),
                       reinterpret_cast<void*>(CloseWindow_Hook),
                       reinterpret_cast<void**>(&CloseWindow_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(SetWindowPos),
                       reinterpret_cast<void*>(SetWindowPos_Hook),
                       reinterpret_cast<void**>(&SetWindowPos_Original));
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    g_unloading.store(true, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        g_unloading.store(true, std::memory_order_relaxed);
    }

    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        threads.swap(g_WorkerThreads);
    }
    for (HANDLE h : threads) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }

    {
        std::lock_guard<std::mutex> lock(g_D2DFactoryMutex);
        if (g_d2dFactory) {
            g_d2dFactory->Release();
            g_d2dFactory = nullptr;
        }
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        if (pair.second.hBmp) {
            DeleteObject(pair.second.hBmp);
        }
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    g_ProcessIconPositions.clear();
    g_DockTargetCache.clear();
    g_LaunchSeen.clear();
    g_AnimActive.clear();
    g_ExcludedPrograms.clear();
}
