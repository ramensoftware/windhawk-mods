// ==WindhawkMod==
// @id              fancyzones-winshift-hotkey
// @name            FancyZones Win+Shift+Arrows Hotkey
// @description     Makes FancyZones override Win+Shift+Arrows instead of Win+Arrows
// @version         1.0
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

Basically, this is a workaround for [this GitHub issue](https://github.com/microsoft/PowerToys/issues/2382).

_Note: Please do not open GitHub issues to ask for support or other modifier keys. The way the mod works only works well with Shift due to how FancyZones checks and uses different modifier keys._
*/
// ==/WindhawkModReadme==

#include <windows.h>

using GetAsyncKeyState_t = decltype(&GetAsyncKeyState);
static GetAsyncKeyState_t pGetAsyncKeyState_Original;

static SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
  SHORT result = pGetAsyncKeyState_Original(vKey);

  if (vKey == VK_SHIFT || vKey == VK_LSHIFT || vKey == VK_RSHIFT) {
    // invert "pressed" bit
    result ^= 0x8000;
  }

  return result;
}

BOOL Wh_ModInit() {
  return Wh_SetFunctionHook(
    (void*)GetAsyncKeyState,
    (void*)GetAsyncKeyState_Hook,
    (void**)&pGetAsyncKeyState_Original
  );
}
