// ==WindhawkMod==
// @id              on-screen-indicator-position
// @name            On-Screen Indicator Position
// @description     Put the volume, brightness and camera on-screen indicators anywhere on the screen, each in its own spot if you like, instead of the three positions Windows offers
// @version         1.2.6
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

Volume, brightness, keyboard brightness, airplane mode, camera, microphone and the
plain text indicator can each be given their own position. Anything left on **Same
as the main position** follows the setting above, so you only have to touch the ones
you want somewhere else. Handy if you want the volume indicator out of the way at the
bottom but still want the camera one where you will notice it.

Volume kept at the top left while brightness sits in the middle. Only one of them is
ever on screen at a time, so this is the same desktop photographed twice:

![Volume top left, brightness center](https://raw.githubusercontent.com/mario0318/windhawk-mods/628f80317652209d3feed54eadf9c329e77b04a7/on-screen-indicator-position/per-indicator.jpg)

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
* Tested on Windows 11 build 26200 (25H2) x64, on a 100% and a 150% display.

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
  $description: Where on the screen the indicator appears
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
    Pixels to nudge the indicator by, at 100% scaling. Positive moves right,
    negative moves left. The value is scaled to match the monitor, so the same
    setting moves the same distance on a scaled display. The indicator is kept
    inside the area Windows lays it out in, so an offset that would push it past
    an edge stops at the edge instead.
- offsetY: 0
  $name: Vertical offset
  $description: >-
    Pixels to nudge the indicator by, at 100% scaling. Positive moves down,
    negative moves up. The value is scaled to match the monitor, so the same
    setting moves the same distance on a scaled display. The indicator is kept
    inside the area Windows lays it out in, so an offset that would push it past
    an edge stops at the edge instead.
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
  $name: Position per indicator
  $description: >-
    Give an individual indicator its own spot. Anything left on Same as the main
    position follows the Position setting above. The offsets apply to all of them.
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
// confirmator's UI thread, so the members are atomic. Each field is still read
// separately, so a settings change landing mid-placement can put one indicator
// on screen with a mix of old and new values. That was true before the atomics
// too, and one misplaced indicator is the whole cost.
struct {
    std::atomic<Position> position;
    std::atomic<int> offsetX;
    std::atomic<int> offsetY;
    // Position::windowsDefault means "no override", so the main position is used.
    // It is never offered as a per-indicator choice, which leaves it free to be
    // the sentinel. The main position keeps its own meaning of leaving Windows'
    // spot alone.
    std::atomic<Position> perIndicator[(size_t)Indicator::count];
} g_settings;

// The position to place the indicator that is being shown right now.
bool AnyPerIndicator() {
    for (size_t i = 0; i < (size_t)Indicator::count; i++) {
        if (g_settings.perIndicator[i].load() != Position::windowsDefault) {
            return true;
        }
    }

    return false;
}

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

// Each kind of indicator has its own entry point on the host, so the kind is
// recorded as one is asked for and read back when the position is worked out.
// They are private coroutines returning winrt::fire_and_forget, an empty struct,
// so the return is passed through as the single byte it occupies. Every one is
// hooked as optional, so a name that stops resolving on some build costs the per
// indicator feature rather than the whole mod. Wh_ModInit checks afterwards that
// all eight resolved, and if any didn't it ignores the overrides for the session
// instead of placing one kind using another kind's spot.

using ShowVolumeAsync_t = char(WINAPI*)(void* pThis, int value);
ShowVolumeAsync_t ShowVolumeAsync_Original;
char WINAPI ShowVolumeAsync_Hook(void* pThis, int value) {
    g_currentIndicator.store(Indicator::volume);
    return ShowVolumeAsync_Original(pThis, value);
}

using ShowBrightnessAsync_t = char(WINAPI*)(void* pThis, int value);
ShowBrightnessAsync_t ShowBrightnessAsync_Original;
char WINAPI ShowBrightnessAsync_Hook(void* pThis, int value) {
    g_currentIndicator.store(Indicator::brightness);
    return ShowBrightnessAsync_Original(pThis, value);
}

using ShowKeyboardBrightnessAsync_t = char(WINAPI*)(void* pThis, int value);
ShowKeyboardBrightnessAsync_t ShowKeyboardBrightnessAsync_Original;
char WINAPI ShowKeyboardBrightnessAsync_Hook(void* pThis, int value) {
    g_currentIndicator.store(Indicator::keyboardBrightness);
    return ShowKeyboardBrightnessAsync_Original(pThis, value);
}

using ShowAirplaneModeOnAsync_t = char(WINAPI*)(void* pThis, bool value);
ShowAirplaneModeOnAsync_t ShowAirplaneModeOnAsync_Original;
char WINAPI ShowAirplaneModeOnAsync_Hook(void* pThis, bool value) {
    g_currentIndicator.store(Indicator::airplaneMode);
    return ShowAirplaneModeOnAsync_Original(pThis, value);
}

using ShowCameraOnAsync_t = char(WINAPI*)(void* pThis, bool value);
ShowCameraOnAsync_t ShowCameraOnAsync_Original;
char WINAPI ShowCameraOnAsync_Hook(void* pThis, bool value) {
    g_currentIndicator.store(Indicator::camera);
    return ShowCameraOnAsync_Original(pThis, value);
}

using ShowCameraAccessEnabledAsync_t = char(WINAPI*)(void* pThis, bool value);
ShowCameraAccessEnabledAsync_t ShowCameraAccessEnabledAsync_Original;
char WINAPI ShowCameraAccessEnabledAsync_Hook(void* pThis, bool value) {
    g_currentIndicator.store(Indicator::camera);
    return ShowCameraAccessEnabledAsync_Original(pThis, value);
}

using ShowMicrophoneMutedAsync_t = char(WINAPI*)(void* pThis, int value);
ShowMicrophoneMutedAsync_t ShowMicrophoneMutedAsync_Original;
char WINAPI ShowMicrophoneMutedAsync_Hook(void* pThis, int value) {
    g_currentIndicator.store(Indicator::microphone);
    return ShowMicrophoneMutedAsync_Original(pThis, value);
}

using ShowTextAsync_t = char(WINAPI*)(void* pThis, void* text, bool value);
ShowTextAsync_t ShowTextAsync_Original;
char WINAPI ShowTextAsync_Hook(void* pThis, void* text, bool value) {
    g_currentIndicator.store(Indicator::text);
    return ShowTextAsync_Original(pThis, text, value);
}

using HardwareConfirmatorHost_GetPositionRect_t =
    WinrtRect*(WINAPI*)(void* pThis, WinrtRect* retval, const WinrtRect* rect);
HardwareConfirmatorHost_GetPositionRect_t
    HardwareConfirmatorHost_GetPositionRect_Original;
WinrtRect* WINAPI
HardwareConfirmatorHost_GetPositionRect_Hook(void* pThis,
                                             WinrtRect* retval,
                                             const WinrtRect* rect) {
    Wh_Log(L">");

    // Read the offsets once so the placement below uses one consistent pair.
    int offsetSettingX = g_settings.offsetX.load();
    int offsetSettingY = g_settings.offsetY.load();

    // The rect is in the target monitor's physical pixels, so a raw offset would
    // cover less ground the more that monitor is scaled up. Scaling by its DPI
    // keeps the setting meaning the same distance everywhere. Both offsets are
    // zero by default, and then there is nothing to scale and no reason to look
    // the monitor up on every showing. Resolve it before the origin is shifted
    // away.
    if (offsetSettingX || offsetSettingY) {
        RECT areaRect{
            .left = (LONG)rect->X,
            .top = (LONG)rect->Y,
            .right = (LONG)(rect->X + rect->Width),
            .bottom = (LONG)(rect->Y + rect->Height),
        };
        HMONITOR monitor = MonitorFromRect(&areaRect, MONITOR_DEFAULTTONEAREST);
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY)) &&
            dpiX && dpiY) {
            offsetSettingX = MulDiv(offsetSettingX, dpiX, 96);
            offsetSettingY = MulDiv(offsetSettingY, dpiY, 96);
        }
    }

    // Shift the input rect to 0,0 since the original function assumes that.
    WinrtRect shiftedRect = *rect;
    float offsetX = shiftedRect.X;
    float offsetY = shiftedRect.Y;
    shiftedRect.X = 0;
    shiftedRect.Y = 0;

    WinrtRect* result = HardwareConfirmatorHost_GetPositionRect_Original(
        pThis, retval, &shiftedRect);

    if (result) {
        PlaceInArea(shiftedRect, CurrentPosition(), offsetSettingX,
                    offsetSettingY, result);

        // Shift the result back.
        result->X += offsetX;
        result->Y += offsetY;
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

    static const PCWSTR kIndicatorSettings[] = {
        L"perIndicator.volume",
        L"perIndicator.brightness",
        L"perIndicator.keyboardBrightness",
        L"perIndicator.airplaneMode",
        L"perIndicator.camera",
        L"perIndicator.microphone",
        L"perIndicator.text",
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
        !g_settings.offsetX && !g_settings.offsetY) {
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
            {LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowMicrophoneMutedAsync(enum winrt::Windows::Internal::HardwareConfirmator::MicrophoneMuteState))",
             LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowMicrophoneMutedAsync(enum winrt::HWConfirmatorUI::MicrophoneMuteState))"},
            &ShowMicrophoneMutedAsync_Original,
            ShowMicrophoneMutedAsync_Hook,
            true,  // optional
        },
    };

    // The placement hook is the first entry and is always needed. The eight that
    // record which kind is being shown only matter when a kind has a spot of its
    // own, and with the shipped defaults none does, so they aren't installed at
    // all rather than patching eight entry points to write a value that would be
    // thrown away.
    size_t hookCount = anyPerIndicator ? ARRAYSIZE(symbolHooks) : 1;

    if (!HookSymbols(g_hardwareConfirmatorModule, symbolHooks, hookCount)) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    // An optional symbol that isn't found leaves its original pointer alone, so
    // a null here means that kind would never be recorded and every kind after
    // it would be placed using a stale one. Rather than misplace an indicator,
    // drop to the main position for everything and say so in the log.
    if (anyPerIndicator) {
        const void* kindRecorders[] = {
            (void*)ShowVolumeAsync_Original,
            (void*)ShowBrightnessAsync_Original,
            (void*)ShowKeyboardBrightnessAsync_Original,
            (void*)ShowAirplaneModeOnAsync_Original,
            (void*)ShowCameraOnAsync_Original,
            (void*)ShowCameraAccessEnabledAsync_Original,
            (void*)ShowMicrophoneMutedAsync_Original,
            (void*)ShowTextAsync_Original,
        };

        for (const void* recorder : kindRecorders) {
            if (!recorder) {
                Wh_Log(
                    L"An indicator entry point didn't resolve, so the position "
                    L"per indicator settings are ignored and everything uses "
                    L"the main position");
                g_kindUnreliable = true;
                break;
            }
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // The hooks are already gone by this point, so handing back the reference
    // taken in Wh_ModInit is safe. Without this every enable and disable cycle
    // leaves one behind.
    if (g_hardwareConfirmatorModule) {
        FreeLibrary(g_hardwareConfirmatorModule);
        g_hardwareConfirmatorModule = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    // Whether the kind recorders are installed is decided in Wh_ModInit, so
    // turning the first override on, or the last one off, needs a reload to
    // match. Everything else takes effect on the next indicator.
    bool hadPerIndicator = AnyPerIndicator();
    LoadSettings();
    *bReload = AnyPerIndicator() != hadPerIndicator;

    return TRUE;
}
