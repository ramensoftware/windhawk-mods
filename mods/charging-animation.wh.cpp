// ==WindhawkMod==
// @id              charging-animation
// @name            Charging Animation
// @description     Plays an animation, shows a battery stats card (optionally with sound) when the charger is plugged in or unplugged.
// @version         1.0.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @donateUrl       https://buymeacoffee.com/dpakb
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ladvapi32 -lole32 -loleaut32 -lwinmm -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Charging Animation

Plays an animation, shows a battery stats card (optionally with sound) when the charger is plugged in or unplugged.

The animation and the card are separate, and either one can be turned off. The settings are split into
three groups (BackgroundEffect, Sounds and StatsPanel) plus a single Quiet mode switch at the bottom. Rows
that your choices don't use hide themselves, so the list stays short.

![Charging Animation in action](https://raw.githubusercontent.com/DeepakBalaraman/windows-charging-animation/main/assets/sample.gif)

## Background Effect

`When plugged in` and `When unplugged` decide which events play anything at all. The rest of the section is
shared by both, apart from the two colors and the unplug direction. The sounds have a section of their own.

There are six looks. `Animation` picks the one you want.

**Wave** is the default. It takes a picture of the desktop and bends it outward along an expanding ring,
tinted with the event color. `Origin` moves where it starts, either one of nine spots on the screen under
the pointer or `Cursor Position` to use the pointer itself.

**Linear Wave** is the same refraction with a straight front instead of a ring. A pair of fronts leave
`Origin` together and run along the line it sits on, one each way, so `Top` washes the screen downward,
`Left` sweeps it left to right and a corner sends it across at an angle. With `Center` the two fronts part
in the middle and run out to either side. The band of light rides them and `Motion` shapes the speed in the
same way. `Reverse` on an unplug runs them back in, so the desktop drains toward the origin instead of
being pushed away from it.

**Edge Glow** never touches the desktop. A band of light swells out of the screen borders, and one bright
spot travels around them while it's up, coming in from the border nearest `Origin`. Set the color to
`rainbow` and the band becomes a gradient running around the screen; `Motion` sets how the spot speeds up
and slows down.

**Bezel** presses the screen in. A dark frame grows out of all four edges, its inner lip catching a little
light, and the desktop, captured once when the animation starts, is shrunk to fit inside it, so the whole
picture slides inward instead of the edges being covered. Nothing on the picture is stretched: it shrinks
by the same amount both ways, so the frame comes out a little thinner across the top and bottom. The
picture sinks into the frame's shadow and creeps a little deeper as it settles, then holds and lets go.
Because the shrunk picture is a still image, the effect gives up after about two seconds whatever `Duration`
says. The frame is plain black and grey throughout, with no tint on it. `Strength` sets how far in it comes,
about 16 pixels on the long edge at the default. It presses all four edges by the same amount, so it has no
starting point and no direction of travel: `Origin` and `Unplug direction` are hidden while Bezel is
chosen.

**Circuit Trace** is the quiet one. A circuit board wakes along the bottom of the screen, above the
taskbar: a power rail steps across the width of the monitor at forty-five degrees, a bundle of thin signal
lines runs beside one stretch of it and fans away at both ends, risers drop off the rail into drilled vias,
and elbows turn up out of the bottom corners. A charge runs along each trace in turn, close to white at its
head with a long tail of light behind it, and the via it is heading for rings as it arrives. Between pulses
the copper only glows faintly, the way a board runs on standby power, and then the whole thing fades out.
Nothing is captured from the desktop. `Strength` sets how thick the traces are and how far their glow
spills, and the layout is different on every event. The board runs along the bottom edge rather than around
a point, so it has no `Origin`; `Reverse` on an unplug sends each charge the other way, out of the far end
and back into the rail, so the board drains instead of charging.

**Matrix Code** is the busy one. Columns of characters drop down the screen, each a bright head over a
trail that dims as it falls, and some cells in a trail stay dark, which is what turns a solid bar into a
stream of characters. The head of every column keeps flickering to a new character while it travels, and
the columns are let go from the `Origin` outward, so the cascade spreads across the monitor in one sweep.
The characters are the event color, with the head of each column close to white, and on `rainbow` the
columns take different points of the gradient. Nothing is captured from the desktop. `Strength` sets how
long the trails are. Like the bezel it is a burst rather than a slow move, so it gives up after about two
seconds whatever `Duration` says. `Reverse` on an unplug turns the cascade around: the characters run
upward, and the release starts at the edges and closes in on `Origin` instead of spreading out from it.

**Colors.** `Plugged-in color` and `Unplugged color` each tint their own event. Pick one from the list, or
choose Custom and type into the field that appears underneath. On Windhawk 2.0 that field is a color
picker. These all work:

| Format | Example |
| --- | --- |
| A name | `purple`, `mint`, `gold` |
| Short hex | `#48F` |
| Hex | `#4285F4` |
| RGB | `66,133,244` |
| Windows accent | `accent` |
| Rolling gradient | `rainbow` |

A name that isn't recognized falls back to the event default, blue for plugging in and red for unplugging,
so a typo doesn't break anything.

**Strength, Duration, Motion and Opacity** control the size and the timing. `Strength` (0-40) is how far
the desktop bends, how wide the light band is, how deep the push goes, how thick the traces are, how long
the cascade trails; at 0 the wave keeps only its glow. `Duration` (300-5000 ms) is how long the animation
runs, though the bezel and the cascade stop well before it, `Opacity` (0-100) fades all of it, and `Motion`
picks the easing used by Wave and Edge Glow.

**Dim screen** darkens the monitor the animation plays on, the way a phone dims its display when it starts
charging. It fades in over the first quarter second, stays down for the length of the animation and fades
back out at the end, so nothing snaps. The range is 0-90, so the screen is never fully black. The battery
card sits above the dim and stays bright, only the monitor the animation plays on is dimmed, and with the
animation turned off for an event there is nothing to dim.

**Unplug direction** picks which way the unplug travels. `Forwards` plays it exactly like a plug-in,
expanding from the origin; `Reverse` starts at the screen edges and collapses back to the origin, like the
charge being pulled away. Wave, Edge Glow, Circuit Trace and Matrix Code use it, and Bezel hides it because
it has nowhere to travel.

## Sounds

Both fields take a path to a .wav file, or a system sound alias such as `Notification.IM` or
`WindowsBackground`. Leave a field empty for no sound. If you want the sound on its own, turn off the
animation and the card for that event.

## Stats Panel

`When plugged in` and `When unplugged` choose which events show the card, so you can keep it for plugging
in only, for unplugging only, for both or for neither. The rest of the section applies either way.

**Fluent (Windows 11-ish)** is the default. It's an acrylic card: the desktop behind it is blurred once
when the card appears and covered with the theme color, with a thin border and a soft shadow on top. A
battery icon sits in a tinted tile next to the percentage, there's a status line under the number, and a
level bar that fills from empty as the card slides in and turns red below 20% while on battery.

**Fluent (Minimal)** is the same card at badge size, with half the padding, a smaller number, no status
line and a thinner bar.

**PineApple (Card)** is the plain system HUD: a rounded square, as wide as it is tall, black in dark mode
and white in light mode, with a large battery icon on top, the percentage below it, the status line under that
and a full-width capsule bar along the bottom. The accent color isn't used anywhere in it, and the icon and
the bar turn red when the battery runs low. It fades and pops in quickly instead of sliding; `Center` is the
usual place to put it.

**Galaxy UI** is the wide soft card with corners that take about a third of its height. The percentage is
twice the normal size, with a bolt in a round tinted badge next to it, the status line underneath and one
thin bar running the full width along the bottom edge. On an unplug the badge shows a battery instead of
the bolt. The surface is nearly opaque, so the desktop behind only tints it.

**PineApple (Circle)** is the phone charging screen: a thick ring that fills clockwise to the battery
level, a bolt and the percentage in the middle, and the status line on a small frosted pill underneath.
The middle of the dial is a soft patch of the blurred desktop, so the number sits on a little frost
instead of bare wallpaper, and the patch fades away before it reaches the ring, which keeps the dial
floating instead of turning it into a card. The number and the bolt also carry a soft shadow that keeps
them readable on a bright background. It springs in with a small overshoot, sweeps the ring once, and
shrinks away when it closes.

The Fluent and Galaxy cards follow the Windows app theme; the PineApple card flips between black and
white with it, so it stays readable on either wallpaper. All five update once a second and fade out at the
end.

**Position, accent and the rest.** `Position` is where the card appears: `Follow Effect Origin` keeps it
where the animation started, and the other choices are the corners, the edges, `Center` and the cursor. It
always stays inside the working area, so the taskbar isn't covered. `Accent color` sets the tint, with
`Match Effect Color` taking the color from the event that brought the card up (the gradient included) and
`Custom` reading the field below it in any of the formats above; the PineApple card has no tint to
give it, so it ignores this one. Turning `Details` off drops the status line, and `Duration`, `Opacity` and
`Font size` tune the rest.

## Quiet Mode

While a full-screen app, a game or presentation mode is running, Quiet mode skips the animation, the sound
and the card.

### Credits

Wave Effect inspired by [Magic Pointer Mod](https://windhawk.net/mods/magic-pointer)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BackgroundEffect:
  - WhenPluggedIn: true
    $name: When plugged in
    $description: "Play the animation when the charger is connected."
  - WhenUnplugged: false
    $name: When unplugged
    $description: "Play the animation when the charger is disconnected."
  - Animation: ripple
    $name: Animation
    $description: "Pick an animation to play."
    $options:
      - ripple: Wave (Refraction)
      - linearwave: Linear Wave (Refraction)
      - edgeglow: Edge Glow (Neon Border)
      - bezel: Bezel (Screen Push)
      - circuit: Circuit Trace (PCB Glow)
      - matrix: Matrix Code (Character Rain)
  - PluggedInColor: accent
    $name: Plugged-in color
    $description: "Color of the animation when the charger is connected. Choose Custom to use the color below."
    $options:
      - accent: Windows Accent Color
      - rainbow: Rainbow (Rolling Gradient)
      - custom: Custom (Pick A Color Below)
      - blue: Blue
      - red: Red
      - green: Green
      - yellow: Yellow
      - orange: Orange
      - purple: Purple
      - pink: Pink
      - magenta: Magenta
      - cyan: Cyan
      - teal: Teal
      - mint: Mint
      - gold: Gold
      - white: White
  - PluggedInCustomColor: "#4285F4"
    $name: Plugged-in custom color
    $description: "The color used when the choice above is Custom, e.g. #4285F4, #48F or 66,133,244."
    #! $format: colorRgb
    #! $showIf: {PluggedInColor: custom}
  - UnpluggedColor: accent
    $name: Unplugged color
    $description: "Color of the animation when the charger is disconnected. Choose Custom to use the color below."
    $options:
      - accent: Windows Accent Color
      - rainbow: Rainbow (Rolling Gradient)
      - custom: Custom (Pick A Color Below)
      - blue: Blue
      - red: Red
      - green: Green
      - yellow: Yellow
      - orange: Orange
      - purple: Purple
      - pink: Pink
      - magenta: Magenta
      - cyan: Cyan
      - teal: Teal
      - mint: Mint
      - gold: Gold
      - white: White
  - UnpluggedCustomColor: "#EA4335"
    $name: Unplugged custom color
    $description: "The color used when the choice above is Custom, e.g. #EA4335, #E43 or 234,67,53."
    #! $format: colorRgb
    #! $showIf: {UnpluggedColor: custom}
  - UnplugDirection: reverse
    $name: Unplug direction
    $description: "Which way the unplug animation travels. Forwards plays it exactly like plugging in, moving out from the origin; Reverse starts at the screen edges and collapses back in toward it. Wave, Linear Wave, Edge Glow, Circuit Trace and Matrix Code use this."
    #! $showIf: {Animation: [ripple, linearwave, edgeglow, circuit, matrix]}
    $options:
      - forwards: Forwards (Expand)
      - reverse: Reverse (Collapse)
  - Origin: bottom
    $name: Origin
    $description: "Where the animation starts on the monitor under the cursor. Wave grows from here, Linear Wave sends its two fronts out from here, Edge Glow brings its travelling highlight in along the border from here, and the cascade is released from here outward. Cursor Position follows the pointer itself."
    #! $showIf: {Animation: [ripple, linearwave, edgeglow, matrix]}
    $options:
      - topleft: Top Left
      - top: Top
      - topright: Top Right
      - left: Left
      - center: Center
      - right: Right
      - bottomleft: Bottom Left
      - bottom: Bottom
      - bottomright: Bottom Right
      - cursor: Cursor Position
  - Strength: 16
    $name: Strength
    $description: "How far the effect reaches: wave displacement, glow band width, push depth, trace thickness or how long the cascade trails behind its head. Lower values are gentler."
    #! $min: 0
    #! $max: 40
  - Duration: 1650
    $name: Duration (ms)
    $description: "How long the animation runs. The bezel and the cascade let go early whatever this says."
    #! $min: 300
    #! $max: 5000
  - Motion: smooth
    $name: Motion
    $description: "How the animation speeds up and slows down. Wave, Linear Wave and Edge Glow use this."
    #! $showIf: {Animation: [ripple, linearwave, edgeglow]}
    $options:
      - smooth: Smooth (Default)
      - linear: Linear
      - easein: Ease In (Starts Slow)
      - easeout: Ease Out (Starts Fast)
      - easeinout: Ease In And Out
  - Opacity: 100
    $name: Opacity (%)
    $description: "Overall opacity of the animation, from 0 (invisible) to 100 (fully visible)."
    #! $min: 0
    #! $max: 100
  - Dim: 0
    $name: Dim screen (%)
    $description: "Darken the screen while the animation plays, fading in and out with it, the way a phone dims its display when it starts charging. 0 leaves the screen untouched."
    #! $min: 0
    #! $max: 90
- Sounds:
  - WhenPluggedIn: ""
    $name: When plugged in
    $description: "Absolute path to a .wav file, or a system sound alias such as Notification.IM. Leave empty for silence."
  - WhenUnplugged: ""
    $name: When unplugged
    $description: "Absolute path to a .wav file, or a system sound alias such as WindowsBackground. Leave empty for silence."
- StatsPanel:
  - WhenPluggedIn: true
    $name: When plugged in
    $description: "Show the panel with the battery level and charging status when the charger is connected."
  - WhenUnplugged: true
    $name: When unplugged
    $description: "Show the panel with the battery level and charging status when the charger is disconnected."
  - Style: fluent
    $name: Style
    $description: "Fluent is the Windows 11 acrylic card, Fluent Minimal the badge-sized version, PineApple (Card) a plain black or white HUD slab, Galaxy UI a wide soft card with a bolt badge beside the percentage, and PineApple (Circle) the phone charging ring with a caption pill."
    $options:
      - fluent: Fluent (Windows 11-ish)
      - minimal: Fluent (Minimal)
      - osd: PineApple (Card)
      - galaxy: Galaxy UI
      - pineapple: PineApple (Circle)
  - Position: ripple
    $name: Position
    $description: "Where the panel appears. Follow Effect puts it where the animation started. It stays inside the working area, so the taskbar is never covered."
    $options:
      - ripple: Follow Effect Origin
      - topleft: Top Left
      - top: Top
      - topright: Top Right
      - left: Left
      - center: Center
      - right: Right
      - bottomleft: Bottom Left
      - bottom: Bottom
      - bottomright: Bottom Right
      - cursor: Cursor Position
  - AccentColor: ripple
    $name: Accent color
    $description: "Accent color of the panel. Match Effect uses the color of the event that brought it up. Choose Custom to use the color below."
    $options:
      - ripple: Match Effect Color
      - custom: Custom (Pick A Color Below)
      - blue: Blue
      - red: Red
      - green: Green
      - orange: Orange
      - purple: Purple
      - pink: Pink
      - cyan: Cyan
      - white: White
      - accent: Windows Accent Color
  - CustomAccentColor: "#4285F4"
    $name: Custom accent color
    $description: "The color used when the choice above is Custom, e.g. #4285F4, #48F or 66,133,244."
    #! $format: colorRgb
    #! $showIf: {AccentColor: custom}
  - Details: true
    $name: Details
    $description: "Add a second line with the charging state, the time to full or remaining, and the charging rate."
  - Duration: 4000
    $name: Duration (ms)
    $description: "How long the panel stays on screen. It refreshes once a second while visible."
    #! $min: 1000
    #! $max: 15000
  - Opacity: 100
    $name: Opacity (%)
    $description: "Opacity of the panel, from 0 (invisible) to 100."
    #! $min: 0
    #! $max: 100
  - FontSize: 20
    $name: Font size
    $description: "Font size of the panel in pixels at 100% display scaling."
    #! $min: 10
    #! $max: 48
- QuietMode: true
  $name: Quiet mode
  $description: "Skip the animation, the sound and the panel while a full-screen app, a game or presentation mode is running."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <powrprof.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <windhawk_utils.h>

// Explicitly define GUID_ACDC_POWER_SOURCE to prevent lld linker errors across MinGW/Clang toolchains
#ifdef __cplusplus
extern "C" {
#endif

const GUID DECLSPEC_SELECTANY GUID_ACDC_POWER_SOURCE = { 0x5d3e9a59, 0xe9d5, 0x4b00, { 0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48 } };
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Constants & Globals

constexpr UINT WM_APP_RIPPLE = WM_APP + 10;
constexpr UINT WM_APP_STATS = WM_APP + 11;
constexpr UINT WM_APP_DISPLAY = WM_APP + 12;
constexpr float kPi = 3.14159265358979f;
constexpr double kFrameMs = 1000.0 / 60.0;
// The matrix cascade repaints the whole monitor, so it takes a slightly slower
// clock than the effects that only touch a band of it.
constexpr double kMatrixFrameMs = 1000.0 / 50.0;

// The cascade draws printable ASCII, the one range every font on the machine
// is certain to have, so no cell can come out as a missing-glyph box.
constexpr wchar_t kMatrixFirstChar = L'!';
constexpr wchar_t kMatrixLastChar = L'~';

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

// winuser.h only declares this for Windows 7 and later.
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

struct Settings {
    std::wstring pluggedInSoundPath;
    std::wstring unpluggedSoundPath;
    bool suppressWhenBusy = true;
    // Per-event ripple settings: enabled flag, preset key and custom color text.
    bool pluggedInRipple = true;
    std::wstring pluggedInRippleColor;
    std::wstring pluggedInCustomColor;
    bool unpluggedRipple = true;
    std::wstring unpluggedRippleColor;
    std::wstring unpluggedRippleDirection;
    std::wstring unpluggedCustomColor;
    // Animation shape, shared by both events.
    std::wstring rippleOrigin;
    int rippleStrength = 16;
    int rippleOpacity = 100;
    int rippleDuration = 1650;
    std::wstring rippleCurve;
    // Which scene effect plays: the wave, the border glow, the bezel or the board.
    std::wstring rippleStyle;
    // How far the screen is darkened behind the scene effect, 0 to 90 percent.
    int dim = 0;
    // Charging stats overlay, shown per power event.
    bool statsPluggedIn = true;
    bool statsUnplugged = true;
    std::wstring statsTheme;
    std::wstring statsPosition;
    std::wstring statsColor;
    std::wstring statsCustomColor;
    bool statsDetails = true;
    int statsDuration = 4000;
    int statsOpacity = 100;
    int statsFontSize = 20;
    std::mutex mtx;
} g_settings;

HANDLE g_hThread = NULL;
DWORD g_threadId = 0;
HANDLE g_readyEvent = NULL;
HWND g_messageWindow = nullptr;
int g_lastPowerCondition = -1; // -1 = unknown baseline

HINSTANCE g_instance = nullptr;
std::atomic<bool> g_stopping{false};

////////////////////////////////////////////////////////////////////////////////
// Colors & Timing

struct Color {
    float r, g, b, a;
};

constexpr Color Rgb(int r, int g, int b, float a = 1.0f) {
    return {r / 255.0f, g / 255.0f, b / 255.0f, a};
}

inline Color Mix(const Color& x, const Color& y, float t) {
    return {x.r + (y.r - x.r) * t, x.g + (y.g - x.g) * t,
            x.b + (y.b - x.b) * t, x.a + (y.a - x.a) * t};
}

constexpr Color kGlowBlue = Rgb(66, 133, 244);
constexpr Color kGlowRed = Rgb(234, 67, 53);
constexpr Color kGlowYellow = Rgb(251, 188, 4);
constexpr Color kGlowGreen = Rgb(52, 168, 83);

Color RainbowGradient(float t) {
    static const Color kStops[] = {kGlowBlue, kGlowRed, kGlowYellow, kGlowGreen};
    t -= floorf(t);
    const float x = t * 4.0f;
    const int i = static_cast<int>(x);
    float f = x - i;
    f = f * f * (3.0f - 2.0f * f);
    return Mix(kStops[i & 3], kStops[(i + 1) & 3], f);
}

inline float Clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

// A single color, or a rolling rainbow gradient when "rainbow" is requested.
struct GlowPalette {
    bool rainbow = false;
    Color color = kGlowBlue;

    Color At(float t) const { return rainbow ? RainbowGradient(t) : color; }
};

GlowPalette MakeGlowPalette(Color base) {
    GlowPalette palette;
    palette.color = base;
    return palette;
}

GlowPalette MakeRainbowPalette() {
    GlowPalette palette;
    palette.rainbow = true;
    return palette;
}

std::wstring Trim(const std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin])) {
        begin++;
    }
    while (end > begin && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(begin, end - begin);
}

GlowPalette ParseGlowColor(std::wstring text, Color fallback = kGlowBlue) {
    for (wchar_t& c : text) {
        c = towlower(c);
    }
    text = Trim(text);
    if (text == L"rainbow") {
        GlowPalette palette;
        palette.rainbow = true;
        return palette;
    }
    if (text.empty()) {
        return MakeGlowPalette(fallback);
    }
    if (text == L"accent" || text == L"system" || text == L"windows") {
        DWORD value = 0;
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                         L"AccentColor", RRF_RT_REG_DWORD, nullptr, &value,
                         &size) == ERROR_SUCCESS) {
            return MakeGlowPalette(Rgb(value & 0xFF, (value >> 8) & 0xFF,
                                       (value >> 16) & 0xFF));
        }
        return MakeGlowPalette(fallback);
    }
    static const struct {
        PCWSTR name;
        Color color;
    } kNames[] = {
        {L"blue", Rgb(66, 133, 244)},    {L"red", Rgb(234, 67, 53)},
        {L"green", Rgb(52, 168, 83)},    {L"yellow", Rgb(251, 188, 4)},
        {L"orange", Rgb(255, 128, 32)},  {L"purple", Rgb(150, 92, 255)},
        {L"violet", Rgb(150, 92, 255)},  {L"pink", Rgb(255, 92, 170)},
        {L"magenta", Rgb(232, 64, 220)}, {L"cyan", Rgb(0, 200, 230)},
        {L"teal", Rgb(0, 168, 150)},     {L"white", Rgb(240, 244, 255)},
        {L"gold", Rgb(240, 180, 40)},    {L"mint", Rgb(64, 220, 160)},
    };
    for (const auto& entry : kNames) {
        if (text == entry.name) {
            return MakeGlowPalette(entry.color);
        }
    }

    std::wstring hex = text;
    if (!hex.empty() && hex[0] == L'#') {
        hex.erase(0, 1);
    }
    const bool isHex =
        (hex.size() == 3 || hex.size() == 6) &&
        hex.find_first_not_of(L"0123456789abcdef") == std::wstring::npos;
    if (isHex) {
        const unsigned long v = wcstoul(hex.c_str(), nullptr, 16);
        if (hex.size() == 3) {
            return MakeGlowPalette(Rgb(((v >> 8) & 0xF) * 17, ((v >> 4) & 0xF) * 17,
                                       (v & 0xF) * 17));
        }
        return MakeGlowPalette(Rgb((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF));
    }

    int values[3];
    int count = 0;
    for (size_t i = 0; i < text.size() && count < 3;) {
        if (iswdigit(text[i])) {
            values[count++] = std::min(255, _wtoi(text.c_str() + i));
            while (i < text.size() && iswdigit(text[i])) {
                i++;
            }
        } else {
            i++;
        }
    }
    if (count == 3) {
        return MakeGlowPalette(Rgb(values[0], values[1], values[2]));
    }
    return MakeGlowPalette(fallback);
}

// Turns a preset key into a palette. "custom" reads the free-form hex field
// next to it, so users can pick a color from the list or type their own.
GlowPalette ResolveRippleColor(const std::wstring& preset,
                               const std::wstring& custom,
                               Color fallback) {
    std::wstring key = Trim(preset);
    for (wchar_t& c : key) {
        c = towlower(c);
    }
    if (key == L"custom") {
        const std::wstring hex = Trim(custom);
        return hex.empty() ? MakeGlowPalette(fallback)
                           : ParseGlowColor(hex, fallback);
    }
    if (key.empty()) {
        return MakeGlowPalette(fallback);
    }
    return ParseGlowColor(key, fallback);
}

// Set per power event: the configured color resolved against the event default.
GlowPalette g_glow = MakeGlowPalette(kGlowBlue);

double NowMs() {
    static const double msPerTick = [] {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return 1000.0 / static_cast<double>(frequency.QuadPart);
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) * msPerTick;
}

////////////////////////////////////////////////////////////////////////////////
// Surface & Rasterization (Adapted from Magic Pointer)

class Surface {
public:
    Surface() = default;
    ~Surface() { Reset(); }
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

    bool Resize(int width, int height) {
        width = std::max(width, 1);
        height = std::max(height, 1);
        if (!bits_ || width > stride_ || height > capacityHeight_) {
            Reset();
            int capW = (width + 63) & ~63;
            int capH = (height + 63) & ~63;

            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biWidth = capW;
            info.bmiHeader.biHeight = -capH;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            dc_ = CreateCompatibleDC(nullptr);
            if (!dc_) return false;
            void* bits = nullptr;
            bitmap_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
            if (!bitmap_ || !bits) {
                Reset();
                return false;
            }
            oldBitmap_ = SelectObject(dc_, bitmap_);
            bits_ = static_cast<uint32_t*>(bits);
            stride_ = capW;
            capacityHeight_ = capH;
        }
        width_ = width;
        height_ = height;
        Clear();
        return true;
    }

    void Reset() {
        if (dc_) {
            if (oldBitmap_) SelectObject(dc_, oldBitmap_);
            DeleteDC(dc_);
        }
        if (bitmap_) DeleteObject(bitmap_);
        dc_ = nullptr;
        bitmap_ = nullptr;
        oldBitmap_ = nullptr;
        bits_ = nullptr;
        width_ = height_ = stride_ = capacityHeight_ = 0;
    }

    void Clear() {
        for (int y = 0; y < height_; y++) {
            memset(Row(y), 0, static_cast<size_t>(width_) * 4);
        }
    }

    void Swap(Surface& other) {
        std::swap(dc_, other.dc_);
        std::swap(bitmap_, other.bitmap_);
        std::swap(oldBitmap_, other.oldBitmap_);
        std::swap(bits_, other.bits_);
        std::swap(width_, other.width_);
        std::swap(height_, other.height_);
        std::swap(stride_, other.stride_);
        std::swap(capacityHeight_, other.capacityHeight_);
    }

    bool Valid() const { return bits_ != nullptr; }
    int Width() const { return width_; }
    int Height() const { return height_; }
    int Stride() const { return stride_; }
    HDC Dc() const { return dc_; }
    uint32_t* Row(int y) { return bits_ + static_cast<size_t>(y) * stride_; }
    const uint32_t* Row(int y) const {
        return bits_ + static_cast<size_t>(y) * stride_;
    }

    // Restores a saved frame, which is how animating parts of a static surface
    // (the charging ring) avoids re-rasterising the text every frame.
    bool CopyFrom(const Surface& other) {
        if (!other.Valid() || !Resize(other.Width(), other.Height())) {
            return false;
        }
        for (int y = 0; y < other.Height(); y++) {
            memcpy(Row(y), other.Row(y), static_cast<size_t>(other.Width()) * 4);
        }
        return true;
    }

private:
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ oldBitmap_ = nullptr;
    uint32_t* bits_ = nullptr;
    int width_ = 0, height_ = 0, stride_ = 0, capacityHeight_ = 0;
};

inline uint32_t PackPremultiplied(const Color& c, float coverage) {
    const float a = Clamp01(c.a * coverage);
    if (a <= 0.0f) {
        return 0;
    }
    const uint32_t alpha = static_cast<uint32_t>(a * 255.0f + 0.5f);
    auto channel = [&](float v) {
        return std::min(alpha,
                        static_cast<uint32_t>(Clamp01(v) * a * 255.0f + 0.5f));
    };
    return (alpha << 24) | (channel(c.r) << 16) | (channel(c.g) << 8) |
           channel(c.b);
}

inline void BlendOver(uint32_t& dst, uint32_t src) {
    uint32_t sa = src >> 24;
    if (sa == 0) return;
    if (sa == 255) {
        dst = src;
        return;
    }
    uint32_t inv = 255 - sa;
    uint32_t d = dst;
    uint32_t rb = (d & 0x00FF00FF) * inv + 0x00800080;
    rb = ((rb + ((rb >> 8) & 0x00FF00FF)) >> 8) & 0x00FF00FF;
    uint32_t ag = ((d >> 8) & 0x00FF00FF) * inv + 0x00800080;
    ag = (ag + ((ag >> 8) & 0x00FF00FF)) & 0xFF00FF00;
    dst = src + (rb | ag);
}

inline uint32_t ScalePremultiplied(uint32_t pixel, uint32_t alpha) {
    uint32_t rb = (pixel & 0x00FF00FF) * alpha + 0x00800080;
    rb = ((rb + ((rb >> 8) & 0x00FF00FF)) >> 8) & 0x00FF00FF;
    uint32_t ag = ((pixel >> 8) & 0x00FF00FF) * alpha + 0x00800080;
    ag = (ag + ((ag >> 8) & 0x00FF00FF)) & 0xFF00FF00;
    return rb | ag;
}

struct OpaquePixels {
    const uint32_t* pixels;
    int width;
    int height;
    int stride;
};

inline uint32_t SampleOpaquePixels(const OpaquePixels& image, float x, float y) {
    x = std::clamp(x, 0.0f, static_cast<float>(image.width - 1));
    y = std::clamp(y, 0.0f, static_cast<float>(image.height - 1));
    const int x0 = static_cast<int>(x), y0 = static_cast<int>(y);
    const int x1 = std::min(x0 + 1, image.width - 1);
    const int y1 = std::min(y0 + 1, image.height - 1);
    const uint32_t fx = static_cast<uint32_t>((x - x0) * 256.0f + 0.5f);
    const uint32_t fy = static_cast<uint32_t>((y - y0) * 256.0f + 0.5f);
    auto lerp = [](uint32_t a, uint32_t b, uint32_t weight) {
        const uint32_t rb = (((a & 0x00FF00FF) * (256 - weight) +
                              (b & 0x00FF00FF) * weight) >> 8) & 0x00FF00FF;
        const uint32_t g = (((a & 0x0000FF00) * (256 - weight) +
                             (b & 0x0000FF00) * weight) >> 8) & 0x0000FF00;
        return 0xFF000000 | rb | g;
    };
    const uint32_t* row0 = image.pixels + static_cast<size_t>(y0) * image.stride;
    const uint32_t* row1 = image.pixels + static_cast<size_t>(y1) * image.stride;
    return lerp(lerp(row0[x0], row0[x1], fx), lerp(row1[x0], row1[x1], fx), fy);
}

////////////////////////////////////////////////////////////////////////////////
// Layered Window Management (Adapted from Magic Pointer)

constexpr WCHAR kRippleWindowClass[] = L"ChargingAnimationRippleWindow";

template <typename T>
T GetProc(PCWSTR module, const char* name) {
    HMODULE handle = GetModuleHandleW(module);
    if (!handle) {
        handle = LoadLibraryExW(module, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!handle) {
        return nullptr;
    }
    return reinterpret_cast<T>(
        reinterpret_cast<void*>(GetProcAddress(handle, name)));
}

bool IsWindows11() {
    static const bool windows11 = [] {
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        auto rtlGetVersion = GetProc<RtlGetVersion_t>(L"ntdll.dll", "RtlGetVersion");
        OSVERSIONINFOW info{};
        info.dwOSVersionInfoSize = sizeof(info);
        return rtlGetVersion && rtlGetVersion(&info) == 0 &&
               (info.dwMajorVersion > 10 ||
                (info.dwMajorVersion == 10 && info.dwBuildNumber >= 22000));
    }();
    return windows11;
}

void SetDwmAttribute(HWND hwnd, DWORD attribute, const void* value, DWORD size) {
    using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    static auto setAttribute = GetProc<DwmSetWindowAttribute_t>(
        L"dwmapi.dll", "DwmSetWindowAttribute");
    if (setAttribute) {
        setAttribute(hwnd, attribute, value, size);
    }
}

void ConfigurePixelWindow(HWND hwnd) {
    const DWORD nonClient = 1;  // DWMNCRP_DISABLED.
    SetDwmAttribute(hwnd, 2 /* DWMWA_NCRENDERING_POLICY */, &nonClient, sizeof(nonClient));
    const BOOL noTransitions = TRUE;
    SetDwmAttribute(hwnd, 3 /* DWMWA_TRANSITIONS_FORCEDISABLED */, &noTransitions, sizeof(noTransitions));
    if (IsWindows11()) {
        const COLORREF border = 0xFFFFFFFE;  // DWMWA_COLOR_NONE.
        SetDwmAttribute(hwnd, 34 /* DWMWA_BORDER_COLOR */, &border, sizeof(border));
    }
}

LRESULT CALLBACK RippleWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    // The overlays are sized to a monitor rectangle taken when they start, so a
    // display that changes under one would leave it covering the wrong place.
    // Closing the effect is cheaper than reworking it mid-flight.
    if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED) {
        if (g_messageWindow) {
            PostMessageW(g_messageWindow, WM_APP_DISPLAY, 0, 0);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

class LayeredWindow {
public:
    bool Create() {
        hwnd_ = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                WS_EX_TRANSPARENT,
            kRippleWindowClass, L"Charging Ripple", WS_POPUP,
            0, 0, 1, 1, nullptr, nullptr, g_instance, nullptr);
        if (hwnd_) {
            ConfigurePixelWindow(hwnd_);
        }
        return hwnd_ != nullptr;
    }

    void Destroy() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        visible_ = false;
    }

    bool Present(Surface& surface, int x, int y, BYTE alpha, const RECT* dirty = nullptr) {
        if (!hwnd_ || !surface.Valid()) return false;
        POINT dst{x, y};
        SIZE size{surface.Width(), surface.Height()};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        GdiFlush();  // Finish any pending DIB writes before uploading the pixels.
        const bool partial = dirty && x == x_ && y == y_ && size.cx == width_ &&
                             size.cy == height_ && alpha == alpha_;
        UPDATELAYEREDWINDOWINFO info{sizeof(info), nullptr, &dst, &size, surface.Dc(),
                                     &src, 0, &blend, ULW_ALPHA, dirty};
        if (!(partial ? UpdateLayeredWindowIndirect(hwnd_, &info)
                      : UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, surface.Dc(),
                                            &src, 0, &blend, ULW_ALPHA))) {
            return false;
        }
        x_ = x;
        y_ = y;
        width_ = size.cx;
        height_ = size.cy;
        alpha_ = alpha;
        return true;
    }

    void Show() {
        if (hwnd_) {
            // Always raises: a window that is still up from the last event has
            // to come back above the dim, which is placed as it is shown.
            SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
            visible_ = true;
        }
    }

    void Hide() {
        if (hwnd_ && visible_) {
            ShowWindow(hwnd_, SW_HIDE);
            visible_ = false;
        }
    }

    HWND Hwnd() const { return hwnd_; }

private:
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
    BYTE alpha_ = 255;
};

constexpr WCHAR kScrimWindowClass[] = L"ChargingAnimationDimWindow";

LRESULT CALLBACK ScrimWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_ERASEBKGND) {
        RECT client;
        if (GetClientRect(hwnd, &client)) {
            FillRect(reinterpret_cast<HDC>(wParam), &client,
                     static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        }
        return TRUE;
    }
    if (message == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// The scrim that darkens the screen behind an effect: a plain layered window
// painted flat black, whose strength is a window attribute rather than pixels.
// Changing it costs a recomposite instead of a monitor-sized bitmap upload,
// which is what lets the dim follow the animation frame by frame.
class ScrimWindow {
public:
    // Sizes the scrim to a monitor, creating the window on first use.
    bool Cover(const RECT& monitor) {
        const int w = monitor.right - monitor.left;
        const int h = monitor.bottom - monitor.top;
        if (w <= 0 || h <= 0) {
            return false;
        }
        if (!hwnd_) {
            hwnd_ = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                    WS_EX_TRANSPARENT,
                kScrimWindowClass, L"Charging Dim", WS_POPUP, 0, 0, 1, 1, nullptr,
                nullptr, g_instance, nullptr);
            if (!hwnd_) {
                return false;
            }
            ConfigurePixelWindow(hwnd_);
        }
        if (w != width_ || h != height_ || monitor.left != left_ || monitor.top != top_) {
            SetWindowPos(hwnd_, HWND_TOPMOST, monitor.left, monitor.top, w, h,
                         SWP_NOACTIVATE | SWP_NOOWNERZORDER);
            left_ = monitor.left;
            top_ = monitor.top;
            width_ = w;
            height_ = h;
            // Paint it black here rather than waiting for the first paint: the
            // window is shown straight after this and must never flash its bare
            // DC across the screen.
            HDC dc = GetDC(hwnd_);
            if (dc) {
                RECT client{0, 0, w, h};
                FillRect(dc, &client,
                         static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
                ReleaseDC(hwnd_, dc);
            }
        }
        return true;
    }

    // Sets how dark the scrim is. This is the whole per-frame cost of the dim.
    void SetStrength(BYTE alpha) {
        if (hwnd_ && alpha != alpha_) {
            SetLayeredWindowAttributes(hwnd_, 0, alpha, LWA_ALPHA);
            alpha_ = alpha;
        }
    }

    // Shows the scrim and holds it directly under `above`, so the effect itself
    // stays at full brightness over the darkened desktop.
    void ShowUnder(HWND above) {
        if (!hwnd_) {
            return;
        }
        SetWindowPos(hwnd_, above ? above : HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        visible_ = true;
    }

    void Hide() {
        if (hwnd_ && visible_) {
            ShowWindow(hwnd_, SW_HIDE);
            visible_ = false;
        }
    }

    void Destroy() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        visible_ = false;
        alpha_ = 0;
        left_ = top_ = width_ = height_ = 0;
    }

    bool Visible() const { return visible_; }

private:
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    BYTE alpha_ = 0;
    int left_ = 0;
    int top_ = 0;
    int width_ = 0;
    int height_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Audio Management

bool ShouldSuppressEffects() {
    {
        std::lock_guard<std::mutex> lock(g_settings.mtx);
        if (!g_settings.suppressWhenBusy) {
            return false;
        }
    }
    QUERY_USER_NOTIFICATION_STATE quns;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) &&
        quns != QUNS_ACCEPTS_NOTIFICATIONS && quns != QUNS_NOT_PRESENT) {
        Wh_Log(L"Notifications suppressed (state %d), skipping charging effects.", quns);
        return true;
    }
    return false;
}

void PlayChargingSound(bool pluggedIn) {
    std::wstring path;
    {
        std::lock_guard<std::mutex> lock(g_settings.mtx);
        path = pluggedIn ? g_settings.pluggedInSoundPath : g_settings.unpluggedSoundPath;
    }

    if (path.empty()) return;

    Wh_Log(L"Playing charging sound: %s", path.c_str());
    bool isFilePath = path.find(L"\\") != std::wstring::npos || path.find(L":") != std::wstring::npos;
    DWORD flags = SND_ASYNC | SND_NODEFAULT;
    if (isFilePath) {
        PlaySoundW(path.c_str(), nullptr, flags | SND_FILENAME);
    } else {
        PlaySoundW(path.c_str(), nullptr, flags | SND_ALIAS | SND_SYSTEM);
    }
}

////////////////////////////////////////////////////////////////////////////////
// The Ripple (Adapted from Magic Pointer)

struct Vec2 {
    float x, y;
};

RECT MonitorRectAt(POINT pt) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &info)) {
        return info.rcMonitor;
    }
    return {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
}

// The part of the monitor a card may cover. A taskbar that stays on screen takes
// its slice out of the work area, while an auto-hidden one is out of the way, so
// the full screen is fair game there.
RECT UsableRect(POINT pt) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &info)) {
        return MonitorRectAt(pt);
    }
    APPBARDATA bar{};
    bar.cbSize = sizeof(bar);
    if (SHAppBarMessage(ABM_GETSTATE, &bar) & ABS_AUTOHIDE) {
        return info.rcMonitor;
    }
    return info.rcWork;
}

float DpiScaleAt(POINT pt) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto getDpiForMonitor =
        GetProc<GetDpiForMonitor_t>(L"shcore.dll", "GetDpiForMonitor");
    if (getDpiForMonitor) {
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (SUCCEEDED(getDpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), 0,
                                       &dpiX, &dpiY))) {
            return std::max(1.0f, static_cast<float>(dpiX) / 96.0f);
        }
    }
    return 1.0f;
}

float MonitorFrameMs(const RECT& monitor) {
    const POINT center{(monitor.left + monitor.right) / 2, (monitor.top + monitor.bottom) / 2};
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    DEVMODEW mode{};
    mode.dmSize = sizeof(mode);
    if (GetMonitorInfoW(MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST), &info) &&
        EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
        mode.dmDisplayFrequency > 1) {
        return 1000.0f / static_cast<float>(std::clamp<DWORD>(mode.dmDisplayFrequency, 60, 144));
    }
    return 1000.0f / 60.0f;
}

// A Gaussian ring: displaces the captured screen along the wave and lights it.
struct RippleWave {
    float displacement;
    float opacity;
    float light;
};

RippleWave EvaluateRippleWave(float distance, float sigma, float strength) {
    const float u = distance / sigma;
    const float envelope = expf(-0.5f * u * u);
    return {-strength * u * envelope, Clamp01(envelope * 4.0f), 0.055f * envelope};
}

// Dimming a captured desktop is a colour scale with the alpha left alone: if the
// alpha came down with it, the window would blend the darkened picture with the
// real desktop behind and it would come back out just as bright.
inline uint32_t DimPixel(uint32_t pixel, uint32_t keep) {
    const uint32_t rb = ((pixel & 0x00FF00FFu) * keep >> 8) & 0x00FF00FFu;
    const uint32_t g = ((pixel & 0x0000FF00u) * keep >> 8) & 0x0000FF00u;
    return (pixel & 0xFF000000u) | rb | g;
}

// 255 keeps a pixel as it is, 0 takes it to black.
inline uint32_t DimKeep(float amount) {
    return static_cast<uint32_t>(Clamp01(1.0f - amount) * 255.0f + 0.5f);
}

// Both surfaces are sized by the animation thread before the worker starts,
// never by the worker itself: a memory DC belongs to the thread that made it,
// and the worker is gone long before the wave has finished reading the picture.
struct RippleCapture {
    Surface snapshot;
    Surface canvas;
    RECT monitor{};
    int centerY = 0;
    HWND notify = nullptr;
    std::atomic<float> frameMs{1000.0f / 60.0f};
    std::atomic<bool> canceled{false};
    std::atomic<int> top{0};
    std::atomic<int> bottom{0};
};

struct RippleCaptureMessage {
    std::shared_ptr<RippleCapture> capture;
    bool first = false;
    bool complete = false;
    bool failed = false;
};

template <typename T>
void PostOwned(HWND hwnd, UINT message, std::unique_ptr<T> object) {
    T* raw = object.release();
    if (!hwnd || !PostMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(raw))) {
        delete raw;
    }
}

// Capture in stripes, starting at the ripple center so the wave can start early.
std::vector<std::pair<int, int>> PlanRippleStripes(int width, int height, int centerY) {
    std::vector<std::pair<int, int>> stripes;
    const int rows = std::max(64, 2200000 / std::max(width, 1));
    if (height <= rows) {
        stripes.push_back({0, height});
        return stripes;
    }
    int top = std::clamp(centerY - rows / 2, 0, height - rows);
    int bottom = top + rows;
    stripes.push_back({top, bottom});
    while (top > 0 || bottom < height) {
        if (top > 0) {
            const int count = std::min(rows, top);
            stripes.push_back({top - count, top});
            top -= count;
        }
        if (bottom < height) {
            const int count = std::min(rows, height - bottom);
            stripes.push_back({bottom, bottom + count});
            bottom += count;
        }
    }
    return stripes;
}

DWORD WINAPI RippleCaptureThreadProc(LPVOID param) {
    std::unique_ptr<std::shared_ptr<RippleCapture>> holder(
        static_cast<std::shared_ptr<RippleCapture>*>(param));
    const std::shared_ptr<RippleCapture> capture = *holder;
    const int width = capture->monitor.right - capture->monitor.left;
    const int height = capture->monitor.bottom - capture->monitor.top;
    auto stopped = [&] { return capture->canceled.load() || g_stopping.load(); };
    auto post = [&](bool first, bool complete, bool failed) {
        auto message = std::make_unique<RippleCaptureMessage>();
        message->capture = capture;
        message->first = first;
        message->complete = complete;
        message->failed = failed;
        PostOwned(capture->notify, WM_APP_RIPPLE, std::move(message));
    };

    capture->frameMs = MonitorFrameMs(capture->monitor);
    bool ok = capture->snapshot.Valid() && capture->canvas.Valid();
    HDC screen = ok ? GetDC(nullptr) : nullptr;
    ok = ok && screen;
    const auto stripes = PlanRippleStripes(width, height, capture->centerY);
    int top = 0;
    int bottom = 0;
    for (size_t i = 0; ok && i < stripes.size() && !stopped(); i++) {
        const int begin = stripes[i].first;
        const int end = stripes[i].second;
        if (!BitBlt(capture->snapshot.Dc(), 0, begin, width, end - begin, screen,
                    capture->monitor.left, capture->monitor.top + begin,
                    SRCCOPY | CAPTUREBLT)) {
            ok = false;
            break;
        }
        GdiFlush();
        if (i == 0) {
            top = begin;
            bottom = end;
        } else if (end == top) {
            top = begin;
        } else {
            bottom = end;
        }
        capture->top.store(top, std::memory_order_release);
        capture->bottom.store(bottom, std::memory_order_release);
        const bool last = i + 1 == stripes.size();
        if (i == 0 || last) {
            post(i == 0, last, false);
        }
    }
    if (screen) {
        ReleaseDC(nullptr, screen);
    }
    if (!ok && !stopped()) {
        post(false, true, true);
    }
    return 0;
}

inline float FastAtan2(float y, float x) {
    const float ax = fabsf(x);
    const float ay = fabsf(y);
    const float mx = std::max(ax, ay);
    const float mn = std::min(ax, ay);
    const float a = mx > 0 ? mn / mx : 0.0f;
    const float s = a * a;
    float r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;
    if (ay > ax) {
        r = 1.57079637f - r;
    }
    if (x < 0) {
        r = 3.14159274f - r;
    }
    return y < 0 ? -r : r;
}

////////////////////////////////////////////////////////////////////////////////
// Rasterization workers

class RasterWorkers {
public:
    ~RasterWorkers() { Reset(); }
    void Reset() {
        if (work_) {
            WaitForThreadpoolWorkCallbacks(work_, TRUE);
            CloseThreadpoolWork(work_);
            work_ = nullptr;
        }
        if (pool_) {
            DestroyThreadpoolEnvironment(&environment_);
            CloseThreadpool(pool_);
            pool_ = nullptr;
        }
        renderRow_ = {};
        attempted_ = false;
    }

    void ForRows(int first, int last, const std::function<void(int)>& renderRow, bool parallel,
                 int batchSize = 16) {
        if (parallel && !attempted_) {
            attempted_ = true;
            SYSTEM_INFO system;
            GetSystemInfo(&system);
            workerCount_ = std::min<DWORD>(2, system.dwNumberOfProcessors > 1
                                           ? system.dwNumberOfProcessors - 1 : 0u);
            if (workerCount_) {
                pool_ = CreateThreadpool(nullptr);
                if (pool_) {
                    InitializeThreadpoolEnvironment(&environment_);
                    SetThreadpoolCallbackPool(&environment_, pool_);
                    SetThreadpoolThreadMaximum(pool_, workerCount_);
                    if (SetThreadpoolThreadMinimum(pool_, workerCount_)) {
                        work_ = CreateThreadpoolWork(Callback, this, &environment_);
                    }
                }
            }
        }
        if (!parallel || !work_) {
            for (int y = first; y < last; y++) renderRow(y);
            return;
        }
        renderRow_ = renderRow;
        nextRow_ = first;
        lastRow_ = last;
        batchSize_ = std::max(1, batchSize);
        for (unsigned i = 0; i < workerCount_; i++) SubmitThreadpoolWork(work_);
        Run();
        WaitForThreadpoolWorkCallbacks(work_, FALSE);
        renderRow_ = {};
    }

private:
    static void CALLBACK Callback(PTP_CALLBACK_INSTANCE, void* context, PTP_WORK) {
        static_cast<RasterWorkers*>(context)->Run();
    }
    void Run() {
        for (;;) {
            const int first = nextRow_.fetch_add(batchSize_, std::memory_order_relaxed);
            if (first >= lastRow_) return;
            for (int y = first; y < std::min(first + batchSize_, lastRow_); y++) renderRow_(y);
        }
    }
    PTP_POOL pool_ = nullptr;
    PTP_WORK work_ = nullptr;
    TP_CALLBACK_ENVIRON environment_{};
    std::function<void(int)> renderRow_;
    std::atomic<int> nextRow_{0};
    int lastRow_ = 0;
    int batchSize_ = 16;
    unsigned workerCount_ = 0;
    bool attempted_ = false;
};

////////////////////////////////////////////////////////////////////////////////
// Animation Shape Settings

enum class RippleCurve {
    Smooth,
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
};

// Maps linear progress (0..1) to the wave's travel distance.
float EaseTravel(RippleCurve curve, float t) {
    switch (curve) {
        case RippleCurve::Linear:
            return t;
        case RippleCurve::EaseIn:
            return t * t;
        case RippleCurve::EaseOut:
            return 1.0f - (1.0f - t) * (1.0f - t);
        case RippleCurve::EaseInOut:
            return t < 0.5f ? 4.0f * t * t * t
                            : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) * 0.5f;
        case RippleCurve::Smooth:
        default:
            return t * t * (3.0f - 2.0f * t);
    }
}

RippleCurve ParseRippleCurve(std::wstring text) {
    for (wchar_t& c : text) {
        c = towlower(c);
    }
    text = Trim(text);
    if (text == L"linear") return RippleCurve::Linear;
    if (text == L"easein") return RippleCurve::EaseIn;
    if (text == L"easeout") return RippleCurve::EaseOut;
    if (text == L"easeinout") return RippleCurve::EaseInOut;
    return RippleCurve::Smooth;
}

// Overshooting ease, so a moving part settles into place instead of stopping
// dead. Used by the spring of the phone dial and by the desktop HUD pop.
float EaseOutBack(float t) {
    constexpr float kBack = 1.30f;
    const float u = Clamp01(t) - 1.0f;
    return 1.0f + (kBack + 1.0f) * u * u * u + kBack * u * u;
}

// Rounded-rect primitives. They sit above the ripple so that anything in the
// scene code or in the cards below can use them without depending on the order
// the two halves of the file are written in.
struct RoundedRect {
    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 0.0f;
    float y1 = 0.0f;
    float radius = 0.0f;
};

RoundedRect MakeRect(float x0, float y0, float x1, float y1, float radius) {
    RoundedRect rect;
    rect.x0 = std::min(x0, x1);
    rect.y0 = std::min(y0, y1);
    rect.x1 = std::max(x0, x1);
    rect.y1 = std::max(y0, y1);
    const float limit = std::min(rect.x1 - rect.x0, rect.y1 - rect.y0) * 0.5f;
    rect.radius = std::clamp(radius, 0.0f, std::max(0.0f, limit));
    return rect;
}

// Signed distance to the rounded rectangle; negative inside, in device pixels.
float RoundedRectSdf(float px, float py, const RoundedRect& rect) {
    const float cx = (rect.x0 + rect.x1) * 0.5f;
    const float cy = (rect.y0 + rect.y1) * 0.5f;
    const float hx = (rect.x1 - rect.x0) * 0.5f;
    const float hy = (rect.y1 - rect.y0) * 0.5f;
    const float qx = fabsf(px - cx) - (hx - rect.radius);
    const float qy = fabsf(py - cy) - (hy - rect.radius);
    const float ax = std::max(qx, 0.0f);
    const float ay = std::max(qy, 0.0f);
    return sqrtf(ax * ax + ay * ay) + std::min(std::max(qx, qy), 0.0f) - rect.radius;
}

// The scene effects. The ripple refracts the captured desktop along an
// expanding wave and the linear wave does the same along a straight front that
// crosses the screen, the edge glow paints a neon band that hugs the border,
// the bezel presses the whole desktop inward inside a flat frame, the circuit
// trace wakes up a board's routing along the taskbar and in the corners, and
// the matrix rains characters down the screen in columns.
enum class RippleEffect {
    Ripple,
    LinearWave,
    EdgeGlow,
    Bezel,
    Circuit,
    Matrix,
};

RippleEffect ParseRippleEffect(std::wstring text) {
    for (wchar_t& c : text) {
        c = towlower(c);
    }
    text = Trim(text);
    if (text == L"linearwave") return RippleEffect::LinearWave;
    if (text == L"edgeglow") return RippleEffect::EdgeGlow;
    if (text == L"bezel") return RippleEffect::Bezel;
    if (text == L"circuit") return RippleEffect::Circuit;
    if (text == L"matrix") return RippleEffect::Matrix;
    return RippleEffect::Ripple;
}

// Everything the settings can change about the wave itself.
struct RippleStyle {
    double durationMs = 1650.0;
    float strength = 14.0f;
    float opacity = 1.0f;
    // How far the screen behind the effect is darkened, 0 to 1. Phones dim the
    // screen while their charging animation runs; 0 leaves it untouched.
    float dim = 0.0f;
    RippleCurve curve = RippleCurve::Smooth;
    // Reversed waves start at the screen edge and collapse into the origin.
    bool reverse = false;
    // Which scene effect plays.
    RippleEffect effect = RippleEffect::Ripple;
};

// Anchors are taken on the monitor under the cursor, so the ripple starts where
// the user is looking. "cursor" follows the pointer instead of an anchor.
POINT ResolveRippleOrigin(std::wstring origin) {
    POINT anchor{};
    if (!GetCursorPos(&anchor)) {
        const RECT primary = MonitorRectAt(POINT{0, 0});
        anchor = {primary.left + (primary.right - primary.left) / 2,
                  primary.top + (primary.bottom - primary.top) / 2};
    }
    for (wchar_t& c : origin) {
        c = towlower(c);
    }
    origin = Trim(origin);
    if (origin == L"cursor") {
        return anchor;
    }

    const RECT monitor = MonitorRectAt(anchor);
    const int left = monitor.left;
    const int top = monitor.top;
    const int right = monitor.right - 1;
    const int bottom = monitor.bottom - 1;
    const int cx = (left + right) / 2;
    const int cy = (top + bottom) / 2;

    if (origin == L"topleft") return {left, top};
    if (origin == L"top") return {cx, top};
    if (origin == L"topright") return {right, top};
    if (origin == L"left") return {left, cy};
    if (origin == L"center") return {cx, cy};
    if (origin == L"right") return {right, cy};
    if (origin == L"bottomleft") return {left, bottom};
    if (origin == L"bottomright") return {right, bottom};
    return {cx, bottom};
}

////////////////////////////////////////////////////////////////////////////////
// The ripple animation

// Declared here because the bezel needs a picture of the desktop before its
// window covers it, while the definition lives with the cards further down.
bool CaptureRegion(int x, int y, int width, int height, Surface& target);

class Ripple {
public:
    void SetStyle(const RippleStyle& style) { style_ = style; }

    bool Prepare(POINT center, float scale, HWND notify) {
        Stop();
        if (!window_.Hwnd() && !window_.Create()) {
            return false;
        }
        monitor_ = MonitorRectAt(center);
        const int w = monitor_.right - monitor_.left;
        const int h = monitor_.bottom - monitor_.top;
        if (w <= 0 || h <= 0) {
            return false;
        }
        center_ = {static_cast<float>(center.x - monitor_.left),
                   static_cast<float>(center.y - monitor_.top)};
        scale_ = scale;
        float farthest = 0;
        for (const POINT corner : {POINT{0, 0}, POINT{w, 0}, POINT{0, h}, POINT{w, h}}) {
            farthest = std::max(farthest, hypotf(static_cast<float>(corner.x) - center_.x,
                                                 static_cast<float>(corner.y) - center_.y));
        }
        maxRadius_ = farthest + 100.0f * scale;

        // The ring wave reads its reach off maxRadius_ as it goes. The linear
        // one needs the line through the origin, and how far the screen runs
        // along it, worked out up front; the monitor and the origin are both
        // known by this point.
        if (style_.effect == RippleEffect::LinearWave) {
            BuildLinearProfile(w, h);
        }

        // The overlay effects paint the screen themselves and never sample the
        // desktop, so they skip the capture thread and start straight away.
        if (style_.effect != RippleEffect::Ripple &&
            style_.effect != RippleEffect::LinearWave) {
            return LaunchOverlay();
        }

        auto capture = std::make_shared<RippleCapture>();
        capture->monitor = monitor_;
        capture->centerY = static_cast<int>(center_.y);
        capture->notify = notify;
        // Made here, on the thread that goes on to read and present them. The
        // capture worker only draws into them, so their memory DCs outlive it.
        if (!capture->snapshot.Resize(w, h) || !capture->canvas.Resize(w, h)) {
            Wh_Log(L"Could not allocate the capture surfaces for %dx%d.", w, h);
            return false;
        }
        auto* holder = new std::shared_ptr<RippleCapture>(capture);
        HANDLE thread = CreateThread(nullptr, 0, RippleCaptureThreadProc, holder, 0, nullptr);
        if (!thread) {
            delete holder;
            return false;
        }
        CloseHandle(thread);
        capture_ = std::move(capture);
        return true;
    }

    bool Owns(const std::shared_ptr<RippleCapture>& capture) const {
        return capture_ && capture_ == capture;
    }

    double FrameMs() const { return frameMs_; }

    void Launch(double now, double fastestFrameMs) {
        if (!capture_ || active_) {
            return;
        }
        frameMs_ = std::max<double>(capture_->frameMs.load(), fastestFrameMs);
        surface_.Swap(capture_->canvas);
        if (!surface_.Valid() || !capture_->snapshot.Valid()) {
            Stop();
            return;
        }
        ResetFrames(surface_.Height());
        start_ = std::max(now, NowMs());
        active_ = true;
        Render(start_);
        window_.Show();
    }

    double Tick(double now) {
        if (!active_) {
            return 1e9;
        }
        if (now - start_ >= SpanMs()) {
            Stop();
            return 1e9;
        }
        Render(now);
        return frameMs_;
    }

    void Stop() {
        workers_.Reset();
        active_ = false;
        window_.Hide();
        // The scrim goes as soon as the effect does, and its strength is reset
        // so the next event fades up from nothing.
        dimWindow_.Hide();
        dimWindow_.SetStrength(0);
        dimLevel_ = 0.0f;
        if (capture_) {
            // The worker holds its own reference and checks this each stripe,
            // and it no longer touches the window, so letting it finish on its
            // own is enough.
            capture_->canceled = true;
            capture_.reset();
        }
        surface_.Reset();
        wave_.clear();
        rows_.clear();
        glowLut_.clear();
        bezelLut_.clear();
        bezelCapture_.Reset();
        bezelDrawnInset_ = -1;
        circuitTraces_.clear();
        circuitBands_.clear();
        circuitFalloff_.clear();
        circuitCov_.clear();
        circuitHeat_.clear();
        circuitTint_.clear();
        circuitLayerW_ = 0;
        circuitLayerH_ = 0;
        matrixColumns_.clear();
        matrixAtlas_.Reset();
        matrixGlyphs_ = 0;
    }

    bool Active() const { return active_; }

    void Destroy() {
        Stop();
        window_.Destroy();
        dimWindow_.Destroy();
    }

private:
    struct RasterWave {
        float displacement;
        uint32_t opacity;
        uint32_t lightAlpha;
        uint32_t light;
    };

    struct RowSpans {
        int begin[2] = {0, 0};
        int end[2] = {0, 0};
    };

    // A corner of the board's routing, in surface pixels.
    struct CircuitNode {
        float x, y;
    };

    // One run of copper: a polyline that a single charge travels along. The
    // delay keeps the runs from all firing at the same instant, the weight is
    // how wide the track is next to the rail, and a run that ends in a via
    // finishes in a drilled pad rather than simply stopping.
    struct CircuitTrace {
        std::vector<CircuitNode> path;
        float length = 0.0f;
        float delay = 0.0f;
        float weight = 1.0f;
        float tint = 0.0f;
        bool via = false;
    };

    // How hard the charge hits a point a given distance behind its head:
    // nothing ahead of it, a hot spark right at it, and a wake that fades out
    // behind like a comet's.
    static float CircuitGlow(float distance, float tail) {
        if (distance >= 0.0f) {
            return 0.0f;
        }
        const float spark = expf(-(distance * distance) / (tail * tail * 0.05f));
        const float wake = expf(distance / tail);
        return std::min(1.6f, spark + wake);
    }

    void ResetFrames(int height) {
        rows_.assign(static_cast<size_t>(height), RowSpans{});
        drawnFirst_ = drawnLast_ = 0;
        drawnX0_ = drawnX1_ = 0;
        firstFrame_ = true;
        step_ = 1;
        averageMs_ = -1;
    }

    void ClearRow(int y) {
        RowSpans& spans = rows_[y];
        uint32_t* row = surface_.Row(y);
        for (int i = 0; i < 2; i++) {
            if (spans.end[i] > spans.begin[i]) {
                memset(row + spans.begin[i], 0,
                       static_cast<size_t>(spans.end[i] - spans.begin[i]) * sizeof(uint32_t));
            }
        }
        spans = {};
    }

    // Degrade the raster resolution when the machine can't keep up.
    void AdaptQuality(double frameMs) {
        const double limit = std::clamp(frameMs_ * 1.5, 14.0, 20.0);
        averageMs_ = averageMs_ < 0 ? frameMs : averageMs_ * 0.5 + frameMs * 0.5;
        if (step_ < 3 && averageMs_ > limit) {
            step_++;
            averageMs_ = -1;
        } else if (step_ > 1 &&
                   averageMs_ * step_ * step_ / ((step_ - 1) * (step_ - 1)) * 0.8 < limit * 0.6) {
            step_--;
            averageMs_ = -1;
        }
    }

    // Rows next to a stripe the capture has not written yet have to stay clear
    // far enough that a sample pushed by the wave cannot land in one. The
    // Gaussian peaks at 0.6065 of the strength, so that is the furthest a
    // sample can travel, and the block height covers the shared row shading.
    int CaptureMargin() const {
        const float push = 0.6065f * style_.strength * scale_;
        return static_cast<int>(ceilf(push)) + step_ + 2;
    }

    void Render(double now) {
        // The dim is driven first, so the renderer that follows can bake the
        // same fade level into the desktop pixels it paints itself.
        UpdateDim(now);
        switch (style_.effect) {
            case RippleEffect::LinearWave:
                RenderLinearWave(now);
                break;
            case RippleEffect::EdgeGlow:
                RenderGlow(now);
                break;
            case RippleEffect::Bezel:
                RenderBezel(now);
                break;
            case RippleEffect::Circuit:
                RenderCircuit(now);
                break;
            case RippleEffect::Matrix:
                RenderMatrix(now);
                break;
            default:
                RenderRipple(now);
                break;
        }
    }

    // How far the dim is faded in right now, 0 to 1. It follows the animation
    // instead of snapping on: it eases in over the opening quarter second, holds
    // at full strength, and eases back out over the last stretch, so the screen
    // darkens and brightens as one continuous move.
    float DimLevel(double now) const {
        if (style_.dim <= 0.0f) {
            return 0.0f;
        }
        const double span = SpanMs();
        const double fadeIn = std::min(260.0, span * 0.25);
        const double fadeOut = std::min(400.0, span * 0.30);
        const double elapsed = now - start_;
        if (elapsed < fadeIn) {
            const float t = static_cast<float>(std::max(0.0, elapsed) / fadeIn);
            return t * t * (3.0f - 2.0f * t);
        }
        if (elapsed > span - fadeOut) {
            const float t =
                static_cast<float>(std::clamp((span - elapsed) / fadeOut, 0.0, 1.0));
            return t * t * (3.0f - 2.0f * t);
        }
        return 1.0f;
    }

    // Drives the scrim from that level. Whatever a renderer leaves transparent
    // shows the scrim, and whatever it paints over it bakes the same strength
    // into its own pixels through dimLevel_, so the two always agree.
    void UpdateDim(double now) {
        dimLevel_ = DimLevel(now);
        if (style_.dim <= 0.0f) {
            return;
        }
        const BYTE alpha =
            static_cast<BYTE>(Clamp01(style_.dim * dimLevel_) * 255.0f + 0.5f);
        if (alpha == 0) {
            dimWindow_.Hide();
            return;
        }
        if (!dimWindow_.Cover(monitor_)) {
            return;
        }
        dimWindow_.SetStrength(alpha);
        if (!dimWindow_.Visible()) {
            dimWindow_.ShowUnder(window_.Hwnd());
        }
    }

    // Starts an overlay effect at once: nothing here waits on a capture thread,
    // so the first frame is drawn on the calling thread and the timer keeps the
    // rest coming.
    bool LaunchOverlay() {
        const int w = monitor_.right - monitor_.left;
        const int h = monitor_.bottom - monitor_.top;
        if (w <= 0 || h <= 0 || !surface_.Resize(w, h)) {
            return false;
        }
        surface_.Clear();
        ResetFrames(h);
        if (style_.effect == RippleEffect::Bezel) {
            BuildBezelProfile(w, h);
            // The frame scales a picture of the desktop, so grab it now, while
            // the window is still hidden. If it fails the frame still shows and
            // the content underneath is simply left where it is. The picture is
            // captured as it is; the dim is applied as the frame is drawn, so it
            // can follow the fade.
            if (!CaptureRegion(monitor_.left, monitor_.top, w, h, bezelCapture_)) {
                bezelCapture_.Reset();
            }
        } else if (style_.effect == RippleEffect::Circuit) {
            BuildCircuitProfile(w, h);
        } else if (style_.effect == RippleEffect::Matrix) {
            BuildMatrixProfile(w, h);
        } else {
            BuildGlowProfile(w, h);
        }
        // The cascade rewrites the whole monitor every frame, so it runs a
        // little under the usual rate: falling glyphs do not need 60 of them.
        frameMs_ = style_.effect == RippleEffect::Matrix ? kMatrixFrameMs : kFrameMs;
        start_ = NowMs();
        active_ = true;
        Render(start_);
        window_.Show();
        return true;
    }

    // The glow is a per-pixel table lookup, so its falloff is built once: a thin
    // hot core right on the edge and a wide bloom washing inward from it.
    void BuildGlowProfile(int w, int h) {
        band_ = std::max(8.0f, (18.0f + style_.strength * 1.9f) * scale_);
        band_ = std::min(band_, std::min(w, h) * 0.45f);
        bandPx_ = std::max(2, static_cast<int>(ceilf(band_)));
        glowLut_.assign(static_cast<size_t>(bandPx_) + 1, 0.0f);
        UpdateGlowFalloff(band_);
        // The travelling highlight enters from the border point nearest the
        // origin, measured on the same angle the perimeter is read on, so the
        // light comes in where the user pointed it.
        const float dx = center_.x - static_cast<float>(w) * 0.5f;
        const float dy = center_.y - static_cast<float>(h) * 0.5f;
        glowStart_ = FastAtan2(dy, dx) / (2.0f * kPi) + 0.5f;
        glowStart_ -= floorf(glowStart_);
    }

    // Rebuilds the falloff for the width lit this frame, so the band can swell out
    // of the border instead of appearing at full width.
    void UpdateGlowFalloff(float width) {
        const float core = std::max(1.5f, 2.2f * scale_);
        const float reach = std::max(1.0f, width);
        for (int d = 0; d <= bandPx_; d++) {
            const float distance = static_cast<float>(d);
            const float q = Clamp01(1.0f - distance / reach);
            const float bloom = powf(q, 2.4f);
            const float hot = distance < core ? 1.0f - distance / core : 0.0f;
            glowLut_[d] = std::min(1.0f, bloom * 0.85f + hot * 0.55f);
        }
    }

    // The edge glow: a neon band along the monitor border, coloured by a gradient
    // that runs around the screen, with one hot spot orbiting it. The band swells
    // in, holds and fades, so the light lingers instead of blinking.
    void RenderGlow(double now) {
        const int w = surface_.Width();
        const int h = surface_.Height();
        if (w <= 0 || h <= 0 || glowLut_.empty() ||
            rows_.size() != static_cast<size_t>(h)) {
            return;
        }

        const float t = static_cast<float>(
            std::clamp((now - start_) / style_.durationMs, 0.0, 1.0));
        const float elapsed = static_cast<float>(now - start_);
        const float riseT = Clamp01(elapsed / 260.0f);
        const float swell = riseT * riseT * (3.0f - 2.0f * riseT);
        const float outStart = 0.55f;
        const float fall = Clamp01((t - outStart) / (1.0f - outStart));
        const float fadeOut = 1.0f - fall * fall * (3.0f - 2.0f * fall);
        const float alpha = swell * fadeOut * style_.opacity;
        // The band widens as it lights up and narrows again as it fades, so it
        // blooms out of the border instead of appearing at full width.
        UpdateGlowFalloff(band_ * (0.5f + 0.5f * swell) * (0.6f + 0.4f * fadeOut));
        const uint32_t coverage = static_cast<uint32_t>(Clamp01(alpha) * 255.0f + 0.5f);

        // The perimeter gradient. A rolling gradient drifts with time, a flat
        // colour simply repeats the same sample all the way round.
        const bool rainbow = g_glow.rainbow;
        uint32_t colors[64];
        for (int i = 0; i < 64; i++) {
            colors[i] = PackPremultiplied(g_glow.At(i / 64.0f + t * 0.35f), 1.0f);
        }
        const uint32_t flat = PackPremultiplied(g_glow.color, 1.0f);

        // The hot spot travels once around the screen, starting at the origin and
        // running the other way round for reverse events.
        const float travel = EaseTravel(style_.curve, t);
        const float spot =
            style_.reverse ? glowStart_ - travel : glowStart_ + travel;
        constexpr int kHeadSteps = 128;
        float spotLut[kHeadSteps];
        for (int i = 0; i < kHeadSteps; i++) {
            float delta = static_cast<float>(i) / kHeadSteps - spot;
            delta -= floorf(delta + 0.5f);
            const float falloff = delta / 0.06f;
            spotLut[i] = 0.55f + 0.80f * expf(-falloff * falloff);
        }

        const float cx = w * 0.5f;
        const float cy = h * 0.5f;
        const float* lut = glowLut_.data();
        const int band = bandPx_;
        const int lastX = w - 1;
        const int lastY = h - 1;

        auto renderRow = [&](int y) {
            ClearRow(y);
            uint32_t* row = surface_.Row(y);
            RowSpans spans;
            // Distance from the top or bottom border, and how far the row reaches.
            const float dy = static_cast<float>(y) - cy;
            const int borderY = std::min(y, lastY - y);
            int segments = 0;
            if (borderY >= band) {
                const int leftEnd = std::min(band, w);
                spans.begin[0] = 0;
                spans.end[0] = leftEnd;
                segments = 1;
                if (lastX - band >= leftEnd) {
                    spans.begin[1] = lastX - band + 1;
                    spans.end[1] = w;
                    segments = 2;
                }
            } else {
                // Rows this close to the border are lit end to end.
                spans.begin[0] = 0;
                spans.end[0] = w;
                segments = 1;
            }
            for (int s = 0; s < segments; s++) {
                for (int x = spans.begin[s]; x < spans.end[s]; x++) {
                    const int borderX = std::min(x, lastX - x);
                    const int distance = std::min(borderY, borderX);
                    if (distance >= band) {
                        continue;
                    }
                    float u = FastAtan2(dy, static_cast<float>(x) - cx) /
                                  (2.0f * kPi) + 0.5f;
                    u -= floorf(u);
                    const float lit = lut[distance] *
                                      spotLut[static_cast<int>(u * kHeadSteps) &
                                              (kHeadSteps - 1)];
                    if (lit <= 0.003f) {
                        continue;
                    }
                    const float intensity = std::min(1.0f, lit);
                    row[x] = ScalePremultiplied(
                        rainbow ? colors[static_cast<int>(u * 64.0f) & 63] : flat,
                        static_cast<uint32_t>(intensity * coverage + 0.5f));
                }
            }
            rows_[y] = spans;
        };

        workers_.ForRows(0, h, renderRow, true, 8);

        if (firstFrame_) {
            window_.Present(surface_, monitor_.left, monitor_.top, 255);
            firstFrame_ = false;
        } else {
            // Only the border band changes while the glow is on, so upload it as
            // four strips instead of the whole monitor every frame.
            const int inner = std::max(band, h - band);
            const RECT strips[4] = {
                {0, 0, w, std::min(band, h)},
                {0, std::max(0, h - band), w, h},
                {0, band, std::min(band, lastX + 1), inner},
                {std::max(band, lastX + 1 - band), band, w, inner},
            };
            for (const RECT& strip : strips) {
                if (strip.right > strip.left && strip.bottom > strip.top) {
                    window_.Present(surface_, monitor_.left, monitor_.top, 255, &strip);
                }
            }
        }
    }

    // The circuit is drawn rather than sampled, so its cross section is a table
    // indexed by distance from the copper: a solid core with a one pixel
    // antialiased edge, a tight sheath of light around it, and a wide bloom that
    // washes outward and dies out at the reach.
    void BuildCircuitFalloff() {
        circuitThickness_ = std::max(0.9f, (0.75f + style_.strength * 0.045f) * scale_);
        circuitReach_ = std::max(5.0f, (4.0f + style_.strength * 0.34f) * scale_);
        const int cells = static_cast<int>(circuitReach_ * kCircuitLutScale) + 2;
        circuitFalloff_.assign(static_cast<size_t>(cells), 0);
        const float half = circuitThickness_ * 0.5f;
        for (int i = 0; i < cells; i++) {
            const float d = static_cast<float>(i) / kCircuitLutScale;
            const float solid = Clamp01(half + 0.5f - d);
            const float sheath =
                0.34f * expf(-(d * d) / (half * half * 6.0f + 0.25f));
            const float q = Clamp01(1.0f - d / circuitReach_);
            const float bloom = 0.30f * q * q * q;
            const float total = solid + (sheath + bloom) * (1.0f - solid);
            circuitFalloff_[static_cast<size_t>(i)] =
                static_cast<uint8_t>(Clamp01(total) * 255.0f + 0.5f);
        }
    }

    // Files a run away with its length already summed, since that is the distance
    // the charge has to travel along it.
    void AddCircuitTrace(std::vector<CircuitNode> path, float delay, bool via,
                         float weight, float tint) {
        if (path.size() < 2) {
            return;
        }
        CircuitTrace trace;
        trace.path = std::move(path);
        trace.delay = delay;
        trace.via = via;
        trace.weight = weight;
        trace.tint = tint;
        for (size_t i = 1; i < trace.path.size(); i++) {
            const float dx = trace.path[i].x - trace.path[i - 1].x;
            const float dy = trace.path[i].y - trace.path[i - 1].y;
            trace.length += sqrtf(dx * dx + dy * dy);
        }
        circuitTraces_.push_back(std::move(trace));
    }

    // Lays the board out once, when the effect starts: a power rail across the
    // bottom of the screen that changes lane at forty-five degrees, a bundle of
    // thin signal lines beside one stretch of it, risers dropping off the rail
    // toward the taskbar that finish in vias, elbows turning out of both bottom
    // corners, and runs in the two top corners. The traces are all that is ever
    // drawn, so their bounding bands are kept for presentation.
    void BuildCircuitProfile(int w, int h) {
        circuitTraces_.clear();
        circuitBands_.clear();
        BuildCircuitFalloff();

        const RECT usable = UsableRect({monitor_.left + w / 2, monitor_.top + h / 2});
        const int workBottom = std::clamp(
            static_cast<int>(usable.bottom) - static_cast<int>(monitor_.top), 40, h);
        int bottomH = static_cast<int>(
            std::clamp(static_cast<float>(h) * 0.17f, 78.0f * scale_, 250.0f * scale_));
        bottomH = std::min(bottomH, std::max(24, workBottom - 8));
        // The rail is the widest run on the board, so its reach is what the
        // bands are padded with.
        const int reach = static_cast<int>(ceilf(circuitReach_ * 1.6f));

        uint32_t seed = static_cast<uint32_t>(NowMs()) | 1u;
        auto next = [&seed]() {
            seed = seed * 1664525u + 1013904223u;
            return (seed >> 8) & 0xFFFFu;
        };
        auto unit = [&next]() { return static_cast<float>(next()) / 65535.0f; };

        const float bandTop = static_cast<float>(workBottom - bottomH);
        const float railMin = bandTop + 12.0f * scale_;
        const float railMax =
            std::max(railMin, static_cast<float>(workBottom) - 30.0f * scale_);

        // Each level stretch is remembered so a riser can hang off it and stay
        // attached to the rail instead of stopping short of it.
        struct Lane {
            float y;
            float x0;
            float x1;
        };
        std::vector<Lane> lanes;
        std::vector<CircuitNode> rail;
        float laneY = std::clamp(static_cast<float>(workBottom) -
                                     static_cast<float>(bottomH) * 0.64f,
                                 railMin, railMax);
        float x = -30.0f;
        rail.push_back({x, laneY});
        while (x < static_cast<float>(w) + 30.0f) {
            const float end = x + (120.0f + unit() * 200.0f) * scale_;
            rail.push_back({end, laneY});
            lanes.push_back({laneY, x, end});
            if (end > static_cast<float>(w) + 30.0f) {
                break;
            }
            const float step = (9.0f + unit() * 16.0f) * scale_;
            const float nextY =
                std::clamp(laneY + (unit() < 0.5f ? -step : step), railMin, railMax);
            x = end + fabsf(nextY - laneY);
            laneY = nextY;
            rail.push_back({x, laneY});
        }
        // The rail carries the charge for the whole board, so it is drawn wider
        // than anything else on it.
        AddCircuitTrace(std::move(rail), 0.0f, false, 1.45f, 0.0f);

        // A bundle of thin signal lines runs beside one of the long stretches
        // and fans away at both ends, the way a bus leaves the power rail.
        if (!lanes.empty()) {
            const Lane& lane = lanes[next() % lanes.size()];
            const float gap = std::max(2.4f, circuitThickness_ * 2.6f);
            const float busX0 = lane.x0 + 20.0f * scale_;
            const float busX1 = lane.x1 - 20.0f * scale_;
            if (busX1 - busX0 > 140.0f * scale_) {
                for (int i = 0; i < 3; i++) {
                    const float drop = gap * static_cast<float>(i + 1);
                    const float y = lane.y + drop;
                    const float ramp = drop + 8.0f * scale_;
                    std::vector<CircuitNode> line;
                    line.push_back({busX0 - ramp, y + ramp});
                    line.push_back({busX0, y});
                    line.push_back({busX1, y});
                    line.push_back({busX1 + ramp, y + ramp});
                    AddCircuitTrace(std::move(line), 0.05f + 0.03f * i, true,
                                    0.62f, 0.03f + 0.035f * i);
                }
            }
        }

        // Risers leave the rail for the taskbar and end in a via, the way a track
        // leaves the board to meet a connector.
        const float floorY = static_cast<float>(workBottom) - circuitReach_ - 3.0f;
        const int risers = 3 + static_cast<int>(unit() * 3.0f);
        for (int i = 0; i < risers && !lanes.empty(); i++) {
            for (int attempt = 0; attempt < 6; attempt++) {
                const Lane& lane = lanes[next() % lanes.size()];
                const float margin = 26.0f * scale_;
                if (lane.x1 - lane.x0 < margin * 2.0f ||
                    floorY - lane.y < 20.0f * scale_) {
                    continue;
                }
                const float riserX =
                    lane.x0 + margin + unit() * (lane.x1 - lane.x0 - margin * 2.0f);
                std::vector<CircuitNode> riser;
                riser.push_back({riserX, lane.y});
                riser.push_back({riserX, floorY - unit() * 12.0f * scale_});
                AddCircuitTrace(std::move(riser), 0.12f + unit() * 0.26f, true,
                                0.8f + unit() * 0.3f, 0.10f + unit() * 0.10f);
                break;
            }
        }

        // Bottom corners: an elbow rising off the taskbar edge and turning inward,
        // so the board reads as continuing past the screen.
        for (int side = 0; side < 2; side++) {
            const float edge = side == 0 ? 0.0f : static_cast<float>(w);
            const float dir = side == 0 ? 1.0f : -1.0f;
            const float inset = (26.0f + unit() * 14.0f) * scale_;
            const float rise = (26.0f + unit() * 12.0f) * scale_;
            const float turn = rise * 0.9f;
            const float run = (100.0f + unit() * 80.0f) * scale_;
            const float topY = static_cast<float>(workBottom) - rise - turn;
            std::vector<CircuitNode> elbow;
            elbow.push_back({edge + dir * inset, static_cast<float>(workBottom)});
            elbow.push_back({edge + dir * inset, topY + turn});
            elbow.push_back({edge + dir * (inset + turn), topY});
            elbow.push_back({edge + dir * (inset + turn + run), topY});
            AddCircuitTrace(std::move(elbow), 0.06f + 0.1f * side, true, 1.1f,
                            0.18f + 0.05f * side);
        }

        const int topH =
            static_cast<int>(std::clamp(120.0f * scale_, 80.0f * scale_, 170.0f * scale_));
        const int cornerW = std::min(static_cast<int>(static_cast<float>(w) * 0.3f),
                                     static_cast<int>(300.0f * scale_));
        if (cornerW >= 120) {
            for (int side = 0; side < 2; side++) {
                const float edge = side == 0 ? 0.0f : static_cast<float>(w);
                const float dir = side == 0 ? 1.0f : -1.0f;
                const float room = static_cast<float>(cornerW) - 10.0f;
                const float startOffset = (22.0f + unit() * 16.0f) * scale_;
                const float drop = (26.0f + unit() * 12.0f) * scale_;
                const float turn = drop * 0.85f;
                const float run =
                    std::max(20.0f, std::min((80.0f + unit() * 90.0f) * scale_,
                                             room - startOffset - turn));
                const float startX = edge + dir * startOffset;
                std::vector<CircuitNode> top;
                top.push_back({startX, -1.0f});
                top.push_back({startX, drop});
                top.push_back({startX + dir * turn, drop + turn});
                top.push_back({startX + dir * (turn + run), drop + turn});
                AddCircuitTrace(std::move(top), 0.08f + 0.07f * side, true, 0.95f,
                                0.24f + 0.05f * side);

                // A shorter run below it, entering from off the edge, so the
                // corner has a little density instead of a single line.
                const float spurY = drop + turn + (18.0f + unit() * 14.0f) * scale_;
                const float spurRun = std::min((50.0f + unit() * 70.0f) * scale_, room * 0.7f);
                std::vector<CircuitNode> spur;
                spur.push_back({edge - dir * 6.0f, spurY});
                spur.push_back({edge + dir * spurRun, spurY});
                AddCircuitTrace(std::move(spur), 0.18f + 0.06f * side, true, 0.7f,
                                0.30f + 0.05f * side);
            }
        }

        circuitBands_.push_back(
            RECT{0, std::max(0, workBottom - bottomH - reach), w,
                 std::min(h, workBottom + reach)});
        if (cornerW >= 120) {
            const int bandH = std::min(h, topH + reach);
            circuitBands_.push_back(RECT{0, 0, std::min(w, cornerW), bandH});
            circuitBands_.push_back(RECT{std::max(0, w - cornerW), 0, w, bandH});
        }

        // The scratch each band is accumulated into, one for coverage, one for
        // how hot the charge is there and one for the tint of the run that laid
        // the copper. It is sized once for the widest and tallest band and
        // reused for all of them.
        int layerW = 0;
        int layerH = 0;
        for (const RECT& band : circuitBands_) {
            layerW = std::max(layerW, static_cast<int>(band.right - band.left));
            layerH = std::max(layerH, static_cast<int>(band.bottom - band.top));
        }
        circuitLayerW_ = std::max(1, layerW);
        circuitLayerH_ = std::max(1, layerH);
        const size_t layerPixels =
            static_cast<size_t>(circuitLayerW_) * static_cast<size_t>(circuitLayerH_);
        circuitCov_.assign(layerPixels, 0);
        circuitHeat_.assign(layerPixels, 0);
        circuitTint_.assign(layerPixels, 0);
    }

    // The circuit trace: the board wakes up faintly, a charge runs along every
    // trace in turn leaving a wake behind it, the vias ring as it arrives, and
    // the whole thing fades back to nothing.
    void RenderCircuit(double now) {
        const int w = surface_.Width();
        const int h = surface_.Height();
        if (w <= 0 || h <= 0 || circuitBands_.empty() || circuitFalloff_.empty() ||
            circuitLayerW_ <= 0 ||
            circuitCov_.size() <
                static_cast<size_t>(circuitLayerW_) * static_cast<size_t>(circuitLayerH_)) {
            return;
        }

        const float span = static_cast<float>(style_.durationMs);
        const float elapsed = static_cast<float>(now - start_);
        const float inMs = std::min(200.0f, span * 0.25f);
        const float outMs = std::min(span * 0.32f, 700.0f);
        float envelope = 1.0f;
        if (elapsed < inMs) {
            const float q = Clamp01(elapsed / inMs);
            envelope = q * q * (3.0f - 2.0f * q);
        } else if (elapsed > span - outMs) {
            const float q = Clamp01((elapsed - span + outMs) / outMs);
            envelope = 1.0f - q * q * (3.0f - 2.0f * q);
        }

        // The charge crosses each trace once; what is left afterwards is the
        // board breathing on standby power until the fade takes it.
        const float travelMs = std::clamp(span * 0.45f, 240.0f, 900.0f);
        const float lead = inMs * 0.6f;
        const float tail = 64.0f * scale_;
        const float t = Clamp01(elapsed / span);
        const float breath = 0.85f + 0.15f * sinf(t * 9.424778f);
        const float standby = 0.18f * envelope * breath * style_.opacity;
        const float peak = 0.95f * envelope * style_.opacity;

        // The board carries one current, so it travels at one speed on every
        // trace: a short run is over in a flash, and the longest run, which is
        // always the rail, takes the whole travel time.
        float longest = 1.0f;
        for (const CircuitTrace& run : circuitTraces_) {
            longest = std::max(longest, run.length);
        }
        const float speed = (longest + tail) / std::max(1.0f, travelMs);
        auto localOf = [&](const CircuitTrace& run) {
            const float runMs = std::max(48.0f, (run.length + tail) / speed);
            return Clamp01((elapsed - lead - run.delay * travelMs) / runMs);
        };

        // How hard the charge hits a point only depends on how far behind the
        // head that point is, so the profile is a table and the per pixel work
        // is one subtraction and one lookup. Interpolating between the two ends
        // of a segment instead would put the head in the wrong place on a long
        // run, which is what a straight line the width of the screen would show.
        const float tailSpan = tail * 4.5f;
        float glowLut[256];
        for (int i = 0; i < 256; i++) {
            const float along =
                -tailSpan + tailSpan * (static_cast<float>(i) + 0.5f) / 256.0f;
            glowLut[i] = CircuitGlow(along, tail);
        }
        const float glowScale = 256.0f / tailSpan;
        const float heatScale = 1.0f / 1.6f;

        // Each run takes its own sample of the palette, so a rolling gradient
        // ends up with different colors on different traces.
        std::vector<Color> tints(circuitTraces_.size());
        for (size_t i = 0; i < tints.size(); i++) {
            tints[i] = g_glow.At(fmodf(circuitTraces_[i].tint + t * 0.15f, 1.0f));
        }

        const uint8_t* lut = circuitFalloff_.data();
        const int lutCount = static_cast<int>(circuitFalloff_.size());
        const int layerW = circuitLayerW_;
        uint8_t* covBase = circuitCov_.data();
        uint8_t* heatBase = circuitHeat_.data();
        uint8_t* tintBase = circuitTint_.data();
        const bool back = style_.reverse;
        const Color white = Rgb(255, 255, 255);
        const Color holeInk = Rgb(10, 12, 16);
        // A via is a drilled pad, so it is wider than the track that reaches it.
        const float padRadius = std::max(2.5f, circuitThickness_ * 3.4f);
        const float holeRadius = padRadius * 0.38f;

        auto drawBand = [&](const RECT& band) {
            const int clipX0 = std::clamp(static_cast<int>(band.left), 0, w);
            const int clipX1 = std::clamp(static_cast<int>(band.right), 0, w);
            const int clipY0 = std::clamp(static_cast<int>(band.top), 0, h);
            const int clipY1 = std::clamp(static_cast<int>(band.bottom), 0, h);
            if (clipX1 <= clipX0 || clipY1 <= clipY0) {
                return false;
            }
            const int spanW = clipX1 - clipX0;
            const int spanH = clipY1 - clipY0;

            // Only the band is touched, so every other row keeps the clear pixels
            // it already holds.
            for (int y = clipY0; y < clipY1; y++) {
                memset(surface_.Row(y) + clipX0, 0,
                       static_cast<size_t>(spanW) * sizeof(uint32_t));
                const size_t at = static_cast<size_t>(y - clipY0) * layerW;
                memset(covBase + at, 0, static_cast<size_t>(spanW));
                memset(heatBase + at, 0, static_cast<size_t>(spanW));
                memset(tintBase + at, 0, static_cast<size_t>(spanW));
            }

            // Every pixel of copper is collected first and blended once, so the
            // joints where two segments meet are not laid down twice and the
            // corners come out clean instead of blotchy.
            for (size_t index = 0; index < circuitTraces_.size(); index++) {
                const CircuitTrace& trace = circuitTraces_[index];
                const float local = localOf(trace);
                if (local <= 0.0f) {
                    continue;
                }
                // Forwards the charge leaves the rail and runs out to the far
                // end of the trace; reverse runs it back the other way, so an
                // unplug drains the board instead of charging it.
                const float travelLen = local * (trace.length + tail);
                const float head = back ? trace.length - travelLen : travelLen - tail;
                const float weight = std::max(0.4f, trace.weight);
                const float reach = circuitReach_ * weight;
                const uint8_t tintIndex =
                    static_cast<uint8_t>(std::min<size_t>(index, 255));
                float arc = 0.0f;
                for (size_t i = 1; i < trace.path.size(); i++) {
                    const CircuitNode& a = trace.path[i - 1];
                    const CircuitNode& b = trace.path[i];
                    const float dx = b.x - a.x;
                    const float dy = b.y - a.y;
                    const float length = sqrtf(dx * dx + dy * dy);
                    if (length < 0.01f) {
                        continue;
                    }
                    // Where the head sits on this segment, as a fraction of it.
                    // A segment the head has already left is lit all the way
                    // across, one it has not reached yet is dark.
                    const float vHead = (head - arc) / length;
                    const bool ahead = back ? vHead >= 1.0f : vHead <= 0.0f;
                    arc += length;

                    const int bx0 = std::max(
                        clipX0,
                        static_cast<int>(floorf(std::min(a.x, b.x) - reach)));
                    const int bx1 = std::min(
                        clipX1,
                        static_cast<int>(ceilf(std::max(a.x, b.x) + reach)));
                    const int by0 = std::max(
                        clipY0,
                        static_cast<int>(floorf(std::min(a.y, b.y) - reach)));
                    const int by1 = std::min(
                        clipY1,
                        static_cast<int>(ceilf(std::max(a.y, b.y) + reach)));
                    if (bx1 <= bx0 || by1 <= by0) {
                        continue;
                    }

                    const float inv = 1.0f / (length * length);
                    for (int y = by0; y < by1; y++) {
                        const float py = static_cast<float>(y) + 0.5f;
                        const size_t at = static_cast<size_t>(y - clipY0) * layerW;
                        uint8_t* covRow = covBase + at;
                        uint8_t* heatRow = heatBase + at;
                        uint8_t* tintRow = tintBase + at;
                        for (int x = bx0; x < bx1; x++) {
                            const float fx = static_cast<float>(x) + 0.5f;
                            // Project onto the segment and clamp, which also
                            // gives the runs round caps where they meet.
                            float v = ((fx - a.x) * dx + (py - a.y) * dy) * inv;
                            v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
                            const float ox = fx - (a.x + dx * v);
                            const float oy = py - (a.y + dy * v);
                            // A wide track is the same cross section at a bigger
                            // size, so the distance is measured against it.
                            const float distance = sqrtf(ox * ox + oy * oy) / weight;
                            if (distance >= circuitReach_) {
                                continue;
                            }
                            const int cell = std::min(
                                static_cast<int>(distance * kCircuitLutScale),
                                lutCount - 1);
                            const uint8_t coverage = lut[cell];
                            const int lx = x - clipX0;
                            if (coverage <= covRow[lx]) {
                                continue;
                            }
                            float heat = 0.0f;
                            if (!ahead) {
                                const float along =
                                    (back ? vHead - v : v - vHead) * length;
                                if (along < 0.0f) {
                                    const int lookup = std::clamp(
                                        static_cast<int>((along + tailSpan) * glowScale),
                                        0, 255);
                                    heat = glowLut[lookup] * heatScale;
                                }
                            }
                            covRow[lx] = coverage;
                            heatRow[lx] = static_cast<uint8_t>(
                                Clamp01(heat) * 255.0f + 0.5f);
                            tintRow[lx] = tintIndex;
                        }
                    }
                }
            }

            // One blend per pixel. The copper keeps its tint while it is only
            // standing by and turns white where a charge is running through it.
            workers_.ForRows(0, spanH, [&](int y) {
                uint32_t* row = surface_.Row(clipY0 + y) + clipX0;
                const size_t at = static_cast<size_t>(y) * layerW;
                const uint8_t* covRow = covBase + at;
                const uint8_t* heatRow = heatBase + at;
                const uint8_t* tintRow = tintBase + at;
                for (int x = 0; x < spanW; x++) {
                    const uint8_t coverage = covRow[x];
                    if (coverage == 0) {
                        continue;
                    }
                    const float heat = heatRow[x] * (1.0f / 255.0f);
                    const float alpha = coverage * (1.0f / 255.0f) *
                                        (standby + heat * peak);
                    if (alpha <= 0.002f) {
                        continue;
                    }
                    const size_t tint = std::min<size_t>(tintRow[x], tints.size() - 1);
                    const Color ink = Mix(tints[tint], white, Clamp01(heat * 1.35f));
                    BlendOver(row[x], PackPremultiplied(ink, Clamp01(alpha)));
                }
            }, true, 16);

            // The vias the runs finish in: a drilled pad that rings as the charge
            // arrives and settles back to the standby glow with it.
            for (size_t index = 0; index < circuitTraces_.size(); index++) {
                const CircuitTrace& trace = circuitTraces_[index];
                if (!trace.via || trace.path.empty()) {
                    continue;
                }
                const float local = localOf(trace);
                if (local <= 0.0f) {
                    continue;
                }
                const CircuitNode& end = trace.path.back();
                const float travelLen = local * (trace.length + tail);
                const float head = back ? trace.length - travelLen : travelLen - tail;
                // A reverse charge reaches the via first and lets it go, so the
                // pad rings as the run starts rather than as it ends.
                const float arrival = back ? head - trace.length : trace.length - head;
                const float heat = Clamp01(CircuitGlow(arrival, tail) * heatScale);
                const float alpha = standby + heat * peak * 1.15f;
                if (alpha <= 0.002f) {
                    continue;
                }
                const size_t tint = std::min<size_t>(index, tints.size() - 1);
                const Color ink = Mix(tints[tint], white, Clamp01(heat * 1.35f));
                const int px0 = std::max(
                    clipX0, static_cast<int>(floorf(end.x - padRadius)));
                const int px1 = std::min(
                    clipX1, static_cast<int>(ceilf(end.x + padRadius)));
                const int py0 = std::max(
                    clipY0, static_cast<int>(floorf(end.y - padRadius)));
                const int py1 = std::min(
                    clipY1, static_cast<int>(ceilf(end.y + padRadius)));
                for (int y = py0; y < py1; y++) {
                    uint32_t* row = surface_.Row(y);
                    const float oy = static_cast<float>(y) + 0.5f - end.y;
                    for (int x = px0; x < px1; x++) {
                        const float ox = static_cast<float>(x) + 0.5f - end.x;
                        const float distance = sqrtf(ox * ox + oy * oy);
                        if (distance >= padRadius) {
                            continue;
                        }
                        if (distance <= holeRadius) {
                            // The drill through the middle of the pad, cut out of
                            // the copper the run has already laid there.
                            BlendOver(row[x],
                                      PackPremultiplied(holeInk, 0.55f * envelope));
                            continue;
                        }
                        const float q =
                            (distance - holeRadius) / (padRadius - holeRadius);
                        const float ring =
                            q < 0.55f ? 1.0f : 1.0f - (q - 0.55f) * 1.6667f;
                        BlendOver(row[x],
                                  PackPremultiplied(ink, Clamp01(ring * alpha)));
                    }
                }
            }
            return true;
        };

        if (firstFrame_) {
            for (const RECT& band : circuitBands_) {
                drawBand(band);
            }
            // A dirty rect only works once the window has been presented at this
            // size and position, so the first frame always goes whole.
            window_.Present(surface_, monitor_.left, monitor_.top, 255);
            firstFrame_ = false;
            return;
        }
        for (const RECT& band : circuitBands_) {
            if (drawBand(band)) {
                window_.Present(surface_, monitor_.left, monitor_.top, 255, &band);
            }
        }
    }

    // One column of the cascade: where it sits, how far its head has fallen, how
    // long its trail is and when the sweep out of the origin releases it.
    struct MatrixColumn {
        int left = 0;        // first pixel column it owns
        float head = 0.0f;   // where the head starts, in cells off screen
        float speed = 0.0f;  // cells per second, negative when it climbs
        float trail = 0.0f;  // cells lit behind the head
        float delay = 0.0f;  // seconds before the sweep reaches it
        float hue = 0.0f;    // where it samples the palette
        uint32_t blank = 0;  // how readily its cells go dark, 0-255
    };

    // A cheap integer hash. The character in a cell and whether that cell is
    // dark both come out of it, so the glyphs stay put while the trail moves
    // over them.
    static uint32_t MatrixHash(uint32_t a, uint32_t b) {
        uint32_t v = (a * 0x9E3779B9u) ^ (b * 0x85EBCA6Bu) ^ 0x27D4EB2Fu;
        v ^= v >> 15;
        v *= 0x2545F491u;
        v ^= v >> 13;
        return v;
    }

    // The cascade: a screen-wide grid of character cells, one column per cell,
    // each with its own head, speed and trail. Drawing text every frame would be
    // far too slow, so the characters are rasterised once into an atlas and a
    // frame only copies their coverage out of it.
    void BuildMatrixProfile(int w, int h) {
        matrixColumns_.clear();
        matrixAtlas_.Reset();
        matrixCellW_ = 0;
        matrixCellH_ = 0;
        matrixGlyphs_ = 0;

        const int fontPx = std::max(9, static_cast<int>(14.0f * scale_ + 0.5f));
        const int cellH = std::max(
            8, static_cast<int>(static_cast<float>(fontPx) * 1.34f + 0.5f));
        const int cellW = std::max(
            6, static_cast<int>(static_cast<float>(fontPx) * 0.64f + 0.5f));
        const int glyphs = static_cast<int>(kMatrixLastChar - kMatrixFirstChar) + 1;
        if (!matrixAtlas_.Resize(cellW * glyphs, cellH)) {
            return;
        }

        HFONT font = CreateFontW(-fontPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 ANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN,
                                 L"Consolas");
        if (!font) {
            matrixAtlas_.Reset();
            return;
        }
        HDC dc = matrixAtlas_.Dc();
        HGDIOBJ previous = SelectObject(dc, font);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        matrixAtlas_.Clear();
        for (int i = 0; i < glyphs; i++) {
            const wchar_t glyph[2] = {static_cast<wchar_t>(kMatrixFirstChar + i), 0};
            RECT cell{i * cellW, 0, (i + 1) * cellW, cellH};
            DrawTextW(dc, glyph, 1, &cell,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
        GdiFlush();
        SelectObject(dc, previous);
        DeleteObject(font);

        // GDI text carries no alpha, so the brightness of a pixel becomes its
        // coverage: one byte that any colour and any strength multiply later.
        for (int y = 0; y < cellH; y++) {
            uint32_t* row = matrixAtlas_.Row(y);
            for (int x = 0; x < cellW * glyphs; x++) {
                const uint32_t pixel = row[x];
                const uint32_t coverage = std::max(
                    {(pixel >> 16) & 0xFFu, (pixel >> 8) & 0xFFu, pixel & 0xFFu});
                row[x] = (coverage << 24) | (coverage << 16) | (coverage << 8) |
                         coverage;
            }
        }

        matrixCellW_ = cellW;
        matrixCellH_ = cellH;
        matrixGlyphs_ = glyphs;

        const float span = static_cast<float>(SpanMs());
        const int rows = std::max(1, (h + cellH - 1) / cellH);
        const int cols = std::max(1, (w + cellW - 1) / cellW);
        // The sweep releases the columns one after another, so the last ones to
        // go only have the rest of the span to cross the screen.
        const float sweepMs = std::min(420.0f, span * 0.30f);
        const float fallMs = std::max(240.0f, span * 0.85f - sweepMs);
        const float trailCells =
            std::clamp(5.0f + style_.strength * 0.75f, 5.0f, static_cast<float>(rows));
        // A screen a second is about as fast as an eye can follow a character.
        const float travel =
            std::min(60.0f, (static_cast<float>(rows) + trailCells) / (fallMs * 0.001f));

        matrixColumns_.resize(static_cast<size_t>(cols));
        uint32_t seed = 0x243F6A88u;
        auto next = [&seed]() {
            seed = seed * 1664525u + 1013904223u;
            return static_cast<float>((seed >> 9) & 0x7FFFu) / 32767.0f;
        };
        const float halfWidth = std::max(1.0f, static_cast<float>(w) * 0.5f);
        for (int i = 0; i < cols; i++) {
            MatrixColumn& column = matrixColumns_[static_cast<size_t>(i)];
            column.left = i * cellW;
            column.trail = trailCells * (0.55f + 0.85f * next());
            const float speed = travel * (0.70f + 0.60f * next());
            // Forwards the characters fall and the columns are released from the
            // origin outward. Reverse sends them back up and starts the release
            // at the edges, so the cascade closes in on the origin instead.
            column.speed = style_.reverse ? -speed : speed;
            // Every head starts off screen, so the columns enter it rather than
            // blinking into place.
            column.head = style_.reverse
                              ? static_cast<float>(rows) + column.trail * (0.4f + next())
                              : -column.trail * (0.4f + next());
            const float sideways = Clamp01(
                fabsf(static_cast<float>(column.left) - center_.x) / halfWidth);
            column.delay =
                (style_.reverse ? 1.0f - sideways : sideways) * sweepMs * 0.001f;
            column.hue = next();
            column.blank = static_cast<uint32_t>(18.0f + 46.0f * next());
        }
    }

    // The cascade itself: columns of characters, released from the origin outward
    // on the way in and closing back onto it on the way out, then fading away.
    // Nothing is captured from the desktop, so it costs nothing to start and
    // nothing lingers once it is over.
    void RenderMatrix(double now) {
        const int w = surface_.Width();
        const int h = surface_.Height();
        const int cellW = matrixCellW_;
        const int cellH = matrixCellH_;
        if (w <= 0 || h <= 0 || cellW <= 0 || cellH <= 0 || matrixGlyphs_ <= 0 ||
            matrixColumns_.empty() || !matrixAtlas_.Valid()) {
            return;
        }

        const float span = static_cast<float>(SpanMs());
        const float elapsed = static_cast<float>(now - start_);
        // It arrives in a blink and lets go over the tail, so the desktop comes
        // back gradually instead of snapping on.
        const float inMs = std::min(110.0f, span * 0.20f);
        const float outMs = std::min(span * 0.40f, 620.0f);
        float envelope = 1.0f;
        if (elapsed < inMs) {
            const float q = Clamp01(elapsed / inMs);
            envelope = q * q * (3.0f - 2.0f * q);
        } else if (elapsed > span - outMs) {
            const float q = Clamp01((elapsed - span + outMs) / outMs);
            envelope = 1.0f - q * q * (3.0f - 2.0f * q);
        }
        const float gain = envelope * Clamp01(style_.opacity);

        // The columns cover the screen, so every frame is the whole monitor: it
        // is cleared and rebuilt rather than patched in strips.
        surface_.Clear();
        if (gain <= 0.0f) {
            window_.Present(surface_, monitor_.left, monitor_.top, 255);
            firstFrame_ = false;
            return;
        }

        const float seconds = elapsed * 0.001f;
        const float drift = seconds * 0.05f;
        const int rows = std::max(1, (h + cellH - 1) / cellH);
        const Color white = Rgb(255, 255, 255);
        const uint32_t glyphCount = static_cast<uint32_t>(matrixGlyphs_);

        auto drawColumn = [&](int index) {
            const MatrixColumn& column = matrixColumns_[static_cast<size_t>(index)];
            const float local = seconds - column.delay;
            if (local <= 0.0f) {
                return;
            }
            const float head = column.head + column.speed * local;
            // The trail always sits behind the head, whichever way the head is
            // running: below it on the way down, above it on the way up.
            const float dir = column.speed >= 0.0f ? 1.0f : -1.0f;
            const float tailEnd = head + (dir > 0.0f ? -column.trail : column.trail);
            const int firstRow = std::max(
                0, static_cast<int>(floorf(std::min(head, tailEnd))));
            const int lastRow = std::min(
                rows - 1, static_cast<int>(floorf(std::max(head, tailEnd))));
            if (firstRow > lastRow) {
                return;
            }
            // Rows are measured from the cell edge the head came in through, so
            // the cell it sits in is the brightest one in both directions.
            const float cellBase = head + (dir > 0.0f ? 0.0f : 1.0f);
            const Color tint = g_glow.At(fmodf(column.hue + drift, 1.0f));
            for (int row = firstRow; row <= lastRow; row++) {
                const float age = dir * (cellBase - static_cast<float>(row));
                if (age < 0.0f || age > column.trail) {
                    continue;
                }
                const float q = Clamp01(1.0f - age / column.trail);
                // The head of a stream is a bright character that flickers to a
                // new one several times a second; the trail behind it is the
                // event colour, dimming as it falls away.
                const uint32_t seed = MatrixHash(static_cast<uint32_t>(row),
                                                 static_cast<uint32_t>(index));
                uint32_t pick;
                float alpha;
                Color ink;
                if (age < 1.1f) {
                    // Rounded first: a column leaving the screen can sit a little
                    // above the top edge, and the hash takes a plain count.
                    const float tick = std::max(0.0f, head) * 2.5f;
                    pick = MatrixHash(seed, static_cast<uint32_t>(tick)) %
                           glyphCount;
                    ink = Mix(tint, white, 0.72f);
                    alpha = 0.95f;
                } else {
                    // Some cells stay dark, which is what turns a solid bar into
                    // a stream of characters.
                    if ((seed >> 24) < column.blank) {
                        continue;
                    }
                    pick = seed % glyphCount;
                    ink = Mix(tint, white, 0.12f + 0.30f * q);
                    alpha = 0.82f * powf(q, 2.1f);
                }
                const float cover = alpha * gain;
                if (cover <= 0.004f) {
                    continue;
                }

                const int top = row * cellH;
                const int bottom = std::min(h, top + cellH);
                const int right = std::min(w, column.left + cellW);
                for (int y = top; y < bottom; y++) {
                    const uint32_t* source = matrixAtlas_.Row(y - top) + pick * cellW;
                    uint32_t* target = surface_.Row(y);
                    for (int x = column.left; x < right; x++) {
                        const uint32_t coverage = source[x - column.left] >> 24;
                        if (!coverage) {
                            continue;
                        }
                        BlendOver(target[x],
                                  PackPremultiplied(
                                      ink, cover * static_cast<float>(coverage) / 255.0f));
                    }
                }
            }
        };

        // One column per unit of work: they never touch each other's pixels.
        workers_.ForRows(0, static_cast<int>(matrixColumns_.size()), drawColumn, true, 6);
        window_.Present(surface_, monitor_.left, monitor_.top, 255);
        firstFrame_ = false;
    }

    // The bezel presses the whole desktop inward: a dark frame grows out of the
    // four edges and the screen, captured once before the window appears, is
    // scaled down to fit inside it. The depth stays small so the desktop reads as
    // nudged in rather than boxed.
    void BuildBezelProfile(int w, int h) {
        const float cap = std::max(4.0f, std::min(w, h) * 0.055f);
        bezelTarget_ = std::clamp((4.0f + style_.strength * 0.75f) * scale_, 2.0f, cap);
        // The push steps in whole pixels, so the frames in between have nothing
        // to draw; -1 means "nothing on screen yet".
        bezelDrawnInset_ = -1;
        // The table starts at the far end of the shadow, out on the picture, and
        // runs to a few pixels into the frame.
        const float inside = BezelShadowReach() * kBezelShadowTail;
        bezelLutBase_ = static_cast<int>(ceilf(inside * 4.0f)) + 4;
        bezelLut_.assign(static_cast<size_t>(bezelLutBase_ + 12), 0u);
    }

    // How far the shadow under the frame falls onto the content. The frame sits
    // well in front of the picture rather than level with it, so the shadow
    // travels a good deal further than the frame is deep.
    float BezelShadowReach() const { return std::max(3.0f, bezelTarget_ * 2.0f); }

    // The frame and its shadow are both a function of the distance to the inner
    // edge, so the whole cross section fits in one lookup table. Negative is
    // inside the pushed-in screen, positive is out on the frame.
    void UpdateBezelFalloff(float amount) {
        const float push = Clamp01(amount);
        const float fade = push * push * (3.0f - 2.0f * push);
        const float reach = BezelShadowReach();
        const float tail = reach * kBezelShadowTail;
        const float opacity = Clamp01(style_.opacity);
        for (size_t i = 0; i < bezelLut_.size(); i++) {
            const float d =
                (static_cast<float>(i) - static_cast<float>(bezelLutBase_)) * 0.25f;
            // The frame is opaque from its first pixel on: it is a bezel, not a
            // fade, and its thickness already grows out of nothing with the push.
            const float frame = Clamp01(d + 0.5f) * opacity;
            float shade = 0.0f;
            if (d < 0.0f) {
                // The picture, sitting under the frame: a dark contact line at
                // the fold, then a long soft falloff spreading in from it.
                const float contact = 0.46f * fade * expf(d / 1.7f);
                const float spread = 0.26f * fade * expf(d / reach) *
                                     Clamp01(1.0f + d / tail);
                shade = Clamp01(contact + spread * (1.0f - contact));
            }
            // The bezel body: near black, lifted a touch towards its inner edge,
            // where its lip catches the glow coming off the screen. It is plain
            // grey, so nothing coloured ever frames the picture.
            const float body = 0.05f * expf(-std::max(0.0f, d) / 4.5f);
            const float q = (d - 1.2f) / 0.75f;
            const float lip = 0.22f * fade * expf(-q * q);
            const float tone = Clamp01(body + lip) * frame;
            const float alpha = Clamp01(frame + shade * (1.0f - frame));
            const uint32_t a = static_cast<uint32_t>(alpha * 255.0f + 0.5f);
            const uint32_t grey = static_cast<uint32_t>(tone * 255.0f + 0.5f);
            bezelLut_[i] = (a << 24) | (grey << 16) | (grey << 8) | grey;
        }
    }

    // The frame holds a still picture of the desktop, so it lets go sooner than
    // the other effects instead of freezing the screen for the whole duration.
    // The cascade is a burst too: it has run through by the time the columns
    // have fallen past the bottom edge.
    double SpanMs() const {
        if (style_.effect == RippleEffect::Bezel) {
            return std::min(style_.durationMs, 2200.0);
        }
        if (style_.effect == RippleEffect::Matrix) {
            return std::min(style_.durationMs, 1800.0);
        }
        return style_.durationMs;
    }

    void RenderBezel(double now) {
        const int w = surface_.Width();
        const int h = surface_.Height();
        if (w <= 0 || h <= 0 || bezelLut_.empty()) {
            return;
        }

        const float span = static_cast<float>(SpanMs());
        const float elapsed = static_cast<float>(now - start_);
        // A quick shove, a slow settle a little deeper, a hold, then the release.
        // The settle is what gives the push its weight: the frame keeps creeping
        // in for a moment after the first hit instead of stopping dead.
        const float pressMs = std::min(210.0f, span * 0.22f);
        const float settleMs = std::min(340.0f, span * 0.24f);
        const float releaseMs = std::min(300.0f, span * 0.30f);
        auto ease = [](float x) {
            x = Clamp01(x);
            return x * x * (3.0f - 2.0f * x);
        };
        float amount;
        float settle = 0.0f;
        if (elapsed < pressMs) {
            amount = ease(elapsed / pressMs);
        } else if (elapsed < pressMs + settleMs) {
            amount = 1.0f;
            settle = ease((elapsed - pressMs) / settleMs);
        } else {
            amount = elapsed < span - releaseMs
                         ? 1.0f
                         : 1.0f - ease((elapsed - span + releaseMs) / releaseMs);
            settle = 1.0f;
        }
        // The extra travel is kept to a couple of pixels: more than that and the
        // frame reads as still sliding rather than coming to rest.
        const float depth = bezelTarget_ * amount * (1.0f + 0.10f * settle);

        // The push steps in whole pixels, so while it holds nothing moves and
        // those frames cost nothing. The dim still has to track the fade, so the
        // frame is redrawn when that level moves as well.
        const uint32_t dimKeep = DimKeep(style_.dim * dimLevel_);
        const int inset = static_cast<int>(depth + 0.5f);
        if (inset == bezelDrawnInset_ && dimKeep == bezelDrawnKeep_) {
            return;
        }
        bezelDrawnInset_ = inset;
        bezelDrawnKeep_ = dimKeep;

        // The picture is zoomed out by the same factor on both axes, so nothing
        // inside it changes shape. Insetting it by the same number of pixels on
        // all four sides would stretch it: the screen is wider than it is tall,
        // so the band comes out a little thinner at the top and the bottom.
        const float longest = static_cast<float>(std::max(w, h));
        const float zoom = 1.0f - 2.0f * static_cast<float>(inset) / longest;
        const int innerW =
            std::max(1, static_cast<int>(static_cast<float>(w) * zoom + 0.5f));
        const int innerH =
            std::max(1, static_cast<int>(static_cast<float>(h) * zoom + 0.5f));
        const int innerLeft = (w - innerW) / 2;
        const int innerTop = (h - innerH) / 2;
        const int innerRight = innerLeft + innerW;   // Exclusive.
        const int innerBottom = innerTop + innerH;   // Exclusive.
        const int lastY = h - 1;
        // Distance to the frame, measured off the four inner edges.
        const float leftEdge = static_cast<float>(innerLeft);
        const float topEdge = static_cast<float>(innerTop);
        const float rightEdge = static_cast<float>(innerRight);
        const float bottomEdge = static_cast<float>(innerBottom);

        UpdateBezelFalloff(amount);

        const uint32_t* lut = bezelLut_.data();
        const int lutCount = static_cast<int>(bezelLut_.size());
        const float base = static_cast<float>(bezelLutBase_);
        // Anything further in than this is untouched by the frame.
        const int band = std::max(innerLeft, innerTop) +
                         static_cast<int>(ceilf(BezelShadowReach() * kBezelShadowTail)) +
                         2;

        // The screen is scaled, not cropped: every captured pixel is resampled
        // into the smaller rect, so the whole picture slides inward together
        // instead of the edges vanishing under a mask. Both axes share one
        // factor, which is what keeps the picture's own shape intact.
        const Surface& shot = bezelCapture_;
        const bool pushed = shot.Valid() && shot.Width() > 0 && shot.Height() > 0;
        const OpaquePixels image{pushed ? shot.Row(0) : nullptr, shot.Width(),
                                 shot.Height(), shot.Stride()};
        const float resample =
            static_cast<float>(w) / static_cast<float>(std::max(1, innerW));
        const uint32_t dim =
            static_cast<uint32_t>(Clamp01(style_.opacity) * 255.0f + 0.5f);

        auto renderRow = [&](int y) {
            uint32_t* row = surface_.Row(y);
            const bool interior = y >= innerTop && y < innerBottom;
            if (interior && pushed) {
                if (innerLeft > 0) {
                    memset(row, 0, static_cast<size_t>(innerLeft) * 4);
                }
                if (innerRight < w) {
                    memset(row + innerRight, 0,
                           static_cast<size_t>(w - innerRight) * 4);
                }
                const float sy =
                    (static_cast<float>(y - innerTop) + 0.5f) * resample - 0.5f;
                for (int x = innerLeft; x < innerRight; x++) {
                    const float sx =
                        (static_cast<float>(x - innerLeft) + 0.5f) * resample - 0.5f;
                    uint32_t pixel = SampleOpaquePixels(image, sx, sy);
                    if (dimKeep < 255u) {
                        pixel = DimPixel(pixel, dimKeep);
                    }
                    row[x] = dim < 255u ? ScalePremultiplied(pixel, dim) : pixel;
                }
            } else {
                memset(row, 0, static_cast<size_t>(w) * 4);
            }

            const float py = static_cast<float>(y) + 0.5f;
            const int borderY = std::min(y, lastY - y);
            auto shade = [&](int from, int to) {
                for (int x = from; x < to; x++) {
                    // Straight-edged distance to the nearest of the four inner
                    // edges: negative over the pushed-in content.
                    const float px = static_cast<float>(x) + 0.5f;
                    const float d = std::max(
                        std::max(leftEdge - px, px - rightEdge),
                        std::max(topEdge - py, py - bottomEdge));
                    const int index = std::clamp(
                        static_cast<int>(d * 4.0f + base), 0, lutCount - 1);
                    BlendOver(row[x], lut[index]);
                }
            };
            if (borderY < band || band * 2 >= w) {
                // Rows this close to the edge carry the frame end to end.
                shade(0, w);
            } else {
                shade(0, band);
                shade(w - band, w);
            }
        };

        workers_.ForRows(0, h, renderRow, true, 8);
        // The content moved, so the whole window is uploaded rather than a strip
        // around the frame.
        window_.Present(surface_, monitor_.left, monitor_.top, 255);
        firstFrame_ = false;
    }

    void RenderRipple(double now) {
        const double frameStart = NowMs();
        const float t = static_cast<float>(
            std::clamp((now - start_) / style_.durationMs, 0.0, 1.0));
        const float elapsed = static_cast<float>(now - start_);
        const int w = surface_.Width();
        const int h = surface_.Height();
        if (!capture_ || rows_.size() != static_cast<size_t>(h)) {
            return;
        }

        const float travel = EaseTravel(style_.curve, t);
        const float radius = (style_.reverse ? 1.0f - travel : travel) * maxRadius_;
        // The wave front stays widest where it is furthest from the origin.
        const float spread = std::min(1.0f, radius / std::max(1.0f, maxRadius_));
        const float sigma = (40.0f + 38.0f * spread) * scale_;
        const float in = Clamp01(elapsed / 180.0f);
        const float outStart = style_.reverse ? 0.72f : 0.55f;
        const float out = Clamp01((t - outStart) / (1.0f - outStart));
        const float fadeIn = in * in * (3.0f - 2.0f * in);
        const float fadeOut = 1.0f - out * out * (3.0f - 2.0f * out);
        const float strength = style_.strength * scale_ * fadeIn * fadeOut;
        const float alpha = fadeIn * fadeOut * style_.opacity;

        uint32_t colors[64];
        for (int i = 0; i < 64; i++) {
            colors[i] = PackPremultiplied(g_glow.At(i / 64.0f + t * 0.2f), 1.0f);
        }

        const float reach = sigma * 3.8f;
        const int offset = static_cast<int>(ceilf(reach * 4.0f));
        wave_.resize(static_cast<size_t>(offset) * 2 + 2);
        size_t firstWave = wave_.size();
        size_t lastWave = 0;
        for (size_t i = 0; i < wave_.size(); i++) {
            const RippleWave wave = EvaluateRippleWave((static_cast<int>(i) - offset) / 4.0f,
                                                        sigma, strength);
            RasterWave& entry = wave_[i];
            entry.displacement = wave.displacement;
            entry.opacity = static_cast<uint32_t>(wave.opacity * alpha * 255.0f + 0.5f);
            entry.lightAlpha = static_cast<uint32_t>(wave.light * alpha * 255.0f + 0.5f);
            entry.light = g_glow.rainbow ? 0 : ScalePremultiplied(colors[0], entry.lightAlpha);
            if (entry.lightAlpha > 0 ||
                (entry.opacity > 1 &&
                 fabsf(entry.displacement) * entry.opacity >= 0.03f * 255.0f)) {
                firstWave = std::min(firstWave, i);
                lastWave = i;
            }
        }

        float inner = 0;
        float outer = 0;
        int firstY = 0;
        int lastY = 0;
        if (firstWave <= lastWave) {
            inner = std::max(0.0f, radius + (static_cast<int>(firstWave) - offset) * 0.25f - 1.0f);
            outer = radius + (static_cast<int>(lastWave) + 1 - offset) * 0.25f + 1.0f;
            const int capturedTop = capture_->top.load(std::memory_order_acquire);
            const int capturedBottom = capture_->bottom.load(std::memory_order_acquire);
            const int margin = CaptureMargin();
            firstY = std::max(capturedTop > 0 ? capturedTop + margin : 0,
                              static_cast<int>(floorf(center_.y - outer)));
            lastY = std::min(capturedBottom < h ? capturedBottom - margin : h,
                             static_cast<int>(ceilf(center_.y + outer)));
        }
        const bool drawing = firstY < lastY;
        const float cx = center_.x;
        const float cy = center_.y;
        int firstX = 0;
        int lastX = 0;
        if (drawing) {
            firstX = std::max(0, static_cast<int>(floorf(cx - outer)));
            lastX = std::min(w, static_cast<int>(ceilf(cx + outer)) + 1);
        }

        const int step = step_;
        int rowsFirst = firstY;
        int rowsLast = lastY;
        int changedX0 = firstX;
        int changedX1 = lastX;
        if (drawnFirst_ < drawnLast_) {
            rowsFirst = drawing ? std::min(rowsFirst, drawnFirst_) : drawnFirst_;
            rowsLast = drawing ? std::max(rowsLast, drawnLast_) : drawnLast_;
            changedX0 = drawing ? std::min(changedX0, drawnX0_) : drawnX0_;
            changedX1 = drawing ? std::max(changedX1, drawnX1_) : drawnX1_;
        }
        drawnFirst_ = drawing ? firstY / step * step : 0;
        drawnLast_ = drawing ? std::min(h, (lastY + step - 1) / step * step) : 0;
        drawnX0_ = firstX;
        drawnX1_ = lastX;

        const Surface& snapshotSurface = capture_->snapshot;
        const OpaquePixels snapshot{snapshotSurface.Valid() ? snapshotSurface.Row(0) : nullptr,
                                    snapshotSurface.Width(), snapshotSurface.Height(),
                                    snapshotSurface.Stride()};
        const RasterWave* waves = wave_.data();
        const int waveCount = static_cast<int>(wave_.size());
        const float edgeWidth = 24.0f * scale_;
        const bool rainbow = g_glow.rainbow;
        // The wave paints the captured desktop back itself, so the scrim behind
        // is not enough: this picture carries the same dim, at the same fade.
        const uint32_t dimKeep = DimKeep(style_.dim * dimLevel_);

        auto shade = [&](float x, float y) -> uint32_t {
            const float dx = x - cx;
            const float dy = y - cy;
            const float distance = sqrtf(dx * dx + dy * dy);
            const int index = static_cast<int>((distance - radius) * 4.0f) + offset;
            if (index < 0 || index >= waveCount) {
                return 0;
            }
            const RasterWave& wave = waves[index];
            uint32_t pixel = 0;
            if (snapshot.pixels && wave.opacity > 1) {
                float edge = Clamp01(std::min(y, h - 1 - y) / edgeWidth);
                if (x < edgeWidth || w - 1 - x < edgeWidth) {
                    edge = std::min(edge, Clamp01(std::min(x, w - 1 - x) / edgeWidth));
                }
                const float displacement = wave.displacement * edge * edge *
                    (3.0f - 2.0f * edge) / std::max(distance, 1.0f);
                pixel = SampleOpaquePixels(snapshot, x + dx * displacement, y + dy * displacement);
                if (dimKeep < 255) {
                    pixel = DimPixel(pixel, dimKeep);
                }
                if (wave.opacity < 255) {
                    pixel = ScalePremultiplied(pixel, wave.opacity);
                }
            }
            if (wave.lightAlpha) {
                uint32_t light = wave.light;
                if (rainbow) {
                    const float phase = FastAtan2(dy, dx) / (2.0f * kPi);
                    light = ScalePremultiplied(
                        colors[static_cast<int>((phase - floorf(phase)) * 64.0f) & 63],
                        wave.lightAlpha);
                }
                BlendOver(pixel, light);
            }
            return pixel;
        };

        auto renderBlock = [&](int block) {
            const int y0 = block * step;
            const int y1 = std::min(h, y0 + step);
            RowSpans spans[4];
            RowSpans leader;
            bool any = false;
            for (int y = y0; y < y1; y++) {
                ClearRow(y);
                const float dy = y - cy;
                if (y < firstY || y >= lastY || fabsf(dy) >= outer) {
                    continue;
                }
                const float xo = sqrtf(std::max(0.0f, outer * outer - dy * dy));
                const float xi = fabsf(dy) < inner
                                     ? sqrtf(std::max(0.0f, inner * inner - dy * dy)) : 0.0f;
                const float edges[2][2] = {{cx - xo, cx - xi}, {cx + xi, cx + xo}};
                for (int i = 0; i < 2; i++) {
                    const int begin = std::max(0, static_cast<int>(ceilf(edges[i][0])));
                    const int end = std::min(w, static_cast<int>(ceilf(edges[i][1])));
                    if (end <= begin) {
                        continue;
                    }
                    spans[y - y0].begin[i] = begin;
                    spans[y - y0].end[i] = end;
                    if (leader.end[i] <= leader.begin[i]) {
                        leader.begin[i] = begin;
                        leader.end[i] = end;
                    } else {
                        leader.begin[i] = std::min(leader.begin[i], begin);
                        leader.end[i] = std::max(leader.end[i], end);
                    }
                    any = true;
                }
            }
            if (!any) {
                return;
            }
            uint32_t* leaderRow = surface_.Row(y0);
            const float blockCenter = (step - 1) * 0.5f;
            for (int i = 0; i < 2; i++) {
                for (int bx = leader.begin[i] / step * step; bx < leader.end[i]; bx += step) {
                    const uint32_t pixel = shade(bx + blockCenter, y0 + blockCenter);
                    if (!pixel) {
                        continue;
                    }
                    const int to = std::min(bx + step, leader.end[i]);
                    for (int x = std::max(bx, leader.begin[i]); x < to; x++) {
                        leaderRow[x] = pixel;
                    }
                }
            }
            for (int y = y0 + 1; y < y1; y++) {
                uint32_t* row = surface_.Row(y);
                for (int i = 0; i < 2; i++) {
                    const int begin = spans[y - y0].begin[i];
                    const int end = spans[y - y0].end[i];
                    if (end > begin) {
                        memcpy(row + begin, leaderRow + begin,
                               static_cast<size_t>(end - begin) * sizeof(uint32_t));
                    }
                }
                rows_[y] = spans[y - y0];
            }
            rows_[y0] = leader;
        };

        workers_.ForRows(rowsFirst / step, (rowsLast + step - 1) / step, renderBlock,
                         static_cast<uint64_t>(w) * h >= 500000, std::max(1, 16 / step));
        const RECT changed{changedX0, rowsFirst / step * step, changedX1,
                           std::min(h, (rowsLast + step - 1) / step * step)};
        if (firstFrame_) {
            window_.Present(surface_, monitor_.left, monitor_.top, 255);
            firstFrame_ = false;
        } else if (changed.right > changed.left && changed.bottom > changed.top) {
            window_.Present(surface_, monitor_.left, monitor_.top, 255, &changed);
        }
        AdaptQuality(NowMs() - frameStart);
    }

    // The same refraction the ring wave uses, laid out along a straight front.
    // The fronts cross the whole monitor in the direction that leads from the
    // origin toward the middle of the screen, so an origin along the bottom
    // washes the desktop upward and one in a corner sends it across at an
    // angle. An origin at the middle has no direction to give, so the fronts
    // part there and run left and right.
    void BuildLinearProfile(int w, int h) {
        const float dirX = static_cast<float>(w) * 0.5f - center_.x;
        const float dirY = static_cast<float>(h) * 0.5f - center_.y;
        const float length = sqrtf(dirX * dirX + dirY * dirY);
        // Anything within a couple of percent of the middle counts as the
        // middle. The anchors are whole pixels, so a centre origin lands half a
        // pixel off and would otherwise hand out a diagonal to follow.
        if (length < std::max(4.0f, std::min(w, h) * 0.02f)) {
            linearX_ = 1.0f;
            linearY_ = 0.0f;
        } else {
            linearX_ = dirX / length;
            linearY_ = dirY / length;
        }
        // How far the furthest corner sits from the origin along that line, so
        // both fronts can leave the screen instead of stopping at the last one.
        float reach = 0.0f;
        for (const POINT corner : {POINT{0, 0}, POINT{w, 0}, POINT{0, h}, POINT{w, h}}) {
            const float along =
                (static_cast<float>(corner.x) - center_.x) * linearX_ +
                (static_cast<float>(corner.y) - center_.y) * linearY_;
            reach = std::max(reach, fabsf(along));
        }
        linearReach_ = reach + 100.0f * scale_;
    }

    void RenderLinearWave(double now) {
        const double frameStart = NowMs();
        const float t = static_cast<float>(
            std::clamp((now - start_) / style_.durationMs, 0.0, 1.0));
        const float elapsed = static_cast<float>(now - start_);
        const int w = surface_.Width();
        const int h = surface_.Height();
        if (!capture_ || rows_.size() != static_cast<size_t>(h)) {
            return;
        }

        const float travel = EaseTravel(style_.curve, t);
        // Both fronts leave the origin together, one each way along the line, so
        // the wave parts the picture at the origin instead of shoving it in from
        // one edge. On an unplug they meet at the origin again.
        const float front = (style_.reverse ? 1.0f - travel : travel) * linearReach_;
        // The band opens up as the front crosses, the way the ring's widens as
        // it grows away from the origin.
        const float spread = style_.reverse ? 1.0f - travel : travel;
        const float sigma = (40.0f + 38.0f * spread) * scale_;
        const float in = Clamp01(elapsed / 180.0f);
        const float outStart = style_.reverse ? 0.72f : 0.55f;
        const float out = Clamp01((t - outStart) / (1.0f - outStart));
        const float fadeIn = in * in * (3.0f - 2.0f * in);
        const float fadeOut = 1.0f - out * out * (3.0f - 2.0f * out);
        const float strength = style_.strength * scale_ * fadeIn * fadeOut;
        const float alpha = fadeIn * fadeOut * style_.opacity;

        uint32_t colors[64];
        for (int i = 0; i < 64; i++) {
            colors[i] = PackPremultiplied(g_glow.At(i / 64.0f + t * 0.2f), 1.0f);
        }

        // The cross section is a straight line's, so it is sampled by distance
        // along the direction of travel instead of by radius.
        const float reach = sigma * 3.8f;
        const int offset = static_cast<int>(ceilf(reach * 4.0f));
        wave_.resize(static_cast<size_t>(offset) * 2 + 2);
        for (size_t i = 0; i < wave_.size(); i++) {
            const RippleWave wave = EvaluateRippleWave(
                (static_cast<int>(i) - offset) / 4.0f, sigma, strength);
            RasterWave& entry = wave_[i];
            entry.displacement = wave.displacement;
            entry.opacity =
                static_cast<uint32_t>(wave.opacity * alpha * 255.0f + 0.5f);
            entry.lightAlpha =
                static_cast<uint32_t>(wave.light * alpha * 255.0f + 0.5f);
            entry.light = g_glow.rainbow
                              ? 0
                              : ScalePremultiplied(colors[0], entry.lightAlpha);
        }

        // The band holds everything whose distance from the origin along the
        // line falls between `front - reach` and `front + reach`, on either
        // side of it. Until the fronts are further out than the band is wide,
        // the two halves are still joined across the origin.
        const float outer = front + reach;
        const float inner = std::max(0.0f, front - reach);
        const float dirX = linearX_;
        const float dirY = linearY_;
        const float ox = center_.x;
        const float oy = center_.y;
        const bool level = fabsf(dirX) < 1.0e-4f;

        // The run of one row whose projection falls between two values. A
        // straight front always cuts a row in a single run.
        auto solve = [&](float base, float from, float to, int* begin,
                         int* end) -> bool {
            if (level) {
                if (base < from || base > to) {
                    return false;
                }
                *begin = 0;
                *end = w;
                return true;
            }
            const float low =
                std::min((from - base) / dirX, (to - base) / dirX) - 0.5f;
            const float high =
                std::max((from - base) / dirX, (to - base) / dirX) - 0.5f;
            // A line that is almost vertical divides by almost nothing, so the
            // ends are pulled back into range before they become ints.
            const int first =
                std::max(0, static_cast<int>(ceilf(std::max(-1.0f, low))));
            const int last = std::min(
                w, static_cast<int>(floorf(std::min(static_cast<float>(w), high))) + 1);
            if (last <= first) {
                return false;
            }
            *begin = first;
            *end = last;
            return true;
        };

        // Where a row's pixel centres land inside the band. Two fronts can put
        // two runs on the same row, which is what the span pair is for.
        auto bandOf = [&](int y, RowSpans* spans) {
            *spans = RowSpans{};
            const float base =
                (static_cast<float>(y) + 0.5f - oy) * dirY - ox * dirX;
            if (inner > 0.0f) {
                solve(base, inner, outer, &spans->begin[0], &spans->end[0]);
                solve(base, -outer, -inner, &spans->begin[1], &spans->end[1]);
            } else {
                solve(base, -outer, outer, &spans->begin[0], &spans->end[0]);
            }
        };

        // The run a whole row is handled over: whichever of the two fronts'
        // runs are there, joined up. Both empty gives an empty run back.
        auto joinSpans = [&](const RowSpans& spans, int* begin, int* end) {
            *begin = w;
            *end = 0;
            for (int i = 0; i < 2; i++) {
                if (spans.end[i] > spans.begin[i]) {
                    *begin = std::min(*begin, spans.begin[i]);
                    *end = std::max(*end, spans.end[i]);
                }
            }
        };

        int firstY = h;
        int lastY = 0;
        int bandX0 = w;
        int bandX1 = 0;
        for (int y = 0; y < h; y++) {
            RowSpans spans;
            bandOf(y, &spans);
            int begin = 0;
            int end = 0;
            joinSpans(spans, &begin, &end);
            if (end <= begin) {
                continue;
            }
            firstY = std::min(firstY, y);
            lastY = y + 1;
            bandX0 = std::min(bandX0, begin);
            bandX1 = std::max(bandX1, end);
        }
        if (lastY <= firstY) {
            firstY = 0;
            bandX0 = 0;
            bandX1 = 0;
        }

        // Rows the capture has not reached yet stay clear, so the front never
        // pushes a picture that is still half black.
        const int capturedTop = capture_->top.load(std::memory_order_acquire);
        const int capturedBottom = capture_->bottom.load(std::memory_order_acquire);
        const int margin = CaptureMargin();
        if (capturedTop > 0) {
            firstY = std::max(firstY, capturedTop + margin);
        }
        if (capturedBottom < h) {
            lastY = std::min(lastY, capturedBottom - margin);
        }

        // A front waiting on the capture is the same as no front at all, so the
        // range collapses and only last frame's rows are cleared this time.
        const bool drawing = firstY < lastY;
        if (!drawing) {
            firstY = 0;
            lastY = 0;
            bandX0 = 0;
            bandX1 = 0;
        }
        const int step = step_;
        int rowsFirst = firstY;
        int rowsLast = lastY;
        int changedX0 = bandX0;
        int changedX1 = bandX1;
        if (drawnFirst_ < drawnLast_) {
            rowsFirst = drawing ? std::min(rowsFirst, drawnFirst_) : drawnFirst_;
            rowsLast = drawing ? std::max(rowsLast, drawnLast_) : drawnLast_;
            changedX0 = drawing ? std::min(changedX0, drawnX0_) : drawnX0_;
            changedX1 = drawing ? std::max(changedX1, drawnX1_) : drawnX1_;
        }
        drawnFirst_ = drawing ? firstY / step * step : 0;
        drawnLast_ = drawing ? std::min(h, (lastY + step - 1) / step * step) : 0;
        drawnX0_ = bandX0;
        drawnX1_ = bandX1;

        const Surface& snapshotSurface = capture_->snapshot;
        const OpaquePixels snapshot{snapshotSurface.Valid() ? snapshotSurface.Row(0) : nullptr,
                                    snapshotSurface.Width(), snapshotSurface.Height(),
                                    snapshotSurface.Stride()};
        const RasterWave* waves = wave_.data();
        const int waveCount = static_cast<int>(wave_.size());
        const float edgeWidth = 24.0f * scale_;
        const bool rainbow = g_glow.rainbow;
        // The wave paints the captured desktop back itself, so the scrim behind
        // is not enough: this picture carries the same dim, at the same fade.
        const uint32_t dimKeep = DimKeep(style_.dim * dimLevel_);
        // The gradient runs along the front, so its phase is the distance
        // sideways from the origin rather than an angle around it.
        const float phaseSpan = std::max(1.0f, w * fabsf(dirY) + h * fabsf(dirX));

        auto shade = [&](int x, int y) -> uint32_t {
            const float px = static_cast<float>(x) + 0.5f;
            const float py = static_cast<float>(y) + 0.5f;
            const float along = (px - ox) * dirX + (py - oy) * dirY;
            const int index =
                static_cast<int>((fabsf(along) - front) * 4.0f) + offset;
            if (index < 0 || index >= waveCount) {
                return 0;
            }
            const RasterWave& wave = waves[index];
            uint32_t pixel = 0;
            if (snapshot.pixels && wave.opacity > 1) {
                float edge = Clamp01(std::min(y, h - 1 - y) / edgeWidth);
                if (x < edgeWidth || w - 1 - x < edgeWidth) {
                    edge = std::min(edge, Clamp01(std::min(x, w - 1 - x) / edgeWidth));
                }
                edge = edge * edge * (3.0f - 2.0f * edge);
                // Each half of the picture is pushed along its own front, away
                // from the origin plane.
                const float side = along < 0.0f ? -1.0f : 1.0f;
                const float push = wave.displacement * edge * side;
                pixel = SampleOpaquePixels(snapshot, static_cast<float>(x) + dirX * push,
                                           static_cast<float>(y) + dirY * push);
                if (dimKeep < 255) {
                    pixel = DimPixel(pixel, dimKeep);
                }
                if (wave.opacity < 255) {
                    pixel = ScalePremultiplied(pixel, wave.opacity);
                }
            }
            if (wave.lightAlpha) {
                uint32_t light = wave.light;
                if (rainbow) {
                    float phase = ((px - ox) * -dirY + (py - oy) * dirX) / phaseSpan;
                    phase += t * 0.2f;
                    phase -= floorf(phase);
                    light = ScalePremultiplied(
                        colors[static_cast<int>(phase * 64.0f) & 63], wave.lightAlpha);
                }
                BlendOver(pixel, light);
            }
            return pixel;
        };

        auto renderBlock = [&](int block) {
            const int y0 = block * step;
            const int y1 = std::min(h, y0 + step);
            RowSpans spans[4];
            int leaderBegin = w;
            int leaderEnd = 0;
            int leader = -1;
            for (int y = y0; y < y1; y++) {
                ClearRow(y);
                if (y < firstY || y >= lastY) {
                    continue;
                }
                RowSpans& row = spans[y - y0];
                bandOf(y, &row);
                int begin = 0;
                int end = 0;
                joinSpans(row, &begin, &end);
                if (end <= begin) {
                    continue;
                }
                leaderBegin = std::min(leaderBegin, begin);
                leaderEnd = std::max(leaderEnd, end);
                if (leader < 0) {
                    leader = y;
                }
            }
            if (leader < 0) {
                return;
            }
            // One row inside the band is shaded for the whole block and the
            // rest copy it, which is how the ring wave keeps its cost down too.
            uint32_t* leaderRow = surface_.Row(leader);
            for (int x = leaderBegin; x < leaderEnd; x++) {
                const uint32_t pixel = shade(x, leader);
                if (pixel) {
                    leaderRow[x] = pixel;
                }
            }
            for (int y = y0; y < y1; y++) {
                if (y == leader) {
                    continue;
                }
                uint32_t* row = surface_.Row(y);
                for (int i = 0; i < 2; i++) {
                    const int begin = spans[y - y0].begin[i];
                    const int end = spans[y - y0].end[i];
                    if (end > begin) {
                        memcpy(row + begin, leaderRow + begin,
                               static_cast<size_t>(end - begin) * sizeof(uint32_t));
                    }
                }
                rows_[y] = spans[y - y0];
            }
            rows_[leader] = spans[leader - y0];
        };

        workers_.ForRows(rowsFirst / step, (rowsLast + step - 1) / step, renderBlock,
                         static_cast<uint64_t>(w) * h >= 500000, std::max(1, 16 / step));
        const RECT changed{changedX0, rowsFirst / step * step, changedX1,
                           std::min(h, (rowsLast + step - 1) / step * step)};
        if (firstFrame_) {
            window_.Present(surface_, monitor_.left, monitor_.top, 255);
            firstFrame_ = false;
        } else if (changed.right > changed.left && changed.bottom > changed.top) {
            window_.Present(surface_, monitor_.left, monitor_.top, 255, &changed);
        }
        AdaptQuality(NowMs() - frameStart);
    }

    LayeredWindow window_;
    Surface surface_;
    // The scrim that darkens the screen behind the effect, and how far it is
    // faded in right now.
    ScrimWindow dimWindow_;
    float dimLevel_ = 0.0f;
    std::shared_ptr<RippleCapture> capture_;
    RECT monitor_{};
    Vec2 center_{};
    float scale_ = 1.0f;
    float maxRadius_ = 0;
    // Linear wave: the unit direction its fronts travel in and how far the
    // screen reaches either side of the origin along it.
    float linearX_ = 1.0f;
    float linearY_ = 0.0f;
    float linearReach_ = 0.0f;
    double start_ = 0;
    bool active_ = false;
    std::vector<RasterWave> wave_;
    std::vector<RowSpans> rows_;
    // Edge glow: the distance falloff, built once per animation.
    std::vector<float> glowLut_;
    float band_ = 0;
    int bandPx_ = 0;
    // Where on the perimeter the travelling highlight comes in, 0 to 1.
    float glowStart_ = 0.0f;
    // Dark bezel: the snapshot that gets scaled into the smaller rect, how deep
    // the push goes, and the cross section of frame, shadow and fold by distance
    // from the inner edge.
    Surface bezelCapture_;
    float bezelTarget_ = 0;
    int bezelDrawnInset_ = -1;
    // How far the frame's shadow spreads, as a multiple of its own depth.
    static constexpr float kBezelShadowTail = 2.6f;
    // The dim the picture on screen was drawn with, so a fade step that lands
    // while the push is holding still redraws the frame.
    uint32_t bezelDrawnKeep_ = 255;
    int bezelLutBase_ = 0;
    std::vector<uint32_t> bezelLut_;
    // Circuit trace: the runs of copper, the bands they occupy, the cross
    // section the line and its glow are drawn from, and the coverage, heat and
    // tint scratch a band is collected into before it is blended once.
    std::vector<CircuitTrace> circuitTraces_;
    std::vector<RECT> circuitBands_;
    std::vector<uint8_t> circuitFalloff_;
    std::vector<uint8_t> circuitCov_;
    std::vector<uint8_t> circuitHeat_;
    std::vector<uint8_t> circuitTint_;
    int circuitLayerW_ = 0;
    int circuitLayerH_ = 0;
    static constexpr float kCircuitLutScale = 4.0f;
    float circuitThickness_ = 1.2f;
    float circuitReach_ = 7.0f;
    // Matrix code: the rasterised atlas of characters, the cell size it was cut
    // at, and the columns falling through the grid.
    Surface matrixAtlas_;
    int matrixCellW_ = 0;
    int matrixCellH_ = 0;
    int matrixGlyphs_ = 0;
    std::vector<MatrixColumn> matrixColumns_;
    int drawnFirst_ = 0;
    int drawnLast_ = 0;
    int drawnX0_ = 0;
    int drawnX1_ = 0;
    bool firstFrame_ = true;
    int step_ = 1;
    double averageMs_ = -1;
    double frameMs_ = 1000.0 / 60.0;
    RippleStyle style_;
    RasterWorkers workers_;
};

Ripple g_ripple;

////////////////////////////////////////////////////////////////////////////////
// Charging Stats Overlay

struct ChargingStats {
    bool onAc = false;
    bool charging = false;
    bool noBattery = false;
    int percent = -1;
    int minutesToFull = 0;
    int minutesLeft = 0;
    int rateMw = 0;
};

struct OverlayStyle {
    std::wstring theme;
    std::wstring position;
    std::wstring color;
    // The free-form color the "custom" choice reads, same formats as the effect
    // colors.
    std::wstring customColor;
    bool details = true;
    // The current event follows the ripple color and that ripple is the rolling
    // gradient, so the card paints the gradient itself.
    bool rainbow = false;
    int durationMs = 4000;
    int opacity = 100;
    int fontPx = 20;
};

bool KeyIs(const std::wstring& value, PCWSTR key) {
    std::wstring text = Trim(value);
    for (wchar_t& c : text) {
        c = towlower(c);
    }
    return text == key;
}

std::wstring FormatMinutes(int minutes) {
    if (minutes <= 0) {
        return std::wstring();
    }
    const int hours = minutes / 60;
    const int rest = minutes % 60;
    if (hours <= 0) {
        return std::to_wstring(rest) + L"m";
    }
    if (rest == 0) {
        return std::to_wstring(hours) + L"h";
    }
    return std::to_wstring(hours) + L"h " + std::to_wstring(rest) + L"m";
}

// The documented layout of SYSTEM_BATTERY_STATE, filled by powrprof. Reading it
// through the exported function keeps the mod independent of SDK headers.
struct BatteryStateInfo {
    BOOLEAN acOnLine;
    BOOLEAN batteryPresent;
    BOOLEAN charging;
    BOOLEAN discharging;
    BOOLEAN spare[3];
    ULONG maxCapacity;
    ULONG remainingCapacity;
    ULONG rate;
    ULONG estimatedTime;
    ULONG defaultAlert1;
    ULONG defaultAlert2;
};

ChargingStats ReadChargingStats() {
    ChargingStats stats;
    SYSTEM_POWER_STATUS status{};
    if (GetSystemPowerStatus(&status)) {
        stats.onAc = status.ACLineStatus == 1;
        stats.charging = (status.BatteryFlag & BATTERY_FLAG_CHARGING) != 0;
        stats.noBattery = (status.BatteryFlag & BATTERY_FLAG_NO_BATTERY) != 0;
        if (!stats.noBattery && status.BatteryLifePercent <= 100) {
            stats.percent = status.BatteryLifePercent;
        }
        if (!stats.onAc && status.BatteryLifeTime != 0xFFFFFFFFu &&
            status.BatteryLifeTime >= 60) {
            stats.minutesLeft = static_cast<int>(status.BatteryLifeTime / 60);
        }
    }

    using CallNtPowerInformation_t = LONG(WINAPI*)(int, PVOID, ULONG, PVOID, ULONG);
    static auto callNtPowerInformation =
        GetProc<CallNtPowerInformation_t>(L"powrprof.dll", "CallNtPowerInformation");
    BatteryStateInfo state{};
    constexpr int kSystemBatteryState = 5;
    if (callNtPowerInformation &&
        callNtPowerInformation(kSystemBatteryState, nullptr, 0, &state,
                               sizeof(state)) == 0) {
        stats.charging = stats.charging || state.charging != FALSE;
        // The rate is typed unsigned but carries a sign: it goes negative while
        // the battery drains, and (ULONG)-1 means the machine cannot tell.
        const bool rateKnown = state.rate != 0 && state.rate != 0xFFFFFFFFu;
        const int rateMw = static_cast<int>(state.rate);
        stats.rateMw = rateMw;
        if (state.maxCapacity > state.remainingCapacity && rateKnown && rateMw > 0) {
            const double hours = (state.maxCapacity - state.remainingCapacity) /
                                 static_cast<double>(rateMw);
            stats.minutesToFull = std::max(1, static_cast<int>(hours * 60.0 + 0.5));
        }
        if (state.discharging && rateKnown && state.remainingCapacity > 0) {
            // Draining reports a negative rate, and the estimate wants its size.
            const int draw = rateMw < 0 ? -rateMw : rateMw;
            const double hours = state.remainingCapacity / static_cast<double>(draw);
            const int minutes = static_cast<int>(hours * 60.0 + 0.5);
            if (minutes > 0) {
                stats.minutesLeft = minutes;
            }
        }
    }
    return stats;
}

std::wstring BuildDetailLine(const ChargingStats& stats) {
    if (stats.noBattery || stats.percent < 0) {
        return stats.onAc ? L"On AC Power" : L"On Battery";
    }
    std::wstring line;
    if (stats.charging) {
        line = L"Charging";
        const std::wstring eta = FormatMinutes(stats.minutesToFull);
        if (!eta.empty()) {
            line += L" \u00B7 " + eta + L" to full";
        } else if (stats.percent >= 100) {
            line = L"Fully Charged";
        }
        const int watts = (stats.rateMw + 500) / 1000;
        if (watts > 0) {
            line += L" \u00B7 " + std::to_wstring(watts) + L" W";
        }
    } else if (stats.onAc) {
        line = stats.percent >= 100 ? L"Fully Charged" : L"Plugged In";
    } else {
        line = L"On Battery";
        const std::wstring left = FormatMinutes(stats.minutesLeft);
        if (!left.empty()) {
            line += L" \u00B7 " + left + L" left";
        }
    }
    return line;
}

// The card sizes itself to its text and then centres on the anchor, so a status
// line that grows after the event drags the whole layout sideways with it: the
// estimate and the wattage only turn up a moment after the cable goes in, on the
// first one-second refresh. Measuring against the widest line the current state
// can produce keeps the card the same size for as long as it is on screen.
std::wstring ReserveDetailLine(const ChargingStats& stats) {
    if (stats.noBattery || stats.percent < 0) {
        return BuildDetailLine(stats);
    }
    ChargingStats widest = stats;
    if (widest.onAc) {
        // Even a "Plugged In" card reserves the charging line, because that is
        // what it becomes as soon as the battery reports a rate.
        widest.charging = true;
        widest.minutesToFull = std::max(widest.minutesToFull, 119);
        widest.rateMw = std::max(widest.rateMw, 999000);
    } else {
        widest.minutesLeft = std::max(widest.minutesLeft, 119);
    }
    return BuildDetailLine(widest);
}

// The flat accent used by the bits that cannot take a gradient, such as the
// panel tint and the glyph. When the card follows a gradient ripple its shapes
// sample that gradient directly instead of using this sample.
Color ResolveOverlayAccent(const std::wstring& key,
                           const std::wstring& custom) {
    if (key.empty() || KeyIs(key, L"ripple")) {
        // For a gradient this is the start of the cycle, which keeps the tint
        // of the panel and the glyph on the cool side instead of a random hue.
        return g_glow.At(0.0f);
    }
    if (KeyIs(key, L"custom")) {
        // The free-form field next to the list, falling back to the event color
        // so a typo never leaves the panel untinted.
        const std::wstring hex = Trim(custom);
        return hex.empty() ? g_glow.color
                           : ParseGlowColor(hex, g_glow.color).color;
    }
    return ParseGlowColor(key, g_glow.color).color;
}

// Fluent drawing primitives. Every shape is rasterised from the rounded-rect SDF
// declared above, so the panel, its hairline and the icon share one coverage
// model, and colours are premultiplied before blending.

// Deterministic grain, so the panel reads like an acrylic backdrop.
float OverlayNoise(int x, int y) {
    uint32_t h = static_cast<uint32_t>(x) * 374761393u +
                 static_cast<uint32_t>(y) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return static_cast<float>(h & 0xFFu) / 127.5f - 1.0f;
}

// Vertical gradient fill; passing the same colour twice gives a flat fill. A
// rainbow palette replaces the colour ramp with the rolling gradient, sampled
// across the shape from left to right while the alpha ramp stays as it is.
void FillRounded(Surface& surface, const RoundedRect& rect, const Color& top,
                 float topAlpha, const Color& bottom, float bottomAlpha,
                 float grain, const GlowPalette* rainbow = nullptr) {
    if (rect.x1 <= rect.x0 || rect.y1 <= rect.y0) {
        return;
    }
    const float span = std::max(1.0f, rect.y1 - rect.y0);
    const float spanX = std::max(1.0f, rect.x1 - rect.x0);
    const int firstY = std::max(0, static_cast<int>(floorf(rect.y0)) - 1);
    const int lastY = std::min(surface.Height(), static_cast<int>(ceilf(rect.y1)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(rect.x0)) - 1);
    const int lastX = std::min(surface.Width(), static_cast<int>(ceilf(rect.x1)) + 1);
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        const float py = static_cast<float>(y) + 0.5f;
        const float v = Clamp01((py - rect.y0) / span);
        const Color base = Mix(top, bottom, v);
        const float alpha = topAlpha + (bottomAlpha - topAlpha) * v;
        for (int x = firstX; x < lastX; x++) {
            const float coverage =
                Clamp01(0.5f - RoundedRectSdf(static_cast<float>(x) + 0.5f, py, rect));
            if (coverage <= 0.0f) {
                continue;
            }
            Color pixel = base;
            if (rainbow) {
                const float u =
                    Clamp01((static_cast<float>(x) + 0.5f - rect.x0) / spanX);
                pixel = rainbow->At(u);
            }
            if (grain > 0.0f) {
                const float n = OverlayNoise(x, y) * grain;
                pixel.r = Clamp01(pixel.r + n);
                pixel.g = Clamp01(pixel.g + n);
                pixel.b = Clamp01(pixel.b + n);
            }
            BlendOver(row[x], PackPremultiplied(pixel, coverage * alpha));
        }
    }
}

// Ring stroke, drawn as the difference of two rounded-rect coverages.
void StrokeRounded(Surface& surface, const RoundedRect& rect, float thickness,
                   const Color& top, float topAlpha, const Color& bottom,
                   float bottomAlpha) {
    if (rect.x1 <= rect.x0 || rect.y1 <= rect.y0 || thickness <= 0.0f) {
        return;
    }
    const float span = std::max(1.0f, rect.y1 - rect.y0);
    const int firstY = std::max(0, static_cast<int>(floorf(rect.y0)) - 1);
    const int lastY = std::min(surface.Height(), static_cast<int>(ceilf(rect.y1)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(rect.x0)) - 1);
    const int lastX = std::min(surface.Width(), static_cast<int>(ceilf(rect.x1)) + 1);
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        const float py = static_cast<float>(y) + 0.5f;
        const float v = Clamp01((py - rect.y0) / span);
        const Color color = Mix(top, bottom, v);
        const float alpha = topAlpha + (bottomAlpha - topAlpha) * v;
        for (int x = firstX; x < lastX; x++) {
            const float distance =
                RoundedRectSdf(static_cast<float>(x) + 0.5f, py, rect);
            const float coverage = Clamp01(0.5f - distance) -
                                   Clamp01(0.5f - (distance + thickness));
            if (coverage <= 0.0f) {
                continue;
            }
            BlendOver(row[x], PackPremultiplied(color, coverage * alpha));
        }
    }
}

// Ambient drop shadow: a gaussian falloff of the distance from the card.
void CastShadow(Surface& surface, const RoundedRect& rect, float sigma,
                float offsetY, float alpha) {
    const float reach = sigma * 3.0f;
    RoundedRect shifted = rect;
    shifted.y0 += offsetY;
    shifted.y1 += offsetY;
    const int firstY = std::max(0, static_cast<int>(floorf(shifted.y0 - reach)));
    const int lastY = std::min(surface.Height(), static_cast<int>(ceilf(shifted.y1 + reach)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(shifted.x0 - reach)));
    const int lastX = std::min(surface.Width(), static_cast<int>(ceilf(shifted.x1 + reach)) + 1);
    const float falloffScale = 1.0f / (2.0f * sigma * sigma);
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        const float py = static_cast<float>(y) + 0.5f;
        for (int x = firstX; x < lastX; x++) {
            const float distance =
                RoundedRectSdf(static_cast<float>(x) + 0.5f, py, shifted);
            if (distance <= 0.0f || distance >= reach) {
                continue;
            }
            const float falloff = std::exp(-(distance * distance) * falloffScale);
            BlendOver(row[x], PackPremultiplied(Rgb(0, 0, 0), alpha * falloff));
        }
    }
}

// Windows 11 style battery: a stroked capsule, a terminal nub and a fill level.
void DrawBatteryGlyph(Surface& surface, float left, float top, float width,
                      float height, int percent, const Color& color, float alpha) {
    if (width <= 0.0f || height <= 0.0f) {
        return;
    }
    const float nubW = std::max(1.5f, width * 0.07f);
    const float nubGap = std::max(1.0f, width * 0.04f);
    const float bodyW = width - nubW - nubGap;
    const float thickness = std::max(1.0f, height * 0.10f);
    const RoundedRect body =
        MakeRect(left, top, left + bodyW, top + height, height * 0.30f);
    StrokeRounded(surface, body, thickness, color, alpha, color, alpha);

    const float nubH = height * 0.44f;
    const float nubTop = top + (height - nubH) * 0.5f;
    const float nubLeft = body.x1 + nubGap * 0.5f;
    FillRounded(surface,
                MakeRect(nubLeft, nubTop, nubLeft + nubW, nubTop + nubH, nubH * 0.44f),
                color, alpha, color, alpha, 0.0f);

    if (percent >= 0) {
        const float inset = thickness + std::max(1.0f, height * 0.12f);
        const float innerH = std::max(1.5f, height - inset * 2.0f);
        const float innerW = std::max(0.0f, bodyW - inset * 2.0f);
        const float fillW = innerW * Clamp01(static_cast<float>(percent) / 100.0f);
        if (fillW > 0.5f) {
            FillRounded(surface,
                        MakeRect(body.x0 + inset, body.y0 + inset,
                                 body.x0 + inset + fillW, body.y0 + inset + innerH,
                                 innerH * 0.5f),
                        color, alpha, color, alpha, 0.0f);
        }
    }
}

// Lightning bolt for the PineApple theme. The outline is tested per sample and
// the pixel gets a 2x2 supersample, which keeps the diagonals smooth.
bool InsideBolt(float px, float py, const float (&points)[6][2]) {
    bool inside = false;
    for (int i = 0, j = 5; i < 6; j = i++) {
        const float xi = points[i][0];
        const float yi = points[i][1];
        const float xj = points[j][0];
        const float yj = points[j][1];
        if ((yi > py) != (yj > py) &&
            px < (xj - xi) * (py - yi) / (yj - yi) + xi) {
            inside = !inside;
        }
    }
    return inside;
}

void FillBolt(Surface& surface, float left, float top, float width, float height,
              const Color& color, float alpha) {
    static const float kPoints[6][2] = {
        {0.60f, 0.00f}, {0.12f, 0.56f}, {0.44f, 0.56f},
        {0.40f, 1.00f}, {0.88f, 0.42f}, {0.54f, 0.42f},
    };
    if (width <= 0.0f || height <= 0.0f) {
        return;
    }
    const int firstY = std::max(0, static_cast<int>(floorf(top)));
    const int lastY =
        std::min(surface.Height(), static_cast<int>(ceilf(top + height)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(left)));
    const int lastX =
        std::min(surface.Width(), static_cast<int>(ceilf(left + width)) + 1);
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        for (int x = firstX; x < lastX; x++) {
            int hits = 0;
            for (int sy = 0; sy < 2; sy++) {
                for (int sx = 0; sx < 2; sx++) {
                    const float px =
                        (static_cast<float>(x) + 0.25f + 0.5f * sx - left) / width;
                    const float py =
                        (static_cast<float>(y) + 0.25f + 0.5f * sy - top) / height;
                    if (px >= 0.0f && px <= 1.0f && py >= 0.0f && py <= 1.0f &&
                        InsideBolt(px, py, kPoints)) {
                        hits++;
                    }
                }
            }
            if (hits) {
                BlendOver(row[x], PackPremultiplied(color, alpha * hits * 0.25f));
            }
        }
    }
}

// Progress ring for the PineApple indicator: an annulus sweep that starts at
// twelve o'clock, runs clockwise and ends in rounded caps. A rainbow palette
// tints the arc by angle, so the gradient wraps around the ring.
void DrawRingArc(Surface& surface, float cx, float cy, float radius,
                 float thickness, float sweep, const Color& color, float alpha,
                 const GlowPalette* rainbow = nullptr) {
    if (radius <= 0.0f || thickness <= 0.0f || sweep <= 0.0f || alpha <= 0.0f) {
        return;
    }
    constexpr float kTwoPi = 2.0f * kPi;
    const float half = thickness * 0.5f;
    const float reach = radius + half + 1.0f;
    const float span = std::min(sweep, kTwoPi);
    const bool full = span >= kTwoPi - 0.0005f;
    const float endX = cx + sinf(span) * radius;
    const float endY = cy - cosf(span) * radius;
    const int firstY = std::max(0, static_cast<int>(floorf(cy - reach)));
    const int lastY =
        std::min(surface.Height(), static_cast<int>(ceilf(cy + reach)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(cx - reach)));
    const int lastX =
        std::min(surface.Width(), static_cast<int>(ceilf(cx + reach)) + 1);
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        const float dy = static_cast<float>(y) + 0.5f - cy;
        for (int x = firstX; x < lastX; x++) {
            const float dx = static_cast<float>(x) + 0.5f - cx;
            const float r = sqrtf(dx * dx + dy * dy);
            if (r <= radius - half - 1.0f || r >= radius + half + 1.0f) {
                continue;
            }
            float distance = fabsf(r - radius);
            float angle = 0.0f;
            if (!full || rainbow) {
                angle = atan2f(dx, -dy);
                if (angle < 0.0f) {
                    angle += kTwoPi;
                }
            }
            if (!full && angle > span) {
                // Past the sweep, the nearest cap decides how far outside
                // the arc the pixel sits, which rounds both ends.
                const float toStart =
                    sqrtf(dx * dx + (dy + radius) * (dy + radius));
                const float ex = dx - (endX - cx);
                const float ey = dy - (endY - cy);
                distance = std::min(toStart, sqrtf(ex * ex + ey * ey));
            }
            const float coverage = Clamp01(0.5f - (distance - half));
            if (coverage > 0.0f) {
                const Color pixel =
                    rainbow ? rainbow->At(angle / kTwoPi) : color;
                BlendOver(row[x], PackPremultiplied(pixel, coverage * alpha));
            }
        }
    }
}

// The cards frost over a copy of whatever sits behind them. A layered window
// cannot ask DWM for that backdrop, so the region is captured once when the card
// appears and blurred here instead.
bool CaptureRegion(int x, int y, int width, int height, Surface& target) {
    if (width <= 0 || height <= 0 || !target.Resize(width, height)) {
        return false;
    }
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    const bool ok = BitBlt(target.Dc(), 0, 0, width, height, screen, x, y,
                           SRCCOPY | CAPTUREBLT) != FALSE;
    ReleaseDC(nullptr, screen);
    if (!ok) {
        return false;
    }
    GdiFlush();
    // Screen pixels carry no alpha, so the capture is made fully opaque.
    for (int row = 0; row < height; row++) {
        uint32_t* line = target.Row(row);
        for (int col = 0; col < width; col++) {
            line[col] |= 0xFF000000u;
        }
    }
    return true;
}

// Three box passes are close enough to a gaussian at this size, and each pass
// runs on a moving sum, so the cost does not grow with the radius.
void BlurSurface(Surface& surface, int radius) {
    const int width = surface.Width();
    const int height = surface.Height();
    if (radius <= 0 || width <= 0 || height <= 0) {
        return;
    }
    radius = std::min(radius, std::max(1, std::min(width, height) / 3));
    const int window = radius * 2 + 1;
    std::vector<uint32_t> scratch(static_cast<size_t>(width) * height);
    for (int pass = 0; pass < 3; pass++) {
        for (int y = 0; y < height; y++) {
            const uint32_t* source = surface.Row(y);
            uint32_t* line = scratch.data() + static_cast<size_t>(y) * width;
            int r = 0, g = 0, b = 0;
            for (int i = -radius; i <= radius; i++) {
                const uint32_t p = source[std::clamp(i, 0, width - 1)];
                r += static_cast<int>((p >> 16) & 0xFFu);
                g += static_cast<int>((p >> 8) & 0xFFu);
                b += static_cast<int>(p & 0xFFu);
            }
            for (int x = 0; x < width; x++) {
                line[x] = 0xFF000000u | (static_cast<uint32_t>(r / window) << 16) |
                          (static_cast<uint32_t>(g / window) << 8) |
                          static_cast<uint32_t>(b / window);
                const uint32_t entering = source[std::clamp(x + radius + 1, 0, width - 1)];
                const uint32_t leaving = source[std::clamp(x - radius, 0, width - 1)];
                r += static_cast<int>((entering >> 16) & 0xFFu) -
                     static_cast<int>((leaving >> 16) & 0xFFu);
                g += static_cast<int>((entering >> 8) & 0xFFu) -
                     static_cast<int>((leaving >> 8) & 0xFFu);
                b += static_cast<int>(entering & 0xFFu) -
                     static_cast<int>(leaving & 0xFFu);
            }
        }
        for (int x = 0; x < width; x++) {
            auto sample = [&](int y) {
                const int clamped = std::clamp(y, 0, height - 1);
                return scratch[static_cast<size_t>(clamped) * width + x];
            };
            int r = 0, g = 0, b = 0;
            for (int i = -radius; i <= radius; i++) {
                const uint32_t p = sample(i);
                r += static_cast<int>((p >> 16) & 0xFFu);
                g += static_cast<int>((p >> 8) & 0xFFu);
                b += static_cast<int>(p & 0xFFu);
            }
            for (int y = 0; y < height; y++) {
                surface.Row(y)[x] =
                    0xFF000000u | (static_cast<uint32_t>(r / window) << 16) |
                    (static_cast<uint32_t>(g / window) << 8) |
                    static_cast<uint32_t>(b / window);
                const uint32_t entering = sample(y + radius + 1);
                const uint32_t leaving = sample(y - radius);
                r += static_cast<int>((entering >> 16) & 0xFFu) -
                     static_cast<int>((leaving >> 16) & 0xFFu);
                g += static_cast<int>((entering >> 8) & 0xFFu) -
                     static_cast<int>((leaving >> 8) & 0xFFu);
                b += static_cast<int>(entering & 0xFFu) -
                     static_cast<int>(leaving & 0xFFu);
            }
        }
    }
}

// Bilinear resample that keeps the target at the source size and shrinks the
// content around its centre, which is all the spring entrance needs.
bool ScaleInto(const Surface& source, float factor, Surface& target) {
    if (!source.Valid() || factor <= 0.02f) {
        return false;
    }
    const int width = source.Width();
    const int height = source.Height();
    if (!target.Resize(width, height)) {
        return false;
    }
    const float cx = (static_cast<float>(width) - 1.0f) * 0.5f;
    const float cy = (static_cast<float>(height) - 1.0f) * 0.5f;
    const float inv = 1.0f / factor;
    for (int y = 0; y < height; y++) {
        uint32_t* row = target.Row(y);
        const float sy = (static_cast<float>(y) - cy) * inv + cy;
        const int y0 = std::clamp(static_cast<int>(floorf(sy)), 0, height - 1);
        const int y1 = std::clamp(y0 + 1, 0, height - 1);
        const float fy = Clamp01(sy - floorf(sy));
        const uint32_t* rowA = source.Row(y0);
        const uint32_t* rowB = source.Row(y1);
        for (int x = 0; x < width; x++) {
            const float sx = (static_cast<float>(x) - cx) * inv + cx;
            const int x0 = std::clamp(static_cast<int>(floorf(sx)), 0, width - 1);
            const int x1 = std::clamp(x0 + 1, 0, width - 1);
            const float fx = Clamp01(sx - floorf(sx));
            const float w00 = (1.0f - fx) * (1.0f - fy);
            const float w10 = fx * (1.0f - fy);
            const float w01 = (1.0f - fx) * fy;
            const float w11 = fx * fy;
            const uint32_t p00 = rowA[x0], p10 = rowA[x1];
            const uint32_t p01 = rowB[x0], p11 = rowB[x1];
            uint32_t pixel = 0;
            for (int shift = 24; shift >= 0; shift -= 8) {
                const float value = static_cast<float>((p00 >> shift) & 0xFFu) * w00 +
                                    static_cast<float>((p10 >> shift) & 0xFFu) * w10 +
                                    static_cast<float>((p01 >> shift) & 0xFFu) * w01 +
                                    static_cast<float>((p11 >> shift) & 0xFFu) * w11;
                pixel |= static_cast<uint32_t>(value + 0.5f) << shift;
            }
            row[x] = pixel;
        }
    }
    return true;
}

// Paints the blurred capture inside a rounded tile and darkens it into glass,
// which is what makes the HUD look translucent instead of painted on.
void FillGlass(Surface& surface, const RoundedRect& rect, const Surface& backdrop,
               const Color& scrim, float scrimAlpha, const Color& tint,
               float tintAlpha) {
    if (rect.x1 <= rect.x0 || rect.y1 <= rect.y0) {
        return;
    }
    const int firstY = std::max(0, static_cast<int>(floorf(rect.y0)) - 1);
    const int lastY = std::min(surface.Height(), static_cast<int>(ceilf(rect.y1)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(rect.x0)) - 1);
    const int lastX = std::min(surface.Width(), static_cast<int>(ceilf(rect.x1)) + 1);
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        const uint32_t* imageRow = backdrop.Valid() && y < backdrop.Height()
                                       ? backdrop.Row(y)
                                       : nullptr;
        for (int x = firstX; x < lastX; x++) {
            const float coverage =
                Clamp01(0.5f - RoundedRectSdf(static_cast<float>(x) + 0.5f,
                                              static_cast<float>(y) + 0.5f, rect));
            if (coverage <= 0.0f) {
                continue;
            }
            Color pixel = scrim;
            if (imageRow && x < backdrop.Width()) {
                const uint32_t p = imageRow[x];
                pixel = Mix(Rgb(static_cast<int>((p >> 16) & 0xFFu),
                                static_cast<int>((p >> 8) & 0xFFu),
                                static_cast<int>(p & 0xFFu)),
                            scrim, scrimAlpha);
            }
            if (tintAlpha > 0.0f) {
                pixel = Mix(pixel, tint, tintAlpha);
            }
            BlendOver(row[x], PackPremultiplied(pixel, coverage));
        }
    }
}

// A round patch of the blurred capture, darkened into glass and feathered out
// to nothing at the rim. The soft edge is the point: it gives a floating HUD
// something to lay a number on without a panel outline showing up.
void FillFrostDisc(Surface& surface, float cx, float cy, float radius,
                   const Surface& backdrop, const Color& scrim,
                   float scrimAlpha, float alpha) {
    if (!surface.Valid() || radius <= 0.5f || alpha <= 0.0f) {
        return;
    }
    const int firstY = std::max(0, static_cast<int>(floorf(cy - radius)));
    const int lastY =
        std::min(surface.Height(), static_cast<int>(ceilf(cy + radius)) + 1);
    const int firstX = std::max(0, static_cast<int>(floorf(cx - radius)));
    const int lastX =
        std::min(surface.Width(), static_cast<int>(ceilf(cx + radius)) + 1);
    const float invRadius = 1.0f / radius;
    for (int y = firstY; y < lastY; y++) {
        uint32_t* row = surface.Row(y);
        const uint32_t* imageRow = backdrop.Valid() && y < backdrop.Height()
                                       ? backdrop.Row(y)
                                       : nullptr;
        const float dy = static_cast<float>(y) + 0.5f - cy;
        for (int x = firstX; x < lastX; x++) {
            const float dx = static_cast<float>(x) + 0.5f - cx;
            const float distance = sqrtf(dx * dx + dy * dy) * invRadius;
            if (distance >= 1.0f) {
                continue;
            }
            // Full strength over the middle and feathered away over the outer
            // third, so the patch has a soft rim instead of a vignette that
            // keeps fading from the centre out.
            const float kCore = 0.62f;
            const float edge = Clamp01((1.0f - distance) / (1.0f - kCore));
            const float fade = edge * edge * (3.0f - 2.0f * edge);
            Color pixel = scrim;
            if (imageRow && x < backdrop.Width()) {
                const uint32_t p = imageRow[x];
                pixel = Mix(Rgb(static_cast<int>((p >> 16) & 0xFFu),
                                static_cast<int>((p >> 8) & 0xFFu),
                                static_cast<int>(p & 0xFFu)),
                            scrim, scrimAlpha);
            }
            BlendOver(row[x], PackPremultiplied(pixel, alpha * fade));
        }
    }
}

// Windows 11 themes its surfaces from the personalisation settings, so the card
// follows the app theme instead of forcing its own.
struct OverlayTheme {
    Color panelTop;
    Color panelBottom;
    float panelAlpha = 0.94f;
    Color hairline;
    float hairlineTop = 0.14f;
    float hairlineBottom = 0.05f;
    Color title;
    Color detail;
    Color track;
    float shadowAlpha = 0.34f;
    float tintTop = 0.07f;
    float tintBottom = 0.05f;
    bool light = false;
};

bool IsLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

OverlayTheme MakeOverlayTheme() {
    OverlayTheme theme;
    theme.light = IsLightTheme();
    if (theme.light) {
        theme.panelTop = Rgb(250, 250, 251);
        theme.panelBottom = Rgb(237, 237, 241);
        theme.panelAlpha = 0.92f;
        theme.hairline = Rgb(0, 0, 0);
        theme.hairlineTop = 0.10f;
        theme.hairlineBottom = 0.04f;
        theme.title = Rgb(28, 28, 30);
        theme.detail = Rgb(97, 100, 107, 0.96f);
        theme.track = Rgb(0, 0, 0);
        theme.shadowAlpha = 0.22f;
        theme.tintTop = 0.03f;
        theme.tintBottom = 0.04f;
    } else {
        theme.panelTop = Rgb(45, 46, 50);
        theme.panelBottom = Rgb(30, 31, 35);
        theme.panelAlpha = 0.94f;
        theme.hairline = Rgb(255, 255, 255);
        theme.hairlineTop = 0.14f;
        theme.hairlineBottom = 0.05f;
        theme.title = Rgb(255, 255, 255);
        theme.detail = Rgb(200, 203, 211, 0.94f);
        theme.track = Rgb(255, 255, 255);
        theme.shadowAlpha = 0.34f;
        theme.tintTop = 0.07f;
        theme.tintBottom = 0.05f;
    }
    return theme;
}

// Text is laid out by GDI into a black scratch surface, then the luminance is
// used as a coverage mask, which keeps antialiasing while staying premultiplied.
class StatsOverlay {
public:
    static constexpr UINT_PTR kTimerId = 0x51;

    bool Show(const POINT& rippleOrigin, const OverlayStyle& style) {
        style_ = style;
        // The anchor is settled once, here. Working it out again on each
        // refresh would let a pointer that has moved to another monitor drag
        // the card over there, still carrying this monitor's scale and the
        // blurred backdrop taken from its old spot.
        anchor_ = KeyIs(style_.position, L"ripple")
                      ? rippleOrigin
                      : ResolveRippleOrigin(style_.position);
        const bool wasVisible = visible_;
        if (!window_.Hwnd() && !window_.Create()) {
            Wh_Log(L"Could not create the stats overlay window.");
            return false;
        }
        visible_ = true;
        closing_ = false;
        shownAt_ = NowMs();
        hideAt_ = shownAt_ + std::max(600, style_.durationMs);
        statsAt_ = shownAt_ + 1000.0;
        if (!Refresh()) {
            Hide();
            return false;
        }
        // The frosted backdrop is grabbed while the card is still hidden, or the
        // capture would contain the previous card and smear it into the blur.
        // The plain PineApple card never needs it.
        if (!wasVisible && UsesBackdrop() && CaptureBackdrop()) {
            Refresh();
        }
        Push(0.0f, (springy_ || hud_) ? 0.0f : IntroOffset(),
             springy_ ? kSpringFrom : (hud_ ? kHudFrom : 1.0f));
        window_.Show();
        Schedule(kAnimMs);
        return true;
    }

    void Tick() {
        if (!visible_) {
            StopTimer();
            return;
        }
        const double now = NowMs();
        if (!closing_ && now >= hideAt_) {
            closing_ = true;
            closingAt_ = now;
        }

        float envelope = 1.0f;
        float slide = 0.0f;
        float pop = 1.0f;
        if (closing_) {
            const double span =
                hud_ ? kHudOutMs : (springy_ ? kSpringOutMs : kOutroMs);
            const float t = static_cast<float>(
                std::clamp((now - closingAt_) / span, 0.0, 1.0));
            if (hud_) {
                // A system HUD disappears the way it arrived, just faster: a fade
                // with the smallest shrink instead of a slide.
                envelope = 1.0f - EaseOut(t);
                pop = 1.0f - 0.05f * t;
            } else if (springy_) {
                // The phone dissolves its HUD with a quick shrink, it does not
                // slide it away.
                envelope = 1.0f - t * t;
                pop = 1.0f - 0.08f * t * t;
            } else {
                envelope = 1.0f - EaseOut(t);
                slide = static_cast<float>(kOutroSlide) * scale_ * t;
            }
            if (t >= 1.0f) {
                Hide();
                return;
            }
        } else if (hud_) {
            const float t = static_cast<float>(
                std::clamp((now - shownAt_) / kHudMs, 0.0, 1.0));
            pop = t >= 1.0f ? 1.0f
                            : kHudFrom + (1.0f - kHudFrom) * EaseOutBack(t);
            // The fade finishes ahead of the scale, so the tile is never caught
            // half transparent while it is still growing.
            envelope = EaseOut(static_cast<float>(
                std::clamp((now - shownAt_) / (kHudMs * 0.7), 0.0, 1.0)));
        } else if (springy_) {
            const float t = static_cast<float>(
                std::clamp((now - shownAt_) / kSpringMs, 0.0, 1.0));
            pop = t >= 1.0f ? 1.0f
                            : kSpringFrom + (1.0f - kSpringFrom) * EaseOutBack(t);
            // The fade runs ahead of the spring so the card is never seen half
            // transparent while it is still growing.
            envelope = EaseOut(static_cast<float>(
                std::clamp((now - shownAt_) / (kSpringMs * 0.4), 0.0, 1.0)));
        } else {
            const float t =
                static_cast<float>(std::clamp((now - shownAt_) / kIntroMs, 0.0, 1.0));
            envelope = EaseOut(t);
            slide = static_cast<float>(kIntroSlide) * scale_ * (1.0f - envelope);
        }

        // The card only needs new content once every second; animation frames just
        // re-present the finished surface.
        if (!closing_ && now >= statsAt_) {
            statsAt_ = now + 1000.0;
            if (!Refresh()) {
                Hide();
                return;
            }
        }

        // The PineApple ring and the Fluent level bar keep filling past the
        // fade-in, so they drive the frame cadence a little longer than the intro
        // does.
        const bool ringAnimating = !closing_ && RingAnimating(now);
        if (ringAnimating) {
            RenderRingSweep(RingProgress(now));
        }
        const bool barAnimating = !closing_ && BarAnimating(now);
        if (barAnimating) {
            RenderBarFill(BarProgress(now));
        }
        const bool popping = std::fabs(pop - 1.0f) > 0.001f;
        Push(envelope, slide, pop);

        // Wake up for the next animation frame, or right when the card is due
        // to close, so a short duration is not stretched by the idle cadence.
        UINT next = kIdleMs;
        if (envelope < 1.0f || ringAnimating || barAnimating || popping) {
            next = kAnimMs;
        } else {
            const double remaining = hideAt_ - now;
            if (remaining < static_cast<double>(kIdleMs)) {
                next = static_cast<UINT>(std::max<double>(1.0, remaining + 1.0));
            }
        }
        Schedule(next);
    }

    void Hide() {
        visible_ = false;
        closing_ = false;
        StopTimer();
        window_.Hide();
    }

    void Destroy() {
        Hide();
        window_.Destroy();
        surface_.Reset();
        mask_.Reset();
        base_.Reset();
        backdrop_.Reset();
        scaled_.Reset();
    }

private:
    void StopTimer() {
        if (g_messageWindow) {
            KillTimer(g_messageWindow, kTimerId);
        }
    }

    // dx/dy shift the mask, which is how the caption gets its soft shadow.
    void CompositeText(const Color& color, int dx = 0, int dy = 0) {
        const int width = std::min(surface_.Width(), mask_.Width());
        const int height = std::min(surface_.Height(), mask_.Height());
        for (int y = 0; y < height; y++) {
            const int targetY = y + dy;
            if (targetY < 0 || targetY >= surface_.Height()) {
                continue;
            }
            const uint32_t* source = mask_.Row(y);
            uint32_t* target = surface_.Row(targetY);
            for (int x = 0; x < width; x++) {
                const int targetX = x + dx;
                if (targetX < 0 || targetX >= surface_.Width()) {
                    continue;
                }
                const uint32_t pixel = source[x];
                if (!pixel) {
                    continue;
                }
                const uint32_t coverage = std::max({(pixel >> 16) & 0xFFu,
                                                    (pixel >> 8) & 0xFFu, pixel & 0xFFu});
                if (!coverage) {
                    continue;
                }
                BlendOver(target[targetX], PackPremultiplied(
                                               color, static_cast<float>(coverage) / 255.0f));
            }
        }
    }

    static constexpr UINT kAnimMs = 16;
    static constexpr UINT kIdleMs = 1000;
    static constexpr double kIntroMs = 170.0;
    static constexpr double kOutroMs = 130.0;
    static constexpr double kRingMs = 520.0;
    // The Fluent level bar fills from empty to the battery level as the card
    // arrives, a touch ahead of the phone ring it mirrors.
    static constexpr double kBarMs = 460.0;
    static constexpr float kIntroSlide = 10.0f;
    static constexpr float kOutroSlide = 6.0f;
    // The card springs in over a little under half a second and
    // leaves much faster than it arrived.
    static constexpr double kSpringMs = 420.0;
    static constexpr double kSpringOutMs = 200.0;
    static constexpr float kSpringFrom = 0.84f;
    // The system-style HUDs arrive in a fraction of the time the cards take and
    // leave almost instantly.
    static constexpr double kHudMs = 190.0;
    static constexpr double kHudOutMs = 150.0;
    static constexpr float kHudFrom = 0.94f;

    static float EaseOut(float t) {
        const float inverted = 1.0f - Clamp01(t);
        return 1.0f - inverted * inverted * inverted;
    }

    float IntroOffset() const { return kIntroSlide * scale_; }

    // The ring fills over roughly half a second, the way the phone draws it.
    bool RingAnimating(double now) const {
        return ringReady_ && now - shownAt_ < kRingMs;
    }

    float RingProgress(double now) const {
        if (ringSweep_ <= 0.0f) {
            return 0.0f;
        }
        const double t = (now - shownAt_) / kRingMs;
        return t >= 1.0 ? 1.0f
                        : EaseOut(static_cast<float>(std::clamp(t, 0.0, 1.0)));
    }

    // The Fluent bar grows over a similar beat, so it lands while the card is
    // still settling instead of popping in at full width.
    bool BarAnimating(double now) const {
        return barReady_ && barLevel_ > 0.0f && now - shownAt_ < kBarMs;
    }

    float BarProgress(double now) const {
        const double t = (now - shownAt_) / kBarMs;
        return t >= 1.0 ? 1.0f
                        : EaseOut(static_cast<float>(std::clamp(t, 0.0, 1.0)));
    }

    // Restores the static frame and paints the level ring on top of it.
    void RenderRingSweep(float progress) {
        if (!ringReady_ || !base_.Valid()) {
            return;
        }
        surface_.CopyFrom(base_);
        GlowPalette gradient = MakeRainbowPalette();
        DrawRingArc(surface_, ringCx_, ringCy_, ringRadius_, ringThickness_,
                    ringSweep_ * Clamp01(progress), ringColor_, 0.97f,
                    rainbow_ ? &gradient : nullptr);
    }

    // Repaints the level bar over the static frame: the track stays where it is
    // and the fill reaches further right as progress rises, which is what makes
    // the card look like it is waking up rather than being pasted on.
    void RenderBarFill(float progress) {
        if (!barReady_ || !base_.Valid()) {
            return;
        }
        surface_.CopyFrom(base_);
        const float fillW = barW_ * barLevel_ * Clamp01(progress);
        if (fillW <= 0.5f) {
            return;
        }
        GlowPalette gradient = MakeRainbowPalette();
        FillRounded(surface_,
                    MakeRect(barX_, barY_, barX_ + fillW, barY_ + barH_,
                             barH_ * 0.5f),
                    barTopColor_, 0.95f, barColor_, 0.95f, 0.0f,
                    barSweep_ ? &gradient : nullptr);
    }

    // 16 ms while the card animates, one second once it has settled.
    void Schedule(UINT interval) {
        if (g_messageWindow) {
            SetTimer(g_messageWindow, kTimerId, interval, nullptr);
        }
    }

    // pop < 1 shrinks the whole card around its centre, which is how the spring
    // is drawn without rasterising the labels at a different size every frame.
    void Push(float envelope, float slide, float pop = 1.0f) {
        const float opacity =
            static_cast<float>(std::clamp(style_.opacity, 0, 100)) / 100.0f;
        const BYTE alpha =
            static_cast<BYTE>(Clamp01(opacity * envelope) * 255.0f + 0.5f);
        Surface* frame = &surface_;
        if (std::fabs(pop - 1.0f) > 0.001f && ScaleInto(surface_, pop, scaled_)) {
            frame = &scaled_;
        }
        window_.Present(*frame, left_, top_ + static_cast<int>(slide + 0.5f), alpha);
    }

    // Only the glass cards read the screen behind them; the plain one is a flat
    // slab of its own colour, so capturing it would just cost a frame.
    bool UsesBackdrop() const { return !KeyIs(style_.theme, L"osd"); }

    // Grabs the screen under the card and blurs it, so the glass has something
    // real behind it. Only valid while the window is hidden.
    bool CaptureBackdrop() {
        if (!surface_.Valid() ||
            !CaptureRegion(left_, top_, surface_.Width(), surface_.Height(),
                           backdrop_)) {
            backdrop_.Reset();
            return false;
        }
        BlurSurface(backdrop_,
                    std::max(6, static_cast<int>(18.0f * scale_ + 0.5f)));
        return true;
    }

    // Lays the card out and rasterises it; presentation happens in Push().
    bool Refresh() {
        ringReady_ = false;
        barReady_ = false;
        const ChargingStats stats = ReadChargingStats();
        const std::wstring title =
            stats.percent >= 0 ? std::to_wstring(stats.percent) + L"%"
                               : (stats.onAc ? L"AC" : L"--");
        const std::wstring detail =
            style_.details ? BuildDetailLine(stats) : std::wstring();

        scale_ = DpiScaleAt(anchor_);
        const bool galaxy = KeyIs(style_.theme, L"galaxy");
        springy_ = KeyIs(style_.theme, L"pineapple");
        // The Galaxy and PineApple cards are system HUDs, so they fade and scale
        // in instead of sliding like the Fluent cards.
        hud_ = KeyIs(style_.theme, L"osd") || galaxy;
        if (springy_) {
            return RefreshPineApple(stats, title, detail);
        }
        if (galaxy) {
            return RefreshGalaxy(stats, title, detail);
        }
        if (hud_) {
            return RefreshPineAppleDesktop(stats, title, detail);
        }
        return RefreshFluent(stats, title, detail, KeyIs(style_.theme, L"minimal"));
    }

    // Fluent: the Windows 11 acrylic card with the icon tile and the level bar.
    // The minimal variant is the same card at badge size, without the status line.
    bool RefreshFluent(const ChargingStats& stats, const std::wstring& title,
                       const std::wstring& detail, bool minimal) {
        const float s = scale_;
        const int basePx = std::clamp(style_.fontPx, 8, 64);
        const int titlePx = std::max(
            12, static_cast<int>(basePx * (minimal ? 1.05f : 1.55f) * s + 0.5f));
        const int detailPx = std::max(9, static_cast<int>(basePx * 0.62f * s + 0.5f));
        const bool windows11 = IsWindows11();

        HFONT titleFont = CreateFontW(-titlePx, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                      windows11 ? L"Segoe UI Variable Display" : L"Segoe UI");
        HFONT detailFont = CreateFontW(-detailPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                                       ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                       windows11 ? L"Segoe UI Variable Text" : L"Segoe UI");
        if (!titleFont || !detailFont) {
            if (titleFont) DeleteObject(titleFont);
            if (detailFont) DeleteObject(detailFont);
            return false;
        }

        SIZE titleSize{};
        SIZE detailSize{};
        const bool showDetail = !minimal && !detail.empty();
        HDC measure = CreateCompatibleDC(nullptr);
        if (measure) {
            HGDIOBJ previous = SelectObject(measure, titleFont);
            GetTextExtentPoint32W(measure, title.c_str(), static_cast<int>(title.size()),
                                  &titleSize);
            if (showDetail) {
                SelectObject(measure, detailFont);
                GetTextExtentPoint32W(measure, detail.c_str(),
                                      static_cast<int>(detail.size()), &detailSize);
                // Width only: the card is sized for the line this state can end
                // up with, while the height stays that of the real one. The text
                // is drawn left aligned, so the spare column costs nothing.
                SIZE reserved{};
                const std::wstring reserve = ReserveDetailLine(stats);
                GetTextExtentPoint32W(measure, reserve.c_str(),
                                      static_cast<int>(reserve.size()), &reserved);
                detailSize.cx = std::max(detailSize.cx, reserved.cx);
            }
            SelectObject(measure, previous);
            DeleteDC(measure);
        }

        // Fluent metrics: 18 px padding, 42 px icon tile, 8 px corner radius. The
        // minimal card halves the padding and the tile and drops the extra line.
        const int pad =
            minimal ? std::max(5, static_cast<int>(9.0f * s + 0.5f))
                    : std::max(8, static_cast<int>(18.0f * s + 0.5f));
        const int tile =
            minimal ? std::max(16, static_cast<int>(22.0f * s + 0.5f))
                    : std::max(24, static_cast<int>(42.0f * s + 0.5f));
        const int gap =
            minimal ? std::max(4, static_cast<int>(8.0f * s + 0.5f))
                    : std::max(6, static_cast<int>(14.0f * s + 0.5f));
        const int lineGap = std::max(1, static_cast<int>(3.0f * s + 0.5f));
        const int barH =
            minimal ? std::max(2, static_cast<int>(2.5f * s + 0.5f))
                    : std::max(3, static_cast<int>(4.0f * s + 0.5f));
        const int barGap =
            minimal ? std::max(3, static_cast<int>(5.0f * s + 0.5f))
                    : std::max(4, static_cast<int>(11.0f * s + 0.5f));
        const bool showBar = stats.percent >= 0;
        const int minTextW =
            minimal ? std::max(34, static_cast<int>(56.0f * s + 0.5f))
                    : std::max(60, static_cast<int>(104.0f * s + 0.5f));

        const int textW = std::max({static_cast<int>(titleSize.cx),
                                    static_cast<int>(detailSize.cx),
                                    showBar ? minTextW : 0});
        int textH = static_cast<int>(titleSize.cy);
        if (showDetail) {
            textH += lineGap + static_cast<int>(detailSize.cy);
        }
        if (showBar) {
            textH += barGap + barH;
        }

        const int cardW = pad * 2 + tile + gap + textW;
        const int cardH = pad * 2 + std::max(textH, tile);
        if (cardW <= 0 || cardH <= 0 || cardW > 4000 || cardH > 2000) {
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            return false;
        }

        // The surface carries the drop shadow too, so it is larger than the card.
        const float sigma = (minimal ? 6.0f : 8.0f) * s;
        const int shadow = static_cast<int>(ceilf(sigma * 3.0f));
        const int width = cardW + shadow * 2;
        const int height = cardH + shadow * 2;
        if (!surface_.Resize(width, height) || !mask_.Resize(width, height)) {
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            return false;
        }

        const Color accent =
            ResolveOverlayAccent(style_.color, style_.customColor);
        const GlowPalette gradient = MakeRainbowPalette();
        const GlowPalette* sweep = style_.rainbow ? &gradient : nullptr;
        const OverlayTheme theme = MakeOverlayTheme();
        const RoundedRect card = MakeRect(static_cast<float>(shadow),
                                          static_cast<float>(shadow),
                                          static_cast<float>(shadow + cardW),
                                          static_cast<float>(shadow + cardH),
                                          minimal ? std::max(5.0f, 6.0f * s)
                                                  : std::max(6.0f, 8.0f * s));
        surface_.Clear();
        CastShadow(surface_, card, sigma, 4.0f * s, theme.shadowAlpha);

        // Hairline: brighter at the top, fading out towards the bottom.
        const float hairline = std::max(1.0f, s);
        StrokeRounded(surface_, card, hairline, theme.hairline, theme.hairlineTop,
                      theme.hairline, theme.hairlineBottom);

        // Acrylic: the blurred desktop behind the card, covered by the theme
        // colour and then by the vertical acrylic gradient. The blur is what
        // stops text behind the card from being read through it.
        const RoundedRect panel =
            MakeRect(card.x0 + hairline, card.y0 + hairline, card.x1 - hairline,
                     card.y1 - hairline, std::max(5.0f, card.radius - hairline));
        FillGlass(surface_, panel, backdrop_, theme.panelTop,
                  theme.panelAlpha * 0.86f, accent, theme.tintTop);
        FillRounded(surface_, panel, Mix(theme.panelTop, accent, theme.tintTop), 0.34f,
                    Mix(theme.panelBottom, accent, theme.tintBottom), 0.34f, 0.012f);

        const float tileX = static_cast<float>(shadow + pad);
        const float tileY = static_cast<float>(shadow) +
                            (static_cast<float>(cardH) - static_cast<float>(tile)) * 0.5f;
        FillRounded(surface_,
                    MakeRect(tileX, tileY, tileX + tile, tileY + tile,
                             minimal ? std::max(4.0f, tile * 0.26f)
                                     : std::max(4.0f, 8.0f * s)),
                    accent, 0.22f, accent, 0.11f, 0.0f, sweep);
        // The badge glyph is drawn larger relative to its smaller tile so it
        // stays readable at minimal size.
        const float glyphW = static_cast<float>(tile) * (minimal ? 0.68f : 0.56f);
        const float glyphH = static_cast<float>(tile) * (minimal ? 0.34f : 0.28f);
        const Color glyph = theme.light ? Mix(accent, Rgb(24, 24, 26), 0.45f)
                                        : Mix(Rgb(255, 255, 255), accent, 0.45f);
        DrawBatteryGlyph(surface_, tileX + (tile - glyphW) * 0.5f,
                         tileY + (tile - glyphH) * 0.5f, glyphW, glyphH, stats.percent,
                         glyph, 0.96f);

        const int textLeft = shadow + pad + tile + gap;
        const int textTop = shadow + (cardH - textH) / 2;

        HDC dc = mask_.Dc();
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));

        mask_.Clear();
        HGDIOBJ previous = SelectObject(dc, titleFont);
        TextOutW(dc, textLeft, textTop, title.c_str(), static_cast<int>(title.size()));
        GdiFlush();
        SelectObject(dc, previous);
        CompositeText(theme.title);

        if (showDetail) {
            mask_.Clear();
            previous = SelectObject(dc, detailFont);
            TextOutW(dc, textLeft, textTop + static_cast<int>(titleSize.cy) + lineGap,
                     detail.c_str(), static_cast<int>(detail.size()));
            GdiFlush();
            SelectObject(dc, previous);
            CompositeText(theme.detail);
        }

        if (showBar) {
            const int barTop =
                textTop + static_cast<int>(titleSize.cy) +
                (showDetail ? lineGap + static_cast<int>(detailSize.cy) : 0) + barGap;
            const float barRight = static_cast<float>(textLeft + textW);
            FillRounded(surface_,
                        MakeRect(static_cast<float>(textLeft), static_cast<float>(barTop),
                                 barRight, static_cast<float>(barTop + barH), barH * 0.5f),
                        theme.track, 0.13f, theme.track, 0.09f, 0.0f);
            Color bar = theme.light ? Mix(accent, Rgb(16, 16, 18), 0.14f) : accent;
            // The low battery warning keeps its red even when the wave is a
            // gradient, so the shape stops sampling the palette in that case.
            const GlowPalette* barSweep = sweep;
            if (!stats.onAc && stats.percent <= 20) {
                bar = Mix(Rgb(232, 84, 66), accent, 0.2f);
                barSweep = nullptr;
            }
            const Color barTopColor =
                theme.light ? bar : Mix(bar, Rgb(255, 255, 255), 0.15f);
            // The fill is not baked in: the empty track above becomes the static
            // frame and the fill is painted over a copy of it on every animation
            // frame, so it grows in from the left instead of arriving complete.
            barReady_ = true;
            barX_ = static_cast<float>(textLeft);
            barY_ = static_cast<float>(barTop);
            barW_ = static_cast<float>(textW);
            barH_ = static_cast<float>(barH);
            barLevel_ = Clamp01(stats.percent / 100.0f);
            barTopColor_ = barTopColor;
            barColor_ = bar;
            barSweep_ = barSweep != nullptr;
        }

        // Keep the track-only frame and put the fill back on at the beat the card
        // is currently on, so a refresh mid-animation does not snap it to full.
        if (barReady_) {
            base_.CopyFrom(surface_);
            RenderBarFill(BarProgress(NowMs()));
        }

        DeleteObject(titleFont);
        DeleteObject(detailFont);

        return PlaceOnScreen(width, height);
    }

    // PineApple: the phone charging indicator. A progress ring fills clockwise
    // up to the battery level around the percentage and a bolt, with the caption
    // on a frosted pill underneath and a soft disc of blur behind the number.
    bool RefreshPineApple(const ChargingStats& stats, const std::wstring& title,
                          const std::wstring& detail) {
        const float s = scale_;
        const int basePx = std::clamp(style_.fontPx, 8, 64);
        const float outer = std::max(56.0f, static_cast<float>(basePx) * 6.4f * s);
        const float thickness = std::max(4.0f, outer * 0.085f);
        const float radius = outer * 0.5f - thickness * 0.5f;
        const int pctPx = std::max(12, static_cast<int>(outer * 0.27f + 0.5f));
        const int capPx = std::max(9, static_cast<int>(basePx * 0.78f * s + 0.5f));
        const bool windows11 = IsWindows11();
        const wchar_t* display = windows11 ? L"Segoe UI Variable Display" : L"Segoe UI";
        const wchar_t* captionFace = windows11 ? L"Segoe UI Variable Text" : L"Segoe UI";

        HFONT pctFont = CreateFontW(-pctPx, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                    display);
        HFONT capFont = CreateFontW(-capPx, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                    captionFace);
        if (!pctFont || !capFont) {
            if (pctFont) DeleteObject(pctFont);
            if (capFont) DeleteObject(capFont);
            return false;
        }

        SIZE pctSize{};
        SIZE capSize{};
        SIZE capReserve{};
        HDC measure = CreateCompatibleDC(nullptr);
        if (measure) {
            HGDIOBJ previous = SelectObject(measure, pctFont);
            GetTextExtentPoint32W(measure, title.c_str(), static_cast<int>(title.size()),
                                  &pctSize);
            if (!detail.empty()) {
                SelectObject(measure, capFont);
                GetTextExtentPoint32W(measure, detail.c_str(),
                                      static_cast<int>(detail.size()), &capSize);
                // The pill is cut for the widest caption this state can reach,
                // so the estimate and the wattage landing later cannot widen it;
                // the caption itself stays centred on its real width.
                const std::wstring reserve = ReserveDetailLine(stats);
                GetTextExtentPoint32W(measure, reserve.c_str(),
                                      static_cast<int>(reserve.size()), &capReserve);
            }
            SelectObject(measure, previous);
            DeleteDC(measure);
        }

        // The bolt only shows while the battery is actually taking charge.
        const bool charging =
            stats.charging || (stats.onAc && stats.percent >= 0 && stats.percent < 100);
        const float glyphH = static_cast<float>(std::max<LONG>(pctSize.cy, 1L));
        const float boltH = charging ? outer * 0.19f : 0.0f;
        const float boltW = boltH * 0.60f;
        const float boltGap = charging ? outer * 0.035f : 0.0f;
        // The bolt and the percentage share the middle of the ring.
        const float groupH = boltH + boltGap + glyphH;

        // No panel behind the dial: the ring floats on the desktop and the
        // supporting line gets a pill, so the caption reads over any wallpaper
        // without a card wrapping the whole HUD. The number gets a soft frosted
        // patch of its own, and because that patch has no edge it still reads
        // as floating glass rather than a tile.
        const int captionW = detail.empty() ? 0 : static_cast<int>(capSize.cx);
        // The pill is cut for the reserved width, but the caption is centred on
        // the width it really has, so a short line sits in the middle of it.
        const int captionReserve =
            detail.empty() ? 0 : std::max(static_cast<int>(capSize.cx),
                                          static_cast<int>(capReserve.cx));
        const int pillPadX = std::max(8, static_cast<int>(12.0f * s + 0.5f));
        const int pillPadY = std::max(4, static_cast<int>(6.0f * s + 0.5f));
        const int pillW = detail.empty() ? 0 : captionReserve + pillPadX * 2;
        const int pillH =
            detail.empty() ? 0 : static_cast<int>(capSize.cy) + pillPadY * 2;
        const int gap = std::max(4, static_cast<int>(10.0f * s + 0.5f));
        const float sigma = 6.0f * s;
        const int shadow = static_cast<int>(ceilf(sigma * 3.0f));
        const int ringBox = static_cast<int>(ceilf(outer));
        const int contentW = std::max(ringBox, pillW);
        const int contentH = ringBox + (detail.empty() ? 0 : gap + pillH);
        const int cardW = contentW + shadow * 2;
        const int cardH = contentH + shadow * 2;
        if (contentW <= 0 || contentH <= 0 || cardW > 4000 || cardH > 2000 ||
            !surface_.Resize(cardW, cardH) || !mask_.Resize(cardW, cardH) ||
            !base_.Resize(cardW, cardH)) {
            DeleteObject(pctFont);
            DeleteObject(capFont);
            return false;
        }

        const Color accent =
            ResolveOverlayAccent(style_.color, style_.customColor);
        const bool low = !stats.onAc && stats.percent >= 0 && stats.percent <= 20;
        // A gradient ripple paints the arc itself; the red low battery warning
        // is still a single colour.
        rainbow_ = style_.rainbow && !low;
        const Color level = low ? Mix(Rgb(255, 59, 48), accent, 0.15f) : accent;

        const float originX = static_cast<float>(shadow);
        const float originY = static_cast<float>(shadow);
        ringCx_ = originX + static_cast<float>(contentW) * 0.5f;
        ringCy_ = originY + outer * 0.5f;
        ringRadius_ = radius;
        ringThickness_ = thickness;
        ringSweep_ = 2.0f * kPi *
                     Clamp01(static_cast<float>(std::max(stats.percent, 0)) / 100.0f);
        ringColor_ = level;

        surface_.Clear();

        // A disc of that same blur behind the number, stopped short of the ring
        // so the two never touch and the arc keeps the wallpaper on both sides
        // of it. It is mixed well toward black, which is what holds the white
        // digits over a bright photo.
        const float lensRadius = std::max(thickness, radius - thickness * 1.4f);
        FillFrostDisc(surface_, ringCx_, ringCy_, lensRadius, backdrop_,
                      Rgb(8, 8, 10), 0.58f, 0.62f);

        // The caption pill is the other frosted piece: the blurred capture under a
        // dark scrim plus a wash of the accent, so the line reads over anything.
        const int pillTop = shadow + ringBox + gap;
        if (!detail.empty()) {
            const float pillX = originX + (static_cast<float>(contentW) -
                                           static_cast<float>(pillW)) * 0.5f;
            const RoundedRect pill =
                MakeRect(pillX, static_cast<float>(pillTop),
                         pillX + static_cast<float>(pillW),
                         static_cast<float>(pillTop + pillH),
                         static_cast<float>(pillH) * 0.5f);
            GlowPalette bloomPalette = MakeRainbowPalette();
            const Color bloom = rainbow_ ? bloomPalette.At(0.18f) : accent;
            CastShadow(surface_, pill, sigma, 2.0f * s, 0.34f);
            FillGlass(surface_, pill, backdrop_, Rgb(8, 8, 10), 0.62f, bloom, 0.08f);
            StrokeRounded(surface_, pill, std::max(1.0f, s), Rgb(255, 255, 255), 0.20f,
                          Rgb(255, 255, 255), 0.05f);
        }

        // The track is a full ring; the level is painted over it by the sweep.
        DrawRingArc(surface_, ringCx_, ringCy_, radius, thickness, 2.0f * kPi,
                    Rgb(255, 255, 255), 0.20f);

        HDC dc = mask_.Dc();
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));

        if (charging) {
            const float boltX = ringCx_ - boltW * 0.5f;
            const float boltY = ringCy_ - groupH * 0.5f;
            // The frost behind it is only a soft wash over the wallpaper, so a
            // dark copy sits just under the lit bolt to hold its edges against a
            // bright window.
            FillBolt(surface_, boltX, boltY + std::max(1.0f, 2.0f * s), boltW, boltH,
                     Rgb(0, 0, 0), 0.38f);
            FillBolt(surface_, boltX, boltY, boltW, boltH, Rgb(255, 255, 255), 0.96f);
        }

        mask_.Clear();
        HGDIOBJ previous = SelectObject(dc, pctFont);
        TextOutW(dc, static_cast<int>(ringCx_ - pctSize.cx * 0.5f + 0.5f),
                 static_cast<int>(ringCy_ + groupH * 0.5f - glyphH + 0.5f),
                 title.c_str(), static_cast<int>(title.size()));
        GdiFlush();
        SelectObject(dc, previous);
        // The frost is a wash rather than a panel, so the number still carries a
        // dark halo of its own: three passes that keep the white legible over
        // bright wallpaper.
        const int shade = std::max(1, static_cast<int>(1.5f * s + 0.5f));
        CompositeText(Rgb(0, 0, 0, 0.38f), -shade, shade);
        CompositeText(Rgb(0, 0, 0, 0.38f), shade, shade);
        CompositeText(Rgb(0, 0, 0, 0.26f), 0, shade * 2);
        CompositeText(Rgb(255, 255, 255, 0.97f));

        if (!detail.empty()) {
            const int capLeft = shadow + (contentW - captionW) / 2;
            const int capTop = pillTop + pillPadY;
            mask_.Clear();
            previous = SelectObject(dc, capFont);
            TextOutW(dc, capLeft, capTop, detail.c_str(),
                     static_cast<int>(detail.size()));
            GdiFlush();
            SelectObject(dc, previous);
            // The pill already guarantees contrast, so the caption needs no
            // shadow of its own.
            CompositeText(Rgb(255, 255, 255, 0.78f));
        }

        DeleteObject(pctFont);
        DeleteObject(capFont);

        // Keep the static frame, then draw the ring over it, so Tick() can
        // replay the sweep without rasterising the text on every frame.
        base_.CopyFrom(surface_);
        ringReady_ = true;
        RenderRingSweep(RingProgress(NowMs()));
        return PlaceOnScreen(cardW, cardH);
    }

    // PineApple (Card): the charging HUD the way a desktop OS draws one. A
    // plain black slab in dark mode, a plain white one in light mode, with the
    // glyph and the number in one centred column over a capsule bar. No accent
    // tint and no frost behind it, which is the point of the style.
    bool RefreshPineAppleDesktop(const ChargingStats& stats,
                                 const std::wstring& title,
                                 const std::wstring& detail) {
        const float s = scale_;
        const int basePx = std::clamp(style_.fontPx, 8, 64);
        const int titlePx = std::max(18, static_cast<int>(basePx * 2.0f * s + 0.5f));
        const int detailPx = std::max(10, static_cast<int>(basePx * 0.66f * s + 0.5f));
        const bool windows11 = IsWindows11();
        const wchar_t* display = windows11 ? L"Segoe UI Variable Display" : L"Segoe UI";
        const wchar_t* captionFace =
            windows11 ? L"Segoe UI Variable Text" : L"Segoe UI";

        HFONT titleFont =
            CreateFontW(-titlePx, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, display);
        HFONT detailFont =
            CreateFontW(-detailPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, captionFace);
        if (!titleFont || !detailFont) {
            if (titleFont) DeleteObject(titleFont);
            if (detailFont) DeleteObject(detailFont);
            return false;
        }

        SIZE titleSize{};
        SIZE detailSize{};
        SIZE detailReserve{};
        HDC measure = CreateCompatibleDC(nullptr);
        if (measure) {
            HGDIOBJ previous = SelectObject(measure, titleFont);
            GetTextExtentPoint32W(measure, title.c_str(), static_cast<int>(title.size()),
                                  &titleSize);
            if (!detail.empty()) {
                SelectObject(measure, detailFont);
                GetTextExtentPoint32W(measure, detail.c_str(),
                                      static_cast<int>(detail.size()), &detailSize);
                // The tile is sized for the widest line this state can reach, so
                // a longer status line arriving on the next refresh cannot make
                // it square up and move; the line is still drawn centred on its
                // real width.
                const std::wstring reserve = ReserveDetailLine(stats);
                GetTextExtentPoint32W(measure, reserve.c_str(),
                                      static_cast<int>(reserve.size()), &detailReserve);
            }
            SelectObject(measure, previous);
            DeleteDC(measure);
        }

        const bool showDetail = !detail.empty();
        const bool showBar = stats.percent >= 0;
        const int pad = std::max(12, static_cast<int>(22.0f * s + 0.5f));
        const int lineGap = std::max(2, static_cast<int>(4.0f * s + 0.5f));
        const int barH = std::max(4, static_cast<int>(5.0f * s + 0.5f));
        const int barGap = std::max(8, static_cast<int>(22.0f * s + 0.5f));
        const float glyphW = std::max(54.0f, static_cast<float>(basePx) * 3.6f * s);
        const float glyphH = glyphW * 0.44f;

        // The status column is as wide as the longer of the line on screen and
        // the longest line this state can reach.
        const int detailW =
            showDetail ? std::max(static_cast<int>(detailSize.cx),
                                  static_cast<int>(detailReserve.cx))
                       : 0;
        const int contentW = std::max(
            {static_cast<int>(ceilf(glyphW)), static_cast<int>(titleSize.cx), detailW});
        int contentH =
            static_cast<int>(ceilf(glyphH)) + lineGap + static_cast<int>(titleSize.cy);
        if (showDetail) {
            contentH += lineGap + static_cast<int>(detailSize.cy);
        }
        if (showBar) {
            contentH += barGap + barH;
        }

        // Square whatever the content, the way these HUDs are cut, but it grows
        // to fit a long status line or a large font size.
        const int minSide = std::max(104, static_cast<int>(basePx * 5.6f * s + 0.5f));
        const int side = std::max(minSide, std::max(contentW, contentH) + pad * 2);
        if (side <= 0 || side > 2000) {
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            return false;
        }

        const float sigma = 10.0f * s;
        const int shadow = static_cast<int>(ceilf(sigma * 3.0f));
        const int cardW = side + shadow * 2;
        const int cardH = cardW;
        if (!surface_.Resize(cardW, cardH) || !mask_.Resize(cardW, cardH) ||
            !base_.Resize(cardW, cardH)) {
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            return false;
        }

        // Deliberately monochrome: a black slab in dark mode and a white one in
        // light mode, with the glyph, the number and the bar in the opposite
        // colour. Nothing here reads the accent.
        const OverlayTheme theme = MakeOverlayTheme();
        const bool light = theme.light;
        const Color panel = light ? Rgb(247, 247, 249) : Rgb(20, 20, 22);
        const Color ink = light ? Rgb(22, 22, 24) : Rgb(255, 255, 255);
        const Color quiet = Mix(ink, panel, 0.32f);
        const bool low = !stats.onAc && stats.percent >= 0 && stats.percent <= 20;
        const Color warn = light ? Rgb(214, 38, 34) : Rgb(255, 69, 58);
        const Color level = low ? warn : ink;
        rainbow_ = false;

        const float tileX = static_cast<float>(shadow);
        const float tileY = static_cast<float>(shadow);
        const RoundedRect tile =
            MakeRect(tileX, tileY, tileX + static_cast<float>(side),
                     tileY + static_cast<float>(side),
                     static_cast<float>(side) * 0.235f);

        surface_.Clear();
        CastShadow(surface_, tile, sigma, 5.0f * s, light ? 0.20f : 0.42f);
        // One flat coat of the theme colour. Barely translucent, so the desktop
        // tints it the way a system HUD is tinted, but there is no frost and no
        // accent in it.
        const float panelAlpha = light ? 0.98f : 0.96f;
        FillRounded(surface_, tile, panel, panelAlpha, panel, panelAlpha, 0.0f);
        // A hairline edge keeps the slab off the wallpaper without drawing a box
        // around it.
        StrokeRounded(surface_, tile, std::max(1.0f, s), ink, light ? 0.10f : 0.08f,
                      ink, light ? 0.04f : 0.03f);

        const float centerX = tileX + static_cast<float>(side) * 0.5f;
        const float contentTop =
            tileY + (static_cast<float>(side) - static_cast<float>(contentH)) * 0.5f;
        DrawBatteryGlyph(surface_, centerX - glyphW * 0.5f, contentTop, glyphW,
                         glyphH, stats.percent, level, 1.0f);

        HDC dc = mask_.Dc();
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));

        const int textTop = static_cast<int>(contentTop + glyphH + 0.5f) + lineGap;
        mask_.Clear();
        HGDIOBJ previous = SelectObject(dc, titleFont);
        TextOutW(dc, shadow + (side - static_cast<int>(titleSize.cx)) / 2, textTop,
                 title.c_str(), static_cast<int>(title.size()));
        GdiFlush();
        SelectObject(dc, previous);
        CompositeText(ink);

        int cursorY = textTop + static_cast<int>(titleSize.cy);
        if (showDetail) {
            const int detailTop = cursorY + lineGap;
            mask_.Clear();
            previous = SelectObject(dc, detailFont);
            TextOutW(dc, shadow + (side - static_cast<int>(detailSize.cx)) / 2,
                     detailTop, detail.c_str(), static_cast<int>(detail.size()));
            GdiFlush();
            SelectObject(dc, previous);
            // The slab already guarantees contrast, so no shadow is needed.
            CompositeText(quiet);
            cursorY = detailTop + static_cast<int>(detailSize.cy);
        }

        if (showBar) {
            const float barTop = static_cast<float>(cursorY + barGap);
            // The track runs the full width of the slab, the way the system bar
            // does, instead of stopping at the width of the number.
            const float barW = static_cast<float>(side - pad * 2);
            const float barLeft = centerX - barW * 0.5f;
            FillRounded(surface_,
                        MakeRect(barLeft, barTop, barLeft + barW,
                                 barTop + static_cast<float>(barH), barH * 0.5f),
                        ink, light ? 0.12f : 0.20f, ink, light ? 0.12f : 0.20f,
                        0.0f);
            // The track is the static frame; the fill is painted over a copy of
            // it on every frame, exactly like the Fluent level bar.
            barReady_ = true;
            barX_ = barLeft;
            barY_ = barTop;
            barW_ = barW;
            barH_ = static_cast<float>(barH);
            barLevel_ = Clamp01(stats.percent / 100.0f);
            barColor_ = level;
            barTopColor_ = level;
            barSweep_ = false;
        }

        DeleteObject(titleFont);
        DeleteObject(detailFont);

        if (barReady_) {
            base_.CopyFrom(surface_);
            RenderBarFill(BarProgress(NowMs()));
        }
        return PlaceOnScreen(cardW, cardH);
    }

    // Galaxy: the phone-vendor charging card. A wide, heavily rounded slab holds
    // the percentage as the hero with a bolt in an accent round badge beside it,
    // the status line under the number and one slim level track across the
    // bottom, which fills the way the Fluent bar does.
    bool RefreshGalaxy(const ChargingStats& stats, const std::wstring& title,
                       const std::wstring& detail) {
        const float s = scale_;
        const int basePx = std::clamp(style_.fontPx, 8, 64);
        const int titlePx = std::max(16, static_cast<int>(basePx * 2.0f * s + 0.5f));
        const int detailPx = std::max(9, static_cast<int>(basePx * 0.62f * s + 0.5f));
        const bool windows11 = IsWindows11();
        const wchar_t* display = windows11 ? L"Segoe UI Variable Display" : L"Segoe UI";
        const wchar_t* captionFace =
            windows11 ? L"Segoe UI Variable Text" : L"Segoe UI";

        HFONT titleFont =
            CreateFontW(-titlePx, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, display);
        HFONT detailFont =
            CreateFontW(-detailPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, captionFace);
        if (!titleFont || !detailFont) {
            if (titleFont) DeleteObject(titleFont);
            if (detailFont) DeleteObject(detailFont);
            return false;
        }

        SIZE titleSize{};
        SIZE detailSize{};
        SIZE detailReserve{};
        HDC measure = CreateCompatibleDC(nullptr);
        if (measure) {
            HGDIOBJ previous = SelectObject(measure, titleFont);
            GetTextExtentPoint32W(measure, title.c_str(), static_cast<int>(title.size()),
                                  &titleSize);
            if (!detail.empty()) {
                SelectObject(measure, detailFont);
                GetTextExtentPoint32W(measure, detail.c_str(),
                                      static_cast<int>(detail.size()), &detailSize);
                // The card is cut for the widest status line this state can
                // reach, so the estimate and the wattage landing later cannot
                // widen it and shove it across the screen.
                const std::wstring reserve = ReserveDetailLine(stats);
                GetTextExtentPoint32W(measure, reserve.c_str(),
                                      static_cast<int>(reserve.size()), &detailReserve);
            }
            SelectObject(measure, previous);
            DeleteDC(measure);
        }

        const bool showDetail = !detail.empty();
        const bool showBar = stats.percent >= 0;
        // A little roomier than the Fluent card: the Galaxy slab reads better
        // with air around the badge, the number and the bar.
        const int pad = std::max(13, static_cast<int>(22.0f * s + 0.5f));
        const int gap = std::max(6, static_cast<int>(13.0f * s + 0.5f));
        const int lineGap = std::max(1, static_cast<int>(4.0f * s + 0.5f));
        const int barH = std::max(3, static_cast<int>(4.0f * s + 0.5f));
        const int barGap = std::max(6, static_cast<int>(14.0f * s + 0.5f));
        // The badge is a touch taller than the number's own line, so the bolt
        // sits level with the digits instead of floating above them.
        const int badge = std::max(20, static_cast<int>(titleSize.cy * 1.12f + 0.5f));
        const int detailW =
            showDetail ? std::max(static_cast<int>(detailSize.cx),
                                  static_cast<int>(detailReserve.cx))
                       : 0;
        const int textW =
            std::max(static_cast<int>(titleSize.cx), detailW);
        // The card is cut wide on purpose: the number sits on a slab with room
        // to breathe rather than hugging its own text like the Fluent card.
        const int contentW =
            std::max(badge + gap + textW,
                     std::max(120, static_cast<int>(basePx * 7.5f * s + 0.5f)));
        const int rowH = std::max(badge, static_cast<int>(titleSize.cy));
        int contentH = rowH;
        if (showDetail) {
            contentH += lineGap + static_cast<int>(detailSize.cy);
        }
        if (showBar) {
            contentH += barGap + barH;
        }
        const int cardW = contentW + pad * 2;
        const int cardH = contentH + pad * 2;
        if (cardW <= 0 || cardH <= 0 || cardW > 4000 || cardH > 2000) {
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            return false;
        }

        const float sigma = 9.0f * s;
        const int shadow = static_cast<int>(ceilf(sigma * 3.0f));
        const int width = cardW + shadow * 2;
        const int height = cardH + shadow * 2;
        if (!surface_.Resize(width, height) || !mask_.Resize(width, height) ||
            !base_.Resize(width, height)) {
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            return false;
        }

        const Color accent =
            ResolveOverlayAccent(style_.color, style_.customColor);
        const GlowPalette gradient = MakeRainbowPalette();
        const GlowPalette* sweep = style_.rainbow ? &gradient : nullptr;
        const OverlayTheme theme = MakeOverlayTheme();
        // Nearly a third of the height: that soft slab is what separates this
        // card from the tighter Fluent panel and the squared-off HUD tile.
        const float radius = std::max(10.0f, static_cast<float>(cardH) * 0.32f);
        const RoundedRect card =
            MakeRect(static_cast<float>(shadow), static_cast<float>(shadow),
                     static_cast<float>(shadow + cardW),
                     static_cast<float>(shadow + cardH), radius);

        surface_.Clear();
        CastShadow(surface_, card, sigma, 5.0f * s, theme.shadowAlpha);

        const float hairline = std::max(1.0f, s);
        StrokeRounded(surface_, card, hairline, theme.hairline, theme.hairlineTop,
                      theme.hairline, theme.hairlineBottom);

        // A Galaxy surface is near opaque rather than acrylic, so the blur is
        // covered almost completely and only the accent wash survives over it.
        const RoundedRect panel =
            MakeRect(card.x0 + hairline, card.y0 + hairline, card.x1 - hairline,
                     card.y1 - hairline, std::max(8.0f, radius - hairline));
        FillGlass(surface_, panel, backdrop_, theme.panelTop, theme.panelAlpha,
                  accent, theme.tintTop);
        FillRounded(surface_, panel, Mix(theme.panelTop, accent, theme.tintTop),
                    0.30f, Mix(theme.panelBottom, accent, theme.tintBottom), 0.34f,
                    0.010f);

        const float badgeX = static_cast<float>(shadow + pad);
        const float badgeY = static_cast<float>(shadow + pad) +
                             (static_cast<float>(rowH) - badge) * 0.5f;
        FillRounded(surface_,
                    MakeRect(badgeX, badgeY, badgeX + badge, badgeY + badge,
                             static_cast<float>(badge) * 0.5f),
                    accent, 0.26f, accent, 0.13f, 0.0f, sweep);

        // The bolt only shows while the battery is actually taking charge; on a
        // plain unplug the badge carries the battery it came from.
        const bool charging =
            stats.charging || (stats.onAc && stats.percent >= 0 && stats.percent < 100);
        const Color ink = theme.light ? Mix(accent, Rgb(24, 24, 26), 0.35f)
                                      : Mix(Rgb(255, 255, 255), accent, 0.25f);
        if (charging) {
            const float boltH = static_cast<float>(badge) * 0.46f;
            const float boltW = boltH * 0.60f;
            FillBolt(surface_, badgeX + (badge - boltW) * 0.5f,
                     badgeY + (badge - boltH) * 0.5f, boltW, boltH, ink, 0.96f);
        } else {
            const float glyphW = static_cast<float>(badge) * 0.54f;
            const float glyphH = glyphW * 0.46f;
            DrawBatteryGlyph(surface_, badgeX + (badge - glyphW) * 0.5f,
                             badgeY + (badge - glyphH) * 0.5f, glyphW, glyphH,
                             stats.percent, ink, 0.94f);
        }

        HDC dc = mask_.Dc();
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));

        const int textLeft = shadow + pad + badge + gap;
        const int titleTop = shadow + pad + (rowH - static_cast<int>(titleSize.cy)) / 2;
        mask_.Clear();
        HGDIOBJ previous = SelectObject(dc, titleFont);
        TextOutW(dc, textLeft, titleTop, title.c_str(), static_cast<int>(title.size()));
        GdiFlush();
        SelectObject(dc, previous);
        CompositeText(theme.title);

        int cursorY = shadow + pad + rowH;
        if (showDetail) {
            const int detailTop = cursorY + lineGap;
            mask_.Clear();
            previous = SelectObject(dc, detailFont);
            TextOutW(dc, textLeft, detailTop, detail.c_str(),
                     static_cast<int>(detail.size()));
            GdiFlush();
            SelectObject(dc, previous);
            CompositeText(theme.detail);
            cursorY = detailTop + static_cast<int>(detailSize.cy);
        }

        if (showBar) {
            const float barTop = static_cast<float>(cursorY + barGap);
            const float barLeft = static_cast<float>(shadow + pad);
            FillRounded(surface_,
                        MakeRect(barLeft, barTop, barLeft + contentW, barTop + barH,
                                 barH * 0.5f),
                        theme.track, 0.13f, theme.track, 0.09f, 0.0f);
            // The track is the static frame and the fill grows over a copy of it,
            // exactly like the Fluent bar.
            barReady_ = true;
            barX_ = barLeft;
            barY_ = barTop;
            barW_ = static_cast<float>(contentW);
            barH_ = static_cast<float>(barH);
            barLevel_ = Clamp01(stats.percent / 100.0f);
            Color bar = theme.light ? Mix(accent, Rgb(16, 16, 18), 0.14f) : accent;
            // The low battery warning keeps its red even when the wave is a
            // gradient, so the shape stops sampling the palette in that case.
            const GlowPalette* barSweep = sweep;
            if (!stats.onAc && stats.percent <= 20) {
                bar = Mix(Rgb(232, 84, 66), accent, 0.2f);
                barSweep = nullptr;
            }
            barTopColor_ = theme.light ? bar : Mix(bar, Rgb(255, 255, 255), 0.15f);
            barColor_ = bar;
            barSweep_ = barSweep != nullptr;
        }

        DeleteObject(titleFont);
        DeleteObject(detailFont);

        // Keep the track-only frame and put the fill back on at the beat the card
        // is currently on, so a refresh mid-animation does not snap it to full.
        if (barReady_) {
            base_.CopyFrom(surface_);
            RenderBarFill(BarProgress(NowMs()));
        }
        return PlaceOnScreen(width, height);
    }

    // Centres the surface on the anchor settled when the card appeared, and
    // keeps it on that monitor, clear of the taskbar.
    bool PlaceOnScreen(int width, int height) {
        const RECT monitor = UsableRect(anchor_);
        const int margin = std::max(8, static_cast<int>(28.0f * scale_ + 0.5f));
        const int minLeft = static_cast<int>(monitor.left) + margin;
        const int minTop = static_cast<int>(monitor.top) + margin;
        const int maxLeft =
            std::max(minLeft, static_cast<int>(monitor.right) - margin - width);
        const int maxTop =
            std::max(minTop, static_cast<int>(monitor.bottom) - margin - height);
        left_ = std::clamp(static_cast<int>(anchor_.x) - width / 2, minLeft, maxLeft);
        top_ = std::clamp(static_cast<int>(anchor_.y) - height / 2, minTop, maxTop);
        return true;
    }

    LayeredWindow window_;
    Surface surface_;
    Surface mask_;
    Surface base_;
    // Blurred copy of the screen behind the card, and the scaled frame the
    // spring presents while the card is still growing.
    Surface backdrop_;
    Surface scaled_;
    bool springy_ = false;
    // The HUD themes, the PineApple card and the Galaxy card, fade and scale
    // instead of sliding.
    bool hud_ = false;
    bool ringReady_ = false;
    float ringCx_ = 0.0f;
    float ringCy_ = 0.0f;
    float ringRadius_ = 0.0f;
    float ringThickness_ = 0.0f;
    float ringSweep_ = 0.0f;
    Color ringColor_{};
    // The level arc samples the rolling gradient instead of ringColor_.
    bool rainbow_ = false;
    // Fluent level bar: the empty track is part of the static frame and the fill
    // grows over a copy of it, up to barLevel_ of the full width.
    bool barReady_ = false;
    float barX_ = 0.0f;
    float barY_ = 0.0f;
    float barW_ = 0.0f;
    float barH_ = 0.0f;
    float barLevel_ = 0.0f;
    Color barTopColor_{};
    Color barColor_{};
    bool barSweep_ = false;
    OverlayStyle style_;
    // Where the card sits while it is up, settled once when it appears.
    POINT anchor_{};
    bool visible_ = false;
    bool closing_ = false;
    float scale_ = 1.0f;
    int left_ = 0;
    int top_ = 0;
    double shownAt_ = 0;
    double hideAt_ = 0;
    double closingAt_ = 0;
    double statsAt_ = 0;
};

StatsOverlay g_stats;

////////////////////////////////////////////////////////////////////////////////
// Effect Coordination

HANDLE g_rippleTimer = NULL;
bool g_highResolutionTimer = false;
bool g_rippleTimerArmed = false;

// The card is laid out and animated on the message thread, which owns the
// overlay state. Power notifications arrive on the notification thread, so they
// hand the request over instead of touching the overlay directly.
std::mutex g_statsRequestMtx;
OverlayStyle g_statsRequest;
POINT g_statsRequestOrigin{};
bool g_statsRequestPending = false;

void ArmRippleTimer(double delayMs) {
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -std::max<LONGLONG>(1, static_cast<LONGLONG>(delayMs * 10000.0));
    SetWaitableTimer(g_rippleTimer, &dueTime, 0, nullptr, nullptr, FALSE);
    g_rippleTimerArmed = true;
}

void RequestRippleTick() {
    if (!g_rippleTimer || g_rippleTimerArmed) {
        return;
    }
    ArmRippleTimer(g_ripple.FrameMs());
}

void OnRippleTimer() {
    g_rippleTimerArmed = false;
    const double now = NowMs();
    const double next = g_ripple.Tick(now);
    if (next < 1e8) {
        const double finished = NowMs();
        ArmRippleTimer(std::max(next - (finished - now), 0.0));
    } else {
        // The timer is manual-reset, so clear it or the loop would spin.
        CancelWaitableTimer(g_rippleTimer);
    }
}

void OnRippleCapture(std::unique_ptr<RippleCaptureMessage> message) {
    if (!g_ripple.Owns(message->capture)) {
        return;
    }
    if (message->failed) {
        g_ripple.Stop();
        return;
    }
    if (message->first && !g_ripple.Active()) {
        g_ripple.Launch(NowMs(), g_highResolutionTimer ? 0.0 : kFrameMs);
        RequestRippleTick();
    }
}

void RunChargingEffect(bool pluggedIn) {
    if (ShouldSuppressEffects()) {
        return;
    }

    bool showRipple;
    std::wstring preset;
    std::wstring custom;
    std::wstring origin;
    std::wstring direction;
    RippleStyle style;
    bool showStats;
    OverlayStyle overlay;
    {
        std::lock_guard<std::mutex> lock(g_settings.mtx);
        showRipple = pluggedIn ? g_settings.pluggedInRipple
                               : g_settings.unpluggedRipple;
        preset = pluggedIn ? g_settings.pluggedInRippleColor
                           : g_settings.unpluggedRippleColor;
        custom = pluggedIn ? g_settings.pluggedInCustomColor
                           : g_settings.unpluggedCustomColor;
        direction = g_settings.unpluggedRippleDirection;
        origin = g_settings.rippleOrigin;
        style.durationMs = std::clamp(g_settings.rippleDuration, 300, 5000);
        style.strength = static_cast<float>(std::clamp(g_settings.rippleStrength, 0, 40));
        style.opacity = std::clamp(g_settings.rippleOpacity, 0, 100) / 100.0f;
        style.dim = std::clamp(g_settings.dim, 0, 90) / 100.0f;
        style.curve = ParseRippleCurve(g_settings.rippleCurve);
        style.effect = ParseRippleEffect(g_settings.rippleStyle);
        showStats = pluggedIn ? g_settings.statsPluggedIn
                              : g_settings.statsUnplugged;
        overlay.theme = g_settings.statsTheme;
        overlay.position = g_settings.statsPosition;
        overlay.color = g_settings.statsColor;
        overlay.customColor = g_settings.statsCustomColor;
        overlay.details = g_settings.statsDetails;
        overlay.durationMs = std::clamp(g_settings.statsDuration, 500, 15000);
        overlay.opacity = std::clamp(g_settings.statsOpacity, 0, 100);
        overlay.fontPx = std::clamp(g_settings.statsFontSize, 8, 64);
    }

    // Only the unplug event offers the reverse wave, which collapses toward the
    // origin. The old inward/outward keys are still honoured so a saved choice
    // survives the rename.
    style.reverse = !pluggedIn &&
                    (KeyIs(direction, L"reverse") || KeyIs(direction, L"inward"));

    g_glow = ResolveRippleColor(preset, custom,
                                pluggedIn ? kGlowBlue : kGlowRed);

    // The card lives in its own window above the wave, so it is never shaded by
    // the ripple. When it follows a gradient ripple it therefore draws the same
    // gradient itself, and only when the colour setting really follows the wave.
    overlay.rainbow = g_glow.rainbow &&
                      (overlay.color.empty() || KeyIs(overlay.color, L"ripple"));

    PlayChargingSound(pluggedIn);

    const POINT pt = ResolveRippleOrigin(origin);

    // Start the scene effect first, so the card is mostly absent from the ripple's
    // snapshot and the edge glow is already lit when the card grabs its backdrop.
    if (!showRipple) {
        Wh_Log(L"Scene effect disabled for this power event, skipping the animation.");
    } else if (!g_rippleTimer) {
        // Without the timer the effect could never advance, leaving it on screen.
        Wh_Log(L"No waitable timer, skipping the scene effect.");
    } else {
        g_ripple.SetStyle(style);
        if (!g_ripple.Prepare(pt, DpiScaleAt(pt), g_messageWindow)) {
            Wh_Log(L"Could not start the scene effect.");
        } else if (g_ripple.Active()) {
            // The border effects have nothing to capture, so they are already
            // running and only need the timer that keeps their frames coming.
            RequestRippleTick();
        }
    }

    if (showStats && g_messageWindow) {
        {
            std::lock_guard<std::mutex> lock(g_statsRequestMtx);
            g_statsRequest = overlay;
            g_statsRequestOrigin = pt;
            g_statsRequestPending = true;
        }
        PostMessageW(g_messageWindow, WM_APP_STATS, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Power Notification Thread & Window Proc (Adapted from Charging Sound)

LRESULT CALLBACK PowerWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_TIMER && wParam == StatsOverlay::kTimerId) {
        g_stats.Tick();
        return 0;
    }
    if (uMsg == WM_APP_STATS) {
        OverlayStyle style;
        POINT origin{};
        bool pending = false;
        {
            std::lock_guard<std::mutex> lock(g_statsRequestMtx);
            pending = g_statsRequestPending;
            g_statsRequestPending = false;
            style = g_statsRequest;
            origin = g_statsRequestOrigin;
        }
        if (pending) {
            g_stats.Show(origin, style);
        }
        return 0;
    }
    if (uMsg == WM_APP_DISPLAY) {
        // Anything already on screen was laid out for the old display, so it
        // goes rather than staying behind on the wrong rectangle.
        g_ripple.Stop();
        g_stats.Hide();
        return 0;
    }
    if (uMsg == WM_APP_RIPPLE) {
        OnRippleCapture(std::unique_ptr<RippleCaptureMessage>(
            reinterpret_cast<RippleCaptureMessage*>(lParam)));
        return 0;
    }
    if (uMsg == WM_POWERBROADCAST && wParam == PBT_POWERSETTINGCHANGE) {
        POWERBROADCAST_SETTING* pbs = (POWERBROADCAST_SETTING*)lParam;
        if (pbs && pbs->PowerSetting == GUID_ACDC_POWER_SOURCE && pbs->DataLength == sizeof(DWORD)) {
            DWORD currentACStatus;
            std::memcpy(&currentACStatus, pbs->Data, sizeof(DWORD));

            int state = (currentACStatus == PoAc) ? PoAc : PoDc;
            int previous = std::exchange(g_lastPowerCondition, state);
            if (previous == state || previous == -1) {
                return TRUE;
            }

            Wh_Log(L"Power status change detected. AC Status: %u", currentACStatus);
            RunChargingEffect(state == PoAc);
        }
        return TRUE;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI PowerNotifyThread(LPVOID lpParam) {
    Wh_Log(L"PowerNotifyThread started.");

    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps) && sps.ACLineStatus != 255) {
        g_lastPowerCondition = (sps.ACLineStatus == 1) ? PoAc : PoDc;
    }

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    WNDCLASSEXW wc{0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = PowerWindowProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"WindhawkChargingAnimationHiddenWindow";

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return 1;
    }

    WNDCLASSEXW rippleClass{0};
    rippleClass.cbSize = sizeof(WNDCLASSEXW);
    rippleClass.lpfnWndProc = RippleWndProc;
    rippleClass.hInstance = wc.hInstance;
    rippleClass.lpszClassName = kRippleWindowClass;

    if (!RegisterClassExW(&rippleClass)) {
        Wh_Log(L"RegisterClassExW for the ripple failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    WNDCLASSEXW scrimClass{0};
    scrimClass.cbSize = sizeof(WNDCLASSEXW);
    scrimClass.lpfnWndProc = ScrimWndProc;
    scrimClass.hInstance = wc.hInstance;
    scrimClass.lpszClassName = kScrimWindowClass;

    if (!RegisterClassExW(&scrimClass)) {
        Wh_Log(L"RegisterClassExW for the dim failed: %u", GetLastError());
        UnregisterClassW(kRippleWindowClass, wc.hInstance);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"WindhawkChargingAnimation", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    if (hwnd) {
        g_messageWindow = hwnd;
        HPOWERNOTIFY hPowerNotify = RegisterPowerSettingNotification(hwnd, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);

        g_rippleTimer = CreateWaitableTimerExW(nullptr, nullptr,
                                               CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                               TIMER_ALL_ACCESS);
        g_highResolutionTimer = g_rippleTimer != nullptr;
        if (!g_rippleTimer) {
            g_rippleTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
        }

        if (g_rippleTimer) {
            const HANDLE timer = g_rippleTimer;
            bool running = true;
            while (running) {
                const DWORD result = MsgWaitForMultipleObjectsEx(1, &timer, INFINITE,
                                                                 QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (result == WAIT_OBJECT_0) {
                    OnRippleTimer();
                } else if (result > WAIT_OBJECT_0 + 1) {
                    Wh_Log(L"MsgWaitForMultipleObjectsEx failed: %u", GetLastError());
                    break;
                }
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT) {
                        running = false;
                        break;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                if (g_stopping) {
                    running = false;
                }
            }
        } else {
            while (GetMessageW(&msg, NULL, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        g_ripple.Destroy();
        g_stats.Destroy();
        if (g_rippleTimer) {
            CloseHandle(g_rippleTimer);
            g_rippleTimer = nullptr;
        }
        if (hPowerNotify) UnregisterPowerSettingNotification(hPowerNotify);
        g_messageWindow = nullptr;
        DestroyWindow(hwnd);
    }

    UnregisterClassW(kScrimWindowClass, wc.hInstance);
    UnregisterClassW(kRippleWindowClass, wc.hInstance);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    Wh_Log(L"PowerNotifyThread exited.");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mod Settings & Initialization

void LoadSettings() {
    auto pluggedIn = WindhawkUtils::StringSetting::make(L"Sounds.WhenPluggedIn");
    auto unplugged = WindhawkUtils::StringSetting::make(L"Sounds.WhenUnplugged");
    auto pluggedInColor =
        WindhawkUtils::StringSetting::make(L"BackgroundEffect.PluggedInColor");
    auto pluggedInCustom =
        WindhawkUtils::StringSetting::make(L"BackgroundEffect.PluggedInCustomColor");
    auto unpluggedColor =
        WindhawkUtils::StringSetting::make(L"BackgroundEffect.UnpluggedColor");
    auto unpluggedDirection =
        WindhawkUtils::StringSetting::make(L"BackgroundEffect.UnplugDirection");
    auto unpluggedCustom =
        WindhawkUtils::StringSetting::make(L"BackgroundEffect.UnpluggedCustomColor");
    auto rippleOrigin = WindhawkUtils::StringSetting::make(L"BackgroundEffect.Origin");
    auto rippleCurve = WindhawkUtils::StringSetting::make(L"BackgroundEffect.Motion");
    auto rippleStyle = WindhawkUtils::StringSetting::make(L"BackgroundEffect.Animation");
    auto statsTheme = WindhawkUtils::StringSetting::make(L"StatsPanel.Style");
    auto statsPosition = WindhawkUtils::StringSetting::make(L"StatsPanel.Position");
    auto statsColor = WindhawkUtils::StringSetting::make(L"StatsPanel.AccentColor");
    auto statsCustomColor =
        WindhawkUtils::StringSetting::make(L"StatsPanel.CustomAccentColor");
    bool suppress = Wh_GetIntSetting(L"QuietMode") != 0;
    bool pluggedInRipple = Wh_GetIntSetting(L"BackgroundEffect.WhenPluggedIn") != 0;
    bool unpluggedRipple = Wh_GetIntSetting(L"BackgroundEffect.WhenUnplugged") != 0;
    int rippleStrength = Wh_GetIntSetting(L"BackgroundEffect.Strength");
    int rippleOpacity = Wh_GetIntSetting(L"BackgroundEffect.Opacity");
    int rippleDuration = Wh_GetIntSetting(L"BackgroundEffect.Duration");
    int dim = Wh_GetIntSetting(L"BackgroundEffect.Dim");
    bool statsPluggedIn = Wh_GetIntSetting(L"StatsPanel.WhenPluggedIn") != 0;
    bool statsUnplugged = Wh_GetIntSetting(L"StatsPanel.WhenUnplugged") != 0;
    bool statsDetails = Wh_GetIntSetting(L"StatsPanel.Details") != 0;
    int statsDuration = Wh_GetIntSetting(L"StatsPanel.Duration");
    int statsOpacity = Wh_GetIntSetting(L"StatsPanel.Opacity");
    int statsFontSize = Wh_GetIntSetting(L"StatsPanel.FontSize");

    std::lock_guard<std::mutex> lock(g_settings.mtx);
    g_settings.pluggedInSoundPath = pluggedIn.get();
    g_settings.unpluggedSoundPath = unplugged.get();
    g_settings.suppressWhenBusy = suppress;
    g_settings.pluggedInRipple = pluggedInRipple;
    g_settings.pluggedInRippleColor = pluggedInColor.get();
    g_settings.pluggedInCustomColor = pluggedInCustom.get();
    g_settings.unpluggedRipple = unpluggedRipple;
    g_settings.unpluggedRippleColor = unpluggedColor.get();
    g_settings.unpluggedRippleDirection = unpluggedDirection.get();
    g_settings.unpluggedCustomColor = unpluggedCustom.get();
    g_settings.rippleOrigin = rippleOrigin.get();
    g_settings.rippleStrength = rippleStrength;
    g_settings.rippleOpacity = rippleOpacity;
    g_settings.rippleDuration = rippleDuration;
    g_settings.rippleCurve = rippleCurve.get();
    g_settings.rippleStyle = rippleStyle.get();
    g_settings.dim = dim;
    g_settings.statsPluggedIn = statsPluggedIn;
    g_settings.statsUnplugged = statsUnplugged;
    g_settings.statsTheme = statsTheme.get();
    g_settings.statsPosition = statsPosition.get();
    g_settings.statsColor = statsColor.get();
    g_settings.statsCustomColor = statsCustomColor.get();
    g_settings.statsDetails = statsDetails;
    g_settings.statsDuration = statsDuration;
    g_settings.statsOpacity = statsOpacity;
    g_settings.statsFontSize = statsFontSize;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Charging Animation Init");
    LoadSettings();

    g_instance = GetModuleHandleW(nullptr);
    g_stopping = false;

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;

    g_hThread = CreateThread(NULL, 0, PowerNotifyThread, NULL, 0, &g_threadId);
    if (!g_hThread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Charging Animation Uninit");
    g_stopping = true;
    PlaySoundW(nullptr, nullptr, 0);

    if (g_threadId != 0 && g_hThread != NULL) {
        if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
        // The loop watches its message queue as well as its timer, so a posted
        // quit wakes it. The timer itself is left alone here: it belongs to
        // that thread, which closes it on the way out.
        if (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0)) {
            Wh_Log(L"Could not post the quit to the message thread: %u", GetLastError());
        }
        if (WaitForSingleObject(g_hThread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"The message thread did not exit in time.");
        }
        CloseHandle(g_hThread);
        g_hThread = NULL;
        g_threadId = 0;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Tool Mod Launcher Boilerplate

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) return FALSE;

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolMod = false;

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) isExcluded = true;
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolMod = true;
        }
    }
    LocalFree(argv);

    if (isExcluded) return FALSE;

    if (isCurrentToolMod) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((BYTE*)dos + dos->e_lfanew);
        Wh_SetFunctionHook((BYTE*)dos + nt->OptionalHeader.AddressOfEntryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) return;

    WCHAR cmd[MAX_PATH + 64];
    swprintf_s(cmd, L"\"%s\" -tool-mod \"%s\"", path, WH_MOD_ID);

    HMODULE kernel = GetModuleHandle(L"kernelbase.dll");
    if (!kernel) kernel = GetModuleHandle(L"kernel32.dll");
    if (!kernel) return;

    using CreateProcessInternalW_t = BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto pCreateProcessInternalW = (CreateProcessInternalW_t)GetProcAddress(kernel, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) return;

    STARTUPINFO si{.cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK};
    PROCESS_INFORMATION pi;
    if (pCreateProcessInternalW(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}
