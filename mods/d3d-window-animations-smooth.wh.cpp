// ==WindhawkMod==
// @id              d3d-window-animations-smooth
// @name            D3D Window Animations (Smooth & Hz-Aware)
// @description     GPU-composited custom window animations using D3D11 + DirectComposition, monitor-Hz pacing and multi-monitor support.
// @version         1.0.2
// @author          Vladyk
// @github          https://github.com/Booge2
// @include         *
// @compilerOptions -O2 -fdata-sections -ffunction-sections -Wl,--gc-sections -ldwmapi -lgdi32 -ld3d11 -ld3dcompiler -ldcomp -lole32 -loleaut32 -luuid -lshell32 -luiautomationcore -lwinmm
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
// # D3D Window Animations (Smooth & Hz-Aware)

// A high-performance, GPU-composited window animation mod for Windows, powered by **Direct3D 11** and **DirectComposition**. It brings smooth, fluid, and monitor-Hz-aware animations to window minimizing, restoring, opening, and closing.

// ---

// ## ⚠️ Beta Version Notice & First Launch Note

// > **Important Note on First Launch / Update:**
// > When you install or update the mod for the first time, **Windows Explorer / Desktop will briefly restart** to inject the hook into system processes. Additionally, running applications that are animated for the very first time may close or restart once to apply the new D3D11 rendering hook. **This is completely normal system behavior** during initial hook injection — simply reopen your apps, and all subsequent animations will work smoothly without any restarts!

// > **Beta Status:** This mod is currently in **Beta**. While it is designed with stability in mind and tested for smooth everyday performance, unexpected bugs or edge-case behavior with specific applications may still occur.

// If you encounter any issues, glitches, or crashes, please report them! Your feedback helps make this mod more stable and compatible for everyone:
// - **Bug Reports & Issues:** Submit an issue directly on [GitHub Issues](https://github.com/Booge2) (or via the Windhawk mod discussion panel if available).
// - **Details to Include:** Please share your Windows version, display setup (multi-monitor/Hz), and the name of the app (`.exe`) where the issue occurred.

// ---

// ## 🚀 Why D3D11 GPU Acceleration vs. CPU-Based Animations?

// Most traditional window animation mods perform frame calculation and rendering directly on the **CPU (GDI/Software rendering)**. This mod takes a completely different approach by leveraging **Direct3D 11 + DirectComposition** for full hardware acceleration.

// ### Key Advantages:

// 1. **Zero CPU Bottlenecks & No Lag:**
//    - **CPU-based:** Drawing complex transitions frame-by-frame on the CPU causes micro-stutters, high CPU usage spikes, and frame drops when the system is under load (e.g., during gaming or heavy multitasking).
//    - **D3D11 GPU-composited:** The CPU only manages window states, while 100% of the pixel transformations, scaling, and easing are offloaded to your Graphics Card. This ensures stable 60/120/144/240+ FPS without lagging your system.

// 2. **Visual Differences & Shader Capabilities:**
//    - **CPU-based:** Limited to simple geometry moves, crude opacity changes, or slow pixel math that looks jittery on modern high-refresh-rate displays.
//    - **D3D11 Pixel Shaders:** Uses dedicated HLSL pixel shaders. This enables complex visual effects like organic cubic easing curve acceleration, smooth scale/overshoot bounds, sub-pixel blending, and subtle start-up lighting pulses that are impossible to render on the CPU efficiently.

// 3. **High Refresh Rate (Hz-Aware) Synchronization:**
//    - **CPU-based:** Tied to fixed timer intervals (e.g., 60Hz default), resulting in uneven frame pacing or screen tearing on 120Hz/144Hz/240Hz monitors.
//    - **D3D11 + DComp:** Dynamically queries your monitor’s refresh rate and uses high-resolution waitable timers and swapchain frame latency management for pixel-perfect synchronization with your monitor's exact refresh rate.

// 4. **Seamless Isolation (Ghost Canvas Technique):**
//    - The mod renders animation frames onto a lightweight, top-most D3D11 "ghost" overlay surface while hardware-cloaking the real window via DWM. This prevents window flickering, artifacting, or race conditions with app surfaces (like Chromium or Electron renderers).

// ---

// ## ✨ Features

// - **GPU Acceleration (D3D11):** Renders animations smoothly on the graphics card, completely avoiding heavy CPU overhead.
// - **Monitor-Hz Aware Pacing:** Automatically adapts to your display’s refresh rate (supports high-Hz monitors like 120Hz, 144Hz, 240Hz+).
// - **Full Animation Suite:** Custom animations for:
//   - **Minimize** (smooth scaling & fade-out)
//   - **Restore** (fluid expansion back to place)
//   - **Open / Launch** (expressive starting scale & subtle lighting pulse)
//   - **Close** (snappy closing transition)
// - **Multi-Monitor Support:** Correctly calculates window targets and positioning across multiple displays.
// - **Robust Safety & Stability:** Includes built-in smart filters, multi-process warm-up handling, and crash protection for web engines (Chrome, Edge, Electron apps) and system components.

// ---

// ## ⚙️ Settings / Configuration

// You can customize the mod behavior directly via the Windhawk settings panel:

// | Setting Name | Default | Description |
// | :--- | :---: | :---|
// | **Minimize Duration (ms)** | `500` | Duration of the window minimize animation. |
// | **Restore Duration (ms)** | `500` | Duration of the window restore animation. |
// | **Open Duration (ms)** | `500` | Duration of the window open/launch animation. |
// | **Close Duration (ms)** | `500` | Duration of the window close animation. |
// | **Frame Rate** | `0` | Target FPS (`0` = auto-sync with Monitor Hz). |
// | **Enable Minimize Animation** | `true` | Toggle minimize effect. |
// | **Enable Restore Animation** | `true` | Toggle restore effect. |
// | **Enable Open / Launch Animation** | `true` | Toggle application launch effect. |
// | **Enable Close Animation** | `true` | Toggle close effect. |
// | **Multi-Monitor Support** | `true` | Enable multi-display coordinate mapping. |
// | **Excluded Programs** | *(None)* | List of executables where animations should be disabled. |

// ---

// ## 🛡️ Compatibility, Exclusions & Special Apps

// - **Web Browsers & Electron Apps:** Fully compatible with main browser windows (after smart warm-up delays), while safely skipping internal helper/renderer processes.
// - **Special Software (Steam, Wallpaper Engine, etc.):** Heavy desktop utilities, launchers, and wallpaper engines (such as **Steam**, **Wallpaper Engine**, etc.) are automatically detected and **excluded from animations** to prevent rendering conflicts, overlay glitches, or performance drops.
// - **Custom Exclusions:** If any other application behaves unexpectedly, you can easily add its `.exe` name to the `Excluded Programs` list in the mod settings.

// ---

// ## 📝 License

// Distributed under the **MIT License**. See `LICENSE` for more information.
// ==/WindhawkModReadme==

// ==WindhawkModDlls==
// winmm
// ==/WindhawkModDlls==

// ==WindhawkModSettings==
// - minimize_duration_ms_v2: 500
//   $name: Minimize Duration (ms)
// - restore_duration_ms_v2: 500
//   $name: Restore Duration (ms)
// - open_duration_ms_v2: 500
//   $name: Open Duration (ms)
// - close_duration_ms_v2: 500
//   $name: Close Duration (ms)
// - frame_rate: 0
//   $name: Frame Rate (0 = Monitor Hz)
// - minimize_animation_v2: true
//   $name: Enable Minimize Animation
// - restore_animation_v2: true
//   $name: Enable Restore Animation
// - open_animation_v2: true
//   $name: Enable Open / Launch Animation
// - close_animation_v2: true
//   $name: Enable Close Animation
// - multi_monitor: true
//   $name: Multi-Monitor Support
// - mesh_quality: 2
//   $name: Mesh Quality (reserved; pixel-shader path uses a fullscreen quad)
// - unhide_taskbar: true
//   $name: Auto-hide Taskbar Support
// - taskbar_animation_v2: false
//   $name: Move Minimize / Restore Animation to Taskbar
// - excluded_programs:
//   - example.exe
//   $name: Excluded Programs
// - settings_version: 2
//   $name: Settings Version
// ==/WindhawkModSettings==

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <ole2.h>
#include <tlhelp32.h>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <cwchar>
//#include <deque>
#include <utility>
#include <new>

enum AnimationType { ANIM_MINIMIZE=0, ANIM_RESTORE=1, ANIM_OPEN=2, ANIM_CLOSE=3 };

static bool TryAcquireAnimationSlot(HWND hwnd, AnimationType type);
static void ReleaseAnimationSlot();

// -----------------------------------------------------------------------------
// Structured diagnostic tracing
// -----------------------------------------------------------------------------
// Every animation attempt gets a monotonically increasing TRACE id.  Search
// the Windhawk log for "[GENIE][TRACE=" to isolate one complete operation.
// Stage numbers are deliberately stable so the execution order is obvious:
//   01 HOOK -> 02 CHECK -> 03 CAPTURE -> 04 TARGET -> 05 STATE ->
//   06 THREAD -> 07 DCOMP -> 08 FIRST_FRAME -> 09 FRAMES ->
//   10 FINALIZE -> 11 CLEANUP -> 12 DONE
// Non-animation background calls use TRACE=00000000.
// -----------------------------------------------------------------------------
static std::atomic<uint64_t> g_traceSequence{0};
static thread_local uint64_t g_traceId = 0;
static thread_local AnimationType g_traceType = ANIM_MINIMIZE;

static const char* AnimationTypeName(AnimationType type)
{
    switch(type) {
    case ANIM_MINIMIZE: return "MINIMIZE";
    case ANIM_RESTORE:  return "RESTORE";
    case ANIM_OPEN:     return "OPEN";
    case ANIM_CLOSE:    return "CLOSE";
    default:            return "UNKNOWN";
    }
}

static uint64_t GenieTraceBegin(HWND hwnd, AnimationType type)
{
    const uint64_t id = ++g_traceSequence;
    g_traceId = id;
    g_traceType = type;
    Wh_Log(L"[GENIE][TRACE=%08llu][%S][S01][BEGIN] hwnd=%p",
           (unsigned long long)id, AnimationTypeName(type), (void*)hwnd);
    return id;
}

static void GenieTraceAdopt(uint64_t id, AnimationType type)
{
    g_traceId = id;
    g_traceType = type;
}

static void GenieTraceClear()
{
    g_traceId = 0;
}

static void GenieTraceLog(uint64_t id, AnimationType type, int stage,
                          const char* function, const char* status,
                          const char* fmt = nullptr, ...)
{
    char detail[1800]{};
    if(fmt) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(detail, sizeof(detail), fmt, args);
        va_end(args);
    }

    if(detail[0]) {
        Wh_Log(L"[GENIE][TRACE=%08llu][%S][S%02d][%S][%S] %S",
               (unsigned long long)id, AnimationTypeName(type), stage,
               function ? function : "?", status ? status : "INFO", detail);
    } else {
        Wh_Log(L"[GENIE][TRACE=%08llu][%S][S%02d][%S][%S]",
               (unsigned long long)id, AnimationTypeName(type), stage,
               function ? function : "?", status ? status : "INFO");
    }
}

static void GenieTraceCurrent(int stage, const char* function, const char* status,
                              const char* fmt = nullptr, ...)
{
    if(g_traceId == 0) return;
    char detail[1800]{};
    if(fmt) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(detail, sizeof(detail), fmt, args);
        va_end(args);
    }
    GenieTraceLog(g_traceId, g_traceType, stage, function, status, "%s", detail);
}

static void GenieTraceBypass(uint64_t id, AnimationType type, const char* function, const char* reason)
{
    Wh_Log(L"[GENIE][TRACE=%08llu][%S][DONE][BYPASS] stage=S01 function=%S reason=%S",
           (unsigned long long)id, AnimationTypeName(type),
           function ? function : "?", reason ? reason : "not intercepted");
}

static void GenieTraceDone(
    uint64_t id,
    AnimationType type,
    bool success,
    int stage,
    const char* reason,
    unsigned long long frames = 0,
    unsigned long long elapsedMs = 0)
{
    const char* safeReason =
        reason
            ? reason
            : (success ? "completed" : "unknown");

    Wh_Log(
        L"[GENIE][TRACE=%08llu][%S][DONE][%S] "
        L"stage=S%02d frames=%llu elapsed=%llums reason=%S",

        (unsigned long long)id,

        AnimationTypeName(type),

        success ? "SUCCESS" : "FAILED",

        stage,

        frames,

        elapsedMs,

        safeReason
    );
}

// Legacy/free-form logging is kept for non-trace startup/settings diagnostics.
static void GenieLog(const char* fmt, ...)
{
    char buffer[2048]{};
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    Wh_Log(L"[GENIE][INFO] %S", buffer);
}

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif
#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif
#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

#define PI 3.14159265358979323846f
#define TILE_MIN 8
#define TILE_MAX 96
#define FPS_MIN 30
#define FPS_MAX 500

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
using ShowWindowAsync_t = BOOL(WINAPI*)(HWND, int);
using SetWindowPlacement_t = BOOL(WINAPI*)(HWND, const WINDOWPLACEMENT*);
using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
using CloseWindow_t = BOOL(WINAPI*)(HWND);

static DefWindowProcW_t g_DefWindowProcW = nullptr;
static ShowWindow_t g_ShowWindow = nullptr;
static ShowWindowAsync_t g_ShowWindowAsync = nullptr;
static SetWindowPlacement_t g_SetWindowPlacement = nullptr;
static SetWindowPos_t g_SetWindowPos = nullptr;
static CloseWindow_t g_CloseWindow = nullptr;

static std::atomic<bool> g_unloading{false};
static std::atomic<int> g_workers{0};
static std::atomic<int> g_activeAnimations{0};
static constexpr int MAX_CONCURRENT_ANIMATIONS = 4;
static constexpr DWORD SNAPSHOT_HARD_TIMEOUT_MS = 50;
static std::atomic<int> g_minDuration{500};
static std::atomic<int> g_restoreDuration{500};
static std::atomic<int> g_openDuration{480};
static std::atomic<int> g_closeDuration{430};
static std::atomic<int> g_frameRate{0};
static std::atomic<bool> g_minimizeEnabled{true};
static std::atomic<bool> g_restoreEnabled{true};
static std::atomic<bool> g_openEnabled{true};
static std::atomic<bool> g_launchAnimation{true};
static std::atomic<bool> g_closeEnabled{true};
static std::atomic<bool> g_multiMonitor{true};
static std::atomic<int> g_meshQuality{2};
static std::atomic<bool> g_unhideTaskbar{true};
// Taskbar movement is deliberately independent from the shader. This lets
// center/local animations run without being forced toward the taskbar.
static std::atomic<bool> g_taskbarAnimation{false};

static std::mutex g_mutex;
static std::vector<std::wstring> g_excluded;

struct Snapshot {
    HBITMAP bitmap=nullptr; void* bits=nullptr; int width=0; int height=0;
    Snapshot() = default;
    Snapshot(const Snapshot&) = delete;
    Snapshot& operator=(const Snapshot&) = delete;
    Snapshot(Snapshot&& other) noexcept {
        bitmap=other.bitmap; bits=other.bits; width=other.width; height=other.height;
        other.bitmap=nullptr; other.bits=nullptr; other.width=0; other.height=0;
    }
    Snapshot& operator=(Snapshot&& other) noexcept {
        if(this!=&other){
            if(bitmap) DeleteObject(bitmap);
            bitmap=other.bitmap; bits=other.bits; width=other.width; height=other.height;
            other.bitmap=nullptr; other.bits=nullptr; other.width=0; other.height=0;
        }
        return *this;
    }
};
struct MonitorInfoEx { HMONITOR monitor=nullptr; RECT monitorRect{}; RECT workRect{}; int refreshRate=60; };
struct AnimationState { uint64_t generation=0; AnimationType type=ANIM_MINIMIZE; bool running=false; };

static std::unordered_map<HWND, Snapshot> g_snapshots;
static std::unordered_map<HWND, ULONGLONG> g_snapshotLastUsed;
static size_t g_snapshotBytes = 0;
static std::unordered_set<DWORD> g_firstAnimationSafeMode;
static std::unordered_set<HWND> g_taskbarLookupInFlight;
static std::mutex g_shaderCacheMutex;
static ID3DBlob* g_cachedVertexShaderBlob = nullptr;
static ID3DBlob* g_cachedPixelShaderBlob = nullptr;
static std::unordered_map<HWND, AnimationState> g_states;
static std::unordered_map<HWND, int> g_iconPositions;
static std::unordered_map<std::wstring, int> g_processIconPositions;
static std::unordered_set<HWND> g_launchSeen;
static std::unordered_set<HWND> g_animActive;
static uint64_t g_generation=1;

static float Clamp01(float x){ return x<0.0f?0.0f:(x>1.0f?1.0f:x); }
static int ClampInt(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }
static float EaseInOutCubic(float x){ x=Clamp01(x); return x<0.5f?4.0f*x*x*x:1.0f-powf(-2.0f*x+2.0f,3.0f)/2.0f; }
static float EaseOutCubic(float x){ x=Clamp01(x); float a=1.0f-x; return 1.0f-a*a*a; }
static float EaseInCubic(float x){ x=Clamp01(x); return x*x*x; }

static bool IsMainWindowCandidate(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd)) return false;

    // Only top-level windows are eligible. This is the primary protection for
    // browser renderer surfaces, WebView controls and application child HWNDs.
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (style & WS_CHILD) return false;
    if (ex & WS_EX_TOOLWINDOW) return false;
    if (ex & WS_EX_NOACTIVATE) return false;
    if (GetWindow(hwnd, GW_OWNER) != nullptr) return false;

    WCHAR cls[128]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    std::wstring c = cls;
    std::transform(c.begin(), c.end(), c.begin(), ::towlower);

    // These are renderer/helper surfaces, not application windows.
    if (c == L"chrome_renderwidgethosthwnd" ||
        (c == L"chrome_widgetwin_0" && (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) ||
        c == L"electron_utility") {
        return false;
    }

    // IMPORTANT:
    // Do NOT reject Chrome_WidgetWin_1 / CEF browser-window here. A separately
    // installed PWA such as YouTube Music can be a legitimate top-level window
    // using those classes. The GA_ROOT + WS_CHILD checks above still prevent
    // animation of the browser's internal content HWNDs.
    return true;
}

static bool IsWindowUsable(HWND hwnd){ 
    if (!IsWindow(hwnd)) return false; 
    if (!IsMainWindowCandidate(hwnd)) return false;

    RECT r{};
    if (IsIconic(hwnd)) { 
        WINDOWPLACEMENT wp{}; 
        wp.length = sizeof(wp); 
        if (!GetWindowPlacement(hwnd, &wp)) return false; 
        r = wp.rcNormalPosition;
    } else { 
        if (!GetWindowRect(hwnd, &r)) return false; 
    } 

    int width = r.right - r.left; 
    int height = r.bottom - r.top; 

    return (width >= 16 && height >= 16); 
}

static bool IsLaunchWindow(HWND hwnd){
    if(!hwnd || GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return false;
    return IsWindowUsable(hwnd);
}

static bool IsExcluded(HWND hwnd){
    if(!hwnd) return true;
    DWORD pid=0; GetWindowThreadProcessId(hwnd,&pid); if(!pid) return false;
    HANDLE p=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid); if(!p) return false;
    WCHAR path[MAX_PATH]{}; DWORD len=MAX_PATH;
    bool ok=QueryFullProcessImageNameW(p,0,path,&len)!=0; CloseHandle(p); if(!ok) return false;
    std::wstring full(path); std::transform(full.begin(),full.end(),full.begin(),[](wchar_t c){return (wchar_t)towlower(c);});
    size_t slash=full.find_last_of(L"\\/");
    std::wstring base=slash==std::wstring::npos?full:full.substr(slash+1), noExt=base;
    if(noExt.size()>4 && noExt.compare(noExt.size()-4,4,L".exe")==0) noExt.resize(noExt.size()-4);
    std::lock_guard<std::mutex> lock(g_mutex);
    for(const auto& e:g_excluded) if(e==full||e==base||e==noExt) return true;
    return false;
}


enum ProcessKind {
    PROCESS_NATIVE = 0,
    PROCESS_WEB = 1,
    PROCESS_EXPLORER = 2,
    PROCESS_WINDHAWK = 3,
    PROCESS_STEAM = 4,
    PROCESS_WALLPAPER = 5,
    PROCESS_CODEX = 6,
};

struct ProcessInfo {
    ProcessKind kind = PROCESS_NATIVE;
    DWORD pid = 0;
    ULONGLONG firstSeen = 0;
    std::wstring image;
};

static std::unordered_map<DWORD, ULONGLONG> g_processFirstSeen;
static std::unordered_map<DWORD, std::pair<std::wstring, ULONGLONG>> g_processImageCache;

static std::wstring GetProcessImageBaseName(HWND hwnd, DWORD* pidOut = nullptr)
{
    DWORD pid = 0;
    if (hwnd) GetWindowThreadProcessId(hwnd, &pid);
    if (pidOut) *pidOut = pid;
    if (!pid) return L"";

    const ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it=g_processImageCache.find(pid);
        if(it!=g_processImageCache.end() && now-it->second.second < 5000)
            return it->second.first;
    }

    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return L"";
    WCHAR path[MAX_PATH]{};
    DWORD len = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(h, 0, path, &len)) {
        const wchar_t* slash = wcsrchr(path, L'\\');
        result = slash ? slash + 1 : path;
        std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    }
    CloseHandle(h);
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_processImageCache[pid] = {result, now};
        if (g_processImageCache.size() > 256) {
            for (auto it=g_processImageCache.begin(); it!=g_processImageCache.end();) {
                if (now-it->second.second > 10000) it=g_processImageCache.erase(it); else ++it;
            }
        }
    }
    return result;
}

static ProcessInfo ClassifyProcess(HWND hwnd)
{
    ProcessInfo info{};
    info.image = GetProcessImageBaseName(hwnd, &info.pid);
    if (info.pid) {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_processFirstSeen.find(info.pid);
        if (it == g_processFirstSeen.end()) {
            info.firstSeen = GetTickCount64();
            g_processFirstSeen.emplace(info.pid, info.firstSeen);
        } else {
            info.firstSeen = it->second;
        }
    }

    const std::wstring& n = info.image;
    if (n == L"windhawk.exe" || n == L"windhawkui.exe" || n == L"windhawkhelper.exe") {
        info.kind = PROCESS_WINDHAWK;
    } else if (n == L"msedge.exe" || n == L"chrome.exe" || n == L"brave.exe" ||
               n == L"firefox.exe" || n == L"opera.exe" || n == L"vivaldi.exe" ||
               n == L"electron.exe" || n == L"discord.exe" || n == L"code.exe" ||
               n == L"codium.exe" || n == L"obsidian.exe" || n == L"notion.exe" ||
               n == L"spotify.exe" || n == L"postman.exe" || n == L"msedgewebview2.exe" ||
               n == L"codex.exe" || n == L"codex-windows.exe") {
        info.kind = (n == L"codex.exe" || n == L"codex-windows.exe") ? PROCESS_CODEX : PROCESS_WEB;
    } else if (n == L"explorer.exe") {
        info.kind = PROCESS_EXPLORER;
    } else if (n == L"steam.exe" || n == L"steamwebhelper.exe") {
        info.kind = PROCESS_STEAM;
    } else if (n == L"wallpaper32.exe" || n == L"wallpaper64.exe" ||
               n == L"wallpaperservice32.exe" || n == L"wallpaperservice64.exe" ||
               n == L"wallpaperengine.exe" || n == L"wallpaper_engine.exe" ||
               n == L"wallpaperengineui.exe") {
        info.kind = PROCESS_WALLPAPER;
    }
    return info;
}

static bool IsWebEngineProcess(HWND hwnd)
{
    ProcessKind k = ClassifyProcess(hwnd).kind;
    return k == PROCESS_WEB || k == PROCESS_CODEX;
}

static bool IsHostProcessProtected(HWND hwnd)
{
    return ClassifyProcess(hwnd).kind == PROCESS_WINDHAWK;
}



static bool IsSystemWindowProtected(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd)) return true;

    // Never animate the taskbar/desktop infrastructure itself.
    WCHAR cls[128]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    std::wstring c = cls;
    std::transform(c.begin(), c.end(), c.begin(), ::towlower);

    if (c == L"shell_traywnd" ||
        c == L"shell_secondarytraywnd" ||
        c == L"progman" ||
        c == L"workerw") {
        return true;
    }

    DWORD pid = 0;
    std::wstring image = GetProcessImageBaseName(hwnd, &pid);
    if (image == L"dwm.exe" ||
        image == L"sihost.exe" ||
        image == L"shellexperiencehost.exe" ||
        image == L"startmenuexperiencehost.exe" ||
        image == L"textinputhost.exe" ||
        image == L"searchhost.exe" ||
        image == L"runtimebroker.exe" ||
        image == L"lockapp.exe") {
        return true;
    }

    return false;
}

static bool IsNonInteractiveSessionProcess()
{
    DWORD sessionId = 0;
    return ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0;
}

static bool IsNonWindowHostingHelperProcess()
{
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return false;

    bool helper = false;
    for (int i = 1; i < argc && !helper; ++i) {
        const wchar_t* role = nullptr;
        if (_wcsnicmp(argv[i], L"--type=", 7) == 0) {
            role = argv[i] + 7;
        } else if (_wcsicmp(argv[i], L"--type") == 0 && i + 1 < argc) {
            role = argv[++i];
        }
        if (!role) continue;
        static constexpr const wchar_t* kRoles[] = {
            L"renderer", L"gpu-process", L"utility", L"crashpad-handler",
            L"plugin", L"ppapi", L"zygote", L"nacl-loader", L"nacl-broker"
        };
        for (const wchar_t* r : kRoles) {
            if (_wcsicmp(role, r) == 0) { helper = true; break; }
        }
    }
    LocalFree(argv);
    return helper;
}

static bool FirstAnimationSafeMode(HWND hwnd, AnimationType type)
{
    ProcessInfo p = ClassifyProcess(hwnd);
    if (p.kind != PROCESS_WEB && p.kind != PROCESS_CODEX &&
        p.kind != PROCESS_STEAM && p.kind != PROCESS_WALLPAPER) {
        return false;
    }
    if (!p.pid) return false;

    // Record the first animation per process for diagnostics, but do not suppress
    // it. The first user action after enabling the mod and the first launch/show
    // should use the configured animation immediately.
    std::lock_guard<std::mutex> lock(g_mutex);
    const auto result = g_firstAnimationSafeMode.insert(p.pid);
    if (result.second) {
        GenieTraceCurrent(2, "FirstLaunchSafeMode", "FIRST_ACTION_ALLOWED",
                          "pid=%lu image=%S kind=%d type=%d",
                          (unsigned long)p.pid, p.image.c_str(), (int)p.kind, (int)type);
    }
    return false;
}

static bool IsExplorerFolderWindow(HWND hwnd)
{
    if (ClassifyProcess(hwnd).kind != PROCESS_EXPLORER) return true;
    WCHAR cls[96]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    return _wcsicmp(cls, L"CabinetWClass") == 0 ||
           _wcsicmp(cls, L"ExploreWClass") == 0;
}

static bool WebEngineWarmupActive(HWND hwnd, AnimationType type)
{
    ProcessInfo p = ClassifyProcess(hwnd);
    if (!IsWebEngineProcess(hwnd)) return false;
    // Never interfere with the host process during its first 750 ms. The first
    // real page/surface frame belongs to the application, not the animation mod.
    const ULONGLONG age = p.firstSeen ? GetTickCount64() - p.firstSeen : 0;
    if (age < 750) {
        GenieTraceCurrent(2, "WebEngineBootstrap", "WARMUP_BYPASS",
                          "image=%S pid=%lu age=%llums type=%d",
                          p.image.c_str(), (unsigned long)p.pid,
                          (unsigned long long)age, (int)type);
        return true;
    }
    return false;
}

static bool IsSafeAnimationWindow(HWND hwnd, AnimationType type)
{
    if (!IsMainWindowCandidate(hwnd)) return false;
    if (IsSystemWindowProtected(hwnd)) return false;
    if (IsHostProcessProtected(hwnd)) return false;
    if (!IsExplorerFolderWindow(hwnd)) return false;

    // Web/CEF child surfaces are never intercepted. Codex/Chromium main windows
    // are allowed after warm-up, but their renderer/helper HWNDs are not.
    ProcessInfo p = ClassifyProcess(hwnd);
    if ((p.kind == PROCESS_WEB || p.kind == PROCESS_CODEX) &&
        !IsWindowVisible(hwnd) && type != ANIM_RESTORE) return false;
    return true;
}

static bool AnimationUsesTaskbarTarget(AnimationType type)
{
    // Current center/local shaders do NOT move to the taskbar.
    // Enable the setting when using a shader/design that is meant to travel
    // between the window center and its taskbar button.
    return g_taskbarAnimation.load() &&
           (type == ANIM_MINIMIZE || type == ANIM_RESTORE);
}

static void SetDwmTransitions(HWND hwnd,bool enabled){ BOOL disable=enabled?FALSE:TRUE; DwmSetWindowAttribute(hwnd,DWMWA_TRANSITIONS_FORCEDISABLED,&disable,sizeof(disable)); }
static bool SetCloak(HWND hwnd,bool cloak){
    BOOL v = cloak ? TRUE : FALSE;
    return SUCCEEDED(DwmSetWindowAttribute(
        hwnd,
        DWMWA_CLOAK,
        &v,
        sizeof(v)));
}

static bool PrepareRestoreVisualIsolation(HWND hwnd)
{
    if (!SetCloak(hwnd, true)) {
        GenieTraceCurrent(
            5,
            "RestoreIsolation",
            "FAIL",
            "DWMWA_CLOAK=TRUE failed");
        return false;
    }

    // Force DWM to commit the cloak before the native restore/placement call.
    // Without this flush, Windows can present one native restore frame before
    // the custom D3D/DirectComposition ghost becomes visible.
    DwmFlush();
    return true;
}

static MonitorInfoEx GetMonitorForWindow(HWND hwnd){
    MonitorInfoEx r{};
    GenieTraceCurrent(4,"GetMonitorForWindow","ENTER","hwnd=%p",(void*)hwnd);
    r.monitor=MonitorFromWindow(hwnd,g_multiMonitor.load()?MONITOR_DEFAULTTONEAREST:MONITOR_DEFAULTTOPRIMARY);
    if(!r.monitor) r.monitor=MonitorFromPoint(POINT{0,0},MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEXW mi{}; mi.cbSize=sizeof(mi);
    if(GetMonitorInfoW(r.monitor,reinterpret_cast<LPMONITORINFO>(&mi))){ r.monitorRect=mi.rcMonitor; r.workRect=mi.rcWork;
        DEVMODEW dm{}; dm.dmSize=sizeof(dm);
        if(EnumDisplaySettingsW(mi.szDevice,ENUM_CURRENT_SETTINGS,&dm) && dm.dmDisplayFrequency>=30 && dm.dmDisplayFrequency<=1000) r.refreshRate=(int)dm.dmDisplayFrequency;
    }
    if(r.monitorRect.right<=r.monitorRect.left){ r.monitorRect.left=0; r.monitorRect.top=0; r.monitorRect.right=GetSystemMetrics(SM_CXSCREEN); r.monitorRect.bottom=GetSystemMetrics(SM_CYSCREEN); r.workRect=r.monitorRect; }
    if(r.refreshRate<30) r.refreshRate=60;
    GenieTraceCurrent(4,"GetMonitorForWindow","RESULT","monitor=%p rect=%ld,%ld-%ld,%ld work=%ld,%ld-%ld,%ld hz=%d",
                      (void*)r.monitor,r.monitorRect.left,r.monitorRect.top,r.monitorRect.right,r.monitorRect.bottom,
                      r.workRect.left,r.workRect.top,r.workRect.right,r.workRect.bottom,r.refreshRate);
    return r;
}
static int GetTargetFPS(HWND hwnd){ int v=g_frameRate.load(); return v>0?ClampInt(v,FPS_MIN,FPS_MAX):ClampInt(GetMonitorForWindow(hwnd).refreshRate,FPS_MIN,FPS_MAX); }

static void DeleteSnapshot(Snapshot& s){ if(s.bitmap) DeleteObject(s.bitmap); s={}; }
static bool CreateSnapshotBitmap(Snapshot& s,int w,int h){
    GenieTraceCurrent(3,"CreateSnapshotBitmap","ENTER","size=%dx%d",w,h);
    DeleteSnapshot(s); if(w<=0||h<=0){ GenieTraceCurrent(3,"CreateSnapshotBitmap","FAIL","invalid size"); return false; } HDC dc=GetDC(nullptr); if(!dc) return false;
    BITMAPINFO bi{}; bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth=w; bi.bmiHeader.biHeight=-h; bi.bmiHeader.biPlanes=1; bi.bmiHeader.biBitCount=32; bi.bmiHeader.biCompression=BI_RGB;
    s.bitmap=CreateDIBSection(dc,&bi,DIB_RGB_COLORS,&s.bits,nullptr,0); ReleaseDC(nullptr,dc);
    if(!s.bitmap||!s.bits){DeleteSnapshot(s);GenieTraceCurrent(3,"CreateSnapshotBitmap","FAIL","CreateDIBSection");return false;} s.width=w;s.height=h; memset(s.bits,0,(size_t)w*h*4); GenieTraceCurrent(3,"CreateSnapshotBitmap","SUCCESS","bitmap=%p bits=%p",(void*)s.bitmap,s.bits); return true;
}
static bool ValidateSnapshot(const Snapshot& s)
{
    return s.bits != nullptr && s.width >= 1 && s.height >= 1;
}
static bool CaptureScreen(const RECT& rect,Snapshot& out){
    int w=rect.right-rect.left,h=rect.bottom-rect.top;
    GenieTraceCurrent(3,"CaptureScreen","ENTER","rect=%ld,%ld-%ld,%ld size=%dx%d",rect.left,rect.top,rect.right,rect.bottom,w,h); if(w<=0||h<=0||!CreateSnapshotBitmap(out,w,h)) return false;
    HDC screen=GetDC(nullptr), mem=screen?CreateCompatibleDC(screen):nullptr; if(!screen||!mem){if(mem)DeleteDC(mem);if(screen)ReleaseDC(nullptr,screen);DeleteSnapshot(out);return false;}
    HBITMAP old=(HBITMAP)SelectObject(mem,out.bitmap); BOOL ok=BitBlt(mem,0,0,w,h,screen,rect.left,rect.top,SRCCOPY|CAPTUREBLT); GdiFlush(); SelectObject(mem,old); DeleteDC(mem); ReleaseDC(nullptr,screen);
    if(!ok){DeleteSnapshot(out);GenieTraceCurrent(3,"CaptureScreen","FAIL","BitBlt");return false;}
    uint32_t* pixels=(uint32_t*)out.bits; size_t count=(size_t)w*h;
    for(size_t i=0;i<count;i++) pixels[i]|=0xFF000000u;
    GenieTraceCurrent(3,"CaptureScreen","SUCCESS","pixels=%zu",count);
    return true;
}

// Виправлена та розширена функція отримання точних координат прямокутника вікна
static bool GetAccurateWindowRect(HWND hwnd, RECT* outRect) {
    if (!hwnd || !outRect) return false;
    RECT r{};
    if (IsIconic(hwnd)) {
        WINDOWPLACEMENT wp{};
        wp.length = sizeof(wp);
        if (GetWindowPlacement(hwnd, &wp)) {
            r = wp.rcNormalPosition;
        } else {
            GetWindowRect(hwnd, &r);
        }
    } else {
        if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r))) ||
            r.right <= r.left || r.bottom <= r.top) {
            GetWindowRect(hwnd, &r);
        }
    }
    *outRect = r;
    return (r.right > r.left && r.bottom > r.top);
}

static bool CaptureWindow(HWND hwnd,Snapshot& out,RECT& outRect,bool preferScreen)
{
    GenieTraceCurrent(3,"CaptureWindow","ENTER","hwnd=%p preferScreen=%d",(void*)hwnd,(int)preferScreen);
    RECT raw{};
    if(!GetWindowRect(hwnd,&raw)){
        GenieTraceCurrent(3,"CaptureWindow","FAIL","GetWindowRect error=%lu",(unsigned long)GetLastError());
        return false;
    }
    RECT ext=raw;
    GetAccurateWindowRect(hwnd,&ext);
    outRect=ext;

    bool visible=IsWindowVisible(hwnd)!=FALSE;
    GenieTraceCurrent(3,"CaptureWindow","RECT","raw=%ld,%ld-%ld,%ld ext=%ld,%ld-%ld,%ld visible=%d",
                      raw.left,raw.top,raw.right,raw.bottom,ext.left,ext.top,ext.right,ext.bottom,(int)visible);

    if(visible){
        // Screen capture is the safest path for all visible windows and, critically,
        // is the only path allowed for Chromium/Electron/Codex. PrintWindow can
        // synchronously enter a renderer/UI thread and is therefore forbidden there.
        bool ok=CaptureScreen(ext,out);
        GenieTraceCurrent(3,"CaptureWindow",ok?"SCREEN_OK":"SCREEN_FAIL","visible screen capture");
        if(ok)return true;
    }
    // This routine runs on a detached capture worker with a hard caller-side
    // deadline. Do not call PrintWindow here: it is synchronous and can hang
    // indefinitely in an application's UI/render path. Visible-window capture
    // is screen-only; if it fails, the caller safely falls back to native UI.
    if(visible){
        bool ok=CaptureScreen(ext,out);
        GenieTraceCurrent(3,"CaptureWindow",ok?"SCREEN_OK":"SCREEN_FAIL","fallback CaptureScreen");
        if(ok)return true;
    }
    GenieTraceCurrent(3,"CaptureWindow","FAIL","all capture methods failed");
    return false;
}


struct CaptureJob {
    std::atomic<LONG> refs{2}; // caller + worker
    std::atomic<bool> cancelled{false};
    HWND hwnd = nullptr;
    Snapshot snapshot{};
    RECT rect{};
    bool preferScreen = true;
    bool success = false;
    HANDLE done = nullptr;
};

static void ReleaseCaptureJob(CaptureJob* job)
{
    if (!job || job->refs.fetch_sub(1, std::memory_order_acq_rel) != 1) return;
    if (job->done) CloseHandle(job->done);
    DeleteSnapshot(job->snapshot);
    delete job;
}

static DWORD WINAPI CaptureWorker(LPVOID p)
{
    CaptureJob* job=(CaptureJob*)p;
    if(!job) return 0;
    if (!job->cancelled.load(std::memory_order_acquire)) {
        job->success=CaptureWindow(job->hwnd,job->snapshot,job->rect,job->preferScreen);
    }
    if(job->done) SetEvent(job->done);
    ReleaseCaptureJob(job);
    g_workers.fetch_sub(1, std::memory_order_release);
    return 0;
}

static bool CaptureSnapshotAsync(HWND hwnd, Snapshot& out, RECT& outRect, bool preferScreen=true)
{
    if(!hwnd || !IsWindow(hwnd)) return false;
    if(!IsWindowVisible(hwnd) && !IsIconic(hwnd)) return false;

    ProcessInfo pi=ClassifyProcess(hwnd);
    if((pi.kind==PROCESS_WEB || pi.kind==PROCESS_CODEX) && IsIconic(hwnd)) return false;

    CaptureJob* job=new (std::nothrow) CaptureJob();
    if(!job) return false;
    job->hwnd=hwnd;
    job->preferScreen=preferScreen || IsWindowVisible(hwnd);
    job->done=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if(!job->done){ ReleaseCaptureJob(job); return false; }

    g_workers.fetch_add(1, std::memory_order_relaxed);
    HANDLE thread=CreateThread(nullptr,0,CaptureWorker,job,0,nullptr);
    if(!thread){
        g_workers.fetch_sub(1, std::memory_order_release); ReleaseCaptureJob(job); ReleaseCaptureJob(job); return false; }
    CloseHandle(thread);

    const DWORD wait=WaitForSingleObject(job->done,SNAPSHOT_HARD_TIMEOUT_MS);
    bool ok=false;
    if(wait==WAIT_OBJECT_0 && IsWindow(hwnd) && job->success && !job->cancelled.load(std::memory_order_acquire)){
        out=std::move(job->snapshot); outRect=job->rect; ok=true;
    } else if (wait != WAIT_OBJECT_0) {
        // A worker that misses the deadline is detached and owns its remaining state.
        // Never free the job or its event while PrintWindow/BitBlt may still be running.
        job->cancelled.store(true, std::memory_order_release);
        GenieTraceCurrent(3,"CaptureSnapshotAsync","TIMEOUT",
                          "hard deadline=%lums hwnd=%p", (unsigned long)SNAPSHOT_HARD_TIMEOUT_MS, (void*)hwnd);
    }
    ReleaseCaptureJob(job); // release caller reference; worker owns the other one
    if(!ok) DeleteSnapshot(out);
    return ok;
}

static void CacheSnapshot(HWND hwnd, Snapshot&& s)
{
    if(!hwnd || !s.bits || !ValidateSnapshot(s)){ DeleteSnapshot(s); return; }
    const size_t bytes=(size_t)s.width*(size_t)s.height*4u;
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it=g_snapshots.find(hwnd);
    if(it!=g_snapshots.end()){
        g_snapshotBytes -= (size_t)it->second.width*(size_t)it->second.height*4u;
        DeleteSnapshot(it->second);
    }
    const int cachedW=s.width, cachedH=s.height;
    g_snapshots[hwnd]=std::move(s);
    g_snapshotLastUsed[hwnd]=GetTickCount64();
    g_snapshotBytes += bytes;

    while(g_snapshots.size()>4 || g_snapshotBytes>48ull*1024ull*1024ull){
        HWND victim=nullptr; ULONGLONG oldest=~0ull;
        for(const auto& kv:g_snapshots){
            const auto jt=g_snapshotLastUsed.find(kv.first);
            const ULONGLONG age=(jt==g_snapshotLastUsed.end()?0:jt->second);
            if(age<oldest){oldest=age;victim=kv.first;}
        }
        if(!victim) break;
        auto vt=g_snapshots.find(victim);
        if(vt!=g_snapshots.end()){
            g_snapshotBytes -= (size_t)vt->second.width*(size_t)vt->second.height*4u;
            DeleteSnapshot(vt->second); g_snapshots.erase(vt);
        }
        g_snapshotLastUsed.erase(victim);
    }
    GenieTraceCurrent(3,"CacheSnapshot","SUCCESS","stored hwnd=%p size=%dx%d cache=%zu bytes=%zu",
                      (void*)hwnd,cachedW,cachedH,g_snapshots.size(),g_snapshotBytes);
}

static bool TakeCachedSnapshot(HWND hwnd,int w,int h,Snapshot& out)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it=g_snapshots.find(hwnd);
    if(it==g_snapshots.end()) return false;
    if(it->second.width!=w||it->second.height!=h){
        g_snapshotBytes -= (size_t)it->second.width*(size_t)it->second.height*4u;
        DeleteSnapshot(it->second); g_snapshots.erase(it); g_snapshotLastUsed.erase(hwnd);
        return false;
    }
    if(!CreateSnapshotBitmap(out,w,h)) return false;
    memcpy(out.bits,it->second.bits,(size_t)w*h*4);
    g_snapshotLastUsed[hwnd]=GetTickCount64();
    const bool valid=ValidateSnapshot(out);
    GenieTraceCurrent(3,"TakeCachedSnapshot",valid?"HIT":"FAIL",
                      "hwnd=%p size=%dx%d retained=1",(void*)hwnd,w,h);
    return valid;
}

// Restore-specific snapshot lookup.
// Unlike TakeCachedSnapshot(), this does NOT compare the cached
// snapshot size with the current minimized window rectangle.
// The cached snapshot was captured while the window was visible,
// so its dimensions are authoritative for the restore ghost.
static bool TakeCachedRestoreSnapshot(HWND hwnd, Snapshot& out)
{
    out = {};

    if (!hwnd)
        return false;

    std::lock_guard<std::mutex> lock(g_mutex);

    auto it = g_snapshots.find(hwnd);
    if (it == g_snapshots.end())
        return false;

    Snapshot& cached = it->second;

    if (!cached.bits ||
        cached.width <= 0 ||
        cached.height <= 0)
        return false;

    // Transfer ownership to the caller and update cache accounting before erase.
    // Snapshot's move assignment clears all source fields, including bitmap.
    const size_t cachedBytes =
        (size_t)cached.width * (size_t)cached.height * 4u;
    out = std::move(cached);
    g_snapshotBytes = (g_snapshotBytes >= cachedBytes)
        ? (g_snapshotBytes - cachedBytes) : 0;
    g_snapshotLastUsed.erase(hwnd);
    g_snapshots.erase(it);

    return true;
}

static void InvalidateSnapshot(HWND hwnd)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it=g_snapshots.find(hwnd);
    if(it!=g_snapshots.end()){
        g_snapshotBytes -= (size_t)it->second.width*(size_t)it->second.height*4u;
        DeleteSnapshot(it->second); g_snapshots.erase(it);
    }
    g_snapshotLastUsed.erase(hwnd);
}

static bool IsAnimationRunning(HWND hwnd)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_animActive.count(hwnd) != 0;
}

// Reserve a window before any capture or worker creation. This closes the
// check-then-start race between DefWindowProc, ShowWindow and placement hooks.
static bool TryBeginAnimation(HWND hwnd, AnimationType type, uint64_t* generationOut)
{
    if (!hwnd || !generationOut)
        return false;

    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_states.find(hwnd);
    if (it != g_states.end() && it->second.running)
        return false;

    const uint64_t generation = ++g_generation;
    g_states[hwnd] = { generation, type, true };
    g_animActive.insert(hwnd);
    *generationOut = generation;
    return true;
}

static bool IsAnimationCurrent(HWND hwnd,uint64_t g){std::lock_guard<std::mutex> lock(g_mutex);auto it=g_states.find(hwnd);return it!=g_states.end()&&it->second.running&&it->second.generation==g;}
static void EndAnimation(HWND hwnd,uint64_t g){std::lock_guard<std::mutex> lock(g_mutex);auto it=g_states.find(hwnd);if(it!=g_states.end()&&it->second.generation==g){it->second.running=false;g_animActive.erase(hwnd);}}

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hMainTray = FindWindowW(L"Shell_TrayWnd", NULL);
    HMONITOR mainMon = MonitorFromWindow(hMainTray, MONITOR_DEFAULTTOPRIMARY);
    if (hMon == mainMon || !hMon) return hMainTray;

    HWND hSecTray = NULL;
    while ((hSecTray = FindWindowExW(NULL, hSecTray, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        if (MonitorFromWindow(hSecTray, MONITOR_DEFAULTTONULL) == hMon) {
            return hSecTray;
        }
    }
    return hMainTray;
}

static int LookupTaskbarButtonXSlow(HWND hWndApp, int fallbackX, HMONITOR hMon) {
    GenieTraceCurrent(4,"GetTaskbarButtonX","ENTER","hwnd=%p fallbackX=%d monitor=%p",(void*)hWndApp,fallbackX,(void*)hMon);
    int targetX = fallbackX;
    bool uiaFound = false;

    std::wstring procNameLower = L"";
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWndApp, &ownerPid);
    if (ownerPid) {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ownerPid);
        if (hProc) {
            WCHAR exePath[MAX_PATH] = {0};
            DWORD exePathLen = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, exePath, &exePathLen)) {
                WCHAR* name = wcsrchr(exePath, L'\\');
                if (name) {
                    procNameLower = (name + 1);
                    size_t dotPos = procNameLower.find(L'.');
                    if (dotPos != std::wstring::npos) procNameLower = procNameLower.substr(0, dotPos);
                    std::transform(procNameLower.begin(), procNameLower.end(), procNameLower.begin(), ::towlower);
                }
            }
            CloseHandle(hProc);
        }
    }

    std::wstring processKey = procNameLower;
    if (!processKey.empty() && hMon) {
        processKey += L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool coInit = (hr == S_OK || hr == S_FALSE);

    if (hr == S_OK || hr == S_FALSE || hr == RPC_E_CHANGED_MODE) {
        IUIAutomation* pAutomation = nullptr;
        HRESULT hrUia = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
        if (FAILED(hrUia)) {
            hrUia = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
        }

        if (SUCCEEDED(hrUia) && pAutomation) {
            HWND hTray = FindTaskbarForMonitor(hMon);
            if (hTray) {
                IUIAutomationElement* pTrayElement = nullptr;
                if (SUCCEEDED(pAutomation->ElementFromHandle(hTray, &pTrayElement)) && pTrayElement) {

                    WCHAR titleW[512] = {0};
                    GetWindowTextW(hWndApp, titleW, 512);
                    std::wstring titleLower = titleW;
                    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::towlower);

                    std::wstring procHintLower = procNameLower;
                    if (procNameLower == L"chrome") procHintLower = L"google chrome";
                    else if (procNameLower == L"msedge") procHintLower = L"microsoft edge";
                    else if (procNameLower == L"firefox") procHintLower = L"firefox";

                    IUIAutomationCondition* pButtonCond = nullptr;
                    IUIAutomationCondition* pListItemCond = nullptr;
                    IUIAutomationCondition* pOrCond = nullptr;

                    VARIANT varBtn; varBtn.vt = VT_I4; varBtn.lVal = UIA_ButtonControlTypeId;
                    pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &pButtonCond);

                    VARIANT varList; varList.vt = VT_I4; varList.lVal = UIA_ListItemControlTypeId;
                    pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &pListItemCond);

                    if (pButtonCond && pListItemCond) {
                        pAutomation->CreateOrCondition(pButtonCond, pListItemCond, &pOrCond);
                    }

                    IUIAutomationElementArray* pArray = nullptr;
                    if (pOrCond && SUCCEEDED(pTrayElement->FindAll(TreeScope_Descendants, pOrCond, &pArray)) && pArray) {
                        int length = 0;
                        pArray->get_Length(&length);

                        MONITORINFO mi = {0};
                        mi.cbSize = sizeof(MONITORINFO);
                        GetMonitorInfoW(hMon, &mi);
                        int monRight = mi.rcMonitor.right;

                        int bestScore = 0;

                        for (int i = 0; i < length; i++) {
                            IUIAutomationElement* pItem = nullptr;
                            if (SUCCEEDED(pArray->GetElement(i, &pItem)) && pItem) {
                                BSTR name;
                                if (SUCCEEDED(pItem->get_CurrentName(&name)) && name) {
                                    std::wstring uiaNameLower = name;
                                    std::transform(uiaNameLower.begin(), uiaNameLower.end(), uiaNameLower.begin(), ::towlower);

                                    if (!uiaNameLower.empty()) {
                                        int score = 0;

                                        if (titleLower == uiaNameLower) score += 1000;
                                        if (!titleLower.empty() && titleLower.find(uiaNameLower) != std::wstring::npos) score += 500;
                                        if (!uiaNameLower.empty() && uiaNameLower.find(titleLower) != std::wstring::npos) score += 500;

                                        if (!procNameLower.empty() && uiaNameLower.find(procNameLower) != std::wstring::npos) score += 400;
                                        if (!procHintLower.empty() && procHintLower != procNameLower &&
                                            uiaNameLower.find(procHintLower) != std::wstring::npos) score += 900;

                                        if (score > bestScore) {
                                            RECT bRect;
                                            if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&bRect))) {
                                                if (bRect.right > bRect.left && bRect.left < monRight - 50) {
                                                    bestScore = score;
                                                    targetX = bRect.left + (bRect.right - bRect.left) / 2;
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
                    if (pButtonCond) pButtonCond->Release();
                    if (pListItemCond) pListItemCond->Release();
                    if (pOrCond) pOrCond->Release();
                    pTrayElement->Release();
                }
            }
            pAutomation->Release();
        }
        if (coInit) CoUninitialize();
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    if (uiaFound) {
        g_iconPositions[hWndApp] = targetX;
        if (!processKey.empty()) {
            g_processIconPositions[processKey] = targetX;
        }
    } else if (g_iconPositions.count(hWndApp)) {
        targetX = g_iconPositions[hWndApp];
    } else if (!processKey.empty() && g_processIconPositions.count(processKey)) {
        targetX = g_processIconPositions[processKey];
    }

    GenieTraceCurrent(4,"GetTaskbarButtonX",uiaFound?"UIA":"CACHE_OR_FALLBACK","targetX=%d",targetX);
    return targetX;
}

struct TaskbarLookupJob { HWND hwnd{}; int fallbackX{}; HMONITOR monitor{}; };
static DWORD WINAPI TaskbarLookupWorker(LPVOID p)
{
    TaskbarLookupJob* job=(TaskbarLookupJob*)p;
    if(!job) return 0;
    LookupTaskbarButtonXSlow(job->hwnd,job->fallbackX,job->monitor);
    { std::lock_guard<std::mutex> lock(g_mutex); g_taskbarLookupInFlight.erase(job->hwnd); }
    delete job;
    g_workers.fetch_sub(1, std::memory_order_release);
    return 0;
}

static int GetTaskbarButtonX(HWND hWndApp, int fallbackX, HMONITOR hMon)
{
    if (g_unloading.load(std::memory_order_acquire)) return fallbackX;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_iconPositions.count(hWndApp)) return g_iconPositions[hWndApp];
        std::wstring key=L"";
        DWORD pid=0; GetWindowThreadProcessId(hWndApp,&pid);
        HANDLE p=pid?OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid):nullptr;
        if(p){ WCHAR path[MAX_PATH]{}; DWORD len=MAX_PATH; if(QueryFullProcessImageNameW(p,0,path,&len)){ const wchar_t* n=wcsrchr(path,L'\\'); key=n?n+1:path; std::transform(key.begin(),key.end(),key.begin(),::towlower); } CloseHandle(p); }
        if(hMon) key += L"_"+std::to_wstring(reinterpret_cast<size_t>(hMon));
        if(!key.empty() && g_processIconPositions.count(key)) return g_processIconPositions[key];
        if(!g_taskbarLookupInFlight.count(hWndApp)) g_taskbarLookupInFlight.insert(hWndApp);
        else return fallbackX;
    }
    TaskbarLookupJob* job=new (std::nothrow) TaskbarLookupJob{hWndApp,fallbackX,hMon};
    if(!job){ std::lock_guard<std::mutex> lock(g_mutex); g_taskbarLookupInFlight.erase(hWndApp); return fallbackX; }
    g_workers.fetch_add(1, std::memory_order_relaxed);
    HANDLE t=CreateThread(nullptr,0,TaskbarLookupWorker,job,0,nullptr);
    if(t) CloseHandle(t); else { g_workers.fetch_sub(1, std::memory_order_release); delete job; std::lock_guard<std::mutex> lock(g_mutex); g_taskbarLookupInFlight.erase(hWndApp); }
    GenieTraceCurrent(4,"GetTaskbarButtonX","ASYNC_MISS","returning fallback=%d; lookup queued",fallbackX);
    return fallbackX;
}

static POINT GetTaskbarTargetPoint(HMONITOR hMon, int fallbackX) {
    GenieTraceCurrent(4,"GetTaskbarTargetPoint","ENTER","fallbackX=%d monitor=%p",fallbackX,(void*)hMon);
    POINT p{fallbackX, 0};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        p.y = mi.rcWork.bottom - 12;
    } else {
        p.y = GetSystemMetrics(SM_CYSCREEN) - 12;
    }

    HWND tray = FindTaskbarForMonitor(hMon);
    RECT tr{};
    if (tray && GetWindowRect(tray, &tr) && tr.right > tr.left && tr.bottom > tr.top) {
        const int cx = tr.left + (tr.right - tr.left) / 2;
        const int cy = tr.top + (tr.bottom - tr.top) / 2;
        if (tr.top <= mi.rcMonitor.top + 2 && tr.bottom < mi.rcMonitor.bottom) {
            p.y = cy;
        } else if (tr.bottom >= mi.rcMonitor.bottom - 2 && tr.top > mi.rcMonitor.top) {
            p.y = cy;
        } else if (tr.left <= mi.rcMonitor.left + 2 || tr.right >= mi.rcMonitor.right - 2) {
            p.y = cy;
        }
        if (p.x == fallbackX) p.x = cx;
    }
    GenieTraceCurrent(4,"GetTaskbarTargetPoint","RESULT","target=%d,%d",p.x,p.y);
    return p;
}

class FrameScheduler {
public:
    FrameScheduler(HWND hwnd,int duration):m_duration(std::max(1,duration)),m_fps(GetTargetFPS(hwnd)){
        QueryPerformanceFrequency(&m_freq); QueryPerformanceCounter(&m_start); m_nextTick=0.0;
        m_interval=1.0/(double)m_fps;
        m_timer=CreateWaitableTimerExW(nullptr,nullptr,CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,TIMER_ALL_ACCESS);
        if(!m_timer) m_timer=CreateWaitableTimerW(nullptr,FALSE,nullptr);
        GenieTraceCurrent(9,"FrameScheduler","READY","duration=%d fps=%d interval=%.6fs timer=%p",m_duration,m_fps,m_interval,(void*)m_timer);
    }
    ~FrameScheduler(){if(m_timer)CloseHandle(m_timer);}
    double Progress() const { LARGE_INTEGER n;QueryPerformanceCounter(&n);return Clamp01((double)(n.QuadPart-m_start.QuadPart)/(double)m_freq.QuadPart/((double)m_duration/1000.0)); }
    void WaitNext(){
        m_nextTick += m_interval;
        LARGE_INTEGER now;QueryPerformanceCounter(&now);
        double elapsed=(double)(now.QuadPart-m_start.QuadPart)/(double)m_freq.QuadPart;
        double waitSec=m_nextTick-elapsed;
        if(waitSec<=0.0){ if(waitSec<-m_interval*0.75){m_nextTick=elapsed;} SwitchToThread(); return; }
        if(m_timer && waitSec>0.0002){ LARGE_INTEGER due; double units=waitSec*10000000.0; LONGLONG q=(LONGLONG)(units<1.0?1.0:units); due.QuadPart=-q; SetWaitableTimer(m_timer,&due,0,nullptr,nullptr,FALSE); WaitForSingleObject(m_timer,100); }
        else if(waitSec>0.00005){ Sleep(0); }
    }
    int FPS()const{return m_fps;}
private:
    LARGE_INTEGER m_freq{},m_start{};HANDLE m_timer=nullptr;int m_duration=450,m_fps=60;double m_interval=1.0/60.0,m_nextTick=0.0;
};

struct Vertex { float x,y,u,v; };
struct ShaderConstants { float progress,width,height,targetX,targetY; uint32_t animation; float padding0,padding1; };

static const char* g_shader = R"(
cbuffer SceneBuffer : register(b0)
{
    float progress;
    float width;
    float height;
    float targetX;
    float targetY;
    uint animation;
    float2 padding;
};

struct VSInput { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct PSInput { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput o;
    o.pos = float4(input.pos, 0.0, 1.0);
    o.uv = input.uv;
    return o;
}

float hash21(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(p.x * p.y);
}

float noise(float2 p)
{
    float2 i = floor(p), f = frac(p);
    f = f*f*(3.0-2.0*f);
    float a = hash21(i);
    float b = hash21(i + float2(1,0));
    float c = hash21(i + float2(0,1));
    float d = hash21(i + float2(1,1));
    return lerp(lerp(a,b,f.x), lerp(c,d,f.x), f.y);
}

float fbm(float2 p)
{
    float v = 0.0, a = 0.5;
    float2x2 rot = float2x2(0.8,0.6,-0.6,0.8);
    [unroll] for(int i=0;i<4;i++) {
        v += a * noise(p);
        p = mul(rot,p) * 2.02;
        a *= 0.5;
    }
    return v;
}

float4 SampleContent(float2 uv)
{
    return g_texture.Sample(g_sampler, saturate(uv));
}

float4 SampleScaled(float2 uv, float scale, float fade, float twist)
{
    float2 c=float2(0.5,0.5);
    float2 q=uv-c;
    float ca=cos(twist), sa=sin(twist);
    q=float2(q.x*ca-q.y*sa,q.x*sa+q.y*ca);
    float2 src=c+q/max(scale,0.001);
    if(src.x<0 || src.x>1 || src.y<0 || src.y>1) return float4(0,0,0,0);
    float4 col=SampleContent(src);
    col.a*=saturate(fade);
    return col;
}

float4 MinimizePS(float2 uv)
{
    float p = saturate(progress);
    float scale;

    // 1. Початковий "підстриб" (+8% масштабу)
    if (p < 0.15)
    {
        float phase1 = p / 0.15;
        scale = 1.0 + 0.08 * sin(phase1 * 1.5707963);
    }
    // 2. Зменшення до центру
    else
    {
        float phase2 = (p - 0.15) / 0.85;
        scale = lerp(1.04, 0.01, pow(phase2, 1.2));
    }

    float fade = 1.0 - p;
    return SampleScaled(uv, scale, fade, 0.0);
}

float4 RestorePS(float2 uv)
{
    float p = saturate(progress);
    float scale;

    // 1. Збільшення з центру від 0.01 до 1.0
    if (p < 0.8)
    {
        float phase1 = p / 0.8;
        scale = lerp(0.01, 1.0, pow(phase1, 0.83));
    }
    // 2. Легкий овершут (+8% масштабу) і повернення
    else
    {
        float phase2 = (p - 0.8) / 0.2;
        scale = 1.0 + 0.08 * sin(phase2 * 3.14159265);
    }

    float fade = p;
    return SampleScaled(uv, scale, fade, 0.0);
}

float4 OpenPS(float2 uv)
{
    float p = saturate(progress);

    // Дуже виразний старт:
    // 0.62 -> 1.00 -> 1.06 -> 1.00
    float scale;

    if (p < 0.72)
    {
        float t = p / 0.72;
        t = smoothstep(0.0, 1.0, t);

        scale = lerp(0.62, 1.0, t);
    }
    else
    {
        float t = (p - 0.72) / 0.28;

        // Яскравий overshoot
        scale = lerp(
            1.0,
            1.06,
            sin(t * 3.14159265)
        );
    }

    // Сильніше відрізняємо OPEN від Windows.
    float fade = smoothstep(0.0, 0.28, p);

    // Легке "стискання" по вертикалі на самому старті.
    float verticalScale =
        lerp(
            0.82,
            1.0,
            smoothstep(0.0, 0.65, p)
        );

    float2 centered = uv - float2(0.5, 0.5);

    centered.y /= max(verticalScale, 0.01);

    float2 transformedUV =
        centered + float2(0.5, 0.5);

    float4 col =
        SampleScaled(
            transformedUV,
            scale,
            fade,
            0.0
        );

    // Невеликий світловий імпульс на старті.
    float pulse =
        sin(p * 3.14159265) *
        (1.0 - smoothstep(0.0, 0.35, p));

    col.rgb += float3(0.10, 0.16, 0.28) * pulse;

    return col;
}

float4 ClosePS(float2 uv)
{
    float p = saturate(progress);

    // Основне стискання:
    // 1.0 -> 0.78
    float scale =
        lerp(
            1.0,
            0.78,
            smoothstep(0.0, 0.82, p)
        );

    // Невеликий "snap" перед зникненням.
    if (p > 0.82)
    {
        float t = (p - 0.82) / 0.18;

        scale *=
            lerp(
                1.0,
                0.90,
                sin(t * 3.14159265)
            );
    }

    // Нелінійне зникнення.
    float fade =
        1.0 -
        smoothstep(
            0.18,
            1.0,
            pow(p, 0.72)
        );

    // Трохи сильніше стискаємо вертикаль.
    float vertical =
        lerp(
            1.0,
            0.72,
            smoothstep(0.0, 1.0, p)
        );

    float2 centered =
        uv - float2(0.5, 0.5);

    centered.y /= max(vertical, 0.01);

    float2 transformedUV =
        centered + float2(0.5, 0.5);

    float4 col =
        SampleScaled(
            transformedUV,
            scale,
            fade,
            0.0
        );

    // Короткий світловий "удар" перед зникненням.
    float flash =
        smoothstep(0.72, 0.88, p) *
        (1.0 - smoothstep(0.88, 1.0, p));

    col.rgb +=
        float3(0.30, 0.12, 0.55) *
        flash;

    return col;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 uv=saturate(input.uv);
    float4 outColor;
    if(animation==0) outColor=MinimizePS(uv);
    else if(animation==1) outColor=RestorePS(uv);
    else if(animation==2) outColor=OpenPS(uv);
    else if(animation==3) outColor=ClosePS(uv);
    else outColor=SampleContent(uv);
    outColor.rgb*=outColor.a;
    return outColor;
}
)";


/*(int tiles,std::vector<Vertex>& v,std::vector<uint32_t>& idx){
    tiles=ClampInt(tiles,TILE_MIN,TILE_MAX);v.clear();idx.clear();v.reserve((tiles+1)*(tiles+1));idx.reserve(tiles*tiles*6);
    for(int y=0;y<=tiles;y++)for(int x=0;x<=tiles;x++){float u=(float)x/tiles,f=(float)y/tiles;v.push_back({u*2.0f-1.0f,1.0f-f*2.0f,u,f});}
    for(int y=0;y<tiles;y++)for(int x=0;x<tiles;x++){uint32_t i0=(uint32_t)(y*(tiles+1)+x),i1=i0+1,i2=i0+tiles+1,i3=i2+1;idx.insert(idx.end(),{i0,i1,i2,i2,i1,i3});}
}*/

static ATOM g_ghostClass=0;
static LRESULT CALLBACK GhostWndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    if(msg==WM_ERASEBKGND) return 1;
    if(msg==WM_NCHITTEST) return HTTRANSPARENT;
    if(msg==WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(hwnd,msg,wp,lp);
}
static bool RegisterGhostClass(){
    GenieTraceCurrent(6,"RegisterGhostClass","ENTER","existingAtom=%u",(unsigned)g_ghostClass);
    if(g_ghostClass){ GenieTraceCurrent(6,"RegisterGhostClass","READY","already registered"); return true; }
    WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.lpfnWndProc=GhostWndProc;wc.hInstance=GetModuleHandleW(nullptr);wc.hbrBackground=(HBRUSH)GetStockObject(NULL_BRUSH);wc.lpszClassName=L"WH_GenieGhost_D3D11";g_ghostClass=RegisterClassExW(&wc);if(!g_ghostClass&&GetLastError()==ERROR_CLASS_ALREADY_EXISTS)g_ghostClass=1;return g_ghostClass!=0;}

// Keep every visual attribute that this mod can temporarily change. In
// particular, restoring the original extended style is what lets Notepad
// resume its own Mica/transparency path after an open animation.
struct SavedWindowVisualState {
    LONG_PTR exStyle = 0;
    COLORREF layeredColorKey = 0;
    BYTE layeredAlpha = 255;
    DWORD layeredFlags = 0;
    BOOL dwmTransitionsDisabled = FALSE;
    bool hasLayeredAttributes = false;
    bool hasDwmTransitionsState = false;
};

static SavedWindowVisualState CaptureWindowVisualState(HWND hwnd)
{
    SavedWindowVisualState state{};
    state.exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (state.exStyle & WS_EX_LAYERED) {
        state.hasLayeredAttributes =
            GetLayeredWindowAttributes(
                hwnd,
                &state.layeredColorKey,
                &state.layeredAlpha,
                &state.layeredFlags) != FALSE;
    }

    state.hasDwmTransitionsState = SUCCEEDED(
        DwmGetWindowAttribute(
            hwnd,
            DWMWA_TRANSITIONS_FORCEDISABLED,
            &state.dwmTransitionsDisabled,
            sizeof(state.dwmTransitionsDisabled)));

    return state;
}

static void RestoreWindowVisualState(
    HWND hwnd,
    const SavedWindowVisualState& state,
    bool uncloak)
{
    if (!IsWindow(hwnd))
        return;

    if (uncloak)
        SetCloak(hwnd, false);

    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) != state.exStyle)
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, state.exStyle);

    if (state.hasLayeredAttributes) {
        SetLayeredWindowAttributes(
            hwnd,
            state.layeredColorKey,
            state.layeredAlpha,
            state.layeredFlags);
    }

    if (state.hasDwmTransitionsState) {
        DwmSetWindowAttribute(
            hwnd,
            DWMWA_TRANSITIONS_FORCEDISABLED,
            &state.dwmTransitionsDisabled,
            sizeof(state.dwmTransitionsDisabled));
    } else {
        // This attribute is supported on the Windows versions targeted by the
        // mod. If it cannot be read, do not leave native transitions disabled.
        SetDwmTransitions(hwnd, true);
    }
}

struct RenderData {
    HWND hwnd=nullptr;
    Snapshot snapshot;
    RECT rect{};
    int width=0,height=0;
    int targetX=0,targetY=0; // screen coordinates used ONLY to move the ghost HWND
    AnimationType animation=ANIM_MINIMIZE;
    int duration=450;
    uint64_t generation=0;
    bool cloakReal=false;
    SavedWindowVisualState visualState;
    bool closeAfter=false;
    bool moveToTaskbar=false;
    HANDLE firstFrame=nullptr;
    uint64_t traceId=0;
    HANDLE frameLatencyWaitable=nullptr;
    bool animationSlotOwned=false;
};

static bool CreateDCompPresentation(
    HWND ghost,
    ID3D11Device* device,
    int width,
    int height,
    IDXGISwapChain1** outSwap,
    ID3D11Texture2D** outBackBuffer,
    ID3D11RenderTargetView** outRTV,
    IDCompositionDevice** outDComp,
    IDCompositionTarget** outTarget,
    IDCompositionVisual** outVisual)
{
    GenieTraceCurrent(7,"CreateDCompPresentation","ENTER",
                      "ghost=%p size=%dx%d device=%p",(void*)ghost,width,height,(void*)device);

    if (!ghost || !device || width <= 0 || height <= 0 ||
        !outSwap || !outBackBuffer || !outRTV ||
        !outDComp || !outTarget || !outVisual) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL","invalid arguments");
        return false;
    }

    *outSwap=nullptr;
    *outBackBuffer=nullptr;
    *outRTV=nullptr;
    *outDComp=nullptr;
    *outTarget=nullptr;
    *outVisual=nullptr;

    IDXGIDevice* dxgiDevice=nullptr;
    IDXGIAdapter* adapter=nullptr;
    IDXGIFactory2* factory=nullptr;
    HRESULT hr=S_OK;

    hr=device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "QueryInterface IDXGIDevice hr=0x%08X",(unsigned)hr);
        return false;
    }

    hr=dxgiDevice->GetAdapter(&adapter);
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "GetAdapter hr=0x%08X",(unsigned)hr);
        dxgiDevice->Release();
        return false;
    }

    hr=adapter->GetParent(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "GetParent IDXGIFactory2 hr=0x%08X",(unsigned)hr);
        adapter->Release();
        dxgiDevice->Release();
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 sd{};
    sd.Width=(UINT)width;
    sd.Height=(UINT)height;
    sd.Format=DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.Stereo=FALSE;
    sd.SampleDesc.Count=1;
    sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount=2;
    sd.Scaling=DXGI_SCALING_STRETCH;
    sd.SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode=DXGI_ALPHA_MODE_PREMULTIPLIED;
    sd.Flags=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    auto cleanup=[&]() {
        if (*outVisual) { (*outVisual)->Release(); *outVisual=nullptr; }
        if (*outTarget) { (*outTarget)->Release(); *outTarget=nullptr; }
        if (*outDComp) { (*outDComp)->Release(); *outDComp=nullptr; }
        if (*outRTV) { (*outRTV)->Release(); *outRTV=nullptr; }
        if (*outBackBuffer) { (*outBackBuffer)->Release(); *outBackBuffer=nullptr; }
        if (*outSwap) { (*outSwap)->Release(); *outSwap=nullptr; }
    };

    hr=factory->CreateSwapChainForComposition(device,&sd,nullptr,outSwap);
    if (FAILED(hr)) {
        // Some drivers reject the frame-latency flag for composition swap chains.
        // Retry without it; the animation remains fully functional, just without
        // the optional one-frame queueing optimization.
        sd.Flags = 0;
        hr=factory->CreateSwapChainForComposition(device,&sd,nullptr,outSwap);
    }
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "CreateSwapChainForComposition hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release();
        adapter->Release();
        dxgiDevice->Release();
        return false;
    }

    // Keep only one frame queued for each animation. This reduces GPU latency and
    // prevents several concurrent ghosts from building a deep Present queue.
    {
        IDXGISwapChain2* swap2=nullptr;
        if (SUCCEEDED((*outSwap)->QueryInterface(IID_PPV_ARGS(&swap2))) && swap2) {
            swap2->SetMaximumFrameLatency(1);
            swap2->Release();
        }
    }

    hr=(*outSwap)->GetBuffer(0,IID_PPV_ARGS(outBackBuffer));
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "GetBuffer hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=device->CreateRenderTargetView(*outBackBuffer,nullptr,outRTV);
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "CreateRenderTargetView hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=DCompositionCreateDevice(dxgiDevice,__uuidof(IDCompositionDevice),
                                reinterpret_cast<void**>(outDComp));
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "DCompositionCreateDevice hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=(*outDComp)->CreateTargetForHwnd(ghost,TRUE,outTarget);
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "CreateTargetForHwnd hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=(*outDComp)->CreateVisual(outVisual);
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "CreateVisual hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=(*outVisual)->SetContent(*outSwap);
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "SetContent hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=(*outTarget)->SetRoot(*outVisual);
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "SetRoot hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    hr=(*outDComp)->Commit();
    if (FAILED(hr)) {
        GenieTraceCurrent(7,"CreateDCompPresentation","FAIL",
                          "initial Commit hr=0x%08X",(unsigned)hr);
        cleanup();
        factory->Release(); adapter->Release(); dxgiDevice->Release();
        return false;
    }

    factory->Release();
    adapter->Release();
    dxgiDevice->Release();
    GenieTraceCurrent(7,"CreateDCompPresentation","SUCCESS",
                      "swapchain=%p target=%p visual=%p",
                      (void*)*outSwap,(void*)*outTarget,(void*)*outVisual);
    return true;
}

static bool CreateDevice(ID3D11Device** dev,ID3D11DeviceContext** ctx)
{
    GenieTraceCurrent(7,"CreateDevice","ENTER","creating D3D11 hardware device");
    if(!dev || !ctx) { GenieTraceCurrent(7,"CreateDevice","FAIL","null output pointers"); return false; }
    *dev=nullptr;
    *ctx=nullptr;

    UINT flags=D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL levels[]={
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL fl{};

    HRESULT hr=D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        dev,&fl,ctx);

    if(FAILED(hr) && hr==E_INVALIDARG){
        D3D_FEATURE_LEVEL fallbackLevels[]={
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };
        hr=D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            fallbackLevels,
            ARRAYSIZE(fallbackLevels),
            D3D11_SDK_VERSION,
            dev,&fl,ctx);
    }

    GenieTraceCurrent(7,"CreateDevice",SUCCEEDED(hr)?"SUCCESS":"FAIL",
                      "hr=0x%08X device=%p context=%p feature=0x%X",
                      (unsigned)hr,(void*)*dev,(void*)*ctx,(unsigned)fl);

    return SUCCEEDED(hr);
}

static bool CompileShaderRaw(
    const char* src,const char* entry,const char* target,ID3DBlob** blob)
{
    GenieTraceCurrent(7,"CompileShader","ENTER","entry=%s target=%s",entry?entry:"?",target?target:"?");
    if(!src || !entry || !target || !blob) { GenieTraceCurrent(7,"CompileShader","FAIL","invalid arguments"); return false; }
    *blob=nullptr;

    UINT flags=D3DCOMPILE_OPTIMIZATION_LEVEL3;
    ID3DBlob* err=nullptr;

    HRESULT hr=D3DCompile(
        src,strlen(src),
        "GenieD3D11Shader",
        nullptr,nullptr,
        entry,target,
        flags,0,
        blob,&err);

    if(FAILED(hr)){
        if(err){
            const char* text=(const char*)err->GetBufferPointer();
            GenieTraceCurrent(7,"CompileShader","FAIL","D3DCompile entry=%s target=%s: %.*s",
                     entry,target,
                     (int)std::min<size_t>(err->GetBufferSize(),1800),
                     text ? text : "");
        }else{
            GenieTraceCurrent(7,"CompileShader","FAIL","D3DCompile entry=%s target=%s hr=0x%08X",
                     entry,target,(unsigned)hr);
        }
    }

    if(err) err->Release();
    if(SUCCEEDED(hr)) GenieTraceCurrent(7,"CompileShader","SUCCESS","entry=%s target=%s",entry,target);
    return SUCCEEDED(hr);
}


static bool CompileShader(const char* src,const char* entry,const char* target,ID3DBlob** blob)
{
    if(!src || !entry || !target || !blob) return false;
    *blob=nullptr;
    std::lock_guard<std::mutex> lock(g_shaderCacheMutex);
    ID3DBlob** cache = nullptr;
    if(strcmp(entry,"VSMain")==0) cache=&g_cachedVertexShaderBlob;
    else if(strcmp(entry,"PSMain")==0) cache=&g_cachedPixelShaderBlob;
    if(cache && *cache){ (*cache)->AddRef(); *blob=*cache; GenieTraceCurrent(7,"CompileShader","CACHE_HIT","entry=%s",entry); return true; }
    ID3DBlob* compiled=nullptr;
    if(!CompileShaderRaw(src,entry,target,&compiled)) return false;
    if(cache){ *cache=compiled; (*cache)->AddRef(); }
    *blob=compiled;
    GenieTraceCurrent(7,"CompileShader","CACHE_STORE","entry=%s",entry);
    return true;
}

static DWORD WINAPI AnimationThread(LPVOID parameter)
{
    RenderData* d = (RenderData*)parameter;
    if (!d) return 0;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    HWND hwnd = d->hwnd;
    const uint64_t generation = d->generation;
    const uint64_t traceId = d->traceId;
    GenieTraceAdopt(traceId, d->animation);
    const ULONGLONG traceStart = GetTickCount64();
    unsigned long long frameCount = 0;
    //double lastRawProgress = 0.0;
    bool traceSuccess = false;
    int traceFailStage = 6;
    const char* traceFailReason = "worker initialization failed";

    HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool comInitialized = (comHr == S_OK || comHr == S_FALSE);

    HWND ghost = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11Texture2D* sourceTexture = nullptr;
    ID3D11ShaderResourceView* sourceView = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11Buffer* constantBuffer = nullptr;
    ID3D11SamplerState* sampler = nullptr;

    IDXGISwapChain1* swapChain = nullptr;
    ID3D11Texture2D* backBuffer = nullptr;
    ID3D11RenderTargetView* renderTarget = nullptr;
    IDCompositionDevice* dcompDevice = nullptr;
    IDCompositionTarget* dcompTarget = nullptr;
    IDCompositionVisual* dcompVisual = nullptr;

    bool firstShown = false;
    bool ok = true;

    // 1. Створення Ghost-вікна як оверлей-полотна для D3D11 рендеру
    if (!RegisterGhostClass()) {
        traceFailStage = 6; traceFailReason = "RegisterGhostClass failed";
        ok = false;
    }

    if (ok) {
        ghost = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT | WS_EX_NOREDIRECTIONBITMAP,
            L"WH_GenieGhost_D3D11", L"", WS_POPUP,
            d->rect.left, d->rect.top, d->width, d->height,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

        if (!ghost) {
            traceFailStage = 6; traceFailReason = "CreateWindowExW ghost failed";
            ok = false;
        }
    }

    if (ok) ok = CreateDevice(&device, &context);
    if (ok) ok = CompileShader(g_shader, "VSMain", "vs_5_0", &vsBlob);
    if (ok) ok = CompileShader(g_shader, "PSMain", "ps_5_0", &psBlob);

    if (!ok) goto cleanup;

    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader))) {
        traceFailStage = 7; traceFailReason = "CreateVertexShader failed"; ok = false;
    }

    if (ok && FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader))) {
        traceFailStage = 7; traceFailReason = "CreatePixelShader failed"; ok = false;
    }

    if (!ok) goto cleanup;

    {
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
        if (FAILED(device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout))) {
            traceFailStage = 7; traceFailReason = "CreateInputLayout failed"; ok = false;
        }
    }

    if (!ok) goto cleanup;

    {
        D3D11_TEXTURE2D_DESC td{};
        td.Width = (UINT)d->width;
        td.Height = (UINT)d->height;
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_IMMUTABLE;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA init{};
        init.pSysMem = d->snapshot.bits;
        init.SysMemPitch = (UINT)d->width * 4;

        if (FAILED(device->CreateTexture2D(&td, &init, &sourceTexture))) {
            traceFailStage = 7; traceFailReason = "CreateTexture2D source failed"; ok = false;
        }
        if (ok && FAILED(device->CreateShaderResourceView(sourceTexture, nullptr, &sourceView)))
            ok = false;
    }

    if (!ok) goto cleanup;

    {
        const Vertex vertices[4] = {
            {-1.0f,  1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f, 1.0f, 0.0f},
            {-1.0f, -1.0f, 0.0f, 1.0f},
            { 1.0f, -1.0f, 1.0f, 1.0f}
        };
        const uint32_t indices[6] = {0, 1, 2, 2, 1, 3};

        D3D11_BUFFER_DESC bd{};
        D3D11_SUBRESOURCE_DATA init{};

        bd.ByteWidth = sizeof(vertices);
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        init.pSysMem = vertices;
        if (FAILED(device->CreateBuffer(&bd, &init, &vertexBuffer))) { ok = false; }

        if (ok) {
            bd = {};
            bd.ByteWidth = sizeof(indices);
            bd.Usage = D3D11_USAGE_IMMUTABLE;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            init = {};
            init.pSysMem = indices;
            if (FAILED(device->CreateBuffer(&bd, &init, &indexBuffer))) { ok = false; }
        }
    }

    if (!ok) goto cleanup;

    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(ShaderConstants);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(device->CreateBuffer(&bd, nullptr, &constantBuffer))) { ok = false; }

        if (ok) {
            D3D11_SAMPLER_DESC sd{};
            sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            sd.MaxLOD = D3D11_FLOAT32_MAX;
            if (FAILED(device->CreateSamplerState(&sd, &sampler))) { ok = false; }
        }
    }

    if (!ok) goto cleanup;

    // 2. Ініціалізація DirectComposition та SwapChain для Ghost-вікна
    if (ghost) {
        if (!CreateDCompPresentation(ghost, device, d->width, d->height,
                                     &swapChain, &backBuffer, &renderTarget,
                                     &dcompDevice, &dcompTarget, &dcompVisual)) {
            traceFailStage = 8; traceFailReason = "CreateDCompPresentation failed";
            ok = false;
        }
    }

    if (!ok) goto cleanup;

    if (swapChain) {
        IDXGISwapChain2* swap2=nullptr;
        if (SUCCEEDED(swapChain->QueryInterface(IID_PPV_ARGS(&swap2))) && swap2) {
            swap2->SetMaximumFrameLatency(1);
            HANDLE hLatency = swap2->GetFrameLatencyWaitableObject();
            if (hLatency) d->frameLatencyWaitable = hLatency;
            swap2->Release();
        }
    }

    {
        D3D11_VIEWPORT viewport{};
        viewport.Width = (FLOAT)d->width;
        viewport.Height = (FLOAT)d->height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        context->RSSetViewports(1, &viewport);

        FrameScheduler scheduler(hwnd, d->duration);

        while (!g_unloading.load(std::memory_order_relaxed) &&
               IsAnimationCurrent(hwnd, generation))
        {
            const double rawProgress = scheduler.Progress();
            //lastRawProgress = rawProgress;
            ++frameCount;
            float p = (float)rawProgress;

            if (d->animation == ANIM_MINIMIZE)
                p = EaseInOutCubic(p);
            else if (d->animation == ANIM_RESTORE || d->animation == ANIM_OPEN)
                p = EaseOutCubic(p);
            else
                p = EaseInCubic(p);

            if (d->moveToTaskbar) {
                const float originalCenterX =
                    (float)d->rect.left + (float)d->width * 0.5f;
                const float originalCenterY =
                    (float)d->rect.top + (float)d->height * 0.5f;

                float centerX = originalCenterX;
                float centerY = originalCenterY;

                if (d->animation == ANIM_MINIMIZE) {
                    centerX = originalCenterX +
                        ((float)d->targetX - originalCenterX) * p;
                    centerY = originalCenterY +
                        ((float)d->targetY - originalCenterY) * p;
                } else if (d->animation == ANIM_RESTORE) {
                    centerX = (float)d->targetX +
                        (originalCenterX - (float)d->targetX) * p;
                    centerY = (float)d->targetY +
                        (originalCenterY - (float)d->targetY) * p;
                }

                SetWindowPos(
                    ghost, HWND_TOPMOST,
                    (int)lroundf(centerX - (float)d->width * 0.5f),
                    (int)lroundf(centerY - (float)d->height * 0.5f),
                    d->width, d->height,
                    SWP_NOACTIVATE | SWP_NOOWNERZORDER);
            } else {
                // Local/center animation: never move the ghost toward the taskbar.
                SetWindowPos(
                    ghost, HWND_TOPMOST,
                    d->rect.left, d->rect.top,
                    d->width, d->height,
                    SWP_NOACTIVATE | SWP_NOOWNERZORDER);
            }

            if (d->frameLatencyWaitable) {
                WaitForSingleObject(d->frameLatencyWaitable, 100);
            }

            ShaderConstants constants{};
            constants.progress = p;
            constants.width = (float)d->width;
            constants.height = (float)d->height;
            constants.targetX = 0.5f * (float)d->width;
            constants.targetY = 0.5f * (float)d->height;
            constants.animation = (uint32_t)d->animation;

            context->UpdateSubresource(constantBuffer, 0, nullptr, &constants, 0, 0);

            if (renderTarget) {
                FLOAT clear[4] = {0, 0, 0, 0};
                context->OMSetRenderTargets(1, &renderTarget, nullptr);
                context->ClearRenderTargetView(renderTarget, clear);

                UINT stride = sizeof(Vertex), offset = 0;
                context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
                context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                context->IASetInputLayout(inputLayout);
                context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                context->VSSetShader(vertexShader, nullptr, 0);
                context->VSSetConstantBuffers(0, 1, &constantBuffer);
                context->PSSetShader(pixelShader, nullptr, 0);
                context->PSSetConstantBuffers(0, 1, &constantBuffer);
                context->PSSetShaderResources(0, 1, &sourceView);
                context->PSSetSamplers(0, 1, &sampler);

                context->DrawIndexed(6, 0, 0);

                if (swapChain) swapChain->Present(0, 0);
                if (dcompDevice) dcompDevice->Commit();
            }

            if (!firstShown) {
                // The first GPU frame is ready. Hide the real surface only at
                // the handoff point, then show the ghost; this avoids both a
                // blank gap before the ghost exists and a native DWM transition.
                if (d->cloakReal && IsWindow(hwnd))
                    SetCloak(hwnd, true);
                if (ghost)
                    ShowWindow(ghost, SW_SHOWNOACTIVATE);
                DwmFlush();
                firstShown = true;

                if (d->firstFrame) {
                    SetEvent(d->firstFrame);
                    CloseHandle(d->firstFrame);
                    d->firstFrame = nullptr;
                }
            }

            if (rawProgress >= 1.0) {
                traceSuccess = true;
                traceFailStage = 10; traceFailReason = "completed";
                break;
            }

            scheduler.WaitNext();
        }
    }

cleanup:
    if (d->firstFrame) {
        SetEvent(d->firstFrame);
        CloseHandle(d->firstFrame);
        d->firstFrame = nullptr;
    }

    if (d->closeAfter) {
        bool closedRequestSent = false;

        if (IsWindow(hwnd) && !g_unloading.load(std::memory_order_relaxed)) {
            SetPropW(hwnd, L"WH_GenieCloseBypass", (HANDLE)1);
            DWORD_PTR ignored = 0;
            LRESULT sendResult =
                SendMessageTimeoutW(
                    hwnd,
                    WM_SYSCOMMAND,
                    SC_CLOSE,
                    0,
                    SMTO_ABORTIFHUNG | SMTO_BLOCK,
                    1000,
                    &ignored
                );

            // Some apps (including modern Notepad/Outlook surfaces) accept SC_CLOSE
            // but destroy the HWND asynchronously. Do not restore the real window
            // immediately after the ghost, or it can flash for a frame before teardown.
            // Poll only on the worker thread; if the app keeps the HWND alive (for
            // example, a save prompt/cancelled close), restore it after the short grace period.
            if (sendResult && IsWindow(hwnd)) {
                const ULONGLONG closeWaitStart = GetTickCount64();
                while (IsWindow(hwnd) &&
                       GetTickCount64() - closeWaitStart < 500 &&
                       !g_unloading.load(std::memory_order_relaxed)) {
                    Sleep(10);
                }
            }

            closedRequestSent = !IsWindow(hwnd);

            GenieTraceCurrent(
                11,
                "AnimationThread",
                closedRequestSent ? "CLOSE_SENT" : "CLOSE_PENDING_OR_CANCELLED",
                "SendMessageTimeout=%lld hwndAlive=%d waitedForAsyncDestroy=500ms",
                (long long)sendResult,
                IsWindow(hwnd) ? 1 : 0
            );
            RemovePropW(hwnd, L"WH_GenieCloseBypass");
        }

        if (IsWindow(hwnd) && !closedRequestSent) {
            RestoreWindowVisualState(hwnd, d->visualState, d->cloakReal);
        }
    } else if (IsWindow(hwnd)) {
        RestoreWindowVisualState(hwnd, d->visualState, d->cloakReal);
    }

    if (dcompTarget) dcompTarget->SetRoot(nullptr);
    if (dcompDevice) dcompDevice->Commit();

    if (dcompVisual) dcompVisual->Release();
    if (dcompTarget) dcompTarget->Release();
    if (dcompDevice) dcompDevice->Release();
    if (renderTarget) renderTarget->Release();
    if (backBuffer) backBuffer->Release();
    if (swapChain) swapChain->Release();

    if (sampler) sampler->Release();
    if (constantBuffer) constantBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexBuffer) vertexBuffer->Release();
    if (sourceView) sourceView->Release();
    if (sourceTexture) sourceTexture->Release();
    if (inputLayout) inputLayout->Release();
    if (pixelShader) pixelShader->Release();
    if (vertexShader) vertexShader->Release();
    if (psBlob) psBlob->Release();
    if (vsBlob) vsBlob->Release();
    if (context) context->Release();
    if (device) device->Release();

    if (ghost) DestroyWindow(ghost);

    // Copy ownership-dependent values BEFORE destroying AnimationData.
    // Never access d after delete d.
    const bool animationSlotOwned = d->animationSlotOwned;
    const HANDLE frameLatencyWaitable = d->frameLatencyWaitable;

    // Keep the last valid window snapshot in cache. In particular, the
    // minimize snapshot is the source frame for the next restore animation.
    // Cache entries are invalidated on size mismatch and HWND destruction.
    DeleteSnapshot(d->snapshot);

    GenieTraceDone(
        traceId,
        d->animation,
        traceSuccess,
        traceSuccess ? 10 : traceFailStage,
        traceSuccess ? "completed" : traceFailReason,
        frameCount,
        (unsigned long long)(GetTickCount64() - traceStart)
    );

    EndAnimation(hwnd, generation);

    // d is no longer valid after this point.
    delete d;
    d = nullptr;

    if (frameLatencyWaitable)
        CloseHandle(frameLatencyWaitable);

    if (animationSlotOwned)
        ReleaseAnimationSlot();

    if (comInitialized)
        CoUninitialize();

    g_workers.fetch_sub(1, std::memory_order_release);
    GenieTraceClear();

    return 0;
}

static bool TryAcquireAnimationSlot(HWND hwnd, AnimationType type)
{
    int current=g_activeAnimations.load(std::memory_order_relaxed);
    while(current<MAX_CONCURRENT_ANIMATIONS){
        if(g_activeAnimations.compare_exchange_weak(current,current+1,std::memory_order_acq_rel)){
            GenieTraceCurrent(5,"AnimationScheduler","SLOT_ACQUIRED","active=%d/%d",current+1,MAX_CONCURRENT_ANIMATIONS);
            return true;
        }
    }
    GenieTraceCurrent(5,"AnimationScheduler","SLOT_FULL","active=%d/%d type=%d",g_activeAnimations.load(),MAX_CONCURRENT_ANIMATIONS,(int)type);
    return false;
}
static void ReleaseAnimationSlot(){ g_activeAnimations.fetch_sub(1,std::memory_order_acq_rel); }

static int DurationFor(AnimationType t){if(t==ANIM_RESTORE)return g_restoreDuration.load();if(t==ANIM_OPEN)return g_openDuration.load();if(t==ANIM_CLOSE)return g_closeDuration.load();return g_minDuration.load();}
static bool StartAnimation(HWND hwnd, AnimationType type,
                           const Snapshot* supplied = nullptr,
                           bool cloakReal = false,
                           bool closeAfter = false,
                           bool waitFirst = false,
                           const SavedWindowVisualState* originalVisual = nullptr)
{
    if (g_traceId == 0) GenieTraceBegin(hwnd, type);
    else g_traceType = type;

    GenieTraceCurrent(5, "StartAnimation", "ENTER", "hwnd=%p type=%d", (void*)hwnd, (int)type);
    if (g_unloading.load(std::memory_order_relaxed)) { GenieTraceCurrent(5, "StartAnimation", "REJECT", "module unloading"); return false; }
    if (IsExcluded(hwnd) || !IsSafeAnimationWindow(hwnd,type)) { GenieTraceCurrent(5, "StartAnimation", "REJECT", "unsafe/non-main window"); return false; }
    if (WebEngineWarmupActive(hwnd,type)) return false;
    if (FirstAnimationSafeMode(hwnd,type)) return false;

    // Keep the existing foreground/background protection. It prevents
    // lifecycle calls for minimized inactive windows from being mistaken for
    // user-triggered animations, while top-level PWA windows remain eligible
    // when they are actually the active target.
    HWND fgHwnd = GetForegroundWindow();
    if (type != ANIM_OPEN && fgHwnd != nullptr && fgHwnd != hwnd &&
        GetAncestor(fgHwnd, GA_ROOT) != hwnd) {
        if (IsIconic(hwnd) && type != ANIM_RESTORE) {
            GenieTraceCurrent(
                5, "StartAnimation", "REJECT",
                "window is minimized/inactive for this animation");
            return false;
        }
    }

    if (type == ANIM_MINIMIZE && !g_minimizeEnabled.load()) { GenieTraceCurrent(5, "StartAnimation", "REJECT", "MINIMIZE disabled"); return false; }
    if (type == ANIM_RESTORE && !g_restoreEnabled.load()) { GenieTraceCurrent(5, "StartAnimation", "REJECT", "RESTORE disabled"); return false; }
    if (type == ANIM_OPEN && !g_openEnabled.load()) { GenieTraceCurrent(5, "StartAnimation", "REJECT", "OPEN disabled"); return false; }
    if (type == ANIM_CLOSE && !g_closeEnabled.load()) { GenieTraceCurrent(5, "StartAnimation", "REJECT", "CLOSE disabled"); return false; }

    uint64_t generation = 0;
    if (!TryBeginAnimation(hwnd, type, &generation)) {
        GenieTraceCurrent(5, "StartAnimation", "REJECT",
                          "duplicate animation request hwnd=%p type=%d",
                          (void*)hwnd, (int)type);
        return false;
    }

    auto abandonReservation = [&]() {
        EndAnimation(hwnd, generation);
    };
    bool slotOwned = false;
    auto releaseSlot = [&]() { if (slotOwned) { ReleaseAnimationSlot(); slotOwned=false; } };

    if (!TryAcquireAnimationSlot(hwnd, type)) {
        abandonReservation();
        return false;
    }
    slotOwned = true;

    RECT ext{};
    if (!GetAccurateWindowRect(hwnd, &ext)) {
        GenieTraceCurrent(5, "StartAnimation", "FAIL", "GetAccurateWindowRect hwnd=%p", (void*)hwnd);
        releaseSlot();
        abandonReservation();
        return false;
    }

    int width = ext.right - ext.left;
    int height = ext.bottom - ext.top;
    GenieTraceCurrent(5, "StartAnimation", "WINDOW", "rect=%ld,%ld-%ld,%ld size=%dx%d", ext.left, ext.top, ext.right, ext.bottom, width, height);
    if (width < 16 || height < 16) {
        GenieTraceCurrent(5, "StartAnimation", "REJECT", "size too small %dx%d", width, height);
        releaseSlot();
        abandonReservation();
        return false;
    }

    Snapshot snapshot{};
    bool haveSnapshot = false;
    if (supplied && supplied->bits)
    {
        if (supplied->width != width || supplied->height != height) {
            GenieTraceCurrent(3, "StartAnimation", "FAIL",
                              "supplied snapshot mismatch supplied=%dx%d expected=%dx%d",
                              supplied->width, supplied->height, width, height);
            releaseSlot();
            abandonReservation();
            return false;
        }
        if (!CreateSnapshotBitmap(snapshot, width, height)) {
            GenieTraceCurrent(3, "StartAnimation", "FAIL",
                              "CreateSnapshotBitmap for supplied snapshot");
            releaseSlot();
            abandonReservation();
            return false;
        }
        memcpy(snapshot.bits, supplied->bits, (size_t)width * height * 4);
        haveSnapshot = true;
    }

    if (!haveSnapshot) {
        haveSnapshot = TakeCachedSnapshot(hwnd, width, height, snapshot);
    }

    if (!haveSnapshot) {
        DeleteSnapshot(snapshot);
        RECT capturedRect{};
        bool preferScreen = (type == ANIM_MINIMIZE || type == ANIM_CLOSE || IsWindowVisible(hwnd));

        if (CaptureSnapshotAsync(hwnd, snapshot, capturedRect, preferScreen)) {
            ext = capturedRect;
            width = ext.right - ext.left;
            height = ext.bottom - ext.top;
            haveSnapshot = true;
        }
    }

    if (!ValidateSnapshot(snapshot))
    {
        GenieTraceCurrent(3, "StartAnimation", "FAIL", "ValidateSnapshot hwnd=%p", (void*)hwnd);
        DeleteSnapshot(snapshot);
        releaseSlot();
        abandonReservation();
        return false;
    }
    GenieTraceCurrent(3, "StartAnimation", "SUCCESS", "snapshot size=%dx%d bits=%p bitmap=%p", snapshot.width, snapshot.height, snapshot.bits, (void*)snapshot.bitmap);

    MonitorInfoEx monitor = GetMonitorForWindow(hwnd);
    const bool moveToTaskbar = AnimationUsesTaskbarTarget(type);

    int targetX = ext.left + width / 2;
    int targetY = ext.top + height / 2;

    if (moveToTaskbar) {
        int fallbackX =
            monitor.monitorRect.left +
            (monitor.monitorRect.right - monitor.monitorRect.left) / 2;

        targetX = GetTaskbarButtonX(hwnd, fallbackX, monitor.monitor);
        POINT taskbarTarget = GetTaskbarTargetPoint(monitor.monitor, targetX);
        targetX = taskbarTarget.x;
        targetY = taskbarTarget.y;
    }

    GenieTraceCurrent(
        4, "StartAnimation", "TARGET",
        "generation=%llu taskbar=%d target=%d,%d rect=%ld,%ld-%ld,%ld",
        (unsigned long long)generation, (int)moveToTaskbar,
        targetX, targetY, ext.left, ext.top, ext.right, ext.bottom);

    const SavedWindowVisualState visualState =
        originalVisual ? *originalVisual : CaptureWindowVisualState(hwnd);

    RenderData* data = new (std::nothrow) RenderData();
    if (!data) {
        DeleteSnapshot(snapshot);
        releaseSlot();
        abandonReservation();
        GenieTraceCurrent(5, "StartAnimation", "FAIL", "RenderData allocation failed");
        return false;
    }
    data->hwnd = hwnd;
    data->snapshot = std::move(snapshot);
    data->rect = ext;
    data->width = width;
    data->height = height;
    data->targetX = targetX;
    data->targetY = targetY;
    data->animation = type;
    data->duration = DurationFor(type);
    data->generation = generation;
    data->cloakReal = cloakReal;
    data->visualState = visualState;
    data->closeAfter = closeAfter;
    data->moveToTaskbar = moveToTaskbar;
    data->traceId = g_traceId;
    data->animationSlotOwned = true;
    GenieTraceCurrent(5, "StartAnimation", "STATE", "generation=%llu worker data prepared trace=%llu",
                      (unsigned long long)generation, (unsigned long long)data->traceId);

    // Capture is complete. Only now change the real window, so a failed
    // restore request cannot leave Explorer or Notepad cloaked.
    // Suppress native DWM transitions before starting the worker, but defer
    // cloaking until the ghost has rendered its first frame. Cloaking here
    // creates a blank interval while D3D11/DirectComposition are initialized.
    SetDwmTransitions(hwnd, false);

    HANDLE waitEvent = nullptr;
    if (waitFirst)
    {
        waitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!waitEvent)
        {
            GenieTraceCurrent(6, "StartAnimation", "FAIL", "CreateEvent error=%lu", (unsigned long)GetLastError());
            DeleteSnapshot(data->snapshot);
            RestoreWindowVisualState(hwnd, data->visualState, data->cloakReal);
            EndAnimation(hwnd, generation);
            releaseSlot();
            delete data;
            return false;
        }
        if (!DuplicateHandle(GetCurrentProcess(), waitEvent,
                             GetCurrentProcess(), &data->firstFrame,
                             0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            GenieTraceCurrent(6, "StartAnimation", "FAIL", "DuplicateHandle error=%lu", (unsigned long)GetLastError());
            CloseHandle(waitEvent);
            DeleteSnapshot(data->snapshot);
            RestoreWindowVisualState(hwnd, data->visualState, data->cloakReal);
            EndAnimation(hwnd, generation);
            releaseSlot();
            delete data;
            return false;
        }
    }

    g_workers.fetch_add(1, std::memory_order_relaxed);
    HANDLE thread = CreateThread(nullptr, 0, AnimationThread, data, 0, nullptr);
    GenieTraceCurrent(6, "StartAnimation", thread ? "SUCCESS" : "FAIL", "CreateThread=%s handle=%p", (thread ? "OK" : "FAILED"), (void*)thread);
    if (!thread)
    {
        g_workers.fetch_sub(1, std::memory_order_release);
        if (data->firstFrame) CloseHandle(data->firstFrame);
        if (waitEvent) CloseHandle(waitEvent);
        DeleteSnapshot(data->snapshot);
        RestoreWindowVisualState(hwnd, data->visualState, data->cloakReal);
        EndAnimation(hwnd, generation);
        releaseSlot();
        delete data;
        return false;
    }
    CloseHandle(thread);

    if (waitEvent)
    {
        DWORD result = WaitForSingleObject(waitEvent, 500);
        CloseHandle(waitEvent);
        GenieTraceCurrent(8, "StartAnimation", "FIRST_FRAME", "wait result=%lu", (unsigned long)result);
        return true;
    }

    GenieTraceCurrent(6, "StartAnimation", "SUCCESS", "worker launched");
    return true;
}

static bool PrepareMinimize(HWND hwnd)
{
    GenieTraceCurrent(3,"PrepareMinimize","ENTER","hwnd=%p",(void*)hwnd);
    if(!hwnd || !IsWindow(hwnd)) { GenieTraceCurrent(3,"PrepareMinimize","FAIL","invalid hwnd"); return false; }
    if (IsAnimationRunning(hwnd)) {
        GenieTraceCurrent(3, "PrepareMinimize", "REJECT",
                          "animation already reserved/running hwnd=%p", (void*)hwnd);
        return false;
    }
    Snapshot s; RECT r{};
    if(!CaptureSnapshotAsync(hwnd,s,r,true)) {
        GenieTraceCurrent(3,"PrepareMinimize","FAIL","CaptureWindow");
        return false;
    }
    GenieTraceCurrent(3,"PrepareMinimize","CAPTURED","size=%dx%d rect=%ld,%ld-%ld,%ld bits=%p bitmap=%p",
                      s.width,s.height,r.left,r.top,r.right,r.bottom,s.bits,(void*)s.bitmap);
    CacheSnapshot(hwnd,std::move(s));
    GenieTraceCurrent(3,"PrepareMinimize","CACHED","snapshot copied to minimize cache");
    DeleteSnapshot(s);
    GenieTraceCurrent(3,"PrepareMinimize","SUCCESS","snapshot ownership released");
    return true;
}

struct LaunchData {
    HWND hwnd;
    SavedWindowVisualState visualState;
    uint64_t traceId;
};

static DWORD WINAPI LaunchThread(LPVOID p)
{
    LaunchData* ld = (LaunchData*)p;
    if (!ld)
        return 0;

    const HWND hwnd = ld->hwnd;
    const SavedWindowVisualState originalVisual = ld->visualState;
    const uint64_t traceId = ld->traceId;
    delete ld;

    GenieTraceAdopt(traceId, ANIM_OPEN);
    GenieTraceCurrent(6, "LaunchThread", "ENTER", "hwnd=%p", (void*)hwnd);

    bool animated = false;
    // The native show occurs with DWM transitions disabled, but without making
    // the window transparent. Capture its actual composed pixels; the real HWND
    // is cloaked only after the ghost has a rendered first frame.
    for (int attempt = 0;
         attempt < 6 && !animated && !g_unloading.load(std::memory_order_acquire);
         ++attempt) {
        if (!IsWindow(hwnd) || !IsWindowVisible(hwnd))
            break;

        Snapshot snapshot{};
        RECT captured{};
        if (CaptureSnapshotAsync(hwnd, snapshot, captured, true)) {
            animated = StartAnimation(
                hwnd, ANIM_OPEN, &snapshot, true, false, false,
                &originalVisual);
            DeleteSnapshot(snapshot);
            if (animated)
                break;
        }

        DeleteSnapshot(snapshot);
        if (attempt < 5)
            Sleep(16);
    }

    if (!animated && IsWindow(hwnd))
        RestoreWindowVisualState(hwnd, originalVisual, false);

    GenieTraceCurrent(10, "LaunchThread", animated ? "SUCCESS" : "NATIVE_FALLBACK",
                      "animation=%d", (int)animated);
    if (!animated)
        GenieTraceDone(traceId, ANIM_OPEN, false, 10,
                       "screen capture/renderer unavailable; native window restored");

    g_workers.fetch_sub(1, std::memory_order_release);
    GenieTraceClear();
    return 0;
}

static bool LaunchPrepare(
    HWND hwnd,
    int cmd,
    SavedWindowVisualState* outVisual)
{
    GenieTraceCurrent(2,"LaunchPrepare","ENTER","hwnd=%p cmd=%d",(void*)hwnd,cmd);
    if(!outVisual){ GenieTraceCurrent(2,"LaunchPrepare","FAIL","null outVisual"); return false; }
    if(!g_launchAnimation.load()||!g_openEnabled.load()||g_unloading.load()){
        GenieTraceCurrent(2,"LaunchPrepare","BYPASS","launch animation disabled or unloading"); return false;
    }
    if(cmd!=SW_SHOW&&cmd!=SW_SHOWNORMAL&&cmd!=SW_SHOWDEFAULT&&cmd!=SW_SHOWMAXIMIZED){
        GenieTraceCurrent(2,"LaunchPrepare","BYPASS","unsupported ShowWindow command"); return false;
    }
    if(IsWindowVisible(hwnd)||IsIconic(hwnd)||!IsLaunchWindow(hwnd)||IsExcluded(hwnd)||!IsSafeAnimationWindow(hwnd,ANIM_OPEN)){
        GenieTraceCurrent(2,"LaunchPrepare","BYPASS","window visible/iconic/unusable/excluded/non-main"); return false;
    }
    // Apply the open animation on the first show, including the first 750 ms
    // after process creation. The safe-mode bypass remains for other actions.
    if (FirstAnimationSafeMode(hwnd, ANIM_OPEN)) return false;

    if (IsAnimationRunning(hwnd)) {
        GenieTraceCurrent(2, "LaunchPrepare", "BYPASS", "animation already running");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if(!g_launchSeen.insert(hwnd).second){
            GenieTraceCurrent(2,"LaunchPrepare","BYPASS","launch already seen for hwnd"); return false;
        }
    }

    const SavedWindowVisualState visualState = CaptureWindowVisualState(hwnd);
    *outVisual = visualState;

    // Disable the native transition before the caller shows the HWND. Do not
    // add WS_EX_LAYERED or force alpha=0: toggling alpha back to 255 for a
    // screenshot caused a visible flash and can interfere with app surfaces.
    SetDwmTransitions(hwnd, false);

    GenieTraceCurrent(5,"LaunchPrepare","SUCCESS",
                      "native DWM transition disabled; first-frame cloak deferred; ex=0x%llX",
                      (unsigned long long)visualState.exStyle);
    return true;
}

static void LaunchCommit(HWND hwnd, const SavedWindowVisualState& visualState)
{
    GenieTraceCurrent(6,"LaunchCommit","ENTER","hwnd=%p ex=0x%llX",
                      (void*)hwnd, (unsigned long long)visualState.exStyle);
    LaunchData* ld = new LaunchData{ hwnd, visualState, g_traceId };
    g_workers.fetch_add(1, std::memory_order_relaxed);
    HANDLE thread = CreateThread(nullptr, 0, LaunchThread, ld, 0, nullptr);
    if (thread)
    {
        GenieTraceCurrent(6,"LaunchCommit","SUCCESS","LaunchThread handle=%p",(void*)thread);
        CloseHandle(thread);
        return;
    }

    GenieTraceCurrent(6,"LaunchCommit","FAIL","CreateThread error=%lu",(unsigned long)GetLastError());
    g_workers.fetch_sub(1, std::memory_order_release);
    delete ld;
    if (IsWindow(hwnd))
    {
        RestoreWindowVisualState(hwnd, visualState, false);
    }
}

static bool IsGhostWindow(HWND hwnd)
{
    if(!hwnd) return false;
    WCHAR cls[64]{};
    int n=GetClassNameW(hwnd,cls,(int)ARRAYSIZE(cls));
    return n>0 && wcscmp(cls,L"WH_GenieGhost_D3D11")==0;
}

static bool StartRestoreAfterNativeShow(HWND hwnd, const char* caller)
{
    if (!g_restoreEnabled.load() ||
        !IsWindowUsable(hwnd) ||
        !IsIconic(hwnd)) {

        GenieTraceCurrent(
            2,
            caller,
            "BYPASS",
            "restore candidate not iconic/usable"
        );

        return false;
    }
    if (IsHostProcessProtected(hwnd) || WebEngineWarmupActive(hwnd, ANIM_RESTORE))
        return false;
    if (FirstAnimationSafeMode(hwnd, ANIM_RESTORE)) return false;
    if (IsAnimationRunning(hwnd)) return false;

    Snapshot snapshot{};

    // Restore must use the snapshot made while the window was last visible.
    // Do NOT compare it with the minimized window rectangle.
    // The minimized RECT can differ from the actual restore dimensions.
    if (!TakeCachedRestoreSnapshot(hwnd, snapshot)) {
        GenieTraceCurrent(
            3,
            caller,
            "BYPASS",
            "no cached restore snapshot"
        );
        return false;
    }

    SavedWindowVisualState visual=CaptureWindowVisualState(hwnd);

    // Disable native DWM transitions and cloak BEFORE forwarding restore.
    // Otherwise Windows can present one native restore frame before our ghost
    // animation gets its first frame.
    SetDwmTransitions(hwnd,false);
    if (!PrepareRestoreVisualIsolation(hwnd)) {
        DeleteSnapshot(snapshot);
        SetDwmTransitions(hwnd, true);
        return false;
    }

    BOOL result=g_ShowWindow(hwnd,SW_RESTORE);
    if (!result && !IsWindow(hwnd)) {
        DeleteSnapshot(snapshot);
        RestoreWindowVisualState(hwnd,visual,true);
        return false;
    }

    bool started=StartAnimation(hwnd,ANIM_RESTORE,&snapshot,true,false,true,&visual);
    DeleteSnapshot(snapshot);
    if (!started) {
        if(IsWindow(hwnd)) RestoreWindowVisualState(hwnd,visual,true);
        return false;
    }
    return true;
}

static bool StartRestoreWithPlacement(
    HWND hwnd,
    const WINDOWPLACEMENT* wp,
    const char* caller)
{
    if (!g_restoreEnabled.load() ||
        !IsWindowUsable(hwnd) ||
        !IsIconic(hwnd) ||
        !wp) {

        GenieTraceCurrent(
            2,
            caller,
            "BYPASS",
            "placement restore candidate not iconic/usable"
        );

        return false;
    }

    if (IsHostProcessProtected(hwnd) ||
        WebEngineWarmupActive(hwnd, ANIM_RESTORE)) {
        return false;
    }
    if (FirstAnimationSafeMode(hwnd, ANIM_RESTORE)) return false;

    if (IsAnimationRunning(hwnd))
        return false;

    Snapshot snapshot{};

    if (!TakeCachedRestoreSnapshot(hwnd, snapshot)) {
        GenieTraceCurrent(
            3,
            caller,
            "BYPASS",
            "no cached restore snapshot"
        );
        return false;
    }

    SavedWindowVisualState visual =
        CaptureWindowVisualState(hwnd);

    // Забороняємо native DWM transition ДО зміни placement.
    SetDwmTransitions(hwnd, false);
    if (!PrepareRestoreVisualIsolation(hwnd)) {
        DeleteSnapshot(snapshot);
        SetDwmTransitions(hwnd, true);
        return false;
    }

    BOOL result = g_SetWindowPlacement(hwnd, wp);

    if (!result || !IsWindow(hwnd)) {
        DeleteSnapshot(snapshot);

        if (IsWindow(hwnd))
            RestoreWindowVisualState(hwnd, visual, true);

        return false;
    }

    bool started =
        StartAnimation(
            hwnd,
            ANIM_RESTORE,
            &snapshot,
            true,
            false,
            true,
            &visual
        );

    DeleteSnapshot(snapshot);

    if (!started) {
        if (IsWindow(hwnd))
            RestoreWindowVisualState(hwnd, visual, true);

        return false;
    }

    return true;
}

static BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd)
{
    if (IsGhostWindow(hwnd) || !IsWindow(hwnd) || IsExcluded(hwnd))
        return g_ShowWindow(hwnd, cmd);

    const bool minimizing =
        cmd == SW_MINIMIZE || cmd == SW_FORCEMINIMIZE ||
        cmd == SW_SHOWMINIMIZED || cmd == SW_SHOWMINNOACTIVE;
    const bool restoring =
        cmd == SW_RESTORE || cmd == SW_SHOWNORMAL || cmd == SW_SHOW;
    const AnimationType traceType = minimizing ? ANIM_MINIMIZE : ANIM_RESTORE;

    GenieTraceBegin(hwnd, traceType);
    GenieTraceCurrent(1, "ShowWindow_Hook", "ENTER",
                      "cmd=%d minimize=%d restore=%d iconic=%d visible=%d",
                      cmd, (int)minimizing, (int)restoring,
                      (int)IsIconic(hwnd), (int)IsWindowVisible(hwnd));

    if (g_unloading.load()) {
        GenieTraceBypass(g_traceId, traceType, "ShowWindow_Hook", "module unloading");
        GenieTraceClear();
        return g_ShowWindow(hwnd, cmd);
    }

    if (minimizing) {
        if (g_minimizeEnabled.load() && IsWindowUsable(hwnd) &&
            !FirstAnimationSafeMode(hwnd, ANIM_MINIMIZE) &&
            !IsAnimationRunning(hwnd) && PrepareMinimize(hwnd) &&
            StartAnimation(hwnd, ANIM_MINIMIZE, nullptr, false, false, true)) {
            return g_ShowWindow(hwnd, cmd);
        }

        GenieTraceBypass(g_traceId, ANIM_MINIMIZE, "ShowWindow_Hook",
                         "minimize not intercepted");
        GenieTraceClear();
        return g_ShowWindow(hwnd, cmd);
    }

    if (restoring && IsIconic(hwnd)) {
        if (StartRestoreAfterNativeShow(hwnd, "ShowWindow_Hook")) {
            GenieTraceCurrent(6,"ShowWindow_Hook","INTERCEPTED","restore handoff started before native frame");
            return TRUE;
        }
        GenieTraceBypass(g_traceId, ANIM_RESTORE, "ShowWindow_Hook",
                         "restore native fallback");
        GenieTraceClear();
        return g_ShowWindow(hwnd, cmd);
    }

    SavedWindowVisualState visualState{};
    if (LaunchPrepare(hwnd, cmd, &visualState)) {
        BOOL result = g_ShowWindow(hwnd, cmd);
        LaunchCommit(hwnd, visualState);
        return result;
    }

    GenieTraceBypass(g_traceId, traceType, "ShowWindow_Hook",
                     "not intercepted by animation path");
    GenieTraceClear();
    return g_ShowWindow(hwnd, cmd);
}

static bool StartRestoreAfterAsyncShow(
    HWND hwnd,
    int cmd,
    const char* caller)
{
    if (!g_restoreEnabled.load() ||
        !IsWindowUsable(hwnd) ||
        !IsIconic(hwnd)) {

        GenieTraceCurrent(
            2,
            caller,
            "BYPASS",
            "async restore candidate not iconic/usable"
        );

        return false;
    }

    if (IsHostProcessProtected(hwnd) ||
        WebEngineWarmupActive(hwnd, ANIM_RESTORE)) {
        return false;
    }
    if (FirstAnimationSafeMode(hwnd, ANIM_RESTORE)) return false;

    if (IsAnimationRunning(hwnd))
        return false;

    Snapshot snapshot{};

    if (!TakeCachedRestoreSnapshot(hwnd, snapshot)) {
        GenieTraceCurrent(
            3,
            caller,
            "BYPASS",
            "no cached restore snapshot"
        );
        return false;
    }

    SavedWindowVisualState visual =
        CaptureWindowVisualState(hwnd);

    // Native DWM transition вимикаємо ДО async restore.
    SetDwmTransitions(hwnd, false);
    if (!PrepareRestoreVisualIsolation(hwnd)) {
        DeleteSnapshot(snapshot);
        SetDwmTransitions(hwnd, true);
        return false;
    }

    BOOL result = g_ShowWindowAsync(hwnd, cmd);

    if (!result) {
        DeleteSnapshot(snapshot);

        if (IsWindow(hwnd))
            RestoreWindowVisualState(hwnd, visual, true);

        return false;
    }

    bool started =
        StartAnimation(
            hwnd,
            ANIM_RESTORE,
            &snapshot,
            true,
            false,
            true,
            &visual
        );

    DeleteSnapshot(snapshot);

    if (!started) {
        if (IsWindow(hwnd))
            RestoreWindowVisualState(hwnd, visual, true);

        return false;
    }

    return true;
}

static BOOL WINAPI ShowWindowAsync_Hook(HWND hwnd, int cmd)
{
    if (IsGhostWindow(hwnd) || !IsWindow(hwnd) || IsExcluded(hwnd))
        return g_ShowWindowAsync(hwnd, cmd);

    const bool minimizing =
        cmd == SW_MINIMIZE || cmd == SW_SHOWMINIMIZED || cmd == SW_SHOWMINNOACTIVE;
    const bool restoring = cmd == SW_RESTORE || cmd == SW_SHOWNORMAL;
    const AnimationType traceType = minimizing ? ANIM_MINIMIZE : ANIM_RESTORE;

    GenieTraceBegin(hwnd, traceType);
    GenieTraceCurrent(1, "ShowWindowAsync_Hook", "ENTER",
                      "cmd=%d minimize=%d restore=%d iconic=%d visible=%d",
                      cmd, (int)minimizing, (int)restoring,
                      (int)IsIconic(hwnd), (int)IsWindowVisible(hwnd));

    if (g_unloading.load()) {
        GenieTraceBypass(g_traceId, traceType, "ShowWindowAsync_Hook", "module unloading");
        GenieTraceClear();
        return g_ShowWindowAsync(hwnd, cmd);
    }

    if (minimizing) {
        if (g_minimizeEnabled.load() && IsWindowUsable(hwnd) &&
            !FirstAnimationSafeMode(hwnd, ANIM_MINIMIZE) &&
            !IsAnimationRunning(hwnd) && PrepareMinimize(hwnd) &&
            StartAnimation(hwnd, ANIM_MINIMIZE, nullptr, false, false, true)) {
            return g_ShowWindowAsync(hwnd, cmd);
        }

        GenieTraceBypass(g_traceId, ANIM_MINIMIZE, "ShowWindowAsync_Hook",
                         "minimize not intercepted");
        GenieTraceClear();
        return g_ShowWindowAsync(hwnd, cmd);
    }

    if (restoring && IsIconic(hwnd)) {
        if (StartRestoreAfterAsyncShow(
                hwnd,
                cmd,
                "ShowWindowAsync_Hook")) {

            GenieTraceCurrent(
                6,
                "ShowWindowAsync_Hook",
                "INTERCEPTED",
                "async restore animation started"
            );

            return TRUE;
        }

        GenieTraceBypass(
            g_traceId,
            ANIM_RESTORE,
            "ShowWindowAsync_Hook",
            "async restore animation failed; native fallback"
        );

        GenieTraceClear();

        return g_ShowWindowAsync(hwnd, cmd);
    }

    SavedWindowVisualState visualState{};
    if (LaunchPrepare(hwnd, cmd, &visualState)) {
        BOOL result = g_ShowWindowAsync(hwnd, cmd);
        LaunchCommit(hwnd, visualState);
        return result;
    }

    GenieTraceBypass(g_traceId, traceType, "ShowWindowAsync_Hook", "not intercepted");
    GenieTraceClear();
    return g_ShowWindowAsync(hwnd, cmd);
}

static BOOL WINAPI SetWindowPlacement_Hook(
    HWND hwnd,
    const WINDOWPLACEMENT* wp)
{
    if (IsGhostWindow(hwnd) || !IsWindow(hwnd) || IsExcluded(hwnd) || !wp)
        return g_SetWindowPlacement(hwnd, wp);

    const bool minimizing =
        wp->showCmd == SW_MINIMIZE || wp->showCmd == SW_SHOWMINIMIZED ||
        wp->showCmd == SW_SHOWMINNOACTIVE;
    const bool restoring =
        wp->showCmd == SW_RESTORE || wp->showCmd == SW_SHOWNORMAL;
    const AnimationType traceType = minimizing ? ANIM_MINIMIZE : ANIM_RESTORE;

    GenieTraceBegin(hwnd, traceType);
    GenieTraceCurrent(1, "SetWindowPlacement_Hook", "ENTER",
                      "showCmd=%u minimize=%d restore=%d iconic=%d visible=%d",
                      (unsigned)wp->showCmd, (int)minimizing, (int)restoring,
                      (int)IsIconic(hwnd), (int)IsWindowVisible(hwnd));

    if (g_unloading.load()) {
        GenieTraceBypass(g_traceId, traceType, "SetWindowPlacement_Hook",
                         "module unloading");
        GenieTraceClear();
        return g_SetWindowPlacement(hwnd, wp);
    }

    if (minimizing) {
        if (g_minimizeEnabled.load() && IsWindowUsable(hwnd) &&
            !FirstAnimationSafeMode(hwnd, ANIM_MINIMIZE) &&
            !IsAnimationRunning(hwnd) && PrepareMinimize(hwnd) &&
            StartAnimation(hwnd, ANIM_MINIMIZE, nullptr, false, false, true)) {
            return g_SetWindowPlacement(hwnd, wp);
        }

        GenieTraceBypass(g_traceId, ANIM_MINIMIZE, "SetWindowPlacement_Hook",
                         "minimize not intercepted");
        GenieTraceClear();
        return g_SetWindowPlacement(hwnd, wp);
    }

    if (restoring && IsIconic(hwnd)) {
        if (StartRestoreWithPlacement(
                hwnd,
                wp,
                "SetWindowPlacement_Hook")) {

            GenieTraceCurrent(
                6,
                "SetWindowPlacement_Hook",
                "INTERCEPTED",
                "restore animation started through SetWindowPlacement"
            );

            return TRUE;
        }
        GenieTraceBypass(g_traceId, ANIM_RESTORE, "SetWindowPlacement_Hook",
                         "restore native fallback");
        GenieTraceClear();
        return g_SetWindowPlacement(hwnd, wp);
    }

    GenieTraceBypass(g_traceId, traceType, "SetWindowPlacement_Hook",
                     "not intercepted by animation path");
    GenieTraceClear();
    return g_SetWindowPlacement(hwnd, wp);
}

static BOOL WINAPI CloseWindow_Hook(HWND hwnd)
{
    if (IsGhostWindow(hwnd) || !IsWindow(hwnd) || IsExcluded(hwnd))
        return g_CloseWindow(hwnd);

    GenieTraceBegin(hwnd, ANIM_MINIMIZE);
    GenieTraceCurrent(1, "CloseWindow_Hook", "ENTER", "hwnd=%p", (void*)hwnd);

    if (g_minimizeEnabled.load() && IsWindowUsable(hwnd) &&
        !FirstAnimationSafeMode(hwnd, ANIM_MINIMIZE) &&
        !IsAnimationRunning(hwnd) && PrepareMinimize(hwnd) &&
        StartAnimation(hwnd, ANIM_MINIMIZE, nullptr, false, false, true)) {
        return g_CloseWindow(hwnd);
    }

    GenieTraceBypass(g_traceId, ANIM_MINIMIZE, "CloseWindow_Hook",
                     "not intercepted by animation path");
    GenieTraceClear();
    return g_CloseWindow(hwnd);
}

static BOOL WINAPI SetWindowPos_Hook(
    HWND hwnd,
    HWND hwndInsertAfter,
    int x,
    int y,
    int cx,
    int cy,
    UINT flags)
{
    if (IsGhostWindow(hwnd)) {
        return g_SetWindowPos(hwnd, hwndInsertAfter, x, y, cx, cy, flags);
    }

    // Виключено запуск restore при системному позиціонуванні SetWindowPos.
    return g_SetWindowPos(hwnd, hwndInsertAfter, x, y, cx, cy, flags);
}

static LRESULT WINAPI DefWindowProcW_Hook(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (IsGhostWindow(hwnd)) return g_DefWindowProcW(hwnd, msg, wp, lp);
    if (g_unloading.load()) return g_DefWindowProcW(hwnd, msg, wp, lp);

    // Фільтр захисту: блокуємо обробку анімацій для сторонніх неосновних/фон-вікони
    if (!IsWindow(hwnd) || IsExcluded(hwnd)) {
        return g_DefWindowProcW(hwnd, msg, wp, lp);
    }

    if (msg == WM_SYSCOMMAND) {
        UINT cmd = (UINT)wp & 0xFFF0u;
        if (cmd == SC_MINIMIZE || cmd == SC_RESTORE || cmd == SC_MAXIMIZE || cmd == SC_CLOSE) {
            GenieTraceBegin(hwnd, cmd == SC_MINIMIZE ? ANIM_MINIMIZE : (cmd == SC_CLOSE ? ANIM_CLOSE : ANIM_RESTORE));
            GenieTraceCurrent(1, "DefWindowProcW_Hook", "ENTER",
                              "WM_SYSCOMMAND raw=0x%llX cmd=0x%08X SC_MINIMIZE=0x%08X",
                              (unsigned long long)wp, (unsigned)cmd, (unsigned)SC_MINIMIZE);
        } else {
            GenieLog("DefWindowProcW: WM_SYSCOMMAND hwnd=%p raw=0x%llX cmd=0x%08X (not animation command)",
                     (void*)hwnd, (unsigned long long)wp, (unsigned)cmd);
        }
    } else if (msg == WM_CLOSE) {
        if (GetPropW(hwnd, L"WH_GenieCloseBypass")) {
            GenieTraceCurrent(11, "DefWindowProcW_Hook", "BYPASS", "WM_CLOSE forwarded to original procedure");
            return g_DefWindowProcW(hwnd, msg, wp, lp);
        }
        GenieTraceBegin(hwnd, ANIM_CLOSE);
        GenieTraceCurrent(1, "DefWindowProcW_Hook", "ENTER", "WM_CLOSE");
    } else if (msg == WM_DESTROY) {
        GenieTraceCurrent(11, "DefWindowProcW_Hook", "DESTROY", "WM_DESTROY cleanup for hwnd=%p", (void*)hwnd);
    }

    if (msg == WM_DESTROY) {
        // Invalidate the HWND snapshot before taking the state lock; the helper
        // owns g_mutex itself and keeps the cache byte accounting consistent.
        InvalidateSnapshot(hwnd);
        std::lock_guard<std::mutex> lock(g_mutex);
        g_states.erase(hwnd);
        g_iconPositions.erase(hwnd);
        g_launchSeen.erase(hwnd);
        g_animActive.erase(hwnd);
        GenieTraceCurrent(11, "DefWindowProcW_Hook", "STATE", "per-window animation state erased");
    }

    bool closeMessage = (msg == WM_CLOSE);
    bool systemClose = (msg == WM_SYSCOMMAND && (((UINT)wp & 0xFFF0u) == SC_CLOSE));

    // The animation worker replays the original system-close command after
    // the ghost finishes. Bypass only our close-animation interception so the
    // application's normal DefWindowProc(SC_CLOSE) -> WM_CLOSE path is kept.
    if (systemClose && GetPropW(hwnd, L"WH_GenieCloseBypass")) {
        GenieTraceCurrent(11, "DefWindowProcW_Hook", "BYPASS",
                          "animated SC_CLOSE forwarded to original procedure");
        return g_DefWindowProcW(hwnd, msg, wp, lp);
    }

    if (closeMessage || systemClose) {
        // Перевіряємо, чи це дійсно активне вікно, яке закриває користувач
        HWND activeWnd = GetForegroundWindow();
        bool isTargetOrChild = (activeWnd == hwnd || GetAncestor(activeWnd, GA_ROOT) == hwnd);

        GenieTraceCurrent(2, "DefWindowProcW_Hook", "CHECK", "close path enabled=%d systemClose=%d",
                          (int)g_closeEnabled.load(), (int)systemClose);

        if (g_closeEnabled.load() && IsWindowUsable(hwnd) && isTargetOrChild &&
            !FirstAnimationSafeMode(hwnd, ANIM_CLOSE) &&
            !IsAnimationRunning(hwnd)) {
            Snapshot s; RECT r{};
            GenieTraceCurrent(3, "DefWindowProcW_Hook", "CAPTURE", "capturing close snapshot");
            if (CaptureSnapshotAsync(hwnd, s, r, true)) {
                GenieTraceCurrent(3, "DefWindowProcW_Hook", "CAPTURED", "snapshot=%p size=%dx%d",
                                  s.bits, s.width, s.height);
                GenieTraceCurrent(
                    5,
                    "DefWindowProcW_Hook",
                    "STATE",
                    "starting CLOSE ghost; real window remains=%d visible=%d",
                    (int)IsWindow(hwnd),
                    (int)IsWindowVisible(hwnd)
                );
                if (StartAnimation(hwnd, ANIM_CLOSE, &s, true, true, false)) {
                    DeleteSnapshot(s);
                    GenieTraceCurrent(5, "DefWindowProcW_Hook", "INTERCEPTED", "close animation owns the window close");
                    return 0;
                }
                DeleteSnapshot(s);
                GenieTraceCurrent(5, "DefWindowProcW_Hook", "FALLBACK", "close animation could not start");
            } else {
                GenieTraceCurrent(3, "DefWindowProcW_Hook", "FAIL", "close snapshot capture failed");
            }
        } else {
            GenieTraceCurrent(2, "DefWindowProcW_Hook", "BYPASS", "close disabled/unusable/excluded/background window");
        }
    }

    if (msg == WM_SYSCOMMAND) {
        UINT cmd = (UINT)wp & 0xFFF0u;
        if (cmd == SC_MINIMIZE) {
            GenieTraceCurrent(2, "DefWindowProcW_Hook", "CHECK",
                              "minimize enabled=%d bypass=%d usable=%d excluded=%d",
                              (int)g_minimizeEnabled.load(),
                              GetPropW(hwnd, L"WH_GenieMinimizeBypass") ? 1 : 0,
                              (int)IsWindowUsable(hwnd), (int)IsExcluded(hwnd));
            if (!GetPropW(hwnd, L"WH_GenieMinimizeBypass") &&
                g_minimizeEnabled.load() && IsWindowUsable(hwnd) &&
                !FirstAnimationSafeMode(hwnd, ANIM_MINIMIZE) &&
                !IsAnimationRunning(hwnd)) {
                if (PrepareMinimize(hwnd)) {
                    if (StartAnimation(hwnd, ANIM_MINIMIZE, nullptr, false, false, true)) {
                        SetPropW(hwnd, L"WH_GenieMinimizeBypass", (HANDLE)1);
                        GenieTraceCurrent(6, "DefWindowProcW_Hook", "FORWARD", "calling original WM_SYSCOMMAND with bypass");
                        LRESULT r = g_DefWindowProcW(hwnd, msg, wp, lp);
                        RemovePropW(hwnd, L"WH_GenieMinimizeBypass");
                        GenieTraceCurrent(6, "DefWindowProcW_Hook", "RETURN", "original minimize result=%lld", (long long)r);
                        return r;
                    }
                    GenieTraceCurrent(5, "DefWindowProcW_Hook", "FALLBACK", "minimize animation could not start");
                } else {
                    GenieTraceCurrent(3, "DefWindowProcW_Hook", "FAIL", "PrepareMinimize failed");
                }
            }
        }
        if (cmd == SC_RESTORE || cmd == SC_MAXIMIZE) {
            const bool wasIconic = IsIconic(hwnd);
            GenieTraceCurrent(2, "DefWindowProcW_Hook", "CHECK",
                              "restore cmd=%u iconic=%d enabled=%d",
                              (unsigned)cmd, (int)wasIconic,
                              (int)g_restoreEnabled.load());

            if (wasIconic && g_restoreEnabled.load() && IsWindowUsable(hwnd)) {
                if (StartRestoreAfterNativeShow(hwnd, "DefWindowProcW_Hook")) {
                    GenieTraceCurrent(6, "DefWindowProcW_Hook", "INTERCEPTED",
                                      "restore handoff started before native frame");
                    return 0;
                }

                GenieTraceBypass(g_traceId, ANIM_RESTORE,
                                 "DefWindowProcW_Hook",
                                 "restore native fallback");
                GenieTraceClear();
                return g_DefWindowProcW(hwnd, msg, wp, lp);
            }
        }
    }

    LRESULT r = g_DefWindowProcW(hwnd, msg, wp, lp);
    if (msg == WM_SYSCOMMAND && g_traceId != 0) {
        GenieTraceBypass(g_traceId, g_traceType, "DefWindowProcW_Hook", "command reached original procedure");
        GenieTraceClear();
    }
    return r;
}

static void LoadSettings()
{
    GenieLog("LoadSettings: begin");

    // ------------------------------------------------------------
    // Read raw settings
    // ------------------------------------------------------------

    const int minDuration =
        ClampInt(
            Wh_GetIntSetting(L"minimize_duration_ms_v2"),
            80,
            2000
        );

    const int restoreDuration =
        ClampInt(
            Wh_GetIntSetting(L"restore_duration_ms_v2"),
            80,
            2000
        );

    const int openDuration =
        ClampInt(
            Wh_GetIntSetting(L"open_duration_ms_v2"),
            80,
            2000
        );

    const int closeDuration =
        ClampInt(
            Wh_GetIntSetting(L"close_duration_ms_v2"),
            80,
            2000
        );

    const int fpsSetting =
        Wh_GetIntSetting(L"frame_rate");

    const int minimizeSetting =
        Wh_GetIntSetting(L"minimize_animation_v2");

    const int restoreSetting =
        Wh_GetIntSetting(L"restore_animation_v2");

    const int openSetting =
        Wh_GetIntSetting(L"open_animation_v2");

    const int closeSetting =
        Wh_GetIntSetting(L"close_animation_v2");

    const int multiMonitorSetting =
        Wh_GetIntSetting(L"multi_monitor");

    const int meshQualitySetting =
        Wh_GetIntSetting(L"mesh_quality");

    const int unhideTaskbarSetting =
        Wh_GetIntSetting(L"unhide_taskbar");

    const int taskbarAnimationSetting =
        Wh_GetIntSetting(L"taskbar_animation_v2");

    const int testSetting =
    Wh_GetIntSetting(L"test_animation_enabled");

    GenieLog(
        "LoadSettings: TEST test_animation_enabled=%d",
        testSetting
    );


    // ------------------------------------------------------------
    // Log RAW values exactly as returned by Windhawk
    // ------------------------------------------------------------

    GenieLog(
        "LoadSettings: RAW "
        "minimize_animation_v2=%d "
        "restore_animation_v2=%d "
        "open_animation_v2=%d "
        "close_animation_v2=%d",
        minimizeSetting,
        restoreSetting,
        openSetting,
        closeSetting
    );

    GenieLog(
        "LoadSettings: RAW "
        "frame_rate=%d "
        "multi_monitor=%d "
        "mesh_quality=%d "
        "unhide_taskbar=%d",
        fpsSetting,
        multiMonitorSetting,
        meshQualitySetting,
        unhideTaskbarSetting
    );


    // ------------------------------------------------------------
    // Store duration / FPS settings
    // ------------------------------------------------------------

    g_minDuration.store(minDuration);
    g_restoreDuration.store(restoreDuration);
    g_openDuration.store(openDuration);
    g_closeDuration.store(closeDuration);

    g_frameRate.store(
        fpsSetting == 0
            ? 0
            : ClampInt(fpsSetting, FPS_MIN, FPS_MAX)
    );


    // ------------------------------------------------------------
    // Store animation enable states
    // ------------------------------------------------------------

    const bool minimizeEnabled = (minimizeSetting != 0);
    const bool restoreEnabled  = (restoreSetting != 0);
    const bool openEnabled     = (openSetting != 0);
    const bool closeEnabled    = (closeSetting != 0);

    g_minimizeEnabled.store(minimizeEnabled);
    g_restoreEnabled.store(restoreEnabled);
    g_openEnabled.store(openEnabled);

    // Launch animation currently uses the same setting as
    // open animation.
    g_launchAnimation.store(openEnabled);

    g_closeEnabled.store(closeEnabled);


    // ------------------------------------------------------------
    // Log EFFECTIVE values after storing them
    // ------------------------------------------------------------

    GenieLog(
        "LoadSettings: EFFECTIVE "
        "minimize=%d "
        "restore=%d "
        "open=%d "
        "launch=%d "
        "close=%d",
        (int)g_minimizeEnabled.load(),
        (int)g_restoreEnabled.load(),
        (int)g_openEnabled.load(),
        (int)g_launchAnimation.load(),
        (int)g_closeEnabled.load()
    );


    // ------------------------------------------------------------
    // Other settings
    // ------------------------------------------------------------

    g_multiMonitor.store(
        multiMonitorSetting != 0
    );

    g_meshQuality.store(
        ClampInt(
            meshQualitySetting,
            0,
            3
        )
    );

    g_unhideTaskbar.store(
        unhideTaskbarSetting != 0
    );

    g_taskbarAnimation.store(
        taskbarAnimationSetting != 0
    );


    GenieLog(
        "LoadSettings: EFFECTIVE "
        "multiMonitor=%d "
        "meshQuality=%d "
        "unhideTaskbar=%d "
        "taskbarAnimation=%d "
        "fps=%d "
        "durations[min=%d restore=%d open=%d close=%d]",
        (int)g_multiMonitor.load(),
        (int)g_meshQuality.load(),
        (int)g_unhideTaskbar.load(),
        (int)g_taskbarAnimation.load(),
        (int)g_frameRate.load(),
        (int)g_minDuration.load(),
        (int)g_restoreDuration.load(),
        (int)g_openDuration.load(),
        (int)g_closeDuration.load()
    );


    // ------------------------------------------------------------
    // Excluded programs
    // ------------------------------------------------------------

    {
        std::lock_guard<std::mutex> lock(g_mutex);

        g_excluded.clear();

        PCWSTR rawExcluded =
            Wh_GetStringSetting(L"excluded_programs");

        if (rawExcluded)
        {
            std::wstring cur;

            for (PCWSTR p = rawExcluded; *p; ++p)
            {
                if (*p == L'\r' || *p == L'\n')
                {
                    if (!cur.empty())
                    {
                        std::transform(
                            cur.begin(),
                            cur.end(),
                            cur.begin(),
                            ::towlower
                        );

                        g_excluded.push_back(cur);
                        cur.clear();
                    }
                }
                else
                {
                    cur.push_back(*p);
                }
            }

            if (!cur.empty())
            {
                std::transform(
                    cur.begin(),
                    cur.end(),
                    cur.begin(),
                    ::towlower
                );

                g_excluded.push_back(cur);
            }

            Wh_FreeStringSetting(rawExcluded);
        }

        GenieLog(
            "LoadSettings: excluded programs=%zu",
            g_excluded.size()
        );
    }

    GenieLog("LoadSettings: complete");
}


static bool HasVisibleTopLevelWindowForCurrentProcess()
{
    struct Probe { bool found=false; };
    auto cb=[](HWND hwnd, LPARAM lp)WINAPI->BOOL {
        Probe* p=reinterpret_cast<Probe*>(lp);
        DWORD pid=0; GetWindowThreadProcessId(hwnd,&pid);
        if(pid==GetCurrentProcessId() && IsWindowVisible(hwnd) &&
           GetAncestor(hwnd,GA_ROOT)==hwnd &&
           !(GetWindowLongPtrW(hwnd,GWL_EXSTYLE)&WS_EX_TOOLWINDOW)) {
            p->found=true; return FALSE;
        }
        return TRUE;
    };
    Probe p{};
    EnumWindows(reinterpret_cast<WNDENUMPROC>(+cb), reinterpret_cast<LPARAM>(&p));
    return p.found;
}

static DWORD WINAPI RendererWarmupThread(LPVOID)
{
    Sleep(1200);
    if (!g_unloading.load(std::memory_order_acquire) && HasVisibleTopLevelWindowForCurrentProcess()) {
        ID3DBlob* vs=nullptr; ID3DBlob* ps=nullptr;
        CompileShader(g_shader,"VSMain","vs_5_0",&vs);
        CompileShader(g_shader,"PSMain","ps_5_0",&ps);
        if(vs) vs->Release(); if(ps) ps->Release();
        GenieLog("RendererWarmupThread: shader pipelines warmed in background");
    }
    g_workers.fetch_sub(1,std::memory_order_release);
    return 0;
}

BOOL Wh_ModInit() {
    GenieLog("Wh_ModInit: initializing module");
    if (IsNonInteractiveSessionProcess() || IsNonWindowHostingHelperProcess()) {
        GenieLog("Wh_ModInit: helper/non-interactive process bypass");
        return FALSE;
    }
    LoadSettings();

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&g_DefWindowProcW);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&g_ShowWindow);
    Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook, (void**)&g_ShowWindowAsync);
    Wh_SetFunctionHook((void*)SetWindowPlacement, (void*)SetWindowPlacement_Hook, (void**)&g_SetWindowPlacement);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&g_SetWindowPos);
    Wh_SetFunctionHook((void*)CloseWindow, (void*)CloseWindow_Hook, (void**)&g_CloseWindow);

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (g_unloading.load(std::memory_order_acquire) ||
        IsNonInteractiveSessionProcess() || IsNonWindowHostingHelperProcess() ||
        !HasVisibleTopLevelWindowForCurrentProcess()) return;
    g_workers.fetch_add(1, std::memory_order_relaxed);
    HANDLE h=CreateThread(nullptr,0,RendererWarmupThread,nullptr,0,nullptr);
    if(h) CloseHandle(h);
    else g_workers.fetch_sub(1,std::memory_order_release);
}

void Wh_ModSettingsChanged() {
    GenieLog("Wh_ModSettingsChanged: reloading settings");
    LoadSettings();
}

void Wh_ModUninit() {
    GenieLog("Wh_ModUninit: unloading module");
    g_unloading.store(true);
    while (g_workers.load(std::memory_order_acquire) > 0) {
        Sleep(10);
    }
    {
        std::lock_guard<std::mutex> lock(g_shaderCacheMutex);
        if (g_cachedVertexShaderBlob) { g_cachedVertexShaderBlob->Release(); g_cachedVertexShaderBlob=nullptr; }
        if (g_cachedPixelShaderBlob) { g_cachedPixelShaderBlob->Release(); g_cachedPixelShaderBlob=nullptr; }
    }
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (auto& kv : g_snapshots) DeleteSnapshot(kv.second);
        g_snapshots.clear(); g_snapshotLastUsed.clear(); g_snapshotBytes=0;
        g_firstAnimationSafeMode.clear(); g_taskbarLookupInFlight.clear(); g_processImageCache.clear();
    }
}
