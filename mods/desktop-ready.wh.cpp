// ==WindhawkMod==
// @id              desktop-ready
// @name            Desktop Ready
// @description     A sign-in splash that hides your desktop until your apps are actually ready - as a status card, a minimal progress screen or a Linux-style boot console
// @version         1.0.0
// @author          CarsonKopec
// @github          https://github.com/CarsonKopec
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -luser32 -ladvapi32 -lshell32 -lshcore -lwtsapi32 -lole32 -ldwmapi -lnetapi32 -liphlpapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Ready

A loading screen for your desktop. The moment Explorer starts after you sign
in, Desktop Ready covers every monitor, shows a live checklist of the apps
that make up your desktop, and fades away once they are actually ready. No
more half-tiled windows, missing bars or a black wallpaper while everything
starts up.

Because it runs inside Explorer through Windhawk, it appears earlier than any
startup app or scheduled task could.

![Card style](https://raw.githubusercontent.com/CarsonKopec/windhawk-desktop-ready/main/screenshots/card.png)

![Console style](https://raw.githubusercontent.com/CarsonKopec/windhawk-desktop-ready/main/screenshots/console.png)

## Getting started

Out of the box it waits for Explorer's desktop and taskbar, so it simply covers
the screen while Windows finishes drawing them. It gets useful when you add the
apps your desktop depends on: open the mod's settings, add them under **Apps**
and save. A preview appears every time you save (nothing is launched or
checked during a preview).

### Examples

**GlazeWM** (window manager), ready when it answers a status query:
- Process name(s): `glazewm.exe`
- Ready when: *The command below succeeds*
- Command: `"C:\Program Files\glzr.io\GlazeWM\cli\glazewm.exe" query app-metadata`
- Command output contains: `"success":true`

**Zebar** (status bar), ready when a bar is on every monitor:
- Process name(s): `zebar.exe`
- Ready when: *It shows a window on every monitor*
- Window title contains: `Zebar - ` (or the name of your widget)

**Wallpaper Engine**, ready when the wallpaper is attached to the desktop:
- Process name(s): `wallpaper64.exe, wallpaper32.exe, webwallpaper64.exe, webwallpaper32.exe`
- Ready when: *It attached a wallpaper to the desktop*

**Windhawk** (or any Windows service):
- Process name(s): leave empty
- Ready when: *The Windows service below is running*
- Service name: `Windhawk`

**Any app with a visible window**, such as Rainmeter:
- Process name(s): `Rainmeter.exe`
- Ready when: *It shows a window*

Tick **Optional** for apps you want listed but not waited for.

## What "ready" means

| Ready when... | Ready once... |
|---|---|
| The process is running | any listed process is running in your session |
| It shows a window | it has a visible window (optional title/class filter) |
| It shows a window on every monitor | its windows cover every connected monitor |
| It attached a wallpaper | it has a window attached to the desktop wallpaper layer |
| The Windows service is running | the named service is running |
| The command succeeds | a hidden command exits with code 0 (and prints the expected text) |

All required apps must stay ready for the *settle time* before the splash
closes. Checks that cannot run (for example a service that does not exist) are
shown as *Unavailable* and do not hold the splash open. It always closes after
the *maximum wait*, and can be skipped with the button, Esc, Enter or Space.

## Customizing

- **Style**: `Card` (a checklist card), `Minimal` (headline, progress bar and
  app chips) or `Console` (a Linux-style boot log, see below).
- **Position**: center, top, bottom or a corner. Combine a corner with
  *Backdrop opacity* `0` for a small progress toast that lets you click
  through to the desktop around it.
- **Themes**: Midnight, Graphite, Catppuccin Mocha, Nord, Gruvbox, Rosé Pine,
  OLED black and Daylight. Any color can be overridden with `#RRGGBB`, and
  the accent can follow Windows with `system`.
- **Background**: theme gradient, solid color, any image, or your current
  Windows wallpaper, with adjustable dimming.
- **Text**: every line is editable and supports these tokens:
  `{greeting}` `{user}` `{time}` `{date}` `{ready}` `{total}` `{seconds}`.
- **Icons**: point an app's icon at an `.exe`, `.dll`, `.ico` or image file,
  type one or two characters, or use a Segoe Fluent Icons glyph such as
  `glyph:E7F4`.

## Console style

A Linux boot, from kernel messages to a login prompt:

- A penguin per CPU thread at the top, like the framebuffer console.
- dmesg-style kernel messages built from your PC: Windows version and boot
  options, BIOS, CPU, memory, GPUs with their PCI IDs, display modes, mounted
  volumes and network links.
- systemd status lines (`Starting GlazeWM...`, `[  OK  ] Started GlazeWM.`,
  `[FAILED]`, `[ TIME ]`, `[DEPEND]`, `Reached target Graphical Interface.`)
  and the bouncing `A start job is running for...` line when an app is slow.
- An agetty-style ending: `yourpc login: you (automatic login)`, your previous
  sign-in time and a blinking cursor, before the desktop appears.
- Chunky, unsmoothed pixels in classic VGA colors, or crisp or smooth text in
  your theme's colors.

*Verbose details* adds what every check found: process IDs, matching windows,
service states and command exit codes. Click or press Esc to skip.

Whatever the style, the same log is saved to `latest-run.log` in the mod's
storage folder and to the Windhawk log, so you can see what happened during
your last sign-in.

## Starting apps

An app's *Start this program if not running* field launches it at sign-in, as
soon as Explorer starts (Windows delays its own startup entries by a few
seconds). Use either this or the app's own startup entry, not both, or it may
start twice.

## GlazeWM

The splash uses tool windows, which GlazeWM does not tile. If you want an
explicit rule anyway, match the window class `WindhawkDesktopReadySplash`:

```yaml
- commands: ['ignore']
  match:
    - window_class: { equals: 'WindhawkDesktopReadySplash' }
```

## Notes

- Shown once per sign-in by default. Enabling the mod mid-session shows a
  one-time preview instead, and Explorer restarts don't show it again unless
  you choose otherwise.
- Readiness comes from process, window, service and command information only.
  Nothing is captured from the screen and nothing is sent over the network.
- A *command* check runs its command at every check interval (500 ms by
  default) until the splash closes.
- The Console style shows your PC name, user name and hardware details on
  screen. They are only displayed locally and written to the log files above.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- behavior:
  - showOn: signIn
    $name: Show the splash
    $options:
    - signIn: Once per sign-in
    - explorerStart: Every time Explorer starts (also after Explorer restarts)
    - previewOnly: Only as a preview when settings are saved
  - previewOnSave: true
    $name: Preview after saving settings
    $description: Shows a simulated splash whenever you save settings, so you can see your changes. Nothing is launched or checked during a preview.
  - maxWaitSeconds: 30
    $name: Maximum wait (seconds)
    $description: The splash always closes after this long, ready or not (5-300).
  - minDisplayMs: 2500
    $name: Minimum display time (ms)
    $description: Keeps the splash up at least this long, even if everything is already running.
  - stableMs: 2000
    $name: Settle time (ms)
    $description: Every required app must stay ready for this long before the splash closes.
  - readyHoldMs: 600
    $name: Ready message hold (ms)
    $description: How long the "ready" headline stays on screen before the fade-out.
  - pollIntervalMs: 500
    $name: Check interval (ms)
  - allowSkip: true
    $name: Allow skipping
    $description: Shows a skip button and lets Esc, Enter or Space dismiss the splash. Apps keep starting in the background.
  - clickAnywhereToSkip: false
    $name: Click anywhere to skip
  - monitors: all
    $name: Monitors
    $options:
    - all: Cover every monitor, with the checklist on each
    - cardOnPrimary: Cover every monitor, checklist on the primary only
    - primary: Primary monitor only
  - keepOnTop: true
    $name: Stay above always-on-top bars
    $description: Re-asserts the splash above other topmost windows (Zebar, YASB, etc.) every second.
  - busyCursor: true
    $name: Busy cursor while waiting
  $name: Behavior
- appearance:
  - style: card
    $name: Style
    $options:
    - card: Card - a checklist with live status
    - minimal: Minimal - headline, progress bar and app chips
    - console: Console - a verbose Linux-style boot log
  - position: center
    $name: Position
    $options:
    - center: Center
    - top: Top
    - bottom: Bottom
    - topLeft: Top left
    - topRight: Top right
    - bottomLeft: Bottom left
    - bottomRight: Bottom right
  - theme: midnight
    $name: Theme
    $options:
    - midnight: Midnight (dark teal)
    - graphite: Graphite
    - catppuccin: Catppuccin Mocha
    - nord: Nord
    - gruvbox: Gruvbox
    - rosepine: Rose Pine
    - oled: OLED black
    - daylight: Daylight (light)
  - background: gradient
    $name: Background
    $options:
    - gradient: Theme gradient with soft glow
    - solid: Solid theme color
    - image: Image file (below)
    - wallpaper: Current Windows wallpaper
  - backgroundImage: ""
    $name: Background image
    $description: Full path to a PNG, JPG or BMP file. Environment variables such as %USERPROFILE% work.
  - backgroundDim: 55
    $name: Image dimming (%)
    $description: Darkens (or, with a light theme, lightens) an image or wallpaper background so the text stays readable.
  - backdropOpacity: 100
    $name: Backdrop opacity (%)
    $description: 100 hides the desktop completely. Lower values let it show through. At 0 only the card is drawn and clicks pass through around it.
  - cardOpacity: 100
    $name: Card opacity (%)
  - scale: 100
    $name: Size (%)
    $description: Scales everything on top of the monitor's own display scaling (50-300).
  - cardWidth: 760
    $name: Card width (px at 100%)
  - cornerRadius: 18
    $name: Corner radius (px)
  - font: Segoe UI
    $name: Font family
    $description: Any installed font, e.g. Segoe UI Variable Display, Inter, JetBrains Mono. Falls back to Segoe UI.
  - fadeInMs: 250
    $name: Fade-in (ms)
  - fadeOutMs: 450
    $name: Fade-out (ms)
  - reduceMotion: false
    $name: Reduce motion
    $description: Turns off spinners, slides and check-mark animations.
  $name: Appearance
- elements:
  - showClock: false
    $name: Show a clock
  - showBrand: true
    $name: Show the brand line
  - showIcons: true
    $name: Show app icons
  - showDetails: true
    $name: Show live status details
  - showStatusLabels: true
    $name: Show status labels
  - showProgressBar: true
    $name: Show progress bar
  - showCountdown: true
    $name: Show time remaining
  $name: Elements
- console:
  - colors: classic
    $name: Colors
    $options:
    - classic: Classic Linux console (black, VGA colors)
    - theme: Follow the theme
  - rendering: retro
    $name: Text rendering
    $options:
    - retro: Retro - chunky unsmoothed pixels, like a VGA framebuffer
    - crisp: Crisp - sharp and unsmoothed at full resolution
    - smooth: Smooth - anti-aliased, with bold text
  - font: Cascadia Mono
    $name: Font
    $description: A monospaced font such as Cascadia Mono, Consolas or JetBrains Mono. Falls back to Consolas.
  - fontSize: 13
    $name: Font size
    $description: In pixels at 100% scaling. In Retro mode, in framebuffer pixels (each drawn 2-3 screen pixels wide).
  - timestamps: kernel
    $name: Timestamps
    $options:
    - kernel: Kernel messages only, like a real boot
    - all: Every line
    - none: None
  - kernelLog: true
    $name: Kernel messages
    $description: Starts with a dmesg-style summary of your real hardware - kernel version, boot options, BIOS, CPU, memory, GPUs, displays, drives and network links.
  - logo: true
    $name: Boot logo
    $description: One penguin per CPU thread at the top, like the Linux framebuffer console.
  - verbose: true
    $name: Verbose details
    $description: Adds what each check found - process IDs, matching windows, service states and command exit codes. Also applies to latest-run.log.
  - welcome: true
    $name: Welcome banner
    $description: Shows "Welcome to Windows 11!" like a Linux distribution's boot banner.
  - statusLine: true
    $name: Animated start-job line
    $description: Shows systemd's "A start job is running for..." line with its bouncing asterisks when an app is slow.
  - loginPrompt: true
    $name: Login prompt
    $description: Ends with a tty "login:" prompt, your previous sign-in time and a blinking cursor before the desktop appears.
  $name: Console style
  $description: Settings for Appearance > Style > Console.
- text:
  - brand: DESKTOP READY
    $name: Brand line
  - headline: Getting your desktop ready
    $name: Headline
  - readyHeadline: Your desktop is ready
    $name: Headline when ready
  - subheading: "{greeting}. A little order before you begin."
    $name: Subheading
    $description: "Tokens: {greeting} {user} {time} {date} {ready} {total} {seconds}. Leave empty to hide."
  - timeoutSubheading: A few things may still be starting.
    $name: Subheading after the maximum wait
  - progress: "{ready} of {total} apps ready"
    $name: Progress line
  - countdown: Up to {seconds} seconds remaining
    $name: Countdown line
  - opening: Opening your desktop
    $name: Closing line
  - skipButton: Skip for now
    $name: Skip button
  - hint: Your desktop stays yours. Skip whenever you like.
    $name: Footnote
    $description: Leave empty to hide.
  $name: Text
- colors:
  - accent: ""
    $name: Accent
    $description: "#RRGGBB, or \"system\" to use your Windows accent color."
  - background: ""
    $name: Background (top)
  - backgroundSecondary: ""
    $name: Background (bottom)
  - surface: ""
    $name: Card
  - border: ""
    $name: Card border
  - text: ""
    $name: Text
  - subtext: ""
    $name: Secondary text
  - ready: ""
    $name: Ready
  - starting: ""
    $name: Starting
  - waiting: ""
    $name: Waiting
  - unavailable: ""
    $name: Unavailable
  $name: Color overrides
  $description: "Optional #RRGGBB colors that override the theme. Leave a field empty to keep the theme's color."
- items:
  - - name: Desktop
      $name: Name
    - process: explorer.exe
      $name: Process name(s)
      $description: Executable name(s), comma separated, e.g. glazewm.exe. May be empty for service and command checks.
    - check: window
      $name: Ready when
      $options:
      - process: The process is running
      - window: It shows a window (title/class filters below)
      - windowEveryMonitor: It shows a window on every monitor (status bars)
      - wallpaper: It attached a wallpaper to the desktop
      - service: The Windows service below is running
      - command: The command below succeeds
    - windowTitle: ""
      $name: Window title contains
    - windowClass: Progman
      $name: Window class is
    - service: ""
      $name: Service name
    - command: ""
      $name: Command
      $description: Runs hidden. Ready when it exits with code 0 and its output contains the text below (spaces are ignored).
    - commandOutput: ""
      $name: Command output contains
    - launch: ""
      $name: Start this program if not running
      $description: Optional full path. Started once at sign-in if none of the processes are running. Do not also keep the app's own startup entry.
    - launchArgs: ""
      $name: Start arguments
    - icon: "glyph:E7F4"
      $name: Icon
      $description: Path to an .exe, .dll, .ico or image, one or two characters, or glyph:XXXX for a Segoe Fluent Icons glyph. Empty uses the program above, then the first letter.
    - subtitle: Icons and wallpaper
      $name: Fixed subtitle
      $description: Replaces the live status text under the name.
    - optional: false
      $name: Optional
      $description: Optional apps are shown but never keep the splash open.
  - - name: Taskbar
    - process: explorer.exe
    - check: window
    - windowTitle: ""
    - windowClass: Shell_TrayWnd
    - service: ""
    - command: ""
    - commandOutput: ""
    - launch: ""
    - launchArgs: ""
    - icon: "glyph:E8A9"
    - subtitle: Start, tray and pinned apps
    - optional: true
  $name: Apps
  $description: The apps to wait for, top to bottom (up to 12). The mod's details page has ready-made examples for GlazeWM, Zebar, Wallpaper Engine and more.
*/
// ==/WindhawkModSettings==

#include <windows.h>
// Windhawk includes windows.h first; that order is fine with mingw-w64.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#pragma clang diagnostic pop
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <lm.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <tlhelp32.h>
#include <wtsapi32.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace gp = Gdiplus;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Small utilities

constexpr wchar_t kWindowClass[] = L"WindhawkDesktopReadySplash";
constexpr UINT kMsgRebuild = WM_APP + 0x31;
constexpr UINT kFrameMs = 33;
constexpr int kMaxItems = 12;

template <typename T>
T Clamp(T v, T lo, T hi) {
    return v < lo ? lo : (hi < v ? hi : v);
}

template <typename T>
T Larger(T a, T b) {
    return a < b ? b : a;
}

template <typename T>
T Smaller(T a, T b) {
    return b < a ? b : a;
}

float EaseOut(float t) {
    t = Clamp(t, 0.f, 1.f);
    float u = 1.f - t;
    return 1.f - u * u * u;
}

std::wstring Lower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool EqualsI(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

bool ContainsI(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty()) {
        return true;
    }
    return Lower(haystack).find(Lower(needle)) != std::wstring::npos;
}

std::wstring ReplaceAll(std::wstring s,
                        const std::wstring& from,
                        const std::wstring& to) {
    if (from.empty()) {
        return s;
    }
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::wstring::npos;
         pos += to.size()) {
        s.replace(pos, from.size(), to);
    }
    return s;
}

std::wstring ExpandEnv(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) {
        return s;
    }
    DWORD needed = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!needed) {
        return s;
    }
    std::wstring out(needed, L'\0');
    DWORD written = ExpandEnvironmentStringsW(s.c_str(), out.data(), needed);
    if (!written || written > needed) {
        return s;
    }
    out.resize(written - 1);
    return out;
}

std::wstring CleanPath(const std::wstring& raw) {
    std::wstring s = Trim(raw);
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"') {
        s = s.substr(1, s.size() - 2);
    }
    return ExpandEnv(s);
}

bool FileExists(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<std::wstring> SplitProcessList(const std::wstring& s) {
    std::vector<std::wstring> out;
    size_t start = 0;
    while (start <= s.size()) {
        size_t end = s.find_first_of(L",;", start);
        if (end == std::wstring::npos) {
            end = s.size();
        }
        std::wstring part = Lower(Trim(s.substr(start, end - start)));
        if (!part.empty()) {
            if (part.find(L'.') == std::wstring::npos) {
                part += L".exe";
            }
            out.push_back(part);
        }
        start = end + 1;
    }
    return out;
}

std::wstring SettingString(const std::wstring& key) {
    PCWSTR value = Wh_GetStringSetting(key.c_str());
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return result;
}

int SettingInt(const std::wstring& key) {
    return Wh_GetIntSetting(key.c_str());
}

int PickOption(const std::wstring& value,
               std::initializer_list<const wchar_t*> options) {
    int index = 0;
    for (const wchar_t* option : options) {
        if (EqualsI(value, option)) {
            return index;
        }
        index++;
    }
    return 0;
}

std::wstring JsonEscape(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        switch (c) {
            case L'"':
                out += L"\\\"";
                break;
            case L'\\':
                out += L"\\\\";
                break;
            case L'\n':
                out += L"\\n";
                break;
            case L'\r':
                out += L"\\r";
                break;
            case L'\t':
                out += L"\\t";
                break;
            default:
                if (c < 0x20) {
                    wchar_t buf[8];
                    swprintf(buf, 8, L"\\u%04x", (unsigned)c);
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Colors and themes

struct Rgb {
    float r = 0, g = 0, b = 0;
};

Rgb HexRgb(uint32_t v) {
    return {((v >> 16) & 255) / 255.f, ((v >> 8) & 255) / 255.f,
            (v & 255) / 255.f};
}

Rgb Mix(const Rgb& a, const Rgb& b, float t) {
    return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t};
}

BYTE Channel(float v) {
    return (BYTE)Clamp((int)std::lround(v * 255.f), 0, 255);
}

gp::Color ToColor(const Rgb& c, float alpha = 1.f) {
    return gp::Color(Channel(Clamp(alpha, 0.f, 1.f)), Channel(c.r),
                     Channel(c.g), Channel(c.b));
}

bool ParseHexColor(std::wstring s, Rgb* out) {
    s = Trim(s);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() == 3) {
        s = std::wstring{s[0], s[0], s[1], s[1], s[2], s[2]};
    }
    if (s.size() != 6) {
        return false;
    }
    for (wchar_t c : s) {
        if (!iswxdigit(c)) {
            return false;
        }
    }
    *out = HexRgb((uint32_t)wcstoul(s.c_str(), nullptr, 16));
    return true;
}

bool SystemAccentColor(Rgb* out) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        // Stored as 0xAABBGGRR.
        *out = {(value & 0xFF) / 255.f, ((value >> 8) & 0xFF) / 255.f,
                ((value >> 16) & 0xFF) / 255.f};
        return true;
    }
    DWORD colorization = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        *out = HexRgb(colorization & 0xFFFFFF);
        return true;
    }
    return false;
}

struct Palette {
    Rgb bg1, bg2, glow, surface, border, text, subtext, accent, ready, busy,
        waiting, error;
    float glowStrength = 1.f;
    bool light = false;
};

struct ThemeDef {
    const wchar_t* id;
    // bg1, bg2, glow, surface, border, text, subtext, accent, ready, busy,
    // waiting, error
    uint32_t c[12];
    float glow;
    bool light;
};

const ThemeDef kThemes[] = {
    {L"midnight",
     {0x09121C, 0x050B13, 0x22738B, 0x101C29, 0x2B3F51, 0xEBF4F9, 0x899EB1,
      0x63DEE5, 0x65D9C1, 0x69CDF1, 0x688194, 0xD0AB6F},
     1.f,
     false},
    {L"graphite",
     {0x161618, 0x0B0B0C, 0x5E5CE6, 0x1C1C1E, 0x323236, 0xF2F2F7, 0x9A9AA1,
      0x0A84FF, 0x30D158, 0x64D2FF, 0x6E6E73, 0xFF9F0A},
     0.8f,
     false},
    {L"catppuccin",
     {0x181825, 0x11111B, 0xCBA6F7, 0x1E1E2E, 0x313244, 0xCDD6F4, 0xA6ADC8,
      0xCBA6F7, 0xA6E3A1, 0x89B4FA, 0x6C7086, 0xFAB387},
     0.9f,
     false},
    {L"nord",
     {0x2E3440, 0x242933, 0x88C0D0, 0x3B4252, 0x4C566A, 0xECEFF4, 0xA9B3C4,
      0x88C0D0, 0xA3BE8C, 0x81A1C1, 0x7B869A, 0xEBCB8B},
     0.7f,
     false},
    {L"gruvbox",
     {0x282828, 0x1D2021, 0xD79921, 0x32302F, 0x504945, 0xEBDBB2, 0xA89984,
      0xFABD2F, 0xB8BB26, 0x83A598, 0x7C6F64, 0xFE8019},
     0.7f,
     false},
    {L"rosepine",
     {0x191724, 0x12101B, 0xC4A7E7, 0x1F1D2E, 0x403D52, 0xE0DEF4, 0x908CAA,
      0xEBBCBA, 0x9CCFD8, 0xC4A7E7, 0x6E6A86, 0xF6C177},
     0.9f,
     false},
    {L"oled",
     {0x000000, 0x000000, 0x000000, 0x0A0A0A, 0x262626, 0xFFFFFF, 0x8C8C8C,
      0xFFFFFF, 0x7EE787, 0x79C0FF, 0x5A5A5A, 0xF0B72F},
     0.f,
     false},
    {L"daylight",
     {0xF4F7FB, 0xE3EAF2, 0x7FB3E6, 0xFFFFFF, 0xD3DCE6, 0x16202A, 0x5A6A7B,
      0x0F6CBD, 0x0E7A5F, 0x0F6CBD, 0x8C99A8, 0xB8650A},
     2.2f,
     true},
};

Palette PaletteFromTheme(const std::wstring& id) {
    const ThemeDef* def = &kThemes[0];
    for (const auto& theme : kThemes) {
        if (EqualsI(id, theme.id)) {
            def = &theme;
        }
    }
    Palette p;
    Rgb* slots[] = {&p.bg1,     &p.bg2,    &p.glow,   &p.surface,
                    &p.border,  &p.text,   &p.subtext, &p.accent,
                    &p.ready,   &p.busy,   &p.waiting, &p.error};
    for (int i = 0; i < 12; i++) {
        *slots[i] = HexRgb(def->c[i]);
    }
    p.glowStrength = def->glow;
    p.light = def->light;
    return p;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class Check { Process, Window, WindowEveryMonitor, Wallpaper, Service, Command };
enum class ShowOn { SignIn, ExplorerStart, PreviewOnly };
enum class MonitorMode { All, CardOnPrimary, PrimaryOnly };
enum class Style { Card, Minimal, Console };
enum class Position { Center, Top, Bottom, TopLeft, TopRight, BottomLeft, BottomRight };
enum class Background { Gradient, Solid, Image, Wallpaper };
enum class ConsoleRendering { Retro, Crisp, Smooth };
enum class ConsoleTimestamps { Kernel, All, None };

struct ItemConfig {
    std::wstring name;
    std::vector<std::wstring> processes;
    Check check = Check::Process;
    std::wstring windowTitle, windowClass, service, command, commandOutput,
        launch, launchArgs, icon, subtitle;
    bool optional = false;
};

struct Settings {
    ShowOn showOn = ShowOn::SignIn;
    bool previewOnSave = true;
    int maxWaitMs = 30000;
    int minDisplayMs = 2500;
    int stableMs = 2000;
    int readyHoldMs = 600;
    int pollMs = 500;
    bool allowSkip = true;
    bool clickAnywhere = false;
    bool keepOnTop = true;
    bool busyCursor = true;
    MonitorMode monitors = MonitorMode::All;

    Style style = Style::Card;
    Position position = Position::Center;
    Palette palette = PaletteFromTheme(L"midnight");
    Background background = Background::Gradient;
    std::wstring backgroundImage;
    float backgroundDim = 0.55f;
    float backdropOpacity = 1.f;
    float cardOpacity = 1.f;
    float scale = 1.f;
    float cardWidth = 760;
    float cornerRadius = 18;
    std::wstring font = L"Segoe UI";
    int fadeInMs = 250;
    int fadeOutMs = 450;
    bool reduceMotion = false;

    bool showClock = false;
    bool showBrand = true;
    bool showIcons = true;
    bool showDetails = true;
    bool showStatusLabels = true;
    bool showProgressBar = true;
    bool showCountdown = true;

    bool consoleClassic = true;
    ConsoleRendering consoleRendering = ConsoleRendering::Retro;
    std::wstring consoleFont = L"Cascadia Mono";
    float consoleFontSize = 13;
    ConsoleTimestamps consoleTimestamps = ConsoleTimestamps::Kernel;
    bool consoleKernelLog = true;
    bool consoleLogo = true;
    bool consoleVerbose = true;
    bool consoleWelcome = true;
    bool consoleStatusLine = true;
    bool consoleLoginPrompt = true;

    // Not a user setting: the previous sign-in, for "Last login:".
    std::wstring previousLogon;

    std::wstring brand = L"DESKTOP READY";
    std::wstring headline = L"Getting your desktop ready";
    std::wstring readyHeadline = L"Your desktop is ready";
    std::wstring subheading;
    std::wstring timeoutSubheading = L"A few things may still be starting.";
    std::wstring progress = L"{ready} of {total} apps ready";
    std::wstring countdown = L"Up to {seconds} seconds remaining";
    std::wstring opening = L"Opening your desktop";
    std::wstring skipButton = L"Skip for now";
    std::wstring hint;

    std::vector<ItemConfig> items;
};

void ApplyColorOverride(const wchar_t* key, Rgb* slot) {
    Rgb value;
    if (ParseHexColor(SettingString(std::wstring(L"colors.") + key), &value)) {
        *slot = value;
    }
}

Settings LoadSettings() {
    Settings s;

    s.showOn = (ShowOn)PickOption(SettingString(L"behavior.showOn"),
                                  {L"signIn", L"explorerStart", L"previewOnly"});
    s.previewOnSave = SettingInt(L"behavior.previewOnSave") != 0;
    s.maxWaitMs = Clamp(SettingInt(L"behavior.maxWaitSeconds"), 5, 300) * 1000;
    s.minDisplayMs = Clamp(SettingInt(L"behavior.minDisplayMs"), 0, 60000);
    s.stableMs = Clamp(SettingInt(L"behavior.stableMs"), 0, 60000);
    s.readyHoldMs = Clamp(SettingInt(L"behavior.readyHoldMs"), 0, 10000);
    s.pollMs = Clamp(SettingInt(L"behavior.pollIntervalMs"), 100, 5000);
    s.allowSkip = SettingInt(L"behavior.allowSkip") != 0;
    s.clickAnywhere = SettingInt(L"behavior.clickAnywhereToSkip") != 0;
    s.keepOnTop = SettingInt(L"behavior.keepOnTop") != 0;
    s.busyCursor = SettingInt(L"behavior.busyCursor") != 0;
    s.monitors = (MonitorMode)PickOption(SettingString(L"behavior.monitors"),
                                         {L"all", L"cardOnPrimary", L"primary"});

    s.style = (Style)PickOption(SettingString(L"appearance.style"),
                                {L"card", L"minimal", L"console"});
    s.position = (Position)PickOption(
        SettingString(L"appearance.position"),
        {L"center", L"top", L"bottom", L"topLeft", L"topRight", L"bottomLeft",
         L"bottomRight"});
    s.palette = PaletteFromTheme(SettingString(L"appearance.theme"));
    s.background = (Background)PickOption(
        SettingString(L"appearance.background"),
        {L"gradient", L"solid", L"image", L"wallpaper"});
    s.backgroundImage = SettingString(L"appearance.backgroundImage");
    s.backgroundDim =
        Clamp(SettingInt(L"appearance.backgroundDim"), 0, 100) / 100.f;
    s.backdropOpacity =
        Clamp(SettingInt(L"appearance.backdropOpacity"), 0, 100) / 100.f;
    s.cardOpacity = Clamp(SettingInt(L"appearance.cardOpacity"), 0, 100) / 100.f;
    s.scale = Clamp(SettingInt(L"appearance.scale"), 50, 300) / 100.f;
    s.cardWidth = (float)Clamp(SettingInt(L"appearance.cardWidth"), 320, 1800);
    s.cornerRadius = (float)Clamp(SettingInt(L"appearance.cornerRadius"), 0, 60);
    s.font = Trim(SettingString(L"appearance.font"));
    if (s.font.empty()) {
        s.font = L"Segoe UI";
    }
    s.fadeInMs = Clamp(SettingInt(L"appearance.fadeInMs"), 0, 5000);
    s.fadeOutMs = Clamp(SettingInt(L"appearance.fadeOutMs"), 0, 5000);
    s.reduceMotion = SettingInt(L"appearance.reduceMotion") != 0;

    s.showClock = SettingInt(L"elements.showClock") != 0;
    s.showBrand = SettingInt(L"elements.showBrand") != 0;
    s.showIcons = SettingInt(L"elements.showIcons") != 0;
    s.showDetails = SettingInt(L"elements.showDetails") != 0;
    s.showStatusLabels = SettingInt(L"elements.showStatusLabels") != 0;
    s.showProgressBar = SettingInt(L"elements.showProgressBar") != 0;
    s.showCountdown = SettingInt(L"elements.showCountdown") != 0;

    s.consoleClassic = !EqualsI(SettingString(L"console.colors"), L"theme");
    s.consoleFont = Trim(SettingString(L"console.font"));
    if (s.consoleFont.empty()) {
        s.consoleFont = L"Cascadia Mono";
    }
    s.consoleRendering = (ConsoleRendering)PickOption(SettingString(L"console.rendering"),
                                                      {L"retro", L"crisp", L"smooth"});
    s.consoleFontSize = (float)Clamp(SettingInt(L"console.fontSize"), 8, 48);
    s.consoleTimestamps = (ConsoleTimestamps)PickOption(SettingString(L"console.timestamps"),
                                                        {L"kernel", L"all", L"none"});
    s.consoleKernelLog = SettingInt(L"console.kernelLog") != 0;
    s.consoleLogo = SettingInt(L"console.logo") != 0;
    s.consoleVerbose = SettingInt(L"console.verbose") != 0;
    s.consoleWelcome = SettingInt(L"console.welcome") != 0;
    s.consoleStatusLine = SettingInt(L"console.statusLine") != 0;
    s.consoleLoginPrompt = SettingInt(L"console.loginPrompt") != 0;

    s.brand = SettingString(L"text.brand");
    s.headline = SettingString(L"text.headline");
    s.readyHeadline = SettingString(L"text.readyHeadline");
    s.subheading = SettingString(L"text.subheading");
    s.timeoutSubheading = SettingString(L"text.timeoutSubheading");
    s.progress = SettingString(L"text.progress");
    s.countdown = SettingString(L"text.countdown");
    s.opening = SettingString(L"text.opening");
    s.skipButton = SettingString(L"text.skipButton");
    s.hint = SettingString(L"text.hint");
    if (s.skipButton.empty()) {
        s.skipButton = L"Skip";
    }

    std::wstring accent = Trim(SettingString(L"colors.accent"));
    Rgb accentColor;
    if (EqualsI(accent, L"system")) {
        if (SystemAccentColor(&accentColor)) {
            s.palette.accent = accentColor;
        }
    } else if (ParseHexColor(accent, &accentColor)) {
        s.palette.accent = accentColor;
    }
    ApplyColorOverride(L"background", &s.palette.bg1);
    ApplyColorOverride(L"backgroundSecondary", &s.palette.bg2);
    ApplyColorOverride(L"surface", &s.palette.surface);
    ApplyColorOverride(L"border", &s.palette.border);
    ApplyColorOverride(L"text", &s.palette.text);
    ApplyColorOverride(L"subtext", &s.palette.subtext);
    ApplyColorOverride(L"ready", &s.palette.ready);
    ApplyColorOverride(L"starting", &s.palette.busy);
    ApplyColorOverride(L"waiting", &s.palette.waiting);
    ApplyColorOverride(L"unavailable", &s.palette.error);

    for (int i = 0; i < 32 && (int)s.items.size() < kMaxItems; i++) {
        auto field = [i](const wchar_t* name) {
            return SettingString(L"items[" + std::to_wstring(i) + L"]." + name);
        };
        ItemConfig item;
        item.name = Trim(field(L"name"));
        item.processes = SplitProcessList(field(L"process"));
        item.service = Trim(field(L"service"));
        item.command = Trim(field(L"command"));
        if (item.name.empty() && item.processes.empty() &&
            item.service.empty() && item.command.empty()) {
            break;  // End of the list (Windhawk keeps array items contiguous).
        }
        if (item.name.empty()) {
            item.name = !item.processes.empty() ? item.processes[0]
                        : !item.service.empty() ? item.service
                                                : L"App";
        }
        item.check = (Check)PickOption(
            field(L"check"), {L"process", L"window", L"windowEveryMonitor",
                              L"wallpaper", L"service", L"command"});
        item.windowTitle = field(L"windowTitle");
        item.windowClass = Trim(field(L"windowClass"));
        item.commandOutput = field(L"commandOutput");
        item.launch = Trim(field(L"launch"));
        item.launchArgs = field(L"launchArgs");
        item.icon = Trim(field(L"icon"));
        item.subtitle = field(L"subtitle");
        item.optional =
            SettingInt(L"items[" + std::to_wstring(i) + L"].optional") != 0;
        s.items.push_back(std::move(item));
    }
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// Readiness checks (run on a worker thread)

enum ItemState { kWaiting = 0, kStarting = 1, kReady = 2, kUnavailable = 3 };

struct ItemStatus {
    int state = kWaiting;
    std::wstring detail;
    // Technical facts behind the state (PIDs, windows, exit codes) for the
    // verbose log.
    std::wstring trace;
};

struct WinInfo {
    DWORD pid = 0;
    RECT rc{};
    std::wstring cls, title;
};

struct ProbeWorld {
    DWORD session = 0;
    std::unordered_map<std::wstring, std::vector<DWORD>> processes;
    std::vector<RECT> monitors;
    bool haveTopLevel = false;
    std::vector<WinInfo> topLevel;
    bool haveDesktop = false;
    std::vector<WinInfo> desktopChildren;
};

BOOL CALLBACK CollectMonitorProc(HMONITOR, HDC, LPRECT rc, LPARAM param) {
    reinterpret_cast<std::vector<RECT>*>(param)->push_back(*rc);
    return TRUE;
}

bool ReadWindow(HWND hwnd, int minSize, WinInfo* out) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }
    RECT rc;
    if (!GetWindowRect(hwnd, &rc) || rc.right - rc.left < minSize ||
        rc.bottom - rc.top < minSize) {
        return false;
    }
    // Only calls that never send messages are used here, so Explorer's own
    // windows (desktop, taskbar) can be checked safely from this thread.
    WCHAR buffer[256];
    int n = GetClassNameW(hwnd, buffer, ARRAYSIZE(buffer));
    if (n > 0 && wcscmp(buffer, kWindowClass) == 0) {
        return false;  // The splash itself.
    }
    out->pid = pid;
    out->rc = rc;
    out->cls.assign(buffer, n > 0 ? n : 0);
    // InternalGetWindowText never sends a message to the window's thread.
    n = InternalGetWindowText(hwnd, buffer, ARRAYSIZE(buffer));
    out->title.assign(buffer, n > 0 ? n : 0);
    return true;
}

BOOL CALLBACK CollectTopLevelProc(HWND hwnd, LPARAM param) {
    WinInfo info;
    if (ReadWindow(hwnd, 1, &info)) {
        reinterpret_cast<std::vector<WinInfo>*>(param)->push_back(
            std::move(info));
    }
    return TRUE;
}

BOOL CALLBACK CollectDesktopChildProc(HWND hwnd, LPARAM param) {
    WinInfo info;
    if (ReadWindow(hwnd, 64, &info)) {
        reinterpret_cast<std::vector<WinInfo>*>(param)->push_back(
            std::move(info));
    }
    return TRUE;
}

BOOL CALLBACK CollectDesktopHostsProc(HWND hwnd, LPARAM param) {
    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0)) {
        EnumChildWindows(hwnd, CollectDesktopChildProc, param);
    }
    return TRUE;
}

void CollectProcesses(ProbeWorld* world) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return;
    }
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    for (BOOL ok = Process32FirstW(snapshot, &entry); ok;
         ok = Process32NextW(snapshot, &entry)) {
        DWORD session = 0;
        if (!ProcessIdToSessionId(entry.th32ProcessID, &session) ||
            session != world->session) {
            continue;
        }
        world->processes[Lower(entry.szExeFile)].push_back(entry.th32ProcessID);
    }
    CloseHandle(snapshot);
}

// -1: missing/inaccessible, 0: stopped, 1: pending, 2: running.
int QueryServiceState(const std::wstring& name) {
    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!manager) {
        return -1;
    }
    int result = -1;
    SC_HANDLE service = OpenServiceW(manager, name.c_str(), SERVICE_QUERY_STATUS);
    if (service) {
        SERVICE_STATUS status{};
        if (QueryServiceStatus(service, &status)) {
            switch (status.dwCurrentState) {
                case SERVICE_RUNNING:
                    result = 2;
                    break;
                case SERVICE_START_PENDING:
                case SERVICE_CONTINUE_PENDING:
                    result = 1;
                    break;
                default:
                    result = 0;
            }
        }
        CloseServiceHandle(service);
    }
    CloseServiceHandle(manager);
    return result;
}

std::string StripSpaces(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            out += c;
        }
    }
    return out;
}

struct CommandResult {
    int result = -1;  // 1: success, 0: ran but did not succeed, -1: could not start.
    DWORD exitCode = 0;
    size_t bytes = 0;
    bool timedOut = false;
    bool matched = false;
    DWORD error = 0;
};

CommandResult RunCheckCommand(const std::wstring& commandLine,
                              const std::wstring& expected,
                              DWORD timeoutMs,
                              HANDLE stopEvent) {
    CommandResult out;
    std::wstring cmd = ExpandEnv(Trim(commandLine));
    if (cmd.empty()) {
        return out;
    }
    bool capture = !Trim(expected).empty();

    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    HANDLE readPipe = nullptr, writePipe = nullptr;
    if (capture) {
        if (!CreatePipe(&readPipe, &writePipe, &sa, 1 << 16)) {
            out.error = GetLastError();
            return out;
        }
        SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);
    }

    STARTUPINFOEXW si{};
    si.StartupInfo.cb = sizeof(si);
    si.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    si.StartupInfo.wShowWindow = SW_HIDE;
    std::vector<BYTE> attributeBuffer;
    LPPROC_THREAD_ATTRIBUTE_LIST attributes = nullptr;
    if (capture) {
        si.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
        si.StartupInfo.hStdOutput = writePipe;
        si.StartupInfo.hStdError = writePipe;
        // Inherit only the pipe, never any of Explorer's other handles.
        SIZE_T size = 0;
        InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
        attributeBuffer.resize(size);
        attributes = (LPPROC_THREAD_ATTRIBUTE_LIST)attributeBuffer.data();
        if (!InitializeProcThreadAttributeList(attributes, 1, 0, &size) ||
            !UpdateProcThreadAttribute(attributes, 0,
                                       PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                       &writePipe, sizeof(writePipe), nullptr,
                                       nullptr)) {
            out.error = GetLastError();
            CloseHandle(readPipe);
            CloseHandle(writePipe);
            return out;
        }
        si.lpAttributeList = attributes;
    }

    PROCESS_INFORMATION pi{};
    std::vector<wchar_t> mutableCmd(cmd.begin(), cmd.end());
    mutableCmd.push_back(L'\0');
    BOOL created = CreateProcessW(
        nullptr, mutableCmd.data(), nullptr, nullptr, capture ? TRUE : FALSE,
        CREATE_NO_WINDOW | (capture ? EXTENDED_STARTUPINFO_PRESENT : 0),
        nullptr, nullptr, &si.StartupInfo, &pi);
    DWORD createError = created ? 0 : GetLastError();
    if (attributes) {
        DeleteProcThreadAttributeList(attributes);
    }
    if (writePipe) {
        CloseHandle(writePipe);
    }
    if (!created) {
        if (readPipe) {
            CloseHandle(readPipe);
        }
        out.error = createError;
        return out;
    }
    CloseHandle(pi.hThread);

    std::string output;
    auto drain = [&]() {
        if (!readPipe) {
            return;
        }
        for (int guard = 0; guard < 64; guard++) {
            DWORD available = 0;
            if (!PeekNamedPipe(readPipe, nullptr, 0, nullptr, &available,
                               nullptr) ||
                !available) {
                return;
            }
            char buffer[4096];
            DWORD read = 0;
            if (!ReadFile(readPipe, buffer,
                          Smaller<DWORD>(available, sizeof(buffer)), &read,
                          nullptr) ||
                !read) {
                return;
            }
            if (output.size() < (1u << 20)) {
                output.append(buffer, read);
            }
        }
    };

    out.result = 0;
    ULONGLONG deadline = GetTickCount64() + timeoutMs;
    for (;;) {
        drain();
        HANDLE handles[2] = {pi.hProcess, stopEvent};
        DWORD wait = WaitForMultipleObjects(stopEvent ? 2 : 1, handles, FALSE, 25);
        if (wait == WAIT_OBJECT_0) {
            drain();
            DWORD exitCode = 1;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            out.exitCode = exitCode;
            out.bytes = output.size();
            if (exitCode == 0) {
                if (!capture) {
                    out.result = 1;
                } else {
                    // Compare without whitespace so pretty-printed JSON
                    // still matches.
                    std::wstring want = Trim(expected);
                    int bytes = WideCharToMultiByte(CP_UTF8, 0, want.c_str(),
                                                    (int)want.size(), nullptr,
                                                    0, nullptr, nullptr);
                    std::string wantUtf8(bytes, '\0');
                    WideCharToMultiByte(CP_UTF8, 0, want.c_str(),
                                        (int)want.size(), wantUtf8.data(),
                                        bytes, nullptr, nullptr);
                    out.matched = StripSpaces(output).find(StripSpaces(wantUtf8)) !=
                                  std::string::npos;
                    out.result = out.matched ? 1 : 0;
                }
            }
            break;
        }
        if (wait == WAIT_OBJECT_0 + 1 || GetTickCount64() >= deadline) {
            // Only the helper command is terminated, never the app itself.
            TerminateProcess(pi.hProcess, 1);
            out.timedOut = wait != WAIT_OBJECT_0 + 1;
            break;
        }
    }
    CloseHandle(pi.hProcess);
    if (readPipe) {
        CloseHandle(readPipe);
    }
    return out;
}

const wchar_t* ReadyDetail(Check check, size_t monitorCount) {
    switch (check) {
        case Check::Process:
            return L"Running";
        case Check::Window:
            return L"Window is up";
        case Check::WindowEveryMonitor:
            return monitorCount <= 1 ? L"Showing on your display"
                                     : L"Showing on every display";
        case Check::Wallpaper:
            return L"Wallpaper attached to the desktop";
        case Check::Service:
            return L"Service running";
        case Check::Command:
            return L"Responding";
    }
    return L"Ready";
}

ItemStatus Status(int state, std::wstring detail) {
    ItemStatus status;
    status.state = state;
    status.detail = std::move(detail);
    return status;
}

std::wstring JoinList(const std::vector<std::wstring>& parts, const wchar_t* separator) {
    std::wstring out;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i) {
            out += separator;
        }
        out += parts[i];
    }
    return out;
}

std::wstring Shorten(const std::wstring& s, size_t max) {
    return s.size() <= max ? s : s.substr(0, max - 1) + L"…";
}

std::wstring PidTrace(const ItemConfig& item, const std::vector<DWORD>& pids) {
    if (pids.empty()) {
        return L"no " + JoinList(item.processes, L"/") + L" process in this session";
    }
    std::wstring out = pids.size() == 1 ? L"pid " : L"pids ";
    for (size_t i = 0; i < pids.size() && i < 4; i++) {
        out += (i ? L", " : L"") + std::to_wstring(pids[i]);
    }
    if (pids.size() > 4) {
        out += L" and " + std::to_wstring(pids.size() - 4) + L" more";
    }
    return out;
}

std::wstring WindowTrace(size_t count, const WinInfo* first) {
    std::wstring out = std::to_wstring(count) + (count == 1 ? L" matching window" : L" matching windows");
    if (first) {
        out += L" (class " + Shorten(first->cls, 40) + L", \"" + Shorten(first->title, 48) + L"\")";
    }
    return out;
}

ItemStatus ProbeItem(const ItemConfig& item, ProbeWorld& world, HANDLE stopEvent) {
    std::vector<DWORD> pids;
    for (const auto& name : item.processes) {
        auto it = world.processes.find(name);
        if (it != world.processes.end()) {
            pids.insert(pids.end(), it->second.begin(), it->second.end());
        }
    }
    bool hasProcess = !item.processes.empty();
    bool running = !pids.empty();
    auto ownsWindow = [&pids](const WinInfo& w) {
        for (DWORD pid : pids) {
            if (pid == w.pid) {
                return true;
            }
        }
        return false;
    };
    std::wstring processTrace = hasProcess ? PidTrace(item, pids) : L"";
    auto with = [&processTrace](ItemStatus status, const std::wstring& extra) {
        status.trace = processTrace;
        if (!extra.empty()) {
            status.trace += (status.trace.empty() ? L"" : L"; ") + extra;
        }
        return status;
    };
    const wchar_t* notRunning = L"Not running yet";

    switch (item.check) {
        case Check::Process:
            if (!hasProcess) {
                return Status(kUnavailable, L"No process name set");
            }
            return with(running ? Status(kReady, ReadyDetail(item.check, 0))
                                : Status(kWaiting, notRunning),
                        L"");

        case Check::Service: {
            if (item.service.empty()) {
                return Status(kUnavailable, L"No service name set");
            }
            int state = QueryServiceState(item.service);
            static const wchar_t* kNames[] = {L"stopped", L"starting", L"running"};
            std::wstring serviceTrace = L"service " + item.service + L" " +
                                        (state < 0 ? L"not found" : kNames[state]);
            if (state < 0) {
                return with(Status(kUnavailable, L"Service not found"), serviceTrace);
            }
            if (state == 2) {
                return with(!hasProcess || running
                                ? Status(kReady, ReadyDetail(item.check, 0))
                                : Status(kStarting, L"Service running; waiting for its app"),
                            serviceTrace);
            }
            if (state == 1) {
                return with(Status(kStarting, L"Service starting"), serviceTrace);
            }
            return with(Status(kWaiting, L"Waiting for its service"), serviceTrace);
        }

        case Check::Window:
        case Check::WindowEveryMonitor: {
            if (!hasProcess) {
                return Status(kUnavailable, L"No process name set");
            }
            if (!running) {
                return with(Status(kWaiting, notRunning), L"");
            }
            if (!world.haveTopLevel) {
                EnumWindows(CollectTopLevelProc, (LPARAM)&world.topLevel);
                world.haveTopLevel = true;
            }
            std::vector<const WinInfo*> matches;
            for (const auto& w : world.topLevel) {
                if (ownsWindow(w) && ContainsI(w.title, item.windowTitle) &&
                    (item.windowClass.empty() || EqualsI(w.cls, item.windowClass))) {
                    matches.push_back(&w);
                }
            }
            std::wstring windows =
                WindowTrace(matches.size(), matches.empty() ? nullptr : matches[0]);
            if (item.check == Check::Window) {
                return with(matches.empty()
                                ? Status(kStarting, L"Running; waiting for its window")
                                : Status(kReady, ReadyDetail(item.check, 0)),
                            windows);
            }
            size_t covered = 0;
            for (const RECT& monitor : world.monitors) {
                for (const WinInfo* w : matches) {
                    RECT overlap;
                    if (IntersectRect(&overlap, &monitor, &w->rc)) {
                        covered++;
                        break;
                    }
                }
            }
            size_t total = world.monitors.size();
            windows += L" covering " + std::to_wstring(covered) + L" of " +
                       std::to_wstring(total) + L" displays";
            if (total && covered == total) {
                return with(Status(kReady, ReadyDetail(item.check, total)), windows);
            }
            return with(Status(kStarting, L"Showing on " + std::to_wstring(covered) +
                                              L" of " + std::to_wstring(total) +
                                              L" displays"),
                        windows);
        }

        case Check::Wallpaper: {
            if (!hasProcess) {
                return Status(kUnavailable, L"No process name set");
            }
            if (!running) {
                return with(Status(kWaiting, notRunning), L"");
            }
            if (!world.haveDesktop) {
                EnumWindows(CollectDesktopHostsProc, (LPARAM)&world.desktopChildren);
                world.haveDesktop = true;
            }
            size_t surfaces = 0;
            const WinInfo* first = nullptr;
            for (const auto& w : world.desktopChildren) {
                if (ownsWindow(w)) {
                    surfaces++;
                    first = first ? first : &w;
                }
            }
            if (surfaces) {
                return with(Status(kReady, ReadyDetail(item.check, 0)),
                            std::to_wstring(surfaces) +
                                (surfaces == 1 ? L" wallpaper surface" : L" wallpaper surfaces") +
                                L" on the desktop (" +
                                std::to_wstring(first->rc.right - first->rc.left) + L"x" +
                                std::to_wstring(first->rc.bottom - first->rc.top) + L")");
            }
            return with(Status(kStarting, L"Running; waiting for the wallpaper surface"),
                        L"no surface under Progman/WorkerW yet");
        }

        case Check::Command: {
            if (hasProcess && !running) {
                return with(Status(kWaiting, notRunning), L"");
            }
            if (item.command.empty()) {
                return hasProcess ? with(Status(kReady, L"Running"), L"no command set")
                                  : Status(kUnavailable, L"No command set");
            }
            CommandResult r =
                RunCheckCommand(item.command, item.commandOutput, 1500, stopEvent);
            std::wstring commandTrace;
            if (r.result < 0) {
                commandTrace = L"command could not start (error " + std::to_wstring(r.error) + L")";
            } else if (r.timedOut) {
                commandTrace = L"command timed out after 1500 ms";
            } else {
                commandTrace = L"command exit code " + std::to_wstring(r.exitCode);
                if (!Trim(item.commandOutput).empty()) {
                    commandTrace += L", " + std::to_wstring(r.bytes) + L" bytes, expected output " +
                                    (r.matched ? L"found" : L"not found");
                }
            }
            if (r.result == 1) {
                return with(Status(kReady, ReadyDetail(item.check, 0)), commandTrace);
            }
            if (r.result < 0) {
                return hasProcess
                           ? with(Status(kReady, L"Running (status command unavailable)"),
                                  commandTrace)
                           : with(Status(kUnavailable, L"Command could not start"), commandTrace);
            }
            return with(Status(kStarting, hasProcess ? L"Running; waiting for a response"
                                                     : L"Waiting for a successful response"),
                        commandTrace);
        }
    }
    return Status(kUnavailable, L"Unknown check");
}

std::vector<ItemStatus> ProbeAll(const std::vector<ItemConfig>& items,
                                 HANDLE stopEvent) {
    ProbeWorld world;
    ProcessIdToSessionId(GetCurrentProcessId(), &world.session);
    CollectProcesses(&world);
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorProc,
                        (LPARAM)&world.monitors);
    std::vector<ItemStatus> result;
    for (const auto& item : items) {
        if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
            result.push_back(Status(kWaiting, L""));
            continue;
        }
        result.push_back(ProbeItem(item, world, stopEvent));
    }
    return result;
}

bool AnyProcessRunning(const ItemConfig& item) {
    ProbeWorld world;
    ProcessIdToSessionId(GetCurrentProcessId(), &world.session);
    CollectProcesses(&world);
    for (const auto& name : item.processes) {
        if (world.processes.count(name)) {
            return true;
        }
    }
    return false;
}

enum LaunchKind { kLaunchStarted, kLaunchAlreadyRunning, kLaunchNotFound, kLaunchFailed };

struct LaunchNote {
    int kind = kLaunchStarted;
    std::wstring name;
    std::wstring detail;
};

LaunchNote LaunchProgram(const ItemConfig& item) {
    LaunchNote note;
    note.name = item.name;
    std::wstring path = CleanPath(item.launch);
    note.detail = path;
    if (!FileExists(path)) {
        note.kind = kLaunchNotFound;
        return note;
    }
    if (!item.processes.empty() && AnyProcessRunning(item)) {
        note.kind = kLaunchAlreadyRunning;
        return note;
    }
    std::wstring directory = path.substr(0, path.find_last_of(L"\\/"));
    std::wstring args = ExpandEnv(item.launchArgs);
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    // ShellExecute (not CreateProcess) so UIAccess apps such as GlazeWM start.
    info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    info.lpVerb = L"open";
    info.lpFile = path.c_str();
    info.lpParameters = args.empty() ? nullptr : args.c_str();
    info.lpDirectory = directory.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        note.kind = kLaunchFailed;
        note.detail += L" (error " + std::to_wstring(GetLastError()) + L")";
    } else if (!args.empty()) {
        note.detail += L" " + args;
    }
    return note;
}

std::vector<ItemStatus> SimulateStatus(const std::vector<ItemConfig>& items,
                                       double ms) {
    std::vector<ItemStatus> result;
    for (size_t i = 0; i < items.size(); i++) {
        double startAt = 450 + i * 750.0;
        double readyAt = startAt + 900 + (i % 3) * 380.0;
        std::wstring pid = L"pid " + std::to_wstring(4100 + i * 236) + L" (simulated)";
        ItemStatus status;
        if (ms < startAt) {
            status = Status(kWaiting, L"Waiting to start");
            status.trace = L"no process yet (simulated)";
        } else if (ms < readyAt) {
            status = Status(kStarting, L"Starting…");
            status.trace = pid;
        } else {
            status = Status(kReady, ReadyDetail(items[i].check, 2));
            status.trace = pid + L"; check passed";
        }
        result.push_back(std::move(status));
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Rendering (GDI+, into premultiplied 32-bit buffers for layered windows)

struct ItemVisual {
    std::unique_ptr<gp::Bitmap> icon;
    std::wstring glyph;
    bool iconFont = false;
};

struct Resources {
    std::unique_ptr<gp::FontFamily> family;
    std::unique_ptr<gp::FontFamily> lightFamily;
    std::unique_ptr<gp::FontFamily> iconFamily;
    std::unique_ptr<gp::FontFamily> monoFamily;
    std::unique_ptr<gp::Bitmap> measureBitmap;
    std::unique_ptr<gp::Graphics> measure;
    std::unique_ptr<gp::Bitmap> backgroundImage;
    std::vector<ItemVisual> visuals;
    std::wstring userName;     // First name, for {user}.
    std::wstring accountName;  // Sign-in name, for the console.
    std::wstring hostName;
    int cpuCount = 1;
};

enum class Outcome { None, Ready, Timeout, Skipped, Stopped };

// Boot log (shown by the Console style, saved to latest-run.log always).
enum LogKind {
    kLogKernel,  // dmesg-style message, no status column
    kLogInfo,    // systemd status text, indented under the status column
    kLogDetail,  // like kLogInfo but dimmed (verbose details)
    kLogOk,
    kLogWarn,
    kLogTime,
    kLogFail,
    kLogDepend,
    kLogSkip,
    kLogWelcome,
    kLogPlain,  // raw text such as the login prompt
    kLogBlank,
};

struct LogLine {
    double seconds = 0;
    int kind = kLogInfo;
    std::wstring text;
    std::wstring highlight;  // Part of the text shown brighter (an app name).
};

const wchar_t* LogTag(int kind) {
    switch (kind) {
        case kLogOk:
            return L"  OK  ";
        case kLogWarn:
            return L" WARN ";
        case kLogTime:
            return L" TIME ";
        case kLogFail:
            return L"FAILED";
        case kLogDepend:
            return L"DEPEND";
        case kLogSkip:
            return L" SKIP ";
    }
    return nullptr;
}

// dmesg's "[    1.234567] " timestamp.
std::wstring LogTimestamp(double seconds) {
    unsigned long long micros = (unsigned long long)std::llround(Larger(0.0, seconds) * 1e6);
    wchar_t buffer[48];
    swprintf(buffer, 48, L"[%5llu.%06llu] ", micros / 1000000, micros % 1000000);
    return buffer;
}

// Plain-text form for latest-run.log and the Windhawk log.
std::wstring FormatLogLine(const LogLine& line) {
    if (line.kind == kLogBlank) {
        return L"";
    }
    std::wstring out = LogTimestamp(line.seconds);
    switch (line.kind) {
        case kLogKernel:
        case kLogPlain:
            return out + line.text;
        case kLogWelcome:
            return out + L"Welcome to " + line.text + L"!";
    }
    const wchar_t* tag = LogTag(line.kind);
    return out + (tag ? std::wstring(L"[") + tag + L"] " : std::wstring(9, L' ')) + line.text;
}

struct ConsolePalette {
    Rgb bg, text, bright, dim, ok, warn, fail, cylonDim, cylon, welcome;
};

ConsolePalette MakeConsolePalette(const Settings& st) {
    if (st.consoleClassic) {
        // The VGA text-mode colors a Linux console uses.
        return {HexRgb(0x000000), HexRgb(0xAAAAAA), HexRgb(0xFFFFFF), HexRgb(0x6E6E6E),
                HexRgb(0x55FF55), HexRgb(0xFFFF55), HexRgb(0xFF5555), HexRgb(0xAA0000),
                HexRgb(0xFF5555), HexRgb(0x5555FF)};
    }
    const Palette& p = st.palette;
    return {p.bg2,    Mix(p.text, p.subtext, 0.3f), p.text, Mix(p.subtext, p.bg1, 0.2f),
            p.ready,  p.error,  p.error, Mix(p.busy, p.bg1, 0.5f), p.busy, p.accent};
}

bool ShowsTimestamp(const Settings& st, int kind) {
    switch (st.consoleTimestamps) {
        case ConsoleTimestamps::All:
            return kind != kLogBlank && kind != kLogPlain && kind != kLogWelcome;
        case ConsoleTimestamps::Kernel:
            return kind == kLogKernel;
        default:
            return false;
    }
}

// How many console rows a line takes once it wraps at the last column.
int LogLineRows(const Settings& st, const LogLine& line, int columns) {
    size_t length = ShowsTimestamp(st, line.kind) ? 15 : 0;
    switch (line.kind) {
        case kLogBlank:
            return 1;
        case kLogKernel:
        case kLogPlain:
            length += line.text.size();
            break;
        case kLogWelcome:
            length += 12 + line.text.size();
            break;
        default:
            length += 9 + line.text.size();
    }
    return Larger(1, (int)((length + columns - 1) / Larger(1, columns)));
}

struct Frame {
    const std::vector<ItemStatus>* status = nullptr;
    std::vector<float> readyAnim;
    float spin = 0;
    float progress = 0;
    int secondsLeft = -1;
    int readyCount = 0;
    Outcome outcome = Outcome::None;
    float slide = 0;
    bool hoverSkip = false;
    bool preview = false;
    // Console style.
    const std::vector<LogLine>* log = nullptr;
    size_t logCount = 0;   // Lines revealed so far (they scroll in quickly).
    std::wstring jobText;  // Empty hides the start-job line.
    std::wstring jobName;
    int cylon = 0;
    bool cursorOn = false;
};

struct Layout {
    float s = 1;
    bool narrow = false;
    bool valid = false;
    gp::RectF content;
    // With a see-through backdrop, everything sits on one panel so the text
    // stays readable over any wallpaper.
    gp::RectF panel;
    gp::RectF clock, date, brand, headline, sub, card, progressBar, footerText,
        footerSub, skip, hint, chipsArea, status;
    std::vector<gp::RectF> rows;
    float rowH = 0;
    // Status indicators are the only thing that animates continuously, so
    // they are drawn separately on top of a cached static scene.
    std::vector<gp::PointF> indicators;
    float indicatorRadius = 0;
    bool statusLabels = false;
    // Console style, in framebuffer pixels (screen pixels / pixel).
    bool console = false;
    int pixel = 1;
    float unit = 1;
    int fbW = 0, fbH = 0;
    float fontPx = 0, lineH = 0, charW = 0, logTop = 0, marginX = 0;
    int maxLines = 0, columns = 0;
    float logoSize = 0;
    int logoRows = 0, logoCount = 0;
    int scroll = 0;
    float cursorY = 0;
    gp::RectF jobLine;  // The cursor row, in screen pixels.
};

gp::RectF IndicatorBounds(const Layout& L) {
    if (L.console) {
        return L.jobLine;  // The animated start-job line.
    }
    gp::RectF bounds;
    bool any = false;
    float r = L.indicatorRadius * 3.f + 2.f * L.s;
    for (const auto& c : L.indicators) {
        gp::RectF one(c.X - r, c.Y - r, r * 2, r * 2);
        if (!any) {
            bounds = one;
            any = true;
        } else {
            gp::RectF current = bounds;
            gp::RectF::Union(bounds, current, one);
        }
    }
    return bounds;
}

std::unique_ptr<gp::Bitmap> HiconToBitmap(HICON icon) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return nullptr;
    }
    std::unique_ptr<gp::Bitmap> result;
    BITMAP bm{};
    if (info.hbmColor && GetObjectW(info.hbmColor, sizeof(bm), &bm) &&
        bm.bmWidth > 0 && bm.bmHeight > 0) {
        int w = bm.bmWidth, h = bm.bmHeight;
        std::vector<uint32_t> pixels(w * h), mask(w * h);
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        bool ok = GetDIBits(dc, info.hbmColor, 0, h, pixels.data(), &bi,
                            DIB_RGB_COLORS) == h;
        bool hasAlpha = false;
        for (uint32_t px : pixels) {
            if (px & 0xFF000000) {
                hasAlpha = true;
                break;
            }
        }
        if (ok && !hasAlpha && info.hbmMask &&
            GetDIBits(dc, info.hbmMask, 0, h, mask.data(), &bi,
                      DIB_RGB_COLORS) == h) {
            for (int i = 0; i < w * h; i++) {
                pixels[i] = (mask[i] & 0xFFFFFF) ? 0 : (pixels[i] | 0xFF000000);
            }
        }
        ReleaseDC(nullptr, dc);
        if (ok) {
            result = std::make_unique<gp::Bitmap>(w, h, PixelFormat32bppARGB);
            gp::BitmapData data{};
            gp::Rect rect(0, 0, w, h);
            if (result->LockBits(&rect, gp::ImageLockModeWrite,
                                 PixelFormat32bppARGB, &data) == gp::Ok) {
                for (int y = 0; y < h; y++) {
                    memcpy((BYTE*)data.Scan0 + y * data.Stride, &pixels[y * w],
                           w * 4);
                }
                result->UnlockBits(&data);
            } else {
                result.reset();
            }
        }
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    return result;
}

std::unique_ptr<gp::Bitmap> LoadImageFile(const std::wstring& path) {
    if (!FileExists(path)) {
        return nullptr;
    }
    std::unique_ptr<gp::Bitmap> loaded(gp::Bitmap::FromFile(path.c_str(), FALSE));
    if (!loaded || loaded->GetLastStatus() != gp::Ok || !loaded->GetWidth() ||
        !loaded->GetHeight()) {
        return nullptr;
    }
    // Decode once into memory so the file is not kept locked.
    auto copy = std::make_unique<gp::Bitmap>(
        (INT)loaded->GetWidth(), (INT)loaded->GetHeight(), PixelFormat32bppPARGB);
    if (copy->GetLastStatus() != gp::Ok) {
        return nullptr;
    }
    {
        gp::Graphics g(copy.get());
        g.DrawImage(loaded.get(), 0, 0, (INT)loaded->GetWidth(),
                    (INT)loaded->GetHeight());
    }
    return copy;
}

std::unique_ptr<gp::Bitmap> LoadIconFile(const std::wstring& raw) {
    std::wstring path = CleanPath(raw);
    if (!FileExists(path)) {
        return nullptr;
    }
    std::wstring ext = Lower(path.substr(path.find_last_of(L'.') + 1));
    if (ext == L"png" || ext == L"jpg" || ext == L"jpeg" || ext == L"bmp" ||
        ext == L"gif" || ext == L"tif" || ext == L"tiff") {
        return LoadImageFile(path);
    }
    HICON icon = nullptr;
    UINT id = 0;
    UINT count = PrivateExtractIconsW(path.c_str(), 0, 64, 64, &icon, &id, 1,
                                      LR_DEFAULTCOLOR);
    if (count == 0 || count == 0xFFFFFFFF || !icon) {
        return nullptr;
    }
    auto bitmap = HiconToBitmap(icon);
    DestroyIcon(icon);
    return bitmap;
}

ItemVisual LoadItemVisual(const ItemConfig& item) {
    ItemVisual visual;
    std::wstring icon = item.icon;
    if (icon.size() > 6 && EqualsI(icon.substr(0, 6), L"glyph:")) {
        wchar_t code = (wchar_t)wcstoul(icon.c_str() + 6, nullptr, 16);
        if (code) {
            visual.glyph.assign(1, code);
            visual.iconFont = true;
            return visual;
        }
    }
    bool looksLikePath = icon.find_first_of(L"\\/.%") != std::wstring::npos;
    if (!icon.empty() && !looksLikePath && icon.size() <= 3) {
        visual.glyph = icon;
        return visual;
    }
    if (!icon.empty()) {
        visual.icon = LoadIconFile(icon);
    }
    if (!visual.icon && !item.launch.empty()) {
        visual.icon = LoadIconFile(item.launch);
    }
    if (!visual.icon) {
        visual.glyph.assign(1, item.name.empty() ? L'?' : (wchar_t)towupper(item.name[0]));
    }
    return visual;
}

std::wstring LookupUserName() {
    WCHAR account[UNLEN + 1];
    DWORD size = ARRAYSIZE(account);
    if (!GetUserNameW(account, &size)) {
        return L"";
    }
    std::wstring name = account;
    USER_INFO_10* info = nullptr;
    if (NetUserGetInfo(nullptr, account, 10, (LPBYTE*)&info) == NERR_Success &&
        info) {
        std::wstring full = info->usri10_full_name ? Trim(info->usri10_full_name)
                                                   : L"";
        NetApiBufferFree(info);
        if (!full.empty()) {
            name = full.substr(0, full.find(L' '));
        }
    }
    return name;
}

void InitResources(Resources* res, const Settings& s) {
    res->family = std::make_unique<gp::FontFamily>(s.font.c_str());
    if (!res->family->IsAvailable()) {
        res->family = std::make_unique<gp::FontFamily>(L"Segoe UI");
    }
    res->lightFamily = std::make_unique<gp::FontFamily>(L"Segoe UI Light");
    if (!res->lightFamily->IsAvailable() || !EqualsI(s.font, L"Segoe UI")) {
        res->lightFamily = std::make_unique<gp::FontFamily>(s.font.c_str());
        if (!res->lightFamily->IsAvailable()) {
            res->lightFamily = std::make_unique<gp::FontFamily>(L"Segoe UI");
        }
    }
    res->iconFamily = std::make_unique<gp::FontFamily>(L"Segoe Fluent Icons");
    if (!res->iconFamily->IsAvailable()) {
        res->iconFamily = std::make_unique<gp::FontFamily>(L"Segoe MDL2 Assets");
    }
    for (const wchar_t* mono : {s.consoleFont.c_str(), L"Cascadia Mono", L"Consolas", L"Courier New"}) {
        res->monoFamily = std::make_unique<gp::FontFamily>(mono);
        if (res->monoFamily->IsAvailable()) {
            break;
        }
    }
    res->measureBitmap = std::make_unique<gp::Bitmap>(8, 8, PixelFormat32bppPARGB);
    res->measure = std::make_unique<gp::Graphics>(res->measureBitmap.get());
    res->measure->SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);

    res->visuals.clear();
    for (const auto& item : s.items) {
        res->visuals.push_back(LoadItemVisual(item));
    }

    std::wstring imagePath;
    if (s.background == Background::Image) {
        imagePath = CleanPath(s.backgroundImage);
    } else if (s.background == Background::Wallpaper) {
        WCHAR wallpaper[MAX_PATH] = {};
        if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaper, 0)) {
            imagePath = wallpaper;
        }
    }
    if (!imagePath.empty()) {
        res->backgroundImage = LoadImageFile(imagePath);
    }
    res->userName = LookupUserName();
    WCHAR buffer[256] = {};
    DWORD size = ARRAYSIZE(buffer);
    res->accountName = GetUserNameW(buffer, &size) ? buffer : L"user";
    size = ARRAYSIZE(buffer);
    res->hostName = GetComputerNameExW(ComputerNameDnsHostname, buffer, &size)
                        ? Lower(buffer)
                        : L"localhost";
    res->cpuCount = (int)Larger<DWORD>(1, GetActiveProcessorCount(ALL_PROCESSOR_GROUPS));
}

std::wstring Greeting() {
    SYSTEMTIME now;
    GetLocalTime(&now);
    if (now.wHour >= 5 && now.wHour < 12) {
        return L"Good morning";
    }
    if (now.wHour >= 12 && now.wHour < 18) {
        return L"Good afternoon";
    }
    if (now.wHour >= 18) {
        return L"Good evening";
    }
    return L"Welcome back";
}

std::wstring TimeText() {
    WCHAR buffer[64] = {};
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, nullptr, nullptr,
                    buffer, ARRAYSIZE(buffer));
    return buffer;
}

std::wstring DateText() {
    WCHAR buffer[128] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, nullptr, nullptr,
                    buffer, ARRAYSIZE(buffer), nullptr);
    return buffer;
}

std::wstring FillTokens(const std::wstring& tpl,
                        const Resources& res,
                        const Frame& f,
                        size_t total) {
    if (tpl.find(L'{') == std::wstring::npos) {
        return tpl;
    }
    std::wstring r = tpl;
    r = ReplaceAll(r, L"{greeting}", Greeting());
    r = ReplaceAll(r, L"{user}", res.userName);
    r = ReplaceAll(r, L"{ready}", std::to_wstring(f.readyCount));
    r = ReplaceAll(r, L"{total}", std::to_wstring(total));
    r = ReplaceAll(r, L"{seconds}", std::to_wstring(Larger(0, f.secondsLeft)));
    if (r.find(L"{time}") != std::wstring::npos) {
        r = ReplaceAll(r, L"{time}", TimeText());
    }
    if (r.find(L"{date}") != std::wstring::npos) {
        r = ReplaceAll(r, L"{date}", DateText());
    }
    return r;
}

std::wstring HeadlineText(const Settings& s, const Frame& f) {
    if (f.outcome == Outcome::Ready && !s.readyHeadline.empty()) {
        return s.readyHeadline;
    }
    return s.headline;
}

std::wstring SubheadingText(const Settings& s, const Frame& f) {
    if (f.outcome == Outcome::Timeout && !s.timeoutSubheading.empty()) {
        return s.timeoutSubheading;
    }
    return s.subheading;
}

// Drawing helpers --------------------------------------------------------------

void RoundedPath(gp::GraphicsPath* path, const gp::RectF& r, float radius) {
    float d = Clamp(radius * 2.f, 0.1f, Smaller(r.Width, r.Height));
    if (radius <= 0.25f) {
        path->AddRectangle(r);
        return;
    }
    path->AddArc(r.X, r.Y, d, d, 180.f, 90.f);
    path->AddArc(r.X + r.Width - d, r.Y, d, d, 270.f, 90.f);
    path->AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0.f, 90.f);
    path->AddArc(r.X, r.Y + r.Height - d, d, d, 90.f, 90.f);
    path->CloseFigure();
}

void FillRounded(gp::Graphics& g, const gp::RectF& r, float radius, gp::Color color) {
    if (r.Width <= 0 || r.Height <= 0 || color.GetA() == 0) {
        return;
    }
    gp::GraphicsPath path;
    RoundedPath(&path, r, radius);
    gp::SolidBrush brush(color);
    g.FillPath(&brush, &path);
}

void StrokeRounded(gp::Graphics& g,
                   const gp::RectF& r,
                   float radius,
                   gp::Color color,
                   float width) {
    if (r.Width <= 0 || r.Height <= 0 || color.GetA() == 0) {
        return;
    }
    gp::GraphicsPath path;
    RoundedPath(&path, r, radius);
    gp::Pen pen(color, Larger(0.5f, width));
    g.DrawPath(&pen, &path);
}

void DrawTextBox(gp::Graphics& g,
                 const gp::FontFamily* family,
                 const std::wstring& text,
                 float size,
                 int style,
                 gp::Color color,
                 const gp::RectF& r,
                 gp::StringAlignment align,
                 bool wrap,
                 gp::StringAlignment vAlign = gp::StringAlignmentCenter) {
    if (text.empty() || r.Width <= 0 || r.Height <= 0 || color.GetA() == 0) {
        return;
    }
    gp::Font font(family, Larger(1.f, size), style, gp::UnitPixel);
    gp::SolidBrush brush(color);
    gp::StringFormat format;
    format.SetAlignment(align);
    format.SetLineAlignment(vAlign);
    format.SetTrimming(gp::StringTrimmingEllipsisCharacter);
    if (!wrap) {
        format.SetFormatFlags(gp::StringFormatFlagsNoWrap);
    }
    g.DrawString(text.c_str(), (INT)text.size(), &font, r, &format, &brush);
}

float MeasureHeight(gp::Graphics& g,
                    const gp::FontFamily* family,
                    const std::wstring& text,
                    float size,
                    int style,
                    float width,
                    int maxLines) {
    if (text.empty()) {
        return 0;
    }
    gp::Font font(family, Larger(1.f, size), style, gp::UnitPixel);
    float lineHeight = font.GetHeight(&g);
    gp::StringFormat format;
    gp::RectF layout(0, 0, width, lineHeight * maxLines + 1);
    gp::RectF bounds;
    g.MeasureString(text.c_str(), (INT)text.size(), &font, layout, &format, &bounds);
    return Clamp(bounds.Height, lineHeight, lineHeight * maxLines) + 2;
}

float MeasureWidth(gp::Graphics& g,
                   const gp::FontFamily* family,
                   const std::wstring& text,
                   float size,
                   int style) {
    if (text.empty()) {
        return 0;
    }
    gp::Font font(family, Larger(1.f, size), style, gp::UnitPixel);
    gp::StringFormat format(gp::StringFormat::GenericTypographic());
    format.SetFormatFlags(format.GetFormatFlags() |
                          gp::StringFormatFlagsMeasureTrailingSpaces);
    gp::RectF bounds;
    g.MeasureString(text.c_str(), (INT)text.size(), &font, gp::PointF(0, 0),
                    &format, &bounds);
    return bounds.Width;
}

gp::RectF Offset(gp::RectF r, float dx, float dy) {
    r.X += dx;
    r.Y += dy;
    return r;
}

Rgb StateColor(const Palette& p, int state) {
    switch (state) {
        case kReady:
            return p.ready;
        case kStarting:
            return p.busy;
        case kUnavailable:
            return p.error;
        default:
            return p.waiting;
    }
}

void DrawIndicator(gp::Graphics& g,
                   const Palette& p,
                   gp::PointF c,
                   float radius,
                   int state,
                   float spin,
                   float readyAnim,
                   bool reduceMotion,
                   float alpha) {
    Rgb color = StateColor(p, state);
    float s = radius / 7.5f;
    gp::RectF circle(c.X - radius, c.Y - radius, radius * 2, radius * 2);
    gp::Pen pen(ToColor(color, alpha), 1.8f * s);
    pen.SetStartCap(gp::LineCapRound);
    pen.SetEndCap(gp::LineCapRound);
    pen.SetLineJoin(gp::LineJoinRound);
    if (state == kReady) {
        float a = reduceMotion ? 1.f : EaseOut(readyAnim);
        if (!reduceMotion && readyAnim < 1.f) {
            float ring = radius * (1.4f + 1.1f * a);
            gp::Pen ringPen(ToColor(color, alpha * 0.45f * (1.f - a)), 1.4f * s);
            g.DrawEllipse(&ringPen, c.X - ring, c.Y - ring, ring * 2, ring * 2);
        }
        gp::SolidBrush halo(ToColor(color, alpha * 0.15f));
        float hr = radius * 1.4f;
        g.FillEllipse(&halo, c.X - hr, c.Y - hr, hr * 2, hr * 2);
        // Check mark drawn progressively.
        gp::PointF pts[3] = {gp::PointF(c.X - 4.2f * s, c.Y + 0.2f * s),
                             gp::PointF(c.X - 1.1f * s, c.Y + 3.2f * s),
                             gp::PointF(c.X + 4.8f * s, c.Y - 3.6f * s)};
        float l1 = std::hypot(pts[1].X - pts[0].X, pts[1].Y - pts[0].Y);
        float l2 = std::hypot(pts[2].X - pts[1].X, pts[2].Y - pts[1].Y);
        float len = (l1 + l2) * Clamp(a * 1.15f, 0.f, 1.f);
        if (len <= l1) {
            float t = l1 > 0 ? len / l1 : 0;
            g.DrawLine(&pen, pts[0],
                       gp::PointF(pts[0].X + (pts[1].X - pts[0].X) * t,
                                  pts[0].Y + (pts[1].Y - pts[0].Y) * t));
        } else {
            float t = l2 > 0 ? (len - l1) / l2 : 0;
            gp::PointF partial[3] = {pts[0], pts[1],
                                     gp::PointF(pts[1].X + (pts[2].X - pts[1].X) * t,
                                                pts[1].Y + (pts[2].Y - pts[1].Y) * t)};
            g.DrawLines(&pen, partial, 3);
        }
    } else if (state == kStarting) {
        gp::Pen track(ToColor(color, alpha * 0.22f), 1.8f * s);
        g.DrawEllipse(&track, circle);
        float start = reduceMotion ? -90.f : spin * 360.f - 90.f;
        g.DrawArc(&pen, circle, start, 110.f);
    } else if (state == kUnavailable) {
        g.DrawEllipse(&pen, circle);
        g.DrawLine(&pen, c.X - 3.2f * s, c.Y, c.X + 3.2f * s, c.Y);
    } else {
        gp::Pen dim(ToColor(color, alpha * 0.9f), 1.6f * s);
        g.DrawEllipse(&dim, circle);
    }
}

void DrawIconTile(gp::Graphics& g,
                  const Settings& st,
                  const Resources& res,
                  size_t index,
                  const gp::RectF& tile,
                  float s,
                  float alpha) {
    const Palette& p = st.palette;
    float radius = Smaller(st.cornerRadius * 0.6f, 11.f) * s;
    FillRounded(g, tile, radius, ToColor(Mix(p.surface, p.text, 0.05f), alpha));
    StrokeRounded(g, tile, radius, ToColor(Mix(p.surface, p.border, 0.8f), alpha), s);
    const ItemVisual* visual =
        index < res.visuals.size() ? &res.visuals[index] : nullptr;
    if (visual && visual->icon) {
        float size = tile.Width * 0.62f;
        gp::RectF dest(tile.X + (tile.Width - size) / 2,
                       tile.Y + (tile.Height - size) / 2, size, size);
        gp::ImageAttributes attributes;
        if (alpha < 0.999f) {
            gp::ColorMatrix matrix = {{{1, 0, 0, 0, 0},
                                       {0, 1, 0, 0, 0},
                                       {0, 0, 1, 0, 0},
                                       {0, 0, 0, alpha, 0},
                                       {0, 0, 0, 0, 1}}};
            attributes.SetColorMatrix(&matrix);
        }
        g.DrawImage(visual->icon.get(), dest, 0, 0,
                    (gp::REAL)visual->icon->GetWidth(),
                    (gp::REAL)visual->icon->GetHeight(), gp::UnitPixel,
                    &attributes);
    } else if (visual) {
        const gp::FontFamily* family =
            visual->iconFont ? res.iconFamily.get() : res.family.get();
        float size = visual->iconFont ? 18.f * s
                     : visual->glyph.size() > 1 ? 13.f * s
                                                : 16.f * s;
        DrawTextBox(g, family, visual->glyph, size,
                    visual->iconFont ? gp::FontStyleRegular : gp::FontStyleBold,
                    ToColor(Mix(p.subtext, p.accent, 0.45f), alpha), tile,
                    gp::StringAlignmentCenter, false);
    }
}

void DrawBrand(gp::Graphics& g,
               const Settings& st,
               const Resources& res,
               const gp::RectF& r,
               float s,
               bool centered) {
    const Palette& p = st.palette;
    float textWidth = MeasureWidth(*res.measure, res.family.get(), st.brand,
                                   10.f * s, gp::FontStyleBold);
    float markWidth = 12.f * s;
    float total = markWidth + 10.f * s + textWidth;
    float x = centered ? r.X + (r.Width - total) / 2 : r.X;
    float y = r.Y + (r.Height - markWidth) / 2;
    gp::SolidBrush accent(ToColor(p.accent));
    float tile = 5.f * s;
    g.FillRectangle(&accent, x, y, tile, tile);
    g.FillRectangle(&accent, x + 7.f * s, y, tile, tile);
    g.FillRectangle(&accent, x, y + 7.f * s, tile, tile);
    gp::SolidBrush faded(ToColor(p.accent, 0.55f));
    g.FillRectangle(&faded, x + 7.f * s, y + 7.f * s, tile, tile);
    DrawTextBox(g, res.family.get(), st.brand, 10.f * s, gp::FontStyleBold,
                ToColor(Mix(p.subtext, p.accent, 0.35f)),
                gp::RectF(x + markWidth + 10.f * s, r.Y, textWidth + 8.f * s, r.Height),
                gp::StringAlignmentNear, false);
}

void DrawButton(gp::Graphics& g,
                const Settings& st,
                const Resources& res,
                const gp::RectF& r,
                float s,
                bool hover) {
    const Palette& p = st.palette;
    float radius = Smaller(st.cornerRadius * 0.5f, 9.f) * s;
    gp::RectF inset = r;
    inset.Inflate(-s, -s);
    FillRounded(g, inset, radius,
                ToColor(Mix(p.surface, p.accent, hover ? 0.2f : 0.09f),
                        Larger(st.cardOpacity, 0.85f)));
    StrokeRounded(g, inset, radius,
                  ToColor(Mix(p.border, p.accent, hover ? 0.55f : 0.22f)), s);
    DrawTextBox(g, res.family.get(), st.skipButton, 12.f * s, gp::FontStyleBold,
                ToColor(p.text), r, gp::StringAlignmentCenter, false);
}

void DrawProgressBar(gp::Graphics& g,
                     const Palette& p,
                     const gp::RectF& r,
                     float fraction,
                     float trackAlpha) {
    float radius = r.Height / 2;
    FillRounded(g, r, radius, ToColor(Mix(p.surface, p.border, 0.9f), trackAlpha));
    fraction = Clamp(fraction, 0.f, 1.f);
    if (fraction > 0.002f) {
        gp::RectF fill = r;
        fill.Width = Larger(r.Height, r.Width * fraction);
        gp::GraphicsPath path;
        RoundedPath(&path, fill, radius);
        gp::LinearGradientBrush brush(fill, ToColor(Mix(p.accent, p.busy, 0.35f)),
                                      ToColor(p.accent),
                                      gp::LinearGradientModeHorizontal);
        g.FillPath(&brush, &path);
    }
}

// Layout -----------------------------------------------------------------------

bool IsFloating(const Settings& st) {
    return st.backdropOpacity < 0.5f;
}

void PlaceBlock(const Settings& st, float mw, float mh, float w, float h, float s,
                float* x, float* y) {
    float margin = (IsFloating(st) ? 64.f : 40.f) * s;
    switch (st.position) {
        case Position::Top:
            *x = (mw - w) / 2;
            *y = Larger(margin, mh * 0.1f);
            break;
        case Position::Bottom:
            *x = (mw - w) / 2;
            *y = mh - h - Larger(margin, mh * 0.08f);
            break;
        case Position::TopLeft:
            *x = margin;
            *y = margin;
            break;
        case Position::TopRight:
            *x = mw - w - margin;
            *y = margin;
            break;
        case Position::BottomLeft:
            *x = margin;
            *y = mh - h - margin;
            break;
        case Position::BottomRight:
            *x = mw - w - margin;
            *y = mh - h - margin;
            break;
        default:
            *x = (mw - w) / 2;
            *y = (mh - h) / 2;
    }
    *x = Larger(*x, 0.f);
    *y = Larger(*y, 0.f);
}

Layout ComputeConsoleLayout(const Settings& st,
                            Resources& res,
                            const Frame& f,
                            int width,
                            int height,
                            UINT dpi) {
    Layout L;
    L.valid = true;
    L.console = true;
    float s = (dpi / 96.f) * st.scale;
    L.s = s;
    // Retro draws into a low-resolution "framebuffer" that is scaled up with
    // hard pixel edges. Everything below is in framebuffer pixels.
    L.pixel = st.consoleRendering == ConsoleRendering::Retro
                  ? Larger(2, (int)std::lround(s * 1.5f))
                  : 1;
    L.unit = L.pixel > 1 ? 1.f : s;
    L.fbW = (width + L.pixel - 1) / L.pixel;
    L.fbH = (height + L.pixel - 1) / L.pixel;
    L.fontPx = L.pixel > 1 ? st.consoleFontSize : st.consoleFontSize * s;
    {
        gp::Graphics& g = *res.measure;
        g.SetTextRenderingHint(st.consoleRendering == ConsoleRendering::Smooth
                                   ? gp::TextRenderingHintAntiAliasGridFit
                                   : gp::TextRenderingHintSingleBitPerPixelGridFit);
        gp::Font font(res.monoFamily.get(), L.fontPx, gp::FontStyleRegular, gp::UnitPixel);
        L.lineH = std::ceil(font.GetHeight(&g));
        L.charW = MeasureWidth(g, res.monoFamily.get(), L"0123456789", L.fontPx,
                               gp::FontStyleRegular) / 10.f;
        g.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
    }
    L.marginX = std::round(4.f * L.unit);
    L.logTop = std::round(2.f * L.unit);
    L.maxLines = Larger(2, (int)((L.fbH - L.logTop) / L.lineH));
    L.columns = Larger(1, (int)((L.fbW - L.marginX * 2) / Larger(1.f, L.charW)));
    if (st.consoleLogo) {
        // One penguin per CPU, as many as fit, like the framebuffer console.
        L.logoSize = std::round(80.f * L.unit);
        float step = L.logoSize + std::round(4.f * L.unit);
        L.logoRows = (int)std::ceil(step / L.lineH);
        L.logoCount = Clamp((int)((L.fbW - L.marginX * 2) / step), 1, Larger(1, res.cpuCount));
    }
    // Rows scroll like a terminal: the logo, then the log (long lines wrap
    // onto extra rows), then the cursor.
    int rows = L.logoRows;
    if (f.log) {
        size_t count = Smaller(f.logCount, f.log->size());
        for (size_t i = 0; i < count; i++) {
            rows += LogLineRows(st, (*f.log)[i], L.columns);
        }
    }
    L.scroll = Larger(0, rows + 1 - L.maxLines);
    L.cursorY = L.logTop + (rows - L.scroll) * L.lineH;
    float rowH = L.lineH + 1;
    L.jobLine = gp::RectF(0, L.cursorY * L.pixel, (gp::REAL)width, rowH * L.pixel);
    L.content = gp::RectF(0, 0, (gp::REAL)width, (gp::REAL)height);
    return L;
}

Layout ComputeLayout(const Settings& st,
                     Resources& res,
                     const Frame& f,
                     int width,
                     int height,
                     UINT dpi,
                     bool showCard) {
    if (showCard && st.style == Style::Console) {
        return ComputeConsoleLayout(st, res, f, width, height, dpi);
    }
    Layout L;
    float mw = (float)width, mh = (float)height;
    float s = (dpi / 96.f) * st.scale;
    size_t n = st.items.size();
    gp::Graphics& g = *res.measure;
    std::wstring headline = FillTokens(HeadlineText(st, f), res, f, n);
    std::wstring sub = FillTokens(SubheadingText(st, f), res, f, n);
    std::wstring hint = FillTokens(st.hint, res, f, n);

    for (int attempt = 0; attempt < 3; attempt++) {
        L = Layout();
        L.s = s;
        float y = 0;
        float W;
        if (!showCard) {
            W = Smaller(420.f * s, mw - 40.f * s);
            L.brand = gp::RectF(0, 0, W, 24.f * s);
            y = 24.f * s;
        } else {
            bool minimal = st.style == Style::Minimal;
            W = Smaller((minimal ? Smaller(st.cardWidth, 700.f) : st.cardWidth) * s,
                        mw - 40.f * s);
            L.narrow = W < 560.f * s;
            if (st.showClock) {
                L.clock = gp::RectF(0, y, W, 72.f * s);
                L.date = gp::RectF(0, y + 72.f * s, W, 24.f * s);
                y += 72.f * s + 24.f * s + 22.f * s;
            }
            if (st.showBrand && !st.brand.empty()) {
                L.brand = gp::RectF(0, y, W, 22.f * s);
                y += 22.f * s + 20.f * s;
            }
            float headSize = (minimal ? 34.f : 30.f) * s;
            float hh = MeasureHeight(g, res.family.get(), headline, headSize,
                                     gp::FontStyleBold, W, 2);
            L.headline = gp::RectF(0, y, W, hh);
            y += hh + 6.f * s;
            if (!sub.empty()) {
                float subSize = (minimal ? 15.f : 14.f) * s;
                float sh = MeasureHeight(g, res.family.get(), sub, subSize,
                                         gp::FontStyleRegular, W - 20.f * s, 2);
                L.sub = gp::RectF(10.f * s, y, W - 20.f * s, sh);
                y += sh;
            }
            y += (minimal ? 30.f : 28.f) * s;

            if (!minimal) {
                L.rowH = (st.showDetails ? 74.f : 58.f) * s;
                float footerH = 78.f * s;
                if (L.narrow && st.allowSkip) {
                    footerH = 128.f * s;
                }
                float cardH = 8.f * s + n * L.rowH + footerH;
                L.card = gp::RectF(0, y, W, cardH);
                for (size_t i = 0; i < n; i++) {
                    L.rows.push_back(gp::RectF(22.f * s, y + 8.f * s + i * L.rowH,
                                               W - 44.f * s, L.rowH));
                }
                float footerTop = y + 8.f * s + n * L.rowH;
                L.progressBar = gp::RectF(22.f * s, footerTop - 1.5f * s,
                                          W - 44.f * s, 3.f * s);
                float textW = L.narrow ? W - 48.f * s : W - 48.f * s - 150.f * s;
                L.footerText = gp::RectF(24.f * s, footerTop + 16.f * s, textW, 22.f * s);
                L.footerSub = gp::RectF(24.f * s, footerTop + 40.f * s, textW, 20.f * s);
                if (st.allowSkip) {
                    L.skip = L.narrow
                                 ? gp::RectF(24.f * s, footerTop + 76.f * s,
                                             W - 48.f * s, 38.f * s)
                                 : gp::RectF(W - 24.f * s - 136.f * s,
                                             footerTop + 19.f * s, 136.f * s, 40.f * s);
                }
                y += cardH;
            } else {
                if (st.showProgressBar) {
                    float bw = Smaller(W, 440.f * s);
                    L.progressBar = gp::RectF((W - bw) / 2, y, bw, 6.f * s);
                    y += 6.f * s + 24.f * s;
                }
                // Chips flow layout, centered per line.
                float chipH = 34.f * s, gap = 10.f * s;
                std::vector<float> widths;
                for (const auto& item : st.items) {
                    float tw = MeasureWidth(g, res.family.get(), item.name,
                                            13.f * s, gp::FontStyleBold);
                    widths.push_back(Smaller(W, 14.f * s + 10.f * s + 9.f * s + tw + 16.f * s));
                }
                std::vector<std::pair<size_t, size_t>> lines;
                size_t lineStart = 0;
                float lineW = 0;
                for (size_t i = 0; i < widths.size(); i++) {
                    float add = (i == lineStart ? 0 : gap) + widths[i];
                    if (i > lineStart && lineW + add > W) {
                        lines.push_back({lineStart, i});
                        lineStart = i;
                        lineW = widths[i];
                    } else {
                        lineW += add;
                    }
                }
                if (lineStart < widths.size()) {
                    lines.push_back({lineStart, widths.size()});
                }
                L.chipsArea = gp::RectF(0, y, W, 0);
                L.rows.resize(n);
                for (const auto& line : lines) {
                    float total = 0;
                    for (size_t i = line.first; i < line.second; i++) {
                        total += widths[i] + (i == line.first ? 0 : gap);
                    }
                    float x = (W - total) / 2;
                    for (size_t i = line.first; i < line.second; i++) {
                        L.rows[i] = gp::RectF(x, y, widths[i], chipH);
                        x += widths[i] + gap;
                    }
                    y += chipH + gap;
                }
                if (!lines.empty()) {
                    y += 10.f * s;
                }
                L.status = gp::RectF(0, y, W, 22.f * s);
                y += 22.f * s + 18.f * s;
                if (st.allowSkip) {
                    float bw = Smaller(W, 156.f * s);
                    L.skip = gp::RectF((W - bw) / 2, y, bw, 38.f * s);
                    y += 38.f * s;
                }
            }
            if (!hint.empty()) {
                y += 18.f * s;
                float hh2 = MeasureHeight(g, res.family.get(), hint, 11.f * s,
                                          gp::FontStyleRegular, W, 2);
                L.hint = gp::RectF(0, y, W, hh2);
                y += hh2;
            }
        }
        float H = y;
        float limit = mh - 32.f * s;
        if (H > limit && attempt < 2 && H > 1) {
            s *= Larger(0.35f, limit / H * 0.98f);
            continue;
        }

        float bx, by;
        PlaceBlock(st, mw, mh, W, H, s, &bx, &by);
        if (!showCard) {
            bx = (mw - W) / 2;
            by = (mh - H) / 2;
        }
        by += f.slide * s;
        gp::RectF* rects[] = {&L.clock, &L.date, &L.brand, &L.headline, &L.sub,
                              &L.card, &L.progressBar, &L.footerText, &L.footerSub,
                              &L.skip, &L.hint, &L.chipsArea, &L.status};
        for (gp::RectF* r : rects) {
            if (r->Width > 0 || r->Height > 0) {
                *r = Offset(*r, bx, by);
            }
        }
        for (auto& r : L.rows) {
            r = Offset(r, bx, by);
        }
        if (showCard) {
            bool minimal = st.style == Style::Minimal;
            L.statusLabels = !minimal && st.showStatusLabels && !L.narrow;
            L.indicatorRadius = (minimal ? 5.f : 7.5f) * s;
            for (const auto& r : L.rows) {
                float x = minimal          ? r.X + 19.f * s
                          : L.statusLabels ? r.X + r.Width - 104.f * s
                                           : r.X + r.Width - 12.f * s;
                L.indicators.push_back(gp::PointF(x, r.Y + r.Height / 2));
            }
        }
        if (showCard && IsFloating(st)) {
            float inset = 24.f * s;
            L.panel = gp::RectF(bx - inset, by - inset, W + inset * 2, H + inset * 2);
        }
        float pad = 50.f * s;
        L.content = gp::RectF(bx - pad, by - pad, W + pad * 2, H + pad * 2);
        L.valid = true;
        break;
    }
    return L;
}

// Console ----------------------------------------------------------------------

// Writes colored segments on a monospace grid so they line up like a terminal.
class ConsoleWriter {
   public:
    ConsoleWriter(gp::Graphics& g, const Resources& res, const Layout& L, bool allowBold)
        : g_(g),
          charW_(L.charW),
          lineH_(L.lineH),
          columns_(Larger(1, L.columns)),
          allowBold_(allowBold),
          regular_(res.monoFamily.get(), L.fontPx, gp::FontStyleRegular, gp::UnitPixel),
          bold_(res.monoFamily.get(), L.fontPx, gp::FontStyleBold, gp::UnitPixel) {}

    void Start(float x, float y) {
        x_ = x;
        y_ = y;
        column_ = 0;
    }

    // A real console has no bold font; "bold" text is just a brighter color.
    // Text wraps at the last column, like a tty.
    void Put(const std::wstring& text, const Rgb& color, bool bold = false) {
        gp::SolidBrush brush(ToColor(color));
        for (size_t pos = 0; pos < text.size();) {
            if (column_ >= columns_) {
                column_ = 0;
                y_ += lineH_;
            }
            size_t take = Smaller(text.size() - pos, (size_t)(columns_ - column_));
            g_.DrawString(text.c_str() + pos, (INT)take, bold && allowBold_ ? &bold_ : &regular_,
                          gp::PointF(x_ + column_ * charW_, y_),
                          gp::StringFormat::GenericTypographic(), &brush);
            column_ += (int)take;
            pos += take;
        }
    }

   private:
    gp::Graphics& g_;
    float charW_;
    float lineH_;
    int columns_;
    bool allowBold_;
    gp::Font regular_, bold_;
    float x_ = 0, y_ = 0;
    int column_ = 0;
};

void DrawLogLine(ConsoleWriter& w,
                 const Settings& st,
                 const ConsolePalette& cp,
                 const LogLine& line) {
    if (line.kind == kLogBlank) {
        return;
    }
    if (ShowsTimestamp(st, line.kind)) {
        w.Put(LogTimestamp(line.seconds),
              st.consoleTimestamps == ConsoleTimestamps::Kernel ? cp.text : cp.dim);
    }
    switch (line.kind) {
        case kLogKernel:
        case kLogPlain:
            w.Put(line.text, cp.text);
            return;
        case kLogWelcome:
            w.Put(L"Welcome to ", cp.text);
            w.Put(line.text, cp.welcome, true);
            w.Put(L"!", cp.text);
            return;
    }
    if (const wchar_t* tag = LogTag(line.kind)) {
        Rgb tagColor = line.kind == kLogOk                                 ? cp.ok
                       : line.kind == kLogWarn || line.kind == kLogSkip ? cp.warn
                                                                          : cp.fail;
        w.Put(L"[", cp.text);
        w.Put(tag, tagColor, true);
        w.Put(L"] ", cp.text);
    } else {
        w.Put(L"         ", cp.text);
    }
    Rgb body = line.kind == kLogDetail ? cp.dim : cp.text;
    size_t at = line.highlight.empty() ? std::wstring::npos : line.text.find(line.highlight);
    if (at == std::wstring::npos) {
        w.Put(line.text, body);
        return;
    }
    w.Put(line.text.substr(0, at), body);
    w.Put(line.highlight, line.kind == kLogDetail ? cp.text : cp.bright,
          line.kind != kLogDetail);
    w.Put(line.text.substr(at + line.highlight.size()), body);
}

// A small penguin, drawn from shapes: the framebuffer console shows one per CPU.
void DrawPenguin(gp::Graphics& g, float x, float y, float size) {
    float k = size / 80.f;
    gp::SolidBrush body(gp::Color(255, 22, 22, 22));
    gp::SolidBrush belly(gp::Color(255, 238, 238, 232));
    gp::SolidBrush orange(gp::Color(255, 240, 176, 16));
    gp::SolidBrush white(gp::Color(255, 255, 255, 255));
    gp::SolidBrush black(gp::Color(255, 0, 0, 0));
    gp::Pen rim(gp::Color(255, 120, 120, 120), Larger(1.f, 1.4f * k));
    auto fill = [&](gp::Brush* brush, float ex, float ey, float ew, float eh) {
        g.FillEllipse(brush, x + ex * k, y + ey * k, ew * k, eh * k);
    };
    auto outlined = [&](float ex, float ey, float ew, float eh) {
        fill(&body, ex, ey, ew, eh);
        g.DrawEllipse(&rim, x + ex * k, y + ey * k, ew * k, eh * k);
    };
    outlined(6, 32, 20, 32);   // Left flipper.
    outlined(54, 32, 20, 32);  // Right flipper.
    outlined(14, 4, 52, 70);   // Body.
    fill(&belly, 23, 30, 34, 42);
    fill(&orange, 12, 65, 27, 12);  // Feet.
    fill(&orange, 41, 65, 27, 12);
    fill(&white, 27, 13, 12, 15);  // Eyes.
    fill(&white, 41, 13, 12, 15);
    fill(&black, 32, 18, 5, 8);
    fill(&black, 43, 18, 5, 8);
    fill(&orange, 30, 27, 20, 9);  // Beak.
}

void PrepareConsoleGraphics(gp::Graphics& g, const Settings& st, bool framebuffer) {
    bool smooth = st.consoleRendering == ConsoleRendering::Smooth;
    g.SetTextRenderingHint(smooth ? gp::TextRenderingHintAntiAliasGridFit
                                  : gp::TextRenderingHintSingleBitPerPixelGridFit);
    g.SetSmoothingMode(framebuffer ? gp::SmoothingModeNone : gp::SmoothingModeAntiAlias);
}

void RestoreGraphics(gp::Graphics& g) {
    g.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
    g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
}

// Scales a framebuffer bitmap up with hard pixel edges.
void UpscaleInto(gp::Graphics& g, gp::Bitmap& source, const gp::RectF& dest) {
    gp::InterpolationMode interpolation = g.GetInterpolationMode();
    gp::PixelOffsetMode offset = g.GetPixelOffsetMode();
    g.SetInterpolationMode(gp::InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(gp::PixelOffsetModeHalf);
    g.DrawImage(&source, dest, 0, 0, (gp::REAL)source.GetWidth(),
                (gp::REAL)source.GetHeight(), gp::UnitPixel);
    g.SetInterpolationMode(interpolation);
    g.SetPixelOffsetMode(offset);
}

void DrawConsoleContent(gp::Graphics& g,
                        const Settings& st,
                        const Resources& res,
                        const Frame& f,
                        const Layout& L) {
    ConsolePalette cp = MakeConsolePalette(st);
    ConsoleWriter w(g, res, L, st.consoleRendering == ConsoleRendering::Smooth);
    float logoY = L.logTop - L.scroll * L.lineH;
    if (L.logoCount > 0 && logoY + L.logoSize > 0) {
        float step = L.logoSize + std::round(4.f * L.unit);
        for (int i = 0; i < L.logoCount; i++) {
            DrawPenguin(g, L.marginX + i * step, logoY, L.logoSize);
        }
    }
    if (f.log) {
        size_t count = Smaller(f.logCount, f.log->size());
        int row = L.logoRows;
        for (size_t i = 0; i < count; i++) {
            int rows = LogLineRows(st, (*f.log)[i], L.columns);
            if (row + rows > L.scroll) {
                w.Start(L.marginX, L.logTop + (row - L.scroll) * L.lineH);
                DrawLogLine(w, st, cp, (*f.log)[i]);
            }
            row += rows;
        }
    }
}

void DrawConsole(gp::Graphics& g,
                 const Settings& st,
                 const Resources& res,
                 const Frame& f,
                 const Layout& L) {
    if (L.pixel <= 1) {
        PrepareConsoleGraphics(g, st, false);
        DrawConsoleContent(g, st, res, f, L);
        RestoreGraphics(g);
        return;
    }
    gp::Bitmap framebuffer(L.fbW, L.fbH, PixelFormat32bppPARGB);
    {
        gp::Graphics fg(&framebuffer);
        fg.Clear(gp::Color(0, 0, 0, 0));
        PrepareConsoleGraphics(fg, st, true);
        DrawConsoleContent(fg, st, res, f, L);
    }
    UpscaleInto(g, framebuffer,
                gp::RectF(0, 0, (gp::REAL)L.fbW * L.pixel, (gp::REAL)L.fbH * L.pixel));
}

std::wstring FormatTimespan(int seconds) {
    if (seconds < 60) {
        return std::to_wstring(seconds) + L"s";
    }
    return std::to_wstring(seconds / 60) + L"min" +
           (seconds % 60 ? L" " + std::to_wstring(seconds % 60) + L"s" : L"");
}

// The cursor row: systemd's "A start job is running for ..." line with its
// bouncing asterisks, or a blinking underline cursor.
void DrawConsoleRowContent(gp::Graphics& g,
                           const Settings& st,
                           const Resources& res,
                           const Frame& f,
                           const Layout& L,
                           float y) {
    ConsolePalette cp = MakeConsolePalette(st);
    if (!f.jobText.empty()) {
        ConsoleWriter w(g, res, L, st.consoleRendering == ConsoleRendering::Smooth);
        w.Start(L.marginX, y);
        if (st.consoleTimestamps == ConsoleTimestamps::All) {
            w.Put(std::wstring(15, L' '), cp.text);
        }
        w.Put(L"[", cp.text);
        for (int i = 0; i < 6; i++) {
            int distance = std::abs(i - f.cylon);
            w.Put(distance <= 1 ? L"*" : L" ", distance == 0 ? cp.cylon : cp.cylonDim, true);
        }
        w.Put(L"] ", cp.text);
        size_t at = f.jobName.empty() ? std::wstring::npos : f.jobText.find(f.jobName);
        if (at == std::wstring::npos) {
            w.Put(f.jobText, cp.text);
        } else {
            w.Put(f.jobText.substr(0, at), cp.text);
            w.Put(f.jobName, cp.bright, true);
            w.Put(f.jobText.substr(at + f.jobName.size()), cp.text);
        }
    } else if (f.cursorOn) {
        gp::SolidBrush brush(ToColor(cp.text));
        float thickness = Larger(1.f, std::round(2.f * L.unit));
        g.FillRectangle(&brush, L.marginX, y + L.lineH - thickness - std::round(L.unit),
                        std::round(L.charW), thickness);
    }
}

void DrawConsoleCursorRow(gp::Graphics& g,
                          const Settings& st,
                          const Resources& res,
                          const Frame& f,
                          const Layout& L) {
    if (f.jobText.empty() && !f.cursorOn) {
        return;
    }
    if (L.pixel <= 1) {
        PrepareConsoleGraphics(g, st, false);
        DrawConsoleRowContent(g, st, res, f, L, L.cursorY);
        RestoreGraphics(g);
        return;
    }
    int rowH = (int)L.lineH + 1;
    gp::Bitmap row(L.fbW, rowH, PixelFormat32bppPARGB);
    {
        gp::Graphics rg(&row);
        rg.Clear(gp::Color(0, 0, 0, 0));
        PrepareConsoleGraphics(rg, st, true);
        DrawConsoleRowContent(rg, st, res, f, L, 0);
    }
    UpscaleInto(g, row,
                gp::RectF(0, L.cursorY * L.pixel, (gp::REAL)L.fbW * L.pixel,
                          (gp::REAL)rowH * L.pixel));
}

// Scene ------------------------------------------------------------------------

void DrawScene(gp::Graphics& g,
               const Settings& st,
               Resources& res,
               const Frame& f,
               const Layout& L,
               bool showCard) {
    const Palette& p = st.palette;
    float s = L.s;
    size_t n = st.items.size();
    const std::vector<ItemStatus>& status = *f.status;

    if (!showCard) {
        DrawBrand(g, st, res, L.brand, s, true);
        return;
    }
    if (L.console) {
        DrawConsole(g, st, res, f, L);
        return;
    }

    auto drawSurface = [&](const gp::RectF& r) {
        float radius = st.cornerRadius * s;
        float ca = st.cardOpacity;
        for (int i = 5; i >= 1; i--) {
            gp::RectF shadow = r;
            shadow.Inflate(i * 2.4f * s, i * 2.4f * s);
            shadow.Y += 6.f * s;
            FillRounded(g, shadow, radius + i * 2.4f * s,
                        gp::Color((BYTE)(p.light ? 3 * ca : 9 * ca), 0, 0, 0));
        }
        FillRounded(g, r, radius, ToColor(p.surface, ca));
        StrokeRounded(g, r, radius, ToColor(p.border, Larger(ca, 0.6f)), s);
        if (radius * 2 < r.Width) {
            gp::Pen highlight(ToColor(Mix(p.border, p.text, 0.25f), 0.35f * ca), s);
            g.DrawLine(&highlight, r.X + radius + 6.f * s, r.Y + 0.5f * s,
                       r.X + r.Width - radius - 6.f * s, r.Y + 0.5f * s);
        }
    };
    bool onPanel = L.panel.Width > 0;
    if (onPanel) {
        drawSurface(L.panel);
    }

    if (L.clock.Width > 0) {
        DrawTextBox(g, res.lightFamily.get(), TimeText(), 64.f * s,
                    gp::FontStyleRegular, ToColor(p.text), L.clock,
                    gp::StringAlignmentCenter, false);
        DrawTextBox(g, res.family.get(), DateText(), 15.f * s, gp::FontStyleRegular,
                    ToColor(p.subtext), L.date, gp::StringAlignmentCenter, false);
    }
    if (L.brand.Width > 0) {
        DrawBrand(g, st, res, L.brand, s, true);
    }
    bool minimal = st.style == Style::Minimal;
    DrawTextBox(g, res.family.get(), FillTokens(HeadlineText(st, f), res, f, n),
                (minimal ? 34.f : 30.f) * s, gp::FontStyleBold, ToColor(p.text),
                L.headline, gp::StringAlignmentCenter, true);
    if (L.sub.Width > 0) {
        DrawTextBox(g, res.family.get(), FillTokens(SubheadingText(st, f), res, f, n),
                    (minimal ? 15.f : 14.f) * s, gp::FontStyleRegular,
                    ToColor(p.subtext), L.sub, gp::StringAlignmentCenter, true);
    }

    std::wstring progressLine = FillTokens(st.progress, res, f, n);
    std::wstring secondLine;
    if (f.outcome != Outcome::None) {
        secondLine = FillTokens(st.opening, res, f, n);
    } else if (st.showCountdown && f.secondsLeft >= 0) {
        secondLine = FillTokens(st.countdown, res, f, n);
    }

    if (!minimal) {
        const gp::RectF& card = L.card;
        float ca = st.cardOpacity;
        if (onPanel) {
            gp::Pen separator(ToColor(Mix(p.surface, p.border, 0.75f), ca), s);
            g.DrawLine(&separator, card.X + 22.f * s, card.Y, card.X + card.Width - 22.f * s,
                       card.Y);
        } else {
            drawSurface(card);
        }

        for (size_t i = 0; i < n && i < L.rows.size(); i++) {
            const gp::RectF& row = L.rows[i];
            int state = i < status.size() ? status[i].state : kWaiting;
            float dim = (state == kWaiting) ? 0.75f : 1.f;
            float textLeft = row.X;
            if (st.showIcons) {
                float tileSize = 40.f * s;
                gp::RectF tile(row.X, row.Y + (row.Height - tileSize) / 2, tileSize,
                               tileSize);
                DrawIconTile(g, st, res, i, tile, s, state == kWaiting ? 0.7f : 1.f);
                textLeft = tile.X + tile.Width + 15.f * s;
            }
            bool labels = L.statusLabels;
            float indicatorX = L.indicators[i].X;
            float textW = Larger(20.f * s, indicatorX - 18.f * s - textLeft);
            const std::wstring& name = st.items[i].name;
            std::wstring detail = !st.items[i].subtitle.empty()
                                      ? st.items[i].subtitle
                                      : (i < status.size() ? status[i].detail : L"");
            if (st.showDetails) {
                DrawTextBox(g, res.family.get(), name, 15.f * s, gp::FontStyleBold,
                            ToColor(p.text, dim), gp::RectF(textLeft, row.Y + 13.f * s, textW, 25.f * s),
                            gp::StringAlignmentNear, false);
                DrawTextBox(g, res.family.get(), detail, 12.5f * s, gp::FontStyleRegular,
                            ToColor(p.subtext), gp::RectF(textLeft, row.Y + 38.f * s, textW, 22.f * s),
                            gp::StringAlignmentNear, false);
            } else {
                DrawTextBox(g, res.family.get(), name, 15.f * s, gp::FontStyleBold,
                            ToColor(p.text, dim), gp::RectF(textLeft, row.Y, textW, row.Height),
                            gp::StringAlignmentNear, false);
            }
            if (labels) {
                const wchar_t* label = state == kReady       ? L"Ready"
                                       : state == kStarting   ? L"Starting"
                                       : state == kUnavailable ? L"Unavailable"
                                       : st.items[i].optional ? L"Optional"
                                                              : L"Waiting";
                DrawTextBox(g, res.family.get(), label, 12.f * s, gp::FontStyleRegular,
                            ToColor(StateColor(p, state)),
                            gp::RectF(indicatorX + 17.f * s, row.Y, 90.f * s, row.Height),
                            gp::StringAlignmentNear, false);
            }
            if (i + 1 < n) {
                gp::Pen separator(ToColor(Mix(p.surface, p.border, 0.55f), ca), s);
                g.DrawLine(&separator, row.X, row.Y + row.Height, row.X + row.Width,
                           row.Y + row.Height);
            }
        }

        if (st.showProgressBar) {
            DrawProgressBar(g, p, L.progressBar, f.progress, ca);
        } else {
            gp::Pen separator(ToColor(Mix(p.surface, p.border, 0.75f), ca), s);
            float y = L.progressBar.Y + L.progressBar.Height / 2;
            g.DrawLine(&separator, L.progressBar.X, y,
                       L.progressBar.X + L.progressBar.Width, y);
        }
        DrawTextBox(g, res.family.get(), progressLine, 13.f * s, gp::FontStyleBold,
                    ToColor(Mix(p.text, p.subtext, 0.15f)), L.footerText,
                    gp::StringAlignmentNear, false);
        DrawTextBox(g, res.family.get(), secondLine, 11.5f * s, gp::FontStyleRegular,
                    ToColor(Mix(p.subtext, p.surface, 0.12f)), L.footerSub,
                    gp::StringAlignmentNear, false);
    } else {
        if (L.progressBar.Width > 0) {
            DrawProgressBar(g, p, L.progressBar, f.progress, Larger(st.cardOpacity, 0.6f));
        }
        for (size_t i = 0; i < n && i < L.rows.size(); i++) {
            const gp::RectF& chip = L.rows[i];
            if (chip.Width <= 0) {
                continue;
            }
            int state = i < status.size() ? status[i].state : kWaiting;
            Rgb color = StateColor(p, state);
            FillRounded(g, chip, chip.Height / 2, ToColor(p.surface, st.cardOpacity));
            StrokeRounded(g, chip, chip.Height / 2,
                          ToColor(Mix(p.border, color, state == kReady ? 0.55f : 0.25f)), s);
            DrawTextBox(g, res.family.get(), st.items[i].name, 13.f * s,
                        gp::FontStyleBold,
                        ToColor(p.text, state == kWaiting ? 0.7f : 1.f),
                        gp::RectF(chip.X + 33.f * s, chip.Y, chip.Width - 40.f * s, chip.Height),
                        gp::StringAlignmentNear, false);
        }
        std::wstring line = progressLine;
        if (!secondLine.empty()) {
            line += line.empty() ? secondLine : L"  \u00B7  " + secondLine;
        }
        DrawTextBox(g, res.family.get(), line, 12.5f * s, gp::FontStyleRegular,
                    ToColor(p.subtext), L.status, gp::StringAlignmentCenter, false);
    }

    if (L.skip.Width > 0) {
        DrawButton(g, st, res, L.skip, s, f.hoverSkip);
    }
    if (L.hint.Width > 0) {
        DrawTextBox(g, res.family.get(), FillTokens(st.hint, res, f, n), 11.f * s,
                    gp::FontStyleRegular, ToColor(Mix(p.subtext, p.bg1, 0.3f)),
                    L.hint, gp::StringAlignmentCenter, true);
    }
}

void DrawIndicators(gp::Graphics& g,
                    const Settings& st,
                    const Resources& res,
                    const Frame& f,
                    const Layout& L) {
    if (L.console) {
        DrawConsoleCursorRow(g, st, res, f, L);
        return;
    }
    for (size_t i = 0; i < L.indicators.size(); i++) {
        int state = i < f.status->size() ? (*f.status)[i].state : kWaiting;
        DrawIndicator(g, st.palette, L.indicators[i], L.indicatorRadius, state, f.spin,
                      i < f.readyAnim.size() ? f.readyAnim[i] : 1.f, st.reduceMotion,
                      1.f);
    }
}

void DrawGlow(gp::Graphics& g, const gp::RectF& r, gp::Color center) {
    gp::GraphicsPath path;
    path.AddEllipse(r);
    gp::PathGradientBrush brush(&path);
    brush.SetCenterColor(center);
    gp::Color surround(0, center.GetR(), center.GetG(), center.GetB());
    INT count = 1;
    brush.SetSurroundColors(&surround, &count);
    g.FillPath(&brush, &path);
}

void DrawPreviewBadge(gp::Graphics& g,
                      const Settings& st,
                      const Resources& res,
                      int width,
                      float s) {
    const Palette& p = st.palette;
    std::wstring text = L"PREVIEW  \u00B7  APPS ARE NOT CHECKED";
    float tw = MeasureWidth(*res.measure, res.family.get(), text, 9.f * s,
                            gp::FontStyleBold);
    gp::RectF badge(width - tw - 40.f * s - 18.f * s, 18.f * s, tw + 40.f * s, 28.f * s);
    FillRounded(g, badge, 8.f * s, ToColor(Mix(p.surface, p.accent, 0.08f), 0.95f));
    StrokeRounded(g, badge, 8.f * s, ToColor(Mix(p.border, p.accent, 0.35f)), s);
    DrawTextBox(g, res.family.get(), text, 9.f * s, gp::FontStyleBold,
                ToColor(Mix(p.text, p.accent, 0.3f)), badge, gp::StringAlignmentCenter,
                false);
}

// Renders the static background into a premultiplied 32-bit buffer.
void RenderBackground(uint32_t* bits,
                      int width,
                      int height,
                      UINT dpi,
                      const Settings& st,
                      const Resources& res,
                      bool preview) {
    const Palette& p = st.palette;
    gp::Bitmap surface(width, height, width * 4, PixelFormat32bppPARGB, (BYTE*)bits);
    {
        gp::Graphics g(&surface);
        g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(gp::PixelOffsetModeHighQuality);
        g.SetCompositingMode(gp::CompositingModeSourceCopy);
        gp::RectF all(0, 0, (gp::REAL)width, (gp::REAL)height);
        gp::Bitmap* image = res.backgroundImage.get();
        bool classicConsole = st.style == Style::Console && st.consoleClassic;
        bool useImage = !classicConsole &&
                        (st.background == Background::Image ||
                         st.background == Background::Wallpaper) && image;
        if (classicConsole) {
            gp::SolidBrush brush(ToColor(MakeConsolePalette(st).bg));
            g.FillRectangle(&brush, all);
        } else if (st.background == Background::Solid) {
            gp::SolidBrush brush(ToColor(p.bg1));
            g.FillRectangle(&brush, all);
        } else {
            gp::LinearGradientBrush brush(gp::RectF(0, -1, (gp::REAL)width, (gp::REAL)height + 2),
                                          ToColor(p.bg1), ToColor(p.bg2),
                                          gp::LinearGradientModeVertical);
            g.FillRectangle(&brush, all);
        }
        g.SetCompositingMode(gp::CompositingModeSourceOver);
        if (useImage) {
            float iw = (float)image->GetWidth(), ih = (float)image->GetHeight();
            float scale = Larger(width / iw, height / ih);
            float sw = width / scale, sh = height / scale;
            g.SetInterpolationMode(gp::InterpolationModeHighQualityBicubic);
            g.DrawImage(image, all, (iw - sw) / 2, (ih - sh) / 2, sw, sh, gp::UnitPixel);
            gp::SolidBrush dim(ToColor(p.light ? p.bg1 : p.bg2, st.backgroundDim));
            g.FillRectangle(&dim, all);
        } else if (!classicConsole && st.background == Background::Gradient &&
                   p.glowStrength > 0) {
            // Soft glows are rendered small and scaled up: cheap and smooth.
            int lw = Larger(1, width / 8), lh = Larger(1, height / 8);
            gp::Bitmap glow(lw, lh, PixelFormat32bppPARGB);
            {
                gp::Graphics lg(&glow);
                lg.SetSmoothingMode(gp::SmoothingModeAntiAlias);
                lg.Clear(gp::Color(0, 0, 0, 0));
                DrawGlow(lg, gp::RectF(lw * 0.17f, -lh * 0.48f, lw * 0.95f, lh * 1.2f),
                         ToColor(p.glow, Clamp(0.085f * p.glowStrength, 0.f, 0.5f)));
                DrawGlow(lg, gp::RectF(-lw * 0.3f, lh * 0.44f, lw * 0.9f, lh * 0.9f),
                         ToColor(Mix(p.glow, p.bg2, 0.25f),
                                 Clamp(0.05f * p.glowStrength, 0.f, 0.5f)));
            }
            gp::ImageAttributes attributes;
            attributes.SetWrapMode(gp::WrapModeTileFlipXY);
            g.SetInterpolationMode(gp::InterpolationModeHighQualityBilinear);
            g.DrawImage(&glow, all, 0, 0, (gp::REAL)lw, (gp::REAL)lh, gp::UnitPixel,
                        &attributes);
        }
    }
    if (st.backdropOpacity < 0.999f) {
        uint32_t factor = (uint32_t)std::lround(st.backdropOpacity * 256.f);
        size_t count = (size_t)width * height;
        for (size_t i = 0; i < count; i++) {
            uint32_t px = bits[i];
            uint32_t rb = ((px & 0x00FF00FF) * factor >> 8) & 0x00FF00FF;
            uint32_t ag = (((px >> 8) & 0x00FF00FF) * factor) & 0xFF00FF00;
            bits[i] = rb | ag;
        }
    }
    if (preview && st.style != Style::Console) {  // The console logs it instead.
        gp::Graphics g(&surface);
        g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
        DrawPreviewBadge(g, st, res, width, (dpi / 96.f) * Smaller(st.scale, 1.5f));
    }
}

void PrepareGraphics(gp::Graphics& g) {
    g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(gp::PixelOffsetModeHighQuality);
    g.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
    g.SetInterpolationMode(gp::InterpolationModeHighQualityBicubic);
}

////////////////////////////////////////////////////////////////////////////////
// The splash session: one UI thread with a layered window per monitor, plus a
// worker thread that checks readiness.

ULONGLONG ProcessAgeMs();
std::wstring CurrentLogonId();

std::wstring FormatSeconds(double seconds, int decimals) {
    wchar_t buffer[32];
    swprintf(buffer, 32, L"%.*fs", decimals, seconds);
    return buffer;
}

bool WriteUtf8File(const std::wstring& path, const std::wstring& text) {
    int bytes = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0,
                                    nullptr, nullptr);
    std::string utf8(bytes, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), utf8.data(), bytes, nullptr,
                        nullptr);
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(file, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    CloseHandle(file);
    return ok && written == utf8.size();
}

std::wstring RegistryString(const wchar_t* key, const wchar_t* value) {
    WCHAR buffer[256] = {};
    DWORD size = sizeof(buffer);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, key, value, RRF_RT_REG_SZ, nullptr, buffer, &size) !=
        ERROR_SUCCESS) {
        return L"";
    }
    return buffer;
}

struct WindowsVersion {
    std::wstring product;  // "Windows 11 Home"
    std::wstring display;  // "25H2"
    std::wstring kernel;   // "10.0.26200.1234"
    std::wstring full;     // "Windows 11 Home 25H2 (build 26200.1234)"
};

WindowsVersion GetWindowsVersion() {
    const wchar_t* key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
    WindowsVersion v;
    v.product = RegistryString(key, L"ProductName");
    if (v.product.empty()) {
        v.product = L"Windows";
    }
    RTL_OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    using RtlGetVersion_t = LONG(WINAPI*)(RTL_OSVERSIONINFOW*);
    auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion")));
    if (rtlGetVersion) {
        rtlGetVersion(&info);
    }
    // ProductName still says "Windows 10" on Windows 11.
    if (info.dwBuildNumber >= 22000) {
        v.product = ReplaceAll(v.product, L"Windows 10", L"Windows 11");
    }
    DWORD ubr = 0, size = sizeof(ubr);
    RegGetValueW(HKEY_LOCAL_MACHINE, key, L"UBR", RRF_RT_REG_DWORD, nullptr, &ubr, &size);
    v.display = RegistryString(key, L"DisplayVersion");
    v.kernel = std::to_wstring(info.dwMajorVersion) + L"." + std::to_wstring(info.dwMinorVersion) +
               L"." + std::to_wstring(info.dwBuildNumber) + L"." + std::to_wstring(ubr);
    v.full = v.product + (v.display.empty() ? L"" : L" " + v.display) + L" (build " +
             std::to_wstring(info.dwBuildNumber) + L"." + std::to_wstring(ubr) + L")";
    return v;
}

std::wstring HumanSize(ULONGLONG bytes) {
    double gib = bytes / 1073741824.0;
    wchar_t buffer[32];
    swprintf(buffer, 32, gib >= 1024 ? L"%.1f TiB" : L"%.1f GiB", gib >= 1024 ? gib / 1024 : gib);
    return buffer;
}

struct BootInfo {
    std::vector<std::wstring> lines;
    bool networkUp = false;
};

// dmesg-style messages built from this PC's real hardware and configuration.
BootInfo GatherBootInfo() {
    BootInfo info;
    std::vector<std::wstring>& out = info.lines;
    wchar_t buffer[160];

    WindowsVersion v = GetWindowsVersion();
    SYSTEM_INFO system{};
    GetNativeSystemInfo(&system);
    bool x64 = system.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
    bool arm64 = system.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64;
    out.push_back(L"Windows NT version " + v.kernel + L" (" + v.product +
                  (v.display.empty() ? L"" : L" " + v.display) + L") #1 SMP " +
                  (x64 ? L"x86_64" : arm64 ? L"aarch64" : L"i686"));

    std::wstring options = Trim(RegistryString(L"SYSTEM\\CurrentControlSet\\Control",
                                               L"SystemStartOptions"));
    while (options.find(L"  ") != std::wstring::npos) {
        options = ReplaceAll(options, L"  ", L" ");
    }
    if (!options.empty()) {
        out.push_back(L"Command line: " + options);
    }

    const wchar_t* bios = L"HARDWARE\\DESCRIPTION\\System\\BIOS";
    std::wstring vendor = Trim(RegistryString(bios, L"SystemManufacturer"));
    std::wstring product = Trim(RegistryString(bios, L"SystemProductName"));
    std::wstring board = Trim(RegistryString(bios, L"BaseBoardProduct"));
    if (!vendor.empty() || !product.empty()) {
        out.push_back(L"DMI: " + vendor + L" " + product + (board.empty() ? L"" : L"/" + board) +
                      L", BIOS " + Trim(RegistryString(bios, L"BIOSVersion")) + L" " +
                      Trim(RegistryString(bios, L"BIOSReleaseDate")));
    }

    const wchar_t* cpu = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
    DWORD mhz = 0, size = sizeof(mhz);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, cpu, L"~MHz", RRF_RT_REG_DWORD, nullptr, &mhz, &size) ==
            ERROR_SUCCESS &&
        mhz) {
        out.push_back(L"tsc: Detected " + std::to_wstring(mhz) + L".000 MHz processor");
    }
    std::wstring cpuName = Trim(RegistryString(cpu, L"ProcessorNameString"));
    if (!cpuName.empty()) {
        std::wstring identifier = RegistryString(cpu, L"Identifier");
        unsigned family = 0, model = 0, stepping = 0;
        size_t at = identifier.find(L"Family");
        std::wstring ids;
        if (at != std::wstring::npos &&
            swscanf(identifier.c_str() + at, L"Family %u Model %u Stepping %u", &family, &model,
                    &stepping) == 3) {
            swprintf(buffer, ARRAYSIZE(buffer), L" (family: 0x%x, model: 0x%x, stepping: 0x%x)",
                     family, model, stepping);
            ids = buffer;
        }
        out.push_back(L"smpboot: CPU0: " + cpuName + ids);
    }
    ULONG highestNode = 0;
    GetNumaHighestNodeNumber(&highestNode);
    out.push_back(L"smp: Brought up " + std::to_wstring(highestNode + 1) +
                  (highestNode ? L" nodes, " : L" node, ") +
                  std::to_wstring(GetActiveProcessorCount(ALL_PROCESSOR_GROUPS)) + L" CPUs");
    if (x64) {
        static const struct {
            DWORD feature;
            const wchar_t* name;
        } kFeatures[] = {{38, L"SSE4.2"}, {39, L"AVX"}, {40, L"AVX2"}, {41, L"AVX-512F"}};
        std::wstring features;
        for (const auto& feature : kFeatures) {
            if (IsProcessorFeaturePresent(feature.feature)) {
                features += L" ";
                features += feature.name;
            }
        }
        if (!features.empty()) {
            out.push_back(L"x86/cpu: Enabled" + features);
        }
    }

    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    if (GlobalMemoryStatusEx(&memory)) {
        out.push_back(L"Memory: " + std::to_wstring(memory.ullAvailPhys / 1024) + L"K/" +
                      std::to_wstring(memory.ullTotalPhys / 1024) + L"K available");
    }
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    swprintf(buffer, ARRAYSIZE(buffer), L"clocksource: Switched to clocksource qpc (%.3f MHz)",
             frequency.QuadPart / 1e6);
    out.push_back(buffer);

    // Graphics adapters with their real PCI vendor:device IDs, then displays.
    std::vector<std::wstring> adapters;
    DISPLAY_DEVICEW device{};
    device.cb = sizeof(device);
    for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &device, 0); i++, device.cb = sizeof(device)) {
        std::wstring name = Trim(device.DeviceString);
        bool seen = name.empty();
        for (const auto& adapter : adapters) {
            seen = seen || adapter == name;
        }
        if (seen) {
            continue;
        }
        adapters.push_back(name);
        // "PCI\VEN_10DE&DEV_2805..." or "USB\VID_17E9&PID_..." (DisplayLink).
        std::wstring id = Lower(device.DeviceID);
        size_t ven = id.find(L"ven_"), dev = id.find(L"dev_");
        size_t vid = id.find(L"vid_"), pid = id.find(L"pid_");
        if (ven != std::wstring::npos && dev != std::wstring::npos) {
            out.push_back(L"pci: [" + id.substr(ven + 4, 4) + L":" + id.substr(dev + 4, 4) +
                          L"] VGA compatible controller: " + name);
        } else if (vid != std::wstring::npos && pid != std::wstring::npos) {
            out.push_back(L"usb: [" + id.substr(vid + 4, 4) + L":" + id.substr(pid + 4, 4) +
                          L"] " + name);
        } else {
            out.push_back(L"drm: Initialized " + name);
        }
    }
    int fb = 0;
    for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &device, 0); i++, device.cb = sizeof(device)) {
        if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
            continue;
        }
        DEVMODEW mode{};
        mode.dmSize = sizeof(mode);
        if (!EnumDisplaySettingsW(device.DeviceName, ENUM_CURRENT_SETTINGS, &mode)) {
            continue;
        }
        swprintf(buffer, ARRAYSIZE(buffer), L"fb%d: %ls frame buffer device, %lux%lu @ %lu Hz, %lu bpp%ls",
                 fb++, Trim(device.DeviceString).c_str(), mode.dmPelsWidth, mode.dmPelsHeight,
                 mode.dmDisplayFrequency, mode.dmBitsPerPel,
                 (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? L" (primary)" : L"");
        out.push_back(buffer);
    }

    // Mounted fixed volumes (removable and network drives are skipped so a
    // slow device can never hold this up).
    WCHAR drives[256] = {};
    DWORD length = GetLogicalDriveStringsW(ARRAYSIZE(drives) - 1, drives);
    for (WCHAR* root = drives; length && *root; root += wcslen(root) + 1) {
        if (GetDriveTypeW(root) != DRIVE_FIXED) {
            continue;
        }
        WCHAR label[MAX_PATH + 1] = {}, fileSystem[32] = {};
        DWORD serial = 0;
        ULARGE_INTEGER available, total, free;
        if (!GetVolumeInformationW(root, label, ARRAYSIZE(label), &serial, nullptr, nullptr,
                                   fileSystem, ARRAYSIZE(fileSystem)) ||
            !GetDiskFreeSpaceExW(root, &available, &total, &free)) {
            continue;
        }
        swprintf(buffer, ARRAYSIZE(buffer), L"%04X-%04X", serial >> 16, serial & 0xFFFF);
        out.push_back(std::wstring(fileSystem) + L"-fs (" + std::wstring(root, 2) +
                      L"): mounted filesystem " + buffer + L" r/w" +
                      (label[0] ? L", label \"" + std::wstring(label) + L"\"" : L"") + L", " +
                      HumanSize(total.QuadPart) + L" (" + HumanSize(free.QuadPart) + L" free)");
    }

    // Physical network adapters only (no virtual, filter or Bluetooth ones).
    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) == NO_ERROR && table) {
        for (ULONG i = 0; i < table->NumEntries; i++) {
            const MIB_IF_ROW2& row = table->Table[i];
            if (!row.InterfaceAndOperStatusFlags.HardwareInterface ||
                row.InterfaceAndOperStatusFlags.FilterInterface ||
                (row.Type != IF_TYPE_ETHERNET_CSMACD && row.Type != IF_TYPE_IEEE80211) ||
                ContainsI(row.Description, L"bluetooth")) {
                continue;
            }
            std::wstring name = std::wstring(row.Alias) + L": " + row.Description;
            if (row.OperStatus == IfOperStatusUp) {
                info.networkUp = true;
                ULONG64 bps = row.TransmitLinkSpeed;
                std::wstring speed = bps >= 1000000000ULL && bps % 1000000000ULL == 0
                                         ? std::to_wstring(bps / 1000000000ULL) + L" Gbps"
                                         : std::to_wstring(bps / 1000000ULL) + L" Mbps";
                out.push_back(name + L": NIC Link is Up " + speed);
            } else {
                out.push_back(name + L": NIC Link is Down");
            }
        }
        FreeMibTable(table);
    }
    return info;
}

// "Wed Nov 25 19:33:49 2026", like ctime().
std::wstring FormatLastLogin(const std::wstring& logonId) {
    if (logonId.empty() || !iswdigit(logonId[0])) {
        return L"";
    }
    ULONGLONG value = wcstoull(logonId.c_str(), nullptr, 10);
    FILETIME utc{(DWORD)value, (DWORD)(value >> 32)};
    SYSTEMTIME universal, local;
    if (!value || !FileTimeToSystemTime(&utc, &universal) ||
        !SystemTimeToTzSpecificLocalTime(nullptr, &universal, &local)) {
        return L"";
    }
    static const wchar_t* kDays[] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
    static const wchar_t* kMonths[] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
                                       L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
    wchar_t buffer[64];
    swprintf(buffer, 64, L"%ls %ls %2u %02u:%02u:%02u %u", kDays[local.wDayOfWeek % 7],
             kMonths[(local.wMonth + 11) % 12], local.wDay, local.wHour, local.wMinute,
             local.wSecond, local.wYear);
    return buffer;
}

std::wstring DescribeCheck(const ItemConfig& item) {
    std::wstring processes = JoinList(item.processes, L" or ");
    std::wstring out;
    switch (item.check) {
        case Check::Process:
            out = L"ready when " + processes + L" is running";
            break;
        case Check::Window:
            out = L"ready when " + processes + L" shows a window";
            break;
        case Check::WindowEveryMonitor:
            out = L"ready when " + processes + L" has a window on every display";
            break;
        case Check::Wallpaper:
            out = L"ready when " + processes + L" attaches a wallpaper to the desktop";
            break;
        case Check::Service:
            out = L"ready when service " + item.service + L" is running" +
                  (processes.empty() ? L"" : L" with " + processes);
            break;
        case Check::Command:
            out = L"ready when `" + Shorten(item.command, 90) + L"` succeeds" +
                  (Trim(item.commandOutput).empty()
                       ? L""
                       : L" and prints " + Trim(item.commandOutput));
            break;
    }
    if (item.check == Check::Window || item.check == Check::WindowEveryMonitor) {
        if (!item.windowTitle.empty()) {
            out += L" titled \"" + item.windowTitle + L"\"";
        }
        if (!item.windowClass.empty()) {
            out += L" of class " + item.windowClass;
        }
    }
    if (item.optional) {
        out += L" (optional)";
    }
    if (!item.launch.empty()) {
        out += L"; starts " + CleanPath(item.launch) + L" if needed";
    }
    return out;
}

struct MonitorInfo {
    RECT rc;
    bool primary;
    UINT dpi;
};

BOOL CALLBACK CollectMonitorInfoProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return TRUE;
    }
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    reinterpret_cast<std::vector<MonitorInfo>*>(param)->push_back(
        {mi.rcMonitor, (mi.dwFlags & MONITORINFOF_PRIMARY) != 0, dpiX});
    return TRUE;
}

class Session;

struct SplashWindow {
    Session* owner = nullptr;
    HWND hwnd = nullptr;
    RECT rc{};
    int width = 0, height = 0;
    UINT dpi = 96;
    bool primary = false;
    bool showCard = true;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    uint32_t* bits = nullptr;
    std::unique_ptr<gp::Bitmap> surface;
    std::vector<uint32_t> background;
    // The scene without indicators, for the content area only.
    std::vector<uint32_t> staticLayer;
    RECT staticRect{};
    Layout lastLayout;
    gp::RectF lastContent;
    bool hasLast = false;
    gp::RectF skipRect;
    bool hoverSkip = false;
    bool trackingMouse = false;
    bool presented = false;
    BYTE alpha = 0;
    std::wstring lastKey;
};

class Session {
   public:
    Session(Settings settings, bool preview)
        : s_(std::move(settings)), preview_(preview) {}

    ~Session() { Stop(); }

    bool Start() {
        WCHAR storage[MAX_PATH] = {};
        if (Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
            reportPath_ = std::wstring(storage) + L"\\latest-run.json";
        }
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stopEvent_) {
            return false;
        }
        start_ = GetTickCount64();
        QueryPerformanceFrequency(&qpcFreq_);
        QueryPerformanceCounter(&qpcStart_);
        uiThread_ = CreateThread(nullptr, 0, UiThreadProc, this, 0, &uiThreadId_);
        return uiThread_ != nullptr;
    }

    void Stop() {
        if (stopEvent_) {
            SetEvent(stopEvent_);
        }
        if (uiThread_) {
            PostThreadMessageW(uiThreadId_, WM_NULL, 0, 0);
            WaitForSingleObject(uiThread_, INFINITE);
            CloseHandle(uiThread_);
            uiThread_ = nullptr;
        }
        if (stopEvent_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
    }

    bool Finished() const { return uiDone_.load(); }

   private:
    static DWORD WINAPI UiThreadProc(LPVOID param) {
        static_cast<Session*>(param)->UiMain();
        return 0;
    }

    static DWORD WINAPI WorkerThreadProc(LPVOID param) {
        static_cast<Session*>(param)->WorkerMain();
        return 0;
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        SplashWindow* w = nullptr;
        if (msg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            w = static_cast<SplashWindow*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)w);
        } else {
            w = reinterpret_cast<SplashWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }
        if (w && w->owner) {
            LRESULT result = 0;
            if (w->owner->HandleMessage(*w, msg, wParam, lParam, &result)) {
                return result;
            }
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    bool HandleMessage(SplashWindow& w, UINT msg, WPARAM wParam, LPARAM lParam,
                       LRESULT* result) {
        switch (msg) {
            case WM_MOUSEMOVE: {
                if (!w.trackingMouse) {
                    TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, w.hwnd, 0};
                    w.trackingMouse = TrackMouseEvent(&tme) != FALSE;
                }
                gp::PointF pt((gp::REAL)(short)LOWORD(lParam), (gp::REAL)(short)HIWORD(lParam));
                w.hoverSkip = w.skipRect.Width > 0 && w.skipRect.Contains(pt);
                return true;
            }
            case WM_MOUSELEAVE:
                w.trackingMouse = false;
                w.hoverSkip = false;
                return true;
            case WM_LBUTTONDOWN:
                // The console has no button, so any click skips it.
                if (s_.allowSkip &&
                    (w.hoverSkip || s_.clickAnywhere || s_.style == Style::Console)) {
                    Finish(Outcome::Skipped);
                }
                return true;
            case WM_KEYDOWN:
                if (s_.allowSkip &&
                    (wParam == VK_ESCAPE || wParam == VK_RETURN || wParam == VK_SPACE)) {
                    Finish(Outcome::Skipped);
                }
                return true;
            case WM_SETCURSOR: {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(w.hwnd, &pt);
                bool overSkip = w.skipRect.Width > 0 &&
                                w.skipRect.Contains(gp::PointF((gp::REAL)pt.x, (gp::REAL)pt.y));
                SetCursor(LoadCursorW(nullptr, overSkip         ? IDC_HAND
                                               : s_.busyCursor && outcome_ == Outcome::None
                                                   ? IDC_APPSTARTING
                                                   : IDC_ARROW));
                *result = TRUE;
                return true;
            }
            case WM_MOUSEACTIVATE:
                *result = MA_ACTIVATE;
                return true;
            case WM_CLOSE:
                if (s_.allowSkip) {
                    Finish(Outcome::Skipped);
                }
                *result = 0;
                return true;
            case WM_DISPLAYCHANGE:
                if (!rebuildPending_) {
                    rebuildPending_ = true;
                    PostThreadMessageW(GetCurrentThreadId(), kMsgRebuild, 0, 0);
                }
                return true;
            case WM_ERASEBKGND:
                *result = 1;
                return true;
        }
        return false;
    }

    void Finish(Outcome outcome) {
        if (outcome_ != Outcome::None) {
            return;
        }
        outcome_ = outcome;
        finishAt_ = GetTickCount64();
        double seconds = (finishAt_ - start_) / 1000.0;
        switch (outcome) {
            case Outcome::Ready:
                Log(kLogOk, L"Reached target Graphical Interface.", L"Graphical Interface");
                Log(kLogInfo, StartupFinishedText(seconds));
                LogLoginPrompt();
                break;
            case Outcome::Timeout:
                for (size_t i = 0; i < s_.items.size() && i < lastStatus_.size(); i++) {
                    int state = lastStatus_[i].state;
                    if (!s_.items[i].optional && state != kReady && state != kUnavailable) {
                        Log(kLogTime, L"Timed out waiting for " + s_.items[i].name + L".",
                            s_.items[i].name);
                    }
                }
                Log(kLogDepend, L"Dependency failed for Graphical Interface.",
                    L"Graphical Interface");
                Log(kLogDetail, L"Opening the desktop anyway after " + FormatSeconds(seconds, 1) +
                                    L".");
                LogLoginPrompt();
                break;
            case Outcome::Skipped:
                Log(kLogSkip, L"Skipped after " + FormatSeconds(seconds, 1) +
                                  L"; apps keep starting in the background.");
                break;
            default:
                break;
        }
    }

    // Like systemd-analyze: "Startup finished in 1.8s (sign-in) + 2.5s (desktop) = 4.3s."
    std::wstring StartupFinishedText(double desktopSeconds) {
        std::wstring simple = L"Startup finished in " + FormatSeconds(desktopSeconds, 3) + L".";
        if (preview_) {
            return simple;
        }
        ULONGLONG logon = wcstoull(CurrentLogonId().c_str(), nullptr, 10);
        FILETIME created, exited, kernel, user;
        if (!logon || !GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
            return simple;
        }
        ULARGE_INTEGER explorer{{created.dwLowDateTime, created.dwHighDateTime}};
        if (explorer.QuadPart < logon || explorer.QuadPart - logon > 600ULL * 10000000ULL) {
            return simple;  // Not a fresh sign-in (Explorer restarted later).
        }
        double signIn = (explorer.QuadPart - logon) / 1e7;
        return L"Startup finished in " + FormatSeconds(signIn, 3) + L" (sign-in) + " +
               FormatSeconds(desktopSeconds, 3) + L" (desktop) = " +
               FormatSeconds(signIn + desktopSeconds, 3) + L".";
    }

    // agetty's autologin, then the desktop takes over like startx.
    void LogLoginPrompt() {
        if (s_.style != Style::Console || !s_.consoleLoginPrompt) {
            return;
        }
        WindowsVersion windows = GetWindowsVersion();
        Log(kLogBlank, L"");
        Log(kLogPlain, windows.product + L" " + windows.kernel + L" (tty1)");
        Log(kLogBlank, L"");
        Log(kLogPlain, res_.hostName + L" login: " + res_.accountName + L" (automatic login)");
        std::wstring last = FormatLastLogin(s_.previousLogon);
        if (!last.empty()) {
            Log(kLogPlain, L"Last login: " + last + L" on tty1");
        }
    }

    void Log(int kind, std::wstring text, std::wstring highlight = L"") {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        LogLine line;
        line.seconds = qpcFreq_.QuadPart
                           ? (double)(now.QuadPart - qpcStart_.QuadPart) / qpcFreq_.QuadPart
                           : 0;
        line.kind = kind;
        line.text = std::move(text);
        line.highlight = std::move(highlight);
        log_.push_back(std::move(line));
        if (log_.size() > 1500) {
            log_.erase(log_.begin(), log_.begin() + 500);
            revealed_ = revealed_ > 500 ? revealed_ - 500 : 0;
        }
        logRevision_++;
    }

    void LogHeader(const BootInfo& boot) {
        // Like dmesg, the log starts at zero with the kernel messages.
        QueryPerformanceCounter(&qpcStart_);
        for (const auto& line : boot.lines) {
            Log(kLogKernel, line);
        }
        WCHAR exe[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        std::wstring exePath = exe;
        std::wstring exeName = exePath.substr(exePath.find_last_of(L'\\') + 1);
        if (s_.consoleKernelLog) {
            if (s_.style == Style::Console && !windows_.empty()) {
                const SplashWindow* primary = windows_[0].get();
                for (const auto& w : windows_) {
                    primary = w->primary ? w.get() : primary;
                }
                Frame empty;
                Layout L = ComputeConsoleLayout(s_, res_, empty, primary->width, primary->height,
                                                primary->dpi);
                Log(kLogKernel, L"Console: switching to colour frame buffer device " +
                                    std::to_wstring(L.columns) + L"x" +
                                    std::to_wstring(L.maxLines));
            }
            Log(kLogKernel, L"Run " + exePath + L" as init process");
        }
        std::wstring self = L"desktop-ready[" + std::to_wstring(GetCurrentProcessId()) + L"]: ";
        static const wchar_t* kRendering[] = {L"+RETRO", L"+CRISP", L"+SMOOTH"};
        Log(kLogKernel, self + L"Desktop Ready " + WH_MOD_VERSION +
                            L" running in user mode. (+WINDHAWK +GDIPLUS +LAYERED +PMV2 " +
                            (s_.consoleVerbose ? L"+VERBOSE " : L"-VERBOSE ") +
                            kRendering[(int)s_.consoleRendering] + L")");
        SYSTEM_INFO system{};
        GetNativeSystemInfo(&system);
        Log(kLogKernel, self + L"Detected architecture " +
                            (system.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64   ? L"x86-64"
                             : system.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 ? L"arm64"
                                                                                             : L"x86") +
                            L".");
        Log(kLogKernel, self + L"Hostname set to <" + res_.hostName + L">.");
        std::vector<std::wstring> names;
        for (const auto& item : s_.items) {
            names.push_back(item.name);
        }
        Log(kLogKernel, self + L"Waiting for " + std::to_wstring(names.size()) +
                            (names.size() == 1 ? L" app" : L" apps") + L" (timeout " +
                            FormatSeconds(maxMs_ / 1000.0, 0) + L", settle " +
                            FormatSeconds(stableMs_ / 1000.0, 1) + L"): " + JoinList(names, L", "));
        if (s_.consoleWelcome) {
            Log(kLogBlank, L"");
            Log(kLogWelcome, GetWindowsVersion().product);
            Log(kLogBlank, L"");
        }
        if (preview_) {
            Log(kLogWarn, L"Preview: nothing is launched or checked; progress is simulated.");
        }
        Log(kLogOk, L"Reached target Local File Systems.", L"Local File Systems");
        if (boot.networkUp) {
            Log(kLogOk, L"Reached target Network.", L"Network");
        }
        DWORD session = 0;
        ProcessIdToSessionId(GetCurrentProcessId(), &session);
        std::wstring sessionName =
            L"Session " + std::to_wstring(session) + L" of User " + res_.accountName;
        Log(kLogOk, L"Started " + sessionName + L".", sessionName);
        Log(kLogOk, L"Started Desktop Shell (" + exeName + L").", L"Desktop Shell");
        if (s_.allowSkip) {
            Log(kLogDetail, L"Press Esc or click to skip to the desktop.");
        }
        if (s_.consoleVerbose) {
            for (const auto& item : s_.items) {
                Log(kLogDetail, item.name + L": " + DescribeCheck(item), item.name);
            }
        }
    }

    // Turns changes in the checks into systemd-style status lines.
    void LogProgress(const std::vector<ItemStatus>& status) {
        bool verbose = s_.consoleVerbose;
        if (!preview_ && !firstProbeLogged_ && firstProbeMs_.load() >= 0) {
            firstProbeLogged_ = true;
            if (verbose) {
                Log(kLogDetail, L"First check of all apps took " +
                                    std::to_wstring(firstProbeMs_.load()) + L" ms.");
            }
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (; launchesLogged_ < launches_.size(); launchesLogged_++) {
                const LaunchNote& note = launches_[launchesLogged_];
                switch (note.kind) {
                    case kLaunchStarted:
                        if (verbose) {
                            Log(kLogDetail, note.name + L": launched " + note.detail, note.name);
                        }
                        break;
                    case kLaunchAlreadyRunning:
                        if (verbose) {
                            Log(kLogDetail, note.name + L": already running; not launching it again.",
                                note.name);
                        }
                        break;
                    default:
                        Log(kLogFail, L"Failed to launch " + note.name + L".", note.name);
                        Log(kLogDetail,
                            note.name + (note.kind == kLaunchNotFound ? L": file not found: "
                                                                      : L": could not start ") +
                                note.detail,
                            note.name);
                }
            }
        }
        for (size_t i = 0; i < status.size() && i < loggedState_.size(); i++) {
            const ItemStatus& st = status[i];
            const std::wstring& name = s_.items[i].name;
            std::wstring detail = name + L": " + st.detail;
            int previous = loggedState_[i];
            if (previous != st.state) {
                if (st.state == kReady) {
                    Log(kLogOk, L"Started " + name + L".", name);
                    if (verbose) {
                        Log(kLogDetail, detail, name);
                    }
                } else if (st.state == kUnavailable) {
                    Log(kLogFail, L"Failed to start " + name + L".", name);
                    Log(kLogDetail, detail, name);
                } else if (previous == kReady) {
                    Log(kLogWarn, name + L" is no longer ready.", name);
                    Log(kLogDetail, detail, name);
                } else if (previous < 0) {
                    Log(kLogInfo, L"Starting " + name + L"...", name);
                    if (verbose) {
                        Log(kLogDetail, detail, name);
                    }
                } else if (verbose) {
                    Log(kLogDetail, detail, name);
                }
            } else if (verbose && st.detail != loggedDetail_[i] && st.state != kReady) {
                Log(kLogDetail, detail, name);
            }
            if (verbose && !st.trace.empty() && st.trace != loggedTrace_[i]) {
                Log(kLogDetail, name + L": " + st.trace, name);
            }
            loggedState_[i] = st.state;
            loggedDetail_[i] = st.detail;
            loggedTrace_[i] = st.trace;
        }
    }

    void WorkerMain() {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        // Launching first: anything started here should start as early as possible.
        for (const auto& item : s_.items) {
            if (preview_ || item.launch.empty() ||
                WaitForSingleObject(stopEvent_, 0) == WAIT_OBJECT_0) {
                continue;
            }
            LaunchNote note = LaunchProgram(item);
            std::lock_guard<std::mutex> lock(mutex_);
            launches_.push_back(std::move(note));
        }
        // Gathered here, off the UI thread, so the splash never waits on it.
        BootInfo boot;
        if (s_.consoleKernelLog) {
            boot = GatherBootInfo();
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            boot_ = std::move(boot);
        }
        bootReady_ = true;
        while (!preview_ && WaitForSingleObject(stopEvent_, 0) != WAIT_OBJECT_0) {
            LARGE_INTEGER t0, t1, frequency;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&t0);
            std::vector<ItemStatus> next = ProbeAll(s_.items, stopEvent_);
            QueryPerformanceCounter(&t1);
            if (firstProbeMs_.load() < 0) {
                firstProbeMs_ = (int)((t1.QuadPart - t0.QuadPart) * 1000 / frequency.QuadPart);
            }
            {
                std::lock_guard<std::mutex> lock(mutex_);
                status_ = std::move(next);
                probed_ = true;
            }
            if (WaitForSingleObject(stopEvent_, (DWORD)s_.pollMs) == WAIT_OBJECT_0) {
                break;
            }
        }
        if (SUCCEEDED(com)) {
            CoUninitialize();
        }
    }

    void UiMain() {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        gp::GdiplusStartupInput input;
        ULONG_PTR token = 0;
        if (gp::GdiplusStartup(&token, &input, nullptr) != gp::Ok) {
            uiDone_ = true;
            return;
        }
        HINSTANCE instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kWindowClass;
        bool registered = RegisterClassExW(&wc) != 0 ||
                          GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
        if (registered) {
            InitResources(&res_, s_);
            status_.assign(s_.items.size(), Status(kWaiting, L"Waiting to start"));
            prevStates_.assign(s_.items.size(), -1);
            readyAt_.assign(s_.items.size(), 0);
            loggedState_.assign(s_.items.size(), -1);
            loggedDetail_.assign(s_.items.size(), L"");
            loggedTrace_.assign(s_.items.size(), L"");
            // Previews are shortened so they don't outstay their welcome.
            maxMs_ = preview_ ? Smaller(s_.maxWaitMs, 20000) : s_.maxWaitMs;
            minMs_ = preview_ ? Larger(s_.minDisplayMs, 3000) : s_.minDisplayMs;
            stableMs_ = preview_ ? Smaller(s_.stableMs, 1200) : s_.stableMs;
            CreateWindows();
            // Previews start the worker too: it gathers the kernel messages.
            worker_ = CreateThread(nullptr, 0, WorkerThreadProc, this, 0, nullptr);
            UINT_PTR timer = SetTimer(nullptr, 0, kFrameMs, nullptr);
            bool done = windows_.empty() || Tick();
            bool stopRequested = false;
            while (!done) {
                DWORD wait = MsgWaitForMultipleObjectsEx(1, &stopEvent_, INFINITE,
                                                         QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (wait == WAIT_OBJECT_0) {
                    Finish(Outcome::Stopped);
                    stopRequested = true;
                    break;
                }
                MSG msg;
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT) {
                        done = true;
                        break;
                    }
                    if (!msg.hwnd && msg.message == WM_TIMER) {
                        if (Tick()) {
                            done = true;
                            break;
                        }
                        continue;
                    }
                    if (!msg.hwnd && msg.message == kMsgRebuild) {
                        rebuildPending_ = false;
                        DestroyWindows();
                        CreateWindows();
                        Log(kLogInfo, L"Display configuration changed; now covering " +
                                          std::to_wstring(windows_.size()) + L" display(s).");
                        continue;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            KillTimer(nullptr, timer);
            SetEvent(stopEvent_);
            if (worker_) {
                WaitForSingleObject(worker_, INFINITE);
                CloseHandle(worker_);
                worker_ = nullptr;
            }
            DestroyWindows();
            // When the mod is unloading, avoid calling back into Windhawk.
            if (!stopRequested) {
                WriteReport();
            }
            res_ = Resources();
            UnregisterClassW(kWindowClass, instance);
        }
        gp::GdiplusShutdown(token);
        uiDone_ = true;
    }

    void CreateWindows() {
        std::vector<MonitorInfo> monitors;
        EnumDisplayMonitors(nullptr, nullptr, CollectMonitorInfoProc, (LPARAM)&monitors);
        if (monitors.empty()) {
            monitors.push_back({{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)},
                                true,
                                96});
        }
        HINSTANCE instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        for (const auto& m : monitors) {
            if (s_.monitors == MonitorMode::PrimaryOnly && !m.primary) {
                continue;
            }
            auto w = std::make_unique<SplashWindow>();
            w->owner = this;
            w->rc = m.rc;
            w->width = m.rc.right - m.rc.left;
            w->height = m.rc.bottom - m.rc.top;
            w->dpi = m.dpi ? m.dpi : 96;
            w->primary = m.primary;
            w->showCard = s_.monitors != MonitorMode::CardOnPrimary || m.primary;
            if (w->width <= 0 || w->height <= 0) {
                continue;
            }
            BITMAPINFO bi{};
            bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
            bi.bmiHeader.biWidth = w->width;
            bi.bmiHeader.biHeight = -w->height;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            HDC screen = GetDC(nullptr);
            w->memDC = CreateCompatibleDC(screen);
            ReleaseDC(nullptr, screen);
            void* bits = nullptr;
            w->dib = CreateDIBSection(w->memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
            if (!w->memDC || !w->dib || !bits) {
                ReleaseWindowResources(*w);
                continue;
            }
            w->bits = static_cast<uint32_t*>(bits);
            w->oldBitmap = SelectObject(w->memDC, w->dib);
            RenderBackground(w->bits, w->width, w->height, w->dpi, s_, res_, preview_);
            w->background.assign(w->bits, w->bits + (size_t)w->width * w->height);
            w->surface = std::make_unique<gp::Bitmap>(w->width, w->height, w->width * 4,
                                                      PixelFormat32bppPARGB, (BYTE*)w->bits);
            w->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                                      kWindowClass, L"Desktop Ready", WS_POPUP, m.rc.left,
                                      m.rc.top, w->width, w->height, nullptr, nullptr,
                                      instance, w.get());
            if (!w->hwnd) {
                ReleaseWindowResources(*w);
                continue;
            }
            windows_.push_back(std::move(w));
        }
        displayCount_ = windows_.size();
    }

    void ReleaseWindowResources(SplashWindow& w) {
        w.surface.reset();
        if (w.memDC && w.oldBitmap) {
            SelectObject(w.memDC, w.oldBitmap);
        }
        if (w.dib) {
            DeleteObject(w.dib);
        }
        if (w.memDC) {
            DeleteDC(w.memDC);
        }
        w.memDC = nullptr;
        w.dib = nullptr;
        w.bits = nullptr;
    }

    void DestroyWindows() {
        for (auto& w : windows_) {
            if (w->hwnd) {
                SetWindowLongPtrW(w->hwnd, GWLP_USERDATA, 0);
                DestroyWindow(w->hwnd);
                w->hwnd = nullptr;
            }
            ReleaseWindowResources(*w);
        }
        windows_.clear();
    }

    // Returns true when the splash is finished.
    bool Tick() {
        ULONGLONG now = GetTickCount64();
        double elapsed = (double)(now - start_);
        size_t n = s_.items.size();

        std::vector<ItemStatus> status;
        if (preview_) {
            status = SimulateStatus(s_.items, elapsed);
        } else {
            std::lock_guard<std::mutex> lock(mutex_);
            status = status_;
        }
        status.resize(n);

        // The kernel messages come first, once the worker has gathered them.
        if (!headerLogged_ && (bootReady_.load() || elapsed > 3000)) {
            BootInfo boot;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                boot = boot_;
            }
            LogHeader(boot);
            headerLogged_ = true;
        }
        bool haveData = (probed_ || preview_) && headerLogged_;
        if (haveData) {
            LogProgress(status);
        }
        lastStatus_ = status;

        int readyCount = 0;
        bool allRequired = haveData;
        for (size_t i = 0; i < n; i++) {
            int state = status[i].state;
            if (state == kReady) {
                readyCount++;
                if (prevStates_[i] != kReady) {
                    readyAt_[i] = now;
                }
            }
            prevStates_[i] = state;
            if (!s_.items[i].optional && state != kReady && state != kUnavailable) {
                allRequired = false;
            }
        }

        int maxMs = maxMs_;
        int minMs = minMs_;
        int stableMs = stableMs_;
        if (outcome_ == Outcome::None) {
            if (allRequired) {
                if (!readySince_) {
                    readySince_ = now;
                }
            } else {
                readySince_ = 0;
            }
            if (allRequired && !settleLogged_) {
                settleLogged_ = true;
                if (!firstAllReady_) {
                    firstAllReady_ = now;
                }
                Log(kLogOk, L"Reached target Startup Applications.", L"Startup Applications");
                if (s_.consoleVerbose && stableMs > 0) {
                    Log(kLogDetail, L"Letting the desktop settle for " +
                                        FormatSeconds(stableMs / 1000.0, 1) + L".");
                }
            } else if (!allRequired && settleLogged_) {
                settleLogged_ = false;
                Log(kLogWarn, L"Stopped target Startup Applications; waiting again.",
                    L"Startup Applications");
            }
            if (elapsed >= maxMs) {
                Finish(Outcome::Timeout);
            } else if (allRequired && elapsed >= minMs &&
                       now - readySince_ >= (ULONGLONG)stableMs) {
                Finish(Outcome::Ready);
            }
        }

        float fadeIn = s_.fadeInMs > 0 ? Clamp((float)(elapsed / s_.fadeInMs), 0.f, 1.f) : 1.f;
        float fadeOut = 0;
        if (outcome_ != Outcome::None) {
            int hold = outcome_ == Outcome::Ready     ? s_.readyHoldMs
                       : outcome_ == Outcome::Timeout ? 900
                                                      : 0;
            if (hold && s_.style == Style::Console && s_.consoleLoginPrompt) {
                hold = Larger(hold, 1600);  // Long enough to see the login prompt.
            }
            double sinceFinish = (double)(now - finishAt_);
            if (sinceFinish >= hold + s_.fadeOutMs) {
                return true;
            }
            fadeOut = s_.fadeOutMs > 0
                          ? Clamp((float)((sinceFinish - hold) / s_.fadeOutMs), 0.f, 1.f)
                          : (sinceFinish >= hold ? 1.f : 0.f);
        }
        float alpha = fadeIn * (1.f - fadeOut);

        Frame f;
        f.status = &status;
        f.preview = preview_;
        f.outcome = outcome_;
        f.readyCount = readyCount;
        f.spin = (float)(now % 1100) / 1100.f;
        f.slide = s_.reduceMotion ? 0.f
                                  : (1.f - EaseOut(fadeIn)) * 18.f - EaseOut(fadeOut) * 12.f;
        f.secondsLeft = outcome_ == Outcome::None
                            ? (int)std::ceil(Larger(0.0, maxMs - elapsed) / 1000.0)
                            : -1;
        float target = n ? (float)readyCount / n : (float)(elapsed / maxMs);
        if (outcome_ == Outcome::Ready) {
            target = 1.f;
        }
        shownProgress_ = s_.reduceMotion ? target
                                         : shownProgress_ + (target - shownProgress_) * 0.2f;
        f.progress = shownProgress_;
        bool indicatorsAnimating = false;
        for (size_t i = 0; i < n; i++) {
            float a = 1.f;
            if (status[i].state == kReady && readyAt_[i] && !s_.reduceMotion) {
                a = Clamp((float)(now - readyAt_[i]) / 520.f, 0.f, 1.f);
            }
            if (a < 1.f || (status[i].state == kStarting && !s_.reduceMotion)) {
                indicatorsAnimating = true;
            }
            f.readyAnim.push_back(a);
        }

        bool console = s_.style == Style::Console;
        // New lines scroll in a few per frame, like a fast boot log.
        if (!console || s_.reduceMotion) {
            revealed_ = log_.size();
        } else if (revealed_ < log_.size()) {
            revealed_ = Smaller(log_.size(), revealed_ + 3);
        }
        f.log = &log_;
        f.logCount = revealed_;
        f.cursorOn = (now / 500) % 2 == 0;
        // systemd only shows a start job once it has been running a while.
        if (console && s_.consoleStatusLine && outcome_ == Outcome::None && elapsed >= 2000 &&
            revealed_ == log_.size()) {
            std::vector<size_t> pending;
            for (size_t i = 0; i < n; i++) {
                if (!s_.items[i].optional && status[i].state != kReady &&
                    status[i].state != kUnavailable) {
                    pending.push_back(i);
                }
            }
            if (!pending.empty()) {
                // Like systemd, rotate through the jobs still running.
                f.jobName = s_.items[pending[(size_t)(now / 2000) % pending.size()]].name;
                f.jobText = L"A start job is running for " + f.jobName + L" (" +
                            FormatTimespan((int)(elapsed / 1000)) + L" / " +
                            FormatTimespan(maxMs / 1000) + L")";
            }
            int cycle = (int)((now / 110) % 14);
            f.cylon = s_.reduceMotion ? 2 : (cycle <= 7 ? cycle - 1 : 13 - cycle);
        }
        if (console) {
            indicatorsAnimating = true;  // The cursor blinks.
        }

        // Everything except the indicators. Progress easing and fades change
        // it every frame while they run; otherwise it rarely changes.
        std::wstring key;
        if (console) {
            key = std::to_wstring((int)outcome_) + L"|" + std::to_wstring(logRevision_) + L"|" +
                  std::to_wstring(revealed_);
        } else {
            key = std::to_wstring((int)outcome_) + L"|" + std::to_wstring(f.secondsLeft) + L"|" +
                  std::to_wstring((int)(f.progress * 1000)) + L"|" +
                  std::to_wstring((int)(f.slide * 10)) + L"|" + TimeText();
            for (const auto& st : status) {
                key += L"|" + std::to_wstring(st.state) + st.detail;
            }
        }

        BYTE alphaByte = (BYTE)Clamp((int)std::lround(alpha * 255.f), 0, 255);
        for (auto& w : windows_) {
            f.hoverSkip = w->hoverSkip;
            std::wstring windowKey = key + (w->hoverSkip ? L"|h" : L"|-");
            if (!w->presented || windowKey != w->lastKey) {
                Render(*w, f, alphaByte, true);
                w->lastKey = windowKey;
            } else if (indicatorsAnimating && w->showCard) {
                Render(*w, f, alphaByte, false);
            } else if (alphaByte != w->alpha) {
                BLENDFUNCTION blend{AC_SRC_OVER, 0, alphaByte, AC_SRC_ALPHA};
                UpdateLayeredWindow(w->hwnd, nullptr, nullptr, nullptr, nullptr, nullptr,
                                    0, &blend, ULW_ALPHA);
                w->alpha = alphaByte;
            }
        }

        if (s_.keepOnTop && outcome_ == Outcome::None && now - lastTopmost_ >= 1000) {
            lastTopmost_ = now;
            for (auto& w : windows_) {
                SetWindowPos(w->hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
            }
        }
        return false;
    }

    RECT ClampRect(const SplashWindow& w, const gp::RectF& r) {
        return RECT{Clamp((int)std::floor(r.X), 0, w.width),
                    Clamp((int)std::floor(r.Y), 0, w.height),
                    Clamp((int)std::ceil(r.X + r.Width), 0, w.width),
                    Clamp((int)std::ceil(r.Y + r.Height), 0, w.height)};
    }

    void DrawIndicatorsInto(SplashWindow& w, const Frame& f, const Layout& layout,
                            const RECT& clip) {
        if ((layout.indicators.empty() && !layout.console) || clip.right <= clip.left ||
            clip.bottom <= clip.top) {
            return;
        }
        gp::Graphics g(w.surface.get());
        PrepareGraphics(g);
        g.SetClip(gp::Rect(clip.left, clip.top, clip.right - clip.left, clip.bottom - clip.top));
        DrawIndicators(g, s_, res_, f, layout);
        g.Flush(gp::FlushIntentionSync);
    }

    // full: redraw the whole scene; otherwise only the indicators, restored
    // from the cached static layer.
    void Render(SplashWindow& w, const Frame& f, BYTE alpha, bool full) {
        RECT dirtyRect{};
        if (!full && w.presented && !w.staticLayer.empty()) {
            const Layout& layout = w.lastLayout;
            RECT r = ClampRect(w, IndicatorBounds(layout));
            const RECT& sr = w.staticRect;
            IntersectRect(&r, &r, &sr);
            if (IsRectEmpty(&r)) {
                return;
            }
            int sw = sr.right - sr.left;
            for (int y = r.top; y < r.bottom; y++) {
                memcpy(w.bits + (size_t)y * w.width + r.left,
                       w.staticLayer.data() + (size_t)(y - sr.top) * sw + (r.left - sr.left),
                       (r.right - r.left) * 4);
            }
            DrawIndicatorsInto(w, f, layout, r);
            dirtyRect = r;
        } else {
            Layout layout = ComputeLayout(s_, res_, f, w.width, w.height, w.dpi, w.showCard);
            gp::RectF dirty = layout.content;
            if (w.hasLast) {
                gp::RectF current = layout.content;
                gp::RectF::Union(dirty, current, w.lastContent);
            }
            dirtyRect = w.presented ? ClampRect(w, dirty) : RECT{0, 0, w.width, w.height};
            if (!IsRectEmpty(&dirtyRect)) {
                for (int y = dirtyRect.top; y < dirtyRect.bottom; y++) {
                    size_t offset = (size_t)y * w.width + dirtyRect.left;
                    memcpy(w.bits + offset, w.background.data() + offset,
                           (dirtyRect.right - dirtyRect.left) * 4);
                }
                gp::Graphics g(w.surface.get());
                PrepareGraphics(g);
                g.SetClip(gp::Rect(dirtyRect.left, dirtyRect.top,
                                   dirtyRect.right - dirtyRect.left,
                                   dirtyRect.bottom - dirtyRect.top));
                DrawScene(g, s_, res_, f, layout, w.showCard);
                g.Flush(gp::FlushIntentionSync);
            }
            // Keep the indicator-free scene so animation frames can redraw
            // just the indicators.
            RECT sr = ClampRect(w, layout.content);
            w.staticRect = sr;
            int sw = sr.right - sr.left, sh = sr.bottom - sr.top;
            if (sw > 0 && sh > 0) {
                w.staticLayer.resize((size_t)sw * sh);
                for (int y = 0; y < sh; y++) {
                    memcpy(w.staticLayer.data() + (size_t)y * sw,
                           w.bits + (size_t)(sr.top + y) * w.width + sr.left, sw * 4);
                }
            } else {
                w.staticLayer.clear();
            }
            DrawIndicatorsInto(w, f, layout, dirtyRect);
            w.lastContent = layout.content;
            w.hasLast = true;
            w.skipRect = w.showCard ? layout.skip : gp::RectF();
            w.lastLayout = std::move(layout);
        }

        POINT dst{w.rc.left, w.rc.top};
        POINT src{0, 0};
        SIZE size{w.width, w.height};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UPDATELAYEREDWINDOWINFO info{};
        info.cbSize = sizeof(info);
        info.pptDst = &dst;
        info.psize = &size;
        info.hdcSrc = w.memDC;
        info.pptSrc = &src;
        info.pblend = &blend;
        info.dwFlags = ULW_ALPHA;
        info.prcDirty = w.presented ? &dirtyRect : nullptr;
        if (!UpdateLayeredWindowIndirect(w.hwnd, &info)) {
            info.prcDirty = nullptr;
            UpdateLayeredWindowIndirect(w.hwnd, &info);
        }
        w.alpha = alpha;
        if (!w.presented) {
            w.presented = true;
            if (w.primary && s_.allowSkip) {
                ShowWindow(w.hwnd, SW_SHOW);
                SetForegroundWindow(w.hwnd);
            } else {
                ShowWindow(w.hwnd, SW_SHOWNOACTIVATE);
            }
        }
    }

    void WriteReport() {
        const wchar_t* outcome = outcome_ == Outcome::Ready     ? L"ready"
                                 : outcome_ == Outcome::Timeout ? L"timeout"
                                 : outcome_ == Outcome::Skipped ? L"skipped"
                                                                : L"stopped";
        double seconds = (GetTickCount64() - start_) / 1000.0;
        std::vector<ItemStatus> status;
        std::vector<LaunchNote> launches;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            status = status_;
            launches = launches_;
        }
        Wh_Log(L"Splash finished: %s after %.2fs (%s)", outcome, seconds,
               preview_ ? L"preview" : L"sign-in");
        std::wstring logText;
        for (const auto& line : log_) {
            std::wstring text = FormatLogLine(line);
            if (!preview_ && !text.empty()) {
                Wh_Log(L"%s", text.c_str());
            }
            logText += text + L"\r\n";
        }
        if (preview_ || reportPath_.empty()) {
            return;
        }
        WriteUtf8File(reportPath_.substr(0, reportPath_.size() - 5) + L".log", logText);
        std::wstring json = L"{\n  \"result\": \"" + std::wstring(outcome) + L"\",\n";
        wchar_t buffer[64];
        swprintf(buffer, 64, L"%.2f", seconds);
        json += L"  \"durationSeconds\": " + std::wstring(buffer) + L",\n";
        json += L"  \"displays\": " + std::to_wstring(displayCount_) + L",\n";
        SYSTEMTIME utc;
        GetSystemTime(&utc);
        swprintf(buffer, 64, L"%04u-%02u-%02uT%02u:%02u:%02uZ", utc.wYear, utc.wMonth,
                 utc.wDay, utc.wHour, utc.wMinute, utc.wSecond);
        json += L"  \"finishedUtc\": \"" + std::wstring(buffer) + L"\",\n  \"apps\": [";
        static const wchar_t* kStates[] = {L"waiting", L"starting", L"ready", L"unavailable"};
        for (size_t i = 0; i < s_.items.size(); i++) {
            const ItemStatus st = i < status.size() ? status[i] : ItemStatus();
            json += (i ? L",\n    " : L"\n    ");
            json += L"{\"name\": \"" + JsonEscape(s_.items[i].name) + L"\", \"state\": \"" +
                    kStates[Clamp(st.state, 0, 3)] + L"\", \"detail\": \"" +
                    JsonEscape(st.detail) + L"\", \"trace\": \"" + JsonEscape(st.trace) + L"\"}";
        }
        json += L"\n  ],\n  \"launches\": [";
        static const wchar_t* kLaunchText[] = {L"started", L"already running", L"file not found",
                                               L"failed"};
        for (size_t i = 0; i < launches.size(); i++) {
            const LaunchNote& note = launches[i];
            json += (i ? L", \"" : L"\"") +
                    JsonEscape(note.name + L": " + kLaunchText[Clamp(note.kind, 0, 3)] + L" (" +
                               note.detail + L")") +
                    L"\"";
        }
        json += L"]\n}\n";
        WriteUtf8File(reportPath_, json);
    }

    Settings s_;
    bool preview_;
    std::wstring reportPath_;
    HANDLE stopEvent_ = nullptr;
    HANDLE uiThread_ = nullptr;
    HANDLE worker_ = nullptr;
    DWORD uiThreadId_ = 0;
    std::atomic<bool> uiDone_{false};
    ULONGLONG start_ = 0;

    std::mutex mutex_;  // Guards status_ and launches_.
    std::vector<ItemStatus> status_;
    std::atomic<bool> probed_{false};
    std::atomic<int> firstProbeMs_{-1};
    std::vector<LaunchNote> launches_;
    BootInfo boot_;
    std::atomic<bool> bootReady_{false};
    LARGE_INTEGER qpcStart_{};
    LARGE_INTEGER qpcFreq_{};

    // UI thread only.
    Resources res_;
    std::vector<std::unique_ptr<SplashWindow>> windows_;
    Outcome outcome_ = Outcome::None;
    ULONGLONG finishAt_ = 0;
    ULONGLONG readySince_ = 0;
    ULONGLONG lastTopmost_ = 0;
    std::vector<int> prevStates_;
    std::vector<ULONGLONG> readyAt_;
    float shownProgress_ = 0;
    bool rebuildPending_ = false;
    size_t displayCount_ = 0;
    int maxMs_ = 30000;
    int minMs_ = 2500;
    int stableMs_ = 2000;
    // Boot log.
    std::vector<LogLine> log_;
    uint64_t logRevision_ = 0;
    size_t revealed_ = 0;
    bool headerLogged_ = false;
    std::vector<int> loggedState_;
    std::vector<std::wstring> loggedDetail_;
    std::vector<std::wstring> loggedTrace_;
    size_t launchesLogged_ = 0;
    bool firstProbeLogged_ = false;
    bool settleLogged_ = false;
    ULONGLONG firstAllReady_ = 0;
    std::vector<ItemStatus> lastStatus_;
};

////////////////////////////////////////////////////////////////////////////////
// When to show

std::mutex g_sessionMutex;
std::unique_ptr<Session> g_session;
bool g_isShellProcess = false;

ULONGLONG ProcessAgeMs() {
    FILETIME created, exited, kernel, user, now;
    if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
        return 0;
    }
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER a{{created.dwLowDateTime, created.dwHighDateTime}};
    ULARGE_INTEGER b{{now.dwLowDateTime, now.dwHighDateTime}};
    return b.QuadPart > a.QuadPart ? (b.QuadPart - a.QuadPart) / 10000 : 0;
}

bool IsShellWindowOurs() {
    HWND shell = GetShellWindow();
    if (!shell) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(shell, &pid);
    return pid == GetCurrentProcessId();
}

// Explorer also runs as separate processes for folder windows; only the
// desktop shell process should show the splash.
bool LooksLikeShellProcess() {
    DWORD session = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &session) || session == 0) {
        return false;
    }
    std::wstring cmd = Lower(GetCommandLineW());
    if (cmd.find(L"/factory") != std::wstring::npos ||
        cmd.find(L"-embedding") != std::wstring::npos ||
        cmd.find(L"/separate") != std::wstring::npos) {
        return false;
    }
    HWND shell = GetShellWindow();
    return !shell || IsShellWindowOurs();
}

std::wstring CurrentLogonId() {
    WTSINFOW* info = nullptr;
    DWORD bytes = 0;
    std::wstring id;
    if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION,
                                    WTSSessionInfo, (LPWSTR*)&info, &bytes) &&
        info) {
        if (info->LogonTime.QuadPart) {
            id = std::to_wstring(info->LogonTime.QuadPart);
        }
        WTSFreeMemory(info);
    }
    if (id.empty()) {
        // Fall back to the boot time, rounded to a minute.
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        ULARGE_INTEGER t{{now.dwLowDateTime, now.dwHighDateTime}};
        ULONGLONG boot = t.QuadPart / 10000 - GetTickCount64();
        id = L"boot-" + std::to_wstring(boot / 60000);
    }
    return id;
}

void StartSessionLocked(Settings settings, bool preview) {
    g_session.reset();
    auto session = std::make_unique<Session>(std::move(settings), preview);
    if (session->Start()) {
        g_session = std::move(session);
    }
}

}  // namespace

BOOL Wh_ModInit() {
    g_isShellProcess = LooksLikeShellProcess();
    Wh_Log(L"Init (shell process: %d)", g_isShellProcess);
    // Folder-window Explorer processes don't need the mod at all.
    return g_isShellProcess ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (!g_isShellProcess) {
        return;
    }
    Settings settings = LoadSettings();
    bool freshExplorer = ProcessAgeMs() < 60000;
    std::lock_guard<std::mutex> lock(g_sessionMutex);
    if (freshExplorer) {
        if (settings.showOn == ShowOn::PreviewOnly) {
            return;
        }
        std::wstring logon = CurrentLogonId();
        WCHAR stored[64] = {};
        Wh_GetStringValue(L"lastLogon", stored, ARRAYSIZE(stored));
        if (settings.showOn == ShowOn::SignIn && logon == stored) {
            Wh_Log(L"Already shown for this sign-in (Explorer restarted)");
            return;
        }
        settings.previousLogon = stored;  // For the console's "Last login:".
        Wh_SetStringValue(L"lastLogon", logon.c_str());
        Wh_Log(L"Showing the sign-in splash");
        StartSessionLocked(std::move(settings), false);
    } else if (settings.previewOnSave && !Wh_GetIntValue(L"introShown", 0)) {
        // Enabled mid-session for the first time: show what it looks like.
        Wh_SetIntValue(L"introShown", 1);
        settings.previousLogon = CurrentLogonId();
        StartSessionLocked(std::move(settings), true);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isShellProcess || !IsShellWindowOurs()) {
        return;
    }
    Settings settings = LoadSettings();
    settings.previousLogon = CurrentLogonId();  // Previews: this sign-in.
    std::lock_guard<std::mutex> lock(g_sessionMutex);
    if (settings.previewOnSave) {
        StartSessionLocked(std::move(settings), true);
    }
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_sessionMutex);
    g_session.reset();
}
