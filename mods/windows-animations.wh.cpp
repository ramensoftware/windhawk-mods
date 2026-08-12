// ==WindhawkMod==
// @id              windows-animations
// @name            Windows Animations
// @description     Smooth minimize, restore, close, switch animations for windows.
// @version         1.2.0
// @author          ReDrag
// @github          https://github.com/redrag2105
// @include         *
// @exclude         TextInputHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         dwm.exe
// @license         MIT
// @compilerOptions -ladvapi32 -ldwmapi -lgdi32 -lole32 -loleaut32 -lshell32 -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Animations
> ⚠️ **NOTE:** 
> * **UWP Apps:** Close animations will not work with UWP applications (e.g., Settings, Calculator, MS Store) because they use a restricted modern windowing framework that prevents standard visual interception and hooking.
> * **Browsers & Tray Apps:** Applications like Chrome, Edge, Discord, and Windhawk do not truly close when you click the 'X' button; they simply hide in the background. To see close animations for these apps, you must turn on the **"Animate windows hidden to the tray"** option in the settings.
> * **Game Launchers:** Previously, the "Animate windows hidden to the tray" option could cause game launchers (like GOG Galaxy) or games to exit immediately after starting. This bug has been patched and is highly unlikely to happen again, but if you experience similar issues, try disabling this option.

Welcome to **Windows Animations**, a comprehensive window transition suite for your desktop. Built from the ground up to deliver cinematic window animations. 

By utilizing a smart Hybrid Rendering Engine, this mod bridges the gap between stunning visual aesthetics and seamless execution.

## ✨ Key Features

* **🚀 Smart Hybrid Engine:** Intelligently seamlessly switches between lightning-fast GDI (for pixel-perfect destruction physics) and the native DWM Thumbnail API (to perfectly preserve Windows 11 rounded corners and drop shadows).

* **🎬 Cinematic Close Effects:** Transform how you close applications with six physics-based animations:
  * **Square Shatter:** The window violently explodes outward into digital blocks before drifting into the void.
    
    ![Square Shatter Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/a7e46c466c7b88552d5d92cad113b652fbd3f10e/shatter_close.gif)

  * **Thanos Snap:** A disintegration wave sweeps across the window, turning it into thousands of tiny dust particles that curve away into the wind.
    
    ![Thanos Snap Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/0e0508083d6c3108b2b3da8c5f0140f01cd21e37/close_preview.gif)

  * **Perlin Dissolve:** The window organically melts and dissolves into thin air using a smooth Perlin noise map.
    
    ![Perlin Dissolve Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/a7e46c466c7b88552d5d92cad113b652fbd3f10e/perlin_close.gif)

  * **Cyber Glitch:** Signal-snow close — horizontal band tears and a fine noise dissolve that eats the window away.

    ![Cyber Glitch Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/cyber_glitch.gif)

  * **Retro TV Off:** Classic CRT power-down — the window squashes into a bright horizontal beam, zips to a glowing center dot, then phosphor-fades to black.

    ![Retro TV Off Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/tv_retro.gif)

  * **Pixel Melt:** The window melts downward like hot wax — staggered pixel columns drip at different speeds into jagged streaks.

    ![Pixel Melt Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/pixel_melt.gif)

* **🧞 Fluid Minimize & Restore:** Transform how windows minimize and restore with eight fluid, physics-inspired animations:

  * **Genie:** The window is pulled toward the taskbar like a classic macOS Genie effect, bending and compressing smoothly into its destination.

    ![Genie Minimize Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/a7e46c466c7b88552d5d92cad113b652fbd3f10e/genie_preview.gif)

  * **Windows 10:** The window follows the classic Windows 10 thumbnail transition, smoothly collapsing into its taskbar button and expanding back from it.

  * **Ink Splash:** The window collapses into a fluid splash of ink, spreading and deforming organically before fading away.

    ![Ink Splash Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ff3a17e818f2d08e43ee4f79059b4ccb3c663cb0/ink_splash.gif)

  * **Mirage:** The window ripples and distorts like a heat haze, gradually dissolving into a soft, shimmering mirage.

    ![Mirage Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/mirage.gif)

  * **Scorch:** The window breaks into vertical stripes that smoothly slide upwards or shrink until they completely disappear.

    ![Scorch Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/scorch.gif)

  * **Splinter:** The window transitions through a grid of overlapping horizontal and vertical stripes that piece the window together or tear it apart.

    ![Splinter Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/splinter.gif)

  * **Stipple:** The window breaks down into a dense field of tiny dots, gradually dispersing until the entire surface disappears.

    ![Stipple Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/stipple.gif)

  * **Swell:** The window expands and bulges outward with a soft, elastic distortion before smoothly fading away.

    ![Swell Preview](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/b96dea88ab53f4b781e472d3683f645f4e368f8e/swell.gif)

* **🔄 Soft Switch Animation:** A pristine scale and fade-in animation triggered *exclusively* when actively switching windows via the Alt+Tab menu.
  * **Before:**
    
    ![Alt Tab Switch Before](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ba4e9efd647c954eb619ec2181d5435a80af7b15/switch_before.gif)
  * **After:**
    
    ![Alt Tab Switch After](https://raw.githubusercontent.com/redrag2105/windhawk-windows-animations-preview/ba4e9efd647c954eb619ec2181d5435a80af7b15/switch_after.gif)
* **🛡️ Rock-Solid Stability:** Features lifecycle management for System Tray apps (like Discord, Steam, etc.)—handling background apps gracefully to minimize ghosting or stuck transparency.

## ⚙️ Customization & Settings

You can deeply customize the feel and pacing of every animation via the Windhawk Settings tab:

* **Minimize/Restore Animation Style:** Choose between Genie, Windows 10 (collapse to taskbar button), Ink Splash, Scorch, Splinter, Mirage, Stipple, and Swell. Enable **Random animation style** to pick one style for each minimize/restore pair; restoring reuses the style selected when that window minimized (ignores the dropdown).
* **Close Animation Effect:** Dropdown menu to switch between 'Square Shatter', 'Thanos Snap', 'Perlin Dissolve', 'Cyber Glitch', 'Retro TV Off', and 'Pixel Melt'. Enable **Random animation style** to pick one at random each time (ignores the dropdown).
* **Minimize/Restore duration (ms):** Controls Genie speed directly; each style applies a small pace tweak. Windows 10 style uses a fixed system-like timing and ignores this setting. (Default: 360ms, clamp 200–1400)
* **Close animation duration (ms):** Controls how long the dramatic close animation lasts. (Default: 630ms)
* **Switch animation duration (ms):** Controls the snappy speed of the Alt+Tab scaling effect. (Default: 200ms)
* **Shatter block size (px):** Determines the size of the dust/shatter particles. 
  * *Performance Tip:* Smaller values (e.g., 1, 2) create hyper-realistic pixel dust but require more CPU power. Larger values (24, 32) yield a stylish retro pixelated shatter and perform effortlessly on any hardware. (Clamped strictly to 1-100).
* **Animate app launches:** Off by default. Enable it to use the restore effect when an application window first opens.
* **Animate windows hidden to the tray:** Off by default. Enable it to animate apps such as Discord, Steam, and Telegram when they hide their window instead of closing it. This can also animate splash screens or windows hidden automatically by an app. Leave off if game launchers (GOG Galaxy, etc.) or games exit right after starting.
* **Toggles:** Individually turn on/off Minimize, Restore, Close, Alt+Tab Switch, and Launch animations to suit your workflow.
* **Reveal taskbar during Genie (auto-hide):** If the taskbar is already visible or hovered, Genie stays behind it without changing focus. Otherwise, Genie briefly reveals it and defers the real minimize until the animation finishes. Ignored for Windows 10, Ink Splash, Scorch, Splinter, Mirage, Stipple, and Swell.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minimize_restore:
  - minimize_animation: true
    $name: Animate minimizing
    $description: Animate windows as they minimize to the taskbar.
  - restore_animation: true
    $name: Animate restoring
    $description: Animate windows as they return from the taskbar.
  - launch_animation: false
    $name: Animate app launches
    $description: Disabled by default. Enable to use the restore animation when an application window first opens.
  - random_effect: false
    $name: Random animation style
    $description: >-
      Pick one random effect for each minimize/restore pair. Restoring reuses the effect selected
      when that window minimized, and rapid reversals keep the same effect. When enabled, the
      Animation style setting below is ignored.
  - effect_style: genie
    $name: Animation style
    $description: >-
      Choose the effect used for minimizing and restoring windows. Ignored when Random animation
      style is enabled.
    $options:
    - genie: Genie — Taskbar suck
    - windows10: Windows 10 — Collapse to taskbar
    - ink_splash: Ink Splash — Organic blot
    - scorch: Scorch — Corner burn
    - splinter: Splinter — Center crack
    - mirage: Mirage — Wavy shimmer
    - stipple: Stipple — Dotted bloom
    - swell: Swell — Expand and fade
  - duration_ms: 360
    $name: Duration (ms)
    $description: >-
      Base animation duration, from 200 to 1400 ms. Non-Genie effects apply a small pacing
      adjustment. Windows 10 style ignores this setting and uses a fixed system-like timing.
  - unhide_taskbar: false
    $name: Reveal the taskbar during Genie
    $description: >-
      When taskbar auto-hide is enabled, keep an already visible or hovered taskbar above the
      Genie effect without changing focus. If it is hidden, briefly reveal it and defer the real
      minimize until the animation finishes. Off by default. No effect on other animation styles.
  - unhide_duration_ms: 450
    $name: Taskbar reveal duration (ms)
    $description: >-
      How long to keep the taskbar revealed before the deferred Genie minimize commits, from 0
      to 5000 ms.
  $name: Minimize and restore
  $description: Configure animations for minimizing, restoring, and opening windows.
- close:
  - close_animation: true
    $name: Animate closing
    $description: Play a cinematic effect when an application window closes.
  - random_effect: false
    $name: Random animation style
    $description: >-
      Pick a random close effect each time. When enabled, the Animation style setting below is
      ignored.
  - effect_style: thanos
    $name: Animation style
    $description: >-
      Choose the effect used when closing a window. Ignored when Random animation style is enabled.
    $options:
    - shatter: Square Shatter — Explosion
    - thanos: Thanos Snap — Disintegration wave
    - perlin: Perlin Dissolve — Acid burn
    - glitch: Cyber Glitch — Digital teleport
    - tv_off: Retro TV Off — CRT power-down
    - pixel_melt: Pixel Melt — Dripping paint
  - duration_ms: 630
    $name: Duration (ms)
    $description: >-
      Close animation duration, from 50 to 5000 ms. The closing app's UI thread is blocked for
      this time (only sent messages are pumped).
  - hide_as_close: false
    $name: Animate windows hidden to the tray
    $description: >-
      Disabled by default. Enable to animate apps such as Discord, Steam, and Telegram when they
      hide their window instead of closing it. This can also animate splash screens, such as the
      Discord updater, or windows hidden automatically by an app, such as the Google Chrome
      profile picker. Leave this off if game launchers (e.g. GOG Galaxy) or games exit unexpectedly
      when starting — those apps often hide a window as part of launching.
  - shatter_block_size: 5
    $name: Dust and shatter block size (px)
    $description: >-
      Particle size, from 1 to 100 px. Smaller particles look finer but use more CPU and memory;
      values from 1 to 4 can be especially demanding on 4K displays.
  $name: Close
  $description: Configure the effects played when windows close or hide (system tray app).
- window_switch:
  - switch_animation: true
    $name: Animate Alt+Tab switching
    $description: Play a scale-and-fade animation when switching windows with Alt+Tab.
  - duration_ms: 200
    $name: Duration (ms)
    $description: Window-switch animation duration, from 50 to 1000 ms.
  $name: Window switch
  $description: Configure the animation used when selecting a window through Alt+Tab.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
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
#include <new>
#include <windhawk_utils.h>
#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif
#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif
#ifndef EVENT_OBJECT_HOSTEDOBJECTSINVALIDATED
#define EVENT_OBJECT_HOSTEDOBJECTSINVALIDATED 0x8020
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

enum class NativeMinimizeState : LONG {
    Pending,
    Submitting,
    Cancelled,
    Failed,
    SyncCompleted,
    AsyncSubmitted,
};

struct NativeMinimizeBarrier {
    std::atomic<LONG> references{1};
    HANDLE submitted{};
    std::atomic<NativeMinimizeState> state{NativeMinimizeState::Pending};
};

static NativeMinimizeBarrier* CreateNativeMinimizeBarrier() {
    auto* barrier = new (std::nothrow) NativeMinimizeBarrier{};
    if (!barrier) return nullptr;
    barrier->submitted = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!barrier->submitted) {
        delete barrier;
        return nullptr;
    }
    return barrier;
}

static void AddRefNativeMinimizeBarrier(NativeMinimizeBarrier* barrier) {
    if (barrier) barrier->references.fetch_add(1, std::memory_order_relaxed);
}

static void ReleaseNativeMinimizeBarrier(NativeMinimizeBarrier* barrier) {
    if (!barrier || barrier->references.fetch_sub(1, std::memory_order_acq_rel) != 1) return;
    CloseHandle(barrier->submitted);
    delete barrier;
}

static void CompleteNativeMinimizeBarrier(NativeMinimizeBarrier* barrier,
                                          NativeMinimizeState state) {
    if (!barrier) return;
    if (barrier->state.load(std::memory_order_acquire) != NativeMinimizeState::Cancelled) {
        barrier->state.store(state, std::memory_order_release);
    }
    SetEvent(barrier->submitted);
    ReleaseNativeMinimizeBarrier(barrier);
}

static bool BeginNativeMinimizeSubmission(NativeMinimizeBarrier* barrier) {
    if (!barrier) return true;
    NativeMinimizeState expected = NativeMinimizeState::Pending;
    return barrier->state.compare_exchange_strong(
        expected, NativeMinimizeState::Submitting, std::memory_order_acq_rel);
}

struct NativeMinimizeBarrierOwner {
    NativeMinimizeBarrier* barrier{};
    explicit NativeMinimizeBarrierOwner(NativeMinimizeBarrier* value) : barrier(value) {}
    ~NativeMinimizeBarrierOwner() { ReleaseNativeMinimizeBarrier(barrier); }
    NativeMinimizeBarrier* release() {
        NativeMinimizeBarrier* value = barrier;
        barrier = nullptr;
        return value;
    }
};

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
    BOOL requestedUnhide{};
    HWND hNextApp{};
    int unhideDurationMs{};
    BOOL deferredMinimize{};
    int deferredShowCmd{};
    int effectStyle{};
    BOOL restoreMaximized{};
    ULONG_PTR pairedEffectToken{};
    ULONG_PTR launchAnimationToken{};
    BOOL taskbarFocusBorrowed{};
    BOOL nativeAsyncMinimizeObserved{};
    BOOL nativeStateTimedOut{};
    NativeMinimizeBarrier* nativeMinimizeBarrier{};
};
struct LaunchAnimData {
    HWND hWnd;
    LONG_PTR originalExStyle;
    ULONG_PTR snapshotToken;
};
struct AsyncRestoreAnimData {
    HWND hWnd;
    LONG_PTR originalExStyle;
    uint64_t reservationGeneration;
    BOOL restoreMaximized;
};
struct SwitchAnimData { HWND hWnd; int durationMs; };
struct SnapCache {
    HBITMAP hBmp;
    void* pBits;
    int w;
    int h;
    size_t bytes;
    uint64_t lastUsed;
    ULONG_PTR windowToken;
};
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
    constexpr float CrtSquashEnd = 0.42f;
    constexpr float CrtZipEnd = 0.78f;
    constexpr int WaitTimeoutMs = 2500;
    constexpr int WaitSlackMs = 500;
    constexpr int NativeStateWaitMs = 2500;
    constexpr int MaximizedRestoreGuardMs = 3000;
    constexpr int AltTabPollMs = 16;
    constexpr int AltTabSessionMs = 500;
    constexpr int UiaLookupWaitMs = 150;
    constexpr int UiaLookupInFlightMs = 3000;
    constexpr int UiaNegativeCacheMs = 10000;
    constexpr int UiaPositiveFallbackCacheMs = 10000;
    constexpr int UiaMinAcceptScore = 400;
    constexpr int TaskbarExpandWaitMs = 250;
    constexpr int Win10MinRestoreMs = 280;
    constexpr size_t SnapshotCacheMaxEntries = 3;
    constexpr size_t SnapshotCacheMaxBytes = 64ull * 1024ull * 1024ull;
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
static constexpr PCWSTR kPropMinRestorePair =
    L"windows-animations.MinRestorePairV1";
static constexpr PCWSTR kPropMaximizedRestoreGuard =
    L"windows-animations.MaximizedRestoreGuardV1";
static constexpr PCWSTR kPropSnapshotCache =
    L"windows-animations.SnapshotCacheV1";
static constexpr PCWSTR kPropLaunchAnimation =
    L"windows-animations.LaunchAnimationV1";
// The HWND properties bridge taskbar-side Explorer hooks and the target app's
// hooks. The pair token's low nibble stores style+1; the remaining bits are a
// nonce so an old worker can't clear a newer pair.
std::atomic<ULONG_PTR> g_NextMinRestorePairToken{0};
std::atomic<ULONG_PTR> g_NextSnapshotCacheToken{0};
std::unordered_map<HWND, SnapCache> g_WndSnapshots;
size_t g_WndSnapshotBytes = 0;
uint64_t g_NextSnapshotCacheSerial = 0;
std::unordered_map<HWND, int> g_TaskbarDockXs;
std::unordered_map<std::wstring, int> g_ProcessDockXs;
std::unordered_map<HWND, uint64_t> g_TaskbarDockLookupGenerations;
std::unordered_map<HWND, DWORD> g_TaskbarDockLookupStartedTicks;
std::unordered_map<HWND, DWORD> g_TaskbarDockNegativeUntilTicks;
std::unordered_map<HWND, DWORD> g_TaskbarDockPositiveUntilTicks;
uint64_t g_NextTaskbarDockLookupGeneration = 0;
uint64_t g_TaskbarWindowSetSignature = 0;
bool g_TaskbarWindowSetSignatureInitialized = false;
LONG g_seenTaskbarLayoutEpoch = 0;
DWORD g_seenTaskbarExplorerPid = 0;
std::unordered_map<HWND, std::wstring> g_ProcessNameCache;
std::unordered_set<HWND> g_LaunchSeen;
std::unordered_set<HWND> g_AnimActive;
std::unordered_map<HWND, bool> g_AnimWantRising;
std::unordered_map<HWND, HWND> g_AnimRestoreRequestForeground;
std::unordered_map<HWND, uint64_t> g_AsyncRestoreReservations;
uint64_t g_NextAsyncRestoreReservation = 0;
std::mutex g_StateMutex;
std::atomic<HWINEVENTHOOK> g_hForegroundHook{NULL};
std::atomic<HWINEVENTHOOK> g_hTaskbarLayoutHook{NULL};
std::atomic<HWINEVENTHOOK> g_hTaskbarLocationHook{NULL};
std::atomic<HWINEVENTHOOK> g_hTaskbarHostedObjectsHook{NULL};
std::atomic<HWINEVENTHOOK> g_hExplorerFgHook{NULL};
HANDLE g_hAltTabSessionThread = NULL;
HANDLE g_hExplorerFgThread = NULL;
HANDLE g_hWinEventThread = NULL;
std::mutex g_AltTabSessionMutex;
std::mutex g_ExplorerFgThreadMutex;
std::atomic<bool> g_winEventThreadStarted{false};
std::atomic<bool> g_explorerAltTabTrackerEnabled{false};
std::atomic<bool> g_altTabSessionPollRunning{false};
std::atomic<HWND> g_lastAppForeground{nullptr};
DWORD WINAPI WinEventHookThread(LPVOID lpParam);
DWORD WINAPI ExplorerFgHookThread(LPVOID lpParam);
DWORD WINAPI AltTabSessionPollThread(LPVOID lpParam);
static void EnsureExplorerForegroundThreadStarted();
struct alignas(8) SharedAnimState {
    volatile LONG magic;
    volatile LONG sessionEpoch;
    volatile LONG heartbeatTick;
    volatile LONG lastAltTabTick;
    volatile LONG altTabSourceWindow;
    volatile LONG altTabStartTick;
    volatile LONG altTabGeneration;
    volatile LONG taskbarLayoutEpoch;
    volatile LONG taskbarObserverPid;
};
static constexpr PCWSTR kSharedStateName = L"Local\\Windhawk_Anim_State_120";
static constexpr LONG kSharedStateMagic = 0x57415338;
static constexpr LONG kSharedHeartbeatStaleMs = 2500;
HANDLE g_hMapFile = NULL;
SharedAnimState* g_pSharedState = nullptr;
bool g_sharedStateWritable = false;
// Shell ownership can be established after injection. Serialize every shared
// view access so promotion can't unmap a view while a hook reads it.
std::mutex g_SharedStateMutex;
std::atomic<LONG> g_consumedGeneration{0};
std::atomic<LONG> g_seenSessionEpoch{0};
std::atomic<int> g_durationMs{360};
std::atomic<int> g_closeDurationMs{630};
std::atomic<int> g_closeEffectStyle{1};
std::atomic<int> g_minRestoreEffectStyle{0};
std::atomic<bool> g_closeRandomEffect{false};
std::atomic<bool> g_minRestoreRandomEffect{false};
std::atomic<unsigned> g_effectRandState{0};
std::atomic<int> g_shatterBlockSize{5};
std::atomic<bool> g_minimizeAnimation{true};
std::atomic<bool> g_restoreAnimation{true};
std::atomic<bool> g_closeAnimation{true};
std::atomic<bool> g_hideAsClose{false};
std::atomic<bool> g_launchAnimation{false};
std::atomic<bool> g_switchAnimation{true};
std::atomic<int> g_switchDurationMs{200};
std::atomic<bool> g_unhideEnabled{false};
std::atomic<int> g_unhideDurationMs{450};
std::atomic<bool> g_unloading{false};
std::mutex g_WorkerThreadsMutex;
std::vector<HANDLE> g_WorkerThreads;
template <typename T> static T Clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}
static int RandomBelow(int n) {
    if (n <= 1) return 0;
    unsigned x = g_effectRandState.load(std::memory_order_relaxed);
    if (!x) {
        x = (unsigned)GetTickCount() ^ ((unsigned)GetCurrentProcessId() * 0x9E3779B9u);
        if (!x) x = 0xA5A5A5A5u;
    }
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    if (!x) x = 1;
    g_effectRandState.store(x, std::memory_order_relaxed);
    return (int)(x % (unsigned)n);
}
static int ResolveMinRestoreEffectStyle() {
    if (g_minRestoreRandomEffect.load(std::memory_order_relaxed)) {
        return RandomBelow(8);
    }
    return g_minRestoreEffectStyle.load(std::memory_order_relaxed);
}
static int ResolveCloseEffectStyle() {
    if (g_closeRandomEffect.load(std::memory_order_relaxed)) {
        return RandomBelow(6);
    }
    return g_closeEffectStyle.load(std::memory_order_relaxed);
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
static void FlushDwmOrYield() {
    if (FAILED(DwmFlush())) Sleep(1);
}
static void PulseSharedHeartbeatLocked() {
    if (!g_pSharedState || !g_sharedStateWritable) return;
    InterlockedExchange(&g_pSharedState->heartbeatTick, NonZeroTick());
}
static void PulseSharedHeartbeat() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    PulseSharedHeartbeatLocked();
}
static void ResetAltTabStateLocked() {
    if (!g_pSharedState || !g_sharedStateWritable) return;
    InterlockedExchange(&g_pSharedState->magic, kSharedStateMagic);
    InterlockedIncrement(&g_pSharedState->sessionEpoch);
    if (g_pSharedState->sessionEpoch == 0) InterlockedIncrement(&g_pSharedState->sessionEpoch);
    InterlockedExchange(&g_pSharedState->lastAltTabTick, 0);
    InterlockedExchange(&g_pSharedState->altTabSourceWindow, 0);
    InterlockedExchange(&g_pSharedState->altTabStartTick, 0);
    InterlockedExchange(&g_pSharedState->altTabGeneration, 0);
    LONG taskbarEpoch = InterlockedIncrement(&g_pSharedState->taskbarLayoutEpoch);
    if (taskbarEpoch == 0) InterlockedIncrement(&g_pSharedState->taskbarLayoutEpoch);
    InterlockedExchange(&g_pSharedState->taskbarObserverPid, 0);
    PulseSharedHeartbeatLocked();
}
static void BeginAltTabSession(HWND source) {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (!g_pSharedState || !g_sharedStateWritable) return;
    InterlockedExchange(&g_pSharedState->altTabSourceWindow, HwndToShared(source));
    InterlockedExchange(&g_pSharedState->altTabStartTick, (LONG)GetTickCount());
    LONG generation = InterlockedIncrement(&g_pSharedState->altTabGeneration);
    if (generation == 0) InterlockedIncrement(&g_pSharedState->altTabGeneration);
    InterlockedExchange(&g_pSharedState->lastAltTabTick, NonZeroTick());
    PulseSharedHeartbeatLocked();
}
static void TouchAltTabSession() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (g_pSharedState && g_sharedStateWritable) {
        InterlockedExchange(&g_pSharedState->lastAltTabTick, NonZeroTick());
        PulseSharedHeartbeatLocked();
    }
}
static bool EnsureSharedStateMapped();
static bool ConsumeAltTabIntent(HWND target, DWORD eventTime) {
    if (!EnsureSharedStateMapped()) return false;
    LONG epoch = 0;
    LONG heartbeat = 0;
    LONG stamp = 0;
    LONG generation = 0;
    DWORD startTick = 0;
    LONG source = 0;
    {
        std::lock_guard<std::mutex> lock(g_SharedStateMutex);
        if (!g_pSharedState || g_pSharedState->magic != kSharedStateMagic) return false;
        epoch = g_pSharedState->sessionEpoch;
        heartbeat = g_pSharedState->heartbeatTick;
        stamp = g_pSharedState->lastAltTabTick;
        generation = g_pSharedState->altTabGeneration;
        startTick = static_cast<DWORD>(g_pSharedState->altTabStartTick);
        source = g_pSharedState->altTabSourceWindow;
        if (generation != g_pSharedState->altTabGeneration) return false;
    }
    LONG seen = g_seenSessionEpoch.load(std::memory_order_relaxed);
    if (epoch != seen) {
        g_seenSessionEpoch.store(epoch, std::memory_order_relaxed);
        g_consumedGeneration.store(0, std::memory_order_relaxed);
    }
    if (!heartbeat ||
        (LONG)(GetTickCount() - (DWORD)heartbeat) > kSharedHeartbeatStaleMs) {
        return false;
    }
    if (!stamp || (LONG)(GetTickCount() - (DWORD)stamp) > AnimConstants::AltTabSessionMs) return false;
    if (!generation) return false;
    if (eventTime && startTick &&
        (LONG)(eventTime - startTick) < -(40 + AnimConstants::AltTabPollMs)) {
        return false;
    }
    LONG consumed = g_consumedGeneration.load(std::memory_order_relaxed);
    if (consumed == generation ||
        !g_consumedGeneration.compare_exchange_strong(consumed, generation, std::memory_order_relaxed)) {
        return false;
    }
    return source != 0 && source != HwndToShared(target);
}
static bool IsAltTabSourceCandidate(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    if (GetWindow(hWnd, GW_OWNER)) return false;
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    WCHAR cls[256];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    if (_wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0) return false;
    if (_wcsicmp(cls, L"MultitaskingViewFrame") == 0) return false;
    return true;
}
static bool IsExplorerProcess() {
    static const bool isExplorer = [] {
        WCHAR path[MAX_PATH]{};
        if (!GetModuleFileNameW(NULL, path, ARRAYSIZE(path))) return false;
        const WCHAR* name = wcsrchr(path, L'\\');
        return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
    }();
    return isExplorer;
}
static bool IsShellExplorerProcess() {
    if (!IsExplorerProcess()) return false;
    DWORD shellPid = 0;
    if (HWND hShell = GetShellWindow()) {
        GetWindowThreadProcessId(hShell, &shellPid);
    }
    if (!shellPid) {
        if (HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            GetWindowThreadProcessId(hTray, &shellPid);
        }
    }
    return shellPid == GetCurrentProcessId();
}

// UI Automation and DWM extended-frame bounds use physical screen pixels,
// while GetWindowRect is DPI-virtualized for the process running this hook.
// Use an HWND owned by this process as the transform reference. Taskbar-side
// calls animate foreign HWNDs from per-monitor-aware Explorer, whose animation
// space is already physical, so those coordinates deliberately stay unchanged.
static HWND GetAnimationCoordinateWindow(HWND hTarget) {
    DWORD targetPid = 0;
    if (hTarget) GetWindowThreadProcessId(hTarget, &targetPid);
    if (targetPid == GetCurrentProcessId()) return hTarget;
    // Foreign animations are normally created by Explorer, whose coordinate
    // space is already physical. Don't transform with the foreign app's DPI.
    return nullptr;
}

static bool PhysicalPointToAnimationSpace(HWND hTarget, POINT* point) {
    if (!point) return false;
    if (HWND hTransform = GetAnimationCoordinateWindow(hTarget)) {
        return PhysicalToLogicalPointForPerMonitorDPI(hTransform, point) != FALSE;
    }
    return true;
}

static bool PhysicalRectToAnimationSpace(HWND hTarget, RECT* rect) {
    if (!rect) return false;
    POINT topLeft{rect->left, rect->top};
    POINT bottomRight{rect->right, rect->bottom};
    if (!PhysicalPointToAnimationSpace(hTarget, &topLeft) ||
        !PhysicalPointToAnimationSpace(hTarget, &bottomRight)) {
        return false;
    }
    rect->left = topLeft.x;
    rect->top = topLeft.y;
    rect->right = bottomRight.x;
    rect->bottom = bottomRight.y;
    return true;
}

struct TaskbarCoordinateTransform {
    RECT physical{};
    RECT animation{};
    bool valid{};
};

static bool GetWindowRectInDpiContext(HWND hWnd,
                                      DPI_AWARENESS_CONTEXT context,
                                      RECT* rect) {
    if (!hWnd || !context || !rect) return false;
    DPI_AWARENESS_CONTEXT previous = SetThreadDpiAwarenessContext(context);
    if (!previous) return false;
    const BOOL result = GetWindowRect(hWnd, rect);
    SetThreadDpiAwarenessContext(previous);
    return result != FALSE;
}

static TaskbarCoordinateTransform BuildTaskbarCoordinateTransform(
    HWND hTarget, HWND hTray) {
    TaskbarCoordinateTransform transform;
    if (!GetWindowRectInDpiContext(
            hTray, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2,
            &transform.physical)) {
        return transform;
    }
    if (HWND hCoordinate = GetAnimationCoordinateWindow(hTarget)) {
        DPI_AWARENESS_CONTEXT targetContext =
            GetWindowDpiAwarenessContext(hCoordinate);
        if (!GetWindowRectInDpiContext(hTray, targetContext,
                                       &transform.animation)) {
            return transform;
        }
    } else {
        // Foreign taskbar animations are prepared by per-monitor-aware
        // Explorer and therefore use physical screen coordinates.
        transform.animation = transform.physical;
    }
    transform.valid = transform.physical.right != transform.physical.left &&
                      transform.physical.bottom != transform.physical.top;
    return transform;
}

static bool MapPhysicalTaskbarRectToAnimationSpace(
    const TaskbarCoordinateTransform& transform, RECT* rect) {
    if (!transform.valid || !rect) return false;
    const int physicalWidth =
        transform.physical.right - transform.physical.left;
    const int physicalHeight =
        transform.physical.bottom - transform.physical.top;
    const int animationWidth =
        transform.animation.right - transform.animation.left;
    const int animationHeight =
        transform.animation.bottom - transform.animation.top;
    auto mapX = [&](LONG value) {
        return transform.animation.left +
               MulDiv(value - transform.physical.left, animationWidth,
                      physicalWidth);
    };
    auto mapY = [&](LONG value) {
        return transform.animation.top +
               MulDiv(value - transform.physical.top, animationHeight,
                      physicalHeight);
    };
    rect->left = mapX(rect->left);
    rect->right = mapX(rect->right);
    rect->top = mapY(rect->top);
    rect->bottom = mapY(rect->bottom);
    return true;
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
static bool GetAnimWantRising(HWND hWnd, bool* wantRisingOut) {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    auto it = g_AnimWantRising.find(hWnd);
    if (it == g_AnimWantRising.end()) return false;
    if (wantRisingOut) *wantRisingOut = it->second;
    return true;
}
static void UpdateDwmTransitions(HWND hWnd, BOOL enable);
static void SetWindowCloak(HWND hWnd, BOOL cloak);
static void RestoreZOrderAfterGhost(HWND hWnd, LONG_PTR originalExStyle) {
    if (!hWnd || !IsWindow(hWnd)) return;
    SetWindowPos_Original(hWnd,
                          (originalExStyle & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0,
                          0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
static void RestoreZOrderAfterGhostAsync(HWND hWnd, LONG_PTR originalExStyle) {
    if (!hWnd || !IsWindow(hWnd)) return;
    SetWindowPos_Original(
        hWnd, (originalExStyle & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_NOTOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}
static BOOL WindowRestoresMaximized(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return FALSE;
    if (IsZoomed(hWnd)) return TRUE;
    WINDOWPLACEMENT placement{sizeof(placement)};
    return GetWindowPlacement(hWnd, &placement) &&
           ((placement.flags & WPF_RESTORETOMAXIMIZED) != 0 ||
            placement.showCmd == SW_SHOWMAXIMIZED);
}
static int StableRestoreShowCmd(BOOL restoreMaximized) {
    return restoreMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNOACTIVATE;
}
static bool HasReachedNativeShowState(HWND hWnd, bool rising,
                                      BOOL restoreMaximized) {
    if (!IsWindow(hWnd)) return false;
    if (!rising) return IsIconic(hWnd) != FALSE;
    return !IsIconic(hWnd) && (!restoreMaximized || IsZoomed(hWnd));
}
static void RestoreWindowUnderGhost(HWND hWnd, LONG_PTR originalExStyle,
                                    BOOL restoreMaximized) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (restoreMaximized) {
        // SW_SHOWNOACTIVATE consumes WPF_RESTORETOMAXIMIZED into the normal
        // rectangle. Use an idempotent maximized command for this state.
        if (IsIconic(hWnd) || !IsZoomed(hWnd)) {
            ShowWindow_Original(hWnd, SW_SHOWMAXIMIZED);
        }
        RestoreZOrderAfterGhost(hWnd, originalExStyle);
        return;
    }
    if (!IsIconic(hWnd)) {
        return;
    }
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (GetWindowPlacement(hWnd, &wp)) {
        wp.showCmd = SW_SHOWNOACTIVATE;
        SetWindowPlacement_Original(hWnd, &wp);
    }
    if (IsIconic(hWnd)) ShowWindow_Original(hWnd, SW_SHOWNOACTIVATE);
    if (IsIconic(hWnd)) ShowWindow_Original(hWnd, SW_RESTORE);
    RestoreZOrderAfterGhost(hWnd, originalExStyle);
}

static ULONG_PTR CreateMinRestorePairToken(int effectStyle) {
    ULONG_PTR serial =
        g_NextMinRestorePairToken.fetch_add(1, std::memory_order_relaxed) + 1;
    ULONG_PTR nonce = serial ^
                       (static_cast<ULONG_PTR>(GetCurrentProcessId()) *
                        static_cast<ULONG_PTR>(0x9E3779B1u)) ^
                       static_cast<ULONG_PTR>(GetTickCount());
    return (nonce << 4) | static_cast<ULONG_PTR>(effectStyle + 1);
}
static ULONG_PTR ReadMinRestorePairToken(HWND hWnd, int* effectStyleOut) {
    const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropMinRestorePair));
    const int encodedStyle = static_cast<int>(token & 0xF);
    if (!token || encodedStyle < 1 || encodedStyle > 8) return 0;
    if (effectStyleOut) *effectStyleOut = encodedStyle - 1;
    return token;
}
static bool PublishMinRestorePairToken(HWND hWnd, ULONG_PTR token) {
    return hWnd && token &&
           SetPropW(hWnd, kPropMinRestorePair,
                    reinterpret_cast<HANDLE>(token));
}
static void ClearMinRestorePairIfCurrent(HWND hWnd, ULONG_PTR token) {
    if (!hWnd || !token) return;
    if (reinterpret_cast<ULONG_PTR>(GetPropW(hWnd, kPropMinRestorePair)) == token) {
        RemovePropW(hWnd, kPropMinRestorePair);
    }
}
static void ClearMinRestorePair(HWND hWnd) {
    if (hWnd) RemovePropW(hWnd, kPropMinRestorePair);
}
static void ArmMaximizedRestoreGuard(HWND hWnd, DWORD durationMs) {
    if (!hWnd || !IsWindow(hWnd)) return;
    DWORD expires = GetTickCount() + std::max<DWORD>(durationMs, 1);
    if (!expires) expires = 1;
    SetPropW(hWnd, kPropMaximizedRestoreGuard,
             reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(expires)));
}
static bool IsMaximizedRestoreGuardActive(HWND hWnd) {
    const ULONG_PTR value = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropMaximizedRestoreGuard));
    if (!value) return false;
    const DWORD expires = static_cast<DWORD>(value);
    if (static_cast<LONG>(GetTickCount() - expires) < 0) return true;
    if (reinterpret_cast<ULONG_PTR>(GetPropW(hWnd,
                                             kPropMaximizedRestoreGuard)) == value) {
        RemovePropW(hWnd, kPropMaximizedRestoreGuard);
    }
    return false;
}
static bool ShouldSuppressRedundantMaximizedRestore(HWND hWnd) {
    return hWnd && IsWindow(hWnd) && !IsIconic(hWnd) && IsZoomed(hWnd) &&
           IsMaximizedRestoreGuardActive(hWnd);
}
static void ClearMaximizedRestoreGuard(HWND hWnd) {
    if (hWnd) RemovePropW(hWnd, kPropMaximizedRestoreGuard);
}
static ULONG_PTR CreateSnapshotCacheToken() {
    ULONG_PTR serial =
        g_NextSnapshotCacheToken.fetch_add(1, std::memory_order_relaxed) + 1;
    ULONG_PTR token = serial ^
                      (static_cast<ULONG_PTR>(GetCurrentProcessId()) *
                       static_cast<ULONG_PTR>(0x85EBCA6Bu)) ^
                      static_cast<ULONG_PTR>(GetTickCount());
    return token ? token : 1;
}
static void ClearSnapshotCachePropertyIfCurrent(HWND hWnd, ULONG_PTR token) {
    if (!hWnd || !token) return;
    if (reinterpret_cast<ULONG_PTR>(GetPropW(hWnd, kPropSnapshotCache)) == token) {
        RemovePropW(hWnd, kPropSnapshotCache);
    }
}
static bool IsLaunchAnimationCurrent(HWND hWnd, ULONG_PTR token) {
    return hWnd && token &&
           reinterpret_cast<ULONG_PTR>(
               GetPropW(hWnd, kPropLaunchAnimation)) == token;
}
static void ClearLaunchAnimationIfCurrent(HWND hWnd, ULONG_PTR token) {
    if (IsLaunchAnimationCurrent(hWnd, token)) {
        RemovePropW(hWnd, kPropLaunchAnimation);
    }
}
static void EraseSnapshotLocked(
    std::unordered_map<HWND, SnapCache>::iterator it) {
    if (it == g_WndSnapshots.end()) return;
    ClearSnapshotCachePropertyIfCurrent(it->first, it->second.windowToken);
    if (it->second.hBmp) DeleteObject(it->second.hBmp);
    g_WndSnapshotBytes = it->second.bytes <= g_WndSnapshotBytes
                             ? g_WndSnapshotBytes - it->second.bytes
                             : 0;
    g_WndSnapshots.erase(it);
}
static void EraseSnapshotLocked(HWND hWnd) {
    EraseSnapshotLocked(g_WndSnapshots.find(hWnd));
}
static void EraseSnapshotIfCurrentLocked(HWND hWnd, ULONG_PTR token) {
    auto it = g_WndSnapshots.find(hWnd);
    if (it != g_WndSnapshots.end() && it->second.windowToken == token) {
        EraseSnapshotLocked(it);
    }
}
static bool ConsumeSnapshotLocked(HWND hWnd, void* destination,
                                  int width, int height) {
    auto it = g_WndSnapshots.find(hWnd);
    if (it == g_WndSnapshots.end()) return false;
    const SnapCache& cache = it->second;
    const bool valid = destination && cache.pBits && cache.w == width &&
                       cache.h == height && cache.windowToken &&
                       reinterpret_cast<ULONG_PTR>(
                           GetPropW(hWnd, kPropSnapshotCache)) ==
                           cache.windowToken;
    if (valid) memcpy(destination, cache.pBits, cache.bytes);
    EraseSnapshotLocked(it);
    return valid;
}
static bool StoreSnapshotLocked(HWND hWnd, HBITMAP hBitmap, void* bits,
                                 int width, int height,
                                 ULONG_PTR* tokenOut = nullptr) {
    if (!hWnd || !hBitmap || !bits || width <= 0 || height <= 0 ||
        static_cast<size_t>(width) >
            SIZE_MAX / 4u / static_cast<size_t>(height)) {
        return false;
    }
    const size_t bytes =
        static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
    if (bytes > AnimConstants::SnapshotCacheMaxBytes) return false;

    EraseSnapshotLocked(hWnd);
    const ULONG_PTR token = CreateSnapshotCacheToken();
    if (!SetPropW(hWnd, kPropSnapshotCache,
                  reinterpret_cast<HANDLE>(token))) {
        return false;
    }
    while (!g_WndSnapshots.empty() &&
           (g_WndSnapshots.size() >= AnimConstants::SnapshotCacheMaxEntries ||
            g_WndSnapshotBytes >
                AnimConstants::SnapshotCacheMaxBytes - bytes)) {
        auto oldest = std::min_element(
            g_WndSnapshots.begin(), g_WndSnapshots.end(),
            [](const auto& left, const auto& right) {
                return left.second.lastUsed < right.second.lastUsed;
            });
        EraseSnapshotLocked(oldest);
    }

    uint64_t serial = ++g_NextSnapshotCacheSerial;
    if (!serial) serial = ++g_NextSnapshotCacheSerial;
    try {
        g_WndSnapshots.emplace(
            hWnd, SnapCache{hBitmap, bits, width, height, bytes, serial, token});
    } catch (const std::exception&) {
        ClearSnapshotCachePropertyIfCurrent(hWnd, token);
        return false;
    }
    g_WndSnapshotBytes += bytes;
    if (tokenOut) *tokenOut = token;
    return true;
}
enum class MinRestoreRetarget { None, Accepted, BusyOther };
static MinRestoreRetarget RetargetLiveMinRestore(HWND hWnd, bool wantRising) {
    const HWND hRequestForeground = wantRising ? GetForegroundWindow() : NULL;
    std::lock_guard<std::mutex> lock(g_StateMutex);
    auto it = g_AnimWantRising.find(hWnd);
    if (!g_AnimActive.count(hWnd)) return MinRestoreRetarget::None;
    if (it == g_AnimWantRising.end()) return MinRestoreRetarget::BusyOther;
    it->second = wantRising;
    if (wantRising) {
        g_AnimRestoreRequestForeground[hWnd] = hRequestForeground;
    } else {
        g_AnimRestoreRequestForeground.erase(hWnd);
    }
    return MinRestoreRetarget::Accepted;
}
static bool ReserveAsyncRestore(HWND hWnd, uint64_t* generationOut) {
    const HWND hRequestForeground = GetForegroundWindow();
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (g_unloading.load(std::memory_order_relaxed) || g_AnimActive.count(hWnd)) return false;
    uint64_t generation = ++g_NextAsyncRestoreReservation;
    if (!generation) generation = ++g_NextAsyncRestoreReservation;
    g_AnimActive.insert(hWnd);
    g_AnimWantRising[hWnd] = true;
    g_AnimRestoreRequestForeground[hWnd] = hRequestForeground;
    g_AsyncRestoreReservations[hWnd] = generation;
    if (generationOut) *generationOut = generation;
    return true;
}
static bool GetAsyncRestoreReservation(HWND hWnd, uint64_t generation,
                                       bool* wantRisingOut) {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
    auto directionIt = g_AnimWantRising.find(hWnd);
    if (reservationIt == g_AsyncRestoreReservations.end() ||
        reservationIt->second != generation || directionIt == g_AnimWantRising.end() ||
        !g_AnimActive.count(hWnd)) {
        return false;
    }
    if (wantRisingOut) *wantRisingOut = directionIt->second;
    return true;
}
static void FinalizeAsyncRestoreReservation(HWND hWnd, uint64_t generation,
                                            LONG_PTR originalExStyle,
                                            bool initialRestoreSubmitted,
                                            BOOL restoreMaximized);
static void CleanupWindowData(HWND hWnd) {
    ClearMinRestorePair(hWnd);
    ClearMaximizedRestoreGuard(hWnd);
    std::lock_guard<std::mutex> lock(g_StateMutex);
    EraseSnapshotLocked(hWnd);
    g_TaskbarDockXs.erase(hWnd);
    g_TaskbarDockLookupGenerations.erase(hWnd);
    g_TaskbarDockLookupStartedTicks.erase(hWnd);
    g_TaskbarDockNegativeUntilTicks.erase(hWnd);
    g_TaskbarDockPositiveUntilTicks.erase(hWnd);
    g_ProcessNameCache.erase(hWnd);
    g_LaunchSeen.erase(hWnd);
    g_AnimWantRising.erase(hWnd);
    g_AnimRestoreRequestForeground.erase(hWnd);
    if (g_AsyncRestoreReservations.erase(hWnd)) {
        g_AnimActive.erase(hWnd);
    }
}
static void SweepStaleData() {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    for (auto it = g_WndSnapshots.begin(); it != g_WndSnapshots.end();) {
        if (!IsWindow(it->first)) {
            auto stale = it++;
            EraseSnapshotLocked(stale);
        } else {
            ++it;
        }
    }
    for (auto it = g_TaskbarDockXs.begin(); it != g_TaskbarDockXs.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockXs.erase(it);
        else ++it;
    }
    for (auto it = g_TaskbarDockLookupGenerations.begin();
         it != g_TaskbarDockLookupGenerations.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockLookupGenerations.erase(it);
        else ++it;
    }
    for (auto it = g_TaskbarDockLookupStartedTicks.begin();
         it != g_TaskbarDockLookupStartedTicks.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockLookupStartedTicks.erase(it);
        else ++it;
    }
    for (auto it = g_TaskbarDockNegativeUntilTicks.begin();
         it != g_TaskbarDockNegativeUntilTicks.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockNegativeUntilTicks.erase(it);
        else ++it;
    }
    for (auto it = g_TaskbarDockPositiveUntilTicks.begin();
         it != g_TaskbarDockPositiveUntilTicks.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockPositiveUntilTicks.erase(it);
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
    for (auto it = g_AnimWantRising.begin(); it != g_AnimWantRising.end();) {
        if (!IsWindow(it->first)) it = g_AnimWantRising.erase(it);
        else ++it;
    }
    for (auto it = g_AnimRestoreRequestForeground.begin();
         it != g_AnimRestoreRequestForeground.end();) {
        if (!IsWindow(it->first)) it = g_AnimRestoreRequestForeground.erase(it);
        else ++it;
    }
    for (auto it = g_AsyncRestoreReservations.begin();
         it != g_AsyncRestoreReservations.end();) {
        if (!IsWindow(it->first)) it = g_AsyncRestoreReservations.erase(it);
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
static void CloseSharedStateLocked() {
    if (g_pSharedState) {
        UnmapViewOfFile(g_pSharedState);
        g_pSharedState = nullptr;
    }
    if (g_hMapFile) {
        CloseHandle(g_hMapFile);
        g_hMapFile = NULL;
    }
    g_sharedStateWritable = false;
}
static void CloseSharedState() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    CloseSharedStateLocked();
}
static void InitSharedMemoryLocked() {
    CloseSharedStateLocked();
    const bool explorer = IsShellExplorerProcess();
    if (explorer) {
        PSECURITY_DESCRIPTOR pSd = BuildSharedStateSd();
        SECURITY_ATTRIBUTES sa{sizeof(sa), pSd, FALSE};
        g_hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, pSd ? &sa : NULL, PAGE_READWRITE, 0,
                                        sizeof(SharedAnimState), kSharedStateName);
        const DWORD createErr = GetLastError();
        if (pSd) LocalFree(pSd);
        if (!g_hMapFile) {
            g_hMapFile = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, kSharedStateName);
        }
        if (!g_hMapFile) {
            Wh_Log(L"Explorer shared state unavailable err=%lu (switch intent writer offline)",
                   createErr);
            return;
        }
        g_pSharedState = (SharedAnimState*)MapViewOfFile(
            g_hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(SharedAnimState));
        if (!g_pSharedState) {
            Wh_Log(L"Explorer failed to map shared state for write err=%lu — switch anim disabled",
                   GetLastError());
            CloseSharedStateLocked();
            return;
        }
        g_sharedStateWritable = true;
        ResetAltTabStateLocked();
        Wh_Log(L"Explorer claimed Alt+Tab shared state epoch=%ld existing=%d",
               g_pSharedState->sessionEpoch, createErr == ERROR_ALREADY_EXISTS ? 1 : 0);
        return;
    }
    g_hMapFile = OpenFileMappingW(FILE_MAP_READ, FALSE, kSharedStateName);
    if (!g_hMapFile) {
        Wh_Log(L"Shared state not open yet (will retry on Alt+Tab)");
        return;
    }
    g_pSharedState = (SharedAnimState*)MapViewOfFile(g_hMapFile, FILE_MAP_READ, 0, 0,
                                                     sizeof(SharedAnimState));
    if (!g_pSharedState) {
        Wh_Log(L"Failed to map shared Alt+Tab state err=%lu", GetLastError());
        CloseSharedStateLocked();
    }
}
void InitSharedMemory() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    InitSharedMemoryLocked();
}
static bool EnsureSharedStateMapped() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (IsShellExplorerProcess() && !g_sharedStateWritable) {
        InitSharedMemoryLocked();
        return g_pSharedState && g_sharedStateWritable &&
               g_pSharedState->magic == kSharedStateMagic;
    }
    if (g_pSharedState) {
        if (g_pSharedState->magic == kSharedStateMagic) return true;
        if (!g_sharedStateWritable) CloseSharedStateLocked();
        else return false;
    }
    if (g_sharedStateWritable) return false;
    HANDLE h = OpenFileMappingW(FILE_MAP_READ, FALSE, kSharedStateName);
    if (!h) return false;
    auto* view = (SharedAnimState*)MapViewOfFile(h, FILE_MAP_READ, 0, 0, sizeof(SharedAnimState));
    if (!view || view->magic != kSharedStateMagic) {
        if (view) UnmapViewOfFile(view);
        CloseHandle(h);
        return false;
    }
    g_hMapFile = h;
    g_pSharedState = view;
    return true;
}
void LoadAnimSettings() {
    g_durationMs.store(Clamp(Wh_GetIntSetting(L"minimize_restore.duration_ms"), 200, 1400), std::memory_order_relaxed);
    g_closeDurationMs.store(Clamp(Wh_GetIntSetting(L"close.duration_ms"), 50, 5000), std::memory_order_relaxed);
    g_shatterBlockSize.store(Clamp(Wh_GetIntSetting(L"close.shatter_block_size"), 1, 100), std::memory_order_relaxed);
    {
        auto style = WindhawkUtils::StringSetting::make(L"close.effect_style");
        const int value = wcscmp(style, L"shatter") == 0 ? 0
                          : wcscmp(style, L"perlin") == 0 ? 2
                          : wcscmp(style, L"glitch") == 0 ? 3
                          : wcscmp(style, L"tv_off") == 0 ? 4
                          : wcscmp(style, L"pixel_melt") == 0 ? 5
                                                         : 1;
        g_closeEffectStyle.store(value, std::memory_order_relaxed);
    }
    {
        auto style = WindhawkUtils::StringSetting::make(L"minimize_restore.effect_style");
        const int value = wcscmp(style, L"windows10") == 0  ? 7
                          : wcscmp(style, L"ink_splash") == 0 ? 1
                          : wcscmp(style, L"scorch") == 0   ? 2
                          : wcscmp(style, L"splinter") == 0 ? 3
                          : wcscmp(style, L"mirage") == 0   ? 4
                          : wcscmp(style, L"stipple") == 0  ? 5
                          : wcscmp(style, L"swell") == 0    ? 6
                                                            : 0;
        g_minRestoreEffectStyle.store(value, std::memory_order_relaxed);
    }
    g_closeRandomEffect.store(Wh_GetIntSetting(L"close.random_effect") != 0, std::memory_order_relaxed);
    g_minRestoreRandomEffect.store(Wh_GetIntSetting(L"minimize_restore.random_effect") != 0,
                                   std::memory_order_relaxed);
    g_minimizeAnimation.store(Wh_GetIntSetting(L"minimize_restore.minimize_animation") != 0, std::memory_order_relaxed);
    g_restoreAnimation.store(Wh_GetIntSetting(L"minimize_restore.restore_animation") != 0, std::memory_order_relaxed);
    g_closeAnimation.store(Wh_GetIntSetting(L"close.close_animation") != 0, std::memory_order_relaxed);
    g_hideAsClose.store(Wh_GetIntSetting(L"close.hide_as_close") != 0, std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"minimize_restore.launch_animation") != 0, std::memory_order_relaxed);
    g_switchAnimation.store(Wh_GetIntSetting(L"window_switch.switch_animation") != 0, std::memory_order_relaxed);
    g_switchDurationMs.store(Clamp(Wh_GetIntSetting(L"window_switch.duration_ms"), 50, 1000), std::memory_order_relaxed);
    g_unhideEnabled.store(Wh_GetIntSetting(L"minimize_restore.unhide_taskbar") != 0, std::memory_order_relaxed);
    g_unhideDurationMs.store(Clamp(Wh_GetIntSetting(L"minimize_restore.unhide_duration_ms"), 0, 5000), std::memory_order_relaxed);
}
static void UpdateDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}
static void SetWindowCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}
static void RefreshDwmChromeAfterUncloak(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    WCHAR cls[64]{};
    const bool explorerFrame =
        GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
        (_wcsicmp(cls, L"CabinetWClass") == 0 ||
         _wcsicmp(cls, L"ExploreWClass") == 0);
    UINT backdrop = 0;
    const bool explicitBackdrop =
        SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop))) &&
        backdrop >= 2;
    if (!explorerFrame && !explicitBackdrop) return;
    if (explicitBackdrop) {
        DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    }
    if (explorerFrame) {
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }
    SetWindowPos_Original(hWnd, nullptr, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                              SWP_FRAMECHANGED);
    RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    SendMessageTimeoutW(hWnd, WM_DWMCOMPOSITIONCHANGED, 0, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 50,
                        nullptr);
}
static void RestoreLayeredOpacity(HWND hWnd, LONG_PTR originalExStyle) {
    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
    if (!(originalExStyle & WS_EX_LAYERED)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    }
}
static void UndoRisingHide(HWND hWnd, LONG_PTR originalExStyle, BOOL cloakHidden) {
    if (cloakHidden) SetWindowCloak(hWnd, FALSE);
    else RestoreLayeredOpacity(hWnd, originalExStyle);
    UpdateDwmTransitions(hWnd, TRUE);
    RestoreZOrderAfterGhost(hWnd, originalExStyle);
    if (cloakHidden) RefreshDwmChromeAfterUncloak(hWnd);
}
static void UndoRisingHideNonBlocking(HWND hWnd, LONG_PTR originalExStyle,
                                     BOOL cloakHidden) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (cloakHidden) SetWindowCloak(hWnd, FALSE);
    else RestoreLayeredOpacity(hWnd, originalExStyle);
    UpdateDwmTransitions(hWnd, TRUE);
    RestoreZOrderAfterGhostAsync(hWnd, originalExStyle);
}
static void FailAnimationStart(HWND hWnd, BOOL rising, LONG_PTR originalExStyle, BOOL cloakHidden,
                               bool skipDwmIfOwned = false) {
    if (rising || cloakHidden) {
        const DWORD targetThreadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (targetThreadId && targetThreadId == GetCurrentThreadId()) {
            UndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            // Failure cleanup must not synchronously enter a foreign/hung UI
            // thread. The asynchronous z-order repair is sufficient here.
            UndoRisingHideNonBlocking(hWnd, originalExStyle, cloakHidden);
        }
        return;
    }
    if (skipDwmIfOwned) {
        bool owned = false;
        { std::lock_guard<std::mutex> lock(g_StateMutex); owned = g_AnimActive.count(hWnd) != 0; }
        if (owned) return;
    }
    UpdateDwmTransitions(hWnd, TRUE);
}

static void FinalizeAsyncRestoreReservation(HWND hWnd, uint64_t generation,
                                             LONG_PTR originalExStyle,
                                             bool initialRestoreSubmitted,
                                             BOOL restoreMaximized) {
    const ULONG_PTR pairedEffectToken =
        ReadMinRestorePairToken(hWnd, nullptr);
    bool haveLastSubmission = initialRestoreSubmitted;
    bool lastSubmittedRising = true;
    bool lastSubmissionObserved = false;
    bool timedOutRecovery = false;

    for (;;) {
        bool wantRising = true;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
            auto directionIt = g_AnimWantRising.find(hWnd);
            if (reservationIt == g_AsyncRestoreReservations.end() ||
                reservationIt->second != generation ||
                directionIt == g_AnimWantRising.end() || !g_AnimActive.count(hWnd)) {
                return;
            }
            wantRising = directionIt->second;
        }

        if (!IsWindow(hWnd)) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
            if (reservationIt != g_AsyncRestoreReservations.end() &&
                reservationIt->second == generation) {
                g_AsyncRestoreReservations.erase(reservationIt);
                g_AnimActive.erase(hWnd);
                g_AnimWantRising.erase(hWnd);
                g_AnimRestoreRequestForeground.erase(hWnd);
            }
            return;
        }

        if (haveLastSubmission && !lastSubmissionObserved) {
            const DWORD deadline = GetTickCount() + AnimConstants::NativeStateWaitMs;
            while (IsWindow(hWnd) && !g_unloading.load(std::memory_order_relaxed) &&
                   !HasReachedNativeShowState(hWnd, lastSubmittedRising,
                                              restoreMaximized) &&
                   (LONG)(GetTickCount() - deadline) < 0) {
                Sleep(10);
            }
            if (!IsWindow(hWnd)) continue;
            if (!g_unloading.load(std::memory_order_relaxed) &&
                HasReachedNativeShowState(hWnd, lastSubmittedRising,
                                          restoreMaximized)) {
                lastSubmissionObserved = true;
                continue;
            }
            if (!g_unloading.load(std::memory_order_relaxed)) {
                Wh_Log(L"Async restore state timeout hwnd=%p rising=%d", hWnd,
                       lastSubmittedRising);
            }
            timedOutRecovery = true;
            haveLastSubmission = false;
            lastSubmissionObserved = false;
            continue;
        }
        if (!haveLastSubmission || wantRising != lastSubmittedRising) {
            const BOOL submitted = g_unloading.load(std::memory_order_relaxed)
                                       ? FALSE
                                       : ShowWindowAsync_Original(
                                             hWnd,
                                             wantRising
                                                 ? StableRestoreShowCmd(restoreMaximized)
                                                 : SW_MINIMIZE);
            haveLastSubmission = true;
            lastSubmittedRising = wantRising;
            if (!submitted) timedOutRecovery = true;
            lastSubmissionObserved = timedOutRecovery;
            continue;
        }
        const DWORD targetThreadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (timedOutRecovery ||
            !targetThreadId || targetThreadId != GetCurrentThreadId()) {
            UndoRisingHideNonBlocking(hWnd, originalExStyle, TRUE);
        } else {
            UndoRisingHide(hWnd, originalExStyle, TRUE);
        }
        bool directionChanged = false;
        bool completed = false;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
            auto directionIt = g_AnimWantRising.find(hWnd);
            if (reservationIt == g_AsyncRestoreReservations.end() ||
                reservationIt->second != generation ||
                directionIt == g_AnimWantRising.end() || !g_AnimActive.count(hWnd)) {
                return;
            }
            directionChanged = directionIt->second != wantRising;
            if (!directionChanged) {
                if (wantRising) EraseSnapshotLocked(hWnd);
                g_AsyncRestoreReservations.erase(reservationIt);
                g_AnimActive.erase(hWnd);
                g_AnimWantRising.erase(hWnd);
                g_AnimRestoreRequestForeground.erase(hWnd);
                completed = true;
            }
        }
        if (completed) {
            if (wantRising) {
                ClearMinRestorePairIfCurrent(hWnd, pairedEffectToken);
                if (restoreMaximized) {
                    ArmMaximizedRestoreGuard(
                        hWnd,
                        static_cast<DWORD>(GetDoubleClickTime() + 100));
                }
            }
            return;
        }

        UpdateDwmTransitions(hWnd, FALSE);
        SetWindowCloak(hWnd, TRUE);
    }
}

static void AbortAsyncRestoreReservation(HWND hWnd, uint64_t generation,
                                         LONG_PTR originalExStyle,
                                         bool initialRestoreSubmitted,
                                         BOOL restoreMaximized) {
    const ULONG_PTR pairedEffectToken =
        ReadMinRestorePairToken(hWnd, nullptr);
    bool wantRising = true;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
        auto directionIt = g_AnimWantRising.find(hWnd);
        if (reservationIt == g_AsyncRestoreReservations.end() ||
            reservationIt->second != generation ||
            directionIt == g_AnimWantRising.end() || !g_AnimActive.count(hWnd)) {
            return;
        }
        wantRising = directionIt->second;
        if (IsWindow(hWnd) && !g_unloading.load(std::memory_order_relaxed) &&
            (initialRestoreSubmitted || !wantRising)) {
            ShowWindowAsync_Original(
                hWnd, wantRising ? StableRestoreShowCmd(restoreMaximized)
                                 : SW_MINIMIZE);
        }
        if (wantRising) EraseSnapshotLocked(hWnd);
        g_AnimWantRising.erase(directionIt);
        g_AnimRestoreRequestForeground.erase(hWnd);
        g_AsyncRestoreReservations.erase(reservationIt);
    }

    UndoRisingHideNonBlocking(hWnd, originalExStyle, TRUE);
    if (wantRising) {
        ClearMinRestorePairIfCurrent(hWnd, pairedEffectToken);
        if (initialRestoreSubmitted && restoreMaximized) {
            ArmMaximizedRestoreGuard(
                hWnd,
                static_cast<DWORD>(GetDoubleClickTime() + 100));
        }
    }
    std::lock_guard<std::mutex> lock(g_StateMutex);
    g_AnimActive.erase(hWnd);
}
static bool IsShowCmdForWinEvent(int cmd) {
    return cmd == SW_SHOW || cmd == SW_SHOWNORMAL || cmd == SW_SHOWMAXIMIZED || cmd == SW_RESTORE ||
           cmd == SW_SHOWDEFAULT || cmd == SW_SHOWMINIMIZED || cmd == SW_SHOWMINNOACTIVE ||
           cmd == SW_SHOWNA || cmd == SW_SHOWNOACTIVATE;
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
    // This is also a lazy recovery path for a shell that became ready after
    // the ownership probe's initial fast-poll period.
    if (g_switchAnimation.load(std::memory_order_relaxed)) {
        EnsureExplorerForegroundThreadStarted();
    }
    if (g_winEventThreadStarted.load(std::memory_order_relaxed)) return;
    if (g_unloading.load(std::memory_order_relaxed)) return;
    const bool needTaskbarObserver =
        IsShellExplorerProcess() &&
        (g_minimizeAnimation.load(std::memory_order_relaxed) ||
         g_restoreAnimation.load(std::memory_order_relaxed));
    if (!g_switchAnimation.load(std::memory_order_relaxed) && !needTaskbarObserver) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (!g_switchAnimation.load(std::memory_order_relaxed) && !needTaskbarObserver) {
        return;
    }
    if (IsShellExplorerProcess()) EnsureSharedStateMapped();
    if (g_hWinEventThread &&
        WaitForSingleObject(g_hWinEventThread, 0) == WAIT_OBJECT_0) {
        CloseHandle(g_hWinEventThread);
        g_hWinEventThread = NULL;
        g_winEventThreadStarted.store(false, std::memory_order_relaxed);
    }
    if (g_hWinEventThread || g_winEventThreadStarted.load(std::memory_order_relaxed)) return;
    g_winEventThreadStarted.store(true, std::memory_order_relaxed);
    HANDLE hThread = CreateThread(NULL, 0, WinEventHookThread, NULL, 0, NULL);
    if (!hThread) {
        g_winEventThreadStarted.store(false, std::memory_order_relaxed);
        return;
    }
    g_hWinEventThread = hThread;
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
static UINT GetWindowDpiCompat(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static const auto getDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    const UINT dpi = getDpiForWindow && hWnd ? getDpiForWindow(hWnd) : 0;
    return dpi ? dpi : 96;
}
static UINT GetAnimationSpaceDpi(HWND hTarget, HWND hTray) {
    if (HWND hCoordinate = GetAnimationCoordinateWindow(hTarget)) {
        return GetWindowDpiCompat(hCoordinate);
    }
    return GetWindowDpiCompat(hTray);
}
static int ScaleTaskbarPx(HWND hTarget, HWND hTray, int value) {
    return MulDiv(value,
                  static_cast<int>(GetAnimationSpaceDpi(hTarget, hTray)), 96);
}
static bool IsBottomTaskbarExpanded(HWND hTarget, HWND hTray,
                                    const MONITORINFO& mi) {
    RECT tr{};
    if (!hTray || !GetWindowRect(hTray, &tr)) return false;
    const int th = tr.bottom - tr.top;
    const int monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
    const int minThickness = ScaleTaskbarPx(hTarget, hTray, 24);
    const int edgeTolerance =
        std::max(1, ScaleTaskbarPx(hTarget, hTray, 2));
    if (th < minThickness || th >= monH / 2) return false;
    if (tr.bottom < mi.rcMonitor.bottom - edgeTolerance) return false;
    return tr.top <= mi.rcMonitor.bottom - minThickness;
}
static bool IsTaskbarExpanded(HWND hTarget, HWND hTray, HMONITOR hMon) {
    if (!hTray) return false;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!hMon) hMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
    return GetMonitorInfoW(hMon, &mi) &&
           IsBottomTaskbarExpanded(hTarget, hTray, mi);
}
static bool IsCursorOverTaskbar(HWND hTray) {
    POINT cursor{};
    if (!hTray || !GetCursorPos(&cursor)) return false;
    HWND hit = WindowFromPoint(cursor);
    return hit && GetAncestor(hit, GA_ROOT) == hTray;
}
static bool RestoreForegroundAfterTaskbarReveal(HWND hTray, HWND hRealWnd, HWND hNextApp,
                                                bool rising) {
    if (!hTray || GetForegroundWindow() != hTray) {
        return false;
    }
    HWND hTarget = NULL;
    if (rising && hRealWnd && IsWindow(hRealWnd) && IsWindowVisible(hRealWnd)) {
        hTarget = hRealWnd;
    } else if (hNextApp && IsWindow(hNextApp) && IsWindowVisible(hNextApp)) {
        hTarget = hNextApp;
    } else {
        hTarget = FindWindowW(L"Progman", NULL);
    }
    return hTarget && SetForegroundWindow(hTarget);
}
static void WaitForTaskbarExpanded(HWND hTarget, HWND hTray, HMONITOR hMon) {
    if (!hTray) return;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!hMon) hMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(hMon, &mi)) return;
    const DWORD start = GetTickCount();
    while ((int)(GetTickCount() - start) < AnimConstants::TaskbarExpandWaitMs) {
        if (g_unloading.load(std::memory_order_relaxed)) return;
        if (IsBottomTaskbarExpanded(hTarget, hTray, mi)) return;
        Sleep(10);
    }
}
static float GetTaskbarDockY(HWND hTarget, HMONITOR hMon,
                             const MONITORINFO& mi,
                             bool aimAtExpandedTaskbar = false) {
    float dockY = (float)mi.rcMonitor.bottom;
    if (mi.rcWork.bottom > mi.rcMonitor.top && mi.rcWork.bottom < mi.rcMonitor.bottom) {
        dockY = (float)mi.rcWork.bottom;
    }
    HWND hTray = FindTaskbarForMonitor(hMon);
    if (!hTray) return dockY;
    RECT tr{};
    if (!GetWindowRect(hTray, &tr)) return dockY;
    const int th = tr.bottom - tr.top;
    const int monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
    const int minThickness = ScaleTaskbarPx(hTarget, hTray, 24);
    const int edgeTolerance =
        std::max(1, ScaleTaskbarPx(hTarget, hTray, 2));
    if (th <= 0 || th >= monH / 2 ||
        tr.bottom < mi.rcMonitor.bottom - edgeTolerance) return dockY;
    if (tr.top <= mi.rcMonitor.bottom - minThickness) {
        return (float)tr.top;
    }
    if (aimAtExpandedTaskbar) return (float)(mi.rcMonitor.bottom - th);
    return (float)mi.rcMonitor.bottom;
}
static HWND FindNextAppWindow(HWND hWnd, HWND hTray) {
    HWND hwndIter = GetWindow(hWnd, GW_HWNDNEXT);
    while (hwndIter) {
        if (IsWindowVisible(hwndIter) && !IsIconic(hwndIter) &&
            GetAncestor(hwndIter, GA_ROOT) == hwndIter &&
            GetWindowTextLengthW(hwndIter) > 0 &&
            hwndIter != hWnd && hwndIter != hTray) {
            const LONG_PTR exStyle = GetWindowLongPtrW(hwndIter, GWL_EXSTYLE);
            if (!(exStyle & WS_EX_TOOLWINDOW)) return hwndIter;
        }
        hwndIter = GetWindow(hwndIter, GW_HWNDNEXT);
    }
    return NULL;
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
    std::wstring procHintLower;
    std::wstring processKey;
    HMONITOR hMon;
    int fallbackX;
    uint64_t generation;
    UiaPending* pending = nullptr;
};
static std::wstring MakeProcessDockKey(const std::wstring& procNameLower, HMONITOR hMon) {
    if (procNameLower.empty()) return L"";
    if (!hMon) return procNameLower;
    return procNameLower + L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
}
static std::wstring ProcHintForUia(const std::wstring& procNameLower) {
    if (procNameLower == L"chrome") return L"google chrome";
    if (procNameLower == L"msedge") return L"microsoft edge";
    if (procNameLower == L"firefox") return L"firefox";
    if (procNameLower == L"brave") return L"brave";
    if (procNameLower == L"opera") return L"opera";
    if (procNameLower == L"vivaldi") return L"vivaldi";
    if (procNameLower == L"discord") return L"discord";
    if (procNameLower == L"spotify") return L"spotify";
    if (procNameLower == L"code") return L"visual studio code";
    if (procNameLower == L"devenv") return L"visual studio";
    return procNameLower;
}

struct TaskbarWindowSetSignatureContext {
    uint64_t xorHash = 0;
    uint64_t count = 0;
};

static void InvalidateTaskbarDockCachesLocked(
    bool preserveProcessFallback = false) {
    g_TaskbarDockXs.clear();
    if (!preserveProcessFallback) g_ProcessDockXs.clear();
    g_TaskbarDockLookupGenerations.clear();
    g_TaskbarDockLookupStartedTicks.clear();
    g_TaskbarDockNegativeUntilTicks.clear();
    g_TaskbarDockPositiveUntilTicks.clear();
}

static DWORD GetTaskbarExplorerPid() {
    DWORD explorerPid = 0;
    if (HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(hTray, &explorerPid);
    }
    return explorerPid;
}

static bool ReadSharedTaskbarState(LONG* epochOut, LONG* observerPidOut) {
    if (!EnsureSharedStateMapped()) return false;
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (!g_pSharedState || g_pSharedState->magic != kSharedStateMagic) return false;
    if (epochOut) *epochOut = g_pSharedState->taskbarLayoutEpoch;
    if (observerPidOut) *observerPidOut = g_pSharedState->taskbarObserverPid;
    return true;
}

static LONG GetSharedTaskbarLayoutEpoch() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    return g_pSharedState && g_pSharedState->magic == kSharedStateMagic
               ? g_pSharedState->taskbarLayoutEpoch
               : 0;
}

static bool SyncTaskbarLayoutEpoch() {
    const DWORD explorerPid = GetTaskbarExplorerPid();
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (explorerPid != g_seenTaskbarExplorerPid) {
            InvalidateTaskbarDockCachesLocked();
            g_seenTaskbarExplorerPid = explorerPid;
            g_seenTaskbarLayoutEpoch = 0;
        }
    }
    LONG epoch = 0;
    LONG observerPid = 0;
    if (!explorerPid || !ReadSharedTaskbarState(&epoch, &observerPid)) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (epoch != g_seenTaskbarLayoutEpoch) {
            InvalidateTaskbarDockCachesLocked();
            g_seenTaskbarLayoutEpoch = epoch;
        }
    }
    return epoch != 0 && observerPid == static_cast<LONG>(explorerPid);
}

static BOOL CALLBACK HashTaskbarWindowProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindowVisible(hWnd) || GetAncestor(hWnd, GA_ROOT) != hWnd) return TRUE;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) ||
        ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) ||
        (GetWindow(hWnd, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW))) {
        return TRUE;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    const uint64_t hwndValue = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(hWnd));
    const uint64_t monitorValue = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)));
    uint64_t value = hwndValue ^ (static_cast<uint64_t>(processId) << 32) ^ monitorValue;
    value ^= value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    value ^= value >> 31;

    auto* context = reinterpret_cast<TaskbarWindowSetSignatureContext*>(lParam);
    context->xorHash ^= value;
    ++context->count;
    return TRUE;
}

static void RefreshTaskbarWindowSetSignature() {
    TaskbarWindowSetSignatureContext context;
    EnumWindows(HashTaskbarWindowProc, reinterpret_cast<LPARAM>(&context));
    const uint64_t signature = context.xorHash ^
                               (context.count * 0x9e3779b97f4a7c15ULL);

    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (g_TaskbarWindowSetSignatureInitialized &&
        signature != g_TaskbarWindowSetSignature) {
        // A fresh UIA lookup is still mandatory, but retain the old process
        // position strictly as a timeout fallback instead of aiming at the
        // monitor center. Process entries are never hot-returned.
        InvalidateTaskbarDockCachesLocked(/*preserveProcessFallback=*/true);
    }
    g_TaskbarWindowSetSignature = signature;
    g_TaskbarWindowSetSignatureInitialized = true;
}

DWORD WINAPI UiaWorkerThread(LPVOID lpParam) {
    UiaTask* t = (UiaTask*)lpParam;
    UiaPending* pending = t->pending;
    struct UiaCleanupGuard {
        UiaTask* task;
        UiaPending* pending;
        ~UiaCleanupGuard() {
            if (pending) {
                if (pending->done) SetEvent(pending->done);
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
                const TaskbarCoordinateTransform taskbarTransform =
                    BuildTaskbarCoordinateTransform(t->hWndApp, hTray);
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
                        MONITORINFO mi{};
                        mi.cbSize = sizeof(MONITORINFO);
                        if (!GetMonitorInfoW(t->hMon, &mi)) {
                            mi.rcMonitor.left = 0;
                            mi.rcMonitor.top = 0;
                            mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
                            mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
                            mi.rcWork = mi.rcMonitor;
                        }
                        int monRight = taskbarTransform.valid
                                           ? taskbarTransform.animation.right
                                           : mi.rcMonitor.right;
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
                                        if (!t->titleLower.empty() && uiaNameLower.find(t->titleLower) != std::wstring::npos) score += 500;
                                        if (!t->procNameLower.empty() && uiaNameLower.find(t->procNameLower) != std::wstring::npos) score += 400;
                                        if (!t->procHintLower.empty() && t->procHintLower != t->procNameLower &&
                                            uiaNameLower.find(t->procHintLower) != std::wstring::npos) {
                                            score += 900;
                                        }
                                        std::wstring currentWord;
                                        for (wchar_t c : t->titleLower) {
                                            if (iswalnum(c)) currentWord += c;
                                            else {
                                                if (currentWord.length() >= 4 &&
                                                    uiaNameLower.find(currentWord) != std::wstring::npos) {
                                                    score += 50;
                                                }
                                                currentWord.clear();
                                            }
                                        }
                                        if (currentWord.length() >= 4 &&
                                            uiaNameLower.find(currentWord) != std::wstring::npos) {
                                            score += 50;
                                        }
                                        if (uiaNameLower.find(L"start") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"search") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"task view") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"widgets") != std::wstring::npos) score -= 500;
                                        if (score >= AnimConstants::UiaMinAcceptScore && score > bestScore) {
                                            RECT bRect{};
                                            if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&bRect)) &&
                                                MapPhysicalTaskbarRectToAnimationSpace(
                                                    taskbarTransform, &bRect)) {
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
    if (g_unloading.load(std::memory_order_relaxed)) {
        return 0;
    }
    SyncTaskbarLayoutEpoch();
    if (uiaFound) {
        bool accepted = false;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto generationIt = g_TaskbarDockLookupGenerations.find(t->hWndApp);
            if (generationIt != g_TaskbarDockLookupGenerations.end() &&
                generationIt->second == t->generation) {
                g_TaskbarDockXs[t->hWndApp] = targetX;
                if (!t->processKey.empty()) g_ProcessDockXs[t->processKey] = targetX;
                g_TaskbarDockLookupStartedTicks.erase(t->hWndApp);
                g_TaskbarDockNegativeUntilTicks.erase(t->hWndApp);
                g_TaskbarDockPositiveUntilTicks[t->hWndApp] =
                    GetTickCount() + AnimConstants::UiaPositiveFallbackCacheMs;
                if (pending) {
                    pending->targetX = targetX;
                    pending->found = true;
                }
                accepted = true;
            }
        }
        if (accepted) Wh_Log(L"UIA dock match hwnd=%p x=%d", t->hWndApp, targetX);
    } else {
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto generationIt = g_TaskbarDockLookupGenerations.find(t->hWndApp);
            if (generationIt != g_TaskbarDockLookupGenerations.end() &&
                generationIt->second == t->generation) {
                g_TaskbarDockLookupStartedTicks.erase(t->hWndApp);
                g_TaskbarDockXs.erase(t->hWndApp);
                g_TaskbarDockPositiveUntilTicks.erase(t->hWndApp);
                g_TaskbarDockNegativeUntilTicks[t->hWndApp] =
                    GetTickCount() + AnimConstants::UiaNegativeCacheMs;
            }
        }
        Wh_Log(L"UIA dock miss hwnd=%p fallback=%d", t->hWndApp, t->fallbackX);
    }
    return 0;
}
int GetTaskbarButtonX_Async(HWND hWndApp, const WCHAR* windowTitle, int fallbackX, HMONITOR hMon) {
    const bool sharedLayoutTracking = SyncTaskbarLayoutEpoch();
    // Win11's XAML taskbar doesn't reliably surface every button removal or
    // reflow through the HWND WinEvent bridge. Keep the shared epoch for
    // taskbar-only layout changes, but also verify the top-level app set before
    // trusting a hot per-window position. This intentionally runs on every
    // Genie/Windows 10 lookup so the first animation after a close is correct.
    RefreshTaskbarWindowSetSignature();
    std::wstring procNameLower = GetProcessNameCached(hWndApp);
    std::wstring processKey = MakeProcessDockKey(procNameLower, hMon);
    int hwndCachedX = 0;
    int processCachedX = 0;
    bool haveHwndCache = false;
    bool haveProcessCache = false;
    bool lookupInFlight = false;
    bool negativeCacheActive = false;
    bool positiveFallbackCacheActive = false;
    uint64_t generation = 0;
    LONG layoutEpochAtRead = 0;
    const DWORD now = GetTickCount();
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto it = g_TaskbarDockXs.find(hWndApp);
        if (it != g_TaskbarDockXs.end()) {
            hwndCachedX = it->second;
            haveHwndCache = true;
        }
        if (!processKey.empty()) {
            auto pit = g_ProcessDockXs.find(processKey);
            if (pit != g_ProcessDockXs.end()) {
                processCachedX = pit->second;
                haveProcessCache = true;
            }
        }
        auto lookupIt = g_TaskbarDockLookupStartedTicks.find(hWndApp);
        lookupInFlight = lookupIt != g_TaskbarDockLookupStartedTicks.end() &&
                         (LONG)(now - lookupIt->second) <
                             AnimConstants::UiaLookupInFlightMs;
        auto negativeIt = g_TaskbarDockNegativeUntilTicks.find(hWndApp);
        negativeCacheActive = negativeIt != g_TaskbarDockNegativeUntilTicks.end() &&
                              (LONG)(now - negativeIt->second) < 0;
        auto positiveIt = g_TaskbarDockPositiveUntilTicks.find(hWndApp);
        positiveFallbackCacheActive =
            positiveIt != g_TaskbarDockPositiveUntilTicks.end() &&
            (LONG)(now - positiveIt->second) < 0;
        layoutEpochAtRead = g_seenTaskbarLayoutEpoch;
    }
    const bool layoutEpochStillCurrent = !sharedLayoutTracking ||
                                         GetSharedTaskbarLayoutEpoch() ==
                                             layoutEpochAtRead;
    if (haveHwndCache && layoutEpochStillCurrent &&
        (sharedLayoutTracking || positiveFallbackCacheActive)) {
        return hwndCachedX;
    }
    if (!layoutEpochStillCurrent) {
        SyncTaskbarLayoutEpoch();
        haveHwndCache = false;
        haveProcessCache = false;
        lookupInFlight = false;
        negativeCacheActive = false;
        positiveFallbackCacheActive = false;
    }
    const int softFallback = haveHwndCache ? hwndCachedX
                           : haveProcessCache ? processCachedX
                                              : fallbackX;
    if (lookupInFlight || negativeCacheActive) return softFallback;

    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        generation = ++g_NextTaskbarDockLookupGeneration;
        if (!generation) generation = ++g_NextTaskbarDockLookupGeneration;
        g_TaskbarDockLookupGenerations[hWndApp] = generation;
        g_TaskbarDockLookupStartedTicks[hWndApp] = now;
        g_TaskbarDockNegativeUntilTicks.erase(hWndApp);
    }
    std::wstring titleLower = windowTitle ? windowTitle : L"";
    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::towlower);
    const std::wstring procHintLower = ProcHintForUia(procNameLower);
    auto cancelLookup = [&]() {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto generationIt = g_TaskbarDockLookupGenerations.find(hWndApp);
        if (generationIt != g_TaskbarDockLookupGenerations.end() &&
            generationIt->second == generation) {
            g_TaskbarDockLookupStartedTicks.erase(hWndApp);
            g_TaskbarDockNegativeUntilTicks[hWndApp] =
                GetTickCount() + AnimConstants::UiaNegativeCacheMs;
        }
    };
    auto* pending = new (std::nothrow) UiaPending{};
    if (!pending) {
        cancelLookup();
        return softFallback;
    }
    pending->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    pending->targetX = softFallback;
    if (!pending->done) {
        cancelLookup();
        pending->Release();
        pending->Release();
        return softFallback;
    }
    UiaTask* task = nullptr;
    try {
        task = new (std::nothrow) UiaTask{hWndApp, std::move(titleLower),
                                         std::move(procNameLower), procHintLower,
                                         std::move(processKey), hMon, softFallback,
                                         generation, pending};
    } catch (const std::exception&) {
        task = nullptr;
    }
    if (!task) {
        cancelLookup();
        pending->Release();
        pending->Release();
        return softFallback;
    }
    if (!StartWorkerThread(UiaWorkerThread, task)) {
        delete task;
        cancelLookup();
        pending->Release();
        pending->Release();
        return softFallback;
    }
    const DWORD waitMs = (DWORD)AnimConstants::UiaLookupWaitMs;
    int result = softFallback;
    if (waitMs > 0) {
        const DWORD wait = WaitForSingleObject(pending->done, waitMs);
        if (wait == WAIT_OBJECT_0 && pending->found) {
            result = pending->targetX;
        }
    }
    pending->Release();
    return result;
}
static void StartAltTabSessionPoll() {
    if (g_unloading.load(std::memory_order_relaxed) ||
        !g_explorerAltTabTrackerEnabled.load(std::memory_order_relaxed) ||
        !g_switchAnimation.load(std::memory_order_relaxed)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_AltTabSessionMutex);
    if (g_unloading.load(std::memory_order_relaxed) ||
        !g_explorerAltTabTrackerEnabled.load(std::memory_order_relaxed) ||
        g_altTabSessionPollRunning.load(std::memory_order_relaxed)) {
        return;
    }
    if (g_hAltTabSessionThread) {
        if (WaitForSingleObject(g_hAltTabSessionThread, 0) == WAIT_OBJECT_0) {
            CloseHandle(g_hAltTabSessionThread);
            g_hAltTabSessionThread = NULL;
        } else {
            return;
        }
    }
    g_altTabSessionPollRunning.store(true, std::memory_order_relaxed);
    g_hAltTabSessionThread = CreateThread(NULL, 0, AltTabSessionPollThread, NULL, 0, NULL);
    if (!g_hAltTabSessionThread) {
        g_altTabSessionPollRunning.store(false, std::memory_order_relaxed);
    }
}
DWORD WINAPI AltTabSessionPollThread(LPVOID lpParam) {
    (void)lpParam;
    while (!g_unloading.load(std::memory_order_relaxed) &&
           g_altTabSessionPollRunning.load(std::memory_order_relaxed)) {
        TouchAltTabSession();
        if (!(GetAsyncKeyState(VK_MENU) & 0x8000)) break;
        Sleep(AnimConstants::AltTabPollMs);
    }
    g_altTabSessionPollRunning.store(false, std::memory_order_relaxed);
    return 0;
}
void CALLBACK ExplorerForegroundProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG idObject,
                                     LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    (void)hWinEventHook;
    (void)dwEventThread;
    (void)dwmsEventTime;
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (!hWnd || g_unloading.load(std::memory_order_relaxed) ||
        !g_explorerAltTabTrackerEnabled.load(std::memory_order_relaxed) ||
        !g_switchAnimation.load(std::memory_order_relaxed)) {
        return;
    }
    const bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    if (!altDown) {
        if (IsAltTabSourceCandidate(hWnd)) {
            g_lastAppForeground.store(hWnd, std::memory_order_relaxed);
        }
        return;
    }
    HWND source = g_lastAppForeground.load(std::memory_order_relaxed);
    if (!source || source == hWnd || !IsAltTabSourceCandidate(source)) {
        if (g_altTabSessionPollRunning.load(std::memory_order_relaxed)) {
            TouchAltTabSession();
        }
        return;
    }
    if (!g_altTabSessionPollRunning.load(std::memory_order_relaxed)) {
        BeginAltTabSession(source);
        StartAltTabSessionPoll();
    } else {
        TouchAltTabSession();
    }
}
DWORD WINAPI ExplorerFgHookThread(LPVOID lpParam) {
    (void)lpParam;
    HWND initial = GetForegroundWindow();
    if (IsAltTabSourceCandidate(initial)) {
        g_lastAppForeground.store(initial, std::memory_order_relaxed);
    }
    PulseSharedHeartbeat();
    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        NULL,
        ExplorerForegroundProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT);
    g_hExplorerFgHook.store(hook, std::memory_order_release);
    if (!hook) {
        Wh_Log(L"Explorer FOREGROUND hook failed");
        return 0;
    }
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (HWINEVENTHOOK oldHook = g_hExplorerFgHook.exchange(NULL, std::memory_order_acquire)) {
        UnhookWinEvent(oldHook);
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
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
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
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        L"STATIC", NULL, WS_POPUP,
        ghostX, ghostY, ghostW, ghostH,
        NULL, NULL, NULL, NULL);
    HTHUMBNAIL hThumb = NULL;
    HRESULT hr = DwmRegisterThumbnail(hGhost, hWnd, &hThumb);
    if (FAILED(hr)) {
        Wh_Log(L"Switch thumbnail register failed hwnd=%p hr=0x%08X", hWnd, hr);
        DestroyWindow_Original(hGhost);
        return 0;
    }
    ShowWindow_Original(hGhost, SW_SHOWNOACTIVATE);
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
        FlushDwmOrYield();
    }
    if (IsWindow(hWnd)) {
        SetWindowCloak(hWnd, FALSE);
        guard.uncloaked = true;
    }
    if (hThumb) DwmUnregisterThumbnail(hThumb);
    DestroyWindow_Original(hGhost);
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
        g_AnimWantRising.erase(hWnd);
        g_AnimRestoreRequestForeground.erase(hWnd);
        g_AsyncRestoreReservations.erase(hWnd);
    }
    Wh_Log(L"Switch animation start hwnd=%p", hWnd);
    SetWindowCloak(hWnd, TRUE);
    auto* data = new (std::nothrow)
        SwitchAnimData{hWnd, g_switchDurationMs.load(std::memory_order_relaxed)};
    if (!data || !StartWorkerThread(SwitchingAnimThread, data)) {
        Wh_Log(L"Switch animation worker failed hwnd=%p", hWnd);
        SetWindowCloak(hWnd, FALSE);
        delete data;
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_AnimActive.erase(hWnd);
    }
}

static bool IsTaskbarEventWindow(HWND hWnd) {
    for (HWND current = hWnd; current; current = GetParent(current)) {
        WCHAR className[64]{};
        if (!GetClassNameW(current, className, ARRAYSIZE(className))) continue;
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            return true;
        }
    }
    return false;
}

static void IncrementSharedTaskbarLayoutEpoch() {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (!g_pSharedState || !g_sharedStateWritable) return;
    LONG epoch = InterlockedIncrement(&g_pSharedState->taskbarLayoutEpoch);
    if (epoch == 0) InterlockedIncrement(&g_pSharedState->taskbarLayoutEpoch);
}

static bool PublishSharedTaskbarObserver(DWORD explorerPid, bool ready) {
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (!g_pSharedState || !g_sharedStateWritable) return false;
    const LONG ownPid = static_cast<LONG>(explorerPid);
    const LONG previous = InterlockedCompareExchange(
        &g_pSharedState->taskbarObserverPid, ready ? ownPid : 0,
        ready ? 0 : ownPid);
    if (ready && previous != 0 && previous != ownPid) return false;
    if ((ready && previous != ownPid) || (!ready && previous == ownPid)) {
        LONG epoch = InterlockedIncrement(&g_pSharedState->taskbarLayoutEpoch);
        if (epoch == 0) InterlockedIncrement(&g_pSharedState->taskbarLayoutEpoch);
    }
    return ready ? (previous == 0 || previous == ownPid) : previous == ownPid;
}

void CALLBACK TaskbarLayoutEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd,
                                     LONG idObject, LONG idChild, DWORD dwEventThread,
                                     DWORD dwmsEventTime) {
    (void)hWinEventHook;
    (void)idChild;
    (void)dwEventThread;
    (void)dwmsEventTime;
    if (g_unloading.load(std::memory_order_relaxed) || !hWnd ||
        (event != EVENT_OBJECT_CREATE && event != EVENT_OBJECT_DESTROY &&
         event != EVENT_OBJECT_SHOW && event != EVENT_OBJECT_HIDE &&
         event != EVENT_OBJECT_REORDER && event != EVENT_OBJECT_LOCATIONCHANGE &&
         event != EVENT_OBJECT_HOSTEDOBJECTSINVALIDATED) ||
        !IsTaskbarEventWindow(hWnd)) {
        return;
    }
    if (idObject == OBJID_WINDOW &&
        (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE ||
         event == EVENT_OBJECT_LOCATIONCHANGE)) {
        return;
    }

    IncrementSharedTaskbarLayoutEpoch();
    std::lock_guard<std::mutex> lock(g_StateMutex);
    InvalidateTaskbarDockCachesLocked();
}

DWORD WINAPI WinEventHookThread(LPVOID lpParam) {
    const bool explorerProcess = IsShellExplorerProcess();
    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, ForegroundEventProc,
        GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    g_hForegroundHook.store(hook, std::memory_order_release);

    const bool observeTaskbar = explorerProcess;
    const DWORD explorerPid = observeTaskbar ? GetCurrentProcessId() : 0;
    HWINEVENTHOOK taskbarLayoutHook = nullptr;
    HWINEVENTHOOK taskbarLocationHook = nullptr;
    HWINEVENTHOOK taskbarHostedObjectsHook = nullptr;
    auto publishObserver = [&](bool ready) -> bool {
        return PublishSharedTaskbarObserver(explorerPid, ready);
    };
    auto removeTaskbarHooks = [&]() {
        if (taskbarLayoutHook) UnhookWinEvent(taskbarLayoutHook);
        if (taskbarLocationHook) UnhookWinEvent(taskbarLocationHook);
        if (taskbarHostedObjectsHook) UnhookWinEvent(taskbarHostedObjectsHook);
        taskbarLayoutHook = taskbarLocationHook = taskbarHostedObjectsHook = nullptr;
        g_hTaskbarLayoutHook.store(nullptr, std::memory_order_release);
        g_hTaskbarLocationHook.store(nullptr, std::memory_order_release);
        g_hTaskbarHostedObjectsHook.store(nullptr, std::memory_order_release);
        publishObserver(false);
    };
    auto installTaskbarHooks = [&]() -> bool {
        if (!observeTaskbar || !IsShellExplorerProcess()) return false;
        taskbarLayoutHook =
            SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_REORDER, nullptr,
                            TaskbarLayoutEventProc, explorerPid, 0, WINEVENT_OUTOFCONTEXT);
        taskbarLocationHook =
            SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                            nullptr, TaskbarLayoutEventProc, explorerPid, 0,
                            WINEVENT_OUTOFCONTEXT);
        taskbarHostedObjectsHook =
            SetWinEventHook(EVENT_OBJECT_HOSTEDOBJECTSINVALIDATED,
                            EVENT_OBJECT_HOSTEDOBJECTSINVALIDATED, nullptr,
                            TaskbarLayoutEventProc, explorerPid, 0,
                            WINEVENT_OUTOFCONTEXT);
        if (!taskbarLayoutHook || !taskbarLocationHook || !taskbarHostedObjectsHook) {
            removeTaskbarHooks();
            return false;
        }
        g_hTaskbarLayoutHook.store(taskbarLayoutHook, std::memory_order_release);
        g_hTaskbarLocationHook.store(taskbarLocationHook, std::memory_order_release);
        g_hTaskbarHostedObjectsHook.store(taskbarHostedObjectsHook,
                                          std::memory_order_release);
        if (!publishObserver(true)) {
            removeTaskbarHooks();
            return false;
        }
        return true;
    };
    bool taskbarHooksReady = installTaskbarHooks();
    UINT_PTR taskbarHookRetryTimer = 0;
    if (observeTaskbar && !taskbarHooksReady) {
        taskbarHookRetryTimer = SetTimer(nullptr, 0, 2000, nullptr);
        if (!taskbarHookRetryTimer) {
            Wh_Log(L"Taskbar layout hook retry timer failed");
        }
    }

    if (!hook && !observeTaskbar) {
        g_winEventThreadStarted.store(false, std::memory_order_relaxed);
        return 0;
    }
        
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_TIMER && msg.wParam == taskbarHookRetryTimer &&
            observeTaskbar && !taskbarHooksReady) {
            if (!IsShellExplorerProcess()) {
                KillTimer(nullptr, taskbarHookRetryTimer);
                taskbarHookRetryTimer = 0;
                continue;
            }
            taskbarHooksReady = installTaskbarHooks();
            if (taskbarHooksReady) {
                KillTimer(nullptr, taskbarHookRetryTimer);
                taskbarHookRetryTimer = 0;
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (taskbarHookRetryTimer) KillTimer(nullptr, taskbarHookRetryTimer);
    
    if (HWINEVENTHOOK oldHook = g_hForegroundHook.exchange(NULL, std::memory_order_acquire)) {
        UnhookWinEvent(oldHook);
    }
    removeTaskbarHooks();
    return 0;
}
class AnimationEngine {
private:
    WindowAnimData* data = nullptr;
    int W = 0, H = 0;
    int origLeft = 0, origTop = 0;
    float origCenterX = 0.0f;
    float dockXf = 0.0f, dockY = 0.0f, neckW = 0.0f;
    float taskbarDpiScale = 1.0f;
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
    bool ownershipReleased = false;
    bool keepGhostBelowTaskbar = false;
    std::vector<ShatterBlock> shatterBlocks;
    std::vector<float> yb;
    std::vector<float> meltLag;
    std::vector<float> meltSpeed;
    static constexpr int kMeltStripPx = 120;
    static constexpr int kMeltSubStepPx = 6;
    static constexpr float kMeltMaxLagStep = 0.08f;
    static constexpr float kMeltMaxSpeedStep = 0.12f;
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
    void BlitVerticalSquash(int dstBaseX, int dstBaseY, int dstH, float alpha, float bright) {
        if (dstH < 1 || alpha <= 0.0f) return;
        if (bright < 0.0f) bright = 0.0f;
        if (bright > 1.5f) bright = 1.5f;
        const float invH = 1.0f / (float)dstH;
        for (int dy = 0; dy < dstH; ++dy) {
            const int outY = dstBaseY + dy;
            if (outY < 0 || outY >= boundH) continue;
            const float srcYF = ((float)dy + 0.5f) * invH * (float)H - 0.5f;
            int y0 = (int)floorf(srcYF);
            float fy = srcYF - (float)y0;
            int y1 = y0 + 1;
            if (y0 < 0) { y0 = 0; fy = 0.0f; }
            if (y1 >= H) y1 = H - 1;
            if (y0 >= H) y0 = H - 1;
            const BYTE* row0 = srcBits + (size_t)y0 * srcStride;
            const BYTE* row1 = srcBits + (size_t)y1 * srcStride;
            BYTE* dstRow = pBits + (size_t)outY * canvasStride;
            const float w0 = 1.0f - fy;
            const float w1 = fy;
            for (int x = 0; x < W; ++x) {
                const int outX = dstBaseX + x;
                if (outX < 0 || outX >= boundW) continue;
                const BYTE* s0 = row0 + (size_t)x * 4;
                const BYTE* s1 = row1 + (size_t)x * 4;
                BYTE* d = dstRow + (size_t)outX * 4;
                float b = ((float)s0[0] * w0 + (float)s1[0] * w1) * bright;
                float g = ((float)s0[1] * w0 + (float)s1[1] * w1) * bright;
                float r = ((float)s0[2] * w0 + (float)s1[2] * w1) * bright;
                float a = ((float)s0[3] * w0 + (float)s1[3] * w1) * alpha;
                if (bright > 1.0f) {
                    const float lift = (bright - 1.0f) * 0.65f;
                    b += (255.0f - b) * lift;
                    g += (255.0f - g) * lift;
                    r += (220.0f - r) * lift;
                }
                d[0] = (BYTE)(b > 255.0f ? 255.0f : b);
                d[1] = (BYTE)(g > 255.0f ? 255.0f : g);
                d[2] = (BYTE)(r > 255.0f ? 255.0f : r);
                d[3] = (BYTE)(a > 255.0f ? 255.0f : a);
            }
        }
    }
    void DrawSharpCrtBeam(float cx, float cy, float halfLen, float intensity) {
        if (intensity <= 0.02f || halfLen < 0.25f) return;
        if (intensity > 1.0f) intensity = 1.0f;
        const float tip = 1.75f;
        const int x0 = (int)floorf(cx - halfLen - tip);
        const int x1 = (int)ceilf(cx + halfLen + tip);
        const int y0 = (int)floorf(cy - 2.0f);
        const int y1 = (int)ceilf(cy + 2.0f);
        for (int y = y0; y <= y1; ++y) {
            if (y < 0 || y >= boundH) continue;
            const float ady = fabsf((float)y + 0.5f - cy);
            float gy = 0.0f;
            if (ady < 0.55f) gy = 1.0f;
            else if (ady < 1.55f) gy = 1.0f - (ady - 0.55f);
            if (gy <= 0.0f) continue;
            BYTE* dstRow = pBits + (size_t)y * canvasStride;
            for (int x = x0; x <= x1; ++x) {
                if (x < 0 || x >= boundW) continue;
                const float adx = fabsf((float)x + 0.5f - cx);
                float gx = 1.0f;
                if (adx > halfLen) {
                    const float t = (adx - halfLen) / tip;
                    if (t >= 1.0f) continue;
                    gx = 1.0f - t;
                }
                const float a = gy * gx * intensity;
                if (a < 0.02f) continue;
                const float hot = gy;
                const float b = (210.0f + 45.0f * hot) * a;
                const float g = (235.0f + 20.0f * hot) * a;
                const float r = (255.0f) * a;
                const float aa = 255.0f * a;
                BYTE* d = dstRow + (size_t)x * 4;
                d[0] = (BYTE)(b > 255.0f ? 255.0f : b);
                d[1] = (BYTE)(g > 255.0f ? 255.0f : g);
                d[2] = (BYTE)(r > 255.0f ? 255.0f : r);
                d[3] = (BYTE)(aa > 255.0f ? 255.0f : aa);
            }
        }
    }
    void DrawSharpCrtDot(float cx, float cy, float intensity) {
        if (intensity <= 0.02f) return;
        if (intensity > 1.0f) intensity = 1.0f;
        const int x0 = (int)floorf(cx - 1.5f);
        const int x1 = (int)ceilf(cx + 1.5f);
        const int y0 = (int)floorf(cy - 1.5f);
        const int y1 = (int)ceilf(cy + 1.5f);
        for (int y = y0; y <= y1; ++y) {
            if (y < 0 || y >= boundH) continue;
            BYTE* dstRow = pBits + (size_t)y * canvasStride;
            for (int x = x0; x <= x1; ++x) {
                if (x < 0 || x >= boundW) continue;
                const float dx = (float)x + 0.5f - cx;
                const float dy = (float)y + 0.5f - cy;
                const float d2 = dx * dx + dy * dy;
                float w = 0.0f;
                if (d2 <= 0.35f) w = 1.0f;
                else if (d2 <= 1.6f) w = 1.0f - (d2 - 0.35f) / 1.25f;
                if (w <= 0.0f) continue;
                const float a = w * intensity;
                BYTE* d = dstRow + (size_t)x * 4;
                d[0] = (BYTE)(255.0f * a);
                d[1] = (BYTE)(255.0f * a);
                d[2] = (BYTE)(255.0f * a);
                d[3] = (BYTE)(255.0f * a);
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
        if (!srcBits || !pBits || W < 1 || H < 1) return;
        auto ihash = [](int x, int y) -> float {
            uint32_t h = (uint32_t)(x * 374761393) ^ (uint32_t)(y * 668265263);
            h = (h ^ (h >> 13)) * 1274126177u;
            return (h & 0xFFFFu) / 65535.0f;
        };
        auto smoothstep01 = [](float e0, float e1, float x) -> float {
            float t = Clamp((x - e0) / std::max(1e-5f, e1 - e0), 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        };
        const float p = progress * progress * (3.0f - 2.0f * progress);
        const int pBand = (int)floorf(p * 18.0f);
        const int pTear = (int)floorf(p * 32.0f);
        const int pMicro = (int)floorf(p * 64.0f);
        const int pBreak = (int)floorf(p * 22.0f);
        float alphaBase = 1.0f - smoothstep01(0.0f, 1.0f, p);
        alphaBase *= 1.0f - smoothstep01(0.55f, 1.0f, p);
        if (alphaBase <= 0.01f) return;
        const float breakupCut = 1.0f - p * 0.92f;
        const bool opaque = alphaBase >= 0.99f;
        const int dstBaseX = origLeft - boundLeft;
        const int dstBaseY = origTop - boundTop;
        if (breakupCut >= 0.92f) {
            for (int y = 0; y < H; ++y) {
                const int lineId = (int)((float)y / (float)H * 70.0f);
                const float bandMask = (ihash(lineId, pBand) >= 0.55f) ? 1.0f : 0.0f;
                const float tear =
                    (ihash((int)(lineId * 1.7f), pTear) - 0.5f) * 0.10f * bandMask;
                const float micro =
                    (ihash((int)((float)y / (float)H * 220.0f), pMicro) - 0.5f) * 0.025f;
                const int sx = (int)((tear + micro) * (float)W);
                DrawBlock(0, y, dstBaseX + sx, dstBaseY + y, alphaBase, W, 1);
            }
            return;
        }
        for (int y = 0; y < H; ++y) {
            const int lineId = (int)((float)y / (float)H * 70.0f);
            const float bandMask = (ihash(lineId, pBand) >= 0.55f) ? 1.0f : 0.0f;
            const float tear =
                (ihash((int)(lineId * 1.7f), pTear) - 0.5f) * 0.10f * bandMask;
            const float micro =
                (ihash((int)((float)y / (float)H * 220.0f), pMicro) - 0.5f) * 0.025f;
            const int tearX = (int)((tear + micro) * (float)W);
            const int dstY = dstBaseY + y;
            if (dstY < 0 || dstY >= boundH) continue;
            const BYTE* srcRow = srcBits + (size_t)y * srcStride;
            BYTE* dstRow = pBits + (size_t)dstY * canvasStride;
            for (int x = 0; x < W; ++x) {
                if (ihash(x, y + pBreak * 17) > breakupCut) continue;
                const int dstX = dstBaseX + x + tearX;
                if (dstX < 0 || dstX >= boundW) continue;
                const BYTE* src = srcRow + (size_t)x * 4;
                BYTE* dst = dstRow + (size_t)dstX * 4;
                if (opaque) {
                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];
                    dst[3] = src[3];
                } else {
                    dst[0] = (BYTE)(src[0] * alphaBase);
                    dst[1] = (BYTE)(src[1] * alphaBase);
                    dst[2] = (BYTE)(src[2] * alphaBase);
                    dst[3] = (BYTE)(src[3] * alphaBase);
                }
            }
        }
    }
    void RenderCrtOff(float progress, float& fade) {
        fade = 1.0f;
        const float squashEnd = AnimConstants::CrtSquashEnd;
        const float zipEnd = AnimConstants::CrtZipEnd;
        const float cx = (float)(origLeft - boundLeft) + (float)W * 0.5f;
        const float cy = (float)(origTop - boundTop) + (float)H * 0.5f;
        if (progress < squashEnd) {
            const float u = progress / squashEnd;
            const float punch = u * u * u;
            const float minH = 2.0f;
            const float dstHf = (float)H * (1.0f - punch) + minH * punch;
            const int dstH = std::max(2, (int)(dstHf + 0.5f));
            const int dstY = (int)(cy - (float)dstH * 0.5f);
            const int dstX = origLeft - boundLeft;
            const float bright = 1.0f + punch * 0.85f;
            const float handoffStart = 10.0f;
            float contentAlpha = 1.0f;
            float beamAlpha = 0.0f;
            if ((float)dstH < handoffStart) {
                const float t = ((float)dstH - minH) / (handoffStart - minH);
                contentAlpha = Clamp(t, 0.0f, 1.0f);
                beamAlpha = 1.0f - contentAlpha;
            }
            if (contentAlpha > 0.02f) {
                BlitVerticalSquash(dstX, dstY, dstH, contentAlpha, bright);
            }
            if (beamAlpha > 0.02f) {
                DrawSharpCrtBeam(cx, cy, (float)W * 0.5f, beamAlpha);
            }
            return;
        }
        if (progress < zipEnd) {
            const float u = (progress - squashEnd) / (zipEnd - squashEnd);
            const float ease = u * u;
            const float halfLen = (float)W * 0.5f * (1.0f - ease) + 1.0f * ease;
            DrawSharpCrtBeam(cx, cy, halfLen, 1.0f);
            return;
        }
        const float u = (progress - zipEnd) / std::max(0.01f, 1.0f - zipEnd);
        const float decay = 1.0f - u;
        if (decay > 0.02f) DrawSharpCrtDot(cx, cy, decay * decay);
    }
    void RenderPixelMelt(float progress, float& fade) {
        fade = 1.0f;
        const int stripCount = (int)meltLag.size();
        if (!srcBits || !pBits || stripCount < 1 || W < 1 || H < 1) return;
        const int baseX = origLeft - boundLeft;
        const int baseY = origTop - boundTop;
        const int maxH = std::max(1, boundH - 1);
        auto sampleStrip = [&](float px, float& lag, float& spd) {
            const float f = px / (float)kMeltStripPx - 0.5f;
            int i0 = (int)floorf(f);
            float u = f - (float)i0;
            if (i0 < 0) {
                i0 = 0;
                u = 0.0f;
            }
            int i1 = i0 + 1;
            if (i0 >= stripCount) i0 = stripCount - 1;
            if (i1 >= stripCount) i1 = stripCount - 1;
            u = u * u * (3.0f - 2.0f * u);
            lag = meltLag[(size_t)i0] * (1.0f - u) + meltLag[(size_t)i1] * u;
            spd = meltSpeed[(size_t)i0] * (1.0f - u) + meltSpeed[(size_t)i1] * u;
        };
        for (int x = 0; x < W; x += kMeltSubStepPx) {
            const int bw = std::min(kMeltSubStepPx, W - x);
            float lag = 0.0f, spd = 1.0f;
            sampleStrip((float)x + 0.5f * (float)bw, lag, spd);
            const int dstX = baseX + x;
            if (dstX >= boundW || dstX + bw <= 0) continue;

            if (progress <= lag) {
                DrawBlock(x, 0, dstX, baseY, 1.0f, bw, H);
                continue;
            }
            float t = (progress - lag) / std::max(0.2f, 1.0f - lag);
            if (t > 1.0f) t = 1.0f;
            t = t * t * t;
            const float fall = t * (float)H * (0.75f + 0.95f * spd);
            const float stretch = 1.0f + t * (0.3f + 0.7f * spd);
            int dstH = std::max(1, (int)((float)H * stretch + 0.5f));
            if (dstH > maxH) dstH = maxH;
            float alpha = 1.0f;
            if (t > 0.55f) {
                const float u = (t - 0.55f) / 0.45f;
                alpha = powf(1.0f - u, 1.2f);
            }
            if (alpha <= 0.02f) continue;

            const int dstY = baseY + (int)fall;
            if (dstY >= boundH || dstY + dstH <= 0) continue;
            DrawBlockScaled(x, 0, bw, H, dstX, dstY, bw, dstH, alpha);
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
    void RenderScorch(float progress, float& fade) {
        fade = 1.0f;
        float p = data->isRising ? progress : (1.0f - progress);
        p = p * p * (3.0f - 2.0f * p);
        const int dstBaseX = origLeft - boundLeft;
        const int dstBaseY = origTop - boundTop;
        for (const auto& b : shatterBlocks) {
            const float dist = hypotf(1.0f - b.dirX, 1.0f - b.dirY) * 1.55f - p * b.force;
            const float r = p - b.noiseX;
            const float reveal = (dist <= r) ? 1.0f : (p * p * p * p);
            if (reveal <= 0.01f) continue;
            DrawBlock(b.srcX, b.srcY, dstBaseX + b.srcX, dstBaseY + b.srcY, reveal, b.bw, b.bh);
        }
    }
    void RenderSplinter(float progress, float& fade) {
        fade = 1.0f;
        float p = data->isRising ? progress : (1.0f - progress);
        p = p * p * (3.0f - 2.0f * p);
        constexpr float fadeEdge = 0.08f;
        auto smooth01 = [](float e0, float e1, float x) -> float {
            float t = (x - e0) / (e1 - e0);
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            return t * t * (3.0f - 2.0f * t);
        };
        const float outer = smooth01(0.0f, fadeEdge, p);
        const float endFill = smooth01(1.0f - fadeEdge, 1.0f, p);
        const int dstBaseX = origLeft - boundLeft;
        const int dstBaseY = origTop - boundTop;
        for (const auto& b : shatterBlocks) {
            const float r = p - b.noiseX;
            const float hard = (b.noiseY <= r) ? 1.0f : 0.0f;
            const float inner = hard + (1.0f - hard) * endFill;
            const float reveal = outer * inner;
            if (reveal <= 0.01f) continue;
            DrawBlock(b.srcX, b.srcY, dstBaseX + b.srcX, dstBaseY + b.srcY, reveal, b.bw, b.bh);
        }
    }
    void RenderMirage(float progress, float& fade) {
        fade = 1.0f;
        float p = data->isRising ? progress : (1.0f - progress);
        p = p * p * (3.0f - 2.0f * p);
        const float inv = 1.0f - p;
        if (p <= 0.01f) return;
        const int dstBaseX = origLeft - boundLeft;
        const int dstBaseY = origTop - boundTop;
        for (const auto& b : shatterBlocks) {
            const float su = b.dirX + inv * b.force;
            const float sv = b.dirY + inv * b.noiseX;
            int sx = (int)(su * (float)W - 0.5f * (float)b.bw);
            int sy = (int)(sv * (float)H - 0.5f * (float)b.bh);
            if (sx < 0) sx = 0;
            if (sy < 0) sy = 0;
            if (sx > W - b.bw) sx = W - b.bw;
            if (sy > H - b.bh) sy = H - b.bh;
            DrawBlock(sx, sy, dstBaseX + b.srcX, dstBaseY + b.srcY, p, b.bw, b.bh);
        }
    }
    void RenderStipple(float progress, float& fade) {
        fade = 1.0f;
        float p = data->isRising ? progress : (1.0f - progress);
        p = p * p * (3.0f - 2.0f * p);
        const int dstBaseX = origLeft - boundLeft;
        const int dstBaseY = origTop - boundTop;
        for (const auto& b : shatterBlocks) {
            const float threshold = p / b.force;
            if (b.noiseY > threshold) continue;
            DrawBlock(b.srcX, b.srcY, dstBaseX + b.srcX, dstBaseY + b.srcY, 1.0f, b.bw, b.bh);
        }
    }
    void RenderSwell(float progress, float& fade) {
        const float t = data->isRising ? (1.0f - progress) : progress;
        const float u = 1.0f - t;
        const float eased = 1.0f - u * u * u;
        const float scale = 1.0f + 0.18f * eased;
        fade = data->isRising
                   ? progress * progress * (3.0f - 2.0f * progress)
                   : 1.0f - eased;
        if (data->isRising && fade < 0.04f) fade = 0.04f;
        const int dstW = std::max(1, (int)((float)W * scale + 0.5f));
        const int dstH = std::max(1, (int)((float)H * scale + 0.5f));
        const int dstX = (origLeft - boundLeft) + (W - dstW) / 2;
        const int dstY = (origTop - boundTop) + (H - dstH) / 2;
        DrawBlockScaled(0, 0, W, H, dstX, dstY, dstW, dstH, 1.0f);
    }
    void RenderWin10MinRestore(float progress, float& fade) {
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;
        float e = 0.0f; 
        if (data->isRising) {
            float inv = 1.0f - progress;
            e = 1.0f - (inv * inv * inv);
            fade = progress / 0.35f;
            if (fade > 1.0f) fade = 1.0f;
        } else {
            e = progress * progress;
            fade = 1.0f - (progress * progress);
            if (fade < 0.0f) fade = 0.0f;
        }
        const float fullL = (float)origLeft;
        const float fullT = (float)origTop;
        const float fullR = (float)(origLeft + W);
        const float fullB = (float)(origTop + H);
        const float btnW = 48.0f * taskbarDpiScale;
        const float btnH = 40.0f * taskbarDpiScale;
        const float tbCenterX = dockXf;
        const float tbCenterY = dockY + 2.0f * taskbarDpiScale + btnH * 0.5f;
        float curL, curT, curR, curB;
        if (data->isRising) {
            const float startScale = 0.85f;
            const float travelRatio = 0.12f;
            float fullCenterX = fullL + W * 0.5f;
            float fullCenterY = fullT + H * 0.5f;
            float startCenterX = fullCenterX + (tbCenterX - fullCenterX) * travelRatio;
            float startCenterY = fullCenterY + (tbCenterY - fullCenterY) * travelRatio;
            float startW = W * startScale;
            float startH = H * startScale;
            float startL = startCenterX - startW * 0.5f;
            float startT = startCenterY - startH * 0.5f;
            float startR = startCenterX + startW * 0.5f;
            float startB = startCenterY + startH * 0.5f;
            curL = startL + (fullL - startL) * e;
            curT = startT + (fullT - startT) * e;
            curR = startR + (fullR - startR) * e;
            curB = startB + (fullB - startB) * e;
        } else {
            const float endScaleMult = 1.6f; 
            const float endW = btnW * endScaleMult;
            const float endH = btnH * endScaleMult;
            const float endL = tbCenterX - endW * 0.5f;
            const float endT = tbCenterY - endH * 0.5f;
            const float endR = tbCenterX + endW * 0.5f;
            const float endB = tbCenterY + endH * 0.5f;
            curL = fullL + (endL - fullL) * e;
            curT = fullT + (endT - fullT) * e;
            curR = fullR + (endR - fullR) * e;
            curB = fullB + (endB - fullB) * e;
        }
        const int outX = (int)floorf(curL) - boundLeft;
        const int outY = (int)floorf(curT) - boundTop;
        const int outW = std::max(1, (int)ceilf(curR - curL));
        const int outH = std::max(1, (int)ceilf(curB - curT));
        DrawBlockScaled(0, 0, W, H, outX, outY, outW, outH, 1.0f);
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
        MONITORINFO mmi{};
        mmi.cbSize = sizeof(mmi);
        if (!hMon || !GetMonitorInfoW(hMon, &mmi)) {
            mmi.rcMonitor.left = 0; mmi.rcMonitor.top = 0;
            mmi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
            mmi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
            mmi.rcWork = mmi.rcMonitor;
        }
        HWND hTray = FindTaskbarForMonitor(hMon ? hMon : data->hMon);
        taskbarDpiScale =
            static_cast<float>(GetAnimationSpaceDpi(data->hRealWnd, hTray)) /
            96.0f;
        const int monLeft = (int)mmi.rcMonitor.left, monTop = (int)mmi.rcMonitor.top;
        const int monRight = (int)mmi.rcMonitor.right, monBottom = (int)mmi.rcMonitor.bottom;
        const int dockX = Clamp(data->targetDockX, monLeft, monRight);
        dockXf = (float)dockX;
        dockY = GetTaskbarDockY(data->hRealWnd,
                                hMon ? hMon : data->hMon, mmi,
                                data->requestedUnhide || data->deferredMinimize);
        if (dockY < (float)monTop + 1.0f) dockY = (float)monBottom;
        if (dockY > (float)monBottom) dockY = (float)monBottom;
        neckW = Clamp(W * 0.03f, 12.0f, 60.0f);
        blockSizeSetting = std::max(1, g_shatterBlockSize.load(std::memory_order_relaxed));
        if (data->isClosing) {
            closeEffect = data->effectStyle;
            minRestoreEffect = 0;
        } else {
            closeEffect = 1;
            minRestoreEffect = data->effectStyle;
        }
        keepGhostBelowTaskbar =
            !data->isClosing && (minRestoreEffect == 0 || minRestoreEffect == 7);
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
                padLeft = padRight = (int)(W * 0.14f) + 8;
                padTop = padBottom = 2;
            } else if (closeEffect == 4) {
                padLeft = padTop = padRight = padBottom = 4;
            } else if (closeEffect == 5) {
                padLeft = padRight = 4;
                padTop = 2;
                padBottom = (int)(H * 0.9f) + 16;
            } else {
                padLeft = padTop = padRight = padBottom = 50;
            }
            boundLeft = std::max(monLeft, origLeft - padLeft);
            boundTop = std::max(monTop, origTop - padTop);
            int boundRight = std::min(monRight, origLeft + W + padRight);
            int boundBottom = std::min(monBottom, origTop + H + padBottom);
            boundW = boundRight - boundLeft;
            boundH = boundBottom - boundTop;
        } else if (minRestoreEffect >= 1 && minRestoreEffect <= 5) {
            boundLeft = Clamp(origLeft, monLeft, monRight - 1);
            boundTop = Clamp(origTop, monTop, monBottom - 1);
            boundW = std::min(W, monRight - boundLeft);
            boundH = std::min(H, monBottom - boundTop);
        } else if (minRestoreEffect == 6) {
            const int padX = (int)ceilf((float)W * 0.10f) + 2;
            const int padY = (int)ceilf((float)H * 0.10f) + 2;
            boundLeft = std::max(monLeft, origLeft - padX);
            boundTop = std::max(monTop, origTop - padY);
            const int boundRight = std::min(monRight, origLeft + W + padX);
            const int boundBottom = std::min(monBottom, origTop + H + padY);
            boundW = boundRight - boundLeft;
            boundH = boundBottom - boundTop;
        } else if (minRestoreEffect == 7) {
            const int pad =
                std::max(1, static_cast<int>(ceilf(8.0f * taskbarDpiScale)));
            boundLeft = std::max(monLeft, std::min(origLeft, dockX) - pad);
            const int boundRight = std::min(monRight, std::max(origLeft + W, dockX) + pad);
            boundTop = std::max(monTop, std::min(origTop, (int)dockY) - pad);
            const int boundBottom = std::min(monBottom, std::max(origTop + H, (int)dockY + pad));
            boundW = boundRight - boundLeft;
            boundH = boundBottom - boundTop;
        } else {
            boundLeft = std::max(monLeft, std::min(origLeft, dockX) - W / 2);
            const int boundRight = std::min(monRight, std::max(origLeft + W, dockX) + W / 2);
            boundTop = std::max(monTop, origTop);
            const int boundBottom = std::min(monBottom, std::max(origTop + H, (int)dockY + 40));
            boundW = boundRight - boundLeft;
            boundH = boundBottom - boundTop;
        }
        if (boundW < 1) boundW = 1;
        if (boundH < 1) boundH = 1;
        totalMs = (double)data->durationMs;
    }
    void ShowGhostNoActivate() {
        if (keepGhostBelowTaskbar) {
            HWND hTray = FindTaskbarForMonitor(data->hMon);
            if (hTray && IsWindow(hTray) &&
                SetWindowPos_Original(
                    hGhost, hTray, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                        SWP_SHOWWINDOW)) {
                return;
            }
        }
        ShowWindow_Original(hGhost, SW_SHOWNOACTIVATE);
    }
    bool Initialize() {
        const DWORD ghostExStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                   WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
        hGhost = CreateWindowExW(
            ghostExStyle,
            L"STATIC", NULL, WS_POPUP,
            boundLeft, boundTop, boundW, boundH,
            NULL, NULL, NULL, NULL);
        if (!hGhost) return false;
        if (keepGhostBelowTaskbar) {
            SetPropW(hGhost, L"NonRudeHWND", reinterpret_cast<HANDLE>(TRUE));
        }
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
            ShowGhostNoActivate();
            if (data->deferredMinimize && IsWindow(data->hRealWnd)) {
                SetWindowCloak(data->hRealWnd, TRUE);
                data->hiddenByCloak = TRUE;
            }
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
        } else if (minRestoreEffect == 6) {
            const int dstW = std::max(1, (int)((float)W * 1.18f + 0.5f));
            const int dstH = std::max(1, (int)((float)H * 1.18f + 0.5f));
            const int dstX = (origLeft - boundLeft) + (W - dstW) / 2;
            const int dstY = (origTop - boundTop) + (H - dstH) / 2;
            DrawBlockScaled(0, 0, W, H, dstX, dstY, dstW, dstH, 1.0f);
            PresentCanvas(0.04f);
            FlushDwmOrYield();
            return;
        }
        PresentCanvas(1.0f);
        FlushDwmOrYield();
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
    bool PrecalcScorch() {
        auto hm_rand = [](float x, float y) -> float {
            const float n = sinf(x * 12.9898f + y * 78.233f) * 43758.5453f;
            return n - floorf(n);
        };
        auto hm_snoise = [](float vx, float vy) -> float {
            const float C0 = 0.211324865405187f;
            const float C1 = 0.366025403784439f;
            const float C2 = -0.577350269189626f;
            const float C3 = 0.024390243902439f;
            auto mod289v = [](float x) -> float { return x - floorf(x * (1.0f / 289.0f)) * 289.0f; };
            auto perm = [&](float x) -> float { return mod289v(((x * 34.0f) + 1.0f) * x); };
            float i_x = floorf(vx + (vx + vy) * C1);
            float i_y = floorf(vy + (vx + vy) * C1);
            float x0 = vx - i_x + (i_x + i_y) * C0;
            float y0 = vy - i_y + (i_x + i_y) * C0;
            const float i1x = (x0 > y0) ? 1.0f : 0.0f;
            const float i1y = (x0 > y0) ? 0.0f : 1.0f;
            float x1 = x0 + C0 - i1x;
            float y1 = y0 + C0 - i1y;
            float x2 = x0 + C2;
            float y2 = y0 + C2;
            i_x = mod289v(i_x);
            i_y = mod289v(i_y);
            const float q0 = perm(perm(i_y) + i_x);
            const float q1 = perm(perm(i_y + i1y) + i_x + i1x);
            const float q2 = perm(perm(i_y + 1.0f) + i_x + 1.0f);

            auto bake = [&](float q, float px, float py) -> float {
                float m = 0.5f - (px * px + py * py);
                if (m < 0.0f) return 0.0f;
                m *= m;
                m *= m;
                const float xf = 2.0f * (q * C3 - floorf(q * C3)) - 1.0f;
                const float h = fabsf(xf) - 0.5f;
                const float ox = floorf(xf + 0.5f);
                const float a0 = xf - ox;
                m *= 1.79284291400159f - 0.85373472095314f * (a0 * a0 + h * h);
                return m * (a0 * px + h * py);
            };
            return 130.0f * (bake(q0, x0, y0) + bake(q1, x1, y1) + bake(q2, x2, y2));
        };
        const int step = Clamp(std::min(W, H) / 96, 2, 5);
        try {
            shatterBlocks.reserve(((W + step - 1) / step) * ((H + step - 1) / step));
            for (int y = 0; y < H; y += step) {
                const int bh = std::min(step, H - y);
                for (int x = 0; x < W; x += step) {
                    const int bw = std::min(step, W - x);
                    const float uvx = ((float)x + 0.5f * (float)bw) / (float)W;
                    const float uvy = ((float)y + 0.5f * (float)bh) / (float)H;
                    const float n = hm_snoise(uvx * 2.8f, 0.0f);
                    const float expN = 0.42f + 0.58f * expf(n * 0.55f);
                    const float rnd = hm_rand(uvx * 2.8f, 0.1f) * 0.42f;
                    shatterBlocks.push_back({x, y, uvx, uvy, expN, rnd, 0.0f, bw, bh});
                }
            }
        } catch (const std::exception&) {
            Wh_Log(L"Scorch precalc failed");
            shatterBlocks.clear();
            return false;
        }
        return true;
    }
    bool PrecalcSplinter() {
        auto ch_rand = [](float x, float y) -> float {
            const float n = sinf(x * 12.9898f + y * 78.233f) * 43758.5453f;
            return n - floorf(n);
        };
        constexpr float threshold = 3.2f;
        const int step = Clamp(std::min(W, H) / 96, 2, 5);
        try {
            shatterBlocks.reserve(((W + step - 1) / step) * ((H + step - 1) / step));
            for (int y = 0; y < H; y += step) {
                const int bh = std::min(step, H - y);
                for (int x = 0; x < W; x += step) {
                    const int bw = std::min(step, W - x);
                    const float uvx = ((float)x + 0.5f * (float)bw) / (float)W;
                    const float uvy = ((float)y + 0.5f * (float)bh) / (float)H;
                    const float dist = hypotf(uvx - 0.5f, uvy - 0.5f) / threshold;
                    const float ry = ch_rand(uvy * 2.6f, 0.0f);
                    const float rx = ch_rand(0.0f, uvx * 2.6f);
                    const float jitter = (std::min)(ry, rx) * 0.38f;
                    shatterBlocks.push_back({x, y, uvx, uvy, 0.0f, jitter, dist, bw, bh});
                }
            }
        } catch (const std::exception&) {
            Wh_Log(L"Splinter precalc failed");
            shatterBlocks.clear();
            return false;
        }
        return true;
    }
    bool PrecalcMirage() {
        constexpr float sz = 0.035f;
        constexpr float zoom = 62.0f;
        const int step = Clamp(std::min(W, H) / 96, 2, 4);
        try {
            shatterBlocks.reserve(((W + step - 1) / step) * ((H + step - 1) / step));
            for (int y = 0; y < H; y += step) {
                const int bh = std::min(step, H - y);
                for (int x = 0; x < W; x += step) {
                    const int bw = std::min(step, W - x);
                    const float uvx = ((float)x + 0.5f * (float)bw) / (float)W;
                    const float uvy = ((float)y + 0.5f * (float)bh) / (float)H;
                    const float dx = sz * cosf(zoom * uvx);
                    const float dy = sz * sinf(zoom * uvy);
                    shatterBlocks.push_back({x, y, uvx, uvy, dx, dy, 0.0f, bw, bh});
                }
            }
        } catch (const std::exception&) {
            Wh_Log(L"Mirage precalc failed");
            shatterBlocks.clear();
            return false;
        }
        return true;
    }
    bool PrecalcStipple() {
        constexpr float dots = 28.0f;
        const int step = Clamp(std::min(W, H) / 110, 2, 4);
        try {
            shatterBlocks.reserve(((W + step - 1) / step) * ((H + step - 1) / step));
            for (int y = 0; y < H; y += step) {
                const int bh = std::min(step, H - y);
                for (int x = 0; x < W; x += step) {
                    const int bw = std::min(step, W - x);
                    const float uvx = ((float)x + 0.5f * (float)bw) / (float)W;
                    const float uvy = ((float)y + 0.5f * (float)bh) / (float)H;
                    float fx = uvx * dots;
                    float fy = uvy * dots;
                    fx -= floorf(fx);
                    fy -= floorf(fy);
                    const float cellDist = hypotf(fx - 0.5f, fy - 0.5f);
                    const float originDist = (std::max)(hypotf(uvx, uvy), 0.0001f);
                    shatterBlocks.push_back({x, y, uvx, uvy, originDist, 0.0f, cellDist, bw, bh});
                }
            }
        } catch (const std::exception&) {
            Wh_Log(L"Stipple precalc failed");
            shatterBlocks.clear();
            return false;
        }
        return true;
    }
    bool PrecalcPixelMelt() {
        auto hash01 = [](int ix) -> float {
            uint32_t h = (uint32_t)ix * 747796405u + 2891336453u;
            h = (h ^ (h >> 14)) * 277803737u;
            h ^= h >> 13;
            return (h & 0xFFFFFFu) / 16777215.0f;
        };
        auto valueNoise = [&](float x) -> float {
            const int i = (int)floorf(x);
            float f = x - (float)i;
            f = f * f * (3.0f - 2.0f * f);
            return hash01(i) * (1.0f - f) + hash01(i + 1) * f;
        };
        auto fbm = [&](float x) -> float {
            float v = 0.0f, a = 0.5f, n = 0.0f;
            for (int o = 0; o < 4; ++o) {
                v += a * valueNoise(x);
                n += a;
                x = x * 2.03f + 17.0f;
                a *= 0.5f;
            }
            return v / std::max(0.001f, n);
        };
        try {
            const int stripCount = (W + kMeltStripPx - 1) / kMeltStripPx;
            meltLag.resize((size_t)stripCount);
            meltSpeed.resize((size_t)stripCount);
            for (int si = 0; si < stripCount; ++si) {
                const float a = fbm((float)si * 0.85f);
                const float b = fbm((float)si * 1.7f + 9.1f);
                float lagN = Clamp(0.6f * a + 0.4f * b, 0.0f, 1.0f);
                lagN = lagN * lagN * (3.0f - 2.0f * lagN);
                meltLag[(size_t)si] = 0.02f + lagN * 0.52f;
                meltSpeed[(size_t)si] = 0.4f + (1.0f - lagN) * 0.85f;
            }
            auto limitSteps = [&](std::vector<float>& v, float maxStep) {
                for (int pass = 0; pass < 3; ++pass) {
                    for (int i = 1; i < stripCount; ++i) {
                        const float d = v[(size_t)i] - v[(size_t)i - 1];
                        if (d > maxStep) v[(size_t)i] = v[(size_t)i - 1] + maxStep;
                        if (d < -maxStep) v[(size_t)i] = v[(size_t)i - 1] - maxStep;
                    }
                    for (int i = stripCount - 2; i >= 0; --i) {
                        const float d = v[(size_t)i] - v[(size_t)i + 1];
                        if (d > maxStep) v[(size_t)i] = v[(size_t)i + 1] + maxStep;
                        if (d < -maxStep) v[(size_t)i] = v[(size_t)i + 1] - maxStep;
                    }
                }
            };
            limitSteps(meltLag, kMeltMaxLagStep);
            limitSteps(meltSpeed, kMeltMaxSpeedStep);
        } catch (const std::exception&) {
            Wh_Log(L"Pixel melt precalc failed");
            meltLag.clear();
            meltSpeed.clear();
            return false;
        }
        return true;
    }
    bool PrecalcPhysics() {
        if (!data->isClosing) {
            if (minRestoreEffect == 1) return PrecalcInkSplash();
            if (minRestoreEffect == 2) return PrecalcScorch();
            if (minRestoreEffect == 3) return PrecalcSplinter();
            if (minRestoreEffect == 4) return PrecalcMirage();
            if (minRestoreEffect == 5) return PrecalcStipple();
            if (minRestoreEffect == 6 || minRestoreEffect == 7) return true;
            return true;
        }
        if (closeEffect == 3 || closeEffect == 4) return true;
        if (closeEffect == 5) return PrecalcPixelMelt();
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
    NativeMinimizeState WaitForNativeMinimizeSubmission() {
        NativeMinimizeBarrier* barrier = data->nativeMinimizeBarrier;
        if (!barrier) return NativeMinimizeState::Cancelled;
        const DWORD deadline = GetTickCount() + AnimConstants::NativeStateWaitMs;
        for (;;) {
            const DWORD wait = WaitForSingleObject(barrier->submitted, 50);
            if (wait == WAIT_OBJECT_0) {
                if (g_unloading.load(std::memory_order_relaxed) &&
                    IsWindow(data->hRealWnd)) {
                    data->nativeStateTimedOut = TRUE;
                }
                return barrier->state.load(std::memory_order_acquire);
            }
            if (wait != WAIT_TIMEOUT || !IsWindow(data->hRealWnd) ||
                g_unloading.load(std::memory_order_relaxed)) {
                if (IsWindow(data->hRealWnd)) data->nativeStateTimedOut = TRUE;
                return NativeMinimizeState::Cancelled;
            }
            if ((LONG)(GetTickCount() - deadline) >= 0) {
                NativeMinimizeState expected = NativeMinimizeState::Pending;
                if (barrier->state.compare_exchange_strong(
                        expected, NativeMinimizeState::Cancelled,
                        std::memory_order_acq_rel)) {
                    SetEvent(barrier->submitted);
                }
                data->nativeStateTimedOut = TRUE;
                Wh_Log(L"Native minimize submission timeout hwnd=%p state=%ld",
                       data->hRealWnd,
                       static_cast<LONG>(barrier->state.load(std::memory_order_acquire)));
                return barrier->state.load(std::memory_order_acquire);
            }
        }
    }
    bool WaitForNativeMinimizeForEndpoint() {
        const NativeMinimizeState state = WaitForNativeMinimizeSubmission();
        if (data->nativeStateTimedOut || state != NativeMinimizeState::AsyncSubmitted) {
            return true;
        }

        if (IsIconic(data->hRealWnd)) data->nativeAsyncMinimizeObserved = TRUE;
        if (data->nativeAsyncMinimizeObserved) return true;
        const DWORD deadline = GetTickCount() + AnimConstants::NativeStateWaitMs;
        while (IsWindow(data->hRealWnd) && !IsIconic(data->hRealWnd) &&
               !g_unloading.load(std::memory_order_relaxed) &&
               (LONG)(GetTickCount() - deadline) < 0) {
            Sleep(10);
        }
        if (IsIconic(data->hRealWnd)) data->nativeAsyncMinimizeObserved = TRUE;
        if (!data->nativeAsyncMinimizeObserved && IsWindow(data->hRealWnd)) {
            data->nativeStateTimedOut = TRUE;
            if (!g_unloading.load(std::memory_order_relaxed)) {
                Wh_Log(L"Native async minimize state timeout hwnd=%p", data->hRealWnd);
            }
        }
        return true;
    }
    void SignalFirstFrameShown() {
        if (!data->hFirstFrameShown) return;
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
        data->hFirstFrameShown = nullptr;
    }
    bool ApplyDirectionState(bool wantRising) {
        if ((BOOL)wantRising == data->isRising) return false;

        HWND hWnd = data->hRealWnd;
        if (wantRising) {
            if (data->restoreMaximized) {
                ArmMaximizedRestoreGuard(
                    hWnd,
                    static_cast<DWORD>(AnimConstants::MaximizedRestoreGuardMs));
            }
            data->deferredMinimize = FALSE;
            if (IsWindow(hWnd)) {
                UpdateDwmTransitions(hWnd, FALSE);
                SetWindowCloak(hWnd, TRUE);
                if (IsIconic(hWnd)) {
                    if (data->nativeMinimizeBarrier) {
                        data->nativeAsyncMinimizeObserved = TRUE;
                    }
                    if (!data->nativeStateTimedOut) {
                        RestoreWindowUnderGhost(hWnd, data->originalExStyle,
                                                data->restoreMaximized);
                    }
                }
                data->hiddenByCloak = TRUE;
            }
            data->isRising = TRUE;
        } else {
            if (IsWindow(hWnd) && !IsIconic(hWnd)) {
                UpdateDwmTransitions(hWnd, FALSE);
                SetWindowCloak(hWnd, TRUE);
                data->hiddenByCloak = TRUE;
                data->deferredMinimize = TRUE;
                if (data->deferredShowCmd != SW_SHOWMINNOACTIVE &&
                    data->deferredShowCmd != SW_SHOWMINIMIZED &&
                    data->deferredShowCmd != SW_MINIMIZE) {
                    data->deferredShowCmd = SW_MINIMIZE;
                }
            } else {
                data->deferredMinimize = FALSE;
            }
            data->isRising = FALSE;
        }
        return true;
    }
    bool ApplyWantedDirectionState() {
        bool wantRising = false;
        if (!GetAnimWantRising(data->hRealWnd, &wantRising)) return false;
        return ApplyDirectionState(wantRising);
    }
    bool TryReverseToWantedDirection(const LARGE_INTEGER& qpcFreq, LARGE_INTEGER& qpcStart) {
        LARGE_INTEGER qpcNow;
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        double progress = totalMs > 0.0 ? (elapsedMs / totalMs) : 1.0;
        if (progress < 0.0) progress = 0.0;
        if (progress > 1.0) progress = 1.0;

        if (!ApplyWantedDirectionState()) return false;

        const double newElapsed = totalMs * (1.0 - progress);
        QueryPerformanceCounter(&qpcNow);
        qpcStart.QuadPart =
            qpcNow.QuadPart - (LONGLONG)(newElapsed * (double)qpcFreq.QuadPart / 1000.0);
        Wh_Log(L"Anim reverse hwnd=%p -> %s at p=%.2f", data->hRealWnd,
               data->isRising ? L"restore" : L"minimize", progress);
        return true;
    }
    void RunLoop() {
        LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
        QueryPerformanceFrequency(&qpcFreq);
        QueryPerformanceCounter(&qpcStart);
        for (;;) {
            if (!data->isClosing) TryReverseToWantedDirection(qpcFreq, qpcStart);
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (!data->isClosing) TryReverseToWantedDirection(qpcFreq, qpcStart);
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
                else if (closeEffect == 4) RenderCrtOff(progress, fade);
                else if (closeEffect == 5) RenderPixelMelt(progress, fade);
                else RenderThanos(progress, fade);
            } else if (minRestoreEffect == 1) {
                RenderInkSplash(progress, fade);
            } else if (minRestoreEffect == 2) {
                RenderScorch(progress, fade);
            } else if (minRestoreEffect == 3) {
                RenderSplinter(progress, fade);
            } else if (minRestoreEffect == 4) {
                RenderMirage(progress, fade);
            } else if (minRestoreEffect == 5) {
                RenderStipple(progress, fade);
            } else if (minRestoreEffect == 6) {
                RenderSwell(progress, fade);
            } else if (minRestoreEffect == 7) {
                RenderWin10MinRestore(progress, fade);
            } else {
                RenderMinimizeRestore(progress, fade);
            }
            PresentCanvas(fade);
            if (g_unloading.load(std::memory_order_relaxed)) break;
            if (lastFrame) {
                if (!data->isClosing && TryReverseToWantedDirection(qpcFreq, qpcStart)) continue;
                break;
            }
            FlushDwmOrYield();
        }
    }
    void FinishRising() {
        if (data->isClosing || !data->isRising) return;
        for (int attempt = 0; attempt < 4 && IsIconic(data->hRealWnd); ++attempt) {
            RestoreWindowUnderGhost(data->hRealWnd, data->originalExStyle,
                                    data->restoreMaximized);
            if (IsIconic(data->hRealWnd)) Sleep(5);
        }
        if (data->restoreMaximized && IsWindow(data->hRealWnd) &&
            !IsZoomed(data->hRealWnd)) {
            RestoreWindowUnderGhost(data->hRealWnd, data->originalExStyle,
                                    TRUE);
        }
        if (data->hiddenByCloak) {
            SetWindowCloak(data->hRealWnd, FALSE);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhost(data->hRealWnd, data->originalExStyle);
            RefreshDwmChromeAfterUncloak(data->hRealWnd);
        } else {
            RestoreLayeredOpacity(data->hRealWnd, data->originalExStyle);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhost(data->hRealWnd, data->originalExStyle);
        }
        FlushDwmOrYield();
    }
    void FinishTimedOutNativeState() {
        if (!IsWindow(data->hRealWnd)) return;
        if (data->hiddenByCloak) {
            SetWindowCloak(data->hRealWnd, FALSE);
        } else {
            RestoreLayeredOpacity(data->hRealWnd, data->originalExStyle);
        }
        UpdateDwmTransitions(data->hRealWnd, TRUE);
        RestoreZOrderAfterGhostAsync(data->hRealWnd, data->originalExStyle);
        data->deferredMinimize = FALSE;
    }
    void FinishClose() {
        if (!data->isClosing) return;
        if (data->hWaitFinish) {
            SetEvent(data->hWaitFinish);
            CloseHandle(data->hWaitFinish);
            data->hWaitFinish = NULL;
        }
        if (data->closeMsg == WM_DESTROY || !IsWindow(data->hRealWnd)) return;
        SetPropW(data->hRealWnd, kPropCloseBypass, (HANDLE)1);
        if (data->closeMsg == ANIM_DEFER_SW_HIDE) ShowWindowAsync_Original(data->hRealWnd, SW_HIDE);
        else if (data->closeMsg == WM_CLOSE) PostMessageW(data->hRealWnd, WM_CLOSE, 0, 0);
        else if (data->closeMsg == WM_SYSCOMMAND) PostMessageW(data->hRealWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        else PostMessageW(data->hRealWnd, data->closeMsg, 0, 0);
        for (int i = 0; i < 50; ++i) {
            if (!IsWindow(data->hRealWnd) || g_unloading.load(std::memory_order_relaxed) ||
                !IsWindowVisible(data->hRealWnd)) {
                break;
            }
            Sleep(10);
        }
        if (IsWindow(data->hRealWnd)) {
            RemovePropW(data->hRealWnd, kPropCloseBypass);
            RemovePropW(data->hRealWnd, kPropClosed);
            SetWindowCloak(data->hRealWnd, FALSE);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
        }
    }
    bool FinishDeferredMinimize() {
        if (!data->deferredMinimize) return true;
        const int unhideMs = data->unhideDurationMs;
        const int animMs = data->durationMs;
        if (data->requestedUnhide && unhideMs > animMs) {
            int remaining = unhideMs - animMs;
            while (remaining > 0 && !g_unloading.load(std::memory_order_relaxed)) {
                bool wantRising = false;
                if (GetAnimWantRising(data->hRealWnd, &wantRising) && wantRising) {
                    return false;
                }
                const int chunk = remaining < 20 ? remaining : 20;
                Sleep(chunk);
                remaining -= chunk;
            }
        }
        {
            bool wantRising = false;
            if (GetAnimWantRising(data->hRealWnd, &wantRising) && wantRising) {
                return false;
            }
        }
        if (!g_unloading.load(std::memory_order_relaxed) && IsWindow(data->hRealWnd)) {
            if (data->deferredShowCmd == SW_SHOWMINNOACTIVE ||
                data->deferredShowCmd == SW_SHOWMINIMIZED ||
                data->deferredShowCmd == SW_MINIMIZE) {
                ShowWindow_Original(data->hRealWnd, data->deferredShowCmd);
            } else {
                ShowWindow_Original(data->hRealWnd, SW_MINIMIZE);
            }
        }
        {
            bool wantRising = false;
            if (GetAnimWantRising(data->hRealWnd, &wantRising) && wantRising) {
                return false;
            }
        }
        data->deferredMinimize = FALSE;
        if (IsWindow(data->hRealWnd)) {
            SetWindowCloak(data->hRealWnd, FALSE);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhost(data->hRealWnd, data->originalExStyle);
        }
        return true;
    }
    bool ReleaseTaskbarRevealFocus() {
        if (!data->requestedUnhide || !data->taskbarFocusBorrowed) return false;
        HWND hTray = FindTaskbarForMonitor(data->hMon);
        if (!hTray || GetForegroundWindow() != hTray) {
            data->taskbarFocusBorrowed = FALSE;
            return false;
        }
        if (!RestoreForegroundAfterTaskbarReveal(hTray, data->hRealWnd, data->hNextApp,
                                                data->isRising != FALSE)) {
            return false;
        }
        data->taskbarFocusBorrowed = FALSE;
        return true;
    }
    void ReacquireTaskbarRevealFocus() {
        if (!data->requestedUnhide || data->taskbarFocusBorrowed ||
            g_unloading.load(std::memory_order_relaxed)) {
            return;
        }
        HWND hTray = FindTaskbarForMonitor(data->hMon);
        if (!hTray || !SetForegroundWindow(hTray)) return;
        data->taskbarFocusBorrowed = TRUE;
        WaitForTaskbarExpanded(data->hRealWnd, hTray, data->hMon);
    }
    bool ActivateRestoredWindowIfRequested() {
        if (!data->isRising || !IsWindow(data->hRealWnd)) return false;
        HWND hRequestForeground = NULL;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto it = g_AnimRestoreRequestForeground.find(data->hRealWnd);
            if (it == g_AnimRestoreRequestForeground.end()) return false;
            hRequestForeground = it->second;
        }
        HWND hForeground = GetForegroundWindow();
        HWND hTray = FindTaskbarForMonitor(data->hMon);
        if (hForeground == hRequestForeground || (hTray && hForeground == hTray)) {
            return SetForegroundWindow(data->hRealWnd);
        }
        return false;
    }
    bool CompleteMinRestoreEndpoint() {
        if (data->isClosing) return true;
        if (data->launchAnimationToken &&
            !IsLaunchAnimationCurrent(data->hRealWnd,
                                      data->launchAnimationToken)) {
            // The launch HWND was destroyed/reused. WM_DESTROY owns state-map
            // cleanup; the stale worker must not mutate the replacement.
            ownershipReleased = true;
            return true;
        }
        if (g_unloading.load(std::memory_order_relaxed) && IsWindow(data->hRealWnd)) {
            data->nativeStateTimedOut = TRUE;
        }
        for (;;) {
            if (ApplyWantedDirectionState()) return false;
            if (!WaitForNativeMinimizeForEndpoint()) {
                ApplyWantedDirectionState();
                return false;
            }
            if (ApplyWantedDirectionState()) return false;

            if (data->nativeStateTimedOut) {
                // The target stopped acknowledging an earlier native request. Don't
                // replace the bounded wait with a synchronous cross-thread window call.
                // The stable handoff below queues the latest intent asynchronously.
                FinishTimedOutNativeState();
            } else if (data->isRising) {
                FinishRising();
            } else {
                if (!FinishDeferredMinimize()) continue;
                if (IsWindow(data->hRealWnd)) {
                    SetWindowCloak(data->hRealWnd, FALSE);
                    UpdateDwmTransitions(data->hRealWnd, TRUE);
                    RestoreZOrderAfterGhost(data->hRealWnd, data->originalExStyle);
                }
            }
            bool directionChanged = false;
            if (hGhost) ShowWindow_Original(hGhost, SW_HIDE);
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                auto it = g_AnimWantRising.find(data->hRealWnd);
                directionChanged =
                    it != g_AnimWantRising.end() && it->second != (data->isRising != FALSE);
            }
            if (directionChanged) {
                if (hGhost) ShowGhostNoActivate();
                continue;
            }

            const bool restoredWindowActivated =
                !data->nativeStateTimedOut && ActivateRestoredWindowIfRequested();
            bool taskbarFocusReleased = false;
            if (restoredWindowActivated && data->taskbarFocusBorrowed) {
                data->taskbarFocusBorrowed = FALSE;
                taskbarFocusReleased = true;
            } else {
                taskbarFocusReleased = ReleaseTaskbarRevealFocus();
            }
            HBITMAP endpointSnapshot = nullptr;
            void* endpointSnapshotBits = nullptr;
            if (!data->isRising && data->pBits && hScreenDC &&
                g_restoreAnimation.load(std::memory_order_relaxed)) {
                bool snapshotMissing = false;
                {
                    std::lock_guard<std::mutex> lock(g_StateMutex);
                    auto snapshotIt = g_WndSnapshots.find(data->hRealWnd);
                    const bool validSnapshot =
                        snapshotIt != g_WndSnapshots.end() &&
                        snapshotIt->second.pBits && snapshotIt->second.w == W &&
                        snapshotIt->second.h == H &&
                        snapshotIt->second.windowToken &&
                        reinterpret_cast<ULONG_PTR>(GetPropW(
                            data->hRealWnd, kPropSnapshotCache)) ==
                            snapshotIt->second.windowToken;
                    if (!validSnapshot &&
                        snapshotIt != g_WndSnapshots.end()) {
                        EraseSnapshotLocked(snapshotIt);
                    }
                    snapshotMissing = !validSnapshot;
                }
                if (snapshotMissing) {
                    endpointSnapshot = CreateDib32(
                        hScreenDC, W, H, &endpointSnapshotBits);
                    if (endpointSnapshot && endpointSnapshotBits) {
                        memcpy(endpointSnapshotBits, data->pBits,
                               (size_t)W * (size_t)H * 4u);
                    } else if (endpointSnapshot) {
                        DeleteObject(endpointSnapshot);
                        endpointSnapshot = nullptr;
                    }
                }
            }
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                auto it = g_AnimWantRising.find(data->hRealWnd);
                directionChanged =
                    it != g_AnimWantRising.end() && it->second != (data->isRising != FALSE);
                if (!directionChanged) {
                    if (data->isRising) {
                        EraseSnapshotLocked(data->hRealWnd);
                    } else if (endpointSnapshot && endpointSnapshotBits) {
                        if (StoreSnapshotLocked(
                                data->hRealWnd, endpointSnapshot,
                                endpointSnapshotBits, W, H)) {
                            endpointSnapshot = nullptr;
                            endpointSnapshotBits = nullptr;
                        }
                    }
                    if (data->nativeStateTimedOut && IsWindow(data->hRealWnd) &&
                        !g_unloading.load(std::memory_order_relaxed)) {
                        int finalShowCmd =
                            StableRestoreShowCmd(data->restoreMaximized);
                        if (!data->isRising) {
                            finalShowCmd =
                                data->deferredShowCmd == SW_SHOWMINNOACTIVE ||
                                        data->deferredShowCmd == SW_SHOWMINIMIZED ||
                                        data->deferredShowCmd == SW_MINIMIZE
                                    ? data->deferredShowCmd
                                    : SW_MINIMIZE;
                        }
                        ShowWindowAsync_Original(data->hRealWnd, finalShowCmd);
                    }
                    if (data->isRising && data->pairedEffectToken) {
                        ClearMinRestorePairIfCurrent(
                            data->hRealWnd, data->pairedEffectToken);
                    }
                    if (data->isRising && data->restoreMaximized) {
                        ArmMaximizedRestoreGuard(
                            data->hRealWnd,
                            static_cast<DWORD>(GetDoubleClickTime() + 100));
                    }
                    if (data->launchAnimationToken) {
                        ClearLaunchAnimationIfCurrent(
                            data->hRealWnd, data->launchAnimationToken);
                    }
                    g_AnimActive.erase(data->hRealWnd);
                    g_AnimWantRising.erase(data->hRealWnd);
                    g_AnimRestoreRequestForeground.erase(data->hRealWnd);
                    g_AsyncRestoreReservations.erase(data->hRealWnd);
                    ownershipReleased = true;
                }
            }
            if (endpointSnapshot) DeleteObject(endpointSnapshot);
            if (!directionChanged) {
                data->requestedUnhide = FALSE;
                return true;
            }
            if (taskbarFocusReleased) ReacquireTaskbarRevealFocus();
            if (hGhost) ShowGhostNoActivate();
        }
    }
    void ReleaseGdiResources() {
        if (hCanvasDC && hOldCanvas && hOldCanvas != HGDI_ERROR) SelectObject(hCanvasDC, hOldCanvas);
        if (hSrcDibDC && hOldSrcDib && hOldSrcDib != HGDI_ERROR) SelectObject(hSrcDibDC, hOldSrcDib);
        if (hSrcDC && hOldSrc && hOldSrc != HGDI_ERROR) SelectObject(hSrcDC, hOldSrc);
        if (hCanvas) DeleteObject(hCanvas);
        if (hSrcDib) DeleteObject(hSrcDib);
        if (data->hBitmap) DeleteObject(data->hBitmap);
        if (hCanvasDC) DeleteDC(hCanvasDC);
        if (hSrcDibDC) DeleteDC(hSrcDibDC);
        if (hSrcDC) DeleteDC(hSrcDC);
        hCanvas = hSrcDib = data->hBitmap = nullptr;
        hCanvasDC = hSrcDibDC = hSrcDC = nullptr;
    }
    void Teardown() {
        SignalFirstFrameShown();
        if (!data->isClosing && !ownershipReleased) {
            if (g_unloading.load(std::memory_order_relaxed) && data->deferredMinimize) {
                data->deferredMinimize = FALSE;
                data->isRising = TRUE;
                std::lock_guard<std::mutex> lock(g_StateMutex);
                auto directionIt = g_AnimWantRising.find(data->hRealWnd);
                if (directionIt != g_AnimWantRising.end()) directionIt->second = true;
            }
            while (!CompleteMinRestoreEndpoint()) {
            }
        }
        if (hScreenDC) {
            ReleaseDC(NULL, hScreenDC);
            hScreenDC = nullptr;
        }
        if (hGhost) {
            DestroyWindow_Original(hGhost);
            hGhost = nullptr;
        }
        FinishClose();
        if (!data->isClosing && !ownershipReleased && !data->isRising && !data->deferredMinimize) {
            SetWindowCloak(data->hRealWnd, FALSE);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhost(data->hRealWnd, data->originalExStyle);
        }
        ReleaseGdiResources();
        if (!data->isClosing && !ownershipReleased && !FinishDeferredMinimize()) {
            ApplyWantedDirectionState();
            FinishRising();
        }
        if (!data->isClosing && !ownershipReleased) {
            ActivateRestoredWindowIfRequested();
            ReleaseTaskbarRevealFocus();
        }
        if (!ownershipReleased) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            g_AnimActive.erase(data->hRealWnd);
            g_AnimWantRising.erase(data->hRealWnd);
            g_AnimRestoreRequestForeground.erase(data->hRealWnd);
            g_AsyncRestoreReservations.erase(data->hRealWnd);
        }
        ReleaseNativeMinimizeBarrier(data->nativeMinimizeBarrier);
        data->nativeMinimizeBarrier = nullptr;
        delete data;
        data = nullptr;
    }
};
DWORD WINAPI MainAnimThread(LPVOID lpParam) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    auto* data = (WindowAnimData*)lpParam;
    AnimationEngine engine(data);
    if (engine.Initialize()) {
        const int style = data->effectStyle;
        const bool inplaceBootstrap = !data->isClosing && (style >= 1 && style <= 6);
        const bool meltBootstrap = data->isClosing && style == 5;
        if (inplaceBootstrap || meltBootstrap) {
            engine.ShowInkBootstrapFrame();
        }
        if (engine.PrecalcPhysics()) {
            if (data->isClosing) {
                engine.RunLoop();
            } else {
                do {
                    engine.RunLoop();
                } while (!g_unloading.load(std::memory_order_relaxed) &&
                         !engine.CompleteMinRestoreEndpoint());
            }
        }
    }
    engine.Teardown();
    return 0;
}
bool StartAnimation(HWND hWnd, BOOL rising, LONG_PTR originalExStyle, BOOL cloakHidden = FALSE,
                    BOOL isClosing = FALSE, UINT closeMsg = 0, HANDLE hWaitFinish = NULL,
                    BOOL deferredMinimize = FALSE, BOOL requestedUnhide = FALSE, HWND hNextApp = NULL,
                    int deferredShowCmd = 0, int effectStyle = -1,
                    BOOL taskbarFocusBorrowed = FALSE,
                    NativeMinimizeBarrier* nativeMinimizeBarrier = nullptr,
                    uint64_t asyncRestoreReservation = 0,
                    BOOL* suppressNativeMinimizeOnFailure = nullptr,
                    BOOL restoreMaximizedHint = FALSE,
                    ULONG_PTR suppliedSnapshotToken = 0) {
    NativeMinimizeBarrierOwner nativeBarrierOwner(nativeMinimizeBarrier);
    if (suppliedSnapshotToken &&
        !IsLaunchAnimationCurrent(hWnd, suppliedSnapshotToken)) {
        return false;
    }
    if (suppressNativeMinimizeOnFailure) *suppressNativeMinimizeOnFailure = FALSE;
    const BOOL restoreMaximized =
        !isClosing &&
        (restoreMaximizedHint || WindowRestoresMaximized(hWnd));
    
    static std::atomic<int> s_animCount{0};
    if (s_animCount.fetch_add(1, std::memory_order_relaxed) % 10 == 0) {
        SweepStaleData();
    }
    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) {
        FailAnimationStart(hWnd, rising, originalExStyle, cloakHidden);
        return false;
    }
    RECT rect = winRect, extRect{};
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &extRect, sizeof(extRect))) &&
        PhysicalRectToAnimationSpace(hWnd, &extRect)) {
        rect = extRect;
    }
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int offsetX = rect.left - winRect.left;
    int offsetY = rect.top - winRect.top;
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;
    if (w <= 0 || h <= 0 || rawW <= 0 || rawH <= 0) {
        FailAnimationStart(hWnd, rising, originalExStyle, cloakHidden);
        return false;
    }
    bool blocked = g_unloading.load(std::memory_order_relaxed);
    if (!blocked) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (asyncRestoreReservation) {
            auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
            auto directionIt = g_AnimWantRising.find(hWnd);
            blocked = reservationIt == g_AsyncRestoreReservations.end() ||
                      reservationIt->second != asyncRestoreReservation ||
                      directionIt == g_AnimWantRising.end() || !g_AnimActive.count(hWnd);
            if (!blocked) {
                rising = directionIt->second;
                if (!rising) {
                    deferredMinimize = TRUE;
                    deferredShowCmd = SW_MINIMIZE;
                }
                g_LaunchSeen.insert(hWnd);
            }
        } else {
            blocked = !g_AnimActive.insert(hWnd).second;
        }
        if (!blocked && !asyncRestoreReservation) {
            g_LaunchSeen.insert(hWnd);
            if (isClosing) {
                g_AnimWantRising.erase(hWnd);
            } else {
                g_AnimWantRising[hWnd] = rising != FALSE;
            }
            g_AnimRestoreRequestForeground.erase(hWnd);
            g_AsyncRestoreReservations.erase(hWnd);
        }
    }
    if (blocked) {
        Wh_Log(L"Animation skipped (busy/unloading) hwnd=%p rising=%d closing=%d", hWnd, rising,
               isClosing);
        if (asyncRestoreReservation) return false;
        if (!isClosing && !g_unloading.load(std::memory_order_relaxed) &&
            RetargetLiveMinRestore(hWnd, rising != FALSE) == MinRestoreRetarget::Accepted) {
            if (suppressNativeMinimizeOnFailure) *suppressNativeMinimizeOnFailure = TRUE;
            return false;
        }
        FailAnimationStart(hWnd, rising, originalExStyle, cloakHidden, /*skipDwmIfOwned=*/true);
        return false;
    }
    LONG_PTR storedExStyle = originalExStyle;
    if (!isClosing) {
        const LONG_PTR currentExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        storedExStyle = (originalExStyle & ~WS_EX_TOPMOST) |
                        (currentExStyle & WS_EX_TOPMOST);
        if (currentExStyle & WS_EX_TOPMOST) {
            // Let the animation ghost cover the real window temporarily. Keep
            // the bit in storedExStyle so endpoint/failure cleanup promotes it
            // back into the topmost band.
            RestoreZOrderAfterGhost(hWnd, 0);
        }
    }
    // A long guard is needed only while an accepted minimize/restore session
    // can receive a duplicate taskbar restore in another injected process.
    // Launch animations don't need it and must not suppress later taskbar use.
    if (restoreMaximized && (!rising || cloakHidden)) {
        ArmMaximizedRestoreGuard(
            hWnd, static_cast<DWORD>(AnimConstants::MaximizedRestoreGuardMs));
    }
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0; mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN); mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        mi.rcWork = mi.rcMonitor;
    }
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    DWORD alignVal = 1, dataSize = sizeof(alignVal);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
    HWND hTrayForDpi = FindTaskbarForMonitor(hMon);
    HWND hCoordinateWindow = GetAnimationCoordinateWindow(hWnd);
    const int leftAlignedOffset = MulDiv(
        160,
        static_cast<int>(GetWindowDpiCompat(
            hCoordinateWindow ? hCoordinateWindow : hTrayForDpi)),
        96);
    int learnedTargetX = (alignVal == 0) ? (mi.rcMonitor.left + leftAlignedOffset)
                                         : (mi.rcMonitor.left + monWidth / 2);
    const bool randomMinRestorePair =
        !isClosing && g_minRestoreRandomEffect.load(std::memory_order_relaxed);
    ULONG_PTR pairedEffectToken = 0;
    bool publishPairToken = false;
    if (randomMinRestorePair && (rising || asyncRestoreReservation)) {
        int pairedEffectStyle = -1;
        pairedEffectToken = ReadMinRestorePairToken(hWnd, &pairedEffectStyle);
        if (pairedEffectToken) effectStyle = pairedEffectStyle;
    }
    if (effectStyle < 0) {
        effectStyle = isClosing ? ResolveCloseEffectStyle()
                                : ResolveMinRestoreEffectStyle();
    }
    if (randomMinRestorePair && !pairedEffectToken) {
        pairedEffectToken = CreateMinRestorePairToken(effectStyle);
        publishPairToken = true;
    } else if (!isClosing && !randomMinRestorePair) {
        ClearMinRestorePair(hWnd);
    }
    if (!isClosing && (effectStyle == 0 || effectStyle == 7)) {
        WCHAR windowTitle[256] = {0};
        GetWindowTextW(hWnd, windowTitle, 256);
        learnedTargetX = GetTaskbarButtonX_Async(hWnd, windowTitle, learnedTargetX, hMon);
    }
    int durationMs = isClosing ? g_closeDurationMs.load(std::memory_order_relaxed)
                               : g_durationMs.load(std::memory_order_relaxed);
    {
        if (!isClosing && effectStyle == 7) {
            durationMs = AnimConstants::Win10MinRestoreMs; // fixed, ignores duration setting
        } else if (!isClosing && effectStyle == 1) {
            durationMs = Clamp((durationMs * 6) / 5, 260, 1400); // Ink Splash
        } else if (!isClosing && effectStyle == 2) {
            durationMs = Clamp((durationMs * 3) / 4, 200, 1100); // Scorch
        } else if (!isClosing && effectStyle == 3) {
            durationMs = Clamp((durationMs * 7) / 6, 300, 1200); // Splinter
        } else if (!isClosing && effectStyle == 4) {
            durationMs = Clamp((durationMs * 4) / 5, 220, 1100); // Mirage
        } else if (!isClosing && effectStyle == 5) {
            durationMs = Clamp((durationMs * 4) / 5, 220, 1100); // Stipple
        } else if (!isClosing && effectStyle == 6) {
            durationMs = Clamp((durationMs * 17) / 20, 200, 1200); // Swell
        }
    }
    {
        if (isClosing && effectStyle == 3) {
            durationMs = Clamp((durationMs * 19) / 20, 320, 1300);
        } else if (isClosing && effectStyle == 4) {
            durationMs = Clamp((durationMs * 55) / 100, 280, 900);
        }
    }
    auto* data = new (std::nothrow) WindowAnimData{
        hWnd, nullptr, nullptr, rect, hMon, w, h, learnedTargetX, rising, storedExStyle, cloakHidden, nullptr,
        durationMs,
        isClosing, closeMsg, hWaitFinish,
        requestedUnhide, hNextApp,
        g_unhideDurationMs.load(std::memory_order_relaxed),
        deferredMinimize,
        deferredShowCmd,
        effectStyle,
        restoreMaximized,
        pairedEffectToken,
        suppliedSnapshotToken,
        taskbarFocusBorrowed,
        FALSE,
        FALSE,
        nativeBarrierOwner.release()
    };
    const DWORD targetThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    const bool onTargetThread =
        targetThreadId && targetThreadId == GetCurrentThreadId();
    bool pairTokenPublished = false;
    const bool initiatingHookWillSubmitMinimize =
        !rising && suppressNativeMinimizeOnFailure != nullptr;
    auto finalizeClaimAfterStartFailure = [&]() {
        if (suppliedSnapshotToken &&
            !IsLaunchAnimationCurrent(hWnd, suppliedSnapshotToken)) {
            return;
        }
        if (asyncRestoreReservation) return;
        if (isClosing) {
            FailAnimationStart(hWnd, rising, storedExStyle, cloakHidden);
            std::lock_guard<std::mutex> lock(g_StateMutex);
            g_AnimActive.erase(hWnd);
            return;
        }

        for (;;) {
            BOOL finalRising = rising;
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                auto directionIt = g_AnimWantRising.find(hWnd);
                if (directionIt != g_AnimWantRising.end()) {
                    finalRising = directionIt->second;
                }
            }

            if (!finalRising && !initiatingHookWillSubmitMinimize &&
                IsWindow(hWnd)) {
                const int fallbackMinimizeCmd =
                    deferredShowCmd == SW_SHOWMINNOACTIVE ||
                            deferredShowCmd == SW_SHOWMINIMIZED ||
                            deferredShowCmd == SW_MINIMIZE
                        ? deferredShowCmd
                        : SW_MINIMIZE;
                if (onTargetThread) {
                    ShowWindow_Original(hWnd, fallbackMinimizeCmd);
                } else {
                    ShowWindowAsync_Original(hWnd, fallbackMinimizeCmd);
                }
            } else if (finalRising &&
                       (IsIconic(hWnd) ||
                        (restoreMaximized && !IsZoomed(hWnd)))) {
                if (onTargetThread) {
                    RestoreWindowUnderGhost(hWnd, storedExStyle,
                                            restoreMaximized);
                } else {
                    ShowWindowAsync_Original(
                        hWnd, StableRestoreShowCmd(restoreMaximized));
                    RestoreZOrderAfterGhostAsync(hWnd, storedExStyle);
                }
            }
            FailAnimationStart(hWnd, finalRising, storedExStyle, cloakHidden);

            bool directionChanged = false;
            bool clearPairedEffect = false;
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                auto directionIt = g_AnimWantRising.find(hWnd);
                directionChanged = directionIt != g_AnimWantRising.end() &&
                                   directionIt->second != (finalRising != FALSE);
            if (!directionChanged) {
                    clearPairedEffect =
                        pairedEffectToken && (finalRising || pairTokenPublished);
                    if (finalRising) EraseSnapshotLocked(hWnd);
                    g_AnimActive.erase(hWnd);
                    g_AnimWantRising.erase(hWnd);
                    g_AnimRestoreRequestForeground.erase(hWnd);
                    g_AsyncRestoreReservations.erase(hWnd);
                    if (suppliedSnapshotToken) g_LaunchSeen.erase(hWnd);
                }
            }
            if (directionChanged) continue;
            if (clearPairedEffect) {
                ClearMinRestorePairIfCurrent(hWnd, pairedEffectToken);
            }
            if (finalRising && restoreMaximized) {
                ArmMaximizedRestoreGuard(
                    hWnd,
                    static_cast<DWORD>(GetDoubleClickTime() + 100));
            }
            if (initiatingHookWillSubmitMinimize && finalRising &&
                suppressNativeMinimizeOnFailure) {
                // The live restore request cancels the not-yet-submitted
                // minimize owned by the initiating hook.
                *suppressNativeMinimizeOnFailure = TRUE;
            }
            if (suppliedSnapshotToken) {
                ClearLaunchAnimationIfCurrent(hWnd, suppliedSnapshotToken);
            }
            return;
        }
    };
    if (!data) {
        Wh_Log(L"Animation state allocation failed hwnd=%p", hWnd);
        finalizeClaimAfterStartFailure();
        return false;
    }
    HDC hScreenDC = GetDC(NULL);
    data->hBitmap = CreateDib32(hScreenDC, w, h, &data->pBits);
    if (!data->hBitmap || !data->pBits) {
        Wh_Log(L"Snapshot DIB alloc failed hwnd=%p %dx%d", hWnd, w, h);
        ReleaseDC(NULL, hScreenDC);
        finalizeClaimAfterStartFailure();
        ReleaseNativeMinimizeBarrier(data->nativeMinimizeBarrier);
        data->nativeMinimizeBarrier = nullptr;
        if (data->hBitmap) DeleteObject(data->hBitmap);
        delete data;
        return false;
    }
    const bool preferScreenCapture = ShouldUseBitBlt(hWnd, isClosing);
    auto CopySnapshot = [&](void* sourceBits, int sourceWidth,
                            int sourceHeight, bool makeOpaque) -> bool {
        auto* src = (DWORD*)sourceBits;
        auto* dst = (DWORD*)data->pBits;
        memset(dst, 0, (size_t)w * h * 4);
        const int startY = std::max(0, -offsetY);
        const int endY = std::min(h, sourceHeight - offsetY);
        const int startX = std::max(0, -offsetX);
        const int endX = std::min(w, sourceWidth - offsetX);
        if (startY >= endY || startX >= endX) return false;
        const int rowPixels = endX - startX;
        const size_t rowBytes = (size_t)rowPixels * 4;
        size_t zeroAlpha = 0;
        for (int y = startY; y < endY; ++y) {
            DWORD* dstRow = dst + (size_t)y * w + startX;
            memcpy(dstRow,
                   src + (size_t)(y + offsetY) * sourceWidth + startX + offsetX,
                   rowBytes);
            if (makeOpaque) {
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
                const DWORD* srcRow =
                    src + (size_t)(y + offsetY) * sourceWidth + startX + offsetX;
                for (int x = 0; x < rowPixels; ++x) {
                    if (dstRow[x] == 0) dstRow[x] = 0xFF000000u | (srcRow[x] & 0x00FFFFFFu);
                }
            }
        }
        return true;
    };
    auto CaptureNow = [&](bool allowScreenFallback) -> bool {
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
        bool capturedFromScreen = preferScreenCapture;
        BOOL captured = FALSE;
        if (capturedFromScreen) {
            captured = BitBlt(tempDC, 0, 0, rawW, rawH, hScreenDC,
                              winRect.left, winRect.top, SRCCOPY);
        } else if (onTargetThread) {
            // PrintWindow is synchronous. It is safe only when this hook is
            // already running on the target window's UI thread.
            captured = PrintWindow(hWnd, tempDC, PW_RENDERFULLCONTENT);
        }
        if (!captured && allowScreenFallback && IsWindowVisible(hWnd) &&
            !IsIconic(hWnd)) {
            capturedFromScreen = true;
            captured = BitBlt(tempDC, 0, 0, rawW, rawH, hScreenDC,
                              winRect.left, winRect.top, SRCCOPY);
            if (captured) {
                GdiFlush();
                HWND hTray = FindTaskbarForMonitor(hMon);
                RECT trayRect{};
                RECT overlap{};
                if (hTray && GetWindowRect(hTray, &trayRect) &&
                    IntersectRect(&overlap, &winRect, &trayRect)) {
                    const int left = Clamp(
                        static_cast<int>(overlap.left - winRect.left), 0, rawW);
                    const int right = Clamp(
                        static_cast<int>(overlap.right - winRect.left), 0, rawW);
                    const int top = Clamp(
                        static_cast<int>(overlap.top - winRect.top), 0, rawH);
                    const int bottom = Clamp(
                        static_cast<int>(overlap.bottom - winRect.top), 0, rawH);
                    auto* pixels = static_cast<DWORD*>(tempBits);
                    const bool horizontal =
                        (trayRect.right - trayRect.left) >=
                        (trayRect.bottom - trayRect.top);
                    const bool trailingEdge = horizontal
                                                  ? trayRect.top >=
                                                        (mi.rcMonitor.top +
                                                         mi.rcMonitor.bottom) / 2
                                                  : trayRect.left >=
                                                        (mi.rcMonitor.left +
                                                         mi.rcMonitor.right) / 2;
                    if (left < right && top < bottom) {
                        if (horizontal) {
                            const int sourceY = trailingEdge
                                                    ? std::max(0, top - 1)
                                                    : std::min(rawH - 1, bottom);
                            for (int y = top; y < bottom; ++y) {
                                memcpy(pixels + (size_t)y * rawW + left,
                                       pixels + (size_t)sourceY * rawW + left,
                                       (size_t)(right - left) * sizeof(DWORD));
                            }
                        } else {
                            const int sourceX = trailingEdge
                                                    ? std::max(0, left - 1)
                                                    : std::min(rawW - 1, right);
                            for (int y = top; y < bottom; ++y) {
                                DWORD* row = pixels + (size_t)y * rawW;
                                std::fill(row + left, row + right, row[sourceX]);
                            }
                        }
                    }
                }
            }
        }
        if (captured) {
            GdiFlush();
            captured = CopySnapshot(tempBits, rawW, rawH,
                                    capturedFromScreen);
        }
        SelectObject(tempDC, oldBmp);
        DeleteObject(tempBmp);
        DeleteDC(tempDC);
        return captured != FALSE;
    };
    bool snapshotReady = false;
    if (suppliedSnapshotToken) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto snapshotIt = g_WndSnapshots.find(hWnd);
        if (snapshotIt != g_WndSnapshots.end() &&
            snapshotIt->second.windowToken == suppliedSnapshotToken &&
            snapshotIt->second.pBits &&
            reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropSnapshotCache)) == suppliedSnapshotToken &&
            IsLaunchAnimationCurrent(hWnd, suppliedSnapshotToken)) {
            // suppliedSnapshotToken is used by launch animations. The owner
            // hook captured a raw GetWindowRect-sized PrintWindow bitmap after
            // the native show; normalize/crop it into extended-frame space.
            snapshotReady = CopySnapshot(
                snapshotIt->second.pBits, snapshotIt->second.w,
                snapshotIt->second.h, /*makeOpaque=*/false);
        }
        EraseSnapshotIfCurrentLocked(hWnd, suppliedSnapshotToken);
    }
    if (!snapshotReady && rising) {
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            snapshotReady = ConsumeSnapshotLocked(hWnd, data->pBits, w, h);
        }
        // A cross-thread restore source is already cloaked. Screen capture
        // would record the background, and PrintWindow could wedge Explorer.
        if (!snapshotReady) {
            snapshotReady = CaptureNow(/*allowScreenFallback=*/false);
        }
    } else if (!snapshotReady) {
        if (!isClosing) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            EraseSnapshotLocked(hWnd);
        }
        snapshotReady = CaptureNow(/*allowScreenFallback=*/true);
        if (snapshotReady && !isClosing &&
            g_restoreAnimation.load(std::memory_order_relaxed)) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            {
                void* pCacheBits = nullptr;
                HBITMAP hCacheBmp = CreateDib32(hScreenDC, w, h, &pCacheBits);
                if (hCacheBmp && pCacheBits) {
                    memcpy(pCacheBits, data->pBits, (size_t)w * h * 4);
                    if (!StoreSnapshotLocked(hWnd, hCacheBmp, pCacheBits, w, h)) {
                        DeleteObject(hCacheBmp);
                    }
                } else if (hCacheBmp) DeleteObject(hCacheBmp);
            }
        }
    }
    ReleaseDC(NULL, hScreenDC);
    if (!snapshotReady) {
        Wh_Log(L"Animation capture failed hwnd=%p rising=%d crossThread=%d",
               hWnd, rising, !onTargetThread);
        finalizeClaimAfterStartFailure();
        ReleaseNativeMinimizeBarrier(data->nativeMinimizeBarrier);
        data->nativeMinimizeBarrier = nullptr;
        DeleteObject(data->hBitmap);
        delete data;
        return false;
    }
    HANDLE hFirstShown = NULL;
    if (!rising) {
        hFirstShown = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (hFirstShown && !DuplicateHandle(GetCurrentProcess(), hFirstShown, GetCurrentProcess(), &data->hFirstFrameShown, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hFirstFrameShown = NULL;
        }
    }
    bool waitForFirstFrame = (data->hFirstFrameShown != NULL);
    const int startedDurationMs = data->durationMs;
    bool workerStarted = false;
    if (!isClosing) {
        if (randomMinRestorePair) {
            if (publishPairToken) {
                pairTokenPublished =
                    PublishMinRestorePairToken(hWnd, data->pairedEffectToken);
                if (!pairTokenPublished) data->pairedEffectToken = 0;
            }
        }
    }
    if (asyncRestoreReservation) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto reservationIt = g_AsyncRestoreReservations.find(hWnd);
        auto directionIt = g_AnimWantRising.find(hWnd);
        const bool reservationStillOwned =
            reservationIt != g_AsyncRestoreReservations.end() &&
            reservationIt->second == asyncRestoreReservation &&
            directionIt != g_AnimWantRising.end() && g_AnimActive.count(hWnd) &&
            IsWindow(hWnd);
        if (reservationStillOwned) {
            workerStarted = StartWorkerThread(MainAnimThread, data);
            if (workerStarted) {
                g_AsyncRestoreReservations.erase(reservationIt);
            }
        }
    } else {
        workerStarted = StartWorkerThread(MainAnimThread, data);
    }
    if (workerStarted) {
        Wh_Log(L"Animation start hwnd=%p kind=%s style=%d dockX=%d duration=%d", hWnd,
               isClosing ? L"close" : (rising ? L"restore" : L"minimize"), effectStyle, learnedTargetX,
               startedDurationMs);
        if (hFirstShown) {
            const DWORD firstFrameWaitMs =
                (!rising && !isClosing && effectStyle >= 1 && effectStyle <= 6) ? 500 : 200;
            if (waitForFirstFrame) WaitForSingleObject(hFirstShown, firstFrameWaitMs);
            CloseHandle(hFirstShown);
        }
        if (isClosing) SetWindowCloak(hWnd, TRUE);
        return true;
    }
    Wh_Log(L"Animation worker failed hwnd=%p kind=%s", hWnd,
           isClosing ? L"close" : (rising ? L"restore" : L"minimize"));
    finalizeClaimAfterStartFailure();
    if (hFirstShown) CloseHandle(hFirstShown);
    if (data->hFirstFrameShown) CloseHandle(data->hFirstFrameShown);
    ReleaseNativeMinimizeBarrier(data->nativeMinimizeBarrier);
    data->nativeMinimizeBarrier = nullptr;
    DeleteObject(data->hBitmap);
    delete data;
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
    const bool waitForFinish = (closeMsg != ANIM_DEFER_SW_HIDE);
    HANDLE wait = NULL;
    HANDLE workerWait = NULL;
    if (waitForFinish) {
        wait = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (wait && !DuplicateHandle(GetCurrentProcess(), wait, GetCurrentProcess(), &workerWait, 0,
                                     FALSE, DUPLICATE_SAME_ACCESS)) {
            CloseHandle(wait);
            wait = nullptr;
        }
    }

    const bool started =
        StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE, TRUE, closeMsg,
                       workerWait);
    if (!started) Wh_Log(L"Close animation did not start hwnd=%p msg=0x%04X", hWnd, closeMsg);

    if (started) {
        if (wait) WaitForCloseAnimation(wait);
    } else if (workerWait) {
        CloseHandle(workerWait);
    }

    if (wait) CloseHandle(wait);
    return started;
}
enum class MinimizeKick { None, Immediate, Deferred };
static MinimizeKick KickMinimizeAnimation(HWND hWnd, bool desktopFocusOnUnhide = false,
                                          bool allowUnhide = true, int showCmd = 0,
                                          NativeMinimizeBarrier* nativeMinimizeBarrier = nullptr) {
    NativeMinimizeBarrierOwner nativeBarrierOwner(nativeMinimizeBarrier);
    const MinRestoreRetarget retarget = RetargetLiveMinRestore(hWnd, false);
    if (retarget == MinRestoreRetarget::Accepted) {
        return MinimizeKick::Deferred;
    }
    if (retarget == MinRestoreRetarget::BusyOther) return MinimizeKick::None;
    if (!g_minimizeAnimation.load(std::memory_order_relaxed)) return MinimizeKick::None;
    if (IsHungAppWindow(hWnd)) return MinimizeKick::None;
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return MinimizeKick::None;
    if (!ShouldAnimateWindow(hWnd)) return MinimizeKick::None;
    BOOL requestedUnhide = FALSE;
    HWND hTray = NULL;
    HWND hNext = NULL;
    BOOL taskbarFocusBorrowed = FALSE;
    HMONITOR hMon = NULL;
    const int effectStyle = ResolveMinRestoreEffectStyle();
    const bool genieStyle = effectStyle == 0;
    if (allowUnhide && genieStyle && g_unhideEnabled.load(std::memory_order_relaxed)) {
        hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        hTray = FindTaskbarForMonitor(hMon);
        if (hTray) {
            APPBARDATA abd = { sizeof(APPBARDATA) };
            abd.hWnd = hTray;
            const UINT uState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
            if ((uState & ABS_AUTOHIDE) &&
                !IsTaskbarExpanded(hWnd, hTray, hMon) &&
                !IsCursorOverTaskbar(hTray)) {
                requestedUnhide = TRUE;
                if (!desktopFocusOnUnhide) hNext = FindNextAppWindow(hWnd, hTray);
            }
        }
    }
    UpdateDwmTransitions(hWnd, FALSE);
    if (requestedUnhide) {
        if (hTray) {
            taskbarFocusBorrowed = SetForegroundWindow(hTray);
            WaitForTaskbarExpanded(hWnd, hTray, hMon);
            if (!IsTaskbarExpanded(hWnd, hTray, hMon)) requestedUnhide = FALSE;
        }
        BOOL revealRequestSettled = FALSE;
        if (requestedUnhide &&
            StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE, FALSE, 0, NULL,
                           TRUE, TRUE, hNext, showCmd, effectStyle, taskbarFocusBorrowed, nullptr, 0,
                           &revealRequestSettled)) {
            return MinimizeKick::Deferred;
        }
        if (taskbarFocusBorrowed) {
            RestoreForegroundAfterTaskbarReveal(hTray, hWnd, hNext, false);
        }
        if (revealRequestSettled) return MinimizeKick::Deferred;
    }
    BOOL suppressNativeMinimize = FALSE;
    const bool started = StartAnimation(
        hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE, FALSE, 0, NULL, FALSE,
        FALSE, NULL, showCmd, effectStyle, FALSE, nativeBarrierOwner.release(), 0,
        &suppressNativeMinimize);
    if (started) return MinimizeKick::Immediate;
    return suppressNativeMinimize ? MinimizeKick::Deferred : MinimizeKick::None;
}
static MinimizeKick TryMinimizeAnim(HWND hWnd, NativeMinimizeBarrier* nativeMinimizeBarrier = nullptr,
                                   int showCmd = 0) {
    return KickMinimizeAnimation(hWnd, /*desktopFocusOnUnhide=*/true, /*allowUnhide=*/false,
                                 showCmd, nativeMinimizeBarrier);
}
static bool CanPrepareRestoreAnimation(HWND hWnd) {
    if (!g_restoreAnimation.load(std::memory_order_relaxed) || IsHungAppWindow(hWnd) ||
        !IsWindowVisible(hWnd) || !ShouldAnimateWindow(hWnd)) {
        return false;
    }
    return true;
}
static bool PrepareRestoreAnimation(HWND hWnd) {
    if (!CanPrepareRestoreAnimation(hWnd)) return false;
    UpdateDwmTransitions(hWnd, FALSE);
    SetWindowCloak(hWnd, TRUE);
    return true;
}
static void CommitRestoreAnimation(HWND hWnd, BOOL restoreMaximizedHint) {
    const bool started =
        StartAnimation(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE,
                       FALSE, 0, NULL, FALSE, FALSE, NULL, 0, -1, FALSE,
                       nullptr, 0, nullptr, restoreMaximizedHint);
    if (!started && restoreMaximizedHint && !IsAnimating(hWnd)) {
        // The native restore was already issued, but no long-lived animation
        // session exists to receive duplicates. Keep only the short debounce.
        ArmMaximizedRestoreGuard(
            hWnd, static_cast<DWORD>(GetDoubleClickTime() + 100));
    }
}
DWORD WINAPI AsyncRestoreAnimThread(LPVOID lpParam) {
    auto* restoreData = (AsyncRestoreAnimData*)lpParam;
    const HWND hWnd = restoreData->hWnd;
    const LONG_PTR originalExStyle = restoreData->originalExStyle;
    const uint64_t reservationGeneration = restoreData->reservationGeneration;
    const BOOL restoreMaximized = restoreData->restoreMaximized;
    delete restoreData;

    const DWORD deadline = GetTickCount() + 1000;
    while (!g_unloading.load(std::memory_order_relaxed) && IsWindow(hWnd) && IsIconic(hWnd) &&
           (LONG)(GetTickCount() - deadline) < 0) {
        Sleep(10);
    }
    if (!g_unloading.load(std::memory_order_relaxed) && IsWindow(hWnd) &&
        IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
        bool wantRising = true;
        if (GetAsyncRestoreReservation(hWnd, reservationGeneration, &wantRising) &&
            StartAnimation(hWnd, wantRising, originalExStyle, TRUE, FALSE, 0, NULL, FALSE, FALSE,
                           NULL, 0, -1, FALSE, nullptr, reservationGeneration,
                           nullptr, restoreMaximized)) {
            return 0;
        }
    }
    FinalizeAsyncRestoreReservation(hWnd, reservationGeneration, originalExStyle,
                                    /*initialRestoreSubmitted=*/true,
                                    restoreMaximized);
    return 0;
}
static bool PrepareLaunchAnim(HWND hWnd, int nCmdShow, LONG_PTR* origExOut,
                              ULONG_PTR* snapshotTokenOut) {
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    if (!g_launchAnimation.load(std::memory_order_relaxed)) return false;
    if (!IsLaunchCommand(nCmdShow)) return false;
    if (GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId()) {
        // Launch capture must happen before the real window is made fully
        // transparent. Never synchronously ask another UI thread to paint it.
        return false;
    }
    if (IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    if (!IsLaunchWindow(hWnd)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) return false;
    { std::lock_guard<std::mutex> lock(g_StateMutex); if (!g_LaunchSeen.insert(hWnd).second) return false; }
    UpdateDwmTransitions(hWnd, FALSE);
    *origExOut = exStyle;
    if (snapshotTokenOut) *snapshotTokenOut = 0;
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    return true;
}
static bool CaptureLaunchSnapshot(HWND hWnd, ULONG_PTR* snapshotTokenOut) {
    if (!hWnd || !snapshotTokenOut ||
        GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId() ||
        !IsWindowVisible(hWnd)) {
        return false;
    }
    RECT windowRect{};
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = hScreenDC ? CreateCompatibleDC(hScreenDC) : nullptr;
    HBITMAP hBitmap = nullptr;
    HBITMAP hOldBitmap = nullptr;
    void* bits = nullptr;
    int width = 0;
    int height = 0;
    ULONG_PTR snapshotToken = 0;
    if (hScreenDC && hMemoryDC && GetWindowRect(hWnd, &windowRect)) {
        width = windowRect.right - windowRect.left;
        height = windowRect.bottom - windowRect.top;
        if (width > 0 && height > 0) {
            hBitmap = CreateDib32(hScreenDC, width, height, &bits);
            if (hBitmap && bits) {
                hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));
                if (!hOldBitmap || hOldBitmap == HGDI_ERROR ||
                    !PrintWindow(hWnd, hMemoryDC, PW_RENDERFULLCONTENT)) {
                    if (hOldBitmap && hOldBitmap != HGDI_ERROR) {
                        SelectObject(hMemoryDC, hOldBitmap);
                    }
                    hOldBitmap = nullptr;
                    DeleteObject(hBitmap);
                    hBitmap = nullptr;
                    bits = nullptr;
                } else {
                    GdiFlush();
                }
            }
        }
    }
    if (hOldBitmap && hOldBitmap != HGDI_ERROR) {
        SelectObject(hMemoryDC, hOldBitmap);
    }
    if (hMemoryDC) DeleteDC(hMemoryDC);
    if (hScreenDC) ReleaseDC(nullptr, hScreenDC);
    if (!hBitmap || !bits) return false;
    bool storedSnapshot = false;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        storedSnapshot = StoreSnapshotLocked(
            hWnd, hBitmap, bits, width, height, &snapshotToken);
    }
    if (!storedSnapshot) {
        DeleteObject(hBitmap);
        return false;
    }
    if (!SetPropW(hWnd, kPropLaunchAnimation,
                  reinterpret_cast<HANDLE>(snapshotToken))) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        EraseSnapshotIfCurrentLocked(hWnd, snapshotToken);
        return false;
    }
    *snapshotTokenOut = snapshotToken;
    return true;
}
static void CommitLaunchAnim(HWND hWnd, LONG_PTR originalExStyle,
                             ULONG_PTR snapshotToken) {
    if (!snapshotToken && !CaptureLaunchSnapshot(hWnd, &snapshotToken)) {
        RestoreLayeredOpacity(hWnd, originalExStyle);
        UpdateDwmTransitions(hWnd, TRUE);
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_LaunchSeen.erase(hWnd);
        return;
    }
    auto* ld = new (std::nothrow) LaunchAnimData{
        hWnd, originalExStyle, snapshotToken};
    if (!ld || !StartWorkerThread(LaunchAnimThread, ld)) {
        delete ld;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            EraseSnapshotIfCurrentLocked(hWnd, snapshotToken);
        }
        ClearLaunchAnimationIfCurrent(hWnd, snapshotToken);
        RestoreLayeredOpacity(hWnd, originalExStyle);
        UpdateDwmTransitions(hWnd, TRUE);
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_LaunchSeen.erase(hWnd);
    }
}
static bool IsOurWindow(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}
static bool IsTaskbarRestoreSysCommand(HWND hWnd, LPARAM lParam) {
    if (lParam == static_cast<LPARAM>(MAKELPARAM(0, 1)) ||
        lParam == static_cast<LPARAM>(-1)) {
        return false;
    }
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    HWND hTray = FindTaskbarForMonitor(hMon);
    if (!hTray) return false;
    if (lParam == 0) return IsCursorOverTaskbar(hTray);
    POINT pt{static_cast<short>(LOWORD(lParam)),
             static_cast<short>(HIWORD(lParam))};
    RECT trayRect{};
    return GetWindowRect(hTray, &trayRect) && PtInRect(&trayRect, pt);
}
static bool IsShellTaskbarRestoreCall(HWND hWnd) {
    if (!IsShellExplorerProcess() || IsOurWindow(hWnd)) return false;
    HWND hTray = FindTaskbarForMonitor(
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
    return hTray && IsCursorOverTaskbar(hTray);
}
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int cmd) {
    if (IsMinimizeCommand(cmd)) {
        const BOOL wasVisible = IsWindowVisible(hWnd);
        NativeMinimizeBarrier* barrier = CreateNativeMinimizeBarrier();
        if (!barrier) {
            if (RetargetLiveMinRestore(hWnd, false) == MinRestoreRetarget::Accepted) {
                return wasVisible;
            }
            return ShowWindow_Original(hWnd, cmd);
        }
        AddRefNativeMinimizeBarrier(barrier);
        const MinimizeKick kick = KickMinimizeAnimation(
            hWnd, /*desktopFocusOnUnhide=*/true, /*allowUnhide=*/true, cmd, barrier);
        if (kick == MinimizeKick::Deferred) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return wasVisible;
        }
        if (!BeginNativeMinimizeSubmission(barrier)) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return wasVisible;
        }
        const BOOL result = ShowWindow_Original(hWnd, cmd);
        CompleteNativeMinimizeBarrier(
            barrier, wasVisible ? NativeMinimizeState::SyncCompleted
                                : NativeMinimizeState::Failed);
        return result;
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) && IsIconic(hWnd)) {
        if (RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
        const BOOL restoreMaximized =
            cmd == SW_RESTORE && WindowRestoresMaximized(hWnd);
        if (PrepareRestoreAnimation(hWnd)) {
            if (restoreMaximized) {
                ArmMaximizedRestoreGuard(
                    hWnd,
                    static_cast<DWORD>(AnimConstants::MaximizedRestoreGuardMs));
            }
            BOOL result = ShowWindow_Original(hWnd, cmd);
            CommitRestoreAnimation(hWnd, restoreMaximized);
            return result;
        }
        return ShowWindow_Original(hWnd, cmd);
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) &&
        RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
    if (cmd == SW_RESTORE && IsShellTaskbarRestoreCall(hWnd) &&
        ShouldSuppressRedundantMaximizedRestore(hWnd)) {
        // This hook can run in a different injected process while the restore
        // worker owns and cloaks the real window. Keep suppression side-effect
        // free; the owning worker activates only after it safely uncloaks.
        return IsWindowVisible(hWnd);
    }
    if (!IsOurWindow(hWnd)) return ShowWindow_Original(hWnd, cmd);
    if (IsShowCmdForWinEvent(cmd)) EnsureWinEventThreadStarted();
    if (cmd == SW_HIDE) {
        if (GetPropW(hWnd, kPropCloseBypass)) return ShowWindow_Original(hWnd, cmd);
        if (ShouldTreatHideAsClose(hWnd)) {
            BOOL wasVisible = IsWindowVisible(hWnd);
            if (RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return wasVisible;
        }
    }
    LONG_PTR originalStyle;
    ULONG_PTR launchSnapshotToken = 0;
    if (PrepareLaunchAnim(hWnd, cmd, &originalStyle,
                          &launchSnapshotToken)) {
        BOOL result = ShowWindow_Original(hWnd, cmd);
        CommitLaunchAnim(hWnd, originalStyle, launchSnapshotToken);
        return result;
    }
    return ShowWindow_Original(hWnd, cmd);
}
BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int cmd) {
    if (IsMinimizeCommand(cmd)) {
        NativeMinimizeBarrier* barrier = CreateNativeMinimizeBarrier();
        if (!barrier) {
            if (RetargetLiveMinRestore(hWnd, false) == MinRestoreRetarget::Accepted) {
                return TRUE;
            }
            return ShowWindowAsync_Original(hWnd, cmd);
        }
        AddRefNativeMinimizeBarrier(barrier);
        if (TryMinimizeAnim(hWnd, barrier, cmd) == MinimizeKick::Deferred) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return TRUE;
        }
        if (!BeginNativeMinimizeSubmission(barrier)) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return TRUE;
        }
        const BOOL result = ShowWindowAsync_Original(hWnd, cmd);
        CompleteNativeMinimizeBarrier(
            barrier, result ? NativeMinimizeState::AsyncSubmitted
                            : NativeMinimizeState::Failed);
        return result;
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) &&
        RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
    if (cmd == SW_RESTORE && IsShellTaskbarRestoreCall(hWnd) &&
        ShouldSuppressRedundantMaximizedRestore(hWnd)) {
        // See ShowWindow_Hook: foregrounding here can re-enter Explorer's
        // taskbar toggle logic and enqueue an unowned late minimize.
        return TRUE;
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) && IsIconic(hWnd) &&
        CanPrepareRestoreAnimation(hWnd)) {
        const LONG_PTR originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        const BOOL restoreMaximized =
            cmd == SW_RESTORE && WindowRestoresMaximized(hWnd);
        uint64_t reservationGeneration = 0;
        if (!ReserveAsyncRestore(hWnd, &reservationGeneration)) {
            if (RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
            return ShowWindowAsync_Original(hWnd, cmd);
        }
        if (restoreMaximized) {
            ArmMaximizedRestoreGuard(
                hWnd,
                static_cast<DWORD>(AnimConstants::MaximizedRestoreGuardMs));
        }
        UpdateDwmTransitions(hWnd, FALSE);
        SetWindowCloak(hWnd, TRUE);
        const BOOL result = ShowWindowAsync_Original(hWnd, cmd);
        auto* restoreData = result
                                ? new (std::nothrow) AsyncRestoreAnimData{
                                      hWnd, originalExStyle, reservationGeneration,
                                      restoreMaximized}
                                : nullptr;
        if (!restoreData || !StartWorkerThread(AsyncRestoreAnimThread, restoreData)) {
            delete restoreData;
            AbortAsyncRestoreReservation(hWnd, reservationGeneration, originalExStyle,
                                          result != FALSE, restoreMaximized);
        }
        return result;
    }
    if (!IsOurWindow(hWnd)) return ShowWindowAsync_Original(hWnd, cmd);
    if (IsShowCmdForWinEvent(cmd)) EnsureWinEventThreadStarted();
    if (cmd == SW_HIDE) {
        if (GetPropW(hWnd, kPropCloseBypass)) return ShowWindowAsync_Original(hWnd, cmd);
        if (ShouldTreatHideAsClose(hWnd) && RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return TRUE;
    }
    LONG_PTR originalStyle;
    ULONG_PTR launchSnapshotToken = 0;
    if (PrepareLaunchAnim(hWnd, cmd, &originalStyle,
                          &launchSnapshotToken)) {
        BOOL result = ShowWindowAsync_Original(hWnd, cmd);
        CommitLaunchAnim(hWnd, originalStyle, launchSnapshotToken);
        return result;
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
        ULONG_PTR launchSnapshotToken = 0;
        if (PrepareLaunchAnim(hWnd, SW_SHOW, &originalStyle,
                              &launchSnapshotToken)) {
            BOOL result = SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
            CommitLaunchAnim(hWnd, originalStyle, launchSnapshotToken);
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
            NativeMinimizeBarrier* barrier = CreateNativeMinimizeBarrier();
            if (!barrier) {
                if (RetargetLiveMinRestore(hWnd, false) == MinRestoreRetarget::Accepted) {
                    return 0;
                }
                return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
            }
            AddRefNativeMinimizeBarrier(barrier);
            if (KickMinimizeAnimation(hWnd, false, true, 0, barrier) ==
                MinimizeKick::Deferred) {
                CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
                return 0;
            }
            if (!BeginNativeMinimizeSubmission(barrier)) {
                CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
                return 0;
            }
            const LRESULT result = DefWindowProcW_Original(hWnd, msg, wParam, lParam);
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::SyncCompleted);
            return result;
        }
        if (cmd == SC_RESTORE) {
            if (RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return 0;
            if (IsTaskbarRestoreSysCommand(hWnd, lParam) &&
                ShouldSuppressRedundantMaximizedRestore(hWnd)) {
                // The animation owner performs endpoint activation. This
                // cross-process debounce must not mutate foreground state.
                return 0;
            }
            const BOOL restoreMaximized = WindowRestoresMaximized(hWnd);
            if (IsIconic(hWnd) && PrepareRestoreAnimation(hWnd)) {
                if (restoreMaximized) {
                    ArmMaximizedRestoreGuard(
                        hWnd,
                        static_cast<DWORD>(AnimConstants::MaximizedRestoreGuardMs));
                }
                LRESULT result = DefWindowProcW_Original(hWnd, msg, wParam, lParam);
                CommitRestoreAnimation(hWnd, restoreMaximized);
                return result;
            }
        }
    }
    return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
}
BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd, const WINDOWPLACEMENT* placement) {
    if (placement && IsMinimizeCommand(placement->showCmd)) {
        NativeMinimizeBarrier* barrier = CreateNativeMinimizeBarrier();
        if (!barrier) {
            if (RetargetLiveMinRestore(hWnd, false) == MinRestoreRetarget::Accepted) {
                return TRUE;
            }
            return SetWindowPlacement_Original(hWnd, placement);
        }
        AddRefNativeMinimizeBarrier(barrier);
        if (TryMinimizeAnim(hWnd, barrier, placement->showCmd) == MinimizeKick::Deferred) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return TRUE;
        }
        if (!BeginNativeMinimizeSubmission(barrier)) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return TRUE;
        }
        const BOOL result = SetWindowPlacement_Original(hWnd, placement);
        CompleteNativeMinimizeBarrier(
            barrier, result ? NativeMinimizeState::SyncCompleted
                            : NativeMinimizeState::Failed);
        return result;
    }
    if (placement && (placement->showCmd == SW_RESTORE || placement->showCmd == SW_SHOWNORMAL) &&
        RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
    if (placement && placement->showCmd == SW_RESTORE &&
        IsShellTaskbarRestoreCall(hWnd) &&
        ShouldSuppressRedundantMaximizedRestore(hWnd)) {
        // Keep the cross-process redundant-restore guard side-effect free.
        return TRUE;
    }
    if (placement && (placement->showCmd == SW_RESTORE || placement->showCmd == SW_SHOWNORMAL) &&
        IsIconic(hWnd) && PrepareRestoreAnimation(hWnd)) {
        const BOOL restoreMaximized =
            placement->showCmd == SW_RESTORE && WindowRestoresMaximized(hWnd);
        if (restoreMaximized) {
            ArmMaximizedRestoreGuard(
                hWnd,
                static_cast<DWORD>(AnimConstants::MaximizedRestoreGuardMs));
        }
        const LONG_PTR originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        BOOL result = SetWindowPlacement_Original(hWnd, placement);
        if (result) CommitRestoreAnimation(hWnd, restoreMaximized);
        else if (!IsAnimating(hWnd)) {
            UndoRisingHide(hWnd, originalExStyle, TRUE);
        }
        return result;
    }
    if (!IsOurWindow(hWnd)) return SetWindowPlacement_Original(hWnd, placement);
    if (placement && placement->showCmd == SW_HIDE && !GetPropW(hWnd, kPropCloseBypass) &&
        ShouldTreatHideAsClose(hWnd)) {
        WINDOWPLACEMENT modified = *placement;
        modified.showCmd = SW_SHOWNA;
        BOOL applied = SetWindowPlacement_Original(hWnd, &modified);
        if (!applied) return FALSE;
        if (RunCloseAnimation(hWnd, ANIM_DEFER_SW_HIDE)) return TRUE;
        return ShowWindow_Original(hWnd, SW_HIDE);
    }
    return SetWindowPlacement_Original(hWnd, placement);
}
BOOL WINAPI CloseWindow_Hook(HWND hWnd) {
    NativeMinimizeBarrier* barrier = CreateNativeMinimizeBarrier();
    if (!barrier) {
        if (RetargetLiveMinRestore(hWnd, false) == MinRestoreRetarget::Accepted) {
            return TRUE;
        }
        return CloseWindow_Original(hWnd);
    }
    AddRefNativeMinimizeBarrier(barrier);
    if (TryMinimizeAnim(hWnd, barrier) == MinimizeKick::Deferred) {
        CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
        return TRUE;
    }
    if (!BeginNativeMinimizeSubmission(barrier)) {
        CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
        return TRUE;
    }
    const BOOL result = CloseWindow_Original(hWnd);
    CompleteNativeMinimizeBarrier(
        barrier, result ? NativeMinimizeState::SyncCompleted
                        : NativeMinimizeState::Failed);
    return result;
}
DWORD WINAPI LaunchAnimThread(LPVOID lpParam) {
    LaunchAnimData* ld = (LaunchAnimData*)lpParam;
    HWND hWnd = ld->hWnd;
    LONG_PTR originalExStyle = ld->originalExStyle;
    ULONG_PTR snapshotToken = ld->snapshotToken;
    delete ld;
    if (!IsLaunchAnimationCurrent(hWnd, snapshotToken)) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        EraseSnapshotIfCurrentLocked(hWnd, snapshotToken);
        return 0;
    }
    Sleep(60);
    for (int i = 0; i < 30; ++i) {
        if (!IsWindow(hWnd) ||
            !IsLaunchAnimationCurrent(hWnd, snapshotToken) ||
            g_unloading.load(std::memory_order_relaxed)) break;
        UINT cloaked = 0; if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) || !cloaked) break; Sleep(50);
    }
    if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(hWnd) ||
        !IsLaunchAnimationCurrent(hWnd, snapshotToken) ||
        IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        const bool identityCurrent =
            IsLaunchAnimationCurrent(hWnd, snapshotToken);
        if (identityCurrent) {
            RestoreLayeredOpacity(hWnd, originalExStyle);
            UpdateDwmTransitions(hWnd, TRUE);
        }
        std::lock_guard<std::mutex> lock(g_StateMutex);
        EraseSnapshotIfCurrentLocked(hWnd, snapshotToken);
        if (identityCurrent) {
            g_LaunchSeen.erase(hWnd);
            ClearLaunchAnimationIfCurrent(hWnd, snapshotToken);
        }
        return 0;
    }
    if (!StartAnimation(hWnd, TRUE, originalExStyle, FALSE, FALSE, 0, nullptr,
                        FALSE, FALSE, nullptr, 0, -1, FALSE, nullptr, 0,
                        nullptr, FALSE, snapshotToken)) {
        const bool identityCurrent =
            IsLaunchAnimationCurrent(hWnd, snapshotToken);
        if (identityCurrent) {
            RestoreLayeredOpacity(hWnd, originalExStyle);
            UpdateDwmTransitions(hWnd, TRUE);
        }
        std::lock_guard<std::mutex> lock(g_StateMutex);
        EraseSnapshotIfCurrentLocked(hWnd, snapshotToken);
        if (identityCurrent) {
            g_LaunchSeen.erase(hWnd);
            ClearLaunchAnimationIfCurrent(hWnd, snapshotToken);
        }
    }
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
static void EnsureExplorerForegroundThreadStarted() {
    if (g_unloading.load(std::memory_order_relaxed) ||
        !g_switchAnimation.load(std::memory_order_relaxed) ||
        !IsShellExplorerProcess()) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_ExplorerFgThreadMutex);
    if (g_unloading.load(std::memory_order_relaxed) ||
        !g_switchAnimation.load(std::memory_order_relaxed) ||
        !IsShellExplorerProcess()) {
        return;
    }
    if (g_hExplorerFgThread &&
        WaitForSingleObject(g_hExplorerFgThread, 0) == WAIT_OBJECT_0) {
        CloseHandle(g_hExplorerFgThread);
        g_hExplorerFgThread = NULL;
    }
    if (g_hExplorerFgThread) return;

    g_explorerAltTabTrackerEnabled.store(true, std::memory_order_relaxed);
    HANDLE hThread = CreateThread(NULL, 0, ExplorerFgHookThread, NULL, 0, NULL);
    if (!hThread) {
        g_explorerAltTabTrackerEnabled.store(false, std::memory_order_relaxed);
        Wh_Log(L"Explorer FOREGROUND thread failed to start");
        return;
    }
    g_hExplorerFgThread = hThread;
}

static void StopExplorerForegroundThread() {
    g_explorerAltTabTrackerEnabled.store(false, std::memory_order_relaxed);
    HANDLE hThread = NULL;
    {
        std::lock_guard<std::mutex> lock(g_ExplorerFgThreadMutex);
        hThread = g_hExplorerFgThread;
        g_hExplorerFgThread = NULL;
    }
    if (!hThread) return;
    DWORD tid = GetThreadId(hThread);
    while (tid && !PostThreadMessageW(tid, WM_QUIT, 0, 0)) {
        if (WaitForSingleObject(hThread, 10) != WAIT_TIMEOUT) break;
    }
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}

void StartSwitchThreads() {
    EnsureExplorerForegroundThreadStarted();
    if (!g_unloading.load(std::memory_order_relaxed) &&
        g_switchAnimation.load(std::memory_order_relaxed)) {
        EnumWindows(EnumWindowsInitProc, 0);
    }
}
static void StopWinEventThread() {
    HANDLE hWinEvent = NULL;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        hWinEvent = g_hWinEventThread;
        g_hWinEventThread = NULL;
    }
    if (!hWinEvent) return;
    DWORD tid = GetThreadId(hWinEvent);
    while (tid && !PostThreadMessageW(tid, WM_QUIT, 0, 0)) {
        if (WaitForSingleObject(hWinEvent, 10) != WAIT_TIMEOUT) break;
    }
    WaitForSingleObject(hWinEvent, INFINITE);
    CloseHandle(hWinEvent);
    g_winEventThreadStarted.store(false, std::memory_order_relaxed);
}
void StopSwitchThreads() {
    g_explorerAltTabTrackerEnabled.store(false, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_AltTabSessionMutex);
        g_altTabSessionPollRunning.store(false, std::memory_order_relaxed);
        if (g_hAltTabSessionThread) {
            WaitForSingleObject(g_hAltTabSessionThread, INFINITE);
            CloseHandle(g_hAltTabSessionThread);
            g_hAltTabSessionThread = NULL;
        }
    }
    StopExplorerForegroundThread();
    const bool keepTaskbarObserver =
        !g_unloading.load(std::memory_order_relaxed) && IsShellExplorerProcess() &&
        (g_minimizeAnimation.load(std::memory_order_relaxed) ||
         g_restoreAnimation.load(std::memory_order_relaxed));
    if (!keepTaskbarObserver) {
        StopWinEventThread();
    }
}
static DWORD WINAPI ShellOwnershipProbeThread(LPVOID) {
    const DWORD started = GetTickCount();
    while (!g_unloading.load(std::memory_order_relaxed)) {
        if (IsShellExplorerProcess()) {
            StopWinEventThread();
            EnsureWinEventThreadStarted();
            if (g_switchAnimation.load(std::memory_order_relaxed)) {
                StartSwitchThreads();
            }
            return 0;
        }
        const DWORD elapsed = GetTickCount() - started;
        Sleep(elapsed < 5000 ? 50 : 500);
    }
    return 0;
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
    const bool explorerProcess = IsShellExplorerProcess();
    WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(ShowWindowAsync, ShowWindowAsync_Hook, &ShowWindowAsync_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPlacement, SetWindowPlacement_Hook, &SetWindowPlacement_Original);
    WindhawkUtils::SetFunctionHook(CloseWindow, CloseWindow_Hook, &CloseWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(DestroyWindow, DestroyWindow_Hook, &DestroyWindow_Original);
    if (g_switchAnimation.load(std::memory_order_relaxed)) {
        StartSwitchThreads();
    }
    if (explorerProcess &&
        (g_minimizeAnimation.load(std::memory_order_relaxed) ||
         g_restoreAnimation.load(std::memory_order_relaxed))) {
        EnsureWinEventThreadStarted();
    }
    if (!explorerProcess && IsExplorerProcess()) {
        StartWorkerThread(ShellOwnershipProbeThread, nullptr);
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
    if (IsShellExplorerProcess() &&
        (g_minimizeAnimation.load(std::memory_order_relaxed) ||
         g_restoreAnimation.load(std::memory_order_relaxed))) {
        EnsureWinEventThreadStarted();
    } else if (!isSwitchAnim) {
        StopWinEventThread();
    }
}
void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit: joining workers");
    g_unloading.store(true, std::memory_order_relaxed);
    // The ownership probe is a registered worker and is the only worker that
    // can request the separately-owned Explorer foreground thread. Join it
    // before detaching that thread, in addition to the start-side lock/check.
    JoinWorkerThreads();
    StopSwitchThreads();
}
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    std::vector<HWND> stuck;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        stuck.assign(g_AnimActive.begin(), g_AnimActive.end());
        g_AnimActive.clear();
        g_AnimWantRising.clear();
        g_AnimRestoreRequestForeground.clear();
        g_AsyncRestoreReservations.clear();
    }
    for (HWND hWnd : stuck) {
        if (!IsWindow(hWnd)) continue;
        SetWindowCloak(hWnd, FALSE);
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        UpdateDwmTransitions(hWnd, TRUE);
        RemovePropW(hWnd, kPropCloseBypass);
        RemovePropW(hWnd, kPropClosed);
    }
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        while (!g_WndSnapshots.empty()) {
            EraseSnapshotLocked(g_WndSnapshots.begin());
        }
        g_WndSnapshotBytes = 0;
        g_TaskbarDockXs.clear(); g_ProcessDockXs.clear();
        g_TaskbarDockLookupGenerations.clear();
        g_TaskbarDockLookupStartedTicks.clear();
        g_TaskbarDockNegativeUntilTicks.clear();
        g_TaskbarDockPositiveUntilTicks.clear();
        g_ProcessNameCache.clear(); g_LaunchSeen.clear();
        g_TaskbarWindowSetSignature = 0;
        g_TaskbarWindowSetSignatureInitialized = false;
        g_seenTaskbarLayoutEpoch = 0;
        g_seenTaskbarExplorerPid = 0;
    }
    CloseSharedState();
}
