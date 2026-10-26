// ==WindhawkMod==
// @id              on-screen-indicator-position
// @name            On-Screen Indicator Position
// @description     Put the volume, brightness and camera on-screen indicators anywhere on the screen, each in its own spot if you like, and optionally skip the slide out animation
// @version         1.5.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshcore
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/mario0318/on-screen-indicator-position/issues

// ==WindhawkModReadme==
/*
# On-Screen Indicator Position

Windows 11 shows an on-screen indicator when you change the volume or
brightness, toggle airplane mode, or when the camera or microphone privacy state
changes. Under **Settings > System > Notifications > On-screen indicators**,
Windows lets you put it in one of three places: top left, top center, or bottom
center.

This mod replaces that with a full nine-point grid, any corner, any edge center,
or dead center, plus a pixel offset for fine-tuning. Each kind of indicator can
also be given a spot of its own, so the volume one can sit somewhere different
from the brightness one.

The brightness indicator moved to the middle of the right edge:

![Indicator at middle right](https://raw.githubusercontent.com/mario0318/windhawk-mods/3685cdf56c55ba8cb3398b1cf9e35b5e95e68eb1/on-screen-indicator-position/middle-right.jpg)

## Positions

```
 Top left        Top center        Top right
 Middle left     Center            Middle right
 Bottom left     Bottom center     Bottom right
```

The indicator is kept inside the area Windows lays it out in, so an offset that
would push it past an edge stops at the edge instead of moving off screen. You
can also leave the position on **Windows default** and use the offsets alone to
nudge one of the built-in positions.

## A different spot per indicator

Volume, brightness, keyboard brightness, airplane mode, camera, microphone, the
virtual desktop name popup and the other text indicators can each be given their own
position. Anything left on **Same as the main position** follows the setting above,
so you only have to touch the ones you want somewhere else. Handy if you want the
volume indicator out of the way at the bottom but still want the camera one where
you will notice it.

If you used **Other indicators** to place the "Desktop N" popup before 1.4.0,
set **Virtual desktop name** to that spot after updating. It now has its own setting
and otherwise follows the main position.

Volume kept at the top left while brightness sits in the middle. Only one of them is
ever on screen at a time, so this is the same desktop photographed twice:

![Volume top left, brightness center](https://raw.githubusercontent.com/mario0318/windhawk-mods/628f80317652209d3feed54eadf9c329e77b04a7/on-screen-indicator-position/per-indicator.jpg)

## Skip the slide out animation

The indicator normally slides off screen when it's done. With **Skip the slide out
animation** on, it just disappears. Windows already has a no-animation hide path
and the mod asks for that one instead, so the slide in and everything outside the
indicator are left alone. If the setting ever appears to do nothing, the mod's log
says so on a build where the entry points have moved.

## Independent offsets and display profiles

Each indicator can use the main horizontal and vertical offsets, or its own
values, including zero. Each one can also stay at the Windows default position
while the others move.

Display profiles change the placement on the screen Windows already chose. A
profile can match part of the monitor's name, the number of monitors in use, and
one kind of indicator. The first enabled profile that matches wins, and anything
it leaves on "Keep" uses the indicator settings. For example, use a different
position when a dock is connected.

**Seconds on screen** changes the indicator's one-shot timeout from 1 to 60
seconds. Leave it at 0 to keep the Windows timeout. It does not change animations.

## Choosing a monitor

This mod only changes where the indicator sits on a screen, not which screen it
appears on. For that, use [Volume control open
location](https://windhawk.net/mods/volume-control-open-location), which selects
a monitor by number or by interface name. The two work together.

## Notes

* The slide-in animation direction is chosen by Windows from the built-in
  setting, not by this mod. If the animation looks wrong for your new position,
  change the built-in setting to whichever of the three has the animation you
  like, then let this mod do the actual placement.
* Offsets are given at 100% scaling and scaled to whichever monitor the
  indicator appears on, so the same value moves the same distance on a display
  running at 150%.
* With two indicators set to different spots you can catch the previous one
  flashing at the new spot for a frame before the new one draws. That's the
  confirmator reusing its frame, the placement hook can't do anything about it.
  Skipping the slide out makes it easier to catch rather than harder, since what
  lands at the new spot is the tail end of the previous indicator instead of an
  empty frame.
* Tested on Windows 11 build 26200 (25H2) x64, on a 100% and a 150% display.
  Build 26300 uses the same confirmator, so it should behave the same.
  The ARM64 hooks were checked against the disassembly of the confirmator DLL
  but not run on hardware, so if you use it there please let me know how it
  goes.

## Credits

The hook onto the indicator's own placement function comes from [Volume control
open location](https://windhawk.net/mods/volume-control-open-location) and
[Taskbar primary on secondary
monitor](https://windhawk.net/mods/taskbar-primary-on-secondary-monitor), which
both target the same function and work out the origin handling.

## Issues

Bugs and feature requests go on the [issues
page](https://github.com/mario0318/on-screen-indicator-position/issues).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: topRight
  $name: Position
  $description: >-
    Where on the screen the indicator appears. Anything left on "Same as the main
    position" below follows this one.
  $options:
  - windowsDefault: Windows default (only apply the offsets)
  - topLeft: Top left
  - topCenter: Top center
  - topRight: Top right
  - middleLeft: Middle left
  - center: Center
  - middleRight: Middle right
  - bottomLeft: Bottom left
  - bottomCenter: Bottom center
  - bottomRight: Bottom right
- offsetX: 0
  $name: Horizontal offset
  $description: >-
    Nudge the indicator sideways. A positive number moves it right, a negative one
    moves it left. It stops at the edge of the screen rather than going off it, and
    the same number moves it the same distance on any display.
- offsetY: 0
  $name: Vertical offset
  $description: >-
    Nudge the indicator up or down. A positive number moves it down, a negative one
    moves it up. It stops at the edge of the screen rather than going off it, and
    the same number moves it the same distance on any display.
- skipHideAnimation: false
  $name: Skip the slide out animation
  $description: The indicator disappears instead of sliding away.
- perIndicator:
  - volume: same
    $name: Volume
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - brightness: same
    $name: Brightness
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - keyboardBrightness: same
    $name: Keyboard brightness
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - airplaneMode: same
    $name: Airplane mode
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - camera: same
    $name: Camera
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - microphone: same
    $name: Microphone
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - text: same
    $name: Other indicators
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  - virtualDesktop: same
    $name: Virtual desktop name
    $options:
    - same: Same as the main position
    - windowsDefault: Windows default (offsets still apply)
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - middleLeft: Middle left
    - center: Center
    - middleRight: Middle right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right
  $name: Position per indicator
  $description: >-
    Give one kind of indicator a spot of its own. The virtual desktop name popup
    is separated from other text indicators so it can be placed independently.
    Anything left on "Same as the main position" follows the Position setting
    above. Independent offsets can be set in Offsets per indicator below.
- durationSeconds: 0
  $name: Seconds on screen
  $description: 0 keeps the Windows timeout. Values from 1 to 60 change the indicator timeout. Only one-shot timers set by the indicator itself are adjusted.
- indicatorOffsets:
  - volume:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Volume
  - brightness:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Brightness
  - keyboardBrightness:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Keyboard brightness
  - airplaneMode:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Airplane mode
  - camera:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Camera
  - microphone:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Microphone
  - text:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Other indicators
  - virtualDesktop:
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Use the main offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    $name: Virtual desktop name
  $name: Offsets per indicator
  $description: Choose Use this offset to set an independent value, including zero. Leaving either axis inherited preserves its main offset.
- displayProfiles:
  - - monitor: ""
      $name: Monitor interface name contains
      $description: Optional unique part of the monitor interface name, or its display name such as DISPLAY1. Leave empty to match any monitor. Enable mod logs to see names.
    - monitorCount: 0
      $name: Number of connected monitors
      $description: 0 matches any setup. Counts the monitors Windows is using for the desktop, so a mirrored pair counts as one, and a closed laptop with one external screen also counts as one. Use the monitor name to tell such setups apart.
    - indicator: all
      $name: Indicator
      $options:
      - all: All indicators
      - volume: Volume
      - brightness: Brightness
      - keyboardBrightness: Keyboard brightness
      - airplaneMode: Airplane mode
      - camera: Camera
      - microphone: Microphone
      - text: Other indicators
      - virtualDesktop: Virtual desktop name
    - position: same
      $name: Position
      $options:
      - same: Keep the indicator position
      - windowsDefault: Windows default
      - topLeft: Top left
      - topCenter: Top center
      - topRight: Top right
      - middleLeft: Middle left
      - center: Center
      - middleRight: Middle right
      - bottomLeft: Bottom left
      - bottomCenter: Bottom center
      - bottomRight: Bottom right
    - horizontal: inherit
      $name: Horizontal offset source
      $options:
      - inherit: Keep the indicator offset
      - custom: Use this offset
    - x: 0
      $name: Horizontal offset
    - vertical: inherit
      $name: Vertical offset source
      $options:
      - inherit: Keep the indicator offset
      - custom: Use this offset
    - y: 0
      $name: Vertical offset
    - enabled: false
      $name: Enable this profile
  $name: Display profiles
  $description: The first enabled matching profile wins. Profiles change placement on the monitor Windows selected. They do not move the indicator to a different monitor.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellscalingapi.h>

#include <atomic>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

enum class Position {
    windowsDefault,
    topLeft,
    topCenter,
    topRight,
    middleLeft,
    center,
    middleRight,
    bottomLeft,
    bottomCenter,
    bottomRight,
    inherit,
};

// Which indicator is being shown. Windows has a separate entry point per kind,
// so the kind is recorded as one is requested and read back when the position is
// worked out. `same` means the kind has no position of its own.
enum class Indicator {
    volume,
    brightness,
    keyboardBrightness,
    airplaneMode,
    camera,
    microphone,
    text,
    virtualDesktop,
    count,
    // Nothing has been shown yet, so there is no kind to look up and the main
    // position is used.
    unknown,
};

// Only one indicator is on screen at a time, and the entry point runs before the
// position is worked out, so a single value is enough.
std::atomic<Indicator> g_currentIndicator{Indicator::unknown};

// Set when any of the per-kind entry points didn't resolve. The recorded kind is
// then meaningless, since an unhooked kind would be placed using whichever kind
// was recorded before it, so the overrides are ignored for the session and
// everything uses the main position. Checked once at init rather than guessed at
// per placement.
std::atomic<bool> g_kindUnreliable{false};

// Must match the `position` default in the settings block above.
constexpr Position kDefaultPosition = Position::topRight;

struct OffsetOverride {
    bool customX = false;
    bool customY = false;
    int x = 0;
    int y = 0;
};
struct DisplayProfile {
    std::wstring monitor;
    int monitorCount = 0;
    Indicator indicator = Indicator::unknown;  // unknown matches all.
    Position position = Position::inherit;
    OffsetOverride offsets;
};
struct Settings {
    Position position = kDefaultPosition;
    int offsetX = 0;
    int offsetY = 0;
    bool skipHideAnimation = false;
    int durationSeconds = 0;
    std::array<Position, (size_t)Indicator::count> perIndicator;
    std::array<OffsetOverride, (size_t)Indicator::count> offsets;
    std::vector<DisplayProfile> profiles;
    Settings() { perIndicator.fill(Position::inherit); }
};
// One immutable snapshot per call: a settings reload cannot mix an anchor from
// one generation with offsets from another. Profiles are never copied per frame.
class SettingsStore {
    SRWLOCK lock = SRWLOCK_INIT;
    std::shared_ptr<const Settings> value = std::make_shared<Settings>();
    std::atomic<int> fastDuration{0};
public:
    std::shared_ptr<const Settings> load() {
        AcquireSRWLockShared(&lock);
        auto result = value;
        ReleaseSRWLockShared(&lock);
        return result;
    }
    int duration() const { return fastDuration.load(); }
    void store(std::shared_ptr<const Settings> next) {
        AcquireSRWLockExclusive(&lock);
        value.swap(next);
        fastDuration.store(value->durationSeconds);
        ReleaseSRWLockExclusive(&lock);
    }
} g_settings;

bool AnyPerIndicator(const Settings& settings) {
    for (size_t i = 0; i < (size_t)Indicator::count; i++) {
        if (settings.perIndicator[i] != Position::inherit ||
            settings.offsets[i].customX || settings.offsets[i].customY) {
            return true;
        }
    }
    return !settings.profiles.empty();
}

// Only for the log, so a line someone is asked to read back says which kind it was
// rather than a number to count enum members against.
PCWSTR IndicatorName(Indicator indicator) {
    static constexpr PCWSTR kNames[] = {
        L"volume",     L"brightness",      L"keyboardBrightness",
        L"airplaneMode", L"camera",        L"microphone",
        L"text",       L"virtualDesktop",
    };
    static_assert(ARRAYSIZE(kNames) == (size_t)Indicator::count);

    size_t i = (size_t)indicator;
    return i < ARRAYSIZE(kNames) ? kNames[i] : L"unknown";
}

// Logged from both Wh_ModInit and Wh_ModSettingsChanged.
constexpr PCWSTR kKindUnreliableMessage =
    L"An indicator entry point didn't resolve, so the position per indicator "
    L"settings are ignored and everything uses the main position";

// Set once in Wh_ModInit when either half of the hide pair didn't resolve.
bool g_hideAnimationUnavailable = false;

constexpr PCWSTR kHideAnimationUnavailableMessage =
    L"The hide entry points didn't resolve, so the slide out animation is left "
    L"alone";

// Set once in Wh_ModInit when ShowText's thunk didn't resolve. The ramp alone still
// recognises text indicators, but only the thunk can tell the virtual desktop popup
// apart from them.
bool g_virtualDesktopUndetectable = false;

constexpr PCWSTR kVirtualDesktopUndetectableMessage =
    L"ShowText thunk did not resolve, so the virtual desktop popup uses the "
    L"Other indicators position";

struct Placement {
    Position position;
    int x;
    int y;
};
Placement ResolvePlacement(const Settings& settings, Indicator kind,
                           PCWSTR monitorName, int monitorCount) {
    Placement result{settings.position, settings.offsetX, settings.offsetY};
    const size_t i = (size_t)kind;
    if (!g_kindUnreliable.load() && i < (size_t)Indicator::count) {
        if (settings.perIndicator[i] != Position::inherit)
            result.position = settings.perIndicator[i];
        const auto& offset = settings.offsets[i];
        if (offset.customX) result.x = offset.x;
        if (offset.customY) result.y = offset.y;
    }
    for (const auto& profile : settings.profiles) {
        if (profile.monitorCount && profile.monitorCount != monitorCount) continue;
        if (profile.indicator != Indicator::unknown &&
            (g_kindUnreliable.load() || profile.indicator != kind)) continue;
        if (!profile.monitor.empty()) {
            const size_t length = wcslen(monitorName);
            const size_t needle = profile.monitor.size();
            bool matched = false;
            for (size_t start = 0; start + needle <= length; ++start) {
                if (CompareStringOrdinal(monitorName + start, (int)needle,
                                         profile.monitor.c_str(), (int)needle,
                                         TRUE) == CSTR_EQUAL) {
                    matched = true;
                    break;
                }
            }
            if (!matched) continue;
        }
        if (profile.position != Position::inherit) result.position = profile.position;
        if (profile.offsets.customX) result.x = profile.offsets.x;
        if (profile.offsets.customY) result.y = profile.offsets.y;
        break;
    }
    return result;
}

HMODULE g_hardwareConfirmatorModule;

// winrt::Windows::Foundation::Rect
struct WinrtRect {
    float X;
    float Y;
    float Width;
    float Height;
};

// `area` is the region the indicator is laid out in, already shifted to 0,0.
// `rect` comes back from the original function holding the size Windows chose
// and the position it picked from the built-in setting; only the position is
// replaced.
void PlaceInArea(const WinrtRect& area,
                 Position position,
                 int offsetX,
                 int offsetY,
                 WinrtRect* rect) {
    float centerX = (area.Width - rect->Width) / 2;
    float right = area.Width - rect->Width;

    float middleY = (area.Height - rect->Height) / 2;
    float bottom = area.Height - rect->Height;

    switch (position) {
        case Position::topLeft:
            rect->X = 0;
            rect->Y = 0;
            break;
        case Position::topCenter:
            rect->X = centerX;
            rect->Y = 0;
            break;
        case Position::topRight:
            rect->X = right;
            rect->Y = 0;
            break;
        case Position::middleLeft:
            rect->X = 0;
            rect->Y = middleY;
            break;
        case Position::center:
            rect->X = centerX;
            rect->Y = middleY;
            break;
        case Position::middleRight:
            rect->X = right;
            rect->Y = middleY;
            break;
        case Position::bottomLeft:
            rect->X = 0;
            rect->Y = bottom;
            break;
        case Position::bottomCenter:
            rect->X = centerX;
            rect->Y = bottom;
            break;
        case Position::bottomRight:
            rect->X = right;
            rect->Y = bottom;
            break;
        case Position::inherit:
        case Position::windowsDefault:
            // Keep the position Windows picked and only apply the offsets.
            break;
    }

    rect->X += offsetX;
    rect->Y += offsetY;

    // An offset large enough to push the indicator out of the area would just
    // make it invisible with no way to tell why, so keep it inside.
    if (rect->X < 0) {
        rect->X = 0;
    } else if (rect->X > right) {
        rect->X = right > 0 ? right : 0;
    }

    if (rect->Y < 0) {
        rect->Y = 0;
    } else if (rect->Y > bottom) {
        rect->Y = bottom > 0 ? bottom : 0;
    }
}

// Both the public COM thunk and the host's private coroutine record the kind.
// The thunk survives on builds where the private name moves; the coroutine is
// retained as the fallback for builds where a thunk has moved instead.
#define DEFINE_RECORDER_HOOK(name, returnType, kind, parameters, arguments) \
    using name##_t = returnType(WINAPI*) parameters;                         \
    name##_t name##_Original;                                                 \
    returnType WINAPI name##_Hook parameters {                                \
        g_currentIndicator.store(kind);                                       \
        return name##_Original arguments;                                     \
    }

DEFINE_RECORDER_HOOK(ShowVolumeThunk, int, Indicator::volume,
                     (void* pThis, int value), (pThis, value));
DEFINE_RECORDER_HOOK(ShowBrightnessThunk, int, Indicator::brightness,
                     (void* pThis, int value), (pThis, value));
DEFINE_RECORDER_HOOK(ShowKeyboardBrightnessThunk, int,
                     Indicator::keyboardBrightness, (void* pThis, int value),
                     (pThis, value));
DEFINE_RECORDER_HOOK(ShowAirplaneModeOnThunk, int, Indicator::airplaneMode,
                     (void* pThis, bool value), (pThis, value));
DEFINE_RECORDER_HOOK(ShowCameraOnThunk, int, Indicator::camera,
                     (void* pThis, bool value), (pThis, value));
DEFINE_RECORDER_HOOK(ShowCameraAccessEnabledThunk, int, Indicator::camera,
                     (void* pThis, bool value), (pThis, value));
DEFINE_RECORDER_HOOK(ShowMicrophoneMutedThunk, int, Indicator::microphone,
                     (void* pThis, int state, void* text), (pThis, state, text));

// The virtual desktop name popup goes through the same ShowText entry point as
// every other text indicator. The only thing that tells them apart at hook time
// is who called it: twinui.dll for the virtual desktop switch, the aeh module
// for everything else. Hooking twinui.dll directly crashes the shell, so the
// caller module is looked up from the return address instead. Cheap, no other
// module touched, no chance of conflicting with another mod that hooks the same
// twinui function. GetModuleHandleEx is used at call time rather than caching a
// range at init, since Wh_ModInit runs before the target process begins and
// twinui isn't loaded yet on a fresh explorer start.
bool ReturnAddressIsTwinui(void* returnAddress) {
    HMODULE fromModule = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<PCWSTR>(returnAddress),
                            &fromModule)) {
        return false;
    }
    return fromModule == GetModuleHandleW(L"twinui.dll");
}

// A ShowText call from twinui goes through the thunk first, and the thunk then
// calls into the async ramp internally. The ramp is a private coroutine only
// reachable from inside the confirmator DLL, so its own return address can
// never be in twinui. The thunk records the decision in this thread_local and
// the ramp reads it back. Cleared when the thunk returns so it doesn't leak
// into a later plain text show on the same thread.
thread_local bool g_textCallFromTwinui = false;

struct ResetOnExit {
    bool& flag;
    ~ResetOnExit() { flag = false; }
};

using ShowTextThunk_t = int(WINAPI*)(void* pThis, void* text, bool value);
ShowTextThunk_t ShowTextThunk_Original;
int WINAPI ShowTextThunk_Hook(void* pThis, void* text, bool value) {
    bool fromTwinui = ReturnAddressIsTwinui(__builtin_return_address(0));
    g_textCallFromTwinui = fromTwinui;
    ResetOnExit reset{g_textCallFromTwinui};
    g_currentIndicator.store(fromTwinui ? Indicator::virtualDesktop
                                        : Indicator::text);
    return ShowTextThunk_Original(pThis, text, value);
}

// Each of these is winrt::fire_and_forget, an empty struct but not a trivial
// one, so MSVC still returns it through a hidden pointer rather than in a
// register: `this` first, the hidden retval pointer next, then the source
// arguments, with the pointer handed back as the return value. A signature
// without that slot compiles and links, but every real argument then arrives
// one register late and the original coroutine gets called with garbage
// where it expects its own arguments to be. Each name is optional;
// if neither layer resolves for a kind, Wh_ModInit disables per-indicator
// placement rather than reuse a previous kind's spot.
DEFINE_RECORDER_HOOK(ShowVolumeAsync, void*, Indicator::volume,
                     (void* pThis, void* retval, int value),
                     (pThis, retval, value));
DEFINE_RECORDER_HOOK(ShowBrightnessAsync, void*, Indicator::brightness,
                     (void* pThis, void* retval, int value),
                     (pThis, retval, value));
DEFINE_RECORDER_HOOK(ShowKeyboardBrightnessAsync, void*,
                     Indicator::keyboardBrightness,
                     (void* pThis, void* retval, int value),
                     (pThis, retval, value));
DEFINE_RECORDER_HOOK(ShowAirplaneModeOnAsync, void*, Indicator::airplaneMode,
                     (void* pThis, void* retval, bool value),
                     (pThis, retval, value));
DEFINE_RECORDER_HOOK(ShowCameraOnAsync, void*, Indicator::camera,
                     (void* pThis, void* retval, bool value),
                     (pThis, retval, value));
DEFINE_RECORDER_HOOK(ShowCameraAccessEnabledAsync, void*, Indicator::camera,
                     (void* pThis, void* retval, bool value),
                     (pThis, retval, value));

// This one takes a message alongside the state on current builds and took only
// the state on older ones. Declared with the extra parameter for both, since the
// build that doesn't take it never reads the register it arrives in. That holds
// because the mod is 64-bit only, x64 and arm64 both, where arguments go in
// registers and the caller does the cleaning up. On a 32-bit stdcall build the
// callee pops its own arguments and the same mismatch would walk the stack.
DEFINE_RECORDER_HOOK(ShowMicrophoneMutedAsync, void*, Indicator::microphone,
                     (void* pThis, void* retval, int value, void* text),
                     (pThis, retval, value, text));

#undef DEFINE_RECORDER_HOOK

using ShowTextAsync_t = void*(WINAPI*)(void* pThis,
                                       void* retval,
                                       void* text,
                                       bool value);
ShowTextAsync_t ShowTextAsync_Original;
void* WINAPI ShowTextAsync_Hook(void* pThis,
                                void* retval,
                                void* text,
                                bool value) {
    g_currentIndicator.store(g_textCallFromTwinui ? Indicator::virtualDesktop
                                                  : Indicator::text);
    return ShowTextAsync_Original(pThis, retval, text, value);
}

// The control hides itself two ways and Windows picks the animated one. Handing the
// call to the other is the whole feature, so nothing has to be torn out of the
// animation and nothing outside this control is touched.
using ConfirmatorHostControl_Hide_t = void(WINAPI*)(void* pThis);
ConfirmatorHostControl_Hide_t ConfirmatorHostControl_Hide_Original;

// Same shape as the one above today, declared separately so a drift in one doesn't
// quietly redefine the other.
using ConfirmatorHostControl_HideWithoutAnimation_t = void(WINAPI*)(void* pThis);
ConfirmatorHostControl_HideWithoutAnimation_t
    ConfirmatorHostControl_HideWithoutAnimation_Original;
void WINAPI ConfirmatorHostControl_Hide_Hook(void* pThis) {
    // Nothing on the builds this was written against calls back the other way,
    // but a build where HideWithoutAnimation went through Hide would otherwise
    // turn the redirect into unbounded recursion rather than a dead setting.
    thread_local bool redirecting = false;

    bool skip = !redirecting && g_settings.load()->skipHideAnimation &&
                ConfirmatorHostControl_HideWithoutAnimation_Original;
    Wh_Log(L"> skip=%d", (int)skip);

    if (skip) {
        // Restored however this returns. Hide is an implementation method rather
        // than an abi thunk, so an hresult_error coming out of it would otherwise
        // leave the flag latched and the setting dead for the rest of the session.
        ResetOnExit reset{redirecting};

        redirecting = true;
        return ConfirmatorHostControl_HideWithoutAnimation_Original(pThis);
    }

    return ConfirmatorHostControl_Hide_Original(pThis);
}

// GetDpiForMonitor returns the real effective DPI only to a per-monitor aware
// caller, so make sure of that for the call and restore the thread's own context.
UINT QueryMonitorDpi(HMONITOR monitor) {
    DPI_AWARENESS_CONTEXT previous =
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    UINT dpiX = 0;
    UINT dpiY = 0;
    HRESULT hr = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    if (previous) {
        SetThreadDpiAwarenessContext(previous);
    }
    return SUCCEEDED(hr) && dpiX ? dpiX : 96;
}

// "<display name> <interface name>" for display profiles to match against.
// Looking up the interface name can be slow on some systems, and a volume
// change places the indicator once per step, so the last answer is reused for a
// few seconds. The short lifetime picks up a dock or monitor change promptly.
// Fixed-size buffers rather than thread_local strings, whose destructors could
// run after the mod is unloaded.
SRWLOCK g_monitorNameLock = SRWLOCK_INIT;
WCHAR g_cachedDevice[CCHDEVICENAME];
WCHAR g_cachedMonitorName[CCHDEVICENAME + 1 + 128];
ULONGLONG g_cachedMonitorNameTime;

std::wstring MonitorDisplayName(PCWSTR device) {
    constexpr ULONGLONG kCacheMs = 5000;
    const ULONGLONG now = GetTickCount64();
    AcquireSRWLockShared(&g_monitorNameLock);
    if (g_cachedMonitorNameTime && now - g_cachedMonitorNameTime < kCacheMs &&
        wcscmp(g_cachedDevice, device) == 0) {
        std::wstring cached = g_cachedMonitorName;
        ReleaseSRWLockShared(&g_monitorNameLock);
        return cached;
    }
    ReleaseSRWLockShared(&g_monitorNameLock);

    std::wstring name = device;
    DISPLAY_DEVICEW displayDevice{};
    displayDevice.cb = sizeof(displayDevice);
    if (EnumDisplayDevicesW(device, 0, &displayDevice,
                            EDD_GET_DEVICE_INTERFACE_NAME)) {
        name += L" ";
        name += displayDevice.DeviceID;
    }

    AcquireSRWLockExclusive(&g_monitorNameLock);
    wcsncpy_s(g_cachedDevice, device, _TRUNCATE);
    wcsncpy_s(g_cachedMonitorName, name.c_str(), _TRUNCATE);
    g_cachedMonitorNameTime = now;
    ReleaseSRWLockExclusive(&g_monitorNameLock);
    return name;
}

void AdjustPositionRect(const WinrtRect& rect, WinrtRect* result) {
    const auto settings = g_settings.load();
    const Indicator kind = g_currentIndicator.load();
    RECT bounds{(LONG)rect.X, (LONG)rect.Y,
                (LONG)(rect.X + rect.Width), (LONG)(rect.Y + rect.Height)};
    HMONITOR monitor = MonitorFromRect(&bounds, MONITOR_DEFAULTTONEAREST);
    std::wstring monitorName;
    if (!settings->profiles.empty()) {
        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(monitor, &info)) {
            monitorName = MonitorDisplayName(info.szDevice);
        }
    }
    Placement placement = ResolvePlacement(*settings, kind, monitorName.c_str(),
                                            GetSystemMetrics(SM_CMONITORS));
    const UINT dpi = placement.x || placement.y ? QueryMonitorDpi(monitor) : 96;
    placement.x = MulDiv(placement.x, dpi, 96);
    placement.y = MulDiv(placement.y, dpi, 96);
    WinrtRect area{0, 0, rect.Width, rect.Height};
    if (placement.position != Position::windowsDefault || placement.x || placement.y)
        PlaceInArea(area, placement.position, placement.x, placement.y, result);
    result->X += rect.X;
    result->Y += rect.Y;
    Wh_Log(L"Placement kind=%s monitor=%s dpi=%u", IndicatorName(kind),
           monitorName.c_str(), dpi);
}

// winrt::Windows::Foundation::Rect has a user-provided constructor, so MSVC
// returns it through a hidden pointer rather than in registers on both
// architectures it's built for here: `this` first, the hidden retval pointer
// next, then the rect argument (RCX/RDX/R8 on x64, x0/x1/x2 on ARM64), with
// the pointer handed back as the return value. A signature without that slot
// looks plausible and crashes the shell on the first call. One signature covers
// both, so there's nothing to branch on here.
using HardwareConfirmatorHost_GetPositionRect_t =
    WinrtRect*(WINAPI*)(void* pThis, WinrtRect* retval, const WinrtRect* rect);
HardwareConfirmatorHost_GetPositionRect_t
    HardwareConfirmatorHost_GetPositionRect_Original;
WinrtRect* WINAPI
HardwareConfirmatorHost_GetPositionRect_Hook(void* pThis,
                                             WinrtRect* retval,
                                             const WinrtRect* rect) {
    WinrtRect inputRect = *rect;
    WinrtRect shiftedRect{0, 0, inputRect.Width, inputRect.Height};

    WinrtRect* result = HardwareConfirmatorHost_GetPositionRect_Original(
        pThis, retval, &shiftedRect);

    if (result) {
        AdjustPositionRect(inputRect, result);
    }

    return result;
}

Position PositionFromString(PCWSTR value, Position fallback = Position::windowsDefault) {
    if (!*value || wcscmp(value, L"same") == 0) return Position::inherit;
    if (wcscmp(value, L"topLeft") == 0) {
        return Position::topLeft;
    } else if (wcscmp(value, L"topCenter") == 0) {
        return Position::topCenter;
    } else if (wcscmp(value, L"topRight") == 0) {
        return Position::topRight;
    } else if (wcscmp(value, L"middleLeft") == 0) {
        return Position::middleLeft;
    } else if (wcscmp(value, L"center") == 0) {
        return Position::center;
    } else if (wcscmp(value, L"middleRight") == 0) {
        return Position::middleRight;
    } else if (wcscmp(value, L"bottomLeft") == 0) {
        return Position::bottomLeft;
    } else if (wcscmp(value, L"bottomCenter") == 0) {
        return Position::bottomCenter;
    } else if (wcscmp(value, L"bottomRight") == 0) {
        return Position::bottomRight;
    }

    // A stale or mistyped stored value would otherwise look like the mod simply
    // isn't working.
    if (wcscmp(value, L"windowsDefault") != 0) {
        Wh_Log(L"Unknown position \"%s\", using the inherited/default position",
               value);
        return fallback;
    }

    return Position::windowsDefault;
}

OffsetOverride LoadOffsets(const std::wstring& prefix) {
    OffsetOverride result;
    result.customX = wcscmp(WindhawkUtils::StringSetting::make(
        (prefix + L".horizontal").c_str()).get(), L"custom") == 0;
    result.customY = wcscmp(WindhawkUtils::StringSetting::make(
        (prefix + L".vertical").c_str()).get(), L"custom") == 0;
    result.x = std::clamp(Wh_GetIntSetting((prefix + L".x").c_str()), -100000, 100000);
    result.y = std::clamp(Wh_GetIntSetting((prefix + L".y").c_str()), -100000, 100000);
    return result;
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();
    auto value = WindhawkUtils::StringSetting::make(L"position");
    settings->position = *value.get() ? PositionFromString(value.get()) : kDefaultPosition;
    if (settings->position == Position::inherit) settings->position = kDefaultPosition;
    settings->offsetX = std::clamp(Wh_GetIntSetting(L"offsetX"), -100000, 100000);
    settings->offsetY = std::clamp(Wh_GetIntSetting(L"offsetY"), -100000, 100000);
    settings->skipHideAnimation = Wh_GetIntSetting(L"skipHideAnimation") != 0;
    settings->durationSeconds = std::clamp(Wh_GetIntSetting(L"durationSeconds"), 0, 60);
    static const PCWSTR names[] = {L"volume", L"brightness", L"keyboardBrightness",
        L"airplaneMode", L"camera", L"microphone", L"text", L"virtualDesktop"};
    for (size_t i = 0; i < ARRAYSIZE(names); ++i) {
        std::wstring key = std::wstring(L"perIndicator.") + names[i];
        auto position = WindhawkUtils::StringSetting::make(key.c_str());
        settings->perIndicator[i] = PositionFromString(position.get(), Position::inherit);
        settings->offsets[i] = LoadOffsets(std::wstring(L"indicatorOffsets.") + names[i]);
    }
    for (int i = 0;; ++i) {
        const auto prefix = L"displayProfiles[" + std::to_wstring(i) + L"]";
        auto kind = WindhawkUtils::StringSetting::make((prefix + L".indicator").c_str());
        // Every stored entry has an indicator value, so an empty one is the end
        // of the array.
        if (!*kind.get()) break;

        if (!Wh_GetIntSetting((prefix + L".enabled").c_str())) continue;
        DisplayProfile profile;
        profile.monitor = WindhawkUtils::StringSetting::make((prefix + L".monitor").c_str()).get();
        profile.monitorCount = std::clamp(
            Wh_GetIntSetting((prefix + L".monitorCount").c_str()), 0, 64);
        bool valid = wcscmp(kind.get(), L"all") == 0;
        for (size_t j = 0; j < ARRAYSIZE(names); ++j) {
            if (wcscmp(kind.get(), names[j]) == 0) {
                profile.indicator = (Indicator)j;
                valid = true;
            }
        }
        if (!valid) continue;
        auto position = WindhawkUtils::StringSetting::make((prefix + L".position").c_str());
        profile.position = PositionFromString(position.get(), Position::inherit);
        profile.offsets = LoadOffsets(prefix);
        settings->profiles.push_back(std::move(profile));
    }
    g_settings.store(std::move(settings));
}

// Duration overrides are scoped to one-shot relative timers requested directly
// by HardwareConfirmator. Absolute, periodic, cancellation and short animation
// timers pass through. Hook both APIs because Windows builds use either one.
FILETIME* AdjustDueTime(void* caller, const FILETIME* due, DWORD period,
                        FILETIME& adjusted) {
    const int seconds = g_settings.duration();
    if (!seconds || !due || period) return const_cast<FILETIME*>(due);
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<PCWSTR>(caller), &module) ||
        module != g_hardwareConfirmatorModule) return const_cast<FILETIME*>(due);
    LARGE_INTEGER value;
    value.LowPart = due->dwLowDateTime;
    value.HighPart = due->dwHighDateTime;
    if (value.QuadPart > -10000000LL || value.QuadPart < -600000000LL)
        return const_cast<FILETIME*>(due);
    Wh_Log(L"Indicator timeout %lld ms -> %d ms", -value.QuadPart / 10000,
           seconds * 1000);
    value.QuadPart = -10000000LL * seconds;
    adjusted = {value.LowPart, (DWORD)value.HighPart};
    return &adjusted;
}
decltype(&SetThreadpoolTimer) SetThreadpoolTimer_Original;
void WINAPI SetThreadpoolTimer_Hook(PTP_TIMER timer,
                                    PFILETIME due,
                                    DWORD period,
                                    DWORD window) {
    FILETIME adjusted;
    FILETIME* dueTime =
        AdjustDueTime(__builtin_return_address(0), due, period, adjusted);
    SetThreadpoolTimer_Original(timer, dueTime, period, window);
}

decltype(&SetThreadpoolTimerEx) SetThreadpoolTimerEx_Original;
BOOL WINAPI SetThreadpoolTimerEx_Hook(PTP_TIMER timer,
                                      PFILETIME due,
                                      DWORD period,
                                      DWORD window) {
    FILETIME adjusted;
    FILETIME* dueTime =
        AdjustDueTime(__builtin_return_address(0), due, period, adjusted);
    return SetThreadpoolTimerEx_Original(timer, dueTime, period, window);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Nothing to place and nothing to nudge, so don't load the DLL or install a
    // hook that would only pass the rect straight through. Windhawk reloads the
    // mod after a settings change, so it comes back as soon as there is work.
    const auto settings = g_settings.load();
    bool anyPerIndicator = AnyPerIndicator(*settings);

    if (settings->position == Position::windowsDefault && !anyPerIndicator &&
        !settings->offsetX && !settings->offsetY &&
        !settings->skipHideAnimation && !settings->durationSeconds) {
        Wh_Log(L"Nothing to do");
        return FALSE;
    }

    g_hardwareConfirmatorModule =
        LoadLibraryEx(L"Windows.Internal.HardwareConfirmator.dll", nullptr,
                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hardwareConfirmatorModule) {
        Wh_Log(L"Couldn't load Windows.Internal.HardwareConfirmator.dll");
        return FALSE;
    }

    // Windows.Internal.HardwareConfirmator.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: struct winrt::Windows::Foundation::Rect __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::GetPositionRect(struct winrt::Windows::Foundation::Rect const &))"},
            &HardwareConfirmatorHost_GetPositionRect_Original,
            HardwareConfirmatorHost_GetPositionRect_Hook,
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowVolumeAsync(int))"},
            &ShowVolumeAsync_Original,
            ShowVolumeAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowBrightnessAsync(int))"},
            &ShowBrightnessAsync_Original,
            ShowBrightnessAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowKeyboardBrightnessAsync(int))"},
            &ShowKeyboardBrightnessAsync_Original,
            ShowKeyboardBrightnessAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowAirplaneModeOnAsync(bool))"},
            &ShowAirplaneModeOnAsync_Original,
            ShowAirplaneModeOnAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowCameraOnAsync(bool))"},
            &ShowCameraOnAsync_Original,
            ShowCameraOnAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowCameraAccessEnabledAsync(bool))"},
            &ShowCameraAccessEnabledAsync_Original,
            ShowCameraAccessEnabledAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowTextAsync(struct winrt::hstring,bool))"},
            &ShowTextAsync_Original,
            ShowTextAsync_Hook,
            true,  // optional
        },
        {
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowMicrophoneMutedAsync(enum winrt::Windows::Internal::HardwareConfirmator::MicrophoneMuteState,struct winrt::hstring))",
             LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowMicrophoneMutedAsync(enum winrt::HWConfirmatorUI::MicrophoneMuteState,struct winrt::hstring))",
             LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowMicrophoneMutedAsync(enum winrt::Windows::Internal::HardwareConfirmator::MicrophoneMuteState))",
             LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowMicrophoneMutedAsync(enum winrt::HWConfirmatorUI::MicrophoneMuteState))"},
            &ShowMicrophoneMutedAsync_Original,
            ShowMicrophoneMutedAsync_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowVolume(int))"},
            &ShowVolumeThunk_Original,
            ShowVolumeThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowBrightness(int))"},
            &ShowBrightnessThunk_Original,
            ShowBrightnessThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowKeyboardBrightness(int))"},
            &ShowKeyboardBrightnessThunk_Original,
            ShowKeyboardBrightnessThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowAirplaneModeOn(bool))"},
            &ShowAirplaneModeOnThunk_Original,
            ShowAirplaneModeOnThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowCameraOn(bool))"},
            &ShowCameraOnThunk_Original,
            ShowCameraOnThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowCameraAccessEnabled(bool))"},
            &ShowCameraAccessEnabledThunk_Original,
            ShowCameraAccessEnabledThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowMicrophoneMuted(int,void *))",
             LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowMicrophoneMuted(int))"},
            &ShowMicrophoneMutedThunk_Original,
            ShowMicrophoneMutedThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowText(void *,bool))"},
            &ShowTextThunk_Original,
            ShowTextThunk_Hook,
            true,  // optional
        },
        {
            {LR"(public: void __cdecl winrt::HWConfirmatorUI::implementation::ConfirmatorHostControl::Hide(void))"},
            &ConfirmatorHostControl_Hide_Original,
            ConfirmatorHostControl_Hide_Hook,
            true,  // optional
        },
        {
            {LR"(public: void __cdecl winrt::HWConfirmatorUI::implementation::ConfirmatorHostControl::HideWithoutAnimation(void))"},
            &ConfirmatorHostControl_HideWithoutAnimation_Original,
            nullptr,  // wanted for its address, not hooked
            true,     // optional
        },
    };

    // The whole set goes in every time so the symbol cache doesn't need to be
    // resolved again when someone turns a setting on later.
    if (!HookSymbols(g_hardwareConfirmatorModule, symbolHooks,
                     ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        // Wh_ModUninit doesn't run when Wh_ModInit returns FALSE, so the
        // reference taken above has to go back here.
        FreeLibrary(g_hardwareConfirmatorModule);
        g_hardwareConfirmatorModule = nullptr;
        return FALSE;
    }

    // Each kind is recognised as long as one of its two entry points resolved.
    // Named after the symbols, not the kinds, since camera has two.
    const struct {
        PCWSTR name;
        const void* ramp;
        const void* thunk;
    } kindRecorders[] = {
        {L"ShowVolume",
         (void*)ShowVolumeAsync_Original,
         (void*)ShowVolumeThunk_Original},
        {L"ShowBrightness",
         (void*)ShowBrightnessAsync_Original,
         (void*)ShowBrightnessThunk_Original},
        {L"ShowKeyboardBrightness",
         (void*)ShowKeyboardBrightnessAsync_Original,
         (void*)ShowKeyboardBrightnessThunk_Original},
        {L"ShowAirplaneModeOn",
         (void*)ShowAirplaneModeOnAsync_Original,
         (void*)ShowAirplaneModeOnThunk_Original},
        {L"ShowCameraOn",
         (void*)ShowCameraOnAsync_Original,
         (void*)ShowCameraOnThunk_Original},
        {L"ShowCameraAccessEnabled",
         (void*)ShowCameraAccessEnabledAsync_Original,
         (void*)ShowCameraAccessEnabledThunk_Original},
        {L"ShowMicrophoneMuted",
         (void*)ShowMicrophoneMutedAsync_Original,
         (void*)ShowMicrophoneMutedThunk_Original},
        {L"ShowText",
         (void*)ShowTextAsync_Original,
         (void*)ShowTextThunk_Original},
    };

    if (!ConfirmatorHostControl_Hide_Original ||
        !ConfirmatorHostControl_HideWithoutAnimation_Original) {
        g_hideAnimationUnavailable = true;
        if (settings->skipHideAnimation) {
            Wh_Log(L"%s", kHideAnimationUnavailableMessage);
        }
    }

    // Report all of them before deciding, so a build that moved several shows all.
    for (const auto& recorder : kindRecorders) {
        Wh_Log(L"Entry point %s resolved through ramp=%d thunk=%d", recorder.name,
               !!recorder.ramp, !!recorder.thunk);

        if (!recorder.ramp && !recorder.thunk) {
            g_kindUnreliable = true;
        }
    }

    // Only worth saying to someone who has an override set. With the shipped
    // defaults there is nothing being ignored to complain about.
    if (g_kindUnreliable && anyPerIndicator) {
        Wh_Log(L"%s", kKindUnreliableMessage);
    }

    // The thread_local flag that tells virtual desktop popups apart from other
    // text indicators is set from ShowText's thunk, so if the thunk didn't
    // resolve, virtual desktop popups silently follow the Other indicators position.
    // The ramp resolving on its own is enough to keep text detection working,
    // so it wouldn't trip the recorder check above. Only worth saying if the
    // user has actually set a per-kind position for the virtual desktop popup, and
    // not when every kind already falls back to the main position.
    g_virtualDesktopUndetectable =
        !ShowTextThunk_Original && ShowTextAsync_Original;
    if (g_virtualDesktopUndetectable && !g_kindUnreliable &&
        settings->perIndicator[(size_t)Indicator::virtualDesktop] !=
            Position::inherit) {
        Wh_Log(L"%s", kVirtualDesktopUndetectableMessage);
    }

    // Every threadpool timer in Explorer goes through these hooks, so they are
    // only installed while Seconds on screen is in use. Turning it on or off
    // reloads the mod (see Wh_ModSettingsChanged). Failure is logged and never
    // prevents the placement feature from loading.
    if (settings->durationSeconds &&
        (!WindhawkUtils::SetFunctionHook(SetThreadpoolTimer,
                                         SetThreadpoolTimer_Hook,
                                         &SetThreadpoolTimer_Original) ||
         !WindhawkUtils::SetFunctionHook(SetThreadpoolTimerEx,
                                         SetThreadpoolTimerEx_Hook,
                                         &SetThreadpoolTimerEx_Original))) {
        Wh_Log(L"Duration timer hook unavailable; Windows timeout may be retained");
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hardwareConfirmatorModule) {
        FreeLibrary(g_hardwareConfirmatorModule);
        g_hardwareConfirmatorModule = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    const bool hadDuration = g_settings.duration() != 0;
    LoadSettings();
    const auto settings = g_settings.load();
    // Reload to install or remove the timer hooks.
    *bReload = hadDuration != (settings->durationSeconds != 0);
    if (g_kindUnreliable && AnyPerIndicator(*settings)) {
        Wh_Log(L"%s", kKindUnreliableMessage);
    }

    if (g_hideAnimationUnavailable && settings->skipHideAnimation) {
        Wh_Log(L"%s", kHideAnimationUnavailableMessage);
    }

    if (g_virtualDesktopUndetectable && !g_kindUnreliable &&
        settings->perIndicator[(size_t)Indicator::virtualDesktop] !=
            Position::inherit) {
        Wh_Log(L"%s", kVirtualDesktopUndetectableMessage);
    }

    return TRUE;
}
