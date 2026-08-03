// ==WindhawkMod==
// @id              windows-animations
// @name            Windows Animations
// @description     Smooth minimize, restore, close, switch animations for windows.
// @version         1.1.5
// @author          ReDrag
// @github          https://github.com/redrag2105
// @include         *
// @exclude         TextInputHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         dwm.exe
// @license         MIT
// @compilerOptions -ladvapi32 -ldwmapi -lgdi32 -lole32 -loleaut32 -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Animations

Welcome to **Windows Animations**, a comprehensive window transition suite for your desktop. Built from the ground up to deliver cinematic window animations. 

By utilizing a smart Hybrid Rendering Engine, this mod bridges the gap between stunning visual aesthetics and seamless execution.

## ✨ Key Features

* **🚀 Smart Hybrid Engine:** Intelligently seamlessly switches between lightning-fast GDI (for pixel-perfect destruction physics) and the native DWM Thumbnail API (to perfectly preserve Windows 11 rounded corners and drop shadows).

* **🎬 Cinematic Close Effects:** Transform how you close applications with four physics-based animations:
  * **Square Shatter:** The window violently explodes outward into digital blocks before drifting into the void.
    
    ![Square Shatter Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/a7e46c466c7b88552d5d92cad113b652fbd3f10e/shatter_close.gif)

  * **Thanos Snap:** A disintegration wave sweeps across the window, turning it into thousands of tiny dust particles that curve away into the wind.
    
    ![Thanos Snap Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/0e0508083d6c3108b2b3da8c5f0140f01cd21e37/close_preview.gif)

  * **Perlin Dissolve:** The window organically melts and dissolves into thin air using a smooth Perlin noise map.
    
    ![Perlin Dissolve Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/a7e46c466c7b88552d5d92cad113b652fbd3f10e/perlin_close.gif)

  * **Cyber Glitch:** The window fractures into irregular horizontal data strips that shudder, then tear left and right like a corrupted hologram.

    ![Cyber Glitch Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ff3a17e818f2d08e43ee4f79059b4ccb3c663cb0/glitchy_close.gif)

* **🧞 Fluid Minimize & Restore:** Choose between the classic Genie "suck into the taskbar" effect (derived from macos-minimize-animation by Abdullah Masood under MIT license) and **Ink Splash** — an organic blot reveal/retract.
  
  ![Genie Minimize Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/a7e46c466c7b88552d5d92cad113b652fbd3f10e/genie_preview.gif)
  
  ![Ink Splash Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ff3a17e818f2d08e43ee4f79059b4ccb3c663cb0/ink_splash.gif)

* **🔄 Soft Switch Animation:** A pristine scale and fade-in animation triggered *exclusively* when actively switching windows via the Alt+Tab menu.
  * **Before:**
    
    ![Alt Tab Switch Before](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ba4e9efd647c954eb619ec2181d5435a80af7b15/switch_before.gif)
  * **After:**
    
    ![Alt Tab Switch After](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ba4e9efd647c954eb619ec2181d5435a80af7b15/switch_after.gif)
* **🛡️ Rock-Solid Stability:** Features lifecycle management for System Tray apps (like Discord, Steam, etc.)—handling background apps gracefully to minimize ghosting or stuck transparency. Tray apps only *hide* their window instead of closing it, so animating them is opt-in via the **Treat hiding a window as closing it** setting.

## ⚙️ Customization & Settings

You can deeply customize the feel and pacing of every animation via the Windhawk Settings tab:

* **Minimize/Restore Animation Style:** Choose between Genie (taskbar suck) and Ink Splash (organic blot reveal/retract).
* **Close Animation Effect:** Dropdown menu to switch between 'Square Shatter', 'Thanos Snap', 'Perlin Dissolve', and 'Cyber Glitch'.
* **Minimize/Restore duration (ms):** Controls Genie speed directly; Ink Splash is paced slower from the same value. (Default: 360ms, clamp 200–1400)
* **Close animation duration (ms):** Controls how long the dramatic close animation lasts. (Default: 900ms)
* **Switch animation duration (ms):** Controls the snappy speed of the Alt+Tab scaling effect. (Default: 200ms)
* **Shatter block size (px):** Determines the size of the dust/shatter particles. 
  * *Performance Tip:* Smaller values (e.g., 1, 2) create hyper-realistic pixel dust but require more CPU power. Larger values (24, 32) yield a stylish retro pixelated shatter and perform effortlessly on any hardware. (Clamped strictly to 1-100).
* **Treat hiding a window as closing it:** Off by default. Enable it to get close animations for system tray apps that hide their window instead of closing it—at the cost of also animating splash screens and other windows an app hides on its own.
* **Toggles:** Individually turn on/off Restore, Close, Alt+Tab Switch, and Launch animations to suit your workflow.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minimize_animation: true
  $name: Animate window minimize
  $description: Play the minimize animation when a window is minimized to the taskbar.
- restore_animation: true
  $name: Animate window restore
  $description: Play the restore animation when a window is restored from the taskbar.
- min_restore_effect_style: genie
  $name: Minimize/Restore Animation Style
  $description: Choose how windows minimize and restore.
  $options:
  - genie: Genie (Taskbar Suck)
  - ink_splash: Ink Splash (Organic Blot)
- duration_ms: 360
  $name: Minimize/Restore duration (ms)
  $description: Controls the speed of Genie (as-is) and Ink Splash (internally paced slower). Clamped between 200 and 1400.
- close_animation: true
  $name: Animate window close
  $description: Play the shatter/disintegration animation when closing an application.
- close_effect_style: thanos
  $name: Close Animation Style
  $description: Choose the cinematic effect used when closing a window.
  $options:
  - shatter: Square Shatter (Explosion)
  - thanos: Thanos Snap (Disintegration Wave)
  - perlin: Perlin Dissolve (Acid Burn)
  - glitch: Cyber Glitch (Digital Teleport)
- close_duration_ms: 900
  $name: Close animation duration (ms)
  $description: How long the shatter/disintegration close animation lasts. Clamped to 50-5000.
- hide_as_close: false
  $name: Treat hiding a window as closing it
  $description: >-
    Plays the close animation when an app hides its window instead of really destroying it.

    Many apps that live in the system tray - Discord, Steam and Telegram, for example - do not
    actually close when you press the X. They only hide the window and keep running in the
    background, so by default those apps disappear with no animation at all. Turn this on to
    animate them too.

    The catch is that Windows gives no way to tell "the user closed this window" apart from
    "the app hid this window for its own reasons", so enabling this also animates things you
    did not close: splash screens that vanish once an app has finished loading, windows an app
    hides while rearranging its own UI, and explicit "minimise to tray" commands. If you start
    seeing the shatter or disintegration effect at moments when you did not close anything,
    turn this back off.

    Windows that are genuinely closed and destroyed are animated either way - this setting only
    affects the hide-based cases. It has no effect unless "Animate window close" is also on.
- shatter_block_size: 12
  $name: Dust/Shatter block size (px)
  $description: >- 
    Base size of the disintegrated dust. 1 = true pixel dust (Heavy CPU). Clamped to 1-100. WARNING: Setting this to 1-4 on a 4K display may allocate a significant amount of RAM per close animation.
- switch_animation: true
  $name: Animate window switch (Alt+Tab strictly)
  $description: Play a scale and fade-in animation ONLY when switching to a window via Alt+Tab.
- switch_duration_ms: 200
  $name: Switch animation duration (ms)
  $description: How long the switching animation lasts. Clamped to 50-1000.
- launch_animation: false
  $name: Animate app launch
  $description: Play the restore animation when an application window first opens.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <cstdint>
#include <cstring>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <uiautomation.h>
#include <sddl.h>
#include <string_view>
#include <exception>
#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif
#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif
#define ANIM_DEFER_SW_HIDE (WM_APP + 101)
using DefWindowProcW_t = LRESULT (WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using ShowWindow_t = BOOL (WINAPI*)(HWND, int);
using ShowWindowAsync_t = BOOL (WINAPI*)(HWND, int);
using SetWindowPlacement_t = BOOL (WINAPI*)(HWND, const WINDOWPLACEMENT*);
using CloseWindow_t = BOOL (WINAPI*)(HWND);
using SetWindowPos_t = BOOL (WINAPI*)(HWND, HWND, int, int, int, int, UINT);
using DestroyWindow_t = BOOL (WINAPI*)(HWND);
DefWindowProcW_t DefWindowProcW_Original;
ShowWindow_t ShowWindow_Original;
ShowWindowAsync_t ShowWindowAsync_Original;
SetWindowPlacement_t SetWindowPlacement_Original;
CloseWindow_t CloseWindow_Original;
SetWindowPos_t SetWindowPos_Original;
DestroyWindow_t DestroyWindow_Original;
struct WindowAnimData {
    HWND hRealWnd{};
    HBITMAP hBitmap{};
    void* pBits{};
    RECT targetRect{};
    HMONITOR hMon{};
    int width{}, height{}, targetDockX{};
    BOOL isRising{};
    LONG_PTR originalExStyle{};
    BOOL hiddenByCloak{};
    HANDLE hFirstFrameShown{};
    int durationMs{};
    BOOL isClosing{};
    UINT closeMsg{};
    HANDLE hWaitFinish{};
};
struct LaunchAnimData { HWND hWnd; LONG_PTR originalExStyle; };
struct SwitchAnimData { HWND hWnd; int durationMs; };
struct SnapCache { HBITMAP hBmp; void* pBits; int w, h; };
struct ShatterBlock { int srcX, srcY; float dirX, dirY, force, noiseX, noiseY; int bw = 0, bh = 0; };
namespace AnimConstants {
    constexpr float SwitchStartScale = 0.94f;
    constexpr float MinimizeSpread = 0.65f;
    constexpr float PerlinNoiseScale = 150.0f;
    constexpr float PerlinLifeSpan = 0.15f;
    constexpr float ThanosBaseStartMax = 0.6f;
    constexpr float ThanosWaveNoiseMult = 0.15f;
    constexpr float ThanosLifeSpan = 0.4f;
    constexpr float ShatterTravelBase = 200.0f;
    constexpr float ShatterTravelMult = 1000.0f;
    constexpr float GlitchJitterEnd = 0.12f;
    constexpr float GlitchBlendLen = 0.08f;
    constexpr float GlitchEjectEnd = 0.56f;
    constexpr float GlitchJitterAmp = 14.0f;
    constexpr float GlitchShrinkStart = 0.58f;
    constexpr float GlitchTravelFracMin = 0.40f;
    constexpr float GlitchTravelFracMax = 0.65f;
    constexpr float GlitchFadeStart = 0.52f;
    constexpr float GlitchFadeDoneAt = 0.96f;
    constexpr int WaitTimeoutMs = 2500;
    constexpr int WaitSlackMs = 500;
    constexpr int AltTabPollMs = 16;
    constexpr int AltTabSessionMs = 500;
    constexpr int UiaLookupWaitMs = 120;
}
static constexpr std::wstring_view kGdiExcludedClasses[] = {
    L"CoreWindow",
    L"ApplicationFrameWindow",
    L"XamlExplorerHostIslandWindow"
};
static constexpr std::wstring_view kAlwaysExcludedClasses[] = {
    L"Xaml_WindowedPopupClass",
    L"Popup",
    L"Overlay",
    L"ToolTip"
};
static constexpr std::wstring_view kSafeCloseClasses[] = {
    L"ConsoleWindowClass",
    L"CASCADIA_HOSTING_WINDOW_CLASS",
    L"Notepad",
    L"TaskManagerWindow",
    L"WinUIDesktopWin32WindowClass"
};
static constexpr PCWSTR kPropCloseBypass = L"windows-animations.CloseBypass";
static constexpr PCWSTR kPropClosed = L"windows-animations.Closed";
std::unordered_map<HWND, SnapCache> g_WndSnapshots;
std::unordered_map<HWND, int> g_TaskbarDockXs;
std::unordered_map<std::wstring, int> g_ProcessDockXs;
std::unordered_map<HWND, std::wstring> g_ProcessNameCache;
std::unordered_set<HWND> g_LaunchSeen;
std::unordered_set<HWND> g_AnimActive;
std::mutex g_StateMutex;

std::atomic<HWINEVENTHOOK> g_hForegroundHook{NULL};
HANDLE g_hAltTabThread = NULL;
HANDLE g_hWinEventThread = NULL;
std::atomic<bool> g_winEventThreadStarted{false};
DWORD WINAPI WinEventHookThread(LPVOID lpParam);
struct alignas(8) SharedAnimState {
    volatile LONG lastAltTabTick;
    volatile LONG altTabSourceWindow;
    volatile LONG altTabStartTick;
    volatile LONG altTabGeneration;
};
static constexpr PCWSTR kSharedStateName = L"Local\\Windhawk_Anim_State_V5";
HANDLE g_hMapFile = NULL;
SharedAnimState* g_pSharedState = nullptr;
bool g_sharedStateWritable = false;
std::atomic<LONG> g_consumedGeneration{0};
std::atomic<int> g_durationMs{360};
std::atomic<int> g_closeDurationMs{900};
std::atomic<int> g_closeEffectStyle{1};
std::atomic<int> g_minRestoreEffectStyle{0};
std::atomic<int> g_shatterBlockSize{12};
std::atomic<bool> g_minimizeAnimation{true};
std::atomic<bool> g_restoreAnimation{true};
std::atomic<bool> g_closeAnimation{true};
std::atomic<bool> g_hideAsClose{false};
std::atomic<bool> g_launchAnimation{false};
std::atomic<bool> g_switchAnimation{true};
std::atomic<int> g_switchDurationMs{200};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_altTabThreadRunning{false};
std::mutex g_WorkerThreadsMutex;
std::vector<HANDLE> g_WorkerThreads;
template <typename T> static T Clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}
static bool StartWorkerThread(LPTHREAD_START_ROUTINE proc, void* param) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    for (size_t i = g_WorkerThreads.size(); i-- > 0;) {
        if (WaitForSingleObject(g_WorkerThreads[i], 0) == WAIT_OBJECT_0) {
            CloseHandle(g_WorkerThreads[i]);
            g_WorkerThreads.erase(g_WorkerThreads.begin() + i);
        }
    }
    try {
        g_WorkerThreads.reserve(g_WorkerThreads.size() + 1);
    } catch (const std::exception&) {
        return false;
    }
    HANDLE hThread = CreateThread(NULL, 0, proc, param, 0, NULL);
    if (!hThread) return false;
    g_WorkerThreads.push_back(hThread);
    return true;
}
static void JoinWorkerThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        threads.swap(g_WorkerThreads);
    }
    for (size_t i = 0; i < threads.size(); i += MAXIMUM_WAIT_OBJECTS) {
        const DWORD count = (DWORD)std::min<size_t>(MAXIMUM_WAIT_OBJECTS, threads.size() - i);
        WaitForMultipleObjects(count, threads.data() + i, TRUE, INFINITE);
    }
    for (HANDLE h : threads) CloseHandle(h);
}
static LONG HwndToShared(HWND hWnd) {
    return (LONG)(LONG_PTR)hWnd;
}
static LONG NonZeroTick() {
    const LONG tick = (LONG)GetTickCount();
    return tick ? tick : 1;
}
static void ResetAltTabState() {
    if (!g_pSharedState || !g_sharedStateWritable) return;
    InterlockedExchange(&g_pSharedState->lastAltTabTick, 0);
    InterlockedExchange(&g_pSharedState->altTabSourceWindow, 0);
    InterlockedExchange(&g_pSharedState->altTabStartTick, 0);
    InterlockedExchange(&g_pSharedState->altTabGeneration, 0);
}
static void BeginAltTabSession(HWND source) {
    if (!g_pSharedState || !g_sharedStateWritable) return;
    InterlockedExchange(&g_pSharedState->lastAltTabTick, 0);
    InterlockedExchange(&g_pSharedState->altTabSourceWindow, HwndToShared(source));
    InterlockedExchange(&g_pSharedState->altTabStartTick, (LONG)GetTickCount());
    LONG generation = InterlockedIncrement(&g_pSharedState->altTabGeneration);
    if (generation == 0) InterlockedIncrement(&g_pSharedState->altTabGeneration);
    InterlockedExchange(&g_pSharedState->lastAltTabTick, NonZeroTick());
}
static void TouchAltTabSession() {
    if (g_pSharedState && g_sharedStateWritable) {
        InterlockedExchange(&g_pSharedState->lastAltTabTick, NonZeroTick());
    }
}
static bool ConsumeAltTabIntent(HWND target, DWORD eventTime) {
    if (!g_pSharedState) return false;

    const LONG stamp = g_pSharedState->lastAltTabTick;
    if (!stamp || (LONG)(GetTickCount() - (DWORD)stamp) > AnimConstants::AltTabSessionMs) return false;

    const LONG generation = g_pSharedState->altTabGeneration;
    if (!generation) return false;
    const DWORD startTick = (DWORD)g_pSharedState->altTabStartTick;
    if (eventTime && startTick &&
        (LONG)(eventTime - startTick) < -(40 + AnimConstants::AltTabPollMs)) {
        return false;
    }

    const LONG source = g_pSharedState->altTabSourceWindow;
    if (generation != g_pSharedState->altTabGeneration) return false;
    LONG consumed = g_consumedGeneration.load(std::memory_order_relaxed);
    if (consumed == generation ||
        !g_consumedGeneration.compare_exchange_strong(consumed, generation, std::memory_order_relaxed)) {
        return false;
    }

    return !source || source != HwndToShared(target);
}
static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    if (!GetModuleFileNameW(NULL, path, ARRAYSIZE(path))) return false;
    const WCHAR* name = wcsrchr(path, L'\\');
    return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
}
static std::wstring GetClassNameStr(HWND hWnd) {
    WCHAR name[256];
    return GetClassNameW(hWnd, name, ARRAYSIZE(name)) ? std::wstring(name) : L"";
}
template <size_t N> static bool ContainsClass(const std::wstring& cls, const std::wstring_view (&items)[N]) {
    for (auto item : items) if (cls.find(item) != std::wstring::npos) return true;
    return false;
}
static HBITMAP CreateDib32(HDC dc, int width, int height, void** bits) {
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, bits, nullptr, 0);
}
static bool IsAnimating(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    return g_AnimActive.count(hWnd) != 0;
}
static void CleanupWindowData(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    auto it = g_WndSnapshots.find(hWnd);
    if (it != g_WndSnapshots.end()) { DeleteObject(it->second.hBmp); g_WndSnapshots.erase(it); }
    g_TaskbarDockXs.clear();
    g_ProcessDockXs.clear();
    g_ProcessNameCache.erase(hWnd);
    g_LaunchSeen.erase(hWnd);
}

static void SweepStaleData() {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    for (auto it = g_WndSnapshots.begin(); it != g_WndSnapshots.end();) {
        if (!IsWindow(it->first)) { DeleteObject(it->second.hBmp); it = g_WndSnapshots.erase(it); }
        else ++it;
    }
    for (auto it = g_TaskbarDockXs.begin(); it != g_TaskbarDockXs.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockXs.erase(it);
        else ++it;
    }
    for (auto it = g_ProcessNameCache.begin(); it != g_ProcessNameCache.end();) {
        if (!IsWindow(it->first)) it = g_ProcessNameCache.erase(it);
        else ++it;
    }
    for (auto it = g_LaunchSeen.begin(); it != g_LaunchSeen.end();) {
        if (!IsWindow(*it)) it = g_LaunchSeen.erase(it);
        else ++it;
    }
    for (auto it = g_AnimActive.begin(); it != g_AnimActive.end();) {
        if (!IsWindow(*it)) it = g_AnimActive.erase(it);
        else ++it;
    }
}

static bool ShouldUseBitBlt(HWND hWnd, bool isClosing) {
    if (!isClosing) return false;
    if (GetForegroundWindow() == hWnd) return true;
    const auto cls = GetClassNameStr(hWnd);
    return cls.find(L"CASCADIA") != std::wstring::npos || cls.find(L"ConsoleWindowClass") != std::wstring::npos;
}
static PSECURITY_DESCRIPTOR BuildSharedStateSd() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) return nullptr;
    DWORD len = 0;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &len);
    std::vector<BYTE> buffer(len);
    LPWSTR userSid = NULL;
    const bool haveSid =
        len && GetTokenInformation(hToken, TokenUser, buffer.data(), len, &len) &&
        ConvertSidToStringSidW(((TOKEN_USER*)buffer.data())->User.Sid, &userSid) && userSid;
    CloseHandle(hToken);
    if (!haveSid) return nullptr;

    std::wstring sddl = L"D:(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;";
    sddl += userSid;
    sddl += L")(A;;GR;;;WD)(A;;GR;;;AC)S:(ML;;NW;;;LW)";
    LocalFree(userSid);

    PSECURITY_DESCRIPTOR pSd = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1, &pSd, NULL)) {
        return nullptr;
    }
    return pSd;
}
void InitSharedMemory() {
    PSECURITY_DESCRIPTOR pSd = BuildSharedStateSd();
    SECURITY_ATTRIBUTES sa{sizeof(sa), pSd, FALSE};
    g_hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, pSd ? &sa : NULL, PAGE_READWRITE, 0,
                                    sizeof(SharedAnimState), kSharedStateName);
    if (pSd) LocalFree(pSd);
    if (!g_hMapFile) g_hMapFile = OpenFileMappingW(FILE_MAP_READ, FALSE, kSharedStateName);
    if (!g_hMapFile) {
        Wh_Log(L"Shared state mapping unavailable (Alt+Tab switch intent disabled in this process)");
        return;
    }
    g_sharedStateWritable = IsExplorerProcess();
    g_pSharedState = (SharedAnimState*)MapViewOfFile(
        g_hMapFile, g_sharedStateWritable ? (FILE_MAP_READ | FILE_MAP_WRITE) : FILE_MAP_READ, 0, 0,
        sizeof(SharedAnimState));
    if (!g_pSharedState && g_sharedStateWritable) {
        g_sharedStateWritable = false;
        g_pSharedState = (SharedAnimState*)MapViewOfFile(g_hMapFile, FILE_MAP_READ, 0, 0,
                                                         sizeof(SharedAnimState));
    }
    if (!g_pSharedState) Wh_Log(L"Failed to map shared Alt+Tab state");
}
void LoadAnimSettings() {
    g_durationMs.store(Clamp(Wh_GetIntSetting(L"duration_ms"), 200, 1400), std::memory_order_relaxed);
    g_closeDurationMs.store(Clamp(Wh_GetIntSetting(L"close_duration_ms"), 50, 5000), std::memory_order_relaxed);
    g_shatterBlockSize.store(Clamp(Wh_GetIntSetting(L"shatter_block_size"), 1, 100), std::memory_order_relaxed);
    if (PCWSTR style = Wh_GetStringSetting(L"close_effect_style")) {
        const int value = wcscmp(style, L"shatter") == 0 ? 0
                          : wcscmp(style, L"perlin") == 0 ? 2
                          : wcscmp(style, L"glitch") == 0 ? 3
                                                         : 1;
        g_closeEffectStyle.store(value, std::memory_order_relaxed);
        Wh_FreeStringSetting(style);
    }
    if (PCWSTR style = Wh_GetStringSetting(L"min_restore_effect_style")) {
        g_minRestoreEffectStyle.store(wcscmp(style, L"ink_splash") == 0 ? 1 : 0, std::memory_order_relaxed);
        Wh_FreeStringSetting(style);
    }
    g_minimizeAnimation.store(Wh_GetIntSetting(L"minimize_animation") != 0, std::memory_order_relaxed);
    g_restoreAnimation.store(Wh_GetIntSetting(L"restore_animation") != 0, std::memory_order_relaxed);
    g_closeAnimation.store(Wh_GetIntSetting(L"close_animation") != 0, std::memory_order_relaxed);
    g_hideAsClose.store(Wh_GetIntSetting(L"hide_as_close") != 0, std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"launch_animation") != 0, std::memory_order_relaxed);
    g_switchAnimation.store(Wh_GetIntSetting(L"switch_animation") != 0, std::memory_order_relaxed);
    g_switchDurationMs.store(Clamp(Wh_GetIntSetting(L"switch_duration_ms"), 50, 1000), std::memory_order_relaxed);
}
static void UpdateDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}
static void SetWindowCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}
static void UndoRisingHide(HWND hWnd, LONG_PTR originalExStyle, BOOL cloakHidden) {
    if (cloakHidden) {
        SetWindowCloak(hWnd, FALSE);
    } else {
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
    }
    UpdateDwmTransitions(hWnd, TRUE);
}
std::wstring GetProcessNameCached(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto it = g_ProcessNameCache.find(hWnd);
        if (it != g_ProcessNameCache.end()) return it->second;
    }
    std::wstring procNameLower = L"";
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWnd, &ownerPid);
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
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_ProcessNameCache[hWnd] = procNameLower;
    }
    return procNameLower;
}
void EnsureWinEventThreadStarted() {
    if (g_winEventThreadStarted.load(std::memory_order_relaxed)) return;
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (!g_switchAnimation.load(std::memory_order_relaxed)) return;
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (!g_switchAnimation.load(std::memory_order_relaxed)) return;
    if (g_hWinEventThread || g_winEventThreadStarted.load(std::memory_order_relaxed)) return;
    HANDLE hThread = CreateThread(NULL, 0, WinEventHookThread, NULL, 0, NULL);
    if (!hThread) return;
    g_hWinEventThread = hThread;
    g_winEventThreadStarted.store(true, std::memory_order_relaxed);
}
static bool IsAppMainWindow(HWND hWnd, bool forSwitch = false) {
    if (!hWnd || !IsWindow(hWnd) || !IsWindowVisible(hWnd) || IsIconic(hWnd) || GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) || GetWindow(hWnd, GW_OWNER)) return false;
    const auto cls = GetClassNameStr(hWnd);
    if (ContainsClass(cls, kAlwaysExcludedClasses) || (!forSwitch && ContainsClass(cls, kGdiExcludedClasses))) return false;
    RECT r{};
    bool isMain = GetWindowRect(hWnd, &r) && r.right - r.left >= 300 && r.bottom - r.top >= 300;
    return isMain;
}
static bool UseSafeClose(HWND hWnd) { return ContainsClass(GetClassNameStr(hWnd), kSafeCloseClasses); }
static bool ShouldTreatHideAsClose(HWND hWnd) {
    return g_hideAsClose.load(std::memory_order_relaxed) &&
           g_closeAnimation.load(std::memory_order_relaxed) && IsAppMainWindow(hWnd) && !UseSafeClose(hWnd);
}
static bool ShouldAnimateWindow(HWND hWnd) {
    if (!hWnd || (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD)) return false;
    RECT r{};
    if (IsIconic(hWnd)) {
        WINDOWPLACEMENT wp{}; wp.length = sizeof(wp);
        if (!GetWindowPlacement(hWnd, &wp)) return false;
        r = wp.rcNormalPosition;
    } else if (!GetWindowRect(hWnd, &r)) return false;
    return r.right - r.left >= 40 && r.bottom - r.top >= 40;
}
static bool IsLaunchWindow(HWND hWnd) {
    if (!hWnd || GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    return (style & WS_CAPTION) && !(exStyle & WS_EX_TOOLWINDOW) && ShouldAnimateWindow(hWnd);
}
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

struct UiaPending {
    volatile LONG refs = 2;
    HANDLE done = nullptr;
    int targetX = 0;
    bool found = false;
    void Release() {
        if (InterlockedDecrement(&refs) == 0) {
            if (done) CloseHandle(done);
            delete this;
        }
    }
};
struct UiaTask {
    HWND hWndApp;
    std::wstring titleLower;
    std::wstring procNameLower;
    std::wstring processKey;
    HMONITOR hMon;
    int fallbackX;
    UiaPending* pending = nullptr;
};

DWORD WINAPI UiaWorkerThread(LPVOID lpParam) {
    UiaTask* t = (UiaTask*)lpParam;
    UiaPending* pending = t->pending;
    struct UiaCleanupGuard {
        UiaTask* task;
        UiaPending* pending;
        ~UiaCleanupGuard() {
            if (pending) {
                SetEvent(pending->done);
                pending->Release();
            }
            delete task;
        }
    } guard{ t, pending };
    if (g_unloading.load(std::memory_order_relaxed)) {
        return 0;
    }

    int targetX = t->fallbackX;
    bool uiaFound = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    bool coInit = (hr == S_OK || hr == S_FALSE);
    if (hr == S_OK || hr == S_FALSE || hr == RPC_E_CHANGED_MODE) {
        IUIAutomation2* pAutomation = nullptr;
        HRESULT hrUia = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER,
                                         __uuidof(IUIAutomation2), (void**)&pAutomation);
        if (SUCCEEDED(hrUia) && pAutomation) {
            pAutomation->put_ConnectionTimeout(1000);
            pAutomation->put_TransactionTimeout(2000);
            HWND hTray = FindTaskbarForMonitor(t->hMon);
            if (hTray) {
                IUIAutomationElement* pTrayElement = nullptr;
                if (SUCCEEDED(pAutomation->ElementFromHandle(hTray, &pTrayElement)) && pTrayElement) {
                    if (g_unloading.load(std::memory_order_relaxed)) {
                        pTrayElement->Release();
                        pAutomation->Release();
                        if (coInit) CoUninitialize();
                        return 0;
                    }
                    IUIAutomationCondition* pButtonCond = nullptr;
                    IUIAutomationCondition* pListItemCond = nullptr;
                    IUIAutomationCondition* pOrCond = nullptr;
                    VARIANT varBtn{}; varBtn.vt = VT_I4; varBtn.lVal = UIA_ButtonControlTypeId;
                    pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &pButtonCond);
                    VARIANT varList{}; varList.vt = VT_I4; varList.lVal = UIA_ListItemControlTypeId;
                    pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &pListItemCond);
                    if (pButtonCond && pListItemCond) pAutomation->CreateOrCondition(pButtonCond, pListItemCond, &pOrCond);
                    IUIAutomationElementArray* pArray = nullptr;
                    if (pOrCond && SUCCEEDED(pTrayElement->FindAll(TreeScope_Descendants, pOrCond, &pArray)) && pArray) {
                        int length = 0;
                        pArray->get_Length(&length);
                        MONITORINFO mi = {0};
                        mi.cbSize = sizeof(MONITORINFO);
                        GetMonitorInfoW(t->hMon, &mi);
                        int monRight = mi.rcMonitor.right;
                        int bestScore = 0;
                        for (int i = 0; i < length; i++) {
                            if (g_unloading.load(std::memory_order_relaxed)) break;
                            IUIAutomationElement* pItem = nullptr;
                            if (SUCCEEDED(pArray->GetElement(i, &pItem)) && pItem) {
                                BSTR name;
                                if (SUCCEEDED(pItem->get_CurrentName(&name)) && name) {
                                    std::wstring uiaNameLower = name;
                                    std::transform(uiaNameLower.begin(), uiaNameLower.end(), uiaNameLower.begin(), ::towlower);
                                    if (!uiaNameLower.empty()) {
                                        int score = 0;
                                        if (t->titleLower == uiaNameLower) score += 1000;
                                        if (!t->titleLower.empty() && t->titleLower.find(uiaNameLower) != std::wstring::npos) score += 500;
                                        if (!uiaNameLower.empty() && uiaNameLower.find(t->titleLower) != std::wstring::npos) score += 500;
                                        if (!t->procNameLower.empty() && uiaNameLower.find(t->procNameLower) != std::wstring::npos) score += 400;
                                        if (uiaNameLower.find(L"start") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"search") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"task view") != std::wstring::npos) score -= 500;
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
    if (uiaFound && !g_unloading.load(std::memory_order_relaxed)) {
        if (pending) {
            pending->targetX = targetX;
            pending->found = true;
        }
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_TaskbarDockXs[t->hWndApp] = targetX;
        if (!t->processKey.empty()) g_ProcessDockXs[t->processKey] = targetX;
        Wh_Log(L"UIA dock match hwnd=%p x=%d", t->hWndApp, targetX);
    } else {
        Wh_Log(L"UIA dock miss hwnd=%p fallback=%d", t->hWndApp, t->fallbackX);
    }
    return 0;
}

int GetTaskbarButtonX_Async(HWND hWndApp, const WCHAR* windowTitle, int fallbackX, HMONITOR hMon) {
    std::wstring procNameLower = GetProcessNameCached(hWndApp);
    std::wstring processKey = procNameLower;
    if (!processKey.empty() && hMon) {
        processKey += L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
    }
    int cachedX = 0;
    bool haveCache = false;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto it = g_TaskbarDockXs.find(hWndApp);
        if (it != g_TaskbarDockXs.end()) {
            cachedX = it->second;
            haveCache = true;
        } else if (!processKey.empty()) {
            auto pit = g_ProcessDockXs.find(processKey);
            if (pit != g_ProcessDockXs.end()) {
                cachedX = pit->second;
                haveCache = true;
            }
        }
    }
    std::wstring titleLower = windowTitle ? windowTitle : L"";
    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::towlower);

    auto* pending = new UiaPending{};
    pending->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    pending->targetX = haveCache ? cachedX : fallbackX;
    if (!pending->done) {
        pending->Release();
        pending->Release();
        return haveCache ? cachedX : fallbackX;
    }

    auto* task = new UiaTask{hWndApp, titleLower, procNameLower, processKey, hMon, fallbackX, pending};
    if (!StartWorkerThread(UiaWorkerThread, task)) {
        delete task;
        pending->Release();
        pending->Release();
        return haveCache ? cachedX : fallbackX;
    }

    const DWORD wait = WaitForSingleObject(pending->done, AnimConstants::UiaLookupWaitMs);
    int result = haveCache ? cachedX : fallbackX;
    if (wait == WAIT_OBJECT_0 && pending->found) result = pending->targetX;
    pending->Release();
    return result;
}
DWORD WINAPI AltTabTrackerThread(LPVOID lpParam) {
    bool altTabSession = false;
    HWND stableForeground = GetForegroundWindow();

    while (g_altTabThreadRunning.load(std::memory_order_relaxed) && !g_unloading.load(std::memory_order_relaxed)) {
        const bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        const bool tabDown = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;

        if (!altTabSession) {
            if (!altDown) {
                if (HWND current = GetForegroundWindow()) stableForeground = current;
            }
            if (altDown && tabDown) {
                HWND source = stableForeground ? stableForeground : GetForegroundWindow();
                BeginAltTabSession(source);
                altTabSession = true;
            }
        } else {
            TouchAltTabSession();
            if (!altDown) altTabSession = false;
        }

        Sleep(AnimConstants::AltTabPollMs);
    }
    return 0;
}
DWORD WINAPI SwitchingAnimThread(LPVOID lpParam) {
    SwitchAnimData* data = (SwitchAnimData*)lpParam;
    HWND hWnd = data->hWnd;
    int durationMs = data->durationMs;
    
    delete data; 

    struct CleanupGuard {
        HWND h;
        bool uncloaked = false; 
        ~CleanupGuard() {
            if (!uncloaked && IsWindow(h)) {
                SetWindowCloak(h, FALSE);
            }
            std::lock_guard<std::mutex> lock(g_StateMutex);
            g_AnimActive.erase(h);
        }
    } guard{ hWnd };

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    RECT winRect;
    if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(hWnd) || !IsAppMainWindow(hWnd, true) || !GetWindowRect(hWnd, &winRect)) {
        return 0; 
    }
    
    RECT extRect = winRect;
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extRect, sizeof(extRect));
    int ghostX = winRect.left;
    int ghostY = winRect.top;
    int ghostW = winRect.right - winRect.left;
    int ghostH = winRect.bottom - winRect.top;
    if (ghostW <= 0 || ghostH <= 0) {
        return 0; 
    }
    
    // Deliberately NOT WS_EX_TRANSPARENT, unlike the close/minimize ghost: that ghost floats
    // over a live window, but this one covers a *cloaked* window, and a cloaked window is not
    // hit-testable. Click-through here would skip the target and activate the window behind
    // it, so the ghost absorbs clicks for the duration instead - the target stays focused at
    // the cost of losing a click aimed at it, which is the better trade of the two.
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        L"STATIC", NULL, WS_POPUP,
        ghostX, ghostY, ghostW, ghostH,
        NULL, NULL, NULL, NULL);
    HTHUMBNAIL hThumb = NULL;
    HRESULT hr = DwmRegisterThumbnail(hGhost, hWnd, &hThumb);
    if (FAILED(hr)) {
        Wh_Log(L"Switch thumbnail register failed hwnd=%p hr=0x%08X", hWnd, hr);
        DestroyWindow(hGhost);
        return 0;
    }
    ShowWindow(hGhost, SW_SHOWNOACTIVATE);
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);
    double totalMs = (double)durationMs;
    float startScale = AnimConstants::SwitchStartScale;
    int W = extRect.right - extRect.left;
    int H = extRect.bottom - extRect.top;
    int offsetX = extRect.left - winRect.left;
    int offsetY = extRect.top - winRect.top;
    
    for (;;) {
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        float ease = 1.0f - powf(1.0f - progress, 3.0f);
        float currentScale = startScale + (1.0f - startScale) * ease;
        int thumbW = (int)(W * currentScale);
        int thumbH = (int)(H * currentScale);
        float cx = offsetX + W / 2.0f;
        float cy = offsetY + H / 2.0f;
        int thumbX = (int)(cx - thumbW / 2.0f);
        int thumbY = (int)(cy - thumbH / 2.0f);
        DWM_THUMBNAIL_PROPERTIES props = {0};
        props.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_OPACITY;
        props.fVisible = TRUE;
        props.opacity = (BYTE)(255.0f * ease);
        props.rcDestination.left = thumbX;
        props.rcDestination.top = thumbY;
        props.rcDestination.right = thumbX + thumbW;
        props.rcDestination.bottom = thumbY + thumbH;
        DwmUpdateThumbnailProperties(hThumb, &props);
        if (lastFrame || g_unloading.load(std::memory_order_relaxed)) break;
        DwmFlush();
    }
    
    if (IsWindow(hWnd)) {
        SetWindowCloak(hWnd, FALSE);
        guard.uncloaked = true;
    }
    
    if (hThumb) DwmUnregisterThumbnail(hThumb);
    DestroyWindow(hGhost);
    
    return 0; 
}
void CALLBACK ForegroundEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (!g_switchAnimation.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) return;

    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);
    if (windowPid != GetCurrentProcessId() || IsIconic(hWnd) || !IsAppMainWindow(hWnd, true)) return;
    if (!ConsumeAltTabIntent(hWnd, dwmsEventTime)) return;

    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (!g_AnimActive.insert(hWnd).second) return;
    }

    Wh_Log(L"Switch animation start hwnd=%p", hWnd);
    SetWindowCloak(hWnd, TRUE);
    auto* data = new SwitchAnimData{hWnd, g_switchDurationMs.load(std::memory_order_relaxed)};
    if (!StartWorkerThread(SwitchingAnimThread, data)) {
        Wh_Log(L"Switch animation worker failed hwnd=%p", hWnd);
        SetWindowCloak(hWnd, FALSE);
        delete data;
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_AnimActive.erase(hWnd);
    }
}
DWORD WINAPI WinEventHookThread(LPVOID lpParam) {
    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        NULL,
        ForegroundEventProc,
        GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT);
    g_hForegroundHook.store(hook, std::memory_order_release);
        
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    if (HWINEVENTHOOK oldHook = g_hForegroundHook.exchange(NULL, std::memory_order_acquire)) {
        UnhookWinEvent(oldHook);
    }
    return 0;
}
class AnimationEngine {
private:
    WindowAnimData* data = nullptr;
    int W = 0, H = 0;
    int origLeft = 0, origTop = 0;
    float origCenterX = 0.0f;
    float dockXf = 0.0f, dockY = 0.0f, neckW = 0.0f;
    int boundLeft = 0, boundTop = 0, boundW = 0, boundH = 0;
    HWND hGhost = NULL;
    HDC hScreenDC = NULL, hSrcDC = NULL, hSrcDibDC = NULL, hCanvasDC = NULL;
    HBITMAP hOldSrc = NULL, hSrcDib = NULL, hOldSrcDib = NULL, hCanvas = NULL, hOldCanvas = NULL;
    BYTE* srcBits = nullptr;
    BYTE* pBits = nullptr;
    int srcStride = 0, canvasStride = 0;
    size_t canvasBytes = 0;
    int blockSizeSetting = 1;
    int closeEffect = 1;
    int minRestoreEffect = 0;
    double totalMs = 0.0;
    bool blocksInput = false;
    std::vector<ShatterBlock> shatterBlocks;
    std::vector<float> yb;
    bool firstFramePending = true;
    inline float MorphAt(float v, float tt) {
        float m = tt * (1.0f + AnimConstants::MinimizeSpread) - (1.0f - v) * AnimConstants::MinimizeSpread;
        if (m < 0.0f) m = 0.0f;
        if (m > 1.0f) m = 1.0f;
        return m * m * (3.0f - 2.0f * m);
    }
    inline void DrawBlock(int srcX, int srcY, int dstX, int dstY, float alpha, int bw = -1, int bh = -1) {
        if (bw <= 0) bw = blockSizeSetting;
        if (bh <= 0) bh = blockSizeSetting;
        const int y0 = dstY < 0 ? -dstY : 0;
        const int y1 = std::min(bh, std::min(H - srcY, boundH - dstY));
        const int x0 = dstX < 0 ? -dstX : 0;
        const int x1 = std::min(bw, std::min(W - srcX, boundW - dstX));
        if (x0 >= x1 || y0 >= y1) return;
        const size_t bytes = (size_t)(x1 - x0) * sizeof(uint32_t);
        for (int by = y0; by < y1; ++by) {
            const BYTE* src = srcBits + (size_t)(srcY + by) * srcStride + (size_t)(srcX + x0) * 4;
            BYTE* dst = pBits + (size_t)(dstY + by) * canvasStride + (size_t)(dstX + x0) * 4;
            if (alpha >= 0.99f) {
                memcpy(dst, src, bytes);
                continue;
            }
            for (int bx = x0; bx < x1; ++bx, src += 4, dst += 4) {
                dst[0] = (BYTE)(src[0] * alpha); dst[1] = (BYTE)(src[1] * alpha);
                dst[2] = (BYTE)(src[2] * alpha); dst[3] = (BYTE)(src[3] * alpha);
            }
        }
    }
    inline void DrawBlockScaled(int srcX, int srcY, int srcW, int srcH, int dstX, int dstY, int dstW,
                                int dstH, float alpha) {
        if (dstW < 1 || dstH < 1 || srcW < 1 || srcH < 1 || alpha <= 0.0f) return;
        if (dstW == srcW && dstH == srcH) {
            DrawBlock(srcX, srcY, dstX, dstY, alpha, srcW, srcH);
            return;
        }
        for (int dy = 0; dy < dstH; ++dy) {
            const int outY = dstY + dy;
            if (outY < 0 || outY >= boundH) continue;
            const int sy = srcY + dy * srcH / dstH;
            if (sy < 0 || sy >= H) continue;
            const BYTE* srcRow = srcBits + (size_t)sy * srcStride;
            BYTE* dstRow = pBits + (size_t)outY * canvasStride;
            for (int dx = 0; dx < dstW; ++dx) {
                const int outX = dstX + dx;
                if (outX < 0 || outX >= boundW) continue;
                const int sx = srcX + dx * srcW / dstW;
                if (sx < 0 || sx >= W) continue;
                const BYTE* s = srcRow + (size_t)sx * 4;
                BYTE* d = dstRow + (size_t)outX * 4;
                if (alpha >= 0.99f) {
                    d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
                } else {
                    d[0] = (BYTE)(s[0] * alpha); d[1] = (BYTE)(s[1] * alpha);
                    d[2] = (BYTE)(s[2] * alpha); d[3] = (BYTE)(s[3] * alpha);
                }
            }
        }
    }
    void RenderPerlin(float progress, float& fade) {
        fade = 1.0f;
        for (const auto& b : shatterBlocks) {
            float localProgress = (progress - b.noiseY) / AnimConstants::PerlinLifeSpan;
            if (localProgress >= 1.0f) continue;
            int dstBaseX = (origLeft - boundLeft) + b.srcX;
            int dstBaseY = (origTop - boundTop) + b.srcY;
            DrawBlock(b.srcX, b.srcY, dstBaseX, dstBaseY, (localProgress <= 0.0f) ? 1.0f : (1.0f - localProgress));
        }
    }
    void RenderShatter(float progress, float& fade) {
        fade = 1.0f - progress;
        if (fade < 0.0f) fade = 0.0f;
        float easeOut = 1.0f - powf(1.0f - progress, 5.0f);
        for (const auto& b : shatterBlocks) {
            float travel = easeOut * (AnimConstants::ShatterTravelBase + b.force * AnimConstants::ShatterTravelMult);
            float dX = b.dirX * travel + b.noiseX * travel * 0.4f;
            float dY = b.dirY * travel + b.noiseY * travel * 0.4f;
            int dstBaseX = (origLeft - boundLeft) + b.srcX + (int)dX;
            int dstBaseY = (origTop - boundTop) + b.srcY + (int)dY;
            DrawBlock(b.srcX, b.srcY, dstBaseX, dstBaseY, 1.0f);
        }
    }
    void RenderThanos(float progress, float& fade) {
        fade = 1.0f;
        for (const auto& b : shatterBlocks) {
            float localProgress = (progress - b.noiseY) / AnimConstants::ThanosLifeSpan;
            if (localProgress >= 1.0f) continue;
            int dstBaseX = (origLeft - boundLeft) + b.srcX;
            int dstBaseY = (origTop - boundTop) + b.srcY;
            float currentAlphaMult = 1.0f;
            if (localProgress > 0.0f) {
                float travel = localProgress;
                float travelSq = travel * travel;
                dstBaseX += (int)(b.dirX * travel + b.force * travelSq);
                dstBaseY += (int)(b.dirY * travel + b.noiseX * travelSq);
                currentAlphaMult = 1.0f - localProgress;
            }
            DrawBlock(b.srcX, b.srcY, dstBaseX, dstBaseY, currentAlphaMult);
        }
    }
    void RenderGlitch(float progress, float& fade) {
        fade = 1.0f;
        const float jitterEnd = AnimConstants::GlitchJitterEnd;
        const float blendLen = AnimConstants::GlitchBlendLen;
        const float ejectEnd = AnimConstants::GlitchEjectEnd;
        const float ejectSpan = 1.0f - jitterEnd;
        const float ejectPortion = (ejectEnd - jitterEnd) / ejectSpan;
        const float shrinkStart = AnimConstants::GlitchShrinkStart;
        const float fadeStart = AnimConstants::GlitchFadeStart;
        const float fadeDone = AnimConstants::GlitchFadeDoneAt;
        static constexpr int kGlitchCuts[] = {0, -2, 3, -4, 2, -3, 4, -1, 5, -5, 1, -6};
        const int cutCount = (int)(sizeof(kGlitchCuts) / sizeof(kGlitchCuts[0]));
        for (const auto& b : shatterBlocks) {
            const int bw = b.bw > 0 ? b.bw : blockSizeSetting;
            const int bh = b.bh > 0 ? b.bh : blockSizeSetting;
            const int baseX = (origLeft - boundLeft) + b.srcX;
            const int baseY = (origTop - boundTop) + b.srcY;
            float alpha = 1.0f;
            float scale = 1.0f;
            float jitterW = 0.0f;
            int jitterX = 0, jitterY = 0;
            if (progress < jitterEnd + blendLen) {
                const float tJ = Clamp(progress / jitterEnd, 0.0f, 1.0f);
                const float amp = AnimConstants::GlitchJitterAmp * (0.55f + 1.6f * tJ * tJ);
                const int frame = (int)(progress * 90.0f);
                const int slot = frame + (b.srcY * 3) + (b.srcX >> 3);
                int cut = kGlitchCuts[slot % cutCount];
                if (tJ > 0.55f) {
                    const float bias = (tJ - 0.55f) / 0.45f;
                    int directed = (b.dirX < 0.0f) ? -abs(cut) : abs(cut);
                    if (directed == 0) directed = (b.dirX < 0.0f) ? -3 : 3;
                    cut = (int)((float)cut * (1.0f - bias) + (float)directed * bias);
                }
                jitterX = (int)((float)cut * amp);
                if ((slot % 3) == 0) {
                    const int vCut = kGlitchCuts[(slot * 5 + 2) % cutCount];
                    jitterY = (int)((float)vCut * amp * 0.35f);
                }
                if (progress < jitterEnd) {
                    jitterW = 1.0f;
                } else {
                    jitterW = 1.0f - (progress - jitterEnd) / blendLen;
                    if (jitterW < 0.0f) jitterW = 0.0f;
                }
            }
            float ejectT = 0.0f;
            int travelX = 0;
            if (progress >= jitterEnd) {
                float raw = (progress - jitterEnd) / ejectSpan;
                const float lag = Clamp(b.noiseX, 0.0f, 0.08f);
                float delayed = (raw - lag) / std::max(0.55f, 1.0f - lag * 0.5f);
                if (delayed < 0.0f) delayed = 0.0f;
                if (delayed > 1.0f) delayed = 1.0f;
                if (delayed < ejectPortion) {
                    float u = delayed / ejectPortion;
                    ejectT = (u >= 1.0f) ? 1.0f : (1.0f - powf(2.0f, -7.0f * u));
                } else {
                    ejectT = 1.0f;
                }

                if (ejectT <= fadeStart) {
                    alpha = 1.0f;
                } else if (ejectT >= fadeDone) {
                    alpha = 0.0f;
                } else {
                    float fadeU = (ejectT - fadeStart) / (fadeDone - fadeStart);
                    alpha = powf(1.0f - fadeU, 1.15f);
                }
                if (ejectT > shrinkStart) {
                    float s = (ejectT - shrinkStart) / std::max(0.05f, fadeDone - shrinkStart);
                    if (s < 0.0f) s = 0.0f;
                    if (s > 1.0f) s = 1.0f;
                    scale = 1.0f - (s * s * s);
                }
                const float travelFrac = AnimConstants::GlitchTravelFracMin +
                    (AnimConstants::GlitchTravelFracMax - AnimConstants::GlitchTravelFracMin) * b.force;
                const float maxTravel = (float)W * travelFrac * Clamp(b.noiseY, 0.75f, 1.15f);
                travelX = (int)(b.dirX * maxTravel * ejectT);
            }
            const int dstX = baseX + (int)((float)jitterX * jitterW) + travelX;
            const int dstY = baseY + (int)((float)jitterY * jitterW);
            if (alpha <= 0.01f || scale <= 0.02f) continue;
            if (scale >= 0.98f) {
                DrawBlock(b.srcX, b.srcY, dstX, dstY, alpha, bw, bh);
            } else {
                const int dw = std::max(1, (int)((float)bw * scale + 0.5f));
                const int dh = std::max(1, (int)((float)bh * scale + 0.5f));
                const int sx = dstX + (bw - dw) / 2;
                const int sy = dstY + (bh - dh) / 2;
                DrawBlockScaled(b.srcX, b.srcY, bw, bh, sx, sy, dw, dh, alpha);
            }
        }
    }
    void RenderInkSplash(float progress, float& fade) {
        fade = 1.0f;
        const float p = data->isRising ? progress : (1.0f - progress);
        const float boundary = p * 1.7f - 0.15f;
        const int dstBaseX = origLeft - boundLeft;
        const int dstBaseY = origTop - boundTop;
        for (const auto& b : shatterBlocks) {
            const float diff = b.noiseY - boundary;
            float t = (diff - 0.04f) / (-0.04f - 0.04f);
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            const float reveal = t * t * (3.0f - 2.0f * t);
            if (reveal <= 0.01f) continue;
            DrawBlock(b.srcX, b.srcY, dstBaseX + b.srcX, dstBaseY + b.srcY, reveal, b.bw, b.bh);
        }
    }
    void RenderMinimizeRestore(float progress, float& fade) {
        float tt = data->isRising ? (1.0f - progress) : progress;
        if (tt > 0.8f) fade = (1.0f - tt) / 0.2f;
        if (fade < 0.0f) fade = 0.0f;
        if (fade > 1.0f) fade = 1.0f;
        for (int k = 0; k <= H; ++k) {
            float v = (float)k / (float)H;
            float e = MorphAt(v, tt);
            float idY = (float)origTop + (float)H * v;
            yb[k] = idY + (dockY - idY) * e;
        }
        int kSeg = 0;
        for (int yC = 0; yC < boundH; ++yC) {
            float screenY = (float)(yC + boundTop) + 0.5f;
            if (screenY < yb[0] || screenY >= yb[H]) continue;
            while (kSeg < H - 1 && yb[kSeg + 1] <= screenY) kSeg++;
            float segH = yb[kSeg + 1] - yb[kSeg];
            float frac = segH > 1e-4f ? (screenY - yb[kSeg]) / segH : 0.0f;
            float v = ((float)kSeg + frac) / (float)H;
            float em = MorphAt(v, tt);
            float width = (float)W + (neckW - (float)W) * em;
            if (width < 1.0f) width = 1.0f;
            float cx = origCenterX + (dockXf - origCenterX) * em;
            float leftCanvas = (cx - width * 0.5f) - (float)boundLeft;
            int srcRow = (int)(v * (float)H);
            if (srcRow < 0) srcRow = 0;
            if (srcRow > H - 1) srcRow = H - 1;
            const BYTE* srcRowPtr = srcBits + (size_t)srcRow * srcStride;
            BYTE* dstRowPtr = pBits + (size_t)yC * canvasStride;
            int xStart = (int)leftCanvas;
            int xEnd   = (int)(leftCanvas + width) + 1;
            if (xStart < 0) xStart = 0;
            if (xEnd > boundW) xEnd = boundW;
            float invW = 1.0f / width;
            for (int xC = xStart; xC < xEnd; ++xC) {
                float u = ((float)xC + 0.5f - leftCanvas) * invW;
                if (u < 0.0f || u >= 1.0f) continue;
                int srcX = (int)(u * (float)W);
                if (srcX < 0) srcX = 0;
                if (srcX > W - 1) srcX = W - 1;
                ((uint32_t*)dstRowPtr)[xC] = ((const uint32_t*)srcRowPtr)[srcX];
            }
        }
    }
public:
    AnimationEngine(WindowAnimData* d) {
        data = d;
        W = data->width;
        H = data->height;
        origLeft = data->targetRect.left;
        origTop  = data->targetRect.top;
        origCenterX = (float)origLeft + W * 0.5f;
        HMONITOR hMon = MonitorFromWindow(data->hRealWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mmi; mmi.cbSize = sizeof(mmi);
        if (hMon && GetMonitorInfoW(hMon, &mmi)) {
        } else {
            mmi.rcMonitor.left = 0; mmi.rcMonitor.top = 0;
            mmi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
            mmi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        }
        const int monLeft = (int)mmi.rcMonitor.left, monTop = (int)mmi.rcMonitor.top;
        const int monRight = (int)mmi.rcMonitor.right, monBottom = (int)mmi.rcMonitor.bottom;
        const int dockX = Clamp(data->targetDockX, monLeft, monRight);
        dockXf = (float)dockX;
        dockY = (float)monBottom;
        neckW = Clamp(W * 0.03f, 12.0f, 60.0f);
        blockSizeSetting = std::max(1, g_shatterBlockSize.load(std::memory_order_relaxed));
        closeEffect = g_closeEffectStyle.load(std::memory_order_relaxed);
        minRestoreEffect = g_minRestoreEffectStyle.load(std::memory_order_relaxed);
        if (data->isClosing) {
            int padLeft = 0, padTop = 0, padRight = 0, padBottom = 0;
            if (closeEffect == 0) {
                int maxShatter = (int)(AnimConstants::ShatterTravelBase + AnimConstants::ShatterTravelMult) + 50;
                padLeft = padTop = padRight = padBottom = maxShatter;
            } else if (closeEffect == 1) {
                padLeft = W / 2;
                padRight = (int)(W * 1.5f);
                padTop = H / 2;
                padBottom = (int)(H * 1.5f);
            } else if (closeEffect == 3) {
                padLeft = padRight = (int)(W * 0.7f) + 24;
                padTop = padBottom = 8;
            } else {
                padLeft = padTop = padRight = padBottom = 50;
            }
            boundLeft = std::max(monLeft, origLeft - padLeft);
            boundTop = std::max(monTop, origTop - padTop);
            int boundRight = std::min(monRight, origLeft + W + padRight);
            int boundBottom = std::min(monBottom, origTop + H + padBottom);
            boundW = boundRight - boundLeft;
            boundH = boundBottom - boundTop;
        } else if (minRestoreEffect == 1) {
            boundLeft = Clamp(origLeft, monLeft, monRight - 1);
            boundTop = Clamp(origTop, monTop, monBottom - 1);
            boundW = std::min(W, monRight - boundLeft);
            boundH = std::min(H, monBottom - boundTop);
        } else {
            boundLeft = std::max(monLeft, std::min(origLeft, dockX) - W / 2);
            const int boundRight = std::min(monRight, std::max(origLeft + W, dockX) + W / 2);
            boundTop = std::max(monTop, origTop);
            boundW = boundRight - boundLeft;
            boundH = monBottom - boundTop;
        }
        if (boundW < 1) boundW = 1;
        if (boundH < 1) boundH = 1;
        totalMs = (double)data->durationMs;
        blocksInput = !data->isClosing && data->isRising && data->hiddenByCloak;
    }
    bool Initialize() {
        hGhost = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
                (blocksInput ? 0 : WS_EX_TRANSPARENT),
            L"STATIC", NULL, WS_POPUP,
            boundLeft, boundTop, boundW, boundH,
            NULL, NULL, NULL, NULL);
        if (!hGhost) return false;
        hScreenDC = GetDC(NULL);
        if (!hScreenDC) return false;
        hSrcDC = CreateCompatibleDC(hScreenDC);
        if (!hSrcDC) return false;
        hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);
        if (!hOldSrc || hOldSrc == HGDI_ERROR) return false;
        hSrcDib = CreateDib32(hScreenDC, W, H, (void**)&srcBits);
        if (!hSrcDib || !srcBits) return false;
        hSrcDibDC = CreateCompatibleDC(hScreenDC);
        if (!hSrcDibDC) return false;
        hOldSrcDib = (HBITMAP)SelectObject(hSrcDibDC, hSrcDib);
        if (!hOldSrcDib || hOldSrcDib == HGDI_ERROR) return false;
        BitBlt(hSrcDibDC, 0, 0, W, H, hSrcDC, 0, 0, SRCCOPY);
        GdiFlush();
        srcStride = W * 4;
        hCanvas = CreateDib32(hScreenDC, boundW, boundH, (void**)&pBits);
        if (!hCanvas || !pBits) return false;
        hCanvasDC = CreateCompatibleDC(hScreenDC);
        if (!hCanvasDC) return false;
        hOldCanvas = (HBITMAP)SelectObject(hCanvasDC, hCanvas);
        if (!hOldCanvas || hOldCanvas == HGDI_ERROR) return false;
        canvasStride = boundW * 4;
        canvasBytes = (size_t)boundW * 4 * boundH;
        if (!data->isClosing && minRestoreEffect == 0) yb.resize(H + 1);
        return true;
    }
    void PresentCanvas(float fade) {
        POINT ptDst = {boundLeft, boundTop};
        SIZE sz = {boundW, boundH};
        POINT ptSrc = {0, 0};
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = (BYTE)(255.0f * fade);
        bf.AlphaFormat = AC_SRC_ALPHA;
        UpdateLayeredWindow(hGhost, hScreenDC, &ptDst, &sz, hCanvasDC, &ptSrc, 0, &bf, ULW_ALPHA);
        if (firstFramePending) {
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFramePending = false;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
        }
    }
    void ShowInkBootstrapFrame() {
        memset(pBits, 0, canvasBytes);
        if (!data->isRising) {
            const int dstX = origLeft - boundLeft;
            const int dstY = origTop - boundTop;
            DrawBlock(0, 0, dstX, dstY, 1.0f, W, H);
        }
        PresentCanvas(1.0f);
        DwmFlush();
    }
    bool PrecalcInkSplash() {
        auto fractf = [](float x) -> float { return x - floorf(x); };
        auto is_hash = [&](float px, float py) -> float {
            return fractf(sinf(px * 127.1f + py * 311.7f) * 43758.5453f);
        };
        auto is_noise = [&](float x, float y) -> float {
            const float ix = floorf(x), iy = floorf(y);
            float fx = x - ix, fy = y - iy;
            fx = fx * fx * (3.0f - 2.0f * fx);
            fy = fy * fy * (3.0f - 2.0f * fy);
            const float a = is_hash(ix, iy);
            const float b = is_hash(ix + 1.0f, iy);
            const float c = is_hash(ix, iy + 1.0f);
            const float d = is_hash(ix + 1.0f, iy + 1.0f);
            return a * (1.0f - fx) * (1.0f - fy) + b * fx * (1.0f - fy) + c * (1.0f - fx) * fy + d * fx * fy;
        };
        auto is_fbm = [&](float x, float y) -> float {
            float v = 0.0f, amp = 0.5f;
            for (int i = 0; i < 4; ++i) {
                v += amp * is_noise(x, y);
                x *= 2.1f;
                y *= 2.1f;
                amp *= 0.5f;
            }
            return v;
        };
        const float aspect = (float)W / (float)std::max(1, H);
        const int fieldStep = std::max(8, std::min(W, H) / 48);
        const int drawStep = std::max(4, fieldStep / 2);
        const int nw = (W + fieldStep - 1) / fieldStep;
        const int nh = (H + fieldStep - 1) / fieldStep;
        try {
            std::vector<float> field((size_t)nw * (size_t)nh);
            for (int iy = 0; iy < nh; ++iy) {
                for (int ix = 0; ix < nw; ++ix) {
                    const float uvx = ((float)ix + 0.5f) * (float)fieldStep / (float)W;
                    const float uvy = ((float)iy + 0.5f) * (float)fieldStep / (float)H;
                    const float blob = is_fbm(uvx * 3.5f, uvy * 3.5f);
                    const float fingers = is_fbm(uvx * 14.0f, uvy * 14.0f);
                    const float distortion = (blob - 0.5f) * 0.5f + (fingers - 0.5f) * 0.18f;
                    float cx = uvx - 0.5f;
                    float cy = uvy - 0.5f;
                    cx *= aspect;
                    field[(size_t)iy * nw + ix] = sqrtf(cx * cx + cy * cy) + distortion;
                }
            }
            shatterBlocks.reserve(((W + drawStep - 1) / drawStep) * ((H + drawStep - 1) / drawStep));
            for (int y = 0; y < H; y += drawStep) {
                const int bh = std::min(drawStep, H - y);
                for (int x = 0; x < W; x += drawStep) {
                    const int bw = std::min(drawStep, W - x);
                    const float fx = ((float)x + 0.5f * (float)bw) / (float)fieldStep - 0.5f;
                    const float fy = ((float)y + 0.5f * (float)bh) / (float)fieldStep - 0.5f;
                    int x0 = (int)floorf(fx);
                    int y0 = (int)floorf(fy);
                    float tx = fx - (float)x0;
                    float ty = fy - (float)y0;
                    if (x0 < 0) x0 = 0;
                    if (y0 < 0) y0 = 0;
                    if (x0 > nw - 2) x0 = std::max(0, nw - 2);
                    if (y0 > nh - 2) y0 = std::max(0, nh - 2);
                    const float a = field[(size_t)y0 * nw + x0];
                    const float b = field[(size_t)y0 * nw + x0 + 1];
                    const float c = field[(size_t)(y0 + 1) * nw + x0];
                    const float d = field[(size_t)(y0 + 1) * nw + x0 + 1];
                    const float splashD = a * (1.0f - tx) * (1.0f - ty) + b * tx * (1.0f - ty) +
                                         c * (1.0f - tx) * ty + d * tx * ty;
                    shatterBlocks.push_back({x, y, 0, 0, 0, 0, splashD, bw, bh});
                }
            }
        } catch (const std::exception&) {
            Wh_Log(L"Ink splash precalc failed");
            shatterBlocks.clear();
            return false;
        }
        return true;
    }
    bool PrecalcPhysics() {
        if (!data->isClosing) {
            if (minRestoreEffect == 1) return PrecalcInkSplash();
            return true;
        }
        float cx = W / 2.0f;
        float cy = H / 2.0f;
        float maxDist = (float)(W + H);
        
        try {
            shatterBlocks.reserve((W / blockSizeSetting + 1) * (H / blockSizeSetting + 1));
            
            if (closeEffect == 2) {
                auto pseudo_hash = [](int ix, int iy) -> float {
                    uint32_t h = ((uint32_t)ix * 73856093u) ^ ((uint32_t)iy * 19349663u);
                    return (h % 10000) / 10000.0f;
                };
                auto smooth_noise = [&](float x, float y) -> float {
                    int ix = (int)floorf(x);
                    int iy = (int)floorf(y);
                    float fx = x - ix;
                    float fy = y - iy;
                    float ux = fx * fx * (3.0f - 2.0f * fx);
                    float uy = fy * fy * (3.0f - 2.0f * fy);
                    float a = pseudo_hash(ix, iy);
                    float b = pseudo_hash(ix + 1, iy);
                    float c = pseudo_hash(ix, iy + 1);
                    float d = pseudo_hash(ix + 1, iy + 1);
                    return a*(1.0f-ux)*(1.0f-uy) + b*ux*(1.0f-uy) + c*(1.0f-ux)*uy + d*ux*uy;
                };
                for (int srcY = 0; srcY < H; srcY += blockSizeSetting) {
                    for (int srcX = 0; srcX < W; srcX += blockSizeSetting) {
                        float nx = (float)srcX / AnimConstants::PerlinNoiseScale;
                        float ny = (float)srcY / AnimConstants::PerlinNoiseScale;
                        float v = 0.0f;
                        float amp = 0.5f;
                        float tx = nx, ty = ny;
                        for (int i = 0; i < 3; i++) {
                            v += amp * smooth_noise(tx, ty);
                            tx *= 2.0f; ty *= 2.0f;
                            amp *= 0.5f;
                        }
                        float startTime = v * 0.9f;
                        shatterBlocks.push_back({srcX, srcY, 0, 0, 0, 0, startTime});
                    }
                }
            } else if (closeEffect == 3) {
                auto next = [](uint32_t& s) -> uint32_t {
                    s ^= s << 13;
                    s ^= s >> 17;
                    s ^= s << 5;
                    return s;
                };
                uint32_t seed = 0xC0FFEEu ^ (uint32_t)W ^ ((uint32_t)H << 16) ^
                                ((uint32_t)blockSizeSetting * 2654435761u);
                const int minStrip = 1;
                const int maxStrip = std::max(minStrip, std::min(std::max(8, H / 6),
                                                                std::max(4, blockSizeSetting * 3)));
                const int minSeg = std::max(4, blockSizeSetting);
                const int maxSeg = std::max(minSeg, std::min(std::max(24, W / 3),
                                                             std::max(16, blockSizeSetting * 6)));
                for (int y = 0; y < H;) {
                    int stripH = minStrip + (int)(next(seed) % (uint32_t)(maxStrip - minStrip + 1));
                    if (y + stripH > H) stripH = H - y;
                    const float speed = 0.55f + 0.45f * (1.0f - (float)(stripH - minStrip) /
                                                                  (float)std::max(1, maxStrip - minStrip));
                    for (int x = 0; x < W;) {
                        int segW = minSeg + (int)(next(seed) % (uint32_t)(maxSeg - minSeg + 1));
                        if (x + segW > W) segW = W - x;
                        const float dirX = (next(seed) & 1) ? -1.0f : 1.0f;
                        const float startLag = (next(seed) % 1000) / 1000.0f * 0.28f;
                        const float travelScale = 0.8f + (next(seed) % 1000) / 1000.0f * 0.55f;
                        shatterBlocks.push_back(
                            {x, y, dirX, 0.0f, speed, startLag, travelScale, segW, stripH});
                        x += segW;
                    }
                    y += stripH;
                }
            } else {
                for (int srcY = 0; srcY < H; srcY += blockSizeSetting) {
                    for (int srcX = 0; srcX < W; srcX += blockSizeSetting) {
                        uint32_t hash = ((uint32_t)srcX * 73856093u) ^ ((uint32_t)srcY * 19349663u);
                        if (closeEffect == 0) {
                            float force = ((hash >> 8) % 100) / 100.0f;
                            float noiseX = ((hash % 2000) / 1000.0f) - 1.0f;
                            float noiseY = (((hash >> 4) % 2000) / 1000.0f) - 1.0f;
                            float dirX = (srcX + blockSizeSetting / 2.0f) - cx;
                            float dirY = (srcY + blockSizeSetting / 2.0f) - cy;
                            float dist = sqrtf(dirX * dirX + dirY * dirY);
                            if (dist > 0.1f) { dirX /= dist; dirY /= dist; }
                            shatterBlocks.push_back({srcX, srcY, dirX, dirY, force, noiseX, noiseY});
                        } else {
                            float distFromBottomRight = (float)((W - srcX) + (H - srcY));
                            float baseStartTime = (distFromBottomRight / maxDist) * AnimConstants::ThanosBaseStartMax;
                            float waveNoise = (((hash % 100) / 100.0f) - 0.5f) * AnimConstants::ThanosWaveNoiseMult;
                            float startTime = baseStartTime + waveNoise;
                            if (startTime < 0.0f) startTime = 0.0f;
                            if (startTime > 0.65f) startTime = 0.65f;
                            float noiseX = (((hash >> 4) % 200) / 100.0f) - 1.0f;
                            float noiseY = (((hash >> 8) % 200) / 100.0f) - 1.0f;
                            float baseWindX = W * 0.5f;
                            float baseWindY = H * 0.2f;
                            float swirl = ((srcY / (float)H) - 0.5f) * 2.0f;
                            baseWindY += swirl * (H * 0.3f);
                            float windX = baseWindX + noiseX * (W * 0.2f);
                            float windY = baseWindY + noiseY * (H * 0.3f);
                            float curveX = noiseY * (W * 0.4f);
                            float curveY = -noiseX * (H * 0.4f);
                            shatterBlocks.push_back({srcX, srcY, windX, windY, curveX, curveY, startTime});
                        }
                    }
                }
            }
        } catch (const std::exception&) {
            Wh_Log(L"Close physics precalc failed (likely OOM) effect=%d blocks~%zu", closeEffect,
                   shatterBlocks.capacity());
            shatterBlocks.clear();
            return false;
        }
        return true;
    }
    void RunLoop() {
        LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
        QueryPerformanceFrequency(&qpcFreq);
        QueryPerformanceCounter(&qpcStart);
        for (;;) {
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            QueryPerformanceCounter(&qpcNow);
            double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            BOOL lastFrame = (elapsedMs >= totalMs);
            float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
            memset(pBits, 0, canvasBytes);
            float fade = 1.0f;
            if (data->isClosing) {
                if (closeEffect == 2) RenderPerlin(progress, fade);
                else if (closeEffect == 0) RenderShatter(progress, fade);
                else if (closeEffect == 3) RenderGlitch(progress, fade);
                else RenderThanos(progress, fade);
            } else if (minRestoreEffect == 1) {
                RenderInkSplash(progress, fade);
            } else {
                RenderMinimizeRestore(progress, fade);
            }
            PresentCanvas(fade);
            if (lastFrame || g_unloading.load(std::memory_order_relaxed)) break;
            DwmFlush();
        }
    }
    void Teardown() {
        if (!data->isClosing && data->isRising) {
            if (data->hiddenByCloak) SetWindowCloak(data->hRealWnd, FALSE);
            else {
                SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
                if (!(data->originalExStyle & WS_EX_LAYERED)) {
                    SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, GetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
                }
            }
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            DwmFlush();
        }
        if (hScreenDC) ReleaseDC(NULL, hScreenDC);
        if (hGhost) DestroyWindow(hGhost);
        if (data->hFirstFrameShown) { SetEvent(data->hFirstFrameShown); CloseHandle(data->hFirstFrameShown); }
        
        if (data->isClosing) {
            if (data->hWaitFinish) { SetEvent(data->hWaitFinish); CloseHandle(data->hWaitFinish); data->hWaitFinish = NULL; }

            if (data->closeMsg != WM_DESTROY && IsWindow(data->hRealWnd)) {
                SetPropW(data->hRealWnd, kPropCloseBypass, (HANDLE)1);
                if (data->closeMsg == ANIM_DEFER_SW_HIDE) ShowWindowAsync_Original(data->hRealWnd, SW_HIDE);
                else if (data->closeMsg == WM_CLOSE) PostMessageW(data->hRealWnd, WM_CLOSE, 0, 0);
                else if (data->closeMsg == WM_SYSCOMMAND) PostMessageW(data->hRealWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                else PostMessageW(data->hRealWnd, data->closeMsg, 0, 0);
            }
            
            if (data->closeMsg != WM_DESTROY && IsWindow(data->hRealWnd)) {
                for (int i = 0; i < 50; ++i) {
                    if (!IsWindow(data->hRealWnd) || g_unloading.load(std::memory_order_relaxed) || !IsWindowVisible(data->hRealWnd)) break;
                    Sleep(10);
                }
                if (IsWindow(data->hRealWnd)) {
                    RemovePropW(data->hRealWnd, kPropCloseBypass);
                    RemovePropW(data->hRealWnd, kPropClosed);
                    
                    SetWindowCloak(data->hRealWnd, FALSE);
                    UpdateDwmTransitions(data->hRealWnd, TRUE);
                }
            }
        } else if (!data->isClosing && !data->isRising) {
            UpdateDwmTransitions(data->hRealWnd, TRUE);
        }
        
        if (hCanvasDC && hOldCanvas && hOldCanvas != HGDI_ERROR) SelectObject(hCanvasDC, hOldCanvas);
        if (hSrcDibDC && hOldSrcDib && hOldSrcDib != HGDI_ERROR) SelectObject(hSrcDibDC, hOldSrcDib);
        if (hSrcDC && hOldSrc && hOldSrc != HGDI_ERROR) SelectObject(hSrcDC, hOldSrc);
        if (hCanvas) DeleteObject(hCanvas);
        if (hSrcDib) DeleteObject(hSrcDib);
        if (data->hBitmap) DeleteObject(data->hBitmap);
        if (hCanvasDC) DeleteDC(hCanvasDC);
        if (hSrcDibDC) DeleteDC(hSrcDibDC);
        if (hSrcDC) DeleteDC(hSrcDC);
        
        { std::lock_guard<std::mutex> lock(g_StateMutex); g_AnimActive.erase(data->hRealWnd); }
        delete data;
    }
};
DWORD WINAPI MainAnimThread(LPVOID lpParam) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    AnimationEngine engine((WindowAnimData*)lpParam);
    if (engine.Initialize()) {
        if (!((WindowAnimData*)lpParam)->isClosing &&
            g_minRestoreEffectStyle.load(std::memory_order_relaxed) == 1) {
            engine.ShowInkBootstrapFrame();
        }
        if (engine.PrecalcPhysics()) {
            engine.RunLoop();
        }
    }
    engine.Teardown();
    return 0;
}
bool StartAnimation(HWND hWnd, BOOL rising, LONG_PTR originalExStyle, BOOL cloakHidden = FALSE, BOOL isClosing = FALSE, UINT closeMsg = 0, HANDLE hWaitFinish = NULL) {
    
    static std::atomic<int> s_animCount{0};
    if (s_animCount.fetch_add(1, std::memory_order_relaxed) % 10 == 0) {
        SweepStaleData();
    }

    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) {
        if (rising) UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else UpdateDwmTransitions(hWnd, TRUE);
        return false;
    }
    RECT rect = winRect, extRect;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extRect, sizeof(extRect)))) {
        rect = extRect;
    }
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int offsetX = rect.left - winRect.left;
    int offsetY = rect.top - winRect.top;
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;
    if (w <= 0 || h <= 0 || rawW <= 0 || rawH <= 0) {
        if (rising) UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else UpdateDwmTransitions(hWnd, TRUE);
        return false;
    }
    bool blocked = g_unloading.load(std::memory_order_relaxed);
    if (!blocked) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        blocked = !g_AnimActive.insert(hWnd).second;
        if (!blocked) g_LaunchSeen.insert(hWnd);
    }
    if (blocked) {
        Wh_Log(L"Animation skipped (busy/unloading) hwnd=%p rising=%d closing=%d", hWnd, rising,
               isClosing);
        if (rising) UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else {
            bool owned;
            { std::lock_guard<std::mutex> lock(g_StateMutex); owned = g_AnimActive.count(hWnd) != 0; }
            if (!owned) UpdateDwmTransitions(hWnd, TRUE);
        }
        return false;
    }
    
    if (g_unloading.load(std::memory_order_relaxed)) {
        if (rising) UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else {
            bool owned;
            { std::lock_guard<std::mutex> lock(g_StateMutex); owned = g_AnimActive.count(hWnd) != 0; }
            if (!owned) UpdateDwmTransitions(hWnd, TRUE);
        }
        return false;
    }
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi; mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0; mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN); mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    DWORD alignVal = 1, dataSize = sizeof(alignVal);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
    int learnedTargetX = (alignVal == 0) ? (mi.rcMonitor.left + 160) : (mi.rcMonitor.left + monWidth / 2);
    POINT pt; GetCursorPos(&pt);
    RECT workArea; MONITORINFO cursorMi; cursorMi.cbSize = sizeof(cursorMi);
    if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &cursorMi)) workArea = cursorMi.rcWork;
    else SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    if (!PtInRect(&workArea, pt)) {
        learnedTargetX = pt.x;
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_TaskbarDockXs[hWnd] = learnedTargetX;
    } else if (!isClosing && g_minRestoreEffectStyle.load(std::memory_order_relaxed) == 0) {
        WCHAR windowTitle[256] = {0};
        GetWindowTextW(hWnd, windowTitle, 256);
        learnedTargetX = GetTaskbarButtonX_Async(hWnd, windowTitle, learnedTargetX, hMon);
    }
    int durationMs = isClosing ? g_closeDurationMs.load(std::memory_order_relaxed)
                               : g_durationMs.load(std::memory_order_relaxed);
    if (!isClosing && g_minRestoreEffectStyle.load(std::memory_order_relaxed) == 1) {
        durationMs = Clamp((durationMs * 6) / 5, 260, 1400);
    }
    auto* data = new WindowAnimData{
        hWnd, nullptr, nullptr, rect, hMon, w, h, learnedTargetX, rising, originalExStyle, cloakHidden, nullptr,
        durationMs,
        isClosing, closeMsg, hWaitFinish
    };
    HDC hScreenDC = GetDC(NULL);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
    data->hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &data->pBits, nullptr, 0);
    if (!data->hBitmap || !data->pBits) {
        Wh_Log(L"Snapshot DIB alloc failed hwnd=%p %dx%d", hWnd, w, h);
        ReleaseDC(NULL, hScreenDC);
        if (rising) UndoRisingHide(hWnd, originalExStyle, cloakHidden); else UpdateDwmTransitions(hWnd, TRUE);
        if (data->hBitmap) DeleteObject(data->hBitmap); delete data;
        { std::lock_guard<std::mutex> lock(g_StateMutex); g_AnimActive.erase(hWnd); }
        return false;
    }
    const bool forceOpaque = ShouldUseBitBlt(hWnd, isClosing);
    auto CopySnapshot = [&](void* sourceBits) {
        auto* src = (DWORD*)sourceBits;
        auto* dst = (DWORD*)data->pBits;
        memset(dst, 0, (size_t)w * h * 4);
        const int startY = std::max(0, -offsetY), endY = std::min(h, rawH - offsetY);
        const int startX = std::max(0, -offsetX), endX = std::min(w, rawW - offsetX);
        if (startY >= endY || startX >= endX) return;
        const int rowPixels = endX - startX;
        const size_t rowBytes = (size_t)rowPixels * 4;
        size_t zeroAlpha = 0;
        for (int y = startY; y < endY; ++y) {
            DWORD* dstRow = dst + (size_t)y * w + startX;
            memcpy(dstRow, src + (size_t)(y + offsetY) * rawW + startX + offsetX, rowBytes);
            if (forceOpaque) {
                for (int x = 0; x < rowPixels; ++x) dstRow[x] |= 0xFF000000u;
            } else {
                for (int x = 0; x < rowPixels; ++x) {
                    if ((dstRow[x] >> 24) == 0) { dstRow[x] = 0; ++zeroAlpha; }
                }
            }
        }
        const size_t inBounds = (size_t)(endY - startY) * (size_t)rowPixels;
        if (zeroAlpha * 2 > inBounds) {
            for (int y = startY; y < endY; ++y) {
                DWORD* dstRow = dst + (size_t)y * w + startX;
                const DWORD* srcRow = src + (size_t)(y + offsetY) * rawW + startX + offsetX;
                for (int x = 0; x < rowPixels; ++x) {
                    if (dstRow[x] == 0) dstRow[x] = 0xFF000000u | (srcRow[x] & 0x00FFFFFFu);
                }
            }
        }
    };
    auto CaptureNow = [&]() -> bool {
        HDC tempDC = CreateCompatibleDC(hScreenDC);
        void* tempBits = nullptr;
        HBITMAP tempBmp = CreateDib32(hScreenDC, rawW, rawH, &tempBits);
        if (!tempDC || !tempBmp || !tempBits) {
            Wh_Log(L"Capture temp DIB alloc failed hwnd=%p %dx%d", hWnd, rawW, rawH);
            if (tempBmp) DeleteObject(tempBmp);
            if (tempDC) DeleteDC(tempDC);
            return false;
        }
        HBITMAP oldBmp = (HBITMAP)SelectObject(tempDC, tempBmp);
        if (forceOpaque)
            BitBlt(tempDC, 0, 0, rawW, rawH, hScreenDC, winRect.left, winRect.top, SRCCOPY);
        else PrintWindow(hWnd, tempDC, PW_RENDERFULLCONTENT);
        GdiFlush();
        CopySnapshot(tempBits);
        SelectObject(tempDC, oldBmp);
        DeleteObject(tempBmp);
        DeleteDC(tempDC);
        return true;
    };
    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto it = g_WndSnapshots.find(hWnd);
            if (it != g_WndSnapshots.end()) {
                SnapCache& c = it->second;
                if (c.w == w && c.h == h) { memcpy(data->pBits, c.pBits, (size_t)w * h * 4); fromCache = TRUE; }
                DeleteObject(c.hBmp); g_WndSnapshots.erase(it);
            }
        }
        if (!fromCache) CaptureNow();
    } else {
        CaptureNow();
        if (!isClosing) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto it = g_WndSnapshots.find(hWnd);
            if (it != g_WndSnapshots.end()) { DeleteObject(it->second.hBmp); g_WndSnapshots.erase(it); }
            void* pCacheBits = nullptr;
            HBITMAP hCacheBmp = CreateDib32(hScreenDC, w, h, &pCacheBits);
            if (hCacheBmp && pCacheBits) {
                memcpy(pCacheBits, data->pBits, (size_t)w * h * 4);
                g_WndSnapshots[hWnd] = { hCacheBmp, pCacheBits, w, h };
            } else if (hCacheBmp) DeleteObject(hCacheBmp);
        }
    }
    ReleaseDC(NULL, hScreenDC);
    HANDLE hFirstShown = NULL;
    if (!rising) {
        hFirstShown = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (hFirstShown && !DuplicateHandle(GetCurrentProcess(), hFirstShown, GetCurrentProcess(), &data->hFirstFrameShown, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hFirstFrameShown = NULL;
        }
    }
    bool waitForFirstFrame = (data->hFirstFrameShown != NULL);
    if (StartWorkerThread(MainAnimThread, data)) {
        Wh_Log(L"Animation start hwnd=%p kind=%s dockX=%d duration=%d", hWnd,
               isClosing ? L"close" : (rising ? L"restore" : L"minimize"), learnedTargetX,
               data->durationMs);
        if (hFirstShown) {
            const DWORD firstFrameWaitMs =
                (!rising && !isClosing &&
                 g_minRestoreEffectStyle.load(std::memory_order_relaxed) == 1)
                    ? 500
                    : 200;
            if (waitForFirstFrame) WaitForSingleObject(hFirstShown, firstFrameWaitMs);
            CloseHandle(hFirstShown);
        }
        if (isClosing) SetWindowCloak(hWnd, TRUE);
        return true;
    }
    Wh_Log(L"Animation worker failed hwnd=%p kind=%s", hWnd,
           isClosing ? L"close" : (rising ? L"restore" : L"minimize"));
    if (rising) UndoRisingHide(hWnd, data->originalExStyle, data->hiddenByCloak); else UpdateDwmTransitions(hWnd, TRUE);
    if (hFirstShown) CloseHandle(hFirstShown); if (data->hFirstFrameShown) CloseHandle(data->hFirstFrameShown);
    DeleteObject(data->hBitmap); delete data;
    { std::lock_guard<std::mutex> lock(g_StateMutex); g_AnimActive.erase(hWnd); }
    return false;
}
DWORD WINAPI LaunchAnimThread(LPVOID lpParam);
static bool IsMinimizeCommand(int cmd) {
    return cmd == SW_MINIMIZE || cmd == SW_SHOWMINIMIZED || cmd == SW_SHOWMINNOACTIVE;
}
static bool IsLaunchCommand(int cmd) {
    return cmd == SW_SHOW || cmd == SW_SHOWNORMAL || cmd == SW_SHOWDEFAULT || cmd == SW_SHOWMAXIMIZED;
}
static void WaitForCloseAnimation(HANDLE wait) {
    const DWORD timeoutMs = (DWORD)std::max(AnimConstants::WaitTimeoutMs,
                                            g_closeDurationMs.load(std::memory_order_relaxed) +
                                                AnimConstants::WaitSlackMs);
    const DWORD deadline = GetTickCount() + timeoutMs;
    for (;;) {
        const DWORD now = GetTickCount();
        if ((LONG)(now - deadline) >= 0) break;
        const DWORD r = MsgWaitForMultipleObjectsEx(1, &wait, deadline - now, QS_SENDMESSAGE,
                                                    MWMO_INPUTAVAILABLE);
        if (r != WAIT_OBJECT_0 + 1) break;
        MSG msg;
        PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    }
}
static bool RunCloseAnimation(HWND hWnd, UINT closeMsg) {
    UpdateDwmTransitions(hWnd, FALSE);
    HANDLE wait = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE workerWait = NULL;
    
    if (wait && !DuplicateHandle(GetCurrentProcess(), wait, GetCurrentProcess(), &workerWait, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        CloseHandle(wait);
        wait = nullptr;
    }
    
    const bool started = StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE, TRUE, closeMsg, workerWait);
    if (!started) Wh_Log(L"Close animation did not start hwnd=%p msg=0x%04X", hWnd, closeMsg);
    
    if (started) {
        if (wait) WaitForCloseAnimation(wait);
    } else {
        if (workerWait) CloseHandle(workerWait);
    }
    
    if (wait) CloseHandle(wait);
    return started;
}
static void TryMinimizeAnim(HWND hWnd) {
    if (!g_minimizeAnimation.load(std::memory_order_relaxed)) return;
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return;
    if (!ShouldAnimateWindow(hWnd)) return;
    UpdateDwmTransitions(hWnd, FALSE);
    StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
}
static bool PrepareLaunchAnim(HWND hWnd, int nCmdShow, LONG_PTR* origExOut) {
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    if (!g_launchAnimation.load(std::memory_order_relaxed)) return false;
    if (!IsLaunchCommand(nCmdShow)) return false;
    if (IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    if (!IsLaunchWindow(hWnd)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) return false;
    { std::lock_guard<std::mutex> lock(g_StateMutex); if (!g_LaunchSeen.insert(hWnd).second) return false; }
    UpdateDwmTransitions(hWnd, FALSE);
    *origExOut = exStyle;
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    return true;
}
static void CommitLaunchAnim(HWND hWnd, LONG_PTR originalExStyle) {
    auto* ld = new LaunchAnimData{hWnd, originalExStyle};
    if (!StartWorkerThread(LaunchAnimThread, ld)) {
        delete ld;
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
        UpdateDwmTransitions(hWnd, TRUE);
    }
}
static bool IsOurWindow(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int cmd) {
    if (!IsOurWindow(hWnd)) return ShowWindow_Original(hWnd, cmd);
    if (cmd == SW_SHOW || cmd == SW_SHOWNORMAL || cmd == SW_SHOWMAXIMIZED || cmd == SW_RESTORE || 
        cmd == SW_SHOWDEFAULT || cmd == SW_SHOWMINIMIZED || cmd == SW_SHOWMINNOACTIVE || 
        cmd == SW_SHOWNA || cmd == SW_SHOWNOACTIVATE) {
        EnsureWinEventThreadStarted();
    }
    if (cmd == SW_HIDE) {
        if (GetPropW(hWnd, kPropCloseBypass)) return ShowWindow_Original(hWnd, cmd);
        if (ShouldTreatHideAsClose(hWnd)) {
            BOOL wasVisible = IsWindowVisible(hWnd);
            if (RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return wasVisible;
        }
    }
    if (IsMinimizeCommand(cmd)) {
        if (g_minimizeAnimation.load(std::memory_order_relaxed) && ShouldAnimateWindow(hWnd)) {
            UpdateDwmTransitions(hWnd, FALSE);
            StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
        }
        return ShowWindow_Original(hWnd, cmd);
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) && IsIconic(hWnd)) {
        if (g_restoreAnimation.load(std::memory_order_relaxed) && ShouldAnimateWindow(hWnd)) {
            UpdateDwmTransitions(hWnd, FALSE);
            SetWindowCloak(hWnd, TRUE);
            BOOL result = ShowWindow_Original(hWnd, cmd);
            StartAnimation(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return result;
        }
        return ShowWindow_Original(hWnd, cmd);
    }
    LONG_PTR originalStyle;
    if (PrepareLaunchAnim(hWnd, cmd, &originalStyle)) {
        BOOL result = ShowWindow_Original(hWnd, cmd);
        CommitLaunchAnim(hWnd, originalStyle);
        return result;
    }
    return ShowWindow_Original(hWnd, cmd);
}
BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int cmd) {
    if (!IsOurWindow(hWnd)) return ShowWindowAsync_Original(hWnd, cmd);
    if (cmd == SW_SHOW || cmd == SW_SHOWNORMAL || cmd == SW_SHOWMAXIMIZED || cmd == SW_RESTORE || 
        cmd == SW_SHOWDEFAULT || cmd == SW_SHOWMINIMIZED || cmd == SW_SHOWMINNOACTIVE || 
        cmd == SW_SHOWNA || cmd == SW_SHOWNOACTIVATE) {
        EnsureWinEventThreadStarted();
    }
    if (cmd == SW_HIDE) {
        if (GetPropW(hWnd, kPropCloseBypass)) return ShowWindowAsync_Original(hWnd, cmd);
        if (ShouldTreatHideAsClose(hWnd) && RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return TRUE;
    }
    if (IsMinimizeCommand(cmd)) TryMinimizeAnim(hWnd);
    else {
        LONG_PTR originalStyle;
        if (PrepareLaunchAnim(hWnd, cmd, &originalStyle)) {
            BOOL result = ShowWindowAsync_Original(hWnd, cmd);
            CommitLaunchAnim(hWnd, originalStyle);
            return result;
        }
    }
    return ShowWindowAsync_Original(hWnd, cmd);
}
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND insertAfter, int x, int y, int cx, int cy, UINT flags) {
    if (!(flags & (SWP_HIDEWINDOW | SWP_SHOWWINDOW))) {
        return SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
    }
    if (!IsOurWindow(hWnd)) return SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
    if (flags & SWP_SHOWWINDOW) {
        EnsureWinEventThreadStarted();
    }
    if ((flags & SWP_HIDEWINDOW) && !GetPropW(hWnd, kPropCloseBypass) && ShouldTreatHideAsClose(hWnd)) {
        
        BOOL applied = SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags & ~SWP_HIDEWINDOW);
        if (!applied) return FALSE;
        
        if (RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return TRUE;
        
        return SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
    }
    if (flags & SWP_SHOWWINDOW) {
        LONG_PTR originalStyle;
        if (PrepareLaunchAnim(hWnd, SW_SHOW, &originalStyle)) {
            BOOL result = SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
            CommitLaunchAnim(hWnd, originalStyle);
            return result;
        }
    }
    return SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
}
BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    bool animated = false;

    if (GetWindowThreadProcessId(hWnd, NULL) == GetCurrentThreadId() &&
        g_closeAnimation.load(std::memory_order_relaxed) &&
        IsAppMainWindow(hWnd) &&
        !IsAnimating(hWnd) &&
        !GetPropW(hWnd, kPropCloseBypass) &&
        !GetPropW(hWnd, kPropClosed)) {
        SetPropW(hWnd, kPropCloseBypass, (HANDLE)1);
        animated = RunCloseAnimation(hWnd, WM_DESTROY);
        if (!animated) RemovePropW(hWnd, kPropCloseBypass);
    }

    BOOL result = DestroyWindow_Original(hWnd);

    if (!result && animated && IsWindow(hWnd)) {
        RemovePropW(hWnd, kPropCloseBypass);
        RemovePropW(hWnd, kPropClosed);
        SetWindowCloak(hWnd, FALSE);
        UpdateDwmTransitions(hWnd, TRUE);
    }

    return result;
}
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg != WM_DESTROY && msg != WM_CLOSE && msg != WM_SYSCOMMAND) {
        return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
    }
    if (!IsOurWindow(hWnd)) return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
    if (msg == WM_DESTROY) {
        RemovePropW(hWnd, kPropCloseBypass);
        RemovePropW(hWnd, kPropClosed);
        CleanupWindowData(hWnd);
    }
    const bool closeMessage = msg == WM_CLOSE || (msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE);
    if (closeMessage && !IsAnimating(hWnd) && !GetPropW(hWnd, kPropCloseBypass) && !GetPropW(hWnd, kPropClosed) &&
        g_closeAnimation.load(std::memory_order_relaxed) && IsAppMainWindow(hWnd) && UseSafeClose(hWnd)) {
        SetPropW(hWnd, kPropClosed, (HANDLE)1);
        const UINT repost = msg == WM_CLOSE ? WM_CLOSE : WM_SYSCOMMAND;
        if (RunCloseAnimation(hWnd, repost)) return 0;
        RemovePropW(hWnd, kPropClosed);
    }
    if (msg == WM_SYSCOMMAND) {
        const UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            if (g_minimizeAnimation.load(std::memory_order_relaxed) && ShouldAnimateWindow(hWnd)) {
                UpdateDwmTransitions(hWnd, FALSE);
                StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
            }
            return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
        }
        if (cmd == SC_RESTORE && IsIconic(hWnd) && g_restoreAnimation.load(std::memory_order_relaxed) && ShouldAnimateWindow(hWnd)) {
            UpdateDwmTransitions(hWnd, FALSE);
            SetWindowCloak(hWnd, TRUE);
            LRESULT result = DefWindowProcW_Original(hWnd, msg, wParam, lParam);
            StartAnimation(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return result;
        }
    }
    return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
}
BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd, const WINDOWPLACEMENT* placement) {
    if (!IsOurWindow(hWnd)) return SetWindowPlacement_Original(hWnd, placement);
    if (placement) {
        if (IsMinimizeCommand(placement->showCmd)) TryMinimizeAnim(hWnd);
        else if (placement->showCmd == SW_HIDE && !GetPropW(hWnd, kPropCloseBypass) &&
                 ShouldTreatHideAsClose(hWnd)) {
            
            WINDOWPLACEMENT modified = *placement;
            modified.showCmd = SW_SHOWNA;
            BOOL applied = SetWindowPlacement_Original(hWnd, &modified);
            
            if (!applied) return FALSE;
            
            if (RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return TRUE;
            
            return ShowWindow_Original(hWnd, SW_HIDE);
        }
    }
    return SetWindowPlacement_Original(hWnd, placement);
}
BOOL WINAPI CloseWindow_Hook(HWND hWnd) {
    if (!IsOurWindow(hWnd)) return CloseWindow_Original(hWnd);
    TryMinimizeAnim(hWnd);
    return CloseWindow_Original(hWnd);
}
DWORD WINAPI LaunchAnimThread(LPVOID lpParam) {
    LaunchAnimData* ld = (LaunchAnimData*)lpParam;
    HWND hWnd = ld->hWnd; LONG_PTR originalExStyle = ld->originalExStyle; delete ld;
    Sleep(60);
    for (int i = 0; i < 30; ++i) {
        if (!IsWindow(hWnd) || g_unloading.load(std::memory_order_relaxed)) break;
        UINT cloaked = 0; if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) || !cloaked) break; Sleep(50);
    }
    if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(hWnd) || IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        if (IsWindow(hWnd)) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
            if (!(originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            }
            UpdateDwmTransitions(hWnd, TRUE);
        }
        return 0;
    }
    StartAnimation(hWnd, TRUE, originalExStyle);
    return 0;
}
static BOOL CALLBACK EnumWindowsInitProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        if (IsWindowVisible(hWnd)) {
            EnsureWinEventThreadStarted();
            return FALSE; 
        }
    }
    return TRUE;
}
void StartSwitchThreads() {
    if (IsExplorerProcess() && !g_hAltTabThread) {
        g_altTabThreadRunning.store(true, std::memory_order_relaxed);
        g_hAltTabThread = CreateThread(NULL, 0, AltTabTrackerThread, NULL, 0, NULL);
    }
    EnumWindows(EnumWindowsInitProc, 0);
}
void StopSwitchThreads() {
    if (g_hAltTabThread) {
        g_altTabThreadRunning.store(false, std::memory_order_relaxed);
        WaitForSingleObject(g_hAltTabThread, INFINITE);
        CloseHandle(g_hAltTabThread);
        g_hAltTabThread = NULL;
    }
    
    HANDLE hWinEvent = NULL;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        hWinEvent = g_hWinEventThread;
        g_hWinEventThread = NULL;
    }
    
    if (hWinEvent) {
        DWORD tid = GetThreadId(hWinEvent);
        while (tid && !PostThreadMessageW(tid, WM_QUIT, 0, 0)) {
            if (WaitForSingleObject(hWinEvent, 10) != WAIT_TIMEOUT) break;
        }
        WaitForSingleObject(hWinEvent, INFINITE);
        CloseHandle(hWinEvent);
        g_winEventThreadStarted.store(false, std::memory_order_relaxed);
    }
}
BOOL Wh_ModInit() {
    LoadAnimSettings();
    if (!g_minimizeAnimation.load(std::memory_order_relaxed) &&
        !g_restoreAnimation.load(std::memory_order_relaxed) &&
        !g_closeAnimation.load(std::memory_order_relaxed) &&
        !g_switchAnimation.load(std::memory_order_relaxed) &&
        !g_launchAnimation.load(std::memory_order_relaxed)) {
        Wh_Log(L"Init skipped: all animation toggles are off");
        return FALSE;
    }
    InitSharedMemory();
    const bool explorerProcess = IsExplorerProcess();
    if (explorerProcess) ResetAltTabState();
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook, (void**)&ShowWindowAsync_Original);
    Wh_SetFunctionHook((void*)SetWindowPlacement, (void*)SetWindowPlacement_Hook, (void**)&SetWindowPlacement_Original);
    Wh_SetFunctionHook((void*)CloseWindow, (void*)CloseWindow_Hook, (void**)&CloseWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    Wh_SetFunctionHook((void*)DestroyWindow, (void*)DestroyWindow_Hook, (void**)&DestroyWindow_Original);
    
    if (g_switchAnimation.load(std::memory_order_relaxed)) {
        StartSwitchThreads();
    }
    Wh_Log(L"Init ok minimize=%d restore=%d close=%d switch=%d launch=%d explorer=%d",
           g_minimizeAnimation.load(std::memory_order_relaxed),
           g_restoreAnimation.load(std::memory_order_relaxed),
           g_closeAnimation.load(std::memory_order_relaxed),
           g_switchAnimation.load(std::memory_order_relaxed),
           g_launchAnimation.load(std::memory_order_relaxed), explorerProcess);
    return TRUE;
}
void Wh_ModSettingsChanged() { 
    bool wasSwitchAnim = g_switchAnimation.load(std::memory_order_relaxed);
    LoadAnimSettings(); 
    bool isSwitchAnim = g_switchAnimation.load(std::memory_order_relaxed);
    Wh_Log(L"Settings changed switch=%d->%d", wasSwitchAnim, isSwitchAnim);

    if (isSwitchAnim && !wasSwitchAnim) {
        StartSwitchThreads();
    } 
    else if (!isSwitchAnim && wasSwitchAnim) {
        StopSwitchThreads();
    }
}
void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit: joining workers");
    g_unloading.store(true, std::memory_order_relaxed);
    StopSwitchThreads();
    JoinWorkerThreads();
}
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    std::vector<HWND> stuck;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        stuck.assign(g_AnimActive.begin(), g_AnimActive.end());
        g_AnimActive.clear();
    }
    for (HWND hWnd : stuck) {
        if (!IsWindow(hWnd)) continue;
        SetWindowCloak(hWnd, FALSE);
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        UpdateDwmTransitions(hWnd, TRUE);
        RemovePropW(hWnd, kPropCloseBypass);
        RemovePropW(hWnd, kPropClosed);
    }

    std::lock_guard<std::mutex> lock(g_StateMutex);
    for (auto& pair : g_WndSnapshots) DeleteObject(pair.second.hBmp);
    g_WndSnapshots.clear(); g_TaskbarDockXs.clear(); g_ProcessDockXs.clear(); g_ProcessNameCache.clear(); g_LaunchSeen.clear();
    if (g_hMapFile) {
        UnmapViewOfFile(g_pSharedState);
        CloseHandle(g_hMapFile);
        g_hMapFile = NULL;
    }
}