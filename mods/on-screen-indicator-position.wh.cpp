// ==WindhawkMod==
// @id              on-screen-indicator-position
// @name            On-Screen Indicator Position
// @description     Put the volume, brightness and camera on-screen indicators anywhere on the screen, each in its own spot if you like, and optionally skip the slide out animation
// @version         1.4.2
// @author          mario0318
// @github          https://github.com/mario0318
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshcore
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

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
virtual desktop name popup and the plain text indicator can each be given their own
position. Anything left on **Same as the main position** follows the setting above,
so you only have to touch the ones you want somewhere else. Handy if you want the
volume indicator out of the way at the bottom but still want the camera one where
you will notice it.

If you used **Plain text indicator** to place the "Desktop N" popup before 1.4.0,
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
  The ARM64 hooks were checked against the disassembly of the confirmator DLL
  but not run on hardware, so if you use it there please let me know how it
  goes.

## Credits

The hook onto the indicator's own placement function comes from [Volume control
open location](https://windhawk.net/mods/volume-control-open-location) and
[Taskbar primary on secondary
monitor](https://windhawk.net/mods/taskbar-primary-on-secondary-monitor), which
both target the same function and work out the origin handling.
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
    above. The offsets apply to all of them either way.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellscalingapi.h>

#include <atomic>

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

// Written from Wh_ModSettingsChanged on an arbitrary thread and read on the
// confirmator's UI thread, so the members are atomic.
struct {
    std::atomic<Position> position;
    std::atomic<int> offsetX;
    std::atomic<int> offsetY;
    std::atomic<bool> skipHideAnimation;
    // Position::windowsDefault means "no override", so the main position is used.
    // It is never offered as a per-indicator choice, which leaves it free to be
    // the sentinel. The main position keeps its own meaning of leaving Windows'
    // spot alone.
    std::atomic<Position> perIndicator[(size_t)Indicator::count];
} g_settings;

bool AnyPerIndicator() {
    for (size_t i = 0; i < (size_t)Indicator::count; i++) {
        if (g_settings.perIndicator[i].load() != Position::windowsDefault) {
            return true;
        }
    }

    return false;
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

// Said from two places. The text is shared rather than the call, so the line still
// reports whichever function actually logged it.
constexpr PCWSTR kKindUnreliableMessage =
    L"An indicator entry point didn't resolve, so the position per indicator "
    L"settings are ignored and everything uses the main position";

// Set once in Wh_ModInit when either half of the hide pair didn't resolve.
bool g_hideAnimationUnavailable = false;

constexpr PCWSTR kHideAnimationUnavailableMessage =
    L"The hide entry points didn't resolve, so the slide out animation is left "
    L"alone";

// The position to place the indicator that is being shown right now.
Position CurrentPosition() {
    if (g_kindUnreliable.load()) {
        return g_settings.position.load();
    }

    size_t i = (size_t)g_currentIndicator.load();
    Position perIndicator = i < (size_t)Indicator::count
                                ? g_settings.perIndicator[i].load()
                                : Position::windowsDefault;

    return perIndicator != Position::windowsDefault ? perIndicator
                                                    : g_settings.position.load();
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
// where it expects its own arguments to be — this shipped for a session
// before a maintainer caught it from the disassembly. Each name is optional;
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

    bool skip = !redirecting && g_settings.skipHideAnimation.load() &&
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

void AdjustPositionRect(const WinrtRect& rect, WinrtRect* result) {
    Wh_Log(L"> indicator=%s", IndicatorName(g_currentIndicator.load()));

    int offsetSettingX = g_settings.offsetX.load();
    int offsetSettingY = g_settings.offsetY.load();

    if (offsetSettingX || offsetSettingY) {
        RECT areaRect{
            .left = (LONG)rect.X,
            .top = (LONG)rect.Y,
            .right = (LONG)(rect.X + rect.Width),
            .bottom = (LONG)(rect.Y + rect.Height),
        };
        HMONITOR monitor = MonitorFromRect(&areaRect, MONITOR_DEFAULTTONEAREST);
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
            dpiX && dpiY) {
            offsetSettingX = MulDiv(offsetSettingX, dpiX, 96);
            offsetSettingY = MulDiv(offsetSettingY, dpiY, 96);
        }
    }

    WinrtRect shiftedRect = rect;
    float offsetX = shiftedRect.X;
    float offsetY = shiftedRect.Y;
    shiftedRect.X = 0;
    shiftedRect.Y = 0;

    Position position = CurrentPosition();
    if (position != Position::windowsDefault || offsetSettingX || offsetSettingY) {
        PlaceInArea(shiftedRect, position, offsetSettingX, offsetSettingY, result);
    }

    result->X += offsetX;
    result->Y += offsetY;
}

#undef DEFINE_RECORDER_HOOK

// winrt::Windows::Foundation::Rect has a user-provided constructor, so MSVC
// returns it through a hidden pointer rather than in registers on both
// architectures it's built for here: `this` first, the hidden retval pointer
// next, then the rect argument (RCX/RDX/R8 on x64, x0/x1/x2 on ARM64), with
// the pointer handed back as the return value. Confirmed against the ARM64
// binary's disassembly, not just inferred from the ABI docs, since the
// mismatch is exactly the kind of thing that looks plausible and crashes the
// shell on the first call. One signature covers both, so there's nothing to
// branch on here.
using HardwareConfirmatorHost_GetPositionRect_t =
    WinrtRect*(WINAPI*)(void* pThis, WinrtRect* retval, const WinrtRect* rect);
HardwareConfirmatorHost_GetPositionRect_t
    HardwareConfirmatorHost_GetPositionRect_Original;
WinrtRect* WINAPI
HardwareConfirmatorHost_GetPositionRect_Hook(void* pThis,
                                             WinrtRect* retval,
                                             const WinrtRect* rect) {
    WinrtRect shiftedRect{0, 0, rect->Width, rect->Height};

    WinrtRect* result = HardwareConfirmatorHost_GetPositionRect_Original(
        pThis, retval, &shiftedRect);

    if (result) {
        AdjustPositionRect(*rect, result);
    }

    return result;
}

Position PositionFromString(PCWSTR value) {
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
    // isn't working. "same" is a valid per-indicator value, handled by the caller.
    if (*value && wcscmp(value, L"windowsDefault") != 0 &&
        wcscmp(value, L"same") != 0) {
        Wh_Log(L"Unknown position \"%s\", using the Windows default", value);
    }

    return Position::windowsDefault;
}

void LoadSettings() {
    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"position");
    // Same reasoning as the per-indicator settings below. A setting that was
    // never written reads back empty, which happens to every setting added by an
    // update, so empty has to mean the default declared in the block rather than
    // windowsDefault. Left as windowsDefault it would trip the "nothing to do"
    // check in Wh_ModInit and the mod would sit there doing nothing.
    PCWSTR storedPosition = position.get();
    g_settings.position =
        *storedPosition ? PositionFromString(storedPosition) : kDefaultPosition;

    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetY = Wh_GetIntSetting(L"offsetY");

    g_settings.skipHideAnimation = Wh_GetIntSetting(L"skipHideAnimation");

    static const PCWSTR kIndicatorSettings[] = {
        L"perIndicator.volume",
        L"perIndicator.brightness",
        L"perIndicator.keyboardBrightness",
        L"perIndicator.airplaneMode",
        L"perIndicator.camera",
        L"perIndicator.microphone",
        L"perIndicator.text",
        L"perIndicator.virtualDesktop",
    };
    static_assert(ARRAYSIZE(kIndicatorSettings) == (size_t)Indicator::count);

    for (size_t i = 0; i < ARRAYSIZE(kIndicatorSettings); i++) {
        WindhawkUtils::StringSetting value =
            WindhawkUtils::StringSetting::make(kIndicatorSettings[i]);
        // Both "same" and an unset value, which reads back empty, already come
        // back as windowsDefault and neither is logged as unrecognised. That is
        // the "no override" sentinel, so the main position applies.
        g_settings.perIndicator[i] = PositionFromString(value.get());
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Nothing to place and nothing to nudge, so don't load the DLL or install a
    // hook that would only pass the rect straight through. Windhawk reloads the
    // mod after a settings change, so it comes back as soon as there is work.
    bool anyPerIndicator = AnyPerIndicator();

    if (g_settings.position == Position::windowsDefault && !anyPerIndicator &&
        !g_settings.offsetX && !g_settings.offsetY &&
        !g_settings.skipHideAnimation) {
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
        if (g_settings.skipHideAnimation) {
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
    // resolve, virtual desktop popups silently follow the plain text position.
    // The ramp resolving on its own is enough to keep text detection working,
    // so it wouldn't trip the recorder check above. Only worth saying if the
    // user has actually set a per-kind position for the virtual desktop popup.
    if (!ShowTextThunk_Original && ShowTextAsync_Original &&
        g_settings.perIndicator[(size_t)Indicator::virtualDesktop].load() !=
            Position::windowsDefault) {
        Wh_Log(L"ShowText thunk did not resolve, virtual desktop popup will "
               L"use the plain text position");
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

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    if (g_kindUnreliable && AnyPerIndicator()) {
        Wh_Log(L"%s", kKindUnreliableMessage);
    }

    if (g_hideAnimationUnavailable && g_settings.skipHideAnimation) {
        Wh_Log(L"%s", kHideAnimationUnavailableMessage);
    }
}
