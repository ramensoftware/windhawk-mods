// ==WindhawkMod==
// @id              on-screen-indicator-position
// @name            On-Screen Indicator Position
// @description     Place the volume/brightness/camera on-screen indicator anywhere on the screen, not just the three positions Windows offers
// @version         1.1.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         explorer.exe
// @architecture    x86-64
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
or dead center, plus a pixel offset for fine-tuning.

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
* Tested on Windows 11 build 26200 (25H2) x64.
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
    Pixels to nudge the indicator by. Positive moves right, negative moves left.
    The indicator is kept on screen, so an offset that would push it past the
    edge is ignored.
- offsetY: 0
  $name: Vertical offset
  $description: >-
    Pixels to nudge the indicator by. Positive moves down, negative moves up.
    The indicator is kept on screen, so an offset that would push it past the
    edge is ignored.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

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

struct {
    Position position;
    int offsetX;
    int offsetY;
} g_settings;

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
void PlaceInArea(const WinrtRect& area, WinrtRect* rect) {
    float left = 0;
    float centerX = (area.Width - rect->Width) / 2;
    float right = area.Width - rect->Width;

    float top = 0;
    float middleY = (area.Height - rect->Height) / 2;
    float bottom = area.Height - rect->Height;

    switch (g_settings.position) {
        case Position::topLeft:
            rect->X = left, rect->Y = top;
            break;
        case Position::topCenter:
            rect->X = centerX, rect->Y = top;
            break;
        case Position::topRight:
            rect->X = right, rect->Y = top;
            break;
        case Position::middleLeft:
            rect->X = left, rect->Y = middleY;
            break;
        case Position::center:
            rect->X = centerX, rect->Y = middleY;
            break;
        case Position::middleRight:
            rect->X = right, rect->Y = middleY;
            break;
        case Position::bottomLeft:
            rect->X = left, rect->Y = bottom;
            break;
        case Position::bottomCenter:
            rect->X = centerX, rect->Y = bottom;
            break;
        case Position::bottomRight:
            rect->X = right, rect->Y = bottom;
            break;
        case Position::windowsDefault:
            // Keep the position Windows picked and only apply the offsets.
            break;
    }

    rect->X += g_settings.offsetX;
    rect->Y += g_settings.offsetY;

    // An offset large enough to push the indicator off screen would just make
    // it invisible with no way to tell why, so keep it within the area.
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

using HardwareConfirmatorHost_GetPositionRect_t =
    WinrtRect*(WINAPI*)(void* pThis, WinrtRect* retval, const WinrtRect* rect);
HardwareConfirmatorHost_GetPositionRect_t
    HardwareConfirmatorHost_GetPositionRect_Original;
WinrtRect* WINAPI
HardwareConfirmatorHost_GetPositionRect_Hook(void* pThis,
                                             WinrtRect* retval,
                                             const WinrtRect* rect) {
    Wh_Log(L">");

    // Shift the input rect to 0,0 since the original function assumes that.
    WinrtRect shiftedRect = *rect;
    float offsetX = shiftedRect.X;
    float offsetY = shiftedRect.Y;
    shiftedRect.X = 0;
    shiftedRect.Y = 0;

    WinrtRect* result = HardwareConfirmatorHost_GetPositionRect_Original(
        pThis, retval, &shiftedRect);

    if (result) {
        PlaceInArea(shiftedRect, result);

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

    return Position::windowsDefault;
}

void LoadSettings() {
    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"position");
    g_settings.position = PositionFromString(position.get());

    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetY = Wh_GetIntSetting(L"offsetY");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

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
    };

    if (!HookSymbols(g_hardwareConfirmatorModule, symbolHooks,
                     ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
