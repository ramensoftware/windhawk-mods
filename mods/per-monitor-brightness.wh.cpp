// ==WindhawkMod==
// @id              per-monitor-brightness
// @name            Per-monitor brightness and more in Quick Settings
// @description     Brightness, contrast, volume, input and power controls for every connected monitor, plus sliders for all of them at once, in the Windows 11 Quick Settings panel
// @version         3.0
// @author          bardelyne
// @github          https://github.com/bardelyne
// @include         ShellHost.exe
// @architecture    x86-64
// @license         GPL-3.0
// @compilerOptions -ldxva2 -lole32 -loleaut32 -lwbemuuid -luuid -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// This mod used to carry XAML-diagnostics plumbing adapted from m417z's
// "Windows 11 Notification Center Styler". That is gone -- diagnostics allows
// only one consumer per process, so holding the slot stopped that very mod
// theming the Control Center -- but the WH_CALLWNDPROC trick in
// RunOnXamlThread still follows its approach, and the mod remains GPLv3.

// ==WindhawkModReadme==
/*
# Per-monitor brightness and more in Quick Settings

Windows gives you exactly one brightness slider no matter how many monitors you
have, and on a desktop it gives you none at all. This mod adds a labelled slider
for every connected display, right in the Quick Settings panel, each showing its
current level and carrying the shell's own animated brightness icon -- plus
contrast and power where the monitor supports them, and sliders that set every
display at once.

![Per-monitor brightness sliders in Quick Settings](https://raw.githubusercontent.com/bardelyne/per-monitor-brightness/main/screenshot.png)

## How each display is driven

- **External monitors** use **DDC/CI** (VCP code `0x10`), the I2C side-channel
  in the video cable that the monitor's own on-screen menu uses. Most monitors
  made in the last decade support it; some budget panels and some USB-C docks
  do not.
- **Laptop internal panels** use **WMI**, the same path the stock slider takes.

A display that answers neither is listed as uncontrollable rather than being
silently dropped.

**Contrast** (VCP `0x12`), **power** (`0xD6`), **volume** (`0x62`) and
**input** (`0x60`) are DDC/CI as well, and each is offered only for monitors
that answer it: many answer brightness but not power, and a laptop panel has
none of them over WMI. Which inputs a monitor has comes from its capabilities
string, which takes a second or so to read -- so it is read once per session,
in the background, and the input buttons appear a moment after the rest.

## Features

- One titled slider per display. The title shows the live percentage of every
  slider under it, in order -- "Samsung · 80% · 50% · 30%" for brightness,
  contrast and volume -- leaving out any folded into a closed dropdown.
- A contrast slider for each monitor that supports it.
- "All displays" brightness and contrast sliders that set every display to the
  same level, shown once there are two or more displays to drive -- or with
  just one, when its own slider is hidden.
- A power button at the end of the brightness slider of monitors that support
  it.
- A volume slider for monitors with audio, with the shell's own animated
  speaker icon.
- Input buttons (HDMI 1, HDMI 2, DP 1...) that switch a monitor's input.
- Two layouts: each display's extra controls in a dropdown (the default), or
  everything visible.
- The mouse wheel moves whichever slider is under the pointer.
- Optionally, clicking a slider's icon jumps to a level: left, middle and right
  button each have their own.
- Per-display settings: a name of your own, hiding a display altogether, or
  hiding just some of its controls. Two monitors of the same model, which
  report the same name, are numbered left to right.
- Sliders ordered left to right to match how the monitors sit on your desk.
- Displays identified by EDID device path, so the right slider follows the right
  monitor across hotplug, reordering and reboots.
- Each slider snaps to what its monitor can actually represent. Panels do not
  all use a 0-100 scale -- a Samsung G32 reports 0-50 -- so offering 1% steps
  would just mean several slider positions that write the same value.
- Function keys move the sliders live, and can optionally drive external
  monitors too, which they cannot do on their own.
- Monitors plugged in or unplugged are picked up immediately.
- Values are re-read whenever the panel opens, so changes made elsewhere show up.
- The stock brightness slider can be hidden, since it duplicates the built-in
  panel's row.
- The sliders sit with the Windows ones by default, and can be moved above them
  or down to the bottom of the flyout.
- Displays that cannot be controlled can be hidden.

## Settings

Everything this mod adds can be shown or hidden on its own, so the panel only
carries what you use.

- **Where to put the sliders** -- below the Windows sliders by default, which
  puts the panel on the same card as the volume and stock brightness rows and
  above the settings button. `aboveSliders` puts it first in that card;
  `bottom` puts it under the settings button, outside the card, which is where
  versions before 2.1 always put it.
- **Hide displays that cannot be controlled** -- off by default. A display
  answering neither DDC/CI nor WMI is normally still listed, labelled
  "Brightness control not supported", so that a monitor missing from the panel
  is never a mystery. Turn this on to leave them out; they are still checked,
  so one that starts answering appears on its own.
- **Hide the built-in brightness slider** -- on by default; the stock slider
  only controls the internal panel, which already has its own row here.
- **Laptop brightness keys control every monitor** -- `off` by default.
  `relative` shifts other monitors by the same amount, preserving their offset;
  `match` sets them all to the same percentage.

  It is off by default because Windows raises the same event for a brightness
  key, for the power plan's AC/battery levels, and for idle dimming, with no
  way to tell them apart -- so unplugging the charger or walking away would
  also write to every external monitor. Unlike everything else this mod does,
  that write survives turning the setting off: the monitor stores the value
  itself.
- **Layout** -- `dropdown` (default) shows each display's brightness and tucks
  its other controls behind a chevron; `expanded` shows everything.
- **Show brightness sliders**, **Show brightness for all displays**, **Show
  contrast sliders**, **Show contrast for all displays**, **Show power
  buttons**, **Show volume sliders**, **Show input buttons** -- all on by
  default, each covering only what it names. **Show brightness sliders** is
  each display's own brightness slider: turned off, brightness is set with
  the all-displays slider alone, while contrast, volume and input stay (the
  power button, which sits at the end of the brightness slider, goes with
  it). The all-displays rows appear with two or more displays to drive, or
  when the per-display sliders they would duplicate are hidden; contrast,
  volume, input and power appear only on monitors that support them.
- **Mouse wheel step** -- 5 percentage points per notch by default; 0 leaves
  the wheel to the flyout.
- **Click an icon to jump to a level** -- off by default; the three levels are
  settings of their own (0, 50 and 100 by default).
- **Per-display settings** -- text to look for in a display's name or device
  id, then a name to show instead, and whether to hide the display or its
  contrast, volume, input or power controls. To tell two monitors of the same
  model apart,
  hover over a display's name to see its device id and use the part that
  differs between them, at the end (for example `UID4352`). That part follows
  the video output the monitor is plugged into, so swapping cables between
  ports swaps the names too.

## Compatibility

Requires a Windows 11 build where the Control Center is hosted by
`ShellHost.exe` -- developed and tested on 25H2 (build 26200).

Earlier builds host it in `ShellExperienceHost.exe` and are **not supported**.
That process is not included, but the reason has changed and is now only that
it is untested. The original objection -- that a second XAML host would start a
second brightness engine, doubling the WMI connection, the DDC/CI probe and the
response to every brightness keypress -- no longer applies: the engine starts
only where `ControlCenter.dll` is loaded, so a process that never hosts the
Control Center never starts one.

What remains unverified is whether the hooked symbol and the `L1Grid` layout
this mod depends on are the same on those builds. Without one to test on, the
include stays off.

## Notes and limitations

- A DDC/CI write takes roughly 50-60 ms, and the bus saturates while dragging.
  All hardware access happens on a background thread and repeated values
  collapse into a single write, so dragging never stalls the shell -- but an
  external monitor will visibly step rather than fade. The internal panel is
  around ten times faster and looks smooth.
- DDC/CI has no notification channel: a monitor only ever answers what the host
  asks it. Brightness changed using the monitor's own buttons therefore cannot
  be detected, and only shows up the next time the panel is opened.
- Not every monitor implements DDC/CI correctly. A display that does not answer
  at all is listed as uncontrollable and gets no slider; one that answers but
  misbehaves shows up as writes reporting `ok=0`. Either way, turn on logging
  for this mod in Windhawk (the mod's **Advanced** settings -> **Logging**) to
  see which.
- The power button turns a monitor off over DDC/CI ("DPM off"). Most monitors
  still listen in that state and come back when the button is pressed again,
  but not all do; one that does not has to be switched on with its own power
  button. The button is only offered while another display is connected, so
  there is always a screen left to see.
- Switching a monitor to another input hands it to whatever is on that input.
  Some monitors keep answering DDC/CI on the input they left, so you can
  switch back from here; others do not, and need their own buttons.
- A monitor that was asleep, switched to another input, or behind a dock that
  was still enumerating when you signed in will fail that first check through
  no fault of its own. It is retried rather than written off for the session:
  opening the panel again re-probes it, backing off from 30 seconds to at most
  16 minutes between attempts, and unplugging and replugging it starts over
  immediately.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideStockBrightness: true
  $name: Hide the built-in brightness slider
  $description: >-
    The stock slider only controls the internal laptop panel, which this mod
    already gives its own labelled slider, so leaving both on shows the same
    display twice. Turn this off to keep the original slider as well.
- panelPosition: belowSliders
  $name: Where to put the sliders
  $description: >-
    Where the brightness panel sits in the Quick Settings flyout.

    "Below the Windows sliders" is the default and looks the most built-in:
    the panel joins the card that holds the volume and stock brightness rows,
    above the settings button. "At the bottom" is where earlier versions put
    it, under the settings button and outside that card.
  $options:
  - aboveSliders: Above the Windows sliders
  - belowSliders: Below the Windows sliders
  - bottom: At the bottom, under the settings button
- hideUnsupported: false
  $name: Hide displays that cannot be controlled
  $description: >-
    A display that answers neither DDC/CI nor WMI is normally still listed,
    with "Brightness control not supported" where its slider would be, so that
    a missing monitor is never a mystery.

    Turn this on to leave those out entirely. They are still checked: one that
    starts answering -- a monitor that was asleep or on another input when you
    signed in -- appears on its own.
- followInternalBrightness: "off"
  $name: Laptop brightness keys control every monitor
  $description: >-
    Function keys only reach the built-in panel -- that is a hardware limit, not
    a Windows one. This mirrors those keypresses onto external monitors over
    DDC/CI, so one keypress dims everything.

    Off by default, because Windows gives no way to tell a keypress apart from
    any other change to the built-in panel. The same signal is raised by the
    power plan's AC and battery brightness levels and by "dim the display
    after N minutes", so with this on, unplugging the charger or leaving the
    machine idle also writes to every external monitor. That write is not
    undone by turning this off again or by disabling the mod: a monitor keeps
    the brightness it was given in its own settings, so its previous value is
    gone unless you set it back by hand.

    Dragging the built-in display's own slider in this panel leaves the other
    monitors alone.
  $options:
  - "off": Leave other monitors alone
  - relative: Shift other monitors by the same amount (keeps their offset)
  - match: Set other monitors to the same percentage
- layoutStyle: dropdown
  $name: Layout
  $description: >-
    "Dropdown" shows each display's brightness slider and tucks its other
    controls, such as contrast, behind a chevron at the end of its title.
    "Everything visible" shows all of them at once. With the brightness
    sliders turned off there is nothing to keep out, so the other controls
    are shown either way.
  $options:
  - dropdown: Dropdown per display
  - expanded: Everything visible
- showPerMonitor: true
  $name: Show brightness sliders
  $description: >-
    A brightness slider for each display. Turn this off to set brightness only
    with the all-displays slider; each display's other controls (contrast,
    volume, input) stay. Power buttons sit at the end of these sliders, so
    they go with them.
- showMasterBrightness: true
  $name: Show brightness for all displays
  $description: >-
    One slider that sets every display to the same brightness. Appears when
    there are two or more displays to drive, or when the brightness sliders
    are hidden.
- showContrast: true
  $name: Show contrast sliders
  $description: >-
    For monitors that support contrast over DDC/CI. A laptop panel has no
    contrast control, so it never gets one.
- showMasterContrast: true
  $name: Show contrast for all displays
  $description: >-
    One slider that sets every monitor that supports contrast to the same
    value. Appears when two or more monitors support it, or when the
    per-display contrast sliders are hidden.
- showPowerButton: true
  $name: Show power buttons
  $description: >-
    A button at the end of the brightness slider that turns a monitor off over
    DDC/CI, for monitors that report a power state -- many do not. Pressing
    it again turns the monitor back on, which most monitors allow; one that
    stops listening once off has to be switched on with its own button. Only
    offered while another display is connected, so there is always a screen
    left to see.
- showVolume: true
  $name: Show volume sliders
  $description: >-
    For monitors with speakers or a headphone jack that report volume over
    DDC/CI. Its speaker icon takes clicks like the other icons.
- showInputSwitcher: true
  $name: Show input buttons
  $description: >-
    A button for each input a monitor lists (HDMI 1, HDMI 2, DP 1...), the
    current one highlighted. Pressing another switches the monitor to it --
    which, if this PC is not on that input, hands the monitor to whatever is.
    Switch back with the monitor's own buttons, or from here if the monitor
    still answers on the input it left.
- scrollStep: 5
  $name: Mouse wheel step
  $description: >-
    How far one notch of the mouse wheel moves the slider under the pointer,
    in percentage points. 0 turns the wheel off, leaving it to scroll the
    flyout.
- iconClicks: false
  $name: Click an icon to jump to a level
  $description: >-
    Clicking a slider's icon sets it to a fixed level, one for each mouse
    button. Off by default, so a stray click never dims a screen.
- iconClickLeft: 0
  $name: Left click level
- iconClickMiddle: 50
  $name: Middle click level
- iconClickRight: 100
  $name: Right click level
- displaySettings:
  - - match: ""
      $name: Text to look for
      $description: >-
        Part of the display's name as the panel shows it (for example
        LS27F32xG), or of its device id -- hover over a display's name in the
        panel to see it.
    - name: ""
      $name: Name to show
      $description: Leave empty to keep the display's own name.
    - hide: false
      $name: Hide this display
    - hideContrast: false
      $name: Hide its contrast slider
    - hideVolume: false
      $name: Hide its volume slider
    - hideInput: false
      $name: Hide its input buttons
    - hidePower: false
      $name: Hide its power button
  $name: Per-display settings
  $description: >-
    Settings for individual displays: a name of your own, or what to hide.
    The first entry whose text appears in a display's name or device id
    applies; case does not matter. Two monitors of the same model report the
    same name: hover over a display's name in the panel to see its device id,
    and use the part at the end that differs between them (for example
    UID4352). It follows the video output the monitor is plugged into.
*/
// ==/WindhawkModSettings==


#include <inspectable.h>

// winbase.h defines GetCurrentTime as a macro, which collides with
// Windows.UI.Xaml.Media.Animation's method of the same name.
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
// Not just the .0.h forward declarations: Append/Size have deduced return
// types and must be defined before use.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.h>

#pragma pop_macro("GetCurrentTime")

#include <roapi.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// WinUI2 ABI, declared by hand. Windhawk does ship the WinUI2 C++/WinRT
// projection, but its headers include "winrt/impl/..." which resolves to the
// WinUI3 copies sitting at the default location, so the declarations and
// definitions disagree and it cannot be included without machine-specific -I
// flags -- which would make this mod non-portable. These three interfaces are
// all we need, and their GUIDs and vtable order come straight from that
// projection. The shell's AnimatedIcon is WinUI2: it lives inside a
// Windows.UI.Xaml tree, which WinUI3 controls cannot.
// GUIDs as constants rather than __declspec(uuid): that is an MSVC extension
// which clang ignores in the mingw target, leaving __uuidof unresolvable.
constexpr GUID kIID_AnimatedIcon = {
    0xF705DFDA, 0x8196, 0x56D0, {0x8D, 0xCF, 0x2B, 0x66, 0xC2, 0xAE, 0xD7, 0x91}};
constexpr GUID kIID_AnimatedIconStatics = {
    0x381896A8, 0xEE9F, 0x5823, {0xAB, 0xF1, 0xB5, 0x96, 0xAD, 0xCC, 0x77, 0xD1}};
constexpr GUID kIID_AnimatedVisualSource2 = {
    0x1A3B53A7, 0xA8FE, 0x59A1, {0xB5, 0x44, 0x43, 0xA4, 0xD9, 0xC8, 0x1E, 0xF2}};

struct IAnimatedIconAbi : ::IInspectable {
    virtual HRESULT __stdcall get_Source(void**) = 0;
    virtual HRESULT __stdcall put_Source(void*) = 0;
    virtual HRESULT __stdcall get_FallbackIconSource(void**) = 0;
    virtual HRESULT __stdcall put_FallbackIconSource(void*) = 0;
    virtual HRESULT __stdcall get_MirroredWhenRightToLeft(bool*) = 0;
    virtual HRESULT __stdcall put_MirroredWhenRightToLeft(bool) = 0;
};

struct IAnimatedIconStaticsAbi : ::IInspectable {
    virtual HRESULT __stdcall get_StateProperty(void**) = 0;
    virtual HRESULT __stdcall SetState(void*, void*) = 0;
    virtual HRESULT __stdcall GetState(void*, void**) = 0;
    virtual HRESULT __stdcall get_SourceProperty(void**) = 0;
    virtual HRESULT __stdcall get_FallbackIconSourceProperty(void**) = 0;
    virtual HRESULT __stdcall get_MirroredWhenRightToLeftProperty(void**) = 0;
};

struct IAnimatedVisualSource2Abi : ::IInspectable {
    virtual HRESULT __stdcall get_Markers(void**) = 0;
    // SetColorProperty follows here; never called, so left undeclared.
};

// XAML controls are composable, so their activation factory does not implement
// IActivationFactory::ActivateInstance -- it answers E_NOTIMPL. Construction
// goes through this instead, passing a null outer for a plain instance.
constexpr GUID kIID_AnimatedIconFactory = {
    0x3356E0D1, 0xD82F, 0x5FC1, {0x81, 0x65, 0x9B, 0x9D, 0x1B, 0x9D, 0x95, 0x14}};

struct IAnimatedIconFactoryAbi : ::IInspectable {
    virtual HRESULT __stdcall CreateInstance(void* outer, void** inner,
                                             void** instance) = 0;
};

constexpr wchar_t kAnimatedIconClass[] = L"Microsoft.UI.Xaml.Controls.AnimatedIcon";

// ===========================================================================
// Brightness engine (spliced in from brightness_engine.h by build_mod.sh).
// ===========================================================================

// Per-monitor brightness engine.
//
// Two transports, picked per display at enumeration time:
//   * DDC/CI  (external monitors) -- low-level VCP 0x10 over I2C. ~56 ms/write.
//   * WMI     (internal laptop panels) -- WmiMonitorBrightnessMethods.
//
// DDC/CI displays can also carry contrast (VCP 0x12), power (0xD6), speaker
// volume (0x62) and input selection (0x60), each probed separately: a monitor
// that answers brightness does not necessarily answer any of them. Which
// inputs exist only the monitor's capabilities string says, so that is read
// too -- once, and in the background, because it is slow.
//
// Every hardware call happens on one worker thread. Callers post a target
// percentage and return immediately; the worker coalesces, so a slider drag
// that posts 200 values only puts as many on the wire as the bus can carry.
//
// Designed to be #included by the Windhawk mod as-is: no globals, no console,
// no dependency on anything outside the Win32/COM SDK.


#include <windows.h>

#include <lowlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>

#include <comdef.h>
#include <wbemidl.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cwctype>
#include <functional>
#include <map>
#include <utility>

namespace brightness {

// Minimum gap between DDC/CI writes. Long enough that the shell's UI thread
// gets the bus back and a slider tracks the pointer; short enough that an
// external monitor still visibly follows a drag.
inline constexpr std::chrono::milliseconds kDdcCooldown{140};

// What other displays do when the internal panel's brightness changes on its
// own -- function keys, mostly.
enum class FollowMode {
    Off,       // leave them alone
    Match,     // set them to the same percentage
    Relative,  // shift them by the same delta, preserving their offset
};

enum class Transport {
    None,   // display found but no way to control it
    DdcCi,  // external, VCP 0x10
    Wmi,    // internal panel
};

struct Display {
    // Stable across reboots, hotplug and monitor reordering: derived from the
    // EDID device instance path. HMONITOR is not stable, so it is never a key.
    std::wstring stableId;
    std::wstring name;           // "LS27F32xG", "Built-in display"
    RECT rect{};
    Transport transport = Transport::None;
    int percent = -1;  // last known, -1 if never read

    // DdcCi
    HANDLE hPhysical = nullptr;
    DWORD vcpMax = 100;  // raw scale; a Samsung G32 reports 50, not 100

    // Contrast, VCP 0x12. DDC/CI only; contrastMax stays 0 for a display that
    // does not answer it, and contrast stays -1 until it is read.
    int contrast = -1;
    DWORD contrastMax = 0;

    // Power, VCP 0xD6. Only a display that reports an actual power state gets
    // a button: plenty of monitors answer the read with 0, which is no state
    // at all, and ignore writes to it.
    bool hasPower = false;
    bool poweredOff = false;

    // Speaker volume, VCP 0x62, for monitors with audio.
    int volume = -1;
    DWORD volumeMax = 0;

    // Input source, VCP 0x60: the values the monitor lists in its
    // capabilities, and which of them is current -- -1 when that is not known,
    // since some monitors answer the read with a value their own list does
    // not contain.
    std::vector<int> inputs;
    int input = -1;

    // Wmi
    std::wstring wmiPath;  // __RELPATH of the WmiMonitorBrightnessMethods instance
};

namespace detail {

// Runs a thread body so that nothing can escape it.
//
// This is not defensive tidiness. An exception leaving a thread procedure
// calls std::terminate(), which aborts the *host* process -- and everything
// below talks to COM, WMI and I2C, all of which fail in ways that throw when
// the shell is still starting up and those services are not ready yet. An
// unguarded throw here takes ShellHost down, and it restarts into the same
// throw, so the shell never comes back.
// `report` takes (where, what) and is expected to log; it must not throw.
template <class Fn, class Report>
void RunGuarded(const wchar_t* where, Fn&& fn, Report&& report) noexcept {
    try {
        fn();
    } catch (const _com_error& e) {
        // swprintf, not wsprintf: wsprintf does not bound its output to the
        // destination, and _com_error::ErrorMessage() can carry an arbitrarily
        // long IErrorInfo description from a WMI provider.
        wchar_t buf[512] = {};
        swprintf(buf, ARRAYSIZE(buf), L"_com_error %08X (%ls)",
                 static_cast<unsigned>(e.Error()),
                 e.ErrorMessage() ? e.ErrorMessage() : L"?");
        report(where, buf);
    } catch (const std::exception& e) {
        // Zero-initialised and checked: MultiByteToWideChar returns 0 without
        // touching the buffer when the message does not fit, and the result is
        // handed straight to a %ls.
        wchar_t buf[512] = {};
        if (MultiByteToWideChar(CP_ACP, 0, e.what(), -1, buf, 512) == 0) {
            report(where, L"(exception message too long to convert)");
        } else {
            report(where, buf);
        }
    } catch (...) {
        report(where, L"unknown exception");
    }
}


inline std::wstring GetStrProp(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT v;
    VariantInit(&v);
    std::wstring out;
    if (SUCCEEDED(obj->Get(name, 0, &v, nullptr, nullptr)) && v.vt == VT_BSTR &&
        v.bstrVal) {
        out = v.bstrVal;
    }
    VariantClear(&v);
    return out;
}

inline int GetIntProp(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT v;
    VariantInit(&v);
    int out = -1;
    if (SUCCEEDED(obj->Get(name, 0, &v, nullptr, nullptr))) {
        switch (v.vt) {
            case VT_I4:
                out = v.lVal;
                break;
            case VT_UI1:
                out = v.bVal;
                break;
            case VT_I2:
                out = v.iVal;
                break;
            case VT_UI4:
                out = static_cast<int>(v.ulVal);
                break;
            default:
                break;
        }
    }
    VariantClear(&v);
    return out;
}

// WMI exposes uint16[] (e.g. UserFriendlyName) as a SAFEARRAY of VT_I4.
inline std::wstring GetU16ArrayProp(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT v;
    VariantInit(&v);
    std::wstring out;
    if (SUCCEEDED(obj->Get(name, 0, &v, nullptr, nullptr)) && (v.vt & VT_ARRAY) &&
        v.parray) {
        SAFEARRAY* sa = v.parray;
        VARTYPE elem = static_cast<VARTYPE>(v.vt & VT_TYPEMASK);
        LONG lb = 0, ub = -1;
        SafeArrayGetLBound(sa, 1, &lb);
        SafeArrayGetUBound(sa, 1, &ub);
        // SafeArrayGetElement writes SafeArrayGetElemsize(sa) bytes into the
        // destination, so the element type has to be one we have actually
        // sized a slot for. WmiMonitorID.UserFriendlyName is uint16[], which
        // arrives as VT_I4, but bail rather than smash the stack if a future
        // property hands us something wider.
        if (elem != VT_UI1 && elem != VT_I2 && elem != VT_UI2 &&
            elem != VT_I4 && elem != VT_UI4) {
            VariantClear(&v);
            return out;
        }
        for (LONG i = lb; i <= ub; ++i) {
            LONG ch = 0;
            if (elem == VT_UI1) {
                BYTE b = 0;
                if (FAILED(SafeArrayGetElement(sa, &i, &b))) {
                    break;
                }
                ch = b;
            } else if (elem == VT_I2 || elem == VT_UI2) {
                SHORT w = 0;
                if (FAILED(SafeArrayGetElement(sa, &i, &w))) {
                    break;
                }
                ch = static_cast<unsigned short>(w);
            } else {
                if (FAILED(SafeArrayGetElement(sa, &i, &ch))) {
                    break;
                }
            }
            if (ch == 0) {
                break;
            }
            out.push_back(static_cast<wchar_t>(ch));
        }
    }
    VariantClear(&v);
    return out;
}

inline std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return s;
}

// "\\?\DISPLAY#SAM78C9#5&31ef9774&0&UID4352#{guid}"
//   -> "DISPLAY\SAM78C9\5&31ef9774&0&UID4352"
// which is the WMI InstanceName minus its "_0" suffix.
inline std::wstring DeviceInterfaceToInstanceId(std::wstring s) {
    if (s.rfind(L"\\\\?\\", 0) == 0) {
        s.erase(0, 4);
    }
    size_t guid = s.rfind(L'#');
    if (guid != std::wstring::npos) {
        s.erase(guid);
    }
    std::replace(s.begin(), s.end(), L'#', L'\\');
    return s;
}

}  // namespace detail

// Thin wrapper over root\wmi. Lives entirely on the worker thread, so the
// interface pointers never cross an apartment.
class WmiSession {
   public:
    WmiSession() = default;
    WmiSession(const WmiSession&) = delete;
    WmiSession& operator=(const WmiSession&) = delete;

    ~WmiSession() {
        if (setBrightnessInDef_) {
            setBrightnessInDef_->Release();
        }
        if (brightnessClass_) {
            brightnessClass_->Release();
        }
        if (services_) {
            services_->Release();
        }
        if (locator_) {
            locator_->Release();
        }
    }

    bool Init() {
        HRESULT hr = CoCreateInstance(CLSID_WbemLocator, nullptr,
                                      CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                      reinterpret_cast<LPVOID*>(&locator_));
        if (FAILED(hr)) {
            return false;
        }

        hr = locator_->ConnectServer(_bstr_t(L"root\\wmi"), nullptr, nullptr,
                                     nullptr, 0, nullptr, nullptr, &services_);
        if (FAILED(hr)) {
            return false;
        }

        CoSetProxyBlanket(services_, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                          nullptr, RPC_C_AUTHN_LEVEL_CALL,
                          RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        return true;
    }

    // Lets a long enumeration give up when the engine is shutting down.
    void SetStopFlag(const std::atomic<bool>* stopping) { stopping_ = stopping; }

    IWbemServices* Services() const { return services_; }

   private:
    const std::atomic<bool>* stopping_ = nullptr;

   public:

    template <class Fn>
    void ForEach(const wchar_t* wql, Fn&& fn) {
        if (!services_) {
            return;
        }
        IEnumWbemClassObject* e = nullptr;
        HRESULT hr = services_->ExecQuery(
            _bstr_t(L"WQL"), _bstr_t(wql),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &e);
        if (FAILED(hr) || !e) {
            return;
        }
        // Bounded, not WBEM_INFINITE. A wedged or slow WMI provider is a
        // common enough failure, and this runs on the worker thread that
        // Wh_ModUninit joins -- an unbounded wait there hangs the unload, so
        // the mod can neither be disabled nor updated.
        for (;;) {
            if (stopping_ && stopping_->load()) {
                break;
            }
            IWbemClassObject* obj = nullptr;
            ULONG got = 0;
            HRESULT next = e->Next(1000, 1, &obj, &got);
            if (next == WBEM_S_TIMEDOUT) {
                continue;  // re-check the stop flag and keep waiting
            }
            if (next != S_OK || got != 1) {
                break;  // exhausted or failed
            }
            fn(obj);
            obj->Release();
        }
        e->Release();
    }

    bool SetBrightness(const std::wstring& objectPath, int percent) {
        if (!services_ || objectPath.empty()) {
            return false;
        }
        if (!brightnessClass_) {
            if (FAILED(services_->GetObject(
                    _bstr_t(L"WmiMonitorBrightnessMethods"), 0, nullptr,
                    &brightnessClass_, nullptr))) {
                return false;
            }
            if (FAILED(brightnessClass_->GetMethod(
                    L"WmiSetBrightness", 0, &setBrightnessInDef_, nullptr))) {
                return false;
            }
        }

        IWbemClassObject* in = nullptr;
        if (FAILED(setBrightnessInDef_->SpawnInstance(0, &in)) || !in) {
            return false;
        }

        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = 0;  // Timeout: apply immediately, never revert
        in->Put(L"Timeout", 0, &v, 0);
        VariantClear(&v);

        VariantInit(&v);
        v.vt = VT_UI1;
        v.bVal = static_cast<BYTE>(std::clamp(percent, 0, 100));
        in->Put(L"Brightness", 0, &v, 0);
        VariantClear(&v);

        HRESULT hr = services_->ExecMethod(
            _bstr_t(objectPath.c_str()), _bstr_t(L"WmiSetBrightness"), 0,
            nullptr, in, nullptr, nullptr);
        in->Release();
        return SUCCEEDED(hr);
    }

   private:
    IWbemLocator* locator_ = nullptr;
    IWbemServices* services_ = nullptr;
    IWbemClassObject* brightnessClass_ = nullptr;
    IWbemClassObject* setBrightnessInDef_ = nullptr;
};

class Engine {
   public:
    using LogFn = void (*)(const wchar_t*);

    Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    ~Engine() { Stop(); }

    void SetLogger(LogFn fn) { log_ = fn; }

    // Per-write logging is invaluable while debugging and pure noise in daily
    // use, so it is off unless asked for.

    void SetFollowMode(FollowMode mode) {
        std::lock_guard<std::mutex> lock(mutex_);
        followMode_ = mode;
    }

    void Start() {
        if (worker_.joinable()) {
            return;
        }
        quit_ = false;
        worker_ = std::thread(
            [this] { detail::RunGuarded(
                    L"WorkerMain", [this] { WorkerMain(); },
                    [this](const wchar_t* w, const wchar_t* what) {
                        Log(L"%ls threw: %ls", w, what);
                    }); });

        // Blocking on purpose, and safe because of when this is called.
        //
        // The first enumeration talks to WMI and does an I2C round trip per
        // external monitor, so it is not fast. That is fine here: the mod does
        // not call Start() from Wh_ModInit -- it waits until the host is up
        // (see StartEngineIfNeeded in the mod) precisely because touching COM
        // before then aborts the shell. By the time we get here the host has
        // finished starting and nothing of its is being delayed.
        //
        // Still bounded, so a wedged WMI connection cannot hang the caller.
        {
            std::unique_lock<std::mutex> lock(mutex_);
            ready_.wait_for(lock, std::chrono::seconds(5),
                            [this] { return enumerated_ || quit_; });
        }
    }

    void Stop() {
        // Set before the lock so anything already inside a long hardware or
        // WMI call can notice and unwind, rather than being noticed only when
        // it next comes back around to the loop condition.
        stopping_.store(true);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            quit_ = true;
        }
        work_.notify_all();
        ready_.notify_all();
        if (worker_.joinable()) {
            worker_.join();
        }
        if (eventThread_.joinable()) {
            eventThread_.join();
        }
    }

    // Snapshot for the UI. Ordered left-to-right by desktop position, so the
    // slider order matches how the monitors physically sit on the desk.
    std::vector<Display> GetDisplays() {
        std::lock_guard<std::mutex> lock(mutex_);
        return displays_;
    }

    // Non-blocking. Repeated calls for the same display collapse into a single
    // hardware write, so a slider drag can never outrun the I2C bus.
    // Setting a display explicitly -- the slider, or anything else the user
    // drove -- is what re-bases relative following on that display. Note that
    // a refresh deliberately does not: the hardware reports the clamped value,
    // so re-basing on a read would throw away the very offset followRaw_
    // exists to remember.
    void SetPercent(const std::wstring& stableId, int percent) {
        SetPercentInternal(stableId, percent, /*rebaseFollow=*/true);
    }

    // Contrast, coalesced exactly like brightness. Displays without contrast
    // ignore it.
    void SetContrast(const std::wstring& stableId, int percent) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            bool known = false;
            for (auto& d : displays_) {
                if (d.stableId == stableId && d.contrastMax > 0) {
                    d.contrast = std::clamp(percent, 0, 100);
                    known = true;
                }
            }
            if (!known) {
                return;
            }
            pendingContrast_[stableId] = std::clamp(percent, 0, 100);
        }
        work_.notify_all();
    }

    // Speaker volume, coalesced like brightness.
    void SetVolume(const std::wstring& stableId, int percent) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            Display* d = FindLocked(stableId);
            if (!d || d->volumeMax == 0) {
                return;
            }
            d->volume = std::clamp(percent, 0, 100);
            pendingVcp_[{stableId, kVcpVolume}] =
                static_cast<DWORD>((d->volume * d->volumeMax + 50) / 100);
        }
        work_.notify_all();
    }

    // Switches the monitor to one of the inputs it lists. Switching away from
    // the one this PC is on hands the monitor to whatever is on the other
    // input; whether it still answers DDC/CI from here afterwards depends on
    // the monitor.
    void SetInput(const std::wstring& stableId, int value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            Display* d = FindLocked(stableId);
            if (!d || std::find(d->inputs.begin(), d->inputs.end(), value) == d->inputs.end()) {
                return;
            }
            d->input = value;
            pendingVcp_[{stableId, kVcpInput}] = static_cast<DWORD>(value);
        }
        work_.notify_all();
    }

    // Turns a display off (DPM off) or back on. Off is 0x04 rather than 0x05:
    // 0x05 is the monitor's own power button, after which many monitors stop
    // listening on DDC/CI altogether, whereas one in DPM off usually still
    // answers the 0x01 that turns it back on. Usually -- some do not, and then
    // only the monitor's own button brings it back.
    void SetPower(const std::wstring& stableId, bool on) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            bool known = false;
            for (auto& d : displays_) {
                if (d.stableId == stableId && d.hasPower) {
                    d.poweredOff = !on;
                    known = true;
                }
            }
            if (!known) {
                return;
            }
            pendingPower_[stableId] = on;
        }
        work_.notify_all();
    }

   private:
    void SetPercentInternal(const std::wstring& stableId, int percent,
                            bool rebaseFollow) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (rebaseFollow) {
                followRaw_.erase(stableId);
            }
            // Stamped on request, not on completion: a drag keeps refreshing
            // this, so the whole drag plus its trailing echoes stay covered.
            lastRequest_[stableId] = std::chrono::steady_clock::now();
            pending_[stableId] = std::clamp(percent, 0, 100);
            // Reflect optimistically so the UI stays glued to the thumb.
            for (auto& d : displays_) {
                if (d.stableId == stableId) {
                    d.percent = std::clamp(percent, 0, 100);
                }
            }
        }
        work_.notify_all();
    }

   public:
    void RequestRescan() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            rescan_ = true;
        }
        work_.notify_all();
    }

    // Re-read what the hardware actually reports, for when brightness was
    // changed behind our back -- laptop function keys, the monitor's own OSD,
    // or another application.
    void RequestRefresh() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            refresh_ = true;
        }
        work_.notify_all();
    }

    // Called on the worker thread once the display list changed (structural =
    // true) or once values were re-read (false). The callee is responsible for
    // marshalling to whatever thread it needs.
    void SetOnChanged(std::function<void(bool)> fn) {
        std::lock_guard<std::mutex> lock(mutex_);
        onChanged_ = std::move(fn);
    }


   private:
    static constexpr BYTE kVcpLuminance = 0x10;
    static constexpr BYTE kVcpContrast = 0x12;
    static constexpr BYTE kVcpPower = 0xD6;
    static constexpr DWORD kPowerOn = 0x01;
    static constexpr DWORD kPowerOff = 0x04;  // DPM off; see SetPower
    static constexpr BYTE kVcpVolume = 0x62;
    static constexpr BYTE kVcpInput = 0x60;

    // Caller holds mutex_.
    Display* FindLocked(const std::wstring& stableId) {
        for (auto& d : displays_) {
            if (d.stableId == stableId) {
                return &d;
            }
        }
        return nullptr;
    }

    // What a monitor's capabilities string says about the VCP codes it
    // implements: each code, and the values it lists for it, if any.
    struct Capabilities {
        bool loaded = false;
        std::map<int, std::vector<int>> vcp;
    };

    static int HexDigit(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    // "...vcp(02 04 10 12 14(05 08 0B) 60(11 12 ) 62 8D)..." -> {0x02: {},
    // ..., 0x14: {5, 8, 11}, 0x60: {0x11, 0x12}, ...}. Codes are read two hex
    // digits at a time, which also covers the monitors that pack the list
    // with no spaces at all.
    static Capabilities ParseCapabilities(const std::string& text) {
        Capabilities caps;
        std::string lower = text;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](char c) { return static_cast<char>(tolower(static_cast<unsigned char>(c))); });
        size_t at = lower.find("vcp(");
        if (at == std::string::npos) {
            return caps;
        }
        size_t i = at + 4;
        auto readByte = [&](size_t& k) -> int {
            while (k < text.size() && text[k] == ' ') ++k;
            if (k + 1 < text.size() && HexDigit(text[k]) >= 0 && HexDigit(text[k + 1]) >= 0) {
                int v = HexDigit(text[k]) * 16 + HexDigit(text[k + 1]);
                k += 2;
                return v;
            }
            return -1;
        };
        while (i < text.size()) {
            while (i < text.size() && text[i] == ' ') ++i;
            if (i >= text.size() || text[i] == ')') {
                break;
            }
            int code = readByte(i);
            if (code < 0) {
                ++i;  // something unexpected; skip it rather than stop
                continue;
            }
            std::vector<int>& values = caps.vcp[code];
            while (i < text.size() && text[i] == ' ') ++i;
            if (i < text.size() && text[i] == '(') {
                ++i;
                int depth = 1;
                while (i < text.size() && depth > 0) {
                    while (i < text.size() && text[i] == ' ') ++i;
                    if (i >= text.size()) break;
                    if (text[i] == '(') {
                        ++depth;
                        ++i;
                    } else if (text[i] == ')') {
                        --depth;
                        ++i;
                    } else {
                        int v = readByte(i);
                        if (v < 0) {
                            ++i;
                        } else if (depth == 1) {
                            values.push_back(v);
                        }
                    }
                }
            }
        }
        caps.loaded = true;
        return caps;
    }

    static Capabilities ReadCapabilities(HANDLE hPhysical) {
        DWORD length = 0;
        if (!GetCapabilitiesStringLength(hPhysical, &length) || length == 0 ||
            length > 16384) {
            return {};
        }
        std::string text(length, '\0');
        if (!CapabilitiesRequestAndCapabilitiesReply(hPhysical, text.data(), length)) {
            return {};
        }
        text.resize(strnlen(text.c_str(), length));
        return ParseCapabilities(text);
    }

    // Caller holds mutex_, or owns d outright (Rescan, before publishing).
    static void ApplyCapabilities(Display& d, const Capabilities& caps) {
        auto input = caps.vcp.find(0x60);
        if (input != caps.vcp.end() && input->second.size() >= 2) {
            d.inputs = input->second;
        }
    }

    // The capabilities string takes a second or more per monitor, so it is
    // read once per display per session, after the displays are already up,
    // rather than as part of enumerating them. Worker thread; returns whether
    // any display changed.
    bool LoadCapabilities() {
        std::vector<std::pair<std::wstring, HANDLE>> todo;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& d : displays_) {
                if (d.transport == Transport::DdcCi && d.hPhysical &&
                    !caps_.count(d.stableId)) {
                    todo.emplace_back(d.stableId, d.hPhysical);
                }
            }
        }
        bool changed = false;
        for (const auto& entry : todo) {
            if (stopping_.load()) {
                break;
            }
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            Capabilities caps = ReadCapabilities(entry.second);
            caps_[entry.first] = caps;
            long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::steady_clock::now() - start)
                               .count();
            Log(L"capabilities %ls: %ls, %zu code(s) (%lld ms)", entry.first.c_str(),
                caps.loaded ? L"read" : L"unavailable", caps.vcp.size(), ms);
            if (caps.loaded) {
                std::lock_guard<std::mutex> lock(mutex_);
                if (Display* d = FindLocked(entry.first)) {
                    ApplyCapabilities(*d, caps);
                    changed = true;
                }
            }
        }
        return changed;
    }

    void Log(const wchar_t* fmt, ...) {
        if (!log_) {
            return;
        }
        wchar_t buf[512];
        va_list args;
        va_start(args, fmt);
        vswprintf(buf, sizeof(buf) / sizeof(buf[0]), fmt, args);
        va_end(args);
        log_(buf);
    }

    void WorkerMain() {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        // Deliberately no CoInitializeSecurity: it is process-wide, so calling
        // it would impose this thread's settings on the whole host.
        // CoSetProxyBlanket on the IWbemServices proxy is what actually governs
        // our WMI calls.

        // Created here, not as a plain member: these interface pointers belong
        // to this thread's apartment and must not outlive it. Releasing them
        // from ~Engine() on another thread, after the CoUninitialize() below,
        // is a crash.
        wmi_ = std::make_unique<WmiSession>();
        wmi_->SetStopFlag(&stopping_);
        wmi_->Init();

        Rescan();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            enumerated_ = true;
        }
        ready_.notify_all();
        NotifyChanged(true);

        // After the panel has its rows, not before: this is the slow part.
        if (LoadCapabilities()) {
            NotifyChanged(true);
        }

        // Only worth a thread if something here reports brightness events;
        // DDC/CI has no equivalent, so this is the internal panel only.
        bool haveWmiPanel = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& d : displays_) {
                if (d.transport == Transport::Wmi) {
                    haveWmiPanel = true;
                    break;
                }
            }
        }
        if (haveWmiPanel) {
            eventThread_ = std::thread([this] {
                detail::RunGuarded(
                    L"EventThreadMain", [this] { EventThreadMain(); },
                    [this](const wchar_t* w, const wchar_t* what) {
                        Log(L"%ls threw: %ls", w, what);
                    });
            });
        }

        for (;;) {
            std::map<std::wstring, int> batch;
            std::map<std::wstring, int> contrastBatch;
            std::map<std::wstring, bool> powerBatch;
            std::map<std::pair<std::wstring, BYTE>, DWORD> vcpBatch;
            bool doRescan = false;
            bool doRefresh = false;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                work_.wait(lock, [this] {
                    return quit_ || rescan_ || refresh_ || !pending_.empty() ||
                           !pendingContrast_.empty() || !pendingPower_.empty() ||
                           !pendingVcp_.empty();
                });
                if (quit_) {
                    break;
                }
                doRescan = std::exchange(rescan_, false);
                batch.swap(pending_);
                contrastBatch.swap(pendingContrast_);
                powerBatch.swap(pendingPower_);
                vcpBatch.swap(pendingVcp_);
                // Never read back while writes are still queued: the value in
                // flight has not reached the panel yet, so reading now would
                // yank the slider backwards under the user's finger. refresh_
                // stays set and we come back to it once the queue drains.
                doRefresh = batch.empty() && contrastBatch.empty() &&
                            powerBatch.empty() && vcpBatch.empty() && refresh_;
                if (doRefresh) {
                    refresh_ = false;
                }
            }

            if (doRescan) {
                Rescan();
                NotifyChanged(true);
                if (LoadCapabilities()) {
                    NotifyChanged(true);
                }
            }

            // Each write blocks for tens of ms. Anything the UI posts while
            // we are on the wire lands in pending_ and overwrites its
            // predecessor, so we always resume with the newest value.
            bool wroteDdc = false;
            // Power first, so a display being switched back on is on before
            // anything else is written to it.
            for (const auto& entry : powerBatch) {
                if (ApplyPower(entry.first, entry.second)) {
                    wroteDdc = true;
                }
            }
            for (const auto& entry : batch) {
                if (Apply(entry.first, entry.second) == Transport::DdcCi) {
                    wroteDdc = true;
                }
            }
            for (const auto& entry : contrastBatch) {
                if (ApplyContrast(entry.first, entry.second)) {
                    wroteDdc = true;
                }
            }
            for (const auto& entry : vcpBatch) {
                if (ApplyVcp(entry.first.first, entry.first.second, entry.second)) {
                    wroteDdc = true;
                }
            }

            // Breathing room after an I2C write, and it is not politeness.
            //
            // A DDC/CI transaction serialises against the display driver, and
            // while one is on the wire the shell's UI thread stalls. Writing
            // back-to-back for the whole of a slider drag -- which is what
            // coalescing alone will happily do, one write every ~60 ms --
            // starves the slider of pointer input, so the thumb falls behind
            // the cursor and stops short. Drag quickly to the right-hand end
            // and you land somewhere in the sixties, which reads as the value
            // "drifting" on its own.
            //
            // So cap the write rate and let the queue coalesce in the gap. The
            // newest value always wins, so the value the user let go of is
            // still the one that lands, just up to kDdcCooldown later.
            if (wroteDdc) {
                std::unique_lock<std::mutex> lock(mutex_);
                work_.wait_for(lock, kDdcCooldown, [this] { return quit_; });
            }

            if (doRefresh) {
                RefreshValues();
                NotifyChanged(false);

                // Opening the panel is the moment a missing slider is
                // noticed, and it is also the only regular event this engine
                // gets -- nothing polls. So that is where an expired DDC/CI
                // verdict is retried, and only if one has actually expired,
                // which is a map lookup per display in the common case.
                if (AnyDdcRetryDue()) {
                    Rescan();
                    NotifyChanged(true);
                }
            }
        }

        ReleasePhysicalMonitors();

        // Tear down the WMI interfaces on the thread that created them, while
        // the apartment is still alive.
        wmi_.reset();

        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    }

    void NotifyChanged(bool structural) {
        std::function<void(bool)> fn;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            fn = onChanged_;
        }
        if (!fn) {
            return;
        }
        try {
            fn(structural);
        } catch (...) {
            // Must never escape: this runs on a worker thread, and an
            // unhandled exception there is std::terminate -> the host aborts.
            Log(L"change callback threw; ignored");
        }
    }

    // Re-reads the level each display actually reports. Unlike Rescan this
    // keeps the existing DDC handles and display list, so it costs one I2C
    // round trip per external monitor and nothing structural changes.
    void RefreshValues() {
        struct Target {
            std::wstring id;
            Transport transport;
            HANDLE hPhysical;
            std::wstring wmiKey;
            bool hasContrast;
            bool hasPower;
            bool hasVolume;
            std::vector<int> inputs;
        };

        std::vector<Target> targets;
        bool needWmi = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& d : displays_) {
                targets.push_back({d.stableId, d.transport, d.hPhysical,
                                   detail::ToLower(d.stableId),
                                   d.contrastMax > 0, d.hasPower,
                                   d.volumeMax > 0, d.inputs});
                if (d.transport == Transport::Wmi) {
                    needWmi = true;
                }
            }
        }

        std::map<std::wstring, WmiFacts> facts;
        if (needWmi) {
            facts = CollectWmiFacts();
        }

        std::vector<std::pair<std::wstring, int>> updates;
        std::vector<std::pair<std::wstring, int>> contrastUpdates;
        std::vector<std::pair<std::wstring, bool>> powerUpdates;
        std::vector<std::pair<std::wstring, int>> volumeUpdates;
        std::vector<std::pair<std::wstring, int>> inputUpdates;
        for (const Target& target : targets) {
            int percent = -1;
            switch (target.transport) {
                case Transport::DdcCi: {
                    MC_VCP_CODE_TYPE type{};
                    DWORD current = 0, maximum = 0;
                    if (GetVCPFeatureAndVCPFeatureReply(target.hPhysical,
                                                        kVcpLuminance, &type,
                                                        &current, &maximum) &&
                        maximum > 0) {
                        percent = static_cast<int>((current * 100 + maximum / 2) /
                                                   maximum);
                    }
                    if (target.hasContrast) {
                        current = maximum = 0;
                        if (GetVCPFeatureAndVCPFeatureReply(
                                target.hPhysical, kVcpContrast, &type, &current,
                                &maximum) &&
                            maximum > 0) {
                            contrastUpdates.emplace_back(
                                target.id,
                                static_cast<int>((current * 100 + maximum / 2) /
                                                 maximum));
                        }
                    }
                    if (target.hasPower) {
                        current = maximum = 0;
                        if (GetVCPFeatureAndVCPFeatureReply(
                                target.hPhysical, kVcpPower, &type, &current,
                                &maximum) &&
                            current >= 1 && current <= 5) {
                            powerUpdates.emplace_back(target.id, current >= 4);
                        }
                    }
                    if (target.hasVolume) {
                        current = maximum = 0;
                        if (GetVCPFeatureAndVCPFeatureReply(target.hPhysical, kVcpVolume,
                                                            &type, &current, &maximum) &&
                            maximum > 0) {
                            volumeUpdates.emplace_back(
                                target.id,
                                static_cast<int>((current * 100 + maximum / 2) / maximum));
                        }
                    }
                    if (!target.inputs.empty()) {
                        current = maximum = 0;
                        // A value outside the monitor's own list (the Samsung
                        // G32 answers 5) is not an input; keep what was set.
                        if (GetVCPFeatureAndVCPFeatureReply(target.hPhysical, kVcpInput,
                                                            &type, &current, &maximum) &&
                            std::find(target.inputs.begin(), target.inputs.end(),
                                      static_cast<int>(current)) != target.inputs.end()) {
                            inputUpdates.emplace_back(target.id, static_cast<int>(current));
                        }
                    }
                    break;
                }
                case Transport::Wmi: {
                    auto it = facts.find(target.wmiKey);
                    if (it != facts.end()) {
                        percent = it->second.currentPercent;
                    }
                    break;
                }
                case Transport::None:
                    break;
            }
            if (percent >= 0) {
                updates.emplace_back(target.id, percent);
            }
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& update : updates) {
                for (auto& d : displays_) {
                    if (d.stableId == update.first) {
                        d.percent = update.second;
                        break;
                    }
                }
            }
            for (const auto& update : contrastUpdates) {
                for (auto& d : displays_) {
                    if (d.stableId == update.first) {
                        d.contrast = update.second;
                        break;
                    }
                }
            }
            for (const auto& update : powerUpdates) {
                if (Display* d = FindLocked(update.first)) {
                    d->poweredOff = update.second;
                }
            }
            for (const auto& update : volumeUpdates) {
                if (Display* d = FindLocked(update.first)) {
                    d->volume = update.second;
                }
            }
            for (const auto& update : inputUpdates) {
                if (Display* d = FindLocked(update.first)) {
                    d->input = update.second;
                }
            }
        }

        Log(L"refreshed %zu display value(s)", updates.size());
    }

    // Windows raises WmiMonitorBrightnessEvent whenever the internal panel's
    // brightness changes -- function keys, power policy, anything. This is how
    // the stock slider stays live, and it beats polling. External monitors have
    // no counterpart: DDC/CI cannot report anything the host did not ask for.
    void EventThreadMain() {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        {
            WmiSession session;
            session.SetStopFlag(&stopping_);
            if (!session.Init() || !session.Services()) {
                Log(L"brightness events: no WMI connection");
            } else {
                IEnumWbemClassObject* events = nullptr;
                HRESULT hr = session.Services()->ExecNotificationQuery(
                    _bstr_t(L"WQL"),
                    _bstr_t(L"SELECT * FROM WmiMonitorBrightnessEvent"),
                    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                    nullptr, &events);

                if (FAILED(hr) || !events) {
                    Log(L"brightness events unavailable: %08X",
                        static_cast<unsigned>(hr));
                } else {
                    Log(L"listening for brightness events");
                    for (;;) {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            if (quit_) {
                                break;
                            }
                        }

                        IWbemClassObject* obj = nullptr;
                        ULONG got = 0;
                        // Short timeout so Stop() is not kept waiting.
                        HRESULT next = events->Next(500, 1, &obj, &got);
                        // Failure first. Any hard failure -- WMI restarting,
                        // WBEM_E_TRANSPORT_FAILURE, WBEM_E_CALL_CANCELLED --
                        // also comes back with got == 0, so testing the
                        // timeout case first would swallow it and spin on a
                        // permanently failing Next, burning a core inside the
                        // shell until the mod is disabled.
                        if (FAILED(next)) {
                            Log(L"brightness events: Next failed %08X; "
                                L"stopping the listener",
                                static_cast<unsigned>(next));
                            break;
                        }
                        if (next == WBEM_S_TIMEDOUT || got != 1 || !obj) {
                            continue;
                        }

                        std::wstring instance =
                            detail::GetStrProp(obj, L"InstanceName");
                        int level = detail::GetIntProp(obj, L"Brightness");
                        obj->Release();

                        if (!instance.empty() && level >= 0) {
                            OnBrightnessEvent(instance, level);
                        }
                    }
                    events->Release();
                }
            }
        }  // session released here, inside its own apartment

        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    }

    void OnBrightnessEvent(const std::wstring& instanceName, int percent) {
        std::wstring key = instanceName;
        if (key.size() > 2 && key.compare(key.size() - 2, 2, L"_0") == 0) {
            key.erase(key.size() - 2);
        }
        key = detail::ToLower(key);

        std::wstring changedId;
        std::vector<std::pair<std::wstring, int>> follow;
        {
            std::lock_guard<std::mutex> lock(mutex_);

            Display* target = nullptr;
            for (auto& d : displays_) {
                if (detail::ToLower(d.stableId) == key) {
                    target = &d;
                    break;
                }
            }
            if (!target || target->percent == percent) {
                return;
            }

            // Our own writes raise this event too, and matching on the value
            // is not enough: coalescing means the event for a value we wrote
            // can arrive after we have already written a newer one, so the
            // values disagree and the echo looks like an external change.
            // Following would then compute a delta against the wrong baseline
            // and shove the other monitors around at random.
            //
            // So the test is per display and purely temporal. This does not
            // swallow the keypresses we want to follow: those change the
            // internal panel, which we never write to in response -- following
            // only ever writes to the *other* displays.
            auto requested = lastRequest_.find(target->stableId);
            if (requested != lastRequest_.end() &&
                std::chrono::steady_clock::now() - requested->second <
                    std::chrono::milliseconds(1500)) {
                return;
            }

            int previous = target->percent;
            changedId = target->stableId;
            target->percent = percent;

            if (followMode_ != FollowMode::Off) {
                int delta = (previous >= 0) ? percent - previous : 0;
                for (auto& d : displays_) {
                    if (d.stableId == changedId ||
                        d.transport == Transport::None) {
                        continue;
                    }

                    int want;
                    if (followMode_ == FollowMode::Match) {
                        want = percent;
                        // Nothing to accumulate in match mode, and leaving a
                        // stale accumulator behind would make a later switch
                        // back to relative jump.
                        followRaw_.erase(d.stableId);
                    } else {
                        // Clamping the *stored* value destroys the offset this
                        // mode exists to preserve: panel 50 -> 10 with a
                        // monitor at 30 stores 0, and panel 10 -> 50 then
                        // gives 40 rather than 30. Every trip to an end stop
                        // used to shift that monitor permanently. So the
                        // accumulator runs unclamped past the ends and only
                        // what goes to the hardware is clamped.
                        auto seen = followRaw_.find(d.stableId);
                        int raw = (seen != followRaw_.end())
                                      ? seen->second
                                      : ((d.percent < 0) ? percent : d.percent);
                        // Bounded so a long run against an end stop cannot
                        // wander arbitrarily far and take many keypresses to
                        // come back.
                        raw = std::clamp(raw + delta, -100, 200);
                        followRaw_[d.stableId] = raw;
                        want = std::clamp(raw, 0, 100);
                    }

                    if (want != d.percent) {
                        follow.emplace_back(d.stableId, want);
                    }
                }
            }
        }

        Log(L"brightness event: %ls is now %d%%", instanceName.c_str(), percent);

        // Outside the lock: SetPercentInternal takes it. Not the public
        // SetPercent -- that re-bases following, which would erase the
        // accumulator computed just above.
        for (const auto& entry : follow) {
            SetPercentInternal(entry.first, entry.second,
                               /*rebaseFollow=*/false);
        }

        NotifyChanged(false);
    }

    void ReleasePhysicalMonitors() {
        // The handles come out under the lock; the destroying happens without
        // it. This is the same mutex the XAML thread takes in GetDisplays()
        // and in SetPercent() from the slider's ValueChanged, so holding it
        // across DestroyPhysicalMonitors let a slow monitor teardown during a
        // rescan stall the shell's UI thread.
        std::vector<HANDLE> handles;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto& d : displays_) {
                if (d.hPhysical) {
                    handles.push_back(d.hPhysical);
                    d.hPhysical = nullptr;
                }
            }
        }
        for (HANDLE h : handles) {
            PHYSICAL_MONITOR pm{};
            pm.hPhysicalMonitor = h;
            DestroyPhysicalMonitors(1, &pm);
        }
    }

    struct WmiFacts {
        std::wstring name;
        std::wstring methodsPath;
        int currentPercent = -1;
        bool hasBrightnessMethods = false;
    };

    std::map<std::wstring, WmiFacts> CollectWmiFacts() {
        std::map<std::wstring, WmiFacts> facts;
        if (!wmi_) {
            return facts;
        }

        auto key = [](std::wstring instanceName) {
            // "DISPLAY\BOE0AAE\4&..._0" -> "display\boe0aae\4&..."
            if (instanceName.size() > 2 &&
                instanceName.compare(instanceName.size() - 2, 2, L"_0") == 0) {
                instanceName.erase(instanceName.size() - 2);
            }
            return detail::ToLower(instanceName);
        };

        wmi_->ForEach(L"SELECT * FROM WmiMonitorID", [&](IWbemClassObject* o) {
            std::wstring inst = detail::GetStrProp(o, L"InstanceName");
            if (!inst.empty()) {
                facts[key(inst)].name =
                    detail::GetU16ArrayProp(o, L"UserFriendlyName");
            }
        });

        wmi_->ForEach(L"SELECT * FROM WmiMonitorBrightness",
                     [&](IWbemClassObject* o) {
                         std::wstring inst =
                             detail::GetStrProp(o, L"InstanceName");
                         if (!inst.empty()) {
                             facts[key(inst)].currentPercent =
                                 detail::GetIntProp(o, L"CurrentBrightness");
                         }
                     });

        wmi_->ForEach(L"SELECT * FROM WmiMonitorBrightnessMethods",
                     [&](IWbemClassObject* o) {
                         std::wstring inst =
                             detail::GetStrProp(o, L"InstanceName");
                         if (!inst.empty()) {
                             WmiFacts& f = facts[key(inst)];
                             f.hasBrightnessMethods = true;
                             f.methodsPath = detail::GetStrProp(o, L"__RELPATH");
                         }
                     });

        return facts;
    }

    static BOOL CALLBACK EnumProc(HMONITOR h, HDC, LPRECT, LPARAM data) {
        reinterpret_cast<std::vector<HMONITOR>*>(data)->push_back(h);
        return TRUE;
    }

    // How long to leave a failed DDC/CI probe alone: 30s, then a minute, then
    // doubling to a 16-minute ceiling. Short enough that a monitor which was
    // merely asleep comes back on its own; long enough that a monitor which
    // genuinely has no DDC/CI is not costing seconds per rescan.
    static std::chrono::steady_clock::duration DdcRetryDelay(int failures) {
        int shift = failures - 1;
        if (shift < 0) {
            shift = 0;
        } else if (shift > 5) {
            shift = 5;
        }
        return std::chrono::seconds(30) * (1 << shift);
    }

    // Whether any display currently written off as uncontrollable is due
    // another probe. Worker thread.
    bool AnyDdcRetryDue() {
        const std::chrono::steady_clock::time_point now =
            std::chrono::steady_clock::now();
        std::vector<Display> snapshot;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            snapshot = displays_;
        }
        for (const Display& d : snapshot) {
            if (d.transport != Transport::None) {
                continue;
            }
            std::map<std::wstring, DdcVerdict>::const_iterator known =
                ddcAnswered_.find(d.stableId);
            if (known != ddcAnswered_.end() && !known->second.answered &&
                (now - known->second.taken) >=
                    DdcRetryDelay(known->second.failures)) {
                return true;
            }
        }
        return false;
    }

    void Rescan() {
        ReleasePhysicalMonitors();

        std::vector<HMONITOR> handles;
        EnumDisplayMonitors(nullptr, nullptr, &Engine::EnumProc,
                            reinterpret_cast<LPARAM>(&handles));

        std::map<std::wstring, WmiFacts> facts = CollectWmiFacts();
        std::vector<Display> found;

        for (HMONITOR h : handles) {
            // Probing a monitor that does not answer DDC/CI can take seconds,
            // and the loop is otherwise uninterruptible, so an unload landing
            // mid-rescan would wait for all of them.
            if (stopping_.load()) {
                break;
            }
            MONITORINFOEXW mi{};
            mi.cbSize = sizeof(mi);
            if (!GetMonitorInfoW(h, &mi)) {
                continue;
            }

            Display d;
            d.rect = mi.rcMonitor;

            DISPLAY_DEVICEW dd{};
            dd.cb = sizeof(dd);
            if (EnumDisplayDevicesW(mi.szDevice, 0, &dd,
                                    EDD_GET_DEVICE_INTERFACE_NAME)) {
                d.stableId = detail::DeviceInterfaceToInstanceId(dd.DeviceID);
            }
            if (d.stableId.empty()) {
                d.stableId = mi.szDevice;  // last resort, not hotplug-stable
            }

            DISPLAY_DEVICEW ddName{};
            ddName.cb = sizeof(ddName);
            std::wstring gdiName;
            if (EnumDisplayDevicesW(mi.szDevice, 0, &ddName, 0)) {
                gdiName = ddName.DeviceString;
            }

            std::map<std::wstring, WmiFacts>::const_iterator it =
                facts.find(detail::ToLower(d.stableId));
            const WmiFacts* f = (it != facts.end()) ? &it->second : nullptr;

            if (f && !f->name.empty()) {
                d.name = f->name;
            }

            // DDC/CI first: it is the only option for external panels, and on
            // a laptop it fails fast (ERROR_GEN_FAILURE) for the internal one.
            // Probing DDC/CI is not free: a monitor or dock that does not
            // answer can take seconds to fail, and that cost is paid again on
            // every rescan -- every hotplug, every WM_DISPLAYCHANGE. So a
            // failure is remembered per EDID id.
            //
            // Remembered, not final. "Did not answer" is not always a property
            // of the monitor: one asleep at sign-in, behind a dock or KVM that
            // is still enumerating, or switched to another input all fail the
            // first probe and would then be written off as uncontrollable for
            // the rest of the session -- no slider, and no way to get one back
            // short of reloading the mod. So a failed verdict expires, on a
            // delay that doubles each time, and a display that goes away drops
            // its verdict entirely (below) so a replug starts over.
            std::map<std::wstring, DdcVerdict>::const_iterator known =
                ddcAnswered_.find(d.stableId);
            const std::chrono::steady_clock::time_point now =
                std::chrono::steady_clock::now();
            bool probe = true;
            if (known != ddcAnswered_.end() && !known->second.answered) {
                probe = (now - known->second.taken) >=
                        DdcRetryDelay(known->second.failures);
            }
            bool attached = false;
            if (probe) {
                attached = TryAttachDdcCi(h, &d);
                DdcVerdict& v = ddcAnswered_[d.stableId];
                v.failures = attached ? 0 : v.failures + 1;
                v.answered = attached;
                v.taken = now;
            }

            if (attached) {
                if (d.name.empty()) {
                    d.name = gdiName.empty() ? L"External display" : gdiName;
                }
            } else if (f && f->hasBrightnessMethods) {
                d.transport = Transport::Wmi;
                d.wmiPath = f->methodsPath;
                d.percent = f->currentPercent;
                if (d.name.empty()) {
                    d.name = L"Built-in display";
                }
            } else {
                d.transport = Transport::None;
                if (d.name.empty()) {
                    d.name = gdiName.empty() ? L"Display" : gdiName;
                }
            }

            found.push_back(std::move(d));
        }

        // Left-to-right, so slider order matches the physical layout.
        std::sort(found.begin(), found.end(),
                  [](const Display& a, const Display& b) {
                      if (a.rect.left != b.rect.left) {
                          return a.rect.left < b.rect.left;
                      }
                      return a.rect.top < b.rect.top;
                  });

        // A display that is gone takes its DDC/CI verdict with it, so
        // unplugging and replugging a monitor that failed its first probe
        // gets a fresh one rather than inheriting the old answer.
        for (auto it = ddcAnswered_.begin(); it != ddcAnswered_.end();) {
            bool present = false;
            for (const Display& d : found) {
                if (d.stableId == it->first) {
                    present = true;
                    break;
                }
            }
            it = present ? std::next(it) : ddcAnswered_.erase(it);
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            // Drop accumulators for displays that are no longer here, so a
            // monitor that comes back does not inherit an offset from an
            // earlier session of itself.
            for (auto it = followRaw_.begin(); it != followRaw_.end();) {
                bool present = false;
                for (const auto& d : found) {
                    if (d.stableId == it->first) {
                        present = true;
                        break;
                    }
                }
                it = present ? std::next(it) : followRaw_.erase(it);
            }

            displays_ = std::move(found);
        }
    }

    // Deliberately does NOT call GetMonitorCapabilities: plenty of monitors
    // (the Samsung G32 among them) fail it with 0xC0262C07 while answering raw
    // VCP reads and writes perfectly well.
    bool TryAttachDdcCi(HMONITOR h, Display* d) {
        DWORD count = 0;
        if (!GetNumberOfPhysicalMonitorsFromHMONITOR(h, &count) || count == 0) {
            return false;
        }

        std::vector<PHYSICAL_MONITOR> physical(count);
        if (!GetPhysicalMonitorsFromHMONITOR(h, count, physical.data())) {
            return false;
        }

        // A single HMONITOR maps to more than one physical monitor only in
        // clone/daisy-chain setups; the first one that answers wins.
        bool attached = false;
        for (DWORD i = 0; i < count; ++i) {
            MC_VCP_CODE_TYPE type{};
            DWORD current = 0, maximum = 0;
            if (!attached &&
                GetVCPFeatureAndVCPFeatureReply(physical[i].hPhysicalMonitor,
                                                kVcpLuminance, &type, &current,
                                                &maximum) &&
                maximum > 0) {
                d->transport = Transport::DdcCi;
                d->hPhysical = physical[i].hPhysicalMonitor;
                d->vcpMax = maximum;
                d->percent =
                    static_cast<int>((current * 100 + maximum / 2) / maximum);
                ProbeExtras(d);
                attached = true;
                continue;  // keep this handle alive
            }
            DestroyPhysicalMonitors(1, &physical[i]);
        }

        return attached;
    }

    // Contrast and power are extras on the handle that answered brightness.
    // A monitor that does not implement one fails the read (or answers it
    // with nothing usable), which costs one I2C round trip and leaves that
    // extra off.
    void ProbeExtras(Display* d) {
        MC_VCP_CODE_TYPE type{};
        DWORD current = 0, maximum = 0;
        if (GetVCPFeatureAndVCPFeatureReply(d->hPhysical, kVcpContrast, &type,
                                            &current, &maximum) &&
            maximum > 0) {
            d->contrastMax = maximum;
            d->contrast =
                static_cast<int>((current * 100 + maximum / 2) / maximum);
        }
        // 1 on, 2 standby, 3 suspend, 4 off, 5 off by the power button. A
        // monitor that answers 0 -- the Samsung G32 does -- has no power
        // control to offer, however willingly it answers the read.
        current = maximum = 0;
        if (GetVCPFeatureAndVCPFeatureReply(d->hPhysical, kVcpPower, &type,
                                            &current, &maximum) &&
            current >= 1 && current <= 5) {
            d->hasPower = true;
            d->poweredOff = current >= 4;
        }
        current = maximum = 0;
        if (GetVCPFeatureAndVCPFeatureReply(d->hPhysical, kVcpVolume, &type, &current,
                                            &maximum) &&
            maximum > 0) {
            d->volumeMax = maximum;
            d->volume = static_cast<int>((current * 100 + maximum / 2) / maximum);
        }
        // Capabilities already read this session carry over to a rescan.
        auto caps = caps_.find(d->stableId);
        if (caps != caps_.end() && caps->second.loaded) {
            ApplyCapabilities(*d, caps->second);
        }
    }

    // Looks up the DDC/CI handle for a display that has the given extra.
    // Worker thread.
    HANDLE DdcHandleFor(const std::wstring& stableId, DWORD* contrastMax,
                        bool* hasPower) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& d : displays_) {
            if (d.stableId == stableId && d.transport == Transport::DdcCi) {
                *contrastMax = d.contrastMax;
                *hasPower = d.hasPower;
                return d.hPhysical;
            }
        }
        return nullptr;
    }

    // Returns whether anything went on the wire, for the DDC/CI cooldown.
    bool ApplyContrast(const std::wstring& stableId, int percent) {
        DWORD contrastMax = 0;
        bool hasPower = false;
        HANDLE hPhysical = DdcHandleFor(stableId, &contrastMax, &hasPower);
        if (!hPhysical || contrastMax == 0) {
            return false;
        }
        std::chrono::steady_clock::time_point start =
            std::chrono::steady_clock::now();
        DWORD raw = static_cast<DWORD>(
            (static_cast<DWORD>(percent) * contrastMax + 50) / 100);
        bool ok = SetVCPFeature(hPhysical, kVcpContrast, raw) != FALSE;
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - start)
                           .count();
        Log(L"contrast %ls -> %d%% ok=%d (%lld ms)", stableId.c_str(), percent,
            ok ? 1 : 0, ms);
        return true;
    }

    bool ApplyVcp(const std::wstring& stableId, BYTE code, DWORD raw) {
        DWORD contrastMax = 0;
        bool hasPower = false;
        HANDLE hPhysical = DdcHandleFor(stableId, &contrastMax, &hasPower);
        if (!hPhysical) {
            return false;
        }
        bool ok = SetVCPFeature(hPhysical, code, raw) != FALSE;
        Log(L"vcp %02X %ls -> %lu ok=%d", code, stableId.c_str(), raw, ok ? 1 : 0);
        return true;
    }

    bool ApplyPower(const std::wstring& stableId, bool on) {
        DWORD contrastMax = 0;
        bool hasPower = false;
        HANDLE hPhysical = DdcHandleFor(stableId, &contrastMax, &hasPower);
        if (!hPhysical || !hasPower) {
            return false;
        }
        bool ok = SetVCPFeature(hPhysical, kVcpPower, on ? kPowerOn : kPowerOff) !=
                  FALSE;
        Log(L"power %ls -> %ls ok=%d", stableId.c_str(), on ? L"on" : L"off",
            ok ? 1 : 0);
        return true;
    }

    Transport Apply(const std::wstring& stableId, int percent) {
        HANDLE hPhysical = nullptr;
        DWORD vcpMax = 100;
        Transport transport = Transport::None;
        std::wstring wmiPath;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& d : displays_) {
                if (d.stableId == stableId) {
                    transport = d.transport;
                    hPhysical = d.hPhysical;
                    vcpMax = d.vcpMax;
                    wmiPath = d.wmiPath;
                    break;
                }
            }
        }

        std::chrono::steady_clock::time_point start =
            std::chrono::steady_clock::now();
        bool ok = false;

        switch (transport) {
            case Transport::DdcCi: {
                // Map the percentage onto the monitor's own scale, which is
                // often not 0-100.
                DWORD raw = static_cast<DWORD>(
                    (static_cast<DWORD>(percent) * vcpMax + 50) / 100);
                ok = SetVCPFeature(hPhysical, kVcpLuminance, raw) != FALSE;
                break;
            }
            case Transport::Wmi:
                ok = wmi_ && wmi_->SetBrightness(wmiPath, percent);
                break;
            case Transport::None:
                break;
        }

        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - start)
                           .count();
        // Unconditional: Windhawk's own per-mod logging switch already gates
        // whether any of this is emitted, and it is off by default.
        Log(L"apply %ls -> %d%% ok=%d (%lld ms)", stableId.c_str(), percent,
            ok ? 1 : 0, ms);
        return transport;
    }

    std::thread worker_;
    std::thread eventThread_;
    std::map<std::wstring, std::chrono::steady_clock::time_point> lastRequest_;
    // What a DDC/CI probe last said about a display, and when.
    //
    // Worker thread only, so it needs no lock.
    struct DdcVerdict {
        bool answered = false;
        int failures = 0;
        std::chrono::steady_clock::time_point taken{};
    };
    std::map<std::wstring, DdcVerdict> ddcAnswered_;
    FollowMode followMode_ = FollowMode::Off;
    std::atomic<bool> stopping_{false};
    std::mutex mutex_;
    std::condition_variable work_;
    std::condition_variable ready_;
    std::map<std::wstring, int> pending_;
    std::map<std::wstring, int> pendingContrast_;
    std::map<std::wstring, bool> pendingPower_;
    std::map<std::pair<std::wstring, BYTE>, DWORD> pendingVcp_;
    // Capabilities read this session, by display. Worker thread only.
    std::map<std::wstring, Capabilities> caps_;

    // Where relative following thinks each display would be if brightness had
    // no end stops. Guarded by mutex_.
    std::map<std::wstring, int> followRaw_;
    std::vector<Display> displays_;
    std::unique_ptr<WmiSession> wmi_;
    LogFn log_ = nullptr;
    std::function<void(bool)> onChanged_;
    bool quit_ = false;
    bool rescan_ = false;
    bool refresh_ = false;
    bool enumerated_ = false;
};

}  // namespace brightness

// ===========================================================================
// Mod state
// ===========================================================================

namespace {

brightness::Engine* g_engine = nullptr;

std::atomic<bool> g_engineStarted{false};

// The engine may not touch COM until the host has finished starting.
//
// Connecting to WMI activates a COM object, and the first activation in a
// process implicitly initialises COM security process-wide. Done from
// Wh_ModInit -- which runs before ShellHost's own startup code -- we win that
// race, ShellHost's own CoInitializeSecurity then fails RPC_E_TOO_LATE, and it
// fast-fails. The host aborts, restarts, and does it again, so the shell never
// comes back. Not throwing our own CoInitializeSecurity call is not enough;
// any activation is sufficient.
//
// Waiting until a XAML window exists means the host is past that point.
// Joined in Wh_ModUninit, not detached: Start() is still inside the engine
// when it runs, and Windhawk frees this DLL the moment uninit returns.
[[clang::no_destroy]] std::optional<std::thread> g_engineStarter;
// The flag alone would leave a gap: a starter that had claimed the flag but
// not yet assigned the thread would be invisible to a join in uninit, and
// would then run on into a freed DLL. The mutex closes it.
std::mutex g_engineStarterMutex;
bool g_engineStarterSpawned = false;

void StartEngineIfNeeded() {
    if (!g_engine || g_engineStarted.exchange(true)) {
        return;
    }
    g_engine->Start();
    Wh_Log(L"engine started (%d display(s))",
           static_cast<int>(g_engine->GetDisplays().size()));
}

// Called from the XAML thread when the Control Center turns up, so it must not
// block there -- the first enumeration can take seconds.
void StartEngineAsync() {
    std::lock_guard<std::mutex> lock(g_engineStarterMutex);
    if (g_engineStarterSpawned) {
        return;
    }
    g_engineStarterSpawned = true;
    g_engineStarter.emplace([] { StartEngineIfNeeded(); });
}

// How long a ControlCenterView lives, since three things here depend on it and
// they used to each say something different.
//
// It is not built at shell start, and it is not rebuilt on every open. It is
// built the first time Quick Settings is shown and then kept, shown and hidden,
// across subsequent opens -- which is why a second open is instant and why the
// first layout pass a live view runs is the one on *close*. Leave it closed for
// a while, though, and the shell discards it; the next open constructs a new
// one. That is the reported "flicker after a while, none if I just opened it",
// and it is why the injection bookkeeping has to prune dead trees rather than
// assume one view forever.
//
// Guards against double-injection within one of those constructions.
std::atomic<bool> g_injecting{false};

// Injection is retried on every layout pass until it takes, so the diagnostic
// tree dump has to be one-shot or it would flood the log.
std::atomic<bool> g_dumpedTree{false};

// Everything we added to somebody else's visual tree, so that disabling the
// mod can put that tree back the way we found it. This is not cosmetic: the
// slider's ValueChanged handler is code inside this DLL, so a slider left
// behind after unload would call into freed memory the moment it is dragged.
struct PanelLinks;
struct RowTitle;

struct Injection {
    // Weak, so a closed Control Center can still be collected.
    winrt::weak_ref<wuxc::Grid> grid;
    winrt::weak_ref<wuxc::StackPanel> panel;
    // Strong, and deliberately so: XAML only keeps projection peers alive for
    // elements in the live visual tree. A RowDefinition is not a UIElement, so
    // a weak ref to it is always dead by teardown even though the row itself is
    // still sitting in the collection. Holding it does not root the grid.
    wuxc::RowDefinition row{nullptr};
    std::vector<wuxc::Primitives::RangeBase::ValueChanged_revoker> revokers;
    std::vector<wux::UIElement::PointerWheelChanged_revoker> wheelRevokers;
    std::vector<wux::UIElement::PointerPressed_revoker> pressRevokers;
    std::vector<wuxc::Primitives::ButtonBase::Click_revoker> clickRevokers;

    // Per-display controls, so a refresh can update values in place instead of
    // rebuilding the whole panel.
    struct Binding {
        std::wstring id;
        std::wstring name;
        winrt::weak_ref<wuxc::Slider> slider;
        winrt::weak_ref<wux::FrameworkElement> icon;
        std::shared_ptr<RowTitle> title;
        // Null when the row has none.
        winrt::weak_ref<wuxc::Slider> contrast;
        winrt::weak_ref<wux::FrameworkElement> contrastIcon;
        winrt::weak_ref<wuxc::Button> power;
        winrt::weak_ref<wuxc::Slider> volume;
        winrt::weak_ref<wux::FrameworkElement> volumeIcon;
        std::vector<std::pair<int, winrt::weak_ref<wuxc::Primitives::ToggleButton>>> inputs;
    };
    std::vector<Binding> bindings;

    // The all-displays rows and what they drive; see PanelLinks.
    std::shared_ptr<PanelLinks> links;

    // Everything that points into this DLL from the panel's controls.
    void DetachHandlers() {
        revokers.clear();
        wheelRevokers.clear();
        pressRevokers.clear();
        clickRevokers.clear();
        links.reset();
    }

    // The display ids this panel's rows were built from, in order, including
    // the ones that got a "not supported" label rather than a slider. Compared
    // against the live list on every refresh so a panel that missed its
    // structural rebuild can notice and redo itself.
    std::vector<std::wstring> builtFor;

    // Taking a row in the middle of somebody else's Grid means renumbering
    // their children, so every change is recorded with the value it replaced.
    // Grid.Row is an attached property on the child, not a link to a
    // RowDefinition, so inserting a row does not move anything by itself --
    // and removing ours later does not move anything back either.
    struct MovedChild {
        winrt::weak_ref<wux::FrameworkElement> element;
        int originalRow = 0;
    };
    std::vector<MovedChild> movedChildren;

    // The card behind the toggles and the native sliders. Widened to cover our
    // row so the panel sits on it rather than below it.
    winrt::weak_ref<wux::FrameworkElement> cardBorder;
    int originalCardRowSpan = 0;
};

// Where the panel goes. Declared up here because the row layout depends on
// it: only one of the positions is drawn to match the shell's own rows.
//
// L1Grid holds the toggles in row 0, the native sliders in row 1 and the
// footer in row 2, and the card behind the first two is a Border spanning
// rows 0-1 -- so "with the native sliders" means taking a row inside that
// span and pushing what follows down, while "at the bottom" means a row after
// everything and outside the card.
enum class PanelPosition {
    AboveSliders,
    BelowSliders,
    Bottom,
};
PanelPosition g_panelPosition = PanelPosition::BelowSliders;

// Where the shell puts the parts of a slider row, so ours can go in the same
// places.
//
// Measured rather than hardcoded, because these are somebody else's layout
// constants and the only thing keeping them true is that nobody has changed
// them. The defaults are what 26200 reports and are used when the native rows
// cannot be measured -- which is normal, not exceptional: the rows are
// virtualized, and injection happens early enough that they are often not
// realized yet.
struct SliderMetrics {
    double iconLeft = 22;
    double sliderLeft = 56;
    double sliderRight = 300;
    double groupWidth = 358;
    double rowHeight = 40;

    // What to leave below the panel when it sits directly on top of the
    // native group, which is usually negative.
    //
    // The group carries its own top padding -- 12 on the GridView, 4 on the
    // item, 2 to the slider inside it -- and that padding exists to separate
    // the group from whatever is above. When the panel is what is above, it
    // is paid twice, and 12px of native row spacing becomes 26. The panel
    // gives the difference back.
    double gapAbove = -6;

    // And what to leave above it when it sits directly below the group.
    //
    // Same arithmetic mirrored: the group's bottom padding sits between the
    // last native track and us, so the panel gives back whatever that padding
    // already provides.
    double gapBelow = -2;
};

// Only the position that butts up against the native rows is drawn to match
// them. Below them and at the bottom the panel keeps its own proportions --
// the one at the bottom is outside the card entirely, where matching a row it
// is nowhere near would be imitation for its own sake.
bool UseNativeMetrics() {
    // Both positions inside the card are drawn to the shell's own row. Only
    // the one at the bottom keeps the panel's own proportions -- it is outside
    // the card entirely, where matching a row it is nowhere near would be
    // imitation for its own sake.
    return g_panelPosition != PanelPosition::Bottom;
}

// The panel's own look, for every other position: what it had before there
// was anything to line up with.
inline constexpr double kOwnMargin = 16;
inline constexpr double kOwnIconGap = 12;

// XAML thread only, like everything else that touches the tree.
SliderMetrics g_metrics;

bool g_hideStockBrightness = true;

// A display that answers nothing is listed by default, because a monitor that
// is simply missing from the panel is a worse puzzle than one labelled
// unsupported.
bool g_hideUnsupported = false;

// How each display's rows are laid out. Dropdown keeps a display to its
// brightness row and tucks the rest (contrast) behind a chevron; Expanded
// shows everything.
enum class LayoutStyle {
    Dropdown,
    Expanded,
};
LayoutStyle g_layoutStyle = LayoutStyle::Dropdown;
// Each display's own brightness slider (setting showPerMonitor, a name kept
// from before there were other per-display controls). The rest of a display's
// controls have their own switches.
bool g_showPerMonitor = true;
bool g_showMasterBrightness = true;
bool g_showContrast = true;
bool g_showMasterContrast = true;
bool g_showPowerButton = true;
bool g_showVolume = true;
bool g_showInputSwitcher = true;
// Percentage points per mouse-wheel notch over a slider; 0 leaves the wheel
// to the flyout.
int g_scrollStep = 5;
// Clicking a slider's icon jumps it to a level: left, middle, right button.
bool g_iconClicks = false;
int g_iconClickLevels[3] = {0, 50, 100};
// Settings for individual displays. The first rule whose text appears in a
// display's name or device id applies to it. Written only while no panel
// exists -- a settings change reloads the mod rather than editing this live.
struct DisplayRule {
    std::wstring match;  // lowercased
    std::wstring name;   // empty keeps the display's own name
    bool hide = false;
    bool hideContrast = false;
    bool hideVolume = false;
    bool hideInput = false;
    bool hidePower = false;
    bool operator==(const DisplayRule&) const = default;
};
std::vector<DisplayRule> g_displayRules;
// Which displays have their dropdown open, by stable id, for the session.
// XAML thread only.
std::set<std::wstring> g_expanded;


// The stock brightness row we collapsed and the group we shrank to close the
// gap. Kept outside Injection because the sliders are virtualized: the row may
// not exist until the panel is first shown, long after we injected, and there
// is only ever one of them. Touched only on the XAML thread.
struct StockSliderState {
    winrt::weak_ref<wux::FrameworkElement> item;
    wux::Visibility visibility = wux::Visibility::Visible;
    winrt::weak_ref<wux::FrameworkElement> group;
    double groupHeight = std::numeric_limits<double>::quiet_NaN();
    bool hidden = false;
};

// No [[clang::no_destroy]]: this holds only weak_refs, a double, an enum and
// a bool. Destroying a weak_ref at process shutdown just decrements an
// in-process control block, which is safe from any thread.
StockSliderState g_stockSlider;
bool g_loggedHideMiss = false;

// Set while pushing refreshed values into sliders, so their ValueChanged
// handlers can tell our own writes apart from the user's. UI thread only.
bool g_suppressValueChanged = false;

// Scoped, and restoring rather than clearing: the all-displays rows move the
// per-display ones while a refresh may already hold it, and a throw from
// Value() must not leave it stuck on, or every drag after that is ignored.
struct SuppressValueChanged {
    bool previous = g_suppressValueChanged;
    SuppressValueChanged() { g_suppressValueChanged = true; }
    ~SuppressValueChanged() { g_suppressValueChanged = previous; }
};

// The shell's own brightness Lottie, borrowed off its AnimatedIcon so our
// sliders can show the real animated sun rather than an imitation. WinUI2
// exposes no brightness visual source publicly, so lifting the live one is the
// only way to get it.
[[clang::no_destroy]] winrt::com_ptr<::IInspectable> g_brightnessSource;
[[clang::no_destroy]] winrt::com_ptr<IAnimatedIconStaticsAbi> g_animatedIconStatics;
bool g_capturedSource = false;

// The shell's volume Lottie, for the volume rows. Unlike the sun it is a state
// machine -- Mute and Volume_00/01/33/66, with a marker pair for every
// transition between them -- so it is driven by state name rather than by
// progress. The shell's AnimatedIcon for it has no name, so it is found by
// the source's class instead.
[[clang::no_destroy]] winrt::com_ptr<::IInspectable> g_volumeSource;
bool g_capturedVolume = false;

// The shell draws its speaker smaller than its sun: read off its AnimatedIcon
// when the source is captured, so ours matches rather than looking bigger.
// The icon still sits in the same slot as every other row's.
double g_volumeIconSize = 16;

// How many opens may retry borrowing from the shell before giving up. A
// desktop has no brightness Lottie to borrow, ever, and walking the shell's
// tree on every open for the rest of the session would be waste.
constexpr int kMaxLookRetries = 30;
int g_lookRetries = 0;

// The shell's own slider style, set on ours explicitly. Left to implicit
// lookup, an injected row can resolve to the plain system slider -- a tall
// rectangular thumb -- instead of the round WinUI one the shell's rows use,
// depending on where the lookup happens to start. Borrowed off a native
// slider, released on the XAML thread in RemoveInjections.
[[clang::no_destroy]] wux::Style g_sliderStyle{nullptr};

// The shell's Lottie carries two markers, Brightness_at_0 and
// Brightness_at_100, sitting at progress 0 and 1. So it is progress-driven
// rather than a state machine, and the usable range comes from those markers.
double g_progressMin = 0.0;
double g_progressMax = 1.0;

std::mutex g_injectionsMutex;
// Wrapped rather than bare, per the Windhawk guidance on globals at process
// shutdown: engaged from static init so it is always safe to dereference, and
// explicitly reset at the very end of Wh_ModUninit so the XAML references go
// while the apartment is still alive. The attribute stops the destructor from
// running on the shutdown thread when the host exits instead.
[[clang::no_destroy]] std::optional<std::vector<Injection>> g_injections{
    std::in_place};

// The XAML thread to marshal onto, captured at injection time. A thread id
// rather than a CoreDispatcher: an id is a plain value with no destructor and
// no refcount, so it cannot be raced into a use-after-free the way a shared
// CoreDispatcher reference could, and it is what the synchronous SendMessage
// hop below needs anyway.
std::atomic<DWORD> g_xamlThreadId{0};

// The timer that injects before the first paint. Declared here because
// RemoveInjections, far above its use, is the one place allowed to stop it:
// a DispatcherTimer is UI-thread affine.
[[clang::no_destroy]] wux::DispatcherTimer g_earlyInject{nullptr};

// Retry subscriptions on the shell's own elements. They must live in mod-owned
// globals: a revoker owned only by the lambda it is captured in cannot be
// reached at unload time, and a handler left registered when Windhawk frees
// this DLL is a crash on the next layout pass.
struct RetryHandlers {
    wux::FrameworkElement::Loaded_revoker loaded;
    wux::FrameworkElement::LayoutUpdated_revoker layout;
    int attempts = 0;

    void Revoke() {
        loaded.revoke();
        layout.revoke();
        attempts = 0;
    }
};

// Bounded on purpose. ArmStockSliderHide looks for an element that only exists
// when there is an internal panel, so on a desktop it would otherwise retry
// forever -- re-walking the shell's visual tree on every single layout pass,
// and adding another permanent handler every time the panel is opened.
constexpr int kMaxRetryAttempts = 60;

[[clang::no_destroy]] RetryHandlers g_injectRetry;
[[clang::no_destroy]] RetryHandlers g_hideRetry;

void EngineLog(const wchar_t* msg) {
    Wh_Log(L"engine: %s", msg);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }
    return module;
}

// ---------------------------------------------------------------------------
// Visual tree helpers
// ---------------------------------------------------------------------------

std::wstring ElementLabel(wux::DependencyObject const& obj) {
    std::wstring label;
    try {
        label = winrt::get_class_name(obj);
    } catch (...) {
        label = L"<unknown>";
    }
    if (auto fe = obj.try_as<wux::FrameworkElement>()) {
        std::wstring name{fe.Name()};
        if (!name.empty()) {
            label += L"#" + name;
        }
    }
    return label;
}

// One-shot structural dump. The Control Center layout is undocumented and
// moves between Windows builds, so when injection cannot find what it expects
// the log has to be enough to fix it without guessing.
void DumpTree(wux::DependencyObject const& root, int depth, int maxDepth) {
    if (depth > maxDepth) {
        return;
    }

    std::wstring indent(static_cast<size_t>(depth) * 2, L' ');
    std::wstring extra;
    if (auto fe = root.try_as<wux::FrameworkElement>()) {
        wchar_t buf[128];
        swprintf(buf, 128, L"  [row=%d col=%d w=%.0f h=%.0f]",
                 wuxc::Grid::GetRow(fe), wuxc::Grid::GetColumn(fe),
                 fe.ActualWidth(), fe.ActualHeight());
        extra = buf;
    }
    Wh_Log(L"%s%s%s", indent.c_str(), ElementLabel(root).c_str(), extra.c_str());

    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        DumpTree(wuxm::VisualTreeHelper::GetChild(root, i), depth + 1, maxDepth);
    }
}

wux::DependencyObject FindDescendant(wux::DependencyObject const& root,
                                     std::wstring_view label, int maxDepth) {
    if (maxDepth < 0) {
        return nullptr;
    }
    if (ElementLabel(root) == label) {
        return root;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto found = FindDescendant(child, label, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

wux::DependencyObject FindDescendantByName(wux::DependencyObject const& root,
                                           std::wstring_view name, int maxDepth) {
    if (maxDepth < 0) {
        return nullptr;
    }
    if (auto fe = root.try_as<wux::FrameworkElement>()) {
        if (std::wstring_view{fe.Name()} == name) {
            return root;
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto found = FindDescendantByName(child, name, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// FindDescendant by exact label cannot find either of the elements the
// layout has to be measured from: the shell's slider is
// `ControlCenter.AsyncSlider`, not a `Windows.UI.Xaml.Controls.Slider`, and
// its icon carries a name on one row and not on the next.
wux::DependencyObject FindDescendantByClassSubstring(
    wux::DependencyObject const& root, std::wstring_view needle, int maxDepth) {
    if (maxDepth < 0) {
        return nullptr;
    }
    if (ElementLabel(root).find(needle) != std::wstring::npos) {
        return root;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto found = FindDescendantByClassSubstring(
                wuxm::VisualTreeHelper::GetChild(root, i), needle,
                maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

wux::DependencyObject FindAncestorOfClass(wux::DependencyObject const& start,
                                          std::wstring_view className,
                                          int maxUp) {
    auto current = start;
    for (int i = 0; i < maxUp; ++i) {
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (!current) {
            return nullptr;
        }
        try {
            if (std::wstring{winrt::get_class_name(current)} == className) {
                return current;
            }
        } catch (...) {
            // Keep walking; an element we cannot name is not a match.
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// UI construction
// ---------------------------------------------------------------------------

// Lifts the shell's brightness Lottie off its own AnimatedIcon so our sliders
// can use the genuine article. The source stays usable after we collapse the
// stock row, because collapsing only hides the element -- it stays in the tree.
void TryCaptureBrightnessSource(wux::FrameworkElement const& l1Grid) {
    if (g_capturedSource) {
        return;
    }

    auto group = FindDescendant(
        l1Grid, L"Windows.UI.Xaml.Controls.ContentControl#SlidersGroup", 6);
    if (!group) {
        return;
    }
    auto iconObj = FindDescendantByName(group, L"BrightnessPlayer", 40);
    if (!iconObj) {
        return;  // virtualized away; a later attempt may find it
    }

    winrt::com_ptr<IAnimatedIconAbi> iconAbi;
    if (FAILED(winrt::get_unknown(iconObj)->QueryInterface(
            kIID_AnimatedIcon, iconAbi.put_void()))) {
        Wh_Log(L"BrightnessPlayer is not a WinUI2 AnimatedIcon (%s)",
               ElementLabel(iconObj).c_str());
        g_capturedSource = true;  // no point retrying
        return;
    }

    winrt::com_ptr<::IInspectable> source;
    if (FAILED(iconAbi->get_Source(source.put_void())) || !source) {
        Wh_Log(L"BrightnessPlayer has no Source yet");
        return;
    }

    g_brightnessSource = source;
    g_capturedSource = true;
    Wh_Log(L"Captured the shell's brightness source");

    winrt::com_ptr<IAnimatedVisualSource2Abi> source2;
    if (FAILED(source->QueryInterface(kIID_AnimatedVisualSource2,
                                      source2.put_void()))) {
        Wh_Log(L"Source is not IAnimatedVisualSource2; cannot read markers");
        return;
    }

    // The markers bound the animation rather than enumerating states, so all
    // we need from them is the progress range to drive between.
    double minProgress = 0.0;
    double maxProgress = 0.0;
    bool haveMarkers = false;
    try {
        void* markersAbi = nullptr;
        if (FAILED(source2->get_Markers(&markersAbi)) || !markersAbi) {
            Wh_Log(L"Source exposes no markers; assuming 0..1");
        } else {
            // Windows.Foundation is projected correctly, so the map itself can
            // be handled with the normal projection once detached from the ABI.
            wf::Collections::IMapView<winrt::hstring, double> markers{nullptr};
            winrt::attach_abi(markers, markersAbi);

            for (auto const& marker : markers) {
                double value = marker.Value();
                Wh_Log(L"  marker: %s = %.4f",
                       std::wstring{marker.Key()}.c_str(), value);
                if (!haveMarkers) {
                    minProgress = maxProgress = value;
                    haveMarkers = true;
                } else {
                    minProgress = std::min(minProgress, value);
                    maxProgress = std::max(maxProgress, value);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"Reading markers failed: %08X", winrt::to_hresult());
    }

    if (maxProgress <= minProgress) {
        minProgress = 0.0;
        maxProgress = 1.0;
    }
    g_progressMin = minProgress;
    g_progressMax = maxProgress;

    // SetState lives on the activation factory, not the instance.
    winrt::hstring className{kAnimatedIconClass};
    HRESULT hr = RoGetActivationFactory(
        static_cast<HSTRING>(winrt::get_abi(className)),
        kIID_AnimatedIconStatics, g_animatedIconStatics.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"No IAnimatedIconStatics (%08X); the icon will not animate", hr);
    }

    Wh_Log(L"Brightness icon progress range %.3f .. %.3f", g_progressMin,
           g_progressMax);
}

// The first native Slider under root, skipping nothing: callers pass a subtree
// that holds only the shell's own rows.
wuxc::Slider FindFirstSlider(wux::DependencyObject const& root, int maxDepth) {
    if (maxDepth < 0 || !root) {
        return nullptr;
    }
    if (auto slider = root.try_as<wuxc::Slider>()) {
        return slider;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto found = FindFirstSlider(wuxm::VisualTreeHelper::GetChild(root, i),
                                         maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// The implicit Slider style in effect for `from`: the first dictionary up the
// tree, then the application's, that has one.
wux::Style LookupImplicitSliderStyle(wux::DependencyObject const& from) {
    auto key = winrt::box_value(winrt::xaml_typename<wuxc::Slider>());
    auto lookup = [&](wux::ResourceDictionary const& resources) -> wux::Style {
        if (!resources) {
            return nullptr;
        }
        try {
            return resources.Lookup(key).try_as<wux::Style>();
        } catch (...) {
            return nullptr;  // Lookup throws when the key is absent
        }
    };
    for (wux::DependencyObject current = from; current;
         current = wuxm::VisualTreeHelper::GetParent(current)) {
        if (auto element = current.try_as<wux::FrameworkElement>()) {
            if (auto style = lookup(element.Resources())) {
                return style;
            }
        }
    }
    try {
        if (auto app = wux::Application::Current()) {
            return lookup(app.Resources());
        }
    } catch (...) {
    }
    return nullptr;
}

// Either the shell's real AnimatedIcon, or the Segoe Fluent brightness glyph
// when the Lottie could not be borrowed or cannot be driven by level.
// Both icon variants are built to this, and the row arithmetic uses the
// constant rather than reading Width() back off the element. It is also the
// icon slot every row reserves, whatever size the icon in it is drawn at.
//
// The fallback glyph had no Width at all, so Width() returned NaN -- XAML's
// spelling of Auto -- and the icon-to-track gap computed from it was NaN,
// which then went into a Thickness. That is not a rare path: it is what every
// desktop gets, since with no internal panel there is no brightness Lottie to
// borrow.
inline constexpr double kIconSize = 20;

// The source of the first AnimatedIcon under root whose source is of the given
// class, or null. The icon itself goes to *iconOut.
winrt::com_ptr<::IInspectable> FindAnimatedSource(wux::DependencyObject const& root,
                                                  std::wstring_view sourceClass, int maxDepth,
                                                  wux::FrameworkElement* iconOut) {
    if (!root || maxDepth < 0) {
        return nullptr;
    }
    try {
        if (std::wstring{winrt::get_class_name(root)} == kAnimatedIconClass) {
            winrt::com_ptr<IAnimatedIconAbi> icon;
            winrt::com_ptr<::IInspectable> source;
            if (SUCCEEDED(winrt::get_unknown(root)->QueryInterface(kIID_AnimatedIcon,
                                                                   icon.put_void())) &&
                SUCCEEDED(icon->get_Source(source.put_void())) && source) {
                wf::IInspectable inspectable{nullptr};
                winrt::copy_from_abi(inspectable, source.get());
                if (std::wstring{winrt::get_class_name(inspectable)} == sourceClass) {
                    *iconOut = root.try_as<wux::FrameworkElement>();
                    return source;
                }
            }
        }
    } catch (...) {
        // An element we cannot inspect is simply not the one.
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto found = FindAnimatedSource(wuxm::VisualTreeHelper::GetChild(root, i),
                                            sourceClass, maxDepth - 1, iconOut)) {
            return found;
        }
    }
    return nullptr;
}

void TryCaptureVolumeSource(wux::FrameworkElement const& l1Grid) {
    if (g_capturedVolume) {
        return;
    }
    if (!g_animatedIconStatics) {
        winrt::hstring className{kAnimatedIconClass};
        RoGetActivationFactory(static_cast<HSTRING>(winrt::get_abi(className)),
                               kIID_AnimatedIconStatics, g_animatedIconStatics.put_void());
    }
    auto group = FindDescendant(
        l1Grid, L"Windows.UI.Xaml.Controls.ContentControl#SlidersGroup", 6);
    if (!group) {
        return;
    }
    wux::FrameworkElement native{nullptr};
    if (auto source = FindAnimatedSource(group, L"ControlCenter.QA_Volume", 40, &native)) {
        g_volumeSource = source;
        g_capturedVolume = true;
        // Laid out by now, since it is on screen; its Width is the fallback
        // for a row that has not been measured yet.
        double size = native ? native.ActualWidth() : 0;
        if (!(size > 0) && native) {
            size = native.Width();
        }
        if (size > 0) {
            g_volumeIconSize = std::min(size, kIconSize);
        }
        Wh_Log(L"Captured the shell's volume source (icon %.0f px)", g_volumeIconSize);
    }
}

// Returns whether the style was newly captured.
bool TryCaptureSliderStyle(wux::FrameworkElement const& l1Grid) {
    if (g_sliderStyle) {
        return false;
    }
    auto group = FindDescendant(
        l1Grid, L"Windows.UI.Xaml.Controls.ContentControl#SlidersGroup", 6);
    if (!group) {
        return false;
    }
    wuxc::Slider native = FindFirstSlider(group, 40);
    if (!native) {
        return false;  // virtualized away; the next open retries
    }
    wux::Style style = native.Style();
    const wchar_t* how = L"explicit";
    if (!style) {
        style = LookupImplicitSliderStyle(native);
        how = L"implicit";
    }
    if (!style) {
        Wh_Log(L"Native slider %s has no style to borrow", ElementLabel(native).c_str());
        return false;
    }
    g_sliderStyle = style;
    Wh_Log(L"Borrowed the shell's slider style (%s, from %s)", how,
           ElementLabel(native).c_str());
    return true;
}

// Everything borrowed from the shell's own rows. The rows it comes from are
// virtualized, so any of it can be missing the first time; this is retried on
// every open. Returns whether anything was newly captured, which means the
// panel's rows were built without it and should be rebuilt.
bool TryCaptureShellLook(wux::FrameworkElement const& l1Grid) {
    const bool hadSource = g_capturedSource;
    const bool hadVolume = g_capturedVolume;
    TryCaptureBrightnessSource(l1Grid);
    TryCaptureVolumeSource(l1Grid);
    const bool gotStyle = TryCaptureSliderStyle(l1Grid);
    return gotStyle || (!hadSource && g_brightnessSource) || (!hadVolume && g_volumeSource);
}

// One of the shell's own AnimatedIcons playing the given Lottie, or the glyph
// when there is no Lottie to borrow.
wux::FrameworkElement MakeAnimatedIcon(winrt::com_ptr<::IInspectable> const& source,
                                       const wchar_t* glyph) {
    if (source) {
        try {
            winrt::hstring className{kAnimatedIconClass};

            // Not RoActivateInstance: that routes through
            // IActivationFactory::ActivateInstance, which a composable XAML
            // control leaves unimplemented (E_NOTIMPL).
            winrt::com_ptr<IAnimatedIconFactoryAbi> factory;
            HRESULT hr = RoGetActivationFactory(
                static_cast<HSTRING>(winrt::get_abi(className)),
                kIID_AnimatedIconFactory, factory.put_void());

            winrt::com_ptr<::IInspectable> inner;
            winrt::com_ptr<::IInspectable> instance;
            if (SUCCEEDED(hr) && factory) {
                hr = factory->CreateInstance(nullptr, inner.put_void(),
                                             instance.put_void());
            }

            winrt::com_ptr<IAnimatedIconAbi> iconAbi;
            if (SUCCEEDED(hr) && instance &&
                SUCCEEDED(instance->QueryInterface(kIID_AnimatedIcon,
                                                   iconAbi.put_void())) &&
                SUCCEEDED(iconAbi->put_Source(source.get()))) {
                wux::FrameworkElement element{nullptr};
                if (SUCCEEDED(instance->QueryInterface(
                        winrt::guid_of<wux::FrameworkElement>(),
                        winrt::put_abi(element)))) {
                    element.Width(kIconSize);
                    element.Height(kIconSize);
                    return element;
                }
            }
            Wh_Log(L"AnimatedIcon creation failed (%08X); using the glyph", hr);
        } catch (...) {
            Wh_Log(L"AnimatedIcon creation threw: %08X", winrt::to_hresult());
        }
    }

    wuxc::FontIcon font;
    font.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    font.Glyph(glyph);
    font.Width(kIconSize);
    font.Height(kIconSize);
    return font;
}

wux::FrameworkElement MakeBrightnessIcon() {
    return MakeAnimatedIcon(g_brightnessSource, L"\xE706");  // Brightness
}

wux::FrameworkElement MakeVolumeIcon() {
    wux::FrameworkElement icon = MakeAnimatedIcon(g_volumeSource, L"\xE767");  // Volume
    if (g_volumeSource) {
        icon.Width(g_volumeIconSize);
        icon.Height(g_volumeIconSize);
    }
    return icon;
}

// The volume Lottie's states, picked the way the shell's own row picks them:
// no waves at 0, then one, two and three. Mute is never used -- 0 is it.
void SetVolumeIconLevel(wux::FrameworkElement const& icon, double percent) {
    if (auto font = icon.try_as<wuxc::FontIcon>()) {
        font.Opacity(0.40 + 0.60 * std::clamp(percent, 0.0, 100.0) / 100.0);
        return;
    }
    if (!g_animatedIconStatics) {
        return;
    }
    const long level = std::lround(percent);
    const wchar_t* state = level <= 0    ? L"Volume_00"
                           : level < 33  ? L"Volume_01"
                           : level < 66  ? L"Volume_33"
                                         : L"Volume_66";
    try {
        if (auto depObj = icon.try_as<wux::DependencyObject>()) {
            winrt::hstring name{state};
            g_animatedIconStatics->SetState(winrt::get_abi(depObj), winrt::get_abi(name));
        }
    } catch (...) {
    }
}

void SetIconLevel(wux::FrameworkElement const& icon, double percent) {
    double t = std::clamp(percent, 0.0, 100.0) / 100.0;

    if (auto font = icon.try_as<wuxc::FontIcon>()) {
        // Opacity only. Changing FontSize changes the icon's measured width,
        // and it sits in an Auto-width column, so the whole row would reflow
        // and the slider shift sideways as the thumb moves.
        font.Opacity(0.40 + 0.60 * t);
        return;
    }

    // Otherwise it is the borrowed AnimatedIcon.
    if (!g_animatedIconStatics) {
        return;
    }

    // AnimatedIcon resolves a state name against the Lottie's markers, and
    // when the name is not a marker but parses as a number it plays to that
    // normalized progress instead. That numeric fallback is what turns two
    // endpoint markers into a continuous level.
    double progress = g_progressMin + t * (g_progressMax - g_progressMin);
    wchar_t buffer[32];
    swprintf(buffer, 32, L"%.4f", progress);

    try {
        auto depObj = icon.try_as<wux::DependencyObject>();
        if (depObj) {
            winrt::hstring state{buffer};
            // A rejected state must not take the slider down with it.
            g_animatedIconStatics->SetState(winrt::get_abi(depObj),
                                            winrt::get_abi(state));
        }
    } catch (...) {
    }
}

// A row title that carries the value of every slider shown under it, in the
// order they are shown: "Samsung  ·  80%  ·  50%  ·  30%". A slider folded into
// a closed dropdown is left out, so each number still lines up with a row in
// view. Held by the handlers that update it, and released with them.
//
// The title and the dropdown are held strongly, the sliders weakly. A weak
// ref to a XAML element only resolves while XAML keeps that element's
// wrapper alive, and it keeps it for elements with handlers attached -- the
// sliders -- but lets it go for a plain TextBlock or StackPanel once nothing
// else holds it, though the element itself stays on screen. Held weakly, the
// title stopped updating a moment after the panel was built, stuck at
// whatever it said then. Holding these two makes no cycle: neither refers
// back to anything here. The sliders stay weak for the opposite reason --
// their own handlers hold this.
struct RowTitle {
    std::wstring name;
    wuxc::TextBlock text{nullptr};
    struct Entry {
        winrt::weak_ref<wuxc::Slider> slider;
        bool inDetails;
    };
    std::vector<Entry> sliders;
    // The dropdown the details sliders are in; null when there is none.
    wuxc::StackPanel details{nullptr};
};

// XAML thread. Reads the values off the sliders, so it is right whether it
// runs from a drag, a refresh, or the dropdown opening.
void UpdateRowTitle(RowTitle const& row) {
    if (!row.text) {
        return;
    }
    bool detailsShown = true;
    if (row.details) {
        detailsShown = row.details.Visibility() == wux::Visibility::Visible;
    }
    std::wstring out = row.name;
    for (const RowTitle::Entry& entry : row.sliders) {
        if (entry.inDetails && !detailsShown) {
            continue;
        }
        if (auto slider = entry.slider.get()) {
            out += L"  ·  " + std::to_wstring(std::lround(slider.Value())) + L"%";
        }
    }
    row.text.Text(out);
}

// The per-display settings that apply to a display: the first rule whose
// text appears in its own name or device id, if any.
const DisplayRule* RuleFor(const brightness::Display& d) {
    std::wstring name = brightness::detail::ToLower(d.name);
    std::wstring id = brightness::detail::ToLower(d.stableId);
    for (const DisplayRule& rule : g_displayRules) {
        if (name.find(rule.match) != std::wstring::npos ||
            id.find(rule.match) != std::wstring::npos) {
            return &rule;
        }
    }
    return nullptr;
}

std::wstring DisplayLabel(const brightness::Display& d) {
    const DisplayRule* rule = RuleFor(d);
    return (rule && !rule->name.empty()) ? rule->name : d.name;
}

bool IsHidden(const brightness::Display& d) {
    const DisplayRule* rule = RuleFor(d);
    return rule && rule->hide;
}

bool Contains(const std::vector<std::wstring>& ids, const std::wstring& id) {
    return std::find(ids.begin(), ids.end(), id) != ids.end();
}

// What every all-displays row is set to on open: the mean of the displays it
// drives. Moving it then sets them all to the same value.
int AverageBrightness(const std::vector<brightness::Display>& displays,
                      const std::vector<std::wstring>& ids) {
    int sum = 0, count = 0;
    for (const brightness::Display& d : displays) {
        if (Contains(ids, d.stableId) && d.percent >= 0) {
            sum += d.percent;
            ++count;
        }
    }
    return count ? (sum + count / 2) / count : -1;
}

int AverageContrast(const std::vector<brightness::Display>& displays,
                    const std::vector<std::wstring>& ids) {
    int sum = 0, count = 0;
    for (const brightness::Display& d : displays) {
        if (Contains(ids, d.stableId) && d.contrast >= 0) {
            sum += d.contrast;
            ++count;
        }
    }
    return count ? (sum + count / 2) / count : -1;
}

// MCCS input source values, as short as the row has room for.
std::wstring InputName(int value) {
    switch (value) {
        case 0x01: return L"VGA 1";
        case 0x02: return L"VGA 2";
        case 0x03: return L"DVI 1";
        case 0x04: return L"DVI 2";
        case 0x0F: return L"DP 1";
        case 0x10: return L"DP 2";
        case 0x11: return L"HDMI 1";
        case 0x12: return L"HDMI 2";
        case 0x1B: return L"USB-C";
        default: {
            wchar_t buffer[16];
            swprintf(buffer, 16, L"Input %02X", value);
            return buffer;
        }
    }
}

// The glyph stand-in for contrast; there is no animated one to borrow.
wux::FrameworkElement MakeContrastIcon() {
    wuxc::FontIcon font;
    font.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    font.Glyph(L"\xE7A1");  // Contrast
    font.Width(kIconSize);
    font.Height(kIconSize);
    return font;
}

// The trailing slot a slider row keeps for the power button. Reserved on
// every row once any display has one, so every track still ends at the same x.
inline constexpr double kTrailingWidth = 36;

// The panel's margins, which depend on whether rows carry a trailing slot:
// inside the card the track has to end where the shell's own tracks end, so
// the slot comes out of the right margin rather than out of the track.
wux::Thickness PanelMargin(double trailing) {
    if (UseNativeMetrics()) {
        // Left and right come from the native row so the icon column and the
        // far end of the track line up with it; the panel used to be 6px left
        // of the icons and 42px past the end of the sliders.
        //
        // The seam goes on whichever edge touches the group, and the other
        // edge gets a plain gap -- these rows carry titles, so they are a
        // group of their own rather than more of the same list.
        const bool above = g_panelPosition == PanelPosition::AboveSliders;
        const double right =
            std::max(0.0, g_metrics.groupWidth - g_metrics.sliderRight - trailing);
        return wux::ThicknessHelper::FromLengths(
            g_metrics.iconLeft, above ? 8 : g_metrics.gapBelow, right,
            above ? g_metrics.gapAbove : 8);
    }
    return wux::ThicknessHelper::FromLengths(kOwnMargin, 4, kOwnMargin, 8);
}

// Links between the rows of one panel build, so the all-displays sliders and
// the per-display ones can keep each other current. Held by the handlers that
// need it; weak refs only, so it roots none of the controls.
struct PanelLinks {
    winrt::weak_ref<wuxc::Slider> masterBrightness;
    winrt::weak_ref<wux::FrameworkElement> masterBrightnessIcon;
    winrt::weak_ref<wuxc::Slider> masterContrast;
    winrt::weak_ref<wux::FrameworkElement> masterContrastIcon;
    std::vector<std::pair<std::wstring, winrt::weak_ref<wuxc::Slider>>> brightness;
    std::vector<std::pair<std::wstring, winrt::weak_ref<wuxc::Slider>>> contrast;
    // What the all-displays rows drive: every display that can take the
    // value, whether or not it has a row of its own.
    std::vector<std::wstring> brightnessIds;
    std::vector<std::wstring> contrastIds;
};

// Brings the all-displays rows back to the mean of what they drive, after a
// per-display change or a refresh. XAML thread.
void SyncMasters(PanelLinks& links) {
    if (!g_engine) {
        return;
    }
    std::vector<brightness::Display> displays = g_engine->GetDisplays();
    SuppressValueChanged guard;
    if (auto slider = links.masterBrightness.get()) {
        const int value = AverageBrightness(displays, links.brightnessIds);
        if (value >= 0 && std::lround(slider.Value()) != value) {
            slider.Value(value);
        }
    }
    if (auto slider = links.masterContrast.get()) {
        const int value = AverageContrast(displays, links.contrastIds);
        if (value >= 0 && std::lround(slider.Value()) != value) {
            slider.Value(value);
        }
    }
}

wuxc::Slider MakeSlider(double value, double step) {
    wuxc::Slider slider;
    if (g_sliderStyle) {
        slider.Style(g_sliderStyle);
    }
    slider.Minimum(0);
    slider.Maximum(100);
    slider.Value(value);
    slider.IsThumbToolTipEnabled(true);
    slider.VerticalAlignment(wux::VerticalAlignment::Center);
    if (UseNativeMetrics()) {
        // Matched to the shell's row so the track sits at the same height
        // and the hit target is the same size. The default template is
        // shorter, which reads as a thinner control next to the real ones.
        slider.Height(g_metrics.rowHeight);
    }
    slider.StepFrequency(step);
    slider.SnapsTo(wuxc::Primitives::SliderSnapsTo::StepValues);
    return slider;
}

// Mouse wheel over a slider moves it by the configured step (issue #5647),
// and keeps the flyout from scrolling underneath it.
void AttachWheel(wuxc::Slider const& slider, Injection& injection) {
    if (g_scrollStep <= 0) {
        return;
    }
    injection.wheelRevokers.push_back(slider.PointerWheelChanged(
        winrt::auto_revoke,
        [weak = winrt::make_weak(slider)](
            wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const& args) {
            auto target = weak.get();
            if (!target) {
                return;
            }
            auto props = args.GetCurrentPoint(target).Properties();
            const int delta = props.MouseWheelDelta();
            if (props.IsHorizontalMouseWheel() || delta == 0) {
                return;
            }
            // A step finer than the monitor can represent would snap straight
            // back to where it was.
            const double step =
                std::max(static_cast<double>(g_scrollStep), target.StepFrequency());
            target.Value(std::clamp(target.Value() + (delta > 0 ? step : -step),
                                    target.Minimum(), target.Maximum()));
            args.Handled(true);
        }));
}

// Clicking a slider's icon jumps it to a level: left, middle and right
// button each have their own (issue #5697). The icon sits in a Border with a
// transparent background, since a glyph only hit-tests where it has ink.
wux::FrameworkElement MakeIconHost(wux::FrameworkElement const& icon,
                                   wuxc::Slider const& slider, Injection& injection) {
    wuxc::Border host;
    host.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    host.VerticalAlignment(wux::VerticalAlignment::Center);
    // A fixed slot, so an icon drawn smaller than the rest (the speaker) is
    // centred under the others and its track still starts where theirs do.
    host.Width(kIconSize);
    host.Height(kIconSize);
    icon.HorizontalAlignment(wux::HorizontalAlignment::Center);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);
    host.Child(icon);
    if (g_iconClicks) {
        injection.pressRevokers.push_back(host.PointerPressed(
            winrt::auto_revoke,
            [weakHost = winrt::make_weak(host), weakSlider = winrt::make_weak(slider)](
                wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const& args) {
                auto h = weakHost.get();
                auto s = weakSlider.get();
                if (!h || !s) {
                    return;
                }
                auto props = args.GetCurrentPoint(h).Properties();
                int level = -1;
                if (props.IsLeftButtonPressed()) {
                    level = g_iconClickLevels[0];
                } else if (props.IsMiddleButtonPressed()) {
                    level = g_iconClickLevels[1];
                } else if (props.IsRightButtonPressed()) {
                    level = g_iconClickLevels[2];
                }
                if (level >= 0) {
                    s.Value(std::clamp(level, 0, 100));
                    args.Handled(true);
                }
            }));
    }
    return host;
}

// Icon, track, and a trailing slot. trailingWidth reserves the slot even when
// there is nothing in it, so every track in the panel ends at the same x.
wuxc::Grid MakeSliderRow(wux::FrameworkElement const& iconHost,
                         wuxc::Slider const& slider,
                         wux::FrameworkElement const& trailing, double trailingWidth) {
    wuxc::Grid row;
    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    wuxc::ColumnDefinition sliderColumn;
    sliderColumn.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    row.ColumnDefinitions().Append(iconColumn);
    row.ColumnDefinitions().Append(sliderColumn);
    if (trailingWidth > 0) {
        wuxc::ColumnDefinition trailingColumn;
        trailingColumn.Width(wux::GridLengthHelper::FromValueAndType(
            trailingWidth, wux::GridUnitType::Pixel));
        row.ColumnDefinitions().Append(trailingColumn);
    }

    // The gap the shell leaves between its icon and its track, derived
    // rather than guessed: the panel already starts at the icon's left
    // edge, so what is left over after the icon is the gap.
    const double iconGap = UseNativeMetrics()
                               ? g_metrics.sliderLeft - g_metrics.iconLeft - kIconSize
                               : kOwnIconGap;
    iconHost.Margin(wux::ThicknessHelper::FromLengths(0, 0, iconGap, 0));
    wuxc::Grid::SetColumn(iconHost, 0);
    row.Children().Append(iconHost);

    wuxc::Grid::SetColumn(slider, 1);
    row.Children().Append(slider);

    if (trailing && trailingWidth > 0) {
        wuxc::Grid::SetColumn(trailing, 2);
        row.Children().Append(trailing);
    }
    return row;
}

wuxc::TextBlock MakeRowTitle(winrt::hstring const& text) {
    wuxc::TextBlock title;
    title.Text(text);
    title.FontSize(12);
    title.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
    title.Opacity(0.85);
    title.VerticalAlignment(wux::VerticalAlignment::Center);
    return title;
}

// A small glyph button in the style of the shell's own row buttons: no
// chrome until hovered.
wuxc::Button MakeGlyphButton(const wchar_t* glyph, double glyphSize, double width,
                             double height) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    icon.FontSize(glyphSize);

    wuxc::Button button;
    button.Content(icon);
    button.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    button.BorderThickness(wux::ThicknessHelper::FromUniformLength(0));
    button.Padding(wux::ThicknessHelper::FromUniformLength(0));
    button.MinWidth(0);
    button.MinHeight(0);
    button.Width(width);
    button.Height(height);
    button.HorizontalAlignment(wux::HorizontalAlignment::Right);
    button.VerticalAlignment(wux::VerticalAlignment::Center);
    return button;
}

void ShowPowerState(wuxc::Button const& button, const std::wstring& name, bool off) {
    button.Opacity(off ? 0.45 : 1.0);
    wuxc::ToolTipService::SetToolTip(
        button, winrt::box_value(winrt::hstring{(off ? L"Turn on " : L"Turn off ") + name}));
}

// The per-display power button. It only exists for displays that report a
// power state over DDC/CI, and only while there is another display to see
// the result on.
wuxc::Button MakePowerButton(const brightness::Display& d, const std::wstring& label,
                             Injection& injection) {
    wuxc::Button button = MakeGlyphButton(L"\xE7E8", 14, 32, 32);  // PowerButton
    ShowPowerState(button, label, d.poweredOff);
    std::wstring id = d.stableId;
    injection.clickRevokers.push_back(button.Click(
        winrt::auto_revoke,
        [id, label, weak = winrt::make_weak(button)](wf::IInspectable const&,
                                                     wux::RoutedEventArgs const&) {
            if (!g_engine) {
                return;
            }
            bool off = false;
            for (const brightness::Display& current : g_engine->GetDisplays()) {
                if (current.stableId == id) {
                    off = current.poweredOff;
                }
            }
            // Off -> on, on -> off. The engine reflects it at once, and a
            // refresh corrects it if the monitor did something else.
            g_engine->SetPower(id, off);
            if (auto b = weak.get()) {
                ShowPowerState(b, label, !off);
            }
        }));
    return button;
}

// One toggle per input the monitor lists, the current one pressed. Buttons
// rather than a dropdown: a dropdown's popup inside the flyout is one more
// thing that can close it.
wuxc::Grid MakeInputRow(const brightness::Display& d, double trailing, Injection& injection,
                        Injection::Binding& binding) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(L"\xE7F4");  // TVMonitor
    icon.Width(kIconSize);
    icon.Height(kIconSize);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::StackPanel buttons;
    buttons.Orientation(wuxc::Orientation::Horizontal);
    buttons.VerticalAlignment(wux::VerticalAlignment::Center);
    if (UseNativeMetrics()) {
        buttons.Height(g_metrics.rowHeight);
    }

    auto group = std::make_shared<
        std::vector<std::pair<int, winrt::weak_ref<wuxc::Primitives::ToggleButton>>>>();
    for (int value : d.inputs) {
        wuxc::Primitives::ToggleButton toggle;
        toggle.Content(winrt::box_value(winrt::hstring{InputName(value)}));
        toggle.FontSize(12);
        toggle.Padding(wux::ThicknessHelper::FromLengths(10, 3, 10, 3));
        toggle.MinWidth(0);
        toggle.MinHeight(0);
        toggle.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
        toggle.VerticalAlignment(wux::VerticalAlignment::Center);
        toggle.IsChecked(value == d.input);
        buttons.Children().Append(toggle);
        group->emplace_back(value, winrt::make_weak(toggle));

        std::wstring id = d.stableId;
        injection.clickRevokers.push_back(toggle.Click(
            winrt::auto_revoke,
            [id, value, group](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                // Pressing the current input keeps it pressed; pressing
                // another moves the highlight and switches the monitor.
                for (auto& entry : *group) {
                    if (auto t = entry.second.get()) {
                        t.IsChecked(entry.first == value);
                    }
                }
                if (g_engine) {
                    g_engine->SetInput(id, value);
                }
            }));
    }
    binding.inputs = *group;

    wuxc::Border iconHost;
    iconHost.Child(icon);
    iconHost.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::Grid row;
    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    wuxc::ColumnDefinition buttonColumn;
    buttonColumn.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    row.ColumnDefinitions().Append(iconColumn);
    row.ColumnDefinitions().Append(buttonColumn);
    if (trailing > 0) {
        wuxc::ColumnDefinition trailingColumn;
        trailingColumn.Width(
            wux::GridLengthHelper::FromValueAndType(trailing, wux::GridUnitType::Pixel));
        row.ColumnDefinitions().Append(trailingColumn);
    }
    const double iconGap = UseNativeMetrics()
                               ? g_metrics.sliderLeft - g_metrics.iconLeft - kIconSize
                               : kOwnIconGap;
    iconHost.Margin(wux::ThicknessHelper::FromLengths(0, 0, iconGap, 0));
    wuxc::Grid::SetColumn(iconHost, 0);
    row.Children().Append(iconHost);
    wuxc::Grid::SetColumn(buttons, 1);
    row.Children().Append(buttons);
    return row;
}

void PopulateSliderPanel(wuxc::StackPanel const& panel, Injection& injection) {
    std::vector<brightness::Display> displays = g_engine->GetDisplays();
    Wh_Log(L"Building panel for %zu display(s)", displays.size());

    // Every display the panel considered, whether or not it got a row. Keyed
    // on this rather than on what was drawn, so that a monitor arriving or
    // leaving is still detected as a structural change when the ones on screen
    // happen to be unchanged.
    injection.builtFor.clear();
    injection.builtFor.reserve(displays.size());
    for (const brightness::Display& d : displays) {
        injection.builtFor.push_back(d.stableId);
    }

    // A panel with nothing in it still carries its margin, which inside the
    // card is a blank strip under the sliders. That happens before the first
    // enumeration -- the early-inject path injects on purpose before the
    // displays are known -- and permanently when everything is hidden.
    panel.Visibility(wux::Visibility::Collapsed);

    auto links = std::make_shared<PanelLinks>();
    injection.links = links;
    for (const brightness::Display& d : displays) {
        // A hidden display is left out everywhere, all-displays rows included.
        if (IsHidden(d)) {
            continue;
        }
        if (d.transport != brightness::Transport::None) {
            links->brightnessIds.push_back(d.stableId);
        }
        if (d.contrastMax > 0) {
            links->contrastIds.push_back(d.stableId);
        }
    }

    // A power button needs another display to be seen from: turning off the
    // only screen leaves nothing to turn it back on with.
    const bool powerAllowed = g_showPowerButton && displays.size() >= 2;
    bool anyPower = false;
    if (g_showPerMonitor && powerAllowed) {
        for (const brightness::Display& d : displays) {
            const DisplayRule* rule = RuleFor(d);
            if (d.hasPower && d.transport == brightness::Transport::DdcCi && !IsHidden(d) &&
                !(rule && rule->hidePower)) {
                anyPower = true;
            }
        }
    }
    const double trailing = anyPower ? kTrailingWidth : 0;
    panel.Margin(PanelMargin(trailing));

    // All-displays rows, where they drive more than one display -- or where
    // the per-display row they would duplicate is hidden, since then they
    // are the only way to reach that display at all.
    const bool perDisplayBrightness = g_showPerMonitor;
    const bool perDisplayContrast = g_showContrast;
    const bool masterBrightness =
        g_showMasterBrightness &&
        (links->brightnessIds.size() >= 2 ||
         (!perDisplayBrightness && !links->brightnessIds.empty()));
    const bool masterContrast =
        g_showMasterContrast &&
        (links->contrastIds.size() >= 2 ||
         (!perDisplayContrast && !links->contrastIds.empty()));
    if (masterBrightness || masterContrast) {
        const int average = AverageBrightness(displays, links->brightnessIds);
        wuxc::TextBlock title = MakeRowTitle(L"All displays");
        panel.Children().Append(title);
        auto rowTitle = std::make_shared<RowTitle>();
        rowTitle->name = L"All displays";
        rowTitle->text = title;

        if (masterBrightness) {
            wux::FrameworkElement icon = MakeBrightnessIcon();
            wuxc::Slider slider = MakeSlider(average < 0 ? 50 : average, 1.0);
            SetIconLevel(icon, slider.Value());
            panel.Children().Append(
                MakeSliderRow(MakeIconHost(icon, slider, injection), slider, nullptr, trailing));
            links->masterBrightness = winrt::make_weak(slider);
            links->masterBrightnessIcon = winrt::make_weak(icon);
            rowTitle->sliders.push_back({winrt::make_weak(slider), false});
            AttachWheel(slider, injection);
            injection.revokers.push_back(slider.ValueChanged(
                winrt::auto_revoke,
                [links, icon, rowTitle](
                    wf::IInspectable const&,
                    wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
                    const int value = static_cast<int>(std::lround(args.NewValue()));
                    SetIconLevel(icon, value);
                    UpdateRowTitle(*rowTitle);
                    if (g_suppressValueChanged || !g_engine) {
                        return;
                    }
                    for (const std::wstring& id : links->brightnessIds) {
                        g_engine->SetPercent(id, value);
                    }
                    // Carry the per-display rows along without writing them a
                    // second time.
                    SuppressValueChanged guard;
                    for (auto& entry : links->brightness) {
                        if (auto s = entry.second.get()) {
                            s.Value(value);
                        }
                    }
                }));
        }

        if (masterContrast) {
            const int average = AverageContrast(displays, links->contrastIds);
            wux::FrameworkElement icon = MakeContrastIcon();
            wuxc::Slider slider = MakeSlider(average < 0 ? 50 : average, 1.0);
            SetIconLevel(icon, slider.Value());
            panel.Children().Append(
                MakeSliderRow(MakeIconHost(icon, slider, injection), slider, nullptr, trailing));
            links->masterContrast = winrt::make_weak(slider);
            links->masterContrastIcon = winrt::make_weak(icon);
            rowTitle->sliders.push_back({winrt::make_weak(slider), false});
            AttachWheel(slider, injection);
            injection.revokers.push_back(slider.ValueChanged(
                winrt::auto_revoke,
                [links, icon, rowTitle](
                    wf::IInspectable const&,
                    wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
                    const int value = static_cast<int>(std::lround(args.NewValue()));
                    SetIconLevel(icon, value);
                    UpdateRowTitle(*rowTitle);
                    if (g_suppressValueChanged || !g_engine) {
                        return;
                    }
                    for (const std::wstring& id : links->contrastIds) {
                        g_engine->SetContrast(id, value);
                    }
                    SuppressValueChanged guard;
                    for (auto& entry : links->contrast) {
                        if (auto s = entry.second.get()) {
                            s.Value(value);
                        }
                    }
                }));
        }
        UpdateRowTitle(*rowTitle);
    }

    // Which of a display's other controls it gets: each needs its setting on,
    // the monitor to support it, and no per-display rule hiding it.
    struct Extras {
        bool contrast = false;
        bool volume = false;
        bool input = false;
        bool any() const { return contrast || volume || input; }
    };
    auto extrasFor = [](const brightness::Display& d) {
        const DisplayRule* rule = RuleFor(d);
        Extras extras;
        extras.contrast = g_showContrast && d.contrastMax > 0 && !(rule && rule->hideContrast);
        extras.volume = g_showVolume && d.volumeMax > 0 && !(rule && rule->hideVolume);
        extras.input =
            g_showInputSwitcher && d.inputs.size() >= 2 && !(rule && rule->hideInput);
        return extras;
    };

    // A display gets a section for its brightness slider -- or, with those
    // turned off, for whatever other controls it has. "Brightness control not
    // supported" is about brightness, so that note goes with the sliders.
    auto hasRow = [&](const brightness::Display& d) {
        if (IsHidden(d)) {
            return false;
        }
        const bool controllable = d.transport != brightness::Transport::None;
        if (g_showPerMonitor) {
            return controllable || !g_hideUnsupported;
        }
        return controllable && extrasFor(d).any();
    };

    // The names rows are shown under, with identical ones numbered left to
    // right. Two monitors of the same model report the same name, and a
    // custom name whose text matches both gives them the same one again;
    // without a number the rows could not be told apart. Only displays that
    // get a row are counted, so a hidden one does not leave a gap.
    std::vector<std::wstring> labels(displays.size());
    std::map<std::wstring, int> labelCount;
    for (size_t i = 0; i < displays.size(); ++i) {
        if (hasRow(displays[i])) {
            labels[i] = DisplayLabel(displays[i]);
            ++labelCount[labels[i]];
        }
    }
    std::map<std::wstring, int> labelSeen;
    for (size_t i = 0; i < displays.size(); ++i) {
        if (hasRow(displays[i]) && labelCount[labels[i]] > 1) {
            const int number = ++labelSeen[labels[i]];
            labels[i] += L" " + std::to_wstring(number);
        }
    }

    for (size_t index = 0; index < displays.size(); ++index) {
        const brightness::Display& d = displays[index];
        const bool controllable = d.transport != brightness::Transport::None;
        if (!hasRow(d)) {
            continue;
        }

        const std::wstring displayName = labels[index];
        // The 50 is a placeholder for a value not read *yet*, and only a
        // controllable display has one coming -- the first refresh replaces
        // it. An uncontrollable one has no value and never will, so showing
        // it "50%" directly above "Brightness control not supported" would
        // invent a reading that does not exist; it gets no slider, and so no
        // percentage in its title either.
        const int shownPercent = d.percent < 0 ? 50 : d.percent;
        const DisplayRule* rule = RuleFor(d);
        const Extras extras = extrasFor(d);
        const bool hasContrast = extras.contrast;
        const bool hasVolume = extras.volume;
        const bool hasInput = extras.input;
        // The dropdown keeps the brightness slider out and folds the rest
        // away. With no brightness slider, it would fold away everything and
        // leave a bare title, so the rest is shown instead.
        const bool dropdown =
            g_layoutStyle == LayoutStyle::Dropdown && g_showPerMonitor && extras.any();
        const bool expanded = !dropdown || g_expanded.count(d.stableId) > 0;

        // Title, with the dropdown's chevron at the far end when there is
        // anything to drop down.
        wuxc::TextBlock title = MakeRowTitle(winrt::hstring{displayName});
        auto rowTitle = std::make_shared<RowTitle>();
        rowTitle->name = displayName;
        rowTitle->text = title;
        // The device id, for telling identical monitors apart when naming
        // them: the part after the last "&" names the output it is plugged
        // into.
        wuxc::ToolTipService::SetToolTip(title, winrt::box_value(winrt::hstring{d.stableId}));
        wuxc::StackPanel details{nullptr};
        if (dropdown && controllable) {
            wuxc::Grid titleRow;
            wuxc::ColumnDefinition textColumn;
            textColumn.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
            wuxc::ColumnDefinition chevronColumn;
            chevronColumn.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
            titleRow.ColumnDefinitions().Append(textColumn);
            titleRow.ColumnDefinitions().Append(chevronColumn);
            wuxc::Grid::SetColumn(title, 0);
            titleRow.Children().Append(title);

            wuxc::Button chevron = MakeGlyphButton(expanded ? L"\xE70E" : L"\xE70D", 10,
                                                   trailing > 0 ? trailing : 28, 22);
            chevron.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
            wuxc::ToolTipService::SetToolTip(chevron, winrt::box_value(L"More controls"));
            wuxc::Grid::SetColumn(chevron, 1);
            titleRow.Children().Append(chevron);
            panel.Children().Append(titleRow);

            details = wuxc::StackPanel();
            rowTitle->details = details;
            std::wstring id = d.stableId;
            injection.clickRevokers.push_back(chevron.Click(
                winrt::auto_revoke,
                [id, rowTitle, weakChevron = winrt::make_weak(chevron),
                 weakDetails = winrt::make_weak(details)](wf::IInspectable const&,
                                                          wux::RoutedEventArgs const&) {
                    const bool open = g_expanded.count(id) == 0;
                    if (open) {
                        g_expanded.insert(id);
                    } else {
                        g_expanded.erase(id);
                    }
                    if (auto panelOfDetails = weakDetails.get()) {
                        panelOfDetails.Visibility(open ? wux::Visibility::Visible
                                                       : wux::Visibility::Collapsed);
                    }
                    if (auto c = weakChevron.get()) {
                        if (auto glyph = c.Content().try_as<wuxc::FontIcon>()) {
                            glyph.Glyph(open ? L"\xE70E" : L"\xE70D");
                        }
                    }
                    // The dropdown's values join the title, or leave it.
                    UpdateRowTitle(*rowTitle);
                }));
        } else {
            panel.Children().Append(title);
        }

        if (!controllable) {
            // Say so rather than showing a slider that does nothing.
            wuxc::TextBlock note;
            note.Text(L"Brightness control not supported");
            note.FontSize(11);
            note.Opacity(0.55);
            panel.Children().Append(note);
            continue;
        }

        std::wstring id = d.stableId;
        Injection::Binding binding{id, displayName, {}, {}, rowTitle};

        // The brightness row, and the power button that sits at its end.
        if (g_showPerMonitor) {
            wux::FrameworkElement icon = MakeBrightnessIcon();
            // Snap to what the hardware can actually represent. A monitor whose
            // VCP range is 0-50 has 2% granularity, so offering 1% steps just
            // means three slider positions that all round to the same raw value.
            double step = 1.0;
            if (d.transport == brightness::Transport::DdcCi && d.vcpMax > 0) {
                step = std::max(1.0, 100.0 / static_cast<double>(d.vcpMax));
            }
            wuxc::Slider slider = MakeSlider(shownPercent, step);
            SetIconLevel(icon, slider.Value());

            wuxc::Button power{nullptr};
            if (powerAllowed && d.hasPower && d.transport == brightness::Transport::DdcCi &&
                !(rule && rule->hidePower)) {
                power = MakePowerButton(d, displayName, injection);
            }
            panel.Children().Append(MakeSliderRow(MakeIconHost(icon, slider, injection), slider,
                                                  power, trailing));
            AttachWheel(slider, injection);

            // auto_revoke so the handler can be detached on unload; a dangling
            // registration into an unloaded DLL is a crash, not a leak.
            rowTitle->sliders.push_back({winrt::make_weak(slider), false});
            injection.revokers.push_back(slider.ValueChanged(
                winrt::auto_revoke,
                [id, icon, rowTitle, links](
                    wf::IInspectable const&,
                    wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
                    SetIconLevel(icon, args.NewValue());
                    UpdateRowTitle(*rowTitle);
                    if (g_suppressValueChanged) {
                        // Echo of a refresh or an all-displays move we just wrote
                        // into the slider; writing it back to the hardware would
                        // be a pointless round trip.
                        return;
                    }
                    if (g_engine) {
                        // Returns immediately; the worker coalesces and writes.
                        g_engine->SetPercent(id, static_cast<int>(std::lround(args.NewValue())));
                    }
                    SyncMasters(*links);
                }));
            links->brightness.emplace_back(id, winrt::make_weak(slider));

            binding.slider = winrt::make_weak(slider);
            binding.icon = winrt::make_weak(icon);
            if (power) {
                binding.power = winrt::make_weak(power);
            }
        }

        // The extra rows go into the dropdown when there is one, straight into
        // the panel otherwise.
        auto appendExtra = [&](wux::UIElement const& row) {
            if (details) {
                details.Children().Append(row);
            } else {
                panel.Children().Append(row);
            }
        };

        if (hasContrast) {
            wux::FrameworkElement contrastIcon = MakeContrastIcon();
            double contrastStep = std::max(1.0, 100.0 / static_cast<double>(d.contrastMax));
            wuxc::Slider contrast =
                MakeSlider(d.contrast < 0 ? 50 : d.contrast, contrastStep);
            SetIconLevel(contrastIcon, contrast.Value());
            appendExtra(MakeSliderRow(MakeIconHost(contrastIcon, contrast, injection), contrast,
                                      nullptr, trailing));
            rowTitle->sliders.push_back({winrt::make_weak(contrast), details != nullptr});
            AttachWheel(contrast, injection);
            injection.revokers.push_back(contrast.ValueChanged(
                winrt::auto_revoke,
                [id, contrastIcon, rowTitle, links](
                    wf::IInspectable const&,
                    wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
                    SetIconLevel(contrastIcon, args.NewValue());
                    UpdateRowTitle(*rowTitle);
                    if (g_suppressValueChanged) {
                        return;
                    }
                    if (g_engine) {
                        g_engine->SetContrast(id, static_cast<int>(std::lround(args.NewValue())));
                    }
                    SyncMasters(*links);
                }));
            links->contrast.emplace_back(id, winrt::make_weak(contrast));
            binding.contrast = winrt::make_weak(contrast);
            binding.contrastIcon = winrt::make_weak(contrastIcon);
        }

        if (hasVolume) {
            double volumeStep = std::max(1.0, 100.0 / static_cast<double>(d.volumeMax));
            wuxc::Slider volume = MakeSlider(d.volume < 0 ? 50 : d.volume, volumeStep);
            wux::FrameworkElement volumeIcon = MakeVolumeIcon();
            SetVolumeIconLevel(volumeIcon, volume.Value());
            appendExtra(MakeSliderRow(MakeIconHost(volumeIcon, volume, injection), volume,
                                      nullptr, trailing));
            rowTitle->sliders.push_back({winrt::make_weak(volume), details != nullptr});
            AttachWheel(volume, injection);
            injection.revokers.push_back(volume.ValueChanged(
                winrt::auto_revoke,
                [id, volumeIcon, rowTitle](
                    wf::IInspectable const&,
                    wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
                    SetVolumeIconLevel(volumeIcon, args.NewValue());
                    UpdateRowTitle(*rowTitle);
                    if (g_suppressValueChanged || !g_engine) {
                        return;
                    }
                    g_engine->SetVolume(id, static_cast<int>(std::lround(args.NewValue())));
                }));
            binding.volume = winrt::make_weak(volume);
            binding.volumeIcon = winrt::make_weak(volumeIcon);
        }

        if (hasInput) {
            appendExtra(MakeInputRow(d, trailing, injection, binding));
        }

        if (details) {
            details.Visibility(expanded ? wux::Visibility::Visible
                                        : wux::Visibility::Collapsed);
            panel.Children().Append(details);
        }
        UpdateRowTitle(*rowTitle);

        injection.bindings.push_back(std::move(binding));
    }

    if (panel.Children().Size() > 0) {
        panel.Visibility(wux::Visibility::Visible);
    }
}

wuxc::StackPanel BuildSliderPanel(Injection& injection) {
    wuxc::StackPanel panel;
    panel.Name(L"WindhawkPerMonitorBrightness");
    panel.Orientation(wuxc::Orientation::Vertical);
    panel.Margin(PanelMargin(0));
    PopulateSliderPanel(panel, injection);
    return panel;
}

// Collapses the stock brightness row, which duplicates the internal-panel
// slider this mod already provides. The sliders live in a virtualized GridView,
// so rather than guess at an index we find the row by the animated sun icon it
// carries -- the volume row has no such element -- and walk up to its item.
bool TryHideStockBrightness(wux::FrameworkElement const& l1Grid) {
    if (g_stockSlider.hidden) {
        if (g_stockSlider.item.get()) {
            return true;  // still the tree we hid
        }
        // The view was rebuilt: our weak refs point into the dead tree, so the
        // new one has an unhidden stock row and unload would restore nothing.
        g_stockSlider = StockSliderState{};
    }

    auto group = FindDescendant(
        l1Grid, L"Windows.UI.Xaml.Controls.ContentControl#SlidersGroup", 6);
    if (!group) {
        return false;
    }

    // Generous depth: the icon lives inside the slider's template, roughly 20
    // levels below the group once the item container and presenters are
    // counted. The first version used 16 and never reached it.
    auto icon = FindDescendantByName(group, L"BrightnessPlayer", 40);
    if (!icon) {
        return false;  // rows not realized yet, or renamed
    }

    auto item = FindAncestorOfClass(
        icon, L"Windows.UI.Xaml.Controls.GridViewItem", 24);
    if (!item) {
        if (!g_loggedHideMiss) {
            g_loggedHideMiss = true;
            Wh_Log(L"Found BrightnessPlayer but no GridViewItem above it");
        }
        return false;
    }

    auto itemFe = item.try_as<wux::FrameworkElement>();
    auto groupFe = group.try_as<wux::FrameworkElement>();
    if (!itemFe || !groupFe) {
        return false;
    }

    // The group is sized for a fixed number of sliders, so collapsing a row on
    // its own leaves a gap. If the height is fixed but the row has not been
    // measured yet, wait rather than collapse and leave a hole.
    double groupHeight = groupFe.Height();
    double itemHeight = itemFe.ActualHeight();
    bool fixedHeight = !std::isnan(groupHeight);
    if (fixedHeight && itemHeight <= 0) {
        return false;
    }

    g_stockSlider.item = winrt::make_weak(itemFe);
    g_stockSlider.visibility = itemFe.Visibility();
    g_stockSlider.group = winrt::make_weak(groupFe);
    g_stockSlider.groupHeight = groupHeight;

    itemFe.Visibility(wux::Visibility::Collapsed);

    if (fixedHeight && groupHeight > itemHeight) {
        groupFe.Height(groupHeight - itemHeight);
        Wh_Log(L"Hid stock brightness row; SlidersGroup %.0f -> %.0f (row %.0f)",
               groupHeight, groupHeight - itemHeight, itemHeight);
    } else {
        Wh_Log(L"Hid stock brightness row; SlidersGroup height Auto (row %.0f)",
               itemHeight);
    }

    g_stockSlider.hidden = true;
    return true;
}

// The sliders are virtualized, so the brightness row often does not exist at
// injection time -- it is created when the panel is first shown. Retry on each
// layout pass until it appears.
void ArmStockSliderHide(wux::FrameworkElement const& l1Grid) {
    if (TryHideStockBrightness(l1Grid)) {
        return;
    }

    g_hideRetry.Revoke();
    g_hideRetry.layout = l1Grid.LayoutUpdated(
        winrt::auto_revoke,
        [l1Grid](wf::IInspectable const&, wf::IInspectable const&) {
            if (TryHideStockBrightness(l1Grid) ||
                ++g_hideRetry.attempts >= kMaxRetryAttempts) {
                // On a desktop there is no internal panel and therefore no
                // BrightnessPlayer to find, so without this it would retry on
                // every layout pass forever.
                g_hideRetry.Revoke();
            }
        });
}

// Called when the Control Center is actually opened, which is the event that
// matters: the sliders are virtualized, so the stock brightness row is created
// when the panel is first shown, not when the view is built.
//
// Arming used to happen only at injection time and was capped at
// kMaxRetryAttempts layout passes. The whole budget went on guessing when the
// row would be realized, and if it ran out the default-on setting silently
// stopped working for the rest of the session with no second chance. Keyed off
// the open instead, the cap is harmless: every open brings a fresh attempt.
void ReArmStockSliderHide() {
    if (!g_hideStockBrightness) {
        return;
    }
    try {
        wux::FrameworkElement grid{nullptr};
        {
            std::lock_guard<std::mutex> lock(g_injectionsMutex);
            if (!g_injections) {
                return;
            }
            for (const Injection& i : *g_injections) {
                if (auto live = i.grid.get()) {
                    grid = live.try_as<wux::FrameworkElement>();
                    if (grid) {
                        break;
                    }
                }
            }
        }
        if (grid) {
            g_hideRetry.attempts = 0;
            ArmStockSliderHide(grid);
        }
    } catch (...) {
        Wh_Log(L"ReArmStockSliderHide threw: %08X", winrt::to_hresult());
    }
}

// Which row of L1Grid the panel should take, and what has to move for it.
//
// Rows are chosen by reading the grid rather than by hardcoding indices: the
// layout below is what 26200 has, and a build that reorders it should degrade
// to "at the bottom" rather than land the panel somewhere absurd.
//
//   [0] Border                      row=0 span=2   the card
//   [1] ContentControl#TogglesGroup row=0
//   [2] ContentControl#SlidersGroup row=1
//   [3] Grid#FooterGrid             row=2          outside the card
//
// Returns the row for the panel. Anything at or below it is pushed down one,
// recorded in `injection` so teardown can put it back.
int PlacePanelRow(wuxc::Grid const& grid, Injection& injection) {
    const int appendedRow = static_cast<int>(grid.RowDefinitions().Size()) - 1;

    if (g_panelPosition == PanelPosition::Bottom) {
        return appendedRow;  // after everything; nothing else moves
    }

    // The native sliders are the anchor. Without them there is nothing to be
    // above or below, which is the case on a build that renamed the group.
    wux::FrameworkElement sliders{nullptr};
    for (uint32_t i = 0; i < grid.Children().Size(); ++i) {
        auto fe = grid.Children().GetAt(i).try_as<wux::FrameworkElement>();
        if (fe && ElementLabel(fe) ==
                      L"Windows.UI.Xaml.Controls.ContentControl#SlidersGroup") {
            sliders = fe;
            break;
        }
    }
    if (!sliders) {
        Wh_Log(L"SlidersGroup not among L1Grid's children; placing the panel "
               L"at the bottom instead");
        return appendedRow;
    }

    const int slidersRow = wuxc::Grid::GetRow(sliders);
    const int wantedRow = (g_panelPosition == PanelPosition::AboveSliders)
                              ? slidersRow
                              : slidersRow + 1;

    // Everything from the wanted row down moves one row further down. Our own
    // panel is not in the tree yet, so this cannot catch it.
    for (uint32_t i = 0; i < grid.Children().Size(); ++i) {
        auto fe = grid.Children().GetAt(i).try_as<wux::FrameworkElement>();
        if (!fe) {
            continue;
        }
        const int row = wuxc::Grid::GetRow(fe);
        const int span = wuxc::Grid::GetRowSpan(fe);

        if (row >= wantedRow) {
            injection.movedChildren.push_back({winrt::make_weak(fe), row});
            wuxc::Grid::SetRow(fe, row + 1);
            continue;
        }

        // The card is whatever spans the sliders row. Widening it by one is
        // what puts the panel inside it rather than underneath it.
        //
        // The test is against the sliders row, not the row being taken. Asking
        // whether the card crosses the *new* row is only true when the new row
        // is above the sliders: for "below", the new row sits immediately
        // after the card's last row, 0 + 2 > 2 is false, and the default
        // position quietly landed outside the card -- which is the very look
        // the issue was about. The sliders row is inside the card by
        // definition, on both sides of it.
        if (span > 1 && row + span > slidersRow && !injection.cardBorder.get()) {
            // Widened to a computed span, not by adding one.
            //
            // Adding one is not idempotent, and the thing it is not idempotent
            // across is mod reloads, not injections. InjectInto returns early
            // on a grid that already holds the panel, so this runs once per
            // view -- and the measurements agree: the card was seen at span 5
            // while the footer had moved exactly one row, which three runs of
            // this function could not produce.
            //
            // What outlives a reload is the view. It is built on first use and
            // kept (see g_injecting), so installing a new build leaves the
            // same Border in place, and each instance stretched it again from
            // wherever the last one left it. A fresh ShellHost with a single
            // injection reports span 3, which is the value below.
            //
            // The span the card wants is fixed: from its own row through the
            // lowest row it has to cover, which is our panel for "below" and
            // the sliders in their new position for "above". Computing it
            // means an instance inheriting a stretched card settles on the
            // right value instead of adding to it.
            const int lastCovered =
                (g_panelPosition == PanelPosition::AboveSliders) ? slidersRow + 1
                                                                 : wantedRow;
            const int wantedSpan = lastCovered - row + 1;
            if (span < wantedSpan) {
                injection.cardBorder = winrt::make_weak(fe);
                injection.originalCardRowSpan = span;
                wuxc::Grid::SetRowSpan(fe, wantedSpan);
            } else if (span > wantedSpan) {
                // Left stretched by an instance whose unload could not reach
                // the XAML thread. Recorded but not changed: shrinking someone
                // else's leftover to a value this instance never measured
                // would be guessing, and it is visible here if it ever
                // matters.
                Wh_Log(L"Card arrived at span %d, wider than the %d this "
                       L"layout needs; leaving it alone",
                       span, wantedSpan);
            }
        }
    }

    int cardSpanNow = 0;
    if (auto card = injection.cardBorder.get()) {
        cardSpanNow = wuxc::Grid::GetRowSpan(card);
    }
    Wh_Log(L"Panel takes row %d (sliders were row %d); moved %zu child(ren) "
           L"down, card span %d -> %d",
           wantedRow, slidersRow, injection.movedChildren.size(),
           injection.originalCardRowSpan, cardSpanNow);
    return wantedRow;
}


// How far the lowest realized native track sits above the bottom of the
// group. The first row gives the top inset; only the last row gives this one,
// and which row that is depends on how many the shell drew.
double LowestSliderBottom(wux::DependencyObject const& node,
                          wux::FrameworkElement const& origin, int maxDepth) {
    double lowest = 0;
    if (maxDepth < 0) {
        return lowest;
    }
    if (ElementLabel(node).find(L"AsyncSlider") != std::wstring::npos) {
        if (auto fe = node.try_as<wux::FrameworkElement>()) {
            if (fe.ActualHeight() > 0) {
                auto point = fe.TransformToVisual(origin).TransformPoint({0, 0});
                lowest = point.Y + fe.ActualHeight();
            }
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        lowest = std::max(lowest,
                          LowestSliderBottom(wuxm::VisualTreeHelper::GetChild(node, i),
                                             origin, maxDepth - 1));
    }
    return lowest;
}

// The first realized native slider, and the icon sharing its row.
void MeasureNativeSliderMetrics(wux::FrameworkElement const& l1Grid) try {
    auto group = FindDescendant(
        l1Grid, L"Windows.UI.Xaml.Controls.ContentControl#SlidersGroup", 6);
    if (!group) {
        return;
    }
    auto groupFe = group.try_as<wux::FrameworkElement>();
    if (!groupFe || groupFe.ActualWidth() <= 0) {
        return;
    }

    // Any slider row will do. The brightness one is the obvious candidate and
    // the wrong choice: it is absent on a desktop, and this mod hides it by
    // default on a laptop, so the volume row is usually the only one left.
    auto slider = FindDescendantByClassSubstring(group, L"AsyncSlider", 30);
    if (!slider) {
        return;
    }
    auto sliderFe = slider.try_as<wux::FrameworkElement>();
    if (!sliderFe || sliderFe.ActualWidth() <= 0) {
        return;
    }

    auto row = FindAncestorOfClass(
        slider, L"Windows.UI.Xaml.Controls.GridViewItem", 24);
    auto icon = row ? FindDescendantByClassSubstring(row, L"AnimatedIcon", 24)
                    : nullptr;

    SliderMetrics measured;
    measured.groupWidth = groupFe.ActualWidth();

    auto point = sliderFe.TransformToVisual(l1Grid).TransformPoint({0, 0});
    measured.sliderLeft = point.X;
    measured.sliderRight = point.X + sliderFe.ActualWidth();
    measured.rowHeight = sliderFe.ActualHeight();

    if (auto iconFe = icon ? icon.try_as<wux::FrameworkElement>() : nullptr) {
        measured.iconLeft =
            iconFe.TransformToVisual(l1Grid).TransformPoint({0, 0}).X;
    } else {
        // Keep the gap the default describes rather than inventing one.
        measured.iconLeft = measured.sliderLeft - 34;
    }

    // What the panel has to give back so that the seam between it and the
    // group looks like one more row boundary. Both halves come off the same
    // row: how far the first slider sits below the top of the group, and how
    // far two native sliders sit apart, which is twice the item's margin plus
    // twice the slider's inset within it.
    if (auto rowFe = row ? row.try_as<wux::FrameworkElement>() : nullptr) {
        const double groupTop =
            groupFe.TransformToVisual(l1Grid).TransformPoint({0, 0}).Y;
        const double rowTop =
            rowFe.TransformToVisual(l1Grid).TransformPoint({0, 0}).Y;
        const double sliderInset = point.Y - rowTop;
        const double nativeRowGap = 2 * (rowFe.Margin().Top + sliderInset);
        measured.gapAbove = nativeRowGap - (point.Y - groupTop);

        const double groupBottom = groupTop + groupFe.ActualHeight();
        // Same depth as the search that found the slider in the first place.
        // At 12 a tree any deeper would silently keep the default seam while
        // every other metric was measured, which is the sort of half-measured
        // result that is worse than either.
        const double lowest = LowestSliderBottom(group, l1Grid, 30);
        if (lowest > 0 && groupBottom > lowest) {
            measured.gapBelow = nativeRowGap - (groupBottom - lowest);
        }
    }

    // A row that measures as nonsense is worse than the defaults.
    if (measured.sliderRight <= measured.sliderLeft ||
        measured.iconLeft < 0 || measured.iconLeft >= measured.sliderLeft ||
        measured.sliderRight > measured.groupWidth || measured.rowHeight < 16) {
        Wh_Log(L"Native slider metrics look wrong (icon %.0f slider %.0f..%.0f "
               L"of %.0f, h %.0f); keeping the defaults",
               measured.iconLeft, measured.sliderLeft, measured.sliderRight,
               measured.groupWidth, measured.rowHeight);
        return;
    }

    g_metrics = measured;
    Wh_Log(L"Native slider row: icon at %.0f, slider %.0f..%.0f of %.0f, "
           L"height %.0f, seams %.0f/%.0f",
           g_metrics.iconLeft, g_metrics.sliderLeft, g_metrics.sliderRight,
           g_metrics.groupWidth, g_metrics.rowHeight, g_metrics.gapAbove,
           g_metrics.gapBelow);
} catch (...) {
    Wh_Log(L"Measuring the native slider row threw: %08X", winrt::to_hresult());
}

bool InjectInto(wux::FrameworkElement const& l1Grid) {
    auto grid = l1Grid.try_as<wuxc::Grid>();
    if (!grid) {
        Wh_Log(L"L1Grid is not a Grid (%s) -- not injecting",
               ElementLabel(l1Grid).c_str());
        return false;
    }

    if (FindDescendant(grid, L"Windows.UI.Xaml.Controls.StackPanel#WindhawkPerMonitorBrightness", 6)) {
        Wh_Log(L"Panel already present, skipping");
        // Not a no-op: the panel surviving does not mean the stock row was
        // ever found. This path used to return before arming, so a hide that
        // had not managed to land yet never got another attempt.
        if (g_hideStockBrightness) {
            ArmStockSliderHide(l1Grid);
        }
        return true;
    }

    uint32_t rowCount = grid.RowDefinitions().Size();
    Wh_Log(L"L1Grid has %u RowDefinition(s), %u child(ren)", rowCount,
           grid.Children().Size());

    // L1Grid's row layout is not documented and has changed across builds.
    // Appending a row is only safe when the grid already declares
    // RowDefinitions; otherwise every existing child implicitly lives in row 0
    // and adding a definition would re-flow the whole panel.
    if (rowCount == 0) {
        // Either the grid genuinely has no rows (in which case appending one
        // would re-flow every existing child out of row 0), or it is not built
        // yet and a later retry will succeed. Either way, log the structure
        // and leave the UI untouched.
        if (!g_dumpedTree.exchange(true)) {
            Wh_Log(L"L1Grid declares no rows; refusing to re-flow it. "
                   L"Tree follows:");
            DumpTree(grid, 0, 4);
        }
        return false;
    }

    // Before building the panel: the icons need the shell's Lottie, and it has
    // to be read while the stock row is still visible. The slider style comes
    // from the same rows. Either may not exist yet; see TryCaptureShellLook.
    TryCaptureShellLook(l1Grid);

    // Likewise before building: the rows are laid out from these.
    MeasureNativeSliderMetrics(l1Grid);

    Injection injection;
    wuxc::StackPanel panel = BuildSliderPanel(injection);

    wuxc::RowDefinition row;
    row.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    // Appended, never inserted. RowDefinitions are positional, so inserting
    // one in the middle would silently renumber every row below it while the
    // children's Grid.Row values stayed put -- the same renumbering, done
    // invisibly and in the wrong direction. Appending adds capacity at the
    // end and leaves the arithmetic to PlacePanelRow, which records it.
    grid.RowDefinitions().Append(row);

    wuxc::Grid::SetRow(panel, PlacePanelRow(grid, injection));
    grid.Children().Append(panel);

    injection.grid = winrt::make_weak(grid);
    injection.panel = winrt::make_weak(panel);
    injection.row = row;

    if (g_hideStockBrightness) {
        ArmStockSliderHide(l1Grid);
    }

    {
        std::lock_guard<std::mutex> lock(g_injectionsMutex);
        // The shell discards an idle Control Center and builds a new one on
        // the next open, so prune the entries whose tree it has already torn
        // down.
        std::erase_if(*g_injections, [](const Injection& i) {
            return !i.grid.get() || !i.panel.get();
        });
        g_injections->push_back(std::move(injection));
    }

    // We are on the XAML thread here; this is how everything else gets back to
    // it. There was also a XamlRoot.Changed subscription here as a secondary
    // "flyout shown" signal, but it never fires for this host on 26200 and the
    // revoker was being assigned to a moved-from local anyway, so it was doing
    // nothing at all. The WinEvent hook in ShellEventWatcher is the real one.
    g_xamlThreadId.store(GetCurrentThreadId());

    Wh_Log(L"Injected per-monitor brightness panel into row %u", rowCount);
    return true;
}

// Undoes every injection. Must run on the XAML thread.
void RemoveInjections() {
    // First: these live on the shell's own elements and are owned by nothing
    // else. Left registered, they call into this DLL after Windhawk frees it.
    g_injectRetry.Revoke();

    // The early-inject timer belongs here too, and only here.
    //
    // A DispatcherTimer is UI-thread affine: stopping one from Windhawk's
    // thread fails with RPC_E_WRONG_THREAD, which C++/WinRT raises as an
    // exception -- and out of Wh_ModUninit that is a crash, not a log line.
    // RemoveInjections already runs on the XAML thread for the same reason
    // the revokers above do, and it is the one place that must also stop a
    // tick from re-injecting into the tree it has just restored.
    if (g_earlyInject) {
        g_earlyInject.Stop();
        g_earlyInject = nullptr;
    }
    g_hideRetry.Revoke();

    std::vector<Injection> injections;
    {
        std::lock_guard<std::mutex> lock(g_injectionsMutex);
        injections.swap(*g_injections);
    }

    // Unhide the stock slider first, independently of our own panel: it may
    // have been hidden by a retry long after the injection was recorded.
    try {
        if (auto item = g_stockSlider.item.get()) {
            item.Visibility(g_stockSlider.visibility);
            Wh_Log(L"Restored stock brightness row");
        }
        if (auto group = g_stockSlider.group.get()) {
            // NaN restores Auto, which is what it was if we never set it.
            group.Height(g_stockSlider.groupHeight);
        }
    } catch (...) {
        Wh_Log(L"Restoring stock slider failed: %08X", winrt::to_hresult());
    }
    g_stockSlider = StockSliderState{};

    // Release the borrowed Lottie and slider style here, on the XAML thread,
    // rather than letting a global destructor drop them after the DLL is gone.
    g_brightnessSource = nullptr;
    g_volumeSource = nullptr;
    g_capturedVolume = false;
    g_lookRetries = 0;
    g_sliderStyle = nullptr;
    g_animatedIconStatics = nullptr;
    g_capturedSource = false;
    g_progressMin = 0.0;
    g_progressMax = 1.0;

    for (Injection& injection : injections) {
        try {
            // Detach handlers before anything else, so nothing can fire at a
            // half-removed panel.
            injection.DetachHandlers();

            auto grid = injection.grid.get();
            auto panel = injection.panel.get();
            if (!grid) {
                continue;
            }

            if (panel) {
                uint32_t index = 0;
                if (grid.Children().IndexOf(panel, index)) {
                    grid.Children().RemoveAt(index);
                }
            }

            // Put the shell's own children back on their original rows before
            // the appended RowDefinition goes. The other order leaves a child
            // addressing a row that no longer exists, which XAML resolves by
            // clamping it into the last row -- the footer lands on top of the
            // sliders, and it looks like the mod broke the flyout on the way
            // out.
            for (Injection::MovedChild& moved : injection.movedChildren) {
                if (auto element = moved.element.get()) {
                    wuxc::Grid::SetRow(element, moved.originalRow);
                }
            }
            if (auto card = injection.cardBorder.get()) {
                wuxc::Grid::SetRowSpan(card, injection.originalCardRowSpan);
            }

            // Remove the row we appended, but only that one -- match by
            // identity rather than by index, which may have shifted.
            auto row = injection.row;
            if (!row) {
                Wh_Log(L"No RowDefinition recorded; %u row(s) left",
                       grid.RowDefinitions().Size());
                continue;
            }

            auto rows = grid.RowDefinitions();
            uint32_t index = 0;
            bool found = rows.IndexOf(row, index);
            if (!found) {
                // XAML's RowDefinitionCollection does not implement IndexOf
                // usefully, so fall back to an explicit identity scan.
                for (uint32_t i = 0; i < rows.Size(); ++i) {
                    if (rows.GetAt(i) == row) {
                        index = i;
                        found = true;
                        break;
                    }
                }
            }

            if (found) {
                rows.RemoveAt(index);
                Wh_Log(L"Removed appended RowDefinition at %u; %u row(s) left",
                       index, rows.Size());
            } else {
                // Loud on purpose: silently skipping this is what let empty
                // rows accumulate one per enable/disable cycle.
                Wh_Log(L"WARNING: appended RowDefinition not found among %u; "
                       L"leaking one empty row",
                       rows.Size());
            }
        } catch (...) {
            Wh_Log(L"RemoveInjections error: %08X", winrt::to_hresult());
        }
    }

    Wh_Log(L"Removed %zu injection(s)", injections.size());
}

// Runs fn on the XAML thread and does not return until it has finished.
//
// Deliberately not CoreDispatcher::RunAsync: that queues the work, so there is
// no point at which we can say nothing of ours is still scheduled -- and
// Wh_ModUninit must be able to say exactly that before Windhawk frees this
// DLL. SendMessage is synchronous by construction, so when it returns the
// callback has already run. This is the same WH_CALLWNDPROC trick the
// notification center styler uses for the same reason.
BOOL CALLBACK FindThreadWindow(HWND hwnd, LPARAM lParam) {
    *reinterpret_cast<HWND*>(lParam) = hwnd;
    return FALSE;  // first one will do; we only need somewhere to send to
}

bool RunOnXamlThread(std::function<void()> fn) {
    // The callback travels in the message's own lParam rather than a global,
    // which is how the styler mods do it. No shared pointer, so no mutex
    // serialising unrelated hops, and no way for the callee to still be
    // running after the caller has given up and destroyed the function.
    static const UINT kRunMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParam {
        std::function<void()>* fn;
        bool ran;
    };

    DWORD threadId = g_xamlThreadId.load();
    if (!threadId) {
        return false;  // nothing was ever injected
    }
    if (threadId == GetCurrentThreadId()) {
        try {
            fn();
        } catch (...) {
            Wh_Log(L"inline call threw: %08X", winrt::to_hresult());
            return false;
        }
        return true;
    }

    HWND target = nullptr;
    EnumThreadWindows(threadId, FindThreadWindow,
                      reinterpret_cast<LPARAM>(&target));
    if (!target) {
        Wh_Log(L"No window on the XAML thread; cannot marshal");
        return false;
    }
    if (!kRunMsg) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == kRunMsg && cwp->lParam) {
                    auto* param = reinterpret_cast<RunParam*>(cwp->lParam);
                    // Claimed before running, not marked after.
                    //
                    // Every hook in the chain sees every message, so with two
                    // of these installed at once the callback ran twice. That
                    // happens in ordinary use: opening the Control Center has
                    // the watcher thread marshalling ReArmStockSliderHide
                    // while the worker marshals ApplyRefreshedValues. The
                    // callees are idempotent, so it cost duplicated work
                    // rather than correctness -- but the primitive should
                    // only run what it was asked to run, once.
                    if (!param->ran) {
                        param->ran = true;
                        try {
                            (*param->fn)();
                        } catch (...) {
                            // Runs inside the shell's own dispatch; letting
                            // anything escape here takes the process down.
                            Wh_Log(L"marshalled call threw: %08X",
                                   winrt::to_hresult());
                        }
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        return false;
    }

    // A plain SendMessage, not SendMessageTimeout: it cannot return while the
    // callee is still using `param`, which is what makes the stack-allocated
    // parameter safe. Only ever called once injection has succeeded, so the
    // target thread is known to be pumping.
    RunParam param{&fn, false};
    SendMessage(target, kRunMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);

    if (!param.ran) {
        Wh_Log(L"Marshalled call did not run");
    }
    return param.ran;
}

// Rebuilds one panel's rows from the current display list. Caller holds
// g_injectionsMutex; XAML thread.
bool RebuildOneInjection(Injection& injection) {
    auto panel = injection.panel.get();
    if (!panel) {
        return false;
    }
    // Detach before discarding the controls the handlers point at.
    injection.DetachHandlers();
    injection.bindings.clear();
    panel.Children().Clear();
    PopulateSliderPanel(panel, injection);
    return true;
}

// Pushes freshly read hardware values into the existing sliders. XAML thread.
void ApplyRefreshedValues() try {
    if (!g_engine) {
        return;
    }
    std::vector<brightness::Display> displays = g_engine->GetDisplays();

    std::vector<std::wstring> currentIds;
    currentIds.reserve(displays.size());
    for (const brightness::Display& d : displays) {
        currentIds.push_back(d.stableId);
    }

    std::lock_guard<std::mutex> lock(g_injectionsMutex);

    // Every open refreshes, which makes this the place to retry what could not
    // be borrowed from the shell when the rows were built: its brightness
    // Lottie and its slider style both come off rows that are virtualized, and
    // are often not there yet the first time. Rows built without them keep
    // the fallback glyph and whatever slider style lookup found, so once
    // either turns up the rows are built again.
    if ((!g_capturedSource || !g_capturedVolume || !g_sliderStyle) &&
        g_lookRetries < kMaxLookRetries) {
        ++g_lookRetries;
        bool captured = false;
        for (Injection& injection : *g_injections) {
            if (auto grid = injection.grid.get()) {
                captured = TryCaptureShellLook(grid) || captured;
            }
        }
        if (captured) {
            size_t rebuilt = 0;
            for (Injection& injection : *g_injections) {
                if (RebuildOneInjection(injection)) {
                    ++rebuilt;
                }
            }
            Wh_Log(L"Borrowed from the shell late; rebuilt %zu panel(s)", rebuilt);
            return;
        }
    }

    for (Injection& injection : *g_injections) {
        // A panel whose rows do not match the current displays never got its
        // structural rebuild, and no amount of refreshing will fix that: this
        // function only writes into bindings that already exist, and a panel
        // with none stays empty forever.
        //
        // The usual way to end up here is the early-inject path. It injects
        // before the engine has enumerated, on purpose, and relies on the
        // enumeration's NotifyChanged(true) to fill the panel in -- but that
        // goes through RunOnXamlThread, which can fail for reasons that have
        // nothing to do with us (no top-level window on the XAML thread at
        // that instant, SetWindowsHookEx refused). Nothing else retried it,
        // so the panel stayed empty until the next hotplug.
        //
        // Every open refreshes, so checking here costs one id comparison and
        // recovers on the next open.
        if (injection.builtFor != currentIds) {
            if (RebuildOneInjection(injection)) {
                Wh_Log(L"Panel was built for %zu display(s) and there are now "
                       L"%zu; rebuilt it",
                       injection.builtFor.size(), currentIds.size());
            }
            continue;
        }

        for (Injection::Binding& binding : injection.bindings) {
            const brightness::Display* d = nullptr;
            for (const brightness::Display& candidate : displays) {
                if (candidate.stableId == binding.id) {
                    d = &candidate;
                    break;
                }
            }
            if (!d) {
                continue;
            }

            // Only what differs, so a thumb that is already right is left
            // alone. Scoped suppression: if Value() throws, the outer catch
            // would otherwise swallow it with the flag stuck true, and from
            // then on every drag is silently ignored until the mod is
            // reloaded.
            auto slider = binding.slider.get();
            if (slider && d->percent >= 0 && std::lround(slider.Value()) != d->percent) {
                {
                    SuppressValueChanged guard;
                    slider.Value(d->percent);
                }
                if (auto icon = binding.icon.get()) {
                    SetIconLevel(icon, d->percent);
                }
            }

            auto contrast = binding.contrast.get();
            if (contrast && d->contrast >= 0 &&
                std::lround(contrast.Value()) != d->contrast) {
                {
                    SuppressValueChanged guard;
                    contrast.Value(d->contrast);
                }
                if (auto icon = binding.contrastIcon.get()) {
                    SetIconLevel(icon, d->contrast);
                }
            }

            if (auto power = binding.power.get()) {
                ShowPowerState(power, binding.name, d->poweredOff);
            }

            auto volume = binding.volume.get();
            if (volume && d->volume >= 0 && std::lround(volume.Value()) != d->volume) {
                {
                    SuppressValueChanged guard;
                    volume.Value(d->volume);
                }
                if (auto icon = binding.volumeIcon.get()) {
                    SetVolumeIconLevel(icon, d->volume);
                }
            }
            // An unknown current input leaves the highlight where it is.
            if (d->input >= 0) {
                for (auto& entry : binding.inputs) {
                    if (auto toggle = entry.second.get()) {
                        toggle.IsChecked(entry.first == d->input);
                    }
                }
            }

            // The sliders' own handlers keep it current as they move; this
            // covers a title that was built before its values were read.
            if (binding.title) {
                UpdateRowTitle(*binding.title);
            }
        }

        // The all-displays rows show the mean of what they drive, so they
        // follow the refreshed values too.
        if (injection.links) {
            SyncMasters(*injection.links);
        }
    }
} catch (...) {
    Wh_Log(L"ApplyRefreshedValues threw: %08X", winrt::to_hresult());
}

// A monitor appeared or vanished, so the rows themselves are wrong. XAML
// thread. The panel and its grid row are kept; only the contents are redone.
void RebuildInjectedPanels() try {
    std::lock_guard<std::mutex> lock(g_injectionsMutex);
    size_t rebuilt = 0;
    for (Injection& injection : *g_injections) {
        if (RebuildOneInjection(injection)) {
            ++rebuilt;
        }
    }
    Wh_Log(L"Rebuilt %zu panel(s) after a display change", rebuilt);
} catch (...) {
    Wh_Log(L"RebuildInjectedPanels threw: %08X", winrt::to_hresult());
}

void OnEngineChanged(bool structural) try {
    if (structural && g_engine) {
        for (const brightness::Display& d : g_engine->GetDisplays()) {
            Wh_Log(L"display: %s  transport=%d  now=%d%%  vcpMax=%lu  id=%s",
                   d.name.c_str(), static_cast<int>(d.transport), d.percent,
                   static_cast<unsigned long>(d.vcpMax), d.stableId.c_str());
        }
    }

    // Called on an engine thread; XAML may only be touched on its own. This
    // blocks until the work has run, which is what lets Wh_ModUninit promise
    // that nothing of ours is still scheduled.
    RunOnXamlThread([structural]() {
        if (structural) {
            RebuildInjectedPanels();
        } else {
            ApplyRefreshedValues();
        }
    });
} catch (...) {
    Wh_Log(L"OnEngineChanged threw: %08X", winrt::to_hresult());
}

// Owns the mod's Win32 listening. Two jobs, both needing a message loop:
//   * WM_DISPLAYCHANGE, which is broadcast to top-level windows only, so a
//     message-only window would never see it -- hence a real invisible popup.
//   * a WinEvent hook for windows being shown, which is how we learn the
//     flyout was opened. XamlRoot.Changed looked like the right signal but
//     never fires for this host on 26200, so the Win32 side it is.
class ShellEventWatcher {
   public:
    void Start() {
        if (thread_.joinable()) {
            return;
        }
        ready_ = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        thread_ = std::thread([this] { ThreadMain(); });
        if (ready_) {
            WaitForSingleObject(ready_, 5000);
        }
    }

    void Stop() {
        // Set before anything is posted, so a stop that arrives while the
        // thread is still starting up is seen by the check before its message
        // loop rather than being lost.
        stopRequested_.store(true);

        if (thread_.joinable()) {
            // Both, and unconditionally. Posting only when hwnd_ is already
            // published meant that if Start()'s 5 s wait had timed out, or
            // CreateEvent had failed so Start() never waited at all, nothing
            // was posted and join() blocked forever on a thread sitting in
            // GetMessageW -- taking Wh_ModUninit with it, so the mod could be
            // neither disabled nor updated. A narrow window, but wedging
            // Windhawk is the worst outcome available here.
            if (HWND hwnd = hwnd_.load()) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            if (DWORD threadId = threadId_.load()) {
                PostThreadMessage(threadId, WM_QUIT, 0, 0);
            }
            thread_.join();
        }
        if (ready_) {
            CloseHandle(ready_);
            ready_ = nullptr;
        }
        stopRequested_.store(false);
        threadId_.store(0);
    }

   private:
    static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                      LONG idObject, LONG idChild, DWORD,
                                      DWORD) {
        if (event != EVENT_OBJECT_SHOW || idObject != OBJID_WINDOW ||
            idChild != CHILDID_SELF || !hwnd) {
            return;
        }
        // Only top-level windows: the flyout appearing, not its contents.
        if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
            return;
        }

        // Only the Control Center itself. Without this the slider's own thumb
        // tooltip (Xaml_WindowedPopupClass) trips the hook, spending a DDC read
        // at the start of every drag and risking a refresh landing on the
        // slider just as the user starts moving it.
        wchar_t className[128] = {};
        GetClassNameW(hwnd, className, ARRAYSIZE(className));
        if (!wcsstr(className, L"ControlCenter")) {
            // Capped: if a future build renames the window, these lines say
            // what to match instead.
            static int ignored = 0;
            if (ignored < 5) {
                ++ignored;
                Wh_Log(L"ignoring shown window (%s)", className);
            }
            return;
        }

        // A burst of show events must not become a burst of I2C reads.
        static ULONGLONG lastTick = 0;
        ULONGLONG now = GetTickCount64();
        if (now - lastTick < 500) {
            return;
        }
        lastTick = now;

        Wh_Log(L"Control Center shown (%s); refreshing values", className);
        if (g_engine) {
            g_engine->RequestRefresh();
        }
        // Marshalled, because this runs on the watcher thread and the hide
        // touches XAML. Same hop OnEngineChanged uses.
        RunOnXamlThread(&ReArmStockSliderHide);
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
        switch (msg) {
            case WM_DISPLAYCHANGE:
                Wh_Log(L"WM_DISPLAYCHANGE: rescanning displays");
                if (g_engine) {
                    g_engine->RequestRescan();
                }
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void ThreadMain() {
        // Published first: it is the only way Stop() can reach this thread
        // before the window exists.
        threadId_.store(GetCurrentThreadId());

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = &ShellEventWatcher::WndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = L"WindhawkPerMonitorBrightnessSink";

        ATOM atom = RegisterClassExW(&wc);
        if (atom) {
            hwnd_ = CreateWindowExW(0, MAKEINTATOM(atom), L"", WS_POPUP, 0, 0, 0,
                                    0, nullptr, nullptr, wc.hInstance, nullptr);
            if (!hwnd_) {
                Wh_Log(L"Display sink window failed: %u", GetLastError());
            }
        } else {
            Wh_Log(L"Display sink class failed: %u", GetLastError());
        }

        // Must be installed and removed on the thread that pumps messages.
        hook_ = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr,
                                &ShellEventWatcher::WinEventProc,
                                GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
        if (!hook_) {
            Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        }

        if (ready_) {
            SetEvent(ready_);
        }
        // A Stop() between the window being created and the loop starting
        // would otherwise leave this thread pumping messages with nothing left
        // to signal it.
        if (stopRequested_.load()) {
            Wh_Log(L"Stop requested during startup; not entering the loop");
            if (hook_) {
                UnhookWinEvent(hook_);
                hook_ = nullptr;
            }
            if (HWND hwnd = hwnd_.exchange(nullptr)) {
                DestroyWindow(hwnd);
            }
            if (atom) {
                UnregisterClassW(MAKEINTATOM(atom), wc.hInstance);
            }
            return;
        }
        if (!hwnd_) {
            if (hook_) {
                UnhookWinEvent(hook_);
                hook_ = nullptr;
            }
            if (atom) {
                UnregisterClassW(MAKEINTATOM(atom), wc.hInstance);
            }
            return;
        }

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (hook_) {
            UnhookWinEvent(hook_);
            hook_ = nullptr;
        }
        hwnd_ = nullptr;
        UnregisterClassW(MAKEINTATOM(atom), wc.hInstance);
    }

    std::thread thread_;
    HANDLE ready_ = nullptr;
    std::atomic<HWND> hwnd_{nullptr};
    // So Stop() has something to post to before hwnd_ exists.
    std::atomic<DWORD> threadId_{0};
    std::atomic<bool> stopRequested_{false};
    HWINEVENTHOOK hook_ = nullptr;
};

[[clang::no_destroy]] std::optional<ShellEventWatcher> g_shellWatcher;

bool TryInject(wux::DependencyObject const& controlCenterView) {
    bool expected = false;
    if (!g_injecting.compare_exchange_strong(expected, true)) {
        return false;
    }
    struct Guard {
        ~Guard() { g_injecting.store(false); }
    } guard;

    try {
        auto l1 = FindDescendant(controlCenterView,
                                 L"Windows.UI.Xaml.Controls.Grid#L1Grid", 8);
        if (!l1) {
            if (!g_dumpedTree.exchange(true)) {
                Wh_Log(L"L1Grid not found under ControlCenterView. "
                       L"Tree follows:");
                DumpTree(controlCenterView, 0, 6);
            }
            return false;
        }
        return InjectInto(l1.as<wux::FrameworkElement>());
    } catch (...) {
        Wh_Log(L"Injection failed: %08X", winrt::to_hresult());
        return false;
    }
}

// By the time the view is reported its tree is normally already complete, so
// inject straight away. Waiting on a layout pass instead meant waiting until
// the panel was next *closed*: a view the shell is keeping rather than
// rebuilding runs no layout while it sits open. The comment on g_injecting
// has the lifetime in full.
// Loaded/LayoutUpdated remain as retries for the case where it really is not
// ready yet; both are revoked as soon as one succeeds.
void AttachInjector(wux::FrameworkElement const& view) {
    // We are on the XAML thread, so record it here rather than only on a
    // successful injection. The retry handlers below are registered on the
    // shell's own element whether or not the tree was ready, and Wh_ModUninit
    // keys "is there anything to remove?" off this id -- so leaving it unset
    // in exactly the case the retries exist for would return from unload with
    // live revokers pointing into an image Windhawk is about to unmap.
    g_xamlThreadId.store(GetCurrentThreadId());

    if (TryInject(view)) {
        return;
    }

    Wh_Log(L"Tree not ready, arming retries");

    g_injectRetry.Revoke();  // replaces any earlier arming
    auto retry = [view]() {
        if (TryInject(view) || ++g_injectRetry.attempts >= kMaxRetryAttempts) {
            g_injectRetry.Revoke();
        }
    };

    g_injectRetry.loaded = view.Loaded(
        winrt::auto_revoke,
        [retry](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            retry();
        });
    g_injectRetry.layout = view.LayoutUpdated(
        winrt::auto_revoke,
        [retry](wf::IInspectable const&, wf::IInspectable const&) { retry(); });
}

}  // namespace

// ===========================================================================
// Finding the Control Center without XAML diagnostics
// ===========================================================================
//
// This used to open an XAML diagnostics connection and watch the visual tree
// for ControlCenterView being added. It worked, but only one diagnostics
// consumer can exist per process -- the Windows 11 Taskbar Styler says so in
// its own README -- so taking that connection stopped the Windows 11
// Notification Center Styler theming the Control Center, which is what a user
// reported. Nothing this mod did was at fault beyond holding the slot.
//
// So it does what explorer-command-bar does instead, and for the reason that
// mod gives: hook a function in the host's own DLL, take the element from it,
// and walk the tree with the public VisualTreeHelper API. No diagnostics, no
// slot to contend for, and the conflict is gone by construction rather than by
// winning a fight over a single-consumer resource.
//
// The hook is on ControlCenter.dll's
//
//   winrt::impl::produce<ControlCenterView, IControlOverrides>::OnGotFocus
//
// Three things had to be true and each took a measurement to establish:
//
//   * The pointer must really be a COM interface on the view. A produce<>
//     override's `this` is one. An implementation member's `this` is not, and
//     calling QueryInterface through it faulted ShellHost outright.
//   * It must run after the view is fully constructed. IComponentConnector::
//     Connect hands over the same element but runs inside InitializeComponent,
//     and merely taking a reference there destroyed the half-built view.
//   * A third constraint was recorded -- that it must not run inside a layout
//     pass, because walking the tree from LayoutUpdated ended in a fastfail --
//     and it is doubtful. That was measured on the diagnostics build, whose
//     connection loop produces the identical 0xc0000409 for an entirely
//     unrelated reason (DEVELOPING.md spells that one out), so the
//     attribution may well have been wrong. The retry paths above do walk the
//     tree from LayoutUpdated and have not reproduced it.
//
// OnGotFocus satisfies both of the confirmed ones: the flyout has been built
// and is taking focus. OnApplyTemplate would have been the obvious choice and is never
// called at all -- ControlCenterView is compiled XAML with an
// InitializeComponent, so no ControlTemplate is ever applied to it.

namespace {

std::atomic<bool> g_discoveryHooked{false};

using ControlCenterView_OnGotFocus_t = int(WINAPI*)(void* pThis, void* args);
ControlCenterView_OnGotFocus_t ControlCenterView_OnGotFocus_Original;

// Defined below, next to the tree helpers it belongs with.
wux::FrameworkElement FindContainingView(wux::DependencyObject const& from);

// Injecting before the first paint, via IComponentConnector::Connect.
//
// PlayIntroAnimation was the previous attempt and it never fires -- it is
// hooked optional, so it failed silently and everything kept going through
// OnGotFocus, which is after the flyout has been drawn. That is the flicker.
//
// Connect does fire, once per x:Name, during InitializeComponent. Two things
// have to be right about how it is used:
//
//   * Not `this`. That is the view, half-constructed; taking a reference to
//     it there destroyed it and took ShellHost down with a call through freed
//     memory. Only the `target` argument is touched here, which is a child
//     element the host has already finished building.
//   * Not immediately. The tree is still being assembled, so walking it now
//     would find an incomplete one. The work is posted to the dispatcher and
//     runs once construction has unwound -- still well before the flyout is
//     shown.
using Connect_t = int(WINAPI*)(void* pThis, int connectionId, void* target);
Connect_t ControlCenterView_Connect_Original;


int WINAPI ControlCenterView_Connect_Hook(void* pThis, int connectionId,
                                          void* target) {
    int ret = ControlCenterView_Connect_Original(pThis, connectionId, target);

    if (!target || g_earlyInject) {
        return ret;  // once per view is enough
    }
    try {
        wux::FrameworkElement child{nullptr};
        static_cast<::IUnknown*>(target)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(child));
        if (!child) {
            return ret;
        }

        // The XAML thread is recorded here, not only in AttachInjector.
        //
        // Otherwise the unload path cannot reach this timer: it decides
        // whether anything needs undoing from g_xamlThreadId, and between
        // Connect and the first tick nothing has been injected yet, so it
        // would conclude there was nothing to do and return -- leaving the
        // tick to fire into an unmapped image.
        g_xamlThreadId.store(GetCurrentThreadId());

        auto timer = wux::DispatcherTimer();
        timer.Interval(std::chrono::milliseconds(1));
        timer.Tick([weak = winrt::make_weak(child)](
                       wf::IInspectable const& sender,
                       wf::IInspectable const&) {
            // Stopped through the sender, not a captured copy. Capturing the
            // timer makes a cycle -- the timer owns the handler, the handler
            // owns the timer -- so clearing g_earlyInject would never release
            // it and every view construction would leak a timer and a
            // delegate whose code lives in this DLL.
            if (auto self = sender.try_as<wux::DispatcherTimer>()) {
                self.Stop();
            }
            g_earlyInject = nullptr;
            try {
                auto element = weak.get();
                if (!element) {
                    return;
                }
                auto view = FindContainingView(element);
                if (!view) {
                    return;
                }
                StartEngineAsync();
                // Injected whether or not the engine has enumerated yet.
                //
                // Bailing on an empty display list made this path useless on
                // the first open after sign-in -- the enumeration takes a WMI
                // connect plus a DDC round trip per monitor, so it is rarely
                // finished this early -- and that open fell through to
                // OnGotFocus, after the first paint, which is the flicker
                // this exists to remove. An empty panel is filled in by the
                // structural rebuild, exactly as the OnGotFocus path already
                // relies on.
                AttachInjector(view);
            } catch (...) {
            }
        });
        timer.Start();
        g_earlyInject = timer;
    } catch (...) {
    }
    return ret;
}

// Walks up from any element to the ControlCenterView that contains it.
//
// Used by the Connect hook, which is handed a child rather than the view --
// deliberately, because the view is half-built at that point and touching it
// crashed the host.
wux::FrameworkElement FindContainingView(wux::DependencyObject const& from) {
    wux::DependencyObject node = from;
    for (int up = 0; up < 12 && node; ++up) {
        try {
            if (std::wstring_view{winrt::get_class_name(node)} ==
                L"ControlCenter.ControlCenterView") {
                return node.try_as<wux::FrameworkElement>();
            }
        } catch (...) {
        }
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    return nullptr;
}

int WINAPI ControlCenterView_OnGotFocus_Hook(void* pThis, void* args) {
    int ret = ControlCenterView_OnGotFocus_Original(pThis, args);

    try {
        // QueryInterface, not copy_from_abi.
        //
        // copy_from_abi does not QI -- it AddRefs and stores the pointer as
        // it is. `pThis` is the produce<ControlCenterView, IControlOverrides>
        // subobject, so the result would be an IControlOverrides* being
        // treated as an IFrameworkElement*. The happy path hides that, since
        // passing it as a DependencyObject does a real conversion, but the
        // retry path calls view.Loaded() and view.LayoutUpdated() straight
        // through the assumed vtable -- slots far past the end of
        // IControlOverrides. That is a wild call, not an exception, so the
        // catch below would not have caught it, and it would have fired on
        // exactly the builds the retries exist for.
        wux::FrameworkElement view{nullptr};
        static_cast<::IUnknown*>(pThis)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(view));
        if (!view) {
            return ret;
        }

        // A backstop for starting the engine, not the only place it starts:
        // the Connect tick and the discovery hook both get there first when
        // they fire, and StartEngineAsync is idempotent. What matters is that
        // every one of those gates is "a Control Center exists in this
        // process". The engine used to start as soon as any XAML window did,
        // which on a build that hosts the Control Center elsewhere meant a WMI
        // connection, a DDC/CI probe of every monitor and two threads running
        // permanently for a UI that would never appear.
        StartEngineAsync();

        // Injected without waiting for the enumeration: an empty panel now is
        // fine, because the first enumeration ends in NotifyChanged(structural)
        // and RebuildInjectedPanels fills it in -- the same path hotplug
        // already uses.
        //
        // Cheap when there is nothing to do: InjectInto returns early if the
        // panel is already in the tree, which matters because focus can be
        // taken more than once per opening.
        AttachInjector(view);
    } catch (...) {
        Wh_Log(L"OnGotFocus hook error: %08X", winrt::to_hresult());
    }

    return ret;
}

// Whether this process has got as far as owning a window.
//
// Used to tell "the mod was just enabled into a running shell" from "the shell
// is starting with the mod already on", which the module being mapped cannot
// distinguish on its own. A ShellHost that has not reached its entry point has
// no top-level window; one that is showing a taskbar has several.
bool ProcessHasTopLevelWindow() {
    struct Search {
        DWORD pid;
        bool found;
    } search{GetCurrentProcessId(), false};

    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            auto* s = reinterpret_cast<Search*>(param);
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == s->pid) {
                s->found = true;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));
    return search.found;
}

void InstallDiscoveryHooks(HMODULE controlCenter, bool applyNow,
                           bool startEngine) {
    if (g_discoveryHooked.exchange(true)) {
        return;
    }

    WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
            &ControlCenterView_OnGotFocus_Original,
            ControlCenterView_OnGotFocus_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
            &ControlCenterView_Connect_Original,
            ControlCenterView_Connect_Hook,
            // Optional: without it the panel still appears, on the OnGotFocus
            // path a frame later. That is the visible flicker when Quick
            // Settings has been closed for a while, not a loss of function.
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(controlCenter, controlCenterDllHooks,
                                    ARRAYSIZE(controlCenterDllHooks))) {
        Wh_Log(L"Could not hook ControlCenterView::OnGotFocus; the panel will "
               L"not be injected");
        return;
    }

    // Windhawk applies what Wh_ModInit registers, once, when it returns.
    // A hook registered later -- from inside the load hook -- has to be
    // applied by hand or it never fires.
    if (applyNow && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Hook registered but could not be armed");
        return;
    }

    Wh_Log(L"ControlCenterView::OnGotFocus hooked");

    // Start the engine now, not when the Control Center is first opened.
    //
    // This is what the flicker was. Injecting early is no use if there is
    // nothing to inject: the panel is built from the engine's display list,
    // and the first enumeration blocks on WMI and an I2C round trip per
    // monitor. Opening Quick Settings cold meant the intro-animation hook
    // found no displays, gave up, and the panel arrived on the later
    // OnGotFocus path -- after the stock layout had already been drawn. Open
    // it twice in a row and the engine was warm, so it looked fine, which is
    // exactly the pattern that was reported.
    //
    // Doing it here still honours why the engine was moved out of
    // Wh_ModAfterInit in the first place. The objection was to starting on
    // "any XAML window exists", which is true in processes that never host a
    // Control Center. ControlCenter.dll being loaded is a far tighter gate:
    // the module is present precisely where the panel can appear, and it is
    // mapped long before the flyout is first opened.
    //
    // startEngine is false for the one case where "mapped" does not imply "the
    // host is up": see StartControlCenterWatch.
    if (startEngine) {
        StartEngineAsync();
    }
}

// Enabling the mod into a running shell usually finds ControlCenter.dll
// already mapped, and StartControlCenterWatch takes that path. A shell that is
// starting does not have it yet -- it is pulled in by WinRT activation the
// first time Quick Settings is opened -- so it has to be caught at the moment
// it loads, before any of its code runs.
//
// The hook goes on kernelbase's LoadLibraryExW, not kernel32's.
//
// That distinction is the whole reason an earlier version needed a polling
// thread: kernel32's export is a forwarder, and the WinRT activation path
// that maps this DLL calls kernelbase directly, so a hook on kernel32 never
// saw the load at all. Polling papered over it, and brought a thread that
// Wh_ModUninit had no way to join -- if the mod were disabled mid-sleep,
// Windhawk would unmap the image and the thread's next instruction would be
// in freed memory. Hooking the right function removes the need for both.

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// LoadLibraryEx takes a module name as readily as a path, appends .dll itself,
// and accepts forward slashes -- so the name has to be matched all three ways
// or the load is missed for a caller that spells it differently. Same handling
// as explorer-command-bar's.
bool IsControlCenterDll(LPCWSTR path) {
    const wchar_t* name = path;
    for (const wchar_t* p = path; *p; ++p) {
        if (*p == L'\\' || *p == L'/') {
            name = p + 1;
        }
    }
    return _wcsicmp(name, L"ControlCenter.dll") == 0 ||
           _wcsicmp(name, L"ControlCenter") == 0;
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR path, HANDLE file, DWORD flags) {
    HMODULE result = LoadLibraryExW_Original(path, file, flags);
    if (result && path && !g_discoveryHooked.load()) {
        if (IsControlCenterDll(path)) {
            // Resolved through the loader rather than trusting what came
            // back.
            //
            // LOAD_LIBRARY_AS_DATAFILE and friends -- which resource lookups
            // use -- return a mapping that is not executable code, with its
            // low bits set. Hooking that would patch a resource view, set the
            // guard, and leave the real load ignored and the mod silently
            // dead for the session. A data-file mapping is never in the
            // loader's module list, so asking for it by name cannot return
            // one.
            HMODULE module = GetModuleHandleW(L"ControlCenter.dll");
            if (!module) {
                return result;
            }
            // Applied immediately: this runs inside the load, before anything
            // in the DLL has executed, so there is no window in which the
            // host could build the view unhooked.
            //
            // The engine starts here: a lazy load of this DLL is the host
            // activating the Control Center, which is long past its own COM
            // startup, and warming the engine now is what keeps the first open
            // from arriving unpopulated.
            InstallDiscoveryHooks(module, /*applyNow=*/true,
                                  /*startEngine=*/true);
        }
    }
    return result;
}

void StartControlCenterWatch() {
    if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll")) {
        // Already mapped means one of two quite different things, and only a
        // window tells them apart.
        //
        // Today it always means the mod was enabled into a running shell --
        // ShellHost does not statically import this DLL, so at Wh_ModInit on a
        // fresh start it is not there yet and the load hook below is what
        // catches it. If a future build does import it statically, the loader
        // maps it before the entry point, this branch would be taken on every
        // cold start, and the engine's first CoCreateInstance would land ahead
        // of the host's CoInitializeSecurity -- the fast-fail restart loop the
        // comment above g_engineStarter describes. A process that has not run
        // its entry point owns no window, so gate on that and leave the start
        // to the Connect/OnGotFocus hooks, which cannot fire too early.
        //
        // Windhawk arms what Wh_ModInit registers, so nothing to apply here.
        const bool hostIsUp = ProcessHasTopLevelWindow();
        Wh_Log(L"ControlCenter.dll already mapped; host %ls",
               hostIsUp ? L"is up, warming the engine"
                        : L"is still starting, leaving the engine to the "
                          L"view hooks");
        InstallDiscoveryHooks(module, /*applyNow=*/false,
                              /*startEngine=*/hostIsUp);
        return;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto target = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                                   GetProcAddress(kernelBase, "LoadLibraryExW"))
                             : nullptr;
    if (!target) {
        Wh_Log(L"No kernelbase!LoadLibraryExW; the panel will not be injected");
        return;
    }
    WindhawkUtils::SetFunctionHook(target, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);
}

// Between the GetModuleHandleW above and Windhawk arming that hook -- which it
// does only once Wh_ModInit returns -- a load of ControlCenter.dll passes
// unseen, and being a one-shot the mod would then do nothing for the rest of
// the session. The window is small, but closing it is one call, which is what
// explorer-command-bar does with its own extension hooks.
void RecheckControlCenterModule() {
    if (g_discoveryHooked.load()) {
        return;
    }
    if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll")) {
        Wh_Log(L"ControlCenter.dll arrived while hooks were being armed");
        // Hooks are live by now, so this registration has to be applied by
        // hand. The engine is gated the same way as in StartControlCenterWatch
        // and for the same reason -- this still runs during injection, which
        // on a starting shell is before the host has done its COM setup.
        InstallDiscoveryHooks(module, /*applyNow=*/true,
                              /*startEngine=*/ProcessHasTopLevelWindow());
    }
}

}  // namespace

// ===========================================================================
// Windhawk lifecycle
// ===========================================================================

// Everything the settings say, read in one go so a change can be compared
// against what the mod is running with before anything live is touched.
struct ModSettings {
    bool hideStockBrightness = true;
    bool hideUnsupported = false;
    PanelPosition panelPosition = PanelPosition::BelowSliders;
    brightness::FollowMode followMode = brightness::FollowMode::Off;
    LayoutStyle layoutStyle = LayoutStyle::Dropdown;
    bool showPerMonitor = true;
    bool showMasterBrightness = true;
    bool showContrast = true;
    bool showMasterContrast = true;
    bool showPowerButton = true;
    bool showVolume = true;
    bool showInputSwitcher = true;
    int scrollStep = 5;
    bool iconClicks = false;
    int iconClickLevels[3] = {0, 50, 100};
    std::vector<DisplayRule> displayRules;

    // Everything but the follow mode changes what is in the shell's visual
    // tree, which is put right by reloading rather than by patching it live.
    bool SameUi(const ModSettings& other) const {
        return hideStockBrightness == other.hideStockBrightness &&
               hideUnsupported == other.hideUnsupported &&
               panelPosition == other.panelPosition &&
               layoutStyle == other.layoutStyle &&
               showPerMonitor == other.showPerMonitor &&
               showMasterBrightness == other.showMasterBrightness &&
               showContrast == other.showContrast &&
               showMasterContrast == other.showMasterContrast &&
               showPowerButton == other.showPowerButton &&
               showVolume == other.showVolume &&
               showInputSwitcher == other.showInputSwitcher &&
               scrollStep == other.scrollStep && iconClicks == other.iconClicks &&
               std::equal(std::begin(iconClickLevels), std::end(iconClickLevels),
                          std::begin(other.iconClickLevels)) &&
               displayRules == other.displayRules;
    }
};

// What the mod is running with. Heap-only members, so no no_destroy needed.
ModSettings g_settings;

ModSettings ReadSettings() {
    ModSettings out;
    out.hideStockBrightness = Wh_GetIntSetting(L"hideStockBrightness") != 0;
    out.hideUnsupported = Wh_GetIntSetting(L"hideUnsupported") != 0;

    // No truthiness tests on string settings: StringSetting converts through
    // operator PCWSTR(), and Wh_GetStringSetting returns L"" rather than NULL,
    // so such a check is always taken.
    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"panelPosition");
    if (wcscmp(position.get(), L"aboveSliders") == 0) {
        out.panelPosition = PanelPosition::AboveSliders;
    } else if (wcscmp(position.get(), L"bottom") == 0) {
        out.panelPosition = PanelPosition::Bottom;
    }

    // Off unless asked for: following writes to monitors that keep the value
    // in their own settings, and Windows cannot distinguish a brightness key
    // from power-plan or idle dimming.
    WindhawkUtils::StringSetting follow =
        WindhawkUtils::StringSetting::make(L"followInternalBrightness");
    if (wcscmp(follow.get(), L"relative") == 0) {
        out.followMode = brightness::FollowMode::Relative;
    } else if (wcscmp(follow.get(), L"match") == 0) {
        out.followMode = brightness::FollowMode::Match;
    }

    WindhawkUtils::StringSetting layout = WindhawkUtils::StringSetting::make(L"layoutStyle");
    if (wcscmp(layout.get(), L"expanded") == 0) {
        out.layoutStyle = LayoutStyle::Expanded;
    }
    out.showPerMonitor = Wh_GetIntSetting(L"showPerMonitor") != 0;
    out.showMasterBrightness = Wh_GetIntSetting(L"showMasterBrightness") != 0;
    out.showContrast = Wh_GetIntSetting(L"showContrast") != 0;
    out.showMasterContrast = Wh_GetIntSetting(L"showMasterContrast") != 0;
    out.showPowerButton = Wh_GetIntSetting(L"showPowerButton") != 0;
    out.showVolume = Wh_GetIntSetting(L"showVolume") != 0;
    out.showInputSwitcher = Wh_GetIntSetting(L"showInputSwitcher") != 0;
    out.scrollStep = std::clamp(Wh_GetIntSetting(L"scrollStep"), 0, 100);
    out.iconClicks = Wh_GetIntSetting(L"iconClicks") != 0;
    out.iconClickLevels[0] = std::clamp(Wh_GetIntSetting(L"iconClickLeft"), 0, 100);
    out.iconClickLevels[1] = std::clamp(Wh_GetIntSetting(L"iconClickMiddle"), 0, 100);
    out.iconClickLevels[2] = std::clamp(Wh_GetIntSetting(L"iconClickRight"), 0, 100);

    // A fixed bound rather than stopping at the first empty entry, so an
    // entry left blank in the middle of the list does not hide the ones after.
    for (int i = 0; i < 32; ++i) {
        WindhawkUtils::StringSetting match =
            WindhawkUtils::StringSetting::make(L"displaySettings[%d].match", i);
        DisplayRule rule;
        rule.match = brightness::detail::ToLower(match.get());
        if (rule.match.empty()) {
            continue;  // an entry that matches nothing does nothing
        }
        WindhawkUtils::StringSetting name =
            WindhawkUtils::StringSetting::make(L"displaySettings[%d].name", i);
        rule.name = name.get();
        rule.hide = Wh_GetIntSetting(L"displaySettings[%d].hide", i) != 0;
        rule.hideContrast = Wh_GetIntSetting(L"displaySettings[%d].hideContrast", i) != 0;
        rule.hideVolume = Wh_GetIntSetting(L"displaySettings[%d].hideVolume", i) != 0;
        rule.hideInput = Wh_GetIntSetting(L"displaySettings[%d].hideInput", i) != 0;
        rule.hidePower = Wh_GetIntSetting(L"displaySettings[%d].hidePower", i) != 0;
        out.displayRules.push_back(std::move(rule));
    }
    return out;
}

void ApplySettings(const ModSettings& settings) {
    g_hideStockBrightness = settings.hideStockBrightness;
    g_hideUnsupported = settings.hideUnsupported;
    g_panelPosition = settings.panelPosition;
    g_layoutStyle = settings.layoutStyle;
    g_showPerMonitor = settings.showPerMonitor;
    g_showMasterBrightness = settings.showMasterBrightness;
    g_showContrast = settings.showContrast;
    g_showMasterContrast = settings.showMasterContrast;
    g_showPowerButton = settings.showPowerButton;
    g_showVolume = settings.showVolume;
    g_showInputSwitcher = settings.showInputSwitcher;
    g_scrollStep = settings.scrollStep;
    g_iconClicks = settings.iconClicks;
    std::copy(std::begin(settings.iconClickLevels), std::end(settings.iconClickLevels),
              std::begin(g_iconClickLevels));
    g_displayRules = settings.displayRules;
    if (g_engine) {
        g_engine->SetFollowMode(settings.followMode);
    }

    Wh_Log(L"settings: hideStock=%d follow=%d position=%d hideUnsupported=%d "
           L"layout=%d perMonitor=%d masterBrightness=%d contrast=%d "
           L"masterContrast=%d power=%d volume=%d inputs=%d wheel=%d iconClicks=%d "
           L"displayRules=%zu",
           g_hideStockBrightness ? 1 : 0, static_cast<int>(settings.followMode),
           static_cast<int>(g_panelPosition), g_hideUnsupported ? 1 : 0,
           static_cast<int>(g_layoutStyle), g_showPerMonitor ? 1 : 0,
           g_showMasterBrightness ? 1 : 0, g_showContrast ? 1 : 0,
           g_showMasterContrast ? 1 : 0, g_showPowerButton ? 1 : 0, g_showVolume ? 1 : 0,
           g_showInputSwitcher ? 1 : 0, g_scrollStep, g_iconClicks ? 1 : 0,
           g_displayRules.size());
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_engine = new brightness::Engine();
    g_engine->SetLogger(&EngineLog);
    g_engine->SetOnChanged(&OnEngineChanged);

    g_settings = ReadSettings();
    ApplySettings(g_settings);

    // In Wh_ModInit so that Windhawk arms the hook itself when it returns, and
    // so it is in place before the host can build the view.
    StartControlCenterWatch();

    // The engine is deliberately NOT started here; see StartEngineIfNeeded.
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The engine still is not started here. It waits until a Control Center
    // is actually seen, so a process that never hosts one never pays for it.
    // This only closes the gap in which the module could have loaded while
    // Windhawk was arming the hook.
    RecheckControlCenterModule();

    g_shellWatcher.emplace();
    g_shellWatcher->Start();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    // Read into a copy and compared before anything live is touched: the XAML
    // thread reads these while building rows, and a reload is the clean way
    // to change what is in somebody else's visual tree anyway -- hiding the
    // stock slider touches an element we do not own, the position moves the
    // shell's own children between rows, and the rest change which rows
    // exist at all.
    ModSettings next = ReadSettings();
    if (!next.SameUi(g_settings)) {
        *bReload = TRUE;
        return TRUE;
    }

    // Follow mode is engine state and takes effect at once.
    g_settings.followMode = next.followMode;
    if (g_engine) {
        g_engine->SetFollowMode(next.followMode);
    }
    *bReload = FALSE;
    return TRUE;
}

// Removing the injected UI is not optional. RunOnXamlThread can fail for
// reasons that have nothing to do with whether we injected -- no window found
// yet, SetWindowsHookEx refused -- and returning anyway would leave the
// sliders' ValueChanged handlers and the retry subscriptions registered on the
// shell's own elements, pointing into an image Windhawk is about to unmap.
// That is a crash on the shell's next layout pass.
bool RemoveInjectionsWithRetry() {
    for (int attempt = 0; attempt < 25; attempt++) {
        if (RunOnXamlThread(&RemoveInjections)) {
            return true;
        }
        Wh_Log(L"XAML thread unreachable (attempt %d); retrying before unload",
               attempt + 1);
        Sleep(200);
    }
    return false;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Order matters. Everything that could still call into this DLL has to be
    // stopped before the UI is dismantled, and all of it has to be finished
    // before we return -- Windhawk frees the module the moment we do.

    // 1. The watcher first: it owns the timer that can still start the engine.
    //
    //    Nothing has to be un-advised any more. The injection hook is a
    //    function hook, and Windhawk removes those itself as part of unloading
    //    the mod -- unlike a diagnostics connection, which had to be given
    //    back by hand and would otherwise have outlived the DLL.
    if (g_shellWatcher) {
        g_shellWatcher->Stop();
        // Explicit, rather than relying on the suppressed destructor: Stop()
        // is what joins the thread and closes the event, so this is the point
        // at which the watcher is provably finished with.
        g_shellWatcher.reset();
    }

    // 2. The starter thread may be inside Engine::Start() right now, so it has
    //    to be finished before anything below touches the engine.
    {
        std::lock_guard<std::mutex> lock(g_engineStarterMutex);
        if (g_engineStarter && g_engineStarter->joinable()) {
            g_engineStarter->join();
        }
        // reset() is the release: there is no assignment to a std::thread
        // that means "done with this", and move-assigning over a joinable
        // one terminates.
        g_engineStarter.reset();
    }

    // 3. No more engine callbacks, and join both engine threads so none can be
    //    in flight. After this nothing can ask to run on the XAML thread.
    if (g_engine) {
        g_engine->SetOnChanged(nullptr);
        g_engine->Stop();
    }

    // 4. Put the visual tree back, synchronously, on the thread that owns it.
    bool treeRestored = true;
    if (g_xamlThreadId.load() == 0) {
        Wh_Log(L"Nothing was injected; nothing to remove");
    } else if (!RemoveInjectionsWithRetry()) {
        Wh_Log(L"Could not reach the XAML thread after 5s; injected UI may "
               L"still be live");
        treeRestored = false;
    }

    // 5. Only now is it safe to drop the engine itself.
    if (g_engine) {
        delete g_engine;
        g_engine = nullptr;
    }

    g_xamlThreadId.store(0);

    // Last: release the injection bookkeeping while COM is still usable. On
    // the success path RemoveInjections has already swapped the vector empty,
    // so this just drops the container.
    //
    // If the tree could not be restored it is NOT empty, and resetting would
    // release live XAML references from this thread rather than the one that
    // owns them. Leaking the container is the lesser evil, and is what the
    // [[clang::no_destroy]] is there to make survivable.
    if (treeRestored) {
        g_injections.reset();
    } else {
        Wh_Log(L"Leaving injection bookkeeping alive; releasing it off the "
               L"XAML thread would be worse");
    }
}
