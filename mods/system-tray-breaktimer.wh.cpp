// ==WindhawkMod==
// @id              system-tray-breaktimer
// @name            System Tray BreakTimer
// @description     Count down to windowed breaks in the notification area with a circular break timer and optional wind chimes.
// @version         0.1.7
// @author          haniobaid
// @github          https://github.com/haniobaid
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ldwmapi -lwinmm -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System Tray BreakTimer

Each time you enable the mod, it starts paused. Click **P** in the notification
area to start. Windows can place new icons in the tray overflow.

![Break window with a circular countdown and examples of the five tray states](https://raw.githubusercontent.com/haniobaid/windhawk-mods/system-tray-breaktimer-media/system-tray-breaktimer.png)

**S** means Standby until the next whole clock minute. A fresh interval started
at exactly :00 begins immediately. With the default 15-minute interval, starting
at 1:01:25 shows S until 1:02:00. The tray shows **2** at 1:15:59, then
**60** in the seconds color at 1:16:00. It counts down to **1** in seconds.
Set **Start seconds countdown** to any value from 5 to 60 seconds. For example,
at 10 seconds the tray shows **1** minute until 1:16:50. The break starts at 1:17:00
and lasts one minute by default. **B** means a break is in progress. Hover over
the icon for the unit, time remaining and pause reason. Both minutes and
seconds use bold numerals in your chosen tray colors.

The borderless break window has a **Skip Break** button and a gauge-colored
**X** in its upper-right corner. Both end the break silently and start a fresh work interval with the usual
Standby. The gauge scales to 90% of the shorter usable monitor dimension.
With multiple windows, one action applies to them all. To skip with the
keyboard, Alt+Tab to the break window marked **B** and press Enter on **Skip Break**.

**Tray commands**

A single left-click toggles pause. Pausing discards the current interval, and
unpausing starts a fresh one. Turn on **Require a double-click to pause or
unpause** if you prefer. Keyboard activation still takes one press. Right-click
the tray icon to open its menu.

When the tray shows **P**, the menu contains only **Unpause**.
During **S / Standby** or
**B / On break**, the menu contains only **Pause**. During the work countdown,
including its final seconds, the menu has three commands.

* **Skip to next break** starts a break immediately. Its end schedules a fresh
  work interval.
* **Reset** ends the current phase and starts a fresh work interval.
* **Pause** discards the current countdown, closes any break windows and
  leaves the mod paused indefinitely until you unpause it. When you unpause,
  **S** appears until the next whole minute, then the entire **Break every**
  interval begins. At exactly :00 it starts immediately.

**Sound** — In Settings, **Play wind chimes** is on by default. The default
audio device plays one short chime for each of these two events.

* The tray countdown reaches zero and a scheduled break begins.
* The gauge reaches 00:00 and the break finishes naturally.

Choosing **Skip to next break** starts the break silently. It still chimes if
the gauge reaches zero. **Skip Break**, **X**, **Reset**, **Pause** and **Unpause**
are silent. Each event plays its cue once across all monitors. Turning sound
off also stops a cue already playing.

The bundled wind chime was recorded by [GnoteSoundz](https://freesound.org/people/GnoteSoundz/sounds/169855/)
and is available under [CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/).
To use your own recording, browse to a local WAV under **Custom sound file (optional)**.
Choose uncompressed 8- or 16-bit PCM, mono or stereo, at 8–48 kHz, up to
10 seconds and 2 MiB. An empty field or an unreadable or unsupported file
uses the bundled chime. On Windhawk 1.x, you can paste a quoted path copied
from Explorer. If you replace the WAV at the same path, switch sound off and
back on to load it again.

**Settings** — Open the mod's Details, then Settings. Windhawk 2.0 alpha 8
shows break durations as minute sliders and includes a file browser. Windhawk 1.x
uses numeric fields for durations and a text field for the WAV path. If
**Break every** is below twice **Break for**, the timer uses twice the break
length. For example, a 25-minute break and a 40-minute interval give 50 minutes of work.
Changes to durations apply on the next fresh interval or break. Appearance
changes take effect immediately. Set **Background opacity** under **Tray icon**
to 0% for a transparent background or to 100% for a solid color. After an
update, the saved opacity slider value controls the background.
Windows controls whether the full-screen
pause banner appears. The timer pauses independently of banner delivery.

After five continuous idle minutes, an active work schedule pauses. Activity
starts a fresh interval. A manual pause is retained. Two separate switches
control full-screen auto-pause and its optional banner. Both switches default
to off. With auto-pause off, the break window can appear over a full-screen game
or presentation and may interrupt exclusive full screen. Enable auto-pause to
hold the timer while a full-screen app is detected. Leaving full screen automatically
resumes the interrupted timer when other pause reasons have cleared. Choosing
Unpause overrides the current automatic episode and starts a fresh work cycle.

Locking the PC during a break hides the window and freezes the gauge. After
unlocking, the window returns with the time that was left once other automatic
pause conditions clear. Disconnecting the session or suspending the PC also
holds an active break. Ordinary idle time during a break lets the gauge run.

Inspired by [Stretchly](https://github.com/hovancik/stretchly). Functionality inspiration only. No code was reused.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- schedule:
  - breakFor: 1
    $name: Break for (minutes)
    $description: Duration of a break, from 1 to 60 minutes.
    #! $min: 1
    #! $max: 60
    #! $format: slider
  - breakEvery: 15
    $name: Break every (minutes)
    $description: Work time between breaks, in minutes. If you choose less than twice Break for, the timer uses twice the break length.
    #! $min: 10
    #! $max: 300
    #! $format: slider
  $name: Schedule
- behavior:
  - idle: true
    $name: Monitor idle time
    $description: After five minutes away during work, start a fresh interval when input resumes.
  - doubleClick: false
    $name: Require a double-click to pause or unpause
    $description: Use two left-clicks on the tray icon. Keyboard activation and the right-click menu work as usual.
  - allMonitors: false
    $name: Show breaks on all monitors
  - fullScreen: false
    $name: Auto-pause for full-screen apps
  - banner: false
    $name: Notify on full-screen pause
    $description: Request a quiet Windows banner. Auto-pause works even if Windows suppresses it.
  - sound: true
    $name: Play wind chimes
    $description: Chime when the tray countdown reaches zero and when the break gauge reaches zero. Starting a break manually or ending it early is silent.
  - soundFile: ""
    $name: Custom sound file (optional)
    $description: Choose a local PCM WAV, 8- or 16-bit, mono or stereo, 8–48 kHz, up to 10 seconds and 2 MiB. Leave empty for the built-in chime. An unreadable or unsupported file also uses the built-in chime.
    #! $format: filePath
  $name: Behavior
- tray:
  - countdownSeconds: 60
    $name: Start seconds countdown
    $description: Seconds before the break to switch from minutes to seconds. At 60, the tray goes directly from 2 minutes to 60 seconds.
    #! $min: 5
    #! $max: 60
    #! $format: slider
  - normal: "#FFB454"
    $name: Normal text
    #! $format: colorRgb
  - final: "#FF0000"
    $name: Final-seconds text
    #! $format: colorRgb
  - background: "#202832"
    $name: Background color
    #! $format: colorRgb
  - opacity: 0
    $name: Background opacity
    $description: 0% is fully transparent. Increase the opacity to show the chosen background color.
    #! $min: 0
    #! $max: 100
    #! $format: slider
  $name: Tray icon
- window:
  - gauge: "#58D68D"
    $name: Gauge progress
    #! $format: colorRgb
  - text: "#FFFFFF"
    $name: Remaining time
    #! $format: colorRgb
  - background: "#182028"
    $name: Background color
    #! $format: colorRgb
  - opacity: 95
    $name: Window opacity
    $description: Opacity of the entire break window (0–100%). At 0% use the tray to pause the break or return here to restore visibility.
    #! $min: 0
    #! $max: 100
    #! $format: slider
  $name: Break window
*/
// ==/WindhawkModSettings==

#include <windowsx.h>
#include <dwmapi.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <wtsapi32.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <utility>

#include <windhawk_utils.h>
#if __has_include(<windhawk_tool_mod.h>)
#include <windhawk_tool_mod.h>
#endif

// Tick time measures elapsed duration. Wall time supplies the local minute boundary.
namespace Timing {
using Millis = std::int64_t;
constexpr Millis minute = 60000;
struct Time {
    Millis tick;
    Millis wall;
};
Time after(Time t, Millis delay) {
    return {t.tick + delay, t.wall + delay};
}
Millis untilMinute(Millis wall) {
    return (minute - wall % minute) % minute;
}
enum class Phase { Ready, Standby, Work, Break };
enum class Transition { None, ScheduledBreakStarted, BreakCompleted };

struct Schedule {
    Phase phase = Phase::Ready;
    bool frozen = true;
    Time due{};
    Millis saved = 0;
    Millis workLength = 0;
    Millis breakLength = 0;

    Millis remaining(Time now) const {
        return frozen ? saved : std::max<Millis>(0, due.tick - now.tick);
    }
    void startWork(Time at, Millis length) {
        workLength = length;
        const auto wait = untilMinute(at.wall);
        phase = wait ? Phase::Standby : Phase::Work;
        due = after(at, wait ? wait : length);
        frozen = false;
    }
    void startBreak(Time at, Millis length) {
        phase = Phase::Break;
        breakLength = length;
        due = after(at, length);
        frozen = false;
    }
    void freeze(Time at) {
        if (!frozen) {
            saved = remaining(at);
            frozen = true;
        }
    }
    void resume(Time at, Millis configuredWork) {
        if (!frozen) return;
        switch (phase) {
            case Phase::Ready: startWork(at, configuredWork); break;
            case Phase::Standby: startWork(at, workLength); break;
            case Phase::Work:
                due = after(at, saved > 0 ? saved + untilMinute(at.wall + saved) : 0);
                frozen = false;
                break;
            case Phase::Break:
                due = after(at, saved);
                frozen = false;
                break;
        }
    }
    Transition advance(Time now, Millis configuredWork, Millis configuredBreak) {
        if (frozen || phase == Phase::Ready || now.tick < due.tick) return Transition::None;
        if (phase == Phase::Standby) {
            phase = Phase::Work;
            due = after(due, workLength);
            if (now.tick < due.tick) return Transition::None;
        }
        if (phase == Phase::Work) {
            // A late wake still presents a usable break. Ordinary timer jitter
            // preserves the planned :00 boundary instead of adding a standby minute.
            startBreak(now.tick - due.tick >= 1000 ? now : due, configuredBreak);
            return Transition::ScheduledBreakStarted;
        }
        if (phase == Phase::Break) {
            startWork(now.tick - due.tick >= 1000 ? now : due, configuredWork);
            return Transition::BreakCompleted;
        }
        return Transition::None;
    }
    void timeChanged(Time now) {
        if (frozen || phase == Phase::Ready) return;
        if (phase == Phase::Standby) {
            startWork(now, workLength);
        } else {
            auto left = remaining(now);
            if (phase == Phase::Work) left += untilMinute(now.wall + left);
            due = after(now, left);
        }
    }
};
}  // namespace Timing
// Tray artwork uses grayscale area coverage so the same glyph works on any taskbar.

namespace break_art {
using Glyph = std::array<unsigned char, 7>;

constexpr Glyph paused{14,9,9,14,8,8,8}, standby{7,8,8,6,1,1,14},
                onBreak{14,9,9,14,9,9,14};

const Glyph* letterFor(wchar_t ch) {
    if (ch == L'P') return &paused;
    if (ch == L'S') return &standby;
    if (ch == L'B') return &onBreak;
    return nullptr;
}

double overlap(double left, double right, int pixel) {
    return (std::max)(0.0, (std::min)(right, pixel + 1.0) -
                          (std::max)(left, static_cast<double>(pixel)));
}

struct Point {
    int x, y;
    bool control = false;
};
constexpr Point curve(int x, int y) { return {x, y, true}; }
constexpr Point nextContour{-1,-1};

// Original sans-serif outlines on a 32-unit cap height. A control point
// between two vertices describes a quadratic Bézier. Each contour closes.
constexpr Point zero[] = {
    {10,0},curve(20,0),{20,16},curve(20,32),{10,32},curve(0,32),{0,16},curve(0,0),{10,0},
    nextContour,{10,6},curve(6,6),{6,16},curve(6,26),{10,26},curve(14,26),{14,16},curve(14,6),{10,6}
};
constexpr Point one[] = {
    {0,6},{8,0},{14,0},{14,32},{8,32},{8,8},{1,13}
};
constexpr Point two[] = {
    {0,4},curve(4,0),{10,0},curve(20,0),{20,9},curve(20,15),{15,20},{8,26},{20,26},
    {20,32},{0,32},{0,26},{11,15},curve(14,12),{14,9},curve(14,6),{10,6},curve(6,6),{2,10}
};
constexpr Point three[] = {
    {1,2},curve(5,0),{10,0},curve(19,0),{19,8},curve(19,13),{14,15},curve(20,16),{20,23},
    curve(20,32),{9,32},curve(4,32),{0,30},{1,24},curve(5,27),{9,27},curve(14,27),{14,22},
    curve(14,18),{9,18},{5,18},{5,13},{9,13},curve(13,13),{13,9},curve(13,5),{9,5},
    curve(5,5),{2,8}
};
constexpr Point four[] = {
    {13,0},{19,0},{19,20},{22,20},{22,25},{19,25},{19,32},{13,32},{13,25},{0,25},{0,20},
    nextContour,{6,20},{13,20},{13,8}
};
constexpr Point five[] = {
    {2,0},{19,0},{19,6},{7,6},{7,12},curve(9,11),{11,11},curve(20,11),{20,21},
    curve(20,32),{9,32},curve(4,32),{0,30},{1,24},curve(4,27),{9,27},curve(14,27),{14,22},
    curve(14,17),{9,17},curve(6,17),{1,18}
};
constexpr Point six[] = {
    {18,1},{17,7},curve(15,6),{12,6},curve(7,6),{6,14},curve(8,12),{12,12},
    curve(20,12),{20,21},curve(20,32),{10,32},curve(0,32),{0,19},curve(0,0),{13,0},
    curve(16,0),{18,1},nextContour,{6,21},curve(6,27),{10,27},curve(14,27),{14,22},
    curve(14,17),{10,17},curve(7,17),{6,20}
};
constexpr Point seven[] = {
    {0,0},{20,0},{20,5},{9,32},{2,32},{13,6},{0,6}
};
constexpr Point eight[] = {
    {10,0},curve(19,0),{19,8},curve(19,13),{15,15},curve(20,17),{20,23},curve(20,32),{10,32},
    curve(0,32),{0,23},curve(0,18),{5,15},curve(1,13),{1,8},curve(1,0),{10,0},
    nextContour,{10,5},curve(7,5),{7,9},curve(7,12),{10,13},curve(13,12),{13,9},curve(13,5),{10,5},
    nextContour,{10,18},curve(6,20),{6,23},curve(6,27),{10,27},curve(14,27),{14,23},curve(14,20),{10,18}
};
constexpr Point nine[] = {
    {2,31},{3,25},curve(6,27),{9,27},curve(14,27),{14,18},curve(12,20),{8,20},
    curve(0,20),{0,11},curve(0,0),{10,0},curve(20,0),{20,13},curve(20,32),{8,32},
    curve(4,32),{2,31},nextContour,{14,11},curve(14,5),{10,5},curve(6,5),{6,11},curve(6,15),{10,15},
    curve(13,15),{14,13}
};

struct Digit {
    const Point* points;
    size_t count;
    int width;
};
template<size_t N>
constexpr Digit digit(const Point (&points)[N], int width) { return {points, N, width}; }
constexpr Digit digits[] = {
    digit(zero,20), digit(one,14), digit(two,20), digit(three,20), digit(four,22),
    digit(five,20), digit(six,20), digit(seven,20), digit(eight,20), digit(nine,20)
};

constexpr size_t curveSteps = 8;

// Reserve for all line segments of any digit. Counting the first vertex of
// each contour covers its closing edge, even when it is implicit.
constexpr size_t maxDigitEdges = [] {
    size_t maximum = 0;
    for (const Digit& shape : digits) {
        size_t bound = 0;
        for (size_t i = 0; i < shape.count; ++i)
            bound += shape.points[i].control ? curveSteps : 1;
        maximum = (std::max)(maximum, bound);
    }
    return maximum;
}();

struct Edge { double x, y, endY, slope; };

void fillDigit(const Digit& shape, double left, double top, double sx, double sy,
               float* coverage, int size) {
    constexpr int subrows = 8;
    std::array<Edge, maxDigitEdges> edges;
    size_t edgeCount = 0;
    auto line = [&](double x0, double y0, double x1, double y1) {
        if (y0 == y1) return;
        if (y1 < y0) { std::swap(x0,x1); std::swap(y0,y1); }
        edges[edgeCount++] = {left + x0*sx, top + y0*sy, top + y1*sy,
                              (x1-x0)*sx / ((y1-y0)*sy)};
    };
    size_t i = 0;
    while (i < shape.count) {
        const Point first = shape.points[i++];
        Point prev = first;
        while (i < shape.count && shape.points[i].x >= 0) {
            Point end = shape.points[i++];
            if (end.control) {
                const Point control = end;
                end = shape.points[i++];
                double x = prev.x, y = prev.y;
                for (size_t j=1; j<=curveSteps; ++j) {
                    const double t = static_cast<double>(j) / curveSteps, u = 1-t;
                    const double nx = u*u*prev.x + 2*u*t*control.x + t*t*end.x;
                    const double ny = u*u*prev.y + 2*u*t*control.y + t*t*end.y;
                    line(x,y,nx,ny);
                    x = nx; y = ny;
                }
            } else {
                line(prev.x,prev.y,end.x,end.y);
            }
            prev = end;
        }
        line(prev.x,prev.y,first.x,first.y);
        ++i;  // Next contour, if present.
    }
    // Integrate horizontal spans at eight heights per pixel. Even-odd fill
    // leaves the counters open and produces a single grayscale alpha channel.
    for (int y=static_cast<int>(top); y<static_cast<int>(std::ceil(top+32*sy)); ++y) {
        for (int sample=0; sample<subrows; ++sample) {
            const double scanY = y+(sample+.5)/subrows;
            std::array<double, maxDigitEdges> crossings;
            size_t count = 0;
            for (size_t k=0; k<edgeCount; ++k) {
                const auto& e = edges[k];
                if (scanY >= e.y && scanY < e.endY)
                    crossings[count++] = e.x + (scanY-e.y)*e.slope;
            }
            std::sort(crossings.begin(), crossings.begin()+count);
            for (size_t k=1; k<count; k+=2) {
                const double x0=crossings[k-1], x1=crossings[k];
                const int end = (std::min)(size, static_cast<int>(std::ceil(x1)));
                for (int x = static_cast<int>(x0); x < end; ++x)
                    coverage[y * size + x] += static_cast<float>(overlap(x0, x1, x) / subrows);
            }
        }
    }
}

void drawNumerals(std::wstring_view glyph, float* coverage, int size) {
    const int height = static_cast<int>(std::lround(size*.75));
    const int top = (size-height)/2;
    const double sy = height/32.0;
    const int gap = (std::max)(1, size/16);
    const int gaps = static_cast<int>(glyph.size()-1)*gap;
    int units = 0;
    for (wchar_t ch : glyph) units += digits[ch-L'0'].width;
    const double sx = (std::min)(sy, (size-gap-gaps)/static_cast<double>(units));
    double left = (size-units*sx-gaps)/2.0;
    for (wchar_t ch : glyph) {
        const auto& shape = digits[ch-L'0'];
        fillDigit(shape, left, top, sx, sy, coverage, size);
        left += shape.width*sx + gap;
    }
}

struct MonoBitmapInfo {
    BITMAPINFOHEADER header{};
    RGBQUAD palette[2]{};
};

BITMAPINFOHEADER bitmapHeader(int w, int h, WORD depth) {
    BITMAPINFOHEADER header{};
    header.biSize = sizeof(header);
    header.biWidth = w;
    header.biHeight = -h;  // Top-down image, including the monochrome AND mask.
    header.biPlanes = 1;
    header.biBitCount = depth;
    header.biCompression = BI_RGB;
    return header;
}
}  // namespace break_art

HICON MakeTrayIcon(std::wstring_view glyph, COLORREF fg, COLORREF bg,
                   int backgroundOpacity, UINT dpi, int metric = SM_CXSMICON) {
    using namespace break_art;
    if (glyph.empty() || glyph.size() > 3) return nullptr;
    const bool numeric = std::all_of(glyph.begin(), glyph.end(), [](wchar_t ch) {
        return ch >= L'0' && ch <= L'9';
    });
    const Glyph* letter = glyph.size() == 1 ? letterFor(glyph[0]) : nullptr;
    if (!numeric && !letter) return nullptr;
    const int size = std::clamp(GetSystemMetricsForDpi(metric, dpi ? dpi : 96), 16, 128);
    const size_t count = static_cast<size_t>(size) * size;
    auto* coverage = new(std::nothrow) float[count]();
    if (!coverage) return nullptr;

    if (numeric) {
        drawNumerals(glyph, coverage, size);
    } else {
        const int margin = (std::max)(1, size / 16);
        const double scale = (size - 2*margin)/7.0;
        const double left = (size-4*scale)/2.0;
        for (int row=0; row<7; ++row) {
            for (int col=0; col<4; ++col) {
                if (!((*letter)[row] & (8 >> col))) continue;
                const double x0 = left+col*scale, x1 = x0+scale;
                const double y0 = margin+row*scale, y1 = y0+scale;
                for (int y=static_cast<int>(y0); y<std::ceil(y1); ++y) {
                    const double height = overlap(y0,y1,y);
                    for (int x=static_cast<int>(x0); x<std::ceil(x1); ++x)
                        coverage[y*size+x] += static_cast<float>(height*overlap(x0,x1,x));
                }
            }
        }
    }

    BITMAPINFO colorInfo{};
    colorInfo.bmiHeader = bitmapHeader(size, size, 32);
    MonoBitmapInfo maskInfo{};
    maskInfo.header = bitmapHeader(size, size, 1);
    maskInfo.palette[1] = RGBQUAD{255, 255, 255, 0};
    void* pixels = nullptr;
    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(nullptr, &colorInfo, DIB_RGB_COLORS, &pixels, nullptr, 0);
    HBITMAP mask = CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO*>(&maskInfo),
                                    DIB_RGB_COLORS, &bits, nullptr, 0);
    HICON icon = nullptr;
    if (color && mask && pixels && bits) {
        const int stride = ((size + 31) / 32) * 4;
        auto* out = static_cast<std::uint32_t*>(pixels);
        auto* andMask = static_cast<unsigned char*>(bits);
        std::fill_n(andMask, stride * size, static_cast<unsigned char>(0));
        const double opacity = std::clamp(backgroundOpacity, 0, 100) / 100.0;
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                const int i = y * size + x;
                const double ink = std::clamp(static_cast<double>(coverage[i]), 0.0, 1.0);
                const double backing = (1.0 - ink) * opacity;
                const auto channel = [ink, backing](int foreground, int background) {
                    return static_cast<std::uint32_t>(std::lround(ink * foreground + backing * background));
                };
                const auto a = static_cast<std::uint32_t>(std::lround((ink + backing) * 255));
                out[i] = (a << 24) | (channel(GetRValue(fg), GetRValue(bg)) << 16) |
                         (channel(GetGValue(fg), GetGValue(bg)) << 8) |
                         channel(GetBValue(fg), GetBValue(bg));
                if (!a) andMask[y * stride + x / 8] |= static_cast<unsigned char>(0x80 >> (x % 8));
            }
        }
        ICONINFO info{};
        info.fIcon = TRUE;
        info.hbmColor = color;
        info.hbmMask = mask;
        icon = CreateIconIndirect(&info);
    }
    if (mask) DeleteObject(mask);
    if (color) DeleteObject(color);
    delete[] coverage;
    return icon;
}

// All methods run on the controller's UI thread. The parent owns the schedule.

class BreakWindows {
    struct Frame {
        BreakWindows* owner{};
        Frame* next{};
        HMONITOR monitor{};
        HWND window{}, button{}, closeButton{};
        HFONT clockFont{}, buttonFont{}, closeFont{};
        HDC back{};
        HBITMAP bitmap{};
        HGDIOBJ oldBitmap{};
        std::uint32_t* pixels{};
        int w{}, h{}, diameter{};
        bool seen{}, positioning{};
    };

    static constexpr wchar_t className_[] = L"SystemTrayBreakTimer.BreakWindow";
    static constexpr int skipId_ = 100;
    static constexpr int closeId_ = 101;
    static constexpr DWORD style_ = WS_POPUP | WS_CLIPCHILDREN;
    static constexpr DWORD exStyle_ = WS_EX_LAYERED | WS_EX_TOPMOST;
    static constexpr double pi_ = 3.14159265358979323846;
    HINSTANCE instance_{};
    HICON largeIcon_{}, smallIcon_{};
    Frame* first_{};
    HWND controller_{};
    UINT skipMessage_{};
    unsigned remaining_{}, total_{1};
    COLORREF gauge_{RGB(88,214,141)}, text_{RGB(255,255,255)}, bg_{RGB(24,32,40)};
    int opacity_{95};
    bool skipPending_{};

    static std::uint32_t pixel(COLORREF c) {
        return (static_cast<std::uint32_t>(GetRValue(c)) << 16) |
               (static_cast<std::uint32_t>(GetGValue(c)) << 8) | GetBValue(c);
    }

    static COLORREF mix(COLORREF a, COLORREF b, double weight) {
        const auto component = [weight](int x, int y) {
            return static_cast<unsigned>(std::lround(x + weight * (y - x)));
        };
        return RGB(component(GetRValue(a), GetRValue(b)),
                   component(GetGValue(a), GetGValue(b)),
                   component(GetBValue(a), GetBValue(b)));
    }

    static HFONT font(int height, int weight) {
        return CreateFontW(-height, 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    }

    static void releaseBuffer(Frame& f) {
        if (f.back && f.oldBitmap) SelectObject(f.back, f.oldBitmap);
        if (f.bitmap) DeleteObject(f.bitmap);
        if (f.back) DeleteDC(f.back);
        f.back = nullptr; f.oldBitmap = nullptr; f.bitmap = nullptr; f.pixels = nullptr;
    }

    static void releaseFonts(Frame& f) {
        if (f.clockFont) DeleteObject(f.clockFont);
        if (f.buttonFont) DeleteObject(f.buttonFont);
        if (f.closeFont) DeleteObject(f.closeFont);
        f.clockFont = nullptr;
        f.buttonFont = nullptr;
        f.closeFont = nullptr;
    }

    static void layout(Frame& f) {
        RECT rect{};
        if (!GetClientRect(f.window, &rect) || !f.button || !f.closeButton) return;
        releaseBuffer(f);
        f.w = rect.right; f.h = rect.bottom;
        const int diameter = (std::max)(1,(std::min)({f.diameter, f.w-2, f.h-2}));
        const int buttonHeight = (std::max)(18, diameter / 13);
        const int buttonWidth = (std::max)(70, diameter / 3);
        const UINT dpi = GetDpiForWindow(f.window);
        // The entire rectangular child must fit in the corner outside the ring.
        const int corner = (std::max)(1, (std::min)(f.w, f.h) / 2 -
                                       static_cast<int>(std::ceil(diameter / std::sqrt(8.0))) - 1);
        const int inset = (std::min)(MulDiv(8, dpi ? dpi : 96, 96), corner / 4);
        const int closeSize = (std::min)(MulDiv(32, dpi ? dpi : 96, 96), corner - inset);
        if (f.buttonFont) SendMessageW(f.button, WM_SETFONT, 0, FALSE);
        releaseFonts(f);
        f.clockFont = font((std::max)(14, diameter / 5), FW_SEMIBOLD);
        f.buttonFont = font((std::max)(11, buttonHeight * 52 / 100), FW_MEDIUM);
        f.closeFont = font((std::max)(1, closeSize * 7 / 8), FW_SEMIBOLD);
        SendMessageW(f.button, WM_SETFONT, reinterpret_cast<WPARAM>(f.buttonFont), FALSE);
        SetWindowPos(f.button, nullptr, (f.w-buttonWidth)/2, f.h/2+diameter*13/100,
                     buttonWidth, buttonHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        SetWindowPos(f.closeButton, nullptr, f.w - inset - closeSize, inset,
                     closeSize, closeSize, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    static void position(Frame& f) {
        if (f.positioning || !f.window) return;
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(f.monitor, &mi)) return;
        f.positioning = true;
        const RECT work = mi.rcWork;
        const int workW = work.right - work.left, workH = work.bottom - work.top;
        const UINT dpi = GetDpiForWindow(f.window);
        const int shorter = (std::min)(workW, workH);
        f.diameter = MulDiv(shorter, 90, 100);
        const int pad = MulDiv(8, dpi ? dpi : 96, 96);
        const int side = (std::min)(shorter, f.diameter + 2*pad);
        SetWindowPos(f.window, HWND_TOPMOST, work.left+(workW-side)/2, work.top+(workH-side)/2,
                     side,side,SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        layout(f);
        f.positioning = false;
    }

    void applyOpacity(Frame& f) {
        // At zero opacity Windows passes mouse input through layered windows.
        SetLayeredWindowAttributes(f.window, 0, static_cast<BYTE>(MulDiv(opacity_,255,100)), LWA_ALPHA);
    }

    void skip() {
        if (!skipPending_ && PostMessageW(controller_, skipMessage_, 0, 0)) skipPending_ = true;
    }

    void paintRing(Frame& f) const {
        const double cx = f.w/2.0, cy = f.h/2.0;
        const double outer = (std::min)({f.diameter, f.w-2, f.h-2}) / 2.0;
        if (outer <= 1) return;
        const double inner = (std::max)(0.0, outer - (std::max)(2.0, outer*0.064));
        const COLORREF track = mix(bg_, text_, .19);
        const double fraction = (std::min)(1.0, static_cast<double>(remaining_)/(std::max)(1u,total_));
        const double cutoff = fraction * 2*pi_;
        for (int y = (std::max)(0,static_cast<int>(std::floor(cy-outer-1)));
             y < (std::min)(f.h,static_cast<int>(std::ceil(cy+outer+1))); ++y) {
            const double dy = y + .5 - cy;
            const double outerExtent = std::sqrt((std::max)(0.0,(outer+.5)*(outer+.5)-dy*dy));
            const double innerExtent = std::sqrt((std::max)(0.0,(inner-.5)*(inner-.5)-dy*dy));
            int left = (std::max)(0, static_cast<int>(std::floor(cx - outerExtent - 1)));
            const int end = (std::min)(f.w, static_cast<int>(std::ceil(cx + outerExtent + 1)));
            const int holeStart = static_cast<int>(std::ceil(cx-innerExtent));
            const int holeEnd = static_cast<int>(std::floor(cx+innerExtent-1));
            for (int x = left; x < end; ++x) {
                if (x >= holeStart && x < holeEnd) x = holeEnd;
                if (x >= end) break;
                const double dx = x+.5-cx;
                const double r = std::sqrt(dx*dx+dy*dy);
                const double coverage = std::clamp((std::min)(outer+.5-r,r-inner+.5),0.0,1.0);
                if (coverage <= 0) continue;
                double angle = std::atan2(dx,-dy);
                if (angle < 0) angle += 2*pi_;
                const double sweep = fraction >= 1 ? 1 : fraction <= 0 ? 0 :
                    std::clamp((cutoff-angle)*r+.5,0.0,1.0);
                const COLORREF arc = mix(track,gauge_,sweep);
                f.pixels[y*f.w+x] = pixel(mix(bg_,arc,coverage));
            }
        }
    }

    bool ensureBuffer(Frame& f, HDC target) {
        if (f.back && f.pixels) return true;
        releaseBuffer(f);
        if (f.w<=0 || f.h<=0) return false;
        f.back = CreateCompatibleDC(target);
        BITMAPINFO info{};
        info.bmiHeader = break_art::bitmapHeader(f.w,f.h,32);
        void* buffer = nullptr;
        f.bitmap = CreateDIBSection(target,&info,DIB_RGB_COLORS,&buffer,nullptr,0);
        if (f.back && f.bitmap && buffer) {
            f.oldBitmap = SelectObject(f.back,f.bitmap);
            if (f.oldBitmap && f.oldBitmap != HGDI_ERROR) {
                f.pixels = static_cast<std::uint32_t*>(buffer);
                return true;
            }
            f.oldBitmap = nullptr;
        }
        releaseBuffer(f);
        return false;
    }

    void paint(Frame& f, HDC target) {
        const bool buffered = ensureBuffer(f,target);
        HDC dc = buffered ? f.back : target;
        RECT rect{0,0,f.w,f.h};
        if (buffered) {
            GdiFlush();  // Previous GDI text/blit operations may still reference the DIB.
            std::fill_n(f.pixels,static_cast<size_t>(f.w)*f.h,pixel(bg_));
            paintRing(f);
        } else {
            SetDCBrushColor(dc,bg_);
            FillRect(dc,&rect,static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
        }
        wchar_t time[32];
        std::swprintf(time,std::size(time),L"%02u:%02u",remaining_/60,remaining_%60);
        HGDIOBJ old = SelectObject(dc,f.clockFont ? static_cast<HGDIOBJ>(f.clockFont) : GetStockObject(DEFAULT_GUI_FONT));
        SetBkMode(dc,TRANSPARENT);
        SetTextColor(dc,text_);
        DrawTextW(dc,time,-1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (old) SelectObject(dc,old);
        if (buffered) BitBlt(target,0,0,f.w,f.h,dc,0,0,SRCCOPY);
    }

    void drawButton(Frame& f,const DRAWITEMSTRUCT& item) {
        HDC dc = item.hDC;
        RECT r = item.rcItem;
        const int saved = SaveDC(dc);
        if (!saved) return;
        const bool close = item.CtlID == closeId_;
        const bool pressed = (item.itemState & ODS_SELECTED) != 0;
        SetDCBrushColor(dc, close && pressed ? mix(bg_,gauge_,.18) : bg_);
        FillRect(dc,&r,static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
        if (!close) {
            SelectObject(dc,GetStockObject(DC_BRUSH)); SelectObject(dc,GetStockObject(DC_PEN));
            SetDCBrushColor(dc,mix(bg_,text_,pressed ? .24 : .1));
            SetDCPenColor(dc,mix(bg_,text_,.45));
            const int curve = (std::max)(3, static_cast<int>(r.bottom-r.top)/4);
            RoundRect(dc,r.left,r.top,r.right,r.bottom,curve,curve);
        }
        HFONT face = close ? f.closeFont : f.buttonFont;
        SelectObject(dc,face ? static_cast<HGDIOBJ>(face) : GetStockObject(DEFAULT_GUI_FONT));
        SetBkMode(dc,TRANSPARENT);
        SetTextColor(dc,close ? gauge_ : text_);
        DrawTextW(dc,close ? L"\u00D7" : L"Skip Break",-1,&r,DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (item.itemState & ODS_FOCUS) {
            InflateRect(&r,-3,-3); DrawFocusRect(dc,&r);
        }
        RestoreDC(dc,saved);
    }

    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        auto* f = reinterpret_cast<Frame*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
        if (msg==WM_NCCREATE) {
            f = static_cast<Frame*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
            SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(f));
            f->window=hwnd;
        }
        if (!f) return DefWindowProcW(hwnd,msg,wp,lp);
        switch (msg) {
        case WM_CREATE: {
            const COLORREF noBorder = DWMWA_COLOR_NONE;
            DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &noBorder, sizeof(noBorder));
            constexpr DWORD buttonStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW;
            f->button=CreateWindowExW(0,L"BUTTON",L"Skip Break",buttonStyle,0,0,1,1,hwnd,
                                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(skipId_)),
                                      f->owner->instance_,nullptr);
            f->closeButton=CreateWindowExW(0,L"BUTTON",L"Close break window",buttonStyle,0,0,1,1,hwnd,
                                           reinterpret_cast<HMENU>(static_cast<INT_PTR>(closeId_)),
                                           f->owner->instance_,nullptr);
            return f->button && f->closeButton ? 0 : -1;
        }
        case WM_SIZE:
            if (!f->positioning) layout(*f);
            InvalidateRect(hwnd,nullptr,FALSE);
            return 0;
        case WM_DPICHANGED:
            position(*f);
            InvalidateRect(hwnd,nullptr,FALSE);
            return 0;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc=BeginPaint(hwnd,&ps);
            if (dc) f->owner->paint(*f,dc);
            EndPaint(hwnd,&ps);
            return 0;
        }
        case WM_DRAWITEM:
            if (wp == skipId_ || wp == closeId_) {
                f->owner->drawButton(*f,*reinterpret_cast<DRAWITEMSTRUCT*>(lp));
                return TRUE;
            }
            break;
        case WM_SETFOCUS:
            if (f->button) SetFocus(f->button);
            return 0;
        case WM_COMMAND:
            if ((LOWORD(wp)==skipId_ || LOWORD(wp)==closeId_) && HIWORD(wp)==BN_CLICKED) {
                f->owner->skip(); return 0;
            }
            break;
        case WM_CLOSE:
            f->owner->skip(); return 0;
        case WM_NCDESTROY:
            SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
            f->window=nullptr; f->button=nullptr; f->closeButton=nullptr;
            break;
        }
        return DefWindowProcW(hwnd,msg,wp,lp);
    }

    void remove(Frame* f) {
        if (f->window) DestroyWindow(f->window);
        releaseBuffer(*f); releaseFonts(*f);
        delete f;
    }

    void ensureMonitor(HMONITOR monitor) {
        for (Frame* f=first_; f; f=f->next) {
            if (f->monitor==monitor) {
                f->seen=true; position(*f); return;
            }
        }
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(monitor,&mi)) return;
        auto* f=new(std::nothrow) Frame;
        if (!f) return;
        f->owner=this; f->monitor=monitor; f->seen=true;
        HWND hwnd=CreateWindowExW(exStyle_,className_,L"System Tray BreakTimer",style_,
                                  mi.rcWork.left,mi.rcWork.top,64,64,nullptr,nullptr,instance_,f);
        if (!hwnd) { remove(f); return; }
        f->next=first_; first_=f;
        position(*f); applyOpacity(*f);
        ShowWindow(hwnd,SW_SHOWNOACTIVATE);
    }

    static BOOL CALLBACK enumerate(HMONITOR monitor,HDC,LPRECT,LPARAM data) {
        reinterpret_cast<BreakWindows*>(data)->ensureMonitor(monitor);
        return TRUE;
    }
public:
    BreakWindows()=default;
    BreakWindows(const BreakWindows&)=delete;
    BreakWindows& operator=(const BreakWindows&)=delete;

    bool init() {
        if (instance_) return true;
        HMODULE mod=nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                className_,&mod)) return false;
        WNDCLASSEXW wc{};
        wc.cbSize=sizeof(wc); wc.lpfnWndProc=windowProc; wc.hInstance=mod;
        wc.lpszClassName=className_; wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        // Keep Alt+Tab available for keyboard users without activating a break on arrival.
        const UINT dpi = GetDpiForSystem();
        wc.hIcon = largeIcon_ = MakeTrayIcon(L"B", RGB(255,180,84), 0, 0, dpi, SM_CXICON);
        wc.hIconSm = smallIcon_ = MakeTrayIcon(L"B", RGB(255,180,84), 0, 0, dpi);
        if (!RegisterClassExW(&wc)) return false;
        instance_=mod; return true;
    }
    void shutdown() {
        hide();
        if (instance_) UnregisterClassW(className_,instance_);
        if (largeIcon_) DestroyIcon(largeIcon_);
        if (smallIcon_) DestroyIcon(smallIcon_);
        largeIcon_=smallIcon_=nullptr;
        instance_=nullptr;
    }
    void setVisuals(COLORREF gauge,COLORREF text,COLORREF background,int opacity) {
        gauge_=gauge; text_=text; bg_=background; opacity_=std::clamp(opacity,0,100);
        for (Frame* f=first_;f;f=f->next) {
            applyOpacity(*f);
            InvalidateRect(f->window,nullptr,FALSE);
            InvalidateRect(f->button,nullptr,FALSE);
            InvalidateRect(f->closeButton,nullptr,FALSE);
        }
    }
    void show(bool all,HMONITOR preferred,HWND controller,UINT skipMessage) {
        refresh(all,preferred,controller,skipMessage);
    }
    void refresh(bool all,HMONITOR preferred,HWND controller,UINT skipMessage) {
        if (!instance_) return;
        controller_=controller; skipMessage_=skipMessage;
        for (Frame* f=first_;f;f=f->next) f->seen=false;
        if (all) EnumDisplayMonitors(nullptr,nullptr,enumerate,reinterpret_cast<LPARAM>(this));
        else {
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            if (!preferred || !GetMonitorInfoW(preferred,&mi))
                preferred=MonitorFromPoint(POINT{0,0},MONITOR_DEFAULTTOPRIMARY);
            ensureMonitor(preferred);
        }
        Frame** link=&first_;
        while (*link) {
            Frame* f=*link;
            if (f->seen) link=&f->next;
            else { *link=f->next; remove(f); }
        }
    }
    void update(unsigned remaining,unsigned total) {
        if (remaining==remaining_ && total==total_) return;
        remaining_=remaining; total_=(std::max)(1u,total);
        for (Frame* f=first_;f;f=f->next) InvalidateRect(f->window,nullptr,FALSE);
    }
    void hide() {
        while (first_) {
            Frame* f=first_; first_=f->next; remove(f);
        }
        skipPending_=false;
    }
    bool handleKey(const MSG& msg) {
        const bool tab = msg.message == WM_KEYDOWN && msg.wParam == VK_TAB;
        const bool enter = msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN;
        const bool close = msg.message == WM_SYSKEYDOWN && msg.wParam == VK_F4 &&
                           (msg.lParam & (1L << 29));
        if ((!tab && !enter && !close) || !owns(msg.hwnd)) return false;
        const HWND window = GetAncestor(msg.hwnd, GA_ROOT);
        if (tab) {
            SendMessageW(window, WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR,UISF_HIDEFOCUS), 0);
            HWND next = GetNextDlgTabItem(window, GetFocus(), GetKeyState(VK_SHIFT) < 0);
            if (next) SetFocus(next);
        } else if (enter) {
            HWND button = GetFocus();
            if (button == GetDlgItem(window, skipId_) || button == GetDlgItem(window, closeId_))
                SendMessageW(button, BM_CLICK, 0, 0);
        } else {
            SendMessageW(window, WM_CLOSE, 0, 0);
        }
        return true;
    }
    bool owns(HWND window) const {
        if (!window) return false;
        HWND root=GetAncestor(window,GA_ROOT);
        for (Frame* f=first_;f;f=f->next) if (f->window==root) return true;
        return false;
    }
};
// Full-screen detection and sound run in the desktop user's dedicated tool host.

namespace BreakEnvironment {

static bool IsAppWindow(HWND window) {
    if (!window || window == GetDesktopWindow() || window == GetShellWindow() ||
        !IsWindowVisible(window) || IsIconic(window)) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (!pid || pid == GetCurrentProcessId()) {
        return false;
    }

    wchar_t name[128] = {};
    if (!GetClassNameW(window, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"Progman") == 0 || wcscmp(name, L"WorkerW") == 0 ||
        wcscmp(name, L"Shell_TrayWnd") == 0 ||
        wcscmp(name, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }

    DWORD cloaked = 0;
    return FAILED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) || !cloaked;
}

// Remember the foreground app's monitor across a click on our tray menu.
static HMONITOR PreferredMonitor(HWND window = GetForegroundWindow()) {
    static HMONITOR lastMonitor = nullptr;
    if (IsAppWindow(window)) {
        lastMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (lastMonitor && GetMonitorInfoW(lastMonitor, &info)) {
        return lastMonitor;
    }
    return MonitorFromPoint(POINT{}, MONITOR_DEFAULTTOPRIMARY);
}

static bool HasMonitorSizedClient(HWND window, const RECT& monitor) {
    if (!IsAppWindow(window)) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    if ((style & WS_CAPTION) == WS_CAPTION && IsZoomed(window)) {
        return false;
    }

    RECT client = {};
    if (!GetClientRect(window, &client)) {
        return false;
    }
    // MapWindowPoints with two points handles mirrored window layouts as a RECT.
    SetLastError(ERROR_SUCCESS);
    if (!MapWindowPoints(window, nullptr, reinterpret_cast<POINT*>(&client), 2) &&
        GetLastError() != ERROR_SUCCESS) {
        return false;
    }
    // Compare the client, not the resize border. A regular maximized title bar
    // or a reserved taskbar keeps the client from filling the monitor.
    return std::abs(client.left - monitor.left) <= 1 &&
           std::abs(client.top - monitor.top) <= 1 &&
           std::abs(client.right - monitor.right) <= 1 &&
           std::abs(client.bottom - monitor.bottom) <= 1;
}

static BOOL CALLBACK ProbeMonitor(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return TRUE;
    }

    const RECT& bounds = info.rcMonitor;
    const LONG width = bounds.right - bounds.left;
    const LONG height = bounds.bottom - bounds.top;
    // Inspect actual visible windows across each monitor. Corner probes avoid
    // treating a tooltip, a volume flyout, or our break window as an app exit.
    const POINT probes[] = {
        {bounds.left + width / 2, bounds.top + height / 2},
        {bounds.left + width / 30, bounds.top + height / 12},
        {bounds.right - 1 - width / 30, bounds.top + height / 12},
        {bounds.left + width / 30, bounds.bottom - 1 - height / 12},
        {bounds.right - 1 - width / 30, bounds.bottom - 1 - height / 12}
    };
    HWND seen[ARRAYSIZE(probes)] = {};
    unsigned seenCount = 0;
    for (const POINT& point : probes) {
        HWND candidate = GetAncestor(WindowFromPoint(point), GA_ROOT);
        if (!candidate) {
            continue;
        }
        bool examined = false;
        for (unsigned index = 0; index < seenCount; ++index) {
            if (seen[index] == candidate) {
                examined = true;
                break;
            }
        }
        if (examined) {
            continue;
        }
        seen[seenCount++] = candidate;
        if (HasMonitorSizedClient(candidate, bounds)) {
            *reinterpret_cast<bool*>(data) = true;
            return FALSE;
        }
    }
    return TRUE;
}

static bool FullscreenAppVisible() {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
        state == QUNS_RUNNING_D3D_FULL_SCREEN) {
        return true;
    }
    bool found = false;
    EnumDisplayMonitors(nullptr, nullptr, ProbeMonitor,
                        reinterpret_cast<LPARAM>(&found));
    return found;
}

static DWORD IdleMilliseconds() {
    // Both values come from the same 32-bit Windows tick clock.
    static DWORD lastKnownIdle = 0;
    LASTINPUTINFO input = {sizeof(input), 0};
    if (GetLastInputInfo(&input)) {
        lastKnownIdle = GetTickCount() - input.dwTime;
        // Some input sources stamp events slightly ahead of the sampled clock.
        if (lastKnownIdle > MAXDWORD - 10000) {
            lastKnownIdle = 0;
        }
    }
    return lastKnownIdle;
}

// The standard 44-byte PCM header is rebuilt for both embedded and custom audio.
struct WaveHeader {
    char riff[4];
    std::uint32_t riffSize;
    char waveFmt[8];
    std::uint32_t formatSize;
    std::uint16_t pcm, channels;
    std::uint32_t sampleRate, bytesPerSecond;
    std::uint16_t blockAlign, bits;
    char data[4];
    std::uint32_t dataSize;
};
static_assert(sizeof(WaveHeader) == 44 && offsetof(WaveHeader, dataSize) == 40);

static WaveHeader PcmHeader(unsigned channels, unsigned rate, unsigned bits,
                            std::uint32_t bytes) {
    const auto frameBytes = static_cast<std::uint16_t>(channels * bits / 8);
    return {{'R','I','F','F'}, 36 + bytes + (bytes & 1u), {'W','A','V','E','f','m','t',' '},
            16, 1, static_cast<std::uint16_t>(channels), rate, rate * frameBytes,
            frameBytes, static_cast<std::uint16_t>(bits), {'d','a','t','a'}, bytes};
}

static std::string_view PackedRecording();  // The CC0 recording lives at the end of this file.

// The data uses a printable 6-bit alphabet. Each 128-frame block starts with two
// 4-bit widths, followed by signed left-channel samples and stereo differences.
class RecordingReader {
    std::string_view source_ = PackedRecording();
    std::size_t next_ = 0;
    std::uint32_t reservoir_ = 0;
    unsigned available_ = 0;

public:
    bool valid = true;
    unsigned read(unsigned width) {
        while (available_ < width) {
            if (next_ == source_.size()) { valid = false; return 0; }
            const char ch = source_[next_++];
            unsigned value;
            if (ch >= 'A' && ch <= 'Z') value = ch - 'A';
            else if (ch >= 'a' && ch <= 'z') value = ch - 'a' + 26;
            else if (ch >= '0' && ch <= '9') value = ch - '0' + 52;
            else if (ch == '+') value = 62;
            else if (ch == '/') value = 63;
            else { valid = false; return 0; }
            reservoir_ = (reservoir_ << 6) | value;
            available_ += 6;
        }
        available_ -= width;
        return (reservoir_ >> available_) & ((1u << width) - 1);
    }
    int sample(unsigned width) {
        const unsigned zigzag = read(width);
        const int magnitude = static_cast<int>(zigzag / 2);
        return (zigzag & 1) ? -magnitude - 1 : magnitude;
    }
};

static const void* BuiltInChime() {
    constexpr std::size_t frames = 148746;
    struct StereoWave {
        WaveHeader header;
        std::int16_t samples[frames * 2];
    };
    static_assert(sizeof(StereoWave) == 595028);
    static StereoWave wave{};
    static const bool unpacked = [] {
        RecordingReader reader;
        wave.header = PcmHeader(2, 44100, 16, sizeof(wave.samples));
        unsigned leftWidth = 0, sideWidth = 0;
        for (std::size_t i = 0; i < frames; ++i) {
            if (i % 128 == 0) {
                leftWidth = reader.read(4);
                sideWidth = reader.read(4);
            }
            const int left = reader.sample(leftWidth);
            wave.samples[i * 2] = static_cast<std::int16_t>(left);
            wave.samples[i * 2 + 1] = static_cast<std::int16_t>(left + reader.sample(sideWidth));
        }
        return reader.valid;
    }();
    return unpacked ? &wave : nullptr;
}

constexpr DWORD maxCustomBytes = 2 * 1024 * 1024;
constexpr unsigned maxCustomSeconds = 10;

struct CustomWave {
    std::unique_ptr<BYTE[]> bytes;
    std::size_t size = 0;
};
static CustomWave customChime;
static bool chimeStarted = false;

template<class Integer>
static Integer ReadLittleEndian(const BYTE* data) {
    // Windows x86 and x64 use little-endian byte order. RIFF chunks may be unaligned.
    Integer value;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

static bool PrepareCustomPcm(CustomWave& wave) {
    BYTE* bytes = wave.bytes.get();
    const std::size_t size = wave.size;
    if (size < sizeof(WaveHeader) || size > maxCustomBytes ||
        std::memcmp(bytes, "RIFF", 4) || std::memcmp(bytes + 8, "WAVE", 4) ||
        ReadLittleEndian<DWORD>(bytes + 4) != size - 8) {
        return false;
    }
    const BYTE* fmt = nullptr;
    const BYTE* samples = nullptr;
    DWORD fmtBytes = 0, sampleBytes = 0;
    for (std::size_t pos = 12; pos < size;) {
        if (size - pos < 8) return false;
        const BYTE* chunk = bytes + pos;
        const DWORD length = ReadLittleEndian<DWORD>(chunk + 4);
        pos += 8;
        if (length > size - pos) return false;
        // Some writers omit the alignment byte for an odd-length final data chunk.
        const bool isData = std::memcmp(chunk, "data", 4) == 0;
        std::size_t advance = length;
        if ((length & 1u) && (length != size - pos || !isData)) {
            if (length == size - pos) return false;
            ++advance;
        }
        if (!std::memcmp(chunk, "fmt ", 4)) {
            if (fmt || length < 16) return false;
            fmt = bytes + pos;
            fmtBytes = length;
        } else if (isData) {
            if (samples) return false;
            samples = bytes + pos;
            sampleBytes = length;
        }
        pos += advance;
    }
    if (!fmt || !samples || !sampleBytes) return false;
    const WORD channels = ReadLittleEndian<WORD>(fmt + 2);
    const DWORD rate = ReadLittleEndian<DWORD>(fmt + 4);
    const WORD bits = ReadLittleEndian<WORD>(fmt + 14);
    if ((channels != 1 && channels != 2) || rate < 8000 || rate > 48000 ||
        (bits != 8 && bits != 16)) return false;
    const WORD tag = ReadLittleEndian<WORD>(fmt);
    if (tag == WAVE_FORMAT_EXTENSIBLE) {
        // KSDATAFORMAT_SUBTYPE_PCM, serialized in the file's GUID byte order.
        constexpr BYTE pcmSubtype[] = {1,0,0,0, 0,0,16,0, 128,0,0,170,0,56,155,113};
        if (fmtBytes < 40 || ReadLittleEndian<WORD>(fmt + 16) < 22 ||
            ReadLittleEndian<WORD>(fmt + 16) > fmtBytes - 18 ||
            ReadLittleEndian<WORD>(fmt + 18) > bits || std::memcmp(fmt + 24, pcmSubtype, 16))
            return false;
    } else if (tag != WAVE_FORMAT_PCM) {
        return false;
    }
    const auto header = PcmHeader(channels, rate, bits, sampleBytes);
    if (ReadLittleEndian<WORD>(fmt + 12) != header.blockAlign ||
        ReadLittleEndian<DWORD>(fmt + 8) != header.bytesPerSecond ||
        sampleBytes % header.blockAlign || sampleBytes > maxCustomSeconds * header.bytesPerSecond)
        return false;

    // Pass WinMM only the verified PCM header and samples. Skip metadata chunks.
    const std::size_t outputSize = sizeof(header) + sampleBytes + (sampleBytes & 1u);
    auto output = std::unique_ptr<BYTE[]>(new(std::nothrow) BYTE[outputSize]);
    if (!output) return false;
    std::memcpy(output.get(), &header, sizeof(header));
    std::memcpy(output.get() + sizeof(header), samples, sampleBytes);
    if (sampleBytes & 1u) output[outputSize - 1] = 0;
    wave.bytes = std::move(output);
    wave.size = outputSize;
    return true;
}

static CustomWave LoadCustomWave(std::wstring path) {
    // Explorer's "Copy as path" supplies one surrounding pair of quotes.
    if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"') {
        path = path.substr(1, path.size() - 2);
    }
    if (path.size() < 7 || !((path[0] >= L'A' && path[0] <= L'Z') ||
                             (path[0] >= L'a' && path[0] <= L'z')) ||
        path[1] != L':' || (path[2] != L'\\' && path[2] != L'/') ||
        path.find(L':', 2) != std::wstring::npos ||
        _wcsicmp(path.c_str() + path.size() - 4, L".wav") != 0) {
        return {};
    }
    const wchar_t root[] = {path[0], L':', L'\\', 0};
    const UINT drive = GetDriveTypeW(root);
    if (drive == DRIVE_REMOTE || drive == DRIVE_UNKNOWN || drive == DRIVE_NO_ROOT_DIR)
        return {};
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return {};
    LARGE_INTEGER length{};
    CustomWave result;
    if (GetFileType(file) == FILE_TYPE_DISK && GetFileSizeEx(file, &length) &&
        length.QuadPart >= static_cast<LONGLONG>(sizeof(WaveHeader)) && length.QuadPart <= maxCustomBytes) {
        const auto size = static_cast<DWORD>(length.QuadPart);
        result.bytes.reset(new(std::nothrow) BYTE[size]);
        DWORD read = 0;
        if (result.bytes && ReadFile(file, result.bytes.get(), size, &read, nullptr) && read == size)
            result.size = size;
    }
    CloseHandle(file);
    if (!PrepareCustomPcm(result)) return {};
    return result;
}

static void StopChime() {
    if (chimeStarted) {
        PlaySoundW(nullptr, nullptr, 0);
        chimeStarted = false;
    }
}

static void ConfigureChime(const std::wstring& path) {
    CustomWave fresh;
    if (!path.empty()) {
        fresh = LoadCustomWave(path);
        if (!fresh.bytes) Wh_Log(L"Custom sound could not be loaded as a supported WAV. Using the built-in chime.");
    }
    if (fresh.size == customChime.size &&
        (!fresh.size || !std::memcmp(fresh.bytes.get(), customChime.bytes.get(), fresh.size))) return;
    if (customChime.bytes) StopChime(); // WinMM may still hold the previous buffer.
    customChime = std::move(fresh);
}

static void PlayChime() {
    const void* wave = customChime.bytes ? customChime.bytes.get() : BuiltInChime();
    if (!wave) return;
    StopChime();
    chimeStarted = !!PlaySoundW(reinterpret_cast<LPCWSTR>(wave), nullptr,
                                SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
}

} // namespace BreakEnvironment
namespace {
using Timing::Millis;
using Timing::Phase;
using Timing::Time;

Time Now() {
    FILETIME ft{};
    GetSystemTimePreciseAsFileTime(&ft);
    ULARGE_INTEGER value{};
    value.LowPart = ft.dwLowDateTime;
    value.HighPart = ft.dwHighDateTime;
    // Time-zone offsets on the target are whole minutes.
    return {static_cast<Millis>(GetTickCount64()), static_cast<Millis>(value.QuadPart / 10000)};
}

struct Preferences {
    int breakFor = 1, breakEvery = 15;
    bool idle = true, allMonitors = false, fullScreen = false, banner = false, sound = true;
    bool doubleClick = false;
    COLORREF normal = RGB(255,180,84), finalText = RGB(255,0,0), trayBg = RGB(32,40,50);
    COLORREF gauge = RGB(88,214,141), clock = RGB(255,255,255), bg = RGB(24,32,40);
    int trayOpacity = 0, windowOpacity = 95;
    int countdownSeconds = 60;
    std::wstring soundFile;
};

std::wstring Duration(int minutes) {
    if (minutes <= 60) return std::to_wstring(minutes) + L" min";
    return std::to_wstring(minutes / 60) + L" h " + std::to_wstring(minutes % 60) + L" min";
}

COLORREF ReadColor(PCWSTR key, COLORREF fallback) {
    auto stored = WindhawkUtils::StringSetting::make(key);
    PCWSTR p = stored.get() + (*stored.get() == L'#');
    if (wcslen(p) == 6 && wcsspn(p, L"0123456789abcdefABCDEF") == 6) {
        const auto value = std::wcstoul(p, nullptr, 16);
        fallback = RGB((value >> 16) & 255, (value >> 8) & 255, value & 255);
    }
    return fallback;
}

Preferences ReadPreferences() {
    Preferences p;
    p.breakFor = std::clamp(Wh_GetIntSetting(L"schedule.breakFor"), 1, 60);
    const int selected = Wh_GetIntSetting(L"schedule.breakEvery");
    const int minimum = std::max(10, 2 * p.breakFor);
    p.breakEvery = std::clamp(selected, minimum, 300);
    p.idle = Wh_GetIntSetting(L"behavior.idle") != 0;
    p.fullScreen = Wh_GetIntSetting(L"behavior.fullScreen") != 0;
    p.banner = Wh_GetIntSetting(L"behavior.banner") != 0;
    p.sound = Wh_GetIntSetting(L"behavior.sound") != 0;
    p.soundFile = WindhawkUtils::StringSetting::make(L"behavior.soundFile").get();
    p.doubleClick = Wh_GetIntSetting(L"behavior.doubleClick") != 0;
    const int countdown = Wh_GetIntSetting(L"tray.countdownSeconds");
    p.countdownSeconds = countdown ? std::clamp(countdown, 5, 60) : 60;
    p.allMonitors = Wh_GetIntSetting(L"behavior.allMonitors") != 0;
    p.normal = ReadColor(L"tray.normal", p.normal);
    p.finalText = ReadColor(L"tray.final", p.finalText);
    p.trayBg = ReadColor(L"tray.background", p.trayBg);
    p.gauge = ReadColor(L"window.gauge", p.gauge);
    p.clock = ReadColor(L"window.text", p.clock);
    p.bg = ReadColor(L"window.background", p.bg);
    p.trayOpacity = std::clamp(Wh_GetIntSetting(L"tray.opacity"), 0, 100);
    p.windowOpacity = std::clamp(Wh_GetIntSetting(L"window.opacity"), 0, 100);
    return p;
}

constexpr UINT kTray = WM_APP + 1, kSkip = WM_APP + 2, kStop = WM_APP + 3;
constexpr wchar_t kControllerClass[] = L"Windhawk.SystemTrayBreakTimer.Controller";
constexpr GUID kTrayGuid = {0xd833e13b,0xecb2,0x45e9,{0xaa,0xb7,0x00,0x8d,0x8d,0x21,0x7e,0x55}};
enum Command : UINT { TakeBreak = 1, ResetSchedule, Pause, Unpause };

std::atomic<HWND> g_controller{};
HANDLE g_stop{}, g_changed{}, g_worker{};

// Explorer can report both the raw button messages and NIN_SELECT for a
// single gesture. Keyboard activation has a separate notification in v4.
class TrayActivation {
    bool buttonDown{}, afterDouble{}, haveRaw{}, haveFirst{};
    DWORD rawTime{}, firstTime{};
    POINT firstPoint{};

public:
    bool accept(UINT event, POINT point, DWORD stamp, bool doubleOnly) {
        if (event == NIN_KEYSELECT) {
            *this = {};
            return true;
        }
        if (event == WM_LBUTTONDOWN) {
            buttonDown = true;
            afterDouble = false;
            return false;
        }
        if (event == WM_LBUTTONDBLCLK) {
            buttonDown = true;
            afterDouble = true;
            haveFirst = false;
            return doubleOnly;
        }
        if (event == WM_LBUTTONUP) {
            buttonDown = false;
            haveRaw = true;
            rawTime = stamp;
            if (afterDouble) {
                afterDouble = false;
                return false;
            }
        } else if (event == NIN_SELECT) {
            if (buttonDown || (haveRaw && stamp - rawTime <= GetDoubleClickTime())) return false;
        } else {
            return false;
        }

        // Some shell hosts send two ordinary clicks without WM_LBUTTONDBLCLK.
        const bool pair = haveFirst && stamp - firstTime <= GetDoubleClickTime() &&
            2 * std::abs(point.x - firstPoint.x) <= GetSystemMetrics(SM_CXDOUBLECLK) &&
            2 * std::abs(point.y - firstPoint.y) <= GetSystemMetrics(SM_CYDOUBLECLK);
        haveFirst = !pair;
        firstTime = stamp;
        firstPoint = point;
        return doubleOnly == pair;
    }
};

class BreakTimer {
    Preferences prefs;
    enum class MenuState { Closed, Paused, Standby, Work, Break };
    MenuState trackedMenu = MenuState::Closed;
    Timing::Schedule timer;
    BreakWindows windows;
    TrayActivation activation;
    HWND hwnd{};
    HINSTANCE instance{};
    HMONITOR breakMonitor{};
    UINT taskbarCreated{};
    bool manual = true, full = false, ignoreFull = false, idle = false;
    bool ignoredIdle = false, locked = false, disconnected = false, sleeping = false;
    bool showing = false, added = false, refreshWindows = false;
    bool controllerClassRegistered = false, sessionNotifications = false, bannerPending = false;
    bool trayVersion4 = false, trackForeground = false;
    HWINEVENTHOOK foregroundHook{};
    static constexpr Millis never = std::numeric_limits<Millis>::max();
    Millis awaySince{}, nextProbe{}, nextIdleProbe{}, nextTrayAttempt{};
    std::wstring lastGlyph, lastTip;
    COLORREF lastFg{};
    UINT lastDpi{};
    int lastOpacity = -1;
    COLORREF lastBg{};

    Millis workMs() const { return prefs.breakEvery * Timing::minute; }
    Millis breakMs() const { return prefs.breakFor * Timing::minute; }
    Millis countdownMs() const { return prefs.countdownSeconds * 1000; }
    bool unavailable() const { return sleeping || locked || disconnected; }
    bool held() const { return manual || unavailable() || (full && !ignoreFull) || idle; }

    MenuState menuState() const {
        if (timer.frozen) return MenuState::Paused;
        if (timer.phase == Phase::Standby) return MenuState::Standby;
        if (timer.phase == Phase::Break) return MenuState::Break;
        return MenuState::Work;
    }

    void applyHold(Time now) {
        if (held()) timer.freeze(now);
        else timer.resume(now, workMs());
    }

    NOTIFYICONDATAW trayData() const {
        NOTIFYICONDATAW data{};
        data.cbSize = sizeof(data);
        data.hWnd = hwnd;
        data.uID = 1;
        data.uFlags = NIF_GUID;
        data.guidItem = kTrayGuid;
        return data;
    }

    void sendBanner() {
        auto data = trayData();
        data.uFlags |= NIF_INFO | NIF_REALTIME;
        data.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND | NIIF_RESPECT_QUIET_TIME;
        wcscpy_s(data.szInfoTitle, L"System Tray BreakTimer");
        wcscpy_s(data.szInfo, L"Breaks paused while an app is full screen.");
        Shell_NotifyIconW(NIM_MODIFY, &data);
    }

    void publishTray(Time now, const std::wstring& glyph, COLORREF fg, const std::wstring& tip) {
        if (now.tick < nextTrayAttempt) return;
        // Explorer owns the primary notification area. Its DPI can differ from the controller's.
        UINT dpi = lastDpi;
        if (!dpi || !added) {
            HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
            dpi = taskbar ? GetDpiForWindow(taskbar) : GetDpiForSystem();
        }
        const bool newIcon = !added || lastGlyph != glyph || lastFg != fg || lastDpi != dpi ||
                             lastOpacity != prefs.trayOpacity || lastBg != prefs.trayBg;
        if (!newIcon && tip == lastTip) {
            nextTrayAttempt = 0;
            return;
        }
        auto data = trayData();
        HICON icon{};
        if (newIcon) {
            icon = MakeTrayIcon(glyph, fg, prefs.trayBg, prefs.trayOpacity, dpi);
            if (!icon) {
                nextTrayAttempt = now.tick + 1800;
                return;
            }
            data.hIcon = icon;
            data.uFlags |= NIF_ICON;
        }
        data.uFlags |= NIF_TIP | NIF_SHOWTIP;
        wcsncpy_s(data.szTip, tip.c_str(), _TRUNCATE);
        if (!added) {
            data.uFlags |= NIF_MESSAGE;
            data.uCallbackMessage = kTray;
        }
        const bool success = Shell_NotifyIconW(added ? NIM_MODIFY : NIM_ADD, &data) != FALSE;
        if (icon) DestroyIcon(icon);
        if (!success) {
            // A recycled shell may no longer know our GUID. Remove any stale
            // registration and retry rather than registering a second icon.
            auto key = trayData();
            Shell_NotifyIconW(NIM_DELETE, &key);
            added = false;
            nextTrayAttempt = now.tick + 1800;
            return;
        }
        if (!added) {
            data.uVersion = NOTIFYICON_VERSION_4;
            trayVersion4 = Shell_NotifyIconW(NIM_SETVERSION, &data) != FALSE;
            activation = {};
            added = true;
        }
        nextTrayAttempt = 0;
        lastGlyph = glyph; lastFg = fg; lastDpi = dpi;
        lastOpacity = prefs.trayOpacity; lastBg = prefs.trayBg; lastTip = tip;
    }

    static void CALLBACK foregroundChanged(HWINEVENTHOOK, DWORD, HWND window,
                                           LONG, LONG, DWORD, DWORD) {
        // An out-of-context hook delivers events to this worker's message loop.
        BreakEnvironment::PreferredMonitor(window);
    }

    void setForegroundTracking(bool enabled) {
        if (enabled == trackForeground) return;
        trackForeground = enabled;
        if (enabled) {
            foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                nullptr, foregroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            BreakEnvironment::PreferredMonitor();
        } else if (foregroundHook) {
            UnhookWinEvent(foregroundHook);
            foregroundHook = nullptr;
        }
    }

    void probe(Time now, bool overrideCurrentEpisode = false) {
        setForegroundTracking(!manual && !unavailable() && !prefs.allMonitors);
        nextProbe = never;
        if (manual || unavailable()) {
            idle = ignoredIdle = full = ignoreFull = bannerPending = false;
            nextIdleProbe = 0;
            applyHold(now);
            return;
        }
        if (trackForeground && !foregroundHook) {
            BreakEnvironment::PreferredMonitor();
            nextProbe = now.tick + 5000;
        }
        const bool previousFull = full;
        full = prefs.fullScreen && BreakEnvironment::FullscreenAppVisible();
        // A window can enter full screen without changing foreground windows.
        if (prefs.fullScreen) nextProbe = std::min(nextProbe, now.tick + 600);
        ignoreFull = full && (ignoreFull || overrideCurrentEpisode);
        const bool previousIdle = idle;
        const bool working = timer.phase == Phase::Work || timer.phase == Phase::Standby;
        if (prefs.idle && (working || overrideCurrentEpisode || ignoredIdle)) {
            if (now.tick >= nextIdleProbe || overrideCurrentEpisode) {
                constexpr Millis threshold = 5 * Timing::minute;
                const Millis elapsed = BreakEnvironment::IdleMilliseconds();
                const bool away = elapsed >= threshold;
                ignoredIdle = away && (ignoredIdle || overrideCurrentEpisode);
                idle = away && !ignoredIdle && working;
                nextIdleProbe = now.tick + (away ? 1000 : threshold - elapsed);
            }
            nextProbe = std::min(nextProbe, nextIdleProbe);
        } else {
            idle = ignoredIdle = false;
            nextIdleProbe = 0;
        }
        if (previousIdle && !idle && !overrideCurrentEpisode)
            timer.startWork(now, workMs());
        if (full && !ignoreFull && !previousFull && prefs.banner && !idle) {
            bannerPending = true;
        }
        if (!full || ignoreFull || !prefs.banner) bannerPending = false;
        applyHold(now);
    }

    static std::wstring clockText(unsigned sec) {
        wchar_t value[32]{};
        std::swprintf(value, std::size(value), L"%02u:%02u", sec / 60, sec % 60);
        return value;
    }

    void render(Time now) {
        // Windows tracks a popup in a nested message loop; its commands are a snapshot.
        if (trackedMenu != MenuState::Closed && trackedMenu != menuState()) EndMenu();
        const bool onBreak = !timer.frozen && timer.phase == Phase::Break;
        const auto remaining = timer.remaining(now);
        const auto secs = static_cast<unsigned>((remaining + 999) / 1000);
        if (onBreak) {
            windows.update(secs, static_cast<unsigned>(timer.breakLength / 1000));
            if (!showing) {
                breakMonitor = BreakEnvironment::PreferredMonitor();
                windows.show(prefs.allMonitors, breakMonitor, hwnd, kSkip);
            } else if (refreshWindows) {
                windows.refresh(prefs.allMonitors, breakMonitor, hwnd, kSkip);
            }
        } else if (showing) {
            windows.hide();
        }
        showing = onBreak;
        refreshWindows = false;

        std::wstring glyph, status;
        COLORREF fg = prefs.normal;
        if (timer.frozen) {
            glyph = L"P";
            if (manual) status = prefs.doubleClick ? L"Paused · Double-click to start a fresh work interval"
                                                  : L"Paused · Click to start a fresh work interval";
            else if (unavailable()) status = L"Paused · Session unavailable";
            else if (full && !ignoreFull) status = L"Paused · Full-screen app";
            else status = L"Paused · Idle";
        } else if (timer.phase == Phase::Standby) {
            glyph = L"S";
            status = L"Standby · " + std::to_wstring(secs) + L" sec until the work timer starts";
        } else if (onBreak) {
            glyph = L"B";
            status = L"On break · " + clockText(secs) + L" remaining";
        } else if (remaining <= countdownMs()) {
            glyph = std::to_wstring(secs);
            fg = prefs.finalText;
            status = L"Next break in " + glyph + (secs == 1 ? L" second" : L" seconds");
        } else {
            const auto mins = static_cast<int>((remaining + Timing::minute - 1) / Timing::minute);
            glyph = std::to_wstring(mins);
            status = L"Next break in " + Duration(mins);
        }
        std::wstring tip = L"System Tray BreakTimer\n" + status;
        if (!timer.frozen && (timer.phase == Phase::Work || timer.phase == Phase::Standby)) {
            ULARGE_INTEGER bits{};
            bits.QuadPart = static_cast<ULONGLONG>(timer.due.wall +
                               (timer.phase == Phase::Standby ? timer.workLength : 0)) * 10000;
            FILETIME utc{bits.LowPart, bits.HighPart};
            SYSTEMTIME systemTime{}, localTime{};
            wchar_t clock[64]{};
            if (FileTimeToSystemTime(&utc, &systemTime) &&
                SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &localTime) &&
                GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &localTime, nullptr,
                                clock, ARRAYSIZE(clock))) {
                tip += L" · "; tip += clock;
            }
        }
        publishTray(now, glyph, fg, tip);
        if (bannerPending) {
            if (added) sendBanner();
            bannerPending = false;
        }
    }

    void tick(Time now) {
        if (!timer.frozen && timer.phase == Phase::Work && now.tick >= timer.due.tick) {
            // Recheck both automatic holds before presenting a scheduled break.
            nextIdleProbe = 0;
            probe(now);
        } else if (now.tick >= nextProbe) {
            probe(now);
        }
        const auto transition = timer.advance(now, workMs(), breakMs());
        if (prefs.sound && transition != Timing::Transition::None) BreakEnvironment::PlayChime();
        if (transition != Timing::Transition::None) nextProbe = nextIdleProbe = 0;
        render(now);
    }

    DWORD timeout(Time now) const {
        Millis delay = INFINITE;
        if (nextProbe != never) delay = std::max<Millis>(1, nextProbe - now.tick);
        if (!added || nextTrayAttempt)
            delay = std::min(delay, std::max<Millis>(1, nextTrayAttempt - now.tick));
        if (!timer.frozen) {
            const auto left = timer.remaining(now);
            const bool displayingMinutes = timer.phase == Phase::Work && left > countdownMs();
            const auto step = displayingMinutes ? Timing::minute : 1000;
            auto next = left > 0 ? (left - 1) % step + 1 : 1;
            if (displayingMinutes) next = std::min(next, left - countdownMs());
            delay = std::min(delay, next);
        }
        return static_cast<DWORD>(delay);
    }

    void wakeSession(Time now, bool wasAway) {
        if (!wasAway && unavailable()) {
            awaySince = now.tick;
            probe(now);
        } else if (wasAway && !unavailable()) {
            if (prefs.idle && !manual && now.tick - awaySince >= 5 * Timing::minute &&
                timer.phase != Phase::Break) {
                timer.startWork(now, workMs());
            }
            nextIdleProbe = 0;
            probe(now);
        }
        applyHold(now);
        render(now);
    }

    void execute(Command cmd) {
        const auto now = Now();
        switch (cmd) {
        case Unpause:
            if (!timer.frozen) return;
            manual = false;
            timer.startWork(now, workMs());
            probe(now, true);
            break;
        case Pause:
            manual = true;
            timer = Timing::Schedule{};
            probe(now);
            break;
        case ResetSchedule:
            manual = false;
            timer.startWork(now, workMs());
            nextProbe = nextIdleProbe = 0;
            break;
        case TakeBreak:
            if (timer.phase == Phase::Break && !timer.frozen) return;
            manual = false;
            timer.startBreak(now, breakMs());
            probe(now, true);
            break;
        }
        applyHold(now);
        tick(now);
    }

    void togglePause() {
        // Choose the action from the state displayed at the time of the click.
        execute(timer.frozen ? Unpause : Pause);
    }

    void appendMenuCommands(HMENU popup) const {
        const auto state = menuState();
        if (state == MenuState::Paused) {
            AppendMenuW(popup, MF_STRING, Unpause, L"Unpause");
            return;
        }
        if (state == MenuState::Work) {
            AppendMenuW(popup, MF_STRING, TakeBreak, L"Skip to next break");
            AppendMenuW(popup, MF_STRING, ResetSchedule, L"Reset");
        }
        AppendMenuW(popup, MF_STRING, Pause, L"Pause");
    }

    void menu(POINT point) {
        if (trackedMenu != MenuState::Closed) { EndMenu(); return; }
        HMENU popup = CreatePopupMenu();
        if (!popup) return;
        const auto openedState = menuState();
        appendMenuCommands(popup);
        if (point.x == -1 && point.y == -1) {
            NOTIFYICONIDENTIFIER id{};
            id.cbSize = sizeof(id);
            id.hWnd = hwnd; id.uID = 1; id.guidItem = kTrayGuid;
            RECT r{};
            if (SUCCEEDED(Shell_NotifyIconGetRect(&id, &r))) point = {r.left, r.top};
            else GetCursorPos(&point);
        }
        // The user explicitly opened this menu. No automatic pause path calls this.
        SetForegroundWindow(hwnd);
        SetTimer(hwnd, 1, 150, nullptr);  // Keep time during the shell menu’s nested message loop.
        trackedMenu = openedState;
        UINT selected = TrackPopupMenuEx(popup, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON |
                                         TPM_RIGHTALIGN | TPM_BOTTOMALIGN, point.x, point.y, hwnd, nullptr);
        trackedMenu = MenuState::Closed;
        KillTimer(hwnd, 1);
        DestroyMenu(popup);
        PostMessageW(hwnd, WM_NULL, 0, 0);
        if (!selected && added) { auto key = trayData(); Shell_NotifyIconW(NIM_SETFOCUS, &key); }
        if (selected && openedState == menuState() && WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0)
            execute(static_cast<Command>(selected));
    }

    LRESULT handle(UINT msg, WPARAM wp, LPARAM lp) {
        if (taskbarCreated && msg == taskbarCreated) {
            added = false; nextTrayAttempt = 0;
            tick(Now());
            return 0;
        }
        switch (msg) {
        case kTray: {
            const UINT event = LOWORD(lp);
            POINT point{GET_X_LPARAM(wp), GET_Y_LPARAM(wp)};
            if (!trayVersion4) {
                const DWORD pos = GetMessagePos();
                point = {GET_X_LPARAM(pos), GET_Y_LPARAM(pos)};
            }
            if (event == WM_CONTEXTMENU || (!trayVersion4 && event == WM_RBUTTONUP)) {
                activation = {};
                menu(trayVersion4 ? POINT{-1, -1} : point);
            } else if (activation.accept(event, point, static_cast<DWORD>(GetMessageTime()), prefs.doubleClick)) {
                togglePause();
            }
            return 0;
        }
        case kSkip:
            if (showing && timer.phase == Phase::Break && !timer.frozen) {
                const auto now = Now();
                timer.startWork(now, workMs());
                nextProbe = nextIdleProbe = 0;
                applyHold(now);
                tick(now);
            }
            return 0;
        case WM_TIMER:
            if (wp == 1) {
                if (WaitForSingleObject(g_stop, 0) == WAIT_OBJECT_0) { EndMenu(); return 0; }
                if (WaitForSingleObject(g_changed, 0) == WAIT_OBJECT_0) reload();
                tick(Now());
                return 0;
            }
            break;
        case kStop:
            EndMenu();
            return 0;
        case WM_TIMECHANGE:
            timer.timeChanged(Now());
            break;
        case WM_DISPLAYCHANGE: case WM_DPICHANGED: case WM_SETTINGCHANGE:
            refreshWindows = true; lastDpi = 0; nextProbe = 0; break;
        case WM_WTSSESSION_CHANGE: {
            const bool wasAway = unavailable();
            if (wp == WTS_SESSION_LOCK) locked = true;
            if (wp == WTS_SESSION_UNLOCK) locked = false;
            if (wp == WTS_CONSOLE_DISCONNECT || wp == WTS_REMOTE_DISCONNECT) disconnected = true;
            if (wp == WTS_CONSOLE_CONNECT || wp == WTS_REMOTE_CONNECT) disconnected = false;
            wakeSession(Now(), wasAway);
            return 0;
        }
        case WM_POWERBROADCAST: {
            const bool wasAway = unavailable();
            if (wp == PBT_APMSUSPEND) sleeping = true;
            if (wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND ||
                wp == PBT_APMRESUMECRITICAL) sleeping = false;
            wakeSession(Now(), wasAway);
            return TRUE;
        }
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    static LRESULT CALLBACK dispatch(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
        auto* self = reinterpret_cast<BreakTimer*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (msg == WM_NCCREATE) {
            self = static_cast<BreakTimer*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
            self->hwnd = window;
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        return self ? self->handle(msg, wp, lp) : DefWindowProcW(window, msg, wp, lp);
    }

    void reload() {
        auto next = ReadPreferences();
        if (prefs.sound && !next.sound) BreakEnvironment::StopChime();
        if (next.sound && (!prefs.sound || next.soundFile != prefs.soundFile))
            BreakEnvironment::ConfigureChime(next.soundFile);
        if (prefs.doubleClick != next.doubleClick) activation = {};
        prefs = std::move(next);
        windows.setVisuals(prefs.gauge, prefs.clock, prefs.bg, prefs.windowOpacity);
        refreshWindows = true; nextProbe = nextIdleProbe = 0;
    }

public:
    bool initialize() {
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                kControllerClass, &instance)) return false;
        if (!windows.init()) return false;
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc); wc.hInstance = instance;
        wc.lpfnWndProc = dispatch; wc.lpszClassName = kControllerClass;
        if (!RegisterClassExW(&wc)) return false;
        controllerClassRegistered = true;
        taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
        hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClass, L"System Tray BreakTimer",
                               WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, this);
        if (!hwnd) return false;
        sessionNotifications = WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION) != FALSE;
        g_controller.store(hwnd);
        return true;
    }

    void run() {
        reload();
        const HANDLE events[] = {g_stop, g_changed};
        while (WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
            const auto now = Now();
            tick(now);
            const DWORD result = MsgWaitForMultipleObjectsEx(2, events, timeout(Now()), QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
            if (result == WAIT_OBJECT_0 + 1) reload();
            MSG msg{};
            // Bound a batch so continuous external broadcasts cannot starve the deadline.
            for (int n = 0; n < 64 && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE); ++n) {
                if (msg.message == WM_QUIT) return;
                if (windows.handleKey(msg)) continue;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    void shutdown() {
        setForegroundTracking(false);
        g_controller.store(nullptr);
        if (added) { auto data = trayData(); Shell_NotifyIconW(NIM_DELETE, &data); }
        windows.shutdown();
        BreakEnvironment::StopChime();
        if (sessionNotifications) WTSUnRegisterSessionNotification(hwnd);
        if (hwnd) DestroyWindow(hwnd);
        if (controllerClassRegistered) UnregisterClassW(kControllerClass, instance);
    }
};

DWORD WINAPI Worker(void*) {
    BreakTimer app;
    if (!app.initialize()) {
        const DWORD error = GetLastError();
        Wh_Log(L"Couldn't initialize the timer windows (error %lu). Closing the dedicated tool process.", error);
        app.shutdown();
        // This worker exists only in our dedicated tool process. The launcher
        // calls WhTool_ModInit before the process entry point, so avoid waiting
        // there for a newly created GUI thread to start.
        ExitProcess(error ? error : ERROR_GEN_FAILURE);
    }
    app.run();
    app.shutdown();
    return 0;
}

void CloseHandleIfSet(HANDLE& h) {
    if (h) { CloseHandle(h); h = nullptr; }
}
}  // namespace

BOOL WhTool_ModInit() {
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_changed = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stop && g_changed) g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (g_worker) return TRUE;
    CloseHandleIfSet(g_changed);
    CloseHandleIfSet(g_stop);
    return FALSE;
}

void WhTool_ModSettingsChanged() {
    SetEvent(g_changed);
}

void WhTool_ModUninit() {
    if (g_stop) SetEvent(g_stop);
    if (HWND window = g_controller.load()) PostMessageW(window, kStop, 0, 0);
    if (g_worker) WaitForSingleObject(g_worker, INFINITE);
    CloseHandleIfSet(g_worker);
    CloseHandleIfSet(g_changed);
    CloseHandleIfSet(g_stop);
}

// CC0 1.0 audio data. GnoteSoundz, Wind Chime (G-003) +.wav.
// https://freesound.org/people/GnoteSoundz/sounds/169855/
// Original WAV SHA-256: 065fce66f92e59c6512b5b8b7659f704089a1adedbbda30ec4ac392e52b64f0c
// Decoded lazily by BuiltInChime. All 148746 stereo frames are losslessly preserved.
namespace BreakEnvironment {
static std::string_view PackedRecording() {
    static constexpr char audio[] =
"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"
"yG4X0J6JMSiGByDgmYoUaBE5jDwjYGEQWWGGyEExj5ChJSClA7hFiEwgUJgOGIqHN1Jqyth2VtATgSBMwDwiME+AEjUwtpkkBeADEhLWILhVIZEc7E5sVSm4"
"z0QEViVJDQBgSiIYuOTJTmFRlQQQBUOeDFCGyM4S0B6KXC9iQhRw0IJgR8CmAYRVYUMAmE/MDgkj2g0gwGCEDBJwjQmzLRKALMWjikdBEFQ4YRcRacI7IEXB"
"iglDQEDzNyVQfybzBgdjbAWCgjbEK7pcbTLSSxQQaAzidAChHkHDfQ14T3QFIkJnlK2FQDX4TwLSf0kwATjCijUhZqZpHwlzgWQVKSBkahqinATgEEMVcScR"
"WRG4QjlAkCYCOBDEUwsRiYDVEDHGej1EhHIUQYGTYXsyQ2fpiIQDaKqvIsjXm4I1JQVQKhzAuEMBCmKlPYvXm6BzKgGkZBrjbKEwhFVVdG8X0Ddj6XAHpG4U"
"BAkUN3un8FcQwFBF4WgIIUAUkYVVdj8WdB0YQKxkhxJEIAMJsL0B8CQg0BxrB7AAllkhsSkJ0Yeiw4BEICwH4U5FEptlNS01fB0GQo4Epw5AGCOXtIsEcQMR"
"hlSV5EhxohklcYNFkCUgsQwmhYCCQiGSeiFXsJMBdlqSjBq1xoBRkIUkMgFEcV8zgqAkIRsCIzQllKtVFCpRWCEUopACQEsSkwtFkn8iED4jAw5DRzpgs7UK"
"IQimBSUFIAGiA8IHgghBBYYCSQVqBuEE5wdoB6AAAgnFBwAAggIBBwADaA5jCWQCwQBmByECQBADB4EI4gPhBYMCohCgBuMKAgZCBeEBAg0EBUMLoghhBYIB"
"gQtmAuMMQQnFBKoB4AhDAQYJJwhkA8cCyAKjACIHwAWCASYB5wAGASAC4AGnAIICQASCBEMAwACBBAIBBQLlAqYFQAUCAeIABwkAB0IEpAPjBsIIAwegBSQL"
"hwGCBowoay/GCYVDrDyDAsM5sEnNIWRJJJQDcqJJQV+kc8NdoWbAlGHEg77mf8Rj5XrCaWJbBE9jOYMhZBpFCcIP4wHkAcMKQADktALiCpQQIBYArQQlSrCh"
"kGJA0cnOHDUp0BGMYgFIMpZG+0WRU7l36IrUFEI2UY0C2EmUPKVKRC/MGeBEGJQCtcFOAxEFYGOdA7Ab8C3EDwgfqP0DJIFBwFCamCQAMRRymH8A80k4jtkY"
"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"
"AAETMwoQkQgSkQISEAAAHEQSgAgAAFQCigTkQUAAAAQDpTAECAgUGgUAQgEwg0wATgwDEAQADEygAkzEQlTACAigUykCACkigUykSkSlQiCAikSgGESEACTA"
"AUXowXgAWkASCiOKAKANQgqABSZAABSlRRDAACQNQOV5NSoo4magqRyakqCgClVFhQY8VFQRDgFSVAUNQJo1PUeVAxENIBBQQCFNSgiSqOlBCJIUoEqKpqAz"
"ADNEAAwFEIgAolAExExN4MAwEhEyUIkJoAEAhABEIqVxEgJyMxMIpMEpMQABFAIAEwlAhEQpEIEIBMEQCEEJMIAEQAhBAgJMoEIIEyEBBAApUQEICNAABEwA"
"BMQiAAAoFEw0QgQAQAABAQJIkjKISEMkAAoSgECSRIywBKCRtACFAgQkkQMoBBASgBAhQEEIACSAIorRFHBAWyMAAAgASAASQkgIBEQCQwYIC4AAQKEEQAAA"
"RICCAACQFAUAlEZIgUAISCEASAkkIEkCAlEpIojaAzMxUwBNxEhEpMAAIAyVwAmEokBEpEEgQoEoINAgIBEgEAIEAohExMFwEIMxAhEIEEwpNxEIpMhABM5E"
"kBF5MIExEpEEQhAFMIApM5FEIEoxAgKNwEBABEopFMohAAopAIIzBRCAIQBeTMATAKjeRAaQAjKCDMRBAERBCCjIQKhKAjCKQARARKgIAQQABKCCBSRACQAT"
"KEBSAKTCRMTcCRARhCBKKACBAQTERKIRRCCBhACBACACIAElAKCBAACRCTCQM0GggQAQkwggQSgACkwAkykTEyggQDEzBESAgzJXgkwgUXkwQjEykEWkGEXA"
"AU0CgCgAwADEwAECQggHAUSAgCEQCijEyoSkCAQiAAiAEQiBCkwACAHo3AEwDAAQHAgZQDMEQEwAANExAkBEgxMFwIOAhFqEBwEIAgIEIgIoAEoJMEpAEwEh"
"EIB5MEAAJpAExEhF5FFwMIEJMpEAAqMoAIFN5MJEoIAAFAAhMRwBMAwhEIgKMoEoGEgwEIBMIAxEhEAzRKACARIAjKTMBBejAACAKBQJRRMBARCIACKTMlKR"
"KAQAQKTCIKhABcRQBKETeAQAAKAABTCCITAEBMTCMBBCTCACTABCAICAlcREDQIRMTRAMACTAMQIRAIhcAQABKQIM0QQowTgEzAYQXgAISkykSkUQAECIQQi"
"kCREykQigESETAEATEygUQiEAEAQCgEUwDgSoSBAyowAiAgEygiAEACAgTEQCAQggQkwXkxAQkygkykASIAAAXAgUgwAig0DAzMwBAoBMpAoFAAAIEBAExAg"
"JEAhMA6MEEIEoFIoFAEEBExEEQEEApAhEopNEoIFEgJAoFMAApEIEoFEIExFxEAAgSMEpAApEoAJEAwoMJEExMwNAIAFEEIAAoFEAIgIFMAzMABjAIAjeB";
    return {audio, sizeof(audio) - 1};
}
} // namespace BreakEnvironment

#if !__has_include(<windhawk_tool_mod.h>)
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
#endif
