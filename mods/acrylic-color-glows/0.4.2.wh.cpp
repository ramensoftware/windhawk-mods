// ==WindhawkMod==
// @id              acrylic-color-glows
// @name            Acrylic Color Glows
// @description     Animated light effects behind selected translucent windows
// @version         0.4.2
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lshell32 -ladvapi32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Acrylic Color Glows

![Acrylic Glow](https://i.imgur.com/qvqqV2I.png)
[Watch the overview video in full quality](https://i.imgur.com/huCSr8H.mp4)

Animated light effects behind the windows you choose: drifting glows, sweeping
light beams, orbs moving in the directions you pick, or flowing waves. The
effect is drawn by a separate, click-through window placed directly below each
target window, so a translucent (acrylic/blur) target blurs it into its own
background. The mod doesn't inject code into other processes, never draws above
text or controls, and doesn't change the transparency of any application.

## Effects

- **Glows**: three large glows drifting from side to side.
- **Light beams**: soft beams coming from one edge of the window and slowly
  sweeping back and forth. Choose the number of beams, the edge and the sweep
  angle.
- **Directional orbs**: up to 12 orbs, each with its own color, direction and
  speed. They bounce off the edges or wrap around to the opposite side.
- **Waves**: three flowing bands of color. Choose the amplitude and the
  wavelength.

Colors 1-3 are used by glows, beams and waves; orbs have their own colors.
"Cycle duration" sets the overall speed of every effect. "Size" sets the glow
and orb diameter and the beam and wave thickness.

## Saving power

While an effect moves, Windows has to redraw the screen and recompute the blur
of every decorated window on each frame. Slower or fainter effects don't
reduce that work; fewer moving windows do:

- **Animate**: all windows, only the active window (the others keep their
  effect, frozen), or none (a still effect, with no ongoing GPU work).
- **Smoothness**: "Reduced" updates the motion about 30 times per second
  instead of at the monitor's refresh rate. Slow, blurred motion looks almost
  the same, and it saves power on high refresh rate monitors.
- **Power saving**: freeze every effect on battery saver, while a fullscreen
  app, game or presentation is in front, or when Windows animation effects
  are turned off; hide it when Windows transparency effects are turned off,
  since acrylic is then opaque.

Frozen effects resume exactly where they stopped. When nothing is moving, the
shared clock stops too.

The effect sits behind the window's blur, so fine details are softened: large,
slow, soft shapes work best. Speeds are rounded very slightly so that every
motion loops seamlessly.

## Requirements

- Windows 10 version 1803 or later (Windows 11 recommended).
- Target windows must already be translucent, for example through their own
  acrylic backdrop or another Windhawk mod. Opaque or Mica backgrounds hide the
  effect.
- Don't combine this mod with another mod that also draws effects behind the
  same windows.

## Modes

- **Selected applications**: every regular top-level window of the listed
  processes gets the effect, whether or not Windows reports it as acrylic.
- **Automatic**: only windows that declare the acrylic system backdrop
  (`DWMSBT_TRANSIENTWINDOW`) to Windows. Acrylic implemented in other ways
  isn't detected.
- **Selected applications + automatic**: both.

Process names are case-insensitive, without paths or wildcards. Exclusions
always win. Most UWP apps run inside `ApplicationFrameHost.exe`.

The desktop, taskbar, shell overlays, tool windows and the mod's own process
are always skipped. Processes that can't be queried (for example elevated or
protected ones) are skipped too.

## How it works

The mod runs as a tool mod in a dedicated Windhawk process. A single worker
thread owns all effect windows. Animations run on the GPU through
Windows.UI.Composition, driven by one shared clock. Window events keep the effect aligned, and a two-second poll
catches anything that was missed.

Settings changes apply immediately. Colors and intensity change in place;
other changes rebuild the effect windows, which fade back in.

The effect is hidden while the target is minimized, hidden or cloaked. It is
also hidden briefly while a window opens, maximizes, restores or comes back
from the taskbar, then fades in once the system animation is over.

Effect windows are fully click-through, never take focus and don't appear in
the taskbar or in Alt+Tab. Invisible overlays (fully transparent or
click-through windows), tiny helper windows and borderless popups such as
tooltips, menus, flyouts and drop-down lists are never decorated.

## Known limitations

- The effect window is a separate window, so during fast dragging it can lag
  slightly behind the target.
- The final color depends on each application's tint and blur strength.
- The window limit includes temporarily hidden effects.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- effect: glows
  $name: Effect
  $options:
  - glows: Glows
  - beams: Light beams
  - orbs: Directional orbs
  - waves: Waves
- mode: selected
  $name: Mode
  $options:
  - selected: Selected applications
  - automatic: Automatic (acrylic declared to Windows)
  - combined: Selected applications + automatic
- applications:
  - explorer.exe
  - notepad.exe
  - WindowsTerminal.exe
  $name: Selected applications
  $description: Executable names, for example notepad.exe. No paths or wildcards.
- excludedApplications:
  - windhawk.exe
  $name: Excluded applications
  $description: Exclusions take priority over both the list and automatic detection.
- color1: "#00BFFF"
  $name: Color 1 (hex)
- color2: "#A855F7"
  $name: Color 2 (hex)
- color3: "#FF4081"
  $name: Color 3 (hex)
- seconds: 12
  $name: Cycle duration (seconds)
  $description: 2 to 120. Higher values make every effect slower.
- opacity: 32
  $name: Intensity (1-100)
- diameter: 380
  $name: Size (DIP)
  $description: 80 to 1600. Glow and orb diameter; beam and wave thickness follow it.
- maxWindows: 20
  $name: Maximum number of windows
  $description: 1 to 100. Includes temporarily hidden effects.
- animate: all
  $name: Animate
  $description: Fewer moving windows means less GPU work and longer battery life.
  $options:
  - all: All windows
  - active: Active window only
  - none: No animation (still effect)
- smoothness: full
  $name: Smoothness
  $description: Reduced updates the motion about 30 times per second. It saves power on high refresh rate monitors.
  $options:
  - full: Full (monitor refresh rate)
  - reduced: Reduced (about 30 fps)
- powerSaving:
  - batterySaver: true
    $name: Freeze on battery saver
  - fullscreen: true
    $name: Freeze while a fullscreen app, game or presentation is in front
  - reduceMotion: true
    $name: Freeze when Windows animation effects are turned off
  - transparency: true
    $name: Hide when Windows transparency effects are turned off
  $name: Power saving
- beams:
  - count: 3
    $name: Number of beams
    $description: 1 to 8. Colors 1-3 are used in turn.
  - origin: top
    $name: Beams come from
    $options:
    - top: Top edge
    - bottom: Bottom edge
    - left: Left edge
    - right: Right edge
  - sweep: 30
    $name: Sweep angle (degrees)
    $description: 0 to 80. 0 keeps the beams still.
  $name: Light beams
- orbs:
  - - color: "#00BFFF"
      $name: Color (hex)
    - angle: 30
      $name: Direction (degrees)
      $description: 0 = right, 90 = up, 180 = left, 270 = down.
    - speed: 100
      $name: Speed (%)
      $description: 10 to 400.
  - - color: "#A855F7"
      $name: Color (hex)
    - angle: 150
      $name: Direction (degrees)
      $description: 0 = right, 90 = up, 180 = left, 270 = down.
    - speed: 70
      $name: Speed (%)
      $description: 10 to 400.
  - - color: "#FF4081"
      $name: Color (hex)
    - angle: 250
      $name: Direction (degrees)
      $description: 0 = right, 90 = up, 180 = left, 270 = down.
    - speed: 130
      $name: Speed (%)
      $description: 10 to 400.
  $name: Directional orbs
  $description: Up to 12 orbs. An orb with an empty color ends the list.
- orbEdges: bounce
  $name: Orbs at the edges
  $options:
  - bounce: Bounce
  - wrap: Wrap around
- waves:
  - amplitude: 18
    $name: Amplitude (% of window height)
    $description: 2 to 45.
  - length: 100
    $name: Wavelength (% of window width)
    $description: 25 to 400.
  $name: Waves
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif
#ifndef SPI_GETCLIENTAREAANIMATION
#define SPI_GETCLIENTAREAANIMATION 0x1042
#endif

namespace wc = winrt::Windows::UI::Composition;
using winrt::Windows::UI::Color;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Constants

// Documented DWM values, used numerically so the mod doesn't depend on how
// recent the SDK headers are.
constexpr DWORD kDwmaTransitionsForceDisabled = 3;
constexpr DWORD kDwmaWindowCornerPreference = 33;
constexpr DWORD kDwmaBorderColor = 34;
constexpr DWORD kDwmaSystemBackdropType = 38;
constexpr int kCornerDoNotRound = 1;
constexpr int kCornerRound = 2;
constexpr int kBackdropTransientWindow = 3;
constexpr COLORREF kColorNone = 0xFFFFFFFE;
constexpr DWORD kCloakedShell = 0x2;

constexpr UINT kPollIntervalMs = 2000;
constexpr UINT kSettleDelayMs = 250;
constexpr int kFadeInMs = 180;
constexpr UINT_PTR kPollTimerId = 1;
constexpr UINT_PTR kSettleTimerId = 2;
constexpr UINT_PTR kFrameTimerId = 3;   // Reduced smoothness only.
constexpr UINT kFrameIntervalMs = 33;
constexpr UINT kRefreshMessage = WM_APP + 1;
constexpr UINT kSettingsMessage = WM_APP + 2;
constexpr int kMinTargetSize = 32;             // Physical pixels.
constexpr ULONGLONG kFailedRetryMs = 60000;    // Retry windows that failed before.
constexpr ULONGLONG kDesktopsRetryMs = 10000;  // Retry the virtual desktop manager.
constexpr DWORD kStopTimeoutMs = 10000;
constexpr wchar_t kClassName[] = L"WindhawkAcrylicColorGlows";

constexpr float kPi = 3.14159265f;
constexpr float kTwoPi = 2.0f * kPi;
// Shared clock period. Every clock-driven motion completes a whole number of
// cycles in this time, so nothing jumps when the clock wraps around.
constexpr float kLoopSeconds = 600.0f;
constexpr int kMaxOrbs = 12;
constexpr int kWaveCount = 3;
constexpr int kWaveBlobs = 10;

const PCWSTR kExcludedClasses[] = {
    L"Progman",
    L"WorkerW",
    L"Shell_TrayWnd",
    L"Shell_SecondaryTrayWnd",
    L"NotifyIconOverflowWindow",
    L"TopLevelWindowForOverflowXamlIsland",
    L"XamlExplorerHostIslandWindow",
    L"MultitaskingViewFrame",
    L"ForegroundStaging",
    L"TaskListThumbnailWnd",
    L"Windows.UI.Core.CoreWindow",
    L"Shell_InputSwitchTopLevelWindow",
    L"Xaml_WindowedPopupClass",
    L"#32768",
    L"tooltips_class32",
    L"SysShadow",
    L"Ghost",
    L"Shell_LightDismissOverlay",
    L"EdgeUiInputTopWndClass",
    L"ApplicationManager_ImmersiveShellWindow",
    L"Windows.Internal.Shell.TabProxyWindow",
    L"Microsoft.UI.Content.PopupWindowSiteBridge",
    L"PopupHost",
    L"ComboLBox",
    L"DropDown",
    L"Net UI Tool Window",
    kClassName,
};

struct HookRange {
    DWORD first;
    DWORD last;
};

const HookRange kHookRanges[] = {
    {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
    {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
    {EVENT_OBJECT_DESTROY, EVENT_OBJECT_REORDER},
    {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
    {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
};

////////////////////////////////////////////////////////////////////////////////
// COM interfaces declared locally, so the mod doesn't depend on optional SDK
// headers. Layouts and IDs are documented by Microsoft.

// ICompositorDesktopInterop
struct DesktopInterop : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateDesktopWindowTarget(HWND hwndTarget,
                                                                BOOL isTopmost,
                                                                void** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnsureOnThread(DWORD threadId) = 0;
};
constexpr GUID kDesktopInteropIid = {
    0x29E691FA, 0x4567, 0x4DCA, {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};

// IVirtualDesktopManager
struct VirtualDesktopManager : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(HWND window,
                                                                      BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(HWND window, GUID* desktopId) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(HWND window, REFGUID desktopId) = 0;
};
constexpr GUID kVirtualDesktopManagerClsid = {
    0xAA509086, 0x5CA9, 0x4C25, {0x8F, 0x95, 0x58, 0x9D, 0x3C, 0x07, 0xB4, 0x8A}};
constexpr GUID kVirtualDesktopManagerIid = {
    0xA5CD92FF, 0x29BE, 0x454C, {0x8D, 0x04, 0xD8, 0x28, 0x79, 0xFB, 0x3F, 0x1B}};

// DispatcherQueueOptions / CreateDispatcherQueueController
struct DispatcherQueueOptionsAbi {
    DWORD dwSize;
    int threadType;
    int apartmentType;
};
constexpr int kDispatcherQueueThreadCurrent = 2;  // DQTYPE_THREAD_CURRENT
constexpr int kDispatcherQueueComSta = 2;         // DQTAT_COM_STA
using CreateDispatcherQueueController_t =
    HRESULT(WINAPI*)(DispatcherQueueOptionsAbi options, void** controller);

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class Mode { Selected, Automatic, Combined };
enum class Effect { Glows, Beams, Orbs, Waves };
enum class Origin { Top, Bottom, Left, Right };
enum class Edges { Bounce, Wrap };
enum class Animate { All, Active, None };

struct Orb {
    Color color{};
    int angle = 0;
    int speed = 100;
};

struct Settings {
    Effect effect = Effect::Glows;
    Mode mode = Mode::Selected;
    std::vector<std::wstring> include;
    std::vector<std::wstring> exclude;
    std::array<Color, 3> colors{};
    int seconds = 12;
    int opacity = 32;
    int diameter = 380;
    int maxWindows = 20;
    int beamCount = 3;
    Origin beamOrigin = Origin::Top;
    int beamSweep = 30;
    std::vector<Orb> orbs;
    Edges orbEdges = Edges::Bounce;
    int waveAmplitude = 18;
    int waveLength = 100;
    Animate animate = Animate::All;
    bool reducedSmoothness = false;
    struct {
        bool batterySaver = true;
        bool fullscreen = true;
        bool reduceMotion = true;
        bool transparency = true;
    } power;
};

// Owned by the worker thread: loaded and read only there.
Settings g_settings;

// Everything that defines the shape of the effect. When it changes, effect
// windows are rebuilt; otherwise colors are updated in place.
struct Layout {
    Effect effect;
    int seconds;
    int diameter;
    int beamCount;
    Origin beamOrigin;
    int beamSweep;
    std::vector<std::pair<int, int>> orbs;  // angle, speed
    Edges orbEdges;
    int waveAmplitude;
    int waveLength;
    bool reducedSmoothness;

    bool operator==(Layout const& other) const {
        return effect == other.effect && seconds == other.seconds &&
               diameter == other.diameter && beamCount == other.beamCount &&
               beamOrigin == other.beamOrigin && beamSweep == other.beamSweep &&
               orbs == other.orbs && orbEdges == other.orbEdges &&
               waveAmplitude == other.waveAmplitude && waveLength == other.waveLength &&
               reducedSmoothness == other.reducedSmoothness;
    }
    bool operator!=(Layout const& other) const { return !(*this == other); }
};

Layout CurrentLayout() {
    Layout layout{g_settings.effect,     g_settings.seconds,   g_settings.diameter,
                  g_settings.beamCount,  g_settings.beamOrigin, g_settings.beamSweep,
                  {},                    g_settings.orbEdges,  g_settings.waveAmplitude,
                  g_settings.waveLength, g_settings.reducedSmoothness};
    for (auto const& orb : g_settings.orbs) {
        layout.orbs.emplace_back(orb.angle, orb.speed);
    }
    return layout;
}

std::wstring ToLower(std::wstring value) {
    for (auto& c : value) {
        c = static_cast<wchar_t>(towlower(c));
    }
    return value;
}

std::wstring Trim(std::wstring const& text, PCWSTR characters = L" \t") {
    size_t first = text.find_first_not_of(characters);
    if (first == std::wstring::npos) {
        return {};
    }
    size_t last = text.find_last_not_of(characters);
    return text.substr(first, last - first + 1);
}

std::wstring NormalizeProcessName(std::wstring const& text) {
    std::wstring name = Trim(text, L" \t\"");
    size_t slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        name.erase(0, slash + 1);
    }
    return ToLower(std::move(name));
}

std::wstring TakeString(PCWSTR raw) {
    std::wstring result = raw ? raw : L"";
    if (raw) {
        Wh_FreeStringSetting(raw);
    }
    return result;
}

enum class NameList { Applications, Excluded };

std::vector<std::wstring> ReadNameList(NameList list) {
    std::vector<std::wstring> result;
    for (int i = 0; i < 1000; i++) {
        // Literal format strings only: never pass a variable as the format.
        std::wstring entry = TakeString(list == NameList::Excluded
                                            ? Wh_GetStringSetting(L"excludedApplications[%d]", i)
                                            : Wh_GetStringSetting(L"applications[%d]", i));
        if (entry.empty()) {
            break;
        }

        // Also accept "a.exe;b.exe" inside a single entry.
        size_t start = 0;
        while (start <= entry.size()) {
            size_t end = entry.find(L';', start);
            if (end == std::wstring::npos) {
                end = entry.size();
            }
            std::wstring name = NormalizeProcessName(entry.substr(start, end - start));
            if (!name.empty() &&
                std::find(result.begin(), result.end(), name) == result.end()) {
                result.push_back(std::move(name));
            }
            start = end + 1;
        }
    }
    return result;
}

Color ParseColor(std::wstring const& raw, unsigned fallback) {
    std::wstring text = Trim(raw, L" \t#");
    unsigned value = fallback;
    if (text.size() == 6) {
        unsigned candidate = 0;
        bool valid = true;
        for (wchar_t c : text) {
            int digit = c >= L'0' && c <= L'9'   ? c - L'0'
                        : c >= L'a' && c <= L'f' ? c - L'a' + 10
                        : c >= L'A' && c <= L'F' ? c - L'A' + 10
                                                 : -1;
            if (digit < 0) {
                valid = false;
                break;
            }
            candidate = (candidate << 4) | static_cast<unsigned>(digit);
        }
        if (valid) {
            value = candidate;
        }
    }
    return {255, static_cast<uint8_t>(value >> 16), static_cast<uint8_t>(value >> 8),
            static_cast<uint8_t>(value)};
}

void LoadSettings() {
    Settings& s = g_settings;

    std::wstring effect = TakeString(Wh_GetStringSetting(L"effect"));
    s.effect = effect == L"beams"   ? Effect::Beams
               : effect == L"orbs"  ? Effect::Orbs
               : effect == L"waves" ? Effect::Waves
                                    : Effect::Glows;

    std::wstring mode = TakeString(Wh_GetStringSetting(L"mode"));
    s.mode = mode == L"automatic"  ? Mode::Automatic
             : mode == L"combined" ? Mode::Combined
                                   : Mode::Selected;

    s.include = ReadNameList(NameList::Applications);
    s.exclude = ReadNameList(NameList::Excluded);
    s.colors = {ParseColor(TakeString(Wh_GetStringSetting(L"color1")), 0x00BFFF),
                ParseColor(TakeString(Wh_GetStringSetting(L"color2")), 0xA855F7),
                ParseColor(TakeString(Wh_GetStringSetting(L"color3")), 0xFF4081)};
    s.seconds = std::clamp(Wh_GetIntSetting(L"seconds"), 2, 120);
    s.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 1, 100);
    s.diameter = std::clamp(Wh_GetIntSetting(L"diameter"), 80, 1600);
    s.maxWindows = std::clamp(Wh_GetIntSetting(L"maxWindows"), 1, 100);

    s.beamCount = std::clamp(Wh_GetIntSetting(L"beams.count"), 1, 8);
    std::wstring origin = TakeString(Wh_GetStringSetting(L"beams.origin"));
    s.beamOrigin = origin == L"bottom" ? Origin::Bottom
                   : origin == L"left" ? Origin::Left
                   : origin == L"right" ? Origin::Right
                                        : Origin::Top;
    s.beamSweep = std::clamp(Wh_GetIntSetting(L"beams.sweep"), 0, 80);

    s.orbs.clear();
    for (int i = 0; i < kMaxOrbs; i++) {
        std::wstring color = Trim(TakeString(Wh_GetStringSetting(L"orbs[%d].color", i)));
        if (color.empty()) {
            break;
        }
        Orb orb;
        orb.color = ParseColor(color, 0xFFFFFF);
        orb.angle = ((Wh_GetIntSetting(L"orbs[%d].angle", i) % 360) + 360) % 360;
        orb.speed = std::clamp(Wh_GetIntSetting(L"orbs[%d].speed", i), 10, 400);
        s.orbs.push_back(orb);
    }
    s.orbEdges = TakeString(Wh_GetStringSetting(L"orbEdges")) == L"wrap" ? Edges::Wrap
                                                                         : Edges::Bounce;

    s.waveAmplitude = std::clamp(Wh_GetIntSetting(L"waves.amplitude"), 2, 45);
    s.waveLength = std::clamp(Wh_GetIntSetting(L"waves.length"), 25, 400);

    std::wstring animate = TakeString(Wh_GetStringSetting(L"animate"));
    s.animate = animate == L"active" ? Animate::Active
                : animate == L"none" ? Animate::None
                                     : Animate::All;
    s.reducedSmoothness = TakeString(Wh_GetStringSetting(L"smoothness")) == L"reduced";
    s.power.batterySaver = Wh_GetIntSetting(L"powerSaving.batterySaver") != 0;
    s.power.fullscreen = Wh_GetIntSetting(L"powerSaving.fullscreen") != 0;
    s.power.reduceMotion = Wh_GetIntSetting(L"powerSaving.reduceMotion") != 0;
    s.power.transparency = Wh_GetIntSetting(L"powerSaving.transparency") != 0;
}

bool Contains(std::vector<std::wstring> const& list, std::wstring const& name) {
    return std::find(list.begin(), list.end(), name) != list.end();
}

////////////////////////////////////////////////////////////////////////////////
// Motion helpers

float Frac(float value) {
    return value - std::floor(value);
}

// Angular speed (radians per second) close to one cycle every `seconds`,
// rounded so a whole number of cycles fits the clock loop.
float LoopFrequency(float seconds) {
    float cycles = std::max(1.0f, std::round(kLoopSeconds / std::max(seconds, 0.1f)));
    return kTwoPi * cycles / kLoopSeconds;
}

// Brings a time back into [0, kLoopSeconds).
float WrapTime(float seconds) {
    float wrapped = std::fmod(seconds, kLoopSeconds);
    return wrapped < 0 ? wrapped + kLoopSeconds : wrapped;
}

// Rounds a velocity (in periods per second) so the motion repeats exactly when
// the clock wraps around.
float QuantizeVelocity(float velocity, float period) {
    return std::round(velocity * kLoopSeconds / period) * period / kLoopSeconds;
}

////////////////////////////////////////////////////////////////////////////////
// Globals

HMODULE g_module = nullptr;
HANDLE g_workerThread = nullptr;
HANDLE g_stopEvent = nullptr;
// Published by the worker so settings changes can be posted to it.
std::atomic<HWND> g_controller{nullptr};

////////////////////////////////////////////////////////////////////////////////
// Composition resources shared by every effect window

struct GradientBrush {
    wc::CompositionBrush brush{nullptr};
    std::vector<wc::CompositionColorGradientStop> stops;
};

struct Resources {
    wc::Compositor compositor{nullptr};
    // "T": shared time in seconds, 0 to kLoopSeconds. It only runs while at
    // least one effect is live; frozen effects keep their own time.
    wc::CompositionPropertySet clock{nullptr};
    std::array<GradientBrush, 3> glowBrushes;  // Glows and waves.
    std::array<GradientBrush, 3> beamBrushes;
    std::vector<GradientBrush> orbBrushes;
    wc::ScalarKeyFrameAnimation fadeIn{nullptr};

    void Create() {
        compositor = wc::Compositor();

        clock = compositor.CreatePropertySet();
        clock.InsertScalar(L"T", 0.0f);
        clockRunning_ = false;

        for (size_t i = 0; i < 3; i++) {
            glowBrushes[i] = MakeRadialBrush();
            beamBrushes[i] = MakeBeamBrush();
        }
        orbBrushes.clear();
        for (size_t i = 0; i < g_settings.orbs.size(); i++) {
            orbBrushes.push_back(MakeRadialBrush());
        }
        ApplyColors();

        fadeIn = compositor.CreateScalarKeyFrameAnimation();
        fadeIn.InsertKeyFrame(0.0f, 0.0f);
        fadeIn.InsertKeyFrame(1.0f, 1.0f);
        fadeIn.Duration(std::chrono::milliseconds(kFadeInMs));
    }

    // Brushes are shared by all effect windows, so this recolors them all at
    // once without recreating anything.
    void ApplyColors() {
        auto alpha = static_cast<float>(g_settings.opacity * 255 / 100);
        for (size_t i = 0; i < 3; i++) {
            Tint(glowBrushes[i], g_settings.colors[i], alpha, {1.0f, 1.0f / 3.0f, 0.0f});
            Tint(beamBrushes[i], g_settings.colors[i], alpha, {0.0f, 0.35f, 1.0f, 0.35f, 0.0f});
        }
        for (size_t i = 0; i < orbBrushes.size() && i < g_settings.orbs.size(); i++) {
            Tint(orbBrushes[i], g_settings.orbs[i].color, alpha, {1.0f, 1.0f / 3.0f, 0.0f});
        }
    }

    bool ClockRunning() const { return clockRunning_; }

    // Full smoothness: the compositor animates the clock at the monitor's
    // refresh rate. Reduced: the worker updates it from a timer (TickClock).
    void StartClock(bool animated) {
        clockStart_ = std::chrono::steady_clock::now();
        clockAnimated_ = animated;
        clockRunning_ = true;
        clock.InsertScalar(L"T", 0.0f);
        if (animated) {
            auto linear = compositor.CreateLinearEasingFunction();
            auto tick = compositor.CreateScalarKeyFrameAnimation();
            tick.InsertKeyFrame(0.0f, 0.0f, linear);
            tick.InsertKeyFrame(1.0f, kLoopSeconds, linear);
            tick.Duration(std::chrono::seconds(static_cast<int>(kLoopSeconds)));
            tick.IterationBehavior(wc::AnimationIterationBehavior::Forever);
            clock.StartAnimation(L"T", tick);
        }
    }

    void StopClock() noexcept {
        if (clockRunning_ && clockAnimated_ && clock) {
            try {
                clock.StopAnimation(L"T");
            } catch (...) {
            }
        }
        clockRunning_ = false;
    }

    void TickClock() {
        if (clockRunning_ && !clockAnimated_) {
            clock.InsertScalar(L"T", ClockNow());
        }
    }

    // CPU-side estimate of the clock, used to freeze and resume effects
    // without a visible jump.
    float ClockNow() const {
        if (!clockRunning_) {
            return 0.0f;
        }
        std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - clockStart_;
        return static_cast<float>(std::fmod(elapsed.count(), static_cast<double>(kLoopSeconds)));
    }

    // Optional, only used to follow targets across virtual desktops. Created
    // lazily and dropped on failure, so it recovers after an Explorer restart.
    VirtualDesktopManager* Desktops() {
        if (desktops_) {
            return desktops_.get();
        }
        ULONGLONG now = GetTickCount64();
        if (now < desktopsRetryAt_) {
            return nullptr;
        }
        HRESULT hr = CoCreateInstance(kVirtualDesktopManagerClsid, nullptr, CLSCTX_ALL,
                                      kVirtualDesktopManagerIid, desktops_.put_void());
        if (FAILED(hr)) {
            desktops_ = nullptr;
            desktopsRetryAt_ = now + kDesktopsRetryMs;
            if (!desktopsErrorLogged_) {
                Wh_Log(L"Virtual desktop manager unavailable: %08X", static_cast<unsigned>(hr));
                desktopsErrorLogged_ = true;
            }
            return nullptr;
        }
        return desktops_.get();
    }

    void ResetDesktops() {
        desktops_ = nullptr;
        desktopsRetryAt_ = GetTickCount64() + kDesktopsRetryMs;
    }

    void Release() noexcept {
        StopClock();
        clock = nullptr;
        for (auto& brush : glowBrushes) {
            brush = GradientBrush{};
        }
        for (auto& brush : beamBrushes) {
            brush = GradientBrush{};
        }
        orbBrushes.clear();
        fadeIn = nullptr;
        desktops_ = nullptr;
        if (compositor) {
            try {
                compositor.Close();
            } catch (...) {
            }
            compositor = nullptr;
        }
    }

   private:
    static void Tint(GradientBrush& target, Color color, float alpha,
                     std::initializer_list<float> factors) {
        size_t i = 0;
        for (float factor : factors) {
            if (i >= target.stops.size()) {
                break;
            }
            color.A = static_cast<uint8_t>(std::clamp(alpha * factor + 0.5f, 0.0f, 255.0f));
            target.stops[i++].Color(color);
        }
    }

    // Soft disc: full color in the middle, fading out to the edge.
    GradientBrush MakeRadialBrush() {
        GradientBrush result;
        auto brush = compositor.CreateRadialGradientBrush();
        brush.EllipseCenter({0.5f, 0.5f});
        brush.EllipseRadius({0.5f, 0.5f});
        for (float offset : {0.0f, 0.55f, 1.0f}) {
            auto stop = compositor.CreateColorGradientStop();
            stop.Offset(offset);
            brush.ColorStops().Append(stop);
            result.stops.push_back(stop);
        }
        result.brush = brush;
        return result;
    }

    // Beam: bright core fading to the sides, masked so it also fades out
    // along its length, away from the source.
    GradientBrush MakeBeamBrush() {
        GradientBrush result;
        auto across = compositor.CreateLinearGradientBrush();
        across.StartPoint({0.0f, 0.0f});
        across.EndPoint({1.0f, 0.0f});
        for (float offset : {0.0f, 0.35f, 0.5f, 0.65f, 1.0f}) {
            auto stop = compositor.CreateColorGradientStop();
            stop.Offset(offset);
            across.ColorStops().Append(stop);
            result.stops.push_back(stop);
        }

        Color opaque{255, 255, 255, 255};
        Color clear{0, 255, 255, 255};
        auto along = compositor.CreateLinearGradientBrush();
        along.StartPoint({0.0f, 0.0f});
        along.EndPoint({0.0f, 1.0f});
        along.ColorStops().Append(compositor.CreateColorGradientStop(0.0f, opaque));
        along.ColorStops().Append(compositor.CreateColorGradientStop(1.0f, clear));

        auto mask = compositor.CreateMaskBrush();
        mask.Source(across);
        mask.Mask(along);
        result.brush = mask;
        return result;
    }

    std::chrono::steady_clock::time_point clockStart_{};
    bool clockRunning_ = false;
    bool clockAnimated_ = false;
    winrt::com_ptr<VirtualDesktopManager> desktops_;
    ULONGLONG desktopsRetryAt_ = 0;
    bool desktopsErrorLogged_ = false;
};

////////////////////////////////////////////////////////////////////////////////
// One effect window placed directly below a target window

class Backdrop {
   public:
    Backdrop(HWND target, DWORD targetPid, Resources& resources)
        : target_(target), targetPid_(targetPid), resources_(resources) {
        try {
            // Deliberately unowned: owned windows always stay above their owner.
            // WS_EX_LAYERED + WS_EX_TRANSPARENT makes it click-through for every
            // process. WS_EX_TRANSPARENT alone (or HTTRANSPARENT) only passes
            // clicks to windows of the same thread, so while the effect briefly
            // lags behind a moving window it would swallow clicks.
            window_ = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW |
                                          WS_EX_NOACTIVATE | WS_EX_TRANSPARENT |
                                          WS_EX_LAYERED,
                                      kClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr,
                                      nullptr, g_module, nullptr);
            if (!window_) {
                winrt::throw_last_error();
            }
            if (!SetLayeredWindowAttributes(window_, 0, 255, LWA_ALPHA)) {
                winrt::throw_last_error();
            }

            BOOL disableTransitions = TRUE;
            DwmSetWindowAttribute(window_, kDwmaTransitionsForceDisabled, &disableTransitions,
                                  sizeof(disableTransitions));
            COLORREF border = kColorNone;
            DwmSetWindowAttribute(window_, kDwmaBorderColor, &border, sizeof(border));

            winrt::com_ptr<DesktopInterop> interop;
            winrt::check_hresult(
                static_cast<::IUnknown*>(winrt::get_abi(resources_.compositor))
                    ->QueryInterface(kDesktopInteropIid, interop.put_void()));
            winrt::check_hresult(interop->CreateDesktopWindowTarget(
                window_, FALSE, winrt::put_abi(compositionTarget_)));

            root_ = resources_.compositor.CreateContainerVisual();
            compositionTarget_.Root(root_);

            // Per-window values read by the expressions: "Scale" is DPI / 96
            // and "T" this window's time, linked to the shared clock while live
            // and fixed while frozen.
            frozenTime_ = resources_.ClockNow();
            env_ = resources_.compositor.CreatePropertySet();
            env_.InsertScalar(L"Scale", scale_);
            env_.InsertScalar(L"T", frozenTime_);

            switch (g_settings.effect) {
                case Effect::Glows:
                    BuildGlows();
                    break;
                case Effect::Beams:
                    BuildBeams();
                    break;
                case Effect::Orbs:
                    BuildOrbs();
                    break;
                case Effect::Waves:
                    BuildWaves();
                    break;
            }
        } catch (...) {
            // A constructor that throws doesn't run the destructor.
            Release();
            throw;
        }
    }

    ~Backdrop() { Release(); }

    Backdrop(Backdrop const&) = delete;
    Backdrop& operator=(Backdrop const&) = delete;

    DWORD TargetPid() const { return targetPid_; }
    bool Shown() const { return shown_; }

    // Live: the effect follows the shared clock. Frozen: it stays still, and
    // later resumes from the same point. `now` is the shared clock time.
    void SetLive(bool live, float now) {
        if (live == live_) {
            return;
        }
        if (live) {
            // The first time, join the shared clock so windows stay in sync.
            float offset = neverLive_ ? 0.0f : WrapTime(now - frozenTime_);
            auto link = resources_.compositor.CreateExpressionAnimation(
                L"Mod(clock.T - offset + 2 * loop, loop)");
            link.SetReferenceParameter(L"clock", resources_.clock);
            link.SetScalarParameter(L"offset", offset);
            link.SetScalarParameter(L"loop", kLoopSeconds);
            env_.StartAnimation(L"T", link);
            offset_ = offset;
            neverLive_ = false;
        } else {
            frozenTime_ = WrapTime(now - offset_);
            env_.StopAnimation(L"T");
            env_.InsertScalar(L"T", frozenTime_);
        }
        live_ = live;
    }

    void Hide() {
        if (shown_ && window_) {
            ShowWindow(window_, SW_HIDE);
        }
        shown_ = false;
        settleUntil_ = 0;
    }

    // Returns true if the caller should refresh again after the settle delay.
    bool Update(ULONGLONG now) {
        DWORD cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(target_, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
            cloaked = 0;
        }
        if (!IsWindow(target_) || !IsWindowVisible(target_) || IsIconic(target_) || cloaked) {
            Hide();
            zoomState_ = -1;
            return false;
        }

        RECT rect{};
        if (FAILED(DwmGetWindowAttribute(target_, DWMWA_EXTENDED_FRAME_BOUNDS, &rect,
                                         sizeof(rect))) ||
            IsRectEmpty(&rect)) {
            if (!GetWindowRect(target_, &rect)) {
                Hide();
                return false;
            }
        }

        // A maximized window extends past its monitor; only the work area is
        // visible, so never let the effect spill onto a neighbor monitor or the
        // taskbar.
        int zoomed = IsZoomed(target_) ? 1 : 0;
        if (zoomed) {
            MONITORINFO info{};
            info.cbSize = sizeof(info);
            if (GetMonitorInfoW(MonitorFromWindow(target_, MONITOR_DEFAULTTONEAREST), &info)) {
                IntersectRect(&rect, &rect, &info.rcWork);
            }
        }

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        if (width <= 0 || height <= 0) {
            Hide();
            return false;
        }

        // DWM animates maximize/restore after the window already has its final
        // rectangle. Hide during the animation instead of showing the effect
        // early.
        if (shown_ && zoomState_ != -1 && zoomed != zoomState_) {
            zoomState_ = zoomed;
            Hide();
            settleUntil_ = now + kSettleDelayMs;
            return true;
        }
        zoomState_ = zoomed;

        // Same for opening and restoring from the taskbar.
        if (!shown_) {
            if (!settleUntil_) {
                settleUntil_ = now + kSettleDelayMs;
            }
            if (now < settleUntil_) {
                return true;
            }
        }

        int corner = zoomed ? kCornerDoNotRound : kCornerRound;
        if (corner != cornerState_) {
            DwmSetWindowAttribute(window_, kDwmaWindowCornerPreference, &corner, sizeof(corner));
            cornerState_ = corner;
        }

        // The worker thread is per-monitor DPI aware, so rect is in physical
        // pixels and the monitor DPI is the right scale for sizes in DIP.
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (FAILED(GetDpiForMonitor(MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST),
                                    MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) ||
            !dpiX) {
            dpiX = 96;
        }
        float scale = static_cast<float>(dpiX) / 96.0f;
        if (scale != scale_) {
            env_.InsertScalar(L"Scale", scale);
            scale_ = scale;
        }

        // Every sprite size and position is an expression of the root size, so
        // resizing the root is all a layout change needs.
        float w = static_cast<float>(width);
        float h = static_cast<float>(height);
        auto rootSize = root_.Size();
        if (rootSize.x != w || rootSize.y != h) {
            root_.Size({w, h});
        }

        bool wasShown = shown_;
        if (!wasShown) {
            root_.Opacity(0.0f);
            root_.StartAnimation(L"Opacity", resources_.fadeIn);
        }

        bool moved = !EqualRect(&rect, &bounds_);
        bool orderChanged = GetWindow(target_, GW_HWNDNEXT) != window_;
        if (moved || orderChanged || !wasShown) {
            if (!SetWindowPos(window_, target_, rect.left, rect.top, width, height,
                              SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW)) {
                return false;
            }
            bounds_ = rect;
        }
        shown_ = true;
        settleUntil_ = 0;

        SyncVirtualDesktop();
        return false;
    }

   private:
    wc::SpriteVisual AddSprite(wc::CompositionBrush const& brush) {
        auto sprite = resources_.compositor.CreateSpriteVisual();
        sprite.Brush(brush);
        root_.Children().InsertAtTop(sprite);
        sprites_.push_back(sprite);
        return sprite;
    }

    // Starts an expression on a sprite property. Every expression can use
    // "area" (the window), "env" (per-window values, env.T is the time in
    // seconds) and "blob" (the sprite itself).
    // Extra references must be set before starting: they aren't picked up later.
    void Animate(wc::SpriteVisual const& sprite, PCWSTR property, PCWSTR expression,
                 std::initializer_list<std::pair<PCWSTR, float>> values,
                 std::initializer_list<std::pair<PCWSTR, wc::CompositionObject>> references = {}) {
        auto animation = resources_.compositor.CreateExpressionAnimation(expression);
        animation.SetReferenceParameter(L"area", root_);
        animation.SetReferenceParameter(L"env", env_);
        animation.SetReferenceParameter(L"blob", sprite);
        for (auto const& [name, value] : values) {
            animation.SetScalarParameter(name, value);
        }
        for (auto const& [name, object] : references) {
            animation.SetReferenceParameter(name, object);
        }
        sprite.StartAnimation(property, animation);
    }

    // Each glow travels left and right at constant speed along a triangle wave
    // of period 2 (0 -> 1 -> 0), starting a third of the way apart.
    void BuildGlows() {
        float base = static_cast<float>(g_settings.diameter);
        for (size_t i = 0; i < 3; i++) {
            float roundTrip = static_cast<float>(g_settings.seconds) + 1.7f * i;
            auto sprite = AddSprite(resources_.glowBrushes[i].brush);
            Animate(sprite, L"Size",
                    L"Vector2(base * env.Scale, Max(base * env.Scale, area.Size.Y * 1.8))",
                    {{L"base", base}});
            Animate(sprite, L"Offset",
                    L"Vector3(area.Size.X * (1 - Abs(Mod(phase + rate * env.T, 2) - 1)) - "
                    L"blob.Size.X * 0.5, (area.Size.Y - blob.Size.Y) * 0.5, 0)",
                    {{L"phase", static_cast<float>(i) / 3.0f},
                     {L"rate", QuantizeVelocity(2.0f / roundTrip, 2.0f)}});
        }
    }

    // Beams start at evenly spaced points on one edge, fan out slightly and
    // sweep back and forth around their source point.
    void BuildBeams() {
        auto const& s = g_settings;
        float base = static_cast<float>(s.diameter);
        int count = s.beamCount;
        for (int k = 0; k < count; k++) {
            float t = (static_cast<float>(k) + 0.5f) / static_cast<float>(count);
            float ax = 0, ay = 0, heading = 0, fan = 0;
            switch (s.beamOrigin) {
                case Origin::Top:
                    ax = t, ay = 0, heading = 0, fan = (t - 0.5f) * 30.0f;
                    break;
                case Origin::Bottom:
                    ax = t, ay = 1, heading = 180, fan = (0.5f - t) * 30.0f;
                    break;
                case Origin::Left:
                    ax = 0, ay = t, heading = -90, fan = (0.5f - t) * 30.0f;
                    break;
                case Origin::Right:
                    ax = 1, ay = t, heading = 90, fan = (t - 0.5f) * 30.0f;
                    break;
            }

            auto sprite = AddSprite(resources_.beamBrushes[k % 3].brush);
            Animate(sprite, L"Size", L"Vector2(base * env.Scale * 0.5, Length(area.Size) * 1.25)",
                    {{L"base", base}});
            // The pivot is the middle of the beam's source end.
            Animate(sprite, L"CenterPoint", L"Vector3(blob.Size.X * 0.5, 0, 0)", {});
            Animate(sprite, L"Offset",
                    L"Vector3(ax * area.Size.X - blob.Size.X * 0.5, ay * area.Size.Y, 0)",
                    {{L"ax", ax}, {L"ay", ay}});
            Animate(sprite, L"RotationAngleInDegrees", L"heading + sweep * Sin(env.T * omega + phase)",
                    {{L"heading", heading + fan},
                     {L"sweep", static_cast<float>(s.beamSweep)},
                     {L"omega", LoopFrequency(s.seconds * (1.0f + 0.23f * k))},
                     {L"phase", 2.4f * k}});
        }
    }

    // Orbs move in straight lines. Positions are in "periods": with bounce, a
    // triangle wave of period 2 maps them back and forth between the edges;
    // with wrap, the fractional part maps them across and around.
    void BuildOrbs() {
        auto const& s = g_settings;
        float base = static_cast<float>(s.diameter);
        bool wrap = s.orbEdges == Edges::Wrap;
        float period = wrap ? 1.0f : 2.0f;
        PCWSTR offset =
            wrap ? L"Vector3("
                   L"(area.Size.X + blob.Size.X) * Mod(Mod(sx + vx * env.T, 1) + 1, 1) - "
                   L"blob.Size.X, "
                   L"(area.Size.Y + blob.Size.Y) * Mod(Mod(sy + vy * env.T, 1) + 1, 1) - "
                   L"blob.Size.Y, 0)"
                 : L"Vector3("
                   L"(area.Size.X - blob.Size.X) * Abs(Mod(Abs(sx + vx * env.T), 2) - 1), "
                   L"(area.Size.Y - blob.Size.Y) * Abs(Mod(Abs(sy + vy * env.T), 2) - 1), 0)";

        for (size_t j = 0; j < s.orbs.size() && j < resources_.orbBrushes.size(); j++) {
            auto const& orb = s.orbs[j];
            // One full back-and-forth (bounce) or crossing (wrap) per cycle at
            // 100% speed.
            float speed = period / static_cast<float>(s.seconds) * orb.speed / 100.0f;
            float radians = static_cast<float>(orb.angle) * kPi / 180.0f;
            float vx = QuantizeVelocity(std::cos(radians) * speed, period);
            float vy = QuantizeVelocity(-std::sin(radians) * speed, period);  // Screen y is down.
            float fj = static_cast<float>(j);

            auto sprite = AddSprite(resources_.orbBrushes[j].brush);
            Animate(sprite, L"Size", L"Vector2(base * env.Scale, base * env.Scale)",
                    {{L"base", base}});
            Animate(sprite, L"Offset", offset,
                    {{L"sx", Frac(0.21f + 0.618034f * fj) * period},
                     {L"sy", Frac(0.67f + 0.381966f * fj) * period},
                     {L"vx", vx},
                     {L"vy", vy}});
        }
    }

    // Each wave is a row of overlapping soft discs whose heights follow a
    // traveling sine; behind the blur they merge into one flowing band.
    void BuildWaves() {
        auto const& s = g_settings;
        float base = static_cast<float>(s.diameter);
        float amplitude = s.waveAmplitude / 100.0f;
        float spatial = kTwoPi / (s.waveLength / 100.0f);
        float span = 1.1f * 2.4f / static_cast<float>(kWaveBlobs - 1);
        for (int k = 0; k < kWaveCount; k++) {
            float direction = k % 2 ? -1.0f : 1.0f;
            float w = direction * LoopFrequency(s.seconds * (1.0f + 0.25f * k));
            for (int j = 0; j < kWaveBlobs; j++) {
                float x = -0.05f + 1.1f * static_cast<float>(j) / (kWaveBlobs - 1);
                auto sprite = AddSprite(resources_.glowBrushes[k].brush);
                Animate(sprite, L"Size", L"Vector2(area.Size.X * span, base * env.Scale * 0.6)",
                        {{L"span", span}, {L"base", base}});
                Animate(sprite, L"Offset",
                        L"Vector3(area.Size.X * px - blob.Size.X * 0.5, "
                        L"area.Size.Y * (py + amp * Sin(kx * px - omega * env.T + phase)) - "
                        L"blob.Size.Y * 0.5, 0)",
                        {{L"px", x},
                         {L"py", 0.3f + 0.2f * k},
                         {L"amp", amplitude},
                         {L"kx", spatial},
                         {L"omega", w},
                         {L"phase", 1.9f * k}});
            }
        }
    }

    // The effect window belongs to the virtual desktop it was created on. If
    // the shell cloaks it because the target lives on another desktop, move it.
    void SyncVirtualDesktop() {
        // Cheap local check first: the cross-process COM call below only runs
        // when the shell actually cloaked the effect window.
        DWORD ownCloak = 0;
        if (FAILED(DwmGetWindowAttribute(window_, DWMWA_CLOAKED, &ownCloak, sizeof(ownCloak))) ||
            !(ownCloak & kCloakedShell)) {
            return;
        }
        VirtualDesktopManager* desktops = resources_.Desktops();
        if (!desktops) {
            return;
        }
        GUID desktop{};
        GUID none{};
        HRESULT hr = desktops->GetWindowDesktopId(target_, &desktop);
        if (SUCCEEDED(hr) && !IsEqualGUID(desktop, none)) {
            hr = desktops->MoveWindowToDesktop(window_, desktop);
        }
        if (FAILED(hr)) {
            // Typically a stale proxy after Explorer restarted.
            resources_.ResetDesktops();
        }
    }

    void Release() noexcept {
        try {
            for (auto const& sprite : sprites_) {
                sprite.StopAnimation(L"Offset");
                sprite.StopAnimation(L"Size");
                sprite.StopAnimation(L"CenterPoint");
                sprite.StopAnimation(L"RotationAngleInDegrees");
            }
            if (env_) {
                env_.StopAnimation(L"T");
            }
            if (root_) {
                root_.StopAnimation(L"Opacity");
                root_.Children().RemoveAll();
            }
            if (compositionTarget_) {
                compositionTarget_.Root(nullptr);
            }
        } catch (...) {
        }
        sprites_.clear();
        env_ = nullptr;
        root_ = nullptr;
        compositionTarget_ = nullptr;
        if (window_) {
            DestroyWindow(window_);
            window_ = nullptr;
        }
    }

    HWND target_;
    DWORD targetPid_;
    Resources& resources_;
    HWND window_ = nullptr;
    wc::Desktop::DesktopWindowTarget compositionTarget_{nullptr};
    wc::ContainerVisual root_{nullptr};
    wc::CompositionPropertySet env_{nullptr};
    std::vector<wc::SpriteVisual> sprites_;
    float scale_ = 1.0f;
    float frozenTime_ = 0.0f;
    float offset_ = 0.0f;
    bool live_ = false;
    bool neverLive_ = true;
    RECT bounds_{};
    bool shown_ = false;
    int cornerState_ = -1;
    int zoomState_ = -1;
    ULONGLONG settleUntil_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Worker state. Everything below runs on the worker thread only.

// Holding the process handle keeps the PID from being reused by another
// process while its name is cached.
class ProcessEntry {
   public:
    ProcessEntry() = default;
    ~ProcessEntry() {
        if (handle_) {
            CloseHandle(handle_);
        }
    }
    ProcessEntry(ProcessEntry const&) = delete;
    ProcessEntry& operator=(ProcessEntry const&) = delete;

    void Open(DWORD pid);
    bool IsOpen() const { return handle_ != nullptr; }
    std::wstring const& Name() const { return name_; }

   private:
    HANDLE handle_ = nullptr;
    std::wstring name_;
};

// Conditions read from Windows on every refresh.
struct PowerState {
    bool freeze = false;  // Keep effects visible but still.
    bool hide = false;    // Remove effects: they couldn't be seen anyway.
};

PowerState QueryPowerState() {
    auto const& power = g_settings.power;
    PowerState state;

    if (power.transparency) {
        DWORD value = 1;
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER,
                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                         L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value,
                         &size) == ERROR_SUCCESS &&
            value == 0) {
            state.hide = true;
        }
    }

    if (power.batterySaver) {
        SYSTEM_POWER_STATUS status{};
        // The 4th byte is SystemStatusFlag (named Reserved1 in older headers):
        // 1 means battery saver is on.
        if (GetSystemPowerStatus(&status) && reinterpret_cast<BYTE const*>(&status)[3] == 1) {
            state.freeze = true;
        }
    }

    if (power.reduceMotion) {
        BOOL animations = TRUE;
        if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animations, 0) && !animations) {
            state.freeze = true;
        }
    }

    if (power.fullscreen) {
        QUERY_USER_NOTIFICATION_STATE notification{};
        if (SUCCEEDED(SHQueryUserNotificationState(&notification)) &&
            (notification == QUNS_BUSY || notification == QUNS_RUNNING_D3D_FULL_SCREEN ||
             notification == QUNS_PRESENTATION_MODE)) {
            state.freeze = true;
        }
    }

    return state;
}

struct Worker {
    Resources resources;  // Declared first: destroyed after the backdrops.
    std::unordered_map<HWND, std::unique_ptr<Backdrop>> backdrops;
    std::unordered_set<HWND> failed;
    std::unordered_map<DWORD, ProcessEntry> processes;
    HWND controller = nullptr;
    PowerState power;
    ULONGLONG failedResetAt = 0;
    bool refreshQueued = false;
    bool settingsPending = false;
    bool busy = false;
};

Worker* g_worker = nullptr;

struct BusyScope {
    explicit BusyScope(bool& flag) : flag_(flag) { flag_ = true; }
    ~BusyScope() { flag_ = false; }
    BusyScope(BusyScope const&) = delete;
    BusyScope& operator=(BusyScope const&) = delete;

   private:
    bool& flag_;
};

void QueueRefresh(Worker& worker) {
    if (worker.refreshQueued || !worker.controller) {
        return;
    }
    if (PostMessageW(worker.controller, kRefreshMessage, 0, 0)) {
        worker.refreshQueued = true;
    }
}

void ArmSettleTimer(Worker& worker) {
    if (worker.controller) {
        SetTimer(worker.controller, kSettleTimerId, kSettleDelayMs + 20, nullptr);
    }
}

void StartClock(Worker& worker) {
    worker.resources.StartClock(!g_settings.reducedSmoothness);
    if (g_settings.reducedSmoothness && worker.controller) {
        SetTimer(worker.controller, kFrameTimerId, kFrameIntervalMs, nullptr);
    }
}

void StopClock(Worker& worker) {
    if (worker.controller) {
        KillTimer(worker.controller, kFrameTimerId);
    }
    worker.resources.StopClock();
}

// Decides which effects move. Hidden effects, and the ones the user or the
// power settings want still, are frozen; the clock runs only if needed.
void UpdateLiveness(Worker& worker) {
    auto const& s = g_settings;
    HWND foreground = GetForegroundWindow();
    HWND foregroundRoot = foreground ? GetAncestor(foreground, GA_ROOTOWNER) : nullptr;

    std::vector<std::pair<Backdrop*, bool>> states;
    states.reserve(worker.backdrops.size());
    bool anyLive = false;
    for (auto const& [hwnd, backdrop] : worker.backdrops) {
        bool live = !worker.power.freeze && s.animate != Animate::None && backdrop->Shown() &&
                    (s.animate == Animate::All || hwnd == foreground || hwnd == foregroundRoot);
        anyLive = anyLive || live;
        states.emplace_back(backdrop.get(), live);
    }

    if (anyLive && !worker.resources.ClockRunning()) {
        StartClock(worker);
    }
    // Freeze before the clock can stop: freezing reads the current time.
    float now = worker.resources.ClockNow();
    for (auto const& [backdrop, live] : states) {
        if (!live) {
            backdrop->SetLive(false, now);
        }
    }
    for (auto const& [backdrop, live] : states) {
        if (live) {
            backdrop->SetLive(true, now);
        }
    }
    if (!anyLive && worker.resources.ClockRunning()) {
        StopClock(worker);
    }
}

std::wstring QueryProcessName(HANDLE process) {
    std::wstring path(MAX_PATH, L'\0');
    DWORD size = static_cast<DWORD>(path.size());
    BOOL ok = QueryFullProcessImageNameW(process, 0, path.data(), &size);
    if (!ok && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        path.assign(32768, L'\0');
        size = static_cast<DWORD>(path.size());
        ok = QueryFullProcessImageNameW(process, 0, path.data(), &size);
    }
    if (!ok) {
        return {};
    }
    path.resize(size);
    return NormalizeProcessName(path);
}

void ProcessEntry::Open(DWORD pid) {
    handle_ = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (handle_) {
        name_ = QueryProcessName(handle_);
    }
}

bool IsBasicCandidate(Worker const& worker, HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    // Ignore invisible helper windows, but keep tracked targets so minimizing
    // can hide their effect without recreating it.
    if (!IsWindowVisible(hwnd) && !worker.backdrops.count(hwnd)) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == GetCurrentProcessId()) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    // Click-through windows are overlays, not app windows.
    if ((style & WS_CHILD) || (exStyle & (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT))) {
        return false;
    }
    // Only app windows and dialogs: they have a caption or a sizing frame, or
    // stand on their own in the taskbar. Tooltips, menus, flyouts and drop-down
    // lists are borderless popups, usually owned or topmost, and never take
    // activation; an effect window behind them shows up as a stray shape.
    if (exStyle & WS_EX_NOACTIVATE) {
        return false;
    }
    bool framed = (style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME);
    bool owned = GetWindow(hwnd, GW_OWNER) != nullptr;
    if (!framed && !(exStyle & WS_EX_APPWINDOW) && (owned || (exStyle & WS_EX_TOPMOST))) {
        return false;
    }
    if (exStyle & WS_EX_LAYERED) {
        BYTE alpha = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) && (flags & LWA_ALPHA) &&
            alpha == 0) {
            return false;  // Fully transparent window.
        }
    }

    wchar_t className[256]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }
    for (PCWSTR blocked : kExcludedClasses) {
        if (_wcsicmp(className, blocked) == 0) {
            return false;
        }
    }
    return true;
}

bool Matches(HWND hwnd, std::wstring const& processName) {
    if (processName.empty() || Contains(g_settings.exclude, processName)) {
        return false;
    }

    bool listed = Contains(g_settings.include, processName);
    if (g_settings.mode == Mode::Selected) {
        return listed;
    }
    if (g_settings.mode == Mode::Combined && listed) {
        return true;
    }

    int backdropType = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, kDwmaSystemBackdropType, &backdropType,
                                           sizeof(backdropType))) &&
           backdropType == kBackdropTransientWindow;
}

bool CanCreateFor(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }
    DWORD cloaked = 0;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        cloaked = 0;
    }
    if (cloaked) {
        return false;
    }
    // Skip tiny helper windows: a composition target each would be wasted.
    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) {
        return false;
    }
    return rect.right - rect.left >= kMinTargetSize && rect.bottom - rect.top >= kMinTargetSize;
}

struct EnumContext {
    Worker* worker;
    std::vector<HWND> targets;
    std::unordered_set<DWORD> seenPids;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param) {
    auto& context = *reinterpret_cast<EnumContext*>(param);
    try {
        if (!IsBasicCandidate(*context.worker, hwnd)) {
            return TRUE;
        }
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        context.seenPids.insert(pid);

        // Cached across refreshes; pruned when the PID is no longer seen.
        auto [it, inserted] = context.worker->processes.try_emplace(pid);
        if (inserted) {
            it->second.Open(pid);
        }
        if (Matches(hwnd, it->second.Name())) {
            context.targets.push_back(hwnd);
        }
    } catch (...) {
    }
    return TRUE;
}

void Refresh(Worker& worker) {
    if (worker.busy) {
        QueueRefresh(worker);
        return;
    }
    BusyScope scope(worker.busy);

    // Composition resources are missing only after a failed settings change.
    if (!worker.resources.compositor) {
        worker.backdrops.clear();
        worker.failed.clear();
        StopClock(worker);
        return;
    }

    worker.power = QueryPowerState();
    if (worker.power.hide) {
        worker.backdrops.clear();
        worker.failed.clear();
        StopClock(worker);
        return;
    }

    EnumContext context{&worker, {}, {}};
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&context));

    // Entries that couldn't be opened don't pin their PID, so they are never
    // trusted across refreshes and are simply queried again.
    for (auto it = worker.processes.begin(); it != worker.processes.end();) {
        if (context.seenPids.count(it->first) && it->second.IsOpen()) {
            ++it;
        } else {
            it = worker.processes.erase(it);
        }
    }

    std::unordered_set<HWND> wanted(context.targets.begin(), context.targets.end());

    for (auto it = worker.backdrops.begin(); it != worker.backdrops.end();) {
        DWORD pid = 0;
        GetWindowThreadProcessId(it->first, &pid);
        if (wanted.count(it->first) && pid == it->second->TargetPid()) {
            ++it;
        } else {
            it = worker.backdrops.erase(it);
        }
    }

    ULONGLONG now = GetTickCount64();
    if (now >= worker.failedResetAt) {
        // Transient failures (e.g. after a GPU reset) shouldn't be permanent.
        worker.failed.clear();
        worker.failedResetAt = now + kFailedRetryMs;
    }
    for (auto it = worker.failed.begin(); it != worker.failed.end();) {
        if (wanted.count(*it)) {
            ++it;
        } else {
            it = worker.failed.erase(it);
        }
    }

    // EnumWindows is in z-order, so when the limit is reached, existing effects
    // keep their slot and new windows nearer the top are served first.
    // If the limit was lowered, drop the effects lowest in z-order first.
    size_t limit = static_cast<size_t>(g_settings.maxWindows);
    for (auto it = context.targets.rbegin();
         it != context.targets.rend() && worker.backdrops.size() > limit; ++it) {
        worker.backdrops.erase(*it);
    }

    bool settle = false;
    for (HWND hwnd : context.targets) {
        try {
            auto it = worker.backdrops.find(hwnd);
            if (it == worker.backdrops.end()) {
                if (worker.backdrops.size() >= limit || worker.failed.count(hwnd) ||
                    !CanCreateFor(hwnd)) {
                    continue;
                }
                DWORD pid = 0;
                GetWindowThreadProcessId(hwnd, &pid);
                it = worker.backdrops
                         .emplace(hwnd, std::make_unique<Backdrop>(hwnd, pid, worker.resources))
                         .first;
                wchar_t className[128]{};
                GetClassNameW(hwnd, className, ARRAYSIZE(className));
                Wh_Log(L"Effect added to window %p (%s)", hwnd, className);
            }
            if (it->second->Update(now)) {
                settle = true;
            }
        } catch (...) {
            // Don't retry every second and flood the log.
            Wh_Log(L"Can't show the effect for window %p: %08X", hwnd,
                   static_cast<unsigned>(winrt::to_hresult()));
            worker.backdrops.erase(hwnd);
            worker.failed.insert(hwnd);
        }
    }

    UpdateLiveness(worker);

    if (settle) {
        ArmSettleTimer(worker);
    }
}

// A settings change that arrived while busy (for example dispatched by a COM
// modal loop during a cross-process call) is applied right after.
void FlushPendingSettings(Worker& worker) {
    if (worker.settingsPending && !worker.busy && worker.controller &&
        PostMessageW(worker.controller, kSettingsMessage, 0, 0)) {
        worker.settingsPending = false;
    }
}

void SafeRefresh(Worker& worker) {
    try {
        Refresh(worker);
    } catch (...) {
        Wh_Log(L"Refresh error: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
    FlushPendingSettings(worker);
}

void CALLBACK OnWinEvent(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                         DWORD, DWORD) {
    Worker* worker = g_worker;
    if (!worker || !hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    try {
        auto it = worker->backdrops.find(hwnd);
        bool tracked = it != worker->backdrops.end();

        switch (event) {
            case EVENT_OBJECT_LOCATIONCHANGE:
                // Fast path: follow a tracked window without enumerating.
                if (!tracked) {
                    return;
                }
                if (worker->busy) {
                    QueueRefresh(*worker);
                    return;
                }
                {
                    BusyScope scope(worker->busy);
                    bool wasShown = it->second->Shown();
                    if (it->second->Update(GetTickCount64())) {
                        ArmSettleTimer(*worker);
                    }
                    if (it->second->Shown() != wasShown) {
                        QueueRefresh(*worker);  // Updates which effects move.
                    }
                }
                FlushPendingSettings(*worker);
                return;

            case EVENT_OBJECT_DESTROY:
            case EVENT_OBJECT_HIDE:
            case EVENT_OBJECT_CLOAKED:
            case EVENT_SYSTEM_MINIMIZESTART:
                if (tracked) {
                    if (!worker->busy) {
                        it->second->Hide();
                    }
                    QueueRefresh(*worker);
                }
                return;

            default:
                // Any foreground change matters: with "active window only" the
                // previous window must freeze even if the new one isn't ours.
                if (event == EVENT_SYSTEM_FOREGROUND || tracked ||
                    IsBasicCandidate(*worker, hwnd)) {
                    QueueRefresh(*worker);
                }
                return;
        }
    } catch (...) {
    }
}

void ApplySettings(Worker& worker) {
    try {
        Layout before = CurrentLayout();
        LoadSettings();
        worker.failed.clear();  // Give previously failed windows another chance.
        if (CurrentLayout() != before || !worker.resources.compositor) {
            // The shape of the effect changed: rebuild every effect window.
            // They fade back in after the usual settle delay.
            worker.backdrops.clear();
            StopClock(worker);
            worker.resources.Release();
            worker.resources.Create();
        } else {
            worker.resources.ApplyColors();
        }
        Wh_Log(L"Settings applied");
    } catch (...) {
        Wh_Log(L"Can't apply settings: %08X", static_cast<unsigned>(winrt::to_hresult()));
        // Leave a clean state: the next settings change rebuilds from scratch.
        worker.backdrops.clear();
        StopClock(worker);
        worker.resources.Release();
    }
    SafeRefresh(worker);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Worker* worker = g_worker;
    if (worker && hwnd == worker->controller) {
        if (msg == kSettingsMessage) {
            if (worker->busy) {
                worker->settingsPending = true;
            } else {
                ApplySettings(*worker);
            }
            return 0;
        }
        if (msg == kRefreshMessage) {
            worker->refreshQueued = false;
            SafeRefresh(*worker);
            return 0;
        }
        if (msg == WM_TIMER && wParam == kFrameTimerId) {
            try {
                worker->resources.TickClock();
            } catch (...) {
            }
            return 0;
        }
        if (msg == WM_TIMER && (wParam == kPollTimerId || wParam == kSettleTimerId)) {
            if (wParam == kSettleTimerId) {
                KillTimer(hwnd, kSettleTimerId);
            }
            SafeRefresh(*worker);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    // Effect windows: never take clicks or activation.
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void RunMessageLoop() {
    for (;;) {
        DWORD result = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT,
                                                   MWMO_INPUTAVAILABLE);
        if (result != WAIT_OBJECT_0 + 1) {
            return;  // Stop requested or wait failed.
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}

// Returns false if the queue may still be running.
bool ShutdownQueue(winrt::Windows::System::DispatcherQueueController const& queue) {
    using winrt::Windows::Foundation::AsyncStatus;
    try {
        auto operation = queue.ShutdownQueueAsync();
        ULONGLONG deadline = GetTickCount64() + 5000;
        while (operation.Status() == AsyncStatus::Started && GetTickCount64() < deadline) {
            MsgWaitForMultipleObjectsEx(0, nullptr, 50, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if (operation.Status() == AsyncStatus::Started) {
            Wh_Log(L"Dispatcher queue shutdown timed out");
            return false;
        }
        operation.GetResults();
    } catch (...) {
        Wh_Log(L"Dispatcher queue shutdown error: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    return true;
}

DWORD WINAPI WorkerThread(void*) {
    // DWM reports bounds in physical pixels; position in the same space.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    Worker worker;
    std::vector<HWINEVENTHOOK> hooks;
    bool apartment = false;
    bool registered = false;
    HMODULE coreMessaging = nullptr;
    winrt::Windows::System::DispatcherQueueController queue{nullptr};

    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        apartment = true;

        // Composition needs a dispatcher queue on this thread.
        coreMessaging =
            LoadLibraryExW(L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!coreMessaging) {
            winrt::throw_last_error();
        }
        auto createQueue = reinterpret_cast<CreateDispatcherQueueController_t>(
            GetProcAddress(coreMessaging, "CreateDispatcherQueueController"));
        if (!createQueue) {
            winrt::throw_last_error();
        }
        DispatcherQueueOptionsAbi options{sizeof(DispatcherQueueOptionsAbi),
                                          kDispatcherQueueThreadCurrent,
                                          kDispatcherQueueComSta};
        winrt::check_hresult(createQueue(options, winrt::put_abi(queue)));

        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = WindowProc;
        windowClass.hInstance = g_module;
        windowClass.lpszClassName = kClassName;
        if (!RegisterClassExW(&windowClass)) {
            winrt::throw_last_error();
        }
        registered = true;

        g_worker = &worker;
        worker.controller = CreateWindowExW(0, kClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                                            nullptr, g_module, nullptr);
        if (!worker.controller) {
            winrt::throw_last_error();
        }

        // Publish the controller before loading settings, so a change made in
        // the meantime is still delivered afterwards.
        g_controller = worker.controller;
        LoadSettings();

        worker.resources.Create();

        for (auto const& range : kHookRanges) {
            HWINEVENTHOOK hook =
                SetWinEventHook(range.first, range.last, nullptr, OnWinEvent, 0, 0,
                                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            if (hook) {
                hooks.push_back(hook);
            } else {
                Wh_Log(L"SetWinEventHook %04X failed, relying on polling", range.first);
            }
        }

        SetTimer(worker.controller, kPollTimerId, kPollIntervalMs, nullptr);
        SafeRefresh(worker);
        RunMessageLoop();
    } catch (...) {
        Wh_Log(L"Worker error: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }

    for (HWINEVENTHOOK hook : hooks) {
        UnhookWinEvent(hook);
    }
    if (worker.controller) {
        KillTimer(worker.controller, kPollTimerId);
        KillTimer(worker.controller, kSettleTimerId);
    }
    StopClock(worker);

    // Composition objects must go before the dispatcher queue.
    worker.backdrops.clear();
    worker.failed.clear();
    worker.resources.Release();

    g_controller = nullptr;
    if (worker.controller) {
        DestroyWindow(worker.controller);
        worker.controller = nullptr;
    }
    g_worker = nullptr;
    if (registered) {
        UnregisterClassW(kClassName, g_module);
    }

    bool queueStopped = !queue || ShutdownQueue(queue);
    if (!queueStopped) {
        // Leak deliberately: unloading CoreMessaging or tearing down the
        // apartment under a live queue could crash the process.
        winrt::detach_abi(queue);
        return 0;
    }
    queue = nullptr;
    if (coreMessaging) {
        FreeLibrary(coreMessaging);
    }
    if (apartment) {
        winrt::uninit_apartment();
    }
    return 0;
}

// Returns false if the worker didn't exit in time. Its handles are then kept,
// because the thread may still use the stop event.
bool StopWorker() {
    if (g_workerThread) {
        SetEvent(g_stopEvent);
        if (WaitForSingleObject(g_workerThread, kStopTimeoutMs) != WAIT_OBJECT_0) {
            Wh_Log(L"Worker didn't stop within %u ms", kStopTimeoutMs);
            return false;
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    return true;
}

void StartWorker() {
    if (g_workerThread) {
        return;
    }

    if (!g_module) {
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&WorkerThread), &g_module);
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    // The worker loads the settings itself. Disabling the mod in Windhawk
    // ends this process, so there is no separate on/off setting.
    StartWorker();
    return g_workerThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    // Applied on the worker thread: no thread restart, and nothing blocks
    // Windhawk's callback.
    HWND controller = g_controller;
    if (!controller || !PostMessageW(controller, kSettingsMessage, 0, 0)) {
        Wh_Log(L"Worker not available, restarting it");
        if (StopWorker()) {
            StartWorker();
        }
    }
}

void WhTool_ModUninit() {
    // If the worker is stuck, the ExitProcess that follows ends it anyway.
    StopWorker();
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
