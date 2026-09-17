// ==WindhawkMod==
// @id              per-monitor-brightness
// @name            Per-monitor brightness in Quick Settings
// @description     Adds a titled brightness slider for every connected monitor to the Windows 11 Quick Settings panel
// @version         1.6
// @author          bardelyne
// @github          https://github.com/bardelyne
// @include         ShellHost.exe
// @architecture    x86-64
// @license         GPL-3.0
// @compilerOptions -ldxva2 -lole32 -loleaut32 -lwbemuuid -luuid -lruntimeobject -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The XAML-diagnostics plumbing (VisualTreeWatcher / WindhawkTAP /
// InjectWindhawkTAP) is adapted from m417z's "Windows 11 Notification Center
// Styler", which is GPLv3; this mod is GPLv3 for that reason.

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

## Settings

- **Hide the built-in brightness slider** -- on by default; the stock slider
  only controls the internal panel, which already has its own row here.
- **Laptop brightness keys control every monitor** -- `relative` (default)
  shifts other monitors by the same amount, preserving their offset; `match`
  sets them all to the same percentage; `off` leaves them alone.
- **Verbose logging** -- logs every brightness write. Useful when diagnosing a
  monitor that will not respond, noisy otherwise.

## Notes and limitations

- A DDC/CI write takes roughly 50-60 ms, and the bus saturates while dragging.
  All hardware access happens on a background thread and repeated values
  collapse into a single write, so dragging never stalls the shell -- but an
  external monitor will visibly step rather than fade. The internal panel is
  around ten times faster and looks smooth.
- DDC/CI has no notification channel: a monitor only ever answers what the host
  asks it. Brightness changed using the monitor's own buttons therefore cannot
  be detected, and only shows up the next time the panel is opened.
- Not every monitor implements DDC/CI correctly. If a display does not respond,
  enable verbose logging and check whether its writes report `ok=0`.
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
- followInternalBrightness: relative
  $name: Laptop brightness keys control every monitor
  $description: >-
    Function keys only reach the built-in panel -- that is a hardware limit, not
    a Windows one. This mirrors those keypresses onto external monitors over
    DDC/CI, so one keypress dims everything.
  $options:
  - relative: Shift other monitors by the same amount (keeps their offset)
  - match: Set other monitors to the same percentage
  - "off": Leave other monitors alone
- debugLogging: false
  $name: Verbose logging
  $description: >-
    Log every brightness write. Useful when diagnosing a monitor that does not
    respond; noisy otherwise.
*/
// ==/WindhawkModSettings==

#include <initguid.h>  // must precede xamlom.h

#include <inspectable.h>
#include <xamlom.h>

// winbase.h defines GetCurrentTime as a macro, which collides with
// Windows.UI.Xaml.Media.Animation's method of the same name.
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
// Not just the .0.h forward declarations: Append/Size have deduced return
// types and must be defined before use.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#pragma pop_macro("GetCurrentTime")

#include <roapi.h>

#include <atomic>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <mutex>
#include <set>
#include <string>
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

#include <highlevelmonitorconfigurationapi.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>

#include <comdef.h>
#include <wbemidl.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cwctype>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace brightness {

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
    std::wstring gdiDeviceName;  // "\\.\DISPLAY5"
    RECT rect{};
    bool isPrimary = false;
    Transport transport = Transport::None;
    int percent = -1;  // last known, -1 if never read

    // DdcCi
    HANDLE hPhysical = nullptr;
    DWORD vcpMax = 100;  // raw scale; a Samsung G32 reports 50, not 100

    // Wmi
    std::wstring wmiPath;  // __RELPATH of the WmiMonitorBrightnessMethods instance
};

namespace detail {

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
        for (LONG i = lb; i <= ub; ++i) {
            LONG ch = 0;
            if (elem == VT_UI1) {
                BYTE b = 0;
                if (FAILED(SafeArrayGetElement(sa, &i, &b))) {
                    break;
                }
                ch = b;
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

    bool Ok() const { return services_ != nullptr; }

    IWbemServices* Services() const { return services_; }

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
        for (;;) {
            IWbemClassObject* obj = nullptr;
            ULONG got = 0;
            if (e->Next(WBEM_INFINITE, 1, &obj, &got) != S_OK || got != 1) {
                break;
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
    void SetVerboseWrites(bool verbose) { verboseWrites_.store(verbose); }

    void SetFollowMode(FollowMode mode) {
        std::lock_guard<std::mutex> lock(mutex_);
        followMode_ = mode;
    }

    void Start() {
        if (worker_.joinable()) {
            return;
        }
        quit_ = false;
        worker_ = std::thread([this] { WorkerMain(); });

        // Block only for the first enumeration, so the caller can render the
        // UI immediately. Costs one DDC round trip per external display.
        {
            std::unique_lock<std::mutex> lock(mutex_);
            ready_.wait(lock, [this] { return enumerated_ || quit_; });
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
            eventThread_ = std::thread([this] { EventThreadMain(); });
        }
    }

    void Stop() {
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
    void SetPercent(const std::wstring& stableId, int percent) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
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

    // Diagnostics for the standalone harness.
    unsigned Writes() const { return writes_.load(); }
    void ResetWrites() { writes_.store(0); }

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

        // Harmless if the host process already did this (RPC_E_TOO_LATE).
        CoInitializeSecurity(nullptr, -1, nullptr, nullptr,
                             RPC_C_AUTHN_LEVEL_DEFAULT,
                             RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE,
                             nullptr);

        // Created here, not as a plain member: these interface pointers belong
        // to this thread's apartment and must not outlive it. Releasing them
        // from ~Engine() on another thread, after the CoUninitialize() below,
        // is a crash.
        wmi_ = std::make_unique<WmiSession>();
        wmi_->Init();

        Rescan();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            enumerated_ = true;
        }
        ready_.notify_all();

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
            for (const auto& entry : batch) {
                Apply(entry.first, entry.second);
            }

            if (doRefresh) {
                RefreshValues();
                NotifyChanged(false);
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
        if (fn) {
            fn(structural);
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
                        if (next == WBEM_S_TIMEDOUT || got != 1 || !obj) {
                            continue;
                        }
                        if (next != S_OK) {
                            break;
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
                    int base = (d.percent < 0) ? percent : d.percent;
                    int want = (followMode_ == FollowMode::Match)
                                   ? percent
                                   : std::clamp(base + delta, 0, 100);
                    if (want != d.percent) {
                        follow.emplace_back(d.stableId, want);
                    }
                }
            }
        }

        Log(L"brightness event: %ls is now %d%%", instanceName.c_str(), percent);

        // Outside the lock: SetPercent takes it.
        for (const auto& entry : follow) {
            SetPercent(entry.first, entry.second);
        }

        NotifyChanged(false);
    }

    void ReleasePhysicalMonitors() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& d : displays_) {
            if (d.hPhysical) {
                PHYSICAL_MONITOR pm{};
                pm.hPhysicalMonitor = d.hPhysical;
                DestroyPhysicalMonitors(1, &pm);
                d.hPhysical = nullptr;
            }
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

    void Rescan() {
        ReleasePhysicalMonitors();

        std::vector<HMONITOR> handles;
        EnumDisplayMonitors(nullptr, nullptr, &Engine::EnumProc,
                            reinterpret_cast<LPARAM>(&handles));

        std::map<std::wstring, WmiFacts> facts = CollectWmiFacts();
        std::vector<Display> found;

        for (HMONITOR h : handles) {
            MONITORINFOEXW mi{};
            mi.cbSize = sizeof(mi);
            if (!GetMonitorInfoW(h, &mi)) {
                continue;
            }

            Display d;
            d.gdiDeviceName = mi.szDevice;
            d.rect = mi.rcMonitor;
            d.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;

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
            if (TryAttachDdcCi(h, &d)) {
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

        {
            std::lock_guard<std::mutex> lock(mutex_);
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

    void Apply(const std::wstring& stableId, int percent) {
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
        writes_.fetch_add(1);
        if (verboseWrites_.load()) {
            Log(L"apply %ls -> %d%% ok=%d (%lld ms)", stableId.c_str(), percent,
                ok ? 1 : 0, ms);
        }
    }

    std::thread worker_;
    std::thread eventThread_;
    std::map<std::wstring, std::chrono::steady_clock::time_point> lastRequest_;
    FollowMode followMode_ = FollowMode::Off;
    std::atomic<bool> verboseWrites_{false};
    std::mutex mutex_;
    std::condition_variable work_;
    std::condition_variable ready_;
    std::map<std::wstring, int> pending_;
    std::vector<Display> displays_;
    std::unique_ptr<WmiSession> wmi_;
    LogFn log_ = nullptr;
    std::atomic<unsigned> writes_{0};
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

// Guards against double-injection: the visual tree reports the Control Center
// being built every time it opens, and it is rebuilt on each open.
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

    // Fires when the flyout is shown or hidden, which is when values are worth
    // re-reading.
    wux::XamlRoot::Changed_revoker xamlRootRevoker;

};

bool g_hideStockBrightness = true;

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

StockSliderState g_stockSlider;
bool g_loggedHideMiss = false;

// Set while pushing refreshed values into sliders, so their ValueChanged
// handlers can tell our own writes apart from the user's. UI thread only.
bool g_suppressValueChanged = false;

// The shell's own brightness Lottie, borrowed off its AnimatedIcon so our
// sliders can show the real animated sun rather than an imitation. WinUI2
// exposes no brightness visual source publicly, so lifting the live one is the
// only way to get it.
winrt::com_ptr<::IInspectable> g_brightnessSource;
winrt::com_ptr<IAnimatedIconStaticsAbi> g_animatedIconStatics;
bool g_capturedSource = false;

// The shell's Lottie carries two markers, Brightness_at_0 and
// Brightness_at_100, sitting at progress 0 and 1. So it is progress-driven
// rather than a state machine, and the usable range comes from those markers.
double g_progressMin = 0.0;
double g_progressMax = 1.0;

std::mutex g_injectionsMutex;
std::vector<Injection> g_injections;

// The XAML thread to marshal teardown onto. Captured at injection time.
winrt::Windows::UI::Core::CoreDispatcher g_dispatcher{nullptr};

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
                    element.Width(20);
                    element.Height(20);
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
    return font;
}

void SetIconLevel(wux::FrameworkElement const& icon, double percent) {
    double t = std::clamp(percent, 0.0, 100.0) / 100.0;

    if (auto font = icon.try_as<wuxc::FontIcon>()) {
        font.Opacity(0.40 + 0.60 * t);
        font.FontSize(14.0 + 4.0 * t);
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

    for (const brightness::Display& d : displays) {
        std::wstring displayName = d.name;
        wuxc::TextBlock title;
        title.Text(FormatRowTitle(displayName, d.percent));
        title.FontSize(12);
        title.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
        title.Opacity(0.85);
        panel.Children().Append(title);

        if (d.transport == brightness::Transport::None) {
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
        icon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 12, 0));
        wuxc::Grid::SetColumn(icon, 0);
        sliderRow.Children().Append(icon);

        wuxc::Slider slider;
        slider.Minimum(0);
        slider.Maximum(100);
        slider.Value(d.percent < 0 ? 50 : d.percent);
        slider.IsThumbToolTipEnabled(true);
        slider.VerticalAlignment(wux::VerticalAlignment::Center);
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
                                          static_cast<int>(args.NewValue())));
                if (g_suppressValueChanged) {
                    // Echo of a refresh we just wrote into the slider; writing
                    // it back to the hardware would be a pointless round trip.
                    return;
                }
                if (g_engine) {
                    // Returns immediately; the worker coalesces and writes.
                    g_engine->SetPercent(id, static_cast<int>(args.NewValue()));
                }
            }));

        sliderRow.Children().Append(slider);
        panel.Children().Append(sliderRow);

        injection.bindings.push_back({id, displayName, winrt::make_weak(slider),
                                      winrt::make_weak(icon),
                                      winrt::make_weak(title)});
    }
}

wuxc::StackPanel BuildSliderPanel(Injection& injection) {
    wuxc::StackPanel panel;
    panel.Name(L"WindhawkPerMonitorBrightness");
    panel.Orientation(wuxc::Orientation::Vertical);
    panel.Margin(wux::ThicknessHelper::FromLengths(16, 4, 16, 8));
    PopulateSliderPanel(panel, injection);
    return panel;
}

// Collapses the stock brightness row, which duplicates the internal-panel
// slider this mod already provides. The sliders live in a virtualized GridView,
// so rather than guess at an index we find the row by the animated sun icon it
// carries -- the volume row has no such element -- and walk up to its item.
bool TryHideStockBrightness(wux::FrameworkElement const& l1Grid) {
    if (g_stockSlider.hidden) {
        return true;
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

    auto revoker = std::make_shared<wux::FrameworkElement::LayoutUpdated_revoker>();
    *revoker = l1Grid.LayoutUpdated(
        winrt::auto_revoke,
        [l1Grid, revoker](wf::IInspectable const&, wf::IInspectable const&) {
            if (TryHideStockBrightness(l1Grid)) {
                revoker->revoke();
            }
        });
}

// The Control Center's L1Grid is a Grid whose row layout is not documented and
// has changed across builds. Appending a row is only safe when it already
// declares RowDefinitions; otherwise every existing child implicitly lives in
// row 0 and adding a definition would re-flow the whole panel. In that case we
// log the structure and leave the UI untouched.
bool InjectInto(wux::FrameworkElement const& l1Grid) {
    auto grid = l1Grid.try_as<wuxc::Grid>();
    if (!grid) {
        Wh_Log(L"L1Grid is not a Grid (%s) -- not injecting",
               ElementLabel(l1Grid).c_str());
        return false;
    }

    if (FindDescendant(grid, L"Windows.UI.Xaml.Controls.StackPanel#WindhawkPerMonitorBrightness", 6)) {
        Wh_Log(L"Panel already present, skipping");
        return true;
    }

    uint32_t rowCount = grid.RowDefinitions().Size();
    Wh_Log(L"L1Grid has %u RowDefinition(s), %u child(ren)", rowCount,
           grid.Children().Size());

    if (rowCount == 0) {
        // Either the grid genuinely has no rows (in which case appending one
        // would re-flow every existing child out of row 0), or it is not built
        // yet and a later retry will succeed.
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

    Injection injection;
    wuxc::StackPanel panel = BuildSliderPanel(injection);

    wuxc::RowDefinition row;
    row.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    grid.RowDefinitions().Append(row);

    wuxc::Grid::SetRow(panel, static_cast<int>(grid.RowDefinitions().Size()) - 1);
    grid.Children().Append(panel);

    injection.grid = winrt::make_weak(grid);
    injection.panel = winrt::make_weak(panel);
    injection.row = row;

    if (g_hideStockBrightness) {
        ArmStockSliderHide(l1Grid);
    }

    {
        std::lock_guard<std::mutex> lock(g_injectionsMutex);
        // The Control Center is rebuilt on every open, so prune the entries
        // whose tree has already been torn down by the shell.
        std::erase_if(g_injections, [](const Injection& i) {
            return !i.grid.get() || !i.panel.get();
        });
        g_injections.push_back(std::move(injection));
    }

    // Teardown has to happen on this thread; remember how to get back to it.
    g_dispatcher = grid.Dispatcher();

    // Secondary signal only. XamlRoot.Changed looked like the right way to
    // hear about the flyout being shown, but it does not fire for this host on
    // 26200 -- the WinEvent hook in ShellEventWatcher is what actually works.
    // Kept because it costs nothing and may fire on other builds; both paths
    // just ask the engine to refresh, which is idempotent.
    try {
        if (auto xamlRoot = grid.XamlRoot()) {
            injection.xamlRootRevoker = xamlRoot.Changed(
                winrt::auto_revoke,
                [](wux::XamlRoot const& sender,
                   wux::XamlRootChangedEventArgs const&) {
                    if (sender.IsHostVisible() && g_engine) {
                        g_engine->RequestRefresh();
                    }
                });
        } else {
            Wh_Log(L"No XamlRoot; values will not refresh on open");
        }
    } catch (...) {
        Wh_Log(L"XamlRoot subscription failed: %08X", winrt::to_hresult());
    }

    Wh_Log(L"Injected per-monitor brightness panel into row %u", rowCount);
    return true;
}

// Undoes every injection. Must run on the XAML thread.
void RemoveInjections() {
    std::vector<Injection> injections;
    {
        std::lock_guard<std::mutex> lock(g_injectionsMutex);
        injections.swap(g_injections);
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

void PostToUiThread(std::function<void()> fn) {
    if (!g_dispatcher) {
        return;
    }
    auto shared = std::make_shared<std::function<void()>>(std::move(fn));
    g_dispatcher.RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
        [shared]() { (*shared)(); });
}

// Pushes freshly read hardware values into the existing sliders. XAML thread.
void ApplyRefreshedValues() {
    if (!g_engine) {
        return;
    }
    std::vector<brightness::Display> displays = g_engine->GetDisplays();

    std::lock_guard<std::mutex> lock(g_injectionsMutex);
    for (Injection& injection : g_injections) {
        for (Injection::Binding& binding : injection.bindings) {
            auto slider = binding.slider.get();
            if (!slider) {
                continue;
            }
            for (const brightness::Display& d : displays) {
                if (d.stableId != binding.id || d.percent < 0) {
                    continue;
                }
                if (std::abs(slider.Value() - d.percent) < 0.5) {
                    break;  // already correct, leave the thumb alone
                }
                g_suppressValueChanged = true;
                slider.Value(d.percent);
                g_suppressValueChanged = false;
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
}

// A monitor appeared or vanished, so the rows themselves are wrong. XAML
// thread. The panel and its grid row are kept; only the contents are redone.
void RebuildInjectedPanels() {
    std::lock_guard<std::mutex> lock(g_injectionsMutex);
    size_t rebuilt = 0;
    for (Injection& injection : g_injections) {
        auto panel = injection.panel.get();
        if (!panel) {
            continue;
        }
        // Detach before discarding the controls the handlers point at.
        injection.revokers.clear();
        injection.bindings.clear();
        panel.Children().Clear();
        PopulateSliderPanel(panel, injection);
        ++rebuilt;
    }
    Wh_Log(L"Rebuilt %zu panel(s) after a display change", rebuilt);
}

void OnEngineChanged(bool structural) {
    // Called on the engine worker; XAML must be touched on its own thread.
    PostToUiThread([structural]() {
        if (structural) {
            RebuildInjectedPanels();
        } else {
            ApplyRefreshedValues();
        }
    });
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
        if (thread_.joinable()) {
            if (hwnd_) {
                PostMessage(hwnd_, WM_CLOSE, 0, 0);
            }
            thread_.join();
        }
        if (ready_) {
            CloseHandle(ready_);
            ready_ = nullptr;
        }
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
    HWINEVENTHOOK hook_ = nullptr;
};

ShellEventWatcher g_shellWatcher;

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

// The Control Center is built once at shell start and merely shown and hidden
// afterwards, so by the time the view is reported its tree is normally already
// complete -- inject straight away. Waiting on a layout pass instead meant
// waiting until the panel was next *closed*, which is the first layout it runs.
// Loaded/LayoutUpdated remain as retries for the case where it really is not
// ready yet; both are revoked as soon as one succeeds.
void AttachInjector(wux::FrameworkElement const& view) {
    if (TryInject(view)) {
        return;
    }

    Wh_Log(L"Tree not ready, arming retries");

    auto loaded = std::make_shared<wux::FrameworkElement::Loaded_revoker>();
    auto layout = std::make_shared<wux::FrameworkElement::LayoutUpdated_revoker>();

    *loaded = view.Loaded(
        winrt::auto_revoke,
        [view, loaded, layout](wf::IInspectable const&,
                               wux::RoutedEventArgs const&) {
            if (TryInject(view)) {
                loaded->revoke();
                layout->revoke();
            }
        });

    *layout = view.LayoutUpdated(
        winrt::auto_revoke,
        [view, loaded, layout](wf::IInspectable const&,
                               wf::IInspectable const&) {
            if (TryInject(view)) {
                loaded->revoke();
                layout->revoke();
            }
        });
}

}  // namespace

// ===========================================================================
// XAML diagnostics plumbing (adapted from m417z's Notification Center Styler)
// ===========================================================================

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_XamlDiagnostics(site.as<IXamlDiagnostics>()) {
        Wh_Log(L"Constructing VisualTreeWatcher");

        // Calling AdviseVisualTreeChange on this thread can hang the app in
        // Advising::RunOnUIThread; doing it from a fresh thread avoids that.
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID param) -> DWORD {
                auto* watcher = reinterpret_cast<VisualTreeWatcher*>(param);
                auto service = watcher->m_XamlDiagnostics.as<IVisualTreeService3>();
                HRESULT hr = service->AdviseVisualTreeChange(watcher);
                watcher->Release();
                if (FAILED(hr)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: %08X", hr);
                }
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    void UnadviseVisualTreeChange() {
        HRESULT hr =
            m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
        if (FAILED(hr)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: %08X", hr);
        }
    }

   private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation,
                                                 VisualElement element,
                                                 VisualMutationType mutationType) override try {
        if (mutationType != Add || !element.Type) {
            return S_OK;
        }

        // The Control Center view is the anchor: it is created fresh each time
        // the panel opens, which is exactly when we want to (re)inject.
        if (wcscmp(element.Type, L"ControlCenter.ControlCenterView") != 0) {
            return S_OK;
        }

        Wh_Log(L"ControlCenterView added, attempting injection");

        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(
            element.Handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));

        if (auto fe = obj.try_as<wux::FrameworkElement>()) {
            AttachInjector(fe);
        }

        return S_OK;
    } catch (...) {
        Wh_Log(L"OnVisualTreeChange error: %08X", winrt::to_hresult());
        return S_OK;  // never fail the shell's callback
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                    VisualElementState,
                                                    LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

namespace {
winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;
}

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = {
    0xc85d8cc7, 0x5463, 0x40e8, {0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5}};

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite,
                                             winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override try {
        if (g_visualTreeWatcher) {
            g_visualTreeWatcher->UnadviseVisualTreeChange();
            g_visualTreeWatcher = nullptr;
        }

        site.copy_from(pUnkSite);

        if (site) {
            // Balance the refcount taken by InitializeXamlDiagnosticsEx.
            FreeLibrary(GetCurrentModuleHandle());
            g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
        }

        return S_OK;
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"SetSite error: %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override {
        return site.as(riid, ppvSite);
    }

   private:
    winrt::com_ptr<IUnknown> site;
};

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid,
                                             void** ppvObject) override try {
        if (pUnkOuter) {
            return CLASS_E_NOAGGREGATION;
        }
        *ppvObject = nullptr;
        return winrt::make<T>().as(riid, ppvObject);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) _Use_decl_annotations_ STDAPI
    DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try {
    if (rclsid == CLSID_WindhawkTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) _Use_decl_annotations_ STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

static HRESULT InjectWindhawkTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wuxDll =
        LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wuxDll) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wuxDll, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // There is no way to know which diagnostics slot is free; try until one
    // takes. Same approach the XAML framework itself uses.
    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), nullptr, location,
                  CLSID_WindhawkTAP, nullptr);
        if (SUCCEEDED(hr)) {
            break;
        }
    }

    return hr;
}

// ===========================================================================
// Windhawk lifecycle
// ===========================================================================

void LoadSettings() {
    g_hideStockBrightness = Wh_GetIntSetting(L"hideStockBrightness") != 0;

    brightness::FollowMode followMode = brightness::FollowMode::Relative;
    if (PCWSTR setting = Wh_GetStringSetting(L"followInternalBrightness")) {
        if (wcscmp(setting, L"off") == 0) {
            followMode = brightness::FollowMode::Off;
        } else if (wcscmp(setting, L"match") == 0) {
            followMode = brightness::FollowMode::Match;
        }
        Wh_FreeStringSetting(setting);
    }

    if (g_engine) {
        g_engine->SetVerboseWrites(Wh_GetIntSetting(L"debugLogging") != 0);
        g_engine->SetFollowMode(followMode);
    }

    Wh_Log(L"hideStockBrightness=%d followInternalBrightness=%d",
           g_hideStockBrightness ? 1 : 0, static_cast<int>(followMode));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_engine = new brightness::Engine();
    g_engine->SetLogger(&EngineLog);
    g_engine->SetOnChanged(&OnEngineChanged);

    LoadSettings();

    g_engine->Start();

    for (const brightness::Display& d : g_engine->GetDisplays()) {
        Wh_Log(L"display: %s  transport=%d  now=%d%%  vcpMax=%lu  id=%s",
               d.name.c_str(), static_cast<int>(d.transport), d.percent,
               static_cast<unsigned long>(d.vcpMax), d.stableId.c_str());
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"InjectWindhawkTAP failed: %08X", hr);
    }

    g_shellWatcher.Start();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool previouslyHidden = g_hideStockBrightness;
    LoadSettings();

    // Follow mode and verbose logging are engine state and take effect at once.
    // Hiding the stock slider changes what was injected into somebody else's
    // visual tree, so only that one needs a reload to rebuild it.
    *bReload = (g_hideStockBrightness != previouslyHidden);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    // Silence everything that could still post work to the XAML thread before
    // we start dismantling what that work would touch.
    g_shellWatcher.Stop();
    if (g_engine) {
        g_engine->SetOnChanged(nullptr);
    }

    // Put the shell's visual tree back the way we found it, before this DLL is
    // unloaded out from under the handlers we registered. XAML objects may only
    // be touched on their own thread, and we must not return until it is done.
    if (g_dispatcher) {
        try {
            if (g_dispatcher.HasThreadAccess()) {
                RemoveInjections();
            } else {
                HANDLE done = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                if (done) {
                    g_dispatcher.RunAsync(
                        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                        [done]() {
                            RemoveInjections();
                            SetEvent(done);
                        });

                    // Bounded, so a wedged XAML thread cannot hang the shell.
                    if (WaitForSingleObject(done, 5000) == WAIT_OBJECT_0) {
                        CloseHandle(done);
                    } else {
                        // Deliberately leak the handle: the queued callback may
                        // still run and signal it, and closing it here would
                        // turn a timeout into a crash.
                        Wh_Log(L"TIMED OUT waiting for UI teardown; "
                               L"leftover sliders may remain");
                    }
                }
            }
            // A callback posted just before SetOnChanged(nullptr) may still be
            // queued, and its code lives in this DLL. A no-op at Low priority
            // only runs once everything queued ahead of it has, so waiting on
            // it drains the queue.
            if (!g_dispatcher.HasThreadAccess()) {
                HANDLE drained = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                if (drained) {
                    g_dispatcher.RunAsync(
                        winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                        [drained]() { SetEvent(drained); });
                    if (WaitForSingleObject(drained, 5000) == WAIT_OBJECT_0) {
                        CloseHandle(drained);
                    } else {
                        Wh_Log(L"TIMED OUT draining the dispatcher");
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"Teardown error: %08X", winrt::to_hresult());
        }
        g_dispatcher = nullptr;
    }

    if (g_engine) {
        // Joins the worker, which releases its COM interfaces inside its own
        // apartment before CoUninitialize.
        g_engine->Stop();
        delete g_engine;
        g_engine = nullptr;
    }
}
