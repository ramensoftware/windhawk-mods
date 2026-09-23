// ==WindhawkMod==
// @id              per-monitor-brightness
// @name            Per-monitor brightness in Quick Settings
// @description     Adds a titled brightness slider for every connected monitor to the Windows 11 Quick Settings panel
// @version         2.1
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
# Per-monitor brightness in Quick Settings

Windows gives you exactly one brightness slider no matter how many monitors you
have, and on a desktop it gives you none at all. This mod adds a labelled slider
for every connected display, right in the Quick Settings panel, each showing its
current level and carrying the shell's own animated brightness icon.

![Per-monitor brightness sliders in Quick Settings](https://raw.githubusercontent.com/bardelyne/per-monitor-brightness/main/screenshot.png)

## How each display is driven

- **External monitors** use **DDC/CI** (VCP code `0x10`), the I2C side-channel
  in the video cable that the monitor's own on-screen menu uses. Most monitors
  made in the last decade support it; some budget panels and some USB-C docks
  do not.
- **Laptop internal panels** use **WMI**, the same path the stock slider takes.

A display that answers neither is listed as uncontrollable rather than being
silently dropped.

## Features

- One titled slider per display, showing the live percentage.
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

#pragma pop_macro("GetCurrentTime")

#include <roapi.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
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
            bool doRescan = false;
            bool doRefresh = false;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                work_.wait(lock, [this] {
                    return quit_ || rescan_ || refresh_ || !pending_.empty();
                });
                if (quit_) {
                    break;
                }
                doRescan = std::exchange(rescan_, false);
                batch.swap(pending_);
                // Never read back while writes are still queued: the value in
                // flight has not reached the panel yet, so reading now would
                // yank the slider backwards under the user's finger. refresh_
                // stays set and we come back to it once the queue drains.
                doRefresh = batch.empty() && refresh_;
                if (doRefresh) {
                    refresh_ = false;
                }
            }

            if (doRescan) {
                Rescan();
                NotifyChanged(true);
            }

            // Each write blocks for tens of ms. Anything the UI posts while
            // we are on the wire lands in pending_ and overwrites its
            // predecessor, so we always resume with the newest value.
            bool wroteDdc = false;
            for (const auto& entry : batch) {
                if (Apply(entry.first, entry.second) == Transport::DdcCi) {
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
        };

        std::vector<Target> targets;
        bool needWmi = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& d : displays_) {
                targets.push_back({d.stableId, d.transport, d.hPhysical,
                                   detail::ToLower(d.stableId)});
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
                attached = true;
                continue;  // keep this handle alive
            }
            DestroyPhysicalMonitors(1, &physical[i]);
        }

        return attached;
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

    // Per-display controls, so a refresh can update values in place instead of
    // rebuilding the whole panel.
    struct Binding {
        std::wstring id;
        std::wstring name;
        winrt::weak_ref<wuxc::Slider> slider;
        winrt::weak_ref<wux::FrameworkElement> icon;
        winrt::weak_ref<wuxc::TextBlock> title;
    };
    std::vector<Binding> bindings;

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

// The shell's own brightness Lottie, borrowed off its AnimatedIcon so our
// sliders can show the real animated sun rather than an imitation. WinUI2
// exposes no brightness visual source publicly, so lifting the live one is the
// only way to get it.
[[clang::no_destroy]] winrt::com_ptr<::IInspectable> g_brightnessSource;
[[clang::no_destroy]] winrt::com_ptr<IAnimatedIconStaticsAbi> g_animatedIconStatics;
bool g_capturedSource = false;

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

// Either the shell's real AnimatedIcon, or the Segoe Fluent brightness glyph
// when the Lottie could not be borrowed or cannot be driven by level.
// Both icon variants are built to this, and the row arithmetic uses the
// constant rather than reading Width() back off the element.
//
// The fallback glyph had no Width at all, so Width() returned NaN -- XAML's
// spelling of Auto -- and the icon-to-track gap computed from it was NaN,
// which then went into a Thickness. That is not a rare path: it is what every
// desktop gets, since with no internal panel there is no brightness Lottie to
// borrow.
inline constexpr double kIconSize = 20;

wux::FrameworkElement MakeBrightnessIcon() {
    if (g_brightnessSource) {
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
                SUCCEEDED(iconAbi->put_Source(g_brightnessSource.get()))) {
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
    font.Glyph(L"");  // Brightness
    font.Width(kIconSize);
    font.Height(kIconSize);
    return font;
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

winrt::hstring FormatRowTitle(const std::wstring& name, int percent) {
    if (percent < 0) {
        return winrt::hstring{name};
    }
    return winrt::hstring{name + L"  ·  " + std::to_wstring(percent) + L"%"};
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
    // displays are known -- and permanently when hideUnsupported hides every
    // one of them.
    panel.Visibility(wux::Visibility::Collapsed);

    for (const brightness::Display& d : displays) {
        const bool controllable = d.transport != brightness::Transport::None;
        if (!controllable && g_hideUnsupported) {
            continue;
        }

        std::wstring displayName = d.name;
        // The 50 is a placeholder for a value not read *yet*, and only a
        // controllable display has one coming -- the first refresh replaces
        // it. An uncontrollable one has no value and never will, so showing
        // it "50%" directly above "Brightness control not supported" invents a
        // reading that does not exist. FormatRowTitle omits the percentage
        // when given a negative, which is exactly this case.
        const int shownPercent =
            controllable ? (d.percent < 0 ? 50 : d.percent) : -1;
        wuxc::TextBlock title;
        title.Text(FormatRowTitle(displayName, shownPercent));
        title.FontSize(12);
        title.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
        title.Opacity(0.85);
        panel.Children().Append(title);

        if (!controllable) {
            // Say so rather than showing a slider that does nothing.
            wuxc::TextBlock note;
            note.Text(L"Brightness control not supported");
            note.FontSize(11);
            note.Opacity(0.55);
            panel.Children().Append(note);
            continue;
        }

        // Icon column + stretching slider column, so each row reads like the
        // shell's own slider rows.
        wuxc::Grid sliderRow;
        wuxc::ColumnDefinition iconColumn;
        iconColumn.Width(
            wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        wuxc::ColumnDefinition sliderColumn;
        sliderColumn.Width(
            wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        sliderRow.ColumnDefinitions().Append(iconColumn);
        sliderRow.ColumnDefinitions().Append(sliderColumn);

        wux::FrameworkElement icon = MakeBrightnessIcon();
        icon.VerticalAlignment(wux::VerticalAlignment::Center);
        // The gap the shell leaves between its icon and its track, derived
        // rather than guessed: the panel already starts at the icon's left
        // edge, so what is left over after the icon is the gap.
        const double iconGap =
            UseNativeMetrics()
                ? g_metrics.sliderLeft - g_metrics.iconLeft - kIconSize
                : kOwnIconGap;
        icon.Margin(wux::ThicknessHelper::FromLengths(0, 0, iconGap, 0));
        wuxc::Grid::SetColumn(icon, 0);
        sliderRow.Children().Append(icon);

        wuxc::Slider slider;
        slider.Minimum(0);
        slider.Maximum(100);
        slider.Value(shownPercent);
        slider.IsThumbToolTipEnabled(true);
        slider.VerticalAlignment(wux::VerticalAlignment::Center);
        if (UseNativeMetrics()) {
            // Matched to the shell's row so the track sits at the same height
            // and the hit target is the same size. The default template is
            // shorter, which reads as a thinner control next to the real ones.
            slider.Height(g_metrics.rowHeight);
        }
        wuxc::Grid::SetColumn(slider, 1);

        SetIconLevel(icon, slider.Value());

        // Snap to what the hardware can actually represent. A monitor whose
        // VCP range is 0-50 has 2% granularity, so offering 1% steps just
        // means three slider positions that all round to the same raw value.
        double step = 1.0;
        if (d.transport == brightness::Transport::DdcCi && d.vcpMax > 0) {
            step = 100.0 / static_cast<double>(d.vcpMax);
        }
        if (step < 1.0) {
            step = 1.0;  // finer than the slider is worth showing
        }
        slider.StepFrequency(step);
        slider.SnapsTo(wuxc::Primitives::SliderSnapsTo::StepValues);

        std::wstring id = d.stableId;
        // auto_revoke so the handler can be detached on unload; a dangling
        // registration into an unloaded DLL is a crash, not a leak.
        injection.revokers.push_back(slider.ValueChanged(
            winrt::auto_revoke,
            [id, icon, title, displayName](
                wf::IInspectable const&,
                wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
                SetIconLevel(icon, args.NewValue());
                title.Text(FormatRowTitle(displayName,
                                          static_cast<int>(std::lround(args.NewValue()))));
                if (g_suppressValueChanged) {
                    // Echo of a refresh we just wrote into the slider; writing
                    // it back to the hardware would be a pointless round trip.
                    return;
                }
                if (g_engine) {
                    // Returns immediately; the worker coalesces and writes.
                    g_engine->SetPercent(id, static_cast<int>(std::lround(args.NewValue())));
                }
            }));

        sliderRow.Children().Append(slider);
        panel.Children().Append(sliderRow);

        injection.bindings.push_back({id, displayName, winrt::make_weak(slider),
                                      winrt::make_weak(icon),
                                      winrt::make_weak(title)});
    }

    if (panel.Children().Size() > 0) {
        panel.Visibility(wux::Visibility::Visible);
    }
}

wuxc::StackPanel BuildSliderPanel(Injection& injection) {
    wuxc::StackPanel panel;
    panel.Name(L"WindhawkPerMonitorBrightness");
    panel.Orientation(wuxc::Orientation::Vertical);
    if (UseNativeMetrics()) {
        // Left and right come from the native row so the icon column and the
        // far end of the track line up with it; the panel used to be 6px left
        // of the icons and 42px past the end of the sliders.
        //
        // The seam goes on whichever edge touches the group, and the other
        // edge gets a plain gap -- these rows carry titles, so they are a
        // group of their own rather than more of the same list.
        const bool above = g_panelPosition == PanelPosition::AboveSliders;
        panel.Margin(wux::ThicknessHelper::FromLengths(
            g_metrics.iconLeft, above ? 8 : g_metrics.gapBelow,
            g_metrics.groupWidth - g_metrics.sliderRight,
            above ? g_metrics.gapAbove : 8));
    } else {
        panel.Margin(
            wux::ThicknessHelper::FromLengths(kOwnMargin, 4, kOwnMargin, 8));
    }
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
    // to be read while the stock row is still visible.
    TryCaptureBrightnessSource(l1Grid);

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

    // Release the borrowed Lottie here, on the XAML thread, rather than letting
    // a global destructor drop it after the DLL is gone.
    g_brightnessSource = nullptr;
    g_animatedIconStatics = nullptr;
    g_capturedSource = false;
    g_progressMin = 0.0;
    g_progressMax = 1.0;

    for (Injection& injection : injections) {
        try {
            // Detach handlers before anything else, so nothing can fire at a
            // half-removed panel.
            injection.revokers.clear();

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
    injection.revokers.clear();
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
            auto slider = binding.slider.get();
            if (!slider) {
                continue;
            }
            for (const brightness::Display& d : displays) {
                if (d.stableId != binding.id || d.percent < 0) {
                    continue;
                }
                if (std::lround(slider.Value()) == d.percent) {
                    break;  // already correct, leave the thumb alone
                }
                // Scoped: if Value() throws, the outer catch would otherwise
                // swallow it with the flag stuck true, and from then on every
                // drag is silently ignored until the mod is reloaded.
                {
                    struct Suppress {
                        Suppress() { g_suppressValueChanged = true; }
                        ~Suppress() { g_suppressValueChanged = false; }
                    } guard;
                    slider.Value(d.percent);
                }
                if (auto icon = binding.icon.get()) {
                    SetIconLevel(icon, d.percent);
                }
                if (auto title = binding.title.get()) {
                    title.Text(FormatRowTitle(binding.name, d.percent));
                }
                break;
            }
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

void LoadSettings() {
    g_hideStockBrightness = Wh_GetIntSetting(L"hideStockBrightness") != 0;
    g_hideUnsupported = Wh_GetIntSetting(L"hideUnsupported") != 0;

    // No truthiness test: StringSetting converts through operator PCWSTR(),
    // and Wh_GetStringSetting returns L"" rather than NULL, so the check that
    // used to wrap this was always taken.
    g_panelPosition = PanelPosition::BelowSliders;
    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"panelPosition");
    std::wstring_view positionValue{position.get()};
    if (positionValue == L"aboveSliders") {
        g_panelPosition = PanelPosition::AboveSliders;
    } else if (positionValue == L"bottom") {
        g_panelPosition = PanelPosition::Bottom;
    }

    // Wh_GetStringSetting returns L"" rather than NULL on failure, so a
    // pointer check would be meaningless; the RAII wrapper also removes the
    // manual Wh_FreeStringSetting.
    // Off unless asked for: following writes to monitors that keep the value
    // in their own settings, and Windows cannot distinguish a brightness key
    // from power-plan or idle dimming.
    brightness::FollowMode followMode = brightness::FollowMode::Off;
    WindhawkUtils::StringSetting follow =
        WindhawkUtils::StringSetting::make(L"followInternalBrightness");
    if (wcscmp(follow.get(), L"relative") == 0) {
        followMode = brightness::FollowMode::Relative;
    } else if (wcscmp(follow.get(), L"match") == 0) {
        followMode = brightness::FollowMode::Match;
    }

    if (g_engine) {
        g_engine->SetFollowMode(followMode);
    }

    Wh_Log(L"hideStockBrightness=%d followInternalBrightness=%d "
           L"panelPosition=%s hideUnsupported=%d",
           g_hideStockBrightness ? 1 : 0, static_cast<int>(followMode),
           positionValue.empty() ? L"belowSliders" : position.get(),
           g_hideUnsupported ? 1 : 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_engine = new brightness::Engine();
    g_engine->SetLogger(&EngineLog);
    g_engine->SetOnChanged(&OnEngineChanged);

    LoadSettings();

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

    const bool previouslyHidden = g_hideStockBrightness;
    const bool previouslyHidUnsupported = g_hideUnsupported;
    const PanelPosition previousPosition = g_panelPosition;
    LoadSettings();

    // Follow mode is engine state and takes effect at once.
    //
    // The other three changed what was put into somebody else's visual tree,
    // and each needs that undone and redone rather than patched: hiding the
    // stock slider touches an element we do not own, the position moved the
    // shell's own children between rows, and hiding unsupported displays
    // changes which rows exist at all.
    *bReload = (g_hideStockBrightness != previouslyHidden) ||
               (g_hideUnsupported != previouslyHidUnsupported) ||
               (g_panelPosition != previousPosition);
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
