// ==WindhawkMod==
// @id              windows-animations
// @name            Windows Animations
// @description     Smooth minimize, restore, close, switch animations for windows.
// @version         1.3.13
// @author          ReDrag
// @github          https://github.com/redrag2105
// @include         *
// @exclude         TextInputHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         dwm.exe
// @exclude         git.exe
// @exclude         git-remote-https.exe
// @exclude         AudioCaptureService.exe
// @exclude         AIVisualPIPESvr.exe
// @exclude         GimateServiceHelper.exe
// @exclude         NVIDIA Web Helper.exe
// @license         MIT
// @compilerOptions -ladvapi32 -ld3d11 -ldcomp -ldwmapi -ldxgi -lgdi32 -lole32 -loleaut32 -lshell32 -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Animations
> ⚠️ **NOTE:** 
> * **Packaged & System Apps:** Many packaged apps can be animated when they expose a normal top-level window, but protected or system-hosted windows such as some Settings, Store, and shell surfaces can bypass interception and use their native transition instead.
> * **Browsers & Tray Apps:** Some browsers and tray apps, including Chrome, Edge, Discord, and Windhawk, can keep running or hide their window when you click the 'X' button instead of exiting. To animate those hidden transitions, turn on **"Animate windows hidden to the tray"** in the settings.

Welcome to **Windows Animations**, a comprehensive window transition suite for your desktop. Built from the ground up to deliver cinematic window animations. 

By utilizing a smart Hybrid Rendering Engine, this mod bridges the gap between stunning visual aesthetics and seamless execution.

&nbsp;

---

&nbsp;

## 🆕 What's new in 1.3.13

My original goal (v1.3.0 -- an unreleased full-GPU prototype) was simple: implement D3D11 versions of every animation and move all rendering to the GPU. An unreleased development build did exactly that for minimize, restore, launch, and all close effects. I then tested it instead of assuming that "GPU" always means "faster."

### The full-GPU experiment

Matched tests compared the released 1.2.0 CPU renderer, the newly optimized CPU renderer developed for this update, and the unreleased forced-GPU prototype. Lower frame cost is better. The value in parentheses is speedup versus 1.2.0, so above 1.0x is faster and below 1.0x is slower. Values are geometric means of each effect's median animation-thread frame cost; each column retains 10 measured animations per effect after two warm-up runs.

| Workload | 1.2.0 CPU | New optimized CPU (vs 1.2.0) | Forced-GPU prototype (vs 1.2.0) | Frame coverage 1.2/CPU/GPU |
|---|---:|---:|---:|---:|
| Taskbar minimize (8 effects) | 1.29 ms | 1.30 ms (0.99x) | 3.71 ms (0.35x) | 100%/100%/100% |
| Taskbar restore (8 effects) | 1.35 ms | 1.39 ms (0.97x) | 0.45 ms (3.02x) | 100%/100%/100% |
| Normal close, 5 px (6 effects) | 1.97 ms | 1.95 ms (1.01x) | 4.78 ms (0.41x) | 100%/100%/100% |
| Full-screen dense close, 1 px (3 effects) | 16.28 ms | 9.97 ms (1.63x) | 9.19 ms (1.77x) | 29%/60%/57% |

The experiment revealed three practical results:

* **GPU restores performed extremely well:** A DirectComposition swap chain avoids the expensive layered-window synchronization path, reducing restore frame cost from 1.39 ms on the new CPU path to 0.45 ms on GPU. Compatible warmed launches reuse this restore path, while a cold first launch falls back to CPU.
* **Minimize and ordinary close became slower:** These transitions must remain synchronized with Windows' native window/taskbar state. Their stable GPU path renders with D3D11, synchronizes the GPU surface with GDI, and presents it through `UpdateLayeredWindow`, adding a fixed cost. That made forced-GPU minimize about 2.9x slower than the new CPU path and ordinary close about 2.5x slower.
* **Very dense close effects were the exception:** At 1 px on a large window, the parallel GPU work can outweigh that fixed cost. Per-effect results showed a useful win for dense Thanos and Perlin, while Square Shatter still favored CPU.

> I also prototyped DirectComposition for minimize to remove the layered-window cost. However, Windows changes the real window's native taskbar state during minimize, and that internal transition intermittently exposed a flicker even after the composition visual was detached, DWM commits were drained, and the real window had both cloak and opacity protection. Rapid reversal tests confirmed that rendering itself was fast; the unreliable part was the final native minimize handoff. Because visual correctness matters more than a benchmark number, that prototype is not included in this release.

### Why 1.3.13 uses a hybrid renderer

The shipped design keeps the best measured path for each workload instead of forcing one renderer everywhere:

When the corresponding **Hybrid GPU acceleration** option is enabled, the mod uses the conservative routing below. Disabling an option forces its entire animation group to CPU.

| Animation | Renderer selected | Reason |
|---|---|---|
| Normal minimize | CPU layered window | Faster in every matched minimize test and avoids DirectComposition/native-taskbar flicker. |
| Normal restore | D3D11 + DirectComposition | Consistently much faster because it avoids layered-window GPU readback and synchronization. The latency-sensitive Optimize Show Desktop batch path remains CPU. |
| Animated launch | D3D11 + DirectComposition when warmed and compatible; otherwise CPU | Uses the restore animation path. The first launch in a process can use CPU while GPU resources warm safely in the background. |
| Close: Thanos or Perlin, 1 px blocks, at least 1.5 million source pixels | D3D11 + layered window | This is the measured dense workload where GPU parallelism can outweigh the layered presentation cost. |
| Every other close workload | CPU layered window | Faster for ordinary windows, larger blocks, Square Shatter, Cyber Glitch, Retro TV, and Pixel Melt in the matched tests. |
| Alt+Tab switch | Native DWM thumbnail | Independent of both GPU settings. |

> Automatic CPU fallback still applies when the GPU is cold, unavailable, remotely accessed, unsafe for the target process/window, or running the latency-sensitive Optimize Show Desktop batch path. The close threshold is based on source-window size and therefore adapts to different resolutions and DPI-scaled window dimensions.

[Full benchmark methodology, per-effect results, and interpretation](https://github.com/redrag2105/windhawk-windows-animations-preview/blob/windows-animations-v1.3.0/windows-animations-1.3.0-benchmarks.md)

### Everything else added since 1.2.0

| Area | Improvement |
|---|---|
| **CPU effects** | Perlin uses a compact dissolve field with cached noise generation. Shatter and Thanos use compact particle data and stop traversing particles after they can no longer contribute to the frame. Specialized 1 px paths reduce the cost of very fine particles. |
| **GPU preparation** | Predictive background warm-up compiles only the next configured or shuffled eligible effect and reuses shared pipelines. |
| **Resource safety** | Right-sized surfaces reduce memory use, and process-wide GPU resources are released before the final app window closes. |
| **Animation pacing** | Frame scheduling follows the active monitor's refresh timing instead of assuming a fixed refresh rate. |
| **Window transitions** | Improved taskbar minimizes, support for taskbars on every screen edge, rounded-corner preservation, rapid reversals, Alt+Tab, backdrop windows, console capture, and Windows Terminal taskbar targeting. |
| **Show Desktop** | Optional **Optimize Show Desktop** custom-animates only the foreground window, minimizes background windows immediately, and keeps their restore behavior native. |
| **Shuffle mode** | Each animation group uses one session-wide cycle shared by every app, so every style plays once before reshuffling without process restarts causing repeats. Launch/restore and the following minimize remain paired. |
| **Compatibility** | Automatic CPU fallback plus early skipping of non-interactive sessions and known background-only helper processes. |

&nbsp;

---

&nbsp;

## ✨ Key Features

* **🚀 Smart Hybrid Engine:** Selects the optimized CPU renderer for minimizes and ordinary closes, D3D11 with DirectComposition for restores/launches, and D3D11 with layered presentation for qualifying dense Thanos/Perlin closes. Alt+Tab uses native DWM thumbnails. After an eligible app window settles, the mod predicts the next configured or shuffled GPU-eligible effect and warms only its required pipelines. GPU startup, device loss, Remote Desktop, or unsupported hardware automatically falls back to CPU.

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

* **Minimize/Restore Animation Style:** Choose between Genie, Windows 10 (collapse to taskbar button), Ink Splash, Scorch, Splinter, Mirage, Stipple, and Swell. Enable **Shuffle animation styles** to play every style once per session-wide shuffled cycle shared by all apps; each restore or animated launch shares its style with the following minimize (ignores the dropdown).
* **Close Animation Effect:** Dropdown menu to switch between 'Square Shatter', 'Thanos Snap', 'Perlin Dissolve', 'Cyber Glitch', 'Retro TV Off', and 'Pixel Melt'. Enable **Shuffle animation styles** to play every close effect once in a session-wide cycle shared by all apps before reshuffling (ignores the dropdown).
* **Minimize/Restore duration (ms):** Controls Genie speed directly; each style applies a small pace tweak. Windows 10 style uses a fixed system-like timing and ignores this setting. (Default: 360ms, clamp 200–1400)
* **Close animation duration (ms):** Controls how long the dramatic close animation lasts. (Default: 630ms)
* **Switch animation duration (ms):** Controls the snappy speed of the Alt+Tab scaling effect. (Default: 200ms)
* **Shatter block size (px):** Determines the size of the dust/shatter particles.
  * *Performance Tip:* The 1 px setting uses specialized single-pixel paths, but maximized windows can still be demanding when an app must fall back to CPU rendering. Around 5 px is a better balance for those windows. Values from 2–4 create dense quad fields, while larger values (24, 32) yield a stylish retro pixelated effect and perform effortlessly on most hardware. (Clamped strictly to 1-100).
* **Animate app launches:** Off by default. Enable it to use the restore effect when an application window first opens.
* **Animate windows hidden to the tray:** Off by default. Enable it to animate apps such as Discord, Steam, and Telegram when they hide their window instead of closing it. This can also animate splash screens or windows hidden automatically by an app.
* **Toggles:** Individually turn on/off Minimize, Restore, Close, Alt+Tab Switch, and Launch animations to suit your workflow.
* **Taskbar placement:** Bottom, top, left, and right taskbars are supported, including taskbars on secondary monitors and auto-hidden taskbars. Genie bends toward the detected edge and targets the app button on that axis; Windows 10 scales toward the full button position. The other minimize/restore effects animate in place, while **None** continues to use Windows' native transition.
* **Rounded corners:** Minimize, restore, and launch effects preserve Windows 11's rounded window silhouette. Maximized windows and apps that explicitly request square corners stay square. This does not add a synthetic window shadow.
* **Hybrid GPU acceleration:** Separate toggles allow the mod to use GPU rendering where it is measurably beneficial. The minimize/restore toggle accelerates restores and launches; normal minimizes intentionally remain on CPU. The close toggle accelerates only sufficiently large 1 px Thanos/Perlin workloads; ordinary close effects remain on CPU. Turn a toggle off to force that whole group to CPU. See **Why 1.3.13 uses a hybrid renderer** above for the complete routing rules and fallbacks.
* **Performance telemetry:** An optional diagnostic setting writes one compact timing and resource summary after each animation, including the selected D3D adapter, display refresh, average pacing wait, and CPU canvas-clear/effect/presentation breakdown. It also enables supporting animation-start, routing, taskbar-lookup, reversal, settings, and GPU-lifecycle events. It is disabled by default and never logs individual frames.
* **Optional Show Desktop optimization:** By default, Win+D uses the classic behavior and custom-animates eligible windows in sequence. Enable **Optimize Show Desktop** to custom-animate only the foreground/top window while background windows minimize immediately without Windows' native minimize transition. The matching Win+D restore remains native for those background windows; opening one individually later uses the normal custom restore path again.
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
    $name: Shuffle animation styles
    $description: >-
      Use one session-wide shuffled cycle shared by every app. Play every effect once before
      starting a new cycle, and never join consecutive cycles on the same effect. Each restore or
      animated launch selects the next global effect, and the following minimize reuses it. A
      minimize without an earlier animated restore selects an effect for itself. Rapid reversals
      keep the same effect. When enabled, the Animation style setting below is ignored.
  - effect_style: genie
    $name: Animation style
    $description: >-
      Choose the effect used for minimizing and restoring windows. Ignored while shuffling is
      enabled.
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
  - show_desktop_top_window_only: false
    $name: Optimize Show Desktop
    $description: >-
      Disabled keeps the classic behavior and custom-animates eligible windows in sequence during
      Win+D. Enable this to custom-animate only the foreground/top window while all remaining
      windows minimize immediately without the native minimize transition. Their matching Win+D
      restore remains native, which keeps Show Desktop fast without changing Windows' restore
      ordering. Opening one of those windows individually later uses the normal custom restore
      path again.
  - gpu_acceleration: true
    $name: Hybrid GPU acceleration
    $description: >-
      Use D3D11 with DirectComposition for warmed, compatible restore and launch animations.
      Normal minimize animations intentionally use the optimized CPU renderer because it is
      faster and avoids native taskbar-state flicker. Rapid reversals keep the renderer already
      in use. Unsupported hardware, Remote Desktop, incompatible processes such as UniKey,
      initialization failure, or device loss falls back to CPU. Disable this setting to force
      CPU for the entire group. See the mod README for the full routing details.
  $name: Minimize, restore, and launch
  $description: Configure animations for minimizing, restoring, and opening windows.
- close:
  - close_animation: true
    $name: Animate closing
    $description: Play a cinematic effect when an application window closes.
  - hide_as_close: false
    $name: Animate windows hidden to the tray
    $description: >-
      Disabled by default. Enable to animate apps such as Discord, Steam, and Telegram when they
      hide their window instead of closing it. This can also animate splash screens, such as the
      Discord updater, or windows hidden automatically by an app, such as the Google Chrome
      profile picker.
  - random_effect: false
    $name: Shuffle animation styles
    $description: >-
      Use one session-wide shuffled cycle shared by every app. Play every close effect once before
      starting a new cycle, and never join consecutive cycles on the same effect. Closing and
      reopening an app continues the same global cycle. When enabled, the Animation style setting
      below is ignored.
  - effect_style: thanos
    $name: Animation style
    $description: >-
      Choose the effect used when closing a window. Ignored while shuffling is enabled.
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
      Close animation duration, from 50 to 5000 ms. The closing or hiding app's UI thread is
      blocked for this time (only sent messages are pumped).
  - shatter_block_size: 5
    $name: Dust and shatter block size (px)
    $description: >-
      Particle size, from 1 to 100 px. The 1 px setting uses specialized single-pixel paths on
      both renderers; values from 2 to 4 use dense quad fields and can be especially demanding
      on 4K displays.
  - gpu_acceleration: true
    $name: Hybrid GPU acceleration
    $description: >-
      Use D3D11 only for very dense Thanos or Perlin closes: 1 px blocks on windows with at least
      1.5 million source pixels. Other close effects and lighter workloads intentionally use the
      optimized CPU renderer because layered GPU synchronization costs more. File Explorer and
      other backdrop-sensitive windows, Remote Desktop, incompatible processes, unavailable
      hardware, initialization failure, or device loss also use CPU. Disable this setting to
      force CPU for every close. See the mod README for the full routing details.
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
- diagnostics:
  - performance_telemetry: false
    $name: Log performance telemetry
    $description: >-
      Disabled by default. Write one compact performance summary after each minimize, restore,
      launch, or close animation and enable supporting animation-start, renderer-routing,
      taskbar-lookup, reversal, settings, and GPU-lifecycle events. CPU samples split canvas
      clearing, effect rendering, and layered-window presentation. No per-frame messages are
      written. Enable this only while collecting measurements, then disable it to keep the log
      concise.
  $name: Diagnostics
  $description: Optional measurements used when diagnosing renderer performance.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <shellapi.h>
#include <wrl/client.h>
#include <math.h>
#include <cstdint>
#include <cstring>
#include <atomic>
#include <array>
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
#include <utility>
#include <windhawk_utils.h>

using Microsoft::WRL::ComPtr;
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
#define ANIM_PREPARE_SHOW_DESKTOP_RESTORE (WM_APP + 102)
using DefWindowProcW_t = LRESULT (WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using ShowWindow_t = BOOL (WINAPI*)(HWND, int);
using ShowWindowAsync_t = BOOL (WINAPI*)(HWND, int);
using SetWindowPlacement_t = BOOL (WINAPI*)(HWND, const WINDOWPLACEMENT*);
using CloseWindow_t = BOOL (WINAPI*)(HWND);
using SetWindowPos_t = BOOL (WINAPI*)(HWND, HWND, int, int, int, int, UINT);
using DestroyWindow_t = BOOL (WINAPI*)(HWND);
using RaiseDesktop_t = void (__cdecl*)(void*, int);
DefWindowProcW_t DefWindowProcW_Original;
ShowWindow_t ShowWindow_Original;
ShowWindowAsync_t ShowWindowAsync_Original;
SetWindowPlacement_t SetWindowPlacement_Original;
CloseWindow_t CloseWindow_Original;
SetWindowPos_t SetWindowPos_Original;
DestroyWindow_t DestroyWindow_Original;
RaiseDesktop_t RaiseDesktop_Original;

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
    int width{}, height{}, targetDockX{}, targetDockY{};
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
    BOOL fastShowDesktopStart{};
    ULONG_PTR showDesktopAnimationToken{};
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
    BOOL fastShowDesktopStart;
    ULONG_PTR showDesktopAnimationToken;
};
struct LocalShowDesktopCloakWatchData {
    HWND hWnd;
    ULONG_PTR token;
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
struct CloseShatterParticle {
    float dirX;
    float dirY;
    float force;
    float noiseX;
    float noiseY;
    uint32_t nextActive;
};
struct CloseThanosParticle {
    float windX;
    float windY;
    float curveX;
    float curveY;
    float startTime;
    uint32_t nextActive;
};
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
    constexpr int EndpointAsyncSettleMs = 50;
    constexpr int MaximizedRestoreGuardMs = 3000;
    constexpr int AltTabPollMs = 16;
    constexpr int AltTabSessionMs = 500;
    // A cold lookup follows a taskbar layout invalidation. Win11's XAML UIA
    // tree can take longer than 150 ms while reflowing; returning earlier is
    // what caused the first post-reflow animation to aim at a generic/stale
    // position.
    // Valid hot HWND cache hits return before this wait.
    constexpr int UiaLookupWaitMs = 400;
    constexpr int UiaLookupInFlightMs = 3000;
    constexpr int UiaNegativeCacheMs = 10000;
    constexpr int UiaColdNegativeCacheMs = 500;
    constexpr int UiaPositiveFallbackCacheMs = 10000;
    // UIPI can prevent Explorer from attaching the lifetime property to an
    // elevated HWND. PID/TID/monitor is a useful but weaker identity, so cache
    // it only briefly before requiring another UIA confirmation.
    constexpr int UiaWeakIdentityCacheMs = 1500;
    constexpr int UiaMinAcceptScore = 400;
    constexpr int TaskbarExpandWaitMs = 250;
    constexpr int GpuFrameLatencyWaitMs = 250;
    constexpr int ClassicShowDesktopWaitMs = 8000;
    constexpr int ShowDesktopNativeSettleMs = 250;
    constexpr int ShowDesktopSwitchGhostWaitMs = 1100;
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
static constexpr PCWSTR kPropTaskbarDockIdentity =
    L"windows-animations.TaskbarDockIdentityV1";
static constexpr PCWSTR kPropShowDesktopNativeMinimize =
    L"windows-animations.ShowDesktopNativeMinimizeV1";
static constexpr PCWSTR kPropShowDesktopInstantMinimize =
    L"windows-animations.ShowDesktopInstantMinimizeV1";
static constexpr PCWSTR kPropShowDesktopAnimationOwner =
    L"windows-animations.ShowDesktopAnimationOwnerV1";
static constexpr PCWSTR kPropShowDesktopAnimationOwnerUntil =
    L"windows-animations.ShowDesktopAnimationOwnerUntilV1";
static constexpr PCWSTR kPropShowDesktopRestorePrepared =
    L"windows-animations.ShowDesktopRestorePreparedV1";
static constexpr PCWSTR kPropShowDesktopOwnedSurfaceCloak =
    L"windows-animations.ShowDesktopOwnedSurfaceCloakV1";
static constexpr PCWSTR kPropShowDesktopLocalCloakWatch =
    L"windows-animations.ShowDesktopLocalCloakWatchV1";
static constexpr PCWSTR kPropShowDesktopCloakEndpoint =
    L"windows-animations.ShowDesktopCloakEndpointV1";
static constexpr PCWSTR kPropShowDesktopCloakReady =
    L"windows-animations.ShowDesktopCloakReadyV1";
static constexpr PCWSTR kPropSwitchAnimationGhost =
    L"windows-animations.SwitchAnimationGhostV1";
static constexpr PCWSTR kPropSwitchAnimationActive =
    L"windows-animations.SwitchAnimationActiveV1";
// The HWND properties bridge taskbar-side Explorer hooks and the target app's
// hooks. The pair token's low nibble stores style+1; the remaining bits are a
// nonce so an old worker can't clear a newer pair.
std::atomic<ULONG_PTR> g_NextMinRestorePairToken{0};
std::atomic<ULONG_PTR> g_NextSnapshotCacheToken{0};
std::atomic<ULONG_PTR> g_NextTaskbarDockIdentityToken{0};
std::atomic<ULONG_PTR> g_NextShowDesktopMarkerToken{0};
std::atomic<ULONG_PTR> g_NextShowDesktopAnimationToken{0};
struct TaskbarDockWindowIdentity {
    DWORD processId = 0;
    DWORD threadId = 0;
    HMONITOR monitor = nullptr;
    ULONG_PTR windowToken = 0;
};
struct ShowDesktopMarkerCleanup {
    ULONG_PTR token{};
    DWORD deadline{};
    bool waitingForMinimize{};
};
enum class TaskbarEdge : uint32_t {
    Bottom = 0,
    Top = 1,
    Left = 2,
    Right = 3,
};
struct TaskbarDockCacheValue {
    POINT position{};
    TaskbarEdge edge{TaskbarEdge::Bottom};
    TaskbarDockWindowIdentity identity;
    DWORD observedTick = 0;
};
struct ProcessNameCacheValue {
    DWORD processId = 0;
    DWORD threadId = 0;
    ULONG_PTR windowToken = 0;
    std::wstring name;
};
std::unordered_map<HWND, SnapCache> g_WndSnapshots;
size_t g_WndSnapshotBytes = 0;
uint64_t g_NextSnapshotCacheSerial = 0;
std::unordered_map<HWND, TaskbarDockCacheValue> g_TaskbarDockPositions;
// Last successfully observed button position for the same HWND. Layout
// invalidation keeps this only as a timeout fallback; it is never trusted as a
// current hot value.
std::unordered_map<HWND, TaskbarDockCacheValue>
    g_TaskbarDockFallbackPositions;
std::unordered_map<std::wstring, POINT> g_ProcessDockPositions;
std::unordered_map<HWND, uint64_t> g_TaskbarDockLookupGenerations;
std::unordered_map<HWND, TaskbarDockWindowIdentity>
    g_TaskbarDockLookupIdentities;
std::unordered_map<HWND, DWORD> g_TaskbarDockLookupStartedTicks;
std::unordered_map<HWND, DWORD> g_TaskbarDockNegativeUntilTicks;
std::unordered_map<HWND, DWORD> g_TaskbarDockPositiveUntilTicks;
uint64_t g_NextTaskbarDockLookupGeneration = 0;
uint64_t g_TaskbarWindowSetSignature = 0;
bool g_TaskbarWindowSetSignatureInitialized = false;
LONG g_seenTaskbarLayoutEpoch = 0;
DWORD g_seenTaskbarExplorerPid = 0;
std::unordered_map<HWND, ProcessNameCacheValue> g_ProcessNameCache;
std::unordered_set<HWND> g_LaunchSeen;
std::unordered_set<HWND> g_AnimActive;
std::unordered_map<HWND, bool> g_AnimWantRising;
std::unordered_map<HWND, HWND> g_AnimRestoreRequestForeground;
std::unordered_map<HWND, uint64_t> g_AsyncRestoreReservations;
std::unordered_map<HWND, ULONG_PTR> g_ShowDesktopNativeMarkers;
std::unordered_map<HWND, ShowDesktopMarkerCleanup>
    g_ShowDesktopMarkerCleanups;
bool g_ShowDesktopMarkerCleanupWorkerRunning = false;
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
std::atomic<DWORD> g_winEventThreadId{0};
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
    // Seqlock-protected Show Desktop intent. The shell is the only writer;
    // target-process hooks read it so posted SC_MINIMIZE/SC_RESTORE messages
    // follow the same one-window animation policy as Explorer-side calls.
    volatile LONG showDesktopSequence;
    volatile LONG showDesktopUntilTick;
    volatile LONG showDesktopTargetWindow;
    volatile LONG showDesktopLastAnimatedWindow;
};
static constexpr PCWSTR kSharedStateName = L"Local\\Windhawk_Anim_State_121";
static constexpr LONG kSharedStateMagic = 0x57415339;
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
std::atomic<bool> g_closeShuffleEffect{false};
std::atomic<bool> g_minRestoreShuffleEffect{false};
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
std::atomic<bool> g_showDesktopTopWindowOnly{false};
std::atomic<bool> g_gpuAcceleration{true};
std::atomic<bool> g_closeGpuAcceleration{true};
std::atomic<bool> g_performanceTelemetry{false};
std::atomic<DWORD> g_localShowDesktopUntilTick{0};
std::atomic<HWND> g_localShowDesktopTarget{nullptr};
std::atomic<HWND> g_localShowDesktopLastAnimated{nullptr};
std::atomic<DWORD> g_localShowDesktopOperationUntilTick{0};
std::atomic<LONG> g_animationGhostCount{0};
bool g_classicShowDesktopCaptureClaimed = false;
std::mutex g_WorkerThreadsMutex;
std::vector<HANDLE> g_WorkerThreads;
template <typename T> static T Clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

static bool IsDiagnosticLoggingEnabled() {
    return g_performanceTelemetry.load(std::memory_order_relaxed);
}

template <size_t EffectCount>
struct EffectShuffleBag {
    static_assert(EffectCount > 1);
    std::array<int, EffectCount> effects{};
    size_t cursor = EffectCount;
    int lastEffect = -1;
    unsigned randomState = 0;
};

// The visible shuffle order belongs to the desktop session, not to whichever
// application happens to run an animation. Keep a small process-local bag only
// as a compatibility fallback for targets which can't open the shared mapping.
// Fixed-width fields keep the mapping layout identical in 32-bit and 64-bit
// injected processes.
struct SharedEffectShuffleState {
    volatile LONG initializationState;
    volatile LONG magic;
    volatile LONG minRestoreNextOrdinal;
    volatile LONG closeNextOrdinal;
    volatile LONG minRestoreSeed;
    volatile LONG closeSeed;
};
static_assert(sizeof(SharedEffectShuffleState) == sizeof(LONG) * 6);

static constexpr PCWSTR kSharedEffectShuffleStateName =
    L"Local\\Windhawk_Anim_Shuffle_131_1";
static constexpr LONG kSharedEffectShuffleStateMagic = 0x57415351;
HANDLE g_hSharedEffectShuffleMap = NULL;
SharedEffectShuffleState* g_pSharedEffectShuffleState = nullptr;
std::mutex g_SharedEffectShuffleStateMutex;

static SharedEffectShuffleState* EnsureSharedEffectShuffleState();
static void CloseSharedEffectShuffleState();

std::mutex g_EffectShuffleMutex;
EffectShuffleBag<6> g_CloseEffectShuffle;
EffectShuffleBag<8> g_MinRestoreEffectShuffle;

static uint32_t MixGlobalShuffleValue(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7FEB352Du;
    value ^= value >> 15;
    value *= 0x846CA68Bu;
    value ^= value >> 16;
    return value ? value : 1u;
}

static uint32_t NextGlobalShuffleRandom(uint32_t& state) {
    uint32_t value = state;
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    state = value ? value : 1u;
    return state;
}

static size_t GlobalShuffleIndexBelow(uint32_t& randomState,
                                      size_t upperExclusive) {
    const uint32_t bound = static_cast<uint32_t>(upperExclusive);
    const uint32_t rejectionThreshold = (0u - bound) % bound;
    uint32_t randomValue = 0;
    do {
        randomValue = NextGlobalShuffleRandom(randomState);
    } while (randomValue < rejectionThreshold);
    return static_cast<size_t>(randomValue % bound);
}

template <size_t EffectCount>
static std::array<int, EffectCount> BuildGlobalEffectShuffleCycle(
    uint32_t seed, uint32_t cycle, uint32_t seedSalt) {
    std::array<int, EffectCount> effects{};
    for (size_t i = 0; i < EffectCount; ++i) {
        effects[i] = static_cast<int>(i);
    }

    uint32_t randomState = MixGlobalShuffleValue(
        seed ^ seedSalt ^ MixGlobalShuffleValue(cycle + 0x9E3779B9u));
    for (size_t remaining = EffectCount; remaining > 1; --remaining) {
        const size_t swapIndex =
            GlobalShuffleIndexBelow(randomState, remaining);
        std::swap(effects[remaining - 1], effects[swapIndex]);
    }
    return effects;
}

template <size_t EffectCount>
static int GlobalEffectAtOrdinal(uint32_t seed, uint32_t ordinal,
                                 uint32_t seedSalt) {
    static_assert(EffectCount > 2);
    const uint32_t cycle = ordinal / static_cast<uint32_t>(EffectCount);
    const size_t slot = ordinal % static_cast<uint32_t>(EffectCount);
    auto effects = BuildGlobalEffectShuffleCycle<EffectCount>(
        seed, cycle, seedSalt);

    if (cycle > 0) {
        // Boundary correction only swaps the first two entries, so the last
        // entry remains the raw permutation's last entry. That lets every
        // process derive the preceding cycle's final effect independently,
        // without recursive history or another shared mutable bag.
        const auto previousEffects =
            BuildGlobalEffectShuffleCycle<EffectCount>(
                seed, cycle - 1, seedSalt);
        if (effects[0] == previousEffects[EffectCount - 1]) {
            std::swap(effects[0], effects[1]);
        }
    }
    return effects[slot];
}

template <size_t EffectCount>
static int ConsumeGlobalEffectShuffle(volatile LONG* nextOrdinal,
                                      volatile LONG* seed,
                                      uint32_t seedSalt) {
    const uint32_t ordinal = static_cast<uint32_t>(
        InterlockedIncrement(nextOrdinal)) - 1u;
    const uint32_t sharedSeed = static_cast<uint32_t>(
        InterlockedCompareExchange(seed, 0, 0));
    return GlobalEffectAtOrdinal<EffectCount>(sharedSeed, ordinal,
                                               seedSalt);
}

template <size_t EffectCount>
static int PeekGlobalEffectShuffle(volatile LONG* nextOrdinal,
                                   volatile LONG* seed,
                                   uint32_t seedSalt) {
    const uint32_t ordinal = static_cast<uint32_t>(
        InterlockedCompareExchange(nextOrdinal, 0, 0));
    const uint32_t sharedSeed = static_cast<uint32_t>(
        InterlockedCompareExchange(seed, 0, 0));
    return GlobalEffectAtOrdinal<EffectCount>(sharedSeed, ordinal,
                                               seedSalt);
}

template <size_t EffectCount>
static unsigned NextShuffleRandomLocked(
    EffectShuffleBag<EffectCount>& bag, unsigned seedSalt) {
    unsigned x = bag.randomState;
    if (!x) {
        LARGE_INTEGER counter{};
        QueryPerformanceCounter(&counter);
        x = static_cast<unsigned>(counter.LowPart) ^
            static_cast<unsigned>(GetTickCount64()) ^
            (static_cast<unsigned>(GetCurrentProcessId()) * 0x9E3779B9u) ^
            seedSalt;
        if (!x) x = 0xA5A5A5A5u ^ seedSalt;
    }
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    if (!x) x = 1;
    bag.randomState = x;
    return x;
}

template <size_t EffectCount>
static size_t ShuffleIndexBelowLocked(EffectShuffleBag<EffectCount>& bag,
                                      size_t upperExclusive,
                                      unsigned seedSalt) {
    const unsigned bound = static_cast<unsigned>(upperExclusive);
    const unsigned rejectionThreshold = (0u - bound) % bound;
    unsigned randomValue = 0;
    do {
        randomValue = NextShuffleRandomLocked(bag, seedSalt);
    } while (randomValue < rejectionThreshold);
    return static_cast<size_t>(randomValue % bound);
}

template <size_t EffectCount>
static void RefillEffectShuffleLocked(EffectShuffleBag<EffectCount>& bag,
                                      unsigned seedSalt) {
    for (size_t i = 0; i < EffectCount; ++i) {
        bag.effects[i] = static_cast<int>(i);
    }
    for (size_t remaining = EffectCount; remaining > 1; --remaining) {
        const size_t swapIndex = ShuffleIndexBelowLocked(
            bag, remaining, seedSalt);
        std::swap(bag.effects[remaining - 1], bag.effects[swapIndex]);
    }
    if (bag.lastEffect >= 0 && bag.effects[0] == bag.lastEffect) {
        const size_t swapIndex = 1 + ShuffleIndexBelowLocked(
            bag, EffectCount - 1, seedSalt);
        std::swap(bag.effects[0], bag.effects[swapIndex]);
    }
    bag.cursor = 0;
}

template <size_t EffectCount>
static int ConsumeEffectShuffleLocked(EffectShuffleBag<EffectCount>& bag,
                                      unsigned seedSalt) {
    if (bag.cursor >= EffectCount) {
        RefillEffectShuffleLocked(bag, seedSalt);
    }
    const int effect = bag.effects[bag.cursor++];
    bag.lastEffect = effect;
    return effect;
}

template <size_t EffectCount>
static int PeekEffectShuffleLocked(EffectShuffleBag<EffectCount>& bag,
                                   unsigned seedSalt) {
    if (bag.cursor >= EffectCount) {
        RefillEffectShuffleLocked(bag, seedSalt);
    }
    return bag.effects[bag.cursor];
}

template <size_t EffectCount>
static void InvalidateEffectShuffleLocked(EffectShuffleBag<EffectCount>& bag) {
    // Preserve lastEffect so disabling and re-enabling shuffle can't create an
    // immediate repeat at the new cycle boundary.
    bag.cursor = EffectCount;
}

static int ResolveMinRestoreEffectStyle() {
    if (g_minRestoreShuffleEffect.load(std::memory_order_relaxed)) {
        if (auto* sharedState = EnsureSharedEffectShuffleState()) {
            return ConsumeGlobalEffectShuffle<8>(
                &sharedState->minRestoreNextOrdinal,
                &sharedState->minRestoreSeed, 0x4D525354u);
        }
        std::lock_guard<std::mutex> lock(g_EffectShuffleMutex);
        return ConsumeEffectShuffleLocked(g_MinRestoreEffectShuffle,
                                          0x4D525354u);
    }
    return g_minRestoreEffectStyle.load(std::memory_order_relaxed);
}
static int ResolveCloseEffectStyle() {
    if (g_closeShuffleEffect.load(std::memory_order_relaxed)) {
        if (auto* sharedState = EnsureSharedEffectShuffleState()) {
            return ConsumeGlobalEffectShuffle<6>(
                &sharedState->closeNextOrdinal,
                &sharedState->closeSeed, 0x434C4F53u);
        }
        std::lock_guard<std::mutex> lock(g_EffectShuffleMutex);
        return ConsumeEffectShuffleLocked(g_CloseEffectShuffle,
                                          0x434C4F53u);
    }
    return g_closeEffectStyle.load(std::memory_order_relaxed);
}

static ULONG_PTR ReadMinRestorePairToken(HWND hWnd, int* effectStyleOut);
static int PeekMinRestoreEffectStyle(HWND hWnd) {
    if (hWnd &&
        g_minRestoreShuffleEffect.load(std::memory_order_relaxed)) {
        int pairedEffectStyle = -1;
        if (ReadMinRestorePairToken(hWnd, &pairedEffectStyle)) {
            return pairedEffectStyle;
        }
    }
    if (g_minRestoreShuffleEffect.load(std::memory_order_relaxed)) {
        if (auto* sharedState = EnsureSharedEffectShuffleState()) {
            return PeekGlobalEffectShuffle<8>(
                &sharedState->minRestoreNextOrdinal,
                &sharedState->minRestoreSeed, 0x4D525354u);
        }
        std::lock_guard<std::mutex> lock(g_EffectShuffleMutex);
        return PeekEffectShuffleLocked(g_MinRestoreEffectShuffle,
                                       0x4D525354u);
    }
    return g_minRestoreEffectStyle.load(std::memory_order_relaxed);
}

static int PeekCloseEffectStyle() {
    if (g_closeShuffleEffect.load(std::memory_order_relaxed)) {
        if (auto* sharedState = EnsureSharedEffectShuffleState()) {
            return PeekGlobalEffectShuffle<6>(
                &sharedState->closeNextOrdinal,
                &sharedState->closeSeed, 0x434C4F53u);
        }
        std::lock_guard<std::mutex> lock(g_EffectShuffleMutex);
        return PeekEffectShuffleLocked(g_CloseEffectShuffle,
                                       0x434C4F53u);
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
    LONG showDesktopSequence =
        InterlockedIncrement(&g_pSharedState->showDesktopSequence);
    if (!(showDesktopSequence & 1)) {
        InterlockedIncrement(&g_pSharedState->showDesktopSequence);
    }
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
    InterlockedExchange(&g_pSharedState->showDesktopUntilTick, 0);
    InterlockedExchange(&g_pSharedState->showDesktopTargetWindow, 0);
    InterlockedExchange(&g_pSharedState->showDesktopLastAnimatedWindow, 0);
    MemoryBarrier();
    InterlockedIncrement(&g_pSharedState->showDesktopSequence);
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
static bool RequiresCpuAnimationRendererProcess() {
    // The renderer is initialized inside every process into which this mod is
    // injected. UniKey's small 32-bit UI process becomes unresponsive while a
    // D3D device and the effect shaders are initialized there, and may abort
    // its dialog show sequence. Keep all animations enabled, but never start
    // the GPU service in that process.
    static const bool requiresCpuRenderer = [] {
        WCHAR path[MAX_PATH]{};
        if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return false;
        const WCHAR* name = wcsrchr(path, L'\\');
        return _wcsicmp(name ? name + 1 : path, L"UniKeyNT.exe") == 0;
    }();
    return requiresCpuRenderer;
}
static DWORD GetShellExplorerProcessId() {
    DWORD shellPid = 0;
    if (HWND hShell = GetShellWindow()) {
        GetWindowThreadProcessId(hShell, &shellPid);
    }
    if (!shellPid) {
        if (HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            GetWindowThreadProcessId(hTray, &shellPid);
        }
    }
    return shellPid;
}
static bool IsShellExplorerProcess() {
    return IsExplorerProcess() &&
           GetShellExplorerProcessId() == GetCurrentProcessId();
}
static bool IsNonInteractiveSessionProcess() {
    DWORD sessionId = 0;
    return ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
           sessionId == 0;
}
static bool IsNonWindowHostingHelperProcess() {
    // Chromium and Electron reuse the application's executable for their
    // renderer, GPU, utility, crash-handler, and Node helper processes. Those
    // children don't own the application's top-level window, so installing
    // this mod's hooks in each of them only adds initialization work. The
    // browser/main process deliberately has no --type role and stays enabled.
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return false;

    bool helperProcess = false;
    for (int i = 1; i < argc && !helperProcess; ++i) {
        const wchar_t* argument = argv[i];
        const wchar_t* role = nullptr;
        if (_wcsnicmp(argument, L"--type=", 7) == 0) {
            role = argument + 7;
        } else if (_wcsicmp(argument, L"--type") == 0 && i + 1 < argc) {
            role = argv[++i];
        }

        if (role) {
            constexpr const wchar_t* kNonWindowHostingRoles[] = {
                L"renderer",        L"gpu-process", L"utility",
                L"crashpad-handler", L"plugin",      L"ppapi",
                L"zygote",          L"nacl-loader", L"nacl-broker",
            };
            for (const wchar_t* helperRole : kNonWindowHostingRoles) {
                if (_wcsicmp(role, helperRole) == 0) {
                    helperProcess = true;
                    break;
                }
            }
        }

        if (_wcsicmp(argument, L"--node-ipc") == 0 ||
            _wcsicmp(argument, L"--headless") == 0 ||
            _wcsicmp(argument, L"--headless=new") == 0 ||
            _wcsicmp(argument, L"--embedded-browser-webview=1") == 0 ||
            _wcsicmp(argument, L"--embedded-browser-edgeview=1") == 0) {
            helperProcess = true;
        }
    }

    LocalFree(argv);
    return helperProcess;
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

static bool GetAnimationWindowRects(HWND hWnd, RECT* windowRectOut,
                                    RECT* animationRectOut) {
    if (!hWnd || !windowRectOut || !animationRectOut ||
        !GetWindowRect(hWnd, windowRectOut)) {
        return false;
    }

    *animationRectOut = *windowRectOut;
    RECT extendedFrame{};
    if (SUCCEEDED(DwmGetWindowAttribute(
            hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extendedFrame,
            sizeof(extendedFrame))) &&
        PhysicalRectToAnimationSpace(hWnd, &extendedFrame)) {
        *animationRectOut = extendedFrame;
    }
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
static ULONG_PTR GetOrCreateTaskbarDockIdentityToken(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return 0;
    ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropTaskbarDockIdentity));
    if (token) return token;
    ULONG_PTR serial = g_NextTaskbarDockIdentityToken.fetch_add(
                           1, std::memory_order_relaxed) +
                       1;
    token = serial ^
            (static_cast<ULONG_PTR>(GetCurrentProcessId()) *
             static_cast<ULONG_PTR>(0x9E3779B1u)) ^
            static_cast<ULONG_PTR>(GetTickCount());
    if (!token) token = 1;
    if (!SetPropW(hWnd, kPropTaskbarDockIdentity,
                  reinterpret_cast<HANDLE>(token))) {
        return 0;
    }
    // Another injected process can race to establish the shared lifetime
    // token. Use whichever value is actually attached to the HWND.
    return reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropTaskbarDockIdentity));
}
static TaskbarDockWindowIdentity CaptureTaskbarDockWindowIdentity(
    HWND hWnd, bool createToken) {
    TaskbarDockWindowIdentity identity;
    identity.threadId = GetWindowThreadProcessId(hWnd, &identity.processId);
    if (!identity.threadId || !identity.processId) return {};
    identity.monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    identity.windowToken = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropTaskbarDockIdentity));
    if (!identity.windowToken && createToken) {
        identity.windowToken = GetOrCreateTaskbarDockIdentityToken(hWnd);
    }
    return identity;
}
static bool TaskbarDockIdentityMatches(
    const TaskbarDockWindowIdentity& lhs,
    const TaskbarDockWindowIdentity& rhs) {
    return lhs.processId && lhs.processId == rhs.processId &&
           lhs.threadId == rhs.threadId && lhs.monitor == rhs.monitor &&
           lhs.windowToken == rhs.windowToken;
}
static bool IsTaskbarDockCacheValueUsable(
    const TaskbarDockCacheValue& value,
    const TaskbarDockWindowIdentity& currentIdentity, HMONITOR monitor,
    TaskbarEdge edge, DWORD now) {
    if (value.edge != edge || value.identity.monitor != monitor ||
        !TaskbarDockIdentityMatches(value.identity, currentIdentity)) {
        return false;
    }
    // A property-backed identity lasts for the HWND lifetime. A tokenless
    // identity can be reused by another HWND on the same UI thread, so it is
    // deliberately useful only for a short burst of repeated operations.
    return value.identity.windowToken ||
           static_cast<DWORD>(now - value.observedTick) <
               static_cast<DWORD>(AnimConstants::UiaWeakIdentityCacheMs);
}
static bool IsTaskbarDockWindowIdentityCurrent(
    HWND hWnd, const TaskbarDockWindowIdentity& expected) {
    if (!hWnd || !IsWindow(hWnd) || !expected.processId ||
        !expected.threadId || !expected.monitor) {
        return false;
    }
    DWORD processId = 0;
    const DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
    if (processId != expected.processId || threadId != expected.threadId ||
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) !=
            expected.monitor) {
        return false;
    }
    return reinterpret_cast<ULONG_PTR>(
               GetPropW(hWnd, kPropTaskbarDockIdentity)) ==
           expected.windowToken;
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
static bool IsShowDesktopRestorePrepared(HWND hWnd);
static void ReassertPreparedShowDesktopRestoreCloak(HWND hWnd);
static bool IsClassicShowDesktopOperation();
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
static void RestoreWindowUnderGhostAsync(HWND hWnd, LONG_PTR originalExStyle,
                                         BOOL restoreMaximized) {
    if (!hWnd || !IsWindow(hWnd)) return;
    // Always enqueue the final state. During rapid reversals an earlier async
    // minimize can still be ahead of the current thread in the queue even when the sampled state currently
    // looks restored; the idempotent command makes the newest intent win.
    ShowWindowAsync_Original(hWnd,
                             StableRestoreShowCmd(restoreMaximized));
    RestoreZOrderAfterGhostAsync(hWnd, originalExStyle);
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

struct ShowDesktopOwnedSurfaceCloakContext {
    HWND rootWindow;
    ULONG_PTR token;
    BOOL cloak;
};

static BOOL CALLBACK UpdateShowDesktopOwnedSurfaceCloakEnumProc(
    HWND candidate, LPARAM lParam) {
    auto* context = reinterpret_cast<ShowDesktopOwnedSurfaceCloakContext*>(
        lParam);
    if (!context || !context->token || candidate == context->rootWindow) {
        return TRUE;
    }

    const ULONG_PTR marker = reinterpret_cast<ULONG_PTR>(
        GetPropW(candidate, kPropShowDesktopOwnedSurfaceCloak));
    if (!context->cloak) {
        // Search by marker rather than current ownership. A framework can
        // detach or replace an owned surface while the animation is running.
        if (marker == context->token) {
            RemovePropW(candidate, kPropShowDesktopOwnedSurfaceCloak);
            SetWindowCloak(candidate, FALSE);
        }
        return TRUE;
    }

    if (!IsWindowVisible(candidate) || IsIconic(candidate) ||
        GetAncestor(candidate, GA_ROOTOWNER) != context->rootWindow) {
        return TRUE;
    }
    if (marker && marker != context->token) return TRUE;

    if (!marker) {
        BOOL alreadyCloaked = FALSE;
        if (SUCCEEDED(DwmGetWindowAttribute(
                candidate, DWMWA_CLOAKED, &alreadyCloaked,
                sizeof(alreadyCloaked))) &&
            alreadyCloaked) {
            // Preserve cloaks owned by Windows or the application itself.
            return TRUE;
        }
        if (!SetPropW(candidate, kPropShowDesktopOwnedSurfaceCloak,
                      reinterpret_cast<HANDLE>(context->token)) ||
            reinterpret_cast<ULONG_PTR>(GetPropW(
                candidate, kPropShowDesktopOwnedSurfaceCloak)) !=
                context->token) {
            return TRUE;
        }
    }

    BOOL cloak = TRUE;
    if (FAILED(DwmSetWindowAttribute(candidate, DWMWA_CLOAK, &cloak,
                                     sizeof(cloak)))) {
        if (!marker) {
            RemovePropW(candidate, kPropShowDesktopOwnedSurfaceCloak);
        }
    }
    return TRUE;
}

static void UpdateShowDesktopOwnedSurfaceCloaks(HWND rootWindow,
                                                 ULONG_PTR token,
                                                 BOOL cloak) {
    if (!rootWindow || !token) return;
    ShowDesktopOwnedSurfaceCloakContext context{rootWindow, token, cloak};
    EnumWindows(UpdateShowDesktopOwnedSurfaceCloakEnumProc,
                reinterpret_cast<LPARAM>(&context));
}

static void ReleaseShowDesktopAnimationIfCurrent(HWND hWnd,
                                                  ULONG_PTR token) {
    if (!hWnd || !token) return;
    if (reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopAnimationOwner)) != token) {
        UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, FALSE);
        if (reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopCloakReady)) == token) {
            RemovePropW(hWnd, kPropShowDesktopCloakReady);
        }
        return;
    }

    // Disarm target-process MINIMIZEEND callbacks before removing any cloak.
    // Keep the owner property until cleanup is complete so a new optimized
    // Show Desktop session cannot claim this HWND in the middle of the handoff.
    if (reinterpret_cast<ULONG_PTR>(GetPropW(
            hWnd, kPropShowDesktopRestorePrepared)) == token) {
        RemovePropW(hWnd, kPropShowDesktopRestorePrepared);
    }
    UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, FALSE);
    SetWindowCloak(hWnd, FALSE);
    UpdateDwmTransitions(hWnd, TRUE);
    if (reinterpret_cast<ULONG_PTR>(GetPropW(
            hWnd, kPropShowDesktopCloakReady)) == token) {
        RemovePropW(hWnd, kPropShowDesktopCloakReady);
    }
    RemovePropW(hWnd, kPropShowDesktopAnimationOwnerUntil);
    RemovePropW(hWnd, kPropShowDesktopAnimationOwner);
}
static ULONG_PTR TryClaimShowDesktopAnimation(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return 0;

    const DWORD now = GetTickCount();
    const ULONG_PTR existing = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopAnimationOwner));
    if (existing) {
        const DWORD untilTick = static_cast<DWORD>(
            reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopAnimationOwnerUntil)));
        if (untilTick && static_cast<LONG>(now - untilTick) < 0) {
            return 0;
        }

        // A process can disappear after claiming a foreign HWND. Expire such
        // an abandoned claim instead of disabling this window's optimized
        // Show Desktop animation for the rest of its lifetime.
        if (reinterpret_cast<ULONG_PTR>(
                GetPropW(hWnd, kPropShowDesktopAnimationOwner)) == existing) {
            ReleaseShowDesktopAnimationIfCurrent(hWnd, existing);
        }
    }

    const ULONG_PTR serial =
        g_NextShowDesktopAnimationToken.fetch_add(
            1, std::memory_order_relaxed) +
        1;
    ULONG_PTR token =
        serial ^
        (static_cast<ULONG_PTR>(GetCurrentProcessId()) *
         static_cast<ULONG_PTR>(0xC2B2AE35u)) ^
        reinterpret_cast<ULONG_PTR>(hWnd) ^
        static_cast<ULONG_PTR>(now);
    if (!token) token = 1;
    DWORD untilTick = now + 5000;
    if (!untilTick) untilTick = 1;
    if (!SetPropW(hWnd, kPropShowDesktopAnimationOwner,
                  reinterpret_cast<HANDLE>(token))) {
        return 0;
    }
    if (!SetPropW(
            hWnd, kPropShowDesktopAnimationOwnerUntil,
            reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(untilTick)))) {
        ReleaseShowDesktopAnimationIfCurrent(hWnd, token);
        return 0;
    }

    // Explorer normally enters the target hook synchronously, but verify the
    // HWND property after publishing both values so a competing process can't
    // silently turn this into two custom animations.
    MemoryBarrier();
    if (reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopAnimationOwner)) != token) {
        return 0;
    }
    return token;
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
                                  int width, int height,
                                  bool logFailure = false) {
    auto it = g_WndSnapshots.find(hWnd);
    if (it == g_WndSnapshots.end()) {
        if (logFailure && IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Restore snapshot cache miss hwnd=%p requested=%dx%d",
                   hWnd, width, height);
        }
        return false;
    }
    const SnapCache& cache = it->second;
    const ULONG_PTR publishedToken = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropSnapshotCache));
    const bool valid = destination && cache.pBits && width > 0 && height > 0 &&
                       cache.w > 0 && cache.h > 0 && cache.windowToken &&
                       publishedToken == cache.windowToken;
    const bool dimensionsCompatible =
        valid && static_cast<int64_t>(width) * 4 >= cache.w &&
        static_cast<int64_t>(cache.w) * 4 >= width &&
        static_cast<int64_t>(height) * 4 >= cache.h &&
        static_cast<int64_t>(cache.h) * 4 >= height;
    if (valid && cache.w == width && cache.h == height) {
        memcpy(destination, cache.pBits, cache.bytes);
    } else if (dimensionsCompatible) {
        // DWM can report slightly different extended-frame dimensions after a
        // minimized window is restored (especially after a display/DPI state
        // change). Preserve the known-good source instead of discarding it and
        // attempting an impossible screen capture while the window is cloaked.
        const auto* source = static_cast<const DWORD*>(cache.pBits);
        auto* target = static_cast<DWORD*>(destination);
        for (int y = 0; y < height; ++y) {
            const int sourceY = std::min(
                cache.h - 1,
                static_cast<int>((static_cast<int64_t>(y) * cache.h) /
                                 height));
            const DWORD* sourceRow =
                source + static_cast<size_t>(sourceY) * cache.w;
            DWORD* targetRow = target + static_cast<size_t>(y) * width;
            for (int x = 0; x < width; ++x) {
                const int sourceX = std::min(
                    cache.w - 1,
                    static_cast<int>((static_cast<int64_t>(x) * cache.w) /
                                     width));
                targetRow[x] = sourceRow[sourceX];
            }
        }
        if (logFailure && IsDiagnosticLoggingEnabled()) {
            Wh_Log(
                L"Restore snapshot resized hwnd=%p cached=%dx%d requested=%dx%d",
                hWnd, cache.w, cache.h, width, height);
        }
    } else if (logFailure && IsDiagnosticLoggingEnabled()) {
        Wh_Log(
            L"Restore snapshot cache invalid/incompatible hwnd=%p "
            L"cached=%dx%d requested=%dx%d token=%p published=%p bits=%d",
            hWnd, cache.w, cache.h, width, height,
            reinterpret_cast<void*>(cache.windowToken),
            reinterpret_cast<void*>(publishedToken), cache.pBits != nullptr);
    }
    EraseSnapshotLocked(it);
    return valid && dimensionsCompatible;
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
    if (bytes > AnimConstants::SnapshotCacheMaxBytes) {
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Snapshot cache rejected hwnd=%p size=%dx%d bytes=%zu",
                   hWnd, width, height, bytes);
        }
        return false;
    }

    EraseSnapshotLocked(hWnd);
    const ULONG_PTR token = CreateSnapshotCacheToken();
    if (!SetPropW(hWnd, kPropSnapshotCache,
                  reinterpret_cast<HANDLE>(token))) {
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Snapshot cache property failed hwnd=%p err=%lu",
                   hWnd, GetLastError());
        }
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
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Snapshot cache allocation failed hwnd=%p size=%dx%d",
                   hWnd, width, height);
        }
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
    RemovePropW(hWnd, kPropShowDesktopNativeMinimize);
    std::lock_guard<std::mutex> lock(g_StateMutex);
    EraseSnapshotLocked(hWnd);
    g_TaskbarDockPositions.erase(hWnd);
    g_TaskbarDockFallbackPositions.erase(hWnd);
    g_TaskbarDockLookupGenerations.erase(hWnd);
    g_TaskbarDockLookupIdentities.erase(hWnd);
    g_TaskbarDockLookupStartedTicks.erase(hWnd);
    g_TaskbarDockNegativeUntilTicks.erase(hWnd);
    g_TaskbarDockPositiveUntilTicks.erase(hWnd);
    g_ProcessNameCache.erase(hWnd);
    g_LaunchSeen.erase(hWnd);
    g_AnimWantRising.erase(hWnd);
    g_AnimRestoreRequestForeground.erase(hWnd);
    g_ShowDesktopNativeMarkers.erase(hWnd);
    g_ShowDesktopMarkerCleanups.erase(hWnd);
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
    for (auto it = g_TaskbarDockPositions.begin();
         it != g_TaskbarDockPositions.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockPositions.erase(it);
        else ++it;
    }
    for (auto it = g_TaskbarDockFallbackPositions.begin();
         it != g_TaskbarDockFallbackPositions.end();) {
        if (!IsWindow(it->first)) {
            it = g_TaskbarDockFallbackPositions.erase(it);
        }
        else ++it;
    }
    for (auto it = g_TaskbarDockLookupGenerations.begin();
         it != g_TaskbarDockLookupGenerations.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockLookupGenerations.erase(it);
        else ++it;
    }
    for (auto it = g_TaskbarDockLookupIdentities.begin();
         it != g_TaskbarDockLookupIdentities.end();) {
        if (!IsWindow(it->first)) it = g_TaskbarDockLookupIdentities.erase(it);
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
    for (auto it = g_ShowDesktopNativeMarkers.begin();
         it != g_ShowDesktopNativeMarkers.end();) {
        if (!IsWindow(it->first) ||
            reinterpret_cast<ULONG_PTR>(GetPropW(
                it->first, kPropShowDesktopNativeMinimize)) != it->second) {
            it = g_ShowDesktopNativeMarkers.erase(it);
        } else {
            ++it;
        }
    }
}
static bool RequiresScreenCapture(HWND hWnd) {
    const auto cls = GetClassNameStr(hWnd);
    return cls.find(L"CASCADIA") != std::wstring::npos ||
           cls.find(L"ConsoleWindowClass") != std::wstring::npos;
}
static bool ShouldUseBitBlt(HWND hWnd, bool isClosing) {
    return isClosing &&
           (GetForegroundWindow() == hWnd || RequiresScreenCapture(hWnd));
}
static bool IsTransparentXamlExplorerHostIsland(HWND hWnd,
                                                LONG_PTR exStyle) {
    // Explorer keeps this composition-island host visible with a large
    // bookkeeping rectangle even after the shell surface it hosted has become
    // fully transparent. Treat only the transparent, per-pixel-composited
    // tool-window form as non-occluding; an ordinary XAML host can still be a
    // real visible window and must continue to reject a screen snapshot.
    constexpr LONG_PTR requiredExStyle =
        WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW |
        WS_EX_TRANSPARENT;
    if ((exStyle & requiredExStyle) != requiredExStyle) return false;
    WCHAR className[64]{};
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0;
}
struct ScreenCaptureEligibility {
    enum class FailureReason {
        None,
        TargetUnavailable,
        TargetCloaked,
        OverlappingWindow,
        UnstableZOrder,
    } failureReason{FailureReason::None};
    bool taskbarAboveAndOverlapping = false;
    RECT taskbarRect{};
    HWND blockingSwitchAnimationGhost = nullptr;
    HWND blockingWindow = nullptr;
};

static bool CanCaptureWindowFromScreen(
    HWND hWnd, const RECT& captureRect, HWND hTaskbar,
    bool includeTargetOwnedSurfaces,
    ScreenCaptureEligibility* eligibilityOut = nullptr) {
    ScreenCaptureEligibility eligibility;
    if (!hWnd || !IsWindowVisible(hWnd) || IsIconic(hWnd)) {
        eligibility.failureReason =
            ScreenCaptureEligibility::FailureReason::TargetUnavailable;
        if (eligibilityOut) *eligibilityOut = eligibility;
        return false;
    }
    BOOL targetCloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &targetCloaked,
                                        sizeof(targetCloaked))) &&
        targetCloaked) {
        eligibility.failureReason =
            ScreenCaptureEligibility::FailureReason::TargetCloaked;
        if (eligibilityOut) *eligibilityOut = eligibility;
        return false;
    }

    // Walk only windows above the target instead of sweeping the desktop. A
    // finite bound keeps a concurrently mutating Z-order from trapping the
    // shell hook; hitting the bound conservatively rejects the capture.
    HWND candidate = GetWindow(hWnd, GW_HWNDPREV);
    for (int remaining = 512; candidate && remaining > 0; --remaining) {
        const HWND next = GetWindow(candidate, GW_HWNDPREV);
        // IsWindowVisible remains true for many minimized Chromium/Electron
        // HWNDs, and DWM can retain their old full-size extended-frame bounds.
        // They don't contribute pixels to the composed desktop and therefore
        // can't occlude this screen snapshot.
        if (IsWindowVisible(candidate) && !IsIconic(candidate)) {
            if (includeTargetOwnedSurfaces &&
                GetAncestor(candidate, GA_ROOTOWNER) == hWnd) {
                // Chromium/Electron can expose a transient top-level surface
                // above its taskbar window. It is part of the same visual and
                // is captured, cloaked and restored with the root window.
                candidate = next;
                continue;
            }
            BOOL cloaked = FALSE;
            const bool isCloaked =
                SUCCEEDED(DwmGetWindowAttribute(candidate, DWMWA_CLOAKED,
                                                &cloaked,
                                                sizeof(cloaked))) &&
                cloaked;
            const LONG_PTR candidateExStyle =
                GetWindowLongPtrW(candidate, GWL_EXSTYLE);
            if (!isCloaked && IsTransparentXamlExplorerHostIsland(
                                  candidate, candidateExStyle)) {
                candidate = next;
                continue;
            }
            bool fullyTransparent = false;
            if (!isCloaked &&
                (candidateExStyle & WS_EX_LAYERED)) {
                COLORREF colorKey = 0;
                BYTE alpha = 255;
                DWORD flags = 0;
                // GetLayeredWindowAttributes fails for per-pixel-alpha windows
                // created with UpdateLayeredWindow; those remain occluders.
                fullyTransparent =
                    GetLayeredWindowAttributes(candidate, &colorKey, &alpha,
                                               &flags) &&
                    (flags & LWA_ALPHA) && alpha == 0;
            }
            RECT candidateRect{};
            RECT overlap{};
            if (!isCloaked && !fullyTransparent) {
                if (candidate == hTaskbar) {
                    if (GetWindowRect(candidate, &candidateRect) &&
                        IntersectRect(&overlap, &captureRect,
                                      &candidateRect)) {
                        eligibility.taskbarAboveAndOverlapping = true;
                        eligibility.taskbarRect = candidateRect;
                    }
                } else {
                    if (FAILED(DwmGetWindowAttribute(
                            candidate, DWMWA_EXTENDED_FRAME_BOUNDS,
                            &candidateRect, sizeof(candidateRect))) ||
                        !PhysicalRectToAnimationSpace(hWnd,
                                                      &candidateRect)) {
                        if (!GetWindowRect(candidate, &candidateRect)) {
                            candidate = next;
                            continue;
                        }
                    }
                    if (IntersectRect(&overlap, &captureRect,
                                      &candidateRect)) {
                        eligibility.failureReason =
                            ScreenCaptureEligibility::FailureReason::
                                OverlappingWindow;
                        eligibility.blockingWindow = candidate;
                        if (GetPropW(candidate, kPropSwitchAnimationGhost)) {
                            eligibility.blockingSwitchAnimationGhost = candidate;
                        }
                        if (eligibilityOut) *eligibilityOut = eligibility;
                        return false;
                    }
                }
            }
        }
        candidate = next;
    }
    if (candidate) {
        eligibility.failureReason =
            ScreenCaptureEligibility::FailureReason::UnstableZOrder;
        eligibility.blockingWindow = candidate;
        if (eligibilityOut) *eligibilityOut = eligibility;
        return false;
    }
    if (eligibilityOut) *eligibilityOut = eligibility;
    return true;
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

static PSECURITY_DESCRIPTOR BuildSharedEffectShuffleStateSd() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        return nullptr;
    }
    DWORD len = 0;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &len);
    std::vector<BYTE> buffer(len);
    LPWSTR userSid = NULL;
    const bool haveSid =
        len && GetTokenInformation(hToken, TokenUser, buffer.data(), len,
                                   &len) &&
        ConvertSidToStringSidW(
            reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid,
            &userSid) &&
        userSid;
    CloseHandle(hToken);
    if (!haveSid) return nullptr;

    // The mapping contains only shuffle seeds and counters. App-container
    // targets need write access so packaged and classic apps can participate
    // in the same session-wide order; no HWNDs or control state are exposed.
    std::wstring sddl = L"D:(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;";
    sddl += userSid;
    sddl += L")(A;;GRGW;;;AC)S:(ML;;NW;;;LW)";
    LocalFree(userSid);
    PSECURITY_DESCRIPTOR pSd = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &pSd, NULL)) {
        return nullptr;
    }
    return pSd;
}

static uint32_t CreateSharedEffectShuffleSeed(uint32_t seedSalt) {
    LARGE_INTEGER counter{};
    QueryPerformanceCounter(&counter);
    uint32_t value = static_cast<uint32_t>(counter.LowPart) ^
                     static_cast<uint32_t>(counter.HighPart) ^
                     static_cast<uint32_t>(GetTickCount64()) ^
                     static_cast<uint32_t>(GetTickCount64() >> 32) ^
                     (static_cast<uint32_t>(GetCurrentProcessId()) *
                      0x9E3779B9u) ^
                     (static_cast<uint32_t>(GetCurrentThreadId()) *
                      0x85EBCA6Bu) ^
                     seedSalt;
    return MixGlobalShuffleValue(value);
}

static bool InitializeSharedEffectShuffleState(
    SharedEffectShuffleState* state) {
    if (!state) return false;
    if (InterlockedCompareExchange(&state->magic, 0, 0) ==
        kSharedEffectShuffleStateMagic) {
        return true;
    }

    const LONG previousInitializationState = InterlockedCompareExchange(
        &state->initializationState, 1, 0);
    if (previousInitializationState == 0) {
        InterlockedExchange(&state->minRestoreNextOrdinal, 0);
        InterlockedExchange(&state->closeNextOrdinal, 0);
        InterlockedExchange(
            &state->minRestoreSeed,
            static_cast<LONG>(CreateSharedEffectShuffleSeed(0x4D525354u)));
        InterlockedExchange(
            &state->closeSeed,
            static_cast<LONG>(CreateSharedEffectShuffleSeed(0x434C4F53u)));
        MemoryBarrier();
        InterlockedExchange(&state->magic,
                            kSharedEffectShuffleStateMagic);
        InterlockedExchange(&state->initializationState, 2);
        return true;
    }

    const DWORD deadline = GetTickCount() + 100;
    do {
        if (InterlockedCompareExchange(&state->magic, 0, 0) ==
            kSharedEffectShuffleStateMagic) {
            return true;
        }
        Sleep(0);
    } while (static_cast<LONG>(GetTickCount() - deadline) < 0);

    return false;
}

static void CloseSharedEffectShuffleStateLocked() {
    if (g_pSharedEffectShuffleState) {
        UnmapViewOfFile(g_pSharedEffectShuffleState);
        g_pSharedEffectShuffleState = nullptr;
    }
    if (g_hSharedEffectShuffleMap) {
        CloseHandle(g_hSharedEffectShuffleMap);
        g_hSharedEffectShuffleMap = NULL;
    }
}

static SharedEffectShuffleState* EnsureSharedEffectShuffleState() {
    std::lock_guard<std::mutex> lock(g_SharedEffectShuffleStateMutex);
    if (g_pSharedEffectShuffleState &&
        InterlockedCompareExchange(&g_pSharedEffectShuffleState->magic,
                                   0, 0) ==
            kSharedEffectShuffleStateMagic) {
        return g_pSharedEffectShuffleState;
    }
    CloseSharedEffectShuffleStateLocked();

    PSECURITY_DESCRIPTOR pSd = BuildSharedEffectShuffleStateSd();
    SECURITY_ATTRIBUTES sa{sizeof(sa), pSd, FALSE};
    HANDLE hMap = CreateFileMappingW(
        INVALID_HANDLE_VALUE, pSd ? &sa : NULL, PAGE_READWRITE, 0,
        sizeof(SharedEffectShuffleState), kSharedEffectShuffleStateName);
    if (pSd) LocalFree(pSd);
    if (!hMap) {
        hMap = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE,
                                kSharedEffectShuffleStateName);
    }
    if (!hMap) return nullptr;

    auto* state = reinterpret_cast<SharedEffectShuffleState*>(MapViewOfFile(
        hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0,
        sizeof(SharedEffectShuffleState)));
    if (!state || !InitializeSharedEffectShuffleState(state)) {
        if (state) UnmapViewOfFile(state);
        CloseHandle(hMap);
        return nullptr;
    }

    g_hSharedEffectShuffleMap = hMap;
    g_pSharedEffectShuffleState = state;
    return state;
}

static void CloseSharedEffectShuffleState() {
    std::lock_guard<std::mutex> lock(g_SharedEffectShuffleStateMutex);
    CloseSharedEffectShuffleStateLocked();
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
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Explorer claimed Alt+Tab shared state epoch=%ld existing=%d",
                   g_pSharedState->sessionEpoch,
                   createErr == ERROR_ALREADY_EXISTS ? 1 : 0);
        }
        return;
    }
    g_hMapFile = OpenFileMappingW(FILE_MAP_READ, FALSE, kSharedStateName);
    if (!g_hMapFile) {
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Shared state not open yet (will retry on Alt+Tab)");
        }
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
    const bool closeShuffleEnabled =
        Wh_GetIntSetting(L"close.random_effect") != 0;
    const bool minRestoreShuffleEnabled =
        Wh_GetIntSetting(L"minimize_restore.random_effect") != 0;
    {
        std::lock_guard<std::mutex> lock(g_EffectShuffleMutex);
        if (g_closeShuffleEffect.load(std::memory_order_relaxed) !=
            closeShuffleEnabled) {
            InvalidateEffectShuffleLocked(g_CloseEffectShuffle);
        }
        if (g_minRestoreShuffleEffect.load(std::memory_order_relaxed) !=
            minRestoreShuffleEnabled) {
            InvalidateEffectShuffleLocked(g_MinRestoreEffectShuffle);
        }
        g_closeShuffleEffect.store(closeShuffleEnabled,
                                   std::memory_order_relaxed);
        g_minRestoreShuffleEffect.store(minRestoreShuffleEnabled,
                                        std::memory_order_relaxed);
    }
    g_minimizeAnimation.store(Wh_GetIntSetting(L"minimize_restore.minimize_animation") != 0, std::memory_order_relaxed);
    g_restoreAnimation.store(Wh_GetIntSetting(L"minimize_restore.restore_animation") != 0, std::memory_order_relaxed);
    g_closeAnimation.store(Wh_GetIntSetting(L"close.close_animation") != 0, std::memory_order_relaxed);
    g_hideAsClose.store(Wh_GetIntSetting(L"close.hide_as_close") != 0, std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"minimize_restore.launch_animation") != 0, std::memory_order_relaxed);
    g_switchAnimation.store(Wh_GetIntSetting(L"window_switch.switch_animation") != 0, std::memory_order_relaxed);
    g_switchDurationMs.store(Clamp(Wh_GetIntSetting(L"window_switch.duration_ms"), 50, 1000), std::memory_order_relaxed);
    g_unhideEnabled.store(Wh_GetIntSetting(L"minimize_restore.unhide_taskbar") != 0, std::memory_order_relaxed);
    g_unhideDurationMs.store(Clamp(Wh_GetIntSetting(L"minimize_restore.unhide_duration_ms"), 0, 5000), std::memory_order_relaxed);
    g_gpuAcceleration.store(
        Wh_GetIntSetting(L"minimize_restore.gpu_acceleration") != 0,
        std::memory_order_relaxed);
    g_closeGpuAcceleration.store(
        Wh_GetIntSetting(L"close.gpu_acceleration") != 0,
        std::memory_order_relaxed);
    g_performanceTelemetry.store(
        Wh_GetIntSetting(L"diagnostics.performance_telemetry") != 0,
        std::memory_order_relaxed);
    const bool showDesktopTopWindowOnly =
        Wh_GetIntSetting(
            L"minimize_restore.show_desktop_top_window_only") != 0;
    g_showDesktopTopWindowOnly.store(showDesktopTopWindowOnly,
                                     std::memory_order_relaxed);
    if (!showDesktopTopWindowOnly) {
        g_localShowDesktopUntilTick.store(0, std::memory_order_relaxed);
        g_localShowDesktopTarget.store(nullptr, std::memory_order_relaxed);
    }
}
static void UpdateDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}
static void SetWindowCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}
static bool IsExplorerFrameWindow(HWND hWnd) {
    WCHAR cls[64]{};
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           (_wcsicmp(cls, L"CabinetWClass") == 0 ||
            _wcsicmp(cls, L"ExploreWClass") == 0);
}
static bool HasExplicitSystemBackdrop(HWND hWnd, UINT* backdropOut = nullptr) {
    UINT backdrop = 0;
    const bool present =
        SUCCEEDED(DwmGetWindowAttribute(
            hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop,
            sizeof(backdrop))) &&
        backdrop >= 2;
    if (backdropOut) *backdropOut = backdrop;
    return present;
}
static bool RequiresCpuClosePresentation(HWND hWnd) {
    // Preserve the already-proven CPU exception for Explorer and windows that
    // explicitly participate in system backdrops while the general GPU close
    // presenter changes independently. Their layered CPU path is stable and
    // avoids unnecessary DWM chrome repair after the real window is uncloaked.
    return IsExplorerFrameWindow(hWnd) || HasExplicitSystemBackdrop(hWnd);
}
static bool RequiresCpuLaunchPresentation(HWND hWnd) {
    WCHAR cls[64]{};
    // UniKey changes this dialog's utility-window state during ShowWindow.
    // Its established layered-window launch path is stable, but placing a
    // DirectComposition host into that transition can make the dialog abort
    // its show sequence. Keep the animation and only change its renderer.
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           _wcsicmp(cls, L"UniKey MainWnd") == 0;
}
static bool RefreshDwmChromeAfterUncloak(HWND hWnd,
                                         bool nonBlocking = false,
                                         bool lightweight = false) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    const bool explorerFrame = IsExplorerFrameWindow(hWnd);
    UINT backdrop = 0;
    const bool explicitBackdrop =
        HasExplicitSystemBackdrop(hWnd, &backdrop);
    if (!explorerFrame && !explicitBackdrop) return false;
    if (explicitBackdrop) {
        DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    }
    if (explorerFrame && !lightweight) {
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }
    if (lightweight) {
        // Alt+Tab only needs an explicit backdrop value reasserted after its
        // short cloak. Extending Explorer's frame across the full client here
        // can leave the default Explorer UI participating in Mica because this
        // lightweight path intentionally skips the later frame/composition
        // rebuild. A full SWP_FRAMECHANGED can also stall heavily styled XAML
        // content, so leave Explorer's existing frame margins untouched.
        return true;
    }
    SetWindowPos_Original(
        hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
            SWP_FRAMECHANGED | (nonBlocking ? SWP_ASYNCWINDOWPOS : 0));
    if (nonBlocking) {
        // RedrawWindow can synchronously marshal to the owner. Let the queued
        // frame change and composition notification repaint a foreign window.
        PostMessageW(hWnd, WM_DWMCOMPOSITIONCHANGED, 0, 0);
    } else {
        RedrawWindow(hWnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                         RDW_ALLCHILDREN);
        SendMessageTimeoutW(hWnd, WM_DWMCOMPOSITIONCHANGED, 0, 0,
                            SMTO_ABORTIFHUNG | SMTO_NORMAL, 50, nullptr);
    }
    return true;
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
            // No custom animation session owns this token. A native fallback
            // must end the pending pair so it can't leak into a later cycle.
            ClearMinRestorePairIfCurrent(hWnd, pairedEffectToken);
            if (wantRising) {
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
    ClearMinRestorePairIfCurrent(hWnd, pairedEffectToken);
    if (wantRising) {
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
    DWORD ownerPid = 0;
    const DWORD ownerTid = GetWindowThreadProcessId(hWnd, &ownerPid);
    const ULONG_PTR windowToken = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropTaskbarDockIdentity));
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto it = g_ProcessNameCache.find(hWnd);
        if (it != g_ProcessNameCache.end() &&
            it->second.processId == ownerPid &&
            it->second.threadId == ownerTid &&
            it->second.windowToken == windowToken) {
            return it->second.name;
        }
        if (it != g_ProcessNameCache.end()) g_ProcessNameCache.erase(it);
    }
    std::wstring procNameLower = L"";
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
        // A transient OpenProcess/QueryFullProcessImageName failure must not
        // permanently remove the process-name hint for this HWND. In
        // particular, "explorer" is often the only stable UIA match for a
        // File Explorer taskbar button whose caption is the current folder.
        if (!procNameLower.empty()) {
            g_ProcessNameCache[hWnd] =
                ProcessNameCacheValue{ownerPid, ownerTid, windowToken,
                                      procNameLower};
        }
    }
    return procNameLower;
}
static bool NeedsLocalWinEventThread() {
    const bool needTaskbarObserver =
        IsShellExplorerProcess() &&
        (g_minimizeAnimation.load(std::memory_order_relaxed) ||
         g_restoreAnimation.load(std::memory_order_relaxed));
    const bool needShowDesktopRestoreObserver =
        g_showDesktopTopWindowOnly.load(std::memory_order_relaxed) &&
        g_restoreAnimation.load(std::memory_order_relaxed);
    return g_switchAnimation.load(std::memory_order_relaxed) ||
           needTaskbarObserver || needShowDesktopRestoreObserver;
}

void EnsureWinEventThreadStarted() {
    // This is also a lazy recovery path for a shell that became ready after
    // the ownership probe's initial fast-poll period.
    if (g_switchAnimation.load(std::memory_order_relaxed)) {
        EnsureExplorerForegroundThreadStarted();
    }
    if (g_winEventThreadStarted.load(std::memory_order_relaxed)) return;
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (!NeedsLocalWinEventThread()) return;
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (!NeedsLocalWinEventThread()) return;
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
    if (!forSwitch && IsExplorerProcess() &&
        _wcsicmp(cls.c_str(), L"CabinetWClass") != 0 &&
        _wcsicmp(cls.c_str(), L"ExploreWClass") != 0) {
        // Explorer owns the desktop, taskbar, and transient full-monitor shell
        // surfaces used around lock/power transitions. They can otherwise look
        // like large unowned app windows and trigger a whole-screen close
        // animation on resume. Only folder windows are close-animation targets.
        return false;
    }
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
    return (style & WS_CAPTION) && !(exStyle & WS_EX_TOOLWINDOW) &&
           ShouldAnimateWindow(hWnd);
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

static int GetAnimatedWindowCornerRadius(HWND hWnd, BOOL maximized,
                                         LONG_PTR originalExStyle) {
    if (!hWnd || maximized) return 0;

    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DEFAULT;
    if (FAILED(DwmGetWindowAttribute(
            hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
            sizeof(preference))) ||
        preference == DWMWCP_DONOTROUND) {
        // The attribute isn't available before Windows 11. In that case the
        // native window has no system-managed rounded silhouette to preserve.
        return 0;
    }

    if (preference == DWMWCP_DEFAULT) {
        const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if (!(style & WS_CAPTION) || (originalExStyle & WS_EX_LAYERED)) {
            // DWM doesn't apply its default rounding to these window forms.
            // An explicit DWMWCP_ROUND/ROUNDSMALL request still takes effect.
            return 0;
        }
    }

    const int logicalRadius =
        preference == DWMWCP_ROUNDSMALL ? 4 : 8;
    return std::max(
        1, MulDiv(logicalRadius,
                  static_cast<int>(GetWindowDpiCompat(hWnd)), 96));
}

static void ApplyRoundedCornerMask(void* bitmapBits, int width, int height,
                                   int radius) {
    if (!bitmapBits || width <= 0 || height <= 0 || radius <= 0) return;
    radius = std::min(radius, std::min(width, height) / 2);
    if (radius <= 0) return;

    auto* pixels = static_cast<DWORD*>(bitmapBits);
    auto limitAlpha = [](DWORD& pixel, BYTE maskAlpha) {
        const BYTE oldAlpha = static_cast<BYTE>(pixel >> 24);
        if (maskAlpha >= oldAlpha) return;
        if (!maskAlpha || !oldAlpha) {
            pixel = 0;
            return;
        }

        // The layered and composition presenters both consume premultiplied
        // BGRA. Scaling RGB by the alpha ratio keeps the edge free of dark
        // halos, and making the operation a clamp keeps it idempotent when a
        // cached minimize snapshot is consumed by restore.
        const DWORD blue = pixel & 0xFFu;
        const DWORD green = (pixel >> 8) & 0xFFu;
        const DWORD red = (pixel >> 16) & 0xFFu;
        const auto scale = [oldAlpha, maskAlpha](DWORD channel) {
            return (channel * maskAlpha + oldAlpha / 2u) / oldAlpha;
        };
        pixel = static_cast<DWORD>(maskAlpha) << 24 |
                scale(red) << 16 | scale(green) << 8 | scale(blue);
    };

    constexpr int samplesPerAxis = 4;
    constexpr int sampleCount = samplesPerAxis * samplesPerAxis;
    const float circleRadius = static_cast<float>(radius);
    const float radiusSquared = circleRadius * circleRadius;
    for (int y = 0; y < radius; ++y) {
        for (int x = 0; x < radius; ++x) {
            int insideSamples = 0;
            for (int sampleY = 0; sampleY < samplesPerAxis; ++sampleY) {
                const float py = static_cast<float>(y) +
                                 (sampleY + 0.5f) / samplesPerAxis;
                const float dy = py - circleRadius;
                for (int sampleX = 0; sampleX < samplesPerAxis; ++sampleX) {
                    const float px = static_cast<float>(x) +
                                     (sampleX + 0.5f) / samplesPerAxis;
                    const float dx = px - circleRadius;
                    if (dx * dx + dy * dy <= radiusSquared) {
                        ++insideSamples;
                    }
                }
            }

            const BYTE maskAlpha = static_cast<BYTE>(
                (insideSamples * 255 + sampleCount / 2) / sampleCount);
            limitAlpha(pixels[static_cast<size_t>(y) * width + x],
                       maskAlpha);
            limitAlpha(pixels[static_cast<size_t>(y) * width +
                              (width - 1 - x)], maskAlpha);
            limitAlpha(pixels[static_cast<size_t>(height - 1 - y) * width + x],
                       maskAlpha);
            limitAlpha(pixels[static_cast<size_t>(height - 1 - y) * width +
                              (width - 1 - x)], maskAlpha);
        }
    }
}

struct TaskbarGeometry {
    TaskbarEdge edge{TaskbarEdge::Bottom};
    RECT rect{};
    bool valid{};
    bool expanded{};
    float innerEdge{};
};

static bool IsHorizontalTaskbar(TaskbarEdge edge) {
    return edge == TaskbarEdge::Bottom || edge == TaskbarEdge::Top;
}

static TaskbarEdge DetectTaskbarEdge(const RECT& taskbarRect,
                                     const RECT& monitorRect) {
    const int width = taskbarRect.right - taskbarRect.left;
    const int height = taskbarRect.bottom - taskbarRect.top;
    if (width >= height) {
        const LONG centerY = taskbarRect.top + height / 2;
        const LONG monitorCenterY =
            monitorRect.top + (monitorRect.bottom - monitorRect.top) / 2;
        return centerY <= monitorCenterY ? TaskbarEdge::Top
                                         : TaskbarEdge::Bottom;
    }
    const LONG centerX = taskbarRect.left + width / 2;
    const LONG monitorCenterX =
        monitorRect.left + (monitorRect.right - monitorRect.left) / 2;
    return centerX <= monitorCenterX ? TaskbarEdge::Left
                                     : TaskbarEdge::Right;
}

static TaskbarGeometry GetTaskbarGeometry(
    HWND hTarget, HWND hTray, const MONITORINFO& mi,
    bool aimAtExpandedTaskbar = false) {
    TaskbarGeometry geometry;
    geometry.innerEdge = static_cast<float>(mi.rcMonitor.bottom);
    if (!hTray) return geometry;

    const TaskbarCoordinateTransform transform =
        BuildTaskbarCoordinateTransform(hTarget, hTray);
    RECT tr{};
    if (transform.valid) {
        tr = transform.animation;
    } else if (!GetWindowRect(hTray, &tr)) {
        return geometry;
    }

    const int width = tr.right - tr.left;
    const int height = tr.bottom - tr.top;
    if (width <= 0 || height <= 0) return geometry;

    geometry.valid = true;
    geometry.rect = tr;
    geometry.edge = DetectTaskbarEdge(tr, mi.rcMonitor);
    const bool horizontal = IsHorizontalTaskbar(geometry.edge);
    const int thickness = horizontal ? height : width;
    const int monitorThickness =
        horizontal ? mi.rcMonitor.bottom - mi.rcMonitor.top
                   : mi.rcMonitor.right - mi.rcMonitor.left;
    const int minThickness = ScaleTaskbarPx(hTarget, hTray, 24);
    const int edgeTolerance =
        std::max(1, ScaleTaskbarPx(hTarget, hTray, 2));

    if (thickness >= minThickness && thickness < monitorThickness / 2) {
        switch (geometry.edge) {
            case TaskbarEdge::Bottom:
                geometry.expanded =
                    tr.bottom >= mi.rcMonitor.bottom - edgeTolerance &&
                    tr.top <= mi.rcMonitor.bottom - minThickness;
                break;
            case TaskbarEdge::Top:
                geometry.expanded =
                    tr.top <= mi.rcMonitor.top + edgeTolerance &&
                    tr.bottom >= mi.rcMonitor.top + minThickness;
                break;
            case TaskbarEdge::Left:
                geometry.expanded =
                    tr.left <= mi.rcMonitor.left + edgeTolerance &&
                    tr.right >= mi.rcMonitor.left + minThickness;
                break;
            case TaskbarEdge::Right:
                geometry.expanded =
                    tr.right >= mi.rcMonitor.right - edgeTolerance &&
                    tr.left <= mi.rcMonitor.right - minThickness;
                break;
        }
    }

    switch (geometry.edge) {
        case TaskbarEdge::Bottom:
            geometry.innerEdge = geometry.expanded
                                     ? static_cast<float>(tr.top)
                                     : static_cast<float>(
                                           aimAtExpandedTaskbar
                                               ? mi.rcMonitor.bottom - thickness
                                               : mi.rcMonitor.bottom);
            break;
        case TaskbarEdge::Top:
            geometry.innerEdge = geometry.expanded
                                     ? static_cast<float>(tr.bottom)
                                     : static_cast<float>(
                                           aimAtExpandedTaskbar
                                               ? mi.rcMonitor.top + thickness
                                               : mi.rcMonitor.top);
            break;
        case TaskbarEdge::Left:
            geometry.innerEdge = geometry.expanded
                                     ? static_cast<float>(tr.right)
                                     : static_cast<float>(
                                           aimAtExpandedTaskbar
                                               ? mi.rcMonitor.left + thickness
                                               : mi.rcMonitor.left);
            break;
        case TaskbarEdge::Right:
            geometry.innerEdge = geometry.expanded
                                     ? static_cast<float>(tr.left)
                                     : static_cast<float>(
                                           aimAtExpandedTaskbar
                                               ? mi.rcMonitor.right - thickness
                                               : mi.rcMonitor.right);
            break;
    }
    return geometry;
}

static bool IsTaskbarExpanded(HWND hTarget, HWND hTray, HMONITOR hMon) {
    if (!hTray) return false;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!hMon) hMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
    return GetMonitorInfoW(hMon, &mi) &&
           GetTaskbarGeometry(hTarget, hTray, mi).expanded;
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
        if (GetTaskbarGeometry(hTarget, hTray, mi).expanded) return;
        Sleep(10);
    }
}

static POINT GetFallbackTaskbarButtonPosition(
    HWND hTarget, HWND hTray, const MONITORINFO& mi,
    const TaskbarGeometry& geometry, DWORD horizontalAlignment) {
    const int dpi = static_cast<int>(
        GetAnimationSpaceDpi(hTarget, hTray));
    const int leadingOffset = MulDiv(160, dpi, 96);
    const int inset = MulDiv(2, dpi, 96);
    const int horizontalButtonHeight = MulDiv(40, dpi, 96);
    const int verticalButtonWidth = horizontalButtonHeight;
    const int monitorCenterX =
        mi.rcMonitor.left +
        (mi.rcMonitor.right - mi.rcMonitor.left) / 2;

    POINT position{monitorCenterX, mi.rcMonitor.top + leadingOffset};
    switch (geometry.edge) {
        case TaskbarEdge::Bottom:
            position.x = horizontalAlignment == 0
                             ? mi.rcMonitor.left + leadingOffset
                             : monitorCenterX;
            position.y = static_cast<LONG>(geometry.innerEdge) + inset +
                         horizontalButtonHeight / 2;
            break;
        case TaskbarEdge::Top:
            position.x = horizontalAlignment == 0
                             ? mi.rcMonitor.left + leadingOffset
                             : monitorCenterX;
            position.y = static_cast<LONG>(geometry.innerEdge) - inset -
                         horizontalButtonHeight / 2;
            break;
        case TaskbarEdge::Left:
            position.x = static_cast<LONG>(geometry.innerEdge) - inset -
                         verticalButtonWidth / 2;
            break;
        case TaskbarEdge::Right:
            position.x = static_cast<LONG>(geometry.innerEdge) + inset +
                         verticalButtonWidth / 2;
            break;
    }
    position.x = Clamp(position.x, mi.rcMonitor.left, mi.rcMonitor.right);
    position.y = Clamp(position.y, mi.rcMonitor.top, mi.rcMonitor.bottom);
    return position;
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
    POINT targetPosition{};
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
    TaskbarEdge taskbarEdge;
    TaskbarDockWindowIdentity identity;
    POINT fallbackPosition;
    bool hasLearnedFallback;
    uint64_t generation;
    UiaPending* pending = nullptr;
};
static std::wstring MakeProcessDockKey(const std::wstring& procNameLower,
                                       HMONITOR hMon, TaskbarEdge edge) {
    if (procNameLower.empty()) return L"";
    return procNameLower + L"_" +
           std::to_wstring(reinterpret_cast<size_t>(hMon)) + L"_" +
           std::to_wstring(static_cast<uint32_t>(edge));
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
    if (procNameLower == L"windowsterminal") return L"terminal";
    return procNameLower;
}

struct TaskbarWindowSetSignatureContext {
    uint64_t xorHash = 0;
    uint64_t count = 0;
};

static void InvalidateTaskbarDockCachesLocked(
    bool preserveProcessFallback = false) {
    g_TaskbarDockPositions.clear();
    if (!preserveProcessFallback) {
        g_TaskbarDockFallbackPositions.clear();
        g_ProcessDockPositions.clear();
    }
    g_TaskbarDockLookupGenerations.clear();
    g_TaskbarDockLookupIdentities.clear();
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
            // The HWND result is stale after a reflow, but the process-level
            // value remains a much better timeout fallback than monitor
            // center. It is never hot-returned as a current result.
            InvalidateTaskbarDockCachesLocked(
                /*preserveProcessFallback=*/true);
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
        // generic edge fallback. Process entries are never hot-returned.
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
    POINT targetPosition = t->fallbackPosition;
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
                        RECT taskbarRect{};
                        bool haveTaskbarRect = taskbarTransform.valid;
                        if (haveTaskbarRect) {
                            taskbarRect = taskbarTransform.animation;
                        } else if (GetWindowRect(hTray, &taskbarRect)) {
                            haveTaskbarRect =
                                PhysicalRectToAnimationSpace(
                                    t->hWndApp, &taskbarRect);
                        }
                        const TaskbarEdge taskbarEdge = haveTaskbarRect
                            ? DetectTaskbarEdge(taskbarRect, mi.rcMonitor)
                            : TaskbarEdge::Bottom;
                        const bool horizontalTaskbar =
                            IsHorizontalTaskbar(taskbarEdge);
                        const int monRight = mi.rcMonitor.right;
                        const int monBottom = mi.rcMonitor.bottom;
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
                                                const bool beforeNotificationArea =
                                                    horizontalTaskbar
                                                        ? bRect.left < monRight - 50
                                                        : bRect.top < monBottom - 50;
                                                if (bRect.right > bRect.left &&
                                                    bRect.bottom > bRect.top &&
                                                    beforeNotificationArea) {
                                                    bestScore = score;
                                                    targetPosition = {
                                                        bRect.left +
                                                            (bRect.right - bRect.left) / 2,
                                                        bRect.top +
                                                            (bRect.bottom - bRect.top) / 2};
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
    const bool targetIdentityCurrent =
        IsTaskbarDockWindowIdentityCurrent(t->hWndApp, t->identity);
    if (uiaFound) {
        bool accepted = false;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto generationIt = g_TaskbarDockLookupGenerations.find(t->hWndApp);
            if (targetIdentityCurrent &&
                generationIt != g_TaskbarDockLookupGenerations.end() &&
                generationIt->second == t->generation) {
                const TaskbarDockCacheValue value{
                    targetPosition, t->taskbarEdge, t->identity,
                    GetTickCount()};
                g_TaskbarDockPositions[t->hWndApp] = value;
                g_TaskbarDockFallbackPositions[t->hWndApp] = value;
                if (!t->processKey.empty()) {
                    g_ProcessDockPositions[t->processKey] = targetPosition;
                }
                g_TaskbarDockLookupStartedTicks.erase(t->hWndApp);
                g_TaskbarDockNegativeUntilTicks.erase(t->hWndApp);
                g_TaskbarDockPositiveUntilTicks[t->hWndApp] =
                    GetTickCount() +
                    (t->identity.windowToken
                         ? AnimConstants::UiaPositiveFallbackCacheMs
                         : AnimConstants::UiaWeakIdentityCacheMs);
                if (pending) {
                    pending->targetPosition = targetPosition;
                    pending->found = true;
                }
                accepted = true;
            }
        }
        if (accepted && IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"UIA dock match hwnd=%p x=%ld y=%ld", t->hWndApp,
                   targetPosition.x, targetPosition.y);
        }
    } else {
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto generationIt = g_TaskbarDockLookupGenerations.find(t->hWndApp);
            if (targetIdentityCurrent &&
                generationIt != g_TaskbarDockLookupGenerations.end() &&
                generationIt->second == t->generation) {
                g_TaskbarDockLookupStartedTicks.erase(t->hWndApp);
                g_TaskbarDockPositions.erase(t->hWndApp);
                g_TaskbarDockPositiveUntilTicks.erase(t->hWndApp);
                g_TaskbarDockNegativeUntilTicks[t->hWndApp] =
                    GetTickCount() +
                    (t->identity.windowToken && t->hasLearnedFallback
                         ? AnimConstants::UiaNegativeCacheMs
                         : AnimConstants::UiaColdNegativeCacheMs);
            }
        }
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"UIA dock miss hwnd=%p fallback=(%ld,%ld)",
                   t->hWndApp, t->fallbackPosition.x,
                   t->fallbackPosition.y);
        }
    }
    return 0;
}
POINT GetTaskbarButtonPositionAsync(
    HWND hWndApp, const WCHAR* windowTitle, POINT fallbackPosition,
    HMONITOR hMon, TaskbarEdge taskbarEdge,
    DWORD waitMs = AnimConstants::UiaLookupWaitMs) {
    const bool sharedLayoutTracking = SyncTaskbarLayoutEpoch();
    // Win11's XAML taskbar doesn't reliably surface every button removal or
    // reflow through the HWND WinEvent bridge. Keep the shared epoch for
    // taskbar-only layout changes, but also verify the top-level app set before
    // trusting a hot per-window position. This intentionally runs on every
    // Genie/Windows 10 lookup so the first animation after a close is correct.
    RefreshTaskbarWindowSetSignature();
    const TaskbarDockWindowIdentity currentIdentity =
        CaptureTaskbarDockWindowIdentity(hWndApp, /*createToken=*/true);
    std::wstring procNameLower = GetProcessNameCached(hWndApp);
    std::wstring processKey =
        MakeProcessDockKey(procNameLower, hMon, taskbarEdge);
    POINT hwndCachedPosition{};
    POINT hwndFallbackPosition{};
    POINT processCachedPosition{};
    bool haveHwndCache = false;
    bool haveHwndFallback = false;
    bool haveProcessCache = false;
    bool lookupInFlight = false;
    bool negativeCacheActive = false;
    bool positiveFallbackCacheActive = false;
    uint64_t generation = 0;
    LONG layoutEpochAtRead = 0;
    const DWORD now = GetTickCount();
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto lookupIdentityIt = g_TaskbarDockLookupIdentities.find(hWndApp);
        const bool transientIdentityCurrent =
            lookupIdentityIt != g_TaskbarDockLookupIdentities.end() &&
            lookupIdentityIt->second.monitor == hMon &&
            TaskbarDockIdentityMatches(lookupIdentityIt->second,
                                       currentIdentity);
        if (!transientIdentityCurrent) {
            g_TaskbarDockLookupGenerations.erase(hWndApp);
            g_TaskbarDockLookupIdentities.erase(hWndApp);
            g_TaskbarDockLookupStartedTicks.erase(hWndApp);
            g_TaskbarDockNegativeUntilTicks.erase(hWndApp);
            g_TaskbarDockPositiveUntilTicks.erase(hWndApp);
            g_TaskbarDockPositions.erase(hWndApp);
        }
        auto it = g_TaskbarDockPositions.find(hWndApp);
        if (it != g_TaskbarDockPositions.end() &&
            IsTaskbarDockCacheValueUsable(it->second, currentIdentity, hMon,
                                          taskbarEdge, now)) {
            hwndCachedPosition = it->second.position;
            haveHwndCache = true;
        }
        auto fallbackIt = g_TaskbarDockFallbackPositions.find(hWndApp);
        if (fallbackIt != g_TaskbarDockFallbackPositions.end() &&
            IsTaskbarDockCacheValueUsable(fallbackIt->second,
                                          currentIdentity, hMon,
                                          taskbarEdge, now)) {
            hwndFallbackPosition = fallbackIt->second.position;
            haveHwndFallback = true;
        }
        if (!processKey.empty()) {
            auto pit = g_ProcessDockPositions.find(processKey);
            if (pit != g_ProcessDockPositions.end()) {
                processCachedPosition = pit->second;
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
    const bool strongWindowIdentity = currentIdentity.windowToken != 0;
    if (haveHwndCache && layoutEpochStillCurrent &&
        ((strongWindowIdentity && sharedLayoutTracking) ||
         positiveFallbackCacheActive)) {
        return hwndCachedPosition;
    }
    if (!layoutEpochStillCurrent) {
        SyncTaskbarLayoutEpoch();
        haveHwndCache = false;
        lookupInFlight = false;
        negativeCacheActive = false;
        positiveFallbackCacheActive = false;
        // Sync may have preserved the process fallback for a same-Explorer
        // reflow, or cleared all fallbacks for an Explorer restart. Reload the
        // actual post-sync state instead of retaining stale local copies.
        haveHwndFallback = false;
        haveProcessCache = false;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            auto fallbackIt = g_TaskbarDockFallbackPositions.find(hWndApp);
            if (fallbackIt != g_TaskbarDockFallbackPositions.end() &&
                IsTaskbarDockCacheValueUsable(fallbackIt->second,
                                              currentIdentity, hMon,
                                              taskbarEdge, now)) {
                hwndFallbackPosition = fallbackIt->second.position;
                haveHwndFallback = true;
            }
            if (!processKey.empty()) {
                auto processIt = g_ProcessDockPositions.find(processKey);
                if (processIt != g_ProcessDockPositions.end()) {
                    processCachedPosition = processIt->second;
                    haveProcessCache = true;
                }
            }
        }
    }
    const POINT softFallback = haveHwndCache ? hwndCachedPosition
                               : haveHwndFallback ? hwndFallbackPosition
                               : haveProcessCache ? processCachedPosition
                                                  : fallbackPosition;
    const bool hasLearnedFallback =
        haveHwndCache || haveHwndFallback || haveProcessCache;
    if (lookupInFlight || negativeCacheActive) return softFallback;

    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        generation = ++g_NextTaskbarDockLookupGeneration;
        if (!generation) generation = ++g_NextTaskbarDockLookupGeneration;
        g_TaskbarDockLookupGenerations[hWndApp] = generation;
        g_TaskbarDockLookupIdentities[hWndApp] = currentIdentity;
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
                GetTickCount() +
                (currentIdentity.windowToken && hasLearnedFallback
                     ? AnimConstants::UiaNegativeCacheMs
                     : AnimConstants::UiaColdNegativeCacheMs);
        }
    };
    auto* pending = new (std::nothrow) UiaPending{};
    if (!pending) {
        cancelLookup();
        return softFallback;
    }
    pending->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    pending->targetPosition = softFallback;
    if (!pending->done) {
        cancelLookup();
        pending->Release();
        pending->Release();
        return softFallback;
    }
    UiaTask* task = nullptr;
    try {
        task = new (std::nothrow) UiaTask{
            hWndApp, std::move(titleLower), std::move(procNameLower),
            procHintLower, std::move(processKey), hMon, taskbarEdge,
            currentIdentity, softFallback, hasLearnedFallback, generation,
            pending};
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
    POINT result = softFallback;
    if (waitMs > 0) {
        const DWORD wait = WaitForSingleObject(pending->done, waitMs);
        if (wait == WAIT_OBJECT_0 && pending->found) {
            result = pending->targetPosition;
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
static bool ConsumeAltTabIntentForForeground(HWND hWnd,
                                             DWORD eventTime) {
    if (ConsumeAltTabIntent(hWnd, eventTime)) return true;
    if (!IsShellExplorerProcess() ||
        !(GetAsyncKeyState(VK_MENU) & 0x8000) ||
        g_altTabSessionPollRunning.load(std::memory_order_relaxed)) {
        return false;
    }

    // File Explorer owns both the shell-wide Alt+Tab tracker and its local
    // destination hook. With heavily styled Explorer windows, the local hook
    // can win the callback race and look for the shared intent just before the
    // tracker publishes it. Seed that same session from the already remembered
    // source instead of dropping this switch animation.
    const HWND source =
        g_lastAppForeground.load(std::memory_order_relaxed);
    if (!source || source == hWnd || !IsAltTabSourceCandidate(source)) {
        return false;
    }
    BeginAltTabSession(source);
    StartAltTabSessionPoll();
    return ConsumeAltTabIntent(hWnd, eventTime);
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
                RefreshDwmChromeAfterUncloak(
                    h, /*nonBlocking=*/true, /*lightweight=*/true);
                FlushDwmOrYield();
            }
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                g_AnimActive.erase(h);
            }
            if (reinterpret_cast<ULONG_PTR>(
                    GetPropW(h, kPropSwitchAnimationActive)) == 1) {
                RemovePropW(h, kPropSwitchAnimationActive);
            }
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
    SetPropW(hGhost, kPropSwitchAnimationGhost, reinterpret_cast<HANDLE>(TRUE));
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
        RefreshDwmChromeAfterUncloak(
            hWnd, /*nonBlocking=*/true, /*lightweight=*/true);
        guard.uncloaked = true;
    }
    FlushDwmOrYield();
    if (hThumb) DwmUnregisterThumbnail(hThumb);
    DestroyWindow_Original(hGhost);
    return 0; 
}
void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD event, HWND hWnd,
                                  LONG idObject, LONG idChild, DWORD,
                                  DWORD dwmsEventTime) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (!g_switchAnimation.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) return;
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);
    if (windowPid != GetCurrentProcessId() || IsIconic(hWnd) || !IsAppMainWindow(hWnd, true)) return;
    if (!ConsumeAltTabIntentForForeground(hWnd, dwmsEventTime)) return;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (!g_AnimActive.insert(hWnd).second) return;
        g_AnimWantRising.erase(hWnd);
        g_AnimRestoreRequestForeground.erase(hWnd);
        g_AsyncRestoreReservations.erase(hWnd);
    }
    if (IsDiagnosticLoggingEnabled()) {
        Wh_Log(L"Switch animation start hwnd=%p", hWnd);
    }
    SetPropW(hWnd, kPropSwitchAnimationActive, reinterpret_cast<HANDLE>(1));
    SetWindowCloak(hWnd, TRUE);
    auto* data = new (std::nothrow)
        SwitchAnimData{hWnd, g_switchDurationMs.load(std::memory_order_relaxed)};
    if (!data || !StartWorkerThread(SwitchingAnimThread, data)) {
        Wh_Log(L"Switch animation worker failed hwnd=%p", hWnd);
        SetWindowCloak(hWnd, FALSE);
        RefreshDwmChromeAfterUncloak(
            hWnd, /*nonBlocking=*/true, /*lightweight=*/true);
        RemovePropW(hWnd, kPropSwitchAnimationActive);
        delete data;
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_AnimActive.erase(hWnd);
    }
}

void CALLBACK MinimizeEndVisibilityEventProc(HWINEVENTHOOK, DWORD event,
                                             HWND hWnd, LONG idObject,
                                             LONG idChild, DWORD, DWORD) {
    if (event != EVENT_SYSTEM_MINIMIZEEND || !hWnd ||
        idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
        g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    DWORD windowProcessId = 0;
    GetWindowThreadProcessId(hWnd, &windowProcessId);
    if (windowProcessId != GetCurrentProcessId()) return;

    // This callback executes in the restored window's injected process. It is
    // the same-process counterpart to Explorer's cross-process Win+D guard and
    // closes the gap left by frameworks that rebuild/decloak their DWM surface
    // after the shell's restore API has already returned.
    ReassertPreparedShowDesktopRestoreCloak(hWnd);
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
    // Force a fresh HWND/UIA result while retaining a non-authoritative
    // process fallback in case Explorer is still reflowing and the bounded
    // lookup doesn't finish in time.
    InvalidateTaskbarDockCachesLocked(/*preserveProcessFallback=*/true);
}

struct ShowDesktopCloakEndpointEnumContext {
    DWORD threadId{};
    bool publish{};
};

static BOOL CALLBACK UpdateShowDesktopCloakEndpointEnumProc(HWND hWnd,
                                                            LPARAM lParam) {
    auto* context = reinterpret_cast<ShowDesktopCloakEndpointEnumContext*>(
        lParam);
    if (!context || !context->threadId) return FALSE;

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) return TRUE;

    const HANDLE endpoint = reinterpret_cast<HANDLE>(
        static_cast<ULONG_PTR>(context->threadId));
    if (context->publish) {
        SetPropW(hWnd, kPropShowDesktopCloakEndpoint, endpoint);
    } else if (GetPropW(hWnd, kPropShowDesktopCloakEndpoint) == endpoint) {
        RemovePropW(hWnd, kPropShowDesktopCloakEndpoint);
    }
    return TRUE;
}

static void PublishShowDesktopCloakEndpointForWindow(HWND hWnd) {
    if (!hWnd) return;
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) return;

    const DWORD threadId =
        g_winEventThreadId.load(std::memory_order_acquire);
    if (threadId) {
        SetPropW(hWnd, kPropShowDesktopCloakEndpoint,
                 reinterpret_cast<HANDLE>(
                     static_cast<ULONG_PTR>(threadId)));
    }
}

DWORD WINAPI WinEventHookThread(LPVOID) {
    // Create the thread message queue before publishing its ID. Explorer can
    // then request a same-process cloak without synchronously entering the
    // application's UI thread during Win+D.
    MSG bootstrapMessage{};
    PeekMessageW(&bootstrapMessage, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    const DWORD endpointThreadId = GetCurrentThreadId();
    g_winEventThreadId.store(endpointThreadId, std::memory_order_release);
    ShowDesktopCloakEndpointEnumContext endpointContext{
        endpointThreadId, true};
    EnumWindows(UpdateShowDesktopCloakEndpointEnumProc,
                reinterpret_cast<LPARAM>(&endpointContext));

    const bool explorerProcess = IsShellExplorerProcess();
    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, ForegroundEventProc,
        GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    g_hForegroundHook.store(hook, std::memory_order_release);
    HWINEVENTHOOK minimizeEndHook = SetWinEventHook(
        EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND, nullptr,
        MinimizeEndVisibilityEventProc, GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT);

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

    if (!hook && !minimizeEndHook && !observeTaskbar) {
        endpointContext.publish = false;
        EnumWindows(UpdateShowDesktopCloakEndpointEnumProc,
                    reinterpret_cast<LPARAM>(&endpointContext));
        DWORD expectedThreadId = endpointThreadId;
        g_winEventThreadId.compare_exchange_strong(
            expectedThreadId, 0, std::memory_order_acq_rel);
        g_winEventThreadStarted.store(false, std::memory_order_relaxed);
        return 0;
    }
        
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == ANIM_PREPARE_SHOW_DESKTOP_RESTORE) {
            const HWND hWnd = reinterpret_cast<HWND>(msg.wParam);
            DWORD processId = 0;
            if (hWnd) GetWindowThreadProcessId(hWnd, &processId);
            if (hWnd && processId == GetCurrentProcessId() &&
                IsShowDesktopRestorePrepared(hWnd)) {
                ReassertPreparedShowDesktopRestoreCloak(hWnd);
                const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
                    GetPropW(hWnd, kPropShowDesktopRestorePrepared));
                if (token &&
                    reinterpret_cast<ULONG_PTR>(GetPropW(
                        hWnd, kPropShowDesktopAnimationOwner)) == token &&
                    reinterpret_cast<ULONG_PTR>(GetPropW(
                        hWnd, kPropShowDesktopLocalCloakWatch)) == token) {
                    // Acknowledge only after the target process has submitted
                    // its own cloak to DWM. Explorer waits for this property
                    // before allowing the native restore to expose a surface.
                    FlushDwmOrYield();
                    SetPropW(hWnd, kPropShowDesktopCloakReady,
                             reinterpret_cast<HANDLE>(token));
                }
            }
            continue;
        }
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
    if (minimizeEndHook) UnhookWinEvent(minimizeEndHook);
    removeTaskbarHooks();
    endpointContext.publish = false;
    EnumWindows(UpdateShowDesktopCloakEndpointEnumProc,
                reinterpret_cast<LPARAM>(&endpointContext));
    DWORD expectedThreadId = endpointThreadId;
    g_winEventThreadId.compare_exchange_strong(
        expectedThreadId, 0, std::memory_order_acq_rel);
    return 0;
}

struct QuadFrame {
    int x{};
    int y{};
    int width{};
    int height{};
    float opacity{1.0f};
};

struct alignas(16) GpuQuadConstants {
    float outputSize[2];
    float rectOrigin[2];
    float rectSize[2];
    float opacity;
    float padding;
    float sourceSize[2];
    float padding2[2];
};

struct alignas(16) GpuGenieConstants {
    float outputSize[2];
    float sourceSize[2];
    float sourceOrigin[2];
    float dockPosition[2];
    float neckWidth;
    float morphTime;
    float opacity;
    float spread;
    uint32_t taskbarEdge;
    float padding[3];
};

struct alignas(16) GpuInplaceConstants {
    float outputSize[2];
    float sourceSize[2];
    float rectOrigin[2];
    float progress;
    float blockSize;
    uint32_t effectStyle;
    float fieldSize[2];
    float padding;
};

struct alignas(16) GpuFieldConstants {
    float sourceSize[2];
    float blockSize;
    float fieldStep;
    uint32_t effectStyle;
    float padding[3];
};

struct alignas(16) GpuCloseConstants {
    float outputSize[2];
    float sourceSize[2];
    float sourceOrigin[2];
    float progress;
    float blockSize;
};

struct alignas(16) GpuShatterConstants {
    float outputSize[2];
    float sourceSize[2];
    float sourceOrigin[2];
    float progress;
    uint32_t blockSize;
    float thanosBaseStartMax;
    float thanosWaveNoiseMult;
    float thanosLifeSpan;
    float padding;
};

struct GpuGenieFrame {
    float sourceOriginX{};
    float sourceOriginY{};
    float dockX{};
    float dockY{};
    float neckWidth{};
    float morphTime{};
    float opacity{1.0f};
    TaskbarEdge taskbarEdge{TaskbarEdge::Bottom};
};

struct GpuInplaceFrame {
    float rectOriginX{};
    float rectOriginY{};
    float progress{};
    int blockSize{1};
    uint32_t effectStyle{};
};

struct GpuCloseFrame {
    float sourceOriginX{};
    float sourceOriginY{};
    float progress{};
    float blockSize{};
};

static_assert(sizeof(GpuQuadConstants) % 16 == 0);
static_assert(sizeof(GpuGenieConstants) % 16 == 0);
static_assert(sizeof(GpuInplaceConstants) % 16 == 0);
static_assert(sizeof(GpuFieldConstants) % 16 == 0);
static_assert(sizeof(GpuCloseConstants) % 16 == 0);
static_assert(sizeof(GpuShatterConstants) % 16 == 0);

using D3DCompile_t = HRESULT(WINAPI*)(
    LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR,
    LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**);

// Keep the small effect shaders beside the matching CPU frame calculations.
// The compiler DLL is loaded lazily from System32; if it is unavailable, the
// existing layered-window renderer remains the automatic fallback.
static constexpr char kGpuQuadShader[] = R"hlsl(
cbuffer FrameConstants : register(b0)
{
    float2 outputSize;
    float2 rectOrigin;
    float2 rectSize;
    float opacity;
    float padding;
    float2 sourceSize;
    float2 padding2;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    float2 uv = float2(vertexId & 1, (vertexId >> 1) & 1);
    float2 pixelPosition = rectOrigin + uv * rectSize;
    output.position = float4(
        pixelPosition.x * (2.0f / outputSize.x) - 1.0f,
        1.0f - pixelPosition.y * (2.0f / outputSize.y),
        0.0f, 1.0f);
    output.uv = uv;
    return output;
}

Texture2D sourceTexture : register(t0);

float4 PSMain(VertexOutput input) : SV_TARGET
{
    int2 destinationPixel = int2(floor(input.position.xy - rectOrigin));
    int2 sourcePixel = int2(floor(
        float2(destinationPixel) * sourceSize / rectSize));
    sourcePixel = clamp(sourcePixel, int2(0, 0), int2(sourceSize) - 1);
    return sourceTexture.Load(int3(sourcePixel, 0)) * opacity;
}
)hlsl";

// Genie is a dense two-vertex strip along the axis perpendicular to the
// taskbar. Horizontal bars bend rows; vertical bars bend columns. The pixel
// shader reconstructs the inverse mapping on the other axis, reducing
// per-frame CPU work to one constant-buffer update for every orientation.
static constexpr char kGpuGenieShader[] = R"hlsl(
cbuffer GenieConstants : register(b0)
{
    float2 outputSize;
    float2 sourceSize;
    float2 sourceOrigin;
    float2 dockPosition;
    float neckWidth;
    float morphTime;
    float opacity;
    float spread;
    uint taskbarEdge;
    float3 padding;
};

float MorphAt(float edgeCoordinate)
{
    float m = saturate(
        morphTime * (1.0f + spread) -
        (1.0f - edgeCoordinate) * spread);
    return m * m * (3.0f - 2.0f * m);
}

struct VertexOutput
{
    float4 position : SV_POSITION;
    float sourceAxis : TEXCOORD0;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    bool verticalTaskbar = taskbarEdge >= 2u;
    uint sourceStep = vertexId >> 1;
    float side = float(vertexId & 1);
    float sourceAxisSize = verticalTaskbar ? sourceSize.x : sourceSize.y;
    float axis = min(float(sourceStep) / sourceAxisSize, 1.0f);
    bool leadingEdge = taskbarEdge == 1u || taskbarEdge == 2u;
    float edgeCoordinate = leadingEdge ? 1.0f - axis : axis;
    float morph = MorphAt(edgeCoordinate);
    float2 outputPixel;
    if (verticalTaskbar) {
        float sourceX = sourceOrigin.x + sourceSize.x * axis;
        outputPixel.x = lerp(sourceX, dockPosition.x, morph);
        float height = max(1.0f, lerp(sourceSize.y, neckWidth, morph));
        float sourceCenterY = sourceOrigin.y + sourceSize.y * 0.5f;
        float centerY = lerp(sourceCenterY, dockPosition.y, morph);
        outputPixel.y = centerY + (side - 0.5f) * height;
    } else {
        float sourceY = sourceOrigin.y + sourceSize.y * axis;
        outputPixel.y = lerp(sourceY, dockPosition.y, morph);
        float width = max(1.0f, lerp(sourceSize.x, neckWidth, morph));
        float sourceCenterX = sourceOrigin.x + sourceSize.x * 0.5f;
        float centerX = lerp(sourceCenterX, dockPosition.x, morph);
        outputPixel.x = centerX + (side - 0.5f) * width;
    }
    output.position = float4(
        outputPixel.x * (2.0f / outputSize.x) - 1.0f,
        1.0f - outputPixel.y * (2.0f / outputSize.y),
        0.0f, 1.0f);
    output.sourceAxis = axis;
    return output;
}

Texture2D sourceTexture : register(t0);

float4 PSMain(VertexOutput input) : SV_TARGET
{
    bool verticalTaskbar = taskbarEdge >= 2u;
    float axis = saturate(input.sourceAxis);
    bool leadingEdge = taskbarEdge == 1u || taskbarEdge == 2u;
    float edgeCoordinate = leadingEdge ? 1.0f - axis : axis;
    float morph = MorphAt(edgeCoordinate);
    float u;
    float v;
    if (verticalTaskbar) {
        float height = max(1.0f, lerp(sourceSize.y, neckWidth, morph));
        float sourceCenterY = sourceOrigin.y + sourceSize.y * 0.5f;
        float centerY = lerp(sourceCenterY, dockPosition.y, morph);
        float top = centerY - height * 0.5f;
        v = (input.position.y - top) / height;
        u = axis;
        if (v < 0.0f || v >= 1.0f) discard;
    } else {
        float width = max(1.0f, lerp(sourceSize.x, neckWidth, morph));
        float sourceCenterX = sourceOrigin.x + sourceSize.x * 0.5f;
        float centerX = lerp(sourceCenterX, dockPosition.x, morph);
        float left = centerX - width * 0.5f;
        u = (input.position.x - left) / width;
        v = axis;
        if (u < 0.0f || u >= 1.0f) discard;
    }
    int2 sourcePixel = int2(floor(float2(u, v) * sourceSize));
    sourcePixel = clamp(sourcePixel, int2(0, 0), int2(sourceSize) - 1);
    return sourceTexture.Load(int3(sourcePixel, 0)) * opacity;
}
)hlsl";

// The five in-place effects share a full-window pass. Ink Splash, Scorch and
// Splinter read a low-resolution field generated once by kGpuFieldShader;
// Mirage and Stipple calculate their small masks directly. Quantizing all math
// to the CPU renderer's dynamic block size preserves the existing character.
static constexpr char kGpuInplaceShader[] = R"hlsl(
cbuffer InplaceConstants : register(b0)
{
    float2 outputSize;
    float2 sourceSize;
    float2 rectOrigin;
    float progress;
    float blockSizeValue;
    uint effectStyle;
    float2 fieldSize;
    float padding;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    float2 uv = float2(vertexId & 1, (vertexId >> 1) & 1);
    float2 pixelPosition = rectOrigin + uv * sourceSize;
    output.position = float4(
        pixelPosition.x * (2.0f / outputSize.x) - 1.0f,
        1.0f - pixelPosition.y * (2.0f / outputSize.y),
        0.0f, 1.0f);
    return output;
}

Texture2D sourceTexture : register(t0);
Texture2D<float4> effectField : register(t1);

float4 PSMain(VertexOutput input) : SV_TARGET
{
    int blockSize = max(1, int(blockSizeValue));
    int2 sourceDimensions = max(int2(sourceSize), int2(1, 1));
    int2 destinationPixel =
        int2(floor(input.position.xy - rectOrigin));
    destinationPixel = clamp(
        destinationPixel, int2(0, 0), sourceDimensions - 1);
    int2 blockOrigin = (destinationPixel / blockSize) * blockSize;
    int2 blockExtent = min(
        int2(blockSize, blockSize), sourceDimensions - blockOrigin);
    int2 inBlock = destinationPixel - blockOrigin;
    float2 blockCenter =
        float2(blockOrigin) + float2(blockExtent) * 0.5f;
    float2 uv = blockCenter / sourceSize;

    if (effectStyle <= 3)
    {
        int2 fieldDimensions = max(int2(fieldSize), int2(1, 1));
        int2 fieldPixel = clamp(
            blockOrigin / blockSize, int2(0, 0), fieldDimensions - 1);
        float4 parameters = effectField.Load(int3(fieldPixel, 0));
        float reveal = 0.0f;

        if (effectStyle == 1)
        {
            float boundary = progress * 1.7f - 0.15f;
            float t = saturate(
                (parameters.x - boundary - 0.04f) / -0.08f);
            reveal = t * t * (3.0f - 2.0f * t);
        }
        else if (effectStyle == 2)
        {
            float distanceFromCorner =
                length(1.0f - uv) * 1.55f -
                progress * parameters.x;
            float radius = progress - parameters.y;
            reveal = distanceFromCorner <= radius
                         ? 1.0f
                         : progress * progress * progress * progress;
        }
        else
        {
            const float fadeEdge = 0.08f;
            float outer = smoothstep(0.0f, fadeEdge, progress);
            float endFill = smoothstep(
                1.0f - fadeEdge, 1.0f, progress);
            float hard = parameters.y <= progress - parameters.x
                             ? 1.0f
                             : 0.0f;
            reveal = outer * lerp(endFill, 1.0f, hard);
        }

        if (reveal <= 0.01f) discard;
        return sourceTexture.Load(int3(destinationPixel, 0)) * reveal;
    }

    if (effectStyle == 4)
    {
        if (progress <= 0.01f) discard;
        float inverseProgress = 1.0f - progress;
        float2 displacedUv = uv + inverseProgress * float2(
            0.035f * cos(62.0f * uv.x),
            0.035f * sin(62.0f * uv.y));
        int2 displacedOrigin = int2(
            displacedUv * sourceSize - float2(blockExtent) * 0.5f);
        displacedOrigin = clamp(
            displacedOrigin, int2(0, 0),
            sourceDimensions - blockExtent);
        int2 sourcePixel = displacedOrigin + inBlock;
        return sourceTexture.Load(int3(sourcePixel, 0)) * progress;
    }

    // Stipple reveals the original block whenever its repeating dot cell is
    // inside the radial threshold used by the CPU effect.
    float2 cell = frac(uv * 28.0f) - 0.5f;
    float cellDistance = length(cell);
    float originDistance = max(length(uv), 0.0001f);
    if (cellDistance > progress / originDistance) discard;
    return sourceTexture.Load(int3(destinationPixel, 0));
}
)hlsl";

// Ink Splash, Scorch and Splinter used to build their block fields on the CPU.
// This one-time low-resolution render reproduces those parameters on the GPU;
// the per-frame shader above then only performs a texture load and mask test.
static constexpr char kGpuFieldShader[] = R"hlsl(
cbuffer FieldConstants : register(b0)
{
    float2 sourceSize;
    float blockSizeValue;
    float fieldStep;
    uint effectStyle;
    float3 padding;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    float2 uv = float2(vertexId & 1, (vertexId >> 1) & 1);
    output.position = float4(
        uv.x * 2.0f - 1.0f, 1.0f - uv.y * 2.0f, 0.0f, 1.0f);
    return output;
}

float InkHash(float2 p)
{
    return frac(sin(dot(p, float2(127.1f, 311.7f))) * 43758.5453f);
}

float PatternHash(float2 p)
{
    return frac(sin(dot(p, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float ValueNoise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    f = f * f * (3.0f - 2.0f * f);
    float a = InkHash(i);
    float b = InkHash(i + float2(1.0f, 0.0f));
    float c = InkHash(i + float2(0.0f, 1.0f));
    float d = InkHash(i + float2(1.0f, 1.0f));
    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}

float Fbm(float2 p)
{
    float value = 0.0f;
    float amplitude = 0.5f;
    [unroll]
    for (int octave = 0; octave < 4; ++octave)
    {
        value += amplitude * ValueNoise(p);
        p *= 2.1f;
        amplitude *= 0.5f;
    }
    return value;
}

float InkDistance(int2 fieldPixel)
{
    float2 uv = (float2(fieldPixel) + 0.5f) * fieldStep / sourceSize;
    float blob = Fbm(uv * 3.5f);
    float fingers = Fbm(uv * 14.0f);
    float distortion =
        (blob - 0.5f) * 0.5f + (fingers - 0.5f) * 0.18f;
    float2 centered = uv - 0.5f;
    centered.x *= sourceSize.x / max(sourceSize.y, 1.0f);
    return length(centered) + distortion;
}

float Mod289(float x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

float Permute(float x)
{
    return Mod289(((x * 34.0f) + 1.0f) * x);
}

float SimplexContribution(float q, float2 p)
{
    float m = 0.5f - dot(p, p);
    if (m < 0.0f) return 0.0f;
    m *= m;
    m *= m;
    float x = 2.0f * frac(q * 0.024390243902439f) - 1.0f;
    float h = abs(x) - 0.5f;
    float ox = floor(x + 0.5f);
    float a0 = x - ox;
    m *= 1.79284291400159f -
         0.85373472095314f * (a0 * a0 + h * h);
    return m * dot(float2(a0, h), p);
}

float SimplexNoise(float2 v)
{
    const float C0 = 0.211324865405187f;
    const float C1 = 0.366025403784439f;
    const float C2 = -0.577350269189626f;
    float2 i = floor(v + (v.x + v.y) * C1);
    float2 x0 = v - i + (i.x + i.y) * C0;
    float2 i1 = x0.x > x0.y ? float2(1.0f, 0.0f)
                              : float2(0.0f, 1.0f);
    float2 x1 = x0 + C0 - i1;
    float2 x2 = x0 + C2;
    i = float2(Mod289(i.x), Mod289(i.y));
    float q0 = Permute(Permute(i.y) + i.x);
    float q1 = Permute(Permute(i.y + i1.y) + i.x + i1.x);
    float q2 = Permute(Permute(i.y + 1.0f) + i.x + 1.0f);
    return 130.0f * (
        SimplexContribution(q0, x0) +
        SimplexContribution(q1, x1) +
        SimplexContribution(q2, x2));
}

float PerlinHash(int x, int y)
{
    uint hash = uint(x) * 73856093u ^ uint(y) * 19349663u;
    return float(hash % 10000u) / 10000.0f;
}

float PerlinValueNoise(float2 position)
{
    int2 cell = int2(floor(position));
    float2 fraction = position - float2(cell);
    float2 blend = fraction * fraction * (3.0f - 2.0f * fraction);
    float a = PerlinHash(cell.x, cell.y);
    float b = PerlinHash(cell.x + 1, cell.y);
    float c = PerlinHash(cell.x, cell.y + 1);
    float d = PerlinHash(cell.x + 1, cell.y + 1);
    return lerp(lerp(a, b, blend.x), lerp(c, d, blend.x), blend.y);
}

float4 PSMain(VertexOutput input) : SV_TARGET
{
    int blockSize = max(1, int(blockSizeValue));
    int2 sourceDimensions = max(int2(sourceSize), int2(1, 1));
    int2 fieldPixel = int2(floor(input.position.xy));
    int2 blockOrigin = fieldPixel * blockSize;
    if (effectStyle == 8)
    {
        float2 noisePosition = float2(blockOrigin) / 150.0f;
        float noiseValue = 0.0f;
        float amplitude = 0.5f;
        [unroll]
        for (int octave = 0; octave < 3; ++octave)
        {
            noiseValue += amplitude * PerlinValueNoise(noisePosition);
            noisePosition *= 2.0f;
            amplitude *= 0.5f;
        }
        return float4(noiseValue * 0.9f, 0.0f, 0.0f, 0.0f);
    }
    int2 blockExtent = min(
        int2(blockSize, blockSize), sourceDimensions - blockOrigin);
    float2 blockCenter =
        float2(blockOrigin) + float2(blockExtent) * 0.5f;
    float2 uv = blockCenter / sourceSize;

    if (effectStyle == 1)
    {
        float2 f = blockCenter / fieldStep - 0.5f;
        int2 base = int2(floor(f));
        float2 t = frac(f);
        int2 coarseSize = max(
            int2(ceil(sourceSize / fieldStep)), int2(2, 2));
        base = clamp(base, int2(0, 0), coarseSize - 2);
        float a = InkDistance(base);
        float b = InkDistance(base + int2(1, 0));
        float c = InkDistance(base + int2(0, 1));
        float d = InkDistance(base + int2(1, 1));
        float splashDistance = lerp(
            lerp(a, b, t.x), lerp(c, d, t.x), t.y);
        return float4(splashDistance, 0.0f, 0.0f, 0.0f);
    }

    if (effectStyle == 2)
    {
        float n = SimplexNoise(float2(uv.x * 2.8f, 0.0f));
        float force = 0.42f + 0.58f * exp(n * 0.55f);
        float randomDelay =
            PatternHash(float2(uv.x * 2.8f, 0.1f)) * 0.42f;
        // The consumer reconstructs uv from the block position, so only keep
        // the two values which cannot be derived there.
        return float4(force, randomDelay, 0.0f, 0.0f);
    }

    float distanceFromCenter = length(uv - 0.5f) / 3.2f;
    float randomY = PatternHash(float2(uv.y * 2.6f, 0.0f));
    float randomX = PatternHash(float2(0.0f, uv.x * 2.6f));
    float jitter = min(randomY, randomX) * 0.38f;
    return float4(jitter, distanceFromCenter, 0.0f, 0.0f);
}
)hlsl";

// Perlin Dissolve is the only full-canvas close effect selected by the
// hybrid GPU router. Its one-value-per-pixel field and source sampling remain
// entirely on the GPU without carrying the discarded forced-GPU prototype
// paths.
static constexpr char kGpuCloseShader[] = R"hlsl(
cbuffer CloseConstants : register(b0)
{
    float2 outputSize;
    float2 sourceSize;
    float2 sourceOrigin;
    float progress;
    float blockSize;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    float2 uv = float2(vertexId & 1, (vertexId >> 1) & 1);
    output.position = float4(
        uv.x * 2.0f - 1.0f, 1.0f - uv.y * 2.0f, 0.0f, 1.0f);
    return output;
}

Texture2D sourceTexture : register(t0);
Texture2D effectFieldTexture : register(t1);

float4 PSMain(VertexOutput input) : SV_TARGET
{
    int2 outputPixel = int2(floor(input.position.xy));
    int2 origin = int2(sourceOrigin);
    int2 sourceDimensions = int2(sourceSize);
    int2 sourcePixel = outputPixel - origin;
    if (sourcePixel.x < 0 || sourcePixel.x >= sourceDimensions.x ||
        sourcePixel.y < 0 || sourcePixel.y >= sourceDimensions.y) discard;

    int step = max(1, int(blockSize + 0.5f));
    int2 fieldPixel = int2(floor(float2(sourcePixel) / float(step)));
    float startTime = effectFieldTexture.Load(int3(fieldPixel, 0)).r;
    float localProgress = (progress - startTime) / 0.15f;
    if (localProgress >= 1.0f) discard;
    float opacity = localProgress <= 0.0f ? 1.0f : 1.0f - localProgress;
    return sourceTexture.Load(int3(sourcePixel, 0)) * opacity;
}
)hlsl";
// Dense Thanos is the only forward-scatter close effect selected by the
// hybrid GPU router. Draw each source pixel as a point directly into the
// presenter, avoiding both per-particle CPU storage and a second GPU surface.
static constexpr char kGpuShatterShader[] = R"hlsl(
cbuffer ShatterConstants : register(b0)
{
    float2 outputSize;
    float2 sourceSize;
    float2 sourceOrigin;
    float progress;
    uint blockSize;
    float thanosBaseStartMax;
    float thanosWaveNoiseMult;
    float thanosLifeSpan;
    float padding;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    nointerpolation float2 sourceBlockOrigin : TEXCOORD0;
    nointerpolation float2 destinationBlockOrigin : TEXCOORD1;
    nointerpolation float opacity : TEXCOORD2;
};

VertexOutput VSMain(uint vertexId : SV_VertexID,
                    uint instanceId : SV_InstanceID)
{
    VertexOutput output;
    uint size = max(1u, blockSize);
    uint2 sourceDimensions = uint2(sourceSize);
    uint blockColumns = (sourceDimensions.x + size - 1u) / size;
    uint2 blockOrigin = uint2(
        (instanceId % blockColumns) * size,
        (instanceId / blockColumns) * size);

    uint hash = blockOrigin.x * 73856093u ^
                blockOrigin.y * 19349663u;
    float clampedProgress = saturate(progress);
    float2 displacement = 0.0f;
    float opacity = 1.0f;
    float distanceFromBottomRight =
        (sourceSize.x - float(blockOrigin.x)) +
        (sourceSize.y - float(blockOrigin.y));
    float startTime = distanceFromBottomRight /
                      (sourceSize.x + sourceSize.y) *
                      thanosBaseStartMax;
    startTime +=
        (float(hash % 100u) / 100.0f - 0.5f) *
        thanosWaveNoiseMult;
    startTime = clamp(startTime, 0.0f, 0.65f);
    float localProgress =
        (clampedProgress - startTime) / thanosLifeSpan;
    if (localProgress >= 1.0f) {
        opacity = 0.0f;
    } else if (localProgress > 0.0f) {
        float noiseX =
            float((hash >> 4) % 200u) / 100.0f - 1.0f;
        float noiseY =
            float((hash >> 8) % 200u) / 100.0f - 1.0f;
        float2 wind = float2(sourceSize.x * 0.5f,
                             sourceSize.y * 0.2f);
        float swirl =
            (float(blockOrigin.y) / sourceSize.y - 0.5f) * 2.0f;
        wind.y += swirl * sourceSize.y * 0.3f;
        wind += float2(noiseX * sourceSize.x * 0.2f,
                       noiseY * sourceSize.y * 0.3f);
        float2 curve = float2(noiseY * sourceSize.x * 0.4f,
                              -noiseX * sourceSize.y * 0.4f);
        displacement = wind * localProgress +
                       curve * localProgress * localProgress;
        opacity = 1.0f - localProgress;
        if (opacity >= 0.99f) opacity = 1.0f;
    }

    int2 destinationOrigin = int2(sourceOrigin) + int2(blockOrigin) +
                             int2(displacement);
    uint2 extent = min(uint2(size, size), sourceDimensions - blockOrigin);
    float2 corner = float2(vertexId & 1u, (vertexId >> 1u) & 1u);
    float2 destination = float2(destinationOrigin) + corner * float2(extent);
    output.position = float4(
        destination.x * (2.0f / outputSize.x) - 1.0f,
        1.0f - destination.y * (2.0f / outputSize.y),
        0.0f, 1.0f);
    output.sourceBlockOrigin = float2(blockOrigin);
    output.destinationBlockOrigin = float2(destinationOrigin);
    output.opacity = opacity;
    return output;
}

VertexOutput VSPointMain(uint vertexId : SV_VertexID)
{
    // At blockSize=1, one point primitive replaces the four-vertex quad for
    // each source pixel while preserving the same hash, motion, and opacity.
    VertexOutput output = VSMain(0u, vertexId);
    output.position.xy += float2(1.0f / outputSize.x,
                                 -1.0f / outputSize.y);
    return output;
}

Texture2D<float4> sourceTexture : register(t0);

float4 PSMain(VertexOutput input) : SV_TARGET
{
    clip(input.opacity - 0.0001f);
    int2 blockPixel = int2(floor(
        input.position.xy - input.destinationBlockOrigin));
    int2 sourcePixel = int2(input.sourceBlockOrigin) + blockPixel;
    return sourceTexture.Load(int3(sourcePixel, 0)) * input.opacity;
}
)hlsl";
enum GpuPipeline : uint32_t {
    GpuPipelineNone = 0,
    GpuPipelineQuad = 1u << 0,
    GpuPipelineGenie = 1u << 1,
    GpuPipelineInplace = 1u << 2,
    GpuPipelineField = 1u << 3,
    GpuPipelineClose = 1u << 4,
    GpuPipelineShatter = 1u << 5,
};

static constexpr uint32_t RequiredGpuPipelines(bool isClosing,
                                                int effectStyle) {
    if (isClosing) {
        if (effectStyle == 1) {
            return GpuPipelineQuad | GpuPipelineShatter;
        }
        if (effectStyle == 2) {
            return GpuPipelineQuad | GpuPipelineField | GpuPipelineClose;
        }
        return GpuPipelineNone;
    }
    if (effectStyle == 0) return GpuPipelineGenie;
    if (effectStyle >= 1 && effectStyle <= 3) {
        return GpuPipelineInplace | GpuPipelineField;
    }
    if (effectStyle == 4 || effectStyle == 5) {
        return GpuPipelineInplace;
    }
    if (effectStyle == 6 || effectStyle == 7) {
        return GpuPipelineQuad;
    }
    return GpuPipelineNone;
}

static_assert(RequiredGpuPipelines(false, 0) == GpuPipelineGenie);
static_assert(RequiredGpuPipelines(false, 1) ==
              (GpuPipelineInplace | GpuPipelineField));
static_assert(RequiredGpuPipelines(false, 6) == GpuPipelineQuad);
static_assert(RequiredGpuPipelines(true, 2) ==
              (GpuPipelineQuad | GpuPipelineField | GpuPipelineClose));
static_assert(RequiredGpuPipelines(true, 1) ==
              (GpuPipelineQuad | GpuPipelineShatter));
static_assert(RequiredGpuPipelines(true, 0) == GpuPipelineNone);
static_assert(RequiredGpuPipelines(true, 3) == GpuPipelineNone);

// Layered GPU presentation has a fixed synchronization/readback cost. The
// matched benchmark only found a repeatable win once Thanos or Perlin reached
// a genuinely dense one-value-per-pixel field on a large source. Keep this
// threshold conservative instead of guessing from the user's GPU model.
static constexpr uint64_t kHybridCloseGpuMinSourcePixels = 1500000ull;
static constexpr bool IsPotentialHybridGpuClose(int effectStyle,
                                                int blockSize) {
    return (effectStyle == 1 || effectStyle == 2) && blockSize == 1;
}
static constexpr bool ShouldUseHybridGpuClose(int effectStyle, int blockSize,
                                              int sourceWidth,
                                              int sourceHeight) {
    if (!IsPotentialHybridGpuClose(effectStyle, blockSize) ||
        sourceWidth <= 0 || sourceHeight <= 0) {
        return false;
    }
    const uint64_t sourcePixels = static_cast<uint64_t>(sourceWidth) *
                                  static_cast<uint64_t>(sourceHeight);
    return sourcePixels >= kHybridCloseGpuMinSourcePixels;
}
static_assert(ShouldUseHybridGpuClose(1, 1, 1920, 1200));
static_assert(ShouldUseHybridGpuClose(2, 1, 1920, 1200));
static_assert(!ShouldUseHybridGpuClose(0, 1, 1920, 1200));
static_assert(!ShouldUseHybridGpuClose(1, 5, 1920, 1200));
static_assert(!ShouldUseHybridGpuClose(2, 1, 1200, 1000));

struct GpuRenderService {
    std::mutex mutex;
    std::atomic<bool> ready{false};
    std::atomic<uint32_t> readyPipelines{GpuPipelineNone};
    uint32_t activePresenters{};
    DWORD retryAfterTick{};
    uint64_t generation{1};
    HMODULE compilerModule{};
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGIDevice1> dxgiDevice;
    ComPtr<IDXGIFactory2> factory;
    ComPtr<IDCompositionDevice> compositionDevice;
    std::wstring adapterDescription;
    uint64_t adapterDedicatedVideoMemory{};
    UINT adapterVendorId{};
    UINT adapterDeviceId{};
    ComPtr<ID3D11VertexShader> quadVertexShader;
    ComPtr<ID3D11PixelShader> quadPixelShader;
    ComPtr<ID3D11Buffer> quadConstants;
    ComPtr<ID3D11VertexShader> genieVertexShader;
    ComPtr<ID3D11PixelShader> geniePixelShader;
    ComPtr<ID3D11Buffer> genieConstants;
    ComPtr<ID3D11VertexShader> inplaceVertexShader;
    ComPtr<ID3D11PixelShader> inplacePixelShader;
    ComPtr<ID3D11Buffer> inplaceConstants;
    ComPtr<ID3D11VertexShader> fieldVertexShader;
    ComPtr<ID3D11PixelShader> fieldPixelShader;
    ComPtr<ID3D11Buffer> fieldConstants;
    ComPtr<ID3D11VertexShader> closeVertexShader;
    ComPtr<ID3D11PixelShader> closePixelShader;
    ComPtr<ID3D11Buffer> closeConstants;
    ComPtr<ID3D11VertexShader> shatterVertexShader;
    ComPtr<ID3D11VertexShader> shatterPointVertexShader;
    ComPtr<ID3D11PixelShader> shatterPixelShader;
    ComPtr<ID3D11Buffer> shatterConstants;
    ComPtr<ID3D11BlendState> blendState;
    ComPtr<ID3D11RasterizerState> rasterizerState;
};

static GpuRenderService g_gpuRenderService;

static void ReleaseGpuDeviceResourcesLocked() {
    auto& service = g_gpuRenderService;
    if (service.context) {
        service.context->ClearState();
        service.context->Flush();
    }
    service.rasterizerState.Reset();
    service.blendState.Reset();
    service.shatterConstants.Reset();
    service.shatterPixelShader.Reset();
    service.shatterPointVertexShader.Reset();
    service.shatterVertexShader.Reset();
    service.closeConstants.Reset();
    service.closePixelShader.Reset();
    service.closeVertexShader.Reset();
    service.fieldConstants.Reset();
    service.fieldPixelShader.Reset();
    service.fieldVertexShader.Reset();
    service.inplaceConstants.Reset();
    service.inplacePixelShader.Reset();
    service.inplaceVertexShader.Reset();
    service.genieConstants.Reset();
    service.geniePixelShader.Reset();
    service.genieVertexShader.Reset();
    service.quadConstants.Reset();
    service.quadPixelShader.Reset();
    service.quadVertexShader.Reset();
    service.adapterDescription.clear();
    service.adapterDedicatedVideoMemory = 0;
    service.adapterVendorId = 0;
    service.adapterDeviceId = 0;
    service.compositionDevice.Reset();
    service.factory.Reset();
    service.dxgiDevice.Reset();
    service.context.Reset();
    service.device.Reset();
    service.activePresenters = 0;
    service.readyPipelines.store(GpuPipelineNone,
                                 std::memory_order_release);
    service.ready.store(false, std::memory_order_release);
    if (++service.generation == 0) ++service.generation;
}

static bool FailGpuServiceLocked(HRESULT hr, PCWSTR stage) {
    ReleaseGpuDeviceResourcesLocked();
    g_gpuRenderService.retryAfterTick = GetTickCount() + 30000;
    Wh_Log(L"GPU renderer unavailable at %s (0x%08X); using CPU fallback",
           stage, static_cast<unsigned>(hr));
    return false;
}

static bool InitializeGpuServiceLocked(uint32_t requiredPipelines) {
    auto& service = g_gpuRenderService;
    if (RequiresCpuAnimationRendererProcess()) {
        return false;
    }
    if (g_unloading.load(std::memory_order_relaxed) ||
        GetSystemMetrics(SM_REMOTESESSION)) {
        return false;
    }

    const bool initializeCore =
        !service.ready.load(std::memory_order_acquire);
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGIDevice1> dxgiDevice;
    ComPtr<IDXGIFactory2> factory;
    ComPtr<IDCompositionDevice> compositionDevice;
    DXGI_ADAPTER_DESC1 selectedAdapterDesc{};
    bool selectedAdapterDescAvailable = false;
    D3D_FEATURE_LEVEL selectedLevel{};
    HRESULT hr = S_OK;

    if (!initializeCore) {
        const HRESULT deviceState = service.device
                                        ? service.device->GetDeviceRemovedReason()
                                        : E_UNEXPECTED;
        if (FAILED(deviceState)) {
            return FailGpuServiceLocked(deviceState, L"device state");
        }
        device = service.device;
    } else {
        const DWORD now = GetTickCount();
        if (service.retryAfterTick &&
            static_cast<LONG>(now - service.retryAfterTick) < 0) {
            return false;
        }
        service.retryAfterTick = 0;
        ReleaseGpuDeviceResourcesLocked();

        const D3D_FEATURE_LEVEL requestedLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT, requestedLevels,
            ARRAYSIZE(requestedLevels), D3D11_SDK_VERSION,
            device.ReleaseAndGetAddressOf(), &selectedLevel,
            context.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"D3D11CreateDevice");
        }

        hr = device.As(&dxgiDevice);
        if (FAILED(hr)) return FailGpuServiceLocked(hr, L"IDXGIDevice1");
        // This limit is shared by every composition swap chain on the device.
        // Two permits keep concurrent same-process animations moving without
        // allowing a long queue of stale frames.
        dxgiDevice->SetMaximumFrameLatency(2);

        ComPtr<IDXGIAdapter> adapter;
        hr = dxgiDevice->GetAdapter(adapter.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return FailGpuServiceLocked(hr, L"GetAdapter");
        ComPtr<IDXGIAdapter1> adapter1;
        if (SUCCEEDED(adapter.As(&adapter1))) {
            if (SUCCEEDED(adapter1->GetDesc1(&selectedAdapterDesc))) {
                selectedAdapterDescAvailable = true;
                if (selectedAdapterDesc.Flags &
                    DXGI_ADAPTER_FLAG_SOFTWARE) {
                    return FailGpuServiceLocked(DXGI_ERROR_UNSUPPORTED,
                                                L"software adapter");
                }
            }
        }

        hr = adapter->GetParent(
            IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()));
        if (FAILED(hr)) return FailGpuServiceLocked(hr, L"IDXGIFactory2");

        hr = DCompositionCreateDevice(
            dxgiDevice.Get(), __uuidof(IDCompositionDevice),
            reinterpret_cast<void**>(
                compositionDevice.ReleaseAndGetAddressOf()));
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"DCompositionCreateDevice");
        }
    }

    const uint32_t readyPipelines =
        initializeCore
            ? GpuPipelineNone
            : service.readyPipelines.load(std::memory_order_acquire);
    const uint32_t missingPipelines =
        requiredPipelines & ~readyPipelines;
    if (!initializeCore && missingPipelines == GpuPipelineNone) return true;

    ComPtr<ID3D11VertexShader> quadVertexShader;
    ComPtr<ID3D11PixelShader> quadPixelShader;
    ComPtr<ID3D11Buffer> quadConstants;
    ComPtr<ID3D11VertexShader> genieVertexShader;
    ComPtr<ID3D11PixelShader> geniePixelShader;
    ComPtr<ID3D11Buffer> genieConstants;
    ComPtr<ID3D11VertexShader> inplaceVertexShader;
    ComPtr<ID3D11PixelShader> inplacePixelShader;
    ComPtr<ID3D11Buffer> inplaceConstants;
    ComPtr<ID3D11VertexShader> fieldVertexShader;
    ComPtr<ID3D11PixelShader> fieldPixelShader;
    ComPtr<ID3D11Buffer> fieldConstants;
    ComPtr<ID3D11VertexShader> closeVertexShader;
    ComPtr<ID3D11PixelShader> closePixelShader;
    ComPtr<ID3D11Buffer> closeConstants;
    ComPtr<ID3D11VertexShader> shatterVertexShader;
    ComPtr<ID3D11VertexShader> shatterPointVertexShader;
    ComPtr<ID3D11PixelShader> shatterPixelShader;
    ComPtr<ID3D11Buffer> shatterConstants;

    if (missingPipelines != GpuPipelineNone && !service.compilerModule) {
        service.compilerModule = LoadLibraryExW(
            L"d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    auto compile = missingPipelines != GpuPipelineNone &&
                           service.compilerModule
                       ? reinterpret_cast<D3DCompile_t>(GetProcAddress(
                             service.compilerModule, "D3DCompile"))
                       : nullptr;
    if (missingPipelines != GpuPipelineNone && !compile) {
        return FailGpuServiceLocked(HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND),
                                    L"D3DCompile");
    }

    constexpr UINT compileFlags =
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
    ComPtr<ID3DBlob> vertexBytecode;
    ComPtr<ID3DBlob> pointVertexBytecode;
    ComPtr<ID3DBlob> pixelBytecode;
    ComPtr<ID3DBlob> errors;
    if (missingPipelines & GpuPipelineQuad) {
    hr = compile(kGpuQuadShader, sizeof(kGpuQuadShader) - 1,
                 "windows-animations-gpu", nullptr, nullptr, "VSMain",
                 "vs_4_0", compileFlags, 0,
                 vertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return FailGpuServiceLocked(hr, L"vertex shader compile");
    errors.Reset();
    hr = compile(kGpuQuadShader, sizeof(kGpuQuadShader) - 1,
                 "windows-animations-gpu", nullptr, nullptr, "PSMain",
                 "ps_4_0", compileFlags, 0,
                 pixelBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return FailGpuServiceLocked(hr, L"pixel shader compile");

    hr = device->CreateVertexShader(
        vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
        nullptr, quadVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return FailGpuServiceLocked(hr, L"CreateVertexShader");
    hr = device->CreatePixelShader(
        pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
        nullptr, quadPixelShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return FailGpuServiceLocked(hr, L"CreatePixelShader");
    }

    if (missingPipelines & GpuPipelineGenie) {
    errors.Reset();
    hr = compile(kGpuGenieShader, sizeof(kGpuGenieShader) - 1,
                 "windows-animations-genie-gpu", nullptr, nullptr, "VSMain",
                 "vs_4_0", compileFlags, 0,
                 vertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Genie vertex shader compile");
    }
    errors.Reset();
    hr = compile(kGpuGenieShader, sizeof(kGpuGenieShader) - 1,
                 "windows-animations-genie-gpu", nullptr, nullptr, "PSMain",
                 "ps_4_0", compileFlags, 0,
                 pixelBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Genie pixel shader compile");
    }

    hr = device->CreateVertexShader(
        vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
        nullptr, genieVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateGenieVertexShader");
    }
    hr = device->CreatePixelShader(
        pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
        nullptr, geniePixelShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateGeniePixelShader");
    }
    }

    if (missingPipelines & GpuPipelineInplace) {
    errors.Reset();
    hr = compile(kGpuInplaceShader, sizeof(kGpuInplaceShader) - 1,
                 "windows-animations-inplace-gpu", nullptr, nullptr,
                 "VSMain", "vs_4_0", compileFlags, 0,
                 vertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Inplace vertex shader compile");
    }
    errors.Reset();
    hr = compile(kGpuInplaceShader, sizeof(kGpuInplaceShader) - 1,
                 "windows-animations-inplace-gpu", nullptr, nullptr,
                 "PSMain", "ps_4_0", compileFlags, 0,
                 pixelBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Inplace pixel shader compile");
    }

    hr = device->CreateVertexShader(
        vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
        nullptr, inplaceVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateInplaceVertexShader");
    }
    hr = device->CreatePixelShader(
        pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
        nullptr, inplacePixelShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateInplacePixelShader");
    }
    }

    if (missingPipelines & GpuPipelineField) {
    errors.Reset();
    hr = compile(kGpuFieldShader, sizeof(kGpuFieldShader) - 1,
                 "windows-animations-field-gpu", nullptr, nullptr,
                 "VSMain", "vs_4_0", compileFlags, 0,
                 vertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Field vertex shader compile");
    }
    errors.Reset();
    hr = compile(kGpuFieldShader, sizeof(kGpuFieldShader) - 1,
                 "windows-animations-field-gpu", nullptr, nullptr,
                 "PSMain", "ps_4_0", compileFlags, 0,
                 pixelBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Field pixel shader compile");
    }

    hr = device->CreateVertexShader(
        vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
        nullptr, fieldVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateFieldVertexShader");
    }
    hr = device->CreatePixelShader(
        pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
        nullptr, fieldPixelShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateFieldPixelShader");
    }
    }

    if (missingPipelines & GpuPipelineClose) {
    errors.Reset();
    hr = compile(kGpuCloseShader, sizeof(kGpuCloseShader) - 1,
                 "windows-animations-close-gpu", nullptr, nullptr,
                 "VSMain", "vs_4_0", compileFlags, 0,
                 vertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Close vertex shader compile");
    }

    errors.Reset();
    hr = compile(kGpuCloseShader, sizeof(kGpuCloseShader) - 1,
                 "windows-animations-close-gpu", nullptr, nullptr,
                 "PSMain", "ps_4_0", compileFlags, 0,
                 pixelBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Close pixel shader compile");
    }

    hr = device->CreateVertexShader(
        vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
        nullptr, closeVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateCloseVertexShader");
    }
    hr = device->CreatePixelShader(
        pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
        nullptr, closePixelShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateClosePixelShader");
    }
    }

    if (missingPipelines & GpuPipelineShatter) {
    errors.Reset();
    hr = compile(kGpuShatterShader, sizeof(kGpuShatterShader) - 1,
                 "windows-animations-shatter-gpu", nullptr, nullptr,
                 "VSMain", "vs_4_0", compileFlags, 0,
                 vertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Shatter vertex shader compile");
    }
    errors.Reset();
    hr = compile(kGpuShatterShader, sizeof(kGpuShatterShader) - 1,
                 "windows-animations-shatter-point-gpu", nullptr, nullptr,
                 "VSPointMain", "vs_4_0", compileFlags, 0,
                 pointVertexBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(
            hr, L"Shatter point vertex shader compile");
    }
    errors.Reset();
    hr = compile(kGpuShatterShader, sizeof(kGpuShatterShader) - 1,
                 "windows-animations-shatter-gpu", nullptr, nullptr,
                 "PSMain", "ps_4_0", compileFlags, 0,
                 pixelBytecode.ReleaseAndGetAddressOf(),
                 errors.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"Shatter pixel shader compile");
    }
    hr = device->CreateVertexShader(
        vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
        nullptr, shatterVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateShatterVertexShader");
    }
    hr = device->CreateVertexShader(
        pointVertexBytecode->GetBufferPointer(),
        pointVertexBytecode->GetBufferSize(), nullptr,
        shatterPointVertexShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(
            hr, L"CreateShatterPointVertexShader");
    }
    hr = device->CreatePixelShader(
        pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
        nullptr, shatterPixelShader.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        return FailGpuServiceLocked(hr, L"CreateShatterPixelShader");
    }
    }

    D3D11_BUFFER_DESC constantsDesc{};
    constantsDesc.Usage = D3D11_USAGE_DEFAULT;
    constantsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (missingPipelines & GpuPipelineQuad) {
        constantsDesc.ByteWidth = sizeof(GpuQuadConstants);
        hr = device->CreateBuffer(&constantsDesc, nullptr,
                                  quadConstants.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateQuadBuffer");
        }
    }
    if (missingPipelines & GpuPipelineGenie) {
        constantsDesc.ByteWidth = sizeof(GpuGenieConstants);
        hr = device->CreateBuffer(&constantsDesc, nullptr,
                                  genieConstants.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateGenieBuffer");
        }
    }
    if (missingPipelines & GpuPipelineInplace) {
        constantsDesc.ByteWidth = sizeof(GpuInplaceConstants);
        hr = device->CreateBuffer(
            &constantsDesc, nullptr,
            inplaceConstants.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateInplaceBuffer");
        }
    }
    if (missingPipelines & GpuPipelineField) {
        constantsDesc.ByteWidth = sizeof(GpuFieldConstants);
        hr = device->CreateBuffer(&constantsDesc, nullptr,
                                  fieldConstants.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateFieldBuffer");
        }
    }
    if (missingPipelines & GpuPipelineClose) {
        constantsDesc.ByteWidth = sizeof(GpuCloseConstants);
        hr = device->CreateBuffer(&constantsDesc, nullptr,
                                  closeConstants.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateCloseBuffer");
        }
    }
    if (missingPipelines & GpuPipelineShatter) {
        constantsDesc.ByteWidth = sizeof(GpuShatterConstants);
        hr = device->CreateBuffer(&constantsDesc, nullptr,
                                  shatterConstants.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateShatterBuffer");
        }
    }

    ComPtr<ID3D11BlendState> blendState;
    ComPtr<ID3D11RasterizerState> rasterizerState;
    if (initializeCore) {
        D3D11_BLEND_DESC blendDesc{};
        auto& targetBlend = blendDesc.RenderTarget[0];
        targetBlend.BlendEnable = TRUE;
        targetBlend.SrcBlend = D3D11_BLEND_ONE;
        targetBlend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        targetBlend.BlendOp = D3D11_BLEND_OP_ADD;
        targetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
        targetBlend.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        targetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        targetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(
            &blendDesc, blendState.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateBlendState");
        }

        D3D11_RASTERIZER_DESC rasterizerDesc{};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.DepthClipEnable = TRUE;
        hr = device->CreateRasterizerState(
            &rasterizerDesc, rasterizerState.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            return FailGpuServiceLocked(hr, L"CreateRasterizerState");
        }
    }

    if (initializeCore) {
        service.device = std::move(device);
        service.context = std::move(context);
        service.dxgiDevice = std::move(dxgiDevice);
        service.factory = std::move(factory);
        service.compositionDevice = std::move(compositionDevice);
        if (selectedAdapterDescAvailable) {
            service.adapterDescription = selectedAdapterDesc.Description;
            service.adapterDedicatedVideoMemory = static_cast<uint64_t>(
                selectedAdapterDesc.DedicatedVideoMemory);
            service.adapterVendorId = selectedAdapterDesc.VendorId;
            service.adapterDeviceId = selectedAdapterDesc.DeviceId;
        }
        service.blendState = std::move(blendState);
        service.rasterizerState = std::move(rasterizerState);
    }
    if (missingPipelines & GpuPipelineQuad) {
        service.quadVertexShader = std::move(quadVertexShader);
        service.quadPixelShader = std::move(quadPixelShader);
        service.quadConstants = std::move(quadConstants);
    }
    if (missingPipelines & GpuPipelineGenie) {
        service.genieVertexShader = std::move(genieVertexShader);
        service.geniePixelShader = std::move(geniePixelShader);
        service.genieConstants = std::move(genieConstants);
    }
    if (missingPipelines & GpuPipelineInplace) {
        service.inplaceVertexShader = std::move(inplaceVertexShader);
        service.inplacePixelShader = std::move(inplacePixelShader);
        service.inplaceConstants = std::move(inplaceConstants);
    }
    if (missingPipelines & GpuPipelineField) {
        service.fieldVertexShader = std::move(fieldVertexShader);
        service.fieldPixelShader = std::move(fieldPixelShader);
        service.fieldConstants = std::move(fieldConstants);
    }
    if (missingPipelines & GpuPipelineClose) {
        service.closeVertexShader = std::move(closeVertexShader);
        service.closePixelShader = std::move(closePixelShader);
        service.closeConstants = std::move(closeConstants);
    }
    if (missingPipelines & GpuPipelineShatter) {
        service.shatterVertexShader = std::move(shatterVertexShader);
        service.shatterPointVertexShader =
            std::move(shatterPointVertexShader);
        service.shatterPixelShader = std::move(shatterPixelShader);
        service.shatterConstants = std::move(shatterConstants);
    }
    service.readyPipelines.fetch_or(missingPipelines,
                                    std::memory_order_release);
    if (initializeCore) {
        service.ready.store(true, std::memory_order_release);
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(
                L"GPU renderer core ready (feature level 0x%X) adapter=%s "
                L"vendor=0x%04X device=0x%04X dedicated_mib=%.0f",
                static_cast<unsigned>(selectedLevel),
                service.adapterDescription.empty()
                    ? L"unknown"
                    : service.adapterDescription.c_str(),
                service.adapterVendorId, service.adapterDeviceId,
                static_cast<double>(service.adapterDedicatedVideoMemory) /
                    (1024.0 * 1024.0));
        }
    }
    if (missingPipelines != GpuPipelineNone &&
        IsDiagnosticLoggingEnabled()) {
        Wh_Log(L"GPU renderer pipelines ready mask=0x%X total=0x%X",
               static_cast<unsigned>(missingPipelines),
               static_cast<unsigned>(service.readyPipelines.load(
                   std::memory_order_relaxed)));
    }
    return true;
}

static std::atomic<bool> g_gpuWarmupRunning{false};
static std::atomic<uint32_t> g_gpuWarmupRequestedPipelines{
    GpuPipelineNone};
static std::atomic<uint64_t> g_gpuWarmupSettingsGeneration{1};
static std::mutex g_gpuWarmupRequestMutex;

static void CancelPendingGpuWarmup() {
    std::lock_guard<std::mutex> lock(g_gpuWarmupRequestMutex);
    uint64_t generation =
        g_gpuWarmupSettingsGeneration.fetch_add(
            1, std::memory_order_acq_rel) + 1;
    if (!generation) {
        g_gpuWarmupSettingsGeneration.store(1, std::memory_order_release);
    }
    g_gpuWarmupRequestedPipelines.store(
        GpuPipelineNone, std::memory_order_release);
}

static bool QueueGpuWarmupRequest(uint32_t requiredPipelines,
                                  uint64_t expectedGeneration = 0) {
    if (requiredPipelines == GpuPipelineNone) return false;
    std::lock_guard<std::mutex> lock(g_gpuWarmupRequestMutex);
    if (expectedGeneration &&
        expectedGeneration !=
            g_gpuWarmupSettingsGeneration.load(
                std::memory_order_acquire)) {
        return false;
    }
    g_gpuWarmupRequestedPipelines.fetch_or(requiredPipelines,
                                           std::memory_order_release);
    return true;
}

static bool IsGpuServiceReady(uint32_t requiredPipelines) {
    if (!g_gpuRenderService.ready.load(std::memory_order_acquire)) {
        return false;
    }
    const uint32_t readyPipelines =
        g_gpuRenderService.readyPipelines.load(std::memory_order_acquire);
    return (readyPipelines & requiredPipelines) == requiredPipelines;
}

static DWORD WINAPI GpuWarmupThread(LPVOID) {
    for (;;) {
        if (g_unloading.load(std::memory_order_relaxed)) {
            g_gpuWarmupRequestedPipelines.store(
                GpuPipelineNone, std::memory_order_release);
            g_gpuWarmupRunning.store(false, std::memory_order_release);
            break;
        }
        uint32_t requestedPipelines = GpuPipelineNone;
        uint64_t requestGeneration = 0;
        {
            std::lock_guard<std::mutex> lock(g_gpuWarmupRequestMutex);
            requestGeneration =
                g_gpuWarmupSettingsGeneration.load(
                    std::memory_order_acquire);
            requestedPipelines =
                g_gpuWarmupRequestedPipelines.exchange(
                    GpuPipelineNone, std::memory_order_acq_rel);
        }
        if (requestedPipelines != GpuPipelineNone) {
            if (requestGeneration !=
                g_gpuWarmupSettingsGeneration.load(
                    std::memory_order_acquire)) {
                continue;
            }
            bool deferForActivePresenter = false;
            {
                std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
                if (g_gpuRenderService.activePresenters != 0) {
                    deferForActivePresenter = true;
                } else if (requestGeneration ==
                           g_gpuWarmupSettingsGeneration.load(
                               std::memory_order_acquire)) {
                    InitializeGpuServiceLocked(requestedPipelines);
                }
            }
            if (deferForActivePresenter) {
                // D3DCompile is intentionally serialized with service state.
                // Don't let a newly requested pipeline stall frames from an
                // already-running GPU animation in the same process.
                QueueGpuWarmupRequest(requestedPipelines,
                                      requestGeneration);
                Sleep(10);
            }
            continue;
        }

        g_gpuWarmupRunning.store(false, std::memory_order_release);
        if (g_gpuWarmupRequestedPipelines.load(std::memory_order_acquire) ==
            GpuPipelineNone) {
            break;
        }
        bool expected = false;
        if (!g_gpuWarmupRunning.compare_exchange_strong(
                expected, true, std::memory_order_acq_rel)) {
            break;
        }
    }
    return 0;
}

static void RequestGpuWarmup(uint32_t requiredPipelines,
                             uint64_t expectedGeneration = 0) {
    if (requiredPipelines == GpuPipelineNone) return;
    if ((!g_gpuAcceleration.load(std::memory_order_relaxed) &&
         !g_closeGpuAcceleration.load(std::memory_order_relaxed)) ||
        RequiresCpuAnimationRendererProcess() ||
        g_unloading.load(std::memory_order_relaxed) ||
        GetSystemMetrics(SM_REMOTESESSION)) {
        return;
    }
    {
        std::unique_lock<std::mutex> lock(g_gpuRenderService.mutex,
                                          std::try_to_lock);
        if (lock.owns_lock()) {
            if (IsGpuServiceReady(requiredPipelines)) {
                return;
            }
            const DWORD retryAfter = g_gpuRenderService.retryAfterTick;
            if (retryAfter &&
                static_cast<LONG>(GetTickCount() - retryAfter) < 0) {
                return;
            }
        }
    }
    if (!QueueGpuWarmupRequest(requiredPipelines,
                               expectedGeneration)) {
        return;
    }
    bool expected = false;
    if (!g_gpuWarmupRunning.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel)) {
        return;
    }
    if (!StartWorkerThread(GpuWarmupThread, nullptr)) {
        g_gpuWarmupRunning.store(false, std::memory_order_release);
    }
}

struct StableGpuWindowSearch {
    DWORD processId{};
    HWND hWnd{};
    bool requireCloseCompatibility{};
    HWND excludedWindow{};
};

static BOOL CALLBACK FindStableGpuWindowProc(HWND hWnd, LPARAM lParam) {
    auto* search = reinterpret_cast<StableGpuWindowSearch*>(lParam);
    if (hWnd == search->excludedWindow) return TRUE;
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != search->processId || !IsWindowVisible(hWnd) ||
        IsIconic(hWnd) || IsHungAppWindow(hWnd) || !IsLaunchWindow(hWnd) ||
        GetPropW(hWnd, kPropCloseBypass) || GetPropW(hWnd, kPropClosed) ||
        GetPropW(hWnd, kPropLaunchAnimation)) {
        return TRUE;
    }
    if (search->requireCloseCompatibility &&
        RequiresCpuClosePresentation(hWnd)) {
        return TRUE;
    }
    search->hWnd = hWnd;
    return FALSE;
}

static HWND FindStableGpuWindow(bool requireCloseCompatibility = false,
                                HWND excludedWindow = nullptr) {
    StableGpuWindowSearch search{
        GetCurrentProcessId(), nullptr, requireCloseCompatibility,
        excludedWindow};
    EnumWindows(FindStableGpuWindowProc,
                reinterpret_cast<LPARAM>(&search));
    return search.hWnd;
}

struct RemainingAnimationWindowSearch {
    DWORD processId{};
    HWND excludedWindow{};
    bool found{};
};

static BOOL CALLBACK FindRemainingAnimationWindowProc(HWND hWnd,
                                                       LPARAM lParam) {
    auto* search =
        reinterpret_cast<RemainingAnimationWindowSearch*>(lParam);
    if (hWnd == search->excludedWindow) return TRUE;

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != search->processId ||
        (!IsWindowVisible(hWnd) && !IsIconic(hWnd)) ||
        GetPropW(hWnd, kPropCloseBypass) ||
        GetPropW(hWnd, kPropClosed) || !IsLaunchWindow(hWnd)) {
        return TRUE;
    }

    search->found = true;
    return FALSE;
}

static bool HasRemainingAnimationWindow(HWND excludedWindow) {
    RemainingAnimationWindowSearch search{
        GetCurrentProcessId(), excludedWindow, false};
    EnumWindows(FindRemainingAnimationWindowProc,
                reinterpret_cast<LPARAM>(&search));
    return search.found;
}

static void RequestPredictedGpuWarmup(HWND hStableWnd) {
    if (RequiresCpuAnimationRendererProcess() ||
        g_unloading.load(std::memory_order_relaxed) ||
        GetSystemMetrics(SM_REMOTESESSION)) {
        return;
    }
    const uint64_t settingsGeneration =
        g_gpuWarmupSettingsGeneration.load(std::memory_order_acquire);

    uint32_t requiredPipelines = GpuPipelineNone;
    const bool minRestoreRelevant =
        g_restoreAnimation.load(std::memory_order_relaxed) ||
        g_launchAnimation.load(std::memory_order_relaxed);
    if (minRestoreRelevant &&
        g_gpuAcceleration.load(std::memory_order_relaxed)) {
        requiredPipelines |= RequiredGpuPipelines(
            /*isClosing=*/false,
            PeekMinRestoreEffectStyle(hStableWnd));
    }

    if (!hStableWnd || !IsWindow(hStableWnd)) {
        hStableWnd = FindStableGpuWindow();
    }
    if (g_closeAnimation.load(std::memory_order_relaxed) &&
        g_closeGpuAcceleration.load(std::memory_order_relaxed)) {
        const int closeEffect = PeekCloseEffectStyle();
        const int blockSize =
            std::max(1, g_shatterBlockSize.load(std::memory_order_relaxed));
        HWND hCloseCompatibleWnd =
            hStableWnd && !RequiresCpuClosePresentation(hStableWnd)
                ? hStableWnd
                : FindStableGpuWindow(/*requireCloseCompatibility=*/true);
        if (hCloseCompatibleWnd &&
            IsPotentialHybridGpuClose(closeEffect, blockSize)) {
            requiredPipelines |= RequiredGpuPipelines(
                /*isClosing=*/true, closeEffect);
        }
    }

    // Peeking doesn't consume the session-wide order. Another process can
    // advance that order before this process animates; if the newly selected
    // pipeline is still cold, the normal CPU fallback keeps the action safe
    // without reserving or precompiling every effect for idle windows.
    RequestGpuWarmup(requiredPipelines, settingsGeneration);
}

static std::atomic<bool> g_stableGpuWarmupRunning{false};

static DWORD WINAPI StableGpuWarmupThread(LPVOID) {
    const DWORD deadline = GetTickCount() + 2000;
    Sleep(200);
    while (!g_unloading.load(std::memory_order_relaxed) &&
           static_cast<LONG>(GetTickCount() - deadline) < 0) {
        bool animationActive = false;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            animationActive = !g_AnimActive.empty();
        }
        if (!animationActive) {
            if (HWND hStableWnd = FindStableGpuWindow()) {
                RequestPredictedGpuWarmup(hStableWnd);
                break;
            }
        }
        Sleep(25);
    }
    g_stableGpuWarmupRunning.store(false, std::memory_order_release);
    return 0;
}

static void ScheduleStableGpuWarmup() {
    if (RequiresCpuAnimationRendererProcess() ||
        g_unloading.load(std::memory_order_relaxed) ||
        GetSystemMetrics(SM_REMOTESESSION) ||
        (!g_gpuAcceleration.load(std::memory_order_relaxed) &&
         !g_closeGpuAcceleration.load(std::memory_order_relaxed))) {
        return;
    }
    bool expected = false;
    if (!g_stableGpuWarmupRunning.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel)) {
        return;
    }
    if (!StartWorkerThread(StableGpuWarmupThread, nullptr)) {
        g_stableGpuWarmupRunning.store(false, std::memory_order_release);
    }
}

static void ReportGpuPresenterFailureLocked(HRESULT hr, PCWSTR stage) {
    auto& service = g_gpuRenderService;
    const HRESULT removedReason =
        service.device ? service.device->GetDeviceRemovedReason() : S_OK;
    if (FAILED(removedReason) || hr == DXGI_ERROR_DEVICE_REMOVED ||
        hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_HUNG ||
        hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR) {
        FailGpuServiceLocked(FAILED(removedReason) ? removedReason : hr, stage);
        return;
    }
    Wh_Log(L"GPU presenter unavailable at %s (0x%08X); using CPU fallback",
           stage, static_cast<unsigned>(hr));
}

class GpuEffectPresenter {
private:
    uint64_t serviceGeneration{};
    HWND presentationWindow{};
    HDC presentationScreenDC{};
    bool layeredPresentation{};
    int outputWidth{};
    int outputHeight{};
    int inputWidth{};
    int inputHeight{};
    uint64_t estimatedAllocationBytes{};
    ComPtr<IDCompositionDevice> compositionDevice;
    ComPtr<IDCompositionTarget> compositionTarget;
    ComPtr<IDCompositionVisual> compositionVisual;
    ComPtr<IDXGISwapChain1> swapChain;
    ComPtr<IDXGISwapChain2> framePacedSwapChain;
    HANDLE frameLatencyWaitableObject{};
    bool frameSlotReserved{};
    ComPtr<ID3D11Texture2D> layeredOutputTexture;
    ComPtr<IDXGISurface1> layeredOutputSurface;
    ComPtr<ID3D11RenderTargetView> renderTarget;
    ComPtr<ID3D11Texture2D> sourceTexture;
    ComPtr<ID3D11ShaderResourceView> sourceView;
    ComPtr<ID3D11Texture2D> effectFieldTexture;
    ComPtr<ID3D11ShaderResourceView> effectFieldView;
    int effectFieldWidth{};
    int effectFieldHeight{};
    void ResetObjectsLocked() {
        if (serviceGeneration &&
            serviceGeneration == g_gpuRenderService.generation &&
            g_gpuRenderService.activePresenters != 0) {
            --g_gpuRenderService.activePresenters;
        }
        if (compositionTarget) compositionTarget->SetRoot(nullptr);
        if (compositionDevice) compositionDevice->Commit();
        // GetFrameLatencyWaitableObject returns a DXGI-owned handle. Releasing
        // the swap chain closes it; calling CloseHandle here would double-close
        // an object the application doesn't own.
        frameLatencyWaitableObject = nullptr;
        frameSlotReserved = false;
        effectFieldView.Reset();
        effectFieldTexture.Reset();
        sourceView.Reset();
        sourceTexture.Reset();
        renderTarget.Reset();
        layeredOutputSurface.Reset();
        layeredOutputTexture.Reset();
        framePacedSwapChain.Reset();
        swapChain.Reset();
        compositionVisual.Reset();
        compositionTarget.Reset();
        compositionDevice.Reset();
        serviceGeneration = 0;
        presentationWindow = nullptr;
        presentationScreenDC = nullptr;
        layeredPresentation = false;
        outputWidth = outputHeight = 0;
        inputWidth = inputHeight = 0;
        effectFieldWidth = effectFieldHeight = 0;
        estimatedAllocationBytes = 0;
    }

    bool CanPresentLocked() const {
        const auto& service = g_gpuRenderService;
        return serviceGeneration && serviceGeneration == service.generation &&
               service.ready.load(std::memory_order_acquire) &&
               ((layeredPresentation && layeredOutputSurface) ||
                (!layeredPresentation && swapChain)) &&
               renderTarget && sourceView;
    }

    bool WaitForFrameSlot() {
        if (!frameLatencyWaitableObject) return true;
        if (frameSlotReserved) return true;
        const DWORD waitResult = WaitForSingleObject(
            frameLatencyWaitableObject,
            AnimConstants::GpuFrameLatencyWaitMs);
        if (waitResult == WAIT_OBJECT_0) {
            frameSlotReserved = true;
            return true;
        }
        Wh_Log(L"GPU frame-latency wait failed result=0x%08X",
               static_cast<unsigned>(waitResult));
        return false;
    }

    void BeginFrameLocked(bool enableBlending = true) {
        auto& service = g_gpuRenderService;
        const FLOAT transparent[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        service.context->ClearRenderTargetView(renderTarget.Get(), transparent);
        ID3D11RenderTargetView* target = renderTarget.Get();
        service.context->OMSetRenderTargets(1, &target, nullptr);
        const FLOAT blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        service.context->OMSetBlendState(
            enableBlending ? service.blendState.Get() : nullptr,
            blendFactor, 0xFFFFFFFFu);
        D3D11_VIEWPORT viewport{};
        viewport.Width = static_cast<float>(outputWidth);
        viewport.Height = static_cast<float>(outputHeight);
        viewport.MaxDepth = 1.0f;
        service.context->RSSetViewports(1, &viewport);
        service.context->RSSetState(service.rasterizerState.Get());
        service.context->IASetInputLayout(nullptr);
        service.context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    }

    bool EndFrameLocked() {
        auto& service = g_gpuRenderService;
        ID3D11ShaderResourceView* noViews[2] = {nullptr, nullptr};
        service.context->PSSetShaderResources(0, 2, noViews);
        service.context->OMSetRenderTargets(0, nullptr, nullptr);
        if (layeredPresentation) {
            HDC surfaceDC = nullptr;
            HRESULT hr = layeredOutputSurface->GetDC(FALSE, &surfaceDC);
            if (FAILED(hr) || !surfaceDC) {
                ReportGpuPresenterFailureLocked(
                    FAILED(hr) ? hr : E_FAIL, L"GetLayeredSurfaceDC");
                return false;
            }

            RECT windowRect{};
            POINT destination{};
            BOOL updated = FALSE;
            DWORD updateError = ERROR_INVALID_WINDOW_HANDLE;
            if (IsWindow(presentationWindow) &&
                GetWindowRect(presentationWindow, &windowRect)) {
                destination.x = windowRect.left;
                destination.y = windowRect.top;
                SIZE size{outputWidth, outputHeight};
                POINT source{};
                BLENDFUNCTION blend{
                    AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                updated = UpdateLayeredWindow(
                    presentationWindow, presentationScreenDC, &destination, &size,
                    surfaceDC, &source, 0, &blend, ULW_ALPHA);
                updateError = updated ? ERROR_SUCCESS : GetLastError();
            }
            const HRESULT releaseHr = layeredOutputSurface->ReleaseDC(nullptr);
            if (!updated || FAILED(releaseHr)) {
                const HRESULT failure = !updated
                                            ? HRESULT_FROM_WIN32(updateError)
                                            : releaseHr;
                ReportGpuPresenterFailureLocked(
                    FAILED(failure) ? failure : E_FAIL,
                    L"PresentLayeredSurface");
                return false;
            }
            return true;
        }
        const HRESULT hr = swapChain->Present(0, 0);
        frameSlotReserved = false;
        if (FAILED(hr)) {
            Wh_Log(L"GPU present failed (0x%08X); ending animation safely",
                   static_cast<unsigned>(hr));
            ReportGpuPresenterFailureLocked(hr, L"Present");
            return false;
        }
        return true;
    }

public:
    bool Initialize(HWND hWnd, HDC screenDC, const void* sourceBits,
                    int sourceWidth,
                    int sourceHeight, int canvasWidth, int canvasHeight,
                    bool framePaced, bool useLayeredPresentation,
                    uint32_t requiredPipelines) {
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        ResetObjectsLocked();
        if (!sourceBits || sourceWidth < 1 || sourceHeight < 1 ||
            canvasWidth < 1 || canvasHeight < 1 ||
            !InitializeGpuServiceLocked(requiredPipelines)) {
            return false;
        }
        auto& service = g_gpuRenderService;
        HRESULT hr = S_OK;
        if (useLayeredPresentation) {
            D3D11_TEXTURE2D_DESC outputDesc{};
            outputDesc.Width = static_cast<UINT>(canvasWidth);
            outputDesc.Height = static_cast<UINT>(canvasHeight);
            outputDesc.MipLevels = 1;
            outputDesc.ArraySize = 1;
            outputDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            outputDesc.SampleDesc.Count = 1;
            outputDesc.Usage = D3D11_USAGE_DEFAULT;
            outputDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
            outputDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
            hr = service.device->CreateTexture2D(
                &outputDesc, nullptr,
                layeredOutputTexture.ReleaseAndGetAddressOf());
            if (SUCCEEDED(hr)) {
                hr = service.device->CreateRenderTargetView(
                    layeredOutputTexture.Get(), nullptr,
                    renderTarget.ReleaseAndGetAddressOf());
            }
            if (SUCCEEDED(hr)) {
                hr = layeredOutputTexture.As(&layeredOutputSurface);
            }
        } else {
            DXGI_SWAP_CHAIN_DESC1 swapDesc{};
            swapDesc.Width = static_cast<UINT>(canvasWidth);
            swapDesc.Height = static_cast<UINT>(canvasHeight);
            swapDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            swapDesc.Stereo = FALSE;
            swapDesc.SampleDesc.Count = 1;
            swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapDesc.BufferCount = 2;
            swapDesc.Scaling = DXGI_SCALING_STRETCH;
            swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swapDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
            if (framePaced) {
                swapDesc.Flags =
                    DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
            }
            hr = service.factory->CreateSwapChainForComposition(
                service.device.Get(), &swapDesc, nullptr,
                swapChain.ReleaseAndGetAddressOf());
            if (SUCCEEDED(hr) && framePaced) {
                hr = swapChain.As(&framePacedSwapChain);
                if (SUCCEEDED(hr)) {
                    hr = framePacedSwapChain->SetMaximumFrameLatency(1);
                }
                if (SUCCEEDED(hr)) {
                    frameLatencyWaitableObject =
                        framePacedSwapChain->GetFrameLatencyWaitableObject();
                    if (!frameLatencyWaitableObject) {
                        hr = HRESULT_FROM_WIN32(GetLastError());
                        if (SUCCEEDED(hr)) hr = E_FAIL;
                    }
                }
            }
            ComPtr<ID3D11Texture2D> backBuffer;
            if (SUCCEEDED(hr)) {
                hr = swapChain->GetBuffer(
                    0, IID_PPV_ARGS(backBuffer.ReleaseAndGetAddressOf()));
            }
            if (SUCCEEDED(hr)) {
                hr = service.device->CreateRenderTargetView(
                    backBuffer.Get(), nullptr,
                    renderTarget.ReleaseAndGetAddressOf());
            }
        }
        if (FAILED(hr)) {
            ResetObjectsLocked();
            ReportGpuPresenterFailureLocked(
                hr, useLayeredPresentation ? L"CreateLayeredRenderTarget"
                                           : L"CreateSwapChainRenderTarget");
            return false;
        }

        D3D11_TEXTURE2D_DESC sourceDesc{};
        sourceDesc.Width = static_cast<UINT>(sourceWidth);
        sourceDesc.Height = static_cast<UINT>(sourceHeight);
        sourceDesc.MipLevels = 1;
        sourceDesc.ArraySize = 1;
        sourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sourceDesc.SampleDesc.Count = 1;
        sourceDesc.Usage = D3D11_USAGE_IMMUTABLE;
        sourceDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA sourceData{};
        sourceData.pSysMem = sourceBits;
        sourceData.SysMemPitch = static_cast<UINT>(sourceWidth * 4);
        hr = service.device->CreateTexture2D(
            &sourceDesc, &sourceData,
            sourceTexture.ReleaseAndGetAddressOf());
        if (SUCCEEDED(hr)) {
            hr = service.device->CreateShaderResourceView(
                sourceTexture.Get(), nullptr,
                sourceView.ReleaseAndGetAddressOf());
        }
        if (FAILED(hr)) {
            ResetObjectsLocked();
            ReportGpuPresenterFailureLocked(hr, L"CreateSourceTexture");
            return false;
        }

        if (!useLayeredPresentation) {
            compositionDevice = service.compositionDevice;
            hr = compositionDevice->CreateTargetForHwnd(
                hWnd, TRUE, compositionTarget.ReleaseAndGetAddressOf());
            if (SUCCEEDED(hr)) {
                hr = compositionDevice->CreateVisual(
                    compositionVisual.ReleaseAndGetAddressOf());
            }
            if (SUCCEEDED(hr)) {
                hr = compositionVisual->SetContent(swapChain.Get());
            }
            if (SUCCEEDED(hr)) {
                hr = compositionTarget->SetRoot(compositionVisual.Get());
            }
            if (SUCCEEDED(hr)) hr = compositionDevice->Commit();
            if (FAILED(hr)) {
                ResetObjectsLocked();
                ReportGpuPresenterFailureLocked(hr, L"composition setup");
                return false;
            }
        }

        serviceGeneration = service.generation;
        ++service.activePresenters;
        presentationWindow = hWnd;
        presentationScreenDC = screenDC;
        layeredPresentation = useLayeredPresentation;
        outputWidth = canvasWidth;
        outputHeight = canvasHeight;
        inputWidth = sourceWidth;
        inputHeight = sourceHeight;
        const uint64_t outputPixels =
            static_cast<uint64_t>(canvasWidth) * canvasHeight;
        const uint64_t sourcePixels =
            static_cast<uint64_t>(sourceWidth) * sourceHeight;
        // Count only explicit per-animation GPU surfaces. Driver-managed
        // composition storage and the shared device/shaders aren't knowable
        // here, so telemetry deliberately reports this as an estimate.
        estimatedAllocationBytes =
            sourcePixels * 4u + outputPixels * 4u *
                (useLayeredPresentation ? 1u : 2u);
        return true;
    }

    bool InitializeInplaceField(uint32_t effectStyle, int blockSize,
                                int fieldStep) {
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        auto& service = g_gpuRenderService;
        if (!CanPresentLocked() || effectStyle < 1 ||
            (effectStyle > 3 && effectStyle != 8) ||
            blockSize < 1 || fieldStep < 1 || !service.fieldVertexShader ||
            !service.fieldPixelShader || !service.fieldConstants) {
            return false;
        }

        const int fieldWidth =
            (inputWidth + blockSize - 1) / blockSize;
        const int fieldHeight =
            (inputHeight + blockSize - 1) / blockSize;
        if (fieldWidth < 1 || fieldHeight < 1) return false;

        D3D11_TEXTURE2D_DESC fieldDesc{};
        fieldDesc.Width = static_cast<UINT>(fieldWidth);
        fieldDesc.Height = static_cast<UINT>(fieldHeight);
        fieldDesc.MipLevels = 1;
        fieldDesc.ArraySize = 1;
        // Ink and Perlin need one value; Scorch and Splinter need two. UVs for
        // Scorch are reconstructed from the block position in the consumer.
        // Preserve 32-bit precision while avoiding unused texture channels.
        const DXGI_FORMAT compactFieldFormat =
            effectStyle == 1 || effectStyle == 8
                ? DXGI_FORMAT_R32_FLOAT
                : DXGI_FORMAT_R32G32_FLOAT;
        fieldDesc.Format = compactFieldFormat;
        fieldDesc.SampleDesc.Count = 1;
        fieldDesc.Usage = D3D11_USAGE_DEFAULT;
        fieldDesc.BindFlags =
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        ComPtr<ID3D11Texture2D> fieldTexture;
        ComPtr<ID3D11RenderTargetView> fieldTarget;
        ComPtr<ID3D11ShaderResourceView> fieldView;
        auto createFieldResources = [&]() -> HRESULT {
            HRESULT result = service.device->CreateTexture2D(
                &fieldDesc, nullptr,
                fieldTexture.ReleaseAndGetAddressOf());
            if (SUCCEEDED(result)) {
                result = service.device->CreateRenderTargetView(
                    fieldTexture.Get(), nullptr,
                    fieldTarget.ReleaseAndGetAddressOf());
            }
            if (SUCCEEDED(result)) {
                result = service.device->CreateShaderResourceView(
                    fieldTexture.Get(), nullptr,
                    fieldView.ReleaseAndGetAddressOf());
            }
            return result;
        };
        HRESULT hr = createFieldResources();
        if (FAILED(hr) &&
            compactFieldFormat != DXGI_FORMAT_R32G32B32A32_FLOAT) {
            // Compact render-target formats are standard on supported D3D11
            // hardware, but retain the original layout as a compatibility
            // fallback for unusual drivers instead of dropping to CPU.
            fieldView.Reset();
            fieldTarget.Reset();
            fieldTexture.Reset();
            fieldDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            hr = createFieldResources();
        }
        if (FAILED(hr)) {
            ReportGpuPresenterFailureLocked(hr, L"CreateEffectField");
            return false;
        }

        const GpuFieldConstants constantsData{
            {static_cast<float>(inputWidth),
             static_cast<float>(inputHeight)},
            static_cast<float>(blockSize),
            static_cast<float>(fieldStep),
            effectStyle,
            {0.0f, 0.0f, 0.0f},
        };
        service.context->UpdateSubresource(
            service.fieldConstants.Get(), 0, nullptr, &constantsData, 0, 0);

        ID3D11RenderTargetView* target = fieldTarget.Get();
        service.context->OMSetRenderTargets(1, &target, nullptr);
        service.context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);
        D3D11_VIEWPORT viewport{};
        viewport.Width = static_cast<float>(fieldWidth);
        viewport.Height = static_cast<float>(fieldHeight);
        viewport.MaxDepth = 1.0f;
        service.context->RSSetViewports(1, &viewport);
        service.context->RSSetState(service.rasterizerState.Get());
        service.context->IASetInputLayout(nullptr);
        service.context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        service.context->VSSetShader(service.fieldVertexShader.Get(), nullptr,
                                     0);
        ID3D11Buffer* constantsBuffer = service.fieldConstants.Get();
        service.context->VSSetConstantBuffers(0, 1, &constantsBuffer);
        service.context->PSSetShader(service.fieldPixelShader.Get(), nullptr,
                                     0);
        service.context->PSSetConstantBuffers(0, 1, &constantsBuffer);
        service.context->Draw(4, 0);
        service.context->OMSetRenderTargets(0, nullptr, nullptr);

        effectFieldTexture = std::move(fieldTexture);
        effectFieldView = std::move(fieldView);
        effectFieldWidth = fieldWidth;
        effectFieldHeight = fieldHeight;
        const uint64_t bytesPerPixel =
            fieldDesc.Format == DXGI_FORMAT_R32_FLOAT
                ? 4u
                : fieldDesc.Format == DXGI_FORMAT_R32G32_FLOAT ? 8u : 16u;
        estimatedAllocationBytes +=
            static_cast<uint64_t>(fieldWidth) * fieldHeight * bytesPerPixel;
        return true;
    }

    bool PresentQuad(const QuadFrame& frame) {
        if (!WaitForFrameSlot()) return false;
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        auto& service = g_gpuRenderService;
        if (!CanPresentLocked() || !service.quadVertexShader ||
            !service.quadPixelShader || !service.quadConstants) {
            return false;
        }

        const GpuQuadConstants constantsData{
            {static_cast<float>(outputWidth),
             static_cast<float>(outputHeight)},
            {static_cast<float>(frame.x), static_cast<float>(frame.y)},
            {static_cast<float>(frame.width),
             static_cast<float>(frame.height)},
            Clamp(frame.opacity, 0.0f, 1.0f),
            0.0f,
            {static_cast<float>(inputWidth),
             static_cast<float>(inputHeight)},
            {0.0f, 0.0f},
        };
        service.context->UpdateSubresource(
            service.quadConstants.Get(), 0, nullptr, &constantsData, 0, 0);

        BeginFrameLocked();
        service.context->VSSetShader(service.quadVertexShader.Get(), nullptr, 0);
        ID3D11Buffer* constantsBuffer = service.quadConstants.Get();
        service.context->VSSetConstantBuffers(0, 1, &constantsBuffer);
        service.context->PSSetShader(service.quadPixelShader.Get(), nullptr, 0);
        service.context->PSSetConstantBuffers(0, 1, &constantsBuffer);
        ID3D11ShaderResourceView* view = sourceView.Get();
        service.context->PSSetShaderResources(0, 1, &view);
        service.context->Draw(4, 0);
        return EndFrameLocked();
    }

    bool PresentGenie(const GpuGenieFrame& frame) {
        if (!WaitForFrameSlot()) return false;
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        auto& service = g_gpuRenderService;
        if (!CanPresentLocked() || !service.genieVertexShader ||
            !service.geniePixelShader || !service.genieConstants) {
            return false;
        }

        const GpuGenieConstants constantsData{
            {static_cast<float>(outputWidth),
             static_cast<float>(outputHeight)},
            {static_cast<float>(inputWidth),
             static_cast<float>(inputHeight)},
            {frame.sourceOriginX, frame.sourceOriginY},
            {frame.dockX, frame.dockY},
            frame.neckWidth,
            Clamp(frame.morphTime, 0.0f, 1.0f),
            Clamp(frame.opacity, 0.0f, 1.0f),
            AnimConstants::MinimizeSpread,
            static_cast<uint32_t>(frame.taskbarEdge),
            {0.0f, 0.0f, 0.0f},
        };
        service.context->UpdateSubresource(
            service.genieConstants.Get(), 0, nullptr, &constantsData, 0, 0);

        BeginFrameLocked();
        service.context->VSSetShader(service.genieVertexShader.Get(), nullptr,
                                     0);
        ID3D11Buffer* constantsBuffer = service.genieConstants.Get();
        service.context->VSSetConstantBuffers(0, 1, &constantsBuffer);
        service.context->PSSetShader(service.geniePixelShader.Get(), nullptr,
                                     0);
        service.context->PSSetConstantBuffers(0, 1, &constantsBuffer);
        ID3D11ShaderResourceView* view = sourceView.Get();
        service.context->PSSetShaderResources(0, 1, &view);
        service.context->Draw(
            static_cast<UINT>(
                (static_cast<uint64_t>(
                     IsHorizontalTaskbar(frame.taskbarEdge)
                         ? inputHeight
                         : inputWidth) +
                 1) *
                2),
            0);
        return EndFrameLocked();
    }

    bool PresentInplace(const GpuInplaceFrame& frame) {
        if (!WaitForFrameSlot()) return false;
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        auto& service = g_gpuRenderService;
        if (!CanPresentLocked() || !service.inplaceVertexShader ||
            !service.inplacePixelShader || !service.inplaceConstants ||
            (frame.effectStyle <= 3 && !effectFieldView)) {
            return false;
        }

        const GpuInplaceConstants constantsData{
            {static_cast<float>(outputWidth),
             static_cast<float>(outputHeight)},
            {static_cast<float>(inputWidth),
             static_cast<float>(inputHeight)},
            {frame.rectOriginX, frame.rectOriginY},
            Clamp(frame.progress, 0.0f, 1.0f),
            static_cast<float>(std::max(1, frame.blockSize)),
            frame.effectStyle,
            {static_cast<float>(effectFieldWidth),
             static_cast<float>(effectFieldHeight)},
            0.0f,
        };
        service.context->UpdateSubresource(
            service.inplaceConstants.Get(), 0, nullptr, &constantsData, 0, 0);

        BeginFrameLocked();
        service.context->VSSetShader(service.inplaceVertexShader.Get(),
                                     nullptr, 0);
        ID3D11Buffer* constantsBuffer = service.inplaceConstants.Get();
        service.context->VSSetConstantBuffers(0, 1, &constantsBuffer);
        service.context->PSSetShader(service.inplacePixelShader.Get(),
                                     nullptr, 0);
        service.context->PSSetConstantBuffers(0, 1, &constantsBuffer);
        ID3D11ShaderResourceView* views[2] = {
            sourceView.Get(), effectFieldView.Get()};
        service.context->PSSetShaderResources(0, 2, views);
        service.context->Draw(4, 0);
        return EndFrameLocked();
    }

    bool PresentShatter(const GpuCloseFrame& frame) {
        if (!WaitForFrameSlot()) return false;
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        auto& service = g_gpuRenderService;
        const int blockSize =
            std::max(1, static_cast<int>(frame.blockSize));
        const bool usePointParticles = blockSize == 1;
        if (!CanPresentLocked() || !service.shatterVertexShader ||
            (usePointParticles && !service.shatterPointVertexShader) ||
            !service.shatterPixelShader || !service.shatterConstants) {
            return false;
        }

        const GpuShatterConstants shatterConstantsData{
            {static_cast<float>(outputWidth),
             static_cast<float>(outputHeight)},
            {static_cast<float>(inputWidth),
             static_cast<float>(inputHeight)},
            {frame.sourceOriginX, frame.sourceOriginY},
            Clamp(frame.progress, 0.0f, 1.0f),
            static_cast<uint32_t>(blockSize),
            AnimConstants::ThanosBaseStartMax,
            AnimConstants::ThanosWaveNoiseMult,
            AnimConstants::ThanosLifeSpan,
            0.0f,
        };
        service.context->UpdateSubresource(
            service.shatterConstants.Get(), 0, nullptr,
            &shatterConstantsData, 0, 0);

        const UINT blockColumns = static_cast<UINT>(
            (inputWidth + blockSize - 1) / blockSize);
        const UINT blockRows = static_cast<UINT>(
            (inputHeight + blockSize - 1) / blockSize);
        const uint64_t blockCount64 =
            static_cast<uint64_t>(blockColumns) * blockRows;
        if (!blockCount64 || blockCount64 > UINT_MAX) return false;

        // The old compute path first wrote every particle into another
        // full-size texture and then copied that texture to the presenter.
        // Rasterizing instances directly removes that duplicate allocation.
        // Disable blending so overlapping blocks retain the previous
        // overwrite behavior instead of accumulating opacity.
        BeginFrameLocked(/*enableBlending=*/false);
        service.context->IASetPrimitiveTopology(
            usePointParticles
                ? D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
                : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        service.context->VSSetShader(
            usePointParticles
                ? service.shatterPointVertexShader.Get()
                : service.shatterVertexShader.Get(),
            nullptr, 0);
        ID3D11Buffer* shatterConstantsBuffer =
            service.shatterConstants.Get();
        service.context->VSSetConstantBuffers(
            0, 1, &shatterConstantsBuffer);
        service.context->PSSetShader(service.shatterPixelShader.Get(),
                                     nullptr, 0);
        service.context->PSSetConstantBuffers(
            0, 1, &shatterConstantsBuffer);
        ID3D11ShaderResourceView* view = sourceView.Get();
        service.context->PSSetShaderResources(0, 1, &view);
        if (usePointParticles) {
            // One source pixel becomes one rasterized point. This cuts the
            // 1 px path from four vertex invocations per particle to one;
            // larger blocks retain their exact clipped-quad geometry.
            service.context->Draw(static_cast<UINT>(blockCount64), 0);
        } else {
            service.context->DrawInstanced(
                4, static_cast<UINT>(blockCount64), 0, 0);
        }
        return EndFrameLocked();
    }

    bool PresentClose(const GpuCloseFrame& frame) {
        if (!WaitForFrameSlot()) return false;
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        auto& service = g_gpuRenderService;
        if (!CanPresentLocked() || !service.closeVertexShader ||
            !service.closePixelShader || !service.closeConstants ||
            !effectFieldView) {
            return false;
        }

        const GpuCloseConstants constantsData{
            {static_cast<float>(outputWidth),
             static_cast<float>(outputHeight)},
            {static_cast<float>(inputWidth),
             static_cast<float>(inputHeight)},
            {frame.sourceOriginX, frame.sourceOriginY},
            Clamp(frame.progress, 0.0f, 1.0f),
            frame.blockSize,
        };
        service.context->UpdateSubresource(
            service.closeConstants.Get(), 0, nullptr, &constantsData, 0, 0);

        BeginFrameLocked();
        service.context->VSSetShader(service.closeVertexShader.Get(), nullptr,
                                     0);
        ID3D11Buffer* constantsBuffer = service.closeConstants.Get();
        service.context->VSSetConstantBuffers(0, 1, &constantsBuffer);
        service.context->PSSetShader(service.closePixelShader.Get(), nullptr,
                                     0);
        service.context->PSSetConstantBuffers(0, 1, &constantsBuffer);
        ID3D11ShaderResourceView* views[2] = {
            sourceView.Get(),
            effectFieldView.Get(),
        };
        service.context->PSSetShaderResources(0, 2, views);
        service.context->Draw(4, 0);
        return EndFrameLocked();
    }

    void Reset() {
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        ResetObjectsLocked();
    }

    bool IsInitialized() const { return serviceGeneration != 0; }
    uint64_t EstimatedAllocationBytes() const {
        return estimatedAllocationBytes;
    }
    void GetAdapterTelemetry(std::wstring& description,
                             uint64_t& dedicatedVideoMemory) const {
        std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
        if (serviceGeneration &&
            serviceGeneration == g_gpuRenderService.generation) {
            description = g_gpuRenderService.adapterDescription;
            dedicatedVideoMemory =
                g_gpuRenderService.adapterDedicatedVideoMemory;
        } else {
            description.clear();
            dedicatedVideoMemory = 0;
        }
    }
    bool IsFramePaced() const {
        return frameLatencyWaitableObject != nullptr;
    }
    bool UsesLayeredPresentation() const {
        return layeredPresentation;
    }
    bool ReserveFrameSlot() {
        return WaitForFrameSlot();
    }
};

static void ShutdownGpuRenderService() {
    std::lock_guard<std::mutex> lock(g_gpuRenderService.mutex);
    ReleaseGpuDeviceResourcesLocked();
    if (g_gpuRenderService.compilerModule) {
        FreeLibrary(g_gpuRenderService.compilerModule);
        g_gpuRenderService.compilerModule = nullptr;
    }
    g_gpuRenderService.retryAfterTick = 0;
}

static bool ShutdownGpuRenderServiceIfIdle(bool waitForService = true) {
    std::unique_lock<std::mutex> lock(g_gpuRenderService.mutex,
                                      std::defer_lock);
    if (waitForService) {
        lock.lock();
    } else if (!lock.try_lock()) {
        return false;
    }
    if (g_gpuRenderService.activePresenters != 0) return false;

    const bool hadResources =
        g_gpuRenderService.ready.load(std::memory_order_relaxed) ||
        g_gpuRenderService.compilerModule != nullptr;
    ReleaseGpuDeviceResourcesLocked();
    if (g_gpuRenderService.compilerModule) {
        FreeLibrary(g_gpuRenderService.compilerModule);
        g_gpuRenderService.compilerModule = nullptr;
    }
    g_gpuRenderService.retryAfterTick = 0;
    return hadResources;
}

static void ReleaseGpuRenderServiceForFinalWindow(
    HWND closingWindow, bool waitForService = true) {
    if (HasRemainingAnimationWindow(closingWindow)) return;

    CancelPendingGpuWarmup();
    if (ShutdownGpuRenderServiceIfIdle(waitForService) &&
        IsDiagnosticLoggingEnabled()) {
        Wh_Log(L"GPU renderer released: no remaining app window");
    }
}

static DWORD WINAPI FinalWindowGpuReleaseThread(LPVOID parameter) {
    const HWND closingWindow = reinterpret_cast<HWND>(parameter);
    ReleaseGpuRenderServiceForFinalWindow(closingWindow);
    return 0;
}

static void ScheduleFinalWindowGpuRelease(HWND closingWindow) {
    StartWorkerThread(FinalWindowGpuReleaseThread, closingWindow);
}

class AnimationEngine {
private:
    WindowAnimData* data = nullptr;
    int W = 0, H = 0;
    int origLeft = 0, origTop = 0;
    float origCenterX = 0.0f, origCenterY = 0.0f;
    float dockXf = 0.0f, dockY = 0.0f, neckW = 0.0f;
    float taskbarButtonCenterX = 0.0f, taskbarButtonCenterY = 0.0f;
    TaskbarEdge taskbarEdge = TaskbarEdge::Bottom;
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
    bool ghostCounted = false;
    bool usingGpuBackend = false;
    GpuEffectPresenter gpuPresenter;
    std::vector<ShatterBlock> shatterBlocks;
    // Close particles occupy a regular block grid. Their source coordinates
    // are implicit in vector order, so only effect-specific physics values
    // and a compact intrusive active-list link need to be stored per block.
    std::vector<CloseShatterParticle> closeShatterParticles;
    std::vector<CloseThanosParticle> closeThanosParticles;
    static constexpr uint32_t kNoActiveCloseParticle = UINT32_MAX;
    uint32_t firstActiveCloseParticle = kNoActiveCloseParticle;
    size_t initialCloseParticleCount = 0;
    size_t activeCloseParticleCount = 0;
    // Perlin blocks never move and only need their dissolve start time. Keep
    // source coordinates implicit in row-major order instead of storing a
    // full ShatterBlock (coordinates plus unrelated physics values) for
    // every block—especially important for 1 px fullscreen fields.
    std::vector<float> perlinStartTimes;
    std::vector<float> axisBounds;
    std::vector<float> meltLag;
    std::vector<float> meltSpeed;
    static constexpr int kMeltStripPx = 120;
    static constexpr int kMeltSubStepPx = 6;
    static constexpr float kMeltMaxLagStep = 0.08f;
    static constexpr float kMeltMaxSpeedStep = 0.12f;
    bool firstFramePending = true;
    bool firstFrameDwmSynchronized = false;
    bool telemetryEnabled = false;
    bool telemetryInitialClosing = false;
    bool telemetryInitialRising = false;
    bool telemetryLaunch = false;
    bool telemetryPresentationFailed = false;
    uint32_t telemetryFrameAttempts = 0;
    uint32_t telemetryFramesPresented = 0;
    uint32_t telemetryReversals = 0;
    LONGLONG telemetryFrequency = 0;
    LONGLONG telemetryStartTick = 0;
    LONGLONG telemetryFirstFrameTick = 0;
    LONGLONG telemetryInitializeTicks = 0;
    LONGLONG telemetryPrecalcTicks = 0;
    LONGLONG telemetryRenderTicks = 0;
    LONGLONG telemetryWorstRenderTicks = 0;
    LONGLONG telemetryCpuClearTicks = 0;
    LONGLONG telemetryCpuWorstClearTicks = 0;
    LONGLONG telemetryCpuEffectTicks = 0;
    LONGLONG telemetryCpuWorstEffectTicks = 0;
    LONGLONG telemetryCpuPresentTicks = 0;
    LONGLONG telemetryCpuWorstPresentTicks = 0;
    uint32_t telemetryCpuClearSamples = 0;
    uint32_t telemetryCpuEffectSamples = 0;
    uint32_t telemetryCpuPresentSamples = 0;
    uint64_t telemetryCpuCloseParticleVisits = 0;
    uint32_t telemetryCpuCloseParticleFrames = 0;
    LONGLONG telemetryPacingWaitTicks = 0;
    uint32_t telemetryPacingWaits = 0;
    std::wstring telemetryAdapterDescription;
    uint64_t telemetryAdapterDedicatedVideoMemory = 0;
    DWORD displayRefreshHz = 0;
    LONGLONG ReadTelemetryCounter() const {
        if (!telemetryEnabled) return 0;
        LARGE_INTEGER counter{};
        return QueryPerformanceCounter(&counter) ? counter.QuadPart : 0;
    }
    void RecordTelemetryCpuPhase(LONGLONG startTick,
                                 LONGLONG& totalTicks,
                                 LONGLONG& worstTicks,
                                 uint32_t& samples) {
        if (!telemetryEnabled || startTick <= 0) return;
        const LONGLONG endTick = ReadTelemetryCounter();
        if (endTick <= 0) return;
        const LONGLONG elapsed =
            std::max<LONGLONG>(0, endTick - startTick);
        totalTicks += elapsed;
        worstTicks = std::max(worstTicks, elapsed);
        ++samples;
    }
    void ClearCpuCanvas() {
        const LONGLONG startTick = ReadTelemetryCounter();
        memset(pBits, 0, canvasBytes);
        RecordTelemetryCpuPhase(
            startTick, telemetryCpuClearTicks,
            telemetryCpuWorstClearTicks, telemetryCpuClearSamples);
    }
    void RecordTelemetryCpuEffect(LONGLONG startTick) {
        RecordTelemetryCpuPhase(
            startTick, telemetryCpuEffectTicks,
            telemetryCpuWorstEffectTicks, telemetryCpuEffectSamples);
    }
    inline float MorphAt(float v, float tt) {
        float m = tt * (1.0f + AnimConstants::MinimizeSpread) - (1.0f - v) * AnimConstants::MinimizeSpread;
        if (m < 0.0f) m = 0.0f;
        if (m > 1.0f) m = 1.0f;
        return m * m * (3.0f - 2.0f * m);
    }
    inline void DrawParticlePixel(int srcX, int srcY, int dstX, int dstY,
                                  float alpha) {
        if (static_cast<unsigned>(srcX) >= static_cast<unsigned>(W) ||
            static_cast<unsigned>(srcY) >= static_cast<unsigned>(H) ||
            static_cast<unsigned>(dstX) >= static_cast<unsigned>(boundW) ||
            static_cast<unsigned>(dstY) >= static_cast<unsigned>(boundH)) {
            return;
        }
        const BYTE* src = srcBits + (size_t)srcY * srcStride +
                          (size_t)srcX * sizeof(uint32_t);
        BYTE* dst = pBits + (size_t)dstY * canvasStride +
                    (size_t)dstX * sizeof(uint32_t);
        if (alpha >= 0.99f) {
            memcpy(dst, src, sizeof(uint32_t));
            return;
        }
        dst[0] = (BYTE)(src[0] * alpha);
        dst[1] = (BYTE)(src[1] * alpha);
        dst[2] = (BYTE)(src[2] * alpha);
        dst[3] = (BYTE)(src[3] * alpha);
    }
    inline void DrawBlock(int srcX, int srcY, int dstX, int dstY, float alpha, int bw = -1, int bh = -1) {
        // Shatter, Thanos, and Perlin call this with the default extent. At
        // one-pixel density, bypass the generic rectangle clipping and row
        // loop for every particle while preserving the same row-major writes.
        if (blockSizeSetting == 1 && bw <= 0 && bh <= 0) {
            DrawParticlePixel(srcX, srcY, dstX, dstY, alpha);
            return;
        }
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
        const int columns =
            (W + blockSizeSetting - 1) / blockSizeSetting;
        const int rows =
            (H + blockSizeSetting - 1) / blockSizeSetting;
        const size_t expectedValues =
            static_cast<size_t>(columns) * static_cast<size_t>(rows);
        if (perlinStartTimes.size() != expectedValues) return;

        const int dstOffsetX = origLeft - boundLeft;
        const int dstOffsetY = origTop - boundTop;
        const float* startTime = perlinStartTimes.data();
        for (int srcY = 0; srcY < H; srcY += blockSizeSetting) {
            for (int srcX = 0; srcX < W;
                 srcX += blockSizeSetting, ++startTime) {
                const float localProgress =
                    (progress - *startTime) /
                    AnimConstants::PerlinLifeSpan;
                if (localProgress >= 1.0f) continue;
                DrawBlock(
                    srcX, srcY, dstOffsetX + srcX, dstOffsetY + srcY,
                    localProgress <= 0.0f ? 1.0f
                                          : 1.0f - localProgress);
            }
        }
    }
    void RenderShatter(float progress, float& fade) {
        fade = 1.0f - progress;
        if (fade < 0.0f) fade = 0.0f;
        const int columns =
            (W + blockSizeSetting - 1) / blockSizeSetting;
        const int rows =
            (H + blockSizeSetting - 1) / blockSizeSetting;
        const size_t expectedParticles =
            static_cast<size_t>(columns) * static_cast<size_t>(rows);
        if (closeShatterParticles.size() != expectedParticles ||
            (firstActiveCloseParticle != kNoActiveCloseParticle &&
             firstActiveCloseParticle >= closeShatterParticles.size())) {
            return;
        }
        if (telemetryEnabled) {
            telemetryCpuCloseParticleVisits += activeCloseParticleCount;
            ++telemetryCpuCloseParticleFrames;
        }

        const float easeOut = 1.0f - powf(1.0f - progress, 5.0f);
        const int dstOffsetX = origLeft - boundLeft;
        const int dstOffsetY = origTop - boundTop;
        size_t rowStart = 0;
        int srcY = 0;
        uint32_t previousParticle = kNoActiveCloseParticle;
        uint32_t particleIndex = firstActiveCloseParticle;
        while (particleIndex != kNoActiveCloseParticle) {
            CloseShatterParticle& particle =
                closeShatterParticles[particleIndex];
            const uint32_t nextParticle = particle.nextActive;
            while (static_cast<size_t>(particleIndex) >=
                   rowStart + static_cast<size_t>(columns)) {
                rowStart += static_cast<size_t>(columns);
                srcY += blockSizeSetting;
            }
            const int srcX = static_cast<int>(
                                 static_cast<size_t>(particleIndex) -
                                 rowStart) *
                             blockSizeSetting;
            const float travel =
                easeOut * (AnimConstants::ShatterTravelBase +
                           particle.force *
                               AnimConstants::ShatterTravelMult);
            const float velocityX =
                particle.dirX + particle.noiseX * 0.4f;
            const float velocityY =
                particle.dirY + particle.noiseY * 0.4f;
            const float dX = particle.dirX * travel +
                             particle.noiseX * travel * 0.4f;
            const float dY = particle.dirY * travel +
                             particle.noiseY * travel * 0.4f;
            const int dstBaseX =
                dstOffsetX + srcX + static_cast<int>(dX);
            const int dstBaseY =
                dstOffsetY + srcY + static_cast<int>(dY);
            const int blockW = std::min(blockSizeSetting, W - srcX);
            const int blockH = std::min(blockSizeSetting, H - srcY);
            const bool outsideLeft = dstBaseX + blockW <= 0;
            const bool outsideRight = dstBaseX >= boundW;
            const bool outsideTop = dstBaseY + blockH <= 0;
            const bool outsideBottom = dstBaseY >= boundH;
            const bool permanentlyOutside =
                (outsideLeft && velocityX <= 0.0f) ||
                (outsideRight && velocityX >= 0.0f) ||
                (outsideTop && velocityY <= 0.0f) ||
                (outsideBottom && velocityY >= 0.0f);

            if (permanentlyOutside) {
                if (previousParticle == kNoActiveCloseParticle)
                    firstActiveCloseParticle = nextParticle;
                else
                    closeShatterParticles[previousParticle].nextActive =
                        nextParticle;
                if (activeCloseParticleCount > 0)
                    --activeCloseParticleCount;
            } else {
                previousParticle = particleIndex;
                if (!outsideLeft && !outsideRight && !outsideTop &&
                    !outsideBottom) {
                    DrawBlock(srcX, srcY, dstBaseX, dstBaseY, 1.0f);
                }
            }
            particleIndex = nextParticle;
        }
    }
    void RenderThanos(float progress, float& fade) {
        fade = 1.0f;
        const int columns =
            (W + blockSizeSetting - 1) / blockSizeSetting;
        const int rows =
            (H + blockSizeSetting - 1) / blockSizeSetting;
        const size_t expectedParticles =
            static_cast<size_t>(columns) * static_cast<size_t>(rows);
        if (closeThanosParticles.size() != expectedParticles ||
            (firstActiveCloseParticle != kNoActiveCloseParticle &&
             firstActiveCloseParticle >= closeThanosParticles.size())) {
            return;
        }
        if (telemetryEnabled) {
            telemetryCpuCloseParticleVisits += activeCloseParticleCount;
            ++telemetryCpuCloseParticleFrames;
        }

        const int dstOffsetX = origLeft - boundLeft;
        const int dstOffsetY = origTop - boundTop;
        size_t rowStart = 0;
        int srcY = 0;
        uint32_t previousParticle = kNoActiveCloseParticle;
        uint32_t particleIndex = firstActiveCloseParticle;
        while (particleIndex != kNoActiveCloseParticle) {
            CloseThanosParticle& particle =
                closeThanosParticles[particleIndex];
            const uint32_t nextParticle = particle.nextActive;
            const float localProgress =
                (progress - particle.startTime) /
                AnimConstants::ThanosLifeSpan;
            if (localProgress >= 1.0f) {
                if (previousParticle == kNoActiveCloseParticle)
                    firstActiveCloseParticle = nextParticle;
                else
                    closeThanosParticles[previousParticle].nextActive =
                        nextParticle;
                if (activeCloseParticleCount > 0)
                    --activeCloseParticleCount;
                particleIndex = nextParticle;
                continue;
            }
            previousParticle = particleIndex;
            while (static_cast<size_t>(particleIndex) >=
                   rowStart + static_cast<size_t>(columns)) {
                rowStart += static_cast<size_t>(columns);
                srcY += blockSizeSetting;
            }
            const int srcX = static_cast<int>(
                                 static_cast<size_t>(particleIndex) -
                                 rowStart) *
                             blockSizeSetting;
            int dstBaseX = dstOffsetX + srcX;
            int dstBaseY = dstOffsetY + srcY;
            float currentAlphaMult = 1.0f;
            if (localProgress > 0.0f) {
                const float travel = localProgress;
                const float travelSq = travel * travel;
                dstBaseX += static_cast<int>(
                    particle.windX * travel +
                    particle.curveX * travelSq);
                dstBaseY += static_cast<int>(
                    particle.windY * travel +
                    particle.curveY * travelSq);
                currentAlphaMult = 1.0f - localProgress;
            }
            DrawBlock(srcX, srcY, dstBaseX, dstBaseY,
                      currentAlphaMult);
            particleIndex = nextParticle;
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
    QuadFrame CalculateSwellFrame(float progress) const {
        const float t = data->isRising ? (1.0f - progress) : progress;
        const float u = 1.0f - t;
        const float eased = 1.0f - u * u * u;
        const float scale = 1.0f + 0.18f * eased;
        float opacity = data->isRising
                            ? progress * progress * (3.0f - 2.0f * progress)
                            : 1.0f - eased;
        if (data->isRising && opacity < 0.04f) opacity = 0.04f;
        const int dstW = std::max(1, (int)((float)W * scale + 0.5f));
        const int dstH = std::max(1, (int)((float)H * scale + 0.5f));
        const int dstX = (origLeft - boundLeft) + (W - dstW) / 2;
        const int dstY = (origTop - boundTop) + (H - dstH) / 2;
        return {dstX, dstY, dstW, dstH, opacity};
    }
    void RenderSwell(float progress, float& fade) {
        const QuadFrame frame = CalculateSwellFrame(progress);
        fade = frame.opacity;
        DrawBlockScaled(0, 0, W, H, frame.x, frame.y, frame.width,
                        frame.height, 1.0f);
    }
    QuadFrame CalculateWin10Frame(float progress) const {
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;
        float e = 0.0f;
        float opacity = 1.0f;
        if (data->isRising) {
            float inv = 1.0f - progress;
            e = 1.0f - (inv * inv * inv);
            opacity = progress / 0.35f;
            if (opacity > 1.0f) opacity = 1.0f;
        } else {
            e = progress * progress;
            opacity = 1.0f - (progress * progress);
            if (opacity < 0.0f) opacity = 0.0f;
        }
        const float fullL = (float)origLeft;
        const float fullT = (float)origTop;
        const float fullR = (float)(origLeft + W);
        const float fullB = (float)(origTop + H);
        const bool horizontalTaskbar =
            IsHorizontalTaskbar(taskbarEdge);
        const float btnW =
            (horizontalTaskbar ? 48.0f : 40.0f) * taskbarDpiScale;
        const float btnH =
            (horizontalTaskbar ? 40.0f : 48.0f) * taskbarDpiScale;
        const float tbCenterX = taskbarButtonCenterX;
        const float tbCenterY = taskbarButtonCenterY;
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
        return {outX, outY, outW, outH, opacity};
    }
    void RenderWin10MinRestore(float progress, float& fade) {
        const QuadFrame frame = CalculateWin10Frame(progress);
        fade = frame.opacity;
        DrawBlockScaled(0, 0, W, H, frame.x, frame.y, frame.width,
                        frame.height, 1.0f);
    }
    void RenderMinimizeRestore(float progress, float& fade) {
        float tt = data->isRising ? (1.0f - progress) : progress;
        if (tt > 0.8f) fade = (1.0f - tt) / 0.2f;
        if (fade < 0.0f) fade = 0.0f;
        if (fade > 1.0f) fade = 1.0f;
        const bool horizontalTaskbar =
            IsHorizontalTaskbar(taskbarEdge);
        const bool leadingEdge = taskbarEdge == TaskbarEdge::Top ||
                                 taskbarEdge == TaskbarEdge::Left;
        const int axisSize = horizontalTaskbar ? H : W;
        for (int k = 0; k <= axisSize; ++k) {
            const float axis = static_cast<float>(k) /
                               static_cast<float>(axisSize);
            const float edgeCoordinate = leadingEdge ? 1.0f - axis : axis;
            const float e = MorphAt(edgeCoordinate, tt);
            const float sourceAxis = horizontalTaskbar
                ? static_cast<float>(origTop) + H * axis
                : static_cast<float>(origLeft) + W * axis;
            const float dockAxis = horizontalTaskbar ? dockY : dockXf;
            axisBounds[k] = sourceAxis + (dockAxis - sourceAxis) * e;
        }

        int segment = 0;
        const int canvasAxisSize = horizontalTaskbar ? boundH : boundW;
        for (int canvasAxis = 0; canvasAxis < canvasAxisSize; ++canvasAxis) {
            const float screenAxis = static_cast<float>(
                                         canvasAxis +
                                         (horizontalTaskbar ? boundTop
                                                            : boundLeft)) +
                                     0.5f;
            if (screenAxis < axisBounds.front() ||
                screenAxis >= axisBounds.back()) {
                continue;
            }
            while (segment < axisSize - 1 &&
                   axisBounds[segment + 1] <= screenAxis) {
                ++segment;
            }
            const float segmentSize =
                axisBounds[segment + 1] - axisBounds[segment];
            const float fraction = segmentSize > 1e-4f
                ? (screenAxis - axisBounds[segment]) / segmentSize
                : 0.0f;
            const float axis =
                (static_cast<float>(segment) + fraction) /
                static_cast<float>(axisSize);
            const float edgeCoordinate = leadingEdge ? 1.0f - axis : axis;
            const float morph = MorphAt(edgeCoordinate, tt);
            const float crossSourceSize =
                static_cast<float>(horizontalTaskbar ? W : H);
            const float crossSize =
                std::max(1.0f,
                         crossSourceSize +
                             (neckW - crossSourceSize) * morph);
            const float sourceCenter =
                horizontalTaskbar ? origCenterX : origCenterY;
            const float dockCenter = horizontalTaskbar ? dockXf : dockY;
            const float crossCenter =
                sourceCenter + (dockCenter - sourceCenter) * morph;
            const float crossCanvasStart =
                crossCenter - crossSize * 0.5f -
                static_cast<float>(horizontalTaskbar ? boundLeft
                                                     : boundTop);
            int crossStart = std::max(0, static_cast<int>(crossCanvasStart));
            int crossEnd = std::min(
                horizontalTaskbar ? boundW : boundH,
                static_cast<int>(crossCanvasStart + crossSize) + 1);
            const float inverseCrossSize = 1.0f / crossSize;

            if (horizontalTaskbar) {
                int sourceY = Clamp(
                    static_cast<int>(axis * H), 0, H - 1);
                const BYTE* sourceRow =
                    srcBits + static_cast<size_t>(sourceY) * srcStride;
                uint32_t* destinationRow = reinterpret_cast<uint32_t*>(
                    pBits + static_cast<size_t>(canvasAxis) * canvasStride);
                for (int x = crossStart; x < crossEnd; ++x) {
                    const float u =
                        (static_cast<float>(x) + 0.5f -
                         crossCanvasStart) * inverseCrossSize;
                    if (u < 0.0f || u >= 1.0f) continue;
                    const int sourceX = Clamp(
                        static_cast<int>(u * W), 0, W - 1);
                    destinationRow[x] =
                        reinterpret_cast<const uint32_t*>(sourceRow)[sourceX];
                }
            } else {
                const int sourceX = Clamp(
                    static_cast<int>(axis * W), 0, W - 1);
                for (int y = crossStart; y < crossEnd; ++y) {
                    const float v =
                        (static_cast<float>(y) + 0.5f -
                         crossCanvasStart) * inverseCrossSize;
                    if (v < 0.0f || v >= 1.0f) continue;
                    const int sourceY = Clamp(
                        static_cast<int>(v * H), 0, H - 1);
                    const uint32_t* sourceRow =
                        reinterpret_cast<const uint32_t*>(
                            srcBits + static_cast<size_t>(sourceY) *
                                          srcStride);
                    uint32_t* destinationRow =
                        reinterpret_cast<uint32_t*>(
                            pBits + static_cast<size_t>(y) * canvasStride);
                    destinationRow[canvasAxis] = sourceRow[sourceX];
                }
            }
        }
    }
    void SetAnimationWindowCloak(BOOL cloak) {
        if (!data || !IsWindow(data->hRealWnd)) return;
        if (!cloak && data->showDesktopAnimationToken &&
            reinterpret_cast<ULONG_PTR>(GetPropW(
                data->hRealWnd, kPropShowDesktopRestorePrepared)) ==
                data->showDesktopAnimationToken) {
            // Disarm target-process MINIMIZEEND callbacks before the endpoint
            // removes the real window's animation-owned cloak.
            RemovePropW(data->hRealWnd, kPropShowDesktopRestorePrepared);
        }
        SetWindowCloak(data->hRealWnd, cloak);
        UpdateShowDesktopOwnedSurfaceCloaks(
            data->hRealWnd, data->showDesktopAnimationToken, cloak);
    }
    void MaintainShowDesktopRestoreCloak() {
        if (!data || !data->isRising || !data->hiddenByCloak ||
            !data->showDesktopAnimationToken ||
            !IsWindow(data->hRealWnd) ||
            reinterpret_cast<ULONG_PTR>(GetPropW(
                data->hRealWnd, kPropShowDesktopAnimationOwner)) !=
                data->showDesktopAnimationToken) {
            return;
        }

        // Native Win+D restore and Chromium/Electron can update the real
        // surface asynchronously after the first ghost frame. Guard the main
        // HWND for the entire rising animation, not just its initial handoff.
        // This is one inexpensive DWM attribute write per displayed frame and
        // applies only to the single optimized Show Desktop leader.
        SetWindowCloak(data->hRealWnd, TRUE);
    }
    void WaitForLocalShowDesktopCloakRelease() {
        if (!data || !data->showDesktopAnimationToken ||
            !IsWindow(data->hRealWnd)) {
            return;
        }
        const DWORD deadline = GetTickCount() + 100;
        while (reinterpret_cast<ULONG_PTR>(GetPropW(
                   data->hRealWnd, kPropShowDesktopLocalCloakWatch)) ==
                   data->showDesktopAnimationToken &&
               !g_unloading.load(std::memory_order_relaxed) &&
               static_cast<LONG>(GetTickCount() - deadline) < 0) {
            Sleep(1);
        }
        if (reinterpret_cast<ULONG_PTR>(GetPropW(
                data->hRealWnd, kPropShowDesktopLocalCloakWatch)) ==
                data->showDesktopAnimationToken &&
            IsDiagnosticLoggingEnabled()) {
            Wh_Log(L"Show Desktop local uncloak wait timed out hwnd=%p",
                   data->hRealWnd);
        }
    }
public:
    AnimationEngine(WindowAnimData* d) {
        data = d;
        telemetryEnabled =
            g_performanceTelemetry.load(std::memory_order_relaxed);
        telemetryInitialClosing = data->isClosing != FALSE;
        telemetryInitialRising = data->isRising != FALSE;
        telemetryLaunch = data->launchAnimationToken != 0;
        if (telemetryEnabled) {
            LARGE_INTEGER frequency{};
            LARGE_INTEGER start{};
            if (QueryPerformanceFrequency(&frequency) &&
                QueryPerformanceCounter(&start)) {
                telemetryFrequency = frequency.QuadPart;
                telemetryStartTick = start.QuadPart;
            } else {
                telemetryEnabled = false;
            }
        }
        W = data->width;
        H = data->height;
        origLeft = data->targetRect.left;
        origTop  = data->targetRect.top;
        origCenterX = (float)origLeft + W * 0.5f;
        origCenterY = (float)origTop + H * 0.5f;
        HMONITOR hMon = MonitorFromWindow(data->hRealWnd, MONITOR_DEFAULTTONEAREST);
        if (hMon) {
            MONITORINFOEXW monitorInfo{};
            monitorInfo.cbSize = sizeof(monitorInfo);
            if (GetMonitorInfoW(
                    hMon,
                    reinterpret_cast<MONITORINFO*>(&monitorInfo))) {
                DEVMODEW displayMode{};
                displayMode.dmSize = sizeof(displayMode);
                if (EnumDisplaySettingsW(
                        monitorInfo.szDevice, ENUM_CURRENT_SETTINGS,
                        &displayMode) &&
                    (displayMode.dmFields & DM_DISPLAYFREQUENCY) &&
                    displayMode.dmDisplayFrequency > 1) {
                    displayRefreshHz = displayMode.dmDisplayFrequency;
                }
            }
        }
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
        const TaskbarGeometry taskbarGeometry = GetTaskbarGeometry(
            data->hRealWnd, hTray, mmi,
            data->requestedUnhide || data->deferredMinimize);
        taskbarEdge = taskbarGeometry.edge;
        const bool horizontalTaskbar =
            IsHorizontalTaskbar(taskbarEdge);
        const float buttonWidth =
            (horizontalTaskbar ? 48.0f : 40.0f) * taskbarDpiScale;
        const float buttonHeight =
            (horizontalTaskbar ? 40.0f : 48.0f) * taskbarDpiScale;
        const float buttonInset = 2.0f * taskbarDpiScale;
        if (horizontalTaskbar) {
            dockXf = static_cast<float>(
                Clamp(data->targetDockX, monLeft, monRight));
            dockY = Clamp(taskbarGeometry.innerEdge,
                          static_cast<float>(monTop),
                          static_cast<float>(monBottom));
            taskbarButtonCenterX = dockXf;
            taskbarButtonCenterY =
                dockY + (taskbarEdge == TaskbarEdge::Bottom
                             ? buttonInset + buttonHeight * 0.5f
                             : -buttonInset - buttonHeight * 0.5f);
        } else {
            dockXf = Clamp(taskbarGeometry.innerEdge,
                           static_cast<float>(monLeft),
                           static_cast<float>(monRight));
            dockY = static_cast<float>(
                Clamp(data->targetDockY, monTop, monBottom));
            taskbarButtonCenterX =
                dockXf + (taskbarEdge == TaskbarEdge::Right
                              ? buttonInset + buttonWidth * 0.5f
                              : -buttonInset - buttonWidth * 0.5f);
            taskbarButtonCenterY = dockY;
        }
        taskbarButtonCenterX = Clamp(
            taskbarButtonCenterX, static_cast<float>(monLeft),
            static_cast<float>(monRight));
        taskbarButtonCenterY = Clamp(
            taskbarButtonCenterY, static_cast<float>(monTop),
            static_cast<float>(monBottom));
        neckW = Clamp((horizontalTaskbar ? W : H) * 0.03f,
                      12.0f, 60.0f);
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
            // RenderWin10MinRestore finishes at a 1.6x taskbar-button box, not
            // at the button's center point. Include that complete endpoint in
            // the ghost canvas so the last frames aren't clipped on any edge.
            const float buttonW =
                (horizontalTaskbar ? 48.0f : 40.0f) * taskbarDpiScale;
            const float buttonH =
                (horizontalTaskbar ? 40.0f : 48.0f) * taskbarDpiScale;
            const float targetHalfW = buttonW * 0.8f;
            const float targetHalfH = buttonH * 0.8f;
            const int targetLeft =
                static_cast<int>(floorf(
                    taskbarButtonCenterX - targetHalfW));
            const int targetRight =
                static_cast<int>(ceilf(
                    taskbarButtonCenterX + targetHalfW));
            const int targetTop =
                static_cast<int>(floorf(
                    taskbarButtonCenterY - targetHalfH));
            const int targetBottom =
                static_cast<int>(ceilf(
                    taskbarButtonCenterY + targetHalfH));
            boundLeft =
                std::max(monLeft, std::min(origLeft, targetLeft) - pad);
            const int boundRight = std::min(
                monRight, std::max(origLeft + W, targetRight) + pad);
            boundTop =
                std::max(monTop, std::min(origTop, targetTop) - pad);
            const int boundBottom = std::min(
                monBottom, std::max(origTop + H, targetBottom) + pad);
            boundW = boundRight - boundLeft;
            boundH = boundBottom - boundTop;
        } else {
            // Genie bends rows for horizontal taskbars and columns for
            // vertical taskbars. Its canvas is the exact union of the source
            // rectangle and final neck, plus a tiny edge-side safety pad.
            constexpr int genieBoundsSafetyPad = 2;
            constexpr int genieEdgePad = 40;
            int boundRight = origLeft + W;
            int boundBottom = origTop + H;
            if (horizontalTaskbar) {
                const int targetLeft = static_cast<int>(
                    floorf(dockXf - neckW * 0.5f));
                const int targetRight = static_cast<int>(
                    ceilf(dockXf + neckW * 0.5f));
                boundLeft = std::max(
                    monLeft,
                    std::min(origLeft, targetLeft) - genieBoundsSafetyPad);
                boundRight = std::min(
                    monRight,
                    std::max(origLeft + W, targetRight) +
                        genieBoundsSafetyPad);
                boundTop = std::max(
                    monTop,
                    std::min(origTop,
                             static_cast<int>(floorf(dockY)) -
                                 (taskbarEdge == TaskbarEdge::Top
                                      ? genieEdgePad
                                      : 0)));
                boundBottom = std::min(
                    monBottom,
                    std::max(origTop + H,
                             static_cast<int>(ceilf(dockY)) +
                                 (taskbarEdge == TaskbarEdge::Bottom
                                      ? genieEdgePad
                                      : 0)));
            } else {
                const int targetTop = static_cast<int>(
                    floorf(dockY - neckW * 0.5f));
                const int targetBottom = static_cast<int>(
                    ceilf(dockY + neckW * 0.5f));
                boundTop = std::max(
                    monTop,
                    std::min(origTop, targetTop) - genieBoundsSafetyPad);
                boundBottom = std::min(
                    monBottom,
                    std::max(origTop + H, targetBottom) +
                        genieBoundsSafetyPad);
                boundLeft = std::max(
                    monLeft,
                    std::min(origLeft,
                             static_cast<int>(floorf(dockXf)) -
                                 (taskbarEdge == TaskbarEdge::Left
                                      ? genieEdgePad
                                      : 0)));
                boundRight = std::min(
                    monRight,
                    std::max(origLeft + W,
                             static_cast<int>(ceilf(dockXf)) +
                                 (taskbarEdge == TaskbarEdge::Right
                                      ? genieEdgePad
                                      : 0)));
            }
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
    bool CreateGhostWindow(bool gpuGhost,
                           bool gpuLayeredPresentation = false) {
        const bool layeredGhost =
            !gpuGhost || gpuLayeredPresentation;
        const DWORD ghostExStyle =
            (layeredGhost ? WS_EX_LAYERED : WS_EX_NOREDIRECTIONBITMAP) |
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT;
        hGhost = CreateWindowExW(
            ghostExStyle,
            L"STATIC", NULL, WS_POPUP,
            boundLeft, boundTop, boundW, boundH,
            NULL, NULL, NULL, NULL);
        if (!hGhost) return false;
        g_animationGhostCount.fetch_add(1, std::memory_order_acq_rel);
        ghostCounted = true;
        if (keepGhostBelowTaskbar) {
            SetPropW(hGhost, L"NonRudeHWND", reinterpret_cast<HANDLE>(TRUE));
        }
        return true;
    }
    void DestroyGhostForBackendSwitch() {
        if (hGhost) {
            DestroyWindow_Original(hGhost);
            hGhost = nullptr;
        }
        if (ghostCounted) {
            g_animationGhostCount.fetch_sub(1, std::memory_order_acq_rel);
            ghostCounted = false;
        }
    }
    bool InitializeCpuRenderer() {
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
        SelectObject(hSrcDC, hOldSrc);
        hOldSrc = nullptr;
        DeleteDC(hSrcDC);
        hSrcDC = nullptr;
        if (data->isClosing && data->hBitmap) {
            // The CPU renderer now owns an independent source DIB. A close
            // cannot reverse into a restore, so release the capture promptly.
            DeleteObject(data->hBitmap);
            data->hBitmap = nullptr;
            data->pBits = nullptr;
        }
        srcStride = W * 4;
        hCanvas = CreateDib32(hScreenDC, boundW, boundH, (void**)&pBits);
        if (!hCanvas || !pBits) return false;
        hCanvasDC = CreateCompatibleDC(hScreenDC);
        if (!hCanvasDC) return false;
        hOldCanvas = (HBITMAP)SelectObject(hCanvasDC, hCanvas);
        if (!hOldCanvas || hOldCanvas == HGDI_ERROR) return false;
        canvasStride = boundW * 4;
        canvasBytes = (size_t)boundW * 4 * boundH;
        if (!data->isClosing && minRestoreEffect == 0) {
            axisBounds.resize(
                (IsHorizontalTaskbar(taskbarEdge) ? H : W) + 1);
        }
        return true;
    }
    bool Initialize() {
        hScreenDC = GetDC(NULL);
        if (!hScreenDC) return false;

        const uint32_t requiredGpuPipelines = RequiredGpuPipelines(
            data->isClosing != FALSE,
            data->isClosing ? closeEffect : minRestoreEffect);
        const bool gpuSupportedEffect =
            requiredGpuPipelines != GpuPipelineNone;
        const bool gpuCloseCompatibilityFallback =
            data->isClosing &&
            RequiresCpuClosePresentation(data->hRealWnd);
        const bool gpuLaunchCompatibilityFallback =
            data->launchAnimationToken &&
            RequiresCpuLaunchPresentation(data->hRealWnd);
        const bool gpuProcessCompatibilityFallback =
            RequiresCpuAnimationRendererProcess();
        const bool hybridGpuSelected =
            data->isClosing
                ? ShouldUseHybridGpuClose(closeEffect, blockSizeSetting, W, H)
                : data->isRising != FALSE;
        const bool gpuToggleEnabled =
            data->isClosing
                ? g_closeGpuAcceleration.load(std::memory_order_relaxed)
                : g_gpuAcceleration.load(std::memory_order_relaxed);
        const bool gpuConfigured =
            hybridGpuSelected && gpuSupportedEffect &&
            !gpuCloseCompatibilityFallback &&
            !gpuLaunchCompatibilityFallback &&
            !gpuProcessCompatibilityFallback &&
            !data->fastShowDesktopStart &&
            gpuToggleEnabled &&
            !GetSystemMetrics(SM_REMOTESESSION);
        if (telemetryEnabled && gpuToggleEnabled && gpuSupportedEffect &&
            !hybridGpuSelected) {
            if (data->isClosing) {
                Wh_Log(L"Animation renderer=CPU hybrid selection hwnd=%p "
                       L"style=%d source=%dx%d block_px=%d",
                       data->hRealWnd, closeEffect, W, H,
                       blockSizeSetting);
            } else {
                Wh_Log(L"Animation renderer=CPU hybrid selection hwnd=%p "
                       L"style=%d standalone minimize",
                       data->hRealWnd, minRestoreEffect);
            }
        }
        if (telemetryEnabled && hybridGpuSelected &&
            gpuCloseCompatibilityFallback &&
            gpuSupportedEffect && gpuToggleEnabled) {
            Wh_Log(L"Animation renderer=CPU compatibility fallback hwnd=%p "
                   L"style=%d backdrop-sensitive close",
                   data->hRealWnd, closeEffect);
        }
        if (telemetryEnabled && hybridGpuSelected &&
            gpuLaunchCompatibilityFallback &&
            gpuSupportedEffect && gpuToggleEnabled) {
            Wh_Log(L"Animation renderer=CPU compatibility fallback hwnd=%p "
                   L"style=%d utility launch",
                   data->hRealWnd, minRestoreEffect);
        }
        if (telemetryEnabled && hybridGpuSelected &&
            gpuProcessCompatibilityFallback &&
            gpuSupportedEffect && gpuToggleEnabled) {
            Wh_Log(L"Animation renderer=CPU compatibility fallback hwnd=%p "
                   L"GPU-incompatible process",
                   data->hRealWnd);
        }
        if (gpuConfigured) {
            if (IsGpuServiceReady(requiredGpuPipelines)) {
                // Hybrid selection reaches this path for a rising
                // restore/launch or a qualifying dense close. Rising effects
                // use DirectComposition; closes use the atomic layered GPU
                // presenter. Standalone minimizes were intentionally routed
                // to CPU above. A live reversal keeps the presenter on which
                // it began instead of swapping resources mid-animation.
                const bool gpuLayeredPresentation =
                    data->isClosing != FALSE;
                if (CreateGhostWindow(
                        /*gpuGhost=*/true,
                        gpuLayeredPresentation) &&
                    gpuPresenter.Initialize(
                        hGhost, hScreenDC, data->pBits, W, H, boundW, boundH,
                        // Restore swap chains can use DXGI's waitable
                        // frame-latency object. Layered closes use their
                        // atomic HWND presentation path instead.
                        /*framePaced=*/!gpuLayeredPresentation,
                        /*useLayeredPresentation=*/gpuLayeredPresentation,
                        requiredGpuPipelines)) {
                    bool effectFieldReady = true;
                    if (!data->isClosing && minRestoreEffect >= 1 &&
                        minRestoreEffect <= 3) {
                        int fieldStep = 1;
                        int blockSize = 1;
                        if (minRestoreEffect == 1) {
                            fieldStep =
                                std::max(8, std::min(W, H) / 48);
                            blockSize = std::max(4, fieldStep / 2);
                        } else {
                            blockSize = Clamp(
                                std::min(W, H) / 96, 2, 5);
                        }
                        effectFieldReady =
                            gpuPresenter.InitializeInplaceField(
                                static_cast<uint32_t>(minRestoreEffect),
                                blockSize, fieldStep);
                    } else if (data->isClosing && closeEffect == 2) {
                        effectFieldReady =
                            gpuPresenter.InitializeInplaceField(
                                8, blockSizeSetting, blockSizeSetting);
                    }
                    if (effectFieldReady) {
                        usingGpuBackend = true;
                        if (telemetryEnabled) {
                            gpuPresenter.GetAdapterTelemetry(
                                telemetryAdapterDescription,
                                telemetryAdapterDedicatedVideoMemory);
                            for (wchar_t& character :
                                 telemetryAdapterDescription) {
                                if (iswspace(character)) character = L'_';
                            }
                        }
                        if (telemetryEnabled) {
                            Wh_Log(
                                L"Animation renderer=GPU presenter=%s hwnd=%p "
                                L"style=%d",
                                gpuPresenter.UsesLayeredPresentation()
                                    ? L"layered"
                                    : L"composition",
                                data->hRealWnd,
                                data->isClosing ? closeEffect
                                                : minRestoreEffect);
                        }
                        return true;
                    }
                }
                gpuPresenter.Reset();
                DestroyGhostForBackendSwitch();
            } else {
                // A cold device/shader/swap-chain path can exceed the shell's
                // first-frame handshake. Warm it on a separate registered
                // worker and keep this animation on the proven CPU renderer.
                RequestGpuWarmup(requiredGpuPipelines);
            }
        }

        if (!CreateGhostWindow(/*gpuGhost=*/false)) return false;
        usingGpuBackend = false;
        return InitializeCpuRenderer();
    }
    bool IsUsingGpuBackend() const {
        return usingGpuBackend;
    }
    bool IsPerformanceTelemetryEnabled() const {
        return telemetryEnabled;
    }
    void SetTelemetryInitializeTicks(LONGLONG ticks) {
        if (telemetryEnabled) telemetryInitializeTicks = std::max<LONGLONG>(0, ticks);
    }
    void SetTelemetryPrecalcTicks(LONGLONG ticks) {
        if (telemetryEnabled) telemetryPrecalcTicks = std::max<LONGLONG>(0, ticks);
    }
    void RecordTelemetryFrameSample(LONGLONG startTick, bool presented) {
        if (!telemetryEnabled || startTick <= 0) return;
        LARGE_INTEGER end{};
        QueryPerformanceCounter(&end);
        const LONGLONG elapsed = std::max<LONGLONG>(
            0, end.QuadPart - startTick);
        ++telemetryFrameAttempts;
        if (presented) {
            ++telemetryFramesPresented;
        } else {
            telemetryPresentationFailed = true;
        }
        telemetryRenderTicks += elapsed;
        telemetryWorstRenderTicks =
            std::max(telemetryWorstRenderTicks, elapsed);
    }
    bool TelemetryPresentationFailed() const {
        return telemetryPresentationFailed;
    }
    void LogPerformanceTelemetry(PCWSTR status) const {
        if (!telemetryEnabled || !telemetryFrequency ||
            !telemetryStartTick) {
            return;
        }
        LARGE_INTEGER now{};
        QueryPerformanceCounter(&now);
        const auto ticksToMs = [this](LONGLONG ticks) {
            return static_cast<double>(ticks) * 1000.0 /
                   static_cast<double>(telemetryFrequency);
        };
        const PCWSTR kind = telemetryInitialClosing
                                ? L"close"
                                : telemetryLaunch
                                      ? L"launch"
                                      : telemetryInitialRising ? L"restore"
                                                               : L"minimize";
        const int effectStyle = telemetryInitialClosing
                                    ? closeEffect
                                    : minRestoreEffect;
        PCWSTR effect = L"unknown";
        if (telemetryInitialClosing) {
            static constexpr PCWSTR closeNames[] = {
                L"shatter", L"thanos", L"perlin", L"glitch",
                L"crt", L"melt"};
            if (effectStyle >= 0 && effectStyle <
                                        static_cast<int>(ARRAYSIZE(closeNames))) {
                effect = closeNames[effectStyle];
            }
        } else {
            static constexpr PCWSTR minRestoreNames[] = {
                L"genie", L"ink", L"scorch", L"splinter",
                L"mirage", L"stipple", L"swell", L"windows10"};
            if (effectStyle >= 0 && effectStyle < static_cast<int>(
                                                    ARRAYSIZE(minRestoreNames))) {
                effect = minRestoreNames[effectStyle];
            }
        }
        uint64_t blockCount = 0;
        if (telemetryInitialClosing &&
            (effectStyle >= 0 && effectStyle <= 2)) {
            blockCount =
                static_cast<uint64_t>((W + blockSizeSetting - 1) /
                                      blockSizeSetting) *
                static_cast<uint64_t>((H + blockSizeSetting - 1) /
                                      blockSizeSetting);
        }
        const double firstFrameMs = telemetryFirstFrameTick
                                        ? ticksToMs(telemetryFirstFrameTick -
                                                    telemetryStartTick)
                                        : -1.0;
        const double averageFrameMs = telemetryFrameAttempts
                                          ? ticksToMs(telemetryRenderTicks) /
                                                telemetryFrameAttempts
                                          : 0.0;
        const double averageCpuClearMs = telemetryCpuClearSamples
                                             ? ticksToMs(telemetryCpuClearTicks) /
                                                   telemetryCpuClearSamples
                                             : 0.0;
        const double averageCpuEffectMs = telemetryCpuEffectSamples
                                              ? ticksToMs(telemetryCpuEffectTicks) /
                                                    telemetryCpuEffectSamples
                                              : 0.0;
        const double averageCpuPresentMs = telemetryCpuPresentSamples
                                               ? ticksToMs(telemetryCpuPresentTicks) /
                                                     telemetryCpuPresentSamples
                                               : 0.0;
        const double averagePacingWaitMs = telemetryPacingWaits
                                               ? ticksToMs(
                                                     telemetryPacingWaitTicks) /
                                                     telemetryPacingWaits
                                               : 0.0;
        const double averageActiveParticlePercent =
            initialCloseParticleCount && telemetryCpuCloseParticleFrames
                ? static_cast<double>(telemetryCpuCloseParticleVisits) *
                      100.0 /
                      (static_cast<double>(initialCloseParticleCount) *
                       telemetryCpuCloseParticleFrames)
                : 0.0;
        const double gpuMiB = usingGpuBackend
                                  ? static_cast<double>(
                                        gpuPresenter.EstimatedAllocationBytes()) /
                                        (1024.0 * 1024.0)
                                  : 0.0;
        const PCWSTR adapter = usingGpuBackend
                                   ? telemetryAdapterDescription.empty()
                                         ? L"unknown"
                                         : telemetryAdapterDescription.c_str()
                                   : L"cpu";
        PCWSTR particlePath = L"none";
        if (telemetryInitialClosing) {
            if (effectStyle == 2) {
                particlePath = usingGpuBackend ? L"field"
                                               : L"compact-field";
            } else if (effectStyle == 0 || effectStyle == 1) {
                particlePath = usingGpuBackend
                                   ? blockSizeSetting == 1 ? L"point"
                                                           : L"quad"
                                   : L"active-compact";
            }
        }
        Wh_Log(
            L"Perf summary hwnd=%p kind=%s effect=%s style=%d renderer=%s "
            L"presenter=%s adapter=%s adapter_vram_mib=%.0f refresh_hz=%lu "
            L"source=%dx%d canvas=%dx%d target_ms=%.0f gpu_mib=%.2f "
            L"blocks=%.0f block_px=%d particle_path=%s "
            L"init_ms=%.3f precalc_ms=%.3f first_ms=%.3f "
            L"frames=%u/%u avg_frame_ms=%.3f worst_frame_ms=%.3f "
            L"cpu_samples=%u/%u/%u avg_clear_ms=%.3f "
            L"avg_effect_ms=%.3f avg_present_ms=%.3f "
            L"worst_clear_ms=%.3f worst_effect_ms=%.3f "
            L"worst_present_ms=%.3f "
            L"active_avg_pct=%.1f active_final=%zu/%zu "
            L"pace_wait_ms=%.3f reversals=%u total_ms=%.3f status=%s",
            data->hRealWnd, kind, effect, effectStyle,
            usingGpuBackend ? L"GPU" : L"CPU",
            usingGpuBackend
                ? gpuPresenter.UsesLayeredPresentation()
                      ? L"layered"
                      : L"composition"
                : L"cpu",
            adapter,
            static_cast<double>(telemetryAdapterDedicatedVideoMemory) /
                (1024.0 * 1024.0),
            static_cast<unsigned long>(displayRefreshHz),
            W, H, boundW, boundH,
            totalMs, gpuMiB, static_cast<double>(blockCount),
            blockSizeSetting, particlePath,
            ticksToMs(telemetryInitializeTicks),
            ticksToMs(telemetryPrecalcTicks), firstFrameMs,
            telemetryFramesPresented, telemetryFrameAttempts,
            averageFrameMs, ticksToMs(telemetryWorstRenderTicks),
            telemetryCpuClearSamples, telemetryCpuEffectSamples,
            telemetryCpuPresentSamples, averageCpuClearMs,
            averageCpuEffectMs, averageCpuPresentMs,
            ticksToMs(telemetryCpuWorstClearTicks),
            ticksToMs(telemetryCpuWorstEffectTicks),
            ticksToMs(telemetryCpuWorstPresentTicks),
            averageActiveParticlePercent, activeCloseParticleCount,
            initialCloseParticleCount, averagePacingWaitMs,
            telemetryReversals,
            ticksToMs(now.QuadPart - telemetryStartTick),
            status ? status : L"unknown");
    }
    void OnFramePresented(bool synchronizeBeforeSignal) {
        if (!firstFramePending) return;
        ShowGhostNoActivate();
        if ((!data->isRising || data->hiddenByCloak) &&
            IsWindow(data->hRealWnd)) {
            // Reassert the real-window side of the handoff when the first ghost
            // frame is ready. This also covers cross-process Win+D restores,
            // whose framework can rebuild its DWM surface during native restore.
            SetAnimationWindowCloak(TRUE);
            data->hiddenByCloak = TRUE;
        }
        const bool synchronizeRestoreHandoff =
            data->isRising && data->hiddenByCloak;
        if (synchronizeBeforeSignal || synchronizeRestoreHandoff) {
            // Commit the complete source-to-ghost visibility handoff before
            // signaling the initiating hook (when one is waiting). Without
            // this flush a rising real window can leak below its first ghost.
            FlushDwmOrYield();
            firstFrameDwmSynchronized = true;
        }
        if (telemetryEnabled && !telemetryFirstFrameTick) {
            LARGE_INTEGER firstFrame{};
            QueryPerformanceCounter(&firstFrame);
            telemetryFirstFrameTick = firstFrame.QuadPart;
        }
        firstFramePending = false;
        if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
    }
    bool PresentCanvas(float fade) {
        const LONGLONG presentStartTick = ReadTelemetryCounter();
        POINT ptDst = {boundLeft, boundTop};
        SIZE sz = {boundW, boundH};
        POINT ptSrc = {0, 0};
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = (BYTE)(255.0f * fade);
        bf.AlphaFormat = AC_SRC_ALPHA;
        UpdateLayeredWindow(hGhost, hScreenDC, &ptDst, &sz, hCanvasDC, &ptSrc, 0, &bf, ULW_ALPHA);
        const bool synchronizePilotHandoff =
            !data->fastShowDesktopStart && !data->isClosing &&
            (minRestoreEffect == 6 || minRestoreEffect == 7);
        OnFramePresented(synchronizePilotHandoff);
        RecordTelemetryCpuPhase(
            presentStartTick, telemetryCpuPresentTicks,
            telemetryCpuWorstPresentTicks, telemetryCpuPresentSamples);
        return true;
    }
    bool PresentGpuQuad(const QuadFrame& frame) {
        if (!gpuPresenter.PresentQuad(frame)) {
            return false;
        }
        OnFramePresented(/*synchronizeBeforeSignal=*/true);
        return true;
    }
    bool PresentGpuGenie(float progress) {
        const float morphTime =
            data->isRising ? (1.0f - progress) : progress;
        float opacity = 1.0f;
        if (morphTime > 0.8f) {
            opacity = (1.0f - morphTime) / 0.2f;
        }
        const GpuGenieFrame frame{
            static_cast<float>(origLeft - boundLeft),
            static_cast<float>(origTop - boundTop),
            dockXf - static_cast<float>(boundLeft),
            dockY - static_cast<float>(boundTop),
            neckW,
            morphTime,
            Clamp(opacity, 0.0f, 1.0f),
            taskbarEdge,
        };
        if (!gpuPresenter.PresentGenie(frame)) {
            return false;
        }
        OnFramePresented(/*synchronizeBeforeSignal=*/true);
        return true;
    }
    bool PresentGpuInplace(float progress) {
        float visibleProgress =
            data->isRising ? progress : (1.0f - progress);
        if (minRestoreEffect != 1) {
            visibleProgress = visibleProgress * visibleProgress *
                              (3.0f - 2.0f * visibleProgress);
        }
        int blockSize = 1;
        if (minRestoreEffect == 1) {
            const int fieldStep =
                std::max(8, std::min(W, H) / 48);
            blockSize = std::max(4, fieldStep / 2);
        } else if (minRestoreEffect == 2 || minRestoreEffect == 3) {
            blockSize = Clamp(std::min(W, H) / 96, 2, 5);
        } else {
            const int divisor = minRestoreEffect == 4 ? 96 : 110;
            blockSize = Clamp(std::min(W, H) / divisor, 2, 4);
        }
        const GpuInplaceFrame frame{
            static_cast<float>(origLeft - boundLeft),
            static_cast<float>(origTop - boundTop),
            visibleProgress,
            blockSize,
            static_cast<uint32_t>(minRestoreEffect),
        };
        if (!gpuPresenter.PresentInplace(frame)) {
            return false;
        }
        OnFramePresented(/*synchronizeBeforeSignal=*/true);
        return true;
    }
    bool PresentGpuClose(float progress) {
        const GpuCloseFrame frame{
            static_cast<float>(origLeft - boundLeft),
            static_cast<float>(origTop - boundTop),
            progress,
            static_cast<float>(blockSizeSetting),
        };
        const bool presented = closeEffect == 1
                                   ? gpuPresenter.PresentShatter(frame)
                                   : gpuPresenter.PresentClose(frame);
        if (!presented) {
            return false;
        }
        OnFramePresented(/*synchronizeBeforeSignal=*/true);
        return true;
    }
    bool ShowInkBootstrapFrame() {
        if (usingGpuBackend) {
            if (data->isClosing) {
                return PresentGpuQuad(QuadFrame{
                    origLeft - boundLeft,
                    origTop - boundTop,
                    W,
                    H,
                    1.0f,
                });
            }
            if (minRestoreEffect >= 1 && minRestoreEffect <= 5) {
                return PresentGpuInplace(0.0f);
            }
            return PresentGpuQuad(CalculateSwellFrame(0.0f));
        }
        ClearCpuCanvas();
        const LONGLONG effectStartTick = ReadTelemetryCounter();
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
            RecordTelemetryCpuEffect(effectStartTick);
            PresentCanvas(0.04f);
            if (!firstFrameDwmSynchronized) FlushDwmOrYield();
            return true;
        }
        RecordTelemetryCpuEffect(effectStartTick);
        PresentCanvas(1.0f);
        if (!firstFrameDwmSynchronized) FlushDwmOrYield();
        return true;
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
            if (usingGpuBackend &&
                (minRestoreEffect >= 1 && minRestoreEffect <= 5)) {
                return true;
            }
            if (minRestoreEffect == 1) return PrecalcInkSplash();
            if (minRestoreEffect == 2) return PrecalcScorch();
            if (minRestoreEffect == 3) return PrecalcSplinter();
            if (minRestoreEffect == 4) return PrecalcMirage();
            if (minRestoreEffect == 5) return PrecalcStipple();
            if (minRestoreEffect == 6 || minRestoreEffect == 7) return true;
            return true;
        }
        if (closeEffect == 3 || closeEffect == 4 ||
            (usingGpuBackend &&
             (closeEffect == 1 || closeEffect == 2))) {
            return true;
        }
        if (closeEffect == 5) return PrecalcPixelMelt();
        float cx = W / 2.0f;
        float cy = H / 2.0f;
        float maxDist = (float)(W + H);
        
        try {
            if (closeEffect == 2) {
                auto pseudo_hash = [](int ix, int iy) -> float {
                    uint32_t h = ((uint32_t)ix * 73856093u) ^ ((uint32_t)iy * 19349663u);
                    return (h % 10000) / 10000.0f;
                };
                const size_t columns = static_cast<size_t>(
                    (W + blockSizeSetting - 1) / blockSizeSetting);
                const size_t rows = static_cast<size_t>(
                    (H + blockSizeSetting - 1) / blockSizeSetting);
                perlinStartTimes.assign(columns * rows, 0.0f);

                // Adjacent output rows share the same pair of lattice rows
                // for long stretches (150, 75, and 37.5 source pixels for
                // the three octaves). Cache their horizontal contributions
                // instead of recalculating four hashes for every block.
                std::vector<float> latticeA(columns);
                std::vector<float> latticeB(columns);
                std::vector<float> latticeC(columns);
                std::vector<float> latticeD(columns);
                float amplitude = 0.5f;
                for (int octave = 0; octave < 3; ++octave) {
                    int cachedLatticeY = INT_MIN;
                    size_t valueIndex = 0;
                    for (int srcY = 0; srcY < H;
                         srcY += blockSizeSetting) {
                        float ty = static_cast<float>(srcY) /
                                   AnimConstants::PerlinNoiseScale;
                        for (int i = 0; i < octave; ++i) ty *= 2.0f;
                        const int iy = static_cast<int>(floorf(ty));
                        const float fy = ty - iy;
                        const float uy =
                            fy * fy * (3.0f - 2.0f * fy);

                        if (iy != cachedLatticeY) {
                            size_t column = 0;
                            for (int srcX = 0; srcX < W;
                                 srcX += blockSizeSetting, ++column) {
                                float tx = static_cast<float>(srcX) /
                                           AnimConstants::PerlinNoiseScale;
                                for (int i = 0; i < octave; ++i)
                                    tx *= 2.0f;
                                const int ix =
                                    static_cast<int>(floorf(tx));
                                const float fx = tx - ix;
                                const float ux =
                                    fx * fx * (3.0f - 2.0f * fx);
                                const float oneMinusUx = 1.0f - ux;
                                latticeA[column] =
                                    pseudo_hash(ix, iy) * oneMinusUx;
                                latticeB[column] =
                                    pseudo_hash(ix + 1, iy) * ux;
                                latticeC[column] =
                                    pseudo_hash(ix, iy + 1) * oneMinusUx;
                                latticeD[column] =
                                    pseudo_hash(ix + 1, iy + 1) * ux;
                            }
                            cachedLatticeY = iy;
                        }

                        const float oneMinusUy = 1.0f - uy;
                        for (size_t column = 0; column < columns;
                             ++column, ++valueIndex) {
                            const float noise =
                                latticeA[column] * oneMinusUy +
                                latticeB[column] * oneMinusUy +
                                latticeC[column] * uy +
                                latticeD[column] * uy;
                            perlinStartTimes[valueIndex] +=
                                amplitude * noise;
                        }
                    }
                    amplitude *= 0.5f;
                }
                for (float& startTime : perlinStartTimes)
                    startTime *= 0.9f;
            } else {
                const size_t particleCount =
                    static_cast<size_t>(
                        (W + blockSizeSetting - 1) / blockSizeSetting) *
                    static_cast<size_t>(
                        (H + blockSizeSetting - 1) / blockSizeSetting);
                if (particleCount > UINT32_MAX) return false;
                firstActiveCloseParticle = particleCount
                                               ? 0
                                               : kNoActiveCloseParticle;
                initialCloseParticleCount = particleCount;
                activeCloseParticleCount = particleCount;
                if (closeEffect == 0)
                    closeShatterParticles.reserve(particleCount);
                else
                    closeThanosParticles.reserve(particleCount);
                uint32_t particleIndex = 0;
                for (int srcY = 0; srcY < H; srcY += blockSizeSetting) {
                    for (int srcX = 0; srcX < W; srcX += blockSizeSetting) {
                        uint32_t hash = ((uint32_t)srcX * 73856093u) ^ ((uint32_t)srcY * 19349663u);
                        const uint32_t nextActive = particleIndex + 1;
                        if (closeEffect == 0) {
                            float force = ((hash >> 8) % 100) / 100.0f;
                            float noiseX = ((hash % 2000) / 1000.0f) - 1.0f;
                            float noiseY = (((hash >> 4) % 2000) / 1000.0f) - 1.0f;
                            float dirX = (srcX + blockSizeSetting / 2.0f) - cx;
                            float dirY = (srcY + blockSizeSetting / 2.0f) - cy;
                            float dist = sqrtf(dirX * dirX + dirY * dirY);
                            if (dist > 0.1f) { dirX /= dist; dirY /= dist; }
                            closeShatterParticles.push_back(
                                {dirX, dirY, force, noiseX, noiseY,
                                 nextActive});
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
                            closeThanosParticles.push_back(
                                {windX, windY, curveX, curveY, startTime,
                                 nextActive});
                        }
                        ++particleIndex;
                    }
                }
                if (closeEffect == 0 && !closeShatterParticles.empty())
                    closeShatterParticles.back().nextActive =
                        kNoActiveCloseParticle;
                else if (closeEffect == 1 &&
                         !closeThanosParticles.empty())
                    closeThanosParticles.back().nextActive =
                        kNoActiveCloseParticle;
            }
        } catch (const std::exception&) {
            Wh_Log(L"Close physics precalc failed (likely OOM) effect=%d "
                   L"shatter_particles~%zu thanos_particles~%zu "
                   L"perlin_values~%zu",
                   closeEffect, closeShatterParticles.capacity(),
                   closeThanosParticles.capacity(),
                   perlinStartTimes.capacity());
            closeShatterParticles.clear();
            closeThanosParticles.clear();
            perlinStartTimes.clear();
            firstActiveCloseParticle = kNoActiveCloseParticle;
            initialCloseParticleCount = 0;
            activeCloseParticleCount = 0;
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
                SetAnimationWindowCloak(TRUE);
                if (IsIconic(hWnd)) {
                    if (data->nativeMinimizeBarrier) {
                        data->nativeAsyncMinimizeObserved = TRUE;
                    }
                    if (!data->nativeStateTimedOut) {
                        RestoreWindowUnderGhostAsync(
                            hWnd, data->originalExStyle,
                            data->restoreMaximized);
                    }
                }
                data->hiddenByCloak = TRUE;
            }
            data->isRising = TRUE;
        } else {
            if (IsWindow(hWnd)) {
                if (!IsIconic(hWnd)) {
                    UpdateDwmTransitions(hWnd, FALSE);
                    SetAnimationWindowCloak(TRUE);
                    data->hiddenByCloak = TRUE;
                }
                // Queue a final minimize even if the window still looks
                // iconic: an earlier async restore may not have been consumed
                // by the target thread yet.
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
    bool WaitForEndpointShowState(bool rising) {
        const bool reachedAtSubmission = HasReachedNativeShowState(
            data->hRealWnd, rising, data->restoreMaximized);
        bool sawOppositeState = !reachedAtSubmission;
        const DWORD settleDeadline =
            GetTickCount() + AnimConstants::EndpointAsyncSettleMs;
        const DWORD deadline =
            GetTickCount() + AnimConstants::NativeStateWaitMs;
        while (IsWindow(data->hRealWnd) &&
               !g_unloading.load(std::memory_order_relaxed) &&
               (LONG)(GetTickCount() - deadline) < 0) {
            bool wantRising = rising;
            if (GetAnimWantRising(data->hRealWnd, &wantRising) &&
                wantRising != rising) {
                return false;
            }
            const bool reached = HasReachedNativeShowState(
                data->hRealWnd, rising, data->restoreMaximized);
            if (!reached) {
                sawOppositeState = true;
            } else if (sawOppositeState ||
                       (LONG)(GetTickCount() - settleDeadline) >= 0) {
                return true;
            }
            Sleep(10);
        }
        if (IsWindow(data->hRealWnd) &&
            !HasReachedNativeShowState(data->hRealWnd, rising,
                                       data->restoreMaximized)) {
            data->nativeStateTimedOut = TRUE;
        }
        return true;
    }
    bool TryReverseToWantedDirection(const LARGE_INTEGER& qpcFreq, LARGE_INTEGER& qpcStart) {
        LARGE_INTEGER qpcNow;
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        double progress = totalMs > 0.0 ? (elapsedMs / totalMs) : 1.0;
        if (progress < 0.0) progress = 0.0;
        if (progress > 1.0) progress = 1.0;

        if (!ApplyWantedDirectionState()) return false;
        if (telemetryEnabled) ++telemetryReversals;

        const double newElapsed = totalMs * (1.0 - progress);
        QueryPerformanceCounter(&qpcNow);
        qpcStart.QuadPart =
            qpcNow.QuadPart - (LONGLONG)(newElapsed * (double)qpcFreq.QuadPart / 1000.0);
        if (telemetryEnabled) {
            Wh_Log(L"Anim reverse hwnd=%p -> %s at p=%.2f",
                   data->hRealWnd,
                   data->isRising ? L"restore" : L"minimize", progress);
        }
        return true;
    }
    void RunLoop() {
        LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
        QueryPerformanceFrequency(&qpcFreq);
        QueryPerformanceCounter(&qpcStart);
        // Pace both renderers to the target monitor. CPU layered windows can
        // finish UpdateLayeredWindow/DwmFlush much faster than the display and
        // previously rendered hundreds of frames that DWM could never show.
        // GPU layered presenters need the same explicit deadline; composition
        // presenters additionally retain their DXGI frame-latency reservation.
        // The first frame remains immediate and all animation progress stays
        // time based, so this only removes redundant submissions.
        constexpr bool refreshPaceAnimationFrames = true;
        const bool reserveGpuFrameSlot =
            usingGpuBackend && gpuPresenter.IsFramePaced();
        const DWORD pacingRefreshHz =
            displayRefreshHz >= 24 && displayRefreshHz <= 1000
                ? displayRefreshHz
                : 60;
        const LONGLONG frameIntervalTicks =
            refreshPaceAnimationFrames
                ? std::max<LONGLONG>(
                      1, (qpcFreq.QuadPart + pacingRefreshHz / 2) /
                             pacingRefreshHz)
                : 0;
        HANDLE framePacingTimer = nullptr;
        if (refreshPaceAnimationFrames) {
            // CREATE_WAITABLE_TIMER_HIGH_RESOLUTION is supported on the
            // Windows 11 versions targeted by this mod. Keep a regular
            // waitable-timer fallback so pacing remains sleep-based if the
            // high-resolution flag isn't available at runtime.
            framePacingTimer = CreateWaitableTimerExW(
                nullptr, nullptr, 0x00000002,
                TIMER_MODIFY_STATE | SYNCHRONIZE);
            if (!framePacingTimer) {
                framePacingTimer =
                    CreateWaitableTimerW(nullptr, FALSE, nullptr);
            }
        }
        bool refreshDeadlinePacing = framePacingTimer != nullptr;
        LONGLONG nextFrameDeadline = qpcStart.QuadPart;
        auto WaitForRefreshDeadline = [&]() {
            while (refreshDeadlinePacing) {
                LARGE_INTEGER now{};
                QueryPerformanceCounter(&now);
                const LONGLONG remainingTicks =
                    nextFrameDeadline - now.QuadPart;
                if (remainingTicks <= 0) return;

                LARGE_INTEGER dueTime{};
                const LONGLONG due100ns = std::max<LONGLONG>(
                    1, (remainingTicks * 10000000LL +
                        qpcFreq.QuadPart - 1) /
                           qpcFreq.QuadPart);
                dueTime.QuadPart = -due100ns;
                if (!SetWaitableTimer(framePacingTimer, &dueTime, 0,
                                      nullptr, nullptr, FALSE)) {
                    Wh_Log(L"Animation refresh timer arm failed error=%lu; "
                           L"continuing without refresh-deadline pacing",
                           static_cast<unsigned long>(GetLastError()));
                    refreshDeadlinePacing = false;
                    return;
                }
                const DWORD waitResult = WaitForSingleObject(
                    framePacingTimer,
                    AnimConstants::GpuFrameLatencyWaitMs);
                if (waitResult != WAIT_OBJECT_0) {
                    Wh_Log(L"Animation refresh timer wait failed result=0x%08X; "
                           L"continuing without refresh-deadline pacing",
                           static_cast<unsigned>(waitResult));
                    refreshDeadlinePacing = false;
                    return;
                }
                // A timer can wake fractionally early. Recheck QPC and rearm
                // instead of spinning for the remaining fraction.
            }
        };
        for (;;) {
            if (!data->isClosing) TryReverseToWantedDirection(qpcFreq, qpcStart);
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (!data->isClosing) TryReverseToWantedDirection(qpcFreq, qpcStart);
            const bool waitForFramePacing =
                reserveGpuFrameSlot || refreshDeadlinePacing;
            const LONGLONG pacingWaitStart =
                telemetryEnabled && waitForFramePacing ? []() {
                    LARGE_INTEGER counter{};
                    QueryPerformanceCounter(&counter);
                    return counter.QuadPart;
                }() : 0;
            if (reserveGpuFrameSlot && !gpuPresenter.ReserveFrameSlot()) {
                RecordTelemetryFrameSample(pacingWaitStart, false);
                break;
            }
            if (refreshDeadlinePacing) {
                WaitForRefreshDeadline();
            }
            // A minimize/restore request can reverse while the worker is
            // sleeping for the next refresh. Apply it before calculating this
            // frame so pacing never adds one stale-direction frame.
            if (!data->isClosing) {
                TryReverseToWantedDirection(qpcFreq, qpcStart);
            }
            MaintainShowDesktopRestoreCloak();
            QueryPerformanceCounter(&qpcNow);
            if (telemetryEnabled && waitForFramePacing) {
                telemetryPacingWaitTicks +=
                    std::max<LONGLONG>(0,
                                       qpcNow.QuadPart - pacingWaitStart);
                ++telemetryPacingWaits;
            }
            double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            BOOL lastFrame = (elapsedMs >= totalMs);
            float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
            const bool presentingFirstFrame = firstFramePending;
            bool presented = false;
            const LONGLONG renderStartTick =
                telemetryEnabled ? qpcNow.QuadPart : 0;
            if (usingGpuBackend) {
                if (data->isClosing) {
                    presented = PresentGpuClose(progress);
                } else if (minRestoreEffect == 0) {
                    presented = PresentGpuGenie(progress);
                } else if (minRestoreEffect >= 1 &&
                           minRestoreEffect <= 5) {
                    presented = PresentGpuInplace(progress);
                } else {
                    const QuadFrame frame =
                        minRestoreEffect == 6
                            ? CalculateSwellFrame(progress)
                            : CalculateWin10Frame(progress);
                    presented = PresentGpuQuad(frame);
                }
            } else {
                ClearCpuCanvas();
                const LONGLONG effectStartTick = ReadTelemetryCounter();
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
                RecordTelemetryCpuEffect(effectStartTick);
                presented = PresentCanvas(fade);
            }
            if (!presented) {
                RecordTelemetryFrameSample(renderStartTick, false);
                break;
            }
            const bool synchronizeLayeredGpuCloseFrame =
                usingGpuBackend && data->isClosing &&
                !gpuPresenter.IsFramePaced() &&
                (!presentingFirstFrame || !firstFrameDwmSynchronized);
            if (usingGpuBackend &&
                (lastFrame || synchronizeLayeredGpuCloseFrame)) {
                // Layered GPU closes don't own a composition swap chain, so
                // retain their stable DWM synchronization and always drain
                // the final frame before the ghost is hidden. The separate
                // refresh timer only limits how often frames are submitted.
                FlushDwmOrYield();
            }
            RecordTelemetryFrameSample(renderStartTick, true);
            if (refreshDeadlinePacing) {
                LARGE_INTEGER afterPresent{};
                QueryPerformanceCounter(&afterPresent);
                nextFrameDeadline += frameIntervalTicks;
                if (nextFrameDeadline <= afterPresent.QuadPart) {
                    // Rendering already consumed this deadline. Discard all
                    // missed slots and rebase to completion time so a slow
                    // effect can submit its next current frame immediately.
                    // The following fast frame gets a fresh full interval,
                    // preventing a burst of catch-up submissions.
                    nextFrameDeadline = afterPresent.QuadPart;
                }
            }
            if (g_unloading.load(std::memory_order_relaxed)) break;
            if (lastFrame) {
                if (!data->isClosing && TryReverseToWantedDirection(qpcFreq, qpcStart)) continue;
                break;
            }
            if (!usingGpuBackend &&
                (!presentingFirstFrame || !firstFrameDwmSynchronized)) {
                FlushDwmOrYield();
            }
        }
        if (framePacingTimer) {
            CancelWaitableTimer(framePacingTimer);
            CloseHandle(framePacingTimer);
        }
    }
    bool FinishRising() {
        if (data->isClosing || !data->isRising) return true;
        if (!data->launchAnimationToken) {
            RestoreWindowUnderGhostAsync(data->hRealWnd,
                                         data->originalExStyle,
                                         data->restoreMaximized);
            if (!WaitForEndpointShowState(true)) return false;
        }
        bool chromeRefreshQueued = false;
        if (data->hiddenByCloak) {
            SetAnimationWindowCloak(FALSE);
            // If the target process applied the effective cloak, keep the
            // completed ghost in place until that same process confirms its
            // DWM-synchronized uncloak (bounded so a dead app can't stall us).
            WaitForLocalShowDesktopCloakRelease();
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhostAsync(data->hRealWnd,
                                         data->originalExStyle);
            chromeRefreshQueued = RefreshDwmChromeAfterUncloak(
                data->hRealWnd, /*nonBlocking=*/true);
        } else {
            RestoreLayeredOpacity(data->hRealWnd, data->originalExStyle);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhostAsync(data->hRealWnd,
                                         data->originalExStyle);
        }
        FlushDwmOrYield();
        if (chromeRefreshQueued) {
            // Explorer and Mica-backed windows rebuild their client surface in
            // response to the queued frame/composition refresh. Keep the
            // completed snapshot over the real window for a short bounded
            // handoff; otherwise the client body can flash while DWM's title
            // bar remains intact. A live reversal leaves the ghost visible and
            // returns to rendering immediately.
            const DWORD deadline =
                GetTickCount() + AnimConstants::EndpointAsyncSettleMs;
            while (IsWindow(data->hRealWnd) &&
                   !g_unloading.load(std::memory_order_relaxed) &&
                   (LONG)(GetTickCount() - deadline) < 0) {
                bool wantRising = true;
                if (GetAnimWantRising(data->hRealWnd, &wantRising) &&
                    !wantRising) {
                    return false;
                }
                Sleep(5);
            }
            FlushDwmOrYield();
        }
        if (hGhost) ShowWindow_Original(hGhost, SW_HIDE);
        return true;
    }
    void FinishTimedOutNativeState() {
        if (!IsWindow(data->hRealWnd)) return;
        if (hGhost) ShowWindow_Original(hGhost, SW_HIDE);
        if (data->hiddenByCloak) {
            SetAnimationWindowCloak(FALSE);
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
            SetAnimationWindowCloak(FALSE);
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
        if (!g_unloading.load(std::memory_order_relaxed) &&
            IsWindow(data->hRealWnd)) {
            const int showCmd =
                data->deferredShowCmd == SW_SHOWMINNOACTIVE ||
                        data->deferredShowCmd == SW_SHOWMINIMIZED ||
                        data->deferredShowCmd == SW_MINIMIZE
                    ? data->deferredShowCmd
                    : SW_MINIMIZE;
            const BOOL submitted =
                ShowWindowAsync_Original(data->hRealWnd, showCmd);
            if (submitted && !WaitForEndpointShowState(false)) {
                return false;
            }
            if (!submitted) {
                data->nativeStateTimedOut = TRUE;
            }
        }
        {
            bool wantRising = false;
            if (GetAnimWantRising(data->hRealWnd, &wantRising) && wantRising) {
                return false;
            }
        }
        if (hGhost) ShowWindow_Original(hGhost, SW_HIDE);
        data->deferredMinimize = FALSE;
        if (IsWindow(data->hRealWnd)) {
            SetAnimationWindowCloak(FALSE);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhostAsync(data->hRealWnd,
                                         data->originalExStyle);
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
                if (!FinishRising()) {
                    if (hGhost) ShowGhostNoActivate();
                    continue;
                }
            } else {
                if (!FinishDeferredMinimize()) {
                    continue;
                }
                if (hGhost) ShowWindow_Original(hGhost, SW_HIDE);
                if (IsWindow(data->hRealWnd)) {
                    SetAnimationWindowCloak(FALSE);
                    UpdateDwmTransitions(data->hRealWnd, TRUE);
                    RestoreZOrderAfterGhostAsync(data->hRealWnd,
                                                 data->originalExStyle);
                }
            }
            bool directionChanged = false;
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
            bool endpointSnapshotUsesCapture = false;
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
                    if (data->hBitmap) {
                        // Both renderers own an independent source now. Move
                        // the original capture into the restore cache instead
                        // of allocating and copying an identical full bitmap.
                        endpointSnapshot = data->hBitmap;
                        endpointSnapshotBits = data->pBits;
                        endpointSnapshotUsesCapture = true;
                    } else {
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
                            if (endpointSnapshotUsesCapture) {
                                data->hBitmap = nullptr;
                                data->pBits = nullptr;
                            }
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
                    if (data->pairedEffectToken) {
                        const bool preserveForNextMinimize =
                            data->isRising &&
                            g_minRestoreShuffleEffect.load(
                                std::memory_order_relaxed);
                        if (!preserveForNextMinimize) {
                            ClearMinRestorePairIfCurrent(
                                data->hRealWnd, data->pairedEffectToken);
                        }
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
            if (endpointSnapshot && !endpointSnapshotUsesCapture) {
                DeleteObject(endpointSnapshot);
            }
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
        if (usingGpuBackend || gpuPresenter.IsInitialized()) {
            gpuPresenter.Reset();
        }
        usingGpuBackend = false;
        if (hScreenDC) {
            ReleaseDC(NULL, hScreenDC);
            hScreenDC = nullptr;
        }
        if (hGhost) {
            DestroyWindow_Original(hGhost);
            hGhost = nullptr;
        }
        if (ghostCounted) {
            g_animationGhostCount.fetch_sub(1, std::memory_order_acq_rel);
            ghostCounted = false;
        }
        if (data->isClosing) {
            // Release the process-wide graphics runtime before allowing the
            // last real window's owner thread to finish its native close.
            // Waiting until after FinishClose can lose the race with process
            // teardown in Chromium, Task Manager, and similar applications.
            ReleaseGpuRenderServiceForFinalWindow(data->hRealWnd);
        }
        FinishClose();
        if (!data->isClosing && !ownershipReleased && !data->isRising && !data->deferredMinimize) {
            SetAnimationWindowCloak(FALSE);
            UpdateDwmTransitions(data->hRealWnd, TRUE);
            RestoreZOrderAfterGhostAsync(data->hRealWnd,
                                         data->originalExStyle);
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
        ReleaseShowDesktopAnimationIfCurrent(
            data->hRealWnd, data->showDesktopAnimationToken);
        data->showDesktopAnimationToken = 0;
        delete data;
        data = nullptr;
    }
};
DWORD WINAPI MainAnimThread(LPVOID lpParam) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    auto* data = (WindowAnimData*)lpParam;
    const HWND animatedWindow = data->hRealWnd;
    const bool closingAnimation = data->isClosing != FALSE;
    AnimationEngine engine(data);
    const bool telemetryEnabled = engine.IsPerformanceTelemetryEnabled();
    const auto readCounter = []() {
        LARGE_INTEGER counter{};
        QueryPerformanceCounter(&counter);
        return counter.QuadPart;
    };
    const LONGLONG initializeStart =
        telemetryEnabled ? readCounter() : 0;
    const bool initialized = engine.Initialize();
    if (telemetryEnabled) {
        engine.SetTelemetryInitializeTicks(
            readCounter() - initializeStart);
    }
    bool bootstrapPresented = true;
    bool precalcPrepared = false;
    if (initialized) {
        const int style = data->effectStyle;
        const bool inplaceBootstrap = !data->isClosing && (style >= 1 && style <= 6);
        const bool closeBootstrap =
            data->isClosing &&
            ((style >= 3 && style <= 5) ||
             (style == 2 && engine.IsUsingGpuBackend()));
        if (inplaceBootstrap || closeBootstrap) {
            const LONGLONG bootstrapStart =
                telemetryEnabled ? readCounter() : 0;
            bootstrapPresented = engine.ShowInkBootstrapFrame();
            engine.RecordTelemetryFrameSample(
                bootstrapStart, bootstrapPresented);
        }
        if (bootstrapPresented) {
            const LONGLONG precalcStart =
                telemetryEnabled ? readCounter() : 0;
            precalcPrepared = engine.PrecalcPhysics();
            if (telemetryEnabled) {
                engine.SetTelemetryPrecalcTicks(
                    readCounter() - precalcStart);
            }
        }
        if (bootstrapPresented && precalcPrepared) {
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
    PCWSTR telemetryStatus = L"ok";
    if (g_unloading.load(std::memory_order_relaxed)) {
        telemetryStatus = L"unload";
    } else if (!initialized) {
        telemetryStatus = L"init_failed";
    } else if (!bootstrapPresented) {
        telemetryStatus = L"bootstrap_failed";
    } else if (!precalcPrepared) {
        telemetryStatus = L"precalc_failed";
    } else if (engine.TelemetryPresentationFailed()) {
        telemetryStatus = L"present_failed";
    }
    engine.LogPerformanceTelemetry(telemetryStatus);
    engine.Teardown();
    if (!closingAnimation) {
        RequestPredictedGpuWarmup(animatedWindow);
    } else if (HWND hStableWnd = FindStableGpuWindow(
                   /*requireCloseCompatibility=*/false, animatedWindow)) {
        RequestPredictedGpuWarmup(hStableWnd);
    } else if (!HasRemainingAnimationWindow(animatedWindow)) {
        // FinishClose normally released this before handing control back to
        // the owner thread. Retry here in case another GPU presenter was still
        // unwinding at that instant.
        ReleaseGpuRenderServiceForFinalWindow(animatedWindow);
    }
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
                    ULONG_PTR suppliedSnapshotToken = 0,
                    BOOL fastShowDesktopStart = FALSE,
                    ULONG_PTR showDesktopAnimationToken = 0) {
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
    RECT winRect{};
    RECT rect{};
    if (!GetAnimationWindowRects(hWnd, &winRect, &rect)) {
        FailAnimationStart(hWnd, rising, originalExStyle, cloakHidden);
        return false;
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
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(
                L"Animation skipped (busy/unloading) hwnd=%p rising=%d "
                L"closing=%d",
                hWnd, rising, isClosing);
        }
        if (asyncRestoreReservation) return false;
        if (!isClosing && !g_unloading.load(std::memory_order_relaxed) &&
            RetargetLiveMinRestore(hWnd, rising != FALSE) == MinRestoreRetarget::Accepted) {
            if (suppressNativeMinimizeOnFailure) *suppressNativeMinimizeOnFailure = TRUE;
            return false;
        }
        FailAnimationStart(hWnd, rising, originalExStyle, cloakHidden, /*skipDwmIfOwned=*/true);
        return false;
    }
    const DWORD targetThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    const bool onTargetThread =
        targetThreadId && targetThreadId == GetCurrentThreadId();
    LONG_PTR storedExStyle = originalExStyle;
    if (!isClosing) {
        const LONG_PTR currentExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        storedExStyle = (originalExStyle & ~WS_EX_TOPMOST) |
                        (currentExStyle & WS_EX_TOPMOST);
        // The ghost is itself inserted into the topmost band, above existing
        // topmost windows (or directly below the topmost taskbar). Leaving the
        // real window in its band avoids an asynchronous demote/promote race
        // with a rapid follow-up animation.
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
    DWORD alignVal = 1, dataSize = sizeof(alignVal);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
    HWND hTrayForDpi = FindTaskbarForMonitor(hMon);
    const TaskbarGeometry taskbarGeometry = GetTaskbarGeometry(
        hWnd, hTrayForDpi, mi,
        requestedUnhide || deferredMinimize);
    POINT learnedTarget = GetFallbackTaskbarButtonPosition(
        hWnd, hTrayForDpi, mi, taskbarGeometry, alignVal);
    const bool shuffledMinRestorePair =
        !isClosing &&
        g_minRestoreShuffleEffect.load(std::memory_order_relaxed);
    ULONG_PTR pairedEffectToken = 0;
    bool publishPairToken = false;
    if (shuffledMinRestorePair) {
        int pairedEffectStyle = -1;
        pairedEffectToken = ReadMinRestorePairToken(hWnd, &pairedEffectStyle);
        if (pairedEffectToken) effectStyle = pairedEffectStyle;
    }
    if (effectStyle < 0) {
        effectStyle = isClosing ? ResolveCloseEffectStyle()
                                : ResolveMinRestoreEffectStyle();
    }
    if (shuffledMinRestorePair && !pairedEffectToken) {
        pairedEffectToken = CreateMinRestorePairToken(effectStyle);
        publishPairToken = true;
    } else if (!isClosing && !shuffledMinRestorePair) {
        ClearMinRestorePair(hWnd);
    }
    if (!isClosing && (effectStyle == 0 || effectStyle == 7)) {
        WCHAR windowTitle[256] = {0};
        GetWindowTextW(hWnd, windowTitle, 256);
        learnedTarget = GetTaskbarButtonPositionAsync(
            hWnd, windowTitle, learnedTarget, hMon,
            taskbarGeometry.edge,
            fastShowDesktopStart ? 0
                                 : AnimConstants::UiaLookupWaitMs);
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
        hWnd, nullptr, nullptr, rect, hMon, w, h, learnedTarget.x,
        learnedTarget.y, rising, storedExStyle, cloakHidden, nullptr,
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
        nativeBarrierOwner.release(),
        fastShowDesktopStart,
        showDesktopAnimationToken
    };
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
                    clearPairedEffect = pairedEffectToken != 0;
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
    const bool screenOnlyCapture = isClosing && RequiresScreenCapture(hWnd);
    const bool preferScreenCapture =
        (fastShowDesktopStart && !rising) ||
        ShouldUseBitBlt(hWnd, isClosing);
    const bool allowShowDesktopSwitchGhostWait =
        !rising && !isClosing && !onTargetThread &&
        (fastShowDesktopStart || IsClassicShowDesktopOperation());
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
        const HWND hCaptureTaskbar = FindTaskbarForMonitor(hMon);
        bool capturedTaskbarAbove = false;
        RECT capturedTaskbarRect{};
        auto LogScreenCaptureFailure = [&](PCWSTR phase,
                                           const ScreenCaptureEligibility& e) {
            if (!screenOnlyCapture && !IsDiagnosticLoggingEnabled()) return;
            WCHAR blockerClass[96]{};
            DWORD blockerProcessId = 0;
            BOOL blockerIconic = FALSE;
            HWND blockerOwner = nullptr;
            HWND blockerRootOwner = nullptr;
            if (e.blockingWindow) {
                GetClassNameW(e.blockingWindow, blockerClass,
                              ARRAYSIZE(blockerClass));
                GetWindowThreadProcessId(e.blockingWindow,
                                         &blockerProcessId);
                blockerIconic = IsIconic(e.blockingWindow);
                blockerOwner = GetWindow(e.blockingWindow, GW_OWNER);
                blockerRootOwner =
                    GetAncestor(e.blockingWindow, GA_ROOTOWNER);
            }
            Wh_Log(
                L"Screen capture unavailable hwnd=%p phase=%s rising=%d "
                L"crossThread=%d reason=%d blocker=%p blocker_pid=%lu "
                L"blocker_class=%s blocker_iconic=%d blocker_owner=%p "
                L"blocker_root_owner=%p",
                hWnd, phase, rising, !onTargetThread,
                static_cast<int>(e.failureReason),
                e.blockingWindow, blockerProcessId,
                blockerClass[0] ? blockerClass : L"none", blockerIconic,
                blockerOwner, blockerRootOwner);
        };
        auto CaptureScreen = [&]() -> BOOL {
            // A screen DC contains the fully composed desktop, not just hWnd.
            // Refuse the fallback when another top-level window contributes
            // pixels above the target. Checking the z-order also catches the
            // hit-test-transparent ghosts during batched shell minimizes.
            DWORD switchGhostDeadline = 0;
            for (;;) {
                ScreenCaptureEligibility beforeCapture;
                if (!CanCaptureWindowFromScreen(
                        hWnd, rect, hCaptureTaskbar,
                        fastShowDesktopStart && !isClosing,
                        &beforeCapture)) {
                    if (!allowShowDesktopSwitchGhostWait ||
                        !beforeCapture.blockingSwitchAnimationGhost) {
                        LogScreenCaptureFailure(L"before", beforeCapture);
                        return FALSE;
                    }
                    if (!switchGhostDeadline) {
                        switchGhostDeadline =
                            GetTickCount() +
                            AnimConstants::ShowDesktopSwitchGhostWaitMs;
                    }
                    if (static_cast<LONG>(GetTickCount() -
                                          switchGhostDeadline) >= 0) {
                        LogScreenCaptureFailure(L"before-timeout",
                                                beforeCapture);
                        return FALSE;
                    }
                    // Alt+Tab's thumbnail belongs to the destination process,
                    // so Explorer can't see its process-local animation set.
                    // Let that short handoff finish before taking Win+D's
                    // composed-screen snapshot instead of falling through to
                    // a visible native minimize.
                    FlushDwmOrYield();
                    continue;
                }
                const BOOL result =
                    BitBlt(tempDC, 0, 0, rawW, rawH, hScreenDC,
                           winRect.left, winRect.top, SRCCOPY);
                ScreenCaptureEligibility afterCapture;
                if (!result ||
                    !CanCaptureWindowFromScreen(
                        hWnd, rect, hCaptureTaskbar,
                        fastShowDesktopStart && !isClosing,
                        &afterCapture)) {
                    if (allowShowDesktopSwitchGhostWait &&
                        afterCapture.blockingSwitchAnimationGhost) {
                        if (!switchGhostDeadline) {
                            switchGhostDeadline =
                                GetTickCount() +
                                AnimConstants::ShowDesktopSwitchGhostWaitMs;
                        }
                        if (static_cast<LONG>(GetTickCount() -
                                              switchGhostDeadline) < 0) {
                            FlushDwmOrYield();
                            continue;
                        }
                    }
                    LogScreenCaptureFailure(
                        result ? L"after" : L"bitblt", afterCapture);
                    return FALSE;
                }
                if (beforeCapture.taskbarAboveAndOverlapping !=
                        afterCapture.taskbarAboveAndOverlapping ||
                    (beforeCapture.taskbarAboveAndOverlapping &&
                     !EqualRect(&beforeCapture.taskbarRect,
                                &afterCapture.taskbarRect))) {
                    return FALSE;
                }
                capturedTaskbarAbove =
                    beforeCapture.taskbarAboveAndOverlapping;
                capturedTaskbarRect = beforeCapture.taskbarRect;
                return TRUE;
            }
        };
        bool capturedFromScreen = false;
        bool screenCaptureAttempted = false;
        BOOL captured = FALSE;
        if (preferScreenCapture) {
            screenCaptureAttempted = true;
            captured = CaptureScreen();
            capturedFromScreen = captured != FALSE;
        }
        if (!captured && onTargetThread && !screenOnlyCapture) {
            // PrintWindow is synchronous. It is safe only when this hook is
            // already running on the target window's UI thread. Clear a
            // rejected screen attempt first so partial PrintWindow output
            // can't retain pixels from the composed desktop.
            GdiFlush();
            memset(tempBits, 0, (size_t)rawW * rawH * sizeof(DWORD));
            captured = PrintWindow(hWnd, tempDC, PW_RENDERFULLCONTENT);
        }
        if (!captured && !screenCaptureAttempted && allowScreenFallback &&
            IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
            screenCaptureAttempted = true;
            captured = CaptureScreen();
            capturedFromScreen = captured != FALSE;
        }
        if (captured && capturedFromScreen) {
            GdiFlush();
            RECT overlap{};
            if (capturedTaskbarAbove &&
                IntersectRect(&overlap, &winRect,
                              &capturedTaskbarRect)) {
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
                    (capturedTaskbarRect.right - capturedTaskbarRect.left) >=
                    (capturedTaskbarRect.bottom - capturedTaskbarRect.top);
                const bool trailingEdge = horizontal
                                              ? capturedTaskbarRect.top >=
                                                    (mi.rcMonitor.top +
                                                     mi.rcMonitor.bottom) / 2
                                              : capturedTaskbarRect.left >=
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
            snapshotReady = ConsumeSnapshotLocked(
                hWnd, data->pBits, w, h, /*logFailure=*/!onTargetThread);
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
        // Keep the original capture with the active animation. If minimize
        // reaches its endpoint, CompleteMinRestoreEndpoint transfers that DIB
        // directly to the restore cache. Live reversals therefore allocate no
        // redundant cache bitmap and still keep their current source pixels.
    }
    if (snapshotReady && !isClosing) {
        ApplyRoundedCornerMask(
            data->pBits, w, h,
            GetAnimatedWindowCornerRadius(
                hWnd, restoreMaximized, originalExStyle));
    }
    if (snapshotReady && !rising && !isClosing && fastShowDesktopStart &&
        g_restoreAnimation.load(std::memory_order_relaxed)) {
        // Keep an independent copy as soon as the optimized Win+D leader has
        // been captured. Its minimize can cross both Explorer and the target
        // process; publishing the restore source here makes it independent of
        // which nested native hook observes the eventual endpoint first.
        void* restoreBits = nullptr;
        HBITMAP restoreBitmap =
            CreateDib32(hScreenDC, w, h, &restoreBits);
        if (restoreBitmap && restoreBits) {
            memcpy(restoreBits, data->pBits,
                   static_cast<size_t>(w) * static_cast<size_t>(h) * 4u);
            bool stored = false;
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                stored = StoreSnapshotLocked(hWnd, restoreBitmap, restoreBits,
                                             w, h);
            }
            if (!stored) DeleteObject(restoreBitmap);
        } else {
            if (restoreBitmap) DeleteObject(restoreBitmap);
            if (IsDiagnosticLoggingEnabled()) {
                Wh_Log(L"Show Desktop restore snapshot allocation failed "
                       L"hwnd=%p size=%dx%d",
                       hWnd, w, h);
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
        if (shuffledMinRestorePair) {
            if (publishPairToken) {
                if (!PublishMinRestorePairToken(
                        hWnd, data->pairedEffectToken)) {
                    data->pairedEffectToken = 0;
                }
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
        if (IsDiagnosticLoggingEnabled()) {
            WCHAR windowClass[96]{};
            DWORD windowProcessId = 0;
            GetClassNameW(hWnd, windowClass, ARRAYSIZE(windowClass));
            GetWindowThreadProcessId(hWnd, &windowProcessId);
            Wh_Log(
                L"Animation start hwnd=%p kind=%s style=%d dock=(%ld,%ld) "
                L"taskbar_edge=%u duration=%d target_pid=%lu class=%s",
                hWnd,
                isClosing ? L"close"
                          : (rising ? L"restore" : L"minimize"),
                effectStyle, learnedTarget.x, learnedTarget.y,
                static_cast<unsigned>(taskbarGeometry.edge),
                startedDurationMs,
                windowProcessId,
                windowClass[0] ? windowClass : L"unknown");
        }
        if (hFirstShown) {
            const bool crossThreadGpuGenie =
                !onTargetThread && effectStyle == 0 &&
                g_gpuAcceleration.load(std::memory_order_relaxed) &&
                !GetSystemMetrics(SM_REMOTESESSION);
            const DWORD firstFrameWaitMs =
                fastShowDesktopStart
                    ? 50
                    : ((!rising && !isClosing &&
                        ((effectStyle >= 1 && effectStyle <= 7) ||
                         crossThreadGpuGenie))
                           ? 500
                           : 200);
            if (waitForFirstFrame) WaitForSingleObject(hFirstShown, firstFrameWaitMs);
            CloseHandle(hFirstShown);
        }
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
    if (RequiresScreenCapture(hWnd) &&
        !HasRemainingAnimationWindow(hWnd)) {
        // Console surfaces require a composed-screen snapshot. A composition
        // renderer retained from a preceding restore can leave that capture
        // temporarily unavailable. This is the final app window, so there is
        // no later restore for which keeping the service warm would help.
        ReleaseGpuRenderServiceForFinalWindow(
            hWnd, /*waitForService=*/false);
    }
    HANDLE wait = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE workerWait = NULL;
    if (wait && !DuplicateHandle(GetCurrentProcess(), wait, GetCurrentProcess(), &workerWait, 0,
                                 FALSE, DUPLICATE_SAME_ACCESS)) {
        CloseHandle(wait);
        wait = nullptr;
    }

    const bool started =
        StartAnimation(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE, TRUE, closeMsg,
                       workerWait);
    if (!started) {
        Wh_Log(L"Close animation did not start hwnd=%p msg=0x%04X", hWnd,
               closeMsg);
        // Failed capture/initialization must not leave the process-wide GPU
        // service behind while the native close continues.
        ReleaseGpuRenderServiceForFinalWindow(
            hWnd, /*waitForService=*/false);
        ScheduleFinalWindowGpuRelease(hWnd);
    }

    if (started) {
        if (wait) WaitForCloseAnimation(wait);
    } else if (workerWait) {
        CloseHandle(workerWait);
    }

    if (wait) CloseHandle(wait);
    return started;
}

enum class ShowDesktopBatchDecision { None, Animate, Skip };

struct ShowDesktopIntentSnapshot {
    DWORD untilTick{};
    HWND target{};
    HWND lastAnimated{};
};

static HWND SharedToHwnd(LONG value) {
    return reinterpret_cast<HWND>(static_cast<LONG_PTR>(value));
}

static bool IsFutureTick(DWORD deadline, DWORD now = GetTickCount()) {
    return deadline && static_cast<LONG>(now - deadline) < 0;
}

static bool ReadShowDesktopIntent(ShowDesktopIntentSnapshot* snapshot) {
    if (!snapshot || !EnsureSharedStateMapped()) return false;
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (!g_pSharedState || g_pSharedState->magic != kSharedStateMagic) {
        return false;
    }
    for (int attempt = 0; attempt < 8; ++attempt) {
        const LONG sequenceBefore = g_pSharedState->showDesktopSequence;
        if (sequenceBefore & 1) {
            SwitchToThread();
            continue;
        }
        MemoryBarrier();
        const LONG untilTick = g_pSharedState->showDesktopUntilTick;
        const LONG target = g_pSharedState->showDesktopTargetWindow;
        const LONG lastAnimated =
            g_pSharedState->showDesktopLastAnimatedWindow;
        MemoryBarrier();
        const LONG sequenceAfter = g_pSharedState->showDesktopSequence;
        if (sequenceBefore != sequenceAfter || (sequenceAfter & 1)) {
            SwitchToThread();
            continue;
        }
        snapshot->untilTick = static_cast<DWORD>(untilTick);
        snapshot->target = SharedToHwnd(target);
        snapshot->lastAnimated = SharedToHwnd(lastAnimated);
        return true;
    }
    return false;
}

static ShowDesktopBatchDecision GetShowDesktopBatchDecision(HWND hWnd) {
    if (!g_showDesktopTopWindowOnly.load(std::memory_order_relaxed)) {
        return ShowDesktopBatchDecision::None;
    }
    const DWORD now = GetTickCount();
    if (IsShellExplorerProcess()) {
        const DWORD localUntil =
            g_localShowDesktopUntilTick.load(std::memory_order_acquire);
        if (IsFutureTick(localUntil, now)) {
            const HWND target =
                g_localShowDesktopTarget.load(std::memory_order_relaxed);
            return target &&
                           (target == hWnd ||
                            target == GetAncestor(hWnd, GA_ROOTOWNER))
                       ? ShowDesktopBatchDecision::Animate
                       : ShowDesktopBatchDecision::Skip;
        }
    }

    ShowDesktopIntentSnapshot snapshot{};
    if (!ReadShowDesktopIntent(&snapshot) ||
        !IsFutureTick(snapshot.untilTick, now)) {
        return ShowDesktopBatchDecision::None;
    }
    return snapshot.target &&
                   (snapshot.target == hWnd ||
                    snapshot.target == GetAncestor(hWnd, GA_ROOTOWNER))
               ? ShowDesktopBatchDecision::Animate
               : ShowDesktopBatchDecision::Skip;
}

static bool IsShowDesktopRestorePrepared(HWND hWnd) {
    if (!hWnd) return false;
    const ULONG_PTR preparedToken = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopRestorePrepared));
    return preparedToken &&
           reinterpret_cast<ULONG_PTR>(GetPropW(
               hWnd, kPropShowDesktopAnimationOwner)) == preparedToken;
}

static void ClearLocalShowDesktopCloakWatchIfCurrent(HWND hWnd,
                                                      ULONG_PTR token) {
    if (hWnd && token &&
        reinterpret_cast<ULONG_PTR>(GetPropW(
            hWnd, kPropShowDesktopLocalCloakWatch)) == token) {
        RemovePropW(hWnd, kPropShowDesktopLocalCloakWatch);
    }
    if (hWnd && token &&
        reinterpret_cast<ULONG_PTR>(GetPropW(
            hWnd, kPropShowDesktopCloakReady)) == token) {
        RemovePropW(hWnd, kPropShowDesktopCloakReady);
    }
}

static DWORD WINAPI LocalShowDesktopCloakWatchThread(LPVOID parameter) {
    auto* data = static_cast<LocalShowDesktopCloakWatchData*>(parameter);
    const HWND hWnd = data->hWnd;
    const ULONG_PTR token = data->token;
    delete data;

    const DWORD deadline = GetTickCount() + 7000;
    bool timedOut = false;
    while (!g_unloading.load(std::memory_order_relaxed) && IsWindow(hWnd)) {
        const ULONG_PTR prepared = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopRestorePrepared));
        const ULONG_PTR owner = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopAnimationOwner));
        if (prepared != token || owner != token) break;
        if (static_cast<LONG>(GetTickCount() - deadline) >= 0) {
            timedOut = true;
            break;
        }
        Sleep(5);
    }

    if (IsWindow(hWnd)) {
        ULONG_PTR currentOwner = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopAnimationOwner));
        ULONG_PTR currentPrepared = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopRestorePrepared));
        if (timedOut && currentOwner == token && currentPrepared == token) {
            RemovePropW(hWnd, kPropShowDesktopRestorePrepared);
            currentPrepared = 0;
            if (IsDiagnosticLoggingEnabled()) {
                Wh_Log(L"Show Desktop local cloak watch timed out hwnd=%p",
                       hWnd);
            }
        }

        const bool newerRestoreOwnsCloak =
            currentOwner && currentOwner != token &&
            currentPrepared == currentOwner;
        if (newerRestoreOwnsCloak) {
            // Drop only this watcher's owned-surface markers, then let the
            // newer token take them over without touching the shared root
            // cloak which the newer restore still needs.
            UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, FALSE);
            UpdateShowDesktopOwnedSurfaceCloaks(
                hWnd, currentOwner, TRUE);
        } else {
            // The effective cloak was applied from this target process, so
            // clear it from this process too. Explorer's cross-process false
            // write isn't reliable for every Chromium/Electron DWM surface.
            UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, FALSE);
            SetWindowCloak(hWnd, FALSE);
            UpdateDwmTransitions(hWnd, TRUE);
            FlushDwmOrYield();
        }
    }

    ClearLocalShowDesktopCloakWatchIfCurrent(hWnd, token);
    return 0;
}

static bool EnsureLocalShowDesktopCloakWatch(HWND hWnd, ULONG_PTR token) {
    if (!hWnd || !token || g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopLocalCloakWatch)) == token) {
            return true;
        }
        if (!SetPropW(hWnd, kPropShowDesktopLocalCloakWatch,
                      reinterpret_cast<HANDLE>(token)) ||
            reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopLocalCloakWatch)) != token) {
            return false;
        }
    }

    auto* data = new (std::nothrow)
        LocalShowDesktopCloakWatchData{hWnd, token};
    if (data && StartWorkerThread(LocalShowDesktopCloakWatchThread, data)) {
        return true;
    }

    delete data;
    ClearLocalShowDesktopCloakWatchIfCurrent(hWnd, token);
    return false;
}

static void ReassertPreparedShowDesktopRestoreCloak(HWND hWnd) {
    if (!hWnd) return;
    const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopRestorePrepared));
    const auto tokenStillPrepared = [hWnd, token]() {
        return token &&
               reinterpret_cast<ULONG_PTR>(GetPropW(
                   hWnd, kPropShowDesktopRestorePrepared)) == token &&
               reinterpret_cast<ULONG_PTR>(GetPropW(
                   hWnd, kPropShowDesktopAnimationOwner)) == token;
    };
    if (!tokenStillPrepared()) {
        return;
    }

    DWORD windowProcessId = 0;
    GetWindowThreadProcessId(hWnd, &windowProcessId);
    if (windowProcessId == GetCurrentProcessId() &&
        !EnsureLocalShowDesktopCloakWatch(hWnd, token)) {
        // Never apply a process-local cloak unless that same process has also
        // guaranteed the corresponding endpoint cleanup.
        return;
    }

    // CTray's native restore is still required for the real iconic state and
    // the background-window batch, but ShowWindow/DefWindowProc can clear an
    // app cloak while rebuilding a Chromium/Electron DWM surface. Reapply the
    // animation-owned cloak on the same call stack, before DWM can expose that
    // full-size real window underneath the transparent restore ghost.
    UpdateDwmTransitions(hWnd, FALSE);
    SetWindowCloak(hWnd, TRUE);
    UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, TRUE);

    // The animation endpoint can disarm this callback concurrently after the
    // first ownership check. Undo our write if that happened. Don't disturb a
    // newer session which has already claimed the same recycled/live HWND.
    if (!tokenStillPrepared()) {
        UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, FALSE);
        const ULONG_PTR currentOwner = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopAnimationOwner));
        if (!currentOwner || currentOwner == token) {
            SetWindowCloak(hWnd, FALSE);
            UpdateDwmTransitions(hWnd, TRUE);
        }
    }
}

static bool PrepareTargetShowDesktopRestoreCloak(HWND hWnd,
                                                  ULONG_PTR token) {
    if (!hWnd || !token || !IsShowDesktopRestorePrepared(hWnd) ||
        g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }

    if (reinterpret_cast<ULONG_PTR>(GetPropW(
            hWnd, kPropShowDesktopCloakReady)) == token) {
        RemovePropW(hWnd, kPropShowDesktopCloakReady);
    }

    DWORD targetProcessId = 0;
    GetWindowThreadProcessId(hWnd, &targetProcessId);
    if (!targetProcessId) return false;

    if (targetProcessId == GetCurrentProcessId()) {
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        if (reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopLocalCloakWatch)) != token) {
            return false;
        }
        FlushDwmOrYield();
        return SetPropW(hWnd, kPropShowDesktopCloakReady,
                        reinterpret_cast<HANDLE>(token)) != FALSE;
    }

    const ULONG_PTR endpointValue = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopCloakEndpoint));
    if (!endpointValue || endpointValue > MAXDWORD) return false;
    const DWORD endpointThreadId = static_cast<DWORD>(endpointValue);

    HANDLE endpointThread = OpenThread(
        SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION, FALSE,
        endpointThreadId);
    if (!endpointThread) return false;
    const bool endpointMatchesTarget =
        GetProcessIdOfThread(endpointThread) == targetProcessId;
    if (!endpointMatchesTarget ||
        !PostThreadMessageW(endpointThreadId,
                            ANIM_PREPARE_SHOW_DESKTOP_RESTORE,
                            reinterpret_cast<WPARAM>(hWnd), 0)) {
        CloseHandle(endpointThread);
        return false;
    }

    const DWORD deadline = GetTickCount() + 40;
    bool ready = false;
    while (static_cast<LONG>(GetTickCount() - deadline) < 0 &&
           WaitForSingleObject(endpointThread, 0) == WAIT_TIMEOUT &&
           IsShowDesktopRestorePrepared(hWnd)) {
        if (reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopCloakReady)) == token) {
            ready = true;
            break;
        }
        Sleep(1);
    }
    CloseHandle(endpointThread);
    return ready;
}

static bool IsVisibleShowDesktopCandidate(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || !IsWindowVisible(hWnd) ||
        IsIconic(hWnd) || GetAncestor(hWnd, GA_ROOT) != hWnd ||
        GetWindow(hWnd, GW_OWNER) ||
        (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        !ShouldAnimateWindow(hWnd)) {
        return false;
    }
    const std::wstring cls = GetClassNameStr(hWnd);
    if (ContainsClass(cls, kAlwaysExcludedClasses) ||
        ContainsClass(cls, kGdiExcludedClasses) ||
        _wcsicmp(cls.c_str(), L"Progman") == 0 ||
        _wcsicmp(cls.c_str(), L"WorkerW") == 0 ||
        _wcsicmp(cls.c_str(), L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls.c_str(), L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);
    if (IsExplorerProcess() && windowPid == GetCurrentProcessId() &&
        _wcsicmp(cls.c_str(), L"CabinetWClass") != 0 &&
        _wcsicmp(cls.c_str(), L"ExploreWClass") != 0) {
        // Explorer owns the desktop, taskbar, Start/Search hosts and several
        // transient XAML surfaces. Only its real File Explorer top-level
        // windows are eligible to lead a Show Desktop animation.
        return false;
    }
    BOOL cloaked = FALSE;
    return FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) ||
           !cloaked;
}

static HWND FindTopShowDesktopCandidate() {
    for (HWND hWnd = GetTopWindow(nullptr); hWnd;
         hWnd = GetWindow(hWnd, GW_HWNDNEXT)) {
        if (IsVisibleShowDesktopCandidate(hWnd)) return hWnd;
    }
    return nullptr;
}

static HWND GetLastShowDesktopAnimatedWindow() {
    ShowDesktopIntentSnapshot snapshot{};
    if (ReadShowDesktopIntent(&snapshot) && snapshot.lastAnimated) {
        return snapshot.lastAnimated;
    }
    return g_localShowDesktopLastAnimated.load(std::memory_order_relaxed);
}

static HWND ChooseShowDesktopAnimatedWindow() {
    HWND foreground = GetForegroundWindow();
    if (foreground) foreground = GetAncestor(foreground, GA_ROOTOWNER);
    if (IsVisibleShowDesktopCandidate(foreground)) {
        // For the minimize half, animate the window that is actually highest
        // in the visual Z-order. Windows can temporarily report a foreground
        // HWND below another visible window; choosing it would make screen
        // capture include the real top window or reject the snapshot.
        if (HWND topWindow = FindTopShowDesktopCandidate()) return topWindow;
        return foreground;
    }

    // When Show Desktop is already active, the foreground belongs to the
    // desktop/taskbar and no app candidate is visible. Restore the same iconic
    // leader that was selected for the minimize half.
    const HWND lastAnimated = GetLastShowDesktopAnimatedWindow();
    if (lastAnimated && IsWindow(lastAnimated) && IsIconic(lastAnimated) &&
        ShouldAnimateWindow(lastAnimated)) {
        return lastAnimated;
    }
    return FindTopShowDesktopCandidate();
}

static void PublishShowDesktopIntent(HWND target, DWORD untilTick,
                                     bool rememberTarget) {
    g_localShowDesktopTarget.store(target, std::memory_order_relaxed);
    if (rememberTarget && target) {
        g_localShowDesktopLastAnimated.store(target,
                                             std::memory_order_relaxed);
    }
    g_localShowDesktopUntilTick.store(untilTick, std::memory_order_release);

    if (!EnsureSharedStateMapped()) return;
    std::lock_guard<std::mutex> lock(g_SharedStateMutex);
    if (!g_pSharedState || !g_sharedStateWritable ||
        g_pSharedState->magic != kSharedStateMagic) {
        return;
    }
    LONG sequence =
        InterlockedIncrement(&g_pSharedState->showDesktopSequence);
    if (!(sequence & 1)) {
        InterlockedIncrement(&g_pSharedState->showDesktopSequence);
    }
    InterlockedExchange(&g_pSharedState->showDesktopUntilTick,
                        static_cast<LONG>(untilTick));
    InterlockedExchange(&g_pSharedState->showDesktopTargetWindow,
                        HwndToShared(target));
    if (rememberTarget && target) {
        InterlockedExchange(
            &g_pSharedState->showDesktopLastAnimatedWindow,
            HwndToShared(target));
    }
    MemoryBarrier();
    InterlockedIncrement(&g_pSharedState->showDesktopSequence);
    PulseSharedHeartbeatLocked();
}

thread_local bool g_showDesktopIntentHookActive = false;
static void RestoreAllShowDesktopInstantMinimizeTransitions();
static bool CanPrepareRestoreAnimation(HWND hWnd);
DWORD WINAPI AsyncRestoreAnimThread(LPVOID lpParam);

static void __cdecl RaiseDesktop_Hook(void* pThis, int flags) {
    if (!RaiseDesktop_Original) return;
    const bool showDesktopCall =
        (flags == 2 || flags == 3) && IsShellExplorerProcess() &&
        !g_showDesktopIntentHookActive;
    if (!showDesktopCall) {
        RaiseDesktop_Original(pThis, flags);
        return;
    }

    // Detect the native Show Desktop operation for both modes. The classic
    // mode uses this scope to serialize captures so one animation ghost can't
    // contaminate the next window's bitmap.
    g_showDesktopIntentHookActive = true;
    g_localShowDesktopOperationUntilTick.store(
        GetTickCount() + 5000, std::memory_order_release);

    const bool optimizeTopWindow =
        g_showDesktopTopWindowOnly.load(std::memory_order_relaxed);
    HWND target = nullptr;
    bool restoreReserved = false;
    LONG_PTR restoreOriginalExStyle = 0;
    uint64_t restoreReservationGeneration = 0;
    BOOL restoreMaximized = FALSE;
    ULONG_PTR restoreAnimationToken = 0;
    if (optimizeTopWindow) {
        // A previous optimized batch can leave skipped windows minimized with
        // their native minimize transition suppressed. Restore that attribute
        // before CTray begins either half of its next Show Desktop operation;
        // newly skipped windows will opt in again below if this is a minimize.
        RestoreAllShowDesktopInstantMinimizeTransitions();
        target = ChooseShowDesktopAnimatedWindow();
        PublishShowDesktopIntent(target, GetTickCount() + 5000,
                                 target != nullptr);
        if (target && IsIconic(target) &&
            g_restoreAnimation.load(std::memory_order_relaxed)) {
            const MinRestoreRetarget retarget =
                RetargetLiveMinRestore(target, true);
            if (retarget == MinRestoreRetarget::Accepted) {
                const ULONG_PTR activeAnimationToken = reinterpret_cast<ULONG_PTR>(
                    GetPropW(target, kPropShowDesktopAnimationOwner));
                if (activeAnimationToken &&
                    (!SetPropW(target, kPropShowDesktopRestorePrepared,
                               reinterpret_cast<HANDLE>(
                                   activeAnimationToken)) ||
                     reinterpret_cast<ULONG_PTR>(GetPropW(
                         target, kPropShowDesktopRestorePrepared)) !=
                         activeAnimationToken)) {
                    RemovePropW(target, kPropShowDesktopRestorePrepared);
                }
            } else if (retarget == MinRestoreRetarget::None) {
                restoreAnimationToken =
                    TryClaimShowDesktopAnimation(target);
                if (restoreAnimationToken) {
                    if (SetPropW(target, kPropShowDesktopRestorePrepared,
                                 reinterpret_cast<HANDLE>(
                                     restoreAnimationToken)) &&
                        reinterpret_cast<ULONG_PTR>(GetPropW(
                            target, kPropShowDesktopRestorePrepared)) ==
                            restoreAnimationToken) {
                        restoreMaximized = WindowRestoresMaximized(target);
                        restoreOriginalExStyle =
                            GetWindowLongPtrW(target, GWL_EXSTYLE);
                        if (CanPrepareRestoreAnimation(target) &&
                            ReserveAsyncRestore(
                                target, &restoreReservationGeneration)) {
                            UpdateDwmTransitions(target, FALSE);
                            SetWindowCloak(target, TRUE);
                            UpdateShowDesktopOwnedSurfaceCloaks(
                                target, restoreAnimationToken, TRUE);
                            restoreReserved = true;
                        }
                    }
                    if (!restoreReserved) {
                        ReleaseShowDesktopAnimationIfCurrent(
                            target, restoreAnimationToken);
                        restoreAnimationToken = 0;
                    }
                }
            }
        }
    }

    if (restoreReserved) {
        // CTray restores foreign HWNDs immediately after this hook returns.
        // Explorer's cross-process DWM write isn't sufficient for every
        // Chromium/Electron surface. Ask the target process's independent
        // observer thread to submit and acknowledge the cloak before CTray's
        // native restore can expose the real window.
        if (!PrepareTargetShowDesktopRestoreCloak(
                target, restoreAnimationToken) &&
            IsDiagnosticLoggingEnabled()) {
            DWORD targetProcessId = 0;
            GetWindowThreadProcessId(target, &targetProcessId);
            Wh_Log(L"Show Desktop target cloak handshake unavailable "
                   L"hwnd=%p target_pid=%lu",
                   target, targetProcessId);
        }
        FlushDwmOrYield();
    }
    RaiseDesktop_Original(pThis, flags);

    if (restoreReserved) {
        auto* restoreData = new (std::nothrow) AsyncRestoreAnimData{
            target, restoreOriginalExStyle, restoreReservationGeneration,
            restoreMaximized, /*fastShowDesktopStart=*/TRUE,
            restoreAnimationToken};
        if (!restoreData ||
            !StartWorkerThread(AsyncRestoreAnimThread, restoreData)) {
            delete restoreData;
            AbortAsyncRestoreReservation(
                target, restoreReservationGeneration, restoreOriginalExStyle,
                /*initialRestoreSubmitted=*/true, restoreMaximized);
            ReleaseShowDesktopAnimationIfCurrent(target,
                                                  restoreAnimationToken);
        }
    }

    g_localShowDesktopOperationUntilTick.store(
        GetTickCount() + 500, std::memory_order_release);
    if (optimizeTopWindow) {
        if (g_showDesktopTopWindowOnly.load(std::memory_order_relaxed)) {
            PublishShowDesktopIntent(target, GetTickCount() + 500, false);
        } else {
            PublishShowDesktopIntent(nullptr, 0, false);
        }
    }
    g_showDesktopIntentHookActive = false;
}

static ShowDesktopBatchDecision ResolveShowDesktopMinimizeDecision(HWND hWnd) {
    if (!g_showDesktopTopWindowOnly.load(std::memory_order_relaxed)) {
        return ShowDesktopBatchDecision::None;
    }
    ShowDesktopBatchDecision decision = GetShowDesktopBatchDecision(hWnd);
    if (decision != ShowDesktopBatchDecision::None) return decision;

    // Symbol lookup is optional for forward compatibility. The live Win+D
    // chord still gives a shell-side fallback route a deterministic
    // one-window batch. Touchpad Show Desktop needs the _RaiseDesktop hook.
    const bool winDDown =
        (GetAsyncKeyState('D') & 0x8000) &&
        ((GetAsyncKeyState(VK_LWIN) & 0x8000) ||
         (GetAsyncKeyState(VK_RWIN) & 0x8000));
    if (IsShellExplorerProcess() && winDDown) {
        PublishShowDesktopIntent(hWnd, GetTickCount() + 500, true);
        return ShowDesktopBatchDecision::Animate;
    }
    return ShowDesktopBatchDecision::None;
}

static bool IsClassicShowDesktopOperation() {
    if (g_showDesktopTopWindowOnly.load(std::memory_order_relaxed) ||
        !IsShellExplorerProcess()) {
        return false;
    }
    if (g_showDesktopIntentHookActive) {
        g_localShowDesktopOperationUntilTick.store(
            GetTickCount() + 5000, std::memory_order_release);
        return true;
    }
    const DWORD now = GetTickCount();
    if (IsFutureTick(g_localShowDesktopOperationUntilTick.load(
                         std::memory_order_acquire),
                     now)) {
        // The optional-symbol fallback may outlive the physical key chord.
        // Refresh on every window so long classic batches stay serialized.
        g_localShowDesktopOperationUntilTick.store(
            now + 2000, std::memory_order_release);
        return true;
    }
    const bool winDDown =
        (GetAsyncKeyState('D') & 0x8000) &&
        ((GetAsyncKeyState(VK_LWIN) & 0x8000) ||
         (GetAsyncKeyState(VK_RWIN) & 0x8000));
    if (!winDDown) return false;
    // Fallback for builds where the optional CTray symbol isn't available.
    // Refreshing this on each minimize covers a long multi-window batch.
    g_localShowDesktopOperationUntilTick.store(
        now + 2000, std::memory_order_release);
    return true;
}

class ClassicShowDesktopCaptureClaim {
    bool held_ = false;

public:
    ~ClassicShowDesktopCaptureClaim() {
        if (!held_) return;
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_classicShowDesktopCaptureClaimed = false;
    }

    bool Acquire(HWND hWnd) {
        const DWORD targetThreadId =
            GetWindowThreadProcessId(hWnd, nullptr);
        if (targetThreadId && targetThreadId == GetCurrentThreadId()) {
            // Owner-thread paths use PrintWindow rather than composed-screen
            // capture, so another ghost can't contaminate their bitmap.
            return true;
        }
        if (!IsClassicShowDesktopOperation()) return true;

        const DWORD deadline =
            GetTickCount() + AnimConstants::ClassicShowDesktopWaitMs;
        bool observedBlocker = false;
        while (!g_unloading.load(std::memory_order_relaxed) &&
               !g_showDesktopTopWindowOnly.load(std::memory_order_relaxed) &&
               IsWindow(hWnd) &&
               static_cast<LONG>(GetTickCount() - deadline) < 0) {
            bool blocker = false;
            {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                blocker = g_classicShowDesktopCaptureClaimed;
                if (!blocker) {
                    for (HWND activeHwnd : g_AnimActive) {
                        if (activeHwnd != hWnd && IsWindow(activeHwnd)) {
                            blocker = true;
                            break;
                        }
                    }
                }
                // Check the ghost count while holding the same state lock used
                // for active-session publication/release. This closes the
                // endpoint handoff where g_AnimActive is erased just before
                // the ghost HWND is destroyed.
                if (!blocker &&
                    g_animationGhostCount.load(std::memory_order_acquire) > 0) {
                    blocker = true;
                }
                if (!blocker) {
                    g_classicShowDesktopCaptureClaimed = true;
                    held_ = true;
                }
            }
            if (held_) {
                if (observedBlocker) FlushDwmOrYield();
                return true;
            }
            observedBlocker = true;
            // Dispatch only nonqueued sent messages while CTray waits. Posted
            // input stays ordered behind the current Show Desktop operation;
            // a reentrant minimize can compete for the claim without a mutex
            // being held across message dispatch.
            MsgWaitForMultipleObjectsEx(0, nullptr, 10, QS_SENDMESSAGE,
                                        MWMO_INPUTAVAILABLE);
            MSG msg{};
            PeekMessageW(&msg, nullptr, 0, 0,
                         PM_NOREMOVE | PM_QS_SENDMESSAGE);
        }
        return false;
    }
};

static void SettleClassicShowDesktopNativeMinimize(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    const DWORD deadline =
        GetTickCount() + AnimConstants::ShowDesktopNativeSettleMs;
    while (IsWindowVisible(hWnd) && !IsIconic(hWnd) &&
           !g_unloading.load(std::memory_order_relaxed) &&
           static_cast<LONG>(GetTickCount() - deadline) < 0) {
        // ShowWindowAsync can return before the target thread removes this
        // window from the composed desktop. Let a native fallback settle so
        // the following classic Win+D screen capture doesn't see it as an
        // occluder and incorrectly fall back too.
        MsgWaitForMultipleObjectsEx(0, nullptr, 10, QS_SENDMESSAGE,
                                    MWMO_INPUTAVAILABLE);
        MSG msg{};
        PeekMessageW(&msg, nullptr, 0, 0,
                     PM_NOREMOVE | PM_QS_SENDMESSAGE);
    }
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) FlushDwmOrYield();
}

static ULONG_PTR CreateShowDesktopMarkerToken(HWND hWnd) {
    const ULONG_PTR serial =
        g_NextShowDesktopMarkerToken.fetch_add(1,
                                               std::memory_order_relaxed) +
        1;
    ULONG_PTR token =
        serial ^ reinterpret_cast<ULONG_PTR>(hWnd) ^
        (static_cast<ULONG_PTR>(GetCurrentProcessId()) *
         static_cast<ULONG_PTR>(0x9E3779B1u)) ^
        static_cast<ULONG_PTR>(GetTickCount());
    return token ? token : 1;
}

static void ClearShowDesktopMarkerPropertyIfCurrent(HWND hWnd,
                                                     ULONG_PTR token) {
    if (!hWnd || !token) return;
    if (reinterpret_cast<ULONG_PTR>(GetPropW(
            hWnd, kPropShowDesktopNativeMinimize)) == token) {
        RemovePropW(hWnd, kPropShowDesktopNativeMinimize);
    }
}

static bool SuppressShowDesktopInstantMinimizeTransition(HWND hWnd,
                                                          ULONG_PTR token) {
    if (!hWnd || !token || !IsWindow(hWnd)) return false;

    // Explorer can synchronously enter the same hook in the target process.
    // The HWND property makes the first hook the owner so nested hooks don't
    // overwrite the original DWM transition state or race its cleanup.
    if (GetPropW(hWnd, kPropShowDesktopInstantMinimize)) return true;

    BOOL transitionsDisabled = FALSE;
    // This attribute is documented as set-only. Some DWM versions still
    // expose its current value, so preserve an existing application-owned
    // disable when available; a failed query must not prevent the documented
    // DwmSetWindowAttribute path from running.
    if (SUCCEEDED(DwmGetWindowAttribute(
            hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &transitionsDisabled,
            sizeof(transitionsDisabled))) &&
        transitionsDisabled) {
        // Respect a transition policy which the application already owns.
        return true;
    }
    if (!SetPropW(hWnd, kPropShowDesktopInstantMinimize,
                  reinterpret_cast<HANDLE>(token))) {
        return false;
    }
    if (reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopInstantMinimize)) != token) {
        return true;
    }

    BOOL disable = TRUE;
    if (FAILED(DwmSetWindowAttribute(
            hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable,
            sizeof(disable)))) {
        if (reinterpret_cast<ULONG_PTR>(
                GetPropW(hWnd, kPropShowDesktopInstantMinimize)) == token) {
            RemovePropW(hWnd, kPropShowDesktopInstantMinimize);
        }
        return false;
    }
    return true;
}

static void RestoreShowDesktopInstantMinimizeTransition(
    HWND hWnd, ULONG_PTR expectedToken = 0) {
    if (!hWnd || !IsWindow(hWnd)) return;
    const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopInstantMinimize));
    if (!token || (expectedToken && token != expectedToken)) return;
    if (!RemovePropW(hWnd, kPropShowDesktopInstantMinimize)) return;
    UpdateDwmTransitions(hWnd, TRUE);
}

static BOOL CALLBACK RestoreShowDesktopInstantMinimizeEnumProc(HWND hWnd,
                                                                LPARAM) {
    RemovePropW(hWnd, kPropShowDesktopRestorePrepared);
    RestoreShowDesktopInstantMinimizeTransition(hWnd);
    return TRUE;
}

static void RestoreAllShowDesktopInstantMinimizeTransitions() {
    EnumWindows(RestoreShowDesktopInstantMinimizeEnumProc, 0);
}

static DWORD WINAPI ShowDesktopMarkerCleanupThread(LPVOID) {
    for (;;) {
        struct CleanupAction {
            HWND hWnd;
            ULONG_PTR token;
            bool restoreTransition;
        };
        std::vector<CleanupAction> cleanupActions;
        bool keepWaiting = false;
        {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            const DWORD now = GetTickCount();
            for (auto it = g_ShowDesktopMarkerCleanups.begin();
                 it != g_ShowDesktopMarkerCleanups.end();) {
                const HWND hWnd = it->first;
                const ULONG_PTR token = it->second.token;
                const bool tokenCurrent =
                    IsWindow(hWnd) &&
                    reinterpret_cast<ULONG_PTR>(GetPropW(
                        hWnd, kPropShowDesktopNativeMinimize)) == token;
                if (!tokenCurrent) {
                    if (it->second.waitingForMinimize) {
                        cleanupActions.push_back({hWnd, token, true});
                    }
                    it = g_ShowDesktopMarkerCleanups.erase(it);
                    continue;
                }
                const bool reachedExpectedState =
                    it->second.waitingForMinimize ? IsIconic(hWnd)
                                                  : !IsIconic(hWnd);
                if (reachedExpectedState) {
                    if (it->second.waitingForMinimize) {
                        // Keep both HWND properties while minimized. They make
                        // the matching batch restore native and allow it to
                        // re-enable transitions before displaying the window.
                        it = g_ShowDesktopMarkerCleanups.erase(it);
                        continue;
                    }
                    auto markerIt = g_ShowDesktopNativeMarkers.find(hWnd);
                    if (markerIt != g_ShowDesktopNativeMarkers.end() &&
                        markerIt->second == token) {
                        g_ShowDesktopNativeMarkers.erase(markerIt);
                    }
                    cleanupActions.push_back({hWnd, token, false});
                    it = g_ShowDesktopMarkerCleanups.erase(it);
                    continue;
                }
                if (static_cast<LONG>(now - it->second.deadline) >= 0) {
                    if (it->second.waitingForMinimize) {
                        // A failed or hung asynchronous minimize must not leave
                        // native transitions disabled on a visible window.
                        auto markerIt = g_ShowDesktopNativeMarkers.find(hWnd);
                        if (markerIt != g_ShowDesktopNativeMarkers.end() &&
                            markerIt->second == token) {
                            g_ShowDesktopNativeMarkers.erase(markerIt);
                        }
                        cleanupActions.push_back({hWnd, token, true});
                    }
                    // For a restore timeout, leave the native marker intact so
                    // a later target-side restore can still bypass custom
                    // capture; another asynchronous call can schedule again.
                    it = g_ShowDesktopMarkerCleanups.erase(it);
                    continue;
                }
                keepWaiting = true;
                ++it;
            }
            if (g_ShowDesktopMarkerCleanups.empty()) {
                g_ShowDesktopMarkerCleanupWorkerRunning = false;
            }
        }
        for (const auto& action : cleanupActions) {
            ClearShowDesktopMarkerPropertyIfCurrent(action.hWnd,
                                                     action.token);
            if (action.restoreTransition) {
                RestoreShowDesktopInstantMinimizeTransition(action.hWnd,
                                                             action.token);
            }
        }
        if (!keepWaiting) return 0;
        if (g_unloading.load(std::memory_order_relaxed)) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            g_ShowDesktopMarkerCleanupWorkerRunning = false;
            return 0;
        }
        Sleep(10);
    }
}

static void ScheduleShowDesktopMarkerCleanup(HWND hWnd, ULONG_PTR token,
                                             bool waitingForMinimize = false) {
    if (!hWnd || !token) return;
    bool workerStartFailed = false;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(hWnd) ||
            reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropShowDesktopNativeMinimize)) != token) {
            return;
        }
        g_ShowDesktopMarkerCleanups[hWnd] = {
            token, GetTickCount() + AnimConstants::NativeStateWaitMs,
            waitingForMinimize};
        if (g_ShowDesktopMarkerCleanupWorkerRunning) return;
        g_ShowDesktopMarkerCleanupWorkerRunning = true;
        if (!StartWorkerThread(ShowDesktopMarkerCleanupThread, nullptr)) {
            g_ShowDesktopMarkerCleanupWorkerRunning = false;
            g_ShowDesktopMarkerCleanups.erase(hWnd);
            workerStartFailed = true;
        }
    }
    if (workerStartFailed && waitingForMinimize) {
        RestoreShowDesktopInstantMinimizeTransition(hWnd, token);
    }
}

static ULONG_PTR MarkShowDesktopNativeMinimize(HWND hWnd) {
    if (!hWnd) return 0;
    ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopNativeMinimize));
    if (!token) token = CreateShowDesktopMarkerToken(hWnd);
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (!IsWindow(hWnd) ||
        g_unloading.load(std::memory_order_relaxed)) {
        return 0;
    }
    if (!GetPropW(hWnd, kPropShowDesktopNativeMinimize) &&
        !SetPropW(hWnd, kPropShowDesktopNativeMinimize,
                  reinterpret_cast<HANDLE>(token))) {
        return 0;
    }
    token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopNativeMinimize));
    if (!token) return 0;
    g_ShowDesktopNativeMarkers[hWnd] = token;
    return token;
}

static bool ShouldBypassShowDesktopRestore(HWND hWnd) {
    const ShowDesktopBatchDecision decision =
        GetShowDesktopBatchDecision(hWnd);
    const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopNativeMinimize));
    if (decision == ShowDesktopBatchDecision::Skip ||
        (decision == ShowDesktopBatchDecision::Animate && token)) {
        return true;
    }

    // The marker guarantees that a matching *batch* restore stays native when
    // the skipped/failed minimize has no snapshot. It must not permanently
    // opt this HWND out of custom restores: once the Show Desktop intent has
    // ended, an individual taskbar restore gets the normal prepare/capture
    // path again.
    if (token) {
        RestoreShowDesktopInstantMinimizeTransition(hWnd, token);
        ClearShowDesktopMarkerPropertyIfCurrent(hWnd, token);
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto markerIt = g_ShowDesktopNativeMarkers.find(hWnd);
        if (markerIt != g_ShowDesktopNativeMarkers.end() &&
            markerIt->second == token) {
            g_ShowDesktopNativeMarkers.erase(markerIt);
        }
        auto cleanupIt = g_ShowDesktopMarkerCleanups.find(hWnd);
        if (cleanupIt != g_ShowDesktopMarkerCleanups.end() &&
            cleanupIt->second.token == token) {
            g_ShowDesktopMarkerCleanups.erase(cleanupIt);
        }
    }
    return false;
}

static void ClearShowDesktopMarkerAfterRestore(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || IsIconic(hWnd)) return;
    const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
        GetPropW(hWnd, kPropShowDesktopNativeMinimize));
    RestoreShowDesktopInstantMinimizeTransition(hWnd, token);
    if (!token) return;
    ClearShowDesktopMarkerPropertyIfCurrent(hWnd, token);
    std::lock_guard<std::mutex> lock(g_StateMutex);
    auto markerIt = g_ShowDesktopNativeMarkers.find(hWnd);
    if (markerIt != g_ShowDesktopNativeMarkers.end() &&
        markerIt->second == token) {
        g_ShowDesktopNativeMarkers.erase(markerIt);
    }
}

thread_local unsigned g_showDesktopNativeRestoreDepth = 0;
struct ScopedShowDesktopNativeRestore {
    ScopedShowDesktopNativeRestore() {
        ++g_showDesktopNativeRestoreDepth;
    }
    ~ScopedShowDesktopNativeRestore() {
        --g_showDesktopNativeRestoreDepth;
    }
};

static bool WaitForForeignSwitchAnimation(HWND hWnd) {
    const DWORD targetThreadId =
        GetWindowThreadProcessId(hWnd, nullptr);
    if (!targetThreadId || targetThreadId == GetCurrentThreadId() ||
        !GetPropW(hWnd, kPropSwitchAnimationActive)) {
        return false;
    }
    const DWORD deadline =
        GetTickCount() + AnimConstants::ShowDesktopSwitchGhostWaitMs;
    while (IsWindow(hWnd) &&
           GetPropW(hWnd, kPropSwitchAnimationActive) &&
           !g_unloading.load(std::memory_order_relaxed) &&
           static_cast<LONG>(GetTickCount() - deadline) < 0) {
        // The switch worker owns only the thumbnail/ghost thread. Draining a
        // composition frame here lets its bounded handoff finish without
        // synchronously entering the destination window's UI thread.
        FlushDwmOrYield();
    }
    return IsWindow(hWnd) &&
           !GetPropW(hWnd, kPropSwitchAnimationActive);
}

enum class MinimizeKick { None, Immediate, Deferred };
static MinimizeKick KickMinimizeAnimation(HWND hWnd, bool desktopFocusOnUnhide = false,
                                          bool allowUnhide = true, int showCmd = 0,
                                          NativeMinimizeBarrier* nativeMinimizeBarrier = nullptr) {
    NativeMinimizeBarrierOwner nativeBarrierOwner(nativeMinimizeBarrier);
    MinRestoreRetarget retarget = RetargetLiveMinRestore(hWnd, false);
    if (retarget == MinRestoreRetarget::BusyOther &&
        WaitForForeignSwitchAnimation(hWnd)) {
        retarget = RetargetLiveMinRestore(hWnd, false);
    }
    if (retarget == MinRestoreRetarget::Accepted) {
        return MinimizeKick::Deferred;
    }
    if (retarget == MinRestoreRetarget::BusyOther) return MinimizeKick::None;
    if (!g_minimizeAnimation.load(std::memory_order_relaxed)) return MinimizeKick::None;
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return MinimizeKick::None;
    const ShowDesktopBatchDecision showDesktopDecision =
        ResolveShowDesktopMinimizeDecision(hWnd);
    if (showDesktopDecision == ShowDesktopBatchDecision::Skip) {
        // Skipped windows have no process-local snapshot. Their matching
        // batch Show Desktop restore remains native. Suppress only the native
        // minimize transition so background windows reach their real iconic
        // state immediately while the selected leader plays its custom effect.
        const ULONG_PTR token = MarkShowDesktopNativeMinimize(hWnd);
        if (token) {
            SuppressShowDesktopInstantMinimizeTransition(hWnd, token);
        }
        return MinimizeKick::None;
    }
    ULONG_PTR showDesktopAnimationToken = 0;
    if (showDesktopDecision == ShowDesktopBatchDecision::Animate) {
        showDesktopAnimationToken = TryClaimShowDesktopAnimation(hWnd);
        if (!showDesktopAnimationToken) {
            if (IsDiagnosticLoggingEnabled()) {
                DWORD targetProcessId = 0;
                GetWindowThreadProcessId(hWnd, &targetProcessId);
                Wh_Log(
                    L"Show Desktop leader animation already owned/unavailable "
                    L"hwnd=%p target_pid=%lu; continuing native minimize",
                    hWnd, targetProcessId);
            }
            // The Explorer-side hook starts the one custom leader before its
            // native call can synchronously enter the target process. A nested
            // loser must still deliver that native minimize, only without
            // creating a second ghost for the same HWND.
            return MinimizeKick::Immediate;
        }
    }
    if (IsHungAppWindow(hWnd) || !ShouldAnimateWindow(hWnd)) {
        ReleaseShowDesktopAnimationIfCurrent(hWnd,
                                             showDesktopAnimationToken);
        if (showDesktopDecision == ShowDesktopBatchDecision::Animate) {
            MarkShowDesktopNativeMinimize(hWnd);
        }
        return MinimizeKick::None;
    }
    ClearShowDesktopMarkerAfterRestore(hWnd);
    if (showDesktopDecision == ShowDesktopBatchDecision::Animate) {
        // Don't reveal/focus an auto-hidden taskbar in the middle of a batch.
        allowUnhide = false;
    }
    BOOL requestedUnhide = FALSE;
    HWND hTray = NULL;
    HWND hNext = NULL;
    BOOL taskbarFocusBorrowed = FALSE;
    HMONITOR hMon = NULL;
    int effectStyle = -1;
    if (g_minRestoreShuffleEffect.load(std::memory_order_relaxed)) {
        ReadMinRestorePairToken(hWnd, &effectStyle);
    }
    if (effectStyle < 0) {
        effectStyle = ResolveMinRestoreEffectStyle();
    }
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
    ClassicShowDesktopCaptureClaim classicCaptureClaim;
    if (!classicCaptureClaim.Acquire(hWnd)) {
        // A hung/very long previous animation must not make the next screen
        // capture include that ghost or churn DWM transitions before falling
        // back. Let this window use the native Show Desktop path instead.
        ReleaseShowDesktopAnimationIfCurrent(hWnd,
                                             showDesktopAnimationToken);
        return MinimizeKick::None;
    }
    UpdateDwmTransitions(hWnd, FALSE);
    if (showDesktopDecision == ShowDesktopBatchDecision::Animate) {
        // The optimized Win+D leader must not hold CTray's synchronous Show
        // Desktop loop for a cold UIA lookup or the normal 200/500-ms
        // first-frame timeout. Keep a short first-frame handshake so the
        // source is cloaked before the initiating API submits its exact native
        // minimize command.
        BOOL suppressNativeMinimize = FALSE;
        const bool started = StartAnimation(
            hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), FALSE,
            FALSE, 0, NULL, FALSE, FALSE, NULL, showCmd, effectStyle, FALSE,
            nativeBarrierOwner.release(), 0, &suppressNativeMinimize, FALSE, 0,
            /*fastShowDesktopStart=*/TRUE,
            showDesktopAnimationToken);
        if (started) return MinimizeKick::Immediate;
        ReleaseShowDesktopAnimationIfCurrent(hWnd,
                                             showDesktopAnimationToken);
        if (!suppressNativeMinimize) {
            MarkShowDesktopNativeMinimize(hWnd);
        }
        return suppressNativeMinimize ? MinimizeKick::Deferred
                                      : MinimizeKick::None;
    }
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
                           &revealRequestSettled, FALSE, 0, FALSE)) {
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
        &suppressNativeMinimize, FALSE, 0, FALSE);
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
    if (GetShowDesktopBatchDecision(hWnd) ==
        ShowDesktopBatchDecision::Animate) {
        const ULONG_PTR token = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopAnimationOwner));
        UpdateShowDesktopOwnedSurfaceCloaks(hWnd, token, TRUE);
        // A Show Desktop restore can synchronously expose a foreign/native
        // surface as soon as the original API runs. Commit the cloak first.
        FlushDwmOrYield();
    }
    return true;
}
static bool CommitRestoreAnimation(HWND hWnd, BOOL restoreMaximizedHint) {
    const BOOL fastShowDesktopStart =
        GetShowDesktopBatchDecision(hWnd) ==
        ShowDesktopBatchDecision::Animate;
    const ULONG_PTR showDesktopAnimationToken =
        fastShowDesktopStart
            ? reinterpret_cast<ULONG_PTR>(GetPropW(
                  hWnd, kPropShowDesktopAnimationOwner))
            : 0;
    const bool started =
        StartAnimation(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE,
                       FALSE, 0, NULL, FALSE, FALSE, NULL, 0, -1, FALSE,
                       nullptr, 0, nullptr, restoreMaximizedHint, 0,
                       fastShowDesktopStart,
                       showDesktopAnimationToken);
    if (!started) {
        ReleaseShowDesktopAnimationIfCurrent(
            hWnd, showDesktopAnimationToken);
    }
    if (!started && restoreMaximizedHint && !IsAnimating(hWnd)) {
        // The native restore was already issued, but no long-lived animation
        // session exists to receive duplicates. Keep only the short debounce.
        ArmMaximizedRestoreGuard(
            hWnd, static_cast<DWORD>(GetDoubleClickTime() + 100));
    }
    return started;
}

static bool AreAsyncRestoreBoundsReady(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || IsIconic(hWnd)) return false;

    RECT windowRect{};
    RECT animationRect{};
    if (!GetAnimationWindowRects(hWnd, &windowRect, &animationRect)) {
        return false;
    }
    const int width = animationRect.right - animationRect.left;
    const int height = animationRect.bottom - animationRect.top;
    if (width <= 0 || height <= 0) return false;

    int expectedWidth = 0;
    int expectedHeight = 0;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        auto snapshotIt = g_WndSnapshots.find(hWnd);
        if (snapshotIt != g_WndSnapshots.end() &&
            snapshotIt->second.windowToken &&
            reinterpret_cast<ULONG_PTR>(GetPropW(
                hWnd, kPropSnapshotCache)) ==
                snapshotIt->second.windowToken) {
            expectedWidth = snapshotIt->second.w;
            expectedHeight = snapshotIt->second.h;
        }
    }

    if (!expectedWidth || !expectedHeight) {
        WINDOWPLACEMENT placement{sizeof(placement)};
        if (GetWindowPlacement(hWnd, &placement)) {
            expectedWidth = placement.rcNormalPosition.right -
                            placement.rcNormalPosition.left;
            expectedHeight = placement.rcNormalPosition.bottom -
                             placement.rcNormalPosition.top;
        }
    }

    // IsIconic can turn false one shell step before DWM replaces the compact
    // minimized representation (for example 181x25) with the restored frame.
    // Compare against the cached/normal size so that transient rectangle is
    // never used to create the restore surface.
    return expectedWidth <= 0 || expectedHeight <= 0 ||
           (static_cast<int64_t>(width) * 4 >= expectedWidth &&
            static_cast<int64_t>(height) * 4 >= expectedHeight);
}

DWORD WINAPI AsyncRestoreAnimThread(LPVOID lpParam) {
    auto* restoreData = (AsyncRestoreAnimData*)lpParam;
    const HWND hWnd = restoreData->hWnd;
    const LONG_PTR originalExStyle = restoreData->originalExStyle;
    const uint64_t reservationGeneration = restoreData->reservationGeneration;
    const BOOL restoreMaximized = restoreData->restoreMaximized;
    const BOOL fastShowDesktopStart = restoreData->fastShowDesktopStart;
    const ULONG_PTR showDesktopAnimationToken =
        restoreData->showDesktopAnimationToken;
    delete restoreData;

    const DWORD deadline = GetTickCount() + 1000;
    int stableBoundsSamples = 0;
    while (!g_unloading.load(std::memory_order_relaxed) && IsWindow(hWnd) &&
           (LONG)(GetTickCount() - deadline) < 0) {
        if (AreAsyncRestoreBoundsReady(hWnd)) {
            // Reassert the cross-process cloak after the native restore. Some
            // frameworks rebuild their DWM surface while leaving iconic state,
            // which can otherwise expose the real frame underneath the ghost.
            SetWindowCloak(hWnd, TRUE);
            UpdateShowDesktopOwnedSurfaceCloaks(
                hWnd, showDesktopAnimationToken, TRUE);
            if (++stableBoundsSamples >= 2) break;
        } else {
            stableBoundsSamples = 0;
        }
        Sleep(5);
    }
    if (!g_unloading.load(std::memory_order_relaxed) && IsWindow(hWnd) &&
        IsWindowVisible(hWnd) && stableBoundsSamples >= 2) {
        UpdateDwmTransitions(hWnd, FALSE);
        SetWindowCloak(hWnd, TRUE);
        UpdateShowDesktopOwnedSurfaceCloaks(
            hWnd, showDesktopAnimationToken, TRUE);
        FlushDwmOrYield();
        bool wantRising = true;
        if (GetAsyncRestoreReservation(hWnd, reservationGeneration, &wantRising) &&
            StartAnimation(hWnd, wantRising, originalExStyle, TRUE, FALSE, 0, NULL, FALSE, FALSE,
                           NULL, 0, -1, FALSE, nullptr, reservationGeneration,
                           nullptr, restoreMaximized, 0,
                           fastShowDesktopStart,
                           showDesktopAnimationToken)) {
            return 0;
        }
    }
    FinalizeAsyncRestoreReservation(hWnd, reservationGeneration, originalExStyle,
                                    /*initialRestoreSubmitted=*/true,
                                    restoreMaximized);
    ReleaseShowDesktopAnimationIfCurrent(hWnd,
                                         showDesktopAnimationToken);
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
static bool IsTaskbarSysCommand(HWND hWnd, LPARAM lParam) {
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
    const bool restoreCommand = cmd == SW_RESTORE || cmd == SW_SHOWNORMAL;
    if (restoreCommand && g_showDesktopNativeRestoreDepth) {
        return ShowWindow_Original(hWnd, cmd);
    }
    if (restoreCommand && IsShowDesktopRestorePrepared(hWnd)) {
        // Explorer prepared the optimized Show Desktop leader before entering
        // CTray's restore loop and will start the one shared animation after
        // the native operation returns. Establish the target-owned cloak
        // before Windows can expose the restored surface, then reassert it
        // after the framework has had a chance to rebuild that surface.
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        const BOOL result = ShowWindow_Original(hWnd, cmd);
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        return result;
    }
    if (IsMinimizeCommand(cmd)) {
        const BOOL wasVisible = IsWindowVisible(hWnd);
        const bool classicShowDesktop = IsClassicShowDesktopOperation();
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
            ClearShowDesktopMarkerAfterRestore(hWnd);
            return wasVisible;
        }
        const BOOL result = ShowWindow_Original(hWnd, cmd);
        CompleteNativeMinimizeBarrier(
            barrier, wasVisible ? NativeMinimizeState::SyncCompleted
                                : NativeMinimizeState::Failed);
        if (classicShowDesktop && kick == MinimizeKick::None && result) {
            SettleClassicShowDesktopNativeMinimize(hWnd);
        }
        ClearShowDesktopMarkerAfterRestore(hWnd);
        return result;
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) && IsIconic(hWnd)) {
        if (RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
        if (ShouldBypassShowDesktopRestore(hWnd)) {
            RestoreShowDesktopInstantMinimizeTransition(hWnd);
            ScopedShowDesktopNativeRestore bypassScope;
            const BOOL result = ShowWindow_Original(hWnd, cmd);
            ClearShowDesktopMarkerAfterRestore(hWnd);
            return result;
        }
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
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) &&
        ShouldBypassShowDesktopRestore(hWnd)) {
        RestoreShowDesktopInstantMinimizeTransition(hWnd);
        ScopedShowDesktopNativeRestore bypassScope;
        const BOOL result = ShowWindow_Original(hWnd, cmd);
        ClearShowDesktopMarkerAfterRestore(hWnd);
        return result;
    }
    if (cmd == SW_RESTORE && IsShellTaskbarRestoreCall(hWnd) &&
        ShouldSuppressRedundantMaximizedRestore(hWnd)) {
        // This hook can run in a different injected process while the restore
        // worker owns and cloaks the real window. Keep suppression side-effect
        // free; the owning worker activates only after it safely uncloaks.
        return IsWindowVisible(hWnd);
    }
    if (!IsOurWindow(hWnd)) return ShowWindow_Original(hWnd, cmd);
    const bool mayCreateStableWindow =
        !IsWindowVisible(hWnd) && IsLaunchCommand(cmd);
    if (IsShowCmdForWinEvent(cmd)) {
        EnsureWinEventThreadStarted();
        PublishShowDesktopCloakEndpointForWindow(hWnd);
    }
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
        if (mayCreateStableWindow) ScheduleStableGpuWarmup();
        return result;
    }
    const BOOL result = ShowWindow_Original(hWnd, cmd);
    if (mayCreateStableWindow) ScheduleStableGpuWarmup();
    return result;
}
BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int cmd) {
    const bool restoreCommand = cmd == SW_RESTORE || cmd == SW_SHOWNORMAL;
    if (restoreCommand && g_showDesktopNativeRestoreDepth) {
        return ShowWindowAsync_Original(hWnd, cmd);
    }
    if (restoreCommand && IsShowDesktopRestorePrepared(hWnd)) {
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        const BOOL result = ShowWindowAsync_Original(hWnd, cmd);
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        return result;
    }
    if (IsMinimizeCommand(cmd)) {
        const bool classicShowDesktop = IsClassicShowDesktopOperation();
        NativeMinimizeBarrier* barrier = CreateNativeMinimizeBarrier();
        if (!barrier) {
            if (RetargetLiveMinRestore(hWnd, false) == MinRestoreRetarget::Accepted) {
                return TRUE;
            }
            return ShowWindowAsync_Original(hWnd, cmd);
        }
        AddRefNativeMinimizeBarrier(barrier);
        const MinimizeKick kick = TryMinimizeAnim(hWnd, barrier, cmd);
        if (kick == MinimizeKick::Deferred) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            return TRUE;
        }
        if (!BeginNativeMinimizeSubmission(barrier)) {
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::Cancelled);
            ClearShowDesktopMarkerAfterRestore(hWnd);
            return TRUE;
        }
        const BOOL result = ShowWindowAsync_Original(hWnd, cmd);
        CompleteNativeMinimizeBarrier(
            barrier, result ? NativeMinimizeState::AsyncSubmitted
                            : NativeMinimizeState::Failed);
        if (classicShowDesktop && kick == MinimizeKick::None && result) {
            SettleClassicShowDesktopNativeMinimize(hWnd);
        }
        const ULONG_PTR instantToken = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopInstantMinimize));
        if (result && instantToken) {
            ScheduleShowDesktopMarkerCleanup(
                hWnd, instantToken, /*waitingForMinimize=*/true);
        } else if (!result) {
            ClearShowDesktopMarkerAfterRestore(hWnd);
        }
        return result;
    }
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) &&
        RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
    if ((cmd == SW_RESTORE || cmd == SW_SHOWNORMAL) &&
        ShouldBypassShowDesktopRestore(hWnd)) {
        const ULONG_PTR markerToken = reinterpret_cast<ULONG_PTR>(
            GetPropW(hWnd, kPropShowDesktopNativeMinimize));
        BOOL result = FALSE;
        {
            RestoreShowDesktopInstantMinimizeTransition(hWnd, markerToken);
            ScopedShowDesktopNativeRestore bypassScope;
            result = ShowWindowAsync_Original(hWnd, cmd);
        }
        if (result && markerToken) {
            // Keep the token through any target-side nested restore, then
            // consume it once the asynchronous native restore actually lands.
            ScheduleShowDesktopMarkerCleanup(hWnd, markerToken);
        }
        return result;
    }
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
        const BOOL fastShowDesktopStart =
            GetShowDesktopBatchDecision(hWnd) ==
            ShowDesktopBatchDecision::Animate;
        const ULONG_PTR showDesktopAnimationToken =
            fastShowDesktopStart
                ? reinterpret_cast<ULONG_PTR>(GetPropW(
                      hWnd, kPropShowDesktopAnimationOwner))
                : 0;
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
        if (fastShowDesktopStart) {
            UpdateShowDesktopOwnedSurfaceCloaks(
                hWnd, showDesktopAnimationToken, TRUE);
            FlushDwmOrYield();
        }
        const BOOL result = ShowWindowAsync_Original(hWnd, cmd);
        auto* restoreData = result
                                ? new (std::nothrow) AsyncRestoreAnimData{
                                      hWnd, originalExStyle, reservationGeneration,
                                      restoreMaximized,
                                      fastShowDesktopStart,
                                      showDesktopAnimationToken}
                                : nullptr;
        if (!restoreData || !StartWorkerThread(AsyncRestoreAnimThread, restoreData)) {
            delete restoreData;
            AbortAsyncRestoreReservation(hWnd, reservationGeneration, originalExStyle,
                                          result != FALSE, restoreMaximized);
            ReleaseShowDesktopAnimationIfCurrent(
                hWnd, showDesktopAnimationToken);
        }
        return result;
    }
    if (!IsOurWindow(hWnd)) return ShowWindowAsync_Original(hWnd, cmd);
    const bool mayCreateStableWindow =
        !IsWindowVisible(hWnd) && IsLaunchCommand(cmd);
    if (IsShowCmdForWinEvent(cmd)) {
        EnsureWinEventThreadStarted();
        PublishShowDesktopCloakEndpointForWindow(hWnd);
    }
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
        if (mayCreateStableWindow) ScheduleStableGpuWarmup();
        return result;
    }
    const BOOL result = ShowWindowAsync_Original(hWnd, cmd);
    if (mayCreateStableWindow) ScheduleStableGpuWarmup();
    return result;
}
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND insertAfter, int x, int y, int cx, int cy, UINT flags) {
    if (!(flags & (SWP_HIDEWINDOW | SWP_SHOWWINDOW))) {
        return SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
    }
    if (!IsOurWindow(hWnd)) return SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
    const bool mayCreateStableWindow =
        (flags & SWP_SHOWWINDOW) && !IsWindowVisible(hWnd);
    if (flags & SWP_SHOWWINDOW) {
        EnsureWinEventThreadStarted();
        PublishShowDesktopCloakEndpointForWindow(hWnd);
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
            if (mayCreateStableWindow) ScheduleStableGpuWarmup();
            return result;
        }
    }
    const BOOL result =
        SetWindowPos_Original(hWnd, insertAfter, x, y, cx, cy, flags);
    if (mayCreateStableWindow) ScheduleStableGpuWarmup();
    return result;
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
                ClearShowDesktopMarkerAfterRestore(hWnd);
                return 0;
            }
            const LRESULT result = DefWindowProcW_Original(hWnd, msg, wParam, lParam);
            CompleteNativeMinimizeBarrier(barrier, NativeMinimizeState::SyncCompleted);
            ClearShowDesktopMarkerAfterRestore(hWnd);
            return result;
        }
        if (cmd == SC_RESTORE) {
            if (g_showDesktopNativeRestoreDepth) {
                return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
            }
            if (IsShowDesktopRestorePrepared(hWnd)) {
                ReassertPreparedShowDesktopRestoreCloak(hWnd);
                const LRESULT result =
                    DefWindowProcW_Original(hWnd, msg, wParam, lParam);
                ReassertPreparedShowDesktopRestoreCloak(hWnd);
                return result;
            }
            if (RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return 0;
            if (ShouldBypassShowDesktopRestore(hWnd)) {
                RestoreShowDesktopInstantMinimizeTransition(hWnd);
                ScopedShowDesktopNativeRestore bypassScope;
                const LRESULT result =
                    DefWindowProcW_Original(hWnd, msg, wParam, lParam);
                ClearShowDesktopMarkerAfterRestore(hWnd);
                return result;
            }
            if (IsTaskbarSysCommand(hWnd, lParam) &&
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
    const bool restoreCommand =
        placement && (placement->showCmd == SW_RESTORE ||
                      placement->showCmd == SW_SHOWNORMAL);
    if (restoreCommand && g_showDesktopNativeRestoreDepth) {
        return SetWindowPlacement_Original(hWnd, placement);
    }
    if (restoreCommand && IsShowDesktopRestorePrepared(hWnd)) {
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        const BOOL result = SetWindowPlacement_Original(hWnd, placement);
        ReassertPreparedShowDesktopRestoreCloak(hWnd);
        return result;
    }
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
            ClearShowDesktopMarkerAfterRestore(hWnd);
            return TRUE;
        }
        const BOOL result = SetWindowPlacement_Original(hWnd, placement);
        CompleteNativeMinimizeBarrier(
            barrier, result ? NativeMinimizeState::SyncCompleted
                            : NativeMinimizeState::Failed);
        ClearShowDesktopMarkerAfterRestore(hWnd);
        return result;
    }
    if (placement && (placement->showCmd == SW_RESTORE || placement->showCmd == SW_SHOWNORMAL) &&
        RetargetLiveMinRestore(hWnd, true) == MinRestoreRetarget::Accepted) return TRUE;
    if (placement &&
        (placement->showCmd == SW_RESTORE ||
         placement->showCmd == SW_SHOWNORMAL) &&
        ShouldBypassShowDesktopRestore(hWnd)) {
        RestoreShowDesktopInstantMinimizeTransition(hWnd);
        ScopedShowDesktopNativeRestore bypassScope;
        const BOOL result = SetWindowPlacement_Original(hWnd, placement);
        ClearShowDesktopMarkerAfterRestore(hWnd);
        return result;
    }
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
    if (!StartAnimation(hWnd, TRUE, originalExStyle, FALSE,
                        FALSE, 0, nullptr,
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
static BOOL CALLBACK EnumWindowsInitProc(HWND hWnd, LPARAM) {
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
    if (g_unloading.load(std::memory_order_relaxed) ||
        !NeedsLocalWinEventThread()) {
        StopWinEventThread();
    }
}
static DWORD WINAPI ShellOwnershipProbeThread(LPVOID) {
    const DWORD started = GetTickCount();
    while (!g_unloading.load(std::memory_order_relaxed)) {
        const DWORD shellPid = GetShellExplorerProcessId();
        if (shellPid) {
            // A secondary explorer.exe can own folder windows without ever
            // becoming the desktop shell. Once another PID owns the shell,
            // this process has nothing left to probe for.
            if (shellPid != GetCurrentProcessId()) return 0;
            StopWinEventThread();
            EnsureWinEventThreadStarted();
            if (g_switchAnimation.load(std::memory_order_relaxed)) {
                StartSwitchThreads();
            }
            return 0;
        }
        const DWORD elapsed = GetTickCount() - started;
        // Custom shells might never publish either Explorer ownership HWND.
        // Keep the startup recovery bounded instead of polling forever.
        if (elapsed >= 30000) return 0;
        Sleep(elapsed < 5000 ? 50 : 500);
    }
    return 0;
}
BOOL Wh_ModInit() {
    // Session 0 is isolated from the interactive desktop. Such services can
    // never own a window that this mod can animate. Chromium/Electron helper
    // roles likewise don't own their application's top-level window. Avoid
    // settings/shared-state initialization and API hooks in both cases.
    if (IsNonInteractiveSessionProcess() ||
        IsNonWindowHostingHelperProcess()) {
        return FALSE;
    }
    LoadAnimSettings();
    if (!g_minimizeAnimation.load(std::memory_order_relaxed) &&
        !g_restoreAnimation.load(std::memory_order_relaxed) &&
        !g_closeAnimation.load(std::memory_order_relaxed) &&
        !g_switchAnimation.load(std::memory_order_relaxed) &&
        !g_launchAnimation.load(std::memory_order_relaxed)) {
        return FALSE;
    }
    InitSharedMemory();
    const bool explorerProcess = IsShellExplorerProcess();
    if (explorerProcess &&
        (g_minRestoreShuffleEffect.load(std::memory_order_relaxed) ||
         g_closeShuffleEffect.load(std::memory_order_relaxed))) {
        // Keep the tiny session-wide shuffle mapping alive even when GPU
        // warm-up is disabled and the application which consumed the latest
        // close effect exits immediately afterwards.
        EnsureSharedEffectShuffleState();
    }
    if (IsExplorerProcess()) {
        WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
            {
                {LR"(protected: void __cdecl CTray::_RaiseDesktop(enum RAISEDESKTOPFLAGS))"},
                &RaiseDesktop_Original,
                RaiseDesktop_Hook,
                true,
            },
        };
        if (!WindhawkUtils::HookSymbols(GetModuleHandleW(nullptr),
                                        explorerExeHooks,
                                        ARRAYSIZE(explorerExeHooks))) {
            Wh_Log(L"Optional Show Desktop symbol hook setup failed");
        } else if (!RaiseDesktop_Original) {
            Wh_Log(L"Show Desktop symbol unavailable; using Win+D key-state fallback");
        }
    }
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
    if (explorerProcess && NeedsLocalWinEventThread()) {
        EnsureWinEventThreadStarted();
    } else if (NeedsLocalWinEventThread()) {
        // Existing windows need the same-process Win+D restore observer even
        // when Alt+Tab animation is disabled. New windows start it lazily from
        // their first intercepted ShowWindow call.
        EnumWindows(EnumWindowsInitProc, 0);
    }
    if (!explorerProcess && IsExplorerProcess()) {
        StartWorkerThread(ShellOwnershipProbeThread, nullptr);
    }
    if (FindStableGpuWindow()) {
        ScheduleStableGpuWarmup();
    }
    return TRUE;
}
void Wh_ModSettingsChanged() { 
    // Drop work derived from the old settings before loading replacements.
    // Clear once more afterwards to close the small window in which an
    // already-scheduled predictor could have queued the old selection.
    CancelPendingGpuWarmup();
    bool wasSwitchAnim = g_switchAnimation.load(std::memory_order_relaxed);
    bool wasShowDesktopTopWindowOnly =
        g_showDesktopTopWindowOnly.load(std::memory_order_relaxed);
    LoadAnimSettings();
    CancelPendingGpuWarmup();
    bool isSwitchAnim = g_switchAnimation.load(std::memory_order_relaxed);
    bool isShowDesktopTopWindowOnly =
        g_showDesktopTopWindowOnly.load(std::memory_order_relaxed);
    if (IsShellExplorerProcess() &&
        (g_minRestoreShuffleEffect.load(std::memory_order_relaxed) ||
         g_closeShuffleEffect.load(std::memory_order_relaxed))) {
        EnsureSharedEffectShuffleState();
    }
    if (IsDiagnosticLoggingEnabled()) {
        Wh_Log(L"Settings changed switch=%d->%d showDesktopTop=%d->%d",
               wasSwitchAnim, isSwitchAnim, wasShowDesktopTopWindowOnly,
               isShowDesktopTopWindowOnly);
    }

    if (!g_gpuAcceleration.load(std::memory_order_relaxed) &&
        !g_closeGpuAcceleration.load(std::memory_order_relaxed)) {
        ShutdownGpuRenderService();
        if (IsDiagnosticLoggingEnabled()) {
            Wh_Log(
                L"GPU renderer released: both acceleration settings are off");
        }
    } else if (FindStableGpuWindow()) {
        ScheduleStableGpuWarmup();
    }

    if (wasShowDesktopTopWindowOnly != isShowDesktopTopWindowOnly &&
        IsShellExplorerProcess()) {
        // Don't let an intent from the previous setting state leak across a
        // rapid disable/re-enable. Existing per-window native markers
        // intentionally remain until their matching restore, because those
        // skipped windows don't own a snapshot.
        PublishShowDesktopIntent(nullptr, 0, false);
        if (!isShowDesktopTopWindowOnly) {
            RestoreAllShowDesktopInstantMinimizeTransitions();
        }
    }

    if (isSwitchAnim && !wasSwitchAnim) {
        StartSwitchThreads();
    } 
    else if (!isSwitchAnim && wasSwitchAnim) {
        StopSwitchThreads();
    }
    if (NeedsLocalWinEventThread()) {
        if (IsShellExplorerProcess()) {
            EnsureWinEventThreadStarted();
        } else {
            EnumWindows(EnumWindowsInitProc, 0);
        }
    } else {
        StopWinEventThread();
    }
}
void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
    CancelPendingGpuWarmup();
    // The ownership probe is a registered worker and is the only worker that
    // can request the separately-owned Explorer foreground thread. Join it
    // before detaching that thread, in addition to the start-side lock/check.
    JoinWorkerThreads();
    ShutdownGpuRenderService();
    g_gpuWarmupRunning.store(false, std::memory_order_release);
    g_stableGpuWarmupRunning.store(false, std::memory_order_release);
    StopSwitchThreads();
}
void Wh_ModUninit() {
    std::vector<HWND> stuck;
    std::vector<std::pair<HWND, ULONG_PTR>> showDesktopMarkers;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        stuck.assign(g_AnimActive.begin(), g_AnimActive.end());
        showDesktopMarkers.assign(g_ShowDesktopNativeMarkers.begin(),
                                  g_ShowDesktopNativeMarkers.end());
        g_ShowDesktopNativeMarkers.clear();
        g_ShowDesktopMarkerCleanups.clear();
        g_ShowDesktopMarkerCleanupWorkerRunning = false;
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
    RestoreAllShowDesktopInstantMinimizeTransitions();
    for (const auto& [hWnd, token] : showDesktopMarkers) {
        ClearShowDesktopMarkerPropertyIfCurrent(hWnd, token);
    }
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        while (!g_WndSnapshots.empty()) {
            EraseSnapshotLocked(g_WndSnapshots.begin());
        }
        g_WndSnapshotBytes = 0;
        g_TaskbarDockPositions.clear();
        g_TaskbarDockFallbackPositions.clear();
        g_ProcessDockPositions.clear();
        g_TaskbarDockLookupGenerations.clear();
        g_TaskbarDockLookupIdentities.clear();
        g_TaskbarDockLookupStartedTicks.clear();
        g_TaskbarDockNegativeUntilTicks.clear();
        g_TaskbarDockPositiveUntilTicks.clear();
        g_ProcessNameCache.clear(); g_LaunchSeen.clear();
        g_TaskbarWindowSetSignature = 0;
        g_TaskbarWindowSetSignatureInitialized = false;
        g_seenTaskbarLayoutEpoch = 0;
        g_seenTaskbarExplorerPid = 0;
    }
    CloseSharedEffectShuffleState();
    CloseSharedState();
}
