// ==WindhawkMod==
// @id              fast-wallpaper-switcher
// @name            Fast Wallpaper Switcher
// @description     Browse your wallpapers in an animated carousel and switch with 12 stunning transitions like Ink, Burn, Freeze and Dust. Hotkeys and per-wallpaper subject cutouts.
// @homepage        https://github.com/sarthakaksh/windhawk-wallpaper-adaptive-widgets
// @version         1.1.0
// @author          sarthakaksh
// @github          https://github.com/sarthakaksh
// @include         windhawk.exe
// @license         GPL-3.0
// @compilerOptions -ld2d1 -ldwrite -lwindowscodecs -lole32 -lshell32 -lshlwapi -ldwmapi -lgdi32 -luser32 -luuid -loleaut32 -lmfplat -lmfreadwrite -lmfuuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fast Wallpaper Switcher

A fast, good-looking wallpaper picker for Windows. Press a hotkey, browse your
wallpapers in a carousel, and switch with an animated transition: Ink, Burn,
Freeze, Dust, Glitch, Hyperspace and more.

> ⭐ **Like it? Star the project on GitHub:**
> **https://github.com/sarthakaksh/windhawk-wallpaper-adaptive-widgets**
> Screenshots, sample wallpapers, subject cutouts, and companion mods
> (Audio Visualizer + Lyrics, Desktop Clock) are all there. Found a bug or have
> an idea? Open an issue or a pull request, and share your setups!

![Fast Wallpaper Switcher](https://raw.githubusercontent.com/sarthakaksh/windhawk-wallpaper-adaptive-widgets/main/Picker1.png)
![Picker styles](https://raw.githubusercontent.com/sarthakaksh/windhawk-wallpaper-adaptive-widgets/main/picker2.png)

## ✨ Features

- **12 picker styles**, from a classic carousel to full-screen layouts
  (Hexagons, Spiral, Fisheye grid, Tunnel)
- **12 animated transitions**: Fade, Circle, Slide, Zoom, Ink, Slash, Whip,
  Glitch, Hyperspace, Burn, Freeze, Dust
- **Subject cutouts** for Ink and Dust, remembered per wallpaper
- **Hotkeys** for next / previous / random without opening the picker
- **Scriptable**: trigger everything from other tools via named events
- Works with the **Desktop Clock** mod from the same repo, which switches
  layout at exactly the right moment so it never flashes the old one

## 🚀 Quick start

1. Install and enable the mod.
2. Press `Ctrl+Alt+Shift+W`.
3. The first time, choose the folder that contains your wallpapers.
4. Browse, press `Enter`, and enjoy the transition.

**Tip:** for the cleanest result, turn off Windows' own wallpaper fade.
Press `Win+R`, run `SystemPropertiesPerformance.exe`, choose **Custom**, and
uncheck **Animate controls and elements inside windows**.

## ⌨️ The picker

A strip of your wallpapers appears in the middle of the screen. The one in the
center is big: that's the preview.

| Key / mouse                      | Does                                   |
|----------------------------------|----------------------------------------|
| Left / Right (or A / D), wheel   | browse                                 |
| Up / Down (Hexagons, Fisheye)    | move a row up / down                   |
| Home / End, PageUp / PageDown    | first / last, 5 back / forward         |
| Enter / Space, click the middle  | set it (with the transition)           |
| Click a side picture             | bring it to the middle                 |
| T                                | change the transition                  |
| S                                | change the picker style (normal)       |
| H                                | full-screen styles (Hexagons, Spiral,  |
|                                  | Fisheye grid, Tunnel)                  |
| U                                | choose a subject cutout (Ink / Dust)   |
| F                                | choose another folder                  |
| Esc, click anywhere else         | close                                  |

Extra controls in the full-screen styles:
- **Hexagons:** the mouse hover selects, click sets.
- **Fisheye:** the wheel or drag scrolls rows.
- **Tunnel:** arrows or wheel fly forward / back, click a frame to pick it.

## 🌌 Ink and Dust: subject cutouts

These two transitions can wrap around the main subject of your wallpaper, such
as a character or a mountain, using a **PNG cutout** (transparent background,
**same size as the wallpaper**).

1. Select the wallpaper in the picker and press `T` until Ink or Dust is chosen.
2. Press `U` and pick the cutout PNG. It's remembered **per wallpaper**.
3. Press `Delete` in the picker to forget the cutout. `Cancel` in the dialog falls back to the
   no-cutout version.

Without a cutout, Ink uses ink drops and Dust uses a wind wave.
`Enter` never asks anything: it uses the cutout saved for that wallpaper.

Ready-made cutouts for the sample wallpapers are in the `cutout/` folder of the
GitHub repo.

## 🎲 Hotkeys

Next / previous / random wallpaper without opening the picker. They are editable
in the settings, and an empty field means off. If one doesn't work, another
program (komorebi's whkd, for example) is already using it. Check the mod's log.

## 🔌 Triggering it from other tools

The mod creates named events. Setting one runs the action, so it works with
status bars, scripts, Stream Deck, task scheduler and so on.

| Event                        | Action        |
|------------------------------|---------------|
| `Local\FastWallpaper.Picker` | open picker   |
| `Local\FastWallpaper.Next`   | next picture  |
| `Local\FastWallpaper.Prev`   | previous      |
| `Local\FastWallpaper.Random` | random        |

PowerShell one-liner:

    powershell -NoProfile -WindowStyle Hidden -Command "[Threading.EventWaitHandle]::OpenExisting('Local\FastWallpaper.Picker').Set()"


## Good to know

- **It changes your Windows wallpaper setting.** Choosing a wallpaper sets it
  through Windows itself, the same as Settings > Personalization > Background.
  Windows saves it in your profile, so the last wallpaper stays after you
  disable or remove the mod. It also replaces a Slideshow or Windows Spotlight
  background. The mod writes nothing else outside Windhawk's own storage.
- **It runs in its own process.** The mod runs in a separate Windhawk process,
  not inside Explorer, so a problem in it can't take the taskbar down.
- **The heavy transitions use memory.** Each one builds full-screen pictures,
  which can take a few hundred MB at 4K, more with "All monitors". On a
  low-memory PC, use Fade, Circle, Slide or Zoom.
- **Why a separate mod?** Simple Wallpaper Switcher is a light picker with
  simple transitions. This mod is a much larger engine: 12 picker layouts,
  8 pixel-based transitions (Ink, Slash, Whip, Glitch, Hyperspace, Burn,
  Freeze, Dust), per-wallpaper subject cutouts, hotkeys and named events for
  next / previous / random, and cooperation with the Desktop Clock and Audio
  Visualizer mods. Folding all of that into a small mod would change its
  scope, so it is kept separate.
- **License.** GPL-3.0, because part of the code comes from the Mac Island &
  Dock mod.

## 🧩 Part of a bigger setup

This mod is part of **windhawk-wallpaper-adaptive-widgets**, a desktop that
changes with your wallpaper:

- 🎵 **Audio Visualizer + Lyrics**: remembers its layout per wallpaper
- 🕒 **Desktop Clock**: its style and position change per wallpaper
- 🖼️ **Sample wallpapers and cutouts** for the "behind the subject" 3D effect

👉 **https://github.com/sarthakaksh/windhawk-wallpaper-adaptive-widgets**

## 🤝 Support and contributing

- 🐛 Bug or feature request? Open an issue on GitHub.
- 🔧 Want to add a transition or picker style? Pull requests are welcome.
- 🖼️ Made a nice wallpaper + cutout combo? Share it in the repo.
- ⭐ A star on GitHub helps other people find the project.

## 🙏 Credits

- Built on top of the **Mac Island & Dock** Windhawk mod
- **Ink transition** inspired by [pocket-dots](https://github.com/diegoMalagrida/pocket-dots)
  by diegoMalagrida
- Many ideas inspired by [skwd-wall](https://github.com/liixini/skwd-wall)
  by liixini
- Made by **sarthakaksh**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folder: ""
  $name: Wallpaper folder
  $description: Full path of the folder with your pictures. Leave empty to choose it from the picker (the first time it asks, or press F in it).
- transition: circle
  $name: Transition
  $options:
  - fade: Fade
  - circle: Circle reveal
  - slide: Slide
  - zoom: Zoom
  - ink: Ink (sketch the subject, then bleed)
  - slash: Slash (tilted strips lift and flip in 3D)
  - whip: Whip (sliced plane pulls back and whip-spins 540°)
  - glitch: Glitch (pixel-sort streaks and RGB split sweep across)
  - warp: Hyperspace (zoom streaks, flash, then the new wallpaper drops in)
  - burn: Burn (fire eats the old wallpaper, embers and sparks)
  - freeze: Freeze (frost spreads over the old wallpaper, then thaws into the new one)
  - dust: Dust (the old wallpaper crumbles into grains and the wind blows them away; uses the subject cutout like Ink)
- duration: 700
  $name: Transition length (ms)
  $description: 0 = no transition, the wallpaper just changes.
- slashDuration: 1200
  $name: Slash transition length (ms)
  $description: Used by the Slash transition (6 strips, staggered, so it needs a bit longer). Still 0 = no transition if the length above is 0.
- whipDuration: 1500
  $name: Whip transition length (ms)
  $description: Used by the Whip transition (pull back, 540° spin, landing). Still 0 = no transition if the length above is 0.
- glitchDuration: 1100
  $name: Glitch transition length (ms)
  $description: Used by the Glitch transition. Still 0 = no transition if the length above is 0.
- warpDuration: 1300
  $name: Hyperspace transition length (ms)
  $description: Used by the Hyperspace Warp transition. Still 0 = no transition if the length above is 0.
- burnDuration: 2200
  $name: Burn transition length (ms)
  $description: Used by the Burn transition (fire spreading, then the embers dying out). Still 0 = no transition if the length above is 0.
- freezeDuration: 2400
  $name: Freeze transition length (ms)
  $description: Used by the Freeze transition (frost spreading, then thawing). Still 0 = no transition if the length above is 0.
- dustDuration: 2600
  $name: Dust transition length (ms)
  $description: Used by the Dust transition (the crumbling, then the last grains blowing away). Still 0 = no transition if the length above is 0.
- whipBackdrop: dark
  $name: Whip backdrop
  $description: The colour behind the strips in the Whip transition.
  $options:
  - dark: Near-black
  - beige: Light beige
- whipSpin: "360"
  $name: Whip spin
  $description: How far the plane spins in the Whip transition. The new wallpaper always ends facing you.
  $options:
  - "180": Half turn (180°)
  - "360": One turn (360°)
  - "540": One and a half turns (540°)
  - "720": Two turns (720°)
- inkDuration: 2400
  $name: Ink transition length, with subject (ms)
  $description: Used by the Ink transition when the wallpaper has a subject (it has three phases, so it needs longer). Still 0 = no transition if the length above is 0.
- inkDropsDuration: 1800
  $name: Ink transition length, no subject / ink drops (ms)
  $description: Used by the Ink transition when the wallpaper has no subject (just ink drops). Still 0 = no transition if the length above is 0.
- inkLineWidth: 60
  $name: Ink - pencil line thickness (%)
  $description: Thickness of the thin outline (and its glow) drawn around the subject. 100 = the old look.
- inkGlow: 50
  $name: Ink - glow / harshness (%)
  $description: How strong and bright the outline and its glow are. 0 = almost invisible, 100 = the old look.
- inkRibbonWidth: 70
  $name: Ink - marker stroke thickness (%)
  $description: Thickness of the band of new wallpaper that follows the outline. 100 = the old look.
- pickerStyle: strip
  $name: Picker style
  $description: How the pictures are laid out in the picker (press S inside it to change).
  $options:
  - strip: Strip (flat row)
  - fan: Fan (arc of cards)
  - slant: Slant (leaning cards, wide middle)
  - cards: Cards (upright cards, wide middle, file info)
  - coverflow: Coverflow (turned cards in perspective, floor reflection)
  - deck: Deck (a stack of cards, the middle one on top)
  - flow: Flow (overlapping cards on a smooth S-curve ribbon)
  - liquid: Liquid (slanted accordion that bends and RGB-splits with speed)
  - hex: Hexagons (honeycomb grid, hover to light up)
  - spiral: Spiral (circles gliding along a double spiral)
  - fisheye: Fisheye grid (tidy grid, tiles swell near the mouse)
  - tunnel: Depth tunnel (frames on corridor walls, fly through)
- ambient: false
  $name: Ambient backdrop
  $description: Full-screen picker styles show a blurred, darkened copy of the selected wallpaper behind them (cross-fades as you browse). Off = flat dark dim.
- monitors: cursor
  $name: Monitors to change
  $options:
  - cursor: The monitor under the mouse
  - primary: The primary monitor
  - all: All monitors
- hotkeyPicker: Ctrl+Alt+Shift+W
  $name: Hotkey - open the picker
  $description: "Like Ctrl+Alt+Shift+W, Win+Shift+F5. Empty = off."
- hotkeyNext: Ctrl+Alt+Shift+Right
  $name: Hotkey - next picture
- hotkeyPrev: Ctrl+Alt+Shift+Left
  $name: Hotkey - previous picture
- hotkeyRandom: ""
  $name: Hotkey - random picture
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <wincodec.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

constexpr PCWSTR kWallpaperTypes[] = {L".jpg",  L".jpeg", L".jfif",
                                      L".png",  L".bmp",  L".webp"};
constexpr size_t kWallpaperMax = 1000;

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_PICKFOLDER = WM_APP + 2;
constexpr UINT WM_APP_THUMB = WM_APP + 3;
constexpr UINT WM_APP_PICKSUBJECT = WM_APP + 4;
// Message of the Desktop Clock Widget mod, posted to its control window: "the
// wallpaper was just changed, switch your layout now". Ignored if that mod
// isn't loaded.
constexpr UINT kClockWallpaperNow = WM_APP + 2;
constexpr UINT WM_APP_STEPDONE = WM_APP + 5;
constexpr UINT WM_APP_CLOSEDIALOG = WM_APP + 6;

// Windows showing a new wallpaper: looked at this often, for this long at
// most; without a look, the transition's window goes after a moment.
constexpr DWORD kWallpaperSampleMs = 25;
constexpr double kWallpaperShownSeconds = 1.5;
constexpr DWORD kWallpaperShownFallbackMs = 250;
// How long the finished picture stays over the desktop while Windows brings
// the new wallpaper in underneath (it is visible to recordings too).
// Minimum time the finished picture stays over the desktop while Windows
// brings the new wallpaper in underneath. The audio visualizer (a child of the
// desktop) can only show once the picture is gone, so keep this short.
// How long the finished picture still covers the clock widget after the
// animation ends (the clock then shows above the still picture).
constexpr double kWidgetRevealDelaySeconds = 0.20;
// How long the finished picture stays over the desktop. The audio visualizer
// can only show once it is gone, so this is also the visualizer's delay.
// Keep it at least as long as the clock delay above.
constexpr double kWallpaperHoldSeconds = 0.80;
// Extra hold per MB of the new picture file (big pictures decode slower).
constexpr double kWallpaperHoldPerMB = 0.04;
// The hold never goes beyond this.
constexpr double kWallpaperHoldMaxSeconds = 2.0;

enum class Transition { Fade, Circle, Slide, Zoom, Ink, Slash, Whip, Glitch, Warp, Burn, Freeze, Dust, Count };
constexpr PCWSTR kTransitionNames[] = {L"Fade", L"Circle", L"Slide", L"Zoom",
                                       L"Ink",  L"Slash",  L"Whip",  L"Glitch",
                                       L"Warp", L"Burn", L"Freeze", L"Dust"};
// Transitions that use the remembered subject cutout (U / Del in the picker).
inline bool UsesSubject(int t) {
    return t == (int)Transition::Ink || t == (int)Transition::Dust;
}

enum class PickerStyle { Strip, Fan, Slant, Cards, Coverflow, Deck, Flow, Liquid, Hex, Spiral, Fisheye, Tunnel, Count };
constexpr PCWSTR kPickerStyleNames[] = {L"Strip", L"Fan",  L"Slant",
                                        L"Cards", L"Coverflow", L"Deck", L"Flow", L"Liquid",
                                        L"Hex", L"Spiral", L"Fisheye",
                                        L"Tunnel"};
// The styles that fill the whole screen (H cycles these, S the others).
constexpr bool IsFullStyle(int style) {
    return style >= (int)PickerStyle::Hex && style < (int)PickerStyle::Count;
}

std::atomic<bool> g_unloading{false};
std::atomic<int> g_threads{0};  // every worker thread of the mod
std::atomic<int> g_busy{0};     // transitions running
std::atomic<unsigned> g_carouselGeneration{0};
// Ink look (percent), read by the transition threads.
std::atomic<int> g_inkLine{60};
std::atomic<int> g_inkGlow{50};
std::atomic<int> g_inkRibbon{70};
std::atomic<int> g_whipLight{0};  // Whip backdrop: 0 dark, 1 beige
std::atomic<int> g_whipSpin{2};   // Whip spin, in half turns

HANDLE g_thread = nullptr;
IFileDialog* g_openDialog = nullptr;  // (main thread only) while a dialog shows

// Duplicate handles of every thread the mod starts, so unloading can wait
// until each one has really exited.
std::vector<HANDLE> g_workers;
SRWLOCK g_workersLock = SRWLOCK_INIT;

HANDLE StartTrackedThread(LPSECURITY_ATTRIBUTES attributes,
                          SIZE_T stackSize,
                          LPTHREAD_START_ROUTINE start,
                          LPVOID parameter,
                          DWORD flags,
                          LPDWORD threadId) {
    HANDLE thread =
        CreateThread(attributes, stackSize, start, parameter, flags, threadId);
    HANDLE copy = nullptr;
    if (thread && DuplicateHandle(GetCurrentProcess(), thread,
                                  GetCurrentProcess(), &copy, SYNCHRONIZE,
                                  FALSE, 0)) {
        AcquireSRWLockExclusive(&g_workersLock);
        // Let go of the ones that already finished.
        g_workers.erase(
            std::remove_if(g_workers.begin(), g_workers.end(),
                           [](HANDLE handle) {
                               if (WaitForSingleObject(handle, 0) !=
                                   WAIT_OBJECT_0) {
                                   return false;
                               }
                               CloseHandle(handle);
                               return true;
                           }),
            g_workers.end());
        g_workers.push_back(copy);
        ReleaseSRWLockExclusive(&g_workersLock);
    }
    return thread;
}

// Every CreateThread below goes through the tracker.
#define CreateThread StartTrackedThread
HANDLE g_stop = nullptr;
HANDLE g_events[4] = {};  // picker, next, prev, random
volatile HWND g_hwnd = nullptr;
std::wstring g_lastPath;
std::wstring g_pendingWallpaper;  // (main thread only) waits for the subject dialog
// A next / previous / random pressed while a transition was still running
// (main thread only): it runs when the transition ends instead of being lost.
bool g_stepPending = false;
int g_pendingStep = 0;
// Hotkeys registered right now (main thread only), and the retries left for
// the ones that weren't (the system may not be ready at startup).
bool g_hotkeyOk[4] = {};
int g_hotkeyRetries = 0;
constexpr UINT_PTR kHotkeyRetryTimer = 1;

struct Config {
    std::wstring folder;
    int transition = 1;
    int pickerStyle = 0;  // 0 strip, 1 fan
    int durationMs = 700;
    int inkMs = 2400;
    int inkDropsMs = 1800;
    int slashMs = 1200;
    int whipMs = 1500;
    int glitchMs = 1100;
    int warpMs = 1300;
    int burnMs = 2200;
    int freezeMs = 2400;
    int dustMs = 2600;
    int monitors = 0;  // 0 cursor, 1 primary, 2 all
    bool ambient = true;
    std::wstring hotkeys[4];
};
Config g_cfg;  // (main thread only)

// Actions: the events' order, and the hotkeys' ids (+ 1).
enum Action { ActionPicker = 0, ActionNext = 1, ActionPrev = 2, ActionRandom = 3 };

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

double NowSeconds() {
    static const LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)freq.QuadPart;
}

void WaitForFrame() {
    if (FAILED(DwmFlush())) {
        Sleep(8);
    }
}

std::wstring GetSettingString(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring GetStoredString(PCWSTR key) {
    wchar_t buffer[MAX_PATH * 2];
    buffer[0] = 0;
    Wh_GetStringValue(key, buffer, ARRAYSIZE(buffer));
    return buffer;
}

// ---------------------------------------------------------------------------
// Hotkeys: "Ctrl+Alt+Shift+Right"
// ---------------------------------------------------------------------------

bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        std::wstring token = text.substr(start, end - start);
        start = end + 1;
        // trim + lower
        while (!token.empty() && iswspace(token.front())) token.erase(0, 1);
        while (!token.empty() && iswspace(token.back())) token.pop_back();
        for (auto& ch : token) ch = (wchar_t)towlower(ch);
        if (token.empty()) {
            continue;
        }
        if (token == L"ctrl" || token == L"control") {
            *mods |= MOD_CONTROL;
        } else if (token == L"alt") {
            *mods |= MOD_ALT;
        } else if (token == L"shift") {
            *mods |= MOD_SHIFT;
        } else if (token == L"win" || token == L"windows" ||
                   token == L"super") {
            *mods |= MOD_WIN;
        } else if (token.size() == 1 &&
                   ((token[0] >= L'a' && token[0] <= L'z') ||
                    (token[0] >= L'0' && token[0] <= L'9'))) {
            *vk = (UINT)towupper(token[0]);
        } else if (token[0] == L'f' && token.size() >= 2 &&
                   iswdigit(token[1])) {
            int n = _wtoi(token.c_str() + 1);
            if (n >= 1 && n <= 24) {
                *vk = VK_F1 + n - 1;
            }
        } else if (token == L"left") {
            *vk = VK_LEFT;
        } else if (token == L"right") {
            *vk = VK_RIGHT;
        } else if (token == L"up") {
            *vk = VK_UP;
        } else if (token == L"down") {
            *vk = VK_DOWN;
        } else if (token == L"space") {
            *vk = VK_SPACE;
        } else if (token == L"enter" || token == L"return") {
            *vk = VK_RETURN;
        } else if (token == L"tab") {
            *vk = VK_TAB;
        } else if (token == L"home") {
            *vk = VK_HOME;
        } else if (token == L"end") {
            *vk = VK_END;
        } else if (token == L"pageup") {
            *vk = VK_PRIOR;
        } else if (token == L"pagedown") {
            *vk = VK_NEXT;
        } else if (token == L"insert") {
            *vk = VK_INSERT;
        } else if (token == L"delete") {
            *vk = VK_DELETE;
        }
    }
    return *vk != 0;
}

// Registers the hotkeys that aren't registered yet. Returns how many failed.
int TryRegisterHotkeys(bool log) {
    int failed = 0;
    for (int i = 0; i < 4; i++) {
        UINT mods, vk;
        if (g_hotkeyOk[i] || g_cfg.hotkeys[i].empty() ||
            !ParseHotkey(g_cfg.hotkeys[i], &mods, &vk)) {
            continue;
        }
        if (RegisterHotKey(g_hwnd, i + 1, mods | MOD_NOREPEAT, vk)) {
            g_hotkeyOk[i] = true;
        } else {
            failed++;
            if (log) {
                Wh_Log(L"Could not register hotkey \"%s\" (already used by "
                       L"another program?)",
                       g_cfg.hotkeys[i].c_str());
            }
        }
    }
    return failed;
}

void RegisterHotkeys() {
    KillTimer(g_hwnd, kHotkeyRetryTimer);
    for (int i = 0; i < 4; i++) {
        UnregisterHotKey(g_hwnd, i + 1);
        g_hotkeyOk[i] = false;
    }
    if (TryRegisterHotkeys(true) > 0) {
        // Try again every 3 s for a minute (e.g. the system wasn't ready).
        g_hotkeyRetries = 20;
        SetTimer(g_hwnd, kHotkeyRetryTimer, 3000, nullptr);
    }
}

// ---------------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------------

void LoadConfig() {
    g_cfg.folder = GetStoredString(L"pickedFolder");
    if (g_cfg.folder.empty()) {
        g_cfg.folder = GetSettingString(L"folder");
    }
    const int stored = Wh_GetIntValue(L"pickedTransition", -1);
    if (stored >= 0 && stored < (int)Transition::Count) {
        g_cfg.transition = stored;
    } else {
        const std::wstring t = GetSettingString(L"transition");
        g_cfg.transition = t == L"fade"    ? 0
                           : t == L"slide" ? 2
                           : t == L"zoom"  ? 3
                           : t == L"ink"   ? 4
                           : t == L"slash" ? 5
                           : t == L"whip"  ? 6
                           : t == L"glitch" ? 7
                           : t == L"warp" ? 8
                           : t == L"burn" ? 9
                           : t == L"freeze" ? 10
                           : t == L"dust" ? 11
                                           : 1;
    }
    const int storedStyle = Wh_GetIntValue(L"pickedStyle", -1);
    if (storedStyle >= 0 && storedStyle < (int)PickerStyle::Count) {
        g_cfg.pickerStyle = storedStyle;
    } else {
        const std::wstring ps = GetSettingString(L"pickerStyle");
        g_cfg.pickerStyle = ps == L"fan"         ? (int)PickerStyle::Fan
                            : ps == L"slant"     ? (int)PickerStyle::Slant
                            : ps == L"cards"     ? (int)PickerStyle::Cards
                            : ps == L"coverflow" ? (int)PickerStyle::Coverflow
                            : ps == L"deck"      ? (int)PickerStyle::Deck
                            : ps == L"flow"      ? (int)PickerStyle::Flow
                            : ps == L"liquid"    ? (int)PickerStyle::Liquid
                            : ps == L"hex"       ? (int)PickerStyle::Hex
                            : ps == L"spiral"    ? (int)PickerStyle::Spiral
                            : ps == L"fisheye"   ? (int)PickerStyle::Fisheye
                            : ps == L"tunnel"    ? (int)PickerStyle::Tunnel
                                                 : 0;
    }
    g_cfg.durationMs = std::clamp(Wh_GetIntSetting(L"duration"), 0, 5000);
    g_cfg.inkMs = std::clamp(Wh_GetIntSetting(L"inkDuration"), 300, 10000);
    g_cfg.inkDropsMs =
        std::clamp(Wh_GetIntSetting(L"inkDropsDuration"), 300, 10000);
    g_cfg.slashMs = std::clamp(Wh_GetIntSetting(L"slashDuration"), 300, 10000);
    g_cfg.whipMs = std::clamp(Wh_GetIntSetting(L"whipDuration"), 300, 10000);
    g_cfg.glitchMs = std::clamp(Wh_GetIntSetting(L"glitchDuration"), 300, 10000);
    g_cfg.warpMs = std::clamp(Wh_GetIntSetting(L"warpDuration"), 300, 10000);
    g_cfg.burnMs = std::clamp(Wh_GetIntSetting(L"burnDuration"), 300, 10000);
    g_cfg.freezeMs = std::clamp(Wh_GetIntSetting(L"freezeDuration"), 300, 10000);
    g_cfg.dustMs = std::clamp(Wh_GetIntSetting(L"dustDuration"), 300, 10000);
    g_whipLight = GetSettingString(L"whipBackdrop") == L"beige" ? 1 : 0;
    g_whipSpin = std::clamp(
        (_wtoi(GetSettingString(L"whipSpin").c_str()) + 90) / 180, 1, 8);
    g_inkLine = std::clamp(Wh_GetIntSetting(L"inkLineWidth"), 10, 300);
    g_inkGlow = std::clamp(Wh_GetIntSetting(L"inkGlow"), 0, 300);
    g_inkRibbon = std::clamp(Wh_GetIntSetting(L"inkRibbonWidth"), 20, 300);
    const std::wstring m = GetSettingString(L"monitors");
    g_cfg.monitors = m == L"primary" ? 1 : m == L"all" ? 2 : 0;
    g_cfg.ambient = Wh_GetIntSetting(L"ambient") != 0;
    g_cfg.hotkeys[ActionPicker] = GetSettingString(L"hotkeyPicker");
    g_cfg.hotkeys[ActionNext] = GetSettingString(L"hotkeyNext");
    g_cfg.hotkeys[ActionPrev] = GetSettingString(L"hotkeyPrev");
    g_cfg.hotkeys[ActionRandom] = GetSettingString(L"hotkeyRandom");
}

// ---------------------------------------------------------------------------
// The pictures in a folder, sorted as Explorer sorts them
// ---------------------------------------------------------------------------

std::vector<std::wstring> ListWallpapersUncached(const std::wstring& folder);

// (main thread only) The last list, reused while the folder is unchanged (its
// last-write time moves when pictures are added, removed or renamed).
std::wstring g_listFolder;
FILETIME g_listStamp{};
std::vector<std::wstring> g_listCache;
bool g_listValid = false;

std::vector<std::wstring> ListWallpapers(const std::wstring& folder) {
    WIN32_FILE_ATTRIBUTE_DATA attr{};
    const bool known =
        !folder.empty() &&
        GetFileAttributesEx(folder.c_str(), GetFileExInfoStandard, &attr);
    if (known && g_listValid &&
        _wcsicmp(g_listFolder.c_str(), folder.c_str()) == 0 &&
        CompareFileTime(&g_listStamp, &attr.ftLastWriteTime) == 0) {
        return g_listCache;
    }
    std::vector<std::wstring> paths = ListWallpapersUncached(folder);
    g_listValid = known;
    if (known) {
        g_listFolder = folder;
        g_listStamp = attr.ftLastWriteTime;
        g_listCache = paths;
    }
    return paths;
}

std::vector<std::wstring> ListWallpapersUncached(const std::wstring& folder) {
    std::vector<std::wstring> paths;
    if (folder.empty()) {
        return paths;
    }
    const std::wstring base =
        folder.back() == L'\\' ? folder : folder + L"\\";
    WIN32_FIND_DATA data;
    HANDLE find = FindFirstFileEx((base + L"*").c_str(), FindExInfoBasic,
                                  &data, FindExSearchNameMatch, nullptr,
                                  FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return paths;
    }
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        PCWSTR extension = PathFindExtension(data.cFileName);
        for (PCWSTR type : kWallpaperTypes) {
            if (_wcsicmp(extension, type) == 0) {
                paths.push_back(base + data.cFileName);
                break;
            }
        }
    } while (paths.size() < kWallpaperMax && FindNextFile(find, &data));
    FindClose(find);
    std::sort(paths.begin(), paths.end(),
              [](const std::wstring& a, const std::wstring& b) {
                  return StrCmpLogicalW(a.c_str(), b.c_str()) < 0;
              });
    return paths;
}

// ---------------------------------------------------------------------------
// Windows' wallpaper of a monitor
// ---------------------------------------------------------------------------

bool MonitorIdFor(IDesktopWallpaper* wallpaper,
                  const RECT& monitor,
                  std::wstring* id) {
    UINT count = 0;
    wallpaper->GetMonitorDevicePathCount(&count);
    for (UINT i = 0; i < count; i++) {
        PWSTR path = nullptr;
        RECT rect{};
        if (SUCCEEDED(wallpaper->GetMonitorDevicePathAt(i, &path)) && path) {
            if (SUCCEEDED(wallpaper->GetMonitorRECT(path, &rect)) &&
                EqualRect(&rect, &monitor)) {
                *id = path;
                CoTaskMemFree(path);
                return true;
            }
            CoTaskMemFree(path);
        }
    }
    return false;
}

void SetMonitorWallpaper(const RECT& monitor, PCWSTR path) {
    ComPtr<IDesktopWallpaper> wallpaper;
    if (FAILED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(wallpaper.GetAddressOf())))) {
        return;
    }
    std::wstring id;
    if (MonitorIdFor(wallpaper.Get(), monitor, &id)) {
        wallpaper->SetWallpaper(id.c_str(), path);
        return;
    }
    // (Not matched: with a single monitor it's that one; otherwise the
    // others' wallpapers stay as they are.)
    UINT count = 0;
    wallpaper->GetMonitorDevicePathCount(&count);
    if (count == 1) {
        wallpaper->SetWallpaper(nullptr, path);
    }
}

std::wstring CurrentWallpaperPath(const RECT& monitor) {
    std::wstring result;
    ComPtr<IDesktopWallpaper> wallpaper;
    if (FAILED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(wallpaper.GetAddressOf())))) {
        return result;
    }
    std::wstring id;
    const bool found = MonitorIdFor(wallpaper.Get(), monitor, &id);
    PWSTR path = nullptr;
    if (SUCCEEDED(wallpaper->GetWallpaper(found ? id.c_str() : nullptr,
                                          &path)) &&
        path) {
        result = path;
        CoTaskMemFree(path);
    }
    return result;
}

// ---------------------------------------------------------------------------
// The picture as Windows will show it on a monitor of `size`
// ---------------------------------------------------------------------------

// What part of the scaled picture is shown (and so how big it is on the
// monitor): cut to the monitor if bigger, whole if smaller.
WICRect WallpaperCut(UINT scaledWidth, UINT scaledHeight, SIZE size) {
    const UINT w = std::min<UINT>(scaledWidth, (UINT)size.cx);
    const UINT h = std::min<UINT>(scaledHeight, (UINT)size.cy);
    WICRect rect;
    rect.X = (INT)((scaledWidth - w) / 2);
    rect.Y = (INT)((scaledHeight - h) / 2);
    rect.Width = (INT)w;
    rect.Height = (INT)h;
    return rect;
}

// The picture as Windows will show it on a monitor of `size`, by its
// wallpaper position: cut to fill it ("Fill"; "Span" too), whole inside it
// ("Fit"), stretched, or at its own size in the middle ("Center", "Tile");
// turned as the photo says (its EXIF orientation). `place` gets where it
// goes on the monitor.
ComPtr<IWICBitmapSource> LoadWallpaperPicture(
    IWICImagingFactory* wic,
    PCWSTR path,
    SIZE size,
    DESKTOP_WALLPAPER_POSITION position,
    D2D1_RECT_F* place) {
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(wic->CreateDecoderFromFilename(path, nullptr, GENERIC_READ,
                                              WICDecodeMetadataCacheOnDemand,
                                              decoder.GetAddressOf())) ||
        FAILED(decoder->GetFrame(0, frame.GetAddressOf()))) {
        return nullptr;
    }
    ComPtr<IWICBitmapSource> source;
    frame.As(&source);
    // Turned as the camera says (JPEG's EXIF orientation).
    ComPtr<IWICMetadataQueryReader> metadata;
    PROPVARIANT value;
    PropVariantInit(&value);
    if (SUCCEEDED(frame->GetMetadataQueryReader(metadata.GetAddressOf())) &&
        SUCCEEDED(metadata->GetMetadataByName(L"/app1/ifd/{ushort=274}",
                                              &value)) &&
        value.vt == VT_UI2) {
        const WICBitmapTransformOptions turn =
            value.uiVal == 3   ? WICBitmapTransformRotate180
            : value.uiVal == 6 ? WICBitmapTransformRotate90
            : value.uiVal == 8 ? WICBitmapTransformRotate270
                               : WICBitmapTransformRotate0;
        ComPtr<IWICBitmapFlipRotator> rotator;
        if (turn != WICBitmapTransformRotate0 &&
            SUCCEEDED(wic->CreateBitmapFlipRotator(rotator.GetAddressOf())) &&
            SUCCEEDED(rotator->Initialize(source.Get(), turn))) {
            ComPtr<IWICBitmapSource> rotated;
            rotator.As(&rotated);
            source = rotated;
        }
    }
    PropVariantClear(&value);
    UINT width = 0, height = 0;
    if (!source || FAILED(source->GetSize(&width, &height)) || !width ||
        !height) {
        return nullptr;
    }
    const double fill = std::max((double)size.cx / width,
                                 (double)size.cy / height);
    const double fit = std::min((double)size.cx / width,
                                (double)size.cy / height);
    UINT scaledWidth = width, scaledHeight = height;
    switch (position) {
        case DWPOS_FIT:
            scaledWidth = std::max<UINT>(1, (UINT)std::lround(width * fit));
            scaledHeight = std::max<UINT>(1, (UINT)std::lround(height * fit));
            break;
        case DWPOS_STRETCH:
            scaledWidth = size.cx;
            scaledHeight = size.cy;
            break;
        case DWPOS_CENTER:
        case DWPOS_TILE:
            break;
        default:
            scaledWidth =
                std::max<UINT>(size.cx, (UINT)std::ceil(width * fill));
            scaledHeight =
                std::max<UINT>(size.cy, (UINT)std::ceil(height * fill));
            break;
    }
    const WICRect crop = WallpaperCut(scaledWidth, scaledHeight, size);
    const UINT shownWidth = crop.Width;
    const UINT shownHeight = crop.Height;
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICBitmapClipper> clipper;
    ComPtr<IWICFormatConverter> converter;
    if (FAILED(wic->CreateBitmapScaler(scaler.GetAddressOf())) ||
        FAILED(scaler->Initialize(source.Get(), scaledWidth, scaledHeight,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(wic->CreateBitmapClipper(clipper.GetAddressOf())) ||
        FAILED(clipper->Initialize(scaler.Get(), &crop)) ||
        FAILED(wic->CreateFormatConverter(converter.GetAddressOf())) ||
        FAILED(converter->Initialize(clipper.Get(),
                                     GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeMedianCut))) {
        return nullptr;
    }
    const float left = (size.cx - (float)shownWidth) / 2;
    const float top = (size.cy - (float)shownHeight) / 2;
    *place = {left, top, left + shownWidth, top + shownHeight};
    ComPtr<IWICBitmapSource> result;
    converter.As(&result);
    return result;
}

// ---------------------------------------------------------------------------
// A layered window's canvas: a Direct2D DC render target on a DIB section
// ---------------------------------------------------------------------------

struct LayeredCanvas {
    ComPtr<ID2D1DCRenderTarget> target;
    HDC dc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    SIZE size{};

    bool Create(ID2D1Factory* factory) {
        const D2D1_RENDER_TARGET_PROPERTIES props =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
        if (FAILED(factory->CreateDCRenderTarget(&props,
                                                 target.GetAddressOf()))) {
            return false;
        }
        HDC screen = GetDC(nullptr);
        dc = CreateCompatibleDC(screen);
        ReleaseDC(nullptr, screen);
        return dc != nullptr;
    }

    bool Begin(SIZE wanted) {
        if (!dc || !target) {
            return false;
        }
        if (!dib || wanted.cx != size.cx || wanted.cy != size.cy) {
            if (dib) {
                SelectObject(dc, oldBitmap);
                DeleteObject(dib);
                dib = nullptr;
            }
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = wanted.cx;
            info.bmiHeader.biHeight = -wanted.cy;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;
            void* bits = nullptr;
            dib = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr,
                                   0);
            if (!dib) {
                return false;
            }
            oldBitmap = SelectObject(dc, dib);
            size = wanted;
        }
        const RECT rect{0, 0, wanted.cx, wanted.cy};
        if (FAILED(target->BindDC(dc, &rect))) {
            return false;
        }
        target->BeginDraw();
        target->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        return true;
    }

    // Ends the drawing without showing it.
    void Finish() { target->EndDraw(); }

    // Ends the drawing and shows it as the window's picture.
    void End(HWND window, POINT position, BYTE alpha) {
        target->EndDraw();
        SIZE sz = size;
        POINT source{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(window, nullptr, &position, &sz, dc, &source, 0,
                            &blend, ULW_ALPHA);
    }

    void Destroy() {
        target.Reset();
        if (dib) {
            SelectObject(dc, oldBitmap);
            DeleteObject(dib);
            dib = nullptr;
        }
        if (dc) {
            DeleteDC(dc);
            dc = nullptr;
        }
    }
};

// ---------------------------------------------------------------------------
// Looking at the screen, to know when Windows shows the new wallpaper
// ---------------------------------------------------------------------------

// What's on the monitor's screen, shrunk to a few pixels (a window excluded
// from captures isn't in it): to see when Windows shows a new wallpaper.
constexpr int kScreenSampleWidth = 48;
constexpr int kScreenSampleHeight = 27;

std::vector<DWORD> SampleScreen(const RECT& monitor) {
    std::vector<DWORD> pixels;
    HDC screen = GetDC(nullptr);
    HDC dc = screen ? CreateCompatibleDC(screen) : nullptr;
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), kScreenSampleWidth,
                      -kScreenSampleHeight, 1, 32, BI_RGB};
    void* bits = nullptr;
    HBITMAP bitmap = dc ? CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits,
                                           nullptr, 0)
                        : nullptr;
    if (bitmap) {
        HGDIOBJ old = SelectObject(dc, bitmap);
        SetStretchBltMode(dc, HALFTONE);
        if (StretchBlt(dc, 0, 0, kScreenSampleWidth, kScreenSampleHeight,
                       screen, monitor.left, monitor.top,
                       monitor.right - monitor.left,
                       monitor.bottom - monitor.top, SRCCOPY)) {
            GdiFlush();
            pixels.assign((const DWORD*)bits,
                          (const DWORD*)bits +
                              kScreenSampleWidth * kScreenSampleHeight);
        }
        SelectObject(dc, old);
        DeleteObject(bitmap);
    }
    if (dc) {
        DeleteDC(dc);
    }
    if (screen) {
        ReleaseDC(nullptr, screen);
    }
    return pixels;
}

// Whether two pixels differ by more than `threshold` in a color.
bool PixelDiffers(DWORD a, DWORD b, int threshold) {
    for (int shift : {0, 8, 16}) {
        if (std::abs((int)((a >> shift) & 0xFF) - (int)((b >> shift) & 0xFF)) >
            threshold) {
            return true;
        }
    }
    return false;
}

// How many of two samples' pixels differ by more than `threshold` in a
// color.
int SampleChanges(const std::vector<DWORD>& a,
                  const std::vector<DWORD>& b,
                  int threshold) {
    int changes = 0;
    for (size_t i = 0; i < a.size() && i < b.size(); i++) {
        changes += PixelDiffers(a[i], b[i], threshold);
    }
    return changes;
}

// Whether the screen (`now`) shows the new picture as the transition's
// window showed it (`wanted`), at the pixels where it differs from the old
// wallpaper (`places`): all of them but a few (a video or a moving window
// there). In between, Windows is still bringing it in (its own fade and
// zoom).
bool SampleShows(const std::vector<DWORD>& now,
                 const std::vector<DWORD>& wanted,
                 const std::vector<size_t>& places) {
    size_t off = 0;
    for (size_t i : places) {
        if (i >= now.size() || i >= wanted.size() ||
            PixelDiffers(now[i], wanted[i], 40)) {
            off++;
        }
    }
    return off * 10 <= places.size();
}

// ---------------------------------------------------------------------------
// The ink transition. With a subject (a cut-out PNG): a thin glowing pencil
// outline of it, a thick marker stroke along the outline (the new picture),
// then the new picture bleeds inward and outward with a ragged edge. Without
// one: ink drops pop up, swell with rough edges and merge.
//
// Every pixel gets a "reveal time" (0..kInkT) once, up front. A frame then
// only touches the pixels whose time is near "now" (they are bucketed by
// time), so even big monitors stay smooth.
// ---------------------------------------------------------------------------

constexpr int kInkT = 1023;      // reveal times go 0..kInkT
constexpr int kInkFeather = 3;   // soft edge of the reveal, in those units
// Subject mode only:
constexpr int kInkWetTrail = 48;           // how long fresh ink stays darker behind the front
constexpr float kInkSubjectDone = 0.64f;   // the subject is fully filled in by this share of the time
constexpr float kInkOutsideStart = 0.46f;  // the background starts flooding here (a beat after the subject)
constexpr int kDx8[8] = {-1, -1, 0, 1, 1, 1, 0, -1};  // clockwise from West
constexpr int kDy8[8] = {0, -1, -1, -1, 0, 1, 1, 1};

struct InkLine {
    std::vector<D2D1_POINT_2F> pts;  // the outline, smoothed
};

float InkHash(int x, int y, unsigned seed) {
    unsigned h = (unsigned)x * 374761393u + (unsigned)y * 668265263u +
                 seed * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (float)(h & 0xFFFFFF) / 16777215.0f;
}

float InkValueNoise(float x, float y, unsigned seed) {
    const int xi = (int)std::floor(x);
    const int yi = (int)std::floor(y);
    const float fx = x - xi;
    const float fy = y - yi;
    const float sx = fx * fx * (3 - 2 * fx);
    const float sy = fy * fy * (3 - 2 * fy);
    const float a = InkHash(xi, yi, seed);
    const float b = InkHash(xi + 1, yi, seed);
    const float c = InkHash(xi, yi + 1, seed);
    const float d = InkHash(xi + 1, yi + 1, seed);
    const float top = a + (b - a) * sx;
    const float bottom = c + (d - c) * sx;
    return top + (bottom - top) * sy;
}

// Folds a coordinate back and forth inside 0..limit (a noise that never runs
// out, without a seam).
float InkMirror(float v, float limit) {
    float t = std::fmod(v, 2 * limit);
    if (t < 0) {
        t += 2 * limit;
    }
    return t < limit ? t : 2 * limit - t;
}

// A smooth, many-scale noise (-1..1) kept on a coarse grid (4 px a cell).
struct InkNoise {
    static constexpr int kCell = 4;
    int gw = 0, gh = 0;
    std::vector<float> grid;

    void Build(SIZE size, unsigned seed) {
        gw = size.cx / kCell + 2;
        gh = size.cy / kCell + 2;
        grid.assign((size_t)gw * gh, 0.0f);
        float lo = 1e9f, hi = -1e9f;
        for (int gy = 0; gy < gh; gy++) {
            for (int gx = 0; gx < gw; gx++) {
                const float px = (float)(gx * kCell);
                const float py = (float)(gy * kCell);
                float sum = 0, norm = 0, amp = 0.5f, scale = 1.0f / 120.0f;
                for (int o = 0; o < 4; o++) {
                    sum += amp * InkValueNoise(px * scale + o * 17.3f,
                                               py * scale + o * 9.1f,
                                               seed + o * 101u);
                    norm += amp;
                    amp *= 0.55f;
                    scale *= 2.5f;
                }
                const float v = sum / norm;
                grid[(size_t)gy * gw + gx] = v;
                lo = std::min(lo, v);
                hi = std::max(hi, v);
            }
        }
        const float span = std::max(hi - lo, 1e-4f);
        for (float& v : grid) {
            v = (v - lo) / span * 2 - 1;
        }
    }

    float At(float x, float y) const {
        const float fx = std::clamp(x / kCell, 0.0f, (float)(gw - 1) - 0.001f);
        const float fy = std::clamp(y / kCell, 0.0f, (float)(gh - 1) - 0.001f);
        const int ix = (int)fx;
        const int iy = (int)fy;
        const float tx = fx - ix;
        const float ty = fy - iy;
        const float* r0 = &grid[(size_t)iy * gw + ix];
        const float* r1 = r0 + gw;
        const float a = r0[0] + (r0[1] - r0[0]) * tx;
        const float b = r1[0] + (r1[1] - r1[0]) * tx;
        return a + (b - a) * ty;
    }
};

// Distance to the nearest seed (d == 0), carrying that seed's value `u`
// along (two-pass chamfer).
void InkChamfer(int W, int H, std::vector<float>& d, std::vector<float>& u) {
    const float kDiag = 1.4142f;
    auto relax = [&](size_t i, int nx, int ny, float cost) {
        if (nx < 0 || nx >= W || ny < 0 || ny >= H) {
            return;
        }
        const size_t j = (size_t)ny * W + nx;
        const float c = d[j] + cost;
        if (c < d[i]) {
            d[i] = c;
            u[i] = u[j];
        }
    };
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t i = (size_t)y * W + x;
            relax(i, x - 1, y, 1);
            relax(i, x, y - 1, 1);
            relax(i, x - 1, y - 1, kDiag);
            relax(i, x + 1, y - 1, kDiag);
        }
    }
    for (int y = H - 1; y >= 0; y--) {
        for (int x = W - 1; x >= 0; x--) {
            const size_t i = (size_t)y * W + x;
            relax(i, x + 1, y, 1);
            relax(i, x, y + 1, 1);
            relax(i, x + 1, y + 1, kDiag);
            relax(i, x - 1, y + 1, kDiag);
        }
    }
}

// Subject mode: finds the outline(s) of the cut-out (`mask`: 1 = subject),
// and gives every pixel its reveal time:
//   0.05..0.36  the marker ribbon along the outline, following the pencil
//   0.36..0.97  the bleed, inward and outward, at the same speed, with a
//               ragged edge
bool InkBuildSubject(const std::vector<uint8_t>& mask,
                     SIZE size,
                     const InkNoise& noise,
                     std::vector<uint16_t>* T,
                     std::vector<InkLine>* lines) {
    const int W = size.cx, H = size.cy;
    const size_t N = (size_t)W * H;
    auto at = [&](int x, int y) { return mask[(size_t)y * W + x] != 0; };
    auto fg = [&](int x, int y) {
        return x >= 0 && x < W && y >= 0 && y < H && at(x, y);
    };

    // Outline pixels: subject pixels with a background pixel next to them (the
    // screen's edge doesn't count).
    std::vector<float> ad((size_t)N, 1e9f), uu((size_t)N, 0.0f);
    size_t seeds = 0;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (!at(x, y)) {
                continue;
            }
            const bool edge = (x > 0 && !at(x - 1, y)) ||
                              (x < W - 1 && !at(x + 1, y)) ||
                              (y > 0 && !at(x, y - 1)) ||
                              (y < H - 1 && !at(x, y + 1));
            if (edge) {
                ad[(size_t)y * W + x] = 0;
                seeds++;
            }
        }
    }
    if (!seeds) {
        return false;
    }

    // Follow each outline around (Moore-neighbour tracing), cutting it where
    // it runs along the screen's border.
    std::vector<uint8_t> visited(N, 0);
    std::vector<std::vector<POINT>> runs;
    std::vector<POINT> trace, run;
    const size_t cap = (size_t)(W + H) * 8;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t i = (size_t)y * W + x;
            if (ad[i] != 0 || visited[i]) {
                continue;
            }
            int b = -1;
            for (int dir : {0, 2, 4, 6}) {
                if (!fg(x + kDx8[dir], y + kDy8[dir])) {
                    b = dir;
                    break;
                }
            }
            if (b < 0) {
                continue;
            }
            trace.clear();
            int cx = x, cy = y;
            trace.push_back({cx, cy});
            visited[i] = 1;
            for (size_t step = 0; step < cap; step++) {
                bool moved = false;
                for (int k = 1; k <= 8; k++) {
                    const int dir = (b + k) & 7;
                    const int nx = cx + kDx8[dir];
                    const int ny = cy + kDy8[dir];
                    if (!fg(nx, ny)) {
                        continue;
                    }
                    const int pd = (dir + 7) & 7;
                    const int bx = cx + kDx8[pd];
                    const int by = cy + kDy8[pd];
                    cx = nx;
                    cy = ny;
                    for (int j = 0; j < 8; j++) {
                        if (cx + kDx8[j] == bx && cy + kDy8[j] == by) {
                            b = j;
                            break;
                        }
                    }
                    moved = true;
                    break;
                }
                if (!moved || (cx == x && cy == y)) {
                    break;
                }
                trace.push_back({cx, cy});
                visited[(size_t)cy * W + cx] = 1;
            }
            run.clear();
            for (const POINT& p : trace) {
                if (p.x == 0 || p.y == 0 || p.x == W - 1 || p.y == H - 1) {
                    if (run.size() >= 40) {
                        runs.push_back(run);
                    }
                    run.clear();
                } else {
                    run.push_back(p);
                }
            }
            if (run.size() >= 40) {
                runs.push_back(run);
            }
        }
    }
    if (runs.empty()) {
        return false;
    }
    std::sort(runs.begin(), runs.end(),
              [](const auto& a, const auto& b) { return a.size() > b.size(); });
    if (runs.size() > 16) {
        runs.resize(16);
    }

    // Each outline: where along it (0..1) every pixel of it is (for the
    // ribbon's timing), and a smoothed, thinned copy to draw as the pencil.
    for (const auto& r : runs) {
        const int n = (int)r.size();
        for (int k = 0; k < n; k++) {
            const size_t i = (size_t)r[k].y * W + r[k].x;
            if (ad[i] == 0) {
                uu[i] = (float)k / (float)(n - 1);
            }
        }
        InkLine line;
        auto smooth = [&](int k) {
            float sx = 0, sy = 0;
            int c = 0;
            for (int o = -4; o <= 4; o++) {
                const int j = k + o;
                if (j < 0 || j >= n) {
                    continue;
                }
                sx += (float)r[j].x;
                sy += (float)r[j].y;
                c++;
            }
            return D2D1::Point2F(sx / c + 0.5f, sy / c + 0.5f);
        };
        for (int k = 0; k < n; k += 3) {
            line.pts.push_back(smooth(k));
        }
        if ((n - 1) % 3 != 0) {
            line.pts.push_back(smooth(n - 1));
        }
        lines->push_back(std::move(line));
    }

    InkChamfer(W, H, ad, uu);
    float maxAd = 1;
    for (float a : ad) {
        if (a < 1e8f) {
            maxAd = std::max(maxAd, a);
        }
    }

    const float minDim = (float)std::min(W, H);
    const float halfW = std::max(
        2.0f, std::max(9.0f, 0.016f * minDim) * (g_inkRibbon / 100.0f));  // marker half width
    // How deep the subject is (its widest distance from the outline), so it
    // always fills in over the same stretch of time, big or small.
    float maxIn = 1;
    for (size_t i = 0; i < N; i++) {
        if (mask[i] && ad[i] < 1e8f) {
            maxIn = std::max(maxIn, ad[i]);
        }
    }
    const float span = std::max(1.0f, maxAd - halfW);
    const float spanIn = std::max(1.0f, maxIn - halfW);
    T->assign(N, 0);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t i = (size_t)y * W + x;
            const float a = ad[i];
            const float n = noise.At((float)x, (float)y);
            const float edge = halfW * (1.0f + 0.45f * n);  // ragged ribbon
            float tm;
            if (a <= edge) {
                tm = 0.05f + 0.27f * uu[i] + 0.04f * (a / edge);
            } else {
                const float eff = a + n * (8.0f + 0.16f * a);  // ragged bleed
                if (mask[i]) {
                    // Inside the subject: floods from the outline inward,
                    // fast at first, slowing toward the middle.
                    const float k =
                        std::clamp((eff - halfW) / spanIn, 0.0f, 1.0f);
                    const float e = 1.0f - (1.0f - k) * (1.0f - k);
                    tm = 0.34f + (kInkSubjectDone - 0.34f) * e;
                } else {
                    // Outside: after a short beat, spreads over the rest.
                    const float k =
                        std::clamp((eff - halfW) / span, 0.0f, 1.0f);
                    tm = kInkOutsideStart + (0.97f - kInkOutsideStart) * k;
                }
            }
            (*T)[i] = (uint16_t)std::clamp((int)std::lround(tm * kInkT), 0,
                                           kInkT);
        }
    }
    return true;
}

// No-subject mode: 5-8 drops pop up (a few moments apart) and swell with a
// rough edge; where they meet, the nearest one wins, so they merge.
void InkBuildSplatter(SIZE size,
                      const InkNoise& noise,
                      std::vector<uint16_t>* T) {
    const int W = size.cx, H = size.cy;
    const size_t N = (size_t)W * H;
    const float Wf = (float)W, Hf = (float)H;
    const float minDim = std::min(Wf, Hf);
    const float diag = std::hypot(Wf, Hf);
    std::mt19937 rng{std::random_device{}()};
    auto rnd = [&](float lo, float hi) {
        return lo + (hi - lo) * ((rng() & 0xFFFFFF) / 16777215.0f);
    };
    struct Drop {
        float x, y, r0, delay, speed, ox, oy;
    };
    std::vector<Drop> drops;
    const int count = 5 + (int)(rng() % 4);
    for (int i = 0; i < count; i++) {
        Drop d{};
        for (int tries = 0; tries < 30; tries++) {
            d.x = rnd(0.08f * Wf, 0.92f * Wf);
            d.y = rnd(0.10f * Hf, 0.90f * Hf);
            bool ok = true;
            for (const Drop& o : drops) {
                if (std::hypot(o.x - d.x, o.y - d.y) < 0.16f * diag) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                break;
            }
        }
        d.r0 = rnd(0.014f, 0.032f) * minDim;
        d.delay = i == 0 ? 0.0f : rnd(0.0f, 0.20f) * 0.35f * diag;
        d.speed = rnd(0.75f, 1.3f);
        d.ox = rnd(0.0f, 2 * Wf);
        d.oy = rnd(0.0f, 2 * Hf);
        drops.push_back(d);
    }
    std::vector<float> raw(N);
    float maxRaw = 1;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float best = 1e30f;
            for (const Drop& d : drops) {
                const float dx = (float)x - d.x;
                const float dy = (float)y - d.y;
                const float dist = std::sqrt(dx * dx + dy * dy);
                const float n = noise.At(InkMirror((float)x + d.ox, Wf),
                                         InkMirror((float)y + d.oy, Hf));
                const float eff = dist + n * (0.5f * d.r0 + 0.2f * dist);
                const float v =
                    d.delay + std::max(0.0f, eff - d.r0) / d.speed;
                best = std::min(best, v);
            }
            raw[(size_t)y * W + x] = best;
            maxRaw = std::max(maxRaw, best);
        }
    }
    T->assign(N, 0);
    for (size_t i = 0; i < N; i++) {
        (*T)[i] = (uint16_t)std::clamp(
            (int)std::lround(raw[i] / maxRaw * 0.97f * kInkT), 0, kInkT);
    }
}

// Darkens (and cools) an opaque pixel by w = 0..255: fresh, wet ink.
inline uint32_t InkWet(uint32_t s, unsigned w) {
    const unsigned r = (((s >> 16) & 255u) * (256u - ((w * 78u) >> 8))) >> 8;
    const unsigned g = (((s >> 8) & 255u) * (256u - ((w * 68u) >> 8))) >> 8;
    const unsigned b = ((s & 255u) * (256u - ((w * 36u) >> 8))) >> 8;
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

struct InkAnim {
    SIZE size{};
    bool subject = false;
    std::vector<uint32_t> src;     // the new picture on the monitor (opaque)
    std::vector<uint32_t> frame;   // what is shown now (premultiplied)
    std::vector<uint32_t> order;   // pixels sorted by reveal time
    std::vector<uint32_t> bstart;  // where each reveal time starts in `order`
    std::vector<InkLine> lines;
    uint8_t alphaLut[2 * kInkFeather + 1] = {};
    uint8_t rimLut[2 * kInkFeather + 1] = {};
    uint8_t wetLut[kInkWetTrail + 1] = {};
    int lastP = -kInkFeather - 1;
    ID2D1Factory* factory = nullptr;
    ComPtr<ID2D1Bitmap> reveal;
    ComPtr<ID2D1StrokeStyle> stroke;

    // The heavy part (no drawing): the picture's pixels, the subject's
    // outline and every pixel's reveal time.
    bool Prepare(IWICImagingFactory* wic,
                 IWICBitmapSource* picture,
                 const D2D1_RECT_F& pictureRect,
                 SIZE monitorSize,
                 COLORREF background,
                 DESKTOP_WALLPAPER_POSITION position,
                 const std::wstring& subjectPath) {
        size = monitorSize;
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !picture) {
            return false;
        }
        const size_t N = (size_t)W * H;
        UINT pw = 0, ph = 0;
        if (FAILED(picture->GetSize(&pw, &ph)) || !pw || !ph) {
            return false;
        }
        std::vector<uint32_t> px((size_t)pw * ph);
        if (FAILED(picture->CopyPixels(nullptr, pw * 4,
                                       (UINT)(px.size() * 4),
                                       (BYTE*)px.data()))) {
            return false;
        }
        const uint32_t bg = 0xFF000000u | ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        src.assign(N, bg);
        const int ox = (int)std::floor(pictureRect.left);
        const int oy = (int)std::floor(pictureRect.top);
        for (int y = 0; y < (int)ph; y++) {
            const int dy = y + oy;
            if (dy < 0 || dy >= H) {
                continue;
            }
            for (int x = 0; x < (int)pw; x++) {
                const int dx = x + ox;
                if (dx >= 0 && dx < W) {
                    src[(size_t)dy * W + dx] = px[(size_t)y * pw + x] | 0xFF000000u;
                }
            }
        }
        px.clear();
        px.shrink_to_fit();

        InkNoise noise;
        noise.Build(size, std::random_device{}());
        if (g_unloading) return false;
        std::vector<uint16_t> T;
        subject = false;
        if (!subjectPath.empty()) {
            D2D1_RECT_F place{};
            ComPtr<IWICBitmapSource> pic = LoadWallpaperPicture(
                wic, subjectPath.c_str(), size, position, &place);
            UINT sw = 0, sh = 0;
            if (pic && SUCCEEDED(pic->GetSize(&sw, &sh)) && sw && sh) {
                std::vector<uint32_t> sp((size_t)sw * sh);
                if (SUCCEEDED(pic->CopyPixels(nullptr, sw * 4,
                                              (UINT)(sp.size() * 4),
                                              (BYTE*)sp.data()))) {
                    std::vector<uint8_t> mask(N, 0);
                    const int sx0 = (int)std::floor(place.left);
                    const int sy0 = (int)std::floor(place.top);
                    size_t covered = 0;
                    for (int y = 0; y < (int)sh; y++) {
                        const int dy = y + sy0;
                        if (dy < 0 || dy >= H) {
                            continue;
                        }
                        for (int x = 0; x < (int)sw; x++) {
                            const int dx = x + sx0;
                            if (dx >= 0 && dx < W &&
                                (sp[(size_t)y * sw + x] >> 24) >= 128) {
                                mask[(size_t)dy * W + dx] = 1;
                                covered++;
                            }
                        }
                    }
                    // (A picture with no transparency, or almost nothing in
                    // it, has no subject to outline.)
                    if (covered > N / 500 && covered < N * 97 / 100) {
                        subject = InkBuildSubject(mask, size, noise, &T, &lines);
                    }
                }
            }
        }
        if (!subject) {
            if (g_unloading) return false;
            lines.clear();
            InkBuildSplatter(size, noise, &T);
        }

        // Pixels sorted by reveal time.
        bstart.assign(kInkT + 2, 0);
        for (size_t i = 0; i < N; i++) {
            bstart[T[i] + 1]++;
        }
        for (int v = 0; v <= kInkT; v++) {
            bstart[v + 1] += bstart[v];
        }
        order.resize(N);
        std::vector<uint32_t> next(bstart.begin(), bstart.begin() + kInkT + 1);
        for (size_t i = 0; i < N; i++) {
            order[next[T[i]]++] = (uint32_t)i;
        }
        // The soft edge, and a faint bright rim on it (wet ink).
        for (int d = -kInkFeather; d <= kInkFeather; d++) {
            const float s = (d + kInkFeather) / (2.0f * kInkFeather);
            const float e = s * s * (3 - 2 * s);
            alphaLut[d + kInkFeather] = (uint8_t)std::lround(e * 255);
            rimLut[d + kInkFeather] = (uint8_t)std::lround(4 * e * (1 - e) * 46);
        }
        for (int d = 0; d <= kInkWetTrail; d++) {
            const float q = 1.0f - (float)d / (float)kInkWetTrail;
            wetLut[d] = (uint8_t)std::lround(255.0f * q * q);
        }
        frame.assign(N, 0);
        lastP = -kInkFeather - 1;
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target, ID2D1Factory* f) {
        factory = f;
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        if (FAILED(target->CreateBitmap(
                D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
                D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                      D2D1_ALPHA_MODE_PREMULTIPLIED),
                    96.0f, 96.0f),
                reveal.GetAddressOf()))) {
            return false;
        }
        factory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND,
                                        D2D1_CAP_STYLE_ROUND,
                                        D2D1_CAP_STYLE_ROUND,
                                        D2D1_LINE_JOIN_ROUND),
            nullptr, 0, stroke.GetAddressOf());
        return true;
    }

    void Release() {
        reveal.Reset();
        stroke.Reset();
        factory = nullptr;
    }

    // The pencil: each outline is drawn from its start to its end (fast), as
    // a thin white line with a soft icy glow and a bright pen tip; it fades
    // away once the ink is bleeding.
    void DrawSketch(ID2D1DCRenderTarget* target, float t) {
        const float x = std::clamp(t / 0.20f, 0.0f, 1.0f);
        const float f = 1 - (1 - x) * (1 - x);
        const float fade =
            t < 0.38f ? 1.0f : std::max(0.0f, 1 - (t - 0.38f) / 0.16f);
        if (f <= 0 || fade <= 0 || !factory) {
            return;
        }
        ComPtr<ID2D1SolidColorBrush> brush;
        target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                      brush.GetAddressOf());
        if (!brush) {
            return;
        }
        const float sc = std::max(1.0f, size.cy / 1080.0f);
        const float lineK = g_inkLine / 100.0f;
        const float glowK = g_inkGlow / 100.0f;
        // A flash of glow when the marker closes the outline.
        const float pz = (t - 0.34f) / 0.05f;
        const float pulse = std::exp(-pz * pz);
        const float widths[3] = {11 * sc * lineK * (1.0f + 0.6f * pulse),
                                 5.5f * sc * lineK * (1.0f + 0.4f * pulse),
                                 std::max(0.8f, 1.8f * sc * lineK)};
        const float alphas[3] = {
            std::min(1.0f, 0.10f * glowK * (1.0f + 2.0f * pulse)),
            std::min(1.0f, 0.22f * glowK * (1.0f + 1.6f * pulse)),
            std::min(1.0f, 0.95f * (0.5f + 0.5f * glowK))};
        for (const InkLine& line : lines) {
            const int n = (int)line.pts.size();
            const int count = std::clamp((int)(f * (n - 1)) + 1, 2, n);
            ComPtr<ID2D1PathGeometry> geometry;
            ComPtr<ID2D1GeometrySink> sink;
            if (FAILED(factory->CreatePathGeometry(geometry.GetAddressOf())) ||
                FAILED(geometry->Open(sink.GetAddressOf()))) {
                continue;
            }
            sink->BeginFigure(line.pts[0], D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddLines(line.pts.data() + 1, (UINT32)(count - 1));
            sink->EndFigure(D2D1_FIGURE_END_OPEN);
            sink->Close();
            for (int k = 0; k < 3; k++) {
                brush->SetColor(k < 2 ? D2D1::ColorF(0.62f, 0.86f, 1.0f,
                                                     alphas[k] * fade)
                                      : D2D1::ColorF(1, 1, 1, alphas[k] * fade));
                target->DrawGeometry(geometry.Get(), brush.Get(), widths[k],
                                     stroke.Get());
            }
            if (f < 1) {
                const D2D1_POINT_2F head = line.pts[count - 1];
                brush->SetColor(D2D1::ColorF(0.62f, 0.86f, 1.0f,
                                             std::min(1.0f, 0.35f * glowK) * fade));
                target->FillEllipse(
                    D2D1::Ellipse(head, 8 * sc * lineK, 8 * sc * lineK),
                    brush.Get());
                brush->SetColor(D2D1::ColorF(1, 1, 1, alphas[2] * fade));
                target->FillEllipse(
                    D2D1::Ellipse(head, std::max(1.2f, 3 * sc * lineK),
                                  std::max(1.2f, 3 * sc * lineK)),
                    brush.Get());
            }
        }
    }

    // t: 0..1 through the transition. Draws into the (already begun) target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal) {
            return;
        }
        const float t = (float)std::clamp(time, 0.0, 1.0);
        // (The drops slow down as they merge; the subject's phases are linear.)
        const float prog = subject ? t : 1 - std::pow(1 - t, 1.15f);
        const int P = (int)std::lround(prog * (kInkT + 2 * kInkFeather)) -
                      kInkFeather;
        // Subject mode: pixels just behind the front are redrawn each frame
        // a bit darker (wet ink) and settle to their real colours. The wet
        // look is gone by the end.
        const int trail = subject ? kInkWetTrail : 0;
        const float wetFade =
            1.0f - std::clamp((t - 0.90f) / 0.10f, 0.0f, 1.0f);
        const int lo = std::max(0, lastP - kInkFeather - trail);
        const int hi = std::min(kInkT, P + kInkFeather);
        for (int tv = lo; tv <= hi; tv++) {
            const int diff = std::clamp(P - tv, -kInkFeather, kInkFeather);
            const unsigned a = alphaLut[diff + kInkFeather];
            const unsigned rim = rimLut[diff + kInkFeather];
            unsigned wet = 0;
            if (trail > 0) {
                const int behind = std::clamp(P - tv, 0, kInkWetTrail);
                wet = (unsigned)(wetLut[behind] * wetFade);
            }
            for (uint32_t k = bstart[tv]; k < bstart[tv + 1]; k++) {
                const uint32_t idx = order[k];
                uint32_t s = src[idx];
                if (wet) {
                    s = InkWet(s, wet);
                }
                if (a == 255 && rim == 0) {
                    frame[idx] = s;
                    continue;
                }
                auto mix = [&](unsigned c) {
                    return std::min(255u, (c * a + 127) / 255 + rim);
                };
                frame[idx] = (std::min(255u, a + rim) << 24) |
                             (mix((s >> 16) & 255) << 16) |
                             (mix((s >> 8) & 255) << 8) | mix(s & 255);
            }
        }
        lastP = P;
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
        if (subject) {
            DrawSketch(target, t);
        }
    }
};

// ---------------------------------------------------------------------------
// The slash transition: the wallpaper is cut into tilted strips. In a quick
// wave each strip comes forward, turns 180 degrees about its own axis (old
// picture on the front, new one on the back, swapped when it is edge-on),
// then drops flat. Perspective is done on the CPU: every screen pixel is
// mapped back to a point on a strip (closed form), so the turn is exact.
// ---------------------------------------------------------------------------

constexpr int kSlashStrips = 6;
constexpr float kSlashTiltDeg = 20.0f;    // lean of the cuts from vertical
constexpr double kSlashStagger = 0.085;   // start delay between neighbours
constexpr float kSlashLift = 0.10f;       // how far a strip comes forward
constexpr float kSlashGap = 0.16f;        // how much a lifted strip narrows
constexpr float kSlashShade = 0.50f;      // darkness when edge-on
constexpr uint32_t kSlashVoid = 0xFF07080Au;  // the near-black behind

// Blend of two opaque/premultiplied pixels, t = 0..256 (two channels a time).
inline uint32_t SlashLerp(uint32_t a, uint32_t b, uint32_t t) {
    const uint32_t rb = (((a & 0x00FF00FFu) * (256 - t) +
                          (b & 0x00FF00FFu) * t) >> 8) & 0x00FF00FFu;
    const uint32_t ag = (((a >> 8) & 0x00FF00FFu) * (256 - t) +
                         ((b >> 8) & 0x00FF00FFu) * t) & 0xFF00FF00u;
    return rb | ag;
}

// Darkens an opaque pixel, sh = 0..256.
inline uint32_t SlashShadeColor(uint32_t c, uint32_t sh) {
    const uint32_t rb = (((c & 0x00FF00FFu) * sh) >> 8) & 0x00FF00FFu;
    const uint32_t g = ((((c >> 8) & 0xFFu) * sh) >> 8) & 0xFFu;
    return 0xFF000000u | rb | (g << 8);
}

// Bilinear sample at (x, y), in pixel-centre coordinates.
inline uint32_t SlashSample(const uint32_t* img, int W, int H, float x,
                            float y) {
    x -= 0.5f;
    y -= 0.5f;
    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);
    const uint32_t fx = (uint32_t)((x - (float)x0) * 256.0f);
    const uint32_t fy = (uint32_t)((y - (float)y0) * 256.0f);
    const int x1 = std::clamp(x0 + 1, 0, W - 1);
    const int y1 = std::clamp(y0 + 1, 0, H - 1);
    x0 = std::clamp(x0, 0, W - 1);
    y0 = std::clamp(y0, 0, H - 1);
    const uint32_t* r0 = img + (size_t)y0 * W;
    const uint32_t* r1 = img + (size_t)y1 * W;
    return SlashLerp(SlashLerp(r0[x0], r0[x1], fx),
                     SlashLerp(r1[x0], r1[x1], fx), fy);
}

struct SlashAnim {
    SIZE size{};
    std::vector<uint32_t> oldPx, newPx, frame;  // opaque, monitor-sized
    float F = 1;        // camera distance
    float half = 0;     // half the screen's extent across the strips
    float stripW = 0;
    float cosP = 1, sinP = 0;  // the strips' across-direction n = (cosP, sinP)
    ComPtr<ID2D1Bitmap> reveal;

    // A picture placed on the monitor (what Windows would show), opaque.
    static bool Compose(IWICBitmapSource* picture,
                        const D2D1_RECT_F& rect,
                        SIZE size,
                        uint32_t bg,
                        std::vector<uint32_t>* out) {
        const int W = size.cx, H = size.cy;
        out->assign((size_t)W * H, bg);
        if (!picture) {
            return true;
        }
        UINT pw = 0, ph = 0;
        if (FAILED(picture->GetSize(&pw, &ph)) || !pw || !ph) {
            return false;
        }
        std::vector<uint32_t> px((size_t)pw * ph);
        if (FAILED(picture->CopyPixels(nullptr, pw * 4, (UINT)(px.size() * 4),
                                       (BYTE*)px.data()))) {
            return false;
        }
        const int ox = (int)std::floor(rect.left);
        const int oy = (int)std::floor(rect.top);
        for (int y = 0; y < (int)ph; y++) {
            const int dy = y + oy;
            if (dy < 0 || dy >= H) {
                continue;
            }
            for (int x = 0; x < (int)pw; x++) {
                const int dx = x + ox;
                if (dx >= 0 && dx < W) {
                    (*out)[(size_t)dy * W + dx] =
                        px[(size_t)y * pw + x] | 0xFF000000u;
                }
            }
        }
        return true;
    }

    bool Prepare(IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background) {
        size = monitorSize;
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        frame.assign(newPx.size(), kSlashVoid);
        const float phi = kSlashTiltDeg * 3.14159265f / 180.0f;
        cosP = std::cos(phi);
        sinP = std::sin(phi);
        half = 0.5f * ((float)W * cosP + (float)H * sinP);
        stripW = 2 * half / kSlashStrips;
        F = 1.7f * (float)std::min(W, H);
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
    }

    // A strip lying flat (before it starts: the old picture; after: the new).
    void DrawFlat(int k, bool useNew) {
        const int W = size.cx, H = size.cy;
        const float Cx = W * 0.5f, Cy = H * 0.5f;
        const float a = -half + (k + 0.5f) * stripW;
        const float hw = stripW * 0.5f + 1.5f;  // (a little overlap: no seams)
        const uint32_t* src = (useNew ? newPx : oldPx).data();
        for (int y = 0; y < H; y++) {
            const float base = (y + 0.5f - Cy) * sinP;
            const int x0 = std::max(
                0, (int)std::floor((a - hw - 1 - base) / cosP + Cx - 0.5f));
            const int x1 = std::min(
                W - 1, (int)std::ceil((a + hw + 1 - base) / cosP + Cx - 0.5f));
            uint32_t* row = frame.data() + (size_t)y * W;
            const uint32_t* srow = src + (size_t)y * W;
            for (int x = x0; x <= x1; x++) {
                const float q = (x + 0.5f - Cx) * cosP + base;
                const float cov = 0.5f + hw - std::fabs(q - a);
                if (cov <= 0) {
                    continue;
                }
                row[x] = cov >= 1.0f
                             ? srow[x]
                             : SlashLerp(row[x], srow[x],
                                         (uint32_t)(cov * 256.0f));
            }
        }
    }

    // A strip in the air: lifted by liftN (0..1), turned by theta (0..pi).
    // Coordinates are measured from the screen's centre C: n = (cosP, sinP)
    // across the strips, d = (-sinP, cosP) along them. The strip's axis is at
    // n-offset `a`; a point of it at distance u from the axis, after the turn
    // and the perspective, lands at
    //   q = F (a + u cos) / (F - L - u sin)
    // which is solved for u from the pixel's q.
    void DrawLifted(int k, float liftN, float theta) {
        const int W = size.cx, H = size.cy;
        const float Cx = W * 0.5f, Cy = H * 0.5f;
        const float a = -half + (k + 0.5f) * stripW;
        const float hwBase = stripW * 0.5f;
        const float hw = hwBase * (1.0f - kSlashGap * liftN);
        const float L = liftN * kSlashLift * F;
        const float c = std::cos(theta), sn = std::sin(theta);
        // Which face looks at the camera (and how fast the picture moves).
        const float K = c * (F - L) + sn * a;
        const bool back = K < 0;
        const float absK = std::max(std::fabs(K), 1e-3f);
        const float shadeBase = 1.0f - kSlashShade * sn;
        auto qAt = [&](float u) {
            return F * (a + u * c) / (F - L - u * sn);
        };
        const float q1 = qAt(-hw), q2 = qAt(hw);
        const float qmin = std::min(q1, q2) - 2;
        const float qmax = std::max(q1, q2) + 2;
        const uint32_t* src = (back ? newPx : oldPx).data();
        const float nx = cosP, ny = sinP, dx = -sinP, dy = cosP;
        for (int y = 0; y < H; y++) {
            const float ys = y + 0.5f - Cy;
            const float base = ys * sinP;
            const int x0 = std::max(
                0, (int)std::floor((qmin - base) / cosP + Cx - 0.5f));
            const int x1 = std::min(
                W - 1, (int)std::ceil((qmax - base) / cosP + Cx - 0.5f));
            uint32_t* row = frame.data() + (size_t)y * W;
            for (int x = x0; x <= x1; x++) {
                const float xs = x + 0.5f - Cx;
                const float q = xs * cosP + base;
                const float qd = -xs * sinP + ys * cosP;
                const float den = F * c + q * sn;
                if (std::fabs(den) < 1e-3f) {
                    continue;
                }
                const float u = (q * (F - L) - F * a) / den;
                const float au = std::fabs(u);
                const float zd = F - L - u * sn;  // distance to the camera
                if (au > hw + 3 || zd < 1.0f) {
                    continue;
                }
                // Soft edge, in screen pixels.
                const float cov = 0.5f + (hw - au) * (F * absK / (zd * zd));
                if (cov <= 0) {
                    continue;
                }
                // Back to the flat strip: along it, and across (the back is
                // seen mirrored, so it reads the other way: not mirrored).
                const float tAlong = qd * (zd / F);
                const float off = back ? a - u : a + u;
                const float sx = Cx + tAlong * dx + off * nx;
                const float sy = Cy + tAlong * dy + off * ny;
                if (sx < 0 || sx >= W || sy < 0 || sy >= H) {
                    continue;
                }
                uint32_t col = SlashSample(src, W, H, sx, sy);
                const float sh = std::clamp(
                    shadeBase + 0.14f * (u / hwBase) * sn, 0.15f, 1.0f);
                const uint32_t sh256 = (uint32_t)(sh * 256.0f);
                if (sh256 < 256) {
                    col = SlashShadeColor(col, sh256);
                }
                row[x] = cov >= 1.0f
                             ? col
                             : SlashLerp(row[x], col, (uint32_t)(cov * 256.0f));
            }
        }
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty()) {
            return;
        }
        auto clamp01 = [](double v) { return std::clamp(v, 0.0, 1.0); };
        auto easeOut = [](double x) {
            const double y = 1 - x;
            return 1 - y * y * y;
        };
        auto easeInOut = [](double x) {
            return x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
        };
        const double t = clamp01(time);
        struct Pose {
            int k;
            float lift, theta;
            bool idle, newFace;
        };
        Pose poses[kSlashStrips];
        const double span = 1.0 - (kSlashStrips - 1) * kSlashStagger;
        for (int k = 0; k < kSlashStrips; k++) {
            // Each strip's own 0..1: lift, then the turn, then the drop.
            const double tau = clamp01((t - k * kSlashStagger) / span);
            const double up = easeOut(clamp01(tau / 0.25));
            const double flip = easeInOut(clamp01((tau - 0.14) / 0.58));
            const double drop = easeOut(clamp01((tau - 0.72) / 0.28));
            poses[k] = {k, (float)(up * (1 - drop)),
                        (float)(3.14159265 * flip), tau <= 0 || tau >= 1,
                        tau >= 1};
        }
        // The ones nearest the camera last.
        std::stable_sort(poses, poses + kSlashStrips,
                         [](const Pose& a, const Pose& b) {
                             return a.lift < b.lift;
                         });
        std::fill(frame.begin(), frame.end(), kSlashVoid);
        for (const Pose& p : poses) {
            if (p.idle) {
                DrawFlat(p.k, p.newFace);
            } else {
                DrawLifted(p.k, p.lift, p.theta);
            }
        }
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
};

// ---------------------------------------------------------------------------
// The whip transition: the wallpaper (a plane) pulls back, tilts and slices
// into 10 vertical strips with gaps and a stepped up / down stagger; then the
// whole plane whip-spins 540 degrees about its own vertical axis in strong
// perspective (old picture on the front faces, new one on the back faces),
// and finally straightens, closes the gaps and locks into the new picture.
//
// Every screen pixel is mapped back onto a strip by a ray / plane
// intersection (closed form, one division), so the perspective is exact.
// Rows are split between a few worker threads.
// ---------------------------------------------------------------------------

constexpr int kWhipStrips = 10;
constexpr float kWhipTiltDeg = -9.0f;  // roll of the pulled-back plane
constexpr float kWhipPull = 0.22f;     // pull back (share of camera distance)
constexpr float kWhipGap = 0.10f;      // spread between strips (share of width)
constexpr float kWhipStep = 0.055f;    // up / down stagger (share of height)
constexpr float kWhipShade = 0.45f;    // darkness when a strip is edge-on
constexpr double kWhipSpin = 3.14159265358979 * 2;  // 360 degrees
constexpr double kWhipSpinFrom = 0.17;    // the spin's window (share of time)
constexpr double kWhipSpinTo = 0.80;
constexpr double kWhipSpinStagger = 0.0028;  // delay between neighbours
constexpr uint32_t kWhipDark = 0xFF0A0B0Eu;
constexpr uint32_t kWhipLight = 0xFFE6DDCCu;

struct WhipStripState {
    float c = 1, sn = 0;     // cos / sin of the strip's spin
    float xc = 0;            // centre of the strip in the flat picture
    float dk = 0;            // its up / down step
    float shade = 1;         // 1 = facing the camera
    float zc = 0;            // depth of its centre (painter's order)
    bool frontNew = false;   // its front face already shows the new picture
    float cx[4] = {}, cy[4] = {};  // corners on the screen (from the centre)
    float ymin = 0, ymax = 0;
    bool wide = false;       // a corner is too close to the camera
};

struct WhipFrame {
    WhipStripState st[kWhipStrips];
    int order[kWhipStrips] = {};
    float F = 1, M0 = 1, cosR = 1, sinR = 0, g = 0, ov = 0;
};

struct WhipAnim;
DWORD WINAPI WhipWorker(LPVOID parameter);

struct WhipJob {
    WhipAnim* self;
    int y0, y1;
    const WhipFrame* frame;
};

struct WhipAnim {
    SIZE size{};
    std::vector<uint32_t> oldPx, newPx, frame;
    uint32_t voidColor = kWhipDark;
    float F = 1;
    ComPtr<ID2D1Bitmap> reveal;

    bool Prepare(IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background) {
        size = monitorSize;
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!SlashAnim::Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        SlashAnim::Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        voidColor = g_whipLight ? kWhipLight : kWhipDark;
        frame.assign(newPx.size(), voidColor);
        F = 0.9f * (float)W;  // camera distance: strong perspective
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
    }

    // Draws the rows y0..y1 (the backdrop, then the strips far to near).
    // A strip at spin phi is the plane point (u cos, v, -u sin), turned about
    // the screen's z by the roll and pushed back by `pb`. A ray through a
    // pixel hits it at parameter m (closed form), which gives the point's
    // place u, v on the strip.
    void RenderRows(int y0, int y1, const WhipFrame& f) {
        const int W = size.cx, H = size.cy;
        const float Cx = W * 0.5f, Cy = H * 0.5f;
        const float w = (float)W / kWhipStrips;
        const float hw = w * 0.5f + f.ov;
        const float hh = H * 0.5f + f.ov;
        for (int y = y0; y < y1; y++) {
            std::fill(frame.begin() + (size_t)y * W,
                      frame.begin() + (size_t)(y + 1) * W, voidColor);
        }
        for (int oi = 0; oi < kWhipStrips; oi++) {
            const WhipStripState& s = f.st[f.order[oi]];
            const bool back = s.c <= 0;
            const uint32_t* src = (back || s.frontNew ? newPx : oldPx).data();
            const float cAbs = std::max(std::fabs(s.c), 0.05f);
            for (int y = y0; y < y1; y++) {
                const float sy = y + 0.5f - Cy;
                int xa = 0, xb = W - 1;
                if (!s.wide) {
                    if (sy < s.ymin - 2 || sy > s.ymax + 2) {
                        continue;
                    }
                    const float se =
                        std::clamp(sy, s.ymin, s.ymax - 0.001f);
                    float lo = 1e9f, hi = -1e9f;
                    for (int e = 0; e < 4; e++) {
                        const float ax = s.cx[e], ay = s.cy[e];
                        const float bx = s.cx[(e + 1) & 3];
                        const float by = s.cy[(e + 1) & 3];
                        if ((ay <= se && by > se) || (by <= se && ay > se)) {
                            const float xx = ax + (bx - ax) * (se - ay) /
                                                     (by - ay);
                            lo = std::min(lo, xx);
                            hi = std::max(hi, xx);
                        }
                    }
                    if (lo > hi) {
                        continue;
                    }
                    xa = std::max(0, (int)std::floor(lo + Cx - 0.5f) - 2);
                    xb = std::min(W - 1, (int)std::ceil(hi + Cx - 0.5f) + 2);
                }
                uint32_t* row = frame.data() + (size_t)y * W;
                for (int x = xa; x <= xb; x++) {
                    const float sx = x + 0.5f - Cx;
                    const float ax = sx * f.cosR + sy * f.sinR;
                    const float ay = -sx * f.sinR + sy * f.cosR;
                    const float den = f.F * s.c - ax * s.sn;
                    if (std::fabs(den) < 1e-3f) {
                        continue;
                    }
                    const float m = f.M0 * s.c / den;
                    if (m < 0.1f) {
                        continue;  // behind the camera, or edge-on
                    }
                    const float Qx = m * ax;
                    const float Qy = m * ay;
                    const float Qz = f.M0 - m * f.F;
                    const float u = Qx * s.c - Qz * s.sn;
                    const float lx = u - s.xc * f.g;  // along the picture
                    const float ly = Qy - s.dk;       // down the picture
                    const float ex = std::fabs(lx - s.xc);
                    const float ey = std::fabs(ly);
                    // Soft edges, in screen pixels (a turned strip is
                    // narrower on the screen by its cos).
                    const float covX = 0.5f + (hw - ex) * cAbs / m;
                    const float covY = 0.5f + (hh - ey) / m;
                    const float cov = std::min(covX, covY);
                    if (cov <= 0) {
                        continue;
                    }
                    // The back is seen from behind: it reads mirrored the
                    // other way, so the new picture is not mirrored.
                    const float tx = std::clamp(back ? Cx - lx : Cx + lx,
                                                0.0f, W - 0.001f);
                    const float ty = std::clamp(Cy + ly, 0.0f, H - 0.001f);
                    uint32_t col = SlashSample(src, W, H, tx, ty);
                    const float depthDim =
                        std::clamp(1.0f - 0.18f * (m - 1.0f), 0.7f, 1.0f);
                    const uint32_t sh256 =
                        (uint32_t)(std::clamp(s.shade * depthDim, 0.0f, 1.0f) *
                                   256.0f);
                    if (sh256 < 256) {
                        col = SlashShadeColor(col, sh256);
                    }
                    row[x] = cov >= 1.0f
                                 ? col
                                 : SlashLerp(row[x], col,
                                             (uint32_t)(cov * 256.0f));
                }
            }
        }
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty()) {
            return;
        }
        const int W = size.cx, H = size.cy;
        auto clamp01 = [](double v) { return std::clamp(v, 0.0, 1.0); };
        auto easeOut = [](double x) {
            const double y = 1 - x;
            return 1 - y * y * y;
        };
        auto easeInOut = [](double x) {
            return x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
        };
        const double t = clamp01(time);
        if (t <= 0.0) {
            frame = oldPx;
        } else if (t >= 1.0) {
            frame = newPx;
        } else {
            WhipFrame fr;
            const float pi = 3.14159265f;
            // How far "open" the plane is: pulled back, tilted, spread out,
            // stepped. It opens first, and closes with the landing.
            const double open = easeInOut(clamp01(t / 0.22));
            const double land = easeOut(clamp01((t - 0.70) / 0.30));
            const float o = (float)(open * (1.0 - land));
            const float pb = o * kWhipPull * F;
            const float rho = o * kWhipTiltDeg * pi / 180.0f;
            fr.F = F;
            fr.M0 = F + pb;
            fr.cosR = std::cos(rho);
            fr.sinR = std::sin(rho);
            fr.g = o * kWhipGap;
            fr.ov = 1.5f * (1.0f - o);  // (a little overlap when closed)
            const float amp = o * kWhipStep * (float)H;
            const float w = (float)W / kWhipStrips;
            static const float pat[kWhipStrips] = {0.6f,  -0.8f, 0.3f, -0.5f,
                                                   0.9f,  -0.4f, 0.7f, -0.9f,
                                                   0.2f,  -0.6f};
            const int cxs[4] = {-1, 1, 1, -1};
            const int cys[4] = {-1, -1, 1, 1};
            for (int k = 0; k < kWhipStrips; k++) {
                WhipStripState& s = fr.st[k];
                // Each strip spins a little after its left neighbour.
                const double s0 = kWhipSpinFrom + k * kWhipSpinStagger;
                const double s1 =
                    kWhipSpinTo - (kWhipStrips - 1 - k) * kWhipSpinStagger;
                const int halfTurns = g_whipSpin;
                const double spin = 3.14159265358979 * halfTurns;
                const double phi =
                    spin * easeInOut(clamp01((t - s0) / (s1 - s0)));
                s.c = (float)std::cos(phi);
                s.sn = (float)std::sin(phi);
                // After the half turn the front shows the new picture, so
                // the turn ends on it.
                s.frontNew = phi >= 3.14159265358979;
                s.xc = -W * 0.5f + (k + 0.5f) * w;
                s.dk = amp * pat[k];
                s.shade = 1.0f - kWhipShade * std::fabs(s.sn);
                s.zc = -s.xc * (1.0f + fr.g) * s.sn - pb;
                s.ymin = 1e9f;
                s.ymax = -1e9f;
                for (int e = 0; e < 4; e++) {
                    const float lx = s.xc + cxs[e] * w * 0.5f;
                    const float ly = cys[e] * H * 0.5f;
                    const float u = lx + s.xc * fr.g;
                    const float v = ly + s.dk;
                    const float x1 = u * s.c;
                    const float z1 = -u * s.sn - pb;
                    const float X = x1 * fr.cosR - v * fr.sinR;
                    const float Y = x1 * fr.sinR + v * fr.cosR;
                    const float zd = F - z1;
                    if (zd < 0.1f * F) {
                        s.wide = true;
                        continue;
                    }
                    s.cx[e] = X * F / zd;
                    s.cy[e] = Y * F / zd;
                    s.ymin = std::min(s.ymin, s.cy[e]);
                    s.ymax = std::max(s.ymax, s.cy[e]);
                }
                fr.order[k] = k;
            }
            // Far strips first.
            std::sort(fr.order, fr.order + kWhipStrips, [&](int a, int b) {
                return fr.st[a].zc < fr.st[b].zc;
            });
            // Rows are split between a few threads (they don't overlap).
            int n = (int)std::min<DWORD>(
                4, std::max<DWORD>(1,
                                   GetActiveProcessorCount(ALL_PROCESSOR_GROUPS)));
            if (H < 240) {
                n = 1;
            }
            const int band = (H + n - 1) / n;
            WhipJob jobs[4];
            HANDLE threads[4];
            int started = 0;
            for (int i = 1; i < n; i++) {
                const int y0 = i * band;
                const int y1 = std::min(H, (i + 1) * band);
                if (y0 >= y1) {
                    break;
                }
                jobs[i] = {this, y0, y1, &fr};
                HANDLE th =
                    CreateThread(nullptr, 0, WhipWorker, &jobs[i], 0, nullptr);
                if (th) {
                    threads[started++] = th;
                } else {
                    RenderRows(y0, y1, fr);
                }
            }
            RenderRows(0, std::min(H, band), fr);
            if (started) {
                WaitForMultipleObjects((DWORD)started, threads, TRUE,
                                       INFINITE);
                for (int i = 0; i < started; i++) {
                    CloseHandle(threads[i]);
                }
            }
        }
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
};

DWORD WINAPI WhipWorker(LPVOID parameter) {
    auto* job = (WhipJob*)parameter;
    job->self->RenderRows(job->y0, job->y1, *job->frame);
    return 0;
}

// ---------------------------------------------------------------------------
// The glitch transition: a band sweeps left to right. Behind it the new
// wallpaper is clean, ahead of it the old one is. Inside the band the picture
// is torn into random horizontal strips that shift sideways; pixels are
// stretched into streaks (pixel-sort look), the colour channels are split and
// the old / new pictures are chopped together in blocks. The pattern re-rolls
// about 36 times during the transition, so it flickers.
// ---------------------------------------------------------------------------

struct GlitchAnim {
    SIZE size{};
    std::vector<uint32_t> oldPx, newPx, frame;  // opaque, monitor-sized
    std::vector<int> rowStrip;                  // row -> strip
    std::vector<float> stripJit;                // per strip, -1..1
    int stripCount = 0;
    unsigned seed = 0;
    ComPtr<ID2D1Bitmap> reveal;

    bool Prepare(IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background) {
        size = monitorSize;
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!SlashAnim::Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        SlashAnim::Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        frame.assign(newPx.size(), bg);

        // Random horizontal strips (a few rows to a few dozen rows tall).
        std::mt19937 rng{std::random_device{}()};
        seed = rng();
        rowStrip.assign((size_t)H, 0);
        stripJit.clear();
        const int maxH = std::max(6, H / 24);
        int y = 0;
        while (y < H) {
            const int h = 2 + (int)(rng() % (unsigned)maxH);
            const int idx = (int)stripJit.size();
            stripJit.push_back(((rng() & 0xFFFF) / 65535.0f) * 2.0f - 1.0f);
            for (int k = 0; k < h && y < H; k++, y++) {
                rowStrip[(size_t)y] = idx;
            }
        }
        stripCount = (int)stripJit.size();
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
        rowStrip.clear();
        rowStrip.shrink_to_fit();
        stripJit.clear();
        stripJit.shrink_to_fit();
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty()) {
            return;
        }
        const int W = size.cx, H = size.cy;
        const double t = std::clamp(time, 0.0, 1.0);
        if (t <= 0.0) {
            frame = oldPx;
        } else if (t >= 1.0) {
            frame = newPx;
        } else {
            const float e = (float)(0.5 - 0.5 * std::cos(3.14159265358979 * t));
            const float bandW = std::max(120.0f, W * 0.28f);
            const float sc = std::max(1.0f, H / 1080.0f);
            // The front runs from off the left edge to off the right edge
            // (extra room for the per-strip jitter, so it starts and ends
            // clean).
            const float X = -1.6f * bandW + e * ((float)W + 3.2f * bandW);
            const int tq = (int)(t * 36.0);

            // This moment's parameters of every strip.
            std::vector<float> ox(stripCount), split(stripCount);
            std::vector<int> len(stripCount);
            std::vector<uint8_t> streak(stripCount);
            for (int i = 0; i < stripCount; i++) {
                ox[i] = (InkHash(i, tq, seed) - 0.5f) * 2.0f;
                len[i] = std::max(
                    4, (int)((14.0f + InkHash(i, tq, seed + 7) * 90.0f) * sc));
                streak[i] = InkHash(i, tq, seed + 13) > 0.45f ? 1 : 0;
                split[i] = (4.0f + InkHash(i, tq, seed + 23) * 14.0f) * sc;
            }
            const int selW = std::max(8, (int)(24.0f * sc));

            for (int y = 0; y < H; y++) {
                const int si = rowStrip[(size_t)y];
                const float Xs = X + stripJit[si] * bandW * 0.6f;
                const int xl = std::clamp((int)std::ceil(Xs - bandW), 0, W);
                const int xr = std::clamp((int)std::ceil(Xs), xl, W);
                uint32_t* row = frame.data() + (size_t)y * W;
                const uint32_t* oRow = oldPx.data() + (size_t)y * W;
                const uint32_t* nRow = newPx.data() + (size_t)y * W;
                // Behind the band: clean new. Ahead of it: clean old.
                if (xl > 0) {
                    memcpy(row, nRow, (size_t)xl * 4);
                }
                if (xr < W) {
                    memcpy(row + xr, oRow + xr, (size_t)(W - xr) * 4);
                }
                // In the band: the glitch.
                const bool doStreak = streak[si] != 0;
                const int L = len[si];
                const int sp = (int)split[si];
                for (int x = xl; x < xr; x++) {
                    const float u = std::clamp((Xs - x) / bandW, 0.0f, 1.0f);
                    const float env = std::sin(3.14159265f * u);
                    const bool useNew =
                        InkHash(x / selW, si, seed + 31 + (unsigned)tq) < u;
                    const uint32_t* s = useNew ? nRow : oRow;
                    int sx = doStreak ? (x / L) * L : x;
                    sx += (int)(ox[si] * bandW * 0.35f * env);
                    const int sr = std::clamp(sx - sp, 0, W - 1);
                    const int sg = std::clamp(sx, 0, W - 1);
                    const int sb = std::clamp(sx + sp, 0, W - 1);
                    uint32_t col = 0xFF000000u | (s[sr] & 0x00FF0000u) |
                                   (s[sg] & 0x0000FF00u) | (s[sb] & 0x000000FFu);
                    if (y & 1) {
                        col = SlashShadeColor(col, 232);  // faint scanlines
                    }
                    row[x] = col;
                }
            }
        }
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
};

// ---------------------------------------------------------------------------
// The hyperspace warp transition: the old wallpaper zooms toward the camera,
// getting more and more radial streaks (a radial blur toward the middle) and
// brighter; at the peak a white-blue flash hides the swap; the new wallpaper
// then drops out of the warp, its zoom and streaks easing back to a sharp
// picture.
//
// Every output pixel averages a few samples along the ray toward the middle
// of the screen. Rows are split between a few worker threads.
// ---------------------------------------------------------------------------

struct WarpParams {
    bool useNew = false;
    float z = 1.0f;      // zoom (>= 1: the picture is magnified about the middle)
    float bl = 0.0f;     // streak length (0..0.6 of the way to the middle)
    float glow = 0.0f;   // brightness boost (0..1)
    float flash = 0.0f;  // white-blue flash (0..1)
    int n = 3;           // samples per pixel
};

struct WarpAnim;
DWORD WINAPI WarpWorker(LPVOID parameter);

struct WarpJob {
    WarpAnim* self;
    int y0, y1;
    const WarpParams* params;
};

struct WarpAnim {
    static constexpr float kMaxZoom = 2.6f;
    static constexpr double kFlashHalfWidth = 0.14;  // share of the time

    SIZE size{};
    std::vector<uint32_t> oldPx, newPx, frame;  // opaque, monitor-sized
    ComPtr<ID2D1Bitmap> reveal;

    bool Prepare(IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background) {
        size = monitorSize;
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!SlashAnim::Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        SlashAnim::Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        frame.assign(newPx.size(), bg);
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
    }

    // Draws the rows y0..y1.
    void RenderRows(int y0, int y1, const WarpParams& p) {
        const int W = size.cx, H = size.cy;
        const float cx = W * 0.5f, cy = H * 0.5f;
        const float invRmax = 1.0f / std::max(1.0f, std::hypot(cx, cy));
        const uint32_t* src = (p.useNew ? newPx : oldPx).data();
        const float invZ = 1.0f / p.z;
        const int n = std::max(1, p.n);
        const float invN = 1.0f / (float)n;
        const float invN1 = n > 1 ? 1.0f / (float)(n - 1) : 0.0f;
        const int glow256 = 256;
        for (int y = y0; y < y1; y++) {
            uint32_t* row = frame.data() + (size_t)y * W;
            const float dy = y + 0.5f - cy;
            for (int x = 0; x < W; x++) {
                const float dx = x + 0.5f - cx;
                int r = 0, g = 0, b = 0;
                for (int i = 0; i < n; i++) {
                    const float sf = invZ * (1.0f - p.bl * (float)i * invN1);
                    const int sx = std::clamp((int)(cx + dx * sf), 0, W - 1);
                    const int sy = std::clamp((int)(cy + dy * sf), 0, H - 1);
                    const uint32_t c = src[(size_t)sy * W + sx];
                    r += (int)((c >> 16) & 255);
                    g += (int)((c >> 8) & 255);
                    b += (int)(c & 255);
                }
                int R = (int)(r * invN), G = (int)(g * invN),
                    B = (int)(b * invN);
                if (glow256 > 256) {
                    R = std::min(255, (R * glow256) >> 8);
                    G = std::min(255, (G * glow256) >> 8);
                    B = std::min(255, (B * glow256) >> 8);
                }
                if (p.flash > 0.0f) {
                    // Brightest in the middle, still strong at the edges.
                    const float rr =
                        std::sqrt(dx * dx + dy * dy) * invRmax;
                    const float w = std::clamp(
                        p.flash * (0.75f + 0.25f * (1.0f - rr)), 0.0f, 1.0f);
                    const int wi = (int)(w * 256.0f);
                    R += ((6 - R) * wi) / 256;
                    G += ((8 - G) * wi) / 256;
                    B += ((18 - B) * wi) / 256;
                }
                row[x] = 0xFF000000u | ((uint32_t)R << 16) |
                         ((uint32_t)G << 8) | (uint32_t)B;
            }
        }
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty()) {
            return;
        }
        const int H = size.cy;
        const double t = std::clamp(time, 0.0, 1.0);
        if (t <= 0.0) {
            frame = oldPx;
        } else if (t >= 1.0) {
            frame = newPx;
        } else {
            WarpParams p;
            float b;
            if (t < 0.5) {
                // Old wallpaper: the zoom and the streaks speed up.
                const float u = (float)(t / 0.5);
                p.useNew = false;
                p.z = 1.0f + (kMaxZoom - 1.0f) * u * u * u;
                b = u * u;
            } else {
                // New wallpaper: out of the warp, slowing down.
                const float w = 1.0f - (float)((t - 0.5) / 0.5);
                p.useNew = true;
                p.z = 1.0f + (kMaxZoom - 1.0f) * w * w * w;
                b = w * w;
            }
            p.bl = b * 0.6f;
            p.glow = b;
            p.n = 3 + (int)(b * 13.0f);
            const double d = std::fabs(t - 0.5) / kFlashHalfWidth;
            p.flash = d < 1.0 ? (float)std::pow(1.0 - d, 1.5) : 0.0f;

            if (p.z <= 1.002f && p.bl <= 0.002f && p.flash <= 0.002f) {
                // (Nothing to do to the picture: a plain copy.)
                frame = p.useNew ? newPx : oldPx;
            } else {
                // Rows are split between a few threads (they don't overlap).
                int n = (int)std::min<DWORD>(
                    4, std::max<DWORD>(
                           1, GetActiveProcessorCount(ALL_PROCESSOR_GROUPS)));
                if (H < 240) {
                    n = 1;
                }
                const int band = (H + n - 1) / n;
                WarpJob jobs[4];
                HANDLE threads[4];
                int started = 0;
                for (int i = 1; i < n; i++) {
                    const int y0 = i * band;
                    const int y1 = std::min(H, (i + 1) * band);
                    if (y0 >= y1) {
                        break;
                    }
                    jobs[i] = {this, y0, y1, &p};
                    HANDLE th = CreateThread(nullptr, 0, WarpWorker, &jobs[i],
                                             0, nullptr);
                    if (th) {
                        threads[started++] = th;
                    } else {
                        RenderRows(y0, y1, p);
                    }
                }
                RenderRows(0, std::min(H, band), p);
                if (started) {
                    WaitForMultipleObjects((DWORD)started, threads, TRUE,
                                           INFINITE);
                    for (int i = 0; i < started; i++) {
                        CloseHandle(threads[i]);
                    }
                }
            }
        }
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
};

DWORD WINAPI WarpWorker(LPVOID parameter) {
    auto* job = (WarpJob*)parameter;
    job->self->RenderRows(job->y0, job->y1, *job->params);
    return 0;
}

// ---------------------------------------------------------------------------
// The burn transition: a fire starts where you pressed (and in a couple of
// other places a moment later) and eats the old wallpaper like paper. Just
// ahead of the fire the paper browns and chars black; the edge itself glows
// white-hot down to deep red, flickering; behind it the new wallpaper is lit
// warm by the flames, ember specks glow and die on it, and sparks fly up.
//
// Like Ink, every pixel gets a burn-arrival time once, up front, and a frame
// only touches the pixels near "now" (they are bucketed by time).
// ---------------------------------------------------------------------------

constexpr int kBurnT = 2047;  // burn-arrival times go 0..kBurnT

struct BurnLut {
    float hole = 0;    // 0..1: how much of the pixel is burnt away
    float scorch = 0;  // paper side: browning
    float charw = 0;   // paper side: black char
    float rim = 0;     // paper side: glow of the burning edge
    float spill = 0;   // hole side: firelight on the new picture
    float life = 0;    // hole side: how alive the ember specks still are
};

struct BurnSpark {
    float x = 0, y = 0, vx = 0, vy = 0, age = 0, life = 1, size = 1, phase = 0;
};

struct BurnAnim {
    SIZE size{};
    double lengthSeconds = 1.0;
    double lastT = 0;
    float sc = 1;
    int lead = 0, trail = 0;  // buckets: paper side / hole side of the front
    int lastP = 0;
    std::vector<uint32_t> oldPx, newPx, frame;  // opaque, monitor-sized
    std::vector<uint32_t> order;   // pixels sorted by burn time
    std::vector<uint32_t> bstart;  // where each burn time starts in `order`
    std::vector<uint8_t> nz;       // smooth noise per pixel (flicker phase)
    std::vector<uint8_t> grain;    // blocky random per pixel (ember specks)
    std::vector<BurnLut> lut;
    std::vector<BurnSpark> sparks;
    uint32_t pal[256] = {};        // fire colours: black ... white-hot
    float flick[256] = {};
    std::mt19937 rng{std::random_device{}()};
    ComPtr<ID2D1Bitmap> reveal;

    static uint32_t Pack(float R, float G, float B) {
        return 0xFF000000u |
               ((uint32_t)std::clamp((int)R, 0, 255) << 16) |
               ((uint32_t)std::clamp((int)G, 0, 255) << 8) |
               (uint32_t)std::clamp((int)B, 0, 255);
    }

    void AddFire(float& R, float& G, float& B, float x) const {
        if (x <= 0.02f) {
            return;
        }
        const uint32_t c = pal[std::clamp((int)(x * (255.0f / 1.2f)), 0, 255)];
        R += (float)((c >> 16) & 255);
        G += (float)((c >> 8) & 255);
        B += (float)(c & 255);
    }

    // A pixel of the old paper, `s` buckets ahead of the fire.
    uint32_t Paper(uint32_t o, const BurnLut& L, float f) const {
        float R = (float)((o >> 16) & 255);
        float G = (float)((o >> 8) & 255);
        float B = (float)(o & 255);
        const float w = L.scorch;
        if (w > 0.002f) {
            R *= 1.0f - 0.55f * w;
            G *= 1.0f - 0.75f * w;
            B *= 1.0f - 0.92f * w;
        }
        const float c = L.charw;
        if (c > 0.002f) {
            R += (14.0f - R) * c;
            G += (10.0f - G) * c;
            B += (8.0f - B) * c;
        }
        AddFire(R, G, B, L.rim * f);
        return Pack(R, G, B);
    }

    // A pixel of the new wallpaper, behind the fire.
    uint32_t Hole(uint32_t n, uint8_t g, const BurnLut& L, float f) const {
        float R = (float)((n >> 16) & 255);
        float G = (float)((n >> 8) & 255);
        float B = (float)(n & 255);
        const float sp = L.spill;
        if (sp > 0.004f) {
            const float lift = 1.0f + 0.35f * sp;
            R *= lift;
            G *= lift;
            B *= lift;
        }
        float x = 1.05f * sp * f;
        if (g >= 240 && L.life > 0.0f) {
            const float str = (float)(g - 240) * (1.0f / 15.0f);
            const float l2 = L.life * L.life;
            x += 0.95f * str * (str > 0.5f ? l2 : l2 * l2) *
                 (0.55f + 0.7f * f);
        }
        AddFire(R, G, B, x);
        return Pack(R, G, B);
    }

    // The heavy part (no drawing): both pictures' pixels, every pixel's burn
    // time, and the look-up tables.
    bool Prepare(IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background,
                 POINT ignition,
                 double seconds) {
        size = monitorSize;
        lengthSeconds = std::max(0.2, seconds);
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const size_t N = (size_t)W * H;
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!SlashAnim::Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        SlashAnim::Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        frame = oldPx;
        sc = std::max(1.0f, H / 1080.0f);

        auto rnd = [&](float a, float b) {
            return a + (b - a) * ((rng() & 0xFFFFFF) / 16777215.0f);
        };
        const float Wf = (float)W, Hf = (float)H;
        const float diag = std::hypot(Wf, Hf);

        // The fires: the first where you pressed, the others a bit later.
        struct Fire {
            float x, y, delay, speed;
        };
        std::vector<Fire> fires;
        fires.push_back({std::clamp((float)ignition.x, 0.0f, Wf),
                         std::clamp((float)ignition.y, 0.0f, Hf), 0.0f, 1.0f});
        const int extra = 2 + (int)(rng() % 2);
        for (int i = 0; i < extra; i++) {
            fires.push_back({rnd(0.1f * Wf, 0.9f * Wf),
                             rnd(0.15f * Hf, 0.95f * Hf),
                             rnd(0.05f, 0.14f) * diag, rnd(0.8f, 1.1f)});
        }

        InkNoise noise;
        const unsigned seed0 = (unsigned)rng();
        noise.Build(size, seed0);
        const int cell = std::max(2, (int)std::lround(3 * sc));
        const float amp0 = 22.0f * sc;
        std::vector<float> raw(N);
        nz.assign(N, 0);
        grain.assign(N, 0);
        float maxRaw = 1.0f;
        if (g_unloading) return false;
        for (int y = 0; y < H; y++) {
            if (g_unloading) return false;
            for (int x = 0; x < W; x++) {
                const size_t i = (size_t)y * W + x;
                const float n = noise.At((float)x, (float)y);  // -1..1
                float best = 1e30f;
                for (const Fire& f : fires) {
                    const float dx = (float)x - f.x;
                    float dy = (float)y - f.y;
                    dy *= dy < 0 ? 0.82f : 1.12f;  // fire climbs faster than it sinks
                    best = std::min(best,
                                    f.delay + std::sqrt(dx * dx + dy * dy) / f.speed);
                }
                float v = best + n * (amp0 + 0.09f * best);  // ragged fingers
                v += (InkHash(x, y, seed0) - 0.5f) * 3.0f * sc;  // paper fibres
                v = std::max(v, 0.0f);
                raw[i] = v;
                maxRaw = std::max(maxRaw, v);
                nz[i] = (uint8_t)std::clamp((int)((n * 0.5f + 0.5f) * 255.0f),
                                            0, 255);
                grain[i] = (uint8_t)std::clamp(
                    (int)(InkHash(x / cell, y / cell, seed0 + 77u) * 255.0f), 0,
                    255);
            }
        }
        const float bk = (float)kBurnT / maxRaw;  // buckets per pixel
        std::vector<uint16_t> T(N);
        for (size_t i = 0; i < N; i++) {
            T[i] = (uint16_t)std::clamp((int)std::lround(raw[i] * bk), 0,
                                        kBurnT);
        }
        raw.clear();
        raw.shrink_to_fit();

        // Pixels sorted by burn time.
        bstart.assign(kBurnT + 2, 0);
        for (size_t i = 0; i < N; i++) {
            bstart[T[i] + 1]++;
        }
        for (int v = 0; v <= kBurnT; v++) {
            bstart[v + 1] += bstart[v];
        }
        order.resize(N);
        std::vector<uint32_t> next(bstart.begin(), bstart.begin() + kBurnT + 1);
        for (size_t i = 0; i < N; i++) {
            order[next[T[i]]++] = (uint32_t)i;
        }

        // How the look changes with the distance to the front (in buckets;
        // positive = still paper, negative = already burnt away).
        auto sstep = [](float e0, float e1, float v) {
            const float t = std::clamp((v - e0) / (e1 - e0), 0.0f, 1.0f);
            return t * t * (3 - 2 * t);
        };
        const float featherB = std::max(1.0f, 1.3f * bk);
        const float rimB = std::max(1.5f, 7.0f * sc * bk);
        const float charB = std::max(4.0f, 24.0f * sc * bk);
        const float scorchB = std::max(8.0f, 80.0f * sc * bk);
        const float spillB = std::max(2.0f, 9.0f * sc * bk);
        const float lifeB = 0.20f * (float)kBurnT;
        lead = (int)std::ceil(scorchB + featherB) + 2;
        trail = (int)std::ceil(std::max(lifeB, 6.0f * spillB)) + 2;
        lut.assign((size_t)(lead + trail + 1), BurnLut{});
        for (int s = -trail; s <= lead; s++) {
            BurnLut& L = lut[(size_t)(s + trail)];
            const float sf = (float)s;
            const float h =
                std::clamp((featherB - sf) / (2 * featherB), 0.0f, 1.0f);
            L.hole = h * h * (3 - 2 * h);
            const float sp = std::max(sf, 0.0f);
            L.scorch = std::pow(1.0f - sstep(0.0f, scorchB, sp), 1.4f);
            L.charw = 1.0f - sstep(0.3f * charB, charB, sp);
            L.rim = 1.25f * std::exp(-sp / rimB);
            const float d = std::max(-sf, 0.0f);
            L.spill = std::exp(-d / spillB);
            L.life = std::clamp(1.0f - d / lifeB, 0.0f, 1.0f);
        }

        // Fire colours, from black through red and orange to white-hot.
        struct Stop {
            float x, r, g, b;
        };
        const Stop stops[] = {{0.00f, 0, 0, 0},       {0.22f, 95, 10, 0},
                              {0.45f, 215, 55, 4},    {0.70f, 255, 135, 20},
                              {0.92f, 255, 205, 80},  {1.20f, 255, 246, 205}};
        for (int i = 0; i < 256; i++) {
            const float x = i / 255.0f * 1.20f;
            int j = 0;
            while (j < 4 && x > stops[j + 1].x) {
                j++;
            }
            const float f = std::clamp(
                (x - stops[j].x) / (stops[j + 1].x - stops[j].x), 0.0f, 1.0f);
            const int r = (int)(stops[j].r + (stops[j + 1].r - stops[j].r) * f);
            const int g = (int)(stops[j].g + (stops[j + 1].g - stops[j].g) * f);
            const int b = (int)(stops[j].b + (stops[j + 1].b - stops[j].b) * f);
            pal[i] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        }
        // Flame flicker: a few waves of different speeds added together.
        for (int i = 0; i < 256; i++) {
            const float a = i * (2.0f * 3.14159265f / 256.0f);
            flick[i] = 0.78f + 0.16f * std::sin(a) +
                       0.09f * std::sin(3 * a + 1.1f) +
                       0.06f * std::sin(7 * a + 2.3f);
        }
        sparks.clear();
        lastT = 0;
        lastP = -lead - trail - 4;
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
        order.clear();
        order.shrink_to_fit();
        nz.clear();
        nz.shrink_to_fit();
        grain.clear();
        grain.shrink_to_fit();
        sparks.clear();
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty() || bstart.empty()) {
            return;
        }
        const int W = size.cx;
        const double t = std::clamp(time, 0.0, 1.0);
        const double dt = std::clamp((t - lastT) * lengthSeconds, 0.0, 0.05);
        lastT = t;
        if (t >= 1.0) {
            frame = newPx;
            sparks.clear();
        } else {
            // The front: slow to catch, then it runs; the last stretch of
            // time is the embers dying out.
            const double prog = std::pow(t, 1.2);
            const int P = (int)std::lround(
                -(double)lead + prog * (double)(kBurnT + lead + trail));
            const int tick = (int)(t * lengthSeconds * 540.0);
            const int lo = std::max(0, lastP - trail - 1);
            const int hi = std::min(kBurnT, P + lead);
            for (int tv = lo; tv <= hi; tv++) {
                const int s = tv - P;
                const uint32_t k0 = bstart[tv], k1 = bstart[tv + 1];
                if (s < -trail) {
                    // Done burning: the clean new picture.
                    for (uint32_t k = k0; k < k1; k++) {
                        const uint32_t idx = order[k];
                        frame[idx] = newPx[idx];
                    }
                    continue;
                }
                const BurnLut& L = lut[(size_t)(s + trail)];
                for (uint32_t k = k0; k < k1; k++) {
                    const uint32_t idx = order[k];
                    const float f = flick[(nz[idx] + tick) & 255];
                    uint32_t col;
                    if (L.hole >= 0.999f) {
                        col = Hole(newPx[idx], grain[idx], L, f);
                    } else if (L.hole <= 0.001f) {
                        col = Paper(oldPx[idx], L, f);
                    } else {
                        col = SlashLerp(Paper(oldPx[idx], L, f),
                                        Hole(newPx[idx], grain[idx], L, f),
                                        (uint32_t)(L.hole * 256.0f));
                    }
                    frame[idx] = col;
                }
            }
            lastP = P;

            // Sparks leave the burning edge and float up.
            auto rnd = [&](float a, float b) {
                return a + (b - a) * ((rng() & 0xFFFFFF) / 16777215.0f);
            };
            if (P >= 0 && P <= kBurnT && dt > 0.0) {
                const uint32_t cnt = bstart[P + 1] - bstart[P];
                if (cnt > 0) {
                    const float want = std::min(
                        30.0f, (float)cnt * 0.010f * (float)(dt * 60.0));
                    int n = (int)want;
                    if ((rng() & 0xFFFF) / 65535.0f < want - (float)n) {
                        n++;
                    }
                    for (int i = 0; i < n && sparks.size() < 700; i++) {
                        const uint32_t idx = order[bstart[P] + rng() % cnt];
                        BurnSpark sp;
                        sp.x = (float)(idx % (uint32_t)W);
                        sp.y = (float)(idx / (uint32_t)W);
                        sp.vx = rnd(-70.0f, 70.0f) * sc;
                        sp.vy = -rnd(50.0f, 260.0f) * sc;
                        sp.life = rnd(0.45f, 1.1f);
                        sp.size = rnd(1.1f, 2.6f) * sc;
                        sp.phase = rnd(0.0f, 6.28f);
                        sparks.push_back(sp);
                    }
                }
            }
            for (BurnSpark& sp : sparks) {
                sp.age += (float)dt;
                sp.vx += std::sin(sp.age * 9.0f + sp.phase) * 260.0f * sc *
                         (float)dt;  // flutter
                sp.vy -= 40.0f * sc * (float)dt;  // the heat lifts it
                sp.x += sp.vx * (float)dt;
                sp.y += sp.vy * (float)dt;
            }
            sparks.erase(std::remove_if(sparks.begin(), sparks.end(),
                                        [](const BurnSpark& s) {
                                            return s.age >= s.life;
                                        }),
                         sparks.end());
        }
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
        if (t < 1.0 && !sparks.empty()) {
            ComPtr<ID2D1SolidColorBrush> brush;
            target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                          brush.GetAddressOf());
            if (brush) {
                const float u =
                    (float)std::clamp((t - 0.90) / 0.10, 0.0, 1.0);
                const float endFade = 1.0f - u * u * (3 - 2 * u);
                for (const BurnSpark& sp : sparks) {
                    const float p = sp.age / sp.life;
                    const float a = std::pow(1.0f - p, 1.3f) * endFade;
                    if (a <= 0.01f) {
                        continue;
                    }
                    // Yellow-white at first, cooling to orange, then red.
                    const float cr = std::clamp(1.0f - 0.35f * p, 0.0f, 1.0f);
                    const float cg = std::clamp(0.95f - 0.75f * p, 0.0f, 1.0f);
                    const float cb = std::clamp(0.55f - 0.55f * p, 0.0f, 1.0f);
                    const float rad = sp.size * (1.0f - 0.45f * p);
                    const D2D1_POINT_2F c = D2D1::Point2F(sp.x, sp.y);
                    brush->SetColor(D2D1::ColorF(1.0f, 0.45f, 0.08f, 0.10f * a));
                    target->FillEllipse(D2D1::Ellipse(c, rad * 3.2f, rad * 3.2f),
                                        brush.Get());
                    brush->SetColor(D2D1::ColorF(1.0f, 0.55f, 0.12f, 0.55f * a));
                    target->DrawLine(
                        c,
                        D2D1::Point2F(sp.x - sp.vx * 0.035f,
                                      sp.y - sp.vy * 0.035f),
                        brush.Get(), rad * 1.3f);
                    brush->SetColor(D2D1::ColorF(cr, cg, cb, 0.95f * a));
                    target->FillEllipse(D2D1::Ellipse(c, rad, rad), brush.Get());
                }
            }
        }
    }
};

// ---------------------------------------------------------------------------
// The freeze transition: frost starts where you pressed (and in a couple of
// other places a moment later) and spreads as a six-armed crystal. Ahead of
// it, crystal flecks of frost creep over the old wallpaper; at the front a
// cold bright line; behind it the new wallpaper sits under milky frosted glass
// that thaws away in uneven patches with shiny edges. Glints twinkle on the
// frost and ice dust drifts down.
//
// Like Burn and Ink, every pixel gets an arrival time once, up front, and a
// frame only touches the pixels near "now" (rows of them split between a few
// threads).
// ---------------------------------------------------------------------------

// Box-blurs `count` lines of `len` pixels (rows, or columns), the edges
// repeating. Used by FrostBlur.
void FrostBlurLines(const uint32_t* in,
                    uint32_t* out,
                    int count,
                    int len,
                    size_t elemStep,
                    size_t lineStep,
                    int r) {
    const int win = 2 * r + 1;
    for (int l = 0; l < count; l++) {
        const uint32_t* a = in + (size_t)l * lineStep;
        uint32_t* b = out + (size_t)l * lineStep;
        int sr = 0, sg = 0, sb = 0;
        for (int i = -r; i <= r; i++) {
            const uint32_t c = a[(size_t)std::clamp(i, 0, len - 1) * elemStep];
            sr += (int)((c >> 16) & 255);
            sg += (int)((c >> 8) & 255);
            sb += (int)(c & 255);
        }
        for (int x = 0; x < len; x++) {
            b[(size_t)x * elemStep] = 0xFF000000u |
                                      ((uint32_t)(sr / win) << 16) |
                                      ((uint32_t)(sg / win) << 8) |
                                      (uint32_t)(sb / win);
            const uint32_t add =
                a[(size_t)std::clamp(x + r + 1, 0, len - 1) * elemStep];
            const uint32_t rem =
                a[(size_t)std::clamp(x - r, 0, len - 1) * elemStep];
            sr += (int)((add >> 16) & 255) - (int)((rem >> 16) & 255);
            sg += (int)((add >> 8) & 255) - (int)((rem >> 8) & 255);
            sb += (int)(add & 255) - (int)(rem & 255);
        }
    }
}

// A blurred copy of a monitor-sized picture: shrunk 4x, box-blurred twice,
// then scaled back up smoothly (much cheaper than blurring at full size).
void FrostBlur(const std::vector<uint32_t>& src,
               int W,
               int H,
               int radius,
               std::vector<uint32_t>* out) {
    constexpr int ds = 4;
    const int w = std::max(1, (W + ds - 1) / ds);
    const int h = std::max(1, (H + ds - 1) / ds);
    std::vector<uint32_t> a((size_t)w * h), b((size_t)w * h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sr = 0, sg = 0, sb = 0, n = 0;
            for (int yy = y * ds; yy < std::min(H, y * ds + ds); yy++) {
                for (int xx = x * ds; xx < std::min(W, x * ds + ds); xx++) {
                    const uint32_t c = src[(size_t)yy * W + xx];
                    sr += (int)((c >> 16) & 255);
                    sg += (int)((c >> 8) & 255);
                    sb += (int)(c & 255);
                    n++;
                }
            }
            n = std::max(n, 1);
            a[(size_t)y * w + x] = 0xFF000000u | ((uint32_t)(sr / n) << 16) |
                                   ((uint32_t)(sg / n) << 8) |
                                   (uint32_t)(sb / n);
        }
    }
    const int r = std::max(1, radius / ds);
    for (int pass = 0; pass < 2; pass++) {
        FrostBlurLines(a.data(), b.data(), h, w, 1, (size_t)w, r);  // rows
        FrostBlurLines(b.data(), a.data(), w, h, (size_t)w, 1, r);  // columns
    }
    out->resize((size_t)W * H);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            (*out)[(size_t)y * W + x] = SlashSample(
                a.data(), w, h, (x + 0.5f) / ds, (y + 0.5f) / ds);
        }
    }
}

constexpr int kFreezeT = 2047;  // arrival times go 0..kFreezeT

struct FreezeLut {
    float hole = 0;  // 0..1: how much of the pixel is past the front
    float xl = 0;    // ahead of the front: 0 at it ... 1 where the frost starts
    float xd = 0;    // behind the front: 0 at it ... 1 where it has thawed
    float rim = 0;   // cold glow around the front
    float core = 0;  // its bright thin line
};

struct IceSpark {
    float x = 0, y = 0, vx = 0, vy = 0, age = 0, life = 1, size = 1, phase = 0;
};

struct FreezeAnim;
DWORD WINAPI FreezeWorker(LPVOID parameter);

struct FreezeJob {
    FreezeAnim* self;
    int tvLo, tvHi, P, tick;
};

struct FreezeAnim {
    SIZE size{};
    double lengthSeconds = 1.0;
    double lastT = 0;
    float sc = 1;
    int lead = 0, trail = 0;  // buckets: ahead of the front / behind it
    int lastP = 0;
    std::vector<uint32_t> oldPx, newPx, oldFrost, newFrost, frame;
    std::vector<uint32_t> order;   // pixels sorted by arrival time
    std::vector<uint32_t> bstart;  // where each arrival time starts in `order`
    std::vector<uint8_t> nz;       // smooth noise per pixel (thaw patches)
    std::vector<uint8_t> grain;    // blocky random per pixel (flecks, glints)
    std::vector<FreezeLut> lut;
    std::vector<IceSpark> sparks;
    float twinkle[256] = {};
    std::mt19937 rng{std::random_device{}()};
    ComPtr<ID2D1Bitmap> reveal;

    static uint32_t Pack(float R, float G, float B) {
        return 0xFF000000u |
               ((uint32_t)std::clamp((int)R, 0, 255) << 16) |
               ((uint32_t)std::clamp((int)G, 0, 255) << 8) |
               (uint32_t)std::clamp((int)B, 0, 255);
    }

    static float Sstep(float e0, float e1, float v) {
        const float t = std::clamp((v - e0) / (e1 - e0), 0.0f, 1.0f);
        return t * t * (3 - 2 * t);
    }

    // One side of the front: the picture, frosted by F (0 = clear, 1 = solid
    // frost), with a shine on the frost's edges and twinkling glints.
    uint32_t Frost(uint32_t clean,
                   uint32_t frosted,
                   float F,
                   uint8_t g,
                   uint8_t nzv,
                   int tick) const {
        if (F <= 0.002f) {
            return clean;
        }
        const uint32_t col =
            F >= 0.998f ? frosted
                        : SlashLerp(clean, frosted, (uint32_t)(F * 256.0f));
        const float e = 4.0f * F * (1.0f - F);
        const bool glint = g >= 244 && F > 0.35f;
        if (e < 0.03f && !glint) {
            return col;
        }
        float R = (float)((col >> 16) & 255);
        float G = (float)((col >> 8) & 255);
        float B = (float)(col & 255);
        R += 55.0f * e;
        G += 85.0f * e;
        B += 105.0f * e;
        if (glint) {
            const float k =
                170.0f * F *
                twinkle[(tick + (int)g * 37 + (int)nzv * 11) & 255];
            R += k;
            G += k;
            B += k;
        }
        return Pack(R, G, B);
    }

    uint32_t Shade(uint32_t idx, const FreezeLut& L, int tick) const {
        const uint8_t nzv = nz[idx];
        const uint8_t g = grain[idx];
        const float nzf = nzv * (1.0f / 255.0f);
        const float gf = g * (1.0f / 255.0f);
        uint32_t a = 0, b = 0;
        if (L.hole < 0.999f) {
            // The old wallpaper, flecks of frost creeping in.
            const float thr = 0.15f + 0.73f * 0.5f * (nzf + gf);
            const float F = 1.0f - Sstep(thr - 0.12f, thr + 0.12f, L.xl);
            a = Frost(oldPx[idx], oldFrost[idx], F, g, nzv, tick);
        }
        if (L.hole > 0.001f) {
            // The new wallpaper, thawing in patches.
            const float thr = 0.12f + 0.72f * (0.8f * nzf + 0.2f * gf);
            const float F = 1.0f - Sstep(thr - 0.12f, thr + 0.12f, L.xd);
            b = Frost(newPx[idx], newFrost[idx], F, g, nzv, tick);
        }
        uint32_t col =
            L.hole >= 0.999f ? b
            : L.hole <= 0.001f
                ? a
                : SlashLerp(a, b, (uint32_t)(L.hole * 256.0f));
        if (L.rim > 0.02f) {
            // The cold bright line along the front.
            float R = (float)((col >> 16) & 255);
            float G = (float)((col >> 8) & 255);
            float B = (float)(col & 255);
            R += 40.0f * L.rim + 90.0f * L.core;
            G += 95.0f * L.rim + 100.0f * L.core;
            B += 120.0f * L.rim + 110.0f * L.core;
            col = Pack(R, G, B);
        }
        return col;
    }

    // Updates the pixels of the burn-times tvLo..tvHi (different threads get
    // different ones).
    void Process(int tvLo, int tvHi, int P, int tick) {
        for (int tv = tvLo; tv <= tvHi; tv++) {
            const int s = tv - P;
            const uint32_t k0 = bstart[tv], k1 = bstart[tv + 1];
            if (s < -trail) {
                // Thawed: the clean new picture.
                for (uint32_t k = k0; k < k1; k++) {
                    const uint32_t idx = order[k];
                    frame[idx] = newPx[idx];
                }
                continue;
            }
            const FreezeLut& L = lut[(size_t)(s + trail)];
            for (uint32_t k = k0; k < k1; k++) {
                const uint32_t idx = order[k];
                frame[idx] = Shade(idx, L, tick);
            }
        }
    }

    // The heavy part (no drawing): both pictures and their frosted copies,
    // every pixel's arrival time, and the look-up tables.
    bool Prepare(IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background,
                 POINT ignition,
                 double seconds) {
        size = monitorSize;
        lengthSeconds = std::max(0.2, seconds);
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const size_t N = (size_t)W * H;
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!SlashAnim::Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        SlashAnim::Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        frame = oldPx;
        sc = std::max(1.0f, H / 1080.0f);

        // Frosted glass copies of both pictures: blurred, washed with ice blue.
        const int blurR = (int)std::lround(16 * sc);
        FrostBlur(oldPx, W, H, blurR, &oldFrost);
        FrostBlur(newPx, W, H, blurR, &newFrost);
        for (std::vector<uint32_t>* v : {&oldFrost, &newFrost}) {
            for (uint32_t& c : *v) {
                c = Pack((float)((c >> 16) & 255) * 0.58f + 86.0f,
                         (float)((c >> 8) & 255) * 0.58f + 97.0f,
                         (float)(c & 255) * 0.58f + 105.0f);
            }
        }

        auto rnd = [&](float a, float b) {
            return a + (b - a) * ((rng() & 0xFFFFFF) / 16777215.0f);
        };
        const float Wf = (float)W, Hf = (float)H;
        const float diag = std::hypot(Wf, Hf);

        // The crystals: the first where you pressed, the others a bit later.
        // Each grows faster along six arms (turned by its own angle).
        struct Crystal {
            float x, y, delay, speed, cs, sn;
        };
        std::vector<Crystal> crystals;
        auto addCrystal = [&](float x, float y, float delay, float speed) {
            const float a = rnd(0.0f, 1.0472f);
            crystals.push_back({x, y, delay, speed, std::cos(a), std::sin(a)});
        };
        addCrystal(std::clamp((float)ignition.x, 0.0f, Wf),
                   std::clamp((float)ignition.y, 0.0f, Hf), 0.0f, 1.0f);
        const int extra = 1 + (int)(rng() % 2);
        for (int i = 0; i < extra; i++) {
            addCrystal(rnd(0.1f * Wf, 0.9f * Wf), rnd(0.1f * Hf, 0.9f * Hf),
                       rnd(0.05f, 0.12f) * diag, rnd(0.85f, 1.1f));
        }

        InkNoise noise;
        const unsigned seed0 = (unsigned)rng();
        noise.Build(size, seed0);
        const int cell = std::max(2, (int)std::lround(3 * sc));
        const float amp0 = 20.0f * sc;
        std::vector<float> raw(N);
        nz.assign(N, 0);
        grain.assign(N, 0);
        float maxRaw = 1.0f;
        for (int y = 0; y < H; y++) {
            if (g_unloading) return false;
            for (int x = 0; x < W; x++) {
                const size_t i = (size_t)y * W + x;
                const float n = noise.At((float)x, (float)y);  // -1..1
                float best = 1e30f;
                for (const Crystal& c : crystals) {
                    const float dx = (float)x - c.x;
                    const float dy = (float)y - c.y;
                    const float dist = std::sqrt(dx * dx + dy * dy);
                    float arms = 1.0f;
                    if (dist > 1.0f) {
                        // cos(6 * angle) from cos(angle): six arms.
                        const float cc = (dx * c.cs + dy * c.sn) / dist;
                        const float c2 = cc * cc, c4 = c2 * c2;
                        const float cos6 =
                            32.0f * c4 * c2 - 48.0f * c4 + 18.0f * c2 - 1.0f;
                        const float p = 0.5f + 0.5f * cos6;
                        arms = 1.0f + 0.5f * p * p * p;
                    }
                    best = std::min(best, c.delay + dist / (c.speed * arms));
                }
                float v = best + n * (amp0 + 0.07f * best);  // feathery edge
                v += (InkHash(x, y, seed0) - 0.5f) * 2.0f * sc;
                v = std::max(v, 0.0f);
                raw[i] = v;
                maxRaw = std::max(maxRaw, v);
                nz[i] = (uint8_t)std::clamp(
                    (int)((0.5f + 0.5f * std::clamp(n * 2.2f, -1.0f, 1.0f)) *
                          255.0f),
                    0, 255);
                grain[i] = (uint8_t)std::clamp(
                    (int)(InkHash(x / cell, y / cell, seed0 + 77u) * 255.0f), 0,
                    255);
            }
        }
        const float bk = (float)kFreezeT / maxRaw;  // buckets per pixel
        std::vector<uint16_t> T(N);
        for (size_t i = 0; i < N; i++) {
            T[i] = (uint16_t)std::clamp((int)std::lround(raw[i] * bk), 0,
                                        kFreezeT);
        }
        raw.clear();
        raw.shrink_to_fit();

        // Pixels sorted by arrival time.
        bstart.assign(kFreezeT + 2, 0);
        for (size_t i = 0; i < N; i++) {
            bstart[T[i] + 1]++;
        }
        for (int v = 0; v <= kFreezeT; v++) {
            bstart[v + 1] += bstart[v];
        }
        order.resize(N);
        std::vector<uint32_t> next(bstart.begin(),
                                   bstart.begin() + kFreezeT + 1);
        for (size_t i = 0; i < N; i++) {
            order[next[T[i]]++] = (uint32_t)i;
        }

        // How the look changes with the distance to the front (in buckets;
        // positive = still ahead of it, negative = behind it).
        const float featherB = std::max(1.0f, 1.3f * bk);
        const float rimB = std::max(1.5f, 5.0f * sc * bk);
        const float leadB = std::max(8.0f, 90.0f * sc * bk);
        const float trailB = 0.34f * (float)kFreezeT;
        lead = (int)std::ceil(leadB + featherB) + 2;
        trail = (int)std::ceil(trailB) + 2;
        lut.assign((size_t)(lead + trail + 1), FreezeLut{});
        for (int s = -trail; s <= lead; s++) {
            FreezeLut& L = lut[(size_t)(s + trail)];
            const float sf = (float)s;
            const float h =
                std::clamp((featherB - sf) / (2 * featherB), 0.0f, 1.0f);
            L.hole = h * h * (3 - 2 * h);
            L.xl = std::clamp(std::max(sf, 0.0f) / leadB, 0.0f, 1.0f);
            L.xd = std::clamp(std::max(-sf, 0.0f) / trailB, 0.0f, 1.0f);
            const float a = std::fabs(sf);
            L.rim = std::exp(-a / rimB);
            L.core = std::exp(-a / (0.35f * rimB));
        }
        // Glint twinkle: short bright pulses.
        for (int i = 0; i < 256; i++) {
            const float s = std::sin(i * (2.0f * 3.14159265f / 256.0f) * 2.0f);
            twinkle[i] = s > 0.0f ? std::pow(s, 6.0f) : 0.0f;
        }
        sparks.clear();
        lastT = 0;
        lastP = -lead - trail - 4;
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        oldFrost.clear();
        oldFrost.shrink_to_fit();
        newFrost.clear();
        newFrost.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
        order.clear();
        order.shrink_to_fit();
        nz.clear();
        nz.shrink_to_fit();
        grain.clear();
        grain.shrink_to_fit();
        sparks.clear();
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty() || bstart.empty()) {
            return;
        }
        const int W = size.cx;
        const double t = std::clamp(time, 0.0, 1.0);
        const double dt = std::clamp((t - lastT) * lengthSeconds, 0.0, 0.05);
        lastT = t;
        if (t >= 1.0) {
            frame = newPx;
            sparks.clear();
        } else {
            // The frost runs fast at first; the thaw is the slow last part.
            const double prog = 1.0 - std::pow(1.0 - t, 1.6);
            const int P = (int)std::lround(
                -(double)lead + prog * (double)(kFreezeT + lead + trail));
            const int tick = (int)(t * lengthSeconds * 540.0);
            const int lo = std::max(0, lastP - trail - 1);
            const int hi = std::min(kFreezeT, P + lead);
            if (lo <= hi) {
                // The rows of work are split between a few threads (by the
                // number of pixels, they don't overlap).
                const uint32_t pxLo = bstart[lo], pxHi = bstart[hi + 1];
                int n = (int)std::min<DWORD>(
                    4, std::max<DWORD>(
                           1, GetActiveProcessorCount(ALL_PROCESSOR_GROUPS)));
                if (pxHi - pxLo < 80000) {
                    n = 1;
                }
                int cut[5] = {lo, hi + 1, hi + 1, hi + 1, hi + 1};
                for (int i = 1; i < n; i++) {
                    const uint32_t want =
                        pxLo + (uint32_t)((uint64_t)(pxHi - pxLo) * i / n);
                    const auto it = std::lower_bound(
                        bstart.begin() + lo, bstart.begin() + hi + 2, want);
                    cut[i] = std::clamp((int)(it - bstart.begin()), cut[i - 1],
                                        hi + 1);
                }
                cut[n] = hi + 1;
                FreezeJob jobs[4];
                HANDLE threads[4];
                int started = 0;
                for (int i = 1; i < n; i++) {
                    if (cut[i] >= cut[i + 1]) {
                        continue;
                    }
                    jobs[i] = {this, cut[i], cut[i + 1] - 1, P, tick};
                    HANDLE th = CreateThread(nullptr, 0, FreezeWorker, &jobs[i],
                                             0, nullptr);
                    if (th) {
                        threads[started++] = th;
                    } else {
                        Process(cut[i], cut[i + 1] - 1, P, tick);
                    }
                }
                Process(cut[0], cut[1] - 1, P, tick);
                if (started) {
                    WaitForMultipleObjects((DWORD)started, threads, TRUE,
                                           INFINITE);
                    for (int i = 0; i < started; i++) {
                        CloseHandle(threads[i]);
                    }
                }
            }
            lastP = P;

            // Ice dust leaves the front and drifts down.
            auto rnd = [&](float a, float b) {
                return a + (b - a) * ((rng() & 0xFFFFFF) / 16777215.0f);
            };
            if (P >= 0 && P <= kFreezeT && dt > 0.0) {
                const uint32_t cnt = bstart[P + 1] - bstart[P];
                if (cnt > 0) {
                    const float want = std::min(
                        22.0f, (float)cnt * 0.007f * (float)(dt * 60.0));
                    int n = (int)want;
                    if ((rng() & 0xFFFF) / 65535.0f < want - (float)n) {
                        n++;
                    }
                    for (int i = 0; i < n && sparks.size() < 500; i++) {
                        const uint32_t idx = order[bstart[P] + rng() % cnt];
                        IceSpark sp;
                        sp.x = (float)(idx % (uint32_t)W);
                        sp.y = (float)(idx / (uint32_t)W);
                        sp.vx = rnd(-25.0f, 25.0f) * sc;
                        sp.vy = rnd(-20.0f, 40.0f) * sc;
                        sp.life = rnd(0.5f, 1.2f);
                        sp.size = rnd(1.2f, 2.8f) * sc;
                        sp.phase = rnd(0.0f, 6.28f);
                        sparks.push_back(sp);
                    }
                }
            }
            for (IceSpark& sp : sparks) {
                sp.age += (float)dt;
                sp.vx += std::sin(sp.age * 5.0f + sp.phase) * 30.0f * sc *
                         (float)dt;  // sways
                sp.vy += 20.0f * sc * (float)dt;  // sinks
                sp.x += sp.vx * (float)dt;
                sp.y += sp.vy * (float)dt;
            }
            sparks.erase(std::remove_if(sparks.begin(), sparks.end(),
                                        [](const IceSpark& s) {
                                            return s.age >= s.life;
                                        }),
                         sparks.end());
        }
        reveal->CopyFromMemory(nullptr, frame.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
        if (t < 1.0 && !sparks.empty()) {
            ComPtr<ID2D1SolidColorBrush> brush;
            target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                          brush.GetAddressOf());
            if (brush) {
                const float u =
                    (float)std::clamp((t - 0.90) / 0.10, 0.0, 1.0);
                const float endFade = 1.0f - u * u * (3 - 2 * u);
                for (const IceSpark& sp : sparks) {
                    const float p = sp.age / sp.life;
                    const float tw =
                        0.55f + 0.45f * std::sin(sp.age * 28.0f + sp.phase);
                    const float a =
                        std::pow(1.0f - p, 0.8f) * tw * endFade;
                    if (a <= 0.01f) {
                        continue;
                    }
                    const float rad = sp.size * (1.0f - 0.3f * p);
                    const D2D1_POINT_2F c = D2D1::Point2F(sp.x, sp.y);
                    brush->SetColor(D2D1::ColorF(0.55f, 0.82f, 1.0f, 0.10f * a));
                    target->FillEllipse(D2D1::Ellipse(c, rad * 3.0f, rad * 3.0f),
                                        brush.Get());
                    // A four-point glint.
                    brush->SetColor(D2D1::ColorF(0.85f, 0.95f, 1.0f, 0.8f * a));
                    const float arm = rad * 2.6f;
                    const float wd = std::max(1.0f, rad * 0.5f);
                    target->DrawLine(D2D1::Point2F(sp.x - arm, sp.y),
                                     D2D1::Point2F(sp.x + arm, sp.y),
                                     brush.Get(), wd);
                    target->DrawLine(D2D1::Point2F(sp.x, sp.y - arm),
                                     D2D1::Point2F(sp.x, sp.y + arm),
                                     brush.Get(), wd);
                    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f * a));
                    target->FillEllipse(
                        D2D1::Ellipse(c, rad * 0.9f, rad * 0.9f), brush.Get());
                }
            }
        }
    }
};

DWORD WINAPI FreezeWorker(LPVOID parameter) {
    auto* job = (FreezeJob*)parameter;
    job->self->Process(job->tvLo, job->tvHi, job->P, job->tick);
    return 0;
}

// ---------------------------------------------------------------------------
// The dust transition. The old wallpaper crumbles into thousands of its own
// grains that the wind carries away; the new wallpaper is left behind.
// Without a subject the crumbling is a wind-swept wave. With a subject (the
// cut-out PNG Ink uses) it starts on the subject's outline: the subject
// appears first, then the crumbling spreads outward over the rest.
//
// Like Ink / Burn, every pixel gets an arrival time once, up front, and a
// frame only touches the pixels near "now". The grains are drawn straight
// into the frame's pixels.
// ---------------------------------------------------------------------------

constexpr int kDustT = 2047;                 // arrival times go 0..kDustT
constexpr int kDustFeather = 4;              // soft edge of each pixel's change
constexpr double kDustFront = 0.68;          // share of the time the crumbling takes
constexpr size_t kDustMaxParticles = 160000;

struct DustParticle {
    float x, y, vx, vy, age, life, phase;
    uint32_t color;
    int size;
};

struct DustAnim {
    SIZE size{};
    double lengthSeconds = 1.0;
    double lastT = 0;
    float sc = 1;
    bool subject = false;
    int lastP = -kDustFeather - 1;
    int spawnP = -1;
    int step = 16;  // one pixel in `step` of the crumbling edge becomes a grain
    std::vector<uint32_t> oldPx, newPx, frame, shown;
    std::vector<uint32_t> order;   // pixels sorted by arrival time
    std::vector<uint32_t> bstart;  // where each arrival time starts in `order`
    std::vector<DustParticle> parts;
    uint16_t alphaLut[2 * kDustFeather + 1] = {};
    uint8_t rimLut[2 * kDustFeather + 1] = {};
    std::mt19937 rng{std::random_device{}()};
    ComPtr<ID2D1Bitmap> reveal;

    // The heavy part (no drawing): both pictures, the subject's outline and
    // every pixel's arrival time.
    bool Prepare(IWICImagingFactory* wic,
                 IWICBitmapSource* newPicture,
                 const D2D1_RECT_F& newRect,
                 IWICBitmapSource* oldPicture,
                 const D2D1_RECT_F& oldRect,
                 SIZE monitorSize,
                 COLORREF background,
                 DESKTOP_WALLPAPER_POSITION position,
                 const std::wstring& subjectPath,
                 double seconds) {
        size = monitorSize;
        lengthSeconds = std::max(0.2, seconds);
        const int W = size.cx, H = size.cy;
        if (W <= 0 || H <= 0 || !newPicture) {
            return false;
        }
        const size_t N = (size_t)W * H;
        const uint32_t bg = 0xFF000000u |
                            ((uint32_t)GetRValue(background) << 16) |
                            ((uint32_t)GetGValue(background) << 8) |
                            (uint32_t)GetBValue(background);
        if (!SlashAnim::Compose(newPicture, newRect, size, bg, &newPx)) {
            return false;
        }
        // (No readable old picture = the plain background colour.)
        SlashAnim::Compose(oldPicture, oldRect, size, bg, &oldPx);
        if (oldPx.size() != newPx.size()) {
            oldPx.assign(newPx.size(), bg);
        }
        frame = oldPx;
        shown = oldPx;
        sc = std::max(1.0f, H / 1080.0f);
        const float minDim = (float)std::min(W, H);
        const unsigned seed0 = (unsigned)rng();
        InkNoise noise;
        noise.Build(size, seed0);
        std::vector<float> raw(N, 0.0f);  // arrival time, in pixels

        // The subject's cutout (the same one Ink uses).
        std::vector<uint8_t> mask;
        subject = false;
        if (!subjectPath.empty()) {
            D2D1_RECT_F place{};
            ComPtr<IWICBitmapSource> pic = LoadWallpaperPicture(
                wic, subjectPath.c_str(), size, position, &place);
            UINT sw = 0, sh = 0;
            size_t covered = 0;
            if (pic && SUCCEEDED(pic->GetSize(&sw, &sh)) && sw && sh) {
                std::vector<uint32_t> sp((size_t)sw * sh);
                if (SUCCEEDED(pic->CopyPixels(nullptr, sw * 4,
                                              (UINT)(sp.size() * 4),
                                              (BYTE*)sp.data()))) {
                    mask.assign(N, 0);
                    const int sx0 = (int)std::floor(place.left);
                    const int sy0 = (int)std::floor(place.top);
                    for (int y = 0; y < (int)sh; y++) {
                        const int dy = y + sy0;
                        if (dy < 0 || dy >= H) {
                            continue;
                        }
                        for (int x = 0; x < (int)sw; x++) {
                            const int dx = x + sx0;
                            if (dx >= 0 && dx < W &&
                                (sp[(size_t)y * sw + x] >> 24) >= 128) {
                                mask[(size_t)dy * W + dx] = 1;
                                covered++;
                            }
                        }
                    }
                }
            }
            // (A picture with no transparency, or almost nothing in it, has
            // no subject.)
            subject = !mask.empty() && covered > N / 500 &&
                      covered < N * 97 / 100;
        }

        if (subject) {
            // Distance to the outline. The inside goes faster, so the subject
            // is complete well before the rest of the picture.
            std::vector<float> d(N, 1e9f), u(N, 0.0f);
            size_t seeds = 0;
            for (int y = 0; y < H; y++) {
                for (int x = 0; x < W; x++) {
                    const size_t i = (size_t)y * W + x;
                    if (!mask[i]) {
                        continue;
                    }
                    const bool edge = (x > 0 && !mask[i - 1]) ||
                                      (x < W - 1 && !mask[i + 1]) ||
                                      (y > 0 && !mask[i - W]) ||
                                      (y < H - 1 && !mask[i + W]);
                    if (edge) {
                        d[i] = 0;
                        seeds++;
                    }
                }
            }
            if (!seeds) {
                subject = false;
            } else {
                InkChamfer(W, H, d, u);
                for (size_t i = 0; i < N; i++) {
                    raw[i] = mask[i] ? d[i] * 0.55f : d[i];
                }
            }
        }
        mask.clear();
        mask.shrink_to_fit();

        if (!subject) {
            // A wave across the screen, tilted a little, going with the wind.
            const float ang = -0.32f;
            const float dx = std::cos(ang), dy = std::sin(ang);
            const float projMin = std::min(
                {0.0f, (float)W * dx, (float)H * dy, (float)W * dx + (float)H * dy});
            for (int y = 0; y < H; y++) {
                for (int x = 0; x < W; x++) {
                    raw[(size_t)y * W + x] =
                        (float)x * dx + (float)y * dy - projMin;
                }
            }
        }

        // Ragged, clumpy, grainy edge: big noise, patches of ~20 px, and
        // blocky grain, spread over a wide crumbling band.
        const float ampBase = (subject ? 9.0f : 26.0f) * sc;
        const float bandW = (subject ? 0.075f : 0.13f) * minDim;
        const float patchScale = 20.0f * sc;
        const int cell = std::max(2, (int)std::lround(2.2f * sc));
        float maxRaw = 1.0f;
        for (int y = 0; y < H; y++) {
            if (g_unloading) return false;
            for (int x = 0; x < W; x++) {
                const size_t i = (size_t)y * W + x;
                const float e = raw[i];
                const float n = noise.At((float)x, (float)y);  // -1..1
                const float patch =
                    InkValueNoise((float)x / patchScale + 5.3f,
                                  (float)y / patchScale + 1.7f, seed0 + 11u) -
                    0.5f;
                const float grain =
                    InkHash(x / cell, y / cell, seed0 + 7u) - 0.5f;
                float v = e + 0.75f * bandW + n * (ampBase + 0.08f * e) +
                          (0.55f * grain + 0.95f * patch) * bandW;
                v = std::max(v, 0.0f);
                raw[i] = v;
                maxRaw = std::max(maxRaw, v);
            }
        }
        std::vector<uint16_t> T(N);
        const float bk = (float)kDustT / maxRaw;
        for (size_t i = 0; i < N; i++) {
            T[i] = (uint16_t)std::clamp((int)std::lround(raw[i] * bk), 0,
                                        kDustT);
        }
        raw.clear();
        raw.shrink_to_fit();

        // Pixels sorted by arrival time.
        bstart.assign(kDustT + 2, 0);
        for (size_t i = 0; i < N; i++) {
            bstart[T[i] + 1]++;
        }
        for (int v = 0; v <= kDustT; v++) {
            bstart[v + 1] += bstart[v];
        }
        order.resize(N);
        std::vector<uint32_t> next(bstart.begin(), bstart.begin() + kDustT + 1);
        for (size_t i = 0; i < N; i++) {
            order[next[T[i]]++] = (uint32_t)i;
        }

        // The soft edge of a pixel's change, and a faint glint on it.
        for (int dd = -kDustFeather; dd <= kDustFeather; dd++) {
            const float s = (dd + kDustFeather) / (2.0f * kDustFeather);
            const float e = s * s * (3 - 2 * s);
            alphaLut[dd + kDustFeather] = (uint16_t)std::lround(e * 256.0f);
            rimLut[dd + kDustFeather] =
                (uint8_t)std::lround(4.0f * e * (1.0f - e) * 34.0f);
        }
        step = std::max(1, (int)(N / 150000));
        parts.clear();
        lastT = 0;
        lastP = -kDustFeather - 1;
        spawnP = -1;
        return true;
    }

    // Needs the render target (call between its Begin and Finish).
    bool CreateResources(ID2D1DCRenderTarget* target) {
        target->SetDpi(96.0f, 96.0f);  // (1 unit = 1 pixel)
        return SUCCEEDED(target->CreateBitmap(
            D2D1::SizeU((UINT32)size.cx, (UINT32)size.cy), nullptr, 0,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            reveal.GetAddressOf()));
    }

    void Release() {
        reveal.Reset();
        oldPx.clear();
        oldPx.shrink_to_fit();
        newPx.clear();
        newPx.shrink_to_fit();
        frame.clear();
        frame.shrink_to_fit();
        shown.clear();
        shown.shrink_to_fit();
        order.clear();
        order.shrink_to_fit();
        parts.clear();
        parts.shrink_to_fit();
    }

    // time: 0..1 through the transition. Draws into the (already begun)
    // target.
    void Draw(ID2D1DCRenderTarget* target, double time) {
        if (!reveal || frame.empty() || bstart.empty()) {
            return;
        }
        const int W = size.cx, H = size.cy;
        const double t = std::clamp(time, 0.0, 1.0);
        const double dt = std::clamp((t - lastT) * lengthSeconds, 0.0, 0.05);
        lastT = t;
        if (t >= 1.0) {
            shown = newPx;
            parts.clear();
        } else {
            // The crumbling runs for the first part of the time; the rest is
            // the last grains blowing away.
            const double x = std::clamp(t / kDustFront, 0.0, 1.0);
            const double prog = x * x * (3 - 2 * x);
            const int P = (int)std::lround(
                            prog * (double)(kDustT + 2 * kDustFeather)) -
                          kDustFeather;
            const int lo = std::max(0, lastP - kDustFeather);
            const int hi = std::min(kDustT, P + kDustFeather);
            for (int tv = lo; tv <= hi; tv++) {
                const int diff =
                    std::clamp(P - tv, -kDustFeather, kDustFeather);
                const uint32_t a = alphaLut[diff + kDustFeather];
                const uint32_t rim = rimLut[diff + kDustFeather];
                for (uint32_t k = bstart[tv]; k < bstart[tv + 1]; k++) {
                    const uint32_t idx = order[k];
                    uint32_t c = a >= 256 ? newPx[idx]
                                 : a == 0 ? oldPx[idx]
                                          : SlashLerp(oldPx[idx], newPx[idx], a);
                    if (rim) {
                        const uint32_t r =
                            std::min<uint32_t>(255, ((c >> 16) & 255) + rim);
                        const uint32_t g =
                            std::min<uint32_t>(255, ((c >> 8) & 255) + rim);
                        const uint32_t b =
                            std::min<uint32_t>(255, (c & 255) + rim);
                        c = 0xFF000000u | (r << 16) | (g << 8) | b;
                    }
                    frame[idx] = c;
                }
            }
            lastP = P;

            // Grains leave the crumbling edge (the old pixel's own colour).
            auto rnd = [&](float a, float b) {
                return a + (b - a) * ((rng() & 0xFFFFFF) / 16777215.0f);
            };
            if (dt > 0.0) {
                const float remain = (float)((1.0 - t) * lengthSeconds);
                const int upTo = std::min(kDustT, P);
                for (int tv = std::max(0, spawnP + 1); tv <= upTo; tv++) {
                    const uint32_t k0 = bstart[tv], k1 = bstart[tv + 1];
                    if (k1 <= k0) {
                        continue;
                    }
                    for (uint32_t k = k0 + (uint32_t)(rng() % (unsigned)step);
                         k < k1; k += (uint32_t)step) {
                        if (parts.size() >= kDustMaxParticles) {
                            break;
                        }
                        const uint32_t idx = order[k];
                        DustParticle p{};
                        p.x = (float)(idx % (uint32_t)W);
                        p.y = (float)(idx / (uint32_t)W);
                        const float speed = rnd(140.0f, 540.0f) * sc;
                        p.vx = 0.949f * speed + rnd(-40.0f, 40.0f) * sc;
                        p.vy = -0.314f * speed + rnd(-90.0f, 50.0f) * sc;
                        p.age = 0;
                        p.life = std::min(rnd(0.55f, 1.25f),
                                          std::max(0.15f, remain * 0.97f));
                        p.phase = rnd(0.0f, 6.28f);
                        p.color = oldPx[idx];
                        p.size = std::clamp(
                            (int)std::lround(rnd(1.3f, 2.9f) * sc), 1, 6);
                        parts.push_back(p);
                    }
                }
                spawnP = std::max(spawnP, upTo);
            }

            // Move the grains: they start slowly, then the wind takes them.
            size_t keep = 0;
            for (size_t i = 0; i < parts.size(); i++) {
                DustParticle p = parts[i];
                p.age += (float)dt;
                if (p.age >= p.life) {
                    continue;
                }
                const float ramp = std::min(1.0f, p.age * 3.5f);
                p.x += p.vx * ramp * (float)dt;
                p.y += (p.vy * ramp +
                        std::sin(p.age * 7.0f + p.phase) * 40.0f * sc) *
                       (float)dt;
                parts[keep++] = p;
            }
            parts.resize(keep);

            // The frame, plus the grains on top of it.
            shown = frame;
            for (const DustParticle& p : parts) {
                const float q = p.age / p.life;
                const uint32_t a256 = (uint32_t)((1.0f - q * q) * 256.0f);
                if (a256 < 6) {
                    continue;
                }
                const int x0 = (int)p.x, y0 = (int)p.y;
                const uint32_t col = p.color | 0xFF000000u;
                for (int yy = std::max(0, y0); yy < std::min(H, y0 + p.size);
                     yy++) {
                    uint32_t* row = shown.data() + (size_t)yy * W;
                    for (int xx = std::max(0, x0);
                         xx < std::min(W, x0 + p.size); xx++) {
                        row[xx] = SlashLerp(row[xx], col, a256);
                    }
                }
            }
        }
        reveal->CopyFromMemory(nullptr, shown.data(), (UINT32)size.cx * 4);
        target->DrawBitmap(reveal.Get(),
                           D2D1::RectF(0, 0, (float)size.cx, (float)size.cy),
                           1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
};

struct WallpaperChange {
    std::wstring path;
    RECT monitor;
    Transition transition;
    // Where a circle grows from (on the screen).
    POINT origin;
    double seconds;
    // Ink transition: the cut-out of the subject (empty = ink drops).
    std::wstring subject;
};

// ---- Z-order helpers: put the transition above the clock widget and the YASB bar ----

// True if window `a` is higher in the z-order than window `b`.
static bool ZIsAbove(HWND a, HWND b) {
    int guard = 0;
    for (HWND w = GetWindow(a, GW_HWNDNEXT); w && guard < 2000;
         w = GetWindow(w, GW_HWNDNEXT), guard++) {
        if (w == b) {
            return true;
        }
    }
    return false;
}

// Puts `window` directly above the clock widget and the bar, but still under
// normal app windows. Safe to call every few frames: it does nothing if the
// position is already right.
static void PlaceTransitionBase(HWND window, bool coverWidgets = true) {
    HWND progman = FindWindow(L"Progman", nullptr);
    if (!progman) {
        SetWindowPos(window, HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        return;
    }
    auto isTopmost = [](HWND w) {
        return w && (GetWindowLongPtr(w, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    };

    // Only the clock widget is covered. The YASB bar is not looked for at
    // all, so it always stays above the transition. The transition is never
    // topmost, so it also stays under every app window.
    // While animating, the transition covers the clock widget. After the
    // animation (coverWidgets = false) it sits right above the desktop, under
    // the clock and every other widget, so they show again at once.
    HWND clock = coverWidgets ? FindWindow(L"WhDesktopClock_Clock", nullptr)
                              : nullptr;
    if (clock && (!IsWindowVisible(clock) || isTopmost(clock))) {
        clock = nullptr;
    }
    HWND target = clock ? clock : progman;  // no clock: just above the desktop

    HWND prev = GetWindow(target, GW_HWNDPREV);
    if (prev == window) {
        return;  // already directly above the target
    }
    if (!prev || isTopmost(prev)) {
        SetWindowPos(window, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else {
        // Insert right below `prev`, which is right above the target.
        SetWindowPos(window, prev, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

struct BarList {
    HWND self;
    HWND bars[8];
    int count;
};

// Collects every visible YASB bar window (one per monitor).
static BOOL CALLBACK CollectBarsProc(HWND w, LPARAM lp) {
    BarList* list = (BarList*)lp;
    if (w == list->self || !IsWindowVisible(w) || list->count >= 8) {
        return TRUE;
    }
    wchar_t title[128]{};
    GetWindowText(w, title, ARRAYSIZE(title));
    for (wchar_t* p = title; *p; p++) {
        *p = (wchar_t)towlower(*p);
    }
    if (wcsstr(title, L"yasb")) {
        RECT r{};
        GetWindowRect(w, &r);
        if (r.right - r.left > 400 && r.bottom - r.top < 200) {
            list->bars[list->count++] = w;
        }
    }
    return TRUE;
}

// Places the transition over the desktop and the clock widget, then makes
// sure the YASB bar(s) are above it (but still under app windows).
static void PlaceTransitionWindow(HWND window, bool coverWidgets = true) {
    PlaceTransitionBase(window, coverWidgets);

    BarList list{window, {}, 0};
    EnumWindows(CollectBarsProc, (LPARAM)&list);
    for (int i = 0; i < list.count; i++) {
        HWND bar = list.bars[i];
        if (GetWindowLongPtr(bar, GWL_EXSTYLE) & WS_EX_TOPMOST) {
            continue;  // a topmost bar is already above the transition
        }
        if (!ZIsAbove(window, bar)) {
            continue;  // the bar is already above the transition
        }
        // Put the bar directly above the transition.
        HWND prev = GetWindow(window, GW_HWNDPREV);
        if (!prev || (GetWindowLongPtr(prev, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
            SetWindowPos(bar, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        } else {
            SetWindowPos(bar, prev, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

DWORD WINAPI WallpaperChangeThreadProc(LPVOID parameter) {
    std::unique_ptr<WallpaperChange> change((WallpaperChange*)parameter);
    // (In the monitor's real pixels.)
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    // How Windows places its wallpaper, and the color around a smaller one.
    DESKTOP_WALLPAPER_POSITION wallpaperPosition = DWPOS_FILL;
    COLORREF backgroundColor = 0;
    {
        ComPtr<IDesktopWallpaper> wallpaper;
        if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr,
                                       CLSCTX_ALL,
                                       IID_PPV_ARGS(wallpaper.GetAddressOf())))) {
            wallpaper->GetPosition(&wallpaperPosition);
            wallpaper->GetBackgroundColor(&backgroundColor);
        }
    }
    D2D1_RECT_F pictureRect{};
    const SIZE size{change->monitor.right - change->monitor.left,
                    change->monitor.bottom - change->monitor.top};
    const double length = change->seconds;
    ComPtr<IWICImagingFactory> wic;
    ComPtr<ID2D1Factory> factory;
    ComPtr<IWICBitmapSource> picture;
    LayeredCanvas canvas;
    HWND window = nullptr;
    if (length > 0 &&
        SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(wic.GetAddressOf()))) &&
        SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                    factory.GetAddressOf())) &&
        canvas.Create(factory.Get()) &&
        (picture = LoadWallpaperPicture(wic.Get(), change->path.c_str(), size,
                                        wallpaperPosition, &pictureRect))) {
        // It doesn't take the mouse, and stays under the apps' windows,
        // just above the desktop.
        window = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                    WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                L"STATIC", nullptr, WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, nullptr, nullptr);
    }
    // The ink transition works its pixels out first (a moment; the window
    // isn't shown yet).
    InkAnim ink;
    bool inkReady = false;
    if (window && change->transition == Transition::Ink) {
        try {
            inkReady = ink.Prepare(wic.Get(), picture.Get(), pictureRect,
                                   size, backgroundColor, wallpaperPosition,
                                   change->subject);
        } catch (...) {
            inkReady = false;
            ink = InkAnim();  // let go of the big arrays
        }
    }
    // The slash transition needs the old wallpaper too (what's on the
    // screen now), laid out the way Windows shows it.
    SlashAnim slash;
    bool slashReady = false;
    if (window && change->transition == Transition::Slash) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            slashReady = slash.Prepare(picture.Get(), pictureRect,
                                       oldPicture.Get(), oldRect, size,
                                       backgroundColor);
        } catch (...) {
            slashReady = false;
            slash = SlashAnim();  // let go of the big arrays
        }
    }
    // The whip transition needs the old wallpaper too, like Slash.
    WhipAnim whip;
    bool whipReady = false;
    if (window && change->transition == Transition::Whip) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            whipReady = whip.Prepare(picture.Get(), pictureRect,
                                     oldPicture.Get(), oldRect, size,
                                     backgroundColor);
        } catch (...) {
            whipReady = false;
            whip = WhipAnim();  // let go of the big arrays
        }
    }
    // The glitch transition needs the old wallpaper too, like Slash and Whip.
    GlitchAnim glitch;
    bool glitchReady = false;
    if (window && change->transition == Transition::Glitch) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            glitchReady = glitch.Prepare(picture.Get(), pictureRect,
                                         oldPicture.Get(), oldRect, size,
                                         backgroundColor);
        } catch (...) {
            glitchReady = false;
            glitch = GlitchAnim();  // let go of the big arrays
        }
    }
    // The freeze transition needs the old wallpaper too, like Slash and Whip.
    FreezeAnim freeze;
    bool freezeReady = false;
    if (window && change->transition == Transition::Freeze) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            // Where the frost starts, on this monitor (the mouse, or the
            // middle when it comes from the picker).
            const POINT ignition{change->origin.x - change->monitor.left,
                                 change->origin.y - change->monitor.top};
            freezeReady = freeze.Prepare(picture.Get(), pictureRect,
                                         oldPicture.Get(), oldRect, size,
                                         backgroundColor, ignition, length);
        } catch (...) {
            freezeReady = false;
            freeze = FreezeAnim();  // let go of the big arrays
        }
    }
    // The dust transition needs the old wallpaper too, and the subject.
    DustAnim dust;
    bool dustReady = false;
    if (window && change->transition == Transition::Dust) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            dustReady = dust.Prepare(wic.Get(), picture.Get(), pictureRect,
                                     oldPicture.Get(), oldRect, size,
                                     backgroundColor, wallpaperPosition,
                                     change->subject, length);
        } catch (...) {
            dustReady = false;
            dust = DustAnim();  // let go of the big arrays
        }
    }
    // The burn transition needs the old wallpaper too, like Slash and Whip.
    BurnAnim burn;
    bool burnReady = false;
    if (window && change->transition == Transition::Burn) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            // Where the fire starts, on this monitor (the mouse, or the
            // middle when it comes from the picker).
            const POINT ignition{change->origin.x - change->monitor.left,
                                 change->origin.y - change->monitor.top};
            burnReady = burn.Prepare(picture.Get(), pictureRect,
                                     oldPicture.Get(), oldRect, size,
                                     backgroundColor, ignition, length);
        } catch (...) {
            burnReady = false;
            burn = BurnAnim();  // let go of the big arrays
        }
    }
    // The warp transition needs the old wallpaper too, like Slash and Whip.
    WarpAnim warp;
    bool warpReady = false;
    if (window && change->transition == Transition::Warp) {
        try {
            D2D1_RECT_F oldRect{};
            ComPtr<IWICBitmapSource> oldPicture;
            const std::wstring oldPath = CurrentWallpaperPath(change->monitor);
            if (!oldPath.empty()) {
                oldPicture = LoadWallpaperPicture(wic.Get(), oldPath.c_str(),
                                                  size, wallpaperPosition,
                                                  &oldRect);
            }
            warpReady = warp.Prepare(picture.Get(), pictureRect,
                                     oldPicture.Get(), oldRect, size,
                                     backgroundColor);
        } catch (...) {
            warpReady = false;
            warp = WarpAnim();  // let go of the big arrays
        }
    }
    ComPtr<ID2D1Bitmap> bitmap;
    if (window && canvas.Begin(size)) {
        canvas.target->CreateBitmapFromWicBitmap(picture.Get(), nullptr,
                                                 bitmap.GetAddressOf());
        if (inkReady) {
            inkReady = ink.CreateResources(canvas.target.Get(), factory.Get());
        }
        if (slashReady) {
            slashReady = slash.CreateResources(canvas.target.Get());
        }
        if (whipReady) {
            whipReady = whip.CreateResources(canvas.target.Get());
        }
        if (glitchReady) {
            glitchReady = glitch.CreateResources(canvas.target.Get());
        }
        if (warpReady) {
            warpReady = warp.CreateResources(canvas.target.Get());
        }
        if (burnReady) {
            burnReady = burn.CreateResources(canvas.target.Get());
        }
        if (freezeReady) {
            freezeReady = freeze.CreateResources(canvas.target.Get());
        }
        if (dustReady) {
            dustReady = dust.CreateResources(canvas.target.Get());
        }
        canvas.Finish();
    }
    if (change->transition == Transition::Ink && !inkReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Slash && !slashReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Whip && !whipReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Glitch && !glitchReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Freeze && !freezeReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Dust && !dustReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Burn && !burnReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    if (change->transition == Transition::Warp && !warpReady) {
        change->transition = Transition::Circle;  // (it couldn't start)
    }
    const POINT position{change->monitor.left, change->monitor.top};
    // Placed above the desktop, the clock widget and the YASB bar, but still
    // under normal app windows (see PlaceTransitionWindow).
    bool coverWidgets = true;
    auto placeOverDesktop = [window, &coverWidgets]() {
        PlaceTransitionWindow(window, coverWidgets);
    };
    if (window && bitmap) {
        // Not a full-screen app to the taskbar (it covers the whole
        // monitor): a taskbar left showing the desktop stopped being on top
        // during the transition (explorer's "NonRudeHWND" mark).
        SetProp(window, L"NonRudeHWND", (HANDLE)TRUE);
        placeOverDesktop();
        const float w = (float)size.cx;
        const float h = (float)size.cy;
        const D2D1_POINT_2F origin{
            (float)(change->origin.x - change->monitor.left),
            (float)(change->origin.y - change->monitor.top)};
        const float reach = std::hypot(std::max(origin.x, w - origin.x),
                                       std::max(origin.y, h - origin.y));
        const double start = NowSeconds();
        bool shown = false;
        int frameCount = 0;
        for (;;) {
            const double t =
                std::clamp((NowSeconds() - start) / length, 0.0, 1.0);
            const float k = (float)(t < 0.5 ? 4 * t * t * t
                                            : 1 - std::pow(-2 * t + 2, 3) / 2);
            if (!canvas.Begin(size)) {
                break;
            }
            ID2D1DCRenderTarget* target = canvas.target.Get();
            const D2D1_RECT_F full{0, 0, w, h};
            // The background color around a smaller picture, then it (`x`
            // to the right).
            ComPtr<ID2D1SolidColorBrush> fillBrush;
            target->CreateSolidColorBrush(
                D2D1::ColorF(GetRValue(backgroundColor) / 255.0f,
                             GetGValue(backgroundColor) / 255.0f,
                             GetBValue(backgroundColor) / 255.0f, 1),
                fillBrush.GetAddressOf());
            auto drawPicture = [&](float x, float opacity) {
                if (fillBrush) {
                    fillBrush->SetOpacity(opacity);
                    target->FillRectangle({x, 0, x + w, h}, fillBrush.Get());
                }
                target->DrawBitmap(bitmap.Get(),
                                   {pictureRect.left + x, pictureRect.top,
                                    pictureRect.right + x, pictureRect.bottom},
                                   opacity,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            };
            switch (change->transition) {
                case Transition::Circle: {
                    ComPtr<ID2D1EllipseGeometry> circle;
                    factory->CreateEllipseGeometry(
                        D2D1::Ellipse(origin, reach * k, reach * k),
                        circle.GetAddressOf());
                    ComPtr<ID2D1Layer> layer;
                    if (circle &&
                        SUCCEEDED(target->CreateLayer(nullptr,
                                                      layer.GetAddressOf()))) {
                        target->PushLayer(
                            D2D1::LayerParameters(full, circle.Get()),
                            layer.Get());
                        drawPicture(0, 1);
                        target->PopLayer();
                    }
                    break;
                }
                case Transition::Slide: {
                    drawPicture((1 - k) * w, 0.4f + 0.6f * k);
                    break;
                }
                case Transition::Zoom: {
                    target->SetTransform(D2D1::Matrix3x2F::Scale(
                        1.12f - 0.12f * k, 1.12f - 0.12f * k,
                        D2D1::Point2F(w / 2, h / 2)));
                    drawPicture(0, k);
                    target->SetTransform(D2D1::Matrix3x2F::Identity());
                    break;
                }
                case Transition::Ink:
                    ink.Draw(target, t);
                    break;
                case Transition::Slash:
                    slash.Draw(target, t);
                    break;
                case Transition::Whip:
                    whip.Draw(target, t);
                    break;
                case Transition::Glitch:
                    glitch.Draw(target, t);
                    break;
                case Transition::Warp:
                    warp.Draw(target, t);
                    break;
                case Transition::Burn:
                    burn.Draw(target, t);
                    break;
                case Transition::Freeze:
                    freeze.Draw(target, t);
                    break;
                case Transition::Dust:
                    dust.Draw(target, t);
                    break;
                default:
                    drawPicture(0, k);
                    break;
            }
            canvas.End(window, position, 255);
            if (!shown) {
                ShowWindow(window, SW_SHOWNOACTIVATE);
                placeOverDesktop();
                shown = true;
            } else if (++frameCount % 10 == 0) {
                placeOverDesktop();  // the bar may have re-raised itself
            }
            if (t >= 1 || g_unloading) {
                break;
            }
            WaitForFrame();
        }
    }
    // (The window is no longer hidden from captures: it stays visible, to the
    // screen and to recordings alike, until Windows has surely shown the new
    // wallpaper under it.)
    const std::vector<DWORD> wanted;
    const bool sampled = false;
    const std::vector<DWORD> before;
    // The animation is over: the finished picture stays as a still image
    // UNDER the clock / visualizer, so they come back right now while
    // Windows loads the real wallpaper beneath.
    // The clock stays covered for a moment (see kWidgetRevealDelaySeconds).
    const double animationEnd = NowSeconds();
    const double setStart = animationEnd;
    SetMonitorWallpaper(change->monitor, change->path.c_str());
    const double setTook = NowSeconds() - setStart;
    // How long to keep the still picture: the minimum, plus a bit for big
    // files, plus extra if Windows was slow to take the new wallpaper.
    double holdSeconds = kWallpaperHoldSeconds;
    {
        WIN32_FILE_ATTRIBUTE_DATA fileAttr{};
        if (GetFileAttributesEx(change->path.c_str(), GetFileExInfoStandard,
                                &fileAttr)) {
            const double mb =
                (double)(((unsigned long long)fileAttr.nFileSizeHigh << 32) |
                         fileAttr.nFileSizeLow) /
                (1024.0 * 1024.0);
            holdSeconds += mb * kWallpaperHoldPerMB;
        }
    }
    holdSeconds += std::max(0.0, setTook - 0.05) * 4.0;
    // Probe how busy the compositor is: a few frames right after the change.
    // On a loaded system they are slow, and Windows is also slow to finish
    // showing the new wallpaper, so the picture must stay up longer.
    {
        const double probeStart = NowSeconds();
        for (int i = 0; i < 6 && !g_unloading; i++) {
            WaitForFrame();
        }
        const double perFrame = (NowSeconds() - probeStart) / 6.0;
        holdSeconds += std::max(0.0, perFrame - 0.020) * 60.0;
    }
    holdSeconds = std::min(holdSeconds, kWallpaperHoldMaxSeconds);
    Wh_Log(L"Wallpaper hold %.2f s (SetWallpaper took %.3f s)", holdSeconds,
           setTook);
    // Tell the Desktop Clock Widget mod (if loaded) to switch its layout now,
    // while the transition still covers the screen.
    if (HWND clockCtrl = FindWindow(L"WhDesktopClock_Ctrl", nullptr)) {
        PostMessage(clockCtrl, kClockWallpaperNow, 0, 0);
    }
    if (window) {
        // It stays until Windows shows the new one under it as it does (its
        // own fade and zoom done: the desktop's pixels look like it and
        // hold still), not over the old one; then it fades away. With
        // nothing of the desktop showing (or the same picture), it goes at
        // once; after a while at most, and at once when the mod is
        // unloading.
        const int pixels = kScreenSampleWidth * kScreenSampleHeight;
        std::vector<size_t> places;
        if (before.size() == wanted.size()) {
            for (size_t i = 0; i < before.size(); i++) {
                if (PixelDiffers(before[i], wanted[i], 24)) {
                    places.push_back(i);
                }
            }
        }
        const double since = NowSeconds();
        std::vector<DWORD> last = before;
        int still = 0;
        while (!places.empty() && !g_unloading &&
               NowSeconds() - since < kWallpaperShownSeconds && still < 1) {
            Sleep(kWallpaperSampleMs);
            std::vector<DWORD> now = SampleScreen(change->monitor);
            if (now.size() != before.size()) {
                break;
            }
            // (A fade moves a few levels a look: "still" is almost none.)
            still = SampleShows(now, wanted, places) &&
                            SampleChanges(now, last, 4) <= pixels / 50
                        ? still + 1
                        : 0;
            last = std::move(now);
        }
        // Keep the finished picture on screen until Windows has surely
        // finished its own fade underneath (stops early if the mod unloads).
        if (IsWindowVisible(window) && !g_unloading) {
            // Waits in short slices (ends at once on unload) and keeps the
            // still picture under the widgets.
            const double holdEnd = NowSeconds() + holdSeconds;
            while (!g_unloading) {
                const double left = holdEnd - NowSeconds();
                if (left <= 0) {
                    break;
                }
                if (WaitForSingleObject(
                        g_stop, (DWORD)(std::min(left, 0.03) * 1000) + 1) ==
                    WAIT_OBJECT_0) {
                    break;
                }
                if (coverWidgets &&
                    NowSeconds() - animationEnd >= kWidgetRevealDelaySeconds) {
                    coverWidgets = false;  // the clock shows now
                }
                placeOverDesktop();
            }
        }
        // Once more just before the window leaves, in case Windows hadn't
        // reported the new picture yet the first time.
        if (HWND clockCtrl = FindWindow(L"WhDesktopClock_Ctrl", nullptr)) {
            PostMessage(clockCtrl, kClockWallpaperNow, 0, 0);
        }
        if (sampled) {
            SetWindowDisplayAffinity(window, WDA_NONE);
        }
        for (int step = 7; step >= 0; step--) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)(step * 255 / 7),
                                AC_SRC_ALPHA};
            UpdateLayeredWindow(window, nullptr, nullptr, nullptr, nullptr,
                                nullptr, 0, &blend, ULW_ALPHA);
            if (!g_unloading) {
                WaitForFrame();
            }
        }
        RemoveProp(window, L"NonRudeHWND");
        DestroyWindow(window);
    }
    ink.Release();
    slash.Release();
    whip.Release();
    glitch.Release();
    warp.Release();
    burn.Release();
    freeze.Release();
    dust.Release();
    bitmap.Reset();
    canvas.Destroy();
    factory.Reset();
    picture.Reset();
    wic.Reset();
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    g_busy--;
    // Tell the main thread, so a press made meanwhile can run now.
    if (!g_unloading && g_hwnd) {
        PostMessage(g_hwnd, WM_APP_STEPDONE, 0, 0);
    }
    g_threads--;
    return 0;
}

// ---------------------------------------------------------------------------
// Which monitors, and starting a change
// ---------------------------------------------------------------------------

BOOL CALLBACK EnumMonitorsProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    MONITORINFO info{sizeof(info)};
    if (GetMonitorInfo(monitor, &info)) {
        ((std::vector<RECT>*)param)->push_back(info.rcMonitor);
    }
    return TRUE;
}

std::vector<RECT> TargetMonitors(POINT cursor) {
    std::vector<RECT> monitors;
    auto add = [&](HMONITOR monitor) {
        MONITORINFO info{sizeof(info)};
        if (GetMonitorInfo(monitor, &info)) {
            monitors.push_back(info.rcMonitor);
        }
    };
    if (g_cfg.monitors == 2) {
        EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc,
                            (LPARAM)&monitors);
    } else if (g_cfg.monitors == 1) {
        add(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY));
    } else {
        add(MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY));
    }
    return monitors;
}

// The ink transition's subjects (a cut-out PNG per wallpaper), remembered by
// the wallpaper's path.
std::wstring InkSubjectKey(const std::wstring& wallpaper) {
    unsigned long long h = 1469598103934665603ull;
    for (wchar_t ch : wallpaper) {
        h ^= (unsigned long long)towlower(ch);
        h *= 1099511628211ull;
    }
    static const wchar_t* hex = L"0123456789abcdef";
    std::wstring key = L"subject_";
    for (int shift = 60; shift >= 0; shift -= 4) {
        key += hex[(h >> shift) & 15];
    }
    return key;
}

std::wstring InkSubjectFor(const std::wstring& wallpaper) {
    std::wstring subject = GetStoredString(InkSubjectKey(wallpaper).c_str());
    if (!subject.empty() && !PathFileExists(subject.c_str())) {
        subject.clear();
    }
    return subject;
}

// A picture of the folder comes in as the wallpaper, with the transition
// chosen (one at a time). `fromCenter`: a circle grows from the middle of
// the monitor (from the picker), not from the mouse. `subject`: for the ink
// transition, the cut-out of the picture's subject (empty = ink drops).
void ApplyWallpaper(const std::wstring& path,
                    bool fromCenter,
                    const std::wstring& subject = L"") {
    if (g_busy > 0 || g_unloading) {
        return;
    }
    const double seconds =
        g_cfg.durationMs <= 0 ? 0.0
        : g_cfg.transition == (int)Transition::Ink
            ? (subject.empty() ? g_cfg.inkDropsMs : g_cfg.inkMs) / 1000.0
        : g_cfg.transition == (int)Transition::Slash
            ? g_cfg.slashMs / 1000.0
        : g_cfg.transition == (int)Transition::Whip
            ? g_cfg.whipMs / 1000.0
        : g_cfg.transition == (int)Transition::Glitch
            ? g_cfg.glitchMs / 1000.0
        : g_cfg.transition == (int)Transition::Warp
            ? g_cfg.warpMs / 1000.0
        : g_cfg.transition == (int)Transition::Burn
            ? g_cfg.burnMs / 1000.0
            : g_cfg.transition == (int)Transition::Freeze
            ? g_cfg.freezeMs / 1000.0
            : g_cfg.transition == (int)Transition::Dust
            ? g_cfg.dustMs / 1000.0
            : g_cfg.durationMs / 1000.0;
    POINT cursor{};
    GetCursorPos(&cursor);
    for (const RECT& monitor : TargetMonitors(cursor)) {
        POINT origin = !fromCenter && PtInRect(&monitor, cursor)
                           ? cursor
                           : POINT{(monitor.left + monitor.right) / 2,
                                   (monitor.top + monitor.bottom) / 2};
        auto* change = new WallpaperChange{path,
                                           monitor,
                                           (Transition)g_cfg.transition,
                                           origin,
                                           seconds,
                                           subject};
        g_busy++;
        g_threads++;
        HANDLE thread = CreateThread(nullptr, 0, WallpaperChangeThreadProc,
                                     change, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_busy--;
            g_threads--;
            delete change;
        }
    }
    g_lastPath = path;
    Wh_SetStringValue(L"lastPath", path.c_str());
}

int IndexOfPath(const std::vector<std::wstring>& paths,
                const std::wstring& path) {
    if (path.empty()) {
        return -1;
    }
    for (size_t i = 0; i < paths.size(); i++) {
        if (_wcsicmp(paths[i].c_str(), path.c_str()) == 0) {
            return (int)i;
        }
    }
    return -1;
}

void OpenPanel();

// step: +1 next, -1 previous, 0 random.
void StepWallpaper(int step) {
    if (g_busy > 0) {
        // A transition is still running: remember this press (the last one
        // wins) and do it when that transition ends.
        g_pendingStep = step;
        g_stepPending = true;
        return;
    }
    g_stepPending = false;
    const std::vector<std::wstring> paths = ListWallpapers(g_cfg.folder);
    if (paths.empty()) {
        // No pictures there (or no folder chosen yet): the picker asks for
        // a folder.
        OpenPanel();
        return;
    }
    POINT cursor{};
    GetCursorPos(&cursor);
    const std::vector<RECT> monitors = TargetMonitors(cursor);
    int current = -1;
    if (!monitors.empty()) {
        current = IndexOfPath(paths, CurrentWallpaperPath(monitors[0]));
    }
    if (current < 0) {
        current = IndexOfPath(paths, g_lastPath);
    }
    const int count = (int)paths.size();
    int index;
    if (step == 0) {
        static std::mt19937 rng{std::random_device{}()};
        index = (int)(rng() % (unsigned)count);
        if (count > 1 && index == current) {
            index = (index + 1 + (int)(rng() % (unsigned)(count - 1))) % count;
        }
    } else {
        index = current < 0 ? (step > 0 ? 0 : count - 1)
                            : ((current + step) % count + count) % count;
    }
    ApplyWallpaper(paths[index], false,
                   UsesSubject(g_cfg.transition)
                       ? InkSubjectFor(paths[index])
                       : std::wstring());
}

// ---------------------------------------------------------------------------
// The picker: a strip of the folder's pictures in the middle of the screen,
// the chosen one big (the preview). A layered window (per-pixel alpha), just
// the strip's height.
// ---------------------------------------------------------------------------

struct ThumbJob {
    HWND hwnd;
    std::vector<std::pair<int, std::wstring>> items;  // (index, path)
    SIZE size;
    unsigned generation;
    bool slow = false;  // hex grid: spread the pictures out
};

struct ThumbResult {
    unsigned generation;
    int index;
    HBITMAP bitmap;
};

struct CarouselItem {
    ComPtr<ID2D1Bitmap> bitmap;
    ComPtr<ID2D1Bitmap> hexBmp;  // the picture cut to the hexagon (once)
    ComPtr<ID2D1Bitmap> spBmp;   // the picture cut to a circle (spiral)
    ComPtr<ID2D1Bitmap> spMip;   // a smaller copy for the small circles
    std::wstring badge;          // the name shown in the hex's pill
    ComPtr<ID2D1Bitmap> fishBmp, fishMip;  // rounded tile (full / half size)
    ComPtr<ID2D1Bitmap> tunBmp, tunMip;    // tunnel frame (full / half size)
    std::vector<uint32_t> ambPx;  // blurred, darkened backdrop pixels
    ComPtr<ID2D1Bitmap> lqR, lqG, lqB;  // liquid: one colour channel each (RGB split)
    char state = 0;  // 0 not asked, 1 being read, 2 read (or failed)
};

struct CarouselState {
    HWND hwnd = nullptr;
    LayeredCanvas canvas;
    ComPtr<ID2D1Factory> d2d;
    ComPtr<IDWriteFactory> dwrite;
    ComPtr<IWICImagingFactory> wic;
    ComPtr<IDWriteTextFormat> labelFormat;
    ComPtr<IDWriteTextFormat> hintFormat;
    std::vector<std::wstring> paths;
    std::vector<CarouselItem> items;
    RECT monitor{};
    POINT position{};
    SIZE size{};
    // The big (middle) and the smaller pictures' sizes, the gap, where the
    // pictures' middle is.
    float scale = 1, bigW = 0, bigH = 0, smallW = 0, smallH = 0, gap = 0,
          centerY = 0;
    // Past the ends it goes around (with enough pictures to not repeat on
    // the screen).
    bool wrap = false;
    // The place of the middle picture (moving to `target`), as a position
    // in the (endless, if it wraps) row.
    double scroll = 0;
    int target = 0;
    double lastTick = 0, openStart = 0;
    bool timer = false;
    unsigned generation = 0;
    // The fan layout: card size, circle radius, where the middle card rests,
    // where the name goes under it.
    float fanW = 0, fanH = 0, fanRadius = 0, fanCenterY = 0, fanLabelY = 0;
    // Below 1 while opening (cards rise and spread out).
    float spread = 1;
    // 0 = strip ... 1 = fan; moves to mixTarget (smooth style change).
    double styleMix = 0, mixTarget = 0;
    // The slant style: 0 = not, 1 = fully (moves to slantTarget).
    double slantMix = 0, slantTarget = 0;
    // The coverflow style: 0 = not, 1 = fully (moves to cfTarget). The middle
    // card's size, its middle line (the horizon) and where the name goes.
    double cfMix = 0, cfTarget = 0;
    float cvW = 0, cvH = 0, cvCy = 0, cvLabelY = 0;
    // The deck style: 0 = not, 1 = fully (moves to dkTarget). The middle
    // card's size, its middle line and where the name goes.
    double dkMix = 0, dkTarget = 0;
    // The flow style: 0 = not, 1 = fully (moves to flTarget).
    double flMix = 0, flTarget = 0;
    float dkW = 0, dkH = 0, dkCy = 0, dkLabelY = 0;
    // How much the narrow-side layout leans: 1 = Slant, 0 = Cards.
    double leanMix = 0, leanTarget = 0;
    // The hexagon style: a honeycomb grid (pointy-top hexagons, odd rows
    // shifted by half a hexagon) over the whole monitor. `hex` is true while
    // the grid is what's shown (the window is then as big as the monitor).
    bool hex = false;
    // Flat-top honeycomb: vertical columns of `hexPerCol` hexagons, odd
    // columns shifted down by half a pitch. Picture i is in column
    // i / hexPerCol, slot i % hexPerCol.
    int hexCols = 1, hexPerCol = 3;
    float hexW = 0, hexH = 0, hexR = 0, hexPitchX = 0, hexPitchY = 0,
          hexTop = 0, hexLabelY = 0;
    double hexScroll = 0;        // the column that sits at the screen's middle
    double hexScrollTarget = 0;  // (smooth, inertial scroll moves to this)
    bool hexWheel = false;       // the last scroll came from the wheel
    double hexClock = 0;         // seconds since the grid appeared (pop-in stagger)
    bool hexDrag = false, hexDragMoved = false;
    float hexDragX = 0;
    double hexDragScroll = 0;
    std::vector<float> hexAmt;   // per picture: 0 dark ... 1 selected
    std::vector<float> hexShow;  // per picture: 0 empty slot ... 1 popped in
    ComPtr<ID2D1PathGeometry> hexGeom;
    POINT mouseLast{-1, -1};
    // The window of the other styles (the band), to come back to.
    SIZE bandSize{};
    POINT bandPosition{};
    // The hexagon grid's window is a band as wide as the monitor and as tall
    // as the grid and its texts; the dimmed desktop is another window behind.
    float hexBandH = 0;
    HWND dimHwnd = nullptr;
    int dimAlpha = 0;
    // The spiral style: circles on a double spiral, the selected one big in
    // the middle. `spScroll` glides to `target` like a spring.
    bool spiral = false, spWrap = false;
    double spScroll = 0, spVel = 0, spClock = 0;
    float spU = 0, spCx = 0, spCy = 0, spEx = 1, spBig = 0, spLabelY = 0;
    SIZE spSize{}, spThumb{};
    POINT spPosition{};
    // A change between the grid and the other styles (-1 none, 0 to the
    // band, 1 to the grid): it fades out, the window changes, it fades in.
    int switchTo = -1;
    double switchStart = 0;
    // Cards style: text formats and the file info of the middle card.
    ComPtr<IDWriteTextFormat> chipMono, chipSmall, chipSans;
    int infoIndex = -1;
    std::wstring infoDims, infoSize, infoName, infoType;
    // Slant style: width of a side card, and the gap between cards.
    float slantSmallW = 0, slantGap = 0;
    // Liquid style: 0 = not, 1 = fully (moves to lqTarget); the scroll's
    // speed (places / s, drives the bend and the RGB split); a clock for the wave.
    double lqMix = 0, lqTarget = 0, lqVel = 0, lqClock = 0;
    float lqW = 0, lqH = 0, lqCy = 0, lqLabelY = 0;  // Liquid's middle card + label
    double openLen = 0.16;
    SIZE thumbSize{};
    // Ambient backdrop: the dim window's own per-pixel-alpha canvas.
    bool ambOn = false, ambBusy = false;
    LayeredCanvas dimCanvas;
    int ambW = 0, ambH = 0, ambFrom = -1, ambTo = -1, ambDrawnTo = -2,
        ambAlpha = -1;
    double ambStart = 0;
    struct AmbEntry {
        int index = -1;
        double used = 0;
        ComPtr<ID2D1Bitmap> bmp;
    };
    AmbEntry ambCache[4];
    // Fisheye and Tunnel (full-screen): window size and place.
    bool fish = false, tun = false;
    SIZE fxSize{};
    POINT fxPosition{};
    // Fisheye grid.
    int fishCols = 4, fishRows = 1;
    float fishTW = 0, fishTH = 0, fishGap = 0, fishLabelY = 0, fishViewY = 0;
    SIZE fishBake{}, fishThumb{};
    double fishScroll = 0, fishVel = 0, fishScrollTarget = 0, fishClock = 0;
    double fishFc = 0, fishFr = 0, fishFcV = 0, fishFrV = 0;
    bool fishMouse = false, fishDrag = false, fishDragMoved = false;
    float fishDragY = 0;
    double fishDragScroll = 0;
    POINT fishMouseAt{-1, -1};
    std::vector<float> fishShow;                    // per picture: pop-in
    std::vector<float> fishX, fishW, fishY, fishH;  // last layout: cells
    // Depth tunnel.
    bool tunWrap = false;
    double tunScroll = 0, tunVel = 0, tunClock = 0;
    float tunVx = 0, tunVy = 0, tunWall = 0, tunFrameH = 0, tunLift = 0,
          tunHeroW = 0, tunHeroH = 0, tunWallHH = 0, tunLabelY = 0;
    SIZE tunBake{}, tunThumb{};
};

CarouselState g_car;
bool g_panelClassRegistered = false;
constexpr PCWSTR kPanelClass = L"FastWallpaperSwitcherPicker";
constexpr PCWSTR kMessageClass = L"FastWallpaperSwitcherMessages";
constexpr PCWSTR kDimClass = L"FastWallpaperSwitcherDim";
bool g_dimClassRegistered = false;

int CarCount() { return (int)g_car.paths.size(); }

bool CarValid(int v) {
    return g_car.wrap || (v >= 0 && v < CarCount());
}

int CarIndex(int v) {
    const int n = CarCount();
    return g_car.wrap ? ((v % n) + n) % n : v;
}

// 0 band, 1 hexagons, 2 spiral, 3 fisheye grid, 4 tunnel.
int CarMode() {
    const CarouselState& C = g_car;
    return C.hex ? 1 : C.spiral ? 2 : C.fish ? 3 : C.tun ? 4 : 0;
}

// A critically damped spring (no overshoot), a few substeps.
void SpringStep(double& x, double& v, double goal, double omega, double dt) {
    const int steps = 3;
    const double h = dt / steps;
    for (int k = 0; k < steps; k++) {
        v += (omega * omega * (goal - x) - 2.0 * omega * v) * h;
        x += v * h;
    }
}

bool SpringSettled(double x, double v, double goal, double eps = 0.0006,
                   double veps = 0.004) {
    return std::fabs(goal - x) < eps && std::fabs(v) < veps;
}

constexpr float kPi = 3.14159265f;
constexpr float kFanStepDeg = 5.0f;   // tilt per card
constexpr float kFanGapDeg = 4.0f;    // extra room around the middle card
constexpr float kFanSmall = 0.84f;    // size of the side cards
constexpr float kFanLift = 0.06f;     // how much the middle card rises
constexpr float kFanReach = 5.0f;     // cards fade out this many places away
constexpr float kFanRadiusK = 3.6f;   // arc radius, in card heights

// Where a card is: center, size, tilt (degrees, clockwise), how "in the
// middle" it is (1..0) and its opacity.
struct CardPose {
    float cx, cy, w, h, angle, closeness, alpha;
    float skew = 0;  // lean: horizontal shear (0 = none)
    float yaw = 0;   // turn about the vertical axis (radians, > 0: right edge farther)
};

// The flat row. `d`: places from the middle.
CardPose StripPose(float d) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float closeness = std::clamp(1 - ad, 0.0f, 1.0f);
    const float w = C.smallW + (C.bigW - C.smallW) * closeness;
    const float h = C.smallH + (C.bigH - C.smallH) * closeness;
    const float shift =
        (d < 0 ? -1.0f : 1.0f) * (C.bigW - C.smallW) / 2 * std::min(ad, 1.0f);
    const float cx = C.size.cx / 2.0f + d * (C.smallW + C.gap) + shift;
    return {cx, C.centerY, w, h, 0.0f, closeness, 1.0f};
}

// The fan: cards sit on a big circle below, each turned to point away from
// its center; the middle one is bigger, raised and in front.
CardPose FanPose(float d, float spread) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float sign = d < 0 ? -1.0f : 1.0f;
    const float linear = std::clamp(1 - ad, 0.0f, 1.0f);
    const float e = linear * linear * (3 - 2 * linear);  // smooth peak
    const float angle =
        (d * kFanStepDeg + sign * std::min(ad, 1.0f) * kFanGapDeg) * spread;
    const float rad = angle * kPi / 180;
    const float scale = kFanSmall + (1 - kFanSmall) * e;
    const float reach =
        C.wrap ? std::min(kFanReach, CarCount() / 2.0f) : kFanReach;
    CardPose pose;
    pose.cx = C.size.cx / 2.0f + C.fanRadius * std::sin(rad);
    pose.cy = C.fanCenterY + C.fanRadius * (1 - std::cos(rad)) -
              C.fanH * kFanLift * e + (1 - spread) * C.fanH * 0.5f;
    pose.w = C.fanW * scale;
    pose.h = C.fanH * scale;
    pose.angle = angle;
    pose.closeness = e;
    pose.alpha = std::clamp((reach - ad) / 1.2f, 0.0f, 1.0f);
    return pose;
}

// How much the slant style leans (0.35 is about 19 degrees).
constexpr float kSlantK = 0.35f;

// The slant: all cards the same height and leaning the same way; the middle
// one wide, the others narrow slices that fade out away from the middle.
CardPose SlantPose(float d) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float closeness = std::clamp(1 - ad, 0.0f, 1.0f);
    const float w = C.slantSmallW + (C.bigW - C.slantSmallW) * closeness;
    const float shift = (d < 0 ? -1.0f : 1.0f) *
                        (C.bigW - C.slantSmallW) / 2 * std::min(ad, 1.0f);
    const float cx = C.size.cx / 2.0f + d * (C.slantSmallW + C.slantGap) + shift;
    const float reach = C.wrap ? std::min(6.5f, CarCount() / 2.0f) : 6.5f;
    const float alpha = std::clamp((reach - ad) / 1.5f, 0.0f, 1.0f);
    return {cx,       C.centerY, w, C.bigH, 0.0f, closeness,
            alpha,    kSlantK * (float)C.leanMix};
}

// The strip or the slant (or in between while changing style).
CardPose BasePose(float d) {
    const float u = (float)g_car.slantMix;
    if (u <= 0.001f) {
        return StripPose(d);
    }
    const CardPose k = SlantPose(d);
    if (u >= 0.999f) {
        return k;
    }
    const CardPose s = StripPose(d);
    auto mix = [u](float a, float b) { return a + (b - a) * u; };
    return {mix(s.cx, k.cx),       mix(s.cy, k.cy),
            mix(s.w, k.w),         mix(s.h, k.h),
            mix(s.angle, k.angle), mix(s.closeness, k.closeness),
            mix(s.alpha, k.alpha), mix(s.skew, k.skew)};
}

// Coverflow: the middle card faces you, the others stand turned toward it,
// close together, like covers on a shelf.
constexpr float kCoverYaw = 1.05f;     // how far the side cards turn (about 60 deg)
constexpr float kCoverOffset = 0.74f;  // first side card's place (card widths)
constexpr float kCoverStep = 0.17f;    // spacing of the next ones (card widths)
constexpr float kCoverSide = 0.88f;    // side cards' height (share of the middle's)
constexpr float kCoverReach = 6.2f;    // cards fade out this many places away

CardPose CoverPose(float d) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float sign = d < 0 ? -1.0f : 1.0f;
    const float t = std::clamp(ad, 0.0f, 1.0f);
    const float e = t * t * (3 - 2 * t);
    // (Opening: the cards slide out a little while fading in.)
    const float off = C.cvW * (kCoverOffset * t +
                               kCoverStep * std::max(ad - 1.0f, 0.0f)) *
                      (0.4f + 0.6f * C.spread);
    const float reach =
        C.wrap ? std::min(kCoverReach, CarCount() / 2.0f) : kCoverReach;
    CardPose p;
    p.cx = C.size.cx / 2.0f + sign * off;
    p.w = C.cvW;
    p.h = C.cvH * (1.0f - (1.0f - kCoverSide) * e);
    p.cy = C.cvCy + (C.cvH - p.h) / 2;  // (all stand on the same floor)
    p.angle = 0.0f;
    p.closeness = std::clamp(1.0f - ad, 0.0f, 1.0f);
    p.alpha = std::clamp((reach - ad) / 1.8f, 0.0f, 1.0f);
    p.skew = 0.0f;
    p.yaw = -sign * kCoverYaw * e;  // (the outer edge comes toward you)
    return p;
}

// The pose now: the strip, the fan, or in between while changing style.
CardPose CarPoseNoCover(float d) {
    const float m = (float)g_car.styleMix;
    if (m <= 0.001f) {
        return BasePose(d);
    }
    const CardPose f = FanPose(d, g_car.spread);
    if (m >= 0.999f) {
        return f;
    }
    const CardPose s = BasePose(d);
    auto mix = [m](float a, float b) { return a + (b - a) * m; };
    return {mix(s.cx, f.cx),     mix(s.cy, f.cy),
            mix(s.w, f.w),       mix(s.h, f.h),
            mix(s.angle, f.angle), mix(s.closeness, f.closeness),
            mix(s.alpha, f.alpha), mix(s.skew, f.skew)};
}

// Deck: the middle card on top, the others stacked behind it on both sides,
// each smaller, a little tilted and darker; a sliver of each one shows.
constexpr float kDeckShrink = 0.14f;  // size lost per place
constexpr float kDeckPeek = 0.18f;    // how much of each card shows (card widths)
constexpr float kDeckTilt = 3.0f;     // degrees per place (up to 2 places)
constexpr float kDeckReach = 5.2f;    // cards fade out this many places away

CardPose DeckPose(float d) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float sign = d < 0 ? -1.0f : 1.0f;
    const float s = std::max(0.45f, 1.0f - kDeckShrink * ad);
    // Each card's outer edge sits `kDeckPeek` further out than the one in front
    // of it. (Opening: the stack spreads out from under the middle card.)
    const float off = C.dkW * (0.5f + kDeckPeek * ad - 0.5f * s) *
                      (0.3f + 0.7f * C.spread);
    const float reach =
        C.wrap ? std::min(kDeckReach, CarCount() / 2.0f) : kDeckReach;
    CardPose p;
    p.cx = C.size.cx / 2.0f + sign * off;
    p.w = C.dkW * s;
    p.h = C.dkH * s;
    p.cy = C.dkCy - C.dkH * 0.035f * std::min(ad, 3.0f);
    p.angle = sign * kDeckTilt * std::min(ad, 2.0f);
    p.closeness = std::clamp(1.0f - ad, 0.0f, 1.0f);
    p.alpha = std::clamp((reach - ad) / 1.6f, 0.0f, 1.0f);
    return p;
}

// The pose now, with the deck blended in while changing to / from it.
CardPose CarPoseWithDeck(float d) {
    const CardPose s = CarPoseNoCover(d);
    const float c = (float)g_car.dkMix;
    if (c <= 0.001f) {
        return s;
    }
    const CardPose v = DeckPose(d);
    if (c >= 0.999f) {
        return v;
    }
    auto mix = [c](float a, float b) { return a + (b - a) * c; };
    CardPose r;
    r.cx = mix(s.cx, v.cx);
    r.cy = mix(s.cy, v.cy);
    r.w = mix(s.w, v.w);
    r.h = mix(s.h, v.h);
    r.angle = mix(s.angle, v.angle);
    r.closeness = mix(s.closeness, v.closeness);
    r.alpha = mix(s.alpha, v.alpha);
    r.skew = mix(s.skew, v.skew);
    r.yaw = mix(s.yaw, v.yaw);
    return r;
}

// The pose now, with Coverflow blended in while changing to / from it.
CardPose CarPoseNoFlow(float d) {
    const CardPose s = CarPoseWithDeck(d);
    const float c = (float)g_car.cfMix;
    if (c <= 0.001f) {
        return s;
    }
    const CardPose v = CoverPose(d);
    if (c >= 0.999f) {
        return v;
    }
    auto mix = [c](float a, float b) { return a + (b - a) * c; };
    CardPose r;
    r.cx = mix(s.cx, v.cx);
    r.cy = mix(s.cy, v.cy);
    r.w = mix(s.w, v.w);
    r.h = mix(s.h, v.h);
    r.angle = mix(s.angle, v.angle);
    r.closeness = mix(s.closeness, v.closeness);
    r.alpha = mix(s.alpha, v.alpha);
    r.skew = mix(s.skew, v.skew);
    r.yaw = mix(s.yaw, v.yaw);
    return r;
}

// Flow: overlapping cards along a smooth S-curve. The left side rises, the
// right side dips, each card leans along the curve; the middle one is big,
// straight and on top.
constexpr float kFlowReach = 5.6f;   // cards fade out this many places away
constexpr float kFlowSwing = 0.20f;  // height of the curve (share of the card's height)
constexpr float kFlowTilt = 8.0f;    // degrees of lean at the far ends
constexpr float kFlowMin = 0.64f;    // size of the side cards (share of the middle's)

CardPose FlowPose(float d) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float sign = d < 0 ? -1.0f : 1.0f;
    const float t = std::clamp(1.0f - ad, 0.0f, 1.0f);
    const float e = t * t * (3 - 2 * t);  // smooth peak at the middle
    const float beyond = std::clamp(ad - 1.0f, 0.0f, 3.0f);
    const float s = (kFlowMin + (1.0f - kFlowMin) * e) * (1.0f - 0.04f * beyond);
    // (Opening: the ribbon spreads out from the middle while fading in.)
    const float open = 0.55f + 0.45f * C.spread;
    const float off = C.bigW * (0.52f * std::min(ad, 1.0f) +
                                0.34f * std::max(ad - 1.0f, 0.0f)) * open;
    const float curve = std::tanh(d * 0.9f);
    const float reach =
        C.wrap ? std::min(kFlowReach, CarCount() / 2.0f) : kFlowReach;
    CardPose p;
    p.w = C.bigW * s;
    p.h = C.bigH * s;
    p.cx = C.size.cx / 2.0f + sign * off;
    p.cy = C.centerY + curve * kFlowSwing * C.bigH * open;
    p.angle = curve * kFlowTilt * open;
    p.closeness = t;
    p.alpha = std::clamp((reach - ad) / 1.6f, 0.0f, 1.0f);
    p.skew = 0.0f;
    p.yaw = 0.0f;
    return p;
}

// The pose now: everything else, with Flow blended in while changing
// to / from it.
CardPose CarPoseNoLiquid(float d) {
    const CardPose s = CarPoseNoFlow(d);
    const float c = (float)g_car.flMix;
    if (c <= 0.001f) {
        return s;
    }
    const CardPose v = FlowPose(d);
    if (c >= 0.999f) {
        return v;
    }
    auto mix = [c](float a, float b) { return a + (b - a) * c; };
    CardPose r;
    r.cx = mix(s.cx, v.cx);
    r.cy = mix(s.cy, v.cy);
    r.w = mix(s.w, v.w);
    r.h = mix(s.h, v.h);
    r.angle = mix(s.angle, v.angle);
    r.closeness = mix(s.closeness, v.closeness);
    r.alpha = mix(s.alpha, v.alpha);
    r.skew = mix(s.skew, v.skew);
    r.yaw = mix(s.yaw, v.yaw);
    return r;
}

// Liquid: slanted accordion. The middle card is widescreen, the others are
// narrow slivers that get shorter away from the middle and sag a little.
constexpr float kLqSkew = 0.404f;      // lean (tan 22 deg)
constexpr float kLqReach = 4.4f;       // cards fade out this many places away
constexpr float kLqBend = 0.34f;       // single C-curve, max (share of the middle card's height)
constexpr float kLqVelMax = 14.0f;     // scroll speed (places / s) is clamped to this
constexpr float kLqSplit = 16.0f;      // RGB offset at full speed, px (before the curve / lens factors)
constexpr float kLqVelRef = 10.0f;     // speed (places / s) where the split reaches full strength
constexpr float kLqPinch = 0.30f;      // barrel pinch of the top / bottom edges (0 = off)
constexpr float kLqBorderVel = 9.0f;   // speed (places / s) where the border is fully faded
constexpr float kLqActiveK = 0.42f;    // middle card width (share of the monitor)
constexpr float kLqSliceK = 0.066f;    // side card width (share of the monitor)

CardPose LiquidPose(float d) {
    const CarouselState& C = g_car;
    const float ad = std::fabs(d);
    const float sign = d < 0 ? -1.0f : 1.0f;
    const float close = std::clamp(1.0f - ad, 0.0f, 1.0f);
    const float e = close * close * (3 - 2 * close);
    const float midW = C.lqW;
    const float midH = C.lqH;
    const float sliceW = std::round(C.size.cx * kLqSliceK);
    const float gap = std::round(3 * C.scale);
    const float w = sliceW + (midW - sliceW) * close;
    const float shift = sign * (midW - sliceW) / 2 * std::min(ad, 1.0f);
    const float beyond = std::min(std::max(ad - 1.0f, 0.0f), 4.0f);
    const float sag = std::min(ad, 5.0f);
    const float reach =
        C.wrap ? std::min(kLqReach, CarCount() / 2.0f) : kLqReach;
    CardPose p;
    p.cx = C.size.cx / 2.0f + d * (sliceW + gap) + shift;
    p.w = w;
    p.h = midH * (0.80f + 0.20f * e) * (1.0f - 0.04f * beyond);
    p.cy = C.lqCy + midH * 0.0045f * sag * sag;
    p.angle = 0.0f;
    p.closeness = close;
    p.alpha = std::clamp((reach - ad) / 1.5f, 0.0f, 1.0f);
    p.skew = kLqSkew;
    p.yaw = 0.0f;
    return p;
}

// The pose now, with Liquid blended in while changing to / from it.
CardPose CarPose(float d) {
    const CardPose s = CarPoseNoLiquid(d);
    const float c = (float)g_car.lqMix;
    if (c <= 0.001f) {
        return s;
    }
    const CardPose v = LiquidPose(d);
    if (c >= 0.999f) {
        return v;
    }
    auto mix = [c](float a, float b) { return a + (b - a) * c; };
    CardPose r;
    r.cx = mix(s.cx, v.cx);
    r.cy = mix(s.cy, v.cy);
    r.w = mix(s.w, v.w);
    r.h = mix(s.h, v.h);
    r.angle = mix(s.angle, v.angle);
    r.closeness = mix(s.closeness, v.closeness);
    r.alpha = mix(s.alpha, v.alpha);
    r.skew = mix(s.skew, v.skew);
    r.yaw = mix(s.yaw, v.yaw);
    return r;
}

// Whether a point is on the (turned) card.
bool CarPoseHit(const CardPose& p, float x, float y) {
    const float rad = p.angle * kPi / 180;
    const float dx = x - p.cx;
    const float dy = y - p.cy;
    const float ly = -dx * std::sin(rad) + dy * std::cos(rad);
    const float lx = dx * std::cos(rad) + dy * std::sin(rad) + p.skew * ly;
    // (A turned card is narrower on the screen.)
    return std::fabs(lx) <= p.w / 2 * std::max(0.2f, std::cos(p.yaw)) &&
           std::fabs(ly) <= p.h / 2;
}

DWORD WINAPI ThumbnailThreadProc(LPVOID parameter) {
    std::unique_ptr<ThumbJob> job((ThumbJob*)parameter);
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    for (const auto& [index, path] : job->items) {
        if (g_unloading || job->generation != g_carouselGeneration) {
            break;
        }
        HBITMAP bitmap = nullptr;
        ComPtr<IShellItemImageFactory> images;
        if (SUCCEEDED(SHCreateItemFromParsingName(
                path.c_str(), nullptr,
                IID_PPV_ARGS(images.GetAddressOf())))) {
            if (FAILED(images->GetImage(
                    job->size, SIIGBF_BIGGERSIZEOK | SIIGBF_THUMBNAILONLY,
                    &bitmap))) {
                bitmap = nullptr;
            }
        }
        auto* result = new ThumbResult{job->generation, index, bitmap};
        if (!PostMessage(job->hwnd, WM_APP_THUMB, 0, (LPARAM)result)) {
            if (bitmap) {
                DeleteObject(bitmap);
            }
            delete result;
            break;
        }
        if (job->slow) {
            Sleep(12);  // spread the pictures out so frames keep flowing
        }
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    g_threads--;
    return 0;
}

// ---------------------------------------------------------------------------
// The hexagon style: setup, scrolling and picture loading (drawing and mouse
// are further down).
// ---------------------------------------------------------------------------

constexpr float kHexSizeK = 0.18f;      // hexagon HEIGHT / the monitor's height
constexpr float kHexDim = 0.28f;        // how dark the unselected ones are (0 = none)
constexpr float kHexPop = 0.10f;        // how much the selected one grows
constexpr float kHexBackdrop = 0.42f;   // how much the desktop is dimmed
constexpr float kHexActiveCols = 2.3f;  // full-size zone: +- this many columns (~5 columns)

// Sizes of the honeycomb for this monitor, and the hexagon's shape.
void HexSetup() {
    CarouselState& C = g_car;
    const float monitorH = (float)(C.monitor.bottom - C.monitor.top);
    const float gap = std::round(8 * C.scale);
    C.hexPerCol = 3;
    C.hexH = std::round(monitorH * kHexSizeK);
    C.hexR = C.hexH / 1.7320508f;  // circumradius (flat-top: height = sqrt(3) R)
    C.hexW = 2 * C.hexR;
    C.hexPitchY = C.hexH + gap;
    // (Keeps the gap the same between neighbours in the next column too.)
    C.hexPitchX = 0.75f * C.hexW + gap * 0.8660254f;
    C.hexCols = std::max(1, (CarCount() + C.hexPerCol - 1) / C.hexPerCol);
    // 3 hexagons + the half-pitch stagger = 3.5 rows tall.
    const float gridH = C.hexH + 2.5f * C.hexPitchY;
    C.hexTop = std::round(20 * C.scale);
    C.hexLabelY = C.hexTop + gridH + std::round(14 * C.scale);
    C.hexBandH = std::ceil(C.hexLabelY + std::round(86 * C.scale));
    C.hexAmt.assign(CarCount(), 0.0f);
    C.hexShow.assign(CarCount(), 0.0f);

    // One flat-top hexagon around (0, 0); every card moves and scales it.
    C.hexGeom.Reset();
    ComPtr<ID2D1GeometrySink> sink;
    if (SUCCEEDED(C.d2d->CreatePathGeometry(C.hexGeom.GetAddressOf())) &&
        SUCCEEDED(C.hexGeom->Open(sink.GetAddressOf()))) {
        const float r = C.hexR;
        const float hh = C.hexH / 2;
        sink->BeginFigure(D2D1::Point2F(r, 0), D2D1_FIGURE_BEGIN_FILLED);
        const D2D1_POINT_2F pts[5] = {{r / 2, hh},
                                      {-r / 2, hh},
                                      {-r, 0},
                                      {-r / 2, -hh},
                                      {r / 2, -hh}};
        sink->AddLines(pts, 5);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
    }
}

// A picture cut to the hexagon once (soft edge): the grid then only draws
// ready bitmaps, which is much lighter than clipping every hexagon with a
// layer on every frame.
ComPtr<ID2D1Bitmap> HexBake(ID2D1RenderTarget* target,
                            IWICImagingFactory* wic,
                            IWICBitmapSource* source) {
    const CarouselState& C = g_car;
    const UINT W = (UINT)std::lround(C.hexW);
    const UINT H = (UINT)std::lround(C.hexH);
    UINT sw = 0, sh = 0;
    if (!target || !wic || !source || W < 4 || H < 4 ||
        FAILED(source->GetSize(&sw, &sh)) || !sw || !sh) {
        return nullptr;
    }
    // Cut to fill the hexagon's box.
    const double k = std::max((double)W / sw, (double)H / sh);
    const UINT coverW = std::max<UINT>(W, (UINT)std::ceil(sw * k));
    const UINT coverH = std::max<UINT>(H, (UINT)std::ceil(sh * k));
    const WICRect crop{(INT)((coverW - W) / 2), (INT)((coverH - H) / 2),
                       (INT)W, (INT)H};
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICBitmapClipper> clipper;
    if (FAILED(wic->CreateBitmapScaler(scaler.GetAddressOf())) ||
        FAILED(scaler->Initialize(source, coverW, coverH,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(wic->CreateBitmapClipper(clipper.GetAddressOf())) ||
        FAILED(clipper->Initialize(scaler.Get(), &crop))) {
        return nullptr;
    }
    std::vector<uint32_t> px((size_t)W * H);
    if (FAILED(clipper->CopyPixels(nullptr, W * 4, (UINT)(px.size() * 4),
                                   (BYTE*)px.data()))) {
        return nullptr;
    }
    // The hexagon's soft mask (flat top): distance to its nearest edge.
    const float R = W / 2.0f;   // circumradius
    const float hh = H / 2.0f;  // half height
    for (UINT y = 0; y < H; y++) {
        for (UINT x = 0; x < W; x++) {
            const float dx = std::fabs(x + 0.5f - R);
            const float dy = std::fabs(y + 0.5f - hh);
            const float d =
                std::min(hh - dy, (1.7320508f * (R - dx) - dy) / 2.0f);
            const float cover = std::clamp(d + 0.5f, 0.0f, 1.0f);
            if (cover >= 1.0f) {
                continue;
            }
            uint32_t& p = px[(size_t)y * W + x];
            auto scaleChannel = [&](int shift) {
                return (uint32_t)std::lround(((p >> shift) & 255) * cover)
                       << shift;
            };
            p = scaleChannel(24) | scaleChannel(16) | scaleChannel(8) |
                scaleChannel(0);
        }
    }
    ComPtr<ID2D1Bitmap> bitmap;
    target->CreateBitmap(
        D2D1::SizeU(W, H), px.data(), W * 4,
        D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f),
        bitmap.GetAddressOf());
    return bitmap;
}

// Puts the selected picture's column in the middle (smoothly; `center`: at
// once, for when the grid opens).
void HexReveal(bool center) {
    CarouselState& C = g_car;
    const double col = (double)(C.target / std::max(1, C.hexPerCol));
    C.hexScrollTarget = std::clamp(col, 0.0, (double)std::max(0, C.hexCols - 1));
    if (center) {
        C.hexScroll = C.hexScrollTarget;
    }
}

// The pictures of the columns around the middle, nearest first, are read on
// a thread; the far ones are let go.
void HexRequest() {
    CarouselState& C = g_car;
    const int n = CarCount();
    const int span = (int)std::ceil(C.size.cx / 2.0f / C.hexPitchX) + 2;
    const int midCol = (int)std::lround(C.hexScrollTarget);
    const int first = std::max(0, (midCol - span) * C.hexPerCol);
    const int last = std::min(n - 1, (midCol + span + 1) * C.hexPerCol - 1);
    std::vector<int> want;
    for (int i = first; i <= last; i++) {
        want.push_back(i);
    }
    const int midIndex = midCol * C.hexPerCol + 1;
    std::sort(want.begin(), want.end(), [&](int a, int b) {
        return std::abs(a - midIndex) < std::abs(b - midIndex);
    });
    const SIZE hexThumb{(LONG)std::ceil(C.hexW * 1.25f),
                        (LONG)std::ceil(C.hexH * 1.25f)};
    auto* job = new ThumbJob{C.hwnd, {}, hexThumb, C.generation, true};
    for (int i : want) {
        if (C.items[i].state == 0) {
            C.items[i].state = 1;
            job->items.push_back({i, C.paths[i]});
        }
    }
    for (int i = 0; i < n; i++) {
        if (C.items[i].state == 2 &&
            (i < first - 3 * C.hexPerCol || i > last + 3 * C.hexPerCol)) {
            C.items[i].bitmap.Reset();
            C.items[i].hexBmp.Reset();
            C.items[i].state = 0;
        }
    }
    if (job->items.empty()) {
        delete job;
        return;
    }
    g_threads++;
    HANDLE thread = CreateThread(nullptr, 0, ThumbnailThreadProc, job, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_threads--;
        for (const auto& item : job->items) {
            C.items[item.first].state = 0;
        }
        delete job;
    }
}

// ---------------------------------------------------------------------------
// The spiral style
// ---------------------------------------------------------------------------

constexpr int kSpiralN = 8;
constexpr float kSpiralStep = 1.2566f;   // turn per place along an arm (72 deg)
constexpr float kSpiralTheta0 = -0.35f;  // where the first place points
constexpr float kSpiralReach = 7.0f;     // places from the middle that show
// Distance from the middle and diameter (units of the spiral's size) at
// 0, 1, 2 ... 7 places from the selected picture. Tune the look here.
constexpr float kSpiralR[kSpiralN] = {0.0f,  0.31f,  0.335f, 0.37f,
                                      0.39f, 0.405f, 0.415f, 0.42f};
constexpr float kSpiralD[kSpiralN] = {0.36f,  0.20f,  0.145f, 0.11f,
                                      0.085f, 0.065f, 0.05f,  0.04f};

struct SpiralSpot {
    float x, y, d, alpha, close;
};

// Smooth (Catmull-Rom) value of a table at a fractional place. `ext` is how
// the table continues below 0: +1 mirrored (sizes), -1 flipped (distances).
float SpiralTable(const float* t, float a, float ext) {
    a = std::clamp(a, 0.0f, (float)(kSpiralN - 1) - 0.001f);
    const int i = (int)a;
    const float f = a - (float)i;
    const float p0 = i > 0 ? t[i - 1] : ext * t[1];
    const float p1 = t[i];
    const float p2 = t[i + 1];
    const float p3 = i + 2 < kSpiralN ? t[i + 2] : t[kSpiralN - 1];
    return 0.5f * (2 * p1 + (p2 - p0) * f +
                   (2 * p0 - 5 * p1 + 4 * p2 - p3) * f * f +
                   (3 * p1 - p0 - 3 * p2 + p3) * f * f * f);
}

bool SpiralValid(int v) {
    return g_car.spWrap || (v >= 0 && v < CarCount());
}

int SpiralIndex(int v) {
    const int n = CarCount();
    return g_car.spWrap ? ((v % n) + n) % n : v;
}

// Where the picture `u` places from the selected one goes (u is fractional
// while it glides). Pictures after the selected one are on one arm, the ones
// before it on the other (the same arm turned half way round), so a picture
// passes straight through the middle when it changes arm. `spClock` drives
// the bloom of the opening: nearer pictures come first.
SpiralSpot SpiralPlace(float u) {
    const CarouselState& C = g_car;
    const float a = std::fabs(u);
    const float side = u < 0 ? -1.0f : 1.0f;
    const float q =
        std::clamp(((float)C.spClock - 0.045f * a) / 0.6f, 0.0f, 1.0f);
    const float e = 1.0f - (1.0f - q) * (1.0f - q) * (1.0f - q);
    const float theta =
        kSpiralTheta0 + a * kSpiralStep - (1.0f - e) * 1.5f;
    const float r = SpiralTable(kSpiralR, a, -1.0f) * (0.2f + 0.8f * e);
    const float d = SpiralTable(kSpiralD, a, 1.0f) * (0.5f + 0.5f * e);
    SpiralSpot s;
    s.x = C.spCx + side * r * std::cos(theta) * C.spEx * C.spU;
    s.y = C.spCy + side * r * std::sin(theta) * C.spU;
    s.d = d * C.spU;
    s.alpha = std::clamp((kSpiralReach - a) / 2.5f, 0.0f, 1.0f) * e;
    const float c = std::clamp(1.0f - a, 0.0f, 1.0f);
    s.close = c * c * (3 - 2 * c);
    return s;
}

// Sizes for this monitor. The window is as tall as the monitor and a bit
// narrower than it (cheaper to draw); the dimmed desktop is behind it.
void SpiralSetup() {
    CarouselState& C = g_car;
    const float monitorW = (float)(C.monitor.right - C.monitor.left);
    const float monitorH = (float)(C.monitor.bottom - C.monitor.top);
    C.spU = std::round(monitorH * 0.92f);
    C.spEx = std::min(1.55f, 0.9f * monitorW / monitorH);
    C.spBig = std::round(kSpiralD[0] * C.spU);
    const float width = std::min(monitorW, std::round(monitorH * 1.5f));
    C.spSize = {(LONG)width, (LONG)monitorH};
    C.spPosition = {C.monitor.left + (LONG)((monitorW - width) / 2),
                    C.monitor.top};
    C.spCx = width / 2;
    C.spCy = std::round(monitorH * 0.455f);
    C.spLabelY = std::round(monitorH * 0.885f);
    C.spWrap = CarCount() >= 17;
    const float box = C.spBig * 1.25f;
    C.spThumb = {(LONG)std::ceil(box * monitorW / monitorH),
                 (LONG)std::ceil(box)};
}

// A picture cut to a circle of diameter D once (soft edge), like HexBake.
ComPtr<ID2D1Bitmap> SpiralBake(ID2D1RenderTarget* target,
                               IWICImagingFactory* wic,
                               IWICBitmapSource* source,
                               UINT D) {
    UINT sw = 0, sh = 0;
    if (!target || !wic || !source || D < 8 ||
        FAILED(source->GetSize(&sw, &sh)) || !sw || !sh) {
        return nullptr;
    }
    const double k = std::max((double)D / sw, (double)D / sh);
    const UINT coverW = std::max<UINT>(D, (UINT)std::ceil(sw * k));
    const UINT coverH = std::max<UINT>(D, (UINT)std::ceil(sh * k));
    const WICRect crop{(INT)((coverW - D) / 2), (INT)((coverH - D) / 2),
                       (INT)D, (INT)D};
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICBitmapClipper> clipper;
    if (FAILED(wic->CreateBitmapScaler(scaler.GetAddressOf())) ||
        FAILED(scaler->Initialize(source, coverW, coverH,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(wic->CreateBitmapClipper(clipper.GetAddressOf())) ||
        FAILED(clipper->Initialize(scaler.Get(), &crop))) {
        return nullptr;
    }
    std::vector<uint32_t> px((size_t)D * D);
    if (FAILED(clipper->CopyPixels(nullptr, D * 4, (UINT)(px.size() * 4),
                                   (BYTE*)px.data()))) {
        return nullptr;
    }
    const float R = D / 2.0f;
    const float inner = (R - 1.0f) * (R - 1.0f);
    for (UINT y = 0; y < D; y++) {
        for (UINT x = 0; x < D; x++) {
            const float dx = x + 0.5f - R;
            const float dy = y + 0.5f - R;
            const float d2 = dx * dx + dy * dy;
            if (d2 <= inner) {
                continue;  // (well inside: untouched)
            }
            uint32_t& p = px[(size_t)y * D + x];
            const float cover =
                std::clamp(R - std::sqrt(d2) + 0.5f, 0.0f, 1.0f);
            if (cover <= 0.0f) {
                p = 0;
                continue;
            }
            auto scaleChannel = [&](int shift) {
                return (uint32_t)std::lround(((p >> shift) & 255) * cover)
                       << shift;
            };
            p = scaleChannel(24) | scaleChannel(16) | scaleChannel(8) |
                scaleChannel(0);
        }
    }
    ComPtr<ID2D1Bitmap> bitmap;
    target->CreateBitmap(
        D2D1::SizeU(D, D), px.data(), D * 4,
        D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f),
        bitmap.GetAddressOf());
    return bitmap;
}

// The pictures around the selected one (nearest first) are read on a thread;
// the far ones are let go.
void SpiralRequest() {
    CarouselState& C = g_car;
    const int n = CarCount();
    const int center = CarIndex(C.target);
    auto* job = new ThumbJob{C.hwnd, {}, C.spThumb, C.generation, true};
    for (int k = 0; k <= 8; k++) {
        for (int sign : {1, -1}) {
            if (k == 0 && sign == -1) {
                continue;
            }
            const int v = C.target + sign * k;
            if (!SpiralValid(v)) {
                continue;
            }
            const int i = SpiralIndex(v);
            if (C.items[i].state == 0) {
                C.items[i].state = 1;
                job->items.push_back({i, C.paths[i]});
            }
        }
    }
    for (int i = 0; i < n; i++) {
        int distance = std::abs(i - center);
        if (C.spWrap) {
            distance = std::min(distance, n - distance);
        }
        if (C.items[i].state == 2 && distance > 16) {
            C.items[i].spBmp.Reset();
            C.items[i].spMip.Reset();
            C.items[i].state = 0;
        }
    }
    if (job->items.empty()) {
        delete job;
        return;
    }
    g_threads++;
    HANDLE thread = CreateThread(nullptr, 0, ThumbnailThreadProc, job, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_threads--;
        for (const auto& item : job->items) {
            C.items[item.first].state = 0;
        }
        delete job;
    }
}
// ---------------------------------------------------------------------------
// Fisheye grid + depth tunnel: sizes, baking, picture requests
// ---------------------------------------------------------------------------

void CarRequest();
void CarKick();

constexpr float kFishPeak = 1.38f;   // size of the tile at the focus
constexpr float kFishRadius = 1.7f;  // reach of the swelling, in tiles
constexpr float kFishRows = 6.0f;    // rows that fit in view, unmagnified
constexpr float kFishAspect = 1.6f;  // tile width / height

constexpr float kTunK = 0.55f;      // perspective: scale = 1 / (1 + k * depth)
constexpr float kTunSpan = 1.3f;    // a frame's length along the wall (places)
constexpr float kTunReach = 10.0f;  // frames ahead that show
constexpr int kTunSlices = 8;       // slices of a skewed frame

float FishGauss(float d) {
    const float sigma = kFishRadius * 0.5f;
    return std::exp(-d * d / (2 * sigma * sigma));
}

// A picture cut to a rounded rectangle of W x H (soft edge), baked once.
ComPtr<ID2D1Bitmap> RoundBake(ID2D1RenderTarget* target,
                              IWICImagingFactory* wic,
                              IWICBitmapSource* source,
                              UINT W,
                              UINT H,
                              float radius) {
    UINT sw = 0, sh = 0;
    if (!target || !wic || !source || W < 4 || H < 4 ||
        FAILED(source->GetSize(&sw, &sh)) || !sw || !sh) {
        return nullptr;
    }
    const double k = std::max((double)W / sw, (double)H / sh);
    const UINT coverW = std::max<UINT>(W, (UINT)std::ceil(sw * k));
    const UINT coverH = std::max<UINT>(H, (UINT)std::ceil(sh * k));
    const WICRect crop{(INT)((coverW - W) / 2), (INT)((coverH - H) / 2),
                       (INT)W, (INT)H};
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICBitmapClipper> clipper;
    if (FAILED(wic->CreateBitmapScaler(scaler.GetAddressOf())) ||
        FAILED(scaler->Initialize(source, coverW, coverH,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(wic->CreateBitmapClipper(clipper.GetAddressOf())) ||
        FAILED(clipper->Initialize(scaler.Get(), &crop))) {
        return nullptr;
    }
    std::vector<uint32_t> px((size_t)W * H);
    if (FAILED(clipper->CopyPixels(nullptr, W * 4, (UINT)(px.size() * 4),
                                   (BYTE*)px.data()))) {
        return nullptr;
    }
    const float hw = W / 2.0f, hh = H / 2.0f;
    radius = std::min(radius, std::min(hw, hh));
    for (UINT y = 0; y < H; y++) {
        for (UINT x = 0; x < W; x++) {
            const float qx = std::fabs(x + 0.5f - hw) - (hw - radius);
            const float qy = std::fabs(y + 0.5f - hh) - (hh - radius);
            if (qx <= 0.0f && qy <= 0.0f) {
                continue;
            }
            const float dist =
                std::hypot(std::max(qx, 0.0f), std::max(qy, 0.0f)) +
                std::min(std::max(qx, qy), 0.0f) - radius;
            const float cover = std::clamp(0.5f - dist, 0.0f, 1.0f);
            if (cover >= 1.0f) {
                continue;
            }
            uint32_t& p = px[(size_t)y * W + x];
            auto scaleChannel = [&](int shift) {
                return (uint32_t)std::lround(((p >> shift) & 255) * cover)
                       << shift;
            };
            p = scaleChannel(24) | scaleChannel(16) | scaleChannel(8) |
                scaleChannel(0);
        }
    }
    ComPtr<ID2D1Bitmap> bitmap;
    target->CreateBitmap(
        D2D1::SizeU(W, H), px.data(), W * 4,
        D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f),
        bitmap.GetAddressOf());
    return bitmap;
}

void FishSetup() {
    CarouselState& C = g_car;
    const float monitorW = (float)(C.monitor.right - C.monitor.left);
    const float monitorH = (float)(C.monitor.bottom - C.monitor.top);
    const float s = C.scale;
    const float height = std::round(monitorH * 0.92f);
    C.fxSize = {(LONG)monitorW, (LONG)height};
    C.fxPosition = {C.monitor.left,
                    C.monitor.top + (LONG)((monitorH - height) / 2)};
    C.fishGap = std::round(10 * s);
    const float labelH = std::round(92 * s);
    const float area = height - labelH - std::round(24 * s);
    const float sigma = kFishRadius * 0.5f;
    const float extra = (kFishPeak - 1.0f) * sigma * 2.5066f;  // extra tiles
    C.fishTH = std::floor((area - 4 * C.fishGap) / kFishRows);
    C.fishTW = std::floor(C.fishTH * kFishAspect);
    const int cols = (int)std::floor(
        (monitorW - 2 * std::round(24 * s) + C.fishGap) /
            (C.fishTW + C.fishGap) -
        extra);
    C.fishCols = std::clamp(cols, 2, 12);
    C.fishRows = std::max(1, (CarCount() + C.fishCols - 1) / C.fishCols);
    C.fishViewY = std::round(12 * s + area * 0.5f);
    C.fishLabelY = height - labelH;
    C.fishBake = {(LONG)std::ceil(C.fishTW * kFishPeak),
                  (LONG)std::ceil(C.fishTH * kFishPeak)};
    C.fishThumb = C.fishBake;
    C.fishShow.assign(CarCount(), 0.0f);
    C.fishX.assign(C.fishCols, 0.0f);
    C.fishW.assign(C.fishCols, 0.0f);
    C.fishY.assign(C.fishRows, 0.0f);
    C.fishH.assign(C.fishRows, 0.0f);
}

// Cell sizes and places for the current focus and scroll. A column's width
// follows its nearest tile to the focus row, a row's height its nearest to
// the focus column, so tiles (centered in their cells) never overlap.
void FishLayout() {
    CarouselState& C = g_car;
    const int cols = C.fishCols, rows = C.fishRows;
    if ((int)C.fishX.size() != cols || (int)C.fishY.size() != rows) {
        return;
    }
    const double fc = C.fishFc, fr = C.fishFr;
    const float cMin = (float)std::fabs(
        fc - std::clamp(std::round(fc), 0.0, (double)(cols - 1)));
    const float rMin = (float)std::fabs(
        fr - std::clamp(std::round(fr), 0.0, (double)(rows - 1)));
    const float peak = kFishPeak - 1.0f;
    float totalW = 0;
    for (int c = 0; c < cols; c++) {
        C.fishW[c] = C.fishTW * (1.0f + peak * FishGauss((float)(c - fc)) *
                                            FishGauss(rMin));
        totalW += C.fishW[c] + C.fishGap;
    }
    totalW -= C.fishGap;
    float x = (C.fxSize.cx - totalW) / 2;
    for (int c = 0; c < cols; c++) {
        C.fishX[c] = x;
        x += C.fishW[c] + C.fishGap;
    }
    float y = 0;
    for (int r = 0; r < rows; r++) {
        C.fishH[r] = C.fishTH * (1.0f + peak * FishGauss((float)(r - fr)) *
                                            FishGauss(cMin));
        C.fishY[r] = y;
        y += C.fishH[r] + C.fishGap;
    }
    const double sc = std::clamp(C.fishScroll, 0.0, (double)(rows - 1));
    const int r0 = (int)std::floor(sc);
    const int r1 = std::min(rows - 1, r0 + 1);
    const float t = (float)(sc - r0);
    const float c0 = C.fishY[r0] + C.fishH[r0] / 2;
    const float c1 = C.fishY[r1] + C.fishH[r1] / 2;
    const float anchor = c0 + (c1 - c0) * t;
    for (int r = 0; r < rows; r++) {
        C.fishY[r] += C.fishViewY - anchor;
    }
}

// The picture whose cell is under a point (or -1).
int FishAt(float x, float y) {
    const CarouselState& C = g_car;
    if ((int)C.fishX.size() != C.fishCols ||
        (int)C.fishY.size() != C.fishRows) {
        return -1;
    }
    const float half = C.fishGap / 2;
    int c = -1, r = -1;
    for (int i = 0; i < C.fishCols; i++) {
        if (x >= C.fishX[i] - half && x < C.fishX[i] + C.fishW[i] + half) {
            c = i;
            break;
        }
    }
    for (int i = 0; i < C.fishRows; i++) {
        if (y >= C.fishY[i] - half && y < C.fishY[i] + C.fishH[i] + half) {
            r = i;
            break;
        }
    }
    if (c < 0 || r < 0) {
        return -1;
    }
    const int index = r * C.fishCols + c;
    return index < CarCount() ? index : -1;
}

// Where the swelling wants to be: under the mouse (read from the last
// layout), or on the selected tile when the keyboard is used.
void FishFocusGoal(double* gc, double* gr) {
    const CarouselState& C = g_car;
    const int cols = C.fishCols, rows = C.fishRows;
    *gc = C.target % cols;
    *gr = C.target / cols;
    if (!C.fishMouse || C.fishMouseAt.x < 0 || (int)C.fishX.size() != cols ||
        (int)C.fishY.size() != rows) {
        return;
    }
    const float mx = (float)C.fishMouseAt.x, my = (float)C.fishMouseAt.y;
    const float half = C.fishGap / 2;
    double fc = mx < C.fishX[0] ? 0.0 : (double)(cols - 1);
    for (int c = 0; c < cols; c++) {
        if (mx >= C.fishX[c] - half && mx < C.fishX[c] + C.fishW[c] + half) {
            fc = c + (mx - (C.fishX[c] + C.fishW[c] / 2)) /
                         (C.fishW[c] + C.fishGap);
            break;
        }
    }
    double fr = my < C.fishY[0] ? 0.0 : (double)(rows - 1);
    for (int r = 0; r < rows; r++) {
        if (my >= C.fishY[r] - half && my < C.fishY[r] + C.fishH[r] + half) {
            fr = r + (my - (C.fishY[r] + C.fishH[r] / 2)) /
                         (C.fishH[r] + C.fishGap);
            break;
        }
    }
    // Snap to whole tiles: the swelling glides from tile to tile (the spring
    // smooths it) instead of wobbling with every pixel of the mouse.
    double snapC = std::clamp(std::round(fc), 0.0, (double)(cols - 1));
    const double snapR = std::clamp(std::round(fr), 0.0, (double)(rows - 1));
    if ((int)snapR * cols + (int)snapC >= CarCount()) {
        snapC = (double)((CarCount() - 1) % cols);  // (the last row may be short)
    }
    *gc = snapC;
    *gr = snapR;
}

void FishSelect(int index, bool reveal) {
    CarouselState& C = g_car;
    index = std::clamp(index, 0, CarCount() - 1);
    C.target = index;
    C.scroll = index;
    if (reveal) {
        C.fishScrollTarget =
            std::clamp(index / C.fishCols, 0, C.fishRows - 1);
    }
    CarRequest();
    CarKick();
}

// The pictures of the rows around the middle, nearest first, are read on a
// thread; the far ones are let go.
void FishRequest() {
    CarouselState& C = g_car;
    const int n = CarCount();
    const int midRow = (int)std::lround(C.fishScrollTarget);
    const int first = std::max(0, (midRow - 5) * C.fishCols);
    const int last = std::min(n - 1, (midRow + 6) * C.fishCols - 1);
    std::vector<int> want;
    for (int i = first; i <= last; i++) {
        want.push_back(i);
    }
    const int midIndex = midRow * C.fishCols + C.fishCols / 2;
    std::sort(want.begin(), want.end(), [&](int a, int b) {
        return std::abs(a - midIndex) < std::abs(b - midIndex);
    });
    auto* job = new ThumbJob{C.hwnd, {}, C.fishThumb, C.generation, true};
    for (int i : want) {
        if (C.items[i].state == 0) {
            C.items[i].state = 1;
            job->items.push_back({i, C.paths[i]});
        }
    }
    for (int i = 0; i < n; i++) {
        if (C.items[i].state == 2 &&
            (i < first - 3 * C.fishCols || i > last + 3 * C.fishCols)) {
            C.items[i].fishBmp.Reset();
            C.items[i].fishMip.Reset();
            C.items[i].state = 0;
        }
    }
    if (job->items.empty()) {
        delete job;
        return;
    }
    g_threads++;
    HANDLE thread =
        CreateThread(nullptr, 0, ThumbnailThreadProc, job, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_threads--;
        for (const auto& item : job->items) {
            C.items[item.first].state = 0;
        }
        delete job;
    }
}

bool TunValid(int v) {
    return g_car.tunWrap || (v >= 0 && v < CarCount());
}

int TunIndex(int v) {
    const int n = CarCount();
    return g_car.tunWrap ? ((v % n) + n) % n : v;
}

void TunnelSetup() {
    CarouselState& C = g_car;
    const float monitorW = (float)(C.monitor.right - C.monitor.left);
    const float monitorH = (float)(C.monitor.bottom - C.monitor.top);
    const float height = std::round(monitorH * 0.92f);
    C.fxSize = {(LONG)monitorW, (LONG)height};
    C.fxPosition = {C.monitor.left,
                    C.monitor.top + (LONG)((monitorH - height) / 2)};
    C.tunHeroH = std::round(std::min(height * 0.50f, monitorW * 0.44f / 1.6f));
    C.tunHeroW = std::round(C.tunHeroH * 1.6f);
    C.tunVx = monitorW * 0.5f;
    C.tunVy = std::round(height * 0.42f);
    C.tunWall = std::round(monitorW * 0.58f);
    C.tunFrameH = std::round(height * 0.40f);
    C.tunLift = std::round(height * 0.04f);
    C.tunWallHH = std::round(height * 0.62f);
    C.tunLabelY = C.tunVy + C.tunHeroH / 2 + std::round(18 * C.scale);
    C.tunWrap = CarCount() >= 17;
    C.tunBake = {(LONG)C.tunHeroW, (LONG)C.tunHeroH};
    C.tunThumb = {(LONG)std::ceil(C.tunHeroW * 1.1f),
                  (LONG)std::ceil(C.tunHeroH * 1.1f)};
}

// The frame `v` as a quad: vertical left / right edges at xL / xR, the top
// edge at tL..tR and the bottom edge at bL..bR.
struct TunQuad {
    float xL = 0, xR = 0, tL = 0, bL = 0, tR = 0, bR = 0;
    float alpha = 1, dark = 0, hero = 0;
};

// The camera's fly-in at the opening (places, goes 6 -> 0).
float TunFly() {
    const float q = 1.0f - (float)std::clamp(g_car.tunClock / 0.9, 0.0, 1.0);
    return 6.0f * q * q * q;
}

// d: places from the camera (0 = the selected one, > 0 further on).
bool TunnelPose(int v, float d, TunQuad* q) {
    const CarouselState& C = g_car;
    if (d < -1.4f || d > kTunReach) {
        return false;
    }
    const float side = (((v % 2) + 2) % 2) ? 1.0f : -1.0f;
    const float half = kTunSpan * 0.5f;
    auto P = [](float z) { return 1.0f / (1.0f + kTunK * std::max(z, -1.5f)); };
    const float pn = P(d - half), pf = P(d + half);
    const float xn = C.tunVx + side * C.tunWall * pn;
    const float xf = C.tunVx + side * C.tunWall * pf;
    const float cn = C.tunVy - C.tunLift * pn, cf = C.tunVy - C.tunLift * pf;
    const float hn = C.tunFrameH * pn * 0.5f, hf = C.tunFrameH * pf * 0.5f;
    TunQuad w;
    if (side < 0) {
        w.xL = xn; w.xR = xf;
        w.tL = cn - hn; w.bL = cn + hn; w.tR = cf - hf; w.bR = cf + hf;
    } else {
        w.xL = xf; w.xR = xn;
        w.tL = cf - hf; w.bL = cf + hf; w.tR = cn - hn; w.bR = cn + hn;
    }
    // Near the camera it turns to face it (the selected one: straight-on).
    const float hw = C.tunHeroW * 0.5f, hh = C.tunHeroH * 0.5f;
    const float t = std::clamp(1.0f - std::fabs(d), 0.0f, 1.0f);
    const float k = t * t * (3 - 2 * t);
    auto mix = [k](float a, float b) { return a + (b - a) * k; };
    q->xL = mix(w.xL, C.tunVx - hw);
    q->xR = mix(w.xR, C.tunVx + hw);
    q->tL = mix(w.tL, C.tunVy - hh);
    q->bL = mix(w.bL, C.tunVy + hh);
    q->tR = mix(w.tR, C.tunVy - hh);
    q->bR = mix(w.bR, C.tunVy + hh);
    const float fog = std::clamp(d / kTunReach, 0.0f, 1.0f);
    q->hero = k;
    q->dark = 0.62f * std::pow(fog, 0.8f) * (1.0f - k);
    q->alpha = d < 0 ? std::clamp(1.0f + d, 0.0f, 1.0f)
                     : std::clamp((kTunReach - d) / 2.5f, 0.0f, 1.0f);
    return q->alpha > 0.01f;
}

// A point on frame `v` (`d` places from the camera), `u` of the way across it
// (0 = its left edge, 1 = its right edge), with true perspective: its x on
// the screen and the top / bottom edge there. Matches TunnelPose at both ends.
void TunnelEdge(int v,
                float d,
                float u,
                float hero,
                float* x,
                float* top,
                float* bottom) {
    const CarouselState& C = g_car;
    const float side = (((v % 2) + 2) % 2) ? 1.0f : -1.0f;
    const float half = kTunSpan * 0.5f;
    const float z = side < 0 ? (d - half) + u * kTunSpan
                             : (d + half) - u * kTunSpan;
    const float P = 1.0f / (1.0f + kTunK * std::max(z, -1.5f));
    const float wx = C.tunVx + side * C.tunWall * P;
    const float cy = C.tunVy - C.tunLift * P;
    const float hh = C.tunFrameH * P * 0.5f;
    const float hw = C.tunHeroW * 0.5f, hhh = C.tunHeroH * 0.5f;
    const float hx = C.tunVx - hw + u * 2 * hw;
    *x = wx + (hx - wx) * hero;
    *top = (cy - hh) + ((C.tunVy - hhh) - (cy - hh)) * hero;
    *bottom = (cy + hh) + ((C.tunVy + hhh) - (cy + hh)) * hero;
}

// The frame under a point (the nearest to the camera): its place in the row.
bool TunnelHit(float x, float y, int* hit) {
    const CarouselState& C = g_car;
    const float fly = TunFly();
    const int base = (int)std::floor(C.tunScroll);
    float best = 1e9f;
    bool found = false;
    for (int v = base - 2; v <= base + (int)kTunReach + 2; v++) {
        if (!TunValid(v)) {
            continue;
        }
        const float dd = (float)(v - C.tunScroll);
        const float d = dd + (dd >= -0.001f ? fly : 0.0f);
        TunQuad q;
        if (!TunnelPose(v, d, &q) || q.alpha < 0.3f || x < q.xL ||
            x > q.xR || q.xR - q.xL < 1.0f) {
            continue;
        }
        const float u = (x - q.xL) / (q.xR - q.xL);
        const float top = q.tL + (q.tR - q.tL) * u;
        const float bottom = q.bL + (q.bR - q.bL) * u;
        if (y >= top && y <= bottom && std::fabs(d) < best) {
            best = std::fabs(d);
            *hit = v;
            found = true;
        }
    }
    return found;
}

// The frames ahead of the camera (and two behind) are read on a thread.
void TunnelRequest() {
    CarouselState& C = g_car;
    const int n = CarCount();
    const int center = CarIndex(C.target);
    auto* job = new ThumbJob{C.hwnd, {}, C.tunThumb, C.generation, true};
    for (int k = 0; k <= 12; k++) {
        for (int sign : {1, -1}) {
            if ((k == 0 && sign == -1) || (sign == -1 && k > 2)) {
                continue;
            }
            const int v = C.target + sign * k;
            if (!TunValid(v)) {
                continue;
            }
            const int i = TunIndex(v);
            if (C.items[i].state == 0) {
                C.items[i].state = 1;
                job->items.push_back({i, C.paths[i]});
            }
        }
    }
    for (int i = 0; i < n; i++) {
        int distance = std::abs(i - center);
        if (C.tunWrap) {
            distance = std::min(distance, n - distance);
        }
        if (C.items[i].state == 2 && distance > 20) {
            C.items[i].tunBmp.Reset();
            C.items[i].tunMip.Reset();
            C.items[i].state = 0;
        }
    }
    if (job->items.empty()) {
        delete job;
        return;
    }
    g_threads++;
    HANDLE thread =
        CreateThread(nullptr, 0, ThumbnailThreadProc, job, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_threads--;
        for (const auto& item : job->items) {
            C.items[item.first].state = 0;
        }
        delete job;
    }
}
// the far ones are let go.
void CarRequest() {
    CarouselState& C = g_car;
    if (C.hex) {
        HexRequest();
        return;
    }
    if (C.spiral) {
        SpiralRequest();
        return;
    }
    if (C.fish) {
        FishRequest();
        return;
    }
    if (C.tun) {
        TunnelRequest();
        return;
    }
    const int n = CarCount();
    const int center = CarIndex(C.target);
    auto* job = new ThumbJob{C.hwnd,
                             {},
                             C.thumbSize,
                             C.generation};
    for (int k = 0; k <= 6; k++) {
        for (int sign : {1, -1}) {
            if (k == 0 && sign == -1) {
                continue;
            }
            const int v = C.target + sign * k;
            if (!CarValid(v)) {
                continue;
            }
            const int i = CarIndex(v);
            if (C.items[i].state == 0 ||
                (C.lqTarget > 0.5 && k <= 3 && C.items[i].state == 2 &&
                 C.items[i].bitmap && !C.items[i].lqR)) {
                C.items[i].state = 1;
                job->items.push_back({i, C.paths[i]});
            }
        }
    }
    for (int i = 0; i < n; i++) {
        int distance = std::abs(i - center);
        if (C.wrap) {
            distance = std::min(distance, n - distance);
        }
        if (distance > 4) {  // colour copies are only needed near the middle
            C.items[i].lqR.Reset();
            C.items[i].lqG.Reset();
            C.items[i].lqB.Reset();
        }
        if (C.items[i].state == 2 && distance > 14) {
            C.items[i].bitmap.Reset();
            C.items[i].hexBmp.Reset();
            C.items[i].lqR.Reset();
            C.items[i].lqG.Reset();
            C.items[i].lqB.Reset();
            C.items[i].state = 0;
        }
    }
    if (job->items.empty()) {
        delete job;
        return;
    }
    g_threads++;
    HANDLE thread = CreateThread(nullptr, 0, ThumbnailThreadProc, job, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_threads--;
        for (const auto& item : job->items) {
            C.items[item.first].state = 0;
        }
        delete job;
    }
}

// Draws a pill with a line of text, centered; returns where it ends.
float CarPill(ID2D1DCRenderTarget* target,
              ID2D1SolidColorBrush* brush,
              const std::wstring& text,
              IDWriteTextFormat* format,
              float y,
              float padX,
              float padY,
              float backAlpha,
              float textAlpha,
              float maxWidth) {
    CarouselState& C = g_car;
    ComPtr<IDWriteTextLayout> layout;
    if (!format ||
        FAILED(C.dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                          format, maxWidth, 1000.0f,
                                          layout.GetAddressOf()))) {
        return y;
    }
    DWRITE_TEXT_METRICS metrics{};
    layout->GetMetrics(&metrics);
    const float w = std::min(metrics.width, maxWidth);
    const D2D1_RECT_F rect{(C.size.cx - w) / 2 - padX, y,
                           (C.size.cx + w) / 2 + padX,
                           y + metrics.height + 2 * padY};
    const float radius = (rect.bottom - rect.top) / 2;
    brush->SetColor(D2D1::ColorF(0, 0, 0, backAlpha));
    target->FillRoundedRectangle({rect, radius, radius}, brush);
    brush->SetColor(D2D1::ColorF(1, 1, 1, textAlpha));
    target->DrawTextLayout({(C.size.cx - w) / 2, y + padY}, layout.Get(), brush);
    return rect.bottom;
}

// Reads what the chip shows for a picture (once per picture): its size in
// pixels (only the file's header is read), file size, name and type.
void CarLoadInfo(int index) {
    CarouselState& C = g_car;
    if (C.infoIndex == index) {
        return;
    }
    C.infoIndex = index;
    const std::wstring& path = C.paths[index];
    C.infoName = PathFindFileName(path.c_str());
    PCWSTR ext = PathFindExtension(path.c_str());
    C.infoType = ext && *ext ? ext + 1 : L"";
    for (auto& ch : C.infoType) {
        ch = (wchar_t)towupper(ch);
    }
    C.infoSize.clear();
    C.infoDims.clear();
    WIN32_FILE_ATTRIBUTE_DATA attr{};
    if (GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &attr)) {
        const unsigned long long bytes =
            ((unsigned long long)attr.nFileSizeHigh << 32) |
            attr.nFileSizeLow;
        if (bytes >= 1024ull * 1024) {
            const unsigned long long tenths = bytes * 10 / (1024ull * 1024);
            C.infoSize = std::to_wstring(tenths / 10) + L"." +
                         std::to_wstring(tenths % 10) + L" MB";
        } else {
            const unsigned long long tenths = bytes * 10 / 1024;
            C.infoSize = std::to_wstring(tenths / 10) + L"." +
                         std::to_wstring(tenths % 10) + L" KB";
        }
    }
    if (C.wic) {
        ComPtr<IWICBitmapDecoder> decoder;
        ComPtr<IWICBitmapFrameDecode> frame;
        UINT w = 0, h = 0;
        if (SUCCEEDED(C.wic->CreateDecoderFromFilename(
                path.c_str(), nullptr, GENERIC_READ,
                WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf())) &&
            SUCCEEDED(decoder->GetFrame(0, frame.GetAddressOf())) &&
            SUCCEEDED(frame->GetSize(&w, &h))) {
            C.infoDims = std::to_wstring(w) + L" \u00D7 " + std::to_wstring(h);
        }
    }
}

// Cards style: a small box in the corner of the middle card with the
// picture's size in pixels, its file size, its name and a badge with its type.
void CarChip(ID2D1DCRenderTarget* target,
             ID2D1SolidColorBrush* brush,
             const D2D1_RECT_F& card,
             int index,
             float alpha) {
    CarouselState& C = g_car;
    if (alpha <= 0.01f || !C.chipMono || !C.chipSmall || !C.chipSans) {
        return;
    }
    CarLoadInfo(index);
    const float s = C.scale;
    const float pad = std::round(12 * s);
    const float margin = std::round(14 * s);
    const float gap = std::round(3 * s);
    const float badgeGap = std::round(8 * s);
    const float badgePadX = std::round(6 * s);
    const float badgePadY = std::round(2 * s);
    const float maxInner =
        std::max(40.0f, (card.right - card.left) - 2 * margin - 2 * pad);
    auto make = [&](const std::wstring& text, IDWriteTextFormat* format,
                    float width) -> ComPtr<IDWriteTextLayout> {
        ComPtr<IDWriteTextLayout> layout;
        C.dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), format,
                                   std::max(10.0f, width), 200.0f,
                                   layout.GetAddressOf());
        return layout;
    };

    ComPtr<IDWriteTextLayout> dims, size, name, badge;
    DWRITE_TEXT_METRICS dm{}, sm{}, nm{}, bm{};
    float badgeW = 0;
    if (!C.infoDims.empty()) {
        dims = make(C.infoDims, C.chipMono.Get(), maxInner);
    }
    if (!C.infoSize.empty()) {
        size = make(C.infoSize, C.chipSmall.Get(), maxInner);
    }
    if (!C.infoType.empty()) {
        badge = make(C.infoType, C.chipSmall.Get(), maxInner);
    }
    if (badge) {
        badge->GetMetrics(&bm);
        badgeW = bm.width + 2 * badgePadX;
    }
    name = make(C.infoName, C.chipSans.Get(),
                maxInner - (badge ? badgeW + badgeGap : 0));
    if (!name) {
        return;
    }
    float contentW = 0, contentH = 0;
    if (dims) {
        dims->GetMetrics(&dm);
        contentW = std::max(contentW, dm.width);
        contentH += dm.height + gap;
    }
    if (size) {
        size->GetMetrics(&sm);
        contentW = std::max(contentW, sm.width);
        contentH += sm.height + gap;
    }
    name->GetMetrics(&nm);
    const float nameW = std::min(nm.width, maxInner - (badge ? badgeW + badgeGap : 0));
    contentW = std::max(contentW, nameW + (badge ? badgeGap + badgeW : 0));
    contentH += nm.height;

    const float left = card.left + margin;
    const float bottom = card.bottom - margin;
    const D2D1_RECT_F box{left, bottom - contentH - 2 * pad,
                          left + contentW + 2 * pad, bottom};
    const float r = std::round(8 * s);
    brush->SetColor(D2D1::ColorF(0.05f, 0.05f, 0.06f, 0.78f * alpha));
    target->FillRoundedRectangle({box, r, r}, brush);
    brush->SetColor(D2D1::ColorF(1, 1, 1, 0.10f * alpha));
    target->DrawRoundedRectangle({box, r, r}, brush, 1.0f);

    float y = box.top + pad;
    if (dims) {
        brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * alpha));
        target->DrawTextLayout({left + pad, y}, dims.Get(), brush);
        y += dm.height + gap;
    }
    if (size) {
        brush->SetColor(D2D1::ColorF(0.68f, 0.68f, 0.72f, 0.9f * alpha));
        target->DrawTextLayout({left + pad, y}, size.Get(), brush);
        y += sm.height + gap;
    }
    brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * alpha));
    target->DrawTextLayout({left + pad, y}, name.Get(), brush);
    if (badge) {
        float cr = 0.7f, cg = 0.7f, cb = 0.75f;
        if (C.infoType == L"PNG") {
            cr = 0.96f, cg = 0.42f, cb = 0.46f;
        } else if (C.infoType == L"JPG" || C.infoType == L"JPEG" ||
                   C.infoType == L"JFIF") {
            cr = 0.40f, cg = 0.70f, cb = 0.98f;
        } else if (C.infoType == L"WEBP") {
            cr = 0.45f, cg = 0.88f, cb = 0.58f;
        }
        const float bh = bm.height + 2 * badgePadY;
        const float bx = left + pad + nameW + badgeGap;
        const float by = y + (nm.height - bh) / 2;
        const float br = std::round(4 * s);
        brush->SetColor(D2D1::ColorF(cr, cg, cb, 0.18f * alpha));
        target->FillRoundedRectangle({{bx, by, bx + badgeW, by + bh}, br, br},
                                     brush);
        brush->SetColor(D2D1::ColorF(cr, cg, cb, alpha));
        target->DrawTextLayout({bx + badgePadX, by + badgePadY}, badge.Get(),
                               brush);
    }
}

void CarKick();  // (defined further down; the hexagon code below uses it)

// ---------------------------------------------------------------------------
// The hexagon style: where a hexagon is, drawing, and the mouse
// ---------------------------------------------------------------------------

// The middle of picture `i` on the screen. Columns are fixed to the scrolling
// track; odd columns sit half a pitch lower so the honeycomb interlocks.
void HexCenter(int i, float* cx, float* cy) {
    const CarouselState& C = g_car;
    const int col = i / C.hexPerCol;
    const int r = i % C.hexPerCol;
    *cx = C.size.cx / 2.0f + (float)(col - C.hexScroll) * C.hexPitchX;
    *cy = C.hexTop + C.hexH / 2 + r * C.hexPitchY +
          ((col & 1) ? C.hexPitchY / 2 : 0.0f);
}

// Edge falloff: full size and opacity in the middle zone, then each hexagon
// shrinks around its own center and fades out toward the left / right edges.
struct HexFall {
    float scale, alpha;
};

HexFall HexFalloff(float cx) {
    const CarouselState& C = g_car;
    const float half = C.size.cx / 2.0f;
    const float d = std::fabs(cx - half);
    const float outer = half * 0.97f;
    const float inner = std::min(C.hexPitchX * kHexActiveCols, outer * 0.6f);
    const float t =
        std::clamp((d - inner) / std::max(1.0f, outer - inner), 0.0f, 1.0f);
    const float e = t * t * (3 - 2 * t);  // smoothstep
    return {1.0f - e, 1.0f - e};
}

// The columns that can be on the screen now.
void HexColRange(int* first, int* last) {
    const CarouselState& C = g_car;
    const int span = (int)std::ceil(C.size.cx / 2.0f / C.hexPitchX) + 1;
    *first = std::max(0, (int)std::floor(C.hexScroll) - span);
    *last = std::min(C.hexCols - 1, (int)std::ceil(C.hexScroll) + span);
}

// The picture under a point of the window, or -1.
int HexAt(float x, float y) {
    CarouselState& C = g_car;
    int firstCol, lastCol;
    HexColRange(&firstCol, &lastCol);
    const float hw = C.hexR;
    const float hh = C.hexH / 2;
    int best = -1;
    float bestDist = 1e30f;
    for (int col = firstCol; col <= lastCol; col++) {
        for (int r = 0; r < C.hexPerCol; r++) {
            const int i = col * C.hexPerCol + r;
            if (i >= CarCount()) {
                break;
            }
            float cx, cy;
            HexCenter(i, &cx, &cy);
            const HexFall f = HexFalloff(cx);
            if (f.alpha < 0.2f) {
                continue;
            }
            const float dx = std::fabs(x - cx) / f.scale;
            const float dy = std::fabs(y - cy) / f.scale;
            if (dx <= hw && dy <= hh &&
                1.7320508f * dx + dy <= 1.7320508f * hw) {
                const float dist = std::hypot(x - cx, y - cy);
                if (dist < bestDist) {
                    bestDist = dist;
                    best = i;
                }
            }
        }
    }
    return best;
}

// Selects a picture (keys or mouse) and scrolls to it if it's out of view.
void HexSelect(int index, bool reveal = true) {
    CarouselState& C = g_car;
    index = std::clamp(index, 0, CarCount() - 1);
    if (index == C.target) {
        return;
    }
    C.target = index;
    C.scroll = index;  // (the strip's own scroll isn't used in the grid)
    C.hexWheel = false;
    if (reveal) {
        HexReveal(false);
    }
    CarRequest();
    CarKick();
}

// ---------------------------------------------------------------------------
// Ambient backdrop: a blurred, darkened copy of the selected picture, blurred
// once per picture (1/8 size), cached (max 4 bitmaps), cross-faded.
// ---------------------------------------------------------------------------

constexpr float kAmbAlpha = 0.92f;     // opacity of the backdrop window
constexpr float kAmbVignette = 0.55f;  // darkness at the edges
constexpr double kAmbFade = 0.25;      // cross-fade (seconds)

// Bakes the backdrop pixels of a thumbnail (once, on arrival).
void AmbientStore(int index, IWICBitmapSource* source) {
    CarouselState& C = g_car;
    const UINT W = (UINT)C.ambW, H = (UINT)C.ambH;
    UINT sw = 0, sh = 0;
    if (!g_cfg.ambient || !C.wic || !source || W < 8 || H < 8 ||
        FAILED(source->GetSize(&sw, &sh)) || !sw || !sh) {
        return;
    }
    const double k = std::max((double)W / sw, (double)H / sh);
    const UINT coverW = std::max<UINT>(W, (UINT)std::ceil(sw * k));
    const UINT coverH = std::max<UINT>(H, (UINT)std::ceil(sh * k));
    const WICRect crop{(INT)((coverW - W) / 2), (INT)((coverH - H) / 2),
                       (INT)W, (INT)H};
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICBitmapClipper> clipper;
    if (FAILED(C.wic->CreateBitmapScaler(scaler.GetAddressOf())) ||
        FAILED(scaler->Initialize(source, coverW, coverH,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(C.wic->CreateBitmapClipper(clipper.GetAddressOf())) ||
        FAILED(clipper->Initialize(scaler.Get(), &crop))) {
        return;
    }
    std::vector<uint32_t> a((size_t)W * H), b((size_t)W * H);
    if (FAILED(clipper->CopyPixels(nullptr, W * 4, (UINT)(a.size() * 4),
                                   (BYTE*)a.data()))) {
        return;
    }
    for (int pass = 0; pass < 2; pass++) {
        FrostBlurLines(a.data(), b.data(), (int)H, (int)W, 1, (size_t)W, 5);
        FrostBlurLines(b.data(), a.data(), (int)W, (int)H, (size_t)W, 1, 5);
    }
    for (uint32_t& c : a) {  // about 60% darker
        c = 0xFF000000u | (((((c >> 16) & 255) * 100) >> 8) << 16) |
            (((((c >> 8) & 255) * 100) >> 8) << 8) | (((c & 255) * 100) >> 8);
    }
    C.items[index].ambPx = std::move(a);
    // Let go of the pixels of pictures that were dropped.
    for (CarouselItem& it : C.items) {
        if (it.state != 2 && !it.ambPx.empty()) {
            it.ambPx.clear();
            it.ambPx.shrink_to_fit();
        }
    }
}

// The cached backdrop bitmap of a picture (made from its pixels when needed).
ID2D1Bitmap* AmbientGet(int index) {
    CarouselState& C = g_car;
    if (index < 0 || index >= CarCount() || !C.dimCanvas.target) {
        return nullptr;
    }
    for (auto& e : C.ambCache) {
        if (e.index == index && e.bmp) {
            e.used = NowSeconds();
            return e.bmp.Get();
        }
    }
    CarouselItem& item = C.items[index];
    if (item.ambPx.empty() ||
        item.ambPx.size() != (size_t)C.ambW * (size_t)C.ambH) {
        return nullptr;
    }
    int slot = -1;
    for (int i = 0; i < 4; i++) {
        const auto& e = C.ambCache[i];
        if (e.index >= 0 && (e.index == C.ambFrom || e.index == C.ambTo)) {
            continue;
        }
        if (slot < 0 || e.used < C.ambCache[slot].used) {
            slot = i;
        }
    }
    if (slot < 0) {
        return nullptr;
    }
    auto& e = C.ambCache[slot];
    e.bmp.Reset();
    e.index = -1;
    if (FAILED(C.dimCanvas.target->CreateBitmap(
            D2D1::SizeU((UINT32)C.ambW, (UINT32)C.ambH), item.ambPx.data(),
            (UINT32)C.ambW * 4,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            e.bmp.GetAddressOf()))) {
        return nullptr;
    }
    e.index = index;
    e.used = NowSeconds();
    return e.bmp.Get();
}

// Redraws the backdrop only when the picture or the cross-fade changed; a
// change of the picker's fade only re-presents it.
void AmbientRender(float opacity) {
    CarouselState& C = g_car;
    if (!C.dimHwnd || !C.dimCanvas.target || CarCount() == 0) {
        return;
    }
    const double now = NowSeconds();
    const int want = CarIndex(C.target);
    if (want != C.ambTo && AmbientGet(want)) {
        C.ambFrom = C.ambTo;
        C.ambTo = want;
        C.ambStart = now;
    }
    const float f =
        C.ambFrom < 0
            ? 1.0f
            : (float)std::clamp((now - C.ambStart) / kAmbFade, 0.0, 1.0);
    const float e = f * f * (3 - 2 * f);
    const bool fading = f < 1.0f;
    if (fading || C.ambBusy || C.ambDrawnTo != C.ambTo) {
        const SIZE sz{C.monitor.right - C.monitor.left,
                      C.monitor.bottom - C.monitor.top};
        if (C.dimCanvas.Begin(sz)) {
            ID2D1DCRenderTarget* t = C.dimCanvas.target.Get();
            const D2D1_RECT_F full{0, 0, (float)sz.cx, (float)sz.cy};
            ID2D1Bitmap* to = AmbientGet(C.ambTo);
            ID2D1Bitmap* from = fading ? AmbientGet(C.ambFrom) : nullptr;
            if (to) {
                if (from) {
                    t->DrawBitmap(from, full, 1.0f,
                                  D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                t->DrawBitmap(to, full, from ? e : 1.0f,
                              D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                const D2D1_GRADIENT_STOP stops[3] = {
                    {0.0f, D2D1::ColorF(0, 0, 0, 0.0f)},
                    {0.55f, D2D1::ColorF(0, 0, 0, 0.0f)},
                    {1.0f, D2D1::ColorF(0, 0, 0, kAmbVignette)}};
                ComPtr<ID2D1GradientStopCollection> collection;
                ComPtr<ID2D1RadialGradientBrush> vignette;
                const D2D1_POINT_2F mid =
                    D2D1::Point2F(sz.cx * 0.5f, sz.cy * 0.5f);
                const float reach = std::hypot(mid.x, mid.y);
                if (SUCCEEDED(t->CreateGradientStopCollection(
                        stops, 3, collection.GetAddressOf())) &&
                    SUCCEEDED(t->CreateRadialGradientBrush(
                        D2D1::RadialGradientBrushProperties(
                            mid, D2D1::Point2F(0, 0), reach, reach),
                        collection.Get(), vignette.GetAddressOf()))) {
                    t->FillRectangle(full, vignette.Get());
                }
            } else {
                ComPtr<ID2D1SolidColorBrush> flat;
                t->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.42f),
                                         flat.GetAddressOf());
                if (flat) {
                    t->FillRectangle(full, flat.Get());
                }
            }
            C.dimCanvas.Finish();
        }
        C.ambDrawnTo = C.ambTo;
        C.ambAlpha = -1;  // present again
    }
    C.ambBusy = fading;  // (one more frame after it ends)
    const int a = (int)std::lround(std::clamp(opacity, 0.0f, 1.0f) *
                                   kAmbAlpha * 255.0f);
    if (a != C.ambAlpha && C.dimCanvas.dib) {
        C.ambAlpha = a;
        SIZE s = C.dimCanvas.size;
        POINT src{0, 0};
        POINT pos{C.monitor.left, C.monitor.top};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)a, AC_SRC_ALPHA};
        UpdateLayeredWindow(C.dimHwnd, nullptr, &pos, &s, C.dimCanvas.dc,
                            &src, 0, &blend, ULW_ALPHA);
    }
}

// The dimmed desktop behind the grid: a window of its own (plain black with a
// constant see-through level, so it costs nothing to draw), under the picker.
// A click on it closes the picker; the wheel still scrolls the grid.
LRESULT CALLBACK DimProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect((HDC)wParam, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEWHEEL:
            if (g_car.hwnd) {
                SendMessage(g_car.hwnd, WM_MOUSEWHEEL, wParam, lParam);
            }
            return 0;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            if (g_car.hwnd) {
                PostMessage(g_car.hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// How dark the desktop is: follows the picker's own fade.
void HexSetDim(float opacity) {
    CarouselState& C = g_car;
    if (!C.dimHwnd) {
        return;
    }
    if (C.ambOn) {
        AmbientRender(opacity);
        return;
    }
    const int alpha = (int)std::lround(std::clamp(opacity, 0.0f, 1.0f) *
                                       kHexBackdrop * 255.0f);
    if (alpha == C.dimAlpha) {
        return;
    }
    C.dimAlpha = alpha;
    SetLayeredWindowAttributes(C.dimHwnd, 0, (BYTE)alpha, LWA_ALPHA);
}

// Shows (under the picker) or hides the dimmed desktop.
void HexShowDim(bool on) {
    CarouselState& C = g_car;
    if (on) {
        if (!g_dimClassRegistered) {
            WNDCLASS wc{};
            wc.lpfnWndProc = DimProc;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.lpszClassName = kDimClass;
            if (RegisterClass(&wc)) {
                g_dimClassRegistered = true;
            }
        }
        const int w = C.monitor.right - C.monitor.left;
        const int h = C.monitor.bottom - C.monitor.top;
        if (!C.dimHwnd && g_dimClassRegistered) {
            C.dimHwnd = CreateWindowEx(
                WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                    WS_EX_NOACTIVATE,
                kDimClass, L"", WS_POPUP, C.monitor.left, C.monitor.top, w, h,
                nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
            if (C.dimHwnd) {
                C.dimAlpha = 0;
                C.ambOn = g_cfg.ambient;
                if (C.ambOn) {
                    // Per-pixel alpha: AmbientRender fills it.
                    C.ambOn = C.dimCanvas.Create(C.d2d.Get());
                    if (C.ambOn) {
                        C.dimCanvas.target->SetDpi(96.0f, 96.0f);
                    }
                }
                if (!C.ambOn) {
                    SetLayeredWindowAttributes(C.dimHwnd, 0, 0, LWA_ALPHA);
                }
            }
        }
        if (C.dimHwnd) {
            SetWindowPos(C.dimHwnd, HWND_TOPMOST, C.monitor.left,
                         C.monitor.top, w, h,
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);
            // (The picker stays over it.)
            SetWindowPos(C.hwnd, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    } else if (C.dimHwnd) {
        if (!C.ambOn) {
            SetLayeredWindowAttributes(C.dimHwnd, 0, 0, LWA_ALPHA);
        }
        C.ambAlpha = -1;
        C.dimAlpha = 0;
        ShowWindow(C.dimHwnd, SW_HIDE);
    }
}

// The name pill at the bottom-center of a hexagon (drawn in the hexagon's own
// coordinates, so it scales with it).
void HexBadge(ID2D1DCRenderTarget* target,
              ID2D1SolidColorBrush* brush,
              int index,
              float alpha) {
    CarouselState& C = g_car;
    if (alpha <= 0.02f || !C.chipSans) {
        return;
    }
    CarouselItem& item = C.items[index];
    if (item.badge.empty()) {
        std::wstring name = PathFindFileName(C.paths[index].c_str());
        const size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0) {
            name.resize(dot);
        }
        item.badge = name;
    }
    const float s = C.scale;
    const float maxW = C.hexW * 0.46f;
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(C.dwrite->CreateTextLayout(
            item.badge.c_str(), (UINT32)item.badge.size(), C.chipSans.Get(),
            maxW, 100.0f, layout.GetAddressOf()))) {
        return;
    }
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    const float w = std::min(m.width, maxW);
    const float padX = std::round(8 * s);
    const float padY = std::round(3 * s);
    const float h = m.height + 2 * padY;
    const float bottom = C.hexH / 2 - std::round(14 * s);
    const D2D1_RECT_F rect{-w / 2 - padX, bottom - h, w / 2 + padX, bottom};
    brush->SetColor(D2D1::ColorF(0, 0, 0, 0.62f * alpha));
    target->FillRoundedRectangle({rect, h / 2, h / 2}, brush);
    brush->SetColor(D2D1::ColorF(1, 1, 1, 0.92f * alpha));
    target->DrawTextLayout({-w / 2, bottom - h + padY}, layout.Get(), brush);
}

// The honeycomb: dark empty slots, pictures popping into them, every hexagon
// scaled / faded by its place on the screen; the selected one is lit and
// framed.
void CarDrawHex(float opacity) {
    CarouselState& C = g_car;
    if (!C.hwnd || !C.canvas.Begin(C.size)) {
        return;
    }
    ID2D1DCRenderTarget* target = C.canvas.target.Get();
    ComPtr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                  brush.GetAddressOf());
    if (!brush || !C.hexGeom) {
        C.canvas.End(C.hwnd, C.position, 0);
        return;
    }
    const int n = CarCount();
    if ((int)C.hexAmt.size() != n) {
        C.hexAmt.assign(n, 0.0f);
    }
    if ((int)C.hexShow.size() != n) {
        C.hexShow.assign(n, 0.0f);
    }
    const float s = C.scale;
    const D2D1_MATRIX_3X2_F none = D2D1::Matrix3x2F::Identity();

    // (The dimmed desktop is a window of its own; it fades with the picker.)
    HexSetDim(opacity);

    struct Cell {
        int index;
        float cx, cy, scale, alpha, amt, show;
    };
    std::vector<Cell> cells;
    int firstCol, lastCol;
    HexColRange(&firstCol, &lastCol);
    for (int col = firstCol; col <= lastCol; col++) {
        for (int r = 0; r < C.hexPerCol; r++) {
            const int i = col * C.hexPerCol + r;
            if (i >= n) {
                break;
            }
            float cx, cy;
            HexCenter(i, &cx, &cy);
            const HexFall f = HexFalloff(cx);
            if (f.alpha < 0.01f) {
                continue;
            }
            cells.push_back({i, cx, cy, f.scale, f.alpha, C.hexAmt[i],
                             C.hexShow[i]});
        }
    }
    // The lit ones last, so they are over their neighbours.
    std::sort(cells.begin(), cells.end(),
              [](const Cell& a, const Cell& b) { return a.amt < b.amt; });

    for (const Cell& v : cells) {
        CarouselItem& item = C.items[v.index];
        const float ease = 1.0f - std::pow(1.0f - v.show, 3.0f);
        const float pop =
            v.scale * (1.0f + kHexPop * v.amt) * (0.7f + 0.3f * ease);

        // The empty slot (stays under the picture; shows while it isn't in).
        const bool covered = ease > 0.99f && v.alpha > 0.99f && item.hexBmp;
        if (!covered) {
            target->SetTransform(D2D1::Matrix3x2F::Scale(v.scale, v.scale) *
                                 D2D1::Matrix3x2F::Translation(v.cx, v.cy));
            brush->SetColor(D2D1::ColorF(0.03f, 0.04f, 0.08f,
                                         0.40f * v.alpha * (1.0f - 0.6f * ease)));
            target->FillGeometry(C.hexGeom.Get(), brush.Get());
        }

        if (ease < 0.01f || !item.hexBmp) {
            target->SetTransform(none);
            continue;
        }
        const float a = v.alpha * ease;

        // A soft shadow under the lit one.
        if (v.amt > 0.02f) {
            for (int i = 3; i >= 1; i--) {
                const float grow = pop * (1.0f + 0.03f * i);
                target->SetTransform(
                    D2D1::Matrix3x2F::Scale(grow, grow) *
                    D2D1::Matrix3x2F::Translation(v.cx, v.cy + 6 * s));
                brush->SetColor(D2D1::ColorF(0, 0, 0, 0.10f * v.amt * a));
                target->FillGeometry(C.hexGeom.Get(), brush.Get());
            }
        }

        target->SetTransform(D2D1::Matrix3x2F::Scale(pop, pop) *
                             D2D1::Matrix3x2F::Translation(v.cx, v.cy));
        const D2D1_SIZE_F size = item.hexBmp->GetSize();
        target->DrawBitmap(item.hexBmp.Get(),
                           {-size.width / 2, -size.height / 2, size.width / 2,
                            size.height / 2},
                           a, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        // The ones that aren't selected are a little dimmed.
        const float dark = kHexDim * (1.0f - v.amt) * a;
        if (dark > 0.005f) {
            brush->SetColor(D2D1::ColorF(0.01f, 0.02f, 0.05f, dark));
            target->FillGeometry(C.hexGeom.Get(), brush.Get());
        }
        // The selected one: a clean white frame.
        if (v.amt > 0.02f) {
            brush->SetColor(D2D1::ColorF(1, 1, 1, v.amt * a));
            target->DrawGeometry(C.hexGeom.Get(), brush.Get(),
                                 std::max(2.0f, 2.5f * s));
        }
        // The name pill (not on the tiny ones at the edges).
        if (ease > 0.3f && v.scale > 0.45f) {
            HexBadge(target, brush.Get(), v.index, a);
        }
        target->SetTransform(none);
    }

    // The name, the place in the folder, and the keys.
    const int index = C.target;
    std::wstring name = PathFindFileName(C.paths[index].c_str());
    PathRemoveExtension(&name[0]);
    name.resize(wcslen(name.c_str()));
    const std::wstring label = name + L"   \u00B7   " +
                               std::to_wstring(index + 1) + L" / " +
                               std::to_wstring(n);
    const std::wstring hint =
        std::wstring(L"hover or \u2190 \u2191 \u2193 \u2192 browse     wheel / "
                     L"drag scroll     Enter / click set     T transition: ") +
        kTransitionNames[g_cfg.transition] +
        (UsesSubject(g_cfg.transition)
             ? L"     U subject     Del forget"
             : L"") +
        L"     H next full-screen style     S normal styles     F folder     "
        L"Esc close";
    float y = CarPill(target, brush.Get(), label, C.labelFormat.Get(),
                      C.hexLabelY, std::round(16 * s), std::round(6 * s), 0.6f,
                      0.95f, C.size.cx * 0.55f) +
              std::round(8 * s);
    CarPill(target, brush.Get(), hint, C.hintFormat.Get(), y,
            std::round(14 * s), std::round(4 * s), 0.45f, 0.7f,
            C.size.cx * 0.8f);
    C.canvas.End(C.hwnd, C.position,
                 (BYTE)std::clamp(opacity * 255, 0.0f, 255.0f));
}

// The picture under a point of the window: its place in the row.
bool SpiralHit(float x, float y, int* hit) {
    CarouselState& C = g_car;
    const int base = (int)std::floor(C.spScroll);
    float bestD = -1;
    bool found = false;
    for (int v = base - 8; v <= base + 9; v++) {
        if (!SpiralValid(v)) {
            continue;
        }
        const float u = (float)(v - C.spScroll);
        if (std::fabs(u) >= kSpiralReach - 0.5f) {
            continue;
        }
        const SpiralSpot p = SpiralPlace(u);
        if (p.alpha < 0.3f) {
            continue;
        }
        const float rad = std::max(p.d / 2, 10.0f * C.scale);
        const float dx = x - p.x, dy = y - p.y;
        if (dx * dx + dy * dy <= rad * rad && p.d > bestD) {
            bestD = p.d;
            *hit = v;
            found = true;
        }
    }
    return found;
}

// The spiral: a soft glow behind the middle, a thin thread along both arms,
// and the circles on it (small ones first, the big ones over them).
void CarDrawSpiral(float opacity) {
    CarouselState& C = g_car;
    if (!C.hwnd || !C.canvas.Begin(C.size)) {
        return;
    }
    ID2D1DCRenderTarget* target = C.canvas.target.Get();
    ComPtr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                  brush.GetAddressOf());
    if (!brush) {
        C.canvas.End(C.hwnd, C.position, 0);
        return;
    }
    const float s = C.scale;
    const D2D1_POINT_2F mid = D2D1::Point2F(C.spCx, C.spCy);
    const float open = (float)std::clamp(C.spClock / 0.6, 0.0, 1.0);

    // (The dimmed desktop is a window of its own; it fades with the picker.)
    HexSetDim(opacity);

    // A soft glow behind the middle picture.
    {
        const float rg = C.spBig * 1.05f;
        const D2D1_GRADIENT_STOP stops[2] = {
            {0.0f, D2D1::ColorF(0.50f, 0.78f, 1.0f, 0.20f * open)},
            {1.0f, D2D1::ColorF(0.50f, 0.78f, 1.0f, 0.0f)}};
        ComPtr<ID2D1GradientStopCollection> collection;
        ComPtr<ID2D1RadialGradientBrush> glow;
        if (SUCCEEDED(target->CreateGradientStopCollection(
                stops, 2, collection.GetAddressOf())) &&
            SUCCEEDED(target->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(
                    mid, D2D1::Point2F(0, 0), rg, rg),
                collection.Get(), glow.GetAddressOf()))) {
            target->FillEllipse(D2D1::Ellipse(mid, rg, rg), glow.Get());
        }
    }

    // The thread the pictures glide along (both arms, fading out).
    {
        D2D1_POINT_2F last{};
        bool have = false;
        for (int k = -70; k <= 70; k++) {
            const SpiralSpot p = SpiralPlace(k * 0.1f);
            const D2D1_POINT_2F pt = D2D1::Point2F(p.x, p.y);
            if (have) {
                brush->SetColor(
                    D2D1::ColorF(0.75f, 0.88f, 1.0f, 0.20f * p.alpha));
                target->DrawLine(last, pt, brush.Get(),
                                 std::max(1.0f, 1.3f * s));
            }
            last = pt;
            have = true;
        }
    }

    struct Entry {
        int v;
        SpiralSpot p;
    };
    std::vector<Entry> entries;
    const int base = (int)std::floor(C.spScroll);
    for (int v = base - 8; v <= base + 9; v++) {
        if (!SpiralValid(v)) {
            continue;
        }
        const float u = (float)(v - C.spScroll);
        if (std::fabs(u) >= kSpiralReach) {
            continue;
        }
        const SpiralSpot p = SpiralPlace(u);
        if (p.alpha < 0.01f) {
            continue;
        }
        entries.push_back({v, p});
    }
    std::sort(entries.begin(), entries.end(),
              [](const Entry& a, const Entry& b) { return a.p.d < b.p.d; });

    for (const Entry& e : entries) {
        const SpiralSpot& p = e.p;
        CarouselItem& item = C.items[SpiralIndex(e.v)];
        const float rad = p.d / 2;
        const D2D1_POINT_2F c = D2D1::Point2F(p.x, p.y);

        // A soft shadow.
        for (int i = 3; i >= 1; i--) {
            const float g = rad + i * (0.035f * p.d + 1.5f * s);
            brush->SetColor(D2D1::ColorF(0, 0, 0, 0.08f * p.alpha));
            target->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(p.x, p.y + 0.05f * p.d + 3 * s),
                              g, g),
                brush.Get());
        }

        // The picture (the smaller copy for the small circles), or a dark
        // disc while it is being read.
        ID2D1Bitmap* bmp = item.spBmp.Get();
        if (item.spMip && p.d < 0.8f * C.spBig) {
            bmp = item.spMip.Get();
        }
        if (bmp) {
            const D2D1_RECT_F rect{p.x - rad, p.y - rad, p.x + rad,
                                   p.y + rad};
            target->DrawBitmap(bmp, rect, p.alpha,
                               D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            brush->SetColor(D2D1::ColorF(0.10f, 0.11f, 0.14f, 0.85f * p.alpha));
            target->FillEllipse(D2D1::Ellipse(c, rad, rad), brush.Get());
        }

        // The ones away from the middle are a little dimmed.
        const float dark = 0.40f * (1.0f - p.close) * p.alpha;
        if (dark > 0.005f) {
            brush->SetColor(D2D1::ColorF(0.01f, 0.02f, 0.05f, dark));
            target->FillEllipse(D2D1::Ellipse(c, rad, rad), brush.Get());
        }

        // A hairline on all of them, a lit ring on the selected one.
        brush->SetColor(
            D2D1::ColorF(1, 1, 1, (0.10f + 0.10f * p.close) * p.alpha));
        target->DrawEllipse(D2D1::Ellipse(c, rad - 0.5f, rad - 0.5f),
                            brush.Get(), 1.0f);
        if (p.close > 0.02f) {
            const float k = p.close * p.alpha;
            const float w = std::max(2.0f, 3.0f * s);
            brush->SetColor(D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.10f * k));
            target->DrawEllipse(D2D1::Ellipse(c, rad + w, rad + w),
                                brush.Get(), w * 4.0f);
            brush->SetColor(D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.22f * k));
            target->DrawEllipse(D2D1::Ellipse(c, rad + 0.5f * w, rad + 0.5f * w),
                                brush.Get(), w * 2.2f);
            brush->SetColor(D2D1::ColorF(1, 1, 1, k));
            target->DrawEllipse(D2D1::Ellipse(c, rad - 0.5f * w, rad - 0.5f * w),
                                brush.Get(), w);
        }
    }

    // The name, the place in the folder, and the keys.
    const int index = SpiralIndex(C.target);
    std::wstring name = PathFindFileName(C.paths[index].c_str());
    PathRemoveExtension(&name[0]);
    name.resize(wcslen(name.c_str()));
    const std::wstring label = name + L"   \u00B7   " +
                               std::to_wstring(index + 1) + L" / " +
                               std::to_wstring(CarCount());
    const std::wstring hint =
        std::wstring(L"\u2190 \u2191 \u2193 \u2192 browse     wheel scroll     "
                     L"Enter / click set     T transition: ") +
        kTransitionNames[g_cfg.transition] +
        (UsesSubject(g_cfg.transition)
             ? L"     U subject     Del forget"
             : L"") +
        L"     H next full-screen style     S normal styles     F folder     "
        L"Esc close";
    float y = CarPill(target, brush.Get(), label, C.labelFormat.Get(),
                      C.spLabelY, std::round(16 * s), std::round(6 * s), 0.6f,
                      0.95f, C.size.cx * 0.55f) +
              std::round(8 * s);
    CarPill(target, brush.Get(), hint, C.hintFormat.Get(), y,
            std::round(14 * s), std::round(4 * s), 0.45f, 0.7f,
            C.size.cx * 0.9f);
    C.canvas.End(C.hwnd, C.position,
                 (BYTE)std::clamp(opacity * 255, 0.0f, 255.0f));
}

// Switches the window between the band (strip, fan, slant, cards) and the
// full-screen styles: 1 = the hexagon grid (a band of its own, with the dimmed
// desktop behind it), 2 = the spiral. This is instant; CarSwitchMode does it
// while nothing shows.
void CarSetMode(int mode) {
    CarouselState& C = g_car;
    const int now = CarMode();
    if (!C.hwnd || now == mode) {
        return;
    }
    C.hex = mode == 1;
    C.spiral = mode == 2;
    C.fish = mode == 3;
    C.tun = mode == 4;
    C.target = CarIndex(C.target);
    C.scroll = C.target;
    C.lqVel = 0;
    // (The pictures being read were for the other layout: drop them.)
    C.generation = ++g_carouselGeneration;
    const float monitorH = (float)(C.monitor.bottom - C.monitor.top);
    if (mode == 1) {
        C.hexAmt.assign(CarCount(), 0.0f);
        C.hexShow.assign(CarCount(), 0.0f);  // every tile pops in again
        C.hexClock = 0;
        C.hexDrag = C.hexDragMoved = false;
        C.size = {C.monitor.right - C.monitor.left, (LONG)C.hexBandH};
        C.position = {C.monitor.left,
                      C.monitor.top + (LONG)((monitorH - C.hexBandH) / 2)};
        // Only a real mouse move selects: the mouse resting where it is
        // doesn't.
        POINT cursor{};
        GetCursorPos(&cursor);
        C.mouseLast = {cursor.x - C.position.x, cursor.y - C.position.y};
        HexReveal(true);
        HexShowDim(true);
    } else if (mode == 2) {
        SpiralSetup();
        C.size = C.spSize;
        C.position = C.spPosition;
        C.spScroll = C.target;
        C.spVel = 0;
        C.spClock = 0;  // the bloom plays again
        HexShowDim(true);
    } else if (mode == 3) {
        FishSetup();
        C.size = C.fxSize;
        C.position = C.fxPosition;
        const double row = std::clamp(C.target / C.fishCols, 0, C.fishRows - 1);
        C.fishScroll = C.fishScrollTarget = row;
        C.fishVel = 0;
        C.fishClock = 0;
        C.fishFc = C.target % C.fishCols;
        C.fishFr = C.target / C.fishCols;
        C.fishFcV = C.fishFrV = 0;
        C.fishMouse = C.fishDrag = C.fishDragMoved = false;
        C.fishMouseAt = {-1, -1};
        HexShowDim(true);
    } else if (mode == 4) {
        TunnelSetup();
        C.size = C.fxSize;
        C.position = C.fxPosition;
        C.tunScroll = C.target;
        C.tunVel = 0;
        C.tunClock = 0;  // the fly-in plays again
        HexShowDim(true);
    } else {
        C.size = C.bandSize;
        C.position = C.bandPosition;
        C.ambBusy = false;
        HexShowDim(false);
    }
    // Pictures read for another layout don't have the bitmap this one needs:
    // read them again.
    for (CarouselItem& it : C.items) {
        const bool have = mode == 1   ? (bool)it.hexBmp
                          : mode == 2 ? (bool)it.spBmp
                          : mode == 3 ? (bool)it.fishBmp
                          : mode == 4 ? (bool)it.tunBmp
                                      : (bool)it.bitmap;
        if (mode != 2) {
            it.spBmp.Reset();
            it.spMip.Reset();
        }
        if (mode != 3) {
            it.fishBmp.Reset();
            it.fishMip.Reset();
        }
        if (mode != 4) {
            it.tunBmp.Reset();
            it.tunMip.Reset();
        }
        if (mode != 0) {
            it.lqR.Reset();
            it.lqG.Reset();
            it.lqB.Reset();
        }
        if (it.state == 1 || (it.state == 2 && !have)) {
            it.state = 0;
        }
    }
    SetWindowPos(C.hwnd, HWND_TOPMOST, C.position.x, C.position.y, C.size.cx,
                 C.size.cy, SWP_NOACTIVATE);
    CarRequest();
}

// Asks for another layout (0 band, 1 hexagons, 2 spiral): the picker fades
// out, CarTick changes the window while nothing shows, and it fades back in.
void CarSwitchMode(int want) {
    CarouselState& C = g_car;
    if (!C.hwnd) {
        return;
    }
    const int now = CarMode();
    if (now == want) {
        if (C.switchTo >= 0) {
            // Changed its mind halfway: stay, fade back in.
            C.switchTo = -1;
            C.openStart = NowSeconds();
            C.openLen = 0.12;
        }
        return;
    }
    if (C.switchTo == want) {
        return;
    }
    C.switchTo = want;
    C.switchStart = NowSeconds();
}

// The keys line shared by the full-screen styles.
std::wstring CarHint(PCWSTR keys) {
    return std::wstring(keys) + L"     T transition: " +
           kTransitionNames[g_cfg.transition] +
           (UsesSubject(g_cfg.transition)
                ? L"     U subject     Del forget"
                : L"") +
           L"     H next full-screen style     S normal styles     F folder"
           L"     Esc close";
}

// The name pill at the bottom of a tile.
void FishBadge(ID2D1DCRenderTarget* target,
               ID2D1SolidColorBrush* brush,
               int index,
               float cx,
               float bottom,
               float maxW,
               float alpha) {
    CarouselState& C = g_car;
    if (alpha <= 0.02f || !C.chipSans) {
        return;
    }
    CarouselItem& item = C.items[index];
    if (item.badge.empty()) {
        std::wstring name = PathFindFileName(C.paths[index].c_str());
        const size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0) {
            name.resize(dot);
        }
        item.badge = name;
    }
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(C.dwrite->CreateTextLayout(
            item.badge.c_str(), (UINT32)item.badge.size(), C.chipSans.Get(),
            std::max(20.0f, maxW), 100.0f, layout.GetAddressOf()))) {
        return;
    }
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    const float w = std::min(m.width, maxW);
    const float padX = std::round(8 * C.scale);
    const float padY = std::round(3 * C.scale);
    const float h = m.height + 2 * padY;
    const D2D1_RECT_F rect{cx - w / 2 - padX, bottom - h, cx + w / 2 + padX,
                           bottom};
    brush->SetColor(D2D1::ColorF(0, 0, 0, 0.62f * alpha));
    target->FillRoundedRectangle({rect, h / 2, h / 2}, brush);
    brush->SetColor(D2D1::ColorF(1, 1, 1, 0.92f * alpha));
    target->DrawTextLayout({cx - w / 2, bottom - h + padY}, layout.Get(),
                           brush);
}

// The fisheye grid: a tidy grid; tiles near the focus swell and push the
// others apart.
void CarDrawFish(float opacity) {
    CarouselState& C = g_car;
    if (!C.hwnd || !C.canvas.Begin(C.size)) {
        return;
    }
    ID2D1DCRenderTarget* target = C.canvas.target.Get();
    ComPtr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                  brush.GetAddressOf());
    if (!brush || C.fishCols <= 0) {
        C.canvas.End(C.hwnd, C.position, 0);
        return;
    }
    const int n = CarCount();
    if ((int)C.fishShow.size() != n) {
        C.fishShow.assign(n, 0.0f);
    }
    HexSetDim(opacity);
    FishLayout();
    const float s = C.scale;
    const float peak = kFishPeak - 1.0f;
    const float fadeZone = std::round(70 * s);
    const float cornerR = std::round(14 * s);
    for (int pass = 0; pass < 2; pass++) {  // the selected one last
        for (int r = 0; r < C.fishRows; r++) {
            const float cellH = C.fishH[r];
            const float cy = C.fishY[r] + cellH / 2;
            const float edge = std::clamp(
                std::min(cy, C.fishLabelY - cy) / fadeZone, 0.0f, 1.0f);
            if (edge <= 0.01f) {
                continue;
            }
            for (int c = 0; c < C.fishCols; c++) {
                const int i = r * C.fishCols + c;
                if (i >= n) {
                    break;
                }
                if ((i == C.target) != (pass == 1)) {
                    continue;
                }
                const float cx = C.fishX[c] + C.fishW[c] / 2;
                const float dist = (float)std::hypot(c - C.fishFc, r - C.fishFr);
                const float sc = std::min({1.0f + peak * FishGauss(dist),
                                           C.fishW[c] / C.fishTW,
                                           cellH / C.fishTH});
                CarouselItem& item = C.items[i];
                const float ease = 1.0f - std::pow(1.0f - C.fishShow[i], 3.0f);
                const float pop = sc * (0.93f + 0.07f * ease);
                const float w = C.fishTW * pop, h = C.fishTH * pop;
                const float a = edge * ease;
                const D2D1_RECT_F rect{cx - w / 2, cy - h / 2, cx + w / 2,
                                       cy + h / 2};
                const float rad = cornerR * (w / (float)C.fishBake.cx);
                if (!item.fishBmp || ease < 0.99f) {
                    // The empty slot, until the picture pops in.
                    const float ws = C.fishTW * sc, hs = C.fishTH * sc;
                    brush->SetColor(D2D1::ColorF(
                        0.03f, 0.04f, 0.08f,
                        0.40f * edge * (1.0f - 0.6f * ease)));
                    target->FillRoundedRectangle(
                        {{cx - ws / 2, cy - hs / 2, cx + ws / 2, cy + hs / 2},
                         rad, rad},
                        brush.Get());
                }
                if (!item.fishBmp || ease < 0.01f) {
                    continue;
                }
                const float lift = std::clamp((sc - 1.1f) / peak, 0.0f, 1.0f);
                if (lift > 0.02f) {
                    for (int k = 3; k >= 1; k--) {
                        const float g = k * 4 * s;
                        brush->SetColor(
                            D2D1::ColorF(0, 0, 0, 0.10f * lift * a));
                        target->FillRoundedRectangle(
                            {{rect.left - g, rect.top - g + 6 * s,
                              rect.right + g, rect.bottom + g + 6 * s},
                             rad + g, rad + g},
                            brush.Get());
                    }
                }
                ID2D1Bitmap* bmp = item.fishBmp.Get();
                if (item.fishMip && sc / kFishPeak < 0.6f) {
                    bmp = item.fishMip.Get();
                }
                target->DrawBitmap(bmp, rect, a,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                const float dark =
                    0.30f * (1.0f - std::clamp((sc - 1.0f) / peak, 0.0f, 1.0f)) *
                    a;
                if (dark > 0.005f) {
                    brush->SetColor(D2D1::ColorF(0.01f, 0.02f, 0.05f, dark));
                    target->FillRoundedRectangle({rect, rad, rad}, brush.Get());
                }
                brush->SetColor(D2D1::ColorF(1, 1, 1, 0.10f * a));
                target->DrawRoundedRectangle({rect, rad, rad}, brush.Get(),
                                             1.0f);
                if (i == C.target) {
                    const float w2 = std::max(2.0f, 3.0f * s);
                    brush->SetColor(D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.10f * a));
                    target->DrawRoundedRectangle({rect, rad, rad}, brush.Get(),
                                                 w2 * 4.0f);
                    brush->SetColor(D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.22f * a));
                    target->DrawRoundedRectangle({rect, rad, rad}, brush.Get(),
                                                 w2 * 2.2f);
                    brush->SetColor(D2D1::ColorF(1, 1, 1, a));
                    target->DrawRoundedRectangle({rect, rad, rad}, brush.Get(),
                                                 w2);
                    FishBadge(target, brush.Get(), i, cx,
                              rect.bottom - std::round(12 * s), w * 0.8f, a);
                }
            }
        }
    }
    const int index = C.target;
    std::wstring name = PathFindFileName(C.paths[index].c_str());
    PathRemoveExtension(&name[0]);
    name.resize(wcslen(name.c_str()));
    const std::wstring label = name + L"   \u00B7   " +
                               std::to_wstring(index + 1) + L" / " +
                               std::to_wstring(n);
    const std::wstring hint = CarHint(
        L"\u2190 \u2191 \u2193 \u2192 select     wheel / drag scroll     "
        L"hover swell     Enter / click set");
    float y = CarPill(target, brush.Get(), label, C.labelFormat.Get(),
                      C.fishLabelY, std::round(16 * s), std::round(6 * s),
                      0.6f, 0.95f, C.size.cx * 0.55f) +
              std::round(8 * s);
    CarPill(target, brush.Get(), hint, C.hintFormat.Get(), y,
            std::round(14 * s), std::round(4 * s), 0.45f, 0.7f,
            C.size.cx * 0.9f);
    C.canvas.End(C.hwnd, C.position,
                 (BYTE)std::clamp(opacity * 255, 0.0f, 255.0f));
}

// The depth tunnel: frames on the walls of a corridor, the camera flies
// along it; the selected frame faces you at the end.
void CarDrawTunnel(float opacity) {
    CarouselState& C = g_car;
    if (!C.hwnd || !C.canvas.Begin(C.size)) {
        return;
    }
    ID2D1DCRenderTarget* target = C.canvas.target.Get();
    ComPtr<ID2D1SolidColorBrush> brush;
    ComPtr<ID2D1Layer> layer;
    target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                  brush.GetAddressOf());
    target->CreateLayer(nullptr, layer.GetAddressOf());
    if (!brush || !layer) {
        C.canvas.End(C.hwnd, C.position, 0);
        return;
    }
    HexSetDim(opacity);
    const float s = C.scale;
    const D2D1_POINT_2F vp = D2D1::Point2F(C.tunVx, C.tunVy);
    const float open = (float)std::clamp(C.tunClock / 0.9, 0.0, 1.0);
    const float fly = TunFly();
    const D2D1_MATRIX_3X2_F none = D2D1::Matrix3x2F::Identity();
    const float cornerR = std::round(10 * s);

    // Thin glowing lines of the corridor, converging on the vanishing point.
    {
        const D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.0f)},
            {0.45f, D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.10f)},
            {1.0f, D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.40f)}};
        ComPtr<ID2D1GradientStopCollection> collection;
        if (SUCCEEDED(target->CreateGradientStopCollection(
                stops, 3, collection.GetAddressOf()))) {
            for (int e = 0; e < 4; e++) {
                const float sx = (e & 1) ? 1.0f : -1.0f;
                const float sy = (e & 2) ? 1.0f : -1.0f;
                const D2D1_POINT_2F end =
                    D2D1::Point2F(vp.x + sx * C.tunWall * 1.25f,
                                  vp.y + sy * C.tunWallHH * 1.25f);
                ComPtr<ID2D1LinearGradientBrush> lg;
                if (SUCCEEDED(target->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(vp, end),
                        collection.Get(), lg.GetAddressOf()))) {
                    lg->SetOpacity(0.30f * open);
                    target->DrawLine(vp, end, lg.Get(),
                                     std::max(4.0f, 7.0f * s));
                    lg->SetOpacity(open);
                    target->DrawLine(vp, end, lg.Get(),
                                     std::max(1.0f, 1.4f * s));
                }
            }
        }
    }

    const int base = (int)std::floor(C.tunScroll);
    for (int v = base + (int)kTunReach + 2; v >= base - 2; v--) {  // far to near
        if (!TunValid(v)) {
            continue;
        }
        const float dd = (float)(v - C.tunScroll);
        const float d = dd + (dd >= -0.001f ? fly : 0.0f);
        TunQuad q;
        if (!TunnelPose(v, d, &q) || q.xR - q.xL < 2.0f) {
            continue;
        }
        const float left = std::min(q.xL, q.xR), right = std::max(q.xL, q.xR);
        const float top = std::min(q.tL, q.tR), bottom = std::max(q.bL, q.bR);
        if (right < -20 || left > C.size.cx + 20 || bottom < 0 ||
            top > C.size.cy) {
            continue;
        }
        CarouselItem& item = C.items[TunIndex(v)];
        ID2D1Bitmap* bmp = item.tunBmp.Get();
        if (item.tunMip && (q.xR - q.xL) < 0.5f * C.tunHeroW) {
            bmp = item.tunMip.Get();
        }
        ComPtr<ID2D1PathGeometry> poly;
        ComPtr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(C.d2d->CreatePathGeometry(poly.GetAddressOf())) &&
            SUCCEEDED(poly->Open(sink.GetAddressOf()))) {
            sink->BeginFigure(D2D1::Point2F(q.xL, q.tL),
                              D2D1_FIGURE_BEGIN_FILLED);
            const D2D1_POINT_2F pts[3] = {{q.xR, q.tR}, {q.xR, q.bR},
                                          {q.xL, q.bL}};
            sink->AddLines(pts, 3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
        } else {
            poly.Reset();
        }
        const float pad = 16 * s;
        target->PushLayer(
            D2D1::LayerParameters(
                D2D1::RectF(left - pad, top - pad, right + pad, bottom + pad),
                nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                D2D1::IdentityMatrix(), q.alpha),
            layer.Get());
        if (bmp) {
            const D2D1_SIZE_F bs = bmp->GetSize();
            const bool flat = q.hero > 0.999f;
            // Thin slices (about 3 px) with a perspective-correct picture, so
            // the sloped frame reads as one continuous surface, not steps.
            const int N =
                flat ? 1
                     : std::clamp((int)std::ceil((right - left) / 3.0f), 4,
                                  220);
            float x0, t0, b0;
            TunnelEdge(v, d, 0.0f, q.hero, &x0, &t0, &b0);
            for (int j = 0; j < N; j++) {
                const float u0 = (float)j / N, u1 = (float)(j + 1) / N;
                float x1, t1, b1;
                TunnelEdge(v, d, u1, q.hero, &x1, &t1, &b1);
                const float w = x1 - x0;
                if (w > 0.01f) {
                    const float cm = (t0 + t1 + b0 + b1) * 0.25f;
                    const float hm = ((b0 - t0) + (b1 - t1)) * 0.5f;
                    const float xm = (x0 + x1) * 0.5f;
                    const float sh = ((t1 - t0) + (b1 - b0)) * 0.5f / w;
                    // A little overlap hides seams; the picture is widened by
                    // the same share so it stays continuous.
                    const float over = j + 1 < N ? 1.25f : 0.0f;
                    const float uo =
                        std::min(1.0f, u1 + (u1 - u0) * over / w);
                    target->SetTransform(D2D1::Matrix3x2F(
                        1.0f, sh, 0.0f, 1.0f, 0.0f, -sh * xm));
                    const D2D1_RECT_F dst{x0, cm - hm * 0.5f, x1 + over,
                                          cm + hm * 0.5f};
                    const D2D1_RECT_F src{u0 * bs.width, 0, uo * bs.width,
                                          bs.height};
                    target->DrawBitmap(bmp, dst, 1.0f,
                                       D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                                       &src);
                }
                x0 = x1;
                t0 = t1;
                b0 = b1;
            }
            target->SetTransform(none);
        } else if (poly) {
            brush->SetColor(D2D1::ColorF(0.10f, 0.11f, 0.14f, 0.85f));
            target->FillGeometry(poly.Get(), brush.Get());
        }
        if (poly) {
            if (q.dark > 0.01f) {  // fog
                brush->SetColor(D2D1::ColorF(0.01f, 0.02f, 0.05f, q.dark));
                target->FillGeometry(poly.Get(), brush.Get());
            }
            brush->SetColor(
                D2D1::ColorF(0.75f, 0.9f, 1.0f, 0.16f * (1.0f - q.hero)));
            target->DrawGeometry(poly.Get(), brush.Get(), 1.0f);
        }
        const float ringK = q.hero * q.hero * q.hero * q.hero;
        if (ringK > 0.02f) {  // the lit ring of the selected frame
            const D2D1_RECT_F rc{q.xL, q.tL, q.xR, q.bL};
            const float w2 = std::max(2.0f, 3.0f * s);
            brush->SetColor(D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.10f * ringK));
            target->DrawRoundedRectangle({rc, cornerR, cornerR}, brush.Get(),
                                         w2 * 4.0f);
            brush->SetColor(D2D1::ColorF(0.55f, 0.85f, 1.0f, 0.22f * ringK));
            target->DrawRoundedRectangle({rc, cornerR, cornerR}, brush.Get(),
                                         w2 * 2.2f);
            brush->SetColor(D2D1::ColorF(1, 1, 1, ringK));
            target->DrawRoundedRectangle({rc, cornerR, cornerR}, brush.Get(),
                                         w2);
        }
        target->PopLayer();
    }

    const int index = CarIndex(C.target);
    std::wstring name = PathFindFileName(C.paths[index].c_str());
    PathRemoveExtension(&name[0]);
    name.resize(wcslen(name.c_str()));
    const std::wstring label = name + L"   \u00B7   " +
                               std::to_wstring(index + 1) + L" / " +
                               std::to_wstring(CarCount());
    const std::wstring hint = CarHint(
        L"\u2190 \u2191 \u2193 \u2192 / wheel fly     click a frame to "
        L"select, again to set     Enter set");
    float y = CarPill(target, brush.Get(), label, C.labelFormat.Get(),
                      C.tunLabelY, std::round(16 * s), std::round(6 * s),
                      0.6f, 0.95f * open, C.size.cx * 0.55f) +
              std::round(8 * s);
    CarPill(target, brush.Get(), hint, C.hintFormat.Get(), y,
            std::round(14 * s), std::round(4 * s), 0.45f, 0.7f * open,
            C.size.cx * 0.9f);
    C.canvas.End(C.hwnd, C.position,
                 (BYTE)std::clamp(opacity * 255, 0.0f, 255.0f));
}

constexpr float kCoverSlicePx = 4.5f;  // a slice's width (smaller = smoother, heavier)
constexpr float kCoverRefl = 0.42f;    // reflection height (share of the card's)

// A card of the Coverflow style (also while another style changes into it):
// turned about its vertical axis with true perspective, drawn in thin slices,
// with its reflection on the floor under it. It is all drawn in the card's own
// frame and then moved by `turn` (the fan's tilt, the slant's lean), so it
// blends with the other styles.
void CarCoverCard(ID2D1DCRenderTarget* target,
                  ID2D1SolidColorBrush* brush,
                  ID2D1Layer* layer,
                  const CarouselItem& item,
                  const CardPose& pose,
                  const D2D1::Matrix3x2F& turn,
                  float radius,
                  float frameW,
                  float glow,
                  float slant,
                  float places) {
    CarouselState& C = g_car;
    const float cf = (float)C.cfMix;
    const float hw = pose.w / 2, hh = pose.h / 2;
    const float hz = C.cvCy;          // the horizon (eye level)
    const float offY = pose.cy - hz;  // the card's middle, below it
    const float cosY = std::cos(pose.yaw), sinY = std::sin(pose.yaw);
    const float F = 2.4f * C.cvW;  // camera distance
    const D2D1::Matrix3x2F none = D2D1::Matrix3x2F::Identity();
    // A point of the card (x across, y down, from its middle) on the screen.
    auto place = [&](float x, float y, float* sx, float* sy) {
        const float f = F / (F + x * sinY);
        *sx = pose.cx + x * cosY * f;
        *sy = hz + (offY + y) * f;
    };

    // The card's outline (rounded corners, in perspective).
    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(C.d2d->CreatePathGeometry(path.GetAddressOf())) ||
        FAILED(path->Open(sink.GetAddressOf()))) {
        return;
    }
    const float r = std::min(radius, std::min(hw, hh));
    bool first = true;
    for (int c = 0; c < 4; c++) {
        const float cxs = (c == 0 || c == 3) ? 1.0f : -1.0f;
        const float cys = c < 2 ? 1.0f : -1.0f;
        for (int k = 0; k <= 6; k++) {
            const float a = (90.0f * c + 90.0f * k / 6.0f) * kPi / 180.0f;
            float px, py;
            place(cxs * (hw - r) + r * std::cos(a),
                  cys * (hh - r) + r * std::sin(a), &px, &py);
            if (first) {
                sink->BeginFigure(D2D1::Point2F(px, py),
                                  D2D1_FIGURE_BEGIN_FILLED);
                first = false;
            } else {
                sink->AddLine(D2D1::Point2F(px, py));
            }
        }
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    ComPtr<ID2D1TransformedGeometry> clip;
    if (FAILED(C.d2d->CreateTransformedGeometry(path.Get(), &turn,
                                                clip.GetAddressOf()))) {
        return;
    }

    // The picture's part that fills the card.
    float srcX = 0, srcY = 0, srcW = 1, srcH = 1;
    if (item.bitmap) {
        const D2D1_SIZE_F bs = item.bitmap->GetSize();
        const float aspect = pose.w / pose.h;
        if (bs.width / bs.height > aspect) {
            srcH = bs.height;
            srcW = srcH * aspect;
        } else {
            srcW = bs.width;
            srcH = srcW / aspect;
        }
        srcX = (bs.width - srcW) / 2;
        srcY = (bs.height - srcH) / 2;
    }

    const int N =
        std::fabs(sinY) < 0.02f
            ? 1
            : std::clamp((int)std::ceil(pose.w * std::fabs(cosY) /
                                        kCoverSlicePx),
                         4, 160);
    // Thin slices (the picture mapped with perspective); `reflect`: the
    // bottom of the picture, flipped under the card's bottom edge.
    auto slices = [&](bool reflect) {
        if (!item.bitmap) {
            return;
        }
        const int n = reflect && N > 1 ? std::max(3, N / 2) : N;
        float x0, t0, b0, unused;
        place(-hw, -hh, &x0, &t0);
        place(-hw, hh, &unused, &b0);
        for (int j = 0; j < n; j++) {
            const float u0 = (float)j / n, u1 = (float)(j + 1) / n;
            float x1, t1, b1;
            place(-hw + u1 * pose.w, -hh, &x1, &t1);
            place(-hw + u1 * pose.w, hh, &unused, &b1);
            const float w = x1 - x0;
            if (w > 0.01f) {
                const float cm = (t0 + t1 + b0 + b1) * 0.25f;
                const float hm = ((b0 - t0) + (b1 - t1)) * 0.5f;
                const float xm = (x0 + x1) * 0.5f;
                const float sh = ((t1 - t0) + (b1 - b0)) * 0.5f / w;
                // A little overlap hides seams; the picture is widened by the
                // same share so it stays continuous.
                const float over = j + 1 < n ? 1.25f : 0.0f;
                const float uo = std::min(1.0f, u1 + (u1 - u0) * over / w);
                D2D1::Matrix3x2F m(1.0f, sh, 0.0f, 1.0f, 0.0f, -sh * xm);
                float yTop = cm - hm * 0.5f;
                const float yBot = cm + hm * 0.5f;
                float sY0 = srcY;
                if (reflect) {
                    yTop = yBot - hm * kCoverRefl;
                    sY0 = srcY + srcH * (1.0f - kCoverRefl);
                    m = D2D1::Matrix3x2F::Scale(1.0f, -1.0f,
                                                D2D1::Point2F(xm, yBot)) *
                        m;
                }
                target->SetTransform(m * turn);
                const D2D1_RECT_F dst{x0, yTop, x1 + over, yBot};
                const D2D1_RECT_F src{srcX + u0 * srcW, sY0,
                                      srcX + uo * srcW, srcY + srcH};
                target->DrawBitmap(item.bitmap.Get(), dst, 1.0f,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                                   &src);
            }
            x0 = x1;
            t0 = t1;
            b0 = b1;
        }
        target->SetTransform(none);
    };

    // The card.
    target->SetTransform(none);
    target->PushLayer(
        D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get(),
                              D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                              D2D1::IdentityMatrix(), pose.alpha),
        layer);
    if (item.bitmap) {
        slices(false);
    } else {
        target->SetTransform(turn);
        brush->SetColor(D2D1::ColorF(0.10f, 0.11f, 0.14f, 1));
        target->FillGeometry(path.Get(), brush);
        target->SetTransform(none);
    }
    // Shade: the sides are darker, and a turned card gets darker toward its
    // far edge.
    {
        const float sideShade =
            (0.35f + 0.2f * slant) * (1.0f - cf) + 0.18f * cf;
        const float base = (1.0f - pose.closeness) * sideShade;
        const float k = std::min(1.0f, std::fabs(sinY) * 1.2f);
        const float nearA = base * (1.0f - 0.3f * k);
        const float farA = std::min(0.8f, base + 0.30f * std::fabs(sinY));
        const float aL = pose.yaw > 0 ? nearA : farA;
        const float aR = pose.yaw > 0 ? farA : nearA;
        if (aL > 0.005f || aR > 0.005f) {
            const D2D1_GRADIENT_STOP stops[2] = {
                {0.0f, D2D1::ColorF(0.01f * slant, 0.02f * slant,
                                    0.05f * slant, aL)},
                {1.0f, D2D1::ColorF(0.01f * slant, 0.02f * slant,
                                    0.05f * slant, aR)}};
            ComPtr<ID2D1GradientStopCollection> collection;
            ComPtr<ID2D1LinearGradientBrush> shade;
            const float half = hw * std::fabs(cosY);
            if (SUCCEEDED(target->CreateGradientStopCollection(
                    stops, 2, collection.GetAddressOf())) &&
                SUCCEEDED(target->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(pose.cx - half, 0),
                        D2D1::Point2F(pose.cx + half, 0)),
                    collection.Get(), shade.GetAddressOf()))) {
                target->SetTransform(turn);
                target->FillGeometry(path.Get(), shade.Get());
                target->SetTransform(none);
            }
        }
    }
    // The middle card's frame (white, with a soft icy glow).
    if (pose.closeness > 0.05f) {
        const float cr = 1 + (0.55f - 1) * glow;
        const float cg = 1 + (0.85f - 1) * glow;
        target->SetTransform(turn);
        if (glow > 0.01f) {
            brush->SetColor(
                D2D1::ColorF(cr, cg, 1, 0.22f * glow * pose.closeness));
            target->DrawGeometry(path.Get(), brush, frameW * 7.0f);
        }
        brush->SetColor(
            D2D1::ColorF(cr, cg, 1, pose.closeness * pose.closeness));
        target->DrawGeometry(path.Get(), brush, frameW * 2.0f);
        target->SetTransform(none);
    }
    target->PopLayer();

    // The reflection: the bottom of the picture, flipped, fading away down.
    const float refl = cf * pose.alpha * (0.45f + 0.55f * pose.closeness) *
                       0.42f * std::clamp((4.5f - places) / 2.0f, 0.0f, 1.0f);
    if (refl > 0.01f && item.bitmap) {
        const float yb = hz + offY + hh;  // the bottom edge, at the middle
        const D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, D2D1::ColorF(0, 0, 0, refl)},
            {0.45f, D2D1::ColorF(0, 0, 0, refl * 0.30f)},
            {1.0f, D2D1::ColorF(0, 0, 0, 0.0f)}};
        ComPtr<ID2D1GradientStopCollection> collection;
        ComPtr<ID2D1LinearGradientBrush> fade;
        if (SUCCEEDED(target->CreateGradientStopCollection(
                stops, 3, collection.GetAddressOf())) &&
            SUCCEEDED(target->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(0, yb),
                    D2D1::Point2F(0, yb + 2 * hh * kCoverRefl)),
                collection.Get(), fade.GetAddressOf()))) {
            target->SetTransform(none);
            target->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                      D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                      D2D1::IdentityMatrix(), 1.0f,
                                      fade.Get()),
                layer);
            slices(true);
            target->SetTransform(none);
            target->PopLayer();
        }
    }
}

// Three copies of a picture, one colour channel each, with alpha 0: drawn
// over each other they ADD up, so shifting them splits the colours.
void LqBakeChannels(CarouselItem& item, IWICBitmapSource* source) {
    CarouselState& C = g_car;
    UINT w = 0, h = 0;
    if (!C.canvas.target || !source || FAILED(source->GetSize(&w, &h)) ||
        !w || !h) {
        return;
    }
    std::vector<uint32_t> px((size_t)w * h);
    if (FAILED(source->CopyPixels(nullptr, w * 4, (UINT)(px.size() * 4),
                                  (BYTE*)px.data()))) {
        return;
    }
    ComPtr<ID2D1Bitmap>* outs[3] = {&item.lqB, &item.lqG, &item.lqR};
    std::vector<uint32_t> ch(px.size());
    for (int k = 0; k < 3; k++) {
        const int shift = k * 8;  // B, G, R of a BGRA pixel
        for (size_t i = 0; i < px.size(); i++) {
            ch[i] = px[i] & (0xFFu << shift);
        }
        outs[k]->Reset();
        C.canvas.target->CreateBitmap(
            D2D1::SizeU(w, h), ch.data(), w * 4,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f),
            outs[k]->GetAddressOf());
    }
    if (!item.lqR || !item.lqG || !item.lqB) {
        item.lqR.Reset();
        item.lqG.Reset();
        item.lqB.Reset();
    }
}

// One Liquid card (also while another style glides into / out of it). The
// card is cut into horizontal strips; each strip is sheared so that the
// card's edges follow  lean + bend + wave  continuously. The speed of the
// scroll gives the bend (the middle of the card lags behind) and the RGB split.
void LqDrawCard(ID2D1DCRenderTarget* target,
                ID2D1SolidColorBrush* brush,
                ID2D1Layer* layer,
                CarouselItem& item,
                int index,
                const CardPose& pose,
                float d,
                float flatRadius,
                float slant,
                float deck,
                float gk) {
    CarouselState& C = g_car;
    const float s = C.scale;
    const float lm = (float)C.lqMix;
    const float W = pose.w, H = pose.h;
    if (W < 2.0f || H < 2.0f) {
        return;
    }
    const float top = pose.cy - H * 0.5f;
    const float bottom = pose.cy + H * 0.5f;
    const float cy = pose.cy;
    const D2D1::Matrix3x2F none = D2D1::Matrix3x2F::Identity();
    const D2D1::Matrix3x2F rot = D2D1::Matrix3x2F::Rotation(
        pose.angle, D2D1::Point2F(pose.cx, pose.cy));

    // Speed (clamped) -> one smooth C-curve, a barrel pinch, a border fade
    // and the RGB dispersion (all fade out with the style, all 0 when the
    // row is still).
    const float vel = (float)C.lqVel;
    const float velC = std::clamp(vel, -kLqVelMax, kLqVelMax);
    const float bendN = std::tanh(velC / 5.0f) * lm;
    const float chromN = std::min(1.0f, std::fabs(velC) / kLqVelRef) * lm;
    const float chromaPx =
        (velC < 0 ? -1.0f : 1.0f) * chromN * kLqSplit * s;
    // Lens: the dispersion is a bit stronger on the cards farther from the
    // middle of the screen.
    const float lensK =
        0.6f + 0.4f * std::min(1.0f, std::fabs(pose.cx - C.size.cx * 0.5f) /
                                         (C.size.cx * 0.35f));
    // The bend is measured against the MIDDLE card's size and place, the same
    // for every card, so neighbours always shift together and never collide.
    const float bendAmp = kLqBend * C.lqH * bendN;
    const float curveTop = C.lqCy - C.lqH * 0.5f;
    const float skew = pose.skew;
    // How far the card is moved sideways at height y: the lean plus the
    // C-curve, sin(PI * v), nothing else.
    auto shiftAt = [&](float y) {
        const float v = std::clamp((y - curveTop) / C.lqH, 0.0f, 1.0f);
        return skew * (cy - y) + bendAmp * std::sin(kPi * v);
    };

    // Strip boundaries (whole pixels). At rest the shape is a plain
    // parallelogram, so one strip is exact and cheap.
    const bool bending = std::fabs(bendAmp) > 0.5f;
    const int N = bending ? std::clamp((int)std::ceil(H / 24.0f), 8, 18) : 1;
    std::vector<float> yb((size_t)N + 1);
    for (int k = 0; k <= N; k++) {
        yb[(size_t)k] = std::round(top + H * (float)k / (float)N);
    }
    yb[0] = std::floor(top);
    yb[(size_t)N] = std::ceil(bottom);
    for (int k = 1; k <= N; k++) {
        if (yb[(size_t)k] <= yb[(size_t)k - 1]) {
            yb[(size_t)k] = yb[(size_t)k - 1] + 1.0f;
        }
    }

    // The outline: rounded corners, and the sides follow the bend.
    const float r = std::max(
        1.0f, std::min({flatRadius * (1.0f - lm) + std::round(13 * s) * lm,
                        W * 0.5f, H * 0.5f}));
    struct Pt {
        float y, inset;
    };
    std::vector<Pt> pts;
    const int arc = 8;
    for (int i = 0; i <= arc; i++) {
        const float th = kPi * 0.5f * (float)i / (float)arc;
        pts.push_back({top + r * (1.0f - std::cos(th)),
                       r * (1.0f - std::sin(th))});
    }
    for (int k = 1; k < N; k++) {
        const float y = yb[(size_t)k];
        if (y > top + r + 0.5f && y < bottom - r - 0.5f) {
            pts.push_back({y, 0.0f});
        }
    }
    for (int i = arc; i >= 0; i--) {
        const float th = kPi * 0.5f * (float)i / (float)arc;
        pts.push_back({bottom - r * (1.0f - std::cos(th)),
                       r * (1.0f - std::sin(th))});
    }
    const float xlBase = pose.cx - W * 0.5f;
    const float xrBase = pose.cx + W * 0.5f;
    ComPtr<ID2D1PathGeometry> poly;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(C.d2d->CreatePathGeometry(poly.GetAddressOf())) ||
        FAILED(poly->Open(sink.GetAddressOf()))) {
        return;
    }
    // Barrel pinch: the outline is squeezed towards the card's middle line,
    // more the farther it is from the screen's centre (one lens warp for the
    // whole ribbon, so neighbours stay in step). It only moves the outline
    // inwards, so the picture always covers it.
    const float halfScreen = C.size.cx * 0.5f;
    const float pinchK = kLqPinch * std::fabs(bendN);
    auto squeeze = [&](float xn, float y) {
        const float q =
            std::min(1.0f, std::fabs(xn - halfScreen) / halfScreen);
        return cy + (y - cy) * (1.0f - pinchK * q * q);
    };
    const float ua = r / W, ub = 1.0f - r / W;
    const int M = (pinchK > 0.004f && ub > ua)
                      ? std::clamp((int)(W / 40.0f), 2, 10)
                      : 0;
    // Left side down, bottom edge across, right side up, top edge back.
    for (size_t i = 0; i < pts.size(); i++) {
        const float yy = squeeze(xlBase + pts[i].inset, pts[i].y);
        const D2D1_POINT_2F p =
            D2D1::Point2F(xlBase + shiftAt(yy) + pts[i].inset, yy);
        if (i == 0) {
            sink->BeginFigure(p, D2D1_FIGURE_BEGIN_FILLED);
        } else {
            sink->AddLine(p);
        }
    }
    for (int k = 1; k < M; k++) {  // bottom edge, left to right
        const float xn = xlBase + (ua + (ub - ua) * (float)k / M) * W;
        const float yy = squeeze(xn, bottom);
        sink->AddLine(D2D1::Point2F(xn + shiftAt(yy), yy));
    }
    for (size_t i = pts.size(); i-- > 0;) {
        const float yy = squeeze(xrBase - pts[i].inset, pts[i].y);
        sink->AddLine(D2D1::Point2F(
            xrBase + shiftAt(yy) - pts[i].inset, yy));
    }
    for (int k = M - 1; k >= 1; k--) {  // top edge, right to left
        const float xn = xlBase + (ua + (ub - ua) * (float)k / M) * W;
        const float yy = squeeze(xn, top);
        sink->AddLine(D2D1::Point2F(xn + shiftAt(yy), yy));
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    ComPtr<ID2D1TransformedGeometry> clip;
    if (FAILED(C.d2d->CreateTransformedGeometry(poly.Get(), &rot,
                                                clip.GetAddressOf()))) {
        return;
    }

    ID2D1Bitmap* img = item.bitmap.Get();
    const bool split = img && std::fabs(d) < 2.6f &&
                       std::fabs(chromaPx) >= 0.6f && item.lqR && item.lqG &&
                       item.lqB;

    // The card.
    target->SetTransform(none);
    target->PushLayer(
        D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get(),
                              D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                              D2D1::IdentityMatrix(), pose.alpha),
        layer);
    target->SetTransform(rot);
    brush->SetColor(split ? D2D1::ColorF(0, 0, 0, 1)
                          : D2D1::ColorF(0.10f, 0.11f, 0.14f, 1));
    target->FillGeometry(poly.Get(), brush);
    if (img) {
        // The picture keeps one size (cut to fill the card), so a narrow card
        // is a window onto its centre and is never squeezed.
        const D2D1_SIZE_F bs = img->GetSize();
        const float f = std::max(W / bs.width, H / bs.height);
        const float halfSrcW = W * 0.5f / f;
        const float sx0 = std::max(0.0f, bs.width * 0.5f - halfSrcW);
        const float sx1 = std::min(bs.width, bs.width * 0.5f + halfSrcW);
        ID2D1Bitmap* chans[3] = {item.lqR.Get(), item.lqG.Get(),
                                 item.lqB.Get()};
        for (int k = 0; k < N; k++) {
            const float y0 = yb[(size_t)k], y1 = yb[(size_t)k + 1];
            const float S0 = shiftAt(y0), S1 = shiftAt(y1);
            const float sh = (S1 - S0) / (y1 - y0);
            target->SetTransform(
                D2D1::Matrix3x2F(1.0f, 0.0f, sh, 1.0f, S0 - sh * y0, 0.0f) *
                rot);
            const float sy0 =
                std::clamp(bs.height * 0.5f + (y0 - cy) / f, 0.0f, bs.height);
            const float sy1 =
                std::clamp(bs.height * 0.5f + (y1 - cy) / f, 0.0f, bs.height);
            if (sy1 - sy0 < 0.01f) {
                continue;
            }
            const D2D1_RECT_F src{sx0, sy0, sx1, sy1};
            if (!split) {
                target->DrawBitmap(img,
                                   D2D1::RectF(xlBase, y0, xrBase, y1), 1.0f,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                                   &src);
            } else {
                // The offset follows the arc's slope (steepest near the top
                // and bottom, where the edges bend most) and the lens factor,
                // so the fringe curves with the card and is 0 when still.
                const float vm = std::clamp(
                    ((y0 + y1) * 0.5f - curveTop) / C.lqH, 0.0f, 1.0f);
                const float o =
                    chromaPx * (0.30f + 0.70f * std::fabs(std::cos(kPi * vm))) *
                    lensK;
                const float offs[3] = {-o, 0.0f, o};
                for (int j = 0; j < 3; j++) {
                    target->DrawBitmap(
                        chans[j],
                        D2D1::RectF(xlBase + offs[j], y0, xrBase + offs[j], y1),
                        1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src);
                }
            }
        }
    }
    // The side cards are darker.
    {
        const float depth = std::clamp(std::fabs(d) - 1.0f, 0.0f, 4.0f);
        const float darkA = std::min(
            0.85f, (1.0f - pose.closeness) *
                           (0.35f * (1.0f - lm) + 0.50f * lm + 0.2f * slant) +
                       0.10f * deck * depth);
        if (darkA > 0.005f) {
            target->SetTransform(rot);
            brush->SetColor(D2D1::ColorF(0.01f, 0.02f, 0.05f, darkA));
            target->FillGeometry(poly.Get(), brush);
        }
    }
    target->SetTransform(none);
    target->PopLayer();

    // The thin frame of the middle card.
    if (pose.closeness > 0.05f) {
        // The border fades out while the row whips and returns as it settles.
        const float vf = std::min(1.0f, std::fabs(vel) / kLqBorderVel);
        const float borderK = 1.0f - lm * vf * vf * (3.0f - 2.0f * vf);
        const float k =
            pose.closeness * pose.closeness * pose.alpha * borderK;
        const float flatFrame = std::max(
            2.0f - 0.5f * slant, std::round(3 * s * (1.0f - 0.5f * slant)));
        const float fw = flatFrame * (1.0f - lm) + std::max(1.5f, 1.5f * s) * lm;
        const float cr = 1 + (0.55f - 1) * gk;
        const float cg = 1 + (0.85f - 1) * gk;
        target->SetTransform(rot);
        if (gk > 0.01f) {
            brush->SetColor(D2D1::ColorF(cr, cg, 1, 0.22f * gk * k));
            target->DrawGeometry(poly.Get(), brush, fw * 3.5f);
        }
        brush->SetColor(D2D1::ColorF(cr, cg, 1, k));
        target->DrawGeometry(poly.Get(), brush, fw);
        target->SetTransform(none);
    }

    // File name (bottom left) and type badge (top left) on the middle card.
    const float lc = std::clamp((pose.closeness - 0.5f) * 2.0f, 0.0f, 1.0f);
    const float la = lc * lc * pose.alpha * lm;
    if (la > 0.02f && C.chipSans && C.chipSmall && index >= 0 &&
        index < CarCount()) {
        const std::wstring& path = C.paths[index];
        const std::wstring name = PathFindFileName(path.c_str());
        const float mg = std::round(14 * s);
        const float padX = std::round(9 * s);
        const float padY = std::round(4 * s);
        const float maxTextW = std::max(20.0f, W * 0.7f - mg);
        target->SetTransform(rot);
        ComPtr<IDWriteTextLayout> nameL;
        C.dwrite->CreateTextLayout(name.c_str(), (UINT32)name.size(),
                                   C.chipSans.Get(), maxTextW, 100.0f,
                                   nameL.GetAddressOf());
        if (nameL) {
            DWRITE_TEXT_METRICS tm{};
            nameL->GetMetrics(&tm);
            const float ph = tm.height + 2 * padY;
            const float py1 = bottom - mg;
            const float px0 = xlBase + shiftAt(py1 - ph * 0.5f) + mg;
            const D2D1_RECT_F rc{px0, py1 - ph,
                                 px0 + std::min(tm.width, maxTextW) + 2 * padX,
                                 py1};
            brush->SetColor(D2D1::ColorF(0.02f, 0.02f, 0.03f, 0.62f * la));
            target->FillRoundedRectangle({rc, ph / 2, ph / 2}, brush);
            brush->SetColor(D2D1::ColorF(1, 1, 1, 0.92f * la));
            target->DrawTextLayout({px0 + padX, py1 - ph + padY}, nameL.Get(),
                                   brush);
        }
        PCWSTR dot = PathFindExtension(path.c_str());
        std::wstring ext = dot && *dot ? dot + 1 : L"";
        for (auto& ch : ext) {
            ch = (wchar_t)towupper(ch);
        }
        ComPtr<IDWriteTextLayout> extL;
        if (!ext.empty()) {
            C.dwrite->CreateTextLayout(ext.c_str(), (UINT32)ext.size(),
                                       C.chipSmall.Get(), 200.0f, 50.0f,
                                       extL.GetAddressOf());
        }
        if (extL) {
            DWRITE_TEXT_METRICS tm{};
            extL->GetMetrics(&tm);
            float cr = 0.7f, cg = 0.7f, cb = 0.75f;
            if (ext == L"PNG") {
                cr = 0.96f, cg = 0.42f, cb = 0.46f;
            } else if (ext == L"JPG" || ext == L"JPEG" || ext == L"JFIF") {
                cr = 0.40f, cg = 0.70f, cb = 0.98f;
            } else if (ext == L"WEBP") {
                cr = 0.45f, cg = 0.88f, cb = 0.58f;
            }
            const float bpx = std::round(6 * s), bpy = std::round(2 * s);
            const float bh = tm.height + 2 * bpy;
            const float by = top + mg;
            const float bx = xlBase + shiftAt(by + bh * 0.5f) + mg;
            const D2D1_RECT_F rc{bx, by, bx + tm.width + 2 * bpx, by + bh};
            brush->SetColor(D2D1::ColorF(0.02f, 0.02f, 0.03f, 0.55f * la));
            target->FillRoundedRectangle(
                {rc, std::round(4 * s), std::round(4 * s)}, brush);
            brush->SetColor(D2D1::ColorF(cr, cg, cb, la));
            target->DrawTextLayout({bx + bpx, by + bpy}, extL.Get(), brush);
        }
        target->SetTransform(none);
    }
}

void CarDraw(float opacity) {
    CarouselState& C = g_car;
    if (C.hex) {
        CarDrawHex(opacity);
        return;
    }
    if (C.spiral) {
        CarDrawSpiral(opacity);
        return;
    }
    if (C.fish) {
        CarDrawFish(opacity);
        return;
    }
    if (C.tun) {
        CarDrawTunnel(opacity);
        return;
    }
    if (!C.hwnd || !C.canvas.Begin(C.size)) {
        return;
    }
    ID2D1DCRenderTarget* target = C.canvas.target.Get();
    ComPtr<ID2D1SolidColorBrush> brush;
    ComPtr<ID2D1Layer> layer;
    target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1),
                                  brush.GetAddressOf());
    target->CreateLayer(nullptr, layer.GetAddressOf());
    if (!brush || !layer) {
        C.canvas.End(C.hwnd, C.position, 0);
        return;
    }
    // The fan opens: cards rise and spread out a little while fading in.
    C.spread = 0.7f + 0.3f * opacity;
    const float s = C.scale;
    const float m = (float)C.styleMix;
    const float slant = (float)C.slantMix;
    const float deck = (float)C.dkMix;
    const float flow = (float)C.flMix;
    // 1 only in the Cards style (narrow layout, no lean).
    const float cardsK = std::clamp(slant - (float)C.leanMix, 0.0f, 1.0f);
    const float baseRadius = std::round(10 * s);
    const D2D1_MATRIX_3X2_F none = D2D1::Matrix3x2F::Identity();

    // Coverflow: a soft glow on the floor under the middle card.
    if (C.cfMix > 0.01) {
        const float k = (float)C.cfMix * opacity;
        const D2D1_POINT_2F mid =
            D2D1::Point2F(C.size.cx / 2.0f, C.cvCy + C.cvH / 2);
        const float rx = C.cvW * 0.95f, ry = C.cvH * 0.22f;
        const D2D1_GRADIENT_STOP stops[2] = {
            {0.0f, D2D1::ColorF(0.50f, 0.78f, 1.0f, 0.22f * k)},
            {1.0f, D2D1::ColorF(0.50f, 0.78f, 1.0f, 0.0f)}};
        ComPtr<ID2D1GradientStopCollection> collection;
        ComPtr<ID2D1RadialGradientBrush> floorGlow;
        if (SUCCEEDED(target->CreateGradientStopCollection(
                stops, 2, collection.GetAddressOf())) &&
            SUCCEEDED(target->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(
                    mid, D2D1::Point2F(0, 0), rx, ry),
                collection.Get(), floorGlow.GetAddressOf()))) {
            target->FillEllipse(D2D1::Ellipse(mid, rx, ry), floorGlow.Get());
        }
    }

    // From the farthest to the nearest, so the middle one is over the rest.
    const int base = (int)std::floor(C.scroll);
    std::vector<std::pair<float, int>> order;
    for (int v = base - 6; v <= base + 7; v++) {
        if (CarValid(v)) {
            order.push_back({(float)std::fabs(v - C.scroll), v});
        }
    }
    std::sort(order.begin(), order.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Liquid: the whole row fades out toward the left / right edges (the fade
    // grows with the style, so changing style stays smooth).
    ComPtr<ID2D1Layer> edgeLayer;
    ComPtr<ID2D1GradientStopCollection> edgeStops;
    ComPtr<ID2D1LinearGradientBrush> edgeBrush;
    bool edgeFade = false;
    if (C.lqMix > 0.01) {
        const float ea = 1.0f - (float)C.lqMix;
        const D2D1_GRADIENT_STOP stops[6] = {
            {0.0f, D2D1::ColorF(0, 0, 0, ea)},
            {0.05f, D2D1::ColorF(0, 0, 0, ea)},
            {0.20f, D2D1::ColorF(0, 0, 0, 1.0f)},
            {0.80f, D2D1::ColorF(0, 0, 0, 1.0f)},
            {0.95f, D2D1::ColorF(0, 0, 0, ea)},
            {1.0f, D2D1::ColorF(0, 0, 0, ea)}};
        if (SUCCEEDED(target->CreateGradientStopCollection(
                stops, 6, edgeStops.GetAddressOf())) &&
            SUCCEEDED(target->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(0, 0), D2D1::Point2F((float)C.size.cx, 0)),
                edgeStops.Get(), edgeBrush.GetAddressOf())) &&
            SUCCEEDED(target->CreateLayer(nullptr,
                                          edgeLayer.GetAddressOf()))) {
            target->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                      D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                      D2D1::IdentityMatrix(), 1.0f,
                                      edgeBrush.Get()),
                edgeLayer.Get());
            edgeFade = true;
        }
    }
    for (const auto& entry : order) {
        const int v = entry.second;
        const CardPose pose = CarPose((float)(v - C.scroll));
        const float rad = pose.angle * kPi / 180;
        const float halfX = std::fabs(pose.w / 2 * std::cos(rad)) +
                            std::fabs(pose.h / 2 * std::sin(rad)) +
                            std::fabs(pose.skew * pose.h / 2);
        if (pose.alpha < 0.01f || pose.cx + halfX < 0 ||
            pose.cx - halfX > C.size.cx) {
            continue;
        }
        CarouselItem& item = C.items[CarIndex(v)];
        const D2D1_RECT_F rect{pose.cx - pose.w / 2, pose.cy - pose.h / 2,
                               pose.cx + pose.w / 2, pose.cy + pose.h / 2};
        // (Slant cards have nearly square corners.)
        const float radius =
            baseRadius *
            (1.0f - 0.7f * std::clamp(pose.skew / kSlantK, 0.0f, 1.0f));
        const D2D1_ROUNDED_RECT rounded{rect, radius, radius};
        // Lean (shear around the card's middle line), then turn.
        const D2D1::Matrix3x2F lean(1.0f, 0.0f, -pose.skew, 1.0f,
                                    pose.skew * pose.cy, 0.0f);
        // `straight` only turns (for the pictures); `turn` also leans (for
        // the card's shape, its shade and its frame).
        const D2D1::Matrix3x2F straight = D2D1::Matrix3x2F::Rotation(
            pose.angle, D2D1::Point2F(pose.cx, pose.cy));
        const D2D1_MATRIX_3X2_F turn = lean * straight;

        // A soft shadow under the card (fan and deck).
        const float shadowK = std::max({m, deck, flow});
        if (shadowK > 0.01f) {
            target->SetTransform(turn);
            for (int i = 3; i >= 1; i--) {
                const float grow = i * 4 * s;
                brush->SetColor(
                    D2D1::ColorF(0, 0, 0, 0.10f * shadowK * pose.alpha));
                target->FillRoundedRectangle(
                    {{rect.left - grow, rect.top - grow + 6 * s,
                      rect.right + grow, rect.bottom + grow + 6 * s},
                     radius + grow,
                     radius + grow},
                    brush.Get());
            }
            target->SetTransform(none);
        }

        // Coverflow (or on its way): the card turned in perspective, with its
        // reflection.
        const bool persp = C.cfMix > 0.002 || std::fabs(pose.yaw) > 0.002f;
        if (persp) {
            CarCoverCard(target, brush.Get(), layer.Get(), item, pose,
                         lean * straight, radius,
                         std::max(2.0f - 0.5f * slant,
                                  std::round(3 * s * (1.0f - 0.5f * slant))),
                         std::max(m, (float)C.cfMix), slant,
                         (float)std::fabs(v - C.scroll));
        } else if (C.lqMix > 0.002) {
            // Liquid (or on its way): bends and splits with the scroll's speed.
            LqDrawCard(target, brush.Get(), layer.Get(), item, CarIndex(v),
                       pose, (float)(v - C.scroll), radius, slant, deck,
                       std::max({m, deck, flow}));
        } else {
        // The card, cut to its rounded (and turned) shape.
        ComPtr<ID2D1RoundedRectangleGeometry> clip;
        ComPtr<ID2D1TransformedGeometry> clipTurned;
        if (FAILED(C.d2d->CreateRoundedRectangleGeometry(
                rounded, clip.GetAddressOf())) ||
            FAILED(C.d2d->CreateTransformedGeometry(
                clip.Get(), &turn, clipTurned.GetAddressOf()))) {
            continue;
        }
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), clipTurned.Get(),
                                  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                  D2D1::IdentityMatrix(), pose.alpha),
            layer.Get());
        target->SetTransform(turn);
        brush->SetColor(D2D1::ColorF(0.1f, 0.1f, 0.12f, 1));
        target->FillRectangle(rect, brush.Get());
        if (item.bitmap) {
            // Cut to fill. The picture stays straight: only the card's shape
            // leans, so it must cover the leaning frame's full width.
            const float reachW = pose.w + std::fabs(pose.skew) * pose.h;
            const D2D1_SIZE_F size = item.bitmap->GetSize();
            const float scale = std::max(reachW / size.width,
                                         pose.h / size.height);
            const float w = size.width * scale;
            const float h = size.height * scale;
            target->SetTransform(straight);
            target->DrawBitmap(item.bitmap.Get(),
                               {pose.cx - w / 2, pose.cy - h / 2,
                                pose.cx + w / 2, pose.cy + h / 2},
                               1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            target->SetTransform(turn);
        }
        // The ones at the sides are a bit darker (in the deck, the deeper in
        // the stack, the darker).
        if (pose.closeness < 1) {
            const float depth = std::clamp(
                (float)std::fabs(v - C.scroll) - 1.0f, 0.0f, 4.0f);
            brush->SetColor(D2D1::ColorF(
                0.01f * slant, 0.02f * slant, 0.06f * slant,
                std::min(0.85f, (1 - pose.closeness) *
                                        (0.35f + 0.2f * slant) +
                                    0.10f * deck * depth)));
            target->FillRectangle(rect, brush.Get());
        }
        if (deck > 0.01f) {
            // A glassy highlight along the top, and a fine light edge.
            const D2D1_GRADIENT_STOP gs[2] = {
                {0.0f, D2D1::ColorF(1, 1, 1, 0.13f * deck *
                                                 (0.6f + 0.4f * pose.closeness))},
                {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
            ComPtr<ID2D1GradientStopCollection> gloss;
            ComPtr<ID2D1LinearGradientBrush> sheen;
            if (SUCCEEDED(target->CreateGradientStopCollection(
                    gs, 2, gloss.GetAddressOf())) &&
                SUCCEEDED(target->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, rect.top),
                        D2D1::Point2F(0, rect.top + pose.h * 0.45f)),
                    gloss.Get(), sheen.GetAddressOf()))) {
                target->FillRectangle(rect, sheen.Get());
            }
            brush->SetColor(D2D1::ColorF(1, 1, 1, 0.14f * deck *
                                                      (1 - pose.closeness)));
            target->DrawRoundedRectangle({rect, radius, radius}, brush.Get(),
                                         1.0f);
        }
        // The middle one has a frame (white in the strip, icy blue in the
        // fan, with a soft glow).
        if (pose.closeness > 0.05f) {
            const float frame = std::max(
                2.0f - 0.5f * slant,
                std::round(3 * s * (1.0f - 0.5f * slant)));
            const D2D1_RECT_F inner{rect.left + frame / 2, rect.top + frame / 2,
                                    rect.right - frame / 2,
                                    rect.bottom - frame / 2};
            const float gk = std::max({m, deck, flow});
            const float cr = 1 + (0.55f - 1) * gk;
            const float cg = 1 + (0.85f - 1) * gk;
            if (gk > 0.01f) {
                brush->SetColor(D2D1::ColorF(cr, cg, 1,
                                             0.22f * gk * pose.closeness));
                target->DrawRoundedRectangle({inner, radius, radius},
                                             brush.Get(), frame * 3.5f);
            }
            brush->SetColor(D2D1::ColorF(cr, cg, 1,
                                         pose.closeness * pose.closeness));
            target->DrawRoundedRectangle({inner, radius, radius}, brush.Get(),
                                         frame);
        }
        target->SetTransform(none);
        target->PopLayer();
        }  // (the flat card)
        // Cards style: the file info sits on the card that is in the middle.
        if (cardsK > 0.01f && pose.closeness > 0.5f) {
            const float c = (pose.closeness - 0.5f) * 2.0f;
            CarChip(target, brush.Get(), rect, CarIndex(v),
                    cardsK * pose.alpha * c * c);
        }
    }

    if (edgeFade) {
        target->PopLayer();
    }
    // The name and the place in the row, and the keys.
    const int index = CarIndex(C.target);
    std::wstring name = PathFindFileName(C.paths[index].c_str());
    PathRemoveExtension(&name[0]);
    name.resize(wcslen(name.c_str()));
    const std::wstring label = name + L"   \u00B7   " +
                               std::to_wstring(index + 1) + L" / " +
                               std::to_wstring(CarCount());
    const std::wstring hint =
        std::wstring(L"\u2190 \u2192 browse     Enter set     T transition: ") +
        kTransitionNames[g_cfg.transition] +
        (UsesSubject(g_cfg.transition)
             ? L"     U subject     Del forget"
             : L"") +
        L"     S style: " + kPickerStyleNames[g_cfg.pickerStyle] +
        L"     H full-screen styles     F folder     Esc close";
    const float stripY = C.centerY + C.bigH / 2 + std::round(14 * s);
    float y = stripY + (C.fanLabelY - stripY) * m;
    y += (C.dkLabelY - y) * (float)C.dkMix;  // Deck: just under the stack
    y += (C.cvLabelY - y) * (float)C.cfMix;  // Coverflow: under the reflection
    y += (C.lqLabelY - y) * (float)C.lqMix;  // Liquid: under the big card
    y = CarPill(target, brush.Get(), label, C.labelFormat.Get(), y,
                std::round(16 * s), std::round(6 * s), 0.6f, 0.95f,
                C.size.cx * 0.55f) +
        std::round(8 * s);
    CarPill(target, brush.Get(), hint, C.hintFormat.Get(), y,
            std::round(14 * s), std::round(4 * s), 0.45f, 0.7f,
            C.size.cx * 0.8f);
    C.canvas.End(C.hwnd, C.position,
                 (BYTE)std::clamp(opacity * 255, 0.0f, 255.0f));
}

// Moving, fading in: drawn at each tick of the timer until it settles.
void CarTick() {
    CarouselState& C = g_car;
    if (!C.hwnd) {
        return;
    }
    const double now = NowSeconds();
    const double dt = std::min(0.05, now - C.lastTick);
    C.lastTick = now;
    const double diff = C.target - C.scroll;
    bool moving = std::fabs(diff) > 0.002;
    if (C.lqTarget > 0.5 || C.lqMix > 0.01) {
        // Liquid: an elastic spring (a little under-damped, so it settles with
        // a small overshoot). Its speed bends the cards and splits the colours.
        const double omega = 13.0, zeta = 0.72;
        const int steps = 4;
        const double h = dt / steps;
        for (int k = 0; k < steps; k++) {
            const double accel =
                omega * omega * ((double)C.target - C.scroll) -
                2.0 * zeta * omega * C.lqVel;
            C.lqVel += accel * h;
            C.scroll += C.lqVel * h;
        }
        C.lqClock += dt;
        if (std::fabs((double)C.target - C.scroll) < 0.0008 &&
            std::fabs(C.lqVel) < 0.01) {
            C.scroll = C.target;
            C.lqVel = 0;
            moving = false;
        } else {
            moving = true;
        }
    } else {
        C.lqVel = 0;
        if (moving) {
            C.scroll += diff * (1 - std::exp(-dt * 16));
        } else {
            C.scroll = C.target;
        }
    }
    // Changing style: the layout slides from one to the other.
    const double mixDiff = C.mixTarget - C.styleMix;
    const bool mixing = std::fabs(mixDiff) > 0.002;
    if (mixing) {
        C.styleMix += mixDiff * (1 - std::exp(-dt * 9));
    } else {
        C.styleMix = C.mixTarget;
    }
    const double slantDiff = C.slantTarget - C.slantMix;
    const bool slanting = std::fabs(slantDiff) > 0.002;
    if (slanting) {
        C.slantMix += slantDiff * (1 - std::exp(-dt * 9));
    } else {
        C.slantMix = C.slantTarget;
    }
    const double leanDiff = C.leanTarget - C.leanMix;
    const bool leaning = std::fabs(leanDiff) > 0.002;
    if (leaning) {
        C.leanMix += leanDiff * (1 - std::exp(-dt * 9));
    } else {
        C.leanMix = C.leanTarget;
    }
    // Changing to / from Coverflow: the turn, the reflection and the layout
    // glide.
    const double cfDiff = C.cfTarget - C.cfMix;
    const bool covering = std::fabs(cfDiff) > 0.002;
    if (covering) {
        C.cfMix += cfDiff * (1 - std::exp(-dt * 8));
    } else {
        C.cfMix = C.cfTarget;
    }
    // Changing to / from the deck: the stack spreads, tilts and darkens.
    const double dkDiff = C.dkTarget - C.dkMix;
    const bool decking = std::fabs(dkDiff) > 0.002;
    if (decking) {
        C.dkMix += dkDiff * (1 - std::exp(-dt * 8));
    } else {
        C.dkMix = C.dkTarget;
    }
    // Changing to / from the drum: the ring turns in / out.
    const double drDiff = C.flTarget - C.flMix;
    const bool flowing = std::fabs(drDiff) > 0.002;
    if (flowing) {
        C.flMix += drDiff * (1 - std::exp(-dt * 8));
    } else {
        C.flMix = C.flTarget;
    }
    // Changing to / from Liquid: the layout, the lean and the fade glide.
    const double lqDiff = C.lqTarget - C.lqMix;
    const bool liquidMix = std::fabs(lqDiff) > 0.002;
    if (liquidMix) {
        C.lqMix += lqDiff * (1 - std::exp(-dt * 7));
    } else {
        C.lqMix = C.lqTarget;
    }
    // Changing between the grid and the other styles: the picture fades out,
    // the window changes while nothing shows, then it fades back in.
    float fadeOut = 1.0f;
    bool switching = false;
    if (C.switchTo >= 0) {
        const double q = (now - C.switchStart) / 0.12;
        if (q < 1.0) {
            fadeOut = (float)(1.0 - q);
            switching = true;
        } else {
            CarSetMode(C.switchTo);
            C.switchTo = -1;
            C.openStart = now;
            C.openLen = 0.22;
        }
    }
    // The honeycomb: it glides to the scroll target (damped, so the wheel
    // feels inertial), every hexagon lights up smoothly, and the pictures
    // pop into their slots one after another.
    bool hexBusy = false;
    if (C.hex) {
        const double scrollDiff = C.hexScrollTarget - C.hexScroll;
        if (std::fabs(scrollDiff) > 0.002) {
            C.hexScroll += scrollDiff * (1 - std::exp(-dt * 7));
            hexBusy = true;
            // While wheel-scrolling, the hexagon under the resting mouse
            // lights up as columns pass by.
            if (C.hexWheel && C.mouseLast.x >= 0) {
                const int hit =
                    HexAt((float)C.mouseLast.x, (float)C.mouseLast.y);
                if (hit >= 0 && hit != C.target) {
                    C.target = hit;
                    C.scroll = hit;
                }
            }
        } else {
            C.hexScroll = C.hexScrollTarget;
        }
        C.hexClock += dt;
        if (C.hexClock < 1.2) {
            hexBusy = true;
        }
        const int perCol = std::max(1, C.hexPerCol);
        const float k = (float)(1 - std::exp(-dt * 16));
        for (int i = 0; i < (int)C.hexAmt.size() &&
                        i < (int)C.hexShow.size(); i++) {
            const float goal = i == C.target ? 1.0f : 0.0f;
            float& a = C.hexAmt[i];
            if (std::fabs(goal - a) > 0.004f) {
                a += (goal - a) * k;
                hexBusy = true;
            } else {
                a = goal;
            }
            float& sh = C.hexShow[i];
            if (C.items[i].hexBmp) {
                // Ripple out from the middle: farther columns pop in later.
                const float delay = std::min(
                    0.8f, (float)(std::fabs(i / perCol - C.hexScroll) * 0.05) +
                              (i % perCol) * 0.03f);
                if (sh < 1.0f) {
                    if (C.hexClock >= delay) {
                        sh = std::min(1.0f, sh + (float)dt * 4.5f);
                    }
                    hexBusy = true;
                }
            } else {
                sh = 0.0f;
            }
        }
    }
    // The spiral: the row glides to the selected picture as a spring (soft
    // start and stop, no overshoot); the opening blooms.
    if (C.spiral) {
        const double omega = 11.0;  // higher = snappier
        const int steps = 3;
        const double h = dt / steps;
        for (int k = 0; k < steps; k++) {
            const double accel =
                omega * omega * ((double)C.target - C.spScroll) -
                2.0 * omega * C.spVel;
            C.spVel += accel * h;
            C.spScroll += C.spVel * h;
        }
        if (std::fabs((double)C.target - C.spScroll) < 0.0006 &&
            std::fabs(C.spVel) < 0.004) {
            C.spScroll = C.target;
            C.spVel = 0;
        } else {
            hexBusy = true;
        }
        C.spClock += dt;
        if (C.spClock < 1.0) {
            hexBusy = true;
        }
    }
    // The fisheye grid: the swelling focus and the scroll are springs; the
    // pictures pop into their tiles one after another.
    if (C.fish) {
        double gc = 0, gr = 0;
        FishFocusGoal(&gc, &gr);
        SpringStep(C.fishFc, C.fishFcV, gc, 9.0, dt);
        SpringStep(C.fishFr, C.fishFrV, gr, 9.0, dt);
        if (SpringSettled(C.fishFc, C.fishFcV, gc, 0.003, 0.01) &&
            SpringSettled(C.fishFr, C.fishFrV, gr, 0.003, 0.01)) {
            C.fishFc = gc;
            C.fishFr = gr;
            C.fishFcV = C.fishFrV = 0;
        } else {
            hexBusy = true;
        }
        SpringStep(C.fishScroll, C.fishVel, C.fishScrollTarget, 8.0, dt);
        if (SpringSettled(C.fishScroll, C.fishVel, C.fishScrollTarget)) {
            C.fishScroll = C.fishScrollTarget;
            C.fishVel = 0;
        } else {
            hexBusy = true;
        }
        C.fishClock += dt;
        if (C.fishClock < 1.2) {
            hexBusy = true;
        }
        const int perRow = std::max(1, C.fishCols);
        for (int i = 0; i < (int)C.fishShow.size() &&
                        i < (int)C.items.size(); i++) {
            float& sh = C.fishShow[i];
            if (C.items[i].fishBmp) {
                const float delay = std::min(
                    0.8f, (float)(std::fabs(i / perRow - C.fishScroll) * 0.06) +
                              (i % perRow) * 0.03f);
                if (sh < 1.0f) {
                    if (C.fishClock >= delay) {
                        sh = std::min(1.0f, sh + (float)dt * 3.0f);
                    }
                    hexBusy = true;
                }
            } else {
                sh = 0.0f;
            }
        }
    }
    // The tunnel: the camera glides to the selected frame as a spring; the
    // opening flies in.
    if (C.tun) {
        SpringStep(C.tunScroll, C.tunVel, (double)C.target, 10.0, dt);
        if (SpringSettled(C.tunScroll, C.tunVel, (double)C.target)) {
            C.tunScroll = C.target;
            C.tunVel = 0;
        } else {
            hexBusy = true;
        }
        C.tunClock += dt;
        if (C.tunClock < 1.0) {
            hexBusy = true;
        }
    }
    const double p = std::clamp((now - C.openStart) / C.openLen, 0.0, 1.0);
    CarDraw((float)(1 - std::pow(1 - p, 3)) * fadeOut);
    if (!moving && !mixing && !slanting && !leaning && !covering &&
        !decking && !flowing && !liquidMix && !hexBusy && !C.ambBusy &&
        !switching && p >= 1) {
        KillTimer(C.hwnd, 1);
        C.timer = false;
    }
}

void CarKick() {
    CarouselState& C = g_car;
    if (C.hwnd && !C.timer) {
        C.timer = true;
        C.lastTick = NowSeconds();
        SetTimer(C.hwnd, 1, 8, nullptr);
    }
}

void ClosePanel() {
    if (g_car.hwnd) {
        DestroyWindow(g_car.hwnd);
    }
}

void CarMove(int delta) {
    CarouselState& C = g_car;
    if (C.hex) {
        HexSelect(C.target + delta);
        return;
    }
    if (C.spiral) {
        int target = C.target + delta;
        if (!C.spWrap) {
            target = std::clamp(target, 0, CarCount() - 1);
        }
        if (target == C.target) {
            return;
        }
        C.target = target;
        C.scroll = target;
        // A long jump only glides the last few places.
        C.spScroll = std::clamp(C.spScroll, (double)target - 12.0,
                                (double)target + 12.0);
        CarRequest();
        CarKick();
        return;
    }
    if (C.fish) {
        C.fishMouse = false;  // the keyboard takes the focus back
        FishSelect(C.target + delta, true);
        return;
    }
    if (C.tun) {
        int tt = C.target + delta;
        if (!C.tunWrap) {
            tt = std::clamp(tt, 0, CarCount() - 1);
        }
        if (tt == C.target) {
            return;
        }
        C.target = tt;
        C.scroll = tt;
        // A long jump only flies the last few places.
        C.tunScroll = std::clamp(C.tunScroll, (double)tt - 12.0,
                                 (double)tt + 12.0);
        CarRequest();
        CarKick();
        return;
    }
    int target = C.target + delta;
    if (!C.wrap) {
        target = std::clamp(target, 0, CarCount() - 1);
    }
    if (target == C.target) {
        return;
    }
    C.target = target;
    if (C.lqTarget > 0.5) {
        C.scroll = std::clamp(C.scroll, (double)target - 12.0,
                              (double)target + 12.0);
    }
    CarRequest();
    CarKick();
}

// The middle picture comes in as the wallpaper. With the ink transition it
// uses the subject remembered for it, or asks for one (`chooseSubject`: asks
// even if there is one).
void CarApply(bool chooseSubject = false) {
    if (g_busy > 0) {
        return;
    }
    const std::wstring path = g_car.paths[CarIndex(g_car.target)];
    std::wstring subject;
    if (UsesSubject(g_cfg.transition)) {
        // Enter never asks: it uses the remembered subject, or ink drops.
        // The dialog only opens when you press U.
        if (!chooseSubject) {
            subject = InkSubjectFor(path);
        }
        if (chooseSubject) {
            // (The file dialog takes the focus: the picker closes first.)
            ClosePanel();
            g_pendingWallpaper = path;
            PostMessage(g_hwnd, WM_APP_PICKSUBJECT, 0, 0);
            return;
        }
    }
    ClosePanel();
    ApplyWallpaper(path, true, subject);
}

// The last non-hexagon style, so H / S can come back to it from the grid
// (main thread only).
int g_bandStyle = 0;
int g_fullStyle = (int)PickerStyle::Hex;

// Applies a picker style (0 strip, 1 fan, 2 slant, 3 cards, 4 hex).
void CarSetStyle(int style) {
    CarouselState& C = g_car;
    g_cfg.pickerStyle = style;
    Wh_SetIntValue(L"pickedStyle", style);
    C.mixTarget = style == (int)PickerStyle::Fan ? 1.0 : 0.0;
    C.slantTarget = style == (int)PickerStyle::Slant ||
                            style == (int)PickerStyle::Cards
                        ? 1.0
                        : 0.0;
    C.leanTarget = style == (int)PickerStyle::Slant ? 1.0 : 0.0;
    C.cfTarget = style == (int)PickerStyle::Coverflow ? 1.0 : 0.0;
    C.dkTarget = style == (int)PickerStyle::Deck ? 1.0 : 0.0;
    C.flTarget = style == (int)PickerStyle::Flow ? 1.0 : 0.0;
    C.lqTarget = style == (int)PickerStyle::Liquid ? 1.0 : 0.0;
    if (style == (int)PickerStyle::Liquid) {
        CarRequest();  // (the pictures already read get their colour copies)
    }
    CarSwitchMode(style == (int)PickerStyle::Hex       ? 1
                  : style == (int)PickerStyle::Spiral  ? 2
                  : style == (int)PickerStyle::Fisheye ? 3
                  : style == (int)PickerStyle::Tunnel  ? 4
                                                       : 0);
    CarKick();
}

LRESULT CALLBACK CarouselProc(HWND hwnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam) {
    CarouselState& C = g_car;
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_TIMER:
            CarTick();
            // Wait for the screen's next refresh, so frames are evenly spaced.
            if (C.timer) {
                DwmFlush();
            }
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                DestroyWindow(hwnd);
            }
            return 0;
        case WM_MOUSEWHEEL: {
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (C.fish) {
                C.fishScrollTarget = std::clamp(
                    C.fishScrollTarget - (double)delta / WHEEL_DELTA, 0.0,
                    (double)(C.fishRows - 1));
                CarRequest();
                CarKick();
                return 0;
            }
            if (C.hex) {
                // Horizontal, smooth scrolling (one notch = one column;
                // touchpads scroll in proportion to their small deltas).
                const double cols = -(double)delta / WHEEL_DELTA;
                C.hexScrollTarget = std::clamp(
                    C.hexScrollTarget + cols, 0.0,
                    (double)std::max(0, C.hexCols - 1));
                C.hexWheel = true;
                CarRequest();
                CarKick();
                return 0;
            }
            CarMove(delta < 0 ? 1 : -1);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (C.fish) {
                const POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (C.fishDrag) {
                    if (!C.fishDragMoved &&
                        std::fabs((float)p.y - C.fishDragY) > 6.0f) {
                        C.fishDragMoved = true;
                    }
                    if (C.fishDragMoved) {
                        C.fishScrollTarget = std::clamp(
                            C.fishDragScroll -
                                (double)(p.y - C.fishDragY) /
                                    (C.fishTH + C.fishGap),
                            0.0, (double)(C.fishRows - 1));
                        CarRequest();
                    }
                }
                if (p.x != C.fishMouseAt.x || p.y != C.fishMouseAt.y) {
                    C.fishMouse = true;
                    C.fishMouseAt = p;
                    CarKick();
                }
                return 0;
            }
            // Hexagon grid: the hexagon under the mouse becomes the selected
            // one (only when the mouse really moved).
            if (C.hex) {
                const POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (C.hexDrag) {
                    // Dragging: the mouse's X moves the track.
                    if (!C.hexDragMoved &&
                        std::fabs((float)p.x - C.hexDragX) > 6.0f) {
                        C.hexDragMoved = true;
                    }
                    if (C.hexDragMoved) {
                        C.hexScrollTarget = std::clamp(
                            C.hexDragScroll -
                                (double)(p.x - C.hexDragX) / C.hexPitchX,
                            0.0, (double)std::max(0, C.hexCols - 1));
                        C.hexWheel = false;
                        CarRequest();
                        CarKick();
                    }
                    C.mouseLast = p;
                    return 0;
                }
                if (p.x != C.mouseLast.x || p.y != C.mouseLast.y) {
                    C.mouseLast = p;
                    const int hit = HexAt((float)p.x, (float)p.y);
                    if (hit >= 0) {
                        HexSelect(hit, false);
                    }
                }
            }
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    DestroyWindow(hwnd);
                    break;
                case VK_RIGHT:
                case 'D':
                    CarMove(C.hex ? C.hexPerCol : 1);  // next column, same slot
                    break;
                case VK_LEFT:
                case 'A':
                    CarMove(C.hex ? -C.hexPerCol : -1);
                    break;
                case VK_NEXT:
                    CarMove(C.hex ? 5 * C.hexPerCol : C.fish ? 3 * C.fishCols : 5);
                    break;
                case VK_PRIOR:
                    CarMove(C.hex ? -5 * C.hexPerCol : C.fish ? -3 * C.fishCols : -5);
                    break;
                case VK_DOWN:
                    // (Stays inside its column.)
                    if (C.hex && C.target % C.hexPerCol < C.hexPerCol - 1 &&
                        C.target + 1 < CarCount()) {
                        CarMove(1);
                    } else if (C.spiral || C.tun) {
                        CarMove(1);
                    } else if (C.fish) {
                        CarMove(C.fishCols);
                    }
                    break;
                case VK_UP:
                    if (C.hex && C.target % C.hexPerCol > 0) {
                        CarMove(-1);
                    } else if (C.spiral || C.tun) {
                        CarMove(-1);
                    } else if (C.fish) {
                        CarMove(-C.fishCols);
                    }
                    break;
                case VK_HOME:
                    CarMove(C.wrap ? -CarIndex(C.target) : -C.target);
                    break;
                case VK_END:
                    CarMove(C.wrap ? CarCount() - 1 - CarIndex(C.target)
                                   : CarCount() - 1 - C.target);
                    break;
                case VK_RETURN:
                case VK_SPACE:
                    CarApply();
                    break;
                case 'U':
                    if (UsesSubject(g_cfg.transition)) {
                        CarApply(true);
                    }
                    break;
                case VK_DELETE:
                    if (UsesSubject(g_cfg.transition)) {
                        Wh_DeleteValue(
                            InkSubjectKey(C.paths[CarIndex(C.target)]).c_str());
                    }
                    break;
                case 'T':
                    g_cfg.transition =
                        (g_cfg.transition + 1) % (int)Transition::Count;
                    Wh_SetIntValue(L"pickedTransition", g_cfg.transition);
                    CarKick();
                    break;
                case 'S': {
                    // Cycles Strip, Fan, Slant, Cards only (never a
                    // full-screen style). From a full-screen style it goes
                    // back to the last of those.
                    int next;
                    if (IsFullStyle(g_cfg.pickerStyle)) {
                        next = g_bandStyle;
                    } else {
                        next = (g_cfg.pickerStyle + 1) % (int)PickerStyle::Hex;
                    }
                    g_bandStyle = next;
                    CarSetStyle(next);
                    break;
                }
                case 'H': {
                    // Full-screen styles: each press goes to the next one
                    // (Hexagons, Spiral, ...). From a normal style it goes
                    // to the last full-screen one used.
                    int next;
                    if (IsFullStyle(g_cfg.pickerStyle)) {
                        const int count =
                            (int)PickerStyle::Count - (int)PickerStyle::Hex;
                        next = (int)PickerStyle::Hex +
                               (g_cfg.pickerStyle - (int)PickerStyle::Hex + 1) %
                                   count;
                    } else {
                        g_bandStyle = g_cfg.pickerStyle;
                        next = g_fullStyle;
                    }
                    g_fullStyle = next;
                    CarSetStyle(next);
                    break;
                }
                case 'F':
                    DestroyWindow(hwnd);
                    PostMessage(g_hwnd, WM_APP_PICKFOLDER, 0, 0);
                    break;
            }
            return 0;
        case WM_LBUTTONDOWN:
            if (C.fish) {
                C.fishDrag = true;
                C.fishDragMoved = false;
                C.fishDragY = (float)GET_Y_LPARAM(lParam);
                C.fishDragScroll = C.fishScrollTarget;
                SetCapture(hwnd);
                return 0;
            }
            if (C.hex) {
                C.hexDrag = true;
                C.hexDragMoved = false;
                C.hexDragX = (float)GET_X_LPARAM(lParam);
                C.hexDragScroll = C.hexScrollTarget;
                SetCapture(hwnd);
            }
            return 0;
        case WM_LBUTTONUP: {
            // The nearest to the middle of the pictures under the mouse.
            const float x = (float)GET_X_LPARAM(lParam);
            const float y = (float)GET_Y_LPARAM(lParam);
            if (C.hex) {
                const bool wasDrag = C.hexDragMoved;
                if (C.hexDrag) {
                    C.hexDrag = false;
                    ReleaseCapture();
                }
                C.hexDragMoved = false;
                if (wasDrag) {
                    return 0;  // it was a drag, not a click
                }
                // The hexagon under the mouse is already the selected one
                // (hover): a click sets it; a click on nothing closes.
                const int hit = HexAt(x, y);
                if (hit < 0) {
                    DestroyWindow(hwnd);
                } else if (hit == C.target) {
                    CarApply();
                } else {
                    HexSelect(hit);
                }
                return 0;
            }
            if (C.spiral) {
                int hit = 0;
                if (SpiralHit(x, y, &hit)) {
                    if (hit == C.target) {
                        CarApply();
                    } else {
                        CarMove(hit - C.target);
                    }
                } else {
                    DestroyWindow(hwnd);
                }
                return 0;
            }
            if (C.fish) {
                const bool wasDrag = C.fishDragMoved;
                if (C.fishDrag) {
                    C.fishDrag = false;
                    ReleaseCapture();
                }
                C.fishDragMoved = false;
                if (wasDrag) {
                    return 0;
                }
                const int tile = FishAt(x, y);
                if (tile < 0) {
                    DestroyWindow(hwnd);
                } else if (tile == C.target) {
                    CarApply();
                } else {
                    FishSelect(tile, false);
                }
                return 0;
            }
            if (C.tun) {
                int frame = 0;
                if (TunnelHit(x, y, &frame)) {
                    if (frame == C.target) {
                        CarApply();
                    } else {
                        CarMove(frame - C.target);
                    }
                } else {
                    DestroyWindow(hwnd);
                }
                return 0;
            }
            const int base = (int)std::floor(C.scroll);
            bool found = false;
            int hit = 0;
            float best = 1e9f;
            for (int v = base - 6; v <= base + 7; v++) {
                if (!CarValid(v)) {
                    continue;
                }
                const float d = (float)(v - C.scroll);
                const CardPose pose = CarPose(d);
                const float distance = std::fabs(d);
                if (pose.alpha > 0.2f && CarPoseHit(pose, x, y) &&
                    distance < best) {
                    best = distance;
                    hit = v;
                    found = true;
                }
            }
            if (found) {
                if (hit == C.target) {
                    CarApply();
                } else {
                    CarMove(hit - C.target);
                }
            }
            return 0;
        }
        case WM_APP_THUMB: {
            std::unique_ptr<ThumbResult> result((ThumbResult*)lParam);
            const bool ok = result->generation == C.generation &&
                            result->index >= 0 && result->index < CarCount();
            if (!ok) {
                if (result->bitmap) {
                    DeleteObject(result->bitmap);
                }
                return 0;
            }
            CarouselItem& item = C.items[result->index];
            item.state = 2;
            if (result->bitmap) {
                ComPtr<IWICBitmap> wicBitmap;
                ComPtr<IWICFormatConverter> converter;
                if (C.wic &&
                    SUCCEEDED(C.wic->CreateBitmapFromHBITMAP(
                        result->bitmap, nullptr, WICBitmapIgnoreAlpha,
                        wicBitmap.GetAddressOf())) &&
                    SUCCEEDED(C.wic->CreateFormatConverter(
                        converter.GetAddressOf())) &&
                    SUCCEEDED(converter->Initialize(
                        wicBitmap.Get(), GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone, nullptr, 0,
                        WICBitmapPaletteTypeMedianCut)) &&
                    C.canvas.target) {
                    AmbientStore(result->index, converter.Get());
                    if (C.fish) {
                        item.fishBmp = RoundBake(
                            C.canvas.target.Get(), C.wic.Get(),
                            converter.Get(), (UINT)C.fishBake.cx,
                            (UINT)C.fishBake.cy, std::round(14 * C.scale));
                        item.fishMip = RoundBake(
                            C.canvas.target.Get(), C.wic.Get(),
                            converter.Get(), (UINT)C.fishBake.cx / 2,
                            (UINT)C.fishBake.cy / 2, std::round(7 * C.scale));
                    } else if (C.tun) {
                        item.tunBmp = RoundBake(
                            C.canvas.target.Get(), C.wic.Get(),
                            converter.Get(), (UINT)C.tunBake.cx,
                            (UINT)C.tunBake.cy, std::round(10 * C.scale));
                        item.tunMip = RoundBake(
                            C.canvas.target.Get(), C.wic.Get(),
                            converter.Get(), (UINT)C.tunBake.cx / 2,
                            (UINT)C.tunBake.cy / 2, std::round(5 * C.scale));
                    } else if (C.spiral) {
                        item.spBmp = SpiralBake(C.canvas.target.Get(),
                                                C.wic.Get(), converter.Get(),
                                                (UINT)std::lround(C.spBig));
                        item.spMip = SpiralBake(
                            C.canvas.target.Get(), C.wic.Get(), converter.Get(),
                            (UINT)std::lround(C.spBig * 0.6f));
                    } else {
                        if (!C.hex) {
                            C.canvas.target->CreateBitmapFromWicBitmap(
                                converter.Get(), nullptr,
                                item.bitmap.ReleaseAndGetAddressOf());
                        }
                        item.hexBmp = HexBake(C.canvas.target.Get(),
                                              C.wic.Get(), converter.Get());
                        if (!C.hex && C.lqTarget > 0.5) {
                            const int cnt = CarCount();
                            int dist =
                                std::abs(result->index - CarIndex(C.target));
                            if (C.wrap) {
                                dist = std::min(dist, cnt - dist);
                            }
                            if (dist <= 3) {
                                LqBakeChannels(item, converter.Get());
                            }
                        }
                    }
                }
                DeleteObject(result->bitmap);
            }
            CarKick();
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, 1);
            // The picture threads are told to stop; what they posted to this
            // window, about to go, is freed here.
            g_carouselGeneration++;
            MSG pending;
            while (PeekMessage(&pending, hwnd, WM_APP_THUMB, WM_APP_THUMB,
                               PM_REMOVE)) {
                std::unique_ptr<ThumbResult> result(
                    (ThumbResult*)pending.lParam);
                if (result->bitmap) {
                    DeleteObject(result->bitmap);
                }
            }
            if (C.dimHwnd) {
                DestroyWindow(C.dimHwnd);
                C.dimHwnd = nullptr;
            }
            for (auto& e : C.ambCache) {
                e.bmp.Reset();
            }
            C.dimCanvas.Destroy();
            C.items.clear();
            C.canvas.Destroy();
            g_car = CarouselState();
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void BringToFront(HWND hwnd) {
    // An empty input event lets SetForegroundWindow succeed when this process
    // didn't receive the last input (for example, a named event started it).
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void OpenPanel() {
    if (g_car.hwnd) {
        ClosePanel();
        return;
    }
    std::vector<std::wstring> paths = ListWallpapers(g_cfg.folder);
    if (paths.empty()) {
        // No folder (or no pictures in it): ask for one.
        PostMessage(g_hwnd, WM_APP_PICKFOLDER, 0, 0);
        return;
    }
    if (!g_panelClassRegistered) {
        WNDCLASS wc{};
        wc.lpfnWndProc = CarouselProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = kPanelClass;
        if (!RegisterClass(&wc)) {
            return;
        }
        g_panelClassRegistered = true;
    }
    POINT cursor{};
    GetCursorPos(&cursor);
    MONITORINFO info{sizeof(info)};
    GetMonitorInfo(MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY), &info);

    g_car = CarouselState();
    CarouselState& C = g_car;
    C.paths = std::move(paths);
    C.items.assign(C.paths.size(), CarouselItem{});
    C.monitor = info.rcMonitor;
    C.wrap = CarCount() >= 9;
    C.generation = ++g_carouselGeneration;
    C.hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                            kPanelClass, L"Wallpapers", WS_POPUP,
                            C.monitor.left, C.monitor.top, 100, 100, nullptr,
                            nullptr, GetModuleHandle(nullptr), nullptr);
    if (!C.hwnd) {
        g_car = CarouselState();
        return;
    }
    C.scale = GetDpiForWindow(C.hwnd) / 96.0f;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(C.wic.GetAddressOf()));
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, C.d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)C.dwrite.GetAddressOf());
    if (!C.d2d || !C.dwrite || !C.canvas.Create(C.d2d.Get())) {
        DestroyWindow(C.hwnd);
        return;
    }
    C.dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr,
                               DWRITE_FONT_WEIGHT_SEMI_BOLD,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 15 * C.scale, L"",
                               C.labelFormat.GetAddressOf());
    C.dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr,
                               DWRITE_FONT_WEIGHT_NORMAL,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 12 * C.scale, L"",
                               C.hintFormat.GetAddressOf());
    C.dwrite->CreateTextFormat(L"Consolas", nullptr, DWRITE_FONT_WEIGHT_BOLD,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 13 * C.scale, L"",
                               C.chipMono.GetAddressOf());
    C.dwrite->CreateTextFormat(L"Consolas", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 10.5f * C.scale, L"",
                               C.chipSmall.GetAddressOf());
    C.dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr,
                               DWRITE_FONT_WEIGHT_SEMI_BOLD,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 11.5f * C.scale, L"",
                               C.chipSans.GetAddressOf());
    for (IDWriteTextFormat* format :
         {C.labelFormat.Get(), C.hintFormat.Get(), C.chipMono.Get(),
          C.chipSmall.Get(), C.chipSans.Get()}) {
        if (!format) {
            continue;
        }
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        ComPtr<IDWriteInlineObject> ellipsis;
        if (SUCCEEDED(C.dwrite->CreateEllipsisTrimmingSign(
                format, ellipsis.GetAddressOf()))) {
            const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER,
                                           0, 0};
            format->SetTrimming(&trimming, ellipsis.Get());
        }
    }
    // The strip: the monitor's width, in the middle of it. The middle
    // picture is about a third of the width, with the monitor's shape.
    const float monitorW = (float)(C.monitor.right - C.monitor.left);
    const float monitorH = (float)(C.monitor.bottom - C.monitor.top);
    C.bigW = std::round(monitorW * 0.35f);
    C.bigH = std::round(C.bigW * monitorH / monitorW);
    C.smallW = std::round(C.bigW * 0.83f);
    C.smallH = std::round(C.bigH * 0.83f);
    C.gap = std::round(14 * C.scale);
    C.slantSmallW = std::round(C.bigW * 0.17f);  // the narrow side slices
    C.slantGap = std::round(10 * C.scale);
    const float margin = std::round(12 * C.scale);
    const float stripBand = C.bigH + margin * 2 + std::round(100 * C.scale);

    // The fan: portrait cards on a big circle.
    C.fanH = std::round(std::min(monitorH * 0.42f, monitorW * 0.55f));
    C.fanW = std::round(C.fanH * 0.66f);
    C.fanRadius = C.fanH * kFanRadiusK;
    C.fanCenterY = margin + C.fanH * kFanLift + C.fanH / 2;
    C.size = {(LONG)monitorW, 100};  // (the width is what the fan reads)
    C.spread = 1;
    float fanBottom = 0;
    for (int i = 0; i <= 24; i++) {
        const CardPose p = FanPose(i * 0.25f, 1.0f);
        if (p.alpha < 0.02f) {
            continue;
        }
        const float r = p.angle * kPi / 180;
        fanBottom = std::max(fanBottom,
                             p.cy + std::fabs(p.w / 2 * std::sin(r)) +
                                 std::fabs(p.h / 2 * std::cos(r)));
    }
    C.fanLabelY = fanBottom + std::round(14 * C.scale);
    const float fanBand = C.fanLabelY + std::round(86 * C.scale);

    // Liquid: the middle card is half the monitor's width.
    C.lqW = std::round(monitorW * kLqActiveK);
    C.lqH = std::round(std::min(C.lqW * 0.5625f, monitorH * 0.5f));
    const float lqBand = C.lqH + margin * 2 + std::round(100 * C.scale);
    const float bandH = std::max({stripBand, fanBand, lqBand});
    C.lqCy = (bandH - lqBand) / 2 + margin + C.lqH / 2;
    C.lqLabelY = C.lqCy + C.lqH / 2 + std::round(14 * C.scale);
    C.size = {(LONG)monitorW, (LONG)bandH};
    // The strip stays in the middle of the (taller) window.
    C.centerY = (bandH - stripBand) / 2 + margin + C.bigH / 2;
    // Coverflow: the middle card a little smaller than the strip's and raised,
    // with room under it for the reflection and the name.
    C.cvW = std::round(C.bigW * 0.80f);
    C.cvH = std::round(C.bigH * 0.80f);
    C.cvCy = C.centerY - std::round(C.bigH * 0.10f);
    C.cvLabelY = C.cvCy + C.cvH / 2 + std::round(0.20f * C.cvH) +
                 std::round(8 * C.scale);
    // Deck: the middle card on top of the stack, the name just under it.
    C.dkW = std::round(C.bigW * 0.90f);
    C.dkH = std::round(C.bigH * 0.90f);
    C.dkCy = C.centerY - std::round(C.bigH * 0.02f);
    C.dkLabelY = C.dkCy + C.dkH / 2 + std::round(22 * C.scale);
    C.thumbSize = {(LONG)std::ceil(std::max({C.bigW, C.fanH * 1.8f, C.lqW})),
                   (LONG)std::ceil(std::max({C.bigH, C.fanH, C.lqH}))};
    C.position = {C.monitor.left,
                  C.monitor.top + (LONG)((monitorH - bandH) / 2)};
    C.bandSize = C.size;
    C.bandPosition = C.position;
    C.ambW = std::max(16, (int)(monitorW / 8));
    C.ambH = std::max(16, (int)(monitorH / 8));
    HexSetup();
    // It starts at the picture now the wallpaper.
    int start = IndexOfPath(C.paths, CurrentWallpaperPath(C.monitor));
    if (start < 0) {
        start = IndexOfPath(C.paths, g_lastPath);
    }
    C.target = std::max(0, start);
    C.scroll = C.target;
    C.styleMix = C.mixTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Fan ? 1.0 : 0.0;
    C.slantMix = C.slantTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Slant ||
                g_cfg.pickerStyle == (int)PickerStyle::Cards
            ? 1.0
            : 0.0;
    C.leanMix = C.leanTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Slant ? 1.0 : 0.0;
    C.cfMix = C.cfTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Coverflow ? 1.0 : 0.0;
    C.dkMix = C.dkTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Deck ? 1.0 : 0.0;
    C.flMix = C.flTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Flow ? 1.0 : 0.0;
    C.lqMix = C.lqTarget =
        g_cfg.pickerStyle == (int)PickerStyle::Liquid ? 1.0 : 0.0;
    C.lqVel = 0;
    C.lqClock = 0;
    C.openLen = C.mixTarget > 0.5 ? 0.38
                : (C.cfTarget > 0.5 || C.flTarget > 0.5) ? 0.30
                                   : 0.16;  // the fan, Coverflow and Drum open slower
    C.openStart = NowSeconds();
    if (IsFullStyle(g_cfg.pickerStyle)) {
        g_fullStyle = g_cfg.pickerStyle;
        // (The window changes to the full-screen layout.)
        CarSetMode(g_cfg.pickerStyle == (int)PickerStyle::Hex       ? 1
                   : g_cfg.pickerStyle == (int)PickerStyle::Spiral  ? 2
                   : g_cfg.pickerStyle == (int)PickerStyle::Fisheye ? 3
                                                                    : 4);
    } else {
        g_bandStyle = g_cfg.pickerStyle;
    }
    CarRequest();
    CarDraw(0);
    SetWindowPos(C.hwnd, HWND_TOPMOST, C.position.x, C.position.y, C.size.cx,
                 C.size.cy, SWP_NOACTIVATE);
    ShowWindow(C.hwnd, SW_SHOW);
    BringToFront(C.hwnd);
    CarKick();
}

// ---------------------------------------------------------------------------
// Folder dialog
// ---------------------------------------------------------------------------

std::wstring PickFolder(HWND owner) {
    std::wstring result;
    ComPtr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(dialog.GetAddressOf())))) {
        return result;
    }
    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
    dialog->SetTitle(L"The folder with your wallpapers");
    if (g_unloading) {
        return result;
    }
    g_openDialog = dialog.Get();
    const HRESULT shown = dialog->Show(owner);
    g_openDialog = nullptr;
    if (FAILED(shown)) {
        return result;
    }
    ComPtr<IShellItem> item;
    PWSTR path = nullptr;
    if (SUCCEEDED(dialog->GetResult(item.GetAddressOf())) &&
        SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
        result = path;
        CoTaskMemFree(path);
    }
    return result;
}

// The cut-out of the wallpaper's subject (a PNG with a transparent
// background). Cancel = no subject (ink drops).
std::wstring PickSubject(HWND owner) {
    std::wstring result;
    ComPtr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(dialog.GetAddressOf())))) {
        return result;
    }
    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);
    const COMDLG_FILTERSPEC filters[] = {
        {L"Pictures with transparency", L"*.png;*.webp;*.tif;*.tiff"},
        {L"All pictures", L"*.png;*.webp;*.bmp;*.jpg;*.jpeg"}};
    dialog->SetFileTypes(ARRAYSIZE(filters), filters);
    dialog->SetTitle(
        L"Subject of this wallpaper (PNG, transparent background, same size). "
        L"Cancel = ink drops, no subject");
    if (g_unloading) {
        return result;
    }
    g_openDialog = dialog.Get();
    const HRESULT shown = dialog->Show(owner);
    g_openDialog = nullptr;
    if (FAILED(shown)) {
        return result;
    }
    ComPtr<IShellItem> item;
    PWSTR path = nullptr;
    if (SUCCEEDED(dialog->GetResult(item.GetAddressOf())) &&
        SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
        result = path;
        CoTaskMemFree(path);
    }
    return result;
}

// ---------------------------------------------------------------------------
// The mod's thread: hidden window (hotkeys), named events, message loop
// ---------------------------------------------------------------------------

void RunAction(int action) {
    switch (action) {
        case ActionPicker:
            OpenPanel();
            break;
        case ActionNext:
            StepWallpaper(+1);
            break;
        case ActionPrev:
            StepWallpaper(-1);
            break;
        case ActionRandom:
            StepWallpaper(0);
            break;
    }
}

LRESULT CALLBACK MessageProc(HWND hwnd,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY:
            if (wParam >= 1 && wParam <= 4) {
                RunAction((int)wParam - 1);
            }
            return 0;
        case WM_APP_RELOAD:
            LoadConfig();
            RegisterHotkeys();
            return 0;
        case WM_APP_STEPDONE:
            if (g_stepPending && g_busy == 0 && !g_unloading) {
                g_stepPending = false;
                StepWallpaper(g_pendingStep);
            }
            return 0;
        case WM_TIMER:
            if (wParam == kHotkeyRetryTimer) {
                const bool last = --g_hotkeyRetries <= 0;
                if (TryRegisterHotkeys(last) == 0 || last) {
                    KillTimer(hwnd, kHotkeyRetryTimer);
                }
            }
            return 0;
        case WM_APP_PICKFOLDER: {
            const std::wstring folder = PickFolder(hwnd);
            if (!folder.empty()) {
                Wh_SetStringValue(L"pickedFolder", folder.c_str());
                g_cfg.folder = folder;
                // (Only with pictures in it, or it would ask again and
                // again.)
                if (!ListWallpapers(folder).empty()) {
                    OpenPanel();
                }
            }
            return 0;
        }
        case WM_APP_PICKSUBJECT: {
            const std::wstring wallpaper = g_pendingWallpaper;
            g_pendingWallpaper.clear();
            if (wallpaper.empty()) {
                return 0;
            }
            const std::wstring subject = PickSubject(hwnd);
            if (!subject.empty()) {
                Wh_SetStringValue(InkSubjectKey(wallpaper).c_str(),
                                  subject.c_str());
            }
            ApplyWallpaper(wallpaper, true, subject);
            return 0;
        }
        case WM_APP_CLOSEDIALOG:
            if (g_openDialog) {
                g_openDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI MainThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HINSTANCE instance = GetModuleHandle(nullptr);
    WNDCLASS wc{};
    wc.lpfnWndProc = MessageProc;
    wc.hInstance = instance;
    wc.lpszClassName = kMessageClass;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, kMessageClass, L"", WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    g_hwnd = hwnd;
    if (hwnd && !g_unloading) {
        LoadConfig();
        g_lastPath = GetStoredString(L"lastPath");
        RegisterHotkeys();
        ListWallpapers(g_cfg.folder);  // warm the disk and the list for the first press
        HANDLE handles[5] = {g_events[0], g_events[1], g_events[2],
                             g_events[3], g_stop};
        bool running = true;
        while (running) {
            const DWORD wait = MsgWaitForMultipleObjects(5, handles, FALSE,
                                                         INFINITE, QS_ALLINPUT);
            if (wait < WAIT_OBJECT_0 + 4) {
                RunAction((int)(wait - WAIT_OBJECT_0));
                continue;
            }
            if (wait == WAIT_OBJECT_0 + 4) {
                break;  // stop
            }
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        for (int i = 0; i < 4; i++) {
            UnregisterHotKey(hwnd, i + 1);
        }
    }
    ClosePanel();
    if (hwnd) {
        DestroyWindow(hwnd);
    }
    g_hwnd = nullptr;
    UnregisterClass(kMessageClass, instance);
    if (g_panelClassRegistered) {
        UnregisterClass(kPanelClass, instance);
        g_panelClassRegistered = false;
    }
    if (g_dimClassRegistered) {
        UnregisterClass(kDimClass, instance);
        g_dimClassRegistered = false;
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    g_unloading = false;
    // Keep this background process off the efficiency cores and the coarse
    // timers Windows gives to throttled processes.
    for (ULONG flag : {PROCESS_POWER_THROTTLING_EXECUTION_SPEED,
                       PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION}) {
        PROCESS_POWER_THROTTLING_STATE throttle{};
        throttle.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        throttle.ControlMask = flag;
        throttle.StateMask = 0;
        SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling,
                              &throttle, sizeof(throttle));
    }
    g_stop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_events[ActionPicker] =
        CreateEvent(nullptr, FALSE, FALSE, L"Local\\FastWallpaper.Picker");
    g_events[ActionNext] =
        CreateEvent(nullptr, FALSE, FALSE, L"Local\\FastWallpaper.Next");
    g_events[ActionPrev] =
        CreateEvent(nullptr, FALSE, FALSE, L"Local\\FastWallpaper.Prev");
    g_events[ActionRandom] =
        CreateEvent(nullptr, FALSE, FALSE, L"Local\\FastWallpaper.Random");
    if (!g_stop || !g_events[0] || !g_events[1] || !g_events[2] ||
        !g_events[3]) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, MainThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // What the settings page says wins over what the picker chose.
    Wh_DeleteValue(L"pickedFolder");
    Wh_DeleteValue(L"pickedTransition");
    Wh_DeleteValue(L"pickedStyle");
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_APP_RELOAD, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_unloading = true;
    if (g_thread) {
        SetEvent(g_stop);
        // Ends an open file dialog (its message loop runs on that thread).
        if (HWND hwnd = g_hwnd) {
            PostMessage(hwnd, WM_APP_CLOSEDIALOG, 0, 0);
        }
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    // The transitions and picture threads: wait until each one has really
    // exited, not just counted itself out.
    for (;;) {
        HANDLE worker = nullptr;
        AcquireSRWLockExclusive(&g_workersLock);
        if (!g_workers.empty()) {
            worker = g_workers.back();
            g_workers.pop_back();
        }
        ReleaseSRWLockExclusive(&g_workersLock);
        if (!worker) {
            break;
        }
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
    for (HANDLE& handle : g_events) {
        if (handle) {
            CloseHandle(handle);
            handle = nullptr;
        }
    }
    if (g_stop) {
        CloseHandle(g_stop);
        g_stop = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
