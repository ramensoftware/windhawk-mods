// ==WindhawkMod==
// @id              discrete-touchpad-wheel
// @name            Discrete Touchpad Wheel
// @description     Converts continuous precision-touchpad (or high-resolution mouse wheel) scrolling into discrete mouse wheel events for legacy apps that ignore the scroll magnitude and scroll uncontrollably fast
// @version         0.1.0
// @author          yezhiyi9670
// @github          https://github.com/yezhiyi9670
// @license         GPL-3.0
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Discrete Touchpad Wheel

> **IMPORTANT**
>
> This mod does NOTHING by default. You need to add the apps you intend to
> affect to the **Custom process inclusion list** in the mod's Advanced
> settings.

Windows precision touchpads (as well as certain mice with high-resolution
wheels) report scrolling as a continuous stream of wheel
messages, each carrying a signed delta (a magnitude). Modern apps scroll the
page accurately according to the delta, which produces smooth
scrolling. Some legacy apps ignore the magnitude and treat every message as one
full wheel notch, which makes scrolling uncontrollably fast.

This mod swallows those continuous wheel messages and accumulates their deltas
per scroll target, then emits a real discrete `WM_MOUSEWHEEL` / `WM_MOUSEHWHEEL`
event only once the accumulated magnitude reaches the configured threshold. The
result is normal, predictable scrolling in the affected apps.

## Enabling the mod for an app

This mod does not target any process by default. To apply it to an app, open the
mod's **Advanced** tab in Windhawk and add the process name (for example
`notepad.exe`) to the **Custom process inclusion list**. The change takes effect
after saving.

Because the mod runs inside each targeted process, the accumulator state is
naturally scoped per app, and apps that are not on the list are never touched.

## Settings

- **Vertical threshold** — accumulated vertical magnitude required before one
  discrete wheel event is emitted. `120` is one standard wheel notch.
- **Horizontal threshold** — same, for horizontal scrolling.

## Limitations

- Only apps that receive scrolling through the Win32 message queue can be
  handled. Apps using Direct Manipulation bypass the queue entirely.
- Apps that already handle continuous deltas correctly should not be added to
  the inclusion list, or their scrolling will be coarsened.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- verticalThreshold: 120
  $name: Vertical threshold
  $description: Accumulated vertical magnitude required before one discrete wheel event is emitted. 120 is one standard wheel notch.
- horizontalThreshold: 120
  $name: Horizontal threshold
  $description: Accumulated horizontal magnitude required before one discrete wheel event is emitted. 120 is one standard wheel notch.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>

#include <map>

struct {
    int verticalThreshold;
    int horizontalThreshold;
} g_settings;

// Signature stored in the message extra info (GetMessageExtraInfo) of mouse
// messages promoted from touch, pen or precision touchpad input.
static constexpr DWORD kPointerExtraInfoSignature = 0xFF515700;
static constexpr DWORD kPointerExtraInfoMask = 0xFFFFFF00;

// A regular mouse wheel always delivers whole WHEEL_DELTA (120) steps, while
// continuous input (precision touchpad) delivers arbitrary magnitudes that
// rarely divide evenly by WHEEL_DELTA. Touch/pen derived input is additionally
// tagged with a signature in the message extra info.
static bool IsContinuousWheelEvent(const MSG* msg) {
    const short delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);
    if (delta != 0 && delta % WHEEL_DELTA != 0) {
        return true;
    }

    const DWORD extraInfo = (DWORD)GetMessageExtraInfo();
    return (extraInfo & kPointerExtraInfoMask) == kPointerExtraInfoSignature;
}

enum class WheelAction {
    Pass,     // dispatch the original message unchanged
    Swallow,  // drop the message entirely
    Emit,     // dispatch the discrete message `notches` times
};

// Signed magnitude accumulated per scroll target and axis.
struct WheelAccumulator {
    int vertical = 0;
    int horizontal = 0;
};

static CRITICAL_SECTION g_accumulatorLock;
static std::map<HWND, WheelAccumulator> g_accumulators;

// Swallows continuous wheel events, accumulates their magnitude per target and
// emits a real discrete wheel event (one WHEEL_DELTA notch) for every full
// threshold of accumulated magnitude.
static WheelAction HandleWheelMessage(const MSG* msg, MSG* discrete,
                                      int* notchesOut) {
    if (msg->message != WM_MOUSEWHEEL && msg->message != WM_MOUSEHWHEEL) {
        return WheelAction::Pass;
    }

    const short delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);
    if (delta == 0) {
        return WheelAction::Pass;
    }

    const bool horizontal = (msg->message == WM_MOUSEHWHEEL);
    const wchar_t* axis = horizontal ? L"horizontal" : L"vertical";

    if (!IsContinuousWheelEvent(msg)) {
        // Wh_Log(L"%s wheel: discrete event, passing through (hwnd=%p, delta=%d)",
        //        axis, msg->hwnd, delta);
        return WheelAction::Pass;
    }

    // Ctrl+wheel is zoom in most apps;
    // but most apps that cannot handle continuous scroll properly cannot handle continuous zooming either.
    // if (GET_KEYSTATE_WPARAM(msg->wParam) & MK_CONTROL) {
    //     // Wh_Log(L"%s wheel: continuous event with Ctrl held, passing through "
    //     //        L"(hwnd=%p, delta=%d)",
    //     //        axis, msg->hwnd, delta);
    //     return WheelAction::Pass;
    // }

    const int threshold =
        horizontal ? g_settings.horizontalThreshold : g_settings.verticalThreshold;
    if (threshold <= 0) {
        return WheelAction::Pass;
    }

    EnterCriticalSection(&g_accumulatorLock);

    WheelAccumulator& acc = g_accumulators[msg->hwnd];
    int& value = horizontal ? acc.horizontal : acc.vertical;

    // Drop leftover magnitude when the scroll direction reverses, so a partial
    // scroll never bleeds into the opposite direction.
    if ((delta > 0 && value < 0) || (delta < 0 && value > 0)) {
        value = 0;
    }

    value += delta;

    const int sign = value >= 0 ? 1 : -1;
    int notches = 0;
    while (value >= threshold) {
        value -= threshold;
        notches++;
    }
    while (value <= -threshold) {
        value += threshold;
        notches++;
    }

    const int leftover = value;

    if (acc.vertical == 0 && acc.horizontal == 0) {
        g_accumulators.erase(msg->hwnd);
    }

    LeaveCriticalSection(&g_accumulatorLock);

    if (notches == 0) {
        // Wh_Log(L"%s wheel: accumulating continuous event "
        //        L"(hwnd=%p, delta=%d, leftover=%d, threshold=%d)",
        //        axis, msg->hwnd, delta, leftover, threshold);
        return WheelAction::Swallow;
    }

    // Emit a genuine discrete notch; the app's own wheel handling then applies
    // its configured lines-per-notch.
    *discrete = *msg;
    discrete->wParam =
        MAKEWPARAM(LOWORD(msg->wParam), (WORD)(sign * WHEEL_DELTA));
    *notchesOut = notches;

    // Wh_Log(L"%s wheel: emitting %d discrete event(s) "
    //        L"(hwnd=%p, delta=%d, leftover=%d, threshold=%d)",
    //        axis, notches, msg->hwnd, delta, leftover, threshold);
    return WheelAction::Emit;
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
using DispatchMessageA_t = decltype(&DispatchMessageA);
static DispatchMessageW_t g_originalDispatchMessageW;
static DispatchMessageA_t g_originalDispatchMessageA;

static LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg) {
    MSG discrete;
    int notches = 0;
    switch (HandleWheelMessage(msg, &discrete, &notches)) {
        case WheelAction::Swallow:
            return 0;
        case WheelAction::Emit:
            for (int i = 0; i < notches; i++) {
                g_originalDispatchMessageW(&discrete);
            }
            return 0;
        case WheelAction::Pass:
        default:
            return g_originalDispatchMessageW(msg);
    }
}

static LRESULT WINAPI DispatchMessageA_Hook(const MSG* msg) {
    MSG discrete;
    int notches = 0;
    switch (HandleWheelMessage(msg, &discrete, &notches)) {
        case WheelAction::Swallow:
            return 0;
        case WheelAction::Emit:
            for (int i = 0; i < notches; i++) {
                g_originalDispatchMessageA(&discrete);
            }
            return 0;
        case WheelAction::Pass:
        default:
            return g_originalDispatchMessageA(msg);
    }
}

static void LoadSettings() {
    int verticalThreshold = Wh_GetIntSetting(L"verticalThreshold");
    int horizontalThreshold = Wh_GetIntSetting(L"horizontalThreshold");

    g_settings.verticalThreshold =
        verticalThreshold > 0 ? verticalThreshold : WHEEL_DELTA;
    g_settings.horizontalThreshold =
        horizontalThreshold > 0 ? horizontalThreshold : WHEEL_DELTA;

    Wh_Log(L"Loaded settings: verticalThreshold=%d, horizontalThreshold=%d",
           g_settings.verticalThreshold, g_settings.horizontalThreshold);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Discrete Touchpad Wheel: init");

    InitializeCriticalSection(&g_accumulatorLock);
    LoadSettings();

    if (!Wh_SetFunctionHook((void*)&DispatchMessageW,
                            (void*)&DispatchMessageW_Hook,
                            (void**)&g_originalDispatchMessageW)) {
        Wh_Log(L"Failed to hook DispatchMessageW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void*)&DispatchMessageA,
                            (void*)&DispatchMessageA_Hook,
                            (void**)&g_originalDispatchMessageA)) {
        Wh_Log(L"Failed to hook DispatchMessageA");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Discrete Touchpad Wheel: uninit");

    EnterCriticalSection(&g_accumulatorLock);
    g_accumulators.clear();
    LeaveCriticalSection(&g_accumulatorLock);
    DeleteCriticalSection(&g_accumulatorLock);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Discrete Touchpad Wheel: settings changed");

    LoadSettings();
}
