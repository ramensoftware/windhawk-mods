// ==WindhawkMod==
// @id              fancyzones-winshift-hotkey
// @name            FancyZones Win+Shift/Ctrl+Arrows Hotkey
// @description     Makes FancyZones override Win+Shift/Ctrl+Arrows instead of Win+Arrows
// @version         1.1
// @author          David Trapp (CherryDT)
// @github          https://github.com/CherryDT
// @include         PowerToys.FancyZones.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Normally, when you enable "Override Windows Snap" in PowerToys' FancyZones, you can no
longer use the regular Snap feature for maximizing Windows and such. This mod changes
the behavior so that it overrides Win+Shift+Arrows instead (which is normally used for
moving Windows between monitors and stretching a window vertically, these are both
functionalities that can be done with FancyZones too in one way or another). This way,
you can still use Win+Arrows for regular Windows Snap and Win+Shift+Arrows for FancyZones.

Alternatively, you can select Win+Ctrl+Arrows in settings. This would collide with the
default hotkey to move windows between desktops (if you use virtual desktops), but it can
solve issues with Win+Shift+Right not working which some users observed.

Basically, this is a workaround for [this GitHub issue](https://github.com/microsoft/PowerToys/issues/2382).

_Note: Please do not open GitHub issues to ask for Alt as modifier key. The way the mod works does not work as well with every key due to how FancyZones checks and uses different modifier keys._
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKey: shift
  $name: New FancyZones Hotkey
  $options:
    - shift: Win+Shift+Arrows
    - ctrl: Win+Ctrl+Arrows
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

using GetAsyncKeyState_t = decltype(&GetAsyncKeyState);
static GetAsyncKeyState_t pGetAsyncKeyState_Original;

int vKeyCheck = 0;

static SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
  SHORT result = pGetAsyncKeyState_Original(vKey);

  if (vKey == vKeyCheck) {
    // invert "pressed" bit
    result ^= 0x8000;
  }

  return result;
}

void Wh_ModSettingsChanged() {
  WindhawkUtils::StringSetting modifierKey = WindhawkUtils::StringSetting::make(L"modifierKey");
  if (lstrcmp(modifierKey.get(), L"ctrl") == 0) {
    vKeyCheck = VK_CONTROL;
  } else {
    vKeyCheck = VK_SHIFT;
  }
}

BOOL Wh_ModInit() {
  Wh_ModSettingsChanged();
  return Wh_SetFunctionHook(
    (void*)GetAsyncKeyState,
    (void*)GetAsyncKeyState_Hook,
    (void**)&pGetAsyncKeyState_Original
  );
}
